/**
 *  @file       
 *
 *  @brief      Low level input that handles inputs from multiple pins on the micro.
 *
 *  @details    Currently the module only supports Digital, Analog and Feedback type low level input types.
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"
#include "MultiInputLLI.h" 


#if (HBL_LLI_NUM_MULTIINPUT > 0)
#include <string.h>

#if (HBL_LLI_NUM_ANALOG >  0)
    #include "AnalogLLI.h"
#endif

#if (HBL_LLI_NUM_DIGITAL > 0)
    #include "DigitalLLI.h"
#endif

#if (HBL_LLI_NUM_FEEDBACK > 0)
    #include "FeedbackLLI.h"
#endif

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#if (HBL_LLI_NUM_ANALOG >  0)
    #define MULTIINPUT_REGISTERED_LLI_ANALOG 1
#else
    #define MULTIINPUT_REGISTERED_LLI_ANALOG 0
#endif

#if (HBL_LLI_NUM_DIGITAL > 0)
    #define MULTIINPUT_REGISTERED_LLI_DIGITAL 1
#else
    #define MULTIINPUT_REGISTERED_LLI_ANALOG 0
#endif

#if (HBL_LLI_NUM_FEEDBACK > 0)
    #define MULTIINPUT_REGISTERED_LLI_FEEDBACK 1
#else
    #define MULTIINPUT_REGISTERED_LLI_FEEDBACK 0
#endif

//! Defines the number of LLI register definitions that will be supported based on the supported LLIs that are available in the system
#define MULTIINPUT_NUM_REGISTERED_INPUTS (MULTIINPUT_REGISTERED_LLI_ANALOG + MULTIINPUT_REGISTERED_LLI_DIGITAL + MULTIINPUT_REGISTERED_LLI_FEEDBACK)

//! Maximum number of inputs that can be defined for a MultiInput LLI
#define MULTIINPUTLLI_MAX_INPUTS   4

//! Definition of parameters taken from GESE used for a MultiInput LLI
typedef struct
{
    HBL_LLI_ENUM_TYPE LLI_ID;
    uint8 LLI_Params_Offset;
}MULTIINPUTLLI_PARAMETERS_TYPE;

//! Definition of module data used to process a MultiInput LLI
PACKED typedef struct
{
    HBL_LLI_TYPE LLI_Data[MULTIINPUTLLI_MAX_INPUTS];
    uint8 Num_Of_Inputs;
}MULTIINPUTLLI_CONFIG_TYPE;

//! Counts the number of MultiInput LLIs that are allocated for use. Used to know when the limitation in the firmware has been reached.
static uint8 MultiInputLLI_Allocation;

//! Module data used to process a MultiInput LLI
static MULTIINPUTLLI_CONFIG_TYPE MultiInputLLI_Config[HBL_LLI_NUM_MULTIINPUT];

//! Stores the data related to each input in each MultiInput LLI
static MULTIINPUTLLI_DATA_TYPE MultiInputLLI_Data[HBL_LLI_NUM_MULTIINPUT][MULTIINPUTLLI_MAX_INPUTS];

//! Definition of the supported LLI register definitions
static const HBL_LLI_HANDLER_HOOK_TYPE MULTIINPUT_LLI_HANDLERS[MULTIINPUT_NUM_REGISTERED_INPUTS]  = {
                                                                                                        #if (HBL_LLI_NUM_DIGITAL > 0)
                                                                                                            {DIGITAL_LLI_REGISTER,  HBL_LLI_DIGITAL}
                                                                                                        #endif

                                                                                                        #if (HBL_LLI_NUM_ANALOG > 0)
                                                                                                            ,{ANALOG_LLI_REGISTER,   HBL_LLI_ANALOG}
                                                                                                        #endif

                                                                                                        #if (HBL_LLI_NUM_FEEDBACK > 0)
                                                                                                            ,{FEEDBACK_LLI_REGISTER, HBL_LLI_FEEDBACK}
                                                                                                        #endif
                                                                                                     };
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief	Initializes the module MultiInputLLI and its variables
 */
void MultiInputLLI__Initialize(void)
{
    MultiInputLLI_Allocation = 0;
    memset(&MultiInputLLI_Config, 0, sizeof(MultiInputLLI_Config));
    memset(&MultiInputLLI_Data, 0, sizeof(MultiInputLLI_Data));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Determines the destination (index) of the data for the LLI.
 * @return  Index to a local buffer where the data will reside.
 */
uint8 MultiInputLLI__Allocate(void)
{
    uint8 retval;
    retval = 0xFF;

    if (MultiInputLLI_Allocation < HBL_LLI_NUM_MULTIINPUT)
    {
        retval = MultiInputLLI_Allocation;
        MultiInputLLI_Allocation++;
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Initialize an instance of the MultiInput LLI by extracting parameter data and initializing variables.
 * @param   input: data related to the instance of the LLI
 * @param   status: see definition of HBL_LLI_STATUS
 */
PASS_FAIL_TYPE MultiInputLLI__InitializeInstance(HBL_LLI_TYPE * input)
{
    PASS_FAIL_TYPE retval;
    MULTIINPUTLLI_PARAMETERS_TYPE * parameters;
    uint8 index;
    uint8 handler_index;

    retval = FAIL;

    if (input->Data_Index < MultiInputLLI_Allocation)
    {
    	// Adding 2 to skip the header for MultiInputLLI, as the parameters would have to apply to all the involved LLIs
        parameters = (MULTIINPUTLLI_PARAMETERS_TYPE *)(input->Parameter_Ptr.Data + 2);        //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]

        if (input->LLI_ID == HBL_LLI_MULTIINPUT_2)
        {
            MultiInputLLI_Config[input->Data_Index].Num_Of_Inputs = 2;
        }
        else if (input->LLI_ID == HBL_LLI_MULTIINPUT_3)
        {
            MultiInputLLI_Config[input->Data_Index].Num_Of_Inputs = 3;
        }
        else if (input->LLI_ID == HBL_LLI_MULTIINPUT_4)
        {
            MultiInputLLI_Config[input->Data_Index].Num_Of_Inputs = 4;
        }

        for (index = 0; index < MultiInputLLI_Config[input->Data_Index].Num_Of_Inputs; index++)
        {
            MultiInputLLI_Config[input->Data_Index].LLI_Data[index].LLI_Handler_Index = HBL_INDEX_INVALID;
            MultiInputLLI_Config[input->Data_Index].LLI_Data[index].Data_Index = HBL_INDEX_INVALID;

            MultiInputLLI_Config[input->Data_Index].LLI_Data[index].LLI_ID = parameters[index].LLI_ID;
            MultiInputLLI_Config[input->Data_Index].LLI_Data[index].Pin_Ptr = &(input->Pin_Ptr[index]);

            if (parameters[index].LLI_Params_Offset != 0xFF)   // Check for LLI with no parameters
            {
            	//NOTE: A board that is both an ACU and HMI cannot support the MultiInputLLI if it uses an LLI with parameters
#if (SYSTEM_ACU_HANDLING == ENABLED || SYSTEM_ACU_EXPANSION_HANDLING == ENABLED)
                SettingFile__BasicLoader(SF_PTR_ACU_IO_DEVICE, parameters[index].LLI_Params_Offset, &MultiInputLLI_Config[input->Data_Index].LLI_Data[index].Parameter_Ptr);
#elif (SYSTEM_HMI_HANDLING == ENABLED || SYSTEM_HMI_EXPANSION_HANDLING == ENABLED)
                SettingFile__BasicLoader(SF_PTR_UI_IO_DEVICE, parameters[index].LLI_Params_Offset, &MultiInputLLI_Config[input->Data_Index].LLI_Data[index].Parameter_Ptr);
#endif
            }
            else
            {
                MultiInputLLI_Config[input->Data_Index].LLI_Data[index].Parameter_Ptr.Data = NULL;
                MultiInputLLI_Config[input->Data_Index].LLI_Data[index].Parameter_Ptr.Length = 0;
            }

            MultiInputLLI_Config[input->Data_Index].LLI_Data[index].LLI_Mux = 0;            // Mux not allowed for MultiInput LLI
            MultiInputLLI_Config[input->Data_Index].LLI_Data[index].LLI_Position = 0x7F;    // Don't care about LLI position for MultiInput LLI

            retval = PASS;

            for (handler_index = 0; handler_index < MULTIINPUT_NUM_REGISTERED_INPUTS; handler_index++)
            {
                if (MultiInputLLI_Config[input->Data_Index].LLI_Data[index].LLI_ID == MULTIINPUT_LLI_HANDLERS[handler_index].LLI_ID)
                {
                    MultiInputLLI_Config[input->Data_Index].LLI_Data[index].LLI_Handler_Index = handler_index;
                    MultiInputLLI_Config[input->Data_Index].LLI_Data[index].Data_Index = MULTIINPUT_LLI_HANDLERS[handler_index].Allocate();
                    retval &= MULTIINPUT_LLI_HANDLERS[handler_index].InitializeInstance(&MultiInputLLI_Config[input->Data_Index].LLI_Data[index]);
                    break;
                }
            }
        }
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Retrieves the current value of each input assigned to a MultiInput LLI based on the input type.
 * @param   input: data related to the instance of the LLI
 * @param   status: see definition of HBL_LLI_STATUS
 * @return  The current value of each input assigned to a MultiInput LLI based on the input type.
 */
void * MultiInputLLI__Get(HBL_LLI_TYPE * input)
{
    uint8 handler_index;
    uint8 index;
    BOOL_TYPE * digital_lli_data_ptr;
    uint16 * analog_lli_data_ptr;
    uint8 * feedback_lli_data_ptr;

    if (input->Data_Index < MultiInputLLI_Allocation)
    {
        // Iterate through all the inputs of the Multi Input LLI to get the data for each input
        for (index = 0; index < MultiInputLLI_Config[input->Data_Index].Num_Of_Inputs; index++)
        {
            // Iterate through all the registered Multi Input LLI handlers
            for (handler_index = 0; handler_index < MULTIINPUT_NUM_REGISTERED_INPUTS; handler_index++)
            {
                // Look for a match on the LLI ID for the individual input of the Multi Input LLI
                if (MultiInputLLI_Config[input->Data_Index].LLI_Data[index].LLI_ID == MULTIINPUT_LLI_HANDLERS[handler_index].LLI_ID)
                {
                    if (MULTIINPUT_LLI_HANDLERS[handler_index].Get != NULL)
                    {
                        // Set the data for the individual input of the Multi Input LLI based on the LLI ID
                        // This must be handled this way because the LLI Get() functions return a void pointer and each LLI has a different data size
                        switch (MultiInputLLI_Config[input->Data_Index].LLI_Data[index].LLI_ID)
                        {
                            case HBL_LLI_DIGITAL:
                                digital_lli_data_ptr = MULTIINPUT_LLI_HANDLERS[handler_index].Get(&MultiInputLLI_Config[input->Data_Index].LLI_Data[index]);
                                MultiInputLLI_Data[input->Data_Index][index] = (uint16)(digital_lli_data_ptr[0]);
                                break;
                            case HBL_LLI_ANALOG:
                                analog_lli_data_ptr = MULTIINPUT_LLI_HANDLERS[handler_index].Get(&MultiInputLLI_Config[input->Data_Index].LLI_Data[index]);
                                MultiInputLLI_Data[input->Data_Index][index] = (uint16)(analog_lli_data_ptr[0]);
                                break;
                            case HBL_LLI_FEEDBACK:
                                feedback_lli_data_ptr = MULTIINPUT_LLI_HANDLERS[handler_index].Get(&MultiInputLLI_Config[input->Data_Index].LLI_Data[index]);
                                MultiInputLLI_Data[input->Data_Index][index] = (uint16)(feedback_lli_data_ptr[0]);
                                break;
                            default:
                                break;
                        }
                    }
                    break;
                }
            }
        }
        return ((void *)&MultiInputLLI_Data[input->Data_Index]);
    }
    else
    {
        return (NULL);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Asynchronous processing of a MultiInput LLI.
 * @param   input: data related to the instance of the LLI
 * @param   status: see definition of HBL_LLI_STATUS
 * @return  PASS if the LLI was successfully processed, else FAIL.
 */
BOOL_TYPE MultiInputLLI__AsynchProcess(HBL_LLI_TYPE * input)
{
    BOOL_TYPE retval;
    uint8 handler_index;
    uint8 index;

    retval = FALSE;

    if (input->Data_Index < MultiInputLLI_Allocation)
    {
        for (index = 0; index < MultiInputLLI_Config[input->Data_Index].Num_Of_Inputs; index++)
        {
            for (handler_index = 0; handler_index < MULTIINPUT_NUM_REGISTERED_INPUTS; handler_index++)
            {
                if (MultiInputLLI_Config[input->Data_Index].LLI_Data[index].LLI_ID == MULTIINPUT_LLI_HANDLERS[handler_index].LLI_ID)
                {
                    if (MULTIINPUT_LLI_HANDLERS[handler_index].AsynchProcess != NULL)
                    {
                        retval |= MULTIINPUT_LLI_HANDLERS[handler_index].AsynchProcess(&MultiInputLLI_Config[input->Data_Index].LLI_Data[index]);
                    }
                    break;
                }
            }
        }
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Peak processing of a MultiInput LLI.
 * @param   input: data related to the instance of the LLI
 * @param   status: see definition of HBL_LLI_STATUS
 * @param   peak_polarity:
 * @return  PASS if the LLI was successfully processed, else FAIL.
 */
BOOL_TYPE MultiInputLLI__PeakProcess(HBL_LLI_TYPE * input, BOOL_TYPE peak_polarity)
{
    BOOL_TYPE retval;
    uint8 handler_index;
    uint8 index;

    retval = FALSE;

    if (input->Data_Index < MultiInputLLI_Allocation)
    {
        for (index = 0; index < MultiInputLLI_Config[input->Data_Index].Num_Of_Inputs; index++)
        {
            for (handler_index = 0; handler_index < MULTIINPUT_NUM_REGISTERED_INPUTS; handler_index++)
            {
                if (MultiInputLLI_Config[input->Data_Index].LLI_Data[index].LLI_ID == MULTIINPUT_LLI_HANDLERS[handler_index].LLI_ID)
                {
                    if (MULTIINPUT_LLI_HANDLERS[handler_index].PeakProcess != NULL)
                    {
                        retval |= MULTIINPUT_LLI_HANDLERS[handler_index].PeakProcess(&MultiInputLLI_Config[input->Data_Index].LLI_Data[index], peak_polarity);
                    }
                    break;
                }
            }
        }
    }

    return (retval);
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#endif // (HBL_LLI_NUM_MULTIINPUT > 0)

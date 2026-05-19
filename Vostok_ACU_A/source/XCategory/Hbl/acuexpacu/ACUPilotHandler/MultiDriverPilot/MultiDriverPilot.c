/**
 *  @file       
 *
 *  @brief      This is Damper ON/OFF type
 *
 *
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "MultiDriverPilot.h"

#if (HBL_PILOT_NUM_MULTIDRIVER > 0)
#include "string.h"

#if (HBL_PILOT_NUM_DIGITALOUTPUT > 0)
    #include "DigitalOutputPilot.h"
#endif

#if (HBL_PILOT_NUM_ZCTRIAC > 0)
    #include "ZCTriacPilot.h"
#endif

#if (HBL_PILOT_NUM_ZCRELAY > 0)
    #include "ZCRelayPilot.h"
#endif

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#if (HBL_PILOT_NUM_DIGITALOUTPUT > 0)
    #define MULTIDRIVER_REGISTERED_PILOT_DIGITAL        1
#else
    #define MULTIDRIVER_REGISTERED_PILOT_DIGITAL        0
#endif

#if (HBL_PILOT_NUM_ZCTRIAC > 0)
    #define MULTIDRIVER_REGISTERED_PILOT_TRIAC          (MULTIDRIVER_REGISTERED_PILOT_DIGITAL + 1)
#else
    #define MULTIDRIVER_REGISTERED_PILOT_TRIAC          MULTIDRIVER_REGISTERED_PILOT_DIGITAL
#endif

#if (HBL_PILOT_NUM_ZCRELAY > 0)
    #define MULTIDRIVER_REGISTERED_PILOT_ZCRELAY        (MULTIDRIVER_REGISTERED_PILOT_TRIAC + 1)
#else
    #define MULTIDRIVER_REGISTERED_PILOT_ZCRELAY        MULTIDRIVER_REGISTERED_PILOT_TRIAC
#endif


#define MULTIDRIVER_NUM_REGISTERED_PILOT    MULTIDRIVER_REGISTERED_PILOT_ZCRELAY

#define NO_PARAMETER_OFFSET                   0xFF
#define MULTIDRIVERPILOT_MAX_PILOT_HANDLING   4


PACKED typedef struct
{
    HBL_LOAD_TYPE Load[MULTIDRIVERPILOT_MAX_PILOT_HANDLING];
    uint8 Requested_State[MULTIDRIVERPILOT_MAX_PILOT_HANDLING];
    uint8 Processed_State[MULTIDRIVERPILOT_MAX_PILOT_HANDLING];
    uint8 Max_Pilot;
} MULTIDRIVERPILOT_DATA_TYPE;

PACKED typedef struct
{
    HBL_PILOT_ENUM_TYPE Pilot_Type;
    uint8 Pilot_Type_Params_Offset;
} MULTIDRIVERPILOT_PARAMETERS_TYPE;

#if (MULTIDRIVER_NUM_REGISTERED_PILOT > 0)
static MULTIDRIVERPILOT_DATA_TYPE MultiDriverPilot_Data[HBL_PILOT_NUM_MULTIDRIVER];
static uint8 MultiDriverPilot_Allocation;

static const HBL_PILOT_HANDLER_HOOK_TYPE MULTIDRIVER__PILOT_HANDLERS[MULTIDRIVER_NUM_REGISTERED_PILOT]  = {
                                                                                                        #if (HBL_PILOT_NUM_DIGITALOUTPUT > 0)
                                                                                                            {DIGITALOUTPUTPILOT_REGISTER         ,HBL_PILOT_DIGITAL}
                                                                                                        #endif

                                                                                                        #if (HBL_PILOT_NUM_ZCTRIAC > 0)
                                                                                                            ,{ZCTRIACPILOT_REGISTER                ,HBL_PILOT_TRIAC}
                                                                                                        #endif

                                                                                                        #if (HBL_PILOT_NUM_ZCRELAY > 0)
                                                                                                            ,{ZCRELAYPILOT_REGISTER              ,HBL_PILOT_ZEROCROSSING_RELAY}
                                                                                                        #endif
                                                                                                     };

#endif


//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module MultiDriverPilot and its variables
 *
 */
void MultiDriverPilot__Initialize(void)
{
    #if (MULTIDRIVER_NUM_REGISTERED_PILOT > 0)
        MultiDriverPilot_Allocation = 0;
        memset(&MultiDriverPilot_Data,0,sizeof(MultiDriverPilot_Data));
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @return
 */
uint8 MultiDriverPilot__Allocate(void)
{
    uint8 retval;
    retval = 0xFF;

    #if (MULTIDRIVER_NUM_REGISTERED_PILOT > 0)
        if (MultiDriverPilot_Allocation < HBL_PILOT_NUM_MULTIDRIVER)
        {
            retval = MultiDriverPilot_Allocation;
            MultiDriverPilot_Allocation++;
        }
    #endif

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param load
 */
PASS_FAIL_TYPE MultiDriverPilot__InitializeInstance(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval;
    uint8 index;
    uint8 handler_index;
    MULTIDRIVERPILOT_PARAMETERS_TYPE *parameters;

    retval = FAIL;

    #if (MULTIDRIVER_NUM_REGISTERED_PILOT > 0)
        if (load->Pilot_Data_Index < MultiDriverPilot_Allocation)
        {
            parameters = (MULTIDRIVERPILOT_PARAMETERS_TYPE *)load->Parameter_Ptr.Data;

            if (load->Pilot_Type == HBL_PILOT_MULTIDRIVER_2)
            {
                MultiDriverPilot_Data[load->Pilot_Data_Index].Max_Pilot = 2;
            }
            else if (load->Pilot_Type == HBL_PILOT_MULTIDRIVER_3)
            {
                MultiDriverPilot_Data[load->Pilot_Data_Index].Max_Pilot = 3;
            }
            else if (load->Pilot_Type == HBL_PILOT_MULTIDRIVER_4)
            {
                MultiDriverPilot_Data[load->Pilot_Data_Index].Max_Pilot = 4;
            }

            for (index = 0; index < MultiDriverPilot_Data[load->Pilot_Data_Index].Max_Pilot; index++)
            {
                MultiDriverPilot_Data[load->Pilot_Data_Index].Load[index].Load_Id				   = load->Load_Id;
                MultiDriverPilot_Data[load->Pilot_Data_Index].Load[index].Pilot_Type               = parameters[index].Pilot_Type;
                MultiDriverPilot_Data[load->Pilot_Data_Index].Load[index].Pilot_Type_Params_Offset = parameters[index].Pilot_Type_Params_Offset;
                MultiDriverPilot_Data[load->Pilot_Data_Index].Load[index].Pin_Ptr                  = &(load->Pin_Ptr[index]);

                MultiDriverPilot_Data[load->Pilot_Data_Index].Load[index].Hi_Low_Activation_Bitmap =  (load->Hi_Low_Activation_Bitmap >> index) & 0x01;


                if (MultiDriverPilot_Data[load->Pilot_Data_Index].Load[index].Pilot_Type_Params_Offset != NO_PARAMETER_OFFSET)
                {
                    SettingFile__BasicLoader(SF_PTR_ACU_IO_DEVICE, MultiDriverPilot_Data[load->Pilot_Data_Index].Load[index].Pilot_Type_Params_Offset, &MultiDriverPilot_Data[load->Pilot_Data_Index].Load[index].Parameter_Ptr);
                }
                else
                {
                    MultiDriverPilot_Data[load->Pilot_Data_Index].Load[index].Parameter_Ptr.Data = NULL;
                    MultiDriverPilot_Data[load->Pilot_Data_Index].Load[index].Parameter_Ptr.Length = 0;
                }


                MultiDriverPilot_Data[load->Pilot_Data_Index].Load[index].Pilot_Data_Index = 0xFF;
                MultiDriverPilot_Data[load->Pilot_Data_Index].Load[index].Pilot_Handler_Index= HBL_INDEX_INVALID;

                for (handler_index = 0; handler_index < MULTIDRIVER_NUM_REGISTERED_PILOT; handler_index++)
                {
                    if (MultiDriverPilot_Data[load->Pilot_Data_Index].Load[index].Pilot_Type == MULTIDRIVER__PILOT_HANDLERS[handler_index].Pilot)
                    {
                    	MultiDriverPilot_Data[load->Pilot_Data_Index].Load[index].Pilot_Handler_Index  = handler_index;
                        MultiDriverPilot_Data[load->Pilot_Data_Index].Load[index].Pilot_Data_Index  = MULTIDRIVER__PILOT_HANDLERS[handler_index].Allocate();
                        retval  = MULTIDRIVER__PILOT_HANDLERS[handler_index].InitializeInstance(&MultiDriverPilot_Data[load->Pilot_Data_Index].Load[index]);
                        break;
                    }
                }
            }
        }

    #endif

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @param load
 * @param data
 * @return
 */
PASS_FAIL_TYPE MultiDriverPilot__Request(HBL_LOAD_TYPE * load, void * data)
{
    PASS_FAIL_TYPE retval;
    uint8 index;
    uint8 handler_index;

    retval = FAIL;

    #if (MULTIDRIVER_NUM_REGISTERED_PILOT > 0)
        if (load->Pilot_Data_Index < MultiDriverPilot_Allocation)
        {
            memcpy(MultiDriverPilot_Data[load->Pilot_Data_Index].Requested_State, ((uint8 *) data), MultiDriverPilot_Data[load->Pilot_Data_Index].Max_Pilot);

            for (index = 0; index < MultiDriverPilot_Data[load->Pilot_Data_Index].Max_Pilot; index++)
            {
            	handler_index = MultiDriverPilot_Data[load->Pilot_Data_Index].Load[index].Pilot_Handler_Index;

            	if (handler_index != HBL_INDEX_INVALID)
            	{
            		if (MULTIDRIVER__PILOT_HANDLERS[handler_index].Request != NULL)
					{
						retval  = MULTIDRIVER__PILOT_HANDLERS[handler_index].Request(&MultiDriverPilot_Data[load->Pilot_Data_Index].Load[index], &MultiDriverPilot_Data[load->Pilot_Data_Index].Requested_State[index] );
					}
				}
            }
        }

    #endif

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param load
 * @param data
 * @return
 */
PASS_FAIL_TYPE MultiDriverPilot__AsynchProcess(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval;
    uint8 index;
    uint8 handler_index;

    retval     = FAIL;

    #if (MULTIDRIVER_NUM_REGISTERED_PILOT > 0)
        if (load->Pilot_Data_Index < MultiDriverPilot_Allocation)
        {
            for (index = 0; index < MultiDriverPilot_Data[load->Pilot_Data_Index].Max_Pilot; index++)
            {
            	handler_index = MultiDriverPilot_Data[load->Pilot_Data_Index].Load[index].Pilot_Handler_Index;

				if (handler_index != HBL_INDEX_INVALID)
				{
					if (MULTIDRIVER__PILOT_HANDLERS[handler_index].AsynchProcess != NULL)
					{
						retval  = MULTIDRIVER__PILOT_HANDLERS[handler_index].AsynchProcess(&MultiDriverPilot_Data[load->Pilot_Data_Index].Load[index]);
					}
                }
            }
        }

    #endif

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param load
 * @param data
 * @return
 */
PASS_FAIL_TYPE MultiDriverPilot__ZCProcess(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval;
    uint8 index;
    uint8 handler_index;

    retval     = FAIL;

    #if (MULTIDRIVER_NUM_REGISTERED_PILOT > 0)
        if (load->Pilot_Data_Index < MultiDriverPilot_Allocation)
        {
            for (index = 0; index < MultiDriverPilot_Data[load->Pilot_Data_Index].Max_Pilot; index++)
            {
            	handler_index = MultiDriverPilot_Data[load->Pilot_Data_Index].Load[index].Pilot_Handler_Index;

				if (handler_index != HBL_INDEX_INVALID)
				{
					if (MULTIDRIVER__PILOT_HANDLERS[handler_index].ZcProcess != NULL)
					{
						retval  = MULTIDRIVER__PILOT_HANDLERS[handler_index].ZcProcess(&MultiDriverPilot_Data[load->Pilot_Data_Index].Load[index]);
					}
                }
            }
        }

    #endif

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param load
 * @return
 */
PASS_FAIL_TYPE MultiDriverPilot__Process(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval;
    uint8 index;
    uint8 handler_index;

    retval     = FAIL;

    #if (MULTIDRIVER_NUM_REGISTERED_PILOT > 0)
        if (load->Pilot_Data_Index < MultiDriverPilot_Allocation)
        {
            memcpy(MultiDriverPilot_Data[load->Pilot_Data_Index].Processed_State, MultiDriverPilot_Data[load->Pilot_Data_Index].Requested_State, MultiDriverPilot_Data[load->Pilot_Data_Index].Max_Pilot);

            for (index = 0; index < MultiDriverPilot_Data[load->Pilot_Data_Index].Max_Pilot; index++)
            {
            	handler_index = MultiDriverPilot_Data[load->Pilot_Data_Index].Load[index].Pilot_Handler_Index;

				if (handler_index != HBL_INDEX_INVALID)
				{
					if (MULTIDRIVER__PILOT_HANDLERS[handler_index].Process != NULL)
					{
						retval  = MULTIDRIVER__PILOT_HANDLERS[handler_index].Process(&MultiDriverPilot_Data[load->Pilot_Data_Index].Load[index]);
					}
                }
            }
        }

    #endif

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param load
 * @return
 */
void * MultiDriverPilot__GetRequested(HBL_LOAD_TYPE * load)
{
	void *retval;

	retval = NULL;

    #if (MULTIDRIVER_NUM_REGISTERED_PILOT > 0)
        if (load->Pilot_Data_Index < MultiDriverPilot_Allocation)
        {
            retval = (void *)(MultiDriverPilot_Data[load->Pilot_Data_Index].Requested_State);
        }
    #endif

	return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param load
 * @return
 */
void * MultiDriverPilot__GetProcessed(HBL_LOAD_TYPE * load)
{
	void *retval;

	retval = NULL;

    #if (MULTIDRIVER_NUM_REGISTERED_PILOT > 0)
        if (load->Pilot_Data_Index < MultiDriverPilot_Allocation)
        {
            retval = (void *) (MultiDriverPilot_Data[load->Pilot_Data_Index].Processed_State);
        }
    #endif

	return (retval);
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


#endif

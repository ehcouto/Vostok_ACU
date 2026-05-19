/**
 *  @file       
 *
 *  @brief  Handles the SRTouch low level input, which is used for Touch keys.
 *
 *  @defgroup   CLASS_B
 *
    @startuml{SRTouchKeyLLI_Class_Diagram.png}

        hide circle
        hide empty attributes
        hide empty methods
        skinparam classAttributeIconSize 0

        class SRTouchKeyLLI {
            +void SRTouchKeyLLI__Initialize(void);
            +uint8 SRTouchKeyLLI__Allocate(void);
            +PASS_FAIL_TYPE SRTouchKeyLLI__InitializeInstance(HBL_LLI_TYPE * input);
            +void * SRTouchKeyLLI__Get(HBL_LLI_TYPE * input);
            +BOOL_TYPE SRTouchKeyLLI__AsynchProcess(HBL_LLI_TYPE * input);
            }

        class SRSystemConfig {
            +SYSTEM_ENABLE_TASKSEQUENCE
            +HBL_LLI_NUM_SRTOUCH_KEY
        }

        class SystemConfig {
            +TOUCH_I2C_PROTOCOL
        }

        SRTouchKeyLLI --|> TouchInterface
        TouchInterface--|> SRTouchI2c
        TouchInterface--|> Touch
        TouchInterface--|> TouchI2c
        TouchInterface--|> SRTouchIntegrated
        SRTouchKeyLLI ..> SRData
        SRTouchKeyLLI ..> SRTaskSequence
        SRTouchKeyLLI ..> SRSystemConfig
        SRTouchKeyLLI ..> SystemConfig

    @enduml
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SRSystemConfig.h"
#include "SystemConfig.h"
#include "SRTouchKeyLLI.h"

#if(SRTOUCH_INTERFACE_FEATURE == ENABLED)
#include "SRTouchInterface.h"
#endif
#if (HBL_LLI_NUM_SRTOUCH_KEY > 0)
// -- Other Modules --
#include "SRData.h"
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    #include "SRTaskSequence.h"
#endif

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//! Data for each SRTouchKey LLI
static HBL_KEY_TYPE SRTouchKeyLLI_Data[HBL_LLI_NUM_SRTOUCH_KEY];
//! Inverted memory for the Class B variable
static HBL_KEY_TYPE N_SRTouchKeyLLI_Data[HBL_LLI_NUM_SRTOUCH_KEY];

//! Keeps track of how many SRTouchKey LLIs have been allocated by the application
static uint8 SRTouchKeyLLI_Allocation;
//! Inverted memory for the Class B variable
static uint8 N_SRTouchKeyLLI_Allocation;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Initialization of the SRTouchKeyLLI module.
 */
void SRTouchKeyLLI__Initialize(void)
{
    SRData__Fill(&SRTouchKeyLLI_Data, &N_SRTouchKeyLLI_Data, 0, sizeof(SRTouchKeyLLI_Data[0]) * HBL_LLI_NUM_SRTOUCH_KEY);

    SR_SET_BYTE(SRTouchKeyLLI_Allocation, 0);

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_INIT, SRTASKSEQUENCE_APPL_INIT_SRTOUCHKEY_LLI);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Determines the destination of the data for the SRTouchKey LLI.
 * @return  Index to a local buffer where the data will reside.
 */
uint8 SRTouchKeyLLI__Allocate(void)
{
    uint8 retval = HBL_ALLOCATION_INVALID;

    if ((SR_CHECK_BYTE(SRTouchKeyLLI_Allocation) == SRDATA_OK) &&
        (SRTouchKeyLLI_Allocation < HBL_LLI_NUM_SRTOUCH_KEY))
    {
        retval = SRTouchKeyLLI_Allocation;
        SR_SET_BYTE(SRTouchKeyLLI_Allocation, SRTouchKeyLLI_Allocation+1);
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Initialize an instance of the SRTouchKey LLI.
 * @param   input: pointer to various data for the LLI, managed by the HblHmi module
 * @return  PASS if the instance of the LLI was successfully initialized, else FALSE
 */
PASS_FAIL_TYPE SRTouchKeyLLI__InitializeInstance(HBL_LLI_TYPE * input)
{
    if ((SR_CHECK_BYTE(SRTouchKeyLLI_Allocation) == SRDATA_OK) &&
        (input->Data_Index < SRTouchKeyLLI_Allocation))
    {
        SRData__UpdateByte((uint8 *)(void *)&SRTouchKeyLLI_Data[input->Data_Index],
                           (uint8 *)(void *)&N_SRTouchKeyLLI_Data[input->Data_Index],
                           HBL_KEY_RELEASED);
        return(PASS);
    }
    else
    {
        return(FAIL);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Gets the data for one instance of a SRTouchKey LLI.
 * @param   input: pointer to various data for the LLI, managed by the HblHmi module
 * @return  Pointer to the SRTouchKeyLLI_Data buffer that contains the current state of the instance of the SRTouchKey LLI.
 */
void * SRTouchKeyLLI__Get(HBL_LLI_TYPE * input)
{
    if ((SR_CHECK_BYTE(SRTouchKeyLLI_Allocation) == SRDATA_OK) && (input->Data_Index < SRTouchKeyLLI_Allocation))
    {
        uint8 touch_key_state;
        uint8 touch_key_mask;

        uint8 byte_index = input->Parameter_Ptr.Data[3] / 8;       // Determine which byte contains the state of the key
        //input->Parameter_Ptr.Data[2] is used by GESE and SRTouchI2c but not explicit used as parameter on the function call
#if(SRTOUCH_INTERFACE_FEATURE == ENABLED)
        touch_key_state = (SRTouchInterface__GetKeyStatus())[byte_index];   // Get byte that contains the state of the key

        touch_key_mask = (0x01 << (input->Parameter_Ptr.Data[3] % 8)); // Determine the mask to find the bit that correlates to the key
        touch_key_state = (touch_key_state & touch_key_mask) ? 1 : 0;  // Set to 1 if key state matches the mask, else 0

        SRData__UpdateByte((uint8 *) (void *) &SRTouchKeyLLI_Data[input->Data_Index], // Update protected variables that hold the current state of the LLI
        (uint8 *) (void *) &N_SRTouchKeyLLI_Data[input->Data_Index], touch_key_state);

        return ((void *) &SRTouchKeyLLI_Data[input->Data_Index]);
#else
        return (NULL);
#endif
    }
    else
    {
        return (NULL );
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Asynchronous processing of the SRTouchKey LLI. This function only exists because it is one of the functions
 *          required by all LLI modules.
 * @param   input: pointer to various data for the LLI managed by the HblHmi module
 * @return  Always TRUE
 */
BOOL_TYPE SRTouchKeyLLI__AsynchProcess(HBL_LLI_TYPE * input)
{
    // Function needs to return TRUE to increment the LLI sequence number (indicating new sample is available)
    return (TRUE);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#endif //#if (HBL_LLI_NUM_SRTOUCH_KEY > 0)

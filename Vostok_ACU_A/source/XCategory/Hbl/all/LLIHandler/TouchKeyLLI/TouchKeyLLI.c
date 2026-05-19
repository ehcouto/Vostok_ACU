/**
 * @brief       Handles the Extern low level input, which is used for Touch keys.
 *
 * @details    
 *
 * @copyright   Copyright 2023 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
//-------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "SRSystemConfig.h"
#include "TouchKeyLLI.h"
//! Other Modules
#if(SRTOUCH_INTERFACE_FEATURE == ENABLED)
#include "SRTouchInterface.h"
#elif(TOUCH_INTERFACE_FEATURE == ENABLED)
#include "TouchInterface.h"
#endif

#if (HBL_LLI_NUM_TOUCH_KEY > 0)

#include <string.h>
#include "Utilities.h"

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
static HBL_KEY_TYPE TouchKeyLLI_Data[HBL_LLI_NUM_TOUCH_KEY];
static uint8 TouchKeyLLI_Allocation;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Initializes the module TouchKeyLLI and its variables
 */
void TouchKeyLLI__Initialize(void)
{
    memset(&TouchKeyLLI_Data, 0, sizeof(TouchKeyLLI_Data));
    TouchKeyLLI_Allocation = 0;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @return
 */
uint8 TouchKeyLLI__Allocate(void)
{
    uint8 retval;
    retval = HBL_ALLOCATION_INVALID;

    if (TouchKeyLLI_Allocation < HBL_LLI_NUM_TOUCH_KEY)
    {
        retval = TouchKeyLLI_Allocation;
        TouchKeyLLI_Allocation++;
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief
 *
 */
PASS_FAIL_TYPE TouchKeyLLI__InitializeInstance(HBL_LLI_TYPE * input)
{
    PASS_FAIL_TYPE retval;

    retval = FAIL;

    if (input->Data_Index < TouchKeyLLI_Allocation)
    {
        // Note: Intentionally not storing LLI sequence; key press/release detection done in KeyGI module
        TouchKeyLLI_Data[input->Data_Index] = HBL_KEY_RELEASED; // Set all keys not pressed
        retval = PASS;
    }

    return(retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @param input
 *
 * @return
 */
void * TouchKeyLLI__Get(HBL_LLI_TYPE * input)
{

    if (input->Data_Index < TouchKeyLLI_Allocation)
    {
        #if(SRTOUCH_INTERFACE_FEATURE == ENABLED)
            //input->Parameter_Ptr.Data[2] is used by GESE and TouchI2c but not explicit used as parameter on the function call
            TouchKeyLLI_Data[input->Data_Index] = (HBL_KEY_TYPE)Utilities__GetBitInArray((uint8 *)SRTouchInterface__GetKeyStatus(), input->Parameter_Ptr.Data[3]);
            return((void *) &TouchKeyLLI_Data[input->Data_Index]);
       #elif(TOUCH_INTERFACE_FEATURE == ENABLED)
            TouchKeyLLI_Data[input->Data_Index] = (HBL_KEY_TYPE)Utilities__GetBitInArray((uint8 *)TouchInterface__GetKeyStatus(), input->Parameter_Ptr.Data[3]);
            return((void *) &TouchKeyLLI_Data[input->Data_Index]);
        #else
            return (NULL);
        #endif

    }
    else
    {
        return (NULL);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @param input
 * @return
 */
BOOL_TYPE TouchKeyLLI__AsynchProcess(HBL_LLI_TYPE * input)
{
    // Function needs to return TRUE to increment the LLI sequence number (indicating new sample is available)
    return (TRUE);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#endif //#if (HBL_LLI_NUM_TOUCH_KEY > 0)

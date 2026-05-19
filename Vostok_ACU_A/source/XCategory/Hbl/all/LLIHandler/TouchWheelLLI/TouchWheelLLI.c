/**
 *  @file       
 *
 *  @brief  Handles the Touch Wheel low level input, which is used for Touch wheels.
 *
 *  @details
 *
 *  This module is used to provide a LLI specifically for touch wheels. The module is
 *  different from normal touch keys and touch sliders because of the way the data is
 *  processed for wheels and the number of sensors that are attached to each wheel for
 *  processing (which is often more than sliders). The low level interface for a wheel
 *  will obtain a value from the touch library that is between 0 and 255. When the value
 *  is 0, it means that the input from the wheel is invalid, or in other words the touch
 *  wheel is not actively being touched.
 *
 *  This LLI does not yet work with TouchI2c. The enable is currently set to return a
 *  null value when TouchI2c is enabled. This will be updated once the feature is added
 *  in the touch API.
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"
#include "TouchWheelLLI.h"

#if (HBL_LLI_NUM_TOUCH_WHEEL > 0)

#ifndef TOUCH_I2C_PROTOCOL
    #error: TOUCH_I2C_PROTOCOL must be defined in SystemConfig_prm.h as ENABLED or DISABLED
#endif

#if (TOUCH_I2C_PROTOCOL == ENABLED)
    #include "TouchI2c.h"
#else
    #include "Touch.h"
#endif
#include <string.h>

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

static uint8 TouchWheelLLI_Data[HBL_LLI_NUM_TOUCH_WHEEL];
static uint8 TouchWheelLLI_Allocation;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief	Initializes the module TouchWheelLLI and its variables
 */
void TouchWheelLLI__Initialize(void)
{
    memset(&TouchWheelLLI_Data, 0, sizeof(TouchWheelLLI_Data));
    TouchWheelLLI_Allocation = 0;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @return
 */
uint8 TouchWheelLLI__Allocate(void)
{
    uint8 retval = HBL_ALLOCATION_INVALID;

    if (TouchWheelLLI_Allocation < HBL_LLI_NUM_TOUCH_WHEEL)
    {
        retval = TouchWheelLLI_Allocation;
        TouchWheelLLI_Allocation++;
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief
 *
 */
PASS_FAIL_TYPE TouchWheelLLI__InitializeInstance(HBL_LLI_TYPE * input)
{
    PASS_FAIL_TYPE retval;

    retval = FAIL;

    if (input->Data_Index < TouchWheelLLI_Allocation)
    {
        TouchWheelLLI_Data[input->Data_Index] = 0;
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
void * TouchWheelLLI__Get(HBL_LLI_TYPE * input)
{
    if (input->Data_Index < TouchWheelLLI_Allocation)
    {
        #if (TOUCH_I2C_PROTOCOL == ENABLED)
        // as explained above in the details, this must return a NULL pointer to the calling function since the data cannot be processed for touchI2C currently
            return((void*) NULL);
        #else
            //call Touch__GetWheelPosition instead of Touch__GetLastWheelPosition because we need to know if the wheel is actively being touched
            TouchWheelLLI_Data[input->Data_Index] = (uint8)Touch__GetWheelPosition(input->LLI_Position);
            return((void *) &TouchWheelLLI_Data[input->Data_Index]);
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
BOOL_TYPE TouchWheelLLI__AsynchProcess(HBL_LLI_TYPE * input)
{
    // Function needs to return TRUE to increment the LLI sequence number (indicating new sample is available)
    return (TRUE);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#endif //#if (HBL_LLI_NUM_TOUCH_WHEEL > 0)

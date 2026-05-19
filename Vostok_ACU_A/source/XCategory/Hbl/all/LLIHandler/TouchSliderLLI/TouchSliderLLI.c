/**
 *  @file       
 *
 *  @brief  Handles the Touch Slider low level input, which is used for Touch sliders.
 *
 *  @details
 *
 *  This module is used to provide a LLI specifically for touch sliders.
 *  The module is different from normal touch keys because of the way the data is processed
 *  for sliders and the number of sensors that are attached to each slider for processing.
 *
 *  For Atmel Touch:
 *      The low level interface for a slider will obtain a value from the touch library
 *      that is between 0 and 255. When the value is 0, it means that the input from the slider is
 *      invalid.
 *
 *      The GetLastSliderPosition function returns the last touched value for the slider,
 *      which is the needed value for any application. A similar function exists in the Touch.h library,
 *      called GetSliderPosition, but the function only returns the currently touched position. This means
 *      the user would have to be touching the slider to return a valid value. It is better to use the
 *      GetLastSliderPosition function call instead to get the last touched position.
 *
 *  For Cypress Touch:
 *      Valid values range is <0;100>. Library reports slider value 255 when slider is released.
 *
 *  @copyright  Copyright 2020. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"
#include "TouchSliderLLI.h"
#if (HBL_LLI_NUM_TOUCH_SLIDER > 0)

// -- Other Modules --
#if(TOUCH_INTERFACE_FEATURE == ENABLED)
#include "TouchInterface.h"
#elif(SRTOUCH_INTERFACE_FEATURE == ENABLED)
#include "SRTouchInterface.h"
#endif
#include <string.h>

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
static uint8 TouchSliderLLI_Data[HBL_LLI_NUM_TOUCH_SLIDER];
static uint8 TouchSliderLLI_Allocation;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief	Initializes the module TouchSliderLLI and its variables
 */
void TouchSliderLLI__Initialize(void)
{
    memset(&TouchSliderLLI_Data, 0, sizeof(TouchSliderLLI_Data));
    TouchSliderLLI_Allocation = 0;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @return
 */
uint8 TouchSliderLLI__Allocate(void)
{
    uint8 retval;
    retval = HBL_ALLOCATION_INVALID;

    if (TouchSliderLLI_Allocation < HBL_LLI_NUM_TOUCH_SLIDER)
    {
        retval = TouchSliderLLI_Allocation;
        TouchSliderLLI_Allocation++;
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief
 *
 */
PASS_FAIL_TYPE TouchSliderLLI__InitializeInstance(HBL_LLI_TYPE * input)
{
    PASS_FAIL_TYPE retval;

    retval = FAIL;

    if (input->Data_Index < TouchSliderLLI_Allocation)
    {
        TouchSliderLLI_Data[input->Data_Index] = 0;
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
void * TouchSliderLLI__Get(HBL_LLI_TYPE * input)
{
    if (input->Data_Index < TouchSliderLLI_Allocation)
    {
#if(SRTOUCH_INTERFACE_FEATURE == ENABLED)
           TouchSliderLLI_Data[input->Data_Index] = (uint8)SRTouchInterface__GetSliderPosition(input->LLI_Position);
#elif(TOUCH_INTERFACE_FEATURE == ENABLED)
           TouchSliderLLI_Data[input->Data_Index] = (uint8)TouchInterface__GetSliderPosition(input->LLI_Position);
#endif

         return((void *) &TouchSliderLLI_Data[input->Data_Index]);
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
BOOL_TYPE TouchSliderLLI__AsynchProcess(HBL_LLI_TYPE * input)
{
    // Function needs to return TRUE to increment the LLI sequence number (indicating new sample is available)
    return (TRUE);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#endif //#if (HBL_LLI_NUM_TOUCH_SLIDER > 0)

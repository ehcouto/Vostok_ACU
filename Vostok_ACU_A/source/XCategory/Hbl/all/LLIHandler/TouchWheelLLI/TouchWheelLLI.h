/**
 *  @file       
 *
 *  @brief      Public interface for the TouchWheelLLI module.
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef TOUCHWHEELLLI_H_
#define TOUCHWHEELLLI_H_
#ifndef HBL_LLI_NUM_TOUCH_WHEEL
    #define HBL_LLI_NUM_TOUCH_WHEEL 0
#endif

#if (HBL_LLI_NUM_TOUCH_WHEEL > 0)

#include "HblDefs.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#define TOUCHWHEEL_LLI_REGISTER    TouchWheelLLI__Initialize,          \
                                   TouchWheelLLI__Allocate,            \
                                   TouchWheelLLI__InitializeInstance,  \
                                   TouchWheelLLI__Get,                 \
                                   TouchWheelLLI__AsynchProcess,       \
                                   NULL,                        \
                                   NULL,                        \
                                   NULL,                        \
                                   1
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void TouchWheelLLI__Initialize(void);
uint8 TouchWheelLLI__Allocate(void);
PASS_FAIL_TYPE TouchWheelLLI__InitializeInstance(HBL_LLI_TYPE * input);
void * TouchWheelLLI__Get(HBL_LLI_TYPE * input);
BOOL_TYPE TouchWheelLLI__AsynchProcess(HBL_LLI_TYPE * input);

#endif // #if (HBL_LLI_NUM_TOUCH_WHEEL > 0)

#endif // TOUCHWHEELLLI_H_

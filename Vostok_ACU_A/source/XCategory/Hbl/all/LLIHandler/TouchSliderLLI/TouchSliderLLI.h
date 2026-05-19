/**
 *  @file       
 *
 *  @brief      Public interface for the TouchSliderLLI module.
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef TOUCHSLIDERLLI_H_
#define TOUCHSLIDERLLI_H_

#ifndef HBL_LLI_NUM_TOUCH_SLIDER
    #define HBL_LLI_NUM_TOUCH_SLIDER 0
#endif

#if (HBL_LLI_NUM_TOUCH_SLIDER > 0)

#include "HblDefs.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#define TOUCHSLIDER_LLI_REGISTER   TouchSliderLLI__Initialize,          \
                                   TouchSliderLLI__Allocate,            \
                                   TouchSliderLLI__InitializeInstance,  \
                                   TouchSliderLLI__Get,                 \
                                   TouchSliderLLI__AsynchProcess,       \
                                   NULL,                        \
                                   NULL,                        \
                                   NULL,                        \
                                   1
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void TouchSliderLLI__Initialize(void);
uint8 TouchSliderLLI__Allocate(void);
PASS_FAIL_TYPE TouchSliderLLI__InitializeInstance(HBL_LLI_TYPE * input);
void * TouchSliderLLI__Get(HBL_LLI_TYPE * input);
BOOL_TYPE TouchSliderLLI__AsynchProcess(HBL_LLI_TYPE * input);

#endif // #if (HBL_LLI_NUM_TOUCH_SLIDER > 0)

#endif // TOUCHSLIDERLLI_H_

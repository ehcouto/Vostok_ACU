/**
 *  @file       
 *
 *  @brief      Public interface for the MultiInputLLI module.
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef MULTIINPUTLLI_H_
#define MULTIINPUTLLI_H_

#ifndef HBL_LLI_NUM_MULTIINPUT
    #define HBL_LLI_NUM_MULTIINPUT 0
#endif

#if (HBL_LLI_NUM_MULTIINPUT > 0)

#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

typedef uint16 MULTIINPUTLLI_DATA_TYPE;

#define MULTIINIPUT_LLI_REGISTER    MultiInputLLI__Initialize,          \
                                    MultiInputLLI__Allocate,            \
                                    MultiInputLLI__InitializeInstance,  \
                                    MultiInputLLI__Get,                 \
                                    MultiInputLLI__AsynchProcess,       \
                                    MultiInputLLI__PeakProcess,         \
                                    NULL,                               \
                                    NULL,                               \
                                    8

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void MultiInputLLI__Initialize(void);
uint8 MultiInputLLI__Allocate(void);
PASS_FAIL_TYPE MultiInputLLI__InitializeInstance(HBL_LLI_TYPE * input);
void * MultiInputLLI__Get(HBL_LLI_TYPE * input);
BOOL_TYPE MultiInputLLI__AsynchProcess(HBL_LLI_TYPE * input);
BOOL_TYPE MultiInputLLI__PeakProcess(HBL_LLI_TYPE * input, BOOL_TYPE peak_polarity);

#endif // HBL_LLI_NUM_MULTIINPUT

#endif // MULTIINPUTLLI_H_

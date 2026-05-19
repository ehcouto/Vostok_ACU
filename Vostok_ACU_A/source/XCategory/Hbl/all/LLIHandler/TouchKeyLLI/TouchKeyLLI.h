/**
 *  @file       
 *
 *  @brief      Public interface for the TouchKeyLLI module.
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef KEYLLI_H_
#define KEYLLI_H_

#include "C_Extensions.h"
#include "SystemConfig.h"

#ifndef HBL_LLI_NUM_TOUCH_KEY
    #define HBL_LLI_NUM_TOUCH_KEY 0
#endif

#if (HBL_LLI_NUM_TOUCH_KEY > 0)

#include "HblDefs.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#define TOUCHKEY_LLI_REGISTER   TouchKeyLLI__Initialize,          \
                                TouchKeyLLI__Allocate,            \
                                TouchKeyLLI__InitializeInstance,  \
                                TouchKeyLLI__Get,                 \
                                TouchKeyLLI__AsynchProcess,       \
                                NULL,                        \
                                NULL,                        \
                                NULL,                        \
                                1
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void TouchKeyLLI__Initialize(void);
uint8 TouchKeyLLI__Allocate(void);
PASS_FAIL_TYPE TouchKeyLLI__InitializeInstance(HBL_LLI_TYPE * input);
void * TouchKeyLLI__Get(HBL_LLI_TYPE * input);
BOOL_TYPE TouchKeyLLI__AsynchProcess(HBL_LLI_TYPE * input);

#endif // #if (HBL_LLI_NUM_TOUCH_KEY > 0)

#endif // TOUCHKEYLLI_H_

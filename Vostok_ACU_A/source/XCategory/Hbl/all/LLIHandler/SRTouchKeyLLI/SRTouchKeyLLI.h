/**
 *  @file       
 *
 *  @brief      Public interface for the SRTouchKeyLLI module.
 *
 *  @defgroup   CLASS_B
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRTOUCHKEYLLI_H_
#define SRTOUCHKEYLLI_H_

#include "SystemConfig.h"

#ifndef HBL_LLI_NUM_SRTOUCH_KEY
    #define HBL_LLI_NUM_SRTOUCH_KEY 0
#endif

#if (HBL_LLI_NUM_SRTOUCH_KEY > 0)

#include "HblDefs.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#define SRTOUCHKEY_LLI_REGISTER   SRTouchKeyLLI__Initialize,          \
                                  SRTouchKeyLLI__Allocate,            \
                                  SRTouchKeyLLI__InitializeInstance,  \
                                  SRTouchKeyLLI__Get,                 \
                                  SRTouchKeyLLI__AsynchProcess,       \
                                  NULL,                               \
                                  NULL,                               \
                                  NULL,                               \
                                  1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SRTouchKeyLLI__Initialize(void);
uint8 SRTouchKeyLLI__Allocate(void);
PASS_FAIL_TYPE SRTouchKeyLLI__InitializeInstance(HBL_LLI_TYPE * input);
void * SRTouchKeyLLI__Get(HBL_LLI_TYPE * input);
BOOL_TYPE SRTouchKeyLLI__AsynchProcess(HBL_LLI_TYPE * input);

#endif // #if (HBL_LLI_NUM_SRTOUCH_KEY > 0)
#endif // SRTOUCHKEYLLI_H_



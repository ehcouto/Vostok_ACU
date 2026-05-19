/**
 *  @file       
 *
 *  @brief      Public interface for the VirtualDigitalLLI module.
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef VIRTUALDIGITALLLI_H_
#define VIRTUALDIGITALLLI_H_

#include "SystemConfig.h"

#ifndef HBL_LLI_NUM_VIRTUAL_DIGITAL
    #define HBL_LLI_NUM_VIRTUAL_DIGITAL 0
#endif

#if (HBL_LLI_NUM_VIRTUAL_DIGITAL > 0)

#include "HblDefs.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#define VIRTUALDIGITAL_LLI_REGISTER   VirtualDigitalLLI__Initialize,          \
                                      VirtualDigitalLLI__Allocate,            \
                                      VirtualDigitalLLI__InitializeInstance,  \
                                      VirtualDigitalLLI__Get,                 \
                                      VirtualDigitalLLI__AsynchProcess,       \
                                      NULL,                                   \
                                      NULL,                                   \
                                      NULL,                                   \
                                      1
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void VirtualDigitalLLI__Initialize(void);
uint8 VirtualDigitalLLI__Allocate(void);
PASS_FAIL_TYPE VirtualDigitalLLI__InitializeInstance(HBL_LLI_TYPE * input);
void * VirtualDigitalLLI__Get(HBL_LLI_TYPE * input);
BOOL_TYPE VirtualDigitalLLI__AsynchProcess(HBL_LLI_TYPE * input);

#endif // #if (HBL_LLI_NUM_VIRTUAL_DIGITAL > 0)

#endif // VIRTUALDIGITALLLI_H_

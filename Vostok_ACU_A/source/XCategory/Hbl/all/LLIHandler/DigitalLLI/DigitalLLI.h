/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: DigitalLowInput.h 1.7 2015/08/06 17:18:38EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/08/06 17:18:38EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef DIGITALLLI_H_
#define DIGITALLLI_H_
#include "C_Extensions.h"
#include "SystemConfig.h"

#ifndef HBL_LLI_NUM_DIGITAL
    #define HBL_LLI_NUM_DIGITAL 0
#endif

#if (HBL_LLI_NUM_DIGITAL > 0)

#include "HblDefs.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================



#define DIGITAL_LLI_REGISTER                DigitalLLI__Initialize,                 \
                                            DigitalLLI__Allocate,                   \
                                            DigitalLLI__InitializeInstance,         \
                                            DigitalLLI__Get,                        \
                                            DigitalLLI__AsynchProcess,              \
                                            NULL,                                   \
                                            NULL,                                   \
                                            NULL,                                   \
                                            1
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================


void DigitalLLI__Initialize(void);
uint8 DigitalLLI__Allocate(void);
PASS_FAIL_TYPE DigitalLLI__InitializeInstance(HBL_LLI_TYPE * input);
void * DigitalLLI__Get(HBL_LLI_TYPE * input);
BOOL_TYPE DigitalLLI__AsynchProcess(HBL_LLI_TYPE * input);

#endif
#endif // DIGITALLLI_H_



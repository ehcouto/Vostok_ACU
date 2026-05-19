/**
 *  @file       
 *
 *  @brief      Public interface for ExpansionLLI.
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//---------------------------------------------------------------------------------------------------------------------
#ifndef EXPANSIONLLI_H_
#define EXPANSIONLLI_H_

#include "SystemConfig.h"

#ifndef HBL_LLI_NUM_EXPANSION
    #define HBL_LLI_NUM_EXPANSION 0
#endif

#if (HBL_LLI_NUM_EXPANSION > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//! Function pointers definition for expansion LLI methods.
#define EXPANSION_LLI_REGISTER              ExpansionLLI__Initialize,            \
                                            ExpansionLLI__Allocate,              \
                                            ExpansionLLI__InitializeInstance,    \
                                            ExpansionLLI__Get,                   \
                                            NULL,                                       \
                                            NULL,                                       \
                                            NULL,                                       \
                                            NULL,                                       \
                                            1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void            ExpansionLLI__Initialize         (void);
uint8           ExpansionLLI__Allocate           (void);
PASS_FAIL_TYPE  ExpansionLLI__InitializeInstance (HBL_LLI_TYPE * input);
void *          ExpansionLLI__Get                (HBL_LLI_TYPE * input);

#endif /* (HBL_LLI_NUM_EXPANSION > 0) */

#endif /* EXPANSIONLLI_H_ */

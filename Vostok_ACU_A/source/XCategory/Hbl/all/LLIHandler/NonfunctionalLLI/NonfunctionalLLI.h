/**
 * @file
 * @brief       Public interface to the NonfunctionalLLI module.
 *
 * @details     Refer to the NonfunctionalLLI.c source file for more detailed information.
 *
 * @copyright   Copyright 2018 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef NONFUNCTIONALLLI_H_
#define NONFUNCTIONALLLI_H_

#ifndef HBL_LLI_NUM_NONFUNCTIONAL
    #define HBL_LLI_NUM_NONFUNCTIONAL 0
#endif

#if (HBL_LLI_NUM_NONFUNCTIONAL > 0)
//  --- Include Files -------------------------------------------------------------------------------------------------
#include "HblDefs.h"

//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================
//! The LLI function register for HBL use.
#define NON_FUNCTIONAL_LLI_REGISTER         \
    NonfunctionalLLI__Initialize,           \
    NonfunctionalLLI__Allocate,             \
    NonfunctionalLLI__InitializeInstance,   \
    NonfunctionalLLI__Get,                  \
    NULL, /*AsynchProcess*/                 \
    NULL, /*PeakProcess*/                   \
    NULL, /*ICProcess*/                     \
    NULL, /*OneMSHandler*/                  \
    1

//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================
void            NonfunctionalLLI__Initialize            (void);
uint8           NonfunctionalLLI__Allocate              (void);
PASS_FAIL_TYPE  NonfunctionalLLI__InitializeInstance    (HBL_LLI_TYPE* input);
void*           NonfunctionalLLI__Get                   (HBL_LLI_TYPE* input);

#endif // (HBL_LLI_NUM_NONFUNCTIONAL > 0)
#endif // NONFUNCTIONALLLI_H_

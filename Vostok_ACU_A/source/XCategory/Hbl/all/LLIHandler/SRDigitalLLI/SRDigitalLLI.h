/**
 *  @file       
 *
 *  @brief      Public interface for the SRDigitalLLI module.
 *
 *  @defgroup   CLASS_B
 *
 *  @copyright  Copyright 2017 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRDIGITALLLI_H_
#define SRDIGITALLLI_H_

#include "SystemConfig.h"

#ifndef HBL_LLI_NUM_SRDIGITAL
    #define HBL_LLI_NUM_SRDIGITAL 0
#endif

#if (HBL_LLI_NUM_SRDIGITAL > 0)

#include "HblDefs.h"
//! Definition of the data for each SRDigital LLI
typedef struct
{
    BOOL_TYPE value;        /* Current Digital sample value */
    uint8 index;            /* Number of Samples collected */
    uint8 level_cnt;        /* Keeps track of the number of high samples */
}SRDIGITALLLI_DATA_TYPE;
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#define SRDIGITAL_LLI_REGISTER              SRDigitalLLI__Initialize,           \
                                            SRDigitalLLI__Allocate,             \
                                            SRDigitalLLI__InitializeInstance,   \
                                            SRDigitalLLI__Get,                  \
                                            SRDigitalLLI__AsynchProcess,        \
                                            NULL,                               \
                                            NULL,                               \
                                            NULL,                               \
                                            1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SRDigitalLLI__Initialize(void);
uint8 SRDigitalLLI__Allocate(void);
PASS_FAIL_TYPE SRDigitalLLI__InitializeInstance(HBL_LLI_TYPE * input);
void SRDigitalLLI__Handler(void);
void * SRDigitalLLI__Get(HBL_LLI_TYPE * input);
BOOL_TYPE SRDigitalLLI__AsynchProcess(HBL_LLI_TYPE * input);

#endif
#endif // SRDIGITALLLI_H_

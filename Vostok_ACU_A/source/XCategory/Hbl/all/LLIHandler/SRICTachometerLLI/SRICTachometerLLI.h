/**@addtogroup SRICTachometerLLI
 *  @{
 *  @file       
 *
 *  @brief      Public interface for the SRICTachometerLLI module.
 *
 *  @defgroup   CLASS_B
 *
 *  @copyright  Copyright 2017-2018. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 *  }@
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRICTACHOMETERLLI_H
	#define SRICTACHOMETERLLI_H

#include "SRSystemConfig.h"

#ifndef HBL_LLI_NUM_SRICTACHOMETER
    #define HBL_LLI_NUM_SRICTACHOMETER    0
#endif

#if (HBL_LLI_NUM_SRICTACHOMETER > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#define SRICTACHOMETER_LLI_REGISTER         SRICTachometerLLI__Initialize,        \
                                            SRICTachometerLLI__Allocate,          \
                                            SRICTachometerLLI__InitializeInstance,\
                                            SRICTachometerLLI__Get,               \
                                            NULL,                                 \
                                            NULL,                                 \
                                            SRICTachometerLLI__ICProcess,         \
                                            SRICTachometerLLI__OneMsProcess,      \
                                            sizeof(SRICTACHOMETER_PUBLIC_TYPE)

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SRICTachometerLLI__Initialize(void);
uint8 SRICTachometerLLI__Allocate(void);
PASS_FAIL_TYPE SRICTachometerLLI__InitializeInstance(HBL_LLI_TYPE * input);
void SRICTachometerLLI__Handler(void);
void * SRICTachometerLLI__Get(HBL_LLI_TYPE * input);
BOOL_TYPE SRICTachometerLLI__ICProcess(HBL_LLI_TYPE * input);
BOOL_TYPE SRICTachometerLLI__OneMsProcess(HBL_LLI_TYPE * input);

#endif
#endif // SRICTACHOMETERLLI_H

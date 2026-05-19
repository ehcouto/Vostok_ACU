/**
 *  @file
 *  @defgroup   CLASS_B
 *
 *  @brief      consists of API's to access the Cortex-M0 core.
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
#ifndef CORTEXM0PLUS_MACRO_H
    #define CORTEXM0PLUS_MACRO_H
#include "C_Extensions.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
#ifndef __BUILTINS_INCLUDED

void __WFI(void);
void __WFE(void);
void __SEV(void);
void __ISB(void);
void __DSB(void);
void __DMB(void);
void __SVC(void);
void __MSR_CONTROL(uint32 Control);
uint32 __MRS_PSP(void);
void __MSR_PSP(uint32 TopOfProcessStack);
uint32 __MRS_MSP(void);
void __MSR_MSP(uint32 TopOfMainStack);
void __RESETPRIMASK(void);
uint16 __REV_HalfWord(uint16 Data);
uint32 __REV_Word(uint32 Data);
uint8 __StartupRegisterTest(void);
uint8 __RunRegisterTest(void);

#endif  // __BUILTINS_INCLUDED

#endif	// CORTEXM0PLUS_MACRO_H

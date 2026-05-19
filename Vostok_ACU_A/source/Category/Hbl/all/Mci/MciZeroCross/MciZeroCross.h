/**
 *  @file
 *
 *  @brief      header files for zero cross algorithm Mci
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $   Eduardo Henrique Couto (Henrie2)
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

#ifndef MCIZEROCROSS_H_
#define MCIZEROCROSS_H_

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "MciZeroCross_prm.h"
#include "Gpio.h"
#include "ZeroCross.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void MciZeroCross__Initialize(void);
uint8 MciZeroCross__ReadLastEvent(void);
void MciZeroCross__GetEvent(uint8 zc_event);

#endif // MCIZEROCROSS_H_

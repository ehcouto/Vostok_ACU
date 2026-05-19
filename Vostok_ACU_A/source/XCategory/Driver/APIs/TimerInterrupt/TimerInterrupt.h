/**
 *  @file       TimerInterrupt.h
 *
 *  @brief      Timer Interrupt GDM module implements the
 *              Timer Interrupt Standard API (Application Programming Interface).
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 * $Header: $
 *
 *  @copyright  Copyright 2017. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 *
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef TIMERINTERRUPT_H_
#define TIMERINTERRUPT_H_

#include "C_Extensions.h"
#include "uc.h"
#include "TimerInterrupt_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
// Main Methods
void TimerInterrupt__Initialize(void);
void TimerInterrupt__SetLongInterval(void);

uint32 TimerInterrupt__ConvertCyclesToUs(uint32 cycles);

#endif // TIMERINTERRUPT_H_

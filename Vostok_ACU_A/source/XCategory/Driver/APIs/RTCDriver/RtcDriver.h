/**
 *  @file       
 *
 *  @brief      Public Interface for RTC Driver Module for the Whirlpool strategy micro-controllers
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
#ifndef RTCDRIVER_H_
#define RTCDRIVER_H_
#include "C_Extensions.h"
#include "RtcDriver_prm.h"
#include "RtcDriver_Defs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void RtcDriver__Initialize(void);
void RtcDriver__SetCalibration(uint32 calib);
uint32 RtcDriver__GetCalibration(void);

void RtcDriver__Handler(void);
uint32 RtcDriver__GetEpochTime(void);
void RtcDriver__SetEpochTime(uint32 epoch_time);

void RtcDriver__Interrupt(void);

#endif // RTCDRIVER_H_

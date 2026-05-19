/**
 *  @file
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef PUMPFLUSHREQUESTMONITOR_H_
#define PUMPFLUSHREQUESTMONITOR_H_

#include "C_Extensions.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void PumpFlushRequestMonitor__Initialize(void);
void PumpFlushRequestMonitor__Execution(void);
BOOL_TYPE PumpFlushRequestMonitor__GetWashPumpCtrlExecutedFlag(void);
BOOL_TYPE PumpFlushRequestMonitor__GetDrainPumpCtrlExecutedFlag(void);
void  PumpFlushRequestMonitor__ResetWashPumpCtrlExecutedFlag(void);
void  PumpFlushRequestMonitor__ResetDrainPumpCtrlExecutedFlag(void);
void  PumpFlushRequestMonitor__SetWashPumpCtrlExecutedFlag(void);
void  PumpFlushRequestMonitor__SetDrainPumpCtrlExecutedFlag(void);
BOOL_TYPE PumpFlushRequestMonitor__GetWashStopSentFlag(void);
BOOL_TYPE PumpFlushRequestMonitor__GetDrainStopSentFlag(void);
void  PumpFlushRequestMonitor__SetWashStopSentFlag(void);
void  PumpFlushRequestMonitor__SetDrainStopSentFlag(void);
void  PumpFlushRequestMonitor__ResetWashStopSentFlag(void);
void  PumpFlushRequestMonitor__ResetDrainStopSentFlag(void);

#endif // PUMPFLUSHREQUESTMONITOR_H_

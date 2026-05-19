/**
 * @file
 * @brief       Public interface to the code generated Plugging_Model module.
 *
 * @details     Refer to the PluggingMonitor.c source file for more detailed information.
 *
 * $Header: $
 *
 * @copyright  Copyright 2018-2019. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

#ifndef PLUGGINGMONITOR_H_
#define PLUGGINGMONITOR_H_


//  --- Include Files -------------------------------------------------------------------------------------------------
#include "SystemConfig.h"

// -- This Module --
#include "Plugging_Model.h"
#include "PluggingMonitor_def.h"
#include "PluggingMonitor_prv.h"

// -- Other Modules --
#include "SystemTimers.h"
#include "Log.h"
#include "SettingFile.h"

//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================
// -- Public Constant Declarations --

// -- Public Type Declarations --

//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================

void PluggingMonitor__Initialize(void);
void PluggingMonitor__Execution(void);
void PluggingMonitor__Start(void);
void PluggingMonitor__Stop(void);
void PluggingMonitor__Pause(void);
void PluggingMonitor__Resume(void);
void PluggingMonitor__Reset(void);

PLUGGING_STATUS_TYPE PluggingMonitor__GetStatus(void);

SYSTEMTIMERS_STATE_TYPE PluggingMonitor__GetTimerState(void);
PLUGGING_MODEL_DATA_TYPE PluggingMonitor__GetModelData(void);
BOOL_TYPE Plugging__GetResetStatus(void);
void PluggingMonitor__SetRiskDelayTimer(uint32 PluggingTimeout);
void PluggingMonitor__SetStatus(PLUGGING_STATUS_TYPE PluggingStatus);
void PluggingMonitor__SetModelData(PLUGGING_MODEL_DATA_TYPE PluggingData);

#endif      /* PLUGGINGMONITOR_H_ */

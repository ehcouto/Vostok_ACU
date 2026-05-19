/**
 *  @file       SensedDrainProcess.h
 *
 *  @brief      Model Based Control Design code integration wrapper template for a Nucleus Process.
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *
 *
 *  @copyright  Copyright 2017-2018. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
#ifndef SENSEDDRAINPROCESS_H_
#define SENSEDDRAINPROCESS_H_

#include "SystemConfig.h"

#include "SensedDrainProcess_def.h"

#include "Process.h"
#include "Log.h"
#include "SettingFile.h"
#include "SystemTimers.h"
#include "Vwi.h"
#include "Fault.h"
#include "Hbl.h"
#include "string.h"

//=====================================================================================================================
//--------------------------------------------------- MACROS ----------------------------------------------------------
//=====================================================================================================================

/**
 * The model CONTROL_TYPE & SETTING_FILE_DATA MACRO.
 * Wrapper #defines Model MACRO's
 * 
 */
 // Use this section to define all macros that will connect to the model
 
//=====================================================================================================================
//-------------------------------------- Public Variables -------------------------------------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

// General Set functions
void SensedDrain__SetSensedDrainStatus(PASS_FAIL_TYPE senseddrain_status);
void SensedDrain__SetModelData(SENSEDDRAIN_MODELDATA_TYPE modeldata);
void SensedDrain__SetFault(FAULT_ID_TYPE fault_type);
void SensedDrain__SetDrainConfirmed(BOOL_TYPE drain_confirmed);
void SensedDrain__SetWaterInformation(uint8 water_info);
void SensedDrain__ClearFault(FAULT_ID_TYPE fault_type);

// General Get functions
SENSEDDRAIN_MODELDATA_TYPE SensedDrain__GetModelData(void);
BOOL_TYPE SensedDrain__GetDrainConfirmed(void);
uint8 SensedDrain__GetInitialDrainValue(void);

// Timer Set Functions
void SensedDrain__SetTimerAsReset(SENSEDDRAIN_TIMERS_TYPE timer_type);
void SensedDrain__SetTimerAsStart(SENSEDDRAIN_TIMERS_TYPE timer_type);

// Motor Set Functions
void SensedDrain__SetWashMotorState(ON_OFF_TYPE motor_state);
void SensedDrain__SetDrainMotorDefaultSpeedState(ON_OFF_TYPE motor_state);
void SensedDrain__SetDrainMotorEmptySpeedState(ON_OFF_TYPE motor_state);
void SensedDrain__RequestMotorStatus(uint8 motor_index);

// Main Interface
PROCESS_RESULT_TYPE SensedDrain__Process(PROCESS_CONTROL_TYPE* control);
void SensedDrain__Initialize(void);

#endif  //SENSEDDRAINPROCESS_H_

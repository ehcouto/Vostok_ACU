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
#ifndef SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_PROCESSES_PUMPCONTROL_PUMPCONTROL_H_
#define SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_PROCESSES_PUMPCONTROL_PUMPCONTROL_H_

#include "Process.h"
#include "Log.h"
#include "SettingFile.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#define MAX_WAIT_STOP_ATTEMPS 5
#define MAX_WAIT_STATUS_ATTEMPS 5
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void PumpControl__Initialize(void);
PROCESS_RESULT_TYPE PumpControl__Process(PROCESS_CONTROL_TYPE* control);
void PumpControl__SetPreviousWashMotorSpeed(uint16 speed);
void PumpControl__SetPreviousDrainMotorSpeed(uint16 speed);
uint16 PumpControl__GetPreviousWashMotorSpeed(void);
uint16 PumpControl__GetPreviousDrainMotorSpeed(void);
#if (API221_CLIENT_FEATURE == ENABLED)
void PumpControl__UnregisterApi221Events(void);
#endif
BOOL_TYPE PumpControl__GetVSMDrainIsRunning(void);
void PumpControl__SetVSMDrainIsRunning(BOOL_TYPE value);
void PumpControl__McuReset(uint8 node, uint8 reason, uint8 reset_mode);
void PumpControl__SetWashPumpDeblockingReady(BOOL_TYPE value);
void PumpControl__SetDrainPumpDeblockingReady(BOOL_TYPE value);
#endif /* SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_PROCESSES_PUMPCONTROL_PUMPCONTROL_H_ */


/**
 * @file        PartialDrainProcess.h
 * @brief       Public interface to the code generated PartialDrainModel module.
 *
 * @details     Refer to PartialDrainProcess.c source file for more detailed information.
 *
 * $Header: $
 *
 * @copyright  Copyright 2018-2019. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

#ifndef PARTIALDRAINPROCESS_H_
#define PARTIALDRAINPROCESS_H_

#include "SystemConfig.h"

//  --- Include Files -------------------------------------------------------------------------------------------------
#include "Process.h"
#include "Vwi.h"
#include "SystemTimers.h"

//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================

// -- Public Definitions --

//! Default data for machine configuration
#define PARTIALDRAIN_DEFAULT_LITERSMULTIPLIER 		5345
#define PARTIALDRAIN_DEFAULT_STARTINGOFFSET			2352
#define PARTIALDRAIN_DEFAULT_INITIALVALUEMULTIPLIER ((sint8)(-0.01*200.0f))
#define PARTIALDRAIN_DEFAULT_INITIALVALUEOFFSET 	68
#define PARTIALDRAIN_DEFAULT_INITIALVALUEACQTIME 	5
#define PARTIALDRAIN_DEFAULT_INITIALVALUEAVGSTR 	3
#define PARTIALDRAIN_DEFAULT_INITIALVALUECOMP		(1*100)
#define PARTIALDRAIN_DEFAULT_ATTEMPTTIMERSEC 		60
#define PARTIALDRAIN_DEFAULT_VWISTABLETIMERSEC 		30
#define PARTIALDRAIN_DEFAULT_DRAINPUMPSPEED 		(3500/25)
#define PARTIALDRAIN_DEFAULT_DRAINPUMPACCEL 		1500

// -- Public Type Declarations --

typedef enum PARTIALDRAIN_MODES_ENUM
{
	PARTIALDRAIN_AMOUNT_REMAINING = 0,
	PARTIALDRAIN_SPECIFIED_AMOUNT,
} PARTIALDRAIN_MODES;

typedef struct PARTIALDRAIN_MODELDATA_STRUCT
{
	sint32 Drain_Integrator;
	uint32 Drain_Integrator_Count;
	sint16 Integrator_CorrectionFactor;
} PARTIALDRAIN_MODELDATA_TYPE;

typedef PACKED struct SF_PARTIALDRAIN_MACHINECONFIG_STRUCT
{
	uint16 Liters_Multiplier;
	sint16 Starting_Offset;
	sint8 Initial_Value_Multiplier;
	uint8 Initial_Value_Offset;
	uint8 Initial_Value_Acquisition_Time;
	uint8 Initial_Value_Average_Strenght;
	sint8 Initial_Value_Compensation;
	uint8 Attempt_Time_Sec;
	uint8 VwiStable_Time_Sec;
	uint8 Drain_Pump_Speed;
	uint16 Drain_Pump_Acceleration;
} SF_PARTIALDRAIN_MACHINECONFIG_TYPE;

typedef PACKED struct SF_PARTIALDRAIN_PROCESSCONFIG_STRUCT
{
	PARTIALDRAIN_MODES PartialDrain_Mode;
	uint8 PartialDrain_Liters;
} SF_PARTIALDRAIN_PROCESSCONFIG_TYPE;

typedef enum MODULE_PARTIALDRAIN_LOG_MESSAGE_ID_ENUM
{
   PARTIALDRAIN_ERROR_PROCESS_CONTROL = 1,
   PARTIALDRAIN_ERROR_PROCESS_PARAMETERS,
   PARTIALDRAIN_ERROR_MOTOR_INVALID

} MODULE_PARTIALDRAIN_LOG_MESSAGE_ID_TYPE;

//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================

void PartialDrain__Initialize(void);
PROCESS_RESULT_TYPE PartialDrain__Process(PROCESS_CONTROL_TYPE* control);

// Get Functions
PROCESS_RESULT_TYPE PartialDrainProcess__GetProcessResult(void); //TODO: remove this function
uint16 PartialDrainProcess__GetLitersInTub(void);
uint8 PartialDrainProcess__GetDrainInitialValue(void);
uint8 PartialDrainProcess__GetVwiSignal(void);
VWI_WATER_SENSE_TYPE PartialDrainProcess__GetVwiStatus(void);
SYSTEMTIMERS_STATE_TYPE PartialDrainProcess__GetAttemptTimerState(void);
SYSTEMTIMERS_STATE_TYPE PartialDrainProcess__GetVwiStableTimerState(void);
BOOL_TYPE PartialDrainProcess__GetDrainPumpReachedSpeed(void);
BOOL_TYPE PartialDrainProcess__GetDrainPumpIsStopped(void);

// Set Functions
void PartialDrainProcess__SetAttemptTimer(ON_OFF_TYPE timer_mode);
void PartialDrainProcess__SetVwiStableTimer(ON_OFF_TYPE timer_mode);
void PartialDrainProcess__SetDrainPumpMode(ON_OFF_TYPE pump_mode);
void PartialDrainProcess__SendFbkRequest(BOOL_TYPE clear_count);

// Extern Data
extern PASS_FAIL_TYPE Partial_Drain_Status;

#endif      /* PARTIALDRAINPROCESS_H_ */

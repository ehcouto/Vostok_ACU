/**
 * @file
 * @brief       Public definition of the data structure used by the Sensed Drain process and supporting modules.
 *
 * @copyright   Copyright 2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef SENSEDDRAINPROCESS_DEF_H_
#define SENSEDDRAINPROCESS_DEF_H_

#include "SystemConfig.h"

#include "HblDefs.h"
#include "SystemTimers.h"
#include "Vwi.h"
#include "Fault.h"

typedef enum SENSEDDRAIN_PROCESS_OPTIONS_ENUM
{
   PROCESS_SENSEDDRAIN,		//!< F1=0, F0=0
   PROCESS_PARTIALDRAIN,    //!< F1=0, F0=1
} SENSEDDRAIN_PROCESS_OPTIONS_TYPE;

typedef enum MODULE_SENSEDDRAIN_LOG_MESSAGE_ID_ENUM
{
   SENSEDDRAIN_ERROR_PROCESS_CONTROL = 1,
   SENSEDDRAIN_ERROR_PROCESS_PARAMETERS,
   SENSEDDRAIN_ERROR_MOTOR_INVALID,
   SENSEDDRAIN_ERROR_PARTIALDRAIN_PARAMETERS,

} MODULE_SENSEDDRAIN_LOG_MESSAGE_ID_TYPE;


typedef enum SENSEDDRAIN_DM_SPEED_ENUM
{
	DM_SPEED_DEFAULT = 0,
	DM_SPEED_EMPTY   = 1,

} SENSEDDRAIN_DM_SPEED_TYPE;


typedef enum SENSEDDRAIN_TIMERS_ENUM
{
	RETRYPAUSE_TIMER = 0,
	DM_INITIAL_TIMER,
	DM_VWISTABLE_TIMER,
	WM_NOCHANGE_TIMER,
	DM_ATTEMPT_TIMER,
	WM_ATTEMPT_TIMER,
	EMPTYDRAIN_TIMER,

} SENSEDDRAIN_TIMERS_TYPE;


typedef struct SENSEDDRAIN_MOTORDATA_STRUCT
{
	HBL_PILOT_ENUM_TYPE Wash_Pump_Type;
	HBL_PILOT_ENUM_TYPE Drain_Pump_Type;
	BOOL_TYPE WashPump_Is_Stopped;
	BOOL_TYPE DrainPump_Is_Stopped;
	BOOL_TYPE WashPump_Reached_Target_Speed;
	BOOL_TYPE DrainPump_Reached_Target_Speed;

} SENSEDDRAIN_MOTORDATA_TYPE;


typedef struct SENSEDDRAIN_TIMERSTATE_STRUCT
{
	SYSTEMTIMERS_STATE_TYPE SensedDrain_RetryPause_Timer_State;
	SYSTEMTIMERS_STATE_TYPE SensedDrain_DM_Initial_Timer_State;
	SYSTEMTIMERS_STATE_TYPE SensedDrain_DM_VwiStable_Timer_State;
	SYSTEMTIMERS_STATE_TYPE SensedDrain_WM_NoChange_Timer_State;
	SYSTEMTIMERS_STATE_TYPE SensedDrain_DM_Attempt_Timer_State;
	SYSTEMTIMERS_STATE_TYPE SensedDrain_WM_Attempt_Timer_State;
	SYSTEMTIMERS_STATE_TYPE SensedDrain_EmptyDrain_Timer_State;

} SENSEDDRAIN_TIMERSTATE_TYPE;


typedef struct SENSEDDRAIN_VWIINFO_STRUCT
{
	VWI_WATER_SENSE_TYPE Vwi_Status;
	uint16 Vwi_Heavy_Filt_Signal;

} SENSEDDRAIN_VWIINFO_TYPE;


typedef struct SENSEDDRAIN_MODELDATA_STRUCT
{
	// Motor Commands
	ON_OFF_TYPE Drain_Pump_Mode;
	ON_OFF_TYPE Wash_Pump_Mode;
	SENSEDDRAIN_DM_SPEED_TYPE Drain_Pump_Speed_Type;

	// Stability Analysis
	BOOL_TYPE Vwi_Stability_Status;
	uint8 Vwi_Previous;

	// Retry Attempts
	uint8 Attempt_Counter;

	// Fault
	FAULT_ID_TYPE Fault_Type;

	// Drain Flag
	//BOOL_TYPE Drain_Confirmed;

} SENSEDDRAIN_MODELDATA_TYPE;


// Setting File variables
typedef PACKED struct SF_SENSEDDRAIN_DATA_STRUCT
{
	/* Maximum times sensed drain can retry for successful execution */
	uint8 Max_Drain_Attempts;

	/* Maximum time sensed drain can execute when running wash motor */
	uint8 WM_Attempt_Time;

	/* Maximum time sensed drain can wait for VWI no change detection before exit */
	uint8 WM_NoChange_Time;

	/* VWI value below which sensed drain will look for no change detection */
	uint8 Vwi_Drained_Threshold;

	/* Default acceleration information for wash motor pump VSM */
	uint16 WM_Pump_Acceleration;

	/* Default speed information for wash motor pump VSM */
	uint8 WM_Pump_Speed;

	/* Time to pause before resume draining after a retry */
	uint8 Retry_Pause_Time;

	/* Maximum difference between two Vwi readings to consider the signal as stable */
	uint8 Vwi_Stable_Max_Difference;

	/* Default speed information for drain motor pump VSM */
	uint8 DM_Default_Pump_Speed;

	/* Default acceleration information for drain motor pump VSM */
	uint16 DM_Default_Pump_Acceleration;

	/* Acceleration information for drain motor pump VSM when empty */
	uint16 DM_Empty_Pump_Acceleration;

	/* Speed information for drain motor pump VSM when empty */
	uint8 DM_Empty_Pump_Speed;

	/* Maximum time sensed drain can execute when running drain motor */
	uint8 DM_Attempt_Time;

	/* Initial draining time before starting attempt timer - 2VSM mode only */
	uint8 DM_Initial_Time;

	/* Time to keep draining after the Vwi is in NO_WATER state - 2VSM mode only */
	uint8 DM_VwiStable_Time;

	/* Time to keep draining after the machine is already empty - 2VSM mode only */
	uint8 EmptyDrain_Time;

	/* Setting file filler */
	uint8 empty;

} SF_SENSEDDRAIN_DATA_TYPE;


// Setting File modifiers
typedef PACKED struct SF_SENSEDDRAIN_PROCESS_DATA_STRUCT
{
	/* Speed modifier for wash motor pump VSM */
	uint8 WM_Speed_Modifier;

	/* Acceleration modifier for wash motor pump VSM */
	uint8 WM_Acceleration_Modifier;

	/* Modifier for default speed in drain motor pump VSM */
	uint8 DM_Default_Speed_Modifier;

	/* Modifier for default acceleration in drain motor pump VSM */
	uint8 DM_Default_Acceleration_Modifier;

	/* No Fault modifier used to skip showing the not draining fault to the consumer */
	BOOL_TYPE No_Fault;

	/* Padding (0) for SF alignment */
	uint8 empty;

} SF_SENSEDDRAIN_PROCESS_DATA_TYPE;

//-------------------------------------- Unused structures, only for size comparison --------------------------------------

// No VSM SF
typedef PACKED struct SF_SENSEDDRAIN_NOVSM_STRUCT
{
	/* Maximum times sensed drain can retry for successful execution */
	uint8 Max_Drain_Attempts;

	/* Maximum time sensed drain can execute when running wash motor */
	uint8 WM_Attempt_Time;

	/* Maximum time sensed drain can wait for VWI no change detection before exit */
	uint8 WM_NoChange_Time;

	/* VWI value below which sensed drain will look for no change detection */
	uint8 Vwi_Drained_Threshold;

	/* Default acceleration information for wash motor pump VSM */
	uint16 WM_Pump_Acceleration;

	/* Default speed information for wash motor pump VSM */
	uint8 WM_Pump_Speed;

	/* Time to pause before resume draining after a retry */
	uint8 Retry_Pause_Time;

	/* Maximum difference between two Vwi readings to consider the signal as stable */
	uint8 Vwi_Stable_Max_Difference;

	/* Setting file filler */
	uint8 empty;

} SF_SENSEDDRAIN_NOVSM_TYPE;

// Old SF - 8 bytes
typedef PACKED struct SF_SENSEDDRAIN_OLDDATA_STRUCT
{
    /* Maximum times sensed drain can retry for successful execution */
    uint8 Sf_Sensed_Drain_MAX_Retry;

    /* Maximum time sensed drain can execute. */
    uint8 Sf_Sensed_Drain_MAX_Time;

    /* Maximum time sensed drain can wait for VWI no change detection before exit. */
    uint8 Sf_Sensed_Drain_VWI_NoChange_Max_Time;

    /* VWI value below which sensed drain will look for no change detection */
    uint8 Sf_Sensed_Drain_VWI_NoChange_Threshold;

    /* Acceleration information for pump VSM motors */
   	uint16 Sf_Sensed_Drain_Pump_Acceleration;

    /* Speed information for pump VSM motors */
    uint8 Sf_Sensed_Drain_Pump_Speed;

    /* Setting file filler */
    uint8 empty;

}SF_SENSEDDRAIN_OLDDATA_TYPE;

// Process Modifier Positions
typedef PACKED struct SF_SENSEDDRAIN_PROCESS_POS_STRUCT
{
    uint8 Pos1;
    uint8 Pos2;
    uint8 Pos3;
    uint8 Pos4;
    uint8 Pos5;
    uint8 Pos6;
}SF_SENSEDDRAIN_PROCESS_POS_TYPE;

#endif /* SENSEDDRAINPROCESS_DEF_H_ */

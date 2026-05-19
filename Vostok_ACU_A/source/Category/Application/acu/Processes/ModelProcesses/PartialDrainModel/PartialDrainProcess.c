/**
 *  @file		PartialDrainProcess.c
 *
 *  @brief      Wrapper for the code generated PartialDrainModel
 *
 *  @details
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2018-2019. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "PartialDrainProcess.h"
#include "PartialDrainModel.h"
#include "Log.h"
#if (API221_CLIENT_FEATURE == ENABLED)
#include "API221MotionCtrl.h"
#endif
#include "Hbl.h"
#include "Variable.h"
#include "string.h"
#include "Modifier.h"
#include "PumpCommand.h"
#include "SensedDrainProcess.h"
#include "SettingFile.h"
#include "PumpFlushRequestMonitor.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//! Pointer to the "variable" data in the setting file for machine configuration
static SF_PARTIALDRAIN_MACHINECONFIG_TYPE* Sf_PartialDrain_MachineConfig = (SF_PARTIALDRAIN_MACHINECONFIG_TYPE*)NULL;

//! Pointer to the "variable" data in the setting file for process configuration
static SF_PARTIALDRAIN_PROCESSCONFIG_TYPE* Sf_PartialDrain_ProcessConfig = (SF_PARTIALDRAIN_PROCESSCONFIG_TYPE*)NULL;

//! Timer Handlers
static SYSTEMTIMER_NV_TIMER_TYPE PartialDrain_AttemptTimer_Handler;
static SYSTEMTIMER_NV_TIMER_TYPE PartialDrain_VwiStableTimer_Handler;

//! Real Time Model Data
#pragma location = "_appliance_segment"
static RT_MODELPARTIALDRAINMODEL_TYPE PartialDrain_RTModelData; 	/* Real-time model */
static RT_MODELPARTIALDRAINMODEL_TYPE* PartialDrain_RTModelDataPrt = &PartialDrain_RTModelData; 	/* Real-time model */
static DWPARTIALDRAINMODEL_TYPE PartialDrain_ObservableStates;    	/* Observable states */

//! Model Data - Debug information only
static PARTIALDRAIN_MODELDATA_TYPE PartialDrainData;

//! Timer Values - For platform logging only
static uint32 PartialDrain_AttemptTimer_Value;
static uint32 PartialDrain_VwiStableTimer_Value;
static SYSTEMTIMERS_STATE_TYPE PartialDrain_AttemptTimer_State;
static SYSTEMTIMERS_STATE_TYPE PartialDrain_VwiStableTimer_State;


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//! Process Information
static PROCESS_RESULT_TYPE PartialDrainProcess_StatusResult = PROCESS_COMPLETE;

//! Displacement value that indicates that the feature is not used.

//! The name of this module for use by the Log.c
#define MODULE_NAME MODULE_PARTIALDRAIN

//! Default data for machine configuration
static SF_PARTIALDRAIN_MACHINECONFIG_TYPE Sf_PartialDrain_MachineDefault = {
		PARTIALDRAIN_DEFAULT_LITERSMULTIPLIER, 			// Liters_Multiplier
		PARTIALDRAIN_DEFAULT_STARTINGOFFSET,			// Starting_Offset
		PARTIALDRAIN_DEFAULT_INITIALVALUEMULTIPLIER, 	// Initial_Value_Multiplier
		PARTIALDRAIN_DEFAULT_INITIALVALUEOFFSET, 		// Initial_Value_Offset
		PARTIALDRAIN_DEFAULT_INITIALVALUEACQTIME, 		// Initial_Value_Acquisition_Time
		PARTIALDRAIN_DEFAULT_INITIALVALUEAVGSTR, 		// Initial_Value_Average_Strenght
		PARTIALDRAIN_DEFAULT_INITIALVALUECOMP,			// Initial_Value_Compensation
		PARTIALDRAIN_DEFAULT_ATTEMPTTIMERSEC, 			// Attempt_Time_Sec
		PARTIALDRAIN_DEFAULT_VWISTABLETIMERSEC,	 		// VwiStable_Time_Sec
		PARTIALDRAIN_DEFAULT_DRAINPUMPSPEED, 			// Drain_Pump_Speed
		PARTIALDRAIN_DEFAULT_DRAINPUMPACCEL 			// Drain_Pump_Acceleration
};

//! Default structure for process modifiers
static SF_PARTIALDRAIN_PROCESSCONFIG_TYPE Sf_PartialDrain_ProcessDefault = {
		PARTIALDRAIN_AMOUNT_REMAINING,			// PartialDrain_Mode
		2*10,									// PartialDrain_Liters (resolution 0.1)
};

//! Motor Flags
static BOOL_TYPE DrainPump_ReachedTargetSpeed = FALSE;
static BOOL_TYPE DrainPump_IsStopped = FALSE;

//! Motor Decrement Counter
static uint8 DrainCommand_Counter = 0;
static ON_OFF_TYPE DrainCommand_Previous = OFF;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

static void ReadParameters(PROCESS_CONTROL_TYPE* control);
static void DrainCommand_Decrement(ON_OFF_TYPE command, sint16 speed, uint16 acceleration);

static HBL_PILOT_ENUM_TYPE Get_DrainHBLType(void);

#if (API221_CLIENT_FEATURE == ENABLED)
static void API221ApplReceivedStatus (API221_CLIENT_MOTOR_INDEX_TYPE motor, uint32 status);
static void API221HandleMotorEvent (API221_EVENT_TYPE event);
#endif

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

PROCESS_RESULT_TYPE PartialDrainProcess__GetProcessResult(void)
{
	return PartialDrainProcess_StatusResult;
}

//!> Get Functions
uint16 PartialDrainProcess__GetLitersInTub(void)
{
	sint32 liters_in_tub;
	uint16 ret_value;
	if ((uint8)Variable__GetValue(VARIABLE_LITERS_IN_TUB, 0, &liters_in_tub) == (uint8)TRUE)
	{
	    ret_value = (uint16)liters_in_tub; // cL value
	}
	else
	{
	    ret_value = 0;
	}
	return ret_value;
}

uint8 PartialDrainProcess__GetDrainInitialValue(void)
{
	return SensedDrain__GetInitialDrainValue();
}

uint8 PartialDrainProcess__GetVwiSignal(void)
{
	return (uint8)Vwi__GetSignal();
}

VWI_WATER_SENSE_TYPE PartialDrainProcess__GetVwiStatus(void)
{
	return Vwi__GetWaterStatus();
}
SYSTEMTIMERS_STATE_TYPE PartialDrainProcess__GetAttemptTimerState(void)
{
	return SystemTimers__NVCheckTimer(&PartialDrain_AttemptTimer_Handler);
}

SYSTEMTIMERS_STATE_TYPE PartialDrainProcess__GetVwiStableTimerState(void)
{
	return SystemTimers__NVCheckTimer(&PartialDrain_VwiStableTimer_Handler);
}

BOOL_TYPE PartialDrainProcess__GetDrainPumpReachedSpeed(void)
{
	return DrainPump_ReachedTargetSpeed;
}

BOOL_TYPE PartialDrainProcess__GetDrainPumpIsStopped(void)
{
	return DrainPump_IsStopped;
}

//!> Set Functions
void PartialDrainProcess__SetAttemptTimer(ON_OFF_TYPE timer_mode)
{
	uint32 time_ms = Sf_PartialDrain_MachineConfig->Attempt_Time_Sec * (uint32)1000;
	if (timer_mode == ON)
	{
		(void)SystemTimers__NVSetTimerMS(&PartialDrain_AttemptTimer_Handler, time_ms, SYSTEMTIMERS_TYPE_DOWN_COUNTER);
	}
}
void PartialDrainProcess__SetVwiStableTimer(ON_OFF_TYPE timer_mode)
{
	uint32 time_ms = Sf_PartialDrain_MachineConfig->VwiStable_Time_Sec * (uint32)1000;
	if (timer_mode == ON)
	{
		(void)SystemTimers__NVSetTimerMS(&PartialDrain_VwiStableTimer_Handler, time_ms, SYSTEMTIMERS_TYPE_DOWN_COUNTER);
	}
}

void PartialDrainProcess__SetDrainPumpMode(ON_OFF_TYPE pump_mode)
{
	sint16 speed;
	uint16 acceleration;

	if (Get_DrainHBLType() != HBL_PILOT_INVALID)
	{
		if (Get_DrainHBLType() == HBL_PILOT_EXTERNAL_MOTOR) // VSM Motor
		{
			// Get speed from SF
			speed = (sint16)Sf_PartialDrain_MachineConfig->Drain_Pump_Speed * 25;
			acceleration = Sf_PartialDrain_MachineConfig->Drain_Pump_Acceleration;

			// Send Motor Command
			DrainCommand_Decrement(pump_mode, speed, acceleration);
		}
		else // Single speed motor
		{
			(void)Hbl__SetLoadRequestByLoadID(HBL_LOAD_DRAIN_PUMP, HBL_PRIORITY_APPLICATION_HI, (uint8 *) &pump_mode); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
		}
	}
}

void PartialDrainProcess__SendFbkRequest(BOOL_TYPE clear_count)
{
	#if (API221_CLIENT_FEATURE == ENABLED)
	(void)API221MotionCtrl__RequestStatus(MOTOR_1, API221ApplReceivedStatus);
	#endif
}

//----------------------------------------------------------------------------
/**
 *  @brief      PartialDrainProcess__Initialize
 *  @details    Function used to initialize the process.
 *
 */
void PartialDrain__Initialize()
{
	PartialDrain_RTModelDataPrt->dwork = &PartialDrain_ObservableStates; /* Pack model data into RTM */

	// The following code suppress "not used variable" warning from this file, but has no functionality purpose.
	(void)PartialDrainData;
	(void)PartialDrain_AttemptTimer_Value;
	(void)PartialDrain_VwiStableTimer_Value;
	(void)PartialDrain_AttemptTimer_State;
	(void)PartialDrain_VwiStableTimer_State;

}

//----------------------------------------------------------------------------
/**
 *  @brief      PartialDrainProcess__Execution
 *  @details    Function used to perform the process.
 *
 */
PROCESS_RESULT_TYPE PartialDrain__Process(PROCESS_CONTROL_TYPE* control)
{
	//! Initializing
	if (((uint8)(control->Step_Initializing) == (uint8)TRUE) ||
	        ((uint8)(control->Resume_From_Reset) == (uint8)TRUE) ||
	        ((uint8)(control->Resume_From_Pause) == (uint8)TRUE))
	{
		// Load Data from Settings File
		ReadParameters(control);

		// Initialize PumpCommand_Previous variables
		DrainCommand_Previous = OFF;

		// Clear motor flags
		DrainPump_ReachedTargetSpeed = FALSE;
		DrainPump_IsStopped = FALSE;

		#if (API221_CLIENT_FEATURE == ENABLED)
		API221MotionCtrl__RegisterForEvents(API221HandleMotorEvent); // Register for API221 Events
		#endif
	}

	if ((uint8)control->Step_Initializing == (uint8)TRUE)
	{
		// Initialize model
		PartialDrainModel__Initialize(PartialDrain_RTModelDataPrt, &PartialDrainProcess_StatusResult, &Partial_Drain_Status);

		// Reset all state machine states
		memset(&PartialDrain_ObservableStates, 0, sizeof(PartialDrain_ObservableStates));
	}

	//! Executing
	if ((Sf_PartialDrain_MachineConfig != NULL) && (Sf_PartialDrain_ProcessConfig != NULL))
	{

		// Update Timer Values for logging purposes
		PartialDrain_AttemptTimer_Value = SystemTimers__NVGetTimerMs(&PartialDrain_AttemptTimer_Handler);
		PartialDrain_VwiStableTimer_Value = SystemTimers__NVGetTimerMs(&PartialDrain_VwiStableTimer_Handler);
		PartialDrain_AttemptTimer_State = SystemTimers__NVCheckTimer(&PartialDrain_AttemptTimer_Handler);
		PartialDrain_VwiStableTimer_State = SystemTimers__NVCheckTimer(&PartialDrain_VwiStableTimer_Handler);

		if (Get_DrainHBLType() != HBL_PILOT_INVALID)
		{
			if (Get_DrainHBLType() != HBL_PILOT_EXTERNAL_MOTOR) // VSM Motor
			{
				DrainPump_ReachedTargetSpeed = TRUE; // Single speeds ramp up fast, so no need to wait for reached target speed
				DrainPump_IsStopped = (((uint8*)(Hbl__GetLoadStatusByLoadID(HBL_LOAD_WASH_PUMP)))[0] == (uint8)OFF) ? TRUE:FALSE;
			}

			// Run Model
			PartialDrainModel__Execute(PartialDrain_RTModelDataPrt, control, Sf_PartialDrain_MachineConfig,
					Sf_PartialDrain_ProcessConfig, &PartialDrainProcess_StatusResult, &Partial_Drain_Status);

			PartialDrainData = PartialDrain_ObservableStates.partialdraindata;
		}
		else
		{
			LOG_ADD_EXCEPTION(PARTIALDRAIN_ERROR_MOTOR_INVALID, HBL_LOAD_DRAIN_PUMP);
		}
	}
	else
	{
		LOG_ADD_EXCEPTION(PARTIALDRAIN_ERROR_PROCESS_PARAMETERS, (Sf_PartialDrain_MachineConfig == NULL));
	}

	#if (API221_CLIENT_FEATURE == ENABLED)
	// Advise Pump Flush Request Monitor
	if (Get_DrainHBLType() == HBL_PILOT_EXTERNAL_MOTOR)
	{
		PumpFlushRequestMonitor__SetDrainPumpCtrlExecutedFlag();
	}

	if (PartialDrainProcess_StatusResult == PROCESS_COMPLETE)
	{
		API221MotionCtrl__UnregisterForEvents(API221HandleMotorEvent);
	}
	#endif

	return PartialDrainProcess_StatusResult;
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

/**
 * Read SF parameters for this module
 */
static void ReadParameters(PROCESS_CONTROL_TYPE* control)
{
	SETTINGFILE_LOADER_TYPE loader;

	// Get the parameters from the setting file machine structure.
	if (SettingFile__MSettingBasicLoader(control->Setting_File_Index,
										 SF_PTR_ACU_ALGORITHM_PARAMETERS,
										 DISPL_PARTIAL_DRAIN,
										 &loader) == FAIL)
	{
		LOG_ADD_EXCEPTION(PARTIALDRAIN_ERROR_PROCESS_PARAMETERS, control->Data_Field_Index);
	}
	else
	{
		Sf_PartialDrain_MachineDefault.Liters_Multiplier = ((SF_PARTIALDRAIN_MACHINECONFIG_TYPE*)(void*)(loader.Data))->Liters_Multiplier;
		Sf_PartialDrain_MachineDefault.Initial_Value_Multiplier = ((SF_PARTIALDRAIN_MACHINECONFIG_TYPE*)(void*)(loader.Data))->Initial_Value_Multiplier;
		Sf_PartialDrain_MachineDefault.Initial_Value_Offset = ((SF_PARTIALDRAIN_MACHINECONFIG_TYPE*)(void*)(loader.Data))->Initial_Value_Offset;
		Sf_PartialDrain_MachineDefault.Initial_Value_Acquisition_Time = ((SF_PARTIALDRAIN_MACHINECONFIG_TYPE*)(void*)(loader.Data))->Initial_Value_Acquisition_Time;
		Sf_PartialDrain_MachineDefault.Initial_Value_Average_Strenght = ((SF_PARTIALDRAIN_MACHINECONFIG_TYPE*)(void*)(loader.Data))->Initial_Value_Average_Strenght;
		Sf_PartialDrain_MachineDefault.Attempt_Time_Sec  = ((SF_PARTIALDRAIN_MACHINECONFIG_TYPE*)(void*)(loader.Data))->Attempt_Time_Sec;
		Sf_PartialDrain_MachineDefault.VwiStable_Time_Sec  = ((SF_PARTIALDRAIN_MACHINECONFIG_TYPE*)(void*)(loader.Data))->VwiStable_Time_Sec;
		Sf_PartialDrain_MachineDefault.Drain_Pump_Speed  = ((SF_PARTIALDRAIN_MACHINECONFIG_TYPE*)(void*)(loader.Data))->Drain_Pump_Speed;
		Sf_PartialDrain_MachineDefault.Drain_Pump_Acceleration  = ((SF_PARTIALDRAIN_MACHINECONFIG_TYPE*)(void*)(loader.Data))->Drain_Pump_Acceleration;
	}

	// Get the process parameters from the setting file.
	if (SettingFile__MSettingBasicLoader(control->Setting_File_Index,
										 SF_PTR_ACU_PROCESS_SENSED_DRAIN,
										 control->Data_Field_Index,
										 &loader) == FAIL)
	{
		LOG_ADD_EXCEPTION(PARTIALDRAIN_ERROR_PROCESS_PARAMETERS, control->Data_Field_Index);
	}
	else
	{
		Sf_PartialDrain_ProcessDefault.PartialDrain_Mode = ((SF_PARTIALDRAIN_PROCESSCONFIG_TYPE*)(void*)(loader.Data))->PartialDrain_Mode;
		Sf_PartialDrain_ProcessDefault.PartialDrain_Liters  = ((SF_PARTIALDRAIN_PROCESSCONFIG_TYPE*)(void*)(loader.Data))->PartialDrain_Liters;
	}

	Sf_PartialDrain_MachineConfig = &Sf_PartialDrain_MachineDefault;
	Sf_PartialDrain_ProcessConfig = &Sf_PartialDrain_ProcessDefault;
}

/**
 * Decrement Drain Motor Commands
 * Send motor commands every 2 seconds to avoid missing packages or filling the bus every 100ms.
 * @param command = ON or OFF.
 * @param speed = Motor Speed
 * @param acceleration = Motor Acceleration
 */
static void DrainCommand_Decrement(ON_OFF_TYPE command, sint16 speed, uint16 acceleration)
{
	// API 221 Command

	// Send a command at first if its different then the previous
	if (command != DrainCommand_Previous)
	{
		DrainCommand_Counter = 20;
	}

	// Increase counter in every run
	DrainCommand_Counter++;

	if (DrainCommand_Counter >= (uint8)20)
	{
		DrainCommand_Counter = 0;

		#if (API221_CLIENT_FEATURE == ENABLED)
		if (command == ON && !DrainPump_ReachedTargetSpeed)
		{
			API221MotionCtrl__DeviceRun(MOTOR_1, speed, acceleration);
		}
		else if (command == OFF && !DrainPump_IsStopped)
		{
			API221MotionCtrl__DeviceStop(MOTOR_1, acceleration);
		}
		#endif
	}

	DrainCommand_Previous = command;

	// PumpCommand - External Motor Pilot needs to know that the motor is running
	if (command == ON)
	{
		PumpCommand__Run(speed, acceleration, HBL_PRIORITY_APPLICATION_HI, HBL_LOAD_DRAIN_PUMP);
	}
	else
	{
		PumpCommand__Stop(acceleration, HBL_PRIORITY_APPLICATION_HI, HBL_LOAD_DRAIN_PUMP);
	}
}

/**
 * Get Drain HBL Type
 * @return HBL Drain Type
 */
static HBL_PILOT_ENUM_TYPE Get_DrainHBLType(void)
{
	HBL_PILOT_ENUM_TYPE pump_type = Hbl__GetPilotIDByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DRAIN_PUMP));
	return pump_type;
}

#if (API221_CLIENT_FEATURE == ENABLED)

/**
 * Handle events published through API221.
 * This function is called by the communication thread.
 * @param event = The event that was published through API221.
 */
static void API221HandleMotorEvent(API221_EVENT_TYPE event)
{
	switch (event.Event)
	{
		 case API221_EVENT_STOPPED:
		 	 {
				 if (event.Motor == MOTOR_1)
				 {
					 DrainPump_IsStopped = TRUE;
					 DrainPump_ReachedTargetSpeed = FALSE;
				 }
		 	 }
			break;
		 case API221_EVENT_REACHED_TARGET:
			 {
				 if (event.Motor == MOTOR_1)
				 {
					 DrainPump_IsStopped = FALSE;
					 DrainPump_ReachedTargetSpeed = TRUE;
				 }
			 }
			break;
		 case API221_EVENT_RAMP_STARTED:
			 {
				 if (event.Motor == MOTOR_1)
				 {
					 DrainPump_IsStopped = FALSE;
					 DrainPump_ReachedTargetSpeed = FALSE;
				 }
			 }
			 break;
		default:
			// Ignore all other events
			break;
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Use ExternalEventExtEvMCUErrReceive (RECEIVE_ERROR and RECEIVE_NO_ERROR)
 * Process information received from MCU about FF
 * @param motor = The motor ID related to the FF received
 * @param status = bitmap of FF
 */
static void API221ApplReceivedStatus (API221_CLIENT_MOTOR_INDEX_TYPE motor, uint32 status)
{
	if (motor == MOTOR_1) // Drain Motor
	{
		DrainPump_IsStopped = BIT_TEST(status,API221_STS_IS_MOTOR_STOPPED) ? (TRUE) : (FALSE);
		DrainPump_ReachedTargetSpeed = BIT_TEST(status,API221_STS_REACHED_TARGET) ? (TRUE) : (FALSE);
	}
}

#endif

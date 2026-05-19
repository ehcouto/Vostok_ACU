/**
 *  @file       SensedDrainProcess.c
 *
 *  @brief      Model Based Control Design code integration wrapper template for a Nucleus Process.
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *
 *
 *  @copyright  Copyright 2017-2018. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------

#include "SensedDrainProcess.h"
#include "SensedDrainModel.h"

#include "API221MotionCtrl.h"
#include "DishAuxVar.h"
#include "Micro.h"
#include "Modifier.h"
#include "NVData.h"
#include "PumpFlushRequestMonitor.h"
#include "Variable.h"
#include "Vwi.h"
#include "ExternalMotorDeclare.h"
#include "PumpCommand.h"
#include "PartialDrainProcess.h"
#include "Filter.h"
#include "Supervisor.h"

#define MODULE_NAME MODULE_SENSEDDRAIN      //!< The name of this module for use by the Log , add it in log.c

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//! Sensed Drain Flags for GESE

// Drain Confirmed Flag - To be edited in GESE
#pragma location = "_appliance_segment"
uint8 SensedDrain_DrainConfirmed; // important when running Sensed Drain process more then once in 2VSM situation

//! Displacement value that indicates that the feature is not used.
#define UNUSED_FEATURE 0xFF

// -- Private Variable Definitions --

//! Model Variables
static SENSEDDRAIN_MOTORDATA_TYPE SensedDrain_MotorData;
static SENSEDDRAIN_TIMERSTATE_TYPE SensedDrain_TimerState;
static SENSEDDRAIN_VWIINFO_TYPE SensedDrain_VwiInformation;

static RT_MODEL_SensedDrainModel_TYPE SensedDrain_RTModel;

#pragma location = "_appliance_segment"
static SENSEDDRAIN_MODELDATA_TYPE SensedDrain_ModelData;

//! Motor Decrement Counter
static uint8 DrainCommand_Counter = 0;
static uint8 WashCommand_Counter = 0;
static ON_OFF_TYPE DrainCommand_Previous = OFF;
static ON_OFF_TYPE WashCommand_Previous = OFF;
static sint16 DrainSpeed_Previous = 0;

//! Setting File Variables

// Pointer to the "variable" data in the setting file for machine configuration
static SF_SENSEDDRAIN_DATA_TYPE* Sf_SensedDrain_Config = (SF_SENSEDDRAIN_DATA_TYPE*)NULL; //Sf_SensedDrain_Config pointer include the header where the data type is defined

// Process parameters data in setting file
static SF_SENSEDDRAIN_PROCESS_DATA_TYPE SensedDrain_Process_Parameters;

// Default structure for setting file
static SF_SENSEDDRAIN_DATA_TYPE Sf_SensedDrain_Default = {
		3,			// Max_Drain_Attempts
		120,		// WM_Attempt_Time
		10,			// WM_NoChange_Time
		40,			// Vwi_Drained_Thereshold
		500,		// WM_Pump_Acceleration
		2000/25, 	// WM_Pump_Speed
		5,			// Retry_Pause_Time
		1,			// Vwi_Stable_Max_Difference
		3500/25,	// DM_Default_Pump_Speed
		1500,		// DM_Default_Pump_Acceleration
		1500,		// DM_Empty_Pump_Acceleration
		2800/25,	// DM_Empty_Pump_Speed
		120,		// DM_Attempt_Time
		10,			// DM_Initial_Time
		2,			// DM_VwiStable_Time
		15,			// EmptyDrain_Time
		0			// NA - empty
};

// Default structure for process modifiers
static SF_SENSEDDRAIN_PROCESS_DATA_TYPE SensedDrain_ProcessParam_Default = {
		UNUSED_FEATURE,			// WM_Speed_Modifier
		UNUSED_FEATURE,			// WM_Acceleration_Modifier
		UNUSED_FEATURE,			// DM_Default_Speed_Modifier
		UNUSED_FEATURE,			// DM_Default_Acceleration_Modifier
		FALSE,					// No_Fault
		0						// NA - empty
};

// Pointer to the "variable" data in the setting file for Partial Drain machine configuration
static SF_PARTIALDRAIN_MACHINECONFIG_TYPE* SD_PartialDrain_MachineConfig = (SF_PARTIALDRAIN_MACHINECONFIG_TYPE*)NULL;

// Default Partial Drain data for machine configuration
static SF_PARTIALDRAIN_MACHINECONFIG_TYPE SD_PartialDrain_MachineDefault = {
		PARTIALDRAIN_DEFAULT_LITERSMULTIPLIER,
		PARTIALDRAIN_DEFAULT_STARTINGOFFSET,
		PARTIALDRAIN_DEFAULT_INITIALVALUEMULTIPLIER,
		PARTIALDRAIN_DEFAULT_INITIALVALUEOFFSET,
		PARTIALDRAIN_DEFAULT_INITIALVALUEACQTIME,
		PARTIALDRAIN_DEFAULT_INITIALVALUEAVGSTR,
		PARTIALDRAIN_DEFAULT_INITIALVALUECOMP,
		PARTIALDRAIN_DEFAULT_ATTEMPTTIMERSEC,
		PARTIALDRAIN_DEFAULT_VWISTABLETIMERSEC,
		PARTIALDRAIN_DEFAULT_DRAINPUMPSPEED,
		PARTIALDRAIN_DEFAULT_DRAINPUMPACCEL
};

//! Process Variables

// Model Process Result - BUSY or COMPLETE
static PROCESS_RESULT_TYPE Model_ProcessResult = PROCESS_COMPLETE;

//! Timer Variables

// Timer Handlers
static SYSTEMTIMER_NV_TIMER_TYPE SensedDrain_RetryPause_Timer_Handler;
static SYSTEMTIMER_NV_TIMER_TYPE SensedDrain_DM_Initial_Timer_Handler;
static SYSTEMTIMER_NV_TIMER_TYPE SensedDrain_DM_VwiStable_Timer_Handler;
static SYSTEMTIMER_NV_TIMER_TYPE SensedDrain_WM_NoChange_Timer_Handler;
static SYSTEMTIMER_NV_TIMER_TYPE SensedDrain_DM_Attempt_Timer_Handler;
static SYSTEMTIMER_NV_TIMER_TYPE SensedDrain_WM_Attempt_Timer_Handler;
static SYSTEMTIMER_NV_TIMER_TYPE SensedDrain_EmptyDrain_Timer_Handler;
static SYSTEMTIMER_NV_TIMER_TYPE SensedDrain_InitialDrainValue_Timer_Handler;

// Timer Values - For platform logging only
static uint32 SensedDrain_RetryPause_Timer_Value;
static uint32 SensedDrain_DM_Initial_Timer_Value;
static uint32 SensedDrain_DM_VwiStable_Timer_Value;
static uint32 SensedDrain_WM_NoChange_Timer_Value;
static uint32 SensedDrain_DM_Attempt_Timer_Value;
static uint32 SensedDrain_WM_Attempt_Timer_Value;
static uint32 SensedDrain_EmptyDrain_Timer_Value;
static uint32 SensedDrain_InitialDrainValue_Timer_Value;

// ! Motor Variables
static uint16 SensedDrain_WashPumpAcceleration;
static sint16 SensedDrain_WashPumpSpeed;
static uint16 SensedDrain_DrainPumpAcceleration;
static sint16 SensedDrain_DrainPumpSpeed;

#if (API221_CLIENT_FEATURE == ENABLED)

static void API221ApplReceivedStatus (API221_CLIENT_MOTOR_INDEX_TYPE motor, uint32 status);
static void API221HandleMotorEvent (API221_EVENT_TYPE event);

static BOOL_TYPE WM_ReachedTargetSpeed = FALSE;
static BOOL_TYPE DM_ReachedTargetSpeed = FALSE;
static BOOL_TYPE WM_IsStopped = FALSE;
static BOOL_TYPE DM_IsStopped = FALSE;

#endif

//! Initial Drain Value - Filter & Average Variables
#pragma location = "_appliance_segment"
static uint8 SensedDrain_InitialDrainValue;
static uint32 SensedDrain_VwiHFAvg_Accumulator;
static uint32 SensedDrain_VwiHFAvg_Samples;
static uint32 SensedDrain_VwiHFAverage;

PACKED typedef struct
{
	BOOL_TYPE First_Value_Discarded;
	IIR_FILTER_MEASURE_TYPE Initial_Drain_Filter;
} SENSEDDRAIN_FILTER_TYPE;

#pragma location = "_appliance_segment"
SENSEDDRAIN_FILTER_TYPE SensedDrain_FilterData;

//-------------------------------------- PRIVATE (Function Prototypes) -------------------------------------

static void ReadParameters(PROCESS_CONTROL_TYPE* control);
static void WashCommand_Decrement(ON_OFF_TYPE command, sint16 speed, uint16 acceleration);
static void DrainCommand_Decrement(ON_OFF_TYPE command, sint16 speed, uint16 acceleration);
static void CalculateVwiHFAverage(void);


//-------------------------------------- PUBLIC (Get/Set Functions) ----------------------------------------

// General Set functions

void SensedDrain__SetSensedDrainStatus(PASS_FAIL_TYPE senseddrain_status)
{
	DishAuxVar__SetSensedDrainStatus(senseddrain_status);
}


void SensedDrain__SetModelData(SENSEDDRAIN_MODELDATA_TYPE modeldata)
{

	SensedDrain_ModelData = modeldata;
}


void SensedDrain__SetFault(FAULT_ID_TYPE fault_type)
{
	if ((FAULT_ID_NOT_DRAINING == fault_type) && (TRUE == SensedDrain_Process_Parameters.No_Fault))
	{
		// Use a different fault code to not display fault to consumer
		fault_type = FAULT_ID_NOT_DRAINING_NO_CYCLE_END;
	}

	Fault__ConditionDetected(fault_type);
}

void SensedDrain__SetDrainConfirmed(BOOL_TYPE drain_confirmed)
{
	SensedDrain_DrainConfirmed = (uint8)drain_confirmed;
}


void SensedDrain__SetWaterInformation(uint8 water_info)
{
	// This information is set only in the 2VSM logic, only in this logic we are certain that
	// there is no residual water in the sump.

	Variable__SetValue(VARIABLE_OPERATION_SET, VARIABLE_LITERS_IN_TUB, 0, 0);
	Variable__SetValue(VARIABLE_OPERATION_SET, VARIABLE_WATER_PRESENT, 0, 0);
}


void SensedDrain__ClearFault(FAULT_ID_TYPE fault_type)
{
	if ((FAULT_ID_NOT_DRAINING == fault_type) && (TRUE == SensedDrain_Process_Parameters.No_Fault))
	{
		// Use a different fault code to not display fault to consumer
		fault_type = FAULT_ID_NOT_DRAINING_NO_CYCLE_END;
	}

	Fault__ConditionRemoved(fault_type);
}


// General Get functions

SENSEDDRAIN_MODELDATA_TYPE SensedDrain__GetModelData(void)
{
	return SensedDrain_ModelData;
}


BOOL_TYPE SensedDrain__GetDrainConfirmed(void)
{
	return (BOOL_TYPE)SensedDrain_DrainConfirmed;
}


uint8 SensedDrain__GetInitialDrainValue(void)
{
	return SensedDrain_InitialDrainValue;
}

// Timer Set Functions

void SensedDrain__SetTimerAsReset(SENSEDDRAIN_TIMERS_TYPE timer_type)
{
	switch (timer_type)
	{
		case RETRYPAUSE_TIMER:
			SystemTimers__NVResetTimer(&SensedDrain_RetryPause_Timer_Handler);
			break;
		case DM_INITIAL_TIMER:
			SystemTimers__NVResetTimer(&SensedDrain_DM_Initial_Timer_Handler);
			break;
		case DM_VWISTABLE_TIMER:
			SystemTimers__NVResetTimer(&SensedDrain_DM_VwiStable_Timer_Handler);
			break;
		case WM_NOCHANGE_TIMER:
			SystemTimers__NVResetTimer(&SensedDrain_WM_NoChange_Timer_Handler);
			break;
		case DM_ATTEMPT_TIMER:
			SystemTimers__NVResetTimer(&SensedDrain_DM_Attempt_Timer_Handler);
			break;
		case WM_ATTEMPT_TIMER:
			SystemTimers__NVResetTimer(&SensedDrain_WM_Attempt_Timer_Handler);
			break;
		case EMPTYDRAIN_TIMER:
			SystemTimers__NVResetTimer(&SensedDrain_EmptyDrain_Timer_Handler);
			break;
		default:
			break;
	}
}

void SensedDrain__SetTimerAsStart(SENSEDDRAIN_TIMERS_TYPE timer_type)
{
	uint32 time_ms;

	switch (timer_type)
	{
		case RETRYPAUSE_TIMER:
			time_ms = Sf_SensedDrain_Config->Retry_Pause_Time * 1000;
			SystemTimers__NVSetTimerMS(&SensedDrain_RetryPause_Timer_Handler, time_ms, SYSTEMTIMERS_TYPE_DOWN_COUNTER);
			break;
		case DM_INITIAL_TIMER:
			time_ms = Sf_SensedDrain_Config->DM_Initial_Time * 1000;
			SystemTimers__NVSetTimerMS(&SensedDrain_DM_Initial_Timer_Handler, time_ms, SYSTEMTIMERS_TYPE_DOWN_COUNTER);
			break;
		case DM_VWISTABLE_TIMER:
			time_ms = Sf_SensedDrain_Config->DM_VwiStable_Time * 1000;
			SystemTimers__NVSetTimerMS(&SensedDrain_DM_VwiStable_Timer_Handler, time_ms, SYSTEMTIMERS_TYPE_DOWN_COUNTER);
			break;
		case WM_NOCHANGE_TIMER:
			time_ms = Sf_SensedDrain_Config->WM_NoChange_Time * 1000;
			SystemTimers__NVSetTimerMS(&SensedDrain_WM_NoChange_Timer_Handler, time_ms, SYSTEMTIMERS_TYPE_DOWN_COUNTER);
			break;
		case DM_ATTEMPT_TIMER:
			time_ms = Sf_SensedDrain_Config->DM_Attempt_Time * 1000;
			SystemTimers__NVSetTimerMS(&SensedDrain_DM_Attempt_Timer_Handler, time_ms, SYSTEMTIMERS_TYPE_DOWN_COUNTER);
			break;
		case WM_ATTEMPT_TIMER:
			time_ms = Sf_SensedDrain_Config->WM_Attempt_Time * 1000;
			SystemTimers__NVSetTimerMS(&SensedDrain_WM_Attempt_Timer_Handler, time_ms, SYSTEMTIMERS_TYPE_DOWN_COUNTER);
			break;
		case EMPTYDRAIN_TIMER:
			time_ms = Sf_SensedDrain_Config->EmptyDrain_Time * 1000;
			SystemTimers__NVSetTimerMS(&SensedDrain_EmptyDrain_Timer_Handler, time_ms, SYSTEMTIMERS_TYPE_DOWN_COUNTER);
			break;
		default:
			break;
	}
}

// Motor Set Functions

void SensedDrain__SetWashMotorState(ON_OFF_TYPE motor_state)
{
	HBL_PILOT_ENUM_TYPE pump_pilot;
	pump_pilot = Hbl__GetPilotIDByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_WASH_PUMP));

	if (pump_pilot != HBL_PILOT_INVALID)
	{
		if (pump_pilot == HBL_PILOT_EXTERNAL_MOTOR) // VSM Motor
		{
			// Get speed from SF
			SensedDrain_WashPumpSpeed = Sf_SensedDrain_Config->WM_Pump_Speed * 25;
			SensedDrain_WashPumpAcceleration = Sf_SensedDrain_Config->WM_Pump_Acceleration;
		    sint32 modified_value;

			// Apply modifiers when applicable
			if (UNUSED_FEATURE != SensedDrain_Process_Parameters.WM_Speed_Modifier)
			{
				modified_value = Modifier__GetValue((sint32)SensedDrain_WashPumpSpeed, SensedDrain_Process_Parameters.WM_Speed_Modifier);
				if (modified_value < 0)
				{
					modified_value = 0;
				}
				SensedDrain_WashPumpSpeed = (sint16)modified_value;
			}

			if (UNUSED_FEATURE != SensedDrain_Process_Parameters.WM_Acceleration_Modifier)
			{
				modified_value = Modifier__GetValue((sint32)SensedDrain_WashPumpAcceleration, SensedDrain_Process_Parameters.WM_Acceleration_Modifier);
				if (modified_value < 0)
				{
					modified_value = 0;
				}
				SensedDrain_WashPumpAcceleration = (uint16)modified_value;
			}

			// Send Motor Command
			WashCommand_Decrement(motor_state, SensedDrain_WashPumpSpeed, SensedDrain_WashPumpAcceleration);
		}
		else // Single speed motor
		{
			Hbl__SetLoadRequestByLoadID(HBL_LOAD_WASH_PUMP, HBL_PRIORITY_APPLICATION_HI, (uint8 *) &motor_state); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
		}
	}
}

void SensedDrain__SetDrainMotorDefaultSpeedState(ON_OFF_TYPE motor_state)
{
	HBL_PILOT_ENUM_TYPE pump_pilot;
	pump_pilot = Hbl__GetPilotIDByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DRAIN_PUMP));

	if (pump_pilot != HBL_PILOT_INVALID)
	{
		if (pump_pilot == HBL_PILOT_EXTERNAL_MOTOR) // VSM Motor
		{
			// Get speed from SF
			SensedDrain_DrainPumpSpeed = Sf_SensedDrain_Config->DM_Default_Pump_Speed * 25;
			SensedDrain_DrainPumpAcceleration = Sf_SensedDrain_Config->DM_Default_Pump_Acceleration;
		    sint32 modified_value;

			// Apply modifiers when applicable
			if (UNUSED_FEATURE != SensedDrain_Process_Parameters.DM_Default_Speed_Modifier)
			{
				modified_value = Modifier__GetValue((sint32)SensedDrain_DrainPumpSpeed, SensedDrain_Process_Parameters.DM_Default_Speed_Modifier);
				if (modified_value < 0)
				{
					modified_value = 0;
				}
				SensedDrain_DrainPumpSpeed = (sint16)modified_value;
			}

			if (UNUSED_FEATURE != SensedDrain_Process_Parameters.DM_Default_Acceleration_Modifier)
			{
				modified_value = Modifier__GetValue((sint32)SensedDrain_DrainPumpAcceleration, SensedDrain_Process_Parameters.DM_Default_Acceleration_Modifier);
				if (modified_value < 0)
				{
					modified_value = 0;
				}
				SensedDrain_DrainPumpAcceleration = (uint16)modified_value;
			}

			// Send Motor Command
			DrainCommand_Decrement(motor_state, SensedDrain_DrainPumpSpeed, SensedDrain_DrainPumpAcceleration);
		}
		else // Single speed motor
		{
			Hbl__SetLoadRequestByLoadID(HBL_LOAD_DRAIN_PUMP, HBL_PRIORITY_APPLICATION_HI, (uint8 *) &motor_state); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
		}
	}
}


void SensedDrain__SetDrainMotorEmptySpeedState(ON_OFF_TYPE motor_state)
{
	HBL_PILOT_ENUM_TYPE pump_pilot;
	pump_pilot = Hbl__GetPilotIDByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DRAIN_PUMP));

	if (pump_pilot != HBL_PILOT_INVALID)
	{
		if (pump_pilot == HBL_PILOT_EXTERNAL_MOTOR) // VSM Motor
		{
			// Get speed from SF
			SensedDrain_DrainPumpSpeed = Sf_SensedDrain_Config->DM_Empty_Pump_Speed * 25;
			SensedDrain_DrainPumpAcceleration = Sf_SensedDrain_Config->DM_Empty_Pump_Acceleration;

			// Send Motor Command
			DrainCommand_Decrement(motor_state, SensedDrain_DrainPumpSpeed, SensedDrain_DrainPumpAcceleration);
		}
		else // Single speed motor
		{
			Hbl__SetLoadRequestByLoadID(HBL_LOAD_DRAIN_PUMP, HBL_PRIORITY_APPLICATION_HI, (uint8 *) &motor_state); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
		}
	}
}


void SensedDrain__RequestMotorStatus(uint8 motor_index)
{
	#if (API221_CLIENT_FEATURE == ENABLED)
	if (motor_index == 0) // Wash Motor
	{
		(void)API221MotionCtrl__RequestStatus(MOTOR_0, API221ApplReceivedStatus);
	}
	else if(motor_index == 1) // Drain Motor
	{
		(void)API221MotionCtrl__RequestStatus(MOTOR_1, API221ApplReceivedStatus);
	}
	else if(motor_index == 2) // Both Motors
	{
		(void)API221MotionCtrl__RequestStatus(MOTOR_0, API221ApplReceivedStatus);
		(void)API221MotionCtrl__RequestStatus(MOTOR_1, API221ApplReceivedStatus);
	}
	#endif
}


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
/**
 * The Sensed Drain process.
 *
 * @brief
 *
 * @param control = The control parameters provided by the Interpreter.
 * @retval PROCESS_COMPLETE = The process is ready for the Interpreter to proceed to the next step.
 * @retval PROCESS_BUSY = The process is not ready for the Interpreter to proceed to the next step.
 */


PROCESS_RESULT_TYPE SensedDrain__Process(PROCESS_CONTROL_TYPE* 	control)
{
	PROCESS_RESULT_TYPE result = PROCESS_COMPLETE;
	BOOL_TYPE senseddrain_enabled = TRUE;

	if ((control == NULL) ||
	    (control->Compartment >= APPLIANCE_MAX_COMPARTMENTS))
	{
	   	LOG_ADD_EXCEPTION(SENSEDDRAIN_ERROR_PROCESS_CONTROL, (control == NULL));
	}
	else
	{
		if (control->F1_F0_Flags == PROCESS_SENSEDDRAIN)
		{
			// Initializing
			if ((control->Step_Initializing == TRUE) ||
				(control->Resume_From_Reset == TRUE) ||
				(control->Resume_From_Pause == TRUE))
			{
				// Load Data from Settings File
				ReadParameters(control);

				// Initialize PumpCommand_Previous variables
				DrainCommand_Previous = OFF;
				WashCommand_Previous = OFF;

				#if (API221_CLIENT_FEATURE == ENABLED)
				// Register for API221 Events
				API221MotionCtrl__RegisterForEvents(API221HandleMotorEvent);

				// Clear motor flags
				WM_ReachedTargetSpeed = FALSE;
				DM_ReachedTargetSpeed = FALSE;
				WM_IsStopped = FALSE;
				DM_IsStopped = FALSE;
				#endif
			}

			if (control->Step_Initializing == TRUE)
			{
				// Initialize model
				SensedDrainModel__Initialize(&SensedDrain_RTModel);

				// Reset all state machine states
				memset(&SensedDrain_RTModel.self_sf_SensedDrain__Logic.dwork, 0, sizeof(SensedDrain_RTModel.self_sf_SensedDrain__Logic.dwork));

				// Create all time handlers
				SystemTimers__NVResetTimer(&SensedDrain_RetryPause_Timer_Handler);
				SystemTimers__NVResetTimer(&SensedDrain_DM_Initial_Timer_Handler);
				SystemTimers__NVResetTimer(&SensedDrain_DM_VwiStable_Timer_Handler);
				SystemTimers__NVResetTimer(&SensedDrain_WM_NoChange_Timer_Handler);
				SystemTimers__NVResetTimer(&SensedDrain_DM_Attempt_Timer_Handler);
				SystemTimers__NVResetTimer(&SensedDrain_WM_Attempt_Timer_Handler);
				SystemTimers__NVResetTimer(&SensedDrain_EmptyDrain_Timer_Handler);
				SystemTimers__NVResetTimer(&SensedDrain_InitialDrainValue_Timer_Handler);

				// Clear HF averaging values
				SensedDrain_VwiHFAvg_Accumulator = 0;
				SensedDrain_VwiHFAvg_Samples = 0;
			}

			// Executing
			if (Sf_SensedDrain_Config != NULL)
			{
				//! Update Timer Values for logging purposes
				SensedDrain_RetryPause_Timer_Value = SystemTimers__NVGetTimerMs(&SensedDrain_RetryPause_Timer_Handler);
				SensedDrain_DM_Initial_Timer_Value = SystemTimers__NVGetTimerMs(&SensedDrain_DM_Initial_Timer_Handler);
				SensedDrain_DM_VwiStable_Timer_Value = SystemTimers__NVGetTimerMs(&SensedDrain_DM_VwiStable_Timer_Handler);
				SensedDrain_WM_NoChange_Timer_Value = SystemTimers__NVGetTimerMs(&SensedDrain_WM_NoChange_Timer_Handler);
				SensedDrain_DM_Attempt_Timer_Value = SystemTimers__NVGetTimerMs(&SensedDrain_DM_Attempt_Timer_Handler);
				SensedDrain_WM_Attempt_Timer_Value = SystemTimers__NVGetTimerMs(&SensedDrain_WM_Attempt_Timer_Handler);
				SensedDrain_EmptyDrain_Timer_Value = SystemTimers__NVGetTimerMs(&SensedDrain_EmptyDrain_Timer_Handler);

				//! Update External Inputs

				// Timers State
				SensedDrain_TimerState.SensedDrain_RetryPause_Timer_State = SystemTimers__NVCheckTimer(&SensedDrain_RetryPause_Timer_Handler);
				SensedDrain_TimerState.SensedDrain_DM_Initial_Timer_State = SystemTimers__NVCheckTimer(&SensedDrain_DM_Initial_Timer_Handler);
				SensedDrain_TimerState.SensedDrain_DM_VwiStable_Timer_State = SystemTimers__NVCheckTimer(&SensedDrain_DM_VwiStable_Timer_Handler);
				SensedDrain_TimerState.SensedDrain_WM_NoChange_Timer_State = SystemTimers__NVCheckTimer(&SensedDrain_WM_NoChange_Timer_Handler);
				SensedDrain_TimerState.SensedDrain_DM_Attempt_Timer_State = SystemTimers__NVCheckTimer(&SensedDrain_DM_Attempt_Timer_Handler);
				SensedDrain_TimerState.SensedDrain_WM_Attempt_Timer_State = SystemTimers__NVCheckTimer(&SensedDrain_WM_Attempt_Timer_Handler);
				SensedDrain_TimerState.SensedDrain_EmptyDrain_Timer_State = SystemTimers__NVCheckTimer(&SensedDrain_EmptyDrain_Timer_Handler);

				// Vwi Information
				SensedDrain_VwiInformation.Vwi_Status = Vwi__GetWaterStatus();
				SensedDrain_VwiInformation.Vwi_Heavy_Filt_Signal = Vwi__GetHeavyFiltSignal();

				// Motor Data
				SensedDrain_MotorData.Wash_Pump_Type = Hbl__GetPilotIDByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_WASH_PUMP));
				SensedDrain_MotorData.Drain_Pump_Type = Hbl__GetPilotIDByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DRAIN_PUMP));

				if (SensedDrain_MotorData.Wash_Pump_Type != HBL_PILOT_INVALID && SensedDrain_MotorData.Drain_Pump_Type != HBL_PILOT_INVALID)
				{
					if (SensedDrain_MotorData.Wash_Pump_Type == HBL_PILOT_EXTERNAL_MOTOR) // VSM Motor
					{
						#if (API221_CLIENT_FEATURE == ENABLED)
						SensedDrain_MotorData.WashPump_Is_Stopped = WM_IsStopped;
						SensedDrain_MotorData.WashPump_Reached_Target_Speed = WM_ReachedTargetSpeed;
						#endif
					}
					else // RIF or GWS Motor
					{
						SensedDrain_MotorData.WashPump_Is_Stopped = (((uint8*)(Hbl__GetLoadStatusByLoadID(HBL_LOAD_WASH_PUMP)))[0] == OFF) ? TRUE:FALSE;
						SensedDrain_MotorData.WashPump_Reached_Target_Speed = TRUE; // Single speeds ramp up fast, so no need to wait for reached target speed
					}

					if (SensedDrain_MotorData.Drain_Pump_Type == HBL_PILOT_EXTERNAL_MOTOR) // VSM Motor
					{
						#if (API221_CLIENT_FEATURE == ENABLED)
						SensedDrain_MotorData.DrainPump_Is_Stopped = DM_IsStopped;
						SensedDrain_MotorData.DrainPump_Reached_Target_Speed = DM_ReachedTargetSpeed;
						#endif
					}
					else // RIF or GWS Motor
					{
						SensedDrain_MotorData.DrainPump_Is_Stopped = (((uint8*)(Hbl__GetLoadStatusByLoadID(HBL_LOAD_DRAIN_PUMP)))[0] == OFF) ? TRUE:FALSE;
						SensedDrain_MotorData.DrainPump_Reached_Target_Speed = TRUE; // Single speeds ramp up fast, so no need to wait for reached target speed
					}

					//! Run Model
					SensedDrainModel__Execute(&SensedDrain_RTModel, control, &SensedDrain_MotorData, &SensedDrain_TimerState, &SensedDrain_VwiInformation,
							Sf_SensedDrain_Config, senseddrain_enabled, &Model_ProcessResult);
					result = Model_ProcessResult; // Get result from model execution

					//! Calculate Initial Drain Value for Partial Drain
					if (SD_PartialDrain_MachineConfig != NULL)
					{
						CalculateVwiHFAverage();

						// When process is finished, filter Averaged HF signal for Initial Drain value calculation that is used in Partial Drain
						if (result == PROCESS_COMPLETE && DishAuxVar__GetSensedDrainStatus() == PASS)
						{
							if (SensedDrain_FilterData.First_Value_Discarded == FALSE)
							{
								SensedDrain_FilterData.First_Value_Discarded = TRUE;
							}
							else if (Supervisor__IsTestCycleActive() == TRUE)
							{
								SensedDrain_InitialDrainValue = 0;
								SensedDrain_FilterData.First_Value_Discarded = FALSE;
							}
							else
							{
								SensedDrain_FilterData.Initial_Drain_Filter.Inverted_Alpha = 1 << (SD_PartialDrain_MachineConfig->Initial_Value_Average_Strenght); // 2^N

								if (SensedDrain_InitialDrainValue == 0)
								{
									SensedDrain_VwiHFAverage = (SensedDrain_VwiHFAverage >= 255) ? (255) : (SensedDrain_VwiHFAverage);
									Filter__Initialize((void *)(&(SensedDrain_FilterData.Initial_Drain_Filter)),
											(FILTER_DATA_TYPE)SensedDrain_VwiHFAverage, FILTER_IIR);
								}
								else
								{
									// Calculate Initial Drain Value from Filtered Averaging
									Filter__Update((void *)(&(SensedDrain_FilterData.Initial_Drain_Filter)),
											(FILTER_DATA_TYPE)SensedDrain_VwiHFAverage, FILTER_IIR);
								}

								SensedDrain_InitialDrainValue = (uint8)SensedDrain_FilterData.Initial_Drain_Filter.Filtered_Value;
							}
						}
					}
				}
				else
				{
					if (SensedDrain_MotorData.Wash_Pump_Type == HBL_PILOT_INVALID)
					{
						LOG_ADD_EXCEPTION(SENSEDDRAIN_ERROR_MOTOR_INVALID, HBL_LOAD_WASH_PUMP);
					}
					else
					{
						LOG_ADD_EXCEPTION(SENSEDDRAIN_ERROR_MOTOR_INVALID, HBL_LOAD_DRAIN_PUMP);
					}
				}

			}
			else
			{
				LOG_ADD_EXCEPTION(SENSEDDRAIN_ERROR_PROCESS_PARAMETERS, (Sf_SensedDrain_Config == NULL));
			}

			#if (API221_CLIENT_FEATURE == ENABLED)
			// Advise Pump Flush Request Monitor
			if (SensedDrain_MotorData.Wash_Pump_Type == HBL_PILOT_EXTERNAL_MOTOR)
			{
				PumpFlushRequestMonitor__SetWashPumpCtrlExecutedFlag();
			}
			// Advise Pump Flush Request Monitor
			if (SensedDrain_MotorData.Drain_Pump_Type == HBL_PILOT_EXTERNAL_MOTOR)
			{
				PumpFlushRequestMonitor__SetDrainPumpCtrlExecutedFlag();
			}

			if (result == PROCESS_COMPLETE)
			{
				API221MotionCtrl__UnregisterForEvents(API221HandleMotorEvent);
			}
			#endif
		}
		else if (control->F1_F0_Flags == PROCESS_PARTIALDRAIN)
		{
			PartialDrain__Process(control);
			result = PartialDrainProcess__GetProcessResult();
		}
		else
		{
			LOG_ADD_EXCEPTION(SENSEDDRAIN_ERROR_PROCESS_CONTROL, (control == NULL));
		}
	}
	
	return result;
}
	

void SensedDrain__Initialize(void)
{
    if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
    {
		if (NVData__GetLastRestoreStatus()->Bit.Appliance_Data_Recovered != NVDATA_RESTORE_OKAY)
    	{
			SensedDrain_DrainConfirmed = FALSE;
			SensedDrain_InitialDrainValue = 0;
			SensedDrain_FilterData.First_Value_Discarded = FALSE;
    	}
    }

    // The following code suppress "not used variable" warning from this file, but has no functionality purpose.
    (void)SensedDrain_RetryPause_Timer_Value;
    (void)SensedDrain_DM_Initial_Timer_Value;
    (void)SensedDrain_DM_VwiStable_Timer_Value;
    (void)SensedDrain_WM_NoChange_Timer_Value;
    (void)SensedDrain_DM_Attempt_Timer_Value;
    (void)SensedDrain_WM_Attempt_Timer_Value;
    (void)SensedDrain_EmptyDrain_Timer_Value;
    (void)SensedDrain_InitialDrainValue_Timer_Value;

    // Initialize alternative drain - partial drain
    PartialDrain__Initialize();
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


static void ReadParameters(PROCESS_CONTROL_TYPE* control)
{
    SETTINGFILE_LOADER_TYPE loader;

    // Start with default process parameters values
    SensedDrain_Process_Parameters = SensedDrain_ProcessParam_Default;

	// Get the Sensed Drain parameters from the setting file machine structure.
    if((SettingFile__MSettingBasicLoader(control->Setting_File_Index,
                                         SF_PTR_ACU_ALGORITHM_PARAMETERS,
                                         DISPL_SENSED_DRAIN,
                                         &loader)== FAIL))
    {
        LOG_ADD_EXCEPTION(SENSEDDRAIN_ERROR_PROCESS_PARAMETERS, control->Data_Field_Index);
    }
    else
    {
    	// Read the setting file data into local variables.
    	Sf_SensedDrain_Default.Max_Drain_Attempts = ((SF_SENSEDDRAIN_DATA_TYPE*)(loader.Data))->Max_Drain_Attempts; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
    	Sf_SensedDrain_Default.WM_Attempt_Time = ((SF_SENSEDDRAIN_DATA_TYPE*)(loader.Data))->WM_Attempt_Time; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
    	Sf_SensedDrain_Default.WM_NoChange_Time = ((SF_SENSEDDRAIN_DATA_TYPE*)(loader.Data))->WM_NoChange_Time; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
    	Sf_SensedDrain_Default.Vwi_Drained_Threshold = ((SF_SENSEDDRAIN_DATA_TYPE*)(loader.Data))->Vwi_Drained_Threshold; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
    	Sf_SensedDrain_Default.WM_Pump_Acceleration = ENDIANU16_STON(((SF_SENSEDDRAIN_DATA_TYPE*)(loader.Data))->WM_Pump_Acceleration); //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
    	Sf_SensedDrain_Default.WM_Pump_Speed = ((SF_SENSEDDRAIN_DATA_TYPE*)(loader.Data))->WM_Pump_Speed; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
    	Sf_SensedDrain_Default.Retry_Pause_Time = ((SF_SENSEDDRAIN_DATA_TYPE*)(loader.Data))->Retry_Pause_Time; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
    	Sf_SensedDrain_Default.Vwi_Stable_Max_Difference = ((SF_SENSEDDRAIN_DATA_TYPE*)(loader.Data))->Vwi_Stable_Max_Difference; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
    	Sf_SensedDrain_Default.DM_Default_Pump_Speed = ((SF_SENSEDDRAIN_DATA_TYPE*)(loader.Data))->DM_Default_Pump_Speed; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
    	Sf_SensedDrain_Default.DM_Default_Pump_Acceleration = ENDIANU16_STON(((SF_SENSEDDRAIN_DATA_TYPE*)(loader.Data))->DM_Default_Pump_Acceleration); //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
    	Sf_SensedDrain_Default.DM_Empty_Pump_Acceleration = ENDIANU16_STON(((SF_SENSEDDRAIN_DATA_TYPE*)(loader.Data))->DM_Empty_Pump_Acceleration); //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
    	Sf_SensedDrain_Default.DM_Empty_Pump_Speed = ((SF_SENSEDDRAIN_DATA_TYPE*)(loader.Data))->DM_Empty_Pump_Speed; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
    	Sf_SensedDrain_Default.DM_Attempt_Time = ((SF_SENSEDDRAIN_DATA_TYPE*)(loader.Data))->DM_Attempt_Time; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
    	Sf_SensedDrain_Default.DM_Initial_Time = ((SF_SENSEDDRAIN_DATA_TYPE*)(loader.Data))->DM_Initial_Time; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
    	Sf_SensedDrain_Default.DM_VwiStable_Time = ((SF_SENSEDDRAIN_DATA_TYPE*)(loader.Data))->DM_VwiStable_Time; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
    	Sf_SensedDrain_Default.EmptyDrain_Time = ((SF_SENSEDDRAIN_DATA_TYPE*)(loader.Data))->EmptyDrain_Time; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]

    	if(loader.Length <= sizeof(SF_SENSEDDRAIN_NOVSM_TYPE))
    	{
    		if (loader.Length <= sizeof(SF_SENSEDDRAIN_OLDDATA_TYPE))
    		{
    			// Assign default values - Hardcoded variables before
        		Sf_SensedDrain_Default.Retry_Pause_Time = 5;
        		Sf_SensedDrain_Default.Vwi_Stable_Max_Difference = 1;
    		}

    		// Assign default values to Drain Motor for safety
    		Sf_SensedDrain_Default.DM_Default_Pump_Speed = 3500/25;
    		Sf_SensedDrain_Default.DM_Default_Pump_Acceleration = 1500;
    		Sf_SensedDrain_Default.DM_Empty_Pump_Acceleration = 1500;
    		Sf_SensedDrain_Default.DM_Empty_Pump_Speed = 2800/25;
    		Sf_SensedDrain_Default.DM_Attempt_Time = 120;
    		Sf_SensedDrain_Default.DM_Initial_Time = 10;
    		Sf_SensedDrain_Default.DM_VwiStable_Time = 2;
    		Sf_SensedDrain_Default.EmptyDrain_Time = 15;
    	}

        // Get the process parameters from the setting file.
        if (SettingFile__MSettingBasicLoader(control->Setting_File_Index,
        									 SF_PTR_ACU_PROCESS_SENSED_DRAIN,
											 control->Data_Field_Index,
											 &loader) == FAIL)
        {
        	LOG_ADD_EXCEPTION(SENSEDDRAIN_ERROR_PROCESS_PARAMETERS, control->Data_Field_Index);
        }
        else
        {
			if ((Hbl__GetPilotIDByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_WASH_PUMP)) == HBL_PILOT_EXTERNAL_MOTOR) &&
					(Hbl__GetPilotIDByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DRAIN_PUMP)) == HBL_PILOT_EXTERNAL_MOTOR))
			{
				SensedDrain_Process_Parameters.WM_Speed_Modifier = ((SF_SENSEDDRAIN_PROCESS_POS_TYPE*)(loader.Data))->Pos1; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
				SensedDrain_Process_Parameters.WM_Acceleration_Modifier = ((SF_SENSEDDRAIN_PROCESS_POS_TYPE*)(loader.Data))->Pos2; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
				SensedDrain_Process_Parameters.DM_Default_Speed_Modifier = ((SF_SENSEDDRAIN_PROCESS_POS_TYPE*)(loader.Data))->Pos3; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
				SensedDrain_Process_Parameters.DM_Default_Acceleration_Modifier = ((SF_SENSEDDRAIN_PROCESS_POS_TYPE*)(loader.Data))->Pos4; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
				if (loader.Length > 4)
				{
					// 2 VSM + Flag
					SensedDrain_Process_Parameters.No_Fault = ((((SF_SENSEDDRAIN_PROCESS_POS_TYPE*)(loader.Data))->Pos5 & 0x80) ? (TRUE) : (FALSE)); //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
				}

			}
			else if (Hbl__GetPilotIDByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_WASH_PUMP)) == HBL_PILOT_EXTERNAL_MOTOR)
			{
				SensedDrain_Process_Parameters.WM_Speed_Modifier = ((SF_SENSEDDRAIN_PROCESS_POS_TYPE*)(loader.Data))->Pos1; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
				SensedDrain_Process_Parameters.WM_Acceleration_Modifier = ((SF_SENSEDDRAIN_PROCESS_POS_TYPE*)(loader.Data))->Pos2; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
				if (loader.Length > 2)
				{
					// Wash VSM + Flag
					SensedDrain_Process_Parameters.No_Fault = ((((SF_SENSEDDRAIN_PROCESS_POS_TYPE*)(loader.Data))->Pos3 & 0x80) ? (TRUE) : (FALSE)); //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
				}
			}
			else
			{
				if (loader.Length > 0)
				{
					// No VSM + Flag
					SensedDrain_Process_Parameters.No_Fault = ((((SF_SENSEDDRAIN_PROCESS_POS_TYPE*)(loader.Data))->Pos1 & 0x80) ? (TRUE) : (FALSE)); //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
				}
			}
        }
    }
	
	// Get the parameters from the setting file machine structure.
    if (SettingFile__MSettingBasicLoader(control->Setting_File_Index,
 									 SF_PTR_ACU_ALGORITHM_PARAMETERS,
									 DISPL_PARTIAL_DRAIN,
 									 &loader) == FAIL)
    {
    	LOG_ADD_EXCEPTION(SENSEDDRAIN_ERROR_PARTIALDRAIN_PARAMETERS, control->Data_Field_Index);
    }
			else
    {
		SD_PartialDrain_MachineDefault.Liters_Multiplier = ((SF_PARTIALDRAIN_MACHINECONFIG_TYPE*)(loader.Data))->Liters_Multiplier; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
		SD_PartialDrain_MachineDefault.Initial_Value_Multiplier = ((SF_PARTIALDRAIN_MACHINECONFIG_TYPE*)(loader.Data))->Initial_Value_Multiplier; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
		SD_PartialDrain_MachineDefault.Initial_Value_Offset = ((SF_PARTIALDRAIN_MACHINECONFIG_TYPE*)(loader.Data))->Initial_Value_Offset; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
		SD_PartialDrain_MachineDefault.Initial_Value_Acquisition_Time = ((SF_PARTIALDRAIN_MACHINECONFIG_TYPE*)(loader.Data))->Initial_Value_Acquisition_Time; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
		SD_PartialDrain_MachineDefault.Initial_Value_Average_Strenght = ((SF_PARTIALDRAIN_MACHINECONFIG_TYPE*)(loader.Data))->Initial_Value_Average_Strenght; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
		SD_PartialDrain_MachineDefault.Attempt_Time_Sec  = ((SF_PARTIALDRAIN_MACHINECONFIG_TYPE*)(loader.Data))->Attempt_Time_Sec; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
		SD_PartialDrain_MachineDefault.VwiStable_Time_Sec  = ((SF_PARTIALDRAIN_MACHINECONFIG_TYPE*)(loader.Data))->VwiStable_Time_Sec; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
		SD_PartialDrain_MachineDefault.Drain_Pump_Speed  = ((SF_PARTIALDRAIN_MACHINECONFIG_TYPE*)(loader.Data))->Drain_Pump_Speed; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
		SD_PartialDrain_MachineDefault.Drain_Pump_Acceleration  = ((SF_PARTIALDRAIN_MACHINECONFIG_TYPE*)(loader.Data))->Drain_Pump_Acceleration; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
    }

    // If loading fails, SF will be set as the default values
    Sf_SensedDrain_Config = &Sf_SensedDrain_Default;
    SD_PartialDrain_MachineConfig = &SD_PartialDrain_MachineDefault;
}

/**
 * Calculate Initial Drain Value
 * This value, calculated from an IIR filter in VwiSignalHF is used by Partial Drain
 */
void CalculateVwiHFAverage(void)
{
	SYSTEMTIMERS_STATE_TYPE empty_drain_timer_state = SystemTimers__NVCheckTimer(&SensedDrain_EmptyDrain_Timer_Handler);
	SYSTEMTIMERS_STATE_TYPE delay_timer_state = SystemTimers__NVCheckTimer(&SensedDrain_InitialDrainValue_Timer_Handler);
	uint32 wait_time_msec;

	// Execute only during empty timer
	if (empty_drain_timer_state == SYSTEMTIMERS_STATE_RUNNING)
	{
		// Calculate a delay timer to do "acquisition time" in the end of empty drain time
		if (Sf_SensedDrain_Config->EmptyDrain_Time >= SD_PartialDrain_MachineConfig->Initial_Value_Acquisition_Time)
		{
			wait_time_msec = (Sf_SensedDrain_Config->EmptyDrain_Time - SD_PartialDrain_MachineConfig->Initial_Value_Acquisition_Time)*1000;
		}
		else
		{
			wait_time_msec = 0;
		}

		// Start timer if stopped and wait time is valid
		if (wait_time_msec > 0 && delay_timer_state == SYSTEMTIMERS_STATE_STOPPED)
		{
			SystemTimers__NVSetTimerMS(&SensedDrain_InitialDrainValue_Timer_Handler, wait_time_msec, SYSTEMTIMERS_TYPE_DOWN_COUNTER);
		}
		else if (wait_time_msec == 0)
		{
			delay_timer_state = SYSTEMTIMERS_STATE_DONE;
		}

		// Debug information only
		SensedDrain_InitialDrainValue_Timer_Value = SystemTimers__NVGetTimerMs(&SensedDrain_InitialDrainValue_Timer_Handler);

		// Start averaging when delay is finished
		if (delay_timer_state == SYSTEMTIMERS_STATE_DONE)
		{
			SensedDrain_VwiHFAvg_Accumulator += Vwi__GetHeavyFiltSignal();
			SensedDrain_VwiHFAvg_Samples += 1;

			SensedDrain_VwiHFAverage = ((SensedDrain_VwiHFAvg_Accumulator/SensedDrain_VwiHFAvg_Samples) *
					SD_PartialDrain_MachineConfig->Initial_Value_Compensation)/100;
		}
	}
	else if (empty_drain_timer_state == SYSTEMTIMERS_STATE_DONE)
	{
		// Clear averaging values
		SensedDrain_VwiHFAvg_Accumulator = 0;
		SensedDrain_VwiHFAvg_Samples = 0;
	}
}


/**
 * Decrement Wash Motor Commands
 * Send motor commands every 2 seconds to avoid missing packages or filling the bus every 100ms.
 * @param command = ON or OFF.
 * @param speed = Motor Speed
 * @param acceleration = Motor Acceleration
 */
void WashCommand_Decrement(ON_OFF_TYPE command, sint16 speed, uint16 acceleration)
{
	// API 221 Command

	// Send a command at first if its different then the previous
	if (command != WashCommand_Previous)
	{
		WashCommand_Counter = 20;
	}

	// Increase counter in every run
	WashCommand_Counter++;

	if (WashCommand_Counter >= 20)
	{
		WashCommand_Counter = 0;

		#if (API221_CLIENT_FEATURE == ENABLED)
		if (command == ON && !WM_ReachedTargetSpeed)
		{
			API221MotionCtrl__DeviceRun(MOTOR_0, speed, acceleration);
		}
		else if (command == OFF && !WM_IsStopped)
		{
			API221MotionCtrl__DeviceStop(MOTOR_0, acceleration);
		}
		#endif
	}

	WashCommand_Previous = command;

	// PumpCommand - External Motor Pilot needs to know that the motor is running
	if (command == ON)
	{
		PumpCommand__Run(speed, acceleration, HBL_PRIORITY_APPLICATION_HI, HBL_LOAD_WASH_PUMP);
	}
	else
	{
		PumpCommand__Stop(acceleration, HBL_PRIORITY_APPLICATION_HI, HBL_LOAD_WASH_PUMP);
	}
}


/**
 * Decrement Drain Motor Commands
 * Send motor commands every 2 seconds to avoid missing packages or filling the bus every 100ms.
 * @param command = ON or OFF.
 * @param speed = Motor Speed
 * @param accelertion = Motor Acceleration
 */
void DrainCommand_Decrement(ON_OFF_TYPE command, sint16 speed, uint16 acceleration)
{
	// API 221 Command

	// Send a command at first if its different then the previous
	if (command != DrainCommand_Previous)
	{
		DrainCommand_Counter = 20;
	}

	// Clean ReachedTargetSpeed flag if speed is different
	if (speed != DrainSpeed_Previous)
	{
		DrainCommand_Counter = 20;
		#if (API221_CLIENT_FEATURE == ENABLED)
		DM_ReachedTargetSpeed = FALSE;
		#endif
	}

	// Increase counter in every run
	DrainCommand_Counter++;

	if (DrainCommand_Counter >= 20)
	{
		DrainCommand_Counter = 0;

		#if (API221_CLIENT_FEATURE == ENABLED)
		if (command == ON && !DM_ReachedTargetSpeed)
		{
			API221MotionCtrl__DeviceRun(MOTOR_1, speed, acceleration);
		}
		else if (command == OFF && !DM_IsStopped)
		{
			API221MotionCtrl__DeviceStop(MOTOR_1, acceleration);
		}
		#endif
	}

	DrainCommand_Previous = command;
	DrainSpeed_Previous = speed;

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
				 if (event.Motor == MOTOR_0)
				 {
					 WM_IsStopped = TRUE;
					 WM_ReachedTargetSpeed = FALSE;
				 }
				 else if (event.Motor == MOTOR_1)
				 {
					 DM_IsStopped = TRUE;
					 DM_ReachedTargetSpeed = FALSE;
				 }
		 	 }
			break;
		 case API221_EVENT_REACHED_TARGET:
			 {
				 if (event.Motor == MOTOR_0)
				 {
					 WM_IsStopped = FALSE;
					 WM_ReachedTargetSpeed = TRUE;
				 }
				 else if (event.Motor == MOTOR_1)
				 {
					 DM_IsStopped = FALSE;
					 DM_ReachedTargetSpeed = TRUE;
				 }
			 }
			break;
		 case API221_EVENT_RAMP_STARTED:
			 {
				 if (event.Motor == MOTOR_0)
				 {
					 WM_IsStopped = FALSE;
					 WM_ReachedTargetSpeed = FALSE;
				 }
				 else if (event.Motor == MOTOR_1)
				 {
					 DM_IsStopped = FALSE;
					 DM_ReachedTargetSpeed = FALSE;
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
	if (motor == MOTOR_0) // Wash Motor
	{
		WM_IsStopped = BIT_TEST(status,API221_STS_IS_MOTOR_STOPPED) ? (TRUE) : (FALSE);
		WM_ReachedTargetSpeed = BIT_TEST(status,API221_STS_REACHED_TARGET) ? (TRUE) : (FALSE);
	}
	else if (motor == MOTOR_1) // Drain Motor
	{
		DM_IsStopped = BIT_TEST(status,API221_STS_IS_MOTOR_STOPPED) ? (TRUE) : (FALSE);
		DM_ReachedTargetSpeed = BIT_TEST(status,API221_STS_REACHED_TARGET) ? (TRUE) : (FALSE);
	}
}

#endif

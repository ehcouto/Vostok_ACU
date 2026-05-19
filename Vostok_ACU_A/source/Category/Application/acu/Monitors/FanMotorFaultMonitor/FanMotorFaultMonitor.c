/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *
   @startuml{FanMotorFaultMonitor State Machine.jpg}
        title Fan Motor Fault Monitor State Machine

        [*] --> INITIALIZE

        INITIALIZE : entry / InitializeModule()
        INITIALIZE --> NO_FAILURE  : evStartMonitor
        INITIALIZE --> [*]         : evSettingDataMissing

        NO_FAILURE : entry / ResetFailureDetection()
        NO_FAILURE : do /\t CheckForFailure()
        NO_FAILURE --> FAILURE_DETECTED  : evFailureDetected

        FAILURE_DETECTED : entry /\t SetFailureDetection()
        FAILURE_DETECTED : do /\t CheckFailureRemoval()
        FAILURE_DETECTED --> NO_FAILURE  : evNoFailurePresent
        FAILURE_DETECTED --> FAULT       : evDebounceCompleted

		FAULT : exit /\t SetStabilizationTime()
		FAULT : do /\t MonitorFaultState()
		FAULT --> NO_FAILURE : evFanMotorFaultReset

    @enduml

 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "FanMotorFaultMonitor.h"

// -- Other Modules --
#include "Fault.h"
#include "Hbl.h"
#include "Log.h"
#include "Micro.h"
#include "NVData.h"
#include "SettingFile.h"


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Fan Motor Fault Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Fan Motor Fault Monitor state machine.
typedef enum FAN_MOTOR_FAULT_MONITOR_STATE_ENUM
{
    STATE_FAN_MOTOR_FAULT_MONITOR_NONE             = 0,

    STATE_FAN_MOTOR_FAULT_MONITOR_INITIALIZE       = 1, //!< (initial_state)
    STATE_FAN_MOTOR_FAULT_MONITOR_NO_FAILURE       = 2,
    STATE_FAN_MOTOR_FAULT_MONITOR_FAILURE_DETECTED = 3,
    STATE_FAN_MOTOR_FAULT_MONITOR_FAULT            = 4,

    STATE_FAN_MOTOR_FAULT_MONITOR_END              = 5
} FAN_MOTOR_FAULT_MONITOR_STATE_TYPE;

//! The state variable for the Fan Motor Fault Monitor state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static FAN_MOTOR_FAULT_MONITOR_STATE_TYPE Fan_Motor_Fault_Monitor_State;

// -- Private Function Prototypes used by the state machine for customization functions --
static void CheckFailureRemoval(void);
static void CheckForFailure(void);
static void InitializeModule(void);
static void MonitorFaultState(void);
static void ResetFailureDetection(void);
static void SetFailureDetection(void);
static void SetStabilizationTime(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventDebounceCompleted(void);
static void EventFailureDetected(void);
static void EventFanMotorFaultReset(void);
static void EventNoFailurePresent(void);
static void EventSettingDataMissing(void);
static void EventStartMonitor(void);
static void FanMotorFaultMonitorExecute(void);
static void FanMotorFaultMonitorInitialize(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#define MODULE_NAME MODULE_FAN_MOTOR_FAULT_MONITOR

//! Time to wait after a change in fan status or after resetting the fault,
// before evaluating again the fan heater current feedback (unit 100 ms).
#define FAN_MOTOR_FAULT_STABILIZATION_TIME	10

typedef enum MODULE_FAN_MOTOR_FAULT_MONITOR_LOG_MESSAGE_ID_ENUM
{
	ERROR_MISSING_COMPONENTS  = 1,
	ERROR_MISSING_PARAMETERS
} MODULE_FAN_MOTOR_FAULT_MONITOR_LOG_MESSAGE_ID_TYPE;

typedef PACKED struct FAN_MOTOR_FAULT_PARAMS_STRUCT
{
   uint16 Current_Sense_DC_Fan_Min;
   uint16 Current_Sense_DC_Fan_Max;
} FAN_MOTOR_FAULT_PARAMS_TYPE;

// The parameters for detecting the Fan Motor fault as read from Setting File
static FAN_MOTOR_FAULT_PARAMS_TYPE Fan_Fault_Params;
//! Counter for stabilization time.
static uint8 Fan_Fault_Stabilization_Counter;
//! The current on the Fan motor as measured by the Fan Current Sensing circuit (units mA)
static uint16 Measured_Fan_Current;
// State of the Dry Fan in the previous iteration
static uint8 Dry_Fan_Previous_State;
// State of the Dry Fan in the current iteration
static uint8 Dry_Fan_State;

#pragma location = "_appliance_segment"
//! Set to TRUE when an over-current fault is detected, when it is set it prevents the fan from turning on. Reset when a cycle is started.
static BOOL_TYPE Dry_Fan_Disabled;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//----------------------------------------------------------------------------
/**
 *  @brief      FanMotorFaultMonitor__Initialize
 *  @details    Function used to initialize the Fan Motor Fault monitor.
 *
 */
void FanMotorFaultMonitor__Initialize(void)
{
	if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
	{
		if (NVData__GetLastRestoreStatus()->Bit.Appliance_Data_Recovered != (NVDATA_RESTORE_TYPE)NVDATA_RESTORE_OKAY)
		{
			Dry_Fan_Disabled = FALSE;
		}

		FanMotorFaultMonitorInitialize();
	}
}


//----------------------------------------------------------------------------
/**
 *  @brief      FanMotorFaultMonitor__Execution
 *  @details    Function used to perform the Fan Motor Fault monitor.
 *
 */
void FanMotorFaultMonitor__Execution(void)
{
	uint8 fan_current_gi_index;
	uint8 fan_current_sequence_id;
	uint8* dry_fan_state_ptr;

	dry_fan_state_ptr = (uint8 *)Hbl__GetProcessedLoad(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DRY_FAN));

	if (dry_fan_state_ptr != NULL)
	{
		Dry_Fan_State = *dry_fan_state_ptr;

		if (Dry_Fan_State != Dry_Fan_Previous_State)
		{
			Fan_Fault_Stabilization_Counter = FAN_MOTOR_FAULT_STABILIZATION_TIME;
			Dry_Fan_Previous_State = Dry_Fan_State;
		}
		else
		{
			if (Fan_Fault_Stabilization_Counter > 0U)
			{
				Fan_Fault_Stabilization_Counter --;
			}
		}

		fan_current_gi_index = Hbl__GetGIIndexByGIID(HBL_GI_I_DC_FAN);
		fan_current_sequence_id = Hbl__GetGISequenceIDByIndex(fan_current_gi_index);

		if (fan_current_sequence_id > 0U)
		{
			Measured_Fan_Current = (uint16)Hbl__GetGIDataConvertedByIndex(fan_current_gi_index);

			FanMotorFaultMonitorExecute();
		}

		// Turn off the fan if an over-current fault has been detected
		if ((uint8)Dry_Fan_Disabled == (uint8)TRUE)
		{
		  ON_OFF_TYPE dry_fan_state = OFF;

		  (void)(Hbl__SetLoadRequestByLoadID(HBL_LOAD_DRY_FAN, HBL_PRIORITY_FAN_MOTOR_FAULT_MONITOR, (uint8 *)&dry_fan_state)); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
		}
	}
}


//----------------------------------------------------------------------------
/**
 *  @brief      FanMotorFaultMonitor__CycleStarted
 *  @details    Function called when a cycle is started to remove the Fan Motor Fault.
 *
 */
void FanMotorFaultMonitor__CycleStarted(void)
{
	Dry_Fan_Disabled = FALSE;

	EventFanMotorFaultReset();
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void CheckFailureRemoval(void)
{
	if ((uint8)Fault__IsInFaultState(FAULT_ID_FAN_MOTOR_FAILURE) == (uint8)TRUE)
	{
		EventDebounceCompleted();
	}
	else
	{
		if ((Dry_Fan_State == 0U) ||
			(Fault__IsObserverActive(FAULT_ID_FAN_MOTOR_FAILURE) == FALSE) ||
			((Measured_Fan_Current >= Fan_Fault_Params.Current_Sense_DC_Fan_Min) &&
			(Measured_Fan_Current <= Fan_Fault_Params.Current_Sense_DC_Fan_Max)))
		{
			EventNoFailurePresent();
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void CheckForFailure(void)
{
	if ((Dry_Fan_State != 0U) &&
		((Fan_Fault_Stabilization_Counter == 0U) &&
		((Measured_Fan_Current < Fan_Fault_Params.Current_Sense_DC_Fan_Min) ||
		(Measured_Fan_Current > Fan_Fault_Params.Current_Sense_DC_Fan_Max)))
	)
	{
		EventFailureDetected();
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void InitializeModule(void)
{
	uint8 fan_current_gi_index;
	uint8 dry_fan_index;
	FAN_MOTOR_FAULT_PARAMS_TYPE *fan_motor_fault_params;

	Measured_Fan_Current = 0;
	Fan_Fault_Stabilization_Counter = FAN_MOTOR_FAULT_STABILIZATION_TIME;

	Dry_Fan_Previous_State = 0;
	Dry_Fan_State = 0;

	fan_current_gi_index = Hbl__GetGIIndexByGIID(HBL_GI_I_DC_FAN);
	dry_fan_index = Hbl__GetLoadIndexByLoadID(HBL_LOAD_DRY_FAN);

	if ((fan_current_gi_index != (uint8)HBL_INDEX_INVALID) && (dry_fan_index != (uint8)HBL_INDEX_INVALID))
	{
		fan_motor_fault_params = (FAN_MOTOR_FAULT_PARAMS_TYPE*)Fault__GetSpecificData(FAULT_ID_FAN_MOTOR_FAILURE);

		if (fan_motor_fault_params != NULL)
		{
			Fan_Fault_Params.Current_Sense_DC_Fan_Min = ENDIANU16_STON(fan_motor_fault_params->Current_Sense_DC_Fan_Min);
			Fan_Fault_Params.Current_Sense_DC_Fan_Max = ENDIANU16_STON(fan_motor_fault_params->Current_Sense_DC_Fan_Max);

			EventStartMonitor();
		}
		else
		{
			EventSettingDataMissing();
			LOG_ADD_EXCEPTION (ERROR_MISSING_PARAMETERS, FAULT_ID_FAN_MOTOR_FAILURE);
		}
	}
	else
	{
		EventSettingDataMissing();
		LOG_ADD_EXCEPTION (ERROR_MISSING_COMPONENTS, 0);
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void ResetFailureDetection(void)
{
	Fault__ConditionRemoved(FAULT_ID_FAN_MOTOR_FAILURE);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetFailureDetection(void)
{
	Fault__ConditionDetected(FAULT_ID_FAN_MOTOR_FAILURE);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetStabilizationTime(void)
{
	Fan_Fault_Stabilization_Counter = FAN_MOTOR_FAULT_STABILIZATION_TIME;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void MonitorFaultState(void)
{
	if (Measured_Fan_Current > Fan_Fault_Params.Current_Sense_DC_Fan_Max)
	{
		Dry_Fan_Disabled = TRUE;
	}

	if (Fault__IsInFaultState(FAULT_ID_FAN_MOTOR_FAILURE) == FALSE)
	{
		EventFanMotorFaultReset();
	}
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Fan Motor Fault Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Debounce Completed Event for the Fan Motor Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDebounceCompleted(void)
{
    switch (Fan_Motor_Fault_Monitor_State)
    {
        case STATE_FAN_MOTOR_FAULT_MONITOR_FAILURE_DETECTED:
            Fan_Motor_Fault_Monitor_State = STATE_FAN_MOTOR_FAULT_MONITOR_FAULT;
            break;

        default:
            // Event is ignored for all other values of Fan_Motor_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Failure Detected Event for the Fan Motor Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventFailureDetected(void)
{
    switch (Fan_Motor_Fault_Monitor_State)
    {
        case STATE_FAN_MOTOR_FAULT_MONITOR_NO_FAILURE:
            Fan_Motor_Fault_Monitor_State = STATE_FAN_MOTOR_FAULT_MONITOR_FAILURE_DETECTED;
            SetFailureDetection();                  // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Fan_Motor_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Fan Motor Fault Reset Event for the Fan Motor Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventFanMotorFaultReset(void)
{
    switch (Fan_Motor_Fault_Monitor_State)
    {
        case STATE_FAN_MOTOR_FAULT_MONITOR_FAULT:
            SetStabilizationTime();                 // Exit function.
            Fan_Motor_Fault_Monitor_State = STATE_FAN_MOTOR_FAULT_MONITOR_NO_FAILURE;
            ResetFailureDetection();                // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Fan_Motor_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the No Failure Present Event for the Fan Motor Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNoFailurePresent(void)
{
    switch (Fan_Motor_Fault_Monitor_State)
    {
        case STATE_FAN_MOTOR_FAULT_MONITOR_FAILURE_DETECTED:
            Fan_Motor_Fault_Monitor_State = STATE_FAN_MOTOR_FAULT_MONITOR_NO_FAILURE;
            ResetFailureDetection();                // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Fan_Motor_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Setting Data Missing Event for the Fan Motor Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventSettingDataMissing(void)
{
    switch (Fan_Motor_Fault_Monitor_State)
    {
        case STATE_FAN_MOTOR_FAULT_MONITOR_INITIALIZE:
            Fan_Motor_Fault_Monitor_State = STATE_FAN_MOTOR_FAULT_MONITOR_NONE; // Terminate Fan Motor Fault Monitor state machine execution.
            break;

        default:
            // Event is ignored for all other values of Fan_Motor_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Start Monitor Event for the Fan Motor Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventStartMonitor(void)
{
    switch (Fan_Motor_Fault_Monitor_State)
    {
        case STATE_FAN_MOTOR_FAULT_MONITOR_INITIALIZE:
            Fan_Motor_Fault_Monitor_State = STATE_FAN_MOTOR_FAULT_MONITOR_NO_FAILURE;
            ResetFailureDetection();                // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Fan_Motor_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Fan Motor Fault Monitor state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void FanMotorFaultMonitorExecute(void)
{
    switch(Fan_Motor_Fault_Monitor_State)
    {
        case STATE_FAN_MOTOR_FAULT_MONITOR_NO_FAILURE:
            CheckForFailure();                      // Do function.
            break;

        case STATE_FAN_MOTOR_FAULT_MONITOR_FAILURE_DETECTED:
            CheckFailureRemoval();                  // Do function.
            break;

        case STATE_FAN_MOTOR_FAULT_MONITOR_FAULT:
            MonitorFaultState();                    // Do function.
            break;

        default:
            // There is nothing to do for all other values of Fan_Motor_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Fan Motor Fault Monitor state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void FanMotorFaultMonitorInitialize(void)
{
    Fan_Motor_Fault_Monitor_State = STATE_FAN_MOTOR_FAULT_MONITOR_INITIALIZE;
    InitializeModule();                             // Entry Function.
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

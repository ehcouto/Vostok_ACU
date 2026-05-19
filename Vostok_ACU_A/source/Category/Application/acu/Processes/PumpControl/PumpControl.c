/**
 *  @file
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
/*
@startuml
        Title PumpControl State Machine
		' Use state = This_Pc->Pump_Control_State

        [*] --> INITIALIZE

        INITIALIZE : entry / ReadProcessParameters()
        INITIALIZE --> READ_STATUS : ParameterOk
        INITIALIZE --> RUN : ParameterOkNoExternalMotor
        INITIALIZE --> EXIT : ParameterKo

		READ_STATUS : entry / SendRequestStatus() / StartTimerWaitStatus()
        READ_STATUS : do / CheckStatus()
        READ_STATUS --> RUN : Run
        READ_STATUS --> WAIT_STOPPED : WaitStopped
        READ_STATUS --> EXIT : Error
        READ_STATUS --> READ_STATUS : ErrorWaitStatus
        READ_STATUS --> WAIT_MCU_RESET : McuReset

        RUN : do / SendRun()
        RUN --> WAIT_MCU_RESET : McuReset

		WAIT_MCU_RESET : entry / StartTimerWaitMcuReset()
		WAIT_MCU_RESET : do / WaitMcuResetTimer()
		WAIT_MCU_RESET --> READ_STATUS : Restart

		WAIT_STOPPED : entry / StartTimerWaitStopped()
        WAIT_STOPPED : do / WaitStopped()
        WAIT_STOPPED --> RUN : Run
        WAIT_STOPPED --> READ_STATUS : ErrorWaitStopped
        WAIT_STOPPED --> EXIT : Error

        EXIT: do / returnTRUE()
@enduml
*/
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "PumpControl.h"

#include "PumpCommand.h"
#include "C_Extensions.h"
#if (API221_CLIENT_FEATURE == ENABLED)
#include "API221MotionCtrl.h"
#endif /*(API221_CLIENT_FEATURE == ENABLED)*/
#include "AlternateCtrl.h"
#include "DishAuxVar.h"
#include "Fault.h"
#include "Interpreter.h"
#include "Micro.h"
#include "Modifier.h"
#include "NVData.h"
#include "PumpFlushRequestMonitor.h"
#include "Selector.h"
#include "StabilityControl.h"
#include "SystemTimers.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Pump Control State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Pump Control state machine.
typedef enum PUMP_CONTROL_STATE_ENUM
{
    STATE_PUMP_CONTROL_NONE           = 0,

    STATE_PUMP_CONTROL_INITIALIZE     = 1,          //!< (initial_state)
    STATE_PUMP_CONTROL_READ_STATUS    = 2,
    STATE_PUMP_CONTROL_RUN            = 3,
    STATE_PUMP_CONTROL_EXIT           = 4,
    STATE_PUMP_CONTROL_WAIT_STOPPED   = 5,
    STATE_PUMP_CONTROL_WAIT_MCU_RESET = 6,

    STATE_PUMP_CONTROL_END            = 7
} PUMP_CONTROL_STATE_TYPE;

//! The transition variable for the Pump Control state machine.
//! Flag used by the state machine to track when a transition occurs in the middle of a state's
//! Entry() or Do() action that contains more than one function.
static BOOL_TYPE Pump_Control_Transitioned;

// -- Private Function Prototypes used by the state machine for customization functions --
static void CheckStatus(void);
static void ReadProcessParameters(void);
static void ReturnTRUE(void);
static void SendRequestStatus(void);
static void SendRun(void);
static void StartTimerWaitMcuReset(void);
static void StartTimerWaitStatus(void);
static void StartTimerWaitStopped(void);
static void WaitMcuResetTimer(void);
static void WaitStopped(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventError(void);
static void EventErrorWaitStatus(void);
static void EventErrorWaitStopped(void);
static void EventMcuReset(void);
static void EventParameterKo(void);
static void EventParameterOk(void);
static void EventParameterOkNoExternalMotor(void);
static void EventRestart(void);
static void EventRun(void);
static void EventWaitStopped(void);
static void PumpControlExecute(void);
static void PumpControlInitialize(void);
static void PumpControlTerminate(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

//! The maximum number of times that the PumpControl process can be used in the same step.
#ifndef PUMP_CONTROL_MAX_INSTANCES
    //! Set this value to the maximum number of instances of the PumpControl process that can be
    //! executed in a single step of a cycle.
    //! For example, "2" means that the same step can use two instances of the PumpControl process.
    //! One instance could control a load with hysteresis while the other turned loads on.
    #define PUMP_CONTROL_MAX_INSTANCES 2
#endif

//! The name of this module for use by the Log module.
#define MODULE_NAME MODULE_PUMPCONTROL

//! Displacement value that indicates that the feature is not used.
#define UNUSED_FEATURE_BYTE 0xFF

//! The types of OWI variations that are available.
typedef enum PUMP_CONTROL_OPTION_ENUM
{
	WASH_PUMP_CONTROL_ACTIVATE,                          //!< F1=0, F0=0
	DRAIN_PUMP_CONTROL_ACTIVATE,						 //!< F1=0, F0=1
	PUMP_CONTROL_OPTION_COUNT
} PUMP_CONTROL_OPTION_TYPE;

//! A list of log message IDs that are generated by this module.
typedef enum MODULE_PUMPCONTROL_LOG_MESSAGE_ID_ENUM
{
    ERROR_PROCESS_CONTROL       = 1,
    ERROR_LOADING_PARAMETERS,
	ERROR_WAIT_STOPPED,
	ERROR_WAIT_STOPPED_MAX_ATTEMPS,
	ERROR_WAIT_STATUS,
	ERROR_WAIT_STATUS_MAX_ATTEMPS,
	ERROR_MCU_RESET
} MODULE_PUMPCONTROL_LOG_MESSAGE_ID_TYPE;

typedef PACKED struct PUMP_CONTROL_PROCESS_PARAMETER_STRUCT
{
    uint8 Speed;
    uint8 Speed_Modifiers_Table_Index;
    uint16 Motor_Acceleration;
    uint8 Motor_Acceleration_Modifiers_Table_Index;
    uint8 Unused					   : 7;
    uint8 Apply_Modifiers_Continuously : 1;
} PUMP_CONTROL_PROCESS_PARAMETER_TYPE;

typedef enum PUMP_STATE_ENUM
{
    STATE_OFF = 0,
    STATE_ON
}PUMP_STATE_TYPE;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
//! The process control structure used by all the state machine functions.
//! The RAM data structure required for each instance of the Loads Control process.
typedef struct PUMP_CONTROL_INSTANCE_STRUCT
{
	PROCESS_CONTROL_TYPE* Control;
	BOOL_TYPE PumpControl_Ready_To_Exit;
	PUMP_CONTROL_PROCESS_PARAMETER_TYPE* PumpControl_Parameters;

	//! The state variable for the Pump Control state machine.
	//! State machines with a history pseudostate may write to this variable during initialization.
	//! Other state machines must only write to this variable through auto-generated functions.
	PUMP_CONTROL_STATE_TYPE Pump_Control_State;

	uint16 PC_Motor_Speed;
	uint16 PC_Motor_Acceleration;
	uint16 PC_Motor_Speed_Before_Reduction;
#if (API221_CLIENT_FEATURE == ENABLED)
	uint32 PC_Status[MOTOR_MAX];
	BOOL_TYPE PC_PendingStatus[MOTOR_MAX];
#endif
	HBL_PILOT_ENUM_TYPE PC_Pump_Pilot_Type;
	uint8 PC_Wait_Stopped_Attempts;
	uint8 PC_Wait_Status_Attempts;
} PUMP_CONTROL_INSTANCE_TYPE;

// -- Private Variable Definitions --

static PUMP_CONTROL_INSTANCE_TYPE Pump_Control[APPLIANCE_MAX_COMPARTMENTS][PUMP_CONTROL_MAX_INSTANCES];
static PUMP_CONTROL_INSTANCE_TYPE* This_Pc;

#pragma location = "_appliance_segment"
static uint16 Previous_Wash_Motor_Speed;
#pragma location = "_appliance_segment"
static uint16 Previous_Drain_Motor_Speed;
#pragma location = "_appliance_segment"
static BOOL_TYPE PC_Wash_Pump_Deblocking_Ready;
#pragma location = "_appliance_segment"
static BOOL_TYPE PC_Drain_Pump_Deblocking_Ready;

static CYCLE_NAME_TYPE PC_Subcycle_Name;

static BOOL_TYPE VSM_Drain_Is_Running;
static SYSTEMTIMER_NV_TIMER_TYPE PC_Wait_Stopped_Timer_Handle;
static SYSTEMTIMER_NV_TIMER_TYPE PC_Wait_Status_Timer_Handle;
static SYSTEMTIMER_NV_TIMER_TYPE PC_Wait_MCU_Reset_Timer_Handle;



//  --- Private Function Prototypes -----------------------------------------------------------------------------------

#if (API221_CLIENT_FEATURE == ENABLED)
static void API221ApplReceivedStatus (API221_CLIENT_MOTOR_INDEX_TYPE motor, uint32 status);
static void API221HandleMotorEvent(API221_EVENT_TYPE event);
static BOOL_TYPE Double_VSM_Pump_Cannot_Run(PROCESS_CONTROL_TYPE* process_control);
#endif

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module Fill and its variables
 *
 */
void PumpControl__Initialize(void)
{
	if ((Micro__GetResetMode() == MICRO_RESET_MODE_COLD) &&
		(NVData__GetLastRestoreStatus()->Bit.Appliance_Data_Recovered != (NVDATA_RESTORE_TYPE)NVDATA_RESTORE_OKAY))
	{
		Previous_Wash_Motor_Speed = 0;
		Previous_Drain_Motor_Speed = 0;
		PC_Wash_Pump_Deblocking_Ready = FALSE;
		PC_Drain_Pump_Deblocking_Ready = FALSE;
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module OWI and its variables
 *
 */

PROCESS_RESULT_TYPE PumpControl__Process(PROCESS_CONTROL_TYPE* control)
{
    PROCESS_RESULT_TYPE result;
    PUMP_CONTROL_OPTION_TYPE option = (PUMP_CONTROL_OPTION_TYPE)control->F1_F0_Flags;
    if ((control == NULL ) ||
        (control->Compartment >= APPLIANCE_MAX_COMPARTMENTS) ||
        (option >= PUMP_CONTROL_OPTION_COUNT)||
        (control->Process_Instance >= (uint8)PUMP_CONTROL_MAX_INSTANCES)
		#if (API221_CLIENT_FEATURE == ENABLED)
		|| ((uint8)Double_VSM_Pump_Cannot_Run(control) == (uint8)TRUE)
		#endif
		)
    {
        LOG_ADD_EXCEPTION(ERROR_PROCESS_CONTROL, (control != NULL));
    }
    else
    {
    	// Setup variables for the current compartment.
    	This_Pc = &Pump_Control[control->Compartment][control->Process_Instance];

    	This_Pc->Control = control;

		if (((uint8)control->Step_Initializing == (uint8)TRUE) ||
			((uint8)control->Resume_From_Reset == (uint8)TRUE) ||
			((uint8)control->Resume_From_Pause == (uint8)TRUE))
		{
			#if (API221_CLIENT_FEATURE == ENABLED)
			AlternateCtrl__UnregisterApi221Events();
			PumpControl__UnregisterApi221Events();
			#endif

			if (option == WASH_PUMP_CONTROL_ACTIVATE)
			{
				This_Pc->PC_Pump_Pilot_Type = Hbl__GetPilotIDByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_WASH_PUMP));
				#if (API221_CLIENT_FEATURE == ENABLED)
				API221MotionCtrl__RegisterForEvents(API221HandleMotorEvent);
				This_Pc->PC_PendingStatus[MOTOR_0] = FALSE;
				#endif
			}
			else if (option == DRAIN_PUMP_CONTROL_ACTIVATE)
			{
				This_Pc->PC_Pump_Pilot_Type = Hbl__GetPilotIDByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DRAIN_PUMP));
				#if (API221_CLIENT_FEATURE == ENABLED)
				API221MotionCtrl__RegisterForEvents(API221HandleMotorEvent);
				This_Pc->PC_PendingStatus[MOTOR_1] = FALSE;
				#endif
			}
			// Initialize this instance of the PumpControl process.
			This_Pc->PumpControl_Ready_To_Exit = FALSE;

			This_Pc->PC_Wait_Stopped_Attempts = 0;
			This_Pc->PC_Wait_Status_Attempts = 0;

			PumpControlInitialize();
		}
		else
		{
			PumpControlExecute();
		}

		VSM_Drain_Is_Running = FALSE;
		#if (API221_CLIENT_FEATURE == ENABLED)
		if (This_Pc->PC_Pump_Pilot_Type == HBL_PILOT_EXTERNAL_MOTOR)
		{
			if (option == WASH_PUMP_CONTROL_ACTIVATE)
			{
				PumpFlushRequestMonitor__SetWashPumpCtrlExecutedFlag();
				PumpCommand__MaintainPreviousCommand(HBL_PRIORITY_APPLICATION_MID, HBL_LOAD_WASH_PUMP);
			}
			else if (option == DRAIN_PUMP_CONTROL_ACTIVATE)
			{
				PumpFlushRequestMonitor__SetDrainPumpCtrlExecutedFlag();
				PumpCommand__MaintainPreviousCommand(HBL_PRIORITY_APPLICATION_MID, HBL_LOAD_DRAIN_PUMP);

				PumpControl__SetVSMDrainIsRunning(TRUE);
			}
		}
		#endif
    }//*/

    result = ((uint8)This_Pc->PumpControl_Ready_To_Exit == (uint8)TRUE)? PROCESS_COMPLETE : PROCESS_BUSY;
    return result;
}

void PumpControl__SetPreviousWashMotorSpeed(uint16 speed)
{
	Previous_Wash_Motor_Speed = speed;
}

void PumpControl__SetPreviousDrainMotorSpeed(uint16 speed)
{
	Previous_Drain_Motor_Speed = speed;
}

uint16 PumpControl__GetPreviousWashMotorSpeed(void)
{
	return Previous_Wash_Motor_Speed;
}

uint16 PumpControl__GetPreviousDrainMotorSpeed(void)
{
	return Previous_Drain_Motor_Speed;
}

#if (API221_CLIENT_FEATURE == ENABLED)
void PumpControl__UnregisterApi221Events(void)
{
	API221MotionCtrl__UnregisterForEvents(API221HandleMotorEvent);
}
#endif

BOOL_TYPE PumpControl__GetVSMDrainIsRunning(void)
{
	return VSM_Drain_Is_Running;
}

void PumpControl__SetVSMDrainIsRunning(BOOL_TYPE value)
{
	#if (API221_CLIENT_FEATURE == ENABLED)
	VSM_Drain_Is_Running = value;
	#endif
}

void PumpControl__SetWashPumpDeblockingReady(BOOL_TYPE value)
{
	PC_Wash_Pump_Deblocking_Ready = value;
}

void PumpControl__SetDrainPumpDeblockingReady(BOOL_TYPE value)
{
	PC_Drain_Pump_Deblocking_Ready = value;
}


//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Pump Control State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Error Event for the Pump Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventError(void)
{
    switch (This_Pc->Pump_Control_State)
    {
        case STATE_PUMP_CONTROL_READ_STATUS:
        case STATE_PUMP_CONTROL_WAIT_STOPPED:
            This_Pc->Pump_Control_State = STATE_PUMP_CONTROL_EXIT;
            Pump_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of This_Pc->Pump_Control_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Error Wait Status Event for the Pump Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventErrorWaitStatus(void)
{
    switch (This_Pc->Pump_Control_State)
    {
        case STATE_PUMP_CONTROL_READ_STATUS:
            This_Pc->Pump_Control_State = STATE_PUMP_CONTROL_READ_STATUS;
            Pump_Control_Transitioned = FALSE;
            SendRequestStatus();                    // Entry Function.
            if (Pump_Control_Transitioned == FALSE)
            {
                StartTimerWaitStatus();             // Entry Function.
            }
            Pump_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of This_Pc->Pump_Control_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Error Wait Stopped Event for the Pump Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventErrorWaitStopped(void)
{
    switch (This_Pc->Pump_Control_State)
    {
        case STATE_PUMP_CONTROL_WAIT_STOPPED:
            This_Pc->Pump_Control_State = STATE_PUMP_CONTROL_READ_STATUS;
            Pump_Control_Transitioned = FALSE;
            SendRequestStatus();                    // Entry Function.
            if (Pump_Control_Transitioned == FALSE)
            {
                StartTimerWaitStatus();             // Entry Function.
            }
            Pump_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of This_Pc->Pump_Control_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Mcu Reset Event for the Pump Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventMcuReset(void)
{
    switch (This_Pc->Pump_Control_State)
    {
        case STATE_PUMP_CONTROL_READ_STATUS:
        case STATE_PUMP_CONTROL_RUN:
            This_Pc->Pump_Control_State = STATE_PUMP_CONTROL_WAIT_MCU_RESET;
            StartTimerWaitMcuReset();               // Entry Function.
            Pump_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of This_Pc->Pump_Control_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Parameter Ko Event for the Pump Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventParameterKo(void)
{
    switch (This_Pc->Pump_Control_State)
    {
        case STATE_PUMP_CONTROL_INITIALIZE:
            This_Pc->Pump_Control_State = STATE_PUMP_CONTROL_EXIT;
            Pump_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of This_Pc->Pump_Control_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Parameter Ok Event for the Pump Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventParameterOk(void)
{
    switch (This_Pc->Pump_Control_State)
    {
        case STATE_PUMP_CONTROL_INITIALIZE:
            This_Pc->Pump_Control_State = STATE_PUMP_CONTROL_READ_STATUS;
            Pump_Control_Transitioned = FALSE;
            SendRequestStatus();                    // Entry Function.
            if (Pump_Control_Transitioned == FALSE)
            {
                StartTimerWaitStatus();             // Entry Function.
            }
            Pump_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of This_Pc->Pump_Control_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Parameter Ok No External Motor Event for the Pump Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventParameterOkNoExternalMotor(void)
{
    switch (This_Pc->Pump_Control_State)
    {
        case STATE_PUMP_CONTROL_INITIALIZE:
            This_Pc->Pump_Control_State = STATE_PUMP_CONTROL_RUN;
            Pump_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of This_Pc->Pump_Control_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Restart Event for the Pump Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventRestart(void)
{
    switch (This_Pc->Pump_Control_State)
    {
        case STATE_PUMP_CONTROL_WAIT_MCU_RESET:
            This_Pc->Pump_Control_State = STATE_PUMP_CONTROL_READ_STATUS;
            Pump_Control_Transitioned = FALSE;
            SendRequestStatus();                    // Entry Function.
            if (Pump_Control_Transitioned == FALSE)
            {
                StartTimerWaitStatus();             // Entry Function.
            }
            Pump_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of This_Pc->Pump_Control_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Run Event for the Pump Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventRun(void)
{
    switch (This_Pc->Pump_Control_State)
    {
        case STATE_PUMP_CONTROL_READ_STATUS:
        case STATE_PUMP_CONTROL_WAIT_STOPPED:
            This_Pc->Pump_Control_State = STATE_PUMP_CONTROL_RUN;
            Pump_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of This_Pc->Pump_Control_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Wait Stopped Event for the Pump Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventWaitStopped(void)
{
    switch (This_Pc->Pump_Control_State)
    {
        case STATE_PUMP_CONTROL_READ_STATUS:
            This_Pc->Pump_Control_State = STATE_PUMP_CONTROL_WAIT_STOPPED;
            StartTimerWaitStopped();                // Entry Function.
            Pump_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of This_Pc->Pump_Control_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Pump Control state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void PumpControlExecute(void)
{
    Pump_Control_Transitioned = FALSE;
    switch(This_Pc->Pump_Control_State)
    {
        case STATE_PUMP_CONTROL_READ_STATUS:
            CheckStatus();                          // Do function.
            break;

        case STATE_PUMP_CONTROL_RUN:
            SendRun();                              // Do function.
            break;

        case STATE_PUMP_CONTROL_EXIT:
            ReturnTRUE();                           // Do function.
            break;

        case STATE_PUMP_CONTROL_WAIT_STOPPED:
            WaitStopped();                          // Do function.
            break;

        case STATE_PUMP_CONTROL_WAIT_MCU_RESET:
            WaitMcuResetTimer();                    // Do function.
            break;

        default:
            // There is nothing to do for all other values of This_Pc->Pump_Control_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Pump Control state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void PumpControlInitialize(void)
{
    This_Pc->Pump_Control_State = STATE_PUMP_CONTROL_INITIALIZE;
    ReadProcessParameters();                        // Entry Function.
    Pump_Control_Transitioned = TRUE;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The termination function for the Pump Control state machine.
 * This function disables the state machine. Calls to StateMachineExecute() will have no effect.
 * The StateMachineInitialize() function must be called again to restart the state machine.
 * If the current state has an exit function, it will be called.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void PumpControlTerminate(void)
{
    // None of the states have an Exit function.
    This_Pc->Pump_Control_State = STATE_PUMP_CONTROL_NONE;
    Pump_Control_Transitioned = TRUE;
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void StartTimerWaitStatus(void)
{
	(void)SystemTimers__NVSetTimerMS(&PC_Wait_Status_Timer_Handle, 10000,SYSTEMTIMERS_TYPE_DOWN_COUNTER);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void CheckStatus(void)
{
	/*
	API221_STS_ANY_ERROR_FOUND      = 0,             Any error found?
	API221_STS_IS_MOTOR_STOPPED     = 1,             Is motor stopped?
	API221_STS_REACHED_TARGET       = 2,             Target velocity == reference velocity
	API221_STS_IS_IDLE              = 3,             Is the motor idle? If the speed is zero, no error found...
	API221_STS_MOTOR_RUNNING        = 4
	*/
	#if (API221_CLIENT_FEATURE == ENABLED)
	HBL_PILOT_ENUM_TYPE drain_pump_pilot_type = Hbl__GetPilotIDByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DRAIN_PUMP));

	if (drain_pump_pilot_type == HBL_PILOT_EXTERNAL_MOTOR)
	{
		if ((This_Pc->PC_PendingStatus[MOTOR_0]) && (This_Pc->PC_PendingStatus[MOTOR_1]))
		{

			if (BIT_TEST(This_Pc->PC_Status[MOTOR_0],API221_STS_REACHED_TARGET) == 0U)
			{
				PumpControl__SetPreviousWashMotorSpeed(0);
			}

			if (BIT_TEST(This_Pc->PC_Status[MOTOR_1],API221_STS_REACHED_TARGET) == 0U)
			{
				PumpControl__SetPreviousDrainMotorSpeed(0);
			}

			if (((BIT_TEST(This_Pc->PC_Status[MOTOR_0],API221_STS_MOTOR_RUNNING) != 0U) && (BIT_TEST(This_Pc->PC_Status[MOTOR_0],API221_STS_REACHED_TARGET) == 0U)) ||
				((BIT_TEST(This_Pc->PC_Status[MOTOR_1],API221_STS_MOTOR_RUNNING) != 0U) && (BIT_TEST(This_Pc->PC_Status[MOTOR_1],API221_STS_REACHED_TARGET) == 0U)))
			{
				if (((uint8)PumpFlushRequestMonitor__GetWashStopSentFlag() == (uint8)TRUE) ||
					((uint8)PumpFlushRequestMonitor__GetDrainStopSentFlag() == (uint8)TRUE))
				{
					EventWaitStopped();
				}
				else
				{
					EventRun();
				}
			}
			else if (((BIT_TEST(This_Pc->PC_Status[MOTOR_0],API221_STS_MOTOR_RUNNING) != 0U) && (BIT_TEST(This_Pc->PC_Status[MOTOR_0],API221_STS_REACHED_TARGET) != 0U)) &&
					(This_Pc->Control->F1_F0_Flags == (uint8)DRAIN_PUMP_CONTROL_ACTIVATE))
			{
				EventWaitStopped();
			}
			else if (((BIT_TEST(This_Pc->PC_Status[MOTOR_1],API221_STS_MOTOR_RUNNING) != 0U) && (BIT_TEST(This_Pc->PC_Status[MOTOR_1],API221_STS_REACHED_TARGET) != 0U)) &&
					(This_Pc->Control->F1_F0_Flags == (uint8)WASH_PUMP_CONTROL_ACTIVATE))
			{
				EventWaitStopped();
			}
			else
			{
				EventRun();
			}

			PumpFlushRequestMonitor__ResetWashStopSentFlag();
			PumpFlushRequestMonitor__ResetDrainStopSentFlag();

			This_Pc->PC_PendingStatus[MOTOR_0] = FALSE;
			This_Pc->PC_PendingStatus[MOTOR_1] = FALSE;
		}
	}
	else // NO VSM DRAIN PUMP
	{
		if (This_Pc->PC_PendingStatus[MOTOR_0])
		{
			if (BIT_TEST(This_Pc->PC_Status[MOTOR_0],API221_STS_REACHED_TARGET) == 0U)
			{
				PumpControl__SetPreviousWashMotorSpeed(0);
			}

			if ((BIT_TEST(This_Pc->PC_Status[MOTOR_0],API221_STS_MOTOR_RUNNING) != 0U) && (BIT_TEST(This_Pc->PC_Status[MOTOR_0],API221_STS_REACHED_TARGET) == 0U))
			{
				if ((uint8)PumpFlushRequestMonitor__GetWashStopSentFlag() == (uint8)TRUE)
				{
					EventWaitStopped();
				}
				else
				{
					EventRun();
				}
			}
			else
			{
				EventRun();
			}

			PumpFlushRequestMonitor__ResetWashStopSentFlag();

			This_Pc->PC_PendingStatus[MOTOR_0] = FALSE;
		}
	}

	if (SystemTimers__NVCheckTimer(&PC_Wait_Status_Timer_Handle) == SYSTEMTIMERS_STATE_DONE)
	{
		This_Pc->PC_Wait_Status_Attempts++;

		if (This_Pc->PC_Wait_Status_Attempts < (uint8)MAX_WAIT_STATUS_ATTEMPS)
		{
			EventErrorWaitStatus();
			LOG_ADD_EXCEPTION(ERROR_WAIT_STATUS, This_Pc->PC_Wait_Status_Attempts);
		}
		else
		{
			EventError(); // Process terminate
			LOG_ADD_EXCEPTION(ERROR_WAIT_STATUS_MAX_ATTEMPS, This_Pc->PC_Wait_Status_Attempts);
		}
	}
	#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void ReturnTRUE(void)
{
	This_Pc->PumpControl_Ready_To_Exit = TRUE;
	PumpControlTerminate();
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SendRequestStatus(void)
{
	#if (API221_CLIENT_FEATURE == ENABLED)
	if (This_Pc->Control->F1_F0_Flags == (uint8)WASH_PUMP_CONTROL_ACTIVATE)
	{
		HBL_PILOT_ENUM_TYPE drain_pump_pilot_type = Hbl__GetPilotIDByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DRAIN_PUMP));

		(void)API221MotionCtrl__RequestStatus(MOTOR_0, API221ApplReceivedStatus);

		if (drain_pump_pilot_type == HBL_PILOT_EXTERNAL_MOTOR)
		{
			(void)API221MotionCtrl__RequestStatus(MOTOR_1, API221ApplReceivedStatus);
		}
	}
	else if (This_Pc->Control->F1_F0_Flags == (uint8)DRAIN_PUMP_CONTROL_ACTIVATE)
	{
		(void)API221MotionCtrl__RequestStatus(MOTOR_0, API221ApplReceivedStatus);

		(void)API221MotionCtrl__RequestStatus(MOTOR_1, API221ApplReceivedStatus);
	}
	#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SendRun(void)
{
	if (This_Pc->PC_Pump_Pilot_Type == HBL_PILOT_EXTERNAL_MOTOR)
	{
		uint16 temp_motor_speed;

		if ((uint8)This_Pc->PumpControl_Parameters->Apply_Modifiers_Continuously == (uint8)TRUE)
		{
		    sint32 modified_value;
			if ((uint8)UNUSED_FEATURE_BYTE != This_Pc->PumpControl_Parameters->Speed_Modifiers_Table_Index)
			{
				modified_value = Modifier__GetValue(((sint32)This_Pc->PumpControl_Parameters->Speed*(sint32)25), This_Pc->PumpControl_Parameters->Speed_Modifiers_Table_Index);
				if (modified_value < 0)
				{
					modified_value = 0;
				}
				This_Pc->PC_Motor_Speed_Before_Reduction = (uint16)modified_value;
			}

			if ((uint8)UNUSED_FEATURE_BYTE != This_Pc->PumpControl_Parameters->Motor_Acceleration_Modifiers_Table_Index)
			{
				modified_value = Modifier__GetValue((sint32)(This_Pc->PumpControl_Parameters->Motor_Acceleration), This_Pc->PumpControl_Parameters->Motor_Acceleration_Modifiers_Table_Index);
				if (modified_value < 0)
				{
					modified_value = 0;
				}
				This_Pc->PC_Motor_Acceleration = (uint16)modified_value;
			}
		}

		temp_motor_speed = This_Pc->PC_Motor_Speed_Before_Reduction;

		// Check speed reduction from Stability Control (only for wash pump)
		if (This_Pc->Control->F1_F0_Flags == (uint8)WASH_PUMP_CONTROL_ACTIVATE)
		{
			if (StabilityControl__GetSpeedReduction() != SC_NO_SPEED_REDUCTION)
			{
				StabilityControl__ReduceSpeed(&temp_motor_speed, DishAuxVar__GetDiverterPosition().Position);
			}
		}

		if (temp_motor_speed < (uint16)MIN_SPEED)
		{
			temp_motor_speed = MIN_SPEED;
		}
		else if (temp_motor_speed > (uint16)MAX_SPEED)
		{
			temp_motor_speed = MAX_SPEED;
		}

		if (This_Pc->Control->F1_F0_Flags == (uint8)WASH_PUMP_CONTROL_ACTIVATE)
		{
			if (temp_motor_speed != This_Pc->PC_Motor_Speed)
			{
				// Target speed changed, reset Previous_Wash_Motor_Speed (needed if the target is not reached by the end of the step)
				PumpControl__SetPreviousWashMotorSpeed(0);

				This_Pc->PC_Motor_Speed = temp_motor_speed;
			}

			if (Interpreter__IsSubCycleRunning(COMPARTMENT_DEFAULT) &&
				((uint8)PC_Wash_Pump_Deblocking_Ready == (uint8)TRUE))
			{
				CYCLE_NAME_TYPE cycle_name = Selector__GetCycleNameId(Interpreter__GetCurrentSFIndex(COMPARTMENT_DEFAULT),
																		  COMPARTMENT_DEFAULT,
																		  Interpreter__GetStatus().Sub_Cycle[COMPARTMENT_DEFAULT]);

				if ((cycle_name == CYCLE_NAME_WASH_MOTOR_FAULT) &&
				   ((Fault__IsObserverActive(FAULT_ID_MCU_WM_MCI_INVERTER_OVER_TEMPERATURE)) ||
					(Fault__IsObserverActive(FAULT_ID_VS_WASH_MOTOR_MCI_LOCKED_ROTOR))||
					(Fault__IsObserverActive(FAULT_ID_VS_WASH_MOTOR_MCI_MOTOR_OVERLOAD))||
					(Fault__IsObserverActive(FAULT_ID_MCU_WM_MCI_DRIVER_FAILURE))||
					(Fault__IsObserverActive(FAULT_ID_VS_WASH_MOTOR_MCI_SOFTWARE_OVERCURRENT))||
					(Fault__IsObserverActive(FAULT_ID_MCU_WM_MCI_HARDWARE_OVERCURRENT))))
				{
					PumpCommand__Deblocking((sint16)This_Pc->PC_Motor_Speed,This_Pc->PC_Motor_Acceleration, HBL_PRIORITY_APPLICATION_HI,HBL_LOAD_WASH_PUMP);
				}
				else
				{
					PumpCommand__Run((sint16)This_Pc->PC_Motor_Speed,This_Pc->PC_Motor_Acceleration, HBL_PRIORITY_APPLICATION_HI,HBL_LOAD_WASH_PUMP);
				}
			}
			else
			{
				PumpCommand__Run((sint16)This_Pc->PC_Motor_Speed,This_Pc->PC_Motor_Acceleration, HBL_PRIORITY_APPLICATION_HI,HBL_LOAD_WASH_PUMP);
			}
		}
		else if (This_Pc->Control->F1_F0_Flags == (uint8)DRAIN_PUMP_CONTROL_ACTIVATE)
		{
			if (temp_motor_speed != This_Pc->PC_Motor_Speed)
			{
				// Target speed changed, reset Previous_Drain_Motor_Speed (needed if the target is not reached by the end of the step)
				PumpControl__SetPreviousDrainMotorSpeed(0);

				This_Pc->PC_Motor_Speed = temp_motor_speed;
			}
		
			if (Interpreter__IsSubCycleRunning(COMPARTMENT_DEFAULT) &&
				((uint8)PC_Drain_Pump_Deblocking_Ready == (uint8)TRUE))
			{
				CYCLE_NAME_TYPE cycle_name = Selector__GetCycleNameId(Interpreter__GetCurrentSFIndex(COMPARTMENT_DEFAULT),
																		  COMPARTMENT_DEFAULT,
																		  Interpreter__GetStatus().Sub_Cycle[COMPARTMENT_DEFAULT]);

				if ((cycle_name == CYCLE_NAME_DRAIN_MOTOR_FAULT) &&
				   ((Fault__IsObserverActive(FAULT_ID_MCU_DM_MCI_INVERTER_OVER_TEMPERATURE)) ||
					(Fault__IsObserverActive(FAULT_ID_VS_DRAIN_MOTOR_MCI_LOCKED_ROTOR))||
					(Fault__IsObserverActive(FAULT_ID_VS_DRAIN_MOTOR_MCI_MOTOR_OVERLOAD))||
					(Fault__IsObserverActive(FAULT_ID_MCU_DM_MCI_DRIVER_FAILURE))||
					(Fault__IsObserverActive(FAULT_ID_VS_DRAIN_MOTOR_MCI_SOFTWARE_OVERCURRENT))||
					(Fault__IsObserverActive(FAULT_ID_MCU_DM_MCI_HARDWARE_OVERCURRENT))))
				{
					PumpCommand__Deblocking((sint16)This_Pc->PC_Motor_Speed,This_Pc->PC_Motor_Acceleration, HBL_PRIORITY_APPLICATION_HI,HBL_LOAD_DRAIN_PUMP);
				}
				else
				{
					PumpCommand__Run((sint16)This_Pc->PC_Motor_Speed,This_Pc->PC_Motor_Acceleration, HBL_PRIORITY_APPLICATION_HI,HBL_LOAD_DRAIN_PUMP);
				}
			}
			else
			{
				PumpCommand__Run((sint16)This_Pc->PC_Motor_Speed,This_Pc->PC_Motor_Acceleration, HBL_PRIORITY_APPLICATION_HI,HBL_LOAD_DRAIN_PUMP);
			}
		}
	}
	else
	{
		PUMP_STATE_TYPE state;

		state = STATE_ON;

		if (This_Pc->Control->F1_F0_Flags == (uint8)WASH_PUMP_CONTROL_ACTIVATE)
		{
			(void)(Hbl__SetLoadRequestByLoadID(HBL_LOAD_WASH_PUMP, HBL_PRIORITY_APPLICATION_HI, (uint8 *) &state));
		}
		else if (This_Pc->Control->F1_F0_Flags == (uint8)DRAIN_PUMP_CONTROL_ACTIVATE)
		{
			(void)(Hbl__SetLoadRequestByLoadID(HBL_LOAD_DRAIN_PUMP, HBL_PRIORITY_APPLICATION_HI, (uint8 *) &state));
		}
	}

	This_Pc->PumpControl_Ready_To_Exit = TRUE;

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void StartTimerWaitStopped(void)
{
	(void)SystemTimers__NVSetTimerMS(&PC_Wait_Stopped_Timer_Handle, 10000,SYSTEMTIMERS_TYPE_DOWN_COUNTER);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void WaitStopped(void)
{
	if ((SystemTimers__NVCheckTimer(&PC_Wait_Stopped_Timer_Handle) == SYSTEMTIMERS_STATE_DONE) )
	{
		This_Pc->PC_Wait_Stopped_Attempts++;

		if (This_Pc->PC_Wait_Stopped_Attempts < (uint8)MAX_WAIT_STOP_ATTEMPS)
		{
			HBL_PILOT_ENUM_TYPE drain_pump_pilot_type = Hbl__GetPilotIDByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DRAIN_PUMP));

			// Send Stop
			PumpCommand__StopWithDefDecel(HBL_PRIORITY_APPLICATION_HI, HBL_LOAD_WASH_PUMP);
			PumpFlushRequestMonitor__SetWashStopSentFlag();

			if (drain_pump_pilot_type == HBL_PILOT_EXTERNAL_MOTOR)
			{
				PumpCommand__StopWithDefDecel(HBL_PRIORITY_APPLICATION_HI, HBL_LOAD_DRAIN_PUMP);
				PumpFlushRequestMonitor__SetDrainStopSentFlag();
			}

			EventErrorWaitStopped();
			This_Pc->PC_Wait_Status_Attempts = 0;
			LOG_ADD_EXCEPTION(ERROR_WAIT_STOPPED, This_Pc->PC_Wait_Stopped_Attempts);
		}
		else
		{
			EventError(); // Process terminate
			LOG_ADD_EXCEPTION(ERROR_WAIT_STOPPED_MAX_ATTEMPS, This_Pc->PC_Wait_Stopped_Attempts);
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void ReadProcessParameters(void)
{
	if (This_Pc->PC_Pump_Pilot_Type != HBL_PILOT_EXTERNAL_MOTOR)
	{
		SendRun();
		EventParameterOkNoExternalMotor();
	}
	else
	{
		SETTINGFILE_LOADER_TYPE loader;

		// Get the Pump Control process parameters from the setting file.
		if (SettingFile__MSettingBasicLoader(This_Pc->Control->Setting_File_Index,
											 SF_PTR_ACU_PROCESS_PUMP_CTRL,
											 This_Pc->Control->Data_Field_Index,
											 &loader) == FAIL)
		{
			LOG_ADD_EXCEPTION(ERROR_LOADING_PARAMETERS, This_Pc->Control->Data_Field_Index);
			EventParameterKo();
		}
		else
		{
			uint16 temp_motor_speed;
		    sint32 modified_value;

			// Read the setting file data into local variables for convenience
			This_Pc->PumpControl_Parameters = (PUMP_CONTROL_PROCESS_PARAMETER_TYPE*)(void*)loader.Data;

			if ((uint8)UNUSED_FEATURE_BYTE != This_Pc->PumpControl_Parameters->Speed_Modifiers_Table_Index)
			{
				modified_value = Modifier__GetValue(((sint32)This_Pc->PumpControl_Parameters->Speed*(sint32)25), This_Pc->PumpControl_Parameters->Speed_Modifiers_Table_Index);
				if (modified_value < 0)
				{
					modified_value = 0;
				}
				temp_motor_speed = (uint16)modified_value;
			}
			else
			{
				temp_motor_speed = This_Pc->PumpControl_Parameters->Speed*(uint16)25; // in rpm
			}

			This_Pc->PC_Motor_Speed_Before_Reduction = temp_motor_speed;

			// Check speed reduction from Stability Control (only for wash pump)
			if (This_Pc->Control->F1_F0_Flags == (uint8)WASH_PUMP_CONTROL_ACTIVATE)
			{
				if (StabilityControl__GetSpeedReduction() != SC_NO_SPEED_REDUCTION)
				{
					StabilityControl__ReduceSpeed(&temp_motor_speed, DishAuxVar__GetDiverterPosition().Position);
				}
			}

			This_Pc->PC_Motor_Speed = temp_motor_speed;

			if ((uint8)UNUSED_FEATURE_BYTE != This_Pc->PumpControl_Parameters->Motor_Acceleration_Modifiers_Table_Index)
			{
				modified_value = Modifier__GetValue((sint32)(This_Pc->PumpControl_Parameters->Motor_Acceleration), This_Pc->PumpControl_Parameters->Motor_Acceleration_Modifiers_Table_Index);
				if (modified_value < 0)
				{
					modified_value = 0;
				}
				This_Pc->PC_Motor_Acceleration = (uint16)modified_value;
			}
			else
			{
				This_Pc->PC_Motor_Acceleration = This_Pc->PumpControl_Parameters->Motor_Acceleration; // rpm
			}

			PC_Subcycle_Name = Selector__GetCycleNameId(Interpreter__GetCurrentSFIndex(COMPARTMENT_DEFAULT),
														COMPARTMENT_DEFAULT,
														Interpreter__GetStatus().Sub_Cycle[COMPARTMENT_DEFAULT]);

			if ((PC_Subcycle_Name == CYCLE_NAME_WASH_MOTOR_FAULT) ||
				(PC_Subcycle_Name == CYCLE_NAME_DRAIN_MOTOR_FAULT))
			{
				EventParameterOkNoExternalMotor();
			}
			else
			{
				EventParameterOk();
			}
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void StartTimerWaitMcuReset(void)
{
	(void)SystemTimers__NVSetTimerMS(&PC_Wait_MCU_Reset_Timer_Handle, 10000,SYSTEMTIMERS_TYPE_DOWN_COUNTER);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void WaitMcuResetTimer(void)
{
	if (SystemTimers__NVCheckTimer(&PC_Wait_MCU_Reset_Timer_Handle) == SYSTEMTIMERS_STATE_DONE)
	{
		This_Pc->PC_Wait_Status_Attempts = 0;
		EventRestart();
	}
	else
	{
		HBL_PILOT_ENUM_TYPE drain_pump_pilot_type = Hbl__GetPilotIDByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DRAIN_PUMP));

		// Send Stop
		PumpCommand__StopWithDefDecel(HBL_PRIORITY_APPLICATION_HI, HBL_LOAD_WASH_PUMP);

		if (drain_pump_pilot_type == HBL_PILOT_EXTERNAL_MOTOR)
		{
			PumpCommand__StopWithDefDecel(HBL_PRIORITY_APPLICATION_HI, HBL_LOAD_DRAIN_PUMP);
		}
	}
}

#if (API221_CLIENT_FEATURE == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Use ExternalEventExtEvMCUErrReceive (RECEIVE_ERROR and RECEIVE_NO_ERROR)
 * Process information received from MCU about FF
 * @param motor = The motor ID related to the FF received
 * @param failures = bitmap of FF
 */
static void API221ApplReceivedStatus (API221_CLIENT_MOTOR_INDEX_TYPE motor, uint32 status)
{
	HBL_PILOT_ENUM_TYPE drain_pump_pilot_type = Hbl__GetPilotIDByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DRAIN_PUMP));
	if (drain_pump_pilot_type == HBL_PILOT_EXTERNAL_MOTOR)
	{
		This_Pc->PC_Status[motor] = status;
		This_Pc->PC_PendingStatus[motor] = TRUE;
	}
	else
	{
		Pump_Control[COMPARTMENT_DEFAULT][1].PC_Status[motor] = status;
		Pump_Control[COMPARTMENT_DEFAULT][1].PC_PendingStatus[motor] = TRUE;

		Pump_Control[COMPARTMENT_DEFAULT][0].PC_Status[motor] = status;
		Pump_Control[COMPARTMENT_DEFAULT][0].PC_PendingStatus[motor] = TRUE;
	}
}

//---------------------------------------------------------------------------------------------------------------------
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
			 if (This_Pc->Pump_Control_State != STATE_PUMP_CONTROL_WAIT_STOPPED)
			 {
				 if ((PC_Subcycle_Name != CYCLE_NAME_WASH_MOTOR_FAULT) &&
				 	(PC_Subcycle_Name != CYCLE_NAME_DRAIN_MOTOR_FAULT))
				 {
					 LOG_ADD_EXCEPTION(ERROR_MCU_RESET, This_Pc->Pump_Control_State);
				 	 EventMcuReset();
				 }
			 }
			 else
			 {
				 EventRun();
			 }
			 break;
		 case API221_EVENT_REACHED_TARGET:
			 if (event.Motor == (API221_CLIENT_MOTOR_INDEX_TYPE)MOTOR_0)
			 {
				 PumpControl__SetPreviousWashMotorSpeed(This_Pc->PC_Motor_Speed);
			 }
			 else if (event.Motor == (API221_CLIENT_MOTOR_INDEX_TYPE)MOTOR_1)
			 {
				 PumpControl__SetPreviousDrainMotorSpeed(This_Pc->PC_Motor_Speed);
			 }
			 break;
		 case API221_EVENT_DEBLOCKING_READY:
		 	 if (event.Motor == (API221_CLIENT_MOTOR_INDEX_TYPE)MOTOR_0)
			 {
		 		PC_Wash_Pump_Deblocking_Ready = TRUE;
			 }
			 else if (event.Motor == (API221_CLIENT_MOTOR_INDEX_TYPE)MOTOR_1)
			 {
		 		PC_Drain_Pump_Deblocking_Ready = TRUE;
			 }
		 	 break;
		 default:
			 // Ignore all other events
			 break;
	}
}

static BOOL_TYPE Double_VSM_Pump_Cannot_Run(PROCESS_CONTROL_TYPE* process_control)
{
	HBL_PILOT_ENUM_TYPE drain_pump_pilot_type = Hbl__GetPilotIDByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DRAIN_PUMP));
	BOOL_TYPE return_value = FALSE;
	if (drain_pump_pilot_type != HBL_PILOT_EXTERNAL_MOTOR)
	{
		return_value = FALSE;
	}
	else
	{
		if ((process_control->F1_F0_Flags != (uint8)DRAIN_PUMP_CONTROL_ACTIVATE) && ((uint8)PumpControl__GetVSMDrainIsRunning() == (uint8)TRUE))
		{
			return_value = TRUE;
		}
		else
		{
			return_value = FALSE;
		}
	}

	return return_value;
}

#endif



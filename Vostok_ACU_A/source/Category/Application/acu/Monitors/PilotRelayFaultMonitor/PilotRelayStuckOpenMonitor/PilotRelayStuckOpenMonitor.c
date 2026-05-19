/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *
   @startuml{PilotRelayStuckOpenMonitor State Machine.jpg}
        title Pilot Relay Stuck Open Monitor State Machine

         [*] --> INITIALIZE

        INITIALIZE : entry / InitializeModule()
        INITIALIZE --> NO_FAILURE  : evStartMonitor
        INITIALIZE --> [*]         : evSettingDataMissing

        NO_FAILURE : entry / ResetOpenFailure()
        NO_FAILURE : do /\t CheckForOpenFailure()
        NO_FAILURE --> FAILURE_DETECTED  : evOpenFailureDetected

        FAILURE_DETECTED : entry /\t SetOpenFailure()
        FAILURE_DETECTED : do /\t CheckOpenFailureRemoval()
        FAILURE_DETECTED --> NO_FAILURE  : evNoOpenFailurePresent
        FAILURE_DETECTED --> FAULT       : evOpenFailureDebounceCompleted

		FAULT : exit /\t SetStabilizationTime()
		FAULT : do /\t MonitorOpenFaultState()
		FAULT --> NO_FAILURE : evOpenFailureReset

    @enduml

 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "PilotRelayStuckOpenMonitor.h"
#include "PilotRelayFaultMonitor.h"

// -- Other Modules --
#include "Fault.h"
#include "HblDefs.h"
#include "Supervisor.h"


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Pilot Relay Stuck Open Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Pilot Relay Stuck Open Monitor state machine.
typedef enum PILOT_RELAY_STUCK_OPEN_MONITOR_STATE_ENUM
{
    STATE_PILOT_RELAY_STUCK_OPEN_MONITOR_NONE             = 0,

    STATE_PILOT_RELAY_STUCK_OPEN_MONITOR_INITIALIZE       = 1, //!< (initial_state)
    STATE_PILOT_RELAY_STUCK_OPEN_MONITOR_NO_FAILURE       = 2,
    STATE_PILOT_RELAY_STUCK_OPEN_MONITOR_FAILURE_DETECTED = 3,
    STATE_PILOT_RELAY_STUCK_OPEN_MONITOR_FAULT            = 4,

    STATE_PILOT_RELAY_STUCK_OPEN_MONITOR_END              = 5
} PILOT_RELAY_STUCK_OPEN_MONITOR_STATE_TYPE;

//! The state variable for the Pilot Relay Stuck Open Monitor state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static PILOT_RELAY_STUCK_OPEN_MONITOR_STATE_TYPE Pilot_Relay_Stuck_Open_Monitor_State;

// -- Private Function Prototypes used by the state machine for customization functions --
static void CheckForOpenFailure(void);
static void CheckOpenFailureRemoval(void);
static void InitializeModule(void);
static void MonitorOpenFaultState(void);
static void ResetOpenFailure(void);
static void SetOpenFailure(void);
static void SetStabilizationTime(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventNoOpenFailurePresent(void);
static void EventOpenFailureDebounceCompleted(void);
static void EventOpenFailureDetected(void);
static void EventOpenFailureReset(void);
static void EventSettingDataMissing(void);
static void EventStartMonitor(void);
static void PilotRelayStuckOpenMonitorExecute(void);
static void PilotRelayStuckOpenMonitorInitialize(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.



//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------


//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//----------------------------------------------------------------------------
/**
 *  @brief
 *  @details
 *
 */
void PilotRelayStuckOpenMonitor__Initialize(void)
{
	PilotRelayStuckOpenMonitorInitialize();
}


//----------------------------------------------------------------------------
/**
 *  @brief
 *  @details
 *
 */
void PilotRelayStuckOpenMonitor__Execution(void)
{
	PilotRelayStuckOpenMonitorExecute();
}


//----------------------------------------------------------------------------
/**
 *  @brief
 *  @details
 *
 */
void PilotRelayStuckOpenMonitor__CycleStarted(void)
{
	EventOpenFailureReset();
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
static void CheckForOpenFailure(void)
{
	if ((Supervisor__IsTestCycleActive() == FALSE) &&
		(PilotRelayFaultMonitor__GetStabilizationCounter() == (uint8)0) &&
		(PilotRelayFaultMonitor__GetConsistency() == COMBINE_NIBBLES(HBL_CONSISTENCY_TABLE_ON, HBL_CONSISTENCY_OPEN_CIRCUIT)))
	{
		EventOpenFailureDetected();
	}

}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void CheckOpenFailureRemoval(void)
{
	if ((uint8)Fault__IsInFaultState(FAULT_ID_PILOT_RELAY_STUCK_OPEN) == (uint8)TRUE)
	{
		EventOpenFailureDebounceCompleted();
	}
	else
	{
		if ((Fault__IsObserverActive(FAULT_ID_PILOT_RELAY_STUCK_OPEN) == FALSE) ||
			((uint8)Supervisor__IsTestCycleActive() == (uint8)TRUE) ||
			(PilotRelayFaultMonitor__GetConsistency() != COMBINE_NIBBLES(HBL_CONSISTENCY_TABLE_ON, HBL_CONSISTENCY_OPEN_CIRCUIT)))
		{
			EventNoOpenFailurePresent();
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetOpenFailure(void)
{
	Fault__ConditionDetected(FAULT_ID_PILOT_RELAY_STUCK_OPEN);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void ResetOpenFailure(void)
{
	Fault__ConditionRemoved(FAULT_ID_PILOT_RELAY_STUCK_OPEN);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void MonitorOpenFaultState(void)
{
	if (Fault__IsInFaultState(FAULT_ID_PILOT_RELAY_STUCK_OPEN) == FALSE)
	{
		EventOpenFailureReset();
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetStabilizationTime(void)
{
	PilotRelayFaultMonitor__RestartStabilizationCounter();
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void InitializeModule(void)
{
	if (Fault__GetParameters(FAULT_ID_PILOT_RELAY_STUCK_OPEN) != NULL)
	{
		EventStartMonitor();
	}
	else
	{
		EventSettingDataMissing();
	}
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Pilot Relay Stuck Open Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the No Open Failure Present Event for the Pilot Relay Stuck Open Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNoOpenFailurePresent(void)
{
    switch (Pilot_Relay_Stuck_Open_Monitor_State)
    {
        case STATE_PILOT_RELAY_STUCK_OPEN_MONITOR_FAILURE_DETECTED:
            Pilot_Relay_Stuck_Open_Monitor_State = STATE_PILOT_RELAY_STUCK_OPEN_MONITOR_NO_FAILURE;
            ResetOpenFailure();                     // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Pilot_Relay_Stuck_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Open Failure Debounce Completed Event for the Pilot Relay Stuck Open Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventOpenFailureDebounceCompleted(void)
{
    switch (Pilot_Relay_Stuck_Open_Monitor_State)
    {
        case STATE_PILOT_RELAY_STUCK_OPEN_MONITOR_FAILURE_DETECTED:
            Pilot_Relay_Stuck_Open_Monitor_State = STATE_PILOT_RELAY_STUCK_OPEN_MONITOR_FAULT;
            break;

        default:
            // Event is ignored for all other values of Pilot_Relay_Stuck_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Open Failure Detected Event for the Pilot Relay Stuck Open Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventOpenFailureDetected(void)
{
    switch (Pilot_Relay_Stuck_Open_Monitor_State)
    {
        case STATE_PILOT_RELAY_STUCK_OPEN_MONITOR_NO_FAILURE:
            Pilot_Relay_Stuck_Open_Monitor_State = STATE_PILOT_RELAY_STUCK_OPEN_MONITOR_FAILURE_DETECTED;
            SetOpenFailure();                       // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Pilot_Relay_Stuck_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Open Failure Reset Event for the Pilot Relay Stuck Open Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventOpenFailureReset(void)
{
    switch (Pilot_Relay_Stuck_Open_Monitor_State)
    {
        case STATE_PILOT_RELAY_STUCK_OPEN_MONITOR_FAULT:
            SetStabilizationTime();                 // Exit function.
            Pilot_Relay_Stuck_Open_Monitor_State = STATE_PILOT_RELAY_STUCK_OPEN_MONITOR_NO_FAILURE;
            ResetOpenFailure();                     // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Pilot_Relay_Stuck_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Setting Data Missing Event for the Pilot Relay Stuck Open Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventSettingDataMissing(void)
{
    switch (Pilot_Relay_Stuck_Open_Monitor_State)
    {
        case STATE_PILOT_RELAY_STUCK_OPEN_MONITOR_INITIALIZE:
            Pilot_Relay_Stuck_Open_Monitor_State = STATE_PILOT_RELAY_STUCK_OPEN_MONITOR_NONE; // Terminate Pilot Relay Stuck Open Monitor state machine execution.
            break;

        default:
            // Event is ignored for all other values of Pilot_Relay_Stuck_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Start Monitor Event for the Pilot Relay Stuck Open Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventStartMonitor(void)
{
    switch (Pilot_Relay_Stuck_Open_Monitor_State)
    {
        case STATE_PILOT_RELAY_STUCK_OPEN_MONITOR_INITIALIZE:
            Pilot_Relay_Stuck_Open_Monitor_State = STATE_PILOT_RELAY_STUCK_OPEN_MONITOR_NO_FAILURE;
            ResetOpenFailure();                     // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Pilot_Relay_Stuck_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Pilot Relay Stuck Open Monitor state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void PilotRelayStuckOpenMonitorExecute(void)
{
    switch(Pilot_Relay_Stuck_Open_Monitor_State)
    {
        case STATE_PILOT_RELAY_STUCK_OPEN_MONITOR_NO_FAILURE:
            CheckForOpenFailure();                  // Do function.
            break;

        case STATE_PILOT_RELAY_STUCK_OPEN_MONITOR_FAILURE_DETECTED:
            CheckOpenFailureRemoval();              // Do function.
            break;

        case STATE_PILOT_RELAY_STUCK_OPEN_MONITOR_FAULT:
            MonitorOpenFaultState();                // Do function.
            break;

        default:
            // There is nothing to do for all other values of Pilot_Relay_Stuck_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Pilot Relay Stuck Open Monitor state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void PilotRelayStuckOpenMonitorInitialize(void)
{
    Pilot_Relay_Stuck_Open_Monitor_State = STATE_PILOT_RELAY_STUCK_OPEN_MONITOR_INITIALIZE;
    InitializeModule();                             // Entry Function.
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

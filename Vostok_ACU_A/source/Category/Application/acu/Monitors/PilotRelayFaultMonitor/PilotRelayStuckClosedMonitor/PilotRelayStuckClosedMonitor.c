/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *
   @startuml{PilotRelayStuckClosedMonitor State Machine.jpg}
        title Pilot Relay Stuck Closed Monitor State Machine

         [*] --> INITIALIZE

        INITIALIZE : entry / InitializeModule()
        INITIALIZE --> NO_FAILURE  : evStartMonitor
        INITIALIZE --> [*]         : evSettingDataMissing

        NO_FAILURE : entry / ResetClosedFailure()
        NO_FAILURE : do /\t CheckForClosedFailure()
        NO_FAILURE --> FAILURE_DETECTED  : evClosedFailureDetected

        FAILURE_DETECTED : entry /\t SetClosedFailure()
        FAILURE_DETECTED : do /\t CheckClosedFailureRemoval()
        FAILURE_DETECTED --> NO_FAILURE  : evNoClosedFailurePresent
        FAILURE_DETECTED --> FAULT       : evClosedFailureDebounceCompleted

		FAULT : exit /\t SetStabilizationTime()
		FAULT : do /\t MonitorClosedFaultState()
		FAULT --> NO_FAILURE : evClosedFailureReset

    @enduml

 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "PilotRelayStuckClosedMonitor.h"
#include "PilotRelayFaultMonitor.h"

// -- Other Modules --
#include "Fault.h"
#include "HblDefs.h"
#include "Supervisor.h"


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Pilot Relay Stuck Closed Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Pilot Relay Stuck Closed Monitor state machine.
typedef enum PILOT_RELAY_STUCK_CLOSED_MONITOR_STATE_ENUM
{
    STATE_PILOT_RELAY_STUCK_CLOSED_MONITOR_NONE             = 0,

    STATE_PILOT_RELAY_STUCK_CLOSED_MONITOR_INITIALIZE       = 1, //!< (initial_state)
    STATE_PILOT_RELAY_STUCK_CLOSED_MONITOR_NO_FAILURE       = 2,
    STATE_PILOT_RELAY_STUCK_CLOSED_MONITOR_FAILURE_DETECTED = 3,
    STATE_PILOT_RELAY_STUCK_CLOSED_MONITOR_FAULT            = 4,

    STATE_PILOT_RELAY_STUCK_CLOSED_MONITOR_END              = 5
} PILOT_RELAY_STUCK_CLOSED_MONITOR_STATE_TYPE;

//! The state variable for the Pilot Relay Stuck Closed Monitor state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static PILOT_RELAY_STUCK_CLOSED_MONITOR_STATE_TYPE Pilot_Relay_Stuck_Closed_Monitor_State;

// -- Private Function Prototypes used by the state machine for customization functions --
static void CheckClosedFailureRemoval(void);
static void CheckForClosedFailure(void);
static void InitializeModule(void);
static void MonitorClosedFaultState(void);
static void ResetClosedFailure(void);
static void SetClosedFailure(void);
static void SetStabilizationTime(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventClosedFailureDebounceCompleted(void);
static void EventClosedFailureDetected(void);
static void EventClosedFailureReset(void);
static void EventNoClosedFailurePresent(void);
static void EventSettingDataMissing(void);
static void EventStartMonitor(void);
static void PilotRelayStuckClosedMonitorExecute(void);
static void PilotRelayStuckClosedMonitorInitialize(void);

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
void PilotRelayStuckClosedMonitor__Initialize(void)
{
	PilotRelayStuckClosedMonitorInitialize();
}


//----------------------------------------------------------------------------
/**
 *  @brief
 *  @details
 *
 */
void PilotRelayStuckClosedMonitor__Execution(void)
{
	PilotRelayStuckClosedMonitorExecute();
}


//----------------------------------------------------------------------------
/**
 *  @brief
 *  @details
 *
 */
void PilotRelayStuckClosedMonitor__CycleStarted(void)
{
	EventClosedFailureReset();
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
static void CheckForClosedFailure(void)
{
	if ((Supervisor__IsTestCycleActive() == FALSE) &&
		(PilotRelayFaultMonitor__GetStabilizationCounter() == (uint8)0) &&
		(PilotRelayFaultMonitor__GetConsistency() == COMBINE_NIBBLES(HBL_CONSISTENCY_TABLE_OFF, HBL_CONSISTENCY_SHORT_CIRCUIT)))
	{
		EventClosedFailureDetected();
	}

}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void CheckClosedFailureRemoval(void)
{
	if ((uint8)Fault__IsInFaultState(FAULT_ID_PILOT_RELAY_STUCK_CLOSED) == (uint8)TRUE)
	{
		EventClosedFailureDebounceCompleted();
	}
	else
	{
		if ((Fault__IsObserverActive(FAULT_ID_PILOT_RELAY_STUCK_CLOSED) == FALSE) ||
			((uint8)Supervisor__IsTestCycleActive() == (uint8)TRUE) ||
			(PilotRelayFaultMonitor__GetConsistency() != COMBINE_NIBBLES(HBL_CONSISTENCY_TABLE_OFF, HBL_CONSISTENCY_SHORT_CIRCUIT)))
		{
			EventNoClosedFailurePresent();
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetClosedFailure(void)
{
	Fault__ConditionDetected(FAULT_ID_PILOT_RELAY_STUCK_CLOSED);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void ResetClosedFailure(void)
{
	Fault__ConditionRemoved(FAULT_ID_PILOT_RELAY_STUCK_CLOSED);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void MonitorClosedFaultState(void)
{
	if (Fault__IsInFaultState(FAULT_ID_PILOT_RELAY_STUCK_CLOSED) == FALSE)
	{
		EventClosedFailureReset();
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
	if (Fault__GetParameters(FAULT_ID_PILOT_RELAY_STUCK_CLOSED) != NULL)
	{
		EventStartMonitor();
	}
	else
	{
		EventSettingDataMissing();
	}
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Pilot Relay Stuck Closed Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Closed Failure Debounce Completed Event for the Pilot Relay Stuck Closed Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventClosedFailureDebounceCompleted(void)
{
    switch (Pilot_Relay_Stuck_Closed_Monitor_State)
    {
        case STATE_PILOT_RELAY_STUCK_CLOSED_MONITOR_FAILURE_DETECTED:
            Pilot_Relay_Stuck_Closed_Monitor_State = STATE_PILOT_RELAY_STUCK_CLOSED_MONITOR_FAULT;
            break;

        default:
            // Event is ignored for all other values of Pilot_Relay_Stuck_Closed_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Closed Failure Detected Event for the Pilot Relay Stuck Closed Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventClosedFailureDetected(void)
{
    switch (Pilot_Relay_Stuck_Closed_Monitor_State)
    {
        case STATE_PILOT_RELAY_STUCK_CLOSED_MONITOR_NO_FAILURE:
            Pilot_Relay_Stuck_Closed_Monitor_State = STATE_PILOT_RELAY_STUCK_CLOSED_MONITOR_FAILURE_DETECTED;
            SetClosedFailure();                     // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Pilot_Relay_Stuck_Closed_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Closed Failure Reset Event for the Pilot Relay Stuck Closed Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventClosedFailureReset(void)
{
    switch (Pilot_Relay_Stuck_Closed_Monitor_State)
    {
        case STATE_PILOT_RELAY_STUCK_CLOSED_MONITOR_FAULT:
            SetStabilizationTime();                 // Exit function.
            Pilot_Relay_Stuck_Closed_Monitor_State = STATE_PILOT_RELAY_STUCK_CLOSED_MONITOR_NO_FAILURE;
            ResetClosedFailure();                   // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Pilot_Relay_Stuck_Closed_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the No Closed Failure Present Event for the Pilot Relay Stuck Closed Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNoClosedFailurePresent(void)
{
    switch (Pilot_Relay_Stuck_Closed_Monitor_State)
    {
        case STATE_PILOT_RELAY_STUCK_CLOSED_MONITOR_FAILURE_DETECTED:
            Pilot_Relay_Stuck_Closed_Monitor_State = STATE_PILOT_RELAY_STUCK_CLOSED_MONITOR_NO_FAILURE;
            ResetClosedFailure();                   // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Pilot_Relay_Stuck_Closed_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Setting Data Missing Event for the Pilot Relay Stuck Closed Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventSettingDataMissing(void)
{
    switch (Pilot_Relay_Stuck_Closed_Monitor_State)
    {
        case STATE_PILOT_RELAY_STUCK_CLOSED_MONITOR_INITIALIZE:
            Pilot_Relay_Stuck_Closed_Monitor_State = STATE_PILOT_RELAY_STUCK_CLOSED_MONITOR_NONE; // Terminate Pilot Relay Stuck Closed Monitor state machine execution.
            break;

        default:
            // Event is ignored for all other values of Pilot_Relay_Stuck_Closed_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Start Monitor Event for the Pilot Relay Stuck Closed Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventStartMonitor(void)
{
    switch (Pilot_Relay_Stuck_Closed_Monitor_State)
    {
        case STATE_PILOT_RELAY_STUCK_CLOSED_MONITOR_INITIALIZE:
            Pilot_Relay_Stuck_Closed_Monitor_State = STATE_PILOT_RELAY_STUCK_CLOSED_MONITOR_NO_FAILURE;
            ResetClosedFailure();                   // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Pilot_Relay_Stuck_Closed_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Pilot Relay Stuck Closed Monitor state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void PilotRelayStuckClosedMonitorExecute(void)
{
    switch(Pilot_Relay_Stuck_Closed_Monitor_State)
    {
        case STATE_PILOT_RELAY_STUCK_CLOSED_MONITOR_NO_FAILURE:
            CheckForClosedFailure();                // Do function.
            break;

        case STATE_PILOT_RELAY_STUCK_CLOSED_MONITOR_FAILURE_DETECTED:
            CheckClosedFailureRemoval();            // Do function.
            break;

        case STATE_PILOT_RELAY_STUCK_CLOSED_MONITOR_FAULT:
            MonitorClosedFaultState();              // Do function.
            break;

        default:
            // There is nothing to do for all other values of Pilot_Relay_Stuck_Closed_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Pilot Relay Stuck Closed Monitor state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void PilotRelayStuckClosedMonitorInitialize(void)
{
    Pilot_Relay_Stuck_Closed_Monitor_State = STATE_PILOT_RELAY_STUCK_CLOSED_MONITOR_INITIALIZE;
    InitializeModule();                             // Entry Function.
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

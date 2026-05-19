/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *
   @startuml{LineRelayOpenFaultMonitor_State_Machine.jpg}
        title Line Relay Open Monitor State Machine

         [*] --> INITIALIZE

        INITIALIZE : entry / InitializeModule()
        INITIALIZE --> NO_FAILURE  : evStartMonitor\n/ ResetFailuresDetection()
        INITIALIZE --> [*]         : evSettingDataMissing

        NO_FAILURE : do /\t CheckForFailures()
        NO_FAILURE --> RELAY_L_OPEN_DETECTED  : evRelayLOpenDetected

        RELAY_L_OPEN_DETECTED : entry /\t SetRelayLOpenDetection()
        RELAY_L_OPEN_DETECTED : do /\t CheckRelayLOpenRemoval()
        RELAY_L_OPEN_DETECTED --> NO_FAILURE  : evNoRelayLOpenFailurePresent\n/ ResetFailuresDetection()
        RELAY_L_OPEN_DETECTED --> OPEN_FAULT  : evRelayLOpenDebounceCompleted

		OPEN_FAULT : exit /\t RestartStabilizationTime()
		OPEN_FAULT : do /\t MonitorRelayLOpenFaultState()
		OPEN_FAULT --> NO_FAILURE : evHeaterFaultReset\n/ ResetFailuresDetection()

    @enduml

 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "WashHeaterFaultMonitor.h"
#include "HeaterLineRelayOpenMonitor.h"

// -- Other Modules --
#include "Fault.h"
#include "Hbl.h"
#include "HblDefs.h"
#include "Supervisor.h"

//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Line Relay Open Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Line Relay Open Monitor state machine.
typedef enum LINE_RELAY_OPEN_MONITOR_STATE_ENUM
{
    STATE_LINE_RELAY_OPEN_MONITOR_NONE                  = 0,

    STATE_LINE_RELAY_OPEN_MONITOR_INITIALIZE            = 1, //!< (initial_state)
    STATE_LINE_RELAY_OPEN_MONITOR_NO_FAILURE            = 2,
    STATE_LINE_RELAY_OPEN_MONITOR_RELAY_L_OPEN_DETECTED = 3,
    STATE_LINE_RELAY_OPEN_MONITOR_OPEN_FAULT            = 4,

    STATE_LINE_RELAY_OPEN_MONITOR_END                   = 5
} LINE_RELAY_OPEN_MONITOR_STATE_TYPE;

//! The state variable for the Line Relay Open Monitor state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static LINE_RELAY_OPEN_MONITOR_STATE_TYPE Line_Relay_Open_Monitor_State;

// -- Private Function Prototypes used by the state machine for customization functions --
static void CheckForFailures(void);
static void CheckRelayLOpenRemoval(void);
static void InitializeModule(void);
static void MonitorRelayLOpenFaultState(void);
static void ResetFailuresDetection(void);
static void RestartStabilizationTime(void);
static void SetRelayLOpenDetection(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventHeaterFaultReset(void);
static void EventNoRelayLOpenFailurePresent(void);
static void EventRelayLOpenDebounceCompleted(void);
static void EventRelayLOpenDetected(void);
static void EventSettingDataMissing(void);
static void EventStartMonitor(void);
static void LineRelayOpenMonitorExecute(void);
static void LineRelayOpenMonitorInitialize(void);

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
void HeaterLineRelayOpenMonitor__Initialize(void)
{
	LineRelayOpenMonitorInitialize();
}


//----------------------------------------------------------------------------
/**
 *  @brief
 *  @details
 *
 */
void HeaterLineRelayOpenMonitor__Execution(void)
{
	LineRelayOpenMonitorExecute();
}


//----------------------------------------------------------------------------
/**
 *  @brief
 *  @details
 *
 */
void HeaterLineRelayOpenMonitor__ExitFault(void)
{
	EventHeaterFaultReset();
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
static void CheckForFailures(void)
{
	if ((uint8)WashHeaterFaultMonitor__GetMonitorPrecondition() == (uint8)TRUE)
	{
		if (WashHeaterFaultMonitor__GetStabilizationCounter() == (uint8)0)
		{
			if (HEATER_L_RELAY_OPEN_FAULT_CONDITION)
			{
				EventRelayLOpenDetected();
			}
		}
		else
		{
			// Wait for signals stabilization
		}
	}
	else
	{
		// No failures detection if pre-conditions are not satisfied
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void CheckRelayLOpenRemoval(void)
{
	if ((uint8)Fault__IsInFaultState(HEATER_LINE_RELAY_FAULT_ID) == (uint8)TRUE)
	{
		if (Supervisor__IsTestCycleActive() == FALSE)
		{
			WashHeaterFaultMonitor__SetHeaterDisabledBitmapBit(HEATER_DISABLED_BY_L_RELAY_STUCK_OPEN);
		}
		EventRelayLOpenDebounceCompleted();
	}
	else
	{
		if ((WashHeaterFaultMonitor__GetMonitorPrecondition() == FALSE) ||
			(Fault__IsObserverActive(HEATER_LINE_RELAY_FAULT_ID) == FALSE) ||
			(!HEATER_L_RELAY_OPEN_FAULT_CONDITION))
		{
			EventNoRelayLOpenFailurePresent();
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetRelayLOpenDetection(void)
{
	Fault__ConditionDetected(HEATER_LINE_RELAY_FAULT_ID);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void ResetFailuresDetection(void)
{
	Fault__ConditionRemoved(HEATER_LINE_RELAY_FAULT_ID);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void MonitorRelayLOpenFaultState(void)
{
	if (Fault__IsInFaultState(HEATER_LINE_RELAY_FAULT_ID) == FALSE)
	{
		EventHeaterFaultReset();
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void RestartStabilizationTime(void)
{
	WashHeaterFaultMonitor__RestartStabilizationCounter();
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void InitializeModule(void)
{
	if (Fault__GetParameters(HEATER_LINE_RELAY_FAULT_ID) != NULL)
	{
		EventStartMonitor();
	}
	else
	{
		EventSettingDataMissing();
	}
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Line Relay Open Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Heater Fault Reset Event for the Line Relay Open Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventHeaterFaultReset(void)
{
    switch (Line_Relay_Open_Monitor_State)
    {
        case STATE_LINE_RELAY_OPEN_MONITOR_OPEN_FAULT:
            RestartStabilizationTime();             // Exit function.
            ResetFailuresDetection();               // Transition action function.
            Line_Relay_Open_Monitor_State = STATE_LINE_RELAY_OPEN_MONITOR_NO_FAILURE;
            break;

        default:
            // Event is ignored for all other values of Line_Relay_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the No Relay L Open Failure Present Event for the Line Relay Open Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNoRelayLOpenFailurePresent(void)
{
    switch (Line_Relay_Open_Monitor_State)
    {
        case STATE_LINE_RELAY_OPEN_MONITOR_RELAY_L_OPEN_DETECTED:
            ResetFailuresDetection();               // Transition action function.
            Line_Relay_Open_Monitor_State = STATE_LINE_RELAY_OPEN_MONITOR_NO_FAILURE;
            break;

        default:
            // Event is ignored for all other values of Line_Relay_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Relay L Open Debounce Completed Event for the Line Relay Open Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventRelayLOpenDebounceCompleted(void)
{
    switch (Line_Relay_Open_Monitor_State)
    {
        case STATE_LINE_RELAY_OPEN_MONITOR_RELAY_L_OPEN_DETECTED:
            Line_Relay_Open_Monitor_State = STATE_LINE_RELAY_OPEN_MONITOR_OPEN_FAULT;
            break;

        default:
            // Event is ignored for all other values of Line_Relay_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Relay L Open Detected Event for the Line Relay Open Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventRelayLOpenDetected(void)
{
    switch (Line_Relay_Open_Monitor_State)
    {
        case STATE_LINE_RELAY_OPEN_MONITOR_NO_FAILURE:
            Line_Relay_Open_Monitor_State = STATE_LINE_RELAY_OPEN_MONITOR_RELAY_L_OPEN_DETECTED;
            SetRelayLOpenDetection();               // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Line_Relay_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Setting Data Missing Event for the Line Relay Open Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventSettingDataMissing(void)
{
    switch (Line_Relay_Open_Monitor_State)
    {
        case STATE_LINE_RELAY_OPEN_MONITOR_INITIALIZE:
            Line_Relay_Open_Monitor_State = STATE_LINE_RELAY_OPEN_MONITOR_NONE; // Terminate Line Relay Open Monitor state machine execution.
            break;

        default:
            // Event is ignored for all other values of Line_Relay_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Start Monitor Event for the Line Relay Open Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventStartMonitor(void)
{
    switch (Line_Relay_Open_Monitor_State)
    {
        case STATE_LINE_RELAY_OPEN_MONITOR_INITIALIZE:
            ResetFailuresDetection();               // Transition action function.
            Line_Relay_Open_Monitor_State = STATE_LINE_RELAY_OPEN_MONITOR_NO_FAILURE;
            break;

        default:
            // Event is ignored for all other values of Line_Relay_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Line Relay Open Monitor state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void LineRelayOpenMonitorExecute(void)
{
    switch(Line_Relay_Open_Monitor_State)
    {
        case STATE_LINE_RELAY_OPEN_MONITOR_NO_FAILURE:
            CheckForFailures();                     // Do function.
            break;

        case STATE_LINE_RELAY_OPEN_MONITOR_RELAY_L_OPEN_DETECTED:
            CheckRelayLOpenRemoval();               // Do function.
            break;

        case STATE_LINE_RELAY_OPEN_MONITOR_OPEN_FAULT:
            MonitorRelayLOpenFaultState();          // Do function.
            break;

        default:
            // There is nothing to do for all other values of Line_Relay_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Line Relay Open Monitor state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void LineRelayOpenMonitorInitialize(void)
{
    Line_Relay_Open_Monitor_State = STATE_LINE_RELAY_OPEN_MONITOR_INITIALIZE;
    InitializeModule();                             // Entry Function.
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

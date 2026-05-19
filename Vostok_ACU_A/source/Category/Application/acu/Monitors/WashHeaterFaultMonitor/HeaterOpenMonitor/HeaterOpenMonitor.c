/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *
   @startuml{HeaterOpenMonitor_State_Machine.jpg}
        title Heater Open Monitor State Machine

         [*] --> INITIALIZE

        INITIALIZE : entry / InitializeModule()
        INITIALIZE --> NO_FAILURE  : evStartMonitor
        INITIALIZE --> [*]         : evSettingDataMissing

        NO_FAILURE : entry / ResetFailuresDetection()
        NO_FAILURE : do /\t CheckForFailures()
        NO_FAILURE --> HEATER_OPEN_DETECTED  : evHeaterOpenDetected

        HEATER_OPEN_DETECTED : entry /\t SetHeaterOpenDetection()
        HEATER_OPEN_DETECTED : do /\t CheckHeaterOpenRemoval()
        HEATER_OPEN_DETECTED --> NO_FAILURE  : evNoHeaterOpenFailurePresent
        HEATER_OPEN_DETECTED --> HEATER_OPEN_FAULT  : evHeaterOpenDebounceCompleted

		HEATER_OPEN_FAULT : exit /\t RestartStabilizationTime()
		HEATER_OPEN_FAULT : do /\t MonitorHeaterOpenFaultState()
		HEATER_OPEN_FAULT --> NO_FAILURE : evHeaterOpenFaultReset

    @enduml

 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "HeaterOpenMonitor.h"

// -- Other Modules --
#include "Fault.h"
#include "HblDefs.h"
#include "HeaterLineRelayOpenMonitor.h"
#include "HeaterNeutralRelayOpenMonitor.h"
#include "Supervisor.h"

#if (HEATER_OPEN_UNIQUELY_DETECTED == ENABLED)

//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Heater Open Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Heater Open Monitor state machine.
typedef enum HEATER_OPEN_MONITOR_STATE_ENUM
{
    STATE_HEATER_OPEN_MONITOR_NONE                 = 0,

    STATE_HEATER_OPEN_MONITOR_INITIALIZE           = 1, //!< (initial_state)
    STATE_HEATER_OPEN_MONITOR_NO_FAILURE           = 2,
    STATE_HEATER_OPEN_MONITOR_HEATER_OPEN_DETECTED = 3,
    STATE_HEATER_OPEN_MONITOR_HEATER_OPEN_FAULT    = 4,

    STATE_HEATER_OPEN_MONITOR_END                  = 5
} HEATER_OPEN_MONITOR_STATE_TYPE;

//! The state variable for the Heater Open Monitor state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static HEATER_OPEN_MONITOR_STATE_TYPE Heater_Open_Monitor_State;

// -- Private Function Prototypes used by the state machine for customization functions --
static void CheckForFailures(void);
static void CheckHeaterOpenRemoval(void);
static void InitializeModule(void);
static void MonitorHeaterOpenFaultState(void);
static void ResetFailuresDetection(void);
static void RestartStabilizationTime(void);
static void SetHeaterOpenDetection(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventHeaterOpenDebounceCompleted(void);
static void EventHeaterOpenDetected(void);
static void EventHeaterOpenFaultReset(void);
static void EventNoHeaterOpenFailurePresent(void);
static void EventSettingDataMissing(void);
static void EventStartMonitor(void);
static void HeaterOpenMonitorExecute(void);
static void HeaterOpenMonitorInitialize(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#if (HEATER_OPEN_PLUS_N_RELAY_OPEN_DETECTION != ENABLED)
	#define HeaterNeutralRelayOpenMonitor__GetHeaterOpenDetected()	FALSE
#endif // (HEATER_OPEN_PLUS_N_RELAY_OPEN_DETECTION == ENABLED)

//! Fault condition.
#define HEATER_OPEN_FAULT_CONDITION		(HEATER_OPEN_FAULT_CONDITION_HEATER_OFF && (HeaterNeutralRelayOpenMonitor__GetHeaterOpenDetected() == FALSE))


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
void HeaterOpenMonitor__Initialize(void)
{
	HeaterOpenMonitorInitialize();
}


//----------------------------------------------------------------------------
/**
 *  @brief
 *  @details
 *
 */
void HeaterOpenMonitor__Execution(void)
{
	HeaterOpenMonitorExecute();
}


//----------------------------------------------------------------------------
/**
 *  @brief
 *  @details
 *
 */
void HeaterOpenMonitor__ExitFault(void)
{
	EventHeaterOpenFaultReset();
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
			if (HEATER_OPEN_FAULT_CONDITION)
			{
				EventHeaterOpenDetected();
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
static void CheckHeaterOpenRemoval(void)
{
	if ((uint8)Fault__IsInFaultState(FAULT_ID_HEATER_OPEN) == (uint8)TRUE)
	{
		if (Supervisor__IsTestCycleActive() == FALSE)
		{
			WashHeaterFaultMonitor__SetHeaterDisabledBitmapBit(HEATER_DISABLED_BY_HEATER_OPEN);
		}
		EventHeaterOpenDebounceCompleted();
	}
	else
	{
		if ((WashHeaterFaultMonitor__GetMonitorPrecondition() == FALSE) ||
			(Fault__IsObserverActive(FAULT_ID_HEATER_OPEN) == FALSE) ||
			(HEATER_OPEN_FAULT_CONDITION == FALSE)
			)
		{
			EventNoHeaterOpenFailurePresent();
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetHeaterOpenDetection(void)
{
	Fault__ConditionDetected(FAULT_ID_HEATER_OPEN);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void ResetFailuresDetection(void)
{
	Fault__ConditionRemoved(FAULT_ID_HEATER_OPEN);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void MonitorHeaterOpenFaultState(void)
{
	if (Fault__IsInFaultState(FAULT_ID_HEATER_OPEN) == FALSE)
	{
		EventHeaterOpenFaultReset();
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
	if (Fault__GetParameters(FAULT_ID_HEATER_OPEN) != NULL)
	{
		EventStartMonitor();
	}
	else
	{
		EventSettingDataMissing();
	}
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Heater Open Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Heater Open Debounce Completed Event for the Heater Open Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventHeaterOpenDebounceCompleted(void)
{
    switch (Heater_Open_Monitor_State)
    {
        case STATE_HEATER_OPEN_MONITOR_HEATER_OPEN_DETECTED:
            Heater_Open_Monitor_State = STATE_HEATER_OPEN_MONITOR_HEATER_OPEN_FAULT;
            break;

        default:
            // Event is ignored for all other values of Heater_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Heater Open Detected Event for the Heater Open Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventHeaterOpenDetected(void)
{
    switch (Heater_Open_Monitor_State)
    {
        case STATE_HEATER_OPEN_MONITOR_NO_FAILURE:
            Heater_Open_Monitor_State = STATE_HEATER_OPEN_MONITOR_HEATER_OPEN_DETECTED;
            SetHeaterOpenDetection();               // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Heater_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Heater Open Fault Reset Event for the Heater Open Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventHeaterOpenFaultReset(void)
{
    switch (Heater_Open_Monitor_State)
    {
        case STATE_HEATER_OPEN_MONITOR_HEATER_OPEN_FAULT:
            RestartStabilizationTime();             // Exit function.
            Heater_Open_Monitor_State = STATE_HEATER_OPEN_MONITOR_NO_FAILURE;
            ResetFailuresDetection();               // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Heater_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the No Heater Open Failure Present Event for the Heater Open Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNoHeaterOpenFailurePresent(void)
{
    switch (Heater_Open_Monitor_State)
    {
        case STATE_HEATER_OPEN_MONITOR_HEATER_OPEN_DETECTED:
            Heater_Open_Monitor_State = STATE_HEATER_OPEN_MONITOR_NO_FAILURE;
            ResetFailuresDetection();               // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Heater_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Setting Data Missing Event for the Heater Open Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventSettingDataMissing(void)
{
    switch (Heater_Open_Monitor_State)
    {
        case STATE_HEATER_OPEN_MONITOR_INITIALIZE:
            Heater_Open_Monitor_State = STATE_HEATER_OPEN_MONITOR_NONE; // Terminate Heater Open Monitor state machine execution.
            break;

        default:
            // Event is ignored for all other values of Heater_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Start Monitor Event for the Heater Open Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventStartMonitor(void)
{
    switch (Heater_Open_Monitor_State)
    {
        case STATE_HEATER_OPEN_MONITOR_INITIALIZE:
            Heater_Open_Monitor_State = STATE_HEATER_OPEN_MONITOR_NO_FAILURE;
            ResetFailuresDetection();               // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Heater_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Heater Open Monitor state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void HeaterOpenMonitorExecute(void)
{
    switch(Heater_Open_Monitor_State)
    {
        case STATE_HEATER_OPEN_MONITOR_NO_FAILURE:
            CheckForFailures();                     // Do function.
            break;

        case STATE_HEATER_OPEN_MONITOR_HEATER_OPEN_DETECTED:
            CheckHeaterOpenRemoval();               // Do function.
            break;

        case STATE_HEATER_OPEN_MONITOR_HEATER_OPEN_FAULT:
            MonitorHeaterOpenFaultState();          // Do function.
            break;

        default:
            // There is nothing to do for all other values of Heater_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Heater Open Monitor state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void HeaterOpenMonitorInitialize(void)
{
    Heater_Open_Monitor_State = STATE_HEATER_OPEN_MONITOR_INITIALIZE;
    InitializeModule();                             // Entry Function.
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

#endif // (HEATER_OPEN_UNIQUELY_DETECTED == ENABLED)

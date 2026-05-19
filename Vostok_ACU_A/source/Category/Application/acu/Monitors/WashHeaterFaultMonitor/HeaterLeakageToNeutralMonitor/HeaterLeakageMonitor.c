/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *
   @startuml{HeaterLeakage_Monitor_State_Machine.jpg}
        title Heater Leakage Monitor State Machine

         [*] --> INITIALIZE

        INITIALIZE : entry / InitializeModule()
        INITIALIZE --> NO_FAILURE  : evStartMonitor
        INITIALIZE --> [*]         : evSettingDataMissing

        NO_FAILURE : entry / RemoveLeakageFault()
        NO_FAILURE : do /\t CheckForLeakageFailure()
        NO_FAILURE --> HEATER_LEAKAGE_DETECTED  : evLeakageDetected

        HEATER_LEAKAGE_DETECTED : entry /\t SetLeakageDetection()
        HEATER_LEAKAGE_DETECTED : do /\t CheckLeakageRemoval()
        HEATER_LEAKAGE_DETECTED --> NO_FAILURE  : evNoLeakageFailurePresent
        HEATER_LEAKAGE_DETECTED --> LEAKAGE_FAULT  : evLeakageDebounced

		LEAKAGE_FAULT : exit /\t RestartStabilizationTime()
		LEAKAGE_FAULT : do /\t MonitorLeakageFaultState()
		LEAKAGE_FAULT --> NO_FAILURE : evLeakageFaultReset

    @enduml

 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "WashHeaterFaultMonitor.h"
#include "HeaterLeakageMonitor.h"
#include "HeaterLeakageMonitor_prv.h"

// -- Other Modules --
#include "Fault.h"
#include "HblDefs.h"
#include "Supervisor.h"


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Heater Leakage Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Heater Leakage Monitor state machine.
typedef enum HEATER_LEAKAGE_MONITOR_STATE_ENUM
{
    STATE_HEATER_LEAKAGE_MONITOR_NONE                    = 0,

    STATE_HEATER_LEAKAGE_MONITOR_INITIALIZE              = 1, //!< (initial_state)
    STATE_HEATER_LEAKAGE_MONITOR_NO_FAILURE              = 2,
    STATE_HEATER_LEAKAGE_MONITOR_HEATER_LEAKAGE_DETECTED = 3,
    STATE_HEATER_LEAKAGE_MONITOR_LEAKAGE_FAULT           = 4,

    STATE_HEATER_LEAKAGE_MONITOR_END                     = 5
} HEATER_LEAKAGE_MONITOR_STATE_TYPE;

//! The state variable for the Heater Leakage Monitor state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static HEATER_LEAKAGE_MONITOR_STATE_TYPE Heater_Leakage_Monitor_State;

// -- Private Function Prototypes used by the state machine for customization functions --
static void CheckForLeakageFailure(void);
static void CheckLeakageRemoval(void);
static void InitializeModule(void);
static void MonitorLeakageFaultState(void);
static void RemoveLeakageFault(void);
static void RestartStabilizationTime(void);
static void SetLeakageDetection(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventLeakageDebounced(void);
static void EventLeakageDetected(void);
static void EventLeakageFaultReset(void);
static void EventNoLeakageFailurePresent(void);
static void EventSettingDataMissing(void);
static void EventStartMonitor(void);
static void HeaterLeakageMonitorExecute(void);
static void HeaterLeakageMonitorInitialize(void);

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
void HeaterLeakageMonitor__Initialize(void)
{
	HeaterLeakageMonitorInitialize();
}


//----------------------------------------------------------------------------
/**
 *  @brief
 *  @details
 *
 */
void HeaterLeakageMonitor__Execution(void)
{
	HeaterLeakageMonitorExecute();
}


//----------------------------------------------------------------------------
/**
 *  @brief
 *  @details
 *
 */
void HeaterLeakageMonitor__ExitFault(void)
{
	EventLeakageFaultReset();
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
static void CheckForLeakageFailure(void)
{
	if ((uint8)WashHeaterFaultMonitor__GetMonitorPrecondition() == (uint8)TRUE)
	{
		if (WashHeaterFaultMonitor__GetStabilizationCounter() == (uint8)0)
		{
			if (HEATER_LEAKAGE_FAULT_CONDITION)
			{
				EventLeakageDetected();
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
static void CheckLeakageRemoval(void)
{
	if ((uint8)Fault__IsInFaultState(FAULT_ID_HEATER_FAILED) == (uint8)TRUE)
	{
		if (Supervisor__IsTestCycleActive() == FALSE)
		{
			WashHeaterFaultMonitor__SetHeaterDisabledBitmapBit(HEATER_DISABLED_BY_HEATER_FAILED);
		}
		EventLeakageDebounced();
	}
	else
	{
		if ((WashHeaterFaultMonitor__GetMonitorPrecondition() == FALSE) ||
			(Fault__IsObserverActive(FAULT_ID_HEATER_FAILED) == FALSE) ||
			(!HEATER_LEAKAGE_FAULT_CONDITION))
		{
			EventNoLeakageFailurePresent();
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetLeakageDetection(void)
{
	Fault__ConditionDetected(FAULT_ID_HEATER_FAILED);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void RemoveLeakageFault(void)
{
	Fault__ConditionRemoved(FAULT_ID_HEATER_FAILED);
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void MonitorLeakageFaultState(void)
{
	if (Fault__IsInFaultState(FAULT_ID_HEATER_FAILED) == FALSE)
	{
		EventLeakageFaultReset();
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
	if (Fault__GetParameters(FAULT_ID_HEATER_FAILED) != NULL)
	{
		EventStartMonitor();
	}
	else
	{
		EventSettingDataMissing();
	}
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Heater Leakage Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Leakage Debounced Event for the Heater Leakage Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventLeakageDebounced(void)
{
    switch (Heater_Leakage_Monitor_State)
    {
        case STATE_HEATER_LEAKAGE_MONITOR_HEATER_LEAKAGE_DETECTED:
            Heater_Leakage_Monitor_State = STATE_HEATER_LEAKAGE_MONITOR_LEAKAGE_FAULT;
            break;

        default:
            // Event is ignored for all other values of Heater_Leakage_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Leakage Detected Event for the Heater Leakage Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventLeakageDetected(void)
{
    switch (Heater_Leakage_Monitor_State)
    {
        case STATE_HEATER_LEAKAGE_MONITOR_NO_FAILURE:
            Heater_Leakage_Monitor_State = STATE_HEATER_LEAKAGE_MONITOR_HEATER_LEAKAGE_DETECTED;
            SetLeakageDetection();                  // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Heater_Leakage_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Leakage Fault Reset Event for the Heater Leakage Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventLeakageFaultReset(void)
{
    switch (Heater_Leakage_Monitor_State)
    {
        case STATE_HEATER_LEAKAGE_MONITOR_LEAKAGE_FAULT:
            RestartStabilizationTime();             // Exit function.
            Heater_Leakage_Monitor_State = STATE_HEATER_LEAKAGE_MONITOR_NO_FAILURE;
            RemoveLeakageFault();                   // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Heater_Leakage_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the No Leakage Failure Present Event for the Heater Leakage Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNoLeakageFailurePresent(void)
{
    switch (Heater_Leakage_Monitor_State)
    {
        case STATE_HEATER_LEAKAGE_MONITOR_HEATER_LEAKAGE_DETECTED:
            Heater_Leakage_Monitor_State = STATE_HEATER_LEAKAGE_MONITOR_NO_FAILURE;
            RemoveLeakageFault();                   // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Heater_Leakage_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Setting Data Missing Event for the Heater Leakage Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventSettingDataMissing(void)
{
    switch (Heater_Leakage_Monitor_State)
    {
        case STATE_HEATER_LEAKAGE_MONITOR_INITIALIZE:
            Heater_Leakage_Monitor_State = STATE_HEATER_LEAKAGE_MONITOR_NONE; // Terminate Heater Leakage Monitor state machine execution.
            break;

        default:
            // Event is ignored for all other values of Heater_Leakage_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Start Monitor Event for the Heater Leakage Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventStartMonitor(void)
{
    switch (Heater_Leakage_Monitor_State)
    {
        case STATE_HEATER_LEAKAGE_MONITOR_INITIALIZE:
            Heater_Leakage_Monitor_State = STATE_HEATER_LEAKAGE_MONITOR_NO_FAILURE;
            RemoveLeakageFault();                   // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Heater_Leakage_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Heater Leakage Monitor state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void HeaterLeakageMonitorExecute(void)
{
    switch(Heater_Leakage_Monitor_State)
    {
        case STATE_HEATER_LEAKAGE_MONITOR_NO_FAILURE:
            CheckForLeakageFailure();               // Do function.
            break;

        case STATE_HEATER_LEAKAGE_MONITOR_HEATER_LEAKAGE_DETECTED:
            CheckLeakageRemoval();                  // Do function.
            break;

        case STATE_HEATER_LEAKAGE_MONITOR_LEAKAGE_FAULT:
            MonitorLeakageFaultState();             // Do function.
            break;

        default:
            // There is nothing to do for all other values of Heater_Leakage_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Heater Leakage Monitor state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void HeaterLeakageMonitorInitialize(void)
{
    Heater_Leakage_Monitor_State = STATE_HEATER_LEAKAGE_MONITOR_INITIALIZE;
    InitializeModule();                             // Entry Function.
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

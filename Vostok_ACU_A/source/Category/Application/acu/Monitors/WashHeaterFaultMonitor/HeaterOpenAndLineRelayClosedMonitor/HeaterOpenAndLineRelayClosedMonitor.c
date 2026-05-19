/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *
   @startuml{HeaterOpenAndLineRelayClosedFaultMonitor_State_Machine.jpg}
        title Heater Open And Line Relay Closed Monitor State Machine

         [*] --> INITIALIZE

        INITIALIZE : entry / InitializeModule()
        INITIALIZE --> NO_FAILURE  : evStartMonitor
        INITIALIZE --> [*]         : evSettingDataMissing

        NO_FAILURE : entry / ResetFailuresDetection()
        NO_FAILURE : do /\t CheckForFailures()
        NO_FAILURE --> FAILURE_DETECTED  : evHeaterOpenAndRelayLClosedDetected

        FAILURE_DETECTED : entry /\t SetHeaterOpenAndRelayLClosedDetection()
        FAILURE_DETECTED : do /\t CheckHeaterOpenAndRelayLClosedRemoval()
        FAILURE_DETECTED --> NO_FAILURE  : evNoHeaterOpenAndRelayLClosedFailurePresent
        FAILURE_DETECTED --> FAULT  : evHeaterOpenAndRelayLClosedDebounceCompleted

		FAULT : exit /\t RestartStabilizationTime()
		FAULT : do /\t MonitorHeaterOpenAndRelayLClosedFaultState()
		FAULT --> NO_FAILURE : evHeaterOpenAndRelayLClosedFaultReset

    @enduml

 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "WashHeaterFaultMonitor.h"
#include "HeaterOpenAndLineRelayClosedMonitor.h"

// -- Other Modules --
#include "Fault.h"
#include "HblDefs.h"
#include "Statistics.h"
#include "Supervisor.h"

#if (HEATER_OPEN_PLUS_L_RELAY_CLOSED_DETECTION == ENABLED)
//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Heater Open And Line Relay Closed Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Heater Open And Line Relay Closed Monitor state machine.
typedef enum HEATER_OPEN_AND_LINE_RELAY_CLOSED_MONITOR_STATE_ENUM
{
    STATE_HEATER_OPEN_AND_LINE_RELAY_CLOSED_MONITOR_NONE             = 0,

    STATE_HEATER_OPEN_AND_LINE_RELAY_CLOSED_MONITOR_INITIALIZE       = 1, //!< (initial_state)
    STATE_HEATER_OPEN_AND_LINE_RELAY_CLOSED_MONITOR_NO_FAILURE       = 2,
    STATE_HEATER_OPEN_AND_LINE_RELAY_CLOSED_MONITOR_FAILURE_DETECTED = 3,
    STATE_HEATER_OPEN_AND_LINE_RELAY_CLOSED_MONITOR_FAULT            = 4,

    STATE_HEATER_OPEN_AND_LINE_RELAY_CLOSED_MONITOR_END              = 5
} HEATER_OPEN_AND_LINE_RELAY_CLOSED_MONITOR_STATE_TYPE;

//! The state variable for the Heater Open And Line Relay Closed Monitor state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static HEATER_OPEN_AND_LINE_RELAY_CLOSED_MONITOR_STATE_TYPE Heater_Open_And_Line_Relay_Closed_Monitor_State;

// -- Private Function Prototypes used by the state machine for customization functions --
static void CheckForFailures(void);
static void CheckHeaterOpenAndRelayLClosedRemoval(void);
static void InitializeModule(void);
static void MonitorHeaterOpenAndRelayLClosedFaultState(void);
static void ResetFailuresDetection(void);
static void RestartStabilizationTime(void);
static void SetHeaterOpenAndRelayLClosedDetection(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventHeaterOpenAndRelayLClosedDebounceCompleted(void);
static void EventHeaterOpenAndRelayLClosedDetected(void);
static void EventHeaterOpenAndRelayLClosedFaultReset(void);
static void EventNoHeaterOpenAndRelayLClosedFailurePresent(void);
static void EventSettingDataMissing(void);
static void EventStartMonitor(void);
static void HeaterOpenAndLineRelayClosedMonitorExecute(void);
static void HeaterOpenAndLineRelayClosedMonitorInitialize(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//! Fault condition: heater line relay stuck closed and heater open.
#define HEATER_OPEN_AND_L_RELAY_CLOSED_FAULT_CONDITION 	(WashHeaterFaultMonitor__GetHeaterConsistency() == COMBINE_NIBBLES(HBL_CONSISTENCY_TABLE_OFF, HBL_CONSISTENCY_CUSTOM_4))



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
void HeaterOpenAndLineRelayClosedMonitor__Initialize(void)
{
	HeaterOpenAndLineRelayClosedMonitorInitialize();
}


//----------------------------------------------------------------------------
/**
 *  @brief
 *  @details
 *
 */
void HeaterOpenAndLineRelayClosedMonitor__Execution(void)
{
	HeaterOpenAndLineRelayClosedMonitorExecute();
}


//----------------------------------------------------------------------------
/**
 *  @brief
 *  @details
 *
 */
void HeaterOpenAndLineRelayClosedMonitor__ExitFault(void)
{
	EventHeaterOpenAndRelayLClosedFaultReset();
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
			if (HEATER_OPEN_AND_L_RELAY_CLOSED_FAULT_CONDITION)
			{
				EventHeaterOpenAndRelayLClosedDetected();
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
//---------------------------------------------------------------------------------------------------------------------
static void CheckHeaterOpenAndRelayLClosedRemoval(void)
{
	if ((uint8)Fault__IsInFaultState(FAULT_ID_RELAY_HEATER_L_STUCK_CLOSED_AND_HEATER_OPEN) == (uint8)TRUE)
	{
		if (Supervisor__IsTestCycleActive() == FALSE)
		{
			WashHeaterFaultMonitor__SetHeaterDisabledBitmapBit(HEATER_DISABLED_BY_L_RELAY_STUCK_CLOSED_AND_HEATER_OPEN);
		}

		// Simultaneous presence of combined failures line relay stuck closed and heater open
		FAULT_CONFIG_TYPE* heater_open_params_sf_ptr;

		heater_open_params_sf_ptr = Fault__GetParameters(FAULT_ID_HEATER_OPEN);
		if (heater_open_params_sf_ptr != NULL)
		{
			if (Fault__IsInFaultState(FAULT_ID_HEATER_OPEN) == FALSE)
			{
				// Save heater open fault codes in history
				Statistics__SaveFaultData(heater_open_params_sf_ptr->Compartment,
										  heater_open_params_sf_ptr->Id,
										  heater_open_params_sf_ptr->Code,
										  heater_open_params_sf_ptr->Sub_Code,
										  heater_open_params_sf_ptr->Engineering_Code);
			}
			else
			{
				// Heater open failure already detected and saved, non need to save it again in statistics.
			}
		}

		EventHeaterOpenAndRelayLClosedDebounceCompleted();
	}
	else
	{
		if ((WashHeaterFaultMonitor__GetMonitorPrecondition() == FALSE) ||
			(Fault__IsObserverActive(FAULT_ID_RELAY_HEATER_L_STUCK_CLOSED_AND_HEATER_OPEN) == FALSE) ||
			(!HEATER_OPEN_AND_L_RELAY_CLOSED_FAULT_CONDITION))
		{
			EventNoHeaterOpenAndRelayLClosedFailurePresent();
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetHeaterOpenAndRelayLClosedDetection(void)
{
	Fault__ConditionDetected(FAULT_ID_RELAY_HEATER_L_STUCK_CLOSED_AND_HEATER_OPEN);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void ResetFailuresDetection(void)
{
	Fault__ConditionRemoved(FAULT_ID_RELAY_HEATER_L_STUCK_CLOSED_AND_HEATER_OPEN);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void MonitorHeaterOpenAndRelayLClosedFaultState(void)
{
	if (Fault__IsInFaultState(FAULT_ID_RELAY_HEATER_L_STUCK_CLOSED_AND_HEATER_OPEN) == FALSE)
	{
		EventHeaterOpenAndRelayLClosedFaultReset();
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
	if (Fault__GetParameters(FAULT_ID_RELAY_HEATER_L_STUCK_CLOSED_AND_HEATER_OPEN) != NULL)
	{
		EventStartMonitor();
	}
	else
	{
		EventSettingDataMissing();
	}
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Heater Open And Line Relay Closed Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Heater Open And Relay L Closed Debounce Completed Event for the Heater Open And Line Relay Closed Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventHeaterOpenAndRelayLClosedDebounceCompleted(void)
{
    switch (Heater_Open_And_Line_Relay_Closed_Monitor_State)
    {
        case STATE_HEATER_OPEN_AND_LINE_RELAY_CLOSED_MONITOR_FAILURE_DETECTED:
            Heater_Open_And_Line_Relay_Closed_Monitor_State = STATE_HEATER_OPEN_AND_LINE_RELAY_CLOSED_MONITOR_FAULT;
            break;

        default:
            // Event is ignored for all other values of Heater_Open_And_Line_Relay_Closed_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Heater Open And Relay L Closed Detected Event for the Heater Open And Line Relay Closed Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventHeaterOpenAndRelayLClosedDetected(void)
{
    switch (Heater_Open_And_Line_Relay_Closed_Monitor_State)
    {
        case STATE_HEATER_OPEN_AND_LINE_RELAY_CLOSED_MONITOR_NO_FAILURE:
            Heater_Open_And_Line_Relay_Closed_Monitor_State = STATE_HEATER_OPEN_AND_LINE_RELAY_CLOSED_MONITOR_FAILURE_DETECTED;
            SetHeaterOpenAndRelayLClosedDetection(); // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Heater_Open_And_Line_Relay_Closed_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Heater Open And Relay L Closed Fault Reset Event for the Heater Open And Line Relay Closed Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventHeaterOpenAndRelayLClosedFaultReset(void)
{
    switch (Heater_Open_And_Line_Relay_Closed_Monitor_State)
    {
        case STATE_HEATER_OPEN_AND_LINE_RELAY_CLOSED_MONITOR_FAULT:
            RestartStabilizationTime();             // Exit function.
            Heater_Open_And_Line_Relay_Closed_Monitor_State = STATE_HEATER_OPEN_AND_LINE_RELAY_CLOSED_MONITOR_NO_FAILURE;
            ResetFailuresDetection();               // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Heater_Open_And_Line_Relay_Closed_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the No Heater Open And Relay L Closed Failure Present Event for the Heater Open And Line Relay Closed Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNoHeaterOpenAndRelayLClosedFailurePresent(void)
{
    switch (Heater_Open_And_Line_Relay_Closed_Monitor_State)
    {
        case STATE_HEATER_OPEN_AND_LINE_RELAY_CLOSED_MONITOR_FAILURE_DETECTED:
            Heater_Open_And_Line_Relay_Closed_Monitor_State = STATE_HEATER_OPEN_AND_LINE_RELAY_CLOSED_MONITOR_NO_FAILURE;
            ResetFailuresDetection();               // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Heater_Open_And_Line_Relay_Closed_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Setting Data Missing Event for the Heater Open And Line Relay Closed Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventSettingDataMissing(void)
{
    switch (Heater_Open_And_Line_Relay_Closed_Monitor_State)
    {
        case STATE_HEATER_OPEN_AND_LINE_RELAY_CLOSED_MONITOR_INITIALIZE:
            Heater_Open_And_Line_Relay_Closed_Monitor_State = STATE_HEATER_OPEN_AND_LINE_RELAY_CLOSED_MONITOR_NONE; // Terminate Heater Open And Line Relay Closed Monitor state machine execution.
            break;

        default:
            // Event is ignored for all other values of Heater_Open_And_Line_Relay_Closed_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Start Monitor Event for the Heater Open And Line Relay Closed Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventStartMonitor(void)
{
    switch (Heater_Open_And_Line_Relay_Closed_Monitor_State)
    {
        case STATE_HEATER_OPEN_AND_LINE_RELAY_CLOSED_MONITOR_INITIALIZE:
            Heater_Open_And_Line_Relay_Closed_Monitor_State = STATE_HEATER_OPEN_AND_LINE_RELAY_CLOSED_MONITOR_NO_FAILURE;
            ResetFailuresDetection();               // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Heater_Open_And_Line_Relay_Closed_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Heater Open And Line Relay Closed Monitor state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void HeaterOpenAndLineRelayClosedMonitorExecute(void)
{
    switch(Heater_Open_And_Line_Relay_Closed_Monitor_State)
    {
        case STATE_HEATER_OPEN_AND_LINE_RELAY_CLOSED_MONITOR_NO_FAILURE:
            CheckForFailures();                     // Do function.
            break;

        case STATE_HEATER_OPEN_AND_LINE_RELAY_CLOSED_MONITOR_FAILURE_DETECTED:
            CheckHeaterOpenAndRelayLClosedRemoval(); // Do function.
            break;

        case STATE_HEATER_OPEN_AND_LINE_RELAY_CLOSED_MONITOR_FAULT:
            MonitorHeaterOpenAndRelayLClosedFaultState(); // Do function.
            break;

        default:
            // There is nothing to do for all other values of Heater_Open_And_Line_Relay_Closed_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Heater Open And Line Relay Closed Monitor state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void HeaterOpenAndLineRelayClosedMonitorInitialize(void)
{
    Heater_Open_And_Line_Relay_Closed_Monitor_State = STATE_HEATER_OPEN_AND_LINE_RELAY_CLOSED_MONITOR_INITIALIZE;
    InitializeModule();                             // Entry Function.
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

#endif // (HEATER_OPEN_PLUS_L_RELAY_CLOSED_DETECTION == ENABLED)

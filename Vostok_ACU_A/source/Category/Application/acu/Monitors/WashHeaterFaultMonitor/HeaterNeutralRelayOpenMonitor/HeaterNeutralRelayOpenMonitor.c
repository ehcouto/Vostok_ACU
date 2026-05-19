/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *
   @startuml{NeutralRelayOpenFaultMonitor_State_Machine.jpg}
        title Neutral Relay Open Monitor State Machine

         [*] --> INITIALIZE

        INITIALIZE : entry / InitializeModule()
        INITIALIZE --> NO_FAILURE  : evStartMonitor
        INITIALIZE --> [*]         : evSettingDataMissing

        NO_FAILURE : entry / ResetFailuresDetection()
        NO_FAILURE : do /\t CheckForFailures()
        NO_FAILURE --> RELAY_N_OPEN_DETECTED  : evRelayNOpenDetected

        RELAY_N_OPEN_DETECTED : entry /\t SetRelayNOpenDetection()
        RELAY_N_OPEN_DETECTED : do /\t CheckRelayNOpenRemoval()
        RELAY_N_OPEN_DETECTED --> NO_FAILURE  : evNoRelayNOpenFailurePresent
        RELAY_N_OPEN_DETECTED --> OPEN_FAULT  : evRelayNOpenDebounceCompleted

		OPEN_FAULT : exit /\t RestartStabilizationTime()
		OPEN_FAULT : do /\t MonitorRelayNOpenFaultState()
		OPEN_FAULT --> NO_FAILURE : evHeaterFaultReset

    @enduml

 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "WashHeaterFaultMonitor.h"
#include "HeaterNeutralRelayOpenMonitor.h"
#include "HeaterNeutralRelayOpenMonitor_prv.h"

// -- Other Modules --
#include "Fault.h"
#include "HblDefs.h"
#if (HEATER_OPEN_PLUS_N_RELAY_OPEN_DETECTION == ENABLED)
#include "Statistics.h"
#endif
#include "Supervisor.h"

//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Neutral Relay Open Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Neutral Relay Open Monitor state machine.
typedef enum NEUTRAL_RELAY_OPEN_MONITOR_STATE_ENUM
{
    STATE_NEUTRAL_RELAY_OPEN_MONITOR_NONE                  = 0,

    STATE_NEUTRAL_RELAY_OPEN_MONITOR_INITIALIZE            = 1, //!< (initial_state)
    STATE_NEUTRAL_RELAY_OPEN_MONITOR_NO_FAILURE            = 2,
    STATE_NEUTRAL_RELAY_OPEN_MONITOR_RELAY_N_OPEN_DETECTED = 3,
    STATE_NEUTRAL_RELAY_OPEN_MONITOR_OPEN_FAULT            = 4,

    STATE_NEUTRAL_RELAY_OPEN_MONITOR_END                   = 5
} NEUTRAL_RELAY_OPEN_MONITOR_STATE_TYPE;

//! The state variable for the Neutral Relay Open Monitor state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static NEUTRAL_RELAY_OPEN_MONITOR_STATE_TYPE Neutral_Relay_Open_Monitor_State;

// -- Private Function Prototypes used by the state machine for customization functions --
static void CheckForFailures(void);
static void CheckRelayNOpenRemoval(void);
static void InitializeModule(void);
static void MonitorRelayNOpenFaultState(void);
static void ResetFailuresDetection(void);
static void RestartStabilizationTime(void);
static void SetRelayNOpenDetection(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventHeaterFaultReset(void);
static void EventNoRelayNOpenFailurePresent(void);
static void EventRelayNOpenDebounceCompleted(void);
static void EventRelayNOpenDetected(void);
static void EventSettingDataMissing(void);
static void EventStartMonitor(void);
static void NeutralRelayOpenMonitorExecute(void);
static void NeutralRelayOpenMonitorInitialize(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#if (HEATER_OPEN_PLUS_N_RELAY_OPEN_DETECTION == ENABLED)
//! If TRUE the simultaneous presence of combined failures neutral relay stuck open and heater open has been detected
static BOOL_TYPE Heater_Open_Detected;
#endif

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
void HeaterNeutralRelayOpenMonitor__Initialize(void)
{
	NeutralRelayOpenMonitorInitialize();
}


//----------------------------------------------------------------------------
/**
 *  @brief
 *  @details
 *
 */
void HeaterNeutralRelayOpenMonitor__Execution(void)
{
	NeutralRelayOpenMonitorExecute();
}


//----------------------------------------------------------------------------
/**
 *  @brief
 *  @details
 *
 */
void HeaterNeutralRelayOpenMonitor__ExitFault(void)
{
#if (HEATER_OPEN_PLUS_N_RELAY_OPEN_DETECTION == ENABLED)
	Heater_Open_Detected = FALSE;
#endif

	EventHeaterFaultReset();
}


#if (HEATER_OPEN_PLUS_N_RELAY_OPEN_DETECTION == ENABLED)
//----------------------------------------------------------------------------
/**
 *  @brief
 *  @details
 *
 */
BOOL_TYPE HeaterNeutralRelayOpenMonitor__GetHeaterOpenDetected(void)
{
	return Heater_Open_Detected;
}
#endif // (HEATER_OPEN_PLUS_N_RELAY_OPEN_DETECTION == ENABLED)


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
			if (HEATER_N_RELAY_FAULT_CONDITION)
			{
				EventRelayNOpenDetected();
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
static void CheckRelayNOpenRemoval(void)
{
	if ((uint8)Fault__IsInFaultState(FAULT_ID_RELAY_HEATER_N_STUCK_OPEN) == (uint8)TRUE)
	{
		if (Supervisor__IsTestCycleActive() == FALSE)
		{
			WashHeaterFaultMonitor__SetHeaterDisabledBitmapBit(HEATER_DISABLED_BY_N_RELAY_STUCK_OPEN);
		}

#if (HEATER_OPEN_PLUS_N_RELAY_OPEN_DETECTION == ENABLED)
		if (WashHeaterFaultMonitor__GetHeaterConsistency() == COMBINE_NIBBLES(HBL_CONSISTENCY_TABLE_ON, HBL_CONSISTENCY_CUSTOM_3))
		{
			// Simultaneous presence of combined failures neutral relay stuck open and heater open
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

					Heater_Open_Detected = TRUE;
				}
				else
				{
					// Heater open failure already detected and saved, non need to save it again in statistics.
				}
			}

		}
#endif // (HEATER_OPEN_PLUS_N_RELAY_OPEN_DETECTION == ENABLED)

		EventRelayNOpenDebounceCompleted();
	}
	else
	{
		if ((WashHeaterFaultMonitor__GetMonitorPrecondition() == FALSE) ||
			(Fault__IsObserverActive(FAULT_ID_RELAY_HEATER_N_STUCK_OPEN) == FALSE) ||
			(!HEATER_N_RELAY_FAULT_CONDITION))
		{
			EventNoRelayNOpenFailurePresent();
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetRelayNOpenDetection(void)
{
	Fault__ConditionDetected(FAULT_ID_RELAY_HEATER_N_STUCK_OPEN);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void ResetFailuresDetection(void)
{
	Fault__ConditionRemoved(FAULT_ID_RELAY_HEATER_N_STUCK_OPEN);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void MonitorRelayNOpenFaultState(void)
{
	if (Fault__IsInFaultState(FAULT_ID_RELAY_HEATER_N_STUCK_OPEN) == FALSE)
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
#if (HEATER_OPEN_PLUS_N_RELAY_OPEN_DETECTION == ENABLED)
	Heater_Open_Detected = FALSE;
#endif

	if (Fault__GetParameters(FAULT_ID_RELAY_HEATER_N_STUCK_OPEN) != NULL)
	{
		EventStartMonitor();
	}
	else
	{
		EventSettingDataMissing();
	}
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Neutral Relay Open Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Heater Fault Reset Event for the Neutral Relay Open Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventHeaterFaultReset(void)
{
    switch (Neutral_Relay_Open_Monitor_State)
    {
        case STATE_NEUTRAL_RELAY_OPEN_MONITOR_OPEN_FAULT:
            RestartStabilizationTime();             // Exit function.
            Neutral_Relay_Open_Monitor_State = STATE_NEUTRAL_RELAY_OPEN_MONITOR_NO_FAILURE;
            ResetFailuresDetection();               // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Neutral_Relay_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the No Relay N Open Failure Present Event for the Neutral Relay Open Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNoRelayNOpenFailurePresent(void)
{
    switch (Neutral_Relay_Open_Monitor_State)
    {
        case STATE_NEUTRAL_RELAY_OPEN_MONITOR_RELAY_N_OPEN_DETECTED:
            Neutral_Relay_Open_Monitor_State = STATE_NEUTRAL_RELAY_OPEN_MONITOR_NO_FAILURE;
            ResetFailuresDetection();               // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Neutral_Relay_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Relay N Open Debounce Completed Event for the Neutral Relay Open Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventRelayNOpenDebounceCompleted(void)
{
    switch (Neutral_Relay_Open_Monitor_State)
    {
        case STATE_NEUTRAL_RELAY_OPEN_MONITOR_RELAY_N_OPEN_DETECTED:
            Neutral_Relay_Open_Monitor_State = STATE_NEUTRAL_RELAY_OPEN_MONITOR_OPEN_FAULT;
            break;

        default:
            // Event is ignored for all other values of Neutral_Relay_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Relay N Open Detected Event for the Neutral Relay Open Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventRelayNOpenDetected(void)
{
    switch (Neutral_Relay_Open_Monitor_State)
    {
        case STATE_NEUTRAL_RELAY_OPEN_MONITOR_NO_FAILURE:
            Neutral_Relay_Open_Monitor_State = STATE_NEUTRAL_RELAY_OPEN_MONITOR_RELAY_N_OPEN_DETECTED;
            SetRelayNOpenDetection();               // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Neutral_Relay_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Setting Data Missing Event for the Neutral Relay Open Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventSettingDataMissing(void)
{
    switch (Neutral_Relay_Open_Monitor_State)
    {
        case STATE_NEUTRAL_RELAY_OPEN_MONITOR_INITIALIZE:
            Neutral_Relay_Open_Monitor_State = STATE_NEUTRAL_RELAY_OPEN_MONITOR_NONE; // Terminate Neutral Relay Open Monitor state machine execution.
            break;

        default:
            // Event is ignored for all other values of Neutral_Relay_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Start Monitor Event for the Neutral Relay Open Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventStartMonitor(void)
{
    switch (Neutral_Relay_Open_Monitor_State)
    {
        case STATE_NEUTRAL_RELAY_OPEN_MONITOR_INITIALIZE:
            Neutral_Relay_Open_Monitor_State = STATE_NEUTRAL_RELAY_OPEN_MONITOR_NO_FAILURE;
            ResetFailuresDetection();               // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Neutral_Relay_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Neutral Relay Open Monitor state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void NeutralRelayOpenMonitorExecute(void)
{
    switch(Neutral_Relay_Open_Monitor_State)
    {
        case STATE_NEUTRAL_RELAY_OPEN_MONITOR_NO_FAILURE:
            CheckForFailures();                     // Do function.
            break;

        case STATE_NEUTRAL_RELAY_OPEN_MONITOR_RELAY_N_OPEN_DETECTED:
            CheckRelayNOpenRemoval();               // Do function.
            break;

        case STATE_NEUTRAL_RELAY_OPEN_MONITOR_OPEN_FAULT:
            MonitorRelayNOpenFaultState();          // Do function.
            break;

        default:
            // There is nothing to do for all other values of Neutral_Relay_Open_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Neutral Relay Open Monitor state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void NeutralRelayOpenMonitorInitialize(void)
{
    Neutral_Relay_Open_Monitor_State = STATE_NEUTRAL_RELAY_OPEN_MONITOR_INITIALIZE;
    InitializeModule();                             // Entry Function.
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

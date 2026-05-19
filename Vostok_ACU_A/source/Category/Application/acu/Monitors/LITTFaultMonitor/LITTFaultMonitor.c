/*
 *  *  @file       LITTFaultMonitor.c
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *
 *
 *
 *  Created on: 6 lis 2018
 *      Author: WAZNYP
 */


/**

   @startuml{LITTFaultMonitor State Machine.jpg}
        title Light In The Tub Fault Monitor State Machine

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

        FAULT : do /\t MonitorFaultState()
        FAULT --> NO_FAILURE : evLITTFaultReset

    @enduml

 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "LITTFaultMonitor.h"

// -- Other Modules --
#include "Fault.h"
#include "Hbl.h"
#include "Log.h"
#include "Micro.h"
#include "Supervisor.h"

//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Light In The Tub Fault Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Light In The Tub Fault Monitor state machine.
typedef enum LIGHT_IN_THE_TUB_FAULT_MONITOR_STATE_ENUM
{
    STATE_LIGHT_IN_THE_TUB_FAULT_MONITOR_NONE             = 0,

    STATE_LIGHT_IN_THE_TUB_FAULT_MONITOR_INITIALIZE       = 1, //!< (initial_state)
    STATE_LIGHT_IN_THE_TUB_FAULT_MONITOR_NO_FAILURE       = 2,
    STATE_LIGHT_IN_THE_TUB_FAULT_MONITOR_FAILURE_DETECTED = 3,
    STATE_LIGHT_IN_THE_TUB_FAULT_MONITOR_FAULT            = 4,

    STATE_LIGHT_IN_THE_TUB_FAULT_MONITOR_END              = 5
} LIGHT_IN_THE_TUB_FAULT_MONITOR_STATE_TYPE;

//! The state variable for the Light In The Tub Fault Monitor state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static LIGHT_IN_THE_TUB_FAULT_MONITOR_STATE_TYPE Light_In_The_Tub_Fault_Monitor_State;

// -- Private Function Prototypes used by the state machine for customization functions --
static void CheckFailureRemoval(void);
static void CheckForFailure(void);
static void InitializeModule(void);
static void MonitorFaultState(void);
static void ResetFailureDetection(void);
static void SetFailureDetection(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventDebounceCompleted(void);
static void EventFailureDetected(void);
static void EventLITTFaultReset(void);
static void EventNoFailurePresent(void);
static void EventSettingDataMissing(void);
static void EventStartMonitor(void);
static void LightInTheTubFaultMonitorExecute(void);
static void LightInTheTubFaultMonitorInitialize(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#define MODULE_NAME MODULE_LITT_FAULT_MONITOR



typedef enum MODULE_LITT_FAULT_MONITOR_LOG_MESSAGE_ID_ENUM
{
	ERROR_MISSING_COMPONENTS  = 1
} MODULE_LITT_FAULT_MONITOR_LOG_MESSAGE_ID_TYPE;

//! The consistency of the LITT load
static uint8 LITT_Load_Consistency;


//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//----------------------------------------------------------------------------
/**
 *  @brief      LITTFaultMonitor__Initialize
 *  @details    Function used to initialize the Light In The Tub Fault monitor.
 *
 */
void LITTFaultMonitor__Initialize(void)
{
	if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
	{
		LightInTheTubFaultMonitorInitialize();
	}
}


//----------------------------------------------------------------------------
/**
 *  @brief      LITTFaultMonitor__Execution
 *  @details    Function used to perform the LITT Fault monitor.
 *
 */
void LITTFaultMonitor__Execution(void)
{
	LITT_Load_Consistency = Hbl__GetLoadConsistency(Hbl__GetLoadIndexByLoadID(HBL_LOAD_LITT));

	LightInTheTubFaultMonitorExecute();
}


//----------------------------------------------------------------------------
/**
 *  @brief      LITTFaultMonitor__CycleStarted
 *  @details    Function to be called when a cycle is started to remove the LITT Fault.
 *
 */
void LITTFaultMonitor__CycleStarted(void)
{
	EventLITTFaultReset();
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
	if ((uint8)Fault__IsInFaultState(FAULT_ID_LITT_FAILURE) == (uint8)TRUE)
	{
		EventDebounceCompleted();
	}
	else
	{
		if ((Fault__IsObserverActive(FAULT_ID_LITT_FAILURE) == FALSE) || (LITT_Load_Consistency != COMBINE_NIBBLES(HBL_CONSISTENCY_TABLE_ON, HBL_CONSISTENCY_OPEN_CIRCUIT)))
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
	if (((uint8)Supervisor__IsTestCycleActive() == (uint8)TRUE) && (LITT_Load_Consistency == COMBINE_NIBBLES(HBL_CONSISTENCY_TABLE_ON, HBL_CONSISTENCY_OPEN_CIRCUIT)))
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
	LITT_Load_Consistency = COMBINE_NIBBLES(HBL_CONSISTENCY_TABLE_INVALID, HBL_CONSISTENCY_INVALID_VALUE);
	if (Hbl__GetLoadIndexByLoadID(HBL_LOAD_LITT) != (uint8)HBL_INDEX_INVALID)
	{
	    EventStartMonitor();
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
	Fault__ConditionRemoved(FAULT_ID_LITT_FAILURE);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetFailureDetection(void)
{
	Fault__ConditionDetected(FAULT_ID_LITT_FAILURE);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void MonitorFaultState(void)
{
	if (Fault__IsInFaultState(FAULT_ID_LITT_FAILURE) == FALSE)
	{
		EventLITTFaultReset();
	}
}





//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Light In The Tub Fault Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Debounce Completed Event for the Light In The Tub Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDebounceCompleted(void)
{
    switch (Light_In_The_Tub_Fault_Monitor_State)
    {
        case STATE_LIGHT_IN_THE_TUB_FAULT_MONITOR_FAILURE_DETECTED:
            Light_In_The_Tub_Fault_Monitor_State = STATE_LIGHT_IN_THE_TUB_FAULT_MONITOR_FAULT;
            break;

        default:
            // Event is ignored for all other values of Light_In_The_Tub_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Failure Detected Event for the Light In The Tub Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventFailureDetected(void)
{
    switch (Light_In_The_Tub_Fault_Monitor_State)
    {
        case STATE_LIGHT_IN_THE_TUB_FAULT_MONITOR_NO_FAILURE:
            Light_In_The_Tub_Fault_Monitor_State = STATE_LIGHT_IN_THE_TUB_FAULT_MONITOR_FAILURE_DETECTED;
            SetFailureDetection();                  // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Light_In_The_Tub_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the LITT Fault Reset Event for the Light In The Tub Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventLITTFaultReset(void)
{
    switch (Light_In_The_Tub_Fault_Monitor_State)
    {
        case STATE_LIGHT_IN_THE_TUB_FAULT_MONITOR_FAULT:
            Light_In_The_Tub_Fault_Monitor_State = STATE_LIGHT_IN_THE_TUB_FAULT_MONITOR_NO_FAILURE;
            ResetFailureDetection();                // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Light_In_The_Tub_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the No Failure Present Event for the Light In The Tub Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNoFailurePresent(void)
{
    switch (Light_In_The_Tub_Fault_Monitor_State)
    {
        case STATE_LIGHT_IN_THE_TUB_FAULT_MONITOR_FAILURE_DETECTED:
            Light_In_The_Tub_Fault_Monitor_State = STATE_LIGHT_IN_THE_TUB_FAULT_MONITOR_NO_FAILURE;
            ResetFailureDetection();                // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Light_In_The_Tub_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Setting Data Missing Event for the Light In The Tub Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventSettingDataMissing(void)
{
    switch (Light_In_The_Tub_Fault_Monitor_State)
    {
        case STATE_LIGHT_IN_THE_TUB_FAULT_MONITOR_INITIALIZE:
            Light_In_The_Tub_Fault_Monitor_State = STATE_LIGHT_IN_THE_TUB_FAULT_MONITOR_NONE; // Terminate Light In The Tub Fault Monitor state machine execution.
            break;

        default:
            // Event is ignored for all other values of Light_In_The_Tub_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Start Monitor Event for the Light In The Tub Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventStartMonitor(void)
{
    switch (Light_In_The_Tub_Fault_Monitor_State)
    {
        case STATE_LIGHT_IN_THE_TUB_FAULT_MONITOR_INITIALIZE:
            Light_In_The_Tub_Fault_Monitor_State = STATE_LIGHT_IN_THE_TUB_FAULT_MONITOR_NO_FAILURE;
            ResetFailureDetection();                // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Light_In_The_Tub_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Light In The Tub Fault Monitor state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void LightInTheTubFaultMonitorExecute(void)
{
    switch(Light_In_The_Tub_Fault_Monitor_State)
    {
        case STATE_LIGHT_IN_THE_TUB_FAULT_MONITOR_NO_FAILURE:
            CheckForFailure();                      // Do function.
            break;

        case STATE_LIGHT_IN_THE_TUB_FAULT_MONITOR_FAILURE_DETECTED:
            CheckFailureRemoval();                  // Do function.
            break;

        case STATE_LIGHT_IN_THE_TUB_FAULT_MONITOR_FAULT:
            MonitorFaultState();                    // Do function.
            break;

        default:
            // There is nothing to do for all other values of Light_In_The_Tub_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Light In The Tub Fault Monitor state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void LightInTheTubFaultMonitorInitialize(void)
{
    Light_In_The_Tub_Fault_Monitor_State = STATE_LIGHT_IN_THE_TUB_FAULT_MONITOR_INITIALIZE;
    InitializeModule();                             // Entry Function.
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

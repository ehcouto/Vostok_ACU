/**
 *  @file
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *
  @startuml{ThermistorShortedFaultMonitor State Machine.jpg}
        title Thermistor Shorted Fault State Machine

        [*] --> NO_FAILURE_DETECTED

        NO_FAILURE_DETECTED : entry / RemoveShortedFault()
        NO_FAILURE_DETECTED : do /\t CheckForShortedFault()
        NO_FAILURE_DETECTED --> SHORTED_DETECTED    : evShortedDetected


        SHORTED_DETECTED : entry /\t ShortedFaultDetected()
        SHORTED_DETECTED : do /\t CheckShortedRemoval()
        SHORTED_DETECTED --> NO_FAILURE_DETECTED  : evNoShortedFailurePresent
        SHORTED_DETECTED --> SHORTED_FAULT        : evShortedFailureDebounced

		SHORTED_FAULT : do /\t MonitorShortedFaultState()
		SHORTED_FAULT --> NO_FAILURE_DETECTED : evShortedFaultReset


    @enduml

 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "ThermistorFaultMonitor.h"
#include "ThermistorShortedMonitor.h"

// -- Other Modules --
#include "Fault.h"
#include "Selector.h"
#include "Interpreter.h"



//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Thermistor Shorted Fault State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Thermistor Shorted Fault state machine.
typedef enum THERMISTOR_SHORTED_FAULT_STATE_ENUM
{
    STATE_THERMISTOR_SHORTED_FAULT_NONE                = 0,

    STATE_THERMISTOR_SHORTED_FAULT_NO_FAILURE_DETECTED = 1, //!< (initial_state)
    STATE_THERMISTOR_SHORTED_FAULT_SHORTED_DETECTED    = 2,
    STATE_THERMISTOR_SHORTED_FAULT_SHORTED_FAULT       = 3,

    STATE_THERMISTOR_SHORTED_FAULT_END                 = 4
} THERMISTOR_SHORTED_FAULT_STATE_TYPE;

//! The state variable for the Thermistor Shorted Fault state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static THERMISTOR_SHORTED_FAULT_STATE_TYPE Thermistor_Shorted_Fault_State;

// -- Private Function Prototypes used by the state machine for customization functions --
static void CheckForShortedFault(void);
static void CheckShortedRemoval(void);
static void MonitorShortedFaultState(void);
static void RemoveShortedFault(void);
static void ShortedFaultDetected(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventNoShortedFailurePresent(void);
static void EventShortedDetected(void);
static void EventShortedFailureDebounced(void);
static void EventShortedFaultReset(void);
static void ThermistorShortedFaultExecute(void);
static void ThermistorShortedFaultInitialize(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define THERMISTOR_HIGH_THRESHOLD 	ThermistorFaultMonitor__GetShortedThreshold()



//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief
 *
 */
void ThermistorShortedFaultMonitor__Initialize(void)
{
	ThermistorShortedFaultInitialize();
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module ThermistorFault and its variables
 *
 */
void ThermistorShortedFaultMonitor__Execution(void)
{
	ThermistorShortedFaultExecute();
}


//----------------------------------------------------------------------------
/**
 *  @brief
 *  @details
 *
 */
void ThermistorShortedFaultMonitor__ExitFault(void)
{
	EventShortedFaultReset();
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
static void CheckForShortedFault(void)
{
	CYCLE_STATE_TYPE label;

	label = Selector__GetCycleState(0, COMPARTMENT_DEFAULT, Interpreter__GetCurrentStatement(COMPARTMENT_DEFAULT).Cycle);

	if (label == CYCLE_STATE_RUN)
	{
		sint16 washprobetemp = ThermistorFaultMonitor__GetTemperature();
		if (washprobetemp >= THERMISTOR_HIGH_THRESHOLD)
		{
			EventShortedDetected();
		}
		else
		{
			// No shorted fault detected
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void CheckShortedRemoval(void)
{
	if ((uint8)Fault__IsInFaultState(FAULT_ID_THERMISTOR_SHORTED) == (uint8)TRUE)
	{
		EventShortedFailureDebounced();
	}
	else
	{
		CYCLE_STATE_TYPE label;

		label = Selector__GetCycleState(0, COMPARTMENT_DEFAULT, Interpreter__GetCurrentStatement(COMPARTMENT_DEFAULT).Cycle);

		sint16 washprobetemp = ThermistorFaultMonitor__GetTemperature();

		if ((label != CYCLE_STATE_RUN) ||
			(Fault__IsObserverActive(FAULT_ID_THERMISTOR_SHORTED) == FALSE) ||
			(washprobetemp < THERMISTOR_HIGH_THRESHOLD))
		{
			EventNoShortedFailurePresent();
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void ShortedFaultDetected(void)
{
	Fault__ConditionDetected(FAULT_ID_THERMISTOR_SHORTED);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void RemoveShortedFault(void)
{
	Fault__ConditionRemoved(FAULT_ID_THERMISTOR_SHORTED);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void MonitorShortedFaultState(void)
{
	if (Fault__IsInFaultState(FAULT_ID_THERMISTOR_SHORTED) == FALSE)
	{
		EventShortedFaultReset();
	}
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Thermistor Shorted Fault State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the No Shorted Failure Present Event for the Thermistor Shorted Fault state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNoShortedFailurePresent(void)
{
    switch (Thermistor_Shorted_Fault_State)
    {
        case STATE_THERMISTOR_SHORTED_FAULT_SHORTED_DETECTED:
            Thermistor_Shorted_Fault_State = STATE_THERMISTOR_SHORTED_FAULT_NO_FAILURE_DETECTED;
            RemoveShortedFault();                   // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Thermistor_Shorted_Fault_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Shorted Detected Event for the Thermistor Shorted Fault state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventShortedDetected(void)
{
    switch (Thermistor_Shorted_Fault_State)
    {
        case STATE_THERMISTOR_SHORTED_FAULT_NO_FAILURE_DETECTED:
            Thermistor_Shorted_Fault_State = STATE_THERMISTOR_SHORTED_FAULT_SHORTED_DETECTED;
            ShortedFaultDetected();                 // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Thermistor_Shorted_Fault_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Shorted Failure Debounced Event for the Thermistor Shorted Fault state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventShortedFailureDebounced(void)
{
    switch (Thermistor_Shorted_Fault_State)
    {
        case STATE_THERMISTOR_SHORTED_FAULT_SHORTED_DETECTED:
            Thermistor_Shorted_Fault_State = STATE_THERMISTOR_SHORTED_FAULT_SHORTED_FAULT;
            break;

        default:
            // Event is ignored for all other values of Thermistor_Shorted_Fault_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Shorted Fault Reset Event for the Thermistor Shorted Fault state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventShortedFaultReset(void)
{
    switch (Thermistor_Shorted_Fault_State)
    {
        case STATE_THERMISTOR_SHORTED_FAULT_SHORTED_FAULT:
            Thermistor_Shorted_Fault_State = STATE_THERMISTOR_SHORTED_FAULT_NO_FAILURE_DETECTED;
            RemoveShortedFault();                   // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Thermistor_Shorted_Fault_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Thermistor Shorted Fault state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void ThermistorShortedFaultExecute(void)
{
    switch(Thermistor_Shorted_Fault_State)
    {
        case STATE_THERMISTOR_SHORTED_FAULT_NO_FAILURE_DETECTED:
            CheckForShortedFault();                 // Do function.
            break;

        case STATE_THERMISTOR_SHORTED_FAULT_SHORTED_DETECTED:
            CheckShortedRemoval();                  // Do function.
            break;

        case STATE_THERMISTOR_SHORTED_FAULT_SHORTED_FAULT:
            MonitorShortedFaultState();             // Do function.
            break;

        default:
            // There is nothing to do for all other values of Thermistor_Shorted_Fault_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Thermistor Shorted Fault state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void ThermistorShortedFaultInitialize(void)
{
    Thermistor_Shorted_Fault_State = STATE_THERMISTOR_SHORTED_FAULT_NO_FAILURE_DETECTED;
    RemoveShortedFault();                           // Entry Function.
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

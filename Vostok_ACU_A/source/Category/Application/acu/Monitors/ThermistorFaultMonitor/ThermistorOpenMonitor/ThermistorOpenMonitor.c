/**
 *  @file
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *
  @startuml{ThermistorOpenFaultMonitor State Machine.jpg}
        title Thermistor Open Fault State Machine

        [*] --> NO_FAILURE_DETECTED

        NO_FAILURE_DETECTED : entry / RemoveOpenFault()
        NO_FAILURE_DETECTED : do /\t CheckForOpenFault()
        NO_FAILURE_DETECTED --> OPEN_DETECTED    : evOpenDetected


        OPEN_DETECTED : entry /\t OpenFaultDetected()
        OPEN_DETECTED : do /\t CheckOpenRemoval()
        OPEN_DETECTED --> NO_FAILURE_DETECTED  : evNoOpenFailurePresent
        OPEN_DETECTED --> OPEN_FAULT           : evOpenFailureDebounced

		OPEN_FAULT : do /\t MonitorOpenFaultState()
		OPEN_FAULT --> NO_FAILURE_DETECTED : evOpenFaultReset


    @enduml

 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "ThermistorFaultMonitor.h"
#include "ThermistorOpenMonitor.h"

// -- Other Modules --
#include "Fault.h"
#include "Selector.h"
#include "Interpreter.h"



//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Thermistor Open Fault State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Thermistor Open Fault state machine.
typedef enum THERMISTOR_OPEN_FAULT_STATE_ENUM
{
    STATE_THERMISTOR_OPEN_FAULT_NONE                = 0,

    STATE_THERMISTOR_OPEN_FAULT_NO_FAILURE_DETECTED = 1, //!< (initial_state)
    STATE_THERMISTOR_OPEN_FAULT_OPEN_DETECTED       = 2,
    STATE_THERMISTOR_OPEN_FAULT_OPEN_FAULT          = 3,

    STATE_THERMISTOR_OPEN_FAULT_END                 = 4
} THERMISTOR_OPEN_FAULT_STATE_TYPE;

//! The state variable for the Thermistor Open Fault state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static THERMISTOR_OPEN_FAULT_STATE_TYPE Thermistor_Open_Fault_State;

// -- Private Function Prototypes used by the state machine for customization functions --
static void CheckForOpenFault(void);
static void CheckOpenRemoval(void);
static void MonitorOpenFaultState(void);
static void OpenFaultDetected(void);
static void RemoveOpenFault(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventNoOpenFailurePresent(void);
static void EventOpenDetected(void);
static void EventOpenFailureDebounced(void);
static void EventOpenFaultReset(void);
static void ThermistorOpenFaultExecute(void);
static void ThermistorOpenFaultInitialize(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define THERMISTOR_LOW_THRESHOLD	0



//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief
 *
 */
void ThermistorOpenFaultMonitor__Initialize(void)
{
	ThermistorOpenFaultInitialize();
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module ThermistorFault and its variables
 *
 */
void ThermistorOpenFaultMonitor__Execution(void)
{
	ThermistorOpenFaultExecute();
}


//----------------------------------------------------------------------------
/**
 *  @brief
 *  @details
 *
 */
void ThermistorOpenFaultMonitor__ExitFault(void)
{
	EventOpenFaultReset();
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
static void CheckForOpenFault(void)
{
	CYCLE_STATE_TYPE label;

	label = Selector__GetCycleState(0, COMPARTMENT_DEFAULT, Interpreter__GetCurrentStatement(COMPARTMENT_DEFAULT).Cycle);

	if (label == CYCLE_STATE_RUN)
	{
		if (ThermistorFaultMonitor__GetTemperature() < THERMISTOR_LOW_THRESHOLD)
		{
			EventOpenDetected();
		}
		else
		{
			// No open fault detected
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void CheckOpenRemoval(void)
{
	if ((uint8)Fault__IsInFaultState(FAULT_ID_THERMISTOR_OPEN) == (uint8)TRUE)
	{
		EventOpenFailureDebounced();
	}
	else
	{
		CYCLE_STATE_TYPE label;

		label = Selector__GetCycleState(0, COMPARTMENT_DEFAULT, Interpreter__GetCurrentStatement(COMPARTMENT_DEFAULT).Cycle);

		if ((label != CYCLE_STATE_RUN) ||
			(Fault__IsObserverActive(FAULT_ID_THERMISTOR_OPEN) == FALSE) ||
			(ThermistorFaultMonitor__GetTemperature() >= THERMISTOR_LOW_THRESHOLD))
		{
			EventNoOpenFailurePresent();
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void OpenFaultDetected(void)
{
	Fault__ConditionDetected(FAULT_ID_THERMISTOR_OPEN);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void RemoveOpenFault(void)
{
	Fault__ConditionRemoved(FAULT_ID_THERMISTOR_OPEN);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void MonitorOpenFaultState(void)
{
	if (Fault__IsInFaultState(FAULT_ID_THERMISTOR_OPEN) == FALSE)
	{
		EventOpenFaultReset();
	}
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Thermistor Open Fault State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the No Open Failure Present Event for the Thermistor Open Fault state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNoOpenFailurePresent(void)
{
    switch (Thermistor_Open_Fault_State)
    {
        case STATE_THERMISTOR_OPEN_FAULT_OPEN_DETECTED:
            Thermistor_Open_Fault_State = STATE_THERMISTOR_OPEN_FAULT_NO_FAILURE_DETECTED;
            RemoveOpenFault();                      // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Thermistor_Open_Fault_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Open Detected Event for the Thermistor Open Fault state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventOpenDetected(void)
{
    switch (Thermistor_Open_Fault_State)
    {
        case STATE_THERMISTOR_OPEN_FAULT_NO_FAILURE_DETECTED:
            Thermistor_Open_Fault_State = STATE_THERMISTOR_OPEN_FAULT_OPEN_DETECTED;
            OpenFaultDetected();                    // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Thermistor_Open_Fault_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Open Failure Debounced Event for the Thermistor Open Fault state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventOpenFailureDebounced(void)
{
    switch (Thermistor_Open_Fault_State)
    {
        case STATE_THERMISTOR_OPEN_FAULT_OPEN_DETECTED:
            Thermistor_Open_Fault_State = STATE_THERMISTOR_OPEN_FAULT_OPEN_FAULT;
            break;

        default:
            // Event is ignored for all other values of Thermistor_Open_Fault_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Open Fault Reset Event for the Thermistor Open Fault state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventOpenFaultReset(void)
{
    switch (Thermistor_Open_Fault_State)
    {
        case STATE_THERMISTOR_OPEN_FAULT_OPEN_FAULT:
            Thermistor_Open_Fault_State = STATE_THERMISTOR_OPEN_FAULT_NO_FAILURE_DETECTED;
            RemoveOpenFault();                      // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Thermistor_Open_Fault_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Thermistor Open Fault state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void ThermistorOpenFaultExecute(void)
{
    switch(Thermistor_Open_Fault_State)
    {
        case STATE_THERMISTOR_OPEN_FAULT_NO_FAILURE_DETECTED:
            CheckForOpenFault();                    // Do function.
            break;

        case STATE_THERMISTOR_OPEN_FAULT_OPEN_DETECTED:
            CheckOpenRemoval();                     // Do function.
            break;

        case STATE_THERMISTOR_OPEN_FAULT_OPEN_FAULT:
            MonitorOpenFaultState();                // Do function.
            break;

        default:
            // There is nothing to do for all other values of Thermistor_Open_Fault_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Thermistor Open Fault state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void ThermistorOpenFaultInitialize(void)
{
    Thermistor_Open_Fault_State = STATE_THERMISTOR_OPEN_FAULT_NO_FAILURE_DETECTED;
    RemoveOpenFault();                              // Entry Function.
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

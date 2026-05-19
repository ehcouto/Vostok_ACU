/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *
   @startuml{TriacShortedDrainFaultMonitor State Machine.jpg}
        title Triac Shorted Drain Motor Fault Monitor State Machine

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

        FAULT : exit /\t SetStabilizationTime()
        FAULT : do /\t MonitorFaultState()
        FAULT --> NO_FAILURE : evTriacShortedDrainFaultReset

    @enduml

 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "TriacShortedDrainFaultMonitor.h"

#include "SystemConfig.h"
#include "Fault.h"
#include "Hbl.h"
#include "Log.h"
#include "Micro.h"
#include "SettingFile.h"
#include "Vwi.h"


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Triac Shorted Drain Motor Fault Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Triac Shorted Drain Motor Fault Monitor state machine.
typedef enum TRIAC_SHORTED_DRAIN_MOTOR_FAULT_MONITOR_STATE_ENUM
{
    STATE_TRIAC_SHORTED_DRAIN_MOTOR_FAULT_MONITOR_NONE             = 0,

    STATE_TRIAC_SHORTED_DRAIN_MOTOR_FAULT_MONITOR_INITIALIZE       = 1, //!< (initial_state)
    STATE_TRIAC_SHORTED_DRAIN_MOTOR_FAULT_MONITOR_NO_FAILURE       = 2,
    STATE_TRIAC_SHORTED_DRAIN_MOTOR_FAULT_MONITOR_FAILURE_DETECTED = 3,
    STATE_TRIAC_SHORTED_DRAIN_MOTOR_FAULT_MONITOR_FAULT            = 4,

    STATE_TRIAC_SHORTED_DRAIN_MOTOR_FAULT_MONITOR_END              = 5
} TRIAC_SHORTED_DRAIN_MOTOR_FAULT_MONITOR_STATE_TYPE;

//! The state variable for the Triac Shorted Drain Motor Fault Monitor state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static TRIAC_SHORTED_DRAIN_MOTOR_FAULT_MONITOR_STATE_TYPE Triac_Shorted_Drain_Motor_Fault_Monitor_State;

// -- Private Function Prototypes used by the state machine for customization functions --
static void CheckFailureRemoval(void);
static void CheckForFailure(void);
static void InitializeModule(void);
static void MonitorFaultState(void);
static void ResetFailureDetection(void);
static void SetFailureDetection(void);
static void SetStabilizationTime(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventDebounceCompleted(void);
static void EventFailureDetected(void);
static void EventNoFailurePresent(void);
static void EventSettingDataMissing(void);
static void EventStartMonitor(void);
static void EventTriacShortedDrainFaultReset(void);
static void TriacShortedDrainMotorFaultMonitorExecute(void);
static void TriacShortedDrainMotorFaultMonitorInitialize(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#define MODULE_NAME MODULE_TRIAC_SHORTED_DRAIN_FAULT_MONITOR

//! Time to wait after a change in drain status or after resetting the fault,
// before evaluating again the drain current feedback (unit 100 ms).
#define DRAIN_TRIAC_FAULT_STABILIZATION_TIME    5

typedef enum MODULE_DRAIN_TRIAC_FAULT_MONITOR_LOG_MESSAGE_ID_ENUM
{
    ERROR_MISSING_COMPONENTS  = 1,
    ERROR_MISSING_PARAMETERS
} MODULE_TRIAC_SHORTED_DRAIN_FAULT_MONITOR_LOG_MESSAGE_ID_TYPE;


// The parameters for detecting the Drain Triac fault as read from Setting File
static DRAIN_TRIAC_FAULT_PARAMS_TYPE Drain_Fault_Params;
//! Counter for stabilization time.
static uint8 Drain_Triac_Fault_Stabilization_Counter;

static uint8 Drain_Current_GI_Index;
//! The current on the Drain triac as measured by the small triac current sensing circuit (units mA)
static uint16 Measured_Drain_Triac_Current;
// State of the Pilot in the current iteration
static uint8 Pilot_State;
// State of the Drain in the current iteration
static uint8 Drain_State;
// State of the Drain in the previous iteration
static uint8 Drain_Previous_State;
// State of the Vent in the current iteration
static uint8 Vent_State;
// State of the Vent in the previous iteration
static uint8 Vent_Previous_State;
// State of the VWI No Water in the current iteration
static BOOL_TYPE VWI_No_Water_State;
// State of the VWI No Water in the previous iteration
static BOOL_TYPE VWI_No_Water_Previous_State;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//----------------------------------------------------------------------------
/**
 *  @brief      TriacShortedDrainFaultMonitor__Initialize
 *  @details    Function used to initialize the Triac Shorted Drain Fault monitor.
 *
 */
void TriacShortedDrainFaultMonitor__Initialize(void)
{
    if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
    {
        Pilot_State = OFF;
        Drain_State = OFF;
        Vent_State = OFF;
        VWI_No_Water_State = FALSE;
        Drain_Previous_State = OFF;
        Vent_Previous_State = OFF;
        VWI_No_Water_Previous_State = FALSE;
        TriacShortedDrainMotorFaultMonitorInitialize();
    }
}


//----------------------------------------------------------------------------
/**
 *  @brief      TriacShortedDrainFaultMonitor__Execution
 *  @details    Function used to perform the Triac Shorted Drain Fault monitor.
 *
 */
void TriacShortedDrainFaultMonitor__Execution(void)
{
    uint8 *load_state_ptr;

    load_state_ptr = (uint8*)(Hbl__GetLoadStatusByLoadID(HBL_LOAD_PILOT_RELAY));
    if (load_state_ptr != NULL)
    {
    	Pilot_State = (load_state_ptr[0] == (uint8)ON) ? ON:OFF;
    }
    else
    {
        load_state_ptr = (uint8*)(Hbl__GetLoadStatusByLoadID(HBL_LOAD_RELAY_ENABLE));
        if (load_state_ptr != NULL)
        {
        	Pilot_State = (load_state_ptr[0] == (uint8)ON) ? ON:OFF;
        }
        else
        {
        	Pilot_State = ON;
        }
    }

    load_state_ptr = (uint8 *)Hbl__GetProcessedLoad(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DRAIN_PUMP));
    if (load_state_ptr != NULL)
    {
    	Drain_State = load_state_ptr[0];
    }

    load_state_ptr = (uint8 *)Hbl__GetProcessedLoad(Hbl__GetLoadIndexByLoadID(HBL_LOAD_VENT));
    if (load_state_ptr != NULL)
    {
    	Vent_State =  load_state_ptr[0];
    }

    VWI_No_Water_State = ((WS_NO_WATER == Vwi__GetWaterStatus())? TRUE:FALSE);

    if (Drain_State != Drain_Previous_State)
    {
        Drain_Triac_Fault_Stabilization_Counter = DRAIN_TRIAC_FAULT_STABILIZATION_TIME;
        Drain_Previous_State = Drain_State;
    }
    else if (Vent_State != Vent_Previous_State)
    {
        Drain_Triac_Fault_Stabilization_Counter = 20;
        Vent_Previous_State = Vent_State;
    }
    else if ((VWI_No_Water_State != VWI_No_Water_Previous_State) && ((uint8)TRUE == (uint8)VWI_No_Water_State))
    {
        Drain_Triac_Fault_Stabilization_Counter = 20;
    }
    else
    {
        if (Drain_Triac_Fault_Stabilization_Counter > 0U)
        {
            Drain_Triac_Fault_Stabilization_Counter --;
        }
    }
    VWI_No_Water_Previous_State = VWI_No_Water_State;
    Measured_Drain_Triac_Current = (uint16)Hbl__GetGIDataConvertedByIndex(Drain_Current_GI_Index);
    TriacShortedDrainMotorFaultMonitorExecute();
}


//----------------------------------------------------------------------------
/**
 *  @brief      TriacShortedDrainFaultMonitor__CycleStarted
 *  @details    Function called when a cycle is started to remove the Fan Motor Fault.
 *
 */
void TriacShortedDrainFaultMonitor__CycleStarted(void)
{
    EventTriacShortedDrainFaultReset();
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
    if ((uint8)Fault__IsInFaultState(FAULT_ID_DRAIN_MOTOR_TRIAC_SHORTED) == (uint8)TRUE)
    {
        EventDebounceCompleted();
    }
    else
    {
        if ((Drain_State == (uint8)ON) ||
            (Fault__IsObserverActive(FAULT_ID_DRAIN_MOTOR_TRIAC_SHORTED) == FALSE)
            || (Measured_Drain_Triac_Current < Drain_Fault_Params.Current_Sense_Drain_Min)
            || !VWI_No_Water_State)
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
    if ((Pilot_State == (uint8)ON) && (Drain_State == (uint8)OFF) && (Drain_Triac_Fault_Stabilization_Counter == 0U)
        && (Measured_Drain_Triac_Current > Drain_Fault_Params.Current_Sense_Drain_Min)
        && VWI_No_Water_State)
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
    uint8 drain_index;
    DRAIN_TRIAC_FAULT_PARAMS_TYPE *drain_triac_fault_params;

    Measured_Drain_Triac_Current = 0;
    Drain_Triac_Fault_Stabilization_Counter = DRAIN_TRIAC_FAULT_STABILIZATION_TIME;

    Drain_Previous_State = OFF;
    Drain_State = OFF;

    drain_index = Hbl__GetLoadIndexByLoadID(HBL_LOAD_DRAIN_PUMP);
    Drain_Current_GI_Index = Hbl__GetGIIndexByGIID(HBL_GI_I_DRAIN_PUMP);

    if (Drain_Current_GI_Index == (uint8)HBL_INDEX_INVALID)
    {
        Drain_Current_GI_Index = Hbl__GetGIIndexByGIID(HBL_GI_I_DIAGNOSTIC);
    }

    if ((Drain_Current_GI_Index != (uint8)HBL_INDEX_INVALID) && (drain_index != (uint8)HBL_INDEX_INVALID))
    {
        drain_triac_fault_params = (DRAIN_TRIAC_FAULT_PARAMS_TYPE*)Fault__GetSpecificData(FAULT_ID_SINGLE_SPEED_DRAIN_MOTOR);

        if (drain_triac_fault_params != NULL)
        {
            Drain_Fault_Params.Current_Sense_Drain_Min = ENDIANU16_STON(drain_triac_fault_params->Current_Sense_Drain_Min);
            Drain_Fault_Params.Current_Sense_Drain_Max = ENDIANU16_STON(drain_triac_fault_params->Current_Sense_Drain_Max);

            EventStartMonitor();
        }
        else
        {
            EventSettingDataMissing();
            LOG_ADD_EXCEPTION (ERROR_MISSING_PARAMETERS, FAULT_ID_SINGLE_SPEED_DRAIN_MOTOR);
        }
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
    Fault__ConditionRemoved(FAULT_ID_DRAIN_MOTOR_TRIAC_SHORTED);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetFailureDetection(void)
{
    Fault__ConditionDetected(FAULT_ID_DRAIN_MOTOR_TRIAC_SHORTED);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetStabilizationTime(void)
{
    Drain_Triac_Fault_Stabilization_Counter = DRAIN_TRIAC_FAULT_STABILIZATION_TIME;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void MonitorFaultState(void)
{
    if (Fault__IsInFaultState(FAULT_ID_DRAIN_MOTOR_TRIAC_SHORTED) == FALSE)
    {
        EventTriacShortedDrainFaultReset();
    }
}




//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Triac Shorted Drain Motor Fault Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Debounce Completed Event for the Triac Shorted Drain Motor Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDebounceCompleted(void)
{
    switch (Triac_Shorted_Drain_Motor_Fault_Monitor_State)
    {
        case STATE_TRIAC_SHORTED_DRAIN_MOTOR_FAULT_MONITOR_FAILURE_DETECTED:
            Triac_Shorted_Drain_Motor_Fault_Monitor_State = STATE_TRIAC_SHORTED_DRAIN_MOTOR_FAULT_MONITOR_FAULT;
            break;

        default:
            // Event is ignored for all other values of Triac_Shorted_Drain_Motor_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Failure Detected Event for the Triac Shorted Drain Motor Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventFailureDetected(void)
{
    switch (Triac_Shorted_Drain_Motor_Fault_Monitor_State)
    {
        case STATE_TRIAC_SHORTED_DRAIN_MOTOR_FAULT_MONITOR_NO_FAILURE:
            Triac_Shorted_Drain_Motor_Fault_Monitor_State = STATE_TRIAC_SHORTED_DRAIN_MOTOR_FAULT_MONITOR_FAILURE_DETECTED;
            SetFailureDetection();                  // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Triac_Shorted_Drain_Motor_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the No Failure Present Event for the Triac Shorted Drain Motor Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNoFailurePresent(void)
{
    switch (Triac_Shorted_Drain_Motor_Fault_Monitor_State)
    {
        case STATE_TRIAC_SHORTED_DRAIN_MOTOR_FAULT_MONITOR_FAILURE_DETECTED:
            Triac_Shorted_Drain_Motor_Fault_Monitor_State = STATE_TRIAC_SHORTED_DRAIN_MOTOR_FAULT_MONITOR_NO_FAILURE;
            ResetFailureDetection();                // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Triac_Shorted_Drain_Motor_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Setting Data Missing Event for the Triac Shorted Drain Motor Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventSettingDataMissing(void)
{
    switch (Triac_Shorted_Drain_Motor_Fault_Monitor_State)
    {
        case STATE_TRIAC_SHORTED_DRAIN_MOTOR_FAULT_MONITOR_INITIALIZE:
            Triac_Shorted_Drain_Motor_Fault_Monitor_State = STATE_TRIAC_SHORTED_DRAIN_MOTOR_FAULT_MONITOR_NONE; // Terminate Triac Shorted Drain Motor Fault Monitor state machine execution.
            break;

        default:
            // Event is ignored for all other values of Triac_Shorted_Drain_Motor_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Start Monitor Event for the Triac Shorted Drain Motor Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventStartMonitor(void)
{
    switch (Triac_Shorted_Drain_Motor_Fault_Monitor_State)
    {
        case STATE_TRIAC_SHORTED_DRAIN_MOTOR_FAULT_MONITOR_INITIALIZE:
            Triac_Shorted_Drain_Motor_Fault_Monitor_State = STATE_TRIAC_SHORTED_DRAIN_MOTOR_FAULT_MONITOR_NO_FAILURE;
            ResetFailureDetection();                // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Triac_Shorted_Drain_Motor_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Triac Shorted Drain Fault Reset Event for the Triac Shorted Drain Motor Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventTriacShortedDrainFaultReset(void)
{
    switch (Triac_Shorted_Drain_Motor_Fault_Monitor_State)
    {
        case STATE_TRIAC_SHORTED_DRAIN_MOTOR_FAULT_MONITOR_FAULT:
            SetStabilizationTime();                 // Exit function.
            Triac_Shorted_Drain_Motor_Fault_Monitor_State = STATE_TRIAC_SHORTED_DRAIN_MOTOR_FAULT_MONITOR_NO_FAILURE;
            ResetFailureDetection();                // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Triac_Shorted_Drain_Motor_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Triac Shorted Drain Motor Fault Monitor state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void TriacShortedDrainMotorFaultMonitorExecute(void)
{
    switch(Triac_Shorted_Drain_Motor_Fault_Monitor_State)
    {
        case STATE_TRIAC_SHORTED_DRAIN_MOTOR_FAULT_MONITOR_NO_FAILURE:
            CheckForFailure();                      // Do function.
            break;

        case STATE_TRIAC_SHORTED_DRAIN_MOTOR_FAULT_MONITOR_FAILURE_DETECTED:
            CheckFailureRemoval();                  // Do function.
            break;

        case STATE_TRIAC_SHORTED_DRAIN_MOTOR_FAULT_MONITOR_FAULT:
            MonitorFaultState();                    // Do function.
            break;

        default:
            // There is nothing to do for all other values of Triac_Shorted_Drain_Motor_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Triac Shorted Drain Motor Fault Monitor state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void TriacShortedDrainMotorFaultMonitorInitialize(void)
{
    Triac_Shorted_Drain_Motor_Fault_Monitor_State = STATE_TRIAC_SHORTED_DRAIN_MOTOR_FAULT_MONITOR_INITIALIZE;
    InitializeModule();                             // Entry Function.
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

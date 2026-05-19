/**
 *  @file       FillValveStuckOn.c
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *
 *    @startuml{FillValveStuckON.jpg}
 *    scale 0.8
        title Fill Valve Stuck ON State Machine

        [*] --> Init

        Init : entry / FillValveStuckONInit()
        Init --> [*]                        : EventSettingDataMissing
        Init -r-> Debounce_Init             : EventStartDetection

        Debounce_Init : entry / StartTimer()
        Debounce_Init : do / WaitForTimer()
        Debounce_Init -r-> Count_Pilot_ON   : EventDebounceInputOK

        Count_Pilot_ON : entry / StartCountPulses()
        Count_Pilot_ON : do / CountPulses()
        Count_Pilot_ON --> Count_Pilot_ON   : EventTimerElapseWithoutPulses
        Count_Pilot_ON --> Debounce_Init    : EventDebounceInputNOK
        Count_Pilot_ON --> Count_Pilot_OFF : EventTimerElapseWithPulses / SetF8E5Prefault()

        Count_Pilot_OFF : entry / StartCountPulses() \n\t SwitchOFFPilot()
        Count_Pilot_OFF : do / CountPulses() \n\t SwitchOFFPilot()
        Count_Pilot_OFF --> Debounce_Init   : EventDebounceInputNOK
        Count_Pilot_OFF -r-> Prefault_Fill_ON    : EventTimerElapseWithPulses
        Count_Pilot_OFF --> SetF1E13        : EventTimerElapseWithoutPulses / ResetF8E5Prefault()

        Prefault_Fill_ON : Entry / SwitchFillON() \n\t StartPrefaultTimerON()
        Prefault_Fill_ON : do / SwitchFillON() \n\t WaitForPrefaultTimer()
        Prefault_Fill_ON -r-> Prefault_Fill_OFF   : EventTimerExpired

        Prefault_Fill_OFF : entry / SwitchFillOFF() \n\t StartPrefaultTimerOFF()
        Prefault_Fill_OFF : do / SwitchFillOFF() \n\t WaitForPrefaultTimer()
        Prefault_Fill_OFF -r-> Prefault_Fill_OFF_Counting    : EventTimerExpired

        Prefault_Fill_OFF_Counting : Entry / SwitchFillOFF() \n\t StartCountPulses()
        Prefault_Fill_OFF_Counting : do / SwitchFillOFF() \n\t CountPulses()
        Prefault_Fill_OFF_Counting --> SetF8E5 : EventNoMoreTrials
        Prefault_Fill_OFF_Counting --> Prefault_Fill_ON : EventTimerElapseWithPulses
        Prefault_Fill_OFF_Counting --> Debounce_Init : EventTimerElapseWithoutPulses / ResetF8E5Prefault()

        SetF1E13 : entry / SetFailureF1E13()
        SetF1E13 --> Debounce_Init          : EventFillFaultReset

        SetF8E5 : entry / SetFailureF8E5()
        SetF8E5 --> Debounce_Init           : EventFillFaultReset

        note top of Debounce_Init: Check if Pilot and Fill are OFF for 20s
        note top of Count_Pilot_ON: Count pulses for 10 s
        note top of Count_Pilot_OFF: Keep Pilot OFF and count pulses for 10 s
        note top of Prefault_Fill_ON: Keep Fill ON for TON time
        note top of Prefault_Fill_OFF: Keep Fill OFF for debounce time
        note top of Prefault_Fill_OFF_Counting: Count pulses for count time

    @enduml
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2011.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "DishAuxVar.h"
#include "FillValveStuckOn.h"
#include "Fault.h"
#include "Log.h"
#include "Hbl.h"
#include "SettingFile.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#define MODULE_NAME MODULE_FILL_VALVE_FAULT_MONITOR

typedef enum MODULE_FILL_VALVE_FAULT_MONITOR_LOG_MESSAGE_ID_ENUM
{
    ERROR_LOADING_PARAMETERS       = 1,
} MODULE_FILL_VALVE_FAULT_MONITOR_LOG_MESSAGE_ID_TYPE;


static uint16 Fill_Stuck_ON_Debounce_Time;
static uint16 Fill_Stuck_ON_Pulses_Detection_period;
static uint8 Fill_Stuck_ON_Pulses_Number;
static uint16 Fill_Stuck_ON_Current_Debounce_Timer;
static uint16 Fill_Stuck_ON_Pulse_Counter;
static ON_OFF_TYPE Fill_Stuck_ON_Pilot_State;
static ON_OFF_TYPE Fill_Stuck_ON_Fill_Valve_State;
static uint16 Pulse_Count;
static uint16 Pulse_Count_Old;
static uint8 Fill_Valve_Fault_Max_Retry_Number;
static uint8 Fill_Valve_TON_Time;
static uint8 Fill_Valve_Fault_Retry_Counter;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Fill Valve Stuck ON State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Fill Valve Stuck ON state machine.
typedef enum FILL_VALVE_STUCK_ON_STATE_ENUM
{
    STATE_FILL_VALVE_STUCK_ON_NONE                       = 0,

    STATE_FILL_VALVE_STUCK_ON_INIT                       = 1, //!< (initial_state)
    STATE_FILL_VALVE_STUCK_ON_DEBOUNCE_INIT              = 2,
    STATE_FILL_VALVE_STUCK_ON_COUNT_PILOT_ON             = 3,
    STATE_FILL_VALVE_STUCK_ON_COUNT_PILOT_OFF            = 4,
    STATE_FILL_VALVE_STUCK_ON_PREFAULT_FILL_ON           = 5,
    STATE_FILL_VALVE_STUCK_ON_SET_F1E13                  = 6,
    STATE_FILL_VALVE_STUCK_ON_PREFAULT_FILL_OFF          = 7,
    STATE_FILL_VALVE_STUCK_ON_PREFAULT_FILL_OFF_COUNTING = 8,
    STATE_FILL_VALVE_STUCK_ON_SET_F8E5                   = 9,

    STATE_FILL_VALVE_STUCK_ON_END                        = 10
} FILL_VALVE_STUCK_ON_STATE_TYPE;

//! The state variable for the Fill Valve Stuck ON state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static FILL_VALVE_STUCK_ON_STATE_TYPE Fill_Valve_Stuck_ON_State;

//! The transition variable for the Fill Valve Stuck ON state machine.
//! Flag used by the state machine to track when a transition occurs in the middle of a state's
//! Entry() or Do() action that contains more than one function.
static BOOL_TYPE Fill_Valve_Stuck_ON_Transitioned;

// -- Private Function Prototypes used by the state machine for customization functions --
static void CountPulses(void);
static void FillValveStuckONInit(void);
static void ResetF8E5Prefault(void);
static void SetF8E5Prefault(void);
static void SetFailureF1E13(void);
static void SetFailureF8E5(void);
static void StartCountPulses(void);
static void StartPrefaultTimerOFF(void);
static void StartPrefaultTimerON(void);
static void StartTimer(void);
static void SwitchFillOFF(void);
static void SwitchFillON(void);
static void SwitchOFFPilot(void);
static void WaitForPrefaultTimer(void);
static void WaitForTimer(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventDebounceInputNOK(void);
static void EventDebounceInputOK(void);
static void EventFillFaultReset(void);
static void EventNoMoreTrials(void);
static void EventSettingDataMissing(void);
static void EventStartDetection(void);
static void EventTimerElapseWithoutPulses(void);
static void EventTimerElapseWithPulses(void);
static void EventTimerExpired(void);
static void FillValveStuckONExecute(void);
static void FillValveStuckONInitialize(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void StartCountPulses(void)
{
    Fill_Stuck_ON_Pulse_Counter = Pulse_Count;
    Fill_Stuck_ON_Current_Debounce_Timer = Fill_Stuck_ON_Pulses_Detection_period;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SwitchOFFPilot(void)
{
  ON_OFF_TYPE state;
  state = OFF;
  (void)(Hbl__SetLoadRequestByLoadID(HBL_LOAD_PILOT_RELAY, HBL_PRIORITY_FILL_STUCK_ON_MONITOR, (uint8 *) &state));
  (void)(Hbl__SetLoadRequestByLoadID(HBL_LOAD_RELAY_ENABLE, HBL_PRIORITY_FILL_STUCK_ON_MONITOR, (uint8 *) &state));
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void WaitForTimer(void)
{
    if ((ON == Fill_Stuck_ON_Pilot_State) && (ON == Fill_Stuck_ON_Fill_Valve_State))
    {
        Fill_Stuck_ON_Current_Debounce_Timer = Fill_Stuck_ON_Debounce_Time;
    }
    else if ((uint16)0 == Fill_Stuck_ON_Current_Debounce_Timer)
    {
        EventDebounceInputOK();
    }
    else
    {
        // nothing to do
    }
    if (Fill_Stuck_ON_Current_Debounce_Timer > (uint16)0)
    {
        Fill_Stuck_ON_Current_Debounce_Timer--;
    }

}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module FillValveStuckOn and its variables
 *
 */
void FillValveStuckOn__Initialize(void)
{
    FillValveStuckONInitialize();
}

void FillValveStuckOn__Execution(void)
{
    uint16 lli_pulse_count;
    uint8* load_state_ptr;
    ON_OFF_TYPE reg_valve_state = OFF;

    load_state_ptr = (uint8*)Hbl__GetLoadStatusByLoadID(HBL_LOAD_PILOT_RELAY);
    if (load_state_ptr != NULL)
    {
        Fill_Stuck_ON_Pilot_State = (ON_OFF_TYPE)*load_state_ptr;
    }
    else
    {
        load_state_ptr = (uint8*)Hbl__GetLoadStatusByLoadID(HBL_LOAD_RELAY_ENABLE);
        if (load_state_ptr != NULL)
        {
            Fill_Stuck_ON_Pilot_State = (ON_OFF_TYPE)*load_state_ptr;
        }
        else
        {
            Fill_Stuck_ON_Pilot_State = ON;
        }
    }

    load_state_ptr = (uint8*)Hbl__GetLoadStatusByLoadID(HBL_LOAD_FILL_VALVE);
    if (load_state_ptr != NULL)
    {
        Fill_Stuck_ON_Fill_Valve_State = (ON_OFF_TYPE)*load_state_ptr;
    }

    load_state_ptr = (uint8*)Hbl__GetLoadStatusByLoadID(HBL_LOAD_REGENERATION_VALVE);
    if (load_state_ptr != NULL)
    {
        reg_valve_state = (ON_OFF_TYPE)*load_state_ptr;
    }

    lli_pulse_count = (*(uint16*)Hbl__GetGIDataByIndex(Hbl__GetGIIndexByGIID(HBL_GI_WATER_COUNTER)));

    if (((DishAuxVar__IsDrainPumpOn() == FALSE) && (reg_valve_state == OFF)) || (Fill_Stuck_ON_Fill_Valve_State == ON))
    {
        /* Pulses from Pulse Counter are "real" pulses and have to be counted */
        Pulse_Count += (lli_pulse_count - Pulse_Count_Old);
    }
    else
    {
        // If Drain Pump is On OR Regeneration valve is On AND Fill Valve is Off, we assume that any pulse detected is due to a magnetic
        // interference between the drain motor (or the regeneration valve) and the flow meter sensor, so it has not to be counted.
    }

    // Old reference is always aligned with the low level input value to guarantee that (lli_pulse_count - Pulse_Count_Old)
    // is always correct.
    Pulse_Count_Old = lli_pulse_count;

    FillValveStuckONExecute();
}



void FillValveStuckON__ExitFault(void)
{
    EventFillFaultReset();
}


BOOL_TYPE FillValveStuckOn__UnexpectedPulsesDetected(void)
{
	BOOL_TYPE retvalue;

	if ((Fill_Valve_Stuck_ON_State == STATE_FILL_VALVE_STUCK_ON_NONE) ||
		(Fill_Valve_Stuck_ON_State == STATE_FILL_VALVE_STUCK_ON_INIT) ||
		(Fill_Valve_Stuck_ON_State == STATE_FILL_VALVE_STUCK_ON_DEBOUNCE_INIT) ||
		(Fill_Valve_Stuck_ON_State == STATE_FILL_VALVE_STUCK_ON_COUNT_PILOT_ON))
	{
		retvalue = FALSE;
	}
	else
	{
		retvalue = TRUE;
	}

	return retvalue;
}
//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================




//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetFailureF1E13(void)
{
    Fault__ConditionDetected(FAULT_ID_FILL_VALVE_TRIAC_SHORTED);
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetF8E5Prefault(void)
{
    Fault__ConditionDetected(FAULT_ID_FILL_VALVE_STUCK_ON);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void FillValveStuckONInit(void)
{
    SETTINGFILE_LOADER_TYPE loader;

    uint8 water_counter_gi_index = Hbl__GetGIIndexByGIID(HBL_GI_WATER_COUNTER);
    if (water_counter_gi_index != (uint8)HBL_INDEX_INVALID)
    {
        if (SettingFile__BasicLoader(SF_PTR_ACU_PRODUCT_CONFIGURATION_DATA, DSPL_FILL_VALVE_FAULT_PARAMS, &loader) == PASS)
        {
            Fill_Stuck_ON_Debounce_Time = (uint16)10 * (uint16)(((FILL_VALVE_FAULTS_INSTANCE_TYPE *)(void *)loader.Data)->Fill_Valve_Debounce_Time);
            Fill_Stuck_ON_Pulses_Detection_period = (uint16)10 * (uint16)(((FILL_VALVE_FAULTS_INSTANCE_TYPE *)(void *)loader.Data)->Pulses_Detection_Period);
            Fill_Stuck_ON_Pulses_Number = ((FILL_VALVE_FAULTS_INSTANCE_TYPE *)(void *)loader.Data)->Pulses_Number;
            Fill_Valve_TON_Time = ((FILL_VALVE_FAULTS_INSTANCE_TYPE *)(void *)loader.Data)->Time_On;
            Fill_Valve_Fault_Max_Retry_Number = ((FILL_VALVE_FAULTS_INSTANCE_TYPE *)(void *)loader.Data)->Max_Num_Of_Retries;
            EventStartDetection();
        }
        else
        {
            EventSettingDataMissing();
            LOG_ADD_EXCEPTION (ERROR_LOADING_PARAMETERS, SF_DISPL_BASE_UNITS);
        }
    }
    else
    {
        EventSettingDataMissing();
        LOG_ADD_EXCEPTION (ERROR_LOADING_PARAMETERS, water_counter_gi_index);
    }
    Fill_Stuck_ON_Pulse_Counter = 0;
    Pulse_Count = 0;
    Pulse_Count_Old = 0;
    Fill_Stuck_ON_Pilot_State = OFF;
    Fill_Stuck_ON_Fill_Valve_State = OFF;

    Fault__ConditionRemoved(FAULT_ID_FILL_VALVE_TRIAC_SHORTED);
    Fault__ConditionRemoved(FAULT_ID_FILL_VALVE_STUCK_ON);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void CountPulses(void)
{
    //only one event at time
    if ((ON == Fill_Stuck_ON_Pilot_State) && (ON == Fill_Stuck_ON_Fill_Valve_State))
    {
        EventDebounceInputNOK();
    }
    else if ((uint16)0 == Fill_Stuck_ON_Current_Debounce_Timer)
    {
        // Casting (Pulse_Count - Fill_Stuck_ON_Pulse_Counter) to uin16 guarantees that the result is correct even when Pulse_Count overflows
        if ((uint16)(Pulse_Count - Fill_Stuck_ON_Pulse_Counter) > Fill_Stuck_ON_Pulses_Number)
        {
            if (STATE_FILL_VALVE_STUCK_ON_PREFAULT_FILL_OFF_COUNTING == Fill_Valve_Stuck_ON_State)
            {
                Fill_Valve_Fault_Retry_Counter++;
            }
            if (Fill_Valve_Fault_Retry_Counter >= Fill_Valve_Fault_Max_Retry_Number)
            {
                EventNoMoreTrials();
            }
            else
            {
                EventTimerElapseWithPulses();
            }
        }
        else
        {
            EventTimerElapseWithoutPulses();
        }
    }
    else
    {
        // nothing to do
    }
    if (Fill_Stuck_ON_Current_Debounce_Timer > (uint16)0)
    {
        Fill_Stuck_ON_Current_Debounce_Timer--;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void StartTimer(void)
{
    Fill_Stuck_ON_Current_Debounce_Timer = Fill_Stuck_ON_Debounce_Time;
    Fill_Valve_Fault_Retry_Counter = 0;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void ResetF8E5Prefault(void)
{
    Fault__ConditionRemoved(FAULT_ID_FILL_VALVE_STUCK_ON);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetFailureF8E5(void)
{
    Fault__ConditionDetected(FAULT_ID_FILL_VALVE_STUCK_ON);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void StartPrefaultTimerOFF(void)
{
    Fill_Stuck_ON_Current_Debounce_Timer = Fill_Stuck_ON_Debounce_Time;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void StartPrefaultTimerON(void)
{
    Fill_Stuck_ON_Current_Debounce_Timer = Fill_Valve_TON_Time;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SwitchFillOFF(void)
{
    ON_OFF_TYPE state;
    state = OFF;
    (void)(Hbl__SetLoadRequestByLoadID(HBL_LOAD_FILL_VALVE, HBL_PRIORITY_FILL_STUCK_ON_MONITOR, (uint8 *) &state));
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SwitchFillON(void)
{
    ON_OFF_TYPE state;
    state = ON;
    (void)(Hbl__SetLoadRequestByLoadID(HBL_LOAD_FILL_VALVE, HBL_PRIORITY_FILL_STUCK_ON_MONITOR, (uint8 *) &state));
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void WaitForPrefaultTimer(void)
{
    if ((uint16)0 == Fill_Stuck_ON_Current_Debounce_Timer)
    {
        EventTimerExpired();
    }

    if (Fill_Stuck_ON_Current_Debounce_Timer > (uint16)0)
    {
        Fill_Stuck_ON_Current_Debounce_Timer--;
    }
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Fill Valve Stuck ON State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Debounce Input NOK Event for the Fill Valve Stuck ON state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDebounceInputNOK(void)
{
    switch (Fill_Valve_Stuck_ON_State)
    {
        case STATE_FILL_VALVE_STUCK_ON_COUNT_PILOT_ON:
        case STATE_FILL_VALVE_STUCK_ON_COUNT_PILOT_OFF:
            Fill_Valve_Stuck_ON_State = STATE_FILL_VALVE_STUCK_ON_DEBOUNCE_INIT;
            StartTimer();                           // Entry Function.
            Fill_Valve_Stuck_ON_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Fill_Valve_Stuck_ON_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Debounce Input OK Event for the Fill Valve Stuck ON state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDebounceInputOK(void)
{
    switch (Fill_Valve_Stuck_ON_State)
    {
        case STATE_FILL_VALVE_STUCK_ON_DEBOUNCE_INIT:
            Fill_Valve_Stuck_ON_State = STATE_FILL_VALVE_STUCK_ON_COUNT_PILOT_ON;
            StartCountPulses();                     // Entry Function.
            Fill_Valve_Stuck_ON_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Fill_Valve_Stuck_ON_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Fill Fault Reset Event for the Fill Valve Stuck ON state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventFillFaultReset(void)
{
    switch (Fill_Valve_Stuck_ON_State)
    {
        case STATE_FILL_VALVE_STUCK_ON_SET_F1E13:
        case STATE_FILL_VALVE_STUCK_ON_SET_F8E5:
            Fill_Valve_Stuck_ON_State = STATE_FILL_VALVE_STUCK_ON_DEBOUNCE_INIT;
            StartTimer();                           // Entry Function.
            Fill_Valve_Stuck_ON_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Fill_Valve_Stuck_ON_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the No More Trials Event for the Fill Valve Stuck ON state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNoMoreTrials(void)
{
    switch (Fill_Valve_Stuck_ON_State)
    {
        case STATE_FILL_VALVE_STUCK_ON_PREFAULT_FILL_OFF_COUNTING:
            Fill_Valve_Stuck_ON_State = STATE_FILL_VALVE_STUCK_ON_SET_F8E5;
            SetFailureF8E5();                       // Entry Function.
            Fill_Valve_Stuck_ON_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Fill_Valve_Stuck_ON_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Setting Data Missing Event for the Fill Valve Stuck ON state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventSettingDataMissing(void)
{
    switch (Fill_Valve_Stuck_ON_State)
    {
        case STATE_FILL_VALVE_STUCK_ON_INIT:
            Fill_Valve_Stuck_ON_State = STATE_FILL_VALVE_STUCK_ON_NONE; // Terminate Fill Valve Stuck ON state machine execution.
            Fill_Valve_Stuck_ON_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Fill_Valve_Stuck_ON_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Start Detection Event for the Fill Valve Stuck ON state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventStartDetection(void)
{
    switch (Fill_Valve_Stuck_ON_State)
    {
        case STATE_FILL_VALVE_STUCK_ON_INIT:
            Fill_Valve_Stuck_ON_State = STATE_FILL_VALVE_STUCK_ON_DEBOUNCE_INIT;
            StartTimer();                           // Entry Function.
            Fill_Valve_Stuck_ON_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Fill_Valve_Stuck_ON_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Timer Elapse Without Pulses Event for the Fill Valve Stuck ON state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventTimerElapseWithoutPulses(void)
{
    switch (Fill_Valve_Stuck_ON_State)
    {
        case STATE_FILL_VALVE_STUCK_ON_COUNT_PILOT_ON:
            Fill_Valve_Stuck_ON_State = STATE_FILL_VALVE_STUCK_ON_COUNT_PILOT_ON;
            StartCountPulses();                     // Entry Function.
            Fill_Valve_Stuck_ON_Transitioned = TRUE;
            break;

        case STATE_FILL_VALVE_STUCK_ON_COUNT_PILOT_OFF:
            ResetF8E5Prefault();                    // Transition action function.
            Fill_Valve_Stuck_ON_State = STATE_FILL_VALVE_STUCK_ON_SET_F1E13;
            SetFailureF1E13();                      // Entry Function.
            Fill_Valve_Stuck_ON_Transitioned = TRUE;
            break;

        case STATE_FILL_VALVE_STUCK_ON_PREFAULT_FILL_OFF_COUNTING:
            ResetF8E5Prefault();                    // Transition action function.
            Fill_Valve_Stuck_ON_State = STATE_FILL_VALVE_STUCK_ON_DEBOUNCE_INIT;
            StartTimer();                           // Entry Function.
            Fill_Valve_Stuck_ON_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Fill_Valve_Stuck_ON_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Timer Elapse With Pulses Event for the Fill Valve Stuck ON state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventTimerElapseWithPulses(void)
{
    switch (Fill_Valve_Stuck_ON_State)
    {
        case STATE_FILL_VALVE_STUCK_ON_COUNT_PILOT_ON:
            SetF8E5Prefault();                      // Transition action function.
            Fill_Valve_Stuck_ON_State = STATE_FILL_VALVE_STUCK_ON_COUNT_PILOT_OFF;
            Fill_Valve_Stuck_ON_Transitioned = FALSE;
            StartCountPulses();                     // Entry Function.
            if (Fill_Valve_Stuck_ON_Transitioned == FALSE)
            {
                SwitchOFFPilot();                   // Entry Function.
            }
            Fill_Valve_Stuck_ON_Transitioned = TRUE;
            break;

        case STATE_FILL_VALVE_STUCK_ON_COUNT_PILOT_OFF:
        case STATE_FILL_VALVE_STUCK_ON_PREFAULT_FILL_OFF_COUNTING:
            Fill_Valve_Stuck_ON_State = STATE_FILL_VALVE_STUCK_ON_PREFAULT_FILL_ON;
            Fill_Valve_Stuck_ON_Transitioned = FALSE;
            SwitchFillON();                         // Entry Function.
            if (Fill_Valve_Stuck_ON_Transitioned == FALSE)
            {
                StartPrefaultTimerON();             // Entry Function.
            }
            Fill_Valve_Stuck_ON_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Fill_Valve_Stuck_ON_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Timer Expired Event for the Fill Valve Stuck ON state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventTimerExpired(void)
{
    switch (Fill_Valve_Stuck_ON_State)
    {
        case STATE_FILL_VALVE_STUCK_ON_PREFAULT_FILL_ON:
            Fill_Valve_Stuck_ON_State = STATE_FILL_VALVE_STUCK_ON_PREFAULT_FILL_OFF;
            Fill_Valve_Stuck_ON_Transitioned = FALSE;
            SwitchFillOFF();                        // Entry Function.
            if (Fill_Valve_Stuck_ON_Transitioned == FALSE)
            {
                StartPrefaultTimerOFF();            // Entry Function.
            }
            Fill_Valve_Stuck_ON_Transitioned = TRUE;
            break;

        case STATE_FILL_VALVE_STUCK_ON_PREFAULT_FILL_OFF:
            Fill_Valve_Stuck_ON_State = STATE_FILL_VALVE_STUCK_ON_PREFAULT_FILL_OFF_COUNTING;
            Fill_Valve_Stuck_ON_Transitioned = FALSE;
            SwitchFillOFF();                        // Entry Function.
            if (Fill_Valve_Stuck_ON_Transitioned == FALSE)
            {
                StartCountPulses();                 // Entry Function.
            }
            Fill_Valve_Stuck_ON_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Fill_Valve_Stuck_ON_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Fill Valve Stuck ON state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void FillValveStuckONExecute(void)
{
    Fill_Valve_Stuck_ON_Transitioned = FALSE;
    switch(Fill_Valve_Stuck_ON_State)
    {
        case STATE_FILL_VALVE_STUCK_ON_DEBOUNCE_INIT:
            WaitForTimer();                         // Do function.
            break;

        case STATE_FILL_VALVE_STUCK_ON_COUNT_PILOT_ON:
            CountPulses();                          // Do function.
            break;

        case STATE_FILL_VALVE_STUCK_ON_COUNT_PILOT_OFF:
            CountPulses();                          // Do function.
            if (Fill_Valve_Stuck_ON_Transitioned == FALSE)
            {
                SwitchOFFPilot();                   // Do function.
            }
            break;

        case STATE_FILL_VALVE_STUCK_ON_PREFAULT_FILL_ON:
            SwitchFillON();                         // Do function.
            if (Fill_Valve_Stuck_ON_Transitioned == FALSE)
            {
                WaitForPrefaultTimer();             // Do function.
            }
            break;

        case STATE_FILL_VALVE_STUCK_ON_PREFAULT_FILL_OFF:
            SwitchFillOFF();                        // Do function.
            if (Fill_Valve_Stuck_ON_Transitioned == FALSE)
            {
                WaitForPrefaultTimer();             // Do function.
            }
            break;

        case STATE_FILL_VALVE_STUCK_ON_PREFAULT_FILL_OFF_COUNTING:
            SwitchFillOFF();                        // Do function.
            if (Fill_Valve_Stuck_ON_Transitioned == FALSE)
            {
                CountPulses();                      // Do function.
            }
            break;

        default:
            // There is nothing to do for all other values of Fill_Valve_Stuck_ON_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Fill Valve Stuck ON state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void FillValveStuckONInitialize(void)
{
    Fill_Valve_Stuck_ON_State = STATE_FILL_VALVE_STUCK_ON_INIT;
    FillValveStuckONInit();                         // Entry Function.
    Fill_Valve_Stuck_ON_Transitioned = TRUE;
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.



/*
 * DCLoadCurrentMonitor.c
 *
 *  Created on: Jul 7, 2023
 *      Author: WAZNYP
 */
/**
 *  @file
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *
   @startuml{DCLoadCurrentMonitor State Machine.jpg}
        title DC Load Current Monitor State Machine

        [*] --> INITIALIZE

        INITIALIZE : entry / DCLoadCurrentInitializeModule()
        INITIALIZE --> NO_FAILURE  : EventDCLoadCurrentStartMonitor
        INITIALIZE --> [*]         : EventDCLoadCurrentSettingDataMissing

        NO_FAILURE : entry / DCLoadCurrentResetFailureDetection()
        NO_FAILURE : do /\t DCCheckForFailure()
        NO_FAILURE --> FAULT_F9E4_DETECTED : EventF9E4FailureDetected
        NO_FAILURE --> FAULT_F10E3_DETECTED : EventF10E3FailureDetected
        NO_FAILURE --> FAULT_F5E3_DETECTED : EventF5E3FailureDetected

        FAULT_F9E4_DETECTED : entry /\t SetFailureF9E4Detection()
        FAULT_F9E4_DETECTED : do /\t DCCheckFailureRemoval()
        FAULT_F9E4_DETECTED --> NO_FAILURE       : EventNoFailureF9E4Present
        FAULT_F9E4_DETECTED --> NO_FAILURE       : EventFaultReset
        FAULT_F9E4_DETECTED --> FAULT_F9E4       : EventDebounceCompletedF9E4

        FAULT_F10E3_DETECTED : entry /\t SetFailureF10E3Detection()
        FAULT_F10E3_DETECTED : do /\t DCCheckFailureRemoval()
        FAULT_F10E3_DETECTED --> NO_FAILURE       : EventNoFailureF10E3Present
        FAULT_F10E3_DETECTED --> NO_FAILURE       : EventFaultReset
        FAULT_F10E3_DETECTED --> FAULT_F10E3      : EventDebounceCompletedF10E3

        FAULT_F5E3_DETECTED : entry /\t SetFailureF5E3Detection()
        FAULT_F5E3_DETECTED : do /\t DCCheckFailureRemoval()
        FAULT_F5E3_DETECTED --> NO_FAILURE       : EventNoFailureF5E3Present
        FAULT_F5E3_DETECTED --> NO_FAILURE       : EventFaultReset
        FAULT_F5E3_DETECTED --> FAULT_F5E3       : EventDebounceCompletedF5E3

        FAULT_F9E4 : do /\t DCCheckForFailure()
        FAULT_F9E4 --> NO_FAILURE : EventFaultReset
        FAULT_F9E4 --> FAULT_F10E3_DETECTED  : EventF10E3FailureDetected
        FAULT_F9E4 --> FAULT_F5E3_DETECTED  : EventF5E3FailureDetected

        FAULT_F10E3 : do /\t DCCheckForFailure()
        FAULT_F10E3 --> NO_FAILURE : EventFaultReset
        FAULT_F10E3 --> FAULT_F9E4_DETECTED  : EventF9E4FailureDetected
        FAULT_F10E3 --> FAULT_F5E3_DETECTED  : EventF5E3FailureDetected

        FAULT_F5E3 : do /\t DCCheckForFailure()
        FAULT_F5E3 --> NO_FAILURE : EventFaultReset
        FAULT_F5E3 --> FAULT_F9E4_DETECTED  : EventF9E4FailureDetected
        FAULT_F5E3 --> FAULT_F10E3_DETECTED  : EventF10E3FailureDetected

    @enduml

 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "DCLoadCurrentMonitor.h"
#include "DCLoadCurrentMonitor_prv.h"

// -- Other Modules --
#include "Fault.h"
#include "Hbl.h"
#include "LITTMonitor.h"

#include "Log.h"
#include "Micro.h"

static ON_OFF_TYPE Next_LITT_Status;
static ON_OFF_TYPE DC_Load_LITT_Status;
static ON_OFF_TYPE Next_DC_Fan_Status;
static ON_OFF_TYPE DC_Load_DC_Fan_Status;
static ON_OFF_TYPE Next_DOS_Status;
static ON_OFF_TYPE DC_Load_DOS_Status;

//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for DC Load Current Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the DC Load Current Monitor state machine.
typedef enum DC_LOAD_CURRENT_MONITOR_STATE_ENUM
{
    STATE_DC_LOAD_CURRENT_MONITOR_NONE                  = 0,

    STATE_DC_LOAD_CURRENT_MONITOR_INITIALIZE            = 1, //!< (initial_state)
    STATE_DC_LOAD_CURRENT_MONITOR_NO_FAILURE            = 2,
    STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F9_E4_DETECTED  = 3,
    STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F10_E3_DETECTED = 4,
    STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F5_E3_DETECTED  = 5,
    STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F9E4            = 6,
    STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F10E3           = 7,
    STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F5E3            = 8,

    STATE_DC_LOAD_CURRENT_MONITOR_END                   = 9
} DC_LOAD_CURRENT_MONITOR_STATE_TYPE;

//! The state variable for the DC Load Current Monitor state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static DC_LOAD_CURRENT_MONITOR_STATE_TYPE DC_Load_Current_Monitor_State;

// -- Private Function Prototypes used by the state machine for customization functions --
static void DCCheckFailureRemoval(void);
static void DCCheckForFailure(void);
static void DCLoadCurrentInitializeModule(void);
static void DCLoadCurrentResetFailureDetection(void);
static void SetFailureF10E3Detection(void);
static void SetFailureF5E3Detection(void);
static void SetFailureF9E4Detection(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void DCLoadCurrentMonitorExecute(void);
static void DCLoadCurrentMonitorInitialize(void);
static void EventDCLoadCurrentSettingDataMissing(void);
static void EventDCLoadCurrentStartMonitor(void);
static void EventDebounceCompletedF10E3(void);
static void EventDebounceCompletedF5E3(void);
static void EventDebounceCompletedF9E4(void);
static void EventF10E3FailureDetected(void);
static void EventF5E3FailureDetected(void);
static void EventF9E4FailureDetected(void);
static void EventFaultReset(void);
static void EventNoFailureF10E3Present(void);
static void EventNoFailureF5E3Present(void);
static void EventNoFailureF9E4Present(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define MODULE_NAME                                   MODULE_DC_LOAD_MONITOR
#define DC_LOAD_DEBOUNCE_100MS                        5

typedef enum MODULE_DC_LOAD_MONITOR_LOG_MESSAGE_ID_ENUM
{
    ERROR_MISSING_COMPONENTS  = 1,
    ERROR_MISSING_PARAMETERS,
} MODULE_DC_LOAD_MONITOR_LOG_MESSAGE_ID_TYPE;


static LITT_FAULT_PARAMS_TYPE LITT_Fault_Params;
static DC_FAN_FAULT_PARAMS_TYPE DC_Fan_Fault_Params;
static DOS_FAULT_PARAMS_TYPE DOS_Fault_Params;

static BOOL_TYPE DCLoadStatusPending;  // load toggled but diag status not checked
static uint8 DCLoad_Ref_Timer;
static uint8 DCLoad_Debounce_Timer;

#if (DCLOAD_DIAG_DEBUG == ENABLED)
static uint16 Debug_Current_Input_DCLoad;
#endif

static uint16 DC_Load_Current;

static DC_LOAD_DIAGNOSTIC_TYPE DC_Load_Diagnostic;
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

static DC_LOAD_DIAGNOSTIC_STATUS_TYPE DCLoadDiagnosticGetStatus(HBL_LOAD_ENUM_TYPE load_id);
static void DCLoadCurrentHandler(void);
static void DCLoadCurrentUpdateRange(HBL_LOAD_ENUM_TYPE output, ON_OFF_TYPE new_digital_output_state);
static void DCLoadSetRef(void);
static void DCLoadCheckIfInRange(void);

//----------------------------------------------------------------------------
/**
 *  @brief      DCLoadCurrentMonitor__Initialize
 *  @details    Function used to initialize the DC Load Current Monitor.
 *
 */
void DCLoadCurrentMonitor__Initialize(void)
{
    DCLoadCurrentMonitorInitialize();

    DCLoad_Debounce_Timer = DC_LOAD_DEBOUNCE_100MS;
    DCLoad_Ref_Timer = DC_LOAD_DEBOUNCE_100MS;
    Next_LITT_Status = OFF;
    DC_Load_LITT_Status = OFF;
    Next_DC_Fan_Status = OFF;
    DC_Load_DC_Fan_Status = OFF;
    Next_DOS_Status = OFF;
    DC_Load_DOS_Status = OFF;

    DCLoadStatusPending = FALSE;
}


//----------------------------------------------------------------------------
/**
 *  @brief      DCLoadCurrentMonitor__Execution
 *  @details    Function used to perform the DC Load Current Monitor.
 *
 */
void DCLoadCurrentMonitor__Execution(void)
{
    // sequence is important
    DCLoadCurrentHandler();
    DCLoadCheckIfInRange();
    DCLoadCurrentMonitorExecute();
}


//----------------------------------------------------------------------------
/**
 *  @brief      DCLoadCurrentMonitor__ExitFault
 *  @details    Function to be called when the Faults have to be removed.
 *
 */
void DCLoadCurrentMonitor__ExitFault(void)
{
    EventFaultReset();
}

//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief returns lastly calculated diagnostic status for load_id, but ONLY if this load was activated most recently
 */
static DC_LOAD_DIAGNOSTIC_STATUS_TYPE DCLoadDiagnosticGetStatus(HBL_LOAD_ENUM_TYPE load_id)
{
    DC_LOAD_DIAGNOSTIC_STATUS_TYPE DC_Load_Diagnostic_status;

    DC_Load_Diagnostic_status = DC_LOAD_DIAGNOSTIC_UNKNOWN;

    switch ( load_id )
    {
        case HBL_LOAD_LITT:
        case HBL_LOAD_DRY_FAN:
        case HBL_LOAD_DOOR_OPENING_SYSTEM:
            if ( load_id == DC_Load_Diagnostic.Last_Changed_DC_Load_Output )
            {
                DC_Load_Diagnostic_status = DC_Load_Diagnostic.Load_Sense_Status;
            }
            break;
        default:
            // nothing to do
            break;
    }

    return DC_Load_Diagnostic_status;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Handler responsible for monitoring observed loads states, in case of activation requests current range update, Timers handling
 */
static void DCLoadCurrentHandler(void)
{
    uint8 *litt_state_ptr;
    uint8 *load_status_ptr;
    uint8 litt_load_index;

    litt_load_index = Hbl__GetLoadIndexByLoadID(HBL_LOAD_LITT);

	litt_state_ptr = (uint8*)Hbl__GetProcessedLoad(litt_load_index);
    // If LITT pilot type == STEADY_PWM_PROFILE, LITT load status is not compared to HBL_LOAD_STATUS_TARGET_COMPLETED deliberately,
	// ON OFF requests events needs fast reaction before actual current change. If LITT pilot type == DIGITAL, Hbl__GetProcessedLoad()
	// returns the same result Hbl__GetLoadStatusByIndex().
	if (litt_state_ptr != NULL)
	{
	    HBL_PILOT_ENUM_TYPE litt_pilot_type = Hbl__GetPilotIDByIndex(litt_load_index);

	    if (litt_pilot_type == HBL_PILOT_STEADY_PWM_PROFILE)
	    {
        	Next_LITT_Status = (litt_state_ptr[0] == (uint8)LITT_ON) ? ON:OFF;
	    }
	    else
	    {
        	Next_LITT_Status = (litt_state_ptr[0] == (uint8)ON) ? ON:OFF;
	    }
	}
    if (Next_LITT_Status != DC_Load_LITT_Status)
    {
        DC_Load_LITT_Status = Next_LITT_Status;
        DCLoadCurrentUpdateRange(HBL_LOAD_LITT, Next_LITT_Status);
    }

    load_status_ptr = (uint8*)(Hbl__GetLoadStatusByLoadID(HBL_LOAD_DRY_FAN));
    if (load_status_ptr != NULL)
    {
        Next_DC_Fan_Status = (load_status_ptr[0] == (uint8)ON) ? ON:OFF;
    }
    if (Next_DC_Fan_Status != DC_Load_DC_Fan_Status)
    {
        DC_Load_DC_Fan_Status = Next_DC_Fan_Status;
        DCLoadCurrentUpdateRange(HBL_LOAD_DRY_FAN, Next_DC_Fan_Status);
    }

    load_status_ptr = (uint8*)(Hbl__GetLoadStatusByLoadID(HBL_LOAD_DOOR_OPENING_SYSTEM));
    if (load_status_ptr != NULL)
    {
        Next_DOS_Status = (load_status_ptr[0] == (uint8)ON) ? ON:OFF;
    }
    if (Next_DOS_Status != DC_Load_DOS_Status)
    {
        DC_Load_DOS_Status = Next_DOS_Status;
        DCLoadCurrentUpdateRange(HBL_LOAD_DOOR_OPENING_SYSTEM, Next_DOS_Status);
    }

    if (DCLoad_Ref_Timer > (uint8)0)
    {
        DCLoad_Ref_Timer--;
    }
    if (DCLoad_Debounce_Timer > (uint8)0)
    {
        DCLoad_Debounce_Timer--;
    }

}



//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief based on information which and how load has changed sets allowed current range (min, max)
 */
static void DCLoadCurrentUpdateRange(HBL_LOAD_ENUM_TYPE output, ON_OFF_TYPE new_digital_output_state)
{
    uint16 diag_signal_min = 0;
    uint16 diag_signal_max = 0;

    if (ON == new_digital_output_state)
    {
        DCLoadStatusPending = TRUE;
        DC_Load_Diagnostic.Last_Changed_DC_Load_Output = output;
    }
    else
    {
        DCLoadStatusPending = TRUE;
        DC_Load_Diagnostic.Last_Changed_DC_Load_Output = HBL_LOAD_INVALID;
    }

    if ((uint8)0 == DCLoad_Ref_Timer)
    {
        // Allow update DC Load reference after reference timer is completed
        DCLoadSetRef();
    }
    else if (FALSE != DC_Load_Diagnostic.DCLoad_Channel_Freeze )
    {
        //When output is changed too fast or in the same time
        //than diagnostic measure is disabled for this outputs - set measure values to 0
        DC_Load_Diagnostic.Max_Expected_Value = 0;
        DC_Load_Diagnostic.Min_Expected_Value = 0;
    }

    if (HBL_LOAD_LITT == output)
    {
        DCLoad_Ref_Timer = 20; // 2 sec is enough for LITT PWM load from 0 to 100%
    }
    else
    {
        DCLoad_Ref_Timer = DC_LOAD_DEBOUNCE_100MS;
    }

    //Reset last diagnostic measure result
    DC_Load_Diagnostic.Load_Sense_Status = DC_LOAD_DIAGNOSTIC_UNKNOWN;

    if (output != HBL_LOAD_INVALID)
    {
        if (HBL_LOAD_LITT == output)
        {
            diag_signal_min = LITT_Fault_Params.Current_Sense_LITT_Min;
            diag_signal_max = LITT_Fault_Params.Current_Sense_LITT_Max;
        }
        else if (HBL_LOAD_DRY_FAN == output)
        {
            diag_signal_min = DC_Fan_Fault_Params.Current_Sense_DC_Fan_Min;
            diag_signal_max = DC_Fan_Fault_Params.Current_Sense_DC_Fan_Max;
        }
        else if (HBL_LOAD_DOOR_OPENING_SYSTEM == output)
        {
            diag_signal_min = DOS_Fault_Params.Current_Sense_DOS_Min;
            diag_signal_max = DOS_Fault_Params.Current_Sense_DOS_Max;
        }
        else
        {
            diag_signal_min = 0;
            diag_signal_max = 0;
        }

        //Set range value for new digital output
        if ( ON == new_digital_output_state )
        {
            if ( FALSE == DC_Load_Diagnostic.DCLoad_Channel_Freeze )
            {
                if ((DC_Load_Diagnostic.Max_Expected_Value + (uint32)diag_signal_max) < (uint32)0xFFFF)
                {
                    DC_Load_Diagnostic.Max_Expected_Value += diag_signal_max;
                }
                else
                {
                    DC_Load_Diagnostic.Max_Expected_Value = 0xFFFF;
                }
                if ((DC_Load_Diagnostic.Min_Expected_Value + (uint32)diag_signal_min) < (uint32)0xFFFF)
                {
                    DC_Load_Diagnostic.Min_Expected_Value += diag_signal_min;
                }
                else
                {
                    DC_Load_Diagnostic.Min_Expected_Value = 0xFFFF;
                }
                DC_Load_Diagnostic.DCLoad_Channel_Freeze = TRUE;
            }
        }
        else
        {
            if ( FALSE == DC_Load_Diagnostic.DCLoad_Channel_Freeze )
            {
                if ( DC_Load_Diagnostic.Max_Expected_Value > diag_signal_min )
                {
                    //Set range for relative channel value
                    DC_Load_Diagnostic.Max_Expected_Value -= diag_signal_min;
                }
                else
                {
                    // Disable check when reference is incorrect
                    DC_Load_Diagnostic.Max_Expected_Value = 0;
                }
                if ( DC_Load_Diagnostic.Min_Expected_Value > diag_signal_max )
                {
                    DC_Load_Diagnostic.Min_Expected_Value -= diag_signal_max;
                }
                else
                {
                    // Disable check when reference is incorrect
                    DC_Load_Diagnostic.Min_Expected_Value = 0;
                }
                DC_Load_Diagnostic.DCLoad_Channel_Freeze = TRUE;
            }
        }
    }
    if ( (uint32)0 == diag_signal_min ) // When diag_signal_min is 0 diagnostic is disabled
    {
        DC_Load_Diagnostic.Max_Expected_Value = 0;
        DC_Load_Diagnostic.Min_Expected_Value = 0;
    }
    DCLoad_Debounce_Timer = DC_LOAD_DEBOUNCE_100MS;
}



//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief load changed when current is stable - time to remember it and set current range reference
 */
static void DCLoadSetRef(void)
{
    // set baseline according to readout before output change happen
    DC_Load_Diagnostic.DCLoad_Channel_Freeze = FALSE;

    DC_Load_Diagnostic.Max_Expected_Value = DC_Load_Current;   // measured current;
    DC_Load_Diagnostic.Min_Expected_Value = DC_Load_Current;   // measured current;

}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  reads current, check against allowed range and do basic diagnostic status filtering
 */
static void DCLoadCheckIfInRange(void)
{
    uint8 dc_current_gi_index;
    uint8 dc_current_sequence_id;
    uint16 measured_current;

#if (DCLOAD_DIAG_DEBUG == ENABLED)
    DC_Load_Current = Debug_Current_Input_DCLoad;
#else
    dc_current_gi_index = Hbl__GetGIIndexByGIID(HBL_GI_I_DC_FAN);
    dc_current_sequence_id = Hbl__GetGISequenceIDByIndex(dc_current_gi_index);
    if (dc_current_sequence_id > 0U)
    {
        DC_Load_Current = (uint16)Hbl__GetGIDataConvertedByIndex(dc_current_gi_index);
#endif

        measured_current = DC_Load_Current;

        // First check if max and min expected values are set
        if ( DC_Load_Diagnostic.Max_Expected_Value > DC_Load_Diagnostic.Min_Expected_Value)
        {
            // Then check if filtered value is in range
            if ( (measured_current >= DC_Load_Diagnostic.Min_Expected_Value) &&
                 (measured_current <= DC_Load_Diagnostic.Max_Expected_Value) )
            {
                if( (DC_LOAD_DIAGNOSTIC_UNKNOWN == DC_Load_Diagnostic.Temporary_Load_Sense_Status) || (DC_LOAD_DIAGNOSTIC_NOT_IN_RANGE == DC_Load_Diagnostic.Temporary_Load_Sense_Status))
                {
                    DCLoad_Debounce_Timer = DC_LOAD_DEBOUNCE_100MS;
                    DC_Load_Diagnostic.Load_Sense_Status = DC_LOAD_DIAGNOSTIC_UNKNOWN;
                    DC_Load_Diagnostic.Temporary_Load_Sense_Status = DC_LOAD_DIAGNOSTIC_IN_RANGE;
                }
            }
            else
            {
                if( (DC_LOAD_DIAGNOSTIC_UNKNOWN == DC_Load_Diagnostic.Temporary_Load_Sense_Status) || (DC_LOAD_DIAGNOSTIC_IN_RANGE == DC_Load_Diagnostic.Temporary_Load_Sense_Status))
                {
                    DCLoad_Debounce_Timer = DC_LOAD_DEBOUNCE_100MS;
                    DC_Load_Diagnostic.Load_Sense_Status = DC_LOAD_DIAGNOSTIC_UNKNOWN;
                    DC_Load_Diagnostic.Temporary_Load_Sense_Status = DC_LOAD_DIAGNOSTIC_NOT_IN_RANGE;
                }
            }

            if (((uint8)0 == DCLoad_Debounce_Timer) && ((uint8)0 == DCLoad_Ref_Timer) && (DC_Load_Diagnostic.Load_Sense_Status != DC_Load_Diagnostic.Temporary_Load_Sense_Status))
            {
                DC_Load_Diagnostic.Load_Sense_Status = DC_Load_Diagnostic.Temporary_Load_Sense_Status;
            }
        }

#if (DCLOAD_DIAG_DEBUG != ENABLED)
    }
#endif

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void DCCheckFailureRemoval(void)
{
    switch(DC_Load_Current_Monitor_State)
    {
        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F9_E4_DETECTED:
            if (Fault__IsInFaultState(FAULT_ID_LITT_FAILURE) != FALSE)
            {
                EventDebounceCompletedF9E4();
            }
            else if ((Fault__IsObserverActive(FAULT_ID_LITT_FAILURE) == FALSE)
                  || (DC_LOAD_DIAGNOSTIC_IN_RANGE == DCLoadDiagnosticGetStatus(HBL_LOAD_LITT)))
            {
                EventNoFailureF9E4Present();
            }
            else
            {
                // nothing to do
            }
            break;
        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F10_E3_DETECTED:
            if (Fault__IsInFaultState(FAULT_ID_FAN_MOTOR_FAILURE) != FALSE)
            {
                EventDebounceCompletedF10E3();
            }
            else if ((Fault__IsObserverActive(FAULT_ID_FAN_MOTOR_FAILURE) == FALSE)
                  || (DC_LOAD_DIAGNOSTIC_IN_RANGE == DCLoadDiagnosticGetStatus(HBL_LOAD_DRY_FAN)))
            {
                EventNoFailureF10E3Present();
            }
            else
            {
                // nothing to do
            }
            break;
        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F5_E3_DETECTED:
#if (DCLOAD_CURRENT_MONITOR_DOS_FAULT_FEATURE == ENABLED)
            if (Fault__IsInFaultState(FAULT_ID_DOS_FAILURE) != FALSE)
            {
            	EventDebounceCompletedF5E3();
            }
            else if ((Fault__IsObserverActive(FAULT_ID_DOS_FAILURE) == FALSE)
                 || (DC_LOAD_DIAGNOSTIC_IN_RANGE == DCLoadDiagnosticGetStatus(HBL_LOAD_DOOR_OPENING_SYSTEM)))
            {
            	EventNoFailureF5E3Present();
            }
            else
            {
                // nothing to do
            }
#else
            EventNoFailureF5E3Present();
#endif
            break;

        default:
            // There is nothing to do for all other values of DC_Current_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void DCCheckForFailure(void)
{
    if (((uint8)0 == DCLoad_Debounce_Timer) && ((uint8)0 == DCLoad_Ref_Timer) && (FALSE != DCLoadStatusPending))
    {
        if (DC_LOAD_DIAGNOSTIC_NOT_IN_RANGE == DCLoadDiagnosticGetStatus(HBL_LOAD_LITT))
        {
            EventF9E4FailureDetected();
        }
        else if (DC_LOAD_DIAGNOSTIC_NOT_IN_RANGE == DCLoadDiagnosticGetStatus(HBL_LOAD_DRY_FAN))
        {
            EventF10E3FailureDetected();
        }
        else if (DC_LOAD_DIAGNOSTIC_NOT_IN_RANGE == DCLoadDiagnosticGetStatus(HBL_LOAD_DOOR_OPENING_SYSTEM))
        {
        	EventF5E3FailureDetected();
        }
        else
        {
            //nothing to do
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void DCLoadCurrentInitializeModule(void)
{
    uint8 dc_load_gi_index;

    uint8 litt_index;
    uint8 dc_fan_index;
    uint8 dos_index;

    LITT_FAULT_PARAMS_TYPE *litt_fault_params;
    DC_FAN_FAULT_PARAMS_TYPE *dc_fan_fault_Params;
#if (DCLOAD_CURRENT_MONITOR_DOS_FAULT_FEATURE == ENABLED)
    DOS_FAULT_PARAMS_TYPE *dos_fault_params;
#endif

    dc_load_gi_index = Hbl__GetGIIndexByGIID(HBL_GI_I_DC_FAN);

    litt_index = Hbl__GetLoadIndexByLoadID(HBL_LOAD_LITT);
    dc_fan_index = Hbl__GetLoadIndexByLoadID(HBL_LOAD_DRY_FAN);
    dos_index = Hbl__GetLoadIndexByLoadID(HBL_LOAD_DOOR_OPENING_SYSTEM);

    if (dc_load_gi_index == (uint8)HBL_INDEX_INVALID)
    {
        EventDCLoadCurrentSettingDataMissing();
        LOG_ADD_EXCEPTION (ERROR_MISSING_COMPONENTS, 0);
    }
    else
    {
        if (litt_index != (uint8)HBL_INDEX_INVALID)
        {
            litt_fault_params = (LITT_FAULT_PARAMS_TYPE*)Fault__GetSpecificData(FAULT_ID_LITT_FAILURE);
            if (litt_fault_params != NULL)
            {
                LITT_Fault_Params.Current_Sense_LITT_Min = ENDIANU16_STON(litt_fault_params->Current_Sense_LITT_Min);
                LITT_Fault_Params.Current_Sense_LITT_Max = ENDIANU16_STON(litt_fault_params->Current_Sense_LITT_Max);
            }
            else
            {
                LITT_Fault_Params.Current_Sense_LITT_Min = 0;
                LITT_Fault_Params.Current_Sense_LITT_Max = 0;
                LOG_ADD_EXCEPTION (ERROR_MISSING_PARAMETERS, FAULT_ID_LITT_FAILURE);
            }
        }
        if (dc_fan_index != (uint8)HBL_INDEX_INVALID)
        {
            dc_fan_fault_Params = (DC_FAN_FAULT_PARAMS_TYPE*)Fault__GetSpecificData(FAULT_ID_FAN_MOTOR_FAILURE);
            if (dc_fan_fault_Params != NULL)
            {
                DC_Fan_Fault_Params.Current_Sense_DC_Fan_Min = ENDIANU16_STON(dc_fan_fault_Params->Current_Sense_DC_Fan_Min);
                DC_Fan_Fault_Params.Current_Sense_DC_Fan_Max = ENDIANU16_STON(dc_fan_fault_Params->Current_Sense_DC_Fan_Max);
            }
            else
            {
                DC_Fan_Fault_Params.Current_Sense_DC_Fan_Min = 0;
                DC_Fan_Fault_Params.Current_Sense_DC_Fan_Max = 0;
                LOG_ADD_EXCEPTION (ERROR_MISSING_PARAMETERS, FAULT_ID_FAN_MOTOR_FAILURE);
            }
        }
        if (dos_index != (uint8)HBL_INDEX_INVALID)
        {
#if (DCLOAD_CURRENT_MONITOR_DOS_FAULT_FEATURE == ENABLED)
            dos_fault_params = (DOS_FAULT_PARAMS_TYPE*)Fault__GetSpecificData(FAULT_ID_DOS_FAILURE);
            if (dos_fault_params != NULL)
            {
                DOS_Fault_Params.Current_Sense_DOS_Min = ENDIANU16_STON(dos_fault_params->Current_Sense_DOS_Min);
                DOS_Fault_Params.Current_Sense_DOS_Max = ENDIANU16_STON(dos_fault_params->Current_Sense_DOS_Max);
            }
            else
            {
                DOS_Fault_Params.Current_Sense_DOS_Min = 0;
                DOS_Fault_Params.Current_Sense_DOS_Max = 0;
                LOG_ADD_EXCEPTION (ERROR_MISSING_PARAMETERS, FAULT_ID_DOS_FAILURE);
            }
#else
            DOS_Fault_Params.Current_Sense_DOS_Min = 0;
            DOS_Fault_Params.Current_Sense_DOS_Max = 0;
#endif
        }
        EventDCLoadCurrentStartMonitor();
    }

    DC_Load_Diagnostic.Max_Expected_Value = 0;
    DC_Load_Diagnostic.Min_Expected_Value = 0;
    DC_Load_Diagnostic.Temporary_Load_Sense_Status = DC_LOAD_DIAGNOSTIC_UNKNOWN;
    DC_Load_Diagnostic.Load_Sense_Status = DC_LOAD_DIAGNOSTIC_UNKNOWN;
    DC_Load_Diagnostic.DCLoad_Channel_Freeze = FALSE;
    DC_Load_Diagnostic.Last_Changed_DC_Load_Output = HBL_LOAD_INVALID;
#if (DCLOAD_DIAG_DEBUG == ENABLED)
    Debug_Current_Input_DCLoad = 0;
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void DCLoadCurrentResetFailureDetection(void)
{
    Fault__ConditionRemoved(FAULT_ID_LITT_FAILURE);
    Fault__ConditionRemoved(FAULT_ID_FAN_MOTOR_FAILURE);
#if (DCLOAD_CURRENT_MONITOR_DOS_FAULT_FEATURE == ENABLED)
    Fault__ConditionRemoved(FAULT_ID_DOS_FAILURE);
#endif
    DCLoadStatusPending = FALSE;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetFailureF10E3Detection(void)
{
    Fault__ConditionDetected(FAULT_ID_FAN_MOTOR_FAILURE);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetFailureF9E4Detection(void)
{
    Fault__ConditionDetected(FAULT_ID_LITT_FAILURE);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetFailureF5E3Detection(void)
{
#if (DCLOAD_CURRENT_MONITOR_DOS_FAULT_FEATURE == ENABLED)
    Fault__ConditionDetected(FAULT_ID_DOS_FAILURE);
#endif
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for DC Load Current Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the DC Load Current Monitor state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void DCLoadCurrentMonitorExecute(void)
{
    switch(DC_Load_Current_Monitor_State)
    {
        case STATE_DC_LOAD_CURRENT_MONITOR_NO_FAILURE:
        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F9E4:
        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F10E3:
        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F5E3:
            DCCheckForFailure();                    // Do function.
            break;

        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F9_E4_DETECTED:
        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F10_E3_DETECTED:
        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F5_E3_DETECTED:
            DCCheckFailureRemoval();                // Do function.
            break;

        default:
            // There is nothing to do for all other values of DC_Load_Current_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the DC Load Current Monitor state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void DCLoadCurrentMonitorInitialize(void)
{
    DC_Load_Current_Monitor_State = STATE_DC_LOAD_CURRENT_MONITOR_INITIALIZE;
    DCLoadCurrentInitializeModule();                // Entry Function.
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the DC Load Current Setting Data Missing Event for the DC Load Current Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDCLoadCurrentSettingDataMissing(void)
{
    switch (DC_Load_Current_Monitor_State)
    {
        case STATE_DC_LOAD_CURRENT_MONITOR_INITIALIZE:
            DC_Load_Current_Monitor_State = STATE_DC_LOAD_CURRENT_MONITOR_NONE; // Terminate DC Load Current Monitor state machine execution.
            break;

        default:
            // Event is ignored for all other values of DC_Load_Current_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the DC Load Current Start Monitor Event for the DC Load Current Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDCLoadCurrentStartMonitor(void)
{
    switch (DC_Load_Current_Monitor_State)
    {
        case STATE_DC_LOAD_CURRENT_MONITOR_INITIALIZE:
            DC_Load_Current_Monitor_State = STATE_DC_LOAD_CURRENT_MONITOR_NO_FAILURE;
            DCLoadCurrentResetFailureDetection();   // Entry Function.
            break;

        default:
            // Event is ignored for all other values of DC_Load_Current_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Debounce Completed F10E3 Event for the DC Load Current Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDebounceCompletedF10E3(void)
{
    switch (DC_Load_Current_Monitor_State)
    {
        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F10_E3_DETECTED:
            DC_Load_Current_Monitor_State = STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F10E3;
            break;

        default:
            // Event is ignored for all other values of DC_Load_Current_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Debounce Completed F9E4 Event for the DC Load Current Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDebounceCompletedF9E4(void)
{
    switch (DC_Load_Current_Monitor_State)
    {
        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F9_E4_DETECTED:
            DC_Load_Current_Monitor_State = STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F9E4;
            break;

        default:
            // Event is ignored for all other values of DC_Load_Current_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Debounce Completed F5E3 Event for the DC Load Current Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDebounceCompletedF5E3(void)
{
    switch (DC_Load_Current_Monitor_State)
    {
        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F5_E3_DETECTED:
            DC_Load_Current_Monitor_State = STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F5E3;
            break;

        default:
            // Event is ignored for all other values of DC_Load_Current_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the F10E3 Failure Detected Event for the DC Load Current Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventF10E3FailureDetected(void)
{
    switch (DC_Load_Current_Monitor_State)
    {
        case STATE_DC_LOAD_CURRENT_MONITOR_NO_FAILURE:
        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F9E4:
        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F5E3:
            DC_Load_Current_Monitor_State = STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F10_E3_DETECTED;
            SetFailureF10E3Detection();             // Entry Function.
            break;

        default:
            // Event is ignored for all other values of DC_Load_Current_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the F9E4 Failure Detected Event for the DC Load Current Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventF9E4FailureDetected(void)
{
    switch (DC_Load_Current_Monitor_State)
    {
        case STATE_DC_LOAD_CURRENT_MONITOR_NO_FAILURE:
        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F10E3:
        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F5E3:
            DC_Load_Current_Monitor_State = STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F9_E4_DETECTED;
            SetFailureF9E4Detection();              // Entry Function.
            break;

        default:
            // Event is ignored for all other values of DC_Load_Current_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Fault Reset Event for the DC Load Current Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventFaultReset(void)
{
    switch (DC_Load_Current_Monitor_State)
    {
        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F9_E4_DETECTED:
        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F10_E3_DETECTED:
        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F5_E3_DETECTED:
        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F9E4:
        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F10E3:
        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F5E3:
            DC_Load_Current_Monitor_State = STATE_DC_LOAD_CURRENT_MONITOR_NO_FAILURE;
            DCLoadCurrentResetFailureDetection();   // Entry Function.
            break;

        default:
            // Event is ignored for all other values of DC_Load_Current_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the F5E3 Failure Detected Event for the DC Load Current Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventF5E3FailureDetected(void)
{
    switch (DC_Load_Current_Monitor_State)
    {
        case STATE_DC_LOAD_CURRENT_MONITOR_NO_FAILURE:
        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F9E4:
        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F10E3:
            DC_Load_Current_Monitor_State = STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F5_E3_DETECTED;
            SetFailureF5E3Detection();              // Entry Function.
            break;

        default:
            // Event is ignored for all other values of DC_Load_Current_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the No Failure F10E3 Present Event for the DC Load Current Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNoFailureF10E3Present(void)
{
    switch (DC_Load_Current_Monitor_State)
    {
        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F10_E3_DETECTED:
            DC_Load_Current_Monitor_State = STATE_DC_LOAD_CURRENT_MONITOR_NO_FAILURE;
            DCLoadCurrentResetFailureDetection();   // Entry Function.
            break;

        default:
            // Event is ignored for all other values of DC_Load_Current_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the No Failure F9E4 Present Event for the DC Load Current Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNoFailureF9E4Present(void)
{
    switch (DC_Load_Current_Monitor_State)
    {
        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F9_E4_DETECTED:
            DC_Load_Current_Monitor_State = STATE_DC_LOAD_CURRENT_MONITOR_NO_FAILURE;
            DCLoadCurrentResetFailureDetection();   // Entry Function.
            break;

        default:
            // Event is ignored for all other values of DC_Load_Current_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the No Failure F5E3 Present Event for the DC Load Current Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNoFailureF5E3Present(void)
{
    switch (DC_Load_Current_Monitor_State)
    {
        case STATE_DC_LOAD_CURRENT_MONITOR_FAULT_F5_E3_DETECTED:
            DC_Load_Current_Monitor_State = STATE_DC_LOAD_CURRENT_MONITOR_NO_FAILURE;
            DCLoadCurrentResetFailureDetection();   // Entry Function.
            break;

        default:
            // Event is ignored for all other values of DC_Load_Current_Monitor_State.
            break;
    }
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

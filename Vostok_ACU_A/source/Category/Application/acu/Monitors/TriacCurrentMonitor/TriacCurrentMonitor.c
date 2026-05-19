/*
 * TriacCurrentMonitor.c
 *
 *  Created on: 22.05.2018
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
   @startuml{CurrentSenseFaultMonitor State Machine.jpg}
        title Current Sense Fault Monitor State Machine

        [*] --> INITIALIZE

        INITIALIZE : entry / InitializeModule()
        INITIALIZE --> NO_FAILURE  : EventStartMonitor
        INITIALIZE --> [*]         : EventSettingDataMissing

        NO_FAILURE : entry / ResetFailureDetection()
        NO_FAILURE : do /\t CheckForFailure()
        NO_FAILURE --> FAULT_F9E2_DETECTED : EventF9E2FailureDetected
        NO_FAILURE --> FAULT_F8E2_DETECTED : EventF8E2FailureDetected
        NO_FAILURE --> FAULT_F10E1_DETECTED : EventF10E1FailureDetected
        NO_FAILURE --> FAULT_F10E2_F8E7_DETECTED : EventF10E2F8E7FailureDetected
        NO_FAILURE --> FAULT_F1E5_0_DETECTED : EventF1E5_0FailureDetected
        NO_FAILURE --> FAULT_F1E5_4_DETECTED : EventF1E5_4FailureDetected
        NO_FAILURE --> FAULT_F1E5_5_DETECTED : EventF1E5_5FailureDetected
        NO_FAILURE --> FAULT_F1E5_1_DETECTED : EventF1E5_1FailureDetected
        NO_FAILURE --> FAULT_F1E5_2_3_DETECTED : EventF1E5_2_3FailureDetected

        FAULT_F9E2_DETECTED : entry /\t SetFailureF9E2Detection()
        FAULT_F9E2_DETECTED : do /\t CheckFailureRemoval()
        FAULT_F9E2_DETECTED --> NO_FAILURE       : EventNoFailureF9E2Present
        FAULT_F9E2_DETECTED --> NO_FAILURE : EventFaultReset
        FAULT_F9E2_DETECTED --> FAULT_F9E2       : EventDebounceCompletedF9E2

        FAULT_F8E2_DETECTED : entry /\t SetFailureF8E2Detection()
        FAULT_F8E2_DETECTED : do /\t CheckFailureRemoval()
        FAULT_F8E2_DETECTED --> NO_FAILURE       : EventNoFailureF8E2Present
        FAULT_F8E2_DETECTED --> NO_FAILURE       : EventFaultReset
        FAULT_F8E2_DETECTED --> FAULT_F8E2       : EventDebounceCompletedF8E2

        FAULT_F10E1_DETECTED : entry /\t SetFailureF10E1Detection()
        FAULT_F10E1_DETECTED : do /\t CheckFailureRemoval()
        FAULT_F10E1_DETECTED --> NO_FAILURE       : EventNoFailureF10E1Present
        FAULT_F10E1_DETECTED --> NO_FAILURE       : EventFaultReset
        FAULT_F10E1_DETECTED --> FAULT_F10E1      : EventDebounceCompletedF10E1

        FAULT_F10E2_F8E7_DETECTED : entry /\t SetFailureF10E2F8E7Detection()
        FAULT_F10E2_F8E7_DETECTED : do /\t CheckFailureRemoval()
        FAULT_F10E2_F8E7_DETECTED --> NO_FAILURE       : EventNoFailureF10E2F8E7Present
        FAULT_F10E2_F8E7_DETECTED --> NO_FAILURE       : EventFaultReset
        FAULT_F10E2_F8E7_DETECTED --> FAULT_F10E2_F8E7 : EventDebounceCompletedF10E2_F8E7

        FAULT_F1E5_0_DETECTED : entry /\t SetFailureF1E5_0Detection()
        FAULT_F1E5_0_DETECTED : do /\t CheckFailureRemoval()
        FAULT_F1E5_0_DETECTED --> NO_FAILURE       : EventNoFailureF1E5_0Present
        FAULT_F1E5_0_DETECTED --> NO_FAILURE       : EventFaultReset
        FAULT_F1E5_0_DETECTED --> FAULT_F1E5_0 : EventDebounceCompletedF1E5_0

        FAULT_F1E5_4_DETECTED : entry /\t SetFailureF1E5_4Detection()
        FAULT_F1E5_4_DETECTED : do /\t CheckFailureRemoval()
        FAULT_F1E5_4_DETECTED --> NO_FAILURE       : EventNoFailureF1E5_4Present
        FAULT_F1E5_4_DETECTED --> NO_FAILURE       : EventFaultReset
        FAULT_F1E5_4_DETECTED --> FAULT_F1E5_4 : EventDebounceCompletedF1E5_4

        FAULT_F1E5_5_DETECTED : entry /\t SetFailureF1E5_5Detection()
        FAULT_F1E5_5_DETECTED : do /\t CheckFailureRemoval()
        FAULT_F1E5_5_DETECTED --> NO_FAILURE       : EventNoFailureF1E5_5Present
        FAULT_F1E5_5_DETECTED --> NO_FAILURE       : EventFaultReset
        FAULT_F1E5_5_DETECTED --> FAULT_F1E5_5 : EventDebounceCompletedF1E5_5

        FAULT_F1E5_1_DETECTED : entry /\t SetFailureF1E5_1Detection()
        FAULT_F1E5_1_DETECTED : do /\t CheckFailureRemoval()
        FAULT_F1E5_1_DETECTED --> NO_FAILURE       : EventNoFailureF1E5_1Present
        FAULT_F1E5_1_DETECTED --> NO_FAILURE       : EventFaultReset
        FAULT_F1E5_1_DETECTED --> FAULT_F1E5_1 : EventDebounceCompletedF1E5_1

        FAULT_F1E5_2_3_DETECTED : entry /\t SetFailureF1E5_2_3Detection()
        FAULT_F1E5_2_3_DETECTED : do /\t CheckFailureRemoval()
        FAULT_F1E5_2_3_DETECTED --> NO_FAILURE       : EventNoFailureF1E5_2_3Present
        FAULT_F1E5_2_3_DETECTED --> NO_FAILURE       : EventFaultReset
        FAULT_F1E5_2_3_DETECTED --> FAULT_F1E5_2_3 : EventDebounceCompletedF1E5_2_3

        FAULT_F9E2 : do /\t CheckForFailure()
        FAULT_F9E2 --> NO_FAILURE : EventFaultReset
        FAULT_F9E2 --> FAULT_F8E2_DETECTED  : EventF8E2FailureDetected
        FAULT_F9E2 --> FAULT_F10E1_DETECTED  : EventF10E1FailureDetected
        FAULT_F9E2 --> FAULT_F10E2_F8E7_DETECTED  : EventF10E2F8E7FailureDetected
        FAULT_F9E2 --> FAULT_F1E5_0_DETECTED : EventF1E5_0FailureDetected
        FAULT_F9E2 --> FAULT_F1E5_4_DETECTED : EventF1E5_4FailureDetected
        FAULT_F9E2 --> FAULT_F1E5_5_DETECTED : EventF1E5_5FailureDetected
        FAULT_F9E2 --> FAULT_F1E5_1_DETECTED : EventF1E5_1FailureDetected
        FAULT_F9E2 --> FAULT_F1E5_2_3_DETECTED : EventF1E5_2_3FailureDetected

        FAULT_F8E2 : do /\t CheckForFailure()
        FAULT_F8E2 --> NO_FAILURE : EventFaultReset
        FAULT_F8E2 --> FAULT_F9E2_DETECTED  : EventF9E2FailureDetected
        FAULT_F8E2 --> FAULT_F10E1_DETECTED  : EventF10E1FailureDetected
        FAULT_F8E2 --> FAULT_F10E2_F8E7_DETECTED  : EventF10E2F8E7FailureDetected
        FAULT_F8E2 --> FAULT_F1E5_0_DETECTED : EventF1E5_0FailureDetected
        FAULT_F8E2 --> FAULT_F1E5_4_DETECTED : EventF1E5_4FailureDetected
        FAULT_F8E2 --> FAULT_F1E5_5_DETECTED : EventF1E5_5FailureDetected
        FAULT_F8E2 --> FAULT_F1E5_1_DETECTED : EventF1E5_1FailureDetected
        FAULT_F8E2 --> FAULT_F1E5_2_3_DETECTED : EventF1E5_2_3FailureDetected

        FAULT_F10E1 : do /\t CheckForFailure()
        FAULT_F10E1 --> NO_FAILURE : EventFaultReset
        FAULT_F10E1 --> FAULT_F9E2_DETECTED  : EventF9E2FailureDetected
        FAULT_F10E1 --> FAULT_F8E2_DETECTED  : EventF8E2FailureDetected
        FAULT_F10E1 --> FAULT_F10E2_F8E7_DETECTED  : EventF10E2F8E7FailureDetected
        FAULT_F10E1 --> FAULT_F1E5_0_DETECTED : EventF1E5_0FailureDetected
        FAULT_F10E1 --> FAULT_F1E5_4_DETECTED : EventF1E5_4FailureDetected
        FAULT_F10E1 --> FAULT_F1E5_5_DETECTED : EventF1E5_5FailureDetected
        FAULT_F10E1 --> FAULT_F1E5_1_DETECTED : EventF1E5_1FailureDetected
        FAULT_F10E1 --> FAULT_F1E5_2_3_DETECTED : EventF1E5_2_3FailureDetected

        FAULT_F10E2_F8E7 : do /\t CheckForFailure()
        FAULT_F10E2_F8E7 --> NO_FAILURE : EventFaultReset
        FAULT_F10E2_F8E7 --> FAULT_F9E2_DETECTED  : EventF9E2FailureDetected
        FAULT_F10E2_F8E7 --> FAULT_F8E2_DETECTED  : EventF8E2FailureDetected
        FAULT_F10E2_F8E7 --> FAULT_F10E1_DETECTED  : EventF10E1FailureDetected
        FAULT_F10E2_F8E7 --> FAULT_F1E5_0_DETECTED : EventF1E5_0FailureDetected
        FAULT_F10E2_F8E7 --> FAULT_F1E5_4_DETECTED : EventF1E5_4FailureDetected
        FAULT_F10E2_F8E7 --> FAULT_F1E5_5_DETECTED : EventF1E5_5FailureDetected
        FAULT_F10E2_F8E7 --> FAULT_F1E5_1_DETECTED : EventF1E5_1FailureDetected
        FAULT_F10E2_F8E7 --> FAULT_F1E5_2_3_DETECTED : EventF1E5_2_3FailureDetected

        FAULT_F1E5_0 : do /\t CheckForFailure()
        FAULT_F1E5_0 --> NO_FAILURE : EventFaultReset
        FAULT_F1E5_0 --> FAULT_F9E2_DETECTED  : EventF9E2FailureDetected
        FAULT_F1E5_0 --> FAULT_F8E2_DETECTED  : EventF8E2FailureDetected
        FAULT_F1E5_0 --> FAULT_F10E1_DETECTED  : EventF10E1FailureDetected
        FAULT_F1E5_0 --> FAULT_F10E2_F8E7_DETECTED : EventF10E2F8E7FailureDetected
        FAULT_F1E5_0 --> FAULT_F1E5_4_DETECTED : EventF1E5_4FailureDetected
        FAULT_F1E5_0 --> FAULT_F1E5_5_DETECTED : EventF1E5_5FailureDetected
        FAULT_F1E5_0 --> FAULT_F1E5_1_DETECTED : EventF1E5_1FailureDetected
        FAULT_F1E5_0 --> FAULT_F1E5_2_3_DETECTED : EventF1E5_2_3FailureDetected

        FAULT_F1E5_4 : do /\t CheckForFailure()
        FAULT_F1E5_4 --> NO_FAILURE : EventFaultReset
        FAULT_F1E5_4 --> FAULT_F9E2_DETECTED  : EventF9E2FailureDetected
        FAULT_F1E5_4 --> FAULT_F8E2_DETECTED  : EventF8E2FailureDetected
        FAULT_F1E5_4 --> FAULT_F10E1_DETECTED  : EventF10E1FailureDetected
        FAULT_F1E5_4 --> FAULT_F10E2_F8E7_DETECTED : EventF10E2F8E7FailureDetected
        FAULT_F1E5_4 --> FAULT_F1E5_0_DETECTED : EventF1E5_0FailureDetected
        FAULT_F1E5_4 --> FAULT_F1E5_5_DETECTED : EventF1E5_5FailureDetected
        FAULT_F1E5_4 --> FAULT_F1E5_1_DETECTED : EventF1E5_1FailureDetected
        FAULT_F1E5_4 --> FAULT_F1E5_2_3_DETECTED : EventF1E5_2_3FailureDetected

        FAULT_F1E5_5 : do /\t CheckForFailure()
        FAULT_F1E5_5 --> NO_FAILURE : EventFaultReset
        FAULT_F1E5_5 --> FAULT_F9E2_DETECTED  : EventF9E2FailureDetected
        FAULT_F1E5_5 --> FAULT_F8E2_DETECTED  : EventF8E2FailureDetected
        FAULT_F1E5_5 --> FAULT_F10E1_DETECTED  : EventF10E1FailureDetected
        FAULT_F1E5_5 --> FAULT_F10E2_F8E7_DETECTED : EventF10E2F8E7FailureDetected
        FAULT_F1E5_5 --> FAULT_F1E5_0_DETECTED : EventF1E5_0FailureDetected
        FAULT_F1E5_5 --> FAULT_F1E5_4_DETECTED : EventF1E5_4FailureDetected
        FAULT_F1E5_5 --> FAULT_F1E5_1_DETECTED : EventF1E5_1FailureDetected
        FAULT_F1E5_5 --> FAULT_F1E5_2_3_DETECTED : EventF1E5_2_3FailureDetected

        FAULT_F1E5_1 : do /\t CheckForFailure()
        FAULT_F1E5_1 --> NO_FAILURE : EventFaultReset
        FAULT_F1E5_1 --> FAULT_F9E2_DETECTED  : EventF9E2FailureDetected
        FAULT_F1E5_1 --> FAULT_F8E2_DETECTED  : EventF8E2FailureDetected
        FAULT_F1E5_1 --> FAULT_F10E1_DETECTED  : EventF10E1FailureDetected
        FAULT_F1E5_1 --> FAULT_F10E2_F8E7_DETECTED : EventF10E2F8E7FailureDetected
        FAULT_F1E5_1 --> FAULT_F1E5_0_DETECTED : EventF1E5_0FailureDetected
        FAULT_F1E5_1 --> FAULT_F1E5_4_DETECTED : EventF1E5_4FailureDetected
        FAULT_F1E5_1 --> FAULT_F1E5_5_DETECTED : EventF1E5_5FailureDetected
        FAULT_F1E5_1 --> FAULT_F1E5_2_3_DETECTED : EventF1E5_2_3FailureDetected

        FAULT_F1E5_2_3 : do /\t CheckForFailure()
        FAULT_F1E5_2_3 --> NO_FAILURE : EventFaultReset
        FAULT_F1E5_2_3 --> FAULT_F9E2_DETECTED  : EventF9E2FailureDetected
        FAULT_F1E5_2_3 --> FAULT_F8E2_DETECTED  : EventF8E2FailureDetected
        FAULT_F1E5_2_3 --> FAULT_F10E1_DETECTED  : EventF10E1FailureDetected
        FAULT_F1E5_2_3 --> FAULT_F10E2_F8E7_DETECTED : EventF10E2F8E7FailureDetected
        FAULT_F1E5_2_3 --> FAULT_F1E5_0_DETECTED : EventF1E5_0FailureDetected
        FAULT_F1E5_2_3 --> FAULT_F1E5_4_DETECTED : EventF1E5_4FailureDetected
        FAULT_F1E5_2_3 --> FAULT_F1E5_5_DETECTED : EventF1E5_5FailureDetected
        FAULT_F1E5_2_3 --> FAULT_F1E5_1_DETECTED : EventF1E5_1FailureDetected

    @enduml

 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "TriacCurrentMonitor.h"
#include "TriacCurrentMonitor_prv.h"

// -- Other Modules --
#include "TriacVoltageMonitor.h"
#include "Fault.h"
#include "Hbl.h"

#include "Log.h"
#include "Micro.h"
#include "DishAuxVar.h"

static ON_OFF_TYPE Next_Drain_Pump_Status;
static ON_OFF_TYPE Triac_Diag_Drain_Pump_Status;
static ON_OFF_TYPE Next_Fill_Valve_Status;
static ON_OFF_TYPE Triac_Diag_Fill_Valve_Status;
static ON_OFF_TYPE Next_Diverter_Status;
static ON_OFF_TYPE Triac_Diag_Diverter_Status;
static ON_OFF_TYPE Next_Dispenser_Status;
static ON_OFF_TYPE Triac_Diag_Dispenser_Status;
static ON_OFF_TYPE Next_Vent_Status;
static ON_OFF_TYPE Triac_Diag_Vent_Status;
static ON_OFF_TYPE Next_Regeneration_Status;
static ON_OFF_TYPE Triac_Diag_Regeneration_Status;
static ON_OFF_TYPE Next_Lower_Tank_Status;
static ON_OFF_TYPE Triac_Diag_Lower_Tank_Status;
static ON_OFF_TYPE Next_Upper_Tank_Status;
static ON_OFF_TYPE Triac_Diag_Upper_Tank_Status;


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Current Sense Fault Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Current Sense Fault Monitor state machine.
typedef enum CURRENT_SENSE_FAULT_MONITOR_STATE_ENUM
{
    STATE_CURRENT_SENSE_FAULT_MONITOR_NONE                        = 0,

    STATE_CURRENT_SENSE_FAULT_MONITOR_INITIALIZE                  = 1, //!< (initial_state)
    STATE_CURRENT_SENSE_FAULT_MONITOR_NO_FAILURE                  = 2,
    STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F9_E2_DETECTED        = 3,
    STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F8_E2_DETECTED        = 4,
    STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10_E1_DETECTED       = 5,
    STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10_E2_F8_E7_DETECTED = 6,
    STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_0_DETECTED      = 7,
    STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_4_DETECTED      = 8,
    STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_5_DETECTED      = 9,
    STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_1_DETECTED      = 10,
    STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_2_3_DETECTED    = 11,
    STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F9E2                  = 12,
    STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F8E2                  = 13,
    STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10E1                 = 14,
    STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10_E2_F8E7           = 15,
    STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_0               = 16,
    STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_4               = 17,
    STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_5               = 18,
    STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_1               = 19,
    STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_2_3             = 20,

    STATE_CURRENT_SENSE_FAULT_MONITOR_END                         = 21
} CURRENT_SENSE_FAULT_MONITOR_STATE_TYPE;

//! The state variable for the Current Sense Fault Monitor state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static CURRENT_SENSE_FAULT_MONITOR_STATE_TYPE Current_Sense_Fault_Monitor_State;

// -- Private Function Prototypes used by the state machine for customization functions --
static void CheckFailureRemoval(void);
static void CheckForFailure(void);
static void InitializeModule(void);
static void ResetFailureDetection(void);
static void SetFailureF10E1Detection(void);
static void SetFailureF10E2F8E7Detection(void);
static void SetFailureF1E50Detection(void);
static void SetFailureF1E51Detection(void);
static void SetFailureF1E523Detection(void);
static void SetFailureF1E54Detection(void);
static void SetFailureF1E55Detection(void);
static void SetFailureF8E2Detection(void);
static void SetFailureF9E2Detection(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void CurrentSenseFaultMonitorExecute(void);
static void CurrentSenseFaultMonitorInitialize(void);
static void EventDebounceCompletedF10E1(void);
static void EventDebounceCompletedF10E2F8E7(void);
static void EventDebounceCompletedF1E50(void);
static void EventDebounceCompletedF1E51(void);
static void EventDebounceCompletedF1E523(void);
static void EventDebounceCompletedF1E54(void);
static void EventDebounceCompletedF1E55(void);
static void EventDebounceCompletedF8E2(void);
static void EventDebounceCompletedF9E2(void);
static void EventF10E1FailureDetected(void);
static void EventF10E2F8E7FailureDetected(void);
static void EventF1E50FailureDetected(void);
static void EventF1E51FailureDetected(void);
static void EventF1E523FailureDetected(void);
static void EventF1E54FailureDetected(void);
static void EventF1E55FailureDetected(void);
static void EventF8E2FailureDetected(void);
static void EventF9E2FailureDetected(void);
static void EventFaultReset(void);
static void EventNoFailureF10E1Present(void);
static void EventNoFailureF10E2F8E7Present(void);
static void EventNoFailureF1E50Present(void);
static void EventNoFailureF1E51Present(void);
static void EventNoFailureF1E523Present(void);
static void EventNoFailureF1E54Present(void);
static void EventNoFailureF1E55Present(void);
static void EventNoFailureF8E2Present(void);
static void EventNoFailureF9E2Present(void);
static void EventSettingDataMissing(void);
static void EventStartMonitor(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define MODULE_NAME                                             MODULE_SSDRAIN_MOTOR_FAULT_MONITOR
#define TRIACS_DIAGNOSTIC_DEBOUNCE_100MS                        5
#define TRIACS_DIAGNOSTIC_VAX_DEBOUNCE_100MS                    20

typedef enum MODULE_SSDRAIN_MOTOR_FAULT_MONITOR_LOG_MESSAGE_ID_ENUM
{
    ERROR_MISSING_COMPONENTS  = 1,
    ERROR_MISSING_PARAMETERS,
    ERROR_VENT_DELAY_OUT_OF_RANGE,
} MODULE_SSDRAIN_MOTOR_FAULT_MONITOR_LOG_MESSAGE_ID_TYPE;


typedef enum
{
    DRAIN_FAULT_DETECTION_EXTERNAL = 0,
    DRAIN_FAULT_DETECTION_INDEPENDENT,
    DRAIN_FAULT_DETECTION_ACCUMULATED,
} DRAIN_FAULT_DETECTION_TYPE;

typedef enum
{
    VENT_LOAD = 0,
    REGEN_LOAD,
} VENT_REGEN_LOAD_TYPE;

typedef enum
{
    DELAY_0 = 0,
    DELAY_1 = 1,
    DELAY_2 = 2,
    DELAY_3 = 3,
    DELAY_4 = 4,
    //do not modify below
    DELAY_NUM,
} CURRENT_SENSE_DELAY_TYPE;





static DRAIN_FAULT_DETECTION_TYPE Drain_Fault_Detection;
static VENT_REGEN_LOAD_TYPE Vent_Regen_Load;

static SSDRAIN_MOTOR_FAULT_PARAMS_TYPE SSDrain_Fault_Params;
static FILL_VALVE_FAULT_PARAMS_TYPE Fill_Valve_Fault_Params;
static DISPENSER_FAULT_PARAMS_TYPE Dispenser_Fault_Params;
static VENT_FAULT_PARAMS_TYPE Vent_Fault_Params;
static REGEN_FAULT_PARAMS_TYPE Regen_Fault_Params;
static BOOL_TYPE CurrentSenseStatusPending;  // load toggled but diag status not checked
static uint8 Triac_Ref_Timer;
static uint8 Triac_Debounce_Timer;
static uint16 Current_Sense_Pump;

#if (TRIAC_DIAG_DEBUG == ENABLED)
static uint16 Debug_Current_Input_Triac;
static uint16 Debug_Current_Input_Drain;
#endif

static uint16 Current_Sense_Diagnostic[DELAY_NUM];  // Diag current samples history
static CURRENT_SENSE_DELAY_TYPE Current_Sense_Delay_Needed;  // Historic current samples needed for Vent 
static HBL_CROSS_LOAD_TYPE Current_Sense_Crossload_Cfg[HBL_XLOAD_NUM_CROSSLOAD];
static uint8 Current_Sense_Crossloads_Num;
static uint8 Current_Sense_Delay_Load;  // Load ID where Vent delay depend ON
static BOOL_TYPE Current_Sense_Triac_Present; //  Triac present - no historic current data needed



TRIACS_DIAGNOSTIC_TYPE Triacs_Diagnostic;
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

static TRIACS_DIAGNOSTIC_STATUS_TYPE TriacsDiagnosticGetStatus(HBL_LOAD_ENUM_TYPE load_id);
static void TriacsDiagnosticHandler();
static void TriacsDiagnosticUpdateRange(HBL_LOAD_ENUM_TYPE output, ON_OFF_TYPE new_digital_output_state);
static void TriacSetRef(void);
static void DiagnosticCheckIfInRange(void);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//----------------------------------------------------------------------------
/**
 *  @brief      SSDrainMotorFaultMonitor__Initialize
 *  @details    Function used to initialize the Triac Current Monitor.
 *
 */
void TriacCurrentMonitor__Initialize(void)
{
    uint8 index, index2;
    uint8 *ptr;
    HBL_CROSS_LOAD_TYPE *cfg_ptr;
    SETTINGFILE_LOADER_TYPE cross_load_table;
    uint8 Delay_Sum = 0;

    CurrentSenseFaultMonitorInitialize();

    Triac_Debounce_Timer = TRIACS_DIAGNOSTIC_DEBOUNCE_100MS;
    Triac_Ref_Timer = TRIACS_DIAGNOSTIC_DEBOUNCE_100MS;
    Next_Drain_Pump_Status = OFF;
    Triac_Diag_Drain_Pump_Status = OFF;
    Next_Fill_Valve_Status = OFF;
    Triac_Diag_Fill_Valve_Status = OFF;
    Next_Diverter_Status = OFF;
    Triac_Diag_Diverter_Status = OFF;
    Next_Dispenser_Status = OFF;
    Triac_Diag_Dispenser_Status = OFF;
    Next_Vent_Status = OFF;
    Triac_Diag_Vent_Status = OFF;
    Next_Regeneration_Status = OFF;
    Triac_Diag_Regeneration_Status = OFF;
    Next_Lower_Tank_Status = OFF;
    Triac_Diag_Lower_Tank_Status = OFF;
    Next_Upper_Tank_Status = OFF;
    Triac_Diag_Upper_Tank_Status = OFF;

    Current_Sense_Pump = 0;
    CurrentSenseStatusPending = FALSE;

    Current_Sense_Delay_Load = 0;
    Current_Sense_Delay_Needed = DELAY_0;
    for (index = 0; index < DELAY_NUM; index++)
    {
        Current_Sense_Diagnostic[index] = 0;
    }

    Current_Sense_Triac_Present = TRUE;

    if (SettingFile__BasicLoader(SF_PTR_ACU_IO_CONFIG, SF_DISPL_CROSS_LOADS, &cross_load_table) == PASS)
    {
        index = (cross_load_table.Length / 2);

        if (index > HBL_XLOAD_NUM_CROSSLOAD)
        {
            index = HBL_XLOAD_NUM_CROSSLOAD;
        }
        Current_Sense_Crossloads_Num = index;

        for (index = 0 ; index < HBL_XLOAD_NUM_CROSSLOAD ; index++)
        {
            SETTINGFILE_LOADER_TYPE param_table;
            cfg_ptr = &Current_Sense_Crossload_Cfg[index];
            ptr = &cross_load_table.Data[2*index];

            cfg_ptr->Crossload_Position = HBL_LOAD_INVALID;
            cfg_ptr->Parameter_Ptr = NULL;
            cfg_ptr->Number_Of_Couples = 0;

            if ( (index < Current_Sense_Crossloads_Num) &&
                (ptr[0] < Hbl__GetNumLoads()) )
            {
                //Getting the CROSS LOAD parameters from the Settings File.
                if (SettingFile__BasicLoader(SF_PTR_ACU_IO_DEVICE, ptr[1], &param_table) == PASS)
                {
                    CROSSLOAD_PARAMETERS_TYPE *new_param_ptr = ((CROSSLOAD_PARAMETERS_TYPE*)(param_table.Data)); //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4];
                    cfg_ptr->Crossload_Position = ptr[0];
                    cfg_ptr->Parameter_Ptr = new_param_ptr;
                    cfg_ptr->Number_Of_Couples = (param_table.Length - 2) / 2;
                }
            }
        }

        //calculate delay based on On_times from dependent crossloads

        for (index = 0 ; index < HBL_XLOAD_NUM_CROSSLOAD ; index++)
        {
            if (Current_Sense_Crossload_Cfg[index].Crossload_Position == Hbl__GetLoadIndexByLoadID(HBL_LOAD_VENT))
            {
                // No Vent Triac - Vent controlled by Pilot state
                Current_Sense_Triac_Present = FALSE;
                break;
            }
        }
    }

    if (Current_Sense_Triac_Present == FALSE)
    {
    // look for Vent depenency on Pilot Relay
        for (index = 0 ; index < HBL_XLOAD_NUM_CROSSLOAD ; index++)
        {
            for (index2 = 0 ; index2 < Current_Sense_Crossload_Cfg[index].Number_Of_Couples ; index2++)
            {
                if (Current_Sense_Crossload_Cfg[index].Parameter_Ptr->Load_Position[index2].On == Hbl__GetLoadIndexByLoadID(HBL_LOAD_VENT))
                {
                    if (Current_Sense_Crossload_Cfg[index].Crossload_Position == Hbl__GetLoadIndexByLoadID(HBL_LOAD_PILOT_RELAY))
                    {
                        Delay_Sum = Current_Sense_Crossload_Cfg[index].Parameter_Ptr->On_Delay;
                        if(Delay_Sum < DELAY_NUM)
                        {
                            Current_Sense_Delay_Needed = (CURRENT_SENSE_DELAY_TYPE)Delay_Sum;
                        }
                        else
                        {
                            LOG_ADD_EXCEPTION (ERROR_VENT_DELAY_OUT_OF_RANGE, Delay_Sum);
                        }
                        Current_Sense_Delay_Load = Current_Sense_Crossload_Cfg[index].Crossload_Position;
                            // look for Pilot Relay dependency on Relay Enable
                        for (index = 0 ; index < HBL_XLOAD_NUM_CROSSLOAD ; index++)
                        {
                            for (index2 = 0 ; index2 < Current_Sense_Crossload_Cfg[index].Number_Of_Couples ; index2++)
                            {
                                if (Current_Sense_Crossload_Cfg[index].Parameter_Ptr->Load_Position[index2].On == Hbl__GetLoadIndexByLoadID(Hbl__GetLoadIDByIndex(Current_Sense_Delay_Load)))
                                {
                                    if (Current_Sense_Crossload_Cfg[index].Crossload_Position == Hbl__GetLoadIndexByLoadID(HBL_LOAD_RELAY_ENABLE))
                                    {
                                        Delay_Sum = (Current_Sense_Delay_Needed + Current_Sense_Crossload_Cfg[index].Parameter_Ptr->On_Delay);
                                        if (Delay_Sum < DELAY_NUM)
                                        {
                                            Current_Sense_Delay_Needed = (CURRENT_SENSE_DELAY_TYPE)Delay_Sum;
                                        }
                                        else
                                        {
                                            LOG_ADD_EXCEPTION (ERROR_VENT_DELAY_OUT_OF_RANGE, Delay_Sum);
                                        }
                                    }
                                    break;
                                }
                            }
                        }
                    }
                    break;
                }
            }
        }
    }
}


//----------------------------------------------------------------------------
/**
 *  @brief      SSDrainMotorFaultMonitor__Execution
 *  @details    Function used to perform the Triac Current Monitor.
 *
 */
void TriacCurrentMonitor__Execution(void)
{
    // sequence is important
    TriacsDiagnosticHandler();
    DiagnosticCheckIfInRange();
    CurrentSenseFaultMonitorExecute();
}


//----------------------------------------------------------------------------
/**
 *  @brief      SSDrainMotorFaultMonitor__ExitFault
 *  @details    Function to be called when the Faults have to be removed.
 *
 */
void TriacCurrentMonitor__ExitFault(void)
{
    EventFaultReset();
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief if all conditions are met, check for potential faults
 */
static void CheckForFailure(void)
{
    if ((0 == Triac_Debounce_Timer) && (0 == Triac_Ref_Timer) && (FALSE != CurrentSenseStatusPending))
    {
        if ((TRIACS_DIAGNOSTIC_NOT_IN_RANGE == TriacsDiagnosticGetStatus(HBL_LOAD_DRAIN_PUMP)) &&
            (DRAIN_FAULT_DETECTION_EXTERNAL != Drain_Fault_Detection))
        {
            if (TRIAC_LOADS_VOLTAGE_SENSOR_UNKNOWN != GET_VOLTAGE_SENSOR_STATUS)
            {
				if (TRIAC_LOADS_VOLTAGE_SENSOR_DECLINED != GET_VOLTAGE_SENSOR_STATUS)
				{
					EventF9E2FailureDetected();
				}
				else
				{
					EventF1E54FailureDetected();
				}
            }
        }
        else if (TRIACS_DIAGNOSTIC_NOT_IN_RANGE == TriacsDiagnosticGetStatus(HBL_LOAD_FILL_VALVE))
        {
            if (TRIAC_LOADS_VOLTAGE_SENSOR_UNKNOWN != GET_VOLTAGE_SENSOR_STATUS)
            {
                if (TRIAC_LOADS_VOLTAGE_SENSOR_DECLINED != GET_VOLTAGE_SENSOR_STATUS)
                {
                    EventF8E2FailureDetected();
                }
                else
                {
                    EventF1E50FailureDetected();
                }
            }
        }
        else if (TRIACS_DIAGNOSTIC_NOT_IN_RANGE == TriacsDiagnosticGetStatus(HBL_LOAD_DISPENSER))
        {
            if (TRIAC_LOADS_VOLTAGE_SENSOR_UNKNOWN != GET_VOLTAGE_SENSOR_STATUS)
            {
                if (TRIAC_LOADS_VOLTAGE_SENSOR_DECLINED != GET_VOLTAGE_SENSOR_STATUS)
                {
                    EventF10E1FailureDetected();
                }
                else
                {
                    EventF1E51FailureDetected();
                }
            }

        }
        else if (((Vent_Regen_Load == REGEN_LOAD) && (TRIACS_DIAGNOSTIC_NOT_IN_RANGE == TriacsDiagnosticGetStatus(HBL_LOAD_REGENERATION_VALVE)))
                || ((Vent_Regen_Load == VENT_LOAD) && (TRIACS_DIAGNOSTIC_NOT_IN_RANGE == TriacsDiagnosticGetStatus(HBL_LOAD_VENT))))
        {
            if (TRIAC_LOADS_VOLTAGE_SENSOR_UNKNOWN != GET_VOLTAGE_SENSOR_STATUS)
            {
                if (TRIAC_LOADS_VOLTAGE_SENSOR_DECLINED != GET_VOLTAGE_SENSOR_STATUS)
                {
                    EventF10E2F8E7FailureDetected();
                }
                else
                {
                    EventF1E523FailureDetected();
                }
            }

        }
        else if (HBL_LOAD_DIVERTER == Triacs_Diagnostic.Last_Changed_Triac_Output)
        {
            if (TRIAC_LOADS_VOLTAGE_SENSOR_DECLINED == GET_VOLTAGE_SENSOR_STATUS)
            {
                EventF1E55FailureDetected();
            }
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
static void InitializeModule(void)
{
    uint8 diagnostic_current_gi_index;
    uint8 drain_pomp_current_gi_index;

    uint8 ssdrain_index;
    uint8 fill_valve_index;
    uint8 dispenser_index;
    uint8 vent_index;
    uint8 regen_index;
    
    SSDRAIN_MOTOR_FAULT_PARAMS_TYPE *ssdrain_motor_fault_params;
    FILL_VALVE_FAULT_PARAMS_TYPE *fill_valve_fault_params;
    DISPENSER_FAULT_PARAMS_TYPE *dispenser_fault_params;
    VENT_FAULT_PARAMS_TYPE *vent_fault_params;
    REGEN_FAULT_PARAMS_TYPE *regen_fault_params;

    diagnostic_current_gi_index = Hbl__GetGIIndexByGIID(HBL_GI_I_DIAGNOSTIC);
    drain_pomp_current_gi_index = Hbl__GetGIIndexByGIID(HBL_GI_I_DRAIN_PUMP);

    ssdrain_index = Hbl__GetLoadIndexByLoadID(HBL_LOAD_DRAIN_PUMP);
    fill_valve_index = Hbl__GetLoadIndexByLoadID(HBL_LOAD_FILL_VALVE);
    dispenser_index = Hbl__GetLoadIndexByLoadID(HBL_LOAD_DISPENSER);
    vent_index = Hbl__GetLoadIndexByLoadID(HBL_LOAD_VENT);
    regen_index = Hbl__GetLoadIndexByLoadID(HBL_LOAD_REGENERATION_VALVE);

    //Evaluate Vent/Regen load presence
    if (regen_index != HBL_INDEX_INVALID)
    {
        Vent_Regen_Load = REGEN_LOAD;
    }
    else
    {
        Vent_Regen_Load = VENT_LOAD;
    }

    //Evaluate drain pomp detection type
    if((diagnostic_current_gi_index != HBL_INDEX_INVALID) && (ssdrain_index != HBL_INDEX_INVALID))
    {
         if(HBL_PILOT_EXTERNAL_MOTOR == Hbl__GetPilotIDByIndex(ssdrain_index))
         {
             Drain_Fault_Detection = DRAIN_FAULT_DETECTION_EXTERNAL;
         }
         else
         {
            if(drain_pomp_current_gi_index != HBL_INDEX_INVALID)
            {
                Drain_Fault_Detection = DRAIN_FAULT_DETECTION_INDEPENDENT;
            }
            else
            {
                Drain_Fault_Detection = DRAIN_FAULT_DETECTION_ACCUMULATED;
            }
         }
    }
    else
    {
        EventSettingDataMissing();
        LOG_ADD_EXCEPTION (ERROR_MISSING_COMPONENTS, 0);
    }


    if ((diagnostic_current_gi_index != HBL_INDEX_INVALID) && (ssdrain_index != HBL_INDEX_INVALID) && (fill_valve_index != HBL_INDEX_INVALID) \
    && (dispenser_index != HBL_INDEX_INVALID) && ((vent_index != HBL_INDEX_INVALID) || (regen_index != HBL_INDEX_INVALID)))
    {
        ssdrain_motor_fault_params = (SSDRAIN_MOTOR_FAULT_PARAMS_TYPE*)Fault__GetSpecificData(FAULT_ID_SINGLE_SPEED_DRAIN_MOTOR);
        if (ssdrain_motor_fault_params != NULL)
        {
            SSDrain_Fault_Params.Current_Sense_SS_Drain_Min = ENDIANU16_STON(ssdrain_motor_fault_params->Current_Sense_SS_Drain_Min);
            SSDrain_Fault_Params.Current_Sense_SS_Drain_Max = ENDIANU16_STON(ssdrain_motor_fault_params->Current_Sense_SS_Drain_Max);
        }
        else
        {
            SSDrain_Fault_Params.Current_Sense_SS_Drain_Min = 0;
            SSDrain_Fault_Params.Current_Sense_SS_Drain_Max = 0;
            LOG_ADD_EXCEPTION (ERROR_MISSING_PARAMETERS, FAULT_ID_SINGLE_SPEED_DRAIN_MOTOR);
        }

        fill_valve_fault_params = (FILL_VALVE_FAULT_PARAMS_TYPE*)Fault__GetSpecificData(FAULT_ID_NO_WATER_FILL_VALVE_PROBLEM);
        if (fill_valve_fault_params != NULL)
        {
            Fill_Valve_Fault_Params.Current_Sense_Fill_Valve_Min = ENDIANU16_STON(fill_valve_fault_params->Current_Sense_Fill_Valve_Min);
            Fill_Valve_Fault_Params.Current_Sense_Fill_Valve_Max = ENDIANU16_STON(fill_valve_fault_params->Current_Sense_Fill_Valve_Max);
        }
        else
        {
            Fill_Valve_Fault_Params.Current_Sense_Fill_Valve_Min = 0;
            Fill_Valve_Fault_Params.Current_Sense_Fill_Valve_Max = 0;
            LOG_ADD_EXCEPTION (ERROR_MISSING_PARAMETERS, FAULT_ID_NO_WATER_FILL_VALVE_PROBLEM);
        }

        dispenser_fault_params = (DISPENSER_FAULT_PARAMS_TYPE*)Fault__GetSpecificData(FAULT_ID_DISPENSER_FAILURE);
        if (dispenser_fault_params != NULL)
        {
            Dispenser_Fault_Params.Current_Sense_Dispenser_Min = ENDIANU16_STON(dispenser_fault_params->Current_Sense_Dispenser_Min);
            Dispenser_Fault_Params.Current_Sense_Dispenser_Max = ENDIANU16_STON(dispenser_fault_params->Current_Sense_Dispenser_Max);
        }
        else
        {
            Dispenser_Fault_Params.Current_Sense_Dispenser_Min = 0;
            Dispenser_Fault_Params.Current_Sense_Dispenser_Max = 0;
            LOG_ADD_EXCEPTION (ERROR_MISSING_PARAMETERS, FAULT_ID_DISPENSER_FAILURE);
        }

        if (vent_index != HBL_INDEX_INVALID)
        {
            vent_fault_params = (VENT_FAULT_PARAMS_TYPE*)Fault__GetSpecificData(FAULT_ID_VENT_WAX_MOTOR_FAILURE);
            if(vent_fault_params != NULL)
            {
                Vent_Fault_Params.Current_Sense_Vent_Min = ENDIANU16_STON(vent_fault_params->Current_Sense_Vent_Min);
                Vent_Fault_Params.Current_Sense_Vent_Max = ENDIANU16_STON(vent_fault_params->Current_Sense_Vent_Max);
            }
            else
            {
                Vent_Fault_Params.Current_Sense_Vent_Min = 0;
                Vent_Fault_Params.Current_Sense_Vent_Max = 0;
                LOG_ADD_EXCEPTION (ERROR_MISSING_PARAMETERS, FAULT_ID_VENT_WAX_MOTOR_FAILURE);
            }
        }

        if (regen_index != HBL_INDEX_INVALID)
        {
            regen_fault_params = (REGEN_FAULT_PARAMS_TYPE*)Fault__GetSpecificData(FAULT_ID_REGENERATION_VALVE_FAILURE);
            if(regen_fault_params != NULL)
            {
                Regen_Fault_Params.Current_Sense_Regen_Min = ENDIANU16_STON(regen_fault_params->Current_Sense_Regen_Min);
                Regen_Fault_Params.Current_Sense_Regen_Max = ENDIANU16_STON(regen_fault_params->Current_Sense_Regen_Max);
            }
            else
            {
                Regen_Fault_Params.Current_Sense_Regen_Min = 0;
                Regen_Fault_Params.Current_Sense_Regen_Max = 0;
                LOG_ADD_EXCEPTION (ERROR_MISSING_PARAMETERS, FAULT_ID_VENT_WAX_MOTOR_FAILURE);
            }
        }
        EventStartMonitor();
    }
    else
    {
        EventSettingDataMissing();
        LOG_ADD_EXCEPTION (ERROR_MISSING_COMPONENTS, 0);
    }
    Triacs_Diagnostic.Max_Expected_Value = 0;
    Triacs_Diagnostic.Min_Expected_Value = 0;
    Triacs_Diagnostic.Temporary_Load_Sense_Status = TRIACS_DIAGNOSTIC_UNKNOWN;
    Triacs_Diagnostic.Load_Sense_Status = TRIACS_DIAGNOSTIC_UNKNOWN;
    Triacs_Diagnostic.Triac_Channel_Freeze = FALSE;
    Triacs_Diagnostic.Last_Changed_Triac_Output = HBL_LOAD_INVALID;
#if (TRIAC_DIAG_DEBUG == ENABLED)
    Debug_Current_Input_Triac = 0;
    Debug_Current_Input_Drain = 0;
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void ResetFailureDetection(void)
{
    Fault__ConditionRemoved(FAULT_ID_SINGLE_SPEED_DRAIN_MOTOR);
    Fault__ConditionRemoved(FAULT_ID_NO_WATER_FILL_VALVE_PROBLEM);
    Fault__ConditionRemoved(FAULT_ID_DISPENSER_FAILURE);
    Fault__ConditionRemoved(FAULT_ID_VENT_WAX_MOTOR_FAILURE);
    Fault__ConditionRemoved(FAULT_ID_REGENERATION_VALVE_FAILURE);
    Fault__ConditionRemoved(FAULT_ID_FILL_VALVE_TRIAC_OPEN);
    Fault__ConditionRemoved(FAULT_ID_DRAIN_MOTOR_TRIAC_OPEN);
    Fault__ConditionRemoved(FAULT_ID_DIVERTER_TRIAC_OPEN);
    Fault__ConditionRemoved(FAULT_ID_DISPENSER_TRIAC_OPEN);
    Fault__ConditionRemoved(FAULT_ID_REGENERATION_VALVE_TRIAC_OPEN);
    Fault__ConditionRemoved(FAULT_ID_VENT_VALVE_TRIAC_OPEN);
    CurrentSenseStatusPending = FALSE;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief returns lastly calculated diagnostic status for load_id, but ONLY if this load was activated most recently
 */
static TRIACS_DIAGNOSTIC_STATUS_TYPE TriacsDiagnosticGetStatus(HBL_LOAD_ENUM_TYPE load_id)
{
    TRIACS_DIAGNOSTIC_STATUS_TYPE triacs_diagnostic_status;

    triacs_diagnostic_status = TRIACS_DIAGNOSTIC_UNKNOWN;

    switch ( load_id )
    {
        case HBL_LOAD_DRAIN_PUMP:
        case HBL_LOAD_FILL_VALVE:
        case HBL_LOAD_DISPENSER:
        case HBL_LOAD_VENT:
        case HBL_LOAD_REGENERATION_VALVE:
            if ( load_id == Triacs_Diagnostic.Last_Changed_Triac_Output )
            {
                triacs_diagnostic_status = Triacs_Diagnostic.Load_Sense_Status;
            }
            break;
        case HBL_LOAD_DIVERTER: // load too small to reliably diagnose current
            if ( load_id == Triacs_Diagnostic.Last_Changed_Triac_Output )
            {
                triacs_diagnostic_status = TRIACS_DIAGNOSTIC_IN_RANGE;
            }
            break;
        default:
            break;
    }

    return triacs_diagnostic_status;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Handler responsible for monitoring observed loads states, in case of activation requests current range update, Timers handling
 */
static void TriacsDiagnosticHandler()
{
    uint8 *load_state_ptr;
    Diverter_Position_Type Current_Sense_Diverter_Position;

	load_state_ptr = (uint8*)(Hbl__GetLoadStatusByLoadID(HBL_LOAD_DRAIN_PUMP));
	if (load_state_ptr != NULL)
	{
		Next_Drain_Pump_Status = (load_state_ptr[0] == ON) ? ON:OFF;
	}
    if ((Next_Drain_Pump_Status != Triac_Diag_Drain_Pump_Status) && (DRAIN_FAULT_DETECTION_EXTERNAL != Drain_Fault_Detection))
    {
        Triac_Diag_Drain_Pump_Status = Next_Drain_Pump_Status;
        TriacsDiagnosticUpdateRange(HBL_LOAD_DRAIN_PUMP, Next_Drain_Pump_Status);
    }

	load_state_ptr = (uint8*)(Hbl__GetLoadStatusByLoadID(HBL_LOAD_FILL_VALVE));
	if (load_state_ptr != NULL)
	{
		Next_Fill_Valve_Status = (load_state_ptr[0] == ON) ? ON:OFF;
	}
    if (Next_Fill_Valve_Status != Triac_Diag_Fill_Valve_Status)
    {
        Triac_Diag_Fill_Valve_Status = Next_Fill_Valve_Status;
        TriacsDiagnosticUpdateRange(HBL_LOAD_FILL_VALVE, Next_Fill_Valve_Status);
    }

    
    Current_Sense_Diverter_Position = DishAuxVar__GetDiverterPosition();
    if (Current_Sense_Diverter_Position.Is_Stable == TRUE)
    {
        Next_Diverter_Status = OFF;
    }
    else
    {
        Next_Diverter_Status = ON;
    }

    if (Next_Diverter_Status != Triac_Diag_Diverter_Status)
    {
        Triac_Diag_Diverter_Status = Next_Diverter_Status;
        TriacsDiagnosticUpdateRange(HBL_LOAD_DIVERTER, Next_Diverter_Status);
    }

	load_state_ptr = (uint8*)(Hbl__GetLoadStatusByLoadID(HBL_LOAD_DISPENSER));
	if (load_state_ptr != NULL)
	{
		Next_Dispenser_Status = (load_state_ptr[0] == ON) ? ON:OFF;
	}
    if (Next_Dispenser_Status != Triac_Diag_Dispenser_Status)
    {
        Triac_Diag_Dispenser_Status = Next_Dispenser_Status;
        TriacsDiagnosticUpdateRange(HBL_LOAD_DISPENSER, Next_Dispenser_Status);
    }

    if (Vent_Regen_Load == VENT_LOAD)
    {
    	load_state_ptr = (uint8*)(Hbl__GetLoadStatusByLoadID(HBL_LOAD_VENT));
    	if (load_state_ptr != NULL)
    	{
			Next_Vent_Status = (load_state_ptr[0] == ON) ? ON:OFF;
    	}
        if (Next_Vent_Status != Triac_Diag_Vent_Status)
        {
            Triac_Diag_Vent_Status = Next_Vent_Status;
            TriacsDiagnosticUpdateRange(HBL_LOAD_VENT, Next_Vent_Status);
        }
    }
    else
    {
    	load_state_ptr = (uint8*)(Hbl__GetLoadStatusByLoadID(HBL_LOAD_REGENERATION_VALVE));
    	if (load_state_ptr != NULL)
    	{
			Next_Regeneration_Status = (load_state_ptr[0] == ON) ? ON:OFF;
    	}
        if (Next_Regeneration_Status != Triac_Diag_Regeneration_Status)
        {
            Triac_Diag_Regeneration_Status = Next_Regeneration_Status;
            TriacsDiagnosticUpdateRange(HBL_LOAD_REGENERATION_VALVE, Next_Regeneration_Status);
        }
    }

	load_state_ptr = (uint8*)(Hbl__GetLoadStatusByLoadID(HBL_LOAD_LOWER_TANK_VALVE));
	if (load_state_ptr != NULL)
	{
		Next_Lower_Tank_Status = (load_state_ptr[0] == ON) ? ON:OFF;
	}
    if (Next_Lower_Tank_Status != Triac_Diag_Lower_Tank_Status)
    {
        Triac_Diag_Lower_Tank_Status = Next_Lower_Tank_Status;
        TriacsDiagnosticUpdateRange(HBL_LOAD_INVALID, Next_Lower_Tank_Status);
    }

	load_state_ptr = (uint8*)(Hbl__GetLoadStatusByLoadID(HBL_LOAD_UPPER_TANK_VALVE));
	if (load_state_ptr != NULL)
	{
		Next_Upper_Tank_Status = (load_state_ptr[0] == ON) ? ON:OFF;
	}
    if (Next_Upper_Tank_Status != Triac_Diag_Upper_Tank_Status)
    {
        Triac_Diag_Upper_Tank_Status = Next_Upper_Tank_Status;
        TriacsDiagnosticUpdateRange(HBL_LOAD_INVALID, Next_Upper_Tank_Status);
    }

    if (Triac_Ref_Timer > 0)
    {
        Triac_Ref_Timer--;
    }
    if (Triac_Debounce_Timer > 0)
    {
        Triac_Debounce_Timer--;
    }

}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief based on information which and how load has changed sets allowed current range (min, max)
 */
static void TriacsDiagnosticUpdateRange(HBL_LOAD_ENUM_TYPE output, ON_OFF_TYPE new_digital_output_state)
{
    uint16 diag_signal_min = 0;
    uint16 diag_signal_max = 0;

    if (ON == new_digital_output_state)
    {
        CurrentSenseStatusPending = TRUE;
        Triacs_Diagnostic.Last_Changed_Triac_Output = output;
    }
    else
    {
        CurrentSenseStatusPending = TRUE;
        Triacs_Diagnostic.Last_Changed_Triac_Output = HBL_LOAD_INVALID;
    }

    if (0 == Triac_Ref_Timer)
    {
        // Allow update triac reference after reference timer is completed
        TriacSetRef();
    }
    else if (TRUE == Triacs_Diagnostic.Triac_Channel_Freeze )
    {
        //When output is changed too fast or in the same time
        //than diagnostic measure is disabled for this outputs - set measure values to 0
        Triacs_Diagnostic.Max_Expected_Value = 0;
        Triacs_Diagnostic.Min_Expected_Value = 0;
    }

    // Start counting blind time for reference set
    if ((HBL_LOAD_VENT == Triacs_Diagnostic.Last_Changed_Triac_Output) && (ON == new_digital_output_state))
    {
        Triac_Ref_Timer = TRIACS_DIAGNOSTIC_VAX_DEBOUNCE_100MS;
    }
    else
    {
        Triac_Ref_Timer = TRIACS_DIAGNOSTIC_DEBOUNCE_100MS;
    }

    //Reset last diagnostic measure result
    Triacs_Diagnostic.Load_Sense_Status = TRIACS_DIAGNOSTIC_UNKNOWN;

    if (output != HBL_LOAD_INVALID)
    {
        if (HBL_LOAD_DRAIN_PUMP == output)
        {
            diag_signal_min = SSDrain_Fault_Params.Current_Sense_SS_Drain_Min;
            diag_signal_max = SSDrain_Fault_Params.Current_Sense_SS_Drain_Max;
        }
        else if (HBL_LOAD_FILL_VALVE == output)
        {
            diag_signal_min = Fill_Valve_Fault_Params.Current_Sense_Fill_Valve_Min;
            diag_signal_max = Fill_Valve_Fault_Params.Current_Sense_Fill_Valve_Max;
        }
        else if (HBL_LOAD_DISPENSER == output)
        {
            diag_signal_min = Dispenser_Fault_Params.Current_Sense_Dispenser_Min;
            diag_signal_max = Dispenser_Fault_Params.Current_Sense_Dispenser_Max;
        }
        else if (HBL_LOAD_VENT == output)
        {
            diag_signal_min = Vent_Fault_Params.Current_Sense_Vent_Min;
            diag_signal_max = Vent_Fault_Params.Current_Sense_Vent_Max;
        }
        else if (HBL_LOAD_REGENERATION_VALVE == output)
        {
            diag_signal_min = Regen_Fault_Params.Current_Sense_Regen_Min;
            diag_signal_max = Regen_Fault_Params.Current_Sense_Regen_Max;
        }
        else
        {
            diag_signal_min = 0;
            diag_signal_max = 0;
        }

        //Set range value for new digital output
        if ( ON == new_digital_output_state )
        {
            if ( FALSE == Triacs_Diagnostic.Triac_Channel_Freeze )
            {
                if ((uint32)(Triacs_Diagnostic.Max_Expected_Value + diag_signal_max) < 0xFFFF)
                {
                    Triacs_Diagnostic.Max_Expected_Value += diag_signal_max;
                }
                else
                {
                    Triacs_Diagnostic.Max_Expected_Value = 0xFFFF;
                }
                if ((uint32)(Triacs_Diagnostic.Min_Expected_Value + diag_signal_min) < 0xFFFF)
                {
                    Triacs_Diagnostic.Min_Expected_Value += diag_signal_min;
                }
                else
                {
                    Triacs_Diagnostic.Min_Expected_Value = 0xFFFF;
                }
                Triacs_Diagnostic.Triac_Channel_Freeze = TRUE;
            }
        }
        else
        {
            if ( FALSE == Triacs_Diagnostic.Triac_Channel_Freeze )
            {
                if ( Triacs_Diagnostic.Max_Expected_Value > diag_signal_min )
                {
                    //Set range for relative channel value
                    Triacs_Diagnostic.Max_Expected_Value -= diag_signal_min;
                }
                else
                {
                    // Disable check when reference is incorrect
                    Triacs_Diagnostic.Max_Expected_Value = 0;
                }
                if ( Triacs_Diagnostic.Min_Expected_Value > diag_signal_max )
                {
                    Triacs_Diagnostic.Min_Expected_Value -= diag_signal_max;
                }
                else
                {
                    // Disable check when reference is incorrect
                    Triacs_Diagnostic.Min_Expected_Value = 0;
                }
                Triacs_Diagnostic.Triac_Channel_Freeze = TRUE;
            }
        }
    }
    if ( 0 == diag_signal_min ) // When diag_signal_min is 0 diagnostic is disabled
    {
        Triacs_Diagnostic.Max_Expected_Value = 0;
        Triacs_Diagnostic.Min_Expected_Value = 0;
    }
    Triac_Debounce_Timer = TRIACS_DIAGNOSTIC_DEBOUNCE_100MS;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief load changed when current is stable - time to remember it and set current range reference
 */
static void TriacSetRef(void)
{
    // set baseline according to readout before output change happen
    Triacs_Diagnostic.Triac_Channel_Freeze = FALSE;

    if((Drain_Fault_Detection == DRAIN_FAULT_DETECTION_INDEPENDENT) &&
       (Triacs_Diagnostic.Last_Changed_Triac_Output == HBL_LOAD_DRAIN_PUMP))
    {
        Triacs_Diagnostic.Max_Expected_Value = Current_Sense_Pump;  //measured_current;
        Triacs_Diagnostic.Min_Expected_Value = Current_Sense_Pump;  //measured_current;
    }
    else
    {
        if ((Triacs_Diagnostic.Last_Changed_Triac_Output == HBL_LOAD_VENT)
            && (Current_Sense_Delay_Needed != DELAY_0))
        {
            Triacs_Diagnostic.Max_Expected_Value = Current_Sense_Diagnostic[Current_Sense_Delay_Needed];    // historical measured current;
            Triacs_Diagnostic.Min_Expected_Value = Current_Sense_Diagnostic[Current_Sense_Delay_Needed];    // historical measured current;
        }
        else
        {
            Triacs_Diagnostic.Max_Expected_Value = Current_Sense_Diagnostic[DELAY_0];   // measured current;
            Triacs_Diagnostic.Min_Expected_Value = Current_Sense_Diagnostic[DELAY_0];   // measured current;
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  reads current, check against allowed range and do basic diagnostic status filtering
 */
void DiagnosticCheckIfInRange(void)
{
    uint16 measured_current;
    // store historical samples
    Current_Sense_Diagnostic[DELAY_4] = Current_Sense_Diagnostic[DELAY_3];
    Current_Sense_Diagnostic[DELAY_3] = Current_Sense_Diagnostic[DELAY_2];
    Current_Sense_Diagnostic[DELAY_2] = Current_Sense_Diagnostic[DELAY_1];
    Current_Sense_Diagnostic[DELAY_1] = Current_Sense_Diagnostic[DELAY_0];
#if (TRIAC_DIAG_DEBUG == ENABLED)
    Current_Sense_Diagnostic[DELAY_0] = Debug_Current_Input_Triac;
#else
    Current_Sense_Diagnostic[DELAY_0] = (uint16)Hbl__GetGIDataConvertedByIndex(Hbl__GetGIIndexByGIID(HBL_GI_I_DIAGNOSTIC));
#endif


    if(Drain_Fault_Detection == DRAIN_FAULT_DETECTION_INDEPENDENT)
    {
#if (TRIAC_DIAG_DEBUG == ENABLED)
        Current_Sense_Pump = Debug_Current_Input_Drain;
    }
    else
    {
        Current_Sense_Pump = Next_Drain_Pump_Status + Next_Fill_Valve_Status + Next_Diverter_Status + Next_Dispenser_Status + Next_Vent_Status + Next_Regeneration_Status;
#else
        Current_Sense_Pump = (uint16)Hbl__GetGIDataConvertedByIndex(Hbl__GetGIIndexByGIID(HBL_GI_I_DRAIN_PUMP));
#endif
    }

    if((Drain_Fault_Detection == DRAIN_FAULT_DETECTION_INDEPENDENT) &&
       (Triacs_Diagnostic.Last_Changed_Triac_Output == HBL_LOAD_DRAIN_PUMP))
    {
        measured_current = Current_Sense_Pump;
    }
    else //(Drain_Fault_Detection == DRAIN_FAULT_DETECTION_ACCUMULATED)
    {
        measured_current = Current_Sense_Diagnostic[DELAY_0];
    }


    // First check if max and min expected values are set
    if ( Triacs_Diagnostic.Max_Expected_Value > Triacs_Diagnostic.Min_Expected_Value)
    {
        // Then check if filtered value is in range
        if ( (measured_current >= Triacs_Diagnostic.Min_Expected_Value) &&
             (measured_current <= Triacs_Diagnostic.Max_Expected_Value) )
        {
            if( (TRIACS_DIAGNOSTIC_UNKNOWN == Triacs_Diagnostic.Temporary_Load_Sense_Status) || (TRIACS_DIAGNOSTIC_NOT_IN_RANGE == Triacs_Diagnostic.Temporary_Load_Sense_Status))
            {
                Triac_Debounce_Timer = TRIACS_DIAGNOSTIC_DEBOUNCE_100MS;
                Triacs_Diagnostic.Load_Sense_Status = TRIACS_DIAGNOSTIC_UNKNOWN;
                Triacs_Diagnostic.Temporary_Load_Sense_Status = TRIACS_DIAGNOSTIC_IN_RANGE;
            }
        }
        else
        {
            if( (TRIACS_DIAGNOSTIC_UNKNOWN == Triacs_Diagnostic.Temporary_Load_Sense_Status) || (TRIACS_DIAGNOSTIC_IN_RANGE == Triacs_Diagnostic.Temporary_Load_Sense_Status))
            {
                Triac_Debounce_Timer = TRIACS_DIAGNOSTIC_DEBOUNCE_100MS;
                Triacs_Diagnostic.Load_Sense_Status = TRIACS_DIAGNOSTIC_UNKNOWN;
                Triacs_Diagnostic.Temporary_Load_Sense_Status = TRIACS_DIAGNOSTIC_NOT_IN_RANGE;
            }
        }

        if ((0 == Triac_Debounce_Timer) && (0 == Triac_Ref_Timer) && (Triacs_Diagnostic.Load_Sense_Status != Triacs_Diagnostic.Temporary_Load_Sense_Status))
        {
            Triacs_Diagnostic.Load_Sense_Status = Triacs_Diagnostic.Temporary_Load_Sense_Status;
        }
    }

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void CheckFailureRemoval(void)
{
    switch(Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F9_E2_DETECTED:
            if (Fault__IsInFaultState(FAULT_ID_SINGLE_SPEED_DRAIN_MOTOR) == TRUE)
            {
                EventDebounceCompletedF9E2();
            }
            else if ((Fault__IsObserverActive(FAULT_ID_SINGLE_SPEED_DRAIN_MOTOR) == FALSE)
                  || (TRIACS_DIAGNOSTIC_IN_RANGE == TriacsDiagnosticGetStatus(HBL_LOAD_DRAIN_PUMP)))
            {
                EventNoFailureF9E2Present();
            }
            else
            {
                // nothing to do
            }
            break;
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F8_E2_DETECTED:
            if (Fault__IsInFaultState(FAULT_ID_NO_WATER_FILL_VALVE_PROBLEM) == TRUE)
            {
                EventDebounceCompletedF8E2();
            }
            else if ((Fault__IsObserverActive(FAULT_ID_NO_WATER_FILL_VALVE_PROBLEM) == FALSE)
                  || (TRIACS_DIAGNOSTIC_IN_RANGE == TriacsDiagnosticGetStatus(HBL_LOAD_FILL_VALVE)))
            {
                EventNoFailureF8E2Present();
            }
            else
            {
                // nothing to do
            }
            break;
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10_E1_DETECTED:
            if (Fault__IsInFaultState(FAULT_ID_DISPENSER_FAILURE) == TRUE)
            {
                EventDebounceCompletedF10E1();
            }
            else if ((Fault__IsObserverActive(FAULT_ID_DISPENSER_FAILURE) == FALSE)
                  || (TRIACS_DIAGNOSTIC_IN_RANGE == TriacsDiagnosticGetStatus(HBL_LOAD_DISPENSER)))
            {
                EventNoFailureF10E1Present();
            }
            else
            {
                // nothing to do
            }
            break;
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10_E2_F8_E7_DETECTED:
            if (   ((Vent_Regen_Load == REGEN_LOAD) && (Fault__IsInFaultState(FAULT_ID_REGENERATION_VALVE_FAILURE) == TRUE))
                || ((Vent_Regen_Load == VENT_LOAD) && (Fault__IsInFaultState(FAULT_ID_VENT_WAX_MOTOR_FAILURE) == TRUE)))
            {
                EventDebounceCompletedF10E2F8E7();
            }
            else if (((Vent_Regen_Load == REGEN_LOAD)
                    && ((Fault__IsObserverActive(FAULT_ID_REGENERATION_VALVE_FAILURE) == FALSE)
                            || (TRIACS_DIAGNOSTIC_IN_RANGE == TriacsDiagnosticGetStatus(HBL_LOAD_REGENERATION_VALVE))))
                 || ((Vent_Regen_Load == VENT_LOAD)
                    && ((Fault__IsObserverActive(FAULT_ID_VENT_WAX_MOTOR_FAILURE) == FALSE)
                            || (TRIACS_DIAGNOSTIC_IN_RANGE == TriacsDiagnosticGetStatus(HBL_LOAD_VENT)))))
            {
                EventNoFailureF10E2F8E7Present();
            }
            else
            {
                // nothing to do
            }
            break;

        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_0_DETECTED:
            if (Fault__IsInFaultState(FAULT_ID_FILL_VALVE_TRIAC_OPEN) == TRUE)
            {
                EventDebounceCompletedF1E50();
            }
            else if ((Fault__IsObserverActive(FAULT_ID_FILL_VALVE_TRIAC_OPEN) == FALSE)
                    || ((TRIACS_DIAGNOSTIC_IN_RANGE == TriacsDiagnosticGetStatus(HBL_LOAD_FILL_VALVE))
                    && (TRIAC_LOADS_VOLTAGE_SENSOR_DECLINED != GET_VOLTAGE_SENSOR_STATUS)))
            {
                EventNoFailureF1E50Present();
            }
            else
            {
                // nothing to do
            }
            break;

        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_4_DETECTED:
            if (Fault__IsInFaultState(FAULT_ID_DRAIN_MOTOR_TRIAC_OPEN) == TRUE)
            {
                EventDebounceCompletedF1E54();
            }
            else if ((Fault__IsObserverActive(FAULT_ID_DRAIN_MOTOR_TRIAC_OPEN) == FALSE)
                    || ((TRIACS_DIAGNOSTIC_IN_RANGE == TriacsDiagnosticGetStatus(HBL_LOAD_DRAIN_PUMP))
                    && (TRIAC_LOADS_VOLTAGE_SENSOR_DECLINED != GET_VOLTAGE_SENSOR_STATUS)))
            {
                EventNoFailureF1E54Present();
            }
            else
            {
                // nothing to do
            }
            break;
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_5_DETECTED:
            if (Fault__IsInFaultState(FAULT_ID_DIVERTER_TRIAC_OPEN) == TRUE)
            {
                EventDebounceCompletedF1E55();
            }
            else if ((Fault__IsObserverActive(FAULT_ID_DIVERTER_TRIAC_OPEN) == FALSE)
                    || ((TRIACS_DIAGNOSTIC_IN_RANGE == TriacsDiagnosticGetStatus(HBL_LOAD_DIVERTER))
                    && (TRIAC_LOADS_VOLTAGE_SENSOR_DECLINED != GET_VOLTAGE_SENSOR_STATUS)))
            {
                EventNoFailureF1E55Present();
            }
            else
            {
                // nothing to do
            }
            break;
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_1_DETECTED:
            if (Fault__IsInFaultState(FAULT_ID_DISPENSER_TRIAC_OPEN) == TRUE)
            {
                EventDebounceCompletedF1E51();
            }
            else if ((Fault__IsObserverActive(FAULT_ID_DISPENSER_TRIAC_OPEN) == FALSE)
                    || ((TRIACS_DIAGNOSTIC_IN_RANGE == TriacsDiagnosticGetStatus(HBL_LOAD_DISPENSER))
                    && (TRIAC_LOADS_VOLTAGE_SENSOR_DECLINED != GET_VOLTAGE_SENSOR_STATUS)))
            {
                EventNoFailureF1E51Present();
            }
            else
            {
                // nothing to do
            }
            break;
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_2_3_DETECTED:
            if (   ((Vent_Regen_Load == REGEN_LOAD) && (Fault__IsInFaultState(FAULT_ID_REGENERATION_VALVE_TRIAC_OPEN) == TRUE))
                || ((Vent_Regen_Load == VENT_LOAD) && (Fault__IsInFaultState(FAULT_ID_VENT_VALVE_TRIAC_OPEN) == TRUE)))
            {
                EventDebounceCompletedF1E523();
            }
            else if (((Vent_Regen_Load == REGEN_LOAD)
                    && ((Fault__IsObserverActive(FAULT_ID_REGENERATION_VALVE_TRIAC_OPEN) == FALSE)
                            || ((TRIACS_DIAGNOSTIC_IN_RANGE == TriacsDiagnosticGetStatus(HBL_LOAD_REGENERATION_VALVE))
                                    && (TRIAC_LOADS_VOLTAGE_SENSOR_DECLINED != GET_VOLTAGE_SENSOR_STATUS))))
                 || ((Vent_Regen_Load == VENT_LOAD)
                    && ((Fault__IsObserverActive(FAULT_ID_VENT_VALVE_TRIAC_OPEN) == FALSE)
                            || ((TRIACS_DIAGNOSTIC_IN_RANGE == TriacsDiagnosticGetStatus(HBL_LOAD_VENT))
                                    && (TRIAC_LOADS_VOLTAGE_SENSOR_DECLINED != GET_VOLTAGE_SENSOR_STATUS)))))
            {
                EventNoFailureF1E523Present();
            }
            else
            {
                // nothing to do
            }
            break;
        default:
            // There is nothing to do for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }

}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetFailureF10E1Detection(void)
{
    Fault__ConditionDetected(FAULT_ID_DISPENSER_FAILURE);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetFailureF10E2F8E7Detection(void)
{
    if (Vent_Regen_Load == REGEN_LOAD)
    {
        Fault__ConditionDetected(FAULT_ID_REGENERATION_VALVE_FAILURE);
    }
    else
    {
        Fault__ConditionDetected(FAULT_ID_VENT_WAX_MOTOR_FAILURE);
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetFailureF8E2Detection(void)
{
    Fault__ConditionDetected(FAULT_ID_NO_WATER_FILL_VALVE_PROBLEM);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetFailureF9E2Detection(void)
{
    Fault__ConditionDetected(FAULT_ID_SINGLE_SPEED_DRAIN_MOTOR);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetFailureF1E54Detection(void)
{
    Fault__ConditionDetected(FAULT_ID_DRAIN_MOTOR_TRIAC_OPEN);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetFailureF1E50Detection(void)
{
    Fault__ConditionDetected(FAULT_ID_FILL_VALVE_TRIAC_OPEN);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetFailureF1E55Detection(void)
{
    Fault__ConditionDetected(FAULT_ID_DIVERTER_TRIAC_OPEN);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetFailureF1E51Detection(void)
{
    Fault__ConditionDetected(FAULT_ID_DISPENSER_TRIAC_OPEN);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetFailureF1E523Detection(void)
{
    if (Vent_Regen_Load == REGEN_LOAD)
    {
        Fault__ConditionDetected(FAULT_ID_REGENERATION_VALVE_TRIAC_OPEN);
    }
    else
    {
        Fault__ConditionDetected(FAULT_ID_VENT_VALVE_TRIAC_OPEN);
    }
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Current Sense Fault Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Current Sense Fault Monitor state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void CurrentSenseFaultMonitorExecute(void)
{
    switch(Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_NO_FAILURE:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F9E2:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F8E2:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10E1:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10_E2_F8E7:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_0:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_4:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_5:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_1:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_2_3:
            CheckForFailure();                      // Do function.
            break;

        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F9_E2_DETECTED:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F8_E2_DETECTED:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10_E1_DETECTED:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10_E2_F8_E7_DETECTED:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_0_DETECTED:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_4_DETECTED:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_5_DETECTED:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_1_DETECTED:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_2_3_DETECTED:
            CheckFailureRemoval();                  // Do function.
            break;

        default:
            // There is nothing to do for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Current Sense Fault Monitor state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void CurrentSenseFaultMonitorInitialize(void)
{
    Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_INITIALIZE;
    InitializeModule();                             // Entry Function.
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Debounce Completed F10E1 Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDebounceCompletedF10E1(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10_E1_DETECTED:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10E1;
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Debounce Completed F10 E2 F8E7 Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDebounceCompletedF10E2F8E7(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10_E2_F8_E7_DETECTED:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10_E2_F8E7;
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Debounce Completed F1 E5 0 Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDebounceCompletedF1E50(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_0_DETECTED:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_0;
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Debounce Completed F1 E5 1 Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDebounceCompletedF1E51(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_1_DETECTED:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_1;
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Debounce Completed F1 E5 2 3 Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDebounceCompletedF1E523(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_2_3_DETECTED:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_2_3;
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Debounce Completed F1 E5 4 Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDebounceCompletedF1E54(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_4_DETECTED:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_4;
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Debounce Completed F1 E5 5 Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDebounceCompletedF1E55(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_5_DETECTED:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_5;
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Debounce Completed F8E2 Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDebounceCompletedF8E2(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F8_E2_DETECTED:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F8E2;
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Debounce Completed F9E2 Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDebounceCompletedF9E2(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F9_E2_DETECTED:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F9E2;
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the F10E1 Failure Detected Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventF10E1FailureDetected(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_NO_FAILURE:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F9E2:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F8E2:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10_E2_F8E7:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_0:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_4:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_5:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_1:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_2_3:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10_E1_DETECTED;
            SetFailureF10E1Detection();             // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the F10E2F8E7 Failure Detected Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventF10E2F8E7FailureDetected(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_NO_FAILURE:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F9E2:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F8E2:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10E1:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_0:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_4:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_5:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_1:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_2_3:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10_E2_F8_E7_DETECTED;
            SetFailureF10E2F8E7Detection();         // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the F1 E5 0 Failure Detected Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventF1E50FailureDetected(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_NO_FAILURE:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F9E2:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F8E2:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10E1:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10_E2_F8E7:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_4:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_5:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_1:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_2_3:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_0_DETECTED;
            SetFailureF1E50Detection();             // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the F1 E5 1 Failure Detected Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventF1E51FailureDetected(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_NO_FAILURE:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F9E2:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F8E2:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10E1:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10_E2_F8E7:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_0:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_4:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_5:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_2_3:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_1_DETECTED;
            SetFailureF1E51Detection();             // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the F1 E5 2 3 Failure Detected Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventF1E523FailureDetected(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_NO_FAILURE:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F9E2:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F8E2:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10E1:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10_E2_F8E7:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_0:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_4:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_5:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_1:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_2_3_DETECTED;
            SetFailureF1E523Detection();            // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the F1 E5 4 Failure Detected Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventF1E54FailureDetected(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_NO_FAILURE:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F9E2:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F8E2:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10E1:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10_E2_F8E7:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_0:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_5:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_1:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_2_3:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_4_DETECTED;
            SetFailureF1E54Detection();             // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the F1 E5 5 Failure Detected Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventF1E55FailureDetected(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_NO_FAILURE:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F9E2:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F8E2:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10E1:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10_E2_F8E7:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_0:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_4:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_1:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_2_3:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_5_DETECTED;
            SetFailureF1E55Detection();             // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the F8E2 Failure Detected Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventF8E2FailureDetected(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_NO_FAILURE:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F9E2:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10E1:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10_E2_F8E7:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_0:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_4:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_5:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_1:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_2_3:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F8_E2_DETECTED;
            SetFailureF8E2Detection();              // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the F9E2 Failure Detected Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventF9E2FailureDetected(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_NO_FAILURE:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F8E2:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10E1:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10_E2_F8E7:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_0:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_4:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_5:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_1:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_2_3:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F9_E2_DETECTED;
            SetFailureF9E2Detection();              // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Fault Reset Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventFaultReset(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F9_E2_DETECTED:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F8_E2_DETECTED:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10_E1_DETECTED:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10_E2_F8_E7_DETECTED:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_0_DETECTED:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_4_DETECTED:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_5_DETECTED:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_1_DETECTED:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_2_3_DETECTED:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F9E2:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F8E2:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10E1:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10_E2_F8E7:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_0:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_4:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_5:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_1:
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_2_3:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_NO_FAILURE;
            ResetFailureDetection();                // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the No Failure F10E1 Present Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNoFailureF10E1Present(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10_E1_DETECTED:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_NO_FAILURE;
            ResetFailureDetection();                // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the No Failure F10E2F8E7 Present Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNoFailureF10E2F8E7Present(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F10_E2_F8_E7_DETECTED:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_NO_FAILURE;
            ResetFailureDetection();                // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the No Failure F1 E5 0 Present Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNoFailureF1E50Present(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_0_DETECTED:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_NO_FAILURE;
            ResetFailureDetection();                // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the No Failure F1 E5 1 Present Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNoFailureF1E51Present(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_1_DETECTED:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_NO_FAILURE;
            ResetFailureDetection();                // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the No Failure F1 E5 2 3 Present Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNoFailureF1E523Present(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_2_3_DETECTED:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_NO_FAILURE;
            ResetFailureDetection();                // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the No Failure F1 E5 4 Present Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNoFailureF1E54Present(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_4_DETECTED:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_NO_FAILURE;
            ResetFailureDetection();                // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the No Failure F1 E5 5 Present Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNoFailureF1E55Present(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F1_E5_5_DETECTED:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_NO_FAILURE;
            ResetFailureDetection();                // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the No Failure F8E2 Present Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNoFailureF8E2Present(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F8_E2_DETECTED:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_NO_FAILURE;
            ResetFailureDetection();                // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the No Failure F9E2 Present Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNoFailureF9E2Present(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_FAULT_F9_E2_DETECTED:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_NO_FAILURE;
            ResetFailureDetection();                // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Setting Data Missing Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventSettingDataMissing(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_INITIALIZE:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_NONE; // Terminate Current Sense Fault Monitor state machine execution.
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Start Monitor Event for the Current Sense Fault Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventStartMonitor(void)
{
    switch (Current_Sense_Fault_Monitor_State)
    {
        case STATE_CURRENT_SENSE_FAULT_MONITOR_INITIALIZE:
            Current_Sense_Fault_Monitor_State = STATE_CURRENT_SENSE_FAULT_MONITOR_NO_FAILURE;
            ResetFailureDetection();                // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Current_Sense_Fault_Monitor_State.
            break;
    }
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

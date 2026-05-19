/**
 *  @file
 *
 *  @brief      Supervises state transitions among the different operation modes of the product:
 *                  Programming, Run, Delay, END, PAUSE, FAULT, DELAY-PAUSE and Cancelling
 *
 *  @details

    @startuml{Supervisor_SQ.png}
        title Supervisor Sequence Diagram
        scale max 4096 height
        autonumber

        actor User
        participant HMI
        participant Regulations
        participant Status
        participant Supervisor

        loop 100 milliseconds (embOS AppMgr Task call rate)
            alt Supervisor_Data[].State == FSM_STATE_PROGRAMMING
                User -> HMI : Select Delay (Start/Stop) Time
                HMI -> Regulations : SetRequestedIndex(FUNC_ID_DELAY_HH, value)
                HMI -> Regulations : SetRequestedIndex(FUNC_ID_DELAY_MM, value)
                HMI -> Regulations : SetRequestedIndex(FUNC_ID_DELAY_SS, value)
                note right : Any or all 3 regulations must be sent at the same time
                loop function = FUNC_ID_DELAY_HH, FUNC_ID_DELAY_MM and FUNC_ID_DELAY_SS
                    Supervisor -> Regulations : GetValue(function)
                    Regulations --> Supervisor: value = hours, minutes or seconds
                    Supervisor -> Supervisor : Delay_Time = hours * 3600 + minutes * 60 + seconds
                end
                alt
                    User -> HMI : Select Start
                    HMI -> Regulations : SetRequestedIndex(FUNC_ID_START, FS_START_CONDITION)
                    Supervisor -> Regulations : GetValue(FUNC_ID_START)
                    Regulations --> Supervisor: value = FS_START_CONDITION
                    alt Delay_Time == 0 && Selector != 0 && OK_TO_START
                        Supervisor -> Supervisor : eventStart
                        note right : Causes Supervisor to go to RUN state.
                    else Delay_Time != 0 && Selector != 0
                        Supervisor -> Supervisor : eventDelay
                        note right : Causes Supervisor to go to DELAY state.
                    end
                else
                    User -> HMI : Select Cancel
                    HMI -> Regulations : SetRequestedIndex(FUNC_ID_START, FS_RESET_CONDITION)
                    Supervisor -> Regulations : GetValue(FUNC_ID_START)
                    Regulations --> Supervisor: value = FS_RESET_CONDITION
                    Supervisor -> Supervisor : eventReset, Delay_Time = 0
                    note right : Supervisor stays in PROGRAMMING state.
                end
            else Supervisor_Data[].State == FSM_STATE_DELAY
                alt
                    User -> HMI : Select Start
                    HMI -> Regulations : SetRequestedIndex(FUNC_ID_START, FS_RESTART_CONDITION)
                    Supervisor -> Regulations : GetValue(FUNC_ID_START)
                    Regulations --> Supervisor: value = FS_RESTART_CONDITION
                    Supervisor -> Supervisor : [OK_TO_START] eventStart, set Delay_Time to 0
                    note right : Causes Supervisor to go to RUN state.
                else
                    User -> HMI : Select Cancel
                    HMI -> Regulations : SetRequestedIndex(FUNC_ID_START, FS_RESET_CONDITION)
                    Supervisor -> Regulations : GetValue(FUNC_ID_START)
                    Regulations --> Supervisor: value = FS_RESET_CONDITION
                    Supervisor -> Supervisor : eventReset, set Delay_Time to 0
                    note right : Causes Supervisor to go to PROGRAMMING state.
                else
                    User -> HMI : Select Delay (Start/Stop) Time
                    HMI -> Regulations : SetRequestedIndex(FUNC_ID_DELAY_HH, value)
                    HMI -> Regulations : SetRequestedIndex(FUNC_ID_DELAY_MM, value)
                    HMI -> Regulations : SetRequestedIndex(FUNC_ID_DELAY_SS, value)
                    note right : Any or all 3 regulations must be sent at the same time
                    loop function = FUNC_ID_DELAY_HH, FUNC_ID_DELAY_MM and FUNC_ID_DELAY_SS
                        Supervisor -> Regulations : GetValue(function)
                        Regulations --> Supervisor: value = hours, minutes or seconds
                        Supervisor -> Supervisor : Delay_Time = hours * 3600 + minutes * 60 + seconds
                        note right : If Delay_Time set to 0, eventStart will be generated.
                    end
                else
                    User -> HMI : Set Pause
                    HMI -> Regulations : SetRequestedIndex(FUNC_ID_START, FS_PAUSE_CONDITION)
                    Supervisor -> Regulations : GetValue(FUNC_ID_START)
                    Regulations --> Supervisor: value = FS_PAUSE_CONDITION
                    Supervisor --> Supervisor: EventPause
                    note right : Causes Supervisor to go to DELAY_PAUSE state which reports PAUSE state and runs PAUSE cycle
                end
                Supervisor -> Supervisor : Decrement Delay_Time every 10 loops (1 second)
                Status -> HMI : Delay_Time
                alt Delay_Time == 0 && OK_TO_START
                    Supervisor -> Supervisor : eventStart
                    note right : Causes Supervisor to go to RUN state.
                end
            else Supervisor_Data[].State == FSM_STATE_DELAY_PAUSE
                Supervisor --> Supervisor : Run Pause Cycle and report PAUSE state
                alt
                    User -> HMI : Select Delay (Start/Stop) Time
                    HMI -> Regulations : SetRequestedIndex(FUNC_ID_DELAY_HH, value)
                    HMI -> Regulations : SetRequestedIndex(FUNC_ID_DELAY_MM, value)
                    HMI -> Regulations : SetRequestedIndex(FUNC_ID_DELAY_SS, value)
                    note right : Any or all 3 regulations must be sent at the same time
                    loop function = FUNC_ID_DELAY_HH, FUNC_ID_DELAY_MM and FUNC_ID_DELAY_SS
                        Supervisor -> Regulations : GetValue(function)
                        Regulations --> Supervisor: value = hours, minutes or seconds
                        Supervisor -> Supervisor : Delay_Time = hours * 3600 + minutes * 60 + seconds
                        note right : If Delay_Time set to 0, eventStart will be generated when pause is resumed.
                    end
                else
                    User -> HMI : Select Start
                    HMI -> Regulations : SetRequestedIndex(FUNC_ID_START, FS_START_CONDITION)
                    Supervisor -> Regulations : GetValue(FUNC_ID_START)
                    Regulations --> Supervisor: value = FS_START_CONDITION
                    alt OK_TO_START
                        Supervisor -> Supervisor : eventResume
                    end
                    note right : Causes Supervisor to go back to Delay state.
                else
                    User -> HMI : Select Cancel
                    HMI -> Regulations : SetRequestedIndex(FUNC_ID_START, FS_RESET_CONDITION)
                    Supervisor -> Regulations : GetValue(FUNC_ID_START)
                    Regulations --> Supervisor: value = FS_RESET_CONDITION
                    Supervisor -> Supervisor : eventReset, set Delay_Time to 0
                    note right : Causes Supervisor to go to PROGRAMMING state.
                end
            else Supervisor_Data[].State == FSM_STATE_RUN
                alt
                    User -> HMI : Select Cancel
                    HMI -> Regulations : SetRequestedIndex(FUNC_ID_START, FS_RESET_CONDITION)
                    Supervisor -> Regulations : GetValue(FUNC_ID_START)
                    Regulations --> Supervisor: value = FS_RESET_CONDITION
                    Supervisor -> Supervisor : eventReset, ResetTasks
                    note right
                        If (allowed RUN to CANCELLING transition) AND (Cancelling cycle can start)
                        Supervisor goes to CANCELLING state
                        else
                        Supervisor goes to PROGRAMMING state.
                    end note
                end
            else Supervisor_Data[].State == FSM_STATE_END_OF_CYCLE
                alt
                    User -> HMI : Select Cancel
                    HMI -> Regulations : SetRequestedIndex(FUNC_ID_START, FS_RESET_CONDITION)
                    Supervisor -> Regulations : GetValue(FUNC_ID_START)
                    Regulations --> Supervisor: value = FS_RESET_CONDITION
                    Supervisor -> Supervisor : eventReset, ResetTasks
                    note right
                        If (allowed END_OF_CYCLE to CANCELLING transition) AND (Cancelling cycle can start)
                        Supervisor goes to CANCELLING state
                        else
                        Supervisor goes to PROGRAMMING state.
                    end note
                end
            else Supervisor_Data[].State == FSM_STATE_PAUSE
                alt
                    User -> HMI : Select Start
                    HMI -> Regulations : SetRequestedIndex(FUNC_ID_START, FS_START_CONDITION)
                    Supervisor -> Regulations : GetValue(FUNC_ID_START)
                    Regulations --> Supervisor: value = FS_START_CONDITION
                    alt OK_TO_START
                        Supervisor -> Supervisor : eventResume
                    end
                    note right
                        If Cancelling cycle is current paused cycle
                        Supervisor goes to CANCELLING state
                        else
                        Supervisor goes to RUN state.
                    end note
                else
                    User -> HMI : Select Cancel
                    HMI -> Regulations : SetRequestedIndex(FUNC_ID_START, FS_RESET_CONDITION)
                    Supervisor -> Regulations : GetValue(FUNC_ID_START)
                    Regulations --> Supervisor: value = FS_RESET_CONDITION
                    Supervisor -> Supervisor : eventReset
                    note right
                        If (allowed PAUSE to CANCELLING transition) AND (Cancelling cycle can start)
                        Supervisor goes to CANCELLING state
                        else
                        Supervisor goes to PROGRAMMING state.
                    end note
                end
            else Supervisor_Data[].State == FSM_STATE_CANCELLING
                alt
                    User -> HMI : Select Pause
                    HMI -> Regulations : SetRequestedIndex(FUNC_ID_START, FS_PAUSE_CONDITION)
                    Supervisor -> Regulations : GetValue(FUNC_ID_START)
                    Regulations --> Supervisor: value = FS_PAUSE_CONDITION
                    Supervisor -> Supervisor : eventPause
                    note right : Causes Supervisor to go to PAUSE state and runs PAUSE cycle.
                end
            else Supervisor_Data[].State == FSM_STATE_FAULT
                alt
                    User -> HMI : Select Reset Faulty Condition
                    HMI -> Regulations : SetRequestedIndex(FUNC_ID_START, FS_RESET_FAULT)
                    Supervisor -> Regulations : GetValue(FUNC_ID_START)
                    Regulations --> Supervisor: value = FS_RESET_FAULT
                    Supervisor -> Supervisor : evFaultReset
                    note right : Causes Supervisor to go to PROGRAMMING state.
                else
                    User -> HMI : Select Cancel
                    HMI -> Regulations : SetRequestedIndex(FUNC_ID_START, FS_RESET_CONDITION)
                    Supervisor -> Regulations : GetValue(FUNC_ID_START)
                    Regulations --> Supervisor: value = FS_RESET_CONDITION
                    Supervisor -> Supervisor : eventReset
                    note right
                        If (allowed FAULT to CANCELLING transition) AND (Cancelling cycle can start)
                        Supervisor goes to CANCELLING state
                        else
                        Supervisor stays in FAULT state.
                    end note
                end
            end
        end
    @enduml

    @startuml{Supervisor_SM.png}
        title Supervisor State Machine

        'developer variables = BOOL_TYPE * okay_to_start_cycle

        [*] --> CheckForFaults

        state CheckForFaults {
            [*] --> CycleNotRunning

            state CycleNotRunning {
                [*] --> PROGRAMMING

                PROGRAMMING :entry/\n\t ProgrammingEntry \n\t SetSystemStateProgramming
                PROGRAMMING :do/\n\t UpdateOkayToStartCycle \n\t ProgrammingDo \n\t SetSystemStateProgramming

                DELAY :entry/\n\t DelayEntry \n\t SetSystemStateDelay
                DELAY :do/\n\t UpdateOkayToStartCycle \n\t DelayDo \n\t SetSystemStateDelay

                DELAY_PAUSE :entry/\n\t DelayPauseEntry \n\t SetSystemStateDelayPause
                DELAY_PAUSE :do/\n\t UpdateOkayToStartCycle \n\t DelayPauseDo \n\t SetSystemStateDelayPause
                DELAY_PAUSE :exit /\n\t DelayPauseExit

                PROGRAMMING -l-> DELAY : evDelay
                DELAY -l-> DELAY_PAUSE : evPause
                DELAY_PAUSE -r-> DELAY : evResume
            }

            state CycleRunning {
                RUN :entry/\n\t RunEntry \n\t SetSystemStateRun
                RUN :do/\n\t RunDo \n\t SetSystemStateRun
                RUN :exit/\n\t RunExit

                PAUSE :entry/\n\t PauseEntry \n\t SetSystemStatePause
                PAUSE :do/\n\t UpdateOkayToStartCycle \n\t PauseDo \n\t SetSystemStatePause
                PAUSE :exit/\n\t PauseExit

                RUN -l-> PAUSE : evPause
                PAUSE -r-> RUN : evResume \n/ Resume
            }

            END_OF_CYCLE :entry/\n\t EndEntry \n\t SetSystemStateEndOfCycle
            END_OF_CYCLE :do/\n\t SetSystemStateEndOfCycle

            state Cancel {
                CANCELLING :entry/\n\t CancellingEntry \n\t SetSystemStateCancelling
                CANCELLING :do/\n\t RunDo \n\t SetSystemStateCancelling
                CANCELLING :exit/\n\t CancellingExit

                PauseCancelling :entry/\n\t PauseEntry \n\t SetSystemStatePause
                PauseCancelling :do/\n\t UpdateOkayToStartCycle \n\t PauseDo \n\t SetSystemStatePause
                PauseCancelling :exit /\n\t PauseExit

                CANCELLING -r-> PauseCancelling : evPause
                PauseCancelling -l-> CANCELLING : evResume \n/ Resume
            }

            state JoinToProgramming <<join>>
            state JoinToRun <<join>>
            state JoinToCancel <<join>>
        }

        CycleNotRunning --> CycleNotRunning : evReset \n/ Reset
        JoinToProgramming -u-> CycleNotRunning :/ Reset \n ResetTasks

        PROGRAMMING --> JoinToRun : evStart
        DELAY --> JoinToRun : evStart
        JoinToRun --> RUN :/ Start

        RUN --> END_OF_CYCLE : evEnd
        RUN --> JoinToCancel : evReset \n[ IsCancelCycleEnabled ]
        CycleRunning -u-> JoinToProgramming : evReset \n[ !IsCancelCycleEnabled ]
        PAUSE --> JoinToCancel : evReset \n[ IsCancelCycleEnabled ]
        JoinToCancel --> CANCELLING :/ StartCancellingCycle \n ResetTasks

        END_OF_CYCLE -u-> CycleNotRunning : evReset \n/ Reset
        CANCELLING -u-> JoinToProgramming : evEnd
        Cancel -u-> JoinToProgramming : evReset

        CheckForFaults --> FAULT : evFault
        FAULT :entry/\n\t FaultEntry \n\t SetSystemStateFault
        FAULT :do/\n\t FaultDo \n\t SetSystemStateFault
        FAULT --> JoinToCancel : evFaultReset \n [ IsCancelCycleEnabled && IsCancellingToFaultEnabled ]
        FAULT -u-> CheckForFaults : evFaultReset \n [ !IsCancelCycleEnabled || !IsCancellingToFaultEnabled ] \n/ Reset
        FAULT --> FAULT : evFault

    @enduml
 *
 *  @copyright  Copyright 2015-2016 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"

#include "Supervisor.h"
#include "Supervisor_prv.h"
#include "Variable.h"
#include "Supervisor_fsm.h"

#include "Interpreter.h"

// -- Other Modules --
#if (API012EXP_FEATURE == ENABLED)
    #include "API012Exp.h"
#endif

#include "ActivateTask.h"
#include "Estimator.h"
#include "Fault.h"
#include <limits.h>
#include "Log.h"
#include "Maintain.h"
#include "Micro.h"
#include "Mode.h"
#include "NVData.h"
#include "Regulations.h"
#include "Selector.h"
#include "SettingFile.h"
#include "Statistics.h"
#include <string.h>

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//! Definition of extern SYSTEM_STATE_TYPE System_State in Variable module. One entry per compartment (0 indicates compartment 1).
#pragma location = "_appliance_segment"
SYSTEM_STATE_TYPE System_State[APPLIANCE_MAX_COMPARTMENTS];

//! Variable to control delay time count down and provide status to the HMI in seconds.
//! Not static because it is extern to the Variables Module.
#pragma location = "_appliance_segment"
uint32 Supervisor_Delay_Time_Remaining[APPLIANCE_MAX_COMPARTMENTS];

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//! External type for service cycle/phase/start requests.
PACKED typedef struct EXTERNAL_REQUEST_STRUCT
{
    uint8 Phase;
    uint8 Step;
    uint8 Start_Value;
} EXTERNAL_REQUEST_TYPE;

//! Structure type to hold compartment supervisor data.
PACKED typedef struct SUPERVISOR_DATA_STRUCT
{
    uint16                      Cycle_Elapsed_Time;         //!< Run time for current or last cycle.
    uint16                      Time_Counter;               //!< 100 millisecond counter for cycle elapsed time and delay.
    uint8                       Fault_Phase;                //!< Fault phase for current compartment (if active).
    SUPERVISOR_FSM_STATE_TYPE   State;                      //!< State of supervisor for each compartment.
    BOOL_TYPE                   Fault : 1;                  //!< Fault is active for current compartment.
    BOOL_TYPE                   Pause_Request : 1;          //!< Pause request from non HMI source (e.g. monitors).
    BOOL_TYPE                   Test_Cycle_Active : 1;      //!< Compartment 0 running test cycle all others off.
    BOOL_TYPE                   Test_Cycle_Selected : 1;    //!< Test cycle selected waiting for start.
} SUPERVISOR_DATA_TYPE;

//! Supervisor regulations type for status/values from HMI regulations.
PACKED typedef struct SPV_REGULATIONS_DATA_STRUCT
{
    uint8 Selector_Value;
    uint8 Setting_File_Value;
    uint8 Start_Value;
    BOOL_TYPE Start_Changed : 1;
} SPV_REGULATIONS_DATA_TYPE;

//! Compartment of current running supervisor state machine.
static uint8 Current_Compartment;

//! Structure to store data from external request.
static EXTERNAL_REQUEST_TYPE External_Request[APPLIANCE_MAX_COMPARTMENTS];

//! Result from Interpreter per compartment
#pragma location = "_appliance_segment"
static INTERPRETER_RESULT_TYPE Interpreter_Result[APPLIANCE_MAX_COMPARTMENTS];

//! Supervisor regulations compartment array. One entry per compartment (0 indicates compartment 1).
#pragma location = "_appliance_segment"
static SPV_REGULATIONS_DATA_TYPE Spv_Regulations[APPLIANCE_MAX_COMPARTMENTS];

//! Supervisor data compartment array. One entry per compartment (0 indicates compartment 1).
#pragma location = "_appliance_segment"
static SUPERVISOR_DATA_TYPE Supervisor_Data[APPLIANCE_MAX_COMPARTMENTS];

// --- LOG MODULE SETTINGS ---------------------------
// Log Module Messages Start ------------------------------------------------------------------------------------------
// This enumeration was auto-generated by the LogMessagesCreator.
// NOTE: DO NOT modify the code contained between Log Module Messages Start to Log Module Messages End or the
//       LogMessagesCreator will not be able to find the auto-generated code.
// LogMessagesCreator Version: 2.4.0-beta
// Created on: 2023y8m22d 14h12m55s
#define MODULE_NAME MODULE_SUPERVISOR
typedef enum
{
    ATTEMPT_TO_PAUSE_CYCLE_NOT_ALLOWED = 1,     // Attempt to pause a cycle that does not allow pause.
    AUTO_START_CYCLE_ENDED_RESTARTED,           // An auto-start compartment transitioned to end state.
    COMPARTMENT_OUT_OF_RANGE,                   // Compartments start at zero.
    INVALID_CYCLE_STATE_ON_COLD_RESET,          // Invalid cycle state, cycle canceled.
    MAX_INTERPRETER_STEPS,                      // Continuation of loads exceeds max steps.
    NOT_SUBMODE_NORMAL_SUPERVISOR,              // External call made while not in submode normal.
    SELECTOR_CLEARED_BEFORE_EXTERNAL_START      // Selector cleared before external start runs.
} MODULE_SUPERVISOR_LOG_MESSAGE_ID_TYPE;
// Log Module Messages End --------------------------------------------------------------------------------------------

//! Functionality to execute on entry of the Cancelling state
#ifndef CANCELLING_ENTRY
    #define CANCELLING_ENTRY(compartment)   // Define empty macro unless defined by the project
#endif

//! Functionality to execute on exit of the Cancelling state
#ifndef CANCELLING_EXIT
    #define CANCELLING_EXIT(compartment)    // Define empty macro unless defined by the project
#endif

//! Functionality to execute on entry of the Delay state
#ifndef DELAY_ENTRY
    #define DELAY_ENTRY(compartment)        // Define empty macro unless defined by the project
#endif

//! Functionality to execute on entry of the Delay Pause state
#ifndef DELAY_PAUSE_ENTRY
    #define DELAY_PAUSE_ENTRY(compartment)  // Define empty macro unless defined by the project
#endif

//! Functionality to execute on exit of the Delay Pause state
#ifndef DELAY_PAUSE_EXIT
    #define DELAY_PAUSE_EXIT(compartment)   // Define empty macro unless defined by the project
#endif

//! Functionality to execute on entry of the End state
#ifndef END_ENTRY
    #define END_ENTRY(compartment)          // Define empty macro unless defined by the project
#endif

//! Functionality to execute on entry of the Fault state
#ifndef FAULT_ENTRY
    #define FAULT_ENTRY(compartment)        // Define empty macro unless defined by the project
#endif

//! Definition to enable/disable transition from the Fault state to the Cancelling state
#ifndef FAULT_TO_CANCELLING_TRANSITION
    #define FAULT_TO_CANCELLING_TRANSITION  DISABLED    // Default to DISABLED, unless otherwise defined
#endif

//! Functionality to execute on entry of the Pause state and Pause Cancelling state
#ifndef PAUSE_ENTRY
    #define PAUSE_ENTRY(compartment)        // Define empty macro unless defined by the project
#endif

//! Functionality to execute on exit of the Pause state and Pause Cancelling state
#ifndef PAUSE_EXIT
    #define PAUSE_EXIT(compartment)         // Define empty macro unless defined by the project
#endif

//! Functionality to execute on entry of the Programming state
#ifndef PROGRAMMING_ENTRY
    #define PROGRAMMING_ENTRY(compartment)  // Define empty macro unless defined by the project
#endif

//! Functionality to execute when a cycle is resumed (after pausing customer cycle or Cancelling cycle)
#ifndef RESUME
    #define RESUME(compartment)             // Define empty macro unless defined by the project
#endif

//! Functionality to execute on entry of the Run state
#ifndef RUN_ENTRY
    #define RUN_ENTRY(compartment)          // Define empty macro unless defined by the project
#endif

//! Functionality to execute on exit of the Run state
#ifndef RUN_EXIT
    #define RUN_EXIT(compartment)           // Define empty macro unless defined by the project
#endif

//! Functionality to execute when a cycle is started (only executes one time when the cycle is first started)
#ifndef START
    #define START(compartment)              // Define empty macro unless defined by the project
#endif

#ifndef TEST_CYCLE_STAYS_IN_END_UNTIL_RESET
    //! If ENABLED, Supervisor stays in END state until being reset by the HMI after running a Test Manager Cycle.
    //! If DISABLED, Supervisor automatically goes back to PROGRAMMING state after running a Test Manager Cycle.
    #define TEST_CYCLE_STAYS_IN_END_UNTIL_RESET     (DISABLED)
#endif

//! Macro to inform Supervisor if delay regulation should be checked.
#ifndef CYCLE_DELAY_FEATURE
    #define CYCLE_DELAY_FEATURE (DISABLED)
#endif

//! Macro to inform Supervisor if PROGRAMMING, DELAY and PAUSE NOT_OK_TO_START SYSTEM STATES are enabled.
#ifndef NOT_OK_TO_START_SYSTEM_STATES
    #define NOT_OK_TO_START_SYSTEM_STATES    (DISABLED)
#endif

#ifndef SPECIAL_CYCLE_CCRB_FROM_SELECTOR
    //! If ENABLED, special cycle CCRB will behave according to CCRB for SElECTOR.
    //! If DISABLED special cycle CCRB will be CYCLE_COLD_RESET_BEHAVIOR_RESUME
    #define SPECIAL_CYCLE_CCRB_FROM_SELECTOR    (DISABLED)
#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void CheckRegulations(uint8 compartment);
static void ConfigureEndCycle(uint8 phase, SUPERVISOR_FSM_VAR_TYPE * fsm_var);
static void DefineSystemState(BOOL_TYPE okay_to_start_cycle, SYSTEM_STATE_TYPE system_state);
static void SupervisorFaultObserver(uint8 fault_monitor_id);
static void SupervisorPreFaultObserver(uint8 fault_monitor_id);
static void TestCycleEnd(void);
#if (CYCLE_DELAY_FEATURE == ENABLED)
    static void GetDelay(BOOL_TYPE check_for_change);
#endif
static void SupervisorFsmEventCompartment(SUPERVISOR_FSM_EVENT_TYPE ev, uint8 compartment);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module Supervisor and its variables
 */
void Supervisor__Initialize(void)
{
    for (uint8 index = 0u; index < APPLIANCE_MAX_COMPARTMENTS; index++)
    {
        External_Request[index].Start_Value = 0;
        External_Request[index].Phase = PHASE_STEP_NONE;
        External_Request[index].Step = PHASE_STEP_NONE;
    }

    Fault__RegisterFaultEvents(SupervisorFaultObserver);
    Fault__RegisterPreFaultEvents(SupervisorPreFaultObserver);

    Interpreter__Initialize();
    if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
    {
        if (NVData__GetLastRestoreStatus()->Bit.Appliance_Data_Recovered != (NVDATA_RESTORE_TYPE)NVDATA_RESTORE_OKAY)
        {
            memset(Interpreter_Result, 0x00, sizeof(Interpreter_Result));
            memset(Spv_Regulations, 0x00, sizeof(Spv_Regulations));
            memset(Supervisor_Data, 0x00, sizeof(Supervisor_Data));
            memset(Supervisor_Delay_Time_Remaining, 0x00, sizeof(Supervisor_Delay_Time_Remaining));

            for (Current_Compartment = 0u; Current_Compartment < APPLIANCE_MAX_COMPARTMENTS; Current_Compartment++)
            {
                BOOL_TYPE okay_to_start_cycle = TRUE;
                SupervisorFsmInitialize(&Supervisor_Data[Current_Compartment].State, &okay_to_start_cycle);
            }
        }
        else
        {
            // ---------- Manage cold reset behavior for each compartment ----------
            for (Current_Compartment = 0u; Current_Compartment < APPLIANCE_MAX_COMPARTMENTS; Current_Compartment++)
            {
                // Determine what cycle was running.
                CYCLE_PHASE_STATEMENT_TYPE cps = Interpreter__GetCurrentStatement(Current_Compartment);

                // Determine what type of cycle was running
                CYCLE_STATE_TYPE cycle_state = Selector__GetCycleState(0, Current_Compartment, cps.Cycle);

                // Determine cold reset behavior according to selected cycle.
                uint8 selected_cycle = CYCLE_STATE_PROGRAMMING;
                switch (cycle_state)
                {
                    case CYCLE_STATE_PROGRAMMING:
                    case CYCLE_STATE_RUN:
                    case CYCLE_STATE_FAULT:
                        selected_cycle = cps.Cycle;
                        break;
                    case CYCLE_STATE_DELAY:
                    case CYCLE_STATE_END:
                    case CYCLE_STATE_PAUSE:
                        #if (SPECIAL_CYCLE_CCRB_FROM_SELECTOR == DISABLED)
                        selected_cycle = cps.Cycle;
                        #elif (SPECIAL_CYCLE_CCRB_FROM_SELECTOR == ENABLED)
                        selected_cycle = cps.Phase;
                        #endif
                        break;
                    default:
                        // Set CYCLE_STATE_PROGRAMMING - ccrb from Setting file will be CYCLE_COLD_RESET_BEHAVIOR_CANCEL
                        LOG_ADD_EXCEPTION(INVALID_CYCLE_STATE_ON_COLD_RESET, Current_Compartment); // Invalid cycle state, cycle canceled.
                        break;
                }
                CYCLE_COLD_RESET_BEHAVIOR_TYPE ccrb = Selector__GetColdResetBehavior(PRIMARY_SETTING_FILE, Current_Compartment, selected_cycle);

                // Manage cold reset behavior as determined by selection in setting file for the selected cycle.
                if (ccrb == CYCLE_COLD_RESET_BEHAVIOR_SELECT)
                {
                    ccrb = CYCLE_COLD_RESET_SELECT_BEHAVIOR(Current_Compartment, cycle_state);
                }
                switch (ccrb)
                {
                    default: //lint-fallthrough
                    case CYCLE_COLD_RESET_BEHAVIOR_CANCEL:
                        #if (STATISTICS_SAVED_CYCLES > 0)
                            // Only update Statistics that the cycle has stopped if it was Running/Paused before the cold reset
                            switch (cycle_state)
                            {
                                case CYCLE_STATE_RUN:
                                case CYCLE_STATE_PAUSE:
                                    Statistics__CycleStopped(FALSE, Current_Compartment, cps, Supervisor_Data[Current_Compartment].Cycle_Elapsed_Time);
                                    break;
                                case CYCLE_STATE_END:
                                case CYCLE_STATE_DELAY:
                                case CYCLE_STATE_FAULT:
                                case CYCLE_STATE_PROGRAMMING:
                                    //lint -fallthrough
                                default:
                                    // Nothing to do
                                    break;
                            }
                        #endif

                        if (Supervisor__IsTestCycleActive() != FALSE)
                        {
                            if (Current_Compartment == 0u)
                            {
                                TestCycleEnd();
                                SupervisorFsmEventCompartment(FSM_EVENT_RESET, Current_Compartment);
                            }
                        }
                        else
                        {
                            SupervisorFsmEventCompartment(FSM_EVENT_RESET, Current_Compartment);
                        }
                        break;

                    case CYCLE_COLD_RESET_BEHAVIOR_PAUSE:
                        if (cycle_state != CYCLE_STATE_DELAY)
                        {
                            SupervisorFsmEventCompartment(FSM_EVENT_PAUSE, Current_Compartment);
                        }
                        break;

                    case CYCLE_COLD_RESET_BEHAVIOR_RESTART_PHASE:
                    {
                        CYCLE_PHASE_STATEMENT_TYPE lower_scope_cps = Interpreter__GetCurrentStatement(Current_Compartment);
                        INTERPRETER_CONFIG_TYPE interpreter_config;
                        interpreter_config.Setting_File_Index = 0;
                        interpreter_config.Cycle = lower_scope_cps.Cycle;
                        interpreter_config.Phase = lower_scope_cps.Phase;
                        interpreter_config.Statement = 0;
                        interpreter_config.Cycle_State = Selector__GetCycleState(interpreter_config.Setting_File_Index, Current_Compartment, interpreter_config.Cycle);
                        Interpreter__Configure(Current_Compartment, &interpreter_config);
                    }
                        break;

                    case CYCLE_COLD_RESET_BEHAVIOR_RESTART_CYCLE:
                    {
                        CYCLE_PHASE_STATEMENT_TYPE lower_scope_cps = Interpreter__GetCurrentStatement(Current_Compartment);
                        INTERPRETER_CONFIG_TYPE interpreter_config;
                        interpreter_config.Setting_File_Index = 0;
                        interpreter_config.Cycle = lower_scope_cps.Cycle;
                        switch ( cycle_state )
                        {
                            case CYCLE_STATE_DELAY:
                            case CYCLE_STATE_END:
                            case CYCLE_STATE_PAUSE:
                                interpreter_config.Phase = lower_scope_cps.Phase;
                                break;
                            default:
                                interpreter_config.Phase = 0;
                                break;
                        }
                        interpreter_config.Statement = 0;
                        interpreter_config.Cycle_State = Selector__GetCycleState(interpreter_config.Setting_File_Index, Current_Compartment, interpreter_config.Cycle);
                        Interpreter__Configure(Current_Compartment, &interpreter_config);
                    }
                        break;

                    case CYCLE_COLD_RESET_BEHAVIOR_RESUME:
                        break;
                }
            }
        }
    }

    for (Current_Compartment = 0u; Current_Compartment < APPLIANCE_MAX_COMPARTMENTS; Current_Compartment++)
    {
        switch ( Supervisor_Data[Current_Compartment].State )
        {
            case FSM_STATE_NONE:
                {
                    BOOL_TYPE okay_to_start_cycle = TRUE;
                    SupervisorFsmInitialize(&Supervisor_Data[Current_Compartment].State, &okay_to_start_cycle);
                }
                break;
            case FSM_STATE_FAULT:
                {
                    Fault__CompartmentManaged(Current_Compartment);

                    sint32 fault_id;
                    if (( Variable__GetValue(VARIABLE_FAULT, Current_Compartment, &fault_id) != FALSE ) &&
                        ( fault_id == 0 ))
                    {
                        SupervisorFsmEventCompartment(FSM_EVENT_FAULT_RESET, Current_Compartment);
                    }
                }
                break;
            default:
                // do nothing
                break;
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Engage test_cycle_id in compartment 0.
 *  @details    Set all other compartments to the Run State with Selector set to OFF.
 *              Set flag to indicate only compartment 0 should run.
 *
 *  @param      uint8 compartment - identifier for appliance compartment. (0 based, 0 for single compartment appliances)
 *
 *  @return     PASS_FAIL_TYPE result
 *  @retval     PASS - Valid test_cycle_id Test engaged.
 *  @retval     FAIL - Invalid test_cycle_id Test Not engaged.
 */
PASS_FAIL_TYPE Supervisor__EngageTest(uint8 test_cycle_id)
{
    PASS_FAIL_TYPE result = PASS;

    switch ( Selector__GetCycleNameId(Spv_Regulations[0].Setting_File_Value, 0, test_cycle_id) )
    {
        case CYCLE_NAME_EOL_0:
        case CYCLE_NAME_EOL_1:
        case CYCLE_NAME_EOL_2:
        case CYCLE_NAME_SERVICE_0:
        case CYCLE_NAME_SERVICE_1:
            Fault__SetTestEngaged(TRUE);

            #if (API012EXP_FEATURE == ENABLED)
            API012Exp__SetTestCycleEngaged(TRUE);
            #endif

            Supervisor_Data[0].Test_Cycle_Selected = TRUE;

            // Reset all compartments to off.
            for (uint8 compartment = 0u; compartment < APPLIANCE_MAX_COMPARTMENTS; compartment++)
            {
                // Send compartment to programming state.
                (void)Supervisor__SetStart(compartment, (Supervisor_Data[compartment].State == FSM_STATE_FAULT) ? FS_RESET_FAULT : FS_RESET_CONDITION);

                // Run compartment Supervisor to accept reset.
                Supervisor__Run(compartment);

                // Set compartment selector to OFF
                (void)Supervisor__SetCycle(compartment, 0);
            }

            Supervisor_Data[0].Test_Cycle_Active = TRUE;
            Supervisor_Data[0].Test_Cycle_Selected = FALSE;

            // Start service cycle.
            (void)Supervisor__SetCycle(0, test_cycle_id);
            result = Supervisor__SetStart(0, FS_START_CONDITION);
            break;
        default:
            result = FAIL;
            break;
    }

    if (result == FAIL)
    {
        TestCycleEnd();
        SupervisorFsmEventCompartment(FSM_EVENT_RESET, 0u);
    }

    return (result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Return boolean value to indicate if a test cycle is active.
 *
 *  @return     BOOL_TYPE
 *  @retval     TRUE - Test Cycle is active.
 *  @retval     FALSE - Test Cycle is not active.
 */
BOOL_TYPE Supervisor__IsTestCycleActive(void)
{
    return (Supervisor_Data[0].Test_Cycle_Active);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Return selected cycle that is in paused delay.
 *
 *  @param      uint8 compartment - identifier for appliance compartment. (0 based, 0 for single compartment appliances)
 *
 *  @return     uint8
 *  @retval     0 - Not in paused delay
 *  @retval     1-255 - Selector value for delayed cycle.
 */
uint8 Supervisor__IsInPausedDelay(uint8 compartment)
{
    return ( (Supervisor_Data[compartment].State == FSM_STATE_DELAY_PAUSE) ? Spv_Regulations[compartment].Selector_Value : 0u );
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get cycle time of current or last running cycle for compartment.
 *
 *  @param      uint8 compartment - identifier for appliance compartment. (0 based, 0 for single compartment appliances)
 *
 *  @return     uint16 Cycle_Elapsed_Time - Compartment elapsed time of current or last running cycle (in seconds).
 */
uint16 Supervisor__GetCycleTime(uint8 compartment)
{
    return ( Supervisor_Data[compartment].Cycle_Elapsed_Time );
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Run cycle/phase through Interpreter for every Supervisor State.
 *              Called by ApplianceMgr in a loop for each compartment.
 *
 *  @param      uint8 compartment - identifier for appliance compartment. (0 based, 0 for single compartment appliances)
 */
void Supervisor__Run(uint8 compartment)
{
    if (compartment >= APPLIANCE_MAX_COMPARTMENTS)
    {
        LOG_ADD_EXCEPTION(COMPARTMENT_OUT_OF_RANGE, compartment); // Compartments start at zero.
    }
    else if (Mode__GetSubMode() != SUBMODE_NORMAL_SUPERVISOR)
    {
        LOG_ADD_EXCEPTION(NOT_SUBMODE_NORMAL_SUPERVISOR, compartment); // External call made while not in submode normal.
    }
    else
    {
        ActivateTask__RunCompartment(compartment);

        uint8 interpreter_step_count = 0;
        do
        {
            interpreter_step_count++;
            Interpreter_Result[compartment] = Interpreter__Run(compartment);
        } while (((Interpreter_Result[compartment] == INTERPRETER_RESULT_END_STEP)
                    ||
                    ((Interpreter_Result[compartment] == INTERPRETER_RESULT_END_PHASE)
                        &&
                        ((Supervisor_Data[compartment].State == FSM_STATE_RUN)
                            ||
                            (Supervisor_Data[compartment].State == FSM_STATE_CANCELLING))))
                &&
                (interpreter_step_count < (uint8)INTERPRETER_MAX_STEPS));

        if (interpreter_step_count >= (uint8)INTERPRETER_MAX_STEPS)
        {
            LOG_ADD_EXCEPTION(MAX_INTERPRETER_STEPS, interpreter_step_count); // Continuation of loads exceeds max steps.
        }

        Current_Compartment = compartment;

        if (Interpreter_Result[compartment] == INTERPRETER_RESULT_INVALID_COMPARTMENT)
        {
            BOOL_TYPE okay_to_start_cycle = TRUE;
            SupervisorFsmTerminate(&Supervisor_Data[compartment].State, &okay_to_start_cycle);
        }

        CheckRegulations(compartment);

        if (Supervisor_Data[compartment].Fault != (BOOL_TYPE)FALSE)
        {
            Supervisor_Data[compartment].Fault = FALSE;
            SupervisorFsmEventCompartment(FSM_EVENT_FAULT, compartment);
        }

        BOOL_TYPE okay_to_start_cycle = TRUE;
        SupervisorFsmExecute(&Supervisor_Data[compartment].State, &okay_to_start_cycle);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Request cycle to be run. Only allowed in programming state.
 *
 *  @param      uint8 compartment - Identifier for appliance compartment (0 based, 0 for single compartment appliances).
 *  @param      uint8 cycle - Cycle to be run.
 *
 *  @retval     PASS - Supervisor state OK. Compartment and cycle are valid.
 *  @retval     FAIL - Supervisor state not OK. Compartment or cycle are invalid.
 */
PASS_FAIL_TYPE Supervisor__SetCycle(uint8 compartment, uint8 cycle)
{
    PASS_FAIL_TYPE result = FAIL;

    if (compartment >= APPLIANCE_MAX_COMPARTMENTS)
    {
        LOG_ADD_EXCEPTION(COMPARTMENT_OUT_OF_RANGE, compartment); // Compartments start at zero.
    }
    else if (Mode__GetSubMode() != SUBMODE_NORMAL_SUPERVISOR)
    {
        LOG_ADD_EXCEPTION(NOT_SUBMODE_NORMAL_SUPERVISOR, compartment); // External call made while not in submode normal.
    }
    else if ( Supervisor_Data[compartment].State == FSM_STATE_PROGRAMMING )
    {
        switch ( Selector__GetCycleState(Spv_Regulations[compartment].Setting_File_Value, compartment, cycle) )
        {
            case CYCLE_STATE_RUN:
            case CYCLE_STATE_PROGRAMMING:
                {
                    Spv_Regulations[compartment].Selector_Value = cycle;

                    INTERPRETER_CONFIG_TYPE interpreter_config;
                    interpreter_config.Setting_File_Index = Spv_Regulations[compartment].Setting_File_Value;
                    interpreter_config.Cycle = 0;
                    interpreter_config.Phase = Spv_Regulations[compartment].Selector_Value;
                    interpreter_config.Statement = 0;
                    interpreter_config.Cycle_State = Selector__GetCycleState(interpreter_config.Setting_File_Index, compartment, interpreter_config.Cycle);
                    Interpreter__Configure(compartment, &interpreter_config);

                    #if (TIME_ESTIMATION_FEATURE == ENABLED)
                    Estimator__EstimateFast();
                    #endif

                    result = PASS;
                }
                break;
            default:
                // do nothing
                break;
        }
    }

    return (result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Request phase to be run.
 *
 *  @param      uint8 compartment - Identifier for appliance compartment (0 based, 0 for single compartment appliances).
 *  @param      uint8 phase - Phase (0-127), PHASE_STEP_FORWARD, PHASE_STEP_BACKWARD or PHASE_STEP_NONE.
 *
 *  @retval     PASS - Supervisor state OK and compartment is valid.
 *  @retval     FAIL - Supervisor state not OK or compartment is invalid.
 */
PASS_FAIL_TYPE Supervisor__SetPhase(uint8 compartment, uint8 phase)
{
    PASS_FAIL_TYPE result = FAIL;

    if (compartment >= APPLIANCE_MAX_COMPARTMENTS)
    {
        LOG_ADD_EXCEPTION(COMPARTMENT_OUT_OF_RANGE, compartment); // Compartments start at zero.
    }
    else if (Mode__GetSubMode() != SUBMODE_NORMAL_SUPERVISOR)
    {
        LOG_ADD_EXCEPTION(NOT_SUBMODE_NORMAL_SUPERVISOR, compartment); // External call made while not in submode normal.
    }
    else
    {
        switch ( Supervisor_Data[compartment].State )
        {
            case FSM_STATE_RUN:
            case FSM_STATE_CANCELLING:
                External_Request[compartment].Phase = phase;
                result = PASS;
                break;
            default:
                // do nothing
                break;
        }
    }

    return (result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Request step to be run.
 *
 *  @param      uint8 compartment - Identifier for appliance compartment (0 based, 0 for single compartment appliances).
 *  @param      uint8 step - Step (0-127), PHASE_STEP_FORWARD, PHASE_STEP_BACKWARD or PHASE_STEP_NONE.
 *
 *  @retval     PASS - Supervisor state OK and compartment is valid.
 *  @retval     FAIL - Supervisor state not OK or compartment is invalid.
 */
PASS_FAIL_TYPE Supervisor__SetStep(uint8 compartment, uint8 step)
{
    PASS_FAIL_TYPE result = FAIL;

    if (compartment >= APPLIANCE_MAX_COMPARTMENTS)
    {
        LOG_ADD_EXCEPTION(COMPARTMENT_OUT_OF_RANGE, compartment); // Compartments start at zero.
    }
    else if (Mode__GetSubMode() != SUBMODE_NORMAL_SUPERVISOR)
    {
        LOG_ADD_EXCEPTION(NOT_SUBMODE_NORMAL_SUPERVISOR, compartment); // External call made while not in submode normal.
    }
    else
    {
        switch ( Supervisor_Data[compartment].State )
        {
            case FSM_STATE_RUN:
            case FSM_STATE_CANCELLING:
                External_Request[compartment].Step = step;
                result = PASS;
                break;
            default:
                // do nothing
                break;
        }
    }

    return (result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Handle start regulations from an external device.
 *
 *  @param      uint8 compartment - Identifier for appliance compartment (0 based, 0 for single compartment appliances).
 *  @param      REGULATIONS_START_COMMAND_TYPE start_value - Start command value.
 *
 *  @retval     PASS - Supervisor state OK. Start command accepted.
 *  @retval     FAIL - Supervisor state not OK. Start command rejected.
 */
PASS_FAIL_TYPE Supervisor__SetStart(uint8 compartment, REGULATIONS_START_COMMAND_TYPE start_value)
{
    PASS_FAIL_TYPE result = FAIL;

    if (compartment >= APPLIANCE_MAX_COMPARTMENTS)
    {
        LOG_ADD_EXCEPTION(COMPARTMENT_OUT_OF_RANGE, compartment); // Compartments start at zero.
    }
    else if (Mode__GetSubMode() != SUBMODE_NORMAL_SUPERVISOR)
    {
        LOG_ADD_EXCEPTION(NOT_SUBMODE_NORMAL_SUPERVISOR, compartment); // External call made while not in submode normal.
    }
    else
    {
        CYCLE_PHASE_STATEMENT_TYPE cps = Interpreter__GetCurrentStatement(compartment);
        switch (start_value)
        {
            case FS_DONT_CARE:
                External_Request[compartment].Start_Value = FS_DONT_CARE;
                result = PASS;
                break;

            case FS_START_CONDITION:
                if (((( Supervisor_Data[compartment].State == FSM_STATE_PROGRAMMING ) &&
                      ( cps.Phase != 0u )) ||
                     ( Supervisor_Data[compartment].State == FSM_STATE_PAUSE )) &&
                    ( OK_TO_START_CYCLE(compartment) != FALSE ))
                {
                    External_Request[compartment].Start_Value = FS_START_CONDITION;
                    result = PASS;
                }
                break;

            case FS_RESET_CONDITION:
                External_Request[compartment].Start_Value = FS_RESET_CONDITION;
                result = PASS;
                break;

            case FS_PAUSE_CONDITION:
                if ((Supervisor_Data[compartment].State == FSM_STATE_RUN) ||
                    (Supervisor_Data[compartment].State == FSM_STATE_CANCELLING) ||
                    (Supervisor_Data[compartment].State == FSM_STATE_DELAY))
                {
                    External_Request[compartment].Start_Value = FS_PAUSE_CONDITION;
                    result = PASS;
                }
                break;

            case FS_RESET_FAULT:
                if (Supervisor_Data[compartment].State == FSM_STATE_FAULT)
                {
                    External_Request[compartment].Start_Value = FS_RESET_FAULT;
                    result = PASS;
                }
                break;

            case FS_RESTART_CONDITION:
                break;

            case FS_END_CONDITION:
                if ((Supervisor_Data[compartment].State == FSM_STATE_RUN) ||
                    (Supervisor_Data[compartment].State == FSM_STATE_CANCELLING))
                {
                    SupervisorFsmEventCompartment(FSM_EVENT_END, compartment);
                    result = PASS;
                }
                break;

            default:
                // do nothing
                break;
        }
    }

    return (result);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *   @brief      Set local start regulation based on Regulations API.
 *               Handle "FS_DONT_CARE" for all states.
 *               Handle "FS_RESET_CONDITION" for all states from the UI or external device. Generate evReset if true.
 *               Store all other start commands for state specific processing.
 *
 *  @param      uint8 compartment            - compartment.
 */
static void CheckRegulations(uint8 compartment)
{
    BOOL_TYPE start_regulation_changed = FALSE;

    uint8 position = Regulations__GetPositionByFunctionID(FUNC_ID_START, compartment);
    if (position != (uint8)REGULATION_INVALID)
    {
        Spv_Regulations[compartment].Start_Changed = Regulations__IsRegulationChanged(position);
        if (Spv_Regulations[compartment].Start_Changed != (BOOL_TYPE)FALSE)
        {
            start_regulation_changed = TRUE;
            uint8 value = (uint8)Regulations__GetRequestedMainValueByPosition(position);
            switch ( value )
            {
                case (uint8)FS_DONT_CARE:
                    Spv_Regulations[compartment].Start_Changed = FALSE;
                    Regulations__AcceptRequestByPosition(position);
                    break;
                case (uint8)FS_RESET_CONDITION:
                {
                    Spv_Regulations[compartment].Start_Changed = FALSE;
                    Regulations__AcceptRequestByPosition(position);

                    #if (STATISTICS_SAVED_CYCLES > 0)
                    CYCLE_PHASE_STATEMENT_TYPE cps = Interpreter__GetCurrentStatement(compartment);
                    Statistics__CycleStopped(FALSE, compartment, cps, Supervisor_Data[compartment].Cycle_Elapsed_Time);
                    #endif

                    TestCycleEnd();
                    SupervisorFsmEventCompartment(FSM_EVENT_RESET, compartment);
                }
                    break;
                default:
                    Spv_Regulations[compartment].Start_Value = value;
                    break;
            }
        }
    }

    #if (CYCLE_DELAY_FEATURE == ENABLED)
        GetDelay(TRUE);
    #endif

    // External request does not require regulation to be present.
    // Only check external if no current request through regulation.
    if (( start_regulation_changed == FALSE ) &&
        ( External_Request[compartment].Start_Value == (uint8)FS_RESET_CONDITION ))
    {
        External_Request[compartment].Start_Value = FS_DONT_CARE;

        #if (STATISTICS_SAVED_CYCLES > 0)
        CYCLE_PHASE_STATEMENT_TYPE cps = Interpreter__GetCurrentStatement(compartment);
        Statistics__CycleStopped(FALSE, compartment, cps, Supervisor_Data[compartment].Cycle_Elapsed_Time);
        #endif

        TestCycleEnd();
        SupervisorFsmEventCompartment(FSM_EVENT_RESET, compartment);
    }

    // Always accept set phase/step don't care value in order to detect later changes.
    uint8 phase_position = Regulations__GetPositionByFunctionID(FUNC_ID_ADVANCE_PHASE, compartment);
    if (( phase_position != (uint8)REGULATION_INVALID ) &&
        ( Regulations__IsRegulationChanged(phase_position) != FALSE ) &&
        ( Regulations__GetRequestedMainValueByPosition(phase_position) == PHASE_STEP_NONE ))
    {
        Regulations__AcceptRequestByPosition(phase_position);
    }

    uint8 step_position = Regulations__GetPositionByFunctionID(FUNC_ID_ADVANCE_STEP, compartment);
    if (( step_position != (uint8)REGULATION_INVALID ) &&
        ( Regulations__IsRegulationChanged(step_position) != FALSE ) &&
        ( Regulations__GetRequestedMainValueByPosition(step_position) == PHASE_STEP_NONE ))
    {
        Regulations__AcceptRequestByPosition(step_position);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Supervisor Fault Observer. Sets fault when called.
 *
 *  @param      fault_monitor_id - The fault monitor that contains all fault information.
 *                  Additional information about the fault can be found through the Fault module.
 */
static void SupervisorFaultObserver(uint8 fault_monitor_id)
{
    FAULT_CONFIG_TYPE* data;
    Fault__GetEventInfo(fault_monitor_id, &data);
    if ((data != NULL) && (data->Phase != (uint8)UCHAR_MAX))
    {
        uint8 compartment = data->Compartment;

        Supervisor_Data[compartment].Fault_Phase = data->Phase;
        Supervisor_Data[compartment].Fault = TRUE;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Supervisor Pre-Fault Observer. Pauses cycle based on pre-fault configuration when called.
 *
 *  @param      fault_monitor_id - The fault monitor that contains all fault information.
 *                  Additional information about the pre-fault can be found through the Fault module.
 */
static void SupervisorPreFaultObserver(uint8 fault_monitor_id)
{
    FAULT_CONFIG_TYPE* data;

    Fault__GetEventInfo(fault_monitor_id, &data);

    if ((data != NULL) && (data->Pause_Cycle != (BOOL_TYPE)FALSE))
    {
        Current_Compartment = data->Compartment;
        SupervisorFsmEventCompartment(FSM_EVENT_PAUSE, Current_Compartment);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Check to see if a test cycle is running and handle if true.
 *
 */
static void TestCycleEnd(void)
{
    if (Supervisor_Data[0].Test_Cycle_Active != (BOOL_TYPE)FALSE)
    {
        Supervisor_Data[0].Test_Cycle_Active = FALSE;

        uint8 position = Regulations__GetPositionByFunctionID(FUNC_ID_SELECTOR, 0);
        if (position != (uint8)REGULATION_INVALID)
        {
            Spv_Regulations[0].Selector_Value = (uint8)Regulations__GetRequestedMainValueByPosition(position);
        }

        Fault__SetTestEngaged(FALSE);

        #if (API012EXP_FEATURE == ENABLED)
            API012Exp__SetTestCycleEngaged(FALSE);
        #endif

        #if (1u < APPLIANCE_MAX_COMPARTMENTS)
        // Reset other compartments.
        for (uint8 compartment = 1u; compartment < APPLIANCE_MAX_COMPARTMENTS; compartment++)
        {
            SupervisorFsmEventCompartment(FSM_EVENT_RESET, compartment);
        }
        #endif

    }
}

#if (CYCLE_DELAY_FEATURE == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get the delay time.
 *
 *  @param      BOOL_TYPE check_for_change - TRUE:  Regulation must change to read value.
 *                                           FALSE: Read value regardless of regulation change.
 */
static void GetDelay(BOOL_TYPE check_for_change)
{
    typedef struct
    {
        REGULATIONS_FUNCTION_ID_TYPE func_id;
        sint32 value;
    } DELAY_TABLE_TYPE;
    DELAY_TABLE_TYPE delay_table[] =
    {
        { FUNC_ID_DELAY_HH  ,0 },
        { FUNC_ID_DELAY_MM  ,0 },
        { FUNC_ID_DELAY_SS  ,0 }
    };

    BOOL_TYPE delay_changed = FALSE;
    for (uint8 index = 0u; index < ELEMENTS_IN_ARRAY(delay_table); index++)
    {
        uint8 position = Regulations__GetPositionByFunctionID(delay_table[index].func_id, Current_Compartment);
        if (position != (uint8)REGULATION_INVALID)
        {
            delay_table[index].value = Regulations__GetRequestedMainValueByPosition(position);

            if ((check_for_change != FALSE) && (Regulations__IsRegulationChanged(position) != FALSE))
            {
                delay_changed = TRUE;
                Regulations__AcceptRequestByPosition(position);
            }
        }
    }

    if ((delay_changed != FALSE) || (check_for_change == FALSE))
    {
        Supervisor_Delay_Time_Remaining[Current_Compartment] = (uint32)delay_table[0].value * 3600u;
        Supervisor_Delay_Time_Remaining[Current_Compartment] += (uint32)delay_table[1].value * 60u;
        Supervisor_Delay_Time_Remaining[Current_Compartment] += (uint32)delay_table[2].value;
    }
}
#endif  //(CYCLE_DELAY_FEATURE == ENABLED)

//---------------------------------------------------------------------------------------------------------------------
/**
 * Configure the interpreter to run the END Cycle (Macro).
 */
static void ConfigureEndCycle(uint8 phase, SUPERVISOR_FSM_VAR_TYPE * fsm_var)
{
    INTERPRETER_CONFIG_TYPE interpreter_config;
    interpreter_config.Setting_File_Index = Spv_Regulations[Current_Compartment].Setting_File_Value;
    interpreter_config.Cycle = Selector__GetStateCycle(Spv_Regulations[Current_Compartment].Setting_File_Value, Current_Compartment, CYCLE_STATE_END);
    interpreter_config.Phase = phase;
    interpreter_config.Statement = 0;
    interpreter_config.Cycle_State = CYCLE_STATE_END;
    Interpreter__Configure(Current_Compartment, &interpreter_config);
    Interpreter__ClearSubCycleStatus(Current_Compartment);

    ActivateTask__ResetCompartment(Current_Compartment);

    //Keep END_ENTRY as last item in Entry function (Don't call if EventReset called above).
    //Insure that entry tasks have been completed before notifying application of State Change.
    END_ENTRY(Current_Compartment);
}

/*
 * Defines the SYSTEM_STATE_NOT_OK_TO_START_BIT in the System_State.
 *
 * @param okay_to_start_cycle = macro OK_TO_START_CYCLE return value
 * @param system_state = the value to set System_State to
 */
static void DefineSystemState(BOOL_TYPE okay_to_start_cycle, SYSTEM_STATE_TYPE system_state)
{
    #if (NOT_OK_TO_START_SYSTEM_STATES == ENABLED)
    if ( okay_to_start_cycle == FALSE )
    {
        // set the SYSTEM_STATE_NOT_OK_TO_START_BIT bit in the system_state
        BIT_SET(system_state, SYSTEM_STATE_NOT_OK_TO_START_BIT);
    }
    else // okay_to_start_cycle == TRUE
    {
        // clear the SYSTEM_STATE_NOT_OK_TO_START_BIT bit in the system_state
        BIT_CLR(system_state, SYSTEM_STATE_NOT_OK_TO_START_BIT);
    }
    #endif
    System_State[Current_Compartment] = system_state;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Dispatch event in the state machine for compartment
 *
 *  @param      SUPERVISOR_FSM_EVENT_TYPE ev - event.
 *  @param      uint8 compartment            - compartment.
 */
static void SupervisorFsmEventCompartment(SUPERVISOR_FSM_EVENT_TYPE ev, uint8 compartment)
{
    SUPERVISOR_FSM_VAR_TYPE fsm_var = SupervisorBuildFsmVar(Supervisor_Data[compartment].State);
    BOOL_TYPE okay_to_start_cycle = TRUE;
    SupervisorFsmEvent(ev, &fsm_var, &okay_to_start_cycle);
    Supervisor_Data[compartment].State = fsm_var.State;
}



//=====================================================================================================================
// Supervisor State Machine customization functions.
// NOTE: do not modify this header or the StateMachineCodeGenerator will not be able to find it.
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void CancellingEntry(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    // Application specific macro CANCELLING_ENTRY
    CANCELLING_ENTRY(Current_Compartment);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void CancellingExit(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    // Application specific macro CANCELLING_EXIT
    CANCELLING_EXIT(Current_Compartment);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void DelayDo(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    // Decrement delay time remaining first to insure we don't get extra 100 millisecond before start when reaching 0.
    if (Supervisor_Data[Current_Compartment].Time_Counter >= 9u)
    {
        Supervisor_Data[Current_Compartment].Time_Counter = 0u;
        if (Supervisor_Delay_Time_Remaining[Current_Compartment] != 0u)
        {
            Supervisor_Delay_Time_Remaining[Current_Compartment]--;
        }
    }
    else
    {
        Supervisor_Data[Current_Compartment].Time_Counter++;
    }

    //Check for restart or pause from HMI.
    BOOL_TYPE start_accepted = FALSE;
    BOOL_TYPE pause_accepted = FALSE;
    if (Spv_Regulations[Current_Compartment].Start_Changed != (BOOL_TYPE)FALSE)
    {
        Spv_Regulations[Current_Compartment].Start_Changed = FALSE;
        uint8 position = Regulations__GetPositionByFunctionID(FUNC_ID_START, Current_Compartment);
        switch ( Spv_Regulations[Current_Compartment].Start_Value )
        {
            case (uint8)FS_RESTART_CONDITION:
                start_accepted = TRUE;
                Regulations__AcceptRequestByPosition(position);
                break;
            case (uint8)FS_PAUSE_CONDITION:
                pause_accepted = TRUE;
                Regulations__AcceptRequestByPosition(position);
                break;
            default:
                Regulations__RejectRequestByPosition(position);
                break;
        }
    }

    // Check if ready to start else if pause is requested.
    if ((( Supervisor_Delay_Time_Remaining[Current_Compartment] == 0Lu ) ||
         ( start_accepted != FALSE )) &&
        ( *okay_to_start_cycle != FALSE ))
    {
        INTERPRETER_CONFIG_TYPE interpreter_config;
        interpreter_config.Setting_File_Index = Spv_Regulations[Current_Compartment].Setting_File_Value;
        interpreter_config.Cycle = Spv_Regulations[Current_Compartment].Selector_Value;
        interpreter_config.Phase = 0;
        interpreter_config.Statement = 0;
        interpreter_config.Cycle_State = Selector__GetCycleState(interpreter_config.Setting_File_Index, Current_Compartment, interpreter_config.Cycle);
        Interpreter__Configure(Current_Compartment, &interpreter_config);

        Supervisor_Delay_Time_Remaining[Current_Compartment] = 0;
        DISPATCH_FSM_EVENT(FSM_EVENT_START);
    }
    else if ((External_Request[Current_Compartment].Start_Value == (uint8)FS_PAUSE_CONDITION) || (pause_accepted))
    {
        External_Request[Current_Compartment].Start_Value = FS_DONT_CARE;
        DISPATCH_FSM_EVENT(FSM_EVENT_PAUSE);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void DelayEntry(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    Supervisor_Data[Current_Compartment].Time_Counter = 0;

    INTERPRETER_CONFIG_TYPE interpreter_config;
    interpreter_config.Setting_File_Index = Spv_Regulations[Current_Compartment].Setting_File_Value;
    interpreter_config.Cycle = Selector__GetStateCycle(Spv_Regulations[Current_Compartment].Setting_File_Value, Current_Compartment, CYCLE_STATE_DELAY);
    interpreter_config.Phase = Spv_Regulations[Current_Compartment].Selector_Value;
    interpreter_config.Statement = 0;
    interpreter_config.Cycle_State = CYCLE_STATE_DELAY;
    Interpreter__Configure(Current_Compartment, &interpreter_config);

    UpdateOkayToStartCycle(FSM_PASS_ARGUMENTS);

    ActivateTask__ResetCompartment(Current_Compartment);

    //Keep DELAY_ENTRY as last item in Entry function
    //Insure that entry tasks have been completed before notifying application of State Change.
    DELAY_ENTRY(Current_Compartment);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void DelayPauseDo(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    if ( Spv_Regulations[Current_Compartment].Start_Changed != (BOOL_TYPE)FALSE )
    {
        Spv_Regulations[Current_Compartment].Start_Changed = FALSE;

        if (Spv_Regulations[Current_Compartment].Start_Value == (uint8)FS_START_CONDITION)
        {
            uint8 start_position = Regulations__GetPositionByFunctionID(FUNC_ID_START, Current_Compartment);

            if ( *okay_to_start_cycle != FALSE )
            {
                Regulations__AcceptRequestByPosition(start_position);

                DISPATCH_FSM_EVENT(FSM_EVENT_RESUME);
            }
            else
            {
                Regulations__RejectRequestByPosition(start_position);
            }
        }
    }
    else if (External_Request[Current_Compartment].Start_Value == (uint8)FS_START_CONDITION)
    {
        External_Request[Current_Compartment].Start_Value = FS_DONT_CARE;
        if ( *okay_to_start_cycle != FALSE )
        {
            DISPATCH_FSM_EVENT(FSM_EVENT_RESUME);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void DelayPauseEntry(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    INTERPRETER_CONFIG_TYPE interpreter_config;
    interpreter_config.Setting_File_Index = Spv_Regulations[Current_Compartment].Setting_File_Value;
    interpreter_config.Cycle = Selector__GetStateCycle(Spv_Regulations[Current_Compartment].Setting_File_Value, Current_Compartment, CYCLE_STATE_PAUSE);
    interpreter_config.Phase = Spv_Regulations[Current_Compartment].Selector_Value;
    interpreter_config.Statement = 0;
    interpreter_config.Cycle_State = CYCLE_STATE_PAUSE;
    Interpreter__Configure(Current_Compartment, &interpreter_config);

    UpdateOkayToStartCycle(FSM_PASS_ARGUMENTS);

    //Keep PAUSE_DELAY_ENTRY as last item in Entry function
    //Insure that entry tasks have been completed before notifying application of State Change.
    DELAY_PAUSE_ENTRY(Current_Compartment);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void DelayPauseExit(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    //Keep DELAY_PAUSE_EXIT as last item in Exit function
    //Insure that exit tasks have been completed before notifying application of State Change.
    DELAY_PAUSE_EXIT(Current_Compartment);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void EndEntry(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    // Round cycle time results
    Supervisor_Data[Current_Compartment].Time_Counter++;
    if (Supervisor_Data[Current_Compartment].Time_Counter >= 5u)
    {
        Supervisor_Data[Current_Compartment].Time_Counter = 0u;
        if (Supervisor_Data[Current_Compartment].Cycle_Elapsed_Time < (uint16)0xFFFF)
        {
            Supervisor_Data[Current_Compartment].Cycle_Elapsed_Time++;
        }
    }

    #if (STATISTICS_SAVED_CYCLES > 0)
        CYCLE_PHASE_STATEMENT_TYPE cps = Interpreter__GetCurrentStatement(Current_Compartment);
        Statistics__CycleStopped(TRUE, Current_Compartment, cps, Supervisor_Data[Current_Compartment].Cycle_Elapsed_Time);
    #endif

    Supervisor_Data[Current_Compartment].Time_Counter = 0;
    Supervisor_Data[Current_Compartment].Cycle_Elapsed_Time = 0;

    if (AUTO_START(Current_Compartment)) //lint !e506 Suppress Warning: [506]: Constant value Boolean [MISRA 2012 Rule 2.1, required] AUTO_START macro
    {
        TestCycleEnd();
        DISPATCH_FSM_EVENT(FSM_EVENT_RESET);
        LOG_ADD_EXCEPTION(AUTO_START_CYCLE_ENDED_RESTARTED, Current_Compartment); // An auto-start compartment transitioned to end state.
    }
    else if (Supervisor_Data[0].Test_Cycle_Active != (BOOL_TYPE)FALSE)
    {
        #if (TEST_CYCLE_STAYS_IN_END_UNTIL_RESET == ENABLED)
            // Save selected test cycle for running the test cycle END macro if ENABLED.
            uint8 test_cycle = Spv_Regulations[Current_Compartment].Selector_Value;
        #endif
        TestCycleEnd();     // Ends test and sets Selector_Value from test cycle back to regulation value

        #if (TEST_CYCLE_STAYS_IN_END_UNTIL_RESET == DISABLED)
        DISPATCH_FSM_EVENT(FSM_EVENT_RESET);
        #else
            ConfigureEndCycle(test_cycle, fsm_var);
        #endif
    }
    else
    {
        ConfigureEndCycle(Spv_Regulations[Current_Compartment].Selector_Value, fsm_var);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void FaultDo(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    if (Spv_Regulations[Current_Compartment].Start_Changed != (BOOL_TYPE)FALSE)
    {
        Spv_Regulations[Current_Compartment].Start_Changed = FALSE;

        uint8 start_position = Regulations__GetPositionByFunctionID(FUNC_ID_START, Current_Compartment);

        if (( Spv_Regulations[Current_Compartment].Start_Value == (uint8)FS_RESET_FAULT ) &&
            ( Fault__IsOkToExitFaultState(Current_Compartment) != FALSE ))
        {
            Regulations__AcceptRequestByPosition(start_position);
            TestCycleEnd();
            DISPATCH_FSM_EVENT(FSM_EVENT_FAULT_RESET);
        }
        else
        {
            Regulations__RejectRequestByPosition(start_position);
        }
    }
    else if (External_Request[Current_Compartment].Start_Value == (uint8)FS_RESET_FAULT)
    {
        External_Request[Current_Compartment].Start_Value = FS_DONT_CARE;
        if (Fault__IsOkToExitFaultState(Current_Compartment) != FALSE)
        {
            TestCycleEnd();
            DISPATCH_FSM_EVENT(FSM_EVENT_FAULT_RESET);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void FaultEntry(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{

    // Statistics responds to the error event when it happens, Supervisor waits for it's next run of the state machine.
    // OK to clear because Statistics has already recorded the elapsed cycle time.
    Supervisor_Data[Current_Compartment].Time_Counter = 0;
    Supervisor_Data[Current_Compartment].Cycle_Elapsed_Time = 0;

    INTERPRETER_CONFIG_TYPE interpreter_config;
    interpreter_config.Setting_File_Index = PRIMARY_SETTING_FILE;
    interpreter_config.Cycle = Selector__GetStateCycle(Spv_Regulations[Current_Compartment].Setting_File_Value, Current_Compartment, CYCLE_STATE_FAULT);
    interpreter_config.Phase = Supervisor_Data[Current_Compartment].Fault_Phase;
    interpreter_config.Statement = 0;
    interpreter_config.Cycle_State = CYCLE_STATE_FAULT;
    Interpreter__Configure(Current_Compartment, &interpreter_config);
    Interpreter__ClearSubCycleStatus(Current_Compartment);

    ActivateTask__ResetCompartment(Current_Compartment);

    Fault__CompartmentManaged(Current_Compartment);

    //Keep FAULT_ENTRY as last item in Entry function
    //Insure that entry tasks have been completed before notifying application of State Change.
    FAULT_ENTRY(Current_Compartment);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Guard function.
 *
 * @return TRUE enable transition; FALSE disable transition
 */
static BOOL_TYPE IsCancelCycleEnabled(FSM_GUARD_FUNCTION_ARGUMENT)
{
    return ( ((uint8)0xFF != Selector__GetCycleByName(0u, Current_Compartment, CYCLE_NAME_CANCEL)) ? TRUE : FALSE );
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Guard function.
 *
 * @return TRUE enable transition; FALSE disable transition
 */
static BOOL_TYPE IsCancellingToFaultEnabled(FSM_GUARD_FUNCTION_ARGUMENT)
{
    BOOL_TYPE transition_enabled = FALSE;

    if (FAULT_TO_CANCELLING_TRANSITION == ENABLED)
    {
        transition_enabled = TRUE;
    }
    
    return (transition_enabled);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void PauseDo(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    if (Spv_Regulations[Current_Compartment].Start_Changed != (BOOL_TYPE)FALSE)
    {
        Spv_Regulations[Current_Compartment].Start_Changed = FALSE;

        if (Spv_Regulations[Current_Compartment].Start_Value == (uint8)FS_START_CONDITION)
        {
            uint8 start_position = Regulations__GetPositionByFunctionID(FUNC_ID_START, Current_Compartment);

            if ( *okay_to_start_cycle != FALSE )
            {
                Regulations__AcceptRequestByPosition(start_position);

                Interpreter__RestoreFromPauseResumeData(Current_Compartment);

                Interpreter__Resume(Current_Compartment);

                DISPATCH_FSM_EVENT(FSM_EVENT_RESUME);
            }
            else
            {
                Regulations__RejectRequestByPosition(start_position);
            }
        }
    }
    else if (External_Request[Current_Compartment].Start_Value == (uint8)FS_START_CONDITION)
    {
        External_Request[Current_Compartment].Start_Value = FS_DONT_CARE;
        if ( *okay_to_start_cycle != FALSE )
        {
            Interpreter__RestoreFromPauseResumeData(Current_Compartment);
            Interpreter__Resume(Current_Compartment);
            DISPATCH_FSM_EVENT(FSM_EVENT_RESUME);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void PauseEntry(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    Interpreter__SavePauseResumeData(Current_Compartment);

    INTERPRETER_CONFIG_TYPE interpreter_config;
    interpreter_config.Setting_File_Index = Spv_Regulations[Current_Compartment].Setting_File_Value;
    interpreter_config.Cycle = Selector__GetStateCycle(Spv_Regulations[Current_Compartment].Setting_File_Value, Current_Compartment, CYCLE_STATE_PAUSE);
    interpreter_config.Phase = Interpreter__GetCurrentStatement(Current_Compartment).Cycle;
    interpreter_config.Statement = 0;
    interpreter_config.Cycle_State = CYCLE_STATE_PAUSE;
    Interpreter__Configure(Current_Compartment, &interpreter_config);

    UpdateOkayToStartCycle(FSM_PASS_ARGUMENTS);

    ActivateTask__PauseCompartment(Current_Compartment);
    Maintain__PauseEntry(Current_Compartment);

    //Keep PAUSE_ENTRY as last item in Entry function
    //Insure that entry tasks have been completed before notifying application of State Change.
    PAUSE_ENTRY(Current_Compartment);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void PauseExit(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    //Keep PAUSE_EXIT as last item in Exit function
    //Insure that exit tasks have been completed before notifying application of State Change.
    PAUSE_EXIT(Current_Compartment);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void ProgrammingDo(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    BOOL_TYPE start_accepted = FALSE;
    uint8 position = Regulations__GetPositionByFunctionID(FUNC_ID_SELECTOR, Current_Compartment);
    if (( position != (uint8)REGULATION_INVALID ) &&
        ( Regulations__IsRegulationChanged(position) != FALSE ))
    {
        uint8 value = (uint8)Regulations__GetRequestedMainValueByPosition(position);
        switch ( Selector__GetCycleState(Spv_Regulations[Current_Compartment].Setting_File_Value, Current_Compartment, value) )
        {
            case CYCLE_STATE_RUN:
            case CYCLE_STATE_PROGRAMMING:
                {
                    if ( value == 0u ) // OFF
                    {
                        ActivateTask__ResetCompartment(Current_Compartment);
                    }

                    Spv_Regulations[Current_Compartment].Selector_Value = value;
                    Regulations__AcceptRequestByPosition(position);

                    INTERPRETER_CONFIG_TYPE interpreter_config;
                    interpreter_config.Setting_File_Index = Spv_Regulations[Current_Compartment].Setting_File_Value;
                    interpreter_config.Cycle = 0;
                    interpreter_config.Phase = Spv_Regulations[Current_Compartment].Selector_Value;
                    interpreter_config.Statement = 0;
                    interpreter_config.Cycle_State = Selector__GetCycleState(interpreter_config.Setting_File_Index, Current_Compartment, interpreter_config.Cycle);
                    Interpreter__Configure(Current_Compartment, &interpreter_config);

                    #if (TIME_ESTIMATION_FEATURE == ENABLED)
                    Estimator__EstimateFast();
                    #endif
                }
                break;
            default:
                Regulations__RejectRequestByPosition(position);
                break;
        }
    }

    if ( Spv_Regulations[Current_Compartment].Start_Changed != (BOOL_TYPE)FALSE)
    {
        Spv_Regulations[Current_Compartment].Start_Changed = FALSE;
        if (Spv_Regulations[Current_Compartment].Start_Value == (uint8)FS_START_CONDITION)
        {
            position = Regulations__GetPositionByFunctionID(FUNC_ID_START, Current_Compartment);
            if (( *okay_to_start_cycle != FALSE ) &&
                ( Spv_Regulations[Current_Compartment].Selector_Value != 0u ))
            {
                start_accepted = TRUE;
                Regulations__AcceptRequestByPosition(position);
            }
            else
            {
                Regulations__RejectRequestByPosition(position);
            }
        }
    }

    if (( External_Request[Current_Compartment].Start_Value == (uint8)FS_START_CONDITION ) ||
        ( start_accepted != FALSE ))
    {
        External_Request[Current_Compartment].Start_Value = FS_DONT_CARE;

        // Selector_Value can change in communication thread after external request.
        // Make sure selector value is still valid before starting.
        if (Spv_Regulations[Current_Compartment].Selector_Value != 0u)
        {
            if (Supervisor_Delay_Time_Remaining[Current_Compartment] == 0Lu)
            {
                INTERPRETER_CONFIG_TYPE interpreter_config;
                interpreter_config.Setting_File_Index = Spv_Regulations[Current_Compartment].Setting_File_Value;
                interpreter_config.Cycle = Spv_Regulations[Current_Compartment].Selector_Value;
                interpreter_config.Phase = 0;
                interpreter_config.Statement = 0;
                interpreter_config.Cycle_State = Selector__GetCycleState(interpreter_config.Setting_File_Index, Current_Compartment, interpreter_config.Cycle);
                Interpreter__Configure(Current_Compartment, &interpreter_config);

                DISPATCH_FSM_EVENT(FSM_EVENT_START);
            }
            else
            {
                DISPATCH_FSM_EVENT(FSM_EVENT_DELAY);
            }
        }
        else
        {
            LOG_ADD_EXCEPTION(SELECTOR_CLEARED_BEFORE_EXTERNAL_START, COMBINE_BYTES(Current_Compartment,Spv_Regulations[Current_Compartment].Selector_Value)); // Selector cleared before external start runs.
            TestCycleEnd();
            DISPATCH_FSM_EVENT(FSM_EVENT_RESET);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void ProgrammingEntry(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    Supervisor_Data[Current_Compartment].Time_Counter = 0;
    Supervisor_Data[Current_Compartment].Cycle_Elapsed_Time = 0;

    #if (CYCLE_DELAY_FEATURE == ENABLED)
    GetDelay(FALSE);
    #endif

    (void)Fault__CompartmentReset(Current_Compartment);

    UpdateOkayToStartCycle(FSM_PASS_ARGUMENTS);
    
    if (( Supervisor_Data[0].Test_Cycle_Selected == (BOOL_TYPE)FALSE ) &&
        ( *okay_to_start_cycle != FALSE ) &&
        ( AUTO_START(Current_Compartment) == TRUE )) //lint !e506 Suppress Warning: [506]: Constant value Boolean [MISRA 2012 Rule 2.1, required] AUTO_START macro
    {
        if (Selector__GetCycleState(Spv_Regulations[Current_Compartment].Setting_File_Value, Current_Compartment, 1) == CYCLE_STATE_RUN)
        {
            INTERPRETER_CONFIG_TYPE interpreter_config;
            interpreter_config.Setting_File_Index = Spv_Regulations[Current_Compartment].Setting_File_Value;
            interpreter_config.Cycle = 1;
            Spv_Regulations[Current_Compartment].Selector_Value = 1;
            interpreter_config.Phase = 0;
            interpreter_config.Statement = 0;
            interpreter_config.Cycle_State = CYCLE_STATE_RUN;
            Interpreter__Configure(Current_Compartment, &interpreter_config);
            DISPATCH_FSM_EVENT(FSM_EVENT_START);
        }
        ActivateTask__ResetCompartment(Current_Compartment);
    }
    else
    {
        //Keep PROGRAMMING_ENTRY as last item in Entry function.
        //Insure that entry tasks have been completed before notifying application of State Change.
        PROGRAMMING_ENTRY(Current_Compartment);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void Reset(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    INTERPRETER_CONFIG_TYPE interpreter_config;
    interpreter_config.Setting_File_Index = Spv_Regulations[Current_Compartment].Setting_File_Value;
    interpreter_config.Cycle = 0;
    interpreter_config.Phase = Spv_Regulations[Current_Compartment].Selector_Value;
    interpreter_config.Statement = 0;
    interpreter_config.Cycle_State = CYCLE_STATE_PROGRAMMING;
    Interpreter__Configure(Current_Compartment, &interpreter_config);
    Interpreter__ClearSubCycleStatus(Current_Compartment);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void ResetTasks(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    ActivateTask__ResetCompartment(Current_Compartment);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void Resume(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    ActivateTask__ResumeCompartment(Current_Compartment);

    RESUME(Current_Compartment);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void RunDo(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    Supervisor_Data[Current_Compartment].Time_Counter++;
    if ( Supervisor_Data[Current_Compartment].Time_Counter >= 10u )
    {
        Supervisor_Data[Current_Compartment].Time_Counter = 0;
        if ( Supervisor_Data[Current_Compartment].Cycle_Elapsed_Time < (uint16)0xFFFF )
        {
            Supervisor_Data[Current_Compartment].Cycle_Elapsed_Time++;
        }
    }

    uint8 phase_position = Regulations__GetPositionByFunctionID(FUNC_ID_ADVANCE_PHASE, Current_Compartment);
    uint8 step_position = Regulations__GetPositionByFunctionID(FUNC_ID_ADVANCE_STEP, Current_Compartment);

    // If else structure that handles requests according to priority.
    // Highest priorities are cancel/pause requests from Interpreter.
    // Next are regulation changes from HMI.
    // After that are external requests from API005 or other modules.
    // Lastly if no requests to change cycle execution, run the estimator count down.
    switch ( Interpreter_Result[Current_Compartment] )
    {
        case INTERPRETER_RESULT_END_CYCLE:
            DISPATCH_FSM_EVENT(FSM_EVENT_END);
            break;
        case INTERPRETER_RESULT_END_CYCLE_RESET:
            {
                #if (STATISTICS_SAVED_CYCLES > 0)
                CYCLE_PHASE_STATEMENT_TYPE cps = Interpreter__GetCurrentStatement(Current_Compartment);
                Statistics__CycleStopped(FALSE, Current_Compartment, cps, Supervisor_Data[Current_Compartment].Cycle_Elapsed_Time);
                #endif

                TestCycleEnd();
                DISPATCH_FSM_EVENT(FSM_EVENT_RESET);
            }
            break;
        case INTERPRETER_RESULT_PAUSE:
            if ( Selector__OkToPauseCycle(Interpreter__GetCurrentSFIndex(Current_Compartment), Current_Compartment, Interpreter__GetStatus().Main_Cycle[Current_Compartment]) != FALSE )
            {
                DISPATCH_FSM_EVENT(FSM_EVENT_PAUSE);
            }
            else
            {
                LOG_ADD_EXCEPTION(ATTEMPT_TO_PAUSE_CYCLE_NOT_ALLOWED, Current_Compartment); // Attempt to pause a cycle that does not allow pause.
            }
            break;
        default:
            if ( Spv_Regulations[Current_Compartment].Start_Changed != (BOOL_TYPE)FALSE )
            {
                Spv_Regulations[Current_Compartment].Start_Changed = FALSE;

                uint8 start_position = Regulations__GetPositionByFunctionID(FUNC_ID_START, Current_Compartment);
                if (( Spv_Regulations[Current_Compartment].Start_Value == (uint8)FS_PAUSE_CONDITION ) &&
                    ( Selector__OkToPauseCycle(Interpreter__GetCurrentSFIndex(Current_Compartment), Current_Compartment, Interpreter__GetStatus().Main_Cycle[Current_Compartment]) != FALSE ))
                {
                    Regulations__AcceptRequestByPosition(start_position);
                    DISPATCH_FSM_EVENT(FSM_EVENT_PAUSE);
                }
                else
                {
                    // FS_RESET_CONDITION and FS_END_CONDITION are handled globally regardless of Supervisor State.
                    // All other start values are invalid for the RUN state.
                    Regulations__RejectRequestByPosition(start_position);
                }
            }
            else if (( phase_position != (uint8)REGULATION_INVALID ) &&
                     ( Regulations__IsRegulationChanged(phase_position) != FALSE ))
            {
                switch ( Interpreter__SetPhase(Current_Compartment, (uint8)Regulations__GetRequestedMainValueByPosition(phase_position)) )
                {
                    case PHASE_STEP_RESULT_PASS:
                        Regulations__AcceptRequestByPosition(phase_position);
                        break;
                    case PHASE_STEP_RESULT_END:
                        Regulations__AcceptRequestByPosition(step_position);
                        DISPATCH_FSM_EVENT(FSM_EVENT_END);
                        break;
                    default:
                        Regulations__RejectRequestByPosition(phase_position);
                        break;
                }
            }
            else if (( step_position != (uint8)REGULATION_INVALID ) &&
                     ( Regulations__IsRegulationChanged(step_position) != FALSE ))
            {
                switch ( Interpreter__SetStep(Current_Compartment, (uint8)Regulations__GetRequestedMainValueByPosition(step_position)) )
                {
                    case PHASE_STEP_RESULT_PASS:
                        Regulations__AcceptRequestByPosition(step_position);
                        break;
                    case PHASE_STEP_RESULT_END:
                        Regulations__AcceptRequestByPosition(step_position);
                        DISPATCH_FSM_EVENT(FSM_EVENT_END);
                        break;
                    default:
                        Regulations__RejectRequestByPosition(step_position);
                        break;
                }
            }
            else if (( External_Request[Current_Compartment].Start_Value == (uint8)FS_PAUSE_CONDITION ) ||
                     ( Supervisor_Data[Current_Compartment].Pause_Request != (BOOL_TYPE)FALSE ))
            {
                External_Request[Current_Compartment].Start_Value = FS_DONT_CARE;
                Supervisor_Data[Current_Compartment].Pause_Request = FALSE;

                if ( Selector__OkToPauseCycle(Interpreter__GetCurrentSFIndex(Current_Compartment), Current_Compartment, Interpreter__GetStatus().Main_Cycle[Current_Compartment]) != FALSE )
                {
                    DISPATCH_FSM_EVENT(FSM_EVENT_PAUSE);
                }
                else
                {
                    LOG_ADD_EXCEPTION(ATTEMPT_TO_PAUSE_CYCLE_NOT_ALLOWED, Current_Compartment); // Attempt to pause a cycle that does not allow pause.
                }
            }
            else if (External_Request[Current_Compartment].Phase != (uint8)PHASE_STEP_NONE)
            {
                if (Interpreter__SetPhase(Current_Compartment, External_Request[Current_Compartment].Phase) == PHASE_STEP_RESULT_END)
                {
                    DISPATCH_FSM_EVENT(FSM_EVENT_END);
                }

                External_Request[Current_Compartment].Phase = PHASE_STEP_NONE;
            }
            else if (External_Request[Current_Compartment].Step != (uint8)PHASE_STEP_NONE)
            {
                if (Interpreter__SetStep(Current_Compartment, External_Request[Current_Compartment].Step) == PHASE_STEP_RESULT_END)
                {
                    DISPATCH_FSM_EVENT(FSM_EVENT_END);
                }
                External_Request[Current_Compartment].Step = PHASE_STEP_NONE;
            }
            else
            {
                #if (TIME_ESTIMATION_FEATURE == ENABLED)
                Estimator__Countdown(Current_Compartment);
                #endif
            }
            break;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void RunEntry(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    //Keep RUN_ENTRY as last item in Entry function
    //Insure that entry tasks have been completed before notifying application of State Change.
    RUN_ENTRY(Current_Compartment);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void RunExit(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    // Application specific macro. Informs application of exit from RUN state.
    RUN_EXIT(Current_Compartment);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Method forces "ok to start" argument to TRUE since this state does not check the OK_TO_START_CYCLE macro
 */
static void SetSystemStateCancelling(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    DefineSystemState(TRUE, SYSTEM_STATE_CANCELLING);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void SetSystemStateDelay(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    DefineSystemState(*okay_to_start_cycle, SYSTEM_STATE_DELAY);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void SetSystemStateDelayPause(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    DefineSystemState(*okay_to_start_cycle, SYSTEM_STATE_DELAY_PAUSE);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Method forces "ok to start" argument to TRUE since this state does not check the OK_TO_START_CYCLE macro
 */
static void SetSystemStateEndOfCycle(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    DefineSystemState(TRUE, SYSTEM_STATE_END_OF_CYCLE);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Method forces "ok to start" argument to TRUE since this state does not check the OK_TO_START_CYCLE macro
 */
static void SetSystemStateFault(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    DefineSystemState(TRUE, SYSTEM_STATE_FAULT);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void SetSystemStatePause(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    DefineSystemState(*okay_to_start_cycle, SYSTEM_STATE_PAUSE);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void SetSystemStateProgramming(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    DefineSystemState(*okay_to_start_cycle, SYSTEM_STATE_PROGRAMMING);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Method forces "ok to start" argument to TRUE since this state does not check the OK_TO_START_CYCLE macro
 */
static void SetSystemStateRun(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    DefineSystemState(TRUE, SYSTEM_STATE_RUN);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void Start(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    ActivateTask__ResetCompartment(Current_Compartment);
    #if (STATISTICS_SAVED_CYCLES > 0)
    Statistics__CycleStarted(Current_Compartment, Spv_Regulations[Current_Compartment].Selector_Value);
    #endif
    START(Current_Compartment);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void StartCancellingCycle(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    INTERPRETER_CONFIG_TYPE interpreter_config;
    interpreter_config.Setting_File_Index = Spv_Regulations[Current_Compartment].Setting_File_Value;
    interpreter_config.Cycle = Selector__GetCycleByName(0u, Current_Compartment, CYCLE_NAME_CANCEL);
    interpreter_config.Phase = 0;
    interpreter_config.Statement = 0;
    interpreter_config.Cycle_State = CYCLE_STATE_RUN;
    Interpreter__Configure(Current_Compartment, &interpreter_config);
    Interpreter__ClearSubCycleStatus(Current_Compartment);

    #if (TIME_ESTIMATION_FEATURE == ENABLED)
    Estimator__EstimateFast();
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
static void UpdateOkayToStartCycle(FSM_CUSTOMIZATION_FUNCTION_ARGUMENT)
{
    *okay_to_start_cycle = OK_TO_START_CYCLE(Current_Compartment);
}

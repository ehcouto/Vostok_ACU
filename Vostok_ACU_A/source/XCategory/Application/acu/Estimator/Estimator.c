/**
 * @file
 * @brief       Estimates the time remaining in all the appliance compartments.
 *
 * @details     The Estimator module steps through all appliance compartments. For each
 *              compartment, it estimates the remaining time for the selected or running cycle.
 *              The Estimator uses the Interpreter to determine which cycle is selected and
 *              the current step executing in the cycle.
 *
 *              The Estimator uses time entries to predict cycle times. Time entries are assigned
 *              to phases. The time is only counted in the estimate if the following conditions
 *              are satisfied:
 *                  - The phase has a time assigned to it.
 *                  - At least one statement with an N-Flag set is predicted to execute.
 *              The Estimator predicts the execution path by evaluating Jump If process and Set
 *              State process statements.
 *
 *              If the estimated time calculation is taking too long, or if the predicted time is
 *              growing too large, then the Estimator will just use the MAXIMUM_ESTIMATED_SECONDS
 *              value as the time estimation for the compartment.
 *
 *              The estimation of each cycle is done in two parts. The first part is based on
 *              the most recently encountered time entry. The second part is based on the time
 *              entries for the rest of the cycle. The total time estimate is the sum of these two
 *              parts.
 *
 *              While a cycle is running, the Supervisor calls the Estimator__Countdown() function
 *              to update the estimated remaining time for the compartment. The Estimator will
 *              count down seconds until the time in the first part of the estimate has been
 *              consumed.
 *
 * @copyright   Copyright 2016-2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "SystemConfig.h"

#ifndef TIME_ESTIMATION_FEATURE
    #error TIME_ESTIMATION_FEATURE must be defined via SystemConfig.h as ENABLED or DISABLED.
#endif

#include "Estimator.h"
#if (TIME_ESTIMATION_FEATURE == ENABLED)
    #include "Estimator_prv.h"
#endif

// -- Other Modules --
#include "Interpreter.h"
#if (TIME_ESTIMATION_FEATURE == ENABLED)
    #include "JumpIf.h"
    #include <limits.h>
    #include "Log.h"
    #include "Micro.h"
    #include "Modifier.h"
    #include "NVData.h"
    #include "Reveal.h"
    #include "Selector.h"
    #include "SettingFile.h"
    #include <stdlib.h>
    #include "Supervisor.h"
#endif
#include <string.h>


// Set ESTIMATOR_TRACE to ENABLED to provide test information about the estimation.
#define ESTIMATOR_TRACE DISABLED


//  --- Private Declarations ------------------------------------------------------------------------------------------


// -- Private Constant Declarations --

//! The number of times the Estimator__Background() function gets called every second.
#define ESTIMATOR_CALLS_PER_SECOND 10

//! The API used to publish estimated time information when ESTIMATOR_TRACE is ENABLED.
#define ESTIMATOR_DEBUG_API 4

//! Number of seconds difference from one calculation to the next that will trigger a publication
//! of debug data.
#define ESTIMATOR_DEBUG_DIFF_SECS_TO_PUBLISH 5

//! Maximum number of entries that will fit in a single Reveal payload.
//! Time entries are uint16 and consume 2 bytes each.
//! Prediction entries are a pair of uint8 and consume 2 bytes each.
#define ESTIMATOR_DEBUG_MAX_ENTRIES_PER_PAYLOAD ((REVEAL_PAYLOAD_SIZE - 1) / 2)

//! The OpCode used to publish estimated phase times when ESTIMATOR_TRACE is ENABLED.
#define ESTIMATOR_DEBUG_OPCODE_PHASE_TIMES 11

//! The OpCode used to publish predicted statement execution when ESTIMATOR_TRACE is ENABLED.
#define ESTIMATOR_DEBUG_OPCODE_STATEMENTS 12

//! The maximum number of statments to store when the Trace feature is enabled.
#define ESTIMATOR_MAX_STATEMENTS 400

//! The maximum number of Bytes for statements to store when the Trace feature is enabled.
#define ESTIMATOR_MAX_STATEMENT_BYTES (ESTIMATOR_MAX_STATEMENTS * 2)

//! The maximum number of times to store when the Trace feature is enabled.
#define ESTIMATOR_MAX_PHASES 60

//! The maximum number of seconds that will be estimated for a single cycle.
// Value must be a uint16 value.
// 36000 seconds == 10 hours
// 54000 seconds == 15 hours
#define MAXIMUM_ESTIMATED_SECONDS 54000

//! The maximum number of statements that will be evaluated before assuming an infinite loop in the cycle.
#ifndef MAX_STATEMENTS_PER_CYCLE
    #define MAX_STATEMENTS_PER_CYCLE 1000
#endif

//! The name of this module for use by the Log module.
#define MODULE_NAME MODULE_ESTIMATOR


// -- Private Enumerated Constant Declarations --

//! Possible estimation states for a compartment.
typedef enum ESTIMATOR_COMPARTMENT_STATE_ENUM
{
    ESTIMATOR_INITIALIZING,
    ESTIMATOR_NO_CYCLE_SELECTED,
    ESTIMATOR_CYCLE_SELECTED,
    ESTIMATOR_CYCLE_STARTED
} ESTIMATOR_COMPARTMENT_STATE_TYPE;

//! A list of log message IDs that are generated by this module.
typedef enum MODULE_ESTIMATOR_LOG_MESSAGE_ID_ENUM
{
    FIRST_STATEMENT_OUT_OF_RANGE = 1,
    ERROR_READING_SELECTOR_DATA,
    CANCELLED_DUE_TO_TOO_MUCH_TIME,
    CANCELLED_DUE_TO_PROBABLE_INFINITE_LOOP
} MODULE_ESTIMATOR_LOG_MESSAGE_ID_TYPE;

//! Possible F1F0 options for the SetState process
typedef enum SET_STATE_F1_F0_FLAG_ENUM
{
    SET_STATE_NOP,
    SET_STATE_END_CYCLE,
    SET_STATE_END_SUB_CYCLE
} SET_STATE_F1_F0_FLAG_TYPE;

//! Lo Statement values for the SET_STATE_END_CYCLE option.
typedef enum SET_STATE_END_CYCLE_ENUM
{
    END_CYCLE_TO_END,
    END_CYCLE_TO_PAUSE,
    END_CYCLE_TO_PROGRAMMING
} SET_STATE_END_CYCLE_TYPE;

//! Lo Statement values for the SET_STATE_END_SUB_CYCLE option.
typedef enum SET_STATE_END_SUB_CYCLE_ENUM
{
    END_SUB_CYCLE_RESUME_STEP,
    END_SUB_CYCLE_RESTART_STEP,
    END_SUB_CYCLE_RESTART_PHASE,
    END_SUB_CYCLE_SKIP_PHASE
} SET_STATE_END_SUB_CYCLE_TYPE;


// -- Private Structure Type Declarations --

//! The setting file data structure used to define a single time entry.
//! There are three versions of the data structure:
//!     2-byte: Uses the Simple structure and has no modifiers
//!     4-byte: Uses the Full structure and either has no modifier or uses an 8-bit modifier
//!     6-byte: Uses the Full structure and uses a 16-bit modifier
typedef PACKED struct SF_TIME_ENTRY_STRUCT
{
    uint8 First_Statement       : 7;
    uint8 Format                : 1;                // 0 = Simple, 1 = Full
    union TIME_ESTIMATE_UNION
    {
        struct TIME_ENTRY_SIMPLE_STRUCT
        {
            uint8 Value                 : 6;
            uint8 Scale                 : 2;
        } Simple;
        PACKED struct TIME_ENTRY_FULL_STRUCT
        {
            uint8 Modifier_Byte;
            uint16 Seconds;
            uint16 Modifier_Word;
        } Full;
    } Type;
} SF_TIME_ENTRY_TYPE;

//! The data structure used to remember the first time entry of a cycle prediction.
//! There is a first time entry associated with both the main cycle and any active sub-cycle.
//! If the cycle has started running, then the first time entry is the time entry that was most
//! recently executed (not skipped).
typedef struct FIRST_TIME_ENTRY_DATA_STRUCT
{
    //! A pointer to the setting file time entry data for the first time entry.
    SF_TIME_ENTRY_TYPE* First_Time_Entry;

    //! A counter that counts time since the first time entry.
    //! Units are based on #ESTIMATOR_CALLS_PER_SECOND.
    sint32 Calls_Since_Time_Entry;

    //! The cycle that contains the phase with the first time entry.
    uint8 Cycle;

    //! The phase with the first time entry.
    uint8 Phase;
} FIRST_TIME_ENTRY_DATA_TYPE;

//! A structure with the volatile variables used by this module.
typedef struct ESTIMATOR_DATA_STRUCT
{
    //! A pointer to the time entry of the phase that is actively being estimated.
    SF_TIME_ENTRY_TYPE* Sf_Time_Entry;

    //! The number of predicted statements in single cycle. If this value is too large
    //! (> #MAX_STATEMENTS_PER_CYCLE), then the cycle is assumed to have an infinite
    //! prediction loop.
    uint16 Statements_In_Cycle;

    //! Definition of process lo statement sizes. Bit 'n' represents process 'n'.
    //! Bit value of 0 = Process 'n' uses an 8-bit lo statement.
    //! Bit value of 1 = Process 'n' uses a 16-bit lo statement.
    uint16 Lo_Statement_Length_Map;

    //! A copy of Wip_Seconds. This variable should be removed to save RAM.
    uint16 Seconds_After_Time_Entry[APPLIANCE_MAX_COMPARTMENTS];

    //! The remaining number of statements to predict in a single call to the
    //! Estimator__Background() function. If this value reaches zero, then the estimation is
    //! paused and resumed on the next call to the Estimator__Background() function.
    uint16 Statement_Countdown;

    //! The number of seconds estimated for the execution of the current cycle being predicted.
    //! This value does not include the first time entry. (WIP = work in progress)
    uint16 Wip_Seconds;

    //! The run time data for the cycle that will be resumed after the current sub-cycle ends.
    RUN_TIME_DATA_TYPE Return_Data;

    //! The run time data of the cycle that is currently running and should be estimated.
    RUN_TIME_DATA_TYPE Running_Data;

    //! The phase data (from Selector) for the cycle and phase that is currently being predicted.
    PHASE_DATA_TYPE Phase_Data;

    //! Determined state of each compartment's estimation.
    //! This value is the result of the most recent estimation in a compartment.
    ESTIMATOR_COMPARTMENT_STATE_TYPE State[APPLIANCE_MAX_COMPARTMENTS];

    //! A working copy (WIP = work in progress) of the compartment's estimation state.
    //! This value is the state of the current estimation.
    ESTIMATOR_COMPARTMENT_STATE_TYPE Wip_State;

    //! The running cycle the last time that an estimate was started.
    //! Estimations are only calculated when this value is the same for two consecutive calls.
    uint8 Last_Estimated_Cycle[APPLIANCE_MAX_COMPARTMENTS];

    //! The running phase the last time that an estimate was started.
    //! Estimations are only calculated when this value is the same for two consecutive calls.
    uint8 Last_Estimated_Phase[APPLIANCE_MAX_COMPARTMENTS];

    //! The compartment that is currently being estimated.
    //! Used as an index into the arrays of APPLIANCE_MAX_COMPARTMENTS elements.
    uint8 Compartment                   : 4;

    //! TRUE when remaining times must be calculated quickly for all compartments.
    //! This condition occurs when at least one compartment is in the programming state.
    uint8 Do_Fast_Estimates             : 1;

    //! TRUE while actively estimating a compartment's remaining time.
    //! This value is used to track estimations across multiple calls.
    uint8 Is_Estimating                 : 1;

    //! TRUE if the displacement for the modifier is the size of a word, else FALSE.
    //! If one modifier displacement is a word, all of them will be words.
    uint8 Is_Modifier_Displacement_Word : 1;

    //! TRUE when a phase with a time is first encountered during the prediction.
    //! FALSE as soon as the time for the phase is consumed.
    //! Ensures that the time for a phase is never counted more than once.
    uint8 Phase_Time_Counted            : 1;

    #if (ESTIMATOR_TRACE == ENABLED)
        //! Trace debugging data with predicted phase times.
        //! Times[n] = predicted seconds for phase n.
        uint16 Times[ESTIMATOR_MAX_PHASES];

        //! Index into the Statements[] array of statements.
        uint16 Statement_Index;

        //! The cycle associated with the last estimated statement.
        //! Used to determine when a change in cycle is predicted.
        uint8 Cycle_Of_Last_Statement;

        //! Trace debugging data with the predicted statement execution sequence.
        //! Each entry consists of two bytes:
        //!     Cycle initialization or change represented by {0xCC, cycle_number}.
        //!     Statement execution represented by {phase_number, statement_number}.
        uint8 Statements[ESTIMATOR_MAX_STATEMENT_BYTES];

        //! Index into the Times[] array of times.
        uint8 Time_Index;
    #endif
} ESTIMATOR_DATA_TYPE;

//! A structure with the non-volatile variables used by this module.
typedef struct ESTIMATOR_NV_DATA_STRUCT
{
    //! The number of seconds that the module predicts will be repeated after a sub-cycle ends.
    //! If this number is negative, then the prediction is to skip a number of seconds.
    sint16 Repeat_Seconds[APPLIANCE_MAX_COMPARTMENTS];

    //! Seconds elapsed after the last phase with a time entry and before the start of the current phase.
    uint16 Seconds_Elapsed_Before_Phase[APPLIANCE_MAX_COMPARTMENTS];

    //! The recent time entries for the selected or running cycles in all the compartments.
    //! The cycle can be a normal user cycle or a sub-cycle.
    FIRST_TIME_ENTRY_DATA_TYPE Running[APPLIANCE_MAX_COMPARTMENTS];

    //! The recent time entries for the main cycles that will resume after their respective
    //! sub-cycle finish running. This value is only used when the 'Running' field refers to a
    //! time entry for a sub-cycle.
    FIRST_TIME_ENTRY_DATA_TYPE Return[APPLIANCE_MAX_COMPARTMENTS];
} ESTIMATOR_NV_DATA_TYPE;


//  --- Private Definitions -------------------------------------------------------------------------------------------


// -- Private Flash-Based Constant Definitions --

#if (TIME_ESTIMATION_FEATURE == ENABLED)
    //! The Multiplier[] array returns multiplication factors.
    //! The index into the array (of type ESTIMATION_SCALE_TYPE) is used to get a specific multiplication factor.
    static const uint8 MULTIPLIER[] = { 1, 5, 30, 120 };
#endif


// -- Private Variable Definitions --

#if (TIME_ESTIMATION_FEATURE == ENABLED)
    #pragma location = "_appliance_segment"
    //! A (non-static) system variable for the estimated remaining time of each compartment.
    uint16 Estimated_Remaining_Time[APPLIANCE_MAX_COMPARTMENTS];

    //! Data used by the Estimator module across all compartments.
    static ESTIMATOR_DATA_TYPE Estimator;

    #pragma location = "_appliance_segment"
    //! Non-volatile data used by the Estimator module across all compartments.
    static ESTIMATOR_NV_DATA_TYPE Estimator_Nv;
#else
    //! A (non-static) system variable for the estimated remaining time of each compartment.
    uint16 Estimated_Remaining_Time[APPLIANCE_MAX_COMPARTMENTS];
#endif


//  --- Private Function Prototypes -----------------------------------------------------------------------------------

#if (TIME_ESTIMATION_FEATURE == ENABLED)
    static void AdjustTimeForResumingMainCycle(void);
    static void AdvanceToNextPhase(void);
    static void AdvanceToNextStatement(void);
    static void CalculateLoStatementIndex(void);
    static BOOL_TYPE EstimationBegin(void);
    static void EstimationContinue(void);
    static uint16 GetLoStatement(PROCESS_OPCODE_ENUM_TYPE op_code);
    static uint16 GetSecondsFromTimeEntry(SF_TIME_ENTRY_TYPE* te);
    static void LoadEstimationData(RUN_TIME_DATA_TYPE* in, FIRST_TIME_ENTRY_DATA_TYPE* out, BOOL_TYPE always_update);
    static SF_TIME_ENTRY_TYPE* LoadPhaseDataAndGetTimeEntry(RUN_TIME_DATA_TYPE* run_data);
    static BOOL_TYPE PredictNextStatement(void);
    #if (ESTIMATOR_TRACE == ENABLED)
        static void PublishDebugData(void);
    #endif
    static void SetEstimatedRemainingTime(uint8 compartment);
    static BOOL_TYPE SetStateEstimate(void);
#endif


//=====================================================================================================================
//  --- Public Functions ----------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Initialize all Estimator module variables.
 */
void Estimator__Initialize(void)
{
    #if (TIME_ESTIMATION_FEATURE == ENABLED)
        // Initialize all module variables
        memset(&Estimator, 0, sizeof(Estimator));
        // memset also clears Estimator.Wip_State to ESTIMATOR_INITIALIZING.
        Estimator.Lo_Statement_Length_Map = *Selector__GetLoStatmentIndexLengthPtr();

        if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
        {
            if (NVData__GetLastRestoreStatus()->Bit.Appliance_Data_Recovered != NVDATA_RESTORE_OKAY)
            {
                memset(Estimated_Remaining_Time, UCHAR_MAX, sizeof(Estimated_Remaining_Time));
                memset(&Estimator_Nv, 0, sizeof(Estimator_Nv));
            }
        }
    #else
        memset(Estimated_Remaining_Time, UCHAR_MAX, sizeof(Estimated_Remaining_Time));
    #endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Background function for the Estimator module.
 * This function updates the time estimation variables for each of the compartments.
 */
void Estimator__Background(void)
{
    #if (TIME_ESTIMATION_FEATURE == ENABLED)
        // Set number of statements to evaluate depending on running (normal) or programming (fast).
        Estimator.Statement_Countdown = (Estimator.Do_Fast_Estimates == TRUE) ? ESTIMATOR_FAST_STATEMENTS_PER_CALL : ESTIMATOR_NORMAL_STATEMENTS_PER_CALL;

        // Predict until there are no Statement_Countdown or all compartments are estimated.
        while (Estimator.Statement_Countdown > 0)
        {
            // Check for an estimation starting in a new compartment.
            BOOL_TYPE is_stable_enough_to_predict = TRUE;
            if (Estimator.Is_Estimating == FALSE)
            {
                is_stable_enough_to_predict = EstimationBegin();
            }

            // Continue an estimation that already started.
            if (Estimator.Is_Estimating == TRUE)
            {
                EstimationContinue();
            }

            // Check for the completion of an estimation.
            if (Estimator.Is_Estimating == FALSE)
            {
                if (is_stable_enough_to_predict == TRUE)
                {
                    // Save Estimator values for the completed compartment.
                    Estimator.State[Estimator.Compartment] = Estimator.Wip_State;
                    Estimator.Seconds_After_Time_Entry[Estimator.Compartment] = Estimator.Wip_Seconds;

                    // Set the estimated remaining time for the compartment.
                    SetEstimatedRemainingTime(Estimator.Compartment);
                }

                // Move to the next compartment
                Estimator.Compartment = (Estimator.Compartment + 1) % APPLIANCE_MAX_COMPARTMENTS;
                if (Estimator.Compartment == 0)
                {
                    // Finished estimating all compartments.
                    Estimator.Do_Fast_Estimates = FALSE;
                    break;                      // Break out of the while() loop.
                }
                #if (ESTIMATOR_TRACE == ENABLED)
                    Estimator.Compartment = 0;
                    Estimator.Do_Fast_Estimates = FALSE;
                    break;                      // Break out of the while() loop.
                #endif
            }
        }
    #endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Countdown time for the specified compartment based on the most recent Estimator data.
 * This function must only be called when the user cycle is running.
 * @param compartment = The compartment in which to count down time.
 */
void Estimator__Countdown(uint8 compartment)
{
    #if (TIME_ESTIMATION_FEATURE == ENABLED)
        if (compartment < APPLIANCE_MAX_COMPARTMENTS)
        {
            // Update the elapsed time.
            if (Estimator_Nv.Running[compartment].Calls_Since_Time_Entry < ESTIMATOR_CALLS_PER_SECOND * MAXIMUM_ESTIMATED_SECONDS)
            {
                Estimator_Nv.Running[compartment].Calls_Since_Time_Entry++;
            }
        }
    #endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Interrupt any current estimations and start fast estimations in all compartments.
 */
void Estimator__EstimateFast(void)
{
    #if (TIME_ESTIMATION_FEATURE == ENABLED)
        Estimator.Do_Fast_Estimates = TRUE;
        Estimator.Is_Estimating = FALSE;
        Estimator.Compartment = 0;
    #endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the initial estimate of the current block for the specified compartment.
 * @param compartment = The compartment whose initial estimate is requested.
 * @return The initial estimate of the current block for the specified compartment.
 */
uint16 Estimator__GetBlockInitialEstimate(uint8 compartment)
{
    #if (TIME_ESTIMATION_FEATURE == ENABLED)
        if (compartment < APPLIANCE_MAX_COMPARTMENTS)
        {
            return GetSecondsFromTimeEntry(Estimator_Nv.Running[compartment].First_Time_Entry);
        }
    #endif
    return 0;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the seconds elapsed in the phase(s) associated with the first time entry.
 * @param compartment = The compartment for which the elapsed time is requested.
 * @return The seconds elapsed in the phase(s) associated with the first time entry.
 */
uint16 Estimator__GetElapsedTimeInCurrentBlock(uint8 compartment)
{
    #if (TIME_ESTIMATION_FEATURE == ENABLED)
        if ((compartment < APPLIANCE_MAX_COMPARTMENTS) &&
            (Estimator.State[compartment] == ESTIMATOR_CYCLE_STARTED))
        {
            return Estimator_Nv.Running[compartment].Calls_Since_Time_Entry / ESTIMATOR_CALLS_PER_SECOND;
        }
    #endif
    return 0;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the estimated remaining time for the specified compartment.
 * @param compartment = Specifies which estimated remaining time should be returned.
 * @return The estimated remaining time for the specified compartment.
 */
uint16 Estimator__GetRemainingSeconds(uint8 compartment)
{
    #if (TIME_ESTIMATION_FEATURE == ENABLED)
        if (compartment < APPLIANCE_MAX_COMPARTMENTS)
        {
            return Estimated_Remaining_Time[compartment];
        }
    #endif
    return 0;
}


//=====================================================================================================================
//  --- Private Functions ---------------------------------------------------------------------------------------------
//=====================================================================================================================


#if (TIME_ESTIMATION_FEATURE == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Adjust the estimated time when a sub-cycle ends and the main cycle resumes.
 * This function manages the time that was spent in the first time entry of the main cycle before
 * the sub-cycle was triggered.
 */
static void AdjustTimeForResumingMainCycle(void)
{
    SF_TIME_ENTRY_TYPE* te = Estimator_Nv.Return[Estimator.Compartment].First_Time_Entry;
    if (te != NULL)
    {
        sint16 seconds = GetSecondsFromTimeEntry(te) -
                         Estimator_Nv.Return[Estimator.Compartment].Calls_Since_Time_Entry / ESTIMATOR_CALLS_PER_SECOND;
        if (seconds > 0)
        {
            Estimator.Wip_Seconds += seconds;
        }
    }
    Estimator.Wip_Seconds += Estimator_Nv.Repeat_Seconds[Estimator.Compartment];
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Move to the next phase.
 * If a sub-cycle ends, then returns from the sub-cycle and adjusts the estimated time.
 * Sets the Running_Data.Cps.Cycle to 0 if the cycle ends.
 * This function does not update the phase statements or the phase time entry.
 */
static void AdvanceToNextPhase(void)
{
    Estimator.Running_Data.Cps.Phase++;
    Estimator.Running_Data.Cps.Statement = 0;
    Estimator.Running_Data.Lo_Statement_Index = 0;
    if (Estimator.Running_Data.Cps.Phase >= Estimator.Phase_Data.Number_Of_Cycle_Phases)
    {
        if (Estimator.Return_Data.Cps.Cycle != 0)
        {
            // Sub-cycle ended. Resume the main cycle.
            AdjustTimeForResumingMainCycle();
            Estimator.Running_Data = Estimator.Return_Data;
            Estimator.Return_Data.Cps.Cycle = 0;
        }
        else
        {
            // Main cycle ended.
            Estimator.Running_Data.Cps.Cycle = 0;
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Move to the next statement.
 * If a sub-cycle ends, then returns from the sub-cycle and adjusts the estimated time.
 * Sets the Running_Data.Cps.Cycle to 0 if the cycle ends.
 * This function does not update the phase statements or the phase time entry.
 */
static void AdvanceToNextStatement(void)
{
    if ((Estimator.Running_Data.Cps.Statement + 1) >= Estimator.Phase_Data.Number_Of_Phase_Statements)
    {
        AdvanceToNextPhase();
    }
    else
    {
        HI_STATEMENT_TYPE hi_statement = Estimator.Phase_Data.Hi_Statements[Estimator.Running_Data.Cps.Statement];
        Estimator.Running_Data.Lo_Statement_Index += BIT_TEST(Estimator.Lo_Statement_Length_Map, hi_statement.Op_Code) + 1;
        Estimator.Running_Data.Cps.Statement++;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Set the Lo_Statement_Index for the current statement.
 */
static void CalculateLoStatementIndex(void)
{
    uint16 lo_statement_index = 0;
    uint8 statement = Estimator.Running_Data.Cps.Statement;
    for (uint8 index = 1; index <= statement; index++)
    {
        HI_STATEMENT_TYPE hi_statement = Estimator.Phase_Data.Hi_Statements[index - 1];
        lo_statement_index += 1 + BIT_TEST(Estimator.Lo_Statement_Length_Map, hi_statement.Op_Code);
    }
    Estimator.Running_Data.Lo_Statement_Index = lo_statement_index;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Begin a new estimation in the current compartment.
 */
static BOOL_TYPE EstimationBegin(void)
{
    // Reset variables from previous estimations.
    Estimator.Sf_Time_Entry = NULL;

    // Assume that the current cycle and phase is stable enough to predict.
    BOOL_TYPE is_stable_enough_to_predict = TRUE;

    // Get the current state from the compartment's Interpreter.
    // Determine if a cycle is selected and/or running.
    // Determine if a sub-cycle is running.
    // Determine the current cycle, phase, and statement.
    // Determine the elapsed time in the current step.
    BOOL_TYPE cycle_started = Interpreter__GetRunTimeData(Estimator.Compartment,
                                                          &Estimator.Running_Data,
                                                          &Estimator.Return_Data);
    uint8 selector = Supervisor__IsInPausedDelay(Estimator.Compartment);
    if (selector != 0)
    {
        Estimator.Running_Data.Cps.Cycle = selector;
        Estimator.Running_Data.Cps.Phase = 0;
        Estimator.Running_Data.Cps.Statement = 0;
    }

    #if (ESTIMATOR_TRACE == ENABLED)
        // Initialize the trace buffers.
        memset(&Estimator.Statements, 0xCC, sizeof(Estimator.Statements));
        memset(&Estimator.Times, 0xCC, sizeof(Estimator.Times));
        // Initialize the indices into the Statements[] and Times[] buffers.
        Estimator.Statement_Index = 0;
        Estimator.Time_Index = Estimator.Running_Data.Cps.Phase;
        // Set the last cycle as invalid to trigger a new predicted cycle later.
        Estimator.Cycle_Of_Last_Statement = 0xFF;
    #endif

    // Check if the compartment Interpreter is OFF (no cycle selected).
    FIRST_TIME_ENTRY_DATA_TYPE* run = &Estimator_Nv.Running[Estimator.Compartment];
    FIRST_TIME_ENTRY_DATA_TYPE* ret = &Estimator_Nv.Return[Estimator.Compartment];
    if (Estimator.Running_Data.Cps.Cycle == 0)
    {
        // No cycle is selected or running.
        Estimator.Wip_State = ESTIMATOR_NO_CYCLE_SELECTED;
        Estimator.Wip_Seconds = NO_ESTIMATION;
        memset(ret, 0, sizeof(FIRST_TIME_ENTRY_DATA_TYPE));
        memset(run, 0, sizeof(FIRST_TIME_ENTRY_DATA_TYPE));
    }
    else
    {
        BOOL_TYPE phase_changed = FALSE;
        if ((cycle_started == FALSE) ||
            ((Estimator.Last_Estimated_Cycle[Estimator.Compartment] == Estimator.Running_Data.Cps.Cycle) &&
             (Estimator.Last_Estimated_Phase[Estimator.Compartment] == Estimator.Running_Data.Cps.Phase)))
        {
            // Restart the time estimation.
            Estimator.Wip_Seconds = 0;
            Estimator.Statements_In_Cycle = 0;
            Estimator.Is_Estimating = TRUE;
            Estimator.Phase_Time_Counted = FALSE;
        }
        else
        {
            // If the phase changed, then wait before predicting.
            is_stable_enough_to_predict = FALSE;
            Estimator.Last_Estimated_Cycle[Estimator.Compartment] = Estimator.Running_Data.Cps.Cycle;
            Estimator.Last_Estimated_Phase[Estimator.Compartment] = Estimator.Running_Data.Cps.Phase;
            phase_changed = TRUE;
        }

        if (cycle_started == FALSE)
        {
            // A cycle is selected, but not started.
            Estimator.Wip_State = ESTIMATOR_CYCLE_SELECTED;
            memset(ret, 0, sizeof(FIRST_TIME_ENTRY_DATA_TYPE));
            LoadEstimationData(&Estimator.Running_Data, run, TRUE);
        }
        else if ((ret->Cycle == 0) &&
                 (Estimator.Return_Data.Cps.Cycle != 0))
        {
            // A sub-cycle started since the last estimate in this compartment.
            Estimator.Wip_State = ESTIMATOR_CYCLE_STARTED;
            memcpy(ret, run, sizeof(FIRST_TIME_ENTRY_DATA_TYPE));
            LoadEstimationData(&Estimator.Return_Data, ret, FALSE);
            LoadEstimationData(&Estimator.Running_Data, run, TRUE);
        }
        else if ((ret->Cycle != 0) &&
                 (Estimator.Return_Data.Cps.Cycle == 0))
        {
            // A sub-cycle ended since the last estimate in this compartment.
            Estimator.Wip_State = ESTIMATOR_CYCLE_STARTED;
            memcpy(run, ret, sizeof(FIRST_TIME_ENTRY_DATA_TYPE));
            run->Calls_Since_Time_Entry -= Estimator_Nv.Repeat_Seconds[Estimator.Compartment] * ESTIMATOR_CALLS_PER_SECOND;
            memset(ret, 0, sizeof(FIRST_TIME_ENTRY_DATA_TYPE));
            LoadEstimationData(&Estimator.Running_Data, run, FALSE);
        }
        else
        {
            // No sub-cycle started or ended since the last estimate in this compartment.
            Estimator.Wip_State = ESTIMATOR_CYCLE_STARTED;
            LoadEstimationData(&Estimator.Running_Data, run, FALSE);

            // If the phase changed, and if no sub-cycle is running, then save the seconds elapsed
            // between the start of the last phase with a time estimate and the current phase.
            if ((phase_changed == TRUE) && (Estimator.Return_Data.Cps.Cycle == 0))
            {
                sint32 seconds_elapsed_before_phase = 0;
                // Only record a non-zero elapsed time if the phase that just started does not have a time entry.
                if (Estimator.Sf_Time_Entry == NULL)
                {
                    // Get the time elapsed since the start of the phase with the last time entry.
                    seconds_elapsed_before_phase = run->Calls_Since_Time_Entry / ESTIMATOR_CALLS_PER_SECOND;
                    if (seconds_elapsed_before_phase > USHRT_MAX)
                    {
                        seconds_elapsed_before_phase = USHRT_MAX;
                    }
                }
                Estimator_Nv.Seconds_Elapsed_Before_Phase[Estimator.Compartment] = (uint16)seconds_elapsed_before_phase;
            }
        }
    }
    if (Estimator.Return_Data.Cps.Cycle == 0)
    {
        // No sub-cycle is running. Forget the elapsed time to repeat.
        Estimator_Nv.Repeat_Seconds[Estimator.Compartment] = 0;
    }

    #if (ESTIMATOR_TRACE == ENABLED)
        if (Estimator.Time_Index < ESTIMATOR_MAX_PHASES)
        {
            // Record the time remaining in the current phase.
            uint16 seconds = GetSecondsFromTimeEntry(Estimator_Nv.Running[Estimator.Compartment].First_Time_Entry);
            uint16 elapsed = Estimator_Nv.Running[Estimator.Compartment].Calls_Since_Time_Entry / ESTIMATOR_CALLS_PER_SECOND;
            if (seconds > elapsed)
            {
                Estimator.Times[Estimator.Time_Index] = seconds - elapsed;
            }
            else
            {
                Estimator.Times[Estimator.Time_Index] = 0;
            }
        }
    #endif

    return is_stable_enough_to_predict;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Continue an estimation in the current compartment.
 * Repeat until ((Statement_Countdown == 0) || (estimated time > max) || (prediction is complete) || (predict infinite loop)):
 *      1. Predict behavior of the step (update CPS)
 *      2. If ((no jump is predicted) && (N_Flag is set) && (phase has time) && (phase time has not been counted)
 *         Then count the phase time
 *      3. If ((the cycle changed) || (the phase changed))
 *         Then load new phase data and get new phase time
 */
static void EstimationContinue(void)
{
    while (Estimator.Statement_Countdown > 0)
    {
        // Update the number of statements evaluated on this Estimator__Background() function call.
        Estimator.Statement_Countdown--;

        #if (ESTIMATOR_TRACE == ENABLED)
            if (Estimator.Running_Data.Cps.Cycle != Estimator.Cycle_Of_Last_Statement)
            {
                // Record that the predicted execution entered a new cycle.
                if (Estimator.Statement_Index < ESTIMATOR_MAX_STATEMENT_BYTES - 1)
                {
                    Estimator.Statement_Index++;
                    Estimator.Statements[Estimator.Statement_Index++] = Estimator.Running_Data.Cps.Cycle;
                    Estimator.Cycle_Of_Last_Statement = Estimator.Running_Data.Cps.Cycle;
                }
            }
            if ((Estimator.Time_Index != Estimator.Running_Data.Cps.Phase) &&
                (Estimator.Running_Data.Cps.Phase < ESTIMATOR_MAX_PHASES))
            {
                // Record that the predicted execution entered a new phase.
                // Assume 0 seconds for now.
                Estimator.Time_Index = Estimator.Running_Data.Cps.Phase;
                Estimator.Times[Estimator.Time_Index] = 0;
            }
            if (Estimator.Statement_Index < ESTIMATOR_MAX_STATEMENT_BYTES - 1)
            {
                // Record that the predicted execution entered a new statement.
                Estimator.Statements[Estimator.Statement_Index++] = Estimator.Running_Data.Cps.Phase;
                Estimator.Statements[Estimator.Statement_Index++] = Estimator.Running_Data.Cps.Statement;
            }
        #endif

        // Remember the current cycle and phase and statement n-flag.
        uint8 cycle_before = Estimator.Running_Data.Cps.Cycle;
        uint8 phase_before = Estimator.Running_Data.Cps.Phase;
        BOOL_TYPE n_flag = Estimator.Phase_Data.Hi_Statements[Estimator.Running_Data.Cps.Statement].N_Flag;

        // Estimation behavior depends on whether the statement jumps.
        if (PredictNextStatement() == TRUE)
        {
            // If there is a time entry and the time entry should be used, then accumulate the time entry.
            if ((n_flag == TRUE) &&
                (Estimator.Sf_Time_Entry != NULL) &&
                (Estimator.Phase_Time_Counted == FALSE))
            {
                // Update the accumulated estimated cycle seconds.
                uint16 seconds = GetSecondsFromTimeEntry(Estimator.Sf_Time_Entry);
                #if (ESTIMATOR_TRACE == ENABLED)
                    if (Estimator.Time_Index < ESTIMATOR_MAX_PHASES)
                    {
                        // Record that the predicted phase will consume time.
                        Estimator.Times[Estimator.Time_Index] = seconds;
                    }
                #endif
                Estimator.Wip_Seconds += seconds;
                Estimator.Phase_Time_Counted = TRUE;

                // Check for a cycle that is too long.
                if (Estimator.Wip_Seconds >= MAXIMUM_ESTIMATED_SECONDS)
                {
                    LOG_ADD_EXCEPTION(CANCELLED_DUE_TO_TOO_MUCH_TIME,
                                      (Estimator.Compartment << 8) + Estimator.Running_Data.Cps.Cycle);
                    Estimator.Wip_Seconds = MAXIMUM_ESTIMATED_SECONDS;
                    Estimator.Is_Estimating = FALSE;
                    break;                          // Break out of the while() loop.
                }
            }
            AdvanceToNextStatement();
        }

        // Check for the end of the main cycle.
        if (Estimator.Running_Data.Cps.Cycle == 0)
        {
            Estimator.Is_Estimating = FALSE;
            break;                                  // Break out of the while() loop.
        }
        // Load new phase data if the cycle or phase changed.
        else
        {
            if ((Estimator.Running_Data.Cps.Cycle != cycle_before) ||
                (Estimator.Running_Data.Cps.Phase != phase_before))
            {
                // Load the phase statements and get the phase time.
                Estimator.Sf_Time_Entry = LoadPhaseDataAndGetTimeEntry(&Estimator.Running_Data);

                // Don't use the time if it is the first time entry since it is already counted.
                FIRST_TIME_ENTRY_DATA_TYPE* run = &Estimator_Nv.Running[Estimator.Compartment];
                FIRST_TIME_ENTRY_DATA_TYPE* ret = &Estimator_Nv.Return[Estimator.Compartment];
                if (((Estimator.Running_Data.Cps.Phase == run->Phase) &&
                     (Estimator.Running_Data.Cps.Cycle == run->Cycle)) ||
                    ((Estimator.Running_Data.Cps.Phase == ret->Phase) &&
                     (Estimator.Running_Data.Cps.Cycle == ret->Cycle)))
                {
                    Estimator.Phase_Time_Counted = TRUE;
                }
                else
                {
                    Estimator.Phase_Time_Counted = FALSE;
                }
            }
        }

        // Check if too many statements have been evaluated in the current cycle.
        Estimator.Statements_In_Cycle++;
        if (Estimator.Statements_In_Cycle >= MAX_STATEMENTS_PER_CYCLE)
        {
            // There is likely an infinite loop in the predicted cycle.
            LOG_ADD_EXCEPTION(CANCELLED_DUE_TO_PROBABLE_INFINITE_LOOP,
                              (Estimator.Compartment << 8) + Estimator.Running_Data.Cps.Cycle);
            Estimator.Wip_Seconds = MAXIMUM_ESTIMATED_SECONDS;
            Estimator.Is_Estimating = FALSE;
            break;                                  // Break out of the while() loop.
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the Lo Statement associated with the current predicted Hi Statement.
 * @param op_code = The OpCode in the current predicted Hi Statement.
 * @return The Lo Statement associated with the current predicted Hi Statement.
 */
static uint16 GetLoStatement(PROCESS_OPCODE_ENUM_TYPE op_code)
{
    uint16 lo_statement;
    uint16 lo_statement_index = Estimator.Running_Data.Lo_Statement_Index;
    if (BIT_TEST(Estimator.Lo_Statement_Length_Map, op_code) == 0)
    {
        lo_statement = Estimator.Phase_Data.Lo_Statements[lo_statement_index];
    }
    else
    {
        lo_statement = COMBINE_BYTES(Estimator.Phase_Data.Lo_Statements[lo_statement_index + 1],
                                     Estimator.Phase_Data.Lo_Statements[lo_statement_index]);
    }
    return lo_statement;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Calculate the number of seconds for the specified time entry.
 * @param te = Pointer to the time entry data.
 * @return The number of seconds calculated based on the time entry.
 */
static uint16 GetSecondsFromTimeEntry(SF_TIME_ENTRY_TYPE* te)
{
    uint16 seconds = 0;

    if (te != NULL)
    {
        if (te->Format == 0)
        {
            // Time entry uses the simple format.
            seconds = te->Type.Simple.Value;
            seconds *= MULTIPLIER[te->Type.Simple.Scale];
        }
        else
        {
            // Time entry uses the full format.
            seconds = ENDIANU16_STON(te->Type.Full.Seconds);

            // Determine pointer for modifier data
            if (Estimator.Is_Modifier_Displacement_Word == TRUE)
            {
                uint16 modifier = te->Type.Full.Modifier_Word;
                seconds = (uint16)Modifier__ModifyValue(seconds, SF_PTR_ACU_MODIFIERS_EXTENDED, modifier);
            }
            else
            {
                uint8 modifier = te->Type.Full.Modifier_Byte;
                if (modifier != UCHAR_MAX)
                {
                    seconds = (uint16)Modifier__ModifyValue(seconds, SF_PTR_ACU_MODIFIERS, modifier);
                }
            }
        }
    }

    return seconds;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Setup the estimation data for the running or return cycle.
 * @param wip = Pointer to the estimation data structure to fill.
 * @param always_update = TRUE = Always overwrite the old data.
 *                      FALSE = Only overwrite the old data if the new data contains a time estimate.
 */
static void LoadEstimationData(RUN_TIME_DATA_TYPE* in, FIRST_TIME_ENTRY_DATA_TYPE* out, BOOL_TYPE always_update)
{
    // Assume that there are no time estimation entries for the current cycle/phase.
    SF_TIME_ENTRY_TYPE* te = LoadPhaseDataAndGetTimeEntry(in);

    // Determine whether to load the new data.
    BOOL_TYPE load = always_update;
    if ((load == FALSE) &&
        (te != NULL) &&
        ((out->Cycle != in->Cps.Cycle) ||
         (out->Phase != in->Cps.Phase)))
    {
        load = TRUE;
    }
    if (load == TRUE)
    {
        out->Cycle = in->Cps.Cycle;
        out->Phase = in->Cps.Phase;
        out->First_Time_Entry = te;
        out->Calls_Since_Time_Entry = in->Elapsed * ESTIMATOR_CALLS_PER_SECOND;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Load the phase data associated with the run_data and get the time entry for the current cycle/phase.
 * @param run_data = Pointer to the run data from the interpreter.
 * @return The time entry for the current cycle/phase.
 */
static SF_TIME_ENTRY_TYPE* LoadPhaseDataAndGetTimeEntry(RUN_TIME_DATA_TYPE* run_data)
{
    // Assume that there are no time estimation entries for the current phase.
    SF_TIME_ENTRY_TYPE* te = NULL;

    // Initialize the Phase_Data structure.
    uint8 sf_index = run_data->Setting_File_Index;
    if (Selector__LoadPhaseData(sf_index,
                                Estimator.Compartment,
                                run_data->Cps.Cycle,
                                run_data->Cps.Phase,
                                &Estimator.Phase_Data) == FAIL)
    {
        LOG_ADD_EXCEPTION(ERROR_READING_SELECTOR_DATA, COMBINE_BYTES(run_data->Cps.Cycle, run_data->Cps.Phase));
    }
    else if (run_data->Cps.Statement >= Estimator.Phase_Data.Number_Of_Phase_Statements)
    {
        LOG_ADD_EXCEPTION(FIRST_STATEMENT_OUT_OF_RANGE, COMBINE_BYTES(run_data->Cps.Cycle, run_data->Cps.Statement));
    }
    else if (Estimator.Phase_Data.Time_Estimation_Index == UCHAR_MAX)
    {
        // This phase has no time estimation data.
    }
    else
    {
        SETTINGFILE_LOADER_TYPE loader;
        if (SettingFile__MSettingBasicLoader(sf_index,
                                             SF_PTR_ACU_TIME_ESTIMATION,
                                             Estimator.Phase_Data.Time_Estimation_Index,
                                             &loader) == PASS)
        {
            te = (SF_TIME_ENTRY_TYPE*)(void*)loader.Data;

            // Check length of data structure to determine if the modifier displacement is a byte or a word
            if (loader.Length == 6)
            {
                Estimator.Is_Modifier_Displacement_Word = TRUE;
            }
        }
    }
    return te;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Predict the next statement and whether or not it will jump.
 * Either a JumpIf process or a SetState process can jump.
 * This function does not update the phase statements or the phase time entry.
 * @return TRUE if the predicted next statement is the next sequential cycle statement.
 *         FALSE if the next statement is predicted to jump.
 */
static BOOL_TYPE PredictNextStatement(void)
{
    // Setup the return value.
    BOOL_TYPE predict_sequential_execution = TRUE;

    // Check the current statement in the step.
    if (Estimator.Running_Data.Cps.Statement < Estimator.Phase_Data.Number_Of_Phase_Statements)
    {
        // Get the next HI statement.
        HI_STATEMENT_TYPE hi_statement = Estimator.Phase_Data.Hi_Statements[Estimator.Running_Data.Cps.Statement];

        // Determine if the statement is a JumpIf process.
        if (hi_statement.Op_Code == PROCESS_OPCODE_JUMP_IF)
        {
            // Predict whether the JumpIf process will execute a jump.
            uint16 displacement = GetLoStatement(PROCESS_OPCODE_JUMP_IF);
            if (JumpIf__Estimate(Estimator.Running_Data.Setting_File_Index,
                                 displacement,
                                 &Estimator.Running_Data.Cps) == TRUE)
            {
                // Jumped to a non-sequential step.
                CalculateLoStatementIndex();
                predict_sequential_execution = FALSE;
            }
        }
        // Determine if the statement is a SetState process.
        else if (hi_statement.Op_Code == PROCESS_OPCODE_SET_STATE)
        {
            // Predict whether the SetState process will execute an "end sub-cycle" or "end cycle" behavior.
            if (SetStateEstimate() == TRUE)
            {
                // Jumped to a non-sequential step.
                predict_sequential_execution = FALSE;
            }
        }
    }
    return predict_sequential_execution;
}


#if (ESTIMATOR_TRACE == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Publish debug data related to the time estimation that just completed.
 * Published as two OpCodes under API004.
 *  OpCode 11 Payload:
 *      1 byte with the phase number of the first estimated phase time in the payload.
 *      2 big-endian bytes for each estimated phase time.
 *          Phase times are the number of seconds estimated for a phase.
 *          A value of 0xCCCC indicates that no time was estimated.
 *          A value of 0x0000 indicates that a 0 seconds were estimated.
 *  OpCode 12 Payload:
 *      2 big-endian bytes with the statement index of the first statement entry.
 *      2 bytes for each statement entry.
 *          Statement entries consist of byte pairs that represent the predicted cycle execution.
 *          Byte pair that specifies a cycle:
 *              First byte will be 0xCC
 *              Second byte will be the cycle number
 *          Byte pair that specifies a change in phase and/or statement
 *              First byte will be the phase number
 *              Second byte will be the statement number
 */
static void PublishDebugData(void)
{
    uint8 phase = 0;
    while (phase <= Estimator.Time_Index)
    {
        uint8 entries_in_payload = ESTIMATOR_DEBUG_MAX_ENTRIES_PER_PAYLOAD;
        if (entries_in_payload > Estimator.Time_Index - phase + 1)
        {
            entries_in_payload = Estimator.Time_Index - phase + 1;
        }
        REVEAL_COMMAND_TYPE* fbk = Reveal__NewFeedback(ESTIMATOR_DEBUG_API, ESTIMATOR_DEBUG_OPCODE_PHASE_TIMES, WIN_BROADCAST,
                                                       sizeof(uint8) + sizeof(uint16) * entries_in_payload);
        Reveal__AddDataU8(fbk, phase);
        for (uint8 i = 0; i < entries_in_payload; i++)
        {
            Reveal__AddDataU16(fbk, Estimator.Times[phase + i]);
        }
        Reveal__QueueCommand(fbk);
        phase += entries_in_payload;
    }

    uint16 index = 0;
    while (index < Estimator.Statement_Index)
    {
        uint16 entries_in_payload = Estimator.Statement_Index - index;
        if (entries_in_payload > (REVEAL_PAYLOAD_SIZE & 0xFFFE) - sizeof(uint16))
        {
            entries_in_payload = (REVEAL_PAYLOAD_SIZE & 0xFFFE) - sizeof(uint16);
        }
        REVEAL_COMMAND_TYPE* fbk = Reveal__NewFeedback(ESTIMATOR_DEBUG_API, ESTIMATOR_DEBUG_OPCODE_STATEMENTS, WIN_BROADCAST,
                                                       sizeof(uint16) + entries_in_payload);
        Reveal__AddDataU16(fbk, index);
        Reveal__AddDataU8Buffer(fbk, &Estimator.Statements[index], entries_in_payload);
        Reveal__QueueCommand(fbk);
        index += entries_in_payload;
    }
}
#endif


//---------------------------------------------------------------------------------------------------------------------
/**
 * Update the estimated remaining time for the specified compartment.
 * @param compartment = The compartment whose estimated remaining time should be updated.
 */
static void SetEstimatedRemainingTime(uint8 compartment)
{
    sint32 seconds;
    FIRST_TIME_ENTRY_DATA_TYPE* run = &Estimator_Nv.Running[compartment];
    switch (Estimator.State[compartment])
    {
        case ESTIMATOR_NO_CYCLE_SELECTED:
            seconds = NO_ESTIMATION;
            break;

        case ESTIMATOR_CYCLE_SELECTED:
        case ESTIMATOR_CYCLE_STARTED:
            seconds = GetSecondsFromTimeEntry(run->First_Time_Entry) -
                      (run->Calls_Since_Time_Entry / ESTIMATOR_CALLS_PER_SECOND);
            if (seconds < 0)
            {
                seconds = 0;
            }
            seconds += Estimator.Seconds_After_Time_Entry[compartment];
            if (seconds > MAXIMUM_ESTIMATED_SECONDS)
            {
                seconds = MAXIMUM_ESTIMATED_SECONDS;
            }
            break;

        case ESTIMATOR_INITIALIZING:
        default:
            // Do not change the current value while initializing.
            seconds = Estimated_Remaining_Time[compartment];
            break;
    }
    #if (ESTIMATOR_TRACE == ENABLED)
        if (labs(seconds - Estimated_Remaining_Time[compartment]) > 5)
        {
            PublishDebugData();
        }
    #endif
    Estimated_Remaining_Time[compartment] = seconds;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Predict the cycle behavior in response to the SetState process.
 * @return TRUE if the predicted next step includes the ending of a sub-cycle or cycle.
 *         FALSE if the predicted next step is the next sequential cycle step.
 */
static BOOL_TYPE SetStateEstimate(void)
{
    BOOL_TYPE predict_change_in_cycle = FALSE;
    uint8 statement = Estimator.Running_Data.Cps.Statement;
    SET_STATE_F1_F0_FLAG_TYPE f1_f0 = (SET_STATE_F1_F0_FLAG_TYPE)Estimator.Phase_Data.Hi_Statements[statement].F1_F0_Flags;

    uint16 displacement = GetLoStatement(PROCESS_OPCODE_SET_STATE);
    SETTINGFILE_LOADER_TYPE loader;
    if ((f1_f0 != SET_STATE_NOP) &&
        (displacement != UCHAR_MAX) &&
        (SettingFile__MSettingBasicLoader(Estimator.Running_Data.Setting_File_Index,
                                          SF_PTR_ACU_PROCESS_SET_STATE,
                                          displacement,
                                          &loader) == PASS))
    {
        if (f1_f0 == SET_STATE_END_CYCLE)
        {
            switch ((SET_STATE_END_CYCLE_TYPE)loader.Data[0])
            {
                case END_CYCLE_TO_END:
                case END_CYCLE_TO_PROGRAMMING:
                    predict_change_in_cycle = TRUE;
                    Estimator.Running_Data.Cps.Cycle = 0;
                    break;
                case END_CYCLE_TO_PAUSE:
                default:
                    break;
            }
        }
        else if (f1_f0 == SET_STATE_END_SUB_CYCLE)
        {
            predict_change_in_cycle = TRUE;
            FIRST_TIME_ENTRY_DATA_TYPE* ret = &Estimator_Nv.Return[Estimator.Compartment];
            Estimator.Running_Data = Estimator.Return_Data;
            Estimator.Return_Data.Cps.Cycle = 0;
            sint16 repeat_seconds = 0;
            switch ((SET_STATE_END_SUB_CYCLE_TYPE)loader.Data[0])
            {
                case END_SUB_CYCLE_RESTART_STEP:
                    // Add the time elapsed in the current main cycle step.
                    repeat_seconds = Estimator.Running_Data.Elapsed;
                    break;
                case END_SUB_CYCLE_RESTART_PHASE:
                    // Add the time elapsed in the current main cycle phase.

                    repeat_seconds = (ret->Calls_Since_Time_Entry / ESTIMATOR_CALLS_PER_SECOND) -
                                     Estimator_Nv.Seconds_Elapsed_Before_Phase[Estimator.Compartment];
                    Estimator.Running_Data.Cps.Statement = 0;
                    Estimator.Running_Data.Lo_Statement_Index = 0;
                    break;
                case END_SUB_CYCLE_SKIP_PHASE:
                    AdvanceToNextPhase();
                    // Skip the time remaining in the current main cycle phase.
                    if (ret->First_Time_Entry != NULL)
                    {
                        repeat_seconds = (ret->Calls_Since_Time_Entry / ESTIMATOR_CALLS_PER_SECOND) -
                                         GetSecondsFromTimeEntry(ret->First_Time_Entry);
                    }
                    break;
                case END_SUB_CYCLE_RESUME_STEP:
                default:
                    break;
            }
            Estimator_Nv.Repeat_Seconds[Estimator.Compartment] = repeat_seconds;
            AdjustTimeForResumingMainCycle();
        }
    }
    return predict_change_in_cycle;
}
#endif      // #if (TIME_ESTIMATION_FEATURE == ENABLED)


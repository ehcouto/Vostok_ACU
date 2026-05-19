 /**
 *  @file
 *
 *  @brief      Analyze and execute processes line by line.
 *              Analyze and execute cycle/phase requested by the supervisor.
 *              Iterates through statements (processes) for the cycle phase steps in the settings file.
 *
 *  @copyright  Copyright 2015-2019 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "Interpreter.h"
#include "Interpreter_prv.h"

// -- Other Modules --
#include "ActivateTask.h"
#include "Fault.h"
#include "JumpIf.h"
#include "Log.h"
#include "LoadsControl.h"
#include "Maintain.h"
#include "Micro.h"
#include "NVData.h"
#include "Process.h"
#include "SetVariable.h"
#include "SettingFile.h"
#include "SubCycle.h"
#include <string.h>

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
// Default value for allowing sub-cycle during states other than Running is DISABLED (Define in Interpreter_prv.h if required).
#ifndef SUB_CYCLE_ALLOWED_DURING_ALL_STATES
    #define SUB_CYCLE_ALLOWED_DURING_ALL_STATES DISABLED
#endif

// Default value for allowing sub-cycle during test is DISABLED (Define in Interpreter_prv.h if required).
#ifndef SUB_CYCLE_ALLOWED_DURING_TEST
    #define SUB_CYCLE_ALLOWED_DURING_TEST DISABLED
#endif

//! Structure type to hold interpreter running data
PACKED typedef struct INTERPRETER_DATA_STRUCT
{
    uint8               Setting_File_Index;         //!< '0' primary SF. Non 0 new cycle data added in customers home.
    CYCLE_PHASE_STATEMENT_TYPE  Cps;                //!< Current cycle, phase and statement.
    CYCLE_STATE_TYPE    Cycle_State;                //!< The state cycle that the interpreter shall run.
    uint8               Step_Time_Counter;          //!< 100 millisecond counter used to count seconds for elapsed step time.
    BOOL_TYPE           Step_Initializing   : 1;    //!< TRUE = This is the first call to this process on this step.
    BOOL_TYPE           Resume_From_Pause   : 1;    //!< TRUE = Step is resuming after a cycle pause or sub-cycle.
    BOOL_TYPE           Resume_From_Reset   : 1;    //!< TRUE = Step is resuming after a power reset.
    BOOL_TYPE           SF_Error            : 1;    //!< TRUE = Error loading settings file data (Interpreter will not run).
    uint16              Lo_Statement_Index;         //!< Index (displacement) to process data table.
} INTERPRETER_DATA_TYPE;

//! Enumerated list of values for Set State Process F1_F0_Flags.
typedef enum SETSTATE_FLAG_ENUM
{
    SETSTATE_FLAG_NOP = 0,                          //!< F1=0, F0=0
    SETSTATE_FLAG_END_CYCLE = 1,                    //!< F1=0, F0=1
    SETSTATE_FLAG_END_SUB_CYCLE = 2                 //!< F1=1, F0=0
} SETSTATE_FLAG_TYPE;

//! Function pointer table for process initialization functions.
//! These functions initialize module (not instance) data.
//!
//! NOTE: The primary method of initializing instance data is using
//! the Step_Initializing, Resume_From_Pause, or Resume_From_Reset flags
//! in the PROCESS_CONTROL_TYPE passed to the Process_Function.
const PROCESS_INITIALIZE_FUNCTION_TYPE Process_Initialize_Function[] =
{
    // Cross category Op Codes 0 - 5. Common and required for all projects.
    NULL_PROCESS_INITIALZE,                  //!< OpCode:  0 SetState
    NULL_PROCESS_INITIALZE,                  //!< OpCode:  1 SetVariable
    Maintain__Initialize,                    //!< OpCode:  2 Maintain
    LoadsControl__Initialize,                //!< OpCode:  3 LoadsControl
    NULL_PROCESS_INITIALZE,                  //!< OpCode:  4 JumpIf
    ActivateTask__Initialize,                //!< OpCode:  5 ActivateTask
    PROJECT_SPECIFIC_PROCESS_INITIALIZERS    //!< OpCodes: 6 - 15
};

// Process_Initialize_Function array shall have all 16 possible process initialize functions.
CT_ASSERT((sizeof(Process_Initialize_Function) / sizeof(Process_Initialize_Function[0]))  == 16);

//! Function pointer table for application processes.
const PROCESS_FUNCTION_TYPE Process_Function[] =
{
    // Cross category Op Codes 0 - 5. Common and required for all projects.
    NULL_PROCESS,                                   //!< OpCode:  0 SetState (handled by interpreter)
    SetVariable__Process,                           //!< OpCode:  1 SetVariable
    Maintain__Process,                              //!< OpCode:  2 Implements timing in the cycle step.
    LoadsControl__Process,                          //!< OpCode:  3 Implements common simple load management.
    JumpIf__Process,                                //!< OpCode:  4 JumpIf
    ActivateTask__Process,                          //!< OpCode:  5 ActivateTask
    PROJECT_SPECIFIC_PROCESSES                      //!< OpCodes: 6 - 15
};

//! Set state process cycle completion code type definition.
typedef enum CYCLE_COMPLETION_CODE_ENUM
{
    CYCLE_COMPLETION_CODE_END       = 0,
    CYCLE_COMPLETION_CODE_PAUSE     = 1,
    CYCLE_COMPLETION_CODE_END_RESET = 2,
    CYCLE_COMPLETION_CODE_MAX       = 3
} CYCLE_COMPLETION_CODE_TYPE;

//! Set state process sub-cycle completion code type definition.
typedef enum SUB_CYCLE_COMPLETION_CODE_ENUM
{
    SUB_CYCLE_COMPLETION_CODE_RESUME          = 0,
    SUB_CYCLE_COMPLETION_CODE_RESTART_STEP    = 1,
    SUB_CYCLE_COMPLETION_CODE_RESTART_PHASE   = 2,
    SUB_CYCLE_COMPLETION_CODE_SKIP_PHASE      = 3,
    SUB_CYCLE_COMPLETION_CODE_MAX             = 4
} SUB_CYCLE_COMPLETION_CODE_TYPE;

// Process_Function array shall have all 16 possible process functions.
// Ensuring that there are 16 eliminates having run time checks for the high statement opcode.
CT_ASSERT((sizeof(Process_Function) / sizeof(Process_Function[0]))  == 16);

#pragma location = "_appliance_segment"
//! Compartments Interpreter data  array. One entry per compartment (0 indicates compartment 1).
static INTERPRETER_DATA_TYPE Interpreter_Data[APPLIANCE_MAX_COMPARTMENTS];

#pragma location = "_appliance_segment"
//! Compartments Interpreter Status array. One entry per compartment (0 indicates compartment 1).
INTERPRETER_STATUS_TYPE Interpreter_Status;

//! Compartments phase data array. One entry per compartment (0 indicates compartment 1).
static PHASE_DATA_TYPE Itp_Phase_Data[APPLIANCE_MAX_COMPARTMENTS];

#pragma location = "_appliance_segment"
//! Compartments pause resume Interpreter data array . One entry per compartment (0 indicates compartment 1).
static INTERPRETER_DATA_TYPE Pause_Resume_Data[APPLIANCE_MAX_COMPARTMENTS];

#pragma location = "_appliance_segment"
//! Compartments Interpreter pause status resume data array. One entry per compartment (0 indicates compartment 1).
static INTERPRETER_STATUS_TYPE Pause_Resume_Status;

#pragma location = "_appliance_segment"
//! Compartments sub-cycle resume data array. One entry per compartment (0 indicates compartment 1).
static INTERPRETER_DATA_TYPE Sub_Cycle_Resume_Data[APPLIANCE_MAX_COMPARTMENTS];

#pragma location = "_appliance_segment"
//! Compartments sub-cycle resume status array. One entry per compartment (0 indicates compartment 1).
static INTERPRETER_STATUS_TYPE Sub_Cycle_Resume_Status;

//! Pointer to setting file process data table index size.
static uint16 *Lo_Statement_Index_Length_ptr;

#pragma location = "_appliance_segment"
//! Store the number of main cycle phases to insure sub-cycle exit does not skip to invalid phase.
static uint8 Main_Cycle_Number_Of_Phases[APPLIANCE_MAX_COMPARTMENTS];

// ------- LOG MODULE SETTINGS-------------------------
//! The name of this module for use by the Log module.
#define MODULE_NAME MODULE_INTERPRETER

//! A list of log message IDs that are generated by this module.
typedef enum MODULE_INTERPRETER_LOG_MESSAGE_ID_ENUM
{
    COMPARTMENT_OUT_OF_RANGE            = 1,            //!< Compartments start at zero.
    PROCESS_FUNCTION_NULL               = 2,            //!< All Process OpCodes shall have an associated function.
    NOP_PROCESS                         = 3,            //!< No Operation Process in phase step.
    FIRST_STATEMENT_OUT_OF_RANGE        = 4,            //!< Statement of the initial step out of range.
    NULL_POINTER                        = 5,            //!< NULL pointer argument passed to Interpreter function.
    INVALID_SET_STATE_FLAG              = 6,            //!< F1_F0 Flag has invalid value.
    INVALID_END_SUB_CYCLE               = 7,            //!< Set state end sub-cycle received while sub-cycle not running.
    INVALID_COMPLETION_CODE             = 8,            //!< Invalid completion code (set state end sub-cycle).
    SF_PTR_SUB_CYCLE_FAIL               = 9,            //!< Failed to find set state data in setting file.
    JUMP_OCCURRED                       = 10,           //!< Jump occurred (HiByte = new phase; new statement)
    STARTED_NEW_STEP                    = 11,           //!< Started a new step (Data = Step number)
    STARTED_NEW_PHASE                   = 12,           //!< Started a new phase (Data = Phase number)
    JUMP_TO_CYCLE_OCCURRED              = 13,           //!< Jump to cycle occurred (HiByte = new cycle; LoByte = from cycle)
    SUB_CYCLE_COMPLETION_INVALID_PHASE  = 14            //!< Attempt to skip to next phase in last phase of main cycle.
} MODULE_INTERPRETER_LOG_MESSAGE_ID_TYPE;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void                         CheckForSubCycle                (uint8 compartment);
static INTERPRETER_RESULT_TYPE      ExitSubCycle                    (uint8 compartment, uint8 data_field_index);
static CYCLE_COMPLETION_CODE_TYPE   GetCycleCompletionCode          (uint8 compartment, uint8 data_index);
static uint8                        GetFirstStatement               (uint8 compartment, uint8 step);
static uint16                       GetLoStatementIndex             (uint8 compartment, uint8 statement);
static uint8                        GetNumberOfSteps                (uint8 compartment);
static void                         LoadPhaseData                   (uint8 compartment);
static void                         RestoreFromSubCycleResumeData   (uint8 compartment);
static void                         SaveSubCycleResumeData          (uint8 compartment);
static void                         SetStepStatus                   (uint8 compartment);
static void                         RecoverPauseResumeData          (uint8 compartment);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Calls SubCycle__Initialize() and then initializes processes, the Interpreter module and its variables.
 */
void Interpreter__Initialize(void)
{
    INTERPRETER_CONFIG_TYPE interpreter_config;
    uint8 compartment;
    uint8 op_code;

    SubCycle__Initialize();
    Lo_Statement_Index_Length_ptr = Selector__GetLoStatmentIndexLengthPtr();

    // Call regardless of reset reason so that each process can initialize module (not instance) variables.
    // Each process shall manage reset mode and NV data recovery status.
    for (op_code = 0; op_code < ELEMENTS_IN_ARRAY(Process_Initialize_Function); op_code++)
    {
        if (Process_Initialize_Function[op_code] != NULL_PROCESS_INITIALZE)
        {
            Process_Initialize_Function[op_code]();
        }
    }

    if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
    {
        if (NVData__GetLastRestoreStatus()->Bit.Appliance_Data_Recovered != NVDATA_RESTORE_OKAY)
        {
            memset(Interpreter_Data, 0x00, sizeof(Interpreter_Data));
            memset(&Interpreter_Status, 0x00, sizeof(Interpreter_Status));
            memset(Itp_Phase_Data, 0x00, sizeof(Itp_Phase_Data));
            memset(Pause_Resume_Data, 0x00, sizeof(Pause_Resume_Data));
            memset(&Pause_Resume_Status, 0x00, sizeof(Pause_Resume_Status));
            memset(Sub_Cycle_Resume_Data, 0x00, sizeof(Sub_Cycle_Resume_Data));
            memset(&Sub_Cycle_Resume_Status, 0x00, sizeof(Sub_Cycle_Resume_Status));
            memset(&Main_Cycle_Number_Of_Phases, 0x00, sizeof(Main_Cycle_Number_Of_Phases));

            for (compartment = 0; compartment < APPLIANCE_MAX_COMPARTMENTS; compartment++)
            {
                interpreter_config.Cycle = 0;
                interpreter_config.Phase = 0;
                interpreter_config.Statement = 0;
                interpreter_config.Setting_File_Index = PRIMARY_SETTING_FILE;
                interpreter_config.Cycle_State = CYCLE_STATE_PROGRAMMING;
                Interpreter__Configure(compartment, &interpreter_config);
            }
        }
        else
        {
            for (compartment = 0; compartment < APPLIANCE_MAX_COMPARTMENTS; compartment++)
            {
                CYCLE_COLD_RESET_BEHAVIOR_TYPE ccrb = Selector__GetColdResetBehavior(PRIMARY_SETTING_FILE,
                                                                                     compartment,
                                                                                     Interpreter_Data[compartment].Cps.Cycle);
                if ( ccrb != CYCLE_COLD_RESET_BEHAVIOR_CANCEL)
                {
                    Interpreter_Data[compartment].Resume_From_Reset = TRUE;
                }
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Clear interpreter sub-cycle status for compartment.
 *
 *  @param      uint8 compartment - identifier appliance compartment. (0 based, 0 for single compartment appliances)
 */
void Interpreter__ClearSubCycleStatus(uint8 compartment)
{
    if (compartment >= APPLIANCE_MAX_COMPARTMENTS)
    {
        LOG_ADD_EXCEPTION(COMPARTMENT_OUT_OF_RANGE, compartment);
    }
    else if (Interpreter_Status.Sub_Cycle[compartment] != 0)
    {
        Interpreter_Status.Sub_Cycle[compartment] = 0;
        Interpreter_Status.Sub_Phase[compartment] = 0;
        Interpreter_Status.Sub_Cycle_Step[compartment] = 0;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Configure Interpreter for compartment at the start of a cycle.
 *
 *  @param      uint8 compartment - identifier appliance compartment. (0 based, 0 for single compartment appliances)
 *  @param      INTERPRETER_CONFIG_TYPE *config - pointer to configuration for Interpreter_Data[compartment].
 */
void Interpreter__Configure(uint8 compartment, INTERPRETER_CONFIG_TYPE *config)
{
    if (config == NULL)
    {
        LOG_ADD_EXCEPTION(NULL_POINTER, 1);
    }
    else if (compartment >= APPLIANCE_MAX_COMPARTMENTS)
    {
        LOG_ADD_EXCEPTION(COMPARTMENT_OUT_OF_RANGE, compartment);
    }
    else
    {
        Interpreter_Data[compartment].Setting_File_Index = config->Setting_File_Index;
        Interpreter_Data[compartment].Cps.Cycle = config->Cycle;
        if ((config->Cycle_State == CYCLE_STATE_PAUSE) && (Interpreter_Status.Sub_Cycle[compartment] != 0))
        {
            // Pausing while sub-cycle is running. Run pause macro for main cycle.
            Interpreter_Data[compartment].Cps.Phase = Sub_Cycle_Resume_Data[compartment].Cps.Cycle;
        }
        else
        {
            Interpreter_Data[compartment].Cps.Phase = config->Phase;
        }
        Interpreter_Data[compartment].Cps.Statement = config->Statement;
        Interpreter_Data[compartment].Cycle_State = config->Cycle_State;
        Interpreter_Data[compartment].Step_Initializing = TRUE;

        LoadPhaseData(compartment);
        Interpreter_Data[compartment].Step_Time_Counter = 0;
        Interpreter_Status.Step_Time_Elapsed[compartment] = 0;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get Interpreter_Data[compartment].Setting_File_Index.
 *
 *  @param      uint8 compartment - identifier appliance compartment. (0 based, 0 for single compartment appliances)
 *
 *  @return     uint8 setting_file_index - current compartment setting file index
 */
uint8 Interpreter__GetCurrentSFIndex(uint8 compartment)
{
    uint8 setting_file_index;

    if (compartment >= APPLIANCE_MAX_COMPARTMENTS)
    {
        LOG_ADD_EXCEPTION(COMPARTMENT_OUT_OF_RANGE, compartment);
        setting_file_index = 0;
    }
    else
    {
        setting_file_index = Interpreter_Data[compartment].Setting_File_Index;
    }

    return (setting_file_index);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Load Cps with Interpreter_Data[compartment].Cps.
 *
 *  @param      uint8 compartment - identifier appliance compartment. (0 based, 0 for single compartment appliances)
 *
 *  @return     CYCLE_PHASE_STATEMENT_TYPE cps - current cycle phase statement
 */
CYCLE_PHASE_STATEMENT_TYPE  Interpreter__GetCurrentStatement(uint8 compartment)
{
    CYCLE_PHASE_STATEMENT_TYPE cps;

    if (compartment >= APPLIANCE_MAX_COMPARTMENTS)
    {
        LOG_ADD_EXCEPTION(COMPARTMENT_OUT_OF_RANGE, compartment);
        cps.Cycle = 0;
        cps.Phase = 0;
        cps.Statement = 0;
    }
    else
    {
        cps = Interpreter_Data[compartment].Cps;
    }

    return (cps);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Return elapsed time current step for compartment.
 *
 *  @param      uint8 compartment - Identifier appliance compartment (0 based, 0 for single compartment appliances).
 *
 *  @return      uint16 Step_Elapsed_Time - Current step elapsed time for compartment.
 */
uint16 Interpreter__GetCurrentStepTimeElapsed(uint8 compartment)
{
    if (compartment >= APPLIANCE_MAX_COMPARTMENTS)
    {
        LOG_ADD_EXCEPTION(COMPARTMENT_OUT_OF_RANGE, compartment);
        return 0;
    }

    return  Interpreter_Status.Step_Time_Elapsed[compartment];
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get Interpreter run time data for the running or selected cycle.
 *              Based on a specific compartment number, need to know:
 *                  1) Which setting files are being used?
 *                  2) Has the cycle started running or has it only been selected?
 *                  3) What is the cycle, phase, and first statement that is running?
 *                  4) What is the return cycle, phase, and first statement (if any)?
 *                  5) What is the lo statement index for the current step.
 *
 *  @param      uint8 compartment - Identifier appliance compartment (0 based, 0 for single compartment appliances).
 *  @param      RUN_TIME_DATA_TYPE *running_data - pointer to structure to be populated.
 *  @param      RUN_TIME_DATA_TYPE *return_data - pointer to structure to be populated.
 *
 *  @return     BOOL_TYPE cycle_running
 */
BOOL_TYPE Interpreter__GetRunTimeData(uint8 compartment, RUN_TIME_DATA_TYPE *running_data, RUN_TIME_DATA_TYPE *return_data)
{
    BOOL_TYPE cycle_running;

    cycle_running = FALSE;

    if ((running_data == NULL) || (return_data == NULL))
    {
        LOG_ADD_EXCEPTION(NULL_POINTER, 3);
        return cycle_running;
    }

    if (compartment >= APPLIANCE_MAX_COMPARTMENTS)
    {
        LOG_ADD_EXCEPTION(COMPARTMENT_OUT_OF_RANGE, compartment);
        return cycle_running;
    }

    switch (Interpreter_Data[compartment].Cycle_State)
    {
        case CYCLE_STATE_PROGRAMMING:
        case CYCLE_STATE_DELAY:
            running_data->Setting_File_Index = Interpreter_Data[compartment].Setting_File_Index;
            running_data->Cps.Cycle = Interpreter_Data[compartment].Cps.Phase;
            running_data->Cps.Phase = 0;
            running_data->Cps.Statement = 0;
            running_data->Elapsed = 0;
            running_data->Lo_Statement_Index = 0;
            break;

        case CYCLE_STATE_RUN:
            running_data->Setting_File_Index = Interpreter_Data[compartment].Setting_File_Index;
            running_data->Cps = Interpreter_Data[compartment].Cps;
            running_data->Elapsed = Interpreter_Status.Step_Time_Elapsed[compartment];
            running_data->Lo_Statement_Index = Interpreter_Data[compartment].Lo_Statement_Index;
            cycle_running = TRUE;
            break;

        case CYCLE_STATE_PAUSE:
            running_data->Setting_File_Index = Pause_Resume_Data[compartment].Setting_File_Index;
            running_data->Cps = Pause_Resume_Data[compartment].Cps;
            running_data->Elapsed = Pause_Resume_Status.Step_Time_Elapsed[compartment];
            running_data->Lo_Statement_Index = Pause_Resume_Data[compartment].Lo_Statement_Index;
            cycle_running = TRUE;
            break;

        case CYCLE_STATE_END:
        case CYCLE_STATE_FAULT:
        default:
            memset(running_data, 0x00, sizeof(RUN_TIME_DATA_TYPE));
            break;
    }

    if ((cycle_running == TRUE) && (Interpreter_Status.Sub_Cycle[compartment] != 0))
    {
        return_data->Setting_File_Index = Sub_Cycle_Resume_Data[compartment].Setting_File_Index;
        return_data->Cps = Sub_Cycle_Resume_Data[compartment].Cps;
        return_data->Elapsed = Sub_Cycle_Resume_Status.Step_Time_Elapsed[compartment];
        return_data->Lo_Statement_Index = Sub_Cycle_Resume_Data[compartment].Lo_Statement_Index;
    }
    else
    {
        memset(return_data, 0x00, sizeof(RUN_TIME_DATA_TYPE));
    }

    return cycle_running;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get Interpreter status.
 *
 *  @return     INTERPRETER_STATUS_TYPE - Current interpreter compartment main cycle/phase and sub cycle/phase.
 */
INTERPRETER_STATUS_TYPE Interpreter__GetStatus(void)
{
    return (Interpreter_Status);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Check if sub-cycle is running in compartment .
 *
 *  @param      uint8 compartment - appliance compartment identifier. (0 based, 0 for single compartment appliances)
 *
 *  @return     BOOL_TYPE sub_cycle_running - TRUE: Sub-cycle is currently running in compartment.
 *                                            FALSE: Sub-cycle is not currently running in compartment.
 */
BOOL_TYPE Interpreter__IsSubCycleRunning(uint8 compartment)
{
    BOOL_TYPE sub_cycle_is_running = FALSE;

    if (Interpreter_Status.Sub_Cycle[compartment] != 0)
    {
        sub_cycle_is_running = TRUE;
    }

    return (sub_cycle_is_running);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Restore Interpreter_Data[compartment] from Pause_Resume_Data[compartment].
 *              Restore Interpreter_Status[compartment] from Pause_Resume_Status[compartment].
 *              Clear Pause_Resume_Data and Pause_Resume_Status.
 *
 *  @param      uint8 compartment - identifier appliance compartment. (0 based, 0 for single compartment appliances)
 */
void Interpreter__RestoreFromPauseResumeData(uint8 compartment)
{
    if (compartment >= APPLIANCE_MAX_COMPARTMENTS)
    {
        LOG_ADD_EXCEPTION(COMPARTMENT_OUT_OF_RANGE, compartment);
    }
    else
    {
        // Restore data/status
        Interpreter_Data[compartment] = Pause_Resume_Data[compartment];

        Interpreter_Status.Main_Cycle[compartment] = Pause_Resume_Status.Main_Cycle[compartment];
        Interpreter_Status.Main_Phase[compartment] = Pause_Resume_Status.Main_Phase[compartment];
        Interpreter_Status.Main_Step[compartment] = Pause_Resume_Status.Main_Step[compartment];
        Interpreter_Status.Sub_Cycle[compartment] = Pause_Resume_Status.Sub_Cycle[compartment];
        Interpreter_Status.Sub_Phase[compartment] = Pause_Resume_Status.Sub_Phase[compartment];
        Interpreter_Status.Sub_Cycle_Step[compartment] = Pause_Resume_Status.Sub_Cycle_Step[compartment];
        Interpreter_Status.Step_Time_Elapsed[compartment] = Pause_Resume_Status.Step_Time_Elapsed[compartment];

        // Clear resume data/status.
        memset(&Pause_Resume_Data[compartment], 0x00, sizeof(Pause_Resume_Data[0]));

        Pause_Resume_Status.Main_Cycle[compartment]= 0;
        Pause_Resume_Status.Main_Phase[compartment]= 0;
        Pause_Resume_Status.Main_Step[compartment]= 0;
        Pause_Resume_Status.Sub_Cycle[compartment]= 0;
        Pause_Resume_Status.Sub_Phase[compartment]= 0;
        Pause_Resume_Status.Sub_Cycle_Step[compartment]= 0;
        Pause_Resume_Status.Step_Time_Elapsed[compartment]= 0;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Resume current running cycle.
 *
 *  @param      uint8 compartment - identifier appliance compartment. (0 based, 0 for single compartment appliances)
 */
void Interpreter__Resume(uint8 compartment)
{
    if (compartment >= APPLIANCE_MAX_COMPARTMENTS)
    {
        LOG_ADD_EXCEPTION(COMPARTMENT_OUT_OF_RANGE, compartment);
    }
    else
    {
        Interpreter_Data[compartment].Resume_From_Pause = TRUE;
        LoadPhaseData(compartment);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Run the currently configured cycle.
 *
 *  @param      uint8 compartment - identifier for appliance compartment. (0 based, 0 for single compartment appliances)
 *
 *  @return     INTERPRETER_RESULT_TYPE - Result of current run of Interpreter.
 */
INTERPRETER_RESULT_TYPE Interpreter__Run(uint8 compartment)
{
    PROCESS_RESULT_TYPE         process_result;
    PROCESS_CONTROL_TYPE        process_control;
    uint8                       statement_index;
    uint16                      lo_statement_index;
    PROCESS_OPCODE_ENUM_TYPE    op_code;
    BOOL_TYPE                   t_flag;
    BOOL_TYPE                   new_step;
    uint8                       process_instance_count[16];

    if (compartment >= APPLIANCE_MAX_COMPARTMENTS)
    {
        LOG_ADD_EXCEPTION(COMPARTMENT_OUT_OF_RANGE, compartment);
        return INTERPRETER_RESULT_INVALID_COMPARTMENT;
    }

    // Re-enforce phase data in case of reset (Phase Data is not saved in Non-Volatile memory).
    LoadPhaseData(compartment);

    if (Interpreter_Data[compartment].SF_Error)
    {
        return INTERPRETER_RESULT_INVALID_COMPARTMENT;
    }

    CheckForSubCycle(compartment);

    // Use local variables to index through step so that public index is not affected while looping.
    statement_index = Interpreter_Data[compartment].Cps.Statement;
    lo_statement_index = Interpreter_Data[compartment].Lo_Statement_Index;

    t_flag = Itp_Phase_Data[compartment].Hi_Statements[statement_index].T_Flag;
    new_step = TRUE;
    memset(process_instance_count, 0x00, sizeof(process_instance_count));

    if (Interpreter_Data[compartment].Step_Initializing == TRUE)
    {
        ITP_EVENT_STEP_STARTING(compartment);
    }

    process_control.Setting_File_Index = Interpreter_Data[compartment].Setting_File_Index;

    while ((statement_index < Itp_Phase_Data[compartment].Number_Of_Phase_Statements) &&
           (Itp_Phase_Data[compartment].Hi_Statements[statement_index].T_Flag == t_flag))
    {
        op_code = Itp_Phase_Data[compartment].Hi_Statements[statement_index].Op_Code;
        process_control.F1_F0_Flags = Itp_Phase_Data[compartment].Hi_Statements[statement_index].F1_F0_Flags;
        if (BIT_TEST(*Lo_Statement_Index_Length_ptr, op_code) == 0)
        {
            process_control.Data_Field_Index = Itp_Phase_Data[compartment].Lo_Statements[lo_statement_index];
        }
        else
        {
            process_control.Data_Field_Index = COMBINE_BYTES(Itp_Phase_Data[compartment].Lo_Statements[lo_statement_index+1],
                                                             Itp_Phase_Data[compartment].Lo_Statements[lo_statement_index]);
        }

        if (Process_Function[op_code] != NULL_PROCESS)
        {
            process_control.Compartment = compartment;
            process_control.Process_Instance = process_instance_count[op_code]++;
            process_control.Step_Initializing = Interpreter_Data[compartment].Step_Initializing;
            process_control.Resume_From_Pause = Interpreter_Data[compartment].Resume_From_Pause;
            process_control.Resume_From_Reset = Interpreter_Data[compartment].Resume_From_Reset;
            process_control.Sub_Cycle_Running = (Interpreter_Status.Sub_Cycle[compartment] != 0) ? TRUE : FALSE;

            process_result = Process_Function[op_code](&process_control);

            if ((op_code == PROCESS_OPCODE_JUMP_IF) && (process_result == PROCESS_COMPLETE))
            {
                // Use local copy of Interpreter Cps for JumpIf__GetTarget().
                // That way we don't affect Interpreter_Data in case JumpIf__GetTarget() returns FALSE.
                // Pass statement_index as argument so we jump from the current statement as defined in the Setting File.
                // Note: Interpreter_Data[compartment].Cps.Statement is set to the first statement in the step (not the current statement).
                CYCLE_PHASE_STATEMENT_TYPE cps = {Interpreter_Data[compartment].Cps.Cycle,
                                                  Interpreter_Data[compartment].Cps.Phase,
                                                  statement_index};
                if (JumpIf__GetTarget(&cps) == TRUE)
                {
                    if (cps.Cycle != Interpreter_Data[compartment].Cps.Cycle)
                    {
                        LOG_ADD_EVENT(JUMP_TO_CYCLE_OCCURRED, COMBINE_BYTES(cps.Cycle, Interpreter_Data[compartment].Cps.Cycle));
                    }
                    Interpreter_Data[compartment].Cps = cps;
                    LoadPhaseData(compartment);

                    LOG_ADD_EVENT(JUMP_OCCURRED, COMBINE_BYTES(Interpreter_Data[compartment].Cps.Phase,
                                                               Interpreter_Data[compartment].Cps.Statement));

                    Interpreter_Data[compartment].Step_Time_Counter = 0;
                    Interpreter_Status.Step_Time_Elapsed[compartment] = 0;
                    Interpreter_Data[compartment].Step_Initializing = TRUE;
                    Interpreter_Data[compartment].Resume_From_Pause = FALSE;
                    Interpreter_Data[compartment].Resume_From_Reset = FALSE;

                    return INTERPRETER_RESULT_END_STEP;
                }
            }
        }
        else
        {
            process_result = PROCESS_COMPLETE;
            if (op_code == PROCESS_OPCODE_SET_STATE)
            {
                if ((SETSTATE_FLAG_TYPE)(process_control.F1_F0_Flags) == SETSTATE_FLAG_END_CYCLE)
                {
                    CYCLE_COMPLETION_CODE_TYPE completion_code = GetCycleCompletionCode(compartment, process_control.Data_Field_Index);
                    if (completion_code == CYCLE_COMPLETION_CODE_END)
                    {
                        return INTERPRETER_RESULT_END_CYCLE;
                    }
                    else if(completion_code == CYCLE_COMPLETION_CODE_END_RESET)
                    {
                        return INTERPRETER_RESULT_END_CYCLE_RESET;
                    }
                    else if (completion_code == CYCLE_COMPLETION_CODE_PAUSE)
                    {
                        if (Interpreter_Data[compartment].Resume_From_Pause == FALSE)
                        {
                            return (INTERPRETER_RESULT_PAUSE);
                        }
                    }
                }
                else if ((SETSTATE_FLAG_TYPE)(process_control.F1_F0_Flags) == SETSTATE_FLAG_END_SUB_CYCLE)
                {
                    return (ExitSubCycle(compartment, process_control.Data_Field_Index));
                }
                else if ((SETSTATE_FLAG_TYPE)(process_control.F1_F0_Flags) != SETSTATE_FLAG_NOP)
                {
                    LOG_ADD_EXCEPTION(INVALID_SET_STATE_FLAG, process_control.F1_F0_Flags);
                }
            }
            else
            {
                LOG_ADD_EXCEPTION(PROCESS_FUNCTION_NULL, compartment);
            }
        }

        if ((process_result == PROCESS_BUSY) && (Itp_Phase_Data[compartment].Hi_Statements[statement_index].N_Flag))
        {
            new_step = FALSE;
        }

        statement_index++;
        lo_statement_index += 1 + BIT_TEST(*Lo_Statement_Index_Length_ptr, op_code);
    }

    Interpreter_Data[compartment].Resume_From_Pause = FALSE;
    Interpreter_Data[compartment].Resume_From_Reset = FALSE;

    if (new_step == FALSE)
    {
        Interpreter_Data[compartment].Step_Time_Counter++;
        if (Interpreter_Data[compartment].Step_Time_Counter >= 10)
        {
            Interpreter_Data[compartment].Step_Time_Counter = 0;
            if (Interpreter_Status.Step_Time_Elapsed[compartment] < 0xFFFF)
            {
                Interpreter_Status.Step_Time_Elapsed[compartment]++;
            }
        }

        Interpreter_Data[compartment].Step_Initializing = FALSE;
        return INTERPRETER_RESULT_SAME_STEP;
    }

    Interpreter_Data[compartment].Step_Time_Counter = 0;
    Interpreter_Status.Step_Time_Elapsed[compartment] = 0;
    Interpreter_Data[compartment].Step_Initializing = TRUE;

    if (statement_index < Itp_Phase_Data[compartment].Number_Of_Phase_Statements)
    {
        LOG_ADD_EVENT(STARTED_NEW_STEP, Interpreter_Status.Main_Step[compartment] + 1);
        Interpreter_Data[compartment].Cps.Statement = statement_index;
        return INTERPRETER_RESULT_END_STEP;
    }


    if ((Interpreter_Data[compartment].Cycle_State == CYCLE_STATE_RUN) &&
        (Interpreter_Data[compartment].Cps.Phase >= (Itp_Phase_Data[compartment].Number_Of_Cycle_Phases - 1)))
    {
        if (Interpreter_Status.Sub_Cycle[compartment] != 0)
        {
            ActivateTask__SubCycleEnded(compartment);
            SUB_CYCLE_END(compartment,  Interpreter_Status.Sub_Cycle[compartment], SUB_CYCLE_END_COMPLETED);
            RestoreFromSubCycleResumeData(compartment);
            Interpreter_Data[compartment].Resume_From_Pause = TRUE;
            LoadPhaseData(compartment);

            return INTERPRETER_RESULT_END_PHASE;
        }
        else
        {
            return INTERPRETER_RESULT_END_CYCLE;
        }
    }

    if (Interpreter_Data[compartment].Cycle_State == CYCLE_STATE_RUN)
    {
        Interpreter_Data[compartment].Cps.Phase++;
        LOG_ADD_EVENT(STARTED_NEW_PHASE, Interpreter_Data[compartment].Cps.Phase);
    }
    Interpreter_Data[compartment].Cps.Statement = 0;
    LoadPhaseData(compartment);

    return INTERPRETER_RESULT_END_PHASE;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Save Interpreter_Data[compartment] to Pause_Resume_Data[compartment].
 *              Save Interpreter_Status[compartment] to Pause_Resume_Status[compartment].
 *
 *  @param      uint8 compartment - identifier appliance compartment. (0 based, 0 for single compartment appliances)
 */
void Interpreter__SavePauseResumeData(uint8 compartment)
{
    if (compartment >= APPLIANCE_MAX_COMPARTMENTS)
    {
        LOG_ADD_EXCEPTION(COMPARTMENT_OUT_OF_RANGE, compartment);
    }
    else
    {
        Pause_Resume_Data[compartment] = Interpreter_Data[compartment];

        Pause_Resume_Status.Main_Cycle[compartment] = Interpreter_Status.Main_Cycle[compartment];
        Pause_Resume_Status.Main_Phase[compartment] = Interpreter_Status.Main_Phase[compartment];
        Pause_Resume_Status.Main_Step[compartment] = Interpreter_Status.Main_Step[compartment];
        Pause_Resume_Status.Sub_Cycle[compartment] = Interpreter_Status.Sub_Cycle[compartment];
        Pause_Resume_Status.Sub_Phase[compartment] = Interpreter_Status.Sub_Phase[compartment];
        Pause_Resume_Status.Sub_Cycle_Step[compartment] = Interpreter_Status.Sub_Cycle_Step[compartment];
        Pause_Resume_Status.Step_Time_Elapsed[compartment] = Interpreter_Status.Step_Time_Elapsed[compartment];
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Set compartment Interpreter Phase to phase (If requested phase is valid).
 *              Shall only be called by Supervisor in the RUN State.
 *
 *  @param      uint8 compartment - identifier appliance compartment. (0 based, 0 for single compartment appliances)
 *  @param      uint8 phase - Phase (0-127), PHASE_STEP_FORWARD, PHASE_STEP_BACKWARD or PHASE_STEP_NONE.
 *
 *  @return     PHASE_STEP_RESULT_TYPE
 */
PHASE_STEP_RESULT_TYPE Interpreter__SetPhase(uint8 compartment, uint8 phase)
{
    INTERPRETER_CONFIG_TYPE interpreter_config;
    PHASE_STEP_RESULT_TYPE phase_step_result;

    phase_step_result = PHASE_STEP_RESULT_FAIL;
    if ((phase == PHASE_STEP_BACKWARD) &&
        (Interpreter_Data[compartment].Cps.Phase != 0))
    {
        phase_step_result = PHASE_STEP_RESULT_PASS;
        phase  = Interpreter_Data[compartment].Cps.Phase - 1;
    }
    else if ((phase == PHASE_STEP_FORWARD) &&
             (Interpreter_Data[compartment].Cps.Phase < (Itp_Phase_Data[compartment].Number_Of_Cycle_Phases - 1)))
    {
        phase = Interpreter_Data[compartment].Cps.Phase + 1;
        phase_step_result = PHASE_STEP_RESULT_PASS;
    }
    else if (phase == PHASE_STEP_FORWARD)
    {
        phase_step_result = PHASE_STEP_RESULT_END;
    }
    else if ((phase < MAX_PHASES)  &&
             (phase < Itp_Phase_Data[compartment].Number_Of_Cycle_Phases))
    {
        phase_step_result = PHASE_STEP_RESULT_PASS;
    }

    if (phase_step_result == PHASE_STEP_RESULT_PASS)
    {
        interpreter_config.Setting_File_Index = Interpreter_Data[compartment].Setting_File_Index;
        interpreter_config.Cycle = Interpreter_Data[compartment].Cps.Cycle;
        interpreter_config.Phase = phase;
        interpreter_config.Statement = 0;
        interpreter_config.Cycle_State = Interpreter_Data[compartment].Cycle_State;
        Interpreter__Configure(compartment, &interpreter_config);
    }

    return (phase_step_result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Set compartment Interpreter Step to step (If step is valid).
 *              Shall only be called by Supervisor in the RUN State.
 *
 *  @param      uint8 compartment - identifier appliance compartment. (0 based, 0 for single compartment appliances)
 *  @param      uint8 step - Step (0-127), PHASE_STEP_FORWARD, PHASE_STEP_BACKWARD or PHASE_STEP_NONE.
 *
 *  @return     PHASE_STEP_RESULT_TYPE phase_step_result
 */
PHASE_STEP_RESULT_TYPE Interpreter__SetStep(uint8 compartment, uint8 step)
{
    INTERPRETER_CONFIG_TYPE interpreter_config;
    PHASE_STEP_RESULT_TYPE phase_step_result = PHASE_STEP_RESULT_FAIL;
    uint8 number_of_steps = GetNumberOfSteps(compartment);
    uint8 current_step;

    // Set current step.
    if (Interpreter_Status.Sub_Cycle[compartment] != 0)
    {
        current_step = Interpreter_Status.Sub_Cycle_Step[compartment];
    }
    else
    {
        current_step = Interpreter_Status.Main_Step[compartment];
    }

    // Check boundaries.
    if ((step == PHASE_STEP_BACKWARD) && (current_step != 0))
    {
        phase_step_result = PHASE_STEP_RESULT_PASS;
        step  = current_step - 1;
    }
    else if (step == PHASE_STEP_BACKWARD)
    {
        // In step 0, check for previous phase.
        phase_step_result = Interpreter__SetPhase(compartment, PHASE_STEP_BACKWARD);
        if (phase_step_result == PHASE_STEP_RESULT_PASS)
        {
            step  = GetNumberOfSteps(compartment) - 1;
        }
    }
    else if ((step == PHASE_STEP_FORWARD) && current_step < (number_of_steps - 1))
    {
        step  = current_step + 1;
        phase_step_result = PHASE_STEP_RESULT_PASS;
    }
    else if (step == PHASE_STEP_FORWARD)
    {
        // In last step, check for next phase.
        phase_step_result = Interpreter__SetPhase(compartment, PHASE_STEP_FORWARD);
    }
    else if (step < number_of_steps)
    {
        phase_step_result = PHASE_STEP_RESULT_PASS;
    }
    // Configure if boundary checks are OK.
    if (phase_step_result == PHASE_STEP_RESULT_PASS)
    {
        interpreter_config.Setting_File_Index = Interpreter_Data[compartment].Setting_File_Index;
        interpreter_config.Cycle = Interpreter_Data[compartment].Cps.Cycle;
        interpreter_config.Phase = Interpreter_Data[compartment].Cps.Phase;
        interpreter_config.Statement = GetFirstStatement(compartment, step);
        interpreter_config.Cycle_State = Interpreter_Data[compartment].Cycle_State;
        Interpreter__Configure(compartment, &interpreter_config);
    }

    return (phase_step_result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Terminate compartment sub-cycle if active.
 *
 *  @param      uint8 compartment - identifier appliance compartment. (0 based, 0 for single compartment appliances)
 *
 *  @retval     TRUE - Sub-cycle in compartment was terminated.
 *  @retval     FALSE - Paused or running sub-cycle was not found in compartment.
 */
BOOL_TYPE Interpreter__TerminateSubCycle(uint8 compartment)
{
    BOOL_TYPE terminated = FALSE;

    CYCLE_PHASE_STATEMENT_TYPE cps = Interpreter__GetCurrentStatement(compartment);
    CYCLE_STATE_TYPE cycle_state = Selector__GetCycleState(0, compartment, cps.Cycle);

    if (Interpreter_Status.Sub_Cycle[compartment] != 0)
    {
        if (cycle_state == CYCLE_STATE_RUN)
        {
            ActivateTask__SubCycleEnded(compartment);
            SUB_CYCLE_END(compartment,  Interpreter_Status.Sub_Cycle[compartment], SUB_CYCLE_END_TERMINATED);
            RestoreFromSubCycleResumeData(compartment);
            Interpreter_Data[compartment].Resume_From_Pause = TRUE;
            LoadPhaseData(compartment);
            Interpreter__ClearSubCycleStatus(compartment);
            terminated = TRUE;
        }
        else if  (cycle_state == CYCLE_STATE_PAUSE)
        {
            ActivateTask__SubCycleEnded(compartment);
            SUB_CYCLE_END(compartment,  Interpreter_Status.Sub_Cycle[compartment], SUB_CYCLE_END_TERMINATED);
            RecoverPauseResumeData(compartment);
            Interpreter__ClearSubCycleStatus(compartment);
            terminated = TRUE;
        }
    }

    return (terminated);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Manages detection and switching to sub cycle.
 *
 *  @param      uint8 compartment - identifier appliance compartment. (0 based, 0 for single compartment appliances)
 */
static void CheckForSubCycle(uint8 compartment)
{
    // compartment is already validated from calling function.
    uint8 sub_cycle = SubCycle__GetActive(compartment, Interpreter_Status.Sub_Cycle[compartment]);

#if (SUB_CYCLE_ALLOWED_DURING_TEST == DISABLED)
    CYCLE_NAME_TYPE cycle_name = Selector__GetCycleNameId(Interpreter_Data[compartment].Setting_File_Index,
                                                          compartment,
                                                          Interpreter_Data[compartment].Cps.Cycle);
#endif

    if ((sub_cycle != 0) &&                                             // SubCycle__GetActive returns 0 if no sub-cycle should be activated.
        (sub_cycle != Interpreter_Data[compartment].Cps.Cycle)          // Active sub-cycle already running.
#if (SUB_CYCLE_ALLOWED_DURING_ALL_STATES == DISABLED)
        &&
        (Interpreter_Data[compartment].Cycle_State == CYCLE_STATE_RUN)  // Only enter sub-cycle if running a user defined cycle.
#endif
#if (SUB_CYCLE_ALLOWED_DURING_TEST == DISABLED)
        &&
        (cycle_name < CYCLE_NAME_EOL_0)                                 // Do not enter sub-cycle if EOL, Service or Demo cycle is running.
#endif
       )
    {
        if (OK_TO_START_SUB_CYCLE(compartment, sub_cycle, Interpreter_Data[compartment].Cycle_State))
        {
            CYCLE_STATE_TYPE state = Selector__GetCycleState(Interpreter_Data[compartment].Setting_File_Index, compartment, sub_cycle);
            if (state == CYCLE_STATE_RUN)
            {
                // Only store sub-cycle return data if no sub cycle is running
                if (Interpreter_Status.Sub_Cycle[compartment] == 0)
                {
                    // Store main cycle number of phases so that it can be limit checked on sub-cycle exit that skips to next phase.
                    // Stored every time a sub-cycle starts. Does not need to be cleared.
                    Main_Cycle_Number_Of_Phases[compartment] = Itp_Phase_Data[compartment].Number_Of_Cycle_Phases;
                    SaveSubCycleResumeData(compartment);
                    Interpreter_Status.Main_Cycle[compartment] = Interpreter_Data[compartment].Cps.Cycle;
                    Interpreter_Status.Main_Phase[compartment] = Interpreter_Data[compartment].Cps.Phase;
                }
                else
                {
                    // Ending current sub-cycle and switching to new higher priority sub-cycle.
                    // Interpreter main cycle data is already stored (nested sub-cycle not allowed).
                    ActivateTask__SubCycleEnded(compartment);
                    SUB_CYCLE_END(compartment, Interpreter_Status.Sub_Cycle[compartment], SUB_CYCLE_END_PREEMPTED);
                }

                Interpreter_Status.Sub_Cycle[compartment] = sub_cycle;
                Interpreter_Status.Step_Time_Elapsed[compartment] = 0;

                Interpreter_Data[compartment].Cps.Cycle = sub_cycle;
                Interpreter_Data[compartment].Cps.Phase = 0;
                Interpreter_Data[compartment].Cps.Statement = 0;
                Interpreter_Data[compartment].Cycle_State = state;

                Interpreter_Data[compartment].Step_Initializing = TRUE;
                Interpreter_Data[compartment].Step_Time_Counter = 0;

                LoadPhaseData(compartment);
                ActivateTask__SubCycleStarted(compartment);
                SUB_CYCLE_START(compartment, sub_cycle);
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Exit sub-cycle according to completion_code from settings file.
 *
 *  @param      uint8 compartment - identifier appliance compartment. (0 based, 0 for single compartment appliances)
 *  @param      uint8 data_field_index - displacement to retrieve completion code from SF_PTR_ACU_PROCESS_SET_STATE
 *
 *  @retval     INTERPRETER_RESULT_END_SUB_CYCLE - end sub-cycle normally
 *  @retval     INTERPRETER_RESULT_END_CYCLE - SUB_CYCLE_COMPLETION_CODE_SKIP_PHASE requested in last phase of cycle.
 */
static INTERPRETER_RESULT_TYPE ExitSubCycle(uint8 compartment, uint8 data_field_index)
{
    SETTINGFILE_LOADER_TYPE sf_loader_data;
    PASS_FAIL_TYPE load_success;
    SUB_CYCLE_COMPLETION_CODE_TYPE completion_code;
    INTERPRETER_RESULT_TYPE interpreter_result = INTERPRETER_RESULT_END_SUB_CYCLE;

    if (Interpreter_Status.Sub_Cycle[compartment] != 0)
    {
        load_success = SettingFile__MSettingBasicLoader(Interpreter_Data[compartment].Setting_File_Index,
                                                        SF_PTR_ACU_PROCESS_SET_STATE,
                                                        data_field_index,
                                                        &sf_loader_data);
        if (load_success == PASS)
        {
            completion_code = (SUB_CYCLE_COMPLETION_CODE_TYPE)sf_loader_data.Data[0];
            if ( completion_code < SUB_CYCLE_COMPLETION_CODE_MAX)
            {
                ActivateTask__SubCycleEnded(compartment);
                SUB_CYCLE_END(compartment,  Interpreter_Status.Sub_Cycle[compartment], SUB_CYCLE_END_SET_STATE_PROCESS);
                RestoreFromSubCycleResumeData(compartment);

                if (completion_code == SUB_CYCLE_COMPLETION_CODE_RESUME)
                {
                    Interpreter_Data[compartment].Resume_From_Pause = TRUE;
                }
                else
                {
                    // SUB_CYCLE_COMPLETION_CODE_RESTART_STEP
                    Interpreter_Data[compartment].Step_Time_Counter = 0;
                    Interpreter_Status.Step_Time_Elapsed[compartment] = 0;
                    Interpreter_Data[compartment].Step_Initializing = TRUE;
                    Interpreter_Data[compartment].Resume_From_Pause = FALSE;

                    if (completion_code == SUB_CYCLE_COMPLETION_CODE_RESTART_PHASE)
                    {
                        Interpreter_Data[compartment].Cps.Statement = 0;
                    }
                    else if (completion_code == SUB_CYCLE_COMPLETION_CODE_SKIP_PHASE)
                    {
                        if  (Interpreter_Data[compartment].Cps.Phase < (Main_Cycle_Number_Of_Phases[compartment] - 1))
                        {
                            Interpreter_Data[compartment].Cps.Statement = 0;
                            Interpreter_Data[compartment].Cps.Phase++;
                        }
                        else
                        {
                            LOG_ADD_EXCEPTION(SUB_CYCLE_COMPLETION_INVALID_PHASE, sf_loader_data.Data[0]);
                            interpreter_result = INTERPRETER_RESULT_END_CYCLE;
                        }
                    }
                }

                if (interpreter_result != INTERPRETER_RESULT_END_CYCLE)
                {
                    LoadPhaseData(compartment);
                }
            }
            else
            {
              LOG_ADD_EXCEPTION(INVALID_COMPLETION_CODE, sf_loader_data.Data[0]);
            }
        }
        else
        {
            LOG_ADD_EXCEPTION(SF_PTR_SUB_CYCLE_FAIL, Interpreter_Data[compartment].Setting_File_Index);
        }
    }
    else
    {
        LOG_ADD_EXCEPTION(INVALID_END_SUB_CYCLE, 0);
    }

    return (interpreter_result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      get the cycle completion code for set state process
 *
 *  @param      uint8 compartment - identifier appliance compartment. (0 based, 0 for single compartment appliances)
 *
 *  @return     CYCLE_COMPLETION_CODE_TYPE cycle_completion_code - cycle completion code for set state process
 */
static CYCLE_COMPLETION_CODE_TYPE GetCycleCompletionCode(uint8 compartment, uint8 data_index)
{
    SETTINGFILE_LOADER_TYPE sf_loader_data;
    PASS_FAIL_TYPE load_success;
    CYCLE_COMPLETION_CODE_TYPE cycle_completion_code;

    cycle_completion_code = CYCLE_COMPLETION_CODE_END;

    load_success = SettingFile__MSettingBasicLoader(Interpreter_Data[compartment].Setting_File_Index,
                                                    SF_PTR_ACU_PROCESS_SET_STATE,
                                                    data_index,
                                                    &sf_loader_data);
    if (load_success == PASS)
    {

        if ( sf_loader_data.Data[0] < (uint8)CYCLE_COMPLETION_CODE_MAX)
        {
            cycle_completion_code = (CYCLE_COMPLETION_CODE_TYPE)sf_loader_data.Data[0];
        }
        else
        {
            LOG_ADD_EXCEPTION(INVALID_COMPLETION_CODE, sf_loader_data.Data[0]);
        }
    }

    return (cycle_completion_code);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get the first statement for step in current compartment phase macro.
 *
 *  @param      uint8 compartment - identifier appliance compartment. (0 based, 0 for single compartment appliances)
 *  @param      uint8 step - Step of requested first statement.
 *
 *  @return     uint8 first_statement - first statement of step.
 */
static uint8 GetFirstStatement(uint8 compartment, uint8 step)
{
    uint8 first_statement = 0;
    uint8 current_step = 0;
    BOOL_TYPE t_flag;

    t_flag = Itp_Phase_Data[compartment].Hi_Statements[0].T_Flag;

    for (uint8 statement = 0; statement < Itp_Phase_Data[compartment].Number_Of_Phase_Statements; statement++)
    {
        if (Itp_Phase_Data[compartment].Hi_Statements[statement].T_Flag != t_flag)
        {
            t_flag = Itp_Phase_Data[compartment].Hi_Statements[statement].T_Flag;
            current_step++;
        }

        if (current_step == step)
        {
            first_statement = statement;
            break;
        }
    }

    return (first_statement);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get the lo statement index for compartment statement.
 *
 *  @param      uint8 compartment - Identifier appliance compartment (0 based, 0 for single compartment appliances).
 *
 *  @return     uint16 lo_statement_index - lo statement data index.
 */
static uint16 GetLoStatementIndex(uint8 compartment, uint8 statement)
{
    uint16 lo_statement_index = 0;

    for (uint8 index = 1; index <= statement; index++)
    {
        lo_statement_index += 1 + BIT_TEST(*Lo_Statement_Index_Length_ptr, Itp_Phase_Data[compartment].Hi_Statements[index-1].Op_Code);
    }

    return (lo_statement_index);
}
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get the number of steps in the current phase.
 *
 *  @param      uint8 compartment - identifier appliance compartment. (0 based, 0 for single compartment appliances)
 *
 *  @return     uint8 number_of_steps - number of steps in current phase for compartment.
 */
static uint8 GetNumberOfSteps(uint8 compartment)
{
    BOOL_TYPE t_flag;
    uint8 number_of_steps = 1;

    t_flag = Itp_Phase_Data[compartment].Hi_Statements[0].T_Flag;

    for (uint8 statement = 0; statement < Itp_Phase_Data[compartment].Number_Of_Phase_Statements; statement++)
    {
        if (Itp_Phase_Data[compartment].Hi_Statements[statement].T_Flag != t_flag)
        {
            t_flag = Itp_Phase_Data[compartment].Hi_Statements[statement].T_Flag;
            number_of_steps++;
        }
    }

    return (number_of_steps);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Calls Selector module to load phase data from the setting file
 *
 *  @param      uint8 compartment - identifier appliance compartment. (0 based, 0 for single compartment appliances)
 */
static void LoadPhaseData(uint8 compartment)
{
    PASS_FAIL_TYPE load_success;

    load_success = Selector__LoadPhaseData(Interpreter_Data[compartment].Setting_File_Index,
                                           compartment,
                                           Interpreter_Data[compartment].Cps.Cycle,
                                           Interpreter_Data[compartment].Cps.Phase,
                                           &Itp_Phase_Data[compartment]);
    if (load_success == PASS)
    {
        if (Interpreter_Data[compartment].Cps.Statement < Itp_Phase_Data[compartment].Number_Of_Phase_Statements)
        {
            Interpreter_Data[compartment].SF_Error = FALSE;
            Interpreter_Data[compartment].Lo_Statement_Index = GetLoStatementIndex(compartment, Interpreter_Data[compartment].Cps.Statement);

            if (Interpreter_Status.Sub_Cycle[compartment] == 0)
            {
                Interpreter_Status.Main_Cycle[compartment] = Interpreter_Data[compartment].Cps.Cycle;
                Interpreter_Status.Main_Phase[compartment] = Interpreter_Data[compartment].Cps.Phase;

                Interpreter_Status.Sub_Phase[compartment] = 0;
            }
            else
            {
                if ((Interpreter_Data[compartment].Cycle_State  == CYCLE_STATE_PAUSE) ||
                    (Interpreter_Data[compartment].Cycle_State == CYCLE_STATE_END))
                {
                    Interpreter_Status.Main_Cycle[compartment] = Interpreter_Data[compartment].Cps.Cycle;
                    Interpreter_Status.Main_Phase[compartment] = Interpreter_Data[compartment].Cps.Phase;
                }
                else
                {
                    // Main_Cycle and Main_Phase status set in CheckForSubCycle when the sub-cycle is accepted.
                    Interpreter_Status.Sub_Phase[compartment] = Interpreter_Data[compartment].Cps.Phase;
                }
            }

            SetStepStatus(compartment);
        }
        else
        {
            Interpreter_Data[compartment].SF_Error = TRUE;
            LOG_ADD_EXCEPTION(FIRST_STATEMENT_OUT_OF_RANGE, compartment);
        }
    }
    else
    {
        Interpreter_Data[compartment].SF_Error = TRUE;
        // Exception handled by selector module.
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Restore Interpreter_Data[compartment] from Sub_Cycle_Resume_Data[compartment].
 *              Restore Interpreter_Status[compartment] from Sub_Cycle_Resume_Status[compartment].
 *              Clear Sub_Cycle_Resume_Data and Sub_Cycle_Resume_Status.
 *
 *  @param      uint8 compartment - identifier appliance compartment. (0 based, 0 for single compartment appliances)
 */
static void RestoreFromSubCycleResumeData(uint8 compartment)
{
    // Restore data/status
    Interpreter_Data[compartment] = Sub_Cycle_Resume_Data[compartment];

    Interpreter_Status.Main_Cycle[compartment]        = Sub_Cycle_Resume_Status.Main_Cycle[compartment];
    Interpreter_Status.Main_Phase[compartment]        = Sub_Cycle_Resume_Status.Main_Phase[compartment];
    Interpreter_Status.Main_Step[compartment]         = Sub_Cycle_Resume_Status.Main_Step[compartment];
    Interpreter_Status.Sub_Cycle[compartment]         = Sub_Cycle_Resume_Status.Sub_Cycle[compartment];
    Interpreter_Status.Sub_Phase[compartment]         = Sub_Cycle_Resume_Status.Sub_Phase[compartment];
    Interpreter_Status.Sub_Cycle_Step[compartment]    = Sub_Cycle_Resume_Status.Sub_Cycle_Step[compartment];
    Interpreter_Status.Step_Time_Elapsed[compartment] = Sub_Cycle_Resume_Status.Step_Time_Elapsed[compartment];

    // Clear resume data/status.
    memset(&Sub_Cycle_Resume_Data[compartment], 0x00, sizeof(Sub_Cycle_Resume_Data[0]));

    Sub_Cycle_Resume_Status.Main_Cycle[compartment]= 0;
    Sub_Cycle_Resume_Status.Main_Phase[compartment]= 0;
    Sub_Cycle_Resume_Status.Main_Step[compartment]= 0;
    Sub_Cycle_Resume_Status.Sub_Cycle[compartment]= 0;
    Sub_Cycle_Resume_Status.Sub_Phase[compartment]= 0;
    Sub_Cycle_Resume_Status.Sub_Cycle_Step[compartment]= 0;
    Sub_Cycle_Resume_Status.Step_Time_Elapsed[compartment]= 0;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Save Interpreter_Data[compartment] to Sub_Cycle_Resume_Data[compartment].
 *              Save Interpreter_Status[compartment] to Sub_Cycle_Resume_Status[compartment].
 *
 *  @param      uint8 compartment - identifier appliance compartment. (0 based, 0 for single compartment appliances)
 */
static void SaveSubCycleResumeData(uint8 compartment)
{
    Sub_Cycle_Resume_Data[compartment] = Interpreter_Data[compartment];

    Sub_Cycle_Resume_Status.Main_Cycle[compartment]        = Interpreter_Status.Main_Cycle[compartment];
    Sub_Cycle_Resume_Status.Main_Phase[compartment]        = Interpreter_Status.Main_Phase[compartment];
    Sub_Cycle_Resume_Status.Main_Step[compartment]         = Interpreter_Status.Main_Step[compartment];
    Sub_Cycle_Resume_Status.Sub_Cycle[compartment]         = Interpreter_Status.Sub_Cycle[compartment];
    Sub_Cycle_Resume_Status.Sub_Phase[compartment]         = Interpreter_Status.Sub_Phase[compartment];
    Sub_Cycle_Resume_Status.Sub_Cycle_Step[compartment]    = Interpreter_Status.Sub_Cycle_Step[compartment];
    Sub_Cycle_Resume_Status.Step_Time_Elapsed[compartment] = Interpreter_Status.Step_Time_Elapsed[compartment];
}


/**
 *  @brief      Set the step status for main and sub-cycle.
 *
 *  @param      uint8 compartment - identifier appliance compartment. (0 based, 0 for single compartment appliances)
 */
static void SetStepStatus(uint8 compartment)
{
    BOOL_TYPE t_flag;
    uint8 main_step = 0;
    uint8 sub_cycle_step = 0;

    t_flag = Itp_Phase_Data[compartment].Hi_Statements[0].T_Flag;

    for (uint8 statement = 0; statement < Itp_Phase_Data[compartment].Number_Of_Phase_Statements; statement++)
    {
        if (Itp_Phase_Data[compartment].Hi_Statements[statement].T_Flag != t_flag)
        {
            t_flag = Itp_Phase_Data[compartment].Hi_Statements[statement].T_Flag;
            if (Interpreter_Status.Sub_Cycle[compartment] == 0)
            {
                main_step++;
            }
            else
            {
                sub_cycle_step++;
            }
        }

        if (statement == Interpreter_Data[compartment].Cps.Statement)
        {
            break;
        }
    }

    if (Interpreter_Status.Sub_Cycle[compartment] == 0)
    {
        Interpreter_Status.Main_Step[compartment] = main_step;
    }
    Interpreter_Status.Sub_Cycle_Step[compartment] = sub_cycle_step;
}

/**
 *  @brief      Recover main cycle resume data when sub-cycle is canceled during pause.
 *              Copies Sub_Cycle_Resume_Data to Pause_Resume_Data.
 *              Copies Sub_Cycle_Resume_Status to Pause_Resume_Status.
 *              Lastly, clears Sub_Cycle_Resume_Data and Sub_Cycle_Resume_Status.
 *
 *  @param      uint8 compartment - identifier appliance compartment. (0 based, 0 for single compartment appliances)
 */
static void RecoverPauseResumeData(uint8 compartment)
{
    // Restore data/status
    Pause_Resume_Data[compartment] = Sub_Cycle_Resume_Data[compartment];

    Pause_Resume_Status.Main_Cycle[compartment]        = Sub_Cycle_Resume_Status.Main_Cycle[compartment];
    Pause_Resume_Status.Main_Phase[compartment]        = Sub_Cycle_Resume_Status.Main_Phase[compartment];
    Pause_Resume_Status.Main_Step[compartment]         = Sub_Cycle_Resume_Status.Main_Step[compartment];
    Pause_Resume_Status.Sub_Cycle[compartment]         = Sub_Cycle_Resume_Status.Sub_Cycle[compartment];
    Pause_Resume_Status.Sub_Phase[compartment]         = Sub_Cycle_Resume_Status.Sub_Phase[compartment];
    Pause_Resume_Status.Sub_Cycle_Step[compartment]    = Sub_Cycle_Resume_Status.Sub_Cycle_Step[compartment];
    Pause_Resume_Status.Step_Time_Elapsed[compartment] = Sub_Cycle_Resume_Status.Step_Time_Elapsed[compartment];

    // Clear resume data/status.
    memset(&Sub_Cycle_Resume_Data[compartment], 0x00, sizeof(Sub_Cycle_Resume_Data[0]));

    Sub_Cycle_Resume_Status.Main_Cycle[compartment]= 0;
    Sub_Cycle_Resume_Status.Main_Phase[compartment]= 0;
    Sub_Cycle_Resume_Status.Main_Step[compartment]= 0;
    Sub_Cycle_Resume_Status.Sub_Cycle[compartment]= 0;
    Sub_Cycle_Resume_Status.Sub_Phase[compartment]= 0;
    Sub_Cycle_Resume_Status.Sub_Cycle_Step[compartment]= 0;
    Sub_Cycle_Resume_Status.Step_Time_Elapsed[compartment]= 0;

}

/**
 *  @file
 *
 *  @brief      Public interface for the Interpreter module.
 *
 *  @copyright  Copyright 2015-2019 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef INTERPRETER_H_
#define INTERPRETER_H_

//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"

// -- Other Modules --
#include "JumpIf.h"                                 // CYCLE_PHASE_STATEMENT_TYPE
#include "Selector.h"                               // CYCLE_STATE_TYPE

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//! Maximum number of steps to run for continuation of loads.
#define INTERPRETER_MAX_STEPS       (10)

//! Phase value for step backward when setting from an external device.
#define PHASE_STEP_BACKWARD (0xFE)

//! Phase value for step forward when setting from an external device.
#define PHASE_STEP_FORWARD  (0xFF)

//! Phase value for none Requested from external device.
#define PHASE_STEP_NONE     (0xFD)

//! Interpreter configuration type.
PACKED typedef struct INTERPRETER_CONFIG_STRUCT
{
    uint8 Setting_File_Index;                       //!< The setting file that the Interpreter shall use.
    uint8 Cycle;                                    //!< The cycle that the interpreter shall run.
    uint8 Phase;                                    //!< The phase of the cycle that the interpreter shall run.
    uint8 Statement;                                //!< The statement the interpreter shall start on.
    CYCLE_STATE_TYPE Cycle_State;                   //!< The state cycle that the interpreter shall run.
} INTERPRETER_CONFIG_TYPE;

//! Interpreter result type returned to the Supervisor.
typedef enum INTERPRETER_RESULT_ENUM
{
    INTERPRETER_RESULT_END_STEP             = 0x00,
    INTERPRETER_RESULT_END_PHASE            = 0x01,
    INTERPRETER_RESULT_END_CYCLE            = 0x02,
    INTERPRETER_RESULT_END_CYCLE_RESET      = 0x03,
    INTERPRETER_RESULT_SAME_STEP            = 0x04,
    INTERPRETER_RESULT_PROGRAM_FLOW_BREAK   = 0x05,
    INTERPRETER_RESULT_END_SUB_CYCLE        = 0x06,
    INTERPRETER_RESULT_PAUSE                = 0x07,
    INTERPRETER_RESULT_INVALID_COMPARTMENT  = 0x08,
    INTERPRETER_RESULT_ERROR                = 0x80
} INTERPRETER_RESULT_TYPE;

//! Structure type to hold Interpreter Status Variables
//! Required in header for access by Variable module.
PACKED typedef struct INTERPRETER_STATUS_STRUCT
{
    uint8 Main_Cycle[APPLIANCE_MAX_COMPARTMENTS];                               //!< Current main or state macro for selector.
    uint8 Main_Phase[APPLIANCE_MAX_COMPARTMENTS];                               //!< Current phase for main cycle.
    uint8 Main_Step[APPLIANCE_MAX_COMPARTMENTS];                                //!< Current step of phase for main cycle.
    uint8 Sub_Cycle[APPLIANCE_MAX_COMPARTMENTS];                                //!< Current sub-cycle (zero if not running).
    uint8 Sub_Phase[APPLIANCE_MAX_COMPARTMENTS];                                //!< Current phase of sub-cycle (zero if not running).
    uint8 Sub_Cycle_Step[APPLIANCE_MAX_COMPARTMENTS];                           //!< Current step of phase for sub-cycle zero if not running.
    uint16 Step_Time_Elapsed[APPLIANCE_MAX_COMPARTMENTS];                       //!< Elapsed time for current step.
} INTERPRETER_STATUS_TYPE;

//! Structure type to hold Interpreter run time data.
PACKED typedef struct RUN_TIME_STRUCT
{
    uint8 Setting_File_Index;                       //!< The setting file index for cycle run time data.
    CYCLE_PHASE_STATEMENT_TYPE Cps;                 //!< Run time Cycle, Phase Statement.
    uint16 Elapsed;                                 //!< The number of seconds that have already been spent in the step.
    uint16 Lo_Statement_Index;                      //!< Index (displacement) to process data table.
} RUN_TIME_DATA_TYPE;

//! Step result allows for ending the cylce in the last step of the last phase.
typedef enum PHASE_STEP_RESULT_ENUM
{
    PHASE_STEP_RESULT_FAIL = 0,
    PHASE_STEP_RESULT_PASS = 1,
    PHASE_STEP_RESULT_END = 2
} PHASE_STEP_RESULT_TYPE;

//! Enumeration type definition for reason why the sub-cycle ended
typedef enum SUB_CYCLE_END_ENUM
{
    SUB_CYCLE_END_COMPLETED         = 0,            //!< Sub-cycle ran to completion.
    SUB_CYCLE_END_TERMINATED        = 1,            //!< Sub-cycle terminated by ACU application firmware.
    SUB_CYCLE_END_PREEMPTED         = 2,            //!< Sub-cycle interrupted by a higher priority sub-cycle.
    SUB_CYCLE_END_SET_STATE_PROCESS = 3             //!< Sub-cycle ended by an END SUB-CYCLE statement.
} SUB_CYCLE_END_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void                            Interpreter__Initialize                 (void);
void                            Interpreter__ClearSubCycleStatus        (uint8 compartment);
void                            Interpreter__Configure                  (uint8 compartment, INTERPRETER_CONFIG_TYPE *config);
uint8                           Interpreter__GetCurrentSFIndex          (uint8 compartment);
uint16                          Interpreter__GetCurrentStepTimeElapsed  (uint8 compartment);
CYCLE_PHASE_STATEMENT_TYPE      Interpreter__GetCurrentStatement        (uint8 compartment);
BOOL_TYPE                       Interpreter__GetRunTimeData             (uint8 compartment,
                                                                         RUN_TIME_DATA_TYPE *running_data,
                                                                         RUN_TIME_DATA_TYPE *return_data);
INTERPRETER_STATUS_TYPE         Interpreter__GetStatus                  (void);
BOOL_TYPE                       Interpreter__IsSubCycleRunning          (uint8 compartment);
void                            Interpreter__RestoreFromPauseResumeData (uint8 compartment);
void                            Interpreter__Resume                     (uint8 compartment);
INTERPRETER_RESULT_TYPE         Interpreter__Run                        (uint8 compartment);
void                            Interpreter__SavePauseResumeData        (uint8 compartment);
PHASE_STEP_RESULT_TYPE          Interpreter__SetPhase                   (uint8 compartment, uint8 phase);
PHASE_STEP_RESULT_TYPE          Interpreter__SetStep                    (uint8 compartment, uint8 step);
BOOL_TYPE                       Interpreter__TerminateSubCycle          (uint8 compartment);

#endif // INTERPRETER_H_

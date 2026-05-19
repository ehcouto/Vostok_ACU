/**
 *  @file
 *
 *  @brief      Public interface for the Selector module.
 *
 *  @copyright  Copyright 2015. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SELECTOR_H_
#define SELECTOR_H_

//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "Selector_prm.h"
#include "Process.h"

// -- Other Modules --

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#define MAX_CYCLES 128                              //!< Maximum number of cycles allowed.
#define MAX_PHASES 128                              //!< Maximum number of phases in a single cycle.
#define MAX_STATEMENTS 128                          //!< Maximum number of statements in a single phase.

//! Process op-code enumeration type.
typedef enum PROCESS_OPCODE_ENUM
{
    PROCESS_OPCODE_SET_STATE        = 0,            //!< The opcode that is specific to the Set State process.
    PROCESS_OPCODE_SET_VARIABLE     = 1,            //!< The opcode that is specific to the Set Variable process.
    PROCESS_OPCODE_MAINTAIN         = 2,            //!< The opcode that is specific to the Maintain process.
    PROCESS_OPCODE_LOADS_CONTROL    = 3,            //!< The opcode that is specific to the Loads Control process.
    PROCESS_OPCODE_JUMP_IF          = 4,            //!< The opcode that is specific to the JumpIf process.
    PROCESS_OPCODE_ACTIVATE_TASK    = 5             //!< The opcode that is specific to the Activate Task process.
} PROCESS_OPCODE_ENUM_TYPE;

//! Structure type to hold process High statement data.
PACKED typedef struct HI_STATEMENT_STRUCT
{
    PROCESS_OPCODE_ENUM_TYPE Op_Code    : 4;
    uint8 F1_F0_Flags                   : 2;
    BOOL_TYPE N_Flag                    : 1;
    BOOL_TYPE T_Flag                    : 1;
} HI_STATEMENT_TYPE;

//! Structure type to hold cycle phase data.
PACKED typedef struct PHASE_DATA_STRUCT
{
    HI_STATEMENT_TYPE   *Hi_Statements;             //!< Pointer to the start of Hi Statements for the current phase.
    uint8               *Lo_Statements;             //!< Pointer to the start of Lo Statements for the current phase.
    uint8               Time_Estimation_Index;      //!< Displacement for the time estimation data.
    uint8               Number_Of_Cycle_Phases;     //!< Number of phases in the current cycle.
    uint8               Number_Of_Phase_Statements; //!< Number of statements in the current phase.
} PHASE_DATA_TYPE;

//! Cycle State from GESE Cycle Heading
typedef enum CYCLE_STATE_ENUM
{
    CYCLE_STATE_PROGRAMMING  =       0,             //!< Cycle state PROGRAMMING
    CYCLE_STATE_RUN          =       1,             //!< Cycle state RUN
    CYCLE_STATE_DELAY        =       2,             //!< Cycle state DELAY
    CYCLE_STATE_END          =       3,             //!< Cycle state END
    CYCLE_STATE_PAUSE        =       4,             //!< Cycle state PAUSE
    CYCLE_STATE_FAULT        =       5,             //!< Cycle state FAULT
    CYCLE_STATE_INVALID      =       0xFF           //!< Cycle state INVALID
} CYCLE_STATE_TYPE;

//! Cycle Name from GESE Cycle Heading
typedef enum CYCLE_NAME_ENUM
{
    CYCLE_NAME_OFF      = 0,                    //!< Cycle name OFF

    CYCLE_NAME_EOL_0    = 65280,                //!< Cross Platform GESE Name for End Of Line Test 0 cycle
    CYCLE_NAME_EOL_1    = 65281,                //!< Cross Platform GESE Name for End Of Line Test 1 cycle
    CYCLE_NAME_EOL_2    = 65282,                //!< Cross Platform GESE Name for End Of Line Test 2 cycle

    CYCLE_NAME_SERVICE_TEST_AUTO      = 65296,  //!< Cross Platform GESE Name for Service Test Auto cycle
    CYCLE_NAME_SERVICE_TEST_COMPONENT = 65297,  //!< Cross Platform GESE Name for Service Test Component/Manual cycle

    CYCLE_NAME_CANCEL   = 65302,                //!< Cross Platform GESE Name for Cancelling State running cycle

    CYCLE_NAME_DEMO_0   = 65312,                //!< Cross Platform GESE Name for Demo 0 cycle

    CYCLE_NAME_INVALID  = 65535,                //!< Cross Platform GESE Name for Invalid cycle

#ifdef CYCLE_NAME_PLATFORM_SPECIFIC
    CYCLE_NAME_PLATFORM_SPECIFIC                // Platform specific cycle names (allowed range is 1 - 65279).
#endif
} CYCLE_NAME_TYPE;

// The following cycle names are defined for backwards compatibility and may be deprecated in the future
#define CYCLE_NAME_SERVICE_0    CYCLE_NAME_SERVICE_TEST_AUTO
#define CYCLE_NAME_SERVICE_1    CYCLE_NAME_SERVICE_TEST_COMPONENT

//! Cycle Group from GESE Cycle Heading. Used to identify related cycles.
typedef enum CYCLE_GROUP_ENUM
{
    CYCLE_GROUP_GENERIC             = 0,            //!< Cycle Group Generic
    CYCLE_GROUP_RESERVED            = 1,            //!< Cycle Group Reserved
    CYCLE_GROUP_EOL_TEST            = 2,            //!< Cycle Group End Of Line Test
    CYCLE_GROUP_SERVICE             = 3,            //!< Cycle Group Service
    CYCLE_GROUP_DEMO                = 4,            //!< Cycle Group Demo

    CYCLE_GROUP_INVALID             = 127,

#ifdef CYCLE_GROUP_PLATFORM_SPECIFIC
    CYCLE_GROUP_PLATFORM_SPECIFIC                   // Platform specific group names (allowed range is 5 - 126)
#endif
} CYCLE_GROUP_TYPE;

//! User phase name enumeration for RUN Cycles
typedef enum USER_PHASE_NAME_ENUM
{
    USER_PHASE_NAME_DEFAULT = 0,

    // 65280 - 65535 Reserved for Cross Category User Phase Names.
    USER_PHASE_NAME_INVALID = 65535,

#ifdef USER_PHASE_NAME_PLATFORM_SPECIFIC
    USER_PHASE_NAME_PLATFORM_SPECIFIC               // Platform specific phase name IDs (allowed range is 1 - 65279)
#endif
} USER_PHASE_NAME_TYPE;

//! Cycle cold reset behavior
typedef enum CYCLE_COLD_RESET_BEHAVIOR_ENUM
{
    CYCLE_COLD_RESET_BEHAVIOR_CANCEL        = 0,    //!< Cancel the cycle after a cold reset.
    CYCLE_COLD_RESET_BEHAVIOR_PAUSE         = 1,    //!< Pause the cycle after a cold reset.
    CYCLE_COLD_RESET_BEHAVIOR_RESUME        = 2,    //!< Resume the cycle after a cold reset.
    CYCLE_COLD_RESET_BEHAVIOR_RESTART_PHASE = 3,    //!< Resume and restart the phase after a cold reset.
    CYCLE_COLD_RESET_BEHAVIOR_RESTART_CYCLE = 4,    //!< Resume and restart the cycle after a cold reset.
    CYCLE_COLD_RESET_BEHAVIOR_SELECT        = 7     //!< Allow application to select behavior 0-4 after a cold reset.
} CYCLE_COLD_RESET_BEHAVIOR_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void                Selector__Initialize                (void);
BOOL_TYPE           Selector__DoesCompartmentExist      (uint8 sf_index, uint8 compartment);
uint8               Selector__GetCycleByName            (uint8 sf_index, uint8 compartment, CYCLE_NAME_TYPE cycle_name);
CYCLE_NAME_TYPE     Selector__GetCycleNameId            (uint8 sf_index, uint8 compartment, uint8 cycle);
CYCLE_STATE_TYPE    Selector__GetCycleState             (uint8 sf_index, uint8 compartment, uint8 cycle);
uint8               Selector__GetNumberOfCycles         (uint8 sf_index, uint8 compartment);
CYCLE_GROUP_TYPE    Selector__GetCycleGroup             (uint8 sf_index, uint8 compartment, uint8 cycle);
uint8               Selector__GetStateCycle             (uint8 sf_index, uint8 compartment, uint8 state_cycle);
PASS_FAIL_TYPE      Selector__LoadPhaseData             (uint8 sf_index, uint8 compartment, uint8 cycle, uint8 phase, PHASE_DATA_TYPE* phase_data);
BOOL_TYPE           Selector__OkToPauseCycle            (uint8 sf_index, uint8 compartment, uint8 cycle);

CYCLE_COLD_RESET_BEHAVIOR_TYPE  Selector__GetColdResetBehavior          (uint8 sf_index, uint8 compartment, uint8 cycle);
USER_PHASE_NAME_TYPE            Selector__GetUserPhaseName              (uint8 sf_index, uint8 compartment, uint8 cycle, uint8 phase);
uint16 *                        Selector__GetLoStatmentIndexLengthPtr   (void);


#endif // SELECTOR_H_

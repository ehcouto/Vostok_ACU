/**
 *  @file
 *
 *  @brief      Public interface for the Supervisor module.
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/30 12:01:46EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SUPERVISOR_H_
#define SUPERVISOR_H_

//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"

// -- Other Modules --
#include "Regulations.h"                            // REGULATIONS_START_COMMAND_TYPE

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void            Supervisor__Initialize          (void);
PASS_FAIL_TYPE  Supervisor__EngageTest          (uint8 test_cycle_id);
BOOL_TYPE       Supervisor__IsTestCycleActive   (void);
uint8           Supervisor__IsInPausedDelay     (uint8 compartment);
uint16          Supervisor__GetCycleTime        (uint8 compartment);
void            Supervisor__Run                 (uint8 compartment);
PASS_FAIL_TYPE  Supervisor__SetCycle            (uint8 compartment, uint8 cycle);
PASS_FAIL_TYPE  Supervisor__SetPhase            (uint8 compartment, uint8 phase);
PASS_FAIL_TYPE  Supervisor__SetStep             (uint8 compartment, uint8 step);
PASS_FAIL_TYPE  Supervisor__SetStart            (uint8 compartment, REGULATIONS_START_COMMAND_TYPE start_value);

#endif // SUPERVISOR_H_

/**
 * @file
 * @defgroup    CLASS_B
 * @brief       Public parameters for the SRTaskSequence module.
 *
 * @details     Refer to the C source file for more detailed information.
 *
 * @copyright   Copyright 2015-2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef SRTASKSEQUENCE_PRM_H_
#define SRTASKSEQUENCE_PRM_H_

//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public Enumerated Constant Declarations --

//! The list of zero-based sequence IDs used by the application.
//! The list must end with the SRTASKSEQUENCE_NUM_SEQUENCES declaration.
typedef enum SRTASKSEQUENCE_SEQUENCES_ENUM
{
    SRTASKSEQUENCE_INIT_PHASE,
    SRTASKSEQUENCE_TICK_HANDLER,
    SRTASKSEQUENCE_MAIN_HANDLER,
    SRTASKSEQUENCE_APPL_SAFETY_INIT,
    SRTASKSEQUENCE_APPL_SAFETY_THREAD,
    SRTASKSEQUENCE_NUM_SEQUENCES
} SRTASKSEQUENCE_SEQUENCES_TYPE;

//! A list of function IDs that are used by the SRTASKSEQUENCE_INIT_PHASE sequence in the
//! SEQUENCE_INIT_PHASE_STEPS[] array.
//! The function IDs must be unique (within this list) and they cannot be zero.
typedef enum SRTASKSEQUENCE_INIT_STEPS_ENUM
{
    SRTASKSEQUENCE_INIT_SRTASKMONITOR = 1,
    SRTASKSEQUENCE_INIT_SRISRMONITOR,
	SRTASKSEQUENCE_INIT_SRTIMERREFERENCE,
	// This must be used in SRCLOCKMONITOR_INIT_FLOW() macro
	SRTASKSEQUENCE_INIT_SRCLOCKMONITOR,
    SRTASKSEQUENCE_INIT_MICRO,
	SRTASKSEQUENCE_INIT_SETTINGFILE,
	SRTASKSEQUENCE_INIT_INITIALIZATION_TASKS,
	SRTASKSEQUENCE_INIT_OS_INITALIZE,
	SRTASKSEQUENCE_INIT_START_MAIN_THREAD,
	SRTASKSEQUENCE_INIT_START_MODE_THREADS,
	SRTASKSEQUENCE_INIT_COMPLETED,
	SRTASKSEQUENCE_INIT_NUM_STEPS
} SRTASKSEQUENCE_INIT_STEPS_TYPE;

//! A list of function IDs that are used by the SRTASKSEQUENCE_APPL_SAFETY_INIT sequence in the
//! SEQUENCE_APPL_SAFETY_INIT_STEPS[] array.
//! The function IDs must be unique (within this list) and they cannot be zero.
typedef enum SRTASKSEQUENCE_APPL_INIT_STEPS_ENUM
{
    SRTASKSEQUENCE_APPL_INIT_HBL_INITIALIZE = 1,
	SRTASKSEQUENCE_APPL_INIT_HBLHMI_INITIALIZE,
    // Here there must be defined steps for all Hbl SR-Pilots, SR-Loads, SR-LLIs, SR-GIs with same order in Hbl_prv.h

    SRTASKSEQUENCE_APPL_INIT_HBL_IO_INITIALIZE,
	SRTASKSEQUENCE_APPL_INIT_SRPRODUCTMGR,
	// Here there must be defined steps for all SRChecks__Initialize available (at least one per check)
	SRTASKSEQUENCE_APPL_INIT_SRDUMMYCHECK,
	SRTASKSEQUENCE_APPL_INIT_SRCHECKS_DONE,
	// Here there must be defined steps for all SRActions__Initialize  available (at least one per action)
	SRTASKSEQUENCE_APPL_INIT_SRDUMMYACTION,
	SRTASKSEQUENCE_APPL_INIT_SRACTIONS_DONE,

	SRTASKSEQUENCE_APPL_INIT_SRPRODUCTMGR_DONE,
	SRTASKSEQUENCE_APPL_INIT_HBL_DONE
} SRTASKSEQUENCE_APPL_INIT_STEPS_TYPE;

//! A list of function IDs that are used by the SRTASKSEQUENCE_APPL_SAFETY_THREAD sequence in the
//! SEQUENCE_APPL_SAFETY_THREAD_STEPS[] array.
//! The function IDs must be unique (within this list) and they cannot be zero.
typedef enum SRTASKSEQUENCE_APPL_STEPS_ENUM
{
    SRTASKSEQUENCE_APPL_SETTINGFILE_EXECUTED = 1,
	SRTASKSEQUENCE_APPL_CLASSA_EXECUTED,
	SRTASKSEQUENCE_APPL_SRHBL_START,
	SRTASKSEQUENCE_APPL_SRHBL_CFG_CHECKS,
	SRTASKSEQUENCE_APPL_SRPRODUCTMGR_START,
// Here there must be defined steps for all SRChecks available (at least two each check: Enter and Exit!)
	SRTASKSEQUENCE_APPL_SRDUMMYCHECK_ENTER,
	SRTASKSEQUENCE_APPL_SRDUMMYCHECK_EXIT,

	SRTASKSEQUENCE_APPL_SRCHECKS_DONE,
// Here there must be defined steps for all SRActions available (at least two each action: Enter and Exit!)
	SRTASKSEQUENCE_APPL_SRDUMMYACTION_ENTER,
	SRTASKSEQUENCE_APPL_SRDUMMYACTION_EXIT,

	SRTASKSEQUENCE_APPL_SRACTIONS_DONE,
	SRTASKSEQUENCE_APPL_SRPRODUCTMGR_DONE,
	SRTASKSEQUENCE_APPL_SRHBL_LOADS_PROCESSED,
	SRTASKSEQUENCE_APPL_SRHBLHMI_START,
	SRTASKSEQUENCE_APPL_MICRO_SRRAMTEST_ENTER,
	SRTASKSEQUENCE_APPL_MICRO_SRRAMTEST_EXIT,
    SRTASKSEQUENCE_APPL_MICRO_SRFLASHTEST_ENTER,
    SRTASKSEQUENCE_APPL_MICRO_SRFLASHTEST_EXIT,

    SRTASKSEQUENCE_APPL_COMPLETED
} SRTASKSEQUENCE_APPL_STEPS_TYPE;

typedef enum _SRTASKSEQUENCE_TICK_STEPS_ENUM
{
    SRTASKSEQUENCE_TICK_SRTASKSEQUENCE = 1,
    // Add here steps (if any) if there are Class B calls in SRMAIN__BEFORE_1MS() macro

    SRTASKSEQUENCE_TICK_SRTIMERREFERENCE,
    SRTASKSEQUENCE_TICK_SRTASKMONITOR,
    // This must be used in SRCLOCKMONITOR_1MS_FLOW() macro
    SRTASKSEQUENCE_TICK_SRCLOCKMONITOR,
    SRTASKSEQUENCE_TICK_COMMUNICATION,
    SRTASKSEQUENCE_TICK_TASKS,
    SRTASKSEQUENCE_TICK_TASK_STACK_SIGNATURE
} SRTASKSEQUENCE_TICK_STEPS_TYPE;

typedef enum _SRTASKSEQUENCE_MAIN_STEPS_ENUM
{
    SRTASKSEQUENCE_MAIN_MODE_HANDLER = 1,
    SRTASKSEQUENCE_MAIN_TASKS,
    // Add here steps (if any) if there are Class B calls in SRMainRunning_Tasks tasks list

    SRTASKSEQUENCE_MAIN_WATCHDOG,
    SRTASKSEQUENCE_MAIN_ISRMONITOR,
} SRTASKSEQUENCE_MAIN_STEPS_TYPE;

//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


#endif      // SRTASKSEQUENCE_PRM_H_

/**
 * @file
 * @defgroup    CLASS_B
 * @brief       Private parameters for the SRTaskSequence module.
 *
 * @details     Refer to the C source file for more detailed information.
 *
 * @copyright   Copyright 2015-2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef SRTASKSEQUENCE_PRV_H_
#define SRTASKSEQUENCE_PRV_H_

#include "SRSystemConfig.h"
#include "SystemConfig.h"
//=====================================================================================================================
//  --- Private Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Private Constant Declarations --

//! This compile-time option can be used to temporarily disable this module.
//! ENABLED  = The module will still detect an incorrect sequence execution and log an
//!            SRException, but it will not cause the microcontroller to reset.
//! DISABLED = The module will detect an incorrect sequence execution and log an SRException
//!            which will cause the microcontroller to reset.
//! WARNING: Either this value must be set to DISABLED for production code, or the module must be
//!          removed from the project.
#define SRTASKSEQUENCE_SKIP_RESET                   DISABLED

//---------------------------------------------------------------------------------------------------------------------

//! This compile-time option checks each sequence execution time to verify that it does not
//! consume more than the maximum specified time. The value is the time in milliseconds.
//! Set the value to 0 to disable the feature.
//! This feature may be disabled in production code.
#define SRTASKSEQUENCE_PENDING_TIMEOUT              100

//---------------------------------------------------------------------------------------------------------------------

//! This compile-time option saves sequence execution times in RAM.
//! ENABLED  = The module will save the longest sequence execution time for each defined sequence.
//! DISABLED = The feature is disabled.
//! This is not a Class B feature and may be DISABLED for production.
#define SRTASKSEQUENCE_STATS                        ENABLED

//---------------------------------------------------------------------------------------------------------------------


// -- Private Flash-Based Constant Definitions --

//! The definition of the steps that make up the SRTASKSEQUENCE_INIT_PHASE sequence.
//! List the steps in the order that they occur for the SRTASKSEQUENCE_INIT_PHASE sequence.
static const SRTASKSEQUENCE_INIT_STEPS_TYPE SRTaskSequence_Init_Steps[] =
{
    SRTASKSEQUENCE_INIT_SRTASKMONITOR,
    #if (SYSTEM_ENABLE_ISRMONITOR == ENABLED)
        SRTASKSEQUENCE_INIT_SRISRMONITOR,
    #endif
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
};

//! The definition of the steps that make up the SRTASKSEQUENCE_APPL_SAFETY_INIT sequence.
//! List the steps in the order that they occur for the SRTASKSEQUENCE_APPL_SAFETY_INIT sequence.
static const SRTASKSEQUENCE_APPL_INIT_STEPS_TYPE SRTaskSequence_Appl_Init_Steps[] =
{
    SRTASKSEQUENCE_APPL_INIT_HBL_INITIALIZE,

    // Here there must be defined steps for all Hbl SR-Pilots, SR-Loads, SR-LLIs, SR-GIs with same order in Hbl_prv.h
	// ...
    SRTASKSEQUENCE_APPL_INIT_HBL_IO_INITIALIZE,
    SRTASKSEQUENCE_APPL_INIT_SRPRODUCTMGR,

    // Here there must be defined steps for all SRChecks__Initialize available (at least one per check)
	// ...
	SRTASKSEQUENCE_APPL_INIT_SRDUMMYCHECK,
    SRTASKSEQUENCE_APPL_INIT_SRCHECKS_DONE,

    // Here there must be defined steps for all SRActions__Initialize  available (at least one per action)
	// ...
	SRTASKSEQUENCE_APPL_INIT_SRDUMMYACTION,
	SRTASKSEQUENCE_APPL_INIT_SRACTIONS_DONE,

	SRTASKSEQUENCE_APPL_INIT_SRPRODUCTMGR_DONE,
#if (SYSTEM_HMI_HANDLING == ENABLED)
	SRTASKSEQUENCE_APPL_INIT_HBLHMI_INITIALIZE,
#endif
	SRTASKSEQUENCE_APPL_INIT_HBL_DONE
};

//! The definition of the steps that make up the SRTASKSEQUENCE_APPL_SAFETY_THREAD sequence.
//! List the steps in the order that they occur for the SRTASKSEQUENCE_APPL_SAFETY_THREAD sequence.
static const SRTASKSEQUENCE_APPL_STEPS_TYPE SRTaskSequence_Appl_Steps[] =
{
    SRTASKSEQUENCE_APPL_SETTINGFILE_EXECUTED,
    SRTASKSEQUENCE_APPL_CLASSA_EXECUTED,
    SRTASKSEQUENCE_APPL_SRHBL_START,

    // Here there must be defined steps for all Hbl SR-Pilots, SR-Loads, SR-LLIs, SR-GIs
    // Order is defined in HBL__PREPROCESSLOADS() macro, where handler calls should be.
	// ...
    SRTASKSEQUENCE_APPL_SRHBL_CFG_CHECKS,
    SRTASKSEQUENCE_APPL_SRPRODUCTMGR_START,
    // Here there must be defined steps for all SRChecks available (at least two each check: Enter and Exit!)
	// ...
	SRTASKSEQUENCE_APPL_SRDUMMYCHECK_ENTER,
	SRTASKSEQUENCE_APPL_SRDUMMYCHECK_EXIT,

	SRTASKSEQUENCE_APPL_SRCHECKS_DONE,
    // Here there must be defined steps for all SRActions available (at least two each action: Enter and Exit!)
	// ...
	SRTASKSEQUENCE_APPL_SRDUMMYACTION_ENTER,
	SRTASKSEQUENCE_APPL_SRDUMMYACTION_EXIT,

    SRTASKSEQUENCE_APPL_SRACTIONS_DONE,

    SRTASKSEQUENCE_APPL_SRPRODUCTMGR_DONE,
    SRTASKSEQUENCE_APPL_SRHBL_LOADS_PROCESSED,
	#if (SR_HMI_EVENTS_FEATURE == ENABLED)
		SRTASKSEQUENCE_APPL_SRHBLHMI_START,
	#endif
	SRTASKSEQUENCE_APPL_MICRO_SRRAMTEST_ENTER,
	SRTASKSEQUENCE_APPL_MICRO_SRRAMTEST_EXIT,
	SRTASKSEQUENCE_APPL_MICRO_SRFLASHTEST_ENTER,
	SRTASKSEQUENCE_APPL_MICRO_SRFLASHTEST_EXIT,
	SRTASKSEQUENCE_APPL_COMPLETED
};

static const SRTASKSEQUENCE_TICK_STEPS_TYPE SRTaskSequence_Tick_Steps[] =
{
    SRTASKSEQUENCE_TICK_SRTASKSEQUENCE,
    // Add here steps (if any) if there are Class B calls in SRMAIN__BEFORE_1MS() macro
    SRTASKSEQUENCE_TICK_SRTIMERREFERENCE,
    SRTASKSEQUENCE_TICK_SRTASKMONITOR,
    // This must be used in SRCLOCKMONITOR_1MS_FLOW() macro
    SRTASKSEQUENCE_TICK_SRCLOCKMONITOR,
    SRTASKSEQUENCE_TICK_COMMUNICATION,
    SRTASKSEQUENCE_TICK_TASKS,
    SRTASKSEQUENCE_TICK_TASK_STACK_SIGNATURE
};


static const SRTASKSEQUENCE_MAIN_STEPS_TYPE SRTaskSequence_Main_Steps[] =
{
    SRTASKSEQUENCE_MAIN_MODE_HANDLER,
    SRTASKSEQUENCE_MAIN_TASKS,
    // Add here steps (if any) if there are Class B calls in SRMainRunning_Tasks tasks list

    SRTASKSEQUENCE_MAIN_WATCHDOG,
    SRTASKSEQUENCE_MAIN_ISRMONITOR,
};

//! An array with the number of steps for each of the defined sequences.
static const uint8 SRTaskSequcence_NumSteps[] =
{
	sizeof(SRTaskSequence_Init_Steps),
	sizeof(SRTaskSequence_Tick_Steps),
	sizeof(SRTaskSequence_Main_Steps),
	sizeof(SRTaskSequence_Appl_Init_Steps),
	sizeof(SRTaskSequence_Appl_Steps)
};

//! An array with pointers to the steps for each of the defined sequences.
static const uint8 *const SRTaskSequence_All_Steps[] =
{
    (void*)SRTaskSequence_Init_Steps,
	(void*)SRTaskSequence_Tick_Steps,
	(void*)SRTaskSequence_Main_Steps,
    (void*)SRTaskSequence_Appl_Init_Steps,
    (void*)SRTaskSequence_Appl_Steps
};


#endif // SRTASKSEQUENCE_PRV_H_

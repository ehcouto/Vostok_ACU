/**
 *  @file       SRTaskMonitor_prv.h
 *
 *  @brief      Private (prv) configuration file for SRTaskMonitor module
 *
 *  @defgroup   CLASS_B
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRTASKMONITOR_PRV_H_
#define SRTASKMONITOR_PRV_H_

//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================

// This option can be used to disable reset on Task monitor - for debug purposes only, default is DISABLED
// If set to ENABLED, SRTaskMonitor just doesn't go in an Error state,
// but after logging exception it's restored to a normal state.
//
// WARNING: it cannot be set ENABLED in production code; if not used, it's better to not include this module.

// #define SRTASKMONITOR_SKIP_RESET			ENABLED

//---------------------------------------------------------------------------------------------------------------------

// This option allows that there could be no tasks to monitor
// Default is DISABLED: it requires that at least one task enabled, otherwise it will go in an Error state.
// It would be better to leave it undefined or define it DISABLED in production code, unless this feature is required.

// #define SRTASKMONITOR_ALLOW_NO_TASK		DISABLED

//---------------------------------------------------------------------------------------------------------------------

// This option allows a temporary task monitoring suspend by an external criterion - e.g. RTOS task scheduling disabled.
// If set to 0 disables task monitor suspend feature, any other value is the timeout to set and waiting that
// task monitoring is resumed.

#define SRTASKMONITOR_SUSPEND_TIME				100

// This option set maximum number of suspend action can be done until next system reset
// If suspend requests are done more than this value, an exception will be raised
// This option is valid only if task monitor suspend feature  is enabled
// Default and minimum value accepted is 1

#define SRTASKMONITOR_SUSPEND_MAX_ATTEMPTS		5

//---------------------------------------------------------------------------------------------------------------------

// This option enables some statistics useful to check proper execution of Task monitor and related tasks monitored.
// It would be better to leave it undefined or define it DISABLED in production code, unless this feature is required.

#define SRTASKMONITOR_STATS_FEATURE				ENABLED

//---------------------------------------------------------------------------------------------------------------------

// Configuration of counter references to be used in SRTaskMonitor module
// SRTaskMonitor_Tasks_Counter_Ref must be configured as an array of SRTASKMONITOR_TASK_COUNTER_REF elements
// Number of element has to match SRTASKMONITOR_NUM_TASKS.
// Units are in System tick unit time - usually it's 1ms

const SRTASKMONITOR_TASK_COUNTER_REF SRTaskMonitor_Tasks_Counter_Ref[SRTASKMONITOR_NUM_TASKS] =
{
    {   7,      13,     100 },          //  SRTASKMONITOR_MAIN_TASK
    {   95,     105,    200 },          //  SRTASKMONITOR_APPLICATION_TASK
    {   3,      7,       10 },          //  SRTASKMONITOR_ASYNC_TASK
    {   1,      9,       10 }           //  SRTASKMONITOR_COMMUNICATION_TASK
};

// Configuration of counter references to be used in SRTaskMonitor module for Low Speed mode
// SRTaskMonitor_Tasks_Low_Speed_Counter_Ref must be configured as an array of SRTASKMONITOR_TASK_COUNTER_REF elements
// Number of element has to match SRTASKMONITOR_NUM_TASKS.
// Units are in System tick low speed unit time - usually it's 25ms
// Low Speed mode enables only Main Task; please fill other tasks with 0 values.

const SRTASKMONITOR_TASK_COUNTER_REF SRTaskMonitor_Tasks_Low_Speed_Counter_Ref[SRTASKMONITOR_NUM_TASKS] =
{
    {   1,      3,       10 },          //  SRTASKMONITOR_MAIN_TASK
    {   0,      0,        0 },          //  SRTASKMONITOR_APPLICATION_TASK
    {   0,      0,        0 },          //  SRTASKMONITOR_ASYNC_TASK
    {   0,      0,        0 }           //  SRTASKMONITOR_COMMUNICATION_TASK
};

#endif // SRTASKMONITOR_PRV_H_

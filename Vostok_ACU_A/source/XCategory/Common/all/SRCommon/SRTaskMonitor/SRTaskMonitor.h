/**
 *  @file       SRTaskMonitor.h
 *  @defgroup   CLASS_B
 *  @brief      Periodic task execution monitor module header
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
#ifndef SRTASKMONITOR_H_
#define SRTASKMONITOR_H_

#include "SRTaskMonitor_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

PACKED typedef struct
{
	uint16		counter_min;
	uint16		counter_max;
	uint16		counter_max_init;
} SRTASKMONITOR_TASK_COUNTER_REF;

PACKED typedef struct
{
	uint16        start_value;
	uint16        min_value;
	uint16        max_value;
	uint16        max_elapsed;
} SRTASKMONITOR_STATS_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SRTaskMonitor__Initialize(void);
void SRTaskMonitor__Handler(void);
void SRTaskMonitor__EnableMonitor(SRTASKMONITOR_TASKLIST_TYPE task_id);
void SRTaskMonitor__Enter(SRTASKMONITOR_TASKLIST_TYPE task_id);
void SRTaskMonitor__Exit(SRTASKMONITOR_TASKLIST_TYPE task_id);
void SRTaskMonitor__Suspend(void);
void SRTaskMonitor__Resume(void);
BOOL_TYPE SRTaskMonitor__IsError(void);

SRTASKMONITOR_STATS_TYPE * SRTaskMonitor__GetStatisticsData(uint8 task_id);

#endif // SOURCE_XCATEGORY_COMMON_COMMON_SRTASKMONITOR_SRTASKMONITOR_H_



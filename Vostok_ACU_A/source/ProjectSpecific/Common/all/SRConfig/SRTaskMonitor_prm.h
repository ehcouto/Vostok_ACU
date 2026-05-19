/**
 *  @file       SRTaskMonitor_prm.h
 *
 *  @brief      Public (prm) configuration for SRTaskMonitor module
 *
 *  @defgroup	CLASS_B
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
#ifndef SRTASKMONITOR_PRM_H_
#define SRTASKMONITOR_PRM_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

typedef enum SRTASKMONITOR_TASKLIST_ENUM
{
	SRTASKMONITOR_MAIN_TASK,
	SRTASKMONITOR_APPLICATION_TASK,
	SRTASKMONITOR_ASYNCH_TASK,
	SRTASKMONITOR_COMMUNICATION_TASK,
	SRTASKMONITOR_NUM_TASKS				// NOTE This must be present as last item
} SRTASKMONITOR_TASKLIST_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif // SRTASKMONITOR_PRM_H_

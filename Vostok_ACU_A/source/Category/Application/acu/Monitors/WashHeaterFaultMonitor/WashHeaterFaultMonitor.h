/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef WASHHEATERFAULTMONITOR_H_
#define WASHHEATERFAULTMONITOR_H_

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "WashHeaterFaultMonitor_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//! Bit indexes in Heater_Disabled_Bitmap
typedef enum BIT_INDEXES_IN_HEATER_DISABLED_BITMAP_ENUM
{
	HEATER_DISABLED_BY_HEATER_FAILED						= 0,
	HEATER_DISABLED_BY_L_RELAY_STUCK_OPEN					= 1,
	HEATER_DISABLED_BY_N_RELAY_STUCK_OPEN					= 2,
	HEATER_DISABLED_BY_HEATER_OPEN							= 3,
	HEATER_DISABLED_BY_L_RELAY_STUCK_CLOSED_AND_HEATER_OPEN	= 4,

	HEATER_DISABLED_MAX_INDEX				= 8,
} BIT_INDEXES_IN_HEATER_DISABLED_BITMAP_TYPE;

typedef PACKED struct HEATER_CURRENT_SENSING_PARAMS_STRUCT
{
	uint16 Heater_Off_Threshold;
	uint16 Heater_On_Lower_Threshold;
	uint16 Heater_On_Upper_Threshold;
	uint8 Stabilization_Time;
	uint8 Unused;
	uint16 AC_Line_Lower_Threshold;
} HEATER_CURRENT_SENSING_PARAMS_TYPE;


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void WashHeaterFaultMonitor__Initialize(void);
void WashHeaterFaultMonitor__Execution(void);
void WashHeaterFaultMonitor__CycleStarted(void);
BOOL_TYPE WashHeaterFaultMonitor__GetMonitorPrecondition(void);
uint8 WashHeaterFaultMonitor__GetStabilizationCounter(void);
void WashHeaterFaultMonitor__RestartStabilizationCounter(void);
uint8 WashHeaterFaultMonitor__GetHeaterConsistency(void);
uint8 WashHeaterFaultMonitor__GetDisabledBitmap(void);
void WashHeaterFaultMonitor__SetHeaterDisabledBitmapBit(BIT_INDEXES_IN_HEATER_DISABLED_BITMAP_TYPE bit_index);

#endif // WASHHEATERFAULTMONITOR_H_



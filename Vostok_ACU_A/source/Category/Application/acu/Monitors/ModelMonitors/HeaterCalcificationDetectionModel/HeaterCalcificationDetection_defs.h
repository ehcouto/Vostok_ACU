/**
* @file: HeaterCalcificationDetection_defs.h
*
* @model: 'HeaterCalcificationDetectionMaster'.
*
* @brief The intention of this algorithm is to prevent component damage due to the thick layer of calcium buildup.
* Detection is based on the characteristic of the current flowing through the component during a heating phase.
*
* @modelversion:  0.120
*
* @coderversion:  9.3 (R2020a) 18-Nov-2019
*
* C/C++ source code generated on : Wed Sep  1 19:37:57 2021
*
* @copyright  Copyright 2018. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
*/

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//-------------------
#ifndef RTW_HEADER_HeaterCalcificationDetection_defs_h_
#define RTW_HEADER_HeaterCalcificationDetection_defs_h_
#include "rtwtypes.h"

//--------------------
typedef PACKED struct HEATER_CALCIFICATION_PARAMS_STRUCT
{
	uint16 Allowed_Clicks_Num;
	uint8 Allowed_Clicks_To_Reduce;
	uint8 WHS_Max_Increase : 4;
	uint8 Unused_B3_b4_to_b7 : 4;
	uint8 Periodic_HT_Threshold : 7;
	uint8 Unused_B4_b7 : 1;
	uint8 Unused_B5;
	uint16 Periodic_HT_Min_Temperature;
	uint16 Heater_Nominal_Power;
	uint16 Heater_Nominal_Current;
	uint16 Heater_Current_i[6];
} HEATER_CALCIFICATION_PARAMS_TYPE;

typedef PACKED struct HEATER_CALCIFICATION_LEVEL_DIP_PARAMS_STRUCT
{
	uint16 Temperature_for_Heater_Power[6];
	uint8 Calcification_Reduction_Threshold : 6;
	uint8 Descale_Icon_Mode : 2;
	uint8 Descale_Alarm_Threshold;
	uint8 Lockout_Threshold;
	uint8 Unused_B15;
} HEATER_CALCIFICATION_LEVEL_DIP_PARAMS_TYPE;

//! Structure for HCD_Bitmap
typedef union HCD_BITMAP_UNION
{
	uint8 All;

	struct {
		uint8 Periodic_HT_Required	:1;
		uint8 Unused_b1_to_b7		:7;
	};
} HCD_BITMAP_TYPE;

//! Structure for HCD Data shared with HMI
typedef struct HCD_HMI_DATA_STRUCT
{
	union {
		uint8 All;

		struct {
			uint8 Descale_Icon_Mode		:2;
			uint8 Show_Descale			:1;
			uint8 Lockout				:1;
			uint8 Show_Countdown		:1;
			uint8 Increase_WHS			:1;
			uint8 Unused_b6_to_b7		:2;
		};
	} Flags;

	uint8 Cycles_Remaining;
} HCD_HMI_DATA_TYPE;

#endif                     /* RTW_HEADER_HeaterCalcificationDetection_defs_h_ */

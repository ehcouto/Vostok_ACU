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
#ifndef SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_PROCESSES_FILL_FILL_H_
#define SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_PROCESSES_FILL_FILL_H_

#include "Process.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//! The number of times that the Fill__Process function will be called every second while running.
#define FILL_CALLS_PER_SECOND 10

//! The available Fill process variants
typedef enum FILL_OPTION_ENUM
{
	FILL_OPTION_NORMAL_FILL,		//!< F1=0, F0=0
	FILL_OPTION_CLASS_A_FILL,		//!< F1=0, F0=1
	FILL_OPTION_TANK_FILL,			//!< F1=1, F0=0
	FILL_OPTION_TANK_DRAIN,			//!< F1=1, F0=1

	FILL_OPTION_COUNT
} FILL_OPTION_TYPE;

//! Bit indexes in Class_A_Fill_Bitmap
typedef enum BIT_INDEXES_CLASS_A_FILL_BITMAP_ENUM
{
	TANK_STATE_CLEAN						= 0,
	FORCE_TANK_DRAIN						= 1,
	DRAIN_CONFIRMED							= 2,
	SENSED_DRAIN_REQUESTED_BY_TANK_DRAIN	= 3,
	SENSED_DRAIN_REQUESTED_BY_FILL			= 4
} BIT_INDEXES_CLASS_A_FILL_BITMAP_TYPE;

//! Tank State values
typedef enum TANK_STATE_ENUM
{
	TANK_STATE_FAIL				= 0,
	TANK_STATE_PASSED			= 1,
	TANK_STATE_UNKNOWN			= 2,
	TANK_STATE_RUNNING			= 3
} TANK_STATE_TYPE;

//! Return values for Fill__ClassAFillCore() function
typedef enum FILL_CORE_RET_ENUM
{
	FILL_CORE_NO_STATE_CHANGE = 0,
	FILL_CORE_NO_WATER_DETECTED,
	FILL_CORE_WATER_CHECK_NEEDED,
	FILL_CORE_WATER_FILL_OVER
} FILL_CORE_RET_TYPE;

//! Process specific setting file parameters structure for Class A Fill and Tank Fill processes
typedef PACKED struct CLASS_A_FILL_PROCESS_PARAMETER_STRUCT
{
    uint8 Liters_Requested;
    BOOL_TYPE Partial_Fill			:1;
    uint8 Unused_B1_b1_7			:7;
    uint8 Modifier_Table_Index;
    uint8 Unused_B3;
} CLASS_A_FILL_PROCESS_PARAMETER_TYPE;

//! Machine Cofigurations setting file parameters structure for Class A Fill, Tank Fill and Tank Drain processes
typedef PACKED struct CLASS_A_FILL_PARAMETER_STRUCT
{
	uint8 Liters_Offset;
	uint8 Liters_In_Tub_Max;
	uint8 Liters_For_Tank_Full;
	uint8 Liters_For_Tank_Empty;
	uint8 Liters_For_Tank_Empty_Max;
	uint8 Liters_For_WI_Empty_Max;
	uint8 Liters_For_WI_Full_Min;
	uint8 Liters_Unchecked_Max;
	uint8 Liters_In_Tank_Max;
	uint8 Time_For_Tank_Drain;
	uint8 Tank_Fill_Delay_Time;
	uint8 Seconds_To_Calibrate_Min;
	uint8 Flow_Meter_Check_Time;
	BOOL_TYPE Test_Flow_Meter_Fault	:1;
	uint8 Unused_B13_b1_7			:7;
	uint8 Fill_Check_Water_Subcycle_Priority;
	uint8 Unused_B15;
	uint16 Flow_Rate_Nominal;
	uint16 Fill_Flow_Rate_Min;
	uint16 Flow_Rate_Calib_Min;
	uint16 Flow_Rate_Max;
} CLASS_A_FILL_PARAMETER_TYPE;

//! Volatile data structure for Class A Fill and Tank Fill processes
typedef struct CLASS_A_FILL_VOLATILE_DATA_STRUCT
{
	CLASS_A_FILL_PROCESS_PARAMETER_TYPE *Class_A_Fill_Process_Parameters;
	CLASS_A_FILL_PARAMETER_TYPE *Class_A_Fill_Parameters;
	uint16 No_Flow_Meter_Pulses_Timer;
	uint16 Flow_Rate_Current;
	uint16 Fill_Time_Max;
	uint16 Fill_Time_Calibrated;
	BOOL_TYPE Subcycle_Executed;
	BOOL_TYPE Flow_Rate_Calibration_Enabled;
	PASS_FAIL_TYPE Process_Parameters_Loading;
} CLASS_A_FILL_VOLATILE_DATA_TYPE;

//! Non volatile data structure for Class A Fill and Tank Fill processes
typedef struct CLASS_A_FILL_NV_DATA_STRUCT
{
	uint16 Liters_Requested;
	uint16 Fill_Timer;
	BOOL_TYPE H2O_Pause_Executed;	// Saved in NV Data to manage the use case of power fail during Cycle Resume subcycle following PAUSE state
} CLASS_A_FILL_NV_DATA_TYPE;

extern uint8 Class_A_Fill_Bitmap;
extern uint16  Liters_In_Water_Tank;
extern uint16 Liters_In_Tub_Before_Fill;
extern uint16 Liters_In_Tank_Before_Fill;
extern uint16 Liters_Unchecked;
extern TANK_STATE_TYPE Tank_State_Fill;
extern TANK_STATE_TYPE Tank_State_Drain;
extern uint16 Liters_For_Tank_Empty;
extern uint16 Liters_For_Tank_Full;


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void Fill__Initialize(void);
PROCESS_RESULT_TYPE Fill__Process(PROCESS_CONTROL_TYPE* control);
void Fill__SetBitmapBit(uint8 bit_index);
void Fill__ClearBitmapBit(uint8 bit_index);
BOOL_TYPE Fill__GetBitmapBit(uint8 bit_index);
BOOL_TYPE Fill__ProjectUsesClassAFill(void);
void Fill__ClearH2OAlarm(void);
void Fill__ClassAFillCoreInit(CLASS_A_FILL_VOLATILE_DATA_TYPE *fill_data, CLASS_A_FILL_NV_DATA_TYPE *fill_nv_data);
FILL_CORE_RET_TYPE Fill__ClassAFillCore(CLASS_A_FILL_VOLATILE_DATA_TYPE *fill_data, CLASS_A_FILL_NV_DATA_TYPE *fill_nv_data, FILL_OPTION_TYPE fill_option);
#endif // SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_PROCESSES_FILL_FILL_H_



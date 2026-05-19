/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: DyerAuxVar.h 1.1 2015/07/30 10:33:57EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/30 10:33:57EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef DISHAUXVAR_H_
#define DISHAUXVAR_H_

#include "C_Extensions.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//! Number of significant bytes for Model Options in GESE versions prior than 0.1.90.0.
#define MODEL_OPTIONS_NUM_VAR_OLD_GESE_VERSIONS	5

//! Length of the Array with Model Options variables to be used in the Cycles.
#define MODEL_OPTIONS_NUM_VAR					10

//! Length of the Array with Dish general purpose variables to be used in the Cycles.
#define DISH_GEN_PURP_NUM_VAR		2

//! Max diverter positions.
#define DIVERTER_POSITION_MAX	6

//! The maximum number of samples supported by DishAuxVar_AverageWithRejection() function.
#define AVEARAGE_WITH_REJ_MAX_SUPPORTED_SAMPLES_NUM 	12U

typedef enum _OWI_CALIB_STATE_ENUM
{
    OWICALIBRATION_STATE_IDLE 		= 0,
	OWICALIBRATION_STATE_STABILIZE	= 1,
	OWICALIBRATION_STATE_DUTYCYCLE	= 2,
	OWICALIBRATION_STATE_EXIT		= 3
}OWI_CALIB_STATE_TYPE;

typedef enum
{
	OWI_STATUS_NEEDS_CALIBRATION,
	OWI_STATUS_CALIBRATED,
	OWI_STATUS_NON_SENSOR_MODE
}OWI_STATUS_TYPE;

typedef enum LIFE_TEST_INPUT_STATUS_ENUM
{
	INPUT_STATE_LOW = 0,
	INPUT_STATE_HIGH = 1
}LIFE_TEST_INPUT_STATUS_TYPE;

//! Bit indexes in Generic_Bitmap
typedef enum BIT_INDEXES_IN_GENERIC_BITMAP_ENUM
{
	FST_RUN_AFTER_COLD_RESET			= 0,
	DOS_DRIVEN,
} BIT_INDEXES_IN_GENERIC_BITMAP_TYPE;

typedef struct Diverter_Position_Struct
{
	uint8 Position;
	BOOL_TYPE Is_Stable;
} Diverter_Position_Type;

extern OWI_STATUS_TYPE OWI_Status;
extern uint16 OWI_Calibration_Drive_Duty_Cycle;
extern OWI_CALIB_STATE_TYPE OWICalibration_State;
extern PASS_FAIL_TYPE OWICalibration_Status;
extern float32 OWICalibration_Turbidity_Voltage_Reading;
extern sint16 Wash_Probe_Value;
extern uint8 Diverter_Position;
extern uint8 Sanitization_Result;
extern uint16 Pulse_Counter_Ref;
extern uint16 Water_Counter;
extern uint8 Water_Present;
extern uint16  Liters_In_Tub;
extern uint16  Liters_Current_Fill;
extern BOOL_TYPE  H2O_Alarm;
extern uint8  Previous_HHP_Door_Check;
extern uint16 OWI_Analog;
extern uint32 Positional_Loads_Status_Bitmap;
extern uint16 Loads_Bitmap_Status_0_15;
extern uint16 Loads_Bitmap_Status_16_31;
extern uint16 Soil_Measure_Recent;
extern uint16 Soil_Measure_2;
extern uint16 Soil_Measure_3;
extern uint16 Soil_Measure_4;
extern uint16 Soil_Measure_5;
extern uint16 Soil_Measure_6;
extern uint16 Soil_Measure_Difference_Recent;
extern uint16 Soil_Measure_Difference_2;
extern uint16 Soil_Measure_Difference_3;
extern uint8 Soil_Level_1;
extern uint8 Soil_Level_2;
extern PASS_FAIL_TYPE Sensed_Drain_Status;
extern uint8 Life_Test_Mode_Shortcut_Status;
extern uint8 Fill_Fault;
extern uint8 Phase_Repetitions;
extern uint8 Generic_Bitmap;
extern uint8 Do_Factory_Cycle;
extern uint8 Model_Options[MODEL_OPTIONS_NUM_VAR];
extern uint16 Dish_General_Purpose_Variables[DISH_GEN_PURP_NUM_VAR];
extern PASS_FAIL_TYPE Partial_Drain_Status;
extern uint8 Water_Tank_State;
extern uint8  HHP_Door_Check;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void DishAuxVar__Initialize(void);
void DishAuxVar__Handler(void);
void DishAuxVar__UpdatePulseRef(void);
LIFE_TEST_INPUT_STATUS_TYPE DishAuxVar__GetLifetestInputStatus(void);
uint8 DishAuxVar__GetDoFactoryCycle(void);
void DishAuxVar__SetDoFactoryCycle(BOOL_TYPE value);
uint16 DishAuxVar__GetACLine(void);
void DishAuxVar__SetTimedFillRunning(BOOL_TYPE value);
void DishAuxVar__InitializePulseRef(void);
Diverter_Position_Type DishAuxVar__GetDiverterPosition(void);
void DishAuxVar__ResetWaterCounterReference(void);
void DishAuxVar__SetLitersCurrentRegeneration(uint16 value);
uint16 DishAuxVar__GetLitersCurrentRegeneration(void);
PASS_FAIL_TYPE DishAuxVar__GetSensedDrainStatus(void);
void DishAuxVar__SetSensedDrainStatus(PASS_FAIL_TYPE value);
BOOL_TYPE DishAuxVar__IsDrainPumpOn(void);
void DishAuxVar__SetGenericBitmapBit(uint8 bit_index, ON_OFF_TYPE value);
BOOL_TYPE DishAuxVar__AreFlowMeterPulsesPresent(void);
void DishAuxVar__InitializeFlowMeterPulsesPresent(void);
PASS_FAIL_TYPE DishAuxVar_AverageWithRejection(uint16 *input_samples_array, uint8 array_size, uint8 num_samples_available, uint8 num_samples_to_keep, uint16 *calculated_average);
void DishAuxVar__SetWaterLoadedSinceLastRegen(uint16 value);
uint16 DishAuxVar__GetWaterLoadedSinceLastRegen(void);

#endif // DISHAUXVAR_H_



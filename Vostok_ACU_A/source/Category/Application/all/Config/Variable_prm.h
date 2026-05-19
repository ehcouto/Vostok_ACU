/**
 *  @file
 *
 *  @brief      Public parameter file for the Variable module.
 *
 *  @details    Refer to the C source file for more detailed information.
 *
 *  @copyright  Copyright 2015-2016 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef VARIABLE_PRM_H_
#define VARIABLE_PRM_H_

// -- Declare compiler directives --
#if (SYSTEM_ACU_HANDLING == ENABLED)
    #include "DishAuxVar.h"
    #include "StabilityControl.h"
#endif

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
/**
 * Set this macro to ENABLED access the variables using Key (from KVP).
 * Default: DISABLED.
 */
#define VARIABLES_HAVE_KVP_ID		DISABLED

//! Define here the length of the Array with General purpose variables to be used on the Cycles.
#define VARIABLE_NUMBER_OF_GENERAL_PURPOSE 	6

#ifndef MODEL_OPTIONS_NUM_VAR
    #define MODEL_OPTIONS_NUM_VAR 10
#endif
#ifndef DISH_GEN_PURP_NUM_VAR
    #define DISH_GEN_PURP_NUM_VAR 2
#endif
#ifndef SC_SUBC_EXEC_COUNTERS_NUM
    #define SC_SUBC_EXEC_COUNTERS_NUM 5
#endif


#if (SYSTEM_ACU_HANDLING != ENABLED)
typedef enum DOOR_STATUS_ENUM
{
    DOOR_STATUS_OPEN = 0,
    DOOR_STATUS_CLOSE,
	DOOR_STATUS_UNKNOWN
}DOOR_STATUS_TYPE;

typedef enum
{
	OWI_STATUS_NEEDS_CALIBRATION,
	OWI_STATUS_CALIBRATED,
	OWI_STATUS_NON_SENSOR_MODE
}OWI_STATUS_TYPE;

typedef enum _OWI_CALIB_STATE_ENUM
{
    OWICALIBRATION_STATE_IDLE 		= 0,
	OWICALIBRATION_STATE_STABILIZE	= 1,
	OWICALIBRATION_STATE_DUTYCYCLE	= 2,
	OWICALIBRATION_STATE_EXIT		= 3
}OWI_CALIB_STATE_TYPE;

//! Possible values of the variable HeaterShadowing_Status
typedef enum HEATER_SHADOWING_STATUS_ENUM
{
	ALGORITHM_NOT_ACTIVE = 0,
	ALGORITHM_ON_HOLD,
	HEATER_OFF,
	HEATER_NOT_SHADOWED,
	HEATER_SHADOWED,
	UNKNOWN_DIVERTER_POSITION,
	HEATER_SHADOWED_RISK

} HEATER_SHADOWING_STATUS_TYPE;

//! States of Salt_Alarm_Led
typedef enum SALT_ALARM_LED_STATE_ENUM
{
    SALT_ALARM_LED_STATE_OFF 		= 0,
	SALT_ALARM_LED_STATE_SOLID_ON 	= 1,
	SALT_ALARM_LED_STATE_BLINK 		= 2
} SALT_ALARM_LED_STATE_TYPE;

//! Possible water state detected by VWI
typedef enum
{
    WS_NOT_VALID = 0,
    WS_NO_WATER,
    WS_UNSTABLE_NO_WATER,
    WS_UNSTABLE_WATER,
    WS_WATER
} VWI_WATER_SENSE_TYPE;

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

//! Possible VOLTAGERANGE_STATE
typedef enum VOLTAGERANGE_STATE_ENUM
{
	VOLTAGERANGE_STATE_UNDEFINED,
	VOLTAGERANGE_STATE_GOODRANGE,
	VOLTAGERANGE_STATE_LOWVOLTAGE,
	VOLTAGERANGE_STATE_HIGHVOLTAGE,
} VOLTAGERANGE_STATE_TYPE;

//! Tank State values
typedef enum TANK_STATE_ENUM
{
	TANK_STATE_FAIL				= 0,
	TANK_STATE_PASSED			= 1,
	TANK_STATE_UNKNOWN			= 2,
	TANK_STATE_RUNNING			= 3
} TANK_STATE_TYPE;

//! Structure for Cartridge Data shared with HMI
typedef struct CARTRIDGE_HMI_DATA
{
	union
	{
		uint8 All;
		struct
		{
			uint8  Low_Level  				:1; 	//!
			uint8  Empty_Level				:1;		//!
			uint8  Cartridge_Inserted   	:1;		//!
			uint8  Cartridge_Door_Closed	:1;		//!
			uint8  Valid_Data				:1;		//!
			uint8  Unused					:3;
		};
	} Flags;

	uint8 Percentage_level;
} CARTRIDGE_HMI_DATA_TYPE;


#endif
/**
 * Insert here the IDs for the variables you'll install on the PRV file.
 * The definitions here are examples to match the variables on the PRV file.
 *
 * The GESE database must be updated to reflect any changes made here.
 * Comment this out if you don't have any variable to add.
 */
#define ACU_VARIABLE_ID_USERS_ENUM      \
    VARIABLE_WATER_TEMPERATURE              			= 16, \
	VARIABLE_DIVERTER_POSITION              			= 17, \
    VARIABLE_SANITIZATION_RESULT            			= 18, \
    VARIABLE_PULSE_COUNTER_REF              			= 19, \
    VARIABLE_WATER_COUNTER                  			= 20, \
    VARIABLE_DOOR_STATE                     			= 21, \
	VARIABLE_WATER_PRESENT                 				= 22, \
	VARIABLE_LITERS_IN_TUB                  			= 23, \
	VARIABLE_LITERS_CURRENT_FILL            			= 24, \
	VARIABLE_H2O_ALARM                      			= 25, \
	VARIABLE_PREVIOUS_HHP_DOOR_CHECK        			= 26, \
    VARIABLE_OWI_ANALOG                     			= 27, \
    VARIABLE_POSITIONAL_LOADS_BITMAP        			= 28, \
    VARIABLE_RINSE_AID                      			= 29, \
	VARIABLE_SOIL_MEASURE_RECENT            			= 30, \
	VARIABLE_SOIL_MEASURE_2                 			= 31, \
	VARIABLE_SOIL_MEASURE_3                 			= 32, \
	VARIABLE_SOIL_MEASURE_4                 			= 33, \
	VARIABLE_SOIL_MEASURE_5                 			= 34, \
	VARIABLE_SOIL_MEASURE_6                 			= 35, \
	VARIABLE_SOIL_MEASURE_DIFFERNCE_RECENT  			= 36, \
	VARIABLE_SOIL_MEASURE_DIFFERNCE_2       			= 37, \
	VARIABLE_SOIL_MEASURE_DIFFERNCE_3       			= 38, \
	VARIABLE_ESTIMATED_REMAINING_TIME       			= 39, \
	VARIABLE_SUPERVISOR_DELAY_TIME_REMAINING  			= 40, \
	VARIABLE_OWI_CALIBRATION_DRIVE_DUTY_CYCLE 			= 41, \
	VARIABLE_OWI_CALIBRATION_STATUS         			= 42, \
	VARIABLE_SENSED_DRAIN_CONFIRMED           			= 43, \
	VARIABLE_SENSED_DRAIN_STATUS            			= 44, \
	SOIL_LEVEL_1                            			= 45, \
	SOIL_LEVEL_2                            			= 46, \
	LIFE_TEST_MODE_SHORTCUT_STATUS          			= 47, \
	FILL_FAULT                              			= 48, \
	VARIABLE_OWICALIBRATION_STATE           			= 49, \
	VARIABLE_OWICALIBRATION_STATUS          			= 50, \
	VARIABLE_OWICALIBRATION_TURBIDITY_VOLTAGE_READING 	= 51, \
	VARIABLE_PHASE_REPETITIONS              			= 52, \
	RINSEAID_DISPENSER_USE_COUNTER          			= 53, \
	VARIABLE_GENERIC_BITMAP   	       					= 54, \
	VARIABLE_FAULT_STATUS_BITMAP_0_15       			= 55, \
	VARIABLE_FAULT_STATUS_BITMAP_16_31      			= 56, \
	VARIABLE_LOADS_BITMAP_16_31             			= 57, \
    VARIABLE_LOADS_BITMAP_0_15              			= 58, \
	VARIABLE_DO_FACTORY_CYCLE               			= 59, \
	VARIABLE_FAULT_STATUS_BITMAP_32_47      			= 60, \
	VARIABLE_HEATER_SHADOWING_STATUS        			= 61, \
	VARIABLE_SALT_ALARM_LED                 			= 62, \
	VARIABLE_REG_AND_SALT_BITMAP            			= 63, \
	VARIABLE_VWI_STATUS									= 64, \
	VARIABLE_MODEL_OPTIONS								= 65, \
	VARIABLE_FAULT_STATUS_BITMAP_48_63      			= 66, \
	VARIABLE_DISH_GENERAL_PURPOSE						= 67, \
	VARIABLE_SC_SUBCYCLE_EXECUTION_COUNTERS				= 68, \
	VARIABLE_PARTIAL_DRAIN_STATUS						= 69, \
	VARIABLE_FAULT_STATUS_BITMAP_64_79      			= 70, \
	VARIABLE_HCD_HEATER_CLICKS							= 71, \
	VARIABLE_HCD_CALCIFICATION_LEVEL					= 72, \
	VARIABLE_HCD_BITMAP									= 73, \
	VARIABLE_HCD_HMI_DATA_FLAGS							= 74, \
	VARIABLE_HCD_HMI_DATA_CYCLES_REMAINING 				= 75, \
	VARIABLE_LITERS_IN_WATER_TANK 						= 76, \
	VARIABLE_WATER_TANK_STATE 							= 77, \
	VARIABLE_VOLTAGE_RANGE_STATUS						= 78, \
	VARIABLE_OWI_VRO_DUTYCYCLE_STABLE					= 79, \
	VARIABLE_OWI_VRO_DUTYCYCLE_COMP						= 80, \
	VARIABLE_OWI_VRO_CALIBRATION_STATUS					= 81, \
	VARIABLE_STABILITY_CONTROL_FLAGS					= 82, \
	VARIABLE_CLASS_A_FILL_BITMAP						= 83, \
	VARIABLE_TANK_STATE_DRAIN							= 84, \
	VARIABLE_TANK_STATE_FILL							= 85, \
	VARIABLE_LITERS_IN_TUB_BEFORE_FILL					= 86, \
	VARIABLE_LITERS_IN_TANK_BEFORE_FILL					= 87, \
	VARIABLE_LITERS_UNCHECKED							= 88, \
	VARIABLE_LITERS_FOR_TANK_FULL						= 89, \
	VARIABLE_LITERS_FOR_TANK_EMPTY						= 90, \
	VARIABLE_POWER_LOSS_OCCURRING						= 91, \
	VARIABLE_CARTRIDGE_HMI_DATA_FLAGS					= 92, \
	VARIABLE_CARTRIDGE_HMI_DATA_PERCENTAGE_LEVEL 		= 93, \
	VARIABLE_ENZIME_TAG_OPERATING_TEMPERATURE			= 94, \
	VARIABLE_BLEACH_TAG_OPERATING_TEMPERATURE			= 95, \
	VARIABLE_RINSEAID_TAG_OPERATING_TEMPERATURE			= 96

/**
 * This define should match the size of the list defined in Variable_prv.h.
 */
/*
#define  HMI_VARIABLE_ID_USERS_ENUM     \
    HMI_VARIABLE_U8.,					\
    HMI_VARIABLE_U16,                   \
    HMI_VARIABLE_U32,                   \
    HMI_VARIABLE_U8_ARRAY,              \
    HMI_VARIABLE_S8,                    \
    HMI_VARIABLE_S16,                   \
    HMI_VARIABLE_S32*/
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif // VARIABLE_PRM_H_

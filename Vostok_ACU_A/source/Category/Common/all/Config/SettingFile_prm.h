/**
 *  @file
 *
 *  @brief      Public parameters for the SettingFile module.
 *
 *  @copyright  Copyright 2015-$Date: 2015/06/11 16:54:27EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SETTINGFILE_PRM_H_
#define SETTINGFILE_PRM_H_

#include "C_Extensions.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#define DSPL_MOTOR_FAULT                ((uint8) 0x03 )

// -- Public Constant Declarations --

//! Setting file pointers to the category-specific process data.
#define SETTINGFILE_APPLICATION_SPECIFIC_SF_PTR	\
	SF_PTR_ACU_PROCESS_HEAT					= 0x0016, \
    SF_PTR_ACU_PROCESS_ALTERNATE_CTRL		= 0x0017, \
	SF_PTR_ACU_PROCESS_FILL					= 0x0018, \
	SF_PTR_ACU_PROCESS_PUMP_CTRL			= 0x0019, \
	SF_PTR_ACU_PROCESS_REGENERATE			= 0x001A, \
	SF_PTR_ACU_PROCESS_RINSE_AID_DISPENSE	= 0x001B, \
	SF_PTR_ACU_PROCESS_SENSED_DRAIN			= 0x001C, \
	SF_PTR_ACU_PROCESS_OWI_VRO_READ			= 0x001D, \
    SF_PTR_ACU_MOIST_PREV_PARAMETERS		= 0x0028, \
    SF_PTR_ACU_STABILITY_CONTROL_DATA		= 0x0029, \
	SF_PTR_UI_RECIPE_DATA					= 0x0142, \
    SF_PTR_MCU_ALGORITHMS					= 0x0204, \
	



//! Product Specific displacements ENUM.
typedef enum SETTINGFILE_PRODUCT_SPECIFIC_DISPLACEMENT_ENUM
{
    // Displacement numbers for SF_PTR_ACU_PRODUCT_CONFIGURATION_DATA
    DISPL_HEATER_DEACTIVATION_CONDITIONS_PARAM   = 0,
	DSPL_LOADS_STATES_MNG						 = 1,
	DSPL_CYCLE_RESUME_SUBCYCLE_PRIORITY 		 = 2,
	DSPL_HOTHIGHPOT_PARAMS					 	 = 3,
	DSPL_LIGHT_IN_THE_TUBE_PARAMS				 = 4,
	DSPL_FILL_VALVE_FAULT_PARAMS				 = 5,
	DSPL_HEATER_CURRENT_SENSING_PARAMS			 = 6,
	DSPL_WASH_MOTOR_TYPE						 = 7,
	DSPL_DRAIN_MOTOR_TYPE						 = 8,
	DSPL_MODEL_OPTIONS							 = 9,
	DSPL_DIAG_VOLTAGE_PARAMS                     = 10,
	DSPL_VOLTAGE_RANGE_PARAMS					 = 11,
	DSPL_HVDC_BUS_PARAMS						 = 12,
	DSPL_TRIDGE_SYSTEM_PARAMS					 = 13,

    // Displacement numbers for SF_PTR_ACU_ALGORITHM_PARAMETERS
    DISPL_SANITIZATION  		= 0,
	DISPL_FILL 					= 1,
	DISPL_SENSED_FILL 			= 2,
    DISPL_VWI_WASH_PUMP         = 3,
    DISPL_SENSED_DRAIN  		= 4,
	DISPL_OWI_CALIBRATION  		= 5,
	DISPL_STABILITY_CONTROL 	= 6,
	DISPL_AIRLOCK 				= 7,
	DISPL_PLUGGING 				= 8,
	DSPL_HEATER_SHADOWING		= 9,
	DSPL_DOOR_OPENING_SYSTEM	= 10,
	DSPL_RINSE_AID				= 11,
	DSPL_WATER_SOFTENER			= 12,
	DSPL_RINSE_AID_DISPENSE		= 13,
	DISPL_VWI_DRAIN_PUMP		= 14,
	DISPL_PARTIAL_DRAIN  		= 15,
	DISPL_HEATER_CALCIFICATION  = 16,
	DISPL_SPEED_REDUCTION		= 17,
	DISPL_VWI_ZONE_COMPENSATION_COMMON			= 18,
	DISPL_VWI_ZONE_COMPENSATION_MULTIPLIERS		= 19,
	DISPL_HEATER_CALCIFICATION_LEVEL_DIP_PARAMS = 20,
	DISPL_OWI_VRO_COMMON_PARAMS			= 21,
	DISPL_OWI_VRO_SETPOINT_1_PARAMS		= 22,
	DISPL_OWI_VRO_SETPOINT_2_PARAMS		= 23,
	DISPL_OWI_VRO_SETPOINT_3_PARAMS		= 24,
	DISPL_OWI_VRO_LINEARIZATION_TABLE	= 25,
	DISPL_CLASS_A_FILL 					= 26,

	// Displacement numbers for SF_PTR_MCU_CLASS_A_MCI
	SF_DISPL_MOTOR_PARAMS_GLOBAL_WASH_SYSTEM   = 0,

	 // Displacement numbers for SF_PTR_MCU_CLASS_A_MCI
    SF_DISPL_MOTOR_PARAMS_BLAC_WASH              = 0x01,
    SF_DISPL_MOTOR_PARAMS_BLAC_DRAIN             = 0x02,
    SF_DISPL_INVERTER_PARAMS                     = 0x03,
    SF_DISPL_POWERMODULE_MGR_PARAMS              = 0x04,
    SF_DISPL_MOTOR_PARAMS_BLAC_FW_WASH           = 0x05,
    SF_DISPL_MOTOR_PARAMS_BLAC_FW_DRAIN          = 0x06,
    SF_DISPL_MCI_WASH_PRM                        = 0x07,
    SF_DISPL_MCI_DRAIN_PRM                       = 0x08,
    SF_DISPL_MCI_FVT_BLAC_WASH_PRM               = 0x09,
    SF_DISPL_MCI_FVT_BLAC_DRAIN_PRM              = 0x0A,

    // Displacement numbers for SF_PTR_MCU_CLASS_B_MCI
    SF_DISPL_MOTOR_SR_PARAMS_BLAC_WASH           = 0x01,
    SF_DISPL_MOTOR_SR_PARAMS_BLAC_DRAIN          = 0x02,

    // Displacement numbers for SF_PTR_MCU_ALGORITHMS
    DISPL_MCU_VWI_WASH                           = 0x00,
    DISPL_MCU_VWI_DRAIN                          = 0x01,
    DISPL_MCU_DEBLOCKING_WASH                    = 0x02,
    DISPL_MCU_DEBLOCKING_DRAIN                   = 0x03,

    // Displacement numbers for SF_PTR_MCU_CLASSB_ALGORITHMS
    SF_DISPL_MOTOR_SR_PARAMS_CURRENT_UNBALANCE   = 0x00,
} SETTINGFILE_PRODUCT_SPECIFIC_DISPLACEMENT_TYPE;

//! Setting file pointers to the category-specific process data.
#define SETTINGFILE_APPLICATION_SPECIFIC_SR_SF_PTR \
        SF_PTR_MCU_CLASSB_ALGORITHMS            = 0x0212,   \
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif // SETTINGFILE_PRM_H_

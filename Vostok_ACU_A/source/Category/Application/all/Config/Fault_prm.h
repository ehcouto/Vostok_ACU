/**
 * @file
 * @brief       Public parameters to the Fault module.
 *
 * @details     Refer to the C source file for more detailed information.
 *
 * @copyright  Copyright 2016 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef FAULT_PRM_H_
#define FAULT_PRM_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public Type Declarations --

//! Unique application-wide identifiers for faults related to a single platform.
//! This list may include faults that are used on a platform, but may not be used on every project.
//! The GESE database must be updated to reflect any changes made here.
typedef enum FAULT_ID_ENUM
{
	//! The first fault (value of 0) should always be here.
	//! This value is used in various places to indicate that there is no active fault.
    FAULT_ID_NONE,

	// Create a list of platform-specific faults here.
    FAULT_ID_OVERFILL,
	FAULT_ID_ZEROCROSS,
	FAULT_ID_NO_WATER_FILL_VALVE_PROBLEM,
	FAULT_ID_NO_WATER,
	FAULT_ID_FLOW_METER,
	FAULT_ID_VERY_LOW_WATER_OR_SUDS,
	FAULT_ID_FILL_VALVE_TRIAC_SHORTED,
	FAULT_ID_FILL_VALVE_STUCK_ON,
	FAULT_ID_THERMISTOR_SHORTED,
	FAULT_ID_THERMISTOR_OPEN,
	FAULT_ID_FLOW_METER_FINDLAY_FACTORY,
    FAULT_ID_OVERFILL_FINDLAY_FACTORY,
	FAULT_ID_NOT_DRAINING,
	FAULT_ID_SINGLE_SPEED_DRAIN_MOTOR,
	FAULT_ID_PILOT_RELAY_STUCK_CLOSED,
	FAULT_ID_DRAIN_MOTOR_TRIAC_SHORTED,
	FAULT_ID_DIVERTER_TRIAC_SHORTED,
	FAULT_ID_WATER_TANK_VALVE_TRIAC_SHORTED,
	FAULT_ID_HEATER_NOT_HEATING_WATER,
	FAULT_ID_DISPENSER_FAILURE,
	FAULT_ID_VENT_WAX_MOTOR_FAILURE,
	FAULT_ID_FAN_MOTOR_FAILURE,
	FAULT_ID_DIVERTER_CANNOT_FIND_POSITIONS,
	FAULT_ID_DIVERTER_LEAK,
	FAULT_ID_DOOR_SWITCH_STUCK_CLOSED,
	FAULT_ID_DOOR_SWITCH_STUCK_OPEN,
	FAULT_ID_RELAY_HEATER_N_STUCK_OPEN,
	FAULT_ID_RELAY_HEATER_L_STUCK_OPEN_OR_HEATER_OPEN,
	FAULT_ID_RELAY_HEATER_N_STUCK_CLOSED,
	FAULT_ID_RELAY_HEATER_L_STUCK_CLOSED,
	FAULT_ID_HEATER_FAILED,
	FAULT_ID_OWI_CALIBRATION_FAILURE,
	FAULT_ID_KEY_STUCK_CLOSED,
	FAULT_ID_PILOT_RELAY_STUCK_OPEN,
	FAULT_ID_SINGLE_SPEED_WASH_MOTOR,
	FAULT_ID_LITT_FAILURE,
	FAULT_ID_VS_WASH_MOTOR_MCI_LOCKED_ROTOR,
	FAULT_ID_VS_WASH_MOTOR_MCI_MOTOR_PHASE_LOST,
	FAULT_ID_VS_WASH_MOTOR_MCI_MOTOR_OVERLOAD,
	FAULT_ID_VS_WASH_MOTOR_MCI_SOFTWARE_OVERCURRENT,
	FAULT_ID_VS_WASH_MOTOR_MCI_SPEED_CHECK,
	FAULT_ID_MCU_WM_MCI_GENERIC_ERROR,
	FAULT_ID_MCU_WM_MCI_HARDWARE_OVERCURRENT,
	FAULT_ID_MCU_WM_MCI_OVER_VOLTAGE,
	FAULT_ID_MCU_WM_MCI_UNDER_VOLTAGE,
	FAULT_ID_MCU_WM_MCI_DRIVER_FAILURE,
	FAULT_ID_MCU_WM_MCI_INVERTER_OVER_TEMPERATURE,
	FAULT_ID_MCU_NO_MCU_COMMUNICATION,
	FAULT_ID_VS_DRAIN_MOTOR_MCI_LOCKED_ROTOR,
	FAULT_ID_VS_DRAIN_MOTOR_MCI_MOTOR_PHASE_LOST,
	FAULT_ID_VS_DRAIN_MOTOR_MCI_MOTOR_OVERLOAD,
	FAULT_ID_VS_DRAIN_MOTOR_MCI_SOFTWARE_OVERCURRENT,
	FAULT_ID_VS_DRAIN_MOTOR_MCI_SPEED_CHECK,
	FAULT_ID_MCU_DM_MCI_GENERIC_ERROR,
	FAULT_ID_MCU_DM_MCI_HARDWARE_OVERCURRENT,
	FAULT_ID_MCU_DM_MCI_OVER_VOLTAGE,
	FAULT_ID_MCU_DM_MCI_UNDER_VOLTAGE,
	FAULT_ID_MCU_DM_MCI_DRIVER_FAILURE,
	FAULT_ID_MCU_DM_MCI_INVERTER_OVER_TEMPERATURE,
	FAULT_ID_REGENERATION_VALVE_FAILURE,
	FAULT_ID_RIF_FILTER_PLUGGED,
	FAULT_ID_NOT_DRAINING_VWI_INCONSISTENCY,
	FAULT_ID_DIVERTER_LEAK_FACTORY,
	FAULT_ID_NOT_DRAINING_NO_CYCLE_END,
	FAULT_ID_RELAY_HEATER_L_STUCK_OPEN,
	FAULT_ID_HEATER_OPEN,
	FAULT_ID_RELAY_HEATER_L_STUCK_CLOSED_AND_HEATER_OPEN,
	FAULT_ID_FILL_VALVE_TRIAC_OPEN,
	FAULT_ID_DRAIN_MOTOR_TRIAC_OPEN,
	FAULT_ID_DISPENSER_TRIAC_OPEN,
	FAULT_ID_REGENERATION_VALVE_TRIAC_OPEN,
	FAULT_ID_VENT_VALVE_TRIAC_OPEN,
	FAULT_ID_DIVERTER_TRIAC_OPEN,
	FAULT_ID_WATER_TANK_VALVE_NOT_FUNCTIONING,
	FAULT_ID_WATER_TANK_VALVE_STUCK_OPEN,
	FAULT_ID_MODEL_NUMBER_MISMATCH,
	FAULT_ID_MODEL_NUMBER_MISSING,
	FAULT_ID_REGULATION_REJECTED,
	FAULT_ID_DOS_FAILURE,
	//! The last fault is used to keep track of the total number of faults defined for a given platform.
	FAULT_ID_END
} FAULT_ID_TYPE;


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


#endif      /* FAULT_PRM_H_ */

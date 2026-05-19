/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
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


//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "FaultStatusMonitor.h"

// -- Other Modules --
#include "ApplianceMgr.h"
#include "DishAuxVar.h"
#include "Fault.h"
#include "Micro.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
uint16 Fault_Status_Bitmap_0_15;
uint16 Fault_Status_Bitmap_16_31;
uint16 Fault_Status_Bitmap_32_47;
uint16 Fault_Status_Bitmap_48_63;
uint16 Fault_Status_Bitmap_64_79;

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
static const FAULT_ID_TYPE Bitfield_0_15_To_ID_Fault_Table[16] =
{
		FAULT_ID_PILOT_RELAY_STUCK_CLOSED,
		FAULT_ID_FILL_VALVE_TRIAC_SHORTED,
		FAULT_ID_DRAIN_MOTOR_TRIAC_SHORTED,
		FAULT_ID_DIVERTER_TRIAC_SHORTED,
		FAULT_ID_WATER_TANK_VALVE_TRIAC_SHORTED,
		FAULT_ID_ZEROCROSS,
		FAULT_ID_THERMISTOR_OPEN,
		FAULT_ID_THERMISTOR_SHORTED,
		FAULT_ID_HEATER_NOT_HEATING_WATER,
		FAULT_ID_NO_WATER,
		FAULT_ID_NO_WATER_FILL_VALVE_PROBLEM,
		FAULT_ID_VERY_LOW_WATER_OR_SUDS,
		FAULT_ID_OVERFILL,
		FAULT_ID_FILL_VALVE_STUCK_ON,
		FAULT_ID_FLOW_METER,
		FAULT_ID_FLOW_METER_FINDLAY_FACTORY
};

static const FAULT_ID_TYPE Bitfield_16_31_To_ID_Fault_Table[16] =
{
		FAULT_ID_OVERFILL_FINDLAY_FACTORY,
		FAULT_ID_NOT_DRAINING,
		FAULT_ID_SINGLE_SPEED_DRAIN_MOTOR,
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
		FAULT_ID_OWI_CALIBRATION_FAILURE
};

static const FAULT_ID_TYPE Bitfield_32_47_To_ID_Fault_Table[16] =
{
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
		FAULT_ID_MCU_NO_MCU_COMMUNICATION
};

static const FAULT_ID_TYPE Bitfield_48_63_To_ID_Fault_Table[16] =
{
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
		FAULT_ID_NOT_DRAINING_NO_CYCLE_END
};

static const FAULT_ID_TYPE Bitfield_64_79_To_ID_Fault_Table[16] =
{
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
		FAULT_ID_NONE
};

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//----------------------------------------------------------------------------
/**
 *  @brief      FaultStatusMonitor__Initialize
 *  @details    Function used to initialize the Fault Status monitor.
 *
 */
void FaultStatusMonitor__Initialize(void)
{
	if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
	{
		Fault_Status_Bitmap_0_15 = 0;
		Fault_Status_Bitmap_16_31 = 0;
		Fault_Status_Bitmap_32_47 = 0;
		Fault_Status_Bitmap_48_63 = 0;
		Fault_Status_Bitmap_64_79 = 0;
	}
}

//----------------------------------------------------------------------------
/**
 *  @brief      FaultStatusMonitor__Execution
 *  @details    Function used to perform the Fault Status monitor.
 *
 */
void FaultStatusMonitor__Execution(void)
{
	uint8 bit_index;

	for (bit_index = 0U; bit_index < (uint8)16; bit_index++)
    {
		if ((uint8)Fault__IsInFaultState(Bitfield_0_15_To_ID_Fault_Table[bit_index]) == (uint8)TRUE)
		{
			BIT_SET16(Fault_Status_Bitmap_0_15, bit_index);
		}
		else
		{
			BIT_CLR16(Fault_Status_Bitmap_0_15, bit_index);
		}

		if ((uint8)Fault__IsInFaultState(Bitfield_16_31_To_ID_Fault_Table[bit_index]) == (uint8)TRUE)
		{
			BIT_SET16(Fault_Status_Bitmap_16_31, bit_index);
		}
		else
		{
			BIT_CLR16(Fault_Status_Bitmap_16_31, bit_index);
		}

		if ((uint8)Fault__IsInFaultState(Bitfield_32_47_To_ID_Fault_Table[bit_index]) == (uint8)TRUE)
		{
			BIT_SET16(Fault_Status_Bitmap_32_47, bit_index);
		}
		else
		{
			BIT_CLR16(Fault_Status_Bitmap_32_47, bit_index);
		}

		if ((uint8)Fault__IsInFaultState(Bitfield_48_63_To_ID_Fault_Table[bit_index]) == (uint8)TRUE)
		{
			BIT_SET16(Fault_Status_Bitmap_48_63, bit_index);
		}
		else
		{
			BIT_CLR16(Fault_Status_Bitmap_48_63, bit_index);
		}

		if ((uint8)Fault__IsInFaultState(Bitfield_64_79_To_ID_Fault_Table[bit_index]) == (uint8)TRUE)
		{
			BIT_SET16(Fault_Status_Bitmap_64_79, bit_index);
		}
		else
		{
			BIT_CLR16(Fault_Status_Bitmap_64_79, bit_index);
		}
}

	/* Done here since it is the last Post Monitor */
	if ((uint8)ApplianceMgr__IsStable() == (uint8)TRUE)
	{
		DishAuxVar__SetGenericBitmapBit(FST_RUN_AFTER_COLD_RESET, OFF);
	}
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


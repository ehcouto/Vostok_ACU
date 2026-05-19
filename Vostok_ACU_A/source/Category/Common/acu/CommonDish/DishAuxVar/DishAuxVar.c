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
 *  $Header: DyerAuxVar.c 1.5 2015/07/31 12:06:19EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/31 12:06:19EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "DishAuxVar.h"

// -- Other Modules --
#include "Fill.h"
#include "Fault.h"
#include "Hbl.h"
#include <math.h>
#if (API221_CLIENT_FEATURE == ENABLED)
#include "McuGetDataMonitor.h"
#endif /* (API221_CLIENT_FEATURE == ENABLED) */
#include "Micro.h"
#include "NVData.h"
#if (NEW_SENSED_DRAIN_FEATURES == ENABLED)
#include "SensedDrainProcess.h"
#endif /* (NEW_SENSED_DRAIN_FEATURES == ENABLED) */
#include "SettingFile.h"
#include "StatisticsDish.h"
#include <string.h>
#include "TriacVoltageMonitor.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
sint16 Wash_Probe_Value;				// to be linked to the variable module
uint16 Pulse_Counter_Ref;
uint16 Water_Counter;
static uint16 Liters_Since_Reference_Reset_Old; // only for manage the difference
uint16 OWI_Analog;
uint32 Positional_Loads_Status_Bitmap;
uint16 Loads_Bitmap_Status_0_15;
uint16 Loads_Bitmap_Status_16_31;
uint8 Do_Factory_Cycle;
static uint16 AC_Line_Value;

#pragma location = "_appliance_segment"
uint8  HHP_Door_Check;
#pragma location = "_appliance_segment"
uint8  Previous_HHP_Door_Check;

#pragma location = "_appliance_segment"
uint16 Liters_Current_Fill;
#pragma location = "_appliance_segment"
BOOL_TYPE  H2O_Alarm;
#pragma location = "_appliance_segment"
uint8  Water_Present;
#pragma location = "_appliance_segment"
uint8  Sanitization_Result;
#pragma location = "_appliance_segment"
uint16 OWI_Calibration_Drive_Duty_Cycle;
#pragma location = "_appliance_segment"
OWI_CALIB_STATE_TYPE OWICalibration_State;
#pragma location = "_appliance_segment"
PASS_FAIL_TYPE OWICalibration_Status;
#pragma location = "_appliance_segment"
float32 OWICalibration_Turbidity_Voltage_Reading;
#pragma location = "_appliance_segment"
uint8 Soil_Level_1;
#pragma location = "_appliance_segment"
uint8 Soil_Level_2;
#pragma location = "_appliance_segment"
uint8 Fill_Fault;
#pragma location = "_appliance_segment"
static uint16 Liters_Current_Regeneration;

#if (NEW_SENSED_DRAIN_FEATURES != ENABLED)
uint32 Sensed_Drain_On_Time;
#endif /* (NEW_SENSED_DRAIN_FEATURES != ENABLED) */

PASS_FAIL_TYPE Sensed_Drain_Status;

#pragma location = "_appliance_segment"
PASS_FAIL_TYPE Partial_Drain_Status;

#pragma location = "_appliance_segment"
OWI_STATUS_TYPE OWI_Status;
#pragma location = "_appliance_segment"
uint8  Diverter_Position;

uint8 Life_Test_Mode_Shortcut_Status;
static BOOL_TYPE Timed_Fill_Running;
static BOOL_TYPE Diverter_Is_Stable;
static uint8 Water_Counter_GI_Sequence_ID_Old;
uint8 Model_Options[MODEL_OPTIONS_NUM_VAR];
uint8 Generic_Bitmap;

static BOOL_TYPE Flow_Meter_Pulses_Present;


#pragma location = "_appliance_segment"
uint16  Liters_In_Tub;
#pragma location = "_appliance_segment"
uint16 Soil_Measure_Recent;
#pragma location = "_appliance_segment"
uint16 Soil_Measure_2;
#pragma location = "_appliance_segment"
uint16 Soil_Measure_3;
#pragma location = "_appliance_segment"
uint16 Soil_Measure_4;
#pragma location = "_appliance_segment"
uint16 Soil_Measure_5;
#pragma location = "_appliance_segment"
uint16 Soil_Measure_6;
#pragma location = "_appliance_segment"
uint16 Soil_Measure_Difference_Recent;
#pragma location = "_appliance_segment"
uint16 Soil_Measure_Difference_2;
#pragma location = "_appliance_segment"
uint16 Soil_Measure_Difference_3;
#pragma location = "_appliance_segment"
uint8 Phase_Repetitions;
#pragma location = "_appliance_segment"
uint16 Dish_General_Purpose_Variables[DISH_GEN_PURP_NUM_VAR];
#pragma location = "_appliance_segment"
uint8 Water_Tank_State;
#pragma location = "_appliance_segment"
static uint16 Water_Loaded_Since_Last_Regen; //! Amount of water loaded with fill valve since the last 'Last step' regeneration was executed


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//! Position of loads in custom bitmaps
enum MONITORED_LOADS_POSITION_ENUM
{
	WASH_PUMP_POS = 0,
	DRAIN_PUMP_POS,
	WASHING_HEATER_POS,
	DRYING_HEATER_POS,
	FILL_VALVE_POS,
	REGENERATION_VALVE_POS,
	LOWER_SPRAY_ARM_MOTOR_POS,
	WATER_TANK_POS,
	VENT_POS,
	DISPENSER_POS,
	OZONO_POS,
	SALT_INPUT_ACTIVATION_POS,
	POR_TIME_CAPTURE_CIRCUIT_POS,
	DIVERTER_POS,
	AUTO_KILL_POS,
	PILOT_RELAY_POS,
	RELAY_ENABLE_POS,
	DRY_FAN_POS,
	LITT_POS,
	LOTF_POS,
	OWI_POS,
	MCU_ENABLE_POS,
	DOOR_OPENING_SYSTEM_POS,
	LOWER_TANK_VALVE_POS,
	UPPER_TANK_VALVE_POS,
	RINSE_AID_INPUT_ACTIVATION_POS,
	WIN_12V_ACTIVATION_POS,
	BULK_DISPENSER_1_POS,
	BULK_DISPENSER_2_POS,
	BULK_DISPENSER_3_POS,

	MAX_NUM_LOADS_MONITORED = 32
};

static const HBL_LOAD_ENUM_TYPE LoadID_Monitored[] =
{
	HBL_LOAD_WASH_PUMP,
	HBL_LOAD_DRAIN_PUMP,
	HBL_LOAD_WASHING_HEATER,
	HBL_LOAD_DRYING_HEATER,
	HBL_LOAD_FILL_VALVE,
	HBL_LOAD_REGENERATION_VALVE,
	HBL_LOAD_LOWER_SPRAY_ARM_MOTOR,
	HBL_LOAD_WATER_TANK,
	HBL_LOAD_VENT,
	HBL_LOAD_DISPENSER,
	HBL_LOAD_OZONO,
	HBL_LOAD_SALT_INPUT_ACTIVATION,
	HBL_POR_TIME_CAPTURE_CIRCUIT,
	HBL_LOAD_DIVERTER,
	HBL_LOAD_AUTO_KILL,
	HBL_LOAD_PILOT_RELAY,
	HBL_LOAD_RELAY_ENABLE,
	HBL_LOAD_DRY_FAN,
	HBL_LOAD_LITT,
	HBL_LOAD_LOTF,
	HBL_LOAD_OWI,
	HBL_LOAD_MCU_ENABLE,
	HBL_LOAD_DOOR_OPENING_SYSTEM,
	HBL_LOAD_LOWER_TANK_VALVE,
	HBL_LOAD_UPPER_TANK_VALVE,
	HBL_LOAD_RINSE_AID_INPUT_ACTIVATION,
	HBL_WIN_12V_ACTIVATION,
	HBL_BULK_DISPENSER_1,
	HBL_BULK_DISPENSER_2,
	HBL_BULK_DISPENSER_3
};

// Load_Position_In_Custom_Bitmap[i] = bit position (0-31) in (Loads_Bitmap_Status_0_15 + Loads_Bitmap_Status_16_31) of load whose
// index in Setting File Load Configuration is i.
// If i > number of loads in Setting File Load Configuration, Load_Position_In_Custom_Bitmap[i] = HBL_INDEX_INVALID.
// If load whose index in Setting File Load Configuration is i is not monitored in custom bitmap, Load_Position_In_Custom_Bitmap[i] = HBL_INDEX_INVALID.
//
static uint8 Load_Position_In_Custom_Bitmap[MAX_NUM_LOADS_MONITORED];

//! TRUE if the diverter has been driven after the last cold reset
static BOOL_TYPE Diverter_Fst_Driving_Done;

// Array of delta values between each sample and the mean value (static and not local array to avoid using a large amount of stack)
static float32 Delta_Array[AVEARAGE_WITH_REJ_MAX_SUPPORTED_SAMPLES_NUM];

// Array of sorted index as result of Average with Rejection (static and not local array to avoid using a large amount of stack)
static uint8 Sorted_Index_Array[AVEARAGE_WITH_REJ_MAX_SUPPORTED_SAMPLES_NUM];

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module DyerAuxVar and its variables
 *
 */
void DishAuxVar__Initialize(void)
{
	uint8 num_loads;
	uint8 index;
	HBL_LOAD_ENUM_TYPE load_ID_local;
	uint8 index_custom_monitoring;
    SETTINGFILE_LOADER_TYPE loader;

	// Initialize also with warm reset, since WaterCounterGI is reset to 0 after a warm reset
	Liters_Since_Reference_Reset_Old = 0;
	// Initialize also with warm reset, since Hbl_GI_Sequence is reset to 0 after a warm reset
	Water_Counter_GI_Sequence_ID_Old = 0;

	if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
	{
		if (NVData__GetLastRestoreStatus()->Bit.Appliance_Data_Recovered != (NVDATA_RESTORE_TYPE)NVDATA_RESTORE_OKAY)
		{
			// Non-volatile memory corrupted - initialize
			Liters_In_Tub = 0;
			Liters_Current_Fill = 0;
			Liters_Current_Regeneration = 0;
			Soil_Measure_Recent = 0;
			Soil_Measure_2 = 0;
			Soil_Measure_3 = 0;
			Soil_Measure_4 = 0;
			Soil_Measure_5 = 0;
			Soil_Measure_6 = 0;
			Soil_Measure_Difference_Recent = 0;
			Soil_Measure_Difference_2 = 0;
			Soil_Measure_Difference_3 = 0;
			Phase_Repetitions = 0;
			H2O_Alarm = FALSE;
			memset(&Dish_General_Purpose_Variables[0], 0, sizeof(Dish_General_Purpose_Variables));
			Water_Present = 0;
			Diverter_Position = 0;
			Water_Tank_State = 0;
			Water_Loaded_Since_Last_Regen = 0;
		}

		Wash_Probe_Value = 0;
		AC_Line_Value = 0;

		Water_Counter = 0;

		Positional_Loads_Status_Bitmap = 0;
		Loads_Bitmap_Status_0_15 = 0;
		Loads_Bitmap_Status_16_31 = 0;

		Timed_Fill_Running = FALSE;

		Diverter_Is_Stable = TRUE;
		Diverter_Fst_Driving_Done = FALSE;

		Generic_Bitmap = 0;
		BIT_SET(Generic_Bitmap, FST_RUN_AFTER_COLD_RESET);

		memset(&Load_Position_In_Custom_Bitmap[0], HBL_INDEX_INVALID, sizeof(Load_Position_In_Custom_Bitmap));

		num_loads = Hbl__GetNumLoads();

		if (num_loads > (uint8)MAX_NUM_LOADS_MONITORED)
		{
			num_loads = MAX_NUM_LOADS_MONITORED;
		}

		for (index = 0; index < num_loads; index++)
		{
			load_ID_local = Hbl__GetLoadIDByIndex(index);

			for (index_custom_monitoring = 0; index_custom_monitoring < ELEMENTS_IN_ARRAY(LoadID_Monitored); index_custom_monitoring++)
			{
				if (LoadID_Monitored[index_custom_monitoring] == load_ID_local)
				{
					Load_Position_In_Custom_Bitmap[index] = index_custom_monitoring;
					break;
				}
			}
		}

		// Model Options variable initialization
		memset(&Model_Options[0], 0, sizeof(Model_Options));
		if (SettingFile__BasicLoader(SF_PTR_ACU_PRODUCT_CONFIGURATION_DATA, DSPL_MODEL_OPTIONS, &loader) == PASS)
		{
			for (index = 0; index < ELEMENTS_IN_ARRAY(Model_Options); index++)
			{
				if ((index < (uint8)MODEL_OPTIONS_NUM_VAR_OLD_GESE_VERSIONS) || (loader.Length >= sizeof(Model_Options)))
				{
					Model_Options[index] = loader.Data[index];
				}
			}
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the Pulse_Counter_Ref variable.
 *
 *  			It has to be called before the first execution of WaterCounterGI__AsynchProcess()
 *
 */
void DishAuxVar__InitializePulseRef(void)
{
	Pulse_Counter_Ref = 0;
}

void DishAuxVar__Handler(void)
{
	uint8 gi_index;
	uint8 gi_sequence_id;
	uint8 *pos_res;
	HBL_LOAD_STATUS_TYPE pos_val;
	uint8 current_requested_state;
	uint8 *current_state_ptr;
    uint8 index;
    uint8 num_loads;
    void* load_data;
	uint32 custom_load_bitmap;
	uint16 liters_since_reference_reset;


	/* Management storing Loads values */
    num_loads = Hbl__GetNumLoads();

    if (num_loads > (uint8)MAX_NUM_LOADS_MONITORED)
    {
       num_loads = MAX_NUM_LOADS_MONITORED;
    }

    custom_load_bitmap = 0;
	for (index = 0; index < num_loads; index++)
	{
		load_data = Hbl__GetProcessedLoad(index);

		if ((load_data != NULL) && (((uint8*)(load_data))[0] != (uint8)OFF))
		{
			// Load Data has been retrieved and its status is different from 0;

			// Loads_Value_Bitmap =  Loads_Value_Bitmap | (uint16)(1 << index);
			BIT_SET(Positional_Loads_Status_Bitmap, index);

			if (Load_Position_In_Custom_Bitmap[index] < (uint8)32) // It implies Load_Position_In_Custom_Bitmap[index] != HBL_INDEX_INVALID
			{
				BIT_SET(custom_load_bitmap, Load_Position_In_Custom_Bitmap[index]);
			}
		}
		else
		{
			// Load Data hasn't been retrieved or its status is 0

			// Loads_Value_Bitmap =  Loads_Value_Bitmap & ~(uint16)(1 << index);
			BIT_CLR(Positional_Loads_Status_Bitmap, index);
		}
	}

	Loads_Bitmap_Status_0_15  = LOWORD(custom_load_bitmap);
	Loads_Bitmap_Status_16_31 = HIWORD(custom_load_bitmap);


	// Diverter_Position
	current_state_ptr = (uint8*)Hbl__GetRequestedLoad(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DIVERTER));
	if (current_state_ptr != NULL)
	{
		current_requested_state = *current_state_ptr;

		pos_res = (uint8 *) Hbl__GetLoadStatusByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DIVERTER));
		if (pos_res != NULL)
		{
			pos_val.Byte = *pos_res;

			if ((pos_val.Load.Status == (HBL_LOAD_STATUS_ENUM_TYPE)HBL_LOAD_STATUS_TARGET_COMPLETED) &&
				(current_requested_state == pos_val.Load.Cur_Reg))
			{
				if ((uint8)Diverter_Fst_Driving_Done == (uint8)TRUE)
				{
					// Diverter_Position is updated with the HBL value only if the diverter has been driven after the last cold reset,
					// otherwise it keeps the value saved in NV Data.
					Diverter_Position = pos_val.Load.Cur_Reg;
					Diverter_Is_Stable = TRUE;
				}
			}
			else
			{
				Diverter_Fst_Driving_Done = TRUE;

				Diverter_Is_Stable = FALSE;
			}
		}
	}


#if (NEW_SENSED_DRAIN_FEATURES == ENABLED)
	// Reset SensedDrain_DrainConfirmed based on Fill Valve status and Water Tank status
	if ((BIT_TEST(custom_load_bitmap, FILL_VALVE_POS) != 0U) || (BIT_TEST(custom_load_bitmap, WATER_TANK_POS) != 0U))
	{
		SensedDrain__SetDrainConfirmed(FALSE);
	}
#endif /* (NEW_SENSED_DRAIN_FEATURES == ENABLED) */


	// Wash_Probe_Value
	gi_index = Hbl__GetGIIndexByGIID(HBL_GI_WASHING_PROBE);
	gi_sequence_id = Hbl__GetGISequenceIDByIndex(gi_index);
	if (gi_sequence_id > (uint8)0)
	{
		Wash_Probe_Value = (sint16)Hbl__GetGIDataConvertedByIndex(gi_index);
	}

	// AC Line
	gi_index = Hbl__GetGIIndexByGIID(HBL_GI_AC_LINE);
	gi_sequence_id = Hbl__GetGISequenceIDByIndex(gi_index);

	if (gi_sequence_id > (uint8)0)
	{
		AC_Line_Value = (uint16)Hbl__GetGIDataConvertedByIndex(gi_index);
	}

	// Water Counter
	gi_index = Hbl__GetGIIndexByGIID(HBL_GI_WATER_COUNTER);
	gi_sequence_id = Hbl__GetGISequenceIDByIndex(gi_index);

	if (gi_sequence_id != Water_Counter_GI_Sequence_ID_Old)
	{
		// New reading available
		liters_since_reference_reset = (uint16) Hbl__GetGIDataConvertedByIndex(gi_index);

		if ((((Fault__IsInFaultState(FAULT_ID_FLOW_METER) == FALSE) && (Timed_Fill_Running == FALSE)) || ((uint8)Fill__ProjectUsesClassAFill() == (uint8)TRUE)) &&
			(((DishAuxVar__IsDrainPumpOn() == FALSE) && (BIT_TEST(custom_load_bitmap, REGENERATION_VALVE_POS) == 0U)) ||
				(BIT_TEST(custom_load_bitmap, FILL_VALVE_POS) != 0U)) )
		{
			// In Normal (non Class-A) Fill, when Flow Meter is in fault OR Timed Fill is running, these variables are updated inside the
			// Fill Process.
			// If Drain Pump OR Regeneration Valve are On AND Fill Valve is Off, we assume that any pulse detected is due to a magnetic
			// interference between the drain motor (or the regeneration valve) and the flow meter sensor, so it has not to be counted.

			// delta_pulses works also when liters_since_reference_reset overflows
			uint16 delta_pulses = liters_since_reference_reset - Liters_Since_Reference_Reset_Old;

			Liters_Current_Fill +=  delta_pulses;
			Liters_Current_Regeneration +=  delta_pulses;
			Water_Counter += delta_pulses;
			Liters_In_Tub +=  delta_pulses;
			Water_Loaded_Since_Last_Regen += delta_pulses;
			StatisticsDish__IncreaseWaterCounter(delta_pulses);
		}
		Liters_Since_Reference_Reset_Old = liters_since_reference_reset;
		Water_Counter_GI_Sequence_ID_Old = gi_sequence_id;

		Flow_Meter_Pulses_Present = TRUE;
	}
	else
	{
		Flow_Meter_Pulses_Present = FALSE;
	}


	// Life Test Mode Shortcut
	Life_Test_Mode_Shortcut_Status =  (LIFE_TEST_INPUT_STATUS_TYPE)Hbl__GetGIDataConvertedByIndex((uint8)Hbl__GetGIIndexByGIID(HBL_GI_LIFE_TEST_MODE_SHORTCUT));
	if (Life_Test_Mode_Shortcut_Status == (uint8)HBL_INDEX_INVALID )
	{
		Life_Test_Mode_Shortcut_Status = (uint8)FALSE;
	}

}

/*
 * Function to update pulse reference and reset Water Counter
 */
void DishAuxVar__UpdatePulseRef(void)
{
	uint16 *pulse_ref;
	uint8 pulse_index = Hbl__GetLLIIndexByGIIndex(Hbl__GetGIIndexByGIID(HBL_GI_WATER_COUNTER));

	pulse_ref = (uint16 *) Hbl__GetLLIDataByIndex(pulse_index);
	if (pulse_ref != NULL)
	{
		Pulse_Counter_Ref = *pulse_ref;
	}
}

/*
 * Function to return the LifeTestInput_Status
 */
LIFE_TEST_INPUT_STATUS_TYPE DishAuxVar__GetLifetestInputStatus(void)
{
	return (LIFE_TEST_INPUT_STATUS_TYPE)Life_Test_Mode_Shortcut_Status;
}

uint8 DishAuxVar__GetDoFactoryCycle(void)
{
	return Do_Factory_Cycle;
}

void DishAuxVar__SetDoFactoryCycle(BOOL_TYPE value)
{
	Do_Factory_Cycle = (uint8)value;
}

uint16 DishAuxVar__GetACLine(void)
{
	return AC_Line_Value;
}

void DishAuxVar__SetTimedFillRunning(BOOL_TYPE value)
{
	Timed_Fill_Running = value;
}

Diverter_Position_Type DishAuxVar__GetDiverterPosition(void)
{
	Diverter_Position_Type diverter_position;

	diverter_position.Position = Diverter_Position;
	diverter_position.Is_Stable = Diverter_Is_Stable;

	return diverter_position;
}

void DishAuxVar__ResetWaterCounterReference(void)
{
	Liters_Since_Reference_Reset_Old = 0;
	DishAuxVar__UpdatePulseRef();
	Water_Counter_GI_Sequence_ID_Old = Hbl__GetGISequenceIDByIndex(Hbl__GetGIIndexByGIID(HBL_GI_WATER_COUNTER));

}

void DishAuxVar__SetLitersCurrentRegeneration(uint16 value)
{
	Liters_Current_Regeneration = value;
}

uint16 DishAuxVar__GetLitersCurrentRegeneration(void)
{
	return Liters_Current_Regeneration;
}

PASS_FAIL_TYPE DishAuxVar__GetSensedDrainStatus(void)
{
	return Sensed_Drain_Status;
}

void DishAuxVar__SetSensedDrainStatus(PASS_FAIL_TYPE value)
{
	Sensed_Drain_Status = value;
}


/**
 * It returns TRUE if the drain pump is On, otherwise FALSE
 *
 */
BOOL_TYPE DishAuxVar__IsDrainPumpOn(void)
{
	BOOL_TYPE ret_value;
	uint8 drain_pump_index;
	uint8 *drain_pump_state_ptr;
	HBL_PILOT_ENUM_TYPE drain_pump_pilot_type;

	ret_value = FALSE;

	drain_pump_index = Hbl__GetLoadIndexByLoadID(HBL_LOAD_DRAIN_PUMP);
	drain_pump_pilot_type = Hbl__GetPilotIDByIndex(drain_pump_index);

	if (drain_pump_pilot_type != HBL_PILOT_INVALID)
	{
		if (drain_pump_pilot_type == HBL_PILOT_EXTERNAL_MOTOR)
		{
#if (API221_CLIENT_FEATURE == ENABLED)
			if (McuGetDataMonitor__GetDrainPumpSpeed() > 0)
			{
				ret_value = TRUE;
			}
#endif /* (API221_CLIENT_FEATURE == ENABLED) */
		}
		else
		{
			drain_pump_state_ptr = (uint8 *)Hbl__GetProcessedLoad(drain_pump_index);
			if ((drain_pump_state_ptr != NULL) && (*drain_pump_state_ptr != (uint8)OFF))
			{
				ret_value = TRUE;
			}
		}
	}

	return ret_value;
}


/**
 * It sets or resets the required bit in Generic_Bitmap variable
 *
 * @param  bit_index = the index of the bit to be set
 * @param  value = the value to be set for the bit
 *
 * @return  None
 */
void DishAuxVar__SetGenericBitmapBit(uint8 bit_index, ON_OFF_TYPE value)
{
	if (bit_index < (sizeof(Generic_Bitmap) * (uint8)8))
	{
		if (value == OFF)
		{
			BIT_CLR(Generic_Bitmap, bit_index);
		}
		else
		{
			BIT_SET8(Generic_Bitmap, bit_index);
		}
	}
}


/**
 * It returns Flow_Meter_Pulses_Present
 *
 */
BOOL_TYPE DishAuxVar__AreFlowMeterPulsesPresent(void)
{
	return Flow_Meter_Pulses_Present;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the Flow_Meter_Pulses_Present variable.
 *
 *  			It has to be called before the first execution of SRPowerMgr__Initialize()
 *
 */
void DishAuxVar__InitializeFlowMeterPulsesPresent(void)
{
	Flow_Meter_Pulses_Present = FALSE;

	TriacVoltageMonitor__InitISRVariables();
}


PASS_FAIL_TYPE DishAuxVar_AverageWithRejection(uint16 *input_samples_array, uint8 array_size, uint8 num_samples_available, uint8 num_samples_to_keep, uint16 *calculated_average)
{
	PASS_FAIL_TYPE ret_value = PASS;
	uint32 sum;
	float32 f32_local = 0.0F;
	uint8 i;

	if ((num_samples_available > (uint8)0) &&
		(num_samples_available <= array_size) &&
		(num_samples_available <= AVEARAGE_WITH_REJ_MAX_SUPPORTED_SAMPLES_NUM) &&
		(num_samples_to_keep > (uint8)0))
	{
		// Calculate mean value of all available samples
		sum = 0;
		for (i = 0; i < num_samples_available; i ++)
		{
			sum += input_samples_array[i];
		}

		f32_local = ((float32)sum)/ (float32)num_samples_available;

		if (num_samples_available > num_samples_to_keep)
		{
			// Apply Rejection

			BOOL_TYPE sorted;
			uint8 index;

			// First calculate absolute delta array and initialize Sorted_Index_Array
			for (i = 0; i < num_samples_available; i ++)
			{
				float32 diff;

				diff = (float32)input_samples_array[i] - f32_local;
				if (diff < 0.0)
				{
					diff = -diff;
				}
				Delta_Array[i] = diff;

				Sorted_Index_Array[i] = i;
			}

			// Bubble sort of Delta_Array and use it to sort Sorted_Index_Array
			sorted = FALSE;
			index = num_samples_available;
			while ((index > (uint8)1) && (sorted == FALSE))
			{
				sorted = TRUE;
				for (i = 0; i < (index - (uint8)1); i ++)
				{
					if (Delta_Array[i] > Delta_Array[i+(uint8)1])
					{
						uint8 temp;

						// Exchange Delta_Array[i] with Delta_Array[i+1]
						f32_local = Delta_Array[i];
						Delta_Array[i] = Delta_Array[i+(uint8)1];
						Delta_Array[i+(uint8)1] = f32_local;

						// Sort Sorted_Index_Array as Delta_Array
						temp = Sorted_Index_Array[i];
						Sorted_Index_Array[i] = Sorted_Index_Array[i+(uint8)1];
						Sorted_Index_Array[i+(uint8)1] = temp;

						sorted = FALSE;
					}
				}
				index --;
			}

			// Calculate the mean of the values to keep
			sum = 0;
			for (i = 0; i < num_samples_to_keep; i ++)
			{
				sum += input_samples_array[Sorted_Index_Array[i]];
			}
			f32_local = ((float32)sum)/ (float32)num_samples_to_keep;
		}
	}
	else
	{
		// Invalid input data
		ret_value = FAIL;
	}

	*calculated_average = (uint16)lroundf(f32_local);

	return ret_value;
}


/**
**
 * It sets the value of the amount of water loaded with fill valve since the last 'Last step' regeneration was executed.
 *
 */
void DishAuxVar__SetWaterLoadedSinceLastRegen(uint16 value)
{
	Water_Loaded_Since_Last_Regen = value;
}


/**
 * It returns the amount of water loaded with fill valve since the last 'Last step' regeneration was executed.
 *
 */
uint16 DishAuxVar__GetWaterLoadedSinceLastRegen(void)
{
	return Water_Loaded_Since_Last_Regen;
}



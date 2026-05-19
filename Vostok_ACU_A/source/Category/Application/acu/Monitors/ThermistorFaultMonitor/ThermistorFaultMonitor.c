/**
 *  @file
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

// -- This Module --
#include "SystemConfig.h"
#include "ThermistorFaultMonitor.h"
#include "ThermistorOpenMonitor.h"
#include "ThermistorShortedMonitor.h"

// -- Other Modules --
#include "Hbl.h"
#include "Log.h"
#include "Micro.h"
#include "SettingFile.h"


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define MODULE_NAME MODULE_THERMISTOR_FAULT_MONITOR

typedef enum MODULE_THERMISTOR_FAULT_MONITOR_LOG_MESSAGE_ID_ENUM
{
	ERROR_LOADING_PARAMETERS       = 1,
} MODULE_THERMISTOR_FAULT_MONITOR_LOG_MESSAGE_ID_TYPE;

//! Temperature above which shorted thermistor is assumed
static sint16 Thermistor_High_Threshold;
//! The temperature measured by the wash NTC sensor
static sint16 Wash_Probe_Temperature;
//! Boolean indicating if the Thermistor Fault Monitor is active or not
static BOOL_TYPE Thermistor_Fault_Monitor_Is_Active;


//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module ThermistorFault and its variables
 *
 */
void ThermistorFaultMonitor__Initialize(void)
{
	if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
	{
		uint8 wash_temperature_gi_index;
		SETTINGFILE_LOADER_TYPE loader;
		uint16 temperature_scale;

		Thermistor_Fault_Monitor_Is_Active = FALSE;

		wash_temperature_gi_index = Hbl__GetGIIndexByGIID(HBL_GI_WASHING_PROBE);

		if (wash_temperature_gi_index != (uint8)HBL_INDEX_INVALID)
		{
			if (SettingFile__BasicLoader(SF_PTR_ACU_CROSS_PRODUCT_CONFIG, SF_DISPL_BASE_UNITS, &loader) == PASS)
			{
				temperature_scale = ENDIANU16_STON(*(uint16*)(void*)(loader.Data));

				Thermistor_High_Threshold = (sint16)((((sint32)temperature_scale - 1) * (__SIGNED_SHORT_MAX__ + 1)) / (sint32)temperature_scale);

				ThermistorOpenFaultMonitor__Initialize();
				ThermistorShortedFaultMonitor__Initialize();

				Thermistor_Fault_Monitor_Is_Active = TRUE;
			}
			else
			{
				LOG_ADD_EXCEPTION (ERROR_LOADING_PARAMETERS, SF_DISPL_BASE_UNITS);
			}
		}
		else
		{
			LOG_ADD_EXCEPTION (ERROR_LOADING_PARAMETERS, wash_temperature_gi_index);
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief
 *
 */
void ThermistorFaultMonitor__Execution(void)
{
	uint8 wash_temperature_gi_index;
	uint8 wash_temperature_sequence_id;

	if ((uint8)Thermistor_Fault_Monitor_Is_Active == (uint8)TRUE)
	{
		wash_temperature_gi_index = Hbl__GetGIIndexByGIID(HBL_GI_WASHING_PROBE);
		wash_temperature_sequence_id = Hbl__GetGISequenceIDByIndex(wash_temperature_gi_index);

		if (wash_temperature_sequence_id > (uint8)0)
		{
			Wash_Probe_Temperature = (sint16)Hbl__GetGIDataConvertedByIndex(wash_temperature_gi_index);

			ThermistorOpenFaultMonitor__Execution();
			ThermistorShortedFaultMonitor__Execution();
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief
 *
 */
void ThermistorFaultMonitor__CycleStarted(void)
{
	ThermistorOpenFaultMonitor__ExitFault();
	ThermistorShortedFaultMonitor__ExitFault();
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief
 *
 */
sint16 ThermistorFaultMonitor__GetShortedThreshold(void)
{
	return Thermistor_High_Threshold;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief
 *
 */
sint16 ThermistorFaultMonitor__GetTemperature(void)
{
	return Wash_Probe_Temperature;
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


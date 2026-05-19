/*
 * ZeroCrossMonitor.c
 *
 *  Created on: Mar 20, 2018
 *      Author: DHAMAAD
 */

/**
 *  @file       ZeroCrossMonitor.c
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2011.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */

//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "ZeroCrossMonitor.h"

// -- Other Modules --
#include "DishAuxVar.h"
#include "Fault.h"
#include "Hbl.h"
#include "Micro.h"
#include "SettingFile.h"
#include "WashHeaterFaultMonitor.h"
#include "ZeroCross.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
static BOOL_TYPE Zero_Cross_Present;
static uint16 AC_Line_Lower_Threshold;

//! Boolean indicating if the Zero Cross Monitor is active or not
static BOOL_TYPE ZeroCross_Monitor_Is_Active;



//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module ZeroCrossMonitor and its variables
 *
 */
void ZeroCrossMonitor__Initialize(void)
{
	SETTINGFILE_LOADER_TYPE loader;
	if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
	{
		Zero_Cross_Present = FALSE;
		ZeroCross_Monitor_Is_Active = FALSE;

		if (Fault__GetParameters(FAULT_ID_ZEROCROSS) != NULL)
		{
			if ((Hbl__GetLoadIndexByLoadID(HBL_LOAD_PILOT_RELAY) != (uint8)HBL_INDEX_INVALID) ||
				(Hbl__GetLoadIndexByLoadID(HBL_LOAD_RELAY_ENABLE) != (uint8)HBL_INDEX_INVALID))
			{

				AC_Line_Lower_Threshold = 0xFFFF;

				if (SettingFile__BasicLoader(SF_PTR_ACU_PRODUCT_CONFIGURATION_DATA, DSPL_HEATER_CURRENT_SENSING_PARAMS, &loader) == PASS)
				{
					AC_Line_Lower_Threshold = ENDIANU16_STON(((HEATER_CURRENT_SENSING_PARAMS_TYPE *)(void*)loader.Data)->AC_Line_Lower_Threshold);

					ZeroCross_Monitor_Is_Active = TRUE;
				}
			}
			else
			{
				// Neither Pilot Relay nor Relay Enable is present, monitor is disabled
			}
		}
		else
		{
			// Fault parameters not present in setting file, monitor is disabled
		}
	}
}

void ZeroCrossMonitor__Execution(void)
{
	if ((uint8)ZeroCross_Monitor_Is_Active == (uint8)TRUE)
	{
		uint8 *pilot_relay_state;

		pilot_relay_state = (uint8*)(Hbl__GetLoadStatusByLoadID(HBL_LOAD_PILOT_RELAY));

		if (pilot_relay_state == NULL)
		{
			pilot_relay_state = (uint8*)(Hbl__GetLoadStatusByLoadID(HBL_LOAD_RELAY_ENABLE));
		}

		Zero_Cross_Present = Hbl__IsZeroCrossPresent();

		if (pilot_relay_state != NULL)
		{
			if (*pilot_relay_state == (uint8)ON)
			{
				// Pilot Relay is On
				if ((Zero_Cross_Present == FALSE) && (DishAuxVar__GetACLine() > AC_Line_Lower_Threshold))
				{
					// Zero Crossing signal is absent and AC Line signal is present: it means that there is a failure in the Zero Crossing signal
					Fault__ConditionDetected(FAULT_ID_ZEROCROSS);
				}
				else
				{
					// Zero Crossing signal is present (so no failure detected) or Zero Crossing signal is absent but also AC Line signal is absent:
					// it means that Pilot Relay is stuck open or (in the configurations for which the pilot relay status depends on the door)
					// that the door is open.
					Fault__ConditionRemoved(FAULT_ID_ZEROCROSS);
				}
			}
			else
			{
				// Pilot Relay is Off
				Fault__ConditionRemoved(FAULT_ID_ZEROCROSS);
			}
		}
	}
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================





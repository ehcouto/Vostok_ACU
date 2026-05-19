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
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "WashHeaterFaultMonitor.h"
#include "HeaterLeakageMonitor.h"
#if (HEATER_OPEN_PLUS_L_RELAY_CLOSED_DETECTION == ENABLED)
#include "HeaterOpenAndLineRelayClosedMonitor.h"
#endif // (HEATER_OPEN_PLUS_L_RELAY_CLOSED_DETECTION == ENABLED)
#include "HeaterLineRelayOpenMonitor.h"
#include "HeaterNeutralRelayOpenMonitor.h"
#if (HEATER_OPEN_UNIQUELY_DETECTED == ENABLED)
#include "HeaterOpenMonitor.h"
#endif // (HEATER_OPEN_UNIQUELY_DETECTED == ENABLED)

// -- Other Modules --
#include "DishAuxVar.h"
#include "DoorMonitor.h"
#include "Hbl.h"
#include "Log.h"
#include "Micro.h"
#include "NVData.h"
#include "SettingFile.h"



//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#define MODULE_NAME MODULE_HEATER_FAULT_MONITOR

typedef enum MODULE_HEATER_FAULT_MONITOR_LOG_MESSAGE_ID_ENUM
{
	ERROR_MISSING_COMPONENTS  = 1,
	ERROR_MISSING_PARAMETERS
} MODULE_HEATER_FAULT_MONITOR_LOG_MESSAGE_ID_TYPE;

//#define STABLE_HEATER_CURRENT_SENSING_THRESHOLD		ENDIANU16_STON(Heater_Sensing_Paramters_Ptr->Heater_Off_Threshold)
//#define LOWER_HEATER_CURRENT_SENSING_THRESHOLD		ENDIANU16_STON(Heater_Sensing_Paramters_Ptr->Heater_On_Lower_Threshold)
//#define UPPER_HEATER_CURRENT_SENSING_THRESHOLD		ENDIANU16_STON(Heater_Sensing_Paramters_Ptr->Heater_On_Upper_Threshold)
#define HEATER_FAULT_STABILIZATION_TIME				Heater_Sensing_Paramters_Ptr->Stabilization_Time
#define AC_LINE_LOWER_THRESHOLD						ENDIANU16_STON(Heater_Sensing_Paramters_Ptr->AC_Line_Lower_Threshold)

//! The consistency of the wash heater load
static uint8 Wash_Heater_Consistency;
//! Time to wait after a status change before evaluating the heater feedbacks
static uint8 Heater_Fault_Stabilization_Counter;
// Pre condition to evaluate heater's feedback, based on pilot relay state and zero cross signal presence
static BOOL_TYPE Monitor_Precondition;
//! Pointer to the Heater Sensing parameters in Setting File
static HEATER_CURRENT_SENSING_PARAMS_TYPE *Heater_Sensing_Paramters_Ptr;
//! Boolean indicating if the Wash Heater Fault Monitor is active or not
static BOOL_TYPE Wash_Heater_Fault_Monitor_Is_Active;

#pragma location = "_appliance_segment"
//! Bitmap set when a heater fault is detected, all bits reset when a cycle is started.
static uint8 Heater_Disabled_Bitmap;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//----------------------------------------------------------------------------
/**
 *  @brief      WashHeaterFaultMonitor__Initialize
 *  @details    Function used to initialize the Heater Fault monitor.
 *
 */
void WashHeaterFaultMonitor__Initialize(void)
{
	if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
	{
		uint8 wash_heater_index;
#if (HEATER_FAILURES_MANAGED_WITHOUT_PILOT_RELAY == DISABLED)
		uint8 ac_line_gi_index;
#endif // (HEATER_FAILURES_MANAGED_WITHOUT_PILOT_RELAY == DISABLED)
		SETTINGFILE_LOADER_TYPE loader;

		Wash_Heater_Fault_Monitor_Is_Active = FALSE;

		Wash_Heater_Consistency = COMBINE_NIBBLES(HBL_CONSISTENCY_TABLE_INVALID, HBL_CONSISTENCY_INVALID_VALUE);
		Heater_Fault_Stabilization_Counter = 0;

#if (HEATER_FAILURES_MANAGED_WITHOUT_PILOT_RELAY == ENABLED)
		Monitor_Precondition = TRUE;
#else
		Monitor_Precondition = FALSE;
#endif // (HEATER_FAILURES_MANAGED_WITHOUT_PILOT_RELAY == DISABLED)

		Heater_Sensing_Paramters_Ptr = (HEATER_CURRENT_SENSING_PARAMS_TYPE *)NULL;

		if (NVData__GetLastRestoreStatus()->Bit.Appliance_Data_Recovered != (NVDATA_RESTORE_TYPE)NVDATA_RESTORE_OKAY)
		{
			Heater_Disabled_Bitmap = 0;
		}

		wash_heater_index = Hbl__GetLoadIndexByLoadID(HBL_LOAD_WASHING_HEATER);
#if (HEATER_FAILURES_MANAGED_WITHOUT_PILOT_RELAY == DISABLED)
		ac_line_gi_index = Hbl__GetGIIndexByGIID(HBL_GI_AC_LINE);
#endif // (HEATER_FAILURES_MANAGED_WITHOUT_PILOT_RELAY == DISABLED)

		if ((wash_heater_index != (uint8)HBL_INDEX_INVALID)
#if (HEATER_FAILURES_MANAGED_WITHOUT_PILOT_RELAY == DISABLED)
			&& (ac_line_gi_index != (uint8)HBL_INDEX_INVALID)
#endif // (HEATER_FAILURES_MANAGED_WITHOUT_PILOT_RELAY == DISABLED)
			)
		{
			if (SettingFile__BasicLoader(SF_PTR_ACU_PRODUCT_CONFIGURATION_DATA, DSPL_HEATER_CURRENT_SENSING_PARAMS, &loader) == PASS)
			{
				Heater_Sensing_Paramters_Ptr = (HEATER_CURRENT_SENSING_PARAMS_TYPE *)(void *)loader.Data;

				Heater_Fault_Stabilization_Counter = HEATER_FAULT_STABILIZATION_TIME;

				HeaterNeutralRelayOpenMonitor__Initialize();
				HeaterLineRelayOpenMonitor__Initialize();
				HeaterLeakageMonitor__Initialize();
#if (HEATER_OPEN_UNIQUELY_DETECTED == ENABLED)
				HeaterOpenMonitor__Initialize();
#endif // (HEATER_OPEN_UNIQUELY_DETECTED == ENABLED)
#if (HEATER_OPEN_PLUS_L_RELAY_CLOSED_DETECTION == ENABLED)
				HeaterOpenAndLineRelayClosedMonitor__Initialize();
#endif // (HEATER_OPEN_PLUS_L_RELAY_CLOSED_DETECTION == ENABLED)

				Wash_Heater_Fault_Monitor_Is_Active = TRUE;
			}
			else
			{
				LOG_ADD_EXCEPTION (ERROR_MISSING_PARAMETERS, 0);
			}
		}
		else
		{
			LOG_ADD_EXCEPTION (ERROR_MISSING_COMPONENTS, 0);
		}
	}
}


//----------------------------------------------------------------------------
/**
 *  @brief      WashHeaterFaultMonitor__Execution
 *  @details    Function used to perform the Heater Fault monitor.
 *
 */
void WashHeaterFaultMonitor__Execution(void)
{
	uint8 heater_state;
	DOOR_STATUS_TYPE door_status;
	static uint8 heater_prev_state = 0xF0;
#if (HEATER_FAILURES_MANAGED_WITHOUT_PILOT_RELAY == DISABLED)
	BOOL_TYPE zc_presence;
	static BOOL_TYPE zc_prev_presence = FALSE;
	uint8 *pilot_relay_state_ptr;
	uint8 pilot_relay_state;
	static uint8 pilot_relay_prev_state = OFF;
#endif // (HEATER_FAILURES_MANAGED_WITHOUT_PILOT_RELAY == DISABLED)
	static DOOR_STATUS_TYPE previous_door_status = DOOR_STATUS_UNKNOWN;

	if ((uint8)Wash_Heater_Fault_Monitor_Is_Active == (uint8)TRUE)
	{
#if (HEATER_FAILURES_MANAGED_WITHOUT_PILOT_RELAY == DISABLED)
		pilot_relay_state_ptr = (uint8*)Hbl__GetLoadStatusByLoadID(HBL_LOAD_PILOT_RELAY);

		if (pilot_relay_state_ptr != NULL)
		{
			pilot_relay_state = *pilot_relay_state_ptr;
		}
		else
		{
			pilot_relay_state_ptr = (uint8*)Hbl__GetLoadStatusByLoadID(HBL_LOAD_RELAY_ENABLE);

			if (pilot_relay_state_ptr != NULL)
			{
				pilot_relay_state = *pilot_relay_state_ptr;
			}
			else
			{
				pilot_relay_state = (uint8)ON;
			}
		}
#endif // (HEATER_FAILURES_MANAGED_WITHOUT_PILOT_RELAY == DISABLED)

		Wash_Heater_Consistency = Hbl__GetLoadConsistency(Hbl__GetLoadIndexByLoadID(HBL_LOAD_WASHING_HEATER));

		heater_state = Wash_Heater_Consistency & ((uint8)0xF0);

#if (HEATER_FAILURES_MANAGED_WITHOUT_PILOT_RELAY == DISABLED)
		if ((DishAuxVar__GetACLine() > AC_LINE_LOWER_THRESHOLD) && ((uint8)Hbl__IsZeroCrossPresent() == (uint8)TRUE))
		{
			zc_presence = TRUE;
		}
		else
		{
			zc_presence = FALSE;
		}
#endif // (HEATER_FAILURES_MANAGED_WITHOUT_PILOT_RELAY == DISABLED)

		// Check door state
		door_status = DoorMonitor__GetDoorStatus();

		// Reload Heater_Fault_Stabilization_Counter in transitions
		if ((heater_state != heater_prev_state) || (door_status != previous_door_status)
#if (HEATER_FAILURES_MANAGED_WITHOUT_PILOT_RELAY == DISABLED)
			|| ((zc_presence != zc_prev_presence)) || (pilot_relay_state != pilot_relay_prev_state)
#endif // (HEATER_FAILURES_MANAGED_WITHOUT_PILOT_RELAY == DISABLED)
			)
		{
			Heater_Fault_Stabilization_Counter = HEATER_FAULT_STABILIZATION_TIME;
			heater_prev_state = heater_state;
			previous_door_status = door_status;
#if (HEATER_FAILURES_MANAGED_WITHOUT_PILOT_RELAY == DISABLED)
			zc_prev_presence = zc_presence;
			pilot_relay_prev_state = pilot_relay_state;
#endif // (HEATER_FAILURES_MANAGED_WITHOUT_PILOT_RELAY == DISABLED)
		}
		else
		{
			if (Heater_Fault_Stabilization_Counter > 0U)
			{
				Heater_Fault_Stabilization_Counter --;
			}
		}

#if (HEATER_FAILURES_MANAGED_WITHOUT_PILOT_RELAY == DISABLED)
		// Update pre-condition to evaluate heater's feedback
		if ((pilot_relay_state != (uint8)OFF) && ((uint8)zc_presence == (uint8)TRUE))
		{
			Monitor_Precondition = TRUE;
		}
		else
		{
			Monitor_Precondition = FALSE;
		}
#endif // (HEATER_FAILURES_MANAGED_WITHOUT_PILOT_RELAY == DISABLED)

		HeaterNeutralRelayOpenMonitor__Execution();
		HeaterLineRelayOpenMonitor__Execution();
		HeaterLeakageMonitor__Execution();
#if (HEATER_OPEN_UNIQUELY_DETECTED == ENABLED)
		HeaterOpenMonitor__Execution();
#endif // (HEATER_OPEN_UNIQUELY_DETECTED == ENABLED)
#if (HEATER_OPEN_PLUS_L_RELAY_CLOSED_DETECTION == ENABLED)
		HeaterOpenAndLineRelayClosedMonitor__Execution();
#endif // (HEATER_OPEN_PLUS_L_RELAY_CLOSED_DETECTION == ENABLED)
	}
}


//----------------------------------------------------------------------------
/**
 *  @brief      WashHeaterFaultMonitor__CycleStarted
 *  @details    Function to be called when a cycle is started to remove heater's fault.
 *
 */
void WashHeaterFaultMonitor__CycleStarted(void)
{
	Heater_Disabled_Bitmap = 0;

	Heater_Fault_Stabilization_Counter = HEATER_FAULT_STABILIZATION_TIME;

	HeaterNeutralRelayOpenMonitor__ExitFault();
	HeaterLineRelayOpenMonitor__ExitFault();
	HeaterLeakageMonitor__ExitFault();
#if (HEATER_OPEN_UNIQUELY_DETECTED == ENABLED)
	HeaterOpenMonitor__ExitFault();
#endif // (HEATER_OPEN_UNIQUELY_DETECTED == ENABLED)
#if (HEATER_OPEN_PLUS_L_RELAY_CLOSED_DETECTION == ENABLED)
	HeaterOpenAndLineRelayClosedMonitor__ExitFault();
#endif // (HEATER_OPEN_PLUS_L_RELAY_CLOSED_DETECTION == ENABLED)
}


//----------------------------------------------------------------------------
/**
 *  @brief
 *  @details
 *
 */
BOOL_TYPE WashHeaterFaultMonitor__GetMonitorPrecondition(void)
{
	return Monitor_Precondition;
}


//----------------------------------------------------------------------------
/**
 *  @brief
 *  @details
 *
 */
uint8 WashHeaterFaultMonitor__GetStabilizationCounter(void)
{
	return Heater_Fault_Stabilization_Counter;
}


//----------------------------------------------------------------------------
/**
 *  @brief
 *  @details
 *
 */
void WashHeaterFaultMonitor__RestartStabilizationCounter(void)
{
	Heater_Fault_Stabilization_Counter = HEATER_FAULT_STABILIZATION_TIME;
}


//----------------------------------------------------------------------------
/**
 *  @brief
 *  @details
 *
 */
uint8 WashHeaterFaultMonitor__GetHeaterConsistency(void)
{
	return Wash_Heater_Consistency;
}


//----------------------------------------------------------------------------
/**
 *
 */
uint8 WashHeaterFaultMonitor__GetDisabledBitmap(void)
{
	return Heater_Disabled_Bitmap;
}


/**
 * It sets the required bit in Heater_Disabled_Bitmap variable
 *
 * @param  bit_index = the index of the bit to be set
 *
 * @return  None
 */
void WashHeaterFaultMonitor__SetHeaterDisabledBitmapBit(BIT_INDEXES_IN_HEATER_DISABLED_BITMAP_TYPE bit_index)
{
	if (bit_index < HEATER_DISABLED_MAX_INDEX)
	{
		BIT_SET8(Heater_Disabled_Bitmap, bit_index);
	}
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


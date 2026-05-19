/**
 *  @file		VwiZoneCompensationMonitor.c
 *
 *  @brief		It provide the Vwi Zone Compensation multiplier to the VWI module
 *
 *  @details
 *
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "VwiZoneCompensationMonitor.h"
#include "SystemConfig.h"
// -- This Module --
#include "AlternateLoad.h"
#include "DishAuxVar.h"
#include "Hbl.h"
#include "Micro.h"
#include "SystemTimers.h"
#include "Vwi.h"
#include <string.h>
#if (VWI_THREE_PHASE_MOTOR_TYPE== ENABLED)

#if (VWI_CLIENT_FEATURE == DISABLED) && (VWI_INTERNAL_COMMUNICATION == DISABLED)
	#error "For VWI Zone Compensation to work either VWI_CLIENT_FEATURE or VWI_INTERNAL_COMMUNICATION has to be ENABLED."
#endif

#include "Fault.h"

#if (VWI_CLIENT_FEATURE == ENABLED)

#if (API013_CLIENT_FEATURE == DISABLED)
	#error "For VWI Zone Compensation to work API013_CLIENT_FEATURE has to be ENABLED if (VWI_CLIENT_FEATURE == ENABLED)."
#endif

#include "API013RemoteFunction.h"
#include "RemoteFunctionInterface.h"
#include "Reveal.h"
#include "Utilities.h"
#endif // (VWI_CLIENT_FEATURE == ENABLED)

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define VWI_MULTIPLIER_DEFAULT 4096

#if (VWI_CLIENT_FEATURE == ENABLED)
//! Period to refresh the value of VWI multiplier to the server
#define VWI_MULTIPLIER_REFRESH_PERIOD		50
#endif

//! Setting file parameters structure for Vwi Zone Compensation Common Params
typedef PACKED struct VWI_ZONE_COMPENSATION_COMMON_PARAMS_STRUCT
{

	uint8  Unknown_Position_Multiplier_Enable :1; 	//!
	uint8  Unused							  :7;	//! Reserved for future expansions
	uint8  Multiplyer_Delay; 						//!
	uint16 Multiplier_Position_Unknown;			 	//!
} VWI_ZONE_COMPENSATION_COMMON_PARAMS_TYPE;

//! Setting file parameters structure for a Vwi Zone Compensation Multiplier
typedef PACKED struct VWI_ZONE_COMPENSATION_MULTIPLIERS_PARAMS_STRUCT
{
	uint16 Multiplier_Pos;							//!
} VWI_ZONE_COMPENSATION_MULTIPLIERS_PARAMS_TYPE;

static const VWI_ZONE_COMPENSATION_COMMON_PARAMS_TYPE *Vwi_Zone_Compensation_Common_Params;
static BOOL_TYPE VWIZC_Old_Diverter_Position_Is_Stable;
static uint16 VWIZC_Current_Multiplier;
static uint16 VWIZC_Old_Current_Multiplier;
static uint8 *VWIZC_Diverter_Position_Requested;
static SYSTEMTIMER_NV_TIMER_TYPE NV_Timer_Handle_Vwi_Moltiplier_Delay;
static uint8 VWIZC_Diverter_Position;
static uint8 VWIZC_Last_Diverter_Position;
static uint16 VWIZC_Multiplier_Pos[DIVERTER_POSITION_MAX];
#if (VWI_CLIENT_FEATURE == ENABLED)
static uint8 VWIZC_Refresh_Command_Counter;
#endif // (VWI_CLIENT_FEATURE == ENABLED)

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
BOOL_TYPE Diverter_Position_Unknown(void);
void VwiZoneCompensationReadParams(void);
#if (VWI_CLIENT_FEATURE == ENABLED)
static void VwiZoneCompensation_API013ApplReceivedStatus (uint8 client_function_index);
#endif // (VWI_CLIENT_FEATURE == ENABLED)
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//----------------------------------------------------------------------------
/**
 * It initializes the Vwi Zone Compensation Monitor and its data.
 *
 * @return None
 *
 */
void VwiZoneCompensationMonitor__Initialize(void)
{
	if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
	{
		SETTINGFILE_LOADER_TYPE loader;
		Vwi_Zone_Compensation_Common_Params = NULL;
		if (SettingFile__BasicLoader(SF_PTR_ACU_ALGORITHM_PARAMETERS, DISPL_VWI_ZONE_COMPENSATION_COMMON, &loader) == PASS)
		{
			Vwi_Zone_Compensation_Common_Params = (const VWI_ZONE_COMPENSATION_COMMON_PARAMS_TYPE *)(void*)(loader.Data);

			VwiZoneCompensationReadParams();

			VWIZC_Diverter_Position = 0;
			VWIZC_Current_Multiplier = 0;
			VWIZC_Old_Current_Multiplier = 0;
			VWIZC_Old_Diverter_Position_Is_Stable = FALSE;
			VWIZC_Diverter_Position_Requested = NULL;
			VWIZC_Last_Diverter_Position = 0;
		}
		else
		{
			VWIZC_Current_Multiplier = VWI_MULTIPLIER_DEFAULT;
			VWIZC_Old_Current_Multiplier = 0;
		}
#if (VWI_CLIENT_FEATURE == ENABLED)
		// When VWI is on an external server, the first API013 command sending is triggered by the condition
		// (VWIZC_Refresh_Command_Counter = 0).
		// Setting
		// 		VWIZC_Old_Current_Multiplier = VWIZC_Current_Multiplier;
		// prevent the API013 command from being sent multiple times before the first API013 status packet is received.
		VWIZC_Old_Current_Multiplier = VWIZC_Current_Multiplier;

		VWIZC_Refresh_Command_Counter = 0;
		API013RemoteFunction__RegisterForEvents((API013_EVENT_HANDLER_TYPE)VwiZoneCompensation_API013ApplReceivedStatus);
#endif // (VWI_CLIENT_FEATURE == ENABLED)
	}
}


//----------------------------------------------------------------------------
/**
 * It implements the execution of the Vwi Zone Compensation Monitor
 *
 * @return None
 *
 */
void VwiZoneCompensationMonitor__Execution(void)
{
#if (VWI_CLIENT_FEATURE == ENABLED)
	uint16 reveal_node_status;
	uint8 *mcu_enable_state_ptr;
#endif // (VWI_CLIENT_FEATURE == ENABLED)

	if ((Vwi_Zone_Compensation_Common_Params != NULL) && (DishAuxVar__IsDrainPumpOn() == FALSE))
	{
		SETTINGFILE_LOADER_TYPE loader;
		uint8 num_items;

		if (Diverter_Position_Unknown() == FALSE)
		{
			if ((uint8)DishAuxVar__GetDiverterPosition().Is_Stable == (uint8)TRUE)
			{
				VWIZC_Diverter_Position = DishAuxVar__GetDiverterPosition().Position;

				VWIZC_Last_Diverter_Position = VWIZC_Diverter_Position;

				SystemTimers__NVResetTimer(&NV_Timer_Handle_Vwi_Moltiplier_Delay);
			}
			else if ((DishAuxVar__GetDiverterPosition().Is_Stable == FALSE) && ((uint8)VWIZC_Old_Diverter_Position_Is_Stable == (uint8)TRUE))
			{
				VWIZC_Diverter_Position_Requested = (uint8*)Hbl__GetRequestedLoad(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DIVERTER));

				if ((VWIZC_Diverter_Position_Requested != NULL) && (*VWIZC_Diverter_Position_Requested < (uint8)DIVERTER_POSITION_MAX))
				{
					if ((VWIZC_Multiplier_Pos[*VWIZC_Diverter_Position_Requested] > VWIZC_Multiplier_Pos[VWIZC_Diverter_Position]) &&
							(VWIZC_Multiplier_Pos[*VWIZC_Diverter_Position_Requested] != (uint16)__UNSIGNED_SHORT_MAX__))
					{
						VWIZC_Diverter_Position = *VWIZC_Diverter_Position_Requested;
						SystemTimers__NVResetTimer(&NV_Timer_Handle_Vwi_Moltiplier_Delay);
					}
					else
					{
						if (Vwi_Zone_Compensation_Common_Params->Multiplyer_Delay == (uint8)0)
						{
							VWIZC_Diverter_Position = *VWIZC_Diverter_Position_Requested;
						}
						else
						{
							(void)SystemTimers__NVSetTimerMS(&NV_Timer_Handle_Vwi_Moltiplier_Delay, Vwi_Zone_Compensation_Common_Params->Multiplyer_Delay*(uint32)100 ,SYSTEMTIMERS_TYPE_DOWN_COUNTER);
						}
					}
				}
			}

			VWIZC_Old_Diverter_Position_Is_Stable = DishAuxVar__GetDiverterPosition().Is_Stable;

			if (SystemTimers__NVCheckTimer(&NV_Timer_Handle_Vwi_Moltiplier_Delay) == SYSTEMTIMERS_STATE_DONE)
			{
				if (VWIZC_Diverter_Position_Requested != NULL)
				{
					VWIZC_Diverter_Position = *VWIZC_Diverter_Position_Requested;
				}

				SystemTimers__NVResetTimer(&NV_Timer_Handle_Vwi_Moltiplier_Delay);
			}
		}

		if (SettingFile__BasicLoader(SF_PTR_ACU_ALGORITHM_PARAMETERS, DISPL_VWI_ZONE_COMPENSATION_MULTIPLIERS, &loader) == PASS)
		{
			num_items = (uint8)(loader.Length/sizeof(VWI_ZONE_COMPENSATION_MULTIPLIERS_PARAMS_TYPE));

			if (VWIZC_Diverter_Position < num_items)
			{
				if (Diverter_Position_Unknown() == FALSE)
				{
					VWIZC_Current_Multiplier = VWIZC_Multiplier_Pos[VWIZC_Diverter_Position];
				}
				else
				{
					if ((uint8)Vwi_Zone_Compensation_Common_Params->Unknown_Position_Multiplier_Enable == (uint8)TRUE)
					{
						VWIZC_Current_Multiplier = Vwi_Zone_Compensation_Common_Params->Multiplier_Position_Unknown;
					}
					else
					{
						VWIZC_Current_Multiplier = VWIZC_Multiplier_Pos[VWIZC_Last_Diverter_Position];
					}
				}
			}
			else
			{
				VWIZC_Current_Multiplier = VWI_MULTIPLIER_DEFAULT;
			}
		}
		else
		{
			VWIZC_Current_Multiplier = VWI_MULTIPLIER_DEFAULT;
		}
	}
	else
	{
		VWIZC_Current_Multiplier = VWI_MULTIPLIER_DEFAULT;
	}

#if (VWI_CLIENT_FEATURE == ENABLED)
	reveal_node_status = Reveal__GetNodestatus();

	mcu_enable_state_ptr = (uint8 *)Hbl__GetProcessedLoad(Hbl__GetLoadIndexByLoadID(HBL_LOAD_MCU_ENABLE));

	if (VWIZC_Refresh_Command_Counter > (uint8)0)
	{
		VWIZC_Refresh_Command_Counter --;
	}

	if ((BIT_TEST(reveal_node_status, WIN_MCU_ADDRESS) != (uint8)0) && ((mcu_enable_state_ptr == NULL) || (*mcu_enable_state_ptr != (uint8)OFF)))
	{
		// MCU is on and is communicating
		if ((VWIZC_Current_Multiplier != VWIZC_Old_Current_Multiplier) || (VWIZC_Refresh_Command_Counter == (uint8)0))
		{
			// VWI Multiplier has changed or it's time to refresh its value, the command is sent
			uint8 enable_params[2];

			enable_params[0] = HIBYTE(VWIZC_Current_Multiplier);
			enable_params[1] = LOBYTE(VWIZC_Current_Multiplier);

			(void)API013RemoteFunction__SendControl (API013_CLIENT_FUNCTION_VWI_MULTIPLIER,  REMOTE_FUNCTION_COMMAND_ENABLE, enable_params, sizeof(enable_params));

			// Reload the refresh counter
			VWIZC_Refresh_Command_Counter = VWI_MULTIPLIER_REFRESH_PERIOD;
		}
	}

#else
	if (VWIZC_Current_Multiplier != VWIZC_Old_Current_Multiplier)
	{
		Vwi_SetInputSignalMultiplier(VWIZC_Current_Multiplier);

		VWIZC_Old_Current_Multiplier = VWIZC_Current_Multiplier;
	}
#endif // (VWI_CLIENT_FEATURE == DISABLED)

}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
BOOL_TYPE Diverter_Position_Unknown(void)
{
	BOOL_TYPE ret_value;
	if 	(Fault__IsInFaultState(FAULT_ID_DIVERTER_CANNOT_FIND_POSITIONS) ||
	    ((uint8)AlternateLoad__GetAlternateLoadSearchHome() == (uint8)TRUE))
	{
		ret_value = TRUE;
	}
	else
	{
		ret_value = FALSE;
	}
	return ret_value;
}

void VwiZoneCompensationReadParams(void)
{
	SETTINGFILE_LOADER_TYPE loader;
	uint8 num_items;
	const VWI_ZONE_COMPENSATION_MULTIPLIERS_PARAMS_TYPE *vwi_zone_compensation_multiplier_params_ptr;

	memset(VWIZC_Multiplier_Pos,__UNSIGNED_CHAR_MAX__,sizeof(VWIZC_Multiplier_Pos));

	if (SettingFile__BasicLoader(SF_PTR_ACU_ALGORITHM_PARAMETERS, DISPL_VWI_ZONE_COMPENSATION_MULTIPLIERS, &loader) == PASS)
	{
		num_items = (uint8)(loader.Length/sizeof(VWI_ZONE_COMPENSATION_MULTIPLIERS_PARAMS_TYPE));

		for (uint8 pos = 0; pos < num_items; pos++)
		{
			vwi_zone_compensation_multiplier_params_ptr = (const VWI_ZONE_COMPENSATION_MULTIPLIERS_PARAMS_TYPE *)(void*)(loader.Data + (pos * sizeof(VWI_ZONE_COMPENSATION_MULTIPLIERS_PARAMS_TYPE)));
			VWIZC_Multiplier_Pos[pos] = vwi_zone_compensation_multiplier_params_ptr->Multiplier_Pos;
		}
	}
}


#if (VWI_CLIENT_FEATURE == ENABLED) && (VWI_INTERNAL_COMMUNICATION == DISABLED) && (API013_CLIENT_FEATURE == ENABLED)
static void VwiZoneCompensation_API013ApplReceivedStatus (uint8 client_function_index)
{
	const API013_FUNCTION_STATUS_TYPE* received_status;

	received_status = API013RemoteFunction__GetReceivedStatus();

	if (client_function_index == (uint8)API013_CLIENT_FUNCTION_VWI_MULTIPLIER)
	{
		if (received_status->Size >= sizeof(VWIZC_Old_Current_Multiplier))
		{
			VWIZC_Old_Current_Multiplier = Utilities__ConvertArrayTo16bits(received_status->Status);
		}
	}
}
#endif // (VWI_CLIENT_FEATURE == ENABLED)

#endif // (VWI_THREE_PHASE_MOTOR_TYPE== ENABLED)

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
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "LITTMonitor.h"
#include "SystemConfig.h"

#include "DoorMonitor.h"
#include "DoorOpeningSystemMonitor.h"
#include "Hbl.h"
#include "Interpreter.h"
#include "Log.h"
#include "Micro.h"
#include "Regulations.h"
#include "Selector.h"
#include "SettingFile.h"
#include "Supervisor.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//! The number of times that the LITTMonitor__Execution function will be called every second.
#define LITT_MONITOR_CALLS_PER_SECOND	(uint8)10

#define MODULE_NAME MODULE_LITT_MONITOR

#define LM_CYCLE_STATE_OFF  (CYCLE_STATE_TYPE)(((uint8)CYCLE_STATE_INVALID - (uint8)1)) // I put LM_CYCLE_STATE_OFF because CYCLE_STATE_OFF is not covered

typedef enum MODULE_LITT_MONITOR_LOG_MESSAGE_ID_ENUM
{
	ERROR_LOADING_PARAMETERS       = 1,
} MODULE_LITT_MONITOR_LOG_MESSAGE_ID_TYPE;


typedef PACKED struct LITT_MONITOR_INSTANCE_STRUCT
{
   uint8 LITT_Max_Time;	// The maximum time (in minutes) the light in the tub will be on while the door is open.
} LITT_MONITOR_INSTANCE_TYPE;

static uint32 LM_LITT_Max_Timer;
static CYCLE_STATE_TYPE LM_Cycle_State_Old;
static ON_OFF_TYPE LM_LITT_Status;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//----------------------------------------------------------------------------
/**
 *  @brief      LITTMonitor__Initialize
 *  @details    Function used to initialize the LITT monitor.
 *
 */
void LITTMonitor__Initialize(void)
{
	if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
	{
		SETTINGFILE_LOADER_TYPE loader;
		uint8 litt_load_index;
		uint8 current_state;

		// Get the LITT parameters from the setting file.
		if (SettingFile__BasicLoader(SF_PTR_ACU_PRODUCT_CONFIGURATION_DATA,
									 DSPL_LIGHT_IN_THE_TUBE_PARAMS,
									&loader) == FAIL)
		{
			LM_LITT_Max_Timer= 0;
			LOG_ADD_EXCEPTION (ERROR_LOADING_PARAMETERS, 0);
		}
		else
		{
			// Read the setting file data into local variables for convenience & convert it to 100 ms from min.
			LM_LITT_Max_Timer = (((LITT_MONITOR_INSTANCE_TYPE *)loader.Data)->LITT_Max_Time) * (60U * LITT_MONITOR_CALLS_PER_SECOND);
		}

		// Set default state for LITT == OFF.
		LM_LITT_Status = OFF;

		litt_load_index = Hbl__GetLoadIndexByLoadID(HBL_LOAD_LITT);
    	current_state = (Hbl__GetPilotIDByIndex(litt_load_index) == HBL_PILOT_STEADY_PWM_PROFILE) ? ((uint8)LITT_OFF):((uint8)OFF);
		(void)Hbl__SetLoadRequestByIndex(litt_load_index, HBL_PRIORITY_APPLICATION_HI,&current_state);
	}
}

//----------------------------------------------------------------------------
/**
 *  @brief      LITTMonitor__Execution
 *  @details    Function used to perform the LITT monitor.
 *
 */
void LITTMonitor__Execution(void)
{
	SETTINGFILE_LOADER_TYPE loader;
	uint8 LITT_value;
	uint8 current_state;
	uint8 door_input_gi_index;
	uint8 door_input_sequence_id;

	LITT_value = (uint8)Regulations__GetMainValueByPosition(Regulations__GetPositionByFunctionID(FUNC_ID_LIGHT_IN_THE_TUB, 0));

	door_input_gi_index = Hbl__GetGIIndexByGIID(HBL_GI_DOOR);

	if (door_input_gi_index != (uint8)HBL_INDEX_INVALID)
	{
		door_input_sequence_id = Hbl__GetGISequenceIDByIndex(door_input_gi_index);

		if (door_input_sequence_id > 0U)
		{
			if ((DoorMonitor__GetDoorStatus() == DOOR_STATUS_OPEN) && (LM_LITT_Max_Timer != 0U) && (LITT_value == 1U) && (DoorOpeningSystemMonitor__IsDoorOpenedByDOS() == FALSE))
			{
				uint8 litt_load_index;

				LM_LITT_Status = ON;
				litt_load_index = Hbl__GetLoadIndexByLoadID(HBL_LOAD_LITT);
		    	current_state = (Hbl__GetPilotIDByIndex(litt_load_index) == HBL_PILOT_STEADY_PWM_PROFILE) ? ((uint8)LITT_ON):((uint8)ON);
				(void)Hbl__SetLoadRequestByIndex(litt_load_index, HBL_PRIORITY_APPLICATION_HI,&current_state);

			}
			else if((DoorMonitor__GetDoorStatus() == DOOR_STATUS_CLOSE)  || (LITT_value == (uint8)0 ) || (LM_LITT_Max_Timer == 0U) || ((uint8)DoorOpeningSystemMonitor__IsDoorOpenedByDOS() == (uint8)TRUE))
			{
				uint8 litt_load_index;

				LM_LITT_Status = OFF;
				litt_load_index = Hbl__GetLoadIndexByLoadID(HBL_LOAD_LITT);
		    	current_state = (Hbl__GetPilotIDByIndex(litt_load_index) == HBL_PILOT_STEADY_PWM_PROFILE) ? ((uint8)LITT_OFF):((uint8)OFF);
				(void)Hbl__SetLoadRequestByIndex(litt_load_index, HBL_PRIORITY_APPLICATION_HI,&current_state);

			}

			if (DoorMonitor__GetDoorStatus() == DOOR_STATUS_OPEN)
			{
				LM_LITT_Max_Timer = (LM_LITT_Max_Timer == 0U) ? (0U) : (LM_LITT_Max_Timer - 1U) ;

				CYCLE_STATE_TYPE cycle_state = Selector__GetCycleState(Interpreter__GetCurrentSFIndex(COMPARTMENT_DEFAULT),
																	   COMPARTMENT_DEFAULT,
																	   Interpreter__GetCurrentStatement(COMPARTMENT_DEFAULT).Cycle);

				if ((Interpreter__GetCurrentStatement(COMPARTMENT_DEFAULT).Cycle == (uint8)OFF) &&
					(Interpreter__GetCurrentStatement(COMPARTMENT_DEFAULT).Phase == (uint8)OFF))
				{
					cycle_state = LM_CYCLE_STATE_OFF; // I put LM_CYCLE_STATE_OFF because CYCLE_STATE_OFF is not covered
				}


				if (cycle_state != LM_Cycle_State_Old)
				{
					if (((CYCLE_STATE_TYPE)cycle_state != LM_CYCLE_STATE_OFF) || (LM_Cycle_State_Old != CYCLE_STATE_PROGRAMMING))
					{
						if (SettingFile__BasicLoader(SF_PTR_ACU_PRODUCT_CONFIGURATION_DATA,
													 DSPL_LIGHT_IN_THE_TUBE_PARAMS,
													 &loader) != FAIL)
						{
							LM_LITT_Max_Timer = (uint32)(((LITT_MONITOR_INSTANCE_TYPE *)loader.Data)->LITT_Max_Time) * (uint8)60 * LITT_MONITOR_CALLS_PER_SECOND;
						}
					}
					else
					{
						// Transition PROGRAMMING -> OFF: do not reload timer.
					}

					LM_Cycle_State_Old = cycle_state;
				}
			}
			else
			{
				if (SettingFile__BasicLoader(SF_PTR_ACU_PRODUCT_CONFIGURATION_DATA,
											 DSPL_LIGHT_IN_THE_TUBE_PARAMS,
											&loader) != FAIL)
				{
					LM_LITT_Max_Timer = (uint32)(((LITT_MONITOR_INSTANCE_TYPE *)loader.Data)->LITT_Max_Time) * (uint8)60 * LITT_MONITOR_CALLS_PER_SECOND;
				}
			}
		}
	}
}

ON_OFF_TYPE LITTMonitor__GetLITTStatus(void)
{
	return LM_LITT_Status;
}


void LITTMonitor__ResetLITTTimer(void)
{
	LM_LITT_Max_Timer = 0;
}

void LITTMonitor__RestartLITTTimer(void)
{
	SETTINGFILE_LOADER_TYPE loader;

	// Get the LITT parameters from the setting file.
	if (SettingFile__BasicLoader(SF_PTR_ACU_PRODUCT_CONFIGURATION_DATA, DSPL_LIGHT_IN_THE_TUBE_PARAMS, &loader) == PASS)
	{
		LM_LITT_Max_Timer = ((uint32)((LITT_MONITOR_INSTANCE_TYPE *)loader.Data)->LITT_Max_Time) * (uint8)60 * LITT_MONITOR_CALLS_PER_SECOND;
	}
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


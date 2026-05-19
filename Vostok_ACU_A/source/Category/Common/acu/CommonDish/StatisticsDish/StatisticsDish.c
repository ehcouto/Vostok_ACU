/**
 *  @file		StatisticsDish.c
 *
 *  @brief		It manages Dish Platform specific statistics
 *
 *  @details
 *
 *
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//=====================================================================================================================
//-------------------------------------------------- Include files ----------------------------------------------------
//=====================================================================================================================
#include "SystemConfig.h"
#include "StatisticsDish.h"

#include "Fault.h"
#include "HeaterCalcificationDetectionMonitor.h"
#include "Interpreter.h"
#include "limits.h"
#include "Micro.h"
#include "NVData.h"
#include "OWICalibrationProcess.h"
#include "Regulations.h"
#include "SaltSensingMonitor.h"
#include "Selector.h"
#include "Statistics.h"
#include "String.h"
#include "SystemTimers.h"
#include "Variable.h"

//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================

#define OPTION_INDEX_OFF	1

//! Definition for a structure used to store the Water Temperature after the first fill executed in a cycle.
typedef struct FIRST_FILL_TEMPERATURE_STRUCT
{
	sint16 Water_Temperature;

	uint8 First_Fill_Completed	:1;
	uint8 Unused_b1_to_b7		:7;
} FIRST_FILL_TEMPERATURE_TYPE;

//! Definition for a structure used to store temporary values for Last Cycles Statistics.
typedef struct STATISTICS_AUX_VARS_STRUCT
{
	FIRST_FILL_TEMPERATURE_TYPE First_Fill_Data;

	uint16 Water_Consumption_Cl;

	sint16 Heat_Final_Temperature;

	uint16 Heat_Duration_Counter;

	uint8 Heat_Duration_Minutes;

	uint8 Sanitization_Result;

	STATISTICS_FLAGS_TYPE Flags;

	uint8 Cycle_Low_Voltage_Event_Counter;

	uint8 Cycle_High_Voltage_Event_Counter;

	VOLTAGERANGE_STATE_TYPE Cycle_Voltage_Range_Status;
} STATISTICS_AUX_VARS_TYPE;

//! List of options that are saved in lifetime statistics data.
static const REGULATIONS_FUNCTION_ID_TYPE Lifetime_Statistics_Options_Functions_IDs[SAVED_OPTIONS_NUM] =
{
	FUNC_ID_SANI_RINSE,
	FUNC_ID_MIDDLE_RACK_ONLY,
	FUNC_ID_LOWER_RACK_ONLY,
	FUNC_ID_BOTTLE_WASH_ZONE,
	FUNC_ID_EXTRA_DRY,
	FUNC_ID_HEATED_DRY,
	FUNC_ID_STEAM,
	FUNC_ID_HIGH_TEMP,
	FUNC_ID_HALF_LOAD,
	FUNC_ID_TIME_SHORTENING,
	FUNC_ID_POWER_BOOST,
	FUNC_ID_POWER_CLEAN,
	FUNC_ID_DOS,
	FUNC_ID_3D_WASH,
	FUNC_ID_SILENT
};


//! Pointer to the Cycle Statistics for the most recent executed cycle. It is valid only until a new cycle is started.
static PLATFORM_CYCLE_TYPE *Platform_Most_Recent_Cycle_Stats_Ptr;

//! System timer used to count how much time the product is in Demo Mode
#pragma location = "_appliance_segment"
static SYSTEMTIMER_NV_TIMER_TYPE NV_Demo_Active_Timer;

//! Structure used to store temporary values for Last Cycles Statistics.
#pragma location = "_appliance_segment"
static STATISTICS_AUX_VARS_TYPE Stats_Aux_Vars;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

static void UpdateCycleData(PLATFORM_CYCLE_TYPE *cycle);
static void UpdateServiceEnteredCycleOdometer(void);
static void UpdateOWIStatus(PLATFORM_LIFETIME_TYPE *lifetime);
static void UpdateDemoCycleDuration(PLATFORM_LIFETIME_TYPE *lifetime);
static void UpdateRinseAidResult(PLATFORM_CYCLE_TYPE *cycle);
static void UpdateCyclesWithoutSalt(PLATFORM_CYCLE_TYPE *cycle);
static void UpdateTotalCyclesInHCDLockout(void);
static void UpdateOptionsCount(void);

//=====================================================================================================================
//------------------------------------------------ PUBLIC Functions ---------------------------------------------------
//=====================================================================================================================


/**
 * Initializes the StatisticsDish module variables.
 */
void StatisticsDish__Initialize(void)
{
	if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
	{
		if (NVData__GetLastRestoreStatus()->Bit.Appliance_Data_Recovered != (NVDATA_RESTORE_TYPE)NVDATA_RESTORE_OKAY)
		{
			memset(&Stats_Aux_Vars, 0, sizeof(Stats_Aux_Vars)); // Stats_Aux_Vars.Sanitization_Result is initialized to 0, assuming TASK_SANITIZATION_NOT_EXECUTED == 0

			SystemTimers__NVResetTimer(&NV_Demo_Active_Timer);
		}
		else
		{
			SystemTimers__NVRestore(&NV_Demo_Active_Timer);
		}

		Platform_Most_Recent_Cycle_Stats_Ptr = NULL;
	}
}


/*
 * Dish Platform lifetime statistics updates. Called once every second in Statistics__Background().
 *
 * @param lifetime = Pointer to platform lifetime statistics.
 */
void StatisticsDish__UpdateLifetimeData(PLATFORM_LIFETIME_TYPE *lifetime)
{
	UpdateDemoCycleDuration(lifetime);

	UpdateOWIStatus(lifetime);
}


/*
 * Dish Platform cycle statistics update.
 * Called when a cycle starts and stops or when cycle statistics data is requested while the cycle is running.
 * @param compartment = The compartment in which a cycle is actively running.
 * @param state = #STATISTICS_CYCLE_STATE_TYPE for the cycle.
 * @param cycle = Pointer to platform cycle statistic.
 * @param last_cycle = Pointer to the platform last cycle statistics.
 */
void StatisticsDish__UpdateCycleData(uint8 compartment, STATISTICS_CYCLE_STATE_TYPE state, PLATFORM_CYCLE_TYPE *cycle, void* last_cycle)
{
	// The cycle has been started or some information has been requested while the cycle runs.
	if(cycle != NULL)
	{
		switch(state)
		{
			case STATS_CYCLE_STARTED:
				memset(&Stats_Aux_Vars, 0, sizeof(Stats_Aux_Vars)); // Stats_Aux_Vars.Sanitization_Result is initialized to 0, assuming TASK_SANITIZATION_NOT_EXECUTED == 0
				UpdateTotalCyclesInHCDLockout();
				UpdateServiceEnteredCycleOdometer();
				UpdateOptionsCount();
				break;

			case STATS_CYCLE_ENDED:
				UpdateCycleData(cycle);
				UpdateRinseAidResult(cycle);
				UpdateCyclesWithoutSalt(cycle);
				Platform_Most_Recent_Cycle_Stats_Ptr = cycle;
				break;

			case STATS_CYCLE_ACTIVE:
				UpdateCycleData(cycle);
				break;

			case STATS_CYCLE_CANCELLED:
				UpdateCycleData(cycle);
				UpdateRinseAidResult(cycle);
				UpdateCyclesWithoutSalt(cycle);
				break;

			default:
				// nothing to do
				break;
		}
	}
}


/*
 * Dish Platform black box data update.
 * This macro is called when a fault occurs.
 * @param black_box = Pointer to #PLATFORM_BLACK_BOX_TYPE that should be updated.
 * @param fault_id = The fault_id that triggered this update.
 */
void StatisticsDish__UpdateFaultData(PLATFORM_BLACK_BOX_TYPE *black_box, uint8 fault_id)
{
	STATISTICS_LIFETIME_TYPE *lifetime_statistics_ptr = Statistics__GetLifetimeData();

	black_box->Fault_Id = fault_id;
	black_box->Cps = Interpreter__GetCurrentStatement(COMPARTMENT_DEFAULT);

	if (lifetime_statistics_ptr != NULL)
	{
		black_box->Age = lifetime_statistics_ptr->Cycles_Completed;
	}
}


/*
 * Dish Platform black box data update.
 * This macro is called when a pre-fault occurs.
 * @param black_box = Pointer to #PLATFORM_BLACK_BOX_TYPE that should be updated.
 * @param fault_id = The fault_id that triggered this update.
 */
void StatisticsDish__UpdatePreFaultData(PLATFORM_BLACK_BOX_TYPE *black_box, uint8 fault_id)
{
	STATISTICS_LIFETIME_TYPE *lifetime_statistics_ptr = Statistics__GetLifetimeData();

	black_box->Fault_Id = fault_id;
	black_box->Cps = Interpreter__GetCurrentStatement(COMPARTMENT_DEFAULT);

	if (lifetime_statistics_ptr != NULL)
	{
		black_box->Age = lifetime_statistics_ptr->Cycles_Completed;
	}
}

/*
 * Stability_Control_Loop_Reaction_Triggered update.
 * Called when a reaction from Stability Control Loop Detection is triggered.
 */
void StatisticsDish__SetSCLReactionTriggered(void)
{
	Stats_Aux_Vars.Flags.Bits.Stability_Control_Loop_Reaction_Triggered = TRUE;
}


void StatisticsDish__IncreaseSCLPrimaryReactionCounter(void)
{
	STATISTICS_LIFETIME_TYPE *lifetime_statistics_ptr = Statistics__GetLifetimeData();
	if (lifetime_statistics_ptr != NULL)
	{
		if (lifetime_statistics_ptr->Platform.Primary_Reaction_Counter < (uint16)__UNSIGNED_SHORT_MAX__)
		{
			lifetime_statistics_ptr->Platform.Primary_Reaction_Counter ++;
		}
	}
}


void StatisticsDish__IncreaseSCLSecondaryReactionCounter(void)
{
	STATISTICS_LIFETIME_TYPE *lifetime_statistics_ptr = Statistics__GetLifetimeData();
	if (lifetime_statistics_ptr != NULL)
	{
		if (lifetime_statistics_ptr->Platform.Secondary_Reaction_Counter < (uint16)__UNSIGNED_SHORT_MAX__)
		{
			lifetime_statistics_ptr->Platform.Secondary_Reaction_Counter ++;
		}
	}
}


void StatisticsDish__IncreaseDoorOpenings(void)
{
	STATISTICS_LIFETIME_TYPE *lifetime_statistics_ptr = Statistics__GetLifetimeData();
	if (lifetime_statistics_ptr != NULL)
	{
		if (lifetime_statistics_ptr->Platform.Door_Opened_Count < (uint16)__UNSIGNED_SHORT_MAX__)
		{
			lifetime_statistics_ptr->Platform.Door_Opened_Count ++;
		}
	}
}


void StatisticsDish__UpdateDelayRecord(void)
{
	STATISTICS_LIFETIME_TYPE *lifetime_statistics_ptr = Statistics__GetLifetimeData();
	if (lifetime_statistics_ptr != NULL)
	{
		sint32 delay_time_remaining;

		if ((uint8)Variable__GetValue(VARIABLE_SUPERVISOR_DELAY_TIME_REMAINING, COMPARTMENT_DEFAULT,&delay_time_remaining) == (uint8)TRUE)
		{
			if (delay_time_remaining > 0)
			{
				sint32 delay_slot_partial = (delay_time_remaining - 1);
				sint32 delay_slot_partial2 =(delay_slot_partial / 7200);
				uint8 delay_slot = (uint8)delay_slot_partial2;

				if (delay_slot < (uint8)MAX_DELAY_SLOTS)
				{
					if (lifetime_statistics_ptr->Platform.Selected_Delay_Count[delay_slot] < (uint16)__UNSIGNED_SHORT_MAX__)
					{
						lifetime_statistics_ptr->Platform.Selected_Delay_Count[delay_slot] ++;
					}
				}
			}
		}
	}
}


void StatisticsDish__IncreaseWaterCounter(uint16 water_amount)
{
	Stats_Aux_Vars.Water_Consumption_Cl += water_amount;
}


void StatisticsDish__DecreaseWaterCounter(uint16 water_amount)
{
	if (Stats_Aux_Vars.Water_Consumption_Cl > water_amount)
	{
		Stats_Aux_Vars.Water_Consumption_Cl -= water_amount;
	}
	else
	{
		Stats_Aux_Vars.Water_Consumption_Cl = 0;
	}
}


void StatisticsDish__UpdateFstFillWaterTemp(void)
{
	if ((uint8)Stats_Aux_Vars.First_Fill_Data.First_Fill_Completed == (uint8)FALSE)
	{
		sint32 water_temp;

		if ((uint8)Variable__GetValue(VARIABLE_WATER_TEMPERATURE, 0, &water_temp) == (uint8)TRUE)
		{
			Stats_Aux_Vars.First_Fill_Data.Water_Temperature = (sint16)water_temp;
		}

		Stats_Aux_Vars.First_Fill_Data.First_Fill_Completed = TRUE;
	}
}


void StatisticsDish__UpdateOWICalibrationDutyCycle(void)
{
	STATISTICS_LIFETIME_TYPE *lifetime_statistics_ptr = Statistics__GetLifetimeData();
	if (lifetime_statistics_ptr != NULL)
	{
		sint32 owi_calibration_duty_cycle;
		if ((uint8)Variable__GetValue(VARIABLE_OWI_CALIBRATION_DRIVE_DUTY_CYCLE, 0, &owi_calibration_duty_cycle) == (uint8)TRUE)
		{
			lifetime_statistics_ptr->Platform.OWI_Calibration_Drive_Duty_Cycle = (uint16)owi_calibration_duty_cycle;
		}
	}
}


void StatisticsDish__UpdateHeatRelatedMeasures(void)
{
	sint32 value;

	if ((uint8)Variable__GetValue(VARIABLE_WATER_TEMPERATURE, 0, &value) == (uint8)TRUE)
	{
		Stats_Aux_Vars.Heat_Final_Temperature = (sint16)value;
	}

	if ((uint8)Variable__GetValue(VARIABLE_SANITIZATION_RESULT, 0, &value) == (uint8)TRUE)
	{
		Stats_Aux_Vars.Sanitization_Result = (uint8)value;
	}
}


void StatisticsDish__ResetHeatDurationCounter(void)
{
	Stats_Aux_Vars.Heat_Duration_Counter = 0;
	Stats_Aux_Vars.Heat_Duration_Minutes = 0;
}


void StatisticsDish__UpdateHeatDurationCounter(void)
{
	if (Stats_Aux_Vars.Heat_Duration_Counter < (uint16)600)
	{
		Stats_Aux_Vars.Heat_Duration_Counter ++;
	}
	else
	{
		if (Stats_Aux_Vars.Heat_Duration_Minutes < (uint8)__UNSIGNED_CHAR_MAX__)
		{
			Stats_Aux_Vars.Heat_Duration_Minutes ++;
			Stats_Aux_Vars.Heat_Duration_Counter = 1;
		}
	}
}


void StatisticsDish__IncreaseInterruptedPhasesNum(void)
{
	STATISTICS_LIFETIME_TYPE *lifetime_statistics_ptr = Statistics__GetLifetimeData();
	if (lifetime_statistics_ptr != NULL)
	{
		if (lifetime_statistics_ptr->Platform.Num_Phases_Interrupted_by_Water_Indicator < (uint16)__UNSIGNED_SHORT_MAX__)
		{
			lifetime_statistics_ptr->Platform.Num_Phases_Interrupted_by_Water_Indicator ++;
		}
	}

	Stats_Aux_Vars.Flags.Bits.Phase_Interrupted = TRUE;
}


void StatisticsDish__IncreaseWaterAddExecutions(void)
{
	STATISTICS_LIFETIME_TYPE *lifetime_statistics_ptr = Statistics__GetLifetimeData();
	if (lifetime_statistics_ptr != NULL)
	{
		if (lifetime_statistics_ptr->Platform.Total_Water_Add_Executions < (uint16)__UNSIGNED_SHORT_MAX__)
		{
			lifetime_statistics_ptr->Platform.Total_Water_Add_Executions ++;
		}
	}

	Stats_Aux_Vars.Flags.Bits.Water_Add_Executed = TRUE;
}


void StatisticsDish__IncreaseRPMReductions(void)
{
	STATISTICS_LIFETIME_TYPE *lifetime_statistics_ptr = Statistics__GetLifetimeData();
	if (lifetime_statistics_ptr != NULL)
	{
		if (lifetime_statistics_ptr->Platform.Total_RPM_Reductions < (uint16)__UNSIGNED_SHORT_MAX__)
		{
			lifetime_statistics_ptr->Platform.Total_RPM_Reductions ++;
		}
	}

	Stats_Aux_Vars.Flags.Bits.RPM_Reduceed = TRUE;
}


void StatisticsDish__IncreaseCyclesWithLimescale(void)
{
	STATISTICS_LIFETIME_TYPE *lifetime_statistics_ptr = Statistics__GetLifetimeData();
	if (lifetime_statistics_ptr != NULL)
	{
		if (lifetime_statistics_ptr->Platform.Total_Cycles_With_Limescale < (uint16)__UNSIGNED_SHORT_MAX__)
		{
			lifetime_statistics_ptr->Platform.Total_Cycles_With_Limescale ++;
		}
	}

	if (Platform_Most_Recent_Cycle_Stats_Ptr != NULL)
	{
		Platform_Most_Recent_Cycle_Stats_Ptr->Flags.Bits.Heater_Limescaled_Detected = TRUE;
	}
}


void StatisticsDish__UpdateServiceFaultClearOdometer(void)
{
	STATISTICS_LIFETIME_TYPE *lifetime_statistics_ptr = Statistics__GetLifetimeData();
	if (lifetime_statistics_ptr != NULL)
	{
		lifetime_statistics_ptr->Platform.Service_Fault_Clear_Odometer = lifetime_statistics_ptr->Cycles_Completed;
	}
}

void StatisticsDish__IncreaseLowVoltageEventCounter(void)
{
	STATISTICS_LIFETIME_TYPE *lifetime_statistics_ptr = Statistics__GetLifetimeData();
	if (lifetime_statistics_ptr != NULL)
	{
		if (lifetime_statistics_ptr->Platform.Low_Voltage_Event_Counter < (uint16)__UNSIGNED_SHORT_MAX__)
		{
			lifetime_statistics_ptr->Platform.Low_Voltage_Event_Counter ++;
		}
	}

	if (Stats_Aux_Vars.Cycle_Low_Voltage_Event_Counter < (uint8)__UNSIGNED_CHAR_MAX__)
	{
		Stats_Aux_Vars.Cycle_Low_Voltage_Event_Counter ++;
	}
}

void StatisticsDish__IncreaseHighVoltageEventCounter(void)
{
	STATISTICS_LIFETIME_TYPE *lifetime_statistics_ptr = Statistics__GetLifetimeData();
	if (lifetime_statistics_ptr != NULL)
	{
		if (lifetime_statistics_ptr->Platform.High_Voltage_Event_Counter < (uint16)__UNSIGNED_SHORT_MAX__)
		{
			lifetime_statistics_ptr->Platform.High_Voltage_Event_Counter ++;
		}
	}

	if (Stats_Aux_Vars.Cycle_High_Voltage_Event_Counter < (uint8)__UNSIGNED_CHAR_MAX__)
	{
		Stats_Aux_Vars.Cycle_High_Voltage_Event_Counter ++;
	}
}

void StatisticsDish__UpdateVoltageRangeStatus(VOLTAGERANGE_STATE_TYPE voltage_range_status)
{
	STATISTICS_LIFETIME_TYPE *lifetime_statistics_ptr = Statistics__GetLifetimeData();
	if (lifetime_statistics_ptr != NULL)
	{
		lifetime_statistics_ptr->Platform.Voltage_Range_Status = voltage_range_status;
	}

	Stats_Aux_Vars.Cycle_Voltage_Range_Status = voltage_range_status;
}

static void UpdateCycleData(PLATFORM_CYCLE_TYPE *cycle)
{
	sint32 value;

	cycle->Water_Consumption_Cl = Stats_Aux_Vars.Water_Consumption_Cl;
	cycle->Water_Temperature_First_Fill = Stats_Aux_Vars.First_Fill_Data.Water_Temperature;
	cycle->Last_Heat_Final_Temperature = Stats_Aux_Vars.Heat_Final_Temperature;
	cycle->Last_Heat_Duration_Mins = Stats_Aux_Vars.Heat_Duration_Minutes;
	cycle->Sanitization_Result = Stats_Aux_Vars.Sanitization_Result;
	cycle->Flags.All = Stats_Aux_Vars.Flags.All;

	if ((uint8)Variable__GetValue(VARIABLE_SOIL_MEASURE_RECENT, 0, &value) == (uint8)TRUE)
	{
		cycle->Soil_Measure_Recent = (uint16)value;
	}

	if ((uint8)Variable__GetValue(SOIL_LEVEL_1, 0, &value) == (uint8)TRUE)
	{
		cycle->Soil_Level_1 = (uint8)value;
	}

	cycle->Cycle_Low_Voltage_Event_Counter = Stats_Aux_Vars.Cycle_Low_Voltage_Event_Counter;
	cycle->Cycle_High_Voltage_Event_Counter = Stats_Aux_Vars.Cycle_High_Voltage_Event_Counter;
	cycle->Cycle_Voltage_Range_Status = Stats_Aux_Vars.Cycle_Voltage_Range_Status;
}


static void UpdateServiceEnteredCycleOdometer(void)
{
	if (Selector__GetCycleGroup(0, COMPARTMENT_DEFAULT, Interpreter__GetCurrentStatement(COMPARTMENT_DEFAULT).Cycle) == CYCLE_GROUP_SERVICE)
	{
		STATISTICS_LIFETIME_TYPE *lifetime_statistics_ptr = Statistics__GetLifetimeData();
		if (lifetime_statistics_ptr != NULL)
		{
			lifetime_statistics_ptr->Platform.Service_Entered_Cycle_Odometer = lifetime_statistics_ptr->Cycles_Completed;
		}
	}
}

static void UpdateRinseAidResult(PLATFORM_CYCLE_TYPE *cycle)
{
	sint32 rinseaid_status;

	(void)Variable__GetValue(VARIABLE_RINSE_AID, 0,&rinseaid_status);
	cycle->RinseAid_Status = (uint8)rinseaid_status;
}


static void UpdateCyclesWithoutSalt(PLATFORM_CYCLE_TYPE *cycle)
{
	sint32 value;

	if ((uint8)Variable__GetValue(VARIABLE_SALT_ALARM_LED, 0, &value) == (uint8)TRUE)
	{
		STATISTICS_LIFETIME_TYPE *lifetime_statistics_ptr = Statistics__GetLifetimeData();

		if (lifetime_statistics_ptr != NULL)
		{
			if ((SALT_ALARM_LED_STATE_TYPE)value != (SALT_ALARM_LED_STATE_TYPE)SALT_ALARM_LED_STATE_OFF)
			{
				if (lifetime_statistics_ptr->Platform.Total_Cycles_Without_Salt < (uint16)__UNSIGNED_SHORT_MAX__)
				{
					lifetime_statistics_ptr->Platform.Total_Cycles_Without_Salt ++;
				}

				if (lifetime_statistics_ptr->Platform.Cycles_Since_Salt_Empty < (uint16)__UNSIGNED_SHORT_MAX__)
				{
					lifetime_statistics_ptr->Platform.Cycles_Since_Salt_Empty ++;
				}
			}
			else
			{
				lifetime_statistics_ptr->Platform.Cycles_Since_Salt_Empty = 0;
			}

			cycle->Cycles_Since_Salt_Empty = lifetime_statistics_ptr->Platform.Cycles_Since_Salt_Empty;
		}
	}
}

static void UpdateTotalCyclesInHCDLockout(void)
{
	STATISTICS_LIFETIME_TYPE *lifetime_statistics_ptr = Statistics__GetLifetimeData();

	if (lifetime_statistics_ptr != NULL)
	{
		if ((uint8)HeaterCalcificationDetectionMonitor__GetLockoutStatus() == (uint8)TRUE)
		{
			if (lifetime_statistics_ptr->Platform.Total_Cycles_In_HCD_Lockout < (uint16)__UNSIGNED_SHORT_MAX__)
			{
				lifetime_statistics_ptr->Platform.Total_Cycles_In_HCD_Lockout ++;
			}
		}
	}
}


static void UpdateDemoCycleDuration(PLATFORM_LIFETIME_TYPE *lifetime)
{
	BOOL_TYPE demo_active = FALSE;

	if (Regulations__GetMainValueByPosition(Regulations__GetPositionByFunctionID(FUNC_ID_DEMO, COMPARTMENT_DEFAULT)) > 0)
	{
		demo_active = TRUE;
	}

	if((uint8)demo_active == (uint8)TRUE)		// Demo reg activated
	{
		if (SystemTimers__NVCheckTimer(&NV_Demo_Active_Timer) == SYSTEMTIMERS_STATE_STOPPED)
		{
			(void)SystemTimers__NVSetTimerMS(&NV_Demo_Active_Timer, __UNSIGNED_LONG_MAX__, SYSTEMTIMERS_TYPE_UP_COUNTER);
		}
	}
	else
	{
		if (SystemTimers__NVCheckTimer(&NV_Demo_Active_Timer) == SYSTEMTIMERS_STATE_RUNNING)
		{
			SYSTEMTIMER_HMS_TYPE time;

			if (SystemTimers__NVGetTimerHMS(&NV_Demo_Active_Timer, &time) == PASS)
			{
				lifetime->Demo_Duration_Mins = time.minutes;
				lifetime->Demo_Duration_Mins += (uint16)time.hours * 60u;
			}

			SystemTimers__NVResetTimer(&NV_Demo_Active_Timer);
		}
	}
}


static void UpdateOWIStatus(PLATFORM_LIFETIME_TYPE *lifetime)
{
	sint32 owi_data;

	(void)Variable__GetValue(VARIABLE_OWI_CALIBRATION_STATUS, 0, &owi_data);

	if ((OWI_STATUS_TYPE)lifetime->OWI_Status != (OWI_STATUS_TYPE)owi_data)
	{
		STATISTICS_LIFETIME_TYPE *lifetime_statistics_ptr = Statistics__GetLifetimeData();

		if (lifetime_statistics_ptr != NULL)
		{
			lifetime->OWI_Current_Status_Odometer = lifetime_statistics_ptr->Cycles_Completed;
		}

		lifetime->OWI_Status = (OWI_STATUS_TYPE)owi_data;
	}
}


static void UpdateOptionsCount(void)
{
	STATISTICS_LIFETIME_TYPE *lifetime_statistics_ptr = Statistics__GetLifetimeData();
	if (lifetime_statistics_ptr != NULL)
	{
		uint8 index;

		for (index = 0; index < ELEMENTS_IN_ARRAY(Lifetime_Statistics_Options_Functions_IDs); index++)
		{
			if ((REGULATIONS_FUNCTION_ID_TYPE)Lifetime_Statistics_Options_Functions_IDs[index] != (REGULATIONS_FUNCTION_ID_TYPE)REGULATION_INVALID)
			{
				uint8 pos = Regulations__GetPositionByFunctionID(Lifetime_Statistics_Options_Functions_IDs[index], COMPARTMENT_DEFAULT);
				if (pos != (uint8)REGULATION_INVALID)
				{
					if (Regulations__GetIndexValueByPosition(pos) > (uint8)OPTION_INDEX_OFF)
					{
						if (lifetime_statistics_ptr->Platform.Options_Count[index] < (uint16)__UNSIGNED_SHORT_MAX__)
						{
							lifetime_statistics_ptr->Platform.Options_Count[index] ++;
						}
					}
				}
			}
		}
	}
}


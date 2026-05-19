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
/*
@startuml
        Title Fill State Machine

        [*] --> INITIALIZE

        INITIALIZE --> TIMED_VALVE_ACTIVATION : TimedFill
        INITIALIZE --> ACTIVATE_VALVE_NORMAL : NormalFill
        INITIALIZE --> COMPLETE : CheckInitConditionsTrue

		TIMED_VALVE_ACTIVATION: entry / TimedValveActivationEntry()
		TIMED_VALVE_ACTIVATION: do / CheckTimedConditions() \n ActivateValve()
		TIMED_VALVE_ACTIVATION --> COMPLETE : TimedConditionsToExitTrue
		TIMED_VALVE_ACTIVATION --> ACTIVATE_SUB_CYCLE : SubcycleRequested

		ACTIVATE_VALVE_NORMAL : entry / ActivateValveNormalEntry()
        ACTIVATE_VALVE_NORMAL : do / CheckConditions()  \n  / ActivateValve()
        ACTIVATE_VALVE_NORMAL --> ACTIVATE_VALVE_SLOW : NormalFillOver
        ACTIVATE_VALVE_NORMAL --> ACTIVATE_SUB_CYCLE : SubcycleRequested
        ACTIVATE_VALVE_NORMAL --> ACTIVATE_H2O_ALARM : H2OAlarm
		ACTIVATE_VALVE_NORMAL --> COMPLETE : CheckConditionsToExitTrue

        ACTIVATE_SUB_CYCLE : do / StartSubCycle()
        ACTIVATE_SUB_CYCLE --> ACTIVATE_VALVE_SLOW : WaterDetected
        ACTIVATE_SUB_CYCLE --> ACTIVATE_H2O_ALARM : H2OAlarm
        ACTIVATE_SUB_CYCLE --> COMPLETE : CompleteProcess

		ACTIVATE_H2O_ALARM: do / SetH2OAlarm()
		ACTIVATE_H2O_ALARM --> INITIALIZE : evInitialize

		ACTIVATE_VALVE_SLOW : entry / ActivateValveSlowEntry()
		ACTIVATE_VALVE_SLOW : do / CheckSlowConditions()  \n  / ActivateValve()
        ACTIVATE_VALVE_SLOW --> ACTIVATE_H2O_ALARM : H2OAlarm
		ACTIVATE_VALVE_SLOW --> COMPLETE : CheckConditionsToExitTrue


@enduml
*/
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "NormalFill.h"
#include "SystemConfig.h"

// -- Other Modules --
#include "DishAuxVar.h"
#include "Fill.h"
#include "Fault.h"
#include "Hbl.h"
#include "Log.h"
#include "Micro.h"
#include "Modifier.h"
#include "NVData.h"
#include "Regenerate.h"
#include "SaltSensingMonitor.h"
#include "Selector.h"
#include "SettingFile.h"
#include "StatisticsDish.h"
#include "SubCycle.h"
#include "Supervisor.h"
#include <string.h>
#include "Vwi.h"


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Fill State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Fill state machine.
typedef enum FILL_STATE_ENUM
{
    STATE_FILL_NONE                   = 0,

    STATE_FILL_INITIALIZE             = 1,          //!< (initial_state)
    STATE_FILL_TIMED_VALVE_ACTIVATION = 2,
    STATE_FILL_ACTIVATE_VALVE_NORMAL  = 3,
    STATE_FILL_COMPLETE               = 4,
    STATE_FILL_ACTIVATE_SUB_CYCLE     = 5,
    STATE_FILL_ACTIVATE_VALVE_SLOW    = 6,
    STATE_FILL_ACTIVATE_H2O_ALARM     = 7,

    STATE_FILL_END                    = 8
} FILL_STATE_TYPE;

//! The state variable for the Fill state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static FILL_STATE_TYPE Fill_State;

//! The transition variable for the Fill state machine.
//! Flag used by the state machine to track when a transition occurs in the middle of a state's
//! Entry() or Do() action that contains more than one function.
static BOOL_TYPE Fill_Transitioned;

// -- Private Function Prototypes used by the state machine for customization functions --
static void ActivateValve(void);
static void ActivateValveNormalEntry(void);
static void ActivateValveSlowEntry(void);
static void CheckConditions(void);
static void CheckSlowConditions(void);
static void CheckTimedConditions(void);
static void SetH2OAlarm(void);
static void StartSubCycle(void);
static void TimedValveActivationEntry(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventCheckConditionsToExitTrue(void);
static void EventCheckInitConditionsTrue(void);
static void EventCompleteProcess(void);
static void EventH2OAlarm(void);
static void EventInitialize(void);
static void EventNormalFill(void);
static void EventNormalFillOver(void);
static void EventSubcycleRequested(void);
static void EventTimedConditionsToExitTrue(void);
static void EventTimedFill(void);
static void EventWaterDetected(void);
static void FillExecute(void);
static void FillInitialize(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//! The name of this module for use by the Log module.
#define MODULE_NAME MODULE_NORMAL_FILL

//! Displacement value that indicates that the feature is not used.
#define UNUSED_FEATURE 0xFF

//! A list of log message IDs that are generated by this module.
typedef enum MODULE_NORMAL_FILL_LOG_MESSAGE_ID_ENUM
{
    ERROR_LOADING_PARAMETERS	= 1,
} MODULE_NORMAL_FILL_LOG_MESSAGE_ID_TYPE;

typedef enum VALVE_STATE_ENUM
{
    VALVE_STATE_OFF = 0,
    VALVE_STATE_ON
}VALVE_STATE_TYPE;

typedef PACKED struct FILL_PROCESS_PARAMETER_STRUCT
{
    uint8 Liters_Requested;
    uint8 Modifiers_Table_Index;
    uint8 Fill_Timeout_Normal;
    uint8 Fill_Timeout_Slow;
    uint16 Unused					:13;
    uint16 Check_For_Water			:1;
    uint16 Purge_Fill				:1;
    uint16 Partial_Fill				:1;
} FILL_PROCESS_PARAMETER_TYPE;

//! A structure to hold non-volatile data for the Fill process.
typedef struct FILL_NV_DATA_STRUCT
{
	uint16 Fill_Valve_Open_Time;
	uint16 First_Fill_Flow_Rate;
	uint16 Timed_Fill_Timeout;
	uint16 Liters_Timed_Fill_Old;

	uint16 Fill_Timeout_Timer;

	BOOL_TYPE Sub_Cycle_Executed;
	BOOL_TYPE H2O_Alarm;

	BOOL_TYPE Process_Completed;

	//! Fill state
    FILL_STATE_TYPE State;
} FILL_NV_DATA_TYPE;


static FILL_PARAMETER_TYPE* Fill_Parameters;
static FILL_PROCESS_PARAMETER_TYPE* Fill_Process_Parameters;

static uint16 CLiters_Requested;
static uint16 CLiters_Offset;
static uint16 CLiters_Max;
static uint16 CLValve_Closed_Threshold;
static uint16 CLSlow_Inlet_Threshold;
static uint16 CLAcceptable_Inlet_Threshold;
static uint8  FillCheckWaterSubCyclePriorty;
static uint16 Timed_Fill_Flow_Rate;
static BOOL_TYPE Do_Not_Reset_Backrinse;

#pragma location = "_appliance_segment"
//! The non-volatile data required by the Fill module.
static FILL_NV_DATA_TYPE Fill_Nv;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

static void ReadProcessParameters(PROCESS_CONTROL_TYPE* control);
static void CheckProcessCall(void);
static void CheckWater(void);


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------


/**
 * Initialize the NormalFill module's non-volatile data if non-volatile data was corrupted.
 */
void NormalFill__Initialize(void)
{
	if ((Micro__GetResetMode() == MICRO_RESET_MODE_COLD) &&
	    (NVData__GetLastRestoreStatus()->Bit.Appliance_Data_Recovered != (NVDATA_RESTORE_TYPE)NVDATA_RESTORE_OKAY))
	{
		memset(&Fill_Nv, 0, sizeof(Fill_Nv));
	}
}

PROCESS_RESULT_TYPE NormalFill__Manager(PROCESS_CONTROL_TYPE* control)
{
    PROCESS_RESULT_TYPE result;

	Fill_State = Fill_Nv.State;
	if (Fill_State == STATE_FILL_NONE)
	{
		// The Fill Process state machine has never been initialized -- initialize it.
		FillInitialize();
	}

	if (control->Step_Initializing != (BOOL_TYPE)FALSE)
	{
		// Initialize this instance of the Fill process.
		FillInitialize();
		ReadProcessParameters(control);

		Fill_Nv.Process_Completed = FALSE;
		CheckProcessCall();
	}
	else if ((control->Resume_From_Reset != (BOOL_TYPE)FALSE) || (control->Resume_From_Pause != (BOOL_TYPE)FALSE))
	{
		ReadProcessParameters(control);

		if  (control->Resume_From_Pause != (BOOL_TYPE)FALSE)
		{
			if (Fill_State == STATE_FILL_ACTIVATE_SUB_CYCLE)
			{
				if (((uint8)Fill_Nv.Sub_Cycle_Executed == (uint8)TRUE) && (control->Resume_From_Reset == (BOOL_TYPE)FALSE))
				{
					// Resuming from subcycle Fill Check Water
					CheckWater();
				}
			}
			else if (Fill_State == STATE_FILL_ACTIVATE_H2O_ALARM)
			{
				if ((uint8)Fill_Nv.H2O_Alarm == (uint8)TRUE)
				{
					// Resuming from PAUSE after setting H2O alarm
					EventInitialize();
					CheckProcessCall();
				}
			}
		}
	}
	else
	{
		FillExecute();
	}

	DishAuxVar__SetTimedFillRunning((BOOL_TYPE)(Fill_State == STATE_FILL_TIMED_VALVE_ACTIVATION));

	Fill_Nv.Sub_Cycle_Executed = FALSE;
	Fill_Nv.State = Fill_State;

	result = ((Fill_State == STATE_FILL_COMPLETE)? PROCESS_COMPLETE : PROCESS_BUSY);

	if(result == PROCESS_COMPLETE)
	{
		if(Fill_Nv.Process_Completed == FALSE)
		{
			StatisticsDish__UpdateFstFillWaterTemp();
			Fill_Nv.Process_Completed = TRUE;
		}
	}

    return result;
}


void NormalFill__SetFillSubcycleExecuted(uint8 compartment, uint8 subcycle_id, SUB_CYCLE_END_TYPE subcycle_end_code)
{
	if (Selector__GetCycleNameId(0,compartment , subcycle_id) == CYCLE_NAME_SPECIFIC_FILL_CHECK_WATER)
	{
		if ((subcycle_end_code == SUB_CYCLE_END_COMPLETED) || (subcycle_end_code == SUB_CYCLE_END_SET_STATE_PROCESS))
		{
			Fill_Nv.Sub_Cycle_Executed = TRUE;
		}
	}
}

void NormalFill__ClearFillValveFailure(void)
{
	Fill_Nv.H2O_Alarm = FALSE;
	H2O_Alarm = FALSE;
	Fill_Fault = (uint8)FALSE;

	if ((uint8)Fault__IsInFaultState(FAULT_ID_NO_WATER_FILL_VALVE_PROBLEM) == (uint8)TRUE)
	{
		Fault__ConditionRemoved(FAULT_ID_NO_WATER_FILL_VALVE_PROBLEM);
	}

	if ((uint8)Fault__IsInFaultState(FAULT_ID_NO_WATER) == (uint8)TRUE)
	{
		Fault__ConditionRemoved(FAULT_ID_NO_WATER);
	}
}


void NormalFill__ClearH2OAlarm(void)
{
	Fill_Nv.H2O_Alarm = FALSE;
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void ActivateValve(void)
{
	VALVE_STATE_TYPE valve_state;

	valve_state = VALVE_STATE_ON;
	(void)Hbl__SetLoadRequestByLoadID(HBL_LOAD_FILL_VALVE, HBL_PRIORITY_APPLICATION_HI, (uint8 *) &valve_state);	//lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]

	if (Do_Not_Reset_Backrinse == FALSE)
	{
		// Fill Process is activating Fill Valve: reset BACKRINSE_REQUIRED
		Regenerate__ClearBitmapBit(BACKRINSE_REQUIRED);
	}
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void CheckConditions(void)
{
	if (Fill_Nv.Fill_Timeout_Timer > 0U)
	{
		Fill_Nv.Fill_Timeout_Timer --;
	}

	if ((Fill_Nv.Fill_Timeout_Timer == 0U) ||
		(Liters_Current_Fill >= CLiters_Requested) ||
		(Liters_In_Tub >= CLiters_Max))
	{
		Fill_Nv.Fill_Valve_Open_Time = (Fill_Process_Parameters->Fill_Timeout_Normal * (uint16)10) - Fill_Nv.Fill_Timeout_Timer;

		if (Fill_Nv.Fill_Valve_Open_Time != (uint16)0)
		{
			// First_Fill_Flow_Rate expressed in cl/min
			Fill_Nv.First_Fill_Flow_Rate = (uint16)((Liters_Current_Fill * (uint16)600) / Fill_Nv.Fill_Valve_Open_Time);
		}

		if ((Liters_In_Tub >= CLiters_Max) ||
			((uint16)Fill_Process_Parameters->Purge_Fill == 1u))
		{
			// Go to C
			EventCheckConditionsToExitTrue();
		}
		else if ((Liters_Current_Fill >= CLiters_Requested)	||
				 (Fill_Nv.First_Fill_Flow_Rate >= CLValve_Closed_Threshold))
		{
			// Go to B
			if ((Fill_Nv.First_Fill_Flow_Rate >= CLSlow_Inlet_Threshold) ||
			    (Liters_Current_Fill >= CLiters_Requested))
			{
				if ((Liters_Current_Fill >= CLiters_Requested) || (Fill_Process_Parameters->Fill_Timeout_Slow == 0U))
				{
					// If at least one of these conditions is satisfied, the fill valve wouldn't be turned on in ACTIVATE_VALVE_SLOW state:
					// in this case the transition to ACTIVATE_VALVE_SLOW state is skipped and the conditions to be tested at the end of
					// ACTIVATE_VALVE_SLOW state are directly evaluated: by doing so, if the process is followed by a step with another
					// Fill Process, the fill valve is not turned off in between the two instances of the Fill Process.

					if (Liters_Current_Fill >= CLAcceptable_Inlet_Threshold)
					{
						// Go to A
						NormalFill__ClearFillValveFailure();
						EventCheckConditionsToExitTrue();
					}
					else
					{
						EventH2OAlarm();
					}
				}
				else
				{
					EventNormalFillOver();
				}
			}
			else
			{
				EventH2OAlarm();
			}
		}
		else
		{
			// Start subcycle if possible.
			if ((FillCheckWaterSubCyclePriorty != 0U) && (Interpreter__GetStatus().Sub_Cycle[COMPARTMENT_DEFAULT] == 0U))
			{
				EventSubcycleRequested();
			}
			else
			{
				// Fill Check Water SubCycle non present in setting file, or another subcycle is already running:
				// Fill Check Water Subcycle can't be run. Recovery action is to report an H2O Alarm
				EventH2OAlarm();
			}

		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void CheckProcessCall(void)
{
	Fill_Nv.Fill_Valve_Open_Time = 0;
	Fill_Nv.First_Fill_Flow_Rate = 0;
	Fill_Nv.Timed_Fill_Timeout = 0;

	if (((((uint16)Fill_Process_Parameters->Partial_Fill == 1u) && (Liters_In_Tub < CLiters_Max)) == FALSE) &&
		(((Liters_In_Tub >= (CLiters_Requested - CLiters_Offset)) && (Fill_Nv.H2O_Alarm == FALSE)) == TRUE))
	{
		// Go to A
		NormalFill__ClearFillValveFailure();
		EventCheckInitConditionsTrue();
	}
	else
	{
		if (Fill_Nv.H2O_Alarm == FALSE)
		{
			Liters_Current_Fill = CLiters_Offset;
			Liters_In_Tub = Liters_In_Tub + CLiters_Offset;
		}

		if ((Fault__IsInFaultState(FAULT_ID_FLOW_METER) == FALSE) && (Hbl__GetGIIndexByGIID(HBL_GI_WATER_COUNTER) != (uint8)HBL_INDEX_INVALID))
		{
			// Flow meter working
			EventNormalFill();
		}
		else
		{
			// Flow meter not working or absent
			EventTimedFill();
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void ReadProcessParameters(PROCESS_CONTROL_TYPE* control)
{
	SETTINGFILE_LOADER_TYPE loader;

	// Get the Fill process parameters from the setting file.
	if (SettingFile__MSettingBasicLoader(control->Setting_File_Index,
										 SF_PTR_ACU_PROCESS_FILL,
										 control->Data_Field_Index,
										 &loader) == FAIL)
	{
		LOG_ADD_EXCEPTION(ERROR_LOADING_PARAMETERS, control->Data_Field_Index);
	}
	else
	{
		// Read the setting file data into local variables for convenience.
		sint32 modified_value;
		Fill_Process_Parameters = (FILL_PROCESS_PARAMETER_TYPE*)(void *)loader.Data;

		if ((uint8)UNUSED_FEATURE != Fill_Process_Parameters->Modifiers_Table_Index)
		{
			modified_value = Modifier__GetValue(((sint32)(Fill_Process_Parameters->Liters_Requested) * 5) , Fill_Process_Parameters->Modifiers_Table_Index);
			if (modified_value < 0)
			{
				modified_value = 0;
			}

			CLiters_Requested = (uint16)modified_value;
		}
		else
		{
			CLiters_Requested = (Fill_Process_Parameters->Liters_Requested * (uint16)5); // in Cl
		}

		// Get the Fill parameters from the setting file machine structure.
		if ( FAIL == SettingFile__BasicLoader(SF_PTR_ACU_ALGORITHM_PARAMETERS,
											  DISPL_FILL,
											  &loader) )
		{
			LOG_ADD_EXCEPTION(ERROR_LOADING_PARAMETERS, (uint16)SF_PTR_ACU_ALGORITHM_PARAMETERS);
		}
		else
		{
			// Read the setting file data into local variables for convenience.
			Fill_Parameters = (FILL_PARAMETER_TYPE*)(void *)loader.Data;
			// in cl
			CLiters_Offset = Fill_Parameters->Liters_Offset * (uint16)2;
			CLiters_Max = Fill_Parameters->Liters_Max * (uint16)5;
			CLValve_Closed_Threshold = Fill_Parameters->Valve_Closed_Threshold * (uint16)5;
			CLSlow_Inlet_Threshold = Fill_Parameters->Slow_Inlet_Threshold * (uint16)5;
			CLAcceptable_Inlet_Threshold = (uint16)((CLiters_Requested * Fill_Parameters->Acceptable_Inlet_Threshold) / (uint16)100);
			FillCheckWaterSubCyclePriorty = Fill_Parameters->Fill_Check_Water_Sub_Cycle_Priorty;
			Timed_Fill_Flow_Rate = ENDIANU16_STON(Fill_Parameters->Timed_Fill_Flow_rate);
		}


		// Get the Do_Not_Reset_Backrinse_At_Fill parameters from the Regenerate setting file parameters.
		if ( FAIL == SettingFile__BasicLoader(SF_PTR_ACU_ALGORITHM_PARAMETERS,
											  DSPL_WATER_SOFTENER,
											  &loader) )
		{
			Do_Not_Reset_Backrinse = FALSE;
		}
		else
		{
			Do_Not_Reset_Backrinse = (((WATER_SOFTENER_SF_PARAMS_TYPE *)(void *)loader.Data)->Do_Not_Reset_Backrinse_At_Fill) ? (TRUE) : (FALSE);
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void ActivateValveNormalEntry(void)
{
	Fill_Nv.Fill_Timeout_Timer = (Fill_Process_Parameters->Fill_Timeout_Normal * (uint16)FILL_CALLS_PER_SECOND);

	if ((Fill_Nv.Fill_Timeout_Timer > 0U) &&
		(Liters_Current_Fill < CLiters_Requested) &&
		(Liters_In_Tub < CLiters_Max))
	{
		ActivateValve();
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void CheckSlowConditions(void)
{
	if (Fill_Nv.Fill_Timeout_Timer > 0U)
	{
		Fill_Nv.Fill_Timeout_Timer --;
	}

	if ((Fill_Nv.Fill_Timeout_Timer == 0U) ||
		(Liters_Current_Fill >= CLiters_Requested) ||
		(Liters_In_Tub >= CLiters_Max))
	{

		if (Liters_In_Tub >= CLiters_Max)
		{
			// Go to C
			EventCheckConditionsToExitTrue();
		}
		else if (Liters_Current_Fill >= CLAcceptable_Inlet_Threshold)
		{
			NormalFill__ClearFillValveFailure();
			EventCheckConditionsToExitTrue();
		}
		else
		{
			EventH2OAlarm();
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void CheckWater(void)
{
	VWI_WATER_SENSE_TYPE last_valid_water_status;

	last_valid_water_status = Vwi__GetLastValidWaterStatus();

	if (Hbl__GetGIIndexByGIID(HBL_GI_WATER_COUNTER) != (uint8)HBL_INDEX_INVALID)
	{
		// Flow meter present

		if (((last_valid_water_status == WS_WATER)||(last_valid_water_status == WS_UNSTABLE_WATER)) && ((uint16)Fill_Process_Parameters->Partial_Fill == 0U))
		{
			uint32 sum;
			uint16 water_since_last_regen = DishAuxVar__GetWaterLoadedSinceLastRegen();

			// FAULT F8E6
			Fill_Fault = 1U;
			Fault__ConditionDetected(FAULT_ID_FLOW_METER);

			// Since flow meter is not working, Liters_Current_Fill, Liters_In_Tub, Water_Counter and Water_Loaded_Since_Last_Regen
			// are updated adding to them the amount of water that is assumed to have been loaded in the current Fill instance.
			// in the current Fill instance (Flow Meter is supposed to be broken).

			// Fill_Process_Parameters->Fill_Timeout_Normal is expressed in s
			// Timed_Fill_Flow_Rate is expressed in (liters/s) * 10000 == (cl/s) * 100.
			// (Fill_Process_Parameters->Fill_Timeout_Normal * Timed_Fill_Flow_Rate) is expressed in s * (cl/s) * 100 = cl * 100.
			// ((Fill_Process_Parameters->Fill_Timeout_Normal * Timed_Fill_Flow_Rate)/100) is expressed in cl.
			Liters_Current_Fill = (uint16)((Fill_Process_Parameters->Fill_Timeout_Normal * Timed_Fill_Flow_Rate)/(uint8)100);
			Water_Counter += Liters_Current_Fill;
			Liters_In_Tub +=  Liters_Current_Fill;
			StatisticsDish__IncreaseWaterCounter(Liters_Current_Fill);

			sum = (uint32)water_since_last_regen + Liters_Current_Fill;
			if (sum > (uint16)__UNSIGNED_SHORT_MAX__)
			{
				sum = __UNSIGNED_SHORT_MAX__;
			}
			water_since_last_regen = (uint16)sum;

			DishAuxVar__SetWaterLoadedSinceLastRegen(water_since_last_regen);

			EventCompleteProcess(); // EXIT
		}
		else if ((last_valid_water_status == WS_WATER)||(last_valid_water_status == WS_UNSTABLE_WATER))
		{
			if ((Fill_Nv.First_Fill_Flow_Rate >= CLSlow_Inlet_Threshold) ||
				(Liters_Current_Fill >= CLiters_Requested))
			{
				EventWaterDetected();
			}
			else
			{
				EventH2OAlarm();
			}

		}
		else
		{
			EventH2OAlarm();
		}
	}
	else
	{
		// Flow meter absent

		if ((last_valid_water_status == WS_WATER)||(last_valid_water_status == WS_UNSTABLE_WATER))
		{
			// Water detected, end Process
			EventCompleteProcess();
		}
		else
		{
			// Water not detected, subtract the amount of water counted during Timed Fill

			uint16 water_since_last_regen = DishAuxVar__GetWaterLoadedSinceLastRegen();

			// Timed_Fill_Flow_Rate is expressed in (liters/s) * 10000 == (cl/s) * 100.
			// Fill_Nv.Fill_Valve_Open_Time is expressed in (s * FILL_CALLS_PER_SECOND)
			// Fill_Nv.Fill_Valve_Open_Time * Timed_Fill_Flow_Rate is expressed in (s * FILL_CALLS_PER_SECOND) * (cl/s) * 100 == cl * FILL_CALLS_PER_SECOND * 100.
			// (Fill_Nv.Fill_Valve_Open_Time * Timed_Fill_Flow_Rate)/(100 * FILL_CALLS_PER_SECOND) is expressed in cl.
			uint16 liters_to_be_subtracted = (uint16)((Fill_Nv.Fill_Valve_Open_Time * Timed_Fill_Flow_Rate)/(uint16)(100 * FILL_CALLS_PER_SECOND));

			if ( Liters_Current_Fill > liters_to_be_subtracted)
			{
				Liters_Current_Fill -= liters_to_be_subtracted;
			}
			else
			{
				Liters_Current_Fill = 0;
			}

			if ( Water_Counter > liters_to_be_subtracted)
			{
				Water_Counter -= liters_to_be_subtracted;
			}
			else
			{
				Water_Counter = 0;
			}
			if ( Liters_In_Tub > liters_to_be_subtracted)
			{
				Liters_In_Tub -= liters_to_be_subtracted;
			}
			else
			{
				Liters_In_Tub = 0;
			}
			if ( water_since_last_regen > liters_to_be_subtracted)
			{
				water_since_last_regen -= liters_to_be_subtracted;
			}
			else
			{
				water_since_last_regen = 0;
			}
			DishAuxVar__SetWaterLoadedSinceLastRegen(water_since_last_regen);

			StatisticsDish__DecreaseWaterCounter(liters_to_be_subtracted);

			EventH2OAlarm();
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void StartSubCycle(void)
{
	(void)SubCycle__RequestStart(0, FillCheckWaterSubCyclePriorty);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void CheckTimedConditions(void)
{
	uint16 liters_since_fill_Start;
	uint32 sum;
	uint16 water_since_last_regen = DishAuxVar__GetWaterLoadedSinceLastRegen();

	if (Fill_Nv.Fill_Timeout_Timer > 0U)
	{
		Fill_Nv.Fill_Timeout_Timer --;
	}

	Fill_Nv.Fill_Valve_Open_Time = Fill_Nv.Timed_Fill_Timeout - Fill_Nv.Fill_Timeout_Timer;

	// Timed_Fill_Flow_Rate is expressed in (liters/s) * 10000 == (cl/s) * 100.
	// Fill_Nv.Fill_Valve_Open_Time is expressed in (s * FILL_CALLS_PER_SECOND)
	// Fill_Nv.Fill_Valve_Open_Time * Timed_Fill_Flow_Rate is expressed in (s * FILL_CALLS_PER_SECOND) * (cl/s) * 100 == cl * FILL_CALLS_PER_SECOND * 100.
	// (Fill_Nv.Fill_Valve_Open_Time * Timed_Fill_Flow_Rate)/(100 * FILL_CALLS_PER_SECOND) is expressed in cl.
	liters_since_fill_Start = (uint16)((Fill_Nv.Fill_Valve_Open_Time * Timed_Fill_Flow_Rate)/(uint16)(100 * FILL_CALLS_PER_SECOND));

	Liters_Current_Fill +=  (liters_since_fill_Start - Fill_Nv.Liters_Timed_Fill_Old);
	Water_Counter += (liters_since_fill_Start - Fill_Nv.Liters_Timed_Fill_Old);
	Liters_In_Tub +=  (liters_since_fill_Start - Fill_Nv.Liters_Timed_Fill_Old);
	StatisticsDish__IncreaseWaterCounter(liters_since_fill_Start - Fill_Nv.Liters_Timed_Fill_Old);

	sum = (uint32)water_since_last_regen + ((uint32)liters_since_fill_Start - Fill_Nv.Liters_Timed_Fill_Old);
	if (sum > (uint16)__UNSIGNED_SHORT_MAX__)
	{
		sum = __UNSIGNED_SHORT_MAX__;
	}
	water_since_last_regen = (uint16)sum;

	DishAuxVar__SetWaterLoadedSinceLastRegen(water_since_last_regen);

	Fill_Nv.Liters_Timed_Fill_Old = liters_since_fill_Start;

	if ((Fill_Nv.Fill_Timeout_Timer == 0U) ||
		(Liters_In_Tub >= CLiters_Max))
	{
		if ((Hbl__GetGIIndexByGIID(HBL_GI_WATER_COUNTER) != (uint8)HBL_INDEX_INVALID) || ((uint16)Fill_Process_Parameters->Check_For_Water == 0U))
		{
			EventTimedConditionsToExitTrue(); // Go to C
		}
		else
		{
			// Flow meter absent and Check_For_Water == TRUE

			// Start subcycle if possible.
			if ((FillCheckWaterSubCyclePriorty != 0U) && (Interpreter__GetStatus().Sub_Cycle[COMPARTMENT_DEFAULT] == 0U))
			{
				EventSubcycleRequested();
			}
			else
			{
				// Fill Check Water SubCycle non present in setting file, or another subcycle is already running:
				// Fill Check Water Subcycle can't be run. Recovery action is to end Process.
				EventTimedConditionsToExitTrue(); // Go to C
			}
			// Go to D
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void TimedValveActivationEntry(void)
{
	if (Timed_Fill_Flow_Rate != 0U)
	{
		// Timed_Fill_Flow_Rate is expressed in (liters/s) * 10000 == (cl/s) * 100.
		// CLiters_Requested is expressed in cl.
		// (CLiters_Requested/Timed_Fill_Flow_Rate) is expressed in cl / ((cl/s) * 100) == s/100.
		// ((CLiters_Requested * 100 * FILL_CALLS_PER_SECOND)/Timed_Fill_Flow_Rate) is expressed in s/100 * 100 * FILL_CALLS_PER_SECOND == s * FILL_CALLS_PER_SECOND
		Fill_Nv.Timed_Fill_Timeout = (uint16)((CLiters_Requested * (uint8)100 * (uint8)FILL_CALLS_PER_SECOND)/Timed_Fill_Flow_Rate);
	}

	Fill_Nv.Fill_Timeout_Timer = Fill_Nv.Timed_Fill_Timeout;
	Fill_Nv.Liters_Timed_Fill_Old = 0;

	if ((Fill_Nv.Fill_Timeout_Timer > 0U) &&
		(Liters_In_Tub < CLiters_Max))
	{
		ActivateValve();
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetH2OAlarm(void)
{
	Fill_Nv.H2O_Alarm = TRUE;
	H2O_Alarm = TRUE;

	if (Fault__IsInFaultState(FAULT_ID_NO_WATER_FILL_VALVE_PROBLEM) == FALSE)
	{
		Fault__ConditionDetected(FAULT_ID_NO_WATER);
	}
	(void)Supervisor__SetStart(0, FS_PAUSE_CONDITION);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void ActivateValveSlowEntry(void)
{
	Fill_Nv.Fill_Timeout_Timer = (Fill_Process_Parameters->Fill_Timeout_Slow * (uint16)FILL_CALLS_PER_SECOND);

	if ((Fill_Nv.Fill_Timeout_Timer > 0U) &&
		(Liters_Current_Fill < CLiters_Requested) &&
		(Liters_In_Tub < CLiters_Max))
	{
		ActivateValve();
	}
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Fill State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Check Conditions To Exit True Event for the Fill state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventCheckConditionsToExitTrue(void)
{
    switch (Fill_State)
    {
        case STATE_FILL_ACTIVATE_VALVE_NORMAL:
        case STATE_FILL_ACTIVATE_VALVE_SLOW:
            Fill_State = STATE_FILL_COMPLETE;
            Fill_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Fill_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Check Init Conditions True Event for the Fill state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventCheckInitConditionsTrue(void)
{
    switch (Fill_State)
    {
        case STATE_FILL_INITIALIZE:
            Fill_State = STATE_FILL_COMPLETE;
            Fill_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Fill_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Complete Process Event for the Fill state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventCompleteProcess(void)
{
    switch (Fill_State)
    {
        case STATE_FILL_ACTIVATE_SUB_CYCLE:
            Fill_State = STATE_FILL_COMPLETE;
            Fill_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Fill_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the H2O Alarm Event for the Fill state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventH2OAlarm(void)
{
    switch (Fill_State)
    {
        case STATE_FILL_ACTIVATE_VALVE_NORMAL:
        case STATE_FILL_ACTIVATE_SUB_CYCLE:
        case STATE_FILL_ACTIVATE_VALVE_SLOW:
            Fill_State = STATE_FILL_ACTIVATE_H2O_ALARM;
            Fill_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Fill_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Initialize Event for the Fill state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventInitialize(void)
{
    switch (Fill_State)
    {
        case STATE_FILL_ACTIVATE_H2O_ALARM:
            Fill_State = STATE_FILL_INITIALIZE;
            Fill_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Fill_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Normal Fill Event for the Fill state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNormalFill(void)
{
    switch (Fill_State)
    {
        case STATE_FILL_INITIALIZE:
            Fill_State = STATE_FILL_ACTIVATE_VALVE_NORMAL;
            ActivateValveNormalEntry();             // Entry Function.
            Fill_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Fill_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Normal Fill Over Event for the Fill state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNormalFillOver(void)
{
    switch (Fill_State)
    {
        case STATE_FILL_ACTIVATE_VALVE_NORMAL:
            Fill_State = STATE_FILL_ACTIVATE_VALVE_SLOW;
            ActivateValveSlowEntry();               // Entry Function.
            Fill_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Fill_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Subcycle Requested Event for the Fill state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventSubcycleRequested(void)
{
    switch (Fill_State)
    {
        case STATE_FILL_TIMED_VALVE_ACTIVATION:
        case STATE_FILL_ACTIVATE_VALVE_NORMAL:
            Fill_State = STATE_FILL_ACTIVATE_SUB_CYCLE;
            Fill_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Fill_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Timed Conditions To Exit True Event for the Fill state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventTimedConditionsToExitTrue(void)
{
    switch (Fill_State)
    {
        case STATE_FILL_TIMED_VALVE_ACTIVATION:
            Fill_State = STATE_FILL_COMPLETE;
            Fill_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Fill_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Timed Fill Event for the Fill state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventTimedFill(void)
{
    switch (Fill_State)
    {
        case STATE_FILL_INITIALIZE:
            Fill_State = STATE_FILL_TIMED_VALVE_ACTIVATION;
            TimedValveActivationEntry();            // Entry Function.
            Fill_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Fill_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Water Detected Event for the Fill state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventWaterDetected(void)
{
    switch (Fill_State)
    {
        case STATE_FILL_ACTIVATE_SUB_CYCLE:
            Fill_State = STATE_FILL_ACTIVATE_VALVE_SLOW;
            ActivateValveSlowEntry();               // Entry Function.
            Fill_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Fill_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Fill state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void FillExecute(void)
{
    Fill_Transitioned = FALSE;
    switch(Fill_State)
    {
        case STATE_FILL_TIMED_VALVE_ACTIVATION:
            CheckTimedConditions();                 // Do function.
            if (Fill_Transitioned == FALSE)
            {
                ActivateValve();                    // Do function.
            }
            break;

        case STATE_FILL_ACTIVATE_VALVE_NORMAL:
            CheckConditions();                      // Do function.
            if (Fill_Transitioned == FALSE)
            {
                ActivateValve();                    // Do function.
            }
            break;

        case STATE_FILL_ACTIVATE_SUB_CYCLE:
            StartSubCycle();                        // Do function.
            break;

        case STATE_FILL_ACTIVATE_VALVE_SLOW:
            CheckSlowConditions();                  // Do function.
            if (Fill_Transitioned == FALSE)
            {
                ActivateValve();                    // Do function.
            }
            break;

        case STATE_FILL_ACTIVATE_H2O_ALARM:
            SetH2OAlarm();                          // Do function.
            break;

        default:
            // There is nothing to do for all other values of Fill_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Fill state machine.
 * This function resets the state machine to the initial state.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void FillInitialize(void)
{
    Fill_State = STATE_FILL_INITIALIZE;
    Fill_Transitioned = TRUE;
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

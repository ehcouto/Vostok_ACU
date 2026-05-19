/**
 *  @file		HeaterShadowing.c
 *
 *  @brief		A task that implements the Heater Shadowing Detection algorithm
 *
 *  @details	The configuration of loads inside the dishwasher can cause some "dry spots" in the heater when washing
 *  			using certain spray arms. If heater is turned on for some time with one or multiple dry spots, it can
 *  			dramatically increase its temperature to levels that could shorten the heater life. The objective of this
 *  			algorithm is to detect when the heater has a dry spot by monitoring the heater current sensor.
 *
   @startuml{HeaterShadowing State Machine.jpg}
        title Heater Shadowing State Machine

		[*] --> INITIALIZE

		INITIALIZE : entry / InitializeEntry()
		INITIALIZE --> HOLD : evStartHeaterShadowingAlgorithm
        INITIALIZE -r-> [*] : evHeaterShadowingNotActive \n/ SetAlgorithmNotActive()

		HOLD: entry / HoldEntry()
		HOLD: do / HoldDo()
		HOLD --> HOLD : eventCycleResume
		HOLD --> HEATER_OFF : eventHoldTimeExpired \n[!HeaterIsOn()]
		HOLD --> UNKNOWN_POSITION : eventHoldTimeExpired \n[HeaterIsOn() && (!DiverterInLSAOrMSA())]
		HOLD --> WAITING_DELAY : eventHoldTimeExpired \n[HeaterIsOn() && DiverterInLSAOrMSA()]

		HEATER_OFF : entry / HeaterOffEntry()
		HEATER_OFF : do / HeaterOffDo()
		HEATER_OFF --> HOLD : eventCycleResume
		HEATER_OFF --> UNKNOWN_POSITION : evHeaterOn \n[!DiverterInLSAOrMSA()]
		HEATER_OFF --> WAITING_DELAY : evHeaterOn \n[DiverterInLSAOrMSA()]

		UNKNOWN_POSITION : entry / UnknownPositionEntry()
		UNKNOWN_POSITION : do / UnknownPositionDo()
		UNKNOWN_POSITION --> HOLD : eventCycleResume
		UNKNOWN_POSITION --> HEATER_OFF : evHeaterOff
		UNKNOWN_POSITION --> WAITING_DELAY : evDiverterPositionChanged

		WAITING_DELAY : entry / WaitingDelayEntry()
		WAITING_DELAY : do / WaitingDelayDo()
		WAITING_DELAY --> HOLD : eventCycleResume
		WAITING_DELAY --> HEATER_OFF : evHeaterOff
		WAITING_DELAY --> UNKNOWN_POSITION : evDiverterPositionUnknown
		WAITING_DELAY --> WAITING_DELAY : evDiverterPositionChanged
		WAITING_DELAY --> CHECK_DELTA : evDelayExpired \n/ SaveCurrentBaseline()

		CHECK_DELTA : entry / CheckDeltaEntry()
		CHECK_DELTA : do / CheckDeltaDo()
		CHECK_DELTA --> HOLD : eventCycleResume
		CHECK_DELTA --> HEATER_OFF : evHeaterOff
		CHECK_DELTA --> UNKNOWN_POSITION : evDiverterPositionUnknown
		CHECK_DELTA --> WAITING_DELAY : evDiverterPositionChanged
		CHECK_DELTA --> TIMEOUT : evTimeoutExpired

		TIMEOUT : entry / TimeoutEntry()
		TIMEOUT : do / TimeoutDo()
		TIMEOUT --> HOLD : eventCycleResume
		TIMEOUT --> HEATER_OFF : evHeaterOff
		TIMEOUT --> UNKNOWN_POSITION : evDiverterPositionUnknown
		TIMEOUT --> WAITING_DELAY : evDiverterPositionChanged


    @enduml
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "HeaterShadowing.h"

// -- Other Modules --
#include "DishAuxVar.h"
#include "Fault.h"
#include "Hbl.h"
#include "HeatMonitor.h"
#include "Log.h"
#include "Micro.h"
#include "Selector.h"
#include "SettingFile.h"
#include <string.h>
#include "SystemTimers.h"
#include "WashHeaterFaultMonitor.h"


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Heater Shadowing State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Heater Shadowing state machine.
typedef enum HEATER_SHADOWING_STATE_ENUM
{
    STATE_HEATER_SHADOWING_NONE             = 0,

    STATE_HEATER_SHADOWING_INITIALIZE       = 1,    //!< (initial_state)
    STATE_HEATER_SHADOWING_HOLD             = 2,
    STATE_HEATER_SHADOWING_HEATER_OFF       = 3,
    STATE_HEATER_SHADOWING_UNKNOWN_POSITION = 4,
    STATE_HEATER_SHADOWING_WAITING_DELAY    = 5,
    STATE_HEATER_SHADOWING_CHECK_DELTA      = 6,
    STATE_HEATER_SHADOWING_TIMEOUT          = 7,

    STATE_HEATER_SHADOWING_END              = 8
} HEATER_SHADOWING_STATE_TYPE;

//! The state variable for the Heater Shadowing state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static HEATER_SHADOWING_STATE_TYPE Heater_Shadowing_State;

// -- Private Function Prototypes used by the state machine for customization functions --
static void CheckDeltaDo(void);
static void CheckDeltaEntry(void);
static BOOL_TYPE DiverterInLSAOrMSA(void);
static BOOL_TYPE HeaterIsOn(void);
static void HeaterOffDo(void);
static void HeaterOffEntry(void);
static void HoldDo(void);
static void HoldEntry(void);
static void InitializeEntry(void);
static void SaveCurrentBaseline(void);
static void SetAlgorithmNotActive(void);
static void TimeoutDo(void);
static void TimeoutEntry(void);
static void UnknownPositionDo(void);
static void UnknownPositionEntry(void);
static void WaitingDelayDo(void);
static void WaitingDelayEntry(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventCycleResume(void);
static void EventDelayExpired(void);
static void EventDiverterPositionChanged(void);
static void EventDiverterPositionUnknown(void);
static void EventHeaterOff(void);
static void EventHeaterOn(void);
static void EventHeaterShadowingNotActive(void);
static void EventHoldTimeExpired(void);
static void EventStartHeaterShadowingAlgorithm(void);
static void EventTimeoutExpired(void);
static void HeaterShadowingExecute(void);
static void HeaterShadowingInitialize(void);
static void HeaterShadowingTerminate(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.



//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
HEATER_SHADOWING_STATUS_TYPE HeaterShadowing_Status;


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//! The name of this module to be used by the Log module.
#define MODULE_NAME MODULE_HEATER_SHADOWING

//! A list of log message IDs that are generated by this module.
typedef enum MODULE_HEATER_SHADOWING_LOG_MESSAGE_ID_ENUM
{
	ERROR_MISSING_COMPONENTS  = 1,
	ERROR_MISSING_PARAMETERS
} MODULE_HEATER_SHADOWING_LOG_MESSAGE_ID_TYPE;

//! Possible values of the variable HS_Diverter_Status
typedef enum HS_DIVERTER_STATUS_ENUM
{
	DIVERTER_STATUS_UNKNOWN = 0,
	DIVERTER_STATUS_LSA,
	DIVERTER_STATUS_MSA

} HS_DIVERTER_STATUS_TYPE;

//! Setting file data structure for pointer SF_PTR_ACU_PRODUCT_CONFIGURATION_DATA, displacement DSPL_HEATER_SHADOWING_PARAMS
typedef PACKED struct HEATER_SHADOWING_SF_PARAMS_STRUCT
{
	uint16 LSA_Rise_Threshold;
	uint16 MSA_Drop_Threshold;
	uint16 Timeout;
	uint8 LSA_Delay;
	uint8 LSA_Position_Value :4;
	uint8 MSA_Position_Value :4;
	uint8 MSA_Delay;
	uint8 Stability_Time;
	uint16 Algorithm_Hold_Time;
} HEATER_SHADOWING_SF_PARAMS_TYPE;

//! Macros to manage setting file parameters
#define LSA_RISE_THRESHOLD 		ENDIANU16_STON(Heater_Shadowing_Data.SF_Heater_Shadowing_Params_Ptr->LSA_Rise_Threshold)
#define MSA_DROP_THRESHOLD 		ENDIANU16_STON(Heater_Shadowing_Data.SF_Heater_Shadowing_Params_Ptr->MSA_Drop_Threshold)
#define CHECK_TIMEOUT_MS		((uint32)ENDIANU16_STON(Heater_Shadowing_Data.SF_Heater_Shadowing_Params_Ptr->Timeout) * (uint16)1000)
#define LSA_DELAY_MS			((uint32)Heater_Shadowing_Data.SF_Heater_Shadowing_Params_Ptr->LSA_Delay * (uint16)1000)
#define MSA_DELAY_MS			((uint32)Heater_Shadowing_Data.SF_Heater_Shadowing_Params_Ptr->MSA_Delay * (uint16)1000)
#define STABILITY_TIME_MS		((uint32)Heater_Shadowing_Data.SF_Heater_Shadowing_Params_Ptr->Stability_Time * (uint16)1000)
#define ALGORITHM_HOLD_TIME_MS 	((uint32)ENDIANU16_STON(Heater_Shadowing_Data.SF_Heater_Shadowing_Params_Ptr->Algorithm_Hold_Time) * (uint16)1000)


typedef struct HEATERS_SHADOWING_DATA_STRUCT
{
	const HEATER_SHADOWING_SF_PARAMS_TYPE *SF_Heater_Shadowing_Params_Ptr; //! Pointer to the setting file parameters
	SYSTEMTIMER_NV_TIMER_TYPE NV_Timer_Heater_Shadowing; //! Timer used to manage hold time, delay time and timeout
	SYSTEMTIMER_NV_TIMER_TYPE NV_Timer_HS_Stability;	 //! Timer used to detect if the shadowed condition is stable
	ON_OFF_TYPE Heater_Status;		//! Status of the heater
	HS_DIVERTER_STATUS_TYPE Diverter_Status;			//! Status (position) of the diverter
	HS_DIVERTER_STATUS_TYPE Previous_Diverter_Status;	//! Status (position) of the diverter in the previous iteration
	uint16 Heater_Current; 			//! The current on the heater as measured by the Heater Current Sensing circuit (units mA)
	uint16 Current_Baseline_Value;	//! The Current value stored when the delay has expired. It is used to be compared to current readings to determine if heater is shadowed or not (units mA)
	uint16 AC_Line_Lower_Threshold;	//! A threshold against which the current AC Line value is compared to know if the Zero Crossing signal is reliable
} HEATERS_SHADOWING_DATA_TYPE;

//! Data structure for the algorithm's management
static HEATERS_SHADOWING_DATA_TYPE Heater_Shadowing_Data;


//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
void UpdateAlgorithmInputs(void);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//----------------------------------------------------------------------------
/**
 * It initializes the Heater Shadowing Detection task and its data.
 *
 * @param  compartment = compartment inside which the task is running
 * @param  sf_index = the index of the setting file from which to load the task data
 * @param  displacement = the displacement for the task data in the setting file (unused, since all the instances
 * 		   of the Door Opening System Push Task use the same data table).
 * @return the status of the task (ACTIVE or INACTIVE)
 *
 */
ACTIVE_TYPE HeaterShadowing__Initialize(uint8 compartment, uint8 sf_index, uint8 displacement)
{
	HeaterShadowingInitialize();

    return (ACTIVE);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Function called during the execution of the Heater Shadowing Detection task.
 *
 * @param  compartment = compartment inside which the task is running
 * @return the status of the task (ACTIVE or INACTIVE)
 */
ACTIVE_TYPE HeaterShadowing__Task(uint8 compartment)
{
	UpdateAlgorithmInputs();
	HeaterShadowingExecute();
	Heater_Shadowing_Data.Previous_Diverter_Status = Heater_Shadowing_Data.Diverter_Status;

    return (ACTIVE);
}


/**
 * It is called at the initialization of the task after NVdata restore.
 * It restarts the Heater Shadowing Detection algorithm.
 *
 * @param  compartment = compartment inside which the task is running
 *
 * @return  None
 */
void HeaterShadowing__NVDataRestore(uint8 compartment)
{
	// The algorithm has to be reinitialized in case of power loss
	HeaterShadowingInitialize();
}


/**
 * It is called when the task ends
 *
 * @param  compartment = compartment inside which the task is running
 *
 * @return  None
 */
void HeaterShadowing__Terminate(uint8 compartment)
{
	SetAlgorithmNotActive();
	HeaterShadowingTerminate();
}


/**
 * It is called after a reset. It sets HeaterShadowing_Status to its default value ALGORITHM_NOT_ACTIVE.
 * If the task is active, it will be overwritten by HeaterShadowing__NVDataRestore().
 *
 * @return  None
 */
void HeaterShadowing__StatusInitialize(void)
{
	if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
	{
		SetAlgorithmNotActive();
	}
}


/**
 * It is called when the cycle resumes from PAUSE state.
 * If the algorithm is running (Heater_Shadowing_State != STATE_HEATER_SHADOWING_NONE) it reinitializes it.
 *
 * @return  None
 */
void HeaterShadowing__ResumeCycle(void)
{
	EventCycleResume();
}

/**
 * It is called when a subcycle ends.
 * If the ended subcycle is Cycle Resume, it reinitializes the Heater Shadowing Detection algorithm.
 *
 * @param  compartment = the compartment inside which the subcycle was running.
 * @param  subcycle_id = the sub-cycle that has finished or was cancelled.
 *
 * @return  None
 */
void HeaterShadowing__CycleResumeEnd(uint8 compartment,uint8 subcycle_id)
{
	if (Selector__GetCycleNameId(0, compartment , subcycle_id) == CYCLE_NAME_SPECIFIC_CYCLE_RESUME)
	{
		EventCycleResume();
	}
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

/**
 * It updates the inputs to the algorithm: Heater_Status, Diverter_Status, Heater_Current.
 * It is called before the state machine execution.
 *
 * @return  None
 */
void UpdateAlgorithmInputs(void)
{
	// Heater status
	{
		HBL_LOAD_STATUS_TYPE *heater_status_ptr = (HBL_LOAD_STATUS_TYPE *)Hbl__GetLoadStatusByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_WASHING_HEATER));
		if (heater_status_ptr != NULL)
		{
			if ((heater_status_ptr->Load.Status == (HBL_LOAD_STATUS_ENUM_TYPE)HBL_LOAD_STATUS_TARGET_COMPLETED) && (heater_status_ptr->Load.Cur_Reg != (uint8)OFF))
			{
				Heater_Shadowing_Data.Heater_Status = ON;
			}
			else
			{
				Heater_Shadowing_Data.Heater_Status = OFF;
			}
		}
	}

	// Diverter Status
	if (((uint8)Fault__IsInFaultState(FAULT_ID_DIVERTER_CANNOT_FIND_POSITIONS) == (uint8)TRUE) ||
		((uint8)Fault__IsInFaultState(FAULT_ID_DIVERTER_LEAK) == (uint8)TRUE) ||
		((uint8)Fault__IsInFaultState(FAULT_ID_DIVERTER_TRIAC_SHORTED) == (uint8)TRUE) ||
		((DishAuxVar__GetACLine() <= Heater_Shadowing_Data.AC_Line_Lower_Threshold) || (Hbl__IsZeroCrossPresent() == FALSE)))
	{
		// As per requirements, if a fault has been detected regarding the diverter, the algorithm
		// shall manage the situation as if the diverter's position was unknown.
		// Moreover, the heater's current can't be read if the Zero Crossing signal is absent (since
		// peak-to-peak signals are read in synchronization with the Zero Crossing), and again this
		// case is managed as if the diverter position was unknown.
		Heater_Shadowing_Data.Diverter_Status = DIVERTER_STATUS_UNKNOWN;
	}
	else
	{
		if (Heater_Shadowing_Data.SF_Heater_Shadowing_Params_Ptr != NULL)
		{
			Diverter_Position_Type diverter_position = DishAuxVar__GetDiverterPosition();

			if ((uint8)diverter_position.Is_Stable == (uint8)TRUE)
			{
				// Diverter has reached the requested position
				if (diverter_position.Position == Heater_Shadowing_Data.SF_Heater_Shadowing_Params_Ptr->LSA_Position_Value)
				{
					Heater_Shadowing_Data.Diverter_Status = DIVERTER_STATUS_LSA;
				}
				else if (diverter_position.Position == Heater_Shadowing_Data.SF_Heater_Shadowing_Params_Ptr->MSA_Position_Value)
				{
					Heater_Shadowing_Data.Diverter_Status = DIVERTER_STATUS_MSA;
				}
				else
				{
					Heater_Shadowing_Data.Diverter_Status = DIVERTER_STATUS_UNKNOWN;
				}
			}
			else
			{
				Heater_Shadowing_Data.Diverter_Status = DIVERTER_STATUS_UNKNOWN;
			}
		}
	}

	// Heater Current
	{
		uint8 heater_current_index;
		uint8 heater_current_sequence_id;

		heater_current_index = Hbl__GetGIIndexByGIID(HBL_GI_I_HEATER);
		heater_current_sequence_id = Hbl__GetGISequenceIDByIndex(heater_current_index);

		if (heater_current_sequence_id > (uint8)0)
		{
			Heater_Shadowing_Data.Heater_Current = (uint16)Hbl__GetGIDataConvertedByIndex(heater_current_index);
		}
	}
}



//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void InitializeEntry(void)
{
    SETTINGFILE_LOADER_TYPE loader;
    BOOL_TYPE activate_algorithm;

    activate_algorithm = FALSE;

    memset(&Heater_Shadowing_Data, 0, sizeof(Heater_Shadowing_Data));
    Heater_Shadowing_Data.AC_Line_Lower_Threshold = 0xFFFF;
	SystemTimers__NVResetTimer(&Heater_Shadowing_Data.NV_Timer_Heater_Shadowing);
	SystemTimers__NVResetTimer(&Heater_Shadowing_Data.NV_Timer_HS_Stability);

	if ((Hbl__GetLoadIndexByLoadID(HBL_LOAD_WASHING_HEATER) != (uint8)HBL_INDEX_INVALID) &&
		(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DIVERTER) != (uint8)HBL_INDEX_INVALID) &&
		(Hbl__GetGIIndexByGIID(HBL_GI_I_HEATER) != (uint8)HBL_INDEX_INVALID) &&
		(Hbl__GetGIIndexByGIID(HBL_GI_AC_LINE) != (uint8)HBL_INDEX_INVALID))
	{
		// Get the Heat Monitor process parameters from the setting file, to read the heater's type.
		if (SettingFile__BasicLoader(SF_PTR_ACU_PRODUCT_CONFIGURATION_DATA,	DISPL_HEATER_DEACTIVATION_CONDITIONS_PARAM, &loader) == PASS)
		{
			if (((HEAT_MONITOR_INSTANCE_TYPE *)(void*)loader.Data)->Heater_Type == (uint8)CALROD_HEATER) 
			{
				// Calrod Heater type

				if (SettingFile__BasicLoader(SF_PTR_ACU_PRODUCT_CONFIGURATION_DATA, DSPL_HEATER_CURRENT_SENSING_PARAMS, &loader) == PASS)
				{
					Heater_Shadowing_Data.AC_Line_Lower_Threshold = ENDIANU16_STON(((HEATER_CURRENT_SENSING_PARAMS_TYPE *)(void*)loader.Data)->AC_Line_Lower_Threshold); 
					// Get the Heater Shadowing parameters from the setting file.
					if (SettingFile__BasicLoader(SF_PTR_ACU_ALGORITHM_PARAMETERS, DSPL_HEATER_SHADOWING, &loader) == PASS)
					{
						Heater_Shadowing_Data.SF_Heater_Shadowing_Params_Ptr = (HEATER_SHADOWING_SF_PARAMS_TYPE *)(void*)loader.Data; 

					    activate_algorithm = TRUE;
					}
					else
					{
						// Algorithm parameters missing
						LOG_ADD_EXCEPTION(ERROR_MISSING_PARAMETERS, DSPL_HEATER_SHADOWING);
					}
				}
				else
				{
					// It is not possible to know, through the AC Line input, whether the Zero Crossing signal is reliable or not
					LOG_ADD_EXCEPTION(ERROR_MISSING_PARAMETERS, DSPL_HEATER_CURRENT_SENSING_PARAMS);
				}
			}
			else
			{
				// Heater shadowing algorithm doesn't apply to Integrated Type Heater.
			}
		}
		else
		{
			// It is not possible to know the heater's type
			LOG_ADD_EXCEPTION(ERROR_MISSING_PARAMETERS, DISPL_HEATER_DEACTIVATION_CONDITIONS_PARAM);
		}
	}
	else
	{
		// One or more of the input on which the algorithm depends are not configured in setting file:
		// - Washing Heater state, Diverter position and Heater Current are directly used by the algorithm.
		// - AC Line is used because it is a robust feedback to know if Zero Crossing signal (without which
		//   the reading of the Heater Current is not reliable) is reliable. It is not safe to just check the
		//   Zero Crossing presence, because it could be due to the effect of disturbances even if the pilot
		//   relay is open.
		LOG_ADD_EXCEPTION (ERROR_MISSING_COMPONENTS, 0);
	}

	if ((uint8)activate_algorithm == (uint8)TRUE)
	{
		EventStartHeaterShadowingAlgorithm();
	}
	else
	{
		EventHeaterShadowingNotActive();
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void HoldEntry(void)
{
	HeaterShadowing_Status = ALGORITHM_ON_HOLD;
	(void)SystemTimers__NVSetTimerMS(&Heater_Shadowing_Data.NV_Timer_Heater_Shadowing, ALGORITHM_HOLD_TIME_MS, SYSTEMTIMERS_TYPE_UP_COUNTER);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void HoldDo(void)
{
	if (SystemTimers__NVCheckTimer(&Heater_Shadowing_Data.NV_Timer_Heater_Shadowing) == SYSTEMTIMERS_STATE_DONE)
	{
		EventHoldTimeExpired();
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void HeaterOffEntry(void)
{
	HeaterShadowing_Status = HEATER_OFF;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void HeaterOffDo(void)
{
	if (Heater_Shadowing_Data.Heater_Status == ON)
	{
		// Heater on
		EventHeaterOn();
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void UnknownPositionEntry(void)
{
	HeaterShadowing_Status = UNKNOWN_DIVERTER_POSITION;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void UnknownPositionDo(void)
{
	if (Heater_Shadowing_Data.Heater_Status == OFF)
	{
		// Heater off
		EventHeaterOff();
	}
	else if ((uint8)DiverterInLSAOrMSA() == (uint8)TRUE)
	{
		EventDiverterPositionChanged();
	}

}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void WaitingDelayEntry(void)
{
	HeaterShadowing_Status = HEATER_NOT_SHADOWED;
	(void)SystemTimers__NVSetTimerMS(&Heater_Shadowing_Data.NV_Timer_Heater_Shadowing, CHECK_TIMEOUT_MS, SYSTEMTIMERS_TYPE_UP_COUNTER);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void WaitingDelayDo(void)
{
	if (Heater_Shadowing_Data.Heater_Status == OFF)
	{
		// Heater off
		EventHeaterOff();
	}
	else if (DiverterInLSAOrMSA() == FALSE)
	{
		EventDiverterPositionUnknown();
	}
	else if (Heater_Shadowing_Data.Previous_Diverter_Status != Heater_Shadowing_Data.Diverter_Status)
	{
		EventDiverterPositionChanged();
	}
	else
	{
		if (SystemTimers__NVGetTimerMs(&Heater_Shadowing_Data.NV_Timer_Heater_Shadowing) >=	((Heater_Shadowing_Data.Diverter_Status == DIVERTER_STATUS_LSA) ? (LSA_DELAY_MS) : (MSA_DELAY_MS)))
		{
			if (Hbl__GetGISequenceIDByIndex(Hbl__GetGIIndexByGIID(HBL_GI_I_HEATER)) > (uint8)0) // To avoid saving an inconsistent value into Current_Baseline_Value
			{
				EventDelayExpired();
			}
		}
	}

}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void CheckDeltaEntry(void)
{
	(void)SystemTimers__NVSetTimerMS(&Heater_Shadowing_Data.NV_Timer_HS_Stability, STABILITY_TIME_MS, SYSTEMTIMERS_TYPE_DOWN_COUNTER);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void CheckDeltaDo(void)
{
	if (Heater_Shadowing_Data.Heater_Status == OFF)
	{
		// Heater off
		EventHeaterOff();
	}
	else if (DiverterInLSAOrMSA() == FALSE)
	{
		EventDiverterPositionUnknown();
	}
	else if (Heater_Shadowing_Data.Previous_Diverter_Status != Heater_Shadowing_Data.Diverter_Status)
	{
		EventDiverterPositionChanged();
	}
	else if (SystemTimers__NVCheckTimer(&Heater_Shadowing_Data.NV_Timer_Heater_Shadowing) == SYSTEMTIMERS_STATE_DONE)
	{
		EventTimeoutExpired();
	}
	else
	{
		if (Heater_Shadowing_Data.Diverter_Status == DIVERTER_STATUS_LSA)
		{
			if ((Heater_Shadowing_Data.Heater_Current - Heater_Shadowing_Data.Current_Baseline_Value) >= LSA_RISE_THRESHOLD)
			{
				if (SystemTimers__NVCheckTimer(&Heater_Shadowing_Data.NV_Timer_HS_Stability) == SYSTEMTIMERS_STATE_DONE)
				{
					HeaterShadowing_Status = HEATER_SHADOWED;
				}
				else
				{
					HeaterShadowing_Status = HEATER_SHADOWED_RISK;
				}
			}
			else
			{
				(void)SystemTimers__NVSetTimerMS(&Heater_Shadowing_Data.NV_Timer_HS_Stability, STABILITY_TIME_MS, SYSTEMTIMERS_TYPE_DOWN_COUNTER);
				HeaterShadowing_Status = HEATER_NOT_SHADOWED;
			}
		}
		else
		{	// HS_Diverter_Status == DIVERTER_STATUS_MSA

			if ((Heater_Shadowing_Data.Current_Baseline_Value - Heater_Shadowing_Data.Heater_Current) >= MSA_DROP_THRESHOLD)
			{
				if (SystemTimers__NVCheckTimer(&Heater_Shadowing_Data.NV_Timer_HS_Stability) == SYSTEMTIMERS_STATE_DONE)
				{
					HeaterShadowing_Status = HEATER_SHADOWED;
				}
				else
				{
					HeaterShadowing_Status = HEATER_SHADOWED_RISK;
				}
			}
			else
			{
				(void)SystemTimers__NVSetTimerMS(&Heater_Shadowing_Data.NV_Timer_HS_Stability, STABILITY_TIME_MS, SYSTEMTIMERS_TYPE_DOWN_COUNTER);
				HeaterShadowing_Status = HEATER_NOT_SHADOWED;
			}
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void TimeoutEntry(void)
{
	HeaterShadowing_Status = HEATER_NOT_SHADOWED;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void TimeoutDo(void)
{
	if (Heater_Shadowing_Data.Heater_Status == OFF)
	{
		// Heater off
		EventHeaterOff();
	}
	else if (DiverterInLSAOrMSA() == FALSE)
	{
		EventDiverterPositionUnknown();
	}
	else if (Heater_Shadowing_Data.Previous_Diverter_Status != Heater_Shadowing_Data.Diverter_Status)
	{
		EventDiverterPositionChanged();
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @return
 */
static BOOL_TYPE HeaterIsOn(void)
{
	return ((Heater_Shadowing_Data.Heater_Status == ON) ? (TRUE) : (FALSE));
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @return
 */
static BOOL_TYPE DiverterInLSAOrMSA(void)
{
	return (((Heater_Shadowing_Data.Diverter_Status == DIVERTER_STATUS_LSA) || (Heater_Shadowing_Data.Diverter_Status == DIVERTER_STATUS_MSA)) ? (TRUE):(FALSE));
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SaveCurrentBaseline(void)
{
	Heater_Shadowing_Data.Current_Baseline_Value = Heater_Shadowing_Data.Heater_Current;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetAlgorithmNotActive(void)
{
    HeaterShadowing_Status = ALGORITHM_NOT_ACTIVE;
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Heater Shadowing State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Cycle Resume Event for the Heater Shadowing state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventCycleResume(void)
{
    switch (Heater_Shadowing_State)
    {
        case STATE_HEATER_SHADOWING_HOLD:
        case STATE_HEATER_SHADOWING_HEATER_OFF:
        case STATE_HEATER_SHADOWING_UNKNOWN_POSITION:
        case STATE_HEATER_SHADOWING_WAITING_DELAY:
        case STATE_HEATER_SHADOWING_CHECK_DELTA:
        case STATE_HEATER_SHADOWING_TIMEOUT:
            Heater_Shadowing_State = STATE_HEATER_SHADOWING_HOLD;
            HoldEntry();                            // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Heater_Shadowing_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Delay Expired Event for the Heater Shadowing state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDelayExpired(void)
{
    switch (Heater_Shadowing_State)
    {
        case STATE_HEATER_SHADOWING_WAITING_DELAY:
            SaveCurrentBaseline();                  // Transition action function.
            Heater_Shadowing_State = STATE_HEATER_SHADOWING_CHECK_DELTA;
            CheckDeltaEntry();                      // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Heater_Shadowing_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Diverter Position Changed Event for the Heater Shadowing state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDiverterPositionChanged(void)
{
    switch (Heater_Shadowing_State)
    {
        case STATE_HEATER_SHADOWING_UNKNOWN_POSITION:
        case STATE_HEATER_SHADOWING_WAITING_DELAY:
        case STATE_HEATER_SHADOWING_CHECK_DELTA:
        case STATE_HEATER_SHADOWING_TIMEOUT:
            Heater_Shadowing_State = STATE_HEATER_SHADOWING_WAITING_DELAY;
            WaitingDelayEntry();                    // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Heater_Shadowing_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Diverter Position Unknown Event for the Heater Shadowing state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDiverterPositionUnknown(void)
{
    switch (Heater_Shadowing_State)
    {
        case STATE_HEATER_SHADOWING_WAITING_DELAY:
        case STATE_HEATER_SHADOWING_CHECK_DELTA:
        case STATE_HEATER_SHADOWING_TIMEOUT:
            Heater_Shadowing_State = STATE_HEATER_SHADOWING_UNKNOWN_POSITION;
            UnknownPositionEntry();                 // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Heater_Shadowing_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Heater Off Event for the Heater Shadowing state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventHeaterOff(void)
{
    switch (Heater_Shadowing_State)
    {
        case STATE_HEATER_SHADOWING_UNKNOWN_POSITION:
        case STATE_HEATER_SHADOWING_WAITING_DELAY:
        case STATE_HEATER_SHADOWING_CHECK_DELTA:
        case STATE_HEATER_SHADOWING_TIMEOUT:
            Heater_Shadowing_State = STATE_HEATER_SHADOWING_HEATER_OFF;
            HeaterOffEntry();                       // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Heater_Shadowing_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Heater On Event for the Heater Shadowing state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventHeaterOn(void)
{
    switch (Heater_Shadowing_State)
    {
        case STATE_HEATER_SHADOWING_HEATER_OFF:
            if (DiverterInLSAOrMSA() == FALSE)
            {
                Heater_Shadowing_State = STATE_HEATER_SHADOWING_UNKNOWN_POSITION;
                UnknownPositionEntry();             // Entry Function.
            }
            else if ((uint8)DiverterInLSAOrMSA() == (uint8)TRUE)
            {
                Heater_Shadowing_State = STATE_HEATER_SHADOWING_WAITING_DELAY;
                WaitingDelayEntry();                // Entry Function.
            }
            break;

        default:
            // Event is ignored for all other values of Heater_Shadowing_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Heater Shadowing Not Active Event for the Heater Shadowing state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventHeaterShadowingNotActive(void)
{
    switch (Heater_Shadowing_State)
    {
        case STATE_HEATER_SHADOWING_INITIALIZE:
            SetAlgorithmNotActive();                // Transition action function.
            Heater_Shadowing_State = STATE_HEATER_SHADOWING_NONE; // Terminate Heater Shadowing state machine execution.
            break;

        default:
            // Event is ignored for all other values of Heater_Shadowing_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Hold Time Expired Event for the Heater Shadowing state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventHoldTimeExpired(void)
{
    switch (Heater_Shadowing_State)
    {
        case STATE_HEATER_SHADOWING_HOLD:
            if (HeaterIsOn() == FALSE)
            {
                Heater_Shadowing_State = STATE_HEATER_SHADOWING_HEATER_OFF;
                HeaterOffEntry();                   // Entry Function.
            }
            else if (((uint8)HeaterIsOn() == (uint8)TRUE) &&
                     (DiverterInLSAOrMSA() == FALSE))
            {
                Heater_Shadowing_State = STATE_HEATER_SHADOWING_UNKNOWN_POSITION;
                UnknownPositionEntry();             // Entry Function.
            }
            else if (((uint8)HeaterIsOn() == (uint8)TRUE) &&
                     ((uint8)DiverterInLSAOrMSA() == (uint8)TRUE))
            {
                Heater_Shadowing_State = STATE_HEATER_SHADOWING_WAITING_DELAY;
                WaitingDelayEntry();                // Entry Function.
            }
            break;

        default:
            // Event is ignored for all other values of Heater_Shadowing_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Start Heater Shadowing Algorithm Event for the Heater Shadowing state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventStartHeaterShadowingAlgorithm(void)
{
    switch (Heater_Shadowing_State)
    {
        case STATE_HEATER_SHADOWING_INITIALIZE:
            Heater_Shadowing_State = STATE_HEATER_SHADOWING_HOLD;
            HoldEntry();                            // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Heater_Shadowing_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Timeout Expired Event for the Heater Shadowing state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventTimeoutExpired(void)
{
    switch (Heater_Shadowing_State)
    {
        case STATE_HEATER_SHADOWING_CHECK_DELTA:
            Heater_Shadowing_State = STATE_HEATER_SHADOWING_TIMEOUT;
            TimeoutEntry();                         // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Heater_Shadowing_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Heater Shadowing state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void HeaterShadowingExecute(void)
{
    switch(Heater_Shadowing_State)
    {
        case STATE_HEATER_SHADOWING_HOLD:
            HoldDo();                               // Do function.
            break;

        case STATE_HEATER_SHADOWING_HEATER_OFF:
            HeaterOffDo();                          // Do function.
            break;

        case STATE_HEATER_SHADOWING_UNKNOWN_POSITION:
            UnknownPositionDo();                    // Do function.
            break;

        case STATE_HEATER_SHADOWING_WAITING_DELAY:
            WaitingDelayDo();                       // Do function.
            break;

        case STATE_HEATER_SHADOWING_CHECK_DELTA:
            CheckDeltaDo();                         // Do function.
            break;

        case STATE_HEATER_SHADOWING_TIMEOUT:
            TimeoutDo();                            // Do function.
            break;

        default:
            // There is nothing to do for all other values of Heater_Shadowing_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Heater Shadowing state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void HeaterShadowingInitialize(void)
{
    Heater_Shadowing_State = STATE_HEATER_SHADOWING_INITIALIZE;
    InitializeEntry();                              // Entry Function.
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The termination function for the Heater Shadowing state machine.
 * This function disables the state machine. Calls to StateMachineExecute() will have no effect.
 * The StateMachineInitialize() function must be called again to restart the state machine.
 * If the current state has an exit function, it will be called.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void HeaterShadowingTerminate(void)
{
    // None of the states have an Exit function.
    Heater_Shadowing_State = STATE_HEATER_SHADOWING_NONE;
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

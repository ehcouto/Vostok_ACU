/**
 *  @file       
 *
 *  @brief      Alternate Control Process Manager
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
@startuml{Heat_sm.jpg}
        title Alternate Control State Machine

        [*] --> INITIALIZE

        INITIALIZE : entry / ReadProcessParameters()  \n CheckProcessCall()
        INITIALIZE --> DIRECT_CONTROL : directControl
        INITIALIZE --> WAIT_DIVERTER_TARGET : alternateFixed
        INITIALIZE --> READ_STATUS : alternateFixedExternalMotor
        INITIALIZE --> EXIT: ConditionsFalse

        READ_STATUS : entry / SendRequestStatus() / StartTimerWaitStatus()
        READ_STATUS : do / CheckPulseDuration() \n  / CheckStatus()
        READ_STATUS --> WAIT_MOTOR_TARGET : Run
        READ_STATUS --> WAIT_STOPPED : WaitStopped
        READ_STATUS --> EXIT : Error
        READ_STATUS --> READ_STATUS : ErrorWaitStatus

		WAIT_STOPPED : entry / StartTimerWaitStopped()
        WAIT_STOPPED : do /  WaitStopped()
        WAIT_STOPPED --> WAIT_MOTOR_TARGET : Run
        WAIT_STOPPED --> READ_STATUS : ErrorWaitStopped
        WAIT_STOPPED --> EXIT : Error

		DIRECT_CONTROL : entry / CheckTargetPosition() \n SetHomePosition()
        DIRECT_CONTROL : do / CheckTargetPosition()  \n  / ActivateDiverter
        DIRECT_CONTROL --> EXIT : CheckTargConditionTrue

		WAIT_MOTOR_TARGET : entry / StartWaitTargetTimer()
		WAIT_MOTOR_TARGET: do /  PilotPump() \n CheckPulseDuration() \n CheckTargetSpeed() \n CheckWaitTargetTimer()
		WAIT_MOTOR_TARGET --> WAIT_DIVERTER_TARGET : CheckTargetSpeedTrue

		WAIT_DIVERTER_TARGET: entry / PilotPump() \n CheckTargetPosition() \n SetHomePosition()
		WAIT_DIVERTER_TARGET: do / PilotPump() \n CheckTargetPosition() \n ActivateDiverter \n CheckPulseDuration()
		WAIT_DIVERTER_TARGET --> MNT_EXE : CheckTargConditionTrue
		WAIT_DIVERTER_TARGET --> INITIALIZE: CheckNewProfileFalse
		WAIT_DIVERTER_TARGET --> READ_STATUS : CheckNewProfileTrueExternalMotor

		MNT_EXE: do / PilotPump() \n CheckPulseDuration()
		MNT_EXE --> WAIT_DIVERTER_TARGET : CheckNewProfileTrue
		MNT_EXE --> READ_STATUS : CheckNewProfileTrueExternalMotor
		MNT_EXE --> INITIALIZE: CheckNewProfileFalse

		EXIT: entry / returnTRUE()
        EXIT: do / returnTRUE()
@enduml
*/

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "AlternateCtrl.h"

#include "AlternateLoad.h"
#if (API221_CLIENT_FEATURE == ENABLED)
#include "API221MotionCtrl.h"
#endif
#include "C_Extensions.h"
#include "Condition.h"
#include "DishAuxVar.h"
#include "Fault.h"
#include "Hbl.h"
#include "Interpreter.h"
#include "Micro.h"
#include "NVData.h"
#include "PumpCommand.h"
#include "PumpControl.h"
#include "PumpFlushRequestMonitor.h"
#include "Selector.h"
#include "StabilityControl.h"
#include <string.h>
#include "SystemTimers.h"
#include "Variable.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
//! The name of this module for use by the Log module.
#define MODULE_NAME MODULE_ALTERNATE

//! Displacement value that indicates that the feature is not used.
#define UNUSED_FEATURE_BYTE 0xFF
#define UNUSED_FEATURE_WORD 0xFFFF

#define POSITION_HOME_REQUEST 0x10U

typedef enum PUMP_STATE_ENUM
{
    STATE_OFF = 0,
    STATE_ON
}PUMP_STATE_TYPE;

//! The process control structure used by all the state machine functions.
static PROCESS_CONTROL_TYPE* Control;
static BOOL_TYPE First_Request;
static HBL_PILOT_ENUM_TYPE Pump_Pilot_Type;
static HBL_PILOT_ENUM_TYPE Drain_Pump_Pilot_Type;

#if (API221_CLIENT_FEATURE == ENABLED)
static uint32 AC_Status[MOTOR_MAX];
static BOOL_TYPE AC_PendingStatus[MOTOR_MAX];
static BOOL_TYPE Ask_Drain_Pump_Status;
#endif

static SYSTEMTIMER_NV_TIMER_TYPE AC_Wait_Stopped_Timer_Handle;
static SYSTEMTIMER_NV_TIMER_TYPE AC_Wait_Target_Timer_Handle;
static SYSTEMTIMER_NV_TIMER_TYPE AC_Wait_Status_Timer_Handle;
static SYSTEMTIMER_NV_TIMER_TYPE AC_Wait_MCU_Reset_Timer_Handle;

static uint8 AC_Wait_Stopped_Attempts;


#if (API221_CLIENT_FEATURE == ENABLED)
static void API221ApplReceivedStatus (API221_CLIENT_MOTOR_INDEX_TYPE motor, uint32 status);
static void API221HandleMotorEventAC(API221_EVENT_TYPE event);
static uint8 AC_Wait_Status_Attempts;
#endif

#pragma location = "_appliance_segment"
static BOOL_TYPE AlternateCtrl_Ready_To_Exit;
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

BOOL_TYPE CheckNewProfile(void);

//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Alternate Control State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Alternate Control state machine.
typedef enum ALTERNATE_CONTROL_STATE_ENUM
{
    STATE_ALTERNATE_CONTROL_NONE                 = 0,

    STATE_ALTERNATE_CONTROL_INITIALIZE           = 1, //!< (initial_state)
    STATE_ALTERNATE_CONTROL_DIRECT_CONTROL       = 2,
    STATE_ALTERNATE_CONTROL_WAIT_DIVERTER_TARGET = 3,
    STATE_ALTERNATE_CONTROL_READ_STATUS          = 4,
    STATE_ALTERNATE_CONTROL_EXIT                 = 5,
    STATE_ALTERNATE_CONTROL_WAIT_MOTOR_TARGET    = 6,
    STATE_ALTERNATE_CONTROL_WAIT_STOPPED         = 7,
    STATE_ALTERNATE_CONTROL_MNT_EXE              = 8,

    STATE_ALTERNATE_CONTROL_END                  = 9
} ALTERNATE_CONTROL_STATE_TYPE;

//! The state variable for the Alternate Control state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static ALTERNATE_CONTROL_STATE_TYPE Alternate_Control_State;

//! The transition variable for the Alternate Control state machine.
//! Flag used by the state machine to track when a transition occurs in the middle of a state's
//! Entry() or Do() action that contains more than one function.
static BOOL_TYPE Alternate_Control_Transitioned;

// -- Private Function Prototypes used by the state machine for customization functions --
static void ActivateDiverter(void);
static void CheckProcessCall(void);
static void CheckPulseDuration(void);
static void CheckStatus(void);
static void CheckTargetPosition(void);
static void CheckTargetSpeed(void);
static void CheckWaitTargetTimer(void);
static void PilotPump(void);
static void ReadProcessParameters(void);
static void ReturnTRUE(void);
static void SendRequestStatus(void);
static void SetHomePosition(void);
static void StartTimerWaitStatus(void);
static void StartTimerWaitStopped(void);
static void StartWaitTargetTimer(void);
static void WaitStopped(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void AlternateControlExecute(void);
static void AlternateControlInitialize(void);
static void AlternateControlTerminate(void);
static void EventAlternateFixed(void);
static void EventAlternateFixedExternalMotor(void);
static void EventCheckNewProfileFalse(void);
static void EventCheckNewProfileTrue(void);
static void EventCheckNewProfileTrueExternalMotor(void);
static void EventCheckTargConditionTrue(void);
static void EventCheckTargetSpeedTrue(void);
static void EventConditionsFalse(void);
static void EventDirectControl(void);
static void EventError(void);
static void EventErrorWaitStatus(void);
static void EventErrorWaitStopped(void);
static void EventRun(void);
static void EventWaitStopped(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================
//! The types of Heat variations that are available.
typedef enum ALTERNATE_OPTION_ENUM
{
	ALTERNATE_OPTION_DIRECT_CTRL,                        //!< F1=0, F0=0
	ALTERNATE_OPTION_ALTERNATE_FIXED,                    //!< F1=0, F0=1
	ALTERNATE_OPTION_ALTERNATE_WITH_OPTIONS,             //!< F1=1, F0=0
	ALTERNATE_OPTION_COUNT
} ALTERNATE_OPTION_TYPE;

//! A list of log message IDs that are generated by this module.
typedef enum MODULE_ALTERNATE_LOG_MESSAGE_ID_ENUM
{
    ERROR_PROCESS_CONTROL       = 1,
    ERROR_LOADING_PARAMETERS,
	ERROR_WAIT_STOPPED,
	ERROR_WAIT_STOPPED_MAX_ATTEMPS,
	ERROR_WAIT_STATUS,
	ERROR_WAIT_STATUS_MAX_ATTEMPS,
	ERROR_MCU_RESET
} MODULE_ALTERNATE_LOG_MESSAGE_ID_TYPE;

//! Index used to access the Alternate[] arrays.
typedef enum ALTERNATE_CONTROL_CYCLE_ENUM
{
	ALTERNATE_CONTROL_MAIN_CYCLE = 0,
	ALTERNATE_CONTROL_SUB_CYCLE  = 1,
	ALTERNATE_CONTROL_PAUSE_CYCLE= 2,
	ALTERNATE_CONTROL_MAX_COUNTER_CYCLE
} ALTERNATE_CONTROL_CYCLE_TYPE;


//! The data structure of the Heat process parameters as they are found in the setting file.
typedef PACKED struct DIRECT_CTRL_PARAMETER_STRUCT
{

    uint8 Position		:4;
    uint8 				:4;

    uint8 Unused;
} DIRECT_CTRL_PARAMETER_TYPE;

typedef PACKED struct ALTERNATE_CTRL_HEAD_STRUCT
{
    uint8 Conditions_Index;
    uint8 Profile_Index;
} ALTERNATE_CTRL_HEAD_TYPE;

typedef PACKED struct ALTERNATE_PARAMETER_STRUCT
{
    uint16 Motor_Acceleration;
    uint16 Position				:4;
    uint16 Pulse_Duration       :12;
    uint8 Wash_Motor_Speed;
} ALTERNATE_PARAMETER_TYPE;


// -- Private Variable Definitions --

typedef struct ALTERNATE_INSTANCE_STRUCT
{
	uint16 Pulse_Duration;
    uint16 Wash_Motor_Speed;
    uint16 Motor_Acceleration;
	uint16 Wash_Motor_Speed_Before_Reduction;
	uint8 Position;
	uint8 Position_Old;
	uint8 Num_Items;
	ALTERNATE_CONTROL_STATE_TYPE Alternate_State;
} ALTERNATE_INSTANCE_TYPE;

static void ReadAlternateParameters(SETTINGFILE_LOADER_TYPE* loader);

//! ALTERNATE_INSTANCE_TYPE for each of the possible cycles (Main, Sub-Cycle, Pause).
#pragma location = "_appliance_segment"
static ALTERNATE_INSTANCE_TYPE Alternate[ALTERNATE_CONTROL_MAX_COUNTER_CYCLE];

static ALTERNATE_CONTROL_CYCLE_TYPE Alternate_Control_Cycle_Type;

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
void AlternateCtrl__Initialize(void)
{
	if ((Micro__GetResetMode() == MICRO_RESET_MODE_COLD) &&
	    (NVData__GetLastRestoreStatus()->Bit.Appliance_Data_Recovered != (NVDATA_RESTORE_TYPE)NVDATA_RESTORE_OKAY))
	{
		memset(&Alternate[0], 0, sizeof(Alternate));
		AlternateCtrl_Ready_To_Exit = FALSE;
	}
}


PROCESS_RESULT_TYPE AlternateCtrl__Process(PROCESS_CONTROL_TYPE* control)
{
	uint8 *pos_res;
	HBL_LOAD_STATUS_TYPE pos_val;
	uint8 pos_load;
	MASTER_LOAD_TYPE value_state;
	PROCESS_RESULT_TYPE result;
    ALTERNATE_OPTION_TYPE option = (ALTERNATE_OPTION_TYPE)control->F1_F0_Flags;
    if ((control == NULL ) ||
        (control->Compartment >= APPLIANCE_MAX_COMPARTMENTS) ||
        (option >= ALTERNATE_OPTION_COUNT) ||
		(((uint8)PumpControl__GetVSMDrainIsRunning() == (uint8)TRUE) && (option != ALTERNATE_OPTION_DIRECT_CTRL)))
    {
    	AlternateCtrl_Ready_To_Exit = TRUE;
        LOG_ADD_EXCEPTION(ERROR_PROCESS_CONTROL, (control != NULL));
    }
    else
    {
        sint32 cycle_id;
		CYCLE_STATE_TYPE cycle_state;

		if ((uint8)Variable__GetValue(VARIABLE_CYCLE, control->Compartment, &cycle_id) == (uint8)TRUE)
		{
			cycle_state = Selector__GetCycleState(0, control->Compartment, (uint8)cycle_id);
		}
		else
		{
			cycle_state = CYCLE_STATE_RUN;
		}
		if (cycle_state == CYCLE_STATE_RUN)
		{
			if ((uint8)control->Sub_Cycle_Running == 1U)
			{
				Alternate_Control_Cycle_Type = ALTERNATE_CONTROL_SUB_CYCLE;
			}
			else
			{
				Alternate_Control_Cycle_Type = ALTERNATE_CONTROL_MAIN_CYCLE;
			}
		}
		else
		{
			Alternate_Control_Cycle_Type = ALTERNATE_CONTROL_PAUSE_CYCLE;
		}

		Control = control;

        if (((uint8)control->Step_Initializing == 1U) || ((uint8)control->Resume_From_Reset == 1U) || ((uint8)control->Resume_From_Pause == 1U))
        {
        	First_Request = TRUE;
			#if (API221_CLIENT_FEATURE == ENABLED)
        	AC_PendingStatus[MOTOR_0] = FALSE;
        	AC_PendingStatus[MOTOR_1] = FALSE;
        	Ask_Drain_Pump_Status = FALSE;
			#endif
        	Pump_Pilot_Type = Hbl__GetPilotIDByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_WASH_PUMP));
        	Drain_Pump_Pilot_Type = Hbl__GetPilotIDByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DRAIN_PUMP));

        	if ((uint8)control->Step_Initializing == 1U)
        	{
        		// Initialize this instance of the Alternate process.
        		AlternateCtrl_Ready_To_Exit = FALSE;
				#if (API221_CLIENT_FEATURE == ENABLED)
        		Ask_Drain_Pump_Status = TRUE;
				#endif
        		Fault__ConditionRemoved(FAULT_ID_DIVERTER_CANNOT_FIND_POSITIONS);
            	AlternateControlInitialize();
        	}
        	else
        	{
        		Alternate_Control_State = Alternate[Alternate_Control_Cycle_Type].Alternate_State;
				if ((Alternate_Control_State == STATE_ALTERNATE_CONTROL_WAIT_DIVERTER_TARGET)||
					(Alternate_Control_State == STATE_ALTERNATE_CONTROL_DIRECT_CONTROL))
				{
					SetHomePosition(); // Search home position if needed
				}

				if ((uint8)control->Resume_From_Pause == 1U)
				{
					if (Alternate_Control_State != STATE_ALTERNATE_CONTROL_DIRECT_CONTROL)
					{
						Diverter_Position_Type diverter_position = DishAuxVar__GetDiverterPosition();
						if (Pump_Pilot_Type == HBL_PILOT_EXTERNAL_MOTOR)
						{
							if (Alternate_Control_State != STATE_ALTERNATE_CONTROL_NONE)
							{
								#if (API221_CLIENT_FEATURE == ENABLED)
								Ask_Drain_Pump_Status = TRUE;
								#endif
								Alternate_Control_State = STATE_ALTERNATE_CONTROL_READ_STATUS;
								SendRequestStatus();
								StartTimerWaitStatus();
							}
							else
							{
								if ((diverter_position.Position != Alternate[Alternate_Control_Cycle_Type].Position) &&
									(AlternateLoad__GetAlternateLoadSearchHome() == FALSE))
								{
									Alternate_Control_State = STATE_ALTERNATE_CONTROL_DIRECT_CONTROL;
								}
							}
						}
						else
						{
							if (Alternate_Control_State != STATE_ALTERNATE_CONTROL_NONE)
							{
								if (diverter_position.Position != Alternate[Alternate_Control_Cycle_Type].Position)
								{
									Alternate_Control_State = STATE_ALTERNATE_CONTROL_WAIT_DIVERTER_TARGET;
								}
							}
							else
							{
								if ((diverter_position.Position != Alternate[Alternate_Control_Cycle_Type].Position)  &&
								    (AlternateLoad__GetAlternateLoadSearchHome() == FALSE))
								{
									Alternate_Control_State = STATE_ALTERNATE_CONTROL_DIRECT_CONTROL;
								}
							}
						}
					}
				}

				#if (API221_CLIENT_FEATURE == ENABLED)
				PumpFlushRequestMonitor__ResetWashStopSentFlag();
				#endif
        	}

			#if (API221_CLIENT_FEATURE == ENABLED)
        	if (option != ALTERNATE_OPTION_DIRECT_CTRL)
        	{
        		API221MotionCtrl__RegisterForEvents(API221HandleMotorEventAC);
        		PumpControl__UnregisterApi221Events();
        	}

        	AC_Wait_Stopped_Attempts = 0;
        	AC_Wait_Status_Attempts = 0;
			#endif

            Fault__ConditionRemoved(FAULT_ID_DIVERTER_CANNOT_FIND_POSITIONS);
        }
        else
        {

        	if (SystemTimers__NVCheckTimer(&AC_Wait_MCU_Reset_Timer_Handle) == SYSTEMTIMERS_STATE_RUNNING)
			{
				// Send Stop
				PumpCommand__StopWithDefDecel(HBL_PRIORITY_APPLICATION_HI, HBL_LOAD_WASH_PUMP);

				if (Drain_Pump_Pilot_Type == HBL_PILOT_EXTERNAL_MOTOR)
				{
					PumpCommand__StopWithDefDecel(HBL_PRIORITY_APPLICATION_HI, HBL_LOAD_DRAIN_PUMP);
				}
			}
        	else
        	{
        		AlternateControlExecute();
        	}
        }

        if (Pump_Pilot_Type == HBL_PILOT_EXTERNAL_MOTOR)
        {
        	if ((ALTERNATE_OPTION_TYPE)Control->F1_F0_Flags != (ALTERNATE_OPTION_TYPE)ALTERNATE_OPTION_DIRECT_CTRL)
        	{
        		PumpFlushRequestMonitor__SetWashPumpCtrlExecutedFlag();
        		PumpCommand__MaintainPreviousCommand(HBL_PRIORITY_APPLICATION_MID, HBL_LOAD_WASH_PUMP);
        	}
        }

    }//*/


    if ((uint8)Fault__IsInFaultState(FAULT_ID_DIVERTER_CANNOT_FIND_POSITIONS) == (uint8)TRUE)
    {
    	pos_load = Hbl__GetLoadIndexByLoadID(HBL_LOAD_DIVERTER);
    	pos_res = (uint8 *) Hbl__GetLoadStatusByIndex(pos_load);
		pos_val.Byte = *pos_res;

		if (pos_val.Load.Status == (HBL_LOAD_STATUS_ENUM_TYPE)HBL_LOAD_STATUS_TARGET_COMPLETED)
		{
	    	if ((uint8)AlternateLoad__GetRunAfterFaultFlag() == (uint8)TRUE)
			{
				uint8 rotate = 0x20;
				(void)Hbl__SetLoadRequestByLoadID(HBL_LOAD_DIVERTER, HBL_PRIORITY_DIVERTER, (uint8 *) &rotate);

				CYCLE_STATE_TYPE label = Selector__GetCycleState(0, 0, Interpreter__GetCurrentStatement(0).Cycle);

				if (label == CYCLE_STATE_RUN)
				{
					value_state = LOAD_STATE_ON;
					(void)Hbl__SetLoadRequestByLoadID(HBL_LOAD_PILOT_RELAY, HBL_PRIORITY_DIVERTER, (uint8 *) &value_state);
					(void)Hbl__SetLoadRequestByLoadID(HBL_LOAD_RELAY_ENABLE, HBL_PRIORITY_DIVERTER, (uint8 *) &value_state);
				}
			}
		}
    }

    Alternate[Alternate_Control_Cycle_Type].Alternate_State = Alternate_Control_State;

    if(Alternate_Control_State == STATE_ALTERNATE_CONTROL_NONE)
    {
    	AlternateCtrl_Ready_To_Exit = TRUE;
    }

 //result = ((AlternateCtrl_Ready_To_Exit == TRUE) && (Alternate_Control_State != STATE_ALTERNATE_CONTROL_WAIT_DIVERTER_TARGET))? PROCESS_COMPLETE : PROCESS_BUSY;
    result = (((uint8)AlternateCtrl_Ready_To_Exit == (uint8)TRUE) && ((uint8)DishAuxVar__GetDiverterPosition().Is_Stable == (uint8)TRUE))? PROCESS_COMPLETE : PROCESS_BUSY;
    return result;
}


uint8 AlternateCtrl__GetPosition(void)
{
    return Alternate[Alternate_Control_Cycle_Type].Position;
}

#if (API221_CLIENT_FEATURE == ENABLED)
void AlternateCtrl__UnregisterApi221Events(void)
{
	API221MotionCtrl__UnregisterForEvents(API221HandleMotorEventAC);
}
#endif

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
void AlternateCtrl__McuReset(uint8 node, uint8 reason, uint8 reset_mode)
{
    if (node == (uint8)0)
    {
    	LOG_ADD_EXCEPTION(ERROR_MCU_RESET, Alternate_Control_State);
    	(void)SystemTimers__NVSetTimerMS(&AC_Wait_MCU_Reset_Timer_Handle, 10000,SYSTEMTIMERS_TYPE_DOWN_COUNTER);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void ActivateDiverter(void)
{
	uint8 *pos_res;
	HBL_LOAD_STATUS_TYPE pos_val;
	uint8 pos_load;
	MASTER_LOAD_TYPE value_state;

	pos_load = Hbl__GetLoadIndexByLoadID(HBL_LOAD_DIVERTER);
	if ((uint8)HBL_INDEX_INVALID != pos_load)
	{
		pos_res = (uint8 *) Hbl__GetLoadStatusByIndex(pos_load);

		pos_val.Byte = *pos_res;

		if ((Alternate[Alternate_Control_Cycle_Type].Position & POSITION_HOME_REQUEST) != (uint8)0)
		{
			if (pos_val.Load.Status == (HBL_LOAD_STATUS_ENUM_TYPE)HBL_LOAD_STATUS_WAITING_TARGET)
			{
				Alternate[Alternate_Control_Cycle_Type].Position &= 0xEFU;
			}
		}

		//if ((pos_val.Load.Cur_Reg != Alternate.Position) && Alternate.Position != 0)
		if ((pos_val.Load.Status != (HBL_LOAD_STATUS_ENUM_TYPE)HBL_LOAD_STATUS_TARGET_COMPLETED)||((uint8)First_Request == (uint8)TRUE))
		{
			First_Request = FALSE;
			(void)Hbl__SetLoadRequestByLoadID(HBL_LOAD_DIVERTER, HBL_PRIORITY_DIVERTER, (uint8 *) &Alternate[Alternate_Control_Cycle_Type].Position);

			CYCLE_STATE_TYPE label = Selector__GetCycleState(0, 0, Interpreter__GetCurrentStatement(0).Cycle);

			if (label == CYCLE_STATE_RUN)
			{
				value_state = LOAD_STATE_ON;
				(void)Hbl__SetLoadRequestByLoadID(HBL_LOAD_PILOT_RELAY, HBL_PRIORITY_DIVERTER, (uint8 *) &value_state);
				(void)Hbl__SetLoadRequestByLoadID(HBL_LOAD_RELAY_ENABLE, HBL_PRIORITY_DIVERTER, (uint8 *) &value_state);
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
	if ((ALTERNATE_OPTION_TYPE)Control->F1_F0_Flags == (ALTERNATE_OPTION_TYPE)ALTERNATE_OPTION_DIRECT_CTRL)
	{
		EventDirectControl();
	}
	else if (((ALTERNATE_OPTION_TYPE)Control->F1_F0_Flags == (ALTERNATE_OPTION_TYPE)ALTERNATE_OPTION_ALTERNATE_FIXED) ||
			((ALTERNATE_OPTION_TYPE)Control->F1_F0_Flags == (ALTERNATE_OPTION_TYPE)ALTERNATE_OPTION_ALTERNATE_WITH_OPTIONS))
	{
		if (Pump_Pilot_Type == HBL_PILOT_EXTERNAL_MOTOR)
		{
			if (Alternate[Alternate_Control_Cycle_Type].Wash_Motor_Speed == 0U)
			{
				EventAlternateFixed(); // if speed 0 go directly to wait diverter
			}
			else
			{
				EventAlternateFixedExternalMotor();
			}
		}
		else
		{
			EventAlternateFixed();
		}
	}
	else
	{
		// nothing to do
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void CheckTargetPosition(void)
{
	uint8 *pos_res;
	HBL_LOAD_STATUS_TYPE pos_val;

	if ((uint8)HBL_INDEX_INVALID == Hbl__GetLoadIndexByLoadID(HBL_LOAD_DIVERTER))
	{
		EventCheckTargConditionTrue();
	}
	else
	{
		pos_res = (uint8 *) Hbl__GetLoadStatusByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DIVERTER));

		pos_val.Byte = *pos_res;

		if ((pos_val.Load.Cur_Reg == Alternate[Alternate_Control_Cycle_Type].Position) && 
		    (pos_val.Load.Status == (HBL_LOAD_STATUS_ENUM_TYPE)HBL_LOAD_STATUS_TARGET_COMPLETED) &&
			((uint8)Control->Resume_From_Pause == 0U) && 
			((uint8)Control->Resume_From_Reset == 0U) &&
			(AlternateLoad__GetAlternateLoadSearchHome() == FALSE))
		{
			if ((uint8)Control->Step_Initializing == 1U)
			{
				if (Alternate[Alternate_Control_Cycle_Type].Position_Old == Alternate[Alternate_Control_Cycle_Type].Position)
				{
					EventCheckTargConditionTrue();
				}
			}
			else
			{
				EventCheckTargConditionTrue();
			}

		}
		else
		{
		// nothing to do
		}

		if (Fault__IsInFaultState(FAULT_ID_DIVERTER_CANNOT_FIND_POSITIONS) ||
			Fault__IsInFaultState(FAULT_ID_DIVERTER_LEAK) ||
			Fault__IsInFaultState(FAULT_ID_DIVERTER_TRIAC_SHORTED))
		{
			EventCheckTargConditionTrue();
		}
	}

}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void ReadProcessParameters(void)
{
	SETTINGFILE_LOADER_TYPE loader;

	// Get the Maintain process parameters from the setting file.
	if (SettingFile__MSettingBasicLoader(Control->Setting_File_Index,
										SF_PTR_ACU_PROCESS_ALTERNATE_CTRL,
										 Control->Data_Field_Index,
										 &loader) == FAIL)
	{
		LOG_ADD_EXCEPTION(ERROR_LOADING_PARAMETERS, Control->Data_Field_Index);
	}
	else
	{
		// Read the setting file data into local variables for convenience.
		ReadAlternateParameters(&loader);
	}
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Read the setting file parameters for this instance of the Heat process.
 * @param control = The process control parameters from the Interpreter.
 */

static void ReadAlternateParameters(SETTINGFILE_LOADER_TYPE* loader)
{
	ALTERNATE_CTRL_HEAD_TYPE* alternate_head_parameter = NULL;
    ALTERNATE_PARAMETER_TYPE* alternate_parameter;
    DIRECT_CTRL_PARAMETER_TYPE* direct_ctrl_parameter;
    SETTINGFILE_LOADER_TYPE loader_profile;

    Alternate[Alternate_Control_Cycle_Type].Position_Old = Alternate[Alternate_Control_Cycle_Type].Position;

    if ((ALTERNATE_OPTION_TYPE)Control->F1_F0_Flags == (ALTERNATE_OPTION_TYPE)ALTERNATE_OPTION_DIRECT_CTRL)
    {
    	direct_ctrl_parameter = (DIRECT_CTRL_PARAMETER_TYPE*)(void*)loader->Data;

    	Alternate[Alternate_Control_Cycle_Type].Pulse_Duration = UNUSED_FEATURE_WORD;
    	Alternate[Alternate_Control_Cycle_Type].Motor_Acceleration = UNUSED_FEATURE_WORD;
    	Alternate[Alternate_Control_Cycle_Type].Wash_Motor_Speed = UNUSED_FEATURE_WORD;
    	Alternate[Alternate_Control_Cycle_Type].Position = direct_ctrl_parameter->Position;
    }
    else if ((ALTERNATE_OPTION_TYPE)Control->F1_F0_Flags == (ALTERNATE_OPTION_TYPE)ALTERNATE_OPTION_ALTERNATE_FIXED)
    {
    	alternate_head_parameter = (ALTERNATE_CTRL_HEAD_TYPE*)(void*)loader->Data;

		// Get the Maintain process parameters from the setting file.
		if (SettingFile__MSettingBasicLoader(Control->Setting_File_Index,
											SF_PTR_ACU_PROCESS_ALTERNATE_CTRL,
											 alternate_head_parameter->Profile_Index,
											 &loader_profile) == FAIL)
		{
			LOG_ADD_EXCEPTION(ERROR_LOADING_PARAMETERS, alternate_head_parameter->Profile_Index);
		}
		else
		{
			uint16 temp_motor_speed;

			alternate_parameter = (ALTERNATE_PARAMETER_TYPE*)(void*)loader_profile.Data;
			Alternate[Alternate_Control_Cycle_Type].Num_Items = (uint8)(loader_profile.Length / (uint8)6);
			Alternate[Alternate_Control_Cycle_Type].Pulse_Duration = alternate_parameter->Pulse_Duration;
			Alternate[Alternate_Control_Cycle_Type].Motor_Acceleration = alternate_parameter->Motor_Acceleration;

			temp_motor_speed = (uint16)alternate_parameter->Wash_Motor_Speed * 25U;
			Alternate[Alternate_Control_Cycle_Type].Wash_Motor_Speed_Before_Reduction = temp_motor_speed;
			if (StabilityControl__GetSpeedReduction() != SC_NO_SPEED_REDUCTION)
			{
				StabilityControl__ReduceSpeed(&temp_motor_speed, alternate_parameter->Position);
			}
			Alternate[Alternate_Control_Cycle_Type].Wash_Motor_Speed = temp_motor_speed;

			Alternate[Alternate_Control_Cycle_Type].Position = alternate_parameter->Position;
			Alternate[Alternate_Control_Cycle_Type].Num_Items --;
		}
    }
    else if ((ALTERNATE_OPTION_TYPE)Control->F1_F0_Flags == (ALTERNATE_OPTION_TYPE)ALTERNATE_OPTION_ALTERNATE_WITH_OPTIONS)
	{
    	for (uint8 index_conditions = 0; index_conditions < (loader->Length / (uint8)2); index_conditions++)
    	{
    		alternate_head_parameter = (ALTERNATE_CTRL_HEAD_TYPE*)(void*)&loader->Data[index_conditions*2U];

    		if (((uint8)UNUSED_FEATURE_BYTE != alternate_head_parameter->Conditions_Index) &&
			(CONDITION_LEGACY_EVAL(alternate_head_parameter->Conditions_Index) == CONDITION_RESULT_TRUE))
			{
    			break;
			}

    		if (index_conditions == ((loader->Length / 2U)-1U ))
    		{
    			EventConditionsFalse();
    		}
    	}

    	if (alternate_head_parameter != NULL)
    	{
			// Get the Maintain process parameters from the setting file.
			if (SettingFile__MSettingBasicLoader(Control->Setting_File_Index,
												SF_PTR_ACU_PROCESS_ALTERNATE_CTRL,
												 alternate_head_parameter->Profile_Index,
												 &loader_profile) == FAIL)
			{
				LOG_ADD_EXCEPTION(ERROR_LOADING_PARAMETERS, alternate_head_parameter->Profile_Index);
			}
			else
			{
				uint16 temp_motor_speed;

				alternate_parameter = (ALTERNATE_PARAMETER_TYPE*)(void*)loader_profile.Data;
				Alternate[Alternate_Control_Cycle_Type].Num_Items = (uint8)(loader_profile.Length / (uint8)6);
				Alternate[Alternate_Control_Cycle_Type].Pulse_Duration = alternate_parameter->Pulse_Duration;
				Alternate[Alternate_Control_Cycle_Type].Motor_Acceleration = alternate_parameter->Motor_Acceleration;

				temp_motor_speed = (uint16)alternate_parameter->Wash_Motor_Speed * 25U;
				Alternate[Alternate_Control_Cycle_Type].Wash_Motor_Speed_Before_Reduction = temp_motor_speed;
				if (StabilityControl__GetSpeedReduction() != SC_NO_SPEED_REDUCTION)
				{
					StabilityControl__ReduceSpeed(&temp_motor_speed, alternate_parameter->Position);
				}
				Alternate[Alternate_Control_Cycle_Type].Wash_Motor_Speed = temp_motor_speed;

				Alternate[Alternate_Control_Cycle_Type].Position = alternate_parameter->Position;
				Alternate[Alternate_Control_Cycle_Type].Num_Items --;
			}
    	}
	}
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void ReturnTRUE(void)
{
	AlternateCtrl_Ready_To_Exit = TRUE;
	AlternateControlTerminate();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void PilotPump(void)
{
	if (Alternate[Alternate_Control_Cycle_Type].Wash_Motor_Speed > (uint8)STATE_OFF)
	{
		if (Pump_Pilot_Type == HBL_PILOT_EXTERNAL_MOTOR)
		{
			uint16 temp_motor_speed;

			temp_motor_speed = Alternate[Alternate_Control_Cycle_Type].Wash_Motor_Speed_Before_Reduction;

			if (StabilityControl__GetSpeedReduction() != SC_NO_SPEED_REDUCTION)
			{
				StabilityControl__ReduceSpeed(&temp_motor_speed, Alternate[Alternate_Control_Cycle_Type].Position & 0x0FU);
			}

			if (temp_motor_speed < (uint16)MIN_SPEED)
			{
				temp_motor_speed = MIN_SPEED;
			}
			else if (temp_motor_speed > (uint16)MAX_SPEED)
			{
				temp_motor_speed = MAX_SPEED;
			}

			if (temp_motor_speed != Alternate[Alternate_Control_Cycle_Type].Wash_Motor_Speed)
			{
				// Target speed changed, reset Previous_Wash_Motor_Speed (needed if the target is not reached by the end of the pulse)
				PumpControl__SetPreviousWashMotorSpeed(0);

				Alternate[Alternate_Control_Cycle_Type].Wash_Motor_Speed = temp_motor_speed;
			}

			PumpCommand__Run((sint16)Alternate[Alternate_Control_Cycle_Type].Wash_Motor_Speed,Alternate[Alternate_Control_Cycle_Type].Motor_Acceleration, HBL_PRIORITY_APPLICATION_HI, HBL_LOAD_WASH_PUMP);
		}
		else
		{
			PUMP_STATE_TYPE state;

			state = STATE_ON;

			(void)(Hbl__SetLoadRequestByLoadID(HBL_LOAD_WASH_PUMP, HBL_PRIORITY_APPLICATION_MID, (uint8 *) &state));
		}
	}
	else
	{
		if (Pump_Pilot_Type == HBL_PILOT_EXTERNAL_MOTOR)
		{
			PumpCommand__Stop(DEFAULT_DECELERATION,HBL_PRIORITY_APPLICATION_HI, HBL_LOAD_WASH_PUMP);
			PumpFlushRequestMonitor__SetWashStopSentFlag();
		}
	}

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
BOOL_TYPE CheckNewProfile(void)
{
	SETTINGFILE_LOADER_TYPE loader;
	ALTERNATE_CTRL_HEAD_TYPE* alternate_head_parameter = NULL;
	ALTERNATE_PARAMETER_TYPE* alternate_parameter;

	BOOL_TYPE return_value = FALSE;

	// Get the Maintain process parameters from the setting file.
	if (SettingFile__MSettingBasicLoader(Control->Setting_File_Index,
										SF_PTR_ACU_PROCESS_ALTERNATE_CTRL,
										 Control->Data_Field_Index,
										 &loader) == FAIL)
	{
		LOG_ADD_EXCEPTION(ERROR_LOADING_PARAMETERS, Control->Data_Field_Index);
		return_value = FALSE;
	}
	else
	{
		if ((ALTERNATE_OPTION_TYPE)Control->F1_F0_Flags == (ALTERNATE_OPTION_TYPE)ALTERNATE_OPTION_ALTERNATE_WITH_OPTIONS)
		{
			for (uint8 index_conditions = 0; index_conditions < (loader.Length / (uint8)2); index_conditions++)
			{
				alternate_head_parameter = (ALTERNATE_CTRL_HEAD_TYPE*)(void*)&loader.Data[index_conditions*2U];

				if (((uint8)UNUSED_FEATURE_BYTE != alternate_head_parameter->Conditions_Index) &&
				(CONDITION_LEGACY_EVAL(alternate_head_parameter->Conditions_Index) == CONDITION_RESULT_TRUE))
				{
					break;
				}

				if (index_conditions == ((loader.Length / 2U)-1U ))
				{
					EventConditionsFalse();
				}
			}
		}
		else if ((ALTERNATE_OPTION_TYPE)Control->F1_F0_Flags == (ALTERNATE_OPTION_TYPE)ALTERNATE_OPTION_ALTERNATE_FIXED)
		{
			alternate_head_parameter = (ALTERNATE_CTRL_HEAD_TYPE*)(void*)loader.Data;
		}

		// Get the Maintain process parameters from the setting file.
		if (alternate_head_parameter != NULL)
		{
			if (SettingFile__MSettingBasicLoader(Control->Setting_File_Index,
												SF_PTR_ACU_PROCESS_ALTERNATE_CTRL,
												 alternate_head_parameter->Profile_Index,
												 &loader) == FAIL)
			{
				LOG_ADD_EXCEPTION(ERROR_LOADING_PARAMETERS, alternate_head_parameter->Profile_Index);
				return_value = FALSE;
			}
			else
			{
				if (Alternate[Alternate_Control_Cycle_Type].Num_Items > 0U)
				{
					uint16 temp_motor_speed;
					
					Alternate[Alternate_Control_Cycle_Type].Position_Old = Alternate[Alternate_Control_Cycle_Type].Position;

					// Read the setting file data into local variables for convenience.
					alternate_parameter = (ALTERNATE_PARAMETER_TYPE*)(void*)&loader.Data[loader.Length - ((uint16)Alternate[Alternate_Control_Cycle_Type].Num_Items*6U)];
					Alternate[Alternate_Control_Cycle_Type].Pulse_Duration = alternate_parameter->Pulse_Duration;
					Alternate[Alternate_Control_Cycle_Type].Motor_Acceleration = alternate_parameter->Motor_Acceleration;

					temp_motor_speed = (uint16)alternate_parameter->Wash_Motor_Speed * 25U;
					Alternate[Alternate_Control_Cycle_Type].Wash_Motor_Speed_Before_Reduction = temp_motor_speed;
					if (StabilityControl__GetSpeedReduction() != SC_NO_SPEED_REDUCTION)
					{
						StabilityControl__ReduceSpeed(&temp_motor_speed, alternate_parameter->Position);
					}
					Alternate[Alternate_Control_Cycle_Type].Wash_Motor_Speed = temp_motor_speed;

					Alternate[Alternate_Control_Cycle_Type].Position = alternate_parameter->Position;

					First_Request = TRUE;

					Alternate[Alternate_Control_Cycle_Type].Num_Items--;

					return_value = TRUE;
				}
				else
				{
					return_value = FALSE;
				}
			}
		}
	}

	return return_value;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void CheckStatus(void)
{
	/*
	API221_STS_ANY_ERROR_FOUND      = 0,             Any error found?
	API221_STS_IS_MOTOR_STOPPED     = 1,             Is motor stopped?
	API221_STS_REACHED_TARGET       = 2,             Target velocity == reference velocity
	API221_STS_IS_IDLE              = 3,             Is the motor idle? If the speed is zero, no error found...
	API221_STS_MOTOR_RUNNING        = 4
	*/
	#if (API221_CLIENT_FEATURE == ENABLED)
	if ((Drain_Pump_Pilot_Type == HBL_PILOT_EXTERNAL_MOTOR) && ((uint8)Ask_Drain_Pump_Status == (uint8)TRUE))
	{
		if ((AC_PendingStatus[MOTOR_0]) && (AC_PendingStatus[MOTOR_1]))
		{

			if (BIT_TEST(AC_Status[MOTOR_0],API221_STS_REACHED_TARGET) == 0U)
			{
				PumpControl__SetPreviousWashMotorSpeed(0);
			}

			if (BIT_TEST(AC_Status[MOTOR_1],API221_STS_REACHED_TARGET) == 0U)
			{
				PumpControl__SetPreviousDrainMotorSpeed(0);
			}

			if ((BIT_TEST(AC_Status[MOTOR_0],API221_STS_MOTOR_RUNNING) != 0U) && (BIT_TEST(AC_Status[MOTOR_0],API221_STS_REACHED_TARGET) == 0U))
			{
				if ((uint8)PumpFlushRequestMonitor__GetWashStopSentFlag() == (uint8)TRUE)
				{
					EventWaitStopped();
				}
				else
				{
					if (((BIT_TEST(AC_Status[MOTOR_1],API221_STS_MOTOR_RUNNING) != 0U) && (BIT_TEST(AC_Status[MOTOR_1],API221_STS_REACHED_TARGET) == 0U)) ||
					    ((BIT_TEST(AC_Status[MOTOR_1],API221_STS_MOTOR_RUNNING) != 0U) && (BIT_TEST(AC_Status[MOTOR_1],API221_STS_REACHED_TARGET) != 0U)))
					{
						EventWaitStopped();
					}
					else
					{
						EventRun();
					}
				}
			}
			else
			{
				if (((BIT_TEST(AC_Status[MOTOR_1],API221_STS_MOTOR_RUNNING) != 0U) && (BIT_TEST(AC_Status[MOTOR_1],API221_STS_REACHED_TARGET) == 0U)) ||
					((BIT_TEST(AC_Status[MOTOR_1],API221_STS_MOTOR_RUNNING) != 0U) && (BIT_TEST(AC_Status[MOTOR_1],API221_STS_REACHED_TARGET) != 0U)))
				{
					EventWaitStopped();
				}
				else
				{
					EventRun();
				}
			}

			PumpFlushRequestMonitor__ResetWashStopSentFlag();
			PumpFlushRequestMonitor__ResetDrainStopSentFlag();

			AC_PendingStatus[MOTOR_0] = FALSE;
			AC_PendingStatus[MOTOR_1] = FALSE;

			Ask_Drain_Pump_Status = FALSE;
		}


	}
	else // NO VSM DRAIN PUMP
	{
		if (AC_PendingStatus[MOTOR_0])
		{
			if ((BIT_TEST(AC_Status[MOTOR_0],API221_STS_REACHED_TARGET) == 0U))
			{
				PumpControl__SetPreviousWashMotorSpeed(0);
			}

			if ((BIT_TEST(AC_Status[MOTOR_0],API221_STS_MOTOR_RUNNING) != 0U) && (BIT_TEST(AC_Status[MOTOR_0],API221_STS_REACHED_TARGET) == 0U))
			{
				if ((uint8)PumpFlushRequestMonitor__GetWashStopSentFlag() == (uint8)TRUE)
				{
					EventWaitStopped();
				}
				else
				{
					EventRun();
				}
			}
			else
			{
				EventRun();
			}

			PumpFlushRequestMonitor__ResetWashStopSentFlag();

			AC_PendingStatus[MOTOR_0] = FALSE;
		}


	}

	if ((SystemTimers__NVCheckTimer(&AC_Wait_Status_Timer_Handle) == SYSTEMTIMERS_STATE_DONE) )
	{
		AC_Wait_Status_Attempts++;

		if (AC_Wait_Status_Attempts < (uint8)MAX_WAIT_STATUS_ATTEMPS)
		{
			EventErrorWaitStatus();
			LOG_ADD_EXCEPTION(ERROR_WAIT_STATUS, AC_Wait_Status_Attempts);
		}
		else
		{
			EventError(); // Process terminate
			LOG_ADD_EXCEPTION(ERROR_WAIT_STATUS_MAX_ATTEMPS, AC_Wait_Status_Attempts);
		}
	}
	#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SendRequestStatus(void)
{
	#if (API221_CLIENT_FEATURE == ENABLED)
	AC_PendingStatus[MOTOR_0] = FALSE;

	(void)API221MotionCtrl__RequestStatus(MOTOR_0, API221ApplReceivedStatus);

	if ((Drain_Pump_Pilot_Type == HBL_PILOT_EXTERNAL_MOTOR) && ((uint8)Ask_Drain_Pump_Status == (uint8)TRUE))
	{
		AC_PendingStatus[MOTOR_1] = FALSE;
		(void)API221MotionCtrl__RequestStatus(MOTOR_1, API221ApplReceivedStatus);
	}
	#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void WaitStopped(void)
{
	if ((SystemTimers__NVCheckTimer(&AC_Wait_Stopped_Timer_Handle) == SYSTEMTIMERS_STATE_DONE) )
	{
		AC_Wait_Stopped_Attempts++;

		if (AC_Wait_Stopped_Attempts < (uint8)MAX_WAIT_STOP_ATTEMPS)
		{
			// Send Stop
			PumpCommand__StopWithDefDecel(HBL_PRIORITY_APPLICATION_HI, HBL_LOAD_WASH_PUMP);
			PumpFlushRequestMonitor__SetWashStopSentFlag();

			if (Drain_Pump_Pilot_Type == HBL_PILOT_EXTERNAL_MOTOR)
			{
				PumpCommand__StopWithDefDecel(HBL_PRIORITY_APPLICATION_HI, HBL_LOAD_DRAIN_PUMP);
				PumpFlushRequestMonitor__SetDrainStopSentFlag();
			}

			EventErrorWaitStopped();
			#if (API221_CLIENT_FEATURE == ENABLED)
			AC_Wait_Status_Attempts = 0;
			#endif
			LOG_ADD_EXCEPTION(ERROR_WAIT_STOPPED, AC_Wait_Stopped_Attempts);
		}
		else
		{
			EventError(); // Process terminate
			LOG_ADD_EXCEPTION(ERROR_WAIT_STOPPED_MAX_ATTEMPS, AC_Wait_Stopped_Attempts);
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void CheckPulseDuration(void)
{
	static uint8 tick = 0;

	tick++;
	if (tick >= (uint8)10) // tick encrease each 100ms so each 1 second tick reset to 0
	{
		tick = 0;
		if (Alternate[Alternate_Control_Cycle_Type].Pulse_Duration > 0U)
		{
			Alternate[Alternate_Control_Cycle_Type].Pulse_Duration--;
		}
	}

	if (Alternate[Alternate_Control_Cycle_Type].Alternate_State == STATE_ALTERNATE_CONTROL_MNT_EXE)
	{
		if (Alternate[Alternate_Control_Cycle_Type].Pulse_Duration == 0U)
		{
			if (CheckNewProfile())
			{
				if (Pump_Pilot_Type == HBL_PILOT_EXTERNAL_MOTOR)
				{
					if (Alternate[Alternate_Control_Cycle_Type].Wash_Motor_Speed == 0U)
					{
						EventCheckNewProfileTrue(); // is speed is 0 go directly to Wait Diverter
					}
					else
					{
						#if (API221_CLIENT_FEATURE == ENABLED)
						AC_Wait_Status_Attempts = 0;
						#endif
						AC_Wait_Stopped_Attempts = 0;
						EventCheckNewProfileTrueExternalMotor();
					}
				}
				else
				{
					EventCheckNewProfileTrue();
				}
			}
			else
			{
				// Return to inizialize
				First_Request = TRUE;
				AlternateCtrl_Ready_To_Exit = TRUE;
				EventCheckNewProfileFalse();
			}
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void CheckTargetSpeed(void)
{
	if ((PumpControl__GetPreviousWashMotorSpeed() == Alternate[Alternate_Control_Cycle_Type].Wash_Motor_Speed) ||
			(Fault__IsInFaultState(FAULT_ID_MCU_WM_MCI_GENERIC_ERROR)) ||
			(Fault__IsInFaultState(FAULT_ID_MCU_WM_MCI_HARDWARE_OVERCURRENT))||
			(Fault__IsInFaultState(FAULT_ID_MCU_WM_MCI_OVER_VOLTAGE))||
			(Fault__IsInFaultState(FAULT_ID_MCU_WM_MCI_UNDER_VOLTAGE))||
			(Fault__IsInFaultState(FAULT_ID_MCU_WM_MCI_DRIVER_FAILURE))||
			(Fault__IsInFaultState(FAULT_ID_MCU_WM_MCI_INVERTER_OVER_TEMPERATURE))||
			(Fault__IsInFaultState(FAULT_ID_MCU_NO_MCU_COMMUNICATION)))
	{
		PumpControl__SetPreviousWashMotorSpeed(Alternate[Alternate_Control_Cycle_Type].Wash_Motor_Speed);
		EventCheckTargetSpeedTrue();
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetHomePosition(void)
{
	//uint8 first_pos_req;
	if ((uint8)AlternateLoad__GetAlternateLoadSearchHome() == (uint8)TRUE)
	{
	//	AlternateLoad__SetAlternateLoadSearchHome(FALSE);

	    //first_pos_req = POSITION_HOME_REQUEST;
		//Hbl__SetLoadRequestByLoadID(HBL_LOAD_DIVERTER, HBL_PRIORITY_DIVERTER, (uint8 *) &first_pos_req);
		Alternate[Alternate_Control_Cycle_Type].Position |= POSITION_HOME_REQUEST;
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void CheckWaitTargetTimer(void)
{
	if (SystemTimers__NVCheckTimer(&AC_Wait_Target_Timer_Handle) == SYSTEMTIMERS_STATE_DONE)
	{
		PumpControl__SetPreviousWashMotorSpeed(Alternate[Alternate_Control_Cycle_Type].Wash_Motor_Speed);
		EventCheckTargetSpeedTrue();

	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void StartWaitTargetTimer(void)
{
	uint32 ramp_time = (((uint32)Alternate[Alternate_Control_Cycle_Type].Wash_Motor_Speed/(uint32)Alternate[Alternate_Control_Cycle_Type].Motor_Acceleration)*1000U) + 5000U;
	(void)SystemTimers__NVSetTimerMS(&AC_Wait_Target_Timer_Handle, ramp_time,SYSTEMTIMERS_TYPE_DOWN_COUNTER);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void StartTimerWaitStatus(void)
{
	(void)SystemTimers__NVSetTimerMS(&AC_Wait_Status_Timer_Handle, 10000,SYSTEMTIMERS_TYPE_DOWN_COUNTER);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void StartTimerWaitStopped(void)
{
	(void)SystemTimers__NVSetTimerMS(&AC_Wait_Stopped_Timer_Handle, 10000,SYSTEMTIMERS_TYPE_DOWN_COUNTER);
}

//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Alternate Control State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Alternate Control state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void AlternateControlExecute(void)
{
    Alternate_Control_Transitioned = FALSE;
    switch(Alternate_Control_State)
    {
        case STATE_ALTERNATE_CONTROL_DIRECT_CONTROL:
            CheckTargetPosition();                  // Do function.
            if (Alternate_Control_Transitioned == FALSE)
            {
                ActivateDiverter();                 // Do function.
            }
            break;

        case STATE_ALTERNATE_CONTROL_WAIT_DIVERTER_TARGET:
            PilotPump();                            // Do function.
            if (Alternate_Control_Transitioned == FALSE)
            {
                CheckTargetPosition();              // Do function.
                if (Alternate_Control_Transitioned == FALSE)
                {
                    ActivateDiverter();             // Do function.
                    if (Alternate_Control_Transitioned == FALSE)
                    {
                        CheckPulseDuration();       // Do function.
                    }
                }
            }
            break;

        case STATE_ALTERNATE_CONTROL_READ_STATUS:
            CheckPulseDuration();                   // Do function.
            if (Alternate_Control_Transitioned == FALSE)
            {
                CheckStatus();                      // Do function.
            }
            break;

        case STATE_ALTERNATE_CONTROL_EXIT:
            ReturnTRUE();                           // Do function.
            break;

        case STATE_ALTERNATE_CONTROL_WAIT_MOTOR_TARGET:
            PilotPump();                            // Do function.
            if (Alternate_Control_Transitioned == FALSE)
            {
                CheckPulseDuration();               // Do function.
                if (Alternate_Control_Transitioned == FALSE)
                {
                    CheckTargetSpeed();             // Do function.
                    if (Alternate_Control_Transitioned == FALSE)
                    {
                        CheckWaitTargetTimer();     // Do function.
                    }
                }
            }
            break;

        case STATE_ALTERNATE_CONTROL_WAIT_STOPPED:
            WaitStopped();                          // Do function.
            break;

        case STATE_ALTERNATE_CONTROL_MNT_EXE:
            PilotPump();                            // Do function.
            if (Alternate_Control_Transitioned == FALSE)
            {
                CheckPulseDuration();               // Do function.
            }
            break;

        default:
            // There is nothing to do for all other values of Alternate_Control_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Alternate Control state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void AlternateControlInitialize(void)
{
    Alternate_Control_State = STATE_ALTERNATE_CONTROL_INITIALIZE;
    Alternate_Control_Transitioned = FALSE;
    ReadProcessParameters();                        // Entry Function.
    if (Alternate_Control_Transitioned == FALSE)
    {
        CheckProcessCall();                         // Entry Function.
    }
    Alternate_Control_Transitioned = TRUE;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The termination function for the Alternate Control state machine.
 * This function disables the state machine. Calls to StateMachineExecute() will have no effect.
 * The StateMachineInitialize() function must be called again to restart the state machine.
 * If the current state has an exit function, it will be called.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void AlternateControlTerminate(void)
{
    // None of the states have an Exit function.
    Alternate_Control_State = STATE_ALTERNATE_CONTROL_NONE;
    Alternate_Control_Transitioned = TRUE;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Alternate Fixed Event for the Alternate Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventAlternateFixed(void)
{
    switch (Alternate_Control_State)
    {
        case STATE_ALTERNATE_CONTROL_INITIALIZE:
            Alternate_Control_State = STATE_ALTERNATE_CONTROL_WAIT_DIVERTER_TARGET;
            Alternate_Control_Transitioned = FALSE;
            PilotPump();                            // Entry Function.
            if (Alternate_Control_Transitioned == FALSE)
            {
                CheckTargetPosition();              // Entry Function.
                if (Alternate_Control_Transitioned == FALSE)
                {
                    SetHomePosition();              // Entry Function.
                }
            }
            Alternate_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Alternate_Control_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Alternate Fixed External Motor Event for the Alternate Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventAlternateFixedExternalMotor(void)
{
    switch (Alternate_Control_State)
    {
        case STATE_ALTERNATE_CONTROL_INITIALIZE:
            Alternate_Control_State = STATE_ALTERNATE_CONTROL_READ_STATUS;
            Alternate_Control_Transitioned = FALSE;
            SendRequestStatus();                    // Entry Function.
            if (Alternate_Control_Transitioned == FALSE)
            {
                StartTimerWaitStatus();             // Entry Function.
            }
            Alternate_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Alternate_Control_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Check New Profile False Event for the Alternate Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventCheckNewProfileFalse(void)
{
    switch (Alternate_Control_State)
    {
        case STATE_ALTERNATE_CONTROL_WAIT_DIVERTER_TARGET:
        case STATE_ALTERNATE_CONTROL_MNT_EXE:
            Alternate_Control_State = STATE_ALTERNATE_CONTROL_INITIALIZE;
            Alternate_Control_Transitioned = FALSE;
            ReadProcessParameters();                // Entry Function.
            if (Alternate_Control_Transitioned == FALSE)
            {
                CheckProcessCall();                 // Entry Function.
            }
            Alternate_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Alternate_Control_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Check New Profile True Event for the Alternate Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventCheckNewProfileTrue(void)
{
    switch (Alternate_Control_State)
    {
        case STATE_ALTERNATE_CONTROL_MNT_EXE:
            Alternate_Control_State = STATE_ALTERNATE_CONTROL_WAIT_DIVERTER_TARGET;
            Alternate_Control_Transitioned = FALSE;
            PilotPump();                            // Entry Function.
            if (Alternate_Control_Transitioned == FALSE)
            {
                CheckTargetPosition();              // Entry Function.
                if (Alternate_Control_Transitioned == FALSE)
                {
                    SetHomePosition();              // Entry Function.
                }
            }
            Alternate_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Alternate_Control_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Check New Profile True External Motor Event for the Alternate Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventCheckNewProfileTrueExternalMotor(void)
{
    switch (Alternate_Control_State)
    {
        case STATE_ALTERNATE_CONTROL_WAIT_DIVERTER_TARGET:
        case STATE_ALTERNATE_CONTROL_MNT_EXE:
            Alternate_Control_State = STATE_ALTERNATE_CONTROL_READ_STATUS;
            Alternate_Control_Transitioned = FALSE;
            SendRequestStatus();                    // Entry Function.
            if (Alternate_Control_Transitioned == FALSE)
            {
                StartTimerWaitStatus();             // Entry Function.
            }
            Alternate_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Alternate_Control_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Check Targ Condition True Event for the Alternate Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventCheckTargConditionTrue(void)
{
    switch (Alternate_Control_State)
    {
        case STATE_ALTERNATE_CONTROL_DIRECT_CONTROL:
            Alternate_Control_State = STATE_ALTERNATE_CONTROL_EXIT;
            ReturnTRUE();                           // Entry Function.
            Alternate_Control_Transitioned = TRUE;
            break;

        case STATE_ALTERNATE_CONTROL_WAIT_DIVERTER_TARGET:
            Alternate_Control_State = STATE_ALTERNATE_CONTROL_MNT_EXE;
            Alternate_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Alternate_Control_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Check Target Speed True Event for the Alternate Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventCheckTargetSpeedTrue(void)
{
    switch (Alternate_Control_State)
    {
        case STATE_ALTERNATE_CONTROL_WAIT_MOTOR_TARGET:
            Alternate_Control_State = STATE_ALTERNATE_CONTROL_WAIT_DIVERTER_TARGET;
            Alternate_Control_Transitioned = FALSE;
            PilotPump();                            // Entry Function.
            if (Alternate_Control_Transitioned == FALSE)
            {
                CheckTargetPosition();              // Entry Function.
                if (Alternate_Control_Transitioned == FALSE)
                {
                    SetHomePosition();              // Entry Function.
                }
            }
            Alternate_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Alternate_Control_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Conditions False Event for the Alternate Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventConditionsFalse(void)
{
    switch (Alternate_Control_State)
    {
        case STATE_ALTERNATE_CONTROL_INITIALIZE:
            Alternate_Control_State = STATE_ALTERNATE_CONTROL_EXIT;
            ReturnTRUE();                           // Entry Function.
            Alternate_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Alternate_Control_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Direct Control Event for the Alternate Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDirectControl(void)
{
    switch (Alternate_Control_State)
    {
        case STATE_ALTERNATE_CONTROL_INITIALIZE:
            Alternate_Control_State = STATE_ALTERNATE_CONTROL_DIRECT_CONTROL;
            Alternate_Control_Transitioned = FALSE;
            CheckTargetPosition();                  // Entry Function.
            if (Alternate_Control_Transitioned == FALSE)
            {
                SetHomePosition();                  // Entry Function.
            }
            Alternate_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Alternate_Control_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Error Event for the Alternate Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventError(void)
{
    switch (Alternate_Control_State)
    {
        case STATE_ALTERNATE_CONTROL_READ_STATUS:
        case STATE_ALTERNATE_CONTROL_WAIT_STOPPED:
            Alternate_Control_State = STATE_ALTERNATE_CONTROL_EXIT;
            ReturnTRUE();                           // Entry Function.
            Alternate_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Alternate_Control_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Error Wait Status Event for the Alternate Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventErrorWaitStatus(void)
{
    switch (Alternate_Control_State)
    {
        case STATE_ALTERNATE_CONTROL_READ_STATUS:
            Alternate_Control_State = STATE_ALTERNATE_CONTROL_READ_STATUS;
            Alternate_Control_Transitioned = FALSE;
            SendRequestStatus();                    // Entry Function.
            if (Alternate_Control_Transitioned == FALSE)
            {
                StartTimerWaitStatus();             // Entry Function.
            }
            Alternate_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Alternate_Control_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Error Wait Stopped Event for the Alternate Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventErrorWaitStopped(void)
{
    switch (Alternate_Control_State)
    {
        case STATE_ALTERNATE_CONTROL_WAIT_STOPPED:
            Alternate_Control_State = STATE_ALTERNATE_CONTROL_READ_STATUS;
            Alternate_Control_Transitioned = FALSE;
            SendRequestStatus();                    // Entry Function.
            if (Alternate_Control_Transitioned == FALSE)
            {
                StartTimerWaitStatus();             // Entry Function.
            }
            Alternate_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Alternate_Control_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Run Event for the Alternate Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventRun(void)
{
    switch (Alternate_Control_State)
    {
        case STATE_ALTERNATE_CONTROL_READ_STATUS:
        case STATE_ALTERNATE_CONTROL_WAIT_STOPPED:
            Alternate_Control_State = STATE_ALTERNATE_CONTROL_WAIT_MOTOR_TARGET;
            StartWaitTargetTimer();                 // Entry Function.
            Alternate_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Alternate_Control_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Wait Stopped Event for the Alternate Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventWaitStopped(void)
{
    switch (Alternate_Control_State)
    {
        case STATE_ALTERNATE_CONTROL_READ_STATUS:
            Alternate_Control_State = STATE_ALTERNATE_CONTROL_WAIT_STOPPED;
            StartTimerWaitStopped();                // Entry Function.
            Alternate_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Alternate_Control_State.
            break;
    }
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

#if (API221_CLIENT_FEATURE == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Use ExternalEventExtEvMCUErrReceive (RECEIVE_ERROR and RECEIVE_NO_ERROR)
 * Process information received from MCU about FF
 * @param motor = The motor ID related to the FF received
 * @param failures = bitmap of FF
 */

static void API221ApplReceivedStatus (API221_CLIENT_MOTOR_INDEX_TYPE motor, uint32 status)
{
	AC_Status[motor] = status;
	AC_PendingStatus[motor] = TRUE;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Handle events published through API221.
 * This function is called by the communication thread.
 * @param event = The event that was published through API221.
 */
static void API221HandleMotorEventAC(API221_EVENT_TYPE event)
{
	switch (event.Event)
	{
		case API221_EVENT_STOPPED:
			if (SystemTimers__NVCheckTimer(&AC_Wait_MCU_Reset_Timer_Handle) != SYSTEMTIMERS_STATE_RUNNING)
			{
				EventRun();
			}
			break;
		case API221_EVENT_REACHED_TARGET:
			PumpControl__SetPreviousWashMotorSpeed(Alternate[Alternate_Control_Cycle_Type].Wash_Motor_Speed);
			EventCheckTargetSpeedTrue();
			break;
		case API221_EVENT_DEBLOCKING_READY:
			if (event.Motor == (API221_CLIENT_MOTOR_INDEX_TYPE)MOTOR_0)
			{
				PumpControl__SetWashPumpDeblockingReady(TRUE);
			}
			else if (event.Motor == (API221_CLIENT_MOTOR_INDEX_TYPE)MOTOR_1)
			{
				PumpControl__SetDrainPumpDeblockingReady(TRUE);
			}
			break;
		default:
			// Ignore all other events
			break;
	}
}
#endif

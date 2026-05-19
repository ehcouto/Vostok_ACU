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
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "WashMotorFaultMonitor_prv.h"
#include "WashMotorFaultMonitor.h"

#include "SystemConfig.h"
#include "Fault.h"
#if (HBL_LOAD_NUM_TRIACMCI > 0)
#include "Mci.h"
#endif /*(HBL_LOAD_NUM_TRIACMCI > 0)*/
#if (API221_CLIENT_FEATURE == ENABLED)
#include "API221MotionCtrl.h"
#include "ExternalMotorDeclare.h"
#include "VSDrainMotorFaultMonitor.h"
#endif /*(API221_CLIENT_FEATURE == ENABLED)*/
#include "Hbl.h"
#include "Interpreter.h"
#include "Log.h"
#include "Micro.h"
#include "MotorTypeDefs.h"
#include "Selector.h"
#include "SettingFile.h"
#include "SystemTimers.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define MODULE_NAME MODULE_WASH_MOTOR_FAULT_MONITOR

typedef enum MODULE_WASH_MOTOR_FAULT_MONITOR_LOG_MESSAGE_ID_ENUM
{
	ERROR_MISSING_PARAMETERS  = 1,
} MODULE_WASH_MOTOR_FAULT_MONITOR_LOG_MESSAGE_ID_TYPE;

typedef PACKED struct WASH_MOTOR_TYPE_PARAMS_STRUCT
{
	uint8 Wash_Motor_Type;
	uint8 Unused;
} WASH_MOTOR_TYPE_PARAMS_TYPE;

typedef PACKED struct WASH_MOTOR_FAULT_PARAMS_STRUCT
{
   uint16 Current_Sense_Wash_Motor_Min;
   uint16 Current_Sense_Wash_Motor_Max;
   uint16 Current_Sense_AD_Saturation_Current;
   uint16 Current_Sense_AD_Resolution;
} WASH_MOTOR_FAULT_PARAMS_TYPE;

typedef enum
{
    CONDITION_UNDETECTED               = 0,
	CONDITION_DETECTED                 = 1,
	CONDITION_UNUSED				   = 0xFF
}CONDITION_DETECTED_TYPE;

#define WASH_MOTOR_FIRST_START_SPIKE_TIMER_MAX				5 // 500 ms

static APL_WASH_MOTOR_TYPE     Motor_Type;          //!< Motor type

static WASH_MOTOR_FAULT_PARAMS_TYPE Wash_Motor_Fault;

static BOOL_TYPE Gws_Error;

static BOOL_TYPE Disable_Wash_Motor_Fault_Monitor;
// State of the Wash_Motorin the previous iteration
static uint8 Wash_Motor_Previous_State;
// State of the Wash_Motor in the current iteration
static uint8 Wash_Motor_State;

static CONDITION_DETECTED_TYPE Condition_Detected;

static FAULT_ID_TYPE Wash_Motor_Fault_Id;

#if (API221_CLIENT_FEATURE == ENABLED)
static BOOL_TYPE Wash_Motor_VSM_Failure;
static uint32 Wash_Motor_Failures;
static API221_CLIENT_MOTOR_INDEX_TYPE Motor_Fault;
static SYSTEMTIMER_NV_TIMER_TYPE Wait_Clear_Fault_Timer_Handle;
static SYSTEMTIMER_NV_TIMER_TYPE Wait_Clear_Fault_Stop_Motor_Timer_Handle;
#endif

static uint8 Motor_On;
static uint8 Motor_Off;

static uint8 Wash_Motor_First_Start_Spike_Timer;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

BOOL_TYPE GetWashMotorError(void);
BOOL_TYPE Is_WashMotor_Rotating(void);
BOOL_TYPE IsInFaultState(void);
#if (API221_CLIENT_FEATURE == ENABLED)
static void API221ApplReceivedFailureFlags (API221_CLIENT_MOTOR_INDEX_TYPE motor, uint32 failures);
static void API221HandleMotorEventFailure(API221_EVENT_TYPE event);
static void ResetFaults(void);
static void SetConditionDetected(void);
#endif

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//----------------------------------------------------------------------------
/**
 *  @brief      WashMotorFaultMonitor__Initialize
 *  @details    Function used to initialize the Wash Motor Fault Monitor.
 *
 */
void WashMotorFaultMonitor__Initialize(void)
{
	if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
	{
		SETTINGFILE_LOADER_TYPE loader;
		WASH_MOTOR_TYPE_PARAMS_TYPE  *wash_motor_type_ptr;

		Disable_Wash_Motor_Fault_Monitor = FALSE;

		Condition_Detected = CONDITION_UNUSED;

		Wash_Motor_State = OFF;
		Wash_Motor_Previous_State = OFF;

		Motor_On = ON;
		Motor_Off = OFF;

		Gws_Error = FALSE;

		Wash_Motor_First_Start_Spike_Timer = 0;

		#if (API221_CLIENT_FEATURE == ENABLED)
		SystemTimers__NVResetTimer(&Wait_Clear_Fault_Timer_Handle);
		SystemTimers__NVResetTimer(&Wait_Clear_Fault_Stop_Motor_Timer_Handle);
		#endif

		wash_motor_type_ptr = (WASH_MOTOR_TYPE_PARAMS_TYPE *)NULL;

		if (SettingFile__BasicLoader(SF_PTR_ACU_PRODUCT_CONFIGURATION_DATA, DSPL_WASH_MOTOR_TYPE, &loader) == PASS)
		{
			wash_motor_type_ptr = (WASH_MOTOR_TYPE_PARAMS_TYPE *)(void *)loader.Data;

			Motor_Type = (APL_WASH_MOTOR_TYPE) wash_motor_type_ptr->Wash_Motor_Type;

			Wash_Motor_Fault_Id = FAULT_ID_NONE;
			#if (API221_CLIENT_FEATURE == ENABLED)
			if (Motor_Type == WM_VARIAB_SPEED_3PH)
			{
				Motor_On = EXTERNAL_MOTOR_RUN;
				Motor_Off = EXTERNAL_MOTOR_STOP;
			}
			#endif
		}
		else
		{
			Disable_Wash_Motor_Fault_Monitor = TRUE;
			LOG_ADD_EXCEPTION (ERROR_MISSING_PARAMETERS, 0);
		}

		if (Motor_Type == WM_SINGLE_SPEED_ASYNCH_SPH_PSC)
		{
			WASH_MOTOR_FAULT_PARAMS_TYPE *wash_motor_fault_params;
			wash_motor_fault_params = (WASH_MOTOR_FAULT_PARAMS_TYPE*)Fault__GetSpecificData(FAULT_ID_SINGLE_SPEED_WASH_MOTOR);

			if (wash_motor_fault_params != NULL)
			{
				Wash_Motor_Fault.Current_Sense_Wash_Motor_Min = ENDIANU16_STON(wash_motor_fault_params->Current_Sense_Wash_Motor_Min);
				Wash_Motor_Fault.Current_Sense_Wash_Motor_Max = ENDIANU16_STON(wash_motor_fault_params->Current_Sense_Wash_Motor_Max);
				Wash_Motor_Fault.Current_Sense_AD_Saturation_Current = ENDIANU16_STON(wash_motor_fault_params->Current_Sense_AD_Saturation_Current);
				Wash_Motor_Fault.Current_Sense_AD_Resolution = ENDIANU16_STON(wash_motor_fault_params->Current_Sense_AD_Resolution);

			}
			else
			{
				Disable_Wash_Motor_Fault_Monitor = TRUE;
				LOG_ADD_EXCEPTION (ERROR_MISSING_PARAMETERS, FAULT_ID_SINGLE_SPEED_WASH_MOTOR);
			}
		}

		#if (API221_CLIENT_FEATURE == ENABLED)
		if (Disable_Wash_Motor_Fault_Monitor == FALSE)
		{
			API221MotionCtrl__RegisterForEvents(API221HandleMotorEventFailure);
		}
		Wash_Motor_VSM_Failure = FALSE;
		#endif
	}
}

//----------------------------------------------------------------------------
/**
 *  @brief      WashMotorFaultMonitor__Execution
 *  @details    Function used to perform the Wash Motor Fault Monitor.
 *
 */
void WashMotorFaultMonitor__Execution(void)
{
	if (Disable_Wash_Motor_Fault_Monitor == FALSE)
	{
		CYCLE_NAME_TYPE cycle_name = Selector__GetCycleNameId(Interpreter__GetCurrentSFIndex(COMPARTMENT_DEFAULT),
									 	 	 	 	 	 	  COMPARTMENT_DEFAULT,
															  Interpreter__GetStatus().Sub_Cycle[COMPARTMENT_DEFAULT]);

		CYCLE_STATE_TYPE cycle_state = Selector__GetCycleState(Interpreter__GetCurrentSFIndex(COMPARTMENT_DEFAULT),
															   COMPARTMENT_DEFAULT,
															   Interpreter__GetCurrentStatement(COMPARTMENT_DEFAULT).Cycle);

		if ((Interpreter__IsSubCycleRunning(COMPARTMENT_DEFAULT)) &&
			(cycle_name == CYCLE_NAME_WASH_MOTOR_FAULT) &&
			(cycle_state == CYCLE_STATE_RUN))
		{
			Wash_Motor_State = ((uint8 *)Hbl__GetProcessedLoad(Hbl__GetLoadIndexByLoadID(HBL_LOAD_WASH_PUMP)))[0];

			#if (API221_CLIENT_FEATURE == ENABLED)
			if (Wash_Motor_State == (uint8)EXTERNAL_MOTOR_ROTATE_X_DEGREES)
			{
				Wash_Motor_State = Motor_On;
			}
			#endif
			if (Wash_Motor_State != Wash_Motor_Previous_State)
			{
				Wash_Motor_Previous_State = Wash_Motor_State;

				if (Wash_Motor_State == Motor_Off)
				{
					#if (API221_CLIENT_FEATURE == ENABLED)
					if ((SystemTimers__NVCheckTimer(&Wait_Clear_Fault_Timer_Handle) == SYSTEMTIMERS_STATE_RUNNING) &&
						(Condition_Detected == CONDITION_UNDETECTED))
					{
						SetConditionDetected();
						Condition_Detected = CONDITION_DETECTED;
					}
					#endif
					if (Condition_Detected == CONDITION_UNDETECTED) // Fault Solved
					{
						if (Motor_Type != WM_VARIAB_SPEED_3PH)
						{
							if (Fault__IsObserverActive(Wash_Motor_Fault_Id))
							{
								Fault__ConditionRemoved(Wash_Motor_Fault_Id);
							}
						}
						#if (API221_CLIENT_FEATURE == ENABLED)
						else
						{
							ResetFaults();
						}
						#endif
						(void)Interpreter__TerminateSubCycle(COMPARTMENT_DEFAULT);
						Condition_Detected = CONDITION_UNUSED;
					}
					else if  (Condition_Detected == CONDITION_DETECTED) // ready for next attempt
					{
						Condition_Detected = CONDITION_UNDETECTED;
					}

					#if (API221_CLIENT_FEATURE == ENABLED)
					if (Wash_Motor_VSM_Failure != FALSE)
					{
						Wash_Motor_VSM_Failure = FALSE;
					}
					#endif
				}
				#if (API221_CLIENT_FEATURE == ENABLED)
				else if (Wash_Motor_State == Motor_On)
				{
					if (Motor_Type == WM_VARIAB_SPEED_3PH)
					{
						SystemTimers__NVResetTimer(&Wait_Clear_Fault_Stop_Motor_Timer_Handle);
					}
				}
				#endif
			}
			else
			{
				if ((Wash_Motor_State == Motor_On) &&
					(Condition_Detected != CONDITION_DETECTED))
				{
					if (GetWashMotorError())
					{
						if (Motor_Type != WM_VARIAB_SPEED_3PH)
						{
							Fault__ConditionDetected(Wash_Motor_Fault_Id);
						}
						#if (API221_CLIENT_FEATURE == ENABLED)
						else
						{
							SetConditionDetected();
						}
						#endif
						Condition_Detected = CONDITION_DETECTED;
					}
					else
					{
						Condition_Detected = CONDITION_UNDETECTED;
					}
				}
				#if (API221_CLIENT_FEATURE == ENABLED)
				else if (Wash_Motor_State == Motor_Off)
				{
					if (Motor_Type == WM_VARIAB_SPEED_3PH)
					{
						if ((SystemTimers__NVCheckTimer(&Wait_Clear_Fault_Stop_Motor_Timer_Handle) != SYSTEMTIMERS_STATE_RUNNING) &&
							(SystemTimers__NVCheckTimer(&Wait_Clear_Fault_Timer_Handle) != SYSTEMTIMERS_STATE_RUNNING))
						{
							//send Clear Fault to MCU every 1 sec
							(void)API221MotionCtrl__ClearFailureFlags(Motor_Fault);
							(void)SystemTimers__NVSetTimerMS(&Wait_Clear_Fault_Stop_Motor_Timer_Handle, 1000,SYSTEMTIMERS_TYPE_DOWN_COUNTER);
						}
					}
				}
				#endif
			}

			// If fault is detected, terminate subcycle
			if (IsInFaultState() != FALSE)
			{
				(void)Interpreter__TerminateSubCycle(COMPARTMENT_DEFAULT);
				Condition_Detected = CONDITION_UNUSED;
			}
		}
		else
		{

			if (cycle_state == CYCLE_STATE_RUN)
			{
				#if (API221_CLIENT_FEATURE == ENABLED)
				Wash_Motor_State = ((uint8 *)Hbl__GetProcessedLoad(Hbl__GetLoadIndexByLoadID(HBL_LOAD_WASH_PUMP)))[0];
				#endif
				if (GetWashMotorError()
					#if (API221_CLIENT_FEATURE == ENABLED)
					|| ((SystemTimers__NVCheckTimer(&Wait_Clear_Fault_Timer_Handle) == SYSTEMTIMERS_STATE_RUNNING) &&
							(Wash_Motor_State == Motor_On))
					#endif
					)
				{
					if (Motor_Type != WM_VARIAB_SPEED_3PH)
					{
						if (Fault__IsInFaultState(FAULT_ID_SINGLE_SPEED_WASH_MOTOR) == FALSE)
						{
							Wash_Motor_Fault_Id = FAULT_ID_SINGLE_SPEED_WASH_MOTOR;
							Fault__ConditionDetected(Wash_Motor_Fault_Id);
						}
					}
					#if (API221_CLIENT_FEATURE == ENABLED)
					else
					{
						SetConditionDetected();
						Wash_Motor_State = OFF;
						Wash_Motor_Previous_State = OFF;
					}
					#endif
					Condition_Detected = CONDITION_UNUSED;

				}
			}
			else if ((cycle_state != CYCLE_STATE_FAULT) &&
					(cycle_name != CYCLE_NAME_WASH_MOTOR_FAULT)) // if the cycle state is PAUSE of the Subcycle CYCLE_NAME_WASH_MOTOR_FAULT then not reset faults
			{
				(void)GetWashMotorError(); // only for clean the error but in state != RUN&&FAULT shouldn't happen

				if (Motor_Type != WM_VARIAB_SPEED_3PH)
				{
					if (Fault__IsObserverActive(Wash_Motor_Fault_Id))
					{
						Fault__ConditionRemoved(Wash_Motor_Fault_Id);
					}
				}
				#if (API221_CLIENT_FEATURE == ENABLED)
				else
				{
					ResetFaults();
				}
				#endif
			}
		}

		#if (API221_CLIENT_FEATURE == ENABLED)
		if (SystemTimers__NVCheckTimer(&Wait_Clear_Fault_Timer_Handle) == SYSTEMTIMERS_STATE_DONE)
		{
			(void)API221MotionCtrl__ClearFailureFlags(Motor_Fault);
			(void)SystemTimers__NVSetTimerMS(&Wait_Clear_Fault_Timer_Handle, 500,SYSTEMTIMERS_TYPE_DOWN_COUNTER);
		}
		#endif
	}
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

BOOL_TYPE GetWashMotorError(void)
{
	BOOL_TYPE ret_val;
	ret_val = FALSE;
	switch (Motor_Type)
	{
		case WM_VARIAB_SPEED_3PH:				// VSM Motor
		{
			#if (API221_CLIENT_FEATURE == ENABLED)
			ret_val = Wash_Motor_VSM_Failure;
			if (Wash_Motor_VSM_Failure != FALSE)
			{
				Wash_Motor_VSM_Failure = FALSE;
			}
			#endif
		}
		break;

		#if (HBL_LOAD_NUM_TRIACMCI > 0)
		case WM_SINGLE_SPEED_ASYNCH_SPH_PSC:	// RIF motor
		{
			if (Is_WashMotor_Rotating() != FALSE)
			{
				if (Wash_Motor_First_Start_Spike_Timer >= (uint8)WASH_MOTOR_FIRST_START_SPIKE_TIMER_MAX)
				{
					uint16 ad_value = (uint16)Mci__GetAnalog(MOTOR0,  MCI_AI_CURR_PHASE_P2P);
					// need to convert to mA
					uint16 mA = (uint16)((ad_value*Wash_Motor_Fault.Current_Sense_AD_Saturation_Current)/Wash_Motor_Fault.Current_Sense_AD_Resolution);

					if ((mA < Wash_Motor_Fault.Current_Sense_Wash_Motor_Min) || (mA > Wash_Motor_Fault.Current_Sense_Wash_Motor_Max))
					{
						ret_val = TRUE;
					}
				}
				else
				{
					Wash_Motor_First_Start_Spike_Timer++;
				}
			}
			else
			{
				Wash_Motor_First_Start_Spike_Timer = 0;
			}
		}
		break;

		case WM_SINGLE_SPEED_SYNCH_SPH_AW:		// Single Phase Permanent Magnet Synchronous Motor Permanent Wash - GWS
		case WM_SINGLE_SPEED_SYNCH_SPH_PW:		// Single Phase Permanent Magnet Synchronous Motor Alternating Wash - GWS - (Default)
		{
			if ((BOOL_TYPE)MCI_GETDIGITAL(MOTOR0, MCI_DI_MOTOR_STOP) != (BOOL_TYPE)FALSE)
			{
				if (((BOOL_TYPE)MCI_GETANYERROR(MOTOR0) != (BOOL_TYPE)FALSE) &&
					(Gws_Error == FALSE))
				{
					Gws_Error = TRUE; // wait 100 ms before of ClearError
				}
				else if (((BOOL_TYPE)MCI_GETANYERROR(MOTOR0) != (BOOL_TYPE)FALSE) &&
						(Gws_Error != FALSE))
				{
					(void)Mci__ClearError(MOTOR0);
					ret_val = TRUE;
					Gws_Error = FALSE;
				}
			}
		}
		break;
		#endif
		default:
		// There is nothing to do for all other values
		break;
	}

	return ret_val;
}

BOOL_TYPE Is_WashMotor_Rotating(void)
{
    uint8 return_value;
    return_value = *((uint8*)Hbl__GetProcessedLoad(Hbl__GetLoadIndexByLoadID(HBL_LOAD_WASH_PUMP)));
    return (BOOL_TYPE)return_value;
}

BOOL_TYPE IsInFaultState(void)
{
	BOOL_TYPE ret = FALSE;
	if (Motor_Type != WM_VARIAB_SPEED_3PH)
	{
		ret = Fault__IsInFaultState(Wash_Motor_Fault_Id);
	}
	#if (API221_CLIENT_FEATURE == ENABLED)
	else
	{
		for (uint8 index_fault = (uint8)FAULT_ID_VS_WASH_MOTOR_MCI_LOCKED_ROTOR; index_fault < (uint8)FAULT_ID_MCU_NO_MCU_COMMUNICATION; index_fault++)
		{
			if (Fault__IsInFaultState((FAULT_ID_TYPE)index_fault))
			{
				ret = TRUE;
			}
		}
	}
	#endif

	return ret;
}

#if (API221_CLIENT_FEATURE == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Use ExternalEventExtEvMCUErrReceive (RECEIVE_ERROR and RECEIVE_NO_ERROR)
 * Process information received from MCU about FF
 * @param motor = The motor ID related to the FF received
 * @param failures = bitmap of FF
 */
static void API221ApplReceivedFailureFlags (API221_CLIENT_MOTOR_INDEX_TYPE motor, uint32 failures)
{
    if (failures != 0U)
    {
        if (motor == MOTOR_0)
        {
            Wash_Motor_Failures = failures;
            Wash_Motor_VSM_Failure = TRUE;
            Motor_Fault = motor;
            (void)SystemTimers__NVSetTimerMS(&Wait_Clear_Fault_Timer_Handle, 500,SYSTEMTIMERS_TYPE_DOWN_COUNTER);
        }
        else if (motor == MOTOR_1)
        {
            VSDrainMotorFaultMonitor__SetDrainMotorFailures(failures);
            VSDrainMotorFaultMonitor__SetDrainMotorFailureFlag(TRUE);
        }
    }

}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Handle events published through API221.
 * This function is called by the communication thread.
 * @param event = The event that was published through API221.
 */
static void API221HandleMotorEventFailure(API221_EVENT_TYPE event)
{
    switch (event.Event)
    {
		case API221_EVENT_ERROR:
		{
			(void)API221MotionCtrl__RequestFailureFlags(event.Motor, API221ApplReceivedFailureFlags);
		}
		break;

		case API221_EVENT_REMOVED_ERROR:
		{
			if (event.Motor == (API221_CLIENT_MOTOR_INDEX_TYPE)MOTOR_0)
			{
				SystemTimers__NVResetTimer(&Wait_Clear_Fault_Timer_Handle);
			}
			else if (event.Motor == (API221_CLIENT_MOTOR_INDEX_TYPE)MOTOR_1)
			{
				VSDrainMotorFaultMonitor__ResetTimerClearMotorFailureFlag();
			}
			// Keep requesting failures until all errors are cleaned
			(void)API221MotionCtrl__RequestFailureFlags(event.Motor, API221ApplReceivedFailureFlags);
		}
		break;

		default:
			// Ignore all other events
			break;
    }

}

static void SetConditionDetected(void)
{
	if (Wash_Motor_Failures == ((uint32)1 << (uint32)API221_FF_ANY_ERROR_FOUND))
	{
		Fault__ConditionDetected(FAULT_ID_MCU_WM_MCI_GENERIC_ERROR);
	}

	if (BIT_TEST((uint32)Wash_Motor_Failures,API221_FF_DCBUS_OVER_CURRENT) != 0U)
	{
		Fault__ConditionDetected(FAULT_ID_MCU_WM_MCI_HARDWARE_OVERCURRENT);
	}

	if (BIT_TEST((uint32)Wash_Motor_Failures,API221_FF_DCBUS_OVER_VOLTAGE) != 0U)
	{
		Fault__ConditionDetected(FAULT_ID_MCU_WM_MCI_OVER_VOLTAGE);
	}

	if (BIT_TEST((uint32)Wash_Motor_Failures,API221_FF_DCBUS_UNDER_VOLTAGE) != 0U)
	{
		Fault__ConditionDetected(FAULT_ID_MCU_WM_MCI_UNDER_VOLTAGE);
	}

	if (BIT_TEST((uint32)Wash_Motor_Failures,API221_FF_ELECTRONIC_FAILED) != 0U)
	{
		Fault__ConditionDetected(FAULT_ID_MCU_WM_MCI_DRIVER_FAILURE);
	}

	if (BIT_TEST((uint32)Wash_Motor_Failures,API221_FF_INVERTER_OVER_TEMP) != 0U)
	{
		Fault__ConditionDetected(FAULT_ID_MCU_WM_MCI_INVERTER_OVER_TEMPERATURE);
	}

	if (BIT_TEST((uint32)Wash_Motor_Failures,API221_FF_LOCKED_ROTOR) != 0U)
	{
		Fault__ConditionDetected(FAULT_ID_VS_WASH_MOTOR_MCI_LOCKED_ROTOR);
	}

	if (BIT_TEST((uint32)Wash_Motor_Failures,API221_FF_PHASE_LOST) != 0U)
	{
		Fault__ConditionDetected(FAULT_ID_VS_WASH_MOTOR_MCI_MOTOR_PHASE_LOST);
	}

	if (BIT_TEST((uint32)Wash_Motor_Failures,API221_FF_MOTOR_OVERLOADED) != 0U)
	{
		Fault__ConditionDetected(FAULT_ID_VS_WASH_MOTOR_MCI_MOTOR_OVERLOAD);
	}

	if (BIT_TEST((uint32)Wash_Motor_Failures,API221_FF_MOTOR_OVER_CURRENT) != 0U)
	{
		Fault__ConditionDetected(FAULT_ID_VS_WASH_MOTOR_MCI_SOFTWARE_OVERCURRENT);
	}

	if (BIT_TEST((uint32)Wash_Motor_Failures,API221_FF_SPEED_CHECK_FAILED) != 0U)
	{
		Fault__ConditionDetected(FAULT_ID_VS_WASH_MOTOR_MCI_SPEED_CHECK);
	}
}
static void ResetFaults(void)
{
	for (uint8 index_fault = (uint8)FAULT_ID_VS_WASH_MOTOR_MCI_LOCKED_ROTOR; index_fault < (uint8)FAULT_ID_MCU_NO_MCU_COMMUNICATION; index_fault++)
	{
		if (Fault__IsObserverActive((FAULT_ID_TYPE)index_fault))
		{
			Fault__ConditionRemoved((FAULT_ID_TYPE)index_fault);
		}
	}
}
#endif

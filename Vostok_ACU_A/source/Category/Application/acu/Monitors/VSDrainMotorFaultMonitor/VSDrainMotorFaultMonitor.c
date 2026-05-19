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
#include "C_Extensions.h"
#include "VSDrainMotorFaultMonitor.h"

#include "SystemConfig.h"
#if (API221_CLIENT_FEATURE == ENABLED)
#include "API221MotionCtrl.h"
#include "ExternalMotorDeclare.h"
#include "Fault.h"
#include "PumpCommand.h"
#endif /*(API221_CLIENT_FEATURE == ENABLED)*/
#include "Hbl.h"
#include "Interpreter.h"
#include "Log.h"
#include "Micro.h"
#include "Selector.h"
#include "SettingFile.h"
#include "SystemTimers.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define MODULE_NAME MODULE_VSDRAIN_MOTOR_FAULT_MONITOR

typedef enum MODULE_VSDRAIN_MOTOR_FAULT_MONITOR_LOG_MESSAGE_ID_ENUM
{
	ERROR_MISSING_PARAMETERS  = 1,
	ERROR_NO_VSM_DRAIN_MOTOR  = 2,
} MODULE_VSDRAIN_MOTOR_FAULT_MONITOR_LOG_MESSAGE_ID_TYPE;

typedef PACKED struct DRAIN_MOTOR_TYPE_PARAMS_STRUCT
{
	uint8 Drain_Motor_Type;
	uint8 Unused;
} DRAIN_MOTOR_TYPE_PARAMS_TYPE;


typedef enum
{
    UNUSED_DRAIN_MOTOR_TYPE_0           = 0,     // Not Used 0
    DM_VARIAB_SPEED_3PH                 = 1,     // VSM Motor
    DM_SINGLE_SPEED_ASYNCH		        = 2,     // RIF motor
}APL_DRAIN_MOTOR_TYPE;


typedef enum
{
    CONDITION_UNDETECTED               = 0,
	CONDITION_DETECTED                 = 1,
	CONDITION_UNUSED				   = 0xFF
}CONDITION_DETECTED_TYPE;

static APL_DRAIN_MOTOR_TYPE     Motor_Type;          //!< Motor type

static BOOL_TYPE Disable_Drain_Motor_Fault_Monitor;
// State of the Drain_Motor in the previous iteration
static uint8 Drain_Motor_Previous_State;
// State of the Drain_Motor in the current iteration
static uint8 Drain_Motor_State;

static CONDITION_DETECTED_TYPE DM_Condition_Detected;

#if (API221_CLIENT_FEATURE == ENABLED)
static BOOL_TYPE Drain_Motor_VSM_Failure;
static uint32 Drain_Motor_Failures;
static SYSTEMTIMER_NV_TIMER_TYPE Wait_Clear_Fault_Timer_Handle;
static SYSTEMTIMER_NV_TIMER_TYPE Wait_Clear_Fault_Stop_Motor_Timer_Handle;
#endif

static uint8 Motor_On;
static uint8 Motor_Off;


//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

BOOL_TYPE GetDrainMotorError(void);
BOOL_TYPE DrainIsInFaultState(void);
#if (API221_CLIENT_FEATURE == ENABLED)
static void ResetFaults(void);
static void SetConditionDetected(void);
#endif

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//----------------------------------------------------------------------------
/**
 *  @brief      VSDrainMotorFaultMonitor__Initialize
 *  @details    Function used to initialize the Drain Motor Fault Monitor.
 *
 */
void VSDrainMotorFaultMonitor__Initialize(void)
{
	if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
	{
		SETTINGFILE_LOADER_TYPE loader;
		DRAIN_MOTOR_TYPE_PARAMS_TYPE  *drain_motor_type_ptr;

		Disable_Drain_Motor_Fault_Monitor = FALSE;

		DM_Condition_Detected = CONDITION_UNUSED;

		Drain_Motor_State = OFF;
		Drain_Motor_Previous_State = OFF;

		Motor_On = ON;
		Motor_Off = OFF;

		#if (API221_CLIENT_FEATURE == ENABLED)
		SystemTimers__NVResetTimer(&Wait_Clear_Fault_Timer_Handle);
		SystemTimers__NVResetTimer(&Wait_Clear_Fault_Stop_Motor_Timer_Handle);
		#endif
		drain_motor_type_ptr = (DRAIN_MOTOR_TYPE_PARAMS_TYPE *)NULL;

		if (SettingFile__BasicLoader(SF_PTR_ACU_PRODUCT_CONFIGURATION_DATA, DSPL_DRAIN_MOTOR_TYPE, &loader) == PASS)
		{
			drain_motor_type_ptr = (DRAIN_MOTOR_TYPE_PARAMS_TYPE *)(void *)loader.Data;

			Motor_Type = (APL_DRAIN_MOTOR_TYPE) drain_motor_type_ptr->Drain_Motor_Type;

			#if (API221_CLIENT_FEATURE == ENABLED)
			if (Motor_Type == DM_VARIAB_SPEED_3PH)
			{
				Motor_On = EXTERNAL_MOTOR_RUN;
				Motor_Off = EXTERNAL_MOTOR_STOP;
			}
			#endif
		}
		else
		{
			Disable_Drain_Motor_Fault_Monitor = TRUE;
			LOG_ADD_EXCEPTION (ERROR_MISSING_PARAMETERS, 0);
		}

		if (Motor_Type != DM_VARIAB_SPEED_3PH)
		{
			Disable_Drain_Motor_Fault_Monitor = TRUE;
			LOG_ADD_EXCEPTION (ERROR_NO_VSM_DRAIN_MOTOR, 0);
		}

		#if (API221_CLIENT_FEATURE == ENABLED)
		Drain_Motor_VSM_Failure = FALSE;
		#endif
	}
}

//----------------------------------------------------------------------------
/**
 *  @brief      VSDrainMotorFaultMonitor__Execution
 *  @details    Function used to perform the Drain Motor Fault Monitor.
 *
 */
void VSDrainMotorFaultMonitor__Execution(void)
{
	if (Disable_Drain_Motor_Fault_Monitor == FALSE)
	{
		CYCLE_NAME_TYPE cycle_name = Selector__GetCycleNameId(Interpreter__GetCurrentSFIndex(COMPARTMENT_DEFAULT),
									 	 	 	 	 	 	  COMPARTMENT_DEFAULT,
															  Interpreter__GetStatus().Sub_Cycle[COMPARTMENT_DEFAULT]);

		CYCLE_STATE_TYPE cycle_state = Selector__GetCycleState(Interpreter__GetCurrentSFIndex(COMPARTMENT_DEFAULT),
															   COMPARTMENT_DEFAULT,
															   Interpreter__GetCurrentStatement(COMPARTMENT_DEFAULT).Cycle);

		if ((Interpreter__IsSubCycleRunning(COMPARTMENT_DEFAULT)) &&
			(cycle_name == CYCLE_NAME_DRAIN_MOTOR_FAULT) &&
			(cycle_state == CYCLE_STATE_RUN))
		{
			Drain_Motor_State = ((uint8 *)Hbl__GetProcessedLoad(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DRAIN_PUMP)))[0];

			#if (API221_CLIENT_FEATURE == ENABLED)
			if (Drain_Motor_State == (uint8)EXTERNAL_MOTOR_ROTATE_X_DEGREES)
			{
				Drain_Motor_State = Motor_On;
			}
			#endif

			if (Drain_Motor_State != Drain_Motor_Previous_State)
			{
				Drain_Motor_Previous_State = Drain_Motor_State;

				if (Drain_Motor_State == Motor_Off)
				{
					#if (API221_CLIENT_FEATURE == ENABLED)
					if ((SystemTimers__NVCheckTimer(&Wait_Clear_Fault_Timer_Handle) == SYSTEMTIMERS_STATE_RUNNING) &&
						(DM_Condition_Detected == CONDITION_UNDETECTED))
					{
						SetConditionDetected();
						DM_Condition_Detected = CONDITION_DETECTED;
					}
					#endif
					if (DM_Condition_Detected == CONDITION_UNDETECTED) // Fault Solved
					{
						#if (API221_CLIENT_FEATURE == ENABLED)
						ResetFaults();
						#endif
						(void)Interpreter__TerminateSubCycle(COMPARTMENT_DEFAULT);
						DM_Condition_Detected = CONDITION_UNUSED;
					}
					else if  (DM_Condition_Detected == CONDITION_DETECTED) // ready for next attempt
					{
						DM_Condition_Detected = CONDITION_UNDETECTED;
					}

#if (API221_CLIENT_FEATURE == ENABLED)
					if (Drain_Motor_VSM_Failure != FALSE)
					{
						Drain_Motor_VSM_Failure = FALSE;
					}
#endif
				}
				#if (API221_CLIENT_FEATURE == ENABLED)
				else if (Drain_Motor_State == Motor_On)
				{
					if (Motor_Type == DM_VARIAB_SPEED_3PH)
					{
						SystemTimers__NVResetTimer(&Wait_Clear_Fault_Stop_Motor_Timer_Handle);
					}
				}
				#endif
			}
			else
			{
				if ((Drain_Motor_State == Motor_On) &&
					(DM_Condition_Detected != CONDITION_DETECTED))
				{
					if (GetDrainMotorError())
					{
#if (API221_CLIENT_FEATURE == ENABLED)
						SetConditionDetected();
#endif
						DM_Condition_Detected = CONDITION_DETECTED;
					}
					else
					{
						DM_Condition_Detected = CONDITION_UNDETECTED;
					}
				}
				#if (API221_CLIENT_FEATURE == ENABLED)
				else if (Drain_Motor_State == Motor_Off)
				{
					if (Motor_Type == DM_VARIAB_SPEED_3PH)
					{
						if ((SystemTimers__NVCheckTimer(&Wait_Clear_Fault_Stop_Motor_Timer_Handle) != SYSTEMTIMERS_STATE_RUNNING) &&
							(SystemTimers__NVCheckTimer(&Wait_Clear_Fault_Timer_Handle) != SYSTEMTIMERS_STATE_RUNNING))
						{
							//send Clear Fault to MCU every 1 sec
							(void)API221MotionCtrl__ClearFailureFlags(MOTOR_1);
							(void)SystemTimers__NVSetTimerMS(&Wait_Clear_Fault_Stop_Motor_Timer_Handle, 1000,SYSTEMTIMERS_TYPE_DOWN_COUNTER);
						}
					}
				}
				#endif
			}

			// If fault is detected, terminate subcycle
			if (DrainIsInFaultState() != FALSE)
			{
				(void)Interpreter__TerminateSubCycle(COMPARTMENT_DEFAULT);
				DM_Condition_Detected = CONDITION_UNUSED;
			}
		}
		else
		{

			if (cycle_state == CYCLE_STATE_RUN)
			{
				#if (API221_CLIENT_FEATURE == ENABLED)
				Drain_Motor_State = ((uint8 *)Hbl__GetProcessedLoad(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DRAIN_PUMP)))[0];
				#endif
				if (GetDrainMotorError()
					#if (API221_CLIENT_FEATURE == ENABLED)
					|| ((SystemTimers__NVCheckTimer(&Wait_Clear_Fault_Timer_Handle) == SYSTEMTIMERS_STATE_RUNNING) &&
							(Drain_Motor_State == Motor_On))
					#endif
					)
				{
#if (API221_CLIENT_FEATURE == ENABLED)
					SetConditionDetected();
#endif

					DM_Condition_Detected = CONDITION_UNUSED;
					Drain_Motor_State = OFF;
					Drain_Motor_Previous_State = OFF;
				}
			}
			else if ((cycle_state != CYCLE_STATE_FAULT) &&
					(cycle_name != CYCLE_NAME_DRAIN_MOTOR_FAULT)) // if the cycle state is PAUSE of the Subcycle CYCLE_NAME_DRAIN_MOTOR_FAULT then not reset faults
			{
				(void)GetDrainMotorError(); // only for clean the error but in state != RUN&&FAULT shouldn't happen

#if (API221_CLIENT_FEATURE == ENABLED)
				ResetFaults();
#endif
			}
		}
		#if (API221_CLIENT_FEATURE == ENABLED)
		if (SystemTimers__NVCheckTimer(&Wait_Clear_Fault_Timer_Handle) == SYSTEMTIMERS_STATE_DONE)
		{
			(void)API221MotionCtrl__ClearFailureFlags(MOTOR_1);
			(void)SystemTimers__NVSetTimerMS(&Wait_Clear_Fault_Timer_Handle, 500,SYSTEMTIMERS_TYPE_DOWN_COUNTER);
		}
		#endif
	}
}

void VSDrainMotorFaultMonitor__SetDrainMotorFailures(uint32 failures)
{
	#if (API221_CLIENT_FEATURE == ENABLED)
	Drain_Motor_Failures = failures;
	(void)SystemTimers__NVSetTimerMS(&Wait_Clear_Fault_Timer_Handle, 500,SYSTEMTIMERS_TYPE_DOWN_COUNTER);
	#endif
}

void VSDrainMotorFaultMonitor__SetDrainMotorFailureFlag(BOOL_TYPE value)
{
	#if (API221_CLIENT_FEATURE == ENABLED)
	Drain_Motor_VSM_Failure = value;
	#endif
}

void VSDrainMotorFaultMonitor__ResetTimerClearMotorFailureFlag(void)
{
	#if (API221_CLIENT_FEATURE == ENABLED)
	SystemTimers__NVResetTimer(&Wait_Clear_Fault_Timer_Handle);
	#endif
}
//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

BOOL_TYPE GetDrainMotorError(void)
{
	BOOL_TYPE ret_val;

	#if (API221_CLIENT_FEATURE == ENABLED)
	ret_val = Drain_Motor_VSM_Failure;
	if (Drain_Motor_VSM_Failure != FALSE)
	{
		Drain_Motor_VSM_Failure = FALSE;
	}
	#else
	ret_val= FALSE;
	#endif

	return ret_val;
}

BOOL_TYPE DrainIsInFaultState(void)
{
	BOOL_TYPE ret = FALSE;

	#if (API221_CLIENT_FEATURE == ENABLED)
	for (uint8 index_fault = (uint8)FAULT_ID_VS_DRAIN_MOTOR_MCI_LOCKED_ROTOR; index_fault <= (uint8)FAULT_ID_MCU_DM_MCI_INVERTER_OVER_TEMPERATURE; index_fault++)
	{
		if (Fault__IsInFaultState((FAULT_ID_TYPE)index_fault))
		{
			ret = TRUE;
		}
	}
	#endif

	return ret;
}
#if (API221_CLIENT_FEATURE == ENABLED)
static void SetConditionDetected(void)
{
	if (Drain_Motor_Failures == ((uint32)1U << (uint32)API221_FF_ANY_ERROR_FOUND))
	{
		Fault__ConditionDetected(FAULT_ID_MCU_DM_MCI_GENERIC_ERROR);
	}

	if (BIT_TEST((uint32)Drain_Motor_Failures,API221_FF_DCBUS_OVER_CURRENT) != 0U)
	{
		Fault__ConditionDetected(FAULT_ID_MCU_DM_MCI_HARDWARE_OVERCURRENT);
	}

	if (BIT_TEST((uint32)Drain_Motor_Failures,API221_FF_DCBUS_OVER_VOLTAGE) != 0U)
	{
		Fault__ConditionDetected(FAULT_ID_MCU_DM_MCI_OVER_VOLTAGE);
	}

	if (BIT_TEST((uint32)Drain_Motor_Failures,API221_FF_DCBUS_UNDER_VOLTAGE) != 0U)
	{
		Fault__ConditionDetected(FAULT_ID_MCU_DM_MCI_UNDER_VOLTAGE);
	}

	if (BIT_TEST((uint32)Drain_Motor_Failures,API221_FF_ELECTRONIC_FAILED) != 0U)
	{
		Fault__ConditionDetected(FAULT_ID_MCU_DM_MCI_DRIVER_FAILURE);
	}

	if (BIT_TEST((uint32)Drain_Motor_Failures,API221_FF_INVERTER_OVER_TEMP) != 0U)
	{
		Fault__ConditionDetected(FAULT_ID_MCU_DM_MCI_INVERTER_OVER_TEMPERATURE);
	}

	if (BIT_TEST((uint32)Drain_Motor_Failures,API221_FF_LOCKED_ROTOR) != 0U)
	{
		Fault__ConditionDetected(FAULT_ID_VS_DRAIN_MOTOR_MCI_LOCKED_ROTOR);
	}

	if (BIT_TEST((uint32)Drain_Motor_Failures,API221_FF_PHASE_LOST) != 0U)
	{
		Fault__ConditionDetected(FAULT_ID_VS_DRAIN_MOTOR_MCI_MOTOR_PHASE_LOST);
	}

	if (BIT_TEST((uint32)Drain_Motor_Failures,API221_FF_MOTOR_OVERLOADED) != 0U)
	{
		Fault__ConditionDetected(FAULT_ID_VS_DRAIN_MOTOR_MCI_MOTOR_OVERLOAD);
	}

	if (BIT_TEST((uint32)Drain_Motor_Failures,API221_FF_MOTOR_OVER_CURRENT) != 0U)
	{
		Fault__ConditionDetected(FAULT_ID_VS_DRAIN_MOTOR_MCI_SOFTWARE_OVERCURRENT);
	}

	if (BIT_TEST((uint32)Drain_Motor_Failures,API221_FF_SPEED_CHECK_FAILED) != 0U)
	{
		Fault__ConditionDetected(FAULT_ID_VS_DRAIN_MOTOR_MCI_SPEED_CHECK);
	}
}
static void ResetFaults(void)
{
	for (uint8 index_fault = (uint8)FAULT_ID_VS_DRAIN_MOTOR_MCI_LOCKED_ROTOR; index_fault <= (uint8)FAULT_ID_MCU_DM_MCI_INVERTER_OVER_TEMPERATURE; index_fault++)
	{
		if (Fault__IsObserverActive((FAULT_ID_TYPE)index_fault))
		{
			Fault__ConditionRemoved((FAULT_ID_TYPE)index_fault);
		}
	}
}
#endif

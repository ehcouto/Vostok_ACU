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
#include "PumpFlushRequestMonitor.h"

#include "Interpreter.h"
#include "Micro.h"
#include "PumpCommand.h"
#include "SystemTimers.h"
#if (API221_CLIENT_FEATURE == ENABLED)
#include "API221MotionCtrl.h"
#include "McuGetDataMonitor.h"
#endif

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

static BOOL_TYPE WashPumpCtrlExecuted;
static BOOL_TYPE DrainPumpCtrlExecuted;
static HBL_PILOT_ENUM_TYPE Wash_Pump_Pilot_Type;
static HBL_PILOT_ENUM_TYPE Drain_Pump_Pilot_Type;
static BOOL_TYPE Wash_Stop_Sent;
static BOOL_TYPE Drain_Stop_Sent;
#if (API221_CLIENT_FEATURE == ENABLED)
static SYSTEMTIMER_NV_TIMER_TYPE Stop_Motor_Timer_Handle;
#endif
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//----------------------------------------------------------------------------
/**
 *  @brief      PumpFlushRequestMonitor__Initialize
 *  @details    Function used to initialize the PumpFlushRequestMonitor.
 *
 */
void PumpFlushRequestMonitor__Initialize(void)
{
	Wash_Pump_Pilot_Type = Hbl__GetPilotIDByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_WASH_PUMP));

	Drain_Pump_Pilot_Type = Hbl__GetPilotIDByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DRAIN_PUMP));

	if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
	{
		if (Wash_Pump_Pilot_Type == HBL_PILOT_EXTERNAL_MOTOR)
		{
			PumpFlushRequestMonitor__ResetWashPumpCtrlExecutedFlag();
			PumpFlushRequestMonitor__ResetWashStopSentFlag();
		}

		if (Drain_Pump_Pilot_Type == HBL_PILOT_EXTERNAL_MOTOR)
		{
			PumpFlushRequestMonitor__ResetDrainPumpCtrlExecutedFlag();
			PumpFlushRequestMonitor__ResetDrainStopSentFlag();
		}
	}
}

//----------------------------------------------------------------------------
/**
 *  @brief     PumpFlushRequestMonitor__Execution
 *  @details    Function used to perform the PumpFlushRequestMonitor.
 *
 */
void PumpFlushRequestMonitor__Execution(void)
{
	if (Wash_Pump_Pilot_Type == HBL_PILOT_EXTERNAL_MOTOR)
	{
		if (PumpFlushRequestMonitor__GetWashPumpCtrlExecutedFlag() == FALSE)
		{
			PumpCommand__StopWithDefDecel(HBL_PRIORITY_APPLICATION_LOW, HBL_LOAD_WASH_PUMP);
			Wash_Stop_Sent = TRUE;

			#if (API221_CLIENT_FEATURE == ENABLED)
			if (McuGetDataMonitor__GetWashPumpSpeed() > OFF)
			{
				if (SystemTimers__NVCheckTimer(&Stop_Motor_Timer_Handle) != SYSTEMTIMERS_STATE_RUNNING)
				{
					API221MotionCtrl__DeviceStop(MOTOR_0,DEFAULT_DECELERATION+100);
					SystemTimers__NVSetTimerMS(&Stop_Motor_Timer_Handle, 1000,SYSTEMTIMERS_TYPE_DOWN_COUNTER);
				}
			}
			#endif
		}

		PumpFlushRequestMonitor__ResetWashPumpCtrlExecutedFlag();
	}

	if (Drain_Pump_Pilot_Type == HBL_PILOT_EXTERNAL_MOTOR)
	{
		if (PumpFlushRequestMonitor__GetDrainPumpCtrlExecutedFlag() == FALSE)
		{
			PumpCommand__StopWithDefDecel(HBL_PRIORITY_APPLICATION_LOW, HBL_LOAD_DRAIN_PUMP);
			Drain_Stop_Sent = TRUE;

			#if (API221_CLIENT_FEATURE == ENABLED)
			if (McuGetDataMonitor__GetDrainPumpSpeed() > OFF)
			{
				if (SystemTimers__NVCheckTimer(&Stop_Motor_Timer_Handle) != SYSTEMTIMERS_STATE_RUNNING)
				{
					API221MotionCtrl__DeviceStop(MOTOR_1,DEFAULT_DECELERATION+100);
					SystemTimers__NVSetTimerMS(&Stop_Motor_Timer_Handle, 1000,SYSTEMTIMERS_TYPE_DOWN_COUNTER);
				}
			}
			#endif
		}

		PumpFlushRequestMonitor__ResetDrainPumpCtrlExecutedFlag();
	}
}

BOOL_TYPE PumpFlushRequestMonitor__GetWashPumpCtrlExecutedFlag(void)
{
	return WashPumpCtrlExecuted;
}

BOOL_TYPE PumpFlushRequestMonitor__GetDrainPumpCtrlExecutedFlag(void)
{
	return DrainPumpCtrlExecuted;
}

void  PumpFlushRequestMonitor__ResetWashPumpCtrlExecutedFlag(void)
{
	WashPumpCtrlExecuted = FALSE;
}

void  PumpFlushRequestMonitor__ResetDrainPumpCtrlExecutedFlag(void)
{
	DrainPumpCtrlExecuted = FALSE;
}

void  PumpFlushRequestMonitor__SetWashPumpCtrlExecutedFlag(void)
{
	WashPumpCtrlExecuted = TRUE;
}

void  PumpFlushRequestMonitor__SetDrainPumpCtrlExecutedFlag(void)
{
	DrainPumpCtrlExecuted = TRUE;
}

BOOL_TYPE PumpFlushRequestMonitor__GetWashStopSentFlag(void)
{
	return Wash_Stop_Sent;
}

BOOL_TYPE PumpFlushRequestMonitor__GetDrainStopSentFlag(void)
{
	return Drain_Stop_Sent;
}

void  PumpFlushRequestMonitor__SetWashStopSentFlag(void)
{
	Wash_Stop_Sent = TRUE;
}

void  PumpFlushRequestMonitor__SetDrainStopSentFlag(void)
{
	Drain_Stop_Sent = TRUE;
}


void  PumpFlushRequestMonitor__ResetWashStopSentFlag(void)
{
	Wash_Stop_Sent = FALSE;
}

void  PumpFlushRequestMonitor__ResetDrainStopSentFlag(void)
{
	Drain_Stop_Sent = FALSE;
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

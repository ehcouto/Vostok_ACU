/*
 * DiverterFaultMonitor.c
 *  @brief      Basic description of file contents
 *  @brief      @details    Detailed description of the file contents
 *  Created on: 28.08.2018
 *      Author: WAZNYP
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2011.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "DiverterFaultMonitor.h"
#include "Fault.h"
#include "Hbl.h"
#include "SettingFile.h"
#include "AlternateLoad.h"
#include "AlternateCtrl.h"
#include "Interpreter.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define DIVERTER_SF_TIME_FACTOR (uint16)5

static uint8 Diverter_Fb_Counter;
static BOOL_TYPE Diverter_Should_Stop;
static BOOL_TYPE Prev_Diverter_Should_Stop;
static uint16 Diverter_Fault_Timer;
static uint8 Diverter_Fb_Status;
static uint8 Prev_Diverter_Fb_Status;
static BOOL_TYPE Diverter_Fault_Checked;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void SetFailure(void);
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================



//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================


void DiverterFaultMonitor__Initialize(void)
{
    Fault__ConditionRemoved(FAULT_ID_DIVERTER_TRIAC_SHORTED);
    Prev_Diverter_Should_Stop = TRUE;
    Diverter_Should_Stop = TRUE;
    Diverter_Fb_Counter = 0;
    Diverter_Fault_Timer = 0;
    Diverter_Fb_Status = 0;
    Prev_Diverter_Fb_Status = 0;
    Diverter_Fault_Checked = FALSE;
}

void DiverterFaultMonitor__Execution(void)
{
    ON_OFF_TYPE pilot_state;
    uint8 *pos_res;
    HBL_LOAD_STATUS_TYPE pos_val;
    HBL_CONSISTENCY_TYPE            fbk_val;

    CYCLE_NAME_TYPE cycle_name;
	INTERPRETER_STATUS_TYPE status;

	if (Hbl__GetLoadIndexByLoadID(HBL_LOAD_PILOT_RELAY) != (uint8)HBL_INDEX_INVALID)
	{
		pilot_state = (((uint8*)(Hbl__GetLoadStatusByLoadID(HBL_LOAD_PILOT_RELAY)))[0] == 1U) ? ON:OFF;
	}
	else if (Hbl__GetLoadIndexByLoadID(HBL_LOAD_RELAY_ENABLE) != (uint8)HBL_INDEX_INVALID)
	{
		pilot_state = (((uint8*)(Hbl__GetLoadStatusByLoadID(HBL_LOAD_RELAY_ENABLE)))[0] == 1U) ? ON:OFF;
	}
	else
	{
		pilot_state = ON;
	}

    pos_res = (uint8 *) Hbl__GetLoadStatusByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DIVERTER));
    pos_val.Byte = *pos_res;


	status = Interpreter__GetStatus();
	cycle_name = Selector__GetCycleNameId(0,0,status.Main_Cycle[0]);
	if ((CYCLE_NAME_EOL_0 == cycle_name) ||
		(CYCLE_NAME_EOL_1 == cycle_name) ||
		(CYCLE_NAME_EOL_2 == cycle_name))
	{
		if (AlternateLoad__GetDiverterRotateAlwaysFlag() && !Diverter_Fault_Checked)
		{
			SetFailure();
			Diverter_Fault_Checked = TRUE;
		}
		else
		{
			fbk_val.Byte = Hbl__GetLoadConsistency(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DIVERTER));
			Diverter_Fb_Status = (uint8)fbk_val.Consistency.Value;

			if (Prev_Diverter_Fb_Status != Diverter_Fb_Status)
			{
				Fault__ConditionRemoved(FAULT_ID_DIVERTER_TRIAC_SHORTED);
			}
		}
		Prev_Diverter_Fb_Status = Diverter_Fb_Status;
	}
	else
	{
		if (ON == pilot_state)
		{
			if ((pos_val.Load.Cur_Reg == AlternateCtrl__GetPosition()) &&
				(pos_val.Load.Status == (HBL_LOAD_STATUS_ENUM_TYPE)HBL_LOAD_STATUS_TARGET_COMPLETED) &&
				(AlternateLoad__GetDiverterRotateAlwaysFlag() == FALSE))
			{
				Diverter_Should_Stop = TRUE;
				if ((FALSE == Prev_Diverter_Should_Stop))
				{
					Diverter_Fault_Timer = ((uint16)AlternateLoad__GetRotationTime() * DIVERTER_SF_TIME_FACTOR);
				}
			}
			else
			{
				Diverter_Should_Stop = FALSE;
			}
		}
		else
		{
			// to ensure fault check even if Diverter had not been activated
			Diverter_Should_Stop = FALSE;
		}

		if (((uint8)TRUE == (uint8)Diverter_Should_Stop) && (Diverter_Fault_Timer > 0U))
		{
			fbk_val.Byte = Hbl__GetLoadConsistency(Hbl__GetLoadIndexByLoadID(HBL_LOAD_DIVERTER));
			Diverter_Fb_Status = (uint8)fbk_val.Consistency.Value;

			if ((Prev_Diverter_Fb_Status != Diverter_Fb_Status) && (Diverter_Fb_Counter < 255U))
			{
				Diverter_Fb_Counter ++;
			}
			// fb number should be equal Cam Number * 2, 1 less is needed as safety margin
			if ((Diverter_Fb_Counter + (uint8)1)  > (AlternateLoad__GetCamNumber() * (uint8)2))
			{
				SetFailure();
				Diverter_Fb_Counter = 0;
			}

			Diverter_Fault_Timer--;
			Prev_Diverter_Fb_Status = Diverter_Fb_Status;
		}
		else
		{
			Diverter_Fb_Counter = 0;
			Diverter_Fault_Timer = 0;
		}

		Prev_Diverter_Should_Stop = Diverter_Should_Stop;
		Diverter_Fault_Checked = FALSE;
	}
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetFailure(void)
{
    Fault__ConditionDetected(FAULT_ID_DIVERTER_TRIAC_SHORTED);
}



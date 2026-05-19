/**
 *  @file       
 *
 *  @brief      Model Based Control Design code integration wrapper template for a Nucleus Process.
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *
 *
 *  @copyright  Copyright 2017-2018. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------


#include "OWICalibrationProcess.h"

#include "DishAuxVar.h"
#include "Micro.h"
#include "StatisticsDish.h"


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

// -- Private System Variable Definitions --



// -- Private Variable Definitions --

static PROCESS_RESULT_TYPE Result;

//! Pointer to the "Variable" data in the setting file.
SF_DATA_TYPE* Sf_OWICalibration_Config;

PROCESS_CONTROL_TYPE* OWICalibration_Control;

static float32 TurbidityVoltage;

static SYSTEMTIMER_NV_TIMER_TYPE NV_Timer_Handle_OWI_Calibration_Timeout;

static SYSTEMTIMER_NV_TIMER_TYPE NV_Timer_Handle_OWI_Calibration_DelayTime;

static uint32 OWI_Timeout_Timer;

static BOOL_TYPE OWICalibration_isRunningFlag = FALSE;

static uint32 OWI_Delaytimer;

static SYSTEMTIMERS_STATE_TYPE TimeoutState;

static SYSTEMTIMERS_STATE_TYPE DelayTimeState;

static BOOL_TYPE Process_Completed;

static BOOL_TYPE OWICalibration_Process_Initialized;

#pragma location = "_appliance_segment"
static MODEL_DATA_TYPE  ModelRecoveryData;


//-------------------------------------- Settings Files Loader --------------------------------------------------------

static void ReadParameters(PROCESS_CONTROL_TYPE* control);

//-------------------------------------- PUBLIC (Get/Set Functions) ----------------------------------------

void OWICalibration__SetProcessResult(PROCESS_RESULT_TYPE modelresult)
{
	Result = modelresult;
}


void OWICalibration__SetState(OWI_CALIB_STATE_TYPE owicalibration_state)
{
	OWICalibration_State = owicalibration_state;
}

void OWICalibration__SetTurbidityVoltageReading(float32 OWI_Calibration_Turbidity_Voltage_Reading)
{
	OWICalibration_Turbidity_Voltage_Reading = OWI_Calibration_Turbidity_Voltage_Reading;
}


void OWICalibration__SetStatus(PASS_FAIL_TYPE OWI_Calib_Pass_Fail)
{
	OWICalibration_Status = OWI_Calib_Pass_Fail;
}

void OWICalibration__SetDutyCycle(uint16 owi_calib_drive_dutycycle)
{
	HBL_PILOT_ENUM_TYPE owi_pilot_type = Hbl__GetPilotIDByIndex(Hbl__GetLoadIndexByLoadID(HBL_LOAD_OWI));
	if (owi_pilot_type == HBL_PILOT_PWM_HI_RES)
	{
		// Nucleus HI Resolution PRM - from 0 to 10000
		OWI_Calibration_Drive_Duty_Cycle = (uint16)((owi_calib_drive_dutycycle * 10000U)/(uint16)32768);
	}
	else
	{
		// Legacy PWM OWI Pilot - from 0 to 32768
		OWI_Calibration_Drive_Duty_Cycle = owi_calib_drive_dutycycle;
	}

	(void)Hbl__SetLoadRequestByLoadID(HBL_LOAD_OWI, HBL_PRIORITY_APPLICATION_HI, (uint8 *) &OWI_Calibration_Drive_Duty_Cycle);
}

void OWICalibration__SetTimeoutMs(uint32 OWI_Timeout)
{
	OWI_Timeout_Timer = OWI_Timeout;
	(void)SystemTimers__NVSetTimerMS(&NV_Timer_Handle_OWI_Calibration_Timeout, OWI_Timeout_Timer ,SYSTEMTIMERS_TYPE_DOWN_COUNTER);
}

void OWICalibration__SetDelayTimeMs(uint32 OWI_DelayTime)
{
	OWI_Delaytimer = OWI_DelayTime;
	(void)SystemTimers__NVSetTimerMS(&NV_Timer_Handle_OWI_Calibration_DelayTime, OWI_Delaytimer ,SYSTEMTIMERS_TYPE_DOWN_COUNTER);
}

 void OWICalibration__SetModelData(MODEL_DATA_TYPE ModelData)
{
	ModelRecoveryData = ModelData;
}

 void OWICalibration__SetRunningFlag (BOOL_TYPE isRunning)
 {
 	OWICalibration_isRunningFlag = isRunning;
 }


// -- GET Function prototypes --


OWI_CALIB_STATE_TYPE OWICalibration__GetState(void)
{
	return OWICalibration_State;
}

float32 OWICalibration__GetTurbidityVoltage(void)
{
	TurbidityVoltage = (float32)Hbl__GetGIDataConvertedByIndex(Hbl__GetGIIndexByGIID(HBL_GI_TURBIDITY_SENSOR_OWI));
	return TurbidityVoltage;
}

PASS_FAIL_TYPE OWICalibration__GetStatus(void)
{
	return OWICalibration_Status;
}

SYSTEMTIMERS_STATE_TYPE OWICalibration__GetTimeoutState(void)
{
	TimeoutState = SystemTimers__NVCheckTimer(&NV_Timer_Handle_OWI_Calibration_Timeout);
	return TimeoutState;
}

SYSTEMTIMERS_STATE_TYPE OWICalibration__GetDelayTimeState(void)
{
	DelayTimeState = SystemTimers__NVCheckTimer(&NV_Timer_Handle_OWI_Calibration_DelayTime);
	return DelayTimeState;
}

PROCESS_RESULT_TYPE OWICalibration__GetProcessResult(void)
{

	return Result;
}

MODEL_DATA_TYPE OWICalibration__GetModelData(void)
{

	return ModelRecoveryData;
}

BOOL_TYPE OWICalibration__GetRunningFlag (void)
{
	return OWICalibration_isRunningFlag;
}

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
/**
 * The (Myprocess) process.
 *
 * @brief
 *
 * @param control = The control parameters provided by the Interpreter.
 * @retval PROCESS_COMPLETE = The process is ready for the Interpreter to proceed to the next step.
 * @retval PROCESS_BUSY = The process is not ready for the Interpreter to proceed to the next step.
 */


PROCESS_RESULT_TYPE OWICalibration__Process(PROCESS_CONTROL_TYPE* control)
{

	OWICalibration_Control = control;

	if (((uint8)control->Step_Initializing == (uint8)TRUE) ||
		((uint8)control->Resume_From_Reset == (uint8)TRUE) ||
		(((uint8)control->Resume_From_Pause == (uint8)TRUE) && (OWICalibration_Process_Initialized == FALSE)))// && control->F1_F0_Flags == 0 && control->Resume_From_Pause == FALSE && control->Resume_From_Reset == FALSE)
	{
		// Initialize Process Variables
		OWICalibration_Status = FAIL;
		Result = PROCESS_BUSY;

		Process_Completed = FALSE;

		// Initialize Model States
		memset(&OWICalibrationModel_DW_GLOBAL, 0, sizeof(DW_OWICalibrationModel_TYPE));

		// Initialize Model Data
		ModelRecoveryData.OWICalibration_Drive_Initial_Duty_Cycle = 16384;
		ModelRecoveryData.OWICalibration_Drive_Duty_Cycle = 16384;


		// Read data from Setting File
		ReadParameters(control);

    	SystemTimers__NVResetTimer(&NV_Timer_Handle_OWI_Calibration_Timeout);
    	SystemTimers__NVResetTimer(&NV_Timer_Handle_OWI_Calibration_DelayTime);

    	if (Sf_OWICalibration_Config != NULL)
		{
			OWICalibration__SetTimeoutMs(Sf_OWICalibration_Config->OWICalibration_Max_Time * (uint32)60000);
		}

		OWICalibration_State = OWICALIBRATION_STATE_STABILIZE;
	}

	if (Sf_OWICalibration_Config != NULL)
	{
		OWICalibration__Model();
	}
	else
	{
		Result = PROCESS_COMPLETE;
	}

	if(Result == PROCESS_COMPLETE)
	{
		if(Process_Completed == FALSE)
		{
			Process_Completed = TRUE;
			StatisticsDish__UpdateOWICalibrationDutyCycle();
		}
	}

	return Result;
}
	


void OWICalibration__Initialize(void)
{
   	OWICalibration_Process_Initialized = FALSE;
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

//BOOL_TYPE
static void ReadParameters(PROCESS_CONTROL_TYPE* control)
{
	SETTINGFILE_LOADER_TYPE loader;

	// Initialize SF data pointer
	Sf_OWICalibration_Config = (SF_DATA_TYPE*)NULL;

	if((SettingFile__MSettingBasicLoader(control->Setting_File_Index, SF_PTR_ACU_ALGORITHM_PARAMETERS, DISPL_OWI_CALIBRATION, &loader) == PASS))
	{
		Sf_OWICalibration_Config = (SF_DATA_TYPE*)(void*)(loader.Data);

		OWICalibration_Process_Initialized = TRUE;
	}
}

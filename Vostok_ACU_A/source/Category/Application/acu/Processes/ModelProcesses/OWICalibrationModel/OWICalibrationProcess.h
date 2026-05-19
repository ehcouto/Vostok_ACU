//  Manual Code //
#ifndef OWICALIBRATIONPROCESS_H_
#define OWICALIBRATIONPROCESS_H_


/* Macros for accessing real-time model data structure */

/* user code (top of header file) */


#include "C_Extensions.h"
#include "SystemConfig.h"
#include "Process.h"
#include "SystemTimers.h"
#include "Log.h"
#include "string.h"
#include "SettingFile.h"
#include "Hbl.h"
#include "Variable.h"
#include "OWICalibrationProcess_def.h"
#include "OWICalibrationModel.h"
#include "AnalogOutputPilot.h"




extern SF_DATA_TYPE *Sf_OWICalibration_Config; 
extern PROCESS_CONTROL_TYPE* OWICalibration_Control;

//extern MODEL_DATA_TYPE *OWICalibration_ModelDataConfig;


//=====================================================================================================================
//--------------------------------------------------- MACROS ----------------------------------------------------------
//=====================================================================================================================

/**
 * The model CONTROL_TYPE & SETTING_FILE_DATA MACRO.
 * Wrapper #defines Modle MACRO's 
 * 
 */
 // Use this section to define all macros that will connect to the model
 
#define OWICALIBRATION__GETSFDATA()		(*Sf_OWICalibration_Config)


#define MODEL_DATA_MAX_SIZE 2

//! Definition of OWICalibration process status events
typedef enum OWI_PROCESS_EVENT_ENUM
{
    OWI_PROCESS_STARTED    = 0,
    OWI_PROCESS_BUSY	    = 1,
    OWI_PROCESS_COMPLETED  = 2,
    OWI_PROCESS_MAX	    = 3,
} OWI_PROCESS_EVENT_TYPE;

typedef void (*OWI_EVENT_HANDLER_TYPE)(OWI_PROCESS_EVENT_TYPE event);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//Fixed get/set functions
void OWICalibration__SetProcessResult(PROCESS_RESULT_TYPE modelresult);

// Main Interface
PROCESS_RESULT_TYPE OWICalibration__Process(PROCESS_CONTROL_TYPE* control);

// OWI Calibration interfaces

PASS_FAIL_TYPE OWICalibration__GetStatus(void);

OWI_CALIB_STATE_TYPE OWICalibration__GetState(void);

sint16 OWICalibration__GetSoilNTU(void);

float32 OWICalibration__GetDeltaV(void);

float32 OWICalibration__GetTurbidityVoltage(void);

SYSTEMTIMERS_STATE_TYPE OWICalibration__GetTimeoutState(void);

SYSTEMTIMERS_STATE_TYPE OWICalibration__GetDelayTimeState(void);

PROCESS_RESULT_TYPE OWICalibration__GetProcessResult(void);

MODEL_DATA_TYPE OWICalibration__GetModelData(void);

BOOL_TYPE OWICalibration__GetRunningFlag (void);

//float32 OWICalibration__GetTurbidityVoltageReading(void);



void OWICalibration__SetRunningFlag (BOOL_TYPE isRunning);

void OWICalibration__SetState(OWI_CALIB_STATE_TYPE owicalibration_state);

void OWICalibration__SetTurbidityVoltageReading(float32 OWI_Calibration_Turbidity_Voltage_Reading);

void OWICalibration__SetStatus(PASS_FAIL_TYPE OWI_Calib_Pass_Fail);

void OWICalibration__SetDutyCycle(uint16 owi_calib_drive_dutycycle);

void OWICalibration__SetSoilNTU(sint16 OWI_Calib_Soil_NTU);

void OWICalibration__SetDeltaV(float32 OWI_Calib_DeltaV);

void OWICalibration__SetTimeoutMs(uint32 OWI_Timeout);

void OWICalibration__SetDelayTimeMs(uint32 OWI_DelayTime);

void OWICalibration__SetModelData(MODEL_DATA_TYPE ModelData);

void OWICalibration__Initialize(void);


#endif /* OWICALIBRATIONPROCESS_H_ */

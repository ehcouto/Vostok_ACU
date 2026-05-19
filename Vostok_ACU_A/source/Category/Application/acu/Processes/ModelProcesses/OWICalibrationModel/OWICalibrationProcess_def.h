/**
 * @file
 * @brief       Public definition of the data structure used by the OWI Calibration process and supporting modules.
 *
 * @copyright   Copyright 2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef OWICALIBRATIONPROCESS_DEF_H_
#define OWICALIBRATIONPROCESS_DEF_H_

#include "C_Extensions.h"
#include "Compiler_defs.h"
#include "Variable_prv.h"
#include "DishAuxVar.h"
#include "Variable.h"

typedef struct OWICALIBRATION_DATA_STRUCT
{
    uint16 OWICalibration_Drive_Initial_Duty_Cycle;
    uint16 OWICalibration_Drive_Duty_Cycle;
} MODEL_DATA_TYPE;

typedef PACKED struct OWICALIBRATION_SF_DATA_STRUCT
{
	/* A delay after duty cycle simulation to stabilize the OWI turbidity voltage feedback */
	 	 uint8 OWICalibration_Calib_TV_Read_Delay;

	/* Acceptable tolerance of Turbidity voltage feedback offset from  OWI_Calib_Turb_TargetV */
	 	uint8 OWICalibration_Turb_TargetV_Tol;

	/* Target calibration voltage to tune the sensor */
	 	 float32 OWICalibration_Turb_TargetV;

	/* Maximum time OWI calibration sequence can operate */
	 	 uint8 OWICalibration_Max_Time;

	/* Number of consecutive cycles that will try to run OWI Calibration again if it keeps failing.
	 * To be used by OWI Monitor module. */
	 	 uint8 Turbidity_Sensor_Calib_Retry_Count;

	/* Lower bandwidth of dutycycle for Zone 1 */
	 	 uint8 OWICalibration_Zone1_LL;

    /* Upper bandwidth of dutycycle for Zone 1 */
	 	 uint8 OWICalibration_Zone1_UL;

    /* Lower bandwidth of dutycycle for Zone 2 */
	 	 uint8 OWICalibration_Zone2_LL;

    /* Upper bandwidth of dutycycle for Zone 2 */
	 	 uint8 OWICalibration_Zone2_UL;

    /* Lower bandwidth of dutycycle for Zone 3 */
	 	 uint8 OWICalibration_Zone3_LL;

    /* Upper bandwidth of dutycycle for Zone 3 */
	 	 uint8 OWICalibration_Zone3_UL;

    /* Delta Voltage definition between soil levels under Zone 1 */
	 	 float32 OWICalibration_Zone1_DeltaV;

    /* Delta Voltage definition between soil levels under Zone 2 */
	 	 float32 OWICalibration_Zone2_DeltaV;

    /* Delta Voltage definition between soil levels under Zone 3 */
	 	 float32 OWICalibration_Zone3_DeltaV;

    /* NTU increment for each delta Voltage definition */
	 	 uint16 OWICalibration_Calib_Delta_NTU;

	/* Number of cycles between calibrations. To be used by OWI Monitor module. */
	 	 uint16 OWI_Cycles_Between_Calibrations;

	/*  */
		float32 OWICalibration_Input_DC_Voltage;

	/* A delay after duty cycle simulation to stabilize the OWI turbidity voltage feedback Only for OWI One Shot Process*/
	 	 uint8 OWI_One_Shot_Read_Timeout;
	/* To Align to uint16*/
	 	 uint8 OWI_Filler;

} SF_DATA_TYPE;

#endif  /* OWICALIBRATIONPROCESS_DEF_H_ */

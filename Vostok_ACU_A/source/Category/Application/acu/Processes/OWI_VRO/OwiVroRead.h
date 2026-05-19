/**
 * @file		OWI_VRO.h
 * @brief       Public interface to the Regenerate process module.
 *
 */

#ifndef OWI_VRO_READ_H_
#define OWI_VRO_READ_H_

//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --

// -- Other Modules --
#include "DishAuxVar.h"
#include "Process.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
extern OWI_STATUS_TYPE OWI_VRO_OWI_Status;

typedef PACKED struct OWI_VRO_COMMON_SF_PARAMS_STRUCT
{
	uint8 Sample_Max_Time;					//! Units = Tens of seconds
	uint8 Read_Time_ms;						//! Units = Tens of ms
	uint8 Calibration_Read_Retry_Max;
	uint8 Cycles_Btw_Calibration;

	uint8 Coeffs_To_Acquire		:4;
	uint8 Coeffs_To_Reject		:4;

	uint8 Voltage_Tolerance;				// Units = 0.01 V
} OWI_VRO_COMMON_SF_PARAMS_TYPE;

typedef PACKED struct OWI_VRO_SETPOINT_SF_PARAMS_STRUCT
{
	uint8 Voltage_Setpoint;					// Units = 0.1 V
	uint8 Unused_B1;
	uint16 Compensation_Target;
	uint16 Compensation_Exponent;			// setting_file_value = actual_value * 1000
	uint16 Comp_Lower_Limit;
	uint16 Comp_Upper_Limit;
	uint16 NonComp_Lower_Limit;
	uint16 NonComp_Upper_Limit;
} OWI_VRO_SETPOINT_SF_PARAMS_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void OwiVroRead__Initialize(void);
PROCESS_RESULT_TYPE OwiVroRead__Process(PROCESS_CONTROL_TYPE* control);
uint16 OwiVroRead__GetAvgCoeff(uint8 setpoint_index);
void OwiVroRead__CycleEndEvent(void);
void OwiVroRead__CycleStarted(void);

#endif /* OWI_VRO_READ_H_ */

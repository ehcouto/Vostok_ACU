/**
 * @file: OWICalibrationModel.h
 *
 * @model: 'OWICalibrationModel'.
 *
 * @brief
 *
 * @modelversion:  1.531
 *
 * @coderversion:  8.12 (R2017a) 16-Feb-2017
 *
 * C/C++ source code generated on : Wed Oct 17 16:50:28 2018
 *
 * @copyright  Copyright 2017-2018. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef RTW_HEADER_OWICalibrationModel_h_
#define RTW_HEADER_OWICalibrationModel_h_
#include <math.h>
#include <stddef.h>
#ifndef OWICalibrationModel_COMMON_INCLUDES_
# define OWICalibrationModel_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* OWICalibrationModel_COMMON_INCLUDES_ */

/* Includes for objects with custom storage classes. */
#include "OWICalibrationProcess.h"
#include "OWICalibrationProcess_def.h"
#include "SystemTimers.h"
#include "Process.h"
#include "C_Extensions.h"

/* Macros for accessing real-time model data structure */

/* user code (top of header file) */
//#include "C_Extensions.h"
//#include "Process.h"
//#include "OWICalibrationProcess_def.h"
//#include "OWICalibrationProcess.h"
//#include "Variable.h"
//#include "SystemTimers.h"

/* Block states (auto storage) for system '<Root>' */
typedef struct
{
    float32 OWICalibration_Prev_Error; /* '<S1>/Chart' */
    float32 OWICalibration_Best_Turb_Voltag;/* '<S1>/Chart' */
    uint16 OWICalibration_Upper_DutyCycle;/* '<S1>/Chart' */
    uint16 OWICalibration_Lower_DutyCycle;/* '<S1>/Chart' */
    uint16 OWICalibration_Best_Duty_Cycle;/* '<S1>/Chart' */
    uint8 is_active_c3_OWICalibrationMode;/* '<S1>/Chart' */
    uint8 is_c3_OWICalibrationModel;   /* '<S1>/Chart' */
}
DW_OWICalibrationModel_TYPE;

/* Block states (auto storage) */
extern DW_OWICalibrationModel_TYPE OWICalibrationModel_DW_GLOBAL;

/* Model entry point functions */
void OWICalibrationModel_initialize(void);

/* Exported entry point functions */
void OWICalibration__Model(void);

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is <system>/block_name, where system is the system number (uniquely
 * assigned by Simulink) and block_name is the name of the block.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system('<S3>')    - opens system 3
 * hilite_system('<S3>/Kp') - opens and selects block Kp which resides in S3
 *
 * Here is the system hierarchy for this model
 *
 * '<Root>' : 'OWICalibrationModel'
 * '<S1>'   : 'OWICalibrationModel/OWI_Calibration'
 * '<S2>'   : 'OWICalibrationModel/OWI_Calibration/Chart'
 * '<S3>'   : 'OWICalibrationModel/OWI_Calibration/Subsystem'
 * '<S4>'   : 'OWICalibrationModel/OWI_Calibration/Subsystem1'
 */
#endif                                 /* RTW_HEADER_OWICalibrationModel_h_ */

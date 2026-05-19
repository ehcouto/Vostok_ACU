/**
 * @file: Plugging_Model.h
 *
 * @model: 'Plugging_Model'.
 *
 * @brief
 *
 * @modelversion:  1.207
 *
 * @coderversion:  8.14 (R2018a) 06-Feb-2018
 *
 * C/C++ source code generated on : Wed Nov  6 20:04:09 2019
 *
 * @copyright  Copyright 2018. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//-------------------
#ifndef RTW_HEADER_Plugging_Model_h_
#define RTW_HEADER_Plugging_Model_h_
#include "PluggingMonitor_def.h"
#include "SystemTimers.h"
#include "Airlock_Model_types.h"
#include "DishAuxVar.h"
#ifndef Plugging_Model_COMMON_INCLUDES_
# define Plugging_Model_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* Plugging_Model_COMMON_INCLUDES_ */

/* Includes for objects with custom storage classes. */
#include "AirlockMonitor.h"
#include "DishAuxVar.h"
#include "PluggingMonitor.h"
#include "Vwi.h"
#include "PluggingMonitor_prv.h"
#include "rt_defines.h"

//--------------------

/* Macros for accessing real-time model data structure */

/* Forward declaration for rtModel */
typedef struct TAG_RTMPLUGGING_MODEL_TYPE RT_MODELPLUGGING_MODEL_TYPE;

/* Block signals and states (default storage) for system '<Root>' */
typedef struct
{
    uint8 is_active_c3_plugging_model; /* '<S5>/Monitor' */
    uint8 is_diverter_stability;       /* '<S5>/Monitor' */
    uint8 is_plugging_monitor;         /* '<S5>/Monitor' */
    uint8 is_no_plugging;              /* '<S5>/Monitor' */
}
DWPLUGGING_MODEL_TYPE;

/* Self model data, for system '<Root>' */
struct TAG_RTMPLUGGING_MODEL_TYPE
{
    DWPLUGGING_MODEL_TYPE dwork;
};

/* Model entry point functions */
void Plugging_Model_initialize(RT_MODELPLUGGING_MODEL_TYPE *const
    Plugging_Model_M, PLUGGING_MONITORSTATE_TYPE *y_plugging_monitorstate);

/* Exported entry point function */
void PluggingModel__Execute(RT_MODELPLUGGING_MODEL_TYPE *const Plugging_Model_M,
    SF_PLUGGING_DATA_TYPE *u_sfdata, BOOL_TYPE u_plugging_enabled,
    PLUGGING_MONITORSTATE_TYPE *y_plugging_monitorstate);

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
 * '<Root>' : 'Plugging_Model'
 * '<S1>'   : 'Plugging_Model/Plugging_Monitor'
 * '<S2>'   : 'Plugging_Model/Plugging_Monitor/Enabled_Subsystem'
 * '<S3>'   : 'Plugging_Model/Plugging_Monitor/Enabled_Subsystem/ModelDataIn'
 * '<S4>'   : 'Plugging_Model/Plugging_Monitor/Enabled_Subsystem/ModelDataOut'
 * '<S5>'   : 'Plugging_Model/Plugging_Monitor/Enabled_Subsystem/StepFunction'
 * '<S6>'   : 'Plugging_Model/Plugging_Monitor/Enabled_Subsystem/StepFunction/Monitor'
 */
#endif                                 /* RTW_HEADER_Plugging_Model_h_ */

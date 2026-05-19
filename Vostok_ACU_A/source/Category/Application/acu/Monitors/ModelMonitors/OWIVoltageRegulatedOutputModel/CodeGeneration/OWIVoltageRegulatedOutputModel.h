/*
 * File: OWIVoltageRegulatedOutputModel.h
 *
 * Code generated for Simulink model 'OWIVoltageRegulatedOutputModel'.
 *
 * Model version                  : 1.39
 * Simulink Coder version         : 9.3 (R2020a) 18-Nov-2019
 * C/C++ source code generated on : Tue Aug  2 17:21:29 2022
 *
 * Target selection: monitor.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_OWIVoltageRegulatedOutputModel_h_
#define RTW_HEADER_OWIVoltageRegulatedOutputModel_h_
#ifndef OWIVoltageRegulatedOutputModel_COMMON_INCLUDES_
# define OWIVoltageRegulatedOutputModel_COMMON_INCLUDES_
#include "rtwtypes.h"
#include "string.h"
#include "SettingFile.h"
#include "Hbl.h"
#endif                     /* OWIVoltageRegulatedOutputModel_COMMON_INCLUDES_ */

#include "OWIVoltageRegulatedOutputModel_types.h"

/* Child system includes */
#define OWI_VRO_Monitor_MDLREF_HIDE_CHILD_
#include "OWI_VRO_Monitor.h"
#include "rt_defines.h"

/* Macros for accessing real-time model data structure */

/* user code (top of header file) */
#include "Modifier.h"
#include "OwiVroRead.h"
#include "OWIVoltageRegulatedOutput_prv.h"

/* Block signals (default storage) */
typedef struct {
  uint16 search_dutycycle;             /* '<S7>/Model' */
  BOOL_TYPE owi_vro_reset;             /* '<S7>/Data Store Read1' */
} BOWIVOLTAGEREGULATEDOUTPUT_TYPE;

/* Self model data, for system '<Root>' */
struct TAG_RTMOWIVOLTAGEREGULATED_TYPE {
  BOWIVOLTAGEREGULATEDOUTPUT_TYPE blockIO;
};

/* Model entry point functions */
extern void OWIVoltageRegulatedOutputModel__Initialize
  (RT_MODELOWIVOLTAGEREGULATE_TYPE *const m);

/* Exported entry point function */
extern void GetSequenceId(RT_MODELOWIVOLTAGEREGULATE_TYPE *const m, uint8
  *y_owi_vro_dutycycle_sequence_id);

/* Exported entry point function */
extern void Initialize(RT_MODELOWIVOLTAGEREGULATE_TYPE *const m);

/* Exported entry point function */
extern void Monitor(RT_MODELOWIVOLTAGEREGULATE_TYPE *const m);

/* Exported entry point function */
extern void Reset(RT_MODELOWIVOLTAGEREGULATE_TYPE *const m, uint8
                  u_setpoint_index);

/* Exported entry point function */
extern void Stop(RT_MODELOWIVOLTAGEREGULATE_TYPE *const m);

/* Exported data declaration */

/* Declaration for custom storage class: ExportGlobal */
extern uint16 OWI_VRO_DutyCycle_Comp;  /* '<S7>/Model' */
extern uint16 OWI_VRO_DutyCycle_Stable;/* '<S7>/Model' */

/* Declaration for custom storage class: Localizable */
extern BOOL_TYPE OWI_VRO_Reset;        /* synthesized block */

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
 * '<Root>' : 'OWIVoltageRegulatedOutputModel'
 * '<S1>'   : 'OWIVoltageRegulatedOutputModel/Call_OWI_VRO_Monitor'
 * '<S2>'   : 'OWIVoltageRegulatedOutputModel/DocBlock'
 * '<S3>'   : 'OWIVoltageRegulatedOutputModel/Get_SequenceId'
 * '<S4>'   : 'OWIVoltageRegulatedOutputModel/Read_SettingFile_Params'
 * '<S5>'   : 'OWIVoltageRegulatedOutputModel/Reset_Monitor'
 * '<S6>'   : 'OWIVoltageRegulatedOutputModel/Stop_Monitor'
 * '<S7>'   : 'OWIVoltageRegulatedOutputModel/Call_OWI_VRO_Monitor/Enable_OWI_VRO_Monitor'
 * '<S8>'   : 'OWIVoltageRegulatedOutputModel/Call_OWI_VRO_Monitor/Enable_OWI_VRO_Monitor/Get_Average_Coefficient'
 * '<S9>'   : 'OWIVoltageRegulatedOutputModel/Call_OWI_VRO_Monitor/Enable_OWI_VRO_Monitor/Get_OWI_Pilot_Type'
 * '<S10>'  : 'OWIVoltageRegulatedOutputModel/Call_OWI_VRO_Monitor/Enable_OWI_VRO_Monitor/Read_OWI_Voltage'
 * '<S11>'  : 'OWIVoltageRegulatedOutputModel/Call_OWI_VRO_Monitor/Enable_OWI_VRO_Monitor/Set_OWI_DutyCycle'
 * '<S12>'  : 'OWIVoltageRegulatedOutputModel/Call_OWI_VRO_Monitor/Enable_OWI_VRO_Monitor/Get_OWI_Pilot_Type/Enumerated Constant'
 * '<S13>'  : 'OWIVoltageRegulatedOutputModel/Call_OWI_VRO_Monitor/Enable_OWI_VRO_Monitor/Read_OWI_Voltage/Enumerated Constant'
 * '<S14>'  : 'OWIVoltageRegulatedOutputModel/Call_OWI_VRO_Monitor/Enable_OWI_VRO_Monitor/Set_OWI_DutyCycle/Enumerated Constant'
 * '<S15>'  : 'OWIVoltageRegulatedOutputModel/Call_OWI_VRO_Monitor/Enable_OWI_VRO_Monitor/Set_OWI_DutyCycle/Enumerated Constant1'
 * '<S16>'  : 'OWIVoltageRegulatedOutputModel/Read_SettingFile_Params/OWI_VRO_Linearization_Table_Displacement'
 * '<S17>'  : 'OWIVoltageRegulatedOutputModel/Read_SettingFile_Params/OWI_VRO_Params_Displacement'
 * '<S18>'  : 'OWIVoltageRegulatedOutputModel/Read_SettingFile_Params/OWI_VRO_SetPoint_Params_Displacement'
 * '<S19>'  : 'OWIVoltageRegulatedOutputModel/Read_SettingFile_Params/Subsystem'
 * '<S20>'  : 'OWIVoltageRegulatedOutputModel/Read_SettingFile_Params/OWI_VRO_Linearization_Table_Displacement/If Action Subsystem2'
 * '<S21>'  : 'OWIVoltageRegulatedOutputModel/Read_SettingFile_Params/OWI_VRO_Linearization_Table_Displacement/If Action Subsystem3'
 * '<S22>'  : 'OWIVoltageRegulatedOutputModel/Read_SettingFile_Params/OWI_VRO_Params_Displacement/If Action Subsystem'
 * '<S23>'  : 'OWIVoltageRegulatedOutputModel/Read_SettingFile_Params/OWI_VRO_Params_Displacement/If Action Subsystem1'
 * '<S24>'  : 'OWIVoltageRegulatedOutputModel/Read_SettingFile_Params/OWI_VRO_SetPoint_Params_Displacement/Enumerated Constant'
 * '<S25>'  : 'OWIVoltageRegulatedOutputModel/Read_SettingFile_Params/OWI_VRO_SetPoint_Params_Displacement/Enumerated Constant1'
 * '<S26>'  : 'OWIVoltageRegulatedOutputModel/Read_SettingFile_Params/OWI_VRO_SetPoint_Params_Displacement/Enumerated Constant2'
 * '<S27>'  : 'OWIVoltageRegulatedOutputModel/Read_SettingFile_Params/OWI_VRO_SetPoint_Params_Displacement/If Action Subsystem1'
 * '<S28>'  : 'OWIVoltageRegulatedOutputModel/Read_SettingFile_Params/OWI_VRO_SetPoint_Params_Displacement/If Action Subsystem2'
 * '<S29>'  : 'OWIVoltageRegulatedOutputModel/Read_SettingFile_Params/OWI_VRO_SetPoint_Params_Displacement/If Action Subsystem3'
 * '<S30>'  : 'OWIVoltageRegulatedOutputModel/Read_SettingFile_Params/OWI_VRO_SetPoint_Params_Displacement/If Action Subsystem4'
 * '<S31>'  : 'OWIVoltageRegulatedOutputModel/Read_SettingFile_Params/OWI_VRO_SetPoint_Params_Displacement/If Action Subsystem5'
 * '<S32>'  : 'OWIVoltageRegulatedOutputModel/Read_SettingFile_Params/OWI_VRO_SetPoint_Params_Displacement/If Action Subsystem6'
 */
#endif                        /* RTW_HEADER_OWIVoltageRegulatedOutputModel_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */

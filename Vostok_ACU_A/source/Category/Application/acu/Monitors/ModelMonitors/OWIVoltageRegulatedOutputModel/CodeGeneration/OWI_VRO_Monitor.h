/*
 * File: OWI_VRO_Monitor.h
 *
 * Code generated for Simulink model 'OWI_VRO_Monitor'.
 *
 * Model version                  : 1.125
 * Simulink Coder version         : 9.3 (R2020a) 18-Nov-2019
 * C/C++ source code generated on : Tue Aug  2 17:21:20 2022
 *
 * Target selection: monitor.tlc
 * Embedded hardware selection: ARM Compatible->ARM Cortex
 * Code generation objectives: Unspecified
 * Validation result: Not run
 */

#ifndef RTW_HEADER_OWI_VRO_Monitor_h_
#define RTW_HEADER_OWI_VRO_Monitor_h_
#include <math.h>
#ifndef OWI_VRO_Monitor_COMMON_INCLUDES_
# define OWI_VRO_Monitor_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* OWI_VRO_Monitor_COMMON_INCLUDES_ */

#include "OWI_VRO_Monitor_types.h"

/* Block signals for model 'OWI_VRO_Monitor' */
#ifndef OWI_VRO_Monitor_MDLREF_HIDE_CHILD_

typedef struct {
  uint16 max_dutycycle;                /* '<S6>/Switch' */
  uint16 read_time_ms;                 /* '<S5>/Data Type Conversion5' */
} BOWI_VRO_MONITORC_TYPE;

#endif                                 /*OWI_VRO_Monitor_MDLREF_HIDE_CHILD_*/

/* Block states (default storage) for model 'OWI_VRO_Monitor' */
#ifndef OWI_VRO_Monitor_MDLREF_HIDE_CHILD_

typedef struct {
  float32 besterror;                   /* '<S2>/Binary_Search_Logic' */
  uint16 lower;                        /* '<S2>/Binary_Search_Logic' */
  uint16 upper;                        /* '<S2>/Binary_Search_Logic' */
  uint16 bestdutycycle;                /* '<S2>/Binary_Search_Logic' */
  uint16 time_count;                   /* '<S2>/Binary_Search_Logic' */
  uint8 is_active_c1_owi_vro_monitor;  /* '<S2>/Binary_Search_Logic' */
  uint8 is_c1_owi_vro_monitor;         /* '<S2>/Binary_Search_Logic' */
  uint8 is_run;                        /* '<S2>/Binary_Search_Logic' */
  uint8 is_wait;                       /* '<S2>/Binary_Search_Logic' */
  BOOL_TYPE saturation;                /* '<S2>/Binary_Search_Logic' */
  BOOL_TYPE startover;                 /* '<S2>/Binary_Search_Logic' */
} DWOWI_VRO_MONITORF_TYPE;

#endif                                 /*OWI_VRO_Monitor_MDLREF_HIDE_CHILD_*/

extern void OWI_VRO_Monitor__Execute(BOOL_TYPE owi_vro_stop, BOOL_TYPE
  owi_vro_reset, HBL_PILOT_ENUM_TYPE owi_pilot_type, uint16 owi_vro_avg_coeff,
  uint32 owi_vro_voltage_reading, uint8 owi_vro_setpoint_index, uint8
  sf_owivrosetpoint1data_voltage_, uint16 sf_owivrosetpoint1data_compensa, uint8
  sf_owivrosetpoint2data_voltage_, uint16 sf_owivrosetpoint2data_compensa, uint8
  sf_owivrosetpoint3data_voltage_, uint16 sf_owivrosetpoint3data_compensa, const
  OWI_VRO_COMMON_SF_PARAMS_TYPE *sf_owivoltageregulatedoutput, uint8
  sf_owivrolinearizationnumitems, const uint16
  sf_owivrolinearizationtableduty[MAX_LINEARIZATION_TABLE], const uint16
  sf_owivrolinearizationtablemult[MAX_LINEARIZATION_TABLE], uint16
  *search_dutycycle, uint16 *owi_vro_dutycycle_stable, uint16
  *owi_vro_dutycycle_comp, uint8 *owi_vro_dutycycle_sequence_id,
  OWI_VRO_STATEMACHINE_TYPE *owi_vro_statemachine, float32
  *owi_vro_voltage_stable);

/* Exported data declaration */

/* Declaration for custom storage class: Localizable */
extern BOOL_TYPE OWI_VRO_Reset;        /* synthesized block */

#ifndef OWI_VRO_Monitor_MDLREF_HIDE_CHILD_

/* Block signals (default storage) */
extern BOWI_VRO_MONITORC_TYPE OWI_VRO_Monitor_b;

/* Block states (default storage) */
extern DWOWI_VRO_MONITORF_TYPE OWI_VRO_Monitor_dw;

#endif                                 /*OWI_VRO_Monitor_MDLREF_HIDE_CHILD_*/

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
 * '<Root>' : 'OWI_VRO_Monitor'
 * '<S1>'   : 'OWI_VRO_Monitor/DocBlock'
 * '<S2>'   : 'OWI_VRO_Monitor/OWI_VRO_Turbidity_Evaluation'
 * '<S3>'   : 'OWI_VRO_Monitor/OWI_VRO_Turbidity_Evaluation/Binary_Search_Logic'
 * '<S4>'   : 'OWI_VRO_Monitor/OWI_VRO_Turbidity_Evaluation/Input_Processing'
 * '<S5>'   : 'OWI_VRO_Monitor/OWI_VRO_Turbidity_Evaluation/Input_Processing/Data_Conversion'
 * '<S6>'   : 'OWI_VRO_Monitor/OWI_VRO_Turbidity_Evaluation/Input_Processing/Subsystem'
 * '<S7>'   : 'OWI_VRO_Monitor/OWI_VRO_Turbidity_Evaluation/Input_Processing/Subsystem1'
 * '<S8>'   : 'OWI_VRO_Monitor/OWI_VRO_Turbidity_Evaluation/Input_Processing/Subsystem/Compare To Constant'
 */
#endif                                 /* RTW_HEADER_OWI_VRO_Monitor_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */

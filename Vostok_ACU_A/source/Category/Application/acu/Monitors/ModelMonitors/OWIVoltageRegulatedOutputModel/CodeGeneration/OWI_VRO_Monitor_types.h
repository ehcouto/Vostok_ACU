/*
 * File: OWI_VRO_Monitor_types.h
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

#ifndef RTW_HEADER_OWI_VRO_Monitor_types_h_
#define RTW_HEADER_OWI_VRO_Monitor_types_h_
#include "rtwtypes.h"
#include "HblDefs.h"
#include "OwiVroRead.h"

/* Model Code Variants */

/* Includes for objects with custom storage classes. */
#include "OWIVoltageRegulatedOutput_prv.h"

/*
 * Check that imported macros with storage class "ImportedDefine" are defined
 */
#ifndef MAX_LINEARIZATION_TABLE
#error The variable for the parameter "MAX_LINEARIZATION_TABLE" is not defined
#endif

/*
 * Registered constraints for dimension variants
 */
/* Constraint 'MAX_LINEARIZATION_TABLE == 8' registered by:
 * '<S2>/Selector'
 * '<S2>/Selector1'
 */
#if MAX_LINEARIZATION_TABLE != 8
# error "The preprocessor definition 'MAX_LINEARIZATION_TABLE' must be equal to '8'"
#endif

#ifndef DEFINED_TYPEDEF_FOR_OWI_VRO_STATEMACHINE_TYPE_
#define DEFINED_TYPEDEF_FOR_OWI_VRO_STATEMACHINE_TYPE_

typedef enum {
  OWI_VRO_STATEMACHINE_TYPE_None = 0,  /* Default value */
  OWI_VRO_STATEMACHINE_TYPE_STOPPED,
  OWI_VRO_STATEMACHINE_TYPE_INITIALIZATION,
  OWI_VRO_STATEMACHINE_TYPE_WAIT_START,
  OWI_VRO_STATEMACHINE_TYPE_WAIT_DELAY,
  OWI_VRO_STATEMACHINE_TYPE_BINARY_SEARCH,
  OWI_VRO_STATEMACHINE_TYPE_COMPLETE,
  OWI_VRO_STATEMACHINE_TYPE_SATURATED
} OWI_VRO_STATEMACHINE_TYPE;

#endif
#endif                                 /* RTW_HEADER_OWI_VRO_Monitor_types_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */

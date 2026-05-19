/*
 * File: OWIVoltageRegulatedOutputModel_types.h
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

#ifndef RTW_HEADER_OWIVoltageRegulatedOutputModel_types_h_
#define RTW_HEADER_OWIVoltageRegulatedOutputModel_types_h_
#include "rtwtypes.h"
#include "HblDefs_prm.h"
#include "SettingFile_prm.h"
#include "HblDefs.h"
#include "OwiVroRead.h"
#include "SettingFile.h"

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
#if MAX_LINEARIZATION_TABLE <= 0
# error "The preprocessor definition 'MAX_LINEARIZATION_TABLE' must be greater than '0'"
#endif

#if MAX_LINEARIZATION_TABLE >= 9
# error "The preprocessor definition 'MAX_LINEARIZATION_TABLE' must be less than '9'"
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

/* Forward declaration for rtModel */
typedef struct TAG_RTMOWIVOLTAGEREGULATED_TYPE RT_MODELOWIVOLTAGEREGULATE_TYPE;

#endif                  /* RTW_HEADER_OWIVoltageRegulatedOutputModel_types_h_ */

/*
 * File trailer for generated code.
 *
 * [EOF]
 */

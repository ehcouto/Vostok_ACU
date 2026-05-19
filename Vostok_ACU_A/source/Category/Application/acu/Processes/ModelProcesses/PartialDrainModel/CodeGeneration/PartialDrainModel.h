/**
 * @file: PartialDrainModel.h
 *
 * @model: 'PartialDrainModel'.
 *
 * @brief Partial drain algorithm is a global solution to drain an specific amount of water from the dishwasher machine, it uses Virtual Water Indicator (VWI) information and Sensed Drain algorithm, that is an algorithm to fully drain the machine using timers and sensed data, to understand how much water was already drained from the machine and stop the draining at the correct moment.
 * This algorithm can be used, but it's not limited to, filter cleaning, dirty water removal and to improve water consumption.
 *
 * @modelversion:  1.214
 *
 * @coderversion:  9.3 (R2020a) 18-Nov-2019
 *
 * C/C++ source code generated on : Tue Aug 18 12:10:25 2020
 *
 * @copyright  Copyright 2018. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//-------------------
#ifndef RTW_HEADER_PartialDrainModel_h_
#define RTW_HEADER_PartialDrainModel_h_
#include "PartialDrainProcess.h"
#include "Process.h"
#include "SystemTimers.h"
#include "Vwi.h"
#include "C_Extensions.h"
#ifndef PartialDrainModel_COMMON_INCLUDES_
# define PartialDrainModel_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* PartialDrainModel_COMMON_INCLUDES_ */

/* Model Code Variants */

//--------------------

/* Macros for accessing real-time model data structure */

/* Forward declaration for rtModel */
typedef struct TAG_RTMPARTIALDRAINMODEL_TYPE RT_MODELPARTIALDRAINMODEL_TYPE;

/* Block signals and states (default storage) for system '<Root>' */
typedef struct
{
    PARTIALDRAIN_MODELDATA_TYPE partialdraindata;/* '<Root>/Data Store Memory' */
    ON_OFF_TYPE drain_request;         /* '<Root>/Chart' */
    struct
    {
        uint_T is_partial_drain:2;     /* '<Root>/Chart' */
        uint_T is_pump_control:2;      /* '<Root>/Chart' */
        uint_T is_active_c3_partialdrainmodel:1;/* '<Root>/Chart' */
    }
    bitsfortid0;

    uint8 fbk_count;                   /* '<Root>/Chart' */
}
DWPARTIALDRAINMODEL_TYPE;

/* Real-time Model Data Structure */
struct TAG_RTMPARTIALDRAINMODEL_TYPE
{
    DWPARTIALDRAINMODEL_TYPE *dwork;
};

/* Model entry point functions */
void PartialDrainModel__Initialize(RT_MODELPARTIALDRAINMODEL_TYPE *const m,
    PROCESS_RESULT_TYPE *y_partialdrain_processstatus, PASS_FAIL_TYPE
    *y_partialdrain_processresult);
void PartialDrainModel__Execute(RT_MODELPARTIALDRAINMODEL_TYPE *const m,
    PROCESS_CONTROL_TYPE *u_partialdrain_control,
    SF_PARTIALDRAIN_MACHINECONFIG_TYPE *u_sf_partialdrain_machineconfig,
    SF_PARTIALDRAIN_PROCESSCONFIG_TYPE *u_sf_partialdrain_processconfig,
    PROCESS_RESULT_TYPE *y_partialdrain_processstatus, PASS_FAIL_TYPE
    *y_partialdrain_processresult);

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
 * '<Root>' : 'PartialDrainModel'
 * '<S1>'   : 'PartialDrainModel/Chart'
 * '<S2>'   : 'PartialDrainModel/Model Info'
 */
#endif                                 /* RTW_HEADER_PartialDrainModel_h_ */

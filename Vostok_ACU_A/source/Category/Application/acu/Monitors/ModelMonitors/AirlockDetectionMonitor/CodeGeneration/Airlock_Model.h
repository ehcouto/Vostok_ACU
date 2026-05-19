/**
 * @file: Airlock_Model.h
 *
 * @model: 'Airlock_Model'.
 *
 * @brief
 *
 * @modelversion:  1.181
 *
 * @coderversion:  8.14 (R2018a) 06-Feb-2018
 *
 * C/C++ source code generated on : Wed Jun 12 08:22:58 2019
 *
 * @copyright  Copyright 2017-2018. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef RTW_HEADER_Airlock_Model_h_
#define RTW_HEADER_Airlock_Model_h_
#include <string.h>
#ifndef Airlock_Model_COMMON_INCLUDES_
# define Airlock_Model_COMMON_INCLUDES_
#include "rtwtypes.h"
#endif                                 /* Airlock_Model_COMMON_INCLUDES_ */

#include "Airlock_Model_types.h"

/* Includes for objects with custom storage classes. */
#include "Airlock_Model_defs.h"

/* Macros for accessing real-time model data structure */

/* Block signals and states (default storage) for system 'Airlock_Model' */
typedef struct
{
    AIRLOCK_STATUS_TYPE Airlock_Status;/* 'Airlock_Definition' (':295') */
    uint16 avg_storage[AVG_BUFFER_SIZE];/* 'avg_storage_DS' (':331') */
    uint16 avg_delta;                  /* 'Airlock_Definition' (':295') */
    uint16 timeout_count;              /* 'Airlock_Definition' (':295') */
    uint16 prev_avg;                   /* 'prev_avg_DS' (':369') */
    uint8 avg_index;                   /* 'avg_index_DS' (':332') */
    uint8 is_active_c4_Airlock_Model;  /* 'Variance_Hysteresis' (':296') */
    uint8 is_c4_Airlock_Model;         /* 'Variance_Hysteresis' (':296') */
    uint8 is_active_c3_Airlock_Model;  /* 'Airlock_Definition' (':295') */
    uint8 is_c3_Airlock_Model;         /* 'Airlock_Definition' (':295') */
    BOOL_TYPE airlock_flag;            /* 'Airlock_Definition' (':295') */
    BOOL_TYPE wasAirlocked;            /* 'wasAirlocked_DS' (':328') */
}
DW_Airlock_Model_T;

/* Block signals and states (default storage) */
extern DW_Airlock_Model_T Airlock_Model_DW;

/* Model entry point functions */
void AirlockModel__Initialize(void);

/* Customized model step function */
AIRLOCK_STATUS_TYPE AirlockModel__Execute(uint16 Vwi_Signal, uint16 Vwi_Variance,
    const AIRLOCK_PARAMS_TYPE *Sf_AirlockData);

/*-
 * The generated code includes comments that allow you to trace directly
 * back to the appropriate location in the model.  The basic format
 * is 'block_name' ('SID'), where block_name is the name of the block
 * and SID is the Simulink identifier of the block without the model name.
 *
 * Use the MATLAB hilite_system command to trace the generated code back
 * to the model.  For example,
 *
 * hilite_system(':3')    - opens block with Simulink identifier 'model:3'
 */

/*-
 * Requirements for model: Airlock_Model
 */
#endif                                 /* RTW_HEADER_Airlock_Model_h_ */

/**
 * @file: Airlock_Model_types.h
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
#ifndef RTW_HEADER_Airlock_Model_types_h_
#define RTW_HEADER_Airlock_Model_types_h_
#include "rtwtypes.h"

/* Includes for objects with custom storage classes. */
#include "Airlock_Model_defs.h"

/*
 * Registered constraints for dimension variants
 */
#if AVG_BUFFER_SIZE <= 0
# error "The preprocessor definition 'AVG_BUFFER_SIZE' must be greater than '0'"
#endif

#ifndef DEFINED_TYPEDEF_FOR_AIRLOCK_PARAMS_TYPE_
#define DEFINED_TYPEDEF_FOR_AIRLOCK_PARAMS_TYPE_

typedef PACKED struct
{
    uint16 VwiAirlockDelta;
    uint16 VwiVarianceLowThold;
    uint16 VwiVarianceHighThold;
    uint16 AirlockRiskTimeout;
    uint16 VwiRecoveryDelta;
}
AIRLOCK_PARAMS_TYPE;

#endif

#ifndef DEFINED_TYPEDEF_FOR_AIRLOCK_STATUS_TYPE_
#define DEFINED_TYPEDEF_FOR_AIRLOCK_STATUS_TYPE_

typedef enum
{
    ALS_NO_AIRLOCK = 0,
    ALS_AIRLOCK,
    ALS_AIRLOCK_RISK,
    ALS_UNDEFINED,
    ALS_NONE                           /* Default value */
}
AIRLOCK_STATUS_TYPE;

#endif
#endif                                 /* RTW_HEADER_Airlock_Model_types_h_ */

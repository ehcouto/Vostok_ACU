/**
 * @file
 * @brief       Public interface to the AccelBaseline module.
 *
 * @details     Refer to the AccelBaseline.c source file for more detailed information.
 *
 * @copyright   Copyright 2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef ACCELBASELINE_H_
#define ACCELBASELINE_H_

#if (HBL_LLI_NUM_ACCELEROMETER > 0)

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public Constant Declarations --

//! The size of the acceleromater baseline data that needs to be saved on the ACU.
#define ACCEL_BASELINE_SIZE 6


// -- Public Enumerated Constant Declarations --

//! The status of the module's self-test activities.
typedef enum ACCEL_BASELINE_STATUS_ENUM
{
    ACCEL_IDLE                      = 0,
    ACCEL_MEASURING_BASELINE        = 1,
    ACCEL_RUNNING_SELF_TEST         = 2,
    ACCEL_BASELINE_DISABLED         = 3
} ACCEL_BASELINE_STATUS_TYPE;


// -- Public Type Declarations --


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================

void AccelBaseline__Initialize(void);
void AccelBaseline__Background5ms(void);
void AccelBaseline__GetBaseline(sint16* vector);
uint8* AccelBaseline__GetBaselineAddress(void);
uint8 AccelBaseline__GetBaselineErrors(void);
uint8 AccelBaseline__GetSelfTestErrors(void);
ACCEL_BASELINE_STATUS_TYPE AccelBaseline__GetStatus(void);
BOOL_TYPE AccelBaseline__MeasureBaseline(void);
BOOL_TYPE AccelBaseline__RunSelfTest(void);
void AccelBaseline__SetInitialBaseline(uint8* array, uint8 length);


#endif  // (HBL_LLI_NUM_ACCELEROMETER > 0)
#endif  // ACCELBASELINE_H_

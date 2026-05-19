/**
 *  @file       
 *
 *  @brief      Public interface for Stepper Pilot.
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

#ifndef STEPPERPILOT_H_
#define STEPPERPILOT_H_
//  --- Include Files -------------------------------------------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"

// -- Other Modules --

#ifndef HBL_PILOT_NUM_STEPPER
    #define HBL_PILOT_NUM_STEPPER 0
#endif

#if (HBL_PILOT_NUM_STEPPER > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//! Stepper pilot direction type definition.
typedef enum STEPPER_DIRECTION_ENUM
{
    STEPPER_DIRECTION_FORWARD = 0,
    STEPPER_DIRECTION_REVERSE
} STEPPER_DIRECTION_TYPE;

//! Stepper pilot request type definition.
PACKED typedef struct STEPPER_PILOT_REQUEST_STRUCT
{
    uint16 Number_Of_Steps;                         //!< Number of steps, shall be divisible by number of Sequences(4 or 8).
    STEPPER_DIRECTION_TYPE Direction;               //!< Direction to step through sequence array.
    uint8  Pulse_Width;                             //!< Step pulse width in milliseconds.
} STEPPER_PILOT_REQUEST_TYPE;

//! Function pointer definition for pilot methods.
#define STEPPER_PILOT_REGISTER  StepperPilot__Initialize,           \
                                StepperPilot__Allocate,             \
                                StepperPilot__InitializeInstance,   \
                                StepperPilot__Request,              \
                                StepperPilot__Process,              \
                                StepperPilot__GetRequested,         \
                                StepperPilot__GetProcessed,         \
                                NULL,                               \
                                NULL,                               \
                                NULL,                               \
                                StepperPilot__Handler1ms,           \
                                StepperPilot__GetStatus,            \
                                4

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void            StepperPilot__Initialize            (void);
PASS_FAIL_TYPE  StepperPilot__InitializeInstance    (HBL_LOAD_TYPE *load);
uint8           StepperPilot__Allocate              (void);
PASS_FAIL_TYPE  StepperPilot__Request               (HBL_LOAD_TYPE *load, void *data);
PASS_FAIL_TYPE  StepperPilot__Process               (HBL_LOAD_TYPE *load);
void *          StepperPilot__GetRequested          (HBL_LOAD_TYPE *load);
void *          StepperPilot__GetProcessed          (HBL_LOAD_TYPE *load);
PASS_FAIL_TYPE  StepperPilot__Handler1ms            (HBL_LOAD_TYPE *load);
void *          StepperPilot__GetStatus             (HBL_LOAD_TYPE *load);

#endif // (HBL_PILOT_NUM_STEPPER > 0)

#endif // STEPPERPILOT_H_

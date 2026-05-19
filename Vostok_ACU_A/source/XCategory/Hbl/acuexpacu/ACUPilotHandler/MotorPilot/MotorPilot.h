/**
 *  @file
 *
 *  @brief      Public interface to MotorPilot module.
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
#ifndef MOTORPILOT_H_
#define MOTORPILOT_H_

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"

#ifndef HBL_PILOT_NUM_MOTOR
    #define HBL_PILOT_NUM_MOTOR 0
#endif

#if (HBL_PILOT_NUM_MOTOR > 0)
#include "MotorPilot_prm.h"
#include "HblDefs.h"
#include "API039MotionCtrl.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


// -- Public Type Declarations --

//! Define the motor type interface
typedef struct MOTOR_REQ_STRUCT
{
    uint8 Size;
    API039_ACTIVITY_TYPE ReqActivity;
    uint8 Data[MOTOR_PILOT_MAX_BYTE_REQ_NUM];
}MOTOR_REQ_TYPE;


// -- Public List Declarations --

//! Function pointer definition for pilot methods.
#define MOTOR_PILOT_REGISTER                MotorPilot__Initialize,          \
                                            MotorPilot__Allocate,            \
                                            MotorPilot__InitializeInstance,  \
                                            MotorPilot__Request,             \
                                            MotorPilot__Process,             \
                                            MotorPilot__GetRequested,        \
                                            MotorPilot__GetProcessed,        \
                                            NULL,                                \
                                            NULL,                                \
                                            NULL,                                \
                                            NULL,                                \
                                            MotorPilot__GetStatus,           \
                                            sizeof(MOTOR_REQ_TYPE)


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================


void            MotorPilot__Initialize           (void);
uint8           MotorPilot__Allocate             (void);
PASS_FAIL_TYPE  MotorPilot__InitializeInstance   (HBL_LOAD_TYPE *load);
PASS_FAIL_TYPE  MotorPilot__Request              (HBL_LOAD_TYPE *load,void *data);
PASS_FAIL_TYPE  MotorPilot__Process              (HBL_LOAD_TYPE *load);
void *          MotorPilot__GetRequested         (HBL_LOAD_TYPE *load);
void *          MotorPilot__GetProcessed         (HBL_LOAD_TYPE *load);
void *          MotorPilot__GetStatus            (HBL_LOAD_TYPE *load);


#endif      // #if (HBL_PILOT_NUM_MOTOR > 0)

#endif      // MOTORPILOT_H_

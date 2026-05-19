/**
 * @file
 * @brief       Public interface to the ExternalMotorPilot module.
 *
 * @details     Refer to the ExternalMotorPilot.c source file for more detailed information.
 *
 * @copyright   Copyright 2016 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef EXTERNALMOTORPILOT_H_
#define EXTERNALMOTORPILOT_H_


#ifndef HBL_PILOT_NUM_EXTERNAL_MOTOR
    #define HBL_PILOT_NUM_EXTERNAL_MOTOR 0
#endif

#if (HBL_PILOT_NUM_EXTERNAL_MOTOR > 0)
//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --
#include "ExternalMotorDeclare.h"
#include "HblDefs.h"


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public List Declarations --

//! Function pointer definition for pilot methods.
#define EXTERNAL_MOTOR_PILOT_REGISTER       ExternalMotorPilot__Initialize,         \
                                            ExternalMotorPilot__Allocate,           \
                                            ExternalMotorPilot__InitializeInstance, \
                                            ExternalMotorPilot__Request,            \
                                            ExternalMotorPilot__Process,            \
                                            ExternalMotorPilot__GetRequested,       \
                                            ExternalMotorPilot__GetProcessed,       \
                                            NULL,                                   \
                                            NULL,                                   \
                                            NULL,                                   \
                                            NULL,                                   \
                                            ExternalMotorPilot__GetStatus,          \
                                            sizeof(EXTERNAL_MOTOR_COMMAND_TYPE)


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


void            ExternalMotorPilot__Initialize           (void);
uint8           ExternalMotorPilot__Allocate             (void);
PASS_FAIL_TYPE  ExternalMotorPilot__InitializeInstance   (HBL_LOAD_TYPE* load);
PASS_FAIL_TYPE  ExternalMotorPilot__Request              (HBL_LOAD_TYPE* load, void* data);
PASS_FAIL_TYPE  ExternalMotorPilot__Process              (HBL_LOAD_TYPE* load);
void*           ExternalMotorPilot__GetRequested         (HBL_LOAD_TYPE* load);
void*           ExternalMotorPilot__GetProcessed         (HBL_LOAD_TYPE* load);
void*           ExternalMotorPilot__GetStatus            (HBL_LOAD_TYPE* load);


#endif      // #if (HBL_PILOT_NUM_EXTERNAL_MOTOR > 0)

#endif      // EXTERNALMOTORPILOT_H_

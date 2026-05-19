/**
 *  @file
 *
 *  @brief      Public interface to MotorPilot module.
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
#ifndef MOTORPILOT_PRM_H_
#define MOTORPILOT_PRM_H_

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"

#ifndef HBL_PILOT_NUM_MOTOR
    #define HBL_PILOT_NUM_MOTOR 0
#endif

#if (HBL_PILOT_NUM_MOTOR > 0)

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//! Define how many byte are reserved for command buffer
#define MOTOR_PILOT_MAX_BYTE_REQ_NUM 7


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif

#endif /* MOTORPILOT_PRM_H_ */

/**
 * @file
 * @brief       Public interface to the PumpCommand module.
 *
 * @details     Refer to the PumpCommand.c source file for more detailed information.
 *
 * @copyright   Copyright 2016 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef PUMPCOMMAND_H_
#define PUMPCOMMAND_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --
#include "Hbl.h"


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public Constant Declarations --
//! The default acceleration to use when no acceleration is specified.
#define DEFAULT_ACCELERATION 500
#define MAX_ACCELERATION 4000
#define MIN_ACCELERATION 1

#define DEFAULT_DECELERATION 500

#define MAX_SPEED 5500
#define MIN_SPEED 1000

/**
 * A macro to build the standard DEBLOCKING command data structure for the ExternalMotorPilot
 * Example Use:
 *      EMP_BUILD_DEBLOCKING_COMMAND(run, velocity, acceleration);
 *      Hbl__SetLoadRequestByLoadID(HBL_LOAD_MOTOR, priority, run);
 * @param var = An arbitrary name to use as the run command variable in the call to HBL.
 * @param velocity = The target velocity (rpm) to which the motor should ramp.
 * @param acceleration = The acceleration (rpm/s) to use to ramp the motor to the requested velocity.
 */
#define EMP_BUILD_DEBLOCKING_COMMAND(var, velocity, acceleration) \
    uint8 (var)[] = {EXTERNAL_MOTOR_ROTATE_X_DEGREES, 0x00, 0x00, LOBYTE(velocity), HIBYTE(velocity), LOBYTE(acceleration), HIBYTE(acceleration)}

// -- Public Type Declarations --

//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


void PumpCommand__MaintainPreviousCommand(HBL_PRIORITY_TYPE priority, HBL_LOAD_ENUM_TYPE pump);
void PumpCommand__Run(sint16 velocity, uint16 acceleration, HBL_PRIORITY_TYPE priority, HBL_LOAD_ENUM_TYPE pump);
void PumpCommand__RunWithDefAccel(sint16 velocity, HBL_PRIORITY_TYPE priority, HBL_LOAD_ENUM_TYPE pump);
void PumpCommand__Deblocking(sint16 velocity, uint16 acceleration, HBL_PRIORITY_TYPE priority, HBL_LOAD_ENUM_TYPE pump);
void PumpCommand__Stop(uint16 deceleration, HBL_PRIORITY_TYPE priority, HBL_LOAD_ENUM_TYPE pump);
void PumpCommand__StopWithDefDecel(HBL_PRIORITY_TYPE priority, HBL_LOAD_ENUM_TYPE pump);


#endif      // PUMPCOMMAND_H_

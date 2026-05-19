/**
 * @file
 * @brief       Public declarations used when interfacing with an External Motor.
 *
 * @copyright   Copyright 2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef EXTERNALMOTORDECLARE_H_
#define EXTERNALMOTORDECLARE_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "C_Extensions.h"

// -- Other Modules --


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public Constant Declarations --

//! The maximum size of a custom ExternalMotorPilot command.
//! There is no advantage to reducing this size below the maximum standard command (11).
#define EXTERNAL_MOTOR_MAX_CUSTOM_COMMAND_SIZE 11


// -- Public Enumerated Constant Declarations --

//! Standard simple one-byte commands that are handled by the ExternalMotorPilot.
//! These short-hand commands can be used by the LoadsControl process to control the motor.
typedef enum EXTERNAL_MOTOR_SIMPLE_COMMAND_ENUM
{
    EXTERNAL_MOTOR_NO_COMMAND = 0,                  //!< No command was provided to the external motor pilot.
    EXTERNAL_MOTOR_NO_CHANGE = 1,                   //!< A command to preserve the previous command.
    EXTERNAL_MOTOR_STOP = 2,                        //!< Stop the motor. 2 bytes deceleration.
    EXTERNAL_MOTOR_RUN = 3,                         //!< Run the motor. 2 bytes for target velocity. 2 bytes for acceleration.
    EXTERNAL_MOTOR_WASH = 4,                        //!< Perform standard wash action.
    EXTERNAL_MOTOR_ROTATE_X_DEGREES = 5,            //!< Perform standard controlled rotation action.
    EXTERNAL_MOTOR_PULSE_IMMEDIATE = 6,             //!< Perform standard pulse action when received.
    EXTERNAL_MOTOR_PULSE_MODIFY = 7,                //!< Modify (if possible) the running pulse with the new pulse definition.
                                                    //!<    Same as EXTERNAL_MOTOR_PULSE_IMMEDIATE if no pulse is running.
    EXTERNAL_MOTOR_PULSE_QUEUE = 8,                 //!< Queue a pulse to run after the current pulse completes.
                                                    //!<    Same as EXTERNAL_MOTOR_PULSE_IMMEDIATE if no pulse is running.
    EXTERNAL_MOTOR_STANDARD_COMMAND_COUNT           //!< Number of standard commands.
} EXTERNAL_MOTOR_SIMPLE_COMMAND_TYPE;


// -- Public Type Declarations --

//! The parameters expected for a EXTERNAL_MOTOR_STOP command.
typedef PACKED struct EMP_STOP_PARAM_STRUCT
{
    uint16 Deceleration;            //!< The deceleration rate (rpm/s) to use while stopping.
                                    //!< A value of 0 indicates that the motor will coast.
} EMP_STOP_PARAM_TYPE;

//! The parameters expected for a EXTERNAL_MOTOR_RUN command.
typedef PACKED struct EMP_RUN_PARAM_STRUCT
{
    sint16 Velocity;                //!< The run velocity (rpm).
    uint16 Acceleration;            //!< The acceleration (rpm/s) used to ramp to the velocity.
} EMP_RUN_PARAM_TYPE;

//! The parameters expected for a EXTERNAL_MOTOR_WASH command.
typedef PACKED struct EMP_WASH_PARAM_STRUCT
{
    uint16 On_Time;                 //!< The time (ms) that the motor will ramp up and run at the target velocity.
    uint16 Off_Time;                //!< The time (ms) that the motor will ramp down and wait at 0 rpm.
    sint16 Velocity;                //!< The target velocity (rpm) used for each stroke.
                                    //!<    If the value is positive, then the first stroke will be clockwise.
    uint16 Acceleration;            //!< The acceleration (rpm/s) used to ramp up from 0 rpm to the target velocity.
    uint16 Deceleration;            //!< The deceleration (rpm/s) used to ramp down from the target velocity to 0 rpm.
} EMP_WASH_PARAM_TYPE;

//! The parameters expected for a EXTERNAL_MOTOR_ROTATE_X_DEGREES command.
typedef PACKED struct EMP_ROTATE_PARAM_STRUCT
{
    sint16 Degrees;                 //!< The angular distance (degrees) that the motor should rotate before stopping.
    sint16 Velocity;                //!< The target velocity (rpm) used for the rotation.
                                    //!<    If the value is positive, then the rotation is clockwise.
    uint16 Acceleration;            //!< The acceleration (rpm/s) used to ramp to the velocity.
} EMP_ROTATE_PARAM_TYPE;

//! The parameters expected for a EXTERNAL_MOTOR_PULSE_IMMEDIATE, EXTERNAL_MOTOR_PULSE_MODIFY, or
//! a EXTERNAL_MOTOR_PULSE_QUEUE command.
typedef PACKED struct EMP_PULSE_PARAM_STRUCT
{
    uint16 On_Time;                 //!< The time (ms) that the motor will ramp up and run at the target velocity.
    uint16 Off_Time;                //!< The time (ms) that the motor will ramp down and wait at 0 rpm.
                                    //!<    If Off_Time is 0ms, then the motor will not be stopped.
    sint16 Velocity;                //!< The target velocity (rpm) used for the pulse.
                                    //!<    If the value is positive, then the pulse is clockwise.
    uint16 Acceleration;            //!< The acceleration (rpm/s) used to ramp up from 0 rpm to the target velocity.
    uint16 Deceleration;            //!< The deceleration (rpm/s) used to ramp down from the target velocity to 0 rpm.
                                    //!< A value of 0 will cause the next pulse to start from the specified Velocity.
} EMP_PULSE_PARAM_TYPE;

//! The data structure for a standard motor command.
typedef PACKED struct EMP_STANDARD_COMMAND_STRUCT
{
    EXTERNAL_MOTOR_SIMPLE_COMMAND_TYPE Command;
    union EMP_PARAMS_STRUCT
    {
        EMP_STOP_PARAM_TYPE Stop;
        EMP_RUN_PARAM_TYPE Run;
        EMP_WASH_PARAM_TYPE Wash;
        EMP_ROTATE_PARAM_TYPE Rotate;
        EMP_PULSE_PARAM_TYPE Pulse;
    } Params;
} EMP_STANDARD_COMMAND_TYPE;

//! The maximum size of a standard ExternalMotorPilot command.
#define EXTERNAL_MOTOR_MAX_STANDARD_COMMAND_SIZE sizeof(EMP_STANDARD_COMMAND_TYPE)

//! The data structure for a custom motor command.
typedef PACKED struct EMP_CUSTOM_COMMAND_STRUCT
{
    uint8 Size              : 7;    //!< Number of bytes used by this command (including this byte).
    uint8 Is_Custom_Command : 1;    //!< Always set to 1 to indicate a custom command.
    uint8 Params[EXTERNAL_MOTOR_MAX_CUSTOM_COMMAND_SIZE - 1];  //!< Custom data for the custom command.
} EMP_CUSTOM_COMMAND_TYPE;

//! A data structure with generic fields for both standard and custom motor commands.
typedef PACKED struct EMP_COMMON_COMMAND_STRUCT
{
    uint8 Command;
    uint8 Params[EXTERNAL_MOTOR_MAX_STANDARD_COMMAND_SIZE - 1U];
} EMP_COMMON_COMMAND_TYPE;

//! A data structure for a standard or custom motor command.
typedef union EXTERNAL_MOTOR_COMMAND_STUCT
{
    EMP_STANDARD_COMMAND_TYPE Standard;
    EMP_CUSTOM_COMMAND_TYPE Custom;
    EMP_COMMON_COMMAND_TYPE Common;
} EXTERNAL_MOTOR_COMMAND_TYPE;

typedef struct EXTERNAL_MOTOR_STATUS_STRUCT
{
    BOOL_TYPE Is_Connected;
    uint8 Motor_Index;
} EXTERNAL_MOTOR_STATUS_TYPE;

//! The maximum size of any ExternalMotorPilot command.
#define EXTERNAL_MOTOR_MAX_COMMAND_SIZE sizeof(EXTERNAL_MOTOR_COMMAND_TYPE)


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================

/**
 * A macro to build the standard STOP command data structure for the ExternalMotorPilot.
 * Example Use:
 *      EMP_BUILD_STOP_COMMAND(stop, deceleration);
 *      Hbl__SetLoadRequestByLoadID(HBL_LOAD_MOTOR, priority, stop);
 * @param var = An arbitrary name to use as the stop command variable in the call to HBL.
 * @param deceleration = The deceleration (rpm/s) to use when stopping the motor.
 */
#define EMP_BUILD_STOP_COMMAND(var, deceleration) \
    uint8 (var)[] = {EXTERNAL_MOTOR_STOP, LOBYTE(deceleration), HIBYTE(deceleration)}

/**
 * A macro to build the standard RUN command data structure for the ExternalMotorPilot
 * Example Use:
 *      EMP_BUILD_RUN_COMMAND(run, velocity, acceleration);
 *      Hbl__SetLoadRequestByLoadID(HBL_LOAD_MOTOR, priority, run);
 * @param var = An arbitrary name to use as the run command variable in the call to HBL.
 * @param velocity = The target velocity (rpm) to which the motor should ramp.
 * @param acceleration = The acceleration (rpm/s) to use to ramp the motor to the requested velocity.
 */
#define EMP_BUILD_RUN_COMMAND(var, velocity, acceleration) \
    uint8 (var)[] = {EXTERNAL_MOTOR_RUN, LOBYTE(velocity), HIBYTE(velocity), LOBYTE(acceleration), HIBYTE(acceleration)}

/**
 * A macro to build the standard WASH command data structure for the ExternalMotorPilot.
 * Example Use:
 *      EMP_BUILD_WASH_COMMAND(wash, on_time, off_time, velocity, acceleration, deceleration);
 *      Hbl__SetLoadRequestByLoadID(HBL_LOAD_MOTOR, priority, wash);
 * @param var = An arbitrary name to use as the wash command variable in the call to HBL.
 * @param on_time = The time (ms) that the motor will ramp up and run at the velocity.
 * @param off_time = The time (ms) that the motor will ramp down and wait while it is stopped.
 * @param velocity = The target velocity (rpm) used for each stroke. If the value is positive,
 *                   then the first stroke will be clockwise.
 * @param accel = The acceleration (rpm/s) used to ramp up to the velocity.
 * @param decel = The deceleration (rpm/s) used to ramp down to 0 rpm.
 */
#define EMP_BUILD_WASH_COMMAND(var, on_time, off_time, velocity, acceleration, deceleration) \
    uint8 (var)[] = {EXTERNAL_MOTOR_WASH, LOBYTE(on_time), HIBYTE(on_time), LOBYTE(off_time), HIBYTE(off_time), \
                     LOBYTE(velocity), HIBYTE(velocity), LOBYTE(acceleration), HIBYTE(acceleration), \
                     LOBYTE(deceleration), HIBYTE(deceleration)}

/**
 * A macro to build the standard ROTATE command data structure for the ExternalMotorPilot.
 * Example Use:
 *      EMP_BUILD_ROTATE_COMMAND(rotate, degrees, velocity, acceleration);
 *      Hbl__SetLoadRequestByLoadID(HBL_LOAD_MOTOR, priority, rotate);
 * @param var = An arbitrary name to use as the rotate command variable in the call to HBL.
 * @param degrees = The angular distance (degrees) that the motor should rotate before stopping.
 * @param velocity = The target velocity (rpm) used for the rotation.
 *                   If the value is positive, then the rotation is clockwise.
 * @param acceleration = The acceleration (rpm/s) used to ramp to the velocity.
 */
#define EMP_BUILD_ROTATE_COMMAND(var, degrees, velocity, acceleration) \
    uint8 (var)[] = {EXTERNAL_MOTOR_ROTATE_X_DEGREES, LOBYTE(degrees), HIBYTE(degrees), \
                     LOBYTE(velocity), HIBYTE(velocity), LOBYTE(acceleration), HIBYTE(acceleration)}

/**
 * A macro to build the standard PULSE_IMMEDIATE command data structure for the ExternalMotorPilot.
 * Example Use:
 *      EMP_BUILD_PULSE_IMMEDIATE_COMMAND(pulse, on_time, off_time, velocity, acceleration, deceleration);
 *      Hbl__SetLoadRequestByLoadID(HBL_LOAD_MOTOR, priority, pulse);
 * @param var = An arbitrary name to use as the wash command variable in the call to HBL.
 * @param on_time = The time (ms) that the motor will ramp up and run at the target velocity.
 * @param off_time = The time (ms) that the motor will ramp down and wait at 0 rpm.
                     If Off_Time is 0ms, then the motor will not be stopped.
 * @param velocity = The target velocity (rpm) used for the pulse.
                     If the value is positive, then the pulse is clockwise.
 * @param accel = The acceleration (rpm/s) used to ramp up from 0 rpm to the target velocity.
 * @param decel = The deceleration (rpm/s) used to ramp down from the target velocity to 0 rpm.
 */
#define EMP_BUILD_PULSE_IMMEDIATE_COMMAND(var, on_time, off_time, velocity, acceleration, deceleration) \
    uint8 (var)[] = {EXTERNAL_MOTOR_PULSE_IMMEDIATE, LOBYTE(on_time), HIBYTE(on_time), \
                     LOBYTE(off_time), HIBYTE(off_time), LOBYTE(velocity), HIBYTE(velocity), \
                     LOBYTE(acceleration), HIBYTE(acceleration), \
                     LOBYTE(deceleration), HIBYTE(deceleration)}

/**
 * A macro to build the standard PULSE_MODIFY command data structure for the ExternalMotorPilot.
 * Example Use:
 *      EMP_BUILD_PULSE_MODIFY_COMMAND(pulse, on_time, off_time, velocity, acceleration, deceleration);
 *      Hbl__SetLoadRequestByLoadID(HBL_LOAD_MOTOR, priority, pulse);
 * @param var = An arbitrary name to use as the wash command variable in the call to HBL.
 * @param on_time = The time (ms) that the motor will ramp up and run at the target velocity.
 * @param off_time = The time (ms) that the motor will ramp down and wait at 0 rpm.
                     If Off_Time is 0ms, then the motor will not be stopped.
 * @param velocity = The target velocity (rpm) used for the pulse.
                     If the value is positive, then the pulse is clockwise.
 * @param accel = The acceleration (rpm/s) used to ramp up from 0 rpm to the target velocity.
 * @param decel = The deceleration (rpm/s) used to ramp down from the target velocity to 0 rpm.
 */
#define EMP_BUILD_PULSE_MODIFY_COMMAND(var, on_time, off_time, velocity, acceleration, deceleration) \
    uint8 (var)[] = {EXTERNAL_MOTOR_PULSE_MODIFY, LOBYTE(on_time), HIBYTE(on_time), \
                     LOBYTE(off_time), HIBYTE(off_time), LOBYTE(velocity), HIBYTE(velocity), \
                     LOBYTE(acceleration), HIBYTE(acceleration), \
                     LOBYTE(deceleration), HIBYTE(deceleration)}

/**
 * A macro to build the standard PULSE_QUEUE command data structure for the ExternalMotorPilot.
 * Example Use:
 *      EMP_BUILD_PULSE_QUEUE_COMMAND(pulse, on_time, off_time, velocity, acceleration, deceleration);
 *      Hbl__SetLoadRequestByLoadID(HBL_LOAD_MOTOR, priority, pulse);
 * @param var = An arbitrary name to use as the wash command variable in the call to HBL.
 * @param on_time = The time (ms) that the motor will ramp up and run at the target velocity.
 * @param off_time = The time (ms) that the motor will ramp down and wait at 0 rpm.
                     If Off_Time is 0ms, then the motor will not be stopped.
 * @param velocity = The target velocity (rpm) used for the pulse.
                     If the value is positive, then the pulse is clockwise.
 * @param accel = The acceleration (rpm/s) used to ramp up from 0 rpm to the target velocity.
 * @param decel = The deceleration (rpm/s) used to ramp down from the target velocity to 0 rpm.
 */
#define EMP_BUILD_PULSE_QUEUE_COMMAND(var, on_time, off_time, velocity, acceleration, deceleration) \
    uint8 (var)[] = {EXTERNAL_MOTOR_PULSE_QUEUE, LOBYTE(on_time), HIBYTE(on_time), \
                     LOBYTE(off_time), HIBYTE(off_time), LOBYTE(velocity), HIBYTE(velocity), \
                     LOBYTE(acceleration), HIBYTE(acceleration), \
                     LOBYTE(deceleration), HIBYTE(deceleration)}


#endif      // EXTERNALMOTORDECLARE_H_

/**
 * @file
 * @brief       Simplifies formatting of motor commands to control the drum through HBL.
 *
 * @details
 *
 * @copyright   Copyright 2016 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "PumpCommand.h"

#include "SystemConfig.h"
#include "ExternalMotorDeclare.h"
#include <string.h>


//  --- Private Declarations ------------------------------------------------------------------------------------------


// -- Private Constant Declarations --


// -- Private Enumerated Constant Declarations --


// -- Private Structure Type Declarations --


//  --- Private Definitions -------------------------------------------------------------------------------------------


// -- Private Flash-Based Constant Definitions --


// -- Private Variable Definitions --


//  --- Private Function Prototypes -----------------------------------------------------------------------------------

//=====================================================================================================================
//  --- Public Functions ----------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Send a command to maintain the drum command that was previously sent.
 * @param priority = The priority to use when asking to maintain the previous command
 * @param pump = The pump to use when asking to maintain the previous command
 */
void PumpCommand__MaintainPreviousCommand(HBL_PRIORITY_TYPE priority, HBL_LOAD_ENUM_TYPE pump)
{
    uint8 cmd = EXTERNAL_MOTOR_NO_CHANGE;
    (void)Hbl__SetLoadRequestByLoadID(pump, priority, &cmd);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Send a run command that specifies the pump acceleration.
 * @param velocity = The pump target velocity in rpm
 * @param acceleration = The pump acceleration rate in rpm/s
 * @param priority = The priority to use to run the motor
 * @param pump = The pump to use to run
 */
void PumpCommand__Run(sint16 velocity, uint16 acceleration, HBL_PRIORITY_TYPE priority, HBL_LOAD_ENUM_TYPE pump)
{
    EMP_BUILD_RUN_COMMAND(run, velocity, acceleration);
    (void)Hbl__SetLoadRequestByLoadID(pump, priority, run);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Send a run command that does not specify the drum acceleration.
 * @param velocity = The pump target velocity in rpm
 * @param priority = The priority to use to run the motor
 * @param pump = The pump to use to run
 */
void PumpCommand__RunWithDefAccel(sint16 velocity, HBL_PRIORITY_TYPE priority, HBL_LOAD_ENUM_TYPE pump)
{
    PumpCommand__Run(velocity, DEFAULT_ACCELERATION, priority, pump);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Send a deblocking command that specifies the pump acceleration.
 * @param velocity = The pump target velocity in rpm
 * @param acceleration = The pump acceleration rate in rpm/s
 * @param priority = The priority to use to run the motor
 * @param pump = The pump to use to run
 */
void PumpCommand__Deblocking(sint16 velocity, uint16 acceleration, HBL_PRIORITY_TYPE priority, HBL_LOAD_ENUM_TYPE pump)
{
	EMP_BUILD_DEBLOCKING_COMMAND(run, velocity, acceleration);
	(void)Hbl__SetLoadRequestByLoadID(pump, priority, run);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Send a stop command to stop pump.
 * @param deceleration = The pump deceleration rate in rpm/s
 * @param priority = The priority to use to stop the motor
 * @param pump = The pump to use to stop
 */
void PumpCommand__Stop(uint16 deceleration, HBL_PRIORITY_TYPE priority, HBL_LOAD_ENUM_TYPE pump)
{
	EMP_BUILD_STOP_COMMAND(stop, deceleration);
	(void)Hbl__SetLoadRequestByLoadID(pump, priority, stop);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Send a Stop command that does not specify the pump deceleration.
  * @param priority = The priority to use to run the motor
  * @param pump = The pump to use to stop
 */
void PumpCommand__StopWithDefDecel(HBL_PRIORITY_TYPE priority, HBL_LOAD_ENUM_TYPE pump)
{
	EMP_BUILD_STOP_COMMAND(stop, DEFAULT_DECELERATION);
	(void)Hbl__SetLoadRequestByLoadID(pump, priority, stop);
}

//=====================================================================================================================
//  --- Private Functions ---------------------------------------------------------------------------------------------
//=====================================================================================================================


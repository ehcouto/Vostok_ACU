/**
 * @file
 * @brief       Private parameters for the LoadsControl module.
 *
 * @details     Refer to the C source file for more detailed information.
 *
 * @copyright   Copyright 2016-2020 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef LOADSCONTROL_PRV_H_
#define LOADSCONTROL_PRV_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --


//=====================================================================================================================
//  --- Private Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Private Constant Declarations --

//! Set to ENABLED to make the priority of the request based on the load value being set.
//! For On/Off loads, this will make the action to turn a load on (value of 1) a higher priority
//! than the action to turn a load off (value of 0).
#define LOADSCONTROL_HIGH_REQUEST_WINS DISABLED

//! Set this value to the maximum number of instances of the LoadsControl process that can be
//! executed in a single step of a cycle.
//! For example, "2" means that the same step can use two instances of the LoadsControl process.
//! One instance could control a load with hysteresis while the other turned loads on.
#define LOADS_CONTROL_MAX_INSTANCES 3

//! Allows recovery of pulsed loads after a cold reset or a power interruption.
//!     ENABLED =   Pulsed loads are resumed after an interruption. If the load was off when it was
//!                 interrupted, then the load will complete the remainder of the off pulse and
//!                 then start the on portion of the pulse.If the load was on when it was
//!                 interrupted, then the load will complete the remainder of the on pulse and then
//!                 start the off portion of the pulse. Refer to the RESUME_ON_TIME_MIN description.
//!     DISABLED =  Pulsed loads are restarted after an interruption.
//! If declaration is missing, feature is DISABLED.
#define LOADS_CONTROL_RESUME_PULSING ENABLED

#if (LOADS_CONTROL_RESUME_PULSING == ENABLED)
    //! Sets the minimum ON time to apply to a resumed ON pulse. For pulsing to resume in the ON
    //! state, the following conditions must be satisfied:
    //!     1. Pulsing was interrupted while the loads were ON.
    //!     2. The remaining pulse ON time when it was interrupted was >= RESUME_ON_TIME_MIN.
    //! If the ON pulse was interrupted, but the ON time was less than RESUME_ON_TIME_MIN, then
    //! LoadsControl will immediately start counting the OFF pulse time.
	#define RESUME_ON_TIME_MIN 0
#endif


//=====================================================================================================================
//  --- Private Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


#endif      // LOADSCONTROL_PRV_H_

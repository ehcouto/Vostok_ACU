/**
 * @file
 * @brief       Private parameters for the Fault_prv module.
 *
 * @details     Refer to the C source file for more detailed information.
 *
 * @copyright   Copyright 2016 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef FAULT_PRV_H_
#define FAULT_PRV_H_


//  --- Include Files -------------------------------------------------------------------------------------------------
#include "StatisticsDish.h"

// -- This Module --

// -- Other Modules --


//=====================================================================================================================
//  --- Private Properties --------------------------------------------------------------------------------------------
//=====================================================================================================================

// -- Private Constant Declarations --

//! The maximum number of faults that this module can observe at one time.
//! This is the number of parallel state machines that can detect faults simultaneously.
//! Support for each state machine uses 16 bytes of RAM.
#define MAX_PARALLEL_FAULT_OBSERVERS    6

//! The maximum number of handlers for fault events.
//! All fault event handlers will be called each time that a fault occurs.
//! Core code fault handlers: Supervisor, Statistics
//! Support for each fault handler uses 4 bytes of RAM.
#define MAX_FAULT_HANDLERS              3

//! The maximum number of handlers for pre-fault events.
//! All pre-fault event handlers will be called each time that a pre-fault occurs.
//! Core code pre-fault handlers: Supervisor, SubCycle, Statistics
//! Support for each pre-fault handler uses 4 bytes of RAM.
#define MAX_PRE_FAULT_HANDLERS          5


//! Macro used to synchronize the reset of fault detection firmware internal variables with the
//! reset of the faults in the Fault module.
#define RESET_FAULT_DETECTORS()			StatisticsDish__UpdateServiceFaultClearOdometer()

// -- Private Type Declarations --


//=====================================================================================================================
//  --- Private Methods -----------------------------------------------------------------------------------------------
//=====================================================================================================================


#endif      /* FAULT_PRV_H_ */

/**
 * @file
 * @brief       Public parameters for the WashHeaterFaultMonitor module.
 *
 * @details     Refer to the C source file for more detailed information.
 *
 * @copyright   Copyright 2016-2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef WASHHEATERFAULTMONITOR_PRM_H_
#define WASHHEATERFAULTMONITOR_PRM_H_



//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public Constant Declarations --

//! Set HEATER_OPEN_UNIQUELY_DETECTED to ENABLED if the hardware allow the heater open/disconnected failure
//!  to be distinguished from other failures, such as open relay (e.g. in Victoria board).
#define HEATER_OPEN_UNIQUELY_DETECTED					ENABLED

//! Set HEATER_FAILURES_MANAGED_WITHOUT_PILOT_RELAY to ENABLED if the pilot relay load status and the related
//! feedback are not used in heater failures detection  (e.g. in Victoria board).
#define HEATER_FAILURES_MANAGED_WITHOUT_PILOT_RELAY		DISABLED

//! Set HEATER_OPEN_PLUS_L_RELAY_CLOSED_DETECTION to ENABLED if the hardware allows the detection of the simultaneous
//! presence of combined failures line relay stuck closed and heater open (e.g. in Victoria board).
#define HEATER_OPEN_PLUS_L_RELAY_CLOSED_DETECTION		ENABLED

//! Set HEATER_OPEN_PLUS_N_RELAY_OPEN_DETECTION to ENABLED if the hardware allows the detection of the simultaneous
//! presence of combined failures neutral relay stuck open and heater open (e.g. in Victoria board).
#define HEATER_OPEN_PLUS_N_RELAY_OPEN_DETECTION			ENABLED

//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================



#endif      // WASHHEATERFAULTMONITOR_PRM_H_

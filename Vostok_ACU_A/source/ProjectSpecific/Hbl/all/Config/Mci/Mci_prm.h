/**
 *  @file
 *  @brief      Public interface to the Mci module.
 *
 *  $Header: Mci_prm.h 1.4 2014/12/08 04:48:33EST Maciej  Kobuszewski (KOBUSM) Exp  $
 *
 *  @copyright  Copyright 2014 - $Date: 2014/12/08 04:48:33EST $  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
#ifndef MCI_PRM_H_
#define MCI_PRM_H_

//  --- Include Files -------------------------------------------------------------------------------------------------
#include "Mcl_prv.h"
#include "MotorTypeDefs.h"
#include "C_Extensions.h"
#include "TimeCapture.h"

// MOTOR COMMAND MODEs
#define MAIN_uC            0
#define MCI_BD             1

#define COMMAND_MODE                    MAIN_uC

// features
#ifndef COMPILE_4_SIMULINK
	#define MCL_DEBUG_MODE                  DISABLED
#else
	#define MCL_DEBUG_MODE                  ENABLED
#endif

#if(MCL_DEBUG_MODE == ENABLED)
#define FREEMASTER_HANDLER_200US
//#define FREEMASTER_HANDLER_1MS
//#define FREEMASTER_HANDLER_5MS
#endif

#define TIM6_FOR_MC_HANDLER                 DISABLED

//MCU Debug
//#define ENABLE_PILOT_MCI
//#define MCI_AUTOTESTING
//#define MCI_INTERNAL_PARAMETERS
//#define TIME_MEASUREMENT
//#define INITIALIZE_WASH_MOTOR

#ifdef COMPILE_4_SIMULINK
	#define MCI_INTERNAL_PARAMETERS
	#define SIL_MODE                      ENABLED
#else
	#define SIL_MODE                      DISABLED
	#define ENABLE_API220_CALLBACKS
#endif

//=====================================================================================================================
//  --- Private Properties --------------------------------------------------------------------------------------------
//=====================================================================================================================

//! Macros to manage Mci APIs on different projects
#define MCI_GETDIGITAL(motor, data)					Mci__GetDigital(motor, data)
#define MCI_GETANYERROR(motor)						Mci__GetError(motor)
#define MCI_RUN(motor)								Mci__Run(motor, 100, 0)
#define MCI_STOP(motor)								Mci__Stop(motor, 100)
#define MCI_STOP_IMMEDIATELY(motor)				    Mci__Stop(motor, MCI_ACC_EXECUTE_IMMEDIATELY)

#endif  // MCI_PRM_H_

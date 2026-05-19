/**
 *  @file       SystemConfig_prm.h
 *
 *  @brief      Public parameters of the SystemConfig module.
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/01 17:33:20EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SYSTEMCONFIG_PRM_H
#define SYSTEMCONFIG_PRM_H

#include "C_Extensions.h"
#include "Uart.h"

//=====================================================================================================================
//-------------------------------------- APPLICATION ------------------------------------------------------------------
//=====================================================================================================================
//! Number of appliance compartments running cycles (Maximum 8).
//! Category (platform) specific (same for all projects in a specific category).
#define APPLIANCE_MAX_COMPARTMENTS      (1U)

//! Define the number of compartments in the system.
//! This cannot be at the end of the COMPARTMENT_TYPE enumeration because it is needed by the pre-compiler.
#define NUM_OF_COMPARTMENTS             APPLIANCE_MAX_COMPARTMENTS
#define COMPARTMENT_DEFAULT             (0)
#define VWI_FEATURE						ENABLED

//! If ENABLED the project compiles all the features related to new sensed drain.
#define NEW_SENSED_DRAIN_FEATURES		ENABLED

//=====================================================================================================================
//-------------------------------------- GENERAL     -------------------------------------------------
//=====================================================================================================================
//--------------------------------------- System Board Configuration --------------------------------------------------
//! ENABLED if this board is the Main ACU.
#define SYSTEM_ACU_HANDLING             ENABLED

//! ENABLED if this board is the Main HMI.
#define SYSTEM_HMI_HANDLING             DISABLED

//! ENABLED if this board is an MCU board.
#define SYSTEM_MCU_HANDLING             DISABLED

//! ENABLED if this board is an Expansion ACU.
#define SYSTEM_ACU_EXPANSION_HANDLING   DISABLED

//! ENABLED if this board is an Expansion HMI.
#define SYSTEM_HMI_EXPANSION_HANDLING   DISABLED

//! ENABLED if this board manages Expansion boards.
#define SYSTEM_EXPANSION_HANDLING       ENABLED

//! ENABLED if this board is an Expansion board and drives a load on the Main ACU.
#define EXPANSION_DRIVES_MAIN_LOAD      DISABLED

//! ENABLED if this board is a Main ACU and allows an Expansion board to drive a load on this board.
#define ACU_ACCEPTS_LOAD_FROM_EXPANSION DISABLED

//! ENABLED if this board supports connectivity APIs and handling.
#define SYSTEM_CONNECTIVITY_HANDLING    DISABLED

//! ENABLED to control remote application-layer functions on external nodes.
#define API013_CLIENT_FEATURE           ENABLED

//! ENABLED to receive command related to remote application-layer functions from external nodes.
#define API013_SERVER_FEATURE           DISABLED

#if (SYSTEM_ACU_HANDLING == ENABLED)
//! Set API221_CLIENT_FEATURE to ENABLED if the application does the following:
//!     - Sends commands to an external node that directly controls a motor
//!     - Gets status from an external node that directly controls a motor
#define API221_CLIENT_FEATURE           DISABLED
//! Set API221_SERVER_FEATURE to ENABLED if the application does the following:
//!     - Gets commands from an external node to control a motor
//!     - Sends motor control status to an external node
#define API221_SERVER_FEATURE           DISABLED

//! ENABLED to use API004 feature
#define API004DEBUG_FEATURE      		ENABLED

//! ENABLED if Cycle Time Estimation feature is requested
#define TIME_ESTIMATION_FEATURE         ENABLED
#endif

/**
 * This definition enables/disables the integration of Global Service Diagnostic handling.
 */
#if (SYSTEM_HMI_HANDLING == ENABLED)
	#define SERVICE_USE_GES131_METHOD 	ENABLED
#endif

//--------------------------------------- Board WIN Configuration -----------------------------------------------------
//! Node address for the Main ACU
#define WIN_ACU_ADDRESS                 1

//! Node address for the Main HMI
#define WIN_UI_ADDRESS                  4

//! Node address for MCU
#define WIN_MCU_ADDRESS                 0

//! Node address for the first ACU Expansion board
#define WIN_HMI_EXP_ADDRESS             5

//! Node address for the first ACU Expansion board
#define WIN_ACU_EXP_ADDRESS             2

//! Node address for Expansion
#define WIN_EXP_ADDRESS                 2

//! Define Comunications port used by this board.
#define WIN_PORT                       COM2

//! Define the WIN Node for this board (Hard coded or default for dynamic addressing).
#define WIN_NODE_DEF                    WIN_ACU_ADDRESS

//! ENABLED if communication with other WIN nodes needs to be monitored
#define COMMUNICATION_MONITOR_FEATURE   ENABLED

#if (COMMUNICATION_MONITOR_FEATURE == ENABLED)
#define  COMMUNICATION_MONITOR_NODES_TO_MONITOR		{ WIN_ACU_ADDRESS        ,0  }

//! The CommunicationMonitor module may require this definition, but it is not used (even when required).
#define COMMUNICATION_MONITOR_FAULT_ID_COMMUNICATION_ACU 0

#endif

//=====================================================================================================================
//------------------------------------------------- HBL ---------------------------------------------------------------
//=====================================================================================================================

//! Uncomment to override HBLACU_ZEROCROSS_FEATURE default value:
//!		ENABLED for ACU, DISABLED for HMI
#define HBLACU_ZEROCROSS_FEATURE		ENABLED
#define FAST_HANDLER_FEATURE            ENABLED
#define FAST_HANDLER_TIMER              TIMER1
#define FAST_HANDLER_TIMER_CHANNEL      TIMER_CHANNEL4

//---------------------------------------------------------------------------------------------------------------------
// Virtual Pin variants
//---------------------------------------------------------------------------------------------------------------------

#define	STATUS_LED_FEATURE			ENABLED

#if ((SYSTEM_ACU_HANDLING == ENABLED) || (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED))
//---------------------------------------------------------------------------------------------------------------------
// ACU LLI - Define the type and number of LLIs on this board (Not defined or defined as 0 will unlink the feature).
//---------------------------------------------------------------------------------------------------------------------
#define HBL_LLI_NUM_ANALOG          16
#define HBL_LLI_NUM_DIGITAL         16
#define HBL_LLI_NUM_CONDUCTIVITY    0
#define HBL_LLI_NUM_FEEDBACK        1
#define HBL_LLI_NUM_INPUTCAPTURE	0
#define HBL_LLI_NUM_TRIACFEEDBACK	0
#define HBL_LLI_NUM_PULSECOUNTER	1
#define HBL_LLI_NUM_VIRTUAL_DIGITAL 0
#define HBL_LLI_NUM_ENCODER         0
#define HBL_LLI_NUM_MULTIINPUT      0
#define HBL_LLI_NUM_EXPANSION      	3


//---------------------------------------------------------------------------------------------------------------------
// HMI - LED Driver Configuration
//---------------------------------------------------------------------------------------------------------------------
#define BUZZER_SOUND_DEVICE     DISABLED
#define LED_DRIVER_CAT9532      DISABLED
#define LED_DRIVER_PCA9952      DISABLED
#define LED_DRIVER_EXTERNAL     DISABLED
#define LED_DRIVER_MATRIX       DISABLED
#define LED_DRIVER_ISSI37XX     DISABLED

//---------------------------------------------------------------------------------------------------------------------
// ACU GI - Define the type and number of GIs on this board (Not defined or defined as 0 will unlink the feature).
//---------------------------------------------------------------------------------------------------------------------
#define HBL_GI_NUM_PROBE					1 // Used for HBL_GI_TURBIDITY_SENSOR_OWI, that is a uinr16 but doesn't request a Conversion Table
#define HBL_GI_NUM_DOORSWITCH				0
#define HBL_GI_NUM_WATERCOUNTER				1
#define HBL_GI_NUM_PROBE_LPS				0
#define HBL_GI_NUM_CONVERT					18
#define HBL_GI_NUM_ABSOLUTE_ENCODER			0
#define HBL_GI_NUM_INCREMENTAL_ENCODER		0
#define HBL_GI_NUM_KEY						0
#define HBL_GI_NUM_DISCRETE_POTENTIOMETER	0
#define HBL_GI_NUM_ANALOG_POTENTIOMETER		0

//---------------------------------------------------------------------------------------------------------------------
// ACU LOADS - Define the type and number of Loads on this board (Not defined or defined as 0 will unlink the feature).
//---------------------------------------------------------------------------------------------------------------------
#define HBL_LOAD_NUM_DCFAN   		0
#define HBL_LOAD_NUM_ALTERNATE		1
#define HBL_LOAD_NUM_TRIACMCI       1
#define HBL_LOAD_NUM_HEATER	        1

//---------------------------------------------------------------------------------------------------------------------
// ACU Pilots - Define the type and number of Pilots on this board (Not defined or defined as 0 will unlink the feature).
//---------------------------------------------------------------------------------------------------------------------
#define HBL_PILOT_NUM_CHARGEPUMP            1
#define HBL_PILOT_NUM_DIGITALOUTPUT         20
#define HBL_PILOT_NUM_DIGITALOUTPUTMULTI    2
#define HBL_PILOT_NUM_ZCRELAY               4
#define HBL_PILOT_NUM_FSM		            0
#define HBL_PILOT_NUM_STEADYPWMPROFILE      2
#define HBL_PILOT_NUM_MULTISEQUENCE			0
#define HBL_PILOT_NUM_MOTOR					0
#define HBL_PILOT_NUM_PWM_OUTPUT			1
#define HBL_PILOT_NUM_PWM_HI_RES_OUTPUT		1
#define HBL_PILOT_NUM_ANALOGOUTPUT			0
#if (SYSTEM_ACU_HANDLING == ENABLED)
    #define HBL_PILOT_NUM_EXTERNAL_MOTOR    0
#endif

#endif      // #if ((SYSTEM_ACU_HANDLING == ENABLED) || (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED))


//=====================================================================================================================
//---------------------------------------DRIVERS ----------------------------------------------------------------------
//=====================================================================================================================

#endif // SYSTEMCONFIG_PRM_H

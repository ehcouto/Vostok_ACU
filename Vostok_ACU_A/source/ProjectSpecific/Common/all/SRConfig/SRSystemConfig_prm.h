/**
 *  @file       SRSystemConfig_prm.h
 *
 *  @brief      Public (prm) configuration file for SRSystemConfig module
 *
 *  @defgroup	CLASS_B
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRSYSTEMCONFIG_PRM_H_
#define SRSYSTEMCONFIG_PRM_H_
#include "C_Extensions.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//! ENABLED to enable CLASS B features
#define		CLASSB_FEATURE			        ENABLED

//! ENABLED to send or receive Class B content over Reveal.
#define     API020SAFETY_FEATURE            DISABLED

#define SR_HMI_EVENTS_FEATURE               DISABLED

#if (CLASSB_FEATURE == ENABLED)
    // SYSTEM_ENABLE_TASKSEQUENCE must be ENABLED if CLASSB_FEATURE is enabled.
    #define SYSTEM_ENABLE_TASKSEQUENCE      ENABLED
    //! ENABLED if ISR Monitor (check if ISR are executed within an expected occurrence range) needs to be enabled - mandatory for Class B software
    #define SYSTEM_ENABLE_ISRMONITOR        ENABLED
#else
    // SYSTEM_ENABLE_TASKSEQUENCE can be DISABLED if CLASSB_FEATURE is disabled.
    #define SYSTEM_ENABLE_TASKSEQUENCE      DISABLED
    //! ENABLED if ISR Monitor (check if ISR are executed within an expected occurrence range) needs to be enabled - mandatory for Class B software
    #define SYSTEM_ENABLE_ISRMONITOR        DISABLED
#endif

//! Enable/Disable the feature to monitor the accuracy of the clock
#define SRCLOCKMONITOR_FEATURE				ENABLED

//! DISABLED to disable Low Power feature (enabled by default)
#define		LOW_POWER_FEATURE				ENABLED

//---------------------------------------------------------------------------------------------------------------------
// GPIO - SR Unused Pins Check
//---------------------------------------------------------------------------------------------------------------------
#define SR_UNUSED_PINS_CONFIG_FEATURE		DISABLED

//---------------------------------------------------------------------------------------------------------------------
// LLI - Define the type and number of LLIs on this board (Not defined or defined as 0 will unlink the feature).
//---------------------------------------------------------------------------------------------------------------------
#define HBL_LLI_NUM_SRDIGITAL       0
#define HBL_LLI_NUM_SRANALOG        0
#define HBL_LLI_NUM_SRFEEDBACK      0
#define HBL_LLI_NUM_SRTRIACFEEDBACK 0
#define HBL_LLI_NUM_SRINPUTCAPTURE  0

//---------------------------------------------------------------------------------------------------------------------
// SRPilots - Define the type and number of Pilots on this board (Not defined or defined as 0 will unlink the feature).
//---------------------------------------------------------------------------------------------------------------------
#define HBL_PILOT_NUM_SRDOORLOCKEM			0
#define HBL_PILOT_NUM_SRDOORLOCKWAX			0
#define HBL_PILOT_NUM_SRDIGITALOUTPUTMULTI	0


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif // SRSYSTEMCONFIG_PRM_H_

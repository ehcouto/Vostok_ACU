/**
 *  @file       SRSystemConfig.h
 *
 *  @brief      Public declarations that define the type of Safety Relevant system the micro is supporting.
 *
 *  @defgroup	CLASS_B
 *
 *  @details    This module contains all system-level features that are enabled or disabled from a project
 *  $Header: $
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

/**
 *  \def CLASSB_FEATURE
 *  \brief Enables all Class B features
 *
 *  \def SYSTEM_ENABLE_TASKSEQUENCE
 *  \brief Enables Class B Task Sequence feature - mandatory ENABLED if CLASSB_FEATURE is ENABLED
 *
 *  \def SYSTEM_ENABLE_ISRMONITOR
 *  \brief Enables Class B ISR Monitoring feature - mandatory ENABLED if CLASSB_FEATURE is ENABLED
 *
 *  \def API020SAFETY_FEATURE
 *  \brief Provides a mechanism to transfer Class B content over Reveal.
 *
 *  \def SR_HMI_EVENTS_FEATURE
 *  \brief Enables mechanism to send safety relevant events from HMI to ACU.
 *
 *  \def SETTINGFILE_SR_CAT_DATA_FEATURE
 *  \brief Enables definitions of category specific safety relevant setting file data.
 *
 *  \def LOW_POWER_FEATURE
 *  \brief Enables Low Power Mode - ENABLED by default
 *
 *  \def SR_HBLHMI_FEATURE
 *  \brief Enables Class B features for the HblHmi module (only used on HMIs or P1/P2 boards)
 *
 *  \def SRPRODUCTMGR_FEATURE
 *  \brief Enable SRProductMgr module - ENABLED if CLASSB_FEATURE is ENABLED
 *
 *  \def SRPRODUCTMGR_SRACTION_FEATURE
 *  \brief Enable SRAction feature on SRProductMgr module - ENABLED if CLASSB_FEATURE is ENABLED, valid only if SRPRODUCTMGR_FEATURE is ENABLED
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRSYSTEMCONFIG_H_
#define SRSYSTEMCONFIG_H_

#include "C_Extensions.h"
#include "SRSystemConfig_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#ifndef CLASSB_FEATURE
    // API020SAFETY_FEATURE should be ENABLED for applications that must transfer Class B data over Reveal.
    #error "CLASSB_FEATURE must be defined via SRSystemConfig.h as ENABLED or DISABLED."
	// This define is only to show it in documentation - error is raised anyway
	#define CLASSB_FEATURE	ENABLED
#endif

#if (CLASSB_FEATURE == ENABLED)

//--------------------------------------- SR System Configuration -----------------------------------------------------
    #ifndef SRCLOCKMONITOR_FEATURE
        #define SRCLOCKMONITOR_FEATURE          ENABLED
    #endif

    #ifndef SYSTEM_ENABLE_TASKSEQUENCE
        #define SYSTEM_ENABLE_TASKSEQUENCE	    ENABLED
    #endif

    #ifndef SYSTEM_ENABLE_ISRMONITOR
        #define SYSTEM_ENABLE_ISRMONITOR	    ENABLED
    #endif

    #ifndef SRPRODUCTMGR_FEATURE
        #define SRPRODUCTMGR_FEATURE            ENABLED
    #endif

    #if (SYSTEM_ENABLE_TASKSEQUENCE == DISABLED)
        #error "SYSTEM_ENABLE_TASKSEQUENCE must be ENABLED if CLASSB_FEATURE is enabled."
    #endif

    #if (SYSTEM_ENABLE_ISRMONITOR == DISABLED)
        #error "SYSTEM_ENABLE_ISRMONITOR  must be ENABLED if CLASSB_FEATURE is enabled."
    #endif

#else

    #ifndef SRCLOCKMONITOR_FEATURE
        #define SRCLOCKMONITOR_FEATURE          DISABLED
    #endif

    #ifndef SYSTEM_ENABLE_TASKSEQUENCE
        #define SYSTEM_ENABLE_TASKSEQUENCE      DISABLED
    #endif

    #ifndef SYSTEM_ENABLE_ISRMONITOR
        #define SYSTEM_ENABLE_ISRMONITOR        DISABLED
    #endif

    #ifndef SRPRODUCTMGR_FEATURE
        #define SRPRODUCTMGR_FEATURE            DISABLED
    #endif

#endif

// SRAction feature is ENABLED by default: if SRPRODUCTMGR_FEATURE is DISABLED it will not have any effect
#ifndef SRPRODUCTMGR_SRACTION_FEATURE
    #define SRPRODUCTMGR_SRACTION_FEATURE       ENABLED
#endif

//--------------------------------------- API020 Configuration ------------------------------------------------------
#ifndef API020SAFETY_FEATURE
    #define API020SAFETY_FEATURE                DISABLED
#endif

//--------------------------------------- SRHmiEvents Configuration -------------------------------------------------
#ifndef SR_HMI_EVENTS_FEATURE
    #define SR_HMI_EVENTS_FEATURE               DISABLED
#endif

//--------------------------------------- Category Specific SR Setting File Data ------------------------------------
#ifndef SETTINGFILE_SR_CAT_DATA_FEATURE
    #define SETTINGFILE_SR_CAT_DATA_FEATURE     DISABLED
#endif

//--------------------------------------- Low Power Configuration ---------------------------------------------------
#ifndef LOW_POWER_FEATURE
	#define LOW_POWER_FEATURE                   ENABLED
#endif

//--------------------------------------- SRHblHmi Configuration ----------------------------------------------------
#ifndef SR_HBLHMI_FEATURE
    #define SR_HBLHMI_FEATURE                   DISABLED
#endif

//--------------------------------------- SRUnusedPinsconfig Configuration ----------------------------------------------------
#ifndef SR_UNUSED_PINS_CONFIG_FEATURE
    #define SR_UNUSED_PINS_CONFIG_FEATURE       DISABLED
#endif

//--------------------------------------- Class B Interrupt Monitor Check Configuration ---------------------------------------------------
#if (SYSTEM_ENABLE_ISRMONITOR == ENABLED)
	#include "SRIsrMonitor.h"
	#define ISR_MONITOR_ENTER()                 SRIsrMonitor__EnterIsr()
	#define ISR_MONITOR_EXIT()                  SRIsrMonitor__ExitIsr()
#else
	#define ISR_MONITOR_ENTER()
	#define ISR_MONITOR_EXIT()
#endif

// List of all time-slot based Tasks
typedef enum
{
    MAIN_TASK,
    APPLICATION_TASK,
    FASTAPPMGR_TASK,
    ASYNCH_TASK,
    COMMUNICATION_TASK,

    // This must be the last item
    NUM_TASKS
} TASK_ID_TYPE;

// List of all Stacks - time-slot based Task and System Stack
typedef enum
{
    MAIN_TASK_STACK,
    APPLICATION_TASK_STACK,
    FASTAPPMGR_TASK_STACK,
    ASYNCH_TASK_STACK,
    COMMUNICATION_TASK_STACK,

    //System Stacks
    SYSTEM_STACK,
    INTERRUPT_STACK,

    // This must be the last item
    NUM_STACKS,
} STACK_ID_TYPE;


#endif // SRSYSTEMCONFIG_H_

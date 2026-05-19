/**
 *  @file       main_prv.h
 *  @brief		Private parameters for the SRMain module.
 *
 *  @copyright  Copyright 2015-2018: 2015/08/17 13:11:27EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

#ifndef SRMAIN_PRV_H
#define SRMAIN_PRV_H

//-------------------------------------- Include Files ----------------------------------------------------------------
//! Basic
#include "SystemConfig.h"
#include "SeggerRTT.h"

#if ((SYSTEM_HMI_HANDLING == ENABLED) || (SYSTEM_HMI_EXPANSION_HANDLING == ENABLED))
    #include "HblHmi.h"
#endif

#if (SYSTEM_ACU_HANDLING == ENABLED)
	#include "APIMotorControl.h"
    #include "BrownOut.h"
    #include "DishAuxVar.h"
    #include "Fault.h"
    #include "HeaterShadowing.h"
    #include "LowPower.h"
    #include "StabilityControl.h"
    #include "Statistics.h"
    #include "StatisticsDish.h"
    #include "TestMgr.h"
    #include "Vwi.h"
	#include "Mci.h"
	#include "MciZeroCross.h"
#endif

#if ((SYSTEM_ACU_HANDLING == ENABLED) || (SYSTEM_HMI_HANDLING == ENABLED))
    #include "Regulations.h"
    #include "Status.h"
#endif

#if ((SYSTEM_ACU_HANDLING == ENABLED) || (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED))
    #include "ApplianceMgr.h"
    #include "Hbl.h"
#endif

#include "API003Disc.h"
#include "API005LowLevel.h"
#include "API007Data.h"
#if (API007DATA_BULK_DATA_FEATURE == ENABLED)
    #include "API007App.h"
#endif
#include "API009System.h"
#include "API010PollVar.h"
#include "API011AppCtrl.h"
#if (API012EXP_FEATURE == ENABLED)
#include "API012Exp.h"
#endif
#if ((API013_CLIENT_FEATURE == ENABLED) || (API013_SERVER_FEATURE == ENABLED))
#include "API013RemoteFunction.h"
#endif
#if ((API018_CLIENT_FEATURE == ENABLED) || (API018_SERVER_FEATURE == ENABLED))
    #include "API018Diagnostic.h"
#endif
#include "API019ProductInfo.h"
#include "Atod.h"
#include "Debug.h"
#include "DefaultPin.h"
#include "Log.h"
#include "NVData.h"
#include "RTCDriver.h"
#include "SFUpdater.h"
#include "SRPowerMgr.h"
#if (STATUS_LED_FEATURE == ENABLED)
    #include "StatusLed.h"
#endif

#define TX_ENABLE_EVENT_TRACE_SIZE      0


#if (SR_UNUSED_PINS_CONFIG_FEATURE == ENABLED)
    #include "SRUnusedPinsConfig.h"
#endif //(SR_UNUSED_PINS_CONFIG_FEATURE == ENABLED)

//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================

//  --- Compiler Directives

//  --- Macro Definitions
#define SRMAIN__BEFORE_1MS()
#define SRMAIN__AFTER_1MS()


//  --- Variables

static const FUNCTION_TYPE SRMainGeneralInitialization_Tasks[] =
{
	DishAuxVar__InitializeFlowMeterPulsesPresent,
	#if (SYSTEM_ACU_HANDLING == ENABLED)
		LowPower__Initialize,
	#endif
    Atod__EngineInitialize,
	RtcDriver__Initialize,
	#if ((SYSTEM_ACU_HANDLING == ENABLED) || (SYSTEM_HMI_HANDLING == ENABLED))
		Status__Initialize,
	#endif
    #if (SYSTEM_ACU_HANDLING == ENABLED)
        BrownOut__Initialize,
    #endif
	#if(STATUS_LED_FEATURE == ENABLED)
    	StatusLed__Initialize,
	#endif
	#if (SR_UNUSED_PINS_CONFIG_FEATURE == ENABLED)
		SRUnusedPinsConfig__Initialize,
	#endif

	Mci__Initialize,
	MciZeroCross__Initialize,
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

static const FUNCTION_TYPE SRMainNormalMode1ms_Tasks[] =
{
	#if ((SYSTEM_ACU_HANDLING == ENABLED) || (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED))
		Hbl__Handler1ms,
	#if (MCL_DEBUG_MODE == ENABLED)
		Mci__1msHandler,
	#endif
	#endif

	API010PollVar__FastHandler,

    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

#define	MAIN__WATCHDOG_CHECK()  ((SRTaskMonitor__IsError() == FALSE) && \
                                 (SRTaskSequence__IsError() == FALSE) )

//=================================================================================
//=================================================================================
//=================================================================================
// Main Thread - Called Every 10ms
//=================================================================================

// Enable Main thread task monitor feature - default is DISABLED
// When this feature is enabled, SRTASKMONITOR_MAIN_TASK must be an element in SRTASKMONITOR_TASKLIST_ENUM

#define MAIN_THREAD_MONITOR			ENABLED

/** @brief      List of tasks executed before entering the main loop.
 *  @details    Add pointers to routines that should be called during initialization.  These
 *              routines are intended to initialize or begin the initialization process for all the
 *              modules in the application.
 */
static const FUNCTION_TYPE SRMainNormalInitialization_Tasks[] =
{
	#if (LOW_POWER_FEATURE == ENABLED)
		SRPowerMgr__Initialize,
	#endif
	#if ((SYSTEM_ACU_HANDLING == ENABLED) || (SYSTEM_HMI_HANDLING == ENABLED))
		Status__Config,
	#endif
    NVData__Initialize,
	#if ((SYSTEM_ACU_HANDLING == ENABLED) || (SYSTEM_HMI_HANDLING == ENABLED))
		Regulations__Initialize,
	#endif
	#if ((SYSTEM_ACU_HANDLING == ENABLED) || (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED))
		Hbl__Initialize,
	#endif
	#if (SYSTEM_ACU_HANDLING == ENABLED)
		DishAuxVar__InitializePulseRef,			// NOTE: DishAuxVar__InitializePulseRef must be called before Hbl__AsynchHandler() for the Water Counter to work properly after a warm reset
	#endif
	DefaultPin__Initialize,
	// Debug__Initialize,
	NULL_TASK
};

static const FUNCTION_TYPE SRMainLowPowerInitialization_Tasks[] =
{
    #if (LOW_POWER_FEATURE == ENABLED)
		SRPowerMgr__Initialize,
    #endif
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

/**
 * Tasks that run in the Main thread if NOT in Low Power
 */
static const FUNCTION_TYPE SRMain_Non_Low_Power_Tasks[] =
{
	#if(STATUS_LED_FEATURE == ENABLED)
		StatusLed__Execute,
    #endif
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

/**
 * Tasks that run in the Main thread in both Low Power and non Low Power modes
 */
static const FUNCTION_TYPE SRMainRunning_Tasks[] =
{
    #if (LOW_POWER_FEATURE == ENABLED)
        SRPowerMgr__Handler,
    #endif
    #if (SYSTEM_ACU_HANDLING == ENABLED)
     // LowPower__Handler,
    #endif
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

//=================================================================================
//=================================================================================
//=================================================================================
// AppMgr Thread - Called Every 100ms
//=================================================================================

// Enable AppMgr thread task monitor feature - default is DISABLED
// When this feature is enabled, SRTASKMONITOR_MAIN_TASK must be an element in SRTASKMONITOR_TASKLIST_ENUM

#define MAIN_APPMGR_THREAD_MONITOR			ENABLED
#define SRMAIN_CUSTOMIZE_MAIN_APPMGR_THREAD_STACK_SIZE 200
/** @brief      List of tasks executed before entering the AppMgr thread loop.
 *  @details    Add pointers to routines that should be called during initialization.  These
 *              routines are intended to initialize or begin the initialization process for all the
 *              modules in this thread.
 */
static const FUNCTION_TYPE SRMain_AppMgr_Initialize[] =
{
	#if (SYSTEM_ACU_HANDLING == ENABLED)
		Fault__Initialize,
		HeaterShadowing__StatusInitialize,
		StabilityControl__VariablesInitialize,
	#endif
	#if ((SYSTEM_ACU_HANDLING == ENABLED) || (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED))
		ApplianceMgr__Initialize,
	#endif
	#if (SYSTEM_ACU_HANDLING == ENABLED)
		Statistics__Initialize,
		StatisticsDish__Initialize,
		DishAuxVar__Initialize,
		TestMgr__Initialize,
	#endif
    #if (API007DATA_BULK_DATA_FEATURE == ENABLED)
		API007App__Initialize,
    #endif
		APIMotorControl__Initialize,
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

/** @brief      List of tasks executed into AppMgr thread loop.
 *  @details    Add pointers to routines that should be called in normal execution of this thread.
 */
static const FUNCTION_TYPE SRMain_AppMgr_Handler[] =
{
	#if (SYSTEM_ACU_HANDLING == ENABLED)
		DishAuxVar__Handler,
	#endif
		APIMotorControl__SlowHandler,
	#if ((SYSTEM_ACU_HANDLING == ENABLED) || (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED))
		ApplianceMgr__Handler,
	#endif
	#if (SYSTEM_ACU_HANDLING == ENABLED)
		Statistics__Background,
		Fault__Background,
		TestMgr__Handler,
	#endif
	#if (API007DATA_BULK_DATA_FEATURE == ENABLED)
		API007App__Handler,
	#endif
	DefaultPin__Handler,
		
	#if (SR_UNUSED_PINS_CONFIG_FEATURE == ENABLED)
		SRUnusedPinsConfig__Monitor,
	#endif

	//ServiceEncoderSequence__Handler,
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};


//=================================================================================
//=================================================================================
//=================================================================================
// Asynch Thread - Called Every 5ms
//=================================================================================

// Enable Analog thread task monitor feature - default is DISABLED
// When this feature is enabled, SRTASKMONITOR_ASYNCH_TASK must be an element in SRTASKMONITOR_TASKLIST_ENUM

#define MAIN_ASYNCH_THREAD_MONITOR			ENABLED

/** @brief      List of tasks executed before entering the Asynchronous thread loop.
 *  @details    Add pointers to routines that should be called during initialization.  These
 *              routines are intended to initialize or begin the initialization process for all the
 *              modules in this thread.
 */
static const FUNCTION_TYPE SRMain_Asynch_Initialize[] =
{
    #ifdef SEGGER_RTT_MESSAGES_ENABLED
    SeggerRTT__ConfigureUpBuffer0,
    #endif
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

/** @brief      List of tasks executed into Asynchronous thread loop.
 *  @details    Add pointers to routines that should be called in normal execution of this thread.
 */
static const FUNCTION_TYPE SRMain_Asynch_Handler[] =
{
    #if (SYSTEM_ACU_HANDLING == ENABLED)
        Hbl__AsynchHandler,
        BrownOut__Monitor,
    #endif
    #if (NVDATA_STORE_APPLIANCE_AND_STATISTICS_FEATURE == ENABLED)
        NVData__Handler,
    #endif
	#if (SR_UNUSED_PINS_CONFIG_FEATURE == ENABLED)
		SRUnusedPinsConfig__Handler5ms,
	#endif

	#if (MCL_DEBUG_MODE == ENABLED)
		Mci__5msHandler,
	#endif

    /*Debug__Handler,*/
    
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};


//=================================================================================
//=================================================================================
//=================================================================================
// Fast AppMgr Thread - Called Every 20ms
//=================================================================================

// Enable Fast AppMgr thread task monitor feature - default is DISABLED
// When this feature is enabled, SRTASKMONITOR_MAIN_TASK must be an element in SRTASKMONITOR_TASKLIST_ENUM

#define MAIN_FASTAPPMGR_THREAD_MONITOR			DISABLED

/** @brief      List of tasks executed before entering the AppMgr thread loop.
 *  @details    Add pointers to routines that should be called during initialization.  These
 *              routines are intended to initialize or begin the initialization process for all the
 *              modules in this thread.
 */
static const FUNCTION_TYPE SRMain_FastAppMgr_Initialize[] =
{
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

/** @brief      List of tasks executed into AppMgr thread loop.
 *  @details    Add pointers to routines that should be called in normal execution of this thread.
 */
static const FUNCTION_TYPE SRMain_FastAppMgr_Handler[] =
{
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

FUNCTION_TYPE Every_Interrupt_Tasks[] =
{
	//-------------------------------------------------------------------------
	// Mandatory NULL_TASK must be at the end of the list
	NULL_TASK
};


//=================================================================================
//=================================================================================
//=================================================================================
// Programming Thread - Called Every 10ms
//=================================================================================

// Enable Analog thread task monitor feature - default is DISABLED
// When this feature is enabled, SRTASKMONITOR_PROGRAMMING_TASK must be an element in SRTASKMONITOR_TASKLIST_ENUM

#define MAIN_PROGRAMMING_THREAD_MONITOR			DISABLED

/** @brief      List of tasks executed before entering the Programming thread loop.
 *  @details    Add pointers to routines that should be called during initialization.  These
 *              routines are intended to initialize or begin the initialization process for all the
 *              modules in this thread.
 */
static const FUNCTION_TYPE SRMain_Programming_Initialize[] =
{
    SFUpdater__Initialize,
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

/** @brief      List of tasks executed into Programming thread loop.
 *  @details    Add pointers to routines that should be called in normal execution of this thread.
 */
static const FUNCTION_TYPE SRMain_Programming_Handler[] =
{
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};


//=================================================================================
//=================================================================================
//=================================================================================
// Communication Thread - Called Every 5ms
//=================================================================================

// Enable Analog thread task monitor feature - default is DISABLED
// When this feature is enabled, SRTASKMONITOR_COMMUNICATION_TASK must be an element in SRTASKMONITOR_TASKLIST_ENUM

#define MAIN_COMMUNICATION_THREAD_MONITOR			ENABLED

static const FUNCTION_TYPE SRMain_Communication_Initialize[] =
{
    API009System__Initialize,
    API003Disc__Initialize,
    API005LowLevel__Initialize,
    API007Data__Initialize,
    API010PollVar__Initialize,
    API011AppCtrl__Initialize,
	#if (API012EXP_FEATURE == ENABLED)
        API012Exp__Initialize,
    #endif
	#if ((API013_CLIENT_FEATURE == ENABLED) || (API013_SERVER_FEATURE == ENABLED))
        API013RemoteFunction__Initialize,
    #endif
    #if ((API018_CLIENT_FEATURE == ENABLED) || (API018_SERVER_FEATURE == ENABLED))
        API018Diagnostic__Initialize,
    #endif
    API019ProductInfo__Initialize,
    #if (SYSTEM_ACU_HANDLING == ENABLED)
        Vwi__Initialize,
    #endif

    #if ( CONNECTIVITY_FEATURE == ENABLED )
        DMHandler__Initialize,
        NetworkObjectsDataModel__Initialize,
        Relational__Initialize,
    #endif

    #if (SYSTEM_ACU_HANDLING == ENABLED)
        ApplianceMgr__InitializeStable,
    #endif
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

static const FUNCTION_TYPE SRMain_Communication_EveryRun[] =
{
    API011AppCtrl__Handler,
    API005LowLevel__Handler,
	API007BulkDataMgr__Handler,
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

static const FUNCTION_TYPE SRMain_Communication_Run1[] =
{
    API010PollVar__SlowHandler,
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

static const FUNCTION_TYPE SRMain_Communication_Run2[] =
{
    API009System__Handler25ms,
	#if (API012EXP_FEATURE == ENABLED)
       API012Exp__Handler25ms,
   #endif
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

static const FUNCTION_TYPE SRMain_Communication_Run3[] =
{
    API019ProductInfo__Handler25ms,
    #if ((API018_CLIENT_FEATURE == ENABLED) || (API018_SERVER_FEATURE == ENABLED))
        API018Diagnostic__Handler25ms,
    #endif
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

static const FUNCTION_TYPE SRMain_Communication_Run4[] =
{
    #if (SYSTEM_ACU_HANDLING == ENABLED)

    #endif
	Mci__SlowHandler,
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

static const FUNCTION_TYPE SRMain_Communication_Run5[] =
{
    #if (SYSTEM_ACU_HANDLING == ENABLED)
        Vwi__Handler,
    #endif
    #if (LOG_MODULE_ENABLE == ENABLED)
        Log__Background,
    #endif

    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};



//=================================================================================
//=================================================================================
//=================================================================================
// Fast Handler Interruption - 200us
//=================================================================================
static const FUNCTION_TYPE SRMainNormalModeFastInterrupt_Tasks[] =
{
    Mci__MotorControlHandler,
	Hbl__FastHandler,
    //-------------------------------------------------------------------------
    // Mandatory NULL_TASK must be at the end of the list
    NULL_TASK
};

#endif  // SRMAIN_PRV_H

/**
 *  @file
 *
 *  @brief      Private parameters for ApplianceMgr module.
 *
 *  @copyright  Copyright 2015-2018 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef APPLIANCEMGR_PRV_H_
#define APPLIANCEMGR_PRV_H_

#if (SYSTEM_ACU_HANDLING == ENABLED)
    #include "AirlockMonitor.h"
    #include "AutostartFactoryCycleMonitor.h"
	#include "CartridgeDetectionMonitor.h"
    #include "CommunicationMonitor.h"
    #include "DCLoadCurrentMonitor.h"
    #include "DiverterLeakFaultMonitor.h"
    #include "DoorMonitor.h"
    #include "DoorOpeningSystemMonitor.h"
    #include "FaultStatusMonitor.h"
    #include "FillValveStuckOn.h"
    #include "FlowMeterFaultMonitor.h"
    #include "HeaterCalcificationDetectionMonitor.h"
    #include "HeatMonitor.h"
    #include "HotHighPotMonitor.h"
    #include "LITTMonitor.h"
    #include "MoisturePreventionMonitor.h"
    #include "OverfillMonitor.h"
    #include "OWIMonitor.h"
    #include "OWIVoltageRegulatedOutput.h"
	#include "PilotRelayFaultMonitor.h"
    #include "PluggingMonitor.h"
    #include "PORTimeMonitor.h"
    #include "RinseAidMonitor.h"
    #include "SaltSensingMonitor.h"
    #include "SupervisorExtMonitor.h"
    #include "ThermistorFaultMonitor.h"
    #include "ZeroCrossMonitor.h"
    #include "TriacCurrentMonitor.h"
	#include "TriacVoltageMonitor.h"
    #include "WashHeaterFaultMonitor.h"
    #include "DiverterFaultMonitor.h"
    #include "WashMotorFaultMonitor.h"
    #include "StabilityControlLoopMonitor.h"
    #include "TriacShortedDrainFaultMonitor.h"
	#include "VoltageRangeMonitor.h"
#endif

//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
//! Macro to initialize all monitors.
#if (SYSTEM_ACU_HANDLING == ENABLED)
// INIT_MONITORS for Main ACU:
#define INIT_MONITORS()                     		\
    do                                      		\
    {            									\
    	PORTimeMonitor__Initialize();	 			\
        DoorMonitor__Initialize(); 					\
        LITTMonitor__Initialize(); 					\
        DCLoadCurrentMonitor__Initialize();         \
        SupervisorExtMonitor__Initialize();	 		\
        HotHighPotMonitor__Initialize(); 			\
        RinseAidMonitor__Initialize(); 				\
        OverfillMonitor__Initialize(); 				\
        OWIMonitor__Initialize(); 					\
        ZeroCrossMonitor__Initialize(); 			\
        ThermistorFaultMonitor__Initialize(); 		\
        MoisturePreventionMonitor__Initialize(); 	\
        FillValveStuckOn__Initialize(); 			\
        FlowMeterFaultMonitor__Initialize(); 		\
        TriacVoltageMonitor__Initialize();          \
        TriacCurrentMonitor__Initialize();          \
        WashHeaterFaultMonitor__Initialize(); 		\
        PilotRelayFaultMonitor__Initialize(); 		\
        FaultStatusMonitor__Initialize(); 			\
        DiverterFaultMonitor__Initialize(); 		\
        AutostartFactoryCycle__Initialize(); 		\
        AirlockMonitor__Initialize(); 				\
        PluggingMonitor__Initialize();				\
        HeaterCalcificationDetectionMonitor__Initialize(); \
        WashMotorFaultMonitor__Initialize();		\
        CommunicationMonitor__Initialize();			\
        DoorOpeningSystemMonitor__Initialize();		\
        SaltSensingMonitor__Initialize(); 			\
		HeatMonitor__Initialize(); 					\
		TriacShortedDrainFaultMonitor__Initialize();\
		DiverterLeakFaultMonitor__Initialize();     \
		StabilityControlLoopMonitor__Initialize();	\
		VoltageRangeMonitor__Initialize(); 			\
		OWIVoltageRegulatedOutput__Initialize();	\
		CartridgeDetectionMonitor__Initialize();	\
    } while (0)
#else
// INIT_MONITORS for Expansion ACU:
#define INIT_MONITORS()                             \
    do                                              \
    {                                               \
    } while (0)
#endif

//---------------------------------------------------------------------------------------------------------------------
//! Macro to run all monitors before the supervisor and interpreter run, regardless if the system is stable or not.
#if (SYSTEM_ACU_HANDLING == ENABLED)
// CALL_PRE_MONITORS() for Main ACU:
#define CALL_PRE_MONITORS()                 \
    do                                      \
    {                                       \
    	PORTimeMonitor__Execution(); 		\
        DoorMonitor__Execution(); 			\
        LITTMonitor__Execution(); 			\
        DCLoadCurrentMonitor__Execution();  \
        CommunicationMonitor__Execute(); 	\
        SaltSensingMonitor__Execution(); 	\
        VoltageRangeMonitor__Execution(); 	\
    } while (0)
#else
// CALL_PRE_MONITORS() for Expansion ACU:
#define CALL_PRE_MONITORS()                         \
    do                                              \
    {                                               \
    } while (0)
#endif


//---------------------------------------------------------------------------------------------------------------------
//! Macro to run all monitors before the supervisor and interpreter run, only when the system is stable.
#define CALL_PRE_MONITORS_STABLE()                 \
    do                                      \
    {                                       \
    } while (0)

//---------------------------------------------------------------------------------------------------------------------
//! Macro to run all monitors after the supervisor and interpreter run, regardless if the system is stable or not.
#if (SYSTEM_ACU_HANDLING == ENABLED)
// CALL_POST_MONITORS() for Main ACU:
#define CALL_POST_MONITORS() \
    do \
    { \
    	SupervisorExtMonitor__Execution(); 			\
        HotHighPotMonitor__Execution(); 			\
        RinseAidMonitor__Execution(); 				\
        OverfillMonitor__Execution(); 				\
        OWIMonitor__Execution(); 					\
        ZeroCrossMonitor__Execution(); 				\
        ThermistorFaultMonitor__Execution(); 		\
        MoisturePreventionMonitor__Execution(); 	\
        FillValveStuckOn__Execution(); 				\
        FlowMeterFaultMonitor__Execution(); 		\
        TriacVoltageMonitor__Execution();                   \
        TriacCurrentMonitor__Execution();           \
        WashHeaterFaultMonitor__Execution(); 		\
        PilotRelayFaultMonitor__Execution(); 		\
        DiverterFaultMonitor__Execution(); 			\
        AutostartFactoryCycle__Execution(); 		\
        AirlockMonitor__Execution();				\
        PluggingMonitor__Execution();				\
        HeaterCalcificationDetectionMonitor__Execution(); \
        WashMotorFaultMonitor__Execution();			\
        DoorOpeningSystemMonitor__Execution();		\
        HeatMonitor__Execution(); 					\
        TriacShortedDrainFaultMonitor__Execution();	\
        DiverterLeakFaultMonitor__Execution();\
        StabilityControlLoopMonitor__Execution();	\
        CartridgeDetectionMonitor__Execution();     \
        FaultStatusMonitor__Execution(); /*! NOTE: call FaultStatusMonitor__Execution() after all the specific fault monitors.*/ \
    } while (0)
#else
// CALL_POST_MONITORS() for Expansion ACU:
#define CALL_POST_MONITORS()                        \
    do                                              \
    {                                               \
    } while (0)
#endif

//---------------------------------------------------------------------------------------------------------------------
//! Macro to run all monitors after the supervisor and interpreter run, only when the system is stable.
#define CALL_POST_MONITORS_STABLE() 				\
    do 												\
    { 												\
		OWIVoltageRegulatedOutput__Execution();		\
    } while (0)

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Set platform/project specific power up stabilization time.
 *
 *  @details    Define the number of seconds to wait for the system to stabilize before running cycles.
 *              Enter the number of seconds with a resolution of 0.1 seconds.
 *              Allowable values are 0.0 - POWER_UP_STABILIZATION_TIME_MAX.
 *              Set to 0.0 to disable time delay. Typical value should be around 2.0 seconds.
 *              If not defined POWER_UP_STABILIZATION_TIME_DEFAULT value is used.
 *
 *  @retval     Number of seconds (resolution of 0.1 seconds) to wait for inputs to settle before running cycles.
 */
#define POWER_UP_STABILIZATION_TIME (1.0)

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Set platform/project specific power up stabilization guard.
 *
 *  @details    Define the application guard to determine system input stability before running cycles.
 *              Set to TRUE to disable.
 *              Do not set to FALSE.
 *              Setting to FALSE causes system to wait POWER POWER_UP_STABILIZATION_TIME_MAX before running cycles.
 *
 *  @retval     TRUE: System inputs are stable and ready for use by cycles.
 *  @retval     FALSE: System inputs are still settling and being monitored for readiness.
 */
#define POWER_UP_STABILIZATION_GUARD (TRUE)

#endif // APPLIANCEMGR_PRV_H_

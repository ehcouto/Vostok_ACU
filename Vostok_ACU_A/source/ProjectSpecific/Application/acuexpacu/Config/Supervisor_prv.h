/**
 *  @file
 *
 *  @brief      Private parameters for the Supervisor module.
 *
 *  @copyright  Copyright 2015-2016 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SUPERVISOR_PRV_H_
#define SUPERVISOR_PRV_H_

#include "AlternateCtrl.h"
#include "AutostartFactoryCycleMonitor.h"
#include "CartridgeDetectionMonitor.h"
#include "ClassAFill.h"
#include "CycleResume.h"
#include "DishAuxvar.h"
#include "DoorMonitor.h"
#include "DoorOpeningSystemMonitor.h"
#include "DiverterLeakFaultMonitor.h"
#include "Fill.h"
#include "FillValveStuckON.h"
#include "Hbl.h"
#include "Heat.h"
#include "HeaterCalcificationDetectionMonitor.h"
#include "HeaterShadowing.h"
#include "MoisturePreventionMonitor.h"
#include "MoisturePrevention.h"
#include "NormalFill.h"
#include "OwiVroRead.h"
#include "OWIMonitor.h"
#include "OWIOneShot.h"
#include "OWIVoltageRegulatedOutput.h"
#include "PilotRelayFaultMonitor.h"
#include "RinseAidMonitor.h"
#include "Sanitization.h"
#include "SensedDrainProcess.h"
#include "StabilityControl.h"
#include "StatisticsDish.h"
#include "TankFill.h"
#include "TriacCurrentMonitor.h"
#include "ThermistorFaultMonitor.h"
#include "TimeElapse.h"
#include "WashHeaterFaultMonitor.h"
#include "DCLoadCurrentMonitor.h"


//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Macro to decide if compartment should auto-start cycle 1.
 *
 *  @param      uint8 compartment = The compartment checking auto start status.
 *
 *  @retval     TRUE: Start compartment in cycle 1 phase 0 and Supervisor in RUN state.
 *  @retval     FALSE: Start compartment in cycle 0 phase 0 and Supervisor in PROGRAMMING state with Selector OFF.
 */
#define AUTO_START(compartment) (FALSE)

//---------------------------------------------------------------------------------------------------------------------
//! Macro to inform Supervisor if delay regulation should be checked.
#define CYCLE_DELAY_FEATURE (ENABLED)

//---------------------------------------------------------------------------------------------------------------------
//! Macro to inform Supervisor if PROGRAMMING, DELAY and PAUSE NOT_OK_TO_START SYSTEM STATES are enabled.
#define NOT_OK_TO_START_SYSTEM_STATES (ENABLED)

//---------------------------------------------------------------------------------------------------------------------
/**
 * Set as ENABLED to stay in the END state after running a Test Manager Cycle (EOL or Service).
 * Set as DISABLED to automatically return to the programming state after running a Test Manager Cycle.
 * If ENABLED the HMI is responsible to set the START regulation to FS_RESET_CONDITION to go back to PROGRAMMING state.
 * Default value is DISABLED if not defined.
 */
#define TEST_CYCLE_STAYS_IN_END_UNTIL_RESET     (ENABLED)

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Macro to  control cycle cold reset behavior when GESE selection is CYCLE_COLD_RESET_BEHAVIOR_SELECT.
 *              Cross category method to check platform specific items (e.g. door/lock state ...)
 *
 *  @details    Make sure you do not get any compiler warnings!
 *
 *  @param      uint8 compartment = The compartment checking for cycle cold reset behavior.
 *  @param      CYCLE_STATE_TYPE cycle_state = State of current cycle to aid in decision of cold reset behavior.
 *
 *  @retval     CYCLE_COLD_RESET_BEHAVIOR_TYPE - Enumerated Value.
 *  @retval     Non Enumeration values default to CYCLE_COLD_RESET_BEHAVIOR_CANCEL. (Truncates to 8 bits)
 */
#define CYCLE_COLD_RESET_SELECT_BEHAVIOR(compartment, cycle_state)   CYCLE_COLD_RESET_BEHAVIOR_CANCEL

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Macro to control Cycle Cold Reset Behavior for Special Cycles (DELAY, PAUSE and END).
 *
 *              If ENABLED, special cycle CCRB will behave according to CCRB for SElECTOR.
 *              If DISABLED special cycle CCRB will be CYCLE_COLD_RESET_BEHAVIOR_RESUME
 *
 *               Default value is DISABLED if not defined.
 */
#define SPECIAL_CYCLE_CCRB_FROM_SELECTOR (ENABLED)

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Macro to inform Supervisor if it is OK to start a cycle.
 *              Cross category method to check platform specific items such as door / lock state.
 *
 *  @param      uint8 compartment = The compartment checking if it is ok to start cycle.
 *
 *  @retval     TRUE: OK to start the cycle.
 *  @retval     FALSE: Not OK to start the cycle.
 */
#define OK_TO_START_CYCLE(compartment) \
	(((CartridgeDetectionMonitor__GetCartridgeDoorStatus() == CARTRIDGE_SWITCH_STATUS_CLOSED) && (DoorMonitor__GetDoorStatus() == DOOR_STATUS_CLOSE)) ? (TRUE) : (FALSE))
//#define OK_TO_START_CYCLE(compartment)           (TRUE)

//---------------------------------------------------------------------------------------------------------------------
//! Macro to inform application modules of entry into supervisor PROGRAMMING state.
//! uint8 compartment = The compartment being supervised.
#define PROGRAMMING_ENTRY(compartment) \
    do \
    { \
        Sanitization__SetResultVariable(TASK_SANITIZATION_NOT_EXECUTED);\
        FillValveStuckON__ExitFault();\
        TriacCurrentMonitor__ExitFault();\
        Fill__ClearH2OAlarm(); \
        NormalFill__ClearH2OAlarm(); \
        OWIMonitor__CheckFaultCondition();\
        MoisturePrevention__Terminate(compartment);\
        DCLoadCurrentMonitor__ExitFault();\
    } while (0)

//---------------------------------------------------------------------------------------------------------------------
//! Macro to inform application modules of entry into supervisor RUN state.
//! uint8 compartment = The compartment being supervised.
#define RUN_ENTRY(compartment) \
    do \
    { \
		CycleResume__CheckStepAdvance();\
    } while (0)

//---------------------------------------------------------------------------------------------------------------------
//! Macro to inform application modules of exit from supervisor RUN state.
//! uint8 compartment = The compartment being supervised.
#define RUN_EXIT(compartment) \
    do \
    { \
    	DoorOpeningSystemMonitor_CheckIfPullNeeded();\
    } while (0)

//! Macro to inform application modules of entry into supervisor CANCELLING state.
//! uint8 compartment = The compartment being supervised.
#define CANCELLING_ENTRY(compartment) \
    do\
    {\
    } while (0)

//---------------------------------------------------------------------------------------------------------------------
//! Macro to inform application modules of exit from supervisor CANCELLING state.
//! uint8 compartment = The compartment being supervised.
#define CANCELLING_EXIT(compartment) \
    do\
    {\
    } while (0)

//---------------------------------------------------------------------------------------------------------------------
//! Macro to inform application modules of entry into supervisor DELAY state.
//! uint8 compartment = The compartment being supervised.
#define DELAY_ENTRY(compartment) \
    do \
    { \
		Sanitization__SetResultVariable(TASK_SANITIZATION_NOT_EXECUTED);\
		RinseAidMonitor__CheckEvent();\
		MoisturePrevention__Terminate(compartment);\
		StatisticsDish__UpdateDelayRecord(); \
    } while (0)

//---------------------------------------------------------------------------------------------------------------------
//! Macro to inform application modules of entry into supervisor DELAY PAUSE state.
//! uint8 compartment = The compartment being supervised.
#define DELAY_PAUSE_ENTRY(compartment) \
    do \
    { \
		TimeElapse__Pause();\
		MoisturePrevention__Pause(compartment);\
    } while (0)

//---------------------------------------------------------------------------------------------------------------------
//! Macro to inform application modules of exit from supervisor Delay PAUSE state.
//! uint8 compartment = The compartment being supervised.
#define DELAY_PAUSE_EXIT(compartment) \
    do \
    { \
    	MoisturePrevention__Resume(compartment);\
    } while (0)

//---------------------------------------------------------------------------------------------------------------------
//! Macro to inform application modules of entry into supervisor END state.
//! uint8 compartment = The compartment being supervised.
#define END_ENTRY(compartment) \
    do \
    { \
        OWIMonitor__CycleEndEvent();\
        MoisturePrevention__Terminate(compartment);\
        Fill__ClearH2OAlarm(); \
        NormalFill__ClearH2OAlarm(); \
        AutostartFactoryCycle__CycleEndEvent();\
        DiverterLeakFaultMonitor__ResetFaultObserver();\
        HeaterCalcificationDetectionMonitor__CycleEnded();\
        OwiVroRead__CycleEndEvent(); \
        StabilityControl__ResetFlagsMask(SC_FLAGS_END_RESET_MASK); \
    } while (0)

//---------------------------------------------------------------------------------------------------------------------
//! Macro to inform application modules of entry into supervisor PAUSE state.
//! uint8 compartment = The compartment being supervised.
#define PAUSE_ENTRY(compartment) \
    do \
    { \
        TimeElapse__Pause();\
        MoisturePrevention__Pause(compartment);\
        OWIOneShot__PauseTimer();\
        ClassAFill__DisableFlowRateCalibration();\
        TankFill__DisableFlowRateCalibration();\
    } while (0)

//---------------------------------------------------------------------------------------------------------------------
//! Macro to inform application modules of exit from supervisor PAUSE state.
//! uint8 compartment = The compartment being supervised.
#define PAUSE_EXIT(compartment) \
    do \
    { \
        MoisturePrevention__Resume(compartment);\
        Fill__ClearH2OAlarm();\
        ClassAFill__SetH2OPauseExecuted();\
        TankFill__SetH2OPauseExecuted();\
    } while (0)

//---------------------------------------------------------------------------------------------------------------------
//! Macro to inform application modules of entry into supervisor FAULT state.
//! uint8 compartment = The compartment being supervised.
#define FAULT_ENTRY(compartment) \
    do \
    { \
		Sanitization__SetResultVariable(TASK_SANITIZATION_NOT_EXECUTED);\
        Fill__ClearH2OAlarm(); \
        NormalFill__ClearH2OAlarm(); \
        MoisturePrevention__Terminate(compartment);\
    } while (0)

//---------------------------------------------------------------------------------------------------------------------
//! Macro to inform application modules of supervisor transition from PAUSE to RUN state.
//! uint8 compartment = The compartment being supervised.
#define RESUME(compartment) \
    do \
    { \
    	TimeElapse__Resume();\
        OWIOneShot__ResumeTimer();\
    	CycleResume__StartSubcycle();\
        HeaterShadowing__ResumeCycle(); \
    } while (0)

//---------------------------------------------------------------------------------------------------------------------
//! Macro to inform application modules of supervisor transition from PROGRAMMING or DELAY to RUN state.
//! uint8 compartment = The compartment being supervised.
#define START(compartment) \
    do \
    { \
        MoisturePreventionMonitor__CycleStarted();\
        DoorMonitor__ResetStepAdvRequest();\
        OWIMonitor__CycleStarted();\
        NormalFill__ClearFillValveFailure();\
        ThermistorFaultMonitor__CycleStarted();\
        FillValveStuckON__ExitFault();\
        Heat__ResetHeatTimeoutFault();\
        TriacCurrentMonitor__ExitFault();\
        WashHeaterFaultMonitor__CycleStarted();\
        PilotRelayFaultMonitor__CycleStarted(); \
        AutostartFactoryCycle__CycleStartEvent(); \
        RinseAidMonitor__CheckEvent();\
        SensedDrain__SetDrainConfirmed(FALSE);\
        DiverterLeakFaultMonitor__ResetFaultObserver();\
        HeaterCalcificationDetectionMonitor__CycleStarted();\
        OwiVroRead__CycleStarted();\
        OWIVoltageRegulatedOutput__ResetSetpoint();\
        StabilityControl__ResetFlagsMask(SC_FLAGS_START_RESET_MASK); \
        DishAuxVar__ResetWaterCounterReference(); \
        DCLoadCurrentMonitor__ExitFault();\
    } while (0)

#endif // SUPERVISOR_PRV_H_

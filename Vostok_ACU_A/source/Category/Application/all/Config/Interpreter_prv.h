/**
 *  @file       
 *
 *  @brief      Private parameters for the Interpreter module.
 *
 *  @copyright  Copyright 2015-2019 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef INTERPRETER_PRV_H_
#define INTERPRETER_PRV_H_

//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "Process.h"

// -- Other Modules --

#include "DishAuxVar.h"
#include "DoorMonitor.h"
#include "Hbl.h"
#include "Heat.h"
#include "HeaterShadowing.h"
#include "AlternateCtrl.h"
#include "ClassAFill.h"
#include "Fill.h"
#include "NormalFill.h"
#include "SensedDrainProcess.h"
#include "OWI.h"
#include "OWIMonitor.h"
#include "OWIOneShot.h"
#include "OwiVroRead.h"
#include "PumpControl.h"
#include "Regenerate.h"
#include "RinseAidDispense.h"
#include "TankDrain.h"
#include "TankFill.h"

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================
//! Category specific OpCodes 6- 15
//! Common and required for all projects in a specific category (Only Platform SW Lead should modify).
#define PROJECT_SPECIFIC_PROCESSES \
    Heat__Process,                                  /* OpCode:  6 */ \
    AlternateCtrl__Process,                         /* OpCode:  7 */ \
    Fill__Process,                                  /* OpCode:  8 */ \
    SensedDrain__Process,                           /* OpCode:  9 */ \
    OWI__Process,                        			/* OpCode:  10 */ \
    PumpControl__Process,                           /* OpCode:  11 */ \
    Regenerate__Process,                            /* OpCode:  12 */ \
    RinseAidDispense__Process,                      /* OpCode:  13 */ \
    OwiVroRead__Process,                            /* OpCode:  14 */ \
    NULL_PROCESS                                    /* OpCode:  15 */

//! Category specific OpCodes 6- 15 Process initialize functions
//! Common and required for all projects in a specific category (Framework Owner should maintain).
//!
//! //! Function pointer table for process initialization functions.
//! These functions initialize module (not instance) data.
//!
//! NOTE: The primary method of initializing instance data is using
//! the Step_Initializing, Resume_From_Pause, or Resume_From_Reset flags
//! in the PROCESS_CONTROL_TYPE passed to the Process_Function.
//!
#define PROJECT_SPECIFIC_PROCESS_INITIALIZERS \
	Heat__Initialize,										/* OpCode:  6 */ \
	AlternateCtrl__Initialize,								/* OpCode:  7 */ \
	Fill__Initialize,										/* OpCode:  8 */ \
	SensedDrain__Initialize,								/* OpCode:  9 */ \
	OWI__Initialize,										/* OpCode:  10 */ \
	PumpControl__Initialize,								/* OpCode:  11 */ \
	Regenerate__Initialize,									/* OpCode:  12 */ \
    RinseAidDispense__Initialize,							/* OpCode:  13 */ \
    OwiVroRead__Initialize,									/* OpCode:  14 */ \
    NULL_PROCESS_INITIALZE									/* OpCode:  15 */

/**
 *  @brief      Macro to inform Interpreter if it is OK to start a sub-cycle.
 *              Cross category method to check platform specific items such as door/lock state.
 *
 *  @param      uint8 compartment = The compartment checking if it is ok to start sub-cycle.
 *  @param      uint8 sub_cycle = The ID of the sub cycle that is requested to start
 *  @param      uint8 state
 *
 *  @retval     TRUE: OK to start the sub-cycle.
 *  @retval     FALSE: Not OK to start the sub-cycle.
 */
#define OK_TO_START_SUB_CYCLE(compartment, sub_cycle, state) \
	((DoorMonitor__GetDoorStatus() == DOOR_STATUS_CLOSE) ? (TRUE) : (FALSE))

/**
 *  Macro to allow sub-cycles during any state.
 *  Default value is DISABLED. Define here as ENABLED if required.
 *  If ENABLED, additional conditional logic can be applied by the OK_TO_START_SUB_CYCLE macro
 */
#define SUB_CYCLE_ALLOWED_DURING_ALL_STATES  (DISABLED)

/**
 *  Macro to allow sub-cycles during EOL and SERVICE.
 *  Default value is DISABLED. Define here as ENABLED if required.
 */
#define SUB_CYCLE_ALLOWED_DURING_TEST  (ENABLED)


//! Macro to inform application modules of entry into interpreter Step.
//! uint8 compartment = The compartment in which the step is starting.
#define ITP_EVENT_STEP_STARTING(compartment) \
    do \
    { \
    	DishAuxVar__SetTimedFillRunning(FALSE); \
    	PumpControl__SetVSMDrainIsRunning(FALSE); \
    } while (0)

//! Macro to inform application modules of the start of a sub-cycle.
//! uint8 compartment = The compartment in which the sub-cycle is started.
//! uint8 subcycle_id = The sub-cycle that was started.
#define SUB_CYCLE_START(compartment, subcycle_id) \
    do \
    { \
    	OWIOneShot__CycleResumeStart(compartment,subcycle_id); \
    	ClassAFill__DisableFlowRateCalibration(); \
    	TankFill__DisableFlowRateCalibration(); \
    } while (0)

//! Macro to inform application modules of the end of a sub-cycle.
//! uint8 compartment = The compartment in which the sub-cycle was running.
//! uint8 subcycle_id = The sub-cycle that has finished or was cancelled.
//! SUB_CYCLE_END_TYPE subcycle_end_code = Reason sub-cycle ended.
#define SUB_CYCLE_END(compartment, subcycle_id, subcycle_end_code) \
    do \
    { \
        NormalFill__SetFillSubcycleExecuted(compartment, subcycle_id, subcycle_end_code); \
        ClassAFill__SetSubcycleExecuted(compartment, subcycle_id, subcycle_end_code); \
        TankDrain__SetSubcycleExecuted(compartment, subcycle_id, subcycle_end_code); \
        TankFill__SetSubcycleExecuted(compartment, subcycle_id, subcycle_end_code); \
    	OWIOneShot__CycleResumeEnd(compartment,subcycle_id); \
    	HeaterShadowing__CycleResumeEnd(compartment, subcycle_id); \
    } while (0)

#endif // INTERPRETER_PRV_H_

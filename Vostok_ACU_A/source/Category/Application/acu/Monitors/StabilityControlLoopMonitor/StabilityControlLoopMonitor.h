/*
 * @file		StabilityControlLoopMonitor.h
 * @brief       Public interface to the Stability Control Loop monitor module.
 *
 *  Created on: 19 apr 2021
 *      Author: QUINTM1
 */

#ifndef SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_STABILITYCONTROLLOOPMONITOR_STABILITYCONTROLLOOPMONITOR_H_
#define SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_STABILITYCONTROLLOOPMONITOR_STABILITYCONTROLLOOPMONITOR_H_

//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --

// -- Other Modules --

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//! Reactions triggered by the Stability Control Task.
typedef enum STABILITY_CONTROL_LOOP_OUTCOME_ENUM
{
	SCL_NO_REACTION_REQUESTED					= 0,
	SCL_PRIMARY_REACTION_REQUESTED,
	SCL_SECONDARY_REACTION_REQUESTED
} STABILITY_CONTROL_LOOP_OUTCOME_TYPE;


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void StabilityControlLoopMonitor__Initialize(void);
void StabilityControlLoopMonitor__Execution(void);
STABILITY_CONTROL_LOOP_OUTCOME_TYPE StabilityControlLoopMonitor__GetOutcome(void);
void StabilityControlLoopMonitor__ResetCounters(void);

#endif /* SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_STABILITYCONTROLLOOPMONITOR_STABILITYCONTROLLOOPMONITOR_H_ */

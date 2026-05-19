/**
 * @file
 * @brief       Public interface to the code generated HeaterCalcificationDetection module.
 *
 * @details     Refer to HeaterCalcificationDetectionMonitor.c source file for more detailed information.
 *
 * $Header: $
 *
 * @copyright  Copyright 2018-2019. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

#ifndef HEATERCALCIFICATIONDETECTIONMONITOR_H_
#define HEATERCALCIFICATIONDETECTIONMONITOR_H_

//  --- Include Files -------------------------------------------------------------------------------------------------
#include "SystemConfig.h"

// -- This Module --
#include "HeaterCalcificationDetectionMaster.h"
#include "HeaterCalcificationDetection_defs.h"

// -- Other Modules --


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================

extern HCD_BITMAP_TYPE HCD_Bitmap;
extern uint16 HCD_Heater_Clicks;
extern uint8 HCD_Calcification_Level;
extern HCD_HMI_DATA_TYPE HCD_HMI_Data;
extern HCD_MODELDATA_TYPE HCD_ModelData;

// -- Public Constant Declarations --

// -- Public Type Declarations --


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================

void HeaterCalcificationDetectionMonitor__Initialize(void);
void HeaterCalcificationDetectionMonitor__Execution(void);
void HeaterCalcificationDetectionMonitor__Start(void);
void HeaterCalcificationDetectionMonitor__Stop(void);
void HeaterCalcificationDetectionMonitor__Reset(void);
void HeaterCalcificationDetectionMonitor__CycleStarted(void);
void HeaterCalcificationDetectionMonitor__CycleEnded(void);
BOOL_TYPE HeaterCalcificationDetectionMonitor__GetLockoutStatus(void);
uint16 HeaterCalcificationDetectionMonitor__GetHeaterClicks(void);
void HeaterCalcificationDetectionMonitor__SetHeaterClicks(uint16 heaterClicks);


#endif      /* _HEATERCALCIFICATIONDETECTIONMONITOR_H_ */

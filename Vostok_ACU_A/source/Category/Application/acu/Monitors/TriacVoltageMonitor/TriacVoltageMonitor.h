/*
 * TriacVoltageMonitor.h
 *
 *  Created on: 5 maj 2021
 *      Author: WAZNYP
 */

#ifndef SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_TRIACVOLTAGEMONITOR_TRIACVOLTAGEMONITOR_H_
#define SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_TRIACVOLTAGEMONITOR_TRIACVOLTAGEMONITOR_H_

//  --- Include Files -------------------------------------------------------------------------------------------------
#include "TriacVoltageMonitor_prv.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
extern BOOL_TYPE Power_Loss_Occurring;

#include "TriacCurrentMonitor.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void TriacVoltageMonitor__Initialize(void);
void TriacVoltageMonitor__Execution(void);
TRIAC_LOADS_VOLTAGE_SENSOR_TYPE TriacVoltageMonitor__GetLoad_Status(void);
#if (TRIAC_V_DIAG_AC_LINE_FEATURE != ENABLED)
void TriacVoltageMonitor__FastHandler1ms(void);
#endif
void TriacVoltageMonitor__InitISRVariables(void);

#endif /* SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_TRIACVOLTAGEMONITOR_TRIACVOLTAGEMONITOR_H_ */

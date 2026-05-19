/*
 * ThermistorFaultMonitor.h
 *
 *  Created on: Apr 23, 2018
 *      Author: DHAMAAD
 */

#ifndef SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_THERMISTORFAULTMONITOR_THERMISTORFAULTMONITOR_H_
#define SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_THERMISTORFAULTMONITOR_THERMISTORFAULTMONITOR_H_


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void ThermistorFaultMonitor__Initialize(void);
void ThermistorFaultMonitor__Execution(void);
void ThermistorFaultMonitor__CycleStarted(void);
sint16 ThermistorFaultMonitor__GetTemperature(void);
sint16 ThermistorFaultMonitor__GetShortedThreshold(void);


#endif /* SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_THERMISTORFAULTMONITOR_THERMISTORFAULTMONITOR_H_ */

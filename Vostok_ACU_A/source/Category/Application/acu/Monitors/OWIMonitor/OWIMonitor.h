/*
 * OWIMonitor.h
 *
 *  Created on: 15.12.2017
 *      Author: WAZNYP
 */

#ifndef SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_OWIMONITOR_OWIMONITOR_H_
#define SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_OWIMONITOR_OWIMONITOR_H_

#include "Log.h"
#include "SettingFile.h"

void OWIMonitor__Initialize(void);
void OWIMonitor__Execution(void);
void OWIMonitor__CycleStarted(void);
void OWIMonitor__CycleEndEvent(void);
void OWIMonitor__CheckFaultCondition(void);

#endif /* SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_OWIMONITOR_OWIMONITOR_H_ */

/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef HEATEROPENMONITOR_H_
#define HEATEROPENMONITOR_H_

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- Other Modules --
#include "WashHeaterFaultMonitor.h"

#if (HEATER_OPEN_UNIQUELY_DETECTED == ENABLED)
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//! Fault condition to detect heater open when heater is off
#define HEATER_OPEN_FAULT_CONDITION_HEATER_OFF	(WashHeaterFaultMonitor__GetHeaterConsistency() == COMBINE_NIBBLES(HBL_CONSISTENCY_TABLE_OFF, HBL_CONSISTENCY_DISCONNECTED))


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void HeaterOpenMonitor__Initialize(void);
void HeaterOpenMonitor__Execution(void);
void HeaterOpenMonitor__ExitFault(void);
#endif // (HEATER_OPEN_UNIQUELY_DETECTED == ENABLED)

#endif // HEATEROPENMONITOR_H_



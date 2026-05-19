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
#ifndef HEATEROPENANDLINERELAYCLOSEDMONITOR_H_
#define HEATEROPENANDLINERELAYCLOSEDMONITOR_H_

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- Other Modules --
#include "WashHeaterFaultMonitor.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
#if (HEATER_OPEN_PLUS_L_RELAY_CLOSED_DETECTION == ENABLED)
void HeaterOpenAndLineRelayClosedMonitor__Initialize(void);
void HeaterOpenAndLineRelayClosedMonitor__Execution(void);
void HeaterOpenAndLineRelayClosedMonitor__ExitFault(void);
#endif // (HEATER_OPEN_PLUS_L_RELAY_CLOSED_DETECTION == ENABLED)

#endif // HEATEROPENANDLINERELAYCLOSEDMONITOR_H_



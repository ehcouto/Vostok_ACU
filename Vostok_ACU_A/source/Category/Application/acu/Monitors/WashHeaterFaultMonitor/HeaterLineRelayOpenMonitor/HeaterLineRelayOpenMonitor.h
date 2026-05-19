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
#ifndef HEATERLINERELAYOPENMONITOR_H_
#define HEATERLINERELAYOPENMONITOR_H_

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "HeaterLineRelayOpenMonitor_prm.h"


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void HeaterLineRelayOpenMonitor__Initialize(void);
void HeaterLineRelayOpenMonitor__Execution(void);
void HeaterLineRelayOpenMonitor__ExitFault(void);

#endif // HEATERLINERELAYOPENMONITOR_H_



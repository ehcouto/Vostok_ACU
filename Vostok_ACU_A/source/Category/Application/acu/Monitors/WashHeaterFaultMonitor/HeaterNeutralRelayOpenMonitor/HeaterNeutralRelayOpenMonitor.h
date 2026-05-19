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
#ifndef HEATERNEUTRALRELAYOPENMONITOR_H_
#define HEATERNEUTRALRELAYOPENMONITOR_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void HeaterNeutralRelayOpenMonitor__Initialize(void);
void HeaterNeutralRelayOpenMonitor__Execution(void);
void HeaterNeutralRelayOpenMonitor__ExitFault(void);
#if (HEATER_OPEN_PLUS_N_RELAY_OPEN_DETECTION == ENABLED)
BOOL_TYPE HeaterNeutralRelayOpenMonitor__GetHeaterOpenDetected(void);
#endif // (HEATER_OPEN_PLUS_N_RELAY_OPEN_DETECTION == ENABLED)

#endif // HEATERNEUTRALRELAYOPENMONITOR_H_



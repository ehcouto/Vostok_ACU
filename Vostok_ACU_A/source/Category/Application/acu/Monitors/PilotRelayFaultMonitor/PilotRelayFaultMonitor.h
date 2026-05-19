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
#ifndef PILOTRELAYFAULTMONITOR_H_
#define PILOTRELAYFAULTMONITOR_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void PilotRelayFaultMonitor__Initialize(void);
void PilotRelayFaultMonitor__Execution(void);
void PilotRelayFaultMonitor__CycleStarted(void);
uint8 PilotRelayFaultMonitor__GetConsistency(void);
uint8 PilotRelayFaultMonitor__GetStabilizationCounter(void);
void PilotRelayFaultMonitor__RestartStabilizationCounter(void);


#endif // PILOTRELAYFAULTMONITOR_H_



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
#ifndef VSDRAINMOTORFAULTMONITOR_H_
#define VSDRAINMOTORFAULTMONITOR_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void VSDrainMotorFaultMonitor__Initialize(void);
void VSDrainMotorFaultMonitor__Execution(void);
void VSDrainMotorFaultMonitor__SetDrainMotorFailures(uint32 failures);
void VSDrainMotorFaultMonitor__SetDrainMotorFailureFlag(BOOL_TYPE value);
void VSDrainMotorFaultMonitor__ResetTimerClearMotorFailureFlag(void);
#endif // VSDRAINMOTORFAULTMONITOR_H_



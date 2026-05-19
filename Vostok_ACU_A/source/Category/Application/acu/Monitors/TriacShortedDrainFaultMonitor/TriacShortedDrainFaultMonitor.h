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
#ifndef TRIACSHORTEDDRAINFAULTMONITOR_H_
#define TRIACSHORTEDDRAINFAULTMONITOR_H_
#include "Hbl_prv.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

typedef PACKED struct DRAIN_TRIAC_FAULT_PARAMS_STRUCT
{
   uint16 Current_Sense_Drain_Min;
   uint16 Current_Sense_Drain_Max;
} DRAIN_TRIAC_FAULT_PARAMS_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void TriacShortedDrainFaultMonitor__Initialize(void);
void TriacShortedDrainFaultMonitor__Execution(void);
void TriacShortedDrainFaultMonitor__CycleStarted(void);


#endif // TRIACSHORTEDDRAINFAULTMONITOR_H_



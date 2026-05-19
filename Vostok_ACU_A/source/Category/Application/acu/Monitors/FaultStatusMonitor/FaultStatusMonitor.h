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
#ifndef FAULTSTATUSMONITOR_H_
#define FAULTSTATUSMONITOR_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
extern uint16 Fault_Status_Bitmap_0_15;
extern uint16 Fault_Status_Bitmap_16_31;
extern uint16 Fault_Status_Bitmap_32_47;
extern uint16 Fault_Status_Bitmap_48_63;
extern uint16 Fault_Status_Bitmap_64_79;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void FaultStatusMonitor__Initialize(void);
void FaultStatusMonitor__Execution(void);


#endif // FAULTSTATUSMONITOR_H_



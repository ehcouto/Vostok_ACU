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
#ifndef NORMAL_FILL_H_
#define NORMAL_FILL_H_

#include "Interpreter.h"
#include "Process.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef PACKED struct FILL_PARAMETER_STRUCT
{
    uint8 Liters_Offset;
    uint8 Liters_Max;
    uint8 Valve_Closed_Threshold;
    uint8 Slow_Inlet_Threshold;
    uint8 Acceptable_Inlet_Threshold;
    uint8 Fill_Check_Water_Sub_Cycle_Priorty;
    uint16 Timed_Fill_Flow_rate;
} FILL_PARAMETER_TYPE;


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void NormalFill__Initialize(void);
PROCESS_RESULT_TYPE NormalFill__Manager(PROCESS_CONTROL_TYPE* control);
void NormalFill__SetFillSubcycleExecuted(uint8 compartment, uint8 subcycle_id, SUB_CYCLE_END_TYPE subcycle_end_code);
void NormalFill__ClearFillValveFailure(void);
void NormalFill__ClearH2OAlarm(void);

#endif // NORMAL_FILL_H_



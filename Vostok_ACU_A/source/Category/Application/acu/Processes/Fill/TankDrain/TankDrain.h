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
#ifndef TANK_DRAIN_H_
#define TANK_DRAIN_H_

#include "Interpreter.h"
#include "Process.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void TankDrain__Initialize(void);
PROCESS_RESULT_TYPE TankDrain__Manager(PROCESS_CONTROL_TYPE* control);
void TankDrain__SetSubcycleExecuted(uint8 compartment, uint8 subcycle_id, SUB_CYCLE_END_TYPE subcycle_end_code);
#endif // TANK_DRAIN_H_



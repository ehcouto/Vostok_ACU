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
#ifndef CLASS_A_FILL_H_
#define CLASS_A_FILL_H_

#include "Interpreter.h"
#include "Process.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void ClassAFill__Initialize(void);
PROCESS_RESULT_TYPE ClassAFill__Manager(PROCESS_CONTROL_TYPE* control);
void ClassAFill__SetSubcycleExecuted(uint8 compartment, uint8 subcycle_id, SUB_CYCLE_END_TYPE subcycle_end_code);
void ClassAFill__SetH2OPauseExecuted(void);
void ClassAFill__DisableFlowRateCalibration(void);

#endif // CLASS_A_FILL_H_



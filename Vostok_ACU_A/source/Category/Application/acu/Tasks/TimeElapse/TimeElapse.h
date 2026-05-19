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
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SOURCE_CATEGORY_APPLICATION_APPLICATIONDRYER_TASKS_TIMEELAPSE_TIMEELAPSE_H_
#define SOURCE_CATEGORY_APPLICATION_APPLICATIONDRYER_TASKS_TIMEELAPSE_TIMEELAPSE_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
extern uint16 TimeElapse_TimeCount;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void TimeElapse__NVDataRestore(uint8 compartment);
void TimeElapse__Terminate(uint8 compartment);
void TimeElapse__Resume(void);
void TimeElapse__Pause(void);
ACTIVE_TYPE TimeElapse__Initialize(uint8 compartment, uint8 sf_index, uint8 displacement);
ACTIVE_TYPE TimeElapse__Task(uint8 compartment);
#endif // SOURCE_CATEGORY_APPLICATION_APPLICATIONDRYER_TASKS_TIMEELAPSE_TIMEELAPSE_H_



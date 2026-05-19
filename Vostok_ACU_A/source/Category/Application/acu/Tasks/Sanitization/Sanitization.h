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
#ifndef SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_TASKS_SANITIZATION_SANITIZATION_H_
#define SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_TASKS_SANITIZATION_SANITIZATION_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef enum TASK_SANITIZATION_RESULT_ENUM
{
	TASK_SANITIZATION_NOT_EXECUTED = 0,
	TASK_SANITIZATION_SUCCESSFUL,
	TASK_SANITIZATION_FAILED
}TASK_SANITIZATION_RESULT_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void Sanitization__NVDataRestore(uint8 compartment);
void Sanitization__Terminate(uint8 compartment);
ACTIVE_TYPE Sanitization__Initialize(uint8 compartment, uint8 sf_index, uint8 displacement);
ACTIVE_TYPE Sanitization__Task(uint8 compartment);
void Sanitization__SetResultVariable(TASK_SANITIZATION_RESULT_TYPE result);
#endif // SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_TASKS_SANITIZATION_SANITIZATION_H_



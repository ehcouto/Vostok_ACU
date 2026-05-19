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
#ifndef SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_PROCESSES_OWIONESHOT_OWIONESHOT_H_
#define SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_PROCESSES_OWIONESHOT_OWIONESHOT_H_

#include "Process.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
PROCESS_RESULT_TYPE OWIOneShot__Process(PROCESS_CONTROL_TYPE* control);
void OWIOneShot__Initialize(void);
void OWIOneShot__PauseTimer(void);
void OWIOneShot__ResumeTimer(void);
void OWIOneShot__CycleResumeStart(uint8 compartment,uint8 subcycle_id);
void OWIOneShot__CycleResumeEnd(uint8 compartment,uint8 subcycle_id);

#endif // SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_PROCESSES_OWIONESHOT_OWIONESHOT_H_



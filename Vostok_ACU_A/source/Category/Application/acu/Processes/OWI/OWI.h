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
#ifndef SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_PROCESSES_OWI_OWI_H_
#define SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_PROCESSES_OWI_OWI_H_

#include "Process.h"
#include "Log.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void OWI__Initialize(void);
PROCESS_RESULT_TYPE OWI__Process(PROCESS_CONTROL_TYPE* control);
#endif // SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_PROCESSES_OWI_OWI_H_



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
#ifndef SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_PROCESSES_ALTERNATECTRL_ALTERNATECTRL_H_
#define SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_PROCESSES_ALTERNATECTRL_ALTERNATECTRL_H_

#include "Process.h"
#include "Log.h"
#include "SettingFile.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void AlternateCtrl__Initialize(void);
PROCESS_RESULT_TYPE AlternateCtrl__Process(PROCESS_CONTROL_TYPE* control);
uint8 AlternateCtrl__GetPosition(void);
#if (API221_CLIENT_FEATURE == ENABLED)
void AlternateCtrl__UnregisterApi221Events(void);
#endif
void AlternateCtrl__McuReset(uint8 node, uint8 reason, uint8 reset_mode);
#endif // SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_PROCESSES_ALTERNATECTRL_ALTERNATECTRL_H_



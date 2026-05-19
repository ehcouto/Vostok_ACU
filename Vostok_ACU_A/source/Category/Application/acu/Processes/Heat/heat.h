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
#ifndef SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_PROCESSES_HEAT_HEAT_H_
#define SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_PROCESSES_HEAT_HEAT_H_

#include "Process.h"
#include "Heat_prv.h"
#include "Log.h"
#include "SettingFile.h"
#include "Modifier.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//! Definition of Heat process status events
typedef enum HEAT_PROCESS_EVENT_ENUM
{
    HEAT_PROCESS_STARTED    = 0,
    HEAT_PROCESS_BUSY	    = 1,
    HEAT_PROCESS_COMPLETED  = 2,
    HEAT_PROCESS_MAX	    = 3,
} HEAT_PROCESS_EVENT_TYPE;

typedef void (*HEAT_EVENT_HANDLER_TYPE)(HEAT_PROCESS_EVENT_TYPE event);
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

PROCESS_RESULT_TYPE Heat__Process(PROCESS_CONTROL_TYPE* control);
void Heat__Initialize(void);
void Heat__ResetHeatTimeoutFault(void);
#endif // SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_PROCESSES_HEAT_HEAT_H_



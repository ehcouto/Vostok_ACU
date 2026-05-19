/**
 *  @file       HeaterLoad.c
 *
 *  @brief      Load Handler that performs delayed activation between two pins required to activate load
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
#ifndef HEATERLOAD_H_
#define HEATERLOAD_H_

#include "SystemConfig.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#ifndef HBL_LOAD_NUM_HEATER
    #define HBL_LOAD_NUM_HEATER 0
#endif

#if (HBL_LOAD_NUM_HEATER > 0)
#include "HblDefs.h"

#define HEATERLOAD_REGISTER                 HeaterLoad__Initialize,            \
                                            HeaterLoad__Allocate,              \
                                            HeaterLoad__InitializeInstance,    \
                                            HeaterLoad__Request,               \
                                            HeaterLoad__Process,               \
                                            HeaterLoad__GetRequested,          \
                                            HeaterLoad__GetProcessed,          \
                                            HeaterLoad__AsyncProcess,          \
                                            HeaterLoad__GetStatus,             \
                                            1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void HeaterLoad__Initialize(void);
PASS_FAIL_TYPE HeaterLoad__InitializeInstance(HBL_LOAD_TYPE * load);
uint8 HeaterLoad__Allocate(void);
PASS_FAIL_TYPE HeaterLoad__Request(HBL_LOAD_TYPE * load,void * data);
PASS_FAIL_TYPE HeaterLoad__Process(HBL_LOAD_TYPE * load , HBL_PILOT_HANDLER_HOOK_TYPE * pilot);
void * HeaterLoad__GetRequested(HBL_LOAD_TYPE * load);
void * HeaterLoad__GetProcessed(HBL_LOAD_TYPE * load , HBL_PILOT_HANDLER_HOOK_TYPE * pilot);
PASS_FAIL_TYPE HeaterLoad__AsyncProcess(HBL_LOAD_TYPE * load , HBL_PILOT_HANDLER_HOOK_TYPE * pilot);
void * HeaterLoad__GetStatus(HBL_LOAD_TYPE * load , HBL_PILOT_HANDLER_HOOK_TYPE * pilot);
BOOL_TYPE *HeaterLoad__GetInvertedSequenceStatusByLoadId(HBL_LOAD_ENUM_TYPE load_Id);

#endif  // HBL_LOAD_NUM_HEATER > 0

#endif // HEATERLOAD_H_

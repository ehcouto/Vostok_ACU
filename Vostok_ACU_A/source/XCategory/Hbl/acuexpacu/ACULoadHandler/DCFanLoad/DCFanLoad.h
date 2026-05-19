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
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef DCFANLOAD_H_
#define DCFANLOAD_H_
#include "C_Extensions.h"
#include "SystemConfig.h"
#ifndef HBL_LOAD_NUM_DCFAN
    #define HBL_LOAD_NUM_DCFAN 0
#endif

#if (HBL_LOAD_NUM_DCFAN > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#define DCFANLOAD_REGISTER      DCFanLoad__Initialize,              \
                                DCFanLoad__Allocate,                \
                                DCFanLoad__InitializeInstance,      \
                                DCFanLoad__Request,                 \
                                DCFanLoad__Process,                 \
                                DCFanLoad__GetRequested,            \
                                DCFanLoad__GetProcessed,            \
                                DCFanLoad__AsyncProcess,            \
                                DCFanLoad__GetStatus,               \
                                1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void DCFanLoad__Initialize(void);
uint8 DCFanLoad__Allocate(void);
PASS_FAIL_TYPE DCFanLoad__InitializeInstance(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE DCFanLoad__Request(HBL_LOAD_TYPE * load, void * data);
PASS_FAIL_TYPE  DCFanLoad__Process(HBL_LOAD_TYPE * load, HBL_PILOT_HANDLER_HOOK_TYPE * pilot);
void * DCFanLoad__GetRequested(HBL_LOAD_TYPE * load);
void * DCFanLoad__GetProcessed(HBL_LOAD_TYPE * load, HBL_PILOT_HANDLER_HOOK_TYPE * pilot);
PASS_FAIL_TYPE DCFanLoad__AsyncProcess(HBL_LOAD_TYPE * load, HBL_PILOT_HANDLER_HOOK_TYPE * pilot);
void * DCFanLoad__GetStatus	(HBL_LOAD_TYPE * load, HBL_PILOT_HANDLER_HOOK_TYPE * pilot);

#endif
#endif // DCFANLOAD_H_



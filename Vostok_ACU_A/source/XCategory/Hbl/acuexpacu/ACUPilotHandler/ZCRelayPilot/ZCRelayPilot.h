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
#ifndef ZCRELAYPILOT_H
	#define ZCRELAYPILOT_H
#include "C_Extensions.h"
#include "SystemConfig.h"

#ifndef HBL_PILOT_NUM_ZCRELAY
    #define HBL_PILOT_NUM_ZCRELAY               0
#endif

#if (HBL_PILOT_NUM_ZCRELAY > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#define ZCRELAYPILOT_REGISTER               ZCRelayPilot__Initialize,         \
                                            ZCRelayPilot__Allocate,           \
                                            ZCRelayPilot__InitializeInstance, \
                                            ZCRelayPilot__Request,            \
                                            ZCRelayPilot__Process,            \
                                            ZCRelayPilot__GetRequested,       \
                                            ZCRelayPilot__GetProcessed,       \
                                            NULL,                             \
                                            ZCRelayPilot__AsynchProcess,      \
                                            NULL,                             \
                                            NULL,                             \
                                            NULL,                             \
                                            1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void ZCRelayPilot__Initialize(void);
uint8 ZCRelayPilot__Allocate(void);
PASS_FAIL_TYPE ZCRelayPilot__InitializeInstance(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE ZCRelayPilot__Request(HBL_LOAD_TYPE * load,void * data);
PASS_FAIL_TYPE ZCRelayPilot__Process(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE ZCRelayPilot__AsynchProcess(HBL_LOAD_TYPE * load);
void * ZCRelayPilot__GetRequested(HBL_LOAD_TYPE * load);
void * ZCRelayPilot__GetProcessed(HBL_LOAD_TYPE * load);

#endif

#endif // ZCRELAYPILOT_H



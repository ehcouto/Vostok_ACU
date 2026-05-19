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
#ifndef ZCTRIACPILOT_H
	#define ZCTRIACPILOT_H
#include "C_Extensions.h"
#include "SystemConfig.h"

#ifndef HBL_PILOT_NUM_ZCTRIAC
    #define HBL_PILOT_NUM_ZCTRIAC               0
#endif

#if (HBL_PILOT_NUM_ZCTRIAC > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#define ZCTRIACPILOT_REGISTER               ZCTriacPilot__Initialize,         \
                                            ZCTriacPilot__Allocate,           \
                                            ZCTriacPilot__InitializeInstance, \
                                            ZCTriacPilot__Request,            \
                                            ZCTriacPilot__Process,            \
                                            ZCTriacPilot__GetRequested,       \
                                            ZCTriacPilot__GetProcessed,       \
                                            ZCTriacPilot__ZCProcess,          \
                                            NULL,                             \
                                            NULL,                             \
                                            NULL,                             \
                                            NULL,                             \
                                            1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void ZCTriacPilot__Initialize(void);
uint8 ZCTriacPilot__Allocate(void);
PASS_FAIL_TYPE ZCTriacPilot__InitializeInstance(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE ZCTriacPilot__Request(HBL_LOAD_TYPE * load,void * data);
PASS_FAIL_TYPE ZCTriacPilot__Process(HBL_LOAD_TYPE * load);
void * ZCTriacPilot__GetRequested(HBL_LOAD_TYPE * load);
void * ZCTriacPilot__GetProcessed(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE ZCTriacPilot__ZCProcess(HBL_LOAD_TYPE * load);
void * ZCTriacPilot__GetStatus(HBL_LOAD_TYPE *load);

#endif

#endif // ZCTRIACPILOT_H



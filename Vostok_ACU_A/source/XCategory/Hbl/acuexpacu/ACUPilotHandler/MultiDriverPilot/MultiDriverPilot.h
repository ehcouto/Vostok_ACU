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
#ifndef MULTIDRIVERPILOT_H
	#define MULTIDRIVERPILOT_H
#include "C_Extensions.h"
#include "SystemConfig.h"

#ifndef HBL_PILOT_NUM_MULTIDRIVER
    #define HBL_PILOT_NUM_MULTIDRIVER     0
#endif

#if (HBL_PILOT_NUM_MULTIDRIVER > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#define MULTIDRIVERPILOT_REGISTER           MultiDriverPilot__Initialize,         \
                                            MultiDriverPilot__Allocate,           \
                                            MultiDriverPilot__InitializeInstance, \
                                            MultiDriverPilot__Request,            \
                                            MultiDriverPilot__Process,            \
                                            MultiDriverPilot__GetRequested,       \
                                            MultiDriverPilot__GetProcessed,       \
                                            MultiDriverPilot__ZCProcess,          \
                                            MultiDriverPilot__AsynchProcess,      \
                                            NULL,                                 \
                                            NULL,                                 \
                                            NULL,                                 \
                                            4

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void MultiDriverPilot__Initialize(void);
uint8 MultiDriverPilot__Allocate(void);
PASS_FAIL_TYPE MultiDriverPilot__InitializeInstance(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE MultiDriverPilot__Request(HBL_LOAD_TYPE * load,void * data);
PASS_FAIL_TYPE MultiDriverPilot__Process(HBL_LOAD_TYPE * load);
void * MultiDriverPilot__GetRequested(HBL_LOAD_TYPE * load);
void * MultiDriverPilot__GetProcessed(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE MultiDriverPilot__AsynchProcess(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE MultiDriverPilot__ZCProcess(HBL_LOAD_TYPE * load);

#endif

#endif // MULTIDRIVERPILOT_H



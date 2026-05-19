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
#ifndef MULTISEQUENCEPILOT_H
	#define MULTISEQUENCEPILOT_H
#include "C_Extensions.h"
#include "SystemConfig.h"

#ifndef HBL_PILOT_NUM_MULTISEQUENCE
    #define HBL_PILOT_NUM_MULTISEQUENCE     0
#endif

#if (HBL_PILOT_NUM_MULTISEQUENCE > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#define MULTISEQUENCEPILOT_REGISTER         MultiSequencePilot__Initialize,         \
                                            MultiSequencePilot__Allocate,           \
                                            MultiSequencePilot__InitializeInstance, \
                                            MultiSequencePilot__Request,            \
                                            MultiSequencePilot__Process,            \
                                            MultiSequencePilot__GetRequested,       \
                                            MultiSequencePilot__GetProcessed,       \
                                            MultiSequencePilot__ZCProcess,          \
                                            MultiSequencePilot__AsynchProcess,      \
                                            NULL,          							\
                                            MultiSequencePilot__OneMsHandler,       \
                                            MultiSequencePilot__GetStatus,          \
                                            1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void MultiSequencePilot__Initialize(void);
uint8 MultiSequencePilot__Allocate(void);
PASS_FAIL_TYPE MultiSequencePilot__InitializeInstance(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE MultiSequencePilot__Request(HBL_LOAD_TYPE * load,void * data);
PASS_FAIL_TYPE MultiSequencePilot__Process(HBL_LOAD_TYPE * load);
void * MultiSequencePilot__GetRequested(HBL_LOAD_TYPE * load);
void * MultiSequencePilot__GetProcessed(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE MultiSequencePilot__AsynchProcess(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE MultiSequencePilot__ZCProcess(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE MultiSequencePilot__OneMsHandler(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE MultiSequencePilot__OCProcess(HBL_LOAD_TYPE * load);
void * MultiSequencePilot__GetStatus(HBL_LOAD_TYPE *load);

#endif

#endif // MULTISEQUENCEPILOT_H



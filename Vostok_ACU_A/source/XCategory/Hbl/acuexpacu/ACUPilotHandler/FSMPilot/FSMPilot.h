/**
 *  @file
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>
 *
 *  $Header: 	FSMPilot.h 27/04/2016 KAVEH RAZZAGHI (RAZZAK) Exp  $
 *
 *  @copyright  Copyright 2016.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef FSMPILOT_H_
#define FSMPILOT_H_

#include "C_Extensions.h"
#include "SystemConfig.h"

#ifndef HBL_PILOT_NUM_FSM
    #define HBL_PILOT_NUM_FSM 0
#endif

#if (HBL_PILOT_NUM_FSM > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

typedef enum
{
	FSMPILOT_VELOCITY_ZERO 		= 0,
	FSMPILOT_VELOCITY_POSITIVE  = 1,
	FSMPILOT_VELOCITY_NEGATIVE  =-1
} FSMPILOT_VELOCITY_SIGN_TYPE;

#define FSMPILOT_REGISTER            		FSMPilot__Initialize,            \
                                            FSMPilot__Allocate,              \
                                            FSMPilot__InitializeInstance,    \
                                            FSMPilot__Request,               \
                                            FSMPilot__Process,               \
                                            FSMPilot__GetRequested,          \
                                            FSMPilot__GetProcessed,          \
                                            NULL,                            \
                                            FSMPilot__AsyncProcess,          \
                                            NULL,                            \
                                            NULL,          					 \
                                            FSMPilot__GetStatus,             \
                                            1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void FSMPilot__Initialize(void);
PASS_FAIL_TYPE FSMPilot__InitializeInstance(HBL_LOAD_TYPE * load);
uint8 FSMPilot__Allocate(void);
PASS_FAIL_TYPE FSMPilot__Request(HBL_LOAD_TYPE * load,void * data);
PASS_FAIL_TYPE FSMPilot__Process(HBL_LOAD_TYPE * load);
void * FSMPilot__GetRequested(HBL_LOAD_TYPE * load);
void * FSMPilot__GetProcessed(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE FSMPilot__AsyncProcess(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE FSMPilot__OneMsHandler(HBL_LOAD_TYPE * load);
void * FSMPilot__GetStatus(HBL_LOAD_TYPE * load);

#endif

#endif /* FSMPILOT_H_ */

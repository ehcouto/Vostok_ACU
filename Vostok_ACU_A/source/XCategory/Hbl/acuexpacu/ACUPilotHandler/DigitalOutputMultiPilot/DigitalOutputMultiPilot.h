/**
 *  @file       DigitalOutputMultiPilot.h
 *
 *  @brief      Header file for DigitalOutputMulti module
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/14 17:37:53EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef DIGITALOUTPUTMULTIPILOT_H_
#define DIGITALOUTPUTMULTIPILOT_H_
#include "C_Extensions.h"
#include "SystemConfig.h"

#ifndef HBL_PILOT_NUM_DIGITALOUTPUTMULTI
    #define HBL_PILOT_NUM_DIGITALOUTPUTMULTI    0
#endif

#if (HBL_PILOT_NUM_DIGITALOUTPUTMULTI > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

typedef uint8 DIGITALOUTPUTMULTI_HANDLER_DATA_TYPE;

#define DIGITALOUTPUTMULTIPILOT_REGISTER        DigitalOutputMultiPilot__Initialize,            \
                                                DigitalOutputMultiPilot__Allocate,              \
                                                DigitalOutputMultiPilot__InitializeInstance,    \
                                                DigitalOutputMultiPilot__Request,               \
                                                DigitalOutputMultiPilot__Process,               \
                                                DigitalOutputMultiPilot__GetRequested,          \
                                                DigitalOutputMultiPilot__GetProcessed,          \
                                                NULL,                                           \
												DigitalOutputMultiPilot__AsynchProcess,         \
                                                NULL,                                           \
                                                NULL,                                           \
												DigitalOutputMultiPilot__GetStatus,             \
                                                1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void DigitalOutputMultiPilot__Initialize(void);
uint8 DigitalOutputMultiPilot__Allocate(void);
PASS_FAIL_TYPE DigitalOutputMultiPilot__InitializeInstance(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE DigitalOutputMultiPilot__Request(HBL_LOAD_TYPE * load,void * data);
PASS_FAIL_TYPE DigitalOutputMultiPilot__Process(HBL_LOAD_TYPE * load);
void * DigitalOutputMultiPilot__GetRequested(HBL_LOAD_TYPE * load);
void * DigitalOutputMultiPilot__GetProcessed(HBL_LOAD_TYPE * load);
void * DigitalOutputMultiPilot__GetStatus(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE DigitalOutputMultiPilot__AsynchProcess(HBL_LOAD_TYPE * load);

#endif

#endif // DIGITALOUTPUTMULTIPILOT_H_



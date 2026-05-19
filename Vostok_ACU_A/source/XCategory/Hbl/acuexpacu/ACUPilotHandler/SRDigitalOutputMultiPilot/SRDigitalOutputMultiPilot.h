/**
 *  @file       SRDigitalOutputMultiPilot.c
 *
 *  @brief      Interface for DigitalOutputMulti Pilot Handler
 *
 *  @details    It handles 2 pin output configuration only; they are driven together
 *              according to Processed value and each Pin High flag set.
 *
 *  @defgroup   CLASS_B
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/14 17:37:53EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRDIGITALOUTPUTMULTIPILOT_H_
#define SRDIGITALOUTPUTMULTIPILOT_H_
#include "C_Extensions.h"
#include "SystemConfig.h"

#ifndef HBL_PILOT_NUM_SRDIGITALOUTPUTMULTI
    #define HBL_PILOT_NUM_SRDIGITALOUTPUTMULTI    0
#endif

#if (HBL_PILOT_NUM_SRDIGITALOUTPUTMULTI > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

typedef uint8 SRDIGITALOUTPUTMULTI_HANDLER_DATA_TYPE;

#define SRDIGITALOUTPUTMULTIPILOT_REGISTER      SRDigitalOutputMultiPilot__Initialize,            \
                                                SRDigitalOutputMultiPilot__Allocate,              \
                                                SRDigitalOutputMultiPilot__InitializeInstance,    \
                                                SRDigitalOutputMultiPilot__Request,               \
                                                SRDigitalOutputMultiPilot__Process,               \
                                                SRDigitalOutputMultiPilot__GetRequested,          \
                                                SRDigitalOutputMultiPilot__GetProcessed,          \
                                                NULL,                                             \
												SRDigitalOutputMultiPilot__AsynchProcess,         \
                                                NULL,                                             \
                                                NULL,                                             \
												SRDigitalOutputMultiPilot__GetStatus,             \
                                                1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void SRDigitalOutputMultiPilot__Initialize(void);
uint8 SRDigitalOutputMultiPilot__Allocate(void);
PASS_FAIL_TYPE SRDigitalOutputMultiPilot__InitializeInstance(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE SRDigitalOutputMultiPilot__Request(HBL_LOAD_TYPE * load,void * data);
PASS_FAIL_TYPE SRDigitalOutputMultiPilot__Process(HBL_LOAD_TYPE * load);
void * SRDigitalOutputMultiPilot__GetRequested(HBL_LOAD_TYPE * load);
void * SRDigitalOutputMultiPilot__GetProcessed(HBL_LOAD_TYPE * load);
void * SRDigitalOutputMultiPilot__GetStatus(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE SRDigitalOutputMultiPilot__AsynchProcess(HBL_LOAD_TYPE * load);
void SRDigitalOutputMultiPilot__Handler(void);

#endif

#endif // SRDIGITALOUTPUTMULTIPILOT_H_



/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: ChargePumpPilot.h 1.6 2015/07/14 17:37:52EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/14 17:37:52EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef CHARGEPUMPPILOT_H_
#define CHARGEPUMPPILOT_H_

#include "C_Extensions.h"
#include "SystemConfig.h"

#ifndef HBL_PILOT_NUM_CHARGEPUMP
    #define HBL_PILOT_NUM_CHARGEPUMP 0
#endif

#if (HBL_PILOT_NUM_CHARGEPUMP > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================





#define CHARGEPUMPPILOT_REGISTER            ChargePumpPilot__Initialize,            \
                                            ChargePumpPilot__Allocate,              \
                                            ChargePumpPilot__InitializeInstance,    \
                                            ChargePumpPilot__Request,               \
                                            ChargePumpPilot__Process,               \
                                            ChargePumpPilot__GetRequested,          \
                                            ChargePumpPilot__GetProcessed,          \
                                            NULL,                                   \
                                            NULL,                                   \
                                            NULL,                                   \
                                            ChargePumpPilot__Handler1ms,            \
                                            NULL,                                   \
                                            1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void ChargePumpPilot__Initialize(void);
PASS_FAIL_TYPE ChargePumpPilot__InitializeInstance(HBL_LOAD_TYPE * load);
uint8 ChargePumpPilot__Allocate(void);
PASS_FAIL_TYPE ChargePumpPilot__Request(HBL_LOAD_TYPE * load,void * data);
PASS_FAIL_TYPE ChargePumpPilot__Process(HBL_LOAD_TYPE * load);
void * ChargePumpPilot__GetRequested(HBL_LOAD_TYPE * load);
void * ChargePumpPilot__GetProcessed(HBL_LOAD_TYPE * load);

PASS_FAIL_TYPE ChargePumpPilot__Handler1ms(HBL_LOAD_TYPE * load);

#endif

#endif // CHARGEPUMPPILOT_H_



/**
 *  @file       
 *
 *  @brief      Pilot handling ZC synchronized Digital output
 *
 *  @details    It handles multiple instances of a ZCDigital pilot that drives a single output in sync with ZeroCross.
 *
 *  @copyright  Copyright 2022- Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef TRIACSWITCHPILOT_H
#define TRIACSWITCHPILOT_H
#include "C_Extensions.h"
#include "SystemConfig.h"

#ifndef HBL_PILOT_NUM_TRIACSWITCH
    #define HBL_PILOT_NUM_TRIACSWITCH               0
#endif

#if (HBL_PILOT_NUM_TRIACSWITCH > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#define TRIACSWITCHPILOT_REGISTER           TriacSwitchPilot__Initialize,           \
                                            TriacSwitchPilot__Allocate,             \
                                            TriacSwitchPilot__InitializeInstance,   \
                                            TriacSwitchPilot__Request,              \
                                            TriacSwitchPilot__Process,              \
                                            TriacSwitchPilot__GetRequested,         \
                                            TriacSwitchPilot__GetProcessed,         \
                                            TriacSwitchPilot__ZCProcess,            \
                                            NULL,                                   \
                                            NULL,                                   \
                                            TriacSwitchPilot__OneMsHandler,         \
                                            NULL,                                   \
                                            1

PACKED typedef struct
{
    HBL_LOAD_STATUS_TYPE Hbl_Status;
    uint8 Pin_Value;
} TRIACSWITCHPILOT_STATUS_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void TriacSwitchPilot__Initialize(void);
uint8 TriacSwitchPilot__Allocate(void);
PASS_FAIL_TYPE TriacSwitchPilot__InitializeInstance(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE TriacSwitchPilot__Request(HBL_LOAD_TYPE * load,void * data);
PASS_FAIL_TYPE TriacSwitchPilot__Process(HBL_LOAD_TYPE * load);
void * TriacSwitchPilot__GetRequested(HBL_LOAD_TYPE * load);
void * TriacSwitchPilot__GetProcessed(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE TriacSwitchPilot__ZCProcess(HBL_LOAD_TYPE * load);
void * TriacSwitchPilot__GetStatus(HBL_LOAD_TYPE *load);
PASS_FAIL_TYPE TriacSwitchPilot__OneMsHandler(HBL_LOAD_TYPE * load);
#endif

#endif // TRIACSWITCHPILOT_H



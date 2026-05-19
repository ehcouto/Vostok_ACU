/**
 *  @file       
 *
 *  @brief      Public interface for DigitalOutputPilot module.
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/14 17:37:54EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef DIGITALOUTPUTPILOT_H_
#define DIGITALOUTPUTPILOT_H_
#include "C_Extensions.h"
#include "SystemConfig.h"

#ifndef HBL_PILOT_NUM_DIGITALOUTPUT
    #define HBL_PILOT_NUM_DIGITALOUTPUT 0
#endif

#if (HBL_PILOT_NUM_DIGITALOUTPUT > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#define DIGITALOUTPUTPILOT_REGISTER         DigitalOutputPilot__Initialize,         \
                                            DigitalOutputPilot__Allocate,           \
                                            DigitalOutputPilot__InitializeInstance, \
                                            DigitalOutputPilot__Request,            \
                                            DigitalOutputPilot__Process,            \
                                            DigitalOutputPilot__GetRequested,       \
                                            DigitalOutputPilot__GetProcessed,       \
                                            NULL,                                   \
                                            NULL,                                   \
                                            NULL,                                   \
                                            NULL,                                   \
                                            DigitalOutputPilot____GetStatus,        \
                                            1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void            DigitalOutputPilot__Initialize          (void);
uint8           DigitalOutputPilot__Allocate            (void);
PASS_FAIL_TYPE  DigitalOutputPilot__InitializeInstance  (HBL_LOAD_TYPE *load);
PASS_FAIL_TYPE  DigitalOutputPilot__Request             (HBL_LOAD_TYPE *load, void *data);
PASS_FAIL_TYPE  DigitalOutputPilot__Process             (HBL_LOAD_TYPE *load);
void *          DigitalOutputPilot__GetRequested        (HBL_LOAD_TYPE *load);
void *          DigitalOutputPilot__GetProcessed        (HBL_LOAD_TYPE *load);
void *          DigitalOutputPilot____GetStatus         (HBL_LOAD_TYPE *load);

#endif

#endif // DIGITALOUTPUTPILOT_H_

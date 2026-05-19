/**
 *  @file       AnalogOuptutPilot.h
 *
 *  @brief      Public interface for AnalogOutputPilot module.
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/14 17:37:54EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef ANALOGOUTPUTPILOT_H_
#define ANALOGOUTPUTPILOT_H_
#include "C_Extensions.h"
#include "SystemConfig.h"

#ifndef HBL_PILOT_NUM_ANALOGOUTPUT
    #define HBL_PILOT_NUM_ANALOGOUTPUT 0
#endif

#if (HBL_PILOT_NUM_ANALOGOUTPUT > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#define ANALOGOUTPUTPILOT_REGISTER          AnalogOutputPilot__Initialize,         \
                                            AnalogOutputPilot__Allocate,           \
                                            AnalogOutputPilot__InitializeInstance, \
                                            AnalogOutputPilot__Request,            \
                                            AnalogOutputPilot__Process,            \
                                            AnalogOutputPilot__GetRequested,       \
                                            AnalogOutputPilot__GetProcessed,       \
                                            NULL,                                  \
                                            AnalogOutputPilot__AsynchProcess,	   \
                                            NULL,                                  \
                                            NULL,                                  \
                                            AnalogOutputPilot__GetStatus,          \
                                            2

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void            AnalogOutputPilot__Initialize          (void);
uint8           AnalogOutputPilot__Allocate            (void);
PASS_FAIL_TYPE  AnalogOutputPilot__InitializeInstance  (HBL_LOAD_TYPE *load);
PASS_FAIL_TYPE  AnalogOutputPilot__Request             (HBL_LOAD_TYPE *load, void *data);
PASS_FAIL_TYPE  AnalogOutputPilot__Process             (HBL_LOAD_TYPE *load);
PASS_FAIL_TYPE  AnalogOutputPilot__AsynchProcess	   (HBL_LOAD_TYPE *load);
void *          AnalogOutputPilot__GetRequested        (HBL_LOAD_TYPE *load);
void *          AnalogOutputPilot__GetProcessed        (HBL_LOAD_TYPE *load);
void *          AnalogOutputPilot__GetStatus           (HBL_LOAD_TYPE *load);

#endif

#endif // ANALOGOUTPUTPILOT_H_

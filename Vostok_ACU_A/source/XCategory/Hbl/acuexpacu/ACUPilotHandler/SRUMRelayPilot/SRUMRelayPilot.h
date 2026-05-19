/**
 *  @file       SRUMRelayPilot.h
 *
 *  @brief      Pilot for Universal Motor driven with Triac + Relays - Header file
 *
 *  @defgroup   CLASS_B
 *
 *  @copyright  Copyright 2019. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRUMRELAYPILOT_H_
#define SRUMRELAYPILOT_H_

#include "SRSystemConfig.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#ifndef HBL_PILOT_NUM_SRUMRELAY
#define HBL_PILOT_NUM_SRUMRELAY       0
#endif

#define     UNIVERSAL_MOTOR_PILOT_MAX_VALUE           ((1 << 15) - 1)

#if (HBL_PILOT_NUM_SRUMRELAY > 0)
#include "SRUMRelayPilot_defs.h"
#include "ZeroCross.h"
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#define SRUMRELAYPILOT_REGISTER             SRUMRelayPilot__Initialize,         \
                                            SRUMRelayPilot__Allocate,           \
                                            SRUMRelayPilot__InitializeInstance, \
                                            SRUMRelayPilot__Request,            \
                                            SRUMRelayPilot__Process,            \
                                            SRUMRelayPilot__GetRequested,       \
                                            SRUMRelayPilot__GetProcessed,       \
                                            SRUMRelayPilot__ZcProcess,          \
                                            SRUMRelayPilot__AsyncProcess,       \
                                            SRUMRelayPilot__OCProcess,          \
                                            NULL,                               \
                                            SRUMRelayPilot__GetStatus,          \
                                            sizeof(SRUMRELAYPILOT_PARAM_TYPE)

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void            SRUMRelayPilot__Initialize          (void);
uint8           SRUMRelayPilot__Allocate            (void);
PASS_FAIL_TYPE  SRUMRelayPilot__InitializeInstance  (HBL_LOAD_TYPE *load);
PASS_FAIL_TYPE  SRUMRelayPilot__Request             (HBL_LOAD_TYPE *load, void *data);
PASS_FAIL_TYPE  SRUMRelayPilot__Process             (HBL_LOAD_TYPE *load);
PASS_FAIL_TYPE  SRUMRelayPilot__ZcProcess           (HBL_LOAD_TYPE *load);
PASS_FAIL_TYPE  SRUMRelayPilot__AsyncProcess        (HBL_LOAD_TYPE *load);
PASS_FAIL_TYPE  SRUMRelayPilot__OCProcess           (HBL_LOAD_TYPE *load);
void *          SRUMRelayPilot__GetRequested        (HBL_LOAD_TYPE *load);
void *          SRUMRelayPilot__GetProcessed        (HBL_LOAD_TYPE *load);
void *          SRUMRelayPilot__GetStatus           (HBL_LOAD_TYPE *load);
void            SRUMRelayPilot__Handler             (void);
#endif

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
#endif // SRUMRELAYPILOT_H_

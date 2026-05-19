/**
 *  @file       DoorLockEMPilot.h
 *
 *  @brief      Interface for Pilot Handler for EM DoorLock device
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
#ifndef DOORLOCKEMPILOT_H_
#define DOORLOCKEMPILOT_H_

#include "C_Extensions.h"
#include "SystemConfig.h"
#ifndef HBL_PILOT_NUM_DOORLOCKEM
    #define HBL_PILOT_NUM_DOORLOCKEM                0
#endif

#if (HBL_PILOT_NUM_DOORLOCKEM > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#define DOORLOCKEM_PROCESSED_KEEP_PIN_ON    (0x80)
#define DOORLOCKEM_PROCESSED_KEEP_PIN_MASK  (0xF0)

#define DOORLOCKEM_REGISTER      DoorLockEMPilot__Initialize,         	\
                                 DoorLockEMPilot__Allocate,           	\
                                 DoorLockEMPilot__InitializeInstance, 	\
                                 DoorLockEMPilot__Request,            	\
                                 DoorLockEMPilot__Process,            	\
                                 DoorLockEMPilot__GetRequested,         \
                                 DoorLockEMPilot__GetProcessed,       	\
                                 NULL,         	                        \
                                 DoorLockEMPilot__AsynchProcess,        \
                                 NULL,                                  \
                                 DoorLockEMPilot__Handler1ms,           \
                                 DoorLockEMPilot__GetStatus,            \
                                 1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void DoorLockEMPilot__Initialize(void);
uint8 DoorLockEMPilot__Allocate(void);
PASS_FAIL_TYPE DoorLockEMPilot__InitializeInstance(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE DoorLockEMPilot__Request(HBL_LOAD_TYPE * load,void * data);
PASS_FAIL_TYPE DoorLockEMPilot__Process(HBL_LOAD_TYPE * load);
void * DoorLockEMPilot__GetRequested(HBL_LOAD_TYPE * load);
void * DoorLockEMPilot__GetProcessed(HBL_LOAD_TYPE * load);
void * DoorLockEMPilot__GetStatus(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE DoorLockEMPilot__Handler1ms(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE DoorLockEMPilot__AsynchProcess(HBL_LOAD_TYPE * load);
void DoorLockEMPilot__Handler(void);

#endif

#endif // DOORLOCKEMPILOT_H_

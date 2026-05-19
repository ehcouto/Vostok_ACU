/**
 *  @file       SRDoorLockEMPilot.h
 *
 *  @brief      Interface for Pilot Handler for EM DoorLock device with Class B requirements
 *
 *  @defgroup   CLASS_B
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
#ifndef SRDOORLOCKEMPILOT_H_
#define SRDOORLOCKEMPILOT_H_

#include "C_Extensions.h"
#include "SRSystemConfig.h"
#ifndef HBL_PILOT_NUM_SRDOORLOCKEM
    #define HBL_PILOT_NUM_SRDOORLOCKEM                0
#endif

#if (HBL_PILOT_NUM_SRDOORLOCKEM > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#define SRDOORLOCKEM_PROCESSED_KEEP_PIN_ON    (0x80)
#define SRDOORLOCKEM_PROCESSED_KEEP_PIN_MASK  (0xF0)

#define SRDOORLOCKEM_REGISTER    SRDoorLockEMPilot__Initialize,         	\
                                 SRDoorLockEMPilot__Allocate,           	\
                                 SRDoorLockEMPilot__InitializeInstance, 	\
                                 SRDoorLockEMPilot__Request,            	\
                                 SRDoorLockEMPilot__Process,            	\
                                 SRDoorLockEMPilot__GetRequested,           \
                                 SRDoorLockEMPilot__GetProcessed,       	\
                                 NULL,         	                            \
                                 SRDoorLockEMPilot__AsynchProcess,          \
                                 NULL,                                      \
                                 SRDoorLockEMPilot__Handler1ms,             \
                                 SRDoorLockEMPilot__GetStatus,              \
                                 1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SRDoorLockEMPilot__Initialize(void);
uint8 SRDoorLockEMPilot__Allocate(void);
PASS_FAIL_TYPE SRDoorLockEMPilot__InitializeInstance(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE SRDoorLockEMPilot__Request(HBL_LOAD_TYPE * load,void * data);
PASS_FAIL_TYPE SRDoorLockEMPilot__Process(HBL_LOAD_TYPE * load);
void * SRDoorLockEMPilot__GetRequested(HBL_LOAD_TYPE * load);
void * SRDoorLockEMPilot__GetProcessed(HBL_LOAD_TYPE * load);
void * SRDoorLockEMPilot__GetStatus(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE SRDoorLockEMPilot__Handler1ms(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE SRDoorLockEMPilot__AsynchProcess(HBL_LOAD_TYPE * load);
void SRDoorLockEMPilot__Handler(void);

#endif

#endif // SRDOORLOCKEMPILOT_H_

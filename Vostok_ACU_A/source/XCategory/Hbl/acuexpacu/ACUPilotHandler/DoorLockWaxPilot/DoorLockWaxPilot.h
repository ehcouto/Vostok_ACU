/**
 *  @file       DoorLockWaxPilot.h
 *
 *  @brief      Interface for Pilot Handler Wax DoorLock device
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
#ifndef DOORLOCKWAXPILOT_H_
#define DOORLOCKWAXPILOT_H_

#include "C_Extensions.h"
#include "SystemConfig.h"
#ifndef HBL_PILOT_NUM_DOORLOCKWAX
    #define HBL_PILOT_NUM_DOORLOCKWAX                0
#endif

#if (HBL_PILOT_NUM_DOORLOCKWAX > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#define DOORLOCKWAX_REGISTER     DoorLockWaxPilot__Initialize,         	\
                                 DoorLockWaxPilot__Allocate,           	\
                                 DoorLockWaxPilot__InitializeInstance, 	\
                                 DoorLockWaxPilot__Request,            	\
                                 DoorLockWaxPilot__Process,            	\
                                 DoorLockWaxPilot__GetRequested,        \
                                 DoorLockWaxPilot__GetProcessed,       	\
                                 NULL,                                  \
                                 DoorLockWaxPilot__AsynchProcess,     	\
                                 NULL,                                  \
                                 DoorLockWaxPilot__Handler1ms,          \
                                 DoorLockWaxPilot__GetStatus,           \
                                 1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void DoorLockWaxPilot__Initialize(void);
uint8 DoorLockWaxPilot__Allocate(void);
PASS_FAIL_TYPE DoorLockWaxPilot__InitializeInstance(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE DoorLockWaxPilot__Request(HBL_LOAD_TYPE * load,void * data);
PASS_FAIL_TYPE DoorLockWaxPilot__Process(HBL_LOAD_TYPE * load);
void * DoorLockWaxPilot__GetRequested(HBL_LOAD_TYPE * load);
void * DoorLockWaxPilot__GetProcessed(HBL_LOAD_TYPE * load);
void * DoorLockWaxPilot__GetStatus(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE DoorLockWaxPilot__AsynchProcess(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE DoorLockWaxPilot__Handler1ms(HBL_LOAD_TYPE * load);
void DoorLockWaxPilot__Handler();
#endif

#endif // DOORLOCKWAXPILOT_H_

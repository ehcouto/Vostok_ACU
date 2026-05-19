/**
 *  @file       SRDoorLockWaxPilot.h
 *
 *  @brief      Interface for Pilot Handler Wax DoorLock device with Class B requirements
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
#ifndef SRDOORLOCKWAXPILOT_H_
#define SRDOORLOCKWAXPILOT_H_

#include "C_Extensions.h"
#include "SRSystemConfig.h"
#ifndef HBL_PILOT_NUM_SRDOORLOCKWAX
    #define HBL_PILOT_NUM_SRDOORLOCKWAX                0
#endif

#if (HBL_PILOT_NUM_SRDOORLOCKWAX > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#define SRDOORLOCKWAX_REGISTER   SRDoorLockWaxPilot__Initialize,         	\
                                 SRDoorLockWaxPilot__Allocate,           	\
                                 SRDoorLockWaxPilot__InitializeInstance, 	\
                                 SRDoorLockWaxPilot__Request,            	\
                                 SRDoorLockWaxPilot__Process,            	\
                                 SRDoorLockWaxPilot__GetRequested,          \
                                 SRDoorLockWaxPilot__GetProcessed,       	\
                                 NULL,                                      \
                                 SRDoorLockWaxPilot__AsynchProcess,     	\
                                 NULL,                                      \
                                 SRDoorLockWaxPilot__Handler1ms,            \
                                 SRDoorLockWaxPilot__GetStatus,             \
                                 1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SRDoorLockWaxPilot__Initialize(void);
uint8 SRDoorLockWaxPilot__Allocate(void);
PASS_FAIL_TYPE SRDoorLockWaxPilot__InitializeInstance(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE SRDoorLockWaxPilot__Request(HBL_LOAD_TYPE * load,void * data);
PASS_FAIL_TYPE SRDoorLockWaxPilot__Process(HBL_LOAD_TYPE * load);
void * SRDoorLockWaxPilot__GetRequested(HBL_LOAD_TYPE * load);
void * SRDoorLockWaxPilot__GetProcessed(HBL_LOAD_TYPE * load);
void * SRDoorLockWaxPilot__GetStatus(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE SRDoorLockWaxPilot__AsynchProcess(HBL_LOAD_TYPE * load);
PASS_FAIL_TYPE SRDoorLockWaxPilot__Handler1ms(HBL_LOAD_TYPE * load);
void SRDoorLockWaxPilot__Handler();
#endif

#endif // SRDOORLOCKWAXPILOT_H_

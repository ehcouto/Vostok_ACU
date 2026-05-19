/**
 *  @file       SRProductMgr.h
 *
 *  @brief      Class B Product Safety common module - header file
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *	@defgroup	CLASS_B
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
  */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRPRODUCTMGR_H_
#define SRPRODUCTMGR_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#include "SRProductMgr_Defs.h"
#include "SRProductMgr_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SRProductMgr__Initialize(void);
void SRProductMgr__Handler(void);
void SRProductMgr__ResetRequestPriority(void);
void SRProductMgr__IssueSRCheckCmd(SRPRODUCTMGR_CHECK_CMD_TYPE cmd);
PASS_FAIL_TYPE SRProductMgr__GetSRCheckStatus(uint8 index, SRPRODUCTMGR_CHECK_STATUS_TYPE *status);
SRPRODUCTMGR_STATUS_TYPE SRProductMgr__GetSRCheckStatusByFaultID(uint8 fault_id);
uint16 SRProductMgr__GetSRCheckTimer(uint8 index);

SRPRODUCTMGR_ACTION_TYPE SRProductMgr__GetSRActionStatus(uint8 index);
PASS_FAIL_TYPE SRProductMgr__StartSRAction(uint8 index, uint8 priority);
PASS_FAIL_TYPE SRProductMgr__StopSRAction(uint8 index, uint8 priority);

#endif // SRPRODUCTMGR_H_

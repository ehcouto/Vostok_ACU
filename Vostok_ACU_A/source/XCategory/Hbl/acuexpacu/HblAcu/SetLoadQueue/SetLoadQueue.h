/**
 *  @file       SetLoadQueue.c
 *
 *  @brief      Module where API005 Set Load requests are stored and executed on Application layer
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2018-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SETLOADQUEUE_H_
#define SETLOADQUEUE_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#include "SystemConfig.h"
#include "Hbl.h"


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SetLoadQueue__Initialize(void);
void SetLoadQueue__Handler(void);
PASS_FAIL_TYPE SetLoadQueue__SetLoadRequestByIndex(uint8 load_index, HBL_PRIORITY_TYPE priority, uint8 * data);
PASS_FAIL_TYPE SetLoadQueue__SetLoadRequestByLoadID(HBL_LOAD_ENUM_TYPE load_id, HBL_PRIORITY_TYPE priority, uint8 * data);

#endif // SETLOADQUEUE_H_

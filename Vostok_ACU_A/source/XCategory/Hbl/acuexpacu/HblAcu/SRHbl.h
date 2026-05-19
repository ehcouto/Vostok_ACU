/**
 *  @file       
 *
 *  @brief      SRHbl interface to Set Loads in SRChecks / Actions and to SRMain module
 *
 *  @defgroup CLASS_B
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRHBL_H_
#define SRHBL_H_
#include "C_Extensions.h"
#include "HblDefs.h"
#include "SRHbl_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
PASS_FAIL_TYPE SRHbl__SetLoadRequestByIndex(uint8 load_index, HBL_PRIORITY_TYPE priority, uint8 * data);
PASS_FAIL_TYPE SRHbl__SetLoadRequestByLoadID(HBL_LOAD_ENUM_TYPE load_id, HBL_PRIORITY_TYPE priority, uint8 * data);
void SRHbl__Process(void);
void SRHbl__ResetLoadPriority(void);
#endif // SRHBL_H_



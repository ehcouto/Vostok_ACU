/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 * $Header: $
 *
 *  @copyright  Copyright 2017. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 *
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef TIMEOUT_H_
#define TIMEOUT_H_
#include "Timeout_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void Timeout__Initialize(void);
void Timeout__SetTime(TIMEOUT_LABEL tim, uint16 time_ms);
uint16 Timeout__GetCount(void);
BOOL_TYPE Timeout__GetStatus(TIMEOUT_LABEL tim);

#endif // TIMEOUT_H_



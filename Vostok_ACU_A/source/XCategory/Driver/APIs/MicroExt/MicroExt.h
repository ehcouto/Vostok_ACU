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
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef MICROEXT_H_
#define MICROEXT_H_
#include "MicroExt_prm.h"
#include "MicroExt_Defs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void MicroExt__Initialize(void);
void MicroExt__ResetAll(void);
BOOL_TYPE MicroExt__JumptoApplication(void);

#endif // MICROEXT_H_



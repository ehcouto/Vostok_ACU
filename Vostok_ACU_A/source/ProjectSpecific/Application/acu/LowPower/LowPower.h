/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef _LOWPOWER_H_
#define _LOWPOWER_H_
#include "C_Extensions.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void LowPower__Initialize(void);
BOOL_TYPE LowPower__IsWakeupValid(void);
void LowPower__Timer16IrqHandler(void);

#endif // SOURCE_PROJECTSPECIFIC_APPLICATION_LOWPOWER_LOWPOWER_H_



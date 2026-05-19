/**
 *  @file       
 *
 *  @brief      Basic View Header File description
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2022-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef _ACTIVEPOWER_H_
#define _ACTIVEPOWER_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#include "MotorTypeDefs.h"
#include "Mci.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void ActivePower__Initialize(APL_WASH_MOTOR_TYPE wash_motor);
void ActivePower__FastHandler(void);
sint32 ActivePower__GetData(MCI_AI_TYPE read_data);
BOOL_TYPE ActivePower__NewFactor(uint16 new_factor);

#endif //_ACTIVEPOWER_H_


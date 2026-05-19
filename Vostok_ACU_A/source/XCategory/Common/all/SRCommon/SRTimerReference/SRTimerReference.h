/**
 *  @file       SRTimerReference.h
 *
 *  @brief      Class B Timing reference module header
 *
 *  @defgroup   CLASS_B
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRTIMERREFERENCE_H_
#define SRTIMERREFERENCE_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SRTimerReference__Initialize(void);
void SRTimerReference__Handler1ms(void);
uint32 SRTimerReference__GetTimer(void);
#endif // SRTIMERREFERENCE_H_

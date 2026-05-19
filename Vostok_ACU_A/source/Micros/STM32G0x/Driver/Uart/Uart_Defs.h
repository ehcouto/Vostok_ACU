/**
 *  @file
 *  @brief      Contains controller specific definition for STM32F10x
 *
 *
 *  $Header: Uart_Defs.h 1.1 2015/04/23 19:46:54EDT Dasar Hanumanth (DASARHB) Exp  $
 *
 *   @copyright  Copyright 2007-2012-$Date: 2015/04/23 19:46:54EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#ifndef UART_DEFS_H_
#define UART_DEFS_H_
#include "uc.h"

//=====================================================================================================================
//-------------------------------------- ISR Prototypes----------------------------------------------------------------
//=====================================================================================================================
//=====================================================================================================================
void Uart__Interrupt1(void);
void Uart__Interrupt2(void);
void Uart__Interrupt3_4_5_6(void);

#endif // UART_DEFS_H_

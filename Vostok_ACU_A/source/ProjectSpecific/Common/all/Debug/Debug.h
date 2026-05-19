/**
 *  @file       
 *
 *  @brief      Pulbic interface for Debug module.
 *
 *  @copyright  Copyright 2015-2017 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef DEBUG_H_
#define DEBUG_H_

#include "SystemConfig.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void Debug__Initialize(void);
void Debug__Handler(void);
//void Debug__Set(uint8 id);
//void Debug__Clear(uint8 id);

void Debug__Func00(void);
void Debug__Func01(void);
void Debug__Func02(void);
void Debug__Func03(void);
void Debug__Func04(void);
void Debug__Func05(void);
void Debug__Func06(void);
void Debug__Func07(void);

#endif // DEBUG_H_

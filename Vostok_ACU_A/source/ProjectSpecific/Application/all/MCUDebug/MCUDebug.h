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
#ifndef SOURCE_PROJECTSPECIFIC_APPLICATION_ALL_MCUDEBUG_MCUDEBUG_H_
#define SOURCE_PROJECTSPECIFIC_APPLICATION_ALL_MCUDEBUG_MCUDEBUG_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//#define TIME_MEASUREMENT
#include "TimeCapture.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void MCUDebug__Initialize(void);
void MCUDebug_ZeroCross(void);
void MCUDebug_SlowHandler(void);
void MCUDebug_MCHandler(void);
void MCUDebug_TriacHandler(void);

#ifdef TIME_MEASUREMENT
void MCUDebug__MeasTimeSlot(void);
void MCUDebug__MeasTimeReset(void);
void MCUDebug__MeasTimeSet(void);
#endif
#endif // SOURCE_PROJECTSPECIFIC_APPLICATION_ALL_MCUDEBUG_MCUDEBUG_H_


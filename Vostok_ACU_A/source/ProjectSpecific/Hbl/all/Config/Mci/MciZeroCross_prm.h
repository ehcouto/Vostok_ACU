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
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef MCI_MCIZEROCROSS_PRM_H_
#define MCI_MCIZEROCROSS_PRM_H_

#include "C_Extensions.h"
#include "Mci.h"
#include "TimeCapture.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#define MCI_ZEROCROSSING_EVENT(event,value)     Mci__ZeroCrossHandler(event,value)
#define GET_TIMER_CNT()                         TimeCapture__ReadTimerCnt(READ_TIMER_AND_RESET)

#define PORT_MCI_ZEROCROSS     PORTC
#define PIN_MCI_ZEROCROSS      11
#define TYPE_MCI_ZEROCROSS     INPUT_PULLDOWN

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

//#define MCI_POOLING_ZC_HW_SIGNAL

#endif // MCI_MCIZEROCROSS_PRM_H_

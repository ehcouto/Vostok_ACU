/**
 *  @file
 *  @brief      Header file for time capture driver module.
 *
 * $Revision: 1.7 $
 *
 * Copyright 2010 Whirlpool Corporation. All rights reserved - CONFIDENTIAL.
 */
#ifndef TIME_CAPTURE_H
    #define TIME_CAPTURE_H

#include "TimeCapture_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//! Time capture inputs capture number definition
//! Enumeration contain maximal number of module inputs
//! ICxx are located on specific timer and timer channel
//! e.g. IC00 in stm32 is located on timer 1 channel 1
typedef enum
{
    IC00,
    IC01,
    IC02,
    IC03,
    IC04,
    IC05,
    IC06,
    IC07,
    IC08,
    IC09,
    IC10,
    IC11,
    IC12,
    IC13,
    IC14,
    IC15
} TIME_CAPTURE_INPUT_TYPE;

//! Types of modes supported by timer channels.
typedef enum
{
    CHANNEL_CAPTURE_RISING_EDGE,
    CHANNEL_CAPTURE_FALLING_EDGE,
    CHANNEL_CAPTURE_ANY_EDGE
} CHANNEL_EVENT_TYPE;

//! Types of capture events
typedef enum
{
    TC_FALLING_EDGE_EVENT,
    TC_RISING_EDGE_EVENT,
    TC_UNKNOWN_EVENT
} TC_EVENT_TYPE;


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void TimeCapture__Initialize(void);
void TimeCapture__ChannelConfig(TIME_CAPTURE_INPUT_TYPE input_number,CHANNEL_EVENT_TYPE event, uint32 frequency);
void TimeCapture__EnableEventInterrupt(TIME_CAPTURE_INPUT_TYPE input_number);
void TimeCapture__EnablePostEventInterrupt(TIME_CAPTURE_INPUT_TYPE input_number);
void TimeCapture__Start(TIME_CAPTURE_INPUT_TYPE input_number);
void TimeCapture__Stop( TIME_CAPTURE_INPUT_TYPE input_number);
uint16  TimeCapture__ReadEventTime( TIME_CAPTURE_INPUT_TYPE input_number);
uint16  TimeCapture__ReadTimerCnt( TIME_CAPTURE_INPUT_TYPE input_number);
uint32  TimeCapture__GetTimerTickFreq( TIME_CAPTURE_INPUT_TYPE input_number);
void TimeCapture__SetPostEventInterruptDelay(TIME_CAPTURE_INPUT_TYPE input_number, uint16 delay_ticks);
void TimeCapture__InterruptHandler(void);

#endif // TIME_CAPTURE_H



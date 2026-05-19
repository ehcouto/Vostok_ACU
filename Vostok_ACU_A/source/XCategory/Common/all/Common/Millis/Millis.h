/**
 * @file
 * @brief       Public interface to the Millis module.
 *
 * @details     Refer to the Millis.c source file for more detailed information.
 *
 * @copyright   Copyright 2021 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef MILLIS_H_
#define MILLIS_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================

// -- Public Enumerated Constant Declarations --


// -- Public Type Declarations --

//! Structure to hold HMS clock time
typedef struct MILLIS_HMS_STRUCT
{
    uint16 Milliseconds : 10;
    uint16 Hours        : 6;
    uint8 Minutes;
    uint8 Seconds;
} MILLIS_HMS_TYPE;

typedef struct MILLIS_TIMER_STRUCT
{
    uint32 Reference    : 30;
    uint32 Running      : 1;
    uint32 Done      : 1;
} MILLIS_TIMER_TYPE;


//=====================================================================================================================
//  --- Public Functions ----------------------------------------------------------------------------------------------
//=====================================================================================================================


void Millis__Initialize(void);
BOOL_TYPE Millis__InitComplete(void);
void Millis__Handler1ms(void);
void Millis__SlowHandler(void);
uint32 Millis__GetFreeRunningCounter(void);
void Millis__Reset(MILLIS_TIMER_TYPE* timer);
BOOL_TYPE Millis__IsTimerPaused(MILLIS_TIMER_TYPE* timer);
BOOL_TYPE Millis__IsTimerRunning(MILLIS_TIMER_TYPE* timer);
void Millis__SetElapsed(MILLIS_TIMER_TYPE* timer, uint32 milliseconds);
void Millis__SetElapsedSeconds(MILLIS_TIMER_TYPE* timer, uint32 seconds);
void Millis__SetElapsedHms(MILLIS_TIMER_TYPE* timer, MILLIS_HMS_TYPE hms);
void Millis__Start(MILLIS_TIMER_TYPE* timer);
void Millis__Pause(MILLIS_TIMER_TYPE* timer);
void Millis__Resume(MILLIS_TIMER_TYPE* timer);
uint32 Millis__GetElapsed(MILLIS_TIMER_TYPE* timer);
uint32 Millis__GetElapsedSeconds(MILLIS_TIMER_TYPE* timer);
MILLIS_HMS_TYPE Millis__GetElapsedHms(MILLIS_TIMER_TYPE* timer);
uint32 Millis__GetRemaining(MILLIS_TIMER_TYPE* timer, uint32 target_milliseconds);
uint32 Millis__GetRemainingSeconds(MILLIS_TIMER_TYPE* timer, uint32 target_seconds);
MILLIS_HMS_TYPE Millis__GetRemainingHms(MILLIS_TIMER_TYPE* timer, MILLIS_HMS_TYPE target_hms);
BOOL_TYPE Millis__ReachedTarget(MILLIS_TIMER_TYPE* timer, uint32 target_milliseconds);
BOOL_TYPE Millis__ReachedTargetSeconds(MILLIS_TIMER_TYPE* timer, uint32 target_seconds);
BOOL_TYPE Millis__ReachedTargetHms(MILLIS_TIMER_TYPE* timer, MILLIS_HMS_TYPE target_hms);
void Millis__RestartPeriod(MILLIS_TIMER_TYPE* timer, uint32 milliseconds);
void Millis__RestartSecondsPeriod(MILLIS_TIMER_TYPE* timer, uint32 seconds);
void Millis__RestartHmsPeriod(MILLIS_TIMER_TYPE* timer, MILLIS_HMS_TYPE hms);


#endif      // MILLIS_H_

/**
 *  @file       
 *
 *  @brief      SystemTimers API
 *              Added NVTimers which allow Non Volatile Timers in the system;
 *
 *
 *  @copyright  Copyright 2015-2017. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SYSTEMTIMERS_H_
#define SYSTEMTIMERS_H_

#include "SystemConfig.h"

#if (SYSTEM_TIMERS_FEATURE == ENABLED)
#include "C_Extensions.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef enum
{
    SYSTEMTIMERS_TYPE_DOWN_COUNTER = 0U,
    SYSTEMTIMERS_TYPE_UP_COUNTER = 1U,

	SYSTEMTIMERS_TYPE_N_DOWN_COUNTER = (uint8) ~0U,
	SYSTEMTIMERS_TYPE_N_UP_COUNTER   = (uint8) ~1U,

} SYSTEMTIMERS_TYPE_TYPE;


typedef enum
{
    SYSTEMTIMERS_STATE_STOPPED = 0U,
    SYSTEMTIMERS_STATE_RUNNING = 1U,
    SYSTEMTIMERS_STATE_DONE    = 2U,
    SYSTEMTIMERS_STATE_PAUSED  = 3U,
    SYSTEMTIMERS_STATE_INVALID = 4U,


	SYSTEMTIMERS_STATE_N_STOPPED = (uint8) ~0U,
	SYSTEMTIMERS_STATE_N_RUNNING = (uint8) ~1U,
	SYSTEMTIMERS_STATE_N_DONE    = (uint8) ~2U,
	SYSTEMTIMERS_STATE_N_PAUSED  = (uint8) ~3U,
	SYSTEMTIMERS_STATE_N_INVALID = (uint8) ~4U,
} SYSTEMTIMERS_STATE_TYPE;

PACKED typedef struct
{
    uint16 milliseconds;
    uint8 hours;
    uint8 minutes;
    uint8 seconds;
    uint8 pad;
} SYSTEMTIMER_HMS_TYPE;



PACKED typedef struct
{
	uint32 reference;
	uint32 pause_value;
	uint32 compare;
	uint32 last_handle_counter;

	SYSTEMTIMERS_STATE_TYPE state;
	SYSTEMTIMERS_STATE_TYPE n_state;
	SYSTEMTIMERS_TYPE_TYPE type;
	SYSTEMTIMERS_TYPE_TYPE n_type;

}SYSTEMTIMER_NV_TIMER_TYPE;


#define SYSTEMTIMERS_FAIL     0xFFFF

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SystemTimers__Initialize(void);
void SystemTimers__Handler1ms(void);
uint32 SystemTimers__GetFreeRunningCounter(void);

uint16 SystemTimers__GetHandle(SYSTEMTIMERS_TYPE_TYPE type);
PASS_FAIL_TYPE SystemTimers__ReleaseHandle(uint16 handle);

PASS_FAIL_TYPE SystemTimers__SetMs(uint16 handle, uint32 reference_ms);
PASS_FAIL_TYPE SystemTimers__SetHMS(uint16 handle, SYSTEMTIMER_HMS_TYPE * time);
PASS_FAIL_TYPE SystemTimers__Reset(uint16 handle);
PASS_FAIL_TYPE SystemTimers__Pause(uint16 handle);
PASS_FAIL_TYPE SystemTimers__Resume(uint16 handle);


uint32 SystemTimers__GetMs(uint16 handle);
PASS_FAIL_TYPE SystemTimers__GetHMS(uint16 handle, SYSTEMTIMER_HMS_TYPE * time);

SYSTEMTIMERS_STATE_TYPE SystemTimers__GetState(uint16 handle);
BOOL_TYPE SystemTimers__GetEvent(uint16 handle);

/***************************************************************************************
 * NVTimer Methods
 ***************************************************************************************/

void SystemTimers__NVResetTimer(SYSTEMTIMER_NV_TIMER_TYPE * nv_timer);
void SystemTimers__NVRestore(SYSTEMTIMER_NV_TIMER_TYPE * nv_timer);
PASS_FAIL_TYPE SystemTimers__NVSetTimerMS(SYSTEMTIMER_NV_TIMER_TYPE * nv_timer, uint32 ms_duration, SYSTEMTIMERS_TYPE_TYPE type);
PASS_FAIL_TYPE SystemTimers__NVSetTimerHMS(SYSTEMTIMER_NV_TIMER_TYPE * nv_timer, SYSTEMTIMER_HMS_TYPE * time, SYSTEMTIMERS_TYPE_TYPE type);
SYSTEMTIMERS_STATE_TYPE SystemTimers__NVCheckTimer(SYSTEMTIMER_NV_TIMER_TYPE * nv_timer);
PASS_FAIL_TYPE SystemTimers__NVPauseTimer(SYSTEMTIMER_NV_TIMER_TYPE * nv_timer);
PASS_FAIL_TYPE SystemTimers__NVResumeTimer(SYSTEMTIMER_NV_TIMER_TYPE * nv_timer);

uint32 SystemTimers__NVGetTimerMs(SYSTEMTIMER_NV_TIMER_TYPE * nv_timer);
PASS_FAIL_TYPE SystemTimers__NVGetTimerHMS(SYSTEMTIMER_NV_TIMER_TYPE * nv_timer, SYSTEMTIMER_HMS_TYPE * time);

#endif  // (SYSTEM_TIMERS_FEATURE == ENABLED)

#endif // SYSTEMTIMERS_H_

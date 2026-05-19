/**
 * @file
 * @brief       A module to create and manage timers
 *
 * @details     Each timer can measure up to about 12.4 days.
 *              Non-volatile timers can be used by storing the timer data structure in non-volatile
 *              memory.
 *
 *              Millis automatically supports non-volatile timers on ACU project types. For Millis
 *              to support non-volatile timers on other project types, the following macros must be
 *              defined.
 *                  RESTORE_INIT()
 *                  RESTORE_HANDLER()
 *                  STORE_HANDLER()
 *              Refer to the Millis_prv.h file for details.
 *
 *              Usage Example 1: Measure seconds
 *                  MILLIS_TIMER_TYPE my_timer;
 *                  Millis__Start(&my_timer);
 *                  ...
 *                  uint32 Seconds = Millis__ElapsedSeconds(&my_timer);
 *
 *              Usage Example 2: Check for a timeout
 *                  MILLIS_TIMER_TYPE my_timer;
 *                  Millis__Start(&my_timer);
 *                  ...
 *                  if (Millis__ReachedTargetSeconds(&my_timer, TARGET_SECONDS) == TRUE)
 *                  {
 *                      // Timer reached TARGET_SECONDS.
 *                  }
 *
 *              Usage Example 3: Pause & resume timer
 *                  MILLIS_TIMER_TYPE my_timer;
 *                  Millis__Start(&my_timer);
 *                  ...
 *                  Millis__Pause(&my_timer);
 *                  ...
 *                  Millis__Resume(&my_timer);
 *
 *              Usage Example 4: Reset timer
 *                  MILLIS_TIMER_TYPE my_timer;
 *                  Millis__Reset(&my_timer);
 *
 *              Usage Example 5: Restore a non-volatile timer (on non-ACU project types)
 *                  MILLIS_TIMER_TYPE My_Timer;     // Definition of non-volatile timer RAM
 *                  // Arrange to call this function once after non-volatile data is restored.
 *                  void Module__NVDataIsRestored(void)
 *                  {
 *                      Millis__RestoreTimer(&My_Timer);
 *                  }
 *
 *              Millis calls functions for SystemTimers module. When SystemTimers module is deprecated,
 *              it will be removed from this module and replaced by Millis.
 *
 * @copyright   Copyright 2021 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "SystemConfig.h"
#include "Millis.h"
#include "Millis_prv.h"
#if (SYSTEM_TIMERS_FEATURE == ENABLED)
#include "SystemTimers.h"
#endif

// -- Pre-compiler Directives --
#ifndef MILLIS_SUPPORT_NV_TIMERS
    //! If not explicitly specified, then do not support nonvolatile timers
    #define MILLIS_SUPPORT_NV_TIMERS DISABLED
#endif

// -- Other Modules --
#if ((MILLIS_SUPPORT_NV_TIMERS == ENABLED) && (SYSTEM_ACU_HANDLING == DISABLED))
    CT_ASSERT(API007DATA_BULK_DATA_FEATURE == ENABLED);
    #include "Micro.h"
    #include "Mode.h"
#endif

//  --- Private Declarations ------------------------------------------------------------------------------------------


// -- Private Constant Declarations --

//! A mask to limit the Milliseconds free-running counter to 30 bits.
#define MILLISECONDS_MASK 0x3FFFFFFF


// -- Private Enumerated Constant Declarations --


// -- Private Structure Type Declarations --


// -- Private Macro Declarations --

#ifndef RESTORE_HANDLER
#define RESTORE_HANDLER()
#endif

#ifndef RESTORE_TIMER
#define RESTORE_TIMER(timer)
#endif

#ifndef STORE_HANDLER
#define STORE_HANDLER()
#endif


//  --- Private Definitions -------------------------------------------------------------------------------------------


// -- Private Flash-Based Constant Definitions --


// -- Private Variable Definitions --

#if ((MILLIS_SUPPORT_NV_TIMERS == ENABLED) && (SYSTEM_ACU_HANDLING == DISABLED))
    //! Flag indicates whether the Millis module is initialized and ready to run timers.
    static BOOL_TYPE Init_Complete;

    //! A timer used to periodically store the Milliseconds value through API007.
    static MILLIS_TIMER_TYPE Millis_Store_Timer;
#endif


//  --- Private Function Prototypes -----------------------------------------------------------------------------------

static uint32 ConvertHmsToMilliseconds(MILLIS_HMS_TYPE hms);
static MILLIS_HMS_TYPE ConvertMillisecondsToHms(uint32 milliseconds);


//=====================================================================================================================
//  --- Public Functions ----------------------------------------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * Restore the Milliseconds value from non-volatile memory.
 * On the ACU there are two possibilities after a reset:
 *  1. NVData recovered RAM variables
 *     In this case, the Milliseconds variable is restored to its value before the reset and
 *     it is ready for use. Also, all the non-volatile timers are restored and ready for use.
 *  2. NVData was unable to recover RAM variables
 *     In this case, the non-volatile timers were not restored either. Therefore, the value of
 *     the Milliseconds variable is probably 0 from the initialization firmware; but the value
 *     is actually unimportant. Any starting value will work since there are no active timers.
 *
 * On any non-ACU project type, the method of restoring the Milliseconds value must be managed
 * through the Millis_prv.h file using the following macros:
 *      RESTORE_HANDLER()
 *      INIT_COMPLETE()
 *      STORE_HANDLER()
 */
void Millis__Initialize(void)
{
    #if ((MILLIS_SUPPORT_NV_TIMERS == ENABLED) && (SYSTEM_ACU_HANDLING == DISABLED))
        if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
        {
            Init_Complete = FALSE;
            Millis__Reset(&Millis_Store_Timer);
            Milliseconds = 0;
        }
    #endif

    #if (SYSTEM_TIMERS_FEATURE == ENABLED)
        SystemTimers__Initialize();
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Determine whether the Millis module is ready to run timers.
 * @return TRUE if Millis module is ready to run timers; otherwise FALSE.
 */
BOOL_TYPE Millis__InitComplete(void)
{
    #if ((MILLIS_SUPPORT_NV_TIMERS == ENABLED) && (SYSTEM_ACU_HANDLING == DISABLED))
        return Init_Complete;
    #else
        return TRUE;
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Update the free-running millisecond counter. The 1ms handler in SystemTimers module is called if the feature is enabled.
 * This function must be called once every millisecond.
 */
void Millis__Handler1ms(void)
{
    Milliseconds++;

    #if (SYSTEM_TIMERS_FEATURE == ENABLED)
        SystemTimers__Handler1ms();
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Takes care of store(requirement specifies when the value has to be stored, i.e. each second or each minute)
 * and restore (after a power failure) action of the timer reference
 * This function should be called from the lowest priority thread.
 */
void Millis__SlowHandler(void)
{
    #if ((MILLIS_SUPPORT_NV_TIMERS == ENABLED) && (SYSTEM_ACU_HANDLING == DISABLED))
        if (Init_Complete)
        {
            if (Millis__ReachedTargetSeconds(&Millis_Store_Timer, MILLIS_SAVE_PERIOD_SECONDS))
            {
                Stored_Milliseconds = Milliseconds;
                // Prepare for the next update
                Millis__RestartSecondsPeriod(&Millis_Store_Timer, MILLIS_SAVE_PERIOD_SECONDS);
            }
        }
        else
        {
            if ((Mode__GetMode() == MODE_NORMAL) &&
                (Mode__GetSubMode() == SUBMODE_NORMAL_SUPERVISOR) &&
                (IS_MILLIS_DATA_RESTORED() == TRUE))
            {
                Milliseconds = Stored_Milliseconds;
                Init_Complete = TRUE;
            }
            else
            {
                if (Milliseconds >= MILLIS_START_UP_TIMEOUT)
                {
                    Init_Complete = TRUE;
                }
            }

            if (Init_Complete)
            {
                Millis__Start(&Millis_Store_Timer);
                Stored_Milliseconds = Milliseconds;   // store Milliseconds value in case of power interruption before 60 seconds since Power On
            }
        }
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Returns the current value of the 32-bit free-running counter.
 * @return the current value of the free-running 32-bit millisecond counter
 */
uint32 Millis__GetFreeRunningCounter(void)
{
    return Milliseconds;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Reset a timer.
 * @param timer = The timer to reset
 */
void Millis__Reset(MILLIS_TIMER_TYPE* timer)
{
    timer->Reference = 0;
    timer->Running = FALSE;
    timer->Done = FALSE;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Determine whether or not the timer is paused.
 * @param timer = The timer to check
 * @return TRUE if the timer is running; otherwise FALSE.
 */
BOOL_TYPE Millis__IsTimerPaused(MILLIS_TIMER_TYPE* timer)
{
    return (BOOL_TYPE)((timer == NULL) || (timer->Running == FALSE));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Determine whether or not the timer is running.
 * @param timer = The timer to check
 * @return TRUE if the timer is running; otherwise FALSE.
 */
BOOL_TYPE Millis__IsTimerRunning(MILLIS_TIMER_TYPE* timer)
{
    return (BOOL_TYPE)((timer != NULL) && (timer->Running));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Set the elapsed milliseconds on the specified timer.
 * @param timer = The timer to set
 * @param milliseconds = The elapsed milliseconds on the specified timer
 */
void Millis__SetElapsed(MILLIS_TIMER_TYPE* timer, uint32 milliseconds)
{
    if (timer != NULL)
    {
        if (timer->Running)
        {
            timer->Reference = Milliseconds - milliseconds;
        }
        else
        {
            timer->Reference = milliseconds;
        }
        timer->Done = FALSE;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Set the elapsed seconds on the specified timer.
 * @param timer = The timer to set
 * @param seconds = The elapsed seconds on the specified timer
 */
void Millis__SetElapsedSeconds(MILLIS_TIMER_TYPE* timer, uint32 seconds)
{
    Millis__SetElapsed(timer, seconds * 1000);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Set the elapsed HMS clock time on the specified timer.
 * @param timer = The timer to set
 * @param hms = The elapsed HMS clock time on the specified timer
 */
void Millis__SetElapsedHms(MILLIS_TIMER_TYPE* timer, MILLIS_HMS_TYPE hms)
{
    uint32 milliseconds = ConvertHmsToMilliseconds(hms);
    Millis__SetElapsed(timer, milliseconds);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Restart the specified timer from 0 milliseconds.
 * This is a short-cut function that effectively does the following:
 *      Millis__Pause(timer);
 *      Millis__SetElapsed(timer, 0);
 *      Millis__Resume(timer);
 * @param timer = The timer to restart
 */
void Millis__Start(MILLIS_TIMER_TYPE* timer)
{
    if (timer != NULL)
    {
        timer->Reference = Milliseconds;
        timer->Running = TRUE;
        timer->Done = FALSE;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Pause a running timer.
 * This function has no effect if the timer was already paused.
 * @param timer = The timer to pause
 */
void Millis__Pause(MILLIS_TIMER_TYPE* timer)
{
    if ((timer != NULL) && (timer->Running))
    {
        timer->Reference = Milliseconds - timer->Reference;
        timer->Running = FALSE;
        timer->Done = FALSE;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Resume a paused timer.
 * This function has no effect if the timer was already running.
 * @param timer = The timer to resume.
 */
void Millis__Resume(MILLIS_TIMER_TYPE* timer)
{
    if ((timer != NULL) && (timer->Running == FALSE))
    {
        timer->Reference = Milliseconds - timer->Reference;
        timer->Running = TRUE;
        timer->Done = FALSE;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the elapsed milliseconds on the specified timer.
 * @param timer = The timer to read
 * @return The elapsed milliseconds on the specified timer
 */
uint32 Millis__GetElapsed(MILLIS_TIMER_TYPE* timer)
{
    uint32 elapsed = 0;
    if (timer != NULL)
    {
        if (timer->Running)
        {
            elapsed = (Milliseconds - timer->Reference) & MILLISECONDS_MASK;
        }
        else
        {
            elapsed = timer->Reference;
        }
    }
    return elapsed;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the elapsed seconds on the specified timer.
 * @param timer = The timer to read
 * @return The elapsed seconds on the specified timer
 */
uint32 Millis__GetElapsedSeconds(MILLIS_TIMER_TYPE* timer)
{
    return Millis__GetElapsed(timer) / 1000;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the elapsed HMS clock time on the specified timer.
 * @param timer = The timer to read
 * @return The elapsed HMS clock time on the specified timer
 */
MILLIS_HMS_TYPE Millis__GetElapsedHms(MILLIS_TIMER_TYPE* timer)
{
    uint32 milliseconds = Millis__GetElapsed(timer);
    return ConvertMillisecondsToHms(milliseconds);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the remaining milliseconds before the specified timer reaches the specified target milliseconds.
 * @param timer = The timer to read
 * @param target_milliseconds = The target milliseconds
 * @return The remaining milliseconds to reach the target milliseconds.
 *         If the target was already reached, returns 0.
 */
uint32 Millis__GetRemaining(MILLIS_TIMER_TYPE* timer, uint32 target_milliseconds)
{
    uint32 remaining = 0;
    if (timer != NULL)
    {
        if(timer->Done == FALSE)
        {
            uint32 milliseconds = Millis__GetElapsed(timer);
            if (target_milliseconds > milliseconds)
            {
                remaining = target_milliseconds - milliseconds;
            }
            else
            {
                timer->Done = TRUE;
            }
        }
    }
    else
    {
        remaining = target_milliseconds;
    }

    return remaining;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the remaining seconds before the specified timer reaches the specified target seconds.
 * @param timer = The timer to read
 * @param target_seconds = The target seconds
 * @return The remaining seconds (rounded up) to reach the target seconds.
 *         Example: if there are 2.01 seconds remaining, the function will return 3.
 *         If the target was already reached, returns 0.
 */
uint32 Millis__GetRemainingSeconds(MILLIS_TIMER_TYPE* timer, uint32 target_seconds)
{
    uint32 remaining_milliseconds = Millis__GetRemaining(timer, target_seconds * 1000);
    return (remaining_milliseconds + 999) / 1000;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the remaining HMS clock time before the specified timer reaches the specified target HMS clock time.
 * @param timer = The timer to read
 * @param target_hms = The target HMS clock time
 * @return The remaining HMS clock time to reach the target HMS clock time.
 *         If the target was already reached, returns an HMS clock time of 0.
 */
MILLIS_HMS_TYPE Millis__GetRemainingHms(MILLIS_TIMER_TYPE* timer, MILLIS_HMS_TYPE target_hms)
{
    uint32 target_milliseconds = ConvertHmsToMilliseconds(target_hms);
    uint32 remaining_milliseconds = Millis__GetRemaining(timer, target_milliseconds);
    return ConvertMillisecondsToHms(remaining_milliseconds);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Checks if the specified timer has reached the specified milliseconds target.
 * Helper function when using the timer as a count down timer.
 * @param timer = The timer to read
 * @param target_milliseconds = The target milliseconds
 * @return TRUE if the timer has reached the specified target milliseconds; otherwise FALSE.
 */
BOOL_TYPE Millis__ReachedTarget(MILLIS_TIMER_TYPE* timer, uint32 target_milliseconds)
{
    return (BOOL_TYPE)(Millis__GetRemaining(timer, target_milliseconds) == 0);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Checks if the specified timer has reached the specified seconds target.
 * Helper function when using the timer as a count down timer.
 * @param timer = The timer to read
 * @param target_seconds = The target seconds
 * @return TRUE if the timer has reached the specified target seconds; otherwise FALSE.
 */
BOOL_TYPE Millis__ReachedTargetSeconds(MILLIS_TIMER_TYPE* timer, uint32 target_seconds)
{
    return (BOOL_TYPE)(Millis__GetRemainingSeconds(timer, target_seconds) == 0);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Checks if the specified timer has reached the specified HMS clock time target.
 * Helper function when using the timer as a count down timer.
 * @param timer = The timer to read
 * @param target_hms = The target HMS clock time
 * @return TRUE if the timer has reached the specified target HMS clock time; otherwise FALSE.
 */
BOOL_TYPE Millis__ReachedTargetHms(MILLIS_TIMER_TYPE* timer, MILLIS_HMS_TYPE target_hms)
{
    uint32 target_milliseconds = ConvertHmsToMilliseconds(target_hms);
    return (BOOL_TYPE)(Millis__GetRemaining(timer, target_milliseconds) == 0);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Subtract milliseconds from the timer without going below zero.
 * If the timer has less time than the value subtracted, then the timer is set to zero.
 * Allows a timer to be used to measure time periods without losing time between periods.
 * @param timer = The timer to update
 * @param milliseconds = The number of milliseconds to subtract from the timer
 */
void Millis__RestartPeriod(MILLIS_TIMER_TYPE* timer, uint32 milliseconds)
{
    if (timer != NULL)
    {
        if (timer->Running)
        {
            uint32 sample_milliseconds = Milliseconds;
            uint32 elapsed_milliseconds = (sample_milliseconds - timer->Reference) & MILLISECONDS_MASK;
            if (elapsed_milliseconds > milliseconds)
            {
                timer->Reference = timer->Reference + milliseconds;
            }
            else
            {
                timer->Reference = sample_milliseconds;
            }
            timer->Done  = FALSE;
        }
        else
        {
            Millis__Start(timer);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Subtract seconds from the timer without going below zero.
 * If the timer has less time than the value subtracted, then the timer is set to zero.
 * Allows a timer to be used to measure time periods without losing time between periods.
 * @param timer = The timer to update
 * @param seconds = The number of seconds to subtract from the timer
 */
void Millis__RestartSecondsPeriod(MILLIS_TIMER_TYPE* timer, uint32 seconds)
{
    Millis__RestartPeriod(timer, seconds * 1000);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Subtract HMS clock time from the timer without going below zero.
 * If the timer has less time than the value subtracted, then the timer is set to zero.
 * Allows a timer to be used to measure time periods without losing time between periods.
 * @param timer = The timer to update
 * @param hms = The HMS clock time to subtract from the timer
 */
void Millis__RestartHmsPeriod(MILLIS_TIMER_TYPE* timer, MILLIS_HMS_TYPE hms)
{
    uint32 milliseconds = ConvertHmsToMilliseconds(hms);
    Millis__RestartPeriod(timer, milliseconds);
}


//=====================================================================================================================
//  --- Private Functions ---------------------------------------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * Convert HMS clock time to milliseconds.
 * @param hms = The HMS clock time to convert
 * @return The equivalent time in milliseconds
 */
static uint32 ConvertHmsToMilliseconds(MILLIS_HMS_TYPE hms)
{
    uint32 value = hms.Hours;
    value *= 60;
    value += hms.Minutes;
    value *= 60;
    value += hms.Seconds;
    value *= 1000;
    value += hms.Milliseconds;
    return value;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Convert milliseconds to HMS clock time.
 * @param milliseconds = The milliseconds to convert
 * @return The equivalent HMS clock time.
 */
static MILLIS_HMS_TYPE ConvertMillisecondsToHms(uint32 milliseconds)
{
    MILLIS_HMS_TYPE result;
    uint32 value = milliseconds;
    result.Milliseconds = (value % 1000);
    value /= 1000;
    result.Seconds = (value % 60);
    value /= 60;
    result.Minutes = (value % 60);
    value /= 60;
    result.Hours = (value % 24);
    return result;
}

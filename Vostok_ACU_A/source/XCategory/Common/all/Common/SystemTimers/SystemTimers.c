/**
 *  @file       
 *
 *  @brief      SystemTimers implements a Up and Down counter timers based on 1ms System Tick.
 *
 *  @details    This module manages timers by taking snapshots of time when a timer is started, then comparing the
 *              snapshot time to a free running timer to know when a timer has expired.
 *              The number of timers is configurable and defined in SystemTimers_prv.h. The module allocates memory to
 *              handle that many concurrent timers. The purpose of the "handle" is used to identify one of those timers.
 *              All the management of the timer is done in this module. Application modules only need to store the
 *              "handle" to reference their timer. This module also allows for allocation and de-allocation of timer.
 *              The SystemTimers__GetHandle() function is the allocation of a timer and it provides the unique
 *              "handle", which identifies the timer. The SystemTimers__ReleaseHandle() function is the de-allocation
 *              of the timer, which will free up a timer to be used by another module.
 *              Added NVTimers which allow Non Volatile Timers in the system
 *
 *  @copyright  Copyright 2015-2017. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------

#include "SystemConfig.h"

#if (SYSTEM_TIMERS_FEATURE == ENABLED)
#include "SystemTimers.h"
#include "SystemTimers_prv.h"

#include "Log.h"
#include <string.h>
#include "Micro.h"

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//! list of module Log messages
typedef enum
{
    INVALID_HANDLE = 1
} MODULE_SYSTEMTIMERS_LOG_MESSAGE_ID_TYPE;
#define MODULE_NAME MODULE_SYSTEMTIMERS


//! Definition of data related to an instance of a timer
PACKED typedef struct
{
    uint32 compare;
    uint8  handle;
    SYSTEMTIMERS_STATE_TYPE  state;
    SYSTEMTIMERS_TYPE_TYPE  type;
    BOOL_TYPE  event;
}SYSTEMTIMERS_TYPE;

//! Check to ensure there is a limit on the maximum number of timers that can be allocated for the module
CT_ASSERT(SYSTEMTIMER_NUM_INSTANCES < 200);

static SYSTEMTIMERS_TYPE All_Timers[SYSTEMTIMER_NUM_INSTANCES];
static uint8 Handle_Reference;
static uint32 Handle_Counter;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void EvalTimer(SYSTEMTIMER_NV_TIMER_TYPE * nv_timer);
//====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  It Initializes the module Timers and its variables
 * @details The Initialization must be reset type aware
 */
void SystemTimers__Initialize(void)
{
    uint8 index;
    Handle_Reference = 1;

    // Initialize only if Cold Reset and the data is not restored from NVData
    if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
    {
    	Handle_Counter = 0;
    }


    for (index =0; index < SYSTEMTIMER_NUM_INSTANCES; index++)
    {
        All_Timers[index].handle = 0;
        All_Timers[index].compare = 0;
        All_Timers[index].state = SYSTEMTIMERS_STATE_INVALID;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Updates the free running counter and determines if any of the counters has expired.
 *          This function must be called every 1m out of the interruptions.
 */
void SystemTimers__Handler1ms(void)
{
    uint8 index;
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    Handle_Reference++;
    if (Handle_Reference == 0)
    {
        Handle_Reference++;
    }

    Handle_Counter++;   // Intentionally not handling overflow

    for (index =0; index < SYSTEMTIMER_NUM_INSTANCES; index++)
    {
        if (All_Timers[index].state == SYSTEMTIMERS_STATE_RUNNING)
        {
        	if (Handle_Counter == All_Timers[index].compare)
			{
				MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
				All_Timers[index].event = TRUE;
				All_Timers[index].state = SYSTEMTIMERS_STATE_DONE;
				MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
			}
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Returns the current value of the free running counter
 */
uint32 SystemTimers__GetFreeRunningCounter(void)
{
    return (Handle_Counter);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Request a Timer Handle
 * @return  Timer handle, if TIMERS_FAIL(0xFFFF) means not possible to allocate it.
 */
uint16 SystemTimers__GetHandle(SYSTEMTIMERS_TYPE_TYPE type)
{
    uint8 index;
    uint8 first_free;
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    first_free = 255;
    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
    for (index =0; index < SYSTEMTIMER_NUM_INSTANCES; index++)
    {
        if ((first_free == 255) &&
            (All_Timers[index].handle == 0))
        {
            first_free = index;
            All_Timers[first_free].state = SYSTEMTIMERS_STATE_STOPPED;
            All_Timers[first_free].type = type;
            All_Timers[first_free].handle = Handle_Reference;
            break;
        }
    }
    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();


    if (first_free < SYSTEMTIMER_NUM_INSTANCES)
    {
        return ((((uint16)All_Timers[first_free].handle) << 8) + ((uint16 )first_free));
    }
    else
    {
        LOG_ADD_EXCEPTION(INVALID_HANDLE, 0);
        CANNOT_ALLOCATE_SYSTEM_TIMER_HANDLE();
        return (SYSTEMTIMERS_FAIL);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Releases a timer handle
 * @param   handle: unique identifier for a timer
 * @return  PASS if the timer handle was valid, else FAIL
 */
PASS_FAIL_TYPE SystemTimers__ReleaseHandle(uint16 handle)
{
    PASS_FAIL_TYPE retval;
    uint8 index;
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    retval = FAIL;

    index = handle & 0x00FF;
    if (index < SYSTEMTIMER_NUM_INSTANCES)
    {
        if (All_Timers[index].handle == ((uint8)(handle >> 8) ))
        {
            MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
            All_Timers[index].handle = 0;
            All_Timers[index].state =  SYSTEMTIMERS_STATE_INVALID;

            retval = PASS;
            MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
        }
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   This method sets a timer in milliseconds
 * @param   handle: unique identifier for a timer
 * @param   reference_ms: number of milliseconds for the timer
 * @return  PASS if the timer handle was valid, else FAIL
 */
PASS_FAIL_TYPE SystemTimers__SetMs(uint16 handle, uint32 reference_ms)
{
    PASS_FAIL_TYPE retval;
    uint8 index;
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    retval = FAIL;

    index = handle & 0x00FF;
    if (index < SYSTEMTIMER_NUM_INSTANCES)
    {
        if (All_Timers[index].handle == ((uint8)(handle >> 8) ))
        {
            MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
            retval = PASS;

            //As Handle_Counter is pre counted so this check is required otherwise it will cause overloop
            if (reference_ms != 0)
            {
                if (All_Timers[index].type == SYSTEMTIMERS_TYPE_DOWN_COUNTER)
                {
                    All_Timers[index].compare = Handle_Counter + reference_ms;
                }
                else
                {
                    All_Timers[index].compare = Handle_Counter - reference_ms;
                }

                All_Timers[index].state = SYSTEMTIMERS_STATE_RUNNING;
                All_Timers[index].event = FALSE;
            }
            else
            {
                All_Timers[index].state = SYSTEMTIMERS_STATE_DONE;
                All_Timers[index].event = TRUE;
            }

            MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
        }
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   This method sets a timer in hours, minutes and seconds
 * @param   handle: unique identifier for a timer
 * @param   time: amount of time for the timer (see SYSTEMTIMER_HMS_TYPE)
 * @return  PASS if the timer handle was valid, else FAIL
 */
PASS_FAIL_TYPE SystemTimers__SetHMS(uint16 handle, SYSTEMTIMER_HMS_TYPE * time)
{
    uint32 value;

    value = ((((uint32)time->hours * 60) + (uint32)time->minutes) * 60) + (uint32)time->seconds;
	value *= 1000;
	value += (uint32)time->milliseconds;

    return (SystemTimers__SetMs(handle,value));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   This method resets a timer
 * @param   handle: unique identifier for a timer
 * @return  PASS if the timer handle was valid, else FAIL
 */
PASS_FAIL_TYPE SystemTimers__Reset(uint16 handle)
{
    PASS_FAIL_TYPE retval;
    uint8 index;
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    retval = FAIL;

    index = handle & 0x00FF;
    if (index < SYSTEMTIMER_NUM_INSTANCES)
    {
       if (All_Timers[index].handle == ((uint8)(handle >> 8) ))
       {
           MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
           All_Timers[index].state = SYSTEMTIMERS_STATE_STOPPED;
           All_Timers[index].event = FALSE;
           retval = PASS;
           MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
       }
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   This method pauses a Timer if it is RUNNING State.
 * @details It stores the remaining time in the second_compare data field.
 * @param   handle: unique identifier for a timer
 * @return  PASS if the timer handle was valid and in the RUNNING state, else FAIL
 */
PASS_FAIL_TYPE SystemTimers__Pause(uint16 handle)
{
    PASS_FAIL_TYPE retval;
    uint8 index;
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

    retval = FAIL;
    index = handle & 0x00FF;

    if (index < SYSTEMTIMER_NUM_INSTANCES)
    {
        if (All_Timers[index].handle == ((uint8)(handle >> 8) ))
        {
            if (All_Timers[index].state ==  SYSTEMTIMERS_STATE_RUNNING)
            {
            	if (All_Timers[index].type == SYSTEMTIMERS_TYPE_DOWN_COUNTER)
            	{
            		All_Timers[index].compare = All_Timers[index].compare - Handle_Counter;
            	}
            	else
            	{
            		All_Timers[index].compare = Handle_Counter - All_Timers[index].compare;
            	}
                All_Timers[index].state = SYSTEMTIMERS_STATE_PAUSED;
                retval = PASS;
            }
        }
    }

    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   This method resumes a Timer is it is in PAUSE state.
 * @details It resumes the timer using the value stored in the second_compare data field.
 * @param   handle: unique identifier for a timer
 * @return  PASS if the timer handle was valid and in the PAUSED state, else FAIL
 */
PASS_FAIL_TYPE SystemTimers__Resume(uint16 handle)
{
    PASS_FAIL_TYPE retval;
    uint8 index;

    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

    retval = FAIL;
    index = handle & 0x00FF;
    if (index < SYSTEMTIMER_NUM_INSTANCES)
    {
        if (All_Timers[index].handle == ((uint8)(handle >> 8) ))
        {
            if (All_Timers[index].state ==  SYSTEMTIMERS_STATE_PAUSED)
            {
                //All_Timers[index].stored_counter
            	if (All_Timers[index].type == SYSTEMTIMERS_TYPE_DOWN_COUNTER)
				{
					All_Timers[index].compare = Handle_Counter + All_Timers[index].compare;
				}
				else
				{
					All_Timers[index].compare = Handle_Counter - All_Timers[index].compare;
				}

            	All_Timers[index].state = SYSTEMTIMERS_STATE_RUNNING;
            }
        }
    }

    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Request the current value of a counter in milliseconds
 * @param   handle: unique identifier for a timer
 * @return  Remaining time in milliseconds
 */
uint32 SystemTimers__GetMs(uint16 handle)
{
    uint8 index;
    uint32 retval;
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    retval = 0;

    index = handle & 0x00FF;
    if (index < SYSTEMTIMER_NUM_INSTANCES)
    {
        if (All_Timers[index].handle == ((uint8)(handle >> 8) ))
        {
            MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

            switch (All_Timers[index].state)
            {
                case SYSTEMTIMERS_STATE_RUNNING:
                	if (All_Timers[index].type == SYSTEMTIMERS_TYPE_DOWN_COUNTER)
                	{
                		retval = (All_Timers[index].compare - Handle_Counter);
                	}
                	else
                	{
                		//All_Timers[index].compare = Handle_Counter - reference_ms;
                		retval = Handle_Counter - All_Timers[index].compare;
                	}
                    break;
                case SYSTEMTIMERS_STATE_PAUSED:
                    retval = All_Timers[index].compare;
                    break;
                case SYSTEMTIMERS_STATE_DONE:
                	if (All_Timers[index].type == SYSTEMTIMERS_TYPE_DOWN_COUNTER)
					{
                		retval = 0;
					}
					else
					{
						retval = (All_Timers[index].compare + Handle_Counter);
					}
                    break;
                default:
                    //SYSTEMTIMERS_STATE_STOPPED
                    break;
            }

            MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
        }
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   This method request the current value of a timer in HMS
 * @param   handle: unique identifier for a timer
 * @param   time: see SYSTEMTIMER_HMS_TYPE
 * @return  PASS if the timer handle was valid, else FAIL
 */
PASS_FAIL_TYPE SystemTimers__GetHMS(uint16 handle, SYSTEMTIMER_HMS_TYPE * time)
{
    uint32 value;
    value = SystemTimers__GetMs(handle);

    time->milliseconds = (uint16)(value % 1000);
    value /= 1000;
    time->seconds = (uint8)(value % 60);
    value /= 60;
    time->minutes = (uint8)(value % 60);
    value /= 60;
    time->hours = (uint8)value;

    return (PASS);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   This method gets the current state of a timer
 * @param   handle: unique identifier for a timer
 * @return  SYSTEMTIMERS_STATE_INVALID if the timer is invalid, else the current state of the timer (See SYSTEMTIMERS_STATE_TYPE)
 */
SYSTEMTIMERS_STATE_TYPE SystemTimers__GetState(uint16 handle)
{
    SYSTEMTIMERS_STATE_TYPE retval;
    uint8 index;

    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

    retval = SYSTEMTIMERS_STATE_INVALID;
    index = handle & 0x00FF;

    if (index < SYSTEMTIMER_NUM_INSTANCES)
    {
        if (All_Timers[index].handle == ((uint8)(handle >> 8) ))
        {
            retval = All_Timers[index].state;
        }
    }
    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   This method gets the status of the timer, expired or not.
 * @param   handle: unique identifier for a timer
 * @return  TRUE if the timer has expired, else FALSE.
 */
BOOL_TYPE SystemTimers__GetEvent(uint16 handle)
{
    BOOL_TYPE retval;
    uint8 index;

    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    retval = FALSE;
    index = handle & 0x00FF;

    if (index < SYSTEMTIMER_NUM_INSTANCES)
    {
        if (All_Timers[index].handle == ((uint8)(handle >> 8) ))
        {
            MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
            retval = All_Timers[index].event;
            All_Timers[index].event = FALSE;
            MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
        }
    }
    return (retval);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method initializes a NVTimer Struct.
 * @param nv_timer
 */
void SystemTimers__NVResetTimer(SYSTEMTIMER_NV_TIMER_TYPE * nv_timer)
{
	nv_timer->state = SYSTEMTIMERS_STATE_STOPPED;
	nv_timer->n_state = SYSTEMTIMERS_STATE_N_STOPPED;

	nv_timer->type = SYSTEMTIMERS_TYPE_UP_COUNTER;
	nv_timer->n_type = SYSTEMTIMERS_TYPE_N_UP_COUNTER;

	nv_timer->last_handle_counter = Handle_Counter;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method  Adjust the structure of a NVTimer based on the stored value.
 * @param nv_timer
 */
void SystemTimers__NVRestore(SYSTEMTIMER_NV_TIMER_TYPE * nv_timer)
{
	uint32 diff_timer;

	if (nv_timer->last_handle_counter >= Handle_Counter)
	{
		// for instance:
		// Last time the last_handle_counter was 3000 and all values related to that..
		// Now the Handle_Counter is 2000
		// diff_timer  will be 1000
		// and the timers was ahead of the current timer
		// All values should be decremented by the difference
		diff_timer = nv_timer->last_handle_counter - Handle_Counter;
		nv_timer->reference -= diff_timer;
		nv_timer->pause_value -= diff_timer;
		nv_timer->compare -= diff_timer;
		nv_timer->last_handle_counter -= diff_timer;
	}
	else
	{
		// for instance:
		// Last time the last_handle_counter was 3000 and all values related to that..
		// Now the Handle_Counter is 4000
		// diff_timer  will be 1000
		// and the timers was behind of the current timer
		// All values should be incremented by the difference
		diff_timer = Handle_Counter - nv_timer->last_handle_counter;
		nv_timer->reference += diff_timer;
		nv_timer->pause_value += diff_timer;
		nv_timer->compare += diff_timer;
		nv_timer->last_handle_counter += diff_timer;
	}
	nv_timer->last_handle_counter = Handle_Counter;
}



//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method set a NVTimer using milliseconds
 * @param nv_timer
 * @param ms_duration
 * @return
 */
PASS_FAIL_TYPE SystemTimers__NVSetTimerMS(SYSTEMTIMER_NV_TIMER_TYPE * nv_timer, uint32 ms_duration,SYSTEMTIMERS_TYPE_TYPE type)
{
	nv_timer->reference = Handle_Counter;
	nv_timer->compare = Handle_Counter + ms_duration;
	nv_timer->type = type;
	nv_timer->n_type = (SYSTEMTIMERS_TYPE_TYPE)(~type);
	nv_timer->state = SYSTEMTIMERS_STATE_RUNNING;
	nv_timer->n_state = SYSTEMTIMERS_STATE_N_RUNNING;
	nv_timer->last_handle_counter = Handle_Counter;
	return (PASS);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method Sets a NVtimer using HMS data
 * @param nv_timer
 * @param time
 * @return
 */
PASS_FAIL_TYPE SystemTimers__NVSetTimerHMS(SYSTEMTIMER_NV_TIMER_TYPE * nv_timer, SYSTEMTIMER_HMS_TYPE * time,SYSTEMTIMERS_TYPE_TYPE type)
{
	uint32 value;
	value = ((((uint32)time->hours * 60) + (uint32)time->minutes) * 60) + (uint32)time->seconds;
	value *= 1000;
	nv_timer->reference = Handle_Counter;
	nv_timer->compare = Handle_Counter + value;
	nv_timer->type = type;
	nv_timer->n_type = (SYSTEMTIMERS_TYPE_TYPE)(~type);
	nv_timer->state = SYSTEMTIMERS_STATE_RUNNING;
	nv_timer->n_state = SYSTEMTIMERS_STATE_N_RUNNING;
	nv_timer->last_handle_counter = Handle_Counter;

	return (PASS);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method checks the state of a NVTimer
 * @param nv_timer
 * @return
 */
SYSTEMTIMERS_STATE_TYPE SystemTimers__NVCheckTimer(SYSTEMTIMER_NV_TIMER_TYPE * nv_timer)
{
	EvalTimer (nv_timer);
	return (nv_timer->state);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method Pause a NVTimer
 * @param nv_timer
 * @return
 */
PASS_FAIL_TYPE SystemTimers__NVPauseTimer(SYSTEMTIMER_NV_TIMER_TYPE * nv_timer)
{
	if (nv_timer->state == SYSTEMTIMERS_STATE_RUNNING)
	{
		nv_timer->pause_value = Handle_Counter;
		nv_timer->state = SYSTEMTIMERS_STATE_PAUSED;
		nv_timer->n_state = SYSTEMTIMERS_STATE_N_PAUSED;
		nv_timer->last_handle_counter = Handle_Counter;
		return (PASS);
	}
	else
	{
		return (FAIL);
	}
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This Method Resumes a NVTimer
 * @param nv_timer
 * @return
 */
PASS_FAIL_TYPE SystemTimers__NVResumeTimer(SYSTEMTIMER_NV_TIMER_TYPE * nv_timer)
{
	if (nv_timer->state == SYSTEMTIMERS_STATE_PAUSED)
	{
		// reference ---  pause ----- compare
		// ---------------------------------------- handle
		uint32 delta = Handle_Counter - nv_timer->pause_value;
		nv_timer->reference += delta;
		nv_timer->compare += delta;
		nv_timer->pause_value = nv_timer->compare;
		nv_timer->state = SYSTEMTIMERS_STATE_RUNNING;
		nv_timer->n_state = SYSTEMTIMERS_STATE_N_RUNNING;
		nv_timer->last_handle_counter = Handle_Counter;
		return (PASS);
	}
	else
	{
		return (FAIL);
	}
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method exposes a NV timer in milliseconds
 * @param nv_timer
 * @return
 */
uint32 SystemTimers__NVGetTimerMs(SYSTEMTIMER_NV_TIMER_TYPE * nv_timer)
{
	uint32 retval;
	retval = 0;
	EvalTimer (nv_timer);
	switch (nv_timer->state)
	{
		case SYSTEMTIMERS_STATE_RUNNING:
			// reference  ----- Handle_Counter -----  nv_timer->compare
			if (nv_timer->type == SYSTEMTIMERS_TYPE_DOWN_COUNTER)
			{
				if (nv_timer->compare >= Handle_Counter)
				{
					retval = nv_timer->compare - Handle_Counter;
				}
				else
				{
					retval = nv_timer->compare + (0xFFFFFFFF - Handle_Counter);
				}
			}
			else
			{
				if (Handle_Counter >= nv_timer->reference)
				{
					retval = Handle_Counter - nv_timer->reference;
				}
				else
				{
					// in this case the reference is bigger than the the counter which demosntrante overflow of the cunter or underflow of the reference

					retval = Handle_Counter + (0xFFFFFFFF - nv_timer->reference);
				}
			}
			break;
		case SYSTEMTIMERS_STATE_DONE:
			if (nv_timer->type == SYSTEMTIMERS_TYPE_DOWN_COUNTER)
			{
				retval = 0;
			}
			else
			{
				if (nv_timer->compare >= nv_timer->reference)
				{
					retval = nv_timer->compare - nv_timer->reference;
				}
				else
				{
					retval = nv_timer->compare + ( 0xFFFFFFFF - nv_timer->reference);

				}
			}
			break;
		case SYSTEMTIMERS_STATE_PAUSED:
			// reference  ----- pause -----  nv_timer->compare
			if (nv_timer->type == SYSTEMTIMERS_TYPE_DOWN_COUNTER)
			{
				if (nv_timer->compare >= nv_timer->pause_value)
				{
					retval = nv_timer->compare - nv_timer->pause_value;
				}
				else
				{
					retval = nv_timer->compare +(0xFFFFFFFF - nv_timer->pause_value);
				}
			}
			else
			{
				if (nv_timer->pause_value > nv_timer->reference)
				{
					retval = nv_timer->pause_value - nv_timer->reference;
				}
				else
				{
					retval = nv_timer->pause_value + (0xFFFFFFFF - nv_timer->reference);
				}
			}
			break;
		default:
//		case SYSTEMTIMERS_STATE_STOPPED:   // default , STOPPED and INVALID shall return 0
//		case SYSTEMTIMERS_STATE_INVALID:
			break;
	}
	return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method exposes a timer in HMS format
 * @param nv_timer
 * @param time
 * @return
 */
PASS_FAIL_TYPE SystemTimers__NVGetTimerHMS(SYSTEMTIMER_NV_TIMER_TYPE * nv_timer, SYSTEMTIMER_HMS_TYPE * time)
{
	uint32 value;
	value = SystemTimers__NVGetTimerMs(nv_timer);
	time->milliseconds = (uint16)(value % 1000);
	value /= 1000;
	time->seconds = (uint8)(value % 60);
	value /= 60;
	time->minutes = (uint8)(value % 60);
	value /= 60;
	time->hours = (uint8)value;

	return (PASS);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Local method to evaluate a NVTimer
 * @param nv_timer
 */
static void EvalTimer(SYSTEMTIMER_NV_TIMER_TYPE * nv_timer)
{
	nv_timer->last_handle_counter = Handle_Counter;
	if (nv_timer->state == SYSTEMTIMERS_STATE_RUNNING)
	{
		//  |--reference  --------  compare ---|
		//  |----------------------------------|

		uint32 current;
		uint32 compare;

		if (Handle_Counter >= nv_timer->reference)
		{
			current = Handle_Counter - nv_timer->reference;
		}
		else
		{
			current = Handle_Counter +(0xFFFFFFFF - nv_timer->reference);
		}

		if (nv_timer->compare >= nv_timer->reference)
		{
			compare = nv_timer->compare - nv_timer->reference;
		}
		else
		{
			compare = nv_timer->compare + (0xFFFFFFFF - nv_timer->reference);
		}

		if (compare <= current)
		{
			nv_timer->state = SYSTEMTIMERS_STATE_DONE;
			nv_timer->n_state = SYSTEMTIMERS_STATE_N_DONE;
		}

	}
	else if (nv_timer->state > SYSTEMTIMERS_STATE_INVALID)
	{
		nv_timer->state = SYSTEMTIMERS_STATE_INVALID;
		nv_timer->n_state = SYSTEMTIMERS_STATE_N_INVALID;
	}
}

#endif

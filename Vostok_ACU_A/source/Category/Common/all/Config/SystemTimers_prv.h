/**
 *  @file       
 *
 *  @brief      Private parameters for the SystemTimers module.
 *
 *  @copyright  Copyright 2015-$Date: 2015/06/11 16:54:31EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SYSTEMTIMERS_PRV_H_
#define SYSTEMTIMERS_PRV_H_

//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================

/**
 *  Define the maximum number of timer handles that can be allocated.
 *  1 = 1 timer handle.
 */
#define SYSTEMTIMER_NUM_INSTANCES     40

/**
 *  Macro is called the by the SystemTimers core code when the API function SystemTimers__GetHandle() is called and the
 *  return value is SYSTEMTIMERS_FAIL, which indicates a timer handle could not be allocated.
 *
 *  Macro facilitates project specific handling of running out of timers.
 */
#define CANNOT_ALLOCATE_SYSTEM_TIMER_HANDLE()

#endif // SYSTEMTIMERS_PRV_H_

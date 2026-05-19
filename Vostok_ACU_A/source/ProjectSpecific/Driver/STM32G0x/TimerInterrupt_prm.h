/**
 *    @file
 *    @brief    This file contains configurable parameters of TimerInterrupt module
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
#ifndef TIMERINTERRUPT_PRM_H_
    #define TIMERINTERRUPT_PRM_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
/**
 * @brief  - This macro defines long duration time interrupt
 * @details- The value should be in milli second.
 *            Long interval is configured only when the TimerInterrupt__SetLongInterval() is called during low power routine,
 *
 *           Note - User should take care of the long interval time and the watchdog time
 *
 * @param  - 1 to 1000
 *
 */
//=====================================================================================================================
#define TIMERITERUPT_LONG_INTERVAL_IN_MS     25


#endif // TIMERINTERRUPT_PRM_H_



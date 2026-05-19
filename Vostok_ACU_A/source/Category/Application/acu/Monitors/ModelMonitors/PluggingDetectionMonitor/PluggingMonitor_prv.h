/**
 * @file
 * @brief
 *
 * @copyright   Copyright 2018 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef PLUGGINGMONITOR_PRV_H_
#define PLUGGINGMONITOR_PRV_H_

//! Time (in milliseconds) to set the timer to verify plugging risk
#define PLUG_RISK_DELAY_TIMER_MSEC (10U*1000U)

//! Maximum difference between two vwi signals is risk state to define it was just a noise variation
#define PLUG_RISK_NOISE_VALUE 2

#endif                                 /* PLUGGINGMONITOR_PRV_H_*/

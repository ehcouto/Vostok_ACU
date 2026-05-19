/**
 *    @file       ZeroCross_prv.h
 *    @brief      It includes the configurable parameters which can be configured as per requirements
 *
 *    @details    This file includes the configuration parameters which can be configured as per requirements such as
 *                "ZC_PULSE_DELAY" - this parameter configures ZeroCross detection after a delay, "ZC_ADVANCE" - this
 *                parameter configures ZeroCross detection based on perilous pulse, "ZC_COUNTER_BLACKOUT" - this parameter
 *                represents event when signal is not detected after specified time has elapsed
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2011.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef ZEROCROSS_PRV_H
    #define ZEROCROSS_PRV_H

#include "MciZeroCross.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  - Advance delay in micro-seconds
 *
 * @details - This configuration is very important.This configuration defines Timing gaps between Actual zero cross signal detected on main sin wave
 *            & actual Load firing edge with respect to that zero cross signal.
 *            This Load firing time is most of the time not match(Leading) with Actual Zero cross signal detected on main sin wave
 *
 * @note -    In below waveform there is a timing gap of T1,T3 between Actual Zero cross signal on mains Sin wave & Load firing edge.
 *            Loads are not firing exactly on that points where mains sin wave zero cross detect.This timing(T1,T3) get different with different hardware(Not fix).
 *            User should calculate this timing gap and assign the value on ZC_ADVANCE_PULSE_DELAY macro .
 *            this macro will take care to fire the loads on exact Zero cross signal on mains Sin wave.
 *            suppose T1 time gap is 4ms then define macro as below
 *
 *            #define ZC_ADVANCE_PULSE_DELAY          4000
 *
 *            if there is no any time gap between Actual Zero cross signal on mains Sin wave & Load firing edge . then Assign the macro as below
 *
 *            #define ZC_ADVANCE_PULSE_DELAY          0
 *
 *
 *
 *                            |   |               |   |    |               |     |
 *                            |   |<--triac on--->|   |    |<---triac on-->|     |
 *                            |   |               |   T2   |               |     |
 *                            |T1 |               |<------>|--triac off    |     |
 *                 triac off--|<->|               |   | T3 |               |<--->|--triac off
 *                            |   |               |   |    |       -       |     |
 *                            |   |               |   |    |   -       -   |     |
 *                            |   |               |   |    |-             -|     |
 *                            |   |               |   |  - |               |  -  |
 *                          - |   |               |   |-   |               |    -|
 *                     -------------------------------------------------------------------------- Sin Wave
 *                            | - |                  -|
 *                            |   -                -  |
 *                            |   | -           -     |
 *                            |   |    -      -       |
 *                            |   |        -          |
 *                            |                       |
 *                            |
 *                            |    ________________         _______________
 *                            |   |                |        |               |
 *                            |   |                |        |               |
 *                            |___|                |		|				|
 *            								    	 -------                 -----------------ZeroCross Signal
 */
#define ZC_ADVANCE_PULSE_DELAY          0


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       This macro will define the validity range in percentage of current determined Cycle
 *              in Signal Detected mode
 *
 * @details     If 0 or not defined the full range (45-65) shall be used instead.
 *
 */
#define     ZC_THRESHOLD_PERCENTAGE         0


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This macro will be called back whenever the Zero Cross module see's the angle events
 *
 * @details This shall be used as an signaling purpose only as this macro is been called from the Timer Output interrupt
 *          If time consuming algorithms are called then it will break the complete system
 *
 *
 * @param The angle parameter is of ZEROCROSS_EVENT_TYPE type
 *
 */
#ifndef MCI_POOLING_ZC_HW_SIGNAL
	#define ZC_ANGLE_EVENT(event)         MciZeroCross__GetEvent(event)
#endif

#define ZC_LOBE_THRESHOLDS      { ZC_CYCLE_USEC(66.0), ZC_FILTER_Q(-0.37092292712055), ZC_FILTER_Q(0.355668974882550) }, \
                                { ZC_CYCLE_USEC(64.0), ZC_FILTER_Q(-0.31511235235095), ZC_FILTER_Q(0.303005062577700) }, \
                                { ZC_CYCLE_USEC(62.0), ZC_FILTER_Q(-0.24825742624752), ZC_FILTER_Q(0.250446295864170) }, \
                                { ZC_CYCLE_USEC(60.0), ZC_FILTER_Q(-0.18197070137187), ZC_FILTER_Q(0.181003806661860) }, \
                                { ZC_CYCLE_USEC(58.0), ZC_FILTER_Q(-0.12658292609107), ZC_FILTER_Q(0.128948816287418) }, \
                                { ZC_CYCLE_USEC(56.0), ZC_FILTER_Q(-0.07254838946477), ZC_FILTER_Q(0.070480249316788) }, \
                                { ZC_CYCLE_USEC(54.0), ZC_FILTER_Q(-0.04506111145368), ZC_FILTER_Q(0.018224155429200) }, \
                                { ZC_CYCLE_USEC(52.0), ZC_FILTER_Q(0.019826889037200), ZC_FILTER_Q(-0.03986322212124) }, \
                                { ZC_CYCLE_USEC(50.0), ZC_FILTER_Q(0.075342178341360), ZC_FILTER_Q(-0.09450144191880) }, \
                                { ZC_CYCLE_USEC(48.0), ZC_FILTER_Q(0.144275224690800), ZC_FILTER_Q(-0.14435417172960) }, \
                                { ZC_CYCLE_USEC(46.0), ZC_FILTER_Q(0.212688446014800), ZC_FILTER_Q(-0.21463796997720) }, \
                                { ZC_CYCLE_USEC(44.0), ZC_FILTER_Q(0.274912794147600), ZC_FILTER_Q(-0.26688362500800) }

#endif /*ZEROCROSS_PRV_H*/



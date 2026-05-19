/**
 *  @file       SRClockMonitor_prv.h 
 *  @brief      Private (prv) configuration file of SRClockMonitor module.
 *
 *  @defgroup	CLASS_B
 *
 *  $Header: SRClockMonitor_prv.h 1.9 2012/06/18 18:50:53EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2010-$Date: 2012/06/18 18:50:53EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//=====================================================================================================================
//=====================================================================================================================
#ifndef SRCLOCKMONITOR_PRV_H_
#define SRCLOCKMONITOR_PRV_H_
#include "SRSystemConfig.h"
#include "SRTaskSequence.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

/// It defines the maximum number of TimerInterrupt counts above the expected is acceptable for the external clock source base
#define SRCLOCKMONITOR_CLK_TICKS_ABOVE       100

/// It defines the minimum number of TimerInterrupt counts below the expected is acceptable for the external clock source base
#define SRCLOCKMONITOR_CLK_TICKS_BELOW       100

/// It defines the maximum absolute value is acceptable for the counter, if this value is reached an exception if generated
#define SRCLOCK_MAX_ABSOLUT_COUNTER         0xFFFF


/// Directive that defines the number of samples for the debounce.
#define SRCLOCKMONITOR_MAX_ERROR_OCCURENCE  4

/// Definition of the number of comparisons which will be discarded after system initialization.
#define SRCLOCKMONITOR_INITIAL_DISREGARDED_READINGS 4



/// Call back used for SRFlow to log when this module is initialized
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
#define SRCLOCKMONITOR_INIT_FLOW()  SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_INIT_PHASE, SRTASKSEQUENCE_INIT_SRCLOCKMONITOR)
#else
#define SRCLOCKMONITOR_INIT_FLOW()
#endif

/// Call back used for SRFlow to log every 25us when the internal clock is considered
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
#define SRCLOCKMONITOR_1MS_FLOW()  SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_TICK_HANDLER, SRTASKSEQUENCE_TICK_SRCLOCKMONITOR)

#else
#define SRCLOCKMONITOR_1MS_FLOW()
#endif

/// Call back used for SRFlow to log every time the secondaru clock source is evaluated.
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
#define SRCLOCKMONITOR_EXT_FLOW()
#else
#define SRCLOCKMONITOR_EXT_FLOW()
#endif

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif // SRCLOCKMONITOR_PRV_H_



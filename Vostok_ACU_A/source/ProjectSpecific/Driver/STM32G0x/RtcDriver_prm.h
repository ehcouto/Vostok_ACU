/**
 *  @file       
 *
 *  @brief      Contains the configuration for the RtcDriver
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  
 *
 *  @copyright  Copyright 2014-. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef RTCDRIVER_PRM_H_
#define RTCDRIVER_PRM_H_

#include "SRSystemConfig.h"
#include "TimerInterrupt_prm.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//=====================================================================================================================
/**
 *    @brief  - This macro Enables or Disables the RTC interrupt for every number of seconds.
 *    @details-
 *
 *    @param  - ENABLED(1) - Non Zero number
 *              DISABLED(0) – if the macro value is 0
 *
 *    @note   - Following are the Examples\n
 *              "#define RTCDRIVER_GENERATE_INTERRUPT_EVERY_NUMBER_OF_SECONDS         1"   Interrupt generated every 1 second
 *              "#define RTCDRIVER_GENERATE_INTERRUPT_EVERY_NUMBER_OF_SECONDS         0"   no interrupt
 */
//---------------------------------------------------------------------------------------------------------------------
#if (CLASSB_FEATURE == ENABLED)
	#include "SRClockMonitor.h"
	#include "Gpio.h"
	#include "Mode.h"
    #define RTCDRIVER_GENERATE_INTERRUPT_EVERY_NUMBER_OF_SECONDS         2
#else
    #define RTCDRIVER_GENERATE_INTERRUPT_EVERY_NUMBER_OF_SECONDS         0
#endif

//=====================================================================================================================
/**
 *    @brief  - Call-back function every time the RTC interrupt occurs
 *
 *    @param  - User defined application function prototype
 *
 *    @note  -  "#define RTCDRIVER_INTERRUPT_HANDLER()                Application_RTC_TICK()"
 */
//=====================================================================================================================
#define RTC_TIME_IN_MS                              (RTCDRIVER_GENERATE_INTERRUPT_EVERY_NUMBER_OF_SECONDS * 1000)
#define RTCDRIVER_INTERRUPT_HANDLER()               {                                                                                       \
                                                        if (Mode__GetMode() == MODE_LOWPOWER)                                               \
                                                        {                                                                                   \
                                                            /*The system tick at 25ms */                                                    \
                                                            SRClockMonitor__MonitorExtClk(RTC_TIME_IN_MS/ TIMERITERUPT_LONG_INTERVAL_IN_MS);\
                                                        }                                                                                   \
                                                        else                                                                                \
                                                        {                                                                                   \
                                                            /* The system tick at 1ms */                                                    \
                                                            SRClockMonitor__MonitorExtClk(RTC_TIME_IN_MS);                                  \
                                                        }                                                                                   \
                                                     };

//=====================================================================================================================
/**
 *    @brief  - Call-back function every time the Epoch count changes in RtcDriver__Handler() while using the Polling method
 *
 *    @param  - User defined application function prototype
 *              Comment this macro to disable this feature
 *
 *    @note  -  "#define RTCDRIVER_HANDLER_ELAPSED_SEC(elapsed_sec)               SRClockMonitor__MonitorExtClk(elapsed_sec * 4000)"
 */
//=====================================================================================================================
//#define RTCDRIVER_HANDLER_ELAPSED_SEC(elapsed_sec)               API014RtcDriver__1sEvent(0, elapsed_sec)

#endif // RTCDRIVER_PRM_H_

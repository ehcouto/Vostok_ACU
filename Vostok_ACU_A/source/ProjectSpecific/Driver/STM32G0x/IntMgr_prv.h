/**
 *  @file       
 *
 *  @brief      This file offers handling of interrupt generated from External interrupt and Timer module
 *  @details    When used in Nucleus Architecture the interrupts are serviced by the Hbl module
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2021-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef _INTMGR_PRV_H_
#define _INTMGR_PRV_H_

#include "SystemConfig.h"
#if (SYSTEM_ACU_HANDLING == ENABLED)
    #include "Hbl.h"
#endif
#if (FAST_HANDLER_FEATURE == ENABLED)
    #include "SRMain.h"
#endif

//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  - External Interrupt service function call
 *  @details-
 *
 *  @param external_interrupt_channel - EXTINT_CHNL_TYPE(refer from ExtInterrupt.h)
 *                                      External Interrupt Channel from which the interrupt triggered
 *
 *  @note "#define EXTERNAL_INTURRUPT_SERVICE(external_interrupt_channel)     Hbl__ExtIntHandler(external_interrupt_channel)
 */
#if (SYSTEM_ACU_HANDLING == ENABLED)
    #define EXTERNAL_INTURRUPT_SERVICE(external_interrupt_channel)          Hbl__ExtIntHandler(external_interrupt_channel)
#endif


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  - Timer interrupt on output compare match
 *  @details-
 *
 *  @param timer         - TIMER_TYPE(refer from Timer.h file)
 *  @param timer_channel - TIMER_CHANNEL_TYPE(refer from Timer.h file)
 *
 *  @note "#define TIMER_OUTPUT_COMPARE_INTERRUPT_SERVICE(timer, timer_channel)    Hbl__OCIntHandler(timer, timer_channel)"
 */
#if ((HBL_MAX_NUM_OUTPUTCOMPARES_USED > 0) || (HBLACU_ZEROCROSS_FEATURE == ENABLED))
    #if(FAST_HANDLER_FEATURE == ENABLED)
        #define TIMER_OUTPUT_COMPARE_INTERRUPT_SERVICE(timer, timer_channel)    \
            do { \
                if((timer == FAST_HANDLER_TIMER) && \
                    (timer_channel == FAST_HANDLER_TIMER_CHANNEL)) \
                {\
                    SRMain__FastTimerISR();\
                }\
                else\
                {\
                    Hbl__OCIntHandler(timer, timer_channel);\
                }\
            } while(0)
    #else
        #define TIMER_OUTPUT_COMPARE_INTERRUPT_SERVICE(timer, timer_channel)    Hbl__OCIntHandler(timer, timer_channel)
    #endif
#elif(FAST_HANDLER_FEATURE == ENABLED)
    #define TIMER_OUTPUT_COMPARE_INTERRUPT_SERVICE(timer, timer_channel)    \
        do { \
            if((timer == FAST_HANDLER_TIMER) && \
                (timer_channel == FAST_HANDLER_TIMER_CHANNEL)) \
            {\
                SRMain__FastTimerISR();\
            }\
        } while(0)
#endif

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  - Timer interrupt on input capture
 *  @details-
 *
 *  @param timer         - TIMER_TYPE(refer from Timer.h file)
 *  @param timer_channel - TIMER_CHANNEL_TYPE(refer from Timer.h file)
 *
 *  @note "#define TIMER_INPUT_CAPTURE_INTERRUPT_SERVICE(timer, timer_channel) Hbl__ICIntHandler(timer, timer_channel)"
 */
#if (HBL_MAX_NUM_INPUTCAPTURES_USED > 0)
    #define TIMER_INPUT_CAPTURE_INTERRUPT_SERVICE(timer, timer_channel)     Hbl__ICIntHandler(timer, timer_channel)
#endif


#endif // _INTMGR_PRV_H_

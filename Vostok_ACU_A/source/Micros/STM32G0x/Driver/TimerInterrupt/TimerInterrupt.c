/**
 *    @file
 *    @brief    Timer Interrupt GDM module for STM32G0x implements the
 *              Timer Interrupt Standard API (Application Programming Interface)
 *    @details  Timer Interrupt module gives the Timer interrupt service to the application,
 *              basically it is initialized and after that every defined interval an interruption is generated
 *              which drives the slotted Architecture where user can execute the Application
 *
 *  $Header: TimerInterrupt.c 1.2 2015/06/24 14:55:20EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *  @copyright  Copyright 2012-$Date: 2015/06/24 14:55:20EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 *
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "Micro.h"
#include "TimerInterrupt.h"
#include "string.h"

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================
/**
 * @brief  - This macro defines the Systik interrupt priority.
 *
 *           Note - Its recommended to configure the systik at highest priority.
 *
 *
 * @param  - 0/1/2/3
 *          0 has highest priority and 3 has very low priority
 *
 * @note   - Following are the Examples\n
 *           "#define SYSTICK_PREEMPTION_PRIORITY 0"  //The Systik is having highest priority
 */
//=====================================================================================================================
#ifndef SYSTICK_PREEMPTION_PRIORITY
    #define SYSTICK_PREEMPTION_PRIORITY 3
#elif (SYSTICK_PREEMPTION_PRIORITY > 3)
    #error : "SYSTICK_PREEMPTION_PRIORITY macro value should be either 0,1,2,3"
#endif


/**
 *    @brief  - Defines the Interrupt Interval in us
 *    @details- Due to the WIDE implementation it is strongly recommended to use the timer interrupt interval of 1000us.
 *              The macro value should not be changed
 *
 *    @param  - 1000 // Do not change the value
 *
 *    @note   - Following is the Example\n
 *              "#define INTERRUPT_INTERVAL   1000"
 */
//=====================================================================================================================
#ifndef INTERRUPT_INTERVAL
 #define INTERRUPT_INTERVAL          1000             //Interrupt interval in us
#endif



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
 * @note   - Following are the Examples\n
 *           "#define TIMERITERUPT_LONG_INTERVAL_IN_MS 25"      //systik interval tick sets for every 25ms
 */
//=====================================================================================================================
#ifndef TIMERITERUPT_LONG_INTERVAL_IN_MS
    #define TIMERITERUPT_LONG_INTERVAL_IN_MS     25
#endif

#if (TIMERITERUPT_LONG_INTERVAL_IN_MS == 0)
    #error: "The value should be between 1 to 1000 for TIMERITERUPT_LONG_INTERVAL_IN_MS macro in the TimerInterrupt_prm.h file"
#endif

#define DEF_1MHZ                            1000000 //It generates a Timer clock freq = 1MHz
#define DEF_1KHZ                            1000    //It generates a Timer clock freq = 1KHz


#define TIMER_RELOAD_COUNT()            (Micro__GetClock()->HCLK_Frequency / (DEF_1MHZ/INTERRUPT_INTERVAL));
#define TIMER_RELOAD_LONG_COUNT()       (Micro__GetClock()->HCLK_Frequency / (DEF_1KHZ/TIMERITERUPT_LONG_INTERVAL_IN_MS));
#define HCLK_IN_MHZ()                   (Micro__GetClock()->HCLK_Frequency / DEF_1MHZ)
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method initializes the Timer interrupt module
 *    @details  1. This method should be called after initialization of all the modules like micro, Gpio, Hmi, Lsi,
 *                  Wide etc.
 *
 *              2. This function initializes the system tick timer and its interrupt and start the system tick timer.
 *                 Counter is in free running mode to generate periodical interrupts.
 *
 *              3. You can change the SysTick IRQ priority by changing the below macro -
 *                  #define SYSTICK_PREEMPTION_PRIORITY     0
 *
 *              4. SysTick time base uses the below formula:
 *                       Reload Value = SysTick Counter Clock (Hz) x  Desired Time base (s)
 */
void TimerInterrupt__Initialize(void)
{

    //Disable the System Core timer
    SYSTICK.CTRL &= SYSTICK_COUNTER_DISABLE;

     //AHB clock divided by 8 selected as SysTick clock source.
    SYSTICK.LOAD  = TIMER_RELOAD_COUNT();

    //Set Priority for SysTick_IRQ_CHANNEL
    Micro__NVICEnableIRQ (SysTick_IRQ_CHANNEL, SYSTICK_PREEMPTION_PRIORITY);

    SYSTICK.VAL   = 0;

    // Enable SysTick IRQ CLock source and SysTick Timer
    SYSTICK.CTRL  = SYSTICK_CTRL_CLKSOURCE_MSK | SYSTICK_CTRL_TICKINT_MSK | SYSTICK_CTRL_ENABLE_MSK;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It sets the timer interrupt for long duration
 *    @details  Long interval value is depends on the 'TIMERITERUPT_LONG_INTERVAL_IN_MS' macro value
 */
void TimerInterrupt__SetLongInterval(void)
{
    //Disable the System Core timer
    SYSTICK.CTRL &= SYSTICK_COUNTER_DISABLE;

     //AHB clock divided by 8 selected as SysTick clock source.
    SYSTICK.LOAD  = TIMER_RELOAD_LONG_COUNT();

    //Set Priority for SysTick_IRQ_CHANNEL
    Micro__NVICEnableIRQ (SysTick_IRQ_CHANNEL, SYSTICK_PREEMPTION_PRIORITY);

    SYSTICK.VAL   = 0;

    // Enable SysTick IRQ CLock source and SysTick Timer
    SYSTICK.CTRL  = SYSTICK_CTRL_CLKSOURCE_MSK | SYSTICK_CTRL_TICKINT_MSK | SYSTICK_CTRL_ENABLE_MSK;

    SERVICE_WATCHDOG();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    It returns timing (in microseconds) of number of system clock cycles in normal system clock configuration
 *    @param    cycle - number of cycles to return
 *    @return   timing in microseconds
 */
uint32 TimerInterrupt__ConvertCyclesToUs(uint32 cycles)
{
    uint64 us = (uint64) cycles * 1000000U;
    us = us / Micro__GetClock()->HCLK_Frequency;
    return (uint32)us;
}


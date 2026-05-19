/**
 *  @file
 *  @brief      Driver provides interface to setup timer interrupts.
 *
 *  @details
 *
 *  @Module_Owner{Maciej Kobuszewski}
 *  $Revision: 1.6 $
 *
 *  @copyright  Copyright 2011-$Date: 2016/06/24 07:58:02EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "C_Extensions.h"
#include "TimerInterruptExt.h"

// -- ANSI/ISO --

// -- Project --

#include "Micro.h"
#include "Mci_Prm.h"


//  --- Public Variables ----------------------------------------------------------------------------------------------

//  --- Private Properties --------------------------------------------------------------------------------------------

//  --- Private Function Prototypes -----------------------------------------------------------------------------------
#ifndef TIM6_FOR_MC_HANDLER
	#define TIM6_FOR_MC_HANDLER     DISABLED
#endif

//=====================================================================================================================
//  --- Public Functions ----------------------------------------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 * Function which setups periodic timer interrupt and enables it.
 *
 * @param freq  - interrupt frequency
 */
void TimerInterruptExt__SetupPerInt(unsigned short freq, unsigned char preemption_priority, unsigned char sub_priority)
{
#if(TIM6_FOR_MC_HANDLER == ENABLED)
    TIM_DEF_TYPE * tim6;
    tim6 = (TIM_DEF_TYPE *) TIM6_BASE;

    //Setup configuration of TIM6 for interrupt generation
    RCC.APBENR1 |= RCC_APBENR1_TIM6EN;

    tim6->CR1 &= (uint16)(~TIM_CR1_CEN);
    tim6->CR1 |= TIM_CR1_ARPE;
    tim6->CR1 |= TIM_CR1_URS;
    tim6->PSC = 0;

    // Setup pwm frequency
    tim6->ARR = (unsigned short)((Micro__GetClock()->PCLK1_Frequency) / (unsigned long)freq)-1;

    Micro__NVICEnableIRQ(TIM6_DAC_LPTIM1_IRQ_CHANNEL,preemption_priority);
    // Enable the TIM6_IRQ @ the peripheral level
    tim6->DIER |= TIM_DIER_UIE;                                    // Update update interrupt enable @ peripheral level
    tim6->CR1  |= TIM_CR1_CEN;
#endif
}
//---------------------------------------------------------------------------------------------------------------------
/**
 * This procedure starts periodic interrupt event.
 *
 */
void TimerInterruptExt__StartPerInt(void)
{
#if(TIM6_FOR_MC_HANDLER == ENABLED)
    TIM_DEF_TYPE * tim6;
    tim6 = (TIM_DEF_TYPE *) TIM6_BASE;

    tim6->CR1 |= TIM_CR1_CEN;
#endif
}
//---------------------------------------------------------------------------------------------------------------------
/**
 * This procedure stops periodic interrupt event.
 *
 */
void TimerInterruptExt__StopPerInt(void)
{
#if(TIM6_FOR_MC_HANDLER == ENABLED)
    TIM_DEF_TYPE * tim6;
    tim6 = (TIM_DEF_TYPE *) TIM6_BASE;

    tim6->CR1 &= (uint16)(~TIM_CR1_CEN);
#endif
}
//---------------------------------------------------------------------------------------------------------------------
/**
 * This procedure starts periodic interrupt event.
 *
 */
void TimerInterruptExt__ResetPerInt(void)
{
#if(TIM6_FOR_MC_HANDLER == ENABLED)
    TIM_DEF_TYPE * tim6;
    tim6 = (TIM_DEF_TYPE *) TIM6_BASE;

	tim6->CNT = 0;
#endif
}
//---------------------------------------------------------------------------------------------------------------------
/**
 * This procedure clear periodic interrupt flag (update event flag).
 *
 * Should be called at begining of timer interrupt.
 *
 */
void TimerInterruptExt__ClearPerIntF(void)
{
#if(TIM6_FOR_MC_HANDLER == ENABLED)
    TIM_DEF_TYPE * tim6;
    tim6 = (TIM_DEF_TYPE *) TIM6_BASE;

	tim6->SR = 0;                            // Clear all interrupt flags
#endif
}
//---------------------------------------------------------------------------------------------------------------------
/**
 * Function which setups event timer interrupt.
 *
 * @param event   - interrupt frequency
 * @param channel - channel for interrupt generation
 */
void TimerInterruptExt__SetupEventInt(unsigned char channel, unsigned char event)
{
    //For future extension
}
//---------------------------------------------------------------------------------------------------------------------
/**
 * This procedure clears event interrupt flag.
 *
 * Should be called at begining of timer.
 */
void TimerInterruptExt__ClearEvtIntF(unsigned char channel)
{
    //For future extension
}
//=====================================================================================================================
//  --- Private Functions ---------------------------------------------------------------------------------------------
//=====================================================================================================================

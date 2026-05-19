/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "IntMgr.h"
#include "IntMgr_prv.h"

#include "ExtInterrupt.h"
#include "Timer.h"
#include "uc.h"


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#ifndef EXTERNAL_INTURRUPT_SERVICE
    #define EXTERNAL_INTURRUPT_SERVICE(external_interrupt_channel)
#endif

#ifndef TIMER_OUTPUT_COMPARE_INTERRUPT_SERVICE
    #define TIMER_OUTPUT_COMPARE_INTERRUPT_SERVICE(timer, timer_channel)
#endif

#ifndef TIMER_INPUT_CAPTURE_INTERRUPT_SERVICE
    #define TIMER_INPUT_CAPTURE_INTERRUPT_SERVICE(timer, timer_channel)
#endif

#define TIM_SR_CCIF_FOR_ALL_CHANNELS    (TIM_SR_CC1IF | TIM_SR_CC2IF | TIM_SR_CC3IF | TIM_SR_CC4IF)

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module IntMgr and its variables
 *
 */
void IntMgr__Initialize(void)
{

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This API manages the External Interrupts
 *              All the pins of the STM32F0x micro controller are configured as External Interrupt
 *              All the similar PORT pin indexs are connected to that indexed external interrupt
 *              For Example - PORTA.PIN0, PORTB.PIN0, PORTC.PIN0, PORTD.PIN0, PORTE.PIN0...all are connected to EXTI0_IRQ_CHANNEL
 *                            PORTA.PIN1, PORTB.PIN1, PORTC.PIN1, PORTD.PIN1, PORTE.PIN1...all are connected to EXTI1_IRQ_CHANNEL
 *
 *              This drives the hardware limitation - All the External interrupt pins should have different pin number(No more than one same index pin is allowed)
 *
 *              Based on the External interrupt pin index use the ExtInterrupt__Ext() function for handling the ISR's for example -
 *              If the external interrupt is configured for PORTA, PIN0 and PORTC, PIN12 then the InterruptVector_prv.h file shall look like this -
 *              #define EXTI0_IRQHANDLER                IntMgr__ExtInterruptExt   //This handles the EXT0 IRQ for PORTA.PIN0
*/
void IntMgr__ExtInterruptExt(void)
{
    volatile uint32 rpr1;
    volatile uint32 fpr1;
    uint8 pos;

    rpr1 = EXTI.RPR1;
    EXTI.RPR1 = rpr1;

    fpr1 = EXTI.FPR1;
    EXTI.FPR1 = fpr1;

    //Rising pending interrupt
    for (pos = 0; pos < 16; pos++)
    {
        if ((rpr1 & 0x01) != 0)
        {
            EXTERNAL_INTURRUPT_SERVICE((EXTINT_CHNL_TYPE)pos);
        }

        rpr1 = rpr1>>1;

        if (rpr1 == 0)
        {
            break;
        }
    }

    //Falling pending interrupt
    for (pos = 0; pos < 16; pos++)
    {
        if ((fpr1 & 0x01) != 0)
        {
            EXTERNAL_INTURRUPT_SERVICE((EXTINT_CHNL_TYPE)pos);
        }

        fpr1 = fpr1>>1;

        if (fpr1 == 0)
        {
            break;
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief - Handles the Timer1 - IRQ
 *  @details - This API manages the Timer1 InputCapture & Output Compare Interrupts.
 *
 *              For Example - TIMER1 ------ CHANNEL1  :PORTA.PIN8
 *                                    |---- CHANNEL2  :PORTA.PIN9
 *                                    |---- CHANNEL3  :PORTA.PIN10
 *                                    |---- CHANNEL4  :PORTA.PIN11
 *
 *              If the Input captured interrupt is configured for PORTA.PIN8,PORTA.PIN9,PORTA.PIN10 &PORTA.PIN11 then the InterruptVector_prm.h.h file shall look like this -
 *               #define TIM1_CC_IRQHANDLER                IntMgr__Timer1IrqHandler    //This handles the TIMER1 Capture/Compare interrupt
 */
void IntMgr__Timer1IrqHandler(void)
{
    volatile uint16 irq_src;
    volatile uint16 irq_status;
    uint8 chnl;
    volatile uint32 ccmr_word;

    chnl = 0;

    irq_src = (TIM_1.SR & (TIM_1.DIER & TIM_SR_CCIF_FOR_ALL_CHANNELS)); //lint !e923 cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1]

    irq_src = irq_src >> 1;

    //Clear the status register
    TIM_1.SR = 0;

    while(irq_src)
    {
        chnl++;

        if (irq_src & 1)
        {
            ccmr_word = ((TIM_1.CCMR2) << 16) | ((TIM_1.CCMR1)); //lint !e923 cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1]
            ccmr_word = ccmr_word & (TIM_CCMR1_CC1S << ((chnl - 1)*8));

            if (ccmr_word == 0)
            {
                //Clear the Interrupt after the capture event
                TIM_1.DIER &= (uint16)(~(1 << chnl));
                TIMER_OUTPUT_COMPARE_INTERRUPT_SERVICE(TIMER1, (TIMER_CHANNEL_TYPE)chnl);
            }
            else
            {
                if (Timer_ProcessCapture(TIMER1, (TIMER_CHANNEL_TYPE)chnl) == TRUE)
                {
                    TIMER_INPUT_CAPTURE_INTERRUPT_SERVICE(TIMER1, (TIMER_CHANNEL_TYPE)chnl);
                }
            }
        }

        irq_src = irq_src >> 1;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief - Handles the Timer2 - IRQ
 *  @details - This API manages the Timer2 InputCapture & Output Compare Interrupts.
 *
 *              For Example - TIMER2 ------ CHANNEL1  :PORTA.PIN0,PORTA.PIN15
 *                                    |---- CHANNEL2  :PORTA.PIN1,PORTB.PIN3
 *                                    |---- CHANNEL3  :PORTA.PIN2,PORTB.PIN10
 *                                    |---- CHANNEL4  :PORTA.PIN3,PORTB.PIN11
 *
 *              If the Input captured interrupt is configured for PORTA.PIN0,PORTA.PIN1,PORTA.PIN2,PORTA.PIN3,PORTA.PIN15,PORTB.PIN3,PORTB.PIN10&PORTB.PIN11 then the InterruptVector_prm.h.h file shall look like this -
 *               #define TIM2_IRQHANDLER                IntMgr__Timer2IrqHandler    //This handles the TIMER2 Capture/Compare interrupt
 */
void IntMgr__Timer2IrqHandler(void)
{
    volatile uint16 irq_src;
    uint8 chnl;
    volatile uint32 ccmr_word;

    chnl = 0;

    irq_src = (TIM_2.SR & TIM_2.DIER & TIM_SR_CCIF_FOR_ALL_CHANNELS); //lint !e923 cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1]
    irq_src = irq_src >> 1;

    //Clear the status register
    TIM_2.SR = 0;

    while(irq_src)
    {
        chnl++;

        if (irq_src & 1)
        {
            ccmr_word = (TIM_2.CCMR2 << 16) | (TIM_2.CCMR1); //lint !e923 cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1]
            ccmr_word = ccmr_word & (TIM_CCMR1_CC1S << ((chnl - 1)*8));

            if (ccmr_word == 0)
            {
                //Clear the Interrupt after the capture event
                TIM_2.DIER &= (uint16)(~(1 << chnl));
                TIMER_OUTPUT_COMPARE_INTERRUPT_SERVICE(TIMER2, (TIMER_CHANNEL_TYPE)chnl);
            }
            else
            {
                if (Timer_ProcessCapture(TIMER2, (TIMER_CHANNEL_TYPE)chnl) == TRUE)
                {
                    TIMER_INPUT_CAPTURE_INTERRUPT_SERVICE(TIMER2, (TIMER_CHANNEL_TYPE)chnl);
                }
            }
        }

        irq_src = irq_src >> 1;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief - Handles the Timer3 - IRQ
 *  @details - This API manages the Timer3 InputCapture & Output Compare Interrupts.
 *
 *              For Example - TIMER3 ------ CHANNEL1  :PORTA.PIN6,PORTB.PIN4
 *                                    |---- CHANNEL2  :PORTA.PIN7,PORTB.PIN5
 *                                    |---- CHANNEL3  :PORTB.PIN0
 *                                    |---- CHANNEL4  :PORTB.PIN1
 *
 *              If the Input captured interrupt is configured for PORTA.PIN6,PORTB.PIN4,PORTA.PIN7,PORTB.PIN5,PORTB.PIN0,PORTB.PIN1 then the InterruptVector_prm.h.h file shall look like this -
 *               #define TIM3_IRQHANDLER                IntMgr__Timer3IrqHandler    //This handles the TIMER3 Capture/Compare interrupt
 */
void IntMgr__Timer3_4IrqHandler(void)
{
    volatile uint16 irq_src;
    uint8 chnl;
    volatile uint32 ccmr_word;

    chnl = 0;

    /**
     * TIMER3
     */
    irq_src = (TIM_3.SR & TIM_3.DIER & TIM_SR_CCIF_FOR_ALL_CHANNELS); //lint !e923 cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1]
    irq_src = irq_src >> 1;

    //Clear the status register
    TIM_3.SR = 0;

    while(irq_src)
    {
        chnl++;

        if (irq_src & 1)
        {
            ccmr_word = (TIM_3.CCMR2 << 16) | (TIM_3.CCMR1); //lint !e923 cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1]
            ccmr_word = ccmr_word & (TIM_CCMR1_CC1S << ((chnl - 1)*8));

            if (ccmr_word == 0)
            {
                //Clear the Interrupt after the capture event
                TIM_3.DIER &= (uint16)(~(1 << chnl));
                TIMER_OUTPUT_COMPARE_INTERRUPT_SERVICE(TIMER3, (TIMER_CHANNEL_TYPE)chnl);
            }
            else
            {
                if (Timer_ProcessCapture(TIMER3, (TIMER_CHANNEL_TYPE)chnl) == TRUE)
                {
                    TIMER_INPUT_CAPTURE_INTERRUPT_SERVICE(TIMER3, (TIMER_CHANNEL_TYPE)chnl);
                }
            }
        }

        irq_src = irq_src >> 1;
    }


    /**
     * TIMER4
     */
    chnl = 0;

    irq_src = (TIM_4.SR & TIM_4.DIER & TIM_SR_CCIF_FOR_ALL_CHANNELS); //lint !e923 cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1]
    irq_src = irq_src >> 1;

    //Clear the status register
    TIM_4.SR = 0;

    while(irq_src)
    {
        chnl++;

        if (irq_src & 1)
        {
            ccmr_word = (TIM_4.CCMR2 << 16) | (TIM_4.CCMR1); //lint !e923 cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1]
            ccmr_word = ccmr_word & (TIM_CCMR1_CC1S << ((chnl - 1)*8));

            if (ccmr_word == 0)
            {
                //Clear the Interrupt after the capture event
                TIM_4.DIER &= (uint16)(~(1 << chnl));
                TIMER_OUTPUT_COMPARE_INTERRUPT_SERVICE(TIMER4, (TIMER_CHANNEL_TYPE)chnl);
            }
            else
            {
                if (Timer_ProcessCapture(TIMER4, (TIMER_CHANNEL_TYPE)chnl) == TRUE)
                {
                    TIMER_INPUT_CAPTURE_INTERRUPT_SERVICE(TIMER4, (TIMER_CHANNEL_TYPE)chnl);
                }
            }
        }

        irq_src = irq_src >> 1;
    }

}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief - Handles the Timer14 - IRQ
 *  @details - This API manages the Timer4 InputCapture & Output Compare Interrupts.
 *
 *              For Example - TIMER14 ------ CHANNEL1  :PORTB.PIN1,PORTA.PIN4,PORTA.PIN7
 *              If the Input captured interrupt is configured for PORTB.PIN1,PORTA.PIN4,PORTA.PIN7 then the InterruptVector_prm.h.h file shall look like this -
 *               #define TIM14_IRQHANDLER                IntMgr__Timer14IrqHandler    //This handles the TIMER14 Capture/Compare interrupt
 */
void IntMgr__Timer14IrqHandler(void)
{
    volatile uint16 irq_src;
    uint8 chnl;
    volatile uint32 ccmr_word;

    chnl = 0;

    irq_src = (TIM_14.SR & TIM_14.DIER & TIM_SR_CCIF_FOR_ALL_CHANNELS); //lint !e923 cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1]
    irq_src = irq_src >> 1;

    //Clear the status register
    TIM_14.SR = 0;

    while(irq_src)
    {
        chnl++;

        if (irq_src & 1)
        {
            ccmr_word = (TIM_14.CCMR2 << 16) | (TIM_14.CCMR1); //lint !e923 cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1]
            ccmr_word = ccmr_word & (TIM_CCMR1_CC1S << ((chnl - 1)*8));

            if (ccmr_word == 0)
            {
                //Clear the Interrupt after the capture event
                TIM_14.DIER &= (uint16)(~(1 << chnl));
                TIMER_OUTPUT_COMPARE_INTERRUPT_SERVICE(TIMER14, (TIMER_CHANNEL_TYPE)chnl);
            }
            else
            {
                if (Timer_ProcessCapture(TIMER14, (TIMER_CHANNEL_TYPE)chnl) == TRUE)
                {
                    TIMER_INPUT_CAPTURE_INTERRUPT_SERVICE(TIMER14, (TIMER_CHANNEL_TYPE)chnl);
                }
            }
        }

        irq_src = irq_src >> 1;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief - Handles the Timer15 - IRQ
 *  @details - This API manages the Timer4 InputCapture & Output Compare Interrupts.
 *
 *              For Example - TIMER15 ------ CHANNEL1  :PORTA.PIN2,PORTB.PIN14
 *                                    ------ CHANNEL2  :PORTA.PIN3,PORTB.PIN15
 *              If the Input captured interrupt is configured for PORTA.PIN2,PORTB.PIN14,PORTA.PIN3,PORTB.PIN15 then the InterruptVector_prm.h.h file shall look like this -
 *               #define TIM15_IRQHANDLER                IntMgr__Timer15IrqHandler    //This handles the TIMER15 Capture/Compare interrupt
 */
void IntMgr__Timer15IrqHandler(void)
{
    volatile uint16 irq_src;
    uint8 chnl;
    volatile uint32 ccmr_word;

    chnl = 0;

    irq_src = (TIM_15.SR & TIM_15.DIER & TIM_SR_CCIF_FOR_ALL_CHANNELS); //lint !e923 cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1]
    irq_src = irq_src >> 1;

    //Clear the status register
    TIM_15.SR = 0;

    while(irq_src)
    {
        chnl++;

        if (irq_src & 1)
        {
            ccmr_word = (TIM_15.CCMR2 << 16) | (TIM_15.CCMR1); //lint !e923 cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1]
            ccmr_word = ccmr_word & (TIM_CCMR1_CC1S << ((chnl - 1)*8));

            if (ccmr_word == 0)
            {
                //Clear the Interrupt after the capture event
                TIM_15.DIER &= (uint16)(~(1 << chnl));
                TIMER_OUTPUT_COMPARE_INTERRUPT_SERVICE(TIMER15, (TIMER_CHANNEL_TYPE)chnl);
            }
            else
            {
                if (Timer_ProcessCapture(TIMER15, (TIMER_CHANNEL_TYPE)chnl) == TRUE)
                {
                    TIMER_INPUT_CAPTURE_INTERRUPT_SERVICE(TIMER15, (TIMER_CHANNEL_TYPE)chnl);
                }
            }
        }

        irq_src = irq_src >> 1;
    }
}
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief - Handles the Timer16 - IRQ
 *  @details - This API manages the Timer16 InputCapture & Output Compare Interrupts.
 *
 *              For Example - TIMER16 ------ CHANNEL1  :PORTB.PIN8,PORTA.PIN6
 *              If the Input captured interrupt is configured for PORTB.PIN8,PORTA.PIN6 then the InterruptVector_prm.h.h file shall look like this -
 *               #define TIM16_IRQHANDLER                IntMgr__Timer16IrqHandler    //This handles the TIMER16 Capture/Compare interrupt
 */
void IntMgr__Timer16IrqHandler(void)
{
    volatile uint16 irq_src;
    uint8 chnl;
    volatile uint32 ccmr_word;

    chnl = 0;

    irq_src = (TIM_16.SR & TIM_16.DIER & TIM_SR_CCIF_FOR_ALL_CHANNELS); //lint !e923 cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1]
    irq_src = irq_src >> 1;

    //Clear the status register
    TIM_16.SR = 0;

    while(irq_src)
    {
        chnl++;

        if (irq_src & 1)
        {
            ccmr_word = (TIM_16.CCMR2 << 16) | (TIM_16.CCMR1); //lint !e923 cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1]
            ccmr_word = ccmr_word & (TIM_CCMR1_CC1S << ((chnl - 1)*8));

            if (ccmr_word == 0)
            {
                //Clear the Interrupt after the capture event
                TIM_16.DIER &= (uint16)(~(1 << chnl));
                TIMER_OUTPUT_COMPARE_INTERRUPT_SERVICE(TIMER16, (TIMER_CHANNEL_TYPE)chnl);
            }
            else
            {
                if (Timer_ProcessCapture(TIMER16, (TIMER_CHANNEL_TYPE)chnl) == TRUE)
                {
                    TIMER_INPUT_CAPTURE_INTERRUPT_SERVICE(TIMER16, (TIMER_CHANNEL_TYPE)chnl);
                }
            }
        }
        irq_src = irq_src >> 1;
    }
}
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief - Handles the Timer17 - IRQ
 *  @details - This API manages the Timer17 InputCapture & Output Compare Interrupts.
 *
 *              For Example - TIMER17 ------ CHANNEL1  :PORTB.PIN9,PORTA.PIN7
 *              If the Input captured interrupt is configured for PORTB.PIN9,PORTA.PIN7 then the InterruptVector_prm.h.h file shall look like this -
 *               #define TIM17_IRQHANDLER                IntMgr__Timer17IrqHandler    //This handles the TIMER17 Capture/Compare interrupt
 */
void IntMgr__Timer17IrqHandler(void)
{
    volatile uint16 irq_src;
    uint8 chnl;
    volatile uint32 ccmr_word;

    chnl = 0;

    irq_src = (TIM_17.SR & TIM_17.DIER & TIM_SR_CCIF_FOR_ALL_CHANNELS); //lint !e923 cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1]
    irq_src = irq_src >> 1;

    //Clear the status register
    TIM_17.SR = 0;

    while(irq_src)
    {
        chnl++;

        if (irq_src & 1)
        {
            ccmr_word = (TIM_17.CCMR2 << 16) | (TIM_17.CCMR1); //lint !e923 cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1]
            ccmr_word = ccmr_word & (TIM_CCMR1_CC1S << ((chnl - 1)*8));

            if (ccmr_word == 0)
            {
                //Clear the Interrupt after the capture event
                TIM_17.DIER &= (uint16)(~(1 << chnl));
                TIMER_OUTPUT_COMPARE_INTERRUPT_SERVICE(TIMER17, (TIMER_CHANNEL_TYPE)chnl);
            }
            else
            {
                if (Timer_ProcessCapture(TIMER17, (TIMER_CHANNEL_TYPE)chnl) == TRUE)
                {
                    TIMER_INPUT_CAPTURE_INTERRUPT_SERVICE(TIMER17, (TIMER_CHANNEL_TYPE)chnl);
                }
            }
        }
        irq_src = irq_src >> 1;
    }
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================



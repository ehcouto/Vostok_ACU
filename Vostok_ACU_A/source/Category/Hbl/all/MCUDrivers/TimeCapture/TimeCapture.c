/**
 *  @file
 *  @brief      Capture time between consecutive digital input signal states.
 *
 *  @details    Specific implementation for time measurement dedicated to the Triac Control
 *  			TIMx CHx is used to generate a time based signal that will interrupt the
 *  			Application when the triac shall be triggered.
 *
 *  @Module_Owner {Eduardo H Couto}
 *  $Revision: 1.00 $
 *
 *  Copyright 2010 Whirlpool Corporation. All rights reserved - CONFIDENTIAL.
 *
 */

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "TimeCapture.h"
#include "Gpio.h"
#include "TimeCapture_prv.h"
#include "Micro.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static uint32 GetTIMFreq(void);
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module time capture and its variables
 *
 */
void TimeCapture__Initialize(void)
{

}





//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Initialize/configure selected timer and channel.
 *
 *  @param  input_number    : Selected channel number - specified time capture channel
 *  @param  event           : Capture time on specified/defined event
 *  @param  frequency       :
 */
void TimeCapture__ChannelConfig(TIME_CAPTURE_INPUT_TYPE input_number,CHANNEL_EVENT_TYPE event, uint32 frequency)
{
#if (TIM_USED == TIM14_BASE)
    TIM_DEF_TYPE * tim;
    tim = (TIM_DEF_TYPE *) TIM_USED;

    //Enable the clock for TIM14 module
    RCC.APBENR2 |= RCC_APBENR2_TIM14EN;

    tim->ARR = (uint16)(0xFFFF);
    tim->PSC = (uint16)((GetTIMFreq() / frequency) - 1);

    //Up-counter , edge-aligned
    tim->CR1 = (uint16)0x0000; //Clear CR1 Register
    tim->CR1  |= TIM_CR1_ARPE; //Enable the auto reload feature
    tim->CR1  |= TIM_CR1_CEN;  //Enable the Timer
#endif
}





//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Intialize hardware Timer ISR delayed after capture event.
 *
 */
void TimeCapture__EnableEventInterrupt(TIME_CAPTURE_INPUT_TYPE input_number)
{

}





//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Intialize hardware Timer ISR delayed after capture event.
 *
 *  @param  input_number    : Selected channel number - specified time capture channel
 */
void TimeCapture__EnablePostEventInterrupt(TIME_CAPTURE_INPUT_TYPE input_number)
{
#if (TIM_USED == TIM14_BASE)
    TIM_DEF_TYPE * tim;
    tim = (TIM_DEF_TYPE *) TIM_USED;

    if(IC00 == input_number)
    {
        //Configure TIM1 for interrupt in which Triac will be fired
        tim->CCMR1 &= ~(TIM_CCMR1_OC1M | TIM_CCMR1_OC1PE);
        tim->CCR[0] = (uint16)(0xFFFF); //Force it high during initalization. MCI will manage it accordingly.

        Micro__NVICEnableIRQ(TIM14_IRQ_CHANNEL, 0); //Set always as the maximum priority.
        tim->DIER |= TIM_DIER_CC1IE;        //Interrupt enabled
    }
#endif
}






//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Start/enable time capture for selected timer channel
 *
 *  @param  input_number    : Defines the specific Time capture channel
 */
void TimeCapture__Start(TIME_CAPTURE_INPUT_TYPE input_number)
{
#if (TIM_USED == TIM14_BASE)
    TIM_DEF_TYPE * tim;
    tim = (TIM_DEF_TYPE *) TIM_USED;

    switch (input_number)
    {
        #if (IC00_ENABLED == ENABLED)
        case IC00:
        	tim->CCER |= TIM_CCER_CC1E;        //Capture enabled
            break;
        #endif //(IC00_ENABLED == ENABLED)

        default :
            break;
    }
#endif
}






//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief Stop time capture for selected timer channel
 *
 *  @param  input_number    : Defines the specific Time capture channel
 */
void TimeCapture__Stop( TIME_CAPTURE_INPUT_TYPE input_number)
{
#if (TIM_USED == TIM14_BASE)
    TIM_DEF_TYPE * tim;
    tim = (TIM_DEF_TYPE *) TIM_USED;

    switch (input_number)
    {
        #if (IC00_ENABLED == ENABLED)
        case IC00:
        	tim->CCER = 0;        //Capture disabled
        	tim->DIER = 0;
            break;
        #endif //(IC00_ENABLED == ENABLED)

        default :
            break;
    }
#endif
}






//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Last measure event time in time tick
 *
 *  @param  input_number    : Defines the specific Time capture channel
 *
 *  @retval Last capture time in timer tick
 */
uint16  TimeCapture__ReadEventTime( TIME_CAPTURE_INPUT_TYPE input_number)
{
    uint16  retval_time_capture;

    retval_time_capture = 0;
#if (TIM_USED == TIM14_BASE)

    TIM_DEF_TYPE * tim;
	tim = (TIM_DEF_TYPE *) TIM_USED;

    switch (input_number)
    {
        #if (IC00_ENABLED == ENABLED)
        case IC00:
            retval_time_capture = tim->CCR[0];
            break;
        #endif //(IC00_ENABLED == ENABLED)

        default :
            break;
    }
#endif
    return retval_time_capture;
}






//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Get Current timer tick value
 *
 *  @param  input_number    : Defines the specific Time capture channel
 *
 *  @retval Current time value in timer tick
 */
uint16 TimeCapture__ReadTimerCnt( TIME_CAPTURE_INPUT_TYPE input_number)
{
    uint16  retval_time;

    retval_time = 0;
#if (TIM_USED == TIM14_BASE)
    TIM_DEF_TYPE * tim;
    tim = (TIM_DEF_TYPE *) TIM_USED;

    retval_time = tim->CNT;
    if (input_number == IC15)          //TIM14
    {
        //Read and Reset the Counter (Shall be executed when a ZC event is detected)
        tim->CNT = 0;
    }

#endif
    return retval_time;
}





//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Get basic timer tick frequency - value in MHz
 *
 *  @param  input_number    : Defines the specific Time capture channel
 *
 *  @retval Basic timer tick frequency in MHz unit
 */
uint32  TimeCapture__GetTimerTickFreq( TIME_CAPTURE_INPUT_TYPE input_number)
{
    TIM_DEF_TYPE * tim;
    tim = (TIM_DEF_TYPE *) TIM_USED;

    uint32 bus_freq;

    bus_freq = GetTIMFreq() / (uint32)(tim->PSC+1);

    return bus_freq;
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Get TIMxx Frequency
 *
 *  @param  input_number    : Defines the specific Time capture channel
 *
 *  @retval Basic timer tick frequency in MHz unit
 */
uint32 GetTIMFreq(void)
{
    uint32 bus_freq;

    bus_freq = Micro__GetClock()->PCLK1_Frequency;

    // 1. If the APB prescaler is set to 1, TIMPCLK frequency is equal to PCLK frequency.
    // 2. Otherwise, the TIMPCLK frequency is set to twice the PCLK frequency.
    if ((RCC.CFGR & RCC_CFGR_PPRE) != 0)
    {
        bus_freq = bus_freq * 2;
    }

    return bus_freq;
}





//---------------------------------------------------------------------------------------------------------------------
/**
 * Configure hardware timer interrupt by specyfying time after last capture event
 */
void TimeCapture__SetPostEventInterruptDelay(TIME_CAPTURE_INPUT_TYPE input_number, uint16 delay_ticks)
{
#if (TIM_USED == TIM14_BASE)
    TIM_DEF_TYPE * tim;
    tim = (TIM_DEF_TYPE *) TIM_USED;

	tim->CCR[0] = delay_ticks;
#endif
}





//---------------------------------------------------------------------------------------------------------------------
/**
 * Handler called every capture event
 *
 * It debounces input signal state 5 times and then stores time between previous input state change
 */
void TimeCapture__InterruptHandler(void)
{
#if (TIM_USED == TIM14_BASE)
    TIM_DEF_TYPE * tim;
    tim = (TIM_DEF_TYPE *) TIM_USED;

    uint16 sr_status;

    sr_status = tim->SR;
    // Output Compare interrupt for Mci
    if((sr_status & TIM_SR_CC1IF) != FALSE)
    {
    	tim->SR &= ~(TIM_SR_CC1IF); //Clear the Interrupt Flag
        INTERRUPT_CALLBACK_C01(); //Callback to the application
    }
#endif
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

/**
 *  @file
 *  @defgroup   CLASS_B
 *  @brief      This file contains the array of interruptions functions
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "InterruptVector_prv.h"
 
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#pragma language=extended
#pragma segment="CSTACK"

#define VECTOR_TABLE_APPLICATION_INTERRUPT_START_INDEX  15U

//=====================================================================================================================
//-------------------------------------- Interrupt Vector Table -------------------------------------------------------
//=====================================================================================================================
#pragma location = ".intvec"
const INTERRUPT_TABLE_TYPE __vector_table[] =
{
	{ .__ptr = __sfe( "CSTACK" ) },
	RESET_HANDLER,
    NMI_IRQEXCEPTION,
    HARDFAULT_IRQEXCEPTION,
    0, 0, 0, 0, 0, 0, 0,
    SVCALL_IRQHANDLER,
    0, 0,
    PENDSVC_IRQHANDLER,
    SYSTICK_IRQHANDLER,

    WWDG_IRQHANDLER,
    PVD_IRQHANDLER,
    RTC_TAMP_IRQHANDLER,
    FLASH_IRQHANDLER,
    RCC_CRS_IRQHANDLER,
    EXTI0_1_IRQHANDLER,
    EXTI2_3_IRQHANDLER,
    EXTI4_15_IRQHANDLER,
    USB_UCPD1_2_IRQHANDLER,
    DMA1_Channel1_IRQHANDLER,
    DMA1_Channel2_3_IRQHANDLER,
    DMA1_Ch4_7_DMA2_Ch1_5_DMAMUX1_OVR_IRQHANDLER,
    ADC1_COMP_IRQHANDLER,
    TIM1_BRK_UP_TRG_COM_IRQHANDLER,
    TIM1_CC_IRQHANDLER,
    TIM2_IRQHANDLER,

    TIM3_TIM4_IRQHANDLER,
    TIM6_DAC_LPTIM1_IRQHANDLER,
    TIM7_LPTIM2_IRQHANDLER,
    TIM14_IRQHANDLER,
    TIM15_IRQHANDLER,
    TIM16_FDCAN_IT0_IRQHANDLER,
    TIM17_FDCAN_IT1_IRQHANDLER,
    I2C1_IRQHANDLER,
    I2C2_3_IRQHANDLER,
    SPI1_IRQHANDLER,
    SPI2_3_IRQHANDLER,
    USART1_IRQHANDLER,
    USART2_LPUART2_IRQHANDLER,
    USART3_4_5_6_LPUART1_IRQHANDLER,
    CEC_IRQHANDLER,
};


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This returns the number of interrupts enabled used by the application.
 * @details This function does not consider the interrupts used by the core
 */
uint8 Micro__NumberOfInterruptsDefined(void)
{
    uint8 interrupts_enabled_count = 0;

    for (uint8 vector_index = VECTOR_TABLE_APPLICATION_INTERRUPT_START_INDEX; vector_index< (sizeof(__vector_table)/sizeof(INTERRUPT_TABLE_TYPE)); vector_index++)
    {
        if (__vector_table[vector_index].__ptr != (void *)Micro__Trap)
        {
            interrupts_enabled_count++;
        }
    }

    return interrupts_enabled_count;
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================



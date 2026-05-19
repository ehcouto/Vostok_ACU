/**
 *  @file
 *  @defgroup   CLASS_B
 *  @brief      ISR’s which are configurable to the Interrupt Vector Sources are defined in this file.
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#ifndef INTERRUPTVECTOR_PRV_H_
    #define INTERRUPTVECTOR_PRV_H_

#include "Micro.h"
#include "EmbOSBasic.h"


#include "ExtInterrupt.h"
#include "I2c.h"
#include "IntMgr.h"
#include "LowPower.h"
#include "RtcDriver.h"
#include "Spi.h"
#include "Timer.h"
#include "TimeCapture.h"
#include "Uart.h"

extern void PendSV_Handler(void);

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//=====================================================================================================================
/**
 *  @brief  -  This table defines the configuration of the ISRs in the Interrupt Vector Table
 *             and execution of the ISRs when the interrupt occurs from the respective source
 *  @details-  Do not Do the Below-\n
 *             1. No Macro should be commented\n
 *             2. For the unused interrupts should be trapped using Micro__Trap API\n
 *             3. RESET_INTERRUPTION should not be modified\n
 *             4. SYSTICK_IRQHANDLER should not be modified(Which provides the
 *                 basic Tick of 250us to run the slotted Architecture)
 *
 *  @param  - Micro__Trap is used for unused interrupt
 *             except for RESET_INTERRUPTION and SYSTICK_IRQHANDLER
 *
 *  @note    - If the USART1_IRQHANDLER is not used then do this-\n
 *                  "#define USART1_IRQHANDLER      Micro__Trap"\n
 *
 *             If the USART1_IRQHANDLER is used then do this-\n
 *                  "#define USART1_IRQHANDLER      Uart__Interrupt1"
 */
//=====================================================================================================================
/**
 * DO NOT MODIFY, unless you know what you are doing
 */
#define RESET_HANDLER                                       Micro__BootTest
#define NMI_IRQEXCEPTION                                    Micro__Trap
#define HARDFAULT_IRQEXCEPTION                              Micro__Trap
#define SVCALL_IRQHANDLER                                   Micro__Trap
#define PENDSVC_IRQHANDLER                                  PendSV_Handler
#define SYSTICK_IRQHANDLER                                  EmbOSBasic__SystickHandler


/**
 * Serviced by Drivers
 */
#if(EXTINTERRUPT_PIN0_1_FEATURE == ENABLED)
    #define EXTI0_1_IRQHANDLER                              IntMgr__ExtInterruptExt
#else
    #define EXTI0_1_IRQHANDLER                              Micro__Trap
#endif
#if(EXTINTERRUPT_PIN2_3_FEATURE == ENABLED)
    #define EXTI2_3_IRQHANDLER                              IntMgr__ExtInterruptExt
#else
    #define EXTI2_3_IRQHANDLER                              Micro__Trap
#endif
#if(EXTINTERRUPT_PIN4_15_FEATURE == ENABLED)
    #define EXTI4_15_IRQHANDLER                             IntMgr__ExtInterruptExt
#else
    #define EXTI4_15_IRQHANDLER                             Micro__Trap
#endif

#if(TIMER1_FEATURE == ENABLED)
    #define TIM1_CC_IRQHANDLER                              IntMgr__Timer1IrqHandler
#else
    #define TIM1_CC_IRQHANDLER                              Micro__Trap
#endif
#if(TIMER2_FEATURE == ENABLED)
    #define TIM2_IRQHANDLER                                 IntMgr__Timer2IrqHandler
#else
    #define TIM2_IRQHANDLER                                 Micro__Trap
#endif
#if(TIMER3_4_FEATURE == ENABLED)
    #define TIM3_TIM4_IRQHANDLER                            IntMgr__Timer3_4IrqHandler
#else
    #define TIM3_TIM4_IRQHANDLER                            Micro__Trap
#endif

#if(TIMER14_FEATURE == ENABLED)
    #define TIM14_IRQHANDLER                                TimeCapture__InterruptHandler
#else
    #define TIM14_IRQHANDLER                                Micro__Trap
#endif
#if(TIMER15_FEATURE == ENABLED)
    #define TIM15_IRQHANDLER                                IntMgr__Timer15IrqHandler
#else
    #define TIM15_IRQHANDLER                                Micro__Trap
#endif
#if(TIMER16_FEATURE == ENABLED)
    #define TIM16_FDCAN_IT0_IRQHANDLER                      LowPower__Timer16IrqHandler
#else
    #define TIM16_FDCAN_IT0_IRQHANDLER                      Micro__Trap
#endif
#if(TIMER17_FEATURE == ENABLED)
    #define TIM17_FDCAN_IT1_IRQHANDLER                      IntMgr__Timer17IrqHandler
#else
    #define TIM17_FDCAN_IT1_IRQHANDLER                      Micro__Trap
#endif
#if(I2C1_FEATURE == ENABLED)
    #define I2C1_IRQHANDLER                                 I2c1__InterruptHandler
#else
    #define I2C1_IRQHANDLER                                 Micro__Trap
#endif
#if ((I2C2_FEATURE == ENABLED) || (I2C3_FEATURE == ENABLED))
    #define I2C2_3_IRQHANDLER                               I2c2_3__InterruptHandler
#else
    #define I2C2_3_IRQHANDLER                               Micro__Trap
#endif

#if(UART1_FEATURE == ENABLED)
    #define USART1_IRQHANDLER                               Uart__Interrupt1
#else
    #define USART1_IRQHANDLER                               Micro__Trap
#endif
#if(UART2_FEATURE == ENABLED)
    #define USART2_LPUART2_IRQHANDLER                       Uart__Interrupt2
#else
    #define USART2_LPUART2_IRQHANDLER                       Micro__Trap
#endif
#if((UART3_FEATURE == ENABLED) || (UART4_FEATURE == ENABLED) || (UART5_FEATURE == ENABLED) || (UART6_FEATURE == ENABLED))
    #define USART3_4_5_6_LPUART1_IRQHANDLER                 Uart__Interrupt3_4_5_6
#else
    #define USART3_4_5_6_LPUART1_IRQHANDLER                 Micro__Trap
#endif

#if (RTCDRIVER_GENERATE_INTERRUPT_EVERY_NUMBER_OF_SECONDS > 0)
    #define RTC_TAMP_IRQHANDLER                             RtcDriver__Interrupt
#else
    #define RTC_TAMP_IRQHANDLER                             Micro__Trap
#endif


/**
 * ISR's not supported by Drivers
 */
#define WWDG_IRQHANDLER                                     Micro__Trap
#define PVD_IRQHANDLER                                      Micro__Trap
#define FLASH_IRQHANDLER                                    Micro__Trap
#define RCC_CRS_IRQHANDLER                                  Micro__Trap

#define SPI1_IRQHANDLER                                     Micro__Trap
#define SPI2_3_IRQHANDLER                                   Micro__Trap
#define USB_UCPD1_2_IRQHANDLER                              Micro__Trap
#define DMA1_Channel1_IRQHANDLER                            Micro__Trap
#define DMA1_Channel2_3_IRQHANDLER                          Micro__Trap
#define DMA1_Ch4_7_DMA2_Ch1_5_DMAMUX1_OVR_IRQHANDLER        Micro__Trap
#define ADC1_COMP_IRQHANDLER                                Micro__Trap
#define TIM1_BRK_UP_TRG_COM_IRQHANDLER                      Micro__Trap
#define TIM6_DAC_LPTIM1_IRQHANDLER                          Micro__Trap
#define TIM7_LPTIM2_IRQHANDLER                              Micro__Trap
#define CEC_IRQHANDLER                                      Micro__Trap

#endif // INTERRUPTVECTOR_PRV_H_

/**
 *  @file
 *  @defgroup   CLASS_B
 *  @brief      This file contains the STM32G0x controller related register definition.
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------- Include Files ----------------------------------------------------------------
#ifndef UC_STM32G0X_H_
#define UC_STM32G0X_H_      1
#include "Compiler_defs.h"
#include "uc_STM32G0x_prm.h"
#include "C_Extensions.h"

#pragma language=extended
#pragma segment="CSTACK"

//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- GENERIC MACROS ---------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/*!< Device Electronic Signature */
#define PACKAGE_BASE          (0x1FFF7500UL)        /*!< Package data register base address     */
#define UID_BASE              (0x1FFF7590UL)        /*!< Unique device ID register base address */
#define FLASHSIZE_BASE        (0x1FFF75E0UL)        /*!< Flash size data register base address  */

#define FLASH_SIZE_DATA_REGISTER        FLASHSIZE_BASE

#define FLASH_SIZE                      (((*((uint32 *)FLASH_SIZE_DATA_REGISTER)) & (0xFFFFU)) << 10U)

#define FLASH_BANK_SIZE                 (FLASH_SIZE)   /*!< FLASH Bank Size */

#define FLASH_PAGE_SIZE                 0x00000800U    /*!< FLASH Page Size, 2 KBytes */

#define HSI14_CLOCK_FREQUENCY   14000000
#define HSI_CLOCK_FREQUENCY     16000000
#define LSI_CLOCK_FREQUENCY     32000
#define SYSTEM_CLOCK_64MHZ      64000000

#define SET_BIT(REG, BIT)     ((REG) |= (BIT))

#define CLEAR_BIT(REG, BIT)   ((REG) &= ~(BIT))

#define READ_BIT(REG, BIT)    ((REG) & (BIT))

#define CLEAR_REG(REG)        ((REG) = (0x0))

#define WRITE_REG(REG, VAL)   ((REG) = (VAL))

#define READ_REG(REG)         ((REG))

#define MODIFY_REG(REG, CLEARMASK, SETMASK)  WRITE_REG((REG), (((READ_REG(REG)) & (~(CLEARMASK))) | (SETMASK)))




//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- MEMORY BASE ADDRESSES --------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//! Memory mapping of Cortex-M0 Hardware
#define SCS_BASE              (0xE000E000UL)                            /*!< System Control Space Base Address */
#define SYSTICK_BASE          (SCS_BASE +  0x0010UL)                    /*!< SysTick Base Address */
#define NVIC_BASE             (SCS_BASE +  0x0100UL)                    /*!< NVIC Base Address */
#define SCB_BASE              (SCS_BASE +  0x0D00UL)                    /*!< System Control Block Base Address */

#define FLASH_BASE            (0x08000000UL)  /*!< FLASH base address */
#define SRAM_BASE             (0x20000000UL)  /*!< SRAM base address */
#define PERIPH_BASE           (0x40000000UL)  /*!< Peripheral base address */
#define IOPORT_BASE           (0x50000000UL)  /*!< IOPORT base address */

/*!< Peripheral memory map */
#define APBPERIPH_BASE        (PERIPH_BASE)
#define AHBPERIPH_BASE        (PERIPH_BASE + 0x00020000UL)

//----------------------------------------
// Peripheral memory map
//----------------------------------------
/*!< Peripheral memory map */
#define TIM2_BASE             (APBPERIPH_BASE + 0UL)
#define TIM3_BASE             (APBPERIPH_BASE + 0x00000400UL)
#define TIM4_BASE             (APBPERIPH_BASE + 0x00000800UL)
#define TIM6_BASE             (APBPERIPH_BASE + 0x00001000UL)
#define TIM7_BASE             (APBPERIPH_BASE + 0x00001400UL)
#define TIM14_BASE            (APBPERIPH_BASE + 0x00002000UL)
#define RTC_BASE              (APBPERIPH_BASE + 0x00002800UL)
#define WWDG_BASE             (APBPERIPH_BASE + 0x00002C00UL)
#define IWDG_BASE             (APBPERIPH_BASE + 0x00003000UL)
#define SPI2_BASE             (APBPERIPH_BASE + 0x00003800UL)
#define SPI3_BASE             (APBPERIPH_BASE + 0x00003C00UL)
#define USART2_BASE           (APBPERIPH_BASE + 0x00004400UL)
#define USART3_BASE           (APBPERIPH_BASE + 0x00004800UL)
#define USART4_BASE           (APBPERIPH_BASE + 0x00004C00UL)
#define USART5_BASE           (APBPERIPH_BASE + 0x00005000UL)
#define I2C1_BASE             (APBPERIPH_BASE + 0x00005400UL)
#define I2C2_BASE             (APBPERIPH_BASE + 0x00005800UL)
#define USB_BASE              (APBPERIPH_BASE + 0x00005C00UL)  /*!< USB_IP Peripheral Registers base address */
#define FDCAN1_BASE           (APBPERIPH_BASE + 0x00006400UL)
#define FDCAN_CONFIG_BASE     (APBPERIPH_BASE + 0x00006500UL)  /*!< FDCAN configuration registers base address */
#define FDCAN2_BASE           (APBPERIPH_BASE + 0x00006800UL)
#define CRS_BASE              (APBPERIPH_BASE + 0x00006C00UL)
#define PWR_BASE              (APBPERIPH_BASE + 0x00007000UL)
#define DAC1_BASE             (APBPERIPH_BASE + 0x00007400UL)
#define DAC_BASE              (APBPERIPH_BASE + 0x00007400UL) /* Kept for legacy purpose */
#define CEC_BASE              (APBPERIPH_BASE + 0x00007800UL)
#define LPTIM1_BASE           (APBPERIPH_BASE + 0x00007C00UL)
#define LPUART1_BASE          (APBPERIPH_BASE + 0x00008000UL)
#define LPUART2_BASE          (APBPERIPH_BASE + 0x00008400UL)
#define I2C3_BASE             (APBPERIPH_BASE + 0x00008800UL)
#define LPTIM2_BASE           (APBPERIPH_BASE + 0x00009400UL)
#define USB_DRD_BASE          (APBPERIPH_BASE + 0x00005C00UL) /*!< USB_DRD_IP Peripheral Registers base address */
#define USB_DRD_PMAADDR       (APBPERIPH_BASE + 0x00009800UL) /*!< USB_DRD_IP Packet Memory Area base address */
#define UCPD1_BASE            (APBPERIPH_BASE + 0x0000A000UL)
#define UCPD2_BASE            (APBPERIPH_BASE + 0x0000A400UL)
#define TAMP_BASE             (APBPERIPH_BASE + 0x0000B000UL)
#define SRAMCAN_BASE          (APBPERIPH_BASE + 0x0000B400UL)
#define SYSCFG_BASE           (APBPERIPH_BASE + 0x00010000UL)
#define VREFBUF_BASE          (APBPERIPH_BASE + 0x00010030UL)
#define COMP1_BASE            (SYSCFG_BASE + 0x0200UL)
#define COMP2_BASE            (SYSCFG_BASE + 0x0204UL)
#define COMP3_BASE            (SYSCFG_BASE + 0x0208UL)
#define ADC1_BASE             (APBPERIPH_BASE + 0x00012400UL)
#define ADC1_COMMON_BASE      (APBPERIPH_BASE + 0x00012708UL)
#define ADC_BASE              (ADC1_COMMON_BASE) /* Kept for legacy purpose */
#define TIM1_BASE             (APBPERIPH_BASE + 0x00012C00UL)
#define SPI1_BASE             (APBPERIPH_BASE + 0x00013000UL)
#define USART1_BASE           (APBPERIPH_BASE + 0x00013800UL)
#define USART6_BASE           (APBPERIPH_BASE + 0x00013C00UL)
#define TIM15_BASE            (APBPERIPH_BASE + 0x00014000UL)
#define TIM16_BASE            (APBPERIPH_BASE + 0x00014400UL)
#define TIM17_BASE            (APBPERIPH_BASE + 0x00014800UL)
#define DBG_BASE              (APBPERIPH_BASE + 0x00015800UL)


/*!< AHB peripherals */
#define DMA1_BASE             (AHBPERIPH_BASE)
#define DMA2_BASE             (AHBPERIPH_BASE + 0x00000400UL)
#define DMAMUX1_BASE          (AHBPERIPH_BASE + 0x00000800UL)
#define RCC_BASE              (AHBPERIPH_BASE + 0x00001000UL)
#define EXTI_BASE             (AHBPERIPH_BASE + 0x00001800UL)
#define FLASH_R_BASE          (AHBPERIPH_BASE + 0x00002000UL)
#define CRC_BASE              (AHBPERIPH_BASE + 0x00003000UL)


#define DMA1_CHANNEL1_BASE    (DMA1_BASE + 0x00000008UL)
#define DMA1_CHANNEL2_BASE    (DMA1_BASE + 0x0000001CUL)
#define DMA1_CHANNEL3_BASE    (DMA1_BASE + 0x00000030UL)
#define DMA1_CHANNEL4_BASE    (DMA1_BASE + 0x00000044UL)
#define DMA1_CHANNEL5_BASE    (DMA1_BASE + 0x00000058UL)
#define DMA1_CHANNEL6_BASE    (DMA1_BASE + 0x0000006CUL)
#define DMA1_CHANNEL7_BASE    (DMA1_BASE + 0x00000080UL)
#define DMA2_CHANNEL1_BASE    (DMA2_BASE + 0x00000008UL)
#define DMA2_CHANNEL2_BASE    (DMA2_BASE + 0x0000001CUL)
#define DMA2_CHANNEL3_BASE    (DMA2_BASE + 0x00000030UL)
#define DMA2_CHANNEL4_BASE    (DMA2_BASE + 0x00000044UL)
#define DMA2_CHANNEL5_BASE    (DMA2_BASE + 0x00000058UL)

#define DMAMUX1_CHANNEL0_BASE    (DMAMUX1_BASE)
#define DMAMUX1_CHANNEL1_BASE    (DMAMUX1_BASE + 0x00000004UL)
#define DMAMUX1_CHANNEL2_BASE    (DMAMUX1_BASE + 0x00000008UL)
#define DMAMUX1_CHANNEL3_BASE    (DMAMUX1_BASE + 0x0000000CUL)
#define DMAMUX1_CHANNEL4_BASE    (DMAMUX1_BASE + 0x00000010UL)
#define DMAMUX1_CHANNEL5_BASE    (DMAMUX1_BASE + 0x00000014UL)
#define DMAMUX1_CHANNEL6_BASE    (DMAMUX1_BASE + 0x00000018UL)
#define DMAMUX1_CHANNEL7_BASE    (DMAMUX1_BASE + 0x0000001CUL)
#define DMAMUX1_CHANNEL8_BASE    (DMAMUX1_BASE + 0x00000020UL)
#define DMAMUX1_CHANNEL9_BASE    (DMAMUX1_BASE + 0x00000024UL)
#define DMAMUX1_CHANNEL10_BASE   (DMAMUX1_BASE + 0x00000028UL)
#define DMAMUX1_CHANNEL11_BASE   (DMAMUX1_BASE + 0x0000002CUL)

#define DMAMUX1_REQUESTGENERATOR0_BASE  (DMAMUX1_BASE + 0x00000100UL)
#define DMAMUX1_REQUESTGENERATOR1_BASE  (DMAMUX1_BASE + 0x00000104UL)
#define DMAMUX1_REQUESTGENERATOR2_BASE  (DMAMUX1_BASE + 0x00000108UL)
#define DMAMUX1_REQUESTGENERATOR3_BASE  (DMAMUX1_BASE + 0x0000010CUL)

#define DMAMUX1_CHANNELSTATUS_BASE      (DMAMUX1_BASE + 0x00000080UL)
#define DMAMUX1_REQUESTGENSTATUS_BASE   (DMAMUX1_BASE + 0x00000140UL)

/*!< IOPORT */
#define GPIOA_BASE            (IOPORT_BASE + 0x00000000UL)
#define GPIOB_BASE            (IOPORT_BASE + 0x00000400UL)
#define GPIOC_BASE            (IOPORT_BASE + 0x00000800UL)
#define GPIOD_BASE            (IOPORT_BASE + 0x00000C00UL)
#define GPIOE_BASE            (IOPORT_BASE + 0x00001000UL)
#define GPIOF_BASE            (IOPORT_BASE + 0x00001400UL)






//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- INTERRUPTS CHANNEL -----------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//Interrupt vector table type
typedef void( *interruptfunc )( void );

typedef union
{
    interruptfunc __fun;
    void * __ptr;
} INTERRUPT_TABLE_TYPE;
/*!< Interrupt Number Definition */
typedef enum
{
/******  Cortex-M0+ Processor Exceptions Numbers ***************************************************************/
  NonMaskableInt_IRQ_CHANNEL         = -14,    /*!< 2 Non Maskable Interrupt                                          */
  HardFault_IRQ_CHANNEL              = -13,    /*!< 3 Cortex-M Hard Fault Interrupt                                   */
  SVC_IRQ_CHANNEL                    = -5,     /*!< 11 Cortex-M SV Call Interrupt                                     */
  PendSV_IRQ_CHANNEL                 = -2,     /*!< 14 Cortex-M Pend SV Interrupt                                     */
  SysTick_IRQ_CHANNEL                = -1,     /*!< 15 Cortex-M System Tick Interrupt                                 */
/******  STM32G0xxxx specific Interrupt Numbers ****************************************************************/
  WWDG_IRQ_CHANNEL                   = 0,      /*!< Window WatchDog Interrupt                                         */
  PVD_IRQ_CHANNEL                    = 1,      /*!< PVD through EXTI line 16, PVM (monit. VDDIO2) through EXTI line 34*/
  RTC_TAMP_IRQ_CHANNEL               = 2,      /*!< RTC interrupt through the EXTI line 19 & 21                       */
  FLASH_IRQ_CHANNEL                  = 3,      /*!< FLASH global Interrupt                                            */
  RCC_CRS_IRQ_CHANNEL                = 4,      /*!< RCC and CRS global Interrupt                                      */
  EXTI0_1_IRQ_CHANNEL                = 5,      /*!< EXTI 0 and 1 Interrupts                                           */
  EXTI2_3_IRQ_CHANNEL                = 6,      /*!< EXTI Line 2 and 3 Interrupts                                      */
  EXTI4_15_IRQ_CHANNEL               = 7,      /*!< EXTI Line 4 to 15 Interrupts                                      */
  USB_UCPD1_2_IRQ_CHANNEL            = 8,      /*!< USB, UCPD1 and UCPD2 global Interrupt                             */
  DMA1_Channel1_IRQ_CHANNEL          = 9,      /*!< DMA1 Channel 1 Interrupt                                          */
  DMA1_Channel2_3_IRQ_CHANNEL        = 10,     /*!< DMA1 Channel 2 and Channel 3 Interrupts                           */
  DMA1_Ch4_7_DMA2_Ch1_5_DMAMUX1_OVR_IRQ_CHANNEL = 11, /*!< DMA1 Ch4 to Ch7, DMA2 Ch1 to Ch5 and DMAMUX1 Overrun Interrupts */
  ADC1_COMP_IRQ_CHANNEL              = 12,     /*!< ADC1, COMP1,COMP2, COMP3 Interrupts (combined with EXTI 17 & 18)  */
  TIM1_BRK_UP_TRG_COM_IRQ_CHANNEL    = 13,     /*!< TIM1 Break, Update, Trigger and Commutation Interrupts            */
  TIM1_CC_IRQ_CHANNEL                = 14,     /*!< TIM1 Capture Compare Interrupt                                    */
  TIM2_IRQ_CHANNEL                   = 15,     /*!< TIM2 Interrupt                                                    */
  TIM3_TIM4_IRQ_CHANNEL              = 16,     /*!< TIM3, TIM4 global Interrupt                                       */
  TIM6_DAC_LPTIM1_IRQ_CHANNEL        = 17,     /*!< TIM6, DAC and LPTIM1 global Interrupts                            */
  TIM7_LPTIM2_IRQ_CHANNEL            = 18,     /*!< TIM7 and LPTIM2 global Interrupt                                  */
  TIM14_IRQ_CHANNEL                  = 19,     /*!< TIM14 global Interrupt                                            */
  TIM15_IRQ_CHANNEL                  = 20,     /*!< TIM15 global Interrupt                                            */
  TIM16_IRQ_CHANNEL                  = 21,     /*!< TIM16, FDCAN1_IT0 and FDCAN2_IT0 Interrupt                        */
  TIM17_IRQ_CHANNEL                  = 22,     /*!< TIM17, FDCAN1_IT1 and FDCAN2_IT1 Interrupt                        */
  I2C1_IRQ_CHANNEL                   = 23,     /*!< I2C1 Interrupt  (combined with EXTI 23)                           */
  I2C2_3_IRQ_CHANNEL                 = 24,     /*!< I2C2, I2C3 Interrupt (combined with EXTI 24 and EXTI 22)          */
  SPI1_IRQ_CHANNEL                   = 25,     /*!< SPI1/I2S1 Interrupt                                               */
  SPI2_3_IRQ_CHANNEL                 = 26,     /*!< SPI2/I2S2, SPI3/I2S3 Interrupt                                    */
  USART1_IRQ_CHANNEL                 = 27,     /*!< USART1 Interrupt                                                  */
  USART2_IRQ_CHANNEL                 = 28,     /*!< USART2 + LPUART2 Interrupt                                        */
  USART3_4_5_6_LPUART1_IRQ_CHANNEL   = 29,     /*!< USART3, USART4, USART5, USART6, LPUART1 globlal Interrupts (combined with EXTI 28) */
  CEC_IRQ_CHANNEL                    = 30,     /*!< CEC Interrupt(combined with EXTI 27)                               */
} IRQ_CHANNEL_TYPE;


typedef enum
{
    CSTACK_TOP_INDEX                        =   0,
    RESET_HANDLER_INDEX                     =   1,
    NMI_IRQEXCEPTION_INDEX                  =   2,
    HARDFAULT_IRQEXCEPTION_INDEX            =   3,
    SVCALL_IRQHANDLER_INDEX                 =   11,
    PENDSVC_IRQHANDLER_INDEX                =   14,
    SYSTICK_IRQHANDLER_INDEX                =   15,
    WWDG_IRQHANDLER_INDEX                   =   16,
    PVD_IRQHANDLER_INDEX                    =   17,
    RTC_IRQHANDLER_INDEX                    =   18,
    FLASH_IRQHANDLER_INDEX                  =   19,
    RCC_IRQHANDLER_INDEX                    =   20,
    EXTI0_1_IRQHANDLER_INDEX                =   21,
    EXTI2_3_IRQHANDLER_INDEX                =   22,
    EXTI4_15_IRQHANDLER_INDEX               =   23,
    USB_UCPD1_2_IRQHANDLER_INDEX            =   24,
    DMA1CHANNEL1_IRQHANDLER_INDEX           =   25,
    DMA1CHANNEL2_3_IRQHANDLER_INDEX         =   26,
    DMA1CHANNEL4_7_DMAMUX1_OVR_IRQHANDLER_INDEX =   27,
    ADC1_COMP_IRQHANDLER_INDEX              =   28,
    TIM1_BRK_UP_TRG_COM_IRQHANDLER_INDEX    =   29,
    TIM1_CC_IRQHANDLER_INDEX                =   30,
    TIM2_IRQHANDLER_INDEX                   =   31,
    TIM3_TIM4_IRQHANDLER_INDEX              =   32,
    TIM6_DAC_IRQHANDLER_INDEX               =   33,
    TIM7_DAC_IRQHANDLER_INDEX               =   34,
    TIM14_IRQHANDLER_INDEX                  =   35,
    TIM15_IRQHANDLER_INDEX                  =   36,
    TIM16_IRQHANDLER_INDEX                  =   37,
    TIM17_IRQHANDLER_INDEX                  =   38,
    I2C1_IRQHANDLER_INDEX                   =   39,
    I2C2_3_IRQHANDLER_INDEX                 =   40,
    SPI1_IRQHANDLER_INDEX                   =   41,
    SPI2_3_IRQHANDLER_INDEX                 =   42,
    USART1_IRQHANDLER_INDEX                 =   43,
    USART2_LPUART2_IRQHANDLER_INDEX         =   44,
    USART3_4_5_6_LPUART1_IRQHANDLER_INDEX   =   45,
    CEC_IRQHANDLER_INDEX                    =   46,
} IRQ_INDEX_TYPE;





//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- GPIO -------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief General Purpose IO
 */
typedef struct
{
    volatile uint32 MODER;        //! GPIO port mode register,
    volatile uint16 OTYPER;      //! GPIO port output type register,
    volatile uint16 RESERVED0;   //! Reserved,
    volatile uint32 OSPEEDR;      //! GPIO port output speed register,
    volatile uint32 PUPDR;        //! GPIO port pull-up/pull-down register,
    volatile uint16 IDR;         //! GPIO port input data register,
    volatile uint16 RESERVED1;   //! Reserved,
    volatile uint16 ODR;         //! GPIO port output data register,
    volatile uint16 RESERVED2;   //! Reserved,
    volatile uint32 BSRR;         //! GPIO port bit set/reset registerBSRR,
    volatile uint32 LCKR;         //! GPIO port configuration lock register,
    volatile uint32 AFR[2];       //! GPIO alternate function low register,
    volatile uint16 BRR;         //! GPIO bit reset register,
    volatile uint16 RESERVED3;   //! Reserved,
} GPIO_DEF;

#define GPIO0    (*(GPIO_DEF *)GPIOA_BASE)
#define GPIO1    (*(GPIO_DEF *)GPIOB_BASE)
#define GPIO2    (*(GPIO_DEF *)GPIOC_BASE)
#define GPIO3    (*(GPIO_DEF *)GPIOD_BASE)
#define GPIO4    (*(GPIO_DEF *)GPIOE_BASE)
#define GPIO5    (*(GPIO_DEF *)GPIOF_BASE)


//#define PORT_DEF        GPIO_DEF *         //! This define is mandatory, it defines a GPIO register's type


#define GPIOA       (volatile GPIO_DEF *)GPIOA_BASE
#define GPIOB       (volatile GPIO_DEF *)GPIOB_BASE
#define GPIOC       (volatile GPIO_DEF *)GPIOC_BASE
#define GPIOD       (volatile GPIO_DEF *)GPIOD_BASE
#define GPIOE       (volatile GPIO_DEF *)GPIOE_BASE
#define GPIOF       (volatile GPIO_DEF *)GPIOF_BASE


typedef enum
{
    GPIO_AF_0 = 0,
    GPIO_AF_1,
    GPIO_AF_2,
    GPIO_AF_3,
    GPIO_AF_4,
    GPIO_AF_5,
    GPIO_AF_6,
    GPIO_AF_7,
    GPIO_AF_8,
    GPIO_AF_9,
    GPIO_AF_10,
}GPIO_AF_DEF;

//! ******************  Bit definition for GPIO_MODER register  *****************/
#define GPIO_MODER_MODER0          ((uint32)0x00000003)
#define GPIO_MODER_MODER0_0        ((uint32)0x00000001)
#define GPIO_MODER_MODER0_1        ((uint32)0x00000002)
#define GPIO_MODER_MODER1          ((uint32)0x0000000C)
#define GPIO_MODER_MODER1_0        ((uint32)0x00000004)
#define GPIO_MODER_MODER1_1        ((uint32)0x00000008)
#define GPIO_MODER_MODER2          ((uint32)0x00000030)
#define GPIO_MODER_MODER2_0        ((uint32)0x00000010)
#define GPIO_MODER_MODER2_1        ((uint32)0x00000020)
#define GPIO_MODER_MODER3          ((uint32)0x000000C0)
#define GPIO_MODER_MODER3_0        ((uint32)0x00000040)
#define GPIO_MODER_MODER3_1        ((uint32)0x00000080)
#define GPIO_MODER_MODER4          ((uint32)0x00000300)
#define GPIO_MODER_MODER4_0        ((uint32)0x00000100)
#define GPIO_MODER_MODER4_1        ((uint32)0x00000200)
#define GPIO_MODER_MODER5          ((uint32)0x00000C00)
#define GPIO_MODER_MODER5_0        ((uint32)0x00000400)
#define GPIO_MODER_MODER5_1        ((uint32)0x00000800)
#define GPIO_MODER_MODER6          ((uint32)0x00003000)
#define GPIO_MODER_MODER6_0        ((uint32)0x00001000)
#define GPIO_MODER_MODER6_1        ((uint32)0x00002000)
#define GPIO_MODER_MODER7          ((uint32)0x0000C000)
#define GPIO_MODER_MODER7_0        ((uint32)0x00004000)
#define GPIO_MODER_MODER7_1        ((uint32)0x00008000)
#define GPIO_MODER_MODER8          ((uint32)0x00030000)
#define GPIO_MODER_MODER8_0        ((uint32)0x00010000)
#define GPIO_MODER_MODER8_1        ((uint32)0x00020000)
#define GPIO_MODER_MODER9          ((uint32)0x000C0000)
#define GPIO_MODER_MODER9_0        ((uint32)0x00040000)
#define GPIO_MODER_MODER9_1        ((uint32)0x00080000)
#define GPIO_MODER_MODER10         ((uint32)0x00300000)
#define GPIO_MODER_MODER10_0       ((uint32)0x00100000)
#define GPIO_MODER_MODER10_1       ((uint32)0x00200000)
#define GPIO_MODER_MODER11         ((uint32)0x00C00000)
#define GPIO_MODER_MODER11_0       ((uint32)0x00400000)
#define GPIO_MODER_MODER11_1       ((uint32)0x00800000)
#define GPIO_MODER_MODER12         ((uint32)0x03000000)
#define GPIO_MODER_MODER12_0       ((uint32)0x01000000)
#define GPIO_MODER_MODER12_1       ((uint32)0x02000000)
#define GPIO_MODER_MODER13         ((uint32)0x0C000000)
#define GPIO_MODER_MODER13_0       ((uint32)0x04000000)
#define GPIO_MODER_MODER13_1       ((uint32)0x08000000)
#define GPIO_MODER_MODER14         ((uint32)0x30000000)
#define GPIO_MODER_MODER14_0       ((uint32)0x10000000)
#define GPIO_MODER_MODER14_1       ((uint32)0x20000000)
#define GPIO_MODER_MODER15         ((uint32)0xC0000000U)
#define GPIO_MODER_MODER15_0       ((uint32)0x40000000)
#define GPIO_MODER_MODER15_1       ((uint32)0x80000000U)

//! *****************  Bit definition for GPIO_OTYPER register  *****************/
#define GPIO_OTYPER_OT_0           ((uint32)0x00000001)
#define GPIO_OTYPER_OT_1           ((uint32)0x00000002)
#define GPIO_OTYPER_OT_2           ((uint32)0x00000004)
#define GPIO_OTYPER_OT_3           ((uint32)0x00000008)
#define GPIO_OTYPER_OT_4           ((uint32)0x00000010)
#define GPIO_OTYPER_OT_5           ((uint32)0x00000020)
#define GPIO_OTYPER_OT_6           ((uint32)0x00000040)
#define GPIO_OTYPER_OT_7           ((uint32)0x00000080)
#define GPIO_OTYPER_OT_8           ((uint32)0x00000100)
#define GPIO_OTYPER_OT_9           ((uint32)0x00000200)
#define GPIO_OTYPER_OT_10          ((uint32)0x00000400)
#define GPIO_OTYPER_OT_11          ((uint32)0x00000800)
#define GPIO_OTYPER_OT_12          ((uint32)0x00001000)
#define GPIO_OTYPER_OT_13          ((uint32)0x00002000)
#define GPIO_OTYPER_OT_14          ((uint32)0x00004000)
#define GPIO_OTYPER_OT_15          ((uint32)0x00008000)

//! ***************  Bit definition for GPIO_OSPEEDR register  ******************/
#define GPIO_OSPEEDER_50MHZ_SPEED_ALL  (uint32)0xFFFFFFFFU

#define GPIO_OSPEEDER_OSPEEDR0     ((uint32)0x00000003)
#define GPIO_OSPEEDER_OSPEEDR0_0   ((uint32)0x00000001)
#define GPIO_OSPEEDER_OSPEEDR0_1   ((uint32)0x00000002)
#define GPIO_OSPEEDER_OSPEEDR1     ((uint32)0x0000000C)
#define GPIO_OSPEEDER_OSPEEDR1_0   ((uint32)0x00000004)
#define GPIO_OSPEEDER_OSPEEDR1_1   ((uint32)0x00000008)
#define GPIO_OSPEEDER_OSPEEDR2     ((uint32)0x00000030)
#define GPIO_OSPEEDER_OSPEEDR2_0   ((uint32)0x00000010)
#define GPIO_OSPEEDER_OSPEEDR2_1   ((uint32)0x00000020)
#define GPIO_OSPEEDER_OSPEEDR3     ((uint32)0x000000C0)
#define GPIO_OSPEEDER_OSPEEDR3_0   ((uint32)0x00000040)
#define GPIO_OSPEEDER_OSPEEDR3_1   ((uint32)0x00000080)
#define GPIO_OSPEEDER_OSPEEDR4     ((uint32)0x00000300)
#define GPIO_OSPEEDER_OSPEEDR4_0   ((uint32)0x00000100)
#define GPIO_OSPEEDER_OSPEEDR4_1   ((uint32)0x00000200)
#define GPIO_OSPEEDER_OSPEEDR5     ((uint32)0x00000C00)
#define GPIO_OSPEEDER_OSPEEDR5_0   ((uint32)0x00000400)
#define GPIO_OSPEEDER_OSPEEDR5_1   ((uint32)0x00000800)
#define GPIO_OSPEEDER_OSPEEDR6     ((uint32)0x00003000)
#define GPIO_OSPEEDER_OSPEEDR6_0   ((uint32)0x00001000)
#define GPIO_OSPEEDER_OSPEEDR6_1   ((uint32)0x00002000)
#define GPIO_OSPEEDER_OSPEEDR7     ((uint32)0x0000C000)
#define GPIO_OSPEEDER_OSPEEDR7_0   ((uint32)0x00004000)
#define GPIO_OSPEEDER_OSPEEDR7_1   ((uint32)0x00008000)
#define GPIO_OSPEEDER_OSPEEDR8     ((uint32)0x00030000)
#define GPIO_OSPEEDER_OSPEEDR8_0   ((uint32)0x00010000)
#define GPIO_OSPEEDER_OSPEEDR8_1   ((uint32)0x00020000)
#define GPIO_OSPEEDER_OSPEEDR9     ((uint32)0x000C0000)
#define GPIO_OSPEEDER_OSPEEDR9_0   ((uint32)0x00040000)
#define GPIO_OSPEEDER_OSPEEDR9_1   ((uint32)0x00080000)
#define GPIO_OSPEEDER_OSPEEDR10    ((uint32)0x00300000)
#define GPIO_OSPEEDER_OSPEEDR10_0  ((uint32)0x00100000)
#define GPIO_OSPEEDER_OSPEEDR10_1  ((uint32)0x00200000)
#define GPIO_OSPEEDER_OSPEEDR11    ((uint32)0x00C00000)
#define GPIO_OSPEEDER_OSPEEDR11_0  ((uint32)0x00400000)
#define GPIO_OSPEEDER_OSPEEDR11_1  ((uint32)0x00800000)
#define GPIO_OSPEEDER_OSPEEDR12    ((uint32)0x03000000)
#define GPIO_OSPEEDER_OSPEEDR12_0  ((uint32)0x01000000)
#define GPIO_OSPEEDER_OSPEEDR12_1  ((uint32)0x02000000)
#define GPIO_OSPEEDER_OSPEEDR13    ((uint32)0x0C000000)
#define GPIO_OSPEEDER_OSPEEDR13_0  ((uint32)0x04000000)
#define GPIO_OSPEEDER_OSPEEDR13_1  ((uint32)0x08000000)
#define GPIO_OSPEEDER_OSPEEDR14    ((uint32)0x30000000)
#define GPIO_OSPEEDER_OSPEEDR14_0  ((uint32)0x10000000)
#define GPIO_OSPEEDER_OSPEEDR14_1  ((uint32)0x20000000)
#define GPIO_OSPEEDER_OSPEEDR15    ((uint32)0xC0000000U)
#define GPIO_OSPEEDER_OSPEEDR15_0  ((uint32)0x40000000)
#define GPIO_OSPEEDER_OSPEEDR15_1  ((uint32)0x80000000U)

//! ******************  Bit definition for GPIO_PUPDR register ******************/
#define GPIO_PUPDR_PUPDR0          ((uint32)0x00000003)
#define GPIO_PUPDR_PUPDR0_0        ((uint32)0x00000001)
#define GPIO_PUPDR_PUPDR0_1        ((uint32)0x00000002)
#define GPIO_PUPDR_PUPDR1          ((uint32)0x0000000C)
#define GPIO_PUPDR_PUPDR1_0        ((uint32)0x00000004)
#define GPIO_PUPDR_PUPDR1_1        ((uint32)0x00000008)
#define GPIO_PUPDR_PUPDR2          ((uint32)0x00000030)
#define GPIO_PUPDR_PUPDR2_0        ((uint32)0x00000010)
#define GPIO_PUPDR_PUPDR2_1        ((uint32)0x00000020)
#define GPIO_PUPDR_PUPDR3          ((uint32)0x000000C0)
#define GPIO_PUPDR_PUPDR3_0        ((uint32)0x00000040)
#define GPIO_PUPDR_PUPDR3_1        ((uint32)0x00000080)
#define GPIO_PUPDR_PUPDR4          ((uint32)0x00000300)
#define GPIO_PUPDR_PUPDR4_0        ((uint32)0x00000100)
#define GPIO_PUPDR_PUPDR4_1        ((uint32)0x00000200)
#define GPIO_PUPDR_PUPDR5          ((uint32)0x00000C00)
#define GPIO_PUPDR_PUPDR5_0        ((uint32)0x00000400)
#define GPIO_PUPDR_PUPDR5_1        ((uint32)0x00000800)
#define GPIO_PUPDR_PUPDR6          ((uint32)0x00003000)
#define GPIO_PUPDR_PUPDR6_0        ((uint32)0x00001000)
#define GPIO_PUPDR_PUPDR6_1        ((uint32)0x00002000)
#define GPIO_PUPDR_PUPDR7          ((uint32)0x0000C000)
#define GPIO_PUPDR_PUPDR7_0        ((uint32)0x00004000)
#define GPIO_PUPDR_PUPDR7_1        ((uint32)0x00008000)
#define GPIO_PUPDR_PUPDR8          ((uint32)0x00030000)
#define GPIO_PUPDR_PUPDR8_0        ((uint32)0x00010000)
#define GPIO_PUPDR_PUPDR8_1        ((uint32)0x00020000)
#define GPIO_PUPDR_PUPDR9          ((uint32)0x000C0000)
#define GPIO_PUPDR_PUPDR9_0        ((uint32)0x00040000)
#define GPIO_PUPDR_PUPDR9_1        ((uint32)0x00080000)
#define GPIO_PUPDR_PUPDR10         ((uint32)0x00300000)
#define GPIO_PUPDR_PUPDR10_0       ((uint32)0x00100000)
#define GPIO_PUPDR_PUPDR10_1       ((uint32)0x00200000)
#define GPIO_PUPDR_PUPDR11         ((uint32)0x00C00000)
#define GPIO_PUPDR_PUPDR11_0       ((uint32)0x00400000)
#define GPIO_PUPDR_PUPDR11_1       ((uint32)0x00800000)
#define GPIO_PUPDR_PUPDR12         ((uint32)0x03000000)
#define GPIO_PUPDR_PUPDR12_0       ((uint32)0x01000000)
#define GPIO_PUPDR_PUPDR12_1       ((uint32)0x02000000)
#define GPIO_PUPDR_PUPDR13         ((uint32)0x0C000000)
#define GPIO_PUPDR_PUPDR13_0       ((uint32)0x04000000)
#define GPIO_PUPDR_PUPDR13_1       ((uint32)0x08000000)
#define GPIO_PUPDR_PUPDR14         ((uint32)0x30000000)
#define GPIO_PUPDR_PUPDR14_0       ((uint32)0x10000000)
#define GPIO_PUPDR_PUPDR14_1       ((uint32)0x20000000)
#define GPIO_PUPDR_PUPDR15         ((uint32)0xC0000000U)
#define GPIO_PUPDR_PUPDR15_0       ((uint32)0x40000000)
#define GPIO_PUPDR_PUPDR15_1       ((uint32)0x80000000U)

//! ******************  Bit definition for GPIO_IDR register  *******************/
#define GPIO_IDR_0                 ((uint32)0x00000001)
#define GPIO_IDR_1                 ((uint32)0x00000002)
#define GPIO_IDR_2                 ((uint32)0x00000004)
#define GPIO_IDR_3                 ((uint32)0x00000008)
#define GPIO_IDR_4                 ((uint32)0x00000010)
#define GPIO_IDR_5                 ((uint32)0x00000020)
#define GPIO_IDR_6                 ((uint32)0x00000040)
#define GPIO_IDR_7                 ((uint32)0x00000080)
#define GPIO_IDR_8                 ((uint32)0x00000100)
#define GPIO_IDR_9                 ((uint32)0x00000200)
#define GPIO_IDR_10                ((uint32)0x00000400)
#define GPIO_IDR_11                ((uint32)0x00000800)
#define GPIO_IDR_12                ((uint32)0x00001000)
#define GPIO_IDR_13                ((uint32)0x00002000)
#define GPIO_IDR_14                ((uint32)0x00004000)
#define GPIO_IDR_15                ((uint32)0x00008000)

//! *****************  Bit definition for GPIO_ODR register  ********************/
#define GPIO_ODR_0                 ((uint32)0x00000001)
#define GPIO_ODR_1                 ((uint32)0x00000002)
#define GPIO_ODR_2                 ((uint32)0x00000004)
#define GPIO_ODR_3                 ((uint32)0x00000008)
#define GPIO_ODR_4                 ((uint32)0x00000010)
#define GPIO_ODR_5                 ((uint32)0x00000020)
#define GPIO_ODR_6                 ((uint32)0x00000040)
#define GPIO_ODR_7                 ((uint32)0x00000080)
#define GPIO_ODR_8                 ((uint32)0x00000100)
#define GPIO_ODR_9                 ((uint32)0x00000200)
#define GPIO_ODR_10                ((uint32)0x00000400)
#define GPIO_ODR_11                ((uint32)0x00000800)
#define GPIO_ODR_12                ((uint32)0x00001000)
#define GPIO_ODR_13                ((uint32)0x00002000)
#define GPIO_ODR_14                ((uint32)0x00004000)
#define GPIO_ODR_15                ((uint32)0x00008000)

//! ***************** Bit definition for GPIO_BSRR register  ********************/
#define GPIO_BSRR_BS_0             ((uint32)0x00000001)
#define GPIO_BSRR_BS_1             ((uint32)0x00000002)
#define GPIO_BSRR_BS_2             ((uint32)0x00000004)
#define GPIO_BSRR_BS_3             ((uint32)0x00000008)
#define GPIO_BSRR_BS_4             ((uint32)0x00000010)
#define GPIO_BSRR_BS_5             ((uint32)0x00000020)
#define GPIO_BSRR_BS_6             ((uint32)0x00000040)
#define GPIO_BSRR_BS_7             ((uint32)0x00000080)
#define GPIO_BSRR_BS_8             ((uint32)0x00000100)
#define GPIO_BSRR_BS_9             ((uint32)0x00000200)
#define GPIO_BSRR_BS_10            ((uint32)0x00000400)
#define GPIO_BSRR_BS_11            ((uint32)0x00000800)
#define GPIO_BSRR_BS_12            ((uint32)0x00001000)
#define GPIO_BSRR_BS_13            ((uint32)0x00002000)
#define GPIO_BSRR_BS_14            ((uint32)0x00004000)
#define GPIO_BSRR_BS_15            ((uint32)0x00008000)
#define GPIO_BSRR_BR_0             ((uint32)0x00010000)
#define GPIO_BSRR_BR_1             ((uint32)0x00020000)
#define GPIO_BSRR_BR_2             ((uint32)0x00040000)
#define GPIO_BSRR_BR_3             ((uint32)0x00080000)
#define GPIO_BSRR_BR_4             ((uint32)0x00100000)
#define GPIO_BSRR_BR_5             ((uint32)0x00200000)
#define GPIO_BSRR_BR_6             ((uint32)0x00400000)
#define GPIO_BSRR_BR_7             ((uint32)0x00800000)
#define GPIO_BSRR_BR_8             ((uint32)0x01000000)
#define GPIO_BSRR_BR_9             ((uint32)0x02000000)
#define GPIO_BSRR_BR_10            ((uint32)0x04000000)
#define GPIO_BSRR_BR_11            ((uint32)0x08000000)
#define GPIO_BSRR_BR_12            ((uint32)0x10000000)
#define GPIO_BSRR_BR_13            ((uint32)0x20000000)
#define GPIO_BSRR_BR_14            ((uint32)0x40000000)
#define GPIO_BSRR_BR_15            ((uint32)0x80000000U)

//! ***************** Bit definition for GPIO_LCKR register  ********************/
#define GPIO_LCKR_LCK0             ((uint32)0x00000001)
#define GPIO_LCKR_LCK1             ((uint32)0x00000002)
#define GPIO_LCKR_LCK2             ((uint32)0x00000004)
#define GPIO_LCKR_LCK3             ((uint32)0x00000008)
#define GPIO_LCKR_LCK4             ((uint32)0x00000010)
#define GPIO_LCKR_LCK5             ((uint32)0x00000020)
#define GPIO_LCKR_LCK6             ((uint32)0x00000040)
#define GPIO_LCKR_LCK7             ((uint32)0x00000080)
#define GPIO_LCKR_LCK8             ((uint32)0x00000100)
#define GPIO_LCKR_LCK9             ((uint32)0x00000200)
#define GPIO_LCKR_LCK10            ((uint32)0x00000400)
#define GPIO_LCKR_LCK11            ((uint32)0x00000800)
#define GPIO_LCKR_LCK12            ((uint32)0x00001000)
#define GPIO_LCKR_LCK13            ((uint32)0x00002000)
#define GPIO_LCKR_LCK14            ((uint32)0x00004000)
#define GPIO_LCKR_LCK15            ((uint32)0x00008000)
#define GPIO_LCKR_LCKK             ((uint32)0x00010000)

//! ***************** Bit definition for GPIO_AFRL register  ********************/
#define GPIO_AFRL_AFRL0            ((uint32)0x0000000F)
#define GPIO_AFRL_AFRL1            ((uint32)0x000000F0)
#define GPIO_AFRL_AFRL2            ((uint32)0x00000F00)
#define GPIO_AFRL_AFRL3            ((uint32)0x0000F000)
#define GPIO_AFRL_AFRL4            ((uint32)0x000F0000)
#define GPIO_AFRL_AFRL5            ((uint32)0x00F00000)
#define GPIO_AFRL_AFRL6            ((uint32)0x0F000000)
#define GPIO_AFRL_AFRL7            ((uint32)0xF0000000U)

//! ***************** Bit definition for GPIO_AFRH register  ********************/
#define GPIO_AFRH_AFRH0            ((uint32)0x0000000F)
#define GPIO_AFRH_AFRH1            ((uint32)0x000000F0)
#define GPIO_AFRH_AFRH2            ((uint32)0x00000F00)
#define GPIO_AFRH_AFRH3            ((uint32)0x0000F000)
#define GPIO_AFRH_AFRH4            ((uint32)0x000F0000)
#define GPIO_AFRH_AFRH5            ((uint32)0x00F00000)
#define GPIO_AFRH_AFRH6            ((uint32)0x0F000000)
#define GPIO_AFRH_AFRH7            ((uint32)0xF0000000U)

//! ***************** Bit definition for GPIO_BRR register  *********************/
#define GPIO_BRR_BR_0              ((uint32)0x00000001)
#define GPIO_BRR_BR_1              ((uint32)0x00000002)
#define GPIO_BRR_BR_2              ((uint32)0x00000004)
#define GPIO_BRR_BR_3              ((uint32)0x00000008)
#define GPIO_BRR_BR_4              ((uint32)0x00000010)
#define GPIO_BRR_BR_5              ((uint32)0x00000020)
#define GPIO_BRR_BR_6              ((uint32)0x00000040)
#define GPIO_BRR_BR_7              ((uint32)0x00000080)
#define GPIO_BRR_BR_8              ((uint32)0x00000100)
#define GPIO_BRR_BR_9              ((uint32)0x00000200)
#define GPIO_BRR_BR_10             ((uint32)0x00000400)
#define GPIO_BRR_BR_11             ((uint32)0x00000800)
#define GPIO_BRR_BR_12             ((uint32)0x00001000)
#define GPIO_BRR_BR_13             ((uint32)0x00002000)
#define GPIO_BRR_BR_14             ((uint32)0x00004000)
#define GPIO_BRR_BR_15             ((uint32)0x00008000)





//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- ADC --------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

/**
 * @brief Analog to Digital Converter (ADC)
 */
typedef struct
{
  volatile uint32 ISR;          /*!< ADC interrupt and status register,             Address offset: 0x00 */
  volatile uint32 IER;          /*!< ADC interrupt enable register,                 Address offset: 0x04 */
  volatile uint32 CR;           /*!< ADC control register,                          Address offset: 0x08 */
  volatile uint32 CFGR1;        /*!< ADC configuration register 1,                  Address offset: 0x0C */
  volatile uint32 CFGR2;        /*!< ADC configuration register 2,                  Address offset: 0x10 */
  volatile uint32 SMPR;         /*!< ADC sampling time register,                    Address offset: 0x14 */
       uint32 RESERVED1;    /*!< Reserved,                                                      0x18 */
       uint32 RESERVED2;    /*!< Reserved,                                                      0x1C */
  volatile uint32 TR1;          /*!< ADC analog watchdog 1 threshold register,      Address offset: 0x20 */
  volatile uint32 TR2;          /*!< ADC analog watchdog 2 threshold register,      Address offset: 0x24 */
  volatile uint32 CHSELR;       /*!< ADC group regular sequencer register,          Address offset: 0x28 */
  volatile uint32 TR3;          /*!< ADC analog watchdog 3 threshold register,      Address offset: 0x2C */
       uint32 RESERVED3[4]; /*!< Reserved,                                               0x30 - 0x3C */
  volatile uint32 DR;           /*!< ADC group regular data register,               Address offset: 0x40 */
       uint32 RESERVED4[23];/*!< Reserved,                                               0x44 - 0x9C */
  volatile uint32 AWD2CR;       /*!< ADC analog watchdog 2 configuration register,  Address offset: 0xA0 */
  volatile uint32 AWD3CR;       /*!< ADC analog watchdog 3 configuration register,  Address offset: 0xA4 */
       uint32 RESERVED5[3]; /*!< Reserved,                                               0xA8 - 0xB0 */
  volatile uint32 CALFACT;      /*!< ADC Calibration factor register,               Address offset: 0xB4 */
} ADC_TYPEDEF;

typedef struct
{
  volatile uint32 CCR;          /*!< ADC common configuration register,             Address offset: ADC1 base address + 0x308 */
} ADC_COMMON_TYPEDEF;

#define ADC1_COMMON         (*(ADC_COMMON_TYPEDEF *) ADC1_COMMON_BASE)
#define ADC                 (ADC1_COMMON) /* Kept for legacy purpose */
#define ADC1                (*(ADC_TYPEDEF *) ADC1_BASE)


/********************  Bit definition for ADC_ISR register  *******************/
#define ADC_ISR_ADRDY_POS              (0U)
#define ADC_ISR_ADRDY_MSK              (0x1UL << ADC_ISR_ADRDY_POS)            /*!< 0x00000001 */
#define ADC_ISR_ADRDY                  ADC_ISR_ADRDY_MSK                       /*!< ADC ready flag */
#define ADC_ISR_EOSMP_POS              (1U)
#define ADC_ISR_EOSMP_MSK              (0x1UL << ADC_ISR_EOSMP_POS)            /*!< 0x00000002 */
#define ADC_ISR_EOSMP                  ADC_ISR_EOSMP_MSK                       /*!< ADC group regular end of sampling flag */
#define ADC_ISR_EOC_POS                (2U)
#define ADC_ISR_EOC_MSK                (0x1UL << ADC_ISR_EOC_POS)              /*!< 0x00000004 */
#define ADC_ISR_EOC                    ADC_ISR_EOC_MSK                         /*!< ADC group regular end of unitary conversion flag */
#define ADC_ISR_EOS_POS                (3U)
#define ADC_ISR_EOS_MSK                (0x1UL << ADC_ISR_EOS_POS)              /*!< 0x00000008 */
#define ADC_ISR_EOS                    ADC_ISR_EOS_MSK                         /*!< ADC group regular end of sequence conversions flag */
#define ADC_ISR_OVR_POS                (4U)
#define ADC_ISR_OVR_MSK                (0x1UL << ADC_ISR_OVR_POS)              /*!< 0x00000010 */
#define ADC_ISR_OVR                    ADC_ISR_OVR_MSK                         /*!< ADC group regular overrun flag */
#define ADC_ISR_AWD1_POS               (7U)
#define ADC_ISR_AWD1_MSK               (0x1UL << ADC_ISR_AWD1_POS)             /*!< 0x00000080 */
#define ADC_ISR_AWD1                   ADC_ISR_AWD1_MSK                        /*!< ADC analog watchdog 1 flag */
#define ADC_ISR_AWD2_POS               (8U)
#define ADC_ISR_AWD2_MSK               (0x1UL << ADC_ISR_AWD2_POS)             /*!< 0x00000100 */
#define ADC_ISR_AWD2                   ADC_ISR_AWD2_MSK                        /*!< ADC analog watchdog 2 flag */
#define ADC_ISR_AWD3_POS               (9U)
#define ADC_ISR_AWD3_MSK               (0x1UL << ADC_ISR_AWD3_POS)             /*!< 0x00000200 */
#define ADC_ISR_AWD3                   ADC_ISR_AWD3_MSK                        /*!< ADC analog watchdog 3 flag */
#define ADC_ISR_EOCAL_POS              (11U)
#define ADC_ISR_EOCAL_MSK              (0x1UL << ADC_ISR_EOCAL_POS)            /*!< 0x00000800 */
#define ADC_ISR_EOCAL                  ADC_ISR_EOCAL_MSK                       /*!< ADC end of calibration flag */
#define ADC_ISR_CCRDY_POS              (13U)
#define ADC_ISR_CCRDY_MSK              (0x1UL << ADC_ISR_CCRDY_POS)            /*!< 0x00002000 */
#define ADC_ISR_CCRDY                  ADC_ISR_CCRDY_MSK                       /*!< ADC channel configuration ready flag */

/* Legacy defines */
#define ADC_ISR_EOSEQ           (ADC_ISR_EOS)

/********************  Bit definition for ADC_IER register  *******************/
#define ADC_IER_ADRDYIE_POS            (0U)
#define ADC_IER_ADRDYIE_MSK            (0x1UL << ADC_IER_ADRDYIE_POS)          /*!< 0x00000001 */
#define ADC_IER_ADRDYIE                ADC_IER_ADRDYIE_MSK                     /*!< ADC ready interrupt */
#define ADC_IER_EOSMPIE_POS            (1U)
#define ADC_IER_EOSMPIE_MSK            (0x1UL << ADC_IER_EOSMPIE_POS)          /*!< 0x00000002 */
#define ADC_IER_EOSMPIE                ADC_IER_EOSMPIE_MSK                     /*!< ADC group regular end of sampling interrupt */
#define ADC_IER_EOCIE_POS              (2U)
#define ADC_IER_EOCIE_MSK              (0x1UL << ADC_IER_EOCIE_POS)            /*!< 0x00000004 */
#define ADC_IER_EOCIE                  ADC_IER_EOCIE_MSK                       /*!< ADC group regular end of unitary conversion interrupt */
#define ADC_IER_EOSIE_POS              (3U)
#define ADC_IER_EOSIE_MSK              (0x1UL << ADC_IER_EOSIE_POS)            /*!< 0x00000008 */
#define ADC_IER_EOSIE                  ADC_IER_EOSIE_MSK                       /*!< ADC group regular end of sequence conversions interrupt */
#define ADC_IER_OVRIE_POS              (4U)
#define ADC_IER_OVRIE_MSK              (0x1UL << ADC_IER_OVRIE_POS)            /*!< 0x00000010 */
#define ADC_IER_OVRIE                  ADC_IER_OVRIE_MSK                       /*!< ADC group regular overrun interrupt */
#define ADC_IER_AWD1IE_POS             (7U)
#define ADC_IER_AWD1IE_MSK             (0x1UL << ADC_IER_AWD1IE_POS)           /*!< 0x00000080 */
#define ADC_IER_AWD1IE                 ADC_IER_AWD1IE_MSK                      /*!< ADC analog watchdog 1 interrupt */
#define ADC_IER_AWD2IE_POS             (8U)
#define ADC_IER_AWD2IE_MSK             (0x1UL << ADC_IER_AWD2IE_POS)           /*!< 0x00000100 */
#define ADC_IER_AWD2IE                 ADC_IER_AWD2IE_MSK                      /*!< ADC analog watchdog 2 interrupt */
#define ADC_IER_AWD3IE_POS             (9U)
#define ADC_IER_AWD3IE_MSK             (0x1UL << ADC_IER_AWD3IE_POS)           /*!< 0x00000200 */
#define ADC_IER_AWD3IE                 ADC_IER_AWD3IE_MSK                      /*!< ADC analog watchdog 3 interrupt */
#define ADC_IER_EOCALIE_POS            (11U)
#define ADC_IER_EOCALIE_MSK            (0x1UL << ADC_IER_EOCALIE_POS)          /*!< 0x00000800 */
#define ADC_IER_EOCALIE                ADC_IER_EOCALIE_MSK                     /*!< ADC end of calibration interrupt */
#define ADC_IER_CCRDYIE_POS            (13U)
#define ADC_IER_CCRDYIE_MSK            (0x1UL << ADC_IER_CCRDYIE_POS)          /*!< 0x00002000 */
#define ADC_IER_CCRDYIE                ADC_IER_CCRDYIE_MSK                     /*!< ADC channel configuration ready interrupt */

/* Legacy defines */
#define ADC_IER_EOSEQIE           (ADC_IER_EOSIE)

/********************  Bit definition for ADC_CR register  ********************/
#define ADC_CR_ADEN_POS                (0U)
#define ADC_CR_ADEN_MSK                (0x1UL << ADC_CR_ADEN_POS)              /*!< 0x00000001 */
#define ADC_CR_ADEN                    ADC_CR_ADEN_MSK                         /*!< ADC enable */
#define ADC_CR_ADDIS_POS               (1U)
#define ADC_CR_ADDIS_MSK               (0x1UL << ADC_CR_ADDIS_POS)             /*!< 0x00000002 */
#define ADC_CR_ADDIS                   ADC_CR_ADDIS_MSK                        /*!< ADC disable */
#define ADC_CR_ADSTART_POS             (2U)
#define ADC_CR_ADSTART_MSK             (0x1UL << ADC_CR_ADSTART_POS)           /*!< 0x00000004 */
#define ADC_CR_ADSTART                 ADC_CR_ADSTART_MSK                      /*!< ADC group regular conversion start */
#define ADC_CR_ADSTP_POS               (4U)
#define ADC_CR_ADSTP_MSK               (0x1UL << ADC_CR_ADSTP_POS)             /*!< 0x00000010 */
#define ADC_CR_ADSTP                   ADC_CR_ADSTP_MSK                        /*!< ADC group regular conversion stop */
#define ADC_CR_ADVREGEN_POS            (28U)
#define ADC_CR_ADVREGEN_MSK            (0x1UL << ADC_CR_ADVREGEN_POS)          /*!< 0x10000000 */
#define ADC_CR_ADVREGEN                ADC_CR_ADVREGEN_MSK                     /*!< ADC voltage regulator enable */
#define ADC_CR_ADCAL_POS               (31U)
#define ADC_CR_ADCAL_MSK               (0x1UL << ADC_CR_ADCAL_POS)             /*!< 0x80000000 */
#define ADC_CR_ADCAL                   ADC_CR_ADCAL_MSK                        /*!< ADC calibration */

/********************  Bit definition for ADC_CFGR1 register  *****************/
#define ADC_CFGR1_DMAEN_POS            (0U)
#define ADC_CFGR1_DMAEN_MSK            (0x1UL << ADC_CFGR1_DMAEN_POS)          /*!< 0x00000001 */
#define ADC_CFGR1_DMAEN                ADC_CFGR1_DMAEN_MSK                     /*!< ADC DMA transfer enable */
#define ADC_CFGR1_DMACFG_POS           (1U)
#define ADC_CFGR1_DMACFG_MSK           (0x1UL << ADC_CFGR1_DMACFG_POS)         /*!< 0x00000002 */
#define ADC_CFGR1_DMACFG               ADC_CFGR1_DMACFG_MSK                    /*!< ADC DMA transfer configuration */

#define ADC_CFGR1_SCANDIR_POS          (2U)
#define ADC_CFGR1_SCANDIR_MSK          (0x1UL << ADC_CFGR1_SCANDIR_POS)        /*!< 0x00000004 */
#define ADC_CFGR1_SCANDIR              ADC_CFGR1_SCANDIR_MSK                   /*!< ADC group regular sequencer scan direction */

#define ADC_CFGR1_RES_POS              (3U)
#define ADC_CFGR1_RES_MSK              (0x3UL << ADC_CFGR1_RES_POS)            /*!< 0x00000018 */
#define ADC_CFGR1_RES                  ADC_CFGR1_RES_MSK                       /*!< ADC data resolution */
#define ADC_CFGR1_RES_0                (0x1U << ADC_CFGR1_RES_POS)             /*!< 0x00000008 */
#define ADC_CFGR1_RES_1                (0x2U << ADC_CFGR1_RES_POS)             /*!< 0x00000010 */

#define ADC_CFGR1_ALIGN_POS            (5U)
#define ADC_CFGR1_ALIGN_MSK            (0x1UL << ADC_CFGR1_ALIGN_POS)          /*!< 0x00000020 */
#define ADC_CFGR1_ALIGN                ADC_CFGR1_ALIGN_MSK                     /*!< ADC data alignement */

#define ADC_CFGR1_EXTSEL_POS           (6U)
#define ADC_CFGR1_EXTSEL_MSK           (0x7UL << ADC_CFGR1_EXTSEL_POS)         /*!< 0x000001C0 */
#define ADC_CFGR1_EXTSEL               ADC_CFGR1_EXTSEL_MSK                    /*!< ADC group regular external trigger source */
#define ADC_CFGR1_EXTSEL_0             (0x1UL << ADC_CFGR1_EXTSEL_POS)         /*!< 0x00000040 */
#define ADC_CFGR1_EXTSEL_1             (0x2UL << ADC_CFGR1_EXTSEL_POS)         /*!< 0x00000080 */
#define ADC_CFGR1_EXTSEL_2             (0x4UL << ADC_CFGR1_EXTSEL_POS)         /*!< 0x00000100 */

#define ADC_CFGR1_EXTEN_POS            (10U)
#define ADC_CFGR1_EXTEN_MSK            (0x3UL << ADC_CFGR1_EXTEN_POS)          /*!< 0x00000C00 */
#define ADC_CFGR1_EXTEN                ADC_CFGR1_EXTEN_MSK                     /*!< ADC group regular external trigger polarity */
#define ADC_CFGR1_EXTEN_0              (0x1UL << ADC_CFGR1_EXTEN_POS)          /*!< 0x00000400 */
#define ADC_CFGR1_EXTEN_1              (0x2UL << ADC_CFGR1_EXTEN_POS)          /*!< 0x00000800 */

#define ADC_CFGR1_OVRMOD_POS           (12U)
#define ADC_CFGR1_OVRMOD_MSK           (0x1UL << ADC_CFGR1_OVRMOD_POS)         /*!< 0x00001000 */
#define ADC_CFGR1_OVRMOD               ADC_CFGR1_OVRMOD_MSK                    /*!< ADC group regular overrun configuration */
#define ADC_CFGR1_CONT_POS             (13U)
#define ADC_CFGR1_CONT_MSK             (0x1UL << ADC_CFGR1_CONT_POS)           /*!< 0x00002000 */
#define ADC_CFGR1_CONT                 ADC_CFGR1_CONT_MSK                      /*!< ADC group regular continuous conversion mode */
#define ADC_CFGR1_WAIT_POS             (14U)
#define ADC_CFGR1_WAIT_MSK             (0x1UL << ADC_CFGR1_WAIT_POS)           /*!< 0x00004000 */
#define ADC_CFGR1_WAIT                 ADC_CFGR1_WAIT_MSK                      /*!< ADC low power auto wait */
#define ADC_CFGR1_AUTOFF_POS           (15U)
#define ADC_CFGR1_AUTOFF_MSK           (0x1UL << ADC_CFGR1_AUTOFF_POS)         /*!< 0x00008000 */
#define ADC_CFGR1_AUTOFF               ADC_CFGR1_AUTOFF_MSK                    /*!< ADC low power auto power off */
#define ADC_CFGR1_DISCEN_POS           (16U)
#define ADC_CFGR1_DISCEN_MSK           (0x1UL << ADC_CFGR1_DISCEN_POS)         /*!< 0x00010000 */
#define ADC_CFGR1_DISCEN               ADC_CFGR1_DISCEN_MSK                    /*!< ADC group regular sequencer discontinuous mode */
#define ADC_CFGR1_CHSELRMOD_POS        (21U)
#define ADC_CFGR1_CHSELRMOD_MSK        (0x1UL << ADC_CFGR1_CHSELRMOD_POS)      /*!< 0x00200000 */
#define ADC_CFGR1_CHSELRMOD            ADC_CFGR1_CHSELRMOD_MSK                 /*!< ADC group regular sequencer mode */

#define ADC_CFGR1_AWD1SGL_POS          (22U)
#define ADC_CFGR1_AWD1SGL_MSK          (0x1UL << ADC_CFGR1_AWD1SGL_POS)        /*!< 0x00400000 */
#define ADC_CFGR1_AWD1SGL              ADC_CFGR1_AWD1SGL_MSK                   /*!< ADC analog watchdog 1 monitoring a single channel or all channels */
#define ADC_CFGR1_AWD1EN_POS           (23U)
#define ADC_CFGR1_AWD1EN_MSK           (0x1UL << ADC_CFGR1_AWD1EN_POS)         /*!< 0x00800000 */
#define ADC_CFGR1_AWD1EN               ADC_CFGR1_AWD1EN_MSK                    /*!< ADC analog watchdog 1 enable on scope ADC group regular */

#define ADC_CFGR1_AWD1CH_POS           (26U)
#define ADC_CFGR1_AWD1CH_MSK           (0x1FUL << ADC_CFGR1_AWD1CH_POS)        /*!< 0x7C000000 */
#define ADC_CFGR1_AWD1CH               ADC_CFGR1_AWD1CH_MSK                    /*!< ADC analog watchdog 1 monitored channel selection */
#define ADC_CFGR1_AWD1CH_0             (0x01UL << ADC_CFGR1_AWD1CH_POS)        /*!< 0x04000000 */
#define ADC_CFGR1_AWD1CH_1             (0x02UL << ADC_CFGR1_AWD1CH_POS)        /*!< 0x08000000 */
#define ADC_CFGR1_AWD1CH_2             (0x04UL << ADC_CFGR1_AWD1CH_POS)        /*!< 0x10000000 */
#define ADC_CFGR1_AWD1CH_3             (0x08UL << ADC_CFGR1_AWD1CH_POS)        /*!< 0x20000000 */
#define ADC_CFGR1_AWD1CH_4             (0x10UL << ADC_CFGR1_AWD1CH_POS)        /*!< 0x40000000 */

/* Legacy defines */
#define ADC_CFGR1_AUTDLY          (ADC_CFGR1_WAIT)

/********************  Bit definition for ADC_CFGR2 register  *****************/
#define ADC_CFGR2_OVSE_POS             (0U)
#define ADC_CFGR2_OVSE_MSK             (0x1UL << ADC_CFGR2_OVSE_POS)           /*!< 0x00000001 */
#define ADC_CFGR2_OVSE                 ADC_CFGR2_OVSE_MSK                      /*!< ADC oversampler enable on scope ADC group regular */

#define ADC_CFGR2_OVSR_POS             (2U)
#define ADC_CFGR2_OVSR_MSK             (0x7UL << ADC_CFGR2_OVSR_POS)           /*!< 0x0000001C */
#define ADC_CFGR2_OVSR                 ADC_CFGR2_OVSR_MSK                      /*!< ADC oversampling ratio */
#define ADC_CFGR2_OVSR_0               (0x1UL << ADC_CFGR2_OVSR_POS)           /*!< 0x00000004 */
#define ADC_CFGR2_OVSR_1               (0x2UL << ADC_CFGR2_OVSR_POS)           /*!< 0x00000008 */
#define ADC_CFGR2_OVSR_2               (0x4UL << ADC_CFGR2_OVSR_POS)           /*!< 0x00000010 */

#define ADC_CFGR2_OVSS_POS             (5U)
#define ADC_CFGR2_OVSS_MSK             (0xFUL << ADC_CFGR2_OVSS_POS)           /*!< 0x000001E0 */
#define ADC_CFGR2_OVSS                 ADC_CFGR2_OVSS_MSK                      /*!< ADC oversampling shift */
#define ADC_CFGR2_OVSS_0               (0x1UL << ADC_CFGR2_OVSS_POS)           /*!< 0x00000020 */
#define ADC_CFGR2_OVSS_1               (0x2UL << ADC_CFGR2_OVSS_POS)           /*!< 0x00000040 */
#define ADC_CFGR2_OVSS_2               (0x4UL << ADC_CFGR2_OVSS_POS)           /*!< 0x00000080 */
#define ADC_CFGR2_OVSS_3               (0x8UL << ADC_CFGR2_OVSS_POS)           /*!< 0x00000100 */

#define ADC_CFGR2_TOVS_POS             (9U)
#define ADC_CFGR2_TOVS_MSK             (0x1UL << ADC_CFGR2_TOVS_POS)           /*!< 0x00000200 */
#define ADC_CFGR2_TOVS                 ADC_CFGR2_TOVS_MSK                      /*!< ADC oversampling discontinuous mode (triggered mode) for ADC group regular */

#define ADC_CFGR2_LFTRIG_POS           (29U)
#define ADC_CFGR2_LFTRIG_MSK           (0x1UL << ADC_CFGR2_LFTRIG_POS)         /*!< 0x20000000 */
#define ADC_CFGR2_LFTRIG               ADC_CFGR2_LFTRIG_MSK                    /*!< ADC low frequency trigger mode */

#define ADC_CFGR2_CKMODE_POS           (30U)
#define ADC_CFGR2_CKMODE_MSK           (0x3UL << ADC_CFGR2_CKMODE_POS)         /*!< 0xC0000000 */
#define ADC_CFGR2_CKMODE               ADC_CFGR2_CKMODE_MSK                    /*!< ADC clock source and prescaler (prescaler only for clock source synchronous) */
#define ADC_CFGR2_CKMODE_1             (0x2UL << ADC_CFGR2_CKMODE_POS)         /*!< 0x80000000 */
#define ADC_CFGR2_CKMODE_0             (0x1UL << ADC_CFGR2_CKMODE_POS)         /*!< 0x40000000 */

/********************  Bit definition for ADC_SMPR register  ******************/
#define ADC_SMPR_SMP1_POS              (0U)
#define ADC_SMPR_SMP1_MSK              (0x7UL << ADC_SMPR_SMP1_POS)            /*!< 0x00000007 */
#define ADC_SMPR_SMP1                  ADC_SMPR_SMP1_MSK                       /*!< ADC group of channels sampling time 1 */
#define ADC_SMPR_SMP1_0                (0x1UL << ADC_SMPR_SMP1_POS)            /*!< 0x00000001 */
#define ADC_SMPR_SMP1_1                (0x2UL << ADC_SMPR_SMP1_POS)            /*!< 0x00000002 */
#define ADC_SMPR_SMP1_2                (0x4UL << ADC_SMPR_SMP1_POS)            /*!< 0x00000004 */

#define ADC_SMPR_SMP2_POS              (4U)
#define ADC_SMPR_SMP2_MSK              (0x7UL << ADC_SMPR_SMP2_POS)            /*!< 0x00000070 */
#define ADC_SMPR_SMP2                  ADC_SMPR_SMP2_MSK                       /*!< ADC group of channels sampling time 2 */
#define ADC_SMPR_SMP2_0                (0x1UL << ADC_SMPR_SMP2_POS)            /*!< 0x00000010 */
#define ADC_SMPR_SMP2_1                (0x2UL << ADC_SMPR_SMP2_POS)            /*!< 0x00000020 */
#define ADC_SMPR_SMP2_2                (0x4UL << ADC_SMPR_SMP2_POS)            /*!< 0x00000040 */

#define ADC_SMPR_SMPSEL_POS            (8U)
#define ADC_SMPR_SMPSEL_MSK            (0x7FFFFUL << ADC_SMPR_SMPSEL_POS)      /*!< 0x07FFFF00 */
#define ADC_SMPR_SMPSEL                ADC_SMPR_SMPSEL_MSK                     /*!< ADC all channels sampling time selection */
#define ADC_SMPR_SMPSEL0_POS           (8U)
#define ADC_SMPR_SMPSEL0_MSK           (0x1UL << ADC_SMPR_SMPSEL0_POS)         /*!< 0x00000100 */
#define ADC_SMPR_SMPSEL0               ADC_SMPR_SMPSEL0_MSK                    /*!< ADC channel 0 sampling time selection */
#define ADC_SMPR_SMPSEL1_POS           (9U)
#define ADC_SMPR_SMPSEL1_MSK           (0x1UL << ADC_SMPR_SMPSEL1_POS)         /*!< 0x00000200 */
#define ADC_SMPR_SMPSEL1               ADC_SMPR_SMPSEL1_MSK                    /*!< ADC channel 1 sampling time selection */
#define ADC_SMPR_SMPSEL2_POS           (10U)
#define ADC_SMPR_SMPSEL2_MSK           (0x1UL << ADC_SMPR_SMPSEL2_POS)         /*!< 0x00000400 */
#define ADC_SMPR_SMPSEL2               ADC_SMPR_SMPSEL2_MSK                    /*!< ADC channel 2 sampling time selection */
#define ADC_SMPR_SMPSEL3_POS           (11U)
#define ADC_SMPR_SMPSEL3_MSK           (0x1UL << ADC_SMPR_SMPSEL3_POS)         /*!< 0x00000800 */
#define ADC_SMPR_SMPSEL3               ADC_SMPR_SMPSEL3_MSK                    /*!< ADC channel 3 sampling time selection */
#define ADC_SMPR_SMPSEL4_POS           (12U)
#define ADC_SMPR_SMPSEL4_MSK           (0x1UL << ADC_SMPR_SMPSEL4_POS)         /*!< 0x00001000 */
#define ADC_SMPR_SMPSEL4               ADC_SMPR_SMPSEL4_MSK                    /*!< ADC channel 4 sampling time selection */
#define ADC_SMPR_SMPSEL5_POS           (13U)
#define ADC_SMPR_SMPSEL5_MSK           (0x1UL << ADC_SMPR_SMPSEL5_POS)         /*!< 0x00002000 */
#define ADC_SMPR_SMPSEL5               ADC_SMPR_SMPSEL5_MSK                    /*!< ADC channel 5 sampling time selection */
#define ADC_SMPR_SMPSEL6_POS           (14U)
#define ADC_SMPR_SMPSEL6_MSK           (0x1UL << ADC_SMPR_SMPSEL6_POS)         /*!< 0x00004000 */
#define ADC_SMPR_SMPSEL6               ADC_SMPR_SMPSEL6_MSK                    /*!< ADC channel 6 sampling time selection */
#define ADC_SMPR_SMPSEL7_POS           (15U)
#define ADC_SMPR_SMPSEL7_MSK           (0x1UL << ADC_SMPR_SMPSEL7_POS)         /*!< 0x00008000 */
#define ADC_SMPR_SMPSEL7               ADC_SMPR_SMPSEL7_MSK                    /*!< ADC channel 7 sampling time selection */
#define ADC_SMPR_SMPSEL8_POS           (16U)
#define ADC_SMPR_SMPSEL8_MSK           (0x1UL << ADC_SMPR_SMPSEL8_POS)         /*!< 0x00010000 */
#define ADC_SMPR_SMPSEL8               ADC_SMPR_SMPSEL8_MSK                    /*!< ADC channel 8 sampling time selection */
#define ADC_SMPR_SMPSEL9_POS           (17U)
#define ADC_SMPR_SMPSEL9_MSK           (0x1UL << ADC_SMPR_SMPSEL9_POS)         /*!< 0x00020000 */
#define ADC_SMPR_SMPSEL9               ADC_SMPR_SMPSEL9_MSK                    /*!< ADC channel 9 sampling time selection */
#define ADC_SMPR_SMPSEL10_POS          (18U)
#define ADC_SMPR_SMPSEL10_MSK          (0x1UL << ADC_SMPR_SMPSEL10_POS)        /*!< 0x00040000 */
#define ADC_SMPR_SMPSEL10              ADC_SMPR_SMPSEL10_MSK                   /*!< ADC channel 10 sampling time selection */
#define ADC_SMPR_SMPSEL11_POS          (19U)
#define ADC_SMPR_SMPSEL11_MSK          (0x1UL << ADC_SMPR_SMPSEL11_POS)        /*!< 0x00080000 */
#define ADC_SMPR_SMPSEL11              ADC_SMPR_SMPSEL11_MSK                   /*!< ADC channel 11 sampling time selection */
#define ADC_SMPR_SMPSEL12_POS          (20U)
#define ADC_SMPR_SMPSEL12_MSK          (0x1UL << ADC_SMPR_SMPSEL12_POS)        /*!< 0x00100000 */
#define ADC_SMPR_SMPSEL12              ADC_SMPR_SMPSEL12_MSK                   /*!< ADC channel 12 sampling time selection */
#define ADC_SMPR_SMPSEL13_POS          (21U)
#define ADC_SMPR_SMPSEL13_MSK          (0x1UL << ADC_SMPR_SMPSEL13_POS)        /*!< 0x00200000 */
#define ADC_SMPR_SMPSEL13              ADC_SMPR_SMPSEL13_MSK                   /*!< ADC channel 13 sampling time selection */
#define ADC_SMPR_SMPSEL14_POS          (22U)
#define ADC_SMPR_SMPSEL14_MSK          (0x1UL << ADC_SMPR_SMPSEL14_POS)        /*!< 0x00400000 */
#define ADC_SMPR_SMPSEL14              ADC_SMPR_SMPSEL14_MSK                   /*!< ADC channel 14 sampling time selection */
#define ADC_SMPR_SMPSEL15_POS          (23U)
#define ADC_SMPR_SMPSEL15_MSK          (0x1UL << ADC_SMPR_SMPSEL15_POS)        /*!< 0x00800000 */
#define ADC_SMPR_SMPSEL15              ADC_SMPR_SMPSEL15_MSK                   /*!< ADC channel 15 sampling time selection */
#define ADC_SMPR_SMPSEL16_POS          (24U)
#define ADC_SMPR_SMPSEL16_MSK          (0x1UL << ADC_SMPR_SMPSEL16_POS)        /*!< 0x01000000 */
#define ADC_SMPR_SMPSEL16              ADC_SMPR_SMPSEL16_MSK                   /*!< ADC channel 16 sampling time selection */
#define ADC_SMPR_SMPSEL17_POS          (25U)
#define ADC_SMPR_SMPSEL17_MSK          (0x1UL << ADC_SMPR_SMPSEL17_POS)        /*!< 0x02000000 */
#define ADC_SMPR_SMPSEL17              ADC_SMPR_SMPSEL17_MSK                   /*!< ADC channel 17 sampling time selection */
#define ADC_SMPR_SMPSEL18_POS          (26U)
#define ADC_SMPR_SMPSEL18_MSK          (0x1UL << ADC_SMPR_SMPSEL18_POS)        /*!< 0x04000000 */
#define ADC_SMPR_SMPSEL18              ADC_SMPR_SMPSEL18_MSK                   /*!< ADC channel 18 sampling time selection */

/********************  Bit definition for ADC_TR1 register  *******************/
#define ADC_TR1_LT1_POS                (0U)
#define ADC_TR1_LT1_MSK                (0xFFFUL << ADC_TR1_LT1_POS)            /*!< 0x00000FFF */
#define ADC_TR1_LT1                    ADC_TR1_LT1_MSK                         /*!< ADC analog watchdog 1 threshold low */
#define ADC_TR1_LT1_0                  (0x001UL << ADC_TR1_LT1_POS)            /*!< 0x00000001 */
#define ADC_TR1_LT1_1                  (0x002UL << ADC_TR1_LT1_POS)            /*!< 0x00000002 */
#define ADC_TR1_LT1_2                  (0x004UL << ADC_TR1_LT1_POS)            /*!< 0x00000004 */
#define ADC_TR1_LT1_3                  (0x008UL << ADC_TR1_LT1_POS)            /*!< 0x00000008 */
#define ADC_TR1_LT1_4                  (0x010UL << ADC_TR1_LT1_POS)            /*!< 0x00000010 */
#define ADC_TR1_LT1_5                  (0x020UL << ADC_TR1_LT1_POS)            /*!< 0x00000020 */
#define ADC_TR1_LT1_6                  (0x040UL << ADC_TR1_LT1_POS)            /*!< 0x00000040 */
#define ADC_TR1_LT1_7                  (0x080UL << ADC_TR1_LT1_POS)            /*!< 0x00000080 */
#define ADC_TR1_LT1_8                  (0x100UL << ADC_TR1_LT1_POS)            /*!< 0x00000100 */
#define ADC_TR1_LT1_9                  (0x200UL << ADC_TR1_LT1_POS)            /*!< 0x00000200 */
#define ADC_TR1_LT1_10                 (0x400UL << ADC_TR1_LT1_POS)            /*!< 0x00000400 */
#define ADC_TR1_LT1_11                 (0x800UL << ADC_TR1_LT1_POS)            /*!< 0x00000800 */

#define ADC_TR1_HT1_POS                (16U)
#define ADC_TR1_HT1_MSK                (0xFFFUL << ADC_TR1_HT1_POS)            /*!< 0x0FFF0000 */
#define ADC_TR1_HT1                    ADC_TR1_HT1_MSK                         /*!< ADC Analog watchdog 1 threshold high */
#define ADC_TR1_HT1_0                  (0x001UL << ADC_TR1_HT1_POS)            /*!< 0x00010000 */
#define ADC_TR1_HT1_1                  (0x002UL << ADC_TR1_HT1_POS)            /*!< 0x00020000 */
#define ADC_TR1_HT1_2                  (0x004UL << ADC_TR1_HT1_POS)            /*!< 0x00040000 */
#define ADC_TR1_HT1_3                  (0x008UL << ADC_TR1_HT1_POS)            /*!< 0x00080000 */
#define ADC_TR1_HT1_4                  (0x010UL << ADC_TR1_HT1_POS)            /*!< 0x00100000 */
#define ADC_TR1_HT1_5                  (0x020UL << ADC_TR1_HT1_POS)            /*!< 0x00200000 */
#define ADC_TR1_HT1_6                  (0x040UL << ADC_TR1_HT1_POS)            /*!< 0x00400000 */
#define ADC_TR1_HT1_7                  (0x080UL << ADC_TR1_HT1_POS)            /*!< 0x00800000 */
#define ADC_TR1_HT1_8                  (0x100UL << ADC_TR1_HT1_POS)            /*!< 0x01000000 */
#define ADC_TR1_HT1_9                  (0x200UL << ADC_TR1_HT1_POS)            /*!< 0x02000000 */
#define ADC_TR1_HT1_10                 (0x400UL << ADC_TR1_HT1_POS)            /*!< 0x04000000 */
#define ADC_TR1_HT1_11                 (0x800UL << ADC_TR1_HT1_POS)            /*!< 0x08000000 */

/********************  Bit definition for ADC_TR2 register  *******************/
#define ADC_TR2_LT2_POS                (0U)
#define ADC_TR2_LT2_MSK                (0xFFFUL << ADC_TR2_LT2_POS)            /*!< 0x00000FFF */
#define ADC_TR2_LT2                    ADC_TR2_LT2_MSK                         /*!< ADC analog watchdog 2 threshold low */
#define ADC_TR2_LT2_0                  (0x001UL << ADC_TR2_LT2_POS)            /*!< 0x00000001 */
#define ADC_TR2_LT2_1                  (0x002UL << ADC_TR2_LT2_POS)            /*!< 0x00000002 */
#define ADC_TR2_LT2_2                  (0x004UL << ADC_TR2_LT2_POS)            /*!< 0x00000004 */
#define ADC_TR2_LT2_3                  (0x008UL << ADC_TR2_LT2_POS)            /*!< 0x00000008 */
#define ADC_TR2_LT2_4                  (0x010UL << ADC_TR2_LT2_POS)            /*!< 0x00000010 */
#define ADC_TR2_LT2_5                  (0x020UL << ADC_TR2_LT2_POS)            /*!< 0x00000020 */
#define ADC_TR2_LT2_6                  (0x040UL << ADC_TR2_LT2_POS)            /*!< 0x00000040 */
#define ADC_TR2_LT2_7                  (0x080UL << ADC_TR2_LT2_POS)            /*!< 0x00000080 */
#define ADC_TR2_LT2_8                  (0x100UL << ADC_TR2_LT2_POS)            /*!< 0x00000100 */
#define ADC_TR2_LT2_9                  (0x200UL << ADC_TR2_LT2_POS)            /*!< 0x00000200 */
#define ADC_TR2_LT2_10                 (0x400UL << ADC_TR2_LT2_POS)            /*!< 0x00000400 */
#define ADC_TR2_LT2_11                 (0x800UL << ADC_TR2_LT2_POS)            /*!< 0x00000800 */

#define ADC_TR2_HT2_POS                (16U)
#define ADC_TR2_HT2_MSK                (0xFFFUL << ADC_TR2_HT2_POS)            /*!< 0x0FFF0000 */
#define ADC_TR2_HT2                    ADC_TR2_HT2_MSK                         /*!< ADC analog watchdog 2 threshold high */
#define ADC_TR2_HT2_0                  (0x001UL << ADC_TR2_HT2_POS)            /*!< 0x00010000 */
#define ADC_TR2_HT2_1                  (0x002UL << ADC_TR2_HT2_POS)            /*!< 0x00020000 */
#define ADC_TR2_HT2_2                  (0x004UL << ADC_TR2_HT2_POS)            /*!< 0x00040000 */
#define ADC_TR2_HT2_3                  (0x008UL << ADC_TR2_HT2_POS)            /*!< 0x00080000 */
#define ADC_TR2_HT2_4                  (0x010UL << ADC_TR2_HT2_POS)            /*!< 0x00100000 */
#define ADC_TR2_HT2_5                  (0x020UL << ADC_TR2_HT2_POS)            /*!< 0x00200000 */
#define ADC_TR2_HT2_6                  (0x040UL << ADC_TR2_HT2_POS)            /*!< 0x00400000 */
#define ADC_TR2_HT2_7                  (0x080UL << ADC_TR2_HT2_POS)            /*!< 0x00800000 */
#define ADC_TR2_HT2_8                  (0x100UL << ADC_TR2_HT2_POS)            /*!< 0x01000000 */
#define ADC_TR2_HT2_9                  (0x200UL << ADC_TR2_HT2_POS)            /*!< 0x02000000 */
#define ADC_TR2_HT2_10                 (0x400UL << ADC_TR2_HT2_POS)            /*!< 0x04000000 */
#define ADC_TR2_HT2_11                 (0x800UL << ADC_TR2_HT2_POS)            /*!< 0x08000000 */

/********************  Bit definition for ADC_CHSELR register  ****************/
#define ADC_CHSELR_CHSEL_POS           (0U)
#define ADC_CHSELR_CHSEL_MSK           (0x7FFFFUL << ADC_CHSELR_CHSEL_POS)     /*!< 0x0007FFFF */
#define ADC_CHSELR_CHSEL               ADC_CHSELR_CHSEL_MSK                    /*!< ADC group regular sequencer channels, available when ADC_CFGR1_CHSELRMOD is reset */
#define ADC_CHSELR_CHSEL18_POS         (18U)
#define ADC_CHSELR_CHSEL18_MSK         (0x1UL << ADC_CHSELR_CHSEL18_POS)       /*!< 0x00040000 */
#define ADC_CHSELR_CHSEL18             ADC_CHSELR_CHSEL18_MSK                  /*!< ADC group regular sequencer channel 18, available when ADC_CFGR1_CHSELRMOD is reset */
#define ADC_CHSELR_CHSEL17_POS         (17U)
#define ADC_CHSELR_CHSEL17_MSK         (0x1UL << ADC_CHSELR_CHSEL17_POS)       /*!< 0x00020000 */
#define ADC_CHSELR_CHSEL17             ADC_CHSELR_CHSEL17_MSK                  /*!< ADC group regular sequencer channel 17, available when ADC_CFGR1_CHSELRMOD is reset */
#define ADC_CHSELR_CHSEL16_POS         (16U)
#define ADC_CHSELR_CHSEL16_MSK         (0x1UL << ADC_CHSELR_CHSEL16_POS)       /*!< 0x00010000 */
#define ADC_CHSELR_CHSEL16             ADC_CHSELR_CHSEL16_MSK                  /*!< ADC group regular sequencer channel 16, available when ADC_CFGR1_CHSELRMOD is reset */
#define ADC_CHSELR_CHSEL15_POS         (15U)
#define ADC_CHSELR_CHSEL15_MSK         (0x1UL << ADC_CHSELR_CHSEL15_POS)       /*!< 0x00008000 */
#define ADC_CHSELR_CHSEL15             ADC_CHSELR_CHSEL15_MSK                  /*!< ADC group regular sequencer channel 15, available when ADC_CFGR1_CHSELRMOD is reset */
#define ADC_CHSELR_CHSEL14_POS         (14U)
#define ADC_CHSELR_CHSEL14_MSK         (0x1UL << ADC_CHSELR_CHSEL14_POS)       /*!< 0x00004000 */
#define ADC_CHSELR_CHSEL14             ADC_CHSELR_CHSEL14_MSK                  /*!< ADC group regular sequencer channel 14, available when ADC_CFGR1_CHSELRMOD is reset */
#define ADC_CHSELR_CHSEL13_POS         (13U)
#define ADC_CHSELR_CHSEL13_MSK         (0x1UL << ADC_CHSELR_CHSEL13_POS)       /*!< 0x00002000 */
#define ADC_CHSELR_CHSEL13             ADC_CHSELR_CHSEL13_MSK                  /*!< ADC group regular sequencer channel 13, available when ADC_CFGR1_CHSELRMOD is reset */
#define ADC_CHSELR_CHSEL12_POS         (12U)
#define ADC_CHSELR_CHSEL12_MSK         (0x1UL << ADC_CHSELR_CHSEL12_POS)       /*!< 0x00001000 */
#define ADC_CHSELR_CHSEL12             ADC_CHSELR_CHSEL12_MSK                  /*!< ADC group regular sequencer channel 12, available when ADC_CFGR1_CHSELRMOD is reset */
#define ADC_CHSELR_CHSEL11_POS         (11U)
#define ADC_CHSELR_CHSEL11_MSK         (0x1UL << ADC_CHSELR_CHSEL11_POS)       /*!< 0x00000800 */
#define ADC_CHSELR_CHSEL11             ADC_CHSELR_CHSEL11_MSK                  /*!< ADC group regular sequencer channel 11, available when ADC_CFGR1_CHSELRMOD is reset */
#define ADC_CHSELR_CHSEL10_POS         (10U)
#define ADC_CHSELR_CHSEL10_MSK         (0x1UL << ADC_CHSELR_CHSEL10_POS)       /*!< 0x00000400 */
#define ADC_CHSELR_CHSEL10             ADC_CHSELR_CHSEL10_MSK                  /*!< ADC group regular sequencer channel 10, available when ADC_CFGR1_CHSELRMOD is reset */
#define ADC_CHSELR_CHSEL9_POS          (9U)
#define ADC_CHSELR_CHSEL9_MSK          (0x1UL << ADC_CHSELR_CHSEL9_POS)        /*!< 0x00000200 */
#define ADC_CHSELR_CHSEL9              ADC_CHSELR_CHSEL9_MSK                   /*!< ADC group regular sequencer channel 9, available when ADC_CFGR1_CHSELRMOD is reset */
#define ADC_CHSELR_CHSEL8_POS          (8U)
#define ADC_CHSELR_CHSEL8_MSK          (0x1UL << ADC_CHSELR_CHSEL8_POS)        /*!< 0x00000100 */
#define ADC_CHSELR_CHSEL8              ADC_CHSELR_CHSEL8_MSK                   /*!< ADC group regular sequencer channel 8, available when ADC_CFGR1_CHSELRMOD is reset */
#define ADC_CHSELR_CHSEL7_POS          (7U)
#define ADC_CHSELR_CHSEL7_MSK          (0x1UL << ADC_CHSELR_CHSEL7_POS)        /*!< 0x00000080 */
#define ADC_CHSELR_CHSEL7              ADC_CHSELR_CHSEL7_MSK                   /*!< ADC group regular sequencer channel 7, available when ADC_CFGR1_CHSELRMOD is reset */
#define ADC_CHSELR_CHSEL6_POS          (6U)
#define ADC_CHSELR_CHSEL6_MSK          (0x1UL << ADC_CHSELR_CHSEL6_POS)        /*!< 0x00000040 */
#define ADC_CHSELR_CHSEL6              ADC_CHSELR_CHSEL6_MSK                   /*!< ADC group regular sequencer channel 6, available when ADC_CFGR1_CHSELRMOD is reset */
#define ADC_CHSELR_CHSEL5_POS          (5U)
#define ADC_CHSELR_CHSEL5_MSK          (0x1UL << ADC_CHSELR_CHSEL5_POS)        /*!< 0x00000020 */
#define ADC_CHSELR_CHSEL5              ADC_CHSELR_CHSEL5_MSK                   /*!< ADC group regular sequencer channel 5, available when ADC_CFGR1_CHSELRMOD is reset */
#define ADC_CHSELR_CHSEL4_POS          (4U)
#define ADC_CHSELR_CHSEL4_MSK          (0x1UL << ADC_CHSELR_CHSEL4_POS)        /*!< 0x00000010 */
#define ADC_CHSELR_CHSEL4              ADC_CHSELR_CHSEL4_MSK                   /*!< ADC group regular sequencer channel 4, available when ADC_CFGR1_CHSELRMOD is reset */
#define ADC_CHSELR_CHSEL3_POS          (3U)
#define ADC_CHSELR_CHSEL3_MSK          (0x1UL << ADC_CHSELR_CHSEL3_POS)        /*!< 0x00000008 */
#define ADC_CHSELR_CHSEL3              ADC_CHSELR_CHSEL3_MSK                   /*!< ADC group regular sequencer channel 3, available when ADC_CFGR1_CHSELRMOD is reset */
#define ADC_CHSELR_CHSEL2_POS          (2U)
#define ADC_CHSELR_CHSEL2_MSK          (0x1UL << ADC_CHSELR_CHSEL2_POS)        /*!< 0x00000004 */
#define ADC_CHSELR_CHSEL2              ADC_CHSELR_CHSEL2_MSK                   /*!< ADC group regular sequencer channel 2, available when ADC_CFGR1_CHSELRMOD is reset */
#define ADC_CHSELR_CHSEL1_POS          (1U)
#define ADC_CHSELR_CHSEL1_MSK          (0x1UL << ADC_CHSELR_CHSEL1_POS)        /*!< 0x00000002 */
#define ADC_CHSELR_CHSEL1              ADC_CHSELR_CHSEL1_MSK                   /*!< ADC group regular sequencer channel 1, available when ADC_CFGR1_CHSELRMOD is reset */
#define ADC_CHSELR_CHSEL0_POS          (0U)
#define ADC_CHSELR_CHSEL0_MSK          (0x1UL << ADC_CHSELR_CHSEL0_POS)        /*!< 0x00000001 */
#define ADC_CHSELR_CHSEL0              ADC_CHSELR_CHSEL0_MSK                   /*!< ADC group regular sequencer channel 0, available when ADC_CFGR1_CHSELRMOD is reset */

#define ADC_CHSELR_SQ_ALL_POS          (0U)
#define ADC_CHSELR_SQ_ALL_MSK          (0xFFFFFFFFUL << ADC_CHSELR_SQ_ALL_POS) /*!< 0xFFFFFFFF */
#define ADC_CHSELR_SQ_ALL              ADC_CHSELR_SQ_ALL_MSK                   /*!< ADC group regular sequencer all ranks, available when ADC_CFGR1_CHSELRMOD is set */

#define ADC_CHSELR_SQ8_POS             (28U)
#define ADC_CHSELR_SQ8_MSK             (0xFUL << ADC_CHSELR_SQ8_POS)           /*!< 0xF0000000 */
#define ADC_CHSELR_SQ8                 ADC_CHSELR_SQ8_MSK                      /*!< ADC group regular sequencer rank 8, available when ADC_CFGR1_CHSELRMOD is set */
#define ADC_CHSELR_SQ8_0               (0x1UL << ADC_CHSELR_SQ8_POS)           /*!< 0x10000000 */
#define ADC_CHSELR_SQ8_1               (0x2UL << ADC_CHSELR_SQ8_POS)           /*!< 0x20000000 */
#define ADC_CHSELR_SQ8_2               (0x4UL << ADC_CHSELR_SQ8_POS)           /*!< 0x40000000 */
#define ADC_CHSELR_SQ8_3               (0x8UL << ADC_CHSELR_SQ8_POS)           /*!< 0x80000000 */

#define ADC_CHSELR_SQ7_POS             (24U)
#define ADC_CHSELR_SQ7_MSK             (0xFUL << ADC_CHSELR_SQ7_POS)           /*!< 0x0F000000 */
#define ADC_CHSELR_SQ7                 ADC_CHSELR_SQ7_MSK                      /*!< ADC group regular sequencer rank 7, available when ADC_CFGR1_CHSELRMOD is set */
#define ADC_CHSELR_SQ7_0               (0x1UL << ADC_CHSELR_SQ7_POS)           /*!< 0x01000000 */
#define ADC_CHSELR_SQ7_1               (0x2UL << ADC_CHSELR_SQ7_POS)           /*!< 0x02000000 */
#define ADC_CHSELR_SQ7_2               (0x4UL << ADC_CHSELR_SQ7_POS)           /*!< 0x04000000 */
#define ADC_CHSELR_SQ7_3               (0x8UL << ADC_CHSELR_SQ7_POS)           /*!< 0x08000000 */

#define ADC_CHSELR_SQ6_POS             (20U)
#define ADC_CHSELR_SQ6_MSK             (0xFUL << ADC_CHSELR_SQ6_POS)           /*!< 0x00F00000 */
#define ADC_CHSELR_SQ6                 ADC_CHSELR_SQ6_MSK                      /*!< ADC group regular sequencer rank 6, available when ADC_CFGR1_CHSELRMOD is set */
#define ADC_CHSELR_SQ6_0               (0x1UL << ADC_CHSELR_SQ6_POS)           /*!< 0x00100000 */
#define ADC_CHSELR_SQ6_1               (0x2UL << ADC_CHSELR_SQ6_POS)           /*!< 0x00200000 */
#define ADC_CHSELR_SQ6_2               (0x4UL << ADC_CHSELR_SQ6_POS)           /*!< 0x00400000 */
#define ADC_CHSELR_SQ6_3               (0x8UL << ADC_CHSELR_SQ6_POS)           /*!< 0x00800000 */

#define ADC_CHSELR_SQ5_POS             (16U)
#define ADC_CHSELR_SQ5_MSK             (0xFUL << ADC_CHSELR_SQ5_POS)           /*!< 0x000F0000 */
#define ADC_CHSELR_SQ5                 ADC_CHSELR_SQ5_MSK                      /*!< ADC group regular sequencer rank 5, available when ADC_CFGR1_CHSELRMOD is set */
#define ADC_CHSELR_SQ5_0               (0x1UL << ADC_CHSELR_SQ5_POS)           /*!< 0x00010000 */
#define ADC_CHSELR_SQ5_1               (0x2UL << ADC_CHSELR_SQ5_POS)           /*!< 0x00020000 */
#define ADC_CHSELR_SQ5_2               (0x4UL << ADC_CHSELR_SQ5_POS)           /*!< 0x00040000 */
#define ADC_CHSELR_SQ5_3               (0x8UL << ADC_CHSELR_SQ5_POS)           /*!< 0x00080000 */

#define ADC_CHSELR_SQ4_POS             (12U)
#define ADC_CHSELR_SQ4_MSK             (0xFUL << ADC_CHSELR_SQ4_POS)           /*!< 0x0000F000 */
#define ADC_CHSELR_SQ4                 ADC_CHSELR_SQ4_MSK                      /*!< ADC group regular sequencer rank 4, available when ADC_CFGR1_CHSELRMOD is set */
#define ADC_CHSELR_SQ4_0               (0x1UL << ADC_CHSELR_SQ4_POS)           /*!< 0x00001000 */
#define ADC_CHSELR_SQ4_1               (0x2UL << ADC_CHSELR_SQ4_POS)           /*!< 0x00002000 */
#define ADC_CHSELR_SQ4_2               (0x4UL << ADC_CHSELR_SQ4_POS)           /*!< 0x00004000 */
#define ADC_CHSELR_SQ4_3               (0x8UL << ADC_CHSELR_SQ4_POS)           /*!< 0x00008000 */

#define ADC_CHSELR_SQ3_POS             (8U)
#define ADC_CHSELR_SQ3_MSK             (0xFUL << ADC_CHSELR_SQ3_POS)           /*!< 0x00000F00 */
#define ADC_CHSELR_SQ3                 ADC_CHSELR_SQ3_MSK                      /*!< ADC group regular sequencer rank 3, available when ADC_CFGR1_CHSELRMOD is set */
#define ADC_CHSELR_SQ3_0               (0x1UL << ADC_CHSELR_SQ3_POS)           /*!< 0x00000100 */
#define ADC_CHSELR_SQ3_1               (0x2UL << ADC_CHSELR_SQ3_POS)           /*!< 0x00000200 */
#define ADC_CHSELR_SQ3_2               (0x4UL << ADC_CHSELR_SQ3_POS)           /*!< 0x00000400 */
#define ADC_CHSELR_SQ3_3               (0x8UL << ADC_CHSELR_SQ3_POS)           /*!< 0x00000800 */

#define ADC_CHSELR_SQ2_POS             (4U)
#define ADC_CHSELR_SQ2_MSK             (0xFUL << ADC_CHSELR_SQ2_POS)           /*!< 0x000000F0 */
#define ADC_CHSELR_SQ2                 ADC_CHSELR_SQ2_MSK                      /*!< ADC group regular sequencer rank 2, available when ADC_CFGR1_CHSELRMOD is set */
#define ADC_CHSELR_SQ2_0               (0x1UL << ADC_CHSELR_SQ2_POS)           /*!< 0x00000010 */
#define ADC_CHSELR_SQ2_1               (0x2UL << ADC_CHSELR_SQ2_POS)           /*!< 0x00000020 */
#define ADC_CHSELR_SQ2_2               (0x4UL << ADC_CHSELR_SQ2_POS)           /*!< 0x00000040 */
#define ADC_CHSELR_SQ2_3               (0x8UL << ADC_CHSELR_SQ2_POS)           /*!< 0x00000080 */

#define ADC_CHSELR_SQ1_POS             (0U)
#define ADC_CHSELR_SQ1_MSK             (0xFUL << ADC_CHSELR_SQ1_POS)           /*!< 0x0000000F */
#define ADC_CHSELR_SQ1                 ADC_CHSELR_SQ1_MSK                      /*!< ADC group regular sequencer rank 1, available when ADC_CFGR1_CHSELRMOD is set */
#define ADC_CHSELR_SQ1_0               (0x1UL << ADC_CHSELR_SQ1_POS)           /*!< 0x00000001 */
#define ADC_CHSELR_SQ1_1               (0x2UL << ADC_CHSELR_SQ1_POS)           /*!< 0x00000002 */
#define ADC_CHSELR_SQ1_2               (0x4UL << ADC_CHSELR_SQ1_POS)           /*!< 0x00000004 */
#define ADC_CHSELR_SQ1_3               (0x8UL << ADC_CHSELR_SQ1_POS)           /*!< 0x00000008 */

/********************  Bit definition for ADC_TR3 register  *******************/
#define ADC_TR3_LT3_POS                (0U)
#define ADC_TR3_LT3_MSK                (0xFFFUL << ADC_TR3_LT3_POS)            /*!< 0x00000FFF */
#define ADC_TR3_LT3                    ADC_TR3_LT3_MSK                         /*!< ADC analog watchdog 3 threshold low */
#define ADC_TR3_LT3_0                  (0x001UL << ADC_TR3_LT3_POS)            /*!< 0x00000001 */
#define ADC_TR3_LT3_1                  (0x002UL << ADC_TR3_LT3_POS)            /*!< 0x00000002 */
#define ADC_TR3_LT3_2                  (0x004UL << ADC_TR3_LT3_POS)            /*!< 0x00000004 */
#define ADC_TR3_LT3_3                  (0x008UL << ADC_TR3_LT3_POS)            /*!< 0x00000008 */
#define ADC_TR3_LT3_4                  (0x010UL << ADC_TR3_LT3_POS)            /*!< 0x00000010 */
#define ADC_TR3_LT3_5                  (0x020UL << ADC_TR3_LT3_POS)            /*!< 0x00000020 */
#define ADC_TR3_LT3_6                  (0x040UL << ADC_TR3_LT3_POS)            /*!< 0x00000040 */
#define ADC_TR3_LT3_7                  (0x080UL << ADC_TR3_LT3_POS)            /*!< 0x00000080 */
#define ADC_TR3_LT3_8                  (0x100UL << ADC_TR3_LT3_POS)            /*!< 0x00000100 */
#define ADC_TR3_LT3_9                  (0x200UL << ADC_TR3_LT3_POS)            /*!< 0x00000200 */
#define ADC_TR3_LT3_10                 (0x400UL << ADC_TR3_LT3_POS)            /*!< 0x00000400 */
#define ADC_TR3_LT3_11                 (0x800UL << ADC_TR3_LT3_POS)            /*!< 0x00000800 */

#define ADC_TR3_HT3_POS                (16U)
#define ADC_TR3_HT3_MSK                (0xFFFUL << ADC_TR3_HT3_POS)            /*!< 0x0FFF0000 */
#define ADC_TR3_HT3                    ADC_TR3_HT3_MSK                         /*!< ADC analog watchdog 3 threshold high */
#define ADC_TR3_HT3_0                  (0x001UL << ADC_TR3_HT3_POS)            /*!< 0x00010000 */
#define ADC_TR3_HT3_1                  (0x002UL << ADC_TR3_HT3_POS)            /*!< 0x00020000 */
#define ADC_TR3_HT3_2                  (0x004UL << ADC_TR3_HT3_POS)            /*!< 0x00040000 */
#define ADC_TR3_HT3_3                  (0x008UL << ADC_TR3_HT3_POS)            /*!< 0x00080000 */
#define ADC_TR3_HT3_4                  (0x010UL << ADC_TR3_HT3_POS)            /*!< 0x00100000 */
#define ADC_TR3_HT3_5                  (0x020UL << ADC_TR3_HT3_POS)            /*!< 0x00200000 */
#define ADC_TR3_HT3_6                  (0x040UL << ADC_TR3_HT3_POS)            /*!< 0x00400000 */
#define ADC_TR3_HT3_7                  (0x080UL << ADC_TR3_HT3_POS)            /*!< 0x00800000 */
#define ADC_TR3_HT3_8                  (0x100UL << ADC_TR3_HT3_POS)            /*!< 0x01000000 */
#define ADC_TR3_HT3_9                  (0x200UL << ADC_TR3_HT3_POS)            /*!< 0x02000000 */
#define ADC_TR3_HT3_10                 (0x400UL << ADC_TR3_HT3_POS)            /*!< 0x04000000 */
#define ADC_TR3_HT3_11                 (0x800UL << ADC_TR3_HT3_POS)            /*!< 0x08000000 */

/********************  Bit definition for ADC_DR register  ********************/
#define ADC_DR_DATA_POS                (0U)
#define ADC_DR_DATA_MSK                (0xFFFFUL << ADC_DR_DATA_POS)           /*!< 0x0000FFFF */
#define ADC_DR_DATA                    ADC_DR_DATA_MSK                         /*!< ADC group regular conversion data */
#define ADC_DR_DATA_0                  (0x0001UL << ADC_DR_DATA_POS)           /*!< 0x00000001 */
#define ADC_DR_DATA_1                  (0x0002UL << ADC_DR_DATA_POS)           /*!< 0x00000002 */
#define ADC_DR_DATA_2                  (0x0004UL << ADC_DR_DATA_POS)           /*!< 0x00000004 */
#define ADC_DR_DATA_3                  (0x0008UL << ADC_DR_DATA_POS)           /*!< 0x00000008 */
#define ADC_DR_DATA_4                  (0x0010UL << ADC_DR_DATA_POS)           /*!< 0x00000010 */
#define ADC_DR_DATA_5                  (0x0020UL << ADC_DR_DATA_POS)           /*!< 0x00000020 */
#define ADC_DR_DATA_6                  (0x0040UL << ADC_DR_DATA_POS)           /*!< 0x00000040 */
#define ADC_DR_DATA_7                  (0x0080UL << ADC_DR_DATA_POS)           /*!< 0x00000080 */
#define ADC_DR_DATA_8                  (0x0100UL << ADC_DR_DATA_POS)           /*!< 0x00000100 */
#define ADC_DR_DATA_9                  (0x0200UL << ADC_DR_DATA_POS)           /*!< 0x00000200 */
#define ADC_DR_DATA_10                 (0x0400UL << ADC_DR_DATA_POS)           /*!< 0x00000400 */
#define ADC_DR_DATA_11                 (0x0800UL << ADC_DR_DATA_POS)           /*!< 0x00000800 */
#define ADC_DR_DATA_12                 (0x1000UL << ADC_DR_DATA_POS)           /*!< 0x00001000 */
#define ADC_DR_DATA_13                 (0x2000UL << ADC_DR_DATA_POS)           /*!< 0x00002000 */
#define ADC_DR_DATA_14                 (0x4000UL << ADC_DR_DATA_POS)           /*!< 0x00004000 */
#define ADC_DR_DATA_15                 (0x8000UL << ADC_DR_DATA_POS)           /*!< 0x00008000 */

/********************  Bit definition for ADC_AWD2CR register  ****************/
#define ADC_AWD2CR_AWD2CH_POS          (0U)
#define ADC_AWD2CR_AWD2CH_MSK          (0x7FFFFUL << ADC_AWD2CR_AWD2CH_POS)    /*!< 0x0007FFFF */
#define ADC_AWD2CR_AWD2CH              ADC_AWD2CR_AWD2CH_MSK                   /*!< ADC analog watchdog 2 monitored channel selection */
#define ADC_AWD2CR_AWD2CH_0            (0x00001UL << ADC_AWD2CR_AWD2CH_POS)    /*!< 0x00000001 */
#define ADC_AWD2CR_AWD2CH_1            (0x00002UL << ADC_AWD2CR_AWD2CH_POS)    /*!< 0x00000002 */
#define ADC_AWD2CR_AWD2CH_2            (0x00004UL << ADC_AWD2CR_AWD2CH_POS)    /*!< 0x00000004 */
#define ADC_AWD2CR_AWD2CH_3            (0x00008UL << ADC_AWD2CR_AWD2CH_POS)    /*!< 0x00000008 */
#define ADC_AWD2CR_AWD2CH_4            (0x00010UL << ADC_AWD2CR_AWD2CH_POS)    /*!< 0x00000010 */
#define ADC_AWD2CR_AWD2CH_5            (0x00020UL << ADC_AWD2CR_AWD2CH_POS)    /*!< 0x00000020 */
#define ADC_AWD2CR_AWD2CH_6            (0x00040UL << ADC_AWD2CR_AWD2CH_POS)    /*!< 0x00000040 */
#define ADC_AWD2CR_AWD2CH_7            (0x00080UL << ADC_AWD2CR_AWD2CH_POS)    /*!< 0x00000080 */
#define ADC_AWD2CR_AWD2CH_8            (0x00100UL << ADC_AWD2CR_AWD2CH_POS)    /*!< 0x00000100 */
#define ADC_AWD2CR_AWD2CH_9            (0x00200UL << ADC_AWD2CR_AWD2CH_POS)    /*!< 0x00000200 */
#define ADC_AWD2CR_AWD2CH_10           (0x00400UL << ADC_AWD2CR_AWD2CH_POS)    /*!< 0x00000400 */
#define ADC_AWD2CR_AWD2CH_11           (0x00800UL << ADC_AWD2CR_AWD2CH_POS)    /*!< 0x00000800 */
#define ADC_AWD2CR_AWD2CH_12           (0x01000UL << ADC_AWD2CR_AWD2CH_POS)    /*!< 0x00001000 */
#define ADC_AWD2CR_AWD2CH_13           (0x02000UL << ADC_AWD2CR_AWD2CH_POS)    /*!< 0x00002000 */
#define ADC_AWD2CR_AWD2CH_14           (0x04000UL << ADC_AWD2CR_AWD2CH_POS)    /*!< 0x00004000 */
#define ADC_AWD2CR_AWD2CH_15           (0x08000UL << ADC_AWD2CR_AWD2CH_POS)    /*!< 0x00008000 */
#define ADC_AWD2CR_AWD2CH_16           (0x10000UL << ADC_AWD2CR_AWD2CH_POS)    /*!< 0x00010000 */
#define ADC_AWD2CR_AWD2CH_17           (0x20000UL << ADC_AWD2CR_AWD2CH_POS)    /*!< 0x00020000 */
#define ADC_AWD2CR_AWD2CH_18           (0x40000UL << ADC_AWD2CR_AWD2CH_POS)    /*!< 0x00040000 */

/********************  Bit definition for ADC_AWD3CR register  ****************/
#define ADC_AWD3CR_AWD3CH_POS          (0U)
#define ADC_AWD3CR_AWD3CH_MSK          (0x7FFFFUL << ADC_AWD3CR_AWD3CH_POS)    /*!< 0x0007FFFF */
#define ADC_AWD3CR_AWD3CH              ADC_AWD3CR_AWD3CH_MSK                   /*!< ADC analog watchdog 3 monitored channel selection */
#define ADC_AWD3CR_AWD3CH_0            (0x00001UL << ADC_AWD3CR_AWD3CH_POS)    /*!< 0x00000001 */
#define ADC_AWD3CR_AWD3CH_1            (0x00002UL << ADC_AWD3CR_AWD3CH_POS)    /*!< 0x00000002 */
#define ADC_AWD3CR_AWD3CH_2            (0x00004UL << ADC_AWD3CR_AWD3CH_POS)    /*!< 0x00000004 */
#define ADC_AWD3CR_AWD3CH_3            (0x00008UL << ADC_AWD3CR_AWD3CH_POS)    /*!< 0x00000008 */
#define ADC_AWD3CR_AWD3CH_4            (0x00010UL << ADC_AWD3CR_AWD3CH_POS)    /*!< 0x00000010 */
#define ADC_AWD3CR_AWD3CH_5            (0x00020UL << ADC_AWD3CR_AWD3CH_POS)    /*!< 0x00000020 */
#define ADC_AWD3CR_AWD3CH_6            (0x00040UL << ADC_AWD3CR_AWD3CH_POS)    /*!< 0x00000040 */
#define ADC_AWD3CR_AWD3CH_7            (0x00080UL << ADC_AWD3CR_AWD3CH_POS)    /*!< 0x00000080 */
#define ADC_AWD3CR_AWD3CH_8            (0x00100UL << ADC_AWD3CR_AWD3CH_POS)    /*!< 0x00000100 */
#define ADC_AWD3CR_AWD3CH_9            (0x00200UL << ADC_AWD3CR_AWD3CH_POS)    /*!< 0x00000200 */
#define ADC_AWD3CR_AWD3CH_10           (0x00400UL << ADC_AWD3CR_AWD3CH_POS)    /*!< 0x00000400 */
#define ADC_AWD3CR_AWD3CH_11           (0x00800UL << ADC_AWD3CR_AWD3CH_POS)    /*!< 0x00000800 */
#define ADC_AWD3CR_AWD3CH_12           (0x01000UL << ADC_AWD3CR_AWD3CH_POS)    /*!< 0x00001000 */
#define ADC_AWD3CR_AWD3CH_13           (0x02000UL << ADC_AWD3CR_AWD3CH_POS)    /*!< 0x00002000 */
#define ADC_AWD3CR_AWD3CH_14           (0x04000UL << ADC_AWD3CR_AWD3CH_POS)    /*!< 0x00004000 */
#define ADC_AWD3CR_AWD3CH_15           (0x08000UL << ADC_AWD3CR_AWD3CH_POS)    /*!< 0x00008000 */
#define ADC_AWD3CR_AWD3CH_16           (0x10000UL << ADC_AWD3CR_AWD3CH_POS)    /*!< 0x00010000 */
#define ADC_AWD3CR_AWD3CH_17           (0x20000UL << ADC_AWD3CR_AWD3CH_POS)    /*!< 0x00020000 */
#define ADC_AWD3CR_AWD3CH_18           (0x40000UL << ADC_AWD3CR_AWD3CH_POS)    /*!< 0x00040000 */

/********************  Bit definition for ADC_CALFACT register  ***************/
#define ADC_CALFACT_CALFACT_POS        (0U)
#define ADC_CALFACT_CALFACT_MSK        (0x7FUL << ADC_CALFACT_CALFACT_POS)     /*!< 0x0000007F */
#define ADC_CALFACT_CALFACT            ADC_CALFACT_CALFACT_MSK                 /*!< ADC calibration factor in single-ended mode */
#define ADC_CALFACT_CALFACT_0          (0x01UL << ADC_CALFACT_CALFACT_POS)     /*!< 0x00000001 */
#define ADC_CALFACT_CALFACT_1          (0x02UL << ADC_CALFACT_CALFACT_POS)     /*!< 0x00000002 */
#define ADC_CALFACT_CALFACT_2          (0x04UL << ADC_CALFACT_CALFACT_POS)     /*!< 0x00000004 */
#define ADC_CALFACT_CALFACT_3          (0x08UL << ADC_CALFACT_CALFACT_POS)     /*!< 0x00000008 */
#define ADC_CALFACT_CALFACT_4          (0x10UL << ADC_CALFACT_CALFACT_POS)     /*!< 0x00000010 */
#define ADC_CALFACT_CALFACT_5          (0x20UL << ADC_CALFACT_CALFACT_POS)     /*!< 0x00000020 */
#define ADC_CALFACT_CALFACT_6          (0x40UL << ADC_CALFACT_CALFACT_POS)     /*!< 0x00000040 */

/*************************  ADC Common registers  *****************************/
/********************  Bit definition for ADC_CCR register  *******************/
#define ADC_CCR_PRESC_POS              (18U)
#define ADC_CCR_PRESC_MSK              (0xFUL << ADC_CCR_PRESC_POS)            /*!< 0x003C0000 */
#define ADC_CCR_PRESC                  ADC_CCR_PRESC_MSK                       /*!< ADC common clock prescaler, only for clock source asynchronous */
#define ADC_CCR_PRESC_0                (0x1UL << ADC_CCR_PRESC_POS)            /*!< 0x00040000 */
#define ADC_CCR_PRESC_1                (0x2UL << ADC_CCR_PRESC_POS)            /*!< 0x00080000 */
#define ADC_CCR_PRESC_2                (0x4UL << ADC_CCR_PRESC_POS)            /*!< 0x00100000 */
#define ADC_CCR_PRESC_3                (0x8UL << ADC_CCR_PRESC_POS)            /*!< 0x00200000 */

#define ADC_CCR_VREFEN_POS             (22U)
#define ADC_CCR_VREFEN_MSK             (0x1UL << ADC_CCR_VREFEN_POS)           /*!< 0x00400000 */
#define ADC_CCR_VREFEN                 ADC_CCR_VREFEN_MSK                      /*!< ADC internal path to VrefInt enable */
#define ADC_CCR_TSEN_POS               (23U)
#define ADC_CCR_TSEN_MSK               (0x1UL << ADC_CCR_TSEN_POS)             /*!< 0x00800000 */
#define ADC_CCR_TSEN                   ADC_CCR_TSEN_MSK                        /*!< ADC internal path to temperature sensor enable */
#define ADC_CCR_VBATEN_POS             (24U)
#define ADC_CCR_VBATEN_MSK             (0x1UL << ADC_CCR_VBATEN_POS)           /*!< 0x01000000 */
#define ADC_CCR_VBATEN                 ADC_CCR_VBATEN_MSK                      /*!< ADC internal path to battery voltage enable */

/* Legacy */
#define ADC_CCR_LFMEN_POS              (25U)
#define ADC_CCR_LFMEN_MSK              (0x1UL << ADC_CCR_LFMEN_POS)            /*!< 0x02000000 */
#define ADC_CCR_LFMEN                  ADC_CCR_LFMEN_MSK                       /*!< Legacy feature, useless on STM32G0 (ADC common clock low frequency mode is automatically managed by ADC peripheral on STM32G0) */

/** @defgroup ADC_HAL_EC_COMMON_CLOCK_SOURCE  ADC common - Clock source
  * @{
  */
#define LL_ADC_CLOCK_ASYNC_DIV1            (0x00000000UL)                                        /*!< ADC asynchronous clock without prescaler */
#define LL_ADC_CLOCK_ASYNC_DIV2            (ADC_CCR_PRESC_0)                                     /*!< ADC asynchronous clock with prescaler division by 2. ADC common clock asynchronous prescaler is applied to each ADC instance if the corresponding ADC instance clock is set to clock source asynchronous (refer to function @ref LL_ADC_SetClock() ). */
#define LL_ADC_CLOCK_ASYNC_DIV4            (ADC_CCR_PRESC_1                  )                   /*!< ADC asynchronous clock with prescaler division by 4. ADC common clock asynchronous prescaler is applied to each ADC instance if the corresponding ADC instance clock is set to clock source asynchronous (refer to function @ref LL_ADC_SetClock() ). */
#define LL_ADC_CLOCK_ASYNC_DIV6            (ADC_CCR_PRESC_1 | ADC_CCR_PRESC_0)                   /*!< ADC asynchronous clock with prescaler division by 6. ADC common clock asynchronous prescaler is applied to each ADC instance if the corresponding ADC instance clock is set to clock source asynchronous (refer to function @ref LL_ADC_SetClock() ). */
#define LL_ADC_CLOCK_ASYNC_DIV8            (ADC_CCR_PRESC_2                                    ) /*!< ADC asynchronous clock with prescaler division by 8. ADC common clock asynchronous prescaler is applied to each ADC instance if the corresponding ADC instance clock is set to clock source asynchronous (refer to function @ref LL_ADC_SetClock() ). */
#define LL_ADC_CLOCK_ASYNC_DIV10           (ADC_CCR_PRESC_2                   | ADC_CCR_PRESC_0) /*!< ADC asynchronous clock with prescaler division by 10. ADC common clock asynchronous prescaler is applied to each ADC instance if the corresponding ADC instance clock is set to clock source asynchronous (refer to function @ref LL_ADC_SetClock() ). */
#define LL_ADC_CLOCK_ASYNC_DIV12           (ADC_CCR_PRESC_2 | ADC_CCR_PRESC_1                  ) /*!< ADC asynchronous clock with prescaler division by 12. ADC common clock asynchronous prescaler is applied to each ADC instance if the corresponding ADC instance clock is set to clock source asynchronous (refer to function @ref LL_ADC_SetClock() ). */
#define LL_ADC_CLOCK_ASYNC_DIV16           (ADC_CCR_PRESC_2 | ADC_CCR_PRESC_1 | ADC_CCR_PRESC_0) /*!< ADC asynchronous clock with prescaler division by 16. ADC common clock asynchronous prescaler is applied to each ADC instance if the corresponding ADC instance clock is set to clock source asynchronous (refer to function @ref LL_ADC_SetClock() ). */
#define LL_ADC_CLOCK_ASYNC_DIV32           (ADC_CCR_PRESC_3)                                     /*!< ADC asynchronous clock with prescaler division by 32. ADC common clock asynchronous prescaler is applied to each ADC instance if the corresponding ADC instance clock is set to clock source asynchronous (refer to function @ref LL_ADC_SetClock() ). */
#define LL_ADC_CLOCK_ASYNC_DIV64           (ADC_CCR_PRESC_3 | ADC_CCR_PRESC_0)                   /*!< ADC asynchronous clock with prescaler division by 64. ADC common clock asynchronous prescaler is applied to each ADC instance if the corresponding ADC instance clock is set to clock source asynchronous (refer to function @ref LL_ADC_SetClock() ). */
#define LL_ADC_CLOCK_ASYNC_DIV128          (ADC_CCR_PRESC_3 | ADC_CCR_PRESC_1)                   /*!< ADC asynchronous clock with prescaler division by 128. ADC common clock asynchronous prescaler is applied to each ADC instance if the corresponding ADC instance clock is set to clock source asynchronous (refer to function @ref LL_ADC_SetClock() ). */
#define LL_ADC_CLOCK_ASYNC_DIV256          (ADC_CCR_PRESC_3 | ADC_CCR_PRESC_1 | ADC_CCR_PRESC_0) /*!< ADC asynchronous clock with prescaler division by 256. ADC common clock asynchronous prescaler is applied to each ADC instance if the corresponding ADC instance clock is set to clock source asynchronous (refer to function @ref LL_ADC_SetClock() ). */

#define ADC_CLOCK_SYNC_PCLK_DIV1           (LL_ADC_CLOCK_SYNC_PCLK_DIV1)  /*!< ADC synchronous clock derived from AHB clock without prescaler. This configuration must be enabled only if PCLK has a 50% duty clock cycle (APB prescaler configured inside the RCC must be bypassed and the system clock must by 50% duty cycle) */
#define ADC_CLOCK_SYNC_PCLK_DIV2           (LL_ADC_CLOCK_SYNC_PCLK_DIV2)  /*!< ADC synchronous clock derived from AHB clock with prescaler division by 2 */
#define ADC_CLOCK_SYNC_PCLK_DIV4           (LL_ADC_CLOCK_SYNC_PCLK_DIV4)  /*!< ADC synchronous clock derived from AHB clock with prescaler division by 4 */

#define ADC_CLOCK_ASYNC_DIV1               (LL_ADC_CLOCK_ASYNC_DIV1)      /*!< ADC asynchronous clock without prescaler */
#define ADC_CLOCK_ASYNC_DIV2               (LL_ADC_CLOCK_ASYNC_DIV2)      /*!< ADC asynchronous clock with prescaler division by 2   */
#define ADC_CLOCK_ASYNC_DIV4               (LL_ADC_CLOCK_ASYNC_DIV4)      /*!< ADC asynchronous clock with prescaler division by 4   */
#define ADC_CLOCK_ASYNC_DIV6               (LL_ADC_CLOCK_ASYNC_DIV6)      /*!< ADC asynchronous clock with prescaler division by 6   */
#define ADC_CLOCK_ASYNC_DIV8               (LL_ADC_CLOCK_ASYNC_DIV8)      /*!< ADC asynchronous clock with prescaler division by 8   */
#define ADC_CLOCK_ASYNC_DIV10              (LL_ADC_CLOCK_ASYNC_DIV10)     /*!< ADC asynchronous clock with prescaler division by 10  */
#define ADC_CLOCK_ASYNC_DIV12              (LL_ADC_CLOCK_ASYNC_DIV12)     /*!< ADC asynchronous clock with prescaler division by 12  */
#define ADC_CLOCK_ASYNC_DIV16              (LL_ADC_CLOCK_ASYNC_DIV16)     /*!< ADC asynchronous clock with prescaler division by 16  */
#define ADC_CLOCK_ASYNC_DIV32              (LL_ADC_CLOCK_ASYNC_DIV32)     /*!< ADC asynchronous clock with prescaler division by 32  */
#define ADC_CLOCK_ASYNC_DIV64              (LL_ADC_CLOCK_ASYNC_DIV64)     /*!< ADC asynchronous clock with prescaler division by 64  */
#define ADC_CLOCK_ASYNC_DIV128             (LL_ADC_CLOCK_ASYNC_DIV128)    /*!< ADC asynchronous clock with prescaler division by 128 */
#define ADC_CLOCK_ASYNC_DIV256             (LL_ADC_CLOCK_ASYNC_DIV256)    /*!< ADC asynchronous clock with prescaler division by 256 */

/** @defgroup ADC_LL_EC_HW_DELAYS  Definitions of ADC hardware constraints delays
  * @note   Only ADC peripheral HW delays are defined in ADC LL driver driver,
  *         not timeout values.
  *         For details on delays values, refer to descriptions in source code
  *         above each literal definition.
  * @{
  */

/* Note: Only ADC peripheral HW delays are defined in ADC LL driver driver,   */
/*       not timeout values.                                                  */
/*       Timeout values for ADC operations are dependent to device clock      */
/*       configuration (system clock versus ADC clock),                       */
/*       and therefore must be defined in user application.                   */
/*       Indications for estimation of ADC timeout delays, for this           */
/*       STM32 series:                                                        */
/*       - ADC calibration time: maximum delay is 82/fADC.                    */
/*         (refer to device datasheet, parameter "tCAL")                      */
/*       - ADC enable time: maximum delay is 1 conversion cycle.              */
/*         (refer to device datasheet, parameter "tSTAB")                     */
/*       - ADC disable time: maximum delay should be a few ADC clock cycles   */
/*       - ADC stop conversion time: maximum delay should be a few ADC clock  */
/*         cycles                                                             */
/*       - ADC conversion time: duration depending on ADC clock and ADC       */
/*         configuration.                                                     */
/*         (refer to device reference manual, section "Timing")               */

/* Delay for ADC stabilization time (ADC voltage regulator start-up time)     */
/* Delay set to maximum value (refer to device datasheet,                     */
/* parameter "tADCVREG_STUP").                                                */
/* Unit: us                                                                   */
#define LL_ADC_DELAY_INTERNAL_REGUL_STAB_US ( 20UL)  /*!< Delay for ADC stabilization time (ADC voltage regulator start-up time) */

/* Delay for internal voltage reference stabilization time.                   */
/* Delay set to maximum value (refer to device datasheet,                     */
/* parameter "tstart_vrefint").                                               */
/* Unit: us                                                                   */
#define LL_ADC_DELAY_VREFINT_STAB_US           ( 12UL)  /*!< Delay for internal voltage reference stabilization time */

/* Delay for temperature sensor stabilization time.                           */
/* Literal set to maximum value (refer to device datasheet,                   */
/* parameter "tSTART").                                                       */
/* Unit: us                                                                   */
#define LL_ADC_DELAY_TEMPSENSOR_STAB_US        (120UL)  /*!< Delay for temperature sensor stabilization time (starting from temperature sensor enable, refer to @ref LL_ADC_SetCommonPathInternalCh()) */
#define LL_ADC_DELAY_TEMPSENSOR_BUFFER_STAB_US ( 15UL)  /*!< Delay for temperature sensor buffer stabilization time (starting from ADC enable, refer to @ref LL_ADC_Enable()) */

/* Delay required between ADC end of calibration and ADC enable.              */
/* Note: On this STM32 series, a minimum number of ADC clock cycles           */
/*       are required between ADC end of calibration and ADC enable.          */
/*       Wait time can be computed in user application by waiting for the     */
/*       equivalent number of CPU cycles, by taking into account              */
/*       ratio of CPU clock versus ADC clock prescalers.                      */
/* Unit: ADC clock cycles.                                                    */
#define LL_ADC_DELAY_CALIB_ENABLE_ADC_CYCLES   (  2UL)  /*!< Delay required between ADC end of calibration and ADC enable */
//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- FLASH REGISTER ---------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief FLASH Registers
 */
typedef struct
{
  volatile uint32 ACR;          /*!< FLASH Access Control register,                     Address offset: 0x00 */
       uint32 RESERVED1;    /*!< Reserved1,                                         Address offset: 0x04 */
  volatile uint32 KEYR;         /*!< FLASH Key register,                                Address offset: 0x08 */
  volatile uint32 OPTKEYR;      /*!< FLASH Option Key register,                         Address offset: 0x0C */
  volatile uint32 SR;           /*!< FLASH Status register,                             Address offset: 0x10 */
  volatile uint32 CR;           /*!< FLASH Control register,                            Address offset: 0x14 */
  volatile uint32 ECCR;          /*!< FLASH ECC bank 1 register,                        Address offset: 0x18 */
  volatile uint32 ECC2R;         /*!< FLASH ECC bank 2 register,                        Address offset: 0x1C */
  volatile uint32 OPTR;         /*!< FLASH Option register,                             Address offset: 0x20 */
  volatile uint32 PCROP1ASR;    /*!< FLASH Bank PCROP area A Start address register,    Address offset: 0x24 */
  volatile uint32 PCROP1AER;    /*!< FLASH Bank PCROP area A End address register,      Address offset: 0x28 */
  volatile uint32 WRP1AR;       /*!< FLASH Bank WRP area A address register,            Address offset: 0x2C */
  volatile uint32 WRP1BR;       /*!< FLASH Bank WRP area B address register,            Address offset: 0x30 */
  volatile uint32 PCROP1BSR;    /*!< FLASH Bank PCROP area B Start address register,    Address offset: 0x34 */
  volatile uint32 PCROP1BER;    /*!< FLASH Bank PCROP area B End address register,      Address offset: 0x38 */
       uint32 RESERVED5[2]; /*!< Reserved5,                                         Address offset: 0x3C--0x40 */
  volatile uint32 PCROP2ASR;    /*!< FLASH Bank2 PCROP area A Start address register,   Address offset: 0x44 */
  volatile uint32 PCROP2AER;    /*!< FLASH Bank2 PCROP area A End address register,     Address offset: 0x48 */
  volatile uint32 WRP2AR;       /*!< FLASH Bank2 WRP area A address register,           Address offset: 0x4C */
  volatile uint32 WRP2BR;       /*!< FLASH Bank2 WRP area B address register,           Address offset: 0x50 */
  volatile uint32 PCROP2BSR;    /*!< FLASH Bank2 PCROP area B Start address register,   Address offset: 0x54 */
  volatile uint32 PCROP2BER;    /*!< FLASH Bank2 PCROP area B End address register,     Address offset: 0x58 */
       uint32 RESERVED7[9]; /*!< Reserved7,                                         Address offset: 0x5C--0x7C */
  volatile uint32 SECR;         /*!< FLASH security register ,                          Address offset: 0x80 */
} FLASH_TYPEDEF;

#define FLASH               (*(FLASH_TYPEDEF *) FLASH_R_BASE)

#define FLASH_LATENCY_0                 0x00000000UL        /*!< FLASH Zero wait state */
#define FLASH_LATENCY_1                 FLASH_ACR_LATENCY_0 /*!< FLASH One wait state */
#define FLASH_LATENCY_2                 FLASH_ACR_LATENCY_1 /*!< FLASH Two wait states */

#define FLASH_FKEY1                          ((uint32)0x45670123)        //! Flash program erase key1
#define FLASH_FKEY2                          ((uint32)0xCDEF89ABU)        //! Flash program erase key2: used with FLASH_PEKEY1

#define FLASH_SR_ERRORS                 (FLASH_SR_OPERR  | FLASH_SR_PROGERR | FLASH_SR_WRPERR | \
                                        FLASH_SR_PGAERR | FLASH_SR_SIZERR  | FLASH_SR_PGSERR |  \
                                        FLASH_SR_MISERR | FLASH_SR_FASTERR | FLASH_SR_RDERR |   \
                                        FLASH_SR_OPTVERR) /*!< All SR error flags */

#define FLASH_SR_CLEAR                  (FLASH_SR_ERRORS | FLASH_SR_EOP | FLASH_SR_PESD)

#define FLASH_BANK_1                    FLASH_CR_MER1   /*!< Bank 1 */
#define FLASH_BANK_2                    FLASH_CR_MER2   /*!< Bank 2 */


/*******************  Bits definition for FLASH_ACR register  *****************/
#define FLASH_ACR_LATENCY_POS                  (0U)
#define FLASH_ACR_LATENCY_MSK                  (0x7UL << FLASH_ACR_LATENCY_POS) /*!< 0x00000007 */
#define FLASH_ACR_LATENCY                      FLASH_ACR_LATENCY_MSK
#define FLASH_ACR_LATENCY_0                    (0x1UL << FLASH_ACR_LATENCY_POS) /*!< 0x00000001 */
#define FLASH_ACR_LATENCY_1                    (0x2UL << FLASH_ACR_LATENCY_POS) /*!< 0x00000002 */
#define FLASH_ACR_LATENCY_2                    (0x4UL << FLASH_ACR_LATENCY_POS) /*!< 0x00000004 */
#define FLASH_ACR_PRFTEN_POS                   (8U)
#define FLASH_ACR_PRFTEN_MSK                   (0x1UL << FLASH_ACR_PRFTEN_POS)  /*!< 0x00000100 */
#define FLASH_ACR_PRFTEN                       FLASH_ACR_PRFTEN_MSK
#define FLASH_ACR_ICEN_POS                     (9U)
#define FLASH_ACR_ICEN_MSK                     (0x1UL << FLASH_ACR_ICEN_POS)    /*!< 0x00000200 */
#define FLASH_ACR_ICEN                         FLASH_ACR_ICEN_MSK
#define FLASH_ACR_ICRST_POS                    (11U)
#define FLASH_ACR_ICRST_MSK                    (0x1UL << FLASH_ACR_ICRST_POS)   /*!< 0x00000800 */
#define FLASH_ACR_ICRST                        FLASH_ACR_ICRST_MSK
#define FLASH_ACR_PROGEMPTY_POS                (16U)
#define FLASH_ACR_PROGEMPTY_MSK                (0x1UL << FLASH_ACR_PROGEMPTY_POS) /*!< 0x00010000 */
#define FLASH_ACR_PROGEMPTY                    FLASH_ACR_PROGEMPTY_MSK
#define FLASH_ACR_DBG_SWEN_POS                 (18U)
#define FLASH_ACR_DBG_SWEN_MSK                 (0x1UL << FLASH_ACR_DBG_SWEN_POS) /*!< 0x00040000 */
#define FLASH_ACR_DBG_SWEN                     FLASH_ACR_DBG_SWEN_MSK

/*******************  Bits definition for FLASH_SR register  ******************/
#define FLASH_SR_EOP_POS                       (0U)
#define FLASH_SR_EOP_MSK                       (0x1UL << FLASH_SR_EOP_POS)      /*!< 0x00000001 */
#define FLASH_SR_EOP                           FLASH_SR_EOP_MSK
#define FLASH_SR_OPERR_POS                     (1U)
#define FLASH_SR_OPERR_MSK                     (0x1UL << FLASH_SR_OPERR_POS)    /*!< 0x00000002 */
#define FLASH_SR_OPERR                         FLASH_SR_OPERR_MSK
#define FLASH_SR_PROGERR_POS                   (3U)
#define FLASH_SR_PROGERR_MSK                   (0x1UL << FLASH_SR_PROGERR_POS)  /*!< 0x00000008 */
#define FLASH_SR_PROGERR                       FLASH_SR_PROGERR_MSK
#define FLASH_SR_WRPERR_POS                    (4U)
#define FLASH_SR_WRPERR_MSK                    (0x1UL << FLASH_SR_WRPERR_POS)   /*!< 0x00000010 */
#define FLASH_SR_WRPERR                        FLASH_SR_WRPERR_MSK
#define FLASH_SR_PGAERR_POS                    (5U)
#define FLASH_SR_PGAERR_MSK                    (0x1UL << FLASH_SR_PGAERR_POS)   /*!< 0x00000020 */
#define FLASH_SR_PGAERR                        FLASH_SR_PGAERR_MSK
#define FLASH_SR_SIZERR_POS                    (6U)
#define FLASH_SR_SIZERR_MSK                    (0x1UL << FLASH_SR_SIZERR_POS)   /*!< 0x00000040 */
#define FLASH_SR_SIZERR                        FLASH_SR_SIZERR_MSK
#define FLASH_SR_PGSERR_POS                    (7U)
#define FLASH_SR_PGSERR_MSK                    (0x1UL << FLASH_SR_PGSERR_POS)   /*!< 0x00000080 */
#define FLASH_SR_PGSERR                        FLASH_SR_PGSERR_MSK
#define FLASH_SR_MISERR_POS                    (8U)
#define FLASH_SR_MISERR_MSK                    (0x1UL << FLASH_SR_MISERR_POS)   /*!< 0x00000100 */
#define FLASH_SR_MISERR                        FLASH_SR_MISERR_MSK
#define FLASH_SR_FASTERR_POS                   (9U)
#define FLASH_SR_FASTERR_MSK                   (0x1UL << FLASH_SR_FASTERR_POS)  /*!< 0x00000200 */
#define FLASH_SR_FASTERR                       FLASH_SR_FASTERR_MSK
#define FLASH_SR_RDERR_POS                     (14U)
#define FLASH_SR_RDERR_MSK                     (0x1UL << FLASH_SR_RDERR_POS)    /*!< 0x00004000 */
#define FLASH_SR_RDERR                         FLASH_SR_RDERR_MSK
#define FLASH_SR_OPTVERR_POS                   (15U)
#define FLASH_SR_OPTVERR_MSK                   (0x1UL << FLASH_SR_OPTVERR_POS)  /*!< 0x00008000 */
#define FLASH_SR_OPTVERR                       FLASH_SR_OPTVERR_MSK
#define FLASH_SR_BSY1_POS                      (16U)
#define FLASH_SR_BSY1_MSK                      (0x1UL << FLASH_SR_BSY1_POS)     /*!< 0x00010000 */
#define FLASH_SR_BSY1                          FLASH_SR_BSY1_MSK
#define FLASH_SR_BSY2_POS                      (17U)
#define FLASH_SR_BSY2_MSK                      (0x1UL << FLASH_SR_BSY2_POS)     /*!< 0x00020000 */
#define FLASH_SR_BSY2                          FLASH_SR_BSY2_MSK
#define FLASH_SR_CFGBSY_POS                    (18U)
#define FLASH_SR_CFGBSY_MSK                    (0x1UL << FLASH_SR_CFGBSY_POS)   /*!< 0x00040000 */
#define FLASH_SR_CFGBSY                        FLASH_SR_CFGBSY_MSK
#define FLASH_SR_PESD_POS                      (19U)
#define FLASH_SR_PESD_MSK                      (0x1UL << FLASH_SR_PESD_POS)   /*!< 0x00080000 */
#define FLASH_SR_PESD                          FLASH_SR_PESD_MSK

/*******************  Bits definition for FLASH_CR register  ******************/
#define FLASH_CR_PG_POS                        (0U)
#define FLASH_CR_PG_MSK                        (0x1UL << FLASH_CR_PG_POS)         /*!< 0x00000001 */
#define FLASH_CR_PG                            FLASH_CR_PG_MSK
#define FLASH_CR_PER_POS                       (1U)
#define FLASH_CR_PER_MSK                       (0x1UL << FLASH_CR_PER_POS)        /*!< 0x00000002 */
#define FLASH_CR_PER                           FLASH_CR_PER_MSK
#define FLASH_CR_MER1_POS                      (2U)
#define FLASH_CR_MER1_MSK                      (0x1UL << FLASH_CR_MER1_POS)       /*!< 0x00000004 */
#define FLASH_CR_MER1                          FLASH_CR_MER1_MSK
#define FLASH_CR_PNB_POS                       (3U)
#define FLASH_CR_PNB_MSK                       (0x7FUL << FLASH_CR_PNB_POS)       /*!< 0x000003F8 */
#define FLASH_CR_PNB                           FLASH_CR_PNB_MSK
#define FLASH_CR_BKER_POS                      (13U)
#define FLASH_CR_BKER_MSK                      (0x1UL << FLASH_CR_BKER_POS)       /*!< 0x00002000 */
#define FLASH_CR_BKER                          FLASH_CR_BKER_MSK
#define FLASH_CR_MER2_POS                      (15U)
#define FLASH_CR_MER2_MSK                      (0x1UL << FLASH_CR_MER2_POS)       /*!< 0x00008000 */
#define FLASH_CR_MER2                          FLASH_CR_MER2_MSK
#define FLASH_CR_STRT_POS                      (16U)
#define FLASH_CR_STRT_MSK                      (0x1UL << FLASH_CR_STRT_POS)       /*!< 0x00010000 */
#define FLASH_CR_STRT                          FLASH_CR_STRT_MSK
#define FLASH_CR_OPTSTRT_POS                   (17U)
#define FLASH_CR_OPTSTRT_MSK                   (0x1UL << FLASH_CR_OPTSTRT_POS)    /*!< 0x00020000 */
#define FLASH_CR_OPTSTRT                       FLASH_CR_OPTSTRT_MSK
#define FLASH_CR_FSTPG_POS                     (18U)
#define FLASH_CR_FSTPG_MSK                     (0x1UL << FLASH_CR_FSTPG_POS)      /*!< 0x00040000 */
#define FLASH_CR_FSTPG                         FLASH_CR_FSTPG_MSK
#define FLASH_CR_EOPIE_POS                     (24U)
#define FLASH_CR_EOPIE_MSK                     (0x1UL << FLASH_CR_EOPIE_POS)      /*!< 0x01000000 */
#define FLASH_CR_EOPIE                         FLASH_CR_EOPIE_MSK
#define FLASH_CR_ERRIE_POS                     (25U)
#define FLASH_CR_ERRIE_MSK                     (0x1UL << FLASH_CR_ERRIE_POS)      /*!< 0x02000000 */
#define FLASH_CR_ERRIE                         FLASH_CR_ERRIE_MSK
#define FLASH_CR_RDERRIE_POS                   (26U)
#define FLASH_CR_RDERRIE_MSK                   (0x1UL << FLASH_CR_RDERRIE_POS)    /*!< 0x04000000 */
#define FLASH_CR_RDERRIE                       FLASH_CR_RDERRIE_MSK
#define FLASH_CR_OBL_LAUNCH_POS                (27U)
#define FLASH_CR_OBL_LAUNCH_MSK                (0x1UL << FLASH_CR_OBL_LAUNCH_POS) /*!< 0x08000000 */
#define FLASH_CR_OBL_LAUNCH                    FLASH_CR_OBL_LAUNCH_MSK
#define FLASH_CR_SEC_PROT_POS                  (28U)
#define FLASH_CR_SEC_PROT_MSK                  (0x1UL << FLASH_CR_SEC_PROT_POS)   /*!< 0x10000000 */
#define FLASH_CR_SEC_PROT                      FLASH_CR_SEC_PROT_MSK
#define FLASH_CR_SEC_PROT2_POS                 (29U)
#define FLASH_CR_SEC_PROT2_MSK                 (0x1UL << FLASH_CR_SEC_PROT2_POS)  /*!< 0x20000000 */
#define FLASH_CR_SEC_PROT2                     FLASH_CR_SEC_PROT2_MSK
#define FLASH_CR_OPTLOCK_POS                   (30U)
#define FLASH_CR_OPTLOCK_MSK                   (0x1UL << FLASH_CR_OPTLOCK_POS)    /*!< 0x40000000 */
#define FLASH_CR_OPTLOCK                       FLASH_CR_OPTLOCK_MSK
#define FLASH_CR_LOCK_POS                      (31U)
#define FLASH_CR_LOCK_MSK                      (0x1UL << FLASH_CR_LOCK_POS)       /*!< 0x80000000 */
#define FLASH_CR_LOCK                          FLASH_CR_LOCK_MSK

/*******************  Bits definition for FLASH_ECCR register  ****************/
#define FLASH_ECCR_ADDR_ECC_POS                (0U)
#define FLASH_ECCR_ADDR_ECC_MSK                (0x7FFFUL << FLASH_ECCR_ADDR_ECC_POS)  /*!< 0x00007FFF */
#define FLASH_ECCR_ADDR_ECC                    FLASH_ECCR_ADDR_ECC_MSK
#define FLASH_ECCR_SYSF_ECC_POS                (20U)
#define FLASH_ECCR_SYSF_ECC_MSK                (0x1UL << FLASH_ECCR_SYSF_ECC_POS)     /*!< 0x00100000 */
#define FLASH_ECCR_SYSF_ECC                    FLASH_ECCR_SYSF_ECC_MSK
#define FLASH_ECCR_ECCCIE_POS                  (24U)
#define FLASH_ECCR_ECCCIE_MSK                  (0x1UL << FLASH_ECCR_ECCCIE_POS)       /*!< 0x01000000 */
#define FLASH_ECCR_ECCCIE                      FLASH_ECCR_ECCCIE_MSK
#define FLASH_ECCR_ECCC_POS                    (30U)
#define FLASH_ECCR_ECCC_MSK                    (0x1UL << FLASH_ECCR_ECCC_POS)         /*!< 0x40000000 */
#define FLASH_ECCR_ECCC                        FLASH_ECCR_ECCC_MSK
#define FLASH_ECCR_ECCD_POS                    (31U)
#define FLASH_ECCR_ECCD_MSK                    (0x1UL << FLASH_ECCR_ECCD_POS)         /*!< 0x80000000 */
#define FLASH_ECCR_ECCD                        FLASH_ECCR_ECCD_MSK

/*******************  Bits definition for FLASH_ECC2R register  ****************/
#define FLASH_ECC2R_ADDR_ECC_POS                (0U)
#define FLASH_ECC2R_ADDR_ECC_MSK                (0x7FFFUL << FLASH_ECC2R_ADDR_ECC_POS)  /*!< 0x00007FFF */
#define FLASH_ECC2R_ADDR_ECC                    FLASH_ECC2R_ADDR_ECC_MSK
#define FLASH_ECC2R_SYSF_ECC_POS                (20U)
#define FLASH_ECC2R_SYSF_ECC_MSK                (0x1UL << FLASH_ECC2R_SYSF_ECC_POS)     /*!< 0x00100000 */
#define FLASH_ECC2R_SYSF_ECC                    FLASH_ECC2R_SYSF_ECC_MSK
#define FLASH_ECC2R_ECCCIE_POS                  (24U)
#define FLASH_ECC2R_ECCCIE_MSK                  (0x1UL << FLASH_ECC2R_ECCCIE_POS)       /*!< 0x01000000 */
#define FLASH_ECC2R_ECCCIE                      FLASH_ECC2R_ECCCIE_MSK
#define FLASH_ECC2R_ECCC_POS                    (30U)
#define FLASH_ECC2R_ECCC_MSK                    (0x1UL << FLASH_ECC2R_ECCC_POS)         /*!< 0x40000000 */
#define FLASH_ECC2R_ECCC                        FLASH_ECC2R_ECCC_MSK
#define FLASH_ECC2R_ECCD_POS                    (31U)
#define FLASH_ECC2R_ECCD_MSK                    (0x1UL << FLASH_ECC2R_ECCD_POS)         /*!< 0x80000000 */
#define FLASH_ECC2R_ECCD                        FLASH_ECC2R_ECCD_MSK

/*******************  Bits definition for FLASH_OPTR register  ****************/
#define FLASH_OPTR_RDP_POS                     (0U)
#define FLASH_OPTR_RDP_MSK                     (0xFFUL << FLASH_OPTR_RDP_POS)             /*!< 0x000000FF */
#define FLASH_OPTR_RDP                         FLASH_OPTR_RDP_MSK
#define FLASH_OPTR_BOR_EN_POS                  (8U)
#define FLASH_OPTR_BOR_EN_MSK                  (0x1UL << FLASH_OPTR_BOR_EN_POS)           /*!< 0x00000100 */
#define FLASH_OPTR_BOR_EN                      FLASH_OPTR_BOR_EN_MSK
#define FLASH_OPTR_BORR_LEV_POS                (9U)
#define FLASH_OPTR_BORR_LEV_MSK                (0x3UL << FLASH_OPTR_BORR_LEV_POS)         /*!< 0x00000600 */
#define FLASH_OPTR_BORR_LEV                    FLASH_OPTR_BORR_LEV_MSK
#define FLASH_OPTR_BORR_LEV_0                  (0x1UL << FLASH_OPTR_BORR_LEV_POS)         /*!< 0x00000200 */
#define FLASH_OPTR_BORR_LEV_1                  (0x2UL << FLASH_OPTR_BORR_LEV_POS)         /*!< 0x00000400 */
#define FLASH_OPTR_BORF_LEV_POS                (11U)
#define FLASH_OPTR_BORF_LEV_MSK                (0x3UL << FLASH_OPTR_BORF_LEV_POS)         /*!< 0x00001800 */
#define FLASH_OPTR_BORF_LEV                    FLASH_OPTR_BORF_LEV_MSK
#define FLASH_OPTR_BORF_LEV_0                  (0x1UL << FLASH_OPTR_BORF_LEV_POS)         /*!< 0x00000800 */
#define FLASH_OPTR_BORF_LEV_1                  (0x2UL << FLASH_OPTR_BORF_LEV_POS)         /*!< 0x00001000 */
#define FLASH_OPTR_nRST_STOP_POS               (13U)
#define FLASH_OPTR_nRST_STOP_MSK               (0x1UL << FLASH_OPTR_nRST_STOP_POS)        /*!< 0x00002000 */
#define FLASH_OPTR_nRST_STOP                   FLASH_OPTR_nRST_STOP_MSK
#define FLASH_OPTR_nRST_STDBY_POS              (14U)
#define FLASH_OPTR_nRST_STDBY_MSK              (0x1UL << FLASH_OPTR_nRST_STDBY_POS)       /*!< 0x00004000 */
#define FLASH_OPTR_nRST_STDBY                  FLASH_OPTR_nRST_STDBY_MSK
#define FLASH_OPTR_nRST_SHDW_POS               (15U)
#define FLASH_OPTR_nRST_SHDW_MSK               (0x1UL << FLASH_OPTR_nRST_SHDW_POS)        /*!< 0x00008000 */
#define FLASH_OPTR_nRST_SHDW                   FLASH_OPTR_nRST_SHDW_MSK
#define FLASH_OPTR_IWDG_SW_POS                 (16U)
#define FLASH_OPTR_IWDG_SW_MSK                 (0x1UL << FLASH_OPTR_IWDG_SW_POS)          /*!< 0x00010000 */
#define FLASH_OPTR_IWDG_SW                     FLASH_OPTR_IWDG_SW_MSK
#define FLASH_OPTR_IWDG_STOP_POS               (17U)
#define FLASH_OPTR_IWDG_STOP_MSK               (0x1UL << FLASH_OPTR_IWDG_STOP_POS)        /*!< 0x00020000 */
#define FLASH_OPTR_IWDG_STOP                   FLASH_OPTR_IWDG_STOP_MSK
#define FLASH_OPTR_IWDG_STDBY_POS              (18U)
#define FLASH_OPTR_IWDG_STDBY_MSK              (0x1UL << FLASH_OPTR_IWDG_STDBY_POS)       /*!< 0x00040000 */
#define FLASH_OPTR_IWDG_STDBY                  FLASH_OPTR_IWDG_STDBY_MSK
#define FLASH_OPTR_WWDG_SW_POS                 (19U)
#define FLASH_OPTR_WWDG_SW_MSK                 (0x1UL << FLASH_OPTR_WWDG_SW_POS)          /*!< 0x00080000 */
#define FLASH_OPTR_WWDG_SW                     FLASH_OPTR_WWDG_SW_MSK
#define FLASH_OPTR_nSWAP_BANK_POS              (20U)
#define FLASH_OPTR_nSWAP_BANK_MSK              (0x1UL << FLASH_OPTR_nSWAP_BANK_POS)        /*!< 0x00100000 */
#define FLASH_OPTR_nSWAP_BANK                  FLASH_OPTR_nSWAP_BANK_MSK
#define FLASH_OPTR_DUAL_BANK_POS               (21U)
#define FLASH_OPTR_DUAL_BANK_MSK               (0x1UL << FLASH_OPTR_DUAL_BANK_POS)        /*!< 0x00200000 */
#define FLASH_OPTR_DUAL_BANK                   FLASH_OPTR_DUAL_BANK_MSK
#define FLASH_OPTR_RAM_PARITY_CHECK_POS        (22U)
#define FLASH_OPTR_RAM_PARITY_CHECK_MSK        (0x1UL << FLASH_OPTR_RAM_PARITY_CHECK_POS) /*!< 0x00400000 */
#define FLASH_OPTR_RAM_PARITY_CHECK            FLASH_OPTR_RAM_PARITY_CHECK_MSK
#define FLASH_OPTR_nBOOT_SEL_POS               (24U)
#define FLASH_OPTR_nBOOT_SEL_MSK               (0x1UL << FLASH_OPTR_nBOOT_SEL_POS)        /*!< 0x01000000 */
#define FLASH_OPTR_nBOOT_SEL                   FLASH_OPTR_nBOOT_SEL_MSK
#define FLASH_OPTR_nBOOT1_POS                  (25U)
#define FLASH_OPTR_nBOOT1_MSK                  (0x1UL << FLASH_OPTR_nBOOT1_POS)           /*!< 0x02000000 */
#define FLASH_OPTR_nBOOT1                      FLASH_OPTR_nBOOT1_MSK
#define FLASH_OPTR_nBOOT0_POS                  (26U)
#define FLASH_OPTR_nBOOT0_MSK                  (0x1UL << FLASH_OPTR_nBOOT0_POS)           /*!< 0x04000000 */
#define FLASH_OPTR_nBOOT0                      FLASH_OPTR_nBOOT0_MSK
#define FLASH_OPTR_NRST_MODE_POS               (27U)
#define FLASH_OPTR_NRST_MODE_MSK               (0x3UL << FLASH_OPTR_NRST_MODE_POS)        /*!< 0x18000000 */
#define FLASH_OPTR_NRST_MODE                   FLASH_OPTR_NRST_MODE_MSK
#define FLASH_OPTR_NRST_MODE_0                 (0x1UL << FLASH_OPTR_NRST_MODE_POS)        /*!< 0x08000000 */
#define FLASH_OPTR_NRST_MODE_1                 (0x2UL << FLASH_OPTR_NRST_MODE_POS)        /*!< 0x10000000 */
#define FLASH_OPTR_IRHEN_POS                   (29U)
#define FLASH_OPTR_IRHEN_MSK                   (0x1UL << FLASH_OPTR_IRHEN_POS)            /*!< 0x20000000 */
#define FLASH_OPTR_IRHEN                       FLASH_OPTR_IRHEN_MSK

/******************  Bits definition for FLASH_PCROP1ASR register  ************/
#define FLASH_PCROP1ASR_PCROP1A_STRT_POS       (0U)
#define FLASH_PCROP1ASR_PCROP1A_STRT_MSK       (0x1FFUL << FLASH_PCROP1ASR_PCROP1A_STRT_POS)  /*!< 0x0000001FF */
#define FLASH_PCROP1ASR_PCROP1A_STRT           FLASH_PCROP1ASR_PCROP1A_STRT_MSK

/******************  Bits definition for FLASH_PCROP1AER register  ************/
#define FLASH_PCROP1AER_PCROP1A_END_POS        (0U)
#define FLASH_PCROP1AER_PCROP1A_END_MSK        (0x1FFUL << FLASH_PCROP1AER_PCROP1A_END_POS)   /*!< 0x0000001FF */
#define FLASH_PCROP1AER_PCROP1A_END            FLASH_PCROP1AER_PCROP1A_END_MSK
#define FLASH_PCROP1AER_PCROP_RDP_POS          (31U)
#define FLASH_PCROP1AER_PCROP_RDP_MSK          (0x1UL << FLASH_PCROP1AER_PCROP_RDP_POS)       /*!< 0x80000000 */
#define FLASH_PCROP1AER_PCROP_RDP              FLASH_PCROP1AER_PCROP_RDP_MSK

/******************  Bits definition for FLASH_WRP1AR register  ***************/
#define FLASH_WRP1AR_WRP1A_STRT_POS            (0U)
#define FLASH_WRP1AR_WRP1A_STRT_MSK            (0x7FUL << FLASH_WRP1AR_WRP1A_STRT_POS) /*!< 0x0000007F */
#define FLASH_WRP1AR_WRP1A_STRT                FLASH_WRP1AR_WRP1A_STRT_MSK
#define FLASH_WRP1AR_WRP1A_END_POS             (16U)
#define FLASH_WRP1AR_WRP1A_END_MSK             (0x7FUL << FLASH_WRP1AR_WRP1A_END_POS) /*!< 0x007F0000 */
#define FLASH_WRP1AR_WRP1A_END                 FLASH_WRP1AR_WRP1A_END_MSK

/******************  Bits definition for FLASH_WRP1BR register  ***************/
#define FLASH_WRP1BR_WRP1B_STRT_POS            (0U)
#define FLASH_WRP1BR_WRP1B_STRT_MSK            (0x7FUL << FLASH_WRP1BR_WRP1B_STRT_POS) /*!< 0x0000007F */
#define FLASH_WRP1BR_WRP1B_STRT                FLASH_WRP1BR_WRP1B_STRT_MSK
#define FLASH_WRP1BR_WRP1B_END_POS             (16U)
#define FLASH_WRP1BR_WRP1B_END_MSK             (0x7FUL << FLASH_WRP1BR_WRP1B_END_POS) /*!< 0x007F0000 */
#define FLASH_WRP1BR_WRP1B_END                 FLASH_WRP1BR_WRP1B_END_MSK

/******************  Bits definition for FLASH_PCROP1BSR register  ************/
#define FLASH_PCROP1BSR_PCROP1B_STRT_POS       (0U)
#define FLASH_PCROP1BSR_PCROP1B_STRT_MSK       (0x1FFUL << FLASH_PCROP1BSR_PCROP1B_STRT_POS)  /*!< 0x0000001FF */
#define FLASH_PCROP1BSR_PCROP1B_STRT           FLASH_PCROP1BSR_PCROP1B_STRT_MSK

/******************  Bits definition for FLASH_PCROP1BER register  ************/
#define FLASH_PCROP1BER_PCROP1B_END_POS        (0U)
#define FLASH_PCROP1BER_PCROP1B_END_MSK        (0x1FFUL << FLASH_PCROP1BER_PCROP1B_END_POS)   /*!< 0x0000001FF */
#define FLASH_PCROP1BER_PCROP1B_END            FLASH_PCROP1BER_PCROP1B_END_MSK

/******************  Bits definition for FLASH_PCROP2ASR register  ************/
#define FLASH_PCROP2ASR_PCROP2A_STRT_POS       (0U)
#define FLASH_PCROP2ASR_PCROP2A_STRT_MSK       (0x1FFUL << FLASH_PCROP2ASR_PCROP2A_STRT_POS) /*!< 0x0000001FF */
#define FLASH_PCROP2ASR_PCROP2A_STRT           FLASH_PCROP2ASR_PCROP2A_STRT_MSK

/******************  Bits definition for FLASH_PCROP2AER register  ************/
#define FLASH_PCROP2AER_PCROP2A_END_POS        (0U)
#define FLASH_PCROP2AER_PCROP2A_END_MSK        (0x1FFUL << FLASH_PCROP2AER_PCROP2A_END_POS)  /*!< 0x0000001FF */
#define FLASH_PCROP2AER_PCROP2A_END            FLASH_PCROP2AER_PCROP2A_END_MSK


/******************  Bits definition for FLASH_WRP2AR register  ***************/
#define FLASH_WRP2AR_WRP2A_STRT_POS            (0U)
#define FLASH_WRP2AR_WRP2A_STRT_MSK            (0x7FUL << FLASH_WRP2AR_WRP2A_STRT_POS)  /*!< 0x0000007F */
#define FLASH_WRP2AR_WRP2A_STRT                FLASH_WRP2AR_WRP2A_STRT_MSK
#define FLASH_WRP2AR_WRP2A_END_POS             (16U)
#define FLASH_WRP2AR_WRP2A_END_MSK             (0x7FUL << FLASH_WRP2AR_WRP2A_END_POS)   /*!< 0x007F0000 */
#define FLASH_WRP2AR_WRP2A_END                 FLASH_WRP2AR_WRP2A_END_MSK

/******************  Bits definition for FLASH_WRP2BSR register  ***************/
#define FLASH_WRP2BR_WRP2B_STRT_POS            (0U)
#define FLASH_WRP2BR_WRP2B_STRT_MSK            (0x7FUL << FLASH_WRP2BR_WRP2B_STRT_POS)  /*!< 0x0000007F */
#define FLASH_WRP2BR_WRP2B_STRT                FLASH_WRP2BR_WRP2B_STRT_MSK
#define FLASH_WRP2BR_WRP2B_END_POS             (16U)
#define FLASH_WRP2BR_WRP2B_END_MSK             (0x7FUL << FLASH_WRP2BR_WRP2B_END_POS)   /*!< 0x007F0000 */
#define FLASH_WRP2BR_WRP2B_END                 FLASH_WRP2BR_WRP2B_END_MSK

/******************  Bits definition for FLASH_PCROP2BSR register  ************/
#define FLASH_PCROP2BSR_PCROP2B_STRT_POS       (0U)
#define FLASH_PCROP2BSR_PCROP2B_STRT_MSK       (0x1FFUL << FLASH_PCROP2BSR_PCROP2B_STRT_POS) /*!< 0x0000001FF */
#define FLASH_PCROP2BSR_PCROP2B_STRT           FLASH_PCROP2BSR_PCROP2B_STRT_MSK

/******************  Bits definition for FLASH_PCROP2BER register  ************/
#define FLASH_PCROP2BER_PCROP2B_END_POS        (0U)
#define FLASH_PCROP2BER_PCROP2B_END_MSK        (0x1FFUL << FLASH_PCROP2BER_PCROP2B_END_POS)  /*!< 0x0000001FF */
#define FLASH_PCROP2BER_PCROP2B_END            FLASH_PCROP2BER_PCROP2B_END_MSK

/******************  Bits definition for FLASH_SECR register  *****************/
#define FLASH_SECR_SEC_SIZE_POS                (0U)
#define FLASH_SECR_SEC_SIZE_MSK                (0xFFUL << FLASH_SECR_SEC_SIZE_POS) /*!< 0x000000FF */
#define FLASH_SECR_SEC_SIZE                    FLASH_SECR_SEC_SIZE_MSK
#define FLASH_SECR_BOOT_LOCK_POS               (16U)
#define FLASH_SECR_BOOT_LOCK_MSK               (0x1UL << FLASH_SECR_BOOT_LOCK_POS) /*!< 0x00010000 */
#define FLASH_SECR_BOOT_LOCK                   FLASH_SECR_BOOT_LOCK_MSK
#define FLASH_SECR_SEC_SIZE2_POS               (20U)
#define FLASH_SECR_SEC_SIZE2_MSK               (0xFFUL << FLASH_SECR_SEC_SIZE2_POS) /*!< 0x0FF00000 */
#define FLASH_SECR_SEC_SIZE2                   FLASH_SECR_SEC_SIZE2_MSK




//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- POWER CONTROL REGISTERS ------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief Power Control
  */
typedef struct
{
  volatile uint32 CR1;          /*!< PWR Power Control Register 1,                     Address offset: 0x00 */
  volatile uint32 CR2;          /*!< PWR Power Control Register 2,                     Address offset: 0x04 */
  volatile uint32 CR3;          /*!< PWR Power Control Register 3,                     Address offset: 0x08 */
  volatile uint32 CR4;          /*!< PWR Power Control Register 4,                     Address offset: 0x0C */
  volatile uint32 SR1;          /*!< PWR Power Status Register 1,                      Address offset: 0x10 */
  volatile uint32 SR2;          /*!< PWR Power Status Register 2,                      Address offset: 0x14 */
  volatile uint32 SCR;          /*!< PWR Power Status Clear Register,                  Address offset: 0x18 */
       uint32 RESERVED1;    /*!< Reserved,                                         Address offset: 0x1C */
  volatile uint32 PUCRA;        /*!< PWR Pull-Up Control Register of port A,           Address offset: 0x20 */
  volatile uint32 PDCRA;        /*!< PWR Pull-Down Control Register of port A,         Address offset: 0x24 */
  volatile uint32 PUCRB;        /*!< PWR Pull-Up Control Register of port B,           Address offset: 0x28 */
  volatile uint32 PDCRB;        /*!< PWR Pull-Down Control Register of port B,         Address offset: 0x2C */
  volatile uint32 PUCRC;        /*!< PWR Pull-Up Control Register of port C,           Address offset: 0x30 */
  volatile uint32 PDCRC;        /*!< PWR Pull-Down Control Register of port C,         Address offset: 0x34 */
  volatile uint32 PUCRD;        /*!< PWR Pull-Up Control Register of port D,           Address offset: 0x38 */
  volatile uint32 PDCRD;        /*!< PWR Pull-Down Control Register of port D,         Address offset: 0x3C */
  volatile uint32 PUCRE;        /*!< PWR Pull-Up Control Register of port E,           Address offset: 0x40 */
  volatile uint32 PDCRE;        /*!< PWR Pull-Down Control Register of port E,         Address offset: 0x44 */
  volatile uint32 PUCRF;        /*!< PWR Pull-Up Control Register of port F,           Address offset: 0x48 */
  volatile uint32 PDCRF;        /*!< PWR Pull-Down Control Register of port F,         Address offset: 0x4C */
} PWR_TYPEDEF;


#define PWR                 (*(PWR_TYPEDEF *) PWR_BASE)

/********************  Bit definition for PWR_CR1 register  ********************/
#define PWR_CR1_LPMS_POS          (0U)
#define PWR_CR1_LPMS_MSK          (0x7UL << PWR_CR1_LPMS_POS)                  /*!< 0x00000007 */
#define PWR_CR1_LPMS              PWR_CR1_LPMS_MSK                             /*!< Low Power Mode Selection */
#define PWR_CR1_LPMS_0            (0x1UL << PWR_CR1_LPMS_POS)                  /*!< 0x00000001 */
#define PWR_CR1_LPMS_1            (0x2UL << PWR_CR1_LPMS_POS)                  /*!< 0x00000002 */
#define PWR_CR1_LPMS_2            (0x4UL << PWR_CR1_LPMS_POS)                   /*!< 0x00000004 */
#define PWR_CR1_FPD_STOP_POS      (3U)
#define PWR_CR1_FPD_STOP_MSK      (0x1UL << PWR_CR1_FPD_STOP_POS)              /*!< 0x00000008 */
#define PWR_CR1_FPD_STOP          PWR_CR1_FPD_STOP_MSK                         /*!< Flash power down mode during stop */
#define PWR_CR1_FPD_LPRUN_POS     (4U)
#define PWR_CR1_FPD_LPRUN_MSK     (0x1UL << PWR_CR1_FPD_LPRUN_POS)             /*!< 0x00000010 */
#define PWR_CR1_FPD_LPRUN         PWR_CR1_FPD_LPRUN_MSK                        /*!< Flash power down mode during run */
#define PWR_CR1_FPD_LPSLP_POS     (5U)
#define PWR_CR1_FPD_LPSLP_MSK     (0x1UL << PWR_CR1_FPD_LPSLP_POS)             /*!< 0x00000020 */
#define PWR_CR1_FPD_LPSLP         PWR_CR1_FPD_LPSLP_MSK                        /*!< Flash power down mode during sleep */
#define PWR_CR1_DBP_POS           (8U)
#define PWR_CR1_DBP_MSK           (0x1UL << PWR_CR1_DBP_POS)                   /*!< 0x00000100 */
#define PWR_CR1_DBP               PWR_CR1_DBP_MSK                              /*!< Disable Backup Domain write protection */
#define PWR_CR1_VOS_POS           (9U)
#define PWR_CR1_VOS_MSK           (0x3UL << PWR_CR1_VOS_POS)                   /*!< 0x00000600 */
#define PWR_CR1_VOS               PWR_CR1_VOS_MSK                              /*!< Voltage scaling */
#define PWR_CR1_VOS_0             (0x1UL << PWR_CR1_VOS_POS)                   /*!< Voltage scaling bit 0 */
#define PWR_CR1_VOS_1             (0x2UL << PWR_CR1_VOS_POS)                   /*!< Voltage scaling bit 1 */
#define PWR_CR1_LPR_POS           (14U)
#define PWR_CR1_LPR_MSK           (0x1UL << PWR_CR1_LPR_POS)                   /*!< 0x00004000 */
#define PWR_CR1_LPR               PWR_CR1_LPR_MSK                              /*!< Regulator Low-Power Run mode */

/********************  Bit definition for PWR_CR2 register  ********************/
#define PWR_CR2_PVDE_POS          (0U)
#define PWR_CR2_PVDE_MSK          (0x1UL << PWR_CR2_PVDE_POS)                  /*!< 0x00000001 */
#define PWR_CR2_PVDE              PWR_CR2_PVDE_MSK                             /*!< Programmable Voltage Detector Enable */
#define PWR_CR2_PVDFT_POS         (1U)
#define PWR_CR2_PVDFT_MSK         (0x7UL << PWR_CR2_PVDFT_POS)                 /*!< 0x0000000E */
#define PWR_CR2_PVDFT             PWR_CR2_PVDFT_MSK                            /*!< PVD Falling Threshold Selection bit field */
#define PWR_CR2_PVDFT_0           (0x1UL << PWR_CR2_PVDFT_POS)                 /*!< 0x00000002 */
#define PWR_CR2_PVDFT_1           (0x2UL << PWR_CR2_PVDFT_POS)                 /*!< 0x00000004 */
#define PWR_CR2_PVDFT_2           (0x4UL << PWR_CR2_PVDFT_POS)                 /*!< 0x00000008 */
#define PWR_CR2_PVDRT_POS         (4U)
#define PWR_CR2_PVDRT_MSK         (0x7UL << PWR_CR2_PVDRT_POS)                 /*!< 0x00000070 */
#define PWR_CR2_PVDRT             PWR_CR2_PVDRT_MSK                            /*!< PVD Rising Threshold Selection bit field */
#define PWR_CR2_PVDRT_0           (0x1UL << PWR_CR2_PVDRT_POS)                 /*!< 0x00000010 */
#define PWR_CR2_PVDRT_1           (0x2UL << PWR_CR2_PVDRT_POS)                 /*!< 0x00000020 */
#define PWR_CR2_PVDRT_2           (0x4UL << PWR_CR2_PVDRT_POS)                 /*!< 0x00000040 */
#define PWR_CR2_PVMEN_USB_POS     (8U)
#define PWR_CR2_PVMEN_USB_MSK     (0x1UL << PWR_CR2_PVMEN_USB_POS)             /*!< 0x00000100 */
#define PWR_CR2_PVMEN_USB         PWR_CR2_PVMEN_USB_MSK                        /*!< USB Peripheral Voltage Monitor Enable */
#define PWR_CR2_IOSV_POS          (9U)
#define PWR_CR2_IOSV_MSK          (0x1UL << PWR_CR2_IOSV_POS)                  /*!< 0x00000200 */
#define PWR_CR2_IOSV              PWR_CR2_IOSV_MSK                             /*!< VDDIO2 independent I/Os Supply Valid */
#define PWR_CR2_USV_POS           (10U)
#define PWR_CR2_USV_MSK           (0x1UL << PWR_CR2_USV_POS)                   /*!< 0x00000400 */
#define PWR_CR2_USV               PWR_CR2_USV_MSK                              /*!< VDD USB Supply Valid */

/********************  Bit definition for PWR_CR3 register  ********************/
#define PWR_CR3_EWUP_POS          (0U)
#define PWR_CR3_EWUP_MSK          (0x3FUL << PWR_CR3_EWUP_POS)                 /*!< 0x0000003F */
#define PWR_CR3_EWUP              PWR_CR3_EWUP_MSK                             /*!< Enable all Wake-Up Pins  */
#define PWR_CR3_EWUP1_POS         (0U)
#define PWR_CR3_EWUP1_MSK         (0x1UL << PWR_CR3_EWUP1_POS)                 /*!< 0x00000001 */
#define PWR_CR3_EWUP1             PWR_CR3_EWUP1_MSK                            /*!< Enable WKUP pin 1 */
#define PWR_CR3_EWUP2_POS         (1U)
#define PWR_CR3_EWUP2_MSK         (0x1UL << PWR_CR3_EWUP2_POS)                 /*!< 0x00000002 */
#define PWR_CR3_EWUP2             PWR_CR3_EWUP2_MSK                            /*!< Enable WKUP pin 2 */
#define PWR_CR3_EWUP3_POS         (2U)
#define PWR_CR3_EWUP3_MSK         (0x1UL << PWR_CR3_EWUP3_POS)                 /*!< 0x00000004 */
#define PWR_CR3_EWUP3             PWR_CR3_EWUP3_MSK                            /*!< Enable WKUP pin 3 */
#define PWR_CR3_EWUP4_POS         (3U)
#define PWR_CR3_EWUP4_MSK         (0x1UL << PWR_CR3_EWUP4_POS)                 /*!< 0x00000008 */
#define PWR_CR3_EWUP4             PWR_CR3_EWUP4_MSK                            /*!< Enable WKUP pin 4 */
#define PWR_CR3_EWUP5_POS         (4U)
#define PWR_CR3_EWUP5_MSK         (0x1UL << PWR_CR3_EWUP5_POS)                 /*!< 0x00000010 */
#define PWR_CR3_EWUP5             PWR_CR3_EWUP5_MSK                            /*!< Enable WKUP pin 5 */
#define PWR_CR3_EWUP6_POS         (5U)
#define PWR_CR3_EWUP6_MSK         (0x1UL << PWR_CR3_EWUP6_POS)                 /*!< 0x00000020 */
#define PWR_CR3_EWUP6             PWR_CR3_EWUP6_MSK                            /*!< Enable WKUP pin 6 */
#define PWR_CR3_RRS_POS           (8U)
#define PWR_CR3_RRS_MSK           (0x1UL << PWR_CR3_RRS_POS)                   /*!< 0x00000100 */
#define PWR_CR3_RRS               PWR_CR3_RRS_MSK                              /*!< RAM retention in Standby mode */
#define PWR_CR3_ENB_ULP_POS       (9U)
#define PWR_CR3_ENB_ULP_MSK       (0x1UL << PWR_CR3_ENB_ULP_POS)               /*!< 0x00000200 */
#define PWR_CR3_ENB_ULP           PWR_CR3_ENB_ULP_MSK                          /*!< Enable sampling resistor bridge in the LPMU_RESET block */
#define PWR_CR3_APC_POS           (10U)
#define PWR_CR3_APC_MSK           (0x1UL << PWR_CR3_APC_POS)                   /*!< 0x00000400 */
#define PWR_CR3_APC               PWR_CR3_APC_MSK                              /*!< Apply pull-up and pull-down configuration */
#define PWR_CR3_EIWUL_POS         (15U)
#define PWR_CR3_EIWUL_MSK         (0x1UL << PWR_CR3_EIWUL_POS)                 /*!< 0x00008000 */
#define PWR_CR3_EIWUL             PWR_CR3_EIWUL_MSK                            /*!< Enable Internal Wake-up line */

/********************  Bit definition for PWR_CR4 register  ********************/
#define PWR_CR4_WP_POS            (0U)
#define PWR_CR4_WP_MSK            (0x3FUL << PWR_CR4_WP_POS)                   /*!< 0x0000003F */
#define PWR_CR4_WP                PWR_CR4_WP_MSK                               /*!< all Wake-Up Pin polarity */
#define PWR_CR4_WP1_POS           (0U)
#define PWR_CR4_WP1_MSK           (0x1UL << PWR_CR4_WP1_POS)                   /*!< 0x00000001 */
#define PWR_CR4_WP1               PWR_CR4_WP1_MSK                              /*!< Wake-Up Pin 1 polarity */
#define PWR_CR4_WP2_POS           (1U)
#define PWR_CR4_WP2_MSK           (0x1UL << PWR_CR4_WP2_POS)                   /*!< 0x00000002 */
#define PWR_CR4_WP2               PWR_CR4_WP2_MSK                              /*!< Wake-Up Pin 2 polarity */
#define PWR_CR4_WP3_POS           (2U)
#define PWR_CR4_WP3_MSK           (0x1UL << PWR_CR4_WP3_POS)                   /*!< 0x00000004 */
#define PWR_CR4_WP3               PWR_CR4_WP3_MSK                              /*!< Wake-Up Pin 3 polarity */
#define PWR_CR4_WP4_POS           (3U)
#define PWR_CR4_WP4_MSK           (0x1UL << PWR_CR4_WP4_POS)                   /*!< 0x00000008 */
#define PWR_CR4_WP4               PWR_CR4_WP4_MSK                              /*!< Wake-Up Pin 4 polarity */
#define PWR_CR4_WP5_POS           (4U)
#define PWR_CR4_WP5_MSK           (0x1UL << PWR_CR4_WP5_POS)                   /*!< 0x00000010 */
#define PWR_CR4_WP5               PWR_CR4_WP5_MSK                              /*!< Wake-Up Pin 5 polarity */
#define PWR_CR4_WP6_POS           (5U)
#define PWR_CR4_WP6_MSK           (0x1UL << PWR_CR4_WP6_POS)                   /*!< 0x00000020 */
#define PWR_CR4_WP6               PWR_CR4_WP6_MSK                              /*!< Wake-Up Pin 6 polarity */
#define PWR_CR4_VBE_POS           (8U)
#define PWR_CR4_VBE_MSK           (0x1UL << PWR_CR4_VBE_POS)                   /*!< 0x00000100 */
#define PWR_CR4_VBE               PWR_CR4_VBE_MSK                              /*!< VBAT Battery charging Enable  */
#define PWR_CR4_VBRS_POS          (9U)
#define PWR_CR4_VBRS_MSK          (0x1UL << PWR_CR4_VBRS_POS)                  /*!< 0x00000200 */
#define PWR_CR4_VBRS              PWR_CR4_VBRS_MSK                             /*!< VBAT Battery charging Resistor Selection */

/********************  Bit definition for PWR_SR1 register  ********************/
#define PWR_SR1_WUF_POS           (0U)
#define PWR_SR1_WUF_MSK           (0x3FUL << PWR_SR1_WUF_POS)                  /*!< 0x0000003F */
#define PWR_SR1_WUF               PWR_SR1_WUF_MSK                              /*!< Wakeup Flags  */
#define PWR_SR1_WUF1_POS          (0U)
#define PWR_SR1_WUF1_MSK          (0x1UL << PWR_SR1_WUF1_POS)                  /*!< 0x00000001 */
#define PWR_SR1_WUF1              PWR_SR1_WUF1_MSK                             /*!< Wakeup Flag 1 */
#define PWR_SR1_WUF2_POS          (1U)
#define PWR_SR1_WUF2_MSK          (0x1UL << PWR_SR1_WUF2_POS)                  /*!< 0x00000002 */
#define PWR_SR1_WUF2              PWR_SR1_WUF2_MSK                             /*!< Wakeup Flag 2 */
#define PWR_SR1_WUF3_POS          (2U)
#define PWR_SR1_WUF3_MSK          (0x1UL << PWR_SR1_WUF3_POS)                  /*!< 0x00000004 */
#define PWR_SR1_WUF3              PWR_SR1_WUF3_MSK                             /*!< Wakeup Flag 3 */
#define PWR_SR1_WUF4_POS          (3U)
#define PWR_SR1_WUF4_MSK          (0x1UL << PWR_SR1_WUF4_POS)                  /*!< 0x00000008 */
#define PWR_SR1_WUF4              PWR_SR1_WUF4_MSK                             /*!< Wakeup Flag 4 */
#define PWR_SR1_WUF5_POS          (4U)
#define PWR_SR1_WUF5_MSK          (0x1UL << PWR_SR1_WUF5_POS)                  /*!< 0x00000010 */
#define PWR_SR1_WUF5              PWR_SR1_WUF5_MSK                             /*!< Wakeup Flag 5 */
#define PWR_SR1_WUF6_POS          (5U)
#define PWR_SR1_WUF6_MSK          (0x1UL << PWR_SR1_WUF6_POS)                  /*!< 0x00000020 */
#define PWR_SR1_WUF6              PWR_SR1_WUF6_MSK                             /*!< Wakeup Flag 6 */
#define PWR_SR1_SBF_POS           (8U)
#define PWR_SR1_SBF_MSK           (0x1UL << PWR_SR1_SBF_POS)                   /*!< 0x00000100 */
#define PWR_SR1_SBF               PWR_SR1_SBF_MSK                              /*!< Standby Flag  */
#define PWR_SR1_WUFI_POS          (15U)
#define PWR_SR1_WUFI_MSK          (0x1UL << PWR_SR1_WUFI_POS)                  /*!< 0x00008000 */
#define PWR_SR1_WUFI              PWR_SR1_WUFI_MSK                             /*!< Wakeup Flag Internal */

/********************  Bit definition for PWR_SR2 register  ********************/
#define PWR_SR2_FLASH_RDY_POS     (7U)
#define PWR_SR2_FLASH_RDY_MSK     (0x1UL << PWR_SR2_FLASH_RDY_POS)             /*!< 0x00000080 */
#define PWR_SR2_FLASH_RDY         PWR_SR2_FLASH_RDY_MSK                        /*!< Flash Ready */
#define PWR_SR2_REGLPS_POS        (8U)
#define PWR_SR2_REGLPS_MSK        (0x1UL << PWR_SR2_REGLPS_POS)                /*!< 0x00000100 */
#define PWR_SR2_REGLPS            PWR_SR2_REGLPS_MSK                           /*!< Regulator Low Power started */
#define PWR_SR2_REGLPF_POS        (9U)
#define PWR_SR2_REGLPF_MSK        (0x1UL << PWR_SR2_REGLPF_POS)                /*!< 0x00000200 */
#define PWR_SR2_REGLPF            PWR_SR2_REGLPF_MSK                           /*!< Regulator Low Power flag    */
#define PWR_SR2_VOSF_POS          (10U)
#define PWR_SR2_VOSF_MSK          (0x1UL << PWR_SR2_VOSF_POS)                  /*!< 0x00000400 */
#define PWR_SR2_VOSF              PWR_SR2_VOSF_MSK                             /*!< Voltage Scaling Flag */
#define PWR_SR2_PVDO_POS          (11U)
#define PWR_SR2_PVDO_MSK          (0x1UL << PWR_SR2_PVDO_POS)                  /*!< 0x00000800 */
#define PWR_SR2_PVDO              PWR_SR2_PVDO_MSK                             /*!< Power voltage detector output */
#define PWR_SR2_PVMO_USB_POS      (13U)
#define PWR_SR2_PVMO_USB_MSK      (0x1UL << PWR_SR2_PVMO_USB_POS)              /*!< 0x00002000 */
#define PWR_SR2_PVMO_USB          PWR_SR2_PVMO_USB_MSK                         /*!< USB Peripheral Voltage Monitoring Output */

/********************  Bit definition for PWR_SCR register  ********************/
#define PWR_SCR_CWUF_POS          (0U)
#define PWR_SCR_CWUF_MSK          (0x3FUL << PWR_SCR_CWUF_POS)                 /*!< 0x0000003F */
#define PWR_SCR_CWUF              PWR_SCR_CWUF_MSK                             /*!< Clear Wake-up Flags  */
#define PWR_SCR_CWUF1_POS         (0U)
#define PWR_SCR_CWUF1_MSK         (0x1UL << PWR_SCR_CWUF1_POS)                 /*!< 0x00000001 */
#define PWR_SCR_CWUF1             PWR_SCR_CWUF1_MSK                            /*!< Clear Wake-up Flag 1 */
#define PWR_SCR_CWUF2_POS         (1U)
#define PWR_SCR_CWUF2_MSK         (0x1UL << PWR_SCR_CWUF2_POS)                 /*!< 0x00000002 */
#define PWR_SCR_CWUF2             PWR_SCR_CWUF2_MSK                            /*!< Clear Wake-up Flag 2 */
#define PWR_SCR_CWUF3_POS         (2U)
#define PWR_SCR_CWUF3_MSK         (0x1UL << PWR_SCR_CWUF3_POS)                 /*!< 0x00000004 */
#define PWR_SCR_CWUF3             PWR_SCR_CWUF3_MSK                            /*!< Clear Wake-up Flag 3 */
#define PWR_SCR_CWUF4_POS         (3U)
#define PWR_SCR_CWUF4_MSK         (0x1UL << PWR_SCR_CWUF4_POS)                 /*!< 0x00000008 */
#define PWR_SCR_CWUF4             PWR_SCR_CWUF4_MSK                            /*!< Clear Wake-up Flag 4 */
#define PWR_SCR_CWUF5_POS         (4U)
#define PWR_SCR_CWUF5_MSK         (0x1UL << PWR_SCR_CWUF5_POS)                 /*!< 0x00000010 */
#define PWR_SCR_CWUF5             PWR_SCR_CWUF5_MSK                            /*!< Clear Wake-up Flag 5 */
#define PWR_SCR_CWUF6_POS         (5U)
#define PWR_SCR_CWUF6_MSK         (0x1UL << PWR_SCR_CWUF6_POS)                 /*!< 0x00000020 */
#define PWR_SCR_CWUF6             PWR_SCR_CWUF6_MSK                            /*!< Clear Wake-up Flag 6 */
#define PWR_SCR_CSBF_POS          (8U)
#define PWR_SCR_CSBF_MSK          (0x1UL << PWR_SCR_CSBF_POS)                  /*!< 0x00000100 */
#define PWR_SCR_CSBF              PWR_SCR_CSBF_MSK                             /*!< Clear Standby Flag  */

/********************  Bit definition for PWR_PUCRA register  *****************/
#define PWR_PUCRA_PU0_POS         (0U)
#define PWR_PUCRA_PU0_MSK         (0x1UL << PWR_PUCRA_PU0_POS)                 /*!< 0x00000001 */
#define PWR_PUCRA_PU0             PWR_PUCRA_PU0_MSK                            /*!< Pin PA0 Pull-Up set */
#define PWR_PUCRA_PU1_POS         (1U)
#define PWR_PUCRA_PU1_MSK         (0x1UL << PWR_PUCRA_PU1_POS)                 /*!< 0x00000002 */
#define PWR_PUCRA_PU1             PWR_PUCRA_PU1_MSK                            /*!< Pin PA1 Pull-Up set */
#define PWR_PUCRA_PU2_POS         (2U)
#define PWR_PUCRA_PU2_MSK         (0x1UL << PWR_PUCRA_PU2_POS)                 /*!< 0x00000004 */
#define PWR_PUCRA_PU2             PWR_PUCRA_PU2_MSK                            /*!< Pin PA2 Pull-Up set */
#define PWR_PUCRA_PU3_POS         (3U)
#define PWR_PUCRA_PU3_MSK         (0x1UL << PWR_PUCRA_PU3_POS)                 /*!< 0x00000008 */
#define PWR_PUCRA_PU3             PWR_PUCRA_PU3_MSK                            /*!< Pin PA3 Pull-Up set */
#define PWR_PUCRA_PU4_POS         (4U)
#define PWR_PUCRA_PU4_MSK         (0x1UL << PWR_PUCRA_PU4_POS)                 /*!< 0x00000010 */
#define PWR_PUCRA_PU4             PWR_PUCRA_PU4_MSK                            /*!< Pin PA4 Pull-Up set */
#define PWR_PUCRA_PU5_POS         (5U)
#define PWR_PUCRA_PU5_MSK         (0x1UL << PWR_PUCRA_PU5_POS)                 /*!< 0x00000020 */
#define PWR_PUCRA_PU5             PWR_PUCRA_PU5_MSK                            /*!< Pin PA5 Pull-Up set */
#define PWR_PUCRA_PU6_POS         (6U)
#define PWR_PUCRA_PU6_MSK         (0x1UL << PWR_PUCRA_PU6_POS)                 /*!< 0x00000040 */
#define PWR_PUCRA_PU6             PWR_PUCRA_PU6_MSK                            /*!< Pin PA6 Pull-Up set */
#define PWR_PUCRA_PU7_POS         (7U)
#define PWR_PUCRA_PU7_MSK         (0x1UL << PWR_PUCRA_PU7_POS)                 /*!< 0x00000080 */
#define PWR_PUCRA_PU7             PWR_PUCRA_PU7_MSK                            /*!< Pin PA7 Pull-Up set */
#define PWR_PUCRA_PU8_POS         (8U)
#define PWR_PUCRA_PU8_MSK         (0x1UL << PWR_PUCRA_PU8_POS)                 /*!< 0x00000100 */
#define PWR_PUCRA_PU8             PWR_PUCRA_PU8_MSK                            /*!< Pin PA8 Pull-Up set */
#define PWR_PUCRA_PU9_POS         (9U)
#define PWR_PUCRA_PU9_MSK         (0x1UL << PWR_PUCRA_PU9_POS)                 /*!< 0x00000200 */
#define PWR_PUCRA_PU9             PWR_PUCRA_PU9_MSK                            /*!< Pin PA9 Pull-Up set */
#define PWR_PUCRA_PU10_POS        (10U)
#define PWR_PUCRA_PU10_MSK        (0x1UL << PWR_PUCRA_PU10_POS)                /*!< 0x00000400 */
#define PWR_PUCRA_PU10            PWR_PUCRA_PU10_MSK                           /*!< Pin PA10 Pull-Up set */
#define PWR_PUCRA_PU11_POS        (11U)
#define PWR_PUCRA_PU11_MSK        (0x1UL << PWR_PUCRA_PU11_POS)                /*!< 0x00000800 */
#define PWR_PUCRA_PU11            PWR_PUCRA_PU11_MSK                           /*!< Pin PA11 Pull-Up set */
#define PWR_PUCRA_PU12_POS        (12U)
#define PWR_PUCRA_PU12_MSK        (0x1UL << PWR_PUCRA_PU12_POS)                /*!< 0x00001000 */
#define PWR_PUCRA_PU12            PWR_PUCRA_PU12_MSK                           /*!< Pin PA12 Pull-Up set */
#define PWR_PUCRA_PU13_POS        (13U)
#define PWR_PUCRA_PU13_MSK        (0x1UL << PWR_PUCRA_PU13_POS)                /*!< 0x00002000 */
#define PWR_PUCRA_PU13            PWR_PUCRA_PU13_MSK                           /*!< Pin PA13 Pull-Up set */
#define PWR_PUCRA_PU14_POS        (14U)
#define PWR_PUCRA_PU14_MSK        (0x1UL << PWR_PUCRA_PU14_POS)                /*!< 0x00004000 */
#define PWR_PUCRA_PU14            PWR_PUCRA_PU14_MSK                           /*!< Pin PA14 Pull-Up set */
#define PWR_PUCRA_PU15_POS        (15U)
#define PWR_PUCRA_PU15_MSK        (0x1UL << PWR_PUCRA_PU15_POS)                /*!< 0x00008000 */
#define PWR_PUCRA_PU15            PWR_PUCRA_PU15_MSK                           /*!< Pin PA15 Pull-Up set */

/********************  Bit definition for PWR_PDCRA register  *****************/
#define PWR_PDCRA_PD0_POS         (0U)
#define PWR_PDCRA_PD0_MSK         (0x1UL << PWR_PDCRA_PD0_POS)                 /*!< 0x00000001 */
#define PWR_PDCRA_PD0             PWR_PDCRA_PD0_MSK                            /*!< Pin PA0 Pull-Down set */
#define PWR_PDCRA_PD1_POS         (1U)
#define PWR_PDCRA_PD1_MSK         (0x1UL << PWR_PDCRA_PD1_POS)                 /*!< 0x00000002 */
#define PWR_PDCRA_PD1             PWR_PDCRA_PD1_MSK                            /*!< Pin PA1 Pull-Down set */
#define PWR_PDCRA_PD2_POS         (2U)
#define PWR_PDCRA_PD2_MSK         (0x1UL << PWR_PDCRA_PD2_POS)                 /*!< 0x00000004 */
#define PWR_PDCRA_PD2             PWR_PDCRA_PD2_MSK                            /*!< Pin PA2 Pull-Down set */
#define PWR_PDCRA_PD3_POS         (3U)
#define PWR_PDCRA_PD3_MSK         (0x1UL << PWR_PDCRA_PD3_POS)                 /*!< 0x00000008 */
#define PWR_PDCRA_PD3             PWR_PDCRA_PD3_MSK                            /*!< Pin PA3 Pull-Down set */
#define PWR_PDCRA_PD4_POS         (4U)
#define PWR_PDCRA_PD4_MSK         (0x1UL << PWR_PDCRA_PD4_POS)                 /*!< 0x00000010 */
#define PWR_PDCRA_PD4             PWR_PDCRA_PD4_MSK                            /*!< Pin PA4 Pull-Down set */
#define PWR_PDCRA_PD5_POS         (5U)
#define PWR_PDCRA_PD5_MSK         (0x1UL << PWR_PDCRA_PD5_POS)                 /*!< 0x00000020 */
#define PWR_PDCRA_PD5             PWR_PDCRA_PD5_MSK                            /*!< Pin PA5 Pull-Down set */
#define PWR_PDCRA_PD6_POS         (6U)
#define PWR_PDCRA_PD6_MSK         (0x1UL << PWR_PDCRA_PD6_POS)                 /*!< 0x00000040 */
#define PWR_PDCRA_PD6             PWR_PDCRA_PD6_MSK                            /*!< Pin PA6 Pull-Down set */
#define PWR_PDCRA_PD7_POS         (7U)
#define PWR_PDCRA_PD7_MSK         (0x1UL << PWR_PDCRA_PD7_POS)                 /*!< 0x00000080 */
#define PWR_PDCRA_PD7             PWR_PDCRA_PD7_MSK                            /*!< Pin PA7 Pull-Down set */
#define PWR_PDCRA_PD8_POS         (8U)
#define PWR_PDCRA_PD8_MSK         (0x1UL << PWR_PDCRA_PD8_POS)                 /*!< 0x00000100 */
#define PWR_PDCRA_PD8             PWR_PDCRA_PD8_MSK                            /*!< Pin PA8 Pull-Down set */
#define PWR_PDCRA_PD9_POS         (9U)
#define PWR_PDCRA_PD9_MSK         (0x1UL << PWR_PDCRA_PD9_POS)                 /*!< 0x00000200 */
#define PWR_PDCRA_PD9             PWR_PDCRA_PD9_MSK                            /*!< Pin PA9 Pull-Down set */
#define PWR_PDCRA_PD10_POS        (10U)
#define PWR_PDCRA_PD10_MSK        (0x1UL << PWR_PDCRA_PD10_POS)                /*!< 0x00000400 */
#define PWR_PDCRA_PD10            PWR_PDCRA_PD10_MSK                           /*!< Pin PA10 Pull-Down set */
#define PWR_PDCRA_PD11_POS        (11U)
#define PWR_PDCRA_PD11_MSK        (0x1UL << PWR_PDCRA_PD11_POS)                /*!< 0x00000800 */
#define PWR_PDCRA_PD11            PWR_PDCRA_PD11_MSK                           /*!< Pin PA11 Pull-Down set */
#define PWR_PDCRA_PD12_POS        (12U)
#define PWR_PDCRA_PD12_MSK        (0x1UL << PWR_PDCRA_PD12_POS)                /*!< 0x00001000 */
#define PWR_PDCRA_PD12            PWR_PDCRA_PD12_MSK                           /*!< Pin PA12 Pull-Down set */
#define PWR_PDCRA_PD13_POS        (13U)
#define PWR_PDCRA_PD13_MSK        (0x1UL << PWR_PDCRA_PD13_POS)                /*!< 0x00002000 */
#define PWR_PDCRA_PD13            PWR_PDCRA_PD13_MSK                           /*!< Pin PA13 Pull-Down set */
#define PWR_PDCRA_PD14_POS        (14U)
#define PWR_PDCRA_PD14_MSK        (0x1UL << PWR_PDCRA_PD14_POS)                /*!< 0x00004000 */
#define PWR_PDCRA_PD14            PWR_PDCRA_PD14_MSK                           /*!< Pin PA14 Pull-Down set */
#define PWR_PDCRA_PD15_POS        (15U)
#define PWR_PDCRA_PD15_MSK        (0x1UL << PWR_PDCRA_PD15_POS)                /*!< 0x00008000 */
#define PWR_PDCRA_PD15            PWR_PDCRA_PD15_MSK                           /*!< Pin PA15 Pull-Down set */

/********************  Bit definition for PWR_PUCRB register  *****************/
#define PWR_PUCRB_PU0_POS         (0U)
#define PWR_PUCRB_PU0_MSK         (0x1UL << PWR_PUCRB_PU0_POS)                 /*!< 0x00000001 */
#define PWR_PUCRB_PU0             PWR_PUCRB_PU0_MSK                            /*!< Pin PB0 Pull-Up set */
#define PWR_PUCRB_PU1_POS         (1U)
#define PWR_PUCRB_PU1_MSK         (0x1UL << PWR_PUCRB_PU1_POS)                 /*!< 0x00000002 */
#define PWR_PUCRB_PU1             PWR_PUCRB_PU1_MSK                            /*!< Pin PB1 Pull-Up set */
#define PWR_PUCRB_PU2_POS         (2U)
#define PWR_PUCRB_PU2_MSK         (0x1UL << PWR_PUCRB_PU2_POS)                 /*!< 0x00000004 */
#define PWR_PUCRB_PU2             PWR_PUCRB_PU2_MSK                            /*!< Pin PB2 Pull-Up set */
#define PWR_PUCRB_PU3_POS         (3U)
#define PWR_PUCRB_PU3_MSK         (0x1UL << PWR_PUCRB_PU3_POS)                 /*!< 0x00000008 */
#define PWR_PUCRB_PU3             PWR_PUCRB_PU3_MSK                            /*!< Pin PB3 Pull-Up set */
#define PWR_PUCRB_PU4_POS         (4U)
#define PWR_PUCRB_PU4_MSK         (0x1UL << PWR_PUCRB_PU4_POS)                 /*!< 0x00000010 */
#define PWR_PUCRB_PU4             PWR_PUCRB_PU4_MSK                            /*!< Pin PB4 Pull-Up set */
#define PWR_PUCRB_PU5_POS         (5U)
#define PWR_PUCRB_PU5_MSK         (0x1UL << PWR_PUCRB_PU5_POS)                 /*!< 0x00000020 */
#define PWR_PUCRB_PU5             PWR_PUCRB_PU5_MSK                            /*!< Pin PB5 Pull-Up set */
#define PWR_PUCRB_PU6_POS         (6U)
#define PWR_PUCRB_PU6_MSK         (0x1UL << PWR_PUCRB_PU6_POS)                 /*!< 0x00000040 */
#define PWR_PUCRB_PU6             PWR_PUCRB_PU6_MSK                            /*!< Pin PB6 Pull-Up set */
#define PWR_PUCRB_PU7_POS         (7U)
#define PWR_PUCRB_PU7_MSK         (0x1UL << PWR_PUCRB_PU7_POS)                 /*!< 0x00000080 */
#define PWR_PUCRB_PU7             PWR_PUCRB_PU7_MSK                            /*!< Pin PB7 Pull-Up set */
#define PWR_PUCRB_PU8_POS         (8U)
#define PWR_PUCRB_PU8_MSK         (0x1UL << PWR_PUCRB_PU8_POS)                 /*!< 0x00000100 */
#define PWR_PUCRB_PU8             PWR_PUCRB_PU8_MSK                            /*!< Pin PB8 Pull-Up set */
#define PWR_PUCRB_PU9_POS         (9U)
#define PWR_PUCRB_PU9_MSK         (0x1UL << PWR_PUCRB_PU9_POS)                 /*!< 0x00000200 */
#define PWR_PUCRB_PU9             PWR_PUCRB_PU9_MSK                            /*!< Pin PB9 Pull-Up set */
#define PWR_PUCRB_PU10_POS        (10U)
#define PWR_PUCRB_PU10_MSK        (0x1UL << PWR_PUCRB_PU10_POS)                /*!< 0x00000400 */
#define PWR_PUCRB_PU10            PWR_PUCRB_PU10_MSK                           /*!< Pin PB10 Pull-Up set */
#define PWR_PUCRB_PU11_POS        (11U)
#define PWR_PUCRB_PU11_MSK        (0x1UL << PWR_PUCRB_PU11_POS)                /*!< 0x00000800 */
#define PWR_PUCRB_PU11            PWR_PUCRB_PU11_MSK                           /*!< Pin PB11 Pull-Up set */
#define PWR_PUCRB_PU12_POS        (12U)
#define PWR_PUCRB_PU12_MSK        (0x1UL << PWR_PUCRB_PU12_POS)                /*!< 0x00001000 */
#define PWR_PUCRB_PU12            PWR_PUCRB_PU12_MSK                           /*!< Pin PB12 Pull-Up set */
#define PWR_PUCRB_PU13_POS        (13U)
#define PWR_PUCRB_PU13_MSK        (0x1UL << PWR_PUCRB_PU13_POS)                /*!< 0x00002000 */
#define PWR_PUCRB_PU13            PWR_PUCRB_PU13_MSK                           /*!< Pin PB13 Pull-Up set */
#define PWR_PUCRB_PU14_POS        (14U)
#define PWR_PUCRB_PU14_MSK        (0x1UL << PWR_PUCRB_PU14_POS)                /*!< 0x00004000 */
#define PWR_PUCRB_PU14            PWR_PUCRB_PU14_MSK                           /*!< Pin PB14 Pull-Up set */
#define PWR_PUCRB_PU15_POS        (15U)
#define PWR_PUCRB_PU15_MSK        (0x1UL << PWR_PUCRB_PU15_POS)                /*!< 0x00008000 */
#define PWR_PUCRB_PU15            PWR_PUCRB_PU15_MSK                           /*!< Pin PB15 Pull-Up set */

/********************  Bit definition for PWR_PDCRB register  *****************/
#define PWR_PDCRB_PD0_POS         (0U)
#define PWR_PDCRB_PD0_MSK         (0x1UL << PWR_PDCRB_PD0_POS)                 /*!< 0x00000001 */
#define PWR_PDCRB_PD0             PWR_PDCRB_PD0_MSK                            /*!< Pin PB0 Pull-Down set */
#define PWR_PDCRB_PD1_POS         (1U)
#define PWR_PDCRB_PD1_MSK         (0x1UL << PWR_PDCRB_PD1_POS)                 /*!< 0x00000002 */
#define PWR_PDCRB_PD1             PWR_PDCRB_PD1_MSK                            /*!< Pin PB1 Pull-Down set */
#define PWR_PDCRB_PD2_POS         (2U)
#define PWR_PDCRB_PD2_MSK         (0x1UL << PWR_PDCRB_PD2_POS)                 /*!< 0x00000004 */
#define PWR_PDCRB_PD2             PWR_PDCRB_PD2_MSK                            /*!< Pin PB2 Pull-Down set */
#define PWR_PDCRB_PD3_POS         (3U)
#define PWR_PDCRB_PD3_MSK         (0x1UL << PWR_PDCRB_PD3_POS)                 /*!< 0x00000008 */
#define PWR_PDCRB_PD3             PWR_PDCRB_PD3_MSK                            /*!< Pin PB3 Pull-Down set */
#define PWR_PDCRB_PD4_POS         (4U)
#define PWR_PDCRB_PD4_MSK         (0x1UL << PWR_PDCRB_PD4_POS)                 /*!< 0x00000010 */
#define PWR_PDCRB_PD4             PWR_PDCRB_PD4_MSK                            /*!< Pin PB4 Pull-Down set */
#define PWR_PDCRB_PD5_POS         (5U)
#define PWR_PDCRB_PD5_MSK         (0x1UL << PWR_PDCRB_PD5_POS)                 /*!< 0x00000020 */
#define PWR_PDCRB_PD5             PWR_PDCRB_PD5_MSK                            /*!< Pin PB5 Pull-Down set */
#define PWR_PDCRB_PD6_POS         (6U)
#define PWR_PDCRB_PD6_MSK         (0x1UL << PWR_PDCRB_PD6_POS)                 /*!< 0x00000040 */
#define PWR_PDCRB_PD6             PWR_PDCRB_PD6_MSK                            /*!< Pin PB6 Pull-Down set */
#define PWR_PDCRB_PD7_POS         (7U)
#define PWR_PDCRB_PD7_MSK         (0x1UL << PWR_PDCRB_PD7_POS)                 /*!< 0x00000080 */
#define PWR_PDCRB_PD7             PWR_PDCRB_PD7_MSK                            /*!< Pin PB7 Pull-Down set */
#define PWR_PDCRB_PD8_POS         (8U)
#define PWR_PDCRB_PD8_MSK         (0x1UL << PWR_PDCRB_PD8_POS)                 /*!< 0x00000100 */
#define PWR_PDCRB_PD8             PWR_PDCRB_PD8_MSK                            /*!< Pin PB8 Pull-Down set */
#define PWR_PDCRB_PD9_POS         (9U)
#define PWR_PDCRB_PD9_MSK         (0x1UL << PWR_PDCRB_PD9_POS)                 /*!< 0x00000200 */
#define PWR_PDCRB_PD9             PWR_PDCRB_PD9_MSK                            /*!< Pin PB9 Pull-Down set */
#define PWR_PDCRB_PD10_POS        (10U)
#define PWR_PDCRB_PD10_MSK        (0x1UL << PWR_PDCRB_PD10_POS)                /*!< 0x00000400 */
#define PWR_PDCRB_PD10            PWR_PDCRB_PD10_MSK                           /*!< Pin PB10 Pull-Down set */
#define PWR_PDCRB_PD11_POS        (11U)
#define PWR_PDCRB_PD11_MSK        (0x1UL << PWR_PDCRB_PD11_POS)                /*!< 0x00000800 */
#define PWR_PDCRB_PD11            PWR_PDCRB_PD11_MSK                           /*!< Pin PB11 Pull-Down set */
#define PWR_PDCRB_PD12_POS        (12U)
#define PWR_PDCRB_PD12_MSK        (0x1UL << PWR_PDCRB_PD12_POS)                /*!< 0x00001000 */
#define PWR_PDCRB_PD12            PWR_PDCRB_PD12_MSK                           /*!< Pin PB12 Pull-Down set */
#define PWR_PDCRB_PD13_POS        (13U)
#define PWR_PDCRB_PD13_MSK        (0x1UL << PWR_PDCRB_PD13_POS)                /*!< 0x00002000 */
#define PWR_PDCRB_PD13            PWR_PDCRB_PD13_MSK                           /*!< Pin PB13 Pull-Down set */
#define PWR_PDCRB_PD14_POS        (14U)
#define PWR_PDCRB_PD14_MSK        (0x1UL << PWR_PDCRB_PD14_POS)                /*!< 0x00004000 */
#define PWR_PDCRB_PD14            PWR_PDCRB_PD14_MSK                           /*!< Pin PB14 Pull-Down set */
#define PWR_PDCRB_PD15_POS        (15U)
#define PWR_PDCRB_PD15_MSK        (0x1UL << PWR_PDCRB_PD15_POS)                /*!< 0x00008000 */
#define PWR_PDCRB_PD15            PWR_PDCRB_PD15_MSK                           /*!< Pin PB15 Pull-Down set */

/********************  Bit definition for PWR_PUCRC register  *****************/
#define PWR_PUCRC_PU0_POS         (0U)
#define PWR_PUCRC_PU0_MSK         (0x1UL << PWR_PUCRC_PU0_POS)                 /*!< 0x00000001 */
#define PWR_PUCRC_PU0             PWR_PUCRC_PU0_MSK                            /*!< Pin PC0 Pull-Up set */
#define PWR_PUCRC_PU1_POS         (1U)
#define PWR_PUCRC_PU1_MSK         (0x1UL << PWR_PUCRC_PU1_POS)                 /*!< 0x00000002 */
#define PWR_PUCRC_PU1             PWR_PUCRC_PU1_MSK                            /*!< Pin PC1 Pull-Up set */
#define PWR_PUCRC_PU2_POS         (2U)
#define PWR_PUCRC_PU2_MSK         (0x1UL << PWR_PUCRC_PU2_POS)                 /*!< 0x00000004 */
#define PWR_PUCRC_PU2             PWR_PUCRC_PU2_MSK                            /*!< Pin PC2 Pull-Up set */
#define PWR_PUCRC_PU3_POS         (3U)
#define PWR_PUCRC_PU3_MSK         (0x1UL << PWR_PUCRC_PU3_POS)                 /*!< 0x00000008 */
#define PWR_PUCRC_PU3             PWR_PUCRC_PU3_MSK                            /*!< Pin PC3 Pull-Up set */
#define PWR_PUCRC_PU4_POS         (4U)
#define PWR_PUCRC_PU4_MSK         (0x1UL << PWR_PUCRC_PU4_POS)                 /*!< 0x00000010 */
#define PWR_PUCRC_PU4             PWR_PUCRC_PU4_MSK                            /*!< Pin PC4 Pull-Up set */
#define PWR_PUCRC_PU5_POS         (5U)
#define PWR_PUCRC_PU5_MSK         (0x1UL << PWR_PUCRC_PU5_POS)                 /*!< 0x00000020 */
#define PWR_PUCRC_PU5             PWR_PUCRC_PU5_MSK                            /*!< Pin PC5 Pull-Up set */
#define PWR_PUCRC_PU6_POS         (6U)
#define PWR_PUCRC_PU6_MSK         (0x1UL << PWR_PUCRC_PU6_POS)                 /*!< 0x00000040 */
#define PWR_PUCRC_PU6             PWR_PUCRC_PU6_MSK                            /*!< Pin PC6 Pull-Up set */
#define PWR_PUCRC_PU7_POS         (7U)
#define PWR_PUCRC_PU7_MSK         (0x1UL << PWR_PUCRC_PU7_POS)                 /*!< 0x00000080 */
#define PWR_PUCRC_PU7             PWR_PUCRC_PU7_MSK                            /*!< Pin PC7 Pull-Up set */
#define PWR_PUCRC_PU8_POS         (8U)
#define PWR_PUCRC_PU8_MSK         (0x1UL << PWR_PUCRC_PU8_POS)                 /*!< 0x00000100 */
#define PWR_PUCRC_PU8             PWR_PUCRC_PU8_MSK                            /*!< Pin PC8 Pull-Up set */
#define PWR_PUCRC_PU9_POS         (9U)
#define PWR_PUCRC_PU9_MSK         (0x1UL << PWR_PUCRC_PU9_POS)                 /*!< 0x00000200 */
#define PWR_PUCRC_PU9             PWR_PUCRC_PU9_MSK                            /*!< Pin PC9 Pull-Up set */
#define PWR_PUCRC_PU10_POS        (10U)
#define PWR_PUCRC_PU10_MSK        (0x1UL << PWR_PUCRC_PU10_POS)                /*!< 0x00000400 */
#define PWR_PUCRC_PU10            PWR_PUCRC_PU10_MSK                           /*!< Pin PC10 Pull-Up set */
#define PWR_PUCRC_PU11_POS        (11U)
#define PWR_PUCRC_PU11_MSK        (0x1UL << PWR_PUCRC_PU11_POS)                /*!< 0x00000800 */
#define PWR_PUCRC_PU11            PWR_PUCRC_PU11_MSK                           /*!< Pin PC11 Pull-Up set */
#define PWR_PUCRC_PU12_POS        (12U)
#define PWR_PUCRC_PU12_MSK        (0x1UL << PWR_PUCRC_PU12_POS)                /*!< 0x00001000 */
#define PWR_PUCRC_PU12            PWR_PUCRC_PU12_MSK                           /*!< Pin PC12 Pull-Up set */
#define PWR_PUCRC_PU13_POS        (13U)
#define PWR_PUCRC_PU13_MSK        (0x1UL << PWR_PUCRC_PU13_POS)                /*!< 0x00002000 */
#define PWR_PUCRC_PU13            PWR_PUCRC_PU13_MSK                           /*!< Pin PC13 Pull-Up set */
#define PWR_PUCRC_PU14_POS        (14U)
#define PWR_PUCRC_PU14_MSK        (0x1UL << PWR_PUCRC_PU14_POS)                /*!< 0x00004000 */
#define PWR_PUCRC_PU14            PWR_PUCRC_PU14_MSK                           /*!< Pin PC14 Pull-Up set */
#define PWR_PUCRC_PU15_POS        (15U)
#define PWR_PUCRC_PU15_MSK        (0x1UL << PWR_PUCRC_PU15_POS)                /*!< 0x00008000 */
#define PWR_PUCRC_PU15            PWR_PUCRC_PU15_MSK                           /*!< Pin PC15 Pull-Up set */

/********************  Bit definition for PWR_PDCRC register  *****************/
#define PWR_PDCRC_PD0_POS         (0U)
#define PWR_PDCRC_PD0_MSK         (0x1UL << PWR_PDCRC_PD0_POS)                 /*!< 0x00000001 */
#define PWR_PDCRC_PD0             PWR_PDCRC_PD0_MSK                            /*!< Pin PC0 Pull-Down set */
#define PWR_PDCRC_PD1_POS         (1U)
#define PWR_PDCRC_PD1_MSK         (0x1UL << PWR_PDCRC_PD1_POS)                 /*!< 0x00000002 */
#define PWR_PDCRC_PD1             PWR_PDCRC_PD1_MSK                            /*!< Pin PC1 Pull-Down set */
#define PWR_PDCRC_PD2_POS         (2U)
#define PWR_PDCRC_PD2_MSK         (0x1UL << PWR_PDCRC_PD2_POS)                 /*!< 0x00000004 */
#define PWR_PDCRC_PD2             PWR_PDCRC_PD2_MSK                            /*!< Pin PC2 Pull-Down set */
#define PWR_PDCRC_PD3_POS         (3U)
#define PWR_PDCRC_PD3_MSK         (0x1UL << PWR_PDCRC_PD3_POS)                 /*!< 0x00000008 */
#define PWR_PDCRC_PD3             PWR_PDCRC_PD3_MSK                            /*!< Pin PC3 Pull-Down set */
#define PWR_PDCRC_PD4_POS         (4U)
#define PWR_PDCRC_PD4_MSK         (0x1UL << PWR_PDCRC_PD4_POS)                 /*!< 0x00000010 */
#define PWR_PDCRC_PD4             PWR_PDCRC_PD4_MSK                            /*!< Pin PC4 Pull-Down set */
#define PWR_PDCRC_PD5_POS         (5U)
#define PWR_PDCRC_PD5_MSK         (0x1UL << PWR_PDCRC_PD5_POS)                 /*!< 0x00000020 */
#define PWR_PDCRC_PD5             PWR_PDCRC_PD5_MSK                            /*!< Pin PC5 Pull-Down set */
#define PWR_PDCRC_PD6_POS         (6U)
#define PWR_PDCRC_PD6_MSK         (0x1UL << PWR_PDCRC_PD6_POS)                 /*!< 0x00000040 */
#define PWR_PDCRC_PD6             PWR_PDCRC_PD6_MSK                            /*!< Pin PC6 Pull-Down set */
#define PWR_PDCRC_PD7_POS         (7U)
#define PWR_PDCRC_PD7_MSK         (0x1UL << PWR_PDCRC_PD7_POS)                 /*!< 0x00000080 */
#define PWR_PDCRC_PD7             PWR_PDCRC_PD7_MSK                            /*!< Pin PC7 Pull-Down set */
#define PWR_PDCRC_PD8_POS         (8U)
#define PWR_PDCRC_PD8_MSK         (0x1UL << PWR_PDCRC_PD8_POS)                 /*!< 0x00000100 */
#define PWR_PDCRC_PD8             PWR_PDCRC_PD8_MSK                            /*!< Pin PC8 Pull-Down set */
#define PWR_PDCRC_PD9_POS         (9U)
#define PWR_PDCRC_PD9_MSK         (0x1UL << PWR_PDCRC_PD9_POS)                 /*!< 0x00000200 */
#define PWR_PDCRC_PD9             PWR_PDCRC_PD9_MSK                            /*!< Pin PC9 Pull-Down set */
#define PWR_PDCRC_PD10_POS        (10U)
#define PWR_PDCRC_PD10_MSK        (0x1UL << PWR_PDCRC_PD10_POS)                /*!< 0x00000400 */
#define PWR_PDCRC_PD10            PWR_PDCRC_PD10_MSK                           /*!< Pin PC10 Pull-Down set */
#define PWR_PDCRC_PD11_POS        (11U)
#define PWR_PDCRC_PD11_MSK        (0x1UL << PWR_PDCRC_PD11_POS)                /*!< 0x00000800 */
#define PWR_PDCRC_PD11            PWR_PDCRC_PD11_MSK                           /*!< Pin PC11 Pull-Down set */
#define PWR_PDCRC_PD12_POS        (12U)
#define PWR_PDCRC_PD12_MSK        (0x1UL << PWR_PDCRC_PD12_POS)                /*!< 0x00001000 */
#define PWR_PDCRC_PD12            PWR_PDCRC_PD12_MSK                           /*!< Pin PC12 Pull-Down set */
#define PWR_PDCRC_PD13_POS        (13U)
#define PWR_PDCRC_PD13_MSK        (0x1UL << PWR_PDCRC_PD13_POS)                /*!< 0x00002000 */
#define PWR_PDCRC_PD13            PWR_PDCRC_PD13_MSK                           /*!< Pin PC13 Pull-Down set */
#define PWR_PDCRC_PD14_POS        (14U)
#define PWR_PDCRC_PD14_MSK        (0x1UL << PWR_PDCRC_PD14_POS)                /*!< 0x00004000 */
#define PWR_PDCRC_PD14            PWR_PDCRC_PD14_MSK                           /*!< Pin PC14 Pull-Down set */
#define PWR_PDCRC_PD15_POS        (15U)
#define PWR_PDCRC_PD15_MSK        (0x1UL << PWR_PDCRC_PD15_POS)                /*!< 0x00008000 */
#define PWR_PDCRC_PD15            PWR_PDCRC_PD15_MSK                           /*!< Pin PC15 Pull-Down set */

/********************  Bit definition for PWR_PUCRD register  *****************/
#define PWR_PUCRD_PU0_POS         (0U)
#define PWR_PUCRD_PU0_MSK         (0x1UL << PWR_PUCRD_PU0_POS)                 /*!< 0x00000001 */
#define PWR_PUCRD_PU0             PWR_PUCRD_PU0_MSK                            /*!< Pin PD0 Pull-Up set */
#define PWR_PUCRD_PU1_POS         (1U)
#define PWR_PUCRD_PU1_MSK         (0x1UL << PWR_PUCRD_PU1_POS)                 /*!< 0x00000002 */
#define PWR_PUCRD_PU1             PWR_PUCRD_PU1_MSK                            /*!< Pin PD1 Pull-Up set */
#define PWR_PUCRD_PU2_POS         (2U)
#define PWR_PUCRD_PU2_MSK         (0x1UL << PWR_PUCRD_PU2_POS)                 /*!< 0x00000004 */
#define PWR_PUCRD_PU2             PWR_PUCRD_PU2_MSK                            /*!< Pin PD2 Pull-Up set */
#define PWR_PUCRD_PU3_POS         (3U)
#define PWR_PUCRD_PU3_MSK         (0x1UL << PWR_PUCRD_PU3_POS)                 /*!< 0x00000008 */
#define PWR_PUCRD_PU3             PWR_PUCRD_PU3_MSK                            /*!< Pin PD3 Pull-Up set */
#define PWR_PUCRD_PU4_POS         (4U)
#define PWR_PUCRD_PU4_MSK         (0x1UL << PWR_PUCRD_PU4_POS)                 /*!< 0x00000010 */
#define PWR_PUCRD_PU4             PWR_PUCRD_PU4_MSK                            /*!< Pin PD4 Pull-Up set */
#define PWR_PUCRD_PU5_POS         (5U)
#define PWR_PUCRD_PU5_MSK         (0x1UL << PWR_PUCRD_PU5_POS)                 /*!< 0x00000020 */
#define PWR_PUCRD_PU5             PWR_PUCRD_PU5_MSK                            /*!< Pin PD5 Pull-Up set */
#define PWR_PUCRD_PU6_POS         (6U)
#define PWR_PUCRD_PU6_MSK         (0x1UL << PWR_PUCRD_PU6_POS)                 /*!< 0x00000040 */
#define PWR_PUCRD_PU6             PWR_PUCRD_PU6_MSK                            /*!< Pin PD6 Pull-Up set */
#define PWR_PUCRD_PU8_POS         (8U)
#define PWR_PUCRD_PU8_MSK         (0x1UL << PWR_PUCRD_PU8_POS)                 /*!< 0x00000100 */
#define PWR_PUCRD_PU8             PWR_PUCRD_PU8_MSK                            /*!< Pin PD8 Pull-Up set */
#define PWR_PUCRD_PU9_POS         (9U)
#define PWR_PUCRD_PU9_MSK         (0x1UL << PWR_PUCRD_PU9_POS)                 /*!< 0x00000200 */
#define PWR_PUCRD_PU9             PWR_PUCRD_PU9_MSK                            /*!< Pin PD9 Pull-Up set */
#define PWR_PUCRD_PD10_POS        (10U)
#define PWR_PUCRD_PD10_MSK        (0x1UL << PWR_PUCRD_PD10_POS)                /*!< 0x00000400 */
#define PWR_PUCRD_PD10            PWR_PUCRD_PD10_MSK                           /*!< Pin PD10 Pull-Up set */
#define PWR_PUCRD_PD11_POS        (11U)
#define PWR_PUCRD_PD11_MSK        (0x1UL << PWR_PUCRD_PD11_POS)                /*!< 0x00000800 */
#define PWR_PUCRD_PD11            PWR_PUCRD_PD11_MSK                           /*!< Pin PD11 Pull-Up set */
#define PWR_PUCRD_PD12_POS        (12U)
#define PWR_PUCRD_PD12_MSK        (0x1UL << PWR_PUCRD_PD12_POS)                /*!< 0x00001000 */
#define PWR_PUCRD_PD12            PWR_PUCRD_PD12_MSK                           /*!< Pin PD12 Pull-Up set */
#define PWR_PUCRD_PD13_POS        (13U)
#define PWR_PUCRD_PD13_MSK        (0x1UL << PWR_PUCRD_PD13_POS)                /*!< 0x00002000 */
#define PWR_PUCRD_PD13            PWR_PUCRD_PD13_MSK                           /*!< Pin PD13 Pull-Up set */
#define PWR_PUCRD_PD14_POS        (14U)
#define PWR_PUCRD_PD14_MSK        (0x1UL << PWR_PUCRD_PD14_POS)                /*!< 0x00004000 */
#define PWR_PUCRD_PD14            PWR_PUCRD_PD14_MSK                           /*!< Pin PD14 Pull-Up set */
#define PWR_PUCRD_PD15_POS        (15U)
#define PWR_PUCRD_PD15_MSK        (0x1UL << PWR_PUCRD_PD15_POS)                /*!< 0x00008000 */
#define PWR_PUCRD_PD15            PWR_PUCRD_PD15_MSK                           /*!< Pin PD15 Pull-Up set */

/********************  Bit definition for PWR_PDCRD register  *****************/
#define PWR_PDCRD_PD0_POS         (0U)
#define PWR_PDCRD_PD0_MSK         (0x1UL << PWR_PDCRD_PD0_POS)                 /*!< 0x00000001 */
#define PWR_PDCRD_PD0             PWR_PDCRD_PD0_MSK                            /*!< Pin PD0 Pull-Down set */
#define PWR_PDCRD_PD1_POS         (1U)
#define PWR_PDCRD_PD1_MSK         (0x1UL << PWR_PDCRD_PD1_POS)                 /*!< 0x00000002 */
#define PWR_PDCRD_PD1             PWR_PDCRD_PD1_MSK                            /*!< Pin PD1 Pull-Down set */
#define PWR_PDCRD_PD2_POS         (2U)
#define PWR_PDCRD_PD2_MSK         (0x1UL << PWR_PDCRD_PD2_POS)                 /*!< 0x00000004 */
#define PWR_PDCRD_PD2             PWR_PDCRD_PD2_MSK                            /*!< Pin PD2 Pull-Down set */
#define PWR_PDCRD_PD3_POS         (3U)
#define PWR_PDCRD_PD3_MSK         (0x1UL << PWR_PDCRD_PD3_POS)                 /*!< 0x00000008 */
#define PWR_PDCRD_PD3             PWR_PDCRD_PD3_MSK                            /*!< Pin PD3 Pull-Down set */
#define PWR_PDCRD_PD4_POS         (4U)
#define PWR_PDCRD_PD4_MSK         (0x1UL << PWR_PDCRD_PD4_POS)                 /*!< 0x00000010 */
#define PWR_PDCRD_PD4             PWR_PDCRD_PD4_MSK                            /*!< Pin PD4 Pull-Down set */
#define PWR_PDCRD_PD5_POS         (5U)
#define PWR_PDCRD_PD5_MSK         (0x1UL << PWR_PDCRD_PD5_POS)                 /*!< 0x00000020 */
#define PWR_PDCRD_PD5             PWR_PDCRD_PD5_MSK                            /*!< Pin PD5 Pull-Down set */
#define PWR_PDCRD_PD6_POS         (6U)
#define PWR_PDCRD_PD6_MSK         (0x1UL << PWR_PDCRD_PD6_POS)                 /*!< 0x00000040 */
#define PWR_PDCRD_PD6             PWR_PDCRD_PD6_MSK                            /*!< Pin PD6 Pull-Down set */
#define PWR_PDCRD_PD8_POS         (8U)
#define PWR_PDCRD_PD8_MSK         (0x1UL << PWR_PDCRD_PD8_POS)                 /*!< 0x00000100 */
#define PWR_PDCRD_PD8             PWR_PDCRD_PD8_MSK                            /*!< Pin PD8 Pull-Down set */
#define PWR_PDCRD_PD9_POS         (9U)
#define PWR_PDCRD_PD9_MSK         (0x1UL << PWR_PDCRD_PD9_POS)                 /*!< 0x00000200 */
#define PWR_PDCRD_PD9             PWR_PDCRD_PD9_MSK                            /*!< Pin PD9 Pull-Down set */
#define PWR_PDCRD_PD10_POS        (10U)
#define PWR_PDCRD_PD10_MSK        (0x1UL << PWR_PDCRD_PD10_POS)                /*!< 0x00000400 */
#define PWR_PDCRD_PD10            PWR_PDCRD_PD10_MSK                           /*!< Pin PD10 Pull-Down set */
#define PWR_PDCRD_PD11_POS        (11U)
#define PWR_PDCRD_PD11_MSK        (0x1UL << PWR_PDCRD_PD11_POS)                /*!< 0x00000800 */
#define PWR_PDCRD_PD11            PWR_PDCRD_PD11_MSK                           /*!< Pin PD11 Pull-Down set */
#define PWR_PDCRD_PD12_POS        (12U)
#define PWR_PDCRD_PD12_MSK        (0x1UL << PWR_PDCRD_PD12_POS)                /*!< 0x00001000 */
#define PWR_PDCRD_PD12            PWR_PDCRD_PD12_MSK                           /*!< Pin PD12 Pull-Down set */
#define PWR_PDCRD_PD13_POS        (13U)
#define PWR_PDCRD_PD13_MSK        (0x1UL << PWR_PDCRD_PD13_POS)                /*!< 0x00002000 */
#define PWR_PDCRD_PD13            PWR_PDCRD_PD13_MSK                           /*!< Pin PD13 Pull-Down set */
#define PWR_PDCRD_PD14_POS        (14U)
#define PWR_PDCRD_PD14_MSK        (0x1UL << PWR_PDCRD_PD14_POS)                /*!< 0x00004000 */
#define PWR_PDCRD_PD14            PWR_PDCRD_PD14_MSK                           /*!< Pin PD14 Pull-Down set */
#define PWR_PDCRD_PD15_POS        (15U)
#define PWR_PDCRD_PD15_MSK        (0x1UL << PWR_PDCRD_PD15_POS)                /*!< 0x00008000 */
#define PWR_PDCRD_PD15            PWR_PDCRD_PD15_MSK                           /*!< Pin PD15 Pull-Down set */
/********************  Bit definition for PWR_PUCRE register  *****************/
#define PWR_PUCRE_PU0_POS         (0U)
#define PWR_PUCRE_PU0_MSK         (0x1UL << PWR_PUCRE_PU0_POS)                 /*!< 0x00000001 */
#define PWR_PUCRE_PU0             PWR_PUCRE_PU0_MSK                            /*!< Pin PE0 Pull-Up set */
#define PWR_PUCRE_PU1_POS         (1U)
#define PWR_PUCRE_PU1_MSK         (0x1UL << PWR_PUCRE_PU1_POS)                 /*!< 0x00000002 */
#define PWR_PUCRE_PU1             PWR_PUCRE_PU1_MSK                            /*!< Pin PE1 Pull-Up set */
#define PWR_PUCRE_PU2_POS         (2U)
#define PWR_PUCRE_PU2_MSK         (0x1UL << PWR_PUCRE_PU2_POS)                 /*!< 0x00000004 */
#define PWR_PUCRE_PU2             PWR_PUCRE_PU2_MSK                            /*!< Pin PE2 Pull-Up set */
#define PWR_PUCRE_PU3_POS         (3U)
#define PWR_PUCRE_PU3_MSK         (0x1UL << PWR_PUCRE_PU3_POS)                 /*!< 0x00000008 */
#define PWR_PUCRE_PU3             PWR_PUCRE_PU3_MSK                            /*!< Pin PE3 Pull-Up set */
#define PWR_PUCRE_PU4_POS         (4U)
#define PWR_PUCRE_PU4_MSK         (0x1UL << PWR_PUCRE_PU4_POS)                 /*!< 0x00000010 */
#define PWR_PUCRE_PU4             PWR_PUCRE_PU4_MSK                            /*!< Pin PE4 Pull-Up set */
#define PWR_PUCRE_PD5_POS         (5U)
#define PWR_PUCRE_PD5_MSK         (0x1UL << PWR_PUCRE_PD5_POS)                 /*!< 0x00000020 */
#define PWR_PUCRE_PD5             PWR_PUCRE_PD5_MSK                            /*!< Pin PE5 Pull-Up set */
#define PWR_PUCRE_PD6_POS         (6U)
#define PWR_PUCRE_PD6_MSK         (0x1UL << PWR_PUCRE_PD6_POS)                 /*!< 0x00000040 */
#define PWR_PUCRE_PD6             PWR_PUCRE_PD6_MSK                            /*!< Pin PE6 Pull-Up set */
#define PWR_PUCRE_PD7_POS         (7U)
#define PWR_PUCRE_PD7_MSK         (0x1UL << PWR_PUCRE_PD7_POS)                 /*!< 0x00000080 */
#define PWR_PUCRE_PD7             PWR_PUCRE_PD7_MSK                            /*!< Pin PE7 Pull-Up set */
#define PWR_PUCRE_PD8_POS         (8U)
#define PWR_PUCRE_PD8_MSK         (0x1UL << PWR_PUCRE_PD8_POS)                 /*!< 0x00000100 */
#define PWR_PUCRE_PD8             PWR_PUCRE_PD8_MSK                            /*!< Pin PE8 Pull-Up set */
#define PWR_PUCRE_PD9_POS         (9U)
#define PWR_PUCRE_PD9_MSK         (0x1UL << PWR_PUCRE_PD9_POS)                 /*!< 0x00000200 */
#define PWR_PUCRE_PD9             PWR_PUCRE_PD9_MSK                            /*!< Pin PE9 Pull-Up set */
#define PWR_PUCRE_PD10_POS        (10U)
#define PWR_PUCRE_PD10_MSK        (0x1UL << PWR_PUCRE_PD10_POS)                /*!< 0x00000400 */
#define PWR_PUCRE_PD10            PWR_PUCRE_PD10_MSK                           /*!< Pin PE10 Pull-Up set */
#define PWR_PUCRE_PD11_POS        (11U)
#define PWR_PUCRE_PD11_MSK        (0x1UL << PWR_PUCRE_PD11_POS)                /*!< 0x00000800 */
#define PWR_PUCRE_PD11            PWR_PUCRE_PD11_MSK                           /*!< Pin PE11 Pull-Up set */
#define PWR_PUCRE_PD12_POS        (12U)
#define PWR_PUCRE_PD12_MSK        (0x1UL << PWR_PUCRE_PD12_POS)                /*!< 0x00001000 */
#define PWR_PUCRE_PD12            PWR_PUCRE_PD12_MSK                           /*!< Pin PE12 Pull-Up set */
#define PWR_PUCRE_PD13_POS        (13U)
#define PWR_PUCRE_PD13_MSK        (0x1UL << PWR_PUCRE_PD13_POS)                /*!< 0x00002000 */
#define PWR_PUCRE_PD13            PWR_PUCRE_PD13_MSK                           /*!< Pin PE13 Pull-Up set */
#define PWR_PUCRE_PD14_POS        (14U)
#define PWR_PUCRE_PD14_MSK        (0x1UL << PWR_PUCRE_PD14_POS)                /*!< 0x00004000 */
#define PWR_PUCRE_PD14            PWR_PUCRE_PD14_MSK                           /*!< Pin PE14 Pull-Up set */
#define PWR_PUCRE_PD15_POS        (15U)
#define PWR_PUCRE_PD15_MSK        (0x1UL << PWR_PUCRE_PD15_POS)                /*!< 0x00008000 */
#define PWR_PUCRE_PD15            PWR_PUCRE_PD15_MSK                           /*!< Pin PE15 Pull-Up set */

/********************  Bit definition for PWR_PDCRE register  *****************/
#define PWR_PDCRE_PD0_POS         (0U)
#define PWR_PDCRE_PD0_MSK         (0x1UL << PWR_PDCRE_PD0_POS)                 /*!< 0x00000001 */
#define PWR_PDCRE_PD0             PWR_PDCRE_PD0_MSK                            /*!< Pin PE0 Pull-Down set */
#define PWR_PDCRE_PD1_POS         (1U)
#define PWR_PDCRE_PD1_MSK         (0x1UL << PWR_PDCRE_PD1_POS)                 /*!< 0x00000002 */
#define PWR_PDCRE_PD1             PWR_PDCRE_PD1_MSK                            /*!< Pin PE1 Pull-Down set */
#define PWR_PDCRE_PD2_POS         (2U)
#define PWR_PDCRE_PD2_MSK         (0x1UL << PWR_PDCRE_PD2_POS)                 /*!< 0x00000004 */
#define PWR_PDCRE_PD2             PWR_PDCRE_PD2_MSK                            /*!< Pin PE2 Pull-Down set */
#define PWR_PDCRE_PD3_POS         (3U)
#define PWR_PDCRE_PD3_MSK         (0x1UL << PWR_PDCRE_PD3_POS)                 /*!< 0x00000008 */
#define PWR_PDCRE_PD3             PWR_PDCRE_PD3_MSK                            /*!< Pin PE3 Pull-Down set */
#define PWR_PDCRE_PD4_POS         (4U)
#define PWR_PDCRE_PD4_MSK         (0x1UL << PWR_PDCRE_PD4_POS)                 /*!< 0x00000010 */
#define PWR_PDCRE_PD4             PWR_PDCRE_PD4_MSK                            /*!< Pin PE4 Pull-Down set */
#define PWR_PDCRE_PD5_POS         (5U)
#define PWR_PDCRE_PD5_MSK         (0x1UL << PWR_PDCRE_PD5_POS)                 /*!< 0x00000020 */
#define PWR_PDCRE_PD5             PWR_PDCRE_PD5_MSK                            /*!< Pin PE5 Pull-Down set */
#define PWR_PDCRE_PD6_POS         (6U)
#define PWR_PDCRE_PD6_MSK         (0x1UL << PWR_PDCRE_PD6_POS)                 /*!< 0x00000040 */
#define PWR_PDCRE_PD6             PWR_PDCRE_PD6_MSK                            /*!< Pin PE6 Pull-Down set */
#define PWR_PDCRE_PD7_POS         (7U)
#define PWR_PDCRE_PD7_MSK         (0x1UL << PWR_PDCRE_PD7_POS)                 /*!< 0x00000080 */
#define PWR_PDCRE_PD7             PWR_PDCRE_PD7_MSK                            /*!< Pin PE7 Pull-Down set */
#define PWR_PDCRE_PD8_POS         (8U)
#define PWR_PDCRE_PD8_MSK         (0x1UL << PWR_PDCRE_PD8_POS)                 /*!< 0x00000100 */
#define PWR_PDCRE_PD8             PWR_PDCRE_PD8_MSK                            /*!< Pin PE8 Pull-Down set */
#define PWR_PDCRE_PD9_POS         (9U)
#define PWR_PDCRE_PD9_MSK         (0x1UL << PWR_PDCRE_PD9_POS)                 /*!< 0x00000200 */
#define PWR_PDCRE_PD9             PWR_PDCRE_PD9_MSK                            /*!< Pin PE9 Pull-Down set */
#define PWR_PDCRE_PD10_POS        (10U)
#define PWR_PDCRE_PD10_MSK        (0x1UL << PWR_PDCRE_PD10_POS)                /*!< 0x00000400 */
#define PWR_PDCRE_PD10            PWR_PDCRE_PD10_MSK                           /*!< Pin PE10 Pull-Down set */
#define PWR_PDCRE_PD11_POS        (11U)
#define PWR_PDCRE_PD11_MSK        (0x1UL << PWR_PDCRE_PD11_POS)                /*!< 0x00000800 */
#define PWR_PDCRE_PD11            PWR_PDCRE_PD11_MSK                           /*!< Pin PE11 Pull-Down set */
#define PWR_PDCRE_PD12_POS        (12U)
#define PWR_PDCRE_PD12_MSK        (0x1UL << PWR_PDCRE_PD12_POS)                /*!< 0x00001000 */
#define PWR_PDCRE_PD12            PWR_PDCRE_PD12_MSK                           /*!< Pin PE12 Pull-Down set */
#define PWR_PDCRE_PD13_POS        (13U)
#define PWR_PDCRE_PD13_MSK        (0x1UL << PWR_PDCRE_PD13_POS)                /*!< 0x00002000 */
#define PWR_PDCRE_PD13            PWR_PDCRE_PD13_MSK                           /*!< Pin PE13 Pull-Down set */
#define PWR_PDCRE_PD14_POS        (14U)
#define PWR_PDCRE_PD14_MSK        (0x1UL << PWR_PDCRE_PD14_POS)                /*!< 0x00004000 */
#define PWR_PDCRE_PD14            PWR_PDCRE_PD14_MSK                           /*!< Pin PE14 Pull-Down set */
#define PWR_PDCRE_PD15_POS        (15U)
#define PWR_PDCRE_PD15_MSK        (0x1UL << PWR_PDCRE_PD15_POS)                /*!< 0x00008000 */
#define PWR_PDCRE_PD15            PWR_PDCRE_PD15_MSK                           /*!< Pin PE15 Pull-Down set */

/********************  Bit definition for PWR_PUCRF register  *****************/
#define PWR_PUCRF_PU0_POS         (0U)
#define PWR_PUCRF_PU0_MSK         (0x1UL << PWR_PUCRF_PU0_POS)                 /*!< 0x00000001 */
#define PWR_PUCRF_PU0             PWR_PUCRF_PU0_MSK                            /*!< Pin PF0 Pull-Up set */
#define PWR_PUCRF_PU1_POS         (1U)
#define PWR_PUCRF_PU1_MSK         (0x1UL << PWR_PUCRF_PU1_POS)                 /*!< 0x00000002 */
#define PWR_PUCRF_PU1             PWR_PUCRF_PU1_MSK                            /*!< Pin PF1 Pull-Up set */
#define PWR_PUCRF_PU2_POS         (2U)
#define PWR_PUCRF_PU2_MSK         (0x1UL << PWR_PUCRF_PU2_POS)                 /*!< 0x00000004 */
#define PWR_PUCRF_PU2             PWR_PUCRF_PU2_MSK                            /*!< Pin PF2 Pull-Up set */
#define PWR_PUCRF_PU3_POS         (3U)
#define PWR_PUCRF_PU3_MSK         (0x1UL << PWR_PUCRF_PU3_POS)                 /*!< 0x00000008 */
#define PWR_PUCRF_PU3             PWR_PUCRF_PU3_MSK                            /*!< Pin PF3 Pull-Up set */
#define PWR_PUCRF_PU4_POS         (4U)
#define PWR_PUCRF_PU4_MSK         (0x1UL << PWR_PUCRF_PU4_POS)                 /*!< 0x00000010 */
#define PWR_PUCRF_PU4             PWR_PUCRF_PU4_MSK                            /*!< Pin PF4 Pull-Up set */
#define PWR_PUCRF_PD5_POS         (5U)
#define PWR_PUCRF_PD5_MSK         (0x1UL << PWR_PUCRF_PD5_POS)                 /*!< 0x00000020 */
#define PWR_PUCRF_PD5             PWR_PUCRF_PD5_MSK                            /*!< Pin PF5 Pull-Up set */
#define PWR_PUCRF_PD6_POS         (6U)
#define PWR_PUCRF_PD6_MSK         (0x1UL << PWR_PUCRF_PD6_POS)                 /*!< 0x00000040 */
#define PWR_PUCRF_PD6             PWR_PUCRF_PD6_MSK                            /*!< Pin PF6 Pull-Up set */
#define PWR_PUCRF_PD7_POS         (7U)
#define PWR_PUCRF_PD7_MSK         (0x1UL << PWR_PUCRF_PD7_POS)                 /*!< 0x00000080 */
#define PWR_PUCRF_PD7             PWR_PUCRF_PD7_MSK                            /*!< Pin PF7 Pull-Up set */
#define PWR_PUCRF_PD8_POS         (8U)
#define PWR_PUCRF_PD8_MSK         (0x1UL << PWR_PUCRF_PD8_POS)                 /*!< 0x00000100 */
#define PWR_PUCRF_PD8             PWR_PUCRF_PD8_MSK                            /*!< Pin PF8 Pull-Up set */
#define PWR_PUCRF_PD9_POS         (9U)
#define PWR_PUCRF_PD9_MSK         (0x1UL << PWR_PUCRF_PD9_POS)                 /*!< 0x00000200 */
#define PWR_PUCRF_PD9             PWR_PUCRF_PD9_MSK                            /*!< Pin PF9 Pull-Up set */
#define PWR_PUCRF_PD10_POS        (10U)
#define PWR_PUCRF_PD10_MSK        (0x1UL << PWR_PUCRF_PD10_POS)                /*!< 0x00000400 */
#define PWR_PUCRF_PD10            PWR_PUCRF_PD10_MSK                           /*!< Pin PF10 Pull-Up set */
#define PWR_PUCRF_PD11_POS        (11U)
#define PWR_PUCRF_PD11_MSK        (0x1UL << PWR_PUCRF_PD11_POS)                /*!< 0x00000800 */
#define PWR_PUCRF_PD11            PWR_PUCRF_PD11_MSK                           /*!< Pin PF11 Pull-Up set */
#define PWR_PUCRF_PD12_POS        (12U)
#define PWR_PUCRF_PD12_MSK        (0x1UL << PWR_PUCRF_PD12_POS)                /*!< 0x00001000 */
#define PWR_PUCRF_PD12            PWR_PUCRF_PD12_MSK                           /*!< Pin PF12 Pull-Up set */
#define PWR_PUCRF_PD13_POS        (13U)
#define PWR_PUCRF_PD13_MSK        (0x1UL << PWR_PUCRF_PD13_POS)                /*!< 0x00002000 */
#define PWR_PUCRF_PD13            PWR_PUCRF_PD13_MSK                           /*!< Pin PF13 Pull-Up set */

/********************  Bit definition for PWR_PDCRF register  *****************/
#define PWR_PDCRF_PD0_POS         (0U)
#define PWR_PDCRF_PD0_MSK         (0x1UL << PWR_PDCRF_PD0_POS)                 /*!< 0x00000001 */
#define PWR_PDCRF_PD0             PWR_PDCRF_PD0_MSK                            /*!< Pin PF0 Pull-Down set */
#define PWR_PDCRF_PD1_POS         (1U)
#define PWR_PDCRF_PD1_MSK         (0x1UL << PWR_PDCRF_PD1_POS)                 /*!< 0x00000002 */
#define PWR_PDCRF_PD1             PWR_PDCRF_PD1_MSK                            /*!< Pin PF1 Pull-Down set */
#define PWR_PDCRF_PD2_POS         (2U)
#define PWR_PDCRF_PD2_MSK         (0x1UL << PWR_PDCRF_PD2_POS)                 /*!< 0x00000004 */
#define PWR_PDCRF_PD2             PWR_PDCRF_PD2_MSK                            /*!< Pin PF2 Pull-Down set */
#define PWR_PDCRF_PD3_POS         (3U)
#define PWR_PDCRF_PD3_MSK         (0x1UL << PWR_PDCRF_PD3_POS)                 /*!< 0x00000008 */
#define PWR_PDCRF_PD3             PWR_PDCRF_PD3_MSK                            /*!< Pin PF3 Pull-Down set */
#define PWR_PDCRF_PD4_POS         (4U)
#define PWR_PDCRF_PD4_MSK         (0x1UL << PWR_PDCRF_PD4_POS)                 /*!< 0x00000010 */
#define PWR_PDCRF_PD4             PWR_PDCRF_PD4_MSK                            /*!< Pin PF4 Pull-Down set */
#define PWR_PDCRF_PD5_POS         (5U)
#define PWR_PDCRF_PD5_MSK         (0x1UL << PWR_PDCRF_PD5_POS)                 /*!< 0x00000020 */
#define PWR_PDCRF_PD5             PWR_PDCRF_PD5_MSK                            /*!< Pin PF5 Pull-Down set */
#define PWR_PDCRF_PD6_POS         (6U)
#define PWR_PDCRF_PD6_MSK         (0x1UL << PWR_PDCRF_PD6_POS)                 /*!< 0x00000040 */
#define PWR_PDCRF_PD6             PWR_PDCRF_PD6_MSK                            /*!< Pin PF6 Pull-Down set */
#define PWR_PDCRF_PD7_POS         (7U)
#define PWR_PDCRF_PD7_MSK         (0x1UL << PWR_PDCRF_PD7_POS)                 /*!< 0x00000080 */
#define PWR_PDCRF_PD7             PWR_PDCRF_PD7_MSK                            /*!< Pin PF7 Pull-Down set */
#define PWR_PDCRF_PD8_POS         (8U)
#define PWR_PDCRF_PD8_MSK         (0x1UL << PWR_PDCRF_PD8_POS)                 /*!< 0x00000100 */
#define PWR_PDCRF_PD8             PWR_PDCRF_PD8_MSK                            /*!< Pin PF8 Pull-Down set */
#define PWR_PDCRF_PD9_POS         (9U)
#define PWR_PDCRF_PD9_MSK         (0x1UL << PWR_PDCRF_PD9_POS)                 /*!< 0x00000200 */
#define PWR_PDCRF_PD9             PWR_PDCRF_PD9_MSK                            /*!< Pin PF9 Pull-Down set */
#define PWR_PDCRF_PD10_POS        (10U)
#define PWR_PDCRF_PD10_MSK        (0x1UL << PWR_PDCRF_PD10_POS)                /*!< 0x00000400 */
#define PWR_PDCRF_PD10            PWR_PDCRF_PD10_MSK                           /*!< Pin PF10 Pull-Down set */
#define PWR_PDCRF_PD11_POS        (11U)
#define PWR_PDCRF_PD11_MSK        (0x1UL << PWR_PDCRF_PD11_POS)                /*!< 0x00000800 */
#define PWR_PDCRF_PD11            PWR_PDCRF_PD11_MSK                           /*!< Pin PF11 Pull-Down set */
#define PWR_PDCRF_PD12_POS        (12U)
#define PWR_PDCRF_PD12_MSK        (0x1UL << PWR_PDCRF_PD12_POS)                /*!< 0x00001000 */
#define PWR_PDCRF_PD12            PWR_PDCRF_PD12_MSK                           /*!< Pin PF12 Pull-Down set */
#define PWR_PDCRF_PD13_POS        (13U)
#define PWR_PDCRF_PD13_MSK        (0x1UL << PWR_PDCRF_PD13_POS)                /*!< 0x00002000 */
#define PWR_PDCRF_PD13            PWR_PDCRF_PD13_MSK                           /*!< Pin PF13 Pull-Down set */





//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- RESET AND CLOCK CONTROL REGISTERS --------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief Reset and Clock Control
  */
typedef struct
{
  volatile uint32 CR;          /*!< RCC Clock Sources Control Register,                                     Address offset: 0x00 */
  volatile uint32 ICSCR;       /*!< RCC Internal Clock Sources Calibration Register,                        Address offset: 0x04 */
  volatile uint32 CFGR;        /*!< RCC Regulated Domain Clocks Configuration Register,                     Address offset: 0x08 */
  volatile uint32 PLLCFGR;     /*!< RCC System PLL configuration Register,                                  Address offset: 0x0C */
  volatile uint32 RESERVED0;   /*!< Reserved,                                                               Address offset: 0x10 */
  volatile uint32 CRRCR;       /*!< RCC Clock Configuration Register,                                       Address offset: 0x14 */
  volatile uint32 CIER;        /*!< RCC Clock Interrupt Enable Register,                                    Address offset: 0x18 */
  volatile uint32 CIFR;        /*!< RCC Clock Interrupt Flag Register,                                      Address offset: 0x1C */
  volatile uint32 CICR;        /*!< RCC Clock Interrupt Clear Register,                                     Address offset: 0x20 */
  volatile uint32 IOPRSTR;     /*!< RCC IO port reset register,                                             Address offset: 0x24 */
  volatile uint32 AHBRSTR;     /*!< RCC AHB peripherals reset register,                                     Address offset: 0x28 */
  volatile uint32 APBRSTR1;    /*!< RCC APB peripherals reset register 1,                                   Address offset: 0x2C */
  volatile uint32 APBRSTR2;    /*!< RCC APB peripherals reset register 2,                                   Address offset: 0x30 */
  volatile uint32 IOPENR;      /*!< RCC IO port enable register,                                            Address offset: 0x34 */
  volatile uint32 AHBENR;      /*!< RCC AHB peripherals clock enable register,                              Address offset: 0x38 */
  volatile uint32 APBENR1;     /*!< RCC APB peripherals clock enable register1,                             Address offset: 0x3C */
  volatile uint32 APBENR2;     /*!< RCC APB peripherals clock enable register2,                             Address offset: 0x40 */
  volatile uint32 IOPSMENR;    /*!< RCC IO port clocks enable in sleep mode register,                       Address offset: 0x44 */
  volatile uint32 AHBSMENR;    /*!< RCC AHB peripheral clocks enable in sleep mode register,                Address offset: 0x48 */
  volatile uint32 APBSMENR1;   /*!< RCC APB peripheral clocks enable in sleep mode register1,               Address offset: 0x4C */
  volatile uint32 APBSMENR2;   /*!< RCC APB peripheral clocks enable in sleep mode register2,               Address offset: 0x50 */
  volatile uint32 CCIPR;       /*!< RCC Peripherals Independent Clocks Configuration Register,              Address offset: 0x54 */
  volatile uint32 CCIPR2;      /*!< RCC Peripherals Independent Clocks Configuration Register2,             Address offset: 0x58 */
  volatile uint32 BDCR;        /*!< RCC Backup Domain Control Register,                                     Address offset: 0x5C */
  volatile uint32 CSR;         /*!< RCC Unregulated Domain Clock Control and Status Register,               Address offset: 0x60 */
} RCC_TYPEDEF;

#define RCC                 (*(RCC_TYPEDEF *) RCC_BASE)

#define RCC_AHBRSTR_FORCE_RESET            ((uint32)0x00051003)     //Except Flash
#define RCC_APBRSTR1_FORCE_RESET           ((uint32)0xF7FFF3B7U)     //Except debugger
#define RCC_APBRSTR2_FORCE_RESET           ((uint32)0x0017D801)
#define RCC_IOPRSTR_FORCE_RESET            ((uint32)0x0000003F)

#define RCC_AHBRSTR_RELEASE_RESET           ((uint32)0x00000000)
#define RCC_APBRSTR1_RELEASE_RESET          ((uint32)0x00000000)
#define RCC_APBRSTR2_RELEASE_RESET          ((uint32)0x00000000)
#define RCC_IOPRSTR_RELEASE_RESET           ((uint32)0x00000000)

#define RCC_AHBPERIPH_CLK_DISABLE           ((uint32)0x00000100)    //Except Flash
#define RCC_APBENR1_CLK_DISABLE             ((uint32)0x18000000)    //Except debugger, Pwr
#define RCC_APBENR2_CLK_DISABLE             ((uint32)0x00000001)    //Except SYSCFG
#define RCC_IOPENR_CLK_DISABLE              ((uint32)0x00000000)

#define RCC_CICR_CLEAR_ALL_INTERRUPTS       ((uint32)0xFFFFFFFFu)

//! SW configuration
#define  RCC_CFGR_SW_HSI                     ((uint32)0x00000000)        //! HSI selected as system clock
#define  RCC_CFGR_SW_HSE                     ((uint32)0x00000001)        //! HSE selected as system clock
#define  RCC_CFGR_SW_PLL                     ((uint32)0x00000002)        //! PLL selected as system clock

#define  RCC_CFGR_SWS_HSI                    ((uint32)0x00000000)        //! HSI oscillator used as system clock
#define  RCC_CFGR_SWS_PLL                    ((uint32)0x00000010)        //! PLL used as system clock

#define RCC_PLLSOURCE_NONE             0x00000000U             /*!< No clock selected as PLL entry clock source  */
#define RCC_PLLSOURCE_HSI              RCC_PLLCFGR_PLLSRC_HSI  /*!< HSI clock selected as PLL entry clock source */
#define RCC_PLLSOURCE_HSE              RCC_PLLCFGR_PLLSRC_HSE  /*!< HSE clock selected as PLL entry clock source */

// RCC reset configuration
#define  RCC_CR_INIT_CONFIG              ((uint32)0x00000500)
#define  RCC_CFGR_INIT_CONFIG            ((uint32)0x00000000)
#define  RCC_PLLCFGR_INIT_CONFIG         ((uint32)0x00001000)

#define  RCC_CFGR3_INIT_CONFIG           ((uint32)0x00000000)
#define  RCC_CIR_ALL_INTERRUPTS_DISABLE  ((uint32)0x00000000)

#define RCC_HCLK_DIV1                  0x00000000U                                           /*!< HCLK not divided */
#define RCC_HCLK_DIV2                  RCC_CFGR_PPRE_2                                       /*!< HCLK divided by 2 */
#define RCC_HCLK_DIV4                  (RCC_CFGR_PPRE_2 | RCC_CFGR_PPRE_0)                   /*!< HCLK divided by 4 */
#define RCC_HCLK_DIV8                  (RCC_CFGR_PPRE_2 | RCC_CFGR_PPRE_1)                   /*!< HCLK divided by 8 */
#define RCC_HCLK_DIV16                 (RCC_CFGR_PPRE_2 | RCC_CFGR_PPRE_1 | RCC_CFGR_PPRE_0) /*!< HCLK divided by 16 */

#define RCC_HSI_DIV1                   0x00000000U                                        /*!< HSI clock is not divided */
#define RCC_HSI_DIV2                   RCC_CR_HSIDIV_0                                    /*!< HSI clock is divided by 2 */
#define RCC_HSI_DIV4                   RCC_CR_HSIDIV_1                                    /*!< HSI clock is divided by 4 */
#define RCC_HSI_DIV8                   (RCC_CR_HSIDIV_1|RCC_CR_HSIDIV_0)                  /*!< HSI clock is divided by 8 */
#define RCC_HSI_DIV16                  RCC_CR_HSIDIV_2                                    /*!< HSI clock is divided by 16 */
#define RCC_HSI_DIV32                  (RCC_CR_HSIDIV_2|RCC_CR_HSIDIV_0)                  /*!< HSI clock is divided by 32 */
#define RCC_HSI_DIV64                  (RCC_CR_HSIDIV_2|RCC_CR_HSIDIV_1)                  /*!< HSI clock is divided by 64 */
#define RCC_HSI_DIV128                 (RCC_CR_HSIDIV_2|RCC_CR_HSIDIV_1|RCC_CR_HSIDIV_0)  /*!< HSI clock is divided by 128 */

/********************  Bit definition for RCC_CR register  *****************/
#define RCC_CR_HSION_POS                 (8U)
#define RCC_CR_HSION_MSK                 (0x1UL << RCC_CR_HSION_POS)           /*!< 0x00000100 */
#define RCC_CR_HSION                     RCC_CR_HSION_MSK                      /*!< Internal High Speed clock enable */
#define RCC_CR_HSIKERON_POS              (9U)
#define RCC_CR_HSIKERON_MSK              (0x1UL << RCC_CR_HSIKERON_POS)        /*!< 0x00000200 */
#define RCC_CR_HSIKERON                  RCC_CR_HSIKERON_MSK                   /*!< Internal High Speed clock enable for some IPs Kernel */
#define RCC_CR_HSIRDY_POS                (10U)
#define RCC_CR_HSIRDY_MSK                (0x1UL << RCC_CR_HSIRDY_POS)          /*!< 0x00000400 */
#define RCC_CR_HSIRDY                    RCC_CR_HSIRDY_MSK                     /*!< Internal High Speed clock ready flag */
#define RCC_CR_HSIDIV_POS                (11U)
#define RCC_CR_HSIDIV_MSK                (0x7UL << RCC_CR_HSIDIV_POS)          /*!< 0x00003800 */
#define RCC_CR_HSIDIV                    RCC_CR_HSIDIV_MSK                     /*!< HSIDIV[13:11] Internal High Speed clock division factor */
#define RCC_CR_HSIDIV_0                  (0x1UL << RCC_CR_HSIDIV_POS)          /*!< 0x00000800 */
#define RCC_CR_HSIDIV_1                  (0x2UL << RCC_CR_HSIDIV_POS)          /*!< 0x00001000 */
#define RCC_CR_HSIDIV_2                  (0x4UL << RCC_CR_HSIDIV_POS)          /*!< 0x00002000 */
#define RCC_CR_HSEON_POS                 (16U)
#define RCC_CR_HSEON_MSK                 (0x1UL << RCC_CR_HSEON_POS)           /*!< 0x00010000 */
#define RCC_CR_HSEON                     RCC_CR_HSEON_MSK                      /*!< External High Speed clock enable */
#define RCC_CR_HSERDY_POS                (17U)
#define RCC_CR_HSERDY_MSK                (0x1UL << RCC_CR_HSERDY_POS)          /*!< 0x00020000 */
#define RCC_CR_HSERDY                    RCC_CR_HSERDY_MSK                     /*!< External High Speed clock ready */
#define RCC_CR_HSEBYP_POS                (18U)
#define RCC_CR_HSEBYP_MSK                (0x1UL << RCC_CR_HSEBYP_POS)          /*!< 0x00040000 */
#define RCC_CR_HSEBYP                    RCC_CR_HSEBYP_MSK                     /*!< External High Speed clock Bypass */
#define RCC_CR_CSSON_POS                 (19U)
#define RCC_CR_CSSON_MSK                 (0x1UL << RCC_CR_CSSON_POS)           /*!< 0x00080000 */
#define RCC_CR_CSSON                     RCC_CR_CSSON_MSK                      /*!< HSE Clock Security System enable */

#define RCC_CR_HSI48ON_POS                (22U)
#define RCC_CR_HSI48ON_MSK                (0x1UL << RCC_CR_HSI48ON_POS)           /*!< 0x004000000 */
#define RCC_CR_HSI48ON                    RCC_CR_HSI48ON_MSK                      /*!< RC48 clock enable */
#define RCC_CR_HSI48RDY_POS               (23U)
#define RCC_CR_HSI48RDY_MSK               (0x1UL << RCC_CR_HSI48RDY_POS)           /*!< 0x00800000 */
#define RCC_CR_HSI48RDY                   RCC_CR_HSI48RDY_MSK                      /*!< RC48 clock ready */
#define RCC_CR_PLLON_POS                 (24U)
#define RCC_CR_PLLON_MSK                 (0x1UL << RCC_CR_PLLON_POS)           /*!< 0x01000000 */
#define RCC_CR_PLLON                     RCC_CR_PLLON_MSK                      /*!< System PLL clock enable */
#define RCC_CR_PLLRDY_POS                (25U)
#define RCC_CR_PLLRDY_MSK                (0x1UL << RCC_CR_PLLRDY_POS)          /*!< 0x02000000 */
#define RCC_CR_PLLRDY                    RCC_CR_PLLRDY_MSK                     /*!< System PLL clock ready */

/********************  Bit definition for RCC_ICSCR register  ***************/
/*!< HSICAL configuration */
#define RCC_ICSCR_HSICAL_POS             (0U)
#define RCC_ICSCR_HSICAL_MSK             (0xFFUL << RCC_ICSCR_HSICAL_POS)      /*!< 0x000000FF */
#define RCC_ICSCR_HSICAL                 RCC_ICSCR_HSICAL_MSK                  /*!< HSICAL[7:0] bits */
#define RCC_ICSCR_HSICAL_0               (0x01UL << RCC_ICSCR_HSICAL_POS)      /*!< 0x00000001 */
#define RCC_ICSCR_HSICAL_1               (0x02UL << RCC_ICSCR_HSICAL_POS)      /*!< 0x00000002 */
#define RCC_ICSCR_HSICAL_2               (0x04UL << RCC_ICSCR_HSICAL_POS)      /*!< 0x00000004 */
#define RCC_ICSCR_HSICAL_3               (0x08UL << RCC_ICSCR_HSICAL_POS)      /*!< 0x00000008 */
#define RCC_ICSCR_HSICAL_4               (0x10UL << RCC_ICSCR_HSICAL_POS)      /*!< 0x00000010 */
#define RCC_ICSCR_HSICAL_5               (0x20UL << RCC_ICSCR_HSICAL_POS)      /*!< 0x00000020 */
#define RCC_ICSCR_HSICAL_6               (0x40UL << RCC_ICSCR_HSICAL_POS)      /*!< 0x00000040 */
#define RCC_ICSCR_HSICAL_7               (0x80UL << RCC_ICSCR_HSICAL_POS)      /*!< 0x00000080 */

/*!< HSITRIM configuration */
#define RCC_ICSCR_HSITRIM_POS            (8U)
#define RCC_ICSCR_HSITRIM_MSK            (0x7FUL << RCC_ICSCR_HSITRIM_POS)     /*!< 0x00007F00 */
#define RCC_ICSCR_HSITRIM                RCC_ICSCR_HSITRIM_MSK                 /*!< HSITRIM[14:8] bits */
#define RCC_ICSCR_HSITRIM_0              (0x01UL << RCC_ICSCR_HSITRIM_POS)     /*!< 0x00000100 */
#define RCC_ICSCR_HSITRIM_1              (0x02UL << RCC_ICSCR_HSITRIM_POS)     /*!< 0x00000200 */
#define RCC_ICSCR_HSITRIM_2              (0x04UL << RCC_ICSCR_HSITRIM_POS)     /*!< 0x00000400 */
#define RCC_ICSCR_HSITRIM_3              (0x08UL << RCC_ICSCR_HSITRIM_POS)     /*!< 0x00000800 */
#define RCC_ICSCR_HSITRIM_4              (0x10UL << RCC_ICSCR_HSITRIM_POS)     /*!< 0x00001000 */
#define RCC_ICSCR_HSITRIM_5              (0x20UL << RCC_ICSCR_HSITRIM_POS)     /*!< 0x00002000 */
#define RCC_ICSCR_HSITRIM_6              (0x40UL << RCC_ICSCR_HSITRIM_POS)     /*!< 0x00004000 */

/********************  Bit definition for RCC_CFGR register  ***************/
/*!< SW configuration */
#define RCC_CFGR_SW_POS                (0U)
#define RCC_CFGR_SW_MSK                (0x7UL << RCC_CFGR_SW_POS)              /*!< 0x00000007 */
#define RCC_CFGR_SW                    RCC_CFGR_SW_MSK                         /*!< SW[2:0] bits (System clock Switch) */
#define RCC_CFGR_SW_0                  (0x1UL << RCC_CFGR_SW_POS)              /*!< 0x00000001 */
#define RCC_CFGR_SW_1                  (0x2UL << RCC_CFGR_SW_POS)              /*!< 0x00000002 */
#define RCC_CFGR_SW_2                  (0x4UL << RCC_CFGR_SW_POS)              /*!< 0x00000004 */

/*!< SWS configuration */
#define RCC_CFGR_SWS_POS               (3U)
#define RCC_CFGR_SWS_MSK               (0x7UL << RCC_CFGR_SWS_POS)             /*!< 0x00000038 */
#define RCC_CFGR_SWS                   RCC_CFGR_SWS_MSK                        /*!< SWS[2:0] bits (System Clock Switch Status) */
#define RCC_CFGR_SWS_0                 (0x1UL << RCC_CFGR_SWS_POS)             /*!< 0x00000008 */
#define RCC_CFGR_SWS_1                 (0x2UL << RCC_CFGR_SWS_POS)             /*!< 0x00000010 */
#define RCC_CFGR_SWS_2                 (0x4UL << RCC_CFGR_SWS_POS)             /*!< 0x00000020 */

/*!< HPRE configuration */
#define RCC_CFGR_HPRE_POS              (8U)
#define RCC_CFGR_HPRE_MSK              (0xFUL << RCC_CFGR_HPRE_POS)            /*!< 0x00000F00 */
#define RCC_CFGR_HPRE                  RCC_CFGR_HPRE_MSK                       /*!< HPRE[3:0] bits (AHB prescaler) */
#define RCC_CFGR_HPRE_0                (0x1UL << RCC_CFGR_HPRE_POS)            /*!< 0x00000100 */
#define RCC_CFGR_HPRE_1                (0x2UL << RCC_CFGR_HPRE_POS)            /*!< 0x00000200 */
#define RCC_CFGR_HPRE_2                (0x4UL << RCC_CFGR_HPRE_POS)            /*!< 0x00000400 */
#define RCC_CFGR_HPRE_3                (0x8UL << RCC_CFGR_HPRE_POS)            /*!< 0x00000800 */

/*!< PPRE configuration */
#define RCC_CFGR_PPRE_POS              (12U)
#define RCC_CFGR_PPRE_MSK              (0x7UL << RCC_CFGR_PPRE_POS)            /*!< 0x00007000 */
#define RCC_CFGR_PPRE                  RCC_CFGR_PPRE_MSK                       /*!< PRE1[2:0] bits (APB prescaler) */
#define RCC_CFGR_PPRE_0                (0x1UL << RCC_CFGR_PPRE_POS)            /*!< 0x00001000 */
#define RCC_CFGR_PPRE_1                (0x2UL << RCC_CFGR_PPRE_POS)            /*!< 0x00002000 */
#define RCC_CFGR_PPRE_2                (0x4UL << RCC_CFGR_PPRE_POS)            /*!< 0x00004000 */

/*!< MCO2SEL configuration */
#define RCC_CFGR_MCO2SEL_POS            (16U)
#define RCC_CFGR_MCO2SEL_MSK            (0xFUL << RCC_CFGR_MCO2SEL_POS)          /*!< 0x000F0000 */
#define RCC_CFGR_MCO2SEL                RCC_CFGR_MCO2SEL_MSK                     /*!< MCO2SEL [3:0] bits (Clock output selection) */
#define RCC_CFGR_MCO2SEL_0              (0x1UL << RCC_CFGR_MCO2SEL_POS)          /*!< 0x00010000 */
#define RCC_CFGR_MCO2SEL_1              (0x2UL << RCC_CFGR_MCO2SEL_POS)          /*!< 0x00020000 */
#define RCC_CFGR_MCO2SEL_2              (0x4UL << RCC_CFGR_MCO2SEL_POS)          /*!< 0x00040000 */
#define RCC_CFGR_MCO2SEL_3              (0x8UL << RCC_CFGR_MCO2SEL_POS)          /*!< 0x00080000 */

/*!< MCO2 Prescaler configuration */
#define RCC_CFGR_MCO2PRE_POS            (20U)
#define RCC_CFGR_MCO2PRE_MSK            (0xFUL << RCC_CFGR_MCO2PRE_POS)          /*!< 0x00F00000 */
#define RCC_CFGR_MCO2PRE                RCC_CFGR_MCO2PRE_MSK                     /*!< MCO2 prescaler [3:0] */
#define RCC_CFGR_MCO2PRE_0              (0x1UL << RCC_CFGR_MCO2PRE_POS)          /*!< 0x00100000 */
#define RCC_CFGR_MCO2PRE_1              (0x2UL << RCC_CFGR_MCO2PRE_POS)          /*!< 0x00200000 */
#define RCC_CFGR_MCO2PRE_2              (0x4UL << RCC_CFGR_MCO2PRE_POS)          /*!< 0x00400000 */
#define RCC_CFGR_MCO2PRE_3              (0x8UL << RCC_CFGR_MCO2PRE_POS)          /*!< 0x00800000 */

/*!< MCOSEL configuration */
#define RCC_CFGR_MCOSEL_POS            (24U)
#define RCC_CFGR_MCOSEL_MSK            (0xFUL << RCC_CFGR_MCOSEL_POS)          /*!< 0x0F000000 */
#define RCC_CFGR_MCOSEL                RCC_CFGR_MCOSEL_MSK                     /*!< MCOSEL [2:0] bits (Clock output selection) */
#define RCC_CFGR_MCOSEL_0              (0x1UL << RCC_CFGR_MCOSEL_POS)          /*!< 0x01000000 */
#define RCC_CFGR_MCOSEL_1              (0x2UL << RCC_CFGR_MCOSEL_POS)          /*!< 0x02000000 */
#define RCC_CFGR_MCOSEL_2              (0x4UL << RCC_CFGR_MCOSEL_POS)          /*!< 0x04000000 */
#define RCC_CFGR_MCOSEL_3              (0x8UL << RCC_CFGR_MCOSEL_POS)          /*!< 0x08000000 */

/*!< MCO Prescaler configuration */
#define RCC_CFGR_MCOPRE_POS            (28U)
#define RCC_CFGR_MCOPRE_MSK            (0xFUL << RCC_CFGR_MCOPRE_POS)          /*!< 0xF0000000 */
#define RCC_CFGR_MCOPRE                RCC_CFGR_MCOPRE_MSK                     /*!< MCO prescaler [2:0] */
#define RCC_CFGR_MCOPRE_0              (0x1UL << RCC_CFGR_MCOPRE_POS)          /*!< 0x10000000 */
#define RCC_CFGR_MCOPRE_1              (0x2UL << RCC_CFGR_MCOPRE_POS)          /*!< 0x20000000 */
#define RCC_CFGR_MCOPRE_2              (0x4UL << RCC_CFGR_MCOPRE_POS)          /*!< 0x40000000 */
#define RCC_CFGR_MCOPRE_3              (0x8UL << RCC_CFGR_MCOPRE_POS)          /*!< 0x80000000 */

/********************  Bit definition for RCC_PLLCFGR register  ***************/
#define RCC_PLLCFGR_PLLSRC_POS           (0U)
#define RCC_PLLCFGR_PLLSRC_MSK           (0x3UL << RCC_PLLCFGR_PLLSRC_POS)     /*!< 0x00000003 */
#define RCC_PLLCFGR_PLLSRC               RCC_PLLCFGR_PLLSRC_MSK
#define RCC_PLLCFGR_PLLSRC_0             (0x1UL << RCC_PLLCFGR_PLLSRC_POS)     /*!< 0x00000001 */
#define RCC_PLLCFGR_PLLSRC_1             (0x2UL << RCC_PLLCFGR_PLLSRC_POS)     /*!< 0x00000002 */

#define RCC_PLLCFGR_PLLSRC_NONE          (0x00000000UL)                        /*!< No clock sent to PLL      */
#define RCC_PLLCFGR_PLLSRC_HSI_POS       (1U)
#define RCC_PLLCFGR_PLLSRC_HSI_MSK       (0x1UL << RCC_PLLCFGR_PLLSRC_HSI_POS) /*!< 0x00000002 */
#define RCC_PLLCFGR_PLLSRC_HSI           RCC_PLLCFGR_PLLSRC_HSI_MSK            /*!< HSI source clock selected */
#define RCC_PLLCFGR_PLLSRC_HSE_POS       (0U)
#define RCC_PLLCFGR_PLLSRC_HSE_MSK       (0x3UL << RCC_PLLCFGR_PLLSRC_HSE_POS) /*!< 0x00000003 */
#define RCC_PLLCFGR_PLLSRC_HSE           RCC_PLLCFGR_PLLSRC_HSE_MSK            /*!< HSE source clock selected */

#define RCC_PLLCFGR_PLLM_POS             (4U)
#define RCC_PLLCFGR_PLLM_MSK             (0x7UL << RCC_PLLCFGR_PLLM_POS)       /*!< 0x00000070 */
#define RCC_PLLCFGR_PLLM                 RCC_PLLCFGR_PLLM_MSK
#define RCC_PLLCFGR_PLLM_0               (0x1UL << RCC_PLLCFGR_PLLM_POS)       /*!< 0x00000010 */
#define RCC_PLLCFGR_PLLM_1               (0x2UL << RCC_PLLCFGR_PLLM_POS)       /*!< 0x00000020 */
#define RCC_PLLCFGR_PLLM_2               (0x4UL << RCC_PLLCFGR_PLLM_POS)       /*!< 0x00000040 */

#define RCC_PLLM_DIV1                  0x00000000U                                                    /*!< PLLM division factor = 8  */
#define RCC_PLLM_DIV2                  RCC_PLLCFGR_PLLM_0                                             /*!< PLLM division factor = 2  */
#define RCC_PLLM_DIV3                  RCC_PLLCFGR_PLLM_1                                             /*!< PLLM division factor = 3  */
#define RCC_PLLM_DIV4                  (RCC_PLLCFGR_PLLM_1 | RCC_PLLCFGR_PLLM_0)                      /*!< PLLM division factor = 4  */
#define RCC_PLLM_DIV5                  RCC_PLLCFGR_PLLM_2                                             /*!< PLLM division factor = 5  */
#define RCC_PLLM_DIV6                  (RCC_PLLCFGR_PLLM_2 | RCC_PLLCFGR_PLLM_0)                      /*!< PLLM division factor = 6  */
#define RCC_PLLM_DIV7                  (RCC_PLLCFGR_PLLM_2 | RCC_PLLCFGR_PLLM_1)                      /*!< PLLM division factor = 7  */
#define RCC_PLLM_DIV8                  (RCC_PLLCFGR_PLLM_2 | RCC_PLLCFGR_PLLM_1| RCC_PLLCFGR_PLLM_0)  /*!< PLLM division factor = 8  */

#define RCC_PLLCFGR_PLLN_POS             (8U)
#define RCC_PLLCFGR_PLLN_MSK             (0x7FUL << RCC_PLLCFGR_PLLN_POS)      /*!< 0x00007F00 */
#define RCC_PLLCFGR_PLLN                 RCC_PLLCFGR_PLLN_MSK
#define RCC_PLLCFGR_PLLN_0               (0x01UL << RCC_PLLCFGR_PLLN_POS)      /*!< 0x00000100 */
#define RCC_PLLCFGR_PLLN_1               (0x02UL << RCC_PLLCFGR_PLLN_POS)      /*!< 0x00000200 */
#define RCC_PLLCFGR_PLLN_2               (0x04UL << RCC_PLLCFGR_PLLN_POS)      /*!< 0x00000400 */
#define RCC_PLLCFGR_PLLN_3               (0x08UL << RCC_PLLCFGR_PLLN_POS)      /*!< 0x00000800 */
#define RCC_PLLCFGR_PLLN_4               (0x10UL << RCC_PLLCFGR_PLLN_POS)      /*!< 0x00001000 */
#define RCC_PLLCFGR_PLLN_5               (0x20UL << RCC_PLLCFGR_PLLN_POS)      /*!< 0x00002000 */
#define RCC_PLLCFGR_PLLN_6               (0x40UL << RCC_PLLCFGR_PLLN_POS)      /*!< 0x00004000 */

#define RCC_PLLCFGR_PLLPEN_POS           (16U)
#define RCC_PLLCFGR_PLLPEN_MSK           (0x1UL << RCC_PLLCFGR_PLLPEN_POS)     /*!< 0x00010000 */
#define RCC_PLLCFGR_PLLPEN               RCC_PLLCFGR_PLLPEN_MSK

#define RCC_PLLCFGR_PLLP_POS              (17U)
#define RCC_PLLCFGR_PLLP_MSK              (0x1FUL << RCC_PLLCFGR_PLLP_POS)     /*!< 0x003E0000 */
#define RCC_PLLCFGR_PLLP                  RCC_PLLCFGR_PLLP_MSK
#define RCC_PLLCFGR_PLLP_0                (0x01UL << RCC_PLLCFGR_PLLP_POS)     /*!< 0x00020000 */
#define RCC_PLLCFGR_PLLP_1                (0x02UL << RCC_PLLCFGR_PLLP_POS)     /*!< 0x00040000 */
#define RCC_PLLCFGR_PLLP_2                (0x04UL << RCC_PLLCFGR_PLLP_POS)     /*!< 0x00080000 */
#define RCC_PLLCFGR_PLLP_3                (0x08UL << RCC_PLLCFGR_PLLP_POS)     /*!< 0x00100000 */
#define RCC_PLLCFGR_PLLP_4                (0x10UL << RCC_PLLCFGR_PLLP_POS)     /*!< 0x00200000 */

#define RCC_PLLP_DIV2                  RCC_PLLCFGR_PLLP_0                                                                                       /*!< PLLP division factor = 2  */
#define RCC_PLLP_DIV3                  RCC_PLLCFGR_PLLP_1                                                                                       /*!< PLLP division factor = 3  */
#define RCC_PLLP_DIV4                  (RCC_PLLCFGR_PLLP_1 | RCC_PLLCFGR_PLLP_0)                                                                /*!< PLLP division factor = 4  */
#define RCC_PLLP_DIV5                  RCC_PLLCFGR_PLLP_2                                                                                       /*!< PLLP division factor = 5  */
#define RCC_PLLP_DIV6                  (RCC_PLLCFGR_PLLP_2 | RCC_PLLCFGR_PLLP_0)                                                                /*!< PLLP division factor = 6  */
#define RCC_PLLP_DIV7                  (RCC_PLLCFGR_PLLP_2 | RCC_PLLCFGR_PLLP_1)                                                                /*!< PLLP division factor = 7  */
#define RCC_PLLP_DIV8                  (RCC_PLLCFGR_PLLP_2 | RCC_PLLCFGR_PLLP_1 | RCC_PLLCFGR_PLLP_0)                                           /*!< PLLP division factor = 8  */
#define RCC_PLLP_DIV9                  RCC_PLLCFGR_PLLP_3                                                                                       /*!< PLLP division factor = 9  */
#define RCC_PLLP_DIV10                 (RCC_PLLCFGR_PLLP_3 | RCC_PLLCFGR_PLLP_0)                                                                /*!< PLLP division factor = 10 */
#define RCC_PLLP_DIV11                 (RCC_PLLCFGR_PLLP_3 | RCC_PLLCFGR_PLLP_1)                                                                /*!< PLLP division factor = 11 */
#define RCC_PLLP_DIV12                 (RCC_PLLCFGR_PLLP_3 | RCC_PLLCFGR_PLLP_1 | RCC_PLLCFGR_PLLP_0)                                           /*!< PLLP division factor = 12 */
#define RCC_PLLP_DIV13                 (RCC_PLLCFGR_PLLP_3 | RCC_PLLCFGR_PLLP_2)                                                                /*!< PLLP division factor = 13 */
#define RCC_PLLP_DIV14                 (RCC_PLLCFGR_PLLP_3 | RCC_PLLCFGR_PLLP_2 | RCC_PLLCFGR_PLLP_0)                                           /*!< PLLP division factor = 14 */
#define RCC_PLLP_DIV15                 (RCC_PLLCFGR_PLLP_3 | RCC_PLLCFGR_PLLP_2 | RCC_PLLCFGR_PLLP_1)                                           /*!< PLLP division factor = 15 */
#define RCC_PLLP_DIV16                 (RCC_PLLCFGR_PLLP_3 | RCC_PLLCFGR_PLLP_2 | RCC_PLLCFGR_PLLP_1 | RCC_PLLCFGR_PLLP_0)                      /*!< PLLP division factor = 16 */
#define RCC_PLLP_DIV17                 RCC_PLLCFGR_PLLP_4                                                                                       /*!< PLLP division factor = 17 */
#define RCC_PLLP_DIV18                 (RCC_PLLCFGR_PLLP_4 | RCC_PLLCFGR_PLLP_0)                                                                /*!< PLLP division factor = 18 */
#define RCC_PLLP_DIV19                 (RCC_PLLCFGR_PLLP_4 | RCC_PLLCFGR_PLLP_1)                                                                /*!< PLLP division factor = 19 */
#define RCC_PLLP_DIV20                 (RCC_PLLCFGR_PLLP_4 | RCC_PLLCFGR_PLLP_1 | RCC_PLLCFGR_PLLP_0)                                           /*!< PLLP division factor = 20 */
#define RCC_PLLP_DIV21                 (RCC_PLLCFGR_PLLP_4 | RCC_PLLCFGR_PLLP_2)                                                                /*!< PLLP division factor = 21 */
#define RCC_PLLP_DIV22                 (RCC_PLLCFGR_PLLP_4 | RCC_PLLCFGR_PLLP_2 | RCC_PLLCFGR_PLLP_0)                                           /*!< PLLP division factor = 22 */
#define RCC_PLLP_DIV23                 (RCC_PLLCFGR_PLLP_4 | RCC_PLLCFGR_PLLP_2 | RCC_PLLCFGR_PLLP_1)                                           /*!< PLLP division factor = 23 */
#define RCC_PLLP_DIV24                 (RCC_PLLCFGR_PLLP_4 | RCC_PLLCFGR_PLLP_2 | RCC_PLLCFGR_PLLP_1 | RCC_PLLCFGR_PLLP_0)                      /*!< PLLP division factor = 24 */
#define RCC_PLLP_DIV25                 (RCC_PLLCFGR_PLLP_4 | RCC_PLLCFGR_PLLP_3)                                                                /*!< PLLP division factor = 25 */
#define RCC_PLLP_DIV26                 (RCC_PLLCFGR_PLLP_4 | RCC_PLLCFGR_PLLP_3 | RCC_PLLCFGR_PLLP_0)                                           /*!< PLLP division factor = 26 */
#define RCC_PLLP_DIV27                 (RCC_PLLCFGR_PLLP_4 | RCC_PLLCFGR_PLLP_3 | RCC_PLLCFGR_PLLP_1)                                           /*!< PLLP division factor = 27 */
#define RCC_PLLP_DIV28                 (RCC_PLLCFGR_PLLP_4 | RCC_PLLCFGR_PLLP_3 | RCC_PLLCFGR_PLLP_1 | RCC_PLLCFGR_PLLP_0)                      /*!< PLLP division factor = 28 */
#define RCC_PLLP_DIV29                 (RCC_PLLCFGR_PLLP_4 | RCC_PLLCFGR_PLLP_3 | RCC_PLLCFGR_PLLP_2)                                           /*!< PLLP division factor = 29 */
#define RCC_PLLP_DIV30                 (RCC_PLLCFGR_PLLP_4 | RCC_PLLCFGR_PLLP_3 | RCC_PLLCFGR_PLLP_2 | RCC_PLLCFGR_PLLP_0)                      /*!< PLLP division factor = 30 */
#define RCC_PLLP_DIV31                 (RCC_PLLCFGR_PLLP_4 | RCC_PLLCFGR_PLLP_3 | RCC_PLLCFGR_PLLP_2 | RCC_PLLCFGR_PLLP_1)                      /*!< PLLP division factor = 31 */
#define RCC_PLLP_DIV32                 (RCC_PLLCFGR_PLLP_4 | RCC_PLLCFGR_PLLP_3 | RCC_PLLCFGR_PLLP_2 | RCC_PLLCFGR_PLLP_1 | RCC_PLLCFGR_PLLP_0) /*!< PLLP division factor = 32 */

#define RCC_PLLCFGR_PLLQEN_POS           (24U)
#define RCC_PLLCFGR_PLLQEN_MSK           (0x1UL << RCC_PLLCFGR_PLLQEN_POS)     /*!< 0x01000000 */
#define RCC_PLLCFGR_PLLQEN               RCC_PLLCFGR_PLLQEN_MSK

#define RCC_PLLCFGR_PLLQ_POS             (25U)
#define RCC_PLLCFGR_PLLQ_MSK             (0x7UL << RCC_PLLCFGR_PLLQ_POS)       /*!< 0x0E000000 */
#define RCC_PLLCFGR_PLLQ                 RCC_PLLCFGR_PLLQ_MSK
#define RCC_PLLCFGR_PLLQ_0               (0x1UL << RCC_PLLCFGR_PLLQ_POS)       /*!< 0x02000000 */
#define RCC_PLLCFGR_PLLQ_1               (0x2UL << RCC_PLLCFGR_PLLQ_POS)       /*!< 0x04000000 */
#define RCC_PLLCFGR_PLLQ_2               (0x4UL << RCC_PLLCFGR_PLLQ_POS)       /*!< 0x08000000 */

#define RCC_PLLQ_DIV2                  RCC_PLLCFGR_PLLQ_0                                             /*!< PLLQ division factor = 2 */
#define RCC_PLLQ_DIV3                  RCC_PLLCFGR_PLLQ_1                                             /*!< PLLQ division factor = 3 */
#define RCC_PLLQ_DIV4                  (RCC_PLLCFGR_PLLQ_1 | RCC_PLLCFGR_PLLQ_0)                      /*!< PLLQ division factor = 4 */
#define RCC_PLLQ_DIV5                  RCC_PLLCFGR_PLLQ_2                                             /*!< PLLQ division factor = 5 */
#define RCC_PLLQ_DIV6                  (RCC_PLLCFGR_PLLQ_2 | RCC_PLLCFGR_PLLQ_0)                      /*!< PLLQ division factor = 6 */
#define RCC_PLLQ_DIV7                  (RCC_PLLCFGR_PLLQ_2 | RCC_PLLCFGR_PLLQ_1)                      /*!< PLLQ division factor = 7 */
#define RCC_PLLQ_DIV8                  (RCC_PLLCFGR_PLLQ_2 |RCC_PLLCFGR_PLLQ_1 | RCC_PLLCFGR_PLLQ_0)  /*!< PLLQ division factor = 8 */

#define RCC_PLLCFGR_PLLREN_POS           (28U)
#define RCC_PLLCFGR_PLLREN_MSK           (0x1UL << RCC_PLLCFGR_PLLREN_POS)     /*!< 0x10000000 */
#define RCC_PLLCFGR_PLLREN               RCC_PLLCFGR_PLLREN_MSK

#define RCC_PLLCFGR_PLLR_POS             (29U)
#define RCC_PLLCFGR_PLLR_MSK             (0x7UL << RCC_PLLCFGR_PLLR_POS)       /*!< 0xE0000000 */
#define RCC_PLLCFGR_PLLR                 RCC_PLLCFGR_PLLR_MSK
#define RCC_PLLCFGR_PLLR_0               (0x1UL << RCC_PLLCFGR_PLLR_POS)       /*!< 0x20000000 */
#define RCC_PLLCFGR_PLLR_1               (0x2UL << RCC_PLLCFGR_PLLR_POS)       /*!< 0x40000000 */
#define RCC_PLLCFGR_PLLR_2               (0x4UL << RCC_PLLCFGR_PLLR_POS)       /*!< 0x80000000 */

#define RCC_PLLR_DIV2                  RCC_PLLCFGR_PLLR_0                                             /*!< PLLR division factor = 2 */
#define RCC_PLLR_DIV3                  RCC_PLLCFGR_PLLR_1                                             /*!< PLLR division factor = 3 */
#define RCC_PLLR_DIV4                  (RCC_PLLCFGR_PLLR_1 | RCC_PLLCFGR_PLLR_0)                      /*!< PLLR division factor = 4 */
#define RCC_PLLR_DIV5                  RCC_PLLCFGR_PLLR_2                                             /*!< PLLR division factor = 5 */
#define RCC_PLLR_DIV6                  (RCC_PLLCFGR_PLLR_2 | RCC_PLLCFGR_PLLR_0)                      /*!< PLLR division factor = 6 */
#define RCC_PLLR_DIV7                  (RCC_PLLCFGR_PLLR_2 | RCC_PLLCFGR_PLLR_1)                      /*!< PLLR division factor = 7 */
#define RCC_PLLR_DIV8                  (RCC_PLLCFGR_PLLR_2 | RCC_PLLCFGR_PLLR_1 | RCC_PLLCFGR_PLLR_0) /*!< PLLR division factor = 8 */

/********************  Bit definition for RCC_CRRCR register  ******************/
/*!< RC48CAL configuration */
#define RCC_CRRCR_HSI48CAL_POS            (0U)
#define RCC_CRRCR_HSI48CAL_MSK            (0x1FFUL << RCC_CRRCR_HSI48CAL_POS)    /*!< 0x000001FF */
#define RCC_CRRCR_HSI48CAL                RCC_CRRCR_HSI48CAL_MSK                 /*!< RC48CAL[8:0] bits */
#define RCC_CRRCR_HSI48CAL_0              (0x01UL << RCC_CRRCR_HSI48CAL_POS)     /*!< 0x00000001 */
#define RCC_CRRCR_HSI48CAL_1              (0x02UL << RCC_CRRCR_HSI48CAL_POS)     /*!< 0x00000002 */
#define RCC_CRRCR_HSI48CAL_2              (0x04UL << RCC_CRRCR_HSI48CAL_POS)     /*!< 0x00000004 */
#define RCC_CRRCR_HSI48CAL_3              (0x08UL << RCC_CRRCR_HSI48CAL_POS)     /*!< 0x00000008 */
#define RCC_CRRCR_HSI48CAL_4              (0x10UL << RCC_CRRCR_HSI48CAL_POS)     /*!< 0x00000010 */
#define RCC_CRRCR_HSI48CAL_5              (0x20UL << RCC_CRRCR_HSI48CAL_POS)     /*!< 0x00000020 */
#define RCC_CRRCR_HSI48CAL_6              (0x40UL << RCC_CRRCR_HSI48CAL_POS)     /*!< 0x00000040 */
#define RCC_CRRCR_HSI48CAL_7              (0x80UL << RCC_CRRCR_HSI48CAL_POS)     /*!< 0x00000080 */
#define RCC_CRRCR_HSI48CAL_8              (0x100UL << RCC_CRRCR_HSI48CAL_POS)    /*!< 0x00000100 */
/********************  Bit definition for RCC_CIER register  ******************/
#define RCC_CIER_LSIRDYIE_POS            (0U)
#define RCC_CIER_LSIRDYIE_MSK            (0x1UL << RCC_CIER_LSIRDYIE_POS)      /*!< 0x00000001 */
#define RCC_CIER_LSIRDYIE                RCC_CIER_LSIRDYIE_MSK
#define RCC_CIER_LSERDYIE_POS            (1U)
#define RCC_CIER_LSERDYIE_MSK            (0x1UL << RCC_CIER_LSERDYIE_POS)      /*!< 0x00000002 */
#define RCC_CIER_LSERDYIE                RCC_CIER_LSERDYIE_MSK
#define RCC_CIER_HSI48RDYIE_POS          (2U)
#define RCC_CIER_HSI48RDYIE_MSK          (0x1UL << RCC_CIER_HSI48RDYIE_POS)      /*!< 0x00000004 */
#define RCC_CIER_HSI48RDYIE              RCC_CIER_HSI48RDYIE_MSK
#define RCC_CIER_HSIRDYIE_POS            (3U)
#define RCC_CIER_HSIRDYIE_MSK            (0x1UL << RCC_CIER_HSIRDYIE_POS)      /*!< 0x00000008 */
#define RCC_CIER_HSIRDYIE                RCC_CIER_HSIRDYIE_MSK
#define RCC_CIER_HSERDYIE_POS            (4U)
#define RCC_CIER_HSERDYIE_MSK            (0x1UL << RCC_CIER_HSERDYIE_POS)      /*!< 0x00000010 */
#define RCC_CIER_HSERDYIE                RCC_CIER_HSERDYIE_MSK
#define RCC_CIER_PLLRDYIE_POS            (5U)
#define RCC_CIER_PLLRDYIE_MSK            (0x1UL << RCC_CIER_PLLRDYIE_POS)      /*!< 0x00000020 */
#define RCC_CIER_PLLRDYIE                RCC_CIER_PLLRDYIE_MSK

/********************  Bit definition for RCC_CIFR register  ******************/
#define RCC_CIFR_LSIRDYF_POS             (0U)
#define RCC_CIFR_LSIRDYF_MSK             (0x1UL << RCC_CIFR_LSIRDYF_POS)       /*!< 0x00000001 */
#define RCC_CIFR_LSIRDYF                 RCC_CIFR_LSIRDYF_MSK
#define RCC_CIFR_LSERDYF_POS             (1U)
#define RCC_CIFR_LSERDYF_MSK             (0x1UL << RCC_CIFR_LSERDYF_POS)       /*!< 0x00000002 */
#define RCC_CIFR_LSERDYF                 RCC_CIFR_LSERDYF_MSK
#define RCC_CIFR_HSI48RDYF_POS           (2U)
#define RCC_CIFR_HSI48RDYF_MSK           (0x1UL << RCC_CIFR_HSI48RDYF_POS)      /*!< 0x00000004 */
#define RCC_CIFR_HSI48RDYF                RCC_CIFR_HSI48RDYF_MSK
#define RCC_CIFR_HSIRDYF_POS             (3U)
#define RCC_CIFR_HSIRDYF_MSK             (0x1UL << RCC_CIFR_HSIRDYF_POS)       /*!< 0x00000008 */
#define RCC_CIFR_HSIRDYF                 RCC_CIFR_HSIRDYF_MSK
#define RCC_CIFR_HSERDYF_POS             (4U)
#define RCC_CIFR_HSERDYF_MSK             (0x1UL << RCC_CIFR_HSERDYF_POS)       /*!< 0x00000010 */
#define RCC_CIFR_HSERDYF                 RCC_CIFR_HSERDYF_MSK
#define RCC_CIFR_PLLRDYF_POS             (5U)
#define RCC_CIFR_PLLRDYF_MSK             (0x1UL << RCC_CIFR_PLLRDYF_POS)       /*!< 0x00000020 */
#define RCC_CIFR_PLLRDYF                 RCC_CIFR_PLLRDYF_MSK
#define RCC_CIFR_CSSF_POS                (8U)
#define RCC_CIFR_CSSF_MSK                (0x1UL << RCC_CIFR_CSSF_POS)          /*!< 0x00000100 */
#define RCC_CIFR_CSSF                    RCC_CIFR_CSSF_MSK
#define RCC_CIFR_LSECSSF_POS             (9U)
#define RCC_CIFR_LSECSSF_MSK             (0x1UL << RCC_CIFR_LSECSSF_POS)       /*!< 0x00000200 */
#define RCC_CIFR_LSECSSF                 RCC_CIFR_LSECSSF_MSK

/********************  Bit definition for RCC_CICR register  ******************/
#define RCC_CICR_LSIRDYC_POS             (0U)
#define RCC_CICR_LSIRDYC_MSK             (0x1UL << RCC_CICR_LSIRDYC_POS)       /*!< 0x00000001 */
#define RCC_CICR_LSIRDYC                 RCC_CICR_LSIRDYC_MSK
#define RCC_CICR_LSERDYC_POS             (1U)
#define RCC_CICR_LSERDYC_MSK             (0x1UL << RCC_CICR_LSERDYC_POS)       /*!< 0x00000002 */
#define RCC_CICR_LSERDYC                 RCC_CICR_LSERDYC_MSK
#define RCC_CICR_HSI48RDYC_POS           (2U)
#define RCC_CICR_HSI48RDYC_MSK           (0x1UL << RCC_CICR_HSI48RDYC_POS)      /*!< 0x00000004 */
#define RCC_CICR_HSI48RDYC               RCC_CICR_HSI48RDYC_MSK
#define RCC_CICR_HSIRDYC_POS             (3U)
#define RCC_CICR_HSIRDYC_MSK             (0x1UL << RCC_CICR_HSIRDYC_POS)       /*!< 0x00000008 */
#define RCC_CICR_HSIRDYC                 RCC_CICR_HSIRDYC_MSK
#define RCC_CICR_HSERDYC_POS             (4U)
#define RCC_CICR_HSERDYC_MSK             (0x1UL << RCC_CICR_HSERDYC_POS)       /*!< 0x00000010 */
#define RCC_CICR_HSERDYC                 RCC_CICR_HSERDYC_MSK
#define RCC_CICR_PLLRDYC_POS             (5U)
#define RCC_CICR_PLLRDYC_MSK             (0x1UL << RCC_CICR_PLLRDYC_POS)       /*!< 0x00000020 */
#define RCC_CICR_PLLRDYC                 RCC_CICR_PLLRDYC_MSK
#define RCC_CICR_CSSC_POS                (8U)
#define RCC_CICR_CSSC_MSK                (0x1UL << RCC_CICR_CSSC_POS)          /*!< 0x00000100 */
#define RCC_CICR_CSSC                    RCC_CICR_CSSC_MSK
#define RCC_CICR_LSECSSC_POS             (9U)
#define RCC_CICR_LSECSSC_MSK             (0x1UL << RCC_CICR_LSECSSC_POS)       /*!< 0x00000200 */
#define RCC_CICR_LSECSSC                 RCC_CICR_LSECSSC_MSK

/********************  Bit definition for RCC_IOPRSTR register  ****************/
#define RCC_IOPRSTR_GPIOARST_POS         (0U)
#define RCC_IOPRSTR_GPIOARST_MSK         (0x1UL << RCC_IOPRSTR_GPIOARST_POS)   /*!< 0x00000001 */
#define RCC_IOPRSTR_GPIOARST             RCC_IOPRSTR_GPIOARST_MSK
#define RCC_IOPRSTR_GPIOBRST_POS         (1U)
#define RCC_IOPRSTR_GPIOBRST_MSK         (0x1UL << RCC_IOPRSTR_GPIOBRST_POS)   /*!< 0x00000002 */
#define RCC_IOPRSTR_GPIOBRST             RCC_IOPRSTR_GPIOBRST_MSK
#define RCC_IOPRSTR_GPIOCRST_POS         (2U)
#define RCC_IOPRSTR_GPIOCRST_MSK         (0x1UL << RCC_IOPRSTR_GPIOCRST_POS)   /*!< 0x00000004 */
#define RCC_IOPRSTR_GPIOCRST             RCC_IOPRSTR_GPIOCRST_MSK
#define RCC_IOPRSTR_GPIODRST_POS         (3U)
#define RCC_IOPRSTR_GPIODRST_MSK         (0x1UL << RCC_IOPRSTR_GPIODRST_POS)   /*!< 0x00000008 */
#define RCC_IOPRSTR_GPIODRST             RCC_IOPRSTR_GPIODRST_MSK
#define RCC_IOPRSTR_GPIOERST_POS         (4U)
#define RCC_IOPRSTR_GPIOERST_MSK         (0x1UL << RCC_IOPRSTR_GPIOERST_POS)   /*!< 0x00000010 */
#define RCC_IOPRSTR_GPIOERST             RCC_IOPRSTR_GPIOERST_MSK
#define RCC_IOPRSTR_GPIOFRST_POS         (5U)
#define RCC_IOPRSTR_GPIOFRST_MSK         (0x1UL << RCC_IOPRSTR_GPIOFRST_POS)   /*!< 0x00000020 */
#define RCC_IOPRSTR_GPIOFRST             RCC_IOPRSTR_GPIOFRST_MSK

/********************  Bit definition for RCC_AHBRSTR register  ***************/
#define RCC_AHBRSTR_DMA1RST_POS          (0U)
#define RCC_AHBRSTR_DMA1RST_MSK          (0x1UL << RCC_AHBRSTR_DMA1RST_POS)    /*!< 0x00000001 */
#define RCC_AHBRSTR_DMA1RST              RCC_AHBRSTR_DMA1RST_MSK
#define RCC_AHBRSTR_DMA2RST_POS          (1U)
#define RCC_AHBRSTR_DMA2RST_MSK          (0x1UL << RCC_AHBRSTR_DMA2RST_POS)    /*!< 0x00000002 */
#define RCC_AHBRSTR_DMA2RST              RCC_AHBRSTR_DMA2RST_MSK
#define RCC_AHBRSTR_FLASHRST_POS         (8U)
#define RCC_AHBRSTR_FLASHRST_MSK         (0x1UL << RCC_AHBRSTR_FLASHRST_POS)   /*!< 0x00000100 */
#define RCC_AHBRSTR_FLASHRST             RCC_AHBRSTR_FLASHRST_MSK
#define RCC_AHBRSTR_CRCRST_POS           (12U)
#define RCC_AHBRSTR_CRCRST_MSK           (0x1UL << RCC_AHBRSTR_CRCRST_POS)     /*!< 0x00001000 */
#define RCC_AHBRSTR_CRCRST               RCC_AHBRSTR_CRCRST_MSK

/********************  Bit definition for RCC_APBRSTR1 register  **************/
#define RCC_APBRSTR1_TIM2RST_POS         (0U)
#define RCC_APBRSTR1_TIM2RST_MSK         (0x1UL << RCC_APBRSTR1_TIM2RST_POS)   /*!< 0x00000001 */
#define RCC_APBRSTR1_TIM2RST             RCC_APBRSTR1_TIM2RST_MSK
#define RCC_APBRSTR1_TIM3RST_POS         (1U)
#define RCC_APBRSTR1_TIM3RST_MSK         (0x1UL << RCC_APBRSTR1_TIM3RST_POS)   /*!< 0x00000002 */
#define RCC_APBRSTR1_TIM3RST             RCC_APBRSTR1_TIM3RST_MSK
#define RCC_APBRSTR1_TIM4RST_POS         (2U)
#define RCC_APBRSTR1_TIM4RST_MSK         (0x1UL << RCC_APBRSTR1_TIM4RST_POS)   /*!< 0x00000004 */
#define RCC_APBRSTR1_TIM4RST             RCC_APBRSTR1_TIM4RST_MSK
#define RCC_APBRSTR1_TIM6RST_POS         (4U)
#define RCC_APBRSTR1_TIM6RST_MSK         (0x1UL << RCC_APBRSTR1_TIM6RST_POS)   /*!< 0x00000010 */
#define RCC_APBRSTR1_TIM6RST             RCC_APBRSTR1_TIM6RST_MSK
#define RCC_APBRSTR1_TIM7RST_POS         (5U)
#define RCC_APBRSTR1_TIM7RST_MSK         (0x1UL << RCC_APBRSTR1_TIM7RST_POS)   /*!< 0x00000020 */
#define RCC_APBRSTR1_TIM7RST             RCC_APBRSTR1_TIM7RST_MSK
#define RCC_APBRSTR1_LPUART2RST_POS      (7U)
#define RCC_APBRSTR1_LPUART2RST_MSK      (0x1UL << RCC_APBRSTR1_LPUART2RST_POS)/*!< 0x00000080 */
#define RCC_APBRSTR1_LPUART2RST          RCC_APBRSTR1_LPUART2RST_MSK
#define RCC_APBRSTR1_USART5RST_POS       (8U)
#define RCC_APBRSTR1_USART5RST_MSK       (0x1UL << RCC_APBRSTR1_USART5RST_POS) /*!< 0x00000100 */
#define RCC_APBRSTR1_USART5RST           RCC_APBRSTR1_USART5RST_MSK
#define RCC_APBRSTR1_USART6RST_POS       (9U)
#define RCC_APBRSTR1_USART6RST_MSK       (0x1UL << RCC_APBRSTR1_USART6RST_POS) /*!< 0x00000200 */
#define RCC_APBRSTR1_USART6RST           RCC_APBRSTR1_USART6RST_MSK
#define RCC_APBRSTR1_FDCANRST_POS        (12U)
#define RCC_APBRSTR1_FDCANRST_MSK        (0x1UL << RCC_APBRSTR1_FDCANRST_POS)  /*!< 0x00001000 */
#define RCC_APBRSTR1_FDCANRST            RCC_APBRSTR1_FDCANRST_MSK
#define RCC_APBRSTR1_USBRST_POS          (13U)
#define RCC_APBRSTR1_USBRST_MSK          (0x1UL << RCC_APBRSTR1_USBRST_POS)    /*!< 0x00002000 */
#define RCC_APBRSTR1_USBRST              RCC_APBRSTR1_USBRST_MSK
#define RCC_APBRSTR1_SPI2RST_POS         (14U)
#define RCC_APBRSTR1_SPI2RST_MSK         (0x1UL << RCC_APBRSTR1_SPI2RST_POS)   /*!< 0x00004000 */
#define RCC_APBRSTR1_SPI2RST             RCC_APBRSTR1_SPI2RST_MSK
#define RCC_APBRSTR1_SPI3RST_POS         (15U)
#define RCC_APBRSTR1_SPI3RST_MSK         (0x1UL << RCC_APBRSTR1_SPI3RST_POS)   /*!< 0x00008000 */
#define RCC_APBRSTR1_SPI3RST             RCC_APBRSTR1_SPI3RST_MSK
#define RCC_APBRSTR1_CRSRST_POS          (16U)
#define RCC_APBRSTR1_CRSRST_MSK          (0x1UL << RCC_APBRSTR1_CRSRST_POS)    /*!< 0x00010000 */
#define RCC_APBRSTR1_CRSRST              RCC_APBRSTR1_CRSRST_MSK
#define RCC_APBRSTR1_USART2RST_POS       (17U)
#define RCC_APBRSTR1_USART2RST_MSK       (0x1UL << RCC_APBRSTR1_USART2RST_POS) /*!< 0x00020000 */
#define RCC_APBRSTR1_USART2RST           RCC_APBRSTR1_USART2RST_MSK
#define RCC_APBRSTR1_USART3RST_POS       (18U)
#define RCC_APBRSTR1_USART3RST_MSK       (0x1UL << RCC_APBRSTR1_USART3RST_POS) /*!< 0x00040000 */
#define RCC_APBRSTR1_USART3RST           RCC_APBRSTR1_USART3RST_MSK
#define RCC_APBRSTR1_USART4RST_POS       (19U)
#define RCC_APBRSTR1_USART4RST_MSK       (0x1UL << RCC_APBRSTR1_USART4RST_POS) /*!< 0x00080000 */
#define RCC_APBRSTR1_USART4RST           RCC_APBRSTR1_USART4RST_MSK
#define RCC_APBRSTR1_LPUART1RST_POS      (20U)
#define RCC_APBRSTR1_LPUART1RST_MSK      (0x1UL << RCC_APBRSTR1_LPUART1RST_POS) /*!< 0x00100000 */
#define RCC_APBRSTR1_LPUART1RST          RCC_APBRSTR1_LPUART1RST_MSK
#define RCC_APBRSTR1_I2C1RST_POS         (21U)
#define RCC_APBRSTR1_I2C1RST_MSK         (0x1UL << RCC_APBRSTR1_I2C1RST_POS)    /*!< 0x00200000 */
#define RCC_APBRSTR1_I2C1RST             RCC_APBRSTR1_I2C1RST_MSK
#define RCC_APBRSTR1_I2C2RST_POS         (22U)
#define RCC_APBRSTR1_I2C2RST_MSK         (0x1UL << RCC_APBRSTR1_I2C2RST_POS)    /*!< 0x00400000 */
#define RCC_APBRSTR1_I2C2RST             RCC_APBRSTR1_I2C2RST_MSK
#define RCC_APBRSTR1_I2C3RST_POS         (23U)
#define RCC_APBRSTR1_I2C3RST_MSK         (0x1UL << RCC_APBRSTR1_I2C3RST_POS)    /*!< 0x00800000 */
#define RCC_APBRSTR1_I2C3RST             RCC_APBRSTR1_I2C3RST_MSK
#define RCC_APBRSTR1_CECRST_POS          (24U)
#define RCC_APBRSTR1_CECRST_MSK          (0x1UL << RCC_APBRSTR1_CECRST_POS)     /*!< 0x01000000 */
#define RCC_APBRSTR1_CECRST              RCC_APBRSTR1_CECRST_MSK
#define RCC_APBRSTR1_UCPD1RST_POS        (25U)
#define RCC_APBRSTR1_UCPD1RST_MSK        (0x1UL << RCC_APBRSTR1_UCPD1RST_POS)   /*!< 0x02000000 */
#define RCC_APBRSTR1_UCPD1RST            RCC_APBRSTR1_UCPD1RST_MSK
#define RCC_APBRSTR1_UCPD2RST_POS        (26U)
#define RCC_APBRSTR1_UCPD2RST_MSK        (0x1UL << RCC_APBRSTR1_UCPD2RST_POS)   /*!< 0x04000000 */
#define RCC_APBRSTR1_UCPD2RST            RCC_APBRSTR1_UCPD2RST_MSK
#define RCC_APBRSTR1_DBGRST_POS          (27U)
#define RCC_APBRSTR1_DBGRST_MSK          (0x1UL << RCC_APBRSTR1_DBGRST_POS)     /*!< 0x08000000 */
#define RCC_APBRSTR1_DBGRST              RCC_APBRSTR1_DBGRST_MSK
#define RCC_APBRSTR1_PWRRST_POS          (28U)
#define RCC_APBRSTR1_PWRRST_MSK          (0x1UL << RCC_APBRSTR1_PWRRST_POS)     /*!< 0x10000000 */
#define RCC_APBRSTR1_PWRRST              RCC_APBRSTR1_PWRRST_MSK
#define RCC_APBRSTR1_DAC1RST_POS         (29U)
#define RCC_APBRSTR1_DAC1RST_MSK         (0x1UL << RCC_APBRSTR1_DAC1RST_POS)    /*!< 0x20000000 */
#define RCC_APBRSTR1_DAC1RST             RCC_APBRSTR1_DAC1RST_MSK
#define RCC_APBRSTR1_LPTIM2RST_POS       (30U)
#define RCC_APBRSTR1_LPTIM2RST_MSK       (0x1UL << RCC_APBRSTR1_LPTIM2RST_POS)  /*!< 0x40000000 */
#define RCC_APBRSTR1_LPTIM2RST           RCC_APBRSTR1_LPTIM2RST_MSK
#define RCC_APBRSTR1_LPTIM1RST_POS       (31U)
#define RCC_APBRSTR1_LPTIM1RST_MSK       (0x1UL << RCC_APBRSTR1_LPTIM1RST_POS)  /*!< 0x80000000 */
#define RCC_APBRSTR1_LPTIM1RST           RCC_APBRSTR1_LPTIM1RST_MSK

/********************  Bit definition for RCC_APBRSTR2 register  **************/
#define RCC_APBRSTR2_SYSCFGRST_POS       (0U)
#define RCC_APBRSTR2_SYSCFGRST_MSK       (0x1UL << RCC_APBRSTR2_SYSCFGRST_POS)  /*!< 0x00000001 */
#define RCC_APBRSTR2_SYSCFGRST           RCC_APBRSTR2_SYSCFGRST_MSK
#define RCC_APBRSTR2_TIM1RST_POS         (11U)
#define RCC_APBRSTR2_TIM1RST_MSK         (0x1UL << RCC_APBRSTR2_TIM1RST_POS)    /*!< 0x00000800 */
#define RCC_APBRSTR2_TIM1RST             RCC_APBRSTR2_TIM1RST_MSK
#define RCC_APBRSTR2_SPI1RST_POS         (12U)
#define RCC_APBRSTR2_SPI1RST_MSK         (0x1UL << RCC_APBRSTR2_SPI1RST_POS)    /*!< 0x00001000 */
#define RCC_APBRSTR2_SPI1RST             RCC_APBRSTR2_SPI1RST_MSK
#define RCC_APBRSTR2_USART1RST_POS       (14U)
#define RCC_APBRSTR2_USART1RST_MSK       (0x1UL << RCC_APBRSTR2_USART1RST_POS)  /*!< 0x00004000 */
#define RCC_APBRSTR2_USART1RST           RCC_APBRSTR2_USART1RST_MSK
#define RCC_APBRSTR2_TIM14RST_POS        (15U)
#define RCC_APBRSTR2_TIM14RST_MSK        (0x1UL << RCC_APBRSTR2_TIM14RST_POS)   /*!< 0x00008000 */
#define RCC_APBRSTR2_TIM14RST            RCC_APBRSTR2_TIM14RST_MSK
#define RCC_APBRSTR2_TIM15RST_POS        (16U)
#define RCC_APBRSTR2_TIM15RST_MSK        (0x1UL << RCC_APBRSTR2_TIM15RST_POS)   /*!< 0x00010000 */
#define RCC_APBRSTR2_TIM15RST            RCC_APBRSTR2_TIM15RST_MSK
#define RCC_APBRSTR2_TIM16RST_POS        (17U)
#define RCC_APBRSTR2_TIM16RST_MSK        (0x1UL << RCC_APBRSTR2_TIM16RST_POS)   /*!< 0x00020000 */
#define RCC_APBRSTR2_TIM16RST            RCC_APBRSTR2_TIM16RST_MSK
#define RCC_APBRSTR2_TIM17RST_POS        (18U)
#define RCC_APBRSTR2_TIM17RST_MSK        (0x1UL << RCC_APBRSTR2_TIM17RST_POS)   /*!< 0x00040000 */
#define RCC_APBRSTR2_TIM17RST            RCC_APBRSTR2_TIM17RST_MSK
#define RCC_APBRSTR2_ADCRST_POS          (20U)
#define RCC_APBRSTR2_ADCRST_MSK          (0x1UL << RCC_APBRSTR2_ADCRST_POS)     /*!< 0x00100000 */
#define RCC_APBRSTR2_ADCRST              RCC_APBRSTR2_ADCRST_MSK

/********************  Bit definition for RCC_IOPENR register  ****************/
#define RCC_IOPENR_GPIOAEN_POS           (0U)
#define RCC_IOPENR_GPIOAEN_MSK           (0x1UL << RCC_IOPENR_GPIOAEN_POS)      /*!< 0x00000001 */
#define RCC_IOPENR_GPIOAEN               RCC_IOPENR_GPIOAEN_MSK
#define RCC_IOPENR_GPIOBEN_POS           (1U)
#define RCC_IOPENR_GPIOBEN_MSK           (0x1UL << RCC_IOPENR_GPIOBEN_POS)      /*!< 0x00000002 */
#define RCC_IOPENR_GPIOBEN               RCC_IOPENR_GPIOBEN_MSK
#define RCC_IOPENR_GPIOCEN_POS           (2U)
#define RCC_IOPENR_GPIOCEN_MSK           (0x1UL << RCC_IOPENR_GPIOCEN_POS)      /*!< 0x00000004 */
#define RCC_IOPENR_GPIOCEN               RCC_IOPENR_GPIOCEN_MSK
#define RCC_IOPENR_GPIODEN_POS           (3U)
#define RCC_IOPENR_GPIODEN_MSK           (0x1UL << RCC_IOPENR_GPIODEN_POS)      /*!< 0x00000008 */
#define RCC_IOPENR_GPIODEN               RCC_IOPENR_GPIODEN_MSK
#define RCC_IOPENR_GPIOEEN_POS           (4U)
#define RCC_IOPENR_GPIOEEN_MSK           (0x1UL << RCC_IOPENR_GPIOEEN_POS)      /*!< 0x00000010 */
#define RCC_IOPENR_GPIOEEN               RCC_IOPENR_GPIOEEN_MSK
#define RCC_IOPENR_GPIOFEN_POS           (5U)
#define RCC_IOPENR_GPIOFEN_MSK           (0x1UL << RCC_IOPENR_GPIOFEN_POS)      /*!< 0x00000020 */
#define RCC_IOPENR_GPIOFEN               RCC_IOPENR_GPIOFEN_MSK

/********************  Bit definition for RCC_AHBENR register  ****************/
#define RCC_AHBENR_DMA1EN_POS            (0U)
#define RCC_AHBENR_DMA1EN_MSK            (0x1UL << RCC_AHBENR_DMA1EN_POS)       /*!< 0x00000001 */
#define RCC_AHBENR_DMA1EN                RCC_AHBENR_DMA1EN_MSK
#define RCC_AHBENR_DMA2EN_POS            (1U)
#define RCC_AHBENR_DMA2EN_MSK            (0x1UL << RCC_AHBENR_DMA2EN_POS)       /*!< 0x00000002 */
#define RCC_AHBENR_DMA2EN                RCC_AHBENR_DMA2EN_MSK
#define RCC_AHBENR_FLASHEN_POS           (8U)
#define RCC_AHBENR_FLASHEN_MSK           (0x1UL << RCC_AHBENR_FLASHEN_POS)      /*!< 0x00000100 */
#define RCC_AHBENR_FLASHEN               RCC_AHBENR_FLASHEN_MSK
#define RCC_AHBENR_CRCEN_POS             (12U)
#define RCC_AHBENR_CRCEN_MSK             (0x1UL << RCC_AHBENR_CRCEN_POS)        /*!< 0x00001000 */
#define RCC_AHBENR_CRCEN                 RCC_AHBENR_CRCEN_MSK

/********************  Bit definition for RCC_APBENR1 register  ***************/
#define RCC_APBENR1_TIM2EN_POS           (0U)
#define RCC_APBENR1_TIM2EN_MSK           (0x1UL << RCC_APBENR1_TIM2EN_POS)      /*!< 0x00000001 */
#define RCC_APBENR1_TIM2EN               RCC_APBENR1_TIM2EN_MSK
#define RCC_APBENR1_TIM3EN_POS           (1U)
#define RCC_APBENR1_TIM3EN_MSK           (0x1UL << RCC_APBENR1_TIM3EN_POS)      /*!< 0x00000002 */
#define RCC_APBENR1_TIM3EN               RCC_APBENR1_TIM3EN_MSK
#define RCC_APBENR1_TIM4EN_POS           (2U)
#define RCC_APBENR1_TIM4EN_MSK           (0x1UL << RCC_APBENR1_TIM4EN_POS)      /*!< 0x00000004 */
#define RCC_APBENR1_TIM4EN               RCC_APBENR1_TIM4EN_MSK
#define RCC_APBENR1_TIM6EN_POS           (4U)
#define RCC_APBENR1_TIM6EN_MSK           (0x1UL << RCC_APBENR1_TIM6EN_POS)      /*!< 0x00000010 */
#define RCC_APBENR1_TIM6EN               RCC_APBENR1_TIM6EN_MSK
#define RCC_APBENR1_TIM7EN_POS           (5U)
#define RCC_APBENR1_TIM7EN_MSK           (0x1UL << RCC_APBENR1_TIM7EN_POS)      /*!< 0x00000020 */
#define RCC_APBENR1_TIM7EN               RCC_APBENR1_TIM7EN_MSK
#define RCC_APBENR1_LPUART2EN_POS        (7U)
#define RCC_APBENR1_LPUART2EN_MSK        (0x1UL << RCC_APBENR1_LPUART2EN_POS)   /*!< 0x00000080 */
#define RCC_APBENR1_LPUART2EN            RCC_APBENR1_LPUART2EN_MSK
#define RCC_APBENR1_USART5EN_POS         (8U)
#define RCC_APBENR1_USART5EN_MSK         (0x1UL << RCC_APBENR1_USART5EN_POS)    /*!< 0x00000100 */
#define RCC_APBENR1_USART5EN             RCC_APBENR1_USART5EN_MSK
#define RCC_APBENR1_USART6EN_POS         (9U)
#define RCC_APBENR1_USART6EN_MSK         (0x1UL << RCC_APBENR1_USART6EN_POS)    /*!< 0x00000200 */
#define RCC_APBENR1_USART6EN             RCC_APBENR1_USART6EN_MSK
#define RCC_APBENR1_RTCAPBEN_POS         (10U)
#define RCC_APBENR1_RTCAPBEN_MSK         (0x1UL << RCC_APBENR1_RTCAPBEN_POS)    /*!< 0x00000400 */
#define RCC_APBENR1_RTCAPBEN             RCC_APBENR1_RTCAPBEN_MSK
#define RCC_APBENR1_WWDGEN_POS           (11U)
#define RCC_APBENR1_WWDGEN_MSK           (0x1UL << RCC_APBENR1_WWDGEN_POS)      /*!< 0x00000800 */
#define RCC_APBENR1_WWDGEN               RCC_APBENR1_WWDGEN_MSK
#define RCC_APBENR1_FDCANEN_POS          (12U)
#define RCC_APBENR1_FDCANEN_MSK          (0x1UL << RCC_APBENR1_FDCANEN_POS)     /*!< 0x00001000 */
#define RCC_APBENR1_FDCANEN              RCC_APBENR1_FDCANEN_MSK
#define RCC_APBENR1_USBEN_POS            (13U)
#define RCC_APBENR1_USBEN_MSK            (0x1UL << RCC_APBENR1_USBEN_POS)       /*!< 0x00002000 */
#define RCC_APBENR1_USBEN                RCC_APBENR1_USBEN_MSK
#define RCC_APBENR1_SPI2EN_POS           (14U)
#define RCC_APBENR1_SPI2EN_MSK           (0x1UL << RCC_APBENR1_SPI2EN_POS)      /*!< 0x00004000 */
#define RCC_APBENR1_SPI2EN               RCC_APBENR1_SPI2EN_MSK
#define RCC_APBENR1_SPI3EN_POS           (15U)
#define RCC_APBENR1_SPI3EN_MSK           (0x1UL << RCC_APBENR1_SPI3EN_POS)      /*!< 0x00008000 */
#define RCC_APBENR1_SPI3EN               RCC_APBENR1_SPI3EN_MSK
#define RCC_APBENR1_CRSEN_POS            (16U)
#define RCC_APBENR1_CRSEN_MSK            (0x1UL << RCC_APBENR1_CRSEN_POS)       /*!< 0x00010000 */
#define RCC_APBENR1_CRSEN                RCC_APBENR1_CRSEN_MSK
#define RCC_APBENR1_USART2EN_POS         (17U)
#define RCC_APBENR1_USART2EN_MSK         (0x1UL << RCC_APBENR1_USART2EN_POS)    /*!< 0x00020000 */
#define RCC_APBENR1_USART2EN             RCC_APBENR1_USART2EN_MSK
#define RCC_APBENR1_USART3EN_POS         (18U)
#define RCC_APBENR1_USART3EN_MSK         (0x1UL << RCC_APBENR1_USART3EN_POS)    /*!< 0x00040000 */
#define RCC_APBENR1_USART3EN             RCC_APBENR1_USART3EN_MSK
#define RCC_APBENR1_USART4EN_POS         (19U)
#define RCC_APBENR1_USART4EN_MSK         (0x1UL << RCC_APBENR1_USART4EN_POS)    /*!< 0x00080000 */
#define RCC_APBENR1_USART4EN             RCC_APBENR1_USART4EN_MSK
#define RCC_APBENR1_LPUART1EN_POS        (20U)
#define RCC_APBENR1_LPUART1EN_MSK        (0x1UL << RCC_APBENR1_LPUART1EN_POS)   /*!< 0x00100000 */
#define RCC_APBENR1_LPUART1EN            RCC_APBENR1_LPUART1EN_MSK
#define RCC_APBENR1_I2C1EN_POS           (21U)
#define RCC_APBENR1_I2C1EN_MSK           (0x1UL << RCC_APBENR1_I2C1EN_POS)      /*!< 0x00200000 */
#define RCC_APBENR1_I2C1EN               RCC_APBENR1_I2C1EN_MSK
#define RCC_APBENR1_I2C2EN_POS           (22U)
#define RCC_APBENR1_I2C2EN_MSK           (0x1UL << RCC_APBENR1_I2C2EN_POS)      /*!< 0x00400000 */
#define RCC_APBENR1_I2C2EN               RCC_APBENR1_I2C2EN_MSK
#define RCC_APBENR1_I2C3EN_POS           (23U)
#define RCC_APBENR1_I2C3EN_MSK           (0x1UL << RCC_APBENR1_I2C3EN_POS)      /*!< 0x00800000 */
#define RCC_APBENR1_I2C3EN               RCC_APBENR1_I2C3EN_MSK
#define RCC_APBENR1_CECEN_POS            (24U)
#define RCC_APBENR1_CECEN_MSK            (0x1UL << RCC_APBENR1_CECEN_POS)       /*!< 0x01000000 */
#define RCC_APBENR1_CECEN                RCC_APBENR1_CECEN_MSK
#define RCC_APBENR1_UCPD1EN_POS          (25U)
#define RCC_APBENR1_UCPD1EN_MSK          (0x1UL << RCC_APBENR1_UCPD1EN_POS)     /*!< 0x02000000 */
#define RCC_APBENR1_UCPD1EN              RCC_APBENR1_UCPD1EN_MSK
#define RCC_APBENR1_UCPD2EN_POS          (26U)
#define RCC_APBENR1_UCPD2EN_MSK          (0x1UL << RCC_APBENR1_UCPD2EN_POS)     /*!< 0x04000000 */
#define RCC_APBENR1_UCPD2EN              RCC_APBENR1_UCPD2EN_MSK
#define RCC_APBENR1_DBGEN_POS            (27U)
#define RCC_APBENR1_DBGEN_MSK            (0x1UL << RCC_APBENR1_DBGEN_POS)       /*!< 0x08000000 */
#define RCC_APBENR1_DBGEN                RCC_APBENR1_DBGEN_MSK
#define RCC_APBENR1_PWREN_POS            (28U)
#define RCC_APBENR1_PWREN_MSK            (0x1UL << RCC_APBENR1_PWREN_POS)       /*!< 0x10000000 */
#define RCC_APBENR1_PWREN                RCC_APBENR1_PWREN_MSK
#define RCC_APBENR1_DAC1EN_POS           (29U)
#define RCC_APBENR1_DAC1EN_MSK           (0x1UL << RCC_APBENR1_DAC1EN_POS)      /*!< 0x20000000 */
#define RCC_APBENR1_DAC1EN               RCC_APBENR1_DAC1EN_MSK
#define RCC_APBENR1_LPTIM2EN_POS         (30U)
#define RCC_APBENR1_LPTIM2EN_MSK         (0x1UL << RCC_APBENR1_LPTIM2EN_POS)    /*!< 0x40000000 */
#define RCC_APBENR1_LPTIM2EN             RCC_APBENR1_LPTIM2EN_MSK
#define RCC_APBENR1_LPTIM1EN_POS         (31U)
#define RCC_APBENR1_LPTIM1EN_MSK         (0x1UL << RCC_APBENR1_LPTIM1EN_POS)    /*!< 0x80000000 */
#define RCC_APBENR1_LPTIM1EN             RCC_APBENR1_LPTIM1EN_MSK

/********************  Bit definition for RCC_APBENR2 register  **************/
#define RCC_APBENR2_SYSCFGEN_POS         (0U)
#define RCC_APBENR2_SYSCFGEN_MSK         (0x1UL << RCC_APBENR2_SYSCFGEN_POS)    /*!< 0x00000001 */
#define RCC_APBENR2_SYSCFGEN             RCC_APBENR2_SYSCFGEN_MSK
#define RCC_APBENR2_TIM1EN_POS           (11U)
#define RCC_APBENR2_TIM1EN_MSK           (0x1UL << RCC_APBENR2_TIM1EN_POS)      /*!< 0x00000800 */
#define RCC_APBENR2_TIM1EN               RCC_APBENR2_TIM1EN_MSK
#define RCC_APBENR2_SPI1EN_POS           (12U)
#define RCC_APBENR2_SPI1EN_MSK           (0x1UL << RCC_APBENR2_SPI1EN_POS)      /*!< 0x00001000 */
#define RCC_APBENR2_SPI1EN               RCC_APBENR2_SPI1EN_MSK
#define RCC_APBENR2_USART1EN_POS         (14U)
#define RCC_APBENR2_USART1EN_MSK         (0x1UL << RCC_APBENR2_USART1EN_POS)    /*!< 0x00004000 */
#define RCC_APBENR2_USART1EN             RCC_APBENR2_USART1EN_MSK
#define RCC_APBENR2_TIM14EN_POS          (15U)
#define RCC_APBENR2_TIM14EN_MSK          (0x1UL << RCC_APBENR2_TIM14EN_POS)     /*!< 0x00008000 */
#define RCC_APBENR2_TIM14EN              RCC_APBENR2_TIM14EN_MSK
#define RCC_APBENR2_TIM15EN_POS          (16U)
#define RCC_APBENR2_TIM15EN_MSK          (0x1UL << RCC_APBENR2_TIM15EN_POS)     /*!< 0x00010000 */
#define RCC_APBENR2_TIM15EN              RCC_APBENR2_TIM15EN_MSK
#define RCC_APBENR2_TIM16EN_POS          (17U)
#define RCC_APBENR2_TIM16EN_MSK          (0x1UL << RCC_APBENR2_TIM16EN_POS)     /*!< 0x00020000 */
#define RCC_APBENR2_TIM16EN              RCC_APBENR2_TIM16EN_MSK
#define RCC_APBENR2_TIM17EN_POS          (18U)
#define RCC_APBENR2_TIM17EN_MSK          (0x1UL << RCC_APBENR2_TIM17EN_POS)     /*!< 0x00040000 */
#define RCC_APBENR2_TIM17EN              RCC_APBENR2_TIM17EN_MSK
#define RCC_APBENR2_ADCEN_POS            (20U)
#define RCC_APBENR2_ADCEN_MSK            (0x1UL << RCC_APBENR2_ADCEN_POS)       /*!< 0x00100000 */
#define RCC_APBENR2_ADCEN                RCC_APBENR2_ADCEN_MSK

/********************  Bit definition for RCC_IOPSMENR register  *************/
#define RCC_IOPSMENR_GPIOASMEN_POS       (0U)
#define RCC_IOPSMENR_GPIOASMEN_MSK       (0x1UL << RCC_IOPSMENR_GPIOASMEN_POS)  /*!< 0x00000001 */
#define RCC_IOPSMENR_GPIOASMEN           RCC_IOPSMENR_GPIOASMEN_MSK
#define RCC_IOPSMENR_GPIOBSMEN_POS       (1U)
#define RCC_IOPSMENR_GPIOBSMEN_MSK       (0x1UL << RCC_IOPSMENR_GPIOBSMEN_POS)  /*!< 0x00000002 */
#define RCC_IOPSMENR_GPIOBSMEN           RCC_IOPSMENR_GPIOBSMEN_MSK
#define RCC_IOPSMENR_GPIOCSMEN_POS       (2U)
#define RCC_IOPSMENR_GPIOCSMEN_MSK       (0x1UL << RCC_IOPSMENR_GPIOCSMEN_POS)  /*!< 0x00000004 */
#define RCC_IOPSMENR_GPIOCSMEN           RCC_IOPSMENR_GPIOCSMEN_MSK
#define RCC_IOPSMENR_GPIODSMEN_POS       (3U)
#define RCC_IOPSMENR_GPIODSMEN_MSK       (0x1UL << RCC_IOPSMENR_GPIODSMEN_POS)  /*!< 0x00000008 */
#define RCC_IOPSMENR_GPIODSMEN           RCC_IOPSMENR_GPIODSMEN_MSK
#define RCC_IOPSMENR_GPIOESMEN_POS       (4U)
#define RCC_IOPSMENR_GPIOESMEN_MSK       (0x1UL << RCC_IOPSMENR_GPIOESMEN_POS)  /*!< 0x00000010 */
#define RCC_IOPSMENR_GPIOESMEN           RCC_IOPSMENR_GPIOESMEN_MSK
#define RCC_IOPSMENR_GPIOFSMEN_POS       (5U)
#define RCC_IOPSMENR_GPIOFSMEN_MSK       (0x1UL << RCC_IOPSMENR_GPIOFSMEN_POS)  /*!< 0x00000020 */
#define RCC_IOPSMENR_GPIOFSMEN           RCC_IOPSMENR_GPIOFSMEN_MSK

/********************  Bit definition for RCC_AHBSMENR register  *************/
#define RCC_AHBSMENR_DMA1SMEN_POS        (0U)
#define RCC_AHBSMENR_DMA1SMEN_MSK        (0x1UL << RCC_AHBSMENR_DMA1SMEN_POS)   /*!< 0x00000001 */
#define RCC_AHBSMENR_DMA1SMEN            RCC_AHBSMENR_DMA1SMEN_MSK
#define RCC_AHBSMENR_DMA2SMEN_POS        (1U)
#define RCC_AHBSMENR_DMA2SMEN_MSK        (0x1UL << RCC_AHBSMENR_DMA2SMEN_POS)   /*!< 0x00000002 */
#define RCC_AHBSMENR_DMA2SMEN            RCC_AHBSMENR_DMA2SMEN_MSK
#define RCC_AHBSMENR_FLASHSMEN_POS       (8U)
#define RCC_AHBSMENR_FLASHSMEN_MSK       (0x1UL << RCC_AHBSMENR_FLASHSMEN_POS)  /*!< 0x00000100 */
#define RCC_AHBSMENR_FLASHSMEN           RCC_AHBSMENR_FLASHSMEN_MSK
#define RCC_AHBSMENR_SRAMSMEN_POS        (9U)
#define RCC_AHBSMENR_SRAMSMEN_MSK        (0x1UL << RCC_AHBSMENR_SRAMSMEN_POS)   /*!< 0x00000200 */
#define RCC_AHBSMENR_SRAMSMEN            RCC_AHBSMENR_SRAMSMEN_MSK
#define RCC_AHBSMENR_CRCSMEN_POS         (12U)
#define RCC_AHBSMENR_CRCSMEN_MSK         (0x1UL << RCC_AHBSMENR_CRCSMEN_POS)    /*!< 0x00001000 */
#define RCC_AHBSMENR_CRCSMEN             RCC_AHBSMENR_CRCSMEN_MSK

/********************  Bit definition for RCC_APBSMENR1 register  *************/
#define RCC_APBSMENR1_TIM2SMEN_POS       (0U)
#define RCC_APBSMENR1_TIM2SMEN_MSK       (0x1UL << RCC_APBSMENR1_TIM2SMEN_POS)  /*!< 0x00000001 */
#define RCC_APBSMENR1_TIM2SMEN           RCC_APBSMENR1_TIM2SMEN_MSK
#define RCC_APBSMENR1_TIM3SMEN_POS       (1U)
#define RCC_APBSMENR1_TIM3SMEN_MSK       (0x1UL << RCC_APBSMENR1_TIM3SMEN_POS)  /*!< 0x00000002 */
#define RCC_APBSMENR1_TIM3SMEN           RCC_APBSMENR1_TIM3SMEN_MSK
#define RCC_APBSMENR1_TIM4SMEN_POS       (2U)
#define RCC_APBSMENR1_TIM4SMEN_MSK       (0x1UL << RCC_APBSMENR1_TIM4SMEN_POS)  /*!< 0x00000004 */
#define RCC_APBSMENR1_TIM4SMEN           RCC_APBSMENR1_TIM4SMEN_MSK
#define RCC_APBSMENR1_TIM6SMEN_POS       (4U)
#define RCC_APBSMENR1_TIM6SMEN_MSK       (0x1UL << RCC_APBSMENR1_TIM6SMEN_POS)  /*!< 0x00000010 */
#define RCC_APBSMENR1_TIM6SMEN           RCC_APBSMENR1_TIM6SMEN_MSK
#define RCC_APBSMENR1_TIM7SMEN_POS       (5U)
#define RCC_APBSMENR1_TIM7SMEN_MSK       (0x1UL << RCC_APBSMENR1_TIM7SMEN_POS)  /*!< 0x00000020 */
#define RCC_APBSMENR1_TIM7SMEN           RCC_APBSMENR1_TIM7SMEN_MSK
#define RCC_APBSMENR1_LPUART2SMEN_POS    (7U)
#define RCC_APBSMENR1_LPUART2SMEN_MSK    (0x1UL << RCC_APBSMENR1_LPUART2SMEN_POS)/*!< 0x00000080 */
#define RCC_APBSMENR1_LPUART2SMEN        RCC_APBSMENR1_LPUART2SMEN_MSK
#define RCC_APBSMENR1_USART5SMEN_POS     (8U)
#define RCC_APBSMENR1_USART5SMEN_MSK     (0x1UL << RCC_APBSMENR1_USART5SMEN_POS) /*!< 0x00000100 */
#define RCC_APBSMENR1_USART5SMEN         RCC_APBSMENR1_USART5SMEN_MSK
#define RCC_APBSMENR1_USART6SMEN_POS     (9U)
#define RCC_APBSMENR1_USART6SMEN_MSK     (0x1UL << RCC_APBSMENR1_USART6SMEN_POS) /*!< 0x00000200 */
#define RCC_APBSMENR1_USART6SMEN         RCC_APBSMENR1_USART6SMEN_MSK
#define RCC_APBSMENR1_RTCAPBSMEN_POS     (10U)
#define RCC_APBSMENR1_RTCAPBSMEN_MSK     (0x1UL << RCC_APBSMENR1_RTCAPBSMEN_POS) /*!< 0x00000400 */
#define RCC_APBSMENR1_RTCAPBSMEN         RCC_APBSMENR1_RTCAPBSMEN_MSK
#define RCC_APBSMENR1_WWDGSMEN_POS       (11U)
#define RCC_APBSMENR1_WWDGSMEN_MSK       (0x1UL << RCC_APBSMENR1_WWDGSMEN_POS)   /*!< 0x00000800 */
#define RCC_APBSMENR1_WWDGSMEN           RCC_APBSMENR1_WWDGSMEN_MSK
#define RCC_APBSMENR1_FDCANSMEN_POS      (12U)
#define RCC_APBSMENR1_FDCANSMEN_MSK      (0x1UL << RCC_APBSMENR1_FDCANSMEN_POS)  /*!< 0x00001000 */
#define RCC_APBSMENR1_FDCANSMEN          RCC_APBSMENR1_FDCANSMEN_MSK
#define RCC_APBSMENR1_USBSMEN_POS        (13U)
#define RCC_APBSMENR1_USBSMEN_MSK        (0x1UL << RCC_APBSMENR1_USBSMEN_POS)    /*!< 0x00002000 */
#define RCC_APBSMENR1_USBSMEN            RCC_APBSMENR1_USBSMEN_MSK
#define RCC_APBSMENR1_SPI2SMEN_POS       (14U)
#define RCC_APBSMENR1_SPI2SMEN_MSK       (0x1UL << RCC_APBSMENR1_SPI2SMEN_POS)   /*!< 0x00004000 */
#define RCC_APBSMENR1_SPI2SMEN           RCC_APBSMENR1_SPI2SMEN_MSK
#define RCC_APBSMENR1_SPI3SMEN_POS       (15U)
#define RCC_APBSMENR1_SPI3SMEN_MSK       (0x1UL << RCC_APBSMENR1_SPI3SMEN_POS)   /*!< 0x00008000 */
#define RCC_APBSMENR1_SPI3SMEN           RCC_APBSMENR1_SPI3SMEN_MSK
#define RCC_APBSMENR1_CRSSMEN_POS        (16U)
#define RCC_APBSMENR1_CRSSMEN_MSK        (0x1UL << RCC_APBSMENR1_CRSSMEN_POS)   /*!< 0x00010000 */
#define RCC_APBSMENR1_CRSSMEN            RCC_APBSMENR1_CRSSMEN_MSK
#define RCC_APBSMENR1_USART2SMEN_POS     (17U)
#define RCC_APBSMENR1_USART2SMEN_MSK     (0x1UL << RCC_APBSMENR1_USART2SMEN_POS) /*!< 0x00020000 */
#define RCC_APBSMENR1_USART2SMEN         RCC_APBSMENR1_USART2SMEN_MSK
#define RCC_APBSMENR1_USART3SMEN_POS     (18U)
#define RCC_APBSMENR1_USART3SMEN_MSK     (0x1UL << RCC_APBSMENR1_USART3SMEN_POS) /*!< 0x00040000 */
#define RCC_APBSMENR1_USART3SMEN         RCC_APBSMENR1_USART3SMEN_MSK
#define RCC_APBSMENR1_USART4SMEN_POS     (19U)
#define RCC_APBSMENR1_USART4SMEN_MSK     (0x1UL << RCC_APBSMENR1_USART4SMEN_POS) /*!< 0x00080000 */
#define RCC_APBSMENR1_USART4SMEN         RCC_APBSMENR1_USART4SMEN_MSK
#define RCC_APBSMENR1_LPUART1SMEN_POS    (20U)
#define RCC_APBSMENR1_LPUART1SMEN_MSK    (0x1UL << RCC_APBSMENR1_LPUART1SMEN_POS) /*!< 0x00100000 */
#define RCC_APBSMENR1_LPUART1SMEN        RCC_APBSMENR1_LPUART1SMEN_MSK
#define RCC_APBSMENR1_I2C1SMEN_POS       (21U)
#define RCC_APBSMENR1_I2C1SMEN_MSK       (0x1UL << RCC_APBSMENR1_I2C1SMEN_POS)   /*!< 0x00200000 */
#define RCC_APBSMENR1_I2C1SMEN           RCC_APBSMENR1_I2C1SMEN_MSK
#define RCC_APBSMENR1_I2C2SMEN_POS       (22U)
#define RCC_APBSMENR1_I2C2SMEN_MSK       (0x1UL << RCC_APBSMENR1_I2C2SMEN_POS)   /*!< 0x00400000 */
#define RCC_APBSMENR1_I2C2SMEN           RCC_APBSMENR1_I2C2SMEN_MSK
#define RCC_APBSMENR1_I2C3SMEN_POS       (23U)
#define RCC_APBSMENR1_I2C3SMEN_MSK       (0x1UL << RCC_APBSMENR1_I2C3SMEN_POS)   /*!< 0x00800000 */
#define RCC_APBSMENR1_I2C3SMEN           RCC_APBSMENR1_I2C3SMEN_MSK
#define RCC_APBSMENR1_CECSMEN_POS        (24U)
#define RCC_APBSMENR1_CECSMEN_MSK        (0x1UL << RCC_APBSMENR1_CECSMEN_POS)    /*!< 0x01000000 */
#define RCC_APBSMENR1_CECSMEN            RCC_APBSMENR1_CECSMEN_MSK
#define RCC_APBSMENR1_UCPD1SMEN_POS      (25U)
#define RCC_APBSMENR1_UCPD1SMEN_MSK      (0x1UL << RCC_APBSMENR1_UCPD1SMEN_POS)  /*!< 0x02000000 */
#define RCC_APBSMENR1_UCPD1SMEN          RCC_APBSMENR1_UCPD1SMEN_MSK
#define RCC_APBSMENR1_UCPD2SMEN_POS      (26U)
#define RCC_APBSMENR1_UCPD2SMEN_MSK      (0x1UL << RCC_APBSMENR1_UCPD2SMEN_POS)  /*!< 0x04000000 */
#define RCC_APBSMENR1_UCPD2SMEN          RCC_APBSMENR1_UCPD2SMEN_MSK
#define RCC_APBSMENR1_DBGSMEN_POS        (27U)
#define RCC_APBSMENR1_DBGSMEN_MSK        (0x1UL << RCC_APBSMENR1_DBGSMEN_POS)    /*!< 0x08000000 */
#define RCC_APBSMENR1_DBGSMEN            RCC_APBSMENR1_DBGSMEN_MSK
#define RCC_APBSMENR1_PWRSMEN_POS        (28U)
#define RCC_APBSMENR1_PWRSMEN_MSK        (0x1UL << RCC_APBSMENR1_PWRSMEN_POS)    /*!< 0x10000000 */
#define RCC_APBSMENR1_PWRSMEN            RCC_APBSMENR1_PWRSMEN_MSK
#define RCC_APBSMENR1_DAC1SMEN_POS       (29U)
#define RCC_APBSMENR1_DAC1SMEN_MSK       (0x1UL << RCC_APBSMENR1_DAC1SMEN_POS)   /*!< 0x20000000 */
#define RCC_APBSMENR1_DAC1SMEN           RCC_APBSMENR1_DAC1SMEN_MSK
#define RCC_APBSMENR1_LPTIM2SMEN_POS     (30U)
#define RCC_APBSMENR1_LPTIM2SMEN_MSK     (0x1UL << RCC_APBSMENR1_LPTIM2SMEN_POS) /*!< 0x40000000 */
#define RCC_APBSMENR1_LPTIM2SMEN         RCC_APBSMENR1_LPTIM2SMEN_MSK
#define RCC_APBSMENR1_LPTIM1SMEN_POS     (31U)
#define RCC_APBSMENR1_LPTIM1SMEN_MSK     (0x1UL << RCC_APBSMENR1_LPTIM1SMEN_POS) /*!< 0x80000000 */
#define RCC_APBSMENR1_LPTIM1SMEN         RCC_APBSMENR1_LPTIM1SMEN_MSK

/********************  Bit definition for RCC_APBSMENR2 register  *************/
#define RCC_APBSMENR2_SYSCFGSMEN_POS     (0U)
#define RCC_APBSMENR2_SYSCFGSMEN_MSK     (0x1UL << RCC_APBSMENR2_SYSCFGSMEN_POS) /*!< 0x00000001 */
#define RCC_APBSMENR2_SYSCFGSMEN         RCC_APBSMENR2_SYSCFGSMEN_MSK
#define RCC_APBSMENR2_TIM1SMEN_POS       (11U)
#define RCC_APBSMENR2_TIM1SMEN_MSK       (0x1UL << RCC_APBSMENR2_TIM1SMEN_POS)  /*!< 0x00000800 */
#define RCC_APBSMENR2_TIM1SMEN           RCC_APBSMENR2_TIM1SMEN_MSK
#define RCC_APBSMENR2_SPI1SMEN_POS       (12U)
#define RCC_APBSMENR2_SPI1SMEN_MSK       (0x1UL << RCC_APBSMENR2_SPI1SMEN_POS)  /*!< 0x00001000 */
#define RCC_APBSMENR2_SPI1SMEN           RCC_APBSMENR2_SPI1SMEN_MSK
#define RCC_APBSMENR2_USART1SMEN_POS     (14U)
#define RCC_APBSMENR2_USART1SMEN_MSK     (0x1UL << RCC_APBSMENR2_USART1SMEN_POS) /*!< 0x00004000 */
#define RCC_APBSMENR2_USART1SMEN         RCC_APBSMENR2_USART1SMEN_MSK
#define RCC_APBSMENR2_TIM14SMEN_POS      (15U)
#define RCC_APBSMENR2_TIM14SMEN_MSK      (0x1UL << RCC_APBSMENR2_TIM14SMEN_POS) /*!< 0x00008000 */
#define RCC_APBSMENR2_TIM14SMEN          RCC_APBSMENR2_TIM14SMEN_MSK
#define RCC_APBSMENR2_TIM15SMEN_POS      (16U)
#define RCC_APBSMENR2_TIM15SMEN_MSK      (0x1UL << RCC_APBSMENR2_TIM15SMEN_POS) /*!< 0x00010000 */
#define RCC_APBSMENR2_TIM15SMEN          RCC_APBSMENR2_TIM15SMEN_MSK
#define RCC_APBSMENR2_TIM16SMEN_POS      (17U)
#define RCC_APBSMENR2_TIM16SMEN_MSK      (0x1UL << RCC_APBSMENR2_TIM16SMEN_POS) /*!< 0x00020000 */
#define RCC_APBSMENR2_TIM16SMEN          RCC_APBSMENR2_TIM16SMEN_MSK
#define RCC_APBSMENR2_TIM17SMEN_POS      (18U)
#define RCC_APBSMENR2_TIM17SMEN_MSK      (0x1UL << RCC_APBSMENR2_TIM17SMEN_POS) /*!< 0x00040000 */
#define RCC_APBSMENR2_TIM17SMEN          RCC_APBSMENR2_TIM17SMEN_MSK
#define RCC_APBSMENR2_ADCSMEN_POS        (20U)
#define RCC_APBSMENR2_ADCSMEN_MSK        (0x1UL << RCC_APBSMENR2_ADCSMEN_POS)   /*!< 0x00100000 */
#define RCC_APBSMENR2_ADCSMEN            RCC_APBSMENR2_ADCSMEN_MSK

/********************  Bit definition for RCC_CCIPR register  ******************/
#define RCC_CCIPR_USART1SEL_POS          (0U)
#define RCC_CCIPR_USART1SEL_MSK          (0x3UL << RCC_CCIPR_USART1SEL_POS)     /*!< 0x00000003 */
#define RCC_CCIPR_USART1SEL              RCC_CCIPR_USART1SEL_MSK
#define RCC_CCIPR_USART1SEL_0            (0x1UL << RCC_CCIPR_USART1SEL_POS)     /*!< 0x00000001 */
#define RCC_CCIPR_USART1SEL_1            (0x2UL << RCC_CCIPR_USART1SEL_POS)     /*!< 0x00000002 */

#define RCC_CCIPR_USART2SEL_POS          (2U)
#define RCC_CCIPR_USART2SEL_MSK          (0x3UL << RCC_CCIPR_USART2SEL_POS)     /*!< 0x0000000C */
#define RCC_CCIPR_USART2SEL              RCC_CCIPR_USART2SEL_MSK
#define RCC_CCIPR_USART2SEL_0            (0x1UL << RCC_CCIPR_USART2SEL_POS)     /*!< 0x00000004 */
#define RCC_CCIPR_USART2SEL_1            (0x2UL << RCC_CCIPR_USART2SEL_POS)     /*!< 0x00000008 */

#define RCC_CCIPR_USART3SEL_POS          (4U)
#define RCC_CCIPR_USART3SEL_MSK          (0x3UL << RCC_CCIPR_USART3SEL_POS)     /*!< 0x00000030 */
#define RCC_CCIPR_USART3SEL              RCC_CCIPR_USART3SEL_MSK
#define RCC_CCIPR_USART3SEL_0            (0x1UL << RCC_CCIPR_USART3SEL_POS)     /*!< 0x00000010 */
#define RCC_CCIPR_USART3SEL_1            (0x2UL << RCC_CCIPR_USART3SEL_POS)     /*!< 0x00000020 */
#define RCC_CCIPR_CECSEL_POS             (6U)
#define RCC_CCIPR_CECSEL_MSK             (0x1UL << RCC_CCIPR_CECSEL_POS)        /*!< 0x00000040 */
#define RCC_CCIPR_CECSEL                 RCC_CCIPR_CECSEL_MSK

#define RCC_CCIPR_LPUART2SEL_POS         (8U)
#define RCC_CCIPR_LPUART2SEL_MSK         (0x3UL << RCC_CCIPR_LPUART2SEL_POS)    /*!< 0x00000300 */
#define RCC_CCIPR_LPUART2SEL             RCC_CCIPR_LPUART2SEL_MSK
#define RCC_CCIPR_LPUART2SEL_0           (0x1UL << RCC_CCIPR_LPUART2SEL_POS)    /*!< 0x00000100 */
#define RCC_CCIPR_LPUART2SEL_1           (0x2UL << RCC_CCIPR_LPUART2SEL_POS)    /*!< 0x00000200 */
#define RCC_CCIPR_LPUART1SEL_POS         (10U)
#define RCC_CCIPR_LPUART1SEL_MSK         (0x3UL << RCC_CCIPR_LPUART1SEL_POS)    /*!< 0x00000C00 */
#define RCC_CCIPR_LPUART1SEL             RCC_CCIPR_LPUART1SEL_MSK
#define RCC_CCIPR_LPUART1SEL_0           (0x1UL << RCC_CCIPR_LPUART1SEL_POS)    /*!< 0x00000400 */
#define RCC_CCIPR_LPUART1SEL_1           (0x2UL << RCC_CCIPR_LPUART1SEL_POS)    /*!< 0x00000800 */

#define RCC_CCIPR_I2C1SEL_POS            (12U)
#define RCC_CCIPR_I2C1SEL_MSK            (0x3UL << RCC_CCIPR_I2C1SEL_POS)       /*!< 0x00003000 */
#define RCC_CCIPR_I2C1SEL                RCC_CCIPR_I2C1SEL_MSK
#define RCC_CCIPR_I2C1SEL_0              (0x1UL << RCC_CCIPR_I2C1SEL_POS)       /*!< 0x00001000 */
#define RCC_CCIPR_I2C1SEL_1              (0x2UL << RCC_CCIPR_I2C1SEL_POS)       /*!< 0x00002000 */

#define RCC_CCIPR_I2C2SEL_POS           (14U)
#define RCC_CCIPR_I2C2SEL_MSK           (0x3UL << RCC_CCIPR_I2C2SEL_POS)        /*!< 0x0000C000 */
#define RCC_CCIPR_I2C2SEL               RCC_CCIPR_I2C2SEL_MSK
#define RCC_CCIPR_I2C2SEL_0             (0x1UL << RCC_CCIPR_I2C2SEL_POS)        /*!< 0x00004000 */
#define RCC_CCIPR_I2C2SEL_1             (0x2UL << RCC_CCIPR_I2C2SEL_POS)        /*!< 0x00008000 */

#define RCC_CCIPR_LPTIM1SEL_POS          (18U)
#define RCC_CCIPR_LPTIM1SEL_MSK          (0x3UL << RCC_CCIPR_LPTIM1SEL_POS)     /*!< 0x000C0000 */
#define RCC_CCIPR_LPTIM1SEL              RCC_CCIPR_LPTIM1SEL_MSK
#define RCC_CCIPR_LPTIM1SEL_0            (0x1UL << RCC_CCIPR_LPTIM1SEL_POS)     /*!< 0x00040000 */
#define RCC_CCIPR_LPTIM1SEL_1            (0x2UL << RCC_CCIPR_LPTIM1SEL_POS)     /*!< 0x00080000 */

#define RCC_CCIPR_LPTIM2SEL_POS          (20U)
#define RCC_CCIPR_LPTIM2SEL_MSK          (0x3UL << RCC_CCIPR_LPTIM2SEL_POS)     /*!< 0x00300000 */
#define RCC_CCIPR_LPTIM2SEL              RCC_CCIPR_LPTIM2SEL_MSK
#define RCC_CCIPR_LPTIM2SEL_0            (0x1UL << RCC_CCIPR_LPTIM2SEL_POS)     /*!< 0x00100000 */
#define RCC_CCIPR_LPTIM2SEL_1            (0x2UL << RCC_CCIPR_LPTIM2SEL_POS)     /*!< 0x00200000 */

#define RCC_CCIPR_TIM1SEL_POS            (22U)
#define RCC_CCIPR_TIM1SEL_MSK            (0x1UL << RCC_CCIPR_TIM1SEL_POS)       /*!< 0x00400000 */
#define RCC_CCIPR_TIM1SEL                RCC_CCIPR_TIM1SEL_MSK

#define RCC_CCIPR_TIM15SEL_POS           (24U)
#define RCC_CCIPR_TIM15SEL_MSK           (0x1UL << RCC_CCIPR_TIM15SEL_POS)      /*!< 0x01000000 */
#define RCC_CCIPR_TIM15SEL               RCC_CCIPR_TIM15SEL_MSK


#define RCC_CCIPR_ADCSEL_POS             (30U)
#define RCC_CCIPR_ADCSEL_MSK             (0x3UL << RCC_CCIPR_ADCSEL_POS)        /*!< 0xC0000000 */
#define RCC_CCIPR_ADCSEL                 RCC_CCIPR_ADCSEL_MSK
#define RCC_CCIPR_ADCSEL_0               (0x1UL << RCC_CCIPR_ADCSEL_POS)        /*!< 0x40000000 */
#define RCC_CCIPR_ADCSEL_1               (0x2UL << RCC_CCIPR_ADCSEL_POS)        /*!< 0x80000000 */

/********************  Bit definition for RCC_CCIPR2 register  ****************/
#define RCC_CCIPR2_I2S1SEL_POS            (0U)
#define RCC_CCIPR2_I2S1SEL_MSK            (0x3UL << RCC_CCIPR2_I2S1SEL_POS)     /*!< 0x00000003 */
#define RCC_CCIPR2_I2S1SEL                RCC_CCIPR2_I2S1SEL_MSK
#define RCC_CCIPR2_I2S1SEL_0              (0x1UL << RCC_CCIPR2_I2S1SEL_POS)     /*!< 0x00000001 */
#define RCC_CCIPR2_I2S1SEL_1              (0x2UL << RCC_CCIPR2_I2S1SEL_POS)     /*!< 0x00000002 */
#define RCC_CCIPR2_I2S2SEL_POS            (2U)
#define RCC_CCIPR2_I2S2SEL_MSK            (0x3UL << RCC_CCIPR2_I2S2SEL_POS)     /*!< 0x0000000C */
#define RCC_CCIPR2_I2S2SEL                RCC_CCIPR2_I2S2SEL_MSK
#define RCC_CCIPR2_I2S2SEL_0              (0x1UL << RCC_CCIPR2_I2S2SEL_POS)     /*!< 0x00000004 */
#define RCC_CCIPR2_I2S2SEL_1              (0x2UL << RCC_CCIPR2_I2S2SEL_POS)     /*!< 0x00000008 */
#define RCC_CCIPR2_FDCANSEL_POS           (8U)
#define RCC_CCIPR2_FDCANSEL_MSK           (0x3UL << RCC_CCIPR2_FDCANSEL_POS)    /*!< 0x00000300 */
#define RCC_CCIPR2_FDCANSEL               RCC_CCIPR2_FDCANSEL_MSK
#define RCC_CCIPR2_FDCANSEL_0             (0x1UL << RCC_CCIPR2_FDCANSEL_POS)    /*!< 0x00000100 */
#define RCC_CCIPR2_FDCANSEL_1             (0x2UL << RCC_CCIPR2_FDCANSEL_POS)    /*!< 0x00000200 */
#define RCC_CCIPR2_USBSEL_POS             (12U)
#define RCC_CCIPR2_USBSEL_MSK             (0x3UL << RCC_CCIPR2_USBSEL_POS)     /*!< 0x00003000 */
#define RCC_CCIPR2_USBSEL                 RCC_CCIPR2_USBSEL_MSK
#define RCC_CCIPR2_USBSEL_0               (0x1UL << RCC_CCIPR2_USBSEL_POS)      /*!< 0x00001000 */
#define RCC_CCIPR2_USBSEL_1               (0x2UL << RCC_CCIPR2_USBSEL_POS)      /*!< 0x00002000 */
/********************  Bit definition for RCC_BDCR register  ******************/
#define RCC_BDCR_LSEON_POS               (0U)
#define RCC_BDCR_LSEON_MSK               (0x1UL << RCC_BDCR_LSEON_POS)          /*!< 0x00000001 */
#define RCC_BDCR_LSEON                   RCC_BDCR_LSEON_MSK
#define RCC_BDCR_LSERDY_POS              (1U)
#define RCC_BDCR_LSERDY_MSK              (0x1UL << RCC_BDCR_LSERDY_POS)         /*!< 0x00000002 */
#define RCC_BDCR_LSERDY                  RCC_BDCR_LSERDY_MSK
#define RCC_BDCR_LSEBYP_POS              (2U)
#define RCC_BDCR_LSEBYP_MSK              (0x1UL << RCC_BDCR_LSEBYP_POS)         /*!< 0x00000004 */
#define RCC_BDCR_LSEBYP                  RCC_BDCR_LSEBYP_MSK

#define RCC_BDCR_LSEDRV_POS              (3U)
#define RCC_BDCR_LSEDRV_MSK              (0x3UL << RCC_BDCR_LSEDRV_POS)         /*!< 0x00000018 */
#define RCC_BDCR_LSEDRV                  RCC_BDCR_LSEDRV_MSK
#define RCC_BDCR_LSEDRV_0                (0x1UL << RCC_BDCR_LSEDRV_POS)         /*!< 0x00000008 */
#define RCC_BDCR_LSEDRV_1                (0x2UL << RCC_BDCR_LSEDRV_POS)         /*!< 0x00000010 */

#define RCC_BDCR_LSECSSON_POS            (5U)
#define RCC_BDCR_LSECSSON_MSK            (0x1UL << RCC_BDCR_LSECSSON_POS)       /*!< 0x00000020 */
#define RCC_BDCR_LSECSSON                RCC_BDCR_LSECSSON_MSK
#define RCC_BDCR_LSECSSD_POS             (6U)
#define RCC_BDCR_LSECSSD_MSK             (0x1UL << RCC_BDCR_LSECSSD_POS)        /*!< 0x00000040 */
#define RCC_BDCR_LSECSSD                 RCC_BDCR_LSECSSD_MSK

#define RCC_BDCR_RTCSEL_POS              (8U)
#define RCC_BDCR_RTCSEL_MSK              (0x3UL << RCC_BDCR_RTCSEL_POS)         /*!< 0x00000300 */
#define RCC_BDCR_RTCSEL                  RCC_BDCR_RTCSEL_MSK
#define RCC_BDCR_RTCSEL_0                (0x1UL << RCC_BDCR_RTCSEL_POS)         /*!< 0x00000100 */
#define RCC_BDCR_RTCSEL_1                (0x2UL << RCC_BDCR_RTCSEL_POS)         /*!< 0x00000200 */

#define  RCC_BDCR_RTCSEL_NOCLOCK         ((uint32)0x00000000)        //! No clock
#define  RCC_BDCR_RTCSEL_LSE             ((uint32)0x00000100)        //! LSE oscillator clock used as RTC clock
#define  RCC_BDCR_RTCSEL_LSI             ((uint32)0x00000200)        //! LSI oscillator clock used as RTC clock
#define  RCC_BDCR_RTCSEL_HSE             ((uint32)0x00000300)        //! HSE oscillator clock divided by 128 used as RTC clock

#define RCC_BDCR_RTCEN_POS               (15U)
#define RCC_BDCR_RTCEN_MSK               (0x1UL << RCC_BDCR_RTCEN_POS)          /*!< 0x00008000 */
#define RCC_BDCR_RTCEN                   RCC_BDCR_RTCEN_MSK
#define RCC_BDCR_BDRST_POS               (16U)
#define RCC_BDCR_BDRST_MSK               (0x1UL << RCC_BDCR_BDRST_POS)          /*!< 0x00010000 */
#define RCC_BDCR_BDRST                   RCC_BDCR_BDRST_MSK

#define RCC_BDCR_LSCOEN_POS              (24U)
#define RCC_BDCR_LSCOEN_MSK              (0x1UL << RCC_BDCR_LSCOEN_POS)         /*!< 0x01000000 */
#define RCC_BDCR_LSCOEN                  RCC_BDCR_LSCOEN_MSK
#define RCC_BDCR_LSCOSEL_POS             (25U)
#define RCC_BDCR_LSCOSEL_MSK             (0x1UL << RCC_BDCR_LSCOSEL_POS)        /*!< 0x02000000 */
#define RCC_BDCR_LSCOSEL                 RCC_BDCR_LSCOSEL_MSK

/********************  Bit definition for RCC_CSR register  *******************/
#define RCC_CSR_LSION_POS                (0U)
#define RCC_CSR_LSION_MSK                (0x1UL << RCC_CSR_LSION_POS)           /*!< 0x00000001 */
#define RCC_CSR_LSION                    RCC_CSR_LSION_MSK
#define RCC_CSR_LSIRDY_POS               (1U)
#define RCC_CSR_LSIRDY_MSK               (0x1UL << RCC_CSR_LSIRDY_POS)          /*!< 0x00000002 */
#define RCC_CSR_LSIRDY                   RCC_CSR_LSIRDY_MSK

#define RCC_CSR_RMVF_POS                 (23U)
#define RCC_CSR_RMVF_MSK                 (0x1UL << RCC_CSR_RMVF_POS)            /*!< 0x00800000 */
#define RCC_CSR_RMVF                     RCC_CSR_RMVF_MSK
#define RCC_CSR_OBLRSTF_POS              (25U)
#define RCC_CSR_OBLRSTF_MSK              (0x1UL << RCC_CSR_OBLRSTF_POS)         /*!< 0x02000000 */
#define RCC_CSR_OBLRSTF                  RCC_CSR_OBLRSTF_MSK
#define RCC_CSR_PINRSTF_POS              (26U)
#define RCC_CSR_PINRSTF_MSK              (0x1UL << RCC_CSR_PINRSTF_POS)         /*!< 0x04000000 */
#define RCC_CSR_PINRSTF                  RCC_CSR_PINRSTF_MSK
#define RCC_CSR_PWRRSTF_POS              (27U)
#define RCC_CSR_PWRRSTF_MSK              (0x1UL << RCC_CSR_PWRRSTF_POS)         /*!< 0x08000000 */
#define RCC_CSR_PWRRSTF                  RCC_CSR_PWRRSTF_MSK
#define RCC_CSR_SFTRSTF_POS              (28U)
#define RCC_CSR_SFTRSTF_MSK              (0x1UL << RCC_CSR_SFTRSTF_POS)         /*!< 0x10000000 */
#define RCC_CSR_SFTRSTF                  RCC_CSR_SFTRSTF_MSK
#define RCC_CSR_IWDGRSTF_POS             (29U)
#define RCC_CSR_IWDGRSTF_MSK             (0x1UL << RCC_CSR_IWDGRSTF_POS)        /*!< 0x20000000 */
#define RCC_CSR_IWDGRSTF                 RCC_CSR_IWDGRSTF_MSK
#define RCC_CSR_WWDGRSTF_POS             (30U)
#define RCC_CSR_WWDGRSTF_MSK             (0x1UL << RCC_CSR_WWDGRSTF_POS)        /*!< 0x40000000 */
#define RCC_CSR_WWDGRSTF                 RCC_CSR_WWDGRSTF_MSK
#define RCC_CSR_LPWRRSTF_POS             (31U)
#define RCC_CSR_LPWRRSTF_MSK             (0x1UL << RCC_CSR_LPWRRSTF_POS)        /*!< 0x80000000 */
#define RCC_CSR_LPWRRSTF                 RCC_CSR_LPWRRSTF_MSK


//Useful macros
#define RCC_CLOCK_ENABLE_FOR_ALL_GPIO_PORTS()   ( RCC.IOPENR |= (RCC_IOPENR_GPIOAEN | \
                                                                RCC_IOPENR_GPIOBEN | \
                                                                RCC_IOPENR_GPIOCEN | \
                                                                RCC_IOPENR_GPIODEN | \
                                                                RCC_IOPENR_GPIOEEN | \
                                                                RCC_IOPENR_GPIOFEN) )


#define LL_RCC_USART1_CLKSOURCE_PCLK1      ((RCC_CCIPR_USART1SEL << 16U) | 0x00000000U)            /*!< PCLK1 clock used as USART1 clock source */
#define LL_RCC_USART1_CLKSOURCE_SYSCLK     ((RCC_CCIPR_USART1SEL << 16U) | RCC_CCIPR_USART1SEL_0)  /*!< SYSCLK clock used as USART1 clock source */
#define LL_RCC_USART1_CLKSOURCE_HSI        ((RCC_CCIPR_USART1SEL << 16U) | RCC_CCIPR_USART1SEL_1)  /*!< HSI clock used as USART1 clock source */
#define LL_RCC_USART1_CLKSOURCE_LSE        ((RCC_CCIPR_USART1SEL << 16U) | RCC_CCIPR_USART1SEL)    /*!< LSE clock used as USART1 clock source */

#define LL_RCC_USART2_CLKSOURCE_PCLK1      ((RCC_CCIPR_USART2SEL << 16U) | 0x00000000U)            /*!< PCLK1 clock used as USART2 clock source */
#define LL_RCC_USART2_CLKSOURCE_SYSCLK     ((RCC_CCIPR_USART2SEL << 16U) | RCC_CCIPR_USART2SEL_0)  /*!< SYSCLK clock used as USART2 clock source */
#define LL_RCC_USART2_CLKSOURCE_HSI        ((RCC_CCIPR_USART2SEL << 16U) | RCC_CCIPR_USART2SEL_1)  /*!< HSI clock used as USART2 clock source */
#define LL_RCC_USART2_CLKSOURCE_LSE        ((RCC_CCIPR_USART2SEL << 16U) | RCC_CCIPR_USART2SEL)    /*!< LSE clock used as USART2 clock source */

#define LL_RCC_USART3_CLKSOURCE_PCLK1      ((RCC_CCIPR_USART3SEL << 16U) | 0x00000000U)           /*!< PCLK1 clock used as USART3 clock source */
#define LL_RCC_USART3_CLKSOURCE_SYSCLK     ((RCC_CCIPR_USART3SEL << 16U) | RCC_CCIPR_USART3SEL_0) /*!< SYSCLK clock used as USART3 clock source */
#define LL_RCC_USART3_CLKSOURCE_HSI        ((RCC_CCIPR_USART3SEL << 16U) | RCC_CCIPR_USART3SEL_1) /*!< HSI clock used as USART3 clock source */
#define LL_RCC_USART3_CLKSOURCE_LSE        ((RCC_CCIPR_USART3SEL << 16U) | RCC_CCIPR_USART3SEL)   /*!< LSE clock used as USART3 clock source */

#define LL_RCC_I2C1_CLKSOURCE_PCLK1        0x00000000U                  /*!< PCLK1 clock used as I2C1 clock source */
#define LL_RCC_I2C1_CLKSOURCE_SYSCLK       RCC_CCIPR_I2C1SEL_0          /*!< SYSCLK clock used as I2C1 clock source */
#define LL_RCC_I2C1_CLKSOURCE_HSI          RCC_CCIPR_I2C1SEL_1          /*!< HSI clock used as I2C1 clock source */

#define LL_RCC_I2C2_CLKSOURCE_PCLK1        0x00000000U                  /*!< PCLK1 clock used as I2C1 clock source */
#define LL_RCC_I2C2_CLKSOURCE_SYSCLK       RCC_CCIPR_I2C2SEL_0          /*!< SYSCLK clock used as I2C1 clock source */
#define LL_RCC_I2C2_CLKSOURCE_HSI          RCC_CCIPR_I2C2SEL_1          /*!< HSI clock used as I2C1 clock source */

//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- DEBUG REGISTERS --------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Debug MCU
 */
typedef struct
{
  volatile uint32 IDCODE;      /*!< MCU device ID code,              Address offset: 0x00 */
  volatile uint32 CR;          /*!< Debug configuration register,    Address offset: 0x04 */
  volatile uint32 APBFZ1;      /*!< Debug APB freeze register 1,     Address offset: 0x08 */
  volatile uint32 APBFZ2;      /*!< Debug APB freeze register 2,     Address offset: 0x0C */
} DBG_TYPEDEF;

#define DBG              (*(DBG_TYPEDEF *) DBG_BASE)

/********************  Bit definition for DBG_IDCODE register  *************/
#define DBG_IDCODE_DEV_ID_POS                          (0U)
#define DBG_IDCODE_DEV_ID_MSK                          (0xFFFUL << DBG_IDCODE_DEV_ID_POS)  /*!< 0x00000FFF */
#define DBG_IDCODE_DEV_ID                              DBG_IDCODE_DEV_ID_MSK
#define DBG_IDCODE_REV_ID_POS                          (16U)
#define DBG_IDCODE_REV_ID_MSK                          (0xFFFFUL << DBG_IDCODE_REV_ID_POS) /*!< 0xFFFF0000 */
#define DBG_IDCODE_REV_ID                              DBG_IDCODE_REV_ID_MSK

/********************  Bit definition for DBG_CR register  *****************/
#define DBG_CR_DBG_STOP_POS                            (1U)
#define DBG_CR_DBG_STOP_MSK                            (0x1UL << DBG_CR_DBG_STOP_POS)      /*!< 0x00000002 */
#define DBG_CR_DBG_STOP                                DBG_CR_DBG_STOP_MSK
#define DBG_CR_DBG_STANDBY_POS                         (2U)
#define DBG_CR_DBG_STANDBY_MSK                         (0x1UL << DBG_CR_DBG_STANDBY_POS)   /*!< 0x00000004 */
#define DBG_CR_DBG_STANDBY                             DBG_CR_DBG_STANDBY_MSK


/********************  Bit definition for DBG_APB_FZ1 register  ***********/
#define DBG_APB_FZ1_DBG_TIM2_STOP_POS                  (0U)
#define DBG_APB_FZ1_DBG_TIM2_STOP_MSK                  (0x1UL << DBG_APB_FZ1_DBG_TIM2_STOP_POS) /*!< 0x00000001 */
#define DBG_APB_FZ1_DBG_TIM2_STOP                      DBG_APB_FZ1_DBG_TIM2_STOP_MSK
#define DBG_APB_FZ1_DBG_TIM3_STOP_POS                  (1U)
#define DBG_APB_FZ1_DBG_TIM3_STOP_MSK                  (0x1UL << DBG_APB_FZ1_DBG_TIM3_STOP_POS) /*!< 0x00000002 */
#define DBG_APB_FZ1_DBG_TIM3_STOP                      DBG_APB_FZ1_DBG_TIM3_STOP_MSK
#define DBG_APB_FZ1_DBG_TIM4_STOP_POS                  (2U)
#define DBG_APB_FZ1_DBG_TIM4_STOP_MSK                  (0x1UL << DBG_APB_FZ1_DBG_TIM4_STOP_POS) /*!< 0x00000004 */
#define DBG_APB_FZ1_DBG_TIM4_STOP                      DBG_APB_FZ1_DBG_TIM4_STOP_MSK
#define DBG_APB_FZ1_DBG_TIM6_STOP_POS                  (4U)
#define DBG_APB_FZ1_DBG_TIM6_STOP_MSK                  (0x1UL << DBG_APB_FZ1_DBG_TIM6_STOP_POS) /*!< 0x00000010 */
#define DBG_APB_FZ1_DBG_TIM6_STOP                      DBG_APB_FZ1_DBG_TIM6_STOP_MSK
#define DBG_APB_FZ1_DBG_TIM7_STOP_POS                  (5U)
#define DBG_APB_FZ1_DBG_TIM7_STOP_MSK                  (0x1UL << DBG_APB_FZ1_DBG_TIM7_STOP_POS) /*!< 0x00000020 */
#define DBG_APB_FZ1_DBG_TIM7_STOP                      DBG_APB_FZ1_DBG_TIM7_STOP_MSK
#define DBG_APB_FZ1_DBG_RTC_STOP_POS                   (10U)
#define DBG_APB_FZ1_DBG_RTC_STOP_MSK                   (0x1UL << DBG_APB_FZ1_DBG_RTC_STOP_POS)  /*!< 0x00000400 */
#define DBG_APB_FZ1_DBG_RTC_STOP                       DBG_APB_FZ1_DBG_RTC_STOP_MSK
#define DBG_APB_FZ1_DBG_WWDG_STOP_POS                  (11U)
#define DBG_APB_FZ1_DBG_WWDG_STOP_MSK                  (0x1UL << DBG_APB_FZ1_DBG_WWDG_STOP_POS) /*!< 0x00000800 */
#define DBG_APB_FZ1_DBG_WWDG_STOP                      DBG_APB_FZ1_DBG_WWDG_STOP_MSK
#define DBG_APB_FZ1_DBG_IWDG_STOP_POS                  (12U)
#define DBG_APB_FZ1_DBG_IWDG_STOP_MSK                  (0x1UL << DBG_APB_FZ1_DBG_IWDG_STOP_POS) /*!< 0x00001000 */
#define DBG_APB_FZ1_DBG_IWDG_STOP                      DBG_APB_FZ1_DBG_IWDG_STOP_MSK
#define DBG_APB_FZ1_DBG_I2C1_SMBUS_TIMEOUT_STOP_POS    (21U)
#define DBG_APB_FZ1_DBG_I2C1_SMBUS_TIMEOUT_STOP_MSK    (0x1UL << DBG_APB_FZ1_DBG_I2C1_SMBUS_TIMEOUT_STOP_POS) /*!< 0x00200000 */
#define DBG_APB_FZ1_DBG_I2C1_SMBUS_TIMEOUT_STOP        DBG_APB_FZ1_DBG_I2C1_SMBUS_TIMEOUT_STOP_MSK
#define DBG_APB_FZ1_DBG_I2C2_SMBUS_TIMEOUT_STOP_POS    (22U)
#define DBG_APB_FZ1_DBG_I2C2_SMBUS_TIMEOUT_STOP_MSK    (0x1UL << DBG_APB_FZ1_DBG_I2C2_SMBUS_TIMEOUT_STOP_POS) /*!< 0x00400000 */
#define DBG_APB_FZ1_DBG_I2C2_SMBUS_TIMEOUT_STOP        DBG_APB_FZ1_DBG_I2C2_SMBUS_TIMEOUT_STOP_MSK
#define DBG_APB_FZ1_DBG_LPTIM2_STOP_POS                (30U)
#define DBG_APB_FZ1_DBG_LPTIM2_STOP_MSK                (0x1UL << DBG_APB_FZ1_DBG_LPTIM2_STOP_POS) /*!< 0x40000000 */
#define DBG_APB_FZ1_DBG_LPTIM2_STOP                    DBG_APB_FZ1_DBG_LPTIM2_STOP_MSK
#define DBG_APB_FZ1_DBG_LPTIM1_STOP_POS                (31U)
#define DBG_APB_FZ1_DBG_LPTIM1_STOP_MSK                (0x1UL << DBG_APB_FZ1_DBG_LPTIM1_STOP_POS) /*!< 0x80000000 */
#define DBG_APB_FZ1_DBG_LPTIM1_STOP                    DBG_APB_FZ1_DBG_LPTIM1_STOP_MSK

/********************  Bit definition for DBG_APB_FZ2 register  ************/
#define DBG_APB_FZ2_DBG_TIM1_STOP_POS                  (11U)
#define DBG_APB_FZ2_DBG_TIM1_STOP_MSK                  (0x1UL << DBG_APB_FZ2_DBG_TIM1_STOP_POS)  /*!< 0x00000800 */
#define DBG_APB_FZ2_DBG_TIM1_STOP                      DBG_APB_FZ2_DBG_TIM1_STOP_MSK
#define DBG_APB_FZ2_DBG_TIM14_STOP_POS                 (15U)
#define DBG_APB_FZ2_DBG_TIM14_STOP_MSK                 (0x1UL << DBG_APB_FZ2_DBG_TIM14_STOP_POS) /*!< 0x00008000 */
#define DBG_APB_FZ2_DBG_TIM14_STOP                     DBG_APB_FZ2_DBG_TIM14_STOP_MSK
#define DBG_APB_FZ2_DBG_TIM15_STOP_POS                 (16U)
#define DBG_APB_FZ2_DBG_TIM15_STOP_MSK                 (0x1UL << DBG_APB_FZ2_DBG_TIM15_STOP_POS) /*!< 0x00010000 */
#define DBG_APB_FZ2_DBG_TIM15_STOP                     DBG_APB_FZ2_DBG_TIM15_STOP_MSK
#define DBG_APB_FZ2_DBG_TIM16_STOP_POS                 (17U)
#define DBG_APB_FZ2_DBG_TIM16_STOP_MSK                 (0x1UL << DBG_APB_FZ2_DBG_TIM16_STOP_POS) /*!< 0x00020000 */
#define DBG_APB_FZ2_DBG_TIM16_STOP                     DBG_APB_FZ2_DBG_TIM16_STOP_MSK
#define DBG_APB_FZ2_DBG_TIM17_STOP_POS                 (18U)
#define DBG_APB_FZ2_DBG_TIM17_STOP_MSK                 (0x1UL << DBG_APB_FZ2_DBG_TIM17_STOP_POS) /*!< 0x00040000 */
#define DBG_APB_FZ2_DBG_TIM17_STOP                     DBG_APB_FZ2_DBG_TIM17_STOP_MSK





//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- IWDG REGISTERS ---------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Independent Watchdog Register(IWDG)
 */
typedef struct
{
  volatile uint32 KR;          /*!< IWDG Key register,       Address offset: 0x00 */
  volatile uint32 PR;          /*!< IWDG Prescaler register, Address offset: 0x04 */
  volatile uint32 RLR;         /*!< IWDG Reload register,    Address offset: 0x08 */
  volatile uint32 SR;          /*!< IWDG Status register,    Address offset: 0x0C */
  volatile uint32 WINR;        /*!< IWDG Window register,    Address offset: 0x10 */
} IWDG_TYPEDEF;

#define IWDG                (*(IWDG_TYPEDEF *) IWDG_BASE)

#define IWDG_WRITE_ACCESS_ENABLE    ((uint32)0x00005555)
#define IWDG_WRITE_ACCESS_DISABLE   ((uint32)0x00000000)
#define IWDG_RELOAD                 ((uint32)0x0000AAAA)
#define IWDG_ENABLE                 ((uint32)0x0000CCCC)

/*******************  Bit definition for IWDG_KR register  ********************/
#define IWDG_KR_KEY_POS      (0U)
#define IWDG_KR_KEY_MSK      (0xFFFFUL << IWDG_KR_KEY_POS)                     /*!< 0x0000FFFF */
#define IWDG_KR_KEY          IWDG_KR_KEY_MSK                                   /*!<Key value (write only, read 0000h)  */

/*******************  Bit definition for IWDG_PR register  ********************/
#define IWDG_PR_PR_POS       (0U)
#define IWDG_PR_PR_MSK       (0x7UL << IWDG_PR_PR_POS)                         /*!< 0x00000007 */
#define IWDG_PR_PR           IWDG_PR_PR_MSK                                    /*!<PR[2:0] (Prescaler divider)         */
#define IWDG_PR_PR_0         (0x1UL << IWDG_PR_PR_POS)                         /*!< 0x00000001 */
#define IWDG_PR_PR_1         (0x2UL << IWDG_PR_PR_POS)                         /*!< 0x00000002 */
#define IWDG_PR_PR_2         (0x4UL << IWDG_PR_PR_POS)                         /*!< 0x00000004 */

/*******************  Bit definition for IWDG_RLR register  *******************/
#define IWDG_RLR_RL_POS      (0U)
#define IWDG_RLR_RL_MSK      (0xFFFUL << IWDG_RLR_RL_POS)                      /*!< 0x00000FFF */
#define IWDG_RLR_RL          IWDG_RLR_RL_MSK                                   /*!<Watchdog counter reload value        */

/*******************  Bit definition for IWDG_SR register  ********************/
#define IWDG_SR_PVU_POS      (0U)
#define IWDG_SR_PVU_MSK      (0x1UL << IWDG_SR_PVU_POS)                        /*!< 0x00000001 */
#define IWDG_SR_PVU          IWDG_SR_PVU_MSK                                   /*!< Watchdog prescaler value update */
#define IWDG_SR_RVU_POS      (1U)
#define IWDG_SR_RVU_MSK      (0x1UL << IWDG_SR_RVU_POS)                        /*!< 0x00000002 */
#define IWDG_SR_RVU          IWDG_SR_RVU_MSK                                   /*!< Watchdog counter reload value update */
#define IWDG_SR_WVU_POS      (2U)
#define IWDG_SR_WVU_MSK      (0x1UL << IWDG_SR_WVU_POS)                        /*!< 0x00000004 */
#define IWDG_SR_WVU          IWDG_SR_WVU_MSK                                   /*!< Watchdog counter window value update */

/*******************  Bit definition for IWDG_KR register  ********************/
#define IWDG_WINR_WIN_POS    (0U)
#define IWDG_WINR_WIN_MSK    (0xFFFUL << IWDG_WINR_WIN_POS)                    /*!< 0x00000FFF */
#define IWDG_WINR_WIN        IWDG_WINR_WIN_MSK                                 /*!< Watchdog counter window value */





//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- WWDG REGISTERS --------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Window Watchdog Register(WWDG)
 */
typedef struct
{
  volatile uint32 CR;          /*!< WWDG Control register,       Address offset: 0x00 */
  volatile uint32 CFR;         /*!< WWDG Configuration register, Address offset: 0x04 */
  volatile uint32 SR;          /*!< WWDG Status register,        Address offset: 0x08 */
} WWDG_TYPEDEF;


#define WWDG                (*(WWDG_TYPEDEF *) WWDG_BASE)

/*******************  Bit definition for WWDG_CR register  ********************/
#define WWDG_CR_T_POS           (0U)
#define WWDG_CR_T_MSK           (0x7FUL << WWDG_CR_T_POS)                      /*!< 0x0000007F */
#define WWDG_CR_T               WWDG_CR_T_MSK                                  /*!<T[6:0] bits (7-Bit counter (MSB to LSB)) */
#define WWDG_CR_T_0             (0x01UL << WWDG_CR_T_POS)                      /*!< 0x00000001 */
#define WWDG_CR_T_1             (0x02UL << WWDG_CR_T_POS)                      /*!< 0x00000002 */
#define WWDG_CR_T_2             (0x04UL << WWDG_CR_T_POS)                      /*!< 0x00000004 */
#define WWDG_CR_T_3             (0x08UL << WWDG_CR_T_POS)                      /*!< 0x00000008 */
#define WWDG_CR_T_4             (0x10UL << WWDG_CR_T_POS)                      /*!< 0x00000010 */
#define WWDG_CR_T_5             (0x20UL << WWDG_CR_T_POS)                      /*!< 0x00000020 */
#define WWDG_CR_T_6             (0x40UL << WWDG_CR_T_POS)                      /*!< 0x00000040 */

#define WWDG_CR_WDGA_POS        (7U)
#define WWDG_CR_WDGA_MSK        (0x1UL << WWDG_CR_WDGA_POS)                    /*!< 0x00000080 */
#define WWDG_CR_WDGA            WWDG_CR_WDGA_MSK                               /*!<Activation bit */

/*******************  Bit definition for WWDG_CFR register  *******************/
#define WWDG_CFR_W_POS          (0U)
#define WWDG_CFR_W_MSK          (0x7FUL << WWDG_CFR_W_POS)                     /*!< 0x0000007F */
#define WWDG_CFR_W              WWDG_CFR_W_MSK                                 /*!<W[6:0] bits (7-bit window value) */
#define WWDG_CFR_W_0            (0x01UL << WWDG_CFR_W_POS)                     /*!< 0x00000001 */
#define WWDG_CFR_W_1            (0x02UL << WWDG_CFR_W_POS)                     /*!< 0x00000002 */
#define WWDG_CFR_W_2            (0x04UL << WWDG_CFR_W_POS)                     /*!< 0x00000004 */
#define WWDG_CFR_W_3            (0x08UL << WWDG_CFR_W_POS)                     /*!< 0x00000008 */
#define WWDG_CFR_W_4            (0x10UL << WWDG_CFR_W_POS)                     /*!< 0x00000010 */
#define WWDG_CFR_W_5            (0x20UL << WWDG_CFR_W_POS)                     /*!< 0x00000020 */
#define WWDG_CFR_W_6            (0x40UL << WWDG_CFR_W_POS)                     /*!< 0x00000040 */

#define WWDG_CFR_WDGTB_POS      (11U)
#define WWDG_CFR_WDGTB_MSK      (0x7UL << WWDG_CFR_WDGTB_POS)                  /*!< 0x00003800 */
#define WWDG_CFR_WDGTB          WWDG_CFR_WDGTB_MSK                             /*!<WDGTB[2:0] bits (Timer Base) */
#define WWDG_CFR_WDGTB_0        (0x1UL << WWDG_CFR_WDGTB_POS)                  /*!< 0x00000800 */
#define WWDG_CFR_WDGTB_1        (0x2UL << WWDG_CFR_WDGTB_POS)                  /*!< 0x00001000 */
#define WWDG_CFR_WDGTB_2        (0x4UL << WWDG_CFR_WDGTB_POS)                  /*!< 0x00002000 */

#define WWDG_CFR_EWI_POS        (9U)
#define WWDG_CFR_EWI_MSK        (0x1UL << WWDG_CFR_EWI_POS)                    /*!< 0x00000200 */
#define WWDG_CFR_EWI            WWDG_CFR_EWI_MSK                               /*!<Early Wakeup Interrupt */

/*******************  Bit definition for WWDG_SR register  ********************/
#define WWDG_SR_EWIF_POS        (0U)
#define WWDG_SR_EWIF_MSK        (0x1UL << WWDG_SR_EWIF_POS)                    /*!< 0x00000001 */
#define WWDG_SR_EWIF            WWDG_SR_EWIF_MSK                               /*!<Early Wakeup Interrupt Flag */





//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- DMA CONTROLLER REGISTERS -----------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief DMA Controller
  */
typedef struct
{
  volatile uint32 CCR;         /*!< DMA channel x configuration register        */
  volatile uint32 CNDTR;       /*!< DMA channel x number of data register       */
  volatile uint32 CPAR;        /*!< DMA channel x peripheral address register   */
  volatile uint32 CMAR;        /*!< DMA channel x memory address register       */
} DMA_CHANNEL_TYPEDEF;

typedef struct
{
  volatile uint32 ISR;         /*!< DMA interrupt status register,                 Address offset: 0x00 */
  volatile uint32 IFCR;        /*!< DMA interrupt flag clear register,             Address offset: 0x04 */
} DMA_TYPEDEF;

/**
  * @brief DMA Multiplexer
  */
typedef struct
{
  volatile uint32   CCR[12];       /*!< DMA Multiplexer CHANNEL x Control Register    Address offset: 0x0004 * (channel x) */
}DMAMUX_CHANNEL_TYPEDEF;

typedef struct
{
  volatile uint32   CSR;       /*!< DMA CHANNEL Status Register                    Address offset: 0x0080   */
  volatile uint32   CFR;       /*!< DMA CHANNEL Clear Flag Register                Address offset: 0x0084   */
}DMAMUX_CHANNELSTATUS_TYPEDEF;

typedef struct
{
  volatile uint32   RGCR;        /*!< DMA Request Generator x Control Register     Address offset: 0x0100 + 0x0004 * (Req Gen x) */
}DMAMUX_REQUESTGEN_TYPEDEF;

typedef struct
{
  volatile uint32   RGSR;        /*!< DMA Request Generator Status Register        Address offset: 0x0140   */
  volatile uint32   RGCFR;       /*!< DMA Request Generator Clear Flag Register    Address offset: 0x0144   */
}DMAMUX_REQUESTGENSTATUS_TYPEDEF;


#define DMA1                       (*(DMA_TYPEDEF *) DMA1_BASE)
#define DMA2                       (*(DMA_TYPEDEF *) DMA2_BASE)

#define DMA1_CHANNEL1              (*(DMA_CHANNEL_TYPEDEF *) DMA1_CHANNEL1_BASE)
#define DMA1_CHANNEL2              (*(DMA_CHANNEL_TYPEDEF *) DMA1_CHANNEL2_BASE)
#define DMA1_CHANNEL3              (*(DMA_CHANNEL_TYPEDEF *) DMA1_CHANNEL3_BASE)
#define DMA1_CHANNEL4              (*(DMA_CHANNEL_TYPEDEF *) DMA1_CHANNEL4_BASE)
#define DMA1_CHANNEL5              (*(DMA_CHANNEL_TYPEDEF *) DMA1_CHANNEL5_BASE)
#define DMA1_CHANNEL6              (*(DMA_CHANNEL_TYPEDEF *) DMA1_CHANNEL6_BASE)
#define DMA1_CHANNEL7              (*(DMA_CHANNEL_TYPEDEF *) DMA1_CHANNEL7_BASE)
#define DMA2_CHANNEL1              (*(DMA_CHANNEL_TYPEDEF *) DMA2_CHANNEL1_BASE)
#define DMA2_CHANNEL2              (*(DMA_CHANNEL_TYPEDEF *) DMA2_CHANNEL2_BASE)
#define DMA2_CHANNEL3              (*(DMA_CHANNEL_TYPEDEF *) DMA2_CHANNEL3_BASE)
#define DMA2_CHANNEL4              (*(DMA_CHANNEL_TYPEDEF *) DMA2_CHANNEL4_BASE)
#define DMA2_CHANNEL5              (*(DMA_CHANNEL_TYPEDEF *) DMA2_CHANNEL5_BASE)

#define DMAMUX1                    (*(DMAMUX_CHANNEL_TYPEDEF *) DMAMUX1_BASE)

#define DMAMUX1_REQUESTGENERATOR0  (*(DMAMUX_REQUESTGEN_TYPEDEF *) DMAMUX1_REQUESTGENERATOR0_BASE)
#define DMAMUX1_REQUESTGENERATOR1  (*(DMAMUX_REQUESTGEN_TYPEDEF *) DMAMUX1_REQUESTGENERATOR1_BASE)
#define DMAMUX1_REQUESTGENERATOR2  (*(DMAMUX_REQUESTGEN_TYPEDEF *) DMAMUX1_REQUESTGENERATOR2_BASE)
#define DMAMUX1_REQUESTGENERATOR3  (*(DMAMUX_REQUESTGEN_TYPEDEF *) DMAMUX1_REQUESTGENERATOR3_BASE)

#define DMAMUX1_CHANNELSTATUS      (*(DMAMUX_CHANNELSTATUS_TYPEDEF *) DMAMUX1_CHANNELSTATUS_BASE)
#define DMAMUX1_REQUESTGENSTATUS   (*(DMAMUX_REQUESTGENSTATUS_TYPEDEF *) DMAMUX1_REQUESTGENSTATUS_BASE)


/*******************  Bit definition for DMA_ISR register  ********************/
#define DMA_ISR_GIF1_POS       (0U)
#define DMA_ISR_GIF1_MSK       (0x1UL << DMA_ISR_GIF1_POS)                     /*!< 0x00000001 */
#define DMA_ISR_GIF1           DMA_ISR_GIF1_MSK                                /*!< CHANNEL 1 Global interrupt flag */
#define DMA_ISR_TCIF1_POS      (1U)
#define DMA_ISR_TCIF1_MSK      (0x1UL << DMA_ISR_TCIF1_POS)                    /*!< 0x00000002 */
#define DMA_ISR_TCIF1          DMA_ISR_TCIF1_MSK                               /*!< CHANNEL 1 Transfer Complete flag */
#define DMA_ISR_HTIF1_POS      (2U)
#define DMA_ISR_HTIF1_MSK      (0x1UL << DMA_ISR_HTIF1_POS)                    /*!< 0x00000004 */
#define DMA_ISR_HTIF1          DMA_ISR_HTIF1_MSK                               /*!< CHANNEL 1 Half Transfer flag */
#define DMA_ISR_TEIF1_POS      (3U)
#define DMA_ISR_TEIF1_MSK      (0x1UL << DMA_ISR_TEIF1_POS)                    /*!< 0x00000008 */
#define DMA_ISR_TEIF1          DMA_ISR_TEIF1_MSK                               /*!< CHANNEL 1 Transfer Error flag */
#define DMA_ISR_GIF2_POS       (4U)
#define DMA_ISR_GIF2_MSK       (0x1UL << DMA_ISR_GIF2_POS)                     /*!< 0x00000010 */
#define DMA_ISR_GIF2           DMA_ISR_GIF2_MSK                                /*!< CHANNEL 2 Global interrupt flag */
#define DMA_ISR_TCIF2_POS      (5U)
#define DMA_ISR_TCIF2_MSK      (0x1UL << DMA_ISR_TCIF2_POS)                    /*!< 0x00000020 */
#define DMA_ISR_TCIF2          DMA_ISR_TCIF2_MSK                               /*!< CHANNEL 2 Transfer Complete flag */
#define DMA_ISR_HTIF2_POS      (6U)
#define DMA_ISR_HTIF2_MSK      (0x1UL << DMA_ISR_HTIF2_POS)                    /*!< 0x00000040 */
#define DMA_ISR_HTIF2          DMA_ISR_HTIF2_MSK                               /*!< CHANNEL 2 Half Transfer flag */
#define DMA_ISR_TEIF2_POS      (7U)
#define DMA_ISR_TEIF2_MSK      (0x1UL << DMA_ISR_TEIF2_POS)                    /*!< 0x00000080 */
#define DMA_ISR_TEIF2          DMA_ISR_TEIF2_MSK                               /*!< CHANNEL 2 Transfer Error flag */
#define DMA_ISR_GIF3_POS       (8U)
#define DMA_ISR_GIF3_MSK       (0x1UL << DMA_ISR_GIF3_POS)                     /*!< 0x00000100 */
#define DMA_ISR_GIF3           DMA_ISR_GIF3_MSK                                /*!< CHANNEL 3 Global interrupt flag */
#define DMA_ISR_TCIF3_POS      (9U)
#define DMA_ISR_TCIF3_MSK      (0x1UL << DMA_ISR_TCIF3_POS)                    /*!< 0x00000200 */
#define DMA_ISR_TCIF3          DMA_ISR_TCIF3_MSK                               /*!< CHANNEL 3 Transfer Complete flag */
#define DMA_ISR_HTIF3_POS      (10U)
#define DMA_ISR_HTIF3_MSK      (0x1UL << DMA_ISR_HTIF3_POS)                    /*!< 0x00000400 */
#define DMA_ISR_HTIF3          DMA_ISR_HTIF3_MSK                               /*!< CHANNEL 3 Half Transfer flag */
#define DMA_ISR_TEIF3_POS      (11U)
#define DMA_ISR_TEIF3_MSK      (0x1UL << DMA_ISR_TEIF3_POS)                    /*!< 0x00000800 */
#define DMA_ISR_TEIF3          DMA_ISR_TEIF3_MSK                               /*!< CHANNEL 3 Transfer Error flag */
#define DMA_ISR_GIF4_POS       (12U)
#define DMA_ISR_GIF4_MSK       (0x1UL << DMA_ISR_GIF4_POS)                     /*!< 0x00001000 */
#define DMA_ISR_GIF4           DMA_ISR_GIF4_MSK                                /*!< CHANNEL 4 Global interrupt flag */
#define DMA_ISR_TCIF4_POS      (13U)
#define DMA_ISR_TCIF4_MSK      (0x1UL << DMA_ISR_TCIF4_POS)                    /*!< 0x00002000 */
#define DMA_ISR_TCIF4          DMA_ISR_TCIF4_MSK                               /*!< CHANNEL 4 Transfer Complete flag */
#define DMA_ISR_HTIF4_POS      (14U)
#define DMA_ISR_HTIF4_MSK      (0x1UL << DMA_ISR_HTIF4_POS)                    /*!< 0x00004000 */
#define DMA_ISR_HTIF4          DMA_ISR_HTIF4_MSK                               /*!< CHANNEL 4 Half Transfer flag */
#define DMA_ISR_TEIF4_POS      (15U)
#define DMA_ISR_TEIF4_MSK      (0x1UL << DMA_ISR_TEIF4_POS)                    /*!< 0x00008000 */
#define DMA_ISR_TEIF4          DMA_ISR_TEIF4_MSK                               /*!< CHANNEL 4 Transfer Error flag */
#define DMA_ISR_GIF5_POS       (16U)
#define DMA_ISR_GIF5_MSK       (0x1UL << DMA_ISR_GIF5_POS)                     /*!< 0x00010000 */
#define DMA_ISR_GIF5           DMA_ISR_GIF5_MSK                                /*!< CHANNEL 5 Global interrupt flag */
#define DMA_ISR_TCIF5_POS      (17U)
#define DMA_ISR_TCIF5_MSK      (0x1UL << DMA_ISR_TCIF5_POS)                    /*!< 0x00020000 */
#define DMA_ISR_TCIF5          DMA_ISR_TCIF5_MSK                               /*!< CHANNEL 5 Transfer Complete flag */
#define DMA_ISR_HTIF5_POS      (18U)
#define DMA_ISR_HTIF5_MSK      (0x1UL << DMA_ISR_HTIF5_POS)                    /*!< 0x00040000 */
#define DMA_ISR_HTIF5          DMA_ISR_HTIF5_MSK                               /*!< CHANNEL 5 Half Transfer flag */
#define DMA_ISR_TEIF5_POS      (19U)
#define DMA_ISR_TEIF5_MSK      (0x1UL << DMA_ISR_TEIF5_POS)                    /*!< 0x00080000 */
#define DMA_ISR_TEIF5          DMA_ISR_TEIF5_MSK                               /*!< CHANNEL 5 Transfer Error flag */
#define DMA_ISR_GIF6_POS       (20U)
#define DMA_ISR_GIF6_MSK       (0x1UL << DMA_ISR_GIF6_POS)                     /*!< 0x00100000 */
#define DMA_ISR_GIF6           DMA_ISR_GIF6_MSK                                /*!< CHANNEL 6 Global interrupt flag */
#define DMA_ISR_TCIF6_POS      (21U)
#define DMA_ISR_TCIF6_MSK      (0x1UL << DMA_ISR_TCIF6_POS)                    /*!< 0x00200000 */
#define DMA_ISR_TCIF6          DMA_ISR_TCIF6_MSK                               /*!< CHANNEL 6 Transfer Complete flag */
#define DMA_ISR_HTIF6_POS      (22U)
#define DMA_ISR_HTIF6_MSK      (0x1UL << DMA_ISR_HTIF6_POS)                    /*!< 0x00400000 */
#define DMA_ISR_HTIF6          DMA_ISR_HTIF6_MSK                               /*!< CHANNEL 6 Half Transfer flag */
#define DMA_ISR_TEIF6_POS      (23U)
#define DMA_ISR_TEIF6_MSK      (0x1UL << DMA_ISR_TEIF6_POS)                    /*!< 0x00800000 */
#define DMA_ISR_TEIF6          DMA_ISR_TEIF6_MSK                               /*!< CHANNEL 6 Transfer Error flag */
#define DMA_ISR_GIF7_POS       (24U)
#define DMA_ISR_GIF7_MSK       (0x1UL << DMA_ISR_GIF7_POS)                     /*!< 0x01000000 */
#define DMA_ISR_GIF7           DMA_ISR_GIF7_MSK                                /*!< CHANNEL 7 Global interrupt flag */
#define DMA_ISR_TCIF7_POS      (25U)
#define DMA_ISR_TCIF7_MSK      (0x1UL << DMA_ISR_TCIF7_POS)                    /*!< 0x02000000 */
#define DMA_ISR_TCIF7          DMA_ISR_TCIF7_MSK                               /*!< CHANNEL 7 Transfer Complete flag */
#define DMA_ISR_HTIF7_POS      (26U)
#define DMA_ISR_HTIF7_MSK      (0x1UL << DMA_ISR_HTIF7_POS)                    /*!< 0x04000000 */
#define DMA_ISR_HTIF7          DMA_ISR_HTIF7_MSK                               /*!< CHANNEL 7 Half Transfer flag */
#define DMA_ISR_TEIF7_POS      (27U)
#define DMA_ISR_TEIF7_MSK      (0x1UL << DMA_ISR_TEIF7_POS)                    /*!< 0x08000000 */
#define DMA_ISR_TEIF7          DMA_ISR_TEIF7_MSK                               /*!< CHANNEL 7 Transfer Error flag */

/*******************  Bit definition for DMA_IFCR register  *******************/
#define DMA_IFCR_CGIF1_POS     (0U)
#define DMA_IFCR_CGIF1_MSK     (0x1UL << DMA_IFCR_CGIF1_POS)                   /*!< 0x00000001 */
#define DMA_IFCR_CGIF1         DMA_IFCR_CGIF1_MSK                              /*!< CHANNEL 1 Global interrupt clearr */
#define DMA_IFCR_CTCIF1_POS    (1U)
#define DMA_IFCR_CTCIF1_MSK    (0x1UL << DMA_IFCR_CTCIF1_POS)                  /*!< 0x00000002 */
#define DMA_IFCR_CTCIF1        DMA_IFCR_CTCIF1_MSK                             /*!< CHANNEL 1 Transfer Complete clear */
#define DMA_IFCR_CHTIF1_POS    (2U)
#define DMA_IFCR_CHTIF1_MSK    (0x1UL << DMA_IFCR_CHTIF1_POS)                  /*!< 0x00000004 */
#define DMA_IFCR_CHTIF1        DMA_IFCR_CHTIF1_MSK                             /*!< CHANNEL 1 Half Transfer clear */
#define DMA_IFCR_CTEIF1_POS    (3U)
#define DMA_IFCR_CTEIF1_MSK    (0x1UL << DMA_IFCR_CTEIF1_POS)                  /*!< 0x00000008 */
#define DMA_IFCR_CTEIF1        DMA_IFCR_CTEIF1_MSK                             /*!< CHANNEL 1 Transfer Error clear */
#define DMA_IFCR_CGIF2_POS     (4U)
#define DMA_IFCR_CGIF2_MSK     (0x1UL << DMA_IFCR_CGIF2_POS)                   /*!< 0x00000010 */
#define DMA_IFCR_CGIF2         DMA_IFCR_CGIF2_MSK                              /*!< CHANNEL 2 Global interrupt clear */
#define DMA_IFCR_CTCIF2_POS    (5U)
#define DMA_IFCR_CTCIF2_MSK    (0x1UL << DMA_IFCR_CTCIF2_POS)                  /*!< 0x00000020 */
#define DMA_IFCR_CTCIF2        DMA_IFCR_CTCIF2_MSK                             /*!< CHANNEL 2 Transfer Complete clear */
#define DMA_IFCR_CHTIF2_POS    (6U)
#define DMA_IFCR_CHTIF2_MSK    (0x1UL << DMA_IFCR_CHTIF2_POS)                  /*!< 0x00000040 */
#define DMA_IFCR_CHTIF2        DMA_IFCR_CHTIF2_MSK                             /*!< CHANNEL 2 Half Transfer clear */
#define DMA_IFCR_CTEIF2_POS    (7U)
#define DMA_IFCR_CTEIF2_MSK    (0x1UL << DMA_IFCR_CTEIF2_POS)                  /*!< 0x00000080 */
#define DMA_IFCR_CTEIF2        DMA_IFCR_CTEIF2_MSK                             /*!< CHANNEL 2 Transfer Error clear */
#define DMA_IFCR_CGIF3_POS     (8U)
#define DMA_IFCR_CGIF3_MSK     (0x1UL << DMA_IFCR_CGIF3_POS)                   /*!< 0x00000100 */
#define DMA_IFCR_CGIF3         DMA_IFCR_CGIF3_MSK                              /*!< CHANNEL 3 Global interrupt clear */
#define DMA_IFCR_CTCIF3_POS    (9U)
#define DMA_IFCR_CTCIF3_MSK    (0x1UL << DMA_IFCR_CTCIF3_POS)                  /*!< 0x00000200 */
#define DMA_IFCR_CTCIF3        DMA_IFCR_CTCIF3_MSK                             /*!< CHANNEL 3 Transfer Complete clear */
#define DMA_IFCR_CHTIF3_POS    (10U)
#define DMA_IFCR_CHTIF3_MSK    (0x1UL << DMA_IFCR_CHTIF3_POS)                  /*!< 0x00000400 */
#define DMA_IFCR_CHTIF3        DMA_IFCR_CHTIF3_MSK                             /*!< CHANNEL 3 Half Transfer clear */
#define DMA_IFCR_CTEIF3_POS    (11U)
#define DMA_IFCR_CTEIF3_MSK    (0x1UL << DMA_IFCR_CTEIF3_POS)                  /*!< 0x00000800 */
#define DMA_IFCR_CTEIF3        DMA_IFCR_CTEIF3_MSK                             /*!< CHANNEL 3 Transfer Error clear */
#define DMA_IFCR_CGIF4_POS     (12U)
#define DMA_IFCR_CGIF4_MSK     (0x1UL << DMA_IFCR_CGIF4_POS)                   /*!< 0x00001000 */
#define DMA_IFCR_CGIF4         DMA_IFCR_CGIF4_MSK                              /*!< CHANNEL 4 Global interrupt clear */
#define DMA_IFCR_CTCIF4_POS    (13U)
#define DMA_IFCR_CTCIF4_MSK    (0x1UL << DMA_IFCR_CTCIF4_POS)                  /*!< 0x00002000 */
#define DMA_IFCR_CTCIF4        DMA_IFCR_CTCIF4_MSK                             /*!< CHANNEL 4 Transfer Complete clear */
#define DMA_IFCR_CHTIF4_POS    (14U)
#define DMA_IFCR_CHTIF4_MSK    (0x1UL << DMA_IFCR_CHTIF4_POS)                  /*!< 0x00004000 */
#define DMA_IFCR_CHTIF4        DMA_IFCR_CHTIF4_MSK                             /*!< CHANNEL 4 Half Transfer clear */
#define DMA_IFCR_CTEIF4_POS    (15U)
#define DMA_IFCR_CTEIF4_MSK    (0x1UL << DMA_IFCR_CTEIF4_POS)                  /*!< 0x00008000 */
#define DMA_IFCR_CTEIF4        DMA_IFCR_CTEIF4_MSK                             /*!< CHANNEL 4 Transfer Error clear */
#define DMA_IFCR_CGIF5_POS     (16U)
#define DMA_IFCR_CGIF5_MSK     (0x1UL << DMA_IFCR_CGIF5_POS)                   /*!< 0x00010000 */
#define DMA_IFCR_CGIF5         DMA_IFCR_CGIF5_MSK                              /*!< CHANNEL 5 Global interrupt clear */
#define DMA_IFCR_CTCIF5_POS    (17U)
#define DMA_IFCR_CTCIF5_MSK    (0x1UL << DMA_IFCR_CTCIF5_POS)                  /*!< 0x00020000 */
#define DMA_IFCR_CTCIF5        DMA_IFCR_CTCIF5_MSK                             /*!< CHANNEL 5 Transfer Complete clear */
#define DMA_IFCR_CHTIF5_POS    (18U)
#define DMA_IFCR_CHTIF5_MSK    (0x1UL << DMA_IFCR_CHTIF5_POS)                  /*!< 0x00040000 */
#define DMA_IFCR_CHTIF5        DMA_IFCR_CHTIF5_MSK                             /*!< CHANNEL 5 Half Transfer clear */
#define DMA_IFCR_CTEIF5_POS    (19U)
#define DMA_IFCR_CTEIF5_MSK    (0x1UL << DMA_IFCR_CTEIF5_POS)                  /*!< 0x00080000 */
#define DMA_IFCR_CTEIF5        DMA_IFCR_CTEIF5_MSK                             /*!< CHANNEL 5 Transfer Error clear */
#define DMA_IFCR_CGIF6_POS     (20U)
#define DMA_IFCR_CGIF6_MSK     (0x1UL << DMA_IFCR_CGIF6_POS)                   /*!< 0x00100000 */
#define DMA_IFCR_CGIF6         DMA_IFCR_CGIF6_MSK                              /*!< CHANNEL 6 Global interrupt clear */
#define DMA_IFCR_CTCIF6_POS    (21U)
#define DMA_IFCR_CTCIF6_MSK    (0x1UL << DMA_IFCR_CTCIF6_POS)                  /*!< 0x00200000 */
#define DMA_IFCR_CTCIF6        DMA_IFCR_CTCIF6_MSK                             /*!< CHANNEL 6 Transfer Complete clear */
#define DMA_IFCR_CHTIF6_POS    (22U)
#define DMA_IFCR_CHTIF6_MSK    (0x1UL << DMA_IFCR_CHTIF6_POS)                  /*!< 0x00400000 */
#define DMA_IFCR_CHTIF6        DMA_IFCR_CHTIF6_MSK                             /*!< CHANNEL 6 Half Transfer clear */
#define DMA_IFCR_CTEIF6_POS    (23U)
#define DMA_IFCR_CTEIF6_MSK    (0x1UL << DMA_IFCR_CTEIF6_POS)                  /*!< 0x00800000 */
#define DMA_IFCR_CTEIF6        DMA_IFCR_CTEIF6_MSK                             /*!< CHANNEL 6 Transfer Error clear */
#define DMA_IFCR_CGIF7_POS     (24U)
#define DMA_IFCR_CGIF7_MSK     (0x1UL << DMA_IFCR_CGIF7_POS)                   /*!< 0x01000000 */
#define DMA_IFCR_CGIF7         DMA_IFCR_CGIF7_MSK                              /*!< CHANNEL 7 Global interrupt clear */
#define DMA_IFCR_CTCIF7_POS    (25U)
#define DMA_IFCR_CTCIF7_MSK    (0x1UL << DMA_IFCR_CTCIF7_POS)                  /*!< 0x02000000 */
#define DMA_IFCR_CTCIF7        DMA_IFCR_CTCIF7_MSK                             /*!< CHANNEL 7 Transfer Complete clear */
#define DMA_IFCR_CHTIF7_POS    (26U)
#define DMA_IFCR_CHTIF7_MSK    (0x1UL << DMA_IFCR_CHTIF7_POS)                  /*!< 0x04000000 */
#define DMA_IFCR_CHTIF7        DMA_IFCR_CHTIF7_MSK                             /*!< CHANNEL 7 Half Transfer clear */
#define DMA_IFCR_CTEIF7_POS    (27U)
#define DMA_IFCR_CTEIF7_MSK    (0x1UL << DMA_IFCR_CTEIF7_POS)                  /*!< 0x08000000 */
#define DMA_IFCR_CTEIF7        DMA_IFCR_CTEIF7_MSK                             /*!< CHANNEL 7 Transfer Error clear */

/*******************  Bit definition for DMA_CCR register  ********************/
#define DMA_CCR_EN_POS         (0U)
#define DMA_CCR_EN_MSK         (0x1UL << DMA_CCR_EN_POS)                       /*!< 0x00000001 */
#define DMA_CCR_EN             DMA_CCR_EN_MSK                                  /*!< CHANNEL enable                      */
#define DMA_CCR_TCIE_POS       (1U)
#define DMA_CCR_TCIE_MSK       (0x1UL << DMA_CCR_TCIE_POS)                     /*!< 0x00000002 */
#define DMA_CCR_TCIE           DMA_CCR_TCIE_MSK                                /*!< Transfer complete interrupt enable  */
#define DMA_CCR_HTIE_POS       (2U)
#define DMA_CCR_HTIE_MSK       (0x1UL << DMA_CCR_HTIE_POS)                     /*!< 0x00000004 */
#define DMA_CCR_HTIE           DMA_CCR_HTIE_MSK                                /*!< Half Transfer interrupt enable      */
#define DMA_CCR_TEIE_POS       (3U)
#define DMA_CCR_TEIE_MSK       (0x1UL << DMA_CCR_TEIE_POS)                     /*!< 0x00000008 */
#define DMA_CCR_TEIE           DMA_CCR_TEIE_MSK                                /*!< Transfer error interrupt enable     */
#define DMA_CCR_DIR_POS        (4U)
#define DMA_CCR_DIR_MSK        (0x1UL << DMA_CCR_DIR_POS)                      /*!< 0x00000010 */
#define DMA_CCR_DIR            DMA_CCR_DIR_MSK                                 /*!< Data transfer direction             */
#define DMA_CCR_CIRC_POS       (5U)
#define DMA_CCR_CIRC_MSK       (0x1UL << DMA_CCR_CIRC_POS)                     /*!< 0x00000020 */
#define DMA_CCR_CIRC           DMA_CCR_CIRC_MSK                                /*!< Circular mode                       */
#define DMA_CCR_PINC_POS       (6U)
#define DMA_CCR_PINC_MSK       (0x1UL << DMA_CCR_PINC_POS)                     /*!< 0x00000040 */
#define DMA_CCR_PINC           DMA_CCR_PINC_MSK                                /*!< Peripheral increment mode           */
#define DMA_CCR_MINC_POS       (7U)
#define DMA_CCR_MINC_MSK       (0x1UL << DMA_CCR_MINC_POS)                     /*!< 0x00000080 */
#define DMA_CCR_MINC           DMA_CCR_MINC_MSK                                /*!< Memory increment mode               */

#define DMA_CCR_PSIZE_POS      (8U)
#define DMA_CCR_PSIZE_MSK      (0x3UL << DMA_CCR_PSIZE_POS)                    /*!< 0x00000300 */
#define DMA_CCR_PSIZE          DMA_CCR_PSIZE_MSK                               /*!< PSIZE[1:0] bits (Peripheral size)   */
#define DMA_CCR_PSIZE_0        (0x1UL << DMA_CCR_PSIZE_POS)                    /*!< 0x00000100 */
#define DMA_CCR_PSIZE_1        (0x2UL << DMA_CCR_PSIZE_POS)                    /*!< 0x00000200 */

#define DMA_CCR_MSIZE_POS      (10U)
#define DMA_CCR_MSIZE_MSK      (0x3UL << DMA_CCR_MSIZE_POS)                    /*!< 0x00000C00 */
#define DMA_CCR_MSIZE          DMA_CCR_MSIZE_MSK                               /*!< MSIZE[1:0] bits (Memory size)       */
#define DMA_CCR_MSIZE_0        (0x1UL << DMA_CCR_MSIZE_POS)                    /*!< 0x00000400 */
#define DMA_CCR_MSIZE_1        (0x2UL << DMA_CCR_MSIZE_POS)                    /*!< 0x00000800 */

#define DMA_CCR_PL_POS         (12U)
#define DMA_CCR_PL_MSK         (0x3UL << DMA_CCR_PL_POS)                       /*!< 0x00003000 */
#define DMA_CCR_PL             DMA_CCR_PL_MSK                                  /*!< PL[1:0] bits(CHANNEL Priority level)*/
#define DMA_CCR_PL_0           (0x1UL << DMA_CCR_PL_POS)                       /*!< 0x00001000 */
#define DMA_CCR_PL_1           (0x2UL << DMA_CCR_PL_POS)                        /*!< 0x00002000 */

#define DMA_CCR_MEM2MEM_POS    (14U)
#define DMA_CCR_MEM2MEM_MSK    (0x1UL << DMA_CCR_MEM2MEM_POS)                  /*!< 0x00004000 */
#define DMA_CCR_MEM2MEM        DMA_CCR_MEM2MEM_MSK                             /*!< Memory to memory mode               */

/******************  Bit definition for DMA_CNDTR register  *******************/
#define DMA_CNDTR_NDT_POS      (0U)
#define DMA_CNDTR_NDT_MSK      (0xFFFFUL << DMA_CNDTR_NDT_POS)                 /*!< 0x0000FFFF */
#define DMA_CNDTR_NDT          DMA_CNDTR_NDT_MSK                               /*!< Number of data to Transfer          */

/******************  Bit definition for DMA_CPAR register  ********************/
#define DMA_CPAR_PA_POS        (0U)
#define DMA_CPAR_PA_MSK        (0xFFFFFFFFUL << DMA_CPAR_PA_POS)               /*!< 0xFFFFFFFF */
#define DMA_CPAR_PA            DMA_CPAR_PA_MSK                                 /*!< Peripheral Address                  */

/******************  Bit definition for DMA_CMAR register  ********************/
#define DMA_CMAR_MA_POS        (0U)
#define DMA_CMAR_MA_MSK        (0xFFFFFFFFUL << DMA_CMAR_MA_POS)               /*!< 0xFFFFFFFF */
#define DMA_CMAR_MA            DMA_CMAR_MA_MSK                                 /*!< Memory Address                      */

/********************  Bits definition for DMAMUX_CxCR register  **************/
#define DMAMUX_CxCR_DMAREQ_ID_POS              (0U)
#define DMAMUX_CxCR_DMAREQ_ID_MSK              (0xFFUL << DMAMUX_CxCR_DMAREQ_ID_POS) /*!< 0x000000FF */
#define DMAMUX_CxCR_DMAREQ_ID                  DMAMUX_CxCR_DMAREQ_ID_MSK             /*!< DMA Request ID   */
#define DMAMUX_CxCR_DMAREQ_ID_0                (0x01UL << DMAMUX_CxCR_DMAREQ_ID_POS) /*!< 0x00000001 */
#define DMAMUX_CxCR_DMAREQ_ID_1                (0x02UL << DMAMUX_CxCR_DMAREQ_ID_POS) /*!< 0x00000002 */
#define DMAMUX_CxCR_DMAREQ_ID_2                (0x04UL << DMAMUX_CxCR_DMAREQ_ID_POS) /*!< 0x00000004 */
#define DMAMUX_CxCR_DMAREQ_ID_3                (0x08UL << DMAMUX_CxCR_DMAREQ_ID_POS) /*!< 0x00000008 */
#define DMAMUX_CxCR_DMAREQ_ID_4                (0x10UL << DMAMUX_CxCR_DMAREQ_ID_POS) /*!< 0x00000010 */
#define DMAMUX_CxCR_DMAREQ_ID_5                (0x20UL << DMAMUX_CxCR_DMAREQ_ID_POS) /*!< 0x00000020 */
#define DMAMUX_CxCR_DMAREQ_ID_6                (0x40UL << DMAMUX_CxCR_DMAREQ_ID_POS) /*!< 0x00000040 */
#define DMAMUX_CxCR_DMAREQ_ID_7                (0x80UL << DMAMUX_CxCR_DMAREQ_ID_POS) /*!< 0x00000080 */
#define DMAMUX_CxCR_SOIE_POS                   (8U)
#define DMAMUX_CxCR_SOIE_MSK                   (0x1UL << DMAMUX_CxCR_SOIE_POS)  /*!< 0x00000100 */
#define DMAMUX_CxCR_SOIE                       DMAMUX_CxCR_SOIE_MSK             /*!< Synchro overrun interrupt enable     */
#define DMAMUX_CxCR_EGE_POS                    (9U)
#define DMAMUX_CxCR_EGE_MSK                    (0x1UL << DMAMUX_CxCR_EGE_POS)   /*!< 0x00000200 */
#define DMAMUX_CxCR_EGE                        DMAMUX_CxCR_EGE_MSK              /*!< Event generation interrupt enable    */
#define DMAMUX_CxCR_SE_POS                     (16U)
#define DMAMUX_CxCR_SE_MSK                     (0x1UL << DMAMUX_CxCR_SE_POS)    /*!< 0x00010000 */
#define DMAMUX_CxCR_SE                         DMAMUX_CxCR_SE_MSK               /*!< Synchronization enable               */
#define DMAMUX_CxCR_SPOL_POS                   (17U)
#define DMAMUX_CxCR_SPOL_MSK                   (0x3UL << DMAMUX_CxCR_SPOL_POS)  /*!< 0x00060000 */
#define DMAMUX_CxCR_SPOL                       DMAMUX_CxCR_SPOL_MSK             /*!< Synchronization polarity             */
#define DMAMUX_CxCR_SPOL_0                     (0x1UL << DMAMUX_CxCR_SPOL_POS)  /*!< 0x00020000 */
#define DMAMUX_CxCR_SPOL_1                     (0x2UL << DMAMUX_CxCR_SPOL_POS)  /*!< 0x00040000 */
#define DMAMUX_CxCR_NBREQ_POS                  (19U)
#define DMAMUX_CxCR_NBREQ_MSK                  (0x1FUL << DMAMUX_CxCR_NBREQ_POS) /*!< 0x00F80000 */
#define DMAMUX_CxCR_NBREQ                      DMAMUX_CxCR_NBREQ_MSK             /*!< Number of request                    */
#define DMAMUX_CxCR_NBREQ_0                    (0x01UL << DMAMUX_CxCR_NBREQ_POS) /*!< 0x00080000 */
#define DMAMUX_CxCR_NBREQ_1                    (0x02UL << DMAMUX_CxCR_NBREQ_POS) /*!< 0x00100000 */
#define DMAMUX_CxCR_NBREQ_2                    (0x04UL << DMAMUX_CxCR_NBREQ_POS) /*!< 0x00200000 */
#define DMAMUX_CxCR_NBREQ_3                    (0x08UL << DMAMUX_CxCR_NBREQ_POS) /*!< 0x00400000 */
#define DMAMUX_CxCR_NBREQ_4                    (0x10UL << DMAMUX_CxCR_NBREQ_POS) /*!< 0x00800000 */
#define DMAMUX_CxCR_SYNC_ID_POS                (24U)
#define DMAMUX_CxCR_SYNC_ID_MSK                (0x1FUL << DMAMUX_CxCR_SYNC_ID_POS) /*!< 0x1F000000 */
#define DMAMUX_CxCR_SYNC_ID                    DMAMUX_CxCR_SYNC_ID_MSK             /*!< Synchronization ID                   */
#define DMAMUX_CxCR_SYNC_ID_0                  (0x01UL << DMAMUX_CxCR_SYNC_ID_POS) /*!< 0x01000000 */
#define DMAMUX_CxCR_SYNC_ID_1                  (0x02UL << DMAMUX_CxCR_SYNC_ID_POS) /*!< 0x02000000 */
#define DMAMUX_CxCR_SYNC_ID_2                  (0x04UL << DMAMUX_CxCR_SYNC_ID_POS) /*!< 0x04000000 */
#define DMAMUX_CxCR_SYNC_ID_3                  (0x08UL << DMAMUX_CxCR_SYNC_ID_POS) /*!< 0x08000000 */
#define DMAMUX_CxCR_SYNC_ID_4                  (0x10UL << DMAMUX_CxCR_SYNC_ID_POS) /*!< 0x10000000 */

/*******************  Bits definition for DMAMUX_CSR register  **************/
#define DMAMUX_CSR_SOF0_POS                    (0U)
#define DMAMUX_CSR_SOF0_MSK                    (0x1UL << DMAMUX_CSR_SOF0_POS)  /*!< 0x00000001 */
#define DMAMUX_CSR_SOF0                        DMAMUX_CSR_SOF0_MSK             /*!< Synchronization Overrun Flag 0       */
#define DMAMUX_CSR_SOF1_POS                    (1U)
#define DMAMUX_CSR_SOF1_MSK                    (0x1UL << DMAMUX_CSR_SOF1_POS)  /*!< 0x00000002 */
#define DMAMUX_CSR_SOF1                        DMAMUX_CSR_SOF1_MSK             /*!< Synchronization Overrun Flag 1       */
#define DMAMUX_CSR_SOF2_POS                    (2U)
#define DMAMUX_CSR_SOF2_MSK                    (0x1UL << DMAMUX_CSR_SOF2_POS)  /*!< 0x00000004 */
#define DMAMUX_CSR_SOF2                        DMAMUX_CSR_SOF2_MSK             /*!< Synchronization Overrun Flag 2       */
#define DMAMUX_CSR_SOF3_POS                    (3U)
#define DMAMUX_CSR_SOF3_MSK                    (0x1UL << DMAMUX_CSR_SOF3_POS)  /*!< 0x00000008 */
#define DMAMUX_CSR_SOF3                        DMAMUX_CSR_SOF3_MSK             /*!< Synchronization Overrun Flag 3       */
#define DMAMUX_CSR_SOF4_POS                    (4U)
#define DMAMUX_CSR_SOF4_MSK                    (0x1UL << DMAMUX_CSR_SOF4_POS)  /*!< 0x00000010 */
#define DMAMUX_CSR_SOF4                        DMAMUX_CSR_SOF4_MSK             /*!< Synchronization Overrun Flag 4       */
#define DMAMUX_CSR_SOF5_POS                    (5U)
#define DMAMUX_CSR_SOF5_MSK                    (0x1UL << DMAMUX_CSR_SOF5_POS)  /*!< 0x00000020 */
#define DMAMUX_CSR_SOF5                        DMAMUX_CSR_SOF5_MSK             /*!< Synchronization Overrun Flag 5       */
#define DMAMUX_CSR_SOF6_POS                    (6U)
#define DMAMUX_CSR_SOF6_MSK                    (0x1UL << DMAMUX_CSR_SOF6_POS)  /*!< 0x00000040 */
#define DMAMUX_CSR_SOF6                        DMAMUX_CSR_SOF6_MSK             /*!< Synchronization Overrun Flag 6       */

/********************  Bits definition for DMAMUX_CFR register  **************/
#define DMAMUX_CFR_CSOF0_POS                   (0U)
#define DMAMUX_CFR_CSOF0_MSK                   (0x1UL << DMAMUX_CFR_CSOF0_POS)  /*!< 0x00000001 */
#define DMAMUX_CFR_CSOF0                       DMAMUX_CFR_CSOF0_MSK             /*!< Clear Overrun Flag 0                 */
#define DMAMUX_CFR_CSOF1_POS                   (1U)
#define DMAMUX_CFR_CSOF1_MSK                   (0x1UL << DMAMUX_CFR_CSOF1_POS)  /*!< 0x00000002 */
#define DMAMUX_CFR_CSOF1                       DMAMUX_CFR_CSOF1_MSK             /*!< Clear Overrun Flag 1                 */
#define DMAMUX_CFR_CSOF2_POS                   (2U)
#define DMAMUX_CFR_CSOF2_MSK                   (0x1UL << DMAMUX_CFR_CSOF2_POS)  /*!< 0x00000004 */
#define DMAMUX_CFR_CSOF2                       DMAMUX_CFR_CSOF2_MSK             /*!< Clear Overrun Flag 2                 */
#define DMAMUX_CFR_CSOF3_POS                   (3U)
#define DMAMUX_CFR_CSOF3_MSK                   (0x1UL << DMAMUX_CFR_CSOF3_POS)  /*!< 0x00000008 */
#define DMAMUX_CFR_CSOF3                       DMAMUX_CFR_CSOF3_MSK             /*!< Clear Overrun Flag 3                 */
#define DMAMUX_CFR_CSOF4_POS                   (4U)
#define DMAMUX_CFR_CSOF4_MSK                   (0x1UL << DMAMUX_CFR_CSOF4_POS)  /*!< 0x00000010 */
#define DMAMUX_CFR_CSOF4                       DMAMUX_CFR_CSOF4_MSK             /*!< Clear Overrun Flag 4                 */
#define DMAMUX_CFR_CSOF5_POS                   (5U)
#define DMAMUX_CFR_CSOF5_MSK                   (0x1UL << DMAMUX_CFR_CSOF5_POS)  /*!< 0x00000020 */
#define DMAMUX_CFR_CSOF5                       DMAMUX_CFR_CSOF5_MSK             /*!< Clear Overrun Flag 5                 */
#define DMAMUX_CFR_CSOF6_POS                   (6U)
#define DMAMUX_CFR_CSOF6_MSK                   (0x1UL << DMAMUX_CFR_CSOF6_POS)  /*!< 0x00000040 */
#define DMAMUX_CFR_CSOF6                       DMAMUX_CFR_CSOF6_MSK             /*!< Clear Overrun Flag 6                 */

/********************  Bits definition for DMAMUX_RGxCR register  ************/
#define DMAMUX_RGxCR_SIG_ID_POS                (0U)
#define DMAMUX_RGxCR_SIG_ID_MSK                (0x1FUL << DMAMUX_RGxCR_SIG_ID_POS) /*!< 0x0000001F */
#define DMAMUX_RGxCR_SIG_ID                    DMAMUX_RGxCR_SIG_ID_MSK             /*!< Signal ID                         */
#define DMAMUX_RGxCR_SIG_ID_0                  (0x01UL << DMAMUX_RGxCR_SIG_ID_POS) /*!< 0x00000001 */
#define DMAMUX_RGxCR_SIG_ID_1                  (0x02UL << DMAMUX_RGxCR_SIG_ID_POS) /*!< 0x00000002 */
#define DMAMUX_RGxCR_SIG_ID_2                  (0x04UL << DMAMUX_RGxCR_SIG_ID_POS) /*!< 0x00000004 */
#define DMAMUX_RGxCR_SIG_ID_3                  (0x08UL << DMAMUX_RGxCR_SIG_ID_POS) /*!< 0x00000008 */
#define DMAMUX_RGxCR_SIG_ID_4                  (0x10UL << DMAMUX_RGxCR_SIG_ID_POS) /*!< 0x00000010 */
#define DMAMUX_RGxCR_OIE_POS                   (8U)
#define DMAMUX_RGxCR_OIE_MSK                   (0x1UL << DMAMUX_RGxCR_OIE_POS)  /*!< 0x00000100 */
#define DMAMUX_RGxCR_OIE                       DMAMUX_RGxCR_OIE_MSK             /*!< Overrun interrupt enable             */
#define DMAMUX_RGxCR_GE_POS                    (16U)
#define DMAMUX_RGxCR_GE_MSK                    (0x1UL << DMAMUX_RGxCR_GE_POS)   /*!< 0x00010000 */
#define DMAMUX_RGxCR_GE                        DMAMUX_RGxCR_GE_MSK              /*!< Generation enable                    */
#define DMAMUX_RGxCR_GPOL_POS                  (17U)
#define DMAMUX_RGxCR_GPOL_MSK                  (0x3UL << DMAMUX_RGxCR_GPOL_POS) /*!< 0x00060000 */
#define DMAMUX_RGxCR_GPOL                      DMAMUX_RGxCR_GPOL_MSK            /*!< Generation polarity                  */
#define DMAMUX_RGxCR_GPOL_0                    (0x1UL << DMAMUX_RGxCR_GPOL_POS) /*!< 0x00020000 */
#define DMAMUX_RGxCR_GPOL_1                    (0x2UL << DMAMUX_RGxCR_GPOL_POS) /*!< 0x00040000 */
#define DMAMUX_RGxCR_GNBREQ_POS                (19U)
#define DMAMUX_RGxCR_GNBREQ_MSK                (0x1FUL << DMAMUX_RGxCR_GNBREQ_POS) /*!< 0x00F80000 */
#define DMAMUX_RGxCR_GNBREQ                    DMAMUX_RGxCR_GNBREQ_MSK             /*!< Number of request                 */
#define DMAMUX_RGxCR_GNBREQ_0                  (0x01UL << DMAMUX_RGxCR_GNBREQ_POS) /*!< 0x00080000 */
#define DMAMUX_RGxCR_GNBREQ_1                  (0x02UL << DMAMUX_RGxCR_GNBREQ_POS) /*!< 0x00100000 */
#define DMAMUX_RGxCR_GNBREQ_2                  (0x04UL << DMAMUX_RGxCR_GNBREQ_POS) /*!< 0x00200000 */
#define DMAMUX_RGxCR_GNBREQ_3                  (0x08UL << DMAMUX_RGxCR_GNBREQ_POS) /*!< 0x00400000 */
#define DMAMUX_RGxCR_GNBREQ_4                  (0x10UL << DMAMUX_RGxCR_GNBREQ_POS) /*!< 0x00800000 */

/********************  Bits definition for DMAMUX_RGSR register  **************/
#define DMAMUX_RGSR_OF0_POS                    (0U)
#define DMAMUX_RGSR_OF0_MSK                    (0x1UL << DMAMUX_RGSR_OF0_POS)   /*!< 0x00000001 */
#define DMAMUX_RGSR_OF0                        DMAMUX_RGSR_OF0_MSK              /*!< Overrun flag 0                       */
#define DMAMUX_RGSR_OF1_POS                    (1U)
#define DMAMUX_RGSR_OF1_MSK                    (0x1UL << DMAMUX_RGSR_OF1_POS)   /*!< 0x00000002 */
#define DMAMUX_RGSR_OF1                        DMAMUX_RGSR_OF1_MSK              /*!< Overrun flag 1                       */
#define DMAMUX_RGSR_OF2_POS                    (2U)
#define DMAMUX_RGSR_OF2_MSK                    (0x1UL << DMAMUX_RGSR_OF2_POS)   /*!< 0x00000004 */
#define DMAMUX_RGSR_OF2                        DMAMUX_RGSR_OF2_MSK              /*!< Overrun flag 2                       */
#define DMAMUX_RGSR_OF3_POS                    (3U)
#define DMAMUX_RGSR_OF3_MSK                    (0x1UL << DMAMUX_RGSR_OF3_POS)   /*!< 0x00000008 */
#define DMAMUX_RGSR_OF3                        DMAMUX_RGSR_OF3_MSK              /*!< Overrun flag 3                       */

/********************  Bits definition for DMAMUX_RGCFR register  **************/
#define DMAMUX_RGCFR_COF0_POS                  (0U)
#define DMAMUX_RGCFR_COF0_MSK                  (0x1UL << DMAMUX_RGCFR_COF0_POS) /*!< 0x00000001 */
#define DMAMUX_RGCFR_COF0                      DMAMUX_RGCFR_COF0_MSK            /*!< Clear Overrun flag 0                 */
#define DMAMUX_RGCFR_COF1_POS                  (1U)
#define DMAMUX_RGCFR_COF1_MSK                  (0x1UL << DMAMUX_RGCFR_COF1_POS) /*!< 0x00000002 */
#define DMAMUX_RGCFR_COF1                      DMAMUX_RGCFR_COF1_MSK            /*!< Clear Overrun flag 1                 */
#define DMAMUX_RGCFR_COF2_POS                  (2U)
#define DMAMUX_RGCFR_COF2_MSK                  (0x1UL << DMAMUX_RGCFR_COF2_POS) /*!< 0x00000004 */
#define DMAMUX_RGCFR_COF2                      DMAMUX_RGCFR_COF2_MSK            /*!< Clear Overrun flag 2                 */
#define DMAMUX_RGCFR_COF3_POS                  (3U)
#define DMAMUX_RGCFR_COF3_MSK                  (0x1UL << DMAMUX_RGCFR_COF3_POS) /*!< 0x00000008 */
#define DMAMUX_RGCFR_COF3                      DMAMUX_RGCFR_COF3_MSK            /*!< Clear Overrun flag 3                 */


/** @defgroup DMAMUX_LL_EC_REQUEST Transfer request
  * @{
  */
typedef enum
{
    DMAMUX_REQ_MEM2MEM           =  0x00000000U,   /*!< memory to memory transfer  */
    DMAMUX_REQ_GENERATOR0        =  0x00000001U,   /*!< DMAMUX request generator 0 */
    DMAMUX_REQ_GENERATOR1        =  0x00000002U,   /*!< DMAMUX request generator 1 */
    DMAMUX_REQ_GENERATOR2        =  0x00000003U,   /*!< DMAMUX request generator 2 */
    DMAMUX_REQ_GENERATOR3        =  0x00000004U,   /*!< DMAMUX request generator 3 */
    DMAMUX_REQ_ADC1              =  0x00000005U,   /*!< DMAMUX ADC1 request        */
    DMAMUX_REQ_AES_IN            =  0x00000006U,   /*!< DMAMUX AES_IN request      */
    DMAMUX_REQ_AES_OUT           =  0x00000007U,   /*!< DMAMUX AES_OUT request     */
    DMAMUX_REQ_DAC1_CH1          =  0x00000008U,   /*!< DMAMUX DAC_CH1 request     */
    DMAMUX_REQ_DAC1_CH2          =  0x00000009U,   /*!< DMAMUX DAC_CH2 request     */
    DMAMUX_REQ_I2C1_RX           =  0x0000000AU,   /*!< DMAMUX I2C1 RX request     */
    DMAMUX_REQ_I2C1_TX           =  0x0000000BU,   /*!< DMAMUX I2C1 TX request     */
    DMAMUX_REQ_I2C2_RX           =  0x0000000CU,   /*!< DMAMUX I2C2 RX request     */
    DMAMUX_REQ_I2C2_TX           =  0x0000000DU,   /*!< DMAMUX I2C2 TX request     */
    DMAMUX_REQ_LPUART1_RX        =  0x0000000EU,   /*!< DMAMUX LPUART1 RX request  */
    DMAMUX_REQ_LPUART1_TX        =  0x0000000FU,   /*!< DMAMUX LPUART1 TX request  */
    DMAMUX_REQ_SPI1_RX           =  0x00000010U,   /*!< DMAMUX SPI1 RX request     */
    DMAMUX_REQ_SPI1_TX           =  0x00000011U,   /*!< DMAMUX SPI1 TX request     */
    DMAMUX_REQ_SPI2_RX           =  0x00000012U,   /*!< DMAMUX SPI2 RX request     */
    DMAMUX_REQ_SPI2_TX           =  0x00000013U,   /*!< DMAMUX SPI2 TX request     */
    DMAMUX_REQ_TIM1_CH1          =  0x00000014U,   /*!< DMAMUX TIM1 CH1 request    */
    DMAMUX_REQ_TIM1_CH2          =  0x00000015U,   /*!< DMAMUX TIM1 CH2 request    */
    DMAMUX_REQ_TIM1_CH3          =  0x00000016U,   /*!< DMAMUX TIM1 CH3 request    */
    DMAMUX_REQ_TIM1_CH4          =  0x00000017U,   /*!< DMAMUX TIM1 CH4 request    */
    DMAMUX_REQ_TIM1_TRIG_COM     =  0x00000018U,   /*!< DMAMUX TIM1 TRIG COM request */
    DMAMUX_REQ_TIM1_UP           =  0x00000019U,   /*!< DMAMUX TIM1 UP request     */
    DMAMUX_REQ_TIM2_CH1          =  0x0000001AU,   /*!< DMAMUX TIM2 CH1 request    */
    DMAMUX_REQ_TIM2_CH2          =  0x0000001BU,   /*!< DMAMUX TIM2 CH2 request    */
    DMAMUX_REQ_TIM2_CH3          =  0x0000001CU,   /*!< DMAMUX TIM2 CH3 request    */
    DMAMUX_REQ_TIM2_CH4          =  0x0000001DU,   /*!< DMAMUX TIM2 CH4 request    */
    DMAMUX_REQ_TIM2_TRIG         =  0x0000001EU,   /*!< DMAMUX TIM2 TRIG request   */
    DMAMUX_REQ_TIM2_UP           =  0x0000001FU,   /*!< DMAMUX TIM2 UP request     */
    DMAMUX_REQ_TIM3_CH1          =  0x00000020U,   /*!< DMAMUX TIM3 CH1 request    */
    DMAMUX_REQ_TIM3_CH2          =  0x00000021U,   /*!< DMAMUX TIM3 CH2 request    */
    DMAMUX_REQ_TIM3_CH3          =  0x00000022U,   /*!< DMAMUX TIM3 CH3 request    */
    DMAMUX_REQ_TIM3_CH4          =  0x00000023U,   /*!< DMAMUX TIM3 CH4 request    */
    DMAMUX_REQ_TIM3_TRIG         =  0x00000024U,   /*!< DMAMUX TIM3 TRIG request   */
    DMAMUX_REQ_TIM3_UP           =  0x00000025U,   /*!< DMAMUX TIM3 UP request     */
    DMAMUX_REQ_TIM6_UP           =  0x00000026U,   /*!< DMAMUX TIM6 UP request     */
    DMAMUX_REQ_TIM7_UP           =  0x00000027U,   /*!< DMAMUX TIM7 UP request     */
    DMAMUX_REQ_TIM15_CH1         =  0x00000028U,   /*!< DMAMUX TIM15 CH1 request   */
    DMAMUX_REQ_TIM15_CH2         =  0x00000029U,   /*!< DMAMUX TIM15 CH2 request   */
    DMAMUX_REQ_TIM15_TRIG_COM    =  0x0000002AU,   /*!< DMAMUX TIM15 TRIG COM request */
    DMAMUX_REQ_TIM15_UP          =  0x0000002BU,   /*!< DMAMUX TIM15 UP request    */
    DMAMUX_REQ_TIM16_CH1         =  0x0000002CU,   /*!< DMAMUX TIM16 CH1 request   */
    DMAMUX_REQ_TIM16_COM         =  0x0000002DU,   /*!< DMAMUX TIM16 COM request   */
    DMAMUX_REQ_TIM16_UP          =  0x0000002EU,   /*!< DMAMUX TIM16 UP request    */
    DMAMUX_REQ_TIM17_CH1         =  0x0000002FU,   /*!< DMAMUX TIM17 CH1 request   */
    DMAMUX_REQ_TIM17_COM         =  0x00000030U,   /*!< DMAMUX TIM17 COM request   */
    DMAMUX_REQ_TIM17_UP          =  0x00000031U,   /*!< DMAMUX TIM17 UP request    */
    DMAMUX_REQ_USART1_RX         =  0x00000032U,   /*!< DMAMUX USART1 RX request  */
    DMAMUX_REQ_USART1_TX         =  0x00000033U,   /*!< DMAMUX USART1 TX request  */
    DMAMUX_REQ_USART2_RX         =  0x00000034U,   /*!< DMAMUX USART2 RX request  */
    DMAMUX_REQ_USART2_TX         =  0x00000035U,   /*!< DMAMUX USART2 TX request  */
    DMAMUX_REQ_USART3_RX         =  0x00000036U,   /*!< DMAMUX USART3 RX request  */
    DMAMUX_REQ_USART3_TX         =  0x00000037U,   /*!< DMAMUX USART3 TX request  */
    DMAMUX_REQ_USART4_RX         =  0x00000038U,   /*!< DMAMUX USART4 RX request  */
    DMAMUX_REQ_USART4_TX         =  0x00000039U,   /*!< DMAMUX USART4 TX request  */
    DMAMUX_REQ_UCPD1_RX          =  0x0000003AU,   /*!< DMAMUX UCPD1 RX request  */
    DMAMUX_REQ_UCPD1_TX          =  0x0000003BU,   /*!< DMAMUX UCPD1 TX request  */
    DMAMUX_REQ_UCPD2_RX          =  0x0000003CU,   /*!< DMAMUX UCPD2 RX request  */
    DMAMUX_REQ_UCPD2_TX          =  0x0000003DU,   /*!< DMAMUX UCPD2 TX request  */
    DMAMUX_REQ_I2C3_RX           =  0x0000003EU,   /*!< DMAMUX I2C3 RX request  */
    DMAMUX_REQ_I2C3_TX           =  0x0000003FU,   /*!< DMAMUX I2C3 TX request  */
    DMAMUX_REQ_LPUART2_RX        =  0x00000040U,   /*!< DMAMUX LPUART2 RX request  */
    DMAMUX_REQ_LPUART2_TX        =  0x00000041U,   /*!< DMAMUX LPUART2 TX request  */
    DMAMUX_REQ_SPI3_RX           =  0x00000042U,   /*!< DMAMUX SPI3 RX request     */
    DMAMUX_REQ_SPI3_TX           =  0x00000043U,   /*!< DMAMUX SPI3 TX request     */
    DMAMUX_REQ_TIM4_CH1          =  0x00000044U,   /*!< DMAMUX TIM4 CH1 request    */
    DMAMUX_REQ_TIM4_CH2          =  0x00000045U,   /*!< DMAMUX TIM4 CH2 request    */
    DMAMUX_REQ_TIM4_CH3          =  0x00000046U,   /*!< DMAMUX TIM4 CH3 request    */
    DMAMUX_REQ_TIM4_CH4          =  0x00000047U,   /*!< DMAMUX TIM4 CH4 request    */
    DMAMUX_REQ_TIM4_TRIG         =  0x00000048U,   /*!< DMAMUX TIM4 TRIG request   */
    DMAMUX_REQ_TIM4_UP           =  0x00000049U,   /*!< DMAMUX TIM4 UP request     */
    DMAMUX_REQ_USART5_RX         =  0x0000004AU,   /*!< DMAMUX USART5 RX request  */
    DMAMUX_REQ_USART5_TX         =  0x0000004BU,   /*!< DMAMUX USART5 TX request  */
    DMAMUX_REQ_USART6_RX         =  0x0000004CU,   /*!< DMAMUX USART6 RX request  */
    DMAMUX_REQ_USART6_TX         =  0x0000004DU,   /*!< DMAMUX USART6 TX request  */
    DMAMUX_MAX_REQ               =  DMAMUX_REQ_USART6_TX
}DMA_MUX_REQ_ID_TYPE;




/**
  * @}
  */

/** @defgroup DMAMUX_LL_EC_CHANNEL DMAMUX Channel
  * @{
  */
#define LL_DMAMUX_CHANNEL_0               0x00000000U               /*!< DMAMUX Channel 0 connected to DMA1 Channel 1  */
#define LL_DMAMUX_CHANNEL_1               0x00000001U               /*!< DMAMUX Channel 1 connected to DMA1 Channel 2  */
#define LL_DMAMUX_CHANNEL_2               0x00000002U               /*!< DMAMUX Channel 2 connected to DMA1 Channel 3  */
#define LL_DMAMUX_CHANNEL_3               0x00000003U               /*!< DMAMUX Channel 3 connected to DMA1 Channel 4  */
#define LL_DMAMUX_CHANNEL_4               0x00000004U               /*!< DMAMUX Channel 4 connected to DMA1 Channel 5  */
#define LL_DMAMUX_CHANNEL_5               0x00000005U               /*!< DMAMUX Channel 5 connected to DMA1 Channel 6  */
#define LL_DMAMUX_CHANNEL_6               0x00000006U               /*!< DMAMUX Channel 6 connected to DMA1 Channel 7  */
#define LL_DMAMUX_CHANNEL_7               0x00000007U               /*!< DMAMUX Channel 7 connected to DMA2 Channel 1  */
#define LL_DMAMUX_CHANNEL_8               0x00000008U               /*!< DMAMUX Channel 8 connected to DMA2 Channel 2  */
#define LL_DMAMUX_CHANNEL_9               0x00000009U               /*!< DMAMUX Channel 9 connected to DMA2 Channel 3  */
#define LL_DMAMUX_CHANNEL_10              0x0000000AU               /*!< DMAMUX Channel 10 connected to DMA2 Channel 4 */
#define LL_DMAMUX_CHANNEL_11              0x0000000BU               /*!< DMAMUX Channel 11 connected to DMA2 Channel 5 */


//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- EXTERNAL INTERRUPT CONTROL REGISTERS -----------------------------------
//---------------------------------------------------------------------------------------------------------------------
typedef struct
{
  volatile uint32 RTSR1;          /*!< EXTI Rising Trigger Selection Register 1,        Address offset:   0x00 */
  volatile uint32 FTSR1;          /*!< EXTI Falling Trigger Selection Register 1,       Address offset:   0x04 */
  volatile uint32 SWIER1;         /*!< EXTI Software Interrupt event Register 1,        Address offset:   0x08 */
  volatile uint32 RPR1;           /*!< EXTI Rising Pending Register 1,                  Address offset:   0x0C */
  volatile uint32 FPR1;           /*!< EXTI Falling Pending Register 1,                 Address offset:   0x10 */
       uint32 RESERVED1[3];   /*!< Reserved 1,                                                0x14 -- 0x1C */
  volatile uint32 RTSR2;          /*!< EXTI Rising Trigger Selection Register 2,        Address offset:   0x20 */
  volatile uint32 FTSR2;          /*!< EXTI Falling Trigger Selection Register 2,       Address offset:   0x24 */
  volatile uint32 SWIER2;         /*!< EXTI Software Interrupt event Register 2,        Address offset:   0x28 */
  volatile uint32 RPR2;           /*!< EXTI Rising Pending Register 2,                  Address offset:   0x2C */
  volatile uint32 FPR2;           /*!< EXTI Falling Pending Register 2,                 Address offset:   0x30 */
       uint32 RESERVED3[11];  /*!< Reserved 3,                                                0x34 -- 0x5C */
  volatile uint32 EXTICR[4];      /*!< EXTI External Interrupt Configuration Register,            0x60 -- 0x6C */
       uint32 RESERVED4[4];   /*!< Reserved 4,                                                0x70 -- 0x7C */
  volatile uint32 IMR1;           /*!< EXTI Interrupt Mask Register 1,                  Address offset:   0x80 */
  volatile uint32 EMR1;           /*!< EXTI Event Mask Register 1,                      Address offset:   0x84 */
       uint32 RESERVED5[2];   /*!< Reserved 5,                                                0x88 -- 0x8C */
  volatile uint32 IMR2;           /*!< EXTI Interrupt Mask Register 2,                  Address offset:   0x90 */
  volatile uint32 EMR2;           /*!< EXTI Event Mask Register 2,                      Address offset:   0x94 */
} EXTI_TYPEDEF;

#define EXTI                (*(EXTI_TYPEDEF *) EXTI_BASE)

/******************  Bit definition for EXTI_RTSR1 register  ******************/
#define EXTI_RTSR1_RT0_POS           (0U)
#define EXTI_RTSR1_RT0_Msk           (0x1UL << EXTI_RTSR1_RT0_POS)             /*!< 0x00000001 */
#define EXTI_RTSR1_RT0               EXTI_RTSR1_RT0_Msk                        /*!< Rising trigger configuration for input line 0 */
#define EXTI_RTSR1_RT1_POS           (1U)
#define EXTI_RTSR1_RT1_Msk           (0x1UL << EXTI_RTSR1_RT1_POS)             /*!< 0x00000002 */
#define EXTI_RTSR1_RT1               EXTI_RTSR1_RT1_Msk                        /*!< Rising trigger configuration for input line 1 */
#define EXTI_RTSR1_RT2_POS           (2U)
#define EXTI_RTSR1_RT2_Msk           (0x1UL << EXTI_RTSR1_RT2_POS)             /*!< 0x00000004 */
#define EXTI_RTSR1_RT2               EXTI_RTSR1_RT2_Msk                        /*!< Rising trigger configuration for input line 2 */
#define EXTI_RTSR1_RT3_POS           (3U)
#define EXTI_RTSR1_RT3_Msk           (0x1UL << EXTI_RTSR1_RT3_POS)             /*!< 0x00000008 */
#define EXTI_RTSR1_RT3               EXTI_RTSR1_RT3_Msk                        /*!< Rising trigger configuration for input line 3 */
#define EXTI_RTSR1_RT4_POS           (4U)
#define EXTI_RTSR1_RT4_Msk           (0x1UL << EXTI_RTSR1_RT4_POS)             /*!< 0x00000010 */
#define EXTI_RTSR1_RT4               EXTI_RTSR1_RT4_Msk                        /*!< Rising trigger configuration for input line 4 */
#define EXTI_RTSR1_RT5_POS           (5U)
#define EXTI_RTSR1_RT5_Msk           (0x1UL << EXTI_RTSR1_RT5_POS)             /*!< 0x00000020 */
#define EXTI_RTSR1_RT5               EXTI_RTSR1_RT5_Msk                        /*!< Rising trigger configuration for input line 5 */
#define EXTI_RTSR1_RT6_POS           (6U)
#define EXTI_RTSR1_RT6_Msk           (0x1UL << EXTI_RTSR1_RT6_POS)             /*!< 0x00000040 */
#define EXTI_RTSR1_RT6               EXTI_RTSR1_RT6_Msk                        /*!< Rising trigger configuration for input line 6 */
#define EXTI_RTSR1_RT7_POS           (7U)
#define EXTI_RTSR1_RT7_Msk           (0x1UL << EXTI_RTSR1_RT7_POS)             /*!< 0x00000080 */
#define EXTI_RTSR1_RT7               EXTI_RTSR1_RT7_Msk                        /*!< Rising trigger configuration for input line 7 */
#define EXTI_RTSR1_RT8_POS           (8U)
#define EXTI_RTSR1_RT8_Msk           (0x1UL << EXTI_RTSR1_RT8_POS)             /*!< 0x00000100 */
#define EXTI_RTSR1_RT8               EXTI_RTSR1_RT8_Msk                        /*!< Rising trigger configuration for input line 8 */
#define EXTI_RTSR1_RT9_POS           (9U)
#define EXTI_RTSR1_RT9_Msk           (0x1UL << EXTI_RTSR1_RT9_POS)             /*!< 0x00000200 */
#define EXTI_RTSR1_RT9               EXTI_RTSR1_RT9_Msk                        /*!< Rising trigger configuration for input line 9 */
#define EXTI_RTSR1_RT10_POS          (10U)
#define EXTI_RTSR1_RT10_Msk          (0x1UL << EXTI_RTSR1_RT10_POS)            /*!< 0x00000400 */
#define EXTI_RTSR1_RT10              EXTI_RTSR1_RT10_Msk                       /*!< Rising trigger configuration for input line 10 */
#define EXTI_RTSR1_RT11_POS          (11U)
#define EXTI_RTSR1_RT11_Msk          (0x1UL << EXTI_RTSR1_RT11_POS)            /*!< 0x00000800 */
#define EXTI_RTSR1_RT11              EXTI_RTSR1_RT11_Msk                       /*!< Rising trigger configuration for input line 11 */
#define EXTI_RTSR1_RT12_POS          (12U)
#define EXTI_RTSR1_RT12_Msk          (0x1UL << EXTI_RTSR1_RT12_POS)            /*!< 0x00001000 */
#define EXTI_RTSR1_RT12              EXTI_RTSR1_RT12_Msk                       /*!< Rising trigger configuration for input line 12 */
#define EXTI_RTSR1_RT13_POS          (13U)
#define EXTI_RTSR1_RT13_Msk          (0x1UL << EXTI_RTSR1_RT13_POS)            /*!< 0x00002000 */
#define EXTI_RTSR1_RT13              EXTI_RTSR1_RT13_Msk                       /*!< Rising trigger configuration for input line 13 */
#define EXTI_RTSR1_RT14_POS          (14U)
#define EXTI_RTSR1_RT14_Msk          (0x1UL << EXTI_RTSR1_RT14_POS)            /*!< 0x00004000 */
#define EXTI_RTSR1_RT14              EXTI_RTSR1_RT14_Msk                       /*!< Rising trigger configuration for input line 14 */
#define EXTI_RTSR1_RT15_POS          (15U)
#define EXTI_RTSR1_RT15_Msk          (0x1UL << EXTI_RTSR1_RT15_POS)            /*!< 0x00008000 */
#define EXTI_RTSR1_RT15              EXTI_RTSR1_RT15_Msk                       /*!< Rising trigger configuration for input line 15 */
#define EXTI_RTSR1_RT16_POS          (16U)
#define EXTI_RTSR1_RT16_Msk          (0x1UL << EXTI_RTSR1_RT16_POS)            /*!< 0x00010000 */
#define EXTI_RTSR1_RT16              EXTI_RTSR1_RT16_Msk                       /*!< Rising trigger configuration for input line 16 */
#define EXTI_RTSR1_RT17_POS          (17U)
#define EXTI_RTSR1_RT17_Msk          (0x1UL << EXTI_RTSR1_RT17_POS)            /*!< 0x00020000 */
#define EXTI_RTSR1_RT17              EXTI_RTSR1_RT17_Msk                       /*!< Rising trigger configuration for input line 17 */
#define EXTI_RTSR1_RT18_POS          (18U)
#define EXTI_RTSR1_RT18_Msk          (0x1UL << EXTI_RTSR1_RT18_POS)            /*!< 0x00040000 */
#define EXTI_RTSR1_RT18              EXTI_RTSR1_RT18_Msk                       /*!< Rising trigger configuration for input line 18 */
#define EXTI_RTSR1_RT20_POS          (20U)
#define EXTI_RTSR1_RT20_Msk          (0x1UL << EXTI_RTSR1_RT20_POS)            /*!< 0x00100000 */
#define EXTI_RTSR1_RT20              EXTI_RTSR1_RT20_Msk                       /*!< Rising trigger configuration for input line 20 */

/******************  Bit definition for EXTI_FTSR1 register  ******************/
#define EXTI_FTSR1_FT0_POS           (0U)
#define EXTI_FTSR1_FT0_Msk           (0x1UL << EXTI_FTSR1_FT0_POS)             /*!< 0x00000001 */
#define EXTI_FTSR1_FT0               EXTI_FTSR1_FT0_Msk                        /*!< Falling trigger configuration for input line 0 */
#define EXTI_FTSR1_FT1_POS           (1U)
#define EXTI_FTSR1_FT1_Msk           (0x1UL << EXTI_FTSR1_FT1_POS)             /*!< 0x00000002 */
#define EXTI_FTSR1_FT1               EXTI_FTSR1_FT1_Msk                        /*!< Falling trigger configuration for input line 1 */
#define EXTI_FTSR1_FT2_POS           (2U)
#define EXTI_FTSR1_FT2_Msk           (0x1UL << EXTI_FTSR1_FT2_POS)             /*!< 0x00000004 */
#define EXTI_FTSR1_FT2               EXTI_FTSR1_FT2_Msk                        /*!< Falling trigger configuration for input line 2 */
#define EXTI_FTSR1_FT3_POS           (3U)
#define EXTI_FTSR1_FT3_Msk           (0x1UL << EXTI_FTSR1_FT3_POS)             /*!< 0x00000008 */
#define EXTI_FTSR1_FT3               EXTI_FTSR1_FT3_Msk                        /*!< Falling trigger configuration for input line 3 */
#define EXTI_FTSR1_FT4_POS           (4U)
#define EXTI_FTSR1_FT4_Msk           (0x1UL << EXTI_FTSR1_FT4_POS)             /*!< 0x00000010 */
#define EXTI_FTSR1_FT4               EXTI_FTSR1_FT4_Msk                        /*!< Falling trigger configuration for input line 4 */
#define EXTI_FTSR1_FT5_POS           (5U)
#define EXTI_FTSR1_FT5_Msk           (0x1UL << EXTI_FTSR1_FT5_POS)             /*!< 0x00000020 */
#define EXTI_FTSR1_FT5               EXTI_FTSR1_FT5_Msk                        /*!< Falling trigger configuration for input line 5 */
#define EXTI_FTSR1_FT6_POS           (6U)
#define EXTI_FTSR1_FT6_Msk           (0x1UL << EXTI_FTSR1_FT6_POS)             /*!< 0x00000040 */
#define EXTI_FTSR1_FT6               EXTI_FTSR1_FT6_Msk                        /*!< Falling trigger configuration for input line 6 */
#define EXTI_FTSR1_FT7_POS           (7U)
#define EXTI_FTSR1_FT7_Msk           (0x1UL << EXTI_FTSR1_FT7_POS)             /*!< 0x00000080 */
#define EXTI_FTSR1_FT7               EXTI_FTSR1_FT7_Msk                        /*!< Falling trigger configuration for input line 7 */
#define EXTI_FTSR1_FT8_POS           (8U)
#define EXTI_FTSR1_FT8_Msk           (0x1UL << EXTI_FTSR1_FT8_POS)             /*!< 0x00000100 */
#define EXTI_FTSR1_FT8               EXTI_FTSR1_FT8_Msk                        /*!< Falling trigger configuration for input line 8 */
#define EXTI_FTSR1_FT9_POS           (9U)
#define EXTI_FTSR1_FT9_Msk           (0x1UL << EXTI_FTSR1_FT9_POS)             /*!< 0x00000200 */
#define EXTI_FTSR1_FT9               EXTI_FTSR1_FT9_Msk                        /*!< Falling trigger configuration for input line 9 */
#define EXTI_FTSR1_FT10_POS          (10U)
#define EXTI_FTSR1_FT10_Msk          (0x1UL << EXTI_FTSR1_FT10_POS)            /*!< 0x00000400 */
#define EXTI_FTSR1_FT10              EXTI_FTSR1_FT10_Msk                       /*!< Falling trigger configuration for input line 10 */
#define EXTI_FTSR1_FT11_POS          (11U)
#define EXTI_FTSR1_FT11_Msk          (0x1UL << EXTI_FTSR1_FT11_POS)            /*!< 0x00000800 */
#define EXTI_FTSR1_FT11              EXTI_FTSR1_FT11_Msk                       /*!< Falling trigger configuration for input line 11 */
#define EXTI_FTSR1_FT12_POS          (12U)
#define EXTI_FTSR1_FT12_Msk          (0x1UL << EXTI_FTSR1_FT12_POS)            /*!< 0x00001000 */
#define EXTI_FTSR1_FT12              EXTI_FTSR1_FT12_Msk                       /*!< Falling trigger configuration for input line 12 */
#define EXTI_FTSR1_FT13_POS          (13U)
#define EXTI_FTSR1_FT13_Msk          (0x1UL << EXTI_FTSR1_FT13_POS)            /*!< 0x00002000 */
#define EXTI_FTSR1_FT13              EXTI_FTSR1_FT13_Msk                       /*!< Falling trigger configuration for input line 13 */
#define EXTI_FTSR1_FT14_POS          (14U)
#define EXTI_FTSR1_FT14_Msk          (0x1UL << EXTI_FTSR1_FT14_POS)            /*!< 0x00004000 */
#define EXTI_FTSR1_FT14              EXTI_FTSR1_FT14_Msk                       /*!< Falling trigger configuration for input line 14 */
#define EXTI_FTSR1_FT15_POS          (15U)
#define EXTI_FTSR1_FT15_Msk          (0x1UL << EXTI_FTSR1_FT15_POS)            /*!< 0x00008000 */
#define EXTI_FTSR1_FT15              EXTI_FTSR1_FT15_Msk                       /*!< Falling trigger configuration for input line 15 */
#define EXTI_FTSR1_FT16_POS          (16U)
#define EXTI_FTSR1_FT16_Msk          (0x1UL << EXTI_FTSR1_FT16_POS)            /*!< 0x00010000 */
#define EXTI_FTSR1_FT16              EXTI_FTSR1_FT16_Msk                       /*!< Falling trigger configuration for input line 16 */
#define EXTI_FTSR1_FT17_POS          (17U)
#define EXTI_FTSR1_FT17_Msk          (0x1UL << EXTI_FTSR1_FT17_POS)            /*!< 0x00020000 */
#define EXTI_FTSR1_FT17              EXTI_FTSR1_FT17_Msk                       /*!< Falling trigger configuration for input line 17 */
#define EXTI_FTSR1_FT18_POS          (18U)
#define EXTI_FTSR1_FT18_Msk          (0x1UL << EXTI_FTSR1_FT18_POS)            /*!< 0x00040000 */
#define EXTI_FTSR1_FT18              EXTI_FTSR1_FT18_Msk                       /*!< Falling trigger configuration for input line 18 */
#define EXTI_FTSR1_FT20_POS          (20U)
#define EXTI_FTSR1_FT20_Msk          (0x1UL << EXTI_FTSR1_FT20_POS)            /*!< 0x00100000 */
#define EXTI_FTSR1_FT20              EXTI_FTSR1_FT20_Msk                       /*!< Falling trigger configuration for input line 20 */

/******************  Bit definition for EXTI_SWIER1 register  *****************/
#define EXTI_SWIER1_SWI0_POS         (0U)
#define EXTI_SWIER1_SWI0_Msk         (0x1UL << EXTI_SWIER1_SWI0_POS)           /*!< 0x00000001 */
#define EXTI_SWIER1_SWI0             EXTI_SWIER1_SWI0_Msk                      /*!< Software Interrupt on line 0 */
#define EXTI_SWIER1_SWI1_POS         (1U)
#define EXTI_SWIER1_SWI1_Msk         (0x1UL << EXTI_SWIER1_SWI1_POS)           /*!< 0x00000002 */
#define EXTI_SWIER1_SWI1             EXTI_SWIER1_SWI1_Msk                      /*!< Software Interrupt on line 1 */
#define EXTI_SWIER1_SWI2_POS         (2U)
#define EXTI_SWIER1_SWI2_Msk         (0x1UL << EXTI_SWIER1_SWI2_POS)           /*!< 0x00000004 */
#define EXTI_SWIER1_SWI2             EXTI_SWIER1_SWI2_Msk                      /*!< Software Interrupt on line 2 */
#define EXTI_SWIER1_SWI3_POS         (3U)
#define EXTI_SWIER1_SWI3_Msk         (0x1UL << EXTI_SWIER1_SWI3_POS)           /*!< 0x00000008 */
#define EXTI_SWIER1_SWI3             EXTI_SWIER1_SWI3_Msk                      /*!< Software Interrupt on line 3 */
#define EXTI_SWIER1_SWI4_POS         (4U)
#define EXTI_SWIER1_SWI4_Msk         (0x1UL << EXTI_SWIER1_SWI4_POS)           /*!< 0x00000010 */
#define EXTI_SWIER1_SWI4             EXTI_SWIER1_SWI4_Msk                      /*!< Software Interrupt on line 4 */
#define EXTI_SWIER1_SWI5_POS         (5U)
#define EXTI_SWIER1_SWI5_Msk         (0x1UL << EXTI_SWIER1_SWI5_POS)           /*!< 0x00000020 */
#define EXTI_SWIER1_SWI5             EXTI_SWIER1_SWI5_Msk                      /*!< Software Interrupt on line 5 */
#define EXTI_SWIER1_SWI6_POS         (6U)
#define EXTI_SWIER1_SWI6_Msk         (0x1UL << EXTI_SWIER1_SWI6_POS)           /*!< 0x00000040 */
#define EXTI_SWIER1_SWI6             EXTI_SWIER1_SWI6_Msk                      /*!< Software Interrupt on line 6 */
#define EXTI_SWIER1_SWI7_POS         (7U)
#define EXTI_SWIER1_SWI7_Msk         (0x1UL << EXTI_SWIER1_SWI7_POS)           /*!< 0x00000080 */
#define EXTI_SWIER1_SWI7             EXTI_SWIER1_SWI7_Msk                      /*!< Software Interrupt on line 7 */
#define EXTI_SWIER1_SWI8_POS         (8U)
#define EXTI_SWIER1_SWI8_Msk         (0x1UL << EXTI_SWIER1_SWI8_POS)           /*!< 0x00000100 */
#define EXTI_SWIER1_SWI8             EXTI_SWIER1_SWI8_Msk                      /*!< Software Interrupt on line 8 */
#define EXTI_SWIER1_SWI9_POS         (9U)
#define EXTI_SWIER1_SWI9_Msk         (0x1UL << EXTI_SWIER1_SWI9_POS)           /*!< 0x00000200 */
#define EXTI_SWIER1_SWI9             EXTI_SWIER1_SWI9_Msk                      /*!< Software Interrupt on line 9 */
#define EXTI_SWIER1_SWI10_POS        (10U)
#define EXTI_SWIER1_SWI10_Msk        (0x1UL << EXTI_SWIER1_SWI10_POS)          /*!< 0x00000400 */
#define EXTI_SWIER1_SWI10            EXTI_SWIER1_SWI10_Msk                     /*!< Software Interrupt on line 10 */
#define EXTI_SWIER1_SWI11_POS        (11U)
#define EXTI_SWIER1_SWI11_Msk        (0x1UL << EXTI_SWIER1_SWI11_POS)          /*!< 0x00000800 */
#define EXTI_SWIER1_SWI11            EXTI_SWIER1_SWI11_Msk                     /*!< Software Interrupt on line 11 */
#define EXTI_SWIER1_SWI12_POS        (12U)
#define EXTI_SWIER1_SWI12_Msk        (0x1UL << EXTI_SWIER1_SWI12_POS)          /*!< 0x00001000 */
#define EXTI_SWIER1_SWI12            EXTI_SWIER1_SWI12_Msk                     /*!< Software Interrupt on line 12 */
#define EXTI_SWIER1_SWI13_POS        (13U)
#define EXTI_SWIER1_SWI13_Msk        (0x1UL << EXTI_SWIER1_SWI13_POS)          /*!< 0x00002000 */
#define EXTI_SWIER1_SWI13            EXTI_SWIER1_SWI13_Msk                     /*!< Software Interrupt on line 13 */
#define EXTI_SWIER1_SWI14_POS        (14U)
#define EXTI_SWIER1_SWI14_Msk        (0x1UL << EXTI_SWIER1_SWI14_POS)          /*!< 0x00004000 */
#define EXTI_SWIER1_SWI14            EXTI_SWIER1_SWI14_Msk                     /*!< Software Interrupt on line 14 */
#define EXTI_SWIER1_SWI15_POS        (15U)
#define EXTI_SWIER1_SWI15_Msk        (0x1UL << EXTI_SWIER1_SWI15_POS)          /*!< 0x00008000 */
#define EXTI_SWIER1_SWI15            EXTI_SWIER1_SWI15_Msk                     /*!< Software Interrupt on line 15 */
#define EXTI_SWIER1_SWI16_POS        (16U)
#define EXTI_SWIER1_SWI16_Msk        (0x1UL << EXTI_SWIER1_SWI16_POS)          /*!< 0x00010000 */
#define EXTI_SWIER1_SWI16            EXTI_SWIER1_SWI16_Msk                     /*!< Software Interrupt on line 16 */
#define EXTI_SWIER1_SWI17_POS        (17U)
#define EXTI_SWIER1_SWI17_Msk        (0x1UL << EXTI_SWIER1_SWI17_POS)          /*!< 0x00020000 */
#define EXTI_SWIER1_SWI17            EXTI_SWIER1_SWI17_Msk                     /*!< Software Interrupt on line 17 */
#define EXTI_SWIER1_SWI18_POS        (18U)
#define EXTI_SWIER1_SWI18_Msk        (0x1UL << EXTI_SWIER1_SWI18_POS)          /*!< 0x00040000 */
#define EXTI_SWIER1_SWI18            EXTI_SWIER1_SWI18_Msk                     /*!< Software Interrupt on line 18 */
#define EXTI_SWIER1_SWI20_POS        (20U)
#define EXTI_SWIER1_SWI20_Msk        (0x1UL << EXTI_SWIER1_SWI20_POS)          /*!< 0x00100000 */
#define EXTI_SWIER1_SWI20            EXTI_SWIER1_SWI20_Msk                     /*!< Software Interrupt on line 20 */

/*******************  Bit definition for EXTI_RPR1 register  ******************/
#define EXTI_RPR1_RPIF0_POS          (0U)
#define EXTI_RPR1_RPIF0_Msk          (0x1UL << EXTI_RPR1_RPIF0_POS)            /*!< 0x00000001 */
#define EXTI_RPR1_RPIF0              EXTI_RPR1_RPIF0_Msk                       /*!< Rising Pending Interrupt Flag on line 0 */
#define EXTI_RPR1_RPIF1_POS          (1U)
#define EXTI_RPR1_RPIF1_Msk          (0x1UL << EXTI_RPR1_RPIF1_POS)            /*!< 0x00000002 */
#define EXTI_RPR1_RPIF1              EXTI_RPR1_RPIF1_Msk                       /*!< Rising Pending Interrupt Flag on line 1 */
#define EXTI_RPR1_RPIF2_POS          (2U)
#define EXTI_RPR1_RPIF2_Msk          (0x1UL << EXTI_RPR1_RPIF2_POS)            /*!< 0x00000004 */
#define EXTI_RPR1_RPIF2              EXTI_RPR1_RPIF2_Msk                       /*!< Rising Pending Interrupt Flag on line 2 */
#define EXTI_RPR1_RPIF3_POS          (3U)
#define EXTI_RPR1_RPIF3_Msk          (0x1UL << EXTI_RPR1_RPIF3_POS)            /*!< 0x00000008 */
#define EXTI_RPR1_RPIF3              EXTI_RPR1_RPIF3_Msk                       /*!< Rising Pending Interrupt Flag on line 3 */
#define EXTI_RPR1_RPIF4_POS          (4U)
#define EXTI_RPR1_RPIF4_Msk          (0x1UL << EXTI_RPR1_RPIF4_POS)            /*!< 0x00000010 */
#define EXTI_RPR1_RPIF4              EXTI_RPR1_RPIF4_Msk                       /*!< Rising Pending Interrupt Flag on line 4 */
#define EXTI_RPR1_RPIF5_POS          (5U)
#define EXTI_RPR1_RPIF5_Msk          (0x1UL << EXTI_RPR1_RPIF5_POS)            /*!< 0x00000020 */
#define EXTI_RPR1_RPIF5              EXTI_RPR1_RPIF5_Msk                       /*!< Rising Pending Interrupt Flag on line 5 */
#define EXTI_RPR1_RPIF6_POS          (6U)
#define EXTI_RPR1_RPIF6_Msk          (0x1UL << EXTI_RPR1_RPIF6_POS)            /*!< 0x00000040 */
#define EXTI_RPR1_RPIF6              EXTI_RPR1_RPIF6_Msk                       /*!< Rising Pending Interrupt Flag on line 6 */
#define EXTI_RPR1_RPIF7_POS          (7U)
#define EXTI_RPR1_RPIF7_Msk          (0x1UL << EXTI_RPR1_RPIF7_POS)            /*!< 0x00000080 */
#define EXTI_RPR1_RPIF7              EXTI_RPR1_RPIF7_Msk                       /*!< Rising Pending Interrupt Flag on line 7 */
#define EXTI_RPR1_RPIF8_POS          (8U)
#define EXTI_RPR1_RPIF8_Msk          (0x1UL << EXTI_RPR1_RPIF8_POS)            /*!< 0x00000100 */
#define EXTI_RPR1_RPIF8              EXTI_RPR1_RPIF8_Msk                       /*!< Rising Pending Interrupt Flag on line 8 */
#define EXTI_RPR1_RPIF9_POS          (9U)
#define EXTI_RPR1_RPIF9_Msk          (0x1UL << EXTI_RPR1_RPIF9_POS)            /*!< 0x00000200 */
#define EXTI_RPR1_RPIF9              EXTI_RPR1_RPIF9_Msk                       /*!< Rising Pending Interrupt Flag on line 9 */
#define EXTI_RPR1_RPIF10_POS         (10U)
#define EXTI_RPR1_RPIF10_Msk         (0x1UL << EXTI_RPR1_RPIF10_POS)           /*!< 0x00000400 */
#define EXTI_RPR1_RPIF10             EXTI_RPR1_RPIF10_Msk                      /*!< Rising Pending Interrupt Flag on line 10 */
#define EXTI_RPR1_RPIF11_POS         (11U)
#define EXTI_RPR1_RPIF11_Msk         (0x1UL << EXTI_RPR1_RPIF11_POS)           /*!< 0x00000800 */
#define EXTI_RPR1_RPIF11             EXTI_RPR1_RPIF11_Msk                      /*!< Rising Pending Interrupt Flag on line 11 */
#define EXTI_RPR1_RPIF12_POS         (12U)
#define EXTI_RPR1_RPIF12_Msk         (0x1UL << EXTI_RPR1_RPIF12_POS)           /*!< 0x00001000 */
#define EXTI_RPR1_RPIF12             EXTI_RPR1_RPIF12_Msk                      /*!< Rising Pending Interrupt Flag on line 12 */
#define EXTI_RPR1_RPIF13_POS         (13U)
#define EXTI_RPR1_RPIF13_Msk         (0x1UL << EXTI_RPR1_RPIF13_POS)           /*!< 0x00002000 */
#define EXTI_RPR1_RPIF13             EXTI_RPR1_RPIF13_Msk                      /*!< Rising Pending Interrupt Flag on line 13 */
#define EXTI_RPR1_RPIF14_POS         (14U)
#define EXTI_RPR1_RPIF14_Msk         (0x1UL << EXTI_RPR1_RPIF14_POS)           /*!< 0x00004000 */
#define EXTI_RPR1_RPIF14             EXTI_RPR1_RPIF14_Msk                      /*!< Rising Pending Interrupt Flag on line 14 */
#define EXTI_RPR1_RPIF15_POS         (15U)
#define EXTI_RPR1_RPIF15_Msk         (0x1UL << EXTI_RPR1_RPIF15_POS)           /*!< 0x00008000 */
#define EXTI_RPR1_RPIF15             EXTI_RPR1_RPIF15_Msk                      /*!< Rising Pending Interrupt Flag on line 15 */
#define EXTI_RPR1_RPIF16_POS         (16U)
#define EXTI_RPR1_RPIF16_Msk         (0x1UL << EXTI_RPR1_RPIF16_POS)           /*!< 0x00010000 */
#define EXTI_RPR1_RPIF16             EXTI_RPR1_RPIF16_Msk                      /*!< Rising Pending Interrupt Flag on line 16 */
#define EXTI_RPR1_RPIF17_POS         (17U)
#define EXTI_RPR1_RPIF17_Msk         (0x1UL << EXTI_RPR1_RPIF17_POS)           /*!< 0x00020000 */
#define EXTI_RPR1_RPIF17             EXTI_RPR1_RPIF17_Msk                      /*!< Rising Pending Interrupt Flag on line 17 */
#define EXTI_RPR1_RPIF18_POS         (18U)
#define EXTI_RPR1_RPIF18_Msk         (0x1UL << EXTI_RPR1_RPIF18_POS)           /*!< 0x00040000 */
#define EXTI_RPR1_RPIF18             EXTI_RPR1_RPIF18_Msk                      /*!< Rising Pending Interrupt Flag on line 18 */
#define EXTI_RPR1_RPIF20_POS         (20U)
#define EXTI_RPR1_RPIF20_Msk         (0x1UL << EXTI_RPR1_RPIF20_POS)           /*!< 0x00100000 */
#define EXTI_RPR1_RPIF20             EXTI_RPR1_RPIF20_Msk                      /*!< Rising Pending Interrupt Flag on line 20 */

/*******************  Bit definition for EXTI_FPR1 register  ******************/
#define EXTI_FPR1_FPIF0_POS          (0U)
#define EXTI_FPR1_FPIF0_Msk          (0x1UL << EXTI_FPR1_FPIF0_POS)            /*!< 0x00000001 */
#define EXTI_FPR1_FPIF0              EXTI_FPR1_FPIF0_Msk                       /*!< Falling Pending Interrupt Flag on line 0 */
#define EXTI_FPR1_FPIF1_POS          (1U)
#define EXTI_FPR1_FPIF1_Msk          (0x1UL << EXTI_FPR1_FPIF1_POS)            /*!< 0x00000002 */
#define EXTI_FPR1_FPIF1              EXTI_FPR1_FPIF1_Msk                       /*!< Falling Pending Interrupt Flag on line 1 */
#define EXTI_FPR1_FPIF2_POS          (2U)
#define EXTI_FPR1_FPIF2_Msk          (0x1UL << EXTI_FPR1_FPIF2_POS)            /*!< 0x00000004 */
#define EXTI_FPR1_FPIF2              EXTI_FPR1_FPIF2_Msk                       /*!< Falling Pending Interrupt Flag on line 2 */
#define EXTI_FPR1_FPIF3_POS          (3U)
#define EXTI_FPR1_FPIF3_Msk          (0x1UL << EXTI_FPR1_FPIF3_POS)            /*!< 0x00000008 */
#define EXTI_FPR1_FPIF3              EXTI_FPR1_FPIF3_Msk                       /*!< Falling Pending Interrupt Flag on line 3 */
#define EXTI_FPR1_FPIF4_POS          (4U)
#define EXTI_FPR1_FPIF4_Msk          (0x1UL << EXTI_FPR1_FPIF4_POS)            /*!< 0x00000010 */
#define EXTI_FPR1_FPIF4              EXTI_FPR1_FPIF4_Msk                       /*!< Falling Pending Interrupt Flag on line 4 */
#define EXTI_FPR1_FPIF5_POS          (5U)
#define EXTI_FPR1_FPIF5_Msk          (0x1UL << EXTI_FPR1_FPIF5_POS)            /*!< 0x00000020 */
#define EXTI_FPR1_FPIF5              EXTI_FPR1_FPIF5_Msk                       /*!< Falling Pending Interrupt Flag on line 5 */
#define EXTI_FPR1_FPIF6_POS          (6U)
#define EXTI_FPR1_FPIF6_Msk          (0x1UL << EXTI_FPR1_FPIF6_POS)            /*!< 0x00000040 */
#define EXTI_FPR1_FPIF6              EXTI_FPR1_FPIF6_Msk                       /*!< Falling Pending Interrupt Flag on line 6 */
#define EXTI_FPR1_FPIF7_POS          (7U)
#define EXTI_FPR1_FPIF7_Msk          (0x1UL << EXTI_FPR1_FPIF7_POS)            /*!< 0x00000080 */
#define EXTI_FPR1_FPIF7              EXTI_FPR1_FPIF7_Msk                       /*!< Falling Pending Interrupt Flag on line 7 */
#define EXTI_FPR1_FPIF8_POS          (8U)
#define EXTI_FPR1_FPIF8_Msk          (0x1UL << EXTI_FPR1_FPIF8_POS)            /*!< 0x00000100 */
#define EXTI_FPR1_FPIF8              EXTI_FPR1_FPIF8_Msk                       /*!< Falling Pending Interrupt Flag on line 8 */
#define EXTI_FPR1_FPIF9_POS          (9U)
#define EXTI_FPR1_FPIF9_Msk          (0x1UL << EXTI_FPR1_FPIF9_POS)            /*!< 0x00000200 */
#define EXTI_FPR1_FPIF9              EXTI_FPR1_FPIF9_Msk                       /*!< Falling Pending Interrupt Flag on line 9 */
#define EXTI_FPR1_FPIF10_POS         (10U)
#define EXTI_FPR1_FPIF10_Msk         (0x1UL << EXTI_FPR1_FPIF10_POS)           /*!< 0x00000400 */
#define EXTI_FPR1_FPIF10             EXTI_FPR1_FPIF10_Msk                      /*!< Falling Pending Interrupt Flag on line 10 */
#define EXTI_FPR1_FPIF11_POS         (11U)
#define EXTI_FPR1_FPIF11_Msk         (0x1UL << EXTI_FPR1_FPIF11_POS)           /*!< 0x00000800 */
#define EXTI_FPR1_FPIF11             EXTI_FPR1_FPIF11_Msk                      /*!< Falling Pending Interrupt Flag on line 11 */
#define EXTI_FPR1_FPIF12_POS         (12U)
#define EXTI_FPR1_FPIF12_Msk         (0x1UL << EXTI_FPR1_FPIF12_POS)           /*!< 0x00001000 */
#define EXTI_FPR1_FPIF12             EXTI_FPR1_FPIF12_Msk                      /*!< Falling Pending Interrupt Flag on line 12 */
#define EXTI_FPR1_FPIF13_POS         (13U)
#define EXTI_FPR1_FPIF13_Msk         (0x1UL << EXTI_FPR1_FPIF13_POS)           /*!< 0x00002000 */
#define EXTI_FPR1_FPIF13             EXTI_FPR1_FPIF13_Msk                      /*!< Falling Pending Interrupt Flag on line 13 */
#define EXTI_FPR1_FPIF14_POS         (14U)
#define EXTI_FPR1_FPIF14_Msk         (0x1UL << EXTI_FPR1_FPIF14_POS)           /*!< 0x00004000 */
#define EXTI_FPR1_FPIF14             EXTI_FPR1_FPIF14_Msk                      /*!< Falling Pending Interrupt Flag on line 14 */
#define EXTI_FPR1_FPIF15_POS         (15U)
#define EXTI_FPR1_FPIF15_Msk         (0x1UL << EXTI_FPR1_FPIF15_POS)           /*!< 0x00008000 */
#define EXTI_FPR1_FPIF15             EXTI_FPR1_FPIF15_Msk                      /*!< Falling Pending Interrupt Flag on line 15 */
#define EXTI_FPR1_FPIF16_POS         (16U)
#define EXTI_FPR1_FPIF16_Msk         (0x1UL << EXTI_FPR1_FPIF16_POS)           /*!< 0x00010000 */
#define EXTI_FPR1_FPIF16             EXTI_FPR1_FPIF16_Msk                      /*!< Falling Pending Interrupt Flag on line 16 */
#define EXTI_FPR1_FPIF17_POS         (17U)
#define EXTI_FPR1_FPIF17_Msk         (0x1UL << EXTI_FPR1_FPIF17_POS)           /*!< 0x00020000 */
#define EXTI_FPR1_FPIF17             EXTI_FPR1_FPIF17_Msk                      /*!< Falling Pending Interrupt Flag on line 17 */
#define EXTI_FPR1_FPIF18_POS         (18U)
#define EXTI_FPR1_FPIF18_Msk         (0x1UL << EXTI_FPR1_FPIF18_POS)           /*!< 0x00040000 */
#define EXTI_FPR1_FPIF18             EXTI_FPR1_FPIF18_Msk                      /*!< Falling Pending Interrupt Flag on line 18 */
#define EXTI_FPR1_FPIF20_POS         (20U)
#define EXTI_FPR1_FPIF20_Msk         (0x1UL << EXTI_FPR1_FPIF20_POS)           /*!< 0x00100000 */
#define EXTI_FPR1_FPIF20             EXTI_FPR1_FPIF20_Msk                      /*!< Falling Pending Interrupt Flag on line 20 */

/******************  Bit definition for EXTI_RTSR2 register  ******************/
#define EXTI_RTSR2_RT34_POS           (2U)
#define EXTI_RTSR2_RT34_Msk           (0x1UL << EXTI_RTSR2_RT34_POS)             /*!< 0x00000004 */
#define EXTI_RTSR2_RT34               EXTI_RTSR2_RT34_Msk                        /*!< Rising trigger configuration for input line 34 */

/******************  Bit definition for EXTI_FTSR2 register  ******************/
#define EXTI_FTSR2_FT34_POS           (2U)
#define EXTI_FTSR2_FT34_Msk           (0x1UL << EXTI_FTSR2_FT34_POS)             /*!< 0x00000004 */
#define EXTI_FTSR2_FT34               EXTI_FTSR2_FT34_Msk                        /*!< Falling trigger configuration for input line 34 */

/******************  Bit definition for EXTI_SWIER2 register  *****************/
#define EXTI_SWIER2_SWI34_POS         (2U)
#define EXTI_SWIER2_SWI34_Msk         (0x1UL << EXTI_SWIER2_SWI34_POS)           /*!< 0x00000004 */
#define EXTI_SWIER2_SWI34             EXTI_SWIER2_SWI34_Msk                      /*!< Software Interrupt on line 34 */

/*******************  Bit definition for EXTI_RPR2 register  ******************/
#define EXTI_RPR2_RPIF34_POS         (2U)
#define EXTI_RPR2_RPIF34_Msk         (0x1UL << EXTI_RPR2_RPIF34_POS)            /*!< 0x00000004 */
#define EXTI_RPR2_RPIF34             EXTI_RPR2_RPIF34_Msk                       /*!< Rising Pending Interrupt Flag on line 34 */

/*******************  Bit definition for EXTI_FPR2 register  ******************/
#define EXTI_FPR2_RPIF34_POS         (2U)
#define EXTI_FPR2_RPIF34_Msk         (0x1UL << EXTI_FPR2_RPIF34_POS)            /*!< 0x00000004 */
#define EXTI_FPR2_RPIF34             EXTI_FPR2_RPIF34_Msk                       /*!< Rising Pending Interrupt Flag on line 34 */

/*****************  Bit definition for EXTI_EXTICR1 register  **************/
#define EXTI_EXTICR1_EXTI0_POS       (0U)
#define EXTI_EXTICR1_EXTI0_Msk       (0x7UL << EXTI_EXTICR1_EXTI0_POS)         /*!< 0x00000007 */
#define EXTI_EXTICR1_EXTI0           EXTI_EXTICR1_EXTI0_Msk                    /*!< EXTI 0 configuration */
#define EXTI_EXTICR1_EXTI0_0         (0x1UL << EXTI_EXTICR1_EXTI0_POS)         /*!< 0x00000001 */
#define EXTI_EXTICR1_EXTI0_1         (0x2UL << EXTI_EXTICR1_EXTI0_POS)         /*!< 0x00000002 */
#define EXTI_EXTICR1_EXTI0_2         (0x4UL << EXTI_EXTICR1_EXTI0_POS)         /*!< 0x00000004 */
#define EXTI_EXTICR1_EXTI1_POS       (8U)
#define EXTI_EXTICR1_EXTI1_Msk       (0x7UL << EXTI_EXTICR1_EXTI1_POS)         /*!< 0x00000700 */
#define EXTI_EXTICR1_EXTI1           EXTI_EXTICR1_EXTI1_Msk                    /*!< EXTI 1 configuration */
#define EXTI_EXTICR1_EXTI1_0         (0x1UL << EXTI_EXTICR1_EXTI1_POS)         /*!< 0x00000100 */
#define EXTI_EXTICR1_EXTI1_1         (0x2UL << EXTI_EXTICR1_EXTI1_POS)         /*!< 0x00000200 */
#define EXTI_EXTICR1_EXTI1_2         (0x4UL << EXTI_EXTICR1_EXTI1_POS)         /*!< 0x00000400 */
#define EXTI_EXTICR1_EXTI2_POS       (16U)
#define EXTI_EXTICR1_EXTI2_Msk       (0x7UL << EXTI_EXTICR1_EXTI2_POS)         /*!< 0x00070000 */
#define EXTI_EXTICR1_EXTI2           EXTI_EXTICR1_EXTI2_Msk                    /*!< EXTI 2 configuration */
#define EXTI_EXTICR1_EXTI2_0         (0x1UL << EXTI_EXTICR1_EXTI2_POS)         /*!< 0x00010000 */
#define EXTI_EXTICR1_EXTI2_1         (0x2UL << EXTI_EXTICR1_EXTI2_POS)         /*!< 0x00020000 */
#define EXTI_EXTICR1_EXTI2_2         (0x4UL << EXTI_EXTICR1_EXTI2_POS)         /*!< 0x00040000 */
#define EXTI_EXTICR1_EXTI3_POS       (24U)
#define EXTI_EXTICR1_EXTI3_Msk       (0x7UL << EXTI_EXTICR1_EXTI3_POS)         /*!< 0x07000000 */
#define EXTI_EXTICR1_EXTI3           EXTI_EXTICR1_EXTI3_Msk                    /*!< EXTI 3 configuration */
#define EXTI_EXTICR1_EXTI3_0         (0x1UL << EXTI_EXTICR1_EXTI3_POS)         /*!< 0x01000000 */
#define EXTI_EXTICR1_EXTI3_1         (0x2UL << EXTI_EXTICR1_EXTI3_POS)         /*!< 0x02000000 */
#define EXTI_EXTICR1_EXTI3_2         (0x4UL << EXTI_EXTICR1_EXTI3_POS)         /*!< 0x04000000 */

/*****************  Bit definition for EXTI_EXTICR2 register  **************/
#define EXTI_EXTICR2_EXTI4_POS       (0U)
#define EXTI_EXTICR2_EXTI4_Msk       (0x7UL << EXTI_EXTICR2_EXTI4_POS)         /*!< 0x00000007 */
#define EXTI_EXTICR2_EXTI4           EXTI_EXTICR2_EXTI4_Msk                    /*!< EXTI 4 configuration */
#define EXTI_EXTICR2_EXTI4_0         (0x1UL << EXTI_EXTICR2_EXTI4_POS)         /*!< 0x00000001 */
#define EXTI_EXTICR2_EXTI4_1         (0x2UL << EXTI_EXTICR2_EXTI4_POS)         /*!< 0x00000002 */
#define EXTI_EXTICR2_EXTI4_2         (0x4UL << EXTI_EXTICR2_EXTI4_POS)         /*!< 0x00000004 */
#define EXTI_EXTICR2_EXTI5_POS       (8U)
#define EXTI_EXTICR2_EXTI5_Msk       (0x7UL << EXTI_EXTICR2_EXTI5_POS)         /*!< 0x00000700 */
#define EXTI_EXTICR2_EXTI5           EXTI_EXTICR2_EXTI5_Msk                    /*!< EXTI 5 configuration */
#define EXTI_EXTICR2_EXTI5_0         (0x1UL << EXTI_EXTICR2_EXTI5_POS)         /*!< 0x00000100 */
#define EXTI_EXTICR2_EXTI5_1         (0x2UL << EXTI_EXTICR2_EXTI5_POS)         /*!< 0x00000200 */
#define EXTI_EXTICR2_EXTI5_2         (0x4UL << EXTI_EXTICR2_EXTI5_POS)         /*!< 0x00000400 */
#define EXTI_EXTICR2_EXTI6_POS       (16U)
#define EXTI_EXTICR2_EXTI6_Msk       (0x7UL << EXTI_EXTICR2_EXTI6_POS)         /*!< 0x00070000 */
#define EXTI_EXTICR2_EXTI6           EXTI_EXTICR2_EXTI6_Msk                    /*!< EXTI 6 configuration */
#define EXTI_EXTICR2_EXTI6_0         (0x1UL << EXTI_EXTICR2_EXTI6_POS)         /*!< 0x00010000 */
#define EXTI_EXTICR2_EXTI6_1         (0x2UL << EXTI_EXTICR2_EXTI6_POS)         /*!< 0x00020000 */
#define EXTI_EXTICR2_EXTI6_2         (0x4UL << EXTI_EXTICR2_EXTI6_POS)         /*!< 0x00040000 */
#define EXTI_EXTICR2_EXTI7_POS       (24U)
#define EXTI_EXTICR2_EXTI7_Msk       (0x7UL << EXTI_EXTICR2_EXTI7_POS)         /*!< 0x07000000 */
#define EXTI_EXTICR2_EXTI7           EXTI_EXTICR2_EXTI7_Msk                    /*!< EXTI 7 configuration */
#define EXTI_EXTICR2_EXTI7_0         (0x1UL << EXTI_EXTICR2_EXTI7_POS)         /*!< 0x01000000 */
#define EXTI_EXTICR2_EXTI7_1         (0x2UL << EXTI_EXTICR2_EXTI7_POS)         /*!< 0x02000000 */
#define EXTI_EXTICR2_EXTI7_2         (0x4UL << EXTI_EXTICR2_EXTI7_POS)         /*!< 0x04000000 */

/*****************  Bit definition for EXTI_EXTICR3 register  **************/
#define EXTI_EXTICR3_EXTI8_POS       (0U)
#define EXTI_EXTICR3_EXTI8_Msk       (0x7UL << EXTI_EXTICR3_EXTI8_POS)         /*!< 0x00000007 */
#define EXTI_EXTICR3_EXTI8           EXTI_EXTICR3_EXTI8_Msk                    /*!< EXTI 8 configuration */
#define EXTI_EXTICR3_EXTI8_0         (0x1UL << EXTI_EXTICR3_EXTI8_POS)         /*!< 0x00000001 */
#define EXTI_EXTICR3_EXTI8_1         (0x2UL << EXTI_EXTICR3_EXTI8_POS)         /*!< 0x00000002 */
#define EXTI_EXTICR3_EXTI8_2         (0x4UL << EXTI_EXTICR3_EXTI8_POS)         /*!< 0x00000004 */
#define EXTI_EXTICR3_EXTI9_POS       (8U)
#define EXTI_EXTICR3_EXTI9_Msk       (0x7UL << EXTI_EXTICR3_EXTI9_POS)         /*!< 0x00000700 */
#define EXTI_EXTICR3_EXTI9           EXTI_EXTICR3_EXTI9_Msk                    /*!< EXTI 9 configuration */
#define EXTI_EXTICR3_EXTI9_0         (0x1UL << EXTI_EXTICR3_EXTI9_POS)         /*!< 0x00000100 */
#define EXTI_EXTICR3_EXTI9_1         (0x2UL << EXTI_EXTICR3_EXTI9_POS)         /*!< 0x00000200 */
#define EXTI_EXTICR3_EXTI9_2         (0x4UL << EXTI_EXTICR3_EXTI9_POS)         /*!< 0x00000400 */
#define EXTI_EXTICR3_EXTI10_POS      (16U)
#define EXTI_EXTICR3_EXTI10_Msk      (0x7UL << EXTI_EXTICR3_EXTI10_POS)        /*!< 0x00070000 */
#define EXTI_EXTICR3_EXTI10          EXTI_EXTICR3_EXTI10_Msk                   /*!< EXTI 10 configuration */
#define EXTI_EXTICR3_EXTI10_0        (0x1UL << EXTI_EXTICR3_EXTI10_POS)        /*!< 0x00010000 */
#define EXTI_EXTICR3_EXTI10_1        (0x2UL << EXTI_EXTICR3_EXTI10_POS)        /*!< 0x00020000 */
#define EXTI_EXTICR3_EXTI10_2        (0x4UL << EXTI_EXTICR3_EXTI10_POS)        /*!< 0x00040000 */
#define EXTI_EXTICR3_EXTI11_POS      (24U)
#define EXTI_EXTICR3_EXTI11_Msk      (0x7UL << EXTI_EXTICR3_EXTI11_POS)        /*!< 0x07000000 */
#define EXTI_EXTICR3_EXTI11          EXTI_EXTICR3_EXTI11_Msk                   /*!< EXTI 11 configuration */
#define EXTI_EXTICR3_EXTI11_0        (0x1UL << EXTI_EXTICR3_EXTI11_POS)        /*!< 0x01000000 */
#define EXTI_EXTICR3_EXTI11_1        (0x2UL << EXTI_EXTICR3_EXTI11_POS)        /*!< 0x02000000 */
#define EXTI_EXTICR3_EXTI11_2        (0x4UL << EXTI_EXTICR3_EXTI11_POS)        /*!< 0x04000000 */

/*****************  Bit definition for EXTI_EXTICR4 register  **************/
#define EXTI_EXTICR4_EXTI12_POS      (0U)
#define EXTI_EXTICR4_EXTI12_Msk      (0x7UL << EXTI_EXTICR4_EXTI12_POS)        /*!< 0x00000007 */
#define EXTI_EXTICR4_EXTI12          EXTI_EXTICR4_EXTI12_Msk                   /*!< EXTI 12 configuration */
#define EXTI_EXTICR4_EXTI12_0        (0x1UL << EXTI_EXTICR4_EXTI12_POS)        /*!< 0x00000001 */
#define EXTI_EXTICR4_EXTI12_1        (0x2UL << EXTI_EXTICR4_EXTI12_POS)        /*!< 0x00000002 */
#define EXTI_EXTICR4_EXTI12_2        (0x4UL << EXTI_EXTICR4_EXTI12_POS)        /*!< 0x00000004 */
#define EXTI_EXTICR4_EXTI13_POS      (8U)
#define EXTI_EXTICR4_EXTI13_Msk      (0x7UL << EXTI_EXTICR4_EXTI13_POS)        /*!< 0x00000700 */
#define EXTI_EXTICR4_EXTI13          EXTI_EXTICR4_EXTI13_Msk                   /*!< EXTI 13 configuration */
#define EXTI_EXTICR4_EXTI13_0        (0x1UL << EXTI_EXTICR4_EXTI13_POS)        /*!< 0x00000100 */
#define EXTI_EXTICR4_EXTI13_1        (0x2UL << EXTI_EXTICR4_EXTI13_POS)       /*!< 0x00000200 */
#define EXTI_EXTICR4_EXTI13_2        (0x4UL << EXTI_EXTICR4_EXTI13_POS)         /*!< 0x00000400 */
#define EXTI_EXTICR4_EXTI14_POS      (16U)
#define EXTI_EXTICR4_EXTI14_Msk      (0x7UL << EXTI_EXTICR4_EXTI14_POS)        /*!< 0x00070000 */
#define EXTI_EXTICR4_EXTI14          EXTI_EXTICR4_EXTI14_Msk                   /*!< EXTI 14 configuration */
#define EXTI_EXTICR4_EXTI14_0        (0x1UL << EXTI_EXTICR4_EXTI14_POS)        /*!< 0x00010000 */
#define EXTI_EXTICR4_EXTI14_1        (0x2UL << EXTI_EXTICR4_EXTI14_POS)        /*!< 0x00020000 */
#define EXTI_EXTICR4_EXTI14_2        (0x4UL << EXTI_EXTICR4_EXTI14_POS)        /*!< 0x00040000 */
#define EXTI_EXTICR4_EXTI15_POS      (24U)
#define EXTI_EXTICR4_EXTI15_Msk      (0x7UL << EXTI_EXTICR4_EXTI15_POS)        /*!< 0x07000000 */
#define EXTI_EXTICR4_EXTI15          EXTI_EXTICR4_EXTI15_Msk                   /*!< EXTI 15 configuration */
#define EXTI_EXTICR4_EXTI15_0        (0x1UL << EXTI_EXTICR4_EXTI15_POS)        /*!< 0x01000000 */
#define EXTI_EXTICR4_EXTI15_1        (0x2UL << EXTI_EXTICR4_EXTI15_POS)        /*!< 0x02000000 */
#define EXTI_EXTICR4_EXTI15_2        (0x4UL << EXTI_EXTICR4_EXTI15_POS)        /*!< 0x04000000 */

/*******************  Bit definition for EXTI_IMR1 register  ******************/
#define EXTI_IMR1_IM0_POS            (0U)
#define EXTI_IMR1_IM0_Msk            (0x1UL << EXTI_IMR1_IM0_POS)              /*!< 0x00000001 */
#define EXTI_IMR1_IM0                EXTI_IMR1_IM0_Msk                         /*!< Interrupt Mask on line 0 */
#define EXTI_IMR1_IM1_POS            (1U)
#define EXTI_IMR1_IM1_Msk            (0x1UL << EXTI_IMR1_IM1_POS)              /*!< 0x00000002 */
#define EXTI_IMR1_IM1                EXTI_IMR1_IM1_Msk                         /*!< Interrupt Mask on line 1 */
#define EXTI_IMR1_IM2_POS            (2U)
#define EXTI_IMR1_IM2_Msk            (0x1UL << EXTI_IMR1_IM2_POS)              /*!< 0x00000004 */
#define EXTI_IMR1_IM2                EXTI_IMR1_IM2_Msk                         /*!< Interrupt Mask on line 2 */
#define EXTI_IMR1_IM3_POS            (3U)
#define EXTI_IMR1_IM3_Msk            (0x1UL << EXTI_IMR1_IM3_POS)              /*!< 0x00000008 */
#define EXTI_IMR1_IM3                EXTI_IMR1_IM3_Msk                         /*!< Interrupt Mask on line 3 */
#define EXTI_IMR1_IM4_POS            (4U)
#define EXTI_IMR1_IM4_Msk            (0x1UL << EXTI_IMR1_IM4_POS)              /*!< 0x00000010 */
#define EXTI_IMR1_IM4                EXTI_IMR1_IM4_Msk                         /*!< Interrupt Mask on line 4 */
#define EXTI_IMR1_IM5_POS            (5U)
#define EXTI_IMR1_IM5_Msk            (0x1UL << EXTI_IMR1_IM5_POS)              /*!< 0x00000020 */
#define EXTI_IMR1_IM5                EXTI_IMR1_IM5_Msk                         /*!< Interrupt Mask on line 5 */
#define EXTI_IMR1_IM6_POS            (6U)
#define EXTI_IMR1_IM6_Msk            (0x1UL << EXTI_IMR1_IM6_POS)              /*!< 0x00000040 */
#define EXTI_IMR1_IM6                EXTI_IMR1_IM6_Msk                         /*!< Interrupt Mask on line 6 */
#define EXTI_IMR1_IM7_POS            (7U)
#define EXTI_IMR1_IM7_Msk            (0x1UL << EXTI_IMR1_IM7_POS)              /*!< 0x00000080 */
#define EXTI_IMR1_IM7                EXTI_IMR1_IM7_Msk                         /*!< Interrupt Mask on line 7 */
#define EXTI_IMR1_IM8_POS            (8U)
#define EXTI_IMR1_IM8_Msk            (0x1UL << EXTI_IMR1_IM8_POS)              /*!< 0x00000100 */
#define EXTI_IMR1_IM8                EXTI_IMR1_IM8_Msk                         /*!< Interrupt Mask on line 8 */
#define EXTI_IMR1_IM9_POS            (9U)
#define EXTI_IMR1_IM9_Msk            (0x1UL << EXTI_IMR1_IM9_POS)              /*!< 0x00000200 */
#define EXTI_IMR1_IM9                EXTI_IMR1_IM9_Msk                         /*!< Interrupt Mask on line 9 */
#define EXTI_IMR1_IM10_POS           (10U)
#define EXTI_IMR1_IM10_Msk           (0x1UL << EXTI_IMR1_IM10_POS)             /*!< 0x00000400 */
#define EXTI_IMR1_IM10               EXTI_IMR1_IM10_Msk                        /*!< Interrupt Mask on line 10 */
#define EXTI_IMR1_IM11_POS           (11U)
#define EXTI_IMR1_IM11_Msk           (0x1UL << EXTI_IMR1_IM11_POS)             /*!< 0x00000800 */
#define EXTI_IMR1_IM11               EXTI_IMR1_IM11_Msk                        /*!< Interrupt Mask on line 11 */
#define EXTI_IMR1_IM12_POS           (12U)
#define EXTI_IMR1_IM12_Msk           (0x1UL << EXTI_IMR1_IM12_POS)             /*!< 0x00001000 */
#define EXTI_IMR1_IM12               EXTI_IMR1_IM12_Msk                        /*!< Interrupt Mask on line 12 */
#define EXTI_IMR1_IM13_POS           (13U)
#define EXTI_IMR1_IM13_Msk           (0x1UL << EXTI_IMR1_IM13_POS)             /*!< 0x00002000 */
#define EXTI_IMR1_IM13               EXTI_IMR1_IM13_Msk                        /*!< Interrupt Mask on line 13 */
#define EXTI_IMR1_IM14_POS           (14U)
#define EXTI_IMR1_IM14_Msk           (0x1UL << EXTI_IMR1_IM14_POS)             /*!< 0x00004000 */
#define EXTI_IMR1_IM14               EXTI_IMR1_IM14_Msk                        /*!< Interrupt Mask on line 14 */
#define EXTI_IMR1_IM15_POS           (15U)
#define EXTI_IMR1_IM15_Msk           (0x1UL << EXTI_IMR1_IM15_POS)             /*!< 0x00008000 */
#define EXTI_IMR1_IM15               EXTI_IMR1_IM15_Msk                        /*!< Interrupt Mask on line 15 */
#define EXTI_IMR1_IM16_POS           (16U)
#define EXTI_IMR1_IM16_Msk           (0x1UL << EXTI_IMR1_IM16_POS)             /*!< 0x00010000 */
#define EXTI_IMR1_IM16               EXTI_IMR1_IM16_Msk                        /*!< Interrupt Mask on line 16 */
#define EXTI_IMR1_IM17_POS           (17U)
#define EXTI_IMR1_IM17_Msk           (0x1UL << EXTI_IMR1_IM17_POS)             /*!< 0x00020000 */
#define EXTI_IMR1_IM17               EXTI_IMR1_IM17_Msk                        /*!< Interrupt Mask on line 17 */
#define EXTI_IMR1_IM18_POS           (18U)
#define EXTI_IMR1_IM18_Msk           (0x1UL << EXTI_IMR1_IM18_POS)             /*!< 0x00040000 */
#define EXTI_IMR1_IM18               EXTI_IMR1_IM18_Msk                        /*!< Interrupt Mask on line 18 */
#define EXTI_IMR1_IM19_POS           (19U)
#define EXTI_IMR1_IM19_Msk           (0x1UL << EXTI_IMR1_IM19_POS)             /*!< 0x00080000 */
#define EXTI_IMR1_IM19               EXTI_IMR1_IM19_Msk                        /*!< Interrupt Mask on line 19 */
#define EXTI_IMR1_IM20_POS           (20U)
#define EXTI_IMR1_IM20_Msk           (0x1UL << EXTI_IMR1_IM20_POS)             /*!< 0x00100000 */
#define EXTI_IMR1_IM20               EXTI_IMR1_IM20_Msk                        /*!< Interrupt Mask on line 20 */
#define EXTI_IMR1_IM21_POS           (21U)
#define EXTI_IMR1_IM21_Msk           (0x1UL << EXTI_IMR1_IM21_POS)             /*!< 0x00200000 */
#define EXTI_IMR1_IM21               EXTI_IMR1_IM21_Msk                        /*!< Interrupt Mask on line 21 */
#define EXTI_IMR1_IM22_POS           (22U)
#define EXTI_IMR1_IM22_Msk           (0x1UL << EXTI_IMR1_IM22_POS)             /*!< 0x00400000 */
#define EXTI_IMR1_IM22               EXTI_IMR1_IM22_Msk                        /*!< Interrupt Mask on line 22 */
#define EXTI_IMR1_IM23_POS           (23U)
#define EXTI_IMR1_IM23_Msk           (0x1UL << EXTI_IMR1_IM23_POS)             /*!< 0x00800000 */
#define EXTI_IMR1_IM23               EXTI_IMR1_IM23_Msk                        /*!< Interrupt Mask on line 23 */
#define EXTI_IMR1_IM24_POS           (24U)
#define EXTI_IMR1_IM24_Msk           (0x1UL << EXTI_IMR1_IM24_POS)             /*!< 0x01000000 */
#define EXTI_IMR1_IM24               EXTI_IMR1_IM24_Msk                        /*!< Interrupt Mask on line 24 */
#define EXTI_IMR1_IM25_POS           (25U)
#define EXTI_IMR1_IM25_Msk           (0x1UL << EXTI_IMR1_IM25_POS)             /*!< 0x02000000 */
#define EXTI_IMR1_IM25               EXTI_IMR1_IM25_Msk                        /*!< Interrupt Mask on line 25 */
#define EXTI_IMR1_IM26_POS           (26U)
#define EXTI_IMR1_IM26_Msk           (0x1UL << EXTI_IMR1_IM26_POS)             /*!< 0x04000000 */
#define EXTI_IMR1_IM26               EXTI_IMR1_IM26_Msk                        /*!< Interrupt Mask on line 26 */
#define EXTI_IMR1_IM27_POS           (27U)
#define EXTI_IMR1_IM27_Msk           (0x1UL << EXTI_IMR1_IM27_POS)             /*!< 0x08000000 */
#define EXTI_IMR1_IM27               EXTI_IMR1_IM27_Msk                        /*!< Interrupt Mask on line 27 */
#define EXTI_IMR1_IM28_POS           (28U)
#define EXTI_IMR1_IM28_Msk           (0x1UL << EXTI_IMR1_IM28_POS)             /*!< 0x10000000 */
#define EXTI_IMR1_IM28               EXTI_IMR1_IM28_Msk                        /*!< Interrupt Mask on line 28 */
#define EXTI_IMR1_IM29_POS           (29U)
#define EXTI_IMR1_IM29_Msk           (0x1UL << EXTI_IMR1_IM29_POS)             /*!< 0x20000000 */
#define EXTI_IMR1_IM29               EXTI_IMR1_IM29_Msk                        /*!< Interrupt Mask on line 29 */
#define EXTI_IMR1_IM30_POS           (30U)
#define EXTI_IMR1_IM30_Msk           (0x1UL << EXTI_IMR1_IM30_POS)             /*!< 0x40000000 */
#define EXTI_IMR1_IM30               EXTI_IMR1_IM30_Msk                        /*!< Interrupt Mask on line 30 */
#define EXTI_IMR1_IM31_POS           (31U)
#define EXTI_IMR1_IM31_Msk           (0x1UL << EXTI_IMR1_IM31_POS)              /*!< 0x80000000 */
#define EXTI_IMR1_IM31               EXTI_IMR1_IM31_Msk                        /*!< Interrupt Mask on line 31 */
#define EXTI_IMR1_IM_POS             (0U)
#define EXTI_IMR1_IM_Msk             (0xFFFFFFFFUL << EXTI_IMR1_IM_POS)        /*!< 0xFFFFFFFF */
#define EXTI_IMR1_IM                 EXTI_IMR1_IM_Msk                          /*!< Interrupt Mask All */

/*******************  Bit definition for EXTI_IMR2 register  ******************/
#define EXTI_IMR2_IM32_POS           (0U)
#define EXTI_IMR2_IM32_Msk           (0x1UL << EXTI_IMR2_IM32_POS)             /*!< 0x00000001 */
#define EXTI_IMR2_IM32               EXTI_IMR2_IM32_Msk                        /*!< Interrupt Mask on line 32 */
#define EXTI_IMR2_IM33_POS           (1U)
#define EXTI_IMR2_IM33_Msk           (0x1UL << EXTI_IMR2_IM33_POS)             /*!< 0x00000002 */
#define EXTI_IMR2_IM33               EXTI_IMR2_IM33_Msk                        /*!< Interrupt Mask on line 33 */
#define EXTI_IMR2_IM34_POS           (2U)
#define EXTI_IMR2_IM34_Msk           (0x1UL << EXTI_IMR2_IM34_POS)             /*!< 0x00000004 */
#define EXTI_IMR2_IM34               EXTI_IMR2_IM34_Msk                        /*!< Interrupt Mask on line 34 */
#define EXTI_IMR2_IM35_POS           (3U)
#define EXTI_IMR2_IM35_Msk           (0x1UL << EXTI_IMR2_IM35_POS)             /*!< 0x00000008 */
#define EXTI_IMR2_IM35               EXTI_IMR2_IM35_Msk                        /*!< Interrupt Mask on line 35 */
#define EXTI_IMR2_IM36_POS           (4U)
#define EXTI_IMR2_IM36_Msk           (0x1UL << EXTI_IMR2_IM36_POS)             /*!< 0x00000010 */
#define EXTI_IMR2_IM36               EXTI_IMR2_IM36_Msk                        /*!< Interrupt Mask on line 36 */
#define EXTI_IMR2_IM_POS             (0U)
#define EXTI_IMR2_IM_Msk             (0x1FUL << EXTI_IMR2_IM_POS)               /*!< 0x0000001F */
#define EXTI_IMR2_IM                 EXTI_IMR2_IM_Msk                          /*!< Interrupt Mask All */

/*******************  Bit definition for EXTI_EMR1 register  ******************/
#define EXTI_EMR1_EM0_POS            (0U)
#define EXTI_EMR1_EM0_Msk            (0x1UL << EXTI_EMR1_EM0_POS)              /*!< 0x00000001 */
#define EXTI_EMR1_EM0                EXTI_EMR1_EM0_Msk                         /*!< Event Mask on line 0 */
#define EXTI_EMR1_EM1_POS            (1U)
#define EXTI_EMR1_EM1_Msk            (0x1UL << EXTI_EMR1_EM1_POS)              /*!< 0x00000002 */
#define EXTI_EMR1_EM1                EXTI_EMR1_EM1_Msk                         /*!< Event Mask on line 1 */
#define EXTI_EMR1_EM2_POS            (2U)
#define EXTI_EMR1_EM2_Msk            (0x1UL << EXTI_EMR1_EM2_POS)              /*!< 0x00000004 */
#define EXTI_EMR1_EM2                EXTI_EMR1_EM2_Msk                         /*!< Event Mask on line 2 */
#define EXTI_EMR1_EM3_POS            (3U)
#define EXTI_EMR1_EM3_Msk            (0x1UL << EXTI_EMR1_EM3_POS)              /*!< 0x00000008 */
#define EXTI_EMR1_EM3                EXTI_EMR1_EM3_Msk                         /*!< Event Mask on line 3 */
#define EXTI_EMR1_EM4_POS            (4U)
#define EXTI_EMR1_EM4_Msk            (0x1UL << EXTI_EMR1_EM4_POS)              /*!< 0x00000010 */
#define EXTI_EMR1_EM4                EXTI_EMR1_EM4_Msk                         /*!< Event Mask on line 4 */
#define EXTI_EMR1_EM5_POS            (5U)
#define EXTI_EMR1_EM5_Msk            (0x1UL << EXTI_EMR1_EM5_POS)              /*!< 0x00000020 */
#define EXTI_EMR1_EM5                EXTI_EMR1_EM5_Msk                         /*!< Event Mask on line 5 */
#define EXTI_EMR1_EM6_POS            (6U)
#define EXTI_EMR1_EM6_Msk            (0x1UL << EXTI_EMR1_EM6_POS)              /*!< 0x00000040 */
#define EXTI_EMR1_EM6                EXTI_EMR1_EM6_Msk                         /*!< Event Mask on line 6 */
#define EXTI_EMR1_EM7_POS            (7U)
#define EXTI_EMR1_EM7_Msk            (0x1UL << EXTI_EMR1_EM7_POS)              /*!< 0x00000080 */
#define EXTI_EMR1_EM7                EXTI_EMR1_EM7_Msk                         /*!< Event Mask on line 7 */
#define EXTI_EMR1_EM8_POS            (8U)
#define EXTI_EMR1_EM8_Msk            (0x1UL << EXTI_EMR1_EM8_POS)              /*!< 0x00000100 */
#define EXTI_EMR1_EM8                EXTI_EMR1_EM8_Msk                         /*!< Event Mask on line 8 */
#define EXTI_EMR1_EM9_POS            (9U)
#define EXTI_EMR1_EM9_Msk            (0x1UL << EXTI_EMR1_EM9_POS)              /*!< 0x00000200 */
#define EXTI_EMR1_EM9                EXTI_EMR1_EM9_Msk                         /*!< Event Mask on line 9 */
#define EXTI_EMR1_EM10_POS           (10U)
#define EXTI_EMR1_EM10_Msk           (0x1UL << EXTI_EMR1_EM10_POS)             /*!< 0x00000400 */
#define EXTI_EMR1_EM10               EXTI_EMR1_EM10_Msk                        /*!< Event Mask on line 10 */
#define EXTI_EMR1_EM11_POS           (11U)
#define EXTI_EMR1_EM11_Msk           (0x1UL << EXTI_EMR1_EM11_POS)             /*!< 0x00000800 */
#define EXTI_EMR1_EM11               EXTI_EMR1_EM11_Msk                        /*!< Event Mask on line 11 */
#define EXTI_EMR1_EM12_POS           (12U)
#define EXTI_EMR1_EM12_Msk           (0x1UL << EXTI_EMR1_EM12_POS)             /*!< 0x00001000 */
#define EXTI_EMR1_EM12               EXTI_EMR1_EM12_Msk                        /*!< Event Mask on line 12 */
#define EXTI_EMR1_EM13_POS           (13U)
#define EXTI_EMR1_EM13_Msk           (0x1UL << EXTI_EMR1_EM13_POS)             /*!< 0x00002000 */
#define EXTI_EMR1_EM13               EXTI_EMR1_EM13_Msk                        /*!< Event Mask on line 13 */
#define EXTI_EMR1_EM14_POS           (14U)
#define EXTI_EMR1_EM14_Msk           (0x1UL << EXTI_EMR1_EM14_POS)             /*!< 0x00004000 */
#define EXTI_EMR1_EM14               EXTI_EMR1_EM14_Msk                        /*!< Event Mask on line 14 */
#define EXTI_EMR1_EM15_POS           (15U)
#define EXTI_EMR1_EM15_Msk           (0x1UL << EXTI_EMR1_EM15_POS)             /*!< 0x00008000 */
#define EXTI_EMR1_EM15               EXTI_EMR1_EM15_Msk                        /*!< Event Mask on line 15 */
#define EXTI_EMR1_EM16_POS           (16U)
#define EXTI_EMR1_EM16_Msk           (0x1UL << EXTI_EMR1_EM16_POS)             /*!< 0x00010000 */
#define EXTI_EMR1_EM16               EXTI_EMR1_EM16_Msk                        /*!< Event Mask on line 16 */
#define EXTI_EMR1_EM17_POS           (17U)
#define EXTI_EMR1_EM17_Msk           (0x1UL << EXTI_EMR1_EM17_POS)             /*!< 0x00020000 */
#define EXTI_EMR1_EM17               EXTI_EMR1_EM17_Msk                        /*!< Event Mask on line 17 */
#define EXTI_EMR1_EM18_POS           (18U)
#define EXTI_EMR1_EM18_Msk           (0x1UL << EXTI_EMR1_EM18_POS)             /*!< 0x00040000 */
#define EXTI_EMR1_EM18               EXTI_EMR1_EM18_Msk                        /*!< Event Mask on line 18 */
#define EXTI_EMR1_EM19_POS           (19U)
#define EXTI_EMR1_EM19_Msk           (0x1UL << EXTI_EMR1_EM19_POS)             /*!< 0x00080000 */
#define EXTI_EMR1_EM19               EXTI_EMR1_EM19_Msk                        /*!< Event Mask on line 19 */
#define EXTI_EMR1_EM20_POS           (20U)
#define EXTI_EMR1_EM20_Msk           (0x1UL << EXTI_EMR1_EM20_POS)             /*!< 0x00100000 */
#define EXTI_EMR1_EM20               EXTI_EMR1_EM20_Msk                        /*!< Event Mask on line 20 */
#define EXTI_EMR1_EM21_POS           (21U)
#define EXTI_EMR1_EM21_Msk           (0x1UL << EXTI_EMR1_EM21_POS)             /*!< 0x00200000 */
#define EXTI_EMR1_EM21               EXTI_EMR1_EM21_Msk                        /*!< Event Mask on line 21 */
#define EXTI_EMR1_EM22_POS           (22U)
#define EXTI_EMR1_EM22_Msk           (0x1UL << EXTI_EMR1_EM22_POS)             /*!< 0x00400000 */
#define EXTI_EMR1_EM22               EXTI_EMR1_EM22_Msk                        /*!< Event Mask on line 22 */
#define EXTI_EMR1_EM23_POS           (23U)
#define EXTI_EMR1_EM23_Msk           (0x1UL << EXTI_EMR1_EM23_POS)             /*!< 0x00800000 */
#define EXTI_EMR1_EM23               EXTI_EMR1_EM23_Msk                        /*!< Event Mask on line 23 */
#define EXTI_EMR1_EM24_POS           (24U)
#define EXTI_EMR1_EM24_Msk           (0x1UL << EXTI_EMR1_EM24_POS)             /*!< 0x01000000 */
#define EXTI_EMR1_EM24               EXTI_EMR1_EM24_Msk                        /*!< Event Mask on line 24 */
#define EXTI_EMR1_EM25_POS           (25U)
#define EXTI_EMR1_EM25_Msk           (0x1UL << EXTI_EMR1_EM25_POS)             /*!< 0x02000000 */
#define EXTI_EMR1_EM25               EXTI_EMR1_EM25_Msk                        /*!< Event Mask on line 25 */
#define EXTI_EMR1_EM26_POS           (26U)
#define EXTI_EMR1_EM26_Msk           (0x1UL << EXTI_EMR1_EM26_POS)             /*!< 0x04000000 */
#define EXTI_EMR1_EM26               EXTI_EMR1_EM26_Msk                        /*!< Event Mask on line 26 */
#define EXTI_EMR1_EM27_POS           (27U)
#define EXTI_EMR1_EM27_Msk           (0x1UL << EXTI_EMR1_EM27_POS)             /*!< 0x08000000 */
#define EXTI_EMR1_EM27               EXTI_EMR1_EM27_Msk                        /*!< Event Mask on line 27 */
#define EXTI_EMR1_EM28_POS           (28U)
#define EXTI_EMR1_EM28_Msk           (0x1UL << EXTI_EMR1_EM28_POS)             /*!< 0x10000000 */
#define EXTI_EMR1_EM28               EXTI_EMR1_EM28_Msk                        /*!< Event Mask on line 28 */
#define EXTI_EMR1_EM29_POS           (29U)
#define EXTI_EMR1_EM29_Msk           (0x1UL << EXTI_EMR1_EM29_POS)             /*!< 0x20000000 */
#define EXTI_EMR1_EM29               EXTI_EMR1_EM29_Msk                        /*!< Event Mask on line 29 */
#define EXTI_EMR1_EM30_POS           (30U)
#define EXTI_EMR1_EM30_Msk           (0x1UL << EXTI_EMR1_EM30_POS)             /*!< 0x40000000 */
#define EXTI_EMR1_EM30               EXTI_EMR1_EM30_Msk                        /*!< Event Mask on line 30 */
#define EXTI_EMR1_EM31_POS           (31U)
#define EXTI_EMR1_EM31_Msk           (0x1UL << EXTI_EMR1_EM31_POS)             /*!< 0x80000000 */
#define EXTI_EMR1_EM31               EXTI_EMR1_EM31_Msk                        /*!< Event Mask on line 31 */

/*******************  Bit definition for EXTI_EMR2 register  ******************/
#define EXTI_EMR2_EM32_POS           (0U)
#define EXTI_EMR2_EM32_Msk           (0x1UL << EXTI_EMR2_EM32_POS)             /*!< 0x00000001 */
#define EXTI_EMR2_EM32               EXTI_EMR2_EM32_Msk                        /*!< Event Mask on line 32 */
#define EXTI_EMR2_EM33_POS           (1U)
#define EXTI_EMR2_EM33_Msk           (0x1UL << EXTI_EMR2_EM33_POS)             /*!< 0x00000002 */
#define EXTI_EMR2_EM33               EXTI_EMR2_EM33_Msk                        /*!< Event Mask on line 33 */
#define EXTI_EMR2_EM34_POS           (2U)
#define EXTI_EMR2_EM34_Msk           (0x1UL << EXTI_EMR2_EM34_POS)             /*!< 0x00000004 */
#define EXTI_EMR2_EM34               EXTI_EMR2_EM34_Msk                        /*!< Event Mask on line 34 */
#define EXTI_EMR2_EM35_POS           (3U)
#define EXTI_EMR2_EM35_Msk           (0x1UL << EXTI_EMR2_EM35_POS)             /*!< 0x00000008 */
#define EXTI_EMR2_EM35               EXTI_EMR2_EM35_Msk                        /*!< Event Mask on line 35 */
#define EXTI_EMR2_EM36_POS           (4U)
#define EXTI_EMR2_EM36_Msk           (0x1UL << EXTI_EMR2_EM36_POS)             /*!< 0x00000010 */
#define EXTI_EMR2_EM36               EXTI_EMR2_EM36_Msk                        /*!< Event Mask on line 36 */





//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- SCC CONTROL REGISTERS --------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief System configuration controller
  */
typedef struct
{
  volatile uint32 CFGR1;          /*!< SYSCFG configuration register 1,                   Address offset: 0x00 */
       uint32 RESERVED0[5];   /*!< Reserved,                                                   0x04 --0x14 */
  volatile uint32 CFGR2;          /*!< SYSCFG configuration register 2,                   Address offset: 0x18 */
       uint32 RESERVED1[25];  /*!< Reserved                                                           0x1C */
  volatile uint32 IT_LINE_SR[32]; /*!< SYSCFG configuration IT_LINE register,             Address offset: 0x80 */
} SYSCFG_TYPEDEF;

#define SYSCFG              (*(SYSCFG_TYPEDEF *) SYSCFG_BASE)

/*****************  Bit definition for SYSCFG_CFGR1 register  ****************/
#define SYSCFG_CFGR1_MEM_MODE_POS             (0U)
#define SYSCFG_CFGR1_MEM_MODE_MSK             (0x3UL << SYSCFG_CFGR1_MEM_MODE_POS) /*!< 0x00000003 */
#define SYSCFG_CFGR1_MEM_MODE                 SYSCFG_CFGR1_MEM_MODE_MSK            /*!< SYSCFG_Memory Remap Config */
#define SYSCFG_CFGR1_MEM_MODE_0               (0x1UL << SYSCFG_CFGR1_MEM_MODE_POS) /*!< 0x00000001 */
#define SYSCFG_CFGR1_MEM_MODE_1               (0x2UL << SYSCFG_CFGR1_MEM_MODE_POS) /*!< 0x00000002 */
#define SYSCFG_CFGR1_PA11_RMP_POS             (3U)
#define SYSCFG_CFGR1_PA11_RMP_MSK             (0x1UL << SYSCFG_CFGR1_PA11_RMP_POS) /*!< 0x00000008 */
#define SYSCFG_CFGR1_PA11_RMP                 SYSCFG_CFGR1_PA11_RMP_MSK            /*!< PA11 Remap */
#define SYSCFG_CFGR1_PA12_RMP_POS             (4U)
#define SYSCFG_CFGR1_PA12_RMP_MSK             (0x1UL << SYSCFG_CFGR1_PA12_RMP_POS) /*!< 0x00000010 */
#define SYSCFG_CFGR1_PA12_RMP                 SYSCFG_CFGR1_PA12_RMP_MSK            /*!< PA12 Remap */
#define SYSCFG_CFGR1_IR_POL_POS               (5U)
#define SYSCFG_CFGR1_IR_POL_MSK               (0x1UL << SYSCFG_CFGR1_IR_POL_POS) /*!< 0x00000020 */
#define SYSCFG_CFGR1_IR_POL                   SYSCFG_CFGR1_IR_POL_MSK            /*!< IROut Polarity Selection */
#define SYSCFG_CFGR1_IR_MOD_POS               (6U)
#define SYSCFG_CFGR1_IR_MOD_MSK               (0x3UL << SYSCFG_CFGR1_IR_MOD_POS) /*!< 0x000000C0 */
#define SYSCFG_CFGR1_IR_MOD                   SYSCFG_CFGR1_IR_MOD_MSK            /*!< IRDA Modulation Envelope signal source selection */
#define SYSCFG_CFGR1_IR_MOD_0                 (0x1UL << SYSCFG_CFGR1_IR_MOD_POS) /*!< 0x00000040 */
#define SYSCFG_CFGR1_IR_MOD_1                 (0x2UL << SYSCFG_CFGR1_IR_MOD_POS) /*!< 0x00000080 */
#define SYSCFG_CFGR1_BOOSTEN_POS              (8U)
#define SYSCFG_CFGR1_BOOSTEN_MSK              (0x1UL << SYSCFG_CFGR1_BOOSTEN_POS) /*!< 0x00000100 */
#define SYSCFG_CFGR1_BOOSTEN                  SYSCFG_CFGR1_BOOSTEN_MSK            /*!< I/O analog switch voltage booster enable */
#define SYSCFG_CFGR1_UCPD1_STROBE_POS         (9U)
#define SYSCFG_CFGR1_UCPD1_STROBE_MSK         (0x1UL << SYSCFG_CFGR1_UCPD1_STROBE_POS) /*!< 0x00000200 */
#define SYSCFG_CFGR1_UCPD1_STROBE             SYSCFG_CFGR1_UCPD1_STROBE_MSK            /*!< Strobe signal bit for UCPD1 */
#define SYSCFG_CFGR1_UCPD2_STROBE_POS         (10U)
#define SYSCFG_CFGR1_UCPD2_STROBE_MSK         (0x1UL << SYSCFG_CFGR1_UCPD2_STROBE_POS) /*!< 0x00000400 */
#define SYSCFG_CFGR1_UCPD2_STROBE             SYSCFG_CFGR1_UCPD2_STROBE_MSK            /*!< Strobe signal bit for UCPD2 */
#define SYSCFG_CFGR1_I2C_PB6_FMP_POS          (16U)
#define SYSCFG_CFGR1_I2C_PB6_FMP_MSK          (0x1UL << SYSCFG_CFGR1_I2C_PB6_FMP_POS)  /*!< 0x00010000 */
#define SYSCFG_CFGR1_I2C_PB6_FMP              SYSCFG_CFGR1_I2C_PB6_FMP_MSK             /*!< I2C PB6 Fast mode plus */
#define SYSCFG_CFGR1_I2C_PB7_FMP_POS          (17U)
#define SYSCFG_CFGR1_I2C_PB7_FMP_MSK          (0x1UL << SYSCFG_CFGR1_I2C_PB7_FMP_POS)  /*!< 0x00020000 */
#define SYSCFG_CFGR1_I2C_PB7_FMP              SYSCFG_CFGR1_I2C_PB7_FMP_MSK             /*!< I2C PB7 Fast mode plus */
#define SYSCFG_CFGR1_I2C_PB8_FMP_POS          (18U)
#define SYSCFG_CFGR1_I2C_PB8_FMP_MSK          (0x1UL << SYSCFG_CFGR1_I2C_PB8_FMP_POS)  /*!< 0x00040000 */
#define SYSCFG_CFGR1_I2C_PB8_FMP              SYSCFG_CFGR1_I2C_PB8_FMP_MSK             /*!< I2C PB8 Fast mode plus */
#define SYSCFG_CFGR1_I2C_PB9_FMP_POS          (19U)
#define SYSCFG_CFGR1_I2C_PB9_FMP_MSK          (0x1UL << SYSCFG_CFGR1_I2C_PB9_FMP_POS)  /*!< 0x00080000 */
#define SYSCFG_CFGR1_I2C_PB9_FMP              SYSCFG_CFGR1_I2C_PB9_FMP_MSK             /*!< I2C PB9 Fast mode plus */
#define SYSCFG_CFGR1_I2C1_FMP_POS             (20U)
#define SYSCFG_CFGR1_I2C1_FMP_MSK             (0x1UL << SYSCFG_CFGR1_I2C1_FMP_POS)     /*!< 0x00100000 */
#define SYSCFG_CFGR1_I2C1_FMP                 SYSCFG_CFGR1_I2C1_FMP_MSK                /*!< Enable Fast Mode Plus on PB10, PB11, PF6 and PF7  */
#define SYSCFG_CFGR1_I2C2_FMP_POS             (21U)
#define SYSCFG_CFGR1_I2C2_FMP_MSK             (0x1UL << SYSCFG_CFGR1_I2C2_FMP_POS)     /*!< 0x00200000 */
#define SYSCFG_CFGR1_I2C2_FMP                 SYSCFG_CFGR1_I2C2_FMP_MSK                /*!< Enable I2C2 Fast mode plus  */
#define SYSCFG_CFGR1_I2C_PA9_FMP_POS          (22U)
#define SYSCFG_CFGR1_I2C_PA9_FMP_MSK          (0x1UL << SYSCFG_CFGR1_I2C_PA9_FMP_POS)  /*!< 0x00400000 */
#define SYSCFG_CFGR1_I2C_PA9_FMP              SYSCFG_CFGR1_I2C_PA9_FMP_MSK             /*!< Enable Fast Mode Plus on PA9  */
#define SYSCFG_CFGR1_I2C_PA10_FMP_POS         (23U)
#define SYSCFG_CFGR1_I2C_PA10_FMP_MSK         (0x1UL << SYSCFG_CFGR1_I2C_PA10_FMP_POS) /*!< 0x00800000 */
#define SYSCFG_CFGR1_I2C_PA10_FMP             SYSCFG_CFGR1_I2C_PA10_FMP_MSK            /*!< Enable Fast Mode Plus on PA10 */

/******************  Bit definition for SYSCFG_CFGR2 register  ****************/
#define SYSCFG_CFGR2_CLL_POS                  (0U)
#define SYSCFG_CFGR2_CLL_MSK                  (0x1UL << SYSCFG_CFGR2_CLL_POS)   /*!< 0x00000001 */
#define SYSCFG_CFGR2_CLL                      SYSCFG_CFGR2_CLL_MSK              /*!< Enables and locks the LOCKUP (Hardfault) output of CortexM0 with Break Input of TIMER1 */
#define SYSCFG_CFGR2_SPL_POS                  (1U)
#define SYSCFG_CFGR2_SPL_MSK                  (0x1UL << SYSCFG_CFGR2_SPL_POS)   /*!< 0x00000002 */
#define SYSCFG_CFGR2_SPL                      SYSCFG_CFGR2_SPL_MSK              /*!< Enables and locks the SRAM_PARITY error signal with Break Input of TIMER1 */
#define SYSCFG_CFGR2_PVDL_POS                 (2U)
#define SYSCFG_CFGR2_PVDL_MSK                 (0x1UL << SYSCFG_CFGR2_PVDL_POS)  /*!< 0x00000004 */
#define SYSCFG_CFGR2_PVDL                     SYSCFG_CFGR2_PVDL_MSK             /*!< Enables and locks the PVD connection with Timer1 Break Input and also the PVD_EN and PVDSEL[2:0] bits of the Power Control Interface */
#define SYSCFG_CFGR2_ECCL_POS                 (3U)
#define SYSCFG_CFGR2_ECCL_MSK                 (0x1UL << SYSCFG_CFGR2_ECCL_POS)  /*!< 0x00000008 */
#define SYSCFG_CFGR2_ECCL                     SYSCFG_CFGR2_ECCL_MSK             /*!< ECCL */
#define SYSCFG_CFGR2_SPF_POS                  (8U)
#define SYSCFG_CFGR2_SPF_MSK                  (0x1UL << SYSCFG_CFGR2_SPF_POS)   /*!< 0x00000100 */
#define SYSCFG_CFGR2_SPF                      SYSCFG_CFGR2_SPF_MSK              /*!< SRAM Parity error flag */
#define SYSCFG_CFGR2_SRAM_PE                  SYSCFG_CFGR2_SPF                  /*!< SRAM Parity error flag (define maintained for legacy purpose) */

/*****************  Bit definition for SYSCFG_ITLINEx ISR Wrapper register  ****************/
#define SYSCFG_ITLINE0_SR_EWDG_POS            (0U)
#define SYSCFG_ITLINE0_SR_EWDG_MSK            (0x1UL << SYSCFG_ITLINE0_SR_EWDG_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE0_SR_EWDG                SYSCFG_ITLINE0_SR_EWDG_MSK       /*!< EWDG interrupt */
#define SYSCFG_ITLINE1_SR_PVDOUT_POS          (0U)
#define SYSCFG_ITLINE1_SR_PVDOUT_MSK          (0x1UL << SYSCFG_ITLINE1_SR_PVDOUT_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE1_SR_PVDOUT              SYSCFG_ITLINE1_SR_PVDOUT_MSK     /*!< Power voltage detection -> exti[16] Interrupt */
#define SYSCFG_ITLINE2_SR_TAMPER_POS          (0U)
#define SYSCFG_ITLINE2_SR_TAMPER_MSK          (0x1UL << SYSCFG_ITLINE2_SR_TAMPER_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE2_SR_TAMPER              SYSCFG_ITLINE2_SR_TAMPER_MSK     /*!< TAMPER -> exti[21] interrupt */
#define SYSCFG_ITLINE2_SR_RTC_WAKEUP_POS      (1U)
#define SYSCFG_ITLINE2_SR_RTC_WAKEUP_MSK      (0x1UL << SYSCFG_ITLINE2_SR_RTC_WAKEUP_POS) /*!< 0x00000002 */
#define SYSCFG_ITLINE2_SR_RTC_WAKEUP          SYSCFG_ITLINE2_SR_RTC_WAKEUP_MSK /*!< RTC_WAKEUP -> exti[19] interrupt .... */
#define SYSCFG_ITLINE3_SR_FLASH_ECC_POS       (0U)
#define SYSCFG_ITLINE3_SR_FLASH_ECC_MSK       (0x1UL << SYSCFG_ITLINE3_SR_FLASH_ECC_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE3_SR_FLASH_ECC           SYSCFG_ITLINE3_SR_FLASH_ECC_MSK  /*!< Flash ITF ECC interrupt */
#define SYSCFG_ITLINE3_SR_FLASH_ITF_POS       (1U)
#define SYSCFG_ITLINE3_SR_FLASH_ITF_MSK       (0x1UL << SYSCFG_ITLINE3_SR_FLASH_ITF_POS) /*!< 0x00000002 */
#define SYSCFG_ITLINE3_SR_FLASH_ITF           SYSCFG_ITLINE3_SR_FLASH_ITF_MSK  /*!< FLASH ITF interrupt */
#define SYSCFG_ITLINE4_SR_CLK_CTRL_POS        (0U)
#define SYSCFG_ITLINE4_SR_CLK_CTRL_MSK        (0x1UL << SYSCFG_ITLINE4_SR_CLK_CTRL_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE4_SR_CLK_CTRL            SYSCFG_ITLINE4_SR_CLK_CTRL_MSK   /*!< RCC interrupt */
#define SYSCFG_ITLINE5_SR_EXTI0_POS           (0U)
#define SYSCFG_ITLINE5_SR_EXTI0_MSK           (0x1UL << SYSCFG_ITLINE5_SR_EXTI0_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE5_SR_EXTI0               SYSCFG_ITLINE5_SR_EXTI0_MSK      /*!< External Interrupt 0 */
#define SYSCFG_ITLINE5_SR_EXTI1_POS           (1U)
#define SYSCFG_ITLINE5_SR_EXTI1_MSK           (0x1UL << SYSCFG_ITLINE5_SR_EXTI1_POS) /*!< 0x00000002 */
#define SYSCFG_ITLINE5_SR_EXTI1               SYSCFG_ITLINE5_SR_EXTI1_MSK      /*!< External Interrupt 1 */
#define SYSCFG_ITLINE6_SR_EXTI2_POS           (0U)
#define SYSCFG_ITLINE6_SR_EXTI2_MSK           (0x1UL << SYSCFG_ITLINE6_SR_EXTI2_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE6_SR_EXTI2               SYSCFG_ITLINE6_SR_EXTI2_MSK      /*!< External Interrupt 2 */
#define SYSCFG_ITLINE6_SR_EXTI3_POS           (1U)
#define SYSCFG_ITLINE6_SR_EXTI3_MSK           (0x1UL << SYSCFG_ITLINE6_SR_EXTI3_POS) /*!< 0x00000002 */
#define SYSCFG_ITLINE6_SR_EXTI3               SYSCFG_ITLINE6_SR_EXTI3_MSK      /*!< External Interrupt 3 */
#define SYSCFG_ITLINE7_SR_EXTI4_POS           (0U)
#define SYSCFG_ITLINE7_SR_EXTI4_MSK           (0x1UL << SYSCFG_ITLINE7_SR_EXTI4_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE7_SR_EXTI4               SYSCFG_ITLINE7_SR_EXTI4_MSK      /*!< External Interrupt 4 */
#define SYSCFG_ITLINE7_SR_EXTI5_POS           (1U)
#define SYSCFG_ITLINE7_SR_EXTI5_MSK           (0x1UL << SYSCFG_ITLINE7_SR_EXTI5_POS) /*!< 0x00000002 */
#define SYSCFG_ITLINE7_SR_EXTI5               SYSCFG_ITLINE7_SR_EXTI5_MSK      /*!< External Interrupt 5 */
#define SYSCFG_ITLINE7_SR_EXTI6_POS           (2U)
#define SYSCFG_ITLINE7_SR_EXTI6_MSK           (0x1UL << SYSCFG_ITLINE7_SR_EXTI6_POS) /*!< 0x00000004 */
#define SYSCFG_ITLINE7_SR_EXTI6               SYSCFG_ITLINE7_SR_EXTI6_MSK      /*!< External Interrupt 6 */
#define SYSCFG_ITLINE7_SR_EXTI7_POS           (3U)
#define SYSCFG_ITLINE7_SR_EXTI7_MSK           (0x1UL << SYSCFG_ITLINE7_SR_EXTI7_POS) /*!< 0x00000008 */
#define SYSCFG_ITLINE7_SR_EXTI7               SYSCFG_ITLINE7_SR_EXTI7_MSK      /*!< External Interrupt 7 */
#define SYSCFG_ITLINE7_SR_EXTI8_POS           (4U)
#define SYSCFG_ITLINE7_SR_EXTI8_MSK           (0x1UL << SYSCFG_ITLINE7_SR_EXTI8_POS) /*!< 0x00000010 */
#define SYSCFG_ITLINE7_SR_EXTI8               SYSCFG_ITLINE7_SR_EXTI8_MSK      /*!< External Interrupt 8 */
#define SYSCFG_ITLINE7_SR_EXTI9_POS           (5U)
#define SYSCFG_ITLINE7_SR_EXTI9_MSK           (0x1UL << SYSCFG_ITLINE7_SR_EXTI9_POS) /*!< 0x00000020 */
#define SYSCFG_ITLINE7_SR_EXTI9               SYSCFG_ITLINE7_SR_EXTI9_MSK      /*!< External Interrupt 9 */
#define SYSCFG_ITLINE7_SR_EXTI10_POS          (6U)
#define SYSCFG_ITLINE7_SR_EXTI10_MSK          (0x1UL << SYSCFG_ITLINE7_SR_EXTI10_POS) /*!< 0x00000040 */
#define SYSCFG_ITLINE7_SR_EXTI10              SYSCFG_ITLINE7_SR_EXTI10_MSK     /*!< External Interrupt 10 */
#define SYSCFG_ITLINE7_SR_EXTI11_POS          (7U)
#define SYSCFG_ITLINE7_SR_EXTI11_MSK          (0x1UL << SYSCFG_ITLINE7_SR_EXTI11_POS) /*!< 0x00000080 */
#define SYSCFG_ITLINE7_SR_EXTI11              SYSCFG_ITLINE7_SR_EXTI11_MSK     /*!< External Interrupt 11 */
#define SYSCFG_ITLINE7_SR_EXTI12_POS          (8U)
#define SYSCFG_ITLINE7_SR_EXTI12_MSK          (0x1UL << SYSCFG_ITLINE7_SR_EXTI12_POS) /*!< 0x00000100 */
#define SYSCFG_ITLINE7_SR_EXTI12              SYSCFG_ITLINE7_SR_EXTI12_MSK     /*!< External Interrupt 12 */
#define SYSCFG_ITLINE7_SR_EXTI13_POS          (9U)
#define SYSCFG_ITLINE7_SR_EXTI13_MSK          (0x1UL << SYSCFG_ITLINE7_SR_EXTI13_POS) /*!< 0x00000200 */
#define SYSCFG_ITLINE7_SR_EXTI13              SYSCFG_ITLINE7_SR_EXTI13_MSK     /*!< External Interrupt 13 */
#define SYSCFG_ITLINE7_SR_EXTI14_POS          (10U)
#define SYSCFG_ITLINE7_SR_EXTI14_MSK          (0x1UL << SYSCFG_ITLINE7_SR_EXTI14_POS) /*!< 0x00000400 */
#define SYSCFG_ITLINE7_SR_EXTI14              SYSCFG_ITLINE7_SR_EXTI14_MSK     /*!< External Interrupt 14 */
#define SYSCFG_ITLINE7_SR_EXTI15_POS          (11U)
#define SYSCFG_ITLINE7_SR_EXTI15_MSK          (0x1UL << SYSCFG_ITLINE7_SR_EXTI15_POS) /*!< 0x00000800 */
#define SYSCFG_ITLINE7_SR_EXTI15              SYSCFG_ITLINE7_SR_EXTI15_MSK     /*!< External Interrupt 15 */
#define SYSCFG_ITLINE8_SR_UCPD1_POS          (0U)
#define SYSCFG_ITLINE8_SR_UCPD1_MSK          (0x1UL << SYSCFG_ITLINE8_SR_UCPD1_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE8_SR_UCPD1              SYSCFG_ITLINE8_SR_UCPD1_MSK       /*!< UCPD1 -> exti[32] Interrupt */
#define SYSCFG_ITLINE8_SR_UCPD2_POS          (1U)
#define SYSCFG_ITLINE8_SR_UCPD2_MSK          (0x1UL << SYSCFG_ITLINE8_SR_UCPD2_POS) /*!< 0x00000002 */
#define SYSCFG_ITLINE8_SR_UCPD2              SYSCFG_ITLINE8_SR_UCPD2_MSK       /*!< UCPD2 -> exti[33] Interrupt */
#define SYSCFG_ITLINE9_SR_DMA1_CH1_POS        (0U)
#define SYSCFG_ITLINE9_SR_DMA1_CH1_MSK        (0x1UL << SYSCFG_ITLINE9_SR_DMA1_CH1_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE9_SR_DMA1_CH1            SYSCFG_ITLINE9_SR_DMA1_CH1_MSK   /*!< DMA1 CHANNEL 1 Interrupt */
#define SYSCFG_ITLINE10_SR_DMA1_CH2_POS       (0U)
#define SYSCFG_ITLINE10_SR_DMA1_CH2_MSK       (0x1UL << SYSCFG_ITLINE10_SR_DMA1_CH2_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE10_SR_DMA1_CH2           SYSCFG_ITLINE10_SR_DMA1_CH2_MSK  /*!< DMA1 CHANNEL 2 Interrupt */
#define SYSCFG_ITLINE10_SR_DMA1_CH3_POS       (1U)
#define SYSCFG_ITLINE10_SR_DMA1_CH3_MSK       (0x1UL << SYSCFG_ITLINE10_SR_DMA1_CH3_POS) /*!< 0x00000002 */
#define SYSCFG_ITLINE10_SR_DMA1_CH3           SYSCFG_ITLINE10_SR_DMA1_CH3_MSK  /*!< DMA2 CHANNEL 3 Interrupt */
#define SYSCFG_ITLINE11_SR_DMAMUX1_POS         (0U)
#define SYSCFG_ITLINE11_SR_DMAMUX1_MSK         (0x1UL << SYSCFG_ITLINE11_SR_DMAMUX1_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE11_SR_DMAMUX1             SYSCFG_ITLINE11_SR_DMAMUX1_MSK    /*!< DMAMUX Interrupt */
#define SYSCFG_ITLINE11_SR_DMA1_CH4_POS       (1U)
#define SYSCFG_ITLINE11_SR_DMA1_CH4_MSK       (0x1UL << SYSCFG_ITLINE11_SR_DMA1_CH4_POS) /*!< 0x00000002 */
#define SYSCFG_ITLINE11_SR_DMA1_CH4           SYSCFG_ITLINE11_SR_DMA1_CH4_MSK  /*!< DMA1 CHANNEL 4 Interrupt */
#define SYSCFG_ITLINE11_SR_DMA1_CH5_POS       (2U)
#define SYSCFG_ITLINE11_SR_DMA1_CH5_MSK       (0x1UL << SYSCFG_ITLINE11_SR_DMA1_CH5_POS) /*!< 0x00000004 */
#define SYSCFG_ITLINE11_SR_DMA1_CH5           SYSCFG_ITLINE11_SR_DMA1_CH5_MSK  /*!< DMA1 CHANNEL 5 Interrupt */
#define SYSCFG_ITLINE11_SR_DMA1_CH6_POS       (3U)
#define SYSCFG_ITLINE11_SR_DMA1_CH6_MSK       (0x1UL << SYSCFG_ITLINE11_SR_DMA1_CH6_POS) /*!< 0x00000008 */
#define SYSCFG_ITLINE11_SR_DMA1_CH6           SYSCFG_ITLINE11_SR_DMA1_CH6_MSK  /*!< DMA1 CHANNEL 6 Interrupt */
#define SYSCFG_ITLINE11_SR_DMA1_CH7_POS       (4U)
#define SYSCFG_ITLINE11_SR_DMA1_CH7_MSK       (0x1UL << SYSCFG_ITLINE11_SR_DMA1_CH7_POS) /*!< 0x00000010 */
#define SYSCFG_ITLINE11_SR_DMA1_CH7           SYSCFG_ITLINE11_SR_DMA1_CH7_MSK  /*!< DMA1 CHANNEL 7 Interrupt */
#define SYSCFG_ITLINE12_SR_ADC_POS            (0U)
#define SYSCFG_ITLINE12_SR_ADC_MSK            (0x1UL << SYSCFG_ITLINE12_SR_ADC_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE12_SR_ADC                SYSCFG_ITLINE12_SR_ADC_MSK       /*!< ADC Interrupt */
#define SYSCFG_ITLINE12_SR_COMP1_POS          (1U)
#define SYSCFG_ITLINE12_SR_COMP1_MSK          (0x1UL << SYSCFG_ITLINE12_SR_COMP1_POS) /*!< 0x00000002 */
#define SYSCFG_ITLINE12_SR_COMP1              SYSCFG_ITLINE12_SR_COMP1_MSK     /*!< COMP1 Interrupt -> exti[17] */
#define SYSCFG_ITLINE12_SR_COMP2_POS          (2U)
#define SYSCFG_ITLINE12_SR_COMP2_MSK          (0x1UL << SYSCFG_ITLINE12_SR_COMP2_POS) /*!< 0x00000004 */
#define SYSCFG_ITLINE12_SR_COMP2              SYSCFG_ITLINE12_SR_COMP2_MSK     /*!< COMP2 Interrupt -> exti[18] */
#define SYSCFG_ITLINE13_SR_TIM1_CCU_POS       (0U)
#define SYSCFG_ITLINE13_SR_TIM1_CCU_MSK       (0x1UL << SYSCFG_ITLINE13_SR_TIM1_CCU_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE13_SR_TIM1_CCU           SYSCFG_ITLINE13_SR_TIM1_CCU_MSK  /*!< TIM1 CCU Interrupt */
#define SYSCFG_ITLINE13_SR_TIM1_TRG_POS       (1U)
#define SYSCFG_ITLINE13_SR_TIM1_TRG_MSK       (0x1UL << SYSCFG_ITLINE13_SR_TIM1_TRG_POS) /*!< 0x00000002 */
#define SYSCFG_ITLINE13_SR_TIM1_TRG           SYSCFG_ITLINE13_SR_TIM1_TRG_MSK  /*!< TIM1 TRG Interrupt */
#define SYSCFG_ITLINE13_SR_TIM1_UPD_POS       (2U)
#define SYSCFG_ITLINE13_SR_TIM1_UPD_MSK       (0x1UL << SYSCFG_ITLINE13_SR_TIM1_UPD_POS) /*!< 0x00000004 */
#define SYSCFG_ITLINE13_SR_TIM1_UPD           SYSCFG_ITLINE13_SR_TIM1_UPD_MSK  /*!< TIM1 UPD Interrupt */
#define SYSCFG_ITLINE13_SR_TIM1_BRK_POS       (3U)
#define SYSCFG_ITLINE13_SR_TIM1_BRK_MSK       (0x1UL << SYSCFG_ITLINE13_SR_TIM1_BRK_POS) /*!< 0x00000008 */
#define SYSCFG_ITLINE13_SR_TIM1_BRK           SYSCFG_ITLINE13_SR_TIM1_BRK_MSK  /*!< TIM1 BRK Interrupt */
#define SYSCFG_ITLINE14_SR_TIM1_CC_POS        (0U)
#define SYSCFG_ITLINE14_SR_TIM1_CC_MSK        (0x1UL << SYSCFG_ITLINE14_SR_TIM1_CC_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE14_SR_TIM1_CC            SYSCFG_ITLINE14_SR_TIM1_CC_MSK   /*!< TIM1 CC Interrupt */
#define SYSCFG_ITLINE15_SR_TIM2_GLB_POS       (0U)
#define SYSCFG_ITLINE15_SR_TIM2_GLB_MSK       (0x1UL << SYSCFG_ITLINE15_SR_TIM2_GLB_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE15_SR_TIM2_GLB           SYSCFG_ITLINE15_SR_TIM2_GLB_MSK  /*!< TIM2 GLB Interrupt */
#define SYSCFG_ITLINE16_SR_TIM3_GLB_POS       (0U)
#define SYSCFG_ITLINE16_SR_TIM3_GLB_MSK       (0x1UL << SYSCFG_ITLINE16_SR_TIM3_GLB_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE16_SR_TIM3_GLB           SYSCFG_ITLINE16_SR_TIM3_GLB_MSK  /*!< TIM3 GLB Interrupt */
#define SYSCFG_ITLINE17_SR_TIM6_GLB_POS       (0U)
#define SYSCFG_ITLINE17_SR_TIM6_GLB_MSK       (0x1UL << SYSCFG_ITLINE17_SR_TIM6_GLB_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE17_SR_TIM6_GLB           SYSCFG_ITLINE17_SR_TIM6_GLB_MSK  /*!< TIM6 GLB Interrupt */
#define SYSCFG_ITLINE17_SR_DAC_POS            (1U)
#define SYSCFG_ITLINE17_SR_DAC_MSK            (0x1UL << SYSCFG_ITLINE17_SR_DAC_POS) /*!< 0x00000002 */
#define SYSCFG_ITLINE17_SR_DAC                SYSCFG_ITLINE17_SR_DAC_MSK       /*!< DAC Interrupt */
#define SYSCFG_ITLINE17_SR_LPTIM1_GLB_POS     (2U)
#define SYSCFG_ITLINE17_SR_LPTIM1_GLB_MSK     (0x1UL << SYSCFG_ITLINE17_SR_LPTIM1_GLB_POS) /*!< 0x00000004 */
#define SYSCFG_ITLINE17_SR_LPTIM1_GLB         SYSCFG_ITLINE17_SR_LPTIM1_GLB_MSK /*!< LPTIM1 -> exti[29] Interrupt */
#define SYSCFG_ITLINE18_SR_TIM7_GLB_POS       (0U)
#define SYSCFG_ITLINE18_SR_TIM7_GLB_MSK       (0x1UL << SYSCFG_ITLINE18_SR_TIM7_GLB_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE18_SR_TIM7_GLB           SYSCFG_ITLINE18_SR_TIM7_GLB_MSK  /*!< TIM7 GLB Interrupt */
#define SYSCFG_ITLINE18_SR_LPTIM2_GLB_POS     (1U)
#define SYSCFG_ITLINE18_SR_LPTIM2_GLB_MSK     (0x1UL << SYSCFG_ITLINE18_SR_LPTIM2_GLB_POS) /*!< 0x00000002 */
#define SYSCFG_ITLINE18_SR_LPTIM2_GLB         SYSCFG_ITLINE18_SR_LPTIM2_GLB_MSK /*!< LPTIM2 -> exti[30] Interrupt */
#define SYSCFG_ITLINE19_SR_TIM14_GLB_POS      (0U)
#define SYSCFG_ITLINE19_SR_TIM14_GLB_MSK      (0x1UL << SYSCFG_ITLINE19_SR_TIM14_GLB_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE19_SR_TIM14_GLB          SYSCFG_ITLINE19_SR_TIM14_GLB_MSK /*!< TIM14 GLB Interrupt */
#define SYSCFG_ITLINE20_SR_TIM15_GLB_POS      (0U)
#define SYSCFG_ITLINE20_SR_TIM15_GLB_MSK      (0x1UL << SYSCFG_ITLINE20_SR_TIM15_GLB_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE20_SR_TIM15_GLB          SYSCFG_ITLINE20_SR_TIM15_GLB_MSK /*!< TIM15 GLB Interrupt */
#define SYSCFG_ITLINE21_SR_TIM16_GLB_POS      (0U)
#define SYSCFG_ITLINE21_SR_TIM16_GLB_MSK      (0x1UL << SYSCFG_ITLINE21_SR_TIM16_GLB_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE21_SR_TIM16_GLB          SYSCFG_ITLINE21_SR_TIM16_GLB_MSK /*!< TIM16 GLB Interrupt */
#define SYSCFG_ITLINE22_SR_TIM17_GLB_POS      (0U)
#define SYSCFG_ITLINE22_SR_TIM17_GLB_MSK      (0x1UL << SYSCFG_ITLINE22_SR_TIM17_GLB_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE22_SR_TIM17_GLB          SYSCFG_ITLINE22_SR_TIM17_GLB_MSK /*!< TIM17 GLB Interrupt */
#define SYSCFG_ITLINE23_SR_I2C1_GLB_POS       (0U)
#define SYSCFG_ITLINE23_SR_I2C1_GLB_MSK       (0x1UL << SYSCFG_ITLINE23_SR_I2C1_GLB_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE23_SR_I2C1_GLB           SYSCFG_ITLINE23_SR_I2C1_GLB_MSK  /*!< I2C1 GLB Interrupt -> exti[23] */
#define SYSCFG_ITLINE24_SR_I2C2_GLB_POS       (0U)
#define SYSCFG_ITLINE24_SR_I2C2_GLB_MSK       (0x1UL << SYSCFG_ITLINE24_SR_I2C2_GLB_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE24_SR_I2C2_GLB           SYSCFG_ITLINE24_SR_I2C2_GLB_MSK  /*!< I2C2 GLB Interrupt */
#define SYSCFG_ITLINE25_SR_SPI1_POS           (0U)
#define SYSCFG_ITLINE25_SR_SPI1_MSK           (0x1UL << SYSCFG_ITLINE25_SR_SPI1_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE25_SR_SPI1               SYSCFG_ITLINE25_SR_SPI1_MSK      /*!< SPI1 Interrupt */
#define SYSCFG_ITLINE26_SR_SPI2_POS           (0U)
#define SYSCFG_ITLINE26_SR_SPI2_MSK           (0x1UL << SYSCFG_ITLINE26_SR_SPI2_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE26_SR_SPI2               SYSCFG_ITLINE26_SR_SPI2_MSK      /*!< SPI2  Interrupt */
#define SYSCFG_ITLINE27_SR_USART1_GLB_POS     (0U)
#define SYSCFG_ITLINE27_SR_USART1_GLB_MSK     (0x1UL << SYSCFG_ITLINE27_SR_USART1_GLB_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE27_SR_USART1_GLB         SYSCFG_ITLINE27_SR_USART1_GLB_MSK /*!< USART1 GLB Interrupt -> exti[25] */
#define SYSCFG_ITLINE28_SR_USART2_GLB_POS     (0U)
#define SYSCFG_ITLINE28_SR_USART2_GLB_MSK     (0x1UL << SYSCFG_ITLINE28_SR_USART2_GLB_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE28_SR_USART2_GLB         SYSCFG_ITLINE28_SR_USART2_GLB_MSK /*!< USART2 GLB Interrupt -> exti[26] */
#define SYSCFG_ITLINE29_SR_USART3_GLB_POS     (0U)
#define SYSCFG_ITLINE29_SR_USART3_GLB_MSK     (0x1UL << SYSCFG_ITLINE29_SR_USART3_GLB_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE29_SR_USART3_GLB         SYSCFG_ITLINE29_SR_USART3_GLB_MSK /*!< USART3 GLB Interrupt */
#define SYSCFG_ITLINE29_SR_USART4_GLB_POS     (1U)
#define SYSCFG_ITLINE29_SR_USART4_GLB_MSK     (0x1UL << SYSCFG_ITLINE29_SR_USART4_GLB_POS) /*!< 0x00000002 */
#define SYSCFG_ITLINE29_SR_USART4_GLB         SYSCFG_ITLINE29_SR_USART4_GLB_MSK /*!< USART4 GLB Interrupt */
#define SYSCFG_ITLINE29_SR_LPUART1_GLB_POS    (2U)
#define SYSCFG_ITLINE29_SR_LPUART1_GLB_MSK    (0x1UL << SYSCFG_ITLINE29_SR_LPUART1_GLB_POS) /*!< 0x00000004 */
#define SYSCFG_ITLINE29_SR_LPUART1_GLB        SYSCFG_ITLINE29_SR_LPUART1_GLB_MSK /*!< LPUART1 GLB Interrupt -> exti[28] */
#define SYSCFG_ITLINE30_SR_CEC_POS            (0U)
#define SYSCFG_ITLINE30_SR_CEC_MSK            (0x1UL << SYSCFG_ITLINE30_SR_CEC_POS) /*!< 0x00000001 */
#define SYSCFG_ITLINE30_SR_CEC                SYSCFG_ITLINE30_SR_CEC_MSK       /*!< CEC Interrupt-> exti[27] */





//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- TIMER REGISTERS -------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief TIM
  */
typedef struct
{
  volatile uint32 CR1;         /*!< TIM control register 1,                   Address offset: 0x00 */
  volatile uint32 CR2;         /*!< TIM control register 2,                   Address offset: 0x04 */
  volatile uint32 SMCR;        /*!< TIM slave mode control register,          Address offset: 0x08 */
  volatile uint16 DIER;        /*!< TIM DMA/interrupt enable register,        Address offset: 0x0C */
  uint16 RESERVED_0;
  volatile uint16 SR;          /*!< TIM status register,                      Address offset: 0x10 */
  uint16 RESERVED_1;
  volatile uint32 EGR;         /*!< TIM event generation register,            Address offset: 0x14 */
  volatile uint16 CCMR1;       /*!< TIM capture/compare mode register 1,      Address offset: 0x18 */
  uint16 RESERVED_2;
  volatile uint16 CCMR2;       /*!< TIM capture/compare mode register 2,      Address offset: 0x1C */
  uint16 RESERVED_3;
  volatile uint32 CCER;        /*!< TIM capture/compare enable register,      Address offset: 0x20 */
  volatile uint32 CNT;         /*!< TIM counter register,                     Address offset: 0x24 */
  volatile uint32 PSC;         /*!< TIM prescaler register,                   Address offset: 0x28 */
  volatile uint32 ARR;         /*!< TIM auto-reload register,                 Address offset: 0x2C */
  volatile uint32 RCR;         /*!< TIM repetition counter register,          Address offset: 0x30 */
  volatile uint32 CCR[4];      /*!< TIM capture/compare register 1, 2, 3, 4   Address offset: 0x34 */
  volatile uint32 BDTR;        /*!< TIM break and dead-time register,         Address offset: 0x44 */
  volatile uint32 DCR;         /*!< TIM DMA control register,                 Address offset: 0x48 */
  volatile uint32 DMAR;        /*!< TIM DMA address for full transfer,        Address offset: 0x4C */
  volatile uint32 OR1;         /*!< TIM option register,                      Address offset: 0x50 */
  volatile uint32 CCMR3;       /*!< TIM capture/compare mode register 3,      Address offset: 0x54 */
  volatile uint32 CCR5;        /*!< TIM capture/compare register5,            Address offset: 0x58 */
  volatile uint32 CCR6;        /*!< TIM capture/compare register6,            Address offset: 0x5C */
  volatile uint32 AF1;         /*!< TIM alternate function register 1,        Address offset: 0x60 */
  volatile uint32 AF2;         /*!< TIM alternate function register 2,        Address offset: 0x64 */
  volatile uint32 TISEL;       /*!< TIM Input Selection register,             Address offset: 0x68 */
} TIM_DEF_TYPE;

#define TIM_1                (*(TIM_DEF_TYPE *) TIM1_BASE)
#define TIM_2                (*(TIM_DEF_TYPE *) TIM2_BASE)
#define TIM_3                (*(TIM_DEF_TYPE *) TIM3_BASE)
#define TIM_4                (*(TIM_DEF_TYPE *) TIM4_BASE)
#define TIM_6                (*(TIM_DEF_TYPE *) TIM6_BASE)
#define TIM_7                (*(TIM_DEF_TYPE *) TIM7_BASE)
#define TIM_14               (*(TIM_DEF_TYPE *) TIM14_BASE)
#define TIM_15               (*(TIM_DEF_TYPE *) TIM15_BASE)
#define TIM_16               (*(TIM_DEF_TYPE *) TIM16_BASE)
#define TIM_17               (*(TIM_DEF_TYPE *) TIM17_BASE)


typedef enum
{
    TIMER_FROZEN,
    TIMER_ACTIVE_IN_MATCH,
    TIMER_INACTIVE_IN_MATCH,
    TIMER_TOGGLE,
    TIMER_FORCE_INACTIVE,
    TIMER_FORCE_ACTIVE,
    TIMER_PWM1,
    TIMER_PWM2
} TIMER_MODE_DEF;

/*******************  Bit definition for TIM_CR1 register  ********************/
#define TIM_CR1_CEN_POS           (0U)
#define TIM_CR1_CEN_MSK           (0x1UL << TIM_CR1_CEN_POS)                   /*!< 0x00000001 */
#define TIM_CR1_CEN               TIM_CR1_CEN_MSK                              /*!<Counter enable */
#define TIM_CR1_UDIS_POS          (1U)
#define TIM_CR1_UDIS_MSK          (0x1UL << TIM_CR1_UDIS_POS)                  /*!< 0x00000002 */
#define TIM_CR1_UDIS              TIM_CR1_UDIS_MSK                             /*!<Update disable */
#define TIM_CR1_URS_POS           (2U)
#define TIM_CR1_URS_MSK           (0x1UL << TIM_CR1_URS_POS)                   /*!< 0x00000004 */
#define TIM_CR1_URS               TIM_CR1_URS_MSK                              /*!<Update request source */
#define TIM_CR1_OPM_POS           (3U)
#define TIM_CR1_OPM_MSK           (0x1UL << TIM_CR1_OPM_POS)                   /*!< 0x00000008 */
#define TIM_CR1_OPM               TIM_CR1_OPM_MSK                              /*!<One pulse mode */
#define TIM_CR1_DIR_POS           (4U)
#define TIM_CR1_DIR_MSK           (0x1UL << TIM_CR1_DIR_POS)                   /*!< 0x00000010 */
#define TIM_CR1_DIR               TIM_CR1_DIR_MSK                              /*!<Direction */

#define TIM_CR1_CMS_POS           (5U)
#define TIM_CR1_CMS_MSK           (0x3UL << TIM_CR1_CMS_POS)                   /*!< 0x00000060 */
#define TIM_CR1_CMS               TIM_CR1_CMS_MSK                              /*!<CMS[1:0] bits (Center-aligned mode selection) */
#define TIM_CR1_CMS_0             (0x1UL << TIM_CR1_CMS_POS)                   /*!< 0x00000020 */
#define TIM_CR1_CMS_1             (0x2UL << TIM_CR1_CMS_POS)                   /*!< 0x00000040 */

#define TIM_CR1_ARPE_POS          (7U)
#define TIM_CR1_ARPE_MSK          (0x1UL << TIM_CR1_ARPE_POS)                  /*!< 0x00000080 */
#define TIM_CR1_ARPE              TIM_CR1_ARPE_MSK                             /*!<Auto-reload preload enable */

#define TIM_CR1_CKD_POS           (8U)
#define TIM_CR1_CKD_MSK           (0x3UL << TIM_CR1_CKD_POS)                   /*!< 0x00000300 */
#define TIM_CR1_CKD               TIM_CR1_CKD_MSK                              /*!<CKD[1:0] bits (clock division) */
#define TIM_CR1_CKD_0             (0x1UL << TIM_CR1_CKD_POS)                   /*!< 0x00000100 */
#define TIM_CR1_CKD_1             (0x2UL << TIM_CR1_CKD_POS)                   /*!< 0x00000200 */

#define TIM_CR1_UIFREMAP_POS      (11U)
#define TIM_CR1_UIFREMAP_MSK      (0x1UL << TIM_CR1_UIFREMAP_POS)              /*!< 0x00000800 */
#define TIM_CR1_UIFREMAP          TIM_CR1_UIFREMAP_MSK                         /*!<Update interrupt flag remap */

/*******************  Bit definition for TIM_CR2 register  ********************/
#define TIM_CR2_CCPC_POS          (0U)
#define TIM_CR2_CCPC_MSK          (0x1UL << TIM_CR2_CCPC_POS)                  /*!< 0x00000001 */
#define TIM_CR2_CCPC              TIM_CR2_CCPC_MSK                             /*!<Capture/Compare Preloaded Control */
#define TIM_CR2_CCUS_POS          (2U)
#define TIM_CR2_CCUS_MSK          (0x1UL << TIM_CR2_CCUS_POS)                  /*!< 0x00000004 */
#define TIM_CR2_CCUS              TIM_CR2_CCUS_MSK                             /*!<Capture/Compare Control Update Selection */
#define TIM_CR2_CCDS_POS          (3U)
#define TIM_CR2_CCDS_MSK          (0x1UL << TIM_CR2_CCDS_POS)                  /*!< 0x00000008 */
#define TIM_CR2_CCDS              TIM_CR2_CCDS_MSK                             /*!<Capture/Compare DMA Selection */

#define TIM_CR2_MMS_POS           (4U)
#define TIM_CR2_MMS_MSK           (0x7UL << TIM_CR2_MMS_POS)                   /*!< 0x00000070 */
#define TIM_CR2_MMS               TIM_CR2_MMS_MSK                              /*!<MMS[2:0] bits (Master Mode Selection) */
#define TIM_CR2_MMS_0             (0x1UL << TIM_CR2_MMS_POS)                   /*!< 0x00000010 */
#define TIM_CR2_MMS_1             (0x2UL << TIM_CR2_MMS_POS)                   /*!< 0x00000020 */
#define TIM_CR2_MMS_2             (0x4UL << TIM_CR2_MMS_POS)                   /*!< 0x00000040 */

#define TIM_CR2_TI1S_POS          (7U)
#define TIM_CR2_TI1S_MSK          (0x1UL << TIM_CR2_TI1S_POS)                  /*!< 0x00000080 */
#define TIM_CR2_TI1S              TIM_CR2_TI1S_MSK                             /*!<TI1 Selection */
#define TIM_CR2_OIS1_POS          (8U)
#define TIM_CR2_OIS1_MSK          (0x1UL << TIM_CR2_OIS1_POS)                  /*!< 0x00000100 */
#define TIM_CR2_OIS1              TIM_CR2_OIS1_MSK                             /*!<Output Idle state 1 (OC1 output) */
#define TIM_CR2_OIS1N_POS         (9U)
#define TIM_CR2_OIS1N_MSK         (0x1UL << TIM_CR2_OIS1N_POS)                 /*!< 0x00000200 */
#define TIM_CR2_OIS1N             TIM_CR2_OIS1N_MSK                            /*!<Output Idle state 1 (OC1N output) */
#define TIM_CR2_OIS2_POS          (10U)
#define TIM_CR2_OIS2_MSK          (0x1UL << TIM_CR2_OIS2_POS)                  /*!< 0x00000400 */
#define TIM_CR2_OIS2              TIM_CR2_OIS2_MSK                             /*!<Output Idle state 2 (OC2 output) */
#define TIM_CR2_OIS2N_POS         (11U)
#define TIM_CR2_OIS2N_MSK         (0x1UL << TIM_CR2_OIS2N_POS)                 /*!< 0x00000800 */
#define TIM_CR2_OIS2N             TIM_CR2_OIS2N_MSK                            /*!<Output Idle state 2 (OC2N output) */
#define TIM_CR2_OIS3_POS          (12U)
#define TIM_CR2_OIS3_MSK          (0x1UL << TIM_CR2_OIS3_POS)                  /*!< 0x00001000 */
#define TIM_CR2_OIS3              TIM_CR2_OIS3_MSK                             /*!<Output Idle state 3 (OC3 output) */
#define TIM_CR2_OIS3N_POS         (13U)
#define TIM_CR2_OIS3N_MSK         (0x1UL << TIM_CR2_OIS3N_POS)                 /*!< 0x00002000 */
#define TIM_CR2_OIS3N             TIM_CR2_OIS3N_MSK                            /*!<Output Idle state 3 (OC3N output) */
#define TIM_CR2_OIS4_POS          (14U)
#define TIM_CR2_OIS4_MSK          (0x1UL << TIM_CR2_OIS4_POS)                  /*!< 0x00004000 */
#define TIM_CR2_OIS4              TIM_CR2_OIS4_MSK                             /*!<Output Idle state 4 (OC4 output) */
#define TIM_CR2_OIS5_POS          (16U)
#define TIM_CR2_OIS5_MSK          (0x1UL << TIM_CR2_OIS5_POS)                  /*!< 0x00010000 */
#define TIM_CR2_OIS5              TIM_CR2_OIS5_MSK                             /*!<Output Idle state 5 (OC5 output) */
#define TIM_CR2_OIS6_POS          (18U)
#define TIM_CR2_OIS6_MSK          (0x1UL << TIM_CR2_OIS6_POS)                  /*!< 0x00040000 */
#define TIM_CR2_OIS6              TIM_CR2_OIS6_MSK                             /*!<Output Idle state 6 (OC6 output) */

#define TIM_CR2_MMS2_POS          (20U)
#define TIM_CR2_MMS2_MSK          (0xFUL << TIM_CR2_MMS2_POS)                  /*!< 0x00F00000 */
#define TIM_CR2_MMS2              TIM_CR2_MMS2_MSK                             /*!<MMS[2:0] bits (Master Mode Selection) */
#define TIM_CR2_MMS2_0            (0x1UL << TIM_CR2_MMS2_POS)                  /*!< 0x00100000 */
#define TIM_CR2_MMS2_1            (0x2UL << TIM_CR2_MMS2_POS)                  /*!< 0x00200000 */
#define TIM_CR2_MMS2_2            (0x4UL << TIM_CR2_MMS2_POS)                  /*!< 0x00400000 */
#define TIM_CR2_MMS2_3            (0x8UL << TIM_CR2_MMS2_POS)                  /*!< 0x00800000 */

/*******************  Bit definition for TIM_SMCR register  *******************/
#define TIM_SMCR_SMS_POS          (0U)
#define TIM_SMCR_SMS_MSK          (0x10007UL << TIM_SMCR_SMS_POS)              /*!< 0x00010007 */
#define TIM_SMCR_SMS              TIM_SMCR_SMS_MSK                             /*!<SMS[2:0] bits (Slave mode selection) */
#define TIM_SMCR_SMS_0            (0x00001UL << TIM_SMCR_SMS_POS)              /*!< 0x00000001 */
#define TIM_SMCR_SMS_1            (0x00002UL << TIM_SMCR_SMS_POS)              /*!< 0x00000002 */
#define TIM_SMCR_SMS_2            (0x00004UL << TIM_SMCR_SMS_POS)              /*!< 0x00000004 */
#define TIM_SMCR_SMS_3            (0x10000UL << TIM_SMCR_SMS_POS)              /*!< 0x00010000 */

#define TIM_SMCR_OCCS_POS         (3U)
#define TIM_SMCR_OCCS_MSK         (0x1UL << TIM_SMCR_OCCS_POS)                 /*!< 0x00000008 */
#define TIM_SMCR_OCCS             TIM_SMCR_OCCS_MSK                            /*!< OCREF clear selection */

#define TIM_SMCR_TS_POS           (4U)
#define TIM_SMCR_TS_MSK           (0x30007UL << TIM_SMCR_TS_POS)               /*!< 0x00300070 */
#define TIM_SMCR_TS               TIM_SMCR_TS_MSK                              /*!<TS[2:0] bits (Trigger selection) */
#define TIM_SMCR_TS_0             (0x00001UL << TIM_SMCR_TS_POS)               /*!< 0x00000010 */
#define TIM_SMCR_TS_1             (0x00002UL << TIM_SMCR_TS_POS)               /*!< 0x00000020 */
#define TIM_SMCR_TS_2             (0x00004UL << TIM_SMCR_TS_POS)               /*!< 0x00000040 */
#define TIM_SMCR_TS_3             (0x10000UL << TIM_SMCR_TS_POS)               /*!< 0x00100000 */
#define TIM_SMCR_TS_4             (0x20000UL << TIM_SMCR_TS_POS)               /*!< 0x00200000 */

#define TIM_SMCR_MSM_POS          (7U)
#define TIM_SMCR_MSM_MSK          (0x1UL << TIM_SMCR_MSM_POS)                  /*!< 0x00000080 */
#define TIM_SMCR_MSM              TIM_SMCR_MSM_MSK                             /*!<Master/slave mode */

#define TIM_SMCR_ETF_POS          (8U)
#define TIM_SMCR_ETF_MSK          (0xFUL << TIM_SMCR_ETF_POS)                  /*!< 0x00000F00 */
#define TIM_SMCR_ETF              TIM_SMCR_ETF_MSK                             /*!<ETF[3:0] bits (External trigger filter) */
#define TIM_SMCR_ETF_0            (0x1UL << TIM_SMCR_ETF_POS)                  /*!< 0x00000100 */
#define TIM_SMCR_ETF_1            (0x2UL << TIM_SMCR_ETF_POS)                  /*!< 0x00000200 */
#define TIM_SMCR_ETF_2            (0x4UL << TIM_SMCR_ETF_POS)                  /*!< 0x00000400 */
#define TIM_SMCR_ETF_3            (0x8UL << TIM_SMCR_ETF_POS)                  /*!< 0x00000800 */

#define TIM_SMCR_ETPS_POS         (12U)
#define TIM_SMCR_ETPS_MSK         (0x3UL << TIM_SMCR_ETPS_POS)                 /*!< 0x00003000 */
#define TIM_SMCR_ETPS             TIM_SMCR_ETPS_MSK                            /*!<ETPS[1:0] bits (External trigger prescaler) */
#define TIM_SMCR_ETPS_0           (0x1UL << TIM_SMCR_ETPS_POS)                 /*!< 0x00001000 */
#define TIM_SMCR_ETPS_1           (0x2UL << TIM_SMCR_ETPS_POS)                 /*!< 0x00002000 */

#define TIM_SMCR_ECE_POS          (14U)
#define TIM_SMCR_ECE_MSK          (0x1UL << TIM_SMCR_ECE_POS)                  /*!< 0x00004000 */
#define TIM_SMCR_ECE              TIM_SMCR_ECE_MSK                             /*!<External clock enable */
#define TIM_SMCR_ETP_POS          (15U)
#define TIM_SMCR_ETP_MSK          (0x1UL << TIM_SMCR_ETP_POS)                  /*!< 0x00008000 */
#define TIM_SMCR_ETP              TIM_SMCR_ETP_MSK                             /*!<External trigger polarity */

/*******************  Bit definition for TIM_DIER register  *******************/
#define TIM_DIER_UIE_POS          (0U)
#define TIM_DIER_UIE_MSK          (0x1UL << TIM_DIER_UIE_POS)                  /*!< 0x00000001 */
#define TIM_DIER_UIE              TIM_DIER_UIE_MSK                             /*!<Update interrupt enable */
#define TIM_DIER_CC1IE_POS        (1U)
#define TIM_DIER_CC1IE_MSK        (0x1UL << TIM_DIER_CC1IE_POS)                /*!< 0x00000002 */
#define TIM_DIER_CC1IE            TIM_DIER_CC1IE_MSK                           /*!<Capture/Compare 1 interrupt enable */
#define TIM_DIER_CC2IE_POS        (2U)
#define TIM_DIER_CC2IE_MSK        (0x1UL << TIM_DIER_CC2IE_POS)                /*!< 0x00000004 */
#define TIM_DIER_CC2IE            TIM_DIER_CC2IE_MSK                           /*!<Capture/Compare 2 interrupt enable */
#define TIM_DIER_CC3IE_POS        (3U)
#define TIM_DIER_CC3IE_MSK        (0x1UL << TIM_DIER_CC3IE_POS)                /*!< 0x00000008 */
#define TIM_DIER_CC3IE            TIM_DIER_CC3IE_MSK                           /*!<Capture/Compare 3 interrupt enable */
#define TIM_DIER_CC4IE_POS        (4U)
#define TIM_DIER_CC4IE_MSK        (0x1UL << TIM_DIER_CC4IE_POS)                /*!< 0x00000010 */
#define TIM_DIER_CC4IE            TIM_DIER_CC4IE_MSK                           /*!<Capture/Compare 4 interrupt enable */
#define TIM_DIER_COMIE_POS        (5U)
#define TIM_DIER_COMIE_MSK        (0x1UL << TIM_DIER_COMIE_POS)                /*!< 0x00000020 */
#define TIM_DIER_COMIE            TIM_DIER_COMIE_MSK                           /*!<COM interrupt enable */
#define TIM_DIER_TIE_POS          (6U)
#define TIM_DIER_TIE_MSK          (0x1UL << TIM_DIER_TIE_POS)                  /*!< 0x00000040 */
#define TIM_DIER_TIE              TIM_DIER_TIE_MSK                             /*!<Trigger interrupt enable */
#define TIM_DIER_BIE_POS          (7U)
#define TIM_DIER_BIE_MSK          (0x1UL << TIM_DIER_BIE_POS)                  /*!< 0x00000080 */
#define TIM_DIER_BIE              TIM_DIER_BIE_MSK                             /*!<Break interrupt enable */
#define TIM_DIER_UDE_POS          (8U)
#define TIM_DIER_UDE_MSK          (0x1UL << TIM_DIER_UDE_POS)                  /*!< 0x00000100 */
#define TIM_DIER_UDE              TIM_DIER_UDE_MSK                             /*!<Update DMA request enable */
#define TIM_DIER_CC1DE_POS        (9U)
#define TIM_DIER_CC1DE_MSK        (0x1UL << TIM_DIER_CC1DE_POS)                /*!< 0x00000200 */
#define TIM_DIER_CC1DE            TIM_DIER_CC1DE_MSK                           /*!<Capture/Compare 1 DMA request enable */
#define TIM_DIER_CC2DE_POS        (10U)
#define TIM_DIER_CC2DE_MSK        (0x1UL << TIM_DIER_CC2DE_POS)                /*!< 0x00000400 */
#define TIM_DIER_CC2DE            TIM_DIER_CC2DE_MSK                           /*!<Capture/Compare 2 DMA request enable */
#define TIM_DIER_CC3DE_POS        (11U)
#define TIM_DIER_CC3DE_MSK        (0x1UL << TIM_DIER_CC3DE_POS)                /*!< 0x00000800 */
#define TIM_DIER_CC3DE            TIM_DIER_CC3DE_MSK                           /*!<Capture/Compare 3 DMA request enable */
#define TIM_DIER_CC4DE_POS        (12U)
#define TIM_DIER_CC4DE_MSK        (0x1UL << TIM_DIER_CC4DE_POS)                /*!< 0x00001000 */
#define TIM_DIER_CC4DE            TIM_DIER_CC4DE_MSK                           /*!<Capture/Compare 4 DMA request enable */
#define TIM_DIER_COMDE_POS        (13U)
#define TIM_DIER_COMDE_MSK        (0x1UL << TIM_DIER_COMDE_POS)                /*!< 0x00002000 */
#define TIM_DIER_COMDE            TIM_DIER_COMDE_MSK                           /*!<COM DMA request enable */
#define TIM_DIER_TDE_POS          (14U)
#define TIM_DIER_TDE_MSK          (0x1UL << TIM_DIER_TDE_POS)                  /*!< 0x00004000 */
#define TIM_DIER_TDE              TIM_DIER_TDE_MSK                             /*!<Trigger DMA request enable */

/********************  Bit definition for TIM_SR register  ********************/
#define TIM_SR_UIF_POS            (0U)
#define TIM_SR_UIF_MSK            (0x1UL << TIM_SR_UIF_POS)                    /*!< 0x00000001 */
#define TIM_SR_UIF                TIM_SR_UIF_MSK                               /*!<Update interrupt Flag */
#define TIM_SR_CC1IF_POS          (1U)
#define TIM_SR_CC1IF_MSK          (0x1UL << TIM_SR_CC1IF_POS)                  /*!< 0x00000002 */
#define TIM_SR_CC1IF              TIM_SR_CC1IF_MSK                             /*!<Capture/Compare 1 interrupt Flag */
#define TIM_SR_CC2IF_POS          (2U)
#define TIM_SR_CC2IF_MSK          (0x1UL << TIM_SR_CC2IF_POS)                  /*!< 0x00000004 */
#define TIM_SR_CC2IF              TIM_SR_CC2IF_MSK                             /*!<Capture/Compare 2 interrupt Flag */
#define TIM_SR_CC3IF_POS          (3U)
#define TIM_SR_CC3IF_MSK          (0x1UL << TIM_SR_CC3IF_POS)                  /*!< 0x00000008 */
#define TIM_SR_CC3IF              TIM_SR_CC3IF_MSK                             /*!<Capture/Compare 3 interrupt Flag */
#define TIM_SR_CC4IF_POS          (4U)
#define TIM_SR_CC4IF_MSK          (0x1UL << TIM_SR_CC4IF_POS)                  /*!< 0x00000010 */
#define TIM_SR_CC4IF              TIM_SR_CC4IF_MSK                             /*!<Capture/Compare 4 interrupt Flag */
#define TIM_SR_COMIF_POS          (5U)
#define TIM_SR_COMIF_MSK          (0x1UL << TIM_SR_COMIF_POS)                  /*!< 0x00000020 */
#define TIM_SR_COMIF              TIM_SR_COMIF_MSK                             /*!<COM interrupt Flag */
#define TIM_SR_TIF_POS            (6U)
#define TIM_SR_TIF_MSK            (0x1UL << TIM_SR_TIF_POS)                    /*!< 0x00000040 */
#define TIM_SR_TIF                TIM_SR_TIF_MSK                               /*!<Trigger interrupt Flag */
#define TIM_SR_BIF_POS            (7U)
#define TIM_SR_BIF_MSK            (0x1UL << TIM_SR_BIF_POS)                    /*!< 0x00000080 */
#define TIM_SR_BIF                TIM_SR_BIF_MSK                               /*!<Break interrupt Flag */
#define TIM_SR_B2IF_POS           (8U)
#define TIM_SR_B2IF_MSK           (0x1UL << TIM_SR_B2IF_POS)                   /*!< 0x00000100 */
#define TIM_SR_B2IF               TIM_SR_B2IF_MSK                              /*!<Break 2 interrupt Flag */
#define TIM_SR_CC1OF_POS          (9U)
#define TIM_SR_CC1OF_MSK          (0x1UL << TIM_SR_CC1OF_POS)                  /*!< 0x00000200 */
#define TIM_SR_CC1OF              TIM_SR_CC1OF_MSK                             /*!<Capture/Compare 1 Overcapture Flag */
#define TIM_SR_CC2OF_POS          (10U)
#define TIM_SR_CC2OF_MSK          (0x1UL << TIM_SR_CC2OF_POS)                  /*!< 0x00000400 */
#define TIM_SR_CC2OF              TIM_SR_CC2OF_MSK                             /*!<Capture/Compare 2 Overcapture Flag */
#define TIM_SR_CC3OF_POS          (11U)
#define TIM_SR_CC3OF_MSK          (0x1UL << TIM_SR_CC3OF_POS)                  /*!< 0x00000800 */
#define TIM_SR_CC3OF              TIM_SR_CC3OF_MSK                             /*!<Capture/Compare 3 Overcapture Flag */
#define TIM_SR_CC4OF_POS          (12U)
#define TIM_SR_CC4OF_MSK          (0x1UL << TIM_SR_CC4OF_POS)                  /*!< 0x00001000 */
#define TIM_SR_CC4OF              TIM_SR_CC4OF_MSK                             /*!<Capture/Compare 4 Overcapture Flag */
#define TIM_SR_SBIF_POS           (13U)
#define TIM_SR_SBIF_MSK           (0x1UL << TIM_SR_SBIF_POS)                   /*!< 0x00002000 */
#define TIM_SR_SBIF               TIM_SR_SBIF_MSK                              /*!<System Break interrupt Flag */
#define TIM_SR_CC5IF_POS          (16U)
#define TIM_SR_CC5IF_MSK          (0x1UL << TIM_SR_CC5IF_POS)                  /*!< 0x00010000 */
#define TIM_SR_CC5IF              TIM_SR_CC5IF_MSK                             /*!<Capture/Compare 5 interrupt Flag */
#define TIM_SR_CC6IF_POS          (17U)
#define TIM_SR_CC6IF_MSK          (0x1UL << TIM_SR_CC6IF_POS)                  /*!< 0x00020000 */
#define TIM_SR_CC6IF              TIM_SR_CC6IF_MSK                             /*!<Capture/Compare 6 interrupt Flag */


/*******************  Bit definition for TIM_EGR register  ********************/
#define TIM_EGR_UG_POS            (0U)
#define TIM_EGR_UG_MSK            (0x1UL << TIM_EGR_UG_POS)                    /*!< 0x00000001 */
#define TIM_EGR_UG                TIM_EGR_UG_MSK                               /*!<Update Generation */
#define TIM_EGR_CC1G_POS          (1U)
#define TIM_EGR_CC1G_MSK          (0x1UL << TIM_EGR_CC1G_POS)                  /*!< 0x00000002 */
#define TIM_EGR_CC1G              TIM_EGR_CC1G_MSK                             /*!<Capture/Compare 1 Generation */
#define TIM_EGR_CC2G_POS          (2U)
#define TIM_EGR_CC2G_MSK          (0x1UL << TIM_EGR_CC2G_POS)                  /*!< 0x00000004 */
#define TIM_EGR_CC2G              TIM_EGR_CC2G_MSK                             /*!<Capture/Compare 2 Generation */
#define TIM_EGR_CC3G_POS          (3U)
#define TIM_EGR_CC3G_MSK          (0x1UL << TIM_EGR_CC3G_POS)                  /*!< 0x00000008 */
#define TIM_EGR_CC3G              TIM_EGR_CC3G_MSK                             /*!<Capture/Compare 3 Generation */
#define TIM_EGR_CC4G_POS          (4U)
#define TIM_EGR_CC4G_MSK          (0x1UL << TIM_EGR_CC4G_POS)                  /*!< 0x00000010 */
#define TIM_EGR_CC4G              TIM_EGR_CC4G_MSK                             /*!<Capture/Compare 4 Generation */
#define TIM_EGR_COMG_POS          (5U)
#define TIM_EGR_COMG_MSK          (0x1UL << TIM_EGR_COMG_POS)                  /*!< 0x00000020 */
#define TIM_EGR_COMG              TIM_EGR_COMG_MSK                             /*!<Capture/Compare Control Update Generation */
#define TIM_EGR_TG_POS            (6U)
#define TIM_EGR_TG_MSK            (0x1UL << TIM_EGR_TG_POS)                    /*!< 0x00000040 */
#define TIM_EGR_TG                TIM_EGR_TG_MSK                               /*!<Trigger Generation */
#define TIM_EGR_BG_POS            (7U)
#define TIM_EGR_BG_MSK            (0x1UL << TIM_EGR_BG_POS)                    /*!< 0x00000080 */
#define TIM_EGR_BG                TIM_EGR_BG_MSK                               /*!<Break Generation */
#define TIM_EGR_B2G_POS           (8U)
#define TIM_EGR_B2G_MSK           (0x1UL << TIM_EGR_B2G_POS)                   /*!< 0x00000100 */
#define TIM_EGR_B2G               TIM_EGR_B2G_MSK                              /*!<Break 2 Generation */


/******************  Bit definition for TIM_CCMR1 register  *******************/
#define TIM_CCMR1_CC1S_POS        (0U)
#define TIM_CCMR1_CC1S_MSK        (0x3UL << TIM_CCMR1_CC1S_POS)                /*!< 0x00000003 */
#define TIM_CCMR1_CC1S            TIM_CCMR1_CC1S_MSK                           /*!<CC1S[1:0] bits (Capture/Compare 1 Selection) */
#define TIM_CCMR1_CC1S_0          (0x1UL << TIM_CCMR1_CC1S_POS)                /*!< 0x00000001 */
#define TIM_CCMR1_CC1S_1          (0x2UL << TIM_CCMR1_CC1S_POS)                /*!< 0x00000002 */

#define TIM_CCMR1_OC1FE_POS       (2U)
#define TIM_CCMR1_OC1FE_MSK       (0x1UL << TIM_CCMR1_OC1FE_POS)               /*!< 0x00000004 */
#define TIM_CCMR1_OC1FE           TIM_CCMR1_OC1FE_MSK                          /*!<Output Compare 1 Fast enable */
#define TIM_CCMR1_OC1PE_POS       (3U)
#define TIM_CCMR1_OC1PE_MSK       (0x1UL << TIM_CCMR1_OC1PE_POS)               /*!< 0x00000008 */
#define TIM_CCMR1_OC1PE           TIM_CCMR1_OC1PE_MSK                          /*!<Output Compare 1 Preload enable */

#define TIM_CCMR1_OC1M_POS        (4U)
#define TIM_CCMR1_OC1M_MSK        (0x1007UL << TIM_CCMR1_OC1M_POS)             /*!< 0x00010070 */
#define TIM_CCMR1_OC1M            TIM_CCMR1_OC1M_MSK                           /*!<OC1M[2:0] bits (Output Compare 1 Mode) */
#define TIM_CCMR1_OC1M_0          (0x0001UL << TIM_CCMR1_OC1M_POS)             /*!< 0x00000010 */
#define TIM_CCMR1_OC1M_1          (0x0002UL << TIM_CCMR1_OC1M_POS)             /*!< 0x00000020 */
#define TIM_CCMR1_OC1M_2          (0x0004UL << TIM_CCMR1_OC1M_POS)             /*!< 0x00000040 */
#define TIM_CCMR1_OC1M_3          (0x1000UL << TIM_CCMR1_OC1M_POS)             /*!< 0x00010000 */

#define TIM_CCMR1_OC1CE_POS       (7U)
#define TIM_CCMR1_OC1CE_MSK       (0x1UL << TIM_CCMR1_OC1CE_POS)               /*!< 0x00000080 */
#define TIM_CCMR1_OC1CE           TIM_CCMR1_OC1CE_MSK                          /*!<Output Compare 1 Clear Enable */

#define TIM_CCMR1_CC2S_POS        (8U)
#define TIM_CCMR1_CC2S_MSK        (0x3UL << TIM_CCMR1_CC2S_POS)                /*!< 0x00000300 */
#define TIM_CCMR1_CC2S            TIM_CCMR1_CC2S_MSK                           /*!<CC2S[1:0] bits (Capture/Compare 2 Selection) */
#define TIM_CCMR1_CC2S_0          (0x1UL << TIM_CCMR1_CC2S_POS)                /*!< 0x00000100 */
#define TIM_CCMR1_CC2S_1          (0x2UL << TIM_CCMR1_CC2S_POS)                /*!< 0x00000200 */

#define TIM_CCMR1_OC2FE_POS       (10U)
#define TIM_CCMR1_OC2FE_MSK       (0x1UL << TIM_CCMR1_OC2FE_POS)               /*!< 0x00000400 */
#define TIM_CCMR1_OC2FE           TIM_CCMR1_OC2FE_MSK                          /*!<Output Compare 2 Fast enable */
#define TIM_CCMR1_OC2PE_POS       (11U)
#define TIM_CCMR1_OC2PE_MSK       (0x1UL << TIM_CCMR1_OC2PE_POS)               /*!< 0x00000800 */
#define TIM_CCMR1_OC2PE           TIM_CCMR1_OC2PE_MSK                          /*!<Output Compare 2 Preload enable */

#define TIM_CCMR1_OC2M_POS        (12U)
#define TIM_CCMR1_OC2M_MSK        (0x1007UL << TIM_CCMR1_OC2M_POS)             /*!< 0x01007000 */
#define TIM_CCMR1_OC2M            TIM_CCMR1_OC2M_MSK                           /*!<OC2M[2:0] bits (Output Compare 2 Mode) */
#define TIM_CCMR1_OC2M_0          (0x0001UL << TIM_CCMR1_OC2M_POS)             /*!< 0x00001000 */
#define TIM_CCMR1_OC2M_1          (0x0002UL << TIM_CCMR1_OC2M_POS)             /*!< 0x00002000 */
#define TIM_CCMR1_OC2M_2          (0x0004UL << TIM_CCMR1_OC2M_POS)             /*!< 0x00004000 */
#define TIM_CCMR1_OC2M_3          (0x1000UL << TIM_CCMR1_OC2M_POS)             /*!< 0x01000000 */

#define TIM_CCMR1_OC2CE_POS       (15U)
#define TIM_CCMR1_OC2CE_MSK       (0x1UL << TIM_CCMR1_OC2CE_POS)               /*!< 0x00008000 */
#define TIM_CCMR1_OC2CE           TIM_CCMR1_OC2CE_MSK                          /*!<Output Compare 2 Clear Enable */

/*----------------------------------------------------------------------------*/
#define TIM_CCMR1_IC1PSC_POS      (2U)
#define TIM_CCMR1_IC1PSC_MSK      (0x3UL << TIM_CCMR1_IC1PSC_POS)              /*!< 0x0000000C */
#define TIM_CCMR1_IC1PSC          TIM_CCMR1_IC1PSC_MSK                         /*!<IC1PSC[1:0] bits (Input Capture 1 Prescaler) */
#define TIM_CCMR1_IC1PSC_0        (0x1UL << TIM_CCMR1_IC1PSC_POS)              /*!< 0x00000004 */
#define TIM_CCMR1_IC1PSC_1        (0x2UL << TIM_CCMR1_IC1PSC_POS)              /*!< 0x00000008 */

#define TIM_CCMR1_IC1F_POS        (4U)
#define TIM_CCMR1_IC1F_MSK        (0xFUL << TIM_CCMR1_IC1F_POS)                /*!< 0x000000F0 */
#define TIM_CCMR1_IC1F            TIM_CCMR1_IC1F_MSK                           /*!<IC1F[3:0] bits (Input Capture 1 Filter) */
#define TIM_CCMR1_IC1F_0          (0x1UL << TIM_CCMR1_IC1F_POS)                /*!< 0x00000010 */
#define TIM_CCMR1_IC1F_1          (0x2UL << TIM_CCMR1_IC1F_POS)                /*!< 0x00000020 */
#define TIM_CCMR1_IC1F_2          (0x4UL << TIM_CCMR1_IC1F_POS)                /*!< 0x00000040 */
#define TIM_CCMR1_IC1F_3          (0x8UL << TIM_CCMR1_IC1F_POS)                /*!< 0x00000080 */

#define TIM_CCMR1_IC2PSC_POS      (10U)
#define TIM_CCMR1_IC2PSC_MSK      (0x3UL << TIM_CCMR1_IC2PSC_POS)              /*!< 0x00000C00 */
#define TIM_CCMR1_IC2PSC          TIM_CCMR1_IC2PSC_MSK                         /*!<IC2PSC[1:0] bits (Input Capture 2 Prescaler) */
#define TIM_CCMR1_IC2PSC_0        (0x1UL << TIM_CCMR1_IC2PSC_POS)              /*!< 0x00000400 */
#define TIM_CCMR1_IC2PSC_1        (0x2UL << TIM_CCMR1_IC2PSC_POS)              /*!< 0x00000800 */

#define TIM_CCMR1_IC2F_POS        (12U)
#define TIM_CCMR1_IC2F_MSK        (0xFUL << TIM_CCMR1_IC2F_POS)                /*!< 0x0000F000 */
#define TIM_CCMR1_IC2F            TIM_CCMR1_IC2F_MSK                           /*!<IC2F[3:0] bits (Input Capture 2 Filter) */
#define TIM_CCMR1_IC2F_0          (0x1UL << TIM_CCMR1_IC2F_POS)                /*!< 0x00001000 */
#define TIM_CCMR1_IC2F_1          (0x2UL << TIM_CCMR1_IC2F_POS)                /*!< 0x00002000 */
#define TIM_CCMR1_IC2F_2          (0x4UL << TIM_CCMR1_IC2F_POS)                /*!< 0x00004000 */
#define TIM_CCMR1_IC2F_3          (0x8UL << TIM_CCMR1_IC2F_POS)                /*!< 0x00008000 */

/******************  Bit definition for TIM_CCMR2 register  *******************/
#define TIM_CCMR2_CC3S_POS        (0U)
#define TIM_CCMR2_CC3S_MSK        (0x3UL << TIM_CCMR2_CC3S_POS)                /*!< 0x00000003 */
#define TIM_CCMR2_CC3S            TIM_CCMR2_CC3S_MSK                           /*!<CC3S[1:0] bits (Capture/Compare 3 Selection) */
#define TIM_CCMR2_CC3S_0          (0x1UL << TIM_CCMR2_CC3S_POS)                /*!< 0x00000001 */
#define TIM_CCMR2_CC3S_1          (0x2UL << TIM_CCMR2_CC3S_POS)                /*!< 0x00000002 */

#define TIM_CCMR2_OC3FE_POS       (2U)
#define TIM_CCMR2_OC3FE_MSK       (0x1UL << TIM_CCMR2_OC3FE_POS)               /*!< 0x00000004 */
#define TIM_CCMR2_OC3FE           TIM_CCMR2_OC3FE_MSK                          /*!<Output Compare 3 Fast enable */
#define TIM_CCMR2_OC3PE_POS       (3U)
#define TIM_CCMR2_OC3PE_MSK       (0x1UL << TIM_CCMR2_OC3PE_POS)               /*!< 0x00000008 */
#define TIM_CCMR2_OC3PE           TIM_CCMR2_OC3PE_MSK                          /*!<Output Compare 3 Preload enable */

#define TIM_CCMR2_OC3M_POS        (4U)
#define TIM_CCMR2_OC3M_MSK        (0x1007UL << TIM_CCMR2_OC3M_POS)             /*!< 0x00010070 */
#define TIM_CCMR2_OC3M            TIM_CCMR2_OC3M_MSK                           /*!<OC3M[2:0] bits (Output Compare 3 Mode) */
#define TIM_CCMR2_OC3M_0          (0x0001UL << TIM_CCMR2_OC3M_POS)             /*!< 0x00000010 */
#define TIM_CCMR2_OC3M_1          (0x0002UL << TIM_CCMR2_OC3M_POS)             /*!< 0x00000020 */
#define TIM_CCMR2_OC3M_2          (0x0004UL << TIM_CCMR2_OC3M_POS)             /*!< 0x00000040 */
#define TIM_CCMR2_OC3M_3          (0x1000UL << TIM_CCMR2_OC3M_POS)             /*!< 0x00010000 */

#define TIM_CCMR2_OC3CE_POS       (7U)
#define TIM_CCMR2_OC3CE_MSK       (0x1UL << TIM_CCMR2_OC3CE_POS)               /*!< 0x00000080 */
#define TIM_CCMR2_OC3CE           TIM_CCMR2_OC3CE_MSK                          /*!<Output Compare 3 Clear Enable */

#define TIM_CCMR2_CC4S_POS        (8U)
#define TIM_CCMR2_CC4S_MSK        (0x3UL << TIM_CCMR2_CC4S_POS)                /*!< 0x00000300 */
#define TIM_CCMR2_CC4S            TIM_CCMR2_CC4S_MSK                           /*!<CC4S[1:0] bits (Capture/Compare 4 Selection) */
#define TIM_CCMR2_CC4S_0          (0x1UL << TIM_CCMR2_CC4S_POS)                /*!< 0x00000100 */
#define TIM_CCMR2_CC4S_1          (0x2UL << TIM_CCMR2_CC4S_POS)                /*!< 0x00000200 */

#define TIM_CCMR2_OC4FE_POS       (10U)
#define TIM_CCMR2_OC4FE_MSK       (0x1UL << TIM_CCMR2_OC4FE_POS)               /*!< 0x00000400 */
#define TIM_CCMR2_OC4FE           TIM_CCMR2_OC4FE_MSK                          /*!<Output Compare 4 Fast enable */
#define TIM_CCMR2_OC4PE_POS       (11U)
#define TIM_CCMR2_OC4PE_MSK       (0x1UL << TIM_CCMR2_OC4PE_POS)               /*!< 0x00000800 */
#define TIM_CCMR2_OC4PE           TIM_CCMR2_OC4PE_MSK                          /*!<Output Compare 4 Preload enable */

#define TIM_CCMR2_OC4M_POS        (12U)
#define TIM_CCMR2_OC4M_MSK        (0x1007UL << TIM_CCMR2_OC4M_POS)             /*!< 0x01007000 */
#define TIM_CCMR2_OC4M            TIM_CCMR2_OC4M_MSK                           /*!<OC4M[2:0] bits (Output Compare 4 Mode) */
#define TIM_CCMR2_OC4M_0          (0x0001UL << TIM_CCMR2_OC4M_POS)             /*!< 0x00001000 */
#define TIM_CCMR2_OC4M_1          (0x0002UL << TIM_CCMR2_OC4M_POS)             /*!< 0x00002000 */
#define TIM_CCMR2_OC4M_2          (0x0004UL << TIM_CCMR2_OC4M_POS)             /*!< 0x00004000 */
#define TIM_CCMR2_OC4M_3          (0x1000UL << TIM_CCMR2_OC4M_POS)             /*!< 0x01000000 */

#define TIM_CCMR2_OC4CE_POS       (15U)
#define TIM_CCMR2_OC4CE_MSK       (0x1UL << TIM_CCMR2_OC4CE_POS)               /*!< 0x00008000 */
#define TIM_CCMR2_OC4CE           TIM_CCMR2_OC4CE_MSK                          /*!<Output Compare 4 Clear Enable */

/*----------------------------------------------------------------------------*/
#define TIM_CCMR2_IC3PSC_POS      (2U)
#define TIM_CCMR2_IC3PSC_MSK      (0x3UL << TIM_CCMR2_IC3PSC_POS)              /*!< 0x0000000C */
#define TIM_CCMR2_IC3PSC          TIM_CCMR2_IC3PSC_MSK                         /*!<IC3PSC[1:0] bits (Input Capture 3 Prescaler) */
#define TIM_CCMR2_IC3PSC_0        (0x1UL << TIM_CCMR2_IC3PSC_POS)              /*!< 0x00000004 */
#define TIM_CCMR2_IC3PSC_1        (0x2UL << TIM_CCMR2_IC3PSC_POS)              /*!< 0x00000008 */

#define TIM_CCMR2_IC3F_POS        (4U)
#define TIM_CCMR2_IC3F_MSK        (0xFUL << TIM_CCMR2_IC3F_POS)                /*!< 0x000000F0 */
#define TIM_CCMR2_IC3F            TIM_CCMR2_IC3F_MSK                           /*!<IC3F[3:0] bits (Input Capture 3 Filter) */
#define TIM_CCMR2_IC3F_0          (0x1UL << TIM_CCMR2_IC3F_POS)                /*!< 0x00000010 */
#define TIM_CCMR2_IC3F_1          (0x2UL << TIM_CCMR2_IC3F_POS)                /*!< 0x00000020 */
#define TIM_CCMR2_IC3F_2          (0x4UL << TIM_CCMR2_IC3F_POS)                /*!< 0x00000040 */
#define TIM_CCMR2_IC3F_3          (0x8UL << TIM_CCMR2_IC3F_POS)                /*!< 0x00000080 */

#define TIM_CCMR2_IC4PSC_POS      (10U)
#define TIM_CCMR2_IC4PSC_MSK      (0x3UL << TIM_CCMR2_IC4PSC_POS)              /*!< 0x00000C00 */
#define TIM_CCMR2_IC4PSC          TIM_CCMR2_IC4PSC_MSK                         /*!<IC4PSC[1:0] bits (Input Capture 4 Prescaler) */
#define TIM_CCMR2_IC4PSC_0        (0x1UL << TIM_CCMR2_IC4PSC_POS)              /*!< 0x00000400 */
#define TIM_CCMR2_IC4PSC_1        (0x2UL << TIM_CCMR2_IC4PSC_POS)              /*!< 0x00000800 */

#define TIM_CCMR2_IC4F_POS        (12U)
#define TIM_CCMR2_IC4F_MSK        (0xFUL << TIM_CCMR2_IC4F_POS)                /*!< 0x0000F000 */
#define TIM_CCMR2_IC4F            TIM_CCMR2_IC4F_MSK                           /*!<IC4F[3:0] bits (Input Capture 4 Filter) */
#define TIM_CCMR2_IC4F_0          (0x1UL << TIM_CCMR2_IC4F_POS)                /*!< 0x00001000 */
#define TIM_CCMR2_IC4F_1          (0x2UL << TIM_CCMR2_IC4F_POS)                /*!< 0x00002000 */
#define TIM_CCMR2_IC4F_2          (0x4UL << TIM_CCMR2_IC4F_POS)                /*!< 0x00004000 */
#define TIM_CCMR2_IC4F_3          (0x8UL << TIM_CCMR2_IC4F_POS)                /*!< 0x00008000 */

/******************  Bit definition for TIM_CCMR3 register  *******************/
#define TIM_CCMR3_OC5FE_POS       (2U)
#define TIM_CCMR3_OC5FE_MSK       (0x1UL << TIM_CCMR3_OC5FE_POS)               /*!< 0x00000004 */
#define TIM_CCMR3_OC5FE           TIM_CCMR3_OC5FE_MSK                          /*!<Output Compare 5 Fast enable */
#define TIM_CCMR3_OC5PE_POS       (3U)
#define TIM_CCMR3_OC5PE_MSK       (0x1UL << TIM_CCMR3_OC5PE_POS)               /*!< 0x00000008 */
#define TIM_CCMR3_OC5PE           TIM_CCMR3_OC5PE_MSK                          /*!<Output Compare 5 Preload enable */

#define TIM_CCMR3_OC5M_POS        (4U)
#define TIM_CCMR3_OC5M_MSK        (0x1007UL << TIM_CCMR3_OC5M_POS)             /*!< 0x00010070 */
#define TIM_CCMR3_OC5M            TIM_CCMR3_OC5M_MSK                           /*!<OC5M[3:0] bits (Output Compare 5 Mode) */
#define TIM_CCMR3_OC5M_0          (0x0001UL << TIM_CCMR3_OC5M_POS)             /*!< 0x00000010 */
#define TIM_CCMR3_OC5M_1          (0x0002UL << TIM_CCMR3_OC5M_POS)             /*!< 0x00000020 */
#define TIM_CCMR3_OC5M_2          (0x0004UL << TIM_CCMR3_OC5M_POS)             /*!< 0x00000040 */
#define TIM_CCMR3_OC5M_3          (0x1000UL << TIM_CCMR3_OC5M_POS)             /*!< 0x00010000 */

#define TIM_CCMR3_OC5CE_POS       (7U)
#define TIM_CCMR3_OC5CE_MSK       (0x1UL << TIM_CCMR3_OC5CE_POS)               /*!< 0x00000080 */
#define TIM_CCMR3_OC5CE           TIM_CCMR3_OC5CE_MSK                          /*!<Output Compare 5 Clear Enable */

#define TIM_CCMR3_OC6FE_POS       (10U)
#define TIM_CCMR3_OC6FE_MSK       (0x1UL << TIM_CCMR3_OC6FE_POS)               /*!< 0x00000400 */
#define TIM_CCMR3_OC6FE           TIM_CCMR3_OC6FE_MSK                          /*!<Output Compare 6 Fast enable */
#define TIM_CCMR3_OC6PE_POS       (11U)
#define TIM_CCMR3_OC6PE_MSK       (0x1UL << TIM_CCMR3_OC6PE_POS)               /*!< 0x00000800 */
#define TIM_CCMR3_OC6PE           TIM_CCMR3_OC6PE_MSK                          /*!<Output Compare 6 Preload enable */

#define TIM_CCMR3_OC6M_POS        (12U)
#define TIM_CCMR3_OC6M_MSK        (0x1007UL << TIM_CCMR3_OC6M_POS)             /*!< 0x01007000 */
#define TIM_CCMR3_OC6M            TIM_CCMR3_OC6M_MSK                           /*!<OC6M[3:0] bits (Output Compare 6 Mode) */
#define TIM_CCMR3_OC6M_0          (0x0001UL << TIM_CCMR3_OC6M_POS)             /*!< 0x00001000 */
#define TIM_CCMR3_OC6M_1          (0x0002UL << TIM_CCMR3_OC6M_POS)             /*!< 0x00002000 */
#define TIM_CCMR3_OC6M_2          (0x0004UL << TIM_CCMR3_OC6M_POS)             /*!< 0x00004000 */
#define TIM_CCMR3_OC6M_3          (0x1000UL << TIM_CCMR3_OC6M_POS)             /*!< 0x01000000 */

#define TIM_CCMR3_OC6CE_POS       (15U)
#define TIM_CCMR3_OC6CE_MSK       (0x1UL << TIM_CCMR3_OC6CE_POS)               /*!< 0x00008000 */
#define TIM_CCMR3_OC6CE           TIM_CCMR3_OC6CE_MSK                          /*!<Output Compare 6 Clear Enable */

/*******************  Bit definition for TIM_CCER register  *******************/
#define TIM_CCER_CC1E_POS         (0U)
#define TIM_CCER_CC1E_MSK         (0x1UL << TIM_CCER_CC1E_POS)                 /*!< 0x00000001 */
#define TIM_CCER_CC1E             TIM_CCER_CC1E_MSK                            /*!<Capture/Compare 1 output enable */
#define TIM_CCER_CC1P_POS         (1U)
#define TIM_CCER_CC1P_MSK         (0x1UL << TIM_CCER_CC1P_POS)                 /*!< 0x00000002 */
#define TIM_CCER_CC1P             TIM_CCER_CC1P_MSK                            /*!<Capture/Compare 1 output Polarity */
#define TIM_CCER_CC1NE_POS        (2U)
#define TIM_CCER_CC1NE_MSK        (0x1UL << TIM_CCER_CC1NE_POS)                /*!< 0x00000004 */
#define TIM_CCER_CC1NE            TIM_CCER_CC1NE_MSK                           /*!<Capture/Compare 1 Complementary output enable */
#define TIM_CCER_CC1NP_POS        (3U)
#define TIM_CCER_CC1NP_MSK        (0x1UL << TIM_CCER_CC1NP_POS)                /*!< 0x00000008 */
#define TIM_CCER_CC1NP            TIM_CCER_CC1NP_MSK                           /*!<Capture/Compare 1 Complementary output Polarity */
#define TIM_CCER_CC2E_POS         (4U)
#define TIM_CCER_CC2E_MSK         (0x1UL << TIM_CCER_CC2E_POS)                 /*!< 0x00000010 */
#define TIM_CCER_CC2E             TIM_CCER_CC2E_MSK                            /*!<Capture/Compare 2 output enable */
#define TIM_CCER_CC2P_POS         (5U)
#define TIM_CCER_CC2P_MSK         (0x1UL << TIM_CCER_CC2P_POS)                 /*!< 0x00000020 */
#define TIM_CCER_CC2P             TIM_CCER_CC2P_MSK                            /*!<Capture/Compare 2 output Polarity */
#define TIM_CCER_CC2NE_POS        (6U)
#define TIM_CCER_CC2NE_MSK        (0x1UL << TIM_CCER_CC2NE_POS)                /*!< 0x00000040 */
#define TIM_CCER_CC2NE            TIM_CCER_CC2NE_MSK                           /*!<Capture/Compare 2 Complementary output enable */
#define TIM_CCER_CC2NP_POS        (7U)
#define TIM_CCER_CC2NP_MSK        (0x1UL << TIM_CCER_CC2NP_POS)                /*!< 0x00000080 */
#define TIM_CCER_CC2NP            TIM_CCER_CC2NP_MSK                           /*!<Capture/Compare 2 Complementary output Polarity */
#define TIM_CCER_CC3E_POS         (8U)
#define TIM_CCER_CC3E_MSK         (0x1UL << TIM_CCER_CC3E_POS)                 /*!< 0x00000100 */
#define TIM_CCER_CC3E             TIM_CCER_CC3E_MSK                            /*!<Capture/Compare 3 output enable */
#define TIM_CCER_CC3P_POS         (9U)
#define TIM_CCER_CC3P_MSK         (0x1UL << TIM_CCER_CC3P_POS)                 /*!< 0x00000200 */
#define TIM_CCER_CC3P             TIM_CCER_CC3P_MSK                            /*!<Capture/Compare 3 output Polarity */
#define TIM_CCER_CC3NE_POS        (10U)
#define TIM_CCER_CC3NE_MSK        (0x1UL << TIM_CCER_CC3NE_POS)                /*!< 0x00000400 */
#define TIM_CCER_CC3NE            TIM_CCER_CC3NE_MSK                           /*!<Capture/Compare 3 Complementary output enable */
#define TIM_CCER_CC3NP_POS        (11U)
#define TIM_CCER_CC3NP_MSK        (0x1UL << TIM_CCER_CC3NP_POS)                /*!< 0x00000800 */
#define TIM_CCER_CC3NP            TIM_CCER_CC3NP_MSK                           /*!<Capture/Compare 3 Complementary output Polarity */
#define TIM_CCER_CC4E_POS         (12U)
#define TIM_CCER_CC4E_MSK         (0x1UL << TIM_CCER_CC4E_POS)                 /*!< 0x00001000 */
#define TIM_CCER_CC4E             TIM_CCER_CC4E_MSK                            /*!<Capture/Compare 4 output enable */
#define TIM_CCER_CC4P_POS         (13U)
#define TIM_CCER_CC4P_MSK         (0x1UL << TIM_CCER_CC4P_POS)                 /*!< 0x00002000 */
#define TIM_CCER_CC4P             TIM_CCER_CC4P_MSK                            /*!<Capture/Compare 4 output Polarity */
#define TIM_CCER_CC4NP_POS        (15U)
#define TIM_CCER_CC4NP_MSK        (0x1UL << TIM_CCER_CC4NP_POS)                /*!< 0x00008000 */
#define TIM_CCER_CC4NP            TIM_CCER_CC4NP_MSK                           /*!<Capture/Compare 4 Complementary output Polarity */
#define TIM_CCER_CC5E_POS         (16U)
#define TIM_CCER_CC5E_MSK         (0x1UL << TIM_CCER_CC5E_POS)                 /*!< 0x00010000 */
#define TIM_CCER_CC5E             TIM_CCER_CC5E_MSK                            /*!<Capture/Compare 5 output enable */
#define TIM_CCER_CC5P_POS         (17U)
#define TIM_CCER_CC5P_MSK         (0x1UL << TIM_CCER_CC5P_POS)                 /*!< 0x00020000 */
#define TIM_CCER_CC5P             TIM_CCER_CC5P_MSK                            /*!<Capture/Compare 5 output Polarity */
#define TIM_CCER_CC6E_POS         (20U)
#define TIM_CCER_CC6E_MSK         (0x1UL << TIM_CCER_CC6E_POS)                 /*!< 0x00100000 */
#define TIM_CCER_CC6E             TIM_CCER_CC6E_MSK                            /*!<Capture/Compare 6 output enable */
#define TIM_CCER_CC6P_POS         (21U)
#define TIM_CCER_CC6P_MSK         (0x1UL << TIM_CCER_CC6P_POS)                 /*!< 0x00200000 */
#define TIM_CCER_CC6P             TIM_CCER_CC6P_MSK                            /*!<Capture/Compare 6 output Polarity */

/*******************  Bit definition for TIM_CNT register  ********************/
#define TIM_CNT_CNT_POS           (0U)
#define TIM_CNT_CNT_MSK           (0xFFFFFFFFUL << TIM_CNT_CNT_POS)            /*!< 0xFFFFFFFF */
#define TIM_CNT_CNT               TIM_CNT_CNT_MSK                              /*!<Counter Value */
#define TIM_CNT_UIFCPY_POS        (31U)
#define TIM_CNT_UIFCPY_MSK        (0x1UL << TIM_CNT_UIFCPY_POS)                /*!< 0x80000000 */
#define TIM_CNT_UIFCPY            TIM_CNT_UIFCPY_MSK                           /*!<Update interrupt flag copy (if UIFREMAP=1) */

/*******************  Bit definition for TIM_PSC register  ********************/
#define TIM_PSC_PSC_POS           (0U)
#define TIM_PSC_PSC_MSK           (0xFFFFUL << TIM_PSC_PSC_POS)                /*!< 0x0000FFFF */
#define TIM_PSC_PSC               TIM_PSC_PSC_MSK                              /*!<Prescaler Value */

/*******************  Bit definition for TIM_ARR register  ********************/
#define TIM_ARR_ARR_POS           (0U)
#define TIM_ARR_ARR_MSK           (0xFFFFFFFFUL << TIM_ARR_ARR_POS)            /*!< 0xFFFFFFFF */
#define TIM_ARR_ARR               TIM_ARR_ARR_MSK                              /*!<Actual auto-reload Value */

/*******************  Bit definition for TIM_RCR register  ********************/
#define TIM_RCR_REP_POS           (0U)
#define TIM_RCR_REP_MSK           (0xFFFFUL << TIM_RCR_REP_POS)                /*!< 0x0000FFFF */
#define TIM_RCR_REP               TIM_RCR_REP_MSK                              /*!<Repetition Counter Value */

/*******************  Bit definition for TIM_CCR1 register  *******************/
#define TIM_CCR1_CCR1_POS         (0U)
#define TIM_CCR1_CCR1_MSK         (0xFFFFUL << TIM_CCR1_CCR1_POS)              /*!< 0x0000FFFF */
#define TIM_CCR1_CCR1             TIM_CCR1_CCR1_MSK                            /*!<Capture/Compare 1 Value */

/*******************  Bit definition for TIM_CCR2 register  *******************/
#define TIM_CCR2_CCR2_POS         (0U)
#define TIM_CCR2_CCR2_MSK         (0xFFFFUL << TIM_CCR2_CCR2_POS)              /*!< 0x0000FFFF */
#define TIM_CCR2_CCR2             TIM_CCR2_CCR2_MSK                            /*!<Capture/Compare 2 Value */

/*******************  Bit definition for TIM_CCR3 register  *******************/
#define TIM_CCR3_CCR3_POS         (0U)
#define TIM_CCR3_CCR3_MSK         (0xFFFFUL << TIM_CCR3_CCR3_POS)              /*!< 0x0000FFFF */
#define TIM_CCR3_CCR3             TIM_CCR3_CCR3_MSK                            /*!<Capture/Compare 3 Value */

/*******************  Bit definition for TIM_CCR4 register  *******************/
#define TIM_CCR4_CCR4_POS         (0U)
#define TIM_CCR4_CCR4_MSK         (0xFFFFUL << TIM_CCR4_CCR4_POS)              /*!< 0x0000FFFF */
#define TIM_CCR4_CCR4             TIM_CCR4_CCR4_MSK                            /*!<Capture/Compare 4 Value */

/*******************  Bit definition for TIM_CCR5 register  *******************/
#define TIM_CCR5_CCR5_POS         (0U)
#define TIM_CCR5_CCR5_MSK         (0xFFFFFFFFUL << TIM_CCR5_CCR5_POS)          /*!< 0xFFFFFFFF */
#define TIM_CCR5_CCR5             TIM_CCR5_CCR5_MSK                            /*!<Capture/Compare 5 Value */
#define TIM_CCR5_GC5C1_POS        (29U)
#define TIM_CCR5_GC5C1_MSK        (0x1UL << TIM_CCR5_GC5C1_POS)                /*!< 0x20000000 */
#define TIM_CCR5_GC5C1            TIM_CCR5_GC5C1_MSK                           /*!<Group CHANNEL 5 and CHANNEL 1 */
#define TIM_CCR5_GC5C2_POS        (30U)
#define TIM_CCR5_GC5C2_MSK        (0x1UL << TIM_CCR5_GC5C2_POS)                /*!< 0x40000000 */
#define TIM_CCR5_GC5C2            TIM_CCR5_GC5C2_MSK                           /*!<Group CHANNEL 5 and CHANNEL 2 */
#define TIM_CCR5_GC5C3_POS        (31U)
#define TIM_CCR5_GC5C3_MSK        (0x1UL << TIM_CCR5_GC5C3_POS)                /*!< 0x80000000 */
#define TIM_CCR5_GC5C3            TIM_CCR5_GC5C3_MSK                           /*!<Group CHANNEL 5 and CHANNEL 3 */

/*******************  Bit definition for TIM_CCR6 register  *******************/
#define TIM_CCR6_CCR6_POS         (0U)
#define TIM_CCR6_CCR6_MSK         (0xFFFFUL << TIM_CCR6_CCR6_POS)              /*!< 0x0000FFFF */
#define TIM_CCR6_CCR6             TIM_CCR6_CCR6_MSK                            /*!<Capture/Compare 6 Value */

/*******************  Bit definition for TIM_BDTR register  *******************/
#define TIM_BDTR_DTG_POS          (0U)
#define TIM_BDTR_DTG_MSK          (0xFFUL << TIM_BDTR_DTG_POS)                 /*!< 0x000000FF */
#define TIM_BDTR_DTG              TIM_BDTR_DTG_MSK                             /*!<DTG[0:7] bits (Dead-Time Generator set-up) */
#define TIM_BDTR_DTG_0            (0x01UL << TIM_BDTR_DTG_POS)                 /*!< 0x00000001 */
#define TIM_BDTR_DTG_1            (0x02UL << TIM_BDTR_DTG_POS)                 /*!< 0x00000002 */
#define TIM_BDTR_DTG_2            (0x04UL << TIM_BDTR_DTG_POS)                 /*!< 0x00000004 */
#define TIM_BDTR_DTG_3            (0x08UL << TIM_BDTR_DTG_POS)                 /*!< 0x00000008 */
#define TIM_BDTR_DTG_4            (0x10UL << TIM_BDTR_DTG_POS)                 /*!< 0x00000010 */
#define TIM_BDTR_DTG_5            (0x20UL << TIM_BDTR_DTG_POS)                 /*!< 0x00000020 */
#define TIM_BDTR_DTG_6            (0x40UL << TIM_BDTR_DTG_POS)                 /*!< 0x00000040 */
#define TIM_BDTR_DTG_7            (0x80UL << TIM_BDTR_DTG_POS)                 /*!< 0x00000080 */

#define TIM_BDTR_LOCK_POS         (8U)
#define TIM_BDTR_LOCK_MSK         (0x3UL << TIM_BDTR_LOCK_POS)                 /*!< 0x00000300 */
#define TIM_BDTR_LOCK             TIM_BDTR_LOCK_MSK                            /*!<LOCK[1:0] bits (Lock Configuration) */
#define TIM_BDTR_LOCK_0           (0x1UL << TIM_BDTR_LOCK_POS)                 /*!< 0x00000100 */
#define TIM_BDTR_LOCK_1           (0x2UL << TIM_BDTR_LOCK_POS)                 /*!< 0x00000200 */

#define TIM_BDTR_OSSI_POS         (10U)
#define TIM_BDTR_OSSI_MSK         (0x1UL << TIM_BDTR_OSSI_POS)                 /*!< 0x00000400 */
#define TIM_BDTR_OSSI             TIM_BDTR_OSSI_MSK                            /*!<Off-State Selection for Idle mode */
#define TIM_BDTR_OSSR_POS         (11U)
#define TIM_BDTR_OSSR_MSK         (0x1UL << TIM_BDTR_OSSR_POS)                 /*!< 0x00000800 */
#define TIM_BDTR_OSSR             TIM_BDTR_OSSR_MSK                            /*!<Off-State Selection for Run mode */
#define TIM_BDTR_BKE_POS          (12U)
#define TIM_BDTR_BKE_MSK          (0x1UL << TIM_BDTR_BKE_POS)                  /*!< 0x00001000 */
#define TIM_BDTR_BKE              TIM_BDTR_BKE_MSK                             /*!<Break enable for Break 1 */
#define TIM_BDTR_BKP_POS          (13U)
#define TIM_BDTR_BKP_MSK          (0x1UL << TIM_BDTR_BKP_POS)                  /*!< 0x00002000 */
#define TIM_BDTR_BKP              TIM_BDTR_BKP_MSK                             /*!<Break Polarity for Break 1 */
#define TIM_BDTR_AOE_POS          (14U)
#define TIM_BDTR_AOE_MSK          (0x1UL << TIM_BDTR_AOE_POS)                  /*!< 0x00004000 */
#define TIM_BDTR_AOE              TIM_BDTR_AOE_MSK                             /*!<Automatic Output enable */
#define TIM_BDTR_MOE_POS          (15U)
#define TIM_BDTR_MOE_MSK          (0x1UL << TIM_BDTR_MOE_POS)                  /*!< 0x00008000 */
#define TIM_BDTR_MOE              TIM_BDTR_MOE_MSK                             /*!<Main Output enable */

#define TIM_BDTR_BKF_POS          (16U)
#define TIM_BDTR_BKF_MSK          (0xFUL << TIM_BDTR_BKF_POS)                  /*!< 0x000F0000 */
#define TIM_BDTR_BKF              TIM_BDTR_BKF_MSK                             /*!<Break Filter for Break 1 */
#define TIM_BDTR_BK2F_POS         (20U)
#define TIM_BDTR_BK2F_MSK         (0xFUL << TIM_BDTR_BK2F_POS)                 /*!< 0x00F00000 */
#define TIM_BDTR_BK2F             TIM_BDTR_BK2F_MSK                            /*!<Break Filter for Break 2 */

#define TIM_BDTR_BK2E_POS         (24U)
#define TIM_BDTR_BK2E_MSK         (0x1UL << TIM_BDTR_BK2E_POS)                 /*!< 0x01000000 */
#define TIM_BDTR_BK2E             TIM_BDTR_BK2E_MSK                            /*!<Break enable for Break 2 */
#define TIM_BDTR_BK2P_POS         (25U)
#define TIM_BDTR_BK2P_MSK         (0x1UL << TIM_BDTR_BK2P_POS)                 /*!< 0x02000000 */
#define TIM_BDTR_BK2P             TIM_BDTR_BK2P_MSK                            /*!<Break Polarity for Break 2 */

#define TIM_BDTR_BKDSRM_POS       (26U)
#define TIM_BDTR_BKDSRM_MSK       (0x1UL << TIM_BDTR_BKDSRM_POS)               /*!< 0x04000000 */
#define TIM_BDTR_BKDSRM           TIM_BDTR_BKDSRM_MSK                          /*!<Break disarming/re-arming */
#define TIM_BDTR_BK2DSRM_POS      (27U)
#define TIM_BDTR_BK2DSRM_MSK      (0x1UL << TIM_BDTR_BK2DSRM_POS)              /*!< 0x08000000 */
#define TIM_BDTR_BK2DSRM          TIM_BDTR_BK2DSRM_MSK                         /*!<Break2 disarming/re-arming */

#define TIM_BDTR_BKBID_POS        (28U)
#define TIM_BDTR_BKBID_MSK        (0x1UL << TIM_BDTR_BKBID_POS)                /*!< 0x10000000 */
#define TIM_BDTR_BKBID            TIM_BDTR_BKBID_MSK                           /*!<Break BIDirectional */
#define TIM_BDTR_BK2BID_POS       (29U)
#define TIM_BDTR_BK2BID_MSK       (0x1UL << TIM_BDTR_BK2BID_POS)               /*!< 0x20000000 */
#define TIM_BDTR_BK2BID           TIM_BDTR_BK2BID_MSK                          /*!<Break2 BIDirectional */

/*******************  Bit definition for TIM_DCR register  ********************/
#define TIM_DCR_DBA_POS           (0U)
#define TIM_DCR_DBA_MSK           (0x1FUL << TIM_DCR_DBA_POS)                  /*!< 0x0000001F */
#define TIM_DCR_DBA               TIM_DCR_DBA_MSK                              /*!<DBA[4:0] bits (DMA Base Address) */
#define TIM_DCR_DBA_0             (0x01UL << TIM_DCR_DBA_POS)                  /*!< 0x00000001 */
#define TIM_DCR_DBA_1             (0x02UL << TIM_DCR_DBA_POS)                  /*!< 0x00000002 */
#define TIM_DCR_DBA_2             (0x04UL << TIM_DCR_DBA_POS)                  /*!< 0x00000004 */
#define TIM_DCR_DBA_3             (0x08UL << TIM_DCR_DBA_POS)                  /*!< 0x00000008 */
#define TIM_DCR_DBA_4             (0x10UL << TIM_DCR_DBA_POS)                  /*!< 0x00000010 */

#define TIM_DCR_DBL_POS           (8U)
#define TIM_DCR_DBL_MSK           (0x1FUL << TIM_DCR_DBL_POS)                  /*!< 0x00001F00 */
#define TIM_DCR_DBL               TIM_DCR_DBL_MSK                              /*!<DBL[4:0] bits (DMA Burst Length) */
#define TIM_DCR_DBL_0             (0x01UL << TIM_DCR_DBL_POS)                  /*!< 0x00000100 */
#define TIM_DCR_DBL_1             (0x02UL << TIM_DCR_DBL_POS)                  /*!< 0x00000200 */
#define TIM_DCR_DBL_2             (0x04UL << TIM_DCR_DBL_POS)                  /*!< 0x00000400 */
#define TIM_DCR_DBL_3             (0x08UL << TIM_DCR_DBL_POS)                  /*!< 0x00000800 */
#define TIM_DCR_DBL_4             (0x10UL << TIM_DCR_DBL_POS)                  /*!< 0x00001000 */

/*******************  Bit definition for TIM_DMAR register  *******************/
#define TIM_DMAR_DMAB_POS         (0U)
#define TIM_DMAR_DMAB_MSK         (0xFFFFUL << TIM_DMAR_DMAB_POS)              /*!< 0x0000FFFF */
#define TIM_DMAR_DMAB             TIM_DMAR_DMAB_MSK                            /*!<DMA register for burst accesses */

/*******************  Bit definition for TIM1_OR1 register  *******************/
#define TIM1_OR1_OCREF_CLR_POS     (0U)
#define TIM1_OR1_OCREF_CLR_MSK     (0x1UL << TIM1_OR1_OCREF_CLR_POS)           /*!< 0x00000001 */
#define TIM1_OR1_OCREF_CLR         TIM1_OR1_OCREF_CLR_MSK                      /*!<OCREF clear input selection */

/*******************  Bit definition for TIM1_AF1 register  *******************/
#define TIM1_AF1_BKINE_POS        (0U)
#define TIM1_AF1_BKINE_MSK        (0x1UL << TIM1_AF1_BKINE_POS)                /*!< 0x00000001 */
#define TIM1_AF1_BKINE            TIM1_AF1_BKINE_MSK                           /*!<BRK BKIN input enable */
#define TIM1_AF1_BKCMP1E_POS      (1U)
#define TIM1_AF1_BKCMP1E_MSK      (0x1UL << TIM1_AF1_BKCMP1E_POS)              /*!< 0x00000002 */
#define TIM1_AF1_BKCMP1E          TIM1_AF1_BKCMP1E_MSK                         /*!<BRK COMP1 enable */
#define TIM1_AF1_BKCMP2E_POS      (2U)
#define TIM1_AF1_BKCMP2E_MSK      (0x1UL << TIM1_AF1_BKCMP2E_POS)              /*!< 0x00000004 */
#define TIM1_AF1_BKCMP2E          TIM1_AF1_BKCMP2E_MSK                         /*!<BRK COMP2 enable */
#define TIM1_AF1_BKINP_POS        (9U)
#define TIM1_AF1_BKINP_MSK        (0x1UL << TIM1_AF1_BKINP_POS)                /*!< 0x00000200 */
#define TIM1_AF1_BKINP            TIM1_AF1_BKINP_MSK                           /*!<BRK BKIN input polarity */
#define TIM1_AF1_BKCMP1P_POS      (10U)
#define TIM1_AF1_BKCMP1P_MSK      (0x1UL << TIM1_AF1_BKCMP1P_POS)              /*!< 0x00000400 */
#define TIM1_AF1_BKCMP1P          TIM1_AF1_BKCMP1P_MSK                         /*!<BRK COMP1 input polarity */
#define TIM1_AF1_BKCMP2P_POS      (11U)
#define TIM1_AF1_BKCMP2P_MSK      (0x1UL << TIM1_AF1_BKCMP2P_POS)              /*!< 0x00000800 */
#define TIM1_AF1_BKCMP2P          TIM1_AF1_BKCMP2P_MSK                         /*!<BRK COMP2 input polarity */

#define TIM1_AF1_ETRSEL_POS       (14U)
#define TIM1_AF1_ETRSEL_MSK       (0xFUL << TIM1_AF1_ETRSEL_POS)               /*!< 0x0003C000 */
#define TIM1_AF1_ETRSEL           TIM1_AF1_ETRSEL_MSK                          /*!<ETRSEL[3:0] bits (TIM1 ETR source selection) */
#define TIM1_AF1_ETRSEL_0         (0x1UL << TIM1_AF1_ETRSEL_POS)               /*!< 0x00004000 */
#define TIM1_AF1_ETRSEL_1         (0x2UL << TIM1_AF1_ETRSEL_POS)               /*!< 0x00008000 */
#define TIM1_AF1_ETRSEL_2         (0x4UL << TIM1_AF1_ETRSEL_POS)               /*!< 0x00010000 */
#define TIM1_AF1_ETRSEL_3         (0x8UL << TIM1_AF1_ETRSEL_POS)               /*!< 0x00020000 */

/*******************  Bit definition for TIM1_AF2 register  *******************/
#define TIM1_AF2_BK2INE_POS       (0U)
#define TIM1_AF2_BK2INE_MSK       (0x1UL << TIM1_AF2_BK2INE_POS)               /*!< 0x00000001 */
#define TIM1_AF2_BK2INE           TIM1_AF2_BK2INE_MSK                          /*!<BRK2 BKIN2 input enable */
#define TIM1_AF2_BK2CMP1E_POS     (1U)
#define TIM1_AF2_BK2CMP1E_MSK     (0x1UL << TIM1_AF2_BK2CMP1E_POS)             /*!< 0x00000002 */
#define TIM1_AF2_BK2CMP1E         TIM1_AF2_BK2CMP1E_MSK                        /*!<BRK2 COMP1 enable */
#define TIM1_AF2_BK2CMP2E_POS     (2U)
#define TIM1_AF2_BK2CMP2E_MSK     (0x1UL << TIM1_AF2_BK2CMP2E_POS)             /*!< 0x00000004 */
#define TIM1_AF2_BK2CMP2E         TIM1_AF2_BK2CMP2E_MSK                        /*!<BRK2 COMP2 enable */
#define TIM1_AF2_BK2INP_POS       (9U)
#define TIM1_AF2_BK2INP_MSK       (0x1UL << TIM1_AF2_BK2INP_POS)               /*!< 0x00000200 */
#define TIM1_AF2_BK2INP           TIM1_AF2_BK2INP_MSK                          /*!<BRK2 BKIN2 input polarity */
#define TIM1_AF2_BK2CMP1P_POS     (10U)
#define TIM1_AF2_BK2CMP1P_MSK     (0x1UL << TIM1_AF2_BK2CMP1P_POS)             /*!< 0x00000400 */
#define TIM1_AF2_BK2CMP1P         TIM1_AF2_BK2CMP1P_MSK                        /*!<BRK2 COMP1 input polarity */
#define TIM1_AF2_BK2CMP2P_POS     (11U)
#define TIM1_AF2_BK2CMP2P_MSK     (0x1UL << TIM1_AF2_BK2CMP2P_POS)             /*!< 0x00000800 */
#define TIM1_AF2_BK2CMP2P         TIM1_AF2_BK2CMP2P_MSK                        /*!<BRK2 COMP2 input polarity */

/*******************  Bit definition for TIM2_OR1 register  *******************/
#define TIM2_OR1_OCREF_CLR_POS     (0U)
#define TIM2_OR1_OCREF_CLR_MSK     (0x1UL << TIM2_OR1_OCREF_CLR_POS)           /*!< 0x00000001 */
#define TIM2_OR1_OCREF_CLR         TIM2_OR1_OCREF_CLR_MSK                      /*!<OCREF clear input selection */

/*******************  Bit definition for TIM2_AF1 register  *******************/
#define TIM2_AF1_ETRSEL_POS       (14U)
#define TIM2_AF1_ETRSEL_MSK       (0xFUL << TIM2_AF1_ETRSEL_POS)               /*!< 0x0003C000 */
#define TIM2_AF1_ETRSEL           TIM2_AF1_ETRSEL_MSK                          /*!<ETRSEL[3:0] bits (TIM2 ETR source selection) */
#define TIM2_AF1_ETRSEL_0         (0x1UL << TIM2_AF1_ETRSEL_POS)               /*!< 0x00004000 */
#define TIM2_AF1_ETRSEL_1         (0x2UL << TIM2_AF1_ETRSEL_POS)               /*!< 0x00008000 */
#define TIM2_AF1_ETRSEL_2         (0x4UL << TIM2_AF1_ETRSEL_POS)               /*!< 0x00010000 */
#define TIM2_AF1_ETRSEL_3         (0x8UL << TIM2_AF1_ETRSEL_POS)               /*!< 0x00020000 */

/*******************  Bit definition for TIM3_OR1 register  *******************/
#define TIM3_OR1_OCREF_CLR_POS     (0U)
#define TIM3_OR1_OCREF_CLR_MSK     (0x1UL << TIM3_OR1_OCREF_CLR_POS)           /*!< 0x00000001 */
#define TIM3_OR1_OCREF_CLR         TIM3_OR1_OCREF_CLR_MSK                      /*!<OCREF clear input selection */

/*******************  Bit definition for TIM3_AF1 register  *******************/
#define TIM3_AF1_ETRSEL_POS       (14U)
#define TIM3_AF1_ETRSEL_MSK       (0xFUL << TIM3_AF1_ETRSEL_POS)               /*!< 0x0003C000 */
#define TIM3_AF1_ETRSEL           TIM3_AF1_ETRSEL_MSK                          /*!<ETRSEL[3:0] bits (TIM3 ETR source selection) */
#define TIM3_AF1_ETRSEL_0         (0x1UL << TIM3_AF1_ETRSEL_POS)               /*!< 0x00004000 */
#define TIM3_AF1_ETRSEL_1         (0x2UL << TIM3_AF1_ETRSEL_POS)               /*!< 0x00008000 */
#define TIM3_AF1_ETRSEL_2         (0x4UL << TIM3_AF1_ETRSEL_POS)               /*!< 0x00010000 */
#define TIM3_AF1_ETRSEL_3         (0x8UL << TIM3_AF1_ETRSEL_POS)               /*!< 0x00020000 */

/*******************  Bit definition for TIM14_AF1 register  *******************/
#define TIM14_AF1_ETRSEL_POS      (14U)
#define TIM14_AF1_ETRSEL_MSK      (0xFUL << TIM14_AF1_ETRSEL_POS)              /*!< 0x0003C000 */
#define TIM14_AF1_ETRSEL          TIM14_AF1_ETRSEL_MSK                         /*!<ETRSEL[3:0] bits (TIM3 ETR source selection) */
#define TIM14_AF1_ETRSEL_0        (0x1UL << TIM14_AF1_ETRSEL_POS)              /*!< 0x00004000 */
#define TIM14_AF1_ETRSEL_1        (0x2UL << TIM14_AF1_ETRSEL_POS)              /*!< 0x00008000 */
#define TIM14_AF1_ETRSEL_2        (0x4UL << TIM14_AF1_ETRSEL_POS)              /*!< 0x00010000 */
#define TIM14_AF1_ETRSEL_3        (0x8UL << TIM14_AF1_ETRSEL_POS)              /*!< 0x00020000 */

/*******************  Bit definition for TIM15_AF1 register  ******************/
#define TIM15_AF1_BKINE_POS      (0U)
#define TIM15_AF1_BKINE_MSK      (0x1UL << TIM15_AF1_BKINE_POS)                /*!< 0x00000001 */
#define TIM15_AF1_BKINE          TIM15_AF1_BKINE_MSK                           /*!<BRK BKIN input enable */
#define TIM15_AF1_BKCMP1E_POS    (1U)
#define TIM15_AF1_BKCMP1E_MSK    (0x1UL << TIM15_AF1_BKCMP1E_POS)              /*!< 0x00000002 */
#define TIM15_AF1_BKCMP1E        TIM15_AF1_BKCMP1E_MSK                         /*!<BRK COMP1 enable */
#define TIM15_AF1_BKCMP2E_POS    (2U)
#define TIM15_AF1_BKCMP2E_MSK    (0x1UL << TIM15_AF1_BKCMP2E_POS)              /*!< 0x00000004 */
#define TIM15_AF1_BKCMP2E        TIM15_AF1_BKCMP2E_MSK                         /*!<BRK COMP2 enable */
#define TIM15_AF1_BKINP_POS      (9U)
#define TIM15_AF1_BKINP_MSK      (0x1UL << TIM15_AF1_BKINP_POS)                /*!< 0x00000200 */
#define TIM15_AF1_BKINP          TIM15_AF1_BKINP_MSK                           /*!<BRK BKIN input polarity */
#define TIM15_AF1_BKCMP1P_POS    (10U)
#define TIM15_AF1_BKCMP1P_MSK    (0x1UL << TIM15_AF1_BKCMP1P_POS)              /*!< 0x00000400 */
#define TIM15_AF1_BKCMP1P        TIM15_AF1_BKCMP1P_MSK                         /*!<BRK COMP1 input polarity */
#define TIM15_AF1_BKCMP2P_POS    (11U)
#define TIM15_AF1_BKCMP2P_MSK    (0x1UL << TIM15_AF1_BKCMP2P_POS)              /*!< 0x00000800 */
#define TIM15_AF1_BKCMP2P        TIM15_AF1_BKCMP2P_MSK                         /*!<BRK COMP2 input polarity */

/*******************  Bit definition for TIM16_AF1 register  ******************/
#define TIM16_AF1_BKINE_POS      (0U)
#define TIM16_AF1_BKINE_MSK      (0x1UL << TIM16_AF1_BKINE_POS)                /*!< 0x00000001 */
#define TIM16_AF1_BKINE          TIM16_AF1_BKINE_MSK                           /*!<BRK BKIN input enable */
#define TIM16_AF1_BKCMP1E_POS    (1U)
#define TIM16_AF1_BKCMP1E_MSK    (0x1UL << TIM16_AF1_BKCMP1E_POS)              /*!< 0x00000002 */
#define TIM16_AF1_BKCMP1E        TIM16_AF1_BKCMP1E_MSK                         /*!<BRK COMP1 enable */
#define TIM16_AF1_BKCMP2E_POS    (2U)
#define TIM16_AF1_BKCMP2E_MSK    (0x1UL << TIM16_AF1_BKCMP2E_POS)              /*!< 0x00000004 */
#define TIM16_AF1_BKCMP2E        TIM16_AF1_BKCMP2E_MSK                         /*!<BRK COMP2 enable */
#define TIM16_AF1_BKINP_POS      (9U)
#define TIM16_AF1_BKINP_MSK      (0x1UL << TIM16_AF1_BKINP_POS)                /*!< 0x00000200 */
#define TIM16_AF1_BKINP          TIM16_AF1_BKINP_MSK                           /*!<BRK BKIN input polarity */
#define TIM16_AF1_BKCMP1P_POS    (10U)
#define TIM16_AF1_BKCMP1P_MSK    (0x1UL << TIM16_AF1_BKCMP1P_POS)              /*!< 0x00000400 */
#define TIM16_AF1_BKCMP1P        TIM16_AF1_BKCMP1P_MSK                         /*!<BRK COMP1 input polarity */
#define TIM16_AF1_BKCMP2P_POS    (11U)
#define TIM16_AF1_BKCMP2P_MSK    (0x1UL << TIM16_AF1_BKCMP2P_POS)              /*!< 0x00000800 */
#define TIM16_AF1_BKCMP2P        TIM16_AF1_BKCMP2P_MSK                         /*!<BRK COMP2 input polarity */

/*******************  Bit definition for TIM17_AF1 register  ******************/
#define TIM17_AF1_BKINE_POS      (0U)
#define TIM17_AF1_BKINE_MSK      (0x1UL << TIM17_AF1_BKINE_POS)                /*!< 0x00000001 */
#define TIM17_AF1_BKINE          TIM17_AF1_BKINE_MSK                           /*!<BRK BKIN input enable */
#define TIM17_AF1_BKCMP1E_POS    (1U)
#define TIM17_AF1_BKCMP1E_MSK    (0x1UL << TIM17_AF1_BKCMP1E_POS)              /*!< 0x00000002 */
#define TIM17_AF1_BKCMP1E        TIM17_AF1_BKCMP1E_MSK                         /*!<BRK COMP1 enable */
#define TIM17_AF1_BKCMP2E_POS    (2U)
#define TIM17_AF1_BKCMP2E_MSK    (0x1UL << TIM17_AF1_BKCMP2E_POS)              /*!< 0x00000004 */
#define TIM17_AF1_BKCMP2E        TIM17_AF1_BKCMP2E_MSK                         /*!<BRK COMP2 enable */
#define TIM17_AF1_BKINP_POS      (9U)
#define TIM17_AF1_BKINP_MSK      (0x1UL << TIM17_AF1_BKINP_POS)                /*!< 0x00000200 */
#define TIM17_AF1_BKINP          TIM17_AF1_BKINP_MSK                           /*!<BRK BKIN input polarity */
#define TIM17_AF1_BKCMP1P_POS    (10U)
#define TIM17_AF1_BKCMP1P_MSK    (0x1UL << TIM17_AF1_BKCMP1P_POS)              /*!< 0x00000400 */
#define TIM17_AF1_BKCMP1P        TIM17_AF1_BKCMP1P_MSK                         /*!<BRK COMP1 input polarity */
#define TIM17_AF1_BKCMP2P_POS    (11U)
#define TIM17_AF1_BKCMP2P_MSK    (0x1UL << TIM17_AF1_BKCMP2P_POS)              /*!< 0x00000800 */
#define TIM17_AF1_BKCMP2P        TIM17_AF1_BKCMP2P_MSK                         /*!<BRK COMP2 input polarity */

/*******************  Bit definition for TIM_TISEL register  *********************/
#define TIM_TISEL_TI1SEL_POS      (0U)
#define TIM_TISEL_TI1SEL_MSK      (0xFUL << TIM_TISEL_TI1SEL_POS)              /*!< 0x0000000F */
#define TIM_TISEL_TI1SEL          TIM_TISEL_TI1SEL_MSK                         /*!<TI1SEL[3:0] bits (TIM1 TI1 SEL)*/
#define TIM_TISEL_TI1SEL_0        (0x1UL << TIM_TISEL_TI1SEL_POS)              /*!< 0x00000001 */
#define TIM_TISEL_TI1SEL_1        (0x2UL << TIM_TISEL_TI1SEL_POS)              /*!< 0x00000002 */
#define TIM_TISEL_TI1SEL_2        (0x4UL << TIM_TISEL_TI1SEL_POS)              /*!< 0x00000004 */
#define TIM_TISEL_TI1SEL_3        (0x8UL << TIM_TISEL_TI1SEL_POS)              /*!< 0x00000008 */

#define TIM_TISEL_TI2SEL_POS      (8U)
#define TIM_TISEL_TI2SEL_MSK      (0xFUL << TIM_TISEL_TI2SEL_POS)              /*!< 0x00000F00 */
#define TIM_TISEL_TI2SEL          TIM_TISEL_TI2SEL_MSK                         /*!<TI2SEL[3:0] bits (TIM1 TI2 SEL)*/
#define TIM_TISEL_TI2SEL_0        (0x1UL << TIM_TISEL_TI2SEL_POS)              /*!< 0x00000100 */
#define TIM_TISEL_TI2SEL_1        (0x2UL << TIM_TISEL_TI2SEL_POS)              /*!< 0x00000200 */
#define TIM_TISEL_TI2SEL_2        (0x4UL << TIM_TISEL_TI2SEL_POS)              /*!< 0x00000400 */
#define TIM_TISEL_TI2SEL_3        (0x8UL << TIM_TISEL_TI2SEL_POS)              /*!< 0x00000800 */

#define TIM_TISEL_TI3SEL_POS      (16U)
#define TIM_TISEL_TI3SEL_MSK      (0xFUL << TIM_TISEL_TI3SEL_POS)              /*!< 0x000F0000 */
#define TIM_TISEL_TI3SEL          TIM_TISEL_TI3SEL_MSK                         /*!<TI3SEL[3:0] bits (TIM1 TI3 SEL)*/
#define TIM_TISEL_TI3SEL_0        (0x1UL << TIM_TISEL_TI3SEL_POS)              /*!< 0x00010000 */
#define TIM_TISEL_TI3SEL_1        (0x2UL << TIM_TISEL_TI3SEL_POS)              /*!< 0x00020000 */
#define TIM_TISEL_TI3SEL_2        (0x4UL << TIM_TISEL_TI3SEL_POS)              /*!< 0x00040000 */
#define TIM_TISEL_TI3SEL_3        (0x8UL << TIM_TISEL_TI3SEL_POS)              /*!< 0x00080000 */

#define TIM_TISEL_TI4SEL_POS      (24U)
#define TIM_TISEL_TI4SEL_MSK      (0xFUL << TIM_TISEL_TI4SEL_POS)              /*!< 0x0F000000 */
#define TIM_TISEL_TI4SEL          TIM_TISEL_TI4SEL_MSK                         /*!<TI4SEL[3:0] bits (TIM1 TI4 SEL)*/
#define TIM_TISEL_TI4SEL_0        (0x1UL << TIM_TISEL_TI4SEL_POS)              /*!< 0x01000000 */
#define TIM_TISEL_TI4SEL_1        (0x2UL << TIM_TISEL_TI4SEL_POS)              /*!< 0x02000000 */
#define TIM_TISEL_TI4SEL_2        (0x4UL << TIM_TISEL_TI4SEL_POS)              /*!< 0x04000000 */
#define TIM_TISEL_TI4SEL_3        (0x8UL << TIM_TISEL_TI4SEL_POS)              /*!< 0x08000000 */





//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- LPTIMER CONTROL REGISTERS ----------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief LPTIMER
  */
typedef struct
{
  volatile uint32 ISR;              /*!< LPTIM Interrupt and Status register,                Address offset: 0x00 */
  volatile uint32 ICR;              /*!< LPTIM Interrupt Clear register,                     Address offset: 0x04 */
  volatile uint32 IER;              /*!< LPTIM Interrupt Enable register,                    Address offset: 0x08 */
  volatile uint32 CFGR;             /*!< LPTIM Configuration register,                       Address offset: 0x0C */
  volatile uint32 CR;               /*!< LPTIM Control register,                             Address offset: 0x10 */
  volatile uint32 CMP;              /*!< LPTIM Compare register,                             Address offset: 0x14 */
  volatile uint32 ARR;              /*!< LPTIM Autoreload register,                          Address offset: 0x18 */
  volatile uint32 CNT;              /*!< LPTIM Counter register,                             Address offset: 0x1C */
  volatile uint32 RESERVED1;        /*!< Reserved1,                                          Address offset: 0x20 */
  volatile uint32 CFGR2;            /*!< LPTIM Option register,                              Address offset: 0x24 */
} LPTIM_DEF_TYPE;


#define LPTIM2              (*(LPTIM_DEF_TYPE *) LPTIM2_BASE)
#define LPTIM1              (*(LPTIM_DEF_TYPE *) LPTIM1_BASE)

/******************  Bit definition for LPTIM_ISR register  *******************/
#define LPTIM_ISR_CMPM_POS          (0U)
#define LPTIM_ISR_CMPM_MSK          (0x1UL << LPTIM_ISR_CMPM_POS)              /*!< 0x00000001 */
#define LPTIM_ISR_CMPM              LPTIM_ISR_CMPM_MSK                         /*!< Compare match */
#define LPTIM_ISR_ARRM_POS          (1U)
#define LPTIM_ISR_ARRM_MSK          (0x1UL << LPTIM_ISR_ARRM_POS)              /*!< 0x00000002 */
#define LPTIM_ISR_ARRM              LPTIM_ISR_ARRM_MSK                         /*!< Autoreload match */
#define LPTIM_ISR_EXTTRIG_POS       (2U)
#define LPTIM_ISR_EXTTRIG_MSK       (0x1UL << LPTIM_ISR_EXTTRIG_POS)           /*!< 0x00000004 */
#define LPTIM_ISR_EXTTRIG           LPTIM_ISR_EXTTRIG_MSK                      /*!< External trigger edge event */
#define LPTIM_ISR_CMPOK_POS         (3U)
#define LPTIM_ISR_CMPOK_MSK         (0x1UL << LPTIM_ISR_CMPOK_POS)             /*!< 0x00000008 */
#define LPTIM_ISR_CMPOK             LPTIM_ISR_CMPOK_MSK                        /*!< Compare register update OK */
#define LPTIM_ISR_ARROK_POS         (4U)
#define LPTIM_ISR_ARROK_MSK         (0x1UL << LPTIM_ISR_ARROK_POS)             /*!< 0x00000010 */
#define LPTIM_ISR_ARROK             LPTIM_ISR_ARROK_MSK                        /*!< Autoreload register update OK */
#define LPTIM_ISR_UP_POS            (5U)
#define LPTIM_ISR_UP_MSK            (0x1UL << LPTIM_ISR_UP_POS)                /*!< 0x00000020 */
#define LPTIM_ISR_UP                LPTIM_ISR_UP_MSK                           /*!< Counter direction change down to up */
#define LPTIM_ISR_DOWN_POS          (6U)
#define LPTIM_ISR_DOWN_MSK          (0x1UL << LPTIM_ISR_DOWN_POS)              /*!< 0x00000040 */
#define LPTIM_ISR_DOWN              LPTIM_ISR_DOWN_MSK                         /*!< Counter direction change up to down */

/******************  Bit definition for LPTIM_ICR register  *******************/
#define LPTIM_ICR_CMPMCF_POS        (0U)
#define LPTIM_ICR_CMPMCF_MSK        (0x1UL << LPTIM_ICR_CMPMCF_POS)            /*!< 0x00000001 */
#define LPTIM_ICR_CMPMCF            LPTIM_ICR_CMPMCF_MSK                       /*!< Compare match Clear Flag */
#define LPTIM_ICR_ARRMCF_POS        (1U)
#define LPTIM_ICR_ARRMCF_MSK        (0x1UL << LPTIM_ICR_ARRMCF_POS)            /*!< 0x00000002 */
#define LPTIM_ICR_ARRMCF            LPTIM_ICR_ARRMCF_MSK                       /*!< Autoreload match Clear Flag */
#define LPTIM_ICR_EXTTRIGCF_POS     (2U)
#define LPTIM_ICR_EXTTRIGCF_MSK     (0x1UL << LPTIM_ICR_EXTTRIGCF_POS)         /*!< 0x00000004 */
#define LPTIM_ICR_EXTTRIGCF         LPTIM_ICR_EXTTRIGCF_MSK                    /*!< External trigger edge event Clear Flag */
#define LPTIM_ICR_CMPOKCF_POS       (3U)
#define LPTIM_ICR_CMPOKCF_MSK       (0x1UL << LPTIM_ICR_CMPOKCF_POS)           /*!< 0x00000008 */
#define LPTIM_ICR_CMPOKCF           LPTIM_ICR_CMPOKCF_MSK                      /*!< Compare register update OK Clear Flag */
#define LPTIM_ICR_ARROKCF_POS       (4U)
#define LPTIM_ICR_ARROKCF_MSK       (0x1UL << LPTIM_ICR_ARROKCF_POS)           /*!< 0x00000010 */
#define LPTIM_ICR_ARROKCF           LPTIM_ICR_ARROKCF_MSK                      /*!< Autoreload register update OK Clear Flag */
#define LPTIM_ICR_UPCF_POS          (5U)
#define LPTIM_ICR_UPCF_MSK          (0x1UL << LPTIM_ICR_UPCF_POS)              /*!< 0x00000020 */
#define LPTIM_ICR_UPCF              LPTIM_ICR_UPCF_MSK                         /*!< Counter direction change down to up Clear Flag */
#define LPTIM_ICR_DOWNCF_POS        (6U)
#define LPTIM_ICR_DOWNCF_MSK        (0x1UL << LPTIM_ICR_DOWNCF_POS)            /*!< 0x00000040 */
#define LPTIM_ICR_DOWNCF            LPTIM_ICR_DOWNCF_MSK                       /*!< Counter direction change up to down Clear Flag */

/******************  Bit definition for LPTIM_IER register ********************/
#define LPTIM_IER_CMPMIE_POS        (0U)
#define LPTIM_IER_CMPMIE_MSK        (0x1UL << LPTIM_IER_CMPMIE_POS)            /*!< 0x00000001 */
#define LPTIM_IER_CMPMIE            LPTIM_IER_CMPMIE_MSK                       /*!< Compare match Interrupt Enable */
#define LPTIM_IER_ARRMIE_POS        (1U)
#define LPTIM_IER_ARRMIE_MSK        (0x1UL << LPTIM_IER_ARRMIE_POS)            /*!< 0x00000002 */
#define LPTIM_IER_ARRMIE            LPTIM_IER_ARRMIE_MSK                       /*!< Autoreload match Interrupt Enable */
#define LPTIM_IER_EXTTRIGIE_POS     (2U)
#define LPTIM_IER_EXTTRIGIE_MSK     (0x1UL << LPTIM_IER_EXTTRIGIE_POS)         /*!< 0x00000004 */
#define LPTIM_IER_EXTTRIGIE         LPTIM_IER_EXTTRIGIE_MSK                    /*!< External trigger edge event Interrupt Enable */
#define LPTIM_IER_CMPOKIE_POS       (3U)
#define LPTIM_IER_CMPOKIE_MSK       (0x1UL << LPTIM_IER_CMPOKIE_POS)           /*!< 0x00000008 */
#define LPTIM_IER_CMPOKIE           LPTIM_IER_CMPOKIE_MSK                      /*!< Compare register update OK Interrupt Enable */
#define LPTIM_IER_ARROKIE_POS       (4U)
#define LPTIM_IER_ARROKIE_MSK       (0x1UL << LPTIM_IER_ARROKIE_POS)           /*!< 0x00000010 */
#define LPTIM_IER_ARROKIE           LPTIM_IER_ARROKIE_MSK                      /*!< Autoreload register update OK Interrupt Enable */
#define LPTIM_IER_UPIE_POS          (5U)
#define LPTIM_IER_UPIE_MSK          (0x1UL << LPTIM_IER_UPIE_POS)              /*!< 0x00000020 */
#define LPTIM_IER_UPIE              LPTIM_IER_UPIE_MSK                         /*!< Counter direction change down to up Interrupt Enable */
#define LPTIM_IER_DOWNIE_POS        (6U)
#define LPTIM_IER_DOWNIE_MSK        (0x1UL << LPTIM_IER_DOWNIE_POS)            /*!< 0x00000040 */
#define LPTIM_IER_DOWNIE            LPTIM_IER_DOWNIE_MSK                       /*!< Counter direction change up to down Interrupt Enable */

/******************  Bit definition for LPTIM_CFGR register *******************/
#define LPTIM_CFGR_CKSEL_POS        (0U)
#define LPTIM_CFGR_CKSEL_MSK        (0x1UL << LPTIM_CFGR_CKSEL_POS)            /*!< 0x00000001 */
#define LPTIM_CFGR_CKSEL            LPTIM_CFGR_CKSEL_MSK                       /*!< Clock selector */

#define LPTIM_CFGR_CKPOL_POS        (1U)
#define LPTIM_CFGR_CKPOL_MSK        (0x3UL << LPTIM_CFGR_CKPOL_POS)            /*!< 0x00000006 */
#define LPTIM_CFGR_CKPOL            LPTIM_CFGR_CKPOL_MSK                       /*!< CKPOL[1:0] bits (Clock polarity) */
#define LPTIM_CFGR_CKPOL_0          (0x1UL << LPTIM_CFGR_CKPOL_POS)            /*!< 0x00000002 */
#define LPTIM_CFGR_CKPOL_1          (0x2UL << LPTIM_CFGR_CKPOL_POS)            /*!< 0x00000004 */

#define LPTIM_CFGR_CKFLT_POS        (3U)
#define LPTIM_CFGR_CKFLT_MSK        (0x3UL << LPTIM_CFGR_CKFLT_POS)            /*!< 0x00000018 */
#define LPTIM_CFGR_CKFLT            LPTIM_CFGR_CKFLT_MSK                       /*!< CKFLT[1:0] bits (Configurable digital filter for external clock) */
#define LPTIM_CFGR_CKFLT_0          (0x1UL << LPTIM_CFGR_CKFLT_POS)            /*!< 0x00000008 */
#define LPTIM_CFGR_CKFLT_1          (0x2UL << LPTIM_CFGR_CKFLT_POS)            /*!< 0x00000010 */

#define LPTIM_CFGR_TRGFLT_POS       (6U)
#define LPTIM_CFGR_TRGFLT_MSK       (0x3UL << LPTIM_CFGR_TRGFLT_POS)           /*!< 0x000000C0 */
#define LPTIM_CFGR_TRGFLT           LPTIM_CFGR_TRGFLT_MSK                      /*!< TRGFLT[1:0] bits (Configurable digital filter for trigger) */
#define LPTIM_CFGR_TRGFLT_0         (0x1UL << LPTIM_CFGR_TRGFLT_POS)           /*!< 0x00000040 */
#define LPTIM_CFGR_TRGFLT_1         (0x2UL << LPTIM_CFGR_TRGFLT_POS)           /*!< 0x00000080 */

#define LPTIM_CFGR_PRESC_POS        (9U)
#define LPTIM_CFGR_PRESC_MSK        (0x7UL << LPTIM_CFGR_PRESC_POS)            /*!< 0x00000E00 */
#define LPTIM_CFGR_PRESC            LPTIM_CFGR_PRESC_MSK                       /*!< PRESC[2:0] bits (Clock prescaler) */
#define LPTIM_CFGR_PRESC_0          (0x1UL << LPTIM_CFGR_PRESC_POS)            /*!< 0x00000200 */
#define LPTIM_CFGR_PRESC_1          (0x2UL << LPTIM_CFGR_PRESC_POS)            /*!< 0x00000400 */
#define LPTIM_CFGR_PRESC_2          (0x4UL << LPTIM_CFGR_PRESC_POS)            /*!< 0x00000800 */

#define LPTIM_CFGR_TRIGSEL_POS      (13U)
#define LPTIM_CFGR_TRIGSEL_MSK      (0x7UL << LPTIM_CFGR_TRIGSEL_POS)          /*!< 0x0000E000 */
#define LPTIM_CFGR_TRIGSEL          LPTIM_CFGR_TRIGSEL_MSK                     /*!< TRIGSEL[2:0]] bits (Trigger selector) */
#define LPTIM_CFGR_TRIGSEL_0        (0x1UL << LPTIM_CFGR_TRIGSEL_POS)          /*!< 0x00002000 */
#define LPTIM_CFGR_TRIGSEL_1        (0x2UL << LPTIM_CFGR_TRIGSEL_POS)          /*!< 0x00004000 */
#define LPTIM_CFGR_TRIGSEL_2        (0x4UL << LPTIM_CFGR_TRIGSEL_POS)          /*!< 0x00008000 */

#define LPTIM_CFGR_TRIGEN_POS       (17U)
#define LPTIM_CFGR_TRIGEN_MSK       (0x3UL << LPTIM_CFGR_TRIGEN_POS)           /*!< 0x00060000 */
#define LPTIM_CFGR_TRIGEN           LPTIM_CFGR_TRIGEN_MSK                      /*!< TRIGEN[1:0] bits (Trigger enable and polarity) */
#define LPTIM_CFGR_TRIGEN_0         (0x1UL << LPTIM_CFGR_TRIGEN_POS)           /*!< 0x00020000 */
#define LPTIM_CFGR_TRIGEN_1         (0x2UL << LPTIM_CFGR_TRIGEN_POS)           /*!< 0x00040000 */

#define LPTIM_CFGR_TIMOUT_POS       (19U)
#define LPTIM_CFGR_TIMOUT_MSK       (0x1UL << LPTIM_CFGR_TIMOUT_POS)           /*!< 0x00080000 */
#define LPTIM_CFGR_TIMOUT           LPTIM_CFGR_TIMOUT_MSK                      /*!< Timout enable */
#define LPTIM_CFGR_WAVE_POS         (20U)
#define LPTIM_CFGR_WAVE_MSK         (0x1UL << LPTIM_CFGR_WAVE_POS)             /*!< 0x00100000 */
#define LPTIM_CFGR_WAVE             LPTIM_CFGR_WAVE_MSK                        /*!< Waveform shape */
#define LPTIM_CFGR_WAVPOL_POS       (21U)
#define LPTIM_CFGR_WAVPOL_MSK       (0x1UL << LPTIM_CFGR_WAVPOL_POS)           /*!< 0x00200000 */
#define LPTIM_CFGR_WAVPOL           LPTIM_CFGR_WAVPOL_MSK                      /*!< Waveform shape polarity */
#define LPTIM_CFGR_PRELOAD_POS      (22U)
#define LPTIM_CFGR_PRELOAD_MSK      (0x1UL << LPTIM_CFGR_PRELOAD_POS)          /*!< 0x00400000 */
#define LPTIM_CFGR_PRELOAD          LPTIM_CFGR_PRELOAD_MSK                     /*!< Reg update mode */
#define LPTIM_CFGR_COUNTMODE_POS    (23U)
#define LPTIM_CFGR_COUNTMODE_MSK    (0x1UL << LPTIM_CFGR_COUNTMODE_POS)        /*!< 0x00800000 */
#define LPTIM_CFGR_COUNTMODE        LPTIM_CFGR_COUNTMODE_MSK                   /*!< Counter mode enable */
#define LPTIM_CFGR_ENC_POS          (24U)
#define LPTIM_CFGR_ENC_MSK          (0x1UL << LPTIM_CFGR_ENC_POS)              /*!< 0x01000000 */
#define LPTIM_CFGR_ENC              LPTIM_CFGR_ENC_MSK                         /*!< Encoder mode enable */

/******************  Bit definition for LPTIM_CR register  ********************/
#define LPTIM_CR_ENABLE_POS         (0U)
#define LPTIM_CR_ENABLE_MSK         (0x1UL << LPTIM_CR_ENABLE_POS)             /*!< 0x00000001 */
#define LPTIM_CR_ENABLE             LPTIM_CR_ENABLE_MSK                        /*!< LPTIMer enable */
#define LPTIM_CR_SNGSTRT_POS        (1U)
#define LPTIM_CR_SNGSTRT_MSK        (0x1UL << LPTIM_CR_SNGSTRT_POS)            /*!< 0x00000002 */
#define LPTIM_CR_SNGSTRT            LPTIM_CR_SNGSTRT_MSK                       /*!< Timer start in single mode */
#define LPTIM_CR_CNTSTRT_POS        (2U)
#define LPTIM_CR_CNTSTRT_MSK        (0x1UL << LPTIM_CR_CNTSTRT_POS)            /*!< 0x00000004 */
#define LPTIM_CR_CNTSTRT            LPTIM_CR_CNTSTRT_MSK                       /*!< Timer start in continuous mode */
#define LPTIM_CR_COUNTRST_POS       (3U)
#define LPTIM_CR_COUNTRST_MSK       (0x1UL << LPTIM_CR_COUNTRST_POS)           /*!< 0x00000008 */
#define LPTIM_CR_COUNTRST           LPTIM_CR_COUNTRST_MSK                      /*!< Counter reset */
#define LPTIM_CR_RSTARE_POS         (4U)
#define LPTIM_CR_RSTARE_MSK         (0x1UL << LPTIM_CR_RSTARE_POS)             /*!< 0x00000010 */
#define LPTIM_CR_RSTARE             LPTIM_CR_RSTARE_MSK                        /*!< Reset after read enable */

/******************  Bit definition for LPTIM_CMP register  *******************/
#define LPTIM_CMP_CMP_POS           (0U)
#define LPTIM_CMP_CMP_MSK           (0xFFFFUL << LPTIM_CMP_CMP_POS)            /*!< 0x0000FFFF */
#define LPTIM_CMP_CMP               LPTIM_CMP_CMP_MSK                          /*!< Compare register */

/******************  Bit definition for LPTIM_ARR register  *******************/
#define LPTIM_ARR_ARR_POS           (0U)
#define LPTIM_ARR_ARR_MSK           (0xFFFFUL << LPTIM_ARR_ARR_POS)            /*!< 0x0000FFFF */
#define LPTIM_ARR_ARR               LPTIM_ARR_ARR_MSK                          /*!< Auto reload register */

/******************  Bit definition for LPTIM_CNT register  *******************/
#define LPTIM_CNT_CNT_POS           (0U)
#define LPTIM_CNT_CNT_MSK           (0xFFFFUL << LPTIM_CNT_CNT_POS)            /*!< 0x0000FFFF */
#define LPTIM_CNT_CNT               LPTIM_CNT_CNT_MSK                          /*!< Counter register */

/******************  Bit definition for LPTIM_CFGR2 register  *******************/
#define LPTIM_CFGR2_IN1SEL_POS      (0U)
#define LPTIM_CFGR2_IN1SEL_MSK      (0xFUL << LPTIM_CFGR2_IN1SEL_POS)          /*!< 0x0000000F */
#define LPTIM_CFGR2_IN1SEL          LPTIM_CFGR2_IN1SEL_MSK                     /*!< CFGR2[3:0] bits (INPUT1 selection) */
#define LPTIM_CFGR2_IN1SEL_0        (0x1UL << LPTIM_CFGR2_IN1SEL_POS)          /*!< 0x00000001 */
#define LPTIM_CFGR2_IN1SEL_1        (0x2UL << LPTIM_CFGR2_IN1SEL_POS)          /*!< 0x00000002 */
#define LPTIM_CFGR2_IN1SEL_2        (0x4UL << LPTIM_CFGR2_IN1SEL_POS)          /*!< 0x00000004 */
#define LPTIM_CFGR2_IN1SEL_3        (0x8UL << LPTIM_CFGR2_IN1SEL_POS)          /*!< 0x00000008 */

#define LPTIM_CFGR2_IN2SEL_POS      (4U)
#define LPTIM_CFGR2_IN2SEL_MSK      (0xFUL << LPTIM_CFGR2_IN2SEL_POS)          /*!< 0x000000F0 */
#define LPTIM_CFGR2_IN2SEL          LPTIM_CFGR2_IN2SEL_MSK                     /*!< CFGR2[7:4] bits (INPUT2 selection) */
#define LPTIM_CFGR2_IN2SEL_0        (0x1UL << LPTIM_CFGR2_IN2SEL_POS)          /*!< 0x00000010 */
#define LPTIM_CFGR2_IN2SEL_1        (0x2UL << LPTIM_CFGR2_IN2SEL_POS)          /*!< 0x00000020 */
#define LPTIM_CFGR2_IN2SEL_2        (0x4UL << LPTIM_CFGR2_IN2SEL_POS)          /*!< 0x00000040 */
#define LPTIM_CFGR2_IN2SEL_3        (0x8UL << LPTIM_CFGR2_IN2SEL_POS)          /*!< 0x00000080 */





//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- RTC REGISTERS ----------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief Real-Time Clock
  */
typedef struct
{
  volatile uint32 TR;          /*!< RTC time register,                                         Address offset: 0x00 */
  volatile uint32 DR;          /*!< RTC date register,                                         Address offset: 0x04 */
  volatile uint32 SSR;         /*!< RTC sub second register,                                   Address offset: 0x08 */
  volatile uint32 ICSR;        /*!< RTC initialization control and status register,            Address offset: 0x0C */
  volatile uint32 PRER;        /*!< RTC prescaler register,                                    Address offset: 0x10 */
  volatile uint32 WUTR;        /*!< RTC wakeup timer register,                                 Address offset: 0x14 */
  volatile uint32 CR;          /*!< RTC control register,                                      Address offset: 0x18 */
       uint32 RESERVED0;   /*!< Reserved                                                   Address offset: 0x1C */
       uint32 RESERVED1;   /*!< Reserved                                                   Address offset: 0x20 */
  volatile uint32 WPR;         /*!< RTC write protection register,                             Address offset: 0x24 */
  volatile uint32 CALR;        /*!< RTC calibration register,                                  Address offset: 0x28 */
  volatile uint32 SHIFTR;      /*!< RTC shift control register,                                Address offset: 0x2C */
  volatile uint32 TSTR;        /*!< RTC time stamp time register,                              Address offset: 0x30 */
  volatile uint32 TSDR;        /*!< RTC time stamp date register,                              Address offset: 0x34 */
  volatile uint32 TSSSR;       /*!< RTC time-stamp sub second register,                        Address offset: 0x38 */
       uint32 RESERVED2;   /*!< Reserved                                                   Address offset: 0x1C */
  volatile uint32 ALRMAR;      /*!< RTC alarm A register,                                      Address offset: 0x40 */
  volatile uint32 ALRMASSR;    /*!< RTC alarm A sub second register,                           Address offset: 0x44 */
  volatile uint32 ALRMBR;      /*!< RTC alarm B register,                                      Address offset: 0x48 */
  volatile uint32 ALRMBSSR;    /*!< RTC alarm B sub second register,                           Address offset: 0x4C */
  volatile uint32 SR;          /*!< RTC Status register,                                       Address offset: 0x50 */
  volatile uint32 MISR;        /*!< RTC Masked Interrupt Status register,                      Address offset: 0x54 */
       uint32 RESERVED3;   /*!< Reserved                                                   Address offset: 0x58 */
  volatile uint32 SCR;         /*!< RTC Status Clear register,                                 Address offset: 0x5C */
  volatile uint32 OR;          /*!< RTC option register,                                       Address offset: 0x60 */
} RTC_TYPEDEF;

#define RTC                 (*(RTC_TYPEDEF *) RTC_BASE)

/********************  Bits definition for RTC_TR register  *******************/
#define RTC_TR_PM_POS                (22U)
#define RTC_TR_PM_MSK                (0x1UL << RTC_TR_PM_POS)                   /*!< 0x00400000 */
#define RTC_TR_PM                    RTC_TR_PM_MSK
#define RTC_TR_HT_POS                (20U)
#define RTC_TR_HT_MSK                (0x3UL << RTC_TR_HT_POS)                   /*!< 0x00300000 */
#define RTC_TR_HT                    RTC_TR_HT_MSK
#define RTC_TR_HT_0                  (0x1UL << RTC_TR_HT_POS)                   /*!< 0x00100000 */
#define RTC_TR_HT_1                  (0x2UL << RTC_TR_HT_POS)                   /*!< 0x00200000 */
#define RTC_TR_HU_POS                (16U)
#define RTC_TR_HU_MSK                (0xFUL << RTC_TR_HU_POS)                   /*!< 0x000F0000 */
#define RTC_TR_HU                    RTC_TR_HU_MSK
#define RTC_TR_HU_0                  (0x1UL << RTC_TR_HU_POS)                   /*!< 0x00010000 */
#define RTC_TR_HU_1                  (0x2UL << RTC_TR_HU_POS)                   /*!< 0x00020000 */
#define RTC_TR_HU_2                  (0x4UL << RTC_TR_HU_POS)                   /*!< 0x00040000 */
#define RTC_TR_HU_3                  (0x8UL << RTC_TR_HU_POS)                   /*!< 0x00080000 */
#define RTC_TR_MNT_POS               (12U)
#define RTC_TR_MNT_MSK               (0x7UL << RTC_TR_MNT_POS)                  /*!< 0x00007000 */
#define RTC_TR_MNT                   RTC_TR_MNT_MSK
#define RTC_TR_MNT_0                 (0x1UL << RTC_TR_MNT_POS)                  /*!< 0x00001000 */
#define RTC_TR_MNT_1                 (0x2UL << RTC_TR_MNT_POS)                  /*!< 0x00002000 */
#define RTC_TR_MNT_2                 (0x4UL << RTC_TR_MNT_POS)                  /*!< 0x00004000 */
#define RTC_TR_MNU_POS               (8U)
#define RTC_TR_MNU_MSK               (0xFUL << RTC_TR_MNU_POS)                  /*!< 0x00000F00 */
#define RTC_TR_MNU                   RTC_TR_MNU_MSK
#define RTC_TR_MNU_0                 (0x1UL << RTC_TR_MNU_POS)                  /*!< 0x00000100 */
#define RTC_TR_MNU_1                 (0x2UL << RTC_TR_MNU_POS)                  /*!< 0x00000200 */
#define RTC_TR_MNU_2                 (0x4UL << RTC_TR_MNU_POS)                  /*!< 0x00000400 */
#define RTC_TR_MNU_3                 (0x8UL << RTC_TR_MNU_POS)                  /*!< 0x00000800 */
#define RTC_TR_ST_POS                (4U)
#define RTC_TR_ST_MSK                (0x7UL << RTC_TR_ST_POS)                   /*!< 0x00000070 */
#define RTC_TR_ST                    RTC_TR_ST_MSK
#define RTC_TR_ST_0                  (0x1UL << RTC_TR_ST_POS)                   /*!< 0x00000010 */
#define RTC_TR_ST_1                  (0x2UL << RTC_TR_ST_POS)                   /*!< 0x00000020 */
#define RTC_TR_ST_2                  (0x4UL << RTC_TR_ST_POS)                   /*!< 0x00000040 */
#define RTC_TR_SU_POS                (0U)
#define RTC_TR_SU_MSK                (0xFUL << RTC_TR_SU_POS)                   /*!< 0x0000000F */
#define RTC_TR_SU                    RTC_TR_SU_MSK
#define RTC_TR_SU_0                  (0x1UL << RTC_TR_SU_POS)                   /*!< 0x00000001 */
#define RTC_TR_SU_1                  (0x2UL << RTC_TR_SU_POS)                   /*!< 0x00000002 */
#define RTC_TR_SU_2                  (0x4UL << RTC_TR_SU_POS)                   /*!< 0x00000004 */
#define RTC_TR_SU_3                  (0x8UL << RTC_TR_SU_POS)                   /*!< 0x00000008 */

/********************  Bits definition for RTC_DR register  *******************/
#define RTC_DR_YT_POS                (20U)
#define RTC_DR_YT_MSK                (0xFUL << RTC_DR_YT_POS)                   /*!< 0x00F00000 */
#define RTC_DR_YT                    RTC_DR_YT_MSK
#define RTC_DR_YT_0                  (0x1UL << RTC_DR_YT_POS)                   /*!< 0x00100000 */
#define RTC_DR_YT_1                  (0x2UL << RTC_DR_YT_POS)                   /*!< 0x00200000 */
#define RTC_DR_YT_2                  (0x4UL << RTC_DR_YT_POS)                   /*!< 0x00400000 */
#define RTC_DR_YT_3                  (0x8UL << RTC_DR_YT_POS)                   /*!< 0x00800000 */
#define RTC_DR_YU_POS                (16U)
#define RTC_DR_YU_MSK                (0xFUL << RTC_DR_YU_POS)                   /*!< 0x000F0000 */
#define RTC_DR_YU                    RTC_DR_YU_MSK
#define RTC_DR_YU_0                  (0x1UL << RTC_DR_YU_POS)                   /*!< 0x00010000 */
#define RTC_DR_YU_1                  (0x2UL << RTC_DR_YU_POS)                   /*!< 0x00020000 */
#define RTC_DR_YU_2                  (0x4UL << RTC_DR_YU_POS)                   /*!< 0x00040000 */
#define RTC_DR_YU_3                  (0x8UL << RTC_DR_YU_POS)                   /*!< 0x00080000 */
#define RTC_DR_WDU_POS               (13U)
#define RTC_DR_WDU_MSK               (0x7UL << RTC_DR_WDU_POS)                  /*!< 0x0000E000 */
#define RTC_DR_WDU                   RTC_DR_WDU_MSK
#define RTC_DR_WDU_0                 (0x1UL << RTC_DR_WDU_POS)                  /*!< 0x00002000 */
#define RTC_DR_WDU_1                 (0x2UL << RTC_DR_WDU_POS)                  /*!< 0x00004000 */
#define RTC_DR_WDU_2                 (0x4UL << RTC_DR_WDU_POS)                  /*!< 0x00008000 */
#define RTC_DR_MT_POS                (12U)
#define RTC_DR_MT_MSK                (0x1UL << RTC_DR_MT_POS)                   /*!< 0x00001000 */
#define RTC_DR_MT                    RTC_DR_MT_MSK
#define RTC_DR_MU_POS                (8U)
#define RTC_DR_MU_MSK                (0xFUL << RTC_DR_MU_POS)                   /*!< 0x00000F00 */
#define RTC_DR_MU                    RTC_DR_MU_MSK
#define RTC_DR_MU_0                  (0x1UL << RTC_DR_MU_POS)                   /*!< 0x00000100 */
#define RTC_DR_MU_1                  (0x2UL << RTC_DR_MU_POS)                   /*!< 0x00000200 */
#define RTC_DR_MU_2                  (0x4UL << RTC_DR_MU_POS)                   /*!< 0x00000400 */
#define RTC_DR_MU_3                  (0x8UL << RTC_DR_MU_POS)                   /*!< 0x00000800 */
#define RTC_DR_DT_POS                (4U)
#define RTC_DR_DT_MSK                (0x3UL << RTC_DR_DT_POS)                   /*!< 0x00000030 */
#define RTC_DR_DT                    RTC_DR_DT_MSK
#define RTC_DR_DT_0                  (0x1UL << RTC_DR_DT_POS)                   /*!< 0x00000010 */
#define RTC_DR_DT_1                  (0x2UL << RTC_DR_DT_POS)                   /*!< 0x00000020 */
#define RTC_DR_DU_POS                (0U)
#define RTC_DR_DU_MSK                (0xFUL << RTC_DR_DU_POS)                   /*!< 0x0000000F */
#define RTC_DR_DU                    RTC_DR_DU_MSK
#define RTC_DR_DU_0                  (0x1UL << RTC_DR_DU_POS)                   /*!< 0x00000001 */
#define RTC_DR_DU_1                  (0x2UL << RTC_DR_DU_POS)                   /*!< 0x00000002 */
#define RTC_DR_DU_2                  (0x4UL << RTC_DR_DU_POS)                   /*!< 0x00000004 */
#define RTC_DR_DU_3                  (0x8UL << RTC_DR_DU_POS)                   /*!< 0x00000008 */

/********************  Bits definition for RTC_SSR register  ******************/
#define RTC_SSR_SS_POS               (0U)
#define RTC_SSR_SS_MSK               (0xFFFFUL << RTC_SSR_SS_POS)               /*!< 0x0000FFFF */
#define RTC_SSR_SS                   RTC_SSR_SS_MSK

/********************  Bits definition for RTC_ICSR register  ******************/
#define RTC_ICSR_RECALPF_POS         (16U)
#define RTC_ICSR_RECALPF_MSK         (0x1UL << RTC_ICSR_RECALPF_POS)            /*!< 0x00010000 */
#define RTC_ICSR_RECALPF             RTC_ICSR_RECALPF_MSK
#define RTC_ICSR_INIT_POS            (7U)
#define RTC_ICSR_INIT_MSK            (0x1UL << RTC_ICSR_INIT_POS)               /*!< 0x00000080 */
#define RTC_ICSR_INIT                RTC_ICSR_INIT_MSK
#define RTC_ICSR_INITF_POS           (6U)
#define RTC_ICSR_INITF_MSK           (0x1UL << RTC_ICSR_INITF_POS)              /*!< 0x00000040 */
#define RTC_ICSR_INITF               RTC_ICSR_INITF_MSK
#define RTC_ICSR_RSF_POS             (5U)
#define RTC_ICSR_RSF_MSK             (0x1UL << RTC_ICSR_RSF_POS)                /*!< 0x00000020 */
#define RTC_ICSR_RSF                 RTC_ICSR_RSF_MSK
#define RTC_ICSR_INITS_POS           (4U)
#define RTC_ICSR_INITS_MSK           (0x1UL << RTC_ICSR_INITS_POS)              /*!< 0x00000010 */
#define RTC_ICSR_INITS               RTC_ICSR_INITS_MSK
#define RTC_ICSR_SHPF_POS            (3U)
#define RTC_ICSR_SHPF_MSK            (0x1UL << RTC_ICSR_SHPF_POS)               /*!< 0x00000008 */
#define RTC_ICSR_SHPF                RTC_ICSR_SHPF_MSK
#define RTC_ICSR_WUTWF_POS           (2U)
#define RTC_ICSR_WUTWF_MSK           (0x1UL << RTC_ICSR_WUTWF_POS)              /*!< 0x00000004 */
#define RTC_ICSR_WUTWF               RTC_ICSR_WUTWF_MSK                         /*!< Wakeup timer write flag > */
#define RTC_ICSR_ALRBWF_POS          (1U)
#define RTC_ICSR_ALRBWF_MSK          (0x1UL << RTC_ICSR_ALRBWF_POS)             /*!< 0x00000002 */
#define RTC_ICSR_ALRBWF              RTC_ICSR_ALRBWF_MSK
#define RTC_ICSR_ALRAWF_POS          (0U)
#define RTC_ICSR_ALRAWF_MSK          (0x1UL << RTC_ICSR_ALRAWF_POS)             /*!< 0x00000001 */
#define RTC_ICSR_ALRAWF              RTC_ICSR_ALRAWF_MSK

/********************  Bits definition for RTC_PRER register  *****************/
#define RTC_PRER_PREDIV_A_POS        (16U)
#define RTC_PRER_PREDIV_A_MSK        (0x7FUL << RTC_PRER_PREDIV_A_POS)          /*!< 0x007F0000 */
#define RTC_PRER_PREDIV_A            RTC_PRER_PREDIV_A_MSK
#define RTC_PRER_PREDIV_S_POS        (0U)
#define RTC_PRER_PREDIV_S_MSK        (0x7FFFUL << RTC_PRER_PREDIV_S_POS)        /*!< 0x00007FFF */
#define RTC_PRER_PREDIV_S            RTC_PRER_PREDIV_S_MSK

/********************  Bits definition for RTC_WUTR register  *****************/
#define RTC_WUTR_WUT_POS             (0U)
#define RTC_WUTR_WUT_MSK             (0xFFFFUL << RTC_WUTR_WUT_POS)            /*!< 0x0000FFFF */
#define RTC_WUTR_WUT                 RTC_WUTR_WUT_MSK                          /*!< Wakeup auto-reload value bits > */

/********************  Bits definition for RTC_CR register  *******************/
#define RTC_CR_OUT2EN_POS            (31U)
#define RTC_CR_OUT2EN_MSK            (0x1UL << RTC_CR_OUT2EN_POS)              /*!< 0x80000000 */
#define RTC_CR_OUT2EN                RTC_CR_OUT2EN_MSK                         /*!< RTC_OUT2 output enable */
#define RTC_CR_TAMPALRM_TYPE_POS     (30U)
#define RTC_CR_TAMPALRM_TYPE_MSK     (0x1UL << RTC_CR_TAMPALRM_TYPE_POS)       /*!< 0x40000000 */
#define RTC_CR_TAMPALRM_TYPE         RTC_CR_TAMPALRM_TYPE_MSK                  /*!< TAMPALARM output type  */
#define RTC_CR_TAMPALRM_PU_POS       (29U)
#define RTC_CR_TAMPALRM_PU_MSK       (0x1UL << RTC_CR_TAMPALRM_PU_POS)         /*!< 0x20000000 */
#define RTC_CR_TAMPALRM_PU           RTC_CR_TAMPALRM_PU_MSK                    /*!< TAMPALARM output pull-up config */
#define RTC_CR_TAMPOE_POS            (26U)
#define RTC_CR_TAMPOE_MSK            (0x1UL << RTC_CR_TAMPOE_POS)              /*!< 0x04000000 */
#define RTC_CR_TAMPOE                RTC_CR_TAMPOE_MSK                         /*!< Tamper detection output enable on TAMPALARM  */
#define RTC_CR_TAMPTS_POS            (25U)
#define RTC_CR_TAMPTS_MSK            (0x1UL << RTC_CR_TAMPTS_POS)              /*!< 0x02000000 */
#define RTC_CR_TAMPTS                RTC_CR_TAMPTS_MSK                         /*!< Activate timestamp on tamper detection event  */
#define RTC_CR_ITSE_POS              (24U)
#define RTC_CR_ITSE_MSK              (0x1UL << RTC_CR_ITSE_POS)                /*!< 0x01000000 */
#define RTC_CR_ITSE                  RTC_CR_ITSE_MSK                           /*!< Timestamp on internal event enable  */
#define RTC_CR_COE_POS               (23U)
#define RTC_CR_COE_MSK               (0x1UL << RTC_CR_COE_POS)                 /*!< 0x00800000 */
#define RTC_CR_COE                   RTC_CR_COE_MSK
#define RTC_CR_OSEL_POS              (21U)
#define RTC_CR_OSEL_MSK              (0x3UL << RTC_CR_OSEL_POS)                 /*!< 0x00600000 */
#define RTC_CR_OSEL                  RTC_CR_OSEL_MSK
#define RTC_CR_OSEL_0                (0x1UL << RTC_CR_OSEL_POS)                 /*!< 0x00200000 */
#define RTC_CR_OSEL_1                (0x2UL << RTC_CR_OSEL_POS)                 /*!< 0x00400000 */
#define RTC_CR_POL_POS               (20U)
#define RTC_CR_POL_MSK               (0x1UL << RTC_CR_POL_POS)                  /*!< 0x00100000 */
#define RTC_CR_POL                   RTC_CR_POL_MSK
#define RTC_CR_COSEL_POS             (19U)
#define RTC_CR_COSEL_MSK             (0x1UL << RTC_CR_COSEL_POS)                /*!< 0x00080000 */
#define RTC_CR_COSEL                 RTC_CR_COSEL_MSK
#define RTC_CR_BKP_POS               (18U)
#define RTC_CR_BKP_MSK               (0x1UL << RTC_CR_BKP_POS)                  /*!< 0x00040000 */
#define RTC_CR_BKP                   RTC_CR_BKP_MSK
#define RTC_CR_SUB1H_POS             (17U)
#define RTC_CR_SUB1H_MSK             (0x1UL << RTC_CR_SUB1H_POS)                /*!< 0x00020000 */
#define RTC_CR_SUB1H                 RTC_CR_SUB1H_MSK
#define RTC_CR_ADD1H_POS             (16U)
#define RTC_CR_ADD1H_MSK             (0x1UL << RTC_CR_ADD1H_POS)                /*!< 0x00010000 */
#define RTC_CR_ADD1H                 RTC_CR_ADD1H_MSK
#define RTC_CR_TSIE_POS              (15U)
#define RTC_CR_TSIE_MSK              (0x1UL << RTC_CR_TSIE_POS)                /*!< 0x00008000 */
#define RTC_CR_TSIE                  RTC_CR_TSIE_MSK                           /*!< Timestamp interrupt enable > */
#define RTC_CR_WUTIE_POS             (14U)
#define RTC_CR_WUTIE_MSK             (0x1UL << RTC_CR_WUTIE_POS)               /*!< 0x00004000 */
#define RTC_CR_WUTIE                 RTC_CR_WUTIE_MSK                          /*!< Wakeup timer interrupt enable > */
#define RTC_CR_ALRBIE_POS            (13U)
#define RTC_CR_ALRBIE_MSK            (0x1UL << RTC_CR_ALRBIE_POS)              /*!< 0x00002000 */
#define RTC_CR_ALRBIE                RTC_CR_ALRBIE_MSK
#define RTC_CR_ALRAIE_POS            (12U)
#define RTC_CR_ALRAIE_MSK            (0x1UL << RTC_CR_ALRAIE_POS)              /*!< 0x00001000 */
#define RTC_CR_ALRAIE                RTC_CR_ALRAIE_MSK
#define RTC_CR_TSE_POS               (11U)
#define RTC_CR_TSE_MSK               (0x1UL << RTC_CR_TSE_POS)                 /*!< 0x00000800 */
#define RTC_CR_TSE                   RTC_CR_TSE_MSK                            /*!< timestamp enable > */
#define RTC_CR_WUTE_POS              (10U)
#define RTC_CR_WUTE_MSK              (0x1UL << RTC_CR_WUTE_POS)                /*!< 0x00000400 */
#define RTC_CR_WUTE                  RTC_CR_WUTE_MSK                           /*!< Wakeup timer enable > */
#define RTC_CR_ALRBE_POS             (9U)
#define RTC_CR_ALRBE_MSK             (0x1UL << RTC_CR_ALRBE_POS)               /*!< 0x00000200 */
#define RTC_CR_ALRBE                 RTC_CR_ALRBE_MSK
#define RTC_CR_ALRAE_POS             (8U)
#define RTC_CR_ALRAE_MSK             (0x1UL << RTC_CR_ALRAE_POS)                /*!< 0x00000100 */
#define RTC_CR_ALRAE                 RTC_CR_ALRAE_MSK
#define RTC_CR_FMT_POS               (6U)
#define RTC_CR_FMT_MSK               (0x1UL << RTC_CR_FMT_POS)                  /*!< 0x00000040 */
#define RTC_CR_FMT                   RTC_CR_FMT_MSK
#define RTC_CR_BYPSHAD_POS           (5U)
#define RTC_CR_BYPSHAD_MSK           (0x1UL << RTC_CR_BYPSHAD_POS)              /*!< 0x00000020 */
#define RTC_CR_BYPSHAD               RTC_CR_BYPSHAD_MSK
#define RTC_CR_REFCKON_POS           (4U)
#define RTC_CR_REFCKON_MSK           (0x1UL << RTC_CR_REFCKON_POS)              /*!< 0x00000010 */
#define RTC_CR_REFCKON               RTC_CR_REFCKON_MSK
#define RTC_CR_TSEDGE_POS            (3U)
#define RTC_CR_TSEDGE_MSK            (0x1UL << RTC_CR_TSEDGE_POS)              /*!< 0x00000008 */
#define RTC_CR_TSEDGE                RTC_CR_TSEDGE_MSK                         /*!< Timestamp event active edge > */
#define RTC_CR_WUCKSEL_POS           (0U)
#define RTC_CR_WUCKSEL_MSK           (0x7UL << RTC_CR_WUCKSEL_POS)             /*!< 0x00000007 */
#define RTC_CR_WUCKSEL               RTC_CR_WUCKSEL_MSK                        /*!< Wakeup clock selection > */
#define RTC_CR_WUCKSEL_0             (0x1UL << RTC_CR_WUCKSEL_POS)             /*!< 0x00000001 */
#define RTC_CR_WUCKSEL_1             (0x2UL << RTC_CR_WUCKSEL_POS)             /*!< 0x00000002 */
#define RTC_CR_WUCKSEL_2             (0x4UL << RTC_CR_WUCKSEL_POS)             /*!< 0x00000004 */

/********************  Bits definition for RTC_WPR register  ******************/
#define RTC_WPR_KEY_POS              (0U)
#define RTC_WPR_KEY_MSK              (0xFFUL << RTC_WPR_KEY_POS)                /*!< 0x000000FF */
#define RTC_WPR_KEY                  RTC_WPR_KEY_MSK

/********************  Bits definition for RTC_CALR register  *****************/
#define RTC_CALR_CALP_POS            (15U)
#define RTC_CALR_CALP_MSK            (0x1UL << RTC_CALR_CALP_POS)               /*!< 0x00008000 */
#define RTC_CALR_CALP                RTC_CALR_CALP_MSK
#define RTC_CALR_CALW8_POS           (14U)
#define RTC_CALR_CALW8_MSK           (0x1UL << RTC_CALR_CALW8_POS)              /*!< 0x00004000 */
#define RTC_CALR_CALW8               RTC_CALR_CALW8_MSK
#define RTC_CALR_CALW16_POS          (13U)
#define RTC_CALR_CALW16_MSK          (0x1UL << RTC_CALR_CALW16_POS)             /*!< 0x00002000 */
#define RTC_CALR_CALW16              RTC_CALR_CALW16_MSK
#define RTC_CALR_CALM_POS            (0U)
#define RTC_CALR_CALM_MSK            (0x1FFUL << RTC_CALR_CALM_POS)             /*!< 0x000001FF */
#define RTC_CALR_CALM                RTC_CALR_CALM_MSK
#define RTC_CALR_CALM_0              (0x001UL << RTC_CALR_CALM_POS)             /*!< 0x00000001 */
#define RTC_CALR_CALM_1              (0x002UL << RTC_CALR_CALM_POS)             /*!< 0x00000002 */
#define RTC_CALR_CALM_2              (0x004UL << RTC_CALR_CALM_POS)             /*!< 0x00000004 */
#define RTC_CALR_CALM_3              (0x008UL << RTC_CALR_CALM_POS)             /*!< 0x00000008 */
#define RTC_CALR_CALM_4              (0x010UL << RTC_CALR_CALM_POS)             /*!< 0x00000010 */
#define RTC_CALR_CALM_5              (0x020UL << RTC_CALR_CALM_POS)             /*!< 0x00000020 */
#define RTC_CALR_CALM_6              (0x040UL << RTC_CALR_CALM_POS)             /*!< 0x00000040 */
#define RTC_CALR_CALM_7              (0x080UL << RTC_CALR_CALM_POS)             /*!< 0x00000080 */
#define RTC_CALR_CALM_8              (0x100UL << RTC_CALR_CALM_POS)             /*!< 0x00000100 */

/********************  Bits definition for RTC_SHIFTR register  ***************/
#define RTC_SHIFTR_SUBFS_POS         (0U)
#define RTC_SHIFTR_SUBFS_MSK         (0x7FFFUL << RTC_SHIFTR_SUBFS_POS)         /*!< 0x00007FFF */
#define RTC_SHIFTR_SUBFS             RTC_SHIFTR_SUBFS_MSK
#define RTC_SHIFTR_ADD1S_POS         (31U)
#define RTC_SHIFTR_ADD1S_MSK         (0x1UL << RTC_SHIFTR_ADD1S_POS)            /*!< 0x80000000 */
#define RTC_SHIFTR_ADD1S             RTC_SHIFTR_ADD1S_MSK

/********************  Bits definition for RTC_TSTR register  *****************/
#define RTC_TSTR_PM_POS              (22U)
#define RTC_TSTR_PM_MSK              (0x1UL << RTC_TSTR_PM_POS)                /*!< 0x00400000 */
#define RTC_TSTR_PM                  RTC_TSTR_PM_MSK                           /*!< AM-PM notation > */
#define RTC_TSTR_HT_POS              (20U)
#define RTC_TSTR_HT_MSK              (0x3UL << RTC_TSTR_HT_POS)                /*!< 0x00300000 */
#define RTC_TSTR_HT                  RTC_TSTR_HT_MSK
#define RTC_TSTR_HT_0                (0x1UL << RTC_TSTR_HT_POS)                /*!< 0x00100000 */
#define RTC_TSTR_HT_1                (0x2UL << RTC_TSTR_HT_POS)                /*!< 0x00200000 */
#define RTC_TSTR_HU_POS              (16U)
#define RTC_TSTR_HU_MSK              (0xFUL << RTC_TSTR_HU_POS)                /*!< 0x000F0000 */
#define RTC_TSTR_HU                  RTC_TSTR_HU_MSK
#define RTC_TSTR_HU_0                (0x1UL << RTC_TSTR_HU_POS)                /*!< 0x00010000 */
#define RTC_TSTR_HU_1                (0x2UL << RTC_TSTR_HU_POS)                /*!< 0x00020000 */
#define RTC_TSTR_HU_2                (0x4UL << RTC_TSTR_HU_POS)                /*!< 0x00040000 */
#define RTC_TSTR_HU_3                (0x8UL << RTC_TSTR_HU_POS)                /*!< 0x00080000 */
#define RTC_TSTR_MNT_POS             (12U)
#define RTC_TSTR_MNT_MSK             (0x7UL << RTC_TSTR_MNT_POS)               /*!< 0x00007000 */
#define RTC_TSTR_MNT                 RTC_TSTR_MNT_MSK
#define RTC_TSTR_MNT_0               (0x1UL << RTC_TSTR_MNT_POS)               /*!< 0x00001000 */
#define RTC_TSTR_MNT_1               (0x2UL << RTC_TSTR_MNT_POS)               /*!< 0x00002000 */
#define RTC_TSTR_MNT_2               (0x4UL << RTC_TSTR_MNT_POS)                /*!< 0x00004000 */
#define RTC_TSTR_MNU_POS             (8U)
#define RTC_TSTR_MNU_MSK             (0xFUL << RTC_TSTR_MNU_POS)                /*!< 0x00000F00 */
#define RTC_TSTR_MNU                 RTC_TSTR_MNU_MSK
#define RTC_TSTR_MNU_0               (0x1UL << RTC_TSTR_MNU_POS)                /*!< 0x00000100 */
#define RTC_TSTR_MNU_1               (0x2UL << RTC_TSTR_MNU_POS)                /*!< 0x00000200 */
#define RTC_TSTR_MNU_2               (0x4UL << RTC_TSTR_MNU_POS)                /*!< 0x00000400 */
#define RTC_TSTR_MNU_3               (0x8UL << RTC_TSTR_MNU_POS)                /*!< 0x00000800 */
#define RTC_TSTR_ST_POS              (4U)
#define RTC_TSTR_ST_MSK              (0x7UL << RTC_TSTR_ST_POS)                 /*!< 0x00000070 */
#define RTC_TSTR_ST                  RTC_TSTR_ST_MSK
#define RTC_TSTR_ST_0                (0x1UL << RTC_TSTR_ST_POS)                 /*!< 0x00000010 */
#define RTC_TSTR_ST_1                (0x2UL << RTC_TSTR_ST_POS)                 /*!< 0x00000020 */
#define RTC_TSTR_ST_2                (0x4UL << RTC_TSTR_ST_POS)                 /*!< 0x00000040 */
#define RTC_TSTR_SU_POS              (0U)
#define RTC_TSTR_SU_MSK              (0xFUL << RTC_TSTR_SU_POS)                 /*!< 0x0000000F */
#define RTC_TSTR_SU                  RTC_TSTR_SU_MSK
#define RTC_TSTR_SU_0                (0x1UL << RTC_TSTR_SU_POS)                 /*!< 0x00000001 */
#define RTC_TSTR_SU_1                (0x2UL << RTC_TSTR_SU_POS)                 /*!< 0x00000002 */
#define RTC_TSTR_SU_2                (0x4UL << RTC_TSTR_SU_POS)                 /*!< 0x00000004 */
#define RTC_TSTR_SU_3                (0x8UL << RTC_TSTR_SU_POS)                 /*!< 0x00000008 */

/********************  Bits definition for RTC_TSDR register  *****************/
#define RTC_TSDR_WDU_POS             (13U)
#define RTC_TSDR_WDU_MSK             (0x7UL << RTC_TSDR_WDU_POS)               /*!< 0x0000E000 */
#define RTC_TSDR_WDU                 RTC_TSDR_WDU_MSK                          /*!< Week day units > */
#define RTC_TSDR_WDU_0               (0x1UL << RTC_TSDR_WDU_POS)               /*!< 0x00002000 */
#define RTC_TSDR_WDU_1               (0x2UL << RTC_TSDR_WDU_POS)               /*!< 0x00004000 */
#define RTC_TSDR_WDU_2               (0x4UL << RTC_TSDR_WDU_POS)               /*!< 0x00008000 */
#define RTC_TSDR_MT_POS              (12U)
#define RTC_TSDR_MT_MSK              (0x1UL << RTC_TSDR_MT_POS)                /*!< 0x00001000 */
#define RTC_TSDR_MT                  RTC_TSDR_MT_MSK
#define RTC_TSDR_MU_POS              (8U)
#define RTC_TSDR_MU_MSK              (0xFUL << RTC_TSDR_MU_POS)                /*!< 0x00000F00 */
#define RTC_TSDR_MU                  RTC_TSDR_MU_MSK
#define RTC_TSDR_MU_0                (0x1UL << RTC_TSDR_MU_POS)                /*!< 0x00000100 */
#define RTC_TSDR_MU_1                (0x2UL << RTC_TSDR_MU_POS)                /*!< 0x00000200 */
#define RTC_TSDR_MU_2                (0x4UL << RTC_TSDR_MU_POS)                /*!< 0x00000400 */
#define RTC_TSDR_MU_3                (0x8UL << RTC_TSDR_MU_POS)                /*!< 0x00000800 */
#define RTC_TSDR_DT_POS              (4U)
#define RTC_TSDR_DT_MSK              (0x3UL << RTC_TSDR_DT_POS)                /*!< 0x00000030 */
#define RTC_TSDR_DT                  RTC_TSDR_DT_MSK
#define RTC_TSDR_DT_0                (0x1UL << RTC_TSDR_DT_POS)                /*!< 0x00000010 */
#define RTC_TSDR_DT_1                (0x2UL << RTC_TSDR_DT_POS)                /*!< 0x00000020 */
#define RTC_TSDR_DU_POS              (0U)
#define RTC_TSDR_DU_MSK              (0xFUL << RTC_TSDR_DU_POS)                /*!< 0x0000000F */
#define RTC_TSDR_DU                  RTC_TSDR_DU_MSK
#define RTC_TSDR_DU_0                (0x1UL << RTC_TSDR_DU_POS)                /*!< 0x00000001 */
#define RTC_TSDR_DU_1                (0x2UL << RTC_TSDR_DU_POS)                /*!< 0x00000002 */
#define RTC_TSDR_DU_2                (0x4UL << RTC_TSDR_DU_POS)                /*!< 0x00000004 */
#define RTC_TSDR_DU_3                (0x8UL << RTC_TSDR_DU_POS)                /*!< 0x00000008 */

/********************  Bits definition for RTC_TSSSR register  ****************/
#define RTC_TSSSR_SS_POS             (0U)
#define RTC_TSSSR_SS_MSK             (0xFFFFUL << RTC_TSSSR_SS_POS)            /*!< 0x0000FFFF */
#define RTC_TSSSR_SS                 RTC_TSSSR_SS_MSK                          /*!< Sub second value > */

/********************  Bits definition for RTC_ALRMAR register  ***************/
#define RTC_ALRMAR_MSK4_POS          (31U)
#define RTC_ALRMAR_MSK4_MSK          (0x1UL << RTC_ALRMAR_MSK4_POS)            /*!< 0x80000000 */
#define RTC_ALRMAR_MSK4              RTC_ALRMAR_MSK4_MSK
#define RTC_ALRMAR_WDSEL_POS         (30U)
#define RTC_ALRMAR_WDSEL_MSK         (0x1UL << RTC_ALRMAR_WDSEL_POS)           /*!< 0x40000000 */
#define RTC_ALRMAR_WDSEL             RTC_ALRMAR_WDSEL_MSK
#define RTC_ALRMAR_DT_POS            (28U)
#define RTC_ALRMAR_DT_MSK            (0x3UL << RTC_ALRMAR_DT_POS)              /*!< 0x30000000 */
#define RTC_ALRMAR_DT                RTC_ALRMAR_DT_MSK
#define RTC_ALRMAR_DT_0              (0x1UL << RTC_ALRMAR_DT_POS)              /*!< 0x10000000 */
#define RTC_ALRMAR_DT_1              (0x2UL << RTC_ALRMAR_DT_POS)              /*!< 0x20000000 */
#define RTC_ALRMAR_DU_POS            (24U)
#define RTC_ALRMAR_DU_MSK            (0xFUL << RTC_ALRMAR_DU_POS)              /*!< 0x0F000000 */
#define RTC_ALRMAR_DU                RTC_ALRMAR_DU_MSK
#define RTC_ALRMAR_DU_0              (0x1UL << RTC_ALRMAR_DU_POS)              /*!< 0x01000000 */
#define RTC_ALRMAR_DU_1              (0x2UL << RTC_ALRMAR_DU_POS)              /*!< 0x02000000 */
#define RTC_ALRMAR_DU_2              (0x4UL << RTC_ALRMAR_DU_POS)              /*!< 0x04000000 */
#define RTC_ALRMAR_DU_3              (0x8UL << RTC_ALRMAR_DU_POS)              /*!< 0x08000000 */
#define RTC_ALRMAR_MSK3_POS          (23U)
#define RTC_ALRMAR_MSK3_MSK          (0x1UL << RTC_ALRMAR_MSK3_POS)            /*!< 0x00800000 */
#define RTC_ALRMAR_MSK3              RTC_ALRMAR_MSK3_MSK
#define RTC_ALRMAR_PM_POS            (22U)
#define RTC_ALRMAR_PM_MSK            (0x1UL << RTC_ALRMAR_PM_POS)              /*!< 0x00400000 */
#define RTC_ALRMAR_PM                RTC_ALRMAR_PM_MSK
#define RTC_ALRMAR_HT_POS            (20U)
#define RTC_ALRMAR_HT_MSK            (0x3UL << RTC_ALRMAR_HT_POS)              /*!< 0x00300000 */
#define RTC_ALRMAR_HT                RTC_ALRMAR_HT_MSK
#define RTC_ALRMAR_HT_0              (0x1UL << RTC_ALRMAR_HT_POS)              /*!< 0x00100000 */
#define RTC_ALRMAR_HT_1              (0x2UL << RTC_ALRMAR_HT_POS)              /*!< 0x00200000 */
#define RTC_ALRMAR_HU_POS            (16U)
#define RTC_ALRMAR_HU_MSK            (0xFUL << RTC_ALRMAR_HU_POS)              /*!< 0x000F0000 */
#define RTC_ALRMAR_HU                RTC_ALRMAR_HU_MSK
#define RTC_ALRMAR_HU_0              (0x1UL << RTC_ALRMAR_HU_POS)              /*!< 0x00010000 */
#define RTC_ALRMAR_HU_1              (0x2UL << RTC_ALRMAR_HU_POS)              /*!< 0x00020000 */
#define RTC_ALRMAR_HU_2              (0x4UL << RTC_ALRMAR_HU_POS)              /*!< 0x00040000 */
#define RTC_ALRMAR_HU_3              (0x8UL << RTC_ALRMAR_HU_POS)              /*!< 0x00080000 */
#define RTC_ALRMAR_MSK2_POS          (15U)
#define RTC_ALRMAR_MSK2_MSK          (0x1UL << RTC_ALRMAR_MSK2_POS)            /*!< 0x00008000 */
#define RTC_ALRMAR_MSK2              RTC_ALRMAR_MSK2_MSK
#define RTC_ALRMAR_MNT_POS           (12U)
#define RTC_ALRMAR_MNT_MSK           (0x7UL << RTC_ALRMAR_MNT_POS)             /*!< 0x00007000 */
#define RTC_ALRMAR_MNT               RTC_ALRMAR_MNT_MSK
#define RTC_ALRMAR_MNT_0             (0x1UL << RTC_ALRMAR_MNT_POS)             /*!< 0x00001000 */
#define RTC_ALRMAR_MNT_1             (0x2UL << RTC_ALRMAR_MNT_POS)             /*!< 0x00002000 */
#define RTC_ALRMAR_MNT_2             (0x4UL << RTC_ALRMAR_MNT_POS)             /*!< 0x00004000 */
#define RTC_ALRMAR_MNU_POS           (8U)
#define RTC_ALRMAR_MNU_MSK           (0xFUL << RTC_ALRMAR_MNU_POS)             /*!< 0x00000F00 */
#define RTC_ALRMAR_MNU               RTC_ALRMAR_MNU_MSK
#define RTC_ALRMAR_MNU_0             (0x1UL << RTC_ALRMAR_MNU_POS)             /*!< 0x00000100 */
#define RTC_ALRMAR_MNU_1             (0x2UL << RTC_ALRMAR_MNU_POS)             /*!< 0x00000200 */
#define RTC_ALRMAR_MNU_2             (0x4UL << RTC_ALRMAR_MNU_POS)             /*!< 0x00000400 */
#define RTC_ALRMAR_MNU_3             (0x8UL << RTC_ALRMAR_MNU_POS)             /*!< 0x00000800 */
#define RTC_ALRMAR_MSK1_POS          (7U)
#define RTC_ALRMAR_MSK1_MSK          (0x1UL << RTC_ALRMAR_MSK1_POS)            /*!< 0x00000080 */
#define RTC_ALRMAR_MSK1              RTC_ALRMAR_MSK1_MSK
#define RTC_ALRMAR_ST_POS            (4U)
#define RTC_ALRMAR_ST_MSK            (0x7UL << RTC_ALRMAR_ST_POS)              /*!< 0x00000070 */
#define RTC_ALRMAR_ST                RTC_ALRMAR_ST_MSK
#define RTC_ALRMAR_ST_0              (0x1UL << RTC_ALRMAR_ST_POS)              /*!< 0x00000010 */
#define RTC_ALRMAR_ST_1              (0x2UL << RTC_ALRMAR_ST_POS)              /*!< 0x00000020 */
#define RTC_ALRMAR_ST_2              (0x4UL << RTC_ALRMAR_ST_POS)              /*!< 0x00000040 */
#define RTC_ALRMAR_SU_POS            (0U)
#define RTC_ALRMAR_SU_MSK            (0xFUL << RTC_ALRMAR_SU_POS)              /*!< 0x0000000F */
#define RTC_ALRMAR_SU                RTC_ALRMAR_SU_MSK
#define RTC_ALRMAR_SU_0              (0x1UL << RTC_ALRMAR_SU_POS)              /*!< 0x00000001 */
#define RTC_ALRMAR_SU_1              (0x2UL << RTC_ALRMAR_SU_POS)              /*!< 0x00000002 */
#define RTC_ALRMAR_SU_2              (0x4UL << RTC_ALRMAR_SU_POS)              /*!< 0x00000004 */
#define RTC_ALRMAR_SU_3              (0x8UL << RTC_ALRMAR_SU_POS)              /*!< 0x00000008 */

/********************  Bits definition for RTC_ALRMASSR register  *************/
#define RTC_ALRMASSR_MASKSS_POS      (24U)
#define RTC_ALRMASSR_MASKSS_MSK      (0xFUL << RTC_ALRMASSR_MASKSS_POS)        /*!< 0x0F000000 */
#define RTC_ALRMASSR_MASKSS          RTC_ALRMASSR_MASKSS_MSK
#define RTC_ALRMASSR_MASKSS_0        (0x1UL << RTC_ALRMASSR_MASKSS_POS)        /*!< 0x01000000 */
#define RTC_ALRMASSR_MASKSS_1        (0x2UL << RTC_ALRMASSR_MASKSS_POS)        /*!< 0x02000000 */
#define RTC_ALRMASSR_MASKSS_2        (0x4UL << RTC_ALRMASSR_MASKSS_POS)        /*!< 0x04000000 */
#define RTC_ALRMASSR_MASKSS_3        (0x8UL << RTC_ALRMASSR_MASKSS_POS)        /*!< 0x08000000 */
#define RTC_ALRMASSR_SS_POS          (0U)
#define RTC_ALRMASSR_SS_MSK          (0x7FFFUL << RTC_ALRMASSR_SS_POS)         /*!< 0x00007FFF */
#define RTC_ALRMASSR_SS              RTC_ALRMASSR_SS_MSK

/********************  Bits definition for RTC_ALRMBR register  ***************/
#define RTC_ALRMBR_MSK4_POS          (31U)
#define RTC_ALRMBR_MSK4_MSK          (0x1UL << RTC_ALRMBR_MSK4_POS)            /*!< 0x80000000 */
#define RTC_ALRMBR_MSK4              RTC_ALRMBR_MSK4_MSK
#define RTC_ALRMBR_WDSEL_POS         (30U)
#define RTC_ALRMBR_WDSEL_MSK         (0x1UL << RTC_ALRMBR_WDSEL_POS)           /*!< 0x40000000 */
#define RTC_ALRMBR_WDSEL             RTC_ALRMBR_WDSEL_MSK
#define RTC_ALRMBR_DT_POS            (28U)
#define RTC_ALRMBR_DT_MSK            (0x3UL << RTC_ALRMBR_DT_POS)              /*!< 0x30000000 */
#define RTC_ALRMBR_DT                RTC_ALRMBR_DT_MSK
#define RTC_ALRMBR_DT_0              (0x1UL << RTC_ALRMBR_DT_POS)              /*!< 0x10000000 */
#define RTC_ALRMBR_DT_1              (0x2UL << RTC_ALRMBR_DT_POS)              /*!< 0x20000000 */
#define RTC_ALRMBR_DU_POS            (24U)
#define RTC_ALRMBR_DU_MSK            (0xFUL << RTC_ALRMBR_DU_POS)              /*!< 0x0F000000 */
#define RTC_ALRMBR_DU                RTC_ALRMBR_DU_MSK
#define RTC_ALRMBR_DU_0              (0x1UL << RTC_ALRMBR_DU_POS)              /*!< 0x01000000 */
#define RTC_ALRMBR_DU_1              (0x2UL << RTC_ALRMBR_DU_POS)              /*!< 0x02000000 */
#define RTC_ALRMBR_DU_2              (0x4UL << RTC_ALRMBR_DU_POS)              /*!< 0x04000000 */
#define RTC_ALRMBR_DU_3              (0x8UL << RTC_ALRMBR_DU_POS)              /*!< 0x08000000 */
#define RTC_ALRMBR_MSK3_POS          (23U)
#define RTC_ALRMBR_MSK3_MSK          (0x1UL << RTC_ALRMBR_MSK3_POS)            /*!< 0x00800000 */
#define RTC_ALRMBR_MSK3              RTC_ALRMBR_MSK3_MSK
#define RTC_ALRMBR_PM_POS            (22U)
#define RTC_ALRMBR_PM_MSK            (0x1UL << RTC_ALRMBR_PM_POS)              /*!< 0x00400000 */
#define RTC_ALRMBR_PM                RTC_ALRMBR_PM_MSK
#define RTC_ALRMBR_HT_POS            (20U)
#define RTC_ALRMBR_HT_MSK            (0x3UL << RTC_ALRMBR_HT_POS)              /*!< 0x00300000 */
#define RTC_ALRMBR_HT                RTC_ALRMBR_HT_MSK
#define RTC_ALRMBR_HT_0              (0x1UL << RTC_ALRMBR_HT_POS)              /*!< 0x00100000 */
#define RTC_ALRMBR_HT_1              (0x2UL << RTC_ALRMBR_HT_POS)              /*!< 0x00200000 */
#define RTC_ALRMBR_HU_POS            (16U)
#define RTC_ALRMBR_HU_MSK            (0xFUL << RTC_ALRMBR_HU_POS)              /*!< 0x000F0000 */
#define RTC_ALRMBR_HU                RTC_ALRMBR_HU_MSK
#define RTC_ALRMBR_HU_0              (0x1UL << RTC_ALRMBR_HU_POS)              /*!< 0x00010000 */
#define RTC_ALRMBR_HU_1              (0x2UL << RTC_ALRMBR_HU_POS)              /*!< 0x00020000 */
#define RTC_ALRMBR_HU_2              (0x4UL << RTC_ALRMBR_HU_POS)              /*!< 0x00040000 */
#define RTC_ALRMBR_HU_3              (0x8UL << RTC_ALRMBR_HU_POS)              /*!< 0x00080000 */
#define RTC_ALRMBR_MSK2_POS          (15U)
#define RTC_ALRMBR_MSK2_MSK          (0x1UL << RTC_ALRMBR_MSK2_POS)            /*!< 0x00008000 */
#define RTC_ALRMBR_MSK2              RTC_ALRMBR_MSK2_MSK
#define RTC_ALRMBR_MNT_POS           (12U)
#define RTC_ALRMBR_MNT_MSK           (0x7UL << RTC_ALRMBR_MNT_POS)             /*!< 0x00007000 */
#define RTC_ALRMBR_MNT               RTC_ALRMBR_MNT_MSK
#define RTC_ALRMBR_MNT_0             (0x1UL << RTC_ALRMBR_MNT_POS)             /*!< 0x00001000 */
#define RTC_ALRMBR_MNT_1             (0x2UL << RTC_ALRMBR_MNT_POS)             /*!< 0x00002000 */
#define RTC_ALRMBR_MNT_2             (0x4UL << RTC_ALRMBR_MNT_POS)             /*!< 0x00004000 */
#define RTC_ALRMBR_MNU_POS           (8U)
#define RTC_ALRMBR_MNU_MSK           (0xFUL << RTC_ALRMBR_MNU_POS)             /*!< 0x00000F00 */
#define RTC_ALRMBR_MNU               RTC_ALRMBR_MNU_MSK
#define RTC_ALRMBR_MNU_0             (0x1UL << RTC_ALRMBR_MNU_POS)             /*!< 0x00000100 */
#define RTC_ALRMBR_MNU_1             (0x2UL << RTC_ALRMBR_MNU_POS)             /*!< 0x00000200 */
#define RTC_ALRMBR_MNU_2             (0x4UL << RTC_ALRMBR_MNU_POS)             /*!< 0x00000400 */
#define RTC_ALRMBR_MNU_3             (0x8UL << RTC_ALRMBR_MNU_POS)             /*!< 0x00000800 */
#define RTC_ALRMBR_MSK1_POS          (7U)
#define RTC_ALRMBR_MSK1_MSK          (0x1UL << RTC_ALRMBR_MSK1_POS)            /*!< 0x00000080 */
#define RTC_ALRMBR_MSK1              RTC_ALRMBR_MSK1_MSK
#define RTC_ALRMBR_ST_POS            (4U)
#define RTC_ALRMBR_ST_MSK            (0x7UL << RTC_ALRMBR_ST_POS)              /*!< 0x00000070 */
#define RTC_ALRMBR_ST                RTC_ALRMBR_ST_MSK
#define RTC_ALRMBR_ST_0              (0x1UL << RTC_ALRMBR_ST_POS)              /*!< 0x00000010 */
#define RTC_ALRMBR_ST_1              (0x2UL << RTC_ALRMBR_ST_POS)              /*!< 0x00000020 */
#define RTC_ALRMBR_ST_2              (0x4UL << RTC_ALRMBR_ST_POS)              /*!< 0x00000040 */
#define RTC_ALRMBR_SU_POS            (0U)
#define RTC_ALRMBR_SU_MSK            (0xFUL << RTC_ALRMBR_SU_POS)              /*!< 0x0000000F */
#define RTC_ALRMBR_SU                RTC_ALRMBR_SU_MSK
#define RTC_ALRMBR_SU_0              (0x1UL << RTC_ALRMBR_SU_POS)              /*!< 0x00000001 */
#define RTC_ALRMBR_SU_1              (0x2UL << RTC_ALRMBR_SU_POS)              /*!< 0x00000002 */
#define RTC_ALRMBR_SU_2              (0x4UL << RTC_ALRMBR_SU_POS)              /*!< 0x00000004 */
#define RTC_ALRMBR_SU_3              (0x8UL << RTC_ALRMBR_SU_POS)              /*!< 0x00000008 */

/********************  Bits definition for RTC_ALRMASSR register  *************/
#define RTC_ALRMBSSR_MASKSS_POS      (24U)
#define RTC_ALRMBSSR_MASKSS_MSK      (0xFUL << RTC_ALRMBSSR_MASKSS_POS)        /*!< 0x0F000000 */
#define RTC_ALRMBSSR_MASKSS          RTC_ALRMBSSR_MASKSS_MSK
#define RTC_ALRMBSSR_MASKSS_0        (0x1UL << RTC_ALRMBSSR_MASKSS_POS)        /*!< 0x01000000 */
#define RTC_ALRMBSSR_MASKSS_1        (0x2UL << RTC_ALRMBSSR_MASKSS_POS)        /*!< 0x02000000 */
#define RTC_ALRMBSSR_MASKSS_2        (0x4UL << RTC_ALRMBSSR_MASKSS_POS)        /*!< 0x04000000 */
#define RTC_ALRMBSSR_MASKSS_3        (0x8UL << RTC_ALRMBSSR_MASKSS_POS)        /*!< 0x08000000 */
#define RTC_ALRMBSSR_SS_POS          (0U)
#define RTC_ALRMBSSR_SS_MSK          (0x7FFFUL << RTC_ALRMBSSR_SS_POS)         /*!< 0x00007FFF */
#define RTC_ALRMBSSR_SS              RTC_ALRMBSSR_SS_MSK

/********************  Bits definition for RTC_SR register  *******************/
#define RTC_SR_ITSF_POS              (5U)
#define RTC_SR_ITSF_MSK              (0x1UL << RTC_SR_ITSF_POS)                /*!< 0x00000020 */
#define RTC_SR_ITSF                  RTC_SR_ITSF_MSK
#define RTC_SR_TSOVF_POS             (4U)
#define RTC_SR_TSOVF_MSK             (0x1UL << RTC_SR_TSOVF_POS)               /*!< 0x00000010 */
#define RTC_SR_TSOVF                 RTC_SR_TSOVF_MSK                          /*!< Timestamp overflow flag > */
#define RTC_SR_TSF_POS               (3U)
#define RTC_SR_TSF_MSK               (0x1UL << RTC_SR_TSF_POS)                 /*!< 0x00000008 */
#define RTC_SR_TSF                   RTC_SR_TSF_MSK                            /*!< Timestamp flag > */
#define RTC_SR_WUTF_POS              (2U)
#define RTC_SR_WUTF_MSK              (0x1UL << RTC_SR_WUTF_POS)                /*!< 0x00000004 */
#define RTC_SR_WUTF                  RTC_SR_WUTF_MSK                           /*!< Wakeup timer flag > */
#define RTC_SR_ALRBF_POS             (1U)
#define RTC_SR_ALRBF_MSK             (0x1UL << RTC_SR_ALRBF_POS)               /*!< 0x00000002 */
#define RTC_SR_ALRBF                 RTC_SR_ALRBF_MSK
#define RTC_SR_ALRAF_POS             (0U)
#define RTC_SR_ALRAF_MSK             (0x1UL << RTC_SR_ALRAF_POS)               /*!< 0x00000001 */
#define RTC_SR_ALRAF                 RTC_SR_ALRAF_MSK

/********************  Bits definition for RTC_MISR register  *****************/
#define RTC_MISR_ITSMF_POS           (5U)
#define RTC_MISR_ITSMF_MSK           (0x1UL << RTC_MISR_ITSMF_POS)             /*!< 0x00000020 */
#define RTC_MISR_ITSMF               RTC_MISR_ITSMF_MSK
#define RTC_MISR_TSOVMF_POS          (4U)
#define RTC_MISR_TSOVMF_MSK          (0x1UL << RTC_MISR_TSOVMF_POS)            /*!< 0x00000010 */
#define RTC_MISR_TSOVMF              RTC_MISR_TSOVMF_MSK                       /*!< Timestamp overflow masked flag > */
#define RTC_MISR_TSMF_POS            (3U)
#define RTC_MISR_TSMF_MSK            (0x1UL << RTC_MISR_TSMF_POS)              /*!< 0x00000008 */
#define RTC_MISR_TSMF                RTC_MISR_TSMF_MSK                         /*!< Timestamp masked flag > */
#define RTC_MISR_WUTMF_POS           (2U)
#define RTC_MISR_WUTMF_MSK           (0x1UL << RTC_MISR_WUTMF_POS)             /*!< 0x00000004 */
#define RTC_MISR_WUTMF               RTC_MISR_WUTMF_MSK                        /*!< Wakeup timer masked flag > */
#define RTC_MISR_ALRBMF_POS          (1U)
#define RTC_MISR_ALRBMF_MSK          (0x1UL << RTC_MISR_ALRBMF_POS)            /*!< 0x00000002 */
#define RTC_MISR_ALRBMF              RTC_MISR_ALRBMF_MSK
#define RTC_MISR_ALRAMF_POS          (0U)
#define RTC_MISR_ALRAMF_MSK          (0x1UL << RTC_MISR_ALRAMF_POS)            /*!< 0x00000001 */
#define RTC_MISR_ALRAMF              RTC_MISR_ALRAMF_MSK

/********************  Bits definition for RTC_SCR register  ******************/
#define RTC_SCR_CITSF_POS            (5U)
#define RTC_SCR_CITSF_MSK            (0x1UL << RTC_SCR_CITSF_POS)              /*!< 0x00000020 */
#define RTC_SCR_CITSF                RTC_SCR_CITSF_MSK
#define RTC_SCR_CTSOVF_POS           (4U)
#define RTC_SCR_CTSOVF_MSK           (0x1UL << RTC_SCR_CTSOVF_POS)             /*!< 0x00000010 */
#define RTC_SCR_CTSOVF               RTC_SCR_CTSOVF_MSK                        /*!< Clear timestamp overflow flag > */
#define RTC_SCR_CTSF_POS             (3U)
#define RTC_SCR_CTSF_MSK             (0x1UL << RTC_SCR_CTSF_POS)               /*!< 0x00000008 */
#define RTC_SCR_CTSF                 RTC_SCR_CTSF_MSK                          /*!< Clear timestamp flag > */
#define RTC_SCR_CWUTF_POS            (2U)
#define RTC_SCR_CWUTF_MSK            (0x1UL << RTC_SCR_CWUTF_POS)              /*!< 0x00000004 */
#define RTC_SCR_CWUTF                RTC_SCR_CWUTF_MSK                         /*!< Clear wakeup timer flag > */
#define RTC_SCR_CALRBF_POS           (1U)
#define RTC_SCR_CALRBF_MSK           (0x1UL << RTC_SCR_CALRBF_POS)             /*!< 0x00000002 */
#define RTC_SCR_CALRBF               RTC_SCR_CALRBF_MSK
#define RTC_SCR_CALRAF_POS           (0U)
#define RTC_SCR_CALRAF_MSK           (0x1UL << RTC_SCR_CALRAF_POS)             /*!< 0x00000001 */
#define RTC_SCR_CALRAF               RTC_SCR_CALRAF_MSK





//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- SPI REGISTERS ----------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Serial Peripheral Interface( SPI )
 */
typedef struct
{
    volatile uint16 CR1;        //! SPI Control register 1 (not used in I2S mode),
    uint16  RESERVED0;          //! Reserved, 0x02
    volatile uint16 CR2;        //! SPI Control register 2,
    uint16  RESERVED1;          //! Reserved, 0x06
    volatile uint16 SR;         //! SPI Status register,
    uint16  RESERVED2;          //! Reserved, 0x0A
    volatile uint16 DR;         //! SPI data register,
    uint16  RESERVED3;          //! Reserved, 0x0E
    volatile uint16 CRCPR;      //! SPI CRC polynomial register (not used in I2S mode),
    uint16  RESERVED4;          //! Reserved, 0x12
    volatile uint16 RXCRCR;     //! SPI Rx CRC register (not used in I2S mode),
    uint16  RESERVED5;          //! Reserved, 0x16
    volatile uint16 TXCRCR;     //! SPI Tx CRC register (not used in I2S mode),
    uint16  RESERVED6;          //! Reserved, 0x1A
    volatile uint16 I2SCFGR;    //! SPI_I2S configuration register,
    uint16  RESERVED7;          //! Reserved, 0x1E
    volatile uint16 I2SPR;      //! SPI_I2S prescaler register,
    uint16  RESERVED8;          //! Reserved, 0x22
} SPI_BASIC_DEF;

#define SPI_1                (*(SPI_BASIC_DEF *) SPI1_BASE)
#define SPI_2                (*(SPI_BASIC_DEF *) SPI2_BASE)
#define SPI_3                (*(SPI_BASIC_DEF *) SPI3_BASE)

/*******************  Bit definition for SPI_CR1 register  ********************/
#define SPI_CR1_CPHA_POS            (0U)
#define SPI_CR1_CPHA_MSK            (0x1UL << SPI_CR1_CPHA_POS)                /*!< 0x00000001 */
#define SPI_CR1_CPHA                SPI_CR1_CPHA_MSK                           /*!<Clock Phase      */
#define SPI_CR1_CPOL_POS            (1U)
#define SPI_CR1_CPOL_MSK            (0x1UL << SPI_CR1_CPOL_POS)                /*!< 0x00000002 */
#define SPI_CR1_CPOL                SPI_CR1_CPOL_MSK                           /*!<Clock Polarity   */
#define SPI_CR1_MSTR_POS            (2U)
#define SPI_CR1_MSTR_MSK            (0x1UL << SPI_CR1_MSTR_POS)                /*!< 0x00000004 */
#define SPI_CR1_MSTR                SPI_CR1_MSTR_MSK                           /*!<Master Selection */

#define SPI_CR1_BR_POS              (3U)
#define SPI_CR1_BR_MSK              (0x7UL << SPI_CR1_BR_POS)                  /*!< 0x00000038 */
#define SPI_CR1_BR                  SPI_CR1_BR_MSK                             /*!<BR[2:0] bits (Baud Rate Control) */
#define SPI_CR1_BR_0                (0x1UL << SPI_CR1_BR_POS)                  /*!< 0x00000008 */
#define SPI_CR1_BR_1                (0x2UL << SPI_CR1_BR_POS)                  /*!< 0x00000010 */
#define SPI_CR1_BR_2                (0x4UL << SPI_CR1_BR_POS)                  /*!< 0x00000020 */

#define SPI_CR1_SPE_POS             (6U)
#define SPI_CR1_SPE_MSK             (0x1UL << SPI_CR1_SPE_POS)                 /*!< 0x00000040 */
#define SPI_CR1_SPE                 SPI_CR1_SPE_MSK                            /*!<SPI Enable                          */
#define SPI_CR1_LSBFIRST_POS        (7U)
#define SPI_CR1_LSBFIRST_MSK        (0x1UL << SPI_CR1_LSBFIRST_POS)            /*!< 0x00000080 */
#define SPI_CR1_LSBFIRST            SPI_CR1_LSBFIRST_MSK                       /*!<Frame Format                        */
#define SPI_CR1_SSI_POS             (8U)
#define SPI_CR1_SSI_MSK             (0x1UL << SPI_CR1_SSI_POS)                 /*!< 0x00000100 */
#define SPI_CR1_SSI                 SPI_CR1_SSI_MSK                            /*!<Internal slave select               */
#define SPI_CR1_SSM_POS             (9U)
#define SPI_CR1_SSM_MSK             (0x1UL << SPI_CR1_SSM_POS)                 /*!< 0x00000200 */
#define SPI_CR1_SSM                 SPI_CR1_SSM_MSK                            /*!<Software slave management           */
#define SPI_CR1_RXONLY_POS          (10U)
#define SPI_CR1_RXONLY_MSK          (0x1UL << SPI_CR1_RXONLY_POS)              /*!< 0x00000400 */
#define SPI_CR1_RXONLY              SPI_CR1_RXONLY_MSK                         /*!<Receive only                        */
#define SPI_CR1_CRCL_POS            (11U)
#define SPI_CR1_CRCL_MSK            (0x1UL << SPI_CR1_CRCL_POS)                /*!< 0x00000800 */
#define SPI_CR1_CRCL                SPI_CR1_CRCL_MSK                           /*!< CRC Length */
#define SPI_CR1_CRCNEXT_POS         (12U)
#define SPI_CR1_CRCNEXT_MSK         (0x1UL << SPI_CR1_CRCNEXT_POS)             /*!< 0x00001000 */
#define SPI_CR1_CRCNEXT             SPI_CR1_CRCNEXT_MSK                        /*!<Transmit CRC next                   */
#define SPI_CR1_CRCEN_POS           (13U)
#define SPI_CR1_CRCEN_MSK           (0x1UL << SPI_CR1_CRCEN_POS)               /*!< 0x00002000 */
#define SPI_CR1_CRCEN               SPI_CR1_CRCEN_MSK                          /*!<Hardware CRC calculation enable     */
#define SPI_CR1_BIDIOE_POS          (14U)
#define SPI_CR1_BIDIOE_MSK          (0x1UL << SPI_CR1_BIDIOE_POS)              /*!< 0x00004000 */
#define SPI_CR1_BIDIOE              SPI_CR1_BIDIOE_MSK                         /*!<Output enable in bidirectional mode */
#define SPI_CR1_BIDIMODE_POS        (15U)
#define SPI_CR1_BIDIMODE_MSK        (0x1UL << SPI_CR1_BIDIMODE_POS)            /*!< 0x00008000 */
#define SPI_CR1_BIDIMODE            SPI_CR1_BIDIMODE_MSK                       /*!<Bidirectional data mode enable      */

/*******************  Bit definition for SPI_CR2 register  ********************/
#define SPI_CR2_RXDMAEN_POS         (0U)
#define SPI_CR2_RXDMAEN_MSK         (0x1UL << SPI_CR2_RXDMAEN_POS)             /*!< 0x00000001 */
#define SPI_CR2_RXDMAEN             SPI_CR2_RXDMAEN_MSK                        /*!< Rx Buffer DMA Enable */
#define SPI_CR2_TXDMAEN_POS         (1U)
#define SPI_CR2_TXDMAEN_MSK         (0x1UL << SPI_CR2_TXDMAEN_POS)             /*!< 0x00000002 */
#define SPI_CR2_TXDMAEN             SPI_CR2_TXDMAEN_MSK                        /*!< Tx Buffer DMA Enable */
#define SPI_CR2_SSOE_POS            (2U)
#define SPI_CR2_SSOE_MSK            (0x1UL << SPI_CR2_SSOE_POS)                /*!< 0x00000004 */
#define SPI_CR2_SSOE                SPI_CR2_SSOE_MSK                           /*!< SS Output Enable */
#define SPI_CR2_NSSP_POS            (3U)
#define SPI_CR2_NSSP_MSK            (0x1UL << SPI_CR2_NSSP_POS)                /*!< 0x00000008 */
#define SPI_CR2_NSSP                SPI_CR2_NSSP_MSK                           /*!< NSS pulse management Enable */
#define SPI_CR2_FRF_POS             (4U)
#define SPI_CR2_FRF_MSK             (0x1UL << SPI_CR2_FRF_POS)                 /*!< 0x00000010 */
#define SPI_CR2_FRF                 SPI_CR2_FRF_MSK                            /*!< Frame Format Enable */
#define SPI_CR2_ERRIE_POS           (5U)
#define SPI_CR2_ERRIE_MSK           (0x1UL << SPI_CR2_ERRIE_POS)               /*!< 0x00000020 */
#define SPI_CR2_ERRIE               SPI_CR2_ERRIE_MSK                          /*!< Error Interrupt Enable */
#define SPI_CR2_RXNEIE_POS          (6U)
#define SPI_CR2_RXNEIE_MSK          (0x1UL << SPI_CR2_RXNEIE_POS)              /*!< 0x00000040 */
#define SPI_CR2_RXNEIE              SPI_CR2_RXNEIE_MSK                         /*!< RX buffer Not Empty Interrupt Enable */
#define SPI_CR2_TXEIE_POS           (7U)
#define SPI_CR2_TXEIE_MSK           (0x1UL << SPI_CR2_TXEIE_POS)               /*!< 0x00000080 */
#define SPI_CR2_TXEIE               SPI_CR2_TXEIE_MSK                          /*!< Tx buffer Empty Interrupt Enable */
#define SPI_CR2_DS_POS              (8U)
#define SPI_CR2_DS_MSK              (0xFUL << SPI_CR2_DS_POS)                  /*!< 0x00000F00 */
#define SPI_CR2_DS                  SPI_CR2_DS_MSK                             /*!< DS[3:0] Data Size */
#define SPI_CR2_DS_0                (0x1UL << SPI_CR2_DS_POS)                  /*!< 0x00000100 */
#define SPI_CR2_DS_1                (0x2UL << SPI_CR2_DS_POS)                  /*!< 0x00000200 */
#define SPI_CR2_DS_2                (0x4UL << SPI_CR2_DS_POS)                  /*!< 0x00000400 */
#define SPI_CR2_DS_3                (0x8UL << SPI_CR2_DS_POS)                  /*!< 0x00000800 */
#define SPI_CR2_FRXTH_POS           (12U)
#define SPI_CR2_FRXTH_MSK           (0x1UL << SPI_CR2_FRXTH_POS)               /*!< 0x00001000 */
#define SPI_CR2_FRXTH               SPI_CR2_FRXTH_MSK                          /*!< FIFO reception Threshold */
#define SPI_CR2_LDMARX_POS          (13U)
#define SPI_CR2_LDMARX_MSK          (0x1UL << SPI_CR2_LDMARX_POS)              /*!< 0x00002000 */
#define SPI_CR2_LDMARX              SPI_CR2_LDMARX_MSK                         /*!< Last DMA transfer for reception */
#define SPI_CR2_LDMATX_POS          (14U)
#define SPI_CR2_LDMATX_MSK          (0x1UL << SPI_CR2_LDMATX_POS)              /*!< 0x00004000 */
#define SPI_CR2_LDMATX              SPI_CR2_LDMATX_MSK                         /*!< Last DMA transfer for transmission */

/********************  Bit definition for SPI_SR register  ********************/
#define SPI_SR_RXNE_POS             (0U)
#define SPI_SR_RXNE_MSK             (0x1UL << SPI_SR_RXNE_POS)                 /*!< 0x00000001 */
#define SPI_SR_RXNE                 SPI_SR_RXNE_MSK                            /*!< Receive buffer Not Empty */
#define SPI_SR_TXE_POS              (1U)
#define SPI_SR_TXE_MSK              (0x1UL << SPI_SR_TXE_POS)                  /*!< 0x00000002 */
#define SPI_SR_TXE                  SPI_SR_TXE_MSK                             /*!< Transmit buffer Empty */
#define SPI_SR_CHSIDE_POS           (2U)
#define SPI_SR_CHSIDE_MSK           (0x1UL << SPI_SR_CHSIDE_POS)               /*!< 0x00000004 */
#define SPI_SR_CHSIDE               SPI_SR_CHSIDE_MSK                          /*!< CHANNEL side */
#define SPI_SR_UDR_POS              (3U)
#define SPI_SR_UDR_MSK              (0x1UL << SPI_SR_UDR_POS)                  /*!< 0x00000008 */
#define SPI_SR_UDR                  SPI_SR_UDR_MSK                             /*!< Underrun flag */
#define SPI_SR_CRCERR_POS           (4U)
#define SPI_SR_CRCERR_MSK           (0x1UL << SPI_SR_CRCERR_POS)               /*!< 0x00000010 */
#define SPI_SR_CRCERR               SPI_SR_CRCERR_MSK                          /*!< CRC Error flag */
#define SPI_SR_MODF_POS             (5U)
#define SPI_SR_MODF_MSK             (0x1UL << SPI_SR_MODF_POS)                 /*!< 0x00000020 */
#define SPI_SR_MODF                 SPI_SR_MODF_MSK                            /*!< Mode fault */
#define SPI_SR_OVR_POS              (6U)
#define SPI_SR_OVR_MSK              (0x1UL << SPI_SR_OVR_POS)                  /*!< 0x00000040 */
#define SPI_SR_OVR                  SPI_SR_OVR_MSK                             /*!< Overrun flag */
#define SPI_SR_BSY_POS              (7U)
#define SPI_SR_BSY_MSK              (0x1UL << SPI_SR_BSY_POS)                  /*!< 0x00000080 */
#define SPI_SR_BSY                  SPI_SR_BSY_MSK                             /*!< Busy flag */
#define SPI_SR_FRE_POS              (8U)
#define SPI_SR_FRE_MSK              (0x1UL << SPI_SR_FRE_POS)                  /*!< 0x00000100 */
#define SPI_SR_FRE                  SPI_SR_FRE_MSK                             /*!< TI frame format error */
#define SPI_SR_FRLVL_POS            (9U)
#define SPI_SR_FRLVL_MSK            (0x3UL << SPI_SR_FRLVL_POS)                /*!< 0x00000600 */
#define SPI_SR_FRLVL                SPI_SR_FRLVL_MSK                           /*!< FIFO Reception Level */
#define SPI_SR_FRLVL_0              (0x1UL << SPI_SR_FRLVL_POS)                /*!< 0x00000200 */
#define SPI_SR_FRLVL_1              (0x2UL << SPI_SR_FRLVL_POS)                /*!< 0x00000400 */
#define SPI_SR_FTLVL_POS            (11U)
#define SPI_SR_FTLVL_MSK            (0x3UL << SPI_SR_FTLVL_POS)                /*!< 0x00001800 */
#define SPI_SR_FTLVL                SPI_SR_FTLVL_MSK                           /*!< FIFO Transmission Level */
#define SPI_SR_FTLVL_0              (0x1UL << SPI_SR_FTLVL_POS)                /*!< 0x00000800 */
#define SPI_SR_FTLVL_1              (0x2UL << SPI_SR_FTLVL_POS)                /*!< 0x00001000 */

/********************  Bit definition for SPI_DR register  ********************/
#define SPI_DR_DR_POS               (0U)
#define SPI_DR_DR_MSK               (0xFFFFUL << SPI_DR_DR_POS)                /*!< 0x0000FFFF */
#define SPI_DR_DR                   SPI_DR_DR_MSK                              /*!<Data Register           */

/*******************  Bit definition for SPI_CRCPR register  ******************/
#define SPI_CRCPR_CRCPOLY_POS       (0U)
#define SPI_CRCPR_CRCPOLY_MSK       (0xFFFFUL << SPI_CRCPR_CRCPOLY_POS)        /*!< 0x0000FFFF */
#define SPI_CRCPR_CRCPOLY           SPI_CRCPR_CRCPOLY_MSK                      /*!<CRC polynomial register */

/******************  Bit definition for SPI_RXCRCR register  ******************/
#define SPI_RXCRCR_RXCRC_POS        (0U)
#define SPI_RXCRCR_RXCRC_MSK        (0xFFFFUL << SPI_RXCRCR_RXCRC_POS)         /*!< 0x0000FFFF */
#define SPI_RXCRCR_RXCRC            SPI_RXCRCR_RXCRC_MSK                       /*!<Rx CRC Register         */

/******************  Bit definition for SPI_TXCRCR register  ******************/
#define SPI_TXCRCR_TXCRC_POS        (0U)
#define SPI_TXCRCR_TXCRC_MSK        (0xFFFFUL << SPI_TXCRCR_TXCRC_POS)         /*!< 0x0000FFFF */
#define SPI_TXCRCR_TXCRC            SPI_TXCRCR_TXCRC_MSK                       /*!<Tx CRC Register         */

/******************  Bit definition for SPI_I2SCFGR register  *****************/
#define SPI_I2SCFGR_CHLEN_POS       (0U)
#define SPI_I2SCFGR_CHLEN_MSK       (0x1UL << SPI_I2SCFGR_CHLEN_POS)           /*!< 0x00000001 */
#define SPI_I2SCFGR_CHLEN           SPI_I2SCFGR_CHLEN_MSK                      /*!<CHANNEL length (number of bits per audio channel) */
#define SPI_I2SCFGR_DATLEN_POS      (1U)
#define SPI_I2SCFGR_DATLEN_MSK      (0x3UL << SPI_I2SCFGR_DATLEN_POS)          /*!< 0x00000006 */
#define SPI_I2SCFGR_DATLEN          SPI_I2SCFGR_DATLEN_MSK                     /*!<DATLEN[1:0] bits (Data length to be transferred) */
#define SPI_I2SCFGR_DATLEN_0        (0x1UL << SPI_I2SCFGR_DATLEN_POS)          /*!< 0x00000002 */
#define SPI_I2SCFGR_DATLEN_1        (0x2UL << SPI_I2SCFGR_DATLEN_POS)          /*!< 0x00000004 */
#define SPI_I2SCFGR_CKPOL_POS       (3U)
#define SPI_I2SCFGR_CKPOL_MSK       (0x1UL << SPI_I2SCFGR_CKPOL_POS)           /*!< 0x00000008 */
#define SPI_I2SCFGR_CKPOL           SPI_I2SCFGR_CKPOL_MSK                      /*!<steady state clock polarity */
#define SPI_I2SCFGR_I2SSTD_POS      (4U)
#define SPI_I2SCFGR_I2SSTD_MSK      (0x3UL << SPI_I2SCFGR_I2SSTD_POS)          /*!< 0x00000030 */
#define SPI_I2SCFGR_I2SSTD          SPI_I2SCFGR_I2SSTD_MSK                     /*!<I2SSTD[1:0] bits (I2S standard selection) */
#define SPI_I2SCFGR_I2SSTD_0        (0x1UL << SPI_I2SCFGR_I2SSTD_POS)          /*!< 0x00000010 */
#define SPI_I2SCFGR_I2SSTD_1        (0x2UL << SPI_I2SCFGR_I2SSTD_POS)          /*!< 0x00000020 */
#define SPI_I2SCFGR_PCMSYNC_POS     (7U)
#define SPI_I2SCFGR_PCMSYNC_MSK     (0x1UL << SPI_I2SCFGR_PCMSYNC_POS)         /*!< 0x00000080 */
#define SPI_I2SCFGR_PCMSYNC         SPI_I2SCFGR_PCMSYNC_MSK                    /*!<PCM frame synchronization */
#define SPI_I2SCFGR_I2SCFG_POS      (8U)
#define SPI_I2SCFGR_I2SCFG_MSK      (0x3UL << SPI_I2SCFGR_I2SCFG_POS)          /*!< 0x00000300 */
#define SPI_I2SCFGR_I2SCFG          SPI_I2SCFGR_I2SCFG_MSK                     /*!<I2SCFG[1:0] bits (I2S configuration mode) */
#define SPI_I2SCFGR_I2SCFG_0        (0x1UL << SPI_I2SCFGR_I2SCFG_POS)          /*!< 0x00000100 */
#define SPI_I2SCFGR_I2SCFG_1        (0x2UL << SPI_I2SCFGR_I2SCFG_POS)          /*!< 0x00000200 */
#define SPI_I2SCFGR_I2SE_POS        (10U)
#define SPI_I2SCFGR_I2SE_MSK        (0x1UL << SPI_I2SCFGR_I2SE_POS)            /*!< 0x00000400 */
#define SPI_I2SCFGR_I2SE            SPI_I2SCFGR_I2SE_MSK                       /*!<I2S Enable */
#define SPI_I2SCFGR_I2SMOD_POS      (11U)
#define SPI_I2SCFGR_I2SMOD_MSK      (0x1UL << SPI_I2SCFGR_I2SMOD_POS)          /*!< 0x00000800 */
#define SPI_I2SCFGR_I2SMOD          SPI_I2SCFGR_I2SMOD_MSK                     /*!<I2S mode selection */
#define SPI_I2SCFGR_ASTRTEN_POS     (12U)
#define SPI_I2SCFGR_ASTRTEN_MSK     (0x1UL << SPI_I2SCFGR_ASTRTEN_POS)         /*!< 0x00001000 */
#define SPI_I2SCFGR_ASTRTEN         SPI_I2SCFGR_ASTRTEN_MSK                    /*!<Asynchronous start enable */

/******************  Bit definition for SPI_I2SPR register  *******************/
#define SPI_I2SPR_I2SDIV_POS        (0U)
#define SPI_I2SPR_I2SDIV_MSK        (0xFFUL << SPI_I2SPR_I2SDIV_POS)           /*!< 0x000000FF */
#define SPI_I2SPR_I2SDIV            SPI_I2SPR_I2SDIV_MSK                       /*!<I2S Linear prescaler */
#define SPI_I2SPR_ODD_POS           (8U)
#define SPI_I2SPR_ODD_MSK           (0x1UL << SPI_I2SPR_ODD_POS)               /*!< 0x00000100 */
#define SPI_I2SPR_ODD               SPI_I2SPR_ODD_MSK                          /*!<Odd factor for the prescaler */
#define SPI_I2SPR_MCKOE_POS         (9U)
#define SPI_I2SPR_MCKOE_MSK         (0x1UL << SPI_I2SPR_MCKOE_POS)             /*!< 0x00000200 */
#define SPI_I2SPR_MCKOE             SPI_I2SPR_MCKOE_MSK                        /*!<Master Clock Output Enable */

#define SPI_BAUDRATEPRESCALER_2         (0x00000000U)
#define SPI_BAUDRATEPRESCALER_4         (SPI_CR1_BR_0)
#define SPI_BAUDRATEPRESCALER_8         (SPI_CR1_BR_1)
#define SPI_BAUDRATEPRESCALER_16        (SPI_CR1_BR_1 | SPI_CR1_BR_0)
#define SPI_BAUDRATEPRESCALER_32        (SPI_CR1_BR_2)
#define SPI_BAUDRATEPRESCALER_64        (SPI_CR1_BR_2 | SPI_CR1_BR_0)
#define SPI_BAUDRATEPRESCALER_128       (SPI_CR1_BR_2 | SPI_CR1_BR_1)
#define SPI_BAUDRATEPRESCALER_256       (SPI_CR1_BR_2 | SPI_CR1_BR_1 | SPI_CR1_BR_0)


//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- I2C REGISTERS ----------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Inter-integrated Circuit Interface(I2C)
 */
typedef struct
{
  volatile uint32 CR1;         /*!< I2C Control register 1,            Address offset: 0x00 */
  volatile uint32 CR2;         /*!< I2C Control register 2,            Address offset: 0x04 */
  volatile uint32 OAR1;        /*!< I2C Own address 1 register,        Address offset: 0x08 */
  volatile uint32 OAR2;        /*!< I2C Own address 2 register,        Address offset: 0x0C */
  volatile uint32 TIMINGR;     /*!< I2C Timing register,               Address offset: 0x10 */
  volatile uint32 TIMEOUTR;    /*!< I2C Timeout register,              Address offset: 0x14 */
  volatile uint32 ISR;         /*!< I2C Interrupt and status register, Address offset: 0x18 */
  volatile uint32 ICR;         /*!< I2C Interrupt clear register,      Address offset: 0x1C */
  volatile uint32 PECR;        /*!< I2C PEC register,                  Address offset: 0x20 */
  volatile uint32 RXDR;        /*!< I2C Receive data register,         Address offset: 0x24 */
  volatile uint32 TXDR;        /*!< I2C Transmit data register,        Address offset: 0x28 */
} I2C_TYPEDEF;

#define I2C_1                (*(I2C_TYPEDEF *) I2C1_BASE)
#define I2C_2                (*(I2C_TYPEDEF *) I2C2_BASE)
#define I2C_3                (*(I2C_TYPEDEF *) I2C3_BASE)

/*******************  Bit definition for I2C_CR1 register  *******************/
#define I2C_CR1_PE_POS               (0U)
#define I2C_CR1_PE_MSK               (0x1UL << I2C_CR1_PE_POS)                 /*!< 0x00000001 */
#define I2C_CR1_PE                   I2C_CR1_PE_MSK                            /*!< Peripheral enable */
#define I2C_CR1_TXIE_POS             (1U)
#define I2C_CR1_TXIE_MSK             (0x1UL << I2C_CR1_TXIE_POS)               /*!< 0x00000002 */
#define I2C_CR1_TXIE                 I2C_CR1_TXIE_MSK                          /*!< TX interrupt enable */
#define I2C_CR1_RXIE_POS             (2U)
#define I2C_CR1_RXIE_MSK             (0x1UL << I2C_CR1_RXIE_POS)               /*!< 0x00000004 */
#define I2C_CR1_RXIE                 I2C_CR1_RXIE_MSK                          /*!< RX interrupt enable */
#define I2C_CR1_ADDRIE_POS           (3U)
#define I2C_CR1_ADDRIE_MSK           (0x1UL << I2C_CR1_ADDRIE_POS)             /*!< 0x00000008 */
#define I2C_CR1_ADDRIE               I2C_CR1_ADDRIE_MSK                        /*!< Address match interrupt enable */
#define I2C_CR1_NACKIE_POS           (4U)
#define I2C_CR1_NACKIE_MSK           (0x1UL << I2C_CR1_NACKIE_POS)             /*!< 0x00000010 */
#define I2C_CR1_NACKIE               I2C_CR1_NACKIE_MSK                        /*!< NACK received interrupt enable */
#define I2C_CR1_STOPIE_POS           (5U)
#define I2C_CR1_STOPIE_MSK           (0x1UL << I2C_CR1_STOPIE_POS)             /*!< 0x00000020 */
#define I2C_CR1_STOPIE               I2C_CR1_STOPIE_MSK                        /*!< STOP detection interrupt enable */
#define I2C_CR1_TCIE_POS             (6U)
#define I2C_CR1_TCIE_MSK             (0x1UL << I2C_CR1_TCIE_POS)               /*!< 0x00000040 */
#define I2C_CR1_TCIE                 I2C_CR1_TCIE_MSK                          /*!< Transfer complete interrupt enable */
#define I2C_CR1_ERRIE_POS            (7U)
#define I2C_CR1_ERRIE_MSK            (0x1UL << I2C_CR1_ERRIE_POS)              /*!< 0x00000080 */
#define I2C_CR1_ERRIE                I2C_CR1_ERRIE_MSK                         /*!< Errors interrupt enable */
#define I2C_CR1_DNF_POS              (8U)
#define I2C_CR1_DNF_MSK              (0xFUL << I2C_CR1_DNF_POS)                /*!< 0x00000F00 */
#define I2C_CR1_DFN                  I2C_CR1_DNF_MSK                           /*!< Digital noise filter */
#define I2C_CR1_ANFOFF_POS           (12U)
#define I2C_CR1_ANFOFF_MSK           (0x1UL << I2C_CR1_ANFOFF_POS)             /*!< 0x00001000 */
#define I2C_CR1_ANFOFF               I2C_CR1_ANFOFF_MSK                        /*!< Analog noise filter OFF */
#define I2C_CR1_SWRST_POS            (13U)
#define I2C_CR1_SWRST_MSK            (0x1UL << I2C_CR1_SWRST_POS)              /*!< 0x00002000 */
#define I2C_CR1_SWRST                I2C_CR1_SWRST_MSK                         /*!< Software reset */
#define I2C_CR1_TXDMAEN_POS          (14U)
#define I2C_CR1_TXDMAEN_MSK          (0x1UL << I2C_CR1_TXDMAEN_POS)            /*!< 0x00004000 */
#define I2C_CR1_TXDMAEN              I2C_CR1_TXDMAEN_MSK                       /*!< DMA transmission requests enable */
#define I2C_CR1_RXDMAEN_POS          (15U)
#define I2C_CR1_RXDMAEN_MSK          (0x1UL << I2C_CR1_RXDMAEN_POS)            /*!< 0x00008000 */
#define I2C_CR1_RXDMAEN              I2C_CR1_RXDMAEN_MSK                       /*!< DMA reception requests enable */
#define I2C_CR1_SBC_POS              (16U)
#define I2C_CR1_SBC_MSK              (0x1UL << I2C_CR1_SBC_POS)                /*!< 0x00010000 */
#define I2C_CR1_SBC                  I2C_CR1_SBC_MSK                           /*!< Slave byte control */
#define I2C_CR1_NOSTRETCH_POS        (17U)
#define I2C_CR1_NOSTRETCH_MSK        (0x1UL << I2C_CR1_NOSTRETCH_POS)          /*!< 0x00020000 */
#define I2C_CR1_NOSTRETCH            I2C_CR1_NOSTRETCH_MSK                     /*!< Clock stretching disable */
#define I2C_CR1_WUPEN_POS            (18U)
#define I2C_CR1_WUPEN_MSK            (0x1UL << I2C_CR1_WUPEN_POS)              /*!< 0x00040000 */
#define I2C_CR1_WUPEN                I2C_CR1_WUPEN_MSK                         /*!< Wakeup from STOP enable */
#define I2C_CR1_GCEN_POS             (19U)
#define I2C_CR1_GCEN_MSK             (0x1UL << I2C_CR1_GCEN_POS)               /*!< 0x00080000 */
#define I2C_CR1_GCEN                 I2C_CR1_GCEN_MSK                          /*!< General call enable */
#define I2C_CR1_SMBHEN_POS           (20U)
#define I2C_CR1_SMBHEN_MSK           (0x1UL << I2C_CR1_SMBHEN_POS)             /*!< 0x00100000 */
#define I2C_CR1_SMBHEN               I2C_CR1_SMBHEN_MSK                        /*!< SMBus host address enable */
#define I2C_CR1_SMBDEN_POS           (21U)
#define I2C_CR1_SMBDEN_MSK           (0x1UL << I2C_CR1_SMBDEN_POS)             /*!< 0x00200000 */
#define I2C_CR1_SMBDEN               I2C_CR1_SMBDEN_MSK                        /*!< SMBus device default address enable */
#define I2C_CR1_ALERTEN_POS          (22U)
#define I2C_CR1_ALERTEN_MSK          (0x1UL << I2C_CR1_ALERTEN_POS)            /*!< 0x00400000 */
#define I2C_CR1_ALERTEN              I2C_CR1_ALERTEN_MSK                       /*!< SMBus alert enable */
#define I2C_CR1_PECEN_POS            (23U)
#define I2C_CR1_PECEN_MSK            (0x1UL << I2C_CR1_PECEN_POS)              /*!< 0x00800000 */
#define I2C_CR1_PECEN                I2C_CR1_PECEN_MSK                         /*!< PEC enable */

/******************  Bit definition for I2C_CR2 register  ********************/
#define I2C_CR2_SADD_POS             (0U)
#define I2C_CR2_SADD_MSK             (0x3FFUL << I2C_CR2_SADD_POS)             /*!< 0x000003FF */
#define I2C_CR2_SADD                 I2C_CR2_SADD_MSK                          /*!< Slave address (master mode) */
#define I2C_CR2_RD_WRN_POS           (10U)
#define I2C_CR2_RD_WRN_MSK           (0x1UL << I2C_CR2_RD_WRN_POS)             /*!< 0x00000400 */
#define I2C_CR2_RD_WRN               I2C_CR2_RD_WRN_MSK                        /*!< Transfer direction (master mode) */
#define I2C_CR2_ADD10_POS            (11U)
#define I2C_CR2_ADD10_MSK            (0x1UL << I2C_CR2_ADD10_POS)              /*!< 0x00000800 */
#define I2C_CR2_ADD10                I2C_CR2_ADD10_MSK                         /*!< 10-bit addressing mode (master mode) */
#define I2C_CR2_HEAD10R_POS          (12U)
#define I2C_CR2_HEAD10R_MSK          (0x1UL << I2C_CR2_HEAD10R_POS)            /*!< 0x00001000 */
#define I2C_CR2_HEAD10R              I2C_CR2_HEAD10R_MSK                       /*!< 10-bit address header only read direction (master mode) */
#define I2C_CR2_START_POS            (13U)
#define I2C_CR2_START_MSK            (0x1UL << I2C_CR2_START_POS)              /*!< 0x00002000 */
#define I2C_CR2_START                I2C_CR2_START_MSK                         /*!< START generation */
#define I2C_CR2_STOP_POS             (14U)
#define I2C_CR2_STOP_MSK             (0x1UL << I2C_CR2_STOP_POS)               /*!< 0x00004000 */
#define I2C_CR2_STOP                 I2C_CR2_STOP_MSK                          /*!< STOP generation (master mode) */
#define I2C_CR2_NACK_POS             (15U)
#define I2C_CR2_NACK_MSK             (0x1UL << I2C_CR2_NACK_POS)               /*!< 0x00008000 */
#define I2C_CR2_NACK                 I2C_CR2_NACK_MSK                          /*!< NACK generation (slave mode) */
#define I2C_CR2_NBYTES_POS           (16U)
#define I2C_CR2_NBYTES_MSK           (0xFFUL << I2C_CR2_NBYTES_POS)            /*!< 0x00FF0000 */
#define I2C_CR2_NBYTES               I2C_CR2_NBYTES_MSK                        /*!< Number of bytes */
#define I2C_CR2_RELOAD_POS           (24U)
#define I2C_CR2_RELOAD_MSK           (0x1UL << I2C_CR2_RELOAD_POS)             /*!< 0x01000000 */
#define I2C_CR2_RELOAD               I2C_CR2_RELOAD_MSK                        /*!< NBYTES reload mode */
#define I2C_CR2_AUTOEND_POS          (25U)
#define I2C_CR2_AUTOEND_MSK          (0x1UL << I2C_CR2_AUTOEND_POS)            /*!< 0x02000000 */
#define I2C_CR2_AUTOEND              I2C_CR2_AUTOEND_MSK                       /*!< Automatic end mode (master mode) */
#define I2C_CR2_PECBYTE_POS          (26U)
#define I2C_CR2_PECBYTE_MSK          (0x1UL << I2C_CR2_PECBYTE_POS)            /*!< 0x04000000 */
#define I2C_CR2_PECBYTE              I2C_CR2_PECBYTE_MSK                       /*!< Packet error checking byte */

/*******************  Bit definition for I2C_OAR1 register  ******************/
#define I2C_OAR1_OA1_POS             (0U)
#define I2C_OAR1_OA1_MSK             (0x3FFUL << I2C_OAR1_OA1_POS)             /*!< 0x000003FF */
#define I2C_OAR1_OA1                 I2C_OAR1_OA1_MSK                          /*!< Interface own address 1 */
#define I2C_OAR1_OA1MODE_POS         (10U)
#define I2C_OAR1_OA1MODE_MSK         (0x1UL << I2C_OAR1_OA1MODE_POS)           /*!< 0x00000400 */
#define I2C_OAR1_OA1MODE             I2C_OAR1_OA1MODE_MSK                      /*!< Own address 1 10-bit mode */
#define I2C_OAR1_OA1EN_POS           (15U)
#define I2C_OAR1_OA1EN_MSK           (0x1UL << I2C_OAR1_OA1EN_POS)             /*!< 0x00008000 */
#define I2C_OAR1_OA1EN               I2C_OAR1_OA1EN_MSK                        /*!< Own address 1 enable */

/*******************  Bit definition for I2C_OAR2 register  ******************/
#define I2C_OAR2_OA2_POS             (1U)
#define I2C_OAR2_OA2_MSK             (0x7FUL << I2C_OAR2_OA2_POS)              /*!< 0x000000FE */
#define I2C_OAR2_OA2                 I2C_OAR2_OA2_MSK                          /*!< Interface own address 2 */
#define I2C_OAR2_OA2MSK_POS          (8U)
#define I2C_OAR2_OA2MSK_MSK          (0x7UL << I2C_OAR2_OA2MSK_POS)            /*!< 0x00000700 */
#define I2C_OAR2_OA2MSK              I2C_OAR2_OA2MSK_MSK                       /*!< Own address 2 masks */
#define I2C_OAR2_OA2NOMASK           (0U)                                      /*!< No mask                                        */
#define I2C_OAR2_OA2MASK01_POS       (8U)
#define I2C_OAR2_OA2MASK01_MSK       (0x1UL << I2C_OAR2_OA2MASK01_POS)         /*!< 0x00000100 */
#define I2C_OAR2_OA2MASK01           I2C_OAR2_OA2MASK01_MSK                    /*!< OA2[1] is masked, Only OA2[7:2] are compared   */
#define I2C_OAR2_OA2MASK02_POS       (9U)
#define I2C_OAR2_OA2MASK02_MSK       (0x1UL << I2C_OAR2_OA2MASK02_POS)         /*!< 0x00000200 */
#define I2C_OAR2_OA2MASK02           I2C_OAR2_OA2MASK02_MSK                    /*!< OA2[2:1] is masked, Only OA2[7:3] are compared */
#define I2C_OAR2_OA2MASK03_POS       (8U)
#define I2C_OAR2_OA2MASK03_MSK       (0x3UL << I2C_OAR2_OA2MASK03_POS)         /*!< 0x00000300 */
#define I2C_OAR2_OA2MASK03           I2C_OAR2_OA2MASK03_MSK                    /*!< OA2[3:1] is masked, Only OA2[7:4] are compared */
#define I2C_OAR2_OA2MASK04_POS       (10U)
#define I2C_OAR2_OA2MASK04_MSK       (0x1UL << I2C_OAR2_OA2MASK04_POS)         /*!< 0x00000400 */
#define I2C_OAR2_OA2MASK04           I2C_OAR2_OA2MASK04_MSK                    /*!< OA2[4:1] is masked, Only OA2[7:5] are compared */
#define I2C_OAR2_OA2MASK05_POS       (8U)
#define I2C_OAR2_OA2MASK05_MSK       (0x5UL << I2C_OAR2_OA2MASK05_POS)         /*!< 0x00000500 */
#define I2C_OAR2_OA2MASK05           I2C_OAR2_OA2MASK05_MSK                    /*!< OA2[5:1] is masked, Only OA2[7:6] are compared */
#define I2C_OAR2_OA2MASK06_POS       (9U)
#define I2C_OAR2_OA2MASK06_MSK       (0x3UL << I2C_OAR2_OA2MASK06_POS)         /*!< 0x00000600 */
#define I2C_OAR2_OA2MASK06           I2C_OAR2_OA2MASK06_MSK                    /*!< OA2[6:1] is masked, Only OA2[7] are compared   */
#define I2C_OAR2_OA2MASK07_POS       (8U)
#define I2C_OAR2_OA2MASK07_MSK       (0x7UL << I2C_OAR2_OA2MASK07_POS)         /*!< 0x00000700 */
#define I2C_OAR2_OA2MASK07           I2C_OAR2_OA2MASK07_MSK                    /*!< OA2[7:1] is masked, No comparison is done      */
#define I2C_OAR2_OA2EN_POS           (15U)
#define I2C_OAR2_OA2EN_MSK           (0x1UL << I2C_OAR2_OA2EN_POS)             /*!< 0x00008000 */
#define I2C_OAR2_OA2EN               I2C_OAR2_OA2EN_MSK                        /*!< Own address 2 enable */

/*******************  Bit definition for I2C_TIMINGR register *******************/
#define I2C_TIMINGR_SCLL_POS         (0U)
#define I2C_TIMINGR_SCLL_MSK         (0xFFUL << I2C_TIMINGR_SCLL_POS)          /*!< 0x000000FF */
#define I2C_TIMINGR_SCLL             I2C_TIMINGR_SCLL_MSK                      /*!< SCL low period (master mode) */
#define I2C_TIMINGR_SCLH_POS         (8U)
#define I2C_TIMINGR_SCLH_MSK         (0xFFUL << I2C_TIMINGR_SCLH_POS)          /*!< 0x0000FF00 */
#define I2C_TIMINGR_SCLH             I2C_TIMINGR_SCLH_MSK                      /*!< SCL high period (master mode) */
#define I2C_TIMINGR_SDADEL_POS       (16U)
#define I2C_TIMINGR_SDADEL_MSK       (0xFUL << I2C_TIMINGR_SDADEL_POS)         /*!< 0x000F0000 */
#define I2C_TIMINGR_SDADEL           I2C_TIMINGR_SDADEL_MSK                    /*!< Data hold time */
#define I2C_TIMINGR_SCLDEL_POS       (20U)
#define I2C_TIMINGR_SCLDEL_MSK       (0xFUL << I2C_TIMINGR_SCLDEL_POS)         /*!< 0x00F00000 */
#define I2C_TIMINGR_SCLDEL           I2C_TIMINGR_SCLDEL_MSK                    /*!< Data setup time */
#define I2C_TIMINGR_PRESC_POS        (28U)
#define I2C_TIMINGR_PRESC_MSK        (0xFUL << I2C_TIMINGR_PRESC_POS)          /*!< 0xF0000000 */
#define I2C_TIMINGR_PRESC            I2C_TIMINGR_PRESC_MSK                     /*!< Timings prescaler */

/******************* Bit definition for I2C_TIMEOUTR register *******************/
#define I2C_TIMEOUTR_TIMEOUTA_POS    (0U)
#define I2C_TIMEOUTR_TIMEOUTA_MSK    (0xFFFUL << I2C_TIMEOUTR_TIMEOUTA_POS)    /*!< 0x00000FFF */
#define I2C_TIMEOUTR_TIMEOUTA        I2C_TIMEOUTR_TIMEOUTA_MSK                 /*!< Bus timeout A */
#define I2C_TIMEOUTR_TIDLE_POS       (12U)
#define I2C_TIMEOUTR_TIDLE_MSK       (0x1UL << I2C_TIMEOUTR_TIDLE_POS)         /*!< 0x00001000 */
#define I2C_TIMEOUTR_TIDLE           I2C_TIMEOUTR_TIDLE_MSK                    /*!< Idle clock timeout detection */
#define I2C_TIMEOUTR_TIMOUTEN_POS    (15U)
#define I2C_TIMEOUTR_TIMOUTEN_MSK    (0x1UL << I2C_TIMEOUTR_TIMOUTEN_POS)      /*!< 0x00008000 */
#define I2C_TIMEOUTR_TIMOUTEN        I2C_TIMEOUTR_TIMOUTEN_MSK                 /*!< Clock timeout enable */
#define I2C_TIMEOUTR_TIMEOUTB_POS    (16U)
#define I2C_TIMEOUTR_TIMEOUTB_MSK    (0xFFFUL << I2C_TIMEOUTR_TIMEOUTB_POS)    /*!< 0x0FFF0000 */
#define I2C_TIMEOUTR_TIMEOUTB        I2C_TIMEOUTR_TIMEOUTB_MSK                 /*!< Bus timeout B*/
#define I2C_TIMEOUTR_TEXTEN_POS      (31U)
#define I2C_TIMEOUTR_TEXTEN_MSK      (0x1UL << I2C_TIMEOUTR_TEXTEN_POS)        /*!< 0x80000000 */
#define I2C_TIMEOUTR_TEXTEN          I2C_TIMEOUTR_TEXTEN_MSK                   /*!< Extended clock timeout enable */

/******************  Bit definition for I2C_ISR register  *********************/
#define I2C_ISR_TXE_POS              (0U)
#define I2C_ISR_TXE_MSK              (0x1UL << I2C_ISR_TXE_POS)                /*!< 0x00000001 */
#define I2C_ISR_TXE                  I2C_ISR_TXE_MSK                           /*!< Transmit data register empty */
#define I2C_ISR_TXIS_POS             (1U)
#define I2C_ISR_TXIS_MSK             (0x1UL << I2C_ISR_TXIS_POS)               /*!< 0x00000002 */
#define I2C_ISR_TXIS                 I2C_ISR_TXIS_MSK                          /*!< Transmit interrupt status */
#define I2C_ISR_RXNE_POS             (2U)
#define I2C_ISR_RXNE_MSK             (0x1UL << I2C_ISR_RXNE_POS)               /*!< 0x00000004 */
#define I2C_ISR_RXNE                 I2C_ISR_RXNE_MSK                          /*!< Receive data register not empty */
#define I2C_ISR_ADDR_POS             (3U)
#define I2C_ISR_ADDR_MSK             (0x1UL << I2C_ISR_ADDR_POS)               /*!< 0x00000008 */
#define I2C_ISR_ADDR                 I2C_ISR_ADDR_MSK                          /*!< Address matched (slave mode)*/
#define I2C_ISR_NACKF_POS            (4U)
#define I2C_ISR_NACKF_MSK            (0x1UL << I2C_ISR_NACKF_POS)              /*!< 0x00000010 */
#define I2C_ISR_NACKF                I2C_ISR_NACKF_MSK                         /*!< NACK received flag */
#define I2C_ISR_STOPF_POS            (5U)
#define I2C_ISR_STOPF_MSK            (0x1UL << I2C_ISR_STOPF_POS)              /*!< 0x00000020 */
#define I2C_ISR_STOPF                I2C_ISR_STOPF_MSK                         /*!< STOP detection flag */
#define I2C_ISR_TC_POS               (6U)
#define I2C_ISR_TC_MSK               (0x1UL << I2C_ISR_TC_POS)                 /*!< 0x00000040 */
#define I2C_ISR_TC                   I2C_ISR_TC_MSK                            /*!< Transfer complete (master mode) */
#define I2C_ISR_TCR_POS              (7U)
#define I2C_ISR_TCR_MSK              (0x1UL << I2C_ISR_TCR_POS)                /*!< 0x00000080 */
#define I2C_ISR_TCR                  I2C_ISR_TCR_MSK                           /*!< Transfer complete reload */
#define I2C_ISR_BERR_POS             (8U)
#define I2C_ISR_BERR_MSK             (0x1UL << I2C_ISR_BERR_POS)               /*!< 0x00000100 */
#define I2C_ISR_BERR                 I2C_ISR_BERR_MSK                          /*!< Bus error */
#define I2C_ISR_ARLO_POS             (9U)
#define I2C_ISR_ARLO_MSK             (0x1UL << I2C_ISR_ARLO_POS)               /*!< 0x00000200 */
#define I2C_ISR_ARLO                 I2C_ISR_ARLO_MSK                          /*!< Arbitration lost */
#define I2C_ISR_OVR_POS              (10U)
#define I2C_ISR_OVR_MSK              (0x1UL << I2C_ISR_OVR_POS)                /*!< 0x00000400 */
#define I2C_ISR_OVR                  I2C_ISR_OVR_MSK                           /*!< Overrun/Underrun */
#define I2C_ISR_PECERR_POS           (11U)
#define I2C_ISR_PECERR_MSK           (0x1UL << I2C_ISR_PECERR_POS)             /*!< 0x00000800 */
#define I2C_ISR_PECERR               I2C_ISR_PECERR_MSK                        /*!< PEC error in reception */
#define I2C_ISR_TIMEOUT_POS          (12U)
#define I2C_ISR_TIMEOUT_MSK          (0x1UL << I2C_ISR_TIMEOUT_POS)            /*!< 0x00001000 */
#define I2C_ISR_TIMEOUT              I2C_ISR_TIMEOUT_MSK                       /*!< Timeout or Tlow detection flag */
#define I2C_ISR_ALERT_POS            (13U)
#define I2C_ISR_ALERT_MSK            (0x1UL << I2C_ISR_ALERT_POS)              /*!< 0x00002000 */
#define I2C_ISR_ALERT                I2C_ISR_ALERT_MSK                         /*!< SMBus alert */
#define I2C_ISR_BUSY_POS             (15U)
#define I2C_ISR_BUSY_MSK             (0x1UL << I2C_ISR_BUSY_POS)               /*!< 0x00008000 */
#define I2C_ISR_BUSY                 I2C_ISR_BUSY_MSK                          /*!< Bus busy */
#define I2C_ISR_DIR_POS              (16U)
#define I2C_ISR_DIR_MSK              (0x1UL << I2C_ISR_DIR_POS)                /*!< 0x00010000 */
#define I2C_ISR_DIR                  I2C_ISR_DIR_MSK                           /*!< Transfer direction (slave mode) */
#define I2C_ISR_ADDCODE_POS          (17U)
#define I2C_ISR_ADDCODE_MSK          (0x7FUL << I2C_ISR_ADDCODE_POS)           /*!< 0x00FE0000 */
#define I2C_ISR_ADDCODE              I2C_ISR_ADDCODE_MSK                       /*!< Address match code (slave mode) */

/******************  Bit definition for I2C_ICR register  *********************/
#define I2C_ICR_ADDRCF_POS           (3U)
#define I2C_ICR_ADDRCF_MSK           (0x1UL << I2C_ICR_ADDRCF_POS)             /*!< 0x00000008 */
#define I2C_ICR_ADDRCF               I2C_ICR_ADDRCF_MSK                        /*!< Address matched clear flag */
#define I2C_ICR_NACKCF_POS           (4U)
#define I2C_ICR_NACKCF_MSK           (0x1UL << I2C_ICR_NACKCF_POS)             /*!< 0x00000010 */
#define I2C_ICR_NACKCF               I2C_ICR_NACKCF_MSK                        /*!< NACK clear flag */
#define I2C_ICR_STOPCF_POS           (5U)
#define I2C_ICR_STOPCF_MSK           (0x1UL << I2C_ICR_STOPCF_POS)             /*!< 0x00000020 */
#define I2C_ICR_STOPCF               I2C_ICR_STOPCF_MSK                        /*!< STOP detection clear flag */
#define I2C_ICR_BERRCF_POS           (8U)
#define I2C_ICR_BERRCF_MSK           (0x1UL << I2C_ICR_BERRCF_POS)             /*!< 0x00000100 */
#define I2C_ICR_BERRCF               I2C_ICR_BERRCF_MSK                        /*!< Bus error clear flag */
#define I2C_ICR_ARLOCF_POS           (9U)
#define I2C_ICR_ARLOCF_MSK           (0x1UL << I2C_ICR_ARLOCF_POS)             /*!< 0x00000200 */
#define I2C_ICR_ARLOCF               I2C_ICR_ARLOCF_MSK                        /*!< Arbitration lost clear flag */
#define I2C_ICR_OVRCF_POS            (10U)
#define I2C_ICR_OVRCF_MSK            (0x1UL << I2C_ICR_OVRCF_POS)              /*!< 0x00000400 */
#define I2C_ICR_OVRCF                I2C_ICR_OVRCF_MSK                         /*!< Overrun/Underrun clear flag */
#define I2C_ICR_PECCF_POS            (11U)
#define I2C_ICR_PECCF_MSK            (0x1UL << I2C_ICR_PECCF_POS)              /*!< 0x00000800 */
#define I2C_ICR_PECCF                I2C_ICR_PECCF_MSK                         /*!< PAC error clear flag */
#define I2C_ICR_TIMOUTCF_POS         (12U)
#define I2C_ICR_TIMOUTCF_MSK         (0x1UL << I2C_ICR_TIMOUTCF_POS)           /*!< 0x00001000 */
#define I2C_ICR_TIMOUTCF             I2C_ICR_TIMOUTCF_MSK                      /*!< Timeout clear flag */
#define I2C_ICR_ALERTCF_POS          (13U)
#define I2C_ICR_ALERTCF_MSK          (0x1UL << I2C_ICR_ALERTCF_POS)            /*!< 0x00002000 */
#define I2C_ICR_ALERTCF              I2C_ICR_ALERTCF_MSK                       /*!< Alert clear flag */

/******************  Bit definition for I2C_PECR register  *********************/
#define I2C_PECR_PEC_POS             (0U)
#define I2C_PECR_PEC_MSK             (0xFFUL << I2C_PECR_PEC_POS)              /*!< 0x000000FF */
#define I2C_PECR_PEC                 I2C_PECR_PEC_MSK                          /*!< PEC register */

/******************  Bit definition for I2C_RXDR register  *********************/
#define I2C_RXDR_RXDATA_POS          (0U)
#define I2C_RXDR_RXDATA_MSK          (0xFFUL << I2C_RXDR_RXDATA_POS)           /*!< 0x000000FF */
#define I2C_RXDR_RXDATA              I2C_RXDR_RXDATA_MSK                       /*!< 8-bit receive data */

/******************  Bit definition for I2C_TXDR register  *********************/
#define I2C_TXDR_TXDATA_POS          (0U)
#define I2C_TXDR_TXDATA_MSK          (0xFFUL << I2C_TXDR_TXDATA_POS)           /*!< 0x000000FF */
#define I2C_TXDR_TXDATA              I2C_TXDR_TXDATA_MSK                       /*!< 8-bit transmit data */





//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- USART REGISTERS --------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief Universal Synchronous Asynchronous Receiver Transmitter
  */
typedef struct
{
  volatile uint32 CR1;         /*!< USART Control register 1,                 Address offset: 0x00  */
  volatile uint32 CR2;         /*!< USART Control register 2,                 Address offset: 0x04  */
  volatile uint32 CR3;         /*!< USART Control register 3,                 Address offset: 0x08  */
  volatile uint32 BRR;         /*!< USART Baud rate register,                 Address offset: 0x0C  */
  volatile uint32 GTPR;        /*!< USART Guard time and prescaler register,  Address offset: 0x10  */
  volatile uint32 RTOR;        /*!< USART Receiver Time Out register,         Address offset: 0x14  */
  volatile uint32 RQR;         /*!< USART Request register,                   Address offset: 0x18  */
  volatile uint32 ISR;         /*!< USART Interrupt and status register,      Address offset: 0x1C  */
  volatile uint32 ICR;         /*!< USART Interrupt flag Clear register,      Address offset: 0x20  */
  volatile uint32 RDR;         /*!< USART Receive Data register,              Address offset: 0x24  */
  volatile uint32 TDR;         /*!< USART Transmit Data register,             Address offset: 0x28  */
  volatile uint32 PRESC;       /*!< USART Prescaler register,                 Address offset: 0x2C  */
} USART_TYPEDEF;

#define USART1              (*(USART_TYPEDEF *) USART1_BASE)
#define USART2              (*(USART_TYPEDEF *) USART2_BASE)
#define USART3              (*(USART_TYPEDEF *) USART3_BASE)
#define USART4              (*(USART_TYPEDEF *) USART4_BASE)
#define USART5              (*(USART_TYPEDEF *) USART5_BASE)
#define USART6              (*(USART_TYPEDEF *) USART6_BASE)

#define LPUART1             (*(USART_TYPEDEF *) LPUART1_BASE)
#define LPUART2             (*(USART_TYPEDEF *) LPUART2_BASE)

/******************  Bit definition for USART_CR1 register  *******************/
#define USART_CR1_UE_POS             (0U)
#define USART_CR1_UE_MSK             (0x1UL << USART_CR1_UE_POS)               /*!< 0x00000001 */
#define USART_CR1_UE                 USART_CR1_UE_MSK                          /*!< USART Enable */
#define USART_CR1_UESM_POS           (1U)
#define USART_CR1_UESM_MSK           (0x1UL << USART_CR1_UESM_POS)             /*!< 0x00000002 */
#define USART_CR1_UESM               USART_CR1_UESM_MSK                        /*!< USART Enable in STOP Mode */
#define USART_CR1_RE_POS             (2U)
#define USART_CR1_RE_MSK             (0x1UL << USART_CR1_RE_POS)               /*!< 0x00000004 */
#define USART_CR1_RE                 USART_CR1_RE_MSK                          /*!< Receiver Enable */
#define USART_CR1_TE_POS             (3U)
#define USART_CR1_TE_MSK             (0x1UL << USART_CR1_TE_POS)               /*!< 0x00000008 */
#define USART_CR1_TE                 USART_CR1_TE_MSK                          /*!< Transmitter Enable */
#define USART_CR1_IDLEIE_POS         (4U)
#define USART_CR1_IDLEIE_MSK         (0x1UL << USART_CR1_IDLEIE_POS)           /*!< 0x00000010 */
#define USART_CR1_IDLEIE             USART_CR1_IDLEIE_MSK                      /*!< IDLE Interrupt Enable */
#define USART_CR1_RXNEIE_RXFNEIE_POS   (5U)
#define USART_CR1_RXNEIE_RXFNEIE_MSK   (0x1UL << USART_CR1_RXNEIE_RXFNEIE_POS) /*!< 0x00000020 */
#define USART_CR1_RXNEIE             USART_CR1_RXNEIE_RXFNEIE_MSK            /*!< RXNE/RXFIFO not empty Interrupt Enable */
#define USART_CR1_TCIE_POS           (6U)
#define USART_CR1_TCIE_MSK           (0x1UL << USART_CR1_TCIE_POS)             /*!< 0x00000040 */
#define USART_CR1_TCIE               USART_CR1_TCIE_MSK                        /*!< Transmission Complete Interrupt Enable */
#define USART_CR1_TXEIE_TXFNFIE_POS  (7U)
#define USART_CR1_TXEIE_TXFNFIE_MSK  (0x1UL << USART_CR1_TXEIE_TXFNFIE_POS)   /*!< 0x00000080 */
#define USART_CR1_TXEIE              USART_CR1_TXEIE_TXFNFIE_MSK              /*!< TXE/TXFIFO not full Interrupt Enable */
#define USART_CR1_PEIE_POS           (8U)
#define USART_CR1_PEIE_MSK           (0x1UL << USART_CR1_PEIE_POS)             /*!< 0x00000100 */
#define USART_CR1_PEIE               USART_CR1_PEIE_MSK                        /*!< PE Interrupt Enable */
#define USART_CR1_PS_POS             (9U)
#define USART_CR1_PS_MSK             (0x1UL << USART_CR1_PS_POS)               /*!< 0x00000200 */
#define USART_CR1_PS                 USART_CR1_PS_MSK                          /*!< Parity Selection */
#define USART_CR1_PCE_POS            (10U)
#define USART_CR1_PCE_MSK            (0x1UL << USART_CR1_PCE_POS)              /*!< 0x00000400 */
#define USART_CR1_PCE                USART_CR1_PCE_MSK                         /*!< Parity Control Enable */
#define USART_CR1_WAKE_POS           (11U)
#define USART_CR1_WAKE_MSK           (0x1UL << USART_CR1_WAKE_POS)             /*!< 0x00000800 */
#define USART_CR1_WAKE               USART_CR1_WAKE_MSK                        /*!< Receiver Wakeup method */
#define USART_CR1_M_POS              (12U)
#define USART_CR1_M_MSK              (0x10001UL << USART_CR1_M_POS)            /*!< 0x10001000 */
#define USART_CR1_M                  USART_CR1_M_MSK                           /*!< Word length */
#define USART_CR1_M0_POS             (12U)
#define USART_CR1_M0_MSK             (0x1UL << USART_CR1_M0_POS)               /*!< 0x00001000 */
#define USART_CR1_M0                 USART_CR1_M0_MSK                          /*!< Word length - Bit 0 */
#define USART_CR1_MME_POS            (13U)
#define USART_CR1_MME_MSK            (0x1UL << USART_CR1_MME_POS)              /*!< 0x00002000 */
#define USART_CR1_MME                USART_CR1_MME_MSK                         /*!< Mute Mode Enable */
#define USART_CR1_CMIE_POS           (14U)
#define USART_CR1_CMIE_MSK           (0x1UL << USART_CR1_CMIE_POS)             /*!< 0x00004000 */
#define USART_CR1_CMIE               USART_CR1_CMIE_MSK                        /*!< Character match interrupt enable */
#define USART_CR1_OVER8_POS          (15U)
#define USART_CR1_OVER8_MSK          (0x1UL << USART_CR1_OVER8_POS)            /*!< 0x00008000 */
#define USART_CR1_OVER8              USART_CR1_OVER8_MSK                       /*!< Oversampling by 8-bit or 16-bit mode */
#define USART_CR1_DEDT_POS           (16U)
#define USART_CR1_DEDT_MSK           (0x1FUL << USART_CR1_DEDT_POS)            /*!< 0x001F0000 */
#define USART_CR1_DEDT               USART_CR1_DEDT_MSK                        /*!< DEDT[4:0] bits (Driver Enable Deassertion Time) */
#define USART_CR1_DEDT_0             (0x01UL << USART_CR1_DEDT_POS)            /*!< 0x00010000 */
#define USART_CR1_DEDT_1             (0x02UL << USART_CR1_DEDT_POS)            /*!< 0x00020000 */
#define USART_CR1_DEDT_2             (0x04UL << USART_CR1_DEDT_POS)            /*!< 0x00040000 */
#define USART_CR1_DEDT_3             (0x08UL << USART_CR1_DEDT_POS)            /*!< 0x00080000 */
#define USART_CR1_DEDT_4             (0x10UL << USART_CR1_DEDT_POS)            /*!< 0x00100000 */
#define USART_CR1_DEAT_POS           (21U)
#define USART_CR1_DEAT_MSK           (0x1FUL << USART_CR1_DEAT_POS)            /*!< 0x03E00000 */
#define USART_CR1_DEAT               USART_CR1_DEAT_MSK                        /*!< DEAT[4:0] bits (Driver Enable Assertion Time) */
#define USART_CR1_DEAT_0             (0x01UL << USART_CR1_DEAT_POS)            /*!< 0x00200000 */
#define USART_CR1_DEAT_1             (0x02UL << USART_CR1_DEAT_POS)            /*!< 0x00400000 */
#define USART_CR1_DEAT_2             (0x04UL << USART_CR1_DEAT_POS)            /*!< 0x00800000 */
#define USART_CR1_DEAT_3             (0x08UL << USART_CR1_DEAT_POS)            /*!< 0x01000000 */
#define USART_CR1_DEAT_4             (0x10UL << USART_CR1_DEAT_POS)            /*!< 0x02000000 */
#define USART_CR1_RTOIE_POS          (26U)
#define USART_CR1_RTOIE_MSK          (0x1UL << USART_CR1_RTOIE_POS)            /*!< 0x04000000 */
#define USART_CR1_RTOIE              USART_CR1_RTOIE_MSK                       /*!< Receive Time Out interrupt enable */
#define USART_CR1_EOBIE_POS          (27U)
#define USART_CR1_EOBIE_MSK          (0x1UL << USART_CR1_EOBIE_POS)            /*!< 0x08000000 */
#define USART_CR1_EOBIE              USART_CR1_EOBIE_MSK                       /*!< End of Block interrupt enable */
#define USART_CR1_M1_POS             (28U)
#define USART_CR1_M1_MSK             (0x1UL << USART_CR1_M1_POS)               /*!< 0x10000000 */
#define USART_CR1_M1                 USART_CR1_M1_MSK                          /*!< Word length - Bit 1 */
#define USART_CR1_FIFOEN_POS         (29U)
#define USART_CR1_FIFOEN_MSK         (0x1UL << USART_CR1_FIFOEN_POS)           /*!< 0x20000000 */
#define USART_CR1_FIFOEN             USART_CR1_FIFOEN_MSK                      /*!< FIFO mode enable */
#define USART_CR1_TXFEIE_POS         (30U)
#define USART_CR1_TXFEIE_MSK         (0x1UL << USART_CR1_TXFEIE_POS)           /*!< 0x40000000 */
#define USART_CR1_TXFEIE             USART_CR1_TXFEIE_MSK                      /*!< TXFIFO empty interrupt enable */
#define USART_CR1_RXFFIE_POS         (31U)
#define USART_CR1_RXFFIE_MSK         (0x1UL << USART_CR1_RXFFIE_POS)           /*!< 0x80000000 */
#define USART_CR1_RXFFIE             USART_CR1_RXFFIE_MSK                      /*!< RXFIFO Full interrupt enable */

/******************  Bit definition for USART_CR2 register  *******************/
#define USART_CR2_SLVEN_POS          (0U)
#define USART_CR2_SLVEN_MSK          (0x1UL << USART_CR2_SLVEN_POS)            /*!< 0x00000001 */
#define USART_CR2_SLVEN              USART_CR2_SLVEN_MSK                       /*!< Synchronous Slave mode enable */
#define USART_CR2_DIS_NSS_POS        (3U)
#define USART_CR2_DIS_NSS_MSK        (0x1UL << USART_CR2_DIS_NSS_POS)          /*!< 0x00000008 */
#define USART_CR2_DIS_NSS            USART_CR2_DIS_NSS_MSK                     /*!< NSS input pin disable for SPI slave selection */
#define USART_CR2_ADDM7_POS          (4U)
#define USART_CR2_ADDM7_MSK          (0x1UL << USART_CR2_ADDM7_POS)            /*!< 0x00000010 */
#define USART_CR2_ADDM7              USART_CR2_ADDM7_MSK                       /*!< 7-bit or 4-bit Address Detection */
#define USART_CR2_LBDL_POS           (5U)
#define USART_CR2_LBDL_MSK           (0x1UL << USART_CR2_LBDL_POS)             /*!< 0x00000020 */
#define USART_CR2_LBDL               USART_CR2_LBDL_MSK                        /*!< LIN Break Detection Length */
#define USART_CR2_LBDIE_POS          (6U)
#define USART_CR2_LBDIE_MSK          (0x1UL << USART_CR2_LBDIE_POS)            /*!< 0x00000040 */
#define USART_CR2_LBDIE              USART_CR2_LBDIE_MSK                       /*!< LIN Break Detection Interrupt Enable */
#define USART_CR2_LBCL_POS           (8U)
#define USART_CR2_LBCL_MSK           (0x1UL << USART_CR2_LBCL_POS)             /*!< 0x00000100 */
#define USART_CR2_LBCL               USART_CR2_LBCL_MSK                        /*!< Last Bit Clock pulse */
#define USART_CR2_CPHA_POS           (9U)
#define USART_CR2_CPHA_MSK           (0x1UL << USART_CR2_CPHA_POS)             /*!< 0x00000200 */
#define USART_CR2_CPHA               USART_CR2_CPHA_MSK                        /*!< Clock Phase */
#define USART_CR2_CPOL_POS           (10U)
#define USART_CR2_CPOL_MSK           (0x1UL << USART_CR2_CPOL_POS)             /*!< 0x00000400 */
#define USART_CR2_CPOL               USART_CR2_CPOL_MSK                        /*!< Clock Polarity */
#define USART_CR2_CLKEN_POS          (11U)
#define USART_CR2_CLKEN_MSK          (0x1UL << USART_CR2_CLKEN_POS)            /*!< 0x00000800 */
#define USART_CR2_CLKEN              USART_CR2_CLKEN_MSK                       /*!< Clock Enable */
#define USART_CR2_STOP_POS           (12U)
#define USART_CR2_STOP_MSK           (0x3UL << USART_CR2_STOP_POS)             /*!< 0x00003000 */
#define USART_CR2_STOP               USART_CR2_STOP_MSK                        /*!< STOP[1:0] bits (STOP bits) */
#define USART_CR2_STOP_0             (0x1UL << USART_CR2_STOP_POS)             /*!< 0x00001000 */
#define USART_CR2_STOP_1             (0x2UL << USART_CR2_STOP_POS)             /*!< 0x00002000 */
#define USART_CR2_LINEN_POS          (14U)
#define USART_CR2_LINEN_MSK          (0x1UL << USART_CR2_LINEN_POS)            /*!< 0x00004000 */
#define USART_CR2_LINEN              USART_CR2_LINEN_MSK                       /*!< LIN mode enable */
#define USART_CR2_SWAP_POS           (15U)
#define USART_CR2_SWAP_MSK           (0x1UL << USART_CR2_SWAP_POS)             /*!< 0x00008000 */
#define USART_CR2_SWAP               USART_CR2_SWAP_MSK                        /*!< SWAP TX/RX pins */
#define USART_CR2_RXINV_POS          (16U)
#define USART_CR2_RXINV_MSK          (0x1UL << USART_CR2_RXINV_POS)            /*!< 0x00010000 */
#define USART_CR2_RXINV              USART_CR2_RXINV_MSK                       /*!< RX pin active level inversion */
#define USART_CR2_TXINV_POS          (17U)
#define USART_CR2_TXINV_MSK          (0x1UL << USART_CR2_TXINV_POS)            /*!< 0x00020000 */
#define USART_CR2_TXINV              USART_CR2_TXINV_MSK                       /*!< TX pin active level inversion */
#define USART_CR2_DATAINV_POS        (18U)
#define USART_CR2_DATAINV_MSK        (0x1UL << USART_CR2_DATAINV_POS)          /*!< 0x00040000 */
#define USART_CR2_DATAINV            USART_CR2_DATAINV_MSK                     /*!< Binary data inversion */
#define USART_CR2_MSBFIRST_POS       (19U)
#define USART_CR2_MSBFIRST_MSK       (0x1UL << USART_CR2_MSBFIRST_POS)         /*!< 0x00080000 */
#define USART_CR2_MSBFIRST           USART_CR2_MSBFIRST_MSK                    /*!< Most Significant Bit First */
#define USART_CR2_ABREN_POS          (20U)
#define USART_CR2_ABREN_MSK          (0x1UL << USART_CR2_ABREN_POS)            /*!< 0x00100000 */
#define USART_CR2_ABREN              USART_CR2_ABREN_MSK                       /*!< Auto Baud-Rate Enable*/
#define USART_CR2_ABRMODE_POS        (21U)
#define USART_CR2_ABRMODE_MSK        (0x3UL << USART_CR2_ABRMODE_POS)          /*!< 0x00600000 */
#define USART_CR2_ABRMODE            USART_CR2_ABRMODE_MSK                     /*!< ABRMOD[1:0] bits (Auto Baud-Rate Mode) */
#define USART_CR2_ABRMODE_0          (0x1UL << USART_CR2_ABRMODE_POS)          /*!< 0x00200000 */
#define USART_CR2_ABRMODE_1          (0x2UL << USART_CR2_ABRMODE_POS)          /*!< 0x00400000 */
#define USART_CR2_RTOEN_POS          (23U)
#define USART_CR2_RTOEN_MSK          (0x1UL << USART_CR2_RTOEN_POS)            /*!< 0x00800000 */
#define USART_CR2_RTOEN              USART_CR2_RTOEN_MSK                       /*!< Receiver Time-Out enable */
#define USART_CR2_ADD_POS            (24U)
#define USART_CR2_ADD_MSK            (0xFFUL << USART_CR2_ADD_POS)             /*!< 0xFF000000 */
#define USART_CR2_ADD                USART_CR2_ADD_MSK                         /*!< Address of the USART node */

/******************  Bit definition for USART_CR3 register  *******************/
#define USART_CR3_EIE_POS            (0U)
#define USART_CR3_EIE_MSK            (0x1UL << USART_CR3_EIE_POS)              /*!< 0x00000001 */
#define USART_CR3_EIE                USART_CR3_EIE_MSK                         /*!< Error Interrupt Enable */
#define USART_CR3_IREN_POS           (1U)
#define USART_CR3_IREN_MSK           (0x1UL << USART_CR3_IREN_POS)             /*!< 0x00000002 */
#define USART_CR3_IREN               USART_CR3_IREN_MSK                        /*!< IrDA mode Enable */
#define USART_CR3_IRLP_POS           (2U)
#define USART_CR3_IRLP_MSK           (0x1UL << USART_CR3_IRLP_POS)             /*!< 0x00000004 */
#define USART_CR3_IRLP               USART_CR3_IRLP_MSK                        /*!< IrDA Low-Power */
#define USART_CR3_HDSEL_POS          (3U)
#define USART_CR3_HDSEL_MSK          (0x1UL << USART_CR3_HDSEL_POS)            /*!< 0x00000008 */
#define USART_CR3_HDSEL              USART_CR3_HDSEL_MSK                       /*!< Half-Duplex Selection */
#define USART_CR3_NACK_POS           (4U)
#define USART_CR3_NACK_MSK           (0x1UL << USART_CR3_NACK_POS)             /*!< 0x00000010 */
#define USART_CR3_NACK               USART_CR3_NACK_MSK                        /*!< SmartCard NACK enable */
#define USART_CR3_SCEN_POS           (5U)
#define USART_CR3_SCEN_MSK           (0x1UL << USART_CR3_SCEN_POS)             /*!< 0x00000020 */
#define USART_CR3_SCEN               USART_CR3_SCEN_MSK                        /*!< SmartCard mode enable */
#define USART_CR3_DMAR_POS           (6U)
#define USART_CR3_DMAR_MSK           (0x1UL << USART_CR3_DMAR_POS)             /*!< 0x00000040 */
#define USART_CR3_DMAR               USART_CR3_DMAR_MSK                        /*!< DMA Enable Receiver */
#define USART_CR3_DMAT_POS           (7U)
#define USART_CR3_DMAT_MSK           (0x1UL << USART_CR3_DMAT_POS)             /*!< 0x00000080 */
#define USART_CR3_DMAT               USART_CR3_DMAT_MSK                        /*!< DMA Enable Transmitter */
#define USART_CR3_RTSE_POS           (8U)
#define USART_CR3_RTSE_MSK           (0x1UL << USART_CR3_RTSE_POS)             /*!< 0x00000100 */
#define USART_CR3_RTSE               USART_CR3_RTSE_MSK                        /*!< RTS Enable */
#define USART_CR3_CTSE_POS           (9U)
#define USART_CR3_CTSE_MSK           (0x1UL << USART_CR3_CTSE_POS)             /*!< 0x00000200 */
#define USART_CR3_CTSE               USART_CR3_CTSE_MSK                        /*!< CTS Enable */
#define USART_CR3_CTSIE_POS          (10U)
#define USART_CR3_CTSIE_MSK          (0x1UL << USART_CR3_CTSIE_POS)            /*!< 0x00000400 */
#define USART_CR3_CTSIE              USART_CR3_CTSIE_MSK                       /*!< CTS Interrupt Enable */
#define USART_CR3_ONEBIT_POS         (11U)
#define USART_CR3_ONEBIT_MSK         (0x1UL << USART_CR3_ONEBIT_POS)           /*!< 0x00000800 */
#define USART_CR3_ONEBIT             USART_CR3_ONEBIT_MSK                      /*!< One sample bit method enable */
#define USART_CR3_OVRDIS_POS         (12U)
#define USART_CR3_OVRDIS_MSK         (0x1UL << USART_CR3_OVRDIS_POS)           /*!< 0x00001000 */
#define USART_CR3_OVRDIS             USART_CR3_OVRDIS_MSK                      /*!< Overrun Disable */
#define USART_CR3_DDRE_POS           (13U)
#define USART_CR3_DDRE_MSK           (0x1UL << USART_CR3_DDRE_POS)             /*!< 0x00002000 */
#define USART_CR3_DDRE               USART_CR3_DDRE_MSK                        /*!< DMA Disable on Reception Error */
#define USART_CR3_DEM_POS            (14U)
#define USART_CR3_DEM_MSK            (0x1UL << USART_CR3_DEM_POS)              /*!< 0x00004000 */
#define USART_CR3_DEM                USART_CR3_DEM_MSK                         /*!< Driver Enable Mode */
#define USART_CR3_DEP_POS            (15U)
#define USART_CR3_DEP_MSK            (0x1UL << USART_CR3_DEP_POS)              /*!< 0x00008000 */
#define USART_CR3_DEP                USART_CR3_DEP_MSK                         /*!< Driver Enable Polarity Selection */
#define USART_CR3_SCARCNT_POS        (17U)
#define USART_CR3_SCARCNT_MSK        (0x7UL << USART_CR3_SCARCNT_POS)          /*!< 0x000E0000 */
#define USART_CR3_SCARCNT            USART_CR3_SCARCNT_MSK                     /*!< SCARCNT[2:0] bits (SmartCard Auto-Retry Count) */
#define USART_CR3_SCARCNT_0          (0x1UL << USART_CR3_SCARCNT_POS)          /*!< 0x00020000 */
#define USART_CR3_SCARCNT_1          (0x2UL << USART_CR3_SCARCNT_POS)          /*!< 0x00040000 */
#define USART_CR3_SCARCNT_2          (0x4UL << USART_CR3_SCARCNT_POS)          /*!< 0x00080000 */
#define USART_CR3_WUS_POS            (20U)
#define USART_CR3_WUS_MSK            (0x3UL << USART_CR3_WUS_POS)              /*!< 0x00300000 */
#define USART_CR3_WUS                USART_CR3_WUS_MSK                         /*!< WUS[1:0] bits (Wake UP Interrupt Flag Selection) */
#define USART_CR3_WUS_0              (0x1UL << USART_CR3_WUS_POS)              /*!< 0x00100000 */
#define USART_CR3_WUS_1              (0x2UL << USART_CR3_WUS_POS)              /*!< 0x00200000 */
#define USART_CR3_WUFIE_POS          (22U)
#define USART_CR3_WUFIE_MSK          (0x1UL << USART_CR3_WUFIE_POS)            /*!< 0x00400000 */
#define USART_CR3_WUFIE              USART_CR3_WUFIE_MSK                       /*!< Wake Up Interrupt Enable */
#define USART_CR3_TXFTIE_POS         (23U)
#define USART_CR3_TXFTIE_MSK         (0x1UL << USART_CR3_TXFTIE_POS)           /*!< 0x00800000 */
#define USART_CR3_TXFTIE             USART_CR3_TXFTIE_MSK                      /*!< TXFIFO threshold interrupt enable */
#define USART_CR3_TCBGTIE_POS        (24U)
#define USART_CR3_TCBGTIE_MSK        (0x1UL << USART_CR3_TCBGTIE_POS)          /*!< 0x01000000 */
#define USART_CR3_TCBGTIE            USART_CR3_TCBGTIE_MSK                     /*!< Transmission Complete Before Guard Time Interrupt Enable */
#define USART_CR3_RXFTCFG_POS        (25U)
#define USART_CR3_RXFTCFG_MSK        (0x7UL << USART_CR3_RXFTCFG_POS)          /*!< 0x0E000000 */
#define USART_CR3_RXFTCFG            USART_CR3_RXFTCFG_MSK                     /*!< RXFIFO FIFO threshold configuration */
#define USART_CR3_RXFTCFG_0          (0x1UL << USART_CR3_RXFTCFG_POS)          /*!< 0x02000000 */
#define USART_CR3_RXFTCFG_1          (0x2UL << USART_CR3_RXFTCFG_POS)          /*!< 0x04000000 */
#define USART_CR3_RXFTCFG_2          (0x4UL << USART_CR3_RXFTCFG_POS)          /*!< 0x08000000 */
#define USART_CR3_RXFTIE_POS         (28U)
#define USART_CR3_RXFTIE_MSK         (0x1UL << USART_CR3_RXFTIE_POS)           /*!< 0x10000000 */
#define USART_CR3_RXFTIE             USART_CR3_RXFTIE_MSK                      /*!< RXFIFO threshold interrupt enable */
#define USART_CR3_TXFTCFG_POS        (29U)
#define USART_CR3_TXFTCFG_MSK        (0x7UL << USART_CR3_TXFTCFG_POS)          /*!< 0xE0000000 */
#define USART_CR3_TXFTCFG            USART_CR3_TXFTCFG_MSK                     /*!< TXFIFO threshold configuration */
#define USART_CR3_TXFTCFG_0          (0x1UL << USART_CR3_TXFTCFG_POS)          /*!< 0x20000000 */
#define USART_CR3_TXFTCFG_1          (0x2UL << USART_CR3_TXFTCFG_POS)          /*!< 0x40000000 */
#define USART_CR3_TXFTCFG_2          (0x4UL << USART_CR3_TXFTCFG_POS)          /*!< 0x80000000 */

/******************  Bit definition for USART_BRR register  *******************/
#define USART_BRR_LPUART_POS         (0U)
#define USART_BRR_LPUART_MSK         (0xFFFFFUL << USART_BRR_LPUART_POS)       /*!< 0x000FFFFF */
#define USART_BRR_LPUART             USART_BRR_LPUART_MSK                      /*!< LPUART Baud rate register [19:0] */
#define USART_BRR_BRR                ((uint16)0xFFFF)                        /*!< USART  Baud rate register [15:0] */

/******************  Bit definition for USART_GTPR register  ******************/
#define USART_GTPR_PSC_POS           (0U)
#define USART_GTPR_PSC_MSK           (0xFFUL << USART_GTPR_PSC_POS)            /*!< 0x000000FF */
#define USART_GTPR_PSC               USART_GTPR_PSC_MSK                        /*!< PSC[7:0] bits (Prescaler value) */
#define USART_GTPR_GT_POS            (8U)
#define USART_GTPR_GT_MSK            (0xFFUL << USART_GTPR_GT_POS)             /*!< 0x0000FF00 */
#define USART_GTPR_GT                USART_GTPR_GT_MSK                         /*!< GT[7:0] bits (Guard time value) */

/*******************  Bit definition for USART_RTOR register  *****************/
#define USART_RTOR_RTO_POS           (0U)
#define USART_RTOR_RTO_MSK           (0xFFFFFFUL << USART_RTOR_RTO_POS)        /*!< 0x00FFFFFF */
#define USART_RTOR_RTO               USART_RTOR_RTO_MSK                        /*!< Receiver Time Out Value */
#define USART_RTOR_BLEN_POS          (24U)
#define USART_RTOR_BLEN_MSK          (0xFFUL << USART_RTOR_BLEN_POS)           /*!< 0xFF000000 */
#define USART_RTOR_BLEN              USART_RTOR_BLEN_MSK                       /*!< Block Length */

/*******************  Bit definition for USART_RQR register  ******************/
#define USART_RQR_ABRRQ        ((uint16)0x0001)                              /*!< Auto-Baud Rate Request */
#define USART_RQR_SBKRQ        ((uint16)0x0002)                              /*!< Send Break Request */
#define USART_RQR_MMRQ         ((uint16)0x0004)                              /*!< Mute Mode Request */
#define USART_RQR_RXFRQ        ((uint16)0x0008)                              /*!< Receive Data flush Request */
#define USART_RQR_TXFRQ        ((uint16)0x0010)                              /*!< Transmit data flush Request */

/*******************  Bit definition for USART_ISR register  ******************/
#define USART_ISR_PE_POS             (0U)
#define USART_ISR_PE_MSK             (0x1UL << USART_ISR_PE_POS)               /*!< 0x00000001 */
#define USART_ISR_PE                 USART_ISR_PE_MSK                          /*!< Parity Error */
#define USART_ISR_FE_POS             (1U)
#define USART_ISR_FE_MSK             (0x1UL << USART_ISR_FE_POS)               /*!< 0x00000002 */
#define USART_ISR_FE                 USART_ISR_FE_MSK                          /*!< Framing Error */
#define USART_ISR_NE_POS             (2U)
#define USART_ISR_NE_MSK             (0x1UL << USART_ISR_NE_POS)               /*!< 0x00000004 */
#define USART_ISR_NE                 USART_ISR_NE_MSK                          /*!< Noise detected Flag */
#define USART_ISR_ORE_POS            (3U)
#define USART_ISR_ORE_MSK            (0x1UL << USART_ISR_ORE_POS)              /*!< 0x00000008 */
#define USART_ISR_ORE                USART_ISR_ORE_MSK                         /*!< OverRun Error */
#define USART_ISR_IDLE_POS           (4U)
#define USART_ISR_IDLE_MSK           (0x1UL << USART_ISR_IDLE_POS)             /*!< 0x00000010 */
#define USART_ISR_IDLE               USART_ISR_IDLE_MSK                        /*!< IDLE line detected */
#define USART_ISR_RXNE_RXFNE_POS     (5U)
#define USART_ISR_RXNE_RXFNE_MSK     (0x1UL << USART_ISR_RXNE_RXFNE_POS)       /*!< 0x00000020 */
#define USART_ISR_RXNE               USART_ISR_RXNE_RXFNE_MSK                  /*!< Read Data Register Not Empty/RXFIFO Not Empty */
#define USART_ISR_TC_POS             (6U)
#define USART_ISR_TC_MSK             (0x1UL << USART_ISR_TC_POS)               /*!< 0x00000040 */
#define USART_ISR_TC                 USART_ISR_TC_MSK                          /*!< Transmission Complete */
#define USART_ISR_TXE_TXFNF_POS      (7U)
#define USART_ISR_TXE_TXFNF_MSK      (0x1UL << USART_ISR_TXE_TXFNF_POS)        /*!< 0x00000080 */
#define USART_ISR_TXE                USART_ISR_TXE_TXFNF_MSK                   /*!< Transmit Data Register Empty/TXFIFO Not Full */
#define USART_ISR_LBDF_POS           (8U)
#define USART_ISR_LBDF_MSK           (0x1UL << USART_ISR_LBDF_POS)             /*!< 0x00000100 */
#define USART_ISR_LBDF               USART_ISR_LBDF_MSK                        /*!< LIN Break Detection Flag */
#define USART_ISR_CTSIF_POS          (9U)
#define USART_ISR_CTSIF_MSK          (0x1UL << USART_ISR_CTSIF_POS)            /*!< 0x00000200 */
#define USART_ISR_CTSIF              USART_ISR_CTSIF_MSK                       /*!< CTS interrupt flag */
#define USART_ISR_CTS_POS            (10U)
#define USART_ISR_CTS_MSK            (0x1UL << USART_ISR_CTS_POS)              /*!< 0x00000400 */
#define USART_ISR_CTS                USART_ISR_CTS_MSK                         /*!< CTS flag */
#define USART_ISR_RTOF_POS           (11U)
#define USART_ISR_RTOF_MSK           (0x1UL << USART_ISR_RTOF_POS)             /*!< 0x00000800 */
#define USART_ISR_RTOF               USART_ISR_RTOF_MSK                        /*!< Receiver Time Out */
#define USART_ISR_EOBF_POS           (12U)
#define USART_ISR_EOBF_MSK           (0x1UL << USART_ISR_EOBF_POS)             /*!< 0x00001000 */
#define USART_ISR_EOBF               USART_ISR_EOBF_MSK                        /*!< End Of Block Flag */
#define USART_ISR_UDR_POS            (13U)
#define USART_ISR_UDR_MSK            (0x1UL << USART_ISR_UDR_POS)              /*!< 0x00002000 */
#define USART_ISR_UDR                 USART_ISR_UDR_MSK                        /*!< SPI Slave Underrun Error Flag */
#define USART_ISR_ABRE_POS           (14U)
#define USART_ISR_ABRE_MSK           (0x1UL << USART_ISR_ABRE_POS)             /*!< 0x00004000 */
#define USART_ISR_ABRE               USART_ISR_ABRE_MSK                        /*!< Auto-Baud Rate Error */
#define USART_ISR_ABRF_POS           (15U)
#define USART_ISR_ABRF_MSK           (0x1UL << USART_ISR_ABRF_POS)             /*!< 0x00008000 */
#define USART_ISR_ABRF               USART_ISR_ABRF_MSK                        /*!< Auto-Baud Rate Flag */
#define USART_ISR_BUSY_POS           (16U)
#define USART_ISR_BUSY_MSK           (0x1UL << USART_ISR_BUSY_POS)             /*!< 0x00010000 */
#define USART_ISR_BUSY               USART_ISR_BUSY_MSK                        /*!< Busy Flag */
#define USART_ISR_CMF_POS            (17U)
#define USART_ISR_CMF_MSK            (0x1UL << USART_ISR_CMF_POS)              /*!< 0x00020000 */
#define USART_ISR_CMF                USART_ISR_CMF_MSK                         /*!< Character Match Flag */
#define USART_ISR_SBKF_POS           (18U)
#define USART_ISR_SBKF_MSK           (0x1UL << USART_ISR_SBKF_POS)             /*!< 0x00040000 */
#define USART_ISR_SBKF               USART_ISR_SBKF_MSK                        /*!< Send Break Flag */
#define USART_ISR_RWU_POS            (19U)
#define USART_ISR_RWU_MSK            (0x1UL << USART_ISR_RWU_POS)              /*!< 0x00080000 */
#define USART_ISR_RWU                USART_ISR_RWU_MSK                         /*!< Receive Wake Up from mute mode Flag */
#define USART_ISR_WUF_POS            (20U)
#define USART_ISR_WUF_MSK            (0x1UL << USART_ISR_WUF_POS)              /*!< 0x00100000 */
#define USART_ISR_WUF                USART_ISR_WUF_MSK                         /*!< Wake Up from stop mode Flag */
#define USART_ISR_TEACK_POS          (21U)
#define USART_ISR_TEACK_MSK          (0x1UL << USART_ISR_TEACK_POS)            /*!< 0x00200000 */
#define USART_ISR_TEACK              USART_ISR_TEACK_MSK                       /*!< Transmit Enable Acknowledge Flag */
#define USART_ISR_REACK_POS          (22U)
#define USART_ISR_REACK_MSK          (0x1UL << USART_ISR_REACK_POS)            /*!< 0x00400000 */
#define USART_ISR_REACK              USART_ISR_REACK_MSK                       /*!< Receive Enable Acknowledge Flag */
#define USART_ISR_TXFE_POS           (23U)
#define USART_ISR_TXFE_MSK           (0x1UL << USART_ISR_TXFE_POS)             /*!< 0x00800000 */
#define USART_ISR_TXFE               USART_ISR_TXFE_MSK                        /*!< TXFIFO Empty Flag */
#define USART_ISR_RXFF_POS           (24U)
#define USART_ISR_RXFF_MSK           (0x1UL << USART_ISR_RXFF_POS)             /*!< 0x01000000 */
#define USART_ISR_RXFF               USART_ISR_RXFF_MSK                        /*!< RXFIFO Full Flag */
#define USART_ISR_TCBGT_POS          (25U)
#define USART_ISR_TCBGT_MSK          (0x1UL << USART_ISR_TCBGT_POS)            /*!< 0x02000000 */
#define USART_ISR_TCBGT              USART_ISR_TCBGT_MSK                       /*!< Transmission Complete Before Guard Time Completion Flag */
#define USART_ISR_RXFT_POS           (26U)
#define USART_ISR_RXFT_MSK           (0x1UL << USART_ISR_RXFT_POS)             /*!< 0x04000000 */
#define USART_ISR_RXFT               USART_ISR_RXFT_MSK                        /*!< RXFIFO Threshold Flag */
#define USART_ISR_TXFT_POS           (27U)
#define USART_ISR_TXFT_MSK           (0x1UL << USART_ISR_TXFT_POS)             /*!< 0x08000000 */
#define USART_ISR_TXFT               USART_ISR_TXFT_MSK                        /*!< TXFIFO Threshold Flag */

/*******************  Bit definition for USART_ICR register  ******************/
#define USART_ICR_PECF_POS           (0U)
#define USART_ICR_PECF_MSK           (0x1UL << USART_ICR_PECF_POS)             /*!< 0x00000001 */
#define USART_ICR_PECF               USART_ICR_PECF_MSK                        /*!< Parity Error Clear Flag */
#define USART_ICR_FECF_POS           (1U)
#define USART_ICR_FECF_MSK           (0x1UL << USART_ICR_FECF_POS)             /*!< 0x00000002 */
#define USART_ICR_FECF               USART_ICR_FECF_MSK                        /*!< Framing Error Clear Flag */
#define USART_ICR_NECF_POS           (2U)
#define USART_ICR_NECF_MSK           (0x1UL << USART_ICR_NECF_POS)             /*!< 0x00000004 */
#define USART_ICR_NECF               USART_ICR_NECF_MSK                        /*!< Noise Error detected Clear Flag */
#define USART_ICR_ORECF_POS          (3U)
#define USART_ICR_ORECF_MSK          (0x1UL << USART_ICR_ORECF_POS)            /*!< 0x00000008 */
#define USART_ICR_ORECF              USART_ICR_ORECF_MSK                       /*!< OverRun Error Clear Flag */
#define USART_ICR_IDLECF_POS         (4U)
#define USART_ICR_IDLECF_MSK         (0x1UL << USART_ICR_IDLECF_POS)           /*!< 0x00000010 */
#define USART_ICR_IDLECF             USART_ICR_IDLECF_MSK                      /*!< IDLE line detected Clear Flag */
#define USART_ICR_TXFECF_POS         (5U)
#define USART_ICR_TXFECF_MSK         (0x1UL << USART_ICR_TXFECF_POS)           /*!< 0x00000020 */
#define USART_ICR_TXFECF             USART_ICR_TXFECF_MSK                      /*!< TXFIFO Empty Clear Flag */
#define USART_ICR_TCCF_POS           (6U)
#define USART_ICR_TCCF_MSK           (0x1UL << USART_ICR_TCCF_POS)             /*!< 0x00000040 */
#define USART_ICR_TCCF               USART_ICR_TCCF_MSK                        /*!< Transmission Complete Clear Flag */
#define USART_ICR_TCBGTCF_POS        (7U)
#define USART_ICR_TCBGTCF_MSK        (0x1UL << USART_ICR_TCBGTCF_POS)          /*!< 0x00000080 */
#define USART_ICR_TCBGTCF            USART_ICR_TCBGTCF_MSK                     /*!< Transmission Complete Before Guard Time Clear Flag */
#define USART_ICR_LBDCF_POS          (8U)
#define USART_ICR_LBDCF_MSK          (0x1UL << USART_ICR_LBDCF_POS)            /*!< 0x00000100 */
#define USART_ICR_LBDCF              USART_ICR_LBDCF_MSK                       /*!< LIN Break Detection Clear Flag */
#define USART_ICR_CTSCF_POS          (9U)
#define USART_ICR_CTSCF_MSK          (0x1UL << USART_ICR_CTSCF_POS)            /*!< 0x00000200 */
#define USART_ICR_CTSCF              USART_ICR_CTSCF_MSK                       /*!< CTS Interrupt Clear Flag */
#define USART_ICR_RTOCF_POS          (11U)
#define USART_ICR_RTOCF_MSK          (0x1UL << USART_ICR_RTOCF_POS)            /*!< 0x00000800 */
#define USART_ICR_RTOCF              USART_ICR_RTOCF_MSK                       /*!< Receiver Time Out Clear Flag */
#define USART_ICR_EOBCF_POS          (12U)
#define USART_ICR_EOBCF_MSK          (0x1UL << USART_ICR_EOBCF_POS)            /*!< 0x00001000 */
#define USART_ICR_EOBCF              USART_ICR_EOBCF_MSK                       /*!< End Of Block Clear Flag */
#define USART_ICR_UDRCF_POS          (13U)
#define USART_ICR_UDRCF_MSK          (0x1UL << USART_ICR_UDRCF_POS)            /*!< 0x00002000 */
#define USART_ICR_UDRCF              USART_ICR_UDRCF_MSK                       /*!< SPI Slave Underrun Clear Flag */
#define USART_ICR_CMCF_POS           (17U)
#define USART_ICR_CMCF_MSK           (0x1UL << USART_ICR_CMCF_POS)             /*!< 0x00020000 */
#define USART_ICR_CMCF               USART_ICR_CMCF_MSK                        /*!< Character Match Clear Flag */
#define USART_ICR_WUCF_POS           (20U)
#define USART_ICR_WUCF_MSK           (0x1UL << USART_ICR_WUCF_POS)             /*!< 0x00100000 */
#define USART_ICR_WUCF               USART_ICR_WUCF_MSK                        /*!< Wake Up from stop mode Clear Flag */

/*******************  Bit definition for USART_RDR register  ******************/
#define USART_RDR_RDR_POS             (0U)
#define USART_RDR_RDR_MSK             (0x1FFUL << USART_RDR_RDR_POS)           /*!< 0x000001FF */
#define USART_RDR_RDR                 USART_RDR_RDR_MSK                        /*!< RDR[8:0] bits (Receive Data value) */

/*******************  Bit definition for USART_TDR register  ******************/
#define USART_TDR_TDR_POS             (0U)
#define USART_TDR_TDR_MSK             (0x1FFUL << USART_TDR_TDR_POS)           /*!< 0x000001FF */
#define USART_TDR_TDR                 USART_TDR_TDR_MSK                        /*!< TDR[8:0] bits (Transmit Data value) */

/*******************  Bit definition for USART_PRESC register  ****************/
#define USART_PRESC_PRESCALER_POS    (0U)
#define USART_PRESC_PRESCALER_MSK    (0xFUL << USART_PRESC_PRESCALER_POS)      /*!< 0x0000000F */
#define USART_PRESC_PRESCALER        USART_PRESC_PRESCALER_MSK                 /*!< PRESCALER[3:0] bits (Clock prescaler) */
#define USART_PRESC_PRESCALER_0      (0x1UL << USART_PRESC_PRESCALER_POS)      /*!< 0x00000001 */
#define USART_PRESC_PRESCALER_1      (0x2UL << USART_PRESC_PRESCALER_POS)      /*!< 0x00000002 */
#define USART_PRESC_PRESCALER_2      (0x4UL << USART_PRESC_PRESCALER_POS)      /*!< 0x00000004 */
#define USART_PRESC_PRESCALER_3      (0x8UL << USART_PRESC_PRESCALER_POS)      /*!< 0x00000008 */

#define UART_STOPBITS_0_5                    USART_CR2_STOP_0                     /*!< UART frame with 0.5 stop bit  */
#define UART_STOPBITS_1                      0x00000000U                           /*!< UART frame with 1 stop bit    */
#define UART_STOPBITS_1_5                    (USART_CR2_STOP_0 | USART_CR2_STOP_1) /*!< UART frame with 1.5 stop bits */
#define UART_STOPBITS_2                      USART_CR2_STOP_1                     /*!< UART frame with 2 stop bits   */

#define UART_CR1_PARITY_NONE                 0x00000000U                        /*!< No parity   */
#define UART_CR1_PARITY_EVEN                 USART_CR1_PCE                      /*!< Even parity */
#define UART_CR1_PARITY_ODD                  (USART_CR1_PCE | USART_CR1_PS)     /*!< Odd parity  */

//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- DAC REGISTERS ----------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
typedef struct
{
  volatile uint32 CR;          /*!< DAC control register,                                    Address offset: 0x00 */
  volatile uint32 SWTRIGR;     /*!< DAC software trigger register,                           Address offset: 0x04 */
  volatile uint32 DHR12R1;     /*!< DAC channel1 12-bit right-aligned data holding register, Address offset: 0x08 */
  volatile uint32 DHR12L1;     /*!< DAC channel1 12-bit left aligned data holding register,  Address offset: 0x0C */
  volatile uint32 DHR8R1;      /*!< DAC channel1 8-bit right aligned data holding register,  Address offset: 0x10 */
  volatile uint32 DHR12R2;     /*!< DAC channel2 12-bit right aligned data holding register, Address offset: 0x14 */
  volatile uint32 DHR12L2;     /*!< DAC channel2 12-bit left aligned data holding register,  Address offset: 0x18 */
  volatile uint32 DHR8R2;      /*!< DAC channel2 8-bit right-aligned data holding register,  Address offset: 0x1C */
  volatile uint32 DHR12RD;     /*!< Dual DAC 12-bit right-aligned data holding register,     Address offset: 0x20 */
  volatile uint32 DHR12LD;     /*!< DUAL DAC 12-bit left aligned data holding register,      Address offset: 0x24 */
  volatile uint32 DHR8RD;      /*!< DUAL DAC 8-bit right aligned data holding register,      Address offset: 0x28 */
  volatile uint32 DOR1;        /*!< DAC channel1 data output register,                       Address offset: 0x2C */
  volatile uint32 DOR2;        /*!< DAC channel2 data output register,                       Address offset: 0x30 */
  volatile uint32 SR;          /*!< DAC status register,                                     Address offset: 0x34 */
  volatile uint32 CCR;         /*!< DAC calibration control register,                        Address offset: 0x38 */
  volatile uint32 MCR;         /*!< DAC mode control register,                               Address offset: 0x3C */
  volatile uint32 SHSR1;       /*!< DAC Sample and Hold sample time register 1,              Address offset: 0x40 */
  volatile uint32 SHSR2;       /*!< DAC Sample and Hold sample time register 2,              Address offset: 0x44 */
  volatile uint32 SHHR;        /*!< DAC Sample and Hold hold time register,                  Address offset: 0x48 */
  volatile uint32 SHRR;        /*!< DAC Sample and Hold refresh time register,               Address offset: 0x4C */
} DAC_TYPEDEF;

#define DAC                 (*(DAC_TYPEDEF *) DAC_BASE) /* Kept for legacy purpose */
#define DAC1                (*(DAC_TYPEDEF *) DAC1_BASE)

/********************  Bit definition for DAC_CR register  ********************/
#define DAC_CR_EN1_POS              (0U)
#define DAC_CR_EN1_MSK              (0x1UL << DAC_CR_EN1_POS)                  /*!< 0x00000001 */
#define DAC_CR_EN1                  DAC_CR_EN1_MSK                             /*!<DAC channel1 enable */
#define DAC_CR_TEN1_POS             (1U)
#define DAC_CR_TEN1_MSK             (0x1UL << DAC_CR_TEN1_POS)                 /*!< 0x00000002 */
#define DAC_CR_TEN1                 DAC_CR_TEN1_MSK                            /*!<DAC channel1 Trigger enable */

#define DAC_CR_TSEL1_POS            (2U)
#define DAC_CR_TSEL1_MSK            (0xFUL << DAC_CR_TSEL1_POS)                /*!< 0x0000003C */
#define DAC_CR_TSEL1                DAC_CR_TSEL1_MSK                           /*!<TSEL1[3:0] (DAC channel1 Trigger selection) */
#define DAC_CR_TSEL1_0              (0x1UL << DAC_CR_TSEL1_POS)                /*!< 0x00000004 */
#define DAC_CR_TSEL1_1              (0x2UL << DAC_CR_TSEL1_POS)                /*!< 0x00000008 */
#define DAC_CR_TSEL1_2              (0x4UL << DAC_CR_TSEL1_POS)                /*!< 0x00000010 */
#define DAC_CR_TSEL1_3              (0x8UL << DAC_CR_TSEL1_POS)                /*!< 0x00000020 */

#define DAC_CR_WAVE1_POS            (6U)
#define DAC_CR_WAVE1_MSK            (0x3UL << DAC_CR_WAVE1_POS)                /*!< 0x000000C0 */
#define DAC_CR_WAVE1                DAC_CR_WAVE1_MSK                           /*!<WAVE1[1:0] (DAC channel1 noise/triangle wave generation enable) */
#define DAC_CR_WAVE1_0              (0x1UL << DAC_CR_WAVE1_POS)                /*!< 0x00000040 */
#define DAC_CR_WAVE1_1              (0x2UL << DAC_CR_WAVE1_POS)                /*!< 0x00000080 */

#define DAC_CR_MAMP1_POS            (8U)
#define DAC_CR_MAMP1_MSK            (0xFUL << DAC_CR_MAMP1_POS)                /*!< 0x00000F00 */
#define DAC_CR_MAMP1                DAC_CR_MAMP1_MSK                           /*!<MAMP1[3:0] (DAC channel1 Mask/Amplitude selector) */
#define DAC_CR_MAMP1_0              (0x1UL << DAC_CR_MAMP1_POS)                /*!< 0x00000100 */
#define DAC_CR_MAMP1_1              (0x2UL << DAC_CR_MAMP1_POS)                /*!< 0x00000200 */
#define DAC_CR_MAMP1_2              (0x4UL << DAC_CR_MAMP1_POS)                /*!< 0x00000400 */
#define DAC_CR_MAMP1_3              (0x8UL << DAC_CR_MAMP1_POS)                /*!< 0x00000800 */

#define DAC_CR_DMAEN1_POS           (12U)
#define DAC_CR_DMAEN1_MSK           (0x1UL << DAC_CR_DMAEN1_POS)               /*!< 0x00001000 */
#define DAC_CR_DMAEN1               DAC_CR_DMAEN1_MSK                          /*!<DAC channel1 DMA enable */
#define DAC_CR_DMAUDRIE1_POS        (13U)
#define DAC_CR_DMAUDRIE1_MSK        (0x1UL << DAC_CR_DMAUDRIE1_POS)            /*!< 0x00002000 */
#define DAC_CR_DMAUDRIE1            DAC_CR_DMAUDRIE1_MSK                       /*!<DAC channel 1 DMA underrun interrupt enable  >*/
#define DAC_CR_CEN1_POS             (14U)
#define DAC_CR_CEN1_MSK             (0x1UL << DAC_CR_CEN1_POS)                 /*!< 0x00004000 */
#define DAC_CR_CEN1                 DAC_CR_CEN1_MSK                            /*!<DAC channel 1 calibration enable >*/

#define DAC_CR_EN2_POS              (16U)
#define DAC_CR_EN2_MSK              (0x1UL << DAC_CR_EN2_POS)                  /*!< 0x00010000 */
#define DAC_CR_EN2                  DAC_CR_EN2_MSK                             /*!<DAC channel2 enable */
#define DAC_CR_TEN2_POS             (17U)
#define DAC_CR_TEN2_MSK             (0x1UL << DAC_CR_TEN2_POS)                 /*!< 0x00020000 */
#define DAC_CR_TEN2                 DAC_CR_TEN2_MSK                            /*!<DAC channel2 Trigger enable */

#define DAC_CR_TSEL2_POS            (18U)
#define DAC_CR_TSEL2_MSK            (0xFUL << DAC_CR_TSEL2_POS)                /*!< 0x003C0000 */
#define DAC_CR_TSEL2                DAC_CR_TSEL2_MSK                           /*!<TSEL2[2:0] (DAC channel2 Trigger selection) */
#define DAC_CR_TSEL2_0              (0x1UL << DAC_CR_TSEL2_POS)                /*!< 0x00040000 */
#define DAC_CR_TSEL2_1              (0x2UL << DAC_CR_TSEL2_POS)                /*!< 0x00080000 */
#define DAC_CR_TSEL2_2              (0x4UL << DAC_CR_TSEL2_POS)                /*!< 0x00100000 */
#define DAC_CR_TSEL2_3              (0x8UL << DAC_CR_TSEL2_POS)                /*!< 0x00200000 */

#define DAC_CR_WAVE2_POS            (22U)
#define DAC_CR_WAVE2_MSK            (0x3UL << DAC_CR_WAVE2_POS)                /*!< 0x00C00000 */
#define DAC_CR_WAVE2                DAC_CR_WAVE2_MSK                           /*!<WAVE2[1:0] (DAC channel2 noise/triangle wave generation enable) */
#define DAC_CR_WAVE2_0              (0x1UL << DAC_CR_WAVE2_POS)                /*!< 0x00400000 */
#define DAC_CR_WAVE2_1              (0x2UL << DAC_CR_WAVE2_POS)                /*!< 0x00800000 */

#define DAC_CR_MAMP2_POS            (24U)
#define DAC_CR_MAMP2_MSK            (0xFUL << DAC_CR_MAMP2_POS)                /*!< 0x0F000000 */
#define DAC_CR_MAMP2                DAC_CR_MAMP2_MSK                           /*!<MAMP2[3:0] (DAC channel2 Mask/Amplitude selector) */
#define DAC_CR_MAMP2_0              (0x1UL << DAC_CR_MAMP2_POS)                /*!< 0x01000000 */
#define DAC_CR_MAMP2_1              (0x2UL << DAC_CR_MAMP2_POS)                /*!< 0x02000000 */
#define DAC_CR_MAMP2_2              (0x4UL << DAC_CR_MAMP2_POS)                /*!< 0x04000000 */
#define DAC_CR_MAMP2_3              (0x8UL << DAC_CR_MAMP2_POS)                /*!< 0x08000000 */

#define DAC_CR_DMAEN2_POS           (28U)
#define DAC_CR_DMAEN2_MSK           (0x1UL << DAC_CR_DMAEN2_POS)               /*!< 0x10000000 */
#define DAC_CR_DMAEN2               DAC_CR_DMAEN2_MSK                          /*!<DAC channel2 DMA enabled */
#define DAC_CR_DMAUDRIE2_POS        (29U)
#define DAC_CR_DMAUDRIE2_MSK        (0x1UL << DAC_CR_DMAUDRIE2_POS)            /*!< 0x20000000 */
#define DAC_CR_DMAUDRIE2            DAC_CR_DMAUDRIE2_MSK                       /*!<DAC channel2 DMA underrun interrupt enable  >*/
#define DAC_CR_CEN2_POS             (30U)
#define DAC_CR_CEN2_MSK             (0x1UL << DAC_CR_CEN2_POS)                 /*!< 0x40000000 */
#define DAC_CR_CEN2                 DAC_CR_CEN2_MSK                            /*!<DAC channel2 calibration enable >*/

/*****************  Bit definition for DAC_SWTRIGR register  ******************/
#define DAC_SWTRIGR_SWTRIG1_POS     (0U)
#define DAC_SWTRIGR_SWTRIG1_MSK     (0x1UL << DAC_SWTRIGR_SWTRIG1_POS)         /*!< 0x00000001 */
#define DAC_SWTRIGR_SWTRIG1         DAC_SWTRIGR_SWTRIG1_MSK                    /*!<DAC channel1 software trigger */
#define DAC_SWTRIGR_SWTRIG2_POS     (1U)
#define DAC_SWTRIGR_SWTRIG2_MSK     (0x1UL << DAC_SWTRIGR_SWTRIG2_POS)         /*!< 0x00000002 */
#define DAC_SWTRIGR_SWTRIG2         DAC_SWTRIGR_SWTRIG2_MSK                    /*!<DAC channel2 software trigger */

/*****************  Bit definition for DAC_DHR12R1 register  ******************/
#define DAC_DHR12R1_DACC1DHR_POS    (0U)
#define DAC_DHR12R1_DACC1DHR_MSK    (0xFFFUL << DAC_DHR12R1_DACC1DHR_POS)      /*!< 0x00000FFF */
#define DAC_DHR12R1_DACC1DHR        DAC_DHR12R1_DACC1DHR_MSK                   /*!<DAC channel1 12-bit Right aligned data */

/*****************  Bit definition for DAC_DHR12L1 register  ******************/
#define DAC_DHR12L1_DACC1DHR_POS    (4U)
#define DAC_DHR12L1_DACC1DHR_MSK    (0xFFFUL << DAC_DHR12L1_DACC1DHR_POS)      /*!< 0x0000FFF0 */
#define DAC_DHR12L1_DACC1DHR        DAC_DHR12L1_DACC1DHR_MSK                   /*!<DAC channel1 12-bit Left aligned data */

/******************  Bit definition for DAC_DHR8R1 register  ******************/
#define DAC_DHR8R1_DACC1DHR_POS     (0U)
#define DAC_DHR8R1_DACC1DHR_MSK     (0xFFUL << DAC_DHR8R1_DACC1DHR_POS)        /*!< 0x000000FF */
#define DAC_DHR8R1_DACC1DHR         DAC_DHR8R1_DACC1DHR_MSK                    /*!<DAC channel1 8-bit Right aligned data */

/*****************  Bit definition for DAC_DHR12R2 register  ******************/
#define DAC_DHR12R2_DACC2DHR_POS    (0U)
#define DAC_DHR12R2_DACC2DHR_MSK    (0xFFFUL << DAC_DHR12R2_DACC2DHR_POS)      /*!< 0x00000FFF */
#define DAC_DHR12R2_DACC2DHR        DAC_DHR12R2_DACC2DHR_MSK                   /*!<DAC channel2 12-bit Right aligned data */

/*****************  Bit definition for DAC_DHR12L2 register  ******************/
#define DAC_DHR12L2_DACC2DHR_POS    (4U)
#define DAC_DHR12L2_DACC2DHR_MSK    (0xFFFUL << DAC_DHR12L2_DACC2DHR_POS)      /*!< 0x0000FFF0 */
#define DAC_DHR12L2_DACC2DHR        DAC_DHR12L2_DACC2DHR_MSK                   /*!<DAC channel2 12-bit Left aligned data */

/******************  Bit definition for DAC_DHR8R2 register  ******************/
#define DAC_DHR8R2_DACC2DHR_POS     (0U)
#define DAC_DHR8R2_DACC2DHR_MSK     (0xFFUL << DAC_DHR8R2_DACC2DHR_POS)        /*!< 0x000000FF */
#define DAC_DHR8R2_DACC2DHR         DAC_DHR8R2_DACC2DHR_MSK                    /*!<DAC channel2 8-bit Right aligned data */

/*****************  Bit definition for DAC_DHR12RD register  ******************/
#define DAC_DHR12RD_DACC1DHR_POS    (0U)
#define DAC_DHR12RD_DACC1DHR_MSK    (0xFFFUL << DAC_DHR12RD_DACC1DHR_POS)      /*!< 0x00000FFF */
#define DAC_DHR12RD_DACC1DHR        DAC_DHR12RD_DACC1DHR_MSK                   /*!<DAC channel1 12-bit Right aligned data */
#define DAC_DHR12RD_DACC2DHR_POS    (16U)
#define DAC_DHR12RD_DACC2DHR_MSK    (0xFFFUL << DAC_DHR12RD_DACC2DHR_POS)      /*!< 0x0FFF0000 */
#define DAC_DHR12RD_DACC2DHR        DAC_DHR12RD_DACC2DHR_MSK                   /*!<DAC channel2 12-bit Right aligned data */

/*****************  Bit definition for DAC_DHR12LD register  ******************/
#define DAC_DHR12LD_DACC1DHR_POS    (4U)
#define DAC_DHR12LD_DACC1DHR_MSK    (0xFFFUL << DAC_DHR12LD_DACC1DHR_POS)      /*!< 0x0000FFF0 */
#define DAC_DHR12LD_DACC1DHR        DAC_DHR12LD_DACC1DHR_MSK                   /*!<DAC channel1 12-bit Left aligned data */
#define DAC_DHR12LD_DACC2DHR_POS    (20U)
#define DAC_DHR12LD_DACC2DHR_MSK    (0xFFFUL << DAC_DHR12LD_DACC2DHR_POS)      /*!< 0xFFF00000 */
#define DAC_DHR12LD_DACC2DHR        DAC_DHR12LD_DACC2DHR_MSK                   /*!<DAC channel2 12-bit Left aligned data */

/******************  Bit definition for DAC_DHR8RD register  ******************/
#define DAC_DHR8RD_DACC1DHR_POS     (0U)
#define DAC_DHR8RD_DACC1DHR_MSK     (0xFFUL << DAC_DHR8RD_DACC1DHR_POS)        /*!< 0x000000FF */
#define DAC_DHR8RD_DACC1DHR         DAC_DHR8RD_DACC1DHR_MSK                    /*!<DAC channel1 8-bit Right aligned data */
#define DAC_DHR8RD_DACC2DHR_POS     (8U)
#define DAC_DHR8RD_DACC2DHR_MSK     (0xFFUL << DAC_DHR8RD_DACC2DHR_POS)        /*!< 0x0000FF00 */
#define DAC_DHR8RD_DACC2DHR         DAC_DHR8RD_DACC2DHR_MSK                    /*!<DAC channel2 8-bit Right aligned data */

/*******************  Bit definition for DAC_DOR1 register  *******************/
#define DAC_DOR1_DACC1DOR_POS       (0U)
#define DAC_DOR1_DACC1DOR_MSK       (0xFFFUL << DAC_DOR1_DACC1DOR_POS)         /*!< 0x00000FFF */
#define DAC_DOR1_DACC1DOR           DAC_DOR1_DACC1DOR_MSK                      /*!<DAC channel1 data output */

/*******************  Bit definition for DAC_DOR2 register  *******************/
#define DAC_DOR2_DACC2DOR_POS       (0U)
#define DAC_DOR2_DACC2DOR_MSK       (0xFFFUL << DAC_DOR2_DACC2DOR_POS)         /*!< 0x00000FFF */
#define DAC_DOR2_DACC2DOR           DAC_DOR2_DACC2DOR_MSK                      /*!<DAC channel2 data output */

/********************  Bit definition for DAC_SR register  ********************/
#define DAC_SR_DMAUDR1_POS          (13U)
#define DAC_SR_DMAUDR1_MSK          (0x1UL << DAC_SR_DMAUDR1_POS)              /*!< 0x00002000 */
#define DAC_SR_DMAUDR1              DAC_SR_DMAUDR1_MSK                         /*!<DAC channel1 DMA underrun flag */
#define DAC_SR_CAL_FLAG1_POS        (14U)
#define DAC_SR_CAL_FLAG1_MSK        (0x1UL << DAC_SR_CAL_FLAG1_POS)            /*!< 0x00004000 */
#define DAC_SR_CAL_FLAG1            DAC_SR_CAL_FLAG1_MSK                       /*!<DAC channel1 calibration offset status */
#define DAC_SR_BWST1_POS            (15U)
#define DAC_SR_BWST1_MSK            (0x1UL << DAC_SR_BWST1_POS)                /*!< 0x00008000 */
#define DAC_SR_BWST1                DAC_SR_BWST1_MSK                           /*!<DAC channel1 busy writing sample time flag */

#define DAC_SR_DMAUDR2_POS          (29U)
#define DAC_SR_DMAUDR2_MSK          (0x1UL << DAC_SR_DMAUDR2_POS)              /*!< 0x20000000 */
#define DAC_SR_DMAUDR2              DAC_SR_DMAUDR2_MSK                         /*!<DAC channel2 DMA underrun flag */
#define DAC_SR_CAL_FLAG2_POS        (30U)
#define DAC_SR_CAL_FLAG2_MSK        (0x1UL << DAC_SR_CAL_FLAG2_POS)            /*!< 0x40000000 */
#define DAC_SR_CAL_FLAG2            DAC_SR_CAL_FLAG2_MSK                       /*!<DAC channel2 calibration offset status */
#define DAC_SR_BWST2_POS            (31U)
#define DAC_SR_BWST2_MSK            (0x1UL << DAC_SR_BWST2_POS)                /*!< 0x80000000 */
#define DAC_SR_BWST2                DAC_SR_BWST2_MSK                           /*!<DAC channel2 busy writing sample time flag */

/*******************  Bit definition for DAC_CCR register  ********************/
#define DAC_CCR_OTRIM1_POS          (0U)
#define DAC_CCR_OTRIM1_MSK          (0x1FUL << DAC_CCR_OTRIM1_POS)             /*!< 0x0000001F */
#define DAC_CCR_OTRIM1              DAC_CCR_OTRIM1_MSK                         /*!<DAC channel1 offset trimming value */
#define DAC_CCR_OTRIM2_POS          (16U)
#define DAC_CCR_OTRIM2_MSK          (0x1FUL << DAC_CCR_OTRIM2_POS)             /*!< 0x001F0000 */
#define DAC_CCR_OTRIM2              DAC_CCR_OTRIM2_MSK                         /*!<DAC channel2 offset trimming value */

/*******************  Bit definition for DAC_MCR register  *******************/
#define DAC_MCR_MODE1_POS           (0U)
#define DAC_MCR_MODE1_MSK           (0x7UL << DAC_MCR_MODE1_POS)               /*!< 0x00000007 */
#define DAC_MCR_MODE1               DAC_MCR_MODE1_MSK                          /*!<MODE1[2:0] (DAC channel1 mode) */
#define DAC_MCR_MODE1_0             (0x1UL << DAC_MCR_MODE1_POS)               /*!< 0x00000001 */
#define DAC_MCR_MODE1_1             (0x2UL << DAC_MCR_MODE1_POS)               /*!< 0x00000002 */
#define DAC_MCR_MODE1_2             (0x4UL << DAC_MCR_MODE1_POS)               /*!< 0x00000004 */

#define DAC_MCR_MODE2_POS           (16U)
#define DAC_MCR_MODE2_MSK           (0x7UL << DAC_MCR_MODE2_POS)               /*!< 0x00070000 */
#define DAC_MCR_MODE2               DAC_MCR_MODE2_MSK                          /*!<MODE2[2:0] (DAC channel2 mode) */
#define DAC_MCR_MODE2_0             (0x1UL << DAC_MCR_MODE2_POS)               /*!< 0x00010000 */
#define DAC_MCR_MODE2_1             (0x2UL << DAC_MCR_MODE2_POS)               /*!< 0x00020000 */
#define DAC_MCR_MODE2_2             (0x4UL << DAC_MCR_MODE2_POS)               /*!< 0x00040000 */

/******************  Bit definition for DAC_SHSR1 register  ******************/
#define DAC_SHSR1_TSAMPLE1_POS      (0U)
#define DAC_SHSR1_TSAMPLE1_MSK      (0x3FFUL << DAC_SHSR1_TSAMPLE1_POS)        /*!< 0x000003FF */
#define DAC_SHSR1_TSAMPLE1          DAC_SHSR1_TSAMPLE1_MSK                     /*!<DAC channel1 sample time */

/******************  Bit definition for DAC_SHSR2 register  ******************/
#define DAC_SHSR2_TSAMPLE2_POS      (0U)
#define DAC_SHSR2_TSAMPLE2_MSK      (0x3FFUL << DAC_SHSR2_TSAMPLE2_POS)        /*!< 0x000003FF */
#define DAC_SHSR2_TSAMPLE2          DAC_SHSR2_TSAMPLE2_MSK                     /*!<DAC channel2 sample time */

/******************  Bit definition for DAC_SHHR register  ******************/
#define DAC_SHHR_THOLD1_POS         (0U)
#define DAC_SHHR_THOLD1_MSK         (0x3FFUL << DAC_SHHR_THOLD1_POS)           /*!< 0x000003FF */
#define DAC_SHHR_THOLD1             DAC_SHHR_THOLD1_MSK                        /*!<DAC channel1 hold time */
#define DAC_SHHR_THOLD2_POS         (16U)
#define DAC_SHHR_THOLD2_MSK         (0x3FFUL << DAC_SHHR_THOLD2_POS)           /*!< 0x03FF0000 */
#define DAC_SHHR_THOLD2             DAC_SHHR_THOLD2_MSK                        /*!<DAC channel2 hold time */

/******************  Bit definition for DAC_SHRR register  ******************/
#define DAC_SHRR_TREFRESH1_POS      (0U)
#define DAC_SHRR_TREFRESH1_MSK      (0xFFUL << DAC_SHRR_TREFRESH1_POS)         /*!< 0x000000FF */
#define DAC_SHRR_TREFRESH1          DAC_SHRR_TREFRESH1_MSK                     /*!<DAC channel1 refresh time */
#define DAC_SHRR_TREFRESH2_POS      (16U)
#define DAC_SHRR_TREFRESH2_MSK      (0xFFUL << DAC_SHRR_TREFRESH2_POS)         /*!< 0x00FF0000 */
#define DAC_SHRR_TREFRESH2          DAC_SHRR_TREFRESH2_MSK                     /*!<DAC channel2 refresh time */





//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- VREFBUF REGISTERS ------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief VREFBUF
  */
typedef struct
{
  volatile uint32 CSR;         /*!< VREFBUF control and status register,         Address offset: 0x00 */
  volatile uint32 CCR;         /*!< VREFBUF calibration and control register,    Address offset: 0x04 */
} VREFBUF_TYPEDEF;

#define VREFBUF             (*(VREFBUF_TYPEDEF *) VREFBUF_BASE)

/*******************  Bit definition for VREFBUF_CSR register  ****************/
#define VREFBUF_CSR_ENVR_POS    (0U)
#define VREFBUF_CSR_ENVR_MSK    (0x1UL << VREFBUF_CSR_ENVR_POS)                /*!< 0x00000001 */
#define VREFBUF_CSR_ENVR        VREFBUF_CSR_ENVR_MSK                           /*!<Voltage reference buffer enable */
#define VREFBUF_CSR_HIZ_POS     (1U)
#define VREFBUF_CSR_HIZ_MSK     (0x1UL << VREFBUF_CSR_HIZ_POS)                 /*!< 0x00000002 */
#define VREFBUF_CSR_HIZ         VREFBUF_CSR_HIZ_MSK                            /*!<High impedance mode             */
#define VREFBUF_CSR_VRS_POS     (2U)
#define VREFBUF_CSR_VRS_MSK     (0x1UL << VREFBUF_CSR_VRS_POS)                 /*!< 0x00000004 */
#define VREFBUF_CSR_VRS         VREFBUF_CSR_VRS_MSK                            /*!<Voltage reference scale         */
#define VREFBUF_CSR_VRR_POS     (3U)
#define VREFBUF_CSR_VRR_MSK     (0x1UL << VREFBUF_CSR_VRR_POS)                 /*!< 0x00000008 */
#define VREFBUF_CSR_VRR         VREFBUF_CSR_VRR_MSK                            /*!<Voltage reference buffer ready  */

/*******************  Bit definition for VREFBUF_CCR register  ******************/
#define VREFBUF_CCR_TRIM_POS    (0U)
#define VREFBUF_CCR_TRIM_MSK    (0x3FUL << VREFBUF_CCR_TRIM_POS)               /*!< 0x0000003F */
#define VREFBUF_CCR_TRIM        VREFBUF_CCR_TRIM_MSK                           /*!<TRIM[5:0] bits (Trimming code)  */





//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- CRC REGISTERS ----------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief CRC calculation unit
  */
typedef struct
{
  volatile uint32 DR;             /*!< CRC Data register,                         Address offset: 0x00 */
  volatile uint32 IDR;            /*!< CRC Independent data register,             Address offset: 0x04 */
  volatile uint32 CR;             /*!< CRC Control register,                      Address offset: 0x08 */
       uint32 RESERVED1;      /*!< Reserved,                                                  0x0C */
  volatile uint32 INIT;           /*!< Initial CRC value register,                Address offset: 0x10 */
  volatile uint32 POL;            /*!< CRC polynomial register,                   Address offset: 0x14 */
} CRC_TYPEDEF;

#define CRC                 (*(CRC_TYPEDEF *) CRC_BASE)

/*******************  Bit definition for CRC_DR register  *********************/
#define CRC_DR_DR_POS            (0U)
#define CRC_DR_DR_MSK            (0xFFFFFFFFUL << CRC_DR_DR_POS)                /*!< 0xFFFFFFFF */
#define CRC_DR_DR                CRC_DR_DR_MSK                                  /*!< Data register bits */

/*******************  Bit definition for CRC_IDR register  ********************/
#define CRC_IDR_IDR_POS          (0U)
#define CRC_IDR_IDR_MSK          (0xFFFFFFFFUL << CRC_IDR_IDR_POS)              /*!< 0xFFFFFFFF */
#define CRC_IDR_IDR              CRC_IDR_IDR_MSK                                /*!< General-purpose 32-bits data register bits */

/********************  Bit definition for CRC_CR register  ********************/
#define CRC_CR_RESET_POS         (0U)
#define CRC_CR_RESET_MSK         (0x1UL << CRC_CR_RESET_POS)                    /*!< 0x00000001 */
#define CRC_CR_RESET             CRC_CR_RESET_MSK                               /*!< RESET the CRC computation unit bit */
#define CRC_CR_POLYSIZE_POS      (3U)
#define CRC_CR_POLYSIZE_MSK      (0x3UL << CRC_CR_POLYSIZE_POS)                 /*!< 0x00000018 */
#define CRC_CR_POLYSIZE          CRC_CR_POLYSIZE_MSK                            /*!< Polynomial size bits */
#define CRC_CR_POLYSIZE_0        (0x1UL << CRC_CR_POLYSIZE_POS)                 /*!< 0x00000008 */
#define CRC_CR_POLYSIZE_1        (0x2UL << CRC_CR_POLYSIZE_POS)                 /*!< 0x00000010 */
#define CRC_CR_REV_IN_POS        (5U)
#define CRC_CR_REV_IN_MSK        (0x3UL << CRC_CR_REV_IN_POS)                   /*!< 0x00000060 */
#define CRC_CR_REV_IN            CRC_CR_REV_IN_MSK                              /*!< REV_IN Reverse Input Data bits */
#define CRC_CR_REV_IN_0          (0x1UL << CRC_CR_REV_IN_POS)                   /*!< 0x00000020 */
#define CRC_CR_REV_IN_1          (0x2UL << CRC_CR_REV_IN_POS)                   /*!< 0x00000040 */
#define CRC_CR_REV_OUT_POS       (7U)
#define CRC_CR_REV_OUT_MSK       (0x1UL << CRC_CR_REV_OUT_POS)                  /*!< 0x00000080 */
#define CRC_CR_REV_OUT           CRC_CR_REV_OUT_MSK                             /*!< REV_OUT Reverse Output Data bits */

/*******************  Bit definition for CRC_INIT register  *******************/
#define CRC_INIT_INIT_POS        (0U)
#define CRC_INIT_INIT_MSK        (0xFFFFFFFFUL << CRC_INIT_INIT_POS)            /*!< 0xFFFFFFFF */
#define CRC_INIT_INIT            CRC_INIT_INIT_MSK                              /*!< Initial CRC value bits */

/*******************  Bit definition for CRC_POL register  ********************/
#define CRC_POL_POL_POS          (0U)
#define CRC_POL_POL_MSK          (0xFFFFFFFFUL << CRC_POL_POL_POS)              /*!< 0xFFFFFFFF */
#define CRC_POL_POL              CRC_POL_POL_MSK                                /*!< Coefficients of the polynomial */





//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- COMPARATOR REGISTERS ---------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief Comparator
  */
typedef struct
{
  volatile uint32 CSR;         /*!< COMP control and status register,                                                 Address offset: 0x00 */
} COMP_TYPEDEF;

typedef struct
{
  volatile uint32 CSR_ODD;    /*!< COMP control and status register located in register of comparator instance odd, used for bits common to several COMP instances, Address offset: 0x00 */
  volatile uint32 CSR_EVEN;   /*!< COMP control and status register located in register of comparator instance even, used for bits common to several COMP instances, Address offset: 0x04 */
} COMP_COMMON_TYPEDEF;

#define COMP1               (*(COMP_TYPEDEF *) COMP1_BASE)
#define COMP2               (*(COMP_TYPEDEF *) COMP2_BASE)
#define COMP12_COMMON       (*(COMP_COMMON_TYPEDEF *) COMP1_BASE)

/**********************  Bit definition for COMP_CSR register  ****************/
#define COMP_CSR_EN_POS            (0U)
#define COMP_CSR_EN_MSK            (0x1UL << COMP_CSR_EN_POS)                  /*!< 0x00000001 */
#define COMP_CSR_EN                COMP_CSR_EN_MSK                             /*!< Comparator enable */

#define COMP_CSR_INMSEL_POS        (4U)
#define COMP_CSR_INMSEL_MSK        (0xFUL << COMP_CSR_INMSEL_POS)              /*!< 0x000000F0 */
#define COMP_CSR_INMSEL            COMP_CSR_INMSEL_MSK                         /*!< Comparator input minus selection */
#define COMP_CSR_INMSEL_0          (0x1UL << COMP_CSR_INMSEL_POS)              /*!< 0x00000010 */
#define COMP_CSR_INMSEL_1          (0x2UL << COMP_CSR_INMSEL_POS)              /*!< 0x00000020 */
#define COMP_CSR_INMSEL_2          (0x4UL << COMP_CSR_INMSEL_POS)              /*!< 0x00000040 */
#define COMP_CSR_INMSEL_3          (0x8UL << COMP_CSR_INMSEL_POS)              /*!< 0x00000080 */

#define COMP_CSR_INPSEL_POS        (8U)
#define COMP_CSR_INPSEL_MSK        (0x3UL << COMP_CSR_INPSEL_POS)              /*!< 0x00000300 */
#define COMP_CSR_INPSEL            COMP_CSR_INPSEL_MSK                         /*!< Comparator plus minus selection */
#define COMP_CSR_INPSEL_0          (0x1UL << COMP_CSR_INPSEL_POS)              /*!< 0x00000100 */
#define COMP_CSR_INPSEL_1          (0x2UL << COMP_CSR_INPSEL_POS)              /*!< 0x00000200 */

#define COMP_CSR_WINMODE_POS       (11U)
#define COMP_CSR_WINMODE_MSK       (0x1UL << COMP_CSR_WINMODE_POS)             /*!< 0x00000800 */
#define COMP_CSR_WINMODE           COMP_CSR_WINMODE_MSK                        /*!< Pair of comparators window mode. Bit intended to be used with COMP common instance (COMP_Common_TYPEDEF) */
#define COMP_CSR_WINOUT_POS        (14U)
#define COMP_CSR_WINOUT_MSK        (0x1UL << COMP_CSR_WINOUT_POS)              /*!< 0x00004000 */
#define COMP_CSR_WINOUT            COMP_CSR_WINOUT_MSK                         /*!< Pair of comparators window output level. Bit intended to be used with COMP common instance (COMP_Common_TYPEDEF) */

#define COMP_CSR_POLARITY_POS      (15U)
#define COMP_CSR_POLARITY_MSK      (0x1UL << COMP_CSR_POLARITY_POS)            /*!< 0x00008000 */
#define COMP_CSR_POLARITY          COMP_CSR_POLARITY_MSK                       /*!< Comparator output polarity */

#define COMP_CSR_HYST_POS          (16U)
#define COMP_CSR_HYST_MSK          (0x3UL << COMP_CSR_HYST_POS)                /*!< 0x00030000 */
#define COMP_CSR_HYST              COMP_CSR_HYST_MSK                           /*!< Comparator input hysteresis */
#define COMP_CSR_HYST_0            (0x1UL << COMP_CSR_HYST_POS)                /*!< 0x00010000 */
#define COMP_CSR_HYST_1            (0x2UL << COMP_CSR_HYST_POS)                /*!< 0x00020000 */

#define COMP_CSR_PWRMODE_POS       (18U)
#define COMP_CSR_PWRMODE_MSK       (0x3UL << COMP_CSR_PWRMODE_POS)             /*!< 0x000C0000 */
#define COMP_CSR_PWRMODE           COMP_CSR_PWRMODE_MSK                        /*!< Comparator power mode */
#define COMP_CSR_PWRMODE_0         (0x1UL << COMP_CSR_PWRMODE_POS)             /*!< 0x00040000 */
#define COMP_CSR_PWRMODE_1         (0x2UL << COMP_CSR_PWRMODE_POS)             /*!< 0x00080000 */

#define COMP_CSR_BLANKING_POS      (20U)
#define COMP_CSR_BLANKING_MSK      (0x1FUL << COMP_CSR_BLANKING_POS)           /*!< 0x01F00000 */
#define COMP_CSR_BLANKING          COMP_CSR_BLANKING_MSK                       /*!< Comparator blanking source */
#define COMP_CSR_BLANKING_0        (0x01UL << COMP_CSR_BLANKING_POS)           /*!< 0x00100000 */
#define COMP_CSR_BLANKING_1        (0x02UL << COMP_CSR_BLANKING_POS)           /*!< 0x00200000 */
#define COMP_CSR_BLANKING_2        (0x04UL << COMP_CSR_BLANKING_POS)           /*!< 0x00400000 */
#define COMP_CSR_BLANKING_3        (0x08UL << COMP_CSR_BLANKING_POS)           /*!< 0x00800000 */
#define COMP_CSR_BLANKING_4        (0x10UL << COMP_CSR_BLANKING_POS)           /*!< 0x01000000 */

#define COMP_CSR_VALUE_POS         (30U)
#define COMP_CSR_VALUE_MSK         (0x1UL << COMP_CSR_VALUE_POS)               /*!< 0x40000000 */
#define COMP_CSR_VALUE             COMP_CSR_VALUE_MSK                          /*!< Comparator output level */

#define COMP_CSR_LOCK_POS          (31U)
#define COMP_CSR_LOCK_MSK          (0x1UL << COMP_CSR_LOCK_POS)                /*!< 0x80000000 */
#define COMP_CSR_LOCK              COMP_CSR_LOCK_MSK                           /*!< Comparator lock */





//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- HDMI CEC REGISTERS --------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief HDMI-CEC
  */
typedef struct
{
  volatile uint32 CR;           /*!< CEC control register,                                       Address offset:0x00 */
  volatile uint32 CFGR;         /*!< CEC configuration register,                                 Address offset:0x04 */
  volatile uint32 TXDR;         /*!< CEC Tx data register ,                                      Address offset:0x08 */
  volatile uint32 RXDR;         /*!< CEC Rx Data Register,                                       Address offset:0x0C */
  volatile uint32 ISR;          /*!< CEC Interrupt and Status Register,                          Address offset:0x10 */
  volatile uint32 IER;          /*!< CEC interrupt enable register,                              Address offset:0x14 */
}CEC_TYPEDEF;


#define CEC                 (*(CEC_TYPEDEF *) CEC_BASE)

/*******************  Bit definition for CEC_CR register  *********************/
#define CEC_CR_CECEN_POS         (0U)
#define CEC_CR_CECEN_MSK         (0x1UL << CEC_CR_CECEN_POS)                    /*!< 0x00000001 */
#define CEC_CR_CECEN             CEC_CR_CECEN_MSK                               /*!< CEC Enable                         */
#define CEC_CR_TXSOM_POS         (1U)
#define CEC_CR_TXSOM_MSK         (0x1UL << CEC_CR_TXSOM_POS)                    /*!< 0x00000002 */
#define CEC_CR_TXSOM             CEC_CR_TXSOM_MSK                               /*!< CEC Tx Start Of Message            */
#define CEC_CR_TXEOM_POS         (2U)
#define CEC_CR_TXEOM_MSK         (0x1UL << CEC_CR_TXEOM_POS)                    /*!< 0x00000004 */
#define CEC_CR_TXEOM             CEC_CR_TXEOM_MSK                               /*!< CEC Tx End Of Message              */

/*******************  Bit definition for CEC_CFGR register  *******************/
#define CEC_CFGR_SFT_POS         (0U)
#define CEC_CFGR_SFT_MSK         (0x7UL << CEC_CFGR_SFT_POS)                    /*!< 0x00000007 */
#define CEC_CFGR_SFT             CEC_CFGR_SFT_MSK                               /*!< CEC Signal Free Time               */
#define CEC_CFGR_RXTOL_POS       (3U)
#define CEC_CFGR_RXTOL_MSK       (0x1UL << CEC_CFGR_RXTOL_POS)                  /*!< 0x00000008 */
#define CEC_CFGR_RXTOL           CEC_CFGR_RXTOL_MSK                             /*!< CEC Tolerance                      */
#define CEC_CFGR_BRESTP_POS      (4U)
#define CEC_CFGR_BRESTP_MSK      (0x1UL << CEC_CFGR_BRESTP_POS)                 /*!< 0x00000010 */
#define CEC_CFGR_BRESTP          CEC_CFGR_BRESTP_MSK                            /*!< CEC Rx Stop                        */
#define CEC_CFGR_BREGEN_POS      (5U)
#define CEC_CFGR_BREGEN_MSK      (0x1UL << CEC_CFGR_BREGEN_POS)                 /*!< 0x00000020 */
#define CEC_CFGR_BREGEN          CEC_CFGR_BREGEN_MSK                            /*!< CEC Bit Rising Error generation    */
#define CEC_CFGR_LBPEGEN_POS     (6U)
#define CEC_CFGR_LBPEGEN_MSK     (0x1UL << CEC_CFGR_LBPEGEN_POS)                /*!< 0x00000040 */
#define CEC_CFGR_LBPEGEN         CEC_CFGR_LBPEGEN_MSK                           /*!< CEC Long Bit Period Error gener.   */
#define CEC_CFGR_BRDNOGEN_POS    (7U)
#define CEC_CFGR_BRDNOGEN_MSK    (0x1UL << CEC_CFGR_BRDNOGEN_POS)               /*!< 0x00000080 */
#define CEC_CFGR_BRDNOGEN        CEC_CFGR_BRDNOGEN_MSK                          /*!< CEC Broadcast No Error generation  */
#define CEC_CFGR_SFTOPT_POS      (8U)
#define CEC_CFGR_SFTOPT_MSK      (0x1UL << CEC_CFGR_SFTOPT_POS)                 /*!< 0x00000100 */
#define CEC_CFGR_SFTOPT          CEC_CFGR_SFTOPT_MSK                            /*!< CEC Signal Free Time optional      */
#define CEC_CFGR_OAR_POS         (16U)
#define CEC_CFGR_OAR_MSK         (0x7FFFUL << CEC_CFGR_OAR_POS)                 /*!< 0x7FFF0000 */
#define CEC_CFGR_OAR             CEC_CFGR_OAR_MSK                               /*!< CEC Own Address                    */
#define CEC_CFGR_LSTN_POS        (31U)
#define CEC_CFGR_LSTN_MSK        (0x1UL << CEC_CFGR_LSTN_POS)                   /*!< 0x80000000 */
#define CEC_CFGR_LSTN            CEC_CFGR_LSTN_MSK                              /*!< CEC Listen mode                    */

/*******************  Bit definition for CEC_TXDR register  *******************/
#define CEC_TXDR_TXD_POS         (0U)
#define CEC_TXDR_TXD_MSK         (0xFFUL << CEC_TXDR_TXD_POS)                   /*!< 0x000000FF */
#define CEC_TXDR_TXD             CEC_TXDR_TXD_MSK                               /*!< CEC Tx Data                        */

/*******************  Bit definition for CEC_RXDR register  *******************/
#define CEC_RXDR_RXD_POS         (0U)
#define CEC_RXDR_RXD_MSK         (0xFFUL << CEC_RXDR_RXD_POS)                   /*!< 0x000000FF */
#define CEC_RXDR_RXD             CEC_RXDR_RXD_MSK                               /*!< CEC Rx Data                        */

/*******************  Bit definition for CEC_ISR register  ********************/
#define CEC_ISR_RXBR_POS         (0U)
#define CEC_ISR_RXBR_MSK         (0x1UL << CEC_ISR_RXBR_POS)                    /*!< 0x00000001 */
#define CEC_ISR_RXBR             CEC_ISR_RXBR_MSK                               /*!< CEC Rx-Byte Received                   */
#define CEC_ISR_RXEND_POS        (1U)
#define CEC_ISR_RXEND_MSK        (0x1UL << CEC_ISR_RXEND_POS)                   /*!< 0x00000002 */
#define CEC_ISR_RXEND            CEC_ISR_RXEND_MSK                              /*!< CEC End Of Reception                   */
#define CEC_ISR_RXOVR_POS        (2U)
#define CEC_ISR_RXOVR_MSK        (0x1UL << CEC_ISR_RXOVR_POS)                   /*!< 0x00000004 */
#define CEC_ISR_RXOVR            CEC_ISR_RXOVR_MSK                              /*!< CEC Rx-Overrun                         */
#define CEC_ISR_BRE_POS          (3U)
#define CEC_ISR_BRE_MSK          (0x1UL << CEC_ISR_BRE_POS)                     /*!< 0x00000008 */
#define CEC_ISR_BRE              CEC_ISR_BRE_MSK                                /*!< CEC Rx Bit Rising Error                */
#define CEC_ISR_SBPE_POS         (4U)
#define CEC_ISR_SBPE_MSK         (0x1UL << CEC_ISR_SBPE_POS)                    /*!< 0x00000010 */
#define CEC_ISR_SBPE             CEC_ISR_SBPE_MSK                               /*!< CEC Rx Short Bit period Error          */
#define CEC_ISR_LBPE_POS         (5U)
#define CEC_ISR_LBPE_MSK         (0x1UL << CEC_ISR_LBPE_POS)                    /*!< 0x00000020 */
#define CEC_ISR_LBPE             CEC_ISR_LBPE_MSK                               /*!< CEC Rx Long Bit period Error           */
#define CEC_ISR_RXACKE_POS       (6U)
#define CEC_ISR_RXACKE_MSK       (0x1UL << CEC_ISR_RXACKE_POS)                  /*!< 0x00000040 */
#define CEC_ISR_RXACKE           CEC_ISR_RXACKE_MSK                             /*!< CEC Rx Missing Acknowledge             */
#define CEC_ISR_ARBLST_POS       (7U)
#define CEC_ISR_ARBLST_MSK       (0x1UL << CEC_ISR_ARBLST_POS)                  /*!< 0x00000080 */
#define CEC_ISR_ARBLST           CEC_ISR_ARBLST_MSK                             /*!< CEC Arbitration Lost                   */
#define CEC_ISR_TXBR_POS         (8U)
#define CEC_ISR_TXBR_MSK         (0x1UL << CEC_ISR_TXBR_POS)                    /*!< 0x00000100 */
#define CEC_ISR_TXBR             CEC_ISR_TXBR_MSK                               /*!< CEC Tx Byte Request                    */
#define CEC_ISR_TXEND_POS        (9U)
#define CEC_ISR_TXEND_MSK        (0x1UL << CEC_ISR_TXEND_POS)                   /*!< 0x00000200 */
#define CEC_ISR_TXEND            CEC_ISR_TXEND_MSK                              /*!< CEC End of Transmission                */
#define CEC_ISR_TXUDR_POS        (10U)
#define CEC_ISR_TXUDR_MSK        (0x1UL << CEC_ISR_TXUDR_POS)                   /*!< 0x00000400 */
#define CEC_ISR_TXUDR            CEC_ISR_TXUDR_MSK                              /*!< CEC Tx-Buffer Underrun                 */
#define CEC_ISR_TXERR_POS        (11U)
#define CEC_ISR_TXERR_MSK        (0x1UL << CEC_ISR_TXERR_POS)                   /*!< 0x00000800 */
#define CEC_ISR_TXERR            CEC_ISR_TXERR_MSK                              /*!< CEC Tx-Error                           */
#define CEC_ISR_TXACKE_POS       (12U)
#define CEC_ISR_TXACKE_MSK       (0x1UL << CEC_ISR_TXACKE_POS)                  /*!< 0x00001000 */
#define CEC_ISR_TXACKE           CEC_ISR_TXACKE_MSK                             /*!< CEC Tx Missing Acknowledge             */

/*******************  Bit definition for CEC_IER register  ********************/
#define CEC_IER_RXBRIE_POS       (0U)
#define CEC_IER_RXBRIE_MSK       (0x1UL << CEC_IER_RXBRIE_POS)                  /*!< 0x00000001 */
#define CEC_IER_RXBRIE           CEC_IER_RXBRIE_MSK                             /*!< CEC Rx-Byte Received IT Enable         */
#define CEC_IER_RXENDIE_POS      (1U)
#define CEC_IER_RXENDIE_MSK      (0x1UL << CEC_IER_RXENDIE_POS)                 /*!< 0x00000002 */
#define CEC_IER_RXENDIE          CEC_IER_RXENDIE_MSK                            /*!< CEC End Of Reception IT Enable         */
#define CEC_IER_RXOVRIE_POS      (2U)
#define CEC_IER_RXOVRIE_MSK      (0x1UL << CEC_IER_RXOVRIE_POS)                 /*!< 0x00000004 */
#define CEC_IER_RXOVRIE          CEC_IER_RXOVRIE_MSK                            /*!< CEC Rx-Overrun IT Enable               */
#define CEC_IER_BREIE_POS        (3U)
#define CEC_IER_BREIE_MSK        (0x1UL << CEC_IER_BREIE_POS)                   /*!< 0x00000008 */
#define CEC_IER_BREIE            CEC_IER_BREIE_MSK                              /*!< CEC Rx Bit Rising Error IT Enable      */
#define CEC_IER_SBPEIE_POS       (4U)
#define CEC_IER_SBPEIE_MSK       (0x1UL << CEC_IER_SBPEIE_POS)                  /*!< 0x00000010 */
#define CEC_IER_SBPEIE           CEC_IER_SBPEIE_MSK                             /*!< CEC Rx Short Bit period Error IT Enable*/
#define CEC_IER_LBPEIE_POS       (5U)
#define CEC_IER_LBPEIE_MSK       (0x1UL << CEC_IER_LBPEIE_POS)                  /*!< 0x00000020 */
#define CEC_IER_LBPEIE           CEC_IER_LBPEIE_MSK                             /*!< CEC Rx Long Bit period Error IT Enable */
#define CEC_IER_RXACKEIE_POS     (6U)
#define CEC_IER_RXACKEIE_MSK     (0x1UL << CEC_IER_RXACKEIE_POS)                /*!< 0x00000040 */
#define CEC_IER_RXACKEIE         CEC_IER_RXACKEIE_MSK                           /*!< CEC Rx Missing Acknowledge IT Enable   */
#define CEC_IER_ARBLSTIE_POS     (7U)
#define CEC_IER_ARBLSTIE_MSK     (0x1UL << CEC_IER_ARBLSTIE_POS)                /*!< 0x00000080 */
#define CEC_IER_ARBLSTIE         CEC_IER_ARBLSTIE_MSK                           /*!< CEC Arbitration Lost IT Enable         */
#define CEC_IER_TXBRIE_POS       (8U)
#define CEC_IER_TXBRIE_MSK       (0x1UL << CEC_IER_TXBRIE_POS)                  /*!< 0x00000100 */
#define CEC_IER_TXBRIE           CEC_IER_TXBRIE_MSK                             /*!< CEC Tx Byte Request  IT Enable         */
#define CEC_IER_TXENDIE_POS      (9U)
#define CEC_IER_TXENDIE_MSK      (0x1UL << CEC_IER_TXENDIE_POS)                 /*!< 0x00000200 */
#define CEC_IER_TXENDIE          CEC_IER_TXENDIE_MSK                            /*!< CEC End of Transmission IT Enable      */
#define CEC_IER_TXUDRIE_POS      (10U)
#define CEC_IER_TXUDRIE_MSK      (0x1UL << CEC_IER_TXUDRIE_POS)                 /*!< 0x00000400 */
#define CEC_IER_TXUDRIE          CEC_IER_TXUDRIE_MSK                            /*!< CEC Tx-Buffer Underrun IT Enable       */
#define CEC_IER_TXERRIE_POS      (11U)
#define CEC_IER_TXERRIE_MSK      (0x1UL << CEC_IER_TXERRIE_POS)                 /*!< 0x00000800 */
#define CEC_IER_TXERRIE          CEC_IER_TXERRIE_MSK                            /*!< CEC Tx-Error IT Enable                 */
#define CEC_IER_TXACKEIE_POS     (12U)
#define CEC_IER_TXACKEIE_MSK     (0x1UL << CEC_IER_TXACKEIE_POS)                /*!< 0x00001000 */
#define CEC_IER_TXACKEIE         CEC_IER_TXACKEIE_MSK                           /*!< CEC Tx Missing Acknowledge IT Enable   */





//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- TAMPER AND BACKUP REGISTERS --------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief Tamper and backup registers
  */
typedef struct
{
  volatile uint32 CR1;            /*!< TAMP configuration register 1,                             Address offset: 0x00 */
  volatile uint32 CR2;            /*!< TAMP configuration register 2,                             Address offset: 0x04 */
       uint32 RESERVED0;      /*!< Reserved                                                   Address offset: 0x08 */
  volatile uint32 FLTCR;          /*!< Reserved                                                   Address offset: 0x0C */
       uint32 RESERVED1[7];   /*!< Reserved                                                   Address offset: 0x10 -- 0x28 */
  volatile uint32 IER;            /*!< TAMP Interrupt enable register,                            Address offset: 0x2C */
  volatile uint32 SR;             /*!< TAMP Status register,                                      Address offset: 0x30 */
  volatile uint32 MISR;           /*!< TAMP Masked Interrupt Status register,                     Address offset: 0x34 */
       uint32 RESERVED2;      /*!< Reserved                                                   Address offset: 0x38 */
  volatile uint32 SCR;            /*!< TAMP Status clear register,                                Address offset: 0x3C */
       uint32 RESERVED3[48];  /*!< Reserved                                                   Address offset: 0x54 -- 0xFC */
  volatile uint32 BKP0R;          /*!< TAMP backup register 0,                                    Address offset: 0x100 */
  volatile uint32 BKP1R;          /*!< TAMP backup register 1,                                    Address offset: 0x104 */
  volatile uint32 BKP2R;          /*!< TAMP backup register 2,                                    Address offset: 0x108 */
  volatile uint32 BKP3R;          /*!< TAMP backup register 3,                                    Address offset: 0x10C */
  volatile uint32 BKP4R;          /*!< TAMP backup register 4,                                    Address offset: 0x110 */
} TAMP_TYPEDEF;

#define TAMP                (*(TAMP_TYPEDEF *) TAMP_BASE)


/********************  Bits definition for TAMP_CR1 register  *****************/
#define TAMP_CR1_TAMP1E_POS          (0U)
#define TAMP_CR1_TAMP1E_MSK          (0x1UL << TAMP_CR1_TAMP1E_POS)             /*!< 0x00000001 */
#define TAMP_CR1_TAMP1E              TAMP_CR1_TAMP1E_MSK
#define TAMP_CR1_TAMP2E_POS          (1U)
#define TAMP_CR1_TAMP2E_MSK          (0x1UL << TAMP_CR1_TAMP2E_POS)             /*!< 0x00000002 */
#define TAMP_CR1_TAMP2E              TAMP_CR1_TAMP2E_MSK
#define TAMP_CR1_ITAMP3E_POS         (18U)
#define TAMP_CR1_ITAMP3E_MSK         (0x1UL << TAMP_CR1_ITAMP3E_POS)            /*!< 0x00040000 */
#define TAMP_CR1_ITAMP3E             TAMP_CR1_ITAMP3E_MSK
#define TAMP_CR1_ITAMP4E_POS         (19U)
#define TAMP_CR1_ITAMP4E_MSK         (0x1UL << TAMP_CR1_ITAMP4E_POS)            /*!< 0x00080000 */
#define TAMP_CR1_ITAMP4E             TAMP_CR1_ITAMP4E_MSK
#define TAMP_CR1_ITAMP5E_POS         (20U)
#define TAMP_CR1_ITAMP5E_MSK         (0x1UL << TAMP_CR1_ITAMP5E_POS)            /*!< 0x00100000 */
#define TAMP_CR1_ITAMP5E             TAMP_CR1_ITAMP5E_MSK
#define TAMP_CR1_ITAMP6E_POS         (21U)
#define TAMP_CR1_ITAMP6E_MSK         (0x1UL << TAMP_CR1_ITAMP6E_POS)            /*!< 0x00200000 */
#define TAMP_CR1_ITAMP6E             TAMP_CR1_ITAMP6E_MSK

/********************  Bits definition for TAMP_CR2 register  *****************/
#define TAMP_CR2_TAMP1NOERASE_POS    (0U)
#define TAMP_CR2_TAMP1NOERASE_MSK    (0x1UL << TAMP_CR2_TAMP1NOERASE_POS)       /*!< 0x00000001 */
#define TAMP_CR2_TAMP1NOERASE        TAMP_CR2_TAMP1NOERASE_MSK
#define TAMP_CR2_TAMP2NOERASE_POS    (1U)
#define TAMP_CR2_TAMP2NOERASE_MSK    (0x1UL << TAMP_CR2_TAMP2NOERASE_POS)       /*!< 0x00000002 */
#define TAMP_CR2_TAMP2NOERASE        TAMP_CR2_TAMP2NOERASE_MSK
#define TAMP_CR2_TAMP1MSK_POS        (16U)
#define TAMP_CR2_TAMP1MSK_MSK        (0x1UL << TAMP_CR2_TAMP1MSK_POS)           /*!< 0x00010000 */
#define TAMP_CR2_TAMP1MSK            TAMP_CR2_TAMP1MSK_MSK
#define TAMP_CR2_TAMP2MSK_POS        (17U)
#define TAMP_CR2_TAMP2MSK_MSK        (0x1UL << TAMP_CR2_TAMP2MSK_POS)           /*!< 0x00020000 */
#define TAMP_CR2_TAMP2MSK            TAMP_CR2_TAMP2MSK_MSK
#define TAMP_CR2_TAMP1TRG_POS        (24U)
#define TAMP_CR2_TAMP1TRG_MSK        (0x1UL << TAMP_CR2_TAMP1TRG_POS)           /*!< 0x01000000 */
#define TAMP_CR2_TAMP1TRG            TAMP_CR2_TAMP1TRG_MSK
#define TAMP_CR2_TAMP2TRG_POS        (25U)
#define TAMP_CR2_TAMP2TRG_MSK        (0x1UL << TAMP_CR2_TAMP2TRG_POS)           /*!< 0x02000000 */
#define TAMP_CR2_TAMP2TRG            TAMP_CR2_TAMP2TRG_MSK

/********************  Bits definition for TAMP_FLTCR register  ***************/
#define TAMP_FLTCR_TAMPFREQ_0        0x00000001U
#define TAMP_FLTCR_TAMPFREQ_1        0x00000002U
#define TAMP_FLTCR_TAMPFREQ_2        0x00000004U
#define TAMP_FLTCR_TAMPFREQ_POS      (0U)
#define TAMP_FLTCR_TAMPFREQ_MSK      (0x7UL << TAMP_FLTCR_TAMPFREQ_POS)         /*!< 0x00000007 */
#define TAMP_FLTCR_TAMPFREQ          TAMP_FLTCR_TAMPFREQ_MSK
#define TAMP_FLTCR_TAMPFLT_0         0x00000008U
#define TAMP_FLTCR_TAMPFLT_1         0x00000010U
#define TAMP_FLTCR_TAMPFLT_POS       (3U)
#define TAMP_FLTCR_TAMPFLT_MSK       (0x3UL << TAMP_FLTCR_TAMPFLT_POS)          /*!< 0x00000018 */
#define TAMP_FLTCR_TAMPFLT           TAMP_FLTCR_TAMPFLT_MSK
#define TAMP_FLTCR_TAMPPRCH_0        0x00000020U
#define TAMP_FLTCR_TAMPPRCH_1        0x00000040U
#define TAMP_FLTCR_TAMPPRCH_POS      (5U)
#define TAMP_FLTCR_TAMPPRCH_MSK      (0x3UL << TAMP_FLTCR_TAMPPRCH_POS)         /*!< 0x00000060 */
#define TAMP_FLTCR_TAMPPRCH          TAMP_FLTCR_TAMPPRCH_MSK
#define TAMP_FLTCR_TAMPPUDIS_POS     (7U)
#define TAMP_FLTCR_TAMPPUDIS_MSK     (0x1UL << TAMP_FLTCR_TAMPPUDIS_POS)        /*!< 0x00000080 */
#define TAMP_FLTCR_TAMPPUDIS         TAMP_FLTCR_TAMPPUDIS_MSK

/********************  Bits definition for TAMP_IER register  *****************/
#define TAMP_IER_TAMP1IE_POS         (0U)
#define TAMP_IER_TAMP1IE_MSK         (0x1UL << TAMP_IER_TAMP1IE_POS)            /*!< 0x00000001 */
#define TAMP_IER_TAMP1IE             TAMP_IER_TAMP1IE_MSK
#define TAMP_IER_TAMP2IE_POS         (1U)
#define TAMP_IER_TAMP2IE_MSK         (0x1UL << TAMP_IER_TAMP2IE_POS)            /*!< 0x00000002 */
#define TAMP_IER_TAMP2IE             TAMP_IER_TAMP2IE_MSK
#define TAMP_IER_ITAMP3IE_POS        (18U)
#define TAMP_IER_ITAMP3IE_MSK        (0x1UL << TAMP_IER_ITAMP3IE_POS)           /*!< 0x00040000 */
#define TAMP_IER_ITAMP3IE            TAMP_IER_ITAMP3IE_MSK
#define TAMP_IER_ITAMP4IE_POS        (19U)
#define TAMP_IER_ITAMP4IE_MSK        (0x1UL << TAMP_IER_ITAMP4IE_POS)           /*!< 0x00080000 */
#define TAMP_IER_ITAMP4IE            TAMP_IER_ITAMP4IE_MSK
#define TAMP_IER_ITAMP5IE_POS        (20U)
#define TAMP_IER_ITAMP5IE_MSK        (0x1UL << TAMP_IER_ITAMP5IE_POS)           /*!< 0x00100000 */
#define TAMP_IER_ITAMP5IE            TAMP_IER_ITAMP5IE_MSK
#define TAMP_IER_ITAMP6IE_POS        (21U)
#define TAMP_IER_ITAMP6IE_MSK        (0x1UL << TAMP_IER_ITAMP6IE_POS)           /*!< 0x00200000 */
#define TAMP_IER_ITAMP6IE            TAMP_IER_ITAMP6IE_MSK

/********************  Bits definition for TAMP_SR register  ******************/
#define TAMP_SR_TAMP1F_POS           (0U)
#define TAMP_SR_TAMP1F_MSK           (0x1UL << TAMP_SR_TAMP1F_POS)              /*!< 0x00000001 */
#define TAMP_SR_TAMP1F               TAMP_SR_TAMP1F_MSK
#define TAMP_SR_TAMP2F_POS           (1U)
#define TAMP_SR_TAMP2F_MSK           (0x1UL << TAMP_SR_TAMP2F_POS)              /*!< 0x00000002 */
#define TAMP_SR_TAMP2F               TAMP_SR_TAMP2F_MSK
#define TAMP_SR_ITAMP3F_POS          (18U)
#define TAMP_SR_ITAMP3F_MSK          (0x1UL << TAMP_SR_ITAMP3F_POS)             /*!< 0x00040000 */
#define TAMP_SR_ITAMP3F              TAMP_SR_ITAMP3F_MSK
#define TAMP_SR_ITAMP4F_POS          (19U)
#define TAMP_SR_ITAMP4F_MSK          (0x1UL << TAMP_SR_ITAMP4F_POS)             /*!< 0x00080000 */
#define TAMP_SR_ITAMP4F              TAMP_SR_ITAMP4F_MSK
#define TAMP_SR_ITAMP5F_POS          (20U)
#define TAMP_SR_ITAMP5F_MSK          (0x1UL << TAMP_SR_ITAMP5F_POS)             /*!< 0x00100000 */
#define TAMP_SR_ITAMP5F              TAMP_SR_ITAMP5F_MSK
#define TAMP_SR_ITAMP6F_POS          (21U)
#define TAMP_SR_ITAMP6F_MSK          (0x1UL << TAMP_SR_ITAMP6F_POS)             /*!< 0x00200000 */
#define TAMP_SR_ITAMP6F              TAMP_SR_ITAMP6F_MSK

/********************  Bits definition for TAMP_MISR register  ****************/
#define TAMP_MISR_TAMP1MF_POS        (0U)
#define TAMP_MISR_TAMP1MF_MSK        (0x1UL << TAMP_MISR_TAMP1MF_POS)           /*!< 0x00000001 */
#define TAMP_MISR_TAMP1MF            TAMP_MISR_TAMP1MF_MSK
#define TAMP_MISR_TAMP2MF_POS        (1U)
#define TAMP_MISR_TAMP2MF_MSK        (0x1UL << TAMP_MISR_TAMP2MF_POS)           /*!< 0x00000002 */
#define TAMP_MISR_TAMP2MF            TAMP_MISR_TAMP2MF_MSK
#define TAMP_MISR_ITAMP3MF_POS       (18U)
#define TAMP_MISR_ITAMP3MF_MSK       (0x1UL << TAMP_MISR_ITAMP3MF_POS)          /*!< 0x00040000 */
#define TAMP_MISR_ITAMP3MF           TAMP_MISR_ITAMP3MF_MSK
#define TAMP_MISR_ITAMP4MF_POS       (19U)
#define TAMP_MISR_ITAMP4MF_MSK       (0x1UL << TAMP_MISR_ITAMP4MF_POS)          /*!< 0x00080000 */
#define TAMP_MISR_ITAMP4MF           TAMP_MISR_ITAMP4MF_MSK
#define TAMP_MISR_ITAMP5MF_POS       (20U)
#define TAMP_MISR_ITAMP5MF_MSK       (0x1UL << TAMP_MISR_ITAMP5MF_POS)          /*!< 0x00100000 */
#define TAMP_MISR_ITAMP5MF           TAMP_MISR_ITAMP5MF_MSK
#define TAMP_MISR_ITAMP6MF_POS       (21U)
#define TAMP_MISR_ITAMP6MF_MSK       (0x1UL << TAMP_MISR_ITAMP6MF_POS)          /*!< 0x00200000 */
#define TAMP_MISR_ITAMP6MF           TAMP_MISR_ITAMP6MF_MSK

/********************  Bits definition for TAMP_SCR register  *****************/
#define TAMP_SCR_CTAMP1F_POS         (0U)
#define TAMP_SCR_CTAMP1F_MSK         (0x1UL << TAMP_SCR_CTAMP1F_POS)            /*!< 0x00000001 */
#define TAMP_SCR_CTAMP1F             TAMP_SCR_CTAMP1F_MSK
#define TAMP_SCR_CTAMP2F_POS         (1U)
#define TAMP_SCR_CTAMP2F_MSK         (0x1UL << TAMP_SCR_CTAMP2F_POS)            /*!< 0x00000002 */
#define TAMP_SCR_CTAMP2F             TAMP_SCR_CTAMP2F_MSK
#define TAMP_SCR_CITAMP3F_POS        (18U)
#define TAMP_SCR_CITAMP3F_MSK        (0x1UL << TAMP_SCR_CITAMP3F_POS)           /*!< 0x00040000 */
#define TAMP_SCR_CITAMP3F            TAMP_SCR_CITAMP3F_MSK
#define TAMP_SCR_CITAMP4F_POS        (19U)
#define TAMP_SCR_CITAMP4F_MSK        (0x1UL << TAMP_SCR_CITAMP4F_POS)           /*!< 0x00080000 */
#define TAMP_SCR_CITAMP4F            TAMP_SCR_CITAMP4F_MSK
#define TAMP_SCR_CITAMP5F_POS        (20U)
#define TAMP_SCR_CITAMP5F_MSK        (0x1UL << TAMP_SCR_CITAMP5F_POS)           /*!< 0x00100000 */
#define TAMP_SCR_CITAMP5F            TAMP_SCR_CITAMP5F_MSK
#define TAMP_SCR_CITAMP6F_POS        (21U)
#define TAMP_SCR_CITAMP6F_MSK        (0x1UL << TAMP_SCR_CITAMP6F_POS)           /*!< 0x00200000 */
#define TAMP_SCR_CITAMP6F            TAMP_SCR_CITAMP6F_MSK

/********************  Bits definition for TAMP_BKP0R register  ***************/
#define TAMP_BKP0R_POS               (0U)
#define TAMP_BKP0R_MSK               (0xFFFFFFFFUL << TAMP_BKP0R_POS)           /*!< 0xFFFFFFFF */
#define TAMP_BKP0R                   TAMP_BKP0R_MSK

/********************  Bits definition for TAMP_BKP1R register  ***************/
#define TAMP_BKP1R_POS               (0U)
#define TAMP_BKP1R_MSK               (0xFFFFFFFFUL << TAMP_BKP1R_POS)           /*!< 0xFFFFFFFF */
#define TAMP_BKP1R                   TAMP_BKP1R_MSK

/********************  Bits definition for TAMP_BKP2R register  ***************/
#define TAMP_BKP2R_POS               (0U)
#define TAMP_BKP2R_MSK               (0xFFFFFFFFUL << TAMP_BKP2R_POS)           /*!< 0xFFFFFFFF */
#define TAMP_BKP2R                   TAMP_BKP2R_MSK

/********************  Bits definition for TAMP_BKP3R register  ***************/
#define TAMP_BKP3R_POS               (0U)
#define TAMP_BKP3R_MSK               (0xFFFFFFFFUL << TAMP_BKP3R_POS)           /*!< 0xFFFFFFFF */
#define TAMP_BKP3R                   TAMP_BKP3R_MSK

/********************  Bits definition for TAMP_BKP4R register  ***************/
#define TAMP_BKP4R_POS               (0U)
#define TAMP_BKP4R_MSK               (0xFFFFFFFFUL << TAMP_BKP4R_POS)           /*!< 0xFFFFFFFF */
#define TAMP_BKP4R                   TAMP_BKP4R_MSK





//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------------- UCPD REGISTERS ---------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief UCPD
  */
typedef struct
{
  volatile uint32 CFG1;          /*!< UCPD configuration register 1,             Address offset: 0x00 */
  volatile uint32 CFG2;          /*!< UCPD configuration register 2,             Address offset: 0x04 */
  volatile uint32 RESERVED0;     /*!< UCPD reserved register,                    Address offset: 0x08 */
  volatile uint32 CR;            /*!< UCPD control register,                     Address offset: 0x0C */
  volatile uint32 IMR;           /*!< UCPD interrupt mask register,              Address offset: 0x10 */
  volatile uint32 SR;            /*!< UCPD status register,                      Address offset: 0x14 */
  volatile uint32 ICR;           /*!< UCPD interrupt flag clear register         Address offset: 0x18 */
  volatile uint32 TX_ORDSET;     /*!< UCPD Tx ordered set type register,         Address offset: 0x1C */
  volatile uint32 TX_PAYSZ;      /*!< UCPD Tx payload size register,             Address offset: 0x20 */
  volatile uint32 TXDR;          /*!< UCPD Tx data register,                     Address offset: 0x24 */
  volatile uint32 RX_ORDSET;     /*!< UCPD Rx ordered set type register,         Address offset: 0x28 */
  volatile uint32 RX_PAYSZ;      /*!< UCPD Rx payload size register,             Address offset: 0x2C */
  volatile uint32 RXDR;          /*!< UCPD Rx data register,                     Address offset: 0x30 */
  volatile uint32 RX_ORDEXT1;    /*!< UCPD Rx ordered set extension 1 register,  Address offset: 0x34 */
  volatile uint32 RX_ORDEXT2;    /*!< UCPD Rx ordered set extension 2 register,  Address offset: 0x38 */

} UCPD_TYPEDEF;

#define UCPD1               (*(UCPD_TYPEDEF *) UCPD1_BASE)
#define UCPD2               (*(UCPD_TYPEDEF *) UCPD2_BASE)


/********************  Bits definition for UCPD_CFG1 register  *******************/
#define UCPD_CFG1_HBITCLKDIV_POS            (0U)
#define UCPD_CFG1_HBITCLKDIV_MSK            (0x3FUL << UCPD_CFG1_HBITCLKDIV_POS)  /*!< 0x0000003F */
#define UCPD_CFG1_HBITCLKDIV                UCPD_CFG1_HBITCLKDIV_MSK              /*!< Number of cycles (minus 1) for a half bit clock */
#define UCPD_CFG1_HBITCLKDIV_0              (0x01UL << UCPD_CFG1_HBITCLKDIV_POS)  /*!< 0x00000001 */
#define UCPD_CFG1_HBITCLKDIV_1              (0x02UL << UCPD_CFG1_HBITCLKDIV_POS)  /*!< 0x00000002 */
#define UCPD_CFG1_HBITCLKDIV_2              (0x04UL << UCPD_CFG1_HBITCLKDIV_POS)  /*!< 0x00000004 */
#define UCPD_CFG1_HBITCLKDIV_3              (0x08UL << UCPD_CFG1_HBITCLKDIV_POS)  /*!< 0x00000008 */
#define UCPD_CFG1_HBITCLKDIV_4              (0x10UL << UCPD_CFG1_HBITCLKDIV_POS)  /*!< 0x00000010 */
#define UCPD_CFG1_HBITCLKDIV_5              (0x20UL << UCPD_CFG1_HBITCLKDIV_POS)  /*!< 0x00000020 */
#define UCPD_CFG1_IFRGAP_POS                (6U)
#define UCPD_CFG1_IFRGAP_MSK                (0x1FUL << UCPD_CFG1_IFRGAP_POS)      /*!< 0x000007C0 */
#define UCPD_CFG1_IFRGAP                    UCPD_CFG1_IFRGAP_MSK                  /*!< Clock divider value to generates Interframe gap */
#define UCPD_CFG1_IFRGAP_0                  (0x01UL << UCPD_CFG1_IFRGAP_POS)      /*!< 0x00000040 */
#define UCPD_CFG1_IFRGAP_1                  (0x02UL << UCPD_CFG1_IFRGAP_POS)      /*!< 0x00000080 */
#define UCPD_CFG1_IFRGAP_2                  (0x04UL << UCPD_CFG1_IFRGAP_POS)      /*!< 0x00000100 */
#define UCPD_CFG1_IFRGAP_3                  (0x08UL << UCPD_CFG1_IFRGAP_POS)      /*!< 0x00000200 */
#define UCPD_CFG1_IFRGAP_4                  (0x10UL << UCPD_CFG1_IFRGAP_POS)      /*!< 0x00000400 */
#define UCPD_CFG1_TRANSWIN_POS              (11U)
#define UCPD_CFG1_TRANSWIN_MSK              (0x1FUL << UCPD_CFG1_TRANSWIN_POS)    /*!< 0x0000F800 */
#define UCPD_CFG1_TRANSWIN                  UCPD_CFG1_TRANSWIN_MSK                /*!< Number of cycles (minus 1) of the half bit clock */
#define UCPD_CFG1_TRANSWIN_0                (0x01UL << UCPD_CFG1_TRANSWIN_POS)    /*!< 0x00000800 */
#define UCPD_CFG1_TRANSWIN_1                (0x02UL << UCPD_CFG1_TRANSWIN_POS)    /*!< 0x00001000 */
#define UCPD_CFG1_TRANSWIN_2                (0x04UL << UCPD_CFG1_TRANSWIN_POS)    /*!< 0x00002000 */
#define UCPD_CFG1_TRANSWIN_3                (0x08UL << UCPD_CFG1_TRANSWIN_POS)    /*!< 0x00004000 */
#define UCPD_CFG1_TRANSWIN_4                (0x10UL << UCPD_CFG1_TRANSWIN_POS)    /*!< 0x00008000 */
#define UCPD_CFG1_PSC_UCPDCLK_POS           (17U)
#define UCPD_CFG1_PSC_UCPDCLK_MSK           (0x7UL << UCPD_CFG1_PSC_UCPDCLK_POS)  /*!< 0x000E0000 */
#define UCPD_CFG1_PSC_UCPDCLK               UCPD_CFG1_PSC_UCPDCLK_MSK             /*!< Prescaler for UCPDCLK */
#define UCPD_CFG1_PSC_UCPDCLK_0             (0x1UL << UCPD_CFG1_PSC_UCPDCLK_POS)  /*!< 0x00020000 */
#define UCPD_CFG1_PSC_UCPDCLK_1             (0x2UL << UCPD_CFG1_PSC_UCPDCLK_POS)  /*!< 0x00040000 */
#define UCPD_CFG1_PSC_UCPDCLK_2             (0x4UL << UCPD_CFG1_PSC_UCPDCLK_POS)  /*!< 0x00080000 */
#define UCPD_CFG1_RXORDSETEN_POS            (20U)
#define UCPD_CFG1_RXORDSETEN_MSK            (0x1FFUL << UCPD_CFG1_RXORDSETEN_POS) /*!< 0x1FF00000 */
#define UCPD_CFG1_RXORDSETEN                UCPD_CFG1_RXORDSETEN_MSK              /*!< Receiver ordered set detection enable */
#define UCPD_CFG1_RXORDSETEN_0              (0x001UL << UCPD_CFG1_RXORDSETEN_POS) /*!< 0x00100000 */
#define UCPD_CFG1_RXORDSETEN_1              (0x002UL << UCPD_CFG1_RXORDSETEN_POS) /*!< 0x00200000 */
#define UCPD_CFG1_RXORDSETEN_2              (0x004UL << UCPD_CFG1_RXORDSETEN_POS) /*!< 0x00400000 */
#define UCPD_CFG1_RXORDSETEN_3              (0x008UL << UCPD_CFG1_RXORDSETEN_POS) /*!< 0x00800000 */
#define UCPD_CFG1_RXORDSETEN_4              (0x010UL << UCPD_CFG1_RXORDSETEN_POS) /*!< 0x01000000 */
#define UCPD_CFG1_RXORDSETEN_5              (0x020UL << UCPD_CFG1_RXORDSETEN_POS) /*!< 0x02000000 */
#define UCPD_CFG1_RXORDSETEN_6              (0x040UL << UCPD_CFG1_RXORDSETEN_POS) /*!< 0x04000000 */
#define UCPD_CFG1_RXORDSETEN_7              (0x080UL << UCPD_CFG1_RXORDSETEN_POS) /*!< 0x08000000 */
#define UCPD_CFG1_RXORDSETEN_8              (0x100UL << UCPD_CFG1_RXORDSETEN_POS) /*!< 0x10000000 */
#define UCPD_CFG1_TXDMAEN_POS               (29U)
#define UCPD_CFG1_TXDMAEN_MSK               (0x1UL << UCPD_CFG1_TXDMAEN_POS)      /*!< 0x20000000 */
#define UCPD_CFG1_TXDMAEN                   UCPD_CFG1_TXDMAEN_MSK                 /*!< DMA transmission requests enable   */
#define UCPD_CFG1_RXDMAEN_POS               (30U)
#define UCPD_CFG1_RXDMAEN_MSK               (0x1UL << UCPD_CFG1_RXDMAEN_POS)      /*!< 0x40000000 */
#define UCPD_CFG1_RXDMAEN                   UCPD_CFG1_RXDMAEN_MSK                 /*!< DMA reception requests enable   */
#define UCPD_CFG1_UCPDEN_POS                (31U)
#define UCPD_CFG1_UCPDEN_MSK                (0x1UL << UCPD_CFG1_UCPDEN_POS)       /*!< 0x80000000 */
#define UCPD_CFG1_UCPDEN                    UCPD_CFG1_UCPDEN_MSK                  /*!< USB Power Delivery Block Enable */

/********************  Bits definition for UCPD_CFG2 register  *******************/
#define UCPD_CFG2_RXFILTDIS_POS             (0U)
#define UCPD_CFG2_RXFILTDIS_MSK             (0x1UL << UCPD_CFG2_RXFILTDIS_POS)    /*!< 0x00000001 */
#define UCPD_CFG2_RXFILTDIS                 UCPD_CFG2_RXFILTDIS_MSK               /*!< Enables an Rx pre-filter for the BMC decoder */
#define UCPD_CFG2_RXFILT2N3_POS             (1U)
#define UCPD_CFG2_RXFILT2N3_MSK             (0x1UL << UCPD_CFG2_RXFILT2N3_POS)    /*!< 0x00000002 */
#define UCPD_CFG2_RXFILT2N3                 UCPD_CFG2_RXFILT2N3_MSK               /*!< Controls the sampling method for an Rx pre-filter for the BMC decode */
#define UCPD_CFG2_FORCECLK_POS              (2U)
#define UCPD_CFG2_FORCECLK_MSK              (0x1UL << UCPD_CFG2_FORCECLK_POS)     /*!< 0x00000004 */
#define UCPD_CFG2_FORCECLK                  UCPD_CFG2_FORCECLK_MSK                /*!< Controls forcing of the clock request UCPDCLK_REQ */
#define UCPD_CFG2_WUPEN_POS                 (3U)
#define UCPD_CFG2_WUPEN_MSK                 (0x1UL << UCPD_CFG2_WUPEN_POS)        /*!< 0x00000008 */
#define UCPD_CFG2_WUPEN                     UCPD_CFG2_WUPEN_MSK                   /*!< Wakeup from STOP enable */

/********************  Bits definition for UCPD_CR register  ********************/
#define UCPD_CR_TXMODE_POS                  (0U)
#define UCPD_CR_TXMODE_MSK                  (0x3UL << UCPD_CR_TXMODE_POS)         /*!< 0x00000003 */
#define UCPD_CR_TXMODE                      UCPD_CR_TXMODE_MSK                    /*!< Type of Tx packet  */
#define UCPD_CR_TXMODE_0                    (0x1UL << UCPD_CR_TXMODE_POS)         /*!< 0x00000001 */
#define UCPD_CR_TXMODE_1                    (0x2UL << UCPD_CR_TXMODE_POS)         /*!< 0x00000002 */
#define UCPD_CR_TXSEND_POS                  (2U)
#define UCPD_CR_TXSEND_MSK                  (0x1UL << UCPD_CR_TXSEND_POS)         /*!< 0x00000004 */
#define UCPD_CR_TXSEND                      UCPD_CR_TXSEND_MSK                    /*!< Type of Tx packet  */
#define UCPD_CR_TXHRST_POS                  (3U)
#define UCPD_CR_TXHRST_MSK                  (0x1UL << UCPD_CR_TXHRST_POS)         /*!< 0x00000008 */
#define UCPD_CR_TXHRST                      UCPD_CR_TXHRST_MSK                    /*!< Command to send a Tx Hard Reset  */
#define UCPD_CR_RXMODE_POS                  (4U)
#define UCPD_CR_RXMODE_MSK                  (0x1UL << UCPD_CR_RXMODE_POS)         /*!< 0x00000010 */
#define UCPD_CR_RXMODE                      UCPD_CR_RXMODE_MSK                    /*!< Receiver mode  */
#define UCPD_CR_PHYRXEN_POS                 (5U)
#define UCPD_CR_PHYRXEN_MSK                 (0x1UL << UCPD_CR_PHYRXEN_POS)        /*!< 0x00000020 */
#define UCPD_CR_PHYRXEN                     UCPD_CR_PHYRXEN_MSK                   /*!< Controls enable of USB Power Delivery receiver  */
#define UCPD_CR_PHYCCSEL_POS                (6U)
#define UCPD_CR_PHYCCSEL_MSK                (0x1UL << UCPD_CR_PHYCCSEL_POS)       /*!< 0x00000040 */
#define UCPD_CR_PHYCCSEL                    UCPD_CR_PHYCCSEL_MSK                  /*!<  */
#define UCPD_CR_ANASUBMODE_POS              (7U)
#define UCPD_CR_ANASUBMODE_MSK              (0x3UL << UCPD_CR_ANASUBMODE_POS)     /*!< 0x00000180 */
#define UCPD_CR_ANASUBMODE                  UCPD_CR_ANASUBMODE_MSK                /*!< Analog PHY sub-mode   */
#define UCPD_CR_ANASUBMODE_0                (0x1UL << UCPD_CR_ANASUBMODE_POS)     /*!< 0x00000080 */
#define UCPD_CR_ANASUBMODE_1                (0x2UL << UCPD_CR_ANASUBMODE_POS)     /*!< 0x00000100 */
#define UCPD_CR_ANAMODE_POS                 (9U)
#define UCPD_CR_ANAMODE_MSK                 (0x1UL << UCPD_CR_ANAMODE_POS)        /*!< 0x00000200 */
#define UCPD_CR_ANAMODE                     UCPD_CR_ANAMODE_MSK                   /*!< Analog PHY working mode   */
#define UCPD_CR_CCENABLE_POS                (10U)
#define UCPD_CR_CCENABLE_MSK                (0x3UL << UCPD_CR_CCENABLE_POS)       /*!< 0x00000C00 */
#define UCPD_CR_CCENABLE                    UCPD_CR_CCENABLE_MSK                  /*!<  */
#define UCPD_CR_CCENABLE_0                  (0x1UL << UCPD_CR_CCENABLE_POS)       /*!< 0x00000400 */
#define UCPD_CR_CCENABLE_1                  (0x2UL << UCPD_CR_CCENABLE_POS)       /*!< 0x00000800 */
#define UCPD_CR_FRSRXEN_POS                 (16U)
#define UCPD_CR_FRSRXEN_MSK                 (0x1UL << UCPD_CR_FRSRXEN_POS)        /*!< 0x00010000 */
#define UCPD_CR_FRSRXEN                     UCPD_CR_FRSRXEN_MSK                   /*!< Enable FRS request detection function */
#define UCPD_CR_FRSTX_POS                   (17U)
#define UCPD_CR_FRSTX_MSK                   (0x1UL << UCPD_CR_FRSTX_POS)          /*!< 0x00020000 */
#define UCPD_CR_FRSTX                       UCPD_CR_FRSTX_MSK                     /*!< Signal Fast Role Swap request */
#define UCPD_CR_RDCH_POS                    (18U)
#define UCPD_CR_RDCH_MSK                    (0x1UL << UCPD_CR_RDCH_POS)           /*!< 0x00040000 */
#define UCPD_CR_RDCH                        UCPD_CR_RDCH_MSK                      /*!<  */
#define UCPD_CR_CC1TCDIS_POS                (20U)
#define UCPD_CR_CC1TCDIS_MSK                (0x1UL << UCPD_CR_CC1TCDIS_POS)       /*!< 0x00100000 */
#define UCPD_CR_CC1TCDIS                    UCPD_CR_CC1TCDIS_MSK                  /*!< The bit allows the Type-C detector for CC0 to be disabled. */
#define UCPD_CR_CC2TCDIS_POS                (21U)
#define UCPD_CR_CC2TCDIS_MSK                (0x1UL << UCPD_CR_CC2TCDIS_POS)       /*!< 0x00200000 */
#define UCPD_CR_CC2TCDIS                    UCPD_CR_CC2TCDIS_MSK                  /*!< The bit allows the Type-C detector for CC2 to be disabled. */

/********************  Bits definition for UCPD_IMR register  *******************/
#define UCPD_IMR_TXISIE_POS                 (0U)
#define UCPD_IMR_TXISIE_MSK                 (0x1UL << UCPD_IMR_TXISIE_POS)        /*!< 0x00000001 */
#define UCPD_IMR_TXISIE                     UCPD_IMR_TXISIE_MSK                   /*!< Enable TXIS interrupt  */
#define UCPD_IMR_TXMSGDISCIE_POS            (1U)
#define UCPD_IMR_TXMSGDISCIE_MSK            (0x1UL << UCPD_IMR_TXMSGDISCIE_POS)   /*!< 0x00000002 */
#define UCPD_IMR_TXMSGDISCIE                UCPD_IMR_TXMSGDISCIE_MSK              /*!< Enable TXMSGDISC interrupt  */
#define UCPD_IMR_TXMSGSENTIE_POS            (2U)
#define UCPD_IMR_TXMSGSENTIE_MSK            (0x1UL << UCPD_IMR_TXMSGSENTIE_POS)   /*!< 0x00000004 */
#define UCPD_IMR_TXMSGSENTIE                UCPD_IMR_TXMSGSENTIE_MSK              /*!< Enable TXMSGSENT interrupt  */
#define UCPD_IMR_TXMSGABTIE_POS             (3U)
#define UCPD_IMR_TXMSGABTIE_MSK             (0x1UL << UCPD_IMR_TXMSGABTIE_POS)    /*!< 0x00000008 */
#define UCPD_IMR_TXMSGABTIE                 UCPD_IMR_TXMSGABTIE_MSK               /*!< Enable TXMSGABT interrupt  */
#define UCPD_IMR_HRSTDISCIE_POS             (4U)
#define UCPD_IMR_HRSTDISCIE_MSK             (0x1UL << UCPD_IMR_HRSTDISCIE_POS)    /*!< 0x00000010 */
#define UCPD_IMR_HRSTDISCIE                 UCPD_IMR_HRSTDISCIE_MSK               /*!< Enable HRSTDISC interrupt  */
#define UCPD_IMR_HRSTSENTIE_POS             (5U)
#define UCPD_IMR_HRSTSENTIE_MSK             (0x1UL << UCPD_IMR_HRSTSENTIE_POS)    /*!< 0x00000020 */
#define UCPD_IMR_HRSTSENTIE                 UCPD_IMR_HRSTSENTIE_MSK               /*!< Enable HRSTSENT interrupt  */
#define UCPD_IMR_TXUNDIE_POS                (6U)
#define UCPD_IMR_TXUNDIE_MSK                (0x1UL << UCPD_IMR_TXUNDIE_POS)       /*!< 0x00000040 */
#define UCPD_IMR_TXUNDIE                    UCPD_IMR_TXUNDIE_MSK                  /*!< Enable TXUND interrupt  */
#define UCPD_IMR_RXNEIE_POS                 (8U)
#define UCPD_IMR_RXNEIE_MSK                 (0x1UL << UCPD_IMR_RXNEIE_POS)        /*!< 0x00000100 */
#define UCPD_IMR_RXNEIE                     UCPD_IMR_RXNEIE_MSK                   /*!< Enable RXNE interrupt  */
#define UCPD_IMR_RXORDDETIE_POS             (9U)
#define UCPD_IMR_RXORDDETIE_MSK             (0x1UL << UCPD_IMR_RXORDDETIE_POS)    /*!< 0x00000200 */
#define UCPD_IMR_RXORDDETIE                 UCPD_IMR_RXORDDETIE_MSK               /*!< Enable RXORDDET interrupt  */
#define UCPD_IMR_RXHRSTDETIE_POS            (10U)
#define UCPD_IMR_RXHRSTDETIE_MSK            (0x1UL << UCPD_IMR_RXHRSTDETIE_POS)   /*!< 0x00000400 */
#define UCPD_IMR_RXHRSTDETIE                UCPD_IMR_RXHRSTDETIE_MSK              /*!< Enable RXHRSTDET interrupt  */
#define UCPD_IMR_RXOVRIE_POS                (11U)
#define UCPD_IMR_RXOVRIE_MSK                (0x1UL << UCPD_IMR_RXOVRIE_POS)       /*!< 0x00000800 */
#define UCPD_IMR_RXOVRIE                    UCPD_IMR_RXOVRIE_MSK                  /*!< Enable RXOVR interrupt  */
#define UCPD_IMR_RXMSGENDIE_POS             (12U)
#define UCPD_IMR_RXMSGENDIE_MSK             (0x1UL << UCPD_IMR_RXMSGENDIE_POS)    /*!< 0x00001000 */
#define UCPD_IMR_RXMSGENDIE                 UCPD_IMR_RXMSGENDIE_MSK               /*!< Enable RXMSGEND interrupt  */
#define UCPD_IMR_TYPECEVT1IE_POS            (14U)
#define UCPD_IMR_TYPECEVT1IE_MSK            (0x1UL << UCPD_IMR_TYPECEVT1IE_POS)   /*!< 0x00004000 */
#define UCPD_IMR_TYPECEVT1IE                UCPD_IMR_TYPECEVT1IE_MSK              /*!< Enable TYPECEVT1IE interrupt  */
#define UCPD_IMR_TYPECEVT2IE_POS            (15U)
#define UCPD_IMR_TYPECEVT2IE_MSK            (0x1UL << UCPD_IMR_TYPECEVT2IE_POS)   /*!< 0x00008000 */
#define UCPD_IMR_TYPECEVT2IE                UCPD_IMR_TYPECEVT2IE_MSK              /*!< Enable TYPECEVT2IE interrupt  */
#define UCPD_IMR_FRSEVTIE_POS               (20U)
#define UCPD_IMR_FRSEVTIE_MSK               (0x1UL << UCPD_IMR_FRSEVTIE_POS)      /*!< 0x00100000 */
#define UCPD_IMR_FRSEVTIE                   UCPD_IMR_FRSEVTIE_MSK                 /*!< Fast Role Swap interrupt  */

/********************  Bits definition for UCPD_SR register  ********************/
#define UCPD_SR_TXIS_POS                    (0U)
#define UCPD_SR_TXIS_MSK                    (0x1UL << UCPD_SR_TXIS_POS)           /*!< 0x00000001 */
#define UCPD_SR_TXIS                        UCPD_SR_TXIS_MSK                      /*!< Transmit interrupt status  */
#define UCPD_SR_TXMSGDISC_POS               (1U)
#define UCPD_SR_TXMSGDISC_MSK               (0x1UL << UCPD_SR_TXMSGDISC_POS)      /*!< 0x00000002 */
#define UCPD_SR_TXMSGDISC                   UCPD_SR_TXMSGDISC_MSK                 /*!< Transmit message discarded interrupt  */
#define UCPD_SR_TXMSGSENT_POS               (2U)
#define UCPD_SR_TXMSGSENT_MSK               (0x1UL << UCPD_SR_TXMSGSENT_POS)      /*!< 0x00000004 */
#define UCPD_SR_TXMSGSENT                   UCPD_SR_TXMSGSENT_MSK                 /*!< Transmit message sent interrupt  */
#define UCPD_SR_TXMSGABT_POS                (3U)
#define UCPD_SR_TXMSGABT_MSK                (0x1UL << UCPD_SR_TXMSGABT_POS)       /*!< 0x00000008 */
#define UCPD_SR_TXMSGABT                    UCPD_SR_TXMSGABT_MSK                  /*!< Transmit message abort interrupt  */
#define UCPD_SR_HRSTDISC_POS                (4U)
#define UCPD_SR_HRSTDISC_MSK                (0x1UL << UCPD_SR_HRSTDISC_POS)       /*!< 0x00000010 */
#define UCPD_SR_HRSTDISC                    UCPD_SR_HRSTDISC_MSK                  /*!< HRST discarded interrupt  */
#define UCPD_SR_HRSTSENT_POS                (5U)
#define UCPD_SR_HRSTSENT_MSK                (0x1UL << UCPD_SR_HRSTSENT_POS)       /*!< 0x00000020 */
#define UCPD_SR_HRSTSENT                    UCPD_SR_HRSTSENT_MSK                  /*!< HRST sent interrupt  */
#define UCPD_SR_TXUND_POS                   (6U)
#define UCPD_SR_TXUND_MSK                   (0x1UL << UCPD_SR_TXUND_POS)          /*!< 0x00000040 */
#define UCPD_SR_TXUND                       UCPD_SR_TXUND_MSK                     /*!< Tx data underrun condition interrupt  */
#define UCPD_SR_RXNE_POS                    (8U)
#define UCPD_SR_RXNE_MSK                    (0x1UL << UCPD_SR_RXNE_POS)           /*!< 0x00000100 */
#define UCPD_SR_RXNE                        UCPD_SR_RXNE_MSK                      /*!< Receive data register not empty interrupt  */
#define UCPD_SR_RXORDDET_POS                (9U)
#define UCPD_SR_RXORDDET_MSK                (0x1UL << UCPD_SR_RXORDDET_POS)       /*!< 0x00000200 */
#define UCPD_SR_RXORDDET                    UCPD_SR_RXORDDET_MSK                  /*!< Rx ordered set (4 K-codes) detected interrupt  */
#define UCPD_SR_RXHRSTDET_POS               (10U)
#define UCPD_SR_RXHRSTDET_MSK               (0x1UL << UCPD_SR_RXHRSTDET_POS)      /*!< 0x00000400 */
#define UCPD_SR_RXHRSTDET                   UCPD_SR_RXHRSTDET_MSK                 /*!< Rx Hard Reset detect interrupt  */
#define UCPD_SR_RXOVR_POS                   (11U)
#define UCPD_SR_RXOVR_MSK                   (0x1UL << UCPD_SR_RXOVR_POS)          /*!< 0x00000800 */
#define UCPD_SR_RXOVR                       UCPD_SR_RXOVR_MSK                     /*!< Rx data overflow interrupt  */
#define UCPD_SR_RXMSGEND_POS                (12U)
#define UCPD_SR_RXMSGEND_MSK                (0x1UL << UCPD_SR_RXMSGEND_POS)       /*!< 0x00001000 */
#define UCPD_SR_RXMSGEND                    UCPD_SR_RXMSGEND_MSK                  /*!< Rx message received  */
#define UCPD_SR_RXERR_POS                   (13U)
#define UCPD_SR_RXERR_MSK                   (0x1UL << UCPD_SR_RXERR_POS)          /*!< 0x00002000 */
#define UCPD_SR_RXERR                       UCPD_SR_RXERR_MSK                     /*!< RX Error */
#define UCPD_SR_TYPECEVT1_POS               (14U)
#define UCPD_SR_TYPECEVT1_MSK               (0x1UL << UCPD_SR_TYPECEVT1_POS)      /*!< 0x00004000 */
#define UCPD_SR_TYPECEVT1                   UCPD_SR_TYPECEVT1_MSK                 /*!< Type C voltage level event on CC1  */
#define UCPD_SR_TYPECEVT2_POS               (15U)
#define UCPD_SR_TYPECEVT2_MSK               (0x1UL << UCPD_SR_TYPECEVT2_POS)      /*!< 0x00008000 */
#define UCPD_SR_TYPECEVT2                   UCPD_SR_TYPECEVT2_MSK                 /*!< Type C voltage level event on CC2  */
#define UCPD_SR_TYPEC_VSTATE_CC1_POS        (16U)
#define UCPD_SR_TYPEC_VSTATE_CC1_MSK        (0x3UL << UCPD_SR_TYPEC_VSTATE_CC1_POS) /*!< 0x00030000 */
#define UCPD_SR_TYPEC_VSTATE_CC1            UCPD_SR_TYPEC_VSTATE_CC1_MSK            /*!< Status of DC level on CC1 pin  */
#define UCPD_SR_TYPEC_VSTATE_CC1_0          (0x1UL << UCPD_SR_TYPEC_VSTATE_CC1_POS) /*!< 0x00010000 */
#define UCPD_SR_TYPEC_VSTATE_CC1_1          (0x2UL << UCPD_SR_TYPEC_VSTATE_CC1_POS) /*!< 0x00020000 */
#define UCPD_SR_TYPEC_VSTATE_CC2_POS        (18U)
#define UCPD_SR_TYPEC_VSTATE_CC2_MSK        (0x3UL << UCPD_SR_TYPEC_VSTATE_CC2_POS) /*!< 0x000C0000 */
#define UCPD_SR_TYPEC_VSTATE_CC2            UCPD_SR_TYPEC_VSTATE_CC2_MSK            /*!<Status of DC level on CC2 pin  */
#define UCPD_SR_TYPEC_VSTATE_CC2_0          (0x1UL << UCPD_SR_TYPEC_VSTATE_CC2_POS) /*!< 0x00040000 */
#define UCPD_SR_TYPEC_VSTATE_CC2_1          (0x2UL << UCPD_SR_TYPEC_VSTATE_CC2_POS) /*!< 0x00080000 */
#define UCPD_SR_FRSEVT_POS                  (20U)
#define UCPD_SR_FRSEVT_MSK                  (0x1UL << UCPD_SR_FRSEVT_POS)         /*!< 0x00100000 */
#define UCPD_SR_FRSEVT                      UCPD_SR_FRSEVT_MSK                    /*!< Fast Role Swap detection event  */

/********************  Bits definition for UCPD_ICR register  *******************/
#define UCPD_ICR_TXMSGDISCCF_POS            (1U)
#define UCPD_ICR_TXMSGDISCCF_MSK            (0x1UL << UCPD_ICR_TXMSGDISCCF_POS)   /*!< 0x00000002 */
#define UCPD_ICR_TXMSGDISCCF                UCPD_ICR_TXMSGDISCCF_MSK              /*!< Tx message discarded flag (TXMSGDISC) clear  */
#define UCPD_ICR_TXMSGSENTCF_POS            (2U)
#define UCPD_ICR_TXMSGSENTCF_MSK            (0x1UL << UCPD_ICR_TXMSGSENTCF_POS)   /*!< 0x00000004 */
#define UCPD_ICR_TXMSGSENTCF                UCPD_ICR_TXMSGSENTCF_MSK              /*!< Tx message sent flag (TXMSGSENT) clear  */
#define UCPD_ICR_TXMSGABTCF_POS             (3U)
#define UCPD_ICR_TXMSGABTCF_MSK             (0x1UL << UCPD_ICR_TXMSGABTCF_POS)    /*!< 0x00000008 */
#define UCPD_ICR_TXMSGABTCF                 UCPD_ICR_TXMSGABTCF_MSK               /*!< Tx message abort flag (TXMSGABT) clear  */
#define UCPD_ICR_HRSTDISCCF_POS             (4U)
#define UCPD_ICR_HRSTDISCCF_MSK             (0x1UL << UCPD_ICR_HRSTDISCCF_POS)    /*!< 0x00000010 */
#define UCPD_ICR_HRSTDISCCF                 UCPD_ICR_HRSTDISCCF_MSK               /*!< Hard reset discarded flag (HRSTDISC) clear  */
#define UCPD_ICR_HRSTSENTCF_POS             (5U)
#define UCPD_ICR_HRSTSENTCF_MSK             (0x1UL << UCPD_ICR_HRSTSENTCF_POS)    /*!< 0x00000020 */
#define UCPD_ICR_HRSTSENTCF                 UCPD_ICR_HRSTSENTCF_MSK               /*!< Hard reset sent flag (HRSTSENT) clear  */
#define UCPD_ICR_TXUNDCF_POS                (6U)
#define UCPD_ICR_TXUNDCF_MSK                (0x1UL << UCPD_ICR_TXUNDCF_POS)       /*!< 0x00000040 */
#define UCPD_ICR_TXUNDCF                    UCPD_ICR_TXUNDCF_MSK                  /*!< Tx underflow flag (TXUND) clear  */
#define UCPD_ICR_RXORDDETCF_POS             (9U)
#define UCPD_ICR_RXORDDETCF_MSK             (0x1UL << UCPD_ICR_RXORDDETCF_POS)    /*!< 0x00000200 */
#define UCPD_ICR_RXORDDETCF                 UCPD_ICR_RXORDDETCF_MSK               /*!< Rx ordered set detect flag (RXORDDET) clear  */
#define UCPD_ICR_RXHRSTDETCF_POS            (10U)
#define UCPD_ICR_RXHRSTDETCF_MSK            (0x1UL << UCPD_ICR_RXHRSTDETCF_POS)   /*!< 0x00000400 */
#define UCPD_ICR_RXHRSTDETCF                UCPD_ICR_RXHRSTDETCF_MSK              /*!< Rx Hard Reset detected flag (RXHRSTDET) clear  */
#define UCPD_ICR_RXOVRCF_POS                (11U)
#define UCPD_ICR_RXOVRCF_MSK                (0x1UL << UCPD_ICR_RXOVRCF_POS)       /*!< 0x00000800 */
#define UCPD_ICR_RXOVRCF                    UCPD_ICR_RXOVRCF_MSK                  /*!< Rx overflow flag (RXOVR) clear  */
#define UCPD_ICR_RXMSGENDCF_POS             (12U)
#define UCPD_ICR_RXMSGENDCF_MSK             (0x1UL << UCPD_ICR_RXMSGENDCF_POS)    /*!< 0x00001000 */
#define UCPD_ICR_RXMSGENDCF                 UCPD_ICR_RXMSGENDCF_MSK               /*!< Rx message received flag (RXMSGEND) clear  */
#define UCPD_ICR_TYPECEVT1CF_POS            (14U)
#define UCPD_ICR_TYPECEVT1CF_MSK            (0x1UL << UCPD_ICR_TYPECEVT1CF_POS)   /*!< 0x00004000 */
#define UCPD_ICR_TYPECEVT1CF                UCPD_ICR_TYPECEVT1CF_MSK              /*!< TypeC event (CC1) flag (TYPECEVT1) clear  */
#define UCPD_ICR_TYPECEVT2CF_POS            (15U)
#define UCPD_ICR_TYPECEVT2CF_MSK            (0x1UL << UCPD_ICR_TYPECEVT2CF_POS)   /*!< 0x00008000 */
#define UCPD_ICR_TYPECEVT2CF                UCPD_ICR_TYPECEVT2CF_MSK              /*!< TypeC event (CC2) flag (TYPECEVT2) clear  */
#define UCPD_ICR_FRSEVTCF_POS               (20U)
#define UCPD_ICR_FRSEVTCF_MSK               (0x1UL << UCPD_ICR_FRSEVTCF_POS)      /*!< 0x00100000 */
#define UCPD_ICR_FRSEVTCF                   UCPD_ICR_FRSEVTCF_MSK                 /*!< Fast Role Swap event flag clear  */

/********************  Bits definition for UCPD_TXORDSET register  **************/
#define UCPD_TX_ORDSET_TXORDSET_POS         (0U)
#define UCPD_TX_ORDSET_TXORDSET_MSK         (0xFFFFFUL << UCPD_TX_ORDSET_TXORDSET_POS) /*!< 0x000FFFFF */
#define UCPD_TX_ORDSET_TXORDSET             UCPD_TX_ORDSET_TXORDSET_MSK                /*!< Tx Ordered Set */

/********************  Bits definition for UCPD_TXPAYSZ register  ****************/
#define UCPD_TX_PAYSZ_TXPAYSZ_POS           (0U)
#define UCPD_TX_PAYSZ_TXPAYSZ_MSK           (0x3FFUL << UCPD_TX_PAYSZ_TXPAYSZ_POS) /*!< 0x000003FF */
#define UCPD_TX_PAYSZ_TXPAYSZ               UCPD_TX_PAYSZ_TXPAYSZ_MSK              /*!< Tx payload size in bytes  */

/********************  Bits definition for UCPD_TXDR register  *******************/
#define UCPD_TXDR_TXDATA_POS                (0U)
#define UCPD_TXDR_TXDATA_MSK                 (0xFFUL << UCPD_TXDR_TXDATA_POS)      /*!< 0x000000FF */
#define UCPD_TXDR_TXDATA                    UCPD_TXDR_TXDATA_MSK                   /*!< Tx Data Register */

/********************  Bits definition for UCPD_RXORDSET register  **************/
#define UCPD_RX_ORDSET_RXORDSET_POS         (0U)
#define UCPD_RX_ORDSET_RXORDSET_MSK         (0x7UL << UCPD_RX_ORDSET_RXORDSET_POS)  /*!< 0x00000007 */
#define UCPD_RX_ORDSET_RXORDSET             UCPD_RX_ORDSET_RXORDSET_MSK             /*!< Rx Ordered Set Code detected  */
#define UCPD_RX_ORDSET_RXORDSET_0           (0x1UL << UCPD_RX_ORDSET_RXORDSET_POS)  /*!< 0x00000001 */
#define UCPD_RX_ORDSET_RXORDSET_1           (0x2UL << UCPD_RX_ORDSET_RXORDSET_POS)  /*!< 0x00000002 */
#define UCPD_RX_ORDSET_RXORDSET_2           (0x4UL << UCPD_RX_ORDSET_RXORDSET_POS)  /*!< 0x00000004 */
#define UCPD_RX_ORDSET_RXSOP3OF4_POS        (3U)
#define UCPD_RX_ORDSET_RXSOP3OF4_MSK        (0x1UL << UCPD_RX_ORDSET_RXSOP3OF4_POS) /*!< 0x00000008 */
#define UCPD_RX_ORDSET_RXSOP3OF4            UCPD_RX_ORDSET_RXSOP3OF4_MSK            /*!< Rx Ordered Set Debug indication */
#define UCPD_RX_ORDSET_RXSOPKINVALID_POS    (4U)
#define UCPD_RX_ORDSET_RXSOPKINVALID_MSK    (0x7UL << UCPD_RX_ORDSET_RXSOPKINVALID_POS) /*!< 0x00000070 */
#define UCPD_RX_ORDSET_RXSOPKINVALID        UCPD_RX_ORDSET_RXSOPKINVALID_MSK            /*!< Rx Ordered Set corrupted K-Codes (Debug) */

/********************  Bits definition for UCPD_RXPAYSZ register  ****************/
#define UCPD_RX_PAYSZ_RXPAYSZ_POS           (0U)
#define UCPD_RX_PAYSZ_RXPAYSZ_MSK           (0x3FFUL << UCPD_RX_PAYSZ_RXPAYSZ_POS) /*!< 0x000003FF */
#define UCPD_RX_PAYSZ_RXPAYSZ               UCPD_RX_PAYSZ_RXPAYSZ_MSK              /*!< Rx payload size in bytes  */

/********************  Bits definition for UCPD_RXDR register  *******************/
#define UCPD_RXDR_RXDATA_POS                (0U)
#define UCPD_RXDR_RXDATA_MSK                (0xFFUL << UCPD_RXDR_RXDATA_POS)      /*!< 0x000000FF */
#define UCPD_RXDR_RXDATA                    UCPD_RXDR_RXDATA_MSK                  /*!< 8-bit receive data  */

/********************  Bits definition for UCPD_RXORDEXT1 register  **************/
#define UCPD_RX_ORDEXT1_RXSOPX1_POS         (0U)
#define UCPD_RX_ORDEXT1_RXSOPX1_MSK         (0xFFFFFUL << UCPD_RX_ORDEXT1_RXSOPX1_POS) /*!< 0x000FFFFF */
#define UCPD_RX_ORDEXT1_RXSOPX1             UCPD_RX_ORDEXT1_RXSOPX1_MSK                /*!< RX Ordered Set Extension Register 1 */

/********************  Bits definition for UCPD_RXORDEXT2 register  **************/
#define UCPD_RX_ORDEXT2_RXSOPX2_POS         (0U)
#define UCPD_RX_ORDEXT2_RXSOPX2_MSK         (0xFFFFFUL << UCPD_RX_ORDEXT2_RXSOPX2_POS) /*!< 0x000FFFFF */
#define UCPD_RX_ORDEXT2_RXSOPX2             UCPD_RX_ORDEXT2_RXSOPX2_MSK                /*!< RX Ordered Set Extension Register 1 */





/*******************************************************************************
 *                 Register Abstraction
  Core Register contain:
  - Core Register
  - Core NVIC Register
  - Core SCB Register
  - Core SysTick Register
  - Core MPU Register
 ******************************************************************************/
/**
  \defgroup CMSIS_core_register Defines and Type Definitions
  \brief Type definitions and defines for Cortex-M processor based devices.
*/

/**
  \ingroup    CMSIS_core_register
  \defgroup   CMSIS_CORE  Status and Control Registers
  \brief      Core Register type definitions.
  @{
 */

/**
  \brief  Union type to access the Application Program Status Register (APSR).
 */
typedef union
{
  struct
  {
    uint32 _reserved0:28;              /*!< bit:  0..27  Reserved */
    uint32 V:1;                        /*!< bit:     28  Overflow condition code flag */
    uint32 C:1;                        /*!< bit:     29  Carry condition code flag */
    uint32 Z:1;                        /*!< bit:     30  Zero condition code flag */
    uint32 N:1;                        /*!< bit:     31  Negative condition code flag */
  } b;                                   /*!< Structure used for bit  access */
  uint32 w;                            /*!< Type      used for word access */
} APSR_TYPEDEF;

/* APSR Register Definitions */
#define APSR_N_POS                         31U                                            /*!< APSR: N Position */
#define APSR_N_MSK                         (1UL << APSR_N_POS)                            /*!< APSR: N Mask */

#define APSR_Z_POS                         30U                                            /*!< APSR: Z Position */
#define APSR_Z_MSK                         (1UL << APSR_Z_POS)                            /*!< APSR: Z Mask */

#define APSR_C_POS                         29U                                            /*!< APSR: C Position */
#define APSR_C_MSK                         (1UL << APSR_C_POS)                            /*!< APSR: C Mask */

#define APSR_V_POS                         28U                                            /*!< APSR: V Position */
#define APSR_V_MSK                         (1UL << APSR_V_POS)                            /*!< APSR: V Mask */


/**
  \brief  Union type to access the Interrupt Program Status Register (IPSR).
 */
typedef union
{
  struct
  {
    uint32 ISR:9;                      /*!< bit:  0.. 8  Exception number */
    uint32 _reserved0:23;              /*!< bit:  9..31  Reserved */
  } b;                                   /*!< Structure used for bit  access */
  uint32 w;                            /*!< Type      used for word access */
} IPSR_TYPEDEF;

/* IPSR Register Definitions */
#define IPSR_ISR_POS                        0U                                            /*!< IPSR: ISR Position */
#define IPSR_ISR_MSK                       (0x1FFUL /*<< IPSR_ISR_POS*/)                  /*!< IPSR: ISR Mask */


/**
  \brief  Union type to access the Special-Purpose Program Status Registers (xPSR).
 */
typedef union
{
  struct
  {
    uint32 ISR:9;                      /*!< bit:  0.. 8  Exception number */
    uint32 _reserved0:15;              /*!< bit:  9..23  Reserved */
    uint32 T:1;                        /*!< bit:     24  Thumb bit        (read 0) */
    uint32 _reserved1:3;               /*!< bit: 25..27  Reserved */
    uint32 V:1;                        /*!< bit:     28  Overflow condition code flag */
    uint32 C:1;                        /*!< bit:     29  Carry condition code flag */
    uint32 Z:1;                        /*!< bit:     30  Zero condition code flag */
    uint32 N:1;                        /*!< bit:     31  Negative condition code flag */
  } b;                                   /*!< Structure used for bit  access */
  uint32 w;                            /*!< Type      used for word access */
} xPSR_TYPEDEF;

/* xPSR Register Definitions */
#define xPSR_N_POS                         31U                                            /*!< xPSR: N Position */
#define xPSR_N_MSK                         (1UL << xPSR_N_POS)                            /*!< xPSR: N Mask */

#define xPSR_Z_POS                         30U                                            /*!< xPSR: Z Position */
#define xPSR_Z_MSK                         (1UL << xPSR_Z_POS)                            /*!< xPSR: Z Mask */

#define xPSR_C_POS                         29U                                            /*!< xPSR: C Position */
#define xPSR_C_MSK                         (1UL << xPSR_C_POS)                            /*!< xPSR: C Mask */

#define xPSR_V_POS                         28U                                            /*!< xPSR: V Position */
#define xPSR_V_MSK                         (1UL << xPSR_V_POS)                            /*!< xPSR: V Mask */

#define xPSR_T_POS                         24U                                            /*!< xPSR: T Position */
#define xPSR_T_MSK                         (1UL << xPSR_T_POS)                            /*!< xPSR: T Mask */

#define xPSR_ISR_POS                        0U                                            /*!< xPSR: ISR Position */
#define xPSR_ISR_MSK                       (0x1FFUL /*<< xPSR_ISR_POS*/)                  /*!< xPSR: ISR Mask */


/**
  \brief  Union type to access the Control Registers (CONTROL).
 */
typedef union
{
  struct
  {
    uint32 nPRIV:1;                    /*!< bit:      0  Execution privilege in Thread mode */
    uint32 SPSEL:1;                    /*!< bit:      1  Stack to be used */
    uint32 _reserved1:30;              /*!< bit:  2..31  Reserved */
  } b;                                   /*!< Structure used for bit  access */
  uint32 w;                            /*!< Type      used for word access */
} CONTROL_TYPEDEF;

/* CONTROL Register Definitions */
#define CONTROL_SPSEL_POS                   1U                                            /*!< CONTROL: SPSEL Position */
#define CONTROL_SPSEL_MSK                  (1UL << CONTROL_SPSEL_POS)                     /*!< CONTROL: SPSEL Mask */

#define CONTROL_nPRIV_POS                   0U                                            /*!< CONTROL: nPRIV Position */
#define CONTROL_nPRIV_MSK                  (1UL /*<< CONTROL_nPRIV_POS*/)                 /*!< CONTROL: nPRIV Mask */

/*@} end of group CMSIS_CORE */


/**
  \ingroup    CMSIS_core_register
  \defgroup   CMSIS_NVIC  Nested Vectored Interrupt Controller (NVIC)
  \brief      Type definitions for the NVIC Registers
  @{
 */

/**
  \brief  Structure type to access the Nested Vectored Interrupt Controller (NVIC).
 */
typedef struct
{
    volatile uint32 Enable[1];
    volatile uint32 RESERVED0[31];
    volatile uint32 Disable[1];
    volatile uint32 RSERVED1[31];
    volatile uint32 Set[1];
    volatile uint32 RESERVED2[31];
    volatile uint32 Clear[1];
    volatile uint32 RESERVED3[31];
    volatile uint32 RESERVED4[64];
    volatile uint32 Priority[8];
} NVIC_TYPEDEF;


#define NVIC                (*(NVIC_TYPEDEF      *)     NVIC_BASE     )   /*!< NVIC configuration struct */

#define NVIC_PRIO_BITS                      2 //! STM32G0xx uses 2 Bits for the Priority Levels
#define NVIC_INTERRUPT_PRIORITY_ARRAY       8
#define NVIC_INTERRUPT_PRIORITY_CLEAR       ((uint32)0x00000000)

#define NVIC_DISABLE_ALL_INTERRUPTS         ((uint32)0xFFFFFFFFU)
#define NVIC_CLEAR_ALL_INTERRUPTS           ((uint32)0xFFFFFFFFU)

/*@} end of group CMSIS_NVIC */


/**
  \ingroup  CMSIS_core_register
  \defgroup CMSIS_SCB     System Control Block (SCB)
  \brief    Type definitions for the System Control Block Registers
  @{
 */

/**
  \brief  Structure type to access the System Control Block (SCB).
 */
typedef struct
{
  volatile const  uint32 CPUID;                  /*!< Offset: 0x000 (R/ )  CPUID Base Register */
  volatile uint32 ICSR;                   /*!< Offset: 0x004 (R/W)  Interrupt Control and State Register */
  volatile uint32 VTOR;                   /*!< Offset: 0x008 (R/W)  Vector Table Offset Register */
  volatile uint32 AIRCR;                  /*!< Offset: 0x00C (R/W)  Application Interrupt and Reset Control Register */
  volatile uint32 SCR;                    /*!< Offset: 0x010 (R/W)  System Control Register */
  volatile uint32 CCR;                    /*!< Offset: 0x014 (R/W)  Configuration Control Register */
        uint32 RESERVED1;
  volatile uint32 SHP[2U];                /*!< Offset: 0x01C (R/W)  System Handlers Priority Registers. [0] is RESERVED */
  volatile uint32 SHCSR;                  /*!< Offset: 0x024 (R/W)  System Handler Control and State Register */
} SCB_TYPEDEF;

#define SCB                 (*(SCB_TYPEDEF*) SCB_BASE)   /*!< SCB configuration struct */

#define AIRC_VECTKEY_MASK               ((uint32)0x05FA0000)   // See section 2.7.4 STR750 Reference Manual


/* SCB CPUID Register Definitions */
#define SCB_CPUID_IMPLEMENTER_POS          24U                                            /*!< SCB CPUID: IMPLEMENTER Position */
#define SCB_CPUID_IMPLEMENTER_MSK          (0xFFUL << SCB_CPUID_IMPLEMENTER_POS)          /*!< SCB CPUID: IMPLEMENTER Mask */

#define SCB_CPUID_VARIANT_POS              20U                                            /*!< SCB CPUID: VARIANT Position */
#define SCB_CPUID_VARIANT_MSK              (0xFUL << SCB_CPUID_VARIANT_POS)               /*!< SCB CPUID: VARIANT Mask */

#define SCB_CPUID_ARCHITECTURE_POS         16U                                            /*!< SCB CPUID: ARCHITECTURE Position */
#define SCB_CPUID_ARCHITECTURE_MSK         (0xFUL << SCB_CPUID_ARCHITECTURE_POS)          /*!< SCB CPUID: ARCHITECTURE Mask */

#define SCB_CPUID_PARTNO_POS                4U                                            /*!< SCB CPUID: PARTNO Position */
#define SCB_CPUID_PARTNO_MSK               (0xFFFUL << SCB_CPUID_PARTNO_POS)              /*!< SCB CPUID: PARTNO Mask */

#define SCB_CPUID_REVISION_POS              0U                                            /*!< SCB CPUID: REVISION Position */
#define SCB_CPUID_REVISION_MSK             (0xFUL /*<< SCB_CPUID_REVISION_POS*/)          /*!< SCB CPUID: REVISION Mask */

/* SCB Interrupt Control State Register Definitions */
#define SCB_ICSR_NMIPENDSET_POS            31U                                            /*!< SCB ICSR: NMIPENDSET Position */
#define SCB_ICSR_NMIPENDSET_MSK            (1UL << SCB_ICSR_NMIPENDSET_POS)               /*!< SCB ICSR: NMIPENDSET Mask */

#define SCB_ICSR_PENDSVSET_POS             28U                                            /*!< SCB ICSR: PENDSVSET Position */
#define SCB_ICSR_PENDSVSET_MSK             (1UL << SCB_ICSR_PENDSVSET_POS)                /*!< SCB ICSR: PENDSVSET Mask */

#define SCB_ICSR_PENDSVCLR_POS             27U                                            /*!< SCB ICSR: PENDSVCLR Position */
#define SCB_ICSR_PENDSVCLR_MSK             (1UL << SCB_ICSR_PENDSVCLR_POS)                /*!< SCB ICSR: PENDSVCLR Mask */

#define SCB_ICSR_PENDSTSET_POS             26U                                            /*!< SCB ICSR: PENDSTSET Position */
#define SCB_ICSR_PENDSTSET_MSK             (1UL << SCB_ICSR_PENDSTSET_POS)                /*!< SCB ICSR: PENDSTSET Mask */

#define SCB_ICSR_PENDSTCLR_POS             25U                                            /*!< SCB ICSR: PENDSTCLR Position */
#define SCB_ICSR_PENDSTCLR_MSK             (1UL << SCB_ICSR_PENDSTCLR_POS)                /*!< SCB ICSR: PENDSTCLR Mask */

#define SCB_ICSR_ISRPREEMPT_POS            23U                                            /*!< SCB ICSR: ISRPREEMPT Position */
#define SCB_ICSR_ISRPREEMPT_MSK            (1UL << SCB_ICSR_ISRPREEMPT_POS)               /*!< SCB ICSR: ISRPREEMPT Mask */

#define SCB_ICSR_ISRPENDING_POS            22U                                            /*!< SCB ICSR: ISRPENDING Position */
#define SCB_ICSR_ISRPENDING_MSK            (1UL << SCB_ICSR_ISRPENDING_POS)               /*!< SCB ICSR: ISRPENDING Mask */

#define SCB_ICSR_VECTPENDING_POS           12U                                            /*!< SCB ICSR: VECTPENDING Position */
#define SCB_ICSR_VECTPENDING_MSK           (0x1FFUL << SCB_ICSR_VECTPENDING_POS)          /*!< SCB ICSR: VECTPENDING Mask */

#define SCB_ICSR_VECTACTIVE_POS             0U                                            /*!< SCB ICSR: VECTACTIVE Position */
#define SCB_ICSR_VECTACTIVE_MSK            (0x1FFUL /*<< SCB_ICSR_VECTACTIVE_POS*/)       /*!< SCB ICSR: VECTACTIVE Mask */

/* SCB Interrupt Control State Register Definitions */
#define SCB_VTOR_TBLOFF_POS                 8U                                            /*!< SCB VTOR: TBLOFF Position */
#define SCB_VTOR_TBLOFF_MSK                (0xFFFFFFUL << SCB_VTOR_TBLOFF_POS)            /*!< SCB VTOR: TBLOFF Mask */

/* SCB Application Interrupt and Reset Control Register Definitions */
#define SCB_AIRCR_VECTKEY_POS              16U                                            /*!< SCB AIRCR: VECTKEY Position */
#define SCB_AIRCR_VECTKEY_MSK              (0xFFFFUL << SCB_AIRCR_VECTKEY_POS)            /*!< SCB AIRCR: VECTKEY Mask */

#define SCB_AIRCR_VECTKEYSTAT_POS          16U                                            /*!< SCB AIRCR: VECTKEYSTAT Position */
#define SCB_AIRCR_VECTKEYSTAT_MSK          (0xFFFFUL << SCB_AIRCR_VECTKEYSTAT_POS)        /*!< SCB AIRCR: VECTKEYSTAT Mask */

#define SCB_AIRCR_ENDIANESS_POS            15U                                            /*!< SCB AIRCR: ENDIANESS Position */
#define SCB_AIRCR_ENDIANESS_MSK            (1UL << SCB_AIRCR_ENDIANESS_POS)               /*!< SCB AIRCR: ENDIANESS Mask */

#define SCB_AIRCR_SYSRESETREQ_POS           2U                                            /*!< SCB AIRCR: SYSRESETREQ Position */
#define SCB_AIRCR_SYSRESETREQ_MSK          (1UL << SCB_AIRCR_SYSRESETREQ_POS)             /*!< SCB AIRCR: SYSRESETREQ Mask */

#define SCB_AIRCR_VECTCLRACTIVE_POS         1U                                            /*!< SCB AIRCR: VECTCLRACTIVE Position */
#define SCB_AIRCR_VECTCLRACTIVE_MSK        (1UL << SCB_AIRCR_VECTCLRACTIVE_POS)           /*!< SCB AIRCR: VECTCLRACTIVE Mask */

/* SCB System Control Register Definitions */
#define SCB_SCR_SEVONPEND_POS               4U                                            /*!< SCB SCR: SEVONPEND Position */
#define SCB_SCR_SEVONPEND_MSK              (1UL << SCB_SCR_SEVONPEND_POS)                 /*!< SCB SCR: SEVONPEND Mask */

#define SCB_SCR_SLEEPDEEP_POS               2U                                            /*!< SCB SCR: SLEEPDEEP Position */
#define SCB_SCR_SLEEPDEEP_MSK              (1UL << SCB_SCR_SLEEPDEEP_POS)                 /*!< SCB SCR: SLEEPDEEP Mask */

#define SCB_SCR_SLEEPONEXIT_POS             1U                                            /*!< SCB SCR: SLEEPONEXIT Position */
#define SCB_SCR_SLEEPONEXIT_MSK            (1UL << SCB_SCR_SLEEPONEXIT_POS)               /*!< SCB SCR: SLEEPONEXIT Mask */

/* SCB Configuration Control Register Definitions */
#define SCB_CCR_STKALIGN_POS                9U                                            /*!< SCB CCR: STKALIGN Position */
#define SCB_CCR_STKALIGN_MSK               (1UL << SCB_CCR_STKALIGN_POS)                  /*!< SCB CCR: STKALIGN Mask */

#define SCB_CCR_UNALIGN_TRP_POS             3U                                            /*!< SCB CCR: UNALIGN_TRP Position */
#define SCB_CCR_UNALIGN_TRP_MSK            (1UL << SCB_CCR_UNALIGN_TRP_POS)               /*!< SCB CCR: UNALIGN_TRP Mask */

/* SCB System Handler Control and State Register Definitions */
#define SCB_SHCSR_SVCALLPENDED_POS         15U                                            /*!< SCB SHCSR: SVCALLPENDED Position */
#define SCB_SHCSR_SVCALLPENDED_MSK         (1UL << SCB_SHCSR_SVCALLPENDED_POS)            /*!< SCB SHCSR: SVCALLPENDED Mask */

/*@} end of group CMSIS_SCB */


/**
  \ingroup  CMSIS_core_register
  \defgroup CMSIS_SYSTICK     System Tick Timer (SYSTICK)
  \brief    Type definitions for the System Timer Registers.
  @{
 */

/**
  \brief  Structure type to access the System Timer (SYSTICK).
 */
typedef struct
{
  volatile uint32 CTRL;                   /*!< Offset: 0x000 (R/W)  SYSTICK Control and Status Register */
  volatile uint32 LOAD;                   /*!< Offset: 0x004 (R/W)  SYSTICK Reload Value Register */
  volatile uint32 VAL;                    /*!< Offset: 0x008 (R/W)  SYSTICK Current Value Register */
  volatile const  uint32 CALIB;                  /*!< Offset: 0x00C (R/ )  SYSTICK Calibration Register */
} SYSTICK_TYPEDEF;


#define SYSTICK    (*(volatile SYSTICK_TYPEDEF *)SYSTICK_BASE)

#define SYSTICK_COUNTER_DISABLE             ((uint32)0xFFFFFFFEU)
#define SYSTICK_COUNTER_ENABLE              ((uint32)0x00000001)
#define SYSTICK_COUNTER_CLEAR               ((uint32)0x00000000)


/* SYSTICK Control / Status Register Definitions */
#define SYSTICK_CTRL_COUNTFLAG_POS         16U                                            /*!< SYSTICK CTRL: COUNTFLAG Position */
#define SYSTICK_CTRL_COUNTFLAG_MSK         (1UL << SYSTICK_CTRL_COUNTFLAG_POS)            /*!< SYSTICK CTRL: COUNTFLAG Mask */

#define SYSTICK_CTRL_CLKSOURCE_POS          2U                                            /*!< SYSTICK CTRL: CLKSOURCE Position */
#define SYSTICK_CTRL_CLKSOURCE_MSK         (1UL << SYSTICK_CTRL_CLKSOURCE_POS)            /*!< SYSTICK CTRL: CLKSOURCE Mask */

#define SYSTICK_CTRL_TICKINT_POS            1U                                            /*!< SYSTICK CTRL: TICKINT Position */
#define SYSTICK_CTRL_TICKINT_MSK           (1UL << SYSTICK_CTRL_TICKINT_POS)              /*!< SYSTICK CTRL: TICKINT Mask */

#define SYSTICK_CTRL_ENABLE_POS             0U                                            /*!< SYSTICK CTRL: ENABLE Position */
#define SYSTICK_CTRL_ENABLE_MSK            (1UL /*<< SYSTICK_CTRL_ENABLE_POS*/)           /*!< SYSTICK CTRL: ENABLE Mask */

/* SYSTICK Reload Register Definitions */
#define SYSTICK_LOAD_RELOAD_POS             0U                                            /*!< SYSTICK LOAD: RELOAD Position */
#define SYSTICK_LOAD_RELOAD_MSK            (0xFFFFFFUL /*<< SYSTICK_LOAD_RELOAD_POS*/)    /*!< SYSTICK LOAD: RELOAD Mask */

/* SYSTICK Current Register Definitions */
#define SYSTICK_VAL_CURRENT_POS             0U                                            /*!< SYSTICK VAL: CURRENT Position */
#define SYSTICK_VAL_CURRENT_MSK            (0xFFFFFFUL /*<< SYSTICK_VAL_CURRENT_POS*/)    /*!< SYSTICK VAL: CURRENT Mask */

/* SYSTICK Calibration Register Definitions */
#define SYSTICK_CALIB_NOREF_POS            31U                                            /*!< SYSTICK CALIB: NOREF Position */
#define SYSTICK_CALIB_NOREF_MSK            (1UL << SYSTICK_CALIB_NOREF_POS)               /*!< SYSTICK CALIB: NOREF Mask */

#define SYSTICK_CALIB_SKEW_POS             30U                                            /*!< SYSTICK CALIB: SKEW Position */
#define SYSTICK_CALIB_SKEW_MSK             (1UL << SYSTICK_CALIB_SKEW_POS)                /*!< SYSTICK CALIB: SKEW Mask */

#define SYSTICK_CALIB_TENMS_POS             0U                                            /*!< SYSTICK CALIB: TENMS Position */
#define SYSTICK_CALIB_TENMS_MSK            (0xFFFFFFUL /*<< SYSTICK_CALIB_TENMS_POS*/)    /*!< SYSTICK CALIB: TENMS Mask */

/*@} end of group CMSIS_SYSTICK */


#endif

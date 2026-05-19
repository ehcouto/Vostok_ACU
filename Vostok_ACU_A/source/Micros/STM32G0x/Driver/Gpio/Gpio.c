/**
 *  @file
 *  @defgroup   CLASS_B
 *
 *  @brief      This module provides a standard way to access the GPIO ports and port-bits
 *
 *  @details    This module provides the interface to the application.
 *              It contains all the necessary mechanisms to ensure that configuration, reading and
 *              writing to safety relevant Gpio pin is safe
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "Gpio.h"
#include "Micro.h"
#include "SRException.h"

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#ifndef GPIO_PROTECTION_FEATURE
    // By default the feature is ENABLED
    #define GPIO_PROTECTION_FEATURE   ENABLED
#endif

typedef enum
{
    MODE_INPUT_MODE = 0,
    MODE_OUTPUT_MODE,
    MODE_AF_MODE,
    MODE_ANALOG_MODE
}GPIO_MODER;

typedef enum
{
    OTY_OUTPUT_PUSHPULL = 0,
    OTY_OUTPUT_OPENDRAIN,
}GPIO_OTYPER;

typedef enum
{
    SPEED_2MHZ = 0,
    SPEED_10MHZ,
    SPEED_50MHZ
}GPIO_OSPEEDER;

typedef enum
{
    NOPULLUP_NO_PULLDOWN = 0,
    PULLUP,
    PULLDOWN
}GPIO_PUPDR;

#define ALL_INPUT_FLOAT_MODER                       (uint32)0x00000000
#define ALL_INPUT_FLOAT_OTYPER                      (uint32)0x00000000
#define ALL_INPUT_FLOAT_PUPDR                       (uint32)0x00000000

#define ALL_INPUT_PULLUP_MODER                      (uint32)0x00000000
#define ALL_INPUT_PULLUP_OTYPER                     (uint32)0x00000000
#define ALL_INPUT_PULLUP_PUPDR                      (uint32)0x55555555

#define ALL_INPUT_PULLDOWN_MODER                    (uint32)0x00000000
#define ALL_INPUT_PULLDOWN_OTYPER                   (uint32)0x00000000
#define ALL_INPUT_PULLDOWN_PUPDR                    (uint32)0xAAAAAAAA

#define ALL_INPUT_ANALOG_MODER                      (uint32)0xFFFFFFFF
#define ALL_INPUT_ANALOG_OTYPER                     (uint32)0x00000000
#define ALL_INPUT_ANALOG_PUPDR                      (uint32)0x00000000

#define ALL_SPECIAL_FUNCTION_OPEN_DRAIN_MODER       (uint32)0xAAAAAAAA
#define ALL_SPECIAL_FUNCTION_OPEN_DRAIN_OTYPER      (uint32)0x0000FFFF
#define ALL_SPECIAL_FUNCTION_OPEN_DRAIN_PUPDR       (uint32)0x00000000

#define ALL_SPECIAL_FUNCTION_PUSH_PULL_MODER        (uint32)0xAAAAAAAA
#define ALL_SPECIAL_FUNCTION_PUSH_PULL_OTYPER       (uint32)0x00000000
#define ALL_SPECIAL_FUNCTION_PUSH_PULL_PUPDR        (uint32)0x00000000

#define ALL_OUTPUT_PUSHPULL_MODER                   (uint32)0x55555555
#define ALL_OUTPUT_PUSHPULL_OTYPER                  (uint32)0x00000000
#define ALL_OUTPUT_PUSHPULL_PUPDR                   (uint32)0x00000000

#define ALL_OUTPUT_OPEN_DRAIN_MODER                 (uint32)0x55555555
#define ALL_OUTPUT_OPEN_DRAIN_OTYPER                (uint32)0x0000FFFF
#define ALL_OUTPUT_OPEN_DRAIN_PUPDR                 (uint32)0x00000000

#define ORD_PULL                                    (uint32)0x0000FFFF


#define LOWER_SHORT_MASK                            (uint32)0x0000FFFF
#define HIGHER_SHORT_MASK                           (uint32)0xFFFF0000
#define SHORT_SHIFT                                 16

/*
 * The below tables are based on the PIN_TYPE_DEF which is defined in the uc_STM32F0x.h file -
 *      typedef enum
 *      {
 *          INPUT_FLOAT,
 *          INPUT_PULLUP,
 *          INPUT_PULLDOWN,
 *          INPUT_ANALOG,
 *          SPECIAL_FUNCTION_OPEN_DRAIN,
 *          SPECIAL_FUNCTION_PUSH_PULL,
 *          OUTPUT_PUSHPULL,
 *          OUTPUT_OPEN_DRAIN
 *      } PIN_TYPE_DEF;
 */

//! Configuration register type                      INPUT_FLOAT,           INPUT_PULLUP,           INPUT_PULLDOWN,         INPUT_ANALOG,           SPECIAL_FUNCTION_OPEN_DRAIN     SPECIAL_FUNCTION_PUSH_PULL      OUTPUT_PUSHPULL         OUTPUT_OPEN_DRAIN
static const GPIO_MODER  PIN_CONFIG_MODER[8]     =  {MODE_INPUT_MODE,       MODE_INPUT_MODE,        MODE_INPUT_MODE,        MODE_ANALOG_MODE,       MODE_AF_MODE,                   MODE_AF_MODE,                   MODE_OUTPUT_MODE,       MODE_OUTPUT_MODE };
static const GPIO_OTYPER PIN_CONFIG_OTYPER[8]    =  {OTY_OUTPUT_PUSHPULL,   OTY_OUTPUT_PUSHPULL,    OTY_OUTPUT_PUSHPULL,    OTY_OUTPUT_PUSHPULL,    OTY_OUTPUT_OPENDRAIN,           OTY_OUTPUT_PUSHPULL,            OTY_OUTPUT_PUSHPULL,    OTY_OUTPUT_OPENDRAIN};
static const GPIO_PUPDR  PIN_CONFIG_PUPDR[8]     =  {NOPULLUP_NO_PULLDOWN,  PULLUP,                 PULLDOWN,               NOPULLUP_NO_PULLDOWN,   NOPULLUP_NO_PULLDOWN,           NOPULLUP_NO_PULLDOWN,           NOPULLUP_NO_PULLDOWN,   NOPULLUP_NO_PULLDOWN};

//! Configuration register type                             INPUT_FLOAT,               INPUT_PULLUP,               INPUT_PULLDOWN,             INPUT_ANALOG,             SPECIAL_FUNCTION_OPEN_DRAIN               SPECIAL_FUNCTION_PUSH_PULL              OUTPUT_PUSHPULL             OUTPUT_OPEN_DRAIN
static const uint32 PORT_CONFIG_MODER[8]     =  {ALL_INPUT_FLOAT_MODER,     ALL_INPUT_PULLUP_MODER,     ALL_INPUT_PULLDOWN_MODER,   ALL_INPUT_ANALOG_MODER,   ALL_SPECIAL_FUNCTION_OPEN_DRAIN_MODER,    ALL_SPECIAL_FUNCTION_PUSH_PULL_MODER,   ALL_OUTPUT_PUSHPULL_MODER,  ALL_OUTPUT_OPEN_DRAIN_MODER };
static const uint32 PORT_CONFIG_OTYPER[8]    =  {ALL_INPUT_FLOAT_OTYPER,    ALL_INPUT_PULLUP_OTYPER,    ALL_INPUT_PULLDOWN_OTYPER,  ALL_INPUT_ANALOG_OTYPER,  ALL_SPECIAL_FUNCTION_OPEN_DRAIN_OTYPER,   ALL_SPECIAL_FUNCTION_PUSH_PULL_OTYPER,  ALL_OUTPUT_PUSHPULL_OTYPER, ALL_OUTPUT_OPEN_DRAIN_OTYPER};
static const uint32 PORT_CONFIG_PUPDR[8]     =  {ALL_INPUT_FLOAT_PUPDR,     ALL_INPUT_PULLUP_PUPDR,     ALL_INPUT_PULLDOWN_PUPDR,   ALL_INPUT_ANALOG_PUPDR,   ALL_SPECIAL_FUNCTION_OPEN_DRAIN_PUPDR,    ALL_SPECIAL_FUNCTION_PUSH_PULL_PUPDR,   ALL_OUTPUT_PUSHPULL_PUPDR,  ALL_OUTPUT_OPEN_DRAIN_PUPDR };

const uint16 GPIO_SET_MASK[] = {0x0001,0x0002,0x0004,0x0008,0x0010,0x0020,0x0040,0x0080,0x0100,0x0200,0x0400,0x0800,0x1000,0x2000,0x4000,0x8000};
const uint32 GPIO_CLR_MASK[] = {0xFFFFFFFC,0xFFFFFFF3,0xFFFFFFCF,0xFFFFFF3F,0xFFFFFCFF,0xFFFFF3FF,0xFFFFCFFF,0xFFFF3FFF,0xFFFCFFFF,0xFFF3FFFF,0xFFCFFFFF,0xFF3FFFFF,0xFCFFFFFF,0xF3FFFFFF,0xCFFFFFFF,0x3FFFFFFF};

const volatile GPIO_DEF * GPIO_PORT_LIST[] = {  GPIOA,  //lint !e923 Lint: 923 cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1], [MISRA 2004 Rule 11.3]
                                                GPIOB,  //lint !e923 Lint: 923 cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1], [MISRA 2004 Rule 11.3]
                                                GPIOC,  //lint !e923 Lint: 923 cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1], [MISRA 2004 Rule 11.3]
                                                GPIOD,  //lint !e923 Lint: 923 cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1], [MISRA 2004 Rule 11.3]
                                                GPIOE,  //lint !e923 Lint: 923 cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1], [MISRA 2004 Rule 11.3]
                                                GPIOF}; //lint !e923 Lint: 923 cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1], [MISRA 2004 Rule 11.3]


//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static uint32 UpdateMaskedRegister(uint32 current,uint32 mask, uint32 value);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method Initializes the GPIO module and its variables
 *    @details  Connects the GPIO module to the clock Bus and calls the Gpio__RefreshConfig() which configures the
 *              Gpio Ports to there default configuration as specified in the Gpio_prm.h file
 */
void Gpio__Initialize(void)
{
    RCC_CLOCK_ENABLE_FOR_ALL_GPIO_PORTS();
    // Connect the GPIOA, GPIOB, GPIOC, GPIOD and GPIOF  device to the clock BUS
    //lint !e923 Lint: 923 cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1], [MISRA 2004 Rule 11.3]

    Gpio__RefreshConfig();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method refreshes the configuration of the port.
 *    @details  Configures the Gpio Ports to there default configuration as specified in the Gpio_prm.h file
 *              Interrupt are disabled before assigning value to register and enabled after
 *              value is assigned
 */
void Gpio__RefreshConfig(void)
{
    #if (GPIO_PROTECTION_FEATURE == ENABLED)
        MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    #endif

    #if (GPIO_PROTECTION_FEATURE == ENABLED)
        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL(); // It saves and disable interrupts
    #endif

   //Refresh PORTA Registers
    #ifdef PORTA_MODER_MASK
        #ifdef PORTA_DEFAULT_MODER
            GPIO0.MODER = UpdateMaskedRegister(GPIO0.MODER, PORTA_MODER_MASK, PORTA_DEFAULT_MODER);
        #endif
    #else
        #ifdef PORTA_DEFAULT_MODER
            GPIO0.MODER = PORTA_DEFAULT_MODER;
        #endif
    #endif

    #ifdef PORTA_OTYPER_MASK
        #ifdef PORTA_DEFAULT_OTYPER
            GPIO0.OTYPER = UpdateMaskedRegister(GPIO0.OTYPER, PORTA_OTYPER_MASK, PORTA_DEFAULT_OTYPER);
        #endif
    #else
        #ifdef PORTA_DEFAULT_OTYPER
            GPIO0.OTYPER = PORTA_DEFAULT_OTYPER;
        #endif
    #endif

    #ifdef PORTA_PUPDR_MASK
        #ifdef PORTA_DEFAULT_PUPDR
            GPIO0.PUPDR = UpdateMaskedRegister(GPIO0.PUPDR, PORTA_PUPDR_MASK, PORTA_DEFAULT_PUPDR);
        #endif
    #else
        #ifdef PORTA_DEFAULT_PUPDR
            GPIO0.PUPDR = PORTA_DEFAULT_PUPDR;
        #endif
    #endif

    #ifdef PORTA_OSPEEDR_MASK
        #ifdef PORTA_DEFAULT_OSPEEDR
            GPIO0.OSPEEDR = UpdateMaskedRegister(GPIO0.OSPEEDR, PORTA_OSPEEDR_MASK, PORTA_DEFAULT_OSPEEDR);
        #endif
    #else
        #ifdef PORTA_DEFAULT_OSPEEDR
            GPIO0.OSPEEDR = PORTA_DEFAULT_OSPEEDR;
        #endif
    #endif

    #ifdef PORTA_BSRR_MASK
        #ifdef PORTA_DEFAULT_BSRR
            GPIO0.BSRR = UpdateMaskedRegister(GPIO0.BSRR, PORTA_BSRR_MASK, PORTA_DEFAULT_BSRR);
        #endif
    #else
        #ifdef PORTA_DEFAULT_BSRR
            GPIO0.BSRR = PORTA_DEFAULT_BSRR;
        #endif
    #endif


   //Refresh PORTB Registers
    #ifdef PORTB_MODER_MASK
        #ifdef PORTB_DEFAULT_MODER
            GPIO1.MODER = UpdateMaskedRegister(GPIO1.MODER, PORTB_MODER_MASK, PORTB_DEFAULT_MODER);
        #endif
    #else
        #ifdef PORTB_DEFAULT_MODER
            GPIO1.MODER = PORTB_DEFAULT_MODER;
        #endif
    #endif

    #ifdef PORTB_OTYPER_MASK
        #ifdef PORTB_DEFAULT_OTYPER
            GPIO1.OTYPER = UpdateMaskedRegister(GPIO1.OTYPER, PORTB_OTYPER_MASK, PORTB_DEFAULT_OTYPER);
        #endif
    #else
        #ifdef PORTB_DEFAULT_OTYPER
            GPIO1.OTYPER = PORTB_DEFAULT_OTYPER;
        #endif
    #endif

    #ifdef PORTB_PUPDR_MASK
        #ifdef PORTB_DEFAULT_PUPDR
            GPIO1.PUPDR = UpdateMaskedRegister(GPIO1.PUPDR, PORTB_PUPDR_MASK, PORTB_DEFAULT_PUPDR);
        #endif
    #else
        #ifdef PORTB_DEFAULT_PUPDR
            GPIO1.PUPDR = PORTB_DEFAULT_PUPDR;
        #endif
    #endif

    #ifdef PORTB_OSPEEDR_MASK
        #ifdef PORTB_DEFAULT_OSPEEDR
            GPIO1.OSPEEDR = UpdateMaskedRegister(GPIO1.OSPEEDR, PORTB_OSPEEDR_MASK, PORTB_DEFAULT_OSPEEDR);
        #endif
    #else
        #ifdef PORTB_DEFAULT_OSPEEDR
            GPIO1.OSPEEDR = PORTB_DEFAULT_OSPEEDR;
        #endif
    #endif

    #ifdef PORTB_BSRR_MASK
        #ifdef PORTB_DEFAULT_BSRR
            GPIO1.BSRR = UpdateMaskedRegister(GPIO1.BSRR, PORTB_BSRR_MASK, PORTB_DEFAULT_BSRR);
        #endif
    #else
        #ifdef PORTB_DEFAULT_BSRR
            GPIO1.BSRR = PORTB_DEFAULT_BSRR;
        #endif
    #endif

   //Refresh PORTC Registers
    #ifdef PORTC_MODER_MASK
        #ifdef PORTC_DEFAULT_MODER
            GPIO2.MODER = UpdateMaskedRegister(GPIO2.MODER, PORTC_MODER_MASK, PORTC_DEFAULT_MODER);
        #endif
    #else
        #ifdef PORTC_DEFAULT_MODER
            GPIO2.MODER = PORTC_DEFAULT_MODER;
        #endif
    #endif

    #ifdef PORTC_OTYPER_MASK
        #ifdef PORTC_DEFAULT_OTYPER
            GPIO2.OTYPER = UpdateMaskedRegister(GPIO2.OTYPER, PORTC_OTYPER_MASK, PORTC_DEFAULT_OTYPER);
        #endif
    #else
        #ifdef PORTC_DEFAULT_OTYPER
            GPIO2.OTYPER = PORTC_DEFAULT_OTYPER;
        #endif
    #endif

    #ifdef PORTC_PUPDR_MASK
        #ifdef PORTC_DEFAULT_PUPDR
            GPIO2.PUPDR = UpdateMaskedRegister(GPIO2.PUPDR, PORTC_PUPDR_MASK, PORTC_DEFAULT_PUPDR);
        #endif
    #else
        #ifdef PORTC_DEFAULT_PUPDR
            GPIO2.PUPDR = PORTC_DEFAULT_PUPDR;
        #endif
    #endif

    #ifdef PORTC_OSPEEDR_MASK
        #ifdef PORTC_DEFAULT_OSPEEDR
            GPIO2.OSPEEDR = UpdateMaskedRegister(GPIO2.OSPEEDR, PORTC_OSPEEDR_MASK, PORTC_DEFAULT_OSPEEDR);
        #endif
    #else
        #ifdef PORTC_DEFAULT_OSPEEDR
            GPIO2.OSPEEDR = PORTC_DEFAULT_OSPEEDR;
        #endif
    #endif

    #ifdef PORTC_BSRR_MASK
        #ifdef PORTC_DEFAULT_BSRR
            GPIO2.BSRR = UpdateMaskedRegister(GPIO2.BSRR, PORTC_BSRR_MASK, PORTC_DEFAULT_BSRR);
        #endif
    #else
        #ifdef PORTC_DEFAULT_BSRR
            GPIO2.BSRR = PORTC_DEFAULT_BSRR;
        #endif
    #endif

   //Refresh PORTD Registers
    #ifdef PORTD_MODER_MASK
        #ifdef PORTD_DEFAULT_MODER
            GPIO3.MODER = UpdateMaskedRegister(GPIO3.MODER, PORTD_MODER_MASK, PORTD_DEFAULT_MODER);
        #endif
    #else
        #ifdef PORTD_DEFAULT_MODER
            GPIO3.MODER = PORTD_DEFAULT_MODER;
        #endif
    #endif

    #ifdef PORTD_OTYPER_MASK
        #ifdef PORTD_DEFAULT_OTYPER
            GPIO3.OTYPER = UpdateMaskedRegister(GPIO3.OTYPER, PORTD_OTYPER_MASK, PORTD_DEFAULT_OTYPER);
        #endif
    #else
        #ifdef PORTD_DEFAULT_OTYPER
            GPIO3.OTYPER = PORTD_DEFAULT_OTYPER;
        #endif
    #endif

    #ifdef PORTD_PUPDR_MASK
        #ifdef PORTD_DEFAULT_PUPDR
            GPIO3.PUPDR = UpdateMaskedRegister(GPIO3.PUPDR, PORTD_PUPDR_MASK, PORTD_DEFAULT_PUPDR);
        #endif
    #else
        #ifdef PORTD_DEFAULT_PUPDR
            GPIO3.PUPDR = PORTD_DEFAULT_PUPDR;
        #endif
    #endif

    #ifdef PORTD_OSPEEDR_MASK
        #ifdef PORTD_DEFAULT_OSPEEDR
            GPIO3.OSPEEDR = UpdateMaskedRegister(GPIO3.OSPEEDR, PORTD_OSPEEDR_MASK, PORTD_DEFAULT_OSPEEDR);
        #endif
    #else
        #ifdef PORTD_DEFAULT_OSPEEDR
            GPIO3.OSPEEDR = PORTD_DEFAULT_OSPEEDR;
        #endif
    #endif

    #ifdef PORTD_BSRR_MASK
        #ifdef PORTD_DEFAULT_BSRR
            GPIO3.BSRR = UpdateMaskedRegister(GPIO3.BSRR, PORTD_BSRR_MASK, PORTD_DEFAULT_BSRR);
        #endif
    #else
        #ifdef PORTD_DEFAULT_BSRR
            GPIO3.BSRR = PORTD_DEFAULT_BSRR;
        #endif
    #endif

   //Refresh PORTE Registers
    #ifdef PORTE_MODER_MASK
        #ifdef PORTE_DEFAULT_MODER
            GPIO4.MODER = UpdateMaskedRegister(GPIO4.MODER, PORTE_MODER_MASK, PORTE_DEFAULT_MODER);
        #endif
    #else
        #ifdef PORTE_DEFAULT_MODER
            GPIO4.MODER = PORTE_DEFAULT_MODER;
        #endif
    #endif

    #ifdef PORTE_OTYPER_MASK
        #ifdef PORTE_DEFAULT_OTYPER
            GPIO4.OTYPER = UpdateMaskedRegister(GPIO4.OTYPER, PORTE_OTYPER_MASK, PORTE_DEFAULT_OTYPER);
        #endif
    #else
        #ifdef PORTE_DEFAULT_OTYPER
            GPIO4.OTYPER = PORTE_DEFAULT_OTYPER;
        #endif
    #endif

    #ifdef PORTE_PUPDR_MASK
        #ifdef PORTE_DEFAULT_PUPDR
            GPIO4.PUPDR = UpdateMaskedRegister(GPIO4.PUPDR, PORTE_PUPDR_MASK, PORTE_DEFAULT_PUPDR);
        #endif
    #else
        #ifdef PORTE_DEFAULT_PUPDR
            GPIO4.PUPDR = PORTE_DEFAULT_PUPDR;
        #endif
    #endif

    #ifdef PORTE_OSPEEDR_MASK
        #ifdef PORTE_DEFAULT_OSPEEDR
            GPIO4.OSPEEDR = UpdateMaskedRegister(GPIO4.OSPEEDR, PORTE_OSPEEDR_MASK, PORTE_DEFAULT_OSPEEDR);
        #endif
    #else
        #ifdef PORTE_DEFAULT_OSPEEDR
            GPIO4.OSPEEDR = PORTE_DEFAULT_OSPEEDR;
        #endif
    #endif

    #ifdef PORTE_BSRR_MASK
        #ifdef PORTE_DEFAULT_BSRR
            GPIO4.BSRR = UpdateMaskedRegister(GPIO4.BSRR, PORTE_BSRR_MASK, PORTE_DEFAULT_BSRR);
        #endif
    #else
        #ifdef PORTE_DEFAULT_BSRR
            GPIO4.BSRR = PORTE_DEFAULT_BSRR;
        #endif
    #endif

   //Refresh PORTF Registers
    #ifdef PORTF_MODER_MASK
        #ifdef PORTF_DEFAULT_MODER
            GPIO5.MODER = UpdateMaskedRegister(GPIO5.MODER, PORTF_MODER_MASK, PORTF_DEFAULT_MODER);
        #endif
    #else
        #ifdef PORTF_DEFAULT_MODER
            GPIO5.MODER = PORTF_DEFAULT_MODER;
        #endif
    #endif

    #ifdef PORTF_OTYPER_MASK
        #ifdef PORTF_DEFAULT_OTYPER
            GPIO5.OTYPER = UpdateMaskedRegister(GPIO5.OTYPER, PORTF_OTYPER_MASK, PORTF_DEFAULT_OTYPER);
        #endif
    #else
        #ifdef PORTF_DEFAULT_OTYPER
            GPIO5.OTYPER = PORTF_DEFAULT_OTYPER;
        #endif
    #endif

    #ifdef PORTF_PUPDR_MASK
        #ifdef PORTF_DEFAULT_PUPDR
            GPIO5.PUPDR = UpdateMaskedRegister(GPIO5.PUPDR, PORTF_PUPDR_MASK, PORTF_DEFAULT_PUPDR);
        #endif
    #else
        #ifdef PORTF_DEFAULT_PUPDR
            GPIO5.PUPDR = PORTF_DEFAULT_PUPDR;
        #endif
    #endif

    #ifdef PORTF_OSPEEDR_MASK
        #ifdef PORTF_DEFAULT_OSPEEDR
            GPIO5.OSPEEDR = UpdateMaskedRegister(GPIO5.OSPEEDR, PORTF_OSPEEDR_MASK, PORTF_DEFAULT_OSPEEDR);
        #endif
    #else
        #ifdef PORTF_DEFAULT_OSPEEDR
            GPIO5.OSPEEDR = PORTF_DEFAULT_OSPEEDR;
        #endif
    #endif

    #ifdef PORTF_BSRR_MASK
        #ifdef PORTF_DEFAULT_BSRR
            GPIO5.BSRR = UpdateMaskedRegister(GPIO5.BSRR, PORTF_BSRR_MASK, PORTF_DEFAULT_BSRR);
        #endif
    #else
        #ifdef PORTF_DEFAULT_BSRR
            GPIO5.BSRR = PORTF_DEFAULT_BSRR;
        #endif
    #endif

    #if (GPIO_PROTECTION_FEATURE == ENABLED)
            MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();  // Reestablish the interruption configuration
    #endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method configures a port-bit to a specific state.
 *    @details  Interrupt are disabled before assigning value to register and enabled after
 *              value is assigned
 *    @param    port: Defines the specific port
 *    @param    pin : Defines the specific pin
 *    @param    type: Defines the specific type the selected pin will configure as
 */
void Gpio__PinConfig(GPIO_PORT_TYPE port,uint8 pin,GPIO_PIN_MODE_TYPE type)
{
    #if (GPIO_PROTECTION_FEATURE == ENABLED)
        MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    #endif

    volatile GPIO_DEF * port_ptr;

    port_ptr = (volatile GPIO_DEF *)GPIO_PORT_LIST[port];   //lint !e929 Lint: 929 cast from pointer to pointer [MISRA 2004 Rule 11.4]

    if (Gpio__Validate(port,pin) == PASS)
    {
        #if (GPIO_PROTECTION_FEATURE == ENABLED)
            MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();          // Save interrupt state and disable interrupt
        #endif

        if (type >= SPECIAL_FUNCTION_OPEN_DRAIN)
        {
            // Speed mode configuration
            port_ptr->OSPEEDR = GPIO_OSPEEDER_50MHZ_SPEED_ALL;

            // Output mode configuration
            port_ptr->OTYPER = ((port_ptr->OTYPER & (~((GPIO_OTYPER_OT_0) << ((uint16)pin)))) | (uint16)(((uint16)PIN_CONFIG_OTYPER[type]) << ((uint16)pin)));
        }

        port_ptr->MODER = ((port_ptr->MODER  & GPIO_CLR_MASK[pin]) | (((uint32)PIN_CONFIG_MODER[type]) << (pin << 1)));

        // Pull-up Pull down register configuration
        port_ptr->PUPDR = ((port_ptr->PUPDR  & GPIO_CLR_MASK[pin]) | (((uint32)PIN_CONFIG_PUPDR[type]) << (pin << 1)));

        #if (GPIO_PROTECTION_FEATURE == ENABLED)
            MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();  // Reestablish the interruption configuration
        #endif
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method writes on a specific Port-Bit
 *    @details  For the Cortex M3 there's no access to the ORD register, to set or reset a bit on that register
 *              it must be done through the BSRR (Bit set and reset register)
 *    @param    port: Defines the specific port
 *    @param    pin : Defines the specific pin
 *    @param    value: Defines the value to be applied on the port-bit
 */
void Gpio__PinWrite(GPIO_PORT_TYPE port,uint8 pin,BOOL_TYPE value)
{
    uint32 mask;
    #if (GPIO_PROTECTION_FEATURE == ENABLED)
        MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    #endif
    volatile GPIO_DEF * port_ptr;

    port_ptr = (volatile GPIO_DEF *)GPIO_PORT_LIST[port];   //lint !e929 Lint: 929 cast from pointer to pointer [MISRA 2004 Rule 11.4]

    if (Gpio__Validate(port,pin) == PASS)       // It protects against attempts to write out of the range
    {
        #if (GPIO_PROTECTION_FEATURE == ENABLED)
            MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
        #endif

        // The 16 LSB bits of the BSRR are related to SET the output port-bit
        // The 16 MSB bits of the BSRR are related to RESET the output port-bit
        mask = 1<<((uint32)pin);            // It creates a mask with the related bit = 1
        if (value)                                  // If the request is to reset the related bit
        {
            port_ptr->BSRR = mask;                          // Write over the actual register
        }
        else
        {
            port_ptr->BRR = mask;                           // Write over the actual register
        }

        #if (GPIO_PROTECTION_FEATURE == ENABLED)
            MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
        #endif
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method reads the a specific Port-bit status
 *    @details  Reads the IDR register
 *    @param    port: Defines the specific port
 *    @param    pin : Defines the specific pin
 *    @return   result
 *      @retval TRUE: The port-bit is in high level
 *      @retval FALSE: The port-bit is in low level
 */
BOOL_TYPE Gpio__PinRead(GPIO_PORT_TYPE port,uint8 pin)
{
    if (Gpio__Validate(port,pin) == PASS)       // It protects against attempts to write out of the range
    {
        if (BIT_TEST(((volatile GPIO_DEF *)GPIO_PORT_LIST[port])->IDR,pin))             //lint !e929 Lint: 929 cast from pointer to pointer [MISRA 2004 Rule 11.4]
        {
            return (TRUE);                      // Change the response to TRUE
        }
    }

    return (FALSE);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method configures a port to a specific state.
 *    @details  By default configures the port to ALL_INPUT_FLOAT mode
 *    @param    port: Defines the specific port
 *    @param    type: Defines the specific type the selected port will configure as
 */
void Gpio__PortConfig(GPIO_PORT_TYPE port, GPIO_PIN_MODE_TYPE type)
{

    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    volatile GPIO_DEF * port_ptr;

    port_ptr = (volatile GPIO_DEF *)GPIO_PORT_LIST[port];   //lint !e929 Lint: 929 cast from pointer to pointer [MISRA 2004 Rule 11.4]

    if (Gpio__Validate(port,0) == PASS)       // It protects against attempts to write out of the range
    {
        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
        if (type >= SPECIAL_FUNCTION_OPEN_DRAIN)
        {
            // Speed mode configuration
            port_ptr->OSPEEDR = GPIO_OSPEEDER_50MHZ_SPEED_ALL;

            // Output mode configuration
            port_ptr->OTYPER = PORT_CONFIG_OTYPER[type];
        }

        //Configure the Mode
        port_ptr->MODER = PORT_CONFIG_MODER[type];

        // Pull-up Pull down register configuration
        port_ptr->PUPDR = PORT_CONFIG_PUPDR[type];
        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method writes data on a specific port using a mask to define which bits will be affected
 *    @details  For the Cortex M3 there's no access to the ORD register, to set or reset a bit on that register
 *              it must be done through the BSRR (Bit set and reset register)
 *    @param    port: Defines the specific port
 *    @param    mask: mask of affected bits (1 = will be affected, 0 = won't be affected)
 *    @param    value: Defines the value to be applied on the port-bit
 */
void Gpio__PortWrite(GPIO_PORT_TYPE port,PORT_DATA_TYPE mask, PORT_DATA_TYPE value)
{
    uint32 bits_to_be_set;              // Mask for the bits to be set
    uint32 bits_to_be_reset;                // mask for the bits to be reset
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();


    if (Gpio__Validate(port,0) == PASS)       // It protects against attempts to write out of the range
    {
        // The Set bit mask is just a logic AND between the value and the mask
        bits_to_be_set = ((uint32)(value & mask)) & LOWER_SHORT_MASK;

        // The REST bit mask is a logic AND between the inverted content of "value" and the mask shifted 16 bits to the left
        // So each bit 0 in the "value" which has a correspondent bit 1 in mask will be set in the final mask
        // but shifted 16bits to the left
        bits_to_be_reset = ((uint32)((~value) & mask)) & LOWER_SHORT_MASK;
        bits_to_be_reset = bits_to_be_reset << SHORT_SHIFT;

        bits_to_be_reset &= HIGHER_SHORT_MASK;
        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();               // Save interrupt state and disable that
        // Combines SET and RESET masks and apply it to the actual register BSRR
        ((volatile GPIO_DEF *)GPIO_PORT_LIST[port])->BSRR = (bits_to_be_set | bits_to_be_reset);        //lint !e929 Lint: 929 cast from pointer to pointer [MISRA 2004 Rule 11.4]
        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();            // Restore the interrupt context saved before
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method reads and entire port
 *    @details  Reads the IDR register
 *    @param    port: Defines the specific port
 *    @return   PORT_DATA_TYPE : The read value from the port requested
 */
PORT_DATA_TYPE Gpio__PortRead(GPIO_PORT_TYPE port)
{
    if (Gpio__Validate(port,0) == PASS)       // It protects against attempts to write out of the range
    {
        // Returns the related IDR register, correspondent to the requested port
        return (((volatile GPIO_DEF *)GPIO_PORT_LIST[port])->IDR);      //lint !e929 Lint: 929 cast from pointer to pointer [MISRA 2004 Rule 11.4]
    }
    else
    {
        return (0);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This API does validating the Gpio parameters
 * @param    port: Defines the specific port
 * @param    pin : Defines the specific pin
 * @return
 *      @retval - PASS : If the Gpio parameter check is valid
 *      @retval - FAIL : If the Gpio parameter check is invalid
 */
PASS_FAIL_TYPE Gpio__Validate(GPIO_PORT_TYPE port,uint8 pin)
{
    if ((port < GPIO_PORT_NUM) &&
        (pin < GPIO_MAX_PIN))
    {
        return (PASS);
    }
    else
    {
        return (FAIL);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief    It checks configuration and state of the Gpio pin

 * @param port = The port that has the pin.
 * @param pin = The pin number on the port.
 * @param type = The expected pin configuration.
 * @param state = The expected pin state.
 * @return TRUE if the configuration and output state matches what was expected; otherwise FALSE.
 */
BOOL_TYPE Gpio__CheckConfig(GPIO_PORT_TYPE port, uint8 pin, GPIO_PIN_MODE_TYPE type, BOOL_TYPE state)
{
    BOOL_TYPE response = FALSE;
    if (Gpio__Validate(port, pin) == PASS)
    {
        volatile GPIO_DEF * port_ptr;

        port_ptr = (volatile GPIO_DEF *)GPIO_PORT_LIST[port];       //lint !e929 Lint: 929 cast from pointer to pointer [MISRA 2004 Rule 11.4]
        response = TRUE;





        //---------------------------------------------------------------------------------------------------------------------
        /**
         * Check the configuration of the pin
         */
        if (type >= SPECIAL_FUNCTION_OPEN_DRAIN)
        {
            // Speed mode configuration and Output mode configuration
            if ((port_ptr->OSPEEDR != GPIO_OSPEEDER_50MHZ_SPEED_ALL) ||
                ((port_ptr->OTYPER & ((GPIO_OTYPER_OT_0) << ((uint16)pin))) != (uint16)(((uint16)PIN_CONFIG_OTYPER[type]) << ((uint16)pin))))
            {
                response = FALSE;

                //generate an exception if the ADC value read from the "Internal Power Block" is out of range
                SRException__Queue(SREXCEPTION_GPIO_CONFIGURATION_CORRUPTION, port, pin);
            }
        }

        // Mode and Pull-up Pull down register configuration

        if ( ((port_ptr->MODER  & (GPIO_MODER_MODER0 << (pin << 1))) != (((uint32)PIN_CONFIG_MODER[type]) << (pin << 1))) ||
             ((port_ptr->PUPDR & (GPIO_PUPDR_PUPDR0 << ((uint16)pin << 1)))) != (((uint32)PIN_CONFIG_PUPDR[type]) << (pin << 1)) )
        {
            response = FALSE;

            //generate an exception if the ADC value read from the "Internal Power Block" is out of range
            SRException__Queue(SREXCEPTION_GPIO_CONFIGURATION_CORRUPTION, port, pin);
        }




        //---------------------------------------------------------------------------------------------------------------------
        /**
         * tests the state of a particular pin when in output mode.
         */
        // The configuration of the port-bit matches
        if ((type == OUTPUT_PUSHPULL) ||
            (type == OUTPUT_OPEN_DRAIN))
        {
            if(((port_ptr->ODR & GPIO_SET_MASK[pin]) >> pin) != state)
            {
                response = FALSE;

                //generate an exception if the ADC value read from the "Internal Power Block" is out of range
                SRException__Queue(SREXCEPTION_GPIO_DATA_CORRUPTION, port, pin);
            }
        }

    }
    return (response);
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
/**
 *    @brief    Updates a register based on a mask
 *    @param    current
 *    @param    mask
 *    @param    value
 *    @return   uint8 updated value
 */
static uint32 UpdateMaskedRegister(uint32 current,uint32 mask, uint32 value)
{
    uint32 reg;
    uint32 aux;

    aux = mask & (~value);                      //It generates a sub-mask to clear bits
    reg = current & (~aux);
    aux = mask & value;                         //It generates a sub-mask to set bits
    reg = reg | aux;                            //It sets the bits supposed to be set
    return (reg);
}

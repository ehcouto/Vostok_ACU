/**
 *  @file       
 *
 *  @brief      This module handles the External Interrupts
 *
 *  @details    Detailed description of the file contents
 *
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
#include "ExtInterrupt.h"
#include "ExtInterrupt_prv.h"
#include "Micro.h"
#include "Gpio.h"


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#if ((EXTINTERRUPT_PIN0_1_FEATURE == ENABLED) || (EXTINTERRUPT_PIN2_3_FEATURE == ENABLED) || (EXTINTERRUPT_PIN4_15_FEATURE == ENABLED))

    #define ICR_CHNL_MASK    0XFF

    typedef struct
    {
        GPIO_PORT_TYPE port;
        uint8 pin;
    }EXTINTERRUPT_TYPE;

    typedef struct
    {
        IRQ_CHANNEL_TYPE ext_irq_chnl;
        uint8 premp_prior;
    }EXTINTERRUPT_PRIORITY_TYPE;


#endif
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module ExtInterrupt and its variables
 *
 */
void ExtInterrupt__Initialize(void)
{
    #if ((EXTINTERRUPT_PIN0_1_FEATURE == ENABLED) || (EXTINTERRUPT_PIN2_3_FEATURE == ENABLED) || (EXTINTERRUPT_PIN4_15_FEATURE == ENABLED))
        // Connect the Alternate function device to the bus
        RCC.APBENR2 |= RCC_APBENR2_SYSCFGEN;
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This API sets the Trigger method
 * @details Applies only for GPIO pins
 *
 * @param chnl
 * @param port
 * @param pin
 * @param remap
 * @param trig_mode
 * @return
 */
PASS_FAIL_TYPE ExtInterrupt__Config(EXTINT_CHNL_TYPE chnl, GPIO_PORT_TYPE port, uint8 pin, uint8 remap, EXTINT_TRG_TYPE trig_mode)
{
    #if ((EXTINTERRUPT_PIN0_1_FEATURE == ENABLED) || (EXTINTERRUPT_PIN2_3_FEATURE == ENABLED) || (EXTINTERRUPT_PIN4_15_FEATURE == ENABLED))
        PASS_FAIL_TYPE response = FALSE;

        if ((chnl < EXTINT_CH16) && (chnl == pin) && (port < GPIO_PORT_NUM))
        {
            uint32 ext_bit_pos;
            uint32 afio_port_index;

            uint8 icr_index = pin/4;
            uint8 icr_pos = (pin%4) * 8;

            //Configure the I/O as input mode
            Gpio__PinConfig(port, pin, INPUT_FLOAT);

            //Clear the external interrupt configuration
            EXTI.EXTICR[icr_index] &=(~((uint32)(ICR_CHNL_MASK << icr_pos)));

            ext_bit_pos = (uint32)(1 << chnl);

            //Clear the Interrupt
            EXTI.IMR1   &= ~(ext_bit_pos);
            EXTI.EMR1   &= ~(ext_bit_pos);

            //Clear the trigger mode
            EXTI.RTSR1   &= ~(ext_bit_pos);
            EXTI.FTSR1   &= ~(ext_bit_pos);

            if (trig_mode != EXTINT_TRG_DISABLE)
            {
                response = PASS;

                if (trig_mode == EXTINT_TRG_RISE)
                {
                    EXTI.RTSR1 |= ext_bit_pos;
                }
                else if (trig_mode == EXTINT_TRG_FALL)
                {
                    EXTI.FTSR1 |= ext_bit_pos;
                }
                else if (trig_mode == EXTINT_TRG_BOTH)
                {
                    EXTI.RTSR1 |= ext_bit_pos;
                    EXTI.FTSR1 |= ext_bit_pos;
                }

                //Configure the PIN used as EXTI line
                EXTI.EXTICR[icr_index] |=(uint32)(port << icr_pos);
                EXTI.IMR1 |= ext_bit_pos;

                //Interrupt priority
                IRQ_CHANNEL_TYPE ext_irq_chnl;
                uint8 premp_prior;

                if (chnl < 2)
                {
                    ext_irq_chnl = EXTI0_1_IRQ_CHANNEL;
                    premp_prior = EXTINTERRUPT_EXT0_1_PREEMPTION_PRIORITY;
                }
                else if (chnl < 4)
                {
                    ext_irq_chnl = EXTI2_3_IRQ_CHANNEL;
                    premp_prior = EXTINTERRUPT_EXT2_3_PREEMPTION_PRIORITY;
                }
                else
                {
                    ext_irq_chnl = EXTI4_15_IRQ_CHANNEL;
                    premp_prior = EXTINTERRUPT_EXT4_15_PREEMPTION_PRIORITY;
                }

                //Enable the External Interrupt
                Micro__NVICEnableIRQ(ext_irq_chnl, premp_prior);
            }
        }

        return(response);
    #else
        return FAIL;
    #endif
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

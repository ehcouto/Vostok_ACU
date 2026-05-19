/**
 *  @file       
 *
 *  @brief      All the pins of the STM32G0x micro controller are configured as External Interrupt
 *              All the similar PORT pin index's are connected to that indexed external interrupt
 *
 *              For Example - PORTA.PIN0, PORTB.PIN0, PORTC.PIN0, PORTD.PIN0, PORTE.PIN0...all are connected to EXTI0_IRQ_CHANNEL
 *                            PORTA.PIN1, PORTB.PIN1, PORTC.PIN1, PORTD.PIN1, PORTE.PIN1...all are connected to EXTI1_IRQ_CHANNEL
 *
 *              This drives the hardware limitation - All the External interrupt pins should have different pin number(No more than one same index pin is allowed)
 *
 *              Based on the External interrupt pin index use the ExtInterrupt__Ext() function for handling the ISR's for example -
 *              If the external interrupt is configured for PORTA, PIN0 and PORTC, PIN12 then the SRInterruptVector_prv.h file shall look like this -
 *              #define EXTI0_IRQHANDLER                ExtInterrupt__Ext   //This handles the EXT0 IRQ for PORTA.PIN0
 *              #define EXTI1_IRQHANDLER                SRMicro__Trap
 *              #define EXTI2_IRQHANDLER                SRMicro__Trap
 *              #define EXTI3_IRQHANDLER                SRMicro__Trap
 *              #define EXTI4_IRQHANDLER                SRMicro__Trap
 *              #define EXTI9_5_IRQHANDLER              SRMicro__Trap
 *              #define EXTI15_10_IRQHANDLER            ExtInterrupt__Ext   //This handles the EXT12 IRQ for PORTC.PIN12
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#ifndef EXTINTERRUPT_PRV_H_
#define EXTINTERRUPT_PRV_H_

//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================
//=====================================================================================================================
/**
 * @brief  - This macro defines the External interrupt priority.
 * @details- Do not set the priority higher then WIN
 *
 * @param  - 0/1/2/3
 *           0 is the highest priority
 *           3 is the lowest priority
 *
 * @note   - Following are the Examples\n
 *           "#define EXTINTERRUPT_EXT0_1_PREEMPTION_PRIORITY 0"  //The External Ext0 and Ext1 is in interrupt group 0
 *                                                                //having highest priority
 */
//=====================================================================================================================
#define EXTINTERRUPT_EXT0_1_PREEMPTION_PRIORITY         3
#define EXTINTERRUPT_EXT2_3_PREEMPTION_PRIORITY         3
#define EXTINTERRUPT_EXT4_15_PREEMPTION_PRIORITY        3


#endif // EXTINTERRUPT_PRV_H_

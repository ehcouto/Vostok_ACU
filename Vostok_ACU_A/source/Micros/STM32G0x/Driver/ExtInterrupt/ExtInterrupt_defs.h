/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: ExtInterrupt_defs.h 1.1 2015/06/24 08:12:13EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/06/24 08:12:13EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef EXTINTERRUPT_DEFS_H_
#define EXTINTERRUPT_DEFS_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
/**
 *  @brief      All the pins of the STM32F1x micro controller are configured as External Interrupt
 *              All the similar PORT pin indexs are connected to that indexed external interrupt
 *              For Example - PORTA.PIN0, PORTB.PIN0, PORTC.PIN0, PORTD.PIN0, PORTE.PIN0...all are connected to EXTI0_IRQ_CHANNEL
 *                            PORTA.PIN1, PORTB.PIN1, PORTC.PIN1, PORTD.PIN1, PORTE.PIN1...all are connected to EXTI1_IRQ_CHANNEL
 *
 *              This drives the hardware limitation - All the External interrupt pins should have different pin number(No more than one same index pin is allowed)
 *
 *              Based on the External interrupt pin index use the ExtInterrupt__Ext() function for handling the ISR's for example -
 *              If the external interrupt is configured for PORTA, PIN0 and PORTC, PIN12 then the SRInterruptVector_prv.h file shall look like this -
 *              #define EXTI0_IRQHANDLER                ExtInterrupt__Ext    This handles the EXT0 IRQ for PORTA.PIN0
 *              #define EXTI1_IRQHANDLER                SRMicro__Trap
 *              #define EXTI2_IRQHANDLER                SRMicro__Trap
 *              #define EXTI3_IRQHANDLER                SRMicro__Trap
 *              #define EXTI4_IRQHANDLER                SRMicro__Trap
 *              #define EXTI9_5_IRQHANDLER              SRMicro__Trap
 *              #define EXTI15_10_IRQHANDLER            ExtInterrupt__Ext    This handles the EXT12 IRQ for PORTC.PIN12
*/

#endif // EXTINTERRUPT_DEFS_H_



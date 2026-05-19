/**
 *  @file
 *  @defgroup   CLASS_B
 *
 *
 *  @brief      STM32F0x Macros to fast access to the port bits
 *
 *  @details    NOTE ABOUT USE THE MACROS !!!
 * \            READ THIS
 * \
 * \    The macros allow the application to access the registers related to the Gpio.
 * \    This kind of access implies in operations like read, manipulate and write it back.
 * \    It potentially is exposed to issues like conflict when called from Main and Interrupts since it's not an atomic instruction.
 * \    It's fine to call it from processes handled from the timer interruption
 * \
 * \    It's NOT recommended use the macros for process from the Main code (background processes), use Gpio functions instead.
 * \    To use the Macros from the background processes it recommend to do as atomic instructions, for that the code should follow the following example:
 * \
 * \    Micro__SaveInterruptEnableFlag();
 * \    code....
 * \    Micro__RestoreInterruptEnableFlag();
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
#ifndef GPIO_MACROS_H_
	#define GPIO_MACROS_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
extern const uint16 GPIO_SET_MASK[];
extern const uint32 GPIO_CLR_MASK[];
extern const volatile GPIO_DEF * GPIO_PORT_LIST[];

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
#define GPIO__PIN_SET(PORT, PORTBIT)                                {   ((*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).BSRR) = (1<<PORTBIT);}
#define GPIO__PIN_CLR(PORT, PORTBIT)                                {   ((*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).BRR) = (1<<PORTBIT);}
#define GPIO__PIN_TOGGLE(PORT, PORTBIT)                             {   ((*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).ODR) ^=GPIO_SET_MASK[PORTBIT];}
#define GPIO__PIN_WRITE(PORT, PORTBIT,VALUE)                        {   if (VALUE == 0) {GPIO__PIN_CLR(PORT,PORTBIT);} else {GPIO__PIN_SET(PORT,PORTBIT);}}

#define GPIO__PORT_SET(PORT, MASK)                                  {   ((*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).BSRR) = (MASK);}
#define GPIO__PORT_CLR(PORT, MASK)                                  {   ((*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).BRR) = (MASK);}

#define GPIO__PIN_READ(PORT, PORTBIT)                               (   ((*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).IDR & GPIO_SET_MASK[PORTBIT]) >> PORTBIT)
#define GPIO__PORT_READ(PORT)                                       (   (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).IDR)

#define GPIO__PIN_COFIG_I_FLOAT(PORT, PORTBIT)                      {   (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).MODER &= GPIO_CLR_MASK[PORTBIT];               \
                                                                        (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).PUPDR &= GPIO_CLR_MASK[PORTBIT];}

#define GPIO__PIN_COFIG_I_PULLUP(PORT, PORTBIT)                     {   (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).MODER &= GPIO_CLR_MASK[PORTBIT];               \
                                                                        (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).PUPDR &= GPIO_CLR_MASK[PORTBIT];               \
                                                                        (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).PUPDR |= 1 << (PORTBIT*2);}

#define GPIO__PIN_COFIG_I_PULLDOWN(PORT, PORTBIT)                   {   (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).MODER &= GPIO_CLR_MASK[PORTBIT];               \
                                                                        (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).PUPDR &= GPIO_CLR_MASK[PORTBIT];               \
                                                                        (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).PUPDR |= 2 << (PORTBIT*2);}

#define GPIO__PIN_COFIG_I_ANALOG(PORT, PORTBIT)                     {   (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).MODER = (((*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).MODER  & GPIO_CLR_MASK[PORTBIT]) | (((uint32)3) << (PORTBIT << 1)));        \
                                                                        (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).PUPDR &= GPIO_CLR_MASK[PORTBIT];}

#define GPIO__PIN_COFIG_O_PUSHPULL(PORT, PORTBIT)                   {   (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).OSPEEDR = GPIO_OSPEEDER_50MHZ_SPEED_ALL; (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).OTYPER &= ~((GPIO_OTYPER_OT_0) << ((uint16)PORTBIT));        \
                                                                        (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).OTYPER &= ~((GPIO_OTYPER_OT_0) << ((uint16)PORTBIT));                                                                              \
                                                                        (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).MODER = (((*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).MODER  & GPIO_CLR_MASK[PORTBIT]) | (((uint32)1) << (PORTBIT << 1)));                        \
                                                                        (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).PUPDR &= GPIO_CLR_MASK[PORTBIT];}

#define GPIO__PIN_COFIG_O_OPENDRAIN(PORT, PORTBIT)                  {   (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).OSPEEDR = GPIO_OSPEEDER_50MHZ_SPEED_ALL;       \
                                                                        (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).OTYPER = (((*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).OTYPER & (~((GPIO_OTYPER_OT_0) << ((uint16)PORTBIT)))) | GPIO_SET_MASK[PORTBIT]);          \
                                                                        (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).MODER = (((*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).MODER  & GPIO_CLR_MASK[PORTBIT]) | (((uint32)1) << (PORTBIT << 1)));                        \
                                                                        (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).PUPDR &= GPIO_CLR_MASK[PORTBIT];}

#define GPIO__PIN_COFIG_SF_OPENDRAIN(PORT, PORTBIT)                 {   (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).OSPEEDR = GPIO_OSPEEDER_50MHZ_SPEED_ALL;       \
                                                                        (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).OTYPER = (((*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).OTYPER & (~((GPIO_OTYPER_OT_0) << ((uint16)PORTBIT)))) | GPIO_SET_MASK[PORTBIT]);          \
                                                                        (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).MODER = (((*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).MODER  & GPIO_CLR_MASK[PORTBIT]) | (((uint32)2) << (PORTBIT << 1)));                        \
                                                                        (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).PUPDR &= (uint32)(~(3 << (PORTBIT*2)));                                                                                                            \
                                                                        (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).PUPDR |= 1 << (PORTBIT*2);}

#define GPIO__PIN_COFIG_SF_PUSHPULL(PORT, PORTBIT)                  {   (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).OSPEEDR = GPIO_OSPEEDER_50MHZ_SPEED_ALL;                                                                                           \
                                                                        (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).OTYPER &= ~((GPIO_OTYPER_OT_0) << ((uint16)PORTBIT));                                                                              \
                                                                        (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).MODER = (((*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).MODER  & GPIO_CLR_MASK[PORTBIT]) | (((uint32)2) << (PORTBIT << 1)));        \
                                                                        (*(volatile GPIO_DEF *)GPIO_PORT_LIST[PORT]).PUPDR &= GPIO_CLR_MASK[PORTBIT];}

#endif // GPIO_MACROS_H_

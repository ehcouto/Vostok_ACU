/**
 *  @file
 *  @defgroup   CLASS_B
 *
 *  @brief      Specific Micro-controller definition
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#ifndef GPIO_DEF_H_
	#define GPIO_DEF_H_
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef enum
{
    PORTA,
    PORTB,
    PORTC,
    PORTD,
    PORTE,
    PORTF,
    GPIO_PORT_NUM,
    PORT_NULL = 255
}GPIO_PORT_TYPE;


#define PORT_DATA_TYPE  uint16              //! This define is mandatory, it defines the register's type


#define GPIO_MAX_PIN    16
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif // GPIO_DEF_H_



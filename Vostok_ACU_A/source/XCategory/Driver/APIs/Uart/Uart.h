/**
 *  @file
 *
 *  @brief      Standard Uart API
 *
 *   @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 * $Header: $
 *
 *  @copyright  Copyright 2017. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 *
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

#ifndef UART_H
	#define UART_H
#include "C_Extensions.h"
#include "uc.h"
#include "Uart_Defs.h"
#include "Uart_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef enum
{
	COM0,
	COM1,
	COM2,
	COM3,
	COM4,
	COM5,
	COM6,
	COM7,
	COM8,
	COM9,
	COM10,
	COM11,
	COM12,
	COM13,
	COM14

} UART_ENUM_DEF;

typedef enum
{
	B1200,
	B2400,
	B4800,
	B9600,
	B19200,
	B38400,
	B57600,
	B115200,
	B230400,
} UART_SPEED_DEF;

typedef enum
{
	UART_STOP_BIT_1,
	UART_STOP_BIT_2
} UART_STOP_DEF;

typedef enum
{
	UART_PARITY_NONE,
	UART_PARITY_EVEN,
	UART_PARITY_ODD
} UART_PARITY_DEF;


typedef enum
{
	UART_STATUS_BREAK_IN,
	UART_STATUS_BREAK_OUT,
	UART_STATUS_TX_EMPTY,
	UART_STATUS_TX_AVAILABLE,
	UART_STATUS_RX_AVAILABLE,
	UART_STATUS_ERROR,
	UART_STATUS_RX_PIN
}UART_STATUS_DEF;


typedef enum
{
	UART_OPTION_ALL,
	UART_OPTION_TX_EN,
	UART_OPTION_RX_EN
}UART_ENABLE_OPTIONS_DEF;


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
BOOL_TYPE Uart__Initialize(UART_ENUM_DEF uart, UART_SPEED_DEF speed, UART_STOP_DEF stops, UART_PARITY_DEF parity );
void Uart__Clear(UART_ENUM_DEF uart);
void Uart__ClearTx(UART_ENUM_DEF uart);
void Uart__Enable(UART_ENUM_DEF uart,UART_ENABLE_OPTIONS_DEF option);
void Uart__Disable(UART_ENUM_DEF uart,UART_ENABLE_OPTIONS_DEF option);
BOOL_TYPE Uart__Write(UART_ENUM_DEF uart, uint8 * buf, uint8 size);
BOOL_TYPE Uart__Read(UART_ENUM_DEF uart, uint8 * buf, uint8 size);
BOOL_TYPE Uart__WriteByte(UART_ENUM_DEF uart, uint8 data);
uint8 Uart__ReadByte(UART_ENUM_DEF uart);
void Uart__SendBreak(UART_ENUM_DEF uart);
void Uart__StopBreak(UART_ENUM_DEF uart);
uint8 Uart__GetStatus(UART_ENUM_DEF uart, UART_STATUS_DEF index);
void Uart__SetRemapping(UART_ENUM_DEF uart, uint8 remaping);
void Uart__Handler(void);



#endif

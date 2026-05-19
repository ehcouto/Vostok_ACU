/**
 *  @file
 *
 *    @brief    The macro value in this file are configurable as per the user requirement.
 *    @details  It contains data that can be modified by the user in order to configure the Uart TX, RX buffer size,
 *              Enable/Disable the Uart Com Port.
 *
 *              COM1(UART1),
 *              COM2(UART2),
 *              COM3(UART3),
 *              COM4(UART4),
 *              COM5(UART5),
 *              COM6(UART6)
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
#ifndef UART_PRM_H_
	#define UART_PRM_H_
#include "C_Extensions.h"
#include "uc.h"
#include "Mci_prm.h"

//=====================================================================================================================
//-------------------------------------- COM(UART) --------------------------------------------------------------------
//=====================================================================================================================
//=====================================================================================================================
/**
 * @brief  - This macro Enables or Disables the COMs
 * @details- The COM port must be Enabled before using in the application.
 *
 * @param  - ENABLED(1) - Which Enables the COM
 *           DISABLED(0) – Which Disables the COM
 *
 * @note   - Following are the Examples\n
 *           "#define UART1_FEATURE       ENABLED"   This Enables the COM1 Feature\n
 *           "#define UART1_FEATURE       DISABLED"  This Disables the COM1 Feature
 */
//=====================================================================================================================
#define UART1_FEATURE       DISABLED
#define UART2_FEATURE       ENABLED
#define UART3_FEATURE       DISABLED
#define UART4_FEATURE       DISABLED
#define UART5_FEATURE       DISABLED
#if (MCL_DEBUG_MODE == ENABLED)
#define UART6_FEATURE       ENABLED
#else
#define UART6_FEATURE       DISABLED
#endif

//=====================================================================================================================
/**
 * @brief  - This macros configures the port of
 *          COM1(UART1),
 *          COM2(UART2),
 *          COM3(UART3),
 *          COM4(UART4),
 *          COM5(UART5),
 *          COM6(UART6)
 *
 * @details-
 *
 * <pre>
 *  The UART_1 is available in the below port-pins of the STM32F0x Controllers
 *  ---------------------------------------------------------------------
 * |            UART_1_TX_            |            UART_1_RX_            |
 * |----------------------------------|----------------------------------|
 * |   PORT   |  PORTBIT |     AF     |   PORT   |  PORTBIT |     AF     |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTA  |    9     | GPIO_AF_1  |   PORTA  |    10    | GPIO_AF_1  |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTB  |    6     | GPIO_AF_0  |   PORTB  |    7     | GPIO_AF_0  |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTC  |    4     | GPIO_AF_1  |   PORTC  |    5     | GPIO_AF_1  |
 *  ---------------------------------------------------------------------
 *
 *
 *  The UART_2 is available in the below port-pins of the STM32F0x Controllers
 *  ---------------------------------------------------------------------
 * |            UART_2_TX_            |            UART_2_RX_            |
 * |----------------------------------|----------------------------------|
 * |   PORT   |  PORTBIT |     AF     |   PORT   |  PORTBIT |     AF     |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTA  |    2     | GPIO_AF_1  |   PORTA  |    3     | GPIO_AF_1  |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTA  |    14    | GPIO_AF_1  |   PORTA  |    15    | GPIO_AF_1  |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTD  |    5     | GPIO_AF_0  |   PORTD  |    6     | GPIO_AF_0  |
 *  ---------------------------------------------------------------------
 *
 *
 *  The UART_3 is available in the below port-pins of the STM32F0x Controllers
 *  ---------------------------------------------------------------------
 * |            UART_3_TX_            |            UART_3_RX_            |
 * |----------------------------------|----------------------------------|
 * |   PORTB  |    8     | GPIO_AF_4  |   PORTB  |    9     | GPIO_AF_4  |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTA  |    5     | GPIO_AF_4  |   PORTB  |    0     | GPIO_AF_4  |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTB  |    10    | GPIO_AF_4  |   PORTB  |    11    | GPIO_AF_4  |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTC  |    4     | GPIO_AF_0  |   PORTC  |    5     | GPIO_AF_0  |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTC  |    10    | GPIO_AF_0  |   PORTC  |    11    | GPIO_AF_0  |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTD  |    8     | GPIO_AF_0  |   PORTD  |    9     | GPIO_AF_0  |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTB  |    2     | GPIO_AF_4  |                                  |
 *  ---------------------------------------------------------------------
 *
 *
 *  The UART_4 is available in the below port-pins of the STM32F0x Controllers
 *  ---------------------------------------------------------------------
 * |            UART_4_TX_            |            UART_4_RX_            |
 * |----------------------------------|----------------------------------|
 * |   PORT   |  PORTBIT |     AF     |   PORT   |  PORTBIT |     AF     |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTA  |    0     | GPIO_AF_4  |   PORTA  |    1     | GPIO_AF_4  |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTC  |    10    | GPIO_AF_1  |   PORTC  |    11    | GPIO_AF_1  |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTE  |    8     | GPIO_AF_0  |   PORTE  |    9     | GPIO_AF_0  |
 *  ---------------------------------------------------------------------
 *
 *
 *  The UART_5 is available in the below port-pins of the STM32F0x Controllers
 *  ---------------------------------------------------------------------
 * |            UART_5_TX_            |            UART_5_RX_            |
 * |----------------------------------|----------------------------------|
 * |   PORT   |  PORTBIT |     AF     |   PORT   |  PORTBIT |     AF     |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTB  |    3     | GPIO_AF_3  |   PORTB  |    4     | GPIO_AF_3  |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTB  |    0     | GPIO_AF_8  |   PORTB  |    1     | GPIO_AF_8  |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTC  |    12    | GPIO_AF_3  |   PORTD  |    2     | GPIO_AF_3  |
 * |---------------------------------------------------------------------
 * |   PORTE  |    10    | GPIO_AF_3  |   PORTE  |    11    | GPIO_AF_3  |
 * |---------------------------------------------------------------------
 * |   PORTD  |    3     | GPIO_AF_3  |                                  |
 * |---------------------------------------------------------------------
 *
 *
 *  The UART_6 is available in the below port-pins of the STM32F0x Controllers
 *  ---------------------------------------------------------------------
 * |            UART_6_TX_            |            UART_6_RX_            |
 * |----------------------------------|----------------------------------|
 * |   PORT   |  PORTBIT |     AF     |   PORT   |  PORTBIT |     AF     |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTA  |    4     | GPIO_AF_3  |   PORTA  |    5     | GPIO_AF_3  |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTB  |    8     | GPIO_AF_8  |   PORTB  |    9     | GPIO_AF_8  |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTC  |    0     | GPIO_AF_4  |   PORTC  |    1     | GPIO_AF_4  |
 *  ---------------------------------------------------------------------
 *
 *  </pre>
 *
 * @param  -
 *
 * @note   - Following are the Examples\n
 *           " UART1 Port-bit configuration"
 *           "#define UART_1_TX_PORT          PORTA"
 *           "#define UART_1_TX_PORTBIT       9"
 *           "#define UART_1_TX_AF            GPIO_AF_1"
 *
 *           "#define UART_1_RX_PORT          PORTA"
 *           "#define UART_1_RX_PORTBIT       10"
 *           "#define UART_1_RX_AF            GPIO_AF_1"
 */
//=====================================================================================================================
//! UART1 Port-bit configuration
#define UART_1_TX_PORT          PORTC
#define UART_1_TX_PORTBIT       4
#define UART_1_TX_AF            GPIO_AF_1

#define UART_1_RX_PORT          PORTC
#define UART_1_RX_PORTBIT       5
#define UART_1_RX_AF            GPIO_AF_1

//! UART2 Port-bit configuration
#define UART_2_TX_PORT          PORTD
#define UART_2_TX_PORTBIT       5
#define UART_2_TX_AF            GPIO_AF_0

#define UART_2_RX_PORT          PORTD
#define UART_2_RX_PORTBIT       6
#define UART_2_RX_AF            GPIO_AF_0

//! UART3 Port-bit configuration
#define UART_3_TX_PORT          PORTB
#define UART_3_TX_PORTBIT       8
#define UART_3_TX_AF            GPIO_AF_4

#define UART_3_RX_PORT          PORTB
#define UART_3_RX_PORTBIT       9
#define UART_3_RX_AF            GPIO_AF_4

//! UART4 Port-bit configuration
#define UART_4_TX_PORT          PORTA
#define UART_4_TX_PORTBIT       0
#define UART_4_TX_AF            GPIO_AF_4

#define UART_4_RX_PORT          PORTA
#define UART_4_RX_PORTBIT       1
#define UART_4_RX_AF            GPIO_AF_4

//! UART5 Port-bit configuration
#define UART_5_TX_PORT          PORTB
#define UART_5_TX_PORTBIT       3
#define UART_5_TX_AF            GPIO_AF_3

#define UART_5_RX_PORT          PORTB
#define UART_5_RX_PORTBIT       4
#define UART_5_RX_AF            GPIO_AF_3

//! UART6 Port-bit configuration
#define UART_6_TX_PORT          PORTC
#define UART_6_TX_PORTBIT       0
#define UART_6_TX_AF            GPIO_AF_4

#define UART_6_RX_PORT          PORTC
#define UART_6_RX_PORTBIT       1
#define UART_6_RX_AF            GPIO_AF_4

//=====================================================================================================================
/**
 *    @brief  - This macro defines the COM1(UART1), COM2(UART2),  COM3(UART3) and COM4(UART4) Tx FIFO Buffer size.
 *    @details- The Tx FIFO buffer size should be non-zero value.\n
 *              For the Reveal Protocol application it’s recommended that the buffer size should be 22.
 *
 *    @param  - <1\n
 *              <22 for Reveal Application
 *
 *    @note   - Following are the Examples\n
 *              "#define UART2_TX_BUF_SIZE           10"  The Uart2 Tx buffer size is 10\n
 *              "#define UART2_TX_BUF_SIZE           22"  The Uart2 Tx buffer size is 22
 */
//=====================================================================================================================
#define UART1_TX_BUF_SIZE           22
#define UART2_TX_BUF_SIZE           22
#define UART3_TX_BUF_SIZE           22
#define UART4_TX_BUF_SIZE           22
#define UART5_TX_BUF_SIZE           22
#define UART6_TX_BUF_SIZE           22

//=====================================================================================================================
/**
 *    @brief  - This macro defines the COM1(UART1), COM2(UART2),  COM3(UART3) and COM4(UART4) Rx FIFO Buffer size.
 *    @details- The Rx FIFO buffer size should be non-zero value.\n
 *              For the Reveal Protocol application it’s recommended that the buffer size should be 22.
 *
 *    @param  - <1\n
 *              <22 for Reveal Application
 *
 *    @note   - Following are the Examples\n
 *              "#define UART2_RX_BUF_SIZE           10"  The Uart2 Rx buffer size is 10\n
 *              "#define UART2_RX_BUF_SIZE           22"  The Uart2 Rx buffer size is 22
 */
//=====================================================================================================================
#define UART1_RX_BUF_SIZE           22
#define UART2_RX_BUF_SIZE           22
#define UART3_RX_BUF_SIZE           22
#define UART4_RX_BUF_SIZE           22
#define UART5_RX_BUF_SIZE           22
#define UART6_RX_BUF_SIZE           22

//=====================================================================================================================
/**
 * @brief  - Uart interrupt priority is defined as highest to work with embOS
 * @details By purpose they are not defined here to enforce the highest priority.
 *          This is defined in the .c file and can be over ride by defining below
 *
 *              #define COM1_PREEMPTION_PRIORITY                0
 *              #define COM2_PREEMPTION_PRIORITY                0
 *              #define COM3_PREEMPTION_PRIORITY                0
 *              #define COM4_PREEMPTION_PRIORITY                0
 *              #define COM5_PREEMPTION_PRIORITY                0
 *              #define COM6_PREEMPTION_PRIORITY                0
 *
 * @param  - 0/1/2/3
 *
 * @note   - Following are the Examples\n
 *           "#define COM1_PREEMPTION_PRIORITY 0"   The UART1 is in interrupt  0 having highest priority
 *           "#define COM2_PREEMPTION_PRIORITY 1"   The UART2 is in interrupt 1 having priority less than the UART1
 */
//=====================================================================================================================


#endif // UART_PRM_H_



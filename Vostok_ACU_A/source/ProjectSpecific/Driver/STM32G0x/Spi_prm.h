/**
 *  @file
 *
 *  @brief    The macro value in this file are configurable as per the user requirement.
 *
 *  @details  It contains data that can be modified by the user in order to configure the Spi Tx, Rx buffer size,
 *              Spi Clock polarity, Baud rate, interrupt priority etc
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#ifndef SPI_PRM_H_
    #define SPI_PRM_H_
#include "C_Extensions.h"

//=====================================================================================================================
//-------------------------------------- SPI1, SPI2, SPI3 -------------------------------------------------------------
//=====================================================================================================================
//=====================================================================================================================
/**
 *    @brief  - This macro Enables or Disables the Spi modules.
 *    @details- The macro must be Enabled before using in the application.
 *
 *    @param  - ENABLED(1)  - Which Enables the Specified Spi
 *              DISABLED(0) – Which Disables the Specified Spi
 *
 *    @note   - Following are the Examples\n
 *              "#define SPI1_FEATURE        ENABLED"   This Enables the Spi1 Feature\n
 *              "#define SPI1_FEATURE        DISABLED"  This Disables the Spi1 Feature\n
 *              "#define SPI2_FEATURE        ENABLED"   This Enables the Spi2 Feature\n
 *              "#define SPI2_FEATURE        DISABLED"  This Disables the Spi2 Feature
 */
//=====================================================================================================================
#define SPI_1_FEATURE        DISABLED
#define SPI_2_FEATURE        DISABLED
#define SPI_3_FEATURE        DISABLED

//=====================================================================================================================
/**
 *    @brief  - This macro defines the SPI mode either Full duplex (Tx and Rx are ON) or Half duplex (Tx only ON).
 *    @details- If the SPIx_TX_ONLY is Enabled then MISO pin could be free to use as Gpio pin for other application.
 *    @param  - DISABLED(0) –  Full duplex (Tx and Rx are ON)
 *              ENABLED(1) -   Half duplex (Tx only ON)
 *
 *    @note   - Following are the Examples\n
 *              "#define SPI1_TX_ONLY        DISABLED"  Both Transmitter and Receiver are ON\n
 *              "#define SPI1_TX_ONLY        ENABLED"   Only receiver ON
 */
//=====================================================================================================================
#define SPI_1_TX_ONLY        DISABLED
#define SPI_2_TX_ONLY        DISABLED
#define SPI_3_TX_ONLY        DISABLED

//=====================================================================================================================
/**
 *    @brief  - This macro defines the Tx and Rx FIFO Buffer size.
 *    @details- Tx and Rx FIFO buffer size should be big enough so that the application can insert the data into it.
 *              Rx buffer is not allocated if the SPIx_TX_ONLY mode is Enabled.
 *
 *    @param  - n    Where n=1,2,3…
 *
 *    @note   - Following are the Examples\n
 *              "#define SPI_BUFFER_SIZE         5"   The Spi Tx and Rx buffer size is 5\n
 *              "#define SPI_BUFFER_SIZE         10"  The Spi Tx and Rx buffer size is 10
 */
//=====================================================================================================================
#define SPI_BUFFER_SIZE     10

//=====================================================================================================================
/**
 *    @brief  - This macro remaps the Spi1 pins.
 *    @details- The SPI1 is available in the below port pins as per the Remap configuration value
 *
 * <pre>
 *  --------------------------------------------------------------------------------------------------------
 * |            SPI_1_MOSI_           |            SPI_1_MIS0_           |            SPI_1_SCK_            |
 * |----------------------------------|----------------------------------|----------------------------------|
 * |   PORT   |  PORTBIT |     AF     |   PORT   |  PORTBIT |     AF     |   PORT   |  PORTBIT |     AF     |
 * |----------|----------|------------|----------|----------|------------|----------|----------|------------|
 * |   PORTA  |    2     | GPIO_AF_0  |   PORTA  |    6     | GPIO_AF_0  |   PORTA  |    1     | GPIO_AF_0  |
 * |----------|----------|------------|----------|----------|------------|----------|----------|------------|
 * |   PORTA  |    7     | GPIO_AF_0  |   PORTA  |    11    | GPIO_AF_0  |   PORTA  |    5     | GPIO_AF_0  |
 * |----------|----------|------------|----------|----------|------------|----------|----------|------------|
 * |   PORTA  |    12    | GPIO_AF_0  |   PORTB  |    4     | GPIO_AF_0  |   PORTB  |    3     | GPIO_AF_0  |
 * |----------|----------|------------|----------|----------|------------|----------|----------|------------|
 * |   PORTB  |    5     | GPIO_AF_0  |   PORTD  |    5     | GPIO_AF_1  |   PORTE  |    13    | GPIO_AF_0  |
 * |----------|----------|------------|----------|----------|------------|----------|----------|------------|
 * |   PORTD  |    6     | GPIO_AF_1  |          |          |            |   PORTD  |    8     | GPIO_AF_1  |
 *  --------------------------------------------------------------------------------------------------------
 *
 *  --------------------------------------------------------------------------------------------------------
 * |            SPI_2_MOSI_           |            SPI_2_MIS0_           |            SPI_2_SCK_            |
 * |----------------------------------|----------------------------------|----------------------------------|
 * |   PORT   |  PORTBIT |     AF     |   PORT   |  PORTBIT |     AF     |   PORT   |  PORTBIT |     AF     |
 * |----------|----------|------------|----------|----------|------------|----------|----------|------------|
 * |   PORTA  |    10    | GPIO_AF_0  |   PORTA  |    3     | GPIO_AF_0  |   PORTA  |    0     | GPIO_AF_0  |
 * |----------|----------|------------|----------|----------|------------|----------|----------|------------|
 * |   PORTB  |    11    | GPIO_AF_0  |   PORTB  |    14    | GPIO_AF_0  |   PORTB  |    13    | GPIO_AF_0  |
 * |----------|----------|------------|----------|----------|------------|----------|----------|------------|
 * |   PORTB  |    15    | GPIO_AF_0  |   PORTB  |    2     | GPIO_AF_1  |   PORTB  |    8     | GPIO_AF_1  |
 * |----------|----------|------------|----------|----------|------------|----------|----------|------------|
 * |   PORTD  |    4     | GPIO_AF_0  |   PORTC  |    2     | GPIO_AF_1  |   PORTD  |    1     | GPIO_AF_1  |
 * |----------|----------|------------|----------|----------|------------|----------|----------|------------|
 * |   PORTA  |    4     | GPIO_AF_1  |   PORTD  |    3     | GPIO_AF_1  |   PORTB  |    10    | GPIO_AF_5  |
 * |----------|----------|------------|----------|----------|------------|----------|----------|------------|
 * |   PORTB  |    7     | GPIO_AF_1  |   PORTA  |    9     | GPIO_AF_4  |          |          |            |
 * |----------|----------|------------|----------|----------|------------|----------|----------|------------|
 * |   PORTC  |    3     | GPIO_AF_1  |   PORTB  |    6     | GPIO_AF_4  |          |          |            |
 *  --------------------------------------------------------------------------------------------------------
 *
 *
 * AF0 -
 * AF1 -
 * AF4 PC12                             PC11                    PC10
 * AF5
 * AF9 PB5,                             PB4,                    PB3
 *  --------------------------------------------------------------------------------------------------------
 * |            SPI_3_MOSI_           |            SPI_3_MIS0_           |            SPI_3_SCK_            |
 * |----------------------------------|----------------------------------|----------------------------------|
 * |   PORT   |  PORTBIT |     AF     |   PORT   |  PORTBIT |     AF     |   PORT   |  PORTBIT |     AF     |
 * |----------|----------|------------|----------|----------|------------|----------|----------|------------|
 * |   PORTC  |    12    | GPIO_AF_4  |   PORTC  |    11    | GPIO_AF_4  |   PORTC  |    10    | GPIO_AF_4  |
 * |----------|----------|------------|----------|----------|------------|----------|----------|------------|
 * |   PORTB  |    5     | GPIO_AF_9  |   PORTB  |    4     | GPIO_AF_9  |   PORTB  |    3     | GPIO_AF_9  |
 *  --------------------------------------------------------------------------------------------------------
 * </pre>
 *
 *
 *    @param  -
 *
 *    @note   - 1. Full Duplex Communication (SPI_x_TX_ONLY is DISABLED) example for SPI_1 were MOSI-->PORTA, 7; MISO-->PORTA, 6; SCK-->PORTA, 5\n
 *              "#define SPI_1_MOSI_PORT         PORTA"
 *              "#define SPI_1_MOSI_PORTBIT      7"
 *              "#define SPI_1_MOSI_AF           GPIO_AF_0"
 *
 *              "#define SPI_1_MISO_PORT         PORTA"
 *              "#define SPI_1_MISO_PORTBIT      6"
 *              "#define SPI_1_MISO_AF           GPIO_AF_0"
 *
 *              "#define SPI_1_SCK_PORT          PORTA"
 *              "#define SPI_1_SCK_PORTBIT       5"
 *              "#define SPI_1_SCK_AF            GPIO_AF_0"
 *
 *              2. Transmit only Communication (SPI_x_TX_ONLY is ENABLED) example for SPI_1 were MOSI-->PORTA, 7; MISO-->NULL, 0; SCK-->PORTA, 5\n
 *              "#define SPI_1_MOSI_PORT         PORTA"
 *              "#define SPI_1_MOSI_PORTBIT      7"
 *              "#define SPI_1_MOSI_AF           GPIO_AF_0"
 *
 *              "#define SPI_1_MISO_PORT         NULL"
 *              "#define SPI_1_MISO_PORTBIT      0"
 *              "#define SPI_1_MISO_AF           GPIO_AF_0"
 *
 *              "#define SPI_1_SCK_PORT          PORTA"
 *              "#define SPI_1_SCK_PORTBIT       5"
 *              "#define SPI_1_SCK_AF            GPIO_AF_0"
 *
 */
//=====================================================================================================================
//! SPI_1 port configuration
#define SPI_1_MOSI_PORT         PORTA
#define SPI_1_MOSI_PORTBIT      2
#define SPI_1_MISO_AF           GPIO_AF_0

#define SPI_1_MISO_PORT         PORTA
#define SPI_1_MISO_PORTBIT      6
#define SPI_1_MOSI_AF           GPIO_AF_0

#define SPI_1_SCK_PORT          PORTB
#define SPI_1_SCK_PORTBIT       3
#define SPI_1_SCK_AF            GPIO_AF_0

//! SPI_2 port configuration
#define SPI_2_MOSI_PORT         PORTC
#define SPI_2_MOSI_PORTBIT      3
#define SPI_2_MISO_AF           GPIO_AF_1

#define SPI_2_MISO_PORT         PORTC
#define SPI_2_MISO_PORTBIT      2
#define SPI_2_MOSI_AF           GPIO_AF_1

#define SPI_2_SCK_PORT          PORTB
#define SPI_2_SCK_PORTBIT       13
#define SPI_2_SCK_AF            GPIO_AF_0

//! SPI_3 port configuration
#define SPI_3_MOSI_PORT         PORTC
#define SPI_3_MOSI_PORTBIT      12
#define SPI_3_MISO_AF           GPIO_AF_4

#define SPI_3_MISO_PORT         PORTC
#define SPI_3_MISO_PORTBIT      11
#define SPI_3_MOSI_AF           GPIO_AF_4

#define SPI_3_SCK_PORT          PORTC
#define SPI_3_SCK_PORTBIT       10
#define SPI_3_SCK_AF            GPIO_AF_4

//=====================================================================================================================
/**
 *    @brief  - This macro defines Spi bit rate.
 *    @details-
 *<pre>
 *      Below table explains about the different speed mode with respect to Baud rate-
 *      SYSTEM CLOCK is 64MHz
 *      ---------------------------
 *      |  Mode   |  Baud rate     |
 *      |---------|----------------|
 *      |    0    |   64MHz / 2    |
 *      |---------|----------------|
 *      |    1    |   64MHz / 4    |
 *      |---------|----------------|
 *      |    2    |   64MHz / 8    |
 *      |---------|----------------|
 *      |    3    |   64MHz / 16   |
 *      |---------|----------------|
 *      |    4    |   64MHz / 32   |
 *      |---------|----------------|
 *      |    5    |   64MHz / 64   |
 *      |---------|----------------|
 *      |    6    |   64MHz / 128  |
 *      |---------|----------------|
 *      |    7    |   64MHz / 256  |
 *       -------------------------- </pre>
 *
 *    @param  - 0/1/2/3/4/5/6/7
 *
 *    @note   - Following are the Examples\n
 *              "#define SPI_1_PRESCALER        0"  The Spi1 Baud rate is 64MHz/2     \n
 *              "#define SPI_2_PRESCALER        2"  The Spi2 Baud rate is 64MHz/8
 */
//=====================================================================================================================
#define SPI_1_PRESCALER         7
#define SPI_2_PRESCALER         7
#define SPI_3_PRESCALER         7

//=====================================================================================================================
/**
 *    @brief  - This macro's are used to change the baud-rate of the Spi by using Spi__SetSpeed() API during run-time
 *
 *    @param  - SPI_x_PRESCALER_SPEED1
 *              SPI_x_PRESCALER_SPEED2
 *              SPI_x_PRESCALER_SPEED3
 *              SPI_x_PRESCALER_SPEED4
 *              SPI_x_PRESCALER_SPEED5
 *
 *    @note   - Following are the Examples\n
 *              "#define SPI_1_PRESCALER_SPEED1      0"   The Spi1 Baud rate is 64MHz/2 when the Spi__SetSpeed() is called with SPI_SPEED_1
 *              "#define SPI_1_PRESCALER_SPEED3      4"   The Spi1 Baud rate is 64MHz/32 when the Spi__SetSpeed() is called with SPI_SPEED_3
 */
//=====================================================================================================================
#define SPI_1_PRESCALER_SPEED1  0
#define SPI_1_PRESCALER_SPEED2  1
#define SPI_1_PRESCALER_SPEED3  2
#define SPI_1_PRESCALER_SPEED4  3
#define SPI_1_PRESCALER_SPEED5  4

#define SPI_2_PRESCALER_SPEED1  0
#define SPI_2_PRESCALER_SPEED2  1
#define SPI_2_PRESCALER_SPEED3  2
#define SPI_2_PRESCALER_SPEED4  3
#define SPI_2_PRESCALER_SPEED5  4

#define SPI_3_PRESCALER_SPEED1  0
#define SPI_3_PRESCALER_SPEED2  1
#define SPI_3_PRESCALER_SPEED3  2
#define SPI_3_PRESCALER_SPEED4  3
#define SPI_3_PRESCALER_SPEED5  4

//=====================================================================================================================
/**
 *    @brief  - This macro defines the clock polarity when clock stops (i.e. Spi device is idle).
 *
 *    @param  - 0 - Low (L) Clock line is in Low state when Spi becomes idle.
 *              1 – High (H) Clock line is in High state when Spi becomes idle.
 *
 *    @note   - Following are the Examples\n
 *              "#define SPI_1_POL       0"   Clock line is in Low state when Spi1 becomes idle\n
 *              "#define SPI_2_POL       1"   Clock line is in High state when Spi2 becomes idle
 */
//=====================================================================================================================
#define SPI_1_POL               0
#define SPI_2_POL               0
#define SPI_3_POL               0

//=====================================================================================================================
/**
 *    @brief  - This macro defines the SPI clock phase select.
 *    @details- If CPHA (clock phase) bit is set, the second edge on the SCK pin (falling edge if the CPOL bit is reset,
 *              rising edge if the CPOL bit is set) is the MSBit capture strobe. Data is latched on the occurrence of
 *              the first clock transition.
 *              If CPHA bit is reset, the first edge on the SCK pin (falling edge if CPOL bit is set, rising edge if CPOL
 *              bit is reset) is the MSBit capture strobe. Data is latched on the occurrence of the second clock transition.
 *
 *    @param  - 1 - The second clock transition is the first data edge.
 *              0 - The first clock transition is the first data capture edge.
 *
 *    @note   - Following are the Examples\n
 *              "#define SPI_1_CPHA_EDGE      0"  The first clock transition is the first data capture edge\n
 *              "#define SPI_2_CPHA_EDGE      1"  The second clock transition is the first data capture edge
 */
//=====================================================================================================================
#define SPI_1_CPHA_EDGE         0
#define SPI_2_CPHA_EDGE         0
#define SPI_3_CPHA_EDGE         0

//=====================================================================================================================
/**
 *    @brief  - This macro Defines the SPI data transfer order MSB first or LSB first.
 *
 *    @param  - 1 - LSB first
 *              0 - MSB first
 *
 *    @note   - Following are the Examples\n
 *              "#define SPI_1_LSBFIRST      0"   Data order is MSB first\n
 *              "#define SPI_2_LSBFIRST      1"   Data order is LSB first
 */
//=====================================================================================================================
#define SPI_1_LSBFIRST          0
#define SPI_2_LSBFIRST          0
#define SPI_3_LSBFIRST          0

//=====================================================================================================================
/**
 *    @brief  - This macro Enables or Disables SPI Transmission by Reference or by buffer.
 *    @details- When this feature is Disabled the Spi Driver Copies the data byte (and String) sent by the Application
 *              and stores it in the Tx Buffer and initiates the Spi Communication.
 *              But when this feature is Enabled the Spi Driver uses the Address of the application buffer
 *              as reference and initiates the Spi Communication.
 *              Advantage - This feature saves memory(by not defining the Tx Buffer)
 *                          As well as speed of execution is also fast.
 *              Disadvantage – The Application buffer value (reference buffer) should not be altered
 *                             until the current transmission is successful.
 *
 *    @param  - ENABLED(1) - Which Enables the SPI Transmission by Reference.
 *              DISABLED(0) – Which Disables the SPI Transmission by Reference.
 *
 *    @note   - Following are the Examples\n
 *              "#define SPI_1_TX_BY_REFERENCE  ENABLED"   This Enables the SPI0 Transmission by Reference\n
 *              "#define SPI_1_TX_BY_REFERENCE  DISABLED"  This Disables the SPI0 Transmission by Reference
 */
//=====================================================================================================================
#define SPI_1_TX_BY_REFERENCE      DISABLED
#define SPI_2_TX_BY_REFERENCE      DISABLED
#define SPI_3_TX_BY_REFERENCE      DISABLED

#endif // SPI_PRM_H_

/**
 *  @file
 *
 *  @brief    Dynamic section for the I2c driver for the Cortex M0+
 *
 *  @details  This file defines configuration parameters and contains declarations for I2C1 & I2C2 module interrupts
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#ifndef I2C_PRM_H_
	#define I2C_PRM_H_

//=====================================================================================================================
//-------------------------------------- I2C1 and I2C2-------------------------------------------------------------------
//=====================================================================================================================
//=====================================================================================================================
/**
 *    @brief  - This macro Enables or Disables the I2c modules.
 *    @details- The macro must be Enabled before using in the application.
 *
 *    @param  - ENABLED(1)  - Which Enables the Specified I2c
 *              DISABLED(0) – Which Disables the Specified I2c
 *
 *    @note   - Following are the Examples\n
 *              "#define I2C1_FEATURE        ENABLED"   This Enables the I2c1 Feature\n
 *              "#define I2C1_FEATURE        DISABLED"  This Disables the I2c1 Feature\n
 *              "#define I2C2_FEATURE        ENABLED"   This Enables the I2c2 Feature\n
 *              "#define I2C2_FEATURE        DISABLED"  This Disables the I2c2 Feature
 */
#define I2C1_FEATURE                          DISABLED
#define I2C2_FEATURE                          DISABLED
#define I2C3_FEATURE                          DISABLED

//=====================================================================================================================
/**
 *    @brief  - This macro defines remap feature.
 *    @details- It basically provides alternate I/O pins for I2C1 device.
 *
 * <pre>
 *
 *
 *  The I2C_1 is available in the below port-pins of the STM32F0x Controllers
 *  ---------------------------------------------------------------------
 * |            I2C_1_SDA_            |            I2C_1_SCL_            |
 * |----------------------------------|----------------------------------|
 * |   PORT   |  PORTBIT |     AF     |   PORT   |  PORTBIT |     AF     |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTB  |    7     | GPIO_AF_6  |   PORTB  |    6     | GPIO_AF_6  |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTB  |    9     | GPIO_AF_6  |   PORTB  |    8     | GPIO_AF_6  |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTA  |    10    | GPIO_AF_6  |   PORTA  |    9     | GPIO_AF_6  |
 *  ---------------------------------------------------------------------
 *
 *
 *  The I2C_2 is available in the below port-pins of the STM32F0x Controllers
 *  ---------------------------------------------------------------------
 * |            I2C_2_SDA_            |            I2C_2_SCL_            |
 * |----------------------------------|----------------------------------|
 * |   PORT   |  PORTBIT |     AF     |   PORT   |  PORTBIT |     AF     |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTB  |    11    | GPIO_AF_6  |   PORTB  |    10    | GPIO_AF_6  |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTA  |    12    | GPIO_AF_6  |   PORTA  |    11    | GPIO_AF_6  |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTB  |    14    | GPIO_AF_6  |   PORTB  |    13    | GPIO_AF_6  |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTA  |    6     | GPIO_AF_8  |   PORTA  |    7     | GPIO_AF_8  |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTA  |    10    | GPIO_AF_8  |   PORTA  |    9     | GPIO_AF_8  |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTB  |    4     | GPIO_AF_8  |   PORTB  |    3     | GPIO_AF_8  |
 *  ---------------------------------------------------------------------
 *
 *
 *  The I2C_3 is available in the below port-pins of the STM32F0x Controllers
 *  ---------------------------------------------------------------------
 * |            I2C_3_SDA_            |            I2C_3_SCL_            |
 * |----------------------------------|----------------------------------|
 * |   PORT   |  PORTBIT |     AF     |   PORT   |  PORTBIT |     AF     |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTA  |    6     | GPIO_AF_9  |   PORTA  |    7     | GPIO_AF_9  |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTB  |    4     | GPIO_AF_6  |   PORTB  |    3     | GPIO_AF_6  |
 * |----------|----------|------------|----------|----------|------------|
 * |   PORTC  |    1     | GPIO_AF_6  |   PORTC  |    0     | GPIO_AF_6  |
 *  ---------------------------------------------------------------------
 *
 *  </pre>
 *
 *    @param  -
 *
 *    @note   - Following are the Examples for I2c_1
 *              "#define I2C_1_SDA_PORT          PORTB"
 *              "#define I2C_1_SDA_PORTBIT       7"
 *              "#define I2C_1_SDA__AF           GPIO_AF_1"
 *
 *              "#define I2C_1_SCL_PORT          PORTB"
 *              "#define I2C_1_SCL_PORTBIT       6"
 *              "#define I2C_1_SCL__AF           GPIO_AF_1"
 */
//! I2c_1 Pin configuration
#define I2C_1_SDA_PORT          PORTB
#define I2C_1_SDA_PORTBIT       9
#define I2C_1_SDA__AF           GPIO_AF_6

#define I2C_1_SCL_PORT          PORTB
#define I2C_1_SCL_PORTBIT       8
#define I2C_1_SCL__AF           GPIO_AF_6

//! I2c_2 Pin configuration
#define I2C_2_SDA_PORT          PORTA
#define I2C_2_SDA_PORTBIT       6
#define I2C_2_SDA__AF           GPIO_AF_8

#define I2C_2_SCL_PORT          PORTA
#define I2C_2_SCL_PORTBIT       7
#define I2C_2_SCL__AF           GPIO_AF_8

//! I2c_3 Pin configuration
#define I2C_3_SDA_PORT          PORTB
#define I2C_3_SDA_PORTBIT       4
#define I2C_3_SDA__AF           GPIO_AF_6

#define I2C_3_SCL_PORT          PORTB
#define I2C_3_SCL_PORTBIT       3
#define I2C_3_SCL__AF           GPIO_AF_6


//=====================================================================================================================
/**
 *    @brief  - This macro defines the Rx FIFO Buffer size.
 *    @details- Rx FIFO buffer size should be big enough so that the I2c1 driver can insert the read data into it.
 *
 *    @param  - 1,2,3…
 *
 *    @note   - Following are the Examples\n
 *              "#define I2C1_BUFFER_IN_SIZE            5"   The I2c1 Rx buffer size is 5\n
 *              "#define I2C1_BUFFER_IN_SIZE            10"  The I2c1 Rx buffer size is 10
 */
#define I2C1_BUFFER_IN_SIZE                   35
#define I2C2_BUFFER_IN_SIZE                   35
#define I2C3_BUFFER_IN_SIZE                   35

//=====================================================================================================================
/**
 *    @brief  - This macro defines the Tx FIFO Buffer size.
 *    @details- Tx FIFO buffer size should be big enough so that the application can insert the data into it.
 *
 *    @param  - 0,1,2,3…
 *
 *    @note   - Following are the Examples\n
 *              "#define I2C1_BUFFER_OUT_SIZE            5"      The I2c1 Tx buffer size is 5\n
 *              "#define I2C1_BUFFER_OUT_SIZE            10"     The I2c1 Tx buffer size is 10
 */
#define I2C1_BUFFER_OUT_SIZE                  35
#define I2C2_BUFFER_OUT_SIZE                  35
#define I2C3_BUFFER_OUT_SIZE                  35

//=====================================================================================================================
/**
 *    @brief  - This macro groups the interrupts as per priority of interrupts.
 *    @details- I2C1_INTERRUPT_PREEMPTION_PRIORITY this macro group a certain no. of interrupts and sets their priority
 *              This macro basically groups interrupts and sets their priority
 *    @param  - 0/1/2/3
 *
 *    @note   - Following are the Examples
 *              "#define I2C1_INTERRUPT_PREEMPTION_PRIORITY            0"
 *              "#define I2C1_INTERRUPT_PREEMPTION_PRIORITY            1"
 */
#define I2C1_INTERRUPT_PREEMPTION_PRIORITY    3
#define I2C2_3_INTERRUPT_PREEMPTION_PRIORITY  3

#endif /*I2C_PRM_H_*/



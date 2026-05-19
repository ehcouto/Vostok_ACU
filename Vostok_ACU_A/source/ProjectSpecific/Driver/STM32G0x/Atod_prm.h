/**
 *  @file
 *  @defgroup   CLASS_B
 *
 *  @brief      All the possible configurations are defined for Atod module.
 *
 * <pre>
 *               Following table contains the ADC Channel, Port, Pin associated with ADC module for STM32F0x
 *                --------------------------------------
 *               | ATOD Channel   |  Port    |   Pin    |
 *               |----------------|----------|----------|
 *               |    ATOD_CH0    |   PORTA  |    0     |
 *               |----------------|----------|----------|
 *               |    ATOD_CH1    |   PORTA  |    1     |
 *               | ---------------|----------|----------|
 *               |    ATOD_CH2    |   PORTA  |    2     |
 *               |----------------|----------|----------|
 *               |    ATOD_CH3    |   PORTA  |    3     |
 *               |----------------|----------|----------|
 *               |    ATOD_CH4    |   PORTA  |    4     |
 *               |----------------|----------|----------|
 *               |    ATOD_CH5    |   PORTA  |    5     |
 *               |----------------|----------|----------|
 *               |    ATOD_CH6    |   PORTA  |    6     |
 *               |----------------|----------|----------|
 *               |    ATOD_CH7    |   PORTA  |    7     |
 *               |----------------|----------|----------|
 *               |    ATOD_CH8    |   PORTB  |    0     |
 *               |----------------|----------|----------|
 *               |    ATOD_CH9    |   PORTB  |    1     |
 *               |----------------|----------|----------|
 *               |    ATOD_CH10   |   PORTB  |    2     |
 *               |----------------|----------|----------|
 *               |    ATOD_CH11   |   PORTB  |    10    |
 *               |----------------|----------|----------|
 *               |    ATOD_CH12   |  Internal Channel   |
 *               |                | (Temperature Sensor)|
 *               |----------------|----------|----------|
 *               |    ATOD_CH13   |  Internal Channel   |
 *               |                |     (Vrefint)       |
 *               |----------------|----------|----------|
 *               |    ATOD_CH14   |  Monitoring Vbat    |
 *               |                |     (Vbat/3)        |
 *               |----------------|----------|----------|
 *               |    ATOD_CH15   |   PORTB  |    11    |
 *               |----------------|----------|----------|
 *               |    ATOD_CH16   |   PORTB  |    12    |
 *               |----------------|----------|----------|
 *               |    ATOD_CH17   |   PORTC  |    4     |
 *               |----------------|----------|----------|
 *               |    ATOD_CH18   |   PORTC  |    5     |
 *                -------------------------------------- </pre>
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#ifndef ATOD_PRM_H
    #define ATOD_PRM_H

//=====================================================================================================================
/**
 * @brief  -  If the application is reading the ADC conversion in the fixed resolution provided that all the channels use the
 *              resolution then its recommenced to use the below macro in order to reduce the code.
 *
 *              Comment this macro to use the Dynamic Resolution configuration
 *
 * @param  -    4      for 4 bits resolution
 *              8      for 8 bits resolution
 *              10     for 10 bits resolution
 *              12     for 12 bits resolution
 *              14     for 14 bits resolution
 *              16     for 16 bits resolution
 *
 * @note   - Following are the Examples\n
 *           "#define ATOD_HARDCODED_RESOLUTION           12"  12-bit hard-coded resolution is enabled\n
 *           "#define ATOD_HARDCODED_RESOLUTION           4"   4-bit hard-coded resolution is enabled
 */
//=====================================================================================================================
//#define ATOD_HARDCODED_RESOLUTION           12

#endif

#define MCI_CURRENT_CHANNEL      ATOD_CH6
#define MCI_VOLTAGE_CHANNEL      ATOD_CH17

#define ATOD_CH6_SAMPLE_TIME    SAMPLE_TIME_12_5_CYCLE
#define ATOD_CH17_SAMPLE_TIME   SAMPLE_TIME_12_5_CYCLE

#define ATOD_CH0_SAMPLE_TIME	SAMPLE_TIME_12_5_CYCLE
#define ATOD_CH1_SAMPLE_TIME	SAMPLE_TIME_12_5_CYCLE
#define ATOD_CH2_SAMPLE_TIME	SAMPLE_TIME_12_5_CYCLE
#define ATOD_CH4_SAMPLE_TIME	SAMPLE_TIME_12_5_CYCLE
#define ATOD_CH5_SAMPLE_TIME	SAMPLE_TIME_12_5_CYCLE
#define ATOD_CH7_SAMPLE_TIME	SAMPLE_TIME_12_5_CYCLE
#define ATOD_CH8_SAMPLE_TIME	SAMPLE_TIME_12_5_CYCLE
#define ATOD_CH11_SAMPLE_TIME	SAMPLE_TIME_12_5_CYCLE
#define ATOD_CH15_SAMPLE_TIME	SAMPLE_TIME_79_5_CYCLE


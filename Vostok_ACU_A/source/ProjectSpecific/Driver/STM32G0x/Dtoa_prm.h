/**
 *  @file       
 *
 *  @brief      Public configurable parameters of the Dtoa module.
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
#ifndef DTOA_PRM_H_
#define DTOA_PRM_H_
#include "C_Extensions.h"

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  - This macro Enables or Disables the Dtoa Channels
 *
 *    @param  - ENABLED(1)  - Which Enables the Specified Dtoa Channel
 *              DISABLED(0) – Which Disables the Specified Dtoa Channel
 *
 *              DtoA channels are available in the below port-pins of STM32F0x -
 *               <pre>
 *               --------------------------------------
 *              | ATOD Channel   |  Port    |   Pin    |
 *              |----------------|----------|----------|
 *              |    DTOA_CH1    |   PORTA  |    4     |
 *              |----------------|----------|----------|
 *              |    DTOA_CH2    |   PORTA  |    5     |
 *               -------------------------------------- </pre>
 *
 *
 *    @note   - Following are the Examples\n
 *              "#define DTOA_CH1_FEATURE        ENABLED"   This Enables the DTOA_CH1 Feature\n
 *              "#define DTOA_CH1_FEATURE        DISABLED"  This Disables the DTOA_CH1 Feature\n
 */
#define DTOA_CH1_FEATURE                      DISABLED
#define DTOA_CH2_FEATURE                      DISABLED

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  - This macro enables the hard coded Dtoa channel resolution
 *
 *    @param  - 0  - Hard Coded resolution is Disabled so application can change Dynamically
 *              8  - DTOA_RESOLUTION_08BITS resolution is selected
 *              12 - DTOA_RESOLUTION_12BITS resolution is selected
 *
 *    @note   - Following are the Examples\n
 *              "#define DTOA_CHANNEL_RESOLUTION_HARDCODED        0"  Hard coded resolution disabled\n
 *              "#define DTOA_CHANNEL_RESOLUTION_HARDCODED        8"  DTOA_RESOLUTION_08BITS is selected\n
 */
#define DTOA_CHANNEL_RESOLUTION_HARDCODED     8

#endif // DTOA_PRM_H_

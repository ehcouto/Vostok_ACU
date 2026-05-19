/**
 *  @file
 *  @brief      Contains controller specific definition for STM32F0x
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#ifndef I2C_DEFS_H_
#define I2C_DEFS_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void I2c1__InterruptHandler(void);
void I2c2_3__InterruptHandler(void);

#endif // I2C_DEFS_H_


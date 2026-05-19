/**
 *  @file       
 *
 *  @brief      This module is the standard public interface for the I2c manager module
 *
 *  $Header: I2cMgr.h 1.1 2016/08/31 13:12:31EDT Seth Herndon (HERNDS) Exp  $
 *
 *  @copyright  Copyright 2016-$Date: 2016/08/31 13:12:31EDT$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef I2CMGR_H_
#define I2CMGR_H_

#include "I2c.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Define invalid handle value
 * @note  This value is always 0xFF since it is unlikely to have 255 i2c devices \n
 * #define I2CMGR_HANDLE_INVALID       (0xFF)
 *
 */
#define I2CMGR_HANDLE_INVALID       (0xFF)

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void I2cMgr__Initialize(void);

uint8 I2cMgr__GetDeviceHandle(I2C_ENUM_TYPE i2c_channel);
void I2cMgr__ReleaseDeviceHandle(uint8 device_id);

BOOL_TYPE I2cMgr__RequestBus(uint8 device_id);
BOOL_TYPE I2cMgr__ReleaseBus(uint8 device_id);

void I2cMgr__Handler(void);

#endif // I2CMGR_H_



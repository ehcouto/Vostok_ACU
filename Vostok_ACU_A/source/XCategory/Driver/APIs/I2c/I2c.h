/**
 *  @file
 *
 *  @brief      This module provides Standard API for I2c Driver.
 *
 *
 *  $Header: I2c.h 1.1 2015/07/02 14:36:31EDT Dasar Hanumanth (DASARHB) Exp  $
 *
 *  @copyright  Copyright 2010-$Date: 2015/07/02 14:36:31EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef I2C_H
	#define I2C_H
#include "C_Extensions.h"                           // Defined in Micro module
#include "uc.h"                                     // Defined in Micro module
#include "I2c_prm.h"
#include "I2c_Defs.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef enum
{
	I2C0,
	I2C1,
	I2C2,
	I2C3,
	I2C4,
	I2C5,
	I2C6,
	I2C7,
	I2C8,
	I2C9,
	I2C10,
	I2C11,
	I2C12,
	I2C13,
	I2C14,
	I2C15
} I2C_ENUM_TYPE;

typedef enum
{
	I2C_ADDR_7BITS,
	I2C_ADDR_10BITS
} I2C_ADRESS_TYPE;


typedef enum
{
	I2C_50KHZ,
	I2C_100KHZ,
	I2C_400KHZ
} I2C_SPEED_TYPE;


typedef enum
{
	I2C_STATE_IDLE,
	I2C_STATE_BUSY
} I2C_STATE_TYPE;

typedef enum
{
	I2C_ERROR_NONE,
	I2C_ERROR_ACK,
	I2C_ERROR_BUS_LOCKED
} I2C_ERROR_TYPE;


typedef enum
{
	I2C_STATUS_STATE,
	I2C_STATUS_BUFFER_IN,
    I2C_STATUS_ERROR,
	I2C_STATUS_SLAVE_ACTIVITY
} I2C_STATUS_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
BOOL_TYPE I2c__Initialize(I2C_ENUM_TYPE i2c,I2C_SPEED_TYPE speed, I2C_ADRESS_TYPE addr_type, uint8 i2c_addr);
void I2c__Clear(I2C_ENUM_TYPE i2c);
void I2c__Enable(I2C_ENUM_TYPE i2c);
void I2c__Disable(I2C_ENUM_TYPE i2c);
BOOL_TYPE I2c__ResetBus(I2C_ENUM_TYPE i2c);
BOOL_TYPE I2c__Write(I2C_ENUM_TYPE i2c, uint8 internal_address, uint8 * buf,uint16 size);
BOOL_TYPE I2c__BlockingWrite(I2C_ENUM_TYPE i2c,uint8 internal_address, uint8 * buf,uint16 size);
BOOL_TYPE I2c__RequestRead(I2C_ENUM_TYPE i2c,uint16 size);
BOOL_TYPE I2c__Read(I2C_ENUM_TYPE i2c, uint8 * buf,uint16 size);
BOOL_TYPE I2c__BlockingRead(I2C_ENUM_TYPE i2c, uint8 * buf,uint16 size);
BOOL_TYPE I2c__WriteThenRead(I2C_ENUM_TYPE i2c, uint8* write_buf, uint16 write_size, uint16 read_size);
uint16 I2c__GetStatus(I2C_ENUM_TYPE i2c,I2C_STATUS_TYPE index);
BOOL_TYPE I2c__SetSlaveBuffer(I2C_ENUM_TYPE i2c,uint8 * buffer, uint8 size);
BOOL_TYPE I2c__WriteThenRead(I2C_ENUM_TYPE i2c, uint8* write_buf, uint16 write_size, uint16 read_size);

#endif

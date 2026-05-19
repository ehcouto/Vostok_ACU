/**
 *    @file     
 *    @brief    Spi GDM module implements the Spi Standard API (Application Programming Interface).
 *    @details  This module provides the synchronous serial communication services to the application,
 *              such as initialization of Spi module, transmission/reception of data byte and
 *              multiple bytes via Spi and reads the status of Spi communication.
 *  $Header: Spi.h 1.1 2015/07/30 14:50:30EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  *****  Copyright 2010-2011.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL  *****
 */
#ifndef SPI_H
	#define SPI_H
#include "C_Extensions.h"
#include "uc.h"
#include "Spi_prm.h"
#include "Spi_defs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef enum
{
	SPI0,
	SPI1,
	SPI2,
	SPI3,
	SPI4,
	SPI5,
	SPI6,
	SPI7,
	SPI8,
	SPI9,
	SPI10,
	SPI11,
	SPI12,
	SPI13,
	SPI14,
	SPI15
} SPI_ENUM_DEF;


typedef enum
{
	SPI_STATUS_STATE,
	SPI_STATUS_BUFFER_IN
} SPI_STATUS_DEF;

typedef enum
{
	SPI_STATE_BUSY,
	SPI_STATE_IDLE,
	SPI_STATE_ERROR
} SPI_STATE_DEF;

typedef enum
{
	SPI_POLARITY_SCK_IDLE_0,
	SPI_POLARITY_SCK_IDLE_1
} SPI_POLARITY_TYPE;

typedef enum
{
	SPI_EDGE_FIRST,
	SPI_EDGE_SECOND
} SPI_EDGE_TYPE;

typedef enum
{
	SPI_ODER_MSB_FIRST,
	SPI_ODER_LSB_FIRST
} SPI_ORDER_TYPE;


typedef enum
{
    SPI_SPEED_1,
    SPI_SPEED_2,
    SPI_SPEED_3,
    SPI_SPEED_4,
    SPI_SPEED_5
} SPI_SPEED_TYPE;



//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void Spi__Initialize(SPI_ENUM_DEF spi);
BOOL_TYPE Spi__Write(SPI_ENUM_DEF spi, uint8 * buf, uint16 size);
BOOL_TYPE Spi__Read(SPI_ENUM_DEF spi, uint8 * buf, uint16 size);
uint16 Spi__GetStatus(SPI_ENUM_DEF spi, SPI_STATUS_DEF index);
void Spi__Enable(SPI_ENUM_DEF spi);
void Spi__Disable(SPI_ENUM_DEF spi);
void Spi__Clear(SPI_ENUM_DEF spi);
void Spi__SetConfiguration(SPI_ENUM_DEF spi,SPI_POLARITY_TYPE pol,SPI_EDGE_TYPE edge,SPI_ORDER_TYPE order);
void Spi__SetSpeed(SPI_ENUM_DEF spi, SPI_SPEED_TYPE speed);
#endif

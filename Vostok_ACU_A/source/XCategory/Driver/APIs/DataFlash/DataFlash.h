/**
 *  @file       DataFlash.h
 *  @brief      This module provides the interface to the application
 *              in order to write, read, fill or erase in to the Flash memory
 *
 *  $Header: DataFlash.h 1.2 2015/06/24 14:53:57EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2010-$Date: 2015/06/24 14:53:57EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

#ifndef DATAFLASH_H
    #define DATAFLASH_H

#include "C_Extensions.h"
#include "SRMicro.h"
#include "DataFlash_def.h"
#include "DataFlash_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef enum
{
    DATAFLASH_RESPONSE_DONE,
    DATAFLASH_RESPONSE_ACCEPTED,
    DATAFLASH_RESPONSE_ERROR_BUSY,
    DATAFLASH_RESPONSE_WRITE_ERROR,
    DATAFLASH_RESPONSE_ERROR_OUT_OF_RANGE,
    DATAFLASH_RESPONSE_INVALID_SIZE
} DATAFLASH_RESPONSE_TYPE;

typedef enum
{
    DATAFLASH_STATE_BUSY,
    DATAFLASH_STATE_FREE,
    DATAFLASH_STATE_ERROR
} DATAFLASH_STATE_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void DataFlash__Initialize(void);
DATAFLASH_RESPONSE_TYPE DataFlash__Write(ADDRESS_TYPE destination, uint8 * source, SIZE_TYPE size);
DATAFLASH_RESPONSE_TYPE DataFlash__Read(uint8 * destination, ADDRESS_TYPE source, SIZE_TYPE size);
#if (FILL_DATA_FEATURE == ENABLED)
    DATAFLASH_RESPONSE_TYPE DataFlash__Fill(ADDRESS_TYPE destination, SIZE_TYPE size, uint8 value);
#endif
DATAFLASH_RESPONSE_TYPE DataFlash__FormatPage(ADDRESS_TYPE page_init);
void DataFlash__Handler(void);
DATAFLASH_STATE_TYPE DataFlash__GetStatus(void);

#endif

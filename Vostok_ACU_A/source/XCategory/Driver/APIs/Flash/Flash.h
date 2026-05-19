/**
 *  @file
 *
 *  @brief      This module provides the interface to the application in order to write/read, or erase in to the Flash memory
 *
 *  $Header: Flash.h 1.1 2015/04/09 16:19:28EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2013-$Date: 2015/04/09 16:19:28EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef FLASH_H_
#define FLASH_H_

#include "C_Extensions.h"
#include "Flash_def.h"
#include "Flash_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef enum
{
    FLASH_RESPONSE_DONE,
    FLASH_RESPONSE_ACCEPTED,
    FLASH_RESPONSE_ERROR_BUSY,
    FLASH_RESPONSE_WRITE_ERROR,
    FLASH_RESPONSE_ERROR_OUT_OF_RANGE,
    FLASH_RESPONSE_INVALID_SIZE
} FLASH_RESPONSE_TYPE;

typedef enum
{
    FLASH_STATE_BUSY,
    FLASH_STATE_FREE,
    FLASH_STATE_ERROR
} FLASH_STATE_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void Flash__Initialize(void);
FLASH_RESPONSE_TYPE Flash__Write(FLASH_ADDRESS_TYPE destination, uint8 *source, FLASH_SIZE_TYPE size);
FLASH_RESPONSE_TYPE Flash__Format(FLASH_ADDRESS_TYPE start_address, FLASH_ADDRESS_TYPE end_address);
FLASH_RESPONSE_TYPE Flash__FormatPage(FLASH_ADDRESS_TYPE page_init);
FLASH_RESPONSE_TYPE Flash__FormatChip(void);
FLASH_RESPONSE_TYPE Flash__Read(uint8 *destination, FLASH_ADDRESS_TYPE source, FLASH_SIZE_TYPE size);
FLASH_STATE_TYPE Flash__GetStatus(void);

#endif

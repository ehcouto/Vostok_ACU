/**
 *  @file       VirtualPin.h
 *
 *  @brief      Public interface file for the VirtualPin module.
 *
 *	@defgroup	CLASS_B
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/14 17:37:56EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef VIRTUALPIN_H_
#define VIRTUALPIN_H_

#include "Gpio.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

PACKED typedef struct
{
    GPIO_PORT_TYPE port;
    uint8 pin;
    GPIO_PIN_MODE_TYPE pin_config;
    uint8 device;           // ATOD CHANNEL, SPI Channel, I2c channel,

    uint8 pin_remap;
    uint8 param0;
    uint8 param1;            // for ATOd this is the resolution
    uint8 param2;

}VIRTUALPIN_TYPE;



extern const VIRTUALPIN_TYPE VIRTUALPIN_TABLE[];

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

uint8 VirtualPin__GetNumOf(void);

#endif // SOURCE_HBL_VIRTUALPIN_VIRTUALPIN_H_



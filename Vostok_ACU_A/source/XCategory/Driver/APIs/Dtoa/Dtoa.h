/**
 *  @file
 *
 *  @brief      Public Interface for Dtoa Driver Module for the Whirlpool strategy micro-controllers
 *
 * @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 * $Header: $
 *
 *  @copyright  Copyright 2017. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 *
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef DTOA_H_
#define DTOA_H_

#include "C_Extensions.h"
#include "Dtoa_defs.h"
#include "Dtoa_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//DTOA_CHANNEL_DEF defines the list of channels the API supports
//Some of channels in this list can be not supported depending on HW and implementation.
typedef enum
{
    DTOA_CH0,
    DTOA_CH1,
    DTOA_CH2,
    DTOA_CH3,
    DTOA_CH4,
    DTOA_CH5,
    DTOA_CH6,
    DTOA_CH7,
} DTOA_CHANNEL_DEF;

//DTOA_CHANNEL_RESOLUTION_DEF defines the possible resolution with which the channels can be set
typedef enum
{
    DTOA_RESOLUTION_08BITS  = 8,
    DTOA_RESOLUTION_10BITS  = 10,
    DTOA_RESOLUTION_12BITS  = 12,
    DTOA_RESOLUTION_14BITS  = 14,
    DTOA_RESOLUTION_16BITS  = 16,
} DTOA_CHANNEL_RESOLUTION_DEF;

#ifndef DTOA_CONTINUOUS_CONVERSION_FEATURE
    #define DTOA_CONTINUOUS_CONVERSION_FEATURE      DISABLED
#endif

#if (DTOA_CONTINUOUS_CONVERSION_FEATURE == ENABLED)
    #include "Dma.h"
#endif

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void Dtoa__Initialize(void);
void Dtoa__Disable(DTOA_CHANNEL_DEF channel);
PASS_FAIL_TYPE Dtoa__Convert(DTOA_CHANNEL_DEF channel, DTOA_CHANNEL_RESOLUTION_DEF resolution, uint16 sampling_hz, uint32 source_address, uint32 size);
BOOL_TYPE Dtoa__IsFree(DTOA_CHANNEL_DEF channel);

//below API's will be obsolete...for new development use the above API's only
BOOL_TYPE Dtoa__EnableConversion(DTOA_CHANNEL_DEF channel);
BOOL_TYPE Dtoa__DisableConversion(DTOA_CHANNEL_DEF channel);
BOOL_TYPE Dtoa__SetValue(DTOA_CHANNEL_DEF channel, uint16 value);


#endif // DTOA_H_



/**
 *  @file
 *
 *  @brief      Public interface to the Utilities module.
 *
 *  @details    Refer to the Utilities.c source file for more detailed information.
 *
 *  @copyright  Copyright 2011-2023 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef UTILITIES_H_
#define UTILITIES_H_

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Constants, Macros, Enumerations, & Typedefs) -------------------------
//=====================================================================================================================

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

uint32 Utilities__ConvertArrayTo32bits(uint8 * array);
uint32 Utilities__ConvertArrayTo24bits(uint8 * array);
uint16 Utilities__ConvertArrayTo16bits(uint8 * array);
uint16 Utilities__Merg2BytesTo16bits(uint8 msb, uint8 lsb);

void Utilities__SetBitInArray(uint8 * array, uint16 bit, uint8 value);
void Utilities__ToggleBitInArray(uint8 * array, uint16 bit);
uint8 Utilities__GetBitInArray(uint8 * array, uint16 bit);
uint16 Utilities__CountOneBitsInArray(uint8 * array, uint8 size);

#endif // UTILITIES_H_

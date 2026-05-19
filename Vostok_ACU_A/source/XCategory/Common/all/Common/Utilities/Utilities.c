/**
 *  @file
 *
 *  @brief      Collection of commonly used operations.
 *
 *  @details    Commonly used operations implemented in this module:
 *                  - Merging Bytes
 *                  - Manipulating Bits in arrays
 *
 *              The functions in this file are independent of microcontroller endianess.
 *
 *  @copyright  Copyright 2011-2023 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "Utilities.h"
//-------------------------------------- PRIVATE (Compile Directives) -------------------------------------------------

//-------------------------------------- PRIVATE (Constants, Macros, Enumerations, Typedefs, & Variables) -------------

static const uint8 BYTE_SET_MASK[] = {0x01,0x02,0x04,0x08,0x10,0x20,0x40,0x80};
static const uint8 BYTE_CLR_MASK[] = {0xFE,0xFD,0xFB,0xF7,0xEF,0xDF,0xBF,0x7F};

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method extracts a 32bits uint32 from an uint8 array
 * @param array uint8 * pointer to the source array
 * @return uint32 resulting of the data extraction.
 */
uint32 Utilities__ConvertArrayTo32bits(uint8 * array)
{
	uint32 retval;
    retval = ((uint32)array[0]) << 24;
    retval += ((uint32)array[1]) << 16;
    retval += ((uint32)array[2]) << 8;
    retval += ((uint32)array[3]);
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method extracts a 24bits uint32 from an uint8 array
 * @param array uint8 *pointer to the source array.
 * @return uint32 resulting of the data extraction.
 */
uint32 Utilities__ConvertArrayTo24bits(uint8 * array)
{
	uint32 retval;
    retval = ((uint32)array[0]) << 16;
    retval += ((uint32)array[1]) << 8;
    retval += ((uint32)array[2]);
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method merges 2 bytes into a uint16 int 16bits
 * @param msb most significant byte
 * @param lsb least significant byte
 * @return uint16 with it the result of the merging.
 */
uint16 Utilities__Merg2BytesTo16bits(uint8 msb, uint8 lsb)
{
	uint16 retval;
    retval = ((uint16)msb) << 8;
    retval += ((uint16)lsb);
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method extracts a 16bits uint16 from an uint8 array
 * @param array uint16 *pointer to the source array.
 * @return uint16 resulting of the data extraction.
 */
uint16 Utilities__ConvertArrayTo16bits(uint8 * array)
{
    return (Utilities__Merg2BytesTo16bits(array[0],array[1]));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method is used to set a bit within an uint8 array
 * @details the bit would vary from 0 to 255 which means the array can be only 32bytes long
 * @param array pointer to the uint8 array
 * @param bit bit in the array
 * @param value value to be set in the bit (0 / 1)
 */
void Utilities__SetBitInArray(uint8 * array, uint16 bit, uint8 value)
{
    uint8 s_byte;
    uint8 s_bit;
    s_byte = (uint8)(bit / 8U);
    s_bit = bit % 8U;

    if (value != 0U)
    {
        array[s_byte] |= BYTE_SET_MASK[s_bit];
    }
    else
    {
        array[s_byte] &= BYTE_CLR_MASK[s_bit];
    }

}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method is used to set a bit within an uint8 array
 * @details the bit would vary from 0 to 255 which means the array can be only 32bytes long
 * @param array pointer to the uint8 array
 * @param bit bit in the array
 * @return Value of the specific bit
 */
uint8 Utilities__GetBitInArray(uint8 * array, uint16 bit)
{
    return (array[bit / 8U] >> (bit % 8U)) & 1U;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method is used to toggle a bit within an uint8 array
 * @details the bit would vary from 0 to 255 which means the array can be only 32bytes long
 * @param array pointer to the uint8 array
 * @param bit bit in the array
 */
void Utilities__ToggleBitInArray(uint8 * array, uint16 bit)
{
    array[bit / 8U] ^= BYTE_SET_MASK[bit % 8U];
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method counts the number of bits with a value of one in an array
 * @param array uint8 *pointer to the source array.
 * @param size number of bytes in the array.
 * @return The number of bits with a value of one in the array.
 */
uint16 Utilities__CountOneBitsInArray(uint8 * array, uint8 size)
{
    uint16 retval;
    uint8 byte_index;
    uint8 byte_value;
    uint8 bit_mask;

    retval = 0;
    for (byte_index = 0; byte_index < size; byte_index++)
    {
        byte_value = array[byte_index];
        for (bit_mask = 0x80U; bit_mask > 0U; bit_mask >>= 1)
        {
            if ((byte_value & bit_mask) > 0U)
            {
                retval++;
            }
        }
    }
    return retval;
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

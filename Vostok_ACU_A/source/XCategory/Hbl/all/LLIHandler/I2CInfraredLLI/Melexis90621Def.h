/**
 * @file
 * @brief       Public interface to the Melexis90621Def module.
 *
 * @details     Refer to the Melexis90621Def.c source file for more detailed information.
 *
 * @copyright   Copyright 2018 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef MELEXIS90621DEF_H_
#define MELEXIS90621DEF_H_

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================

#define NUMBER_OF_PIXELS 64U

#define KELVIN 273.15F
#define TA_REF 25.0F


// -- Public Type Declarations --

//! Bit resolutions for the IR sensor.
typedef enum IR_BIT_RESOLUTION_ENUM
{
    IR_RESOLUTION_15_BIT            = 0,
    IR_RESOLUTION_16_BIT            = 1,
    IR_RESOLUTION_17_BIT            = 2,
    IR_RESOLUTION_18_BIT            = 3
} IR_BIT_RESOLUTION_TYPE;


//! The status of the I2CInfraredLLI module as it interfaces with the Melexis 90621 Sensor.
typedef enum I2C_IR_LLI_STATUS_ENUM
{
    I2C_IR_LLI_UNSPECIFIED,
    I2C_IR_LLI_INITIALIZING,
    I2C_IR_LLI_WAITING_FOR_DATA,
    I2C_IR_LLI_ACQUIRING_DATA,
    I2C_IR_LLI_DATA_READY,
    I2C_IR_LLI_ERROR
} I2C_IR_LLI_STATUS_TYPE;

//! The data structure of the Melexis 90621 on-chip EEPROM.
//! The data is arranged to avoid a PACKED data structure. PACKing the data would slow down the image processing.
typedef /*NOT PACKED*/ struct MELEXIS_90621_EEPROM_STRUCT
{
    // EEPROM Data Follows                          //   EEPROM Addresses
    uint8 Ai_Pixel_Offset[NUMBER_OF_PIXELS];        //!<    0x00 - 0x3F
    sint8 Bi_Ta_Dependence[NUMBER_OF_PIXELS];       //!<    0x40 - 0x7F
    uint8 Sensitivity[NUMBER_OF_PIXELS];            //!<    0x80 - 0xBF
    sint16 A_Common_Offset;                         //!<    0xD0 - 0xD1
    uint8 KT_Scale;                                 //!<    0xD2
    uint8 A_Compensation_Low;                       //!<    0xD3
    uint8 A_Compensation_High;                      //!<    0xD4
    sint8 B_Compensation;                           //!<    0xD5
    uint16 Sensitivity_Pixel;                       //!<    0xD6 - 0xD7
    sint8 Thermal_Gradient_Coeff;                   //!<    0xD8
    uint8 B_Compensation_Scale : 4;                 //!<    0xD9 (low nibble)
    uint8 A_Compensation_Scale : 4;                 //!<    0xD9 (high nibble)
    sint16 Ta_Vth0;                                 //!<    0xDA - 0xDB
    sint16 Ta_KT1;                                  //!<    0xDC - 0xDD
    sint16 Ta_KT2;                                  //!<    0xDE - 0xDF
    uint16 Sensitivity_Common;                      //!<    0xE0 - 0xE2
    uint8 Sensitivity_Common_Scale;                 //!<    0xE2
    uint8 Sensitivity_Scale;                        //!<    0xE3
    uint16 Emissivity;                              //!<    0xE4 - 0xE5
    sint16 KsTa;                                    //!<    0xE6 - 0xE7
} MELEXIS_90621_EEPROM_TYPE;

//! The data structure provided by the I2CInfraredLLI.
//! The data is arranged to avoid a PACKED data structure. PACKing the data would slow down the image processing.
typedef /*NOT PACKED*/ struct MELEXIS_90621_DATA_STRUCT
{
    I2C_IR_LLI_STATUS_TYPE LLI_Status;              //!< The status of the LLI.
    uint8 Error_Count;                              //!< Number of times that a communication error was detected.
    IR_BIT_RESOLUTION_TYPE Resolution;              //!< The sensor resolution.
    MELEXIS_90621_EEPROM_TYPE const* Eeprom;        //!< A pointer to a copy of the Melexis 90621 EEPROM memory.
                                                    //!< Pointer is used to keep LLI data below 256 (required by HBL)
    sint16 Pixel[NUMBER_OF_PIXELS];                 //!< The individual measurements from the IR Pixels.
    uint16 PTAT;                                    //!< The ambient temperature data.
    sint16 V_CP;                                    //!< The compensation pixel result.
} MELEXIS_90621_DATA_TYPE;


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


#endif      // MELEXIS90621DEF_H_

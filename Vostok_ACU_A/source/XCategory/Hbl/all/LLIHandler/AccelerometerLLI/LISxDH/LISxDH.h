/**
 * @file
 *
 * @brief       Public interface to the LISxDH module.
 *
 * @details     Refer to the LISxDH.c source file for more detailed information.
 *
 * @copyright   Copyright 2022 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef LISXDH_H_
#define LISXDH_H_

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --
#include "I2c.h"
#include "AccelerometerLLI_Defs.h"

//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public Enumerated Constant Declarations --

//! Accelerometer modes supported by the module
typedef enum LISXDH_MODE_ENUM
{
    //! Accelerometer is disabled (low power). No accelerometer acquisition is taking place.
    LISXDH_MODE_DISABLED = 0,

    //! Accelerometer is enabled and sampling points based on user configuration.
    LISXDH_MODE_NORMAL,

    //! Accelerometer is running self-test reference data at 400Hz.
    LISXDH_MODE_ST_REF,

    //! Accelerometer is running self-test #0 with sampling at 400Hz.
    LISXDH_MODE_ST_0,

    //! Accelerometer is running self-test #1 with sampling at 400Hz.
    LISXDH_MODE_ST_1,

    //! The number of accelerometer modes.
    LISXDH_MODE_COUNT
} LISXDH_MODE_TYPE;

//! Setting file data
PACKED typedef struct
{
    uint16 Reserved;            // Bytes where common LLI parameters reside (not used by this module)
    ACCELEROMETER_DEVICE_TYPE Device;   // Identifies the accelerometer device
    uint8  Unused_0;            // Unused byte
    uint8  Port           : 4; // I2C peripheral (I2C_ENUM_TYPE) or SPI peripheral (SPI_ENUM_DEF)
    uint8  Resolution     : 2;  // 0 = 8 bits, 1 = 10 bits, 2 = 12 bits
    uint8  Fifo_Mode      : 2;  // 0 = Bypass, 1 = FIFO, 2 = Stream, 3 = Trigger
    uint8  I2C_Address    : 6;  // Address of the accelerometer on the I2C bus
    I2C_SPEED_TYPE I2C_Speed : 2;  // Speed of the I2C bus
    uint8  Scale          : 2;  // 0 = +/- 2g, 1 = +/- 4g, 2 = +/- 8g, 3 = +/- 16g
    uint8  Trigger_Source : 1;  // 0 = INT1, 1 = INT2
    uint8  Big_Endian     : 1;  // 0 = Little Endian, 1 = Big_Endian
    uint8  Block_Update   : 1;  // 0 = Disabled, 1 = Enabled
    uint8  Z_Axis_Enabled : 1;  // 0 = Disabled, 1 = Enabled
    uint8  Y_Axis_Enabled : 1;  // 0 = Disabled, 1 = Enabled
    uint8  X_Axis_Enabled : 1;  // 0 = Disabled, 1 = Enabled
    uint8  Data_Rate      : 4;  // 0 = Power Down Mode, 1 = 1Hz, 2 = 10Hz, 3 = 25Hz, 4 = 50Hz, 5 = 100Hz, 6 = 200Hz, 7 = 400Hz, 8 = 1620Hz (8 bit resolution only), 9 = 1344Hz (10/12 bit resolution) OR 5376Hz (8 bit resolution)
    uint8  Unused_1       : 4;  // Unused bits
    uint8  X_Transform_Axis : 2; // 0 = X Axis, 1 = Y Axis, 2 = Z Axis
    uint8  X_Transform_Sign : 1; // 0 = Negative, 1 = Positive
    uint8  Y_Transform_Axis : 2; // 0 = X Axis, 1 = Y Axis, 2 = Z Axis
    uint8  Y_Transform_Sign : 1; // 0 = Negative, 1 = Positive
    uint8  Z_Transform_Axis : 2; // 0 = X Axis, 1 = Y Axis, 2 = Z Axis
    uint8  Z_Transform_Sign : 1; // 0 = Negative, 1 = Positive
    uint8  Unused_2         : 7; // Unused bits
}LISXDH_SF_PARAM_TYPE;

//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================
void LISxDH__Initialize(LISXDH_SF_PARAM_TYPE * param_ptr);
void LISxDH__Background(void);
void LISxDH__Disable(void);
void LISxDH__Enable(LISXDH_MODE_TYPE mode);
void LISxDH__FlushData(void);
ACCELEROMETER_STATUS_TYPE LISxDH__GetStatus(void);
sint8 LISxDH__GetTemperatureDelta(void);
BOOL_TYPE LISxDH__Read(sint16* vector);

#endif  // LISXDH_H_

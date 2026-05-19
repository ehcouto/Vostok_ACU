/**
 *  @file       
 *
 *  @brief      Public interface for the FXLS8974 module.
 *
 *  @details    Refer to the FXLS8974.c source file for more detailed information.
 *
 *  @copyright  Copyright 2022. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef FXLS8974_H_
#define FXLS8974_H_

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "FXLS8974_prm.h"

// -- Other Modules --
#include "I2c.h"
#include "AccelerometerLLI_Defs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

// -- Public Enumerated Constant Declarations --

//! Accelerometer modes supported by the module
typedef enum FXLS8974_MODE_ENUM
{
    //! Accelerometer is disabled (low power). No accelerometer acquisition is taking place.
    FXLS8974_MODE_DISABLED = 0,

    //! Accelerometer is enabled and sampling points based on user configuration.
    FXLS8974_MODE_NORMAL,

    //! Accelerometer is running self-test on the positive x axis
    FXLS8974_MODE_ST_POSITIVE_X,

    //! Accelerometer is running self-test on the negative x axis
    FXLS8974_MODE_ST_NEGATIVE_X,

    //! Accelerometer is running self-test on the positive y axis
    FXLS8974_MODE_ST_POSITIVE_Y,

    //! Accelerometer is running self-test on the negative y axis
    FXLS8974_MODE_ST_NEGATIVE_Y,

    //! Accelerometer is running self-test on the positive z axis
    FXLS8974_MODE_ST_POSITIVE_Z,

    //! Accelerometer is running self-test on the negative z axis
    FXLS8974_MODE_ST_NEGATIVE_Z,

    //! The number of accelerometer modes.
    FXLS8974_MODE_COUNT
} FXLS8974_MODE_TYPE;


//! Setting file data
PACKED typedef struct
{
    uint16 Reserved;            // Bytes where common LLI parameters reside (not used by this module)
    ACCELEROMETER_DEVICE_TYPE Device;   // Identifies the accelerometer device
    uint8  Unused_0;            // Unused byte
    uint8  Port           : 4;  // I2C peripheral or SPI peripheral
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
    uint8  Data_Rate      : 4;  // 0 = Power Down Mode, 1 = 1.5Hz, 2 = 12.5Hz, 3 = 25Hz, 4 = 50Hz, 5 = 100Hz, 6 = 200Hz, 7 = 400Hz, 8 = 1600Hz, 9 = 3200Hz
    uint8  Unused_1       : 4;  // Unused bits
    uint8  X_Transform_Axis : 2; // 0 = X Axis, 1 = Y Axis, 2 = Z Axis
    uint8  X_Transform_Sign : 1; // 0 = Negative, 1 = Positive
    uint8  Y_Transform_Axis : 2; // 0 = X Axis, 1 = Y Axis, 2 = Z Axis
    uint8  Y_Transform_Sign : 1; // 0 = Negative, 1 = Positive
    uint8  Z_Transform_Axis : 2; // 0 = X Axis, 1 = Y Axis, 2 = Z Axis
    uint8  Z_Transform_Sign : 1; // 0 = Negative, 1 = Positive
    uint8  Unused_2         : 7; // Unused bits
}FXLS8974_SF_PARAM_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void FXLS8974__Initialize(FXLS8974_SF_PARAM_TYPE * param_ptr);
void FXLS8974__Background(void);
void FXLS8974__Disable(void);
void FXLS8974__Enable(FXLS8974_MODE_TYPE mode);
void FXLS8974__FlushData(void);
ACCELEROMETER_STATUS_TYPE FXLS8974__GetStatus(void);
sint8 FXLS8974__GetTemperatureDelta(void);
BOOL_TYPE FXLS8974__Read(sint16* vector);

#endif // FXLS8974_H_

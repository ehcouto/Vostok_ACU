/**
 *  @file       
 *
 *  @brief      Public definitions for the Accelerometer LLI.
 *
 *  @details    These definitions are common for accelerometers from different manufacturers.
 *
 *  @copyright  Copyright 2022. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef ACCELEROMETERLLI_DEFS_H_
#define ACCELEROMETERLLI_DEFS_H_

#include "API018Diagnostic.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//! Possible status value for an Accelerometer LLI
typedef enum ACCELEROMETER_STATUS_ENUM
{
    //! The accelerometer is disabled.
    ACCELEROMETER_DISABLED      = 0,

    //! The accelerometer is initializing.
    ACCELEROMETER_INITIALIZING  = 2,

    //! The Accelerometer module is acquiring data.
    ACCELEROMETER_ACQUIRING     = 3,

    //! The Accelerometer module buffer is full and data has been lost.
    ACCELEROMETER_BUFFER_FULL   = 4,

    //! The Accelerometer module buffer is in an unknown state.
    ACCELEROMETER_UNKNOWN       = 5
} ACCELEROMETER_STATUS_TYPE;

//! Supported accelerometer devices
//! This list must match the list in GESE used by the Accelerometer LLI
typedef enum
{
    ACCELEROMETER_DEVICE_LISxDH   = 0,  // From ST
    ACCELEROMETER_DEVICE_FXLS8974 = 1,  // From NXP
    ACCELEROMETER_DEVICE_UNKNOWN  = 0xFF
}ACCELEROMETER_DEVICE_TYPE;

//! Bytes used to map the 3 accelerometer channel inputs onto X, Y, and Z coordinates.
//! Bit 3 = Inversion flag. 0 = multiply by 1, 1 = multiply by -1.
//! Bits 1,0 are used for an index to the destination XYZ axis.
//!     0 = Transform destination is X.
//!     1 = Transform destination is Y.
//!     2 = Transform destination is Z.
typedef enum ACCELEROMETER_MAP_ENUM
{
    ACCELEROMETER_MAP_POSITIVE_X    = 0x0,
    ACCELEROMETER_MAP_NEGATIVE_X    = 0x8,
    ACCELEROMETER_MAP_POSITIVE_Y    = 0x1,
    ACCELEROMETER_MAP_NEGATIVE_Y    = 0x9,
    ACCELEROMETER_MAP_POSITIVE_Z    = 0x2,
    ACCELEROMETER_MAP_NEGATIVE_Z    = 0xA
} ACCELEROMETER_MAP_TYPE;

//! Determine if the transformation includes a multiplication by -1.
//! Used to extract 3rd bit based on input type of ACCELEROMETER_MAP_TYPE
#define TRANSFORM_MULTIPLIER(t)     (BIT_TEST(t,3) ? -1 : 1)

//! Get the axis index after the transformation.
//! Used to extract first 2 bits based on input type of ACCELEROMETER_MAP_TYPE
#define TRANSFORM_AXIS(t)           ((t) & 3)

//! Macros to declare accelerometer related faults to the ACU
#define ACCELEROMETER_NOT_RESPONDING()   (void)API018Diagnostic__GenerateFault(FAULT_KEY_ACCELEROMETER_NOT_RESPONDING)
#define ACCELEROMETER_OLD_DATA()         (void)API018Diagnostic__GenerateFault(FAULT_KEY_ACCELEROMETER_OLD_DATA)
#define ACCELEROMETER_DATA_OVERFLOW()    (void)API018Diagnostic__GenerateFault(FAULT_KEY_ACCELEROMETER_DATA_OVERFLOW)
#define ACCELEROMETER_DATA_OVERWRITTEN() (void)API018Diagnostic__GenerateFault(FAULT_KEY_ACCELEROMETER_DATA_OVERWRITTEN)

#endif // ACCELEROMETERLLI_DEFS_H_

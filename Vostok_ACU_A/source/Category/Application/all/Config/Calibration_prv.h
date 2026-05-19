/**
 * @file
 * @brief       Private project-specific parameters for the Calibration module.
 *
 * @details     Refer to the C source file for more detailed information.
 *
 * @copyright   Copyright 2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef CALIBRATION_PRV_H_
#define CALIBRATION_PRV_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "Calibration_def.h"

// -- Other Modules --


//=====================================================================================================================
//  --- Private Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Private Constant Declarations --

//! The number of bytes used to store calibration data. (Refer to the Calibration_Data[] definition.)
//! This value must be the sum of the sizes of the individual calibration data sizes specified in
//! the CALIBRATION_CONFIG[] array.
#define CALIBRATION_DATA_BUFFER_LENGTH 1


// -- Private Enumerated Constant Declarations --


// -- Private Type Declarations --


// -- Private Function Prototypes --

// Prototypes of functions that are used to apply calibration data.
// The function prototypes must match the CALIBRATION_FUNCTION_TYPE defined in Calibration_def.h.
static sint32 CalibrateByAddingSint8Offset(sint32 input, void* data);


// -- Private Flash-Based Definition --

//! Calibration configuration information for each of the Calibration Items present on the hardware.
//! This array should only contain entries for items that will be calibrated on the target hardware.
//! Refer to the #CALIBRATION_ITEM_CONFIG_TYPE (in Calibration_def.h) for more information.
static const CALIBRATION_ITEM_CONFIG_TYPE CALIBRATION_CONFIG[] =
{
    //Calibration Function,             Calibration Item,               Calibrated GI,                  Calibration Data Size
    {CalibrateByAddingSint8Offset,      CALIBRATION_OVEN1_RTD1,         HBL_GI_TEMPERATURE_1,           sizeof(sint8)}
};


//=====================================================================================================================
//  --- Private Functions ---------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Calibrate the input value by adding a sint8 offset.
 * @param input = The raw input value (before calibration).
 * @param data = A pointer to the calibration data.
 * @return The calibrated input value.
 */
static sint32 CalibrateByAddingSint8Offset(sint32 input, void* data)
{
    return (input + *((sint8*)data));
}


#endif      // CALIBRATION_PRV_H_

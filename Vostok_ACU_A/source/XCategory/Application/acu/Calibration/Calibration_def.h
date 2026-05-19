/**
 * @file
 * @brief       Private definitions used by parameter files in the Calibration module.
 *
 * @copyright   Copyright 2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef CALIBRATION_DEF_H_
#define CALIBRATION_DEF_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --
#include "Hbl.h"


//=====================================================================================================================
//  --- Private Properties --------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Private Type Declarations --

/**
 * Function prototype for a function that executes a calibration.
 * @param id = The unique identifier for the combination of the GI and the CALIBRATION_METHOD_TYPE.
 * @param input = The measured raw data before the application of the calibration data.
 * @return The calibration data based on the measured raw data and the calibration data.
 */
typedef sint32 (*CALIBRATION_FUNCTION_TYPE)(sint32 input, void* data);

//! A structure for the information used to configure a Calibration Item.
//! A calibration item is a unique combination of a HBL_GENERIC_INPUT_ENUM_TYPE and a calibration method.
typedef struct CALIBRATION_ITEM_CONFIG_STRUCT
{
    //! The function used to apply the calibration data at run-time.
    //! Calibrated_Value = FunctionOf(input_value, calibration_data)
    CALIBRATION_FUNCTION_TYPE Function;

    //! The Calibration Item that is present in the hardware and will be calibrated for this project.
    CALIBRATION_ITEM_ID_TYPE Item;

    //! The HBL Generic Input (GI) that can be calibrated.
    //! If no GI is associated with the calibration item, then use 0xFF.
    HBL_GENERIC_INPUT_ENUM_TYPE Gi;

    //! The number of bytes reserved to store calibration data for the Calibration Item.
    uint8 Data_Size;
} CALIBRATION_ITEM_CONFIG_TYPE;


//=====================================================================================================================
//  --- Private Methods -----------------------------------------------------------------------------------------------
//=====================================================================================================================


#endif      // CALIBRATION_DEF_H_

/**
 * @file
 * @brief       Public category-specific parameters for the Calibration module.
 *
 * @details     This file is should be shared across a category.
 *              Refer to the C source file for more detailed information.
 *
 * @copyright   Copyright 2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef CALIBRATION_PRM_H_
#define CALIBRATION_PRM_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public Constant Declarations --


// -- Public Enumerated Constant Declarations --

//! The list of the calibration items that can be calibrated on this appliance.
//! A calibration item is a unique combination of a HBL_GENERIC_INPUT_ENUM_TYPE and a calibration method.
typedef enum CALIBRATION_ITEM_ID_ENUM
{
    CALIBRATION_NUM_OF_ITEMS
} CALIBRATION_ITEM_ID_TYPE;


// -- Public Type Declarations --


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


#endif      // CALIBRATION_PRM_H_

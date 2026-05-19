/**
 * @file
 * @brief       Public interface to the Equation module.
 *
 * @details     Refer to the Equation.c source file for more detailed information.
 *
 * @copyright   Copyright 2020 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef EQUATION_H_
#define EQUATION_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --
#include "SettingFile.h"


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public Constant Declarations --


// -- Public Enumerated Constant Declarations --


// -- Public Type Declarations --

//! Information for mocking variables while predicting rules.
typedef struct MOCKED_VALUES_STRUCT
{
    //! Pointer to a list of mock variable values used while predicting rules.
    //! The order of values must match the order of indices in the Indices field.
    const sint32* Values;

    //! Pointer to a list of indices for variables mocked while predicting rules.
    //! The order of indices must match the order of values in the Values field.
    const uint8* Indices;

    //! Number of variables mocked while predicting rules.
    const uint8 Num_Of_Values;
} MOCKED_VALUES_TYPE;


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================

BOOL_TYPE Equation__CheckCondition(SETTINGFILE_OFFSET_TYPE pointer, uint16 displacement);
sint32 Equation__Evaluate(SETTINGFILE_OFFSET_TYPE pointer, uint16 displacement);
sint32 Equation__Predict(SETTINGFILE_OFFSET_TYPE pointer, uint16 displacement);
BOOL_TYPE Equation__PredictCondition(SETTINGFILE_OFFSET_TYPE pointer, uint16 displacement);


#endif      // EQUATION_H_

/**
 * @file
 * @brief       Private parameters for the Equation module.
 *
 * @details     Refer to the C source file for more detailed information.
 *
 * @copyright   Copyright 2021 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef EQUATION_PRV_H_
#define EQUATION_PRV_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --
#if (SYSTEM_HMI_HANDLING == ENABLED)
    #include "HmiVariable.h"
    #include "StepData.h"
#endif
#include "Variable.h"


//=====================================================================================================================
//  --- Private Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Private Constant Declarations --


// -- Private Enumerated Constant Declarations --


// -- Private Type Declarations --


// -- Private Flash-Based Constant Definitions --

//! List of indices for all ACU Variables mocked during rule predictions.
static const uint8 ACU_INDICES[] =
{
    VARIABLE_STEP_SECONDS
};

//! List of values for all ACU Variables mocked during rule predictions.
static const sint32 ACU_VALUES[] =
{
    0xFFFF                                          // VARIABLE_STEP_SECONDS
};

//! Information for all ACU Variables mocked during rule predictions.
static const MOCKED_VALUES_TYPE MOCKED_ACU_VAR_VALUES =
{
    ACU_VALUES,
    ACU_INDICES,
    ELEMENTS_IN_ARRAY(ACU_INDICES)
};

#if (SYSTEM_HMI_HANDLING == ENABLED)
    //! List of indices for all HMI Variables mocked during rule predictions.
    static const uint8 HMI_OFFSETS[] =
    {
        HMI_VARIABLE_GENERAL_PURPOSE1,
        HMI_VARIABLE_GENERAL_PURPOSE2
    };

    //! List of values for all HMI Variables mocked during rule predictions.
    static const sint32 HMI_VALUES[] =
    {
        0,                                          // HMI_VARIABLE_GENERAL_PURPOSE1
        0,                                          // HMI_VARIABLE_GENERAL_PURPOSE2
    };

    //! Information for all HMI Variables mocked during rule predictions.
    static const MOCKED_VALUES_TYPE MOCKED_HMI_VAR_VALUES =
    {
        HMI_VALUES,
        HMI_OFFSETS,
        ELEMENTS_IN_ARRAY(HMI_OFFSETS)
    };

    //! List of indices for all STEP Variables mocked during rule predictions.
    static const uint8 STEP_INDICES[] =
    {
        SV_HMI_EVENT,
        SV_ELAPSED_STEP_SECS,
        SV_COUNTDOWN_DONE,
        SV_ELAPSED_METHOD_SECS
    };

    //! List of values for all STEP Variables mocked during rule predictions.
    static const sint32 STEP_VALUES[] =
    {
        HMI_EVENT_USER_ACK,                         // SV_HMI_EVENT,
        0xFFFF,                                     // SV_ELAPSED_STEP_SECS,
        1,                                          // SV_COUNTDOWN_DONE,
        0xFFFF                                      // SV_ELAPSED_METHOD_SECS
    };

    //! Information for all STEP Variables mocked during rule predictions.
    static const MOCKED_VALUES_TYPE MOCKED_STEP_VAR_VALUES =
    {
        STEP_VALUES,
        STEP_INDICES,
        ELEMENTS_IN_ARRAY(STEP_INDICES)
    };
#endif


//=====================================================================================================================
//  --- Private Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Private Macro Declarations --


#endif      // EQUATION_PRV_H_

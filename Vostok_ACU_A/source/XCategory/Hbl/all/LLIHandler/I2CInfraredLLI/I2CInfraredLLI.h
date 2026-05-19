/**
 * @file
 * @brief       Public interface to the I2CInfraredLLI module.
 *
 * @details     Refer to the I2CInfraredLLI.c source file for more detailed information.
 *
 * @copyright   Copyright 2018 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef I2CINFRAREDLLI_H_
#define I2CINFRAREDLLI_H_


#if (HBL_LLI_NUM_I2C_INFRARED_SENSOR > 0)
//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --
#include "HblDefs.h"
#include "Melexis90621Def.h"


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public Constant Declarations --

//! The LLI function register for HBL use.
#define I2C_INFRARED_SENSOR_LLI_REGISTER    \
    I2CInfraredLLI__Initialize,             \
    I2CInfraredLLI__Allocate,               \
    I2CInfraredLLI__InitializeInstance,     \
    I2CInfraredLLI__Get,                    \
    I2CInfraredLLI__AsynchProcess,          \
    NULL,                                   \
    NULL,                                   \
    NULL,                                   \
    sizeof(I2C_IR_LLI_STATUS_TYPE)


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


void I2CInfraredLLI__Initialize(void);
uint8 I2CInfraredLLI__Allocate(void);
PASS_FAIL_TYPE I2CInfraredLLI__InitializeInstance(HBL_LLI_TYPE* input);
void* I2CInfraredLLI__Get(HBL_LLI_TYPE* input);
BOOL_TYPE I2CInfraredLLI__AsynchProcess(HBL_LLI_TYPE* input);


#endif      // #if (HBL_LLI_NUM_I2C_INFRARED_SENSOR > 0)
#endif      // I2CINFRAREDLLI_H_

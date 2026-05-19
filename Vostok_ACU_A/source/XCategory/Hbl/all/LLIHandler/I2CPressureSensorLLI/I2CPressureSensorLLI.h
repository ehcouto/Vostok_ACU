/**
 *  @file       
 *
 *  @brief      Public interface for the I2CPressureSensorLLI module.
 *
 *  @copyright  Copyright 2017-$Date: 2017/02/17 09:20:30EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef I2CPRESSURESENSOR_LLI_H_
#define I2CPRESSURESENSOR_LLI_H_

#ifndef HBL_LLI_NUM_I2C_PRESSURE_SENSOR
    #define HBL_LLI_NUM_I2C_PRESSURE_SENSOR 0
#endif

#if (HBL_LLI_NUM_I2C_PRESSURE_SENSOR > 0)

#include "HblDefs.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


#define I2C_PRESSURE_SENSOR_LLI_REGISTER    I2CPressureSensorLLI__Initialize,                 \
                                            I2CPressureSensorLLI__Allocate,                   \
                                            I2CPressureSensorLLI__InitializeInstance,         \
                                            I2CPressureSensorLLI__Get,                        \
                                            I2CPressureSensorLLI__AsynchProcess,              \
                                            NULL,                                             \
                                            NULL,                                             \
                                            NULL,                                             \
											4

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void I2CPressureSensorLLI__Initialize(void);
uint8 I2CPressureSensorLLI__Allocate(void);
PASS_FAIL_TYPE I2CPressureSensorLLI__InitializeInstance(HBL_LLI_TYPE * input);
void * I2CPressureSensorLLI__Get(HBL_LLI_TYPE * input);
BOOL_TYPE I2CPressureSensorLLI__AsynchProcess(HBL_LLI_TYPE * input);

#endif //(HBL_GI_I2CPRESSURESENSOR > 0)

#endif // I2CPRESSURESENSOR_LLI_H_

/**
 *  @file       
 *
 *  @brief      Public interface for the Safety Relevant I2CPressureSensorLLI module.
 *
 *  @defgroup   CLASS_B
 *
 *  @copyright  Copyright 2017-$Date: 2017/02/17 09:20:30EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRI2CPRESSURESENSOR_LLI_H_
#define SRI2CPRESSURESENSOR_LLI_H_

#include "SRSystemConfig.h"

#ifndef HBL_LLI_NUM_SRI2C_PRESSURE_SENSOR
    #define HBL_LLI_NUM_SRI2C_PRESSURE_SENSOR 0U
#endif

#if (HBL_LLI_NUM_SRI2C_PRESSURE_SENSOR > 0U)

#include "HblDefs.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


#define SRI2C_PRESSURE_SENSOR_LLI_REGISTER  SRI2CPressureSensorLLI__Initialize,                 \
                                            SRI2CPressureSensorLLI__Allocate,                   \
                                            SRI2CPressureSensorLLI__InitializeInstance,         \
                                            SRI2CPressureSensorLLI__Get,                        \
                                            SRI2CPressureSensorLLI__AsynchProcess,              \
                                            NULL,                                             \
                                            NULL,                                             \
                                            NULL,                                             \
                                            4

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SRI2CPressureSensorLLI__Initialize(void);
uint8 SRI2CPressureSensorLLI__Allocate(void);
void SRI2CPressureSensorLLI__Handler(void);
PASS_FAIL_TYPE SRI2CPressureSensorLLI__InitializeInstance(HBL_LLI_TYPE * input);
void * SRI2CPressureSensorLLI__Get(HBL_LLI_TYPE * input);
BOOL_TYPE SRI2CPressureSensorLLI__AsynchProcess(HBL_LLI_TYPE * input);

#endif //(HBL_LLI_NUM_SRI2C_PRESSURE_SENSOR > 0)

#endif // SRI2CPRESSURESENSOR_LLI_H_

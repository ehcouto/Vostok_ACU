/**
 *  @file       I2CRHTSensorLLI.h
 *
 *  @brief      Public interface for the I2CRHTSensorLLI module.
 *
 *  LLI ID -    HBL_LLI_I2C_RHT_HUMIDITY_SENSOR = 30
 *              HBL_LLI_I2C_RHT_TEMEPERATURE_SENSOR = 31
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef I2CRHTSENSOR_LLI_H_
#define I2CRHTSENSOR_LLI_H_

#ifndef HBL_LLI_NUM_I2C_RHT_HUMIDITY_SENSOR
    #define HBL_LLI_NUM_I2C_RHT_HUMIDITY_SENSOR  1
#endif

#ifndef HBL_LLI_NUM_I2C_RHT_TEMPERATURE_SENSOR
    #define HBL_LLI_NUM_I2C_RHT_TEMPERATURE_SENSOR  1
#endif

#if (HBL_LLI_NUM_I2C_RHT_HUMIDITY_SENSOR > 0) || (HBL_LLI_NUM_I2C_RHT_TEMPERATURE_SENSOR > 0)

#include "Hbl.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#if (HBL_LLI_NUM_I2C_RHT_HUMIDITY_SENSOR > 0)
    //This register returns the Relative Humidity value in %
    #define I2C_RHT_HUMIDITY_SENSOR_LLI_REGISTER         I2CRHTSensorLLI__Initialize,                   \
                                                         I2CRHTSensorLLI__AllocateHumidity,                     \
                                                         I2CRHTSensorLLI__InitializeHumidityInstance,   \
                                                         I2CRHTSensorLLI__GetHumidity,                  \
                                                         I2CRHTSensorLLI__AsynchProcess,                \
                                                         NULL,                                          \
                                                         NULL,                                          \
                                                         I2CRHTSensorLLI__OneMsProcess,                 \
                                                         sizeof(uint16)

#endif

#if (HBL_LLI_NUM_I2C_RHT_TEMPERATURE_SENSOR > 0)
    //This register returns the ambient temperature in degree celsius
    #define I2C_RHT_TEMPERATURE_SENSOR_LLI_REGISTER      I2CRHTSensorLLI__Initialize,                   \
                                                         I2CRHTSensorLLI__AllocateTemperature,                     \
                                                         I2CRHTSensorLLI__InitializeTemperatureInstance,\
                                                         I2CRHTSensorLLI__GetTemperature,               \
                                                         I2CRHTSensorLLI__AsynchProcess,                \
                                                         NULL,                                          \
                                                         NULL,                                          \
                                                         I2CRHTSensorLLI__OneMsProcess,			        \
                                                         sizeof(sint16)
#endif

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void I2CRHTSensorLLI__Initialize(void);
BOOL_TYPE I2CRHTSensorLLI__AsynchProcess(HBL_LLI_TYPE * input);
BOOL_TYPE I2CRHTSensorLLI__OneMsProcess(HBL_LLI_TYPE * input);

#if (HBL_LLI_NUM_I2C_RHT_HUMIDITY_SENSOR > 0)
    uint8 I2CRHTSensorLLI__AllocateHumidity(void);
    PASS_FAIL_TYPE I2CRHTSensorLLI__InitializeHumidityInstance(HBL_LLI_TYPE * input);
    void * I2CRHTSensorLLI__GetHumidity(HBL_LLI_TYPE * input);
#endif

#if (HBL_LLI_NUM_I2C_RHT_TEMPERATURE_SENSOR > 0)
    uint8 I2CRHTSensorLLI__AllocateTemperature(void);
    PASS_FAIL_TYPE I2CRHTSensorLLI__InitializeTemperatureInstance(HBL_LLI_TYPE * input);
    void * I2CRHTSensorLLI__GetTemperature(HBL_LLI_TYPE * input);
#endif

#endif //(HBL_LLI_NUM_I2C_RHT_HUMIDITY_SENSOR > 0)

#endif // I2CRHTSENSOR_LLI_H_

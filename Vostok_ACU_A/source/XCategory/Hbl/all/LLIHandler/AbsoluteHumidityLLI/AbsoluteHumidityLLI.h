/**
 *  @file       
 *
 *  @brief      Public interface for the AbsoluteHumidityLLI.
 *
 *  @copyright  Copyright 2018 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef ABSOLUTEHUMIDITYLLI_H_
#define ABSOLUTEHUMIDITYLLI_H_

#ifndef HBL_LLI_NUM_ABSOLUTEHUMIDITY
    #define HBL_LLI_NUM_ABSOLUTEHUMIDITY  0
#endif

#if (HBL_LLI_NUM_ABSOLUTEHUMIDITY > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//! Absolute humidity LLI state enumeration type definition.
typedef enum ABSOLUTE_HUMIDITY_LLI_GET_STATE_ENUM
{
    ABSOLUTE_HUMIDITY_LLI_STATE_OFF        = 0x00,  // In this state, sensor is not working, no power is consumed.
    ABSOLUTE_HUMIDITY_LLI_STATE_HEATING    = 0x01,  // In this state, heat the sensor to 150C before starting a new measurement.
    ABSOLUTE_HUMIDITY_LLI_STATE_BALANCING  = 0x02,  // In this state, execute successive approximation to get the measuring bridge approximated balance.
    ABSOLUTE_HUMIDITY_LLI_STATE_REGULATING = 0x03,  // In this state, execute fine turning to get the measuring bridge balance.
    ABSOLUTE_HUMIDITY_LLI_STATE_BALANCED   = 0x04,  // In this state, the measuring bridge is balanced, the PWM value can be used for humidity task or cycle.
    ABSOLUTE_HUMIDITY_LLI_STATE_ERROR      = 0x05   // In this state, the PWM value is out of range, failed to get the measuring bridge balance.
} ABSOLUTE_HUMIDITY_LLI_GET_STATE_TYPE;

//! Absolute humidity LLI get data type definition.
typedef PACKED struct ABSOLUTE_HUMIDITY_LLI_GET_DATA_STRUCT
{
    ABSOLUTE_HUMIDITY_LLI_GET_STATE_TYPE State;
    uint16 Value;
} ABSOLUTE_HUMIDITY_LLI_GET_DATA_TYPE;

#define ABSOLUTEHUMIDITY_LLI_REGISTER       AbsoluteHumidityLLI__Initialize,         	\
                                            AbsoluteHumidityLLI__Allocate,           	\
                                            AbsoluteHumidityLLI__InitializeInstance, 	\
                                            AbsoluteHumidityLLI__Get,            		\
                                            AbsoluteHumidityLLI__AsynchProcess,         \
                                            AbsoluteHumidityLLI__PeakProcess,    		\
                                            NULL,                                       \
                                            NULL,                                       \
                                            sizeof(ABSOLUTE_HUMIDITY_LLI_GET_DATA_TYPE)

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void AbsoluteHumidityLLI__Initialize(void);
uint8 AbsoluteHumidityLLI__Allocate(void);
PASS_FAIL_TYPE AbsoluteHumidityLLI__InitializeInstance(HBL_LLI_TYPE * input);
void * AbsoluteHumidityLLI__Get(HBL_LLI_TYPE * input);
BOOL_TYPE AbsoluteHumidityLLI__AsynchProcess(HBL_LLI_TYPE * input);
BOOL_TYPE AbsoluteHumidityLLI__PeakProcess(HBL_LLI_TYPE * input,BOOL_TYPE peak_polarity);
#endif
#endif // ABSOLUTEHUMIDITYLLI_H_



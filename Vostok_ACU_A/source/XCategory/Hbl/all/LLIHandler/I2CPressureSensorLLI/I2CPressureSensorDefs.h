/**
 *  @file       I2CPressureSensorDefs.h
 *
 *  @brief      External API Declaration for the PressureSensorDefs Module
 *
 *  @copyright  Copyright 2018-2019 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef I2CPRESURESENSORDEFS_H_
#define I2CPRESURESENSORDEFS_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//! Unique pressure value returned while the sensor is still stabilizing
#define I2CPRESSURESENSOR_STABILIZING  0xFFFF

//! Unique pressure value returned while there is an I2C communication failure active
#define I2CPRESSURESENSOR_I2C_FAILURE  0xFFFE

#endif // I2CPRESURESENSORDEFS_H_

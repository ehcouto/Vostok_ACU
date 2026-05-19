/**
 *  @file       
 *
 *  @brief      External API Declaration for the PresureSensorDefs Module
 *
 *  @defgroup   CLASS_B
 *
 *  @copyright  Copyright 2018-2019 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRI2CPRESURESENSORDEFS_H_
#define SRI2CPRESURESENSORDEFS_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//! Unique pressure value returned while the sensor is still stabilizing
#define SRI2CPRESSURESENSOR_STABILIZING  0xFFFF

//! Unique pressure value returned while there is an I2C communication failure active
#define SRI2CPRESSURESENSOR_I2C_FAILURE  0xFFFE

#endif // SRI2CPRESURESENSORDEFS_H_

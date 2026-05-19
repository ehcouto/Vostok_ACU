/**
 *  @file       
 *
 *  @brief      Private parameters for the I2CPressureSensorLLI module
 *
 *  @copyright  Copyright 2016-2017. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef I2CPRESSURESENSORGI_PRV_H_
#define I2CPRESSURESENSORGI_PRV_H_

//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief I2c pressure sensor device address
 */
#define PRESSURESENSOR_I2C_ADDRESS  				//0x28

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief I2c Bus used by the device
 *  @note  The I2C_x must be ENABLED at I2c_prm.h file.
 */
#define PRESSURESENSOR_I2C_BUS      				//I2C0

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief I2c clock speed used by the device
 */
#define PRESSURESENSOR_I2C_SPEED    				//I2C_100KHZ

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief It enables the Temperature reading
 *  @details Set as ENABLED or DISABLED
 */
#define PRESSURESENSOR_I2C_TEMPERATURE_READING    	//DISABLED


#endif // I2CPRESSURESENSORGI_PRV_H_

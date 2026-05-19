/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef VOLTAGERANGE_PRV_H_
#define VOLTAGERANGE_PRV_H_

#include "DishAuxVar.h"
//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================

// Default parameters if setting file ones are missing
// NULL means disable VoltageRange feature, assuming that it's always in VOLTAGERANGE_STATE_GOODRANGE
// To set default parameters you should use an unit8 array (you can define in this file as const uint8)
// that contains parameters as placed in dedicated setting file area
// Refer to Voltage Range requirements to create that array
#define VOLTAGERANGE_DEFAULT_PARAM_PTR		NULL

// Execution time in ms of VoltageRange monitor
// It should be aligned with MAIN_APPMGR_THREAD_INTERVAL defined in SRMain_prv.h
// Default value is 100
#define VOLTAGERANGE_EXEC_TIME_MS			100


// Function to get Voltage reference value
#define VOLTAGERANGE_GETREFERENCE()			(DishAuxVar__GetACLine())

#endif // VOLTAGERANGE_PRV_H_

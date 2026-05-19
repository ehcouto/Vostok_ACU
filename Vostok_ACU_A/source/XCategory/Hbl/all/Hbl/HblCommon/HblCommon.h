/**
 *  @file       
 *
 *  @brief      Contains the Common functions the HBL can offer to be used by other modules such as -
 *              - Converting HBL_IO_DEVICE_DATA_CONVERT_TABLE_HEADER type data
 *
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef HBLCOMMON_H
	#define HBLCOMMON_H

#include "C_Extensions.h"
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
sint32 HblCommon__ConvertIODeviceData(uint16 input_data, HBL_IO_DEVICE_DATA_CONVERT_TABLE_HEADER * table_ptr);
uint16 HblCommon__GetIODeviceDataSize(HBL_IO_DEVICE_DATA_CONVERT_TABLE_HEADER * table_ptr);
#endif // HBLCOMMON_H



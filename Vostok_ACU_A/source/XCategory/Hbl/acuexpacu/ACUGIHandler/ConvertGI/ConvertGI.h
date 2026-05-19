/**
 *  @file
 *
 *  @brief This is an cross category GI module shall be used only if the conversion table
 *          follows the HBL_IO_DEVICE_DATA_CONVERT_TABLE_HEADER data generation format from GESE
 *
 *
 *  @details This module Converts Input LLI data (provided by ADC conversion or other source of LLI data) to a value that represents
 *              a specific scale of values such as temperature, pressure, etc. by using the Conversion table defined in the GESE tool.
 *
 *              This method uses the Exact Match, Interpolation, Ceiling or Floor conversion method as specified in the tool
 *
 *
 *
 *              Note 1 : The unused bit 14 is used to extend the search methods.
 *
 *             Unused field could be filled with 0 or 1 value. If the values are filled with 1 then ignore the unused filed if unused bit 15 filled with 0 then
 *             unused bit 14 can be used for the search method
 *
 *             ----------------------------------------------------------------
 *             |     Bit 15    |     Bit 14    |    Bit 13     |               |
 *             |---------------|---------------|---------------| Search Method |
 *             | Unused Bit 15 | Unused Bit 15 | Interpolation |               |
 *             |---------------|---------------|---------------|---------------|
 *             |       1       |       X       |      0        | Ceiling       |
 *             |---------------|---------------|---------------|---------------|
 *             |       1       |       X       |      1        | Interpolation |
 *             |---------------|---------------|---------------|---------------|
 *             |       0       |       1       |      0        | Exact match   |
 *             |---------------|---------------|---------------|---------------|
 *             |       0       |       1       |      1        | Floor         |
 *              ----------------------------------------------------------------
 *
 *
 *
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/24 11:35:19EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef CONVERTGI_H
	#define CONVERTGI_H
#include "C_Extensions.h"
#include "SystemConfig.h"


#ifndef HBL_GI_NUM_CONVERT
    #define HBL_GI_NUM_CONVERT 0
#endif

#if (HBL_GI_NUM_CONVERT > 0)
#include "HblCommon.h"


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


#define CONVERT_GI_REGISTER                 ConvertGI__Initialize,           \
                                            ConvertGI__Allocate,             \
                                            ConvertGI__InitializeInstance,   \
                                            ConvertGI__Get,                  \
                                            ConvertGI__AsynchProcess,        \
                                            ConvertGI__GetConverted,         \
                                            sizeof(sint32)



//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void ConvertGI__Initialize(void);
uint8 ConvertGI__Allocate(void);
PASS_FAIL_TYPE ConvertGI__InitializeInstance(HBL_GI_TYPE * generic_input, uint8 lli_sequence);
void * ConvertGI__Get(HBL_GI_TYPE * generic_input,void * lli_data);
BOOL_TYPE ConvertGI__AsynchProcess(HBL_GI_TYPE * generic_input,void * lli_data, uint8 lli_sequence);
sint32 ConvertGI__GetConverted(HBL_GI_TYPE * generic_input,void * lli_data);

#endif

#endif // CONVERTGI_H



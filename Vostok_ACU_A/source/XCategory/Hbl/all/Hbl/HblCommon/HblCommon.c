/**
 *  @file       
 *
 *  @brief      Implementation of the Hbl Common API interfaces
 *
 *  @details    Detailed description of the file contents
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


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "HblCommon.h" 
#include "Convert.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
static const DATATYPE_TYPE DATA_TYPE_MAP[]          = {DATATYPE_UINT8 ,DATATYPE_UINT16 ,DATATYPE_SINT8 ,DATATYPE_SINT16 };
static const uint8 IS_DATA_TYPE_WORD_MAP[]          = {0              ,1               ,0              ,1               };  //this const shall match the DATA_TYPE_MAP. If the type is word then write it as 1 or else 0

static const CONVERT_SEARCH_TYPE SEARCH_METHOD_MAP[] = {CONVERT_CEILING     ,CONVERT_INTERPOLATION_1D   ,CONVERT_EXACT_MATCH    ,CONVERT_FLOOR  };

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//------------------------------------------------------------------------------------------------
/**
 * @brief   This API is an public function can be used by other modules to performs the conversion
 *          of HBL_IO_DEVICE_DATA_CONVERT_TABLE_HEADER
 *
 * @details This does the conversion of table only of CONVERTGI_HBL_IO_DEVICE_DATA_CONVERT_TABLE_TYPE
 *
 *          The unused bit 14 is used to extend the search methods.
 *          Unused field could be filled with 0 or 1 value. If the values are filled with 1 then ignore the unused filed if unused bit 15 filled with 0 then
 *          unused bit 14 can be used for the search method
 *
 *              ---------------------------------------------------------------
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
 * @param input_data
 * @param table_ptr - Shall be of HBL_IO_DEVICE_DATA_CONVERT_TABLE_HEADER
 * @return
 */
sint32 HblCommon__ConvertIODeviceData(uint16 input_data, HBL_IO_DEVICE_DATA_CONVERT_TABLE_HEADER * table_ptr)
{
    //Input parameters validation code is intentionally not added

    CONVERT_TABLE_TYPE convert_table;

    convert_table.input_type           = DATA_TYPE_MAP[table_ptr->S1];
    convert_table.output_type          = DATA_TYPE_MAP[table_ptr->S2];
    convert_table.table_size           = table_ptr->Point_Number;


    #define INPUT_DATA_TABLE_LOCATION   ((void *)((uint8 *)table_ptr + sizeof(HBL_IO_DEVICE_DATA_CONVERT_TABLE_HEADER)))
    convert_table.input_table_1        = INPUT_DATA_TABLE_LOCATION;

    //Output table location is relative to the input table type
    #define OUTPUT_DATA_TABLE_LOCATION  ((void *)   ((uint8 *)INPUT_DATA_TABLE_LOCATION + (table_ptr->Point_Number << IS_DATA_TYPE_WORD_MAP[table_ptr->S1])) )
    convert_table.output_table         = OUTPUT_DATA_TABLE_LOCATION;

    uint8 search_method_index = table_ptr->Interpolation;

    //This code is added to extend the search method
    if (table_ptr->Unused == 1)
    {
        search_method_index += 2;
    }

    convert_table.search_method = SEARCH_METHOD_MAP[search_method_index];

    return((sint32)Convert__Process(input_data, &convert_table));
}

//------------------------------------------------------------------------------------------------
/**
 * @brief   This function returns data size of the entire HBL_IO_DEVICE_DATA_CONVERT_TABLE_HEADER structure + data content
 *
 * @param table_ptr - Shall be of HBL_IO_DEVICE_DATA_CONVERT_TABLE_HEADER
 * @return  Data size of the structure requested
 */
uint16 HblCommon__GetIODeviceDataSize(HBL_IO_DEVICE_DATA_CONVERT_TABLE_HEADER * table_ptr)
{
    uint16 size = 0;

    if (table_ptr != NULL)
    {
        size = 2 * table_ptr->Point_Number;
        size += 2;
        if (IS_DATA_TYPE_WORD_MAP[table_ptr->S1] != 0)
        {
            size += table_ptr->Point_Number;
        }
        if (IS_DATA_TYPE_WORD_MAP[table_ptr->S2] != 0)
        {
            size += table_ptr->Point_Number;
        }
    }

    return(size);
}
//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================



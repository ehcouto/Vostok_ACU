/**
 *  @file
 *
 *  @brief      Standard interface for Convert module
 *  @details    This specification defines the requirements of the Convert module which shall be
 *              used for converting RAW data (provided by ADC conversion or other source of RAW data) to a value that represents a specific scale of values
 *              such as temperature, pressure, etc.
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: Convert.h
 *
 *  @copyright  Copyright 2013-$Date: 2015/07/15 08:35:49EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef CONVERT_H_
#define CONVERT_H_

#include "C_Extensions.h"
#include "Convert_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef enum
{
    CONVERT_EXACT_MATCH = 0,
    CONVERT_CEILING,
    CONVERT_FLOOR,
    CONVERT_INTERPOLATION_1D,
    CONVERT_INTERPOLATION_2D,
    CONVERT_BIT_MATCH
}CONVERT_SEARCH_TYPE;

/**
 * @brief This are the search status returned when an search is performed
 */
typedef enum
{
    CONVERT_STATUS_PASS = 0,            //!< CONVERT_STATUS_PASS : If the conversion is successful
    CONVERT_STATUS_PARAM_ERROR,         //!< CONVERT_STATUS_NO_MATCH : This error will set if their is a data mismatch in the CONVERT_TABLE_TYPE or converted_data_ptr is NULL type
    CONVERT_STATUS_UNDER_RANGE,         //!< CONVERT_STATUS_UNDER_RANGE : Applies only for Ceiling, Floor and Interpolation search method. If the value of the input_data is under the minimum value of the INPUT_TABLE.
    CONVERT_STATUS_OVER_RANGE,          //!< CONVERT_STATUS_OVER_RANGE: Applies only for Ceiling, Floor and Interpolation search method. If the value of the input_data is over the maximum value of the INPUT_TABLE.
    CONVERT_STATUS_NO_MATCH_FOUND,      //!< CONVERT_STATUS_NO_MATCH: Applies only for exact search method. If the value of the input_data does not match the any value of the INPUT_TABLE.
}CONVERT_STATUS_TYPE;


/**
 * @brief Table type used to define external tables
 */
PACKED typedef struct
{
    void *input_table_1;
    void *input_table_2;
    void *output_table;
    DATATYPE_TYPE input_type;
    DATATYPE_TYPE output_type;
    CONVERT_SIZE_TYPE table_size;
    CONVERT_SEARCH_TYPE search_method;
}CONVERT_TABLE_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void Convert__Initialize(void);
sint32 Convert__Process(uint16 input_data, CONVERT_TABLE_TYPE * table_ptr);     //This API is deprecated. Shall use Convert__GetValue() API instead
CONVERT_STATUS_TYPE Convert__GetValue(sint32 input_data, CONVERT_TABLE_TYPE * table_ptr, sint32 * converted_data_ptr);

#endif // CONVERT_H_

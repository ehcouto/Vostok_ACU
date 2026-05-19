/**
 *  @file
 *
 *  @brief      Standard interface for Convert module
 *  @details    This specification defines the requirements of the Convert module which shall be
 *              used for converting Input data (provided by ADC conversion or other source of RAW data) to a value that represents a specific scale of values
 *              such as temperature, pressure, etc.
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: Convert.c
 *
 *  @copyright  Copyright 2013-$Date: 2015/07/20 19:38:35EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "Convert.h"
#include "String.h"

#include <limits.h>

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define CONVERT_INVALID_INDEX               ((CONVERT_SIZE_TYPE)0xFFFFFFFF)

const static uint8 DATA_TYPE_SIZE[] =
                                                    {
                                                        0,      //DATATYPE_INVALID
                                                        1,      //DATATYPE_UINT8,
                                                        2,      //DATATYPE_UINT16,
                                                        4,      //DATATYPE_UINT32,
                                                        8,      //DATATYPE_UINT64,
                                                        1,      //DATATYPE_SINT8,
                                                        2,      //DATATYPE_SINT16,
                                                        4,      //DATATYPE_SINT32,
                                                        8,      //DATATYPE_SINT64,
                                                        4,      //DATATYPE_FLOAT32,
                                                        8,      //DATATYPE_FLOAT64,
                                                        0,      //DATATYPE_IS_ARRAY
                                                        0,      //DATATYPE_IS_REGULATION
                                                        2,      //DATATYPE_IS_Q15_CELSIUS
                                                    };

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static sint16 ConvertSignExtension(void * table_ptr, CONVERT_SIZE_TYPE index, uint8 data_size, uint16 mask);
static void SearchTableUnsigned(void * table_ptr, CONVERT_SIZE_TYPE table_size, uint16 input_data, uint8 input_data_size, uint16 input_mask, CONVERT_SIZE_TYPE *minimum_index, CONVERT_SIZE_TYPE *maximum_index);
static void SearchTableSigned(void * table_ptr, CONVERT_SIZE_TYPE table_size, sint16 input_data, uint8 input_data_size, uint16 input_mask, CONVERT_SIZE_TYPE *minimum_index, CONVERT_SIZE_TYPE *maximum_index);
static uint16 ExtractData(uint8 *table_ptr, CONVERT_SIZE_TYPE index, uint8 data_size, uint16 mask);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the Convert module and its variable
 *
 */
void Convert__Initialize(void)
{

}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief    This method performs the conversion of raw data using the reference table
 * @details  It uses the successive approximation and interpolate method to find the converted data
 *
 * @param input_data - The data to be converted
 * @param table_ptr  - A reference of CONVERT_TABLE_TYPE used in the application
 *
 * @return
 *      @retval - The converted return value
 *
 * @image html "Convert__ConvertDataByRef.jpg"
 */
sint32 Convert__Process(uint16 input_data, CONVERT_TABLE_TYPE * table_ptr)
{
    sint32 converted_data;
    Convert__GetValue(input_data, table_ptr, &converted_data);

    return(converted_data);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief    This method performs the conversion of raw data using the reference table
 * @details  It uses the successive approximation and interpolate method to find the converted data
 *
 * @param input_data            : The data to be converted
 * @param table_ptr             : A reference of CONVERT_TABLE_TYPE used in the application
 * @param converted_data_ptr    : A reference where converted data is stores
 *
 * @return : returns the CONVERT_STATUS_TYPE
 *
 * @image html "Convert__ConvertDataByRef.jpg"
 */
CONVERT_STATUS_TYPE Convert__GetValue(sint32 input_data, CONVERT_TABLE_TYPE * table_ptr, sint32 * converted_data_ptr)
{
    CONVERT_STATUS_TYPE convert_status = CONVERT_STATUS_PARAM_ERROR;
    BOOL_TYPE check_under_over_range = FALSE;

    uint16 uint_input_data;
    uint16 uint_input_minimum_value = USHRT_MAX;
    uint16 uint_input_maximum_value = 0;

    sint16 sint_input_data;
    sint16 sint_input_minimum_value = SHRT_MAX;
    sint16 sint_input_maximum_value = SHRT_MIN;

    uint8 input_data_size;
    uint8 output_data_size;
    sint32 output_table_data;

    uint16 input_mask;
    uint16 output_mask;

    CONVERT_SIZE_TYPE minimum_index;
    CONVERT_SIZE_TYPE maximum_index;

    sint32 output_data_minimum;
    sint32 output_data_maximum;

    if ((table_ptr->input_table_1 != NULL) &&
        (table_ptr->output_table != NULL) &&
        (converted_data_ptr != NULL))
    {
        //return the input data as a converted value if the conversion fails
        output_table_data   = input_data;

        input_data_size     = DATA_TYPE_SIZE[table_ptr->input_type];
        output_data_size    = DATA_TYPE_SIZE[table_ptr->output_type];

        uint_input_data     = (uint16)input_data;
        sint_input_data     = (sint16)input_data;

        input_mask          = (uint16)(0xFFFFFFFF >> ((4 - input_data_size) * 8));
        output_mask         = (uint16)(0xFFFFFFFF >> ((4 - output_data_size) * 8));

        //Process Input table data based on the type
        if (table_ptr->input_type > DATATYPE_UINT64)
        {
            SearchTableSigned(table_ptr->input_table_1, table_ptr->table_size, sint_input_data, input_data_size, input_mask, &minimum_index, &maximum_index);

            sint_input_minimum_value    =   ConvertSignExtension(table_ptr->input_table_1, minimum_index, input_data_size, input_mask);
            sint_input_maximum_value    =   ConvertSignExtension(table_ptr->input_table_1, maximum_index, input_data_size, input_mask);
        }
        else
        {
            SearchTableUnsigned(table_ptr->input_table_1, table_ptr->table_size, uint_input_data, input_data_size, input_mask, &minimum_index, &maximum_index);

            uint_input_minimum_value    =   (uint16)ExtractData((uint8 *)table_ptr->input_table_1, minimum_index, input_data_size,  input_mask);
            uint_input_maximum_value    =   (uint16)ExtractData((uint8 *)table_ptr->input_table_1, maximum_index, input_data_size,  input_mask);
        }

        //Process the output table data based on the type
        if (table_ptr->output_type > DATATYPE_UINT64)
        {
            output_data_minimum         =   ConvertSignExtension(table_ptr->output_table, minimum_index, output_data_size, output_mask);
            output_data_maximum         =   ConvertSignExtension(table_ptr->output_table, maximum_index, output_data_size, output_mask);
        }
        else
        {
            output_data_minimum         =   ExtractData((uint8 *)table_ptr->output_table,  minimum_index, output_data_size, output_mask);
            output_data_maximum         =   ExtractData((uint8 *)table_ptr->output_table,  maximum_index, output_data_size, output_mask);
        }

		#define INPUT_DATA \
			( \
				( table_ptr->input_type > DATATYPE_UINT64 ) ? \
				sint_input_data : \
				uint_input_data \
			)

		#define INPUT_MINIMUM_VALUE \
			( \
				( table_ptr->input_type > DATATYPE_UINT64 ) ? \
				sint_input_minimum_value : \
				uint_input_minimum_value \
			)

		#define INPUT_MAXIMUM_VALUE \
			( \
				( table_ptr->input_type > DATATYPE_UINT64 ) ? \
				sint_input_maximum_value : \
				uint_input_maximum_value \
			)


        convert_status = CONVERT_STATUS_PASS;

        switch(table_ptr->search_method)
        {
            case CONVERT_EXACT_MATCH :
                check_under_over_range = FALSE;

                //If (input_data == Xn), then Modified Value = Yn
                if (table_ptr->input_type > DATATYPE_UINT64)
                {
                    if (sint_input_data == sint_input_minimum_value)
                    {
                        output_table_data = (sint32)output_data_minimum;
                    }
                    else if (sint_input_data == sint_input_maximum_value)
                    {
                        output_table_data = (sint32)output_data_maximum;
                    }
                    else
                    {
                        convert_status = CONVERT_STATUS_NO_MATCH_FOUND;
                    }
                }
                else
                {
                    //Exact match
                    if (uint_input_data == uint_input_minimum_value)
                    {
                        output_table_data = (sint32)output_data_minimum;
                    }
                    else if (uint_input_data == uint_input_maximum_value)
                    {
                        output_table_data = (sint32)output_data_maximum;
                    }
                    else
                    {
                        convert_status = CONVERT_STATUS_NO_MATCH_FOUND;
                    }
                }
                break;

            case CONVERT_BIT_MATCH :
                //Bit Match shall send the corresponding Output_Table_Data as Converted_Data only when all the '1' bits in the Input_Table successfully match with the '1' bits of the Input_Data
                {
                    check_under_over_range = FALSE;
                    convert_status = CONVERT_STATUS_NO_MATCH_FOUND;
                    CONVERT_SIZE_TYPE current_index = 0;

                    if (table_ptr->input_type > DATATYPE_UINT64)
                    {
                        while(current_index < table_ptr->table_size)
                        {
                            sint16 sint_extract_data = ConvertSignExtension(table_ptr->input_table_1, current_index, input_data_size, input_mask);

                            //Skip the comparison if the table value is 0
                            if (sint_extract_data != 0)
                            {
                                if ((sint_extract_data & input_data) == sint_extract_data)
                                {
                                    //Process the output table data based on the type
                                    if (table_ptr->output_type > DATATYPE_UINT64)
                                    {
                                        output_table_data         =   ConvertSignExtension(table_ptr->output_table, current_index, output_data_size, output_mask);
                                    }
                                    else
                                    {
                                        output_table_data         =   ExtractData((uint8 *)table_ptr->output_table,  current_index, output_data_size, output_mask);
                                    }

                                    convert_status = CONVERT_STATUS_PASS;
                                    break;
                                }
                            }
                            current_index++;
                        }
                    }
                    else
                    {
                        while(current_index < table_ptr->table_size)
                        {
                            uint16 uint_extract_data = ExtractData((uint8 *)table_ptr->input_table_1, current_index, input_data_size, input_mask);

                            //Skip the comparison if the table value is 0
                            if (uint_extract_data != 0)
                            {
                                if ((uint_extract_data & input_data) == uint_extract_data)
                                {
                                    //Process the output table data based on the type
                                    if (table_ptr->output_type > DATATYPE_UINT64)
                                    {
                                        output_table_data         =   ConvertSignExtension(table_ptr->output_table, current_index, output_data_size, output_mask);
                                    }
                                    else
                                    {
                                        output_table_data         =   ExtractData((uint8 *)table_ptr->output_table,  current_index, output_data_size, output_mask);
                                    }

                                    convert_status = CONVERT_STATUS_PASS;
                                    break;
                                }
                            }

                            current_index++;
                        }
                    }
                }
                break;

            case CONVERT_CEILING :
                check_under_over_range = TRUE;

                output_table_data = INPUT_DATA <= INPUT_MINIMUM_VALUE ? output_data_minimum : output_data_maximum;
                break;

            case CONVERT_FLOOR :
                check_under_over_range = TRUE;

                output_table_data = INPUT_DATA >= INPUT_MAXIMUM_VALUE ? output_data_maximum : output_data_minimum;
                break;

            case CONVERT_INTERPOLATION_1D :
                check_under_over_range = TRUE;

                // If ((Xn < input_data) && (input_data < Xn+1)), then…
                // Modified Value = Yn + ((Yn+1 - Yn) x (input_data - Xn) / (Xn+1 - Xn)).
                if (table_ptr->input_type > DATATYPE_UINT64)
                {
                    if (sint_input_data <= sint_input_minimum_value)
                    {
                        //Return the smallest value from the output table if the requested raw data is smaller the smallest value of the input table.
                        output_table_data = output_data_minimum;
                    }
                    else if (sint_input_data >= sint_input_maximum_value)
                    {
                        //Return the smallest value from the output table if the requested raw data is smaller the smallest value of the input table.
                        output_table_data = output_data_maximum;
                    }
                    else
                    {
                        //Interpolation method
                        output_table_data           =   (sint_input_data - sint_input_minimum_value);
                        output_table_data          *=   output_data_maximum - output_data_minimum;
                        output_table_data          /=   sint_input_maximum_value - sint_input_minimum_value;
                        output_table_data          +=   output_data_minimum;
                    }
                }
                else
                {
                    if (uint_input_data <= uint_input_minimum_value)
                    {
                        //Return the smallest value from the output table if the requested raw data is smaller the smallest value of the input table.
                        output_table_data = output_data_minimum;
                    }
                    else if (uint_input_data >= uint_input_maximum_value)
                    {
                        //Return the smallest value from the output table if the requested raw data is smaller the smallest value of the input table.
                        output_table_data = output_data_maximum;
                    }
                    else
                    {
                        //Interpolation method
                        //signed output
                        output_table_data           =   (uint_input_data - uint_input_minimum_value);
                        output_table_data          *=   output_data_maximum - output_data_minimum;
                        output_table_data          /=   (sint32)(uint_input_maximum_value - uint_input_minimum_value);
                        output_table_data          +=   output_data_minimum;
                    }
                }
                break;

            case CONVERT_INTERPOLATION_2D :
            default:
                convert_status = CONVERT_STATUS_PARAM_ERROR;
                break;
        }

        // Method to detect if the conversion value of under or over the range
		if (check_under_over_range == TRUE)
        {
			if ( INPUT_DATA < INPUT_MINIMUM_VALUE )
			{
				convert_status = CONVERT_STATUS_UNDER_RANGE;
			}
			else if ( INPUT_DATA > INPUT_MAXIMUM_VALUE )
			{
				convert_status = CONVERT_STATUS_OVER_RANGE;

			}
			// else INPUT_MINIMUM_VALUE <= INPUT_DATA <= INPUT_MAXIMUM_VALUE, do nothing
		}


        //store the converted data
        (*converted_data_ptr) = output_table_data;
    }

    return(convert_status);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This methods converts the sign extension
 * @param table_ptr
 * @param index
 * @param data_size
 * @param mask
 * @return
 */
static sint16 ConvertSignExtension(void * table_ptr, CONVERT_SIZE_TYPE index, uint8 data_size, uint16 mask)
{
    sint16 sint16_data;

    sint16_data = (sint16)ExtractData((uint8 *)table_ptr, index, data_size, mask);

    //sign extension
    if (sint16_data & (1 << ((8 * data_size) - 1)))
    {
        sint16_data = sint16_data | (~mask);
    }

    return(sint16_data);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @param table_ptr
 * @param table_size
 * @param input_data
 * @param input_data_size
 * @param input_mask
 * @param minimum_index
 * @param maximum_index
 */
static void SearchTableUnsigned(void * table_ptr, CONVERT_SIZE_TYPE table_size, uint16 input_data, uint8 input_data_size, uint16 input_mask, CONVERT_SIZE_TYPE *minimum_index, CONVERT_SIZE_TYPE *maximum_index)
{
    CONVERT_SIZE_TYPE min = 0;
    CONVERT_SIZE_TYPE max = 0;
    CONVERT_SIZE_TYPE curr = 0;

    if (table_size != 0)
    {
        min = 0;
        max = table_size - 1;

        if (input_data < ExtractData((uint8 *)table_ptr, min, input_data_size, input_mask))
        {
            //Return the smallest value from the input table.
            max = min;
        }
        else if (input_data > ExtractData((uint8 *)table_ptr, max, input_data_size, input_mask))
        {
            //Return the biggest value from the input table.
        }
        else
        {
            while((max-min) > 1)
            {
                //Upward
                curr = ((max - min)>>1) + min;

                if (ExtractData((uint8 *)table_ptr, curr, input_data_size, input_mask) < input_data)
                {
                    min = curr;
                }
                else
                {
                    max = curr;
                }
            }
        }
    }

    *minimum_index = min;
    *maximum_index = max;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief -
 * @param table_ptr
 * @param table_size
 * @param input_data
 * @param input_data_size
 * @param input_mask
 * @param minimum_index
 * @param maximum_index
 */
static void SearchTableSigned(void * table_ptr, CONVERT_SIZE_TYPE table_size, sint16 input_data, uint8 input_data_size, uint16 input_mask, CONVERT_SIZE_TYPE *minimum_index, CONVERT_SIZE_TYPE *maximum_index)
{
    CONVERT_SIZE_TYPE min = 0;
    CONVERT_SIZE_TYPE max = 0;
    CONVERT_SIZE_TYPE curr = 0;

    if (table_size != 0)
    {
        min = 0;
        max = table_size - 1;

        if (input_data < ConvertSignExtension(table_ptr, min, input_data_size, input_mask))
        {
            //Return the smallest value from the input table.
            max = min;
        }
        else if (input_data > ConvertSignExtension(table_ptr, max, input_data_size, input_mask))
        {
            //Return the biggest value from the input table.
        }
        else
        {
            while((max-min) > 1)
            {
                //Upward
                curr = ((max - min)>>1) + min;

                if (ConvertSignExtension(table_ptr, curr, input_data_size, input_mask) < input_data)
                {
                    min = curr;
                }
                else
                {
                    max = curr;
                }
            }
        }
    }

    *minimum_index = min;
    *maximum_index = max;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief -
 * @param table_ptr
 * @param index
 * @param data_size
 * @param mask
 * @return
 */
static uint16 ExtractData(uint8 *table_ptr, CONVERT_SIZE_TYPE index, uint8 data_size, uint16 mask)
{
    if (data_size == 1)
    {
        return((*(uint16 PACKED *)(table_ptr + data_size * index) & mask));         //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
    }
    else
    {
        return(ENDIANU16_STON(*(uint16 PACKED *)(table_ptr + data_size * index) & mask));   //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
    }
}

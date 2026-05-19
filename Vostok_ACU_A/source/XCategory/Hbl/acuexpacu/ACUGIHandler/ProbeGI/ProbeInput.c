/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: ProbeInput.c 1.6 2015/07/24 11:27:58EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/24 11:27:58EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "ProbeInput.h" 

#if (HBL_GI_NUM_PROBE > 0)

#include "SettingFile.h"
#include "Convert.h"
#include <String.h>
#include <limits.h>
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
static uint8 ProbeInput_Allocation;
static uint8 ProbeInput_LLI_Sequence[HBL_GI_NUM_PROBE];
static float32 output_table_data = 0;
static float32 floatfromQ15result;

static uint8 DATA_TYPE_SIZE[] =
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


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      It Initializes the module DigitalLowLevel and its variables
 */
void ProbeInput__Initialize(void)
{
    ProbeInput_Allocation = 0;
    memset(ProbeInput_LLI_Sequence, 0, sizeof(ProbeInput_LLI_Sequence));
}



//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @return
 */
uint8 ProbeInput__Allocate(void)
{
    uint8 retval;
    retval = 0xFF;
    if (ProbeInput_Allocation < (uint8)HBL_GI_NUM_PROBE)
    {
        retval = ProbeInput_Allocation;
        ProbeInput_Allocation++;

    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module ProbeInput and its variables
 *
 */
PASS_FAIL_TYPE ProbeInput_InitializeInstance(HBL_GI_TYPE * generic_input, uint8 lli_sequence)
{
    PASS_FAIL_TYPE retval;

    retval = FAIL;

    if(generic_input->Data_Index < ProbeInput_Allocation)
    {
        ProbeInput_LLI_Sequence[generic_input->Data_Index] = lli_sequence;
        retval = PASS;
    }
    return(retval);
}


/**
 *
 * @param generic_input
 * @param lli_data
 * @param data
 * @return
 */
void * ProbeInput__Get(HBL_GI_TYPE * generic_input,void * lli_data)
{
    return(lli_data);
}

/**
 *
 * @param generic_input
 * @param lli_data
 * @return
 */
BOOL_TYPE ProbeInput__AsynchProcess(HBL_GI_TYPE * generic_input,void * lli_data, uint8 lli_sequence)
{
    BOOL_TYPE retval;

    retval = FALSE;

    if(generic_input->Data_Index < ProbeInput_Allocation)
    {
        if (ProbeInput_LLI_Sequence[generic_input->Data_Index] != lli_sequence)
        {
            ProbeInput_LLI_Sequence[generic_input->Data_Index] = lli_sequence;
            retval = TRUE;
        }
    }
    return(retval);
}

/**
 *
 * @param generic_input
 * @param lli_data
 * @return
 */
sint32 ProbeInput__GetConverted(HBL_GI_TYPE * generic_input,void * lli_data)
{
    uint16 data_in;
    sint32 retval;
    uint8 index;
    HBL_IO_DEVICE_DATA_CONVERT_TABLE_HEADER *table_header;
    SETTINGFILE_LOADER_TYPE io_device_data;
    CONVERT_TABLE_TYPE convert_table;

    retval = 0; //

    data_in = *((uint16 PACKED *)lli_data);

    index = generic_input->GI_Param_Offset;
    if (generic_input->GI_Param_Offset != (uint8)0XFF)
    {
        if (SettingFile__BasicLoader(SF_PTR_ACU_IO_DEVICE, index, &io_device_data) == PASS)
        {
            table_header = (HBL_IO_DEVICE_DATA_CONVERT_TABLE_HEADER *)(void*)(io_device_data.Data);    //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]

            convert_table.input_table_1        = (void *)(&io_device_data.Data[2]);
            convert_table.input_type           = DATATYPE_UINT8;
            convert_table.output_type          = DATATYPE_UINT8;
            convert_table.search_method        = CONVERT_CEILING;
            convert_table.table_size           = table_header->Point_Number;

            //Input Data Type
            if (table_header->S1 == (uint8)1)
            {
                convert_table.input_type       = DATATYPE_UINT16;
                convert_table.output_table     = (void *)((&io_device_data.Data[2]) + (table_header->Point_Number << 1));
            }
            else
            {
                convert_table.output_table     = (void *)((&io_device_data.Data[2]) + table_header->Point_Number);
            }

            //Output Data Type
            if (table_header->S2 == (uint8)1)
            {
                convert_table.output_type = DATATYPE_UINT16;
            }
            if (table_header->S2 == (uint8)3)
            {
                convert_table.output_type = DATATYPE_SINT16;
            }

            if ((BOOL_TYPE)table_header->Interpolation != (BOOL_TYPE)FALSE)
            {
                convert_table.search_method        = CONVERT_INTERPOLATION_1D;
            }


            retval = (sint32)Convert__Process(data_in, &convert_table);
        }
    }
    else
    {
        retval = (sint32)data_in;

    }

    return (retval);
}


static uint16 ExtractData(uint8 *table_ptr, CONVERT_SIZE_TYPE index, uint8 data_size, uint16 mask)
{
    uint16 ret;

    if (data_size == (uint8)1)
    {
        ret = ((*(uint16 PACKED *)(void*)(table_ptr + (data_size * index)) & mask));
    }
    else
    {
        ret = (ENDIANU16_STON(*(uint16 PACKED *)(void*)(table_ptr + (data_size * index)) & mask));
    }

    return ret;
}


static void SearchTableUnsigned(void * table_ptr, CONVERT_SIZE_TYPE table_size, uint16 input_data, uint8 input_data_size, uint16 input_mask, CONVERT_SIZE_TYPE *minimum_index, CONVERT_SIZE_TYPE *maximum_index)
{
    CONVERT_SIZE_TYPE min = 0;
    CONVERT_SIZE_TYPE max = 0;
    CONVERT_SIZE_TYPE curr = 0;

    if (table_size != (uint8)0)
    {
        min = 0;
        max = table_size - (uint8)1;

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
            while((max-min) > (uint8)1)
            {
                //Upward
                curr = ((uint8)(max - min)>>1) + min;

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

void * ProbeInput__GetFloat (HBL_GI_TYPE * generic_input,void * lli_data)
{
    BOOL_TYPE check_under_over_range = TRUE;
    uint16 uint_input_data;
    uint16 uint_input_minimum_value = USHRT_MAX;
    uint16 uint_input_maximum_value = 0;
    uint8 input_data_size;
    uint8 output_data_size;
    uint16 input_mask;
    uint16 output_mask;
    CONVERT_SIZE_TYPE minimum_index;
    CONVERT_SIZE_TYPE maximum_index;
    sint32 output_data_minimum;
    sint32 output_data_maximum;
    uint16 data_in;
    uint8 index;
    HBL_IO_DEVICE_DATA_CONVERT_TABLE_HEADER *table_header;
    SETTINGFILE_LOADER_TYPE io_device_data;
    CONVERT_TABLE_TYPE convert_table;
    float32* retvalGetFloat;

    retvalGetFloat = NULL;
    data_in = *((uint16 PACKED *)lli_data);
    index = generic_input->GI_Param_Offset;

    if (generic_input->GI_Param_Offset != (uint8)0XFF)
    {
        if (SettingFile__BasicLoader(SF_PTR_ACU_IO_DEVICE, index, &io_device_data) == PASS)
        {
            table_header = (HBL_IO_DEVICE_DATA_CONVERT_TABLE_HEADER *)(void*)(io_device_data.Data);

            convert_table.input_table_1        = (void *)(&io_device_data.Data[2]);
            convert_table.input_type           = DATATYPE_UINT8;
            convert_table.output_type          = DATATYPE_UINT8;
            convert_table.search_method        = CONVERT_CEILING;
            convert_table.table_size           = table_header->Point_Number;

            //Input Data Type
            if (table_header->S1 == (uint8)1)
            {
                convert_table.input_type       = DATATYPE_UINT16;
                convert_table.output_table     = (void *)((&io_device_data.Data[2]) + (table_header->Point_Number << 1));
            }
            else
            {
                convert_table.output_table     = (void *)((&io_device_data.Data[2]) + table_header->Point_Number);
            }

            //Output Data Type
            if (table_header->S2 == (uint8)1)
            {
                convert_table.output_type = DATATYPE_UINT16;
            }
            if (table_header->S2 == (uint8)3)
            {
                convert_table.output_type = DATATYPE_SINT16;
            }

            if ((BOOL_TYPE)table_header->Interpolation != (BOOL_TYPE)FALSE)
            {
                convert_table.search_method = CONVERT_INTERPOLATION_1D;
            }

            // Convert__Process..
            output_table_data   = data_in;

            input_data_size     = DATA_TYPE_SIZE[convert_table.input_type];
            output_data_size    = DATA_TYPE_SIZE[convert_table.output_type];

            uint_input_data     = (uint16)data_in;

            input_mask          = (0xFFFFFFFFU >> ((uint16)((uint16)4 - input_data_size) * (uint16)8)) & 0xFFFFU;
            output_mask         = (0xFFFFFFFFU >> ((uint16)((uint16)4 - output_data_size) * (uint16)8)) & 0xFFFFU;

            SearchTableUnsigned (convert_table.input_table_1, convert_table.table_size, uint_input_data, input_data_size, input_mask, &minimum_index, &maximum_index);

            uint_input_minimum_value    =   (uint16)ExtractData((uint8 *)convert_table.input_table_1, minimum_index, input_data_size,  input_mask);
            uint_input_maximum_value    =   (uint16)ExtractData((uint8 *)convert_table.input_table_1, maximum_index, input_data_size,  input_mask);

            output_data_minimum         =   ExtractData((uint8 *)convert_table.output_table,  minimum_index, output_data_size, output_mask);
            output_data_maximum         =   ExtractData((uint8 *)convert_table.output_table,  maximum_index, output_data_size, output_mask);

            switch(convert_table.search_method)
            {
                case CONVERT_CEILING :
                    check_under_over_range = FALSE;
                    output_table_data = (BOOL_TYPE)(uint_input_data <= uint_input_minimum_value) ? output_data_minimum : output_data_maximum;
                    break;

                case CONVERT_INTERPOLATION_1D :
                    check_under_over_range = FALSE;

                    // If ((Xn < input_data) && (input_data < Xn+1)), then…
                    // Modified Value = Yn + ((Yn+1 - Yn) x (input_data - Xn) / (Xn+1 - Xn)).
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
                        output_table_data           =   ((float32)uint_input_data - (float32)uint_input_minimum_value);
                        output_table_data          *=   (float32)output_data_maximum - (float32)output_data_minimum;
                        output_table_data          /=   (float32)((float32)uint_input_maximum_value - (float32)uint_input_minimum_value);
                        output_table_data          +=   (float32)output_data_minimum;
                    }

                    break;

                case CONVERT_EXACT_MATCH :
                case CONVERT_BIT_MATCH :
                case CONVERT_FLOOR :
                case CONVERT_INTERPOLATION_2D :
                default:
                    retvalGetFloat = NULL;
                    break;
            }

            // Method to detect if the conversion value of under or over the range
            if (check_under_over_range == FALSE)
            {
                retvalGetFloat = (float32*)&(output_table_data);
            }
            else
            {
                retvalGetFloat = NULL;
            }
        }
    }
    else
    {
        retvalGetFloat = NULL;
    }

    return (void *) (retvalGetFloat);
}

/**
 *
 * @param generic_input generic input structure pointer
 * @param lli_data low level input structure pointer
 * @return GI value converted according to setting file params expressed in C° and in float32 format
 */

void * ProbeInput__GetFloatFromQ15Converted(HBL_GI_TYPE * generic_input,void * lli_data)
{
    float32* value;
    float32* retvalGetFloat = NULL;
    SETTINGFILE_LOADER_TYPE loader;
    uint16 base_temp = 0;

    floatfromQ15result = 0.0;
    value = (float32*)ProbeInput__GetFloat(generic_input,lli_data);
    if (NULL != value)
    {
        if ( SettingFile__BasicLoader(SF_PTR_UI_PRODUCT_CONFIG, SF_DISPL_TEMPERATURE_SCALE, &loader) == PASS )
        {
            if ( sizeof(uint16) <= loader.Length )
            {
                base_temp = (uint16)(*((uint16*)((void*)(loader.Data))));
                floatfromQ15result = (float32)((((*value))*((float32)base_temp))); /*Conversion from Q15 to C°*/
                floatfromQ15result /= (float32)32768;
                retvalGetFloat = (float32*)&(floatfromQ15result);
            }
        }
    }
    return (void *) (retvalGetFloat);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


#endif

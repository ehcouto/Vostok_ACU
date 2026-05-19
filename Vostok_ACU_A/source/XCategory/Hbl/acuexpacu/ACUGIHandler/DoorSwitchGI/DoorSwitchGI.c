/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: DoorSwitchGI.c 1.4 2015/07/24 11:35:20EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/24 11:35:20EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "DoorSwitchGI.h"

#if (HBL_GI_NUM_DOORSWITCH > 0)

#include "Convert.h"
#include "string.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
static uint8 DoorSwitchGI_Allocation;
static uint8 DoorSwitchGI_LLI_Sequence[HBL_GI_NUM_DOORSWITCH];
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      It Initializes the module DigitalLowLevel and its variables
 */
void DoorSwitchGI__Initialize(void)
{
    DoorSwitchGI_Allocation = 0;
    memset(DoorSwitchGI_LLI_Sequence, 0, sizeof(DoorSwitchGI_LLI_Sequence));
}



//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @return
 */
uint8 DoorSwitchGI__Allocate(void)
{
    uint8 retval;
    retval = 0xFF;
    if (DoorSwitchGI_Allocation < HBL_GI_NUM_DOORSWITCH)
    {
        retval = DoorSwitchGI_Allocation;
        DoorSwitchGI_Allocation++;

    }
    return (retval);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module DoorSwitchGI and its variables
 *
 */
PASS_FAIL_TYPE DoorSwitchGI__InitializeInstance(HBL_GI_TYPE * generic_input, uint8 lli_sequence)
{
	PASS_FAIL_TYPE retval;

	retval = FAIL;

	if(generic_input->Data_Index < DoorSwitchGI_Allocation)
	{
		DoorSwitchGI_LLI_Sequence[generic_input->Data_Index] = lli_sequence;
		retval = PASS;
	}
    return(retval);}


void * DoorSwitchGI__Get(HBL_GI_TYPE * generic_input,void * lli_data)
{

    return (lli_data);
}


BOOL_TYPE DoorSwitchGI__AsynchProcess(HBL_GI_TYPE * generic_input,void * lli_data, uint8 lli_sequence)
{
	BOOL_TYPE retval;

	retval = FALSE;

	if(generic_input->Data_Index < DoorSwitchGI_Allocation)
	{
		if (DoorSwitchGI_LLI_Sequence[generic_input->Data_Index] != lli_sequence)
		{
			DoorSwitchGI_LLI_Sequence[generic_input->Data_Index] = lli_sequence;
			retval = TRUE;
		}
	}
    return(retval);
}


sint32 DoorSwitchGI__GetConverted(HBL_GI_TYPE * generic_input,void * lli_data)
{
    uint16 data_in;
    sint32 retval;
    uint8 index;
    HBL_IO_DEVICE_DATA_CONVERT_TABLE_HEADER *table_header;
    SETTINGFILE_LOADER_TYPE io_device_data;
    CONVERT_TABLE_TYPE convert_table;

    retval = 0;

    data_in = (uint16)(*((uint8 *)lli_data));

    index = generic_input->GI_Param_Offset;
    if (generic_input->GI_Param_Offset != 0XFF)
    {
        if (SettingFile__BasicLoader(SF_PTR_ACU_IO_DEVICE, index, &io_device_data) == PASS)
        {
            table_header = (HBL_IO_DEVICE_DATA_CONVERT_TABLE_HEADER *)(io_device_data.Data); //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]

            convert_table.input_table_1        = (void *)(&io_device_data.Data[2]);
            convert_table.input_type           = DATATYPE_UINT8;
            convert_table.output_type          = DATATYPE_UINT8;
            convert_table.search_method        = CONVERT_EXACT_MATCH;
            convert_table.table_size           = table_header->Point_Number;

            //Input Data Type
            if (table_header->S1 == 1)
            {
                convert_table.input_type       = DATATYPE_UINT16;
                convert_table.output_table     = (void *)((&io_device_data.Data[2]) + (table_header->Point_Number << 1));
            }
            else
            {
                convert_table.output_table     = (void *)((&io_device_data.Data[2]) + table_header->Point_Number);
            }

            //Output Data Type
            if (table_header->S2 == 1)
            {
                convert_table.output_type = DATATYPE_UINT16;
            }

            if (table_header->Interpolation == 1)
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


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


#endif

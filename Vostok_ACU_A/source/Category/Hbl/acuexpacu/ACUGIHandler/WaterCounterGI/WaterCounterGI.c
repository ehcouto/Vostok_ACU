/**
 *  @file		WaterCounterGI.c
 *
 *  @brief      This module implement Water Counter GI
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>
 *
 *  @copyright  Copyright 2016-2017 - Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "WaterCounterGI.h"

#include "C_Extensions.h"

#if (HBL_GI_NUM_WATERCOUNTER > 0)

#include "SettingFile.h"
#include "String.h"
#include "Variable.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

PACKED typedef struct
{
	uint16 Pulse_Counter_Ref;
	uint16 Last_LLI_Value;
	uint16 Accumulator;
	uint16 Water_Counter;								//In terms of Centi Litre (CL)
	uint16 Conversion_Coefficient;
} FLOWMETERGI_DATA_TYPE;
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
static uint8 WaterCounterGI_Allocation;
static uint8 WaterCounterGI_LLI_Sequence[HBL_GI_NUM_WATERCOUNTER];
static FLOWMETERGI_DATA_TYPE WaterCounterGI_Data[HBL_GI_NUM_WATERCOUNTER];
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      It Initializes the module and its variables
 */
void WaterCounterGI__Initialize(void)
{
    WaterCounterGI_Allocation = 0;
    memset(WaterCounterGI_LLI_Sequence, 0, sizeof(WaterCounterGI_LLI_Sequence));
    memset(WaterCounterGI_Data, 0, sizeof(WaterCounterGI_Data));
}



//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @return
 */
uint8 WaterCounterGI__Allocate(void)
{
    uint8 retval;
    retval = 0xFF;
    if (WaterCounterGI_Allocation < (uint8)HBL_GI_NUM_WATERCOUNTER)
    {
        retval = WaterCounterGI_Allocation;
        WaterCounterGI_Allocation++;
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module WaterCounterGI and its variables
 *
 */
PASS_FAIL_TYPE WaterCounterGI__InitializeInstance(HBL_GI_TYPE * generic_input, uint8 lli_sequence)
{
	PASS_FAIL_TYPE retval;
	SETTINGFILE_LOADER_TYPE io_device_data;

	retval = FAIL;

	if (generic_input->Data_Index < WaterCounterGI_Allocation)
	{
		WaterCounterGI_LLI_Sequence[generic_input->Data_Index] = lli_sequence;
		if (SettingFile__BasicLoader(SF_PTR_ACU_IO_DEVICE, generic_input->GI_Param_Offset, &io_device_data) == PASS)
		{
			WaterCounterGI_Data[generic_input->Data_Index].Conversion_Coefficient =	ENDIANU16_STON((*(uint16 *)(void *)(io_device_data.Data)));
			retval = PASS;
		}
	}

    return (retval);
}


/**
 *
 * @param generic_input
 * @param lli_data
 * @param data
 * @return
 */
void * WaterCounterGI__Get(HBL_GI_TYPE * generic_input,void * lli_data)
{
    return(lli_data);
}

/**
 *
 * @param generic_input
 * @param lli_data
 * @return
 */
BOOL_TYPE WaterCounterGI__AsynchProcess(HBL_GI_TYPE * generic_input,void * lli_data, uint8 lli_sequence)
{
	BOOL_TYPE retval = FALSE;

	if(generic_input->Data_Index < WaterCounterGI_Allocation)
	{
		FLOWMETERGI_DATA_TYPE *data = &WaterCounterGI_Data[generic_input->Data_Index];
		sint32 flow_meter_ref_variable;

		if (Variable__GetValue(VARIABLE_PULSE_COUNTER_REF, generic_input->Data_Index, &flow_meter_ref_variable) == FALSE)
		{
			flow_meter_ref_variable = 0;
		}

		if (data->Pulse_Counter_Ref != (uint16)flow_meter_ref_variable)
		{
			data->Pulse_Counter_Ref = (uint16)flow_meter_ref_variable;
			data->Last_LLI_Value = (uint16)flow_meter_ref_variable;
			data->Accumulator = 0;
			data->Water_Counter = 0;
			retval = TRUE;
			WaterCounterGI_LLI_Sequence[generic_input->Data_Index] = lli_sequence;
		}

		if (WaterCounterGI_LLI_Sequence[generic_input->Data_Index] != lli_sequence)
		{
			uint32 new_water_level;
			uint16 new_lli_value = *(uint16*)lli_data;

			if (new_lli_value < data->Last_LLI_Value)
			{
				data->Accumulator++;
			}

			new_water_level = (new_lli_value + (uint32)((uint32)data->Accumulator << 16));
			new_water_level -= data->Pulse_Counter_Ref;
			new_water_level *= (uint8)100;
			// Adding divisor/2 before the division, in order to round to the nearest integer instead of truncating
			new_water_level += ((uint32)WaterCounterGI_Data[generic_input->Data_Index].Conversion_Coefficient / (uint8)2);
			new_water_level /= WaterCounterGI_Data[generic_input->Data_Index].Conversion_Coefficient;
			new_water_level &= (uint16)0xFFFF;

			if (data->Water_Counter != new_water_level)
			{
				data->Water_Counter = (uint16)new_water_level;
				retval = TRUE;
			}
			data->Last_LLI_Value = new_lli_value;
			WaterCounterGI_LLI_Sequence[generic_input->Data_Index] = lli_sequence;

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
sint32 WaterCounterGI__GetConverted(HBL_GI_TYPE * generic_input,void * lli_data)
{
    sint32 retval;

    retval = (sint32)WaterCounterGI_Data[generic_input->Data_Index].Water_Counter;

    return (retval);
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#endif

/**
 *  @file       
 *
 *  @brief      This file contains the implementation of the ConvertGI interface
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/24 11:35:20EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "ConvertGI.h"
#include "string.h"
#include "limits.h"


#if (HBL_GI_NUM_CONVERT > 0)
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
static uint8 ConvertGI_Allocation;
static uint8 ConvertGI_LLI_Sequence[HBL_GI_NUM_CONVERT];



//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================


//------------------------------------------------------------------------------------------------
/**
 * @brief Initialize all instances of this GI type.
 *          - Initialize static RAM used by the module.
 *          - Initialize any corrupted NVData used by the module.
 */

void ConvertGI__Initialize(void)
{
    ConvertGI_Allocation = 0;
    memset(ConvertGI_LLI_Sequence, 0, sizeof(ConvertGI_LLI_Sequence));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Reserve memory for this GI instance.
 * @return The index of the allocated GI.
 *          Return UCHAR_MAX if there is no space allocated for another GI of this type.
 */
uint8 ConvertGI__Allocate(void)
{
    uint8 retval = UCHAR_MAX;

    // if there is an unclaimed instance of this GI, then set index to the next GI.
    if (ConvertGI_Allocation < HBL_GI_NUM_CONVERT)
    {
        retval = ConvertGI_Allocation;
        ConvertGI_Allocation++;

    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Initialize this GI instance.
 * @param generic_input = A pointer to the HBL_GI_TYPE structure with GI and GI instance information.
 * @param lli_sequence = The low level input sequence.
 * @return PASS if the GI instance is successfully initialized; otherwise FAIL.
 *          The return value is ignored by HBL.
 */
PASS_FAIL_TYPE ConvertGI__InitializeInstance(HBL_GI_TYPE * generic_input, uint8 lli_sequence)
{
	PASS_FAIL_TYPE retval;

	retval = FAIL;

	if(generic_input->Data_Index < ConvertGI_Allocation)
	{
		ConvertGI_LLI_Sequence[generic_input->Data_Index] = lli_sequence;
		retval = PASS;
	}
    return(retval);}

//------------------------------------------------------------------------------------------------
/**
 * @brief Get a pointer to the unconverted value of this GI instance.
 * @param generic_input = A pointer to the HBL_GI_TYPE structure with GI and GI instance information.
 * @param lli_data = A pointer to the low level input data.
 * @return A pointer to data that holds the unconverted value of the load.
 */
void * ConvertGI__Get(HBL_GI_TYPE * generic_input,void * lli_data)
{
    // The unconverted value is pointed to by lli_data
    return (lli_data);
}

//------------------------------------------------------------------------------------------------
/**
 * @brief Handle behaviors that must be implemented periodically by this GI instance.
 *        This function is called every 5ms.
 *
 *        This function maintains the data sequence logic to differentiate
 *        the old converted value with the new converted value
 *
 * @param generic_input = A pointer to the HBL_GI_TYPE structure with GI and GI instance information.
 * @param lli_data = A pointer to the low level input data.
 * @param lli_sequence = The low level input sequence.
 * @return PASS or FAIL.
 */
BOOL_TYPE ConvertGI__AsynchProcess(HBL_GI_TYPE * generic_input,void * lli_data, uint8 lli_sequence)
{
	BOOL_TYPE retval;

	retval = FALSE;

	if(generic_input->Data_Index < ConvertGI_Allocation)
	{
	    //This logic keeps the sequence or track of old verses new LLI value
		if (ConvertGI_LLI_Sequence[generic_input->Data_Index] != lli_sequence)
		{
			ConvertGI_LLI_Sequence[generic_input->Data_Index] = lli_sequence;
			retval = TRUE;
		}
	}
    return(retval);
}

//------------------------------------------------------------------------------------------------
/**
 * @brief Get the converted value of this GI instance.
 * @param generic_input = A pointer to the HBL_GI_TYPE structure with GI and GI instance information.
 * @param lli_data = A pointer to the low level input data.
 * @return The converted lli_data value of the GI instance.
 */
sint32 ConvertGI__GetConverted(HBL_GI_TYPE * generic_input, void * lli_data)
{
    sint32 retval = 0;

    uint8 index = generic_input->GI_Param_Offset;
    if (generic_input->GI_Param_Offset != 0XFF)
    {
        SETTINGFILE_LOADER_TYPE io_device_data;
        uint16 data_in;

        if (generic_input->LLI_Parameter_Size >= 2)
        {
            data_in = (uint16)(*((PACKED uint16 *)lli_data));

        }
        else
        {
            data_in = (uint16)(*((uint8 *)lli_data));
        }

        if (SettingFile__BasicLoader(SF_PTR_ACU_IO_DEVICE, index, &io_device_data) == PASS)
        {
            retval = HblCommon__ConvertIODeviceData(data_in, (HBL_IO_DEVICE_DATA_CONVERT_TABLE_HEADER *)(io_device_data.Data));
        }
    }
    else
    {
        retval = (sint32)(*((uint8 *)lli_data));
    }

    return (retval);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#endif


/**
 *  @file
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *  @copyright  Copyright 2015-2016 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "ProbeLpsGI.h"
#include "C_Extensions.h"

#if (HBL_GI_NUM_PROBE_LPS > 0)

#include "SettingFile.h"
#include "String.h"
#include "Variable.h"

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
static uint8  ProbeLpsGI_Allocation;
static uint8 ProbeLpsGI_LLI_Sequence[HBL_GI_NUM_PROBE_LPS];
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static uint16 AnalizeZeroLev (uint16 data_inLocal, SETTINGFILE_LOADER_TYPE * io_device_data_local);

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      It Initializes the module DigitalLowLevel and its variables
 */
void  ProbeLpsGI__Initialize(void)
{
	 ProbeLpsGI_Allocation = 0;
	 memset(ProbeLpsGI_LLI_Sequence, 0, sizeof(ProbeLpsGI_LLI_Sequence));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @return
 */
uint8  ProbeLpsGI__Allocate(void)
{
    uint8 retval;
    retval = 0xFF;
    if ( ProbeLpsGI_Allocation < HBL_GI_NUM_PROBE_LPS)
    {
        retval =  ProbeLpsGI_Allocation;
        ProbeLpsGI_Allocation++;

    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module ProbeInput and its variables
 *
 */
PASS_FAIL_TYPE ProbeLpsGI_InitializeInstance(HBL_GI_TYPE * generic_input, uint8 lli_sequence)
{
    PASS_FAIL_TYPE retval;

	retval = FAIL;

	if(generic_input->Data_Index < ProbeLpsGI_Allocation)
	{
		ProbeLpsGI_LLI_Sequence[generic_input->Data_Index] = lli_sequence;
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
void *  ProbeLpsGI__Get(HBL_GI_TYPE * generic_input,void * lli_data)
{
    return(lli_data);
}

/**
 *
 * @param generic_input
 * @param lli_data
 * @return
 */
BOOL_TYPE ProbeLpsGI__AsynchProcess(HBL_GI_TYPE * generic_input,void * lli_data, uint8 lli_sequence)
{
	BOOL_TYPE retval;

	retval = FALSE;

	if(generic_input->Data_Index < ProbeLpsGI_Allocation)
	{
		if (ProbeLpsGI_LLI_Sequence[generic_input->Data_Index] != lli_sequence)
		{
			ProbeLpsGI_LLI_Sequence[generic_input->Data_Index] = lli_sequence;
			retval = TRUE;
		}
	}
    return(retval);

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param generic_input
 * @param lli_data
 * @return
 */
sint32  ProbeLpsGI__GetConverted(HBL_GI_TYPE * generic_input,void * lli_data)
{
    uint16 data_in;
    sint32 retval;
    uint8 index;
    SETTINGFILE_LOADER_TYPE io_device_data;

    retval = 0;

    data_in = *((uint16 PACKED *)lli_data);

    index = generic_input->GI_Param_Offset;

    if (generic_input->GI_Param_Offset != 0XFF)
    {
        if (SettingFile__BasicLoader(SF_PTR_ACU_IO_DEVICE, index, &io_device_data) == PASS)
        {
            data_in = AnalizeZeroLev(data_in, &io_device_data);

            retval = HblCommon__ConvertIODeviceData(data_in, (HBL_IO_DEVICE_DATA_CONVERT_TABLE_HEADER *)(io_device_data.Data + OFFSET_RANGE_TICKS_VALUE));  //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
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

static uint16 AnalizeZeroLev (uint16 data_inLocal, SETTINGFILE_LOADER_TYPE * io_device_data_local)
{
    uint16 zeroLocal;
    sint16 retVal;
    sint32 CalPSensor_ZeroLevel;

	Variable__GetValue(VARIABLE_LPS_CALIBRATION_ZERO, 0, (sint32 *)(&CalPSensor_ZeroLevel));

	if ((CalPSensor_ZeroLevel >= (ENDIANU16_STON((*(uint16 *)(io_device_data_local->Data+2))))) && (CalPSensor_ZeroLevel <= (ENDIANU16_STON((*(uint16 *)(io_device_data_local->Data))))))
	{
		zeroLocal = CalPSensor_ZeroLevel;
	}
	else
	{
		zeroLocal = ENDIANU16_STON((*(uint16 *)(io_device_data_local->Data+4)));
	}

	retVal = data_inLocal - zeroLocal;

	if (retVal<0)
	{
		retVal = 0;
	}

	return (retVal);
}

#endif

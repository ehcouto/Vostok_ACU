/**
 *  @file       SRAnalogLLI.c
 *
 *  @defgroup	CLASS_B
 *
 *  @brief      Module for SRAnalogLLI readings
 *
 *  @details    This module perform reading of Analog channels and perform some filters.
 *
 *  			Analog channels can be read as Asynchronous or on AC line peaks (at angles 90 and 270 degrees of ZC), depending
 *  			on which register macro used (ANALOG_LLI_ASYNCH_REGISTER or ANALOG_LLI_PEAK_REGISTER)
 *
 *  			Filters applied:
 *  			- Average of 2^N samples (N is a setting parameter and can change by channel)
 *  			- Filter reading: if a new sample is higher than 2^M points than last sample read (M is a setting parameter)
 *  				 average is suspended for ANALOGLLI_FILTER_SAMPLES next samples (if they are within valid range with last readings)
 *  			- Channel compensation: If a channel is set as "Compensated", every sample is converted using compensation
 *  				algorithm, depending on Reference channel defined
 *
 *              For Class B checks:
 *              SRANALOG_EXTERNAL_CHECK_GPIO_FEATURE can be ENABLED if micro has no internal feature for Channel disconnection,
 *                  so an external pin is required to be used
 *              SRANALOG_EXTERNAL_CHECK_GPIO_LIST must be defined if SRANALOG_EXTERNAL_CHECK_GPIO_FEATURE is ENABLED; it will contain
 *                  an array of {Atod_Channel, Gpio_Port, Gpio_Pin} to find Gpio pin for an Atod channel.
 *
 *              Optional Features:
 *              SRANALOGLLI_FILTER_SAMPLES (default 3) is debounce counter when Delta filter is enabled
 *              SRANALOG_ONEMS_NUM_READS (default 25) set number of 1ms reading before providing each Peak to Peak / Peak values
 *                  if set to 0, N_Reads is used as parameter for 1ms readings - no further filters will be applied to result
 *              SRANALOG_ONEMS_NUM_READS_DIVIDER (default 1) can be used to divide N_Reads when a new sample comes out during 1ms readings (if SRANALOG_ONEMS_NUM_READS > 0)
 *              SRANALOG_ONEMS_FIXED_DELTA_FILTER (default 1) applies a fixed maximum/minimum Delta filter between 1ms reads.
 *                  If set to 0, setting file Delta parameter is used for that purpose and it will be disabled on sample result.
 *                  If set to 1, Delta filter on 1ms reads is disabled.
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  @copyright  Copyright 2016-$Date: 2015/07/24 11:27:57EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "SRAnalogLLI.h"

#if (HBL_LLI_NUM_SRANALOG > 0)
#include "VirtualPin.h"
#include "Atod.h"
#include "Micro.h"
#include "SRData.h"
#include "SRException.h"
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
#include "SRTaskSequence.h"
#endif

#ifndef SRANALOG_EXTERNAL_CHECK_GPIO_FEATURE
#define SRANALOG_EXTERNAL_CHECK_GPIO_FEATURE    DISABLED
#endif

#ifndef SRANALOG_ONEMS_NUM_READS
#define SRANALOG_ONEMS_NUM_READS                 25
#endif

#ifndef SRANALOG_ONEMS_NUM_READS_DIVIDER
#define SRANALOG_ONEMS_NUM_READS_DIVIDER         1
#endif

#ifndef SRANALOG_ONEMS_FIXED_DELTA_FILTER
#define SRANALOG_ONEMS_FIXED_DELTA_FILTER       1
#endif

#ifndef SRANALOGLLI_FILTER_SAMPLES
#define SRANALOGLLI_FILTER_SAMPLES              3
#endif

#if (SRANALOG_EXTERNAL_CHECK_GPIO_FEATURE == DISABLED)
    #ifdef SRANALOG_EXTERNAL_CHECK_GPIO_LIST
        #error "SRANALOG_EXTERNAL_CHECK_GPIO_LIST cannot be defined if SRANALOG_EXTERNAL_CHECK_GPIO_FEATURE is DISABLED"
    #else
        #define  SRANALOG_EXTERNAL_CHECK_GPIO_LIST   { {(ATOD_CHANNEL_DEF) 0xFF, PORT_NULL, 0} }
    #endif
#endif

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
typedef PACKED struct
{
    uint32 Sum;                     /* A/D samples's sum */
    uint16 Value;                   /* A/D current sample */
    uint16 Raw_Sample;              /* A/D raw sample */
    uint16 Last_Sample;             /* A/D old sample */
    uint16 Lower_Value;             /* A/D current min value */
    uint16 Higher_Value;            /* A/D current max value */
    uint8  Sample_Counter;          /* Number of Samples collected */
    uint8  Fast_Sample_Counter;     /* Number of Samples collected */
    uint8  Sync_Counter;            /* Counter used for max. variation between two samples */
    uint8  Fast_Sync_Counter;       /* Number of considered invalid value during 1ms process ATOD */
} SRANALOGLLI_DATA_TYPE;

// SRAnalogLLI Parameter table
typedef PACKED struct
{
	union
	{
		uint16 Complete;
		struct
		{
			uint8 N_Read:3;
			uint8 Unused:5;
			uint8 Filter_Reading:4;
			uint8 Channel_Discharge:1;
			uint8 Compensation:1;
			uint8 Reference:1;
			uint8 Ac_Line:1;
		};
	} Config;
	union
	{
		struct
		{
			uint16 Min;
			uint16 Max;
			uint16 Nominal;
			sint8 Pre_Offset;
			uint8 Hysteresis;
		} Reference;
		struct
		{
			sint8 Comp_Pre_Offset;
			sint8 Comp_Post_Offset;
			uint16 Zero_Level_Ref;
			uint16 unused[2];
		} Compensated;
	} Config_param;
} SRANALOGLLI_PARAM_TYPE;

typedef PACKED struct
{
	uint16 Reference_Value;
	uint16 Nominal_Value;
	sint8 Nominal_Offset;
} SRANALOGLLI_COMP_TYPE;

typedef PACKED struct
{
    ATOD_CHANNEL_DEF    Atod_Channel;
    GPIO_PORT_TYPE      Gpio_Port;
    uint8               Gpio_Pin;
} SRANALOGLLI_CHECK_PIN_TYPE;

typedef enum
{
    SRANALOGLLI_SINGLE_READ,
    SRANALOGLLI_FAST_READ,
    SRANALOGLLI_HI_PEAK_READ,
    SRANALOGLLI_LO_PEAK_READ,
} SRANALOGLLI_READ_TYPE;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static SRANALOGLLI_DATA_TYPE SRAnalogLLI_Data[HBL_LLI_NUM_SRANALOG];
static uint8 SRAnalogLLI_Channel[HBL_LLI_NUM_SRANALOG];
static SRANALOGLLI_COMP_TYPE SRAnalogLLI_Compensation;
static uint8 SRAnalogLLI_Allocation;

static SRANALOGLLI_DATA_TYPE N_SRAnalogLLI_Data[HBL_LLI_NUM_SRANALOG];
static uint8 N_SRAnalogLLI_Channel[HBL_LLI_NUM_SRANALOG];
static SRANALOGLLI_COMP_TYPE N_SRAnalogLLI_Compensation;
static uint8 N_SRAnalogLLI_Allocation;

static SRANALOGLLI_CHECK_PIN_TYPE const CHECK_PIN_TABLE[] =  SRANALOG_EXTERNAL_CHECK_GPIO_LIST;

static BOOL_TYPE AnalogInputDoRead(HBL_LLI_TYPE * input, SRANALOGLLI_READ_TYPE read_type, BOOL_TYPE is_peak_to_peak);
static PASS_FAIL_TYPE AnalogInputCheckSample(uint16 sample, uint16 ref, uint8 delta);
static uint16 DoCompensation(SRANALOGLLI_PARAM_TYPE *input_param, uint16 ad_value);
static void UpdateReference(SRANALOGLLI_PARAM_TYPE *input_param, uint16 advalue);
static uint8 FindCheckGpio(uint8 channel);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief	Initializes the SRAnalogLLI module and its variables
 */
void SRAnalogLLI__Initialize(void)
{
	SRData__UpdateByte(&SRAnalogLLI_Allocation, &N_SRAnalogLLI_Allocation, 0);
	SRData__Fill(SRAnalogLLI_Channel, N_SRAnalogLLI_Channel, 0xFF, sizeof(SRAnalogLLI_Channel));
	SRData__Fill(SRAnalogLLI_Data, N_SRAnalogLLI_Data, 0, sizeof(SRAnalogLLI_Data));
	SRData__Fill((void *) &SRAnalogLLI_Compensation,(void *) &N_SRAnalogLLI_Compensation, 0, sizeof(SRAnalogLLI_Compensation));

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_INIT, SRTASKSEQUENCE_APPL_INIT_SRANALOG_LLI);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief	Checks if there is free space and reserve a new instance
 * @return	index of AnalogLLI instance, 0xFF if all HBL_LLI_NUM_SRANALOG are already allocated
 */
uint8 SRAnalogLLI__Allocate(void)
{
    uint8 retval;
    retval = 0xFF;

    if ((SRData__CheckByte(&SRAnalogLLI_Allocation, &N_SRAnalogLLI_Allocation) == SRDATA_OK) &&
    	(SRAnalogLLI_Allocation < HBL_LLI_NUM_SRANALOG))
    {
        retval = SRAnalogLLI_Allocation;
    	SRData__UpdateByte(&SRAnalogLLI_Allocation, &N_SRAnalogLLI_Allocation, SRAnalogLLI_Allocation+1);
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      Initializes a new instance of SRAnalogLLI
 * @param input		pointer to HBL_LLI_TYPE structure reserved for that instance
 */
PASS_FAIL_TYPE SRAnalogLLI__InitializeInstance(HBL_LLI_TYPE *input)
{
    PASS_FAIL_TYPE retval = FAIL;

	if ((SRData__CheckByte(&SRAnalogLLI_Allocation, &N_SRAnalogLLI_Allocation) == SRDATA_OK) &&
			(input->Data_Index < SRAnalogLLI_Allocation))
    {
	    SRANALOGLLI_DATA_TYPE *data = &SRAnalogLLI_Data[input->Data_Index];
	    SRANALOGLLI_DATA_TYPE *n_data = &N_SRAnalogLLI_Data[input->Data_Index];
	    uint8 virtual_pin = input->Pin_Ptr[0];

        if((SRDATA_OK == SRData__CheckByte(&SRAnalogLLI_Channel[input->Data_Index], &N_SRAnalogLLI_Channel[input->Data_Index])) &&
        		(SRDATA_OK == SRData__CheckArrayNonBlocking(data, n_data, sizeof(SRANALOGLLI_DATA_TYPE))))
        {
    		if (VIRTUALPIN_TABLE[virtual_pin].pin_config == (uint8) INPUT_ANALOG)
            {
                SRData__UpdateByte(&SRAnalogLLI_Channel[input->Data_Index], &N_SRAnalogLLI_Channel[input->Data_Index], VIRTUALPIN_TABLE[virtual_pin].device);
                Gpio__PinConfig(VIRTUALPIN_TABLE[virtual_pin].port,VIRTUALPIN_TABLE[virtual_pin].pin,INPUT_ANALOG);
                retval = PASS;
            }

            SRData__UpdateLong(&data->Sum, &n_data->Sum, 0);
            SRData__UpdateShort(&data->Value, &n_data->Value, ATOD_INVALID_CONVERSION);
            SRData__UpdateShort(&data->Last_Sample, &n_data->Last_Sample,  ATOD_INVALID_CONVERSION);
            SRData__UpdateShort(&data->Raw_Sample, &n_data->Raw_Sample, ATOD_INVALID_CONVERSION);
            SRData__UpdateShort(&data->Higher_Value, &n_data->Higher_Value, 0);
            SRData__UpdateShort(&data->Lower_Value, &n_data->Lower_Value , ATOD_INVALID_CONVERSION);
            SRData__UpdateByte(&data->Sample_Counter, &n_data->Sample_Counter, 0);
            SRData__UpdateByte(&data->Fast_Sample_Counter, &n_data->Fast_Sample_Counter, 0);
            SRData__UpdateByte(&data->Sync_Counter, &n_data->Sync_Counter, 0);
            SRData__UpdateByte(&data->Fast_Sync_Counter, &n_data->Fast_Sync_Counter, 0);
        }
    }

    return (retval);

}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief	Handler for SRAnalogLLI execution
 */
void SRAnalogLLI__Handler(void)
{
	BOOL_TYPE retval;
	uint8 index;

	retval = (BOOL_TYPE) (SRData__CheckByte(&SRAnalogLLI_Allocation, &N_SRAnalogLLI_Allocation) == SRDATA_OK);
	retval &= (BOOL_TYPE) (SRData__CheckArrayNonBlocking((void *) &SRAnalogLLI_Compensation, (void *) &N_SRAnalogLLI_Compensation, sizeof(SRAnalogLLI_Compensation)) == SRDATA_OK);
    retval &= (BOOL_TYPE) (Atod__EngineClassBCheck() == PASS);

	for (index = 0; index < HBL_LLI_NUM_SRANALOG; index++)
	{
	    uint8 channel;

		retval &= (BOOL_TYPE) (SRData__CheckArrayNonBlocking((void *) &SRAnalogLLI_Data[index],(void *) &N_SRAnalogLLI_Data[index], sizeof(SRAnalogLLI_Data[0])) == SRDATA_OK);
		retval &= (BOOL_TYPE) (SRData__CheckByte(&SRAnalogLLI_Channel[index], &N_SRAnalogLLI_Channel[index]) == SRDATA_OK);
		channel = SRAnalogLLI_Channel[index];
		if (channel != 0xFF)
		{
		    uint8 gpio_index = FindCheckGpio(channel);
		    if (gpio_index < ELEMENTS_IN_ARRAY(CHECK_PIN_TABLE))
		    {
	            retval &= (BOOL_TYPE) (Atod__ChannelClassBCheck((ATOD_CHANNEL_DEF) channel, CHECK_PIN_TABLE[gpio_index].Gpio_Port, CHECK_PIN_TABLE[gpio_index].Gpio_Pin) == PASS);
		    }
		    else
		    {
		        retval = FALSE;
		    }
		}

		if (retval == FALSE)
		{
			SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, 0, index);
			break;
		}
	}
    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_SRANALOG_CHECKS);
    #endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief		Get function to provide value of an SRAnalogLLI instance
 * @param input		pointer to HBL_LLI_TYPE structure instance
 * @return	pointer to Value
 */
void * SRAnalogLLI__Get(HBL_LLI_TYPE * input)
{
    if (input->Data_Index < SRAnalogLLI_Allocation)
    {
        return ((void *)&SRAnalogLLI_Data[input->Data_Index].Value);
    }
    else
    {
        return (NULL);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief	Process function to read on ZC Peak angle (90, 270)
 * @param input		pointer to HBL_LLI_TYPE structure instance
 * @param peak_polarity		TRUE for 90, FALSE for 270
 * @return	TRUE if a new sample is ready, FALSE otherwise
 */
BOOL_TYPE SRAnalogLLI__PeakProcess(HBL_LLI_TYPE * input, BOOL_TYPE peak_polarity)
{
    BOOL_TYPE retval = FALSE;

	if ((SRData__CheckByte(&SRAnalogLLI_Allocation, &N_SRAnalogLLI_Allocation) == SRDATA_OK) &&
			(input->Data_Index < SRAnalogLLI_Allocation))
	{
	    SRANALOGLLI_PARAM_TYPE *param_ptr = (SRANALOGLLI_PARAM_TYPE *) (input->Parameter_Ptr.Data); //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
	    BOOL_TYPE acflag = FALSE;

        if ((param_ptr != NULL) &&
			(param_ptr->Config.Ac_Line != 0))
        {
            acflag = TRUE;
        }

        if (acflag == peak_polarity)
        {
            retval = AnalogInputDoRead(input, SRANALOGLLI_SINGLE_READ, FALSE);
        }
    }
	else
	{
		SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, input->LLI_Handler_Index, input->Data_Index);
	}
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Process function to read on ZC Peak angle (90, 270)
 * @param input     pointer to HBL_LLI_TYPE structure instance
 * @param peak_polarity     TRUE for 90, FALSE for 270
 * @return  TRUE if a new sample is ready, FALSE otherwise
 */
BOOL_TYPE SRAnalogLLI__PeakToPeakProcess(HBL_LLI_TYPE * input, BOOL_TYPE peak_polarity)
{
    BOOL_TYPE retval = FALSE;

    if ((SRData__CheckByte(&SRAnalogLLI_Allocation, &N_SRAnalogLLI_Allocation) == SRDATA_OK) &&
            (input->Data_Index < SRAnalogLLI_Allocation))
    {
        SRANALOGLLI_READ_TYPE read_type = (peak_polarity == TRUE) ? SRANALOGLLI_HI_PEAK_READ : SRANALOGLLI_LO_PEAK_READ;
        retval = AnalogInputDoRead(input, read_type, FALSE);
    }
    else
    {
        SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, input->LLI_Handler_Index, input->Data_Index);
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief	Process function to read on ZC Peak angle (90, 270)
 * @param input		pointer to HBL_LLI_TYPE structure instance
 * @return	PASS if a new sample is ready, FAIL otherwise
 */
BOOL_TYPE SRAnalogLLI__AsynchProcess(HBL_LLI_TYPE * input)
{
    BOOL_TYPE retval = FALSE;

	if ((SRData__CheckByte(&SRAnalogLLI_Allocation, &N_SRAnalogLLI_Allocation) == SRDATA_OK) &&
		(input->Data_Index < SRAnalogLLI_Allocation))
    {
		retval = AnalogInputDoRead(input, SRANALOGLLI_SINGLE_READ, FALSE);
    }
	else
	{
		SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, input->LLI_Handler_Index, input->Data_Index);
	}

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Process function to read Analog value in 1ms Handler
 * @param input     pointer to HBL_LLI_TYPE structure instance
 * @return  PASS if a new sample is ready, FAIL otherwise
 */
BOOL_TYPE SRAnalogLLI__OneMsPeakToPeakProcess(HBL_LLI_TYPE * input)
{
    BOOL_TYPE retval = FALSE;

    if ((SRData__CheckByte(&SRAnalogLLI_Allocation, &N_SRAnalogLLI_Allocation) == SRDATA_OK) &&
       (input->Data_Index < SRAnalogLLI_Allocation))
    {
        retval = AnalogInputDoRead(input, SRANALOGLLI_FAST_READ, TRUE);
    }
    else
    {
        SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, input->LLI_Handler_Index, input->Data_Index);
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Process function to read SRAnalogLLI value in 1ms Handler, returning Max value on 90 degrees, minimum on 270
 * @param input     pointer to HBL_LLI_TYPE structure instance
 * @return  PASS if a new sample is ready, FAIL otherwise
 */
BOOL_TYPE SRAnalogLLI__OneMsPeakProcess(HBL_LLI_TYPE * input)
{
    BOOL_TYPE retval = FALSE;

    if ((SRData__CheckByte(&SRAnalogLLI_Allocation, &N_SRAnalogLLI_Allocation) == SRDATA_OK) &&
        (input->Data_Index < SRAnalogLLI_Allocation))
    {
        retval = AnalogInputDoRead(input, SRANALOGLLI_FAST_READ, FALSE);
    }
    else
    {
        SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, input->LLI_Handler_Index, input->Data_Index);
    }
    return (retval);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief		Algorithm to update and filter a new sample
 * @param input		pointer to HBL_LLI_TYPE structure instance
 * @param status
 * @return	PASS if a new sample is ready, FAIL otherwise
 */
static BOOL_TYPE AnalogInputDoRead(HBL_LLI_TYPE * input, SRANALOGLLI_READ_TYPE read_type, BOOL_TYPE is_peak_to_peak)
{
    BOOL_TYPE ret_value = FALSE;
    SRANALOGLLI_DATA_TYPE *data = &SRAnalogLLI_Data[input->Data_Index];
    SRANALOGLLI_DATA_TYPE *n_data = &N_SRAnalogLLI_Data[input->Data_Index];

	if (SRData__CheckArrayNonBlocking((void *) data, (void *) n_data, sizeof(SRAnalogLLI_Data[0])) == SRDATA_OK)
	{
        uint8 virtual_pin = input->Pin_Ptr[0];

        if (VIRTUALPIN_TABLE[virtual_pin].pin_config == (uint8) INPUT_ANALOG)
        {
            SRANALOGLLI_PARAM_TYPE *param_ptr = (void *)input->Parameter_Ptr.Data;
            uint8 filter_value = 1;
            uint8 n_samples = 1;

            uint16 new_ad_sample = Atod__Convert((ATOD_CHANNEL_DEF) VIRTUALPIN_TABLE[virtual_pin].device, (ATOD_CHANNEL_RESOLUTION_DEF) VIRTUALPIN_TABLE[virtual_pin].param0);
            uint16 new_raw_sample = new_ad_sample;

            if (param_ptr != NULL)
            {
                n_samples = 1 << (param_ptr->Config.N_Read);
                filter_value = 1 << (param_ptr->Config.Filter_Reading);
            }

            if((read_type == SRANALOGLLI_FAST_READ) &&
                (new_ad_sample != ATOD_INVALID_CONVERSION))
            {
                uint8 fast_max_sample_count;
                uint8 one_ms_filter = (SRANALOG_ONEMS_FIXED_DELTA_FILTER == 0) ? filter_value : SRANALOG_ONEMS_FIXED_DELTA_FILTER;
                if((one_ms_filter > 1) &&
                    (FAIL == AnalogInputCheckSample(new_ad_sample, data->Raw_Sample, one_ms_filter)))
                {
                    SRData__UpdateByte(&data->Fast_Sync_Counter, &n_data->Fast_Sync_Counter, data->Fast_Sync_Counter+1);

                    if (data->Fast_Sync_Counter < SRANALOGLLI_FILTER_SAMPLES)
                    {
                        new_ad_sample = data->Raw_Sample;
                    }
                    else
                    {
                        SRData__UpdateByte(&data->Fast_Sync_Counter, &n_data->Fast_Sync_Counter, 0);
                    }
                }
                else
                {
                    SRData__UpdateByte(&data->Fast_Sync_Counter, &n_data->Fast_Sync_Counter, 0);
                }

                // Store the maximum Value acquired during the NUMBER_OF_A2D_SAMPLES samples
                if(new_ad_sample > data->Higher_Value)
                {
                   SRData__UpdateShort(&data->Higher_Value, &n_data->Higher_Value, new_ad_sample);
                }

                // Store the minimum Value acquired during the NUMBER_OF_A2D_SAMPLES samples
                if(new_ad_sample < data->Lower_Value)
                {
                    SRData__UpdateShort(&data->Lower_Value, &n_data->Lower_Value, new_ad_sample);
                }

                // increment counter

                SRData__UpdateByte(&data->Fast_Sample_Counter, &n_data->Fast_Sample_Counter, data->Fast_Sample_Counter+1);
                fast_max_sample_count = (SRANALOG_ONEMS_NUM_READS > 0) ? SRANALOG_ONEMS_NUM_READS : n_samples;

                if( data->Fast_Sample_Counter >= fast_max_sample_count)
                {
                    if(is_peak_to_peak == TRUE)
                    {
                      new_ad_sample = (data->Higher_Value - data->Lower_Value);
                    }
                    else if(param_ptr->Config.Ac_Line == TRUE)
                    {
                      new_ad_sample = data->Higher_Value;
                    }
                    else
                    {
                      new_ad_sample = data->Lower_Value;
                    }
                    SRData__UpdateByte(&data->Fast_Sample_Counter, &n_data->Fast_Sample_Counter, 0);
                    SRData__UpdateShort(&data->Lower_Value, &n_data->Lower_Value, ATOD_INVALID_CONVERSION);
                    SRData__UpdateShort(&data->Higher_Value, &n_data->Higher_Value, 0);
                }
                else
                {
                    new_ad_sample = ATOD_INVALID_CONVERSION;
                }
            }

            if((is_peak_to_peak == TRUE) &&
                (new_ad_sample != ATOD_INVALID_CONVERSION) &&
                ((read_type == SRANALOGLLI_HI_PEAK_READ) ||
                    (read_type == SRANALOGLLI_LO_PEAK_READ)) )
            {
                if(read_type == SRANALOGLLI_HI_PEAK_READ)
                {
                    SRData__UpdateShort(&data->Higher_Value, &n_data->Higher_Value, new_ad_sample);
                }
                else
                {
                    SRData__UpdateShort(&data->Lower_Value, &n_data->Lower_Value, new_ad_sample);
                }

                if((data->Higher_Value != 0) &&
                    (data->Lower_Value != ATOD_INVALID_CONVERSION))
                {
                    if(data->Higher_Value > data->Lower_Value)
                    {
                        new_ad_sample = data->Higher_Value - data->Lower_Value;

                    }
                    else
                    {
                        new_ad_sample = data->Lower_Value - data->Higher_Value;
                    }
                    SRData__UpdateShort(&data->Lower_Value, &n_data->Lower_Value, ATOD_INVALID_CONVERSION);
                    SRData__UpdateShort(&data->Higher_Value, &n_data->Higher_Value, 0);
                }
                else
                {
                    new_ad_sample = ATOD_INVALID_CONVERSION;
                }
            }

                SRData__UpdateShort(&data->Raw_Sample, &n_data->Raw_Sample , new_raw_sample);

            // If Compensation reference is set, apply compensation algorithm
            if ((SRAnalogLLI_Compensation.Reference_Value > 0) &&
                (new_ad_sample != ATOD_INVALID_CONVERSION) &&
                (param_ptr->Config.Compensation != 0) &&
                (input->Parameter_Ptr.Length > 2))
            {
                uint16 max_sample;
                new_ad_sample = DoCompensation(param_ptr, new_ad_sample);

                switch(VIRTUALPIN_TABLE[virtual_pin].param0)
                {
                    case ATOD_RESOLUTION_04BITS:
                        max_sample = 0x000F;
                        break;
                    case ATOD_RESOLUTION_08BITS:
                        max_sample = 0x00FF;
                        break;
                    case ATOD_RESOLUTION_10BITS:
                        max_sample = 0x03FF;
                        break;
                    case ATOD_RESOLUTION_12BITS:
                        max_sample = 0x0FFF;
                        break;
                    case ATOD_RESOLUTION_14BITS:
                        max_sample = 0x3FFF;
                        break;
                    default:
                        max_sample = 0xFFFE;
                }
                if(new_ad_sample > max_sample)
                {
                    new_ad_sample = max_sample;
                }
            }

            if(read_type == SRANALOGLLI_FAST_READ)
            {
                #if(SRANALOG_ONEMS_FIXED_DELTA_FILTER == 0)
                // This check skips Sample Delta filtering on OneMs readings - it's performed at 1ms level
                    filter_value = 1;
                #endif
                #if(SRANALOG_ONEMS_NUM_READS == 0)
                    // If N_Samples is used on 1ms readings, force it to 1
                    n_samples = 1;
                #elif(SRANALOG_ONEMS_NUM_READS_DIVIDER > 1)
                    // Applies SRANALOG_ONEMS_NUM_READS_DIVIDER into N_Samples, if set
                    if(n_samples > SRANALOG_ONEMS_NUM_READS_DIVIDER)
                    {
                        n_samples /= SRANALOG_ONEMS_NUM_READS_DIVIDER;
                    }
                    else
                    {
                        n_samples = 1;
                    }
                #endif
            }

            // Delta filter of a new sample compared to last one
            if ((new_ad_sample != ATOD_INVALID_CONVERSION) &&
                (filter_value > 1))
            {
                if (FAIL == AnalogInputCheckSample(new_ad_sample, data->Last_Sample, filter_value))
                {
                    if (data->Sync_Counter == 0)
                    {
                        SRData__UpdateShort(&data->Last_Sample, &n_data->Last_Sample, new_ad_sample);
                    }
                    else
                    {
                        SRData__UpdateByte(&data->Sync_Counter, &n_data->Sync_Counter, 0);
                    }
                    SRData__UpdateByte(&data->Sample_Counter, &n_data->Sample_Counter, 0);
                    SRData__UpdateLong(&data->Sum, &n_data->Sum, 0);
                    new_ad_sample = ATOD_INVALID_CONVERSION;
                }
                else
                {
                    SRData__UpdateShort(&data->Last_Sample, &n_data->Last_Sample, new_ad_sample);
                    if (data->Sync_Counter < SRANALOGLLI_FILTER_SAMPLES)
                    {
                        new_ad_sample = ATOD_INVALID_CONVERSION;
                        SRData__UpdateByte(&data->Sync_Counter, &n_data->Sync_Counter, data->Sync_Counter+1);
                    }
                }
            }

            // Process new valid sample and update average (if ready)
            if (new_ad_sample != ATOD_INVALID_CONVERSION)
            {
                SRData__UpdateLong(&data->Sum, &n_data->Sum, data->Sum + new_ad_sample);
                SRData__UpdateByte(&data->Sample_Counter, &n_data->Sample_Counter, data->Sample_Counter + 1);

                if (data->Sample_Counter >= n_samples)
                {
                    SRData__UpdateShort(&data->Value, &n_data->Value, data->Sum / n_samples);
                    SRData__UpdateLong(&data->Sum, &n_data->Sum, 0);
                    SRData__UpdateByte(&data->Sample_Counter, &n_data->Sample_Counter, 0);
                    ret_value = TRUE;
                }

                // If it's compensation reference, check and updates
                if  ((param_ptr->Config.Reference != 0) &&
                        (input->Parameter_Ptr.Length > 2))
                {
                    UpdateReference(param_ptr, new_ad_sample);
                }
            }
        }
	}
	else
	{
		SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, input->LLI_Handler_Index, input->Data_Index);
	}


    return (ret_value);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief		Check new sample with filter value
 * @param sample new AD sample read
 * @param ref last AD value refrence
 * @param delta maximum delta between new and last values
 * @return PASS if new sample differs from reference less than delta, FAIL otherwise
 */
static PASS_FAIL_TYPE AnalogInputCheckSample(uint16 sample, uint16 ref, uint8 delta)
{
    PASS_FAIL_TYPE ret_value;

    ret_value = PASS;

    if (ref != ATOD_INVALID_CONVERSION)
    {
        if (sample > (ref + delta))
        {
            ret_value = FAIL;
        }

        if (ref > delta)
        {
            if (sample < (ref - delta))
            {
                ret_value = FAIL;
            }
        }
    }

    return (ret_value);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief		Compensation algorithm
 * @param input_param	Configuration parameters related to a channel
 * @param ad_value		AD Value to compensate
 * @return AD Value compensated; ad_value if not applied
 */
static uint16 DoCompensation(SRANALOGLLI_PARAM_TYPE *input_param, uint16 ad_value)
{
	sint32 temp_value = ad_value;

	if (SRData__CheckArrayNonBlocking(&SRAnalogLLI_Compensation, &N_SRAnalogLLI_Compensation, sizeof(SRAnalogLLI_Compensation)) == SRDATA_OK)
	{
		if (SRAnalogLLI_Compensation.Reference_Value > SRAnalogLLI_Compensation.Nominal_Offset)
		{
			temp_value -= (sint32) input_param->Config_param.Compensated.Comp_Pre_Offset;
			temp_value -= ENDIANU16_STON(input_param->Config_param.Compensated.Zero_Level_Ref);
			temp_value *= (SRAnalogLLI_Compensation.Nominal_Value - SRAnalogLLI_Compensation.Nominal_Offset);
			temp_value /= (SRAnalogLLI_Compensation.Reference_Value - SRAnalogLLI_Compensation.Nominal_Offset);
			temp_value += ENDIANU16_STON(input_param->Config_param.Compensated.Zero_Level_Ref);
			temp_value += (sint32) input_param->Config_param.Compensated.Comp_Post_Offset;

			if (temp_value < 0)
			{
				temp_value = 0;
			}
		}
	}
	else
	{
		SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, 0xFF, 0xFF);
	}

	return((uint16) temp_value);

}

/**
 * @brief		Check and update reference value
 * @param input_param	Configuration parameters related to a channel
 * @param ad_value		AD Value of reference read
 */
static void UpdateReference(SRANALOGLLI_PARAM_TYPE *input_param, uint16 advalue)
{
	if (SRData__CheckArrayNonBlocking(&SRAnalogLLI_Compensation, &N_SRAnalogLLI_Compensation,
		sizeof(SRAnalogLLI_Compensation)) == SRDATA_OK)
	{
		uint16 min_val = ENDIANU16_STON(input_param->Config_param.Reference.Min);
		uint16 max_val = ENDIANU16_STON(input_param->Config_param.Reference.Max);

		// If a reference Value is set, applies an hysteresis to Min and Max thresholds
		if (SRAnalogLLI_Compensation.Reference_Value > 0)
		{
			if (min_val > input_param->Config_param.Reference.Hysteresis)
			{
				min_val -= input_param->Config_param.Reference.Hysteresis;
			}
			else
			{
				min_val = 0;
			}

			max_val += input_param->Config_param.Reference.Hysteresis;
		}

		// Check and update reference values
		if ( (advalue >= min_val) &&
				(advalue <= max_val) )
		{
			SRData__UpdateShort(&SRAnalogLLI_Compensation.Reference_Value, &N_SRAnalogLLI_Compensation.Reference_Value, advalue);
			SRData__UpdateShort(&SRAnalogLLI_Compensation.Nominal_Value, &N_SRAnalogLLI_Compensation.Nominal_Value, ENDIANU16_STON(input_param->Config_param.Reference.Nominal));
			SRData__UpdateByte((void *) &SRAnalogLLI_Compensation.Nominal_Offset,(void *) &N_SRAnalogLLI_Compensation.Nominal_Offset, input_param->Config_param.Reference.Pre_Offset);
		}
		else
		{
			SRData__UpdateShort(&SRAnalogLLI_Compensation.Reference_Value, &N_SRAnalogLLI_Compensation.Reference_Value, 0);
		}
	}
	else
	{
		SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, 0xFF, 0xFF);
	}

}

uint8 FindCheckGpio(uint8 channel)
{
#if (SRANALOG_EXTERNAL_CHECK_GPIO_FEATURE == ENABLED)
    uint8 index;

    for (index = 0; index < ELEMENTS_IN_ARRAY(CHECK_PIN_TABLE); index++)
    {
        if(CHECK_PIN_TABLE[index].Atod_Channel == channel)
        {
            // Found index - break the loop
            break;
        }
    }
    return index;

#else
    return 0;
#endif
}

#endif

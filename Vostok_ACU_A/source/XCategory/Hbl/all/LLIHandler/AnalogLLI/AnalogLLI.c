/**
 *  @file       AnalogLLI.c
 *
 *  @brief      Module for AnalogLLI readings
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
 *              Optional Features - if SR- counterparts are defined in SRSystemConfig_prm.h and those are not, SR- values will be used as their default:
 *              ANALOGLLI_FILTER_SAMPLES (default 3) is debounce counter when Delta filter is enabled
 *              ANALOG_ONEMS_NUM_READS (default 25) set number of 1ms reading before providing each Peak to Peak / Peak values
 *                  if set to 0, N_Reads is used as parameter for 1ms readings - no further filters will be applied to result
 *              ANALOG_ONEMS_NUM_READS_DIVIDER (default 1) can be used to divide N_Reads when a new sample comes out during 1ms readings (if SRANALOG_ONEMS_NUM_READS > 0)
 *              ANALOG_ONEMS_FIXED_DELTA_FILTER (default 1) applies a fixed maximum/minimum Delta filter between 1ms reads.
 *                  If set to 0, setting file Delta parameter is used for that purpose and it will be disabled on sample result.
 *                  If set to 1, Delta filter on 1ms reads is disabled.
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/24 11:27:57EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "AnalogLLI.h"

#if (HBL_LLI_NUM_ANALOG > 0)
#include "VirtualPin.h"
#include "Atod.h"
#include "string.h"

#ifndef ANALOG_EXTERNAL_CHECK_GPIO_FEATURE
#define ANALOG_EXTERNAL_CHECK_GPIO_FEATURE          DISABLED
#endif

#ifndef ANALOG_ONEMS_NUM_READS
    #ifdef SRANALOG_ONEMS_NUM_READS
        #define ANALOG_ONEMS_NUM_READS              SRANALOG_ONEMS_NUM_READS
    #else
        #define ANALOG_ONEMS_NUM_READS              25
    #endif
#endif

#ifndef ANALOG_ONEMS_NUM_READS_DIVIDER
    #ifdef SRANALOG_ONEMS_NUM_READS_DIVIDER
        #define ANALOG_ONEMS_NUM_READS_DIVIDER      SRANALOG_ONEMS_NUM_READS_DIVIDER
    #else
        #define ANALOG_ONEMS_NUM_READS_DIVIDER         1
    #endif
#endif

#ifndef ANALOG_ONEMS_FIXED_DELTA_FILTER
    #ifdef SRANALOG_ONEMS_FIXED_DELTA_FILTER
        #define ANALOG_ONEMS_FIXED_DELTA_FILTER     SRANALOG_ONEMS_FIXED_DELTA_FILTER
    #else
        #define ANALOG_ONEMS_FIXED_DELTA_FILTER     1
    #endif
#endif

#ifndef ANALOGLLI_FILTER_SAMPLES
    #ifdef SRANALOGLLI_FILTER_SAMPLES
        #define ANALOGLLI_FILTER_SAMPLES            SRANALOGLLI_FILTER_SAMPLES
    #else
        #define ANALOGLLI_FILTER_SAMPLES            3
    #endif
#endif

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
PACKED typedef struct
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
} ANALOGLLI_DATA_TYPE;

// AnalogLLI Parameter table
typedef struct
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
} ANALOGLLI_PARAM_TYPE;

typedef struct
{
	uint16 Reference_Value;
	uint16 Nominal_Value;
	sint8 Nominal_Offset;
} ANALOGLLI_COMP_TYPE;

typedef enum
{
    ANALOGLLI_SINGLE_READ,
    ANALOGLLI_FAST_READ,
    ANALOGLLI_HI_PEAK_READ,
    ANALOGLLI_LO_PEAK_READ,
} ANALOGLLI_READ_TYPE;
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static ANALOGLLI_DATA_TYPE AnalogLLI_Data[HBL_LLI_NUM_ANALOG];
static ANALOGLLI_COMP_TYPE Compensation;
static uint8 AnalogLLI_Allocation;

static BOOL_TYPE AnalogInputDoRead(HBL_LLI_TYPE * input, ANALOGLLI_READ_TYPE read_type, BOOL_TYPE is_peak_to_peak);
static PASS_FAIL_TYPE AnalogInputCheckSample(uint16 sample, uint16 ref, uint8 delta);
static uint16 DoCompensation(ANALOGLLI_PARAM_TYPE *input_param, uint16 ad_value);
static void UpdateReference(ANALOGLLI_PARAM_TYPE *input_param, uint16 advalue);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief	Initializes the AnalogLLI module and its variables
 */
void AnalogLLI__Initialize(void)
{
    memset(&AnalogLLI_Data,0,sizeof(AnalogLLI_Data));
    AnalogLLI_Allocation = 0;
    Compensation.Reference_Value = ATOD_INVALID_CONVERSION;
    Compensation.Nominal_Value = ATOD_INVALID_CONVERSION;
    Compensation.Nominal_Offset = 0;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief	Checks if there is free space and reserve a new instance
 * @return	index of AnalogLLI instance, 0xFF if all HBL_LLI_NUM_ANALOG are already allocated
 */
uint8 AnalogLLI__Allocate(void)
{
    uint8 retval;
    retval = 0xFF;
    if (AnalogLLI_Allocation < HBL_LLI_NUM_ANALOG)
    {
        retval = AnalogLLI_Allocation;
        AnalogLLI_Allocation++;

    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      Initializes a new instance of AnalogLLI
 * @param input		pointer to HBL_LLI_TYPE structure reserved for that instance
 * @param status
 */
PASS_FAIL_TYPE AnalogLLI__InitializeInstance(HBL_LLI_TYPE *input)
{
    ANALOGLLI_DATA_TYPE *data;
    PASS_FAIL_TYPE retval;
    uint8 virtual_pin;

    retval = FAIL;

    if (input->Data_Index < AnalogLLI_Allocation)
    {
        virtual_pin = input->Pin_Ptr[0];

        if (VIRTUALPIN_TABLE[virtual_pin].pin_config == (uint8) INPUT_ANALOG)
        {
            Gpio__PinConfig(VIRTUALPIN_TABLE[virtual_pin].port,VIRTUALPIN_TABLE[virtual_pin].pin,INPUT_ANALOG);

            retval = PASS;
        }

        data = &AnalogLLI_Data[input->Data_Index];
        data->Sum = 0;
        data->Value = ATOD_INVALID_CONVERSION;
        data->Last_Sample = ATOD_INVALID_CONVERSION;
        data->Raw_Sample = ATOD_INVALID_CONVERSION;
        data->Higher_Value = 0;
        data->Lower_Value = ATOD_INVALID_CONVERSION;
        data->Sample_Counter = 0;
        data->Fast_Sample_Counter = 0;
        data->Sync_Counter = 0;
        data->Fast_Sync_Counter = 0;
    }

    return (retval);

}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Get function to provide value of an AnalogLLI instance
 *
 * @param   input		pointer to HBL_LLI_TYPE structure instance
 *
 * @return	pointer to a uint16 which contains the specified input atod count
 * @return  NULL is the input parameter is invalid
 */
void * AnalogLLI__Get(HBL_LLI_TYPE * input)
{
    if (input->Data_Index < AnalogLLI_Allocation)
    {
        return ((void *)&AnalogLLI_Data[input->Data_Index].Value);
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
 * @param status
 * @param peak_polarity		TRUE for 90, FALSE for 270
 * @return	TRUE if a new sample is ready, FALSE otherwise
 */
BOOL_TYPE AnalogLLI__PeakProcess(HBL_LLI_TYPE * input, BOOL_TYPE peak_polarity)
{
    BOOL_TYPE retval = FALSE;

    if (input->Data_Index < AnalogLLI_Allocation)
    {
    	ANALOGLLI_PARAM_TYPE *param_ptr = (void *) input->Parameter_Ptr.Data;
        BOOL_TYPE acflag = FALSE;

        if ((param_ptr != NULL) &&
        	(param_ptr->Config.Ac_Line != 0))
        {
            acflag = TRUE;
        }

        if (acflag == peak_polarity)
        {
            retval = AnalogInputDoRead(input, ANALOGLLI_SINGLE_READ, FALSE);
        }
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief	Process peak to peak function on ZC Peak angles (90, 270)
 * @param input		pointer to HBL_LLI_TYPE structure instance
 * @param status
 * @param peak_polarity		TRUE for 90, FALSE for 270
 * @return	TRUE if a new sample is ready, FALSE otherwise
 */
BOOL_TYPE AnalogLLI__PeakToPeakProcess(HBL_LLI_TYPE * input, BOOL_TYPE peak_polarity)
{
    BOOL_TYPE retval = FALSE;

    if (input->Data_Index < AnalogLLI_Allocation)
    {
        ANALOGLLI_READ_TYPE read_type = (peak_polarity == TRUE) ? ANALOGLLI_HI_PEAK_READ : ANALOGLLI_LO_PEAK_READ;
        retval = AnalogInputDoRead(input, read_type, TRUE);
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief	Process function to read on ZC Peak angle (90, 270)
 * @param input		pointer to HBL_LLI_TYPE structure instance
 * @param status
 * @return	PASS if a new sample is ready, FAIL otherwise
 */
BOOL_TYPE AnalogLLI__AsynchProcess(HBL_LLI_TYPE * input)
{
    BOOL_TYPE retval = FALSE;

    if (input->Data_Index < AnalogLLI_Allocation)
    {
    	retval = AnalogInputDoRead(input, ANALOGLLI_SINGLE_READ, FALSE);
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Process function to read Analog value in 1ms Handler
 * @param input     pointer to HBL_LLI_TYPE structure instance
 * @param status
 * @return  PASS if a new sample is ready, FAIL otherwise
 */
BOOL_TYPE AnalogLLI__OneMsPeakToPeakProcess(HBL_LLI_TYPE * input)
{
    BOOL_TYPE retval = FALSE;

    if (input->Data_Index < AnalogLLI_Allocation)
    {
        retval = AnalogInputDoRead(input, ANALOGLLI_FAST_READ, TRUE);
    }
    return (retval);
}

// ---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Process function to read Analog value in 1ms Handler, returning Max value on 90 degrees, minimum on 270
 * @param input     pointer to HBL_LLI_TYPE structure instance
 * @param status
 * @return  PASS if a new sample is ready, FAIL otherwise
 */
BOOL_TYPE AnalogLLI__OneMsPeakProcess(HBL_LLI_TYPE * input)
{
    BOOL_TYPE retval = FALSE;

    if (input->Data_Index < AnalogLLI_Allocation)
    {
        retval = AnalogInputDoRead(input, ANALOGLLI_FAST_READ, FALSE);
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
static BOOL_TYPE AnalogInputDoRead(HBL_LLI_TYPE * input, ANALOGLLI_READ_TYPE read_type, BOOL_TYPE is_peak_to_peak)
{
    uint8 virtual_pin = input->Pin_Ptr[0];
    BOOL_TYPE ret_value = FALSE;

    if (VIRTUALPIN_TABLE[virtual_pin].pin_config == (uint8) INPUT_ANALOG)
    {
        ANALOGLLI_PARAM_TYPE *param_ptr = (void *)input->Parameter_Ptr.Data;
        ANALOGLLI_DATA_TYPE *data = &AnalogLLI_Data[input->Data_Index];
        uint8 filter_value = 1;
        uint8 n_samples = 1;
        uint16 new_ad_sample = Atod__Convert((ATOD_CHANNEL_DEF) VIRTUALPIN_TABLE[virtual_pin].device, (ATOD_CHANNEL_RESOLUTION_DEF) VIRTUALPIN_TABLE[virtual_pin].param0);
        uint16 new_raw_sample = new_ad_sample;

        if(param_ptr != NULL)
        {
            n_samples = 1 << (param_ptr->Config.N_Read);
            filter_value = 1 << (param_ptr->Config.Filter_Reading);
        }

        if((read_type == ANALOGLLI_FAST_READ) &&
            (new_ad_sample != ATOD_INVALID_CONVERSION))
        {
            uint8 fast_max_sample_count = (ANALOG_ONEMS_NUM_READS > 0) ? ANALOG_ONEMS_NUM_READS : n_samples;
            uint8 one_ms_filter = (ANALOG_ONEMS_FIXED_DELTA_FILTER == 0) ? filter_value : ANALOG_ONEMS_FIXED_DELTA_FILTER;
            if((one_ms_filter > 1) &&
				(FAIL == AnalogInputCheckSample(new_ad_sample, data->Raw_Sample, one_ms_filter)))
            {
                data->Fast_Sync_Counter++;

                if (data->Fast_Sync_Counter < ANALOGLLI_FILTER_SAMPLES)
                {
                    new_ad_sample = data->Raw_Sample;
                }
                else
                {
                    data->Fast_Sync_Counter = 0;
                }
            }
            else
            {
                data->Fast_Sync_Counter = 0;
            }

            // Store the maximum Value acquired during the NUMBER_OF_A2D_SAMPLES samples
            if(new_ad_sample > data->Higher_Value)
            {
               data->Higher_Value = new_ad_sample;
            }

            // Store the minimum Value acquired during the NUMBER_OF_A2D_SAMPLES samples
            if(new_ad_sample < data->Lower_Value)
            {
                data->Lower_Value = new_ad_sample;
            }

            // increment counter

            data->Fast_Sample_Counter++;

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
                data->Fast_Sample_Counter = 0;
                data->Lower_Value = ATOD_INVALID_CONVERSION;
                data->Higher_Value = 0;
            }
            else
            {
                new_ad_sample = ATOD_INVALID_CONVERSION;
            }
        }

        if((is_peak_to_peak == TRUE) &&
            (new_ad_sample != ATOD_INVALID_CONVERSION) &&
            ((read_type == ANALOGLLI_HI_PEAK_READ) ||
                (read_type == ANALOGLLI_LO_PEAK_READ)))
        {
            if(read_type == ANALOGLLI_HI_PEAK_READ)
            {
                data->Higher_Value = new_ad_sample;
            }
            else
            {
                data->Lower_Value = new_ad_sample;
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
                data->Lower_Value = ATOD_INVALID_CONVERSION;
                data->Higher_Value = 0;
            }
            else
            {
                new_ad_sample = ATOD_INVALID_CONVERSION;
            }
        }

        data->Raw_Sample = new_raw_sample;


        // If Compensation reference is set, apply compensation algorithm
        if ((Compensation.Reference_Value != ATOD_INVALID_CONVERSION) &&
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

        if(read_type == ANALOGLLI_FAST_READ)
        {
            #if(ANALOG_ONEMS_FIXED_DELTA_FILTER == 0)
            // This check skips Sample Delta filtering on OneMs readings - it's performed at 1ms level
                filter_value = 1;
            #endif
            #if(ANALOG_ONEMS_NUM_READS == 0)
                // If N_Samples is used on 1ms readings, force it to 1
                n_samples = 1;
            #elif(ANALOG_ONEMS_NUM_READS_DIVIDER > 1)
                // Applies ANALOG_ONEMS_NUM_READS_DIVIDER into N_Samples, if set
                if(n_samples > ANALOG_ONEMS_NUM_READS_DIVIDER)
                {
                    n_samples /= ANALOG_ONEMS_NUM_READS_DIVIDER;
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
                    data->Last_Sample = new_ad_sample;
                }
                else
                {
                    data->Sync_Counter = 0;
                }
                data->Sample_Counter = 0;
                data->Sum = 0;
                new_ad_sample = ATOD_INVALID_CONVERSION;
            }
            else
            {
                data->Last_Sample = new_ad_sample;
                if (data->Sync_Counter < ANALOGLLI_FILTER_SAMPLES)
                {
                    new_ad_sample = ATOD_INVALID_CONVERSION;
                    data->Sync_Counter++;
                }
            }
        }

        // Process new valid sample and update average (if ready)
        if (new_ad_sample != ATOD_INVALID_CONVERSION)
        {
            data->Sum += new_ad_sample;
            data->Sample_Counter++;

            if (data->Sample_Counter >= n_samples)
            {
                data->Value = data->Sum / n_samples;
                data->Sum = 0;
                data->Sample_Counter = 0;
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
static uint16 DoCompensation(ANALOGLLI_PARAM_TYPE *input_param, uint16 ad_value)
{
	sint32 temp_value;

	temp_value = ad_value;

	if (Compensation.Reference_Value > Compensation.Nominal_Offset)
	{
		temp_value -= (sint32) input_param->Config_param.Compensated.Comp_Pre_Offset;
		temp_value -= ENDIANU16_STON(input_param->Config_param.Compensated.Zero_Level_Ref);
		temp_value *= (Compensation.Nominal_Value - Compensation.Nominal_Offset);
		temp_value /= (Compensation.Reference_Value - Compensation.Nominal_Offset);
		temp_value += ENDIANU16_STON(input_param->Config_param.Compensated.Zero_Level_Ref);
		temp_value += (sint32) input_param->Config_param.Compensated.Comp_Post_Offset;

		if (temp_value < 0)
		{
			temp_value = 0;
		}
	}

	return((uint16) temp_value);

}

/**
 * @brief		Check and update reference value
 * @param input_param	Configuration parameters related to a channel
 * @param ad_value		AD Value of reference read
 */
static void UpdateReference(ANALOGLLI_PARAM_TYPE *input_param, uint16 advalue)
{
	uint16 min_val = ENDIANU16_STON(input_param->Config_param.Reference.Min);
	uint16 max_val = ENDIANU16_STON(input_param->Config_param.Reference.Max);

	// If a reference value is set, applies an hysteresis to Min and Max thresholds
	if (Compensation.Reference_Value != ATOD_INVALID_CONVERSION)
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
		Compensation.Reference_Value = advalue;
		Compensation.Nominal_Value = ENDIANU16_STON(input_param->Config_param.Reference.Nominal);
		Compensation.Nominal_Offset = input_param->Config_param.Reference.Pre_Offset;
	}
	else
	{
		Compensation.Reference_Value = ATOD_INVALID_CONVERSION;
	}

}

#endif

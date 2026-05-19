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
 *  $Header:    PulseCounterLLI.c 1.9 2016/06/22 15:48:11EDT KAVEH RAZZAGHI (RAZZAK) Exp  $
 *
 *  @copyright  Copyright 2016-$Date: 2016/06/22 15:48:11EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "PulseCounterLLI.h"

#include "C_Extensions.h"

#if (HBL_LLI_NUM_PULSECOUNTER > 0)
#include "VirtualPin.h"
#include "Gpio.h"
#include "string.h"
#include "SystemTimers.h"
#include "Timer.h"


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#define		NOISE_THRESHOLD				3
#define		MAX_PULSE_COUNTER			65535

typedef enum
{
    PULSECOUNTERLLI_POLARITY_POSITIVE,
    PULSECOUNTERLLI_POLARITY_NEGATIVE
} PULSECOUNTERLLI_PULSE_POLARITY_TYPE;

PACKED typedef struct
{
    uint16      Unused                      : 15;
    uint16      Inverted                    : 1 ;
    uint16      Time_Base;
    uint16      Maximum_Pulse_Duration;
    uint16      Minimum_Pulse_Duration;
} PULSECOUNTERLLI_PARAMETERS_TYPE;

PACKED typedef struct
{
    uint16                  Pulse_Count;            //!< Roll-Back Valid Pulse Counter
    uint16                  Pulse_Width;
    uint16                  Invalid_Pulse_Count;
    uint16                  Invalid_Pulse_Width;
    uint16                  Total_Pulse_Count;
    uint16					On_Counter;
    uint8					Off_Counter;
    BOOL_TYPE				OneMs_Handler_Enabled;
    TIMER_PARAMETER_TYPE    Timer_Parameter;
} PULSECOUNTERLLI_DATA_TYPE;

static PULSECOUNTERLLI_DATA_TYPE PulseCounterLLI_Data[HBL_LLI_NUM_PULSECOUNTER];
static uint8 PulseCounterLLI_Allocation;
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      It Initializes the module DigitalLowLevel and its variables
 */
void PulseCounterLLI__Initialize(void)
{
    memset(&PulseCounterLLI_Data,0,sizeof(PulseCounterLLI_Data));
    PulseCounterLLI_Allocation = 0;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @return
 */
uint8 PulseCounterLLI__Allocate(void)
{
    uint8 retval;

    retval = 0xFF;

    if (PulseCounterLLI_Allocation < HBL_LLI_NUM_PULSECOUNTER)
    {
        retval = PulseCounterLLI_Allocation;
        PulseCounterLLI_Allocation++;

    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module DigitalLowLevel and its variables
 *
 */
PASS_FAIL_TYPE PulseCounterLLI__InitializeInstance(HBL_LLI_TYPE * input)
{
	PASS_FAIL_TYPE ret_val = FAIL;

    if (input->Data_Index < PulseCounterLLI_Allocation)
    {
    	PULSECOUNTERLLI_DATA_TYPE *data_ptr = &PulseCounterLLI_Data[input->Data_Index];
    	PULSECOUNTERLLI_PARAMETERS_TYPE *param_ptr = (PULSECOUNTERLLI_PARAMETERS_TYPE*)(input->Parameter_Ptr.Data); //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
        uint8 virtual_pin = input->Pin_Ptr[0];
        uint32 min_pulse_us = param_ptr->Minimum_Pulse_Duration * param_ptr->Time_Base;

        if (min_pulse_us >= 5000)
        {
            Gpio__PinConfig(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin, INPUT_FLOAT);
            data_ptr->OneMs_Handler_Enabled = TRUE;
            ret_val = PASS;
        }
        else
        {
            TIMER_POLARITY_TYPE polarity = TIMER_POLARITY_ACTIVE_HIGH;
            TIMER_CAPTURE_EDGE_TYPE edge = TIMER_CAPTUREON_RISING_EDGE;

            data_ptr->OneMs_Handler_Enabled = FALSE;

            if (param_ptr->Inverted == PULSECOUNTERLLI_POLARITY_NEGATIVE)
            {
                polarity = TIMER_POLARITY_ACTIVE_LOW;
                edge = TIMER_CAPTUREON_FALLING_EDGE;
            }

            data_ptr->Timer_Parameter.Timer      = (TIMER_TYPE)VIRTUALPIN_TABLE[virtual_pin].device;
            data_ptr->Timer_Parameter.Channel    = (TIMER_CHANNEL_TYPE)VIRTUALPIN_TABLE[virtual_pin].param0;
            data_ptr->Timer_Parameter.Mode       = TIMER_MODE_INPUT_CAPTURE;
            data_ptr->Timer_Parameter.Port       = VIRTUALPIN_TABLE[virtual_pin].port;
            data_ptr->Timer_Parameter.Pin        = VIRTUALPIN_TABLE[virtual_pin].pin;
            data_ptr->Timer_Parameter.Pin_ReMap  = VIRTUALPIN_TABLE[virtual_pin].pin_remap;
            data_ptr->Timer_Parameter.Polarity   = polarity;

            ret_val = Timer__Config(&data_ptr->Timer_Parameter);
            ret_val &= Timer__CaptureStart(&data_ptr->Timer_Parameter,
                        TIMER_CAPTURE_PULSE,
                        edge);
        }
    }
    return (ret_val);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param input
 * @param data
 * @return
 */
void * PulseCounterLLI__GetCount(HBL_LLI_TYPE * input)
{
    if (input->Data_Index < PulseCounterLLI_Allocation)
    {
        return ((void *)&PulseCounterLLI_Data[input->Data_Index].Pulse_Count);
    }
    else
    {
        return (NULL);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param input
 * @param data
 * @return
 */
void * PulseCounterLLI__GetWidth(HBL_LLI_TYPE * input)
{
    if (input->Data_Index < PulseCounterLLI_Allocation)
    {
        return ((void *)&PulseCounterLLI_Data[input->Data_Index].Pulse_Width);
    }
    else
    {
        return (NULL);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This process gets detected pulse duration and checks if it matches valid pulse duration range
 * @param input	- Hbl LLI instance data structure
 * @return	TRUE if a new valid pulse is available, FALSE otherwise
 */
BOOL_TYPE PulseCounterLLI__ICProcess(HBL_LLI_TYPE * input)
{
	PULSECOUNTERLLI_PARAMETERS_TYPE *param_ptr = (PULSECOUNTERLLI_PARAMETERS_TYPE*)(input->Parameter_Ptr.Data); //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    BOOL_TYPE retval = FALSE;

    if ((input->Data_Index < PulseCounterLLI_Allocation)
    	&& (PulseCounterLLI_Data[input->Data_Index].OneMs_Handler_Enabled == FALSE))
    {
        PULSECOUNTERLLI_DATA_TYPE *data_ptr = &PulseCounterLLI_Data[input->Data_Index];
        TIMER_CAPTURE_EDGE_TYPE edge = TIMER_CAPTUREON_RISING_EDGE;
        uint32 pulse_duration = Timer__GetCaptureDuration(&data_ptr->Timer_Parameter);

        data_ptr->Total_Pulse_Count++;
        pulse_duration /= ENDIANU16_STON(param_ptr->Time_Base);

        //validate the pulse duration
        if ((pulse_duration >= ENDIANU16_STON(param_ptr->Minimum_Pulse_Duration)) &&
            (pulse_duration <= ENDIANU16_STON(param_ptr->Maximum_Pulse_Duration)) )
        {
            data_ptr->Pulse_Width = pulse_duration;
            data_ptr->Pulse_Count++;
            retval = TRUE;
        }
        else
        {
            data_ptr->Invalid_Pulse_Width = pulse_duration;
            data_ptr->Invalid_Pulse_Count++;
        }

    	edge = TIMER_CAPTUREON_FALLING_EDGE;

    	Timer__CaptureStart(&PulseCounterLLI_Data[input->Data_Index].Timer_Parameter, TIMER_CAPTURE_PULSE, edge);
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This process polls Gpio to detect correct pulse, and validate against pulse range
 * @param input	- Hbl LLI instance data structure
 * @return	TRUE if a new valid pulse is available, FALSE otherwise
 */
BOOL_TYPE PulseCounterLLI__OneMsProcess(HBL_LLI_TYPE * input)
{
	PULSECOUNTERLLI_PARAMETERS_TYPE *param_ptr = (PULSECOUNTERLLI_PARAMETERS_TYPE*)(input->Parameter_Ptr.Data); //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    BOOL_TYPE retval = FALSE;

    if ((input->Data_Index < PulseCounterLLI_Allocation)
    	&& (PulseCounterLLI_Data[input->Data_Index].OneMs_Handler_Enabled == TRUE))
    {
        PULSECOUNTERLLI_DATA_TYPE *data_ptr = &PulseCounterLLI_Data[input->Data_Index];
        uint8 virtual_pin = input->Pin_Ptr[0];
        BOOL_TYPE pinval;

        pinval = Gpio__PinRead(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin);

        // Inverted logic flag enabled - swap TRUE to FALSE
        if (param_ptr->Inverted)
        {
            pinval ^= TRUE;
        }

        if (pinval)
        {
        	// Any Off (0) samples counted before will be considered noise
        	uint32 new_counter = data_ptr->On_Counter + data_ptr->Off_Counter + 1;
        	if (new_counter > MAX_PULSE_COUNTER)
        	{
        		new_counter = MAX_PULSE_COUNTER;
        	}
        	data_ptr->On_Counter = (uint16) new_counter;
        	data_ptr->Off_Counter = 0;
        }
        else if (data_ptr->On_Counter > 0)
        {
    		data_ptr->Off_Counter++;
        	if (data_ptr->Off_Counter > NOISE_THRESHOLD)
        	{
        		uint32 pulse_duration = (data_ptr->On_Counter * 1000) / param_ptr->Time_Base;
            	data_ptr->On_Counter = 0;
            	data_ptr->Off_Counter = 0;
                data_ptr->Total_Pulse_Count++;

                if (pulse_duration > MAX_PULSE_COUNTER)
                {
                	pulse_duration = MAX_PULSE_COUNTER;
                }

                //validate the pulse duration
                if ((pulse_duration >= ENDIANU16_STON(param_ptr->Minimum_Pulse_Duration)) &&
                    (pulse_duration <= ENDIANU16_STON(param_ptr->Maximum_Pulse_Duration)) )
                {
                    data_ptr->Pulse_Width = (uint16) pulse_duration;
                    data_ptr->Pulse_Count++;
                    retval = TRUE;
                }
                else
                {
                    data_ptr->Invalid_Pulse_Width = (uint16) pulse_duration;
                    data_ptr->Invalid_Pulse_Count++;
                }
        	}
        }
        else
        {
        	data_ptr->On_Counter = 0;
        	data_ptr->Off_Counter = 0;
        }
    }

    return (retval);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#endif

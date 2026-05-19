/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @details This module uses 2 methods for capturing the input frequency
 *              1. Continuous Conversion Mode -
 *                 Conversion is performed continuously. Valid for frequency 2 to 200Hz.
 *                 Sampling period is unused in this mode
 *
 *              2. Discrete Conversion Mode -
 *                 Conversion is performed discretely on every sampling period. Valid for frequency above 200Hz. This method avoids overriding of timer interrupt
 *                 Sampling period is derived from the setting file which is multiple of 5ms
 *
 *
 *           Filter - Filter is used only if the number of readings are more than 1
 *           This method uses the IIR filter for Period and Duty calculation.
 *
 *
 *  Note- 1. Valid Frequency range for the detection is 2Hz to 65KHz
 *        2. During testing its observed that the input capture Period readings are accurate for up to 10Khz frequency
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: InputCaptureLLI.c 1.9 2015/08/10 15:48:11EDT Dasar Hanumanth (DASARHB) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/08/10 15:48:11EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "InputCaptureLLI.h"

#if (HBL_LLI_NUM_INPUTCAPTURE > 0)
#include "Filter.h"
#include "Micro.h"
#include "Millis.h"
#include "Timer.h"
#include "VirtualPin.h"
#include <string.h>

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#define INPUTCAPTURELLI_AUTO_FEATURE_MIN_FREQUENCY          2
#define INPUTCAPTURELLI_AUTO_FEATURE_MAX_FREQUENCY          150

#define INPUTCAPTURELLI_AUTO_FEATURE_FREQUENCY_HYS          30 //SSS_ frequency hysteresis value[Hz]
#define INPUTCAPTURELLI_AUTO_FEATURE_DOWN_PERIOD_HYS       (1000000 / (INPUTCAPTURELLI_AUTO_FEATURE_MAX_FREQUENCY))
#define INPUTCAPTURELLI_AUTO_FEATURE_UP_PERIOD_HYS         (1000000 / (INPUTCAPTURELLI_AUTO_FEATURE_MAX_FREQUENCY-INPUTCAPTURELLI_AUTO_FEATURE_FREQUENCY_HYS))
#define MAX_INVALID_SAMPLE_COUNT                            255
#define DEFAULT_SAMPLING_MODE                               1 //continue mode=1 don't change

PACKED typedef struct
{
    uint8 N_Read:3;
    uint8 Unused_1:5;
    uint8 Delta:4;
    uint8 Unused_2:4;
    uint8 Time_Out;
    uint8 Sampling_Time;
    uint16 Minimum_Period;
    uint16 Maximum_Period;
    uint16 Minimum_Duty;
    uint16 Maximum_Duty;
    uint16 Filter_Period;
    uint16 Filter_Duty;
    uint8 Base_Period;
} INPUTCAPTURELLI_PARAMETERS_TYPE;

typedef struct
{
    INPUTCAPTURE_PUBLIC_TYPE Public_Data;
    IIR_FILTER_MEASURE_TYPE Private_Period;
    IIR_FILTER_MEASURE_TYPE Private_Duty;
    BOOL_TYPE Continue_mode;
    BOOL_TYPE Filter_Initialized;
    uint8 Private_Valid_Sample_Count;
    uint8 Private_Invalid_Sample_Count;
    uint8 Private_Current_Count;
    MILLIS_TIMER_TYPE Timer_Sampling;
    MILLIS_TIMER_TYPE Timer_Timeout;
    TIMER_PARAMETER_TYPE Timer_Parameter;
}INPUTCAPTURE_DATA_TYPE;

static INPUTCAPTURE_DATA_TYPE InputCaptureLLI_Data[HBL_LLI_NUM_INPUTCAPTURE];
static uint8 InputCaptureLLI_Allocation;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      It Initializes the module DigitalLowLevel and its variables
 */
void InputCaptureLLI__Initialize(void)
{
    memset(&InputCaptureLLI_Data,0,sizeof(InputCaptureLLI_Data));
    InputCaptureLLI_Allocation = 0;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @return
 */
uint8 InputCaptureLLI__Allocate(void)
{
    uint8 retval;

    retval = 0xFF;

    if (InputCaptureLLI_Allocation < HBL_LLI_NUM_INPUTCAPTURE)
    {
        retval = InputCaptureLLI_Allocation;
        InputCaptureLLI_Allocation++;

    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module DigitalLowLevel and its variables
 *
 */
PASS_FAIL_TYPE InputCaptureLLI__InitializeInstance(HBL_LLI_TYPE * input)
{
    PASS_FAIL_TYPE ret_status = FAIL;

    if (input->Data_Index < InputCaptureLLI_Allocation)
    {
        MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
        uint8 delta_expanded;
        INPUTCAPTURE_DATA_TYPE *data = (INPUTCAPTURE_DATA_TYPE *)&InputCaptureLLI_Data[input->Data_Index]; //lint !e929 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
        INPUTCAPTURELLI_PARAMETERS_TYPE *parameters = (INPUTCAPTURELLI_PARAMETERS_TYPE *)input->Parameter_Ptr.Data; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
        uint8 virtual_pin = input->Pin_Ptr[0];

        delta_expanded = 1 << parameters->Delta;
        // If Delta is set to 1, fallback to N.Reads - backward compatibility
        if (delta_expanded <= 1)
        {
            delta_expanded = 1 << parameters->N_Read;
        }

        data->Continue_mode = TRUE;
        data->Filter_Initialized = FALSE;

        //Initialize the filters
        data->Private_Period.Inverted_Alpha = delta_expanded;
        data->Private_Duty.Inverted_Alpha   = delta_expanded;

        data->Timer_Parameter.Timer       =   (TIMER_TYPE)VIRTUALPIN_TABLE[virtual_pin].device;
        data->Timer_Parameter.Channel     =   (TIMER_CHANNEL_TYPE)VIRTUALPIN_TABLE[virtual_pin].param0;
        data->Timer_Parameter.Mode        =   TIMER_MODE_INPUT_CAPTURE;
        data->Timer_Parameter.Port        =   VIRTUALPIN_TABLE[virtual_pin].port;
        data->Timer_Parameter.Pin         =   VIRTUALPIN_TABLE[virtual_pin].pin;
        data->Timer_Parameter.Pin_ReMap   =   VIRTUALPIN_TABLE[virtual_pin].pin_remap;
        data->Timer_Parameter.Polarity    =   TIMER_POLARITY_ACTIVE_HIGH;

        //Configure Continuous reading
        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
        ret_status =  Timer__Config(&data->Timer_Parameter);
        ret_status |= Timer__CaptureStart(&data->Timer_Parameter, TIMER_CAPTURE_CYCLE, TIMER_CAPTUREON_RISING_EDGE);
        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();

        //Init the timeout and start it
        Millis__Reset(&data->Timer_Timeout);
        Millis__Start(&data->Timer_Timeout);

        //Init the sampling for discrete mode
        Millis__Reset(&data->Timer_Sampling);
    }

    return (ret_status);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param   input
 * @return  pointer to the INPUTCAPTURE_PUBLIC_TYPE for the specified input
 * @return  NULL passed input was invalid
 */
void * InputCaptureLLI__Get(HBL_LLI_TYPE * input)
{
    if (input->Data_Index < InputCaptureLLI_Allocation)
    {
        return ((void *)&InputCaptureLLI_Data[input->Data_Index]);
    }
    else
    {
        return (NULL);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @details - Controls the Discrete Sampling mode and Timeout
 * @param input
 * @param status
 * @return
 */
BOOL_TYPE InputCaptureLLI__AsynchProcess(HBL_LLI_TYPE * input)
{
    BOOL_TYPE ret_val;

    ret_val = FALSE;

    INPUTCAPTURELLI_PARAMETERS_TYPE *parameters;
    INPUTCAPTURE_DATA_TYPE *data;

    if (input->Data_Index < InputCaptureLLI_Allocation)
    {
        MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
        data = (INPUTCAPTURE_DATA_TYPE *)&InputCaptureLLI_Data[input->Data_Index]; //lint !e929 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
        parameters = (INPUTCAPTURELLI_PARAMETERS_TYPE *)(input->Parameter_Ptr.Data);        //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]

        //Enable Sampling only if Auto or Continuous reading is set
        if ((data->Continue_mode == FALSE) &&
            (data->Timer_Parameter.Mode == TIMER_MODE_DISABLE))
        {
            if (Millis__ReachedTarget(&data->Timer_Sampling, parameters->Sampling_Time * 5) == TRUE)
            {
                Millis__Start(&data->Timer_Sampling);
                // Enable timer as Input Capture
                MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
                data->Timer_Parameter.Mode = TIMER_MODE_INPUT_CAPTURE;
                Timer__Config(&data->Timer_Parameter);
                Timer__CaptureStart(&data->Timer_Parameter, TIMER_CAPTURE_CYCLE, TIMER_CAPTUREON_RISING_EDGE);
                MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
            }
        }

        //Re-Initialize the InputCapture peripheral if their is no signal received in the timeout period
        if (Millis__ReachedTarget(&data->Timer_Timeout, parameters->Time_Out * 5) == TRUE)
        {
            // Disable timer
            MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
            InputCaptureLLI_Data[input->Data_Index].Timer_Parameter.Mode = TIMER_MODE_DISABLE;
            Timer__Config(&InputCaptureLLI_Data[input->Data_Index].Timer_Parameter);
            MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
            // Reset filter
            data->Filter_Initialized = FALSE;
            data->Continue_mode = TRUE;
            //Restart reading of samples
            data->Private_Valid_Sample_Count                    = 0;
            data->Private_Invalid_Sample_Count                  = 0;
            data->Private_Current_Count                         = 0;
            //Refresh the Timeout timer
            Millis__Start(&data->Timer_Timeout);
            //Refresh sampling time
            Millis__Start(&data->Timer_Sampling);
            // Initialize the Timer module and start sampling
            MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
            InputCaptureLLI_Data[input->Data_Index].Timer_Parameter.Mode = TIMER_MODE_INPUT_CAPTURE;
            Timer__Config(&InputCaptureLLI_Data[input->Data_Index].Timer_Parameter);
            Timer__CaptureStart(&InputCaptureLLI_Data[input->Data_Index].Timer_Parameter, TIMER_CAPTURE_CYCLE, TIMER_CAPTUREON_RISING_EDGE);
            MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
        }

        ret_val = TRUE;
    }

    return(ret_val);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param input
 * @return
 */
BOOL_TYPE InputCaptureLLI__ICProcess(HBL_LLI_TYPE * input)
{
    INPUTCAPTURELLI_PARAMETERS_TYPE *parameters;
    INPUTCAPTURE_DATA_TYPE *data;
    BOOL_TYPE retval;
    uint32 captured_duration;
    uint16 captured_duty;
    uint8 n_read_expanded;


    retval = FALSE;

    if (input->Data_Index < InputCaptureLLI_Allocation)
    {
        MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
        data = (INPUTCAPTURE_DATA_TYPE *)&InputCaptureLLI_Data[input->Data_Index]; //lint !e929 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
        parameters = (INPUTCAPTURELLI_PARAMETERS_TYPE *)input->Parameter_Ptr.Data; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]

        //Read the current captured duration and duty
        captured_duration = Timer__GetCaptureDuration(&data->Timer_Parameter);
        captured_duty = Timer__GetCaptureDuty(&data->Timer_Parameter);

        //Refresh the Timeout timer
        Millis__Start(&data->Timer_Timeout);

        //Disable the Timer Input Capture if the Sampling Read is set
        if (data->Continue_mode == FALSE)
        {
            //Discrete Sampling Mode
            //Switch to Discrete sampling as Current captured duration is too small to do in continuous sampling
            //Disable the Timer and let control the sampling from the InputCaptureLLI__AsynchProcess()
            MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
            data->Timer_Parameter.Mode = TIMER_MODE_DISABLE;
            Timer__Config(&data->Timer_Parameter);
            MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();

        }

        /*SSS latch_check to changing mode*/
        if (((captured_duration) < (INPUTCAPTURELLI_AUTO_FEATURE_DOWN_PERIOD_HYS)) && (data->Continue_mode)) /*(f > MAX_FREQUENCY*)*/
        {
            Millis__Start(&data->Timer_Sampling);
            //Change the mode and stop InputCapture
            MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
            data->Continue_mode = FALSE;
            data->Timer_Parameter.Mode = TIMER_MODE_DISABLE;
            Timer__Config(&data->Timer_Parameter);
            MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
        }
        else if(((captured_duration) > (INPUTCAPTURELLI_AUTO_FEATURE_UP_PERIOD_HYS))&& !(data->Continue_mode))/*((f > MAX_FREQUENCY-HYSTERESIS*))*/
        {
            //Change the mode and restart InputCapture
            MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
            data->Continue_mode = TRUE;
            data->Timer_Parameter.Mode = TIMER_MODE_INPUT_CAPTURE;
            Timer__Config(&data->Timer_Parameter);
            Timer__CaptureStart(&data->Timer_Parameter, TIMER_CAPTURE_CYCLE, TIMER_CAPTUREON_RISING_EDGE);
            MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
        }
        else
        {
            /*all ok*/
        }

        //Expand the number of reads
        n_read_expanded = 1 << parameters->N_Read;

        data->Private_Current_Count++;

        //validate the Cycle Period
        if ((captured_duration >= (ENDIANU16_STON(parameters->Minimum_Period) << 3)) &&
            (captured_duration <= (ENDIANU16_STON(parameters->Maximum_Period) << 3)))
        {
            //validate the captured_duty
            if ((captured_duty >= ENDIANU16_STON(parameters->Minimum_Duty)) &&
                (captured_duty <= ENDIANU16_STON(parameters->Maximum_Duty)))
            {
                data->Private_Valid_Sample_Count++;

                //scale the period based on the base
                captured_duration = captured_duration / parameters->Base_Period;
                if (data->Filter_Initialized == TRUE)
                {
                    Filter__Update((void *)(&(data->Private_Period)), (FILTER_DATA_TYPE)captured_duration, FILTER_IIR);
                    Filter__Update((void *)(&(data->Private_Duty)), (FILTER_DATA_TYPE)captured_duty, FILTER_IIR);
                }
                else
                {
                    Filter__Initialize((void *)(&(data->Private_Period)), (FILTER_DATA_TYPE) captured_duration, FILTER_IIR);
                    Filter__Initialize((void *)(&(data->Private_Duty)), (FILTER_DATA_TYPE) captured_duty , FILTER_IIR);
                    data->Filter_Initialized = TRUE;
                }
            }
            else
            {
                data->Private_Invalid_Sample_Count++;
            }
        }
        else
        {
            data->Private_Invalid_Sample_Count++;
        }

        //Check if there are too many invalid samples
        if(data->Private_Invalid_Sample_Count == MAX_INVALID_SAMPLE_COUNT)
        {
            //Force Timer_Timeout to DONE state
            Millis__Start(&data->Timer_Timeout);
            Millis__SetElapsed(&data->Timer_Timeout, parameters->Time_Out * 5);
        }

        //if its the new sampling
        if(data->Private_Valid_Sample_Count >= n_read_expanded)
        {
            //Copy previous sampling data
            data->Public_Data.Period                    = data->Private_Period.Filtered_Value;
            data->Public_Data.Duty                      = data->Private_Duty.Filtered_Value;
            data->Public_Data.Valid_Sample_Count        = data->Private_Valid_Sample_Count;
            data->Public_Data.InValid_Sample_Count      = data->Private_Invalid_Sample_Count;

            //new readings available
            data->Public_Data.Sequence_Count++;

            if (data->Public_Data.Sequence_Count == 0)
            {
                data->Public_Data.Sequence_Count++;
            }

            //Prepare for reading new sampling
            data->Private_Valid_Sample_Count                    = 0;
            data->Private_Invalid_Sample_Count                  = 0;
            data->Private_Current_Count                         = 0;

            //Data ready after the filter
            retval = TRUE;
        }
    }

    return (retval);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#endif

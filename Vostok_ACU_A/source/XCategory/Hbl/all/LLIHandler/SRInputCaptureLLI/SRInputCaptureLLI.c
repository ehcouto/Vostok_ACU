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
 *					List here all the applicable documents if needed. <tr>	
 *
 *  @defgroup	CLASS_B
 *
 *  $Header: SRInputCaptureLLI.c 1.9 2015/08/10 15:48:11EDT Dasar Hanumanth (DASARHB) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/08/10 15:48:11EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SRInputCaptureLLI.h"

#if (HBL_LLI_NUM_SRINPUTCAPTURE > 0)
#include "SRData.h"
#include "SRException.h"
#include "SRMicro.h"
#include "SRTaskSequence.h"
#include "SRTimerReference.h"
#include "VirtualPin.h"
#include "Timer.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define SRINPUTCAPTURELLI_AUTO_FEATURE_MIN_FREQUENCY          2
#define SRINPUTCAPTURELLI_AUTO_FEATURE_MAX_FREQUENCY          150

#define SRINPUTCAPTURELLI_AUTO_FEATURE_FREQUENCY_HYS          30
#define SRINPUTCAPTURELLI_AUTO_FEATURE_DOWN_PERIOD_HYS       (1000000 / (SRINPUTCAPTURELLI_AUTO_FEATURE_MAX_FREQUENCY))
#define SRINPUTCAPTURELLI_AUTO_FEATURE_UP_PERIOD_HYS         (1000000 / (SRINPUTCAPTURELLI_AUTO_FEATURE_MAX_FREQUENCY-SRINPUTCAPTURELLI_AUTO_FEATURE_FREQUENCY_HYS))
#define MAX_INVALID_SAMPLE_COUNT                             255

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
} SRINPUTCAPTURELLI_PARAMETERS_TYPE;

PACKED typedef struct
{
    SRINPUTCAPTURE_PUBLIC_TYPE Public_Data;
    uint32 Accumulator_Period;
    uint32 Accumulator_Duty;
    uint16 Filtered_Period;
    uint16 Filtered_Duty;
    BOOL_TYPE Continue_mode;
    BOOL_TYPE Filter_Initialized;
    uint8 Private_Valid_Sample_Count;
    uint8 Private_Invalid_Sample_Count;
    uint8 Private_Current_Count;
    uint32 Sampling_Timeout;
    uint32 Timeout;
    TIMER_PARAMETER_TYPE Timer_Parameter;
}SRINPUTCAPTURE_DATA_TYPE;

static SRINPUTCAPTURE_DATA_TYPE SRInputCaptureLLI_Data[HBL_LLI_NUM_SRINPUTCAPTURE];
static uint8 SRInputCaptureLLI_Allocation;

static SRINPUTCAPTURE_DATA_TYPE N_SRInputCaptureLLI_Data[HBL_LLI_NUM_SRINPUTCAPTURE];
static uint8 N_SRInputCaptureLLI_Allocation;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      It Initializes the module DigitalLowLevel and its variables
 */
void SRInputCaptureLLI__Initialize(void)
{
    SRData__Fill(SRInputCaptureLLI_Data, N_SRInputCaptureLLI_Data, 0, sizeof(SRInputCaptureLLI_Data));
    SRData__UpdateByte(&SRInputCaptureLLI_Allocation, &N_SRInputCaptureLLI_Allocation, 0);

#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_INIT, SRTASKSEQUENCE_APPL_INIT_SRINPUTCAPTURE_LLI);
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @return
 */
uint8 SRInputCaptureLLI__Allocate(void)
{
    uint8 retval;

    retval = 0xFF;

    if (SRData__CheckByte(&SRInputCaptureLLI_Allocation, &N_SRInputCaptureLLI_Allocation) == SRDATA_OK)
    {
        if (SRInputCaptureLLI_Allocation < HBL_LLI_NUM_SRINPUTCAPTURE)
        {
            retval = SRInputCaptureLLI_Allocation;
            SRData__UpdateByte(&SRInputCaptureLLI_Allocation, &N_SRInputCaptureLLI_Allocation, SRInputCaptureLLI_Allocation+1);
        }
    }
    else
    {
        SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, 0, 0xFF);
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module DigitalLowLevel and its variables
 *
 */
PASS_FAIL_TYPE SRInputCaptureLLI__InitializeInstance(HBL_LLI_TYPE * input)
{
    PASS_FAIL_TYPE ret_status = FAIL;

    if (SRData__CheckByte(&SRInputCaptureLLI_Allocation, &N_SRInputCaptureLLI_Allocation) == SRDATA_OK)
    {
        if (input->Data_Index < SRInputCaptureLLI_Allocation)
        {
            SRINPUTCAPTURE_DATA_TYPE *data = (SRINPUTCAPTURE_DATA_TYPE *)&SRInputCaptureLLI_Data[input->Data_Index]; //lint !e929 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
            SRINPUTCAPTURE_DATA_TYPE *n_data = (SRINPUTCAPTURE_DATA_TYPE *)&N_SRInputCaptureLLI_Data[input->Data_Index]; //lint !e929 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]

            if (SRData__CheckArrayNonBlocking(data, n_data, sizeof(SRInputCaptureLLI_Data[0])) == SRDATA_OK) //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
            {
                MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
                uint32 current_time = SRTimerReference__GetTimer();
                SRINPUTCAPTURELLI_PARAMETERS_TYPE *parameters = (SRINPUTCAPTURELLI_PARAMETERS_TYPE *)input->Parameter_Ptr.Data; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
                VIRTUALPIN_TYPE *virtual_pin_ptr = (void *) &VIRTUALPIN_TABLE[input->Pin_Ptr[0]];

                SRData__UpdateLong(&data->Timeout, &n_data->Timeout, current_time + (parameters->Time_Out * 5));
                SRData__UpdateLong(&data->Sampling_Timeout, &n_data->Sampling_Timeout, current_time + (parameters->Sampling_Time * 5));
                SRData__UpdateByte((void *) &data->Continue_mode, (void *) &n_data->Continue_mode, TRUE);
                SRData__UpdateByte((void *) &data->Filter_Initialized, (void *) &n_data->Filter_Initialized, FALSE);

                // Initialize Timer
                MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
                SRData__UpdateByte((void *) &data->Timer_Parameter.Timer, (void *) &n_data->Timer_Parameter.Timer, virtual_pin_ptr->device);
                SRData__UpdateByte((void *) &data->Timer_Parameter.Channel, (void *) &n_data->Timer_Parameter.Channel, virtual_pin_ptr->param0);
                SRData__UpdateByte((void *) &data->Timer_Parameter.Mode, (void *) &n_data->Timer_Parameter.Mode, TIMER_MODE_INPUT_CAPTURE);
                SRData__UpdateByte((void *) &data->Timer_Parameter.Port, (void *) &n_data->Timer_Parameter.Port, virtual_pin_ptr->port);
                SRData__UpdateByte((void *) &data->Timer_Parameter.Pin, (void *) &n_data->Timer_Parameter.Pin, virtual_pin_ptr->pin);
                SRData__UpdateByte((void *) &data->Timer_Parameter.Pin_ReMap, (void *) &n_data->Timer_Parameter.Pin_ReMap, virtual_pin_ptr->pin_remap);
                SRData__UpdateByte((void *) &data->Timer_Parameter.Polarity, (void *) &n_data->Timer_Parameter.Polarity, TIMER_POLARITY_ACTIVE_HIGH);

                //Configure Continuous reading
                ret_status =  Timer__Config(&data->Timer_Parameter);
                ret_status |= Timer__CaptureStart(&data->Timer_Parameter, TIMER_CAPTURE_CYCLE, TIMER_CAPTUREON_RISING_EDGE);
                ret_status &= Timer__ConfigCheck(&data->Timer_Parameter);
                MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
            }
        }
    }

    return (ret_status);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief	Handler for SRInputCaptureLLI execution; this call (and sequence) checks SRInputCaptureLLI
 * @detail
 */
void SRInputCaptureLLI__Handler(void)
{
    BOOL_TYPE retval;
    uint8 index;

    retval = (BOOL_TYPE) (SRData__CheckByte(&SRInputCaptureLLI_Allocation, &N_SRInputCaptureLLI_Allocation) == SRDATA_OK);
    for (index = 0; index < HBL_LLI_NUM_SRINPUTCAPTURE; index++)
    {
        retval &= (BOOL_TYPE) (SRData__CheckArrayNonBlocking(&SRInputCaptureLLI_Data[index], &N_SRInputCaptureLLI_Data[index],
            sizeof(SRInputCaptureLLI_Data[0])) == SRDATA_OK);
        if (retval == FALSE)
        {
            SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, 0, index);
            break;
        }
    }
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_SRINPUTCAPTURE_CHECKS);
#endif

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param input
 * @param data
 * @return
 */
void *SRInputCaptureLLI__Get(HBL_LLI_TYPE * input)
{
    void *ret_val;

    ret_val = NULL;

    if (input->Data_Index < SRInputCaptureLLI_Allocation)
    {
        ret_val = ((void *)&SRInputCaptureLLI_Data[input->Data_Index]);
    }

    return(ret_val);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param input
 * @param status
 * @return
 */
BOOL_TYPE SRInputCaptureLLI__AsynchProcess(HBL_LLI_TYPE * input)
{
    if ((SRData__CheckByte(&SRInputCaptureLLI_Allocation, &N_SRInputCaptureLLI_Allocation) == SRDATA_OK)
        && (input->Data_Index < SRInputCaptureLLI_Allocation))
    {
        uint32 current_time = SRTimerReference__GetTimer();

        SRINPUTCAPTURE_DATA_TYPE *data = (void *)&SRInputCaptureLLI_Data[input->Data_Index];
        SRINPUTCAPTURE_DATA_TYPE *n_data = (void *)&N_SRInputCaptureLLI_Data[input->Data_Index];
        BOOL_TYPE crc_check = (BOOL_TYPE) (SRData__CheckArrayNonBlocking(data, n_data, sizeof(SRInputCaptureLLI_Data[0])) == SRDATA_OK);

        if (crc_check)
        {
            SRINPUTCAPTURELLI_PARAMETERS_TYPE *parameters = (SRINPUTCAPTURELLI_PARAMETERS_TYPE *)(input->Parameter_Ptr.Data);        //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
            MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

            //Enable Sampling only if Auto or Continuous reading is set
            if ((data->Continue_mode == FALSE) &&
                (data->Timer_Parameter.Mode == TIMER_MODE_DISABLE))
            {
                if (((sint32) (current_time - data->Sampling_Timeout)) > 0)
                {
                    // Refresh sampling time
                    SRData__UpdateLong(&data->Sampling_Timeout, &n_data->Sampling_Timeout, current_time + (parameters->Sampling_Time * 5));

                    // Restart Timer in Input Capture mode
                    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
                    SRData__UpdateByte((void *) &data->Timer_Parameter.Mode, (void *) &n_data->Timer_Parameter.Mode, TIMER_MODE_INPUT_CAPTURE);
                    Timer__Config(&data->Timer_Parameter);
                    Timer__CaptureStart(&data->Timer_Parameter, TIMER_CAPTURE_CYCLE, TIMER_CAPTUREON_RISING_EDGE);
                    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
                }
            }

            if (((sint32) (current_time - data->Timeout)) > 0)
            {
                // Stop Input Capture mode
                MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
                SRData__UpdateByte((void *) &data->Timer_Parameter.Mode, (void *) &n_data->Timer_Parameter.Mode, TIMER_MODE_DISABLE);
                Timer__Config(&data->Timer_Parameter);
                MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
                // Reset filter
                SRData__UpdateByte((void *) &data->Filter_Initialized, (void *) &n_data->Filter_Initialized, FALSE);
                //Restart reading of samples
                SRData__UpdateByte(&data->Private_Valid_Sample_Count, &n_data->Private_Valid_Sample_Count, 0);
                SRData__UpdateByte(&data->Private_Invalid_Sample_Count, &n_data->Private_Invalid_Sample_Count, 0);
                SRData__UpdateByte(&data->Private_Current_Count, &n_data->Private_Current_Count, 0);
                //Refresh the Timeout
                SRData__UpdateLong(&data->Timeout, &n_data->Timeout, current_time + (parameters->Time_Out * 5));
                //Refresh sampling time
                SRData__UpdateLong(&data->Sampling_Timeout, &n_data->Sampling_Timeout, current_time + (parameters->Sampling_Time * 5));
                // Restart Timer in Input Capture mode
                MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
                SRData__UpdateByte((void *) &data->Timer_Parameter.Mode, (void *) &n_data->Timer_Parameter.Mode, TIMER_MODE_INPUT_CAPTURE);
                Timer__Config(&data->Timer_Parameter);
                Timer__CaptureStart(&data->Timer_Parameter, TIMER_CAPTURE_CYCLE, TIMER_CAPTUREON_RISING_EDGE);
                MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
            }
        }
    }

    return(FALSE);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param input
 * @return
 */
BOOL_TYPE SRInputCaptureLLI__ICProcess(HBL_LLI_TYPE * input)
{
    BOOL_TYPE retval = FALSE;

    if ((SRData__CheckByte(&SRInputCaptureLLI_Allocation, &N_SRInputCaptureLLI_Allocation) == SRDATA_OK)
        && (input->Data_Index < SRInputCaptureLLI_Allocation))
    {
        SRINPUTCAPTURE_DATA_TYPE *data = (void *)&SRInputCaptureLLI_Data[input->Data_Index];
        SRINPUTCAPTURE_DATA_TYPE *n_data = (void *)&N_SRInputCaptureLLI_Data[input->Data_Index];
        BOOL_TYPE crc_check = (BOOL_TYPE) (SRData__CheckArrayNonBlocking(data, n_data, sizeof(SRInputCaptureLLI_Data[0])) == SRDATA_OK);

        if (crc_check)
        {
            uint32 current_time = SRTimerReference__GetTimer();
            SRINPUTCAPTURELLI_PARAMETERS_TYPE *parameters = (SRINPUTCAPTURELLI_PARAMETERS_TYPE *)input->Parameter_Ptr.Data; //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
            uint32 captured_duration = 0;
            uint16 captured_duty = 0;
            MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

            // Check the current Timer configuration of the Timer Channel (Gpio pin).
            if ((Timer__ConfigCheck(&data->Timer_Parameter)) == PASS)
            {
                //Read the current captured duration and duty
                captured_duration = Timer__GetCaptureDuration(&data->Timer_Parameter);
                captured_duty = Timer__GetCaptureDuty(&data->Timer_Parameter);
            }
            else
            {
            	SRException__Queue(SREXCEPTION_TIMER_DATA_CORRUPTION, 0, 0xFF);
            }

            //Disable the Timer Input Capture if the Sampling Read is set
            if (data->Continue_mode == FALSE)
            {
                //Discrete Sampling Mode
                //Switch to Discrete sampling as Current captured duration is too small to do in continuous sampling
                //Disable the Timer and let control the sampling from the InputCaptureLLI__AsynchProcess()
                MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
                SRData__UpdateByte((void *) &data->Timer_Parameter.Mode, (void *) &n_data->Timer_Parameter.Mode, TIMER_MODE_DISABLE);
                Timer__Config(&data->Timer_Parameter);
                MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
            }


            /*Latch_check to changing mode*/
            if (((captured_duration) < (SRINPUTCAPTURELLI_AUTO_FEATURE_DOWN_PERIOD_HYS)) &&
                (data->Continue_mode)) /*(f > MAX_FREQUENCY*)*/
            {
                //Change the mode and stop InputCapture
                MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
                SRData__UpdateByte((void *) &data->Timer_Parameter.Mode, (void *) &n_data->Timer_Parameter.Mode, TIMER_MODE_DISABLE);
                Timer__Config(&data->Timer_Parameter);
                MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
                SRData__UpdateLong(&data->Sampling_Timeout, &n_data->Sampling_Timeout, current_time + (parameters->Sampling_Time * 5));
                SRData__UpdateByte((void *) &data->Continue_mode, (void *) &n_data->Continue_mode, FALSE);
            }
            else if(((captured_duration) > (SRINPUTCAPTURELLI_AUTO_FEATURE_UP_PERIOD_HYS))&&
                !(data->Continue_mode))/*((f > MAX_FREQUENCY-HYSTERESIS*))*/
            {
                //Change the mode and restart InputCapture
                SRData__UpdateByte((void *) &data->Continue_mode, (void *) &n_data->Continue_mode, TRUE);
                MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
                SRData__UpdateByte((void *) &data->Timer_Parameter.Mode, (void *) &n_data->Timer_Parameter.Mode, TIMER_MODE_INPUT_CAPTURE);
                Timer__Config(&data->Timer_Parameter);
                Timer__CaptureStart(&data->Timer_Parameter, TIMER_CAPTURE_CYCLE, TIMER_CAPTUREON_RISING_EDGE);
                MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
            }

            SRData__UpdateByte(&data->Private_Current_Count, &n_data->Private_Current_Count, data->Private_Current_Count+1);

            //validate the Cycle Period
            if ((captured_duration >= (ENDIANU16_STON(parameters->Minimum_Period) << 3)) &&
                (captured_duration <= (ENDIANU16_STON(parameters->Maximum_Period) << 3)) &&
                //validate the captured_duty
                (captured_duty >= ENDIANU16_STON(parameters->Minimum_Duty)) &&
                (captured_duty <= ENDIANU16_STON(parameters->Maximum_Duty)))
            {
                uint8 delta = parameters->Delta;
                // Use N_Read as delta if 1 for backward compatibility
                if(delta == 1)
                {
                    delta = parameters->N_Read;
                }

                SRData__UpdateByte(&data->Private_Valid_Sample_Count, &n_data->Private_Valid_Sample_Count, data->Private_Valid_Sample_Count+1);

                //scale the period based on the base
                captured_duration = captured_duration / parameters->Base_Period;

                if (data->Filter_Initialized == TRUE)
                {
                    uint32 temp_value;

                    // Update filtered value for Period
                    temp_value = data->Accumulator_Period;
                    temp_value += captured_duration;
                    temp_value -= data->Filtered_Period;
                    SRData__UpdateLong(&data->Accumulator_Period, &n_data->Accumulator_Period, temp_value);
                    if(delta > 1)
                    {
                        temp_value >>= (delta - 1);
                    }
                    if (delta > 0)
                    {
                        temp_value += 1;
                        temp_value >>= 1;
                    }
                    SRData__UpdateShort(&data->Filtered_Period, &n_data->Filtered_Period, (uint16) temp_value);

                    // Update filtered value for Period
                    temp_value = data->Accumulator_Duty;
                    temp_value += captured_duty;
                    temp_value -= data->Filtered_Duty;
                    SRData__UpdateLong(&data->Accumulator_Duty, &n_data->Accumulator_Duty, temp_value);
                    if(delta > 1)
                    {
                        temp_value >>= (delta - 1);
                    }
                    if (delta > 0)
                    {
                        temp_value += 1;
                        temp_value >>= 1;
                    }
                    SRData__UpdateShort(&data->Filtered_Duty, &n_data->Filtered_Duty, (uint16) temp_value);
                }
                else
                {
                    SRData__UpdateLong(&data->Accumulator_Period, &n_data->Accumulator_Period, captured_duration << delta);
                    SRData__UpdateShort(&data->Filtered_Period, &n_data->Filtered_Period, captured_duration);
                    SRData__UpdateLong(&data->Accumulator_Duty, &n_data->Accumulator_Duty, captured_duty << delta);
                    SRData__UpdateShort(&data->Filtered_Duty, &n_data->Filtered_Duty, captured_duty);
                    SRData__UpdateByte((void *) &data->Filter_Initialized, (void *) &n_data->Filter_Initialized, TRUE);
                }

                //Refresh the Timeout timer
                SRData__UpdateLong(&data->Timeout, &n_data->Timeout, current_time + (parameters->Time_Out * 5));
            }
            else
            {
                SRData__UpdateByte(&data->Private_Invalid_Sample_Count, &n_data->Private_Invalid_Sample_Count, data->Private_Invalid_Sample_Count+1);
            }

            //Check if there are too many invalid samples
            if(data->Private_Invalid_Sample_Count == MAX_INVALID_SAMPLE_COUNT)
            {
                //Force Timer_Timeout to DONE state
                SRData__UpdateLong(&data->Timeout, &n_data->Timeout, current_time);
            }

            //if its the new sampling
            if(data->Private_Valid_Sample_Count >= (1 << parameters->N_Read))
            {
                uint8 new_seq_count;
                //Copy previous sampling data
                SRData__UpdateShort(&data->Public_Data.Period, &n_data->Public_Data.Period, data->Filtered_Period);
                SRData__UpdateShort(&data->Public_Data.Duty, &n_data->Public_Data.Duty, data->Filtered_Duty);
                SRData__UpdateByte(&data->Private_Valid_Sample_Count, &n_data->Private_Valid_Sample_Count, data->Private_Valid_Sample_Count);
                SRData__UpdateByte(&data->Private_Invalid_Sample_Count, &n_data->Private_Invalid_Sample_Count, data->Private_Invalid_Sample_Count);

            //new readings available
                new_seq_count = data->Public_Data.Sequence_Count+1;
                if (new_seq_count == 0)
                {
                    new_seq_count++;
                }
                SRData__UpdateByte(&data->Public_Data.Sequence_Count, &n_data->Public_Data.Sequence_Count, new_seq_count);

                //Prepare for reading new sampling
                SRData__UpdateByte(&data->Private_Valid_Sample_Count, &n_data->Private_Valid_Sample_Count, 0);
                SRData__UpdateByte(&data->Private_Invalid_Sample_Count, &n_data->Private_Invalid_Sample_Count, 0);
                SRData__UpdateByte(&data->Private_Current_Count, &n_data->Private_Current_Count, 0);

                //Data ready after the filter
                retval = TRUE;
            }
        }
    }

    return (retval);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#endif

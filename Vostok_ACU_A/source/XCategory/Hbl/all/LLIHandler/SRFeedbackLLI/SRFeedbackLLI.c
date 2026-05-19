/**
 *  @file
 *
 *  @brief      SRFeedbackLLI module implementation
 *
 *  @details    This module implements the LLI Handler for SRFeedback Input, SRAC Input and SRZeroCross Input
 *              It's aligned with FeedbackLLI module, adding Class B requirements
 *
 *  @defgroup    CLASS_B
 *
 *  @section    Applicable_Documents
 *                    List here all the applicable documents if needed. <tr>
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/24 11:27:56EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SRFeedbackLLI.h"

#if (HBL_LLI_NUM_SRFEEDBACK > 0)
#include "Gpio.h"
#include "Micro.h"
#include "SRData.h"
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    #include "SRTaskSequence.h"
#endif
#include "VirtualPin.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define DEFAULT_NUM_SAMPLES                 16
#define MIN_NUM_SAMPLES                     8
#define LAST_FREQUENCY_EVENT_TIMEOUT        30
#define SAMPLES_NOISE_THRESHOLD             2

typedef struct
{
    uint8 N_Read:3;
    uint8 Unused_0:5;
    uint8 Unused_1:5;
    uint8 Pull_Up:1;
    uint8 Unused_2:1;
    uint8 Inverted:1;
} SRFEEDBACKLLI_PARAM_TYPE;

typedef struct
{
    // Buffer of Gpio samples collected
    uint16 Pin_Buffer;
    // Sampling Sequence counter - updated on each 1ms sample
    uint8 Sampling_Counter;
    // last Sampling Sequence counter reference of the last Asynch iteration
    uint8 Async_Sampling_Reference;
    // Frequency detection in progress
    BOOL_TYPE Is_Detecting_Frequency;
    // Sampling Sequence counter reference of the last frequency event detected
    uint8 Last_Frequency_Event_Reference;
    // Time (in ms) between last two frequency events detected
    uint8 Time_Between_Frequency_Events;
    // Counter of High / Low pin level event detected: it increments on a new High value event, decrements on a new Low value event
    sint8 Level_Counter;
    // Counter of valid Frequency event detected: it increments when valid Frequency events have been detected
    uint8 Frequency_Counter;
    // Counter of Frequency an in phase with ZeroCross (phase shift around +/- 90 degrees) event detected
    uint8 Phase_Counter;
    // Current Feedback LLI value (0, 1, 2, 3)
    SRFEEDBACKLLI_VALUE_ENUM_TYPE Value;
    // Current AC LLI value (0, 1)
    uint8 Ac_Is_Present;
    // Last phase shift (in ms) between Frequency signal and ZeroCross - 0 if channel or ZeroCross signals are not valid frequency ones
    uint8 Phase_Shift;
} SRFEEDBACKLLI_DATA_TYPE;

// FeedbackLLI data structure
static SRFEEDBACKLLI_DATA_TYPE SRFeedbackLLI_Data[HBL_LLI_NUM_SRFEEDBACK];
// ZeroCross / SRZeroCross instance index - if present
static uint8 SRZeroCross_Index;
// Number of FeedbackLLI instances allocated
static uint8 SRFeedbackLLI_Allocation;

// Complement Class B variables
static SRFEEDBACKLLI_DATA_TYPE N_SRFeedbackLLI_Data[HBL_LLI_NUM_SRFEEDBACK];
static uint8 N_SRZeroCross_Index;
static uint8 N_SRFeedbackLLI_Allocation;
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void UpdateLastFrequencyTimingEvents(SRFEEDBACKLLI_DATA_TYPE *data_ptr, SRFEEDBACKLLI_DATA_TYPE *n_data_ptr, uint8 new_frequency_reference);
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      It initialises the module SRFeedbackLLI and its variables
 */
void SRFeedbackLLI__Initialize(void)
{
    SRData__UpdateByte(&SRFeedbackLLI_Allocation, &N_SRFeedbackLLI_Allocation, 0);
    SRData__UpdateByte(&SRZeroCross_Index, &N_SRZeroCross_Index, HBL_INDEX_INVALID);
    SRData__Fill(SRFeedbackLLI_Data, N_SRFeedbackLLI_Data, 0, sizeof(SRFeedbackLLI_Data));

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_INIT, SRTASKSEQUENCE_APPL_INIT_SRFEEDBACK_LLI);
    #endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Checks if there is free space and reserve a new instance
 * @return  index of AnalogLLI instance, 0xFF if all HBL_LLI_NUM_SRFEEDBACK are already allocated
 */
uint8 SRFeedbackLLI__Allocate(void)
{
    uint8 retval = HBL_ALLOCATION_INVALID;

    (void) SRData__CheckByte(&SRFeedbackLLI_Allocation, &N_SRFeedbackLLI_Allocation);
    if(SRFeedbackLLI_Allocation < (uint8) HBL_LLI_NUM_SRFEEDBACK)
    {
        retval = SRFeedbackLLI_Allocation;
        SRData__UpdateByte(&SRFeedbackLLI_Allocation, &N_SRFeedbackLLI_Allocation, SRFeedbackLLI_Allocation + 1U);
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       It initialises an instance in the module SRFeedbackLLI and its variables
 * @param       input - Hbl LLI Data structure of the instance to process
 * @return      PASS if initialization succeeded
 *
 */

PASS_FAIL_TYPE SRFeedbackLLI__InitializeInstance(HBL_LLI_TYPE * input)
{
    PASS_FAIL_TYPE retval = FAIL;

    (void) SRData__CheckByte(&SRFeedbackLLI_Allocation, &N_SRFeedbackLLI_Allocation);

    if(input->Data_Index < SRFeedbackLLI_Allocation)
    {
        SRFEEDBACKLLI_DATA_TYPE *data = &SRFeedbackLLI_Data[input->Data_Index];
        SRFEEDBACKLLI_DATA_TYPE *n_data = &N_SRFeedbackLLI_Data[input->Data_Index];
        SRFEEDBACKLLI_PARAM_TYPE *feedback_param = (void *)input->Parameter_Ptr.Data;
        GPIO_PIN_MODE_TYPE type = INPUT_FLOAT;
        uint8 virtual_pin = input->Pin_Ptr[0];

        (void) SRData__CheckArray(data, n_data, sizeof(SRFEEDBACKLLI_DATA_TYPE));

        if((feedback_param != NULL) &&
            ((uint8) feedback_param->Pull_Up != 0U))
        {
            type = INPUT_PULLUP;
        }

        if(virtual_pin < VirtualPin__GetNumOf())
        {
            Gpio__PinConfig(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin, type);
            SRData__UpdateByte((void *) &data->Value,(void *) &n_data->Value, SRFEEDBACKLLI_INVALID);

            // Save SRZeroCrossLLI instance reference to check FeedbackLLI in/opposite phase
            if(input->LLI_ID == HBL_LLI_SRZEROCROSS)
            {
                (void) SRData__CheckByte(&SRZeroCross_Index, &N_SRZeroCross_Index);
                SRData__UpdateByte(&SRZeroCross_Index, &N_SRZeroCross_Index, input->Data_Index);
            }

            if(Gpio__CheckConfig(VIRTUALPIN_TABLE[virtual_pin].port,VIRTUALPIN_TABLE[virtual_pin].pin, type, FALSE))
            {
                retval = PASS;
            }
        }
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief    Handler for SRFeedbackLLI execution; this call (and sequence) checks both SRFeedbackLLI and SRAcLLI
 */
void SRFeedbackLLI__Handler(void)
{
    (void) SRData__CheckByte(&SRFeedbackLLI_Allocation, &N_SRFeedbackLLI_Allocation);
    (void) SRData__CheckByte(&SRZeroCross_Index, &N_SRZeroCross_Index);
    (void) SRData__CheckArrayNonBlocking(SRFeedbackLLI_Data, N_SRFeedbackLLI_Data, sizeof(SRFeedbackLLI_Data));

#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_SRFEEDBACK_CHECKS);
#endif

}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Provides the SRFeedbackLLI current value
 * @param       input - Hbl LLI Data structure of the instance to process
 * @return      pointer to Value
 */
void * SRFeedbackLLI__Get(HBL_LLI_TYPE * input)
{
    void *dataptr = NULL;

    (void)SRData__CheckByte(&SRFeedbackLLI_Allocation, &N_SRFeedbackLLI_Allocation);
    if (input->Data_Index < SRFeedbackLLI_Allocation)
    {
        (void) SRData__CheckByte((void *) &SRFeedbackLLI_Data[input->Data_Index].Value, (void *) &N_SRFeedbackLLI_Data[input->Data_Index].Value);
        dataptr = &SRFeedbackLLI_Data[input->Data_Index].Value;
    }
    return dataptr;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Provides the SRFeedbackLLI current value as AC Input
 * @param       input - Hbl LLI Data structure of the instance to process
 * @return      pointer to Ac_Is_Present
 */
void * SRFeedbackLLI__GetAcInput(HBL_LLI_TYPE * input)
{
    void *dataptr = NULL;

    (void) SRData__CheckByte(&SRFeedbackLLI_Allocation, &N_SRFeedbackLLI_Allocation);
    if (input->Data_Index < SRFeedbackLLI_Allocation)
    {
        (void) SRData__CheckByte(&SRFeedbackLLI_Data[input->Data_Index].Ac_Is_Present, &N_SRFeedbackLLI_Data[input->Data_Index].Ac_Is_Present);
        dataptr = &SRFeedbackLLI_Data[input->Data_Index].Ac_Is_Present;
    }
    return dataptr;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Executes SRFeedbackLLI 1ms sampling
 * @param       input - Hbl LLI Data structure of the instance to process
 * @return      FALSE    this process never updates LLI Sequence ID
 */
BOOL_TYPE SRFeedbackLLI__OneMsProcess(HBL_LLI_TYPE * input)
{
    (void) SRData__CheckByte(&SRFeedbackLLI_Allocation, &N_SRFeedbackLLI_Allocation);
    if (input->Data_Index < SRFeedbackLLI_Allocation)
    {
        SRFEEDBACKLLI_DATA_TYPE *data = &SRFeedbackLLI_Data[input->Data_Index];
        SRFEEDBACKLLI_DATA_TYPE *n_data = &N_SRFeedbackLLI_Data[input->Data_Index];
        SRFEEDBACKLLI_PARAM_TYPE *feedback_param = (void *) input->Parameter_Ptr.Data;
        uint8 virtual_pin = input->Pin_Ptr[0];
        GPIO_PIN_MODE_TYPE type = INPUT_FLOAT;

        // Check if Pull up configuration to match Gpio__CheckConfig
        if((feedback_param != NULL) &&
            ((uint8) feedback_param->Pull_Up != 0U))
        {
            type = INPUT_PULLUP;
        }

        if(Gpio__CheckConfig(VIRTUALPIN_TABLE[virtual_pin].port,VIRTUALPIN_TABLE[virtual_pin].pin, type, FALSE))
        {
            uint8 pin_value = (uint8) Gpio__PinRead(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin);
            uint16 pin_buffer;

            // Inverted logic flag enabled
            if((feedback_param != NULL) &&
                ((uint8) feedback_param->Inverted != 0U))
            {
                pin_value = (pin_value == 0U) ? 1U : 0U;
            }

            // Update pin buffer with a new sample
            (void) SRData__CheckShort(&data->Pin_Buffer, &n_data->Pin_Buffer);
            pin_buffer = data->Pin_Buffer;
            pin_buffer  <<= 1U;
            pin_buffer |= (uint16) pin_value & 0x01U;
            SRData__UpdateShort(&data->Pin_Buffer, &n_data->Pin_Buffer, pin_buffer);

            // Update sampling counter
            (void) SRData__CheckByte(&data->Sampling_Counter, &n_data->Sampling_Counter);
            SRData__UpdateByte(&data->Sampling_Counter, &n_data->Sampling_Counter, data->Sampling_Counter + 1U);
        }
    }
    return(FALSE);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Executes SRFeedbackLLI Asynchronous handler to update LLI value depending on 1ms sampling
 * @param       input - Hbl LLI Data structure of the instance to process
 * @return      TRUE if a new LLI value is available to update sequence ID, FALSE otherwise
 */
BOOL_TYPE SRFeedbackLLI__AsynchProcess(HBL_LLI_TYPE * input)
{
    BOOL_TYPE retval = FALSE;

    (void)SRData__CheckByte(&SRFeedbackLLI_Allocation, &N_SRFeedbackLLI_Allocation);
    if (input->Data_Index < SRFeedbackLLI_Allocation)
    {
        SRFEEDBACKLLI_DATA_TYPE *data = &SRFeedbackLLI_Data[input->Data_Index];
        SRFEEDBACKLLI_DATA_TYPE *n_data = &N_SRFeedbackLLI_Data[input->Data_Index];
        SRFEEDBACKLLI_PARAM_TYPE *feedback_param = (void *) input->Parameter_Ptr.Data;
        uint8 n_read = DEFAULT_NUM_SAMPLES;
        // Minimum number of samples to give priority between level (High / Low) or Frequency detection
        uint8 level_freq_threshold = (uint8) (DEFAULT_NUM_SAMPLES / 2);

        if(feedback_param != NULL)
        {
            uint8 param_read = 1U << ((uint8) feedback_param->N_Read);
            if (param_read <= (uint8) MIN_NUM_SAMPLES)
            {
                param_read = (uint8) MIN_NUM_SAMPLES;
            }
            n_read = param_read;
            level_freq_threshold = param_read / 2U;
        }

        (void) SRData__CheckArrayNonBlocking(data, n_data, sizeof(SRFeedbackLLI_Data[0]));
        // Update Feedback values if there are new samples coming from  1ms Process
        if(data->Async_Sampling_Reference != data->Sampling_Counter)
        {
            MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
            uint16 local_pin_buffer;
            BOOL_TYPE valid_freq_detected = FALSE;
            BOOL_TYPE is_charge_pump = FALSE;
            sint8 pin_value_counter = 0;
            sint8 local_level_counter;
            uint8 local_sampling_counter;
            uint8 local_phase_counter;
            uint8 local_phase_shift;
            uint8 local_freq_counter;
            uint8 num_samples;
            uint8 sample_reference;

            // Take a snapshot of pin buffer and sample counter to prevent concurrent updates from OneMsProcess function
            MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
            local_sampling_counter = data->Sampling_Counter;
            local_pin_buffer = data->Pin_Buffer;
            MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();

            // Get number of samples to process with a limit to 10 samples
            // This limit comes from the falling edge transition pattern checks
            num_samples = (uint8) (local_sampling_counter - data->Async_Sampling_Reference);
            if(num_samples > 10U)
            {
                num_samples = 10U;
            }
            sample_reference = data->Async_Sampling_Reference + num_samples;

            // Collects high, low pin value occurrences and detects a frequency signal through falling edge transitions:
            // - It counts number of High and Low new samples collected
            // - It detects a falling edge transition through a pattern detection:
            //   - last 4 ms for a clean falling edge transition with a 2ms debounce for each edge (2ms ON, 2ms OFF)
            //   - last 6 ms for a clean Charge pump signal (3 times: 1ms ON, 1ms OFF)
            //   - last 6 ms for a noisy falling edge transition with a 2ms debounce for each steady edge (2ms ON, 1ms OFF, 1 ms ON, 2ms OFF)
            //   - last 8 ms for a very noisy falling edge transition with a 2ms debounce for each steady edge (2ms ON, 1ms OFF, 1 ms ON, 1ms OFF, 1 ms ON, 2ms OFF)
            // - When a frequency signal has been detected:
            //   - it saves frequency time reference
            //   - it updates the time between frequency event to estimate the cycle (period) time
            for(uint8 i = 0; i < num_samples; i++)
            {
                uint16 pin_value = (local_pin_buffer & 0x01U);

                // Update Level counter: increment if High pin value, decrement if Low pin value
                if(pin_value != 0U)
                {
                    pin_value_counter++;
                }
                else
                {
                    pin_value_counter--;
                }

                // Find a frequency event based to a falling edge transition patterns, including noisy ones
                if ((local_pin_buffer & 0x0FU) == 0x0CU)          // Check if a falling edge event occur with 2ms debounce (2 ON - 2 OFF)
                {
                    UpdateLastFrequencyTimingEvents(data, n_data, sample_reference - 2U);
                }
                else if(((local_pin_buffer & 0x3FU) == 0x2AU) &&    // Check if there is a Charge pump signal and keep the last frequency event
                    !is_charge_pump )
                {
                    is_charge_pump = TRUE;
                    UpdateLastFrequencyTimingEvents(data, n_data, sample_reference - 1U);
                }
                else if  ((local_pin_buffer & 0x3FU) == 0x34U)      // Check if a falling edge event occur with 2ms debounce and 1 ms spikes (2 ON, 1 OFF, 1 ON, 2 OFF)
                {
                    UpdateLastFrequencyTimingEvents(data, n_data, sample_reference - 3U);
                }
                else if ((local_pin_buffer & 0xFFU) == 0xD4U)         // Check if a falling edge event occur with 2ms debounce and 1 ms spikes (2 ON, 1 OFF,1 ON, 1 OFF, 1 ON, 2 OFF)
                {
                    UpdateLastFrequencyTimingEvents(data, n_data, sample_reference - 4U);
                }

                sample_reference--;
                local_pin_buffer >>= 1U;
            }
            // Update Async Sampling Reference for the next execution
            SRData__UpdateByte(&data->Async_Sampling_Reference, &n_data->Async_Sampling_Reference, local_sampling_counter);

            // Check if no frequency signal detected after LAST_FREQUENCY_EVENT_TIMEOUT
            // If no frequency events for more than timeout, switch to level detection
           if((uint8) (local_sampling_counter - data->Last_Frequency_Event_Reference) >= (uint8) LAST_FREQUENCY_EVENT_TIMEOUT)
            {
                SRData__UpdateByte((void *) &data->Is_Detecting_Frequency,(void *)  &n_data->Is_Detecting_Frequency,(uint8) FALSE);
            }

           // Take a snapshot of Frequency, Phase, Level event counters to
           local_freq_counter = data->Frequency_Counter;
           local_phase_counter = data->Phase_Counter;
           local_level_counter = data->Level_Counter;
           local_phase_shift = data->Phase_Shift;

            // Frequency detection: it checks if a valid frequency signal has been detected
            // Valid frequency signals are Charge Pump and Mains (AC Signal) like frequency signal
            if(data->Is_Detecting_Frequency)
            {
                BOOL_TYPE is_ac_signal;
                // Valid Charge pump is detected around every Asynchronous iteration (at least 150% of 5ms)
                // Last frequency event is detected starting from the latest sample, so an average of 5ms shift is expected
                is_charge_pump = (BOOL_TYPE) (is_charge_pump && (data->Time_Between_Frequency_Events > 0U));
                is_charge_pump = (BOOL_TYPE) (is_charge_pump && (data->Time_Between_Frequency_Events <= 8U));
                // Valid AC Signal frequency events happen within  45 Hz (22.22ms) - 65 Hz (15.38ms) range
                is_ac_signal = (BOOL_TYPE) (data->Time_Between_Frequency_Events >= 15U);
                is_ac_signal = (BOOL_TYPE) (is_ac_signal && (data->Time_Between_Frequency_Events <= 23U));

                // Evaluate only if frequency is a Charge pump or is in the AC range
                // Criteria between AC signal and Charge pump are mutually exclusive, they cannot be TRUE at the same time
                if(is_charge_pump || is_ac_signal)
                {
                    BOOL_TYPE is_phase_signal = TRUE;

                    valid_freq_detected = TRUE;
                    local_freq_counter++;

                    // If SRZeroCross LLI has been registered by this module, an AC detected signal shall be compared to ZeroCross signal
                    // to check if they are in phase or not
                    // This check is performed only when Frequency detection should be running on both signal:
                    // - It evaluates phase shift in milliseconds from ZeroCross through Last Frequency event detected of both signals
                    // - If phase shift is more than half of ZeroCross cycle, phase shift is decremented by one cycle
                    //   - This discrepancy is due to also take care a different duty cycle on Frequency signals
                    // - Phase shift evaluation is done on absolute value
                    // - If phase shift is more than 90 degrees equivalent time, signal will be considered opposite / out of phase
                    (void) SRData__CheckByte(&SRZeroCross_Index, &N_SRZeroCross_Index);
                    if(is_ac_signal &&
                        (SRZeroCross_Index < SRFeedbackLLI_Allocation) &&
                        (SRZeroCross_Index != input->Data_Index))
                    {
                        (void) SRData__CheckByte((void *) &SRFeedbackLLI_Data[SRZeroCross_Index].Is_Detecting_Frequency,
                            (void *) &N_SRFeedbackLLI_Data[SRZeroCross_Index].Is_Detecting_Frequency);
                        if(SRFeedbackLLI_Data[SRZeroCross_Index].Is_Detecting_Frequency &&
                            (SRFeedbackLLI_Data[SRZeroCross_Index].Time_Between_Frequency_Events > 0U))
                        {
                            // Maximum phase shift accepted is 90 degrees equivalent
                            sint16 quarter_cycle = (sint16) data->Time_Between_Frequency_Events / 4;
                            sint16 phase_shift;
                            uint8 zc_timeout;

                            (void) SRData__CheckByte(&SRFeedbackLLI_Data[SRZeroCross_Index].Last_Frequency_Event_Reference,
                                &N_SRFeedbackLLI_Data[SRZeroCross_Index].Last_Frequency_Event_Reference);
                            phase_shift = data->Last_Frequency_Event_Reference;
                            phase_shift -= (sint16) SRFeedbackLLI_Data[SRZeroCross_Index].Last_Frequency_Event_Reference;
                            // Consider ZeroCross half cycle margin between frequency signal timeouts
                            // if that distance is higher than half cycle, phase shift should be decremented by one cycle
                            zc_timeout = SRFeedbackLLI_Data[SRZeroCross_Index].Last_Frequency_Event_Reference;
                            zc_timeout += (SRFeedbackLLI_Data[SRZeroCross_Index].Time_Between_Frequency_Events / 2U);
                            if(zc_timeout < data->Last_Frequency_Event_Reference)
                            {
                                phase_shift -= (sint16) SRFeedbackLLI_Data[SRZeroCross_Index].Time_Between_Frequency_Events;
                            }
                            if(phase_shift < 0)
                            {
                                phase_shift = -phase_shift;
                            }

                            // Signal out of phase if phase shift is more than 90 degrees
                            // this limit takes care of a possible different duty cycle between Feedback signals
                            if(phase_shift > quarter_cycle)
                            {
                                is_phase_signal = FALSE;
                            }
                            local_phase_shift = (uint8) phase_shift;
                        }
                    }
                    else
                    {
                        local_phase_shift = 0;
                    }

                    // Increment the Phase counter if no out of phase condition has been detected on that signal
                    if(is_charge_pump || is_phase_signal)
                    {
                        local_phase_counter++;
                    }

                    // If the number of Frequency event occurrences becomes higher than the half of valid samples to collect
                    // Frequency detection takes precedence over Level detection
                    if(local_freq_counter >= level_freq_threshold)
                    {
                        local_level_counter = 0;
                    }
                    
                    // Update LLI value to Frequency (Phase / Opp.Phase) if number of valid samples have been collected
                    if(local_freq_counter >= n_read)
                    {
                        SRFEEDBACKLLI_VALUE_ENUM_TYPE new_value = (local_phase_counter >= (n_read / 2U)) ? SRFEEDBACKLLI_PHASE : SRFEEDBACKLLI_OPP_PHASE;
                        SRData__UpdateByte((void *) &data->Value, (void *) &n_data->Value, (uint8) new_value);
                        SRData__UpdateByte(&data->Ac_Is_Present, &n_data->Ac_Is_Present, 1);
                        local_freq_counter = 0;
                        local_phase_counter = 0;
                        retval = TRUE;
                    }
                }
            }

            // If not valid Frequency signal has been detected, High / Low detection algorithm is performed
            if(!valid_freq_detected)
            {
                // High value level event: Level counter is a positive number, higher than pin noise threshold
                if(pin_value_counter >= SAMPLES_NOISE_THRESHOLD)
                {
                    local_level_counter++;
                }
                // Low value level event: Level counter is a negative number, lower than pin noise threshold
                else if(pin_value_counter <= -SAMPLES_NOISE_THRESHOLD)
                {
                    local_level_counter--;
                }

                // If the number of High/Low value Level event occurrences becomes higher than the half of valid samples to collect
                // Level detection takes precedence over Frequency detection
                if((local_level_counter >= (sint8) level_freq_threshold) ||
                    (local_level_counter <= -((sint8) level_freq_threshold)))
                {
                    local_freq_counter = 0;
                    local_phase_counter = 0;
                }

                // Update LLI value to High if number of valid samples have been collected
                if(local_level_counter >= (sint8) n_read)
                {
                    SRData__UpdateByte((void *) &data->Value, (void *) &n_data->Value, (uint8) SRFEEDBACKLLI_HIGH);
                    SRData__UpdateByte(&data->Ac_Is_Present, &n_data->Ac_Is_Present, 0);
                    local_level_counter = 0;
                    local_phase_shift = 0;
                    retval = TRUE;
                }
                
                // Update LLI value to Low if number of valid samples have been collected
                if(local_level_counter <= -((sint8) n_read))
                {
                    SRData__UpdateByte((void *) &data->Value, (void *) &n_data->Value, (uint8) SRFEEDBACKLLI_LOW);
                    SRData__UpdateByte(&data->Ac_Is_Present, &n_data->Ac_Is_Present, 0);
                    local_level_counter = 0;
                    local_phase_shift = 0;
                    retval = TRUE;
                }
            }

            SRData__UpdateByte(&data->Frequency_Counter, &n_data->Frequency_Counter, local_freq_counter);
            SRData__UpdateByte((void *) &data->Level_Counter, (void *) &n_data->Level_Counter, (uint8) local_level_counter);
            SRData__UpdateByte(&data->Phase_Counter, &n_data->Phase_Counter, local_phase_counter);
            SRData__UpdateByte(&data->Phase_Shift, &n_data->Phase_Shift, local_phase_shift);
        }

    }

    return (retval);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

/*
 *
 */
static void UpdateLastFrequencyTimingEvents(SRFEEDBACKLLI_DATA_TYPE *data_ptr, SRFEEDBACKLLI_DATA_TYPE *n_data_ptr, uint8 new_frequency_reference)
{
    if(data_ptr->Is_Detecting_Frequency)
    {
        SRData__UpdateByte(&data_ptr->Time_Between_Frequency_Events, &n_data_ptr->Time_Between_Frequency_Events, new_frequency_reference - data_ptr->Last_Frequency_Event_Reference);
    }
    SRData__UpdateByte(&data_ptr->Last_Frequency_Event_Reference, &n_data_ptr->Last_Frequency_Event_Reference, new_frequency_reference);
    SRData__UpdateByte((void *) &data_ptr->Is_Detecting_Frequency,(void *)  &n_data_ptr->Is_Detecting_Frequency,(uint8) TRUE);
}


#endif

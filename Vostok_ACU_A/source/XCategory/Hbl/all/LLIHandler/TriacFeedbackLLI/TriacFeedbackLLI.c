/**
 *  @file       TriacFeedbackLLI.c
 *
 *  @brief      Module for TriacFeedbackLLI reading - detects a Nominal/Double grid frequency or a Digital level
 *
 *  @details    This module track an input and decode it as level (Low or High) or frequency (grid frequency or its double).
 *
 *              In order to detect a frequency two event must be measured without additional spurious event.
 *              After this, the module is capable of rejecting an arbitrary number of spurious event.
 *
 *              The read of the level at the end of this time window is performed via a simple read.
 *
 *              Macros available to configure the module:
 *
 *               TRIACFEEDBACKLLI_ALLOWED_MIN_FREQUENCY:
 *                  (Default 45) Minimum mains frequency allowed
 *
 *               TRIACFEEDBACKLLI_ALLOWED_MAX_FREQUENCY:
 *                  (Default 65) Maximum mains frequency allowed
 *
 *
 *  @copyright  Copyright 2019 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "TriacFeedbackLLI.h"

#if (HBL_LLI_NUM_TRIACFEEDBACK > 0)
#include "Gpio.h"
#include "Micro.h"
#include "VirtualPin.h"
#include <string.h>

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define DEFAULT_NUM_SAMPLES                 16
#define MIN_NUM_SAMPLES                     8
#define SAMPLES_NOISE_THRESHOLD             2

// Minimum frequency allowed
#ifndef TRIACFEEDBACKLLI_ALLOWED_MIN_FREQUENCY
    #ifdef SRTRIACFEEDBACKLLI_ALLOWED_MIN_FREQUENCY
        #define TRIACFEEDBACKLLI_ALLOWED_MIN_FREQUENCY SRTRIACFEEDBACKLLI_ALLOWED_MIN_FREQUENCY
    #else
        #define TRIACFEEDBACKLLI_ALLOWED_MIN_FREQUENCY   45
    #endif
#endif

// Maximum frequency allowed
#ifndef TRIACFEEDBACKLLI_ALLOWED_MAX_FREQUENCY
    #ifdef SRTRIACFEEDBACKLLI_ALLOWED_MAX_FREQUENCY
        #define TRIACFEEDBACKLLI_ALLOWED_MAX_FREQUENCY SRTRIACFEEDBACKLLI_ALLOWED_MAX_FREQUENCY
    #else
        #define TRIACFEEDBACKLLI_ALLOWED_MAX_FREQUENCY   65
    #endif
#endif

#define TIME_MSEC_IN_ONE_SEC                (1000)

#define MINIMUM_DURATION_ALLOWED            (TIME_MSEC_IN_ONE_SEC / TRIACFEEDBACKLLI_ALLOWED_MAX_FREQUENCY)
#define MAXIMUM_DURATION_ALLOWED            ((TIME_MSEC_IN_ONE_SEC / TRIACFEEDBACKLLI_ALLOWED_MIN_FREQUENCY) + 1)

#define MINIMUM_HALF_DURATION_ALLOWED       ((TIME_MSEC_IN_ONE_SEC / (2 * TRIACFEEDBACKLLI_ALLOWED_MAX_FREQUENCY)))
#define MAXIMUM_HALF_DURATION_ALLOWED       ((TIME_MSEC_IN_ONE_SEC / (2 * TRIACFEEDBACKLLI_ALLOWED_MIN_FREQUENCY)) + 1)

#define LAST_FREQUENCY_EVENT_TIMEOUT        ((3 * TIME_MSEC_IN_ONE_SEC) / (2 * TRIACFEEDBACKLLI_ALLOWED_MIN_FREQUENCY))

typedef struct
{
    uint8 N_Read:3;
    uint8 Unused_0:5;
    uint8 Unused_1:5;
    uint8 Pull_Up:1;
    uint8 Unused_2:1;
    uint8 Inverted:1;
} TRIACFEEDBACKLLI_PARAM_TYPE;

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
    // Double frequency detection in progress
    BOOL_TYPE Is_Detecting_Double_Frequency;
    // Sampling Sequence counter reference of the last frequency event detected
    uint8 Last_Frequency_Event_Reference;
    // Time (in ms) between last two frequency events detected
    uint8 Time_Between_Frequency_Events;
    // Counter of High / Low pin level event detected: it increments on a new High value event, decrements on a new Low value event
    sint8 Level_Counter;
    // Counter of valid Frequency event detected: it increments when valid Frequency events have been detected
    uint8 Frequency_Counter;
    // Counter of Double Frequency event detected: it increments if a valid double frequency event happened
    uint8 Double_Frequency_Counter;
    // Current Feedback LLI value (0, 1, 2, 3)
    TRIAC_FEEDBACK_LLI_OUTPUT_TYPE Value;
} TRIACFEEDBACKLLI_DATA_TYPE;

static TRIACFEEDBACKLLI_DATA_TYPE TriacFeedbackLLI_Data[HBL_LLI_NUM_TRIACFEEDBACK];
static uint8 TriacFeedbackLLI_Allocation;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void UpdateLastFrequencyTimingEvents(TRIACFEEDBACKLLI_DATA_TYPE *data_ptr, uint8 new_frequency_reference);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      It Initializes the module TriacFeedbackLLI and its variables
 */
void TriacFeedbackLLI__Initialize(void)
{
    TriacFeedbackLLI_Allocation = 0;
    memset(TriacFeedbackLLI_Data, 0, sizeof(TriacFeedbackLLI_Data));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Checks if there is free space and reserve a new instance
 * @return  index of TriacFeedbackLLI instance, 0xFF if all HBL_LLI_NUM_TRIACFEEDBACK are already allocated
 */
uint8 TriacFeedbackLLI__Allocate(void)
{
    uint8 retval = HBL_ALLOCATION_INVALID;

    if (TriacFeedbackLLI_Allocation < (uint8) HBL_LLI_NUM_TRIACFEEDBACK)
    {
        retval = TriacFeedbackLLI_Allocation;
        TriacFeedbackLLI_Allocation++;

    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      Initializes a new instance of TriacFeedbackLLI
 * @param   input     pointer to HBL_LLI_TYPE structure reserved for that instance
 * @return  PASS if initialize succeed, FAIL if there were any error
 */
PASS_FAIL_TYPE TriacFeedbackLLI__InitializeInstance(HBL_LLI_TYPE * input)
{
    PASS_FAIL_TYPE ret_status = FAIL;

    if(input->Data_Index < TriacFeedbackLLI_Allocation)
    {
        TRIACFEEDBACKLLI_DATA_TYPE *data = &TriacFeedbackLLI_Data[input->Data_Index];
        TRIACFEEDBACKLLI_PARAM_TYPE *feedback_param = (void *)input->Parameter_Ptr.Data;
        GPIO_PIN_MODE_TYPE type = INPUT_FLOAT;
        uint8 virtual_pin = input->Pin_Ptr[0];

        if((feedback_param != NULL) &&
            ((uint8) feedback_param->Pull_Up != 0U))
        {
            type = INPUT_PULLUP;
        }

        if(virtual_pin < VirtualPin__GetNumOf())
        {
            Gpio__PinConfig(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin, type);
            data->Value = TRIAC_UNKNOWN_UNKNOWN_VALUE;
            ret_status = PASS;
        }
    }
    return (ret_status);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Provides current TriacFeedbackLLI value
 * @param   input     pointer to HBL_LLI_TYPE structure reserved for that instance
 * @return  Pointer to LLI Data structure (Public_Data.Output) or NULL if input points to an invalid item
 */
void * TriacFeedbackLLI__Get(HBL_LLI_TYPE * input)
{
    void *dataptr = NULL;

    if (input->Data_Index < TriacFeedbackLLI_Allocation)
    {
        dataptr = &TriacFeedbackLLI_Data[input->Data_Index].Value;
    }
    return(dataptr);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Process function to read on Asynch Process (5ms) - it detects High or Low values if no Frequency event occurs and updates LLI
 * @param input     pointer to HBL_LLI_TYPE structure instance
 * @return  TRUE if a new sample is ready, FALSE otherwise
 */
BOOL_TYPE TriacFeedbackLLI__AsynchProcess(HBL_LLI_TYPE * input)
{
    BOOL_TYPE retval = FALSE;

    if (input->Data_Index < TriacFeedbackLLI_Allocation)
    {
        TRIACFEEDBACKLLI_DATA_TYPE *data = &TriacFeedbackLLI_Data[input->Data_Index];
        TRIACFEEDBACKLLI_PARAM_TYPE *feedback_param = (void *) input->Parameter_Ptr.Data;
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

        // Update Feedback values if there are new samples coming from  1ms Process
        if(data->Async_Sampling_Reference != data->Sampling_Counter)
        {
            MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
            uint16 local_pin_buffer;
            BOOL_TYPE valid_freq_detected = FALSE;
            sint8 pin_value_counter = 0;
            sint8 local_level_counter;
            uint8 local_sampling_counter;
            uint8 local_double_freq_counter;
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
                    UpdateLastFrequencyTimingEvents(data, sample_reference - 2U);
                }
                else if((local_pin_buffer & 0x1FU) == 0x04U)      // Check if a falling edge event occur with 1 ms spikes (2 OFF, 1 ON, 2 OFF)
                {
                    UpdateLastFrequencyTimingEvents(data, sample_reference - 2U);
                }
                else if((local_pin_buffer & 0x1FU) == 0x1BU)      // Check if a falling edge event occur with 1 ms spikes (2 ON, 1 OFF, 2 ON)
                {
                    UpdateLastFrequencyTimingEvents(data, sample_reference - 3U);
                }
                else if  ((local_pin_buffer & 0x3FU) == 0x34U)      // Check if a falling edge event occur with 2ms debounce and 1 ms spikes (2 ON, 1 OFF, 1 ON, 2 OFF)
                {
                    UpdateLastFrequencyTimingEvents(data, sample_reference - 3U);
                }
                else if ((local_pin_buffer & 0xFFU) == 0xD4U)         // Check if a falling edge event occur with 2ms debounce and 1 ms spikes (2 ON, 1 OFF,1 ON, 1 OFF, 1 ON, 2 OFF)
                {
                    UpdateLastFrequencyTimingEvents(data, sample_reference - 4U);
                }

                sample_reference--;
                local_pin_buffer >>= 1U;
            }
            // Update Async Sampling Reference for the next execution
            data->Async_Sampling_Reference = local_sampling_counter;

            // Check if no double frequency signal detected after LAST_FREQUENCY_EVENT_TIMEOUT / 2
            // If no frequency events for more than timeout, switch to frequency detection
            if((uint8) (local_sampling_counter - data->Last_Frequency_Event_Reference) >= (uint8) (LAST_FREQUENCY_EVENT_TIMEOUT / 2))
            {
                data->Is_Detecting_Double_Frequency = FALSE;
            }

            // Check if no frequency signal detected after LAST_FREQUENCY_EVENT_TIMEOUT
            // If no frequency events for more than timeout, switch to level detection
           if((uint8) (local_sampling_counter - data->Last_Frequency_Event_Reference) >= (uint8) LAST_FREQUENCY_EVENT_TIMEOUT)
            {
                data->Is_Detecting_Frequency = FALSE;
                data->Time_Between_Frequency_Events = 0;
            }

           // Take a snapshot of Frequency, Double Frequency, Level event counters to
           local_freq_counter = data->Frequency_Counter;
           local_double_freq_counter = data->Double_Frequency_Counter;
           local_level_counter = data->Level_Counter;

            // Frequency detection: it checks if a valid frequency signal has been detected
            // Valid frequency signals are Charge Pump and Mains (AC Signal) like frequency signal
            if(data->Is_Detecting_Frequency)
            {
                TRIAC_FEEDBACK_LLI_OUTPUT_TYPE new_value = TRIAC_UNKNOWN_UNKNOWN_VALUE;
                BOOL_TYPE is_mains_freq;

                // Valid AC Signal frequency events happen within duration range - e.g. 45 Hz (22.22ms) - 65 Hz (15.38ms) range
                is_mains_freq = (BOOL_TYPE) (data->Time_Between_Frequency_Events >= (uint8) MINIMUM_DURATION_ALLOWED);
                is_mains_freq = (BOOL_TYPE) (is_mains_freq && (data->Time_Between_Frequency_Events <= (uint8) MAXIMUM_DURATION_ALLOWED));

                if(is_mains_freq || data->Is_Detecting_Double_Frequency)
                {
                    valid_freq_detected = TRUE;

                    // Increment the Double counter if Time between Frequency Events is less than the Mains frequency
                    if(data->Is_Detecting_Double_Frequency)
                    {
                        local_double_freq_counter++;
                        // If the number of Frequency event occurrences becomes higher than the half of valid samples to collect
                        // Frequency detection takes precedence over Level and double frequency detection
                        if(local_double_freq_counter >= level_freq_threshold)
                        {
                            local_level_counter = 0;
                            local_freq_counter = 0;
                        }

                        // Update LLI value to Frequency (Phase / Opp.Phase) if number of valid samples have been collected
                        if(local_double_freq_counter >= n_read)
                        {
                            new_value = TRIAC_FB_LLI_DOUBLE_GRID_FREQ;
                        }
                    }
                    else
                    {
                        local_freq_counter++;
                        // If the number of Frequency event occurrences becomes higher than the half of valid samples to collect
                        // Frequency detection takes precedence over Level and double frequency detection
                        if(local_freq_counter >= level_freq_threshold)
                        {
                            local_level_counter = 0;
                            local_double_freq_counter = 0;
                        }

                        // Update LLI value to Frequency (Phase / Opp.Phase) if number of valid samples have been collected
                        if(local_freq_counter >= n_read)
                        {
                            new_value = TRIAC_FB_LLI_NOMINAL_GRID_FREQ;
                        }
                    }
                }

                // Update LLI value to Frequency (Phase / Opp.Phase) if number of valid samples have been collected
                if(new_value != TRIAC_UNKNOWN_UNKNOWN_VALUE)
                {
                    data->Value = new_value;
                    local_freq_counter = 0;
                    local_double_freq_counter = 0;
                    retval = TRUE;
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
                    local_double_freq_counter = 0;
                }

                // Update LLI value to High if number of valid samples have been collected
                if(local_level_counter >= (sint8) n_read)
                {
                    data->Value = TRIAC_FB_LLI_1_VALUE;
                    local_level_counter = 0;
                    retval = TRUE;
                }

                // Update LLI value to Low if number of valid samples have been collected
                if(local_level_counter <= -((sint8) n_read))
                {
                    data->Value = TRIAC_FB_LLI_0_VALUE;
                    local_level_counter = 0;
                    retval = TRUE;
                }
            }

            data->Frequency_Counter = local_freq_counter;
            data->Level_Counter = local_level_counter;
            data->Double_Frequency_Counter = local_double_freq_counter;
        }
    }

    return(retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Executes TriacFeedbackLLI 1ms sampling
 * @param       input - Hbl LLI Data structure of the instance to process
 * @return      FALSE    this process never updates LLI Sequence ID
 */
BOOL_TYPE TriacFeedbackLLI__OneMsProcess(HBL_LLI_TYPE * input)
{
    if (input->Data_Index < TriacFeedbackLLI_Allocation)
    {
        TRIACFEEDBACKLLI_DATA_TYPE *data = &TriacFeedbackLLI_Data[input->Data_Index];
        TRIACFEEDBACKLLI_PARAM_TYPE *feedback_param = (void *) input->Parameter_Ptr.Data;
        uint8 virtual_pin = input->Pin_Ptr[0];
        uint8 pin_value = (uint8) Gpio__PinRead(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin);
        uint16 pin_buffer;

        // Inverted logic flag enabled
        if((feedback_param != NULL) &&
           ((uint8) feedback_param->Inverted != 0U))
        {
            pin_value = (pin_value == 0U) ? 1U : 0U;
        }

        // Update pin buffer with a new sample
        pin_buffer = data->Pin_Buffer;
        pin_buffer  <<= 1U;
        pin_buffer |= (uint16) pin_value & 0x01U;
        data->Pin_Buffer = pin_buffer;

        // Update sampling counter
        data->Sampling_Counter++;
    }
    return(FALSE);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

/*
 *
 */
static void UpdateLastFrequencyTimingEvents(TRIACFEEDBACKLLI_DATA_TYPE *data_ptr, uint8 new_frequency_reference)
{
    if(data_ptr->Is_Detecting_Frequency)
    {
        BOOL_TYPE is_double_freq;
        data_ptr->Time_Between_Frequency_Events = new_frequency_reference - data_ptr->Last_Frequency_Event_Reference;
        // Valid AC Signal frequency events happen within half of durationr range - e.g. 45 Hz (11.11ms) - 65 Hz (7.38ms) range
        is_double_freq = (BOOL_TYPE) (data_ptr->Time_Between_Frequency_Events >= (uint8) MINIMUM_HALF_DURATION_ALLOWED);
        is_double_freq = (BOOL_TYPE) (is_double_freq && (data_ptr->Time_Between_Frequency_Events <= (uint8) MAXIMUM_HALF_DURATION_ALLOWED));
        data_ptr->Is_Detecting_Double_Frequency = is_double_freq;
    }
    data_ptr->Last_Frequency_Event_Reference = new_frequency_reference;
    data_ptr->Is_Detecting_Frequency = TRUE;
}


#endif

/** @addtogroup SRICTachometerLLI
 *  @{
 *  @file       SRICTachometerLLI.c
 *
 *  @brief      SR Input Capture based Tachometer reading
 *
 *  @details    This module implements the SR methods for reading a tachometer input which provides to the microcontroller
 *              a square wave signal whose frequency range is expected to be between 0 to 1KHz.
 *
 *              The 0 Hz detection, which is critical for the system, is made through a configurable timeout timer.
 *
 *  @defgroup   CLASS_B
 *
 *  @copyright  Copyright 2017-2018. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 *  }@
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SRICTachometerLLI.h"

#if (HBL_LLI_NUM_SRICTACHOMETER > 0)
#include "Crc8.h"
#include "Timer.h"
#include "SRData.h"
#include "SRException.h"
#include "SRMicro.h"
#include "SRTaskSequence.h"
#include <string.h>
#include "VirtualPin.h"

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//!Parameter hardcoded as it is fundamental for the SR
#define SRICTACHOMETER_COUNT_TO_DETECT_0Hz      1000// 1s of non IC detection It assumes Zero RPM

//! Counts to 1000 rpm
#define TACHO_MINUMUM_COUNT_VALUE               468

//! Defines the data for an instance of an SRICTachometer LLI
PACKED typedef struct
{
    SRICTACHOMETER_PUBLIC_TYPE Public_Data;		//!< Holds the public data of this module. It can be accessed by the "Get" function
    TIMER_PARAMETER_TYPE Timer_Parameter;		//!< Holds the parameters of this module.
    uint32 Timer_Counter;						//!< Used to detect zero RPM. It increments every 1ms and gets reset once an IC happens.
    uint32 Accumulator;							//!< Accumulate the last 5 samples to average it. The result of the average is at "Filtered_Period" variable.
    uint8  Samples_Counter;						//!< Holds the numember of samples to take the average.
}SRICTACHOMETER_DATA_TYPE;

//! Module data for each instance of an SRICTachometer LLI
static SRICTACHOMETER_DATA_TYPE SRICTachometerLLI_Data[HBL_LLI_NUM_SRICTACHOMETER];

//! Holds a CRC for data in SRICTachometerLLI_Data variable)
static uint8 SRICTachometerLLI_Data_Crc[HBL_LLI_NUM_SRICTACHOMETER];

//! Keeps track of how many SRICTachometer LLIs have been allocated by the application (based on setting file configuration)
static uint8 SRICTachometerLLI_Allocation;
//! Inverted memory for the Class B variable
static uint8 N_SRICTachometerLLI_Allocation;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module SRICTachometerLLI and its variables.
 *
 */
void SRICTachometerLLI__Initialize(void)
{
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    for (uint8 index = 0; index < HBL_LLI_NUM_SRICTACHOMETER; index++)
    {
        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
        memset(&SRICTachometerLLI_Data[index],0,sizeof(SRICTachometerLLI_Data[0]));

        SRICTachometerLLI_Data[index].Public_Data.Filtered_Period = TACHO_ZERO_RPM_TIMER_COUNTS;
        SRICTachometerLLI_Data[index].Public_Data.Raw_Period = TACHO_ZERO_RPM_TIMER_COUNTS;

        SRICTachometerLLI_Data_Crc[index] = Crc8(0xFF,(uint8 *)(void *)&SRICTachometerLLI_Data[index], sizeof(SRICTachometerLLI_Data[0]));
        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
    }
    SR_SET_BYTE(SRICTachometerLLI_Allocation, 0);

#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_INIT, SRTASKSEQUENCE_APPL_INIT_SRICTACHOMETER_LLI);
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  It allocates the index for the SRICTachometer.
 *
 * @return SRICTachometerLLI_Allocation
 */
uint8 SRICTachometerLLI__Allocate(void)
{
    uint8 retval = HBL_ALLOCATION_INVALID;

    if ((SR_CHECK_BYTE(SRICTachometerLLI_Allocation) == SRDATA_OK) &&
        (SRICTachometerLLI_Allocation < HBL_LLI_NUM_SRICTACHOMETER))
    {
        retval = SRICTachometerLLI_Allocation;
        SR_SET_BYTE(SRICTachometerLLI_Allocation, SRICTachometerLLI_Allocation+1);
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief It initialize the instance of the SRICTachometer module.
 *
 *  @details A timer is used as InputCapture. The configuration comes from the virtual pin table.
 */
PASS_FAIL_TYPE SRICTachometerLLI__InitializeInstance(HBL_LLI_TYPE * input)
{
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    PASS_FAIL_TYPE ret_status;
    uint8 virtual_pin;
    SRICTACHOMETER_DATA_TYPE *data;

    ret_status = FAIL;

    if ((SR_CHECK_BYTE(SRICTachometerLLI_Allocation) == SRDATA_OK) &&
        (input->Data_Index < SRICTachometerLLI_Allocation) &&
        (Crc8(0xFF, (uint8 *)(void *)&SRICTachometerLLI_Data[input->Data_Index], sizeof(SRICTachometerLLI_Data[0])) == SRICTachometerLLI_Data_Crc[input->Data_Index]))
    {
        data = (SRICTACHOMETER_DATA_TYPE *)(void *)&SRICTachometerLLI_Data[input->Data_Index];

        virtual_pin = input->Pin_Ptr[0];
        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

        //configure timer for Input Capture
        data->Timer_Parameter.Timer       =   (TIMER_TYPE)VIRTUALPIN_TABLE[virtual_pin].device;
        data->Timer_Parameter.Channel     =   (TIMER_CHANNEL_TYPE)VIRTUALPIN_TABLE[virtual_pin].param0;
        data->Timer_Parameter.Mode        =   TIMER_MODE_INPUT_CAPTURE;
        data->Timer_Parameter.Port        =   VIRTUALPIN_TABLE[virtual_pin].port;
        data->Timer_Parameter.Pin         =   VIRTUALPIN_TABLE[virtual_pin].pin;
        data->Timer_Parameter.Pin_ReMap   =   VIRTUALPIN_TABLE[virtual_pin].pin_remap;
        data->Timer_Parameter.Polarity    =   TIMER_POLARITY_ACTIVE_HIGH;

        //Configure Continuous reading
        ret_status =  Timer__Config(&data->Timer_Parameter);
        ret_status |= Timer__CaptureStart(&data->Timer_Parameter, TIMER_CAPTURE_CYCLE, TIMER_CAPTUREON_RISING_EDGE);

        SRICTachometerLLI_Data_Crc[input->Data_Index] = Crc8(0xFF, (uint8 *)(void *)data, sizeof(SRICTachometerLLI_Data[0]));
        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
    }

    return (ret_status);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   One ms Handler of SRICTachometer module.
 *
 * @details This method handles the zero rpm detection. An internal counter is used to detect It.
 *
 * @param   input
 *
 * @return  BOOL_TYPE
 */
BOOL_TYPE SRICTachometerLLI__OneMsProcess(HBL_LLI_TYPE * input)
{
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    SRICTACHOMETER_DATA_TYPE *data;
    BOOL_TYPE crc_check;
    PASS_FAIL_TYPE config_check;
    BOOL_TYPE ret_val;

    ret_val = FALSE; // Always return FALSE to avoid any unexpected sequence updated. The LLI sequence gets updated by the ICPorcess.

    if ((SR_CHECK_BYTE(SRICTachometerLLI_Allocation) == SRDATA_OK) &&
        (input->Data_Index < SRICTachometerLLI_Allocation))
    {
        data = (SRICTACHOMETER_DATA_TYPE *)(void *)&SRICTachometerLLI_Data[input->Data_Index];
        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
        crc_check = (BOOL_TYPE) (Crc8(0xFF, (uint8 *)(void *)data, sizeof(SRICTachometerLLI_Data[0])) == SRICTachometerLLI_Data_Crc[input->Data_Index]);
        config_check = Timer__ConfigCheck(&data->Timer_Parameter);
        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();

        if ( (TRUE == crc_check) &&
             (PASS == config_check) )
        {
            MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
            //Zero RPM

            if (data->Timer_Counter >= SRICTACHOMETER_COUNT_TO_DETECT_0Hz)
            {
                data->Public_Data.Filtered_Period = TACHO_ZERO_RPM_TIMER_COUNTS;    // reset filtered period variable
                data->Public_Data.Raw_Period = TACHO_ZERO_RPM_TIMER_COUNTS;    		// reset raw period variable
            }
            else
            {
                data->Timer_Counter++;
            }

            SRICTachometerLLI_Data_Crc[input->Data_Index] = Crc8(0xFF,(uint8 *)(void *)&SRICTachometerLLI_Data[input->Data_Index], sizeof(SRICTachometerLLI_Data[0]));
            MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();

        }
    }

    return (ret_val);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief    Performs periodic check on the integrity of the RAM variables used in this module
 * @details  This function shall be called from HBL__PREPROCESSLOADS() in Hbl module
 */
void SRICTachometerLLI__Handler(void)
{
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    BOOL_TYPE retval = (BOOL_TYPE) (SR_CHECK_BYTE(SRICTachometerLLI_Allocation) == SRDATA_OK);

    for (uint8 index = 0; index < HBL_LLI_NUM_SRICTACHOMETER; index++)
    {
        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
        retval &= (BOOL_TYPE) (Crc8(0xFF, (uint8 *)(void *)&SRICTachometerLLI_Data[index], sizeof(SRICTachometerLLI_Data[0])) == SRICTachometerLLI_Data_Crc[index]);
        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
        if (retval == FALSE)
        {
            SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, 0, index);
            break;
        }
    }

#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_SRICTACHOMETERLLI_CHECKS);
#endif

}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Getter of SRICTachometer module.
 *
 * @details It return the SRICTachometerLLI data. It contains a buffer with the raw counts from the IC, as the Zero rpm detection.
 *          No integrity checks on RAM variables performed in this function since returning bad data is no different
 *          than returning NULL. The Handler() function will ensure a safe reaction when the integrity check fails.
 *
 * @param  input
 *
 * @return SRICTachometerLLI_Data[index]
 */
void *SRICTachometerLLI__Get(HBL_LLI_TYPE * input)
{
    void *ret_val = NULL;

    if (input->Data_Index < SRICTachometerLLI_Allocation)
    {
        ret_val = ((void *)&SRICTachometerLLI_Data[input->Data_Index]);
    }

    return(ret_val);
}
//---------------------------------------------------------------------------------------------------------------------

/**
 * @brief   Input Capture of SRICTachometer module.
 *
 * @details It works as a callback from IntMgr. It fills the SRICTachometer buffer with the capture durations (in counts).
 *
 * @param   HBL_LLI_TYPE * input
 *
 * @return  Returns TRUE once it gets a valid sample. It can be tracked by the sequence at HBL.
 */
BOOL_TYPE SRICTachometerLLI__ICProcess(HBL_LLI_TYPE * input)
{

    SRICTACHOMETER_DATA_TYPE *data;
    BOOL_TYPE crc_check;
    BOOL_TYPE retval;
    uint32 captured_duration;
    uint32 lower_limit;
    uint32 upper_limit;

    retval = FALSE;

    if ((SR_CHECK_BYTE(SRICTachometerLLI_Allocation) == SRDATA_OK)
        && (input->Data_Index < SRICTachometerLLI_Allocation))
    {
        data = (SRICTACHOMETER_DATA_TYPE *)(void *)&SRICTachometerLLI_Data[input->Data_Index];

        crc_check = (BOOL_TYPE) (Crc8(0xFF, (uint8 *)(void *)data, sizeof(SRICTachometerLLI_Data[0])) == SRICTachometerLLI_Data_Crc[input->Data_Index]);

        if (crc_check)
        {
            //Read the current captured duration and duty
            captured_duration = Timer__GetCaptureDuration(&data->Timer_Parameter);
            if (captured_duration >= TACHO_MINUMUM_COUNT_VALUE)
            {
                // The limit is: previous_speed/8 (Delta of 25%).
                lower_limit = data->Public_Data.Raw_Period - (data->Public_Data.Raw_Period >> 3);
                upper_limit = data->Public_Data.Raw_Period + (data->Public_Data.Raw_Period >> 3);

                if ((captured_duration <= upper_limit) &&
                    (captured_duration >= lower_limit))
                {
                    //Store the captured duration at the current buffer position
                    data->Public_Data.Raw_Period = captured_duration;
                }
                else
                {
                    // if speed is above upper limit
                    if (captured_duration >= upper_limit)
                    {
                        // Set period buffer to upper_limit
                        data->Public_Data.Raw_Period = upper_limit;
                    }
                    else
                    {
                        // Set period buffer to lower_limit
                        data->Public_Data.Raw_Period = lower_limit;
                    }
                }

                data->Accumulator += data->Public_Data.Raw_Period;
                data->Samples_Counter++;

                if (data->Samples_Counter >= SRICTACHOMETERLLI_AVERAGE_SAMPLES)
                {
                	// Take the average of raw period
                    data->Public_Data.Filtered_Period = data->Accumulator / SRICTACHOMETERLLI_AVERAGE_SAMPLES;

                    // Reset the variables
                	data->Samples_Counter = 0;
                	data->Accumulator = 0;
                }

                retval = TRUE; // Once it returns TRUE the Sequence gets updated.

            }

            // Reset the timer counting while IC is processing
            data->Timer_Counter = 0;

            //Update CRC
            SRICTachometerLLI_Data_Crc[input->Data_Index] = Crc8(0xFF, (uint8 *)(void *)data, sizeof(SRICTachometerLLI_Data[0]));
        }
    }
    return (retval);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#endif

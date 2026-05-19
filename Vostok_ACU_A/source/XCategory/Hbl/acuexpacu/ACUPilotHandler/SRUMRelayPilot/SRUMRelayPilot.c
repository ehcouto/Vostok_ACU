/**
 *  @file       SRUMRelayPilot.c
 *
 *  @brief      Pilot for Universal Motor driven with Triac + Relays
 *
 *  @defgroup   CLASS_B
 *
 *  @details    This module implements all feature to use a Triac + 2 Relay + Tap field relay circuit to drive an
 *              Universal Motor
 *
 *              Optional parameters:
 *              - SRUMRELAYPILOT_FORCE_OC - Set to ENABLED to use Output Compare to drive triac gate when ZC_HANDLER_TIMING_US != 0
 *              - SRUMRELAYPILOT_FALLBACK_SF_PARAMS - Macro to define fallback SF Params should be set if Parameter_Ptr is NULL
 *                  If not set, InitializeInstance will return FAIL
 *
 *              Function macros to define in SRUMRelayPilot_prv.h
 *              - PASS_FAIL_TYPE SRUMRELAYPILOT__CONTROL_INITIALIZE(HBL_LOAD_TYPE* load);
 *              Initialize the motor control module; returns PASS if process completed, FAIL if there were errors
 *
 *              - PASS_FAIL_TYPE SRUMRELAYPILOT__CONTROL_HANDLER(HBL_LOAD_TYPE* load, SRUMRELAYPILOT_CONTROL_PARAM_TYPE *param_ptr, SRUMRELAYPILOT_CONTROL_STATUS_TYPE *status_ptr);
 *              Motor control handler; it gets data from param_ptr and returns status in status_ptr variables.
 *              PASS if handler works properly; FAIL if there are errors (failures are not included on FAIL condition!)
 *
 *              - uint8 SRUMRELAYPILOT__GET_CSTM_TACHO_INDEX(HBL_LOAD_TYPE* load);              (Optional)
 *              Provides LLI index for tacho signal if no Setting file parameters are available
 *
 *              - SRUMRELAYPILOT_FALLBACK_SF_PARAMS                                             (Optional)
 *              Contains default Setting file parameters when no parameters are set; if not defined, initialization fails
 *
 *              - BOOL_TYPE SRUMRELAYPILOT__ISCONSISTENCYOK_ADDITIONAL_CHECK()                  (Optional)
 *              Adds an additional consistency check to be satisfied - e.g. ZeroCross signal is present
 *
 *              - BOOL_TYPE SRUMRELAYPILOT__ISSTOPCONDITION_ADDITIONAL_CHECK()
 *              Adds an additional condition to force motor stop - e.g. ZeroCross signal is lost
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  @copyright  Copyright 2019. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SRUMRelayPilot.h"
#if (HBL_PILOT_NUM_SRUMRELAY > 0)
#include "SRUMRelayPilot_prv.h"
#include "SRException.h"
#include "SRData.h"
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    #include "SRTaskSequence.h"
#endif
#include "Hbl.h"
#include "Gpio.h"
#include "VirtualPin.h"
#include "string.h"

#if (ZC_HANDLER_TIMING_US == 0) || (ZC_NUM_GPIO_REQUESTS == 0) || (SRUMRELAYPILOT_FORCE_OC == ENABLED)
#include "Timer.h"
#endif

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define     PULSE_WIDTH_US                      500
#define     MAX_ANGLE_VALUE                     ((1 << 15) - 1)
#define     MIN_ANGLE                           (15 * (1 << 15) / 1000)
#define     MAX_ANGLE                           (850 * (1 << 15) / 1000)
#define     THRES_ANGLE                         (300 * (1 << 15) / 1000)
#define     TACHO_TIMING_SCALE                  (100/5)

#ifndef     SRUMRELAYPILOT_FORCE_OC
#define     SRUMRELAYPILOT_FORCE_OC             DISABLED
#endif

#ifndef     SRUMRELAYPILOT_TRIAC_DEBOUNCE_CNT
#define     SRUMRELAYPILOT_TRIAC_DEBOUNCE_CNT   3
#endif

#ifndef     SRUMRELAYPILOT__ISCONSISTENCYOK_ADDITIONAL_CHECK
#define     SRUMRELAYPILOT__ISCONSISTENCYOK_ADDITIONAL_CHECK()      TRUE
#endif

#ifndef     SRUMRELAYPILOT__ISSTOPCONDITION_ADDITIONAL_CHECK
#define     SRUMRELAYPILOT__ISSTOPCONDITION_ADDITIONAL_CHECK()      FALSE
#endif

typedef PACKED struct UNIVERSALMOTORPILOT_SF_PARAM_STRUCT
{
    uint8 Tacho_Fb_Index;
    uint8 Fbk_Debounce_Time;
    uint8 Tapfield_Suspend_Time;
    uint8 Tacho_Timeout;
    uint16 Angle_Fbk_Threshold;         // Q15 format
} SRUMRELAYPILOT_SF_PARAM_TYPE;

#ifdef      SRUMRELAYPILOT_FALLBACK_SF_PARAMS
	static SRUMRELAYPILOT_SF_PARAM_TYPE const SRUMRelayPilot_Default_SF_Params = SRUMRELAYPILOT_FALLBACK_SF_PARAMS;
#endif

typedef PACKED struct SRUMRELAYPILOT_DATA_STRUCT
{
    SRUMRELAYPILOT_PARAM_TYPE Requested;
    SRUMRELAYPILOT_PARAM_TYPE Processed;
    #if (ZC_HANDLER_TIMING_US > 0) && (ZC_NUM_GPIO_REQUESTS > 0) && (SRUMRELAYPILOT_FORCE_OC == DISABLED)
        uint16 Triac_Id;
    #else
        TIMER_PARAMETER_TYPE OC_Timer_Config;
    #endif
    uint16 Time_Cnt;
    uint16 Tacho_Cnt;
    uint8 Tacho_Seq_Id;
    uint8 ZeroCross_Sequence;
    uint8 Enable_Triac;
    uint8 Triac_Check_Debounce;
    SRUMRELAYPILOT_STATE_TYPE State;
    SRUMRELAYPILOT_STATUS_TYPE Status;
} SRUMRELAYPILOT_DATA_TYPE;

static uint8 SRUMRelayPilot_Allocation;
SRUMRELAYPILOT_DATA_TYPE PACKED SRUMRelayPilot_Data[HBL_PILOT_NUM_SRUMRELAY];

static uint8 N_SRUMRelayPilot_Allocation;
SRUMRELAYPILOT_DATA_TYPE PACKED N_SRUMRelayPilot_Data[HBL_PILOT_NUM_SRUMRELAY];


//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static PASS_FAIL_TYPE CheckPin(uint8 pin, uint8 value, uint8 active_value, uint8 debounce_counter);
static BOOL_TYPE IsConsistencyOK(HBL_CONSISTENCY_TYPE consistency);
static BOOL_TYPE IsStopCommand(SRUMRELAYPILOT_DATA_TYPE *data_ptr);
static void StopMotor(SRUMRELAYPILOT_DATA_TYPE *data_ptr, SRUMRELAYPILOT_DATA_TYPE *n_data_ptr, SRUMRELAYPILOT_SF_PARAM_TYPE *param);
static void UpdatePin(uint8 pin, uint8 value, uint8 active_value);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Initializes the SRUMRelayPilot module and its variables
 */
void SRUMRelayPilot__Initialize(void)
{
    SRData__UpdateByte((void *) &SRUMRelayPilot_Allocation, (void *) &N_SRUMRelayPilot_Allocation, 0);
    SRData__Fill((void *) SRUMRelayPilot_Data, (void *) N_SRUMRelayPilot_Data, 0, sizeof(SRUMRelayPilot_Data));
    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_INIT, SRTASKSEQUENCE_APPL_INIT_SRUMRELAY_PILOT);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Checks if there is free space and reserve a new instance
 * @return  index of SRUMRelayPilot instance, HBL_INDEX_INVALID if all HBL_PILOT_NUM_SRUMRELAY are already allocated
 */
uint8 SRUMRelayPilot__Allocate(void)
{
    uint8 retval = HBL_INDEX_INVALID;

    if ((SRDATA_OK == SRData__CheckByte((void *)&SRUMRelayPilot_Allocation,(void *)&N_SRUMRelayPilot_Allocation)) &&
        (SRUMRelayPilot_Allocation < HBL_PILOT_NUM_SRUMRELAY))
    {
        retval = SRUMRelayPilot_Allocation;
        SRData__UpdateByte((void *)&SRUMRelayPilot_Allocation, (void *)&N_SRUMRelayPilot_Allocation, SRUMRelayPilot_Allocation+1);
    }

    return (retval);

}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      Initializes a new instance of SRUMRelayPilot
 * @param      load     pointer to HBL_LOAD_TYPE structure reserved for that instance
 * @retval     PASS      if instance initialization succeeded
 * @retval     FAIL      if there were errors
 */
PASS_FAIL_TYPE  SRUMRelayPilot__InitializeInstance(HBL_LOAD_TYPE *load)
{
    PASS_FAIL_TYPE ret_val = FAIL;

    if((SRDATA_OK == SRData__CheckByte((void *)&SRUMRelayPilot_Allocation,(void *)&N_SRUMRelayPilot_Allocation)) &&
        (load->Pilot_Data_Index < SRUMRelayPilot_Allocation))
    {
        SRUMRELAYPILOT_DATA_TYPE *data_ptr = &SRUMRelayPilot_Data[load->Pilot_Data_Index];
        SRUMRELAYPILOT_DATA_TYPE *n_data_ptr = &N_SRUMRelayPilot_Data[load->Pilot_Data_Index];
        HBL_LOAD_STATUS_TYPE load_value;
        uint8 *pin_ptr = load->Pin_Ptr;
        uint8 hi_lo_bitmap = load->Hi_Low_Activation_Bitmap;
        uint8 pin_value = 0;
        ret_val = PASS;

        if (SRDATA_OK != SRData__CheckArrayNonBlocking(data_ptr, n_data_ptr, sizeof(SRUMRELAYPILOT_DATA_TYPE)))
        {
            ret_val = FAIL;
        }


        if (load->Pin_Ptr == NULL)
        {
            ret_val = FAIL;
        }

        #ifndef SRUMRELAYPILOT_FALLBACK_SF_PARAMS
            if (load->Parameter_Ptr.Data == NULL)
            {
                ret_val = FAIL;
            }
        #endif
        SRData__UpdateByte((void *) &data_ptr->State, (void *) &n_data_ptr->State, SRUMRELAYPILOT_INITALIZE);
        load_value = data_ptr->Status.Load;
        load_value.Load.Status = HBL_LOAD_STATUS_INITIALIZE;
        SRData__UpdateByte(&data_ptr->Status.Load.Byte, &n_data_ptr->Status.Load.Byte, load_value.Byte);
        SRData__UpdateByte(&data_ptr->ZeroCross_Sequence, &n_data_ptr->ZeroCross_Sequence, 0);

        // Initialize Motor Control
        ret_val &= SRUMRELAYPILOT__CONTROL_INITIALIZE(load);
        // Check Triac pin, Cw Pin, Ccw_Pin, Tap Field pin
        ret_val &= (PASS_FAIL_TYPE) (pin_ptr[0] < VirtualPin__GetNumOf());
        ret_val &= (PASS_FAIL_TYPE) (pin_ptr[1] < VirtualPin__GetNumOf());
        ret_val &= (PASS_FAIL_TYPE) (pin_ptr[2] < VirtualPin__GetNumOf());
        ret_val &= (PASS_FAIL_TYPE) (pin_ptr[3] < VirtualPin__GetNumOf());

        if (ret_val == PASS)
        {
            VIRTUALPIN_TYPE *virtual_pin_ptr = (void *) &VIRTUALPIN_TABLE[pin_ptr[0]];

            // Configure Triac
            #if (ZC_HANDLER_TIMING_US > 0) && (ZC_NUM_GPIO_REQUESTS > 0) && (SRUMRELAYPILOT_FORCE_OC == DISABLED)
            SRData__UpdateByte((void *) &data_ptr->Triac_Id, (void *) &n_data_ptr->Triac_Id,
                    ZeroCross__RegisterGpioActivity(virtual_pin_ptr->port, virtual_pin_ptr->pin));
                ret_val &= (data_ptr->Triac_Id != ZEROCROSS_ID_INVALID);
            #else
                // Output Compare
                SRData__UpdateByte((void *) &data_ptr->OC_Timer_Config.Port, (void *) &n_data_ptr->OC_Timer_Config.Port, virtual_pin_ptr->port);
                SRData__UpdateByte((void *) &data_ptr->OC_Timer_Config.Pin, (void *) &n_data_ptr->OC_Timer_Config.Pin, virtual_pin_ptr->pin);
                SRData__UpdateByte((void *) &data_ptr->OC_Timer_Config.Polarity, (void *) &n_data_ptr->OC_Timer_Config.Polarity,
                    (hi_lo_bitmap & 0x01) ? TIMER_POLARITY_ACTIVE_HIGH : TIMER_POLARITY_ACTIVE_LOW);
                SRData__UpdateByte((void *) &data_ptr->OC_Timer_Config.Timer, (void *) &n_data_ptr->OC_Timer_Config.Timer, virtual_pin_ptr->device);
                SRData__UpdateByte((void *) &data_ptr->OC_Timer_Config.Channel, (void *) &n_data_ptr->OC_Timer_Config.Channel, virtual_pin_ptr->param0);
                SRData__UpdateByte((void *) &data_ptr->OC_Timer_Config.Mode, (void *) &n_data_ptr->OC_Timer_Config.Mode, TIMER_MODE_DISABLE);
                ret_val = Timer__Config(&data_ptr->OC_Timer_Config);
            #endif

                // Configure Pins
                UpdatePin(pin_ptr[0], pin_value, hi_lo_bitmap & 0x01);
                UpdatePin(pin_ptr[1], pin_value, (hi_lo_bitmap >> 1) & 0x01);
                UpdatePin(pin_ptr[2], pin_value, (hi_lo_bitmap >> 2) & 0x01);
                UpdatePin(pin_ptr[3], pin_value, (hi_lo_bitmap >> 3) & 0x01);

                // Check pin configuration
                ret_val &= CheckPin(pin_ptr[0], pin_value, hi_lo_bitmap & 0x01, SRUMRELAYPILOT_TRIAC_DEBOUNCE_CNT);
                ret_val &= CheckPin(pin_ptr[1], pin_value, (hi_lo_bitmap >> 1) & 0x01, SRUMRELAYPILOT_TRIAC_DEBOUNCE_CNT);
                ret_val &= CheckPin(pin_ptr[2], pin_value, (hi_lo_bitmap >> 2) & 0x01, SRUMRELAYPILOT_TRIAC_DEBOUNCE_CNT);
                ret_val &= CheckPin(pin_ptr[3], pin_value, (hi_lo_bitmap >> 3) & 0x01, SRUMRELAYPILOT_TRIAC_DEBOUNCE_CNT);

                SRData__UpdateByte((void *) &data_ptr->State, (void *) &n_data_ptr->State, SRUMRELAYPILOT_IDLE);
        }
        else
        {
            load_value = data_ptr->Status.Load;
            load_value.Load.Status = HBL_LOAD_STATUS_FAILURE;
            SRData__UpdateByte((void *) &data_ptr->Status.Load.Byte, (void *) &n_data_ptr->Status.Load.Byte, load_value.Byte);
        }
    }

    return(ret_val);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Set SRUMRelayPilot instance request.
 *
 *  @param      load - Pointer to associated DoorLockEMPilot instance.
 *  @param      data - Pointer to request data. Pass argument as uint8 *.
 *
 *  @return     PASS_FAIL_TYPE result - Result of SRUMRelayPilot request.
 *  @retval     PASS - Instance request succeeded.
 *  @retval     FAIL - Instance request failed.
 */
PASS_FAIL_TYPE  SRUMRelayPilot__Request(HBL_LOAD_TYPE *load, void *data)
{
    PASS_FAIL_TYPE result = FAIL;

    if ((SRDATA_OK == SRData__CheckByte(&SRUMRelayPilot_Allocation, &N_SRUMRelayPilot_Allocation)) &&
        (load->Pilot_Data_Index < SRUMRelayPilot_Allocation))
    {
        SRUMRELAYPILOT_DATA_TYPE *data_ptr = &SRUMRelayPilot_Data[load->Pilot_Data_Index];
        SRUMRELAYPILOT_DATA_TYPE *n_data_ptr = &N_SRUMRelayPilot_Data[load->Pilot_Data_Index];
        if ((SRDATA_OK == SRData__CheckArrayNonBlocking((void *)data_ptr,(void *)n_data_ptr, sizeof(SRUMRELAYPILOT_DATA_TYPE))) &&
            (data_ptr->State != SRUMRELAYPILOT_INITALIZE))
        {
            SRData__UpdateArray((void *) &data_ptr->Requested, (void *) &n_data_ptr->Requested,
                (void *)data, sizeof(SRUMRELAYPILOT_PARAM_TYPE));
            result = PASS;
        }
    }

    return (result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Process SRUMRelayPilot instance.
 *
 *  @param      load - Pointer to SRUMRelayPilot instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of SRUMRelayPilot instance process.
 *  @retval     PASS - Instance process succeeded.
 *  @retval     FAIL - Instance process failed.
 */
PASS_FAIL_TYPE  SRUMRelayPilot__Process(HBL_LOAD_TYPE *load)
{
    PASS_FAIL_TYPE result = FAIL;

    if ((SRDATA_OK == SRData__CheckByte((void *)&SRUMRelayPilot_Allocation,(void *)&N_SRUMRelayPilot_Allocation)) &&
        (load->Pilot_Data_Index < SRUMRelayPilot_Allocation))
    {
        SRUMRELAYPILOT_DATA_TYPE *data_ptr = &SRUMRelayPilot_Data[load->Pilot_Data_Index];
        SRUMRELAYPILOT_DATA_TYPE *n_data_ptr = &N_SRUMRelayPilot_Data[load->Pilot_Data_Index];
        if ((SRDATA_OK == SRData__CheckArrayNonBlocking((void *)data_ptr,(void *)n_data_ptr, sizeof(SRUMRELAYPILOT_DATA_TYPE))) &&
            (SRUMRelayPilot_Data[load->Pilot_Data_Index].State != SRUMRELAYPILOT_INITALIZE))
        {
            SRData__UpdateArray((void *) &data_ptr->Processed, (void *) &n_data_ptr->Processed,
                (void *) &data_ptr->Requested, sizeof(SRUMRELAYPILOT_PARAM_TYPE));
            result = PASS;
        }
    }

    return (result);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Class B Data Integrity check handler
 * It should be run on Application thread before SRHbl__Process()
 * It checks SRUMRelayPilot_Allocation and SRUMRelayPilot_Data integrity
 * It marks SRTASKSEQUENCE_APPL_SRUMRELAYPILOT_CHECK step on SRTASKSEQUENCE_APPL_SAFETY_THREAD sequence
 */
void SRUMRelayPilot__Handler(void)
{

    if (SRDATA_OK != SRData__CheckByte((void *)&SRUMRelayPilot_Allocation,(void *)&N_SRUMRelayPilot_Allocation))
    {
        SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, 0x00, 0xAB);
        // System will reset after this exception occurs
    }
    else
    {
        for (uint8 index = 0; index < HBL_PILOT_NUM_SRUMRELAY; index++)
        {
            if (SRData__CheckArrayNonBlocking((uint8 *)(void *)&SRUMRelayPilot_Data[index],
                (uint8 *)(void *)&N_SRUMRelayPilot_Data[index], sizeof(SRUMRELAYPILOT_DATA_TYPE)) != SRDATA_OK)
            {
                SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, 0x01, index);
                // System will reset after this exception occurs
            }
        }
    }

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_SRUMRELAYPILOT_CHECK);
    #endif

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      ZeroCross event processing for SRUMRelayPilot.
 *
 *  @details     ZeroCross process handles Triac activation (if requested) and ZeroCross sequence counter
 *
 *  @param      load - Pointer to SRUMRelayPilot instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of SRUMRelayPilot asynch execution.
 *  @retval     PASS - Execution succeeded.
 *  @retval     FAIL - Execution failed.
 */
PASS_FAIL_TYPE  SRUMRelayPilot__ZcProcess(HBL_LOAD_TYPE *load)
{
    PASS_FAIL_TYPE ret_val = FAIL;

    if ((SRDATA_OK == SRData__CheckByte((void *)&SRUMRelayPilot_Allocation,(void *)&N_SRUMRelayPilot_Allocation)) &&
        (load->Pilot_Data_Index < SRUMRelayPilot_Allocation))
    {
        SRUMRELAYPILOT_DATA_TYPE *data_ptr = &SRUMRelayPilot_Data[load->Pilot_Data_Index];
        SRUMRELAYPILOT_DATA_TYPE *n_data_ptr = &N_SRUMRelayPilot_Data[load->Pilot_Data_Index];

        if ((SRDATA_OK == SRData__CheckArrayNonBlocking((void *)data_ptr,(void *)n_data_ptr, sizeof(SRUMRELAYPILOT_DATA_TYPE))) &&
            (data_ptr->State != SRUMRELAYPILOT_INITALIZE))
        {
            ZEROCROSS_EVENT_TYPE last_event = ZeroCross__GetLastEvent();
            BOOL_TYPE firing_triac = FALSE;

            // Update ZeroCross_Sequence
            if(last_event != ZEROCROSS_BLACKOUT_EVENT)
            {
                uint8 zc_next_seq = data_ptr->ZeroCross_Sequence + 1;
                if (zc_next_seq == 0)
                {
                    zc_next_seq++;
                }
                SRData__UpdateByte(&data_ptr->ZeroCross_Sequence, &n_data_ptr->ZeroCross_Sequence, zc_next_seq);
            }

            if((last_event != ZEROCROSS_BLACKOUT_EVENT) &&
                (data_ptr->Enable_Triac == TRUE))
            {
                uint32 zc_half_period = ZeroCross__GetCycleCount()/2;
                uint16 zc_latency = ZeroCross__GetTimeFromLastEvent();
                ret_val = PASS;

                #if (ZC_HANDLER_TIMING_US > 0) && (ZC_NUM_GPIO_REQUESTS > 0) && (SRUMRELAYPILOT_FORCE_OC == DISABLED)
                    uint16 angle = (data_ptr->Status.Firing_Angle_S15 * zc_half_period) >> 15;
                    ZEROCROSS_GPIO_REQUEST_TYPE triac_req;

                    angle -= zc_latency;
                    triac_req.Active_Degree = (uint8) ((angle * ZEROCROSS_HALF_WAVE_ANGLE) / zc_half_period);
                    angle += PULSE_WIDTH_US;
                    triac_req.Deactive_Degree = (uint8) ((angle * ZEROCROSS_HALF_WAVE_ANGLE) / zc_half_period);
                    triac_req.Active_value = (BOOL_TYPE) (load->Hi_Low_Activation_Bitmap & 0x01);
                    triac_req.Gpio_Id = data_ptr->Triac_Id;
                    ZeroCross__RequestGpioActivity(&triac_req);
                #else
                    sint32 oc_timeout = (zc_half_period * data_ptr->Status.Firing_Angle_S15  + 16384) >> 15;
                    TIMER_ONCOMPARE_TYPE oc_type = (load->Hi_Low_Activation_Bitmap & 0x01) ? TIMER_ONCOMPARE_PIN_HIGH_INTERRUPT : TIMER_ONCOMPARE_PIN_LOW_INTERRUPT;

                    if ((data_ptr->Status.Firing_Angle_S15 < MIN_ANGLE) ||
                        (oc_timeout < zc_latency))
                    {
                        oc_timeout =  (zc_half_period*MIN_ANGLE)>>15;
                    }
                    else
                    {
                        if (data_ptr->Status.Firing_Angle_S15 > MAX_ANGLE)
                        {
                            oc_timeout = (zc_half_period * MAX_ANGLE)>>15;
                        }
                        oc_timeout -= zc_latency;
                    }

                    ret_val &= Timer__ConfigCheck(&data_ptr->OC_Timer_Config);
                    SRData__UpdateByte((void *) &data_ptr->OC_Timer_Config.Mode,
                        (void *) &n_data_ptr->OC_Timer_Config.Mode, TIMER_MODE_OUTPUT_COMPARE);
                    ret_val &= Timer__Config(&data_ptr->OC_Timer_Config);
                    Timer__OutputCompareStart(&data_ptr->OC_Timer_Config, oc_type, oc_timeout);
                #endif
                firing_triac = TRUE;
            }
            else
            {
                uint8 *pin_ptr = load->Pin_Ptr;
                uint8 hi_lo_bitmap = load->Hi_Low_Activation_Bitmap;
                uint8 new_debounce_counter = data_ptr->Triac_Check_Debounce+1;

                ret_val = CheckPin(pin_ptr[0], OFF, hi_lo_bitmap & 0x01, new_debounce_counter);
                if (ret_val == PASS)
                {
                    #if (ZC_HANDLER_TIMING_US == 0) || (ZC_NUM_GPIO_REQUESTS == 0) || (SRUMRELAYPILOT_FORCE_OC == ENABLED)
                        // Keep Timer disabled
                        ret_val &= Timer__ConfigCheck(&data_ptr->OC_Timer_Config);
                        if(data_ptr->OC_Timer_Config.Mode != TIMER_MODE_DISABLE)
                        {
                            SRData__UpdateByte((void *) &data_ptr->OC_Timer_Config.Mode,
                                (void *) &n_data_ptr->OC_Timer_Config.Mode, TIMER_MODE_DISABLE);
                            ret_val &= Timer__Config(&data_ptr->OC_Timer_Config);
                        }
                    #endif
                    UpdatePin(pin_ptr[0], OFF, hi_lo_bitmap & 0x01);
                    new_debounce_counter = 0;
                }

                SRData__UpdateByte((void *) &data_ptr->Triac_Check_Debounce,
                    (void *) &n_data_ptr->Triac_Check_Debounce, new_debounce_counter);
            }

            SRData__UpdateByte((void *) &data_ptr->Status.Is_Triac_Firing,
                (void *) &n_data_ptr->Status.Is_Triac_Firing, firing_triac);
        }
    }
    return(ret_val);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Asynchronous process for SRUMRelayPilot.
 *
 *  @details    Asynchronous process handles motor pilot state machine and sequences between Relays and Triac
 *
 *  @param      load - Pointer to SRUMRelayPilot instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of SRUMRelayPilot asynch execution.
 *  @retval     PASS - Execution succeeded.
 *  @retval     FAIL - Execution failed.
 */
PASS_FAIL_TYPE  SRUMRelayPilot__AsyncProcess(HBL_LOAD_TYPE *load)
{
    PASS_FAIL_TYPE ret_val = FAIL;

    if ((SRDATA_OK == SRData__CheckByte((void *)&SRUMRelayPilot_Allocation,(void *)&N_SRUMRelayPilot_Allocation)) &&
        (load->Pilot_Data_Index < SRUMRelayPilot_Allocation))
    {
        SRUMRELAYPILOT_DATA_TYPE *data_ptr = &SRUMRelayPilot_Data[load->Pilot_Data_Index];
        SRUMRELAYPILOT_DATA_TYPE *n_data_ptr = &N_SRUMRelayPilot_Data[load->Pilot_Data_Index];

        if (SRDATA_OK == SRData__CheckArrayNonBlocking((void *)data_ptr,(void *)n_data_ptr, sizeof(SRUMRELAYPILOT_DATA_TYPE)))
        {
            SRUMRELAYPILOT_SF_PARAM_TYPE *param_ptr = (void *)load->Parameter_Ptr.Data;
            uint8 hi_lo_bitmap = load->Hi_Low_Activation_Bitmap;
            uint8 *pin_ptr = load->Pin_Ptr;
            BOOL_TYPE enable_triac = FALSE;
            SRUMRELAYPILOT_CONTROL_STATUS_TYPE control_status;
            SRUMRELAYPILOT_CONTROL_PARAM_TYPE control_param;
            HBL_LOAD_STATUS_TYPE current_load_status;
            HBL_CONSISTENCY_TYPE own_consistency;

            // Initialize local variables
            current_load_status.Load.Cur_Reg = OFF;
            current_load_status.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
            own_consistency.Consistency.Table = HBL_CONSISTENCY_TABLE_INVALID;
            own_consistency.Consistency.Value = HBL_CONSISTENCY_INVALID_VALUE;
            memset(&control_status, 0, sizeof(control_status));

            #ifdef SRUMRELAYPILOT_FALLBACK_SF_PARAMS
                if (param_ptr == NULL)
                {
                    param_ptr = (void *) &SRUMRelayPilot_Default_SF_Params;
                }
            #endif
            if((data_ptr->State != SRUMRELAYPILOT_INITALIZE) &&
                (PASS == CheckPin(pin_ptr[1], data_ptr->Status.Cw_Relay, (hi_lo_bitmap >> 1) & 0x01, SRUMRELAYPILOT_TRIAC_DEBOUNCE_CNT)) &&
                (PASS == CheckPin(pin_ptr[2], data_ptr->Status.Ccw_Relay, (hi_lo_bitmap >> 2) & 0x01, SRUMRELAYPILOT_TRIAC_DEBOUNCE_CNT)) &&
                (PASS == CheckPin(pin_ptr[3], data_ptr->Status.Tapped_Field_Relay, (hi_lo_bitmap >> 3) & 0x01, SRUMRELAYPILOT_TRIAC_DEBOUNCE_CNT)))
            {
                uint16 tacho_cnt = data_ptr->Tacho_Cnt;
                uint8 tacho_index = param_ptr->Tacho_Fb_Index;
                uint8 tacho_seq_id;

                #ifdef SRUMRELAYPILOT__GET_CSTM_TACHO_INDEX
                    if (tacho_index >= Hbl__GetNumLLI())
                    {
                        tacho_index = SRUMRELAYPILOT__GET_CSTM_TACHO_INDEX(load);
                    }
                #endif

                // Decrement Feedback / Suspend time counter
                if (data_ptr->Time_Cnt > 0)
                {
                    SRData__UpdateShort((void *)&data_ptr->Time_Cnt,
                        (void *)&n_data_ptr->Time_Cnt, data_ptr->Time_Cnt-1);
                }

                // Check Tacho signal
                tacho_seq_id = Hbl__GetLLISequenceIDByIndex(tacho_index);
                if(data_ptr->Tacho_Seq_Id == tacho_seq_id)
                {
                    if (tacho_cnt > 0)
                    {
                        tacho_cnt--;
                    }
                }
                else
                {
                    tacho_cnt = param_ptr->Tacho_Timeout * TACHO_TIMING_SCALE;
                    SRData__UpdateByte(&data_ptr->Tacho_Seq_Id, &n_data_ptr->Tacho_Seq_Id, tacho_seq_id);
                }
                SRData__UpdateShort(&data_ptr->Tacho_Cnt, &n_data_ptr->Tacho_Cnt, tacho_cnt);

                if ((data_ptr->State != SRUMRELAYPILOT_IDLE) &&
                    (data_ptr->State != SRUMRELAYPILOT_STOP))
                {
                    current_load_status.Load.Cur_Reg = ON;
                    if ((data_ptr->State == SRUMRELAYPILOT_RUN) &&
                        (data_ptr->Status.Firing_Angle_S15 < param_ptr->Angle_Fbk_Threshold))
                    {
                        current_load_status.Load.Status = HBL_LOAD_STATUS_TARGET_ALT_TABLE;
                    }
                }
                own_consistency.Byte = Hbl__GetOwnLoadConsistency(load, current_load_status.Byte);

                // Execute Control Handler
                control_param.Processed = data_ptr->Processed;
                control_param.State = data_ptr->State;
                control_param.Tacho_Index = tacho_index;
                control_param.ZeroCross_Sequence = data_ptr->ZeroCross_Sequence;
                control_param.Is_Triac_Firing = data_ptr->Status.Is_Triac_Firing;
                control_param.Is_Running = data_ptr->Status.Is_Running;
                control_param.Is_Tacho_Signal = (BOOL_TYPE) (data_ptr->Tacho_Cnt != 0);

                if(SRUMRELAYPILOT__CONTROL_HANDLER(load, &control_param, &control_status) == PASS)
                {
                    SRData__UpdateLong((void *)&data_ptr->Status.Motor_Speed_S15,
                        (void *)&n_data_ptr->Status.Motor_Speed_S15, control_status.Motor_Speed_S15);
                    SRData__UpdateLong((void *)&data_ptr->Status.Motor_Speed_Ref_S15,
                        (void *)&n_data_ptr->Status.Motor_Speed_Ref_S15, control_status.Motor_Speed_Ref_S15);
                    SRData__UpdateByte((void *) &data_ptr->Status.Fault,
                        (void *)  &n_data_ptr->Status.Fault, control_status.Fault);
                    SRData__UpdateByte((void *) &data_ptr->Status.Has_Start_Failed,
                        (void *) &n_data_ptr->Status.Has_Start_Failed, control_status.Has_Start_Failed);
                    SRData__UpdateByte((void *) &data_ptr->Status.Is_Speed_Reached,
                        (void *) &n_data_ptr->Status.Is_Speed_Reached, control_status.Is_Speed_Reached);
                    SRData__UpdateByte((void *) &data_ptr->Status.Is_Running,
                        (void *) &n_data_ptr->Status.Is_Running, control_status.Is_Running);

                    if (data_ptr->Processed.Custom_Param.Data.Force_Tap_Field_On == ON)
                    {
                        control_status.Tapped_Field_Relay = ON;
                    }
                }
                else
                {
                    SRData__UpdateLong((void *)&data_ptr->Status.Motor_Speed_S15,
                        (void *)&n_data_ptr->Status.Motor_Speed_S15, 0);
                    SRData__UpdateLong((void *)&data_ptr->Status.Motor_Speed_Ref_S15,
                        (void *)&n_data_ptr->Status.Motor_Speed_Ref_S15, 0);
                    SRData__UpdateByte((void *) &data_ptr->Status.Fault,
                        (void *) &n_data_ptr->Status.Fault, TRUE);
                    SRData__UpdateByte((void *) &data_ptr->Status.Has_Start_Failed,
                        (void *) &n_data_ptr->Status.Has_Start_Failed, FALSE);
                    SRData__UpdateByte((void *) &data_ptr->Status.Is_Speed_Reached,
                        (void *) &n_data_ptr->Status.Is_Speed_Reached, TRUE);
                    SRData__UpdateByte((void *) &data_ptr->Status.Is_Running,
                        (void *) &data_ptr->Status.Is_Running, FALSE);
                    StopMotor(data_ptr, n_data_ptr, param_ptr);
                }

                ret_val = PASS;
            }
            else
            {
                HBL_LOAD_STATUS_TYPE load_status;
                load_status.Load.Cur_Reg = OFF;
                load_status.Load.Status = HBL_LOAD_STATUS_FAILURE;
                SRData__UpdateByte((void *) &data_ptr->Status.Load.Byte,
                    (void *) &n_data_ptr->Status.Load.Byte, load_status.Byte);
            }

            switch (data_ptr->State)
            {
                case SRUMRELAYPILOT_IDLE:
                    current_load_status.Load.Cur_Reg = OFF;
                    SRData__UpdateByte((void *)&data_ptr->Status.Tapped_Field_Relay,
                        (void *)&n_data_ptr->Status.Tapped_Field_Relay, OFF);
                    SRData__UpdateByte((void *)&data_ptr->Status.Is_Triac_Firing,
                        (void *)&n_data_ptr->Status.Is_Triac_Firing, FALSE);
                    SRData__UpdateByte((void *)&data_ptr->Status.Is_Running,
                        (void *)&n_data_ptr->Status.Is_Running, FALSE);

                    if (IsConsistencyOK(own_consistency) != TRUE)
                    {
                        SRData__UpdateShort((void *)&data_ptr->Time_Cnt,
                            (void *)&n_data_ptr->Time_Cnt, param_ptr->Fbk_Debounce_Time);
                    }

                    // New command will start if Consistency is OK, one direction relay is set and tacho is lost
                    if ((control_status.Cw_Relay != control_status.Ccw_Relay) &&
                        (control_status.Firing_Angle_S15 >= param_ptr->Angle_Fbk_Threshold) &&
                        (data_ptr->Time_Cnt == 0) &&
                        (data_ptr->Tacho_Cnt == 0))
                    {
                        SRData__UpdateByte((void *)&data_ptr->Status.Cw_Relay,
                            (void *)&n_data_ptr->Status.Cw_Relay, control_status.Cw_Relay);
                        SRData__UpdateByte((void *)&data_ptr->Status.Ccw_Relay,
                            (void *)&n_data_ptr->Status.Ccw_Relay, control_status.Ccw_Relay);
                        SRData__UpdateByte((void *)&data_ptr->State, (void *)&n_data_ptr->State, SRUMRELAYPILOT_START);
                        current_load_status.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
                        SRData__UpdateShort((void *)&data_ptr->Time_Cnt,
                            (void *)&n_data_ptr->Time_Cnt, param_ptr->Fbk_Debounce_Time);
                        SRData__UpdateByte((void *) &data_ptr->Status.Is_Speed_Reached,
                            (void *) &n_data_ptr->Status.Is_Speed_Reached, FALSE);
                    }
                    else
                    {
                        SRData__UpdateByte((void *)&data_ptr->Status.Cw_Relay,
                            (void *)&n_data_ptr->Status.Cw_Relay, data_ptr->Processed.Custom_Param.Data.Idle_Relays_On);
                        SRData__UpdateByte((void *)&data_ptr->Status.Ccw_Relay,
                            (void *)&n_data_ptr->Status.Ccw_Relay, data_ptr->Processed.Custom_Param.Data.Idle_Relays_On);
                        SRData__UpdateByte((void *)&data_ptr->Status.Is_Speed_Reached,
                            (void *)&n_data_ptr->Status.Is_Speed_Reached, (BOOL_TYPE)((data_ptr->Status.Fault == 0) &&
                                (data_ptr->Tacho_Cnt == 0)));
                        current_load_status.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
                    }
                    SRData__UpdateByte((void *)&data_ptr->Status.Load.Byte,
                        (void *)&n_data_ptr->Status.Load.Byte, current_load_status.Byte);
                    break;

                case SRUMRELAYPILOT_START:
                   if(IsStopCommand(data_ptr) ||
                       (control_status.Firing_Angle_S15 < param_ptr->Angle_Fbk_Threshold) ||
                       (control_status.Cw_Relay != data_ptr->Status.Cw_Relay) ||
                       (control_status.Ccw_Relay != data_ptr->Status.Ccw_Relay))
                    {
                        StopMotor(data_ptr, n_data_ptr, param_ptr);
                    }
                   else if (IsConsistencyOK(own_consistency) == TRUE)
                   {
                        if ((data_ptr->Time_Cnt == 0) &&
                            (data_ptr->Tacho_Cnt == 0))
                        {
                            enable_triac = TRUE;
                            SRData__UpdateByte((void *)&data_ptr->State, (void *)&n_data_ptr->State, SRUMRELAYPILOT_RUN);
                        }
                   }
                   else
                   {
                       SRData__UpdateShort((void *)&data_ptr->Time_Cnt,
                           (void *)&n_data_ptr->Time_Cnt, param_ptr->Fbk_Debounce_Time);
                   }
                   break;

                case SRUMRELAYPILOT_RUN:
                    if(IsStopCommand(data_ptr) ||
                         (control_status.Cw_Relay != data_ptr->Status.Cw_Relay) ||
                         (control_status.Ccw_Relay != data_ptr->Status.Ccw_Relay))
                    {
                        StopMotor(data_ptr, n_data_ptr, param_ptr);
                    }
                    else if (data_ptr->Status.Tapped_Field_Relay != control_status.Tapped_Field_Relay)
                    {
                        if (data_ptr->Status.Is_Triac_Firing == TRUE)
                        {
                            SRData__UpdateByte((void *)&data_ptr->State, (void *)&n_data_ptr->State, SRUMRELAYPILOT_SUSPEND);
                            SRData__UpdateShort((void *)&data_ptr->Time_Cnt,
                                (void *)&n_data_ptr->Time_Cnt, param_ptr->Tapfield_Suspend_Time);
                        }
                        else
                        {
                            SRData__UpdateByte((void *)&data_ptr->Status.Tapped_Field_Relay,
                                (void *)&n_data_ptr->Status.Tapped_Field_Relay, control_status.Tapped_Field_Relay);
                        }
                    }
                    else
                    {
                        enable_triac = TRUE;
                        current_load_status.Load.Cur_Reg = ON;
                        SRData__UpdateByte((void *)&data_ptr->Status.Load.Byte,
                            (void *)&n_data_ptr->Status.Load.Byte, current_load_status.Byte);
                    }
                    break;

                case SRUMRELAYPILOT_SUSPEND:
                    if(IsStopCommand(data_ptr) ||
                        (control_status.Cw_Relay != data_ptr->Status.Cw_Relay) ||
                        (control_status.Ccw_Relay != data_ptr->Status.Ccw_Relay))
                    {
                        StopMotor(data_ptr, n_data_ptr, param_ptr);
                    }
                    else if ((data_ptr->Status.Tapped_Field_Relay == control_status.Tapped_Field_Relay) &&
                        (IsConsistencyOK(own_consistency) == FALSE))
                    {
                        SRData__UpdateShort((void *)&data_ptr->Time_Cnt,
                            (void *)&n_data_ptr->Time_Cnt, param_ptr->Fbk_Debounce_Time);
                    }
                    else if (data_ptr->Time_Cnt == 0)
                    {
                        if ((data_ptr->Status.Is_Triac_Firing == FALSE) &&
                            (data_ptr->Status.Tapped_Field_Relay != control_status.Tapped_Field_Relay))
                        {
                            SRData__UpdateByte((void *)&data_ptr->Status.Tapped_Field_Relay,
                                (void *)&n_data_ptr->Status.Tapped_Field_Relay, control_status.Tapped_Field_Relay);
                            SRData__UpdateShort((void *)&data_ptr->Time_Cnt,
                                (void *)&n_data_ptr->Time_Cnt, param_ptr->Tapfield_Suspend_Time);
                        }
                        else
                        {
                            enable_triac = TRUE;
                            SRData__UpdateByte((void *)&data_ptr->State, (void *)&n_data_ptr->State, SRUMRELAYPILOT_RUN);
                        }
                    }
                    break;

                case SRUMRELAYPILOT_STOP:
                    if ((data_ptr->Time_Cnt == 0) &&
                        (data_ptr->Status.Is_Triac_Firing == FALSE))
                    {
                        SRData__UpdateByte((void *)&data_ptr->State, (void *)&n_data_ptr->State, SRUMRELAYPILOT_IDLE);
                        SRData__UpdateShort((void *)&data_ptr->Time_Cnt, (void *)&n_data_ptr->Time_Cnt, param_ptr->Fbk_Debounce_Time);
                    }
                    break;

                default:
                    ret_val = FAIL;
                    break;
            }

            if (ret_val == PASS)
            {
                // Configure Relay Pin status
                UpdatePin(pin_ptr[1], data_ptr->Status.Cw_Relay, (hi_lo_bitmap >> 1) & 0x01);
                UpdatePin(pin_ptr[2], data_ptr->Status.Ccw_Relay, (hi_lo_bitmap >> 2) & 0x01);
                UpdatePin(pin_ptr[3], data_ptr->Status.Tapped_Field_Relay, (hi_lo_bitmap >> 3) & 0x01);

                // Set Triac Firing
                if((data_ptr->Processed.Custom_Param.Data.Disable_Triac != OFF) ||
                    (control_status.Firing_Angle_S15 >= MAX_ANGLE_VALUE))
                {
                    enable_triac = FALSE;
                }
                SRData__UpdateByte((void *)&data_ptr->Enable_Triac, (void *)&n_data_ptr->Enable_Triac, enable_triac);
                SRData__UpdateShort((void *)&data_ptr->Status.Firing_Angle_S15, (void *)&n_data_ptr->Status.Firing_Angle_S15,
                    ((enable_triac == TRUE) ? control_status.Firing_Angle_S15 : MAX_ANGLE_VALUE));
            }
        }
    }


    return(ret_val);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Output compare event processing for SRUMRelayPilot.
 *
 *  @details    Output compare process handles Triac deactivation (if it has been activated)
 *
 *  @param      load - Pointer to SRUMRelayPilot instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of SRUMRelayPilot asynch execution.
 *  @retval     PASS - Execution succeeded.
 *  @retval     FAIL - Execution failed.
 */
PASS_FAIL_TYPE  SRUMRelayPilot__OCProcess(HBL_LOAD_TYPE *load)
{
    PASS_FAIL_TYPE ret_val = FAIL;

#if (ZC_HANDLER_TIMING_US == 0) || (ZC_NUM_GPIO_REQUESTS == 0) || (SRUMRELAYPILOT_FORCE_OC == ENABLED)
    if ((SRDATA_OK == SRData__CheckByte((void *)&SRUMRelayPilot_Allocation,(void *)&N_SRUMRelayPilot_Allocation)) &&
        (load->Pilot_Data_Index < SRUMRelayPilot_Allocation))
    {
        SRUMRELAYPILOT_DATA_TYPE *data_ptr = &SRUMRelayPilot_Data[load->Pilot_Data_Index];
        SRUMRELAYPILOT_DATA_TYPE *n_data_ptr = &N_SRUMRelayPilot_Data[load->Pilot_Data_Index];

        if ((SRDATA_OK == SRData__CheckArrayNonBlocking((void *)data_ptr,(void *)n_data_ptr, sizeof(SRUMRELAYPILOT_DATA_TYPE))) &&
            (data_ptr->State != SRUMRELAYPILOT_INITALIZE))
        {
            uint8 *pin_ptr = load->Pin_Ptr;
            uint8 hi_lo_bitmap = load->Hi_Low_Activation_Bitmap;
            uint8 new_debounce_counter = data_ptr->Triac_Check_Debounce + 1;

            if (PASS == CheckPin(pin_ptr[0], ON, hi_lo_bitmap & 0x01, new_debounce_counter))
            {
                uint32 zc_half_period = ZeroCross__GetCycleCount()/2;
                sint32 angle_us = (zc_half_period * data_ptr->Status.Firing_Angle_S15  + 16384) >> 15;
                uint32 pulse = PULSE_WIDTH_US;

                if(angle_us < zc_half_period/2)
                {
                    pulse = zc_half_period/2 - angle_us + PULSE_WIDTH_US;
                }

                uint8 hi_lo_bitmap = load->Hi_Low_Activation_Bitmap;
                TIMER_ONCOMPARE_TYPE oc_type;

                (void) Timer__ConfigCheck(&data_ptr->OC_Timer_Config);
                oc_type = (hi_lo_bitmap & 0x01) ? TIMER_ONCOMPARE_PIN_LOW : TIMER_ONCOMPARE_PIN_HIGH;
                Timer__Config(&data_ptr->OC_Timer_Config);
                Timer__OutputCompareStart(&data_ptr->OC_Timer_Config, oc_type, pulse);
                new_debounce_counter = 0;
                ret_val = PASS;
            }
            SRData__UpdateByte((void *) &data_ptr->Triac_Check_Debounce,
                (void *) &n_data_ptr->Triac_Check_Debounce, new_debounce_counter);
        }
    }
#endif
    return(ret_val);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get current SRUMRelayPilot instance request.
 *
 *  @param      load - Pointer to associated SRUMRelayPilot instance.
 *
 *  @return     void *requested - Requested instance data.
 *  @retval     Address of requested instance data. Cast it using *((uint8 *)ptr) to get values.
 *  @retval     NULL - Invalid load index.
 */
void *SRUMRelayPilot__GetRequested(HBL_LOAD_TYPE *load)
{
    void *requested = NULL;

    if ((SRDATA_OK == SRData__CheckByte((void *)&SRUMRelayPilot_Allocation,(void *)&N_SRUMRelayPilot_Allocation)) &&
        (load->Pilot_Data_Index < SRUMRelayPilot_Allocation))
    {
        SRUMRELAYPILOT_DATA_TYPE *data_ptr = &SRUMRelayPilot_Data[load->Pilot_Data_Index];
        SRUMRELAYPILOT_DATA_TYPE *n_data_ptr = &N_SRUMRelayPilot_Data[load->Pilot_Data_Index];
        if (SRDATA_OK == SRData__CheckArrayNonBlocking((void *)data_ptr, (void *)n_data_ptr, sizeof(SRUMRELAYPILOT_DATA_TYPE)))
        {
            requested =  &data_ptr->Requested;
        }
    }

    return (requested);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get SRUMRelayPilot instance processed request.
 *
 *  @param      load - Pointer to associated SRUMRelayPilot instance.
 *
 *  @return     void * processed - Processed instance data.
 *  @retval     Address of processed instance data. Cast it using *((uint8 *)ptr) to get values.
 *  @retval     NULL - Invalid load index.
 */
void *SRUMRelayPilot__GetProcessed(HBL_LOAD_TYPE *load)
{
    void * processed = NULL;

    if ((SRDATA_OK == SRData__CheckByte((void *)&SRUMRelayPilot_Allocation,(void *)&N_SRUMRelayPilot_Allocation)) &&
        (load->Pilot_Data_Index < SRUMRelayPilot_Allocation))
    {
        SRUMRELAYPILOT_DATA_TYPE *data_ptr = &SRUMRelayPilot_Data[load->Pilot_Data_Index];
        SRUMRELAYPILOT_DATA_TYPE *n_data_ptr = &N_SRUMRelayPilot_Data[load->Pilot_Data_Index];
        if (SRDATA_OK == SRData__CheckArrayNonBlocking((void *)data_ptr, (void *)n_data_ptr, sizeof(SRUMRELAYPILOT_DATA_TYPE)))
        {
            processed =  &data_ptr->Processed;
        }
    }

    return (processed);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get SRUMRelayPilot instance status.
 *
 *  @param      load - Pointer to SRUMRelayPilot instance.
 *
 *  @return     void *status - instance Status.
 *  @retval     Address of instance Status. Cast it using *((HBL_LOAD_STATUS_TYPE *)ptr) to get the value.
 *  @retval     NULL - Invalid load index.
 */
void *SRUMRelayPilot__GetStatus(HBL_LOAD_TYPE *load)
{
    void *status_ptr = NULL;

    if ((SRDATA_OK == SRData__CheckByte((void *)&SRUMRelayPilot_Allocation,(void *)&N_SRUMRelayPilot_Allocation)) &&
        (load->Pilot_Data_Index < SRUMRelayPilot_Allocation))
    {
        SRUMRELAYPILOT_DATA_TYPE *data_ptr = &SRUMRelayPilot_Data[load->Pilot_Data_Index];
        SRUMRELAYPILOT_DATA_TYPE *n_data_ptr = &N_SRUMRelayPilot_Data[load->Pilot_Data_Index];
        if (SRDATA_OK == SRData__CheckArrayNonBlocking((void *)data_ptr, (void *)n_data_ptr, sizeof(SRUMRELAYPILOT_DATA_TYPE)))
        {
            status_ptr =  &data_ptr->Status;
        }
    }

    return (status_ptr);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Performs pin check according to value and active_value (from SF).
 *
 *  @param      pin - pin index on VirtualPin Table.
 *  @param      value - ON/OFF value to be set
 *  @param      active_value - Real pin value to set if ON
 *  @return     Pin check result: PASS if check passed
 */
PASS_FAIL_TYPE CheckPin(uint8 pin, uint8 value, uint8 active_value, uint8 debounce_counter)
{
    PASS_FAIL_TYPE result = FAIL;
    VIRTUALPIN_TYPE *virtual_pin_ptr = (VIRTUALPIN_TYPE *) &VIRTUALPIN_TABLE[pin]; //lint !e929 cast from pointer to pointer [MISRA 2004 Rule 11.4]
    BOOL_TYPE pin_value_read = Gpio__PinRead(virtual_pin_ptr->port, virtual_pin_ptr->pin);
    uint8 pin_value = value ^ ~active_value;
    pin_value &= 0x01;

    if( (pin_value == pin_value_read) ||
        (debounce_counter >= SRUMRELAYPILOT_TRIAC_DEBOUNCE_CNT))
    {
        if (TRUE == Gpio__CheckConfig(virtual_pin_ptr->port, virtual_pin_ptr->pin, virtual_pin_ptr->pin_config, (BOOL_TYPE) pin_value))
        {
            result = PASS;
        }
    }
    return(result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Performs pin update according to value and active_value (from SF).
 *
 *  @param      pin - pin index on VirtualPin Table.
 *  @param      value - ON/OFF value to be set
 *  @param      active_value - Real pin value to set if ON
 */
void UpdatePin(uint8 pin, uint8 value, uint8 active_value)
{
    VIRTUALPIN_TYPE *virtual_pin_ptr = (VIRTUALPIN_TYPE *) &VIRTUALPIN_TABLE[pin]; //lint !e929 cast from pointer to pointer [MISRA 2004 Rule 11.4]
    uint8 pin_value = value ^ ~active_value;
    pin_value &= 0x01;
    Gpio__PinWrite(virtual_pin_ptr->port, virtual_pin_ptr->pin, (BOOL_TYPE) pin_value);
    Gpio__PinConfig(virtual_pin_ptr->port, virtual_pin_ptr->pin, virtual_pin_ptr->pin_config);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Returns if consistency value can be considered good
 *              HBL_CONSISTENCY_TABLE_INVALID allows to bypass this check if no consistency table is set - for tests only!
 *
 *  @param      consistency - Consistency value to check
 *  @return     TRUE/FALSE if Consistency is considered good or not
 */
BOOL_TYPE IsConsistencyOK(HBL_CONSISTENCY_TYPE consistency)
{
    BOOL_TYPE consistency_ok = (BOOL_TYPE) SRUMRELAYPILOT__ISCONSISTENCYOK_ADDITIONAL_CHECK();
    consistency_ok &= (BOOL_TYPE) ((consistency.Consistency.Value == HBL_CONSISTENCY_OK) ||
        (consistency.Consistency.Table == HBL_CONSISTENCY_TABLE_INVALID));
    return(consistency_ok);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Returns if a stop command has been detected
 *              Stop command can be both Target_Speed and Acceleration 0 or both Cw and Ccw Relay at same status
 *
 *  @param      data_ptr - Pointer to instance data to check
 *  @return     TRUE/FALSE if a stop command has been detected or not
 */
BOOL_TYPE IsStopCommand(SRUMRELAYPILOT_DATA_TYPE *data_ptr)
{
    BOOL_TYPE stop_check = (BOOL_TYPE) SRUMRELAYPILOT__ISSTOPCONDITION_ADDITIONAL_CHECK();

    stop_check |= (BOOL_TYPE) (((data_ptr->Processed.Target_Speed == 0) &&
                    (data_ptr->Processed.Acceleration == 0)));
    stop_check |= (BOOL_TYPE) (data_ptr->Status.Cw_Relay == data_ptr->Status.Ccw_Relay);

    return(stop_check);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Execute transition to Stop or Idle status
 *
 *  @param      data_ptr - Pointer to instance data to check
 */
void StopMotor(SRUMRELAYPILOT_DATA_TYPE *data_ptr, SRUMRELAYPILOT_DATA_TYPE *n_data_ptr, SRUMRELAYPILOT_SF_PARAM_TYPE *param)
{
    if (data_ptr->State == SRUMRELAYPILOT_START)
    {
        SRData__UpdateByte((void *)&data_ptr->State, (void *)&n_data_ptr->State, SRUMRELAYPILOT_IDLE);
    }
    else if((data_ptr->State != SRUMRELAYPILOT_IDLE) &&
        (data_ptr->State != SRUMRELAYPILOT_STOP))
    {
        HBL_LOAD_STATUS_TYPE load_status = data_ptr->Status.Load;
        load_status.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
        load_status.Load.Cur_Reg = OFF;
        SRData__UpdateByte((void *)&data_ptr->Status.Load.Byte, (void *)&n_data_ptr->Status.Load.Byte, load_status.Byte);
        SRData__UpdateByte((void *)&data_ptr->State, (void *)&n_data_ptr->State, SRUMRELAYPILOT_STOP);
    }
    SRData__UpdateShort((void *)&data_ptr->Time_Cnt, (void *)&n_data_ptr->Time_Cnt, param->Fbk_Debounce_Time);
    SRData__UpdateByte((void *)&data_ptr->Enable_Triac, (void *)&n_data_ptr->Enable_Triac, OFF);
}


#endif

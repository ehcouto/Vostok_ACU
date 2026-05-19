/**
 *  @file       SRDoorLockEMPilot.c
 *
 *  @brief      Pilot Handler for EM DoorLock device with Class B requirements
 *
 *  @details    This module performs locking and unlocking process of an electromagnetic Doorlock device.
 *
 *	@defgroup   CLASS_B
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "SRSystemConfig.h"
#include "SRDoorLockEMPilot.h"

#if (HBL_PILOT_NUM_SRDOORLOCKEM > 0)
#include "SRData.h"
#include "SRTaskSequence.h"
#include "SRException.h"
#include "Hbl.h"
#include "Gpio.h"
#include "VirtualPin.h"
#include "string.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#define SRDOORLOCKEM_DEFAULT_PULSES           (3)
#define SRDOORLOCKEM_INITIAL_PERIOD           (18)
#define SRDOORLOCKEM_TIME_SPACING_UNIT_MS     (100)
#define SRDOORLOCKEM_FBK_TIMEOUT              (SRDOORLOCKEM_TIME_SPACING_UNIT_MS)

#define ZC_ALLOWED_MIN_FREQUENCY            45
#define ZC_ALLOWED_MAX_FREQUENCY            65

// Minimum and maximum period from ZeroCross frequency boundaries set - Round to near
#define SRDOORLOCKEM_MIN_PERIOD_MS		((2000+ZC_ALLOWED_MAX_FREQUENCY)/(ZC_ALLOWED_MAX_FREQUENCY*2))
#define SRDOORLOCKEM_MAX_PERIOD_MS		((2000+ZC_ALLOWED_MIN_FREQUENCY)/(ZC_ALLOWED_MIN_FREQUENCY*2))

typedef enum SRDOORLOCKEM_STATUS_ENUM
{
    SRDOORLOCKEM_STATUS_INIT,
    SRDOORLOCKEM_STATUS_DOOR_UNLOCKED,
    SRDOORLOCKEM_STATUS_DOOR_LOCKING,
    SRDOORLOCKEM_STATUS_DOOR_LOCKING_FAILED,
    SRDOORLOCKEM_STATUS_DOOR_LOCKED,
    SRDOORLOCKEM_STATUS_DOOR_UNLOCKING,
    SRDOORLOCKEM_STATUS_DOOR_UNLOCKING_FAILED
} SRDOORLOCKEM_STATUS_TYPE;

PACKED typedef struct
{
    uint16 Retries_Counter;
    uint8 Fbk_Counter;
    uint8 Fbk_Last_Counter;
    uint8 Period;
    uint8 Cycle_Counter;
    uint8 Fbk_Last_Pin_Value;
    uint8 Last_Pin_Out_Value;
    uint8 Fbk_Pin;
    uint8 Requested;
    uint8 Processed;
    HBL_LOAD_STATUS_TYPE PilotStatus;
    SRDOORLOCKEM_STATUS_TYPE Status;
    uint8 Request_Triac_On;
    uint8 Process_Triac_On;
    uint8 Keep_Triac_On;
    uint8 Retries;
} SRDOORLOCKEMPILOT_DATA_TYPE;

typedef struct
{
    uint8 Max_Retries :4;
    uint8 Num_Pulses  :4;
    uint8 Time_Spacing;
} SRDOORLOCKEMPILOT_PARAM_TYPE;

static uint8 SRDoorLockEMPilot_Allocation;
static SRDOORLOCKEMPILOT_DATA_TYPE SRDoorLockEMPilot_Data[HBL_PILOT_NUM_SRDOORLOCKEM];
static uint8 N_SRDoorLockEMPilot_Allocation;
static SRDOORLOCKEMPILOT_DATA_TYPE N_SRDoorLockEMPilot_Data[HBL_PILOT_NUM_SRDOORLOCKEM];

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static BOOL_TYPE CheckItem(uint8 index );

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module DoorLockEMPilot and its variables
 *
 */
void SRDoorLockEMPilot__Initialize(void )
{
    SRData__UpdateByte(&SRDoorLockEMPilot_Allocation, &N_SRDoorLockEMPilot_Allocation, 0);
    SRData__Fill(SRDoorLockEMPilot_Data, N_SRDoorLockEMPilot_Data, 0, sizeof(SRDoorLockEMPilot_Data));

#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_INIT, SRTASKSEQUENCE_APPL_INIT_SRDOORLOCKEMPILOT);
#endif

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Allocate pilot instance for DoorlockEMPilot.
 *
 *  @return     uint8 result - Allocation result.
 *  @retval     0-254 - Pilot allocation instance.
 *  @retval     0xFF - Allocation failed.
 */
uint8 SRDoorLockEMPilot__Allocate(void )
{
    uint8 retval = 0xFF;

    if (SRData__CheckByte(&SRDoorLockEMPilot_Allocation, &N_SRDoorLockEMPilot_Allocation) == SRDATA_OK)
    {
        if (SRDoorLockEMPilot_Allocation < HBL_PILOT_NUM_SRDOORLOCKEM)
        {
            retval = SRDoorLockEMPilot_Allocation;
            SRData__UpdateByte(&SRDoorLockEMPilot_Allocation, &N_SRDoorLockEMPilot_Allocation,
            SRDoorLockEMPilot_Allocation + 1);
        }
    }
    else
    {
        SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, HBL_PILOT_IMPULSES_DOOR_LOCK, 0xFF);
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Initialize DoorLockEMPilot instance.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated digital output load instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of DoorLockEMPilot instance initialization.
 *  @retval     PASS - Instance initialization succeeded.
 *  @retval     FAIL - Instance initialization failed.
 */
PASS_FAIL_TYPE SRDoorLockEMPilot__InitializeInstance(HBL_LOAD_TYPE * load )
{
    PASS_FAIL_TYPE retval = FAIL;
    uint8 index = load->Pilot_Data_Index;
    SRDOORLOCKEMPILOT_PARAM_TYPE *param_ptr = (SRDOORLOCKEMPILOT_PARAM_TYPE *) (load->Parameter_Ptr.Data); //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    SRDOORLOCKEMPILOT_DATA_TYPE *ptr;
    SRDOORLOCKEMPILOT_DATA_TYPE *n_ptr;
    HBL_LOAD_STATUS_TYPE tmp_load;
    BOOL_TYPE pin_val;
    uint8 *ptr_pin_table;
    uint8 fbk_index;
    uint8 virtual_pin;

    if ((CheckItem(index)) && (param_ptr != NULL ))
    {
        ptr = &SRDoorLockEMPilot_Data[index];
        n_ptr = &N_SRDoorLockEMPilot_Data[index];

        SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, (uint8) SRDOORLOCKEM_STATUS_INIT); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
        SRData__UpdateByte(&ptr->Period, &n_ptr->Period, (uint8) SRDOORLOCKEM_INITIAL_PERIOD);
        tmp_load.Load.Status = HBL_LOAD_STATUS_INITIALIZE;
        tmp_load.Load.Cur_Reg = 0;
        SRData__UpdateByte(&ptr->PilotStatus.Byte, &n_ptr->PilotStatus.Byte, tmp_load.Byte);
        SRData__UpdateShort(&ptr->Retries_Counter, &n_ptr->Retries_Counter, 0);

        // Check if a SRFeedback LLI is configured for pin synchronization
        fbk_index = Hbl__GetLLIIndexByOwnLoadConsistencyType(load, HBL_LLI_SRFEEDBACK);
        if (fbk_index >= Hbl__GetNumLLI())
        {
            // Fallback for backward compatibility - Check that this LLI ID is handled by a class B module to use it!!
            fbk_index = Hbl__GetLLIIndexByOwnLoadConsistencyType(load, HBL_LLI_FEEDBACK);
        }
        ptr_pin_table = Hbl__GetLLIPinTabPtr(fbk_index);

        if (ptr_pin_table == NULL)
        {
            virtual_pin = VirtualPin__GetNumOf();
        }
        else
        {
            virtual_pin = ptr_pin_table[0];
        }
        SRData__UpdateByte(&ptr->Fbk_Pin, &n_ptr->Fbk_Pin, virtual_pin);

        virtual_pin = load->Pin_Ptr[0];
        if ((virtual_pin < VirtualPin__GetNumOf()) && (VIRTUALPIN_TABLE[virtual_pin].port != 0xFF))
        {
            pin_val = (BOOL_TYPE) ~load->Hi_Low_Activation_Bitmap;
            pin_val &= TRUE;
            Gpio__PinWrite(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin, pin_val);
            Gpio__PinConfig(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin,
            VIRTUALPIN_TABLE[virtual_pin].pin_config);
            SRData__UpdateByte(&ptr->Last_Pin_Out_Value, &n_ptr->Last_Pin_Out_Value, pin_val);

            if (Gpio__CheckConfig(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin,
            VIRTUALPIN_TABLE[virtual_pin].pin_config, pin_val) == TRUE)
            {
                retval = PASS;
            }
        }

    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Set DoorLockEMPilot instance request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated DoorLockEMPilot instance.
 *
 *  @param      void *data - Pointer to request data. Pass argument as uint8 *.
 *
 *  @return     PASS_FAIL_TYPE result - Result of DoorLockEMPilot request.
 *  @retval     PASS - Instance request succeeded.
 *  @retval     FAIL - Instance request failed.
 */
PASS_FAIL_TYPE SRDoorLockEMPilot__Request(HBL_LOAD_TYPE * load ,void * data )
{
    PASS_FAIL_TYPE retval = FAIL;
    uint8 index = load->Pilot_Data_Index;

    if (CheckItem(index))
    {
        uint8 new_target = (*(uint8 *)data);
        uint8 new_triac_on = (new_target & SRDOORLOCKEM_PROCESSED_KEEP_PIN_MASK) ? TRUE : FALSE;
        new_target &= ~SRDOORLOCKEM_PROCESSED_KEEP_PIN_MASK;

        SRData__UpdateByte(&SRDoorLockEMPilot_Data[index].Requested, &N_SRDoorLockEMPilot_Data[index].Requested,
            new_target);
        SRData__UpdateByte(&SRDoorLockEMPilot_Data[index].Request_Triac_On, &N_SRDoorLockEMPilot_Data[index].Request_Triac_On,
            new_triac_on);
        retval = PASS;
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Process DoorLockEMPilot instance.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to DoorLockEMPilot instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of DoorLockEMPilot instance process.
 *  @retval     PASS - Instance process succeeded.
 *  @retval     FAIL - Instance process failed.
 */
PASS_FAIL_TYPE SRDoorLockEMPilot__Process(HBL_LOAD_TYPE * load )
{
    PASS_FAIL_TYPE retval = FAIL;
    uint8 index = load->Pilot_Data_Index;

    if (CheckItem(index))
    {
        SRDOORLOCKEMPILOT_PARAM_TYPE *param_ptr;
        SRDOORLOCKEMPILOT_DATA_TYPE *ptr;
        SRDOORLOCKEMPILOT_DATA_TYPE *n_ptr;

        param_ptr = (SRDOORLOCKEMPILOT_PARAM_TYPE *) (load->Parameter_Ptr.Data); //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
        ptr = &SRDoorLockEMPilot_Data[index];
        n_ptr = &N_SRDoorLockEMPilot_Data[index];

		// Prevent any state update if Time_Spacing time has not been elapsed
        if ((ptr->Retries_Counter >= (param_ptr->Time_Spacing * SRDOORLOCKEM_TIME_SPACING_UNIT_MS))
            && (ptr->Requested != ptr->Processed))
        {
            SRDOORLOCKEM_STATUS_TYPE new_status;
            HBL_LOAD_STATUS_TYPE tmp_load;
            HBL_CONSISTENCY_TYPE requested_consistency;

            tmp_load.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
            tmp_load.Load.Cur_Reg = ptr->Requested;
            requested_consistency.Byte = Hbl__GetOwnLoadConsistency(load, tmp_load.Byte);

            if (ptr->Requested == (uint8) OFF)
            {
                switch (ptr->Status)
                {
                    case SRDOORLOCKEM_STATUS_DOOR_LOCKED:
                        if (requested_consistency.Consistency.Value != HBL_CONSISTENCY_OK)
                        {
                            tmp_load.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
                            tmp_load.Load.Cur_Reg = ptr->PilotStatus.Load.Cur_Reg;
                            new_status = SRDOORLOCKEM_STATUS_DOOR_UNLOCKING;
                            SRData__UpdateByte(&ptr->Retries, &n_ptr->Retries, param_ptr->Max_Retries);
                        }
                        else
                        {
                            // If current consistency related to Requested value already match, go directly to
                            // new status - this prevents to change door lock status
                            new_status = SRDOORLOCKEM_STATUS_DOOR_UNLOCKED;
                        }
                        SRData__UpdateByte(&ptr->PilotStatus.Byte, &n_ptr->PilotStatus.Byte, tmp_load.Byte);
                        SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                        (uint8) new_status);
                        retval = PASS;
                        break;

                    case SRDOORLOCKEM_STATUS_DOOR_LOCKING_FAILED:
                        tmp_load.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
                        tmp_load.Load.Cur_Reg = ptr->PilotStatus.Load.Cur_Reg;
                        SRData__UpdateByte(&ptr->PilotStatus.Byte, &n_ptr->PilotStatus.Byte, tmp_load.Byte);
                        SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                            (uint8) SRDOORLOCKEM_STATUS_INIT);
                        retval = PASS;
                        break;

                    default:
                        break;
                }
            }
            // Everything else is considered Door Locked target
            else
            {
                switch (ptr->Status)
                {
                    case SRDOORLOCKEM_STATUS_DOOR_UNLOCKED:
                        if (requested_consistency.Consistency.Value != HBL_CONSISTENCY_OK)
                        {
                            tmp_load.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
                            tmp_load.Load.Cur_Reg = ptr->PilotStatus.Load.Cur_Reg;
                            new_status = SRDOORLOCKEM_STATUS_DOOR_LOCKING;
                            SRData__UpdateByte(&ptr->Retries, &n_ptr->Retries, param_ptr->Max_Retries);
                        }
                        else
                        {
                            // If current consistency related to Requested value already match, go directly to
                            // new status - this prevents to change door lock status
                            new_status = SRDOORLOCKEM_STATUS_DOOR_LOCKED;
                        }
                        SRData__UpdateByte(&ptr->PilotStatus.Byte, &n_ptr->PilotStatus.Byte, tmp_load.Byte);
                        SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                        (uint8) new_status);
                        retval = PASS;
                        break;

                    case SRDOORLOCKEM_STATUS_DOOR_UNLOCKING_FAILED:
                        tmp_load.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
                        tmp_load.Load.Cur_Reg = ptr->PilotStatus.Load.Cur_Reg;
                        SRData__UpdateByte(&ptr->PilotStatus.Byte, &n_ptr->PilotStatus.Byte, tmp_load.Byte);
                        SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                            (uint8) SRDOORLOCKEM_STATUS_INIT);
                        retval = PASS;
                        break;

                    default:
                        break;
                }
            }
        }
        else if ((ptr->Status == SRDOORLOCKEM_STATUS_INIT) ||
            (ptr->Requested  == ptr->Processed))
        {
            retval = PASS;
        }

        // Update Processed data only when status has been updated
        if (retval == PASS)
        {
            SRData__UpdateByte(&ptr->Processed, &n_ptr->Processed, ptr->Requested);
        }

        // Check Request_Triac_On and Process_Triac_On changes
        if (ptr->Request_Triac_On != ptr->Process_Triac_On)
        {
            /* If Process_Triac_On was set, pin has been forced ON and this could alter Door lock device status regardless of
             * Status instance field value; in order to align it properly and initialize will be performed.
             */
            if(ptr->Process_Triac_On)
            {
                HBL_LOAD_STATUS_TYPE tmp_load;

                tmp_load.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
                tmp_load.Load.Cur_Reg = ptr->PilotStatus.Load.Cur_Reg;
                SRData__UpdateByte(&ptr->PilotStatus.Byte, &n_ptr->PilotStatus.Byte, tmp_load.Byte);
                SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                    (uint8) SRDOORLOCKEM_STATUS_INIT);
            }
            SRData__UpdateByte(&ptr->Process_Triac_On, &n_ptr->Process_Triac_On,
                ptr->Request_Triac_On);
            retval = PASS;
        }

    }


    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get current DoorLockEMPilot instance request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated DoorLockEMPilot instance.
 *
 *  @return     void *requested - Requested instance data.
 *  @retval     Address of requested instance data. Cast it using *((uint8 *)ptr) to get values.
 *  @retval     NULL - Invalid load index.
 */
void * SRDoorLockEMPilot__GetRequested(HBL_LOAD_TYPE * load )
{
    void *ptr = NULL;

    if (load->Pilot_Data_Index < SRDoorLockEMPilot_Allocation)
    {
        ptr = (void *) (&SRDoorLockEMPilot_Data[load->Pilot_Data_Index].Requested);
    }

    return (ptr);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get DoorLockEMPilot instance processed request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated DoorLockEMPilot instance.
 *
 *  @return     void * processed - Processed instance data.
 *  @retval     Address of processed instance data. Cast it using *((uint8 *)ptr) to get values.
 *  @retval     NULL - Invalid load index.
 */
void * SRDoorLockEMPilot__GetProcessed(HBL_LOAD_TYPE * load )
{
    void *ptr = NULL;

    if (load->Pilot_Data_Index < SRDoorLockEMPilot_Allocation)
    {
        ptr = (void *) (&SRDoorLockEMPilot_Data[load->Pilot_Data_Index].Processed);
    }

    return (ptr);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get DoorLockEMPilot instance status.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to DoorLockEMPilot instance.
 *
 *  @return     void *status - instance Status.
 *  @retval     Address of instance Status. Cast it using *((HBL_LOAD_STATUS_TYPE *)ptr) to get the value.
 *  @retval     NULL - Invalid load index.
 */
void * SRDoorLockEMPilot__GetStatus(HBL_LOAD_TYPE * load )
{
    void *ptr = NULL;

    if (load->Pilot_Data_Index < SRDoorLockEMPilot_Allocation)
    {
        ptr = (void *) (&SRDoorLockEMPilot_Data[load->Pilot_Data_Index].PilotStatus);
    }

    return (ptr);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Asynchronous process for DoorLockEMPilot.
 *
 *	@detail     Asynchronous process handles status transition and consistency check for door lock status
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to DoorLockEMPilot instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of DoorLockEMPilot asynch execution.
 *  @retval     PASS - Execution succeeded.
 *  @retval     FAIL - Execution failed.
 */
PASS_FAIL_TYPE SRDoorLockEMPilot__AsynchProcess(HBL_LOAD_TYPE * load )
{
    SRDOORLOCKEMPILOT_PARAM_TYPE *param_ptr;
    SRDOORLOCKEMPILOT_DATA_TYPE *ptr;
    SRDOORLOCKEMPILOT_DATA_TYPE *n_ptr;
    HBL_LOAD_STATUS_TYPE tmp_load;
    PASS_FAIL_TYPE retval;
    BOOL_TYPE pin_val;
    uint8 index;
    HBL_CONSISTENCY_TYPE lock_feedback;
    HBL_LOAD_STATUS_TYPE target_status;

    retval = FAIL;
    pin_val = FALSE;
    index = load->Pilot_Data_Index;

    if (CheckItem(index))
    {
        param_ptr = (SRDOORLOCKEMPILOT_PARAM_TYPE *) (load->Parameter_Ptr.Data); //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
        ptr = &SRDoorLockEMPilot_Data[index];
        n_ptr = &N_SRDoorLockEMPilot_Data[index];

        target_status.Load.Cur_Reg = ptr->Processed;
        // Prevent use of TRN table when DoorLock status is OFF;
        // during INITIALIZE consider Processed target like it has been already reached
        if ((ptr->Processed != 0) && (ptr->PilotStatus.Load.Status != HBL_LOAD_STATUS_INITIALIZE))
        {
            target_status.Load.Status = ptr->PilotStatus.Load.Status;
        }
        else
        {
            target_status.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
        }
        lock_feedback.Byte = Hbl__GetOwnLoadConsistency(load, target_status.Byte);
        retval = PASS;

        switch (ptr->Status)
        {
            case SRDOORLOCKEM_STATUS_DOOR_LOCKING:
                if ((lock_feedback.Consistency.Value == HBL_CONSISTENCY_OK)
                && (lock_feedback.Consistency.Table != HBL_CONSISTENCY_TABLE_OFF))
                {
                    SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                    SRDOORLOCKEM_STATUS_DOOR_LOCKED);
                    SRData__UpdateByte(&ptr->Retries, &n_ptr->Retries, 0);
                }
                else if ((ptr->Retries == 0) &&
                    (ptr->Retries_Counter >= (param_ptr->Time_Spacing * SRDOORLOCKEM_TIME_SPACING_UNIT_MS)))
                {
                    SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                    SRDOORLOCKEM_STATUS_DOOR_LOCKING_FAILED);
                }
                break;

            case SRDOORLOCKEM_STATUS_DOOR_UNLOCKING:
                if ((   (lock_feedback.Consistency.Value == HBL_CONSISTENCY_OK) ||
                        (lock_feedback.Consistency.Value == HBL_CONSISTENCY_DISCONNECTED))
                && (lock_feedback.Consistency.Table != HBL_CONSISTENCY_TABLE_ON)
                && (lock_feedback.Consistency.Table != HBL_CONSISTENCY_TABLE_ON_ALTERNATE))
                {
                    SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                    SRDOORLOCKEM_STATUS_DOOR_UNLOCKED);
                    SRData__UpdateByte(&ptr->Retries, &n_ptr->Retries, 0);
                }
                else if ((ptr->Retries == 0) &&
                    (ptr->Retries_Counter >= (param_ptr->Time_Spacing * SRDOORLOCKEM_TIME_SPACING_UNIT_MS)))
                {
                    SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                    SRDOORLOCKEM_STATUS_DOOR_UNLOCKING_FAILED);
                }
                break;

            case SRDOORLOCKEM_STATUS_DOOR_LOCKED:
            case SRDOORLOCKEM_STATUS_DOOR_UNLOCKED:
            case SRDOORLOCKEM_STATUS_DOOR_LOCKING_FAILED:
            case SRDOORLOCKEM_STATUS_DOOR_UNLOCKING_FAILED:
                break;

            default:
                // Consistency detects door is locked/unlocked properly, according to acutal set point
                if ((ptr->Requested == ptr->Processed) &&
                    (   (lock_feedback.Consistency.Value == HBL_CONSISTENCY_OK) ||
                        (lock_feedback.Consistency.Value == HBL_CONSISTENCY_DISCONNECTED)) )
                {
                    uint16 duration = param_ptr->Num_Pulses;

                    if (duration == 0)
                    {
                        duration = SRDOORLOCKEM_DEFAULT_PULSES;
                    }
                    duration *= ptr->Period;

                    if (lock_feedback.Consistency.Table == HBL_CONSISTENCY_TABLE_OFF)
                    {
                        // Door already unlocked
                        SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                        SRDOORLOCKEM_STATUS_DOOR_UNLOCKED);
                        if(ptr->Retries_Counter < duration)
                        {
                            SRData__UpdateShort(&ptr->Retries_Counter, &n_ptr->Retries_Counter, duration);
                        }
                    }
                    else if (lock_feedback.Consistency.Table == HBL_CONSISTENCY_TABLE_ON)
                    {
                        // Door already locked
                        SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                        SRDOORLOCKEM_STATUS_DOOR_LOCKED);
                        if(ptr->Retries_Counter < duration)
                        {
                            SRData__UpdateShort(&ptr->Retries_Counter, &n_ptr->Retries_Counter, duration);
                        }
                    }
                }
                else
                {
                    SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, SRDOORLOCKEM_STATUS_INIT); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                    // Wait for Time between pulses
                    if (ptr->Retries_Counter >= (param_ptr->Time_Spacing * SRDOORLOCKEM_TIME_SPACING_UNIT_MS))
                    {
                        if (ptr->Processed == OFF)
                        {
                            // Door is assumed to be locked
                            SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                            SRDOORLOCKEM_STATUS_DOOR_LOCKED);
                            SRData__UpdateByte((uint8 *) &ptr->Processed, (uint8 *) &n_ptr->Processed, ON); //lint !e926 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                        }
                        else
                        {
                            // Door is assumed to be unlocked
                            SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                            SRDOORLOCKEM_STATUS_DOOR_UNLOCKED);
                            SRData__UpdateByte((uint8 *) &ptr->Processed, (uint8 *) &n_ptr->Processed, OFF); //lint !e926 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                        }
                    }
                }
                break;

        }

        switch (ptr->Status)
        {
            case SRDOORLOCKEM_STATUS_DOOR_LOCKED:
            case SRDOORLOCKEM_STATUS_DOOR_UNLOCKED:
                tmp_load.Load.Status = ((pin_val == TRUE) ? HBL_LOAD_STATUS_TARGET_ALT_TABLE : HBL_LOAD_STATUS_TARGET_COMPLETED);
                tmp_load.Load.Cur_Reg = ptr->Processed;
                SRData__UpdateByte(&ptr->PilotStatus.Byte, &n_ptr->PilotStatus.Byte, tmp_load.Byte);
                break;

            case SRDOORLOCKEM_STATUS_DOOR_LOCKING_FAILED:
            case SRDOORLOCKEM_STATUS_DOOR_UNLOCKING_FAILED:
                tmp_load.Load.Status = HBL_LOAD_STATUS_FAILURE;
                tmp_load.Load.Cur_Reg = ptr->Processed;
                SRData__UpdateByte(&ptr->PilotStatus.Byte, &n_ptr->PilotStatus.Byte, tmp_load.Byte);
                break;

            default:
                if (ptr->PilotStatus.Load.Status != HBL_LOAD_STATUS_INITIALIZE)
                {
                    tmp_load.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
                    tmp_load.Load.Cur_Reg = ptr->PilotStatus.Load.Cur_Reg;
                    SRData__UpdateByte(&ptr->PilotStatus.Byte, &n_ptr->PilotStatus.Byte, tmp_load.Byte);
                }
        }
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      One millisecond process for DoorLockEMPilot.
 *
 *	@detail     Low level feedback probe to detect frequency and pin activation in sync with feedback
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to DoorLockEMPilot instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of DoorLockEMPilot 1ms execution.
 *  @retval     PASS - Execution succeeded.
 *  @retval     FAIL - Execution failed.
 */
PASS_FAIL_TYPE SRDoorLockEMPilot__Handler1ms(HBL_LOAD_TYPE * load )
{
    SRDOORLOCKEMPILOT_PARAM_TYPE *param_ptr;
    SRDOORLOCKEMPILOT_DATA_TYPE *n_ptr;
    SRDOORLOCKEMPILOT_DATA_TYPE *ptr;
    PASS_FAIL_TYPE retval;
    BOOL_TYPE pin_val;
    BOOL_TYPE pin_fbk_val;
    uint8 virtual_pin;
    uint8 index;

    retval = FAIL;
    pin_val = FALSE;
    pin_fbk_val = FALSE;
    index = load->Pilot_Data_Index;

    if (CheckItem(index))
    {
        param_ptr = (SRDOORLOCKEMPILOT_PARAM_TYPE *) (load->Parameter_Ptr.Data); //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
        ptr = &SRDoorLockEMPilot_Data[index];
        n_ptr = &N_SRDoorLockEMPilot_Data[index];

        if (ptr->Fbk_Pin < VirtualPin__GetNumOf())
        {
            pin_fbk_val = Gpio__PinRead(VIRTUALPIN_TABLE[ptr->Fbk_Pin].port, VIRTUALPIN_TABLE[ptr->Fbk_Pin].pin);
        }

        if (pin_fbk_val != ptr->Fbk_Last_Pin_Value)
        {
            if (pin_fbk_val == TRUE)
            {
                if ((ptr->Fbk_Counter >= SRDOORLOCKEM_MIN_PERIOD_MS) &&
                    (ptr->Fbk_Counter <= SRDOORLOCKEM_MAX_PERIOD_MS))
                {
                    uint8 new_period = (ptr->Period + ptr->Fbk_Counter + 1) >> 1;
                    sint8 tmp_offset = (ptr->Fbk_Counter - (2 * ptr->Fbk_Last_Counter )) / 4;
                    if (tmp_offset < 0)
                    {
                        tmp_offset += (ptr->Period / 2);
                    }
                    SRData__UpdateByte(&ptr->Period, &n_ptr->Period, new_period);
                    SRData__UpdateByte((uint8 *) &ptr->Cycle_Counter, (uint8 *) &n_ptr->Cycle_Counter, (uint8) tmp_offset); //lint !e926 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                }
                SRData__UpdateByte(&ptr->Fbk_Counter, &n_ptr->Fbk_Counter, 0);
            }
            else
            {
                SRData__UpdateByte(&ptr->Fbk_Last_Counter, &n_ptr->Fbk_Last_Counter, ptr->Fbk_Counter);
            }

            SRData__UpdateByte(&ptr->Fbk_Last_Pin_Value, &n_ptr->Fbk_Last_Pin_Value, pin_fbk_val);
        }

        if(ptr->Cycle_Counter >= ptr->Period)
        {
            SRData__UpdateByte(&ptr->Cycle_Counter, &n_ptr->Cycle_Counter, 0);
        }

        if (ptr->Retries_Counter < (param_ptr->Time_Spacing * SRDOORLOCKEM_TIME_SPACING_UNIT_MS))
        {
            SRData__UpdateShort(&ptr->Retries_Counter, &n_ptr->Retries_Counter, ptr->Retries_Counter + 1);
        }

        if (ptr->Status != SRDOORLOCKEM_STATUS_INIT)
        {
            uint16 pulse_duration;
            BOOL_TYPE force_pin = (BOOL_TYPE) ptr->Keep_Triac_On;
            uint8 num_pulses = param_ptr->Num_Pulses;

            if (num_pulses == 0)
            {
                num_pulses = SRDOORLOCKEM_DEFAULT_PULSES;
            }

            if ((ptr->Cycle_Counter == 0) ||
                (ptr->Cycle_Counter == (ptr->Period/2)))
            {
                if ((ptr->Retries > 0) &&
                    (ptr->Retries_Counter >= (param_ptr->Time_Spacing * SRDOORLOCKEM_TIME_SPACING_UNIT_MS)))
                {
                    SRData__UpdateShort(&ptr->Retries_Counter, &n_ptr->Retries_Counter, 0);
                    SRData__UpdateByte(&ptr->Retries, &n_ptr->Retries, ptr->Retries-1);
                }
                else if (ptr->Process_Triac_On)
                {
                    force_pin = TRUE;
                }
            }

            if (!ptr->Process_Triac_On)
            {
                force_pin = FALSE;
            }
            SRData__UpdateByte(&ptr->Keep_Triac_On, &n_ptr->Keep_Triac_On, (uint8) force_pin);

            pulse_duration = num_pulses * ptr->Period;
            pulse_duration /= 2;
            // Pulse duration is reduced by 1/4 period to ensure triac can switch OFF
            pulse_duration -= (ptr->Period/4);

            if ((force_pin == TRUE) ||
                (ptr->Retries_Counter < pulse_duration))
            {
                pin_val = TRUE;
            }

        }

        virtual_pin = (*load->Pin_Ptr);

        if ((Gpio__CheckConfig(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin,
        VIRTUALPIN_TABLE[virtual_pin].pin_config, (BOOL_TYPE) ptr->Last_Pin_Out_Value)) == TRUE)
        {
            pin_val ^= (BOOL_TYPE) ~load->Hi_Low_Activation_Bitmap;
            pin_val &= TRUE;
            Gpio__PinWrite(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin, pin_val);
            SRData__UpdateByte(&ptr->Last_Pin_Out_Value, &n_ptr->Last_Pin_Out_Value, pin_val);
            retval = PASS;
        }
        else
        {
            SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, HBL_PILOT_IMPULSES_DOOR_LOCK, index);
        }

        SRData__UpdateByte(&ptr->Cycle_Counter, &n_ptr->Cycle_Counter, ptr->Cycle_Counter+1);
        if (ptr->Fbk_Counter < SRDOORLOCKEM_FBK_TIMEOUT)
        {
            SRData__UpdateByte(&ptr->Fbk_Counter, &n_ptr->Fbk_Counter, ptr->Fbk_Counter+1);
        }
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief	Handler for DoorLockEMPilot execution
 *
 * It checks SRDoorLockEMPilot_Allocation integrity and all SRDoorLockEMPilot_Data instances allocated
 * It sets SRTASKSEQUENCE_APPL_SRDOORLOCKEMPILOT_CHECKS step in SRTASKSEQUENCE_APPL_SAFETY_THREAD sequence
 *
 */
void SRDoorLockEMPilot__Handler(void )
{
    if (SRData__CheckByte(&SRDoorLockEMPilot_Allocation, &N_SRDoorLockEMPilot_Allocation) != SRDATA_OK)
    {
        SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, HBL_PILOT_IMPULSES_DOOR_LOCK, 0xFF);
    }
    else
    {
        uint8 index;

        for (index = 0; index < HBL_PILOT_NUM_SRDOORLOCKEM; index++)
        {
            if (SRData__CheckArray(&SRDoorLockEMPilot_Data[index], &N_SRDoorLockEMPilot_Data[index],
            sizeof(SRDOORLOCKEMPILOT_DATA_TYPE)) != SRDATA_OK)
            {
                SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, HBL_PILOT_IMPULSES_DOOR_LOCK, index);
                break;
            }
        }
    }

#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_SRDOORLOCKEMPILOT_CHECKS);
#endif

}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

/**
 * @brief   Function to perform data integrity of a SRDoorlockEM instance
 *
 * It checks SRDoorLockEMPilot_Allocation integrity and all SRDoorLockEMPilot_Data instances allocated
 * It sets SRTASKSEQUENCE_APPL_SRDOORLOCKEMPILOT_CHECKS step in SRTASKSEQUENCE_APPL_SAFETY_THREAD sequence
 *
 *  @param      uint8 index - Instance index
 *
 *  @return     BOOL_TYPE result - Result of DoorLockEMPilot instance data integrity check
 *  @retval     TRUE - Data integrity check succeded
 *  @retval     FAIL - Data integrity check failed
 *
 */
BOOL_TYPE CheckItem(uint8 index )
{
    BOOL_TYPE retval = FALSE;

    if (SRData__CheckByte(&SRDoorLockEMPilot_Allocation, &N_SRDoorLockEMPilot_Allocation) != SRDATA_OK)
    {
        SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, HBL_PILOT_IMPULSES_DOOR_LOCK, 0xFF);
    }
    else
    {
        if (index < SRDoorLockEMPilot_Allocation)
        {
            if (SRData__CheckArray(&SRDoorLockEMPilot_Data[index], &N_SRDoorLockEMPilot_Data[index],
            sizeof(SRDOORLOCKEMPILOT_DATA_TYPE)) != SRDATA_OK)
            {
                SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, HBL_PILOT_IMPULSES_DOOR_LOCK, index);
            }
            else
            {
                retval = TRUE;
            }
        }
    }

    return (retval);
}

#endif

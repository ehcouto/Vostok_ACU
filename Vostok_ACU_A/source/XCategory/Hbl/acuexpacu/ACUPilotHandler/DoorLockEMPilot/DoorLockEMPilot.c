/**
 *  @file       DoorLockEMPilot.c
 *
 *  @brief      Pilot Handler for EM DoorLock device
 *
 *  @details    This module performs locking and unlocking process of an electromagnetic Doorlock device.
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "DoorLockEMPilot.h" 

#if (HBL_PILOT_NUM_DOORLOCKEM > 0)
#include "Hbl.h"
#include "Gpio.h"
#include "VirtualPin.h"
#include "string.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#define DOORLOCKEM_DEFAULT_PULSES            (3)
#define DOORLOCKEM_INITIAL_PERIOD           (18)
#define DOORLOCKEM_TIME_SPACING_UNIT_MS     (100)
#define DOORLOCKEM_FBK_TIMEOUT              (DOORLOCKEM_TIME_SPACING_UNIT_MS)

#define ZC_ALLOWED_MIN_FREQUENCY            45
#define ZC_ALLOWED_MAX_FREQUENCY            65

// Minimum and maximum period from ZeroCross frequency boundaries set - Round to near
#define DOORLOCKEM_MIN_PERIOD_MS		((2000+ZC_ALLOWED_MAX_FREQUENCY)/(ZC_ALLOWED_MAX_FREQUENCY*2))
#define DOORLOCKEM_MAX_PERIOD_MS		((2000+ZC_ALLOWED_MIN_FREQUENCY)/(ZC_ALLOWED_MIN_FREQUENCY*2))

typedef enum DOORLOCKEM_STATUS_ENUM
{
    DOORLOCKEM_STATUS_INIT,
    DOORLOCKEM_STATUS_DOOR_UNLOCKED,
    DOORLOCKEM_STATUS_DOOR_LOCKING,
    DOORLOCKEM_STATUS_DOOR_LOCKING_FAILED,
    DOORLOCKEM_STATUS_DOOR_LOCKED,
    DOORLOCKEM_STATUS_DOOR_UNLOCKING,
    DOORLOCKEM_STATUS_DOOR_UNLOCKING_FAILED
} DOORLOCKEM_STATUS_TYPE;

PACKED typedef struct
{
uint16 Retries_Counter;
uint8 Fbk_Counter;
uint8 Fbk_Last_Counter;
uint8 Period;
uint8 Cycle_Counter;
uint8 Fbk_Last_Pin_Value;
uint8 Fbk_Pin;
uint8 Requested;
uint8 Processed;
HBL_LOAD_STATUS_TYPE PilotStatus;
DOORLOCKEM_STATUS_TYPE Status;
    uint8 Request_Triac_On;
    uint8 Process_Triac_On;
    uint8 Keep_Triac_On;
uint8 Retries;
} DOORLOCKEMPILOT_DATA_TYPE;

typedef struct
{
uint8 Max_Retries :4;
uint8 Num_Pulses :4;
uint8 Time_Spacing;
} DOORLOCKEMPILOT_PARAM_TYPE;

static uint8 DoorLockEMPilot_Allocation;
static DOORLOCKEMPILOT_DATA_TYPE DoorLockEMPilot_Data[HBL_PILOT_NUM_DOORLOCKEM];

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module DoorLockEMPilot and its variables
 *
 */
void DoorLockEMPilot__Initialize(void )
{
    DoorLockEMPilot_Allocation = 0;
    memset(DoorLockEMPilot_Data, 0, sizeof(DoorLockEMPilot_Data));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Allocate pilot instance for DoorlockEMPilot.
 *
 *  @return     uint8 result - Allocation result.
 *  @retval     0-254 - Pilot allocation instance.
 *  @retval     0xFF - Allocation failed.
 */
uint8 DoorLockEMPilot__Allocate(void )
{
    uint8 retval = 0xFF;

    if (DoorLockEMPilot_Allocation < HBL_PILOT_NUM_DOORLOCKEM)
    {
        retval = DoorLockEMPilot_Allocation;
        DoorLockEMPilot_Allocation++;
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
PASS_FAIL_TYPE DoorLockEMPilot__InitializeInstance(HBL_LOAD_TYPE * load )
{
    PASS_FAIL_TYPE retval = FAIL;
    uint8 index = load->Pilot_Data_Index;
    DOORLOCKEMPILOT_PARAM_TYPE *param_ptr = (DOORLOCKEMPILOT_PARAM_TYPE *) (load->Parameter_Ptr.Data); //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    DOORLOCKEMPILOT_DATA_TYPE *ptr;
    uint8 *ptr_pin_table;
    uint8 fbk_index;
    uint8 virtual_pin;

    if ((index < DoorLockEMPilot_Allocation) && (param_ptr != NULL ))
    {
        ptr = &DoorLockEMPilot_Data[index];

        ptr->Status = DOORLOCKEM_STATUS_INIT;
        ptr->Period = DOORLOCKEM_INITIAL_PERIOD;
        ptr->PilotStatus.Load.Status = HBL_LOAD_STATUS_INITIALIZE;
        ptr->PilotStatus.Load.Cur_Reg = 0;
        ptr->Retries_Counter = 0;

        // Check if a Feedback/SRFeedback LLI is configured for pin synchronization
        fbk_index = Hbl__GetLLIIndexByOwnLoadConsistencyType(load, HBL_LLI_FEEDBACK);
        if (fbk_index >= Hbl__GetNumLLI())
        {
            fbk_index = Hbl__GetLLIIndexByOwnLoadConsistencyType(load, HBL_LLI_SRFEEDBACK);
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
        ptr->Fbk_Pin = virtual_pin;

        virtual_pin = load->Pin_Ptr[0];
        if ((virtual_pin < VirtualPin__GetNumOf()) && (VIRTUALPIN_TABLE[virtual_pin].port != 0xFF))
        {
            uint8 pin_val = ~load->Hi_Low_Activation_Bitmap;
            pin_val &= 0x01;
            Gpio__PinWrite(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin, (BOOL_TYPE) pin_val);
            Gpio__PinConfig(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin,
            VIRTUALPIN_TABLE[virtual_pin].pin_config);
            retval = PASS;
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
PASS_FAIL_TYPE DoorLockEMPilot__Request(HBL_LOAD_TYPE * load ,void * data )
{
    PASS_FAIL_TYPE retval = FAIL;
    uint8 index = load->Pilot_Data_Index;

    if (index < DoorLockEMPilot_Allocation)
    {
        uint8 new_target = (*(uint8 *)data);
        uint8 new_triac_on = (new_target & DOORLOCKEM_PROCESSED_KEEP_PIN_MASK) ? TRUE : FALSE;
        new_target &= ~DOORLOCKEM_PROCESSED_KEEP_PIN_MASK;

        DoorLockEMPilot_Data[index].Requested = new_target;
        DoorLockEMPilot_Data[index].Request_Triac_On = new_triac_on;
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
PASS_FAIL_TYPE DoorLockEMPilot__Process(HBL_LOAD_TYPE * load )
{
    PASS_FAIL_TYPE retval = FAIL;
    uint8 index = load->Pilot_Data_Index;

    if (index < DoorLockEMPilot_Allocation)
    {
        DOORLOCKEMPILOT_PARAM_TYPE *param_ptr;
        DOORLOCKEMPILOT_DATA_TYPE *ptr;

        param_ptr = (DOORLOCKEMPILOT_PARAM_TYPE *) (load->Parameter_Ptr.Data); //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
        ptr = &DoorLockEMPilot_Data[index];

		// Prevent any state update if Time_Spacing time has not been elapsed
        if ((ptr->Retries_Counter >= (param_ptr->Time_Spacing * DOORLOCKEM_TIME_SPACING_UNIT_MS))
            && (ptr->Requested != ptr->Processed))
        {
            HBL_LOAD_STATUS_TYPE tmp_load;
            HBL_CONSISTENCY_TYPE requested_consistency;

            tmp_load.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
            tmp_load.Load.Cur_Reg = ptr->Requested;
            requested_consistency.Byte = Hbl__GetOwnLoadConsistency(load, tmp_load.Byte);

            if (ptr->Requested == 0)
            {
                switch (ptr->Status)
                {
                    case DOORLOCKEM_STATUS_DOOR_LOCKED:
                        if (requested_consistency.Consistency.Value != HBL_CONSISTENCY_OK)
                        {
                            ptr->PilotStatus.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
                            ptr->Status = DOORLOCKEM_STATUS_DOOR_UNLOCKING;
                            ptr->Retries = param_ptr->Max_Retries;
                        }
                        else
                        {
                            // If current consistency related to Requested value already match, go directly to
                            // new status - this prevents to change door lock status
                            ptr->Status = DOORLOCKEM_STATUS_DOOR_UNLOCKED;
                        }
                        retval = PASS;
                        break;

                    case DOORLOCKEM_STATUS_DOOR_LOCKING_FAILED:
                        ptr->PilotStatus.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
                        ptr->Status = DOORLOCKEM_STATUS_INIT;
                        retval = PASS;
                        break;

                    default:
                        break;
                }
            }
            else
            {
                switch (DoorLockEMPilot_Data[index].Status)
                {
                    case DOORLOCKEM_STATUS_DOOR_UNLOCKED:
                        if (requested_consistency.Consistency.Value != HBL_CONSISTENCY_OK)
                        {
                            ptr->PilotStatus.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
                            ptr->Status = DOORLOCKEM_STATUS_DOOR_LOCKING;
                            ptr->Retries = param_ptr->Max_Retries;
                        }
                        else
                        {
                            // If current consistency related to Requested value already match, go directly to
                            // new status - this prevents to change door lock status
                            ptr->Status = DOORLOCKEM_STATUS_DOOR_LOCKED;
                        }
                        retval = PASS;
                        break;

                    case DOORLOCKEM_STATUS_DOOR_UNLOCKING_FAILED:
                        ptr->PilotStatus.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
                        ptr->Status = DOORLOCKEM_STATUS_INIT;
                        retval = PASS;
                        break;

                    default:
                        break;
                }
            }
        }
        else if ((ptr->Status == DOORLOCKEM_STATUS_INIT) ||
            (ptr->Requested  == ptr->Processed))
        {
            retval = PASS;
        }

         // Update Processed data only when status has been updated
        if (retval == PASS)
        {
           ptr->Processed = ptr->Requested;
        }
        // Check Request_Triac_On and Process_Triac_On changes
        if (ptr->Request_Triac_On != ptr->Process_Triac_On)
        {
            /* If Process_Triac_On was set, pin has been forced ON and this could alter Door lock device status regardless of
             * Status instance field value; in order to align it properly and initialize will be performed.
             */
            if(ptr->Process_Triac_On)
            {
                ptr->PilotStatus.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
                ptr->Status = DOORLOCKEM_STATUS_INIT;
            }
            ptr->Process_Triac_On = ptr->Request_Triac_On;
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
void * DoorLockEMPilot__GetRequested(HBL_LOAD_TYPE * load )
{
    void *ptr = NULL;

    if (load->Pilot_Data_Index < DoorLockEMPilot_Allocation)
    {
        ptr = (void *) (&DoorLockEMPilot_Data[load->Pilot_Data_Index].Requested);
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
void * DoorLockEMPilot__GetProcessed(HBL_LOAD_TYPE * load )
{
    void *ptr = NULL;

    if (load->Pilot_Data_Index < DoorLockEMPilot_Allocation)
    {
        ptr = (void *) (&DoorLockEMPilot_Data[load->Pilot_Data_Index].Processed);
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
 *  @retval     Address of instance Status. Cast it using *((uint8 *)ptr) to get the value.
 *  @retval     NULL - Invalid load index.
 */
void * DoorLockEMPilot__GetStatus(HBL_LOAD_TYPE * load )
{
    void *ptr = NULL;

    if (load->Pilot_Data_Index < DoorLockEMPilot_Allocation)
    {
        ptr = (void *) (&DoorLockEMPilot_Data[load->Pilot_Data_Index].PilotStatus);
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
PASS_FAIL_TYPE DoorLockEMPilot__AsynchProcess(HBL_LOAD_TYPE * load )
{
    DOORLOCKEMPILOT_PARAM_TYPE *param_ptr;
    DOORLOCKEMPILOT_DATA_TYPE *ptr;
    PASS_FAIL_TYPE retval;
    BOOL_TYPE pin_val;
    uint8 index;
    HBL_CONSISTENCY_TYPE lock_feedback;
    HBL_LOAD_STATUS_TYPE target_status;

    retval = FAIL;
    pin_val = FALSE;
    index = load->Pilot_Data_Index;

    if (index < DoorLockEMPilot_Allocation)
    {
        param_ptr = (DOORLOCKEMPILOT_PARAM_TYPE *) (load->Parameter_Ptr.Data); //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
        ptr = &DoorLockEMPilot_Data[index];

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
            case DOORLOCKEM_STATUS_DOOR_LOCKING:
                if ((lock_feedback.Consistency.Value == HBL_CONSISTENCY_OK)
                && (lock_feedback.Consistency.Table != HBL_CONSISTENCY_TABLE_OFF))
                {
                    ptr->Status = DOORLOCKEM_STATUS_DOOR_LOCKED;
                    ptr->Retries = 0;
                }
                else if ((ptr->Retries == 0) &&
				    (ptr->Retries_Counter >= (param_ptr->Time_Spacing * DOORLOCKEM_TIME_SPACING_UNIT_MS)))
                {
                    ptr->Status = DOORLOCKEM_STATUS_DOOR_LOCKING_FAILED;
                }
                break;

            case DOORLOCKEM_STATUS_DOOR_UNLOCKING:
                if ((   (lock_feedback.Consistency.Value == HBL_CONSISTENCY_OK) ||
                        (lock_feedback.Consistency.Value == HBL_CONSISTENCY_DISCONNECTED))
                && (lock_feedback.Consistency.Table != HBL_CONSISTENCY_TABLE_ON)
                && (lock_feedback.Consistency.Table != HBL_CONSISTENCY_TABLE_ON_ALTERNATE))
                {
                    ptr->Status = DOORLOCKEM_STATUS_DOOR_UNLOCKED;
                    ptr->Retries = 0;
                }
                else if ((ptr->Retries == 0) &&
				    (ptr->Retries_Counter >= (param_ptr->Time_Spacing * DOORLOCKEM_TIME_SPACING_UNIT_MS)))
                {
                    ptr->Status = DOORLOCKEM_STATUS_DOOR_UNLOCKING_FAILED;
                }
                break;

            case DOORLOCKEM_STATUS_DOOR_LOCKED:
            case DOORLOCKEM_STATUS_DOOR_UNLOCKED:
            case DOORLOCKEM_STATUS_DOOR_LOCKING_FAILED:
            case DOORLOCKEM_STATUS_DOOR_UNLOCKING_FAILED:
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
                        ptr->Status = DOORLOCKEM_STATUS_DOOR_UNLOCKED;
                        if(ptr->Retries_Counter < duration)
                        {
                            ptr->Retries_Counter = duration;
                        }
                    else
                    {
                        // Door already locked
                        ptr->Status = DOORLOCKEM_STATUS_DOOR_LOCKED;
                        if(ptr->Retries_Counter < duration)
                        {
                            ptr->Retries_Counter = duration;
                        }
                    }
                }
                else
                {
                    ptr->Status = DOORLOCKEM_STATUS_INIT;
                    // Wait for Time between pulses
                    if (ptr->Retries_Counter >= (param_ptr->Time_Spacing * DOORLOCKEM_TIME_SPACING_UNIT_MS))
                    {
                        if (ptr->Processed == OFF)
                        {
                            // Door is assumed to be locked
                            ptr->Status = DOORLOCKEM_STATUS_DOOR_LOCKED;
                            ptr->Processed = ON;
                        }
                        else
                        {
                            // Door is assumed to be unlocked
                            ptr->Status = DOORLOCKEM_STATUS_DOOR_UNLOCKED;
                            ptr->Processed = OFF;
                        }
                    }
                }
                break;

        }

        switch (ptr->Status)
        {
            case DOORLOCKEM_STATUS_DOOR_LOCKED:
            case DOORLOCKEM_STATUS_DOOR_UNLOCKED:
                ptr->PilotStatus.Load.Status = ((pin_val == TRUE) ? HBL_LOAD_STATUS_TARGET_ALT_TABLE : HBL_LOAD_STATUS_TARGET_COMPLETED);
                ptr->PilotStatus.Load.Cur_Reg = ptr->Processed;
                break;

            case DOORLOCKEM_STATUS_DOOR_LOCKING_FAILED:
            case DOORLOCKEM_STATUS_DOOR_UNLOCKING_FAILED:
                ptr->PilotStatus.Load.Status = HBL_LOAD_STATUS_FAILURE;
                ptr->PilotStatus.Load.Cur_Reg = ptr->Processed;
                break;

            default:
                if (ptr->PilotStatus.Load.Status != HBL_LOAD_STATUS_INITIALIZE)
                {
                    ptr->PilotStatus.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
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
PASS_FAIL_TYPE DoorLockEMPilot__Handler1ms(HBL_LOAD_TYPE * load )
{
    DOORLOCKEMPILOT_PARAM_TYPE *param_ptr;
    DOORLOCKEMPILOT_DATA_TYPE *ptr;
    PASS_FAIL_TYPE retval;
    BOOL_TYPE pin_val;
    BOOL_TYPE pin_fbk_val;
    uint8 virtual_pin;
    uint8 index;

    retval = FAIL;
    pin_val = FALSE;
    pin_fbk_val = FALSE;
    index = load->Pilot_Data_Index;

    if (index < DoorLockEMPilot_Allocation)
    {
        param_ptr = (DOORLOCKEMPILOT_PARAM_TYPE *) (load->Parameter_Ptr.Data); //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
        ptr = &DoorLockEMPilot_Data[index];
        retval = PASS;

        if (ptr->Fbk_Pin < VirtualPin__GetNumOf())
        {
            pin_fbk_val = Gpio__PinRead(VIRTUALPIN_TABLE[ptr->Fbk_Pin].port, VIRTUALPIN_TABLE[ptr->Fbk_Pin].pin);
        }

        if (pin_fbk_val != ptr->Fbk_Last_Pin_Value)
        {
            if (pin_fbk_val == TRUE)
            {
                if ((ptr->Fbk_Counter >= DOORLOCKEM_MIN_PERIOD_MS) && 
				    (ptr->Fbk_Counter <= DOORLOCKEM_MAX_PERIOD_MS))
                {
                    uint16 new_period = (ptr->Period + ptr->Fbk_Counter + 1) >> 1;
                    sint8 tmp_offset = (ptr->Fbk_Counter - (2 * ptr->Fbk_Last_Counter)) / 4;
                    if (tmp_offset < 0)
                    {
                        tmp_offset += (ptr->Period / 2);
                    }
                    ptr->Period = new_period;
                    ptr->Cycle_Counter = tmp_offset;
                }
                ptr->Fbk_Counter = 0;
            }
            else
            {
                ptr->Fbk_Last_Counter = ptr->Fbk_Counter;
            }

            ptr->Fbk_Last_Pin_Value = pin_fbk_val;
        }
        if (ptr->Cycle_Counter >= ptr->Period)
        {
            ptr->Cycle_Counter = 0;
        }
        if (ptr->Retries_Counter < (param_ptr->Time_Spacing * DOORLOCKEM_TIME_SPACING_UNIT_MS))
        {
            ptr->Retries_Counter += 1;
        }

        if (ptr->Status != DOORLOCKEM_STATUS_INIT)
        {
            uint16 pulse_duration;
            BOOL_TYPE force_pin = (BOOL_TYPE) ptr->Keep_Triac_On;
            uint8 num_pulses = param_ptr->Num_Pulses;
            if (num_pulses == 0)
            {
                num_pulses = DOORLOCKEM_DEFAULT_PULSES;
            }

            if ((ptr->Cycle_Counter == 0) ||
                (ptr->Cycle_Counter == (ptr->Period/2)))
            {
                if ((ptr->Retries > 0) &&
                    (ptr->Retries_Counter >= (param_ptr->Time_Spacing * DOORLOCKEM_TIME_SPACING_UNIT_MS)))
                {
                    ptr->Retries_Counter = 0;
                    ptr->Retries--;
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
            ptr->Keep_Triac_On = force_pin;

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
        pin_val ^= (BOOL_TYPE) ~load->Hi_Low_Activation_Bitmap;
        pin_val &= (BOOL_TYPE) 0x01;
        Gpio__PinWrite(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin, pin_val);
		
        ptr->Cycle_Counter++;
        if (ptr->Fbk_Counter < DOORLOCKEM_FBK_TIMEOUT)
        {
		    ptr->Fbk_Counter++;
  		}
    }

    return (retval);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#endif

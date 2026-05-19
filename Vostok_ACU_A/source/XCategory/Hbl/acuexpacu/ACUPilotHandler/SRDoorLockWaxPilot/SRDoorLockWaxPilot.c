/**
 *  @file       SRDoorLockWaxPilot.c
 *
 *  @brief      Pilot Handler for Wax DoorLock device with Class B requirements
 *
 *  @details    This module performs locking and unlocking process of a Wax Doorlock device.
 *
 *	@defgroup 	CLASS_B
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
#include "C_Extensions.h"
#include "SRSystemConfig.h"
#include "SRDoorLockWaxPilot.h"

#if (HBL_PILOT_NUM_SRDOORLOCKWAX > 0)
#include "Gpio.h"
#include "VirtualPin.h"
#include "Hbl.h"
#include "string.h"
#include "SRData.h"
#include "SRTaskSequence.h"
#include "SRException.h"
#include "Micro.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define SRDOORLOCKWAX_INITIAL_PERIOD        20
#define SRDOORLOCKWAX_TIMING_REF			10
#define SRDOORLOCKWAX_MAX_DEBOUNCE			2

#define ZC_ALLOWED_MIN_FREQUENCY            45
#define ZC_ALLOWED_MAX_FREQUENCY            65

// Minimum and maximum period from ZeroCross frequency boundaries set - Round to near
#define SRDOORLOCKWAX_MIN_PERIOD_MS		((2000+ZC_ALLOWED_MAX_FREQUENCY)/(ZC_ALLOWED_MAX_FREQUENCY*2))
#define SRDOORLOCKWAX_MAX_PERIOD_MS		((2000+ZC_ALLOWED_MIN_FREQUENCY)/(ZC_ALLOWED_MIN_FREQUENCY*2))


typedef enum SRDOORLOCKWAX_STATUS_ENUM
{
	SRDOORLOCKWAX_STATUS_INIT,
	SRDOORLOCKWAX_STATUS_DOOR_UNLOCKED,
	SRDOORLOCKWAX_STATUS_DOOR_LOCKING,
	SRDOORLOCKWAX_STATUS_DOOR_LOCKING_FAILED,
	SRDOORLOCKWAX_STATUS_DOOR_LOCKED,
	SRDOORLOCKWAX_STATUS_DOOR_UNLOCKING,
	SRDOORLOCKWAX_STATUS_DOOR_UNLOCKING_FAILED
} SRDOORLOCKWAX_STATUS_TYPE;

PACKED typedef struct
{
	uint8 Requested;
	uint8 Processed;
	HBL_LOAD_STATUS_TYPE PilotStatus;
	SRDOORLOCKWAX_STATUS_TYPE Status;
	uint16 Counter;
	uint16 Fbk_Timeout_Counter;
	uint8 Fbk_Counter;
	uint8 Fbk_Last_Counter;
	uint8 Fbk_Period;
	sint8 Fbk_Offset;
	uint8 Fbk_Last_Pin_Value;
	uint8 Last_Pin_Out_Value;
	uint8 Wait_Trigger;
	uint8 Debounce;
	uint8 TimingRef;
	uint8 Fbk_Pin;
} SRDOORLOCKWAXPILOT_DATA_TYPE;

typedef struct
{
	uint16 Unlock_Timeout;
	uint16 Lock_Timeout;
	uint8 Duty_On;
	uint8 Duty_Off;
	uint8 Filter_Time;
	uint8 Unused;
} SRDOORLOCKWAXPILOT_PARAM_TYPE;

static uint8 SRDoorLockWaxPilot_Allocation;
static SRDOORLOCKWAXPILOT_DATA_TYPE SRDoorLockWaxPilot_Data[HBL_PILOT_NUM_SRDOORLOCKWAX];

static uint8 N_SRDoorLockWaxPilot_Allocation;
static SRDOORLOCKWAXPILOT_DATA_TYPE N_SRDoorLockWaxPilot_Data[HBL_PILOT_NUM_SRDOORLOCKWAX];

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static BOOL_TYPE CheckItem(uint8 index);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module DoorLockWaxPilot and its variables
 *
 */
void SRDoorLockWaxPilot__Initialize(void)
{
	SRData__UpdateByte(&SRDoorLockWaxPilot_Allocation, &N_SRDoorLockWaxPilot_Allocation, 0);
	memset(&SRDoorLockWaxPilot_Data, 0, sizeof(SRDoorLockWaxPilot_Data));
	memset(&N_SRDoorLockWaxPilot_Data, 0xFF, sizeof(N_SRDoorLockWaxPilot_Data));
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_INIT, SRTASKSEQUENCE_APPL_INIT_SRDOORLOCKWAXPILOT);
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Allocate pilot instance for DoorlockWaxPilot.
 *
 *  @return     uint8 result - Allocation result.
 *  @retval     0-254 - Pilot allocation instance.
 *  @retval     0xFF - Allocation failed.
 */
uint8 SRDoorLockWaxPilot__Allocate(void)
{
    uint8 retval = 0xFF;

    if (SRData__CheckByte(&SRDoorLockWaxPilot_Allocation, &N_SRDoorLockWaxPilot_Allocation) == SRDATA_OK)
    {
        if (SRDoorLockWaxPilot_Allocation < HBL_PILOT_NUM_SRDOORLOCKWAX)
        {
            retval = SRDoorLockWaxPilot_Allocation;
        	SRData__UpdateByte(&SRDoorLockWaxPilot_Allocation, &N_SRDoorLockWaxPilot_Allocation, SRDoorLockWaxPilot_Allocation+1);
        }
    }
    else
    {
    	SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, HBL_PILOT_WAX_DOOR_LOCK, 0xFF );
    }
    return(retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Initialize DoorLockWaxPilot instance.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated digital output load instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of DoorLockWaxPilot instance initialization.
 *  @retval     PASS - Instance initialization succeeded.
 *  @retval     FAIL - Instance initialization failed.
 */
PASS_FAIL_TYPE SRDoorLockWaxPilot__InitializeInstance(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval = FAIL;
    uint8 index = load->Pilot_Data_Index;
    SRDOORLOCKWAXPILOT_PARAM_TYPE *param_ptr;
    SRDOORLOCKWAXPILOT_DATA_TYPE *ptr;
    SRDOORLOCKWAXPILOT_DATA_TYPE *n_ptr;
	HBL_LOAD_STATUS_TYPE tmp_load;
    BOOL_TYPE pin_val;
    uint8 *ptr_pin_table;
    uint8 fbk_index;
    uint8 virtual_pin;


    if (CheckItem(index))
    {
    	ptr = (SRDOORLOCKWAXPILOT_DATA_TYPE *) &SRDoorLockWaxPilot_Data[index];  //lint !e929 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    	n_ptr = (SRDOORLOCKWAXPILOT_DATA_TYPE *) &N_SRDoorLockWaxPilot_Data[index];  //lint !e929 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    	param_ptr = (SRDOORLOCKWAXPILOT_PARAM_TYPE *) load->Parameter_Ptr.Data; //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]

    	SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, (uint8)SRDOORLOCKWAX_STATUS_INIT); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    	tmp_load.Load.Status = HBL_LOAD_STATUS_INITIALIZE;
    	tmp_load.Load.Cur_Reg = 0;
    	SRData__UpdateByte(&ptr->PilotStatus.Byte, &n_ptr->PilotStatus.Byte, tmp_load.Byte);
    	SRData__UpdateShort(&ptr->Counter, &n_ptr->Counter, param_ptr->Filter_Time);
    	SRData__UpdateShort(&ptr->Fbk_Timeout_Counter, &n_ptr->Fbk_Timeout_Counter, param_ptr->Filter_Time * SRDOORLOCKWAX_TIMING_REF);
    	SRData__UpdateByte(&ptr->Fbk_Period, &n_ptr->Fbk_Period, SRDOORLOCKWAX_INITIAL_PERIOD);
    	SRData__UpdateByte(&ptr->Debounce, &n_ptr->Debounce, SRDOORLOCKWAX_MAX_DEBOUNCE);

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
		if ((virtual_pin < VirtualPin__GetNumOf()) &&
			(VIRTUALPIN_TABLE[virtual_pin].port != 0xFF))
		{
			pin_val = (BOOL_TYPE) ~load->Hi_Low_Activation_Bitmap;
			pin_val &= TRUE;
			Gpio__PinWrite(VIRTUALPIN_TABLE[virtual_pin].port,VIRTUALPIN_TABLE[virtual_pin].pin, pin_val);
			Gpio__PinConfig(VIRTUALPIN_TABLE[virtual_pin].port,VIRTUALPIN_TABLE[virtual_pin].pin,VIRTUALPIN_TABLE[virtual_pin].pin_config);
	    	SRData__UpdateByte(&ptr->Last_Pin_Out_Value, &n_ptr->Last_Pin_Out_Value, pin_val);

	    	if(Gpio__CheckConfig(VIRTUALPIN_TABLE[virtual_pin].port,VIRTUALPIN_TABLE[virtual_pin].pin,VIRTUALPIN_TABLE[virtual_pin].pin_config, pin_val)== TRUE)
	    	{
	    		retval = PASS;
	    	}
		}
    }
    return(retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Set DoorLockWaxPilot instance request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated DoorLockWaxPilot instance.
 *
 *  @param      void *data - Pointer to request data. Pass argument as uint8 *.
 *
 *  @return     PASS_FAIL_TYPE result - Result of DoorLockWaxPilot request.
 *  @retval     PASS - Instance request succeeded.
 *  @retval     FAIL - Instance request failed.
 */
PASS_FAIL_TYPE SRDoorLockWaxPilot__Request(HBL_LOAD_TYPE * load,void * data)
{
	PASS_FAIL_TYPE retval = FAIL;
	uint8 index = load->Pilot_Data_Index;

    if (CheckItem(index))
    {
    	SRData__UpdateByte(&SRDoorLockWaxPilot_Data[index].Requested, &N_SRDoorLockWaxPilot_Data[index].Requested, *((uint8 *) data));
    	retval = PASS;
    }

	return(retval);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Process DoorLockWaxPilot instance.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to DoorLockWaxPilot instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of DoorLockWaxPilot instance process.
 *  @retval     PASS - Instance process succeeded.
 *  @retval     FAIL - Instance process failed.
 */
PASS_FAIL_TYPE SRDoorLockWaxPilot__Process(HBL_LOAD_TYPE * load)
{
    SRDOORLOCKWAXPILOT_PARAM_TYPE *param_ptr;
	SRDOORLOCKWAXPILOT_DATA_TYPE *ptr;
	SRDOORLOCKWAXPILOT_DATA_TYPE *n_ptr;
	HBL_LOAD_STATUS_TYPE tmp_load;
	PASS_FAIL_TYPE retval = FAIL;
	uint8 index = load->Pilot_Data_Index;

    if (CheckItem(index))
    {
    	ptr = &SRDoorLockWaxPilot_Data[index];
    	n_ptr = &N_SRDoorLockWaxPilot_Data[index];

    	if (ptr->Requested != ptr->Processed)
    	{
        	param_ptr = (SRDOORLOCKWAXPILOT_PARAM_TYPE *) load->Parameter_Ptr.Data; //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]

    		if (ptr->Requested == 0)
    		{
    			switch (ptr->Status)
    			{
    			case SRDOORLOCKWAX_STATUS_DOOR_LOCKED:
    			case SRDOORLOCKWAX_STATUS_DOOR_LOCKING:
    	    		tmp_load.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
    	    		tmp_load.Load.Cur_Reg = ptr->Processed;
    	    		SRData__UpdateByte(&ptr->PilotStatus.Byte, &n_ptr->PilotStatus.Byte, tmp_load.Byte);
    		    	SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, (uint8)SRDOORLOCKWAX_STATUS_DOOR_UNLOCKING); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    				SRData__UpdateShort(&ptr->Counter, &n_ptr->Counter, ENDIANS16_STON(param_ptr->Unlock_Timeout));
    		    	SRData__UpdateByte(&ptr->Wait_Trigger, &n_ptr->Wait_Trigger, 1);
    				break;

    			default:
    		    	SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, (uint8)SRDOORLOCKWAX_STATUS_INIT); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    				SRData__UpdateShort(&ptr->Counter, &n_ptr->Counter, param_ptr->Filter_Time);
    				break;
    			}
    		}
    		else
			{
				switch (SRDoorLockWaxPilot_Data[index].Status)
				{
				case SRDOORLOCKWAX_STATUS_DOOR_UNLOCKED:
				case SRDOORLOCKWAX_STATUS_DOOR_UNLOCKING:
    	    		tmp_load.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
    	    		tmp_load.Load.Cur_Reg = ptr->Processed;
    	    		SRData__UpdateByte(&ptr->PilotStatus.Byte, &n_ptr->PilotStatus.Byte, tmp_load.Byte);
    		    	SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, (uint8)SRDOORLOCKWAX_STATUS_DOOR_LOCKING); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    				SRData__UpdateShort(&ptr->Counter, &n_ptr->Counter, ENDIANS16_STON(param_ptr->Unlock_Timeout));
    		    	SRData__UpdateByte(&ptr->Wait_Trigger, &n_ptr->Wait_Trigger, 1);
					break;

				default:
    		    	SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, (uint8)SRDOORLOCKWAX_STATUS_INIT); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    				SRData__UpdateShort(&ptr->Counter, &n_ptr->Counter, param_ptr->Filter_Time);
					break;
				}
			}
    	}

    	SRData__UpdateByte(&ptr->Processed, &n_ptr->Processed, ptr->Requested);
    }

	return(retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get current DoorLockWaxPilot instance request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated DoorLockWaxPilot instance.
 *
 *  @return     void *requested - Requested instance data.
 *  @retval     Address of requested instance data. Cast it using *((uint8 *)ptr) to get values.
 *  @retval     NULL - Invalid load index.
 */
void * SRDoorLockWaxPilot__GetRequested(HBL_LOAD_TYPE * load)
{
	void *ptr = NULL;

    if (load->Pilot_Data_Index < SRDoorLockWaxPilot_Allocation)
    {
        ptr = (void *) (&SRDoorLockWaxPilot_Data[load->Pilot_Data_Index].Requested);
    }

    return(ptr);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get DoorLockWaxPilot instance processed request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated DoorLockWaxPilot instance.
 *
 *  @return     void * processed - Processed instance data.
 *  @retval     Address of processed instance data. Cast it using *((uint8 *)ptr) to get values.
 *  @retval     NULL - Invalid load index.
 */
void * SRDoorLockWaxPilot__GetProcessed(HBL_LOAD_TYPE * load)
{
	void *ptr;

	ptr = NULL;

    if (load->Pilot_Data_Index < SRDoorLockWaxPilot_Allocation)
    {
        ptr = (void *) (&SRDoorLockWaxPilot_Data[load->Pilot_Data_Index].Processed);
    }

    return ptr;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get DoorLockWaxPilot instance status.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to DoorLockWaxPilot instance.
 *
 *  @return     void *status - instance Status.
 *  @retval     Address of instance Status. Cast it using *((uint8 *)ptr) to get the value.
 *  @retval     NULL - Invalid load index.
 */
void * SRDoorLockWaxPilot__GetStatus(HBL_LOAD_TYPE * load)
{
	void *ptr = NULL;

    if (load->Pilot_Data_Index < SRDoorLockWaxPilot_Allocation)
    {
        ptr = (void *) (&SRDoorLockWaxPilot_Data[load->Pilot_Data_Index].PilotStatus);
    }

    return(ptr);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Asynchronous process for DoorLockWaxPilot.
 *
 *	@detail     Asynchronous process handles status transition and consistency check for door lock status
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to DoorLockWaxPilot instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of DoorLockWaxPilot asynch execution.
 *  @retval     PASS - Execution succeeded.
 *  @retval     FAIL - Execution failed.
 */
PASS_FAIL_TYPE SRDoorLockWaxPilot__AsynchProcess(HBL_LOAD_TYPE * load)
{
	SRDOORLOCKWAXPILOT_PARAM_TYPE *param_ptr;
	SRDOORLOCKWAXPILOT_DATA_TYPE *ptr;
	SRDOORLOCKWAXPILOT_DATA_TYPE *n_ptr;
	HBL_CONSISTENCY_TYPE lock_feedback;
	HBL_LOAD_STATUS_TYPE tmp_load;
	PASS_FAIL_TYPE retval = FAIL;
	BOOL_TYPE pin_val = FALSE;
	uint8 index = load->Pilot_Data_Index;

    if (CheckItem(index))
    {
    	ptr = &SRDoorLockWaxPilot_Data[index];
    	n_ptr = &N_SRDoorLockWaxPilot_Data[index];
    	param_ptr = (SRDOORLOCKWAXPILOT_PARAM_TYPE *) load->Parameter_Ptr.Data; //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    	tmp_load.Load.Cur_Reg = ptr->Processed;
    	// Prevent use of TRN table when DoorLock status is OFF;
    	// during INITIALIZE consider Processed target like it has been already reached
    	if ( (ptr->Processed != 0) &&
    		(ptr->PilotStatus.Load.Status != HBL_LOAD_STATUS_INITIALIZE))
    	{
    		tmp_load.Load.Status = ptr->PilotStatus.Load.Status;
    	}
    	else
    	{
    		tmp_load.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
    	}
    	lock_feedback.Byte = Hbl__GetOwnLoadConsistency(load, tmp_load.Byte);
    	retval = PASS;

    	switch(ptr->Status)
    	{
    	case SRDOORLOCKWAX_STATUS_DOOR_LOCKING:
    		pin_val = TRUE;
    		if (ptr->Counter == 0)
    		{
		    	SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, (uint8)SRDOORLOCKWAX_STATUS_DOOR_LOCKING_FAILED); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    		}
    		else
    		{
        		if ( (lock_feedback.Consistency.Value == HBL_CONSISTENCY_OK) &&
        				(lock_feedback.Consistency.Table != HBL_CONSISTENCY_TABLE_OFF))
        		{
    		    	SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, (uint8)SRDOORLOCKWAX_STATUS_DOOR_LOCKED); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    		    	SRData__UpdateShort(&ptr->Counter, &n_ptr->Counter, (param_ptr->Duty_On + param_ptr->Duty_Off) * 10);
        		}

    		}
    		break;


    	case SRDOORLOCKWAX_STATUS_DOOR_UNLOCKING:
    		if (ptr->Counter == 0)
    		{
		    	SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, (uint8)SRDOORLOCKWAX_STATUS_DOOR_UNLOCKING_FAILED); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    		}
    		else
    		{
        		if ( (lock_feedback.Consistency.Value == HBL_CONSISTENCY_OK) &&
        			(lock_feedback.Consistency.Table != HBL_CONSISTENCY_TABLE_ON) &&
					(lock_feedback.Consistency.Table != HBL_CONSISTENCY_TABLE_ON_ALTERNATE) )
        		{
    		    	SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, (uint8)SRDOORLOCKWAX_STATUS_DOOR_UNLOCKED); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    		    	SRData__UpdateShort(&ptr->Counter, &n_ptr->Counter, 0);
        		}

    		}
    		break;

    	case SRDOORLOCKWAX_STATUS_DOOR_LOCKED:
    		if (param_ptr->Duty_On == 0)
    		{
				pin_val = TRUE;
    		}
    		else
    		{
        		if (ptr->Counter == 0)
        		{
    		    	SRData__UpdateShort(&ptr->Counter, &n_ptr->Counter, (param_ptr->Duty_On + param_ptr->Duty_Off) * 10);
        		}
        		else if (ptr->Counter >= (param_ptr->Duty_Off * 10))
    			{
    				pin_val = TRUE;
    			}
    		}

    		break;

    	case SRDOORLOCKWAX_STATUS_DOOR_UNLOCKED:
    	case SRDOORLOCKWAX_STATUS_DOOR_LOCKING_FAILED:
    	case SRDOORLOCKWAX_STATUS_DOOR_UNLOCKING_FAILED:
    		break;

    	default:
    		if (ptr->Counter == 0)
    		{
    			if (ptr->Processed == OFF)
    			{
    				// Check if door feedback value is fine
            		if ((lock_feedback.Consistency.Value == HBL_CONSISTENCY_OK) ||
            				(lock_feedback.Byte == HBL_CONSISTENCY_TABLE_INVALID))
            		{
            			// Door already unlocked
        		    	SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, (uint8)SRDOORLOCKWAX_STATUS_DOOR_UNLOCKED); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
        		    	SRData__UpdateShort(&ptr->Counter, &n_ptr->Counter, 0);
            		}
            		else
            		{
            			// Door is going to be unlocked
        		    	SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, (uint8)SRDOORLOCKWAX_STATUS_DOOR_UNLOCKING); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
        		    	SRData__UpdateShort(&ptr->Counter, &n_ptr->Counter, ENDIANS16_STON(param_ptr->Unlock_Timeout));
            		}
    			}
    			else
    			{
    				// Check if door feedback value is fine
            		if ((lock_feedback.Consistency.Value == HBL_CONSISTENCY_OK) ||
            				(lock_feedback.Byte == HBL_CONSISTENCY_TABLE_INVALID))
            		{
            			// Door already locked
        		    	SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, (uint8)SRDOORLOCKWAX_STATUS_DOOR_LOCKED); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
        		    	SRData__UpdateShort(&ptr->Counter, &n_ptr->Counter, (param_ptr->Duty_On + param_ptr->Duty_Off) * 10);
            		}
            		else
            		{
            			// Door is going to be locked
        		    	SRData__UpdateByte((uint8 *) &ptr->Status, (uint8 *) &n_ptr->Status, (uint8)SRDOORLOCKWAX_STATUS_DOOR_LOCKING); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
        		    	SRData__UpdateShort(&ptr->Counter, &n_ptr->Counter, ENDIANS16_STON(param_ptr->Lock_Timeout));
            		}
    			}
    		}
    		break;

    	}

    	switch(ptr->Status)
    	{
    	case SRDOORLOCKWAX_STATUS_DOOR_LOCKED:
    	case SRDOORLOCKWAX_STATUS_DOOR_UNLOCKED:
    		tmp_load.Load.Status = ((pin_val == TRUE) ? HBL_LOAD_STATUS_TARGET_ALT_TABLE : HBL_LOAD_STATUS_TARGET_COMPLETED);
    		tmp_load.Load.Cur_Reg = ptr->Processed;
    		SRData__UpdateByte(&ptr->PilotStatus.Byte, &n_ptr->PilotStatus.Byte, tmp_load.Byte);
    		break;

    	case SRDOORLOCKWAX_STATUS_DOOR_LOCKING_FAILED:
    	case SRDOORLOCKWAX_STATUS_DOOR_UNLOCKING_FAILED:
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

	return(retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      One millisecond process for DoorLockWaxPilot.
 *
 *	@detail     Low level feedback probe to detect frequency and pin activation in sync with feedback
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to DoorLockWaxPilot instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of DoorLockWaxPilot 1ms execution.
 *  @retval     PASS - Execution succeeded.
 *  @retval     FAIL - Execution failed.
 */
PASS_FAIL_TYPE SRDoorLockWaxPilot__Handler1ms(HBL_LOAD_TYPE * load)
{
	SRDOORLOCKWAXPILOT_PARAM_TYPE *param_ptr;
	SRDOORLOCKWAXPILOT_DATA_TYPE *ptr;
	SRDOORLOCKWAXPILOT_DATA_TYPE *n_ptr;
	uint8 virtual_pin;
	PASS_FAIL_TYPE retval = FAIL;
	BOOL_TYPE pin_val = FALSE;
	BOOL_TYPE pin_fbk_val = FALSE;
	uint8 index = load->Pilot_Data_Index;

    if (CheckItem(index))
    {
    	param_ptr = (SRDOORLOCKWAXPILOT_PARAM_TYPE *)(load->Parameter_Ptr.Data); //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    	ptr = &SRDoorLockWaxPilot_Data[index];
    	n_ptr = &N_SRDoorLockWaxPilot_Data[index];
    	retval = PASS;

    	if (ptr->Fbk_Pin < VirtualPin__GetNumOf())
    	{
    		pin_fbk_val = Gpio__PinRead(VIRTUALPIN_TABLE[ptr->Fbk_Pin].port, VIRTUALPIN_TABLE[ptr->Fbk_Pin].pin);
    	}

    	if (pin_fbk_val != ptr->Fbk_Last_Pin_Value)
    	{
    		if (pin_fbk_val == TRUE)
    		{
    	    	if ( (ptr->Fbk_Counter >= SRDOORLOCKWAX_MIN_PERIOD_MS) &&
    					(ptr->Fbk_Counter <= SRDOORLOCKWAX_MAX_PERIOD_MS))
    			{
    	    		if (ptr->Debounce)
    	    		{
    	    			SRData__UpdateByte(&ptr->Debounce, &n_ptr->Debounce, ptr->Debounce-1);
    	    		}
    	    		else
    	    		{
    	    			sint8 tmp_offset = (2*ptr->Fbk_Last_Counter - ptr->Fbk_Counter)/4;
						if (tmp_offset < 0)
						{
							tmp_offset += (ptr->Fbk_Period / 2);
						}
	    	    		SRData__UpdateByte((uint8 *) &ptr->Fbk_Offset,(uint8 *) &n_ptr->Fbk_Offset,(uint8) tmp_offset); //lint !e926 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    	    			SRData__UpdateByte(&ptr->Fbk_Period, &n_ptr->Fbk_Period, ptr->Fbk_Counter);

    	    		}
    			}
    	    	else
    	    	{
    	    		SRData__UpdateByte(&ptr->Debounce, &n_ptr->Debounce, SRDOORLOCKWAX_MAX_DEBOUNCE);
    	    	}
        		SRData__UpdateShort(&ptr->Fbk_Timeout_Counter, &n_ptr->Fbk_Timeout_Counter, 0);
        		SRData__UpdateByte(&ptr->Fbk_Counter, &n_ptr->Fbk_Counter, 0);
    		}
    		else
    		{
        		SRData__UpdateByte(&ptr->Fbk_Last_Counter, &n_ptr->Fbk_Last_Counter, ptr->Fbk_Counter);
    		}

    		SRData__UpdateByte(&ptr->Fbk_Last_Pin_Value, &n_ptr->Fbk_Last_Pin_Value, pin_fbk_val);
    	}
    	else if (ptr->Fbk_Timeout_Counter > (param_ptr->Filter_Time*SRDOORLOCKWAX_TIMING_REF))
    	{
			SRData__UpdateByte(&ptr->Debounce, &n_ptr->Debounce, SRDOORLOCKWAX_MAX_DEBOUNCE);
    		if (ptr->Fbk_Counter >= ptr->Fbk_Period)
			{
        		SRData__UpdateByte(&ptr->Fbk_Counter, &n_ptr->Fbk_Counter, 0);
			}
    	}
    	else
    	{
    		SRData__UpdateShort(&ptr->Fbk_Timeout_Counter, &n_ptr->Fbk_Timeout_Counter, ptr->Fbk_Timeout_Counter+1);
    	}
		SRData__UpdateByte(&ptr->Fbk_Counter, &n_ptr->Fbk_Counter, ptr->Fbk_Counter+1);

    	switch(ptr->Status)
    	{
    	case SRDOORLOCKWAX_STATUS_DOOR_LOCKING:
    		pin_val = TRUE;
    		break;

    	case SRDOORLOCKWAX_STATUS_DOOR_LOCKED:
			if ((param_ptr->Duty_On == 0) ||
				(ptr->Counter >= (param_ptr->Duty_Off * 10)))
			{
				pin_val = TRUE;
			}
    		break;

    	default:
    		break;
    	}

		if (ptr->Wait_Trigger)
		{
			if ((ptr->Fbk_Counter == ptr->Fbk_Offset) ||
				(ptr->Fbk_Counter == ptr->Fbk_Offset + (ptr->Fbk_Period / 2)))
			{
				SRData__UpdateByte(&ptr->Wait_Trigger, &n_ptr->Wait_Trigger, 0);
			}
			else
			{
				pin_val = FALSE;
			}
		}

    	if (ptr->TimingRef > 0)
    	{
    		SRData__UpdateByte(&ptr->TimingRef, &n_ptr->TimingRef, ptr->TimingRef-1);
    	}
    	else
    	{
    		SRData__UpdateByte(&ptr->TimingRef, &n_ptr->TimingRef, SRDOORLOCKWAX_TIMING_REF);
    		if (ptr->Counter > 0)
    		{
				SRData__UpdateShort(&ptr->Counter, &n_ptr->Counter, ptr->Counter-1);
    		}
    	}

		pin_val ^= (BOOL_TYPE) ~load->Hi_Low_Activation_Bitmap;
		pin_val &= TRUE;
        virtual_pin = (*load->Pin_Ptr);
        if((Gpio__CheckConfig(VIRTUALPIN_TABLE[virtual_pin].port,VIRTUALPIN_TABLE[virtual_pin].pin,VIRTUALPIN_TABLE[virtual_pin].pin_config, (BOOL_TYPE)ptr->Last_Pin_Out_Value))== TRUE)
        {
			pin_val ^= (BOOL_TYPE) ~load->Hi_Low_Activation_Bitmap;
			pin_val &= TRUE;
        	Gpio__PinWrite(VIRTUALPIN_TABLE[virtual_pin].port,VIRTUALPIN_TABLE[virtual_pin].pin, pin_val);
        	SRData__UpdateByte(&ptr->Last_Pin_Out_Value, &n_ptr->Last_Pin_Out_Value, pin_val);
        }
        else
        {
        	SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, HBL_PILOT_WAX_DOOR_LOCK, index);
        }


    }

    return(retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief	Handler for DoorLockWaxPilot execution
 *
 * It checks SRDoorLockWaxPilot_Allocation integrity and all SRDoorLockWaxPilot_Data instances allocated
 * It sets SRTASKSEQUENCE_APPL_SRDOORLOCKWAXPILOT_CHECKS step in SRTASKSEQUENCE_APPL_SAFETY_THREAD sequence
 *
 */
void SRDoorLockWaxPilot__Handler(void)
{
    if (SRData__CheckByte(&SRDoorLockWaxPilot_Allocation, &N_SRDoorLockWaxPilot_Allocation) != SRDATA_OK)
    {
    	SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, HBL_PILOT_WAX_DOOR_LOCK, 0xFF);
    }
    else
    {
    	uint8 index;

    	for(index = 0; index < HBL_PILOT_NUM_SRDOORLOCKWAX; index++)
        {
        	if (SRData__CheckArray(&SRDoorLockWaxPilot_Data[index], &N_SRDoorLockWaxPilot_Data[index], sizeof(SRDOORLOCKWAXPILOT_DATA_TYPE)) != SRDATA_OK)
            {
            	SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, HBL_PILOT_WAX_DOOR_LOCK, index);
            	break;
            }
        }
    }

#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_SRDOORLOCKWAXPILOT_CHECKS);
#endif

}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

BOOL_TYPE CheckItem(uint8 index)
{
	BOOL_TYPE retval = FALSE;

    if (SRData__CheckByte(&SRDoorLockWaxPilot_Allocation, &N_SRDoorLockWaxPilot_Allocation) != SRDATA_OK)
    {
    	SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, HBL_PILOT_IMPULSES_DOOR_LOCK, 0xFF);
    }
    else
    {
    	if (index < SRDoorLockWaxPilot_Allocation)
        {
        	if (SRData__CheckArray(&SRDoorLockWaxPilot_Data[index], &N_SRDoorLockWaxPilot_Data[index], sizeof(SRDOORLOCKWAXPILOT_DATA_TYPE)) != SRDATA_OK)
            {
            	SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, HBL_PILOT_IMPULSES_DOOR_LOCK, index);
            }
        	retval = TRUE;
        }
    }

    return(retval);
}


#endif

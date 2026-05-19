/**
 *  @file       DoorLockWaxPilot.c
 *
 *  @brief      Pilot Handler for Wax DoorLock device
 *
 *  @details    This module performs locking and unlocking process of a Wax Doorlock device.
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
#include "DoorLockWaxPilot.h"

#if (HBL_PILOT_NUM_DOORLOCKWAX > 0)
#include "Gpio.h"
#include "VirtualPin.h"
#include "Hbl.h"
#include "string.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define DOORLOCKWAX_INITIAL_PERIOD          20
#define DOORLOCKWAX_TIMING_REF				10
#define DOORLOCKWAX_MAX_DEBOUNCE			2

#define ZC_ALLOWED_MIN_FREQUENCY            45
#define ZC_ALLOWED_MAX_FREQUENCY            65

// Minimum and maximum period from ZeroCross frequency boundaries set - Round to near
#define DOORLOCKWAX_MIN_PERIOD_MS		((2000+ZC_ALLOWED_MAX_FREQUENCY)/(ZC_ALLOWED_MAX_FREQUENCY*2))
#define DOORLOCKWAX_MAX_PERIOD_MS		((2000+ZC_ALLOWED_MIN_FREQUENCY)/(ZC_ALLOWED_MIN_FREQUENCY*2))


typedef enum DOORLOCKWAX_STATUS_ENUM
{
	DOORLOCKWAX_STATUS_INIT,
	DOORLOCKWAX_STATUS_DOOR_UNLOCKED,
	DOORLOCKWAX_STATUS_DOOR_LOCKING,
	DOORLOCKWAX_STATUS_DOOR_LOCKING_FAILED,
	DOORLOCKWAX_STATUS_DOOR_LOCKED,
	DOORLOCKWAX_STATUS_DOOR_UNLOCKING,
	DOORLOCKWAX_STATUS_DOOR_UNLOCKING_FAILED
} DOORLOCKWAX_STATUS_TYPE;

PACKED typedef struct
{
	uint8 Requested;
	uint8 Processed;
	HBL_LOAD_STATUS_TYPE PilotStatus;
	DOORLOCKWAX_STATUS_TYPE Status;
	uint16 Counter;
	uint16 Fbk_Timeout_Counter;
	uint8 Fbk_Counter;
	uint8 Fbk_Last_Counter;
	uint8 Fbk_Period;
	sint8 Fbk_Offset;
	uint8 Fbk_Last_Pin_Value;
	uint8 Wait_Trigger;
	uint8 Debounce;
	uint8 TimingRef;
	uint8 Fbk_Pin;
} DOORLOCKWAXPILOT_DATA_TYPE;

typedef struct
{
	uint16 Unlock_Timeout;
	uint16 Lock_Timeout;
	uint8 Duty_On;
	uint8 Duty_Off;
	uint8 Filter_Time;
	uint8 Unused;
} DOORLOCKWAXPILOT_PARAM_TYPE;

static uint8 DoorLockWaxPilot_Allocation;
static DOORLOCKWAXPILOT_DATA_TYPE DoorLockWaxPilot_Data[HBL_PILOT_NUM_DOORLOCKWAX];

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module DoorLockWaxPilot and its variables
 *
 */
void DoorLockWaxPilot__Initialize(void)
{
	DoorLockWaxPilot_Allocation = 0;
	memset(&DoorLockWaxPilot_Data, 0, sizeof(DoorLockWaxPilot_Data));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Allocate pilot instance for DoorlockWaxPilot.
 *
 *  @return     uint8 result - Allocation result.
 *  @retval     0-254 - Pilot allocation instance.
 *  @retval     0xFF - Allocation failed.
 */
uint8 DoorLockWaxPilot__Allocate(void)
{
    uint8 retval = 0xFF;

	if (DoorLockWaxPilot_Allocation < HBL_PILOT_NUM_DOORLOCKWAX)
	{
		retval = DoorLockWaxPilot_Allocation;
		DoorLockWaxPilot_Allocation++;
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
PASS_FAIL_TYPE DoorLockWaxPilot__InitializeInstance(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval = FAIL;
    uint8 index = load->Pilot_Data_Index;
    DOORLOCKWAXPILOT_PARAM_TYPE *param_ptr;
    DOORLOCKWAXPILOT_DATA_TYPE *ptr;
    uint8 *ptr_pin_table;
    uint8 fbk_index;
    uint8 virtual_pin;


    if (index < DoorLockWaxPilot_Allocation)
    {
    	ptr = (DOORLOCKWAXPILOT_DATA_TYPE *) &DoorLockWaxPilot_Data[index];  //lint !e929 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    	param_ptr = (DOORLOCKWAXPILOT_PARAM_TYPE *) load->Parameter_Ptr.Data; //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]

    	ptr->Status = DOORLOCKWAX_STATUS_INIT;
    	ptr->PilotStatus.Load.Status = HBL_LOAD_STATUS_INITIALIZE;
    	ptr->Counter = param_ptr->Filter_Time;
    	ptr->Fbk_Timeout_Counter = param_ptr->Filter_Time * DOORLOCKWAX_TIMING_REF;
    	ptr->Fbk_Period = DOORLOCKWAX_INITIAL_PERIOD;
    	ptr->Debounce = DOORLOCKWAX_MAX_DEBOUNCE;

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
		if ((virtual_pin < VirtualPin__GetNumOf()) &&
			(VIRTUALPIN_TABLE[virtual_pin].port != 0xFF))
		{
			uint8 pin_val = ~load->Hi_Low_Activation_Bitmap;
			pin_val &= 0x01;
        	Gpio__PinWrite(VIRTUALPIN_TABLE[virtual_pin].port,VIRTUALPIN_TABLE[virtual_pin].pin, (BOOL_TYPE) pin_val);
			Gpio__PinConfig(VIRTUALPIN_TABLE[virtual_pin].port,VIRTUALPIN_TABLE[virtual_pin].pin,VIRTUALPIN_TABLE[virtual_pin].pin_config);
			retval = PASS;
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
PASS_FAIL_TYPE DoorLockWaxPilot__Request(HBL_LOAD_TYPE * load,void * data)
{
	PASS_FAIL_TYPE retval = FAIL;
	uint8 index = load->Pilot_Data_Index;

    if (index < DoorLockWaxPilot_Allocation)
    {
    	DoorLockWaxPilot_Data[index].Requested = *((uint8 *) data);
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
PASS_FAIL_TYPE DoorLockWaxPilot__Process(HBL_LOAD_TYPE * load)
{
    DOORLOCKWAXPILOT_PARAM_TYPE *param_ptr;
	DOORLOCKWAXPILOT_DATA_TYPE *ptr;
	PASS_FAIL_TYPE retval = FAIL;
	uint8 index = load->Pilot_Data_Index;

    if (index < DoorLockWaxPilot_Allocation)
    {
    	ptr = &DoorLockWaxPilot_Data[index];

    	if (ptr->Requested != ptr->Processed)
    	{
        	param_ptr = (DOORLOCKWAXPILOT_PARAM_TYPE *) load->Parameter_Ptr.Data; //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]

    		if (ptr->Requested == 0)
    		{
    			switch (ptr->Status)
    			{
    			case DOORLOCKWAX_STATUS_DOOR_LOCKED:
    			case DOORLOCKWAX_STATUS_DOOR_LOCKING:
    				ptr->PilotStatus.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
    				ptr->PilotStatus.Load.Cur_Reg = ptr->Processed;
    		    	ptr->Status = DOORLOCKWAX_STATUS_DOOR_UNLOCKING;
    				ptr->Counter = ENDIANS16_STON(param_ptr->Unlock_Timeout);
    		    	ptr->Wait_Trigger = 1;
    				break;

    			default:
    		    	ptr->Status = DOORLOCKWAX_STATUS_INIT;
    				ptr->Counter = param_ptr->Filter_Time;
    				break;
    			}
    		}
    		else
			{
				switch (DoorLockWaxPilot_Data[index].Status)
				{
				case DOORLOCKWAX_STATUS_DOOR_UNLOCKED:
				case DOORLOCKWAX_STATUS_DOOR_UNLOCKING:
					ptr->PilotStatus.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
					ptr->PilotStatus.Load.Cur_Reg = ptr->Processed;
    		    	ptr->Status = DOORLOCKWAX_STATUS_DOOR_LOCKING;
    				ptr->Counter = ENDIANS16_STON(param_ptr->Unlock_Timeout);
    		    	ptr->Wait_Trigger = 1;
					break;

				default:
    		    	ptr->Status = DOORLOCKWAX_STATUS_INIT;
    				ptr->Counter = param_ptr->Filter_Time;
					break;
				}
			}
    	}

    	ptr->Processed = ptr->Requested;
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
void * DoorLockWaxPilot__GetRequested(HBL_LOAD_TYPE * load)
{
	void *ptr = NULL;

    if (load->Pilot_Data_Index < DoorLockWaxPilot_Allocation)
    {
        ptr = (void *) (&DoorLockWaxPilot_Data[load->Pilot_Data_Index].Requested);
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
void * DoorLockWaxPilot__GetProcessed(HBL_LOAD_TYPE * load)
{
	void *ptr;

	ptr = NULL;

    if (load->Pilot_Data_Index < DoorLockWaxPilot_Allocation)
    {
        ptr = (void *) (&DoorLockWaxPilot_Data[load->Pilot_Data_Index].Processed);
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
void * DoorLockWaxPilot__GetStatus(HBL_LOAD_TYPE * load)
{
	void *ptr = NULL;

    if (load->Pilot_Data_Index < DoorLockWaxPilot_Allocation)
    {
        ptr = (void *) (&DoorLockWaxPilot_Data[load->Pilot_Data_Index].PilotStatus);
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
PASS_FAIL_TYPE DoorLockWaxPilot__AsynchProcess(HBL_LOAD_TYPE * load)
{
	DOORLOCKWAXPILOT_PARAM_TYPE *param_ptr;
	DOORLOCKWAXPILOT_DATA_TYPE *ptr;
	HBL_CONSISTENCY_TYPE lock_feedback;
	HBL_LOAD_STATUS_TYPE tmp_load;
	PASS_FAIL_TYPE retval = FAIL;
	BOOL_TYPE pin_val = FALSE;
	uint8 index = load->Pilot_Data_Index;

    if (index < DoorLockWaxPilot_Allocation)
    {
    	ptr = &DoorLockWaxPilot_Data[index];
    	param_ptr = (DOORLOCKWAXPILOT_PARAM_TYPE *) load->Parameter_Ptr.Data; //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
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
    	case DOORLOCKWAX_STATUS_DOOR_LOCKING:
    		pin_val = TRUE;
    		if (ptr->Counter == 0)
    		{
		    	ptr->Status = DOORLOCKWAX_STATUS_DOOR_LOCKING_FAILED;
    		}
    		else
    		{
        		if ( (lock_feedback.Consistency.Value == HBL_CONSISTENCY_OK) &&
        				(lock_feedback.Consistency.Table != HBL_CONSISTENCY_TABLE_OFF))
        		{
    		    	ptr->Status = DOORLOCKWAX_STATUS_DOOR_LOCKED;
    		    	ptr->Counter = (param_ptr->Duty_On + param_ptr->Duty_Off) * 10;
        		}

    		}
    		break;


    	case DOORLOCKWAX_STATUS_DOOR_UNLOCKING:
    		if (ptr->Counter == 0)
    		{
		    	ptr->Status = DOORLOCKWAX_STATUS_DOOR_UNLOCKING_FAILED;
    		}
    		else
    		{
        		if ( (lock_feedback.Consistency.Value == HBL_CONSISTENCY_OK) &&
        			(lock_feedback.Consistency.Table != HBL_CONSISTENCY_TABLE_ON) &&
					(lock_feedback.Consistency.Table != HBL_CONSISTENCY_TABLE_ON_ALTERNATE) )
        		{
    		    	ptr->Status = DOORLOCKWAX_STATUS_DOOR_UNLOCKED;
    		    	ptr->Counter = 0;
        		}

    		}
    		break;

    	case DOORLOCKWAX_STATUS_DOOR_LOCKED:
    		if (param_ptr->Duty_On == 0)
    		{
				pin_val = TRUE;
    		}
    		else
    		{
        		if (ptr->Counter == 0)
        		{
    		    	ptr->Counter = (param_ptr->Duty_On + param_ptr->Duty_Off) * 10;
        		}
        		else if (ptr->Counter >= (param_ptr->Duty_Off * 10))
    			{
    				pin_val = TRUE;
    			}
    		}

    		break;

    	case DOORLOCKWAX_STATUS_DOOR_UNLOCKED:
    	case DOORLOCKWAX_STATUS_DOOR_LOCKING_FAILED:
    	case DOORLOCKWAX_STATUS_DOOR_UNLOCKING_FAILED:
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
        		    	ptr->Status = DOORLOCKWAX_STATUS_DOOR_UNLOCKED;
        		    	ptr->Counter = 0;
            		}
            		else
            		{
            			// Door is going to be unlocked
        		    	ptr->Status = DOORLOCKWAX_STATUS_DOOR_UNLOCKING;
        		    	ptr->Counter = ENDIANS16_STON(param_ptr->Unlock_Timeout);
            		}
    			}
    			else
    			{
    				// Check if door feedback value is fine
            		if ((lock_feedback.Consistency.Value == HBL_CONSISTENCY_OK) ||
            				(lock_feedback.Byte == HBL_CONSISTENCY_TABLE_INVALID))
            		{
            			// Door already locked
        		    	ptr->Status = DOORLOCKWAX_STATUS_DOOR_LOCKED;
        		    	ptr->Counter = (param_ptr->Duty_On + param_ptr->Duty_Off) * 10;
            		}
            		else
            		{
            			// Door is going to be locked
        		    	ptr->Status = DOORLOCKWAX_STATUS_DOOR_LOCKING;
        		    	ptr->Counter = ENDIANS16_STON(param_ptr->Lock_Timeout);
            		}
    			}
    		}
    		break;

    	}

    	switch(ptr->Status)
    	{
    	case DOORLOCKWAX_STATUS_DOOR_LOCKED:
    	case DOORLOCKWAX_STATUS_DOOR_UNLOCKED:
    		ptr->PilotStatus.Load.Status = ((pin_val == TRUE) ? HBL_LOAD_STATUS_TARGET_ALT_TABLE : HBL_LOAD_STATUS_TARGET_COMPLETED);
    		ptr->PilotStatus.Load.Cur_Reg = ptr->Processed;
    		break;

    	case DOORLOCKWAX_STATUS_DOOR_LOCKING_FAILED:
    	case DOORLOCKWAX_STATUS_DOOR_UNLOCKING_FAILED:
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
PASS_FAIL_TYPE DoorLockWaxPilot__Handler1ms(HBL_LOAD_TYPE * load)
{
	DOORLOCKWAXPILOT_PARAM_TYPE *param_ptr;
	DOORLOCKWAXPILOT_DATA_TYPE *ptr;
	uint8 virtual_pin;
	PASS_FAIL_TYPE retval = FAIL;
	BOOL_TYPE pin_val = FALSE;
	BOOL_TYPE pin_fbk_val = FALSE;
	uint8 index = load->Pilot_Data_Index;

    if (index < DoorLockWaxPilot_Allocation)
    {
    	param_ptr = (DOORLOCKWAXPILOT_PARAM_TYPE *)(load->Parameter_Ptr.Data); //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    	ptr = &DoorLockWaxPilot_Data[index];
    	retval = PASS;

    	if (ptr->Fbk_Pin < VirtualPin__GetNumOf())
    	{
    		pin_fbk_val = Gpio__PinRead(VIRTUALPIN_TABLE[ptr->Fbk_Pin].port, VIRTUALPIN_TABLE[ptr->Fbk_Pin].pin);
    	}

    	if (pin_fbk_val != ptr->Fbk_Last_Pin_Value)
    	{
    		if (pin_fbk_val == TRUE)
    		{
    	    	if ( (ptr->Fbk_Counter >= DOORLOCKWAX_MIN_PERIOD_MS) &&
    					(ptr->Fbk_Counter <= DOORLOCKWAX_MAX_PERIOD_MS))
    			{
    	    		if (ptr->Debounce)
    	    		{
    	    			ptr->Debounce--;
    	    		}
    	    		else
    	    		{
    	    			sint8 tmp_offset = (2*ptr->Fbk_Last_Counter - ptr->Fbk_Counter)/4;
						if (tmp_offset < 0)
						{
							tmp_offset += (ptr->Fbk_Period / 2);
						}
	    	    		ptr->Fbk_Offset =  tmp_offset; //lint !e926 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    	    			ptr->Fbk_Period = ptr->Fbk_Counter;

    	    		}
    			}
    	    	else
    	    	{
    	    		ptr->Debounce = DOORLOCKWAX_MAX_DEBOUNCE;
    	    	}
        		ptr->Fbk_Timeout_Counter = 0;
        		ptr->Fbk_Counter= 0;
    		}
    		else
    		{
        		ptr->Fbk_Last_Counter = ptr->Fbk_Counter;
    		}

    		ptr->Fbk_Last_Pin_Value =  pin_fbk_val;
    	}
    	else if (ptr->Fbk_Timeout_Counter > (param_ptr->Filter_Time*DOORLOCKWAX_TIMING_REF))
    	{
			ptr->Debounce = DOORLOCKWAX_MAX_DEBOUNCE;
    		if (ptr->Fbk_Counter >= ptr->Fbk_Period)
			{
        		ptr->Fbk_Counter = 0;
			}
    	}
    	else
    	{
    		ptr->Fbk_Timeout_Counter++;
    	}
		ptr->Fbk_Counter++;

    	switch(ptr->Status)
    	{
    	case DOORLOCKWAX_STATUS_DOOR_LOCKING:
    		pin_val = TRUE;
    		break;

    	case DOORLOCKWAX_STATUS_DOOR_LOCKED:
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
				ptr->Wait_Trigger = 0;
			}
			else
			{
				pin_val = FALSE;
			}
		}

    	if (ptr->TimingRef > 0)
    	{
    		ptr->TimingRef--;
    	}
    	else
    	{
    		ptr->TimingRef = DOORLOCKWAX_TIMING_REF;
    		if (ptr->Counter > 0)
    		{
				ptr->Counter--;
    		}
    	}

		pin_val ^= ~load->Hi_Low_Activation_Bitmap;
		pin_val &= 0x01;
        virtual_pin = (*load->Pin_Ptr);
		Gpio__PinWrite(VIRTUALPIN_TABLE[virtual_pin].port,VIRTUALPIN_TABLE[virtual_pin].pin, pin_val);

    }

    return(retval);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


#endif

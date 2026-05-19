/**
 *  @file       HeaterLoad.c
 *
 *  @brief      Load Handler that performs delayed activation between two pins required to activate load
 *
 *  @details    This module executes heater activation with a predefined sequence:
 *              - If Pilot is DigitalOutputPilot or MultiDriver, it toggles Pin1, wait time, then toggles Pin2
 *              - If Pilot is MultiSequence, sequence has to be set in Pilot configuration;
 *                  it will use Sequence 1 to OFF, Sequence 2 to ON
 *              - Any other Pilot type will not perform any sequence at all
 *              Wait time can be customized defining HBL_LOAD_HEATER_DELAY with value in ms; ; if undefined, default set is 100ms
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "HeaterLoad.h"

#if (HBL_LOAD_NUM_HEATER > 0)
#include "SystemTimers.h"
#include "string.h"

#include "Hbl.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#ifndef HBL_LOAD_HEATER_DELAY
    #define HBL_LOAD_HEATER_DELAY   50
#endif

typedef struct _HEATERLOAD_DATA_STRUCT
{
    uint32               TimerCounter;
    uint8                Requested;
    uint8                Processed;
    HBL_LOAD_STATUS_TYPE Status;
    BOOL_TYPE			 Invert_Pin_Sequence;
    HBL_LOAD_ENUM_TYPE	 Load_Id;
} HEATERLOAD_DATA_TYPE;


static uint8    HeaterLoad_Allocation;
static HEATERLOAD_DATA_TYPE HeaterLoad_Data[HBL_LOAD_NUM_HEATER];

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module HeaterLoad and its variables
 *
 */
void HeaterLoad__Initialize(void)
{
    HeaterLoad_Allocation = 0;
    memset(&HeaterLoad_Data[0], 0, sizeof(HeaterLoad_Data));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Reserve memory for this load instance.
 * @return The index of the allocated load.
 */
uint8 HeaterLoad__Allocate(void)
{
    uint8 retval;
    retval = 0xFF;
    if (HeaterLoad_Allocation < (uint8)HBL_LOAD_NUM_HEATER)
    {
        retval = HeaterLoad_Allocation;
        HeaterLoad_Allocation++;
    }
    return (retval);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  This InitializeInstance function is called after an instance has been allocated in order to initialize the load instance.
 * @param A pointer to the HBL_LOAD_TYPE structure with load and load instance information.
 * @return PASS if the load is successfully initialize; otherwise FAIL.
 *
 */
PASS_FAIL_TYPE HeaterLoad__InitializeInstance(HBL_LOAD_TYPE * load)
{
    HEATERLOAD_DATA_TYPE  *data_ptr;
    PASS_FAIL_TYPE retval;

    retval = FAIL;

    if (load->Load_Data_Index < HeaterLoad_Allocation)
    {
        data_ptr = &HeaterLoad_Data[load->Load_Data_Index];

        data_ptr->Status.Load.Cur_Reg = data_ptr->Processed;
        data_ptr->Status.Load.Status = HBL_LOAD_STATUS_INITIALIZE;
        data_ptr->TimerCounter = SystemTimers__GetFreeRunningCounter();
        data_ptr->Load_Id = load->Load_Id;

        retval = PASS;
    }

    return (retval);
}

PASS_FAIL_TYPE HeaterLoad__Request(HBL_LOAD_TYPE * load,void * data)
{
    PASS_FAIL_TYPE retval = FAIL;

    if (load->Load_Data_Index < HeaterLoad_Allocation)
    {
        HeaterLoad_Data[load->Load_Data_Index].Requested = *((uint8 *)data);
        retval = PASS;
    }

    return(retval);
}

PASS_FAIL_TYPE HeaterLoad__Process(HBL_LOAD_TYPE * load , HBL_PILOT_HANDLER_HOOK_TYPE * pilot)
{
    PASS_FAIL_TYPE retval = FAIL;

    if (load->Load_Data_Index < HeaterLoad_Allocation)
    {
        HEATERLOAD_DATA_TYPE* data_ptr = &HeaterLoad_Data[load->Load_Data_Index];
        if (data_ptr->Requested != data_ptr->Processed)
        {
            data_ptr->TimerCounter = SystemTimers__GetFreeRunningCounter();

            if ((data_ptr->Requested == (uint8)ON) ||
                (data_ptr->Requested == (uint8)OFF))
            {
                if ((data_ptr->Processed == (uint8)ON) || (data_ptr->Processed == (uint8)OFF))
                {
                	data_ptr->TimerCounter += (uint8)HBL_LOAD_HEATER_DELAY;
                }
                else
                {
                	// If ((data_ptr->Processed == 0x1x or 0x2x) no delay is applied because
                	// no transient state is required.
                }

                data_ptr->Status.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
                if (data_ptr->Requested == (uint8)ON)
                {
                	data_ptr->Invert_Pin_Sequence = !data_ptr->Invert_Pin_Sequence;
                }
            }
            else
            {
            	data_ptr->Status.Load.Status = HBL_LOAD_STATUS_TARGET_ALT_TABLE;
            }

            data_ptr->Processed = data_ptr->Requested;
            data_ptr->Status.Load.Cur_Reg = data_ptr->Requested;
        }

        retval = PASS;
    }

    return(retval);
}

void * HeaterLoad__GetRequested(HBL_LOAD_TYPE * load)
{
    void* retval = NULL;

    if (load->Load_Data_Index < HeaterLoad_Allocation)
    {
        retval = &HeaterLoad_Data[load->Load_Data_Index].Requested;
    }

    return(retval);
}

void * HeaterLoad__GetProcessed(HBL_LOAD_TYPE * load , HBL_PILOT_HANDLER_HOOK_TYPE * pilot)
{
    void* retval = NULL;

    if (load->Load_Data_Index < HeaterLoad_Allocation)
    {
        retval = &HeaterLoad_Data[load->Load_Data_Index].Processed;
    }

    return(retval);
}


PASS_FAIL_TYPE HeaterLoad__AsyncProcess(HBL_LOAD_TYPE * load , HBL_PILOT_HANDLER_HOOK_TYPE * pilot)
{
    PASS_FAIL_TYPE retval = FAIL;

    if (load->Load_Data_Index < HeaterLoad_Allocation)
    {
        sint32 timestamp = (sint32) SystemTimers__GetFreeRunningCounter();
        HEATERLOAD_DATA_TYPE* data_ptr = &HeaterLoad_Data[load->Load_Data_Index];
        uint8 pilot_output[2];

        pilot_output[0] = data_ptr->Processed;
        pilot_output[1] = (uint8) OFF;

        if ( ((data_ptr->Status.Load.Status == (HBL_LOAD_STATUS_ENUM_TYPE)HBL_LOAD_STATUS_WAITING_TARGET) ||
             (data_ptr->Status.Load.Status == (HBL_LOAD_STATUS_ENUM_TYPE)HBL_LOAD_STATUS_INITIALIZE)) &&
            ((timestamp - (sint32) data_ptr->TimerCounter) >= 0) )
        {
            data_ptr->Status.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
        }

        if ((load->Pilot_Type == HBL_PILOT_DIGITAL_OUTPUT_MULTI_2_PIN) ||
            (load->Pilot_Type == HBL_PILOT_SR_DIGITAL_OUTPUT_MULTI_2_PIN))
        {
            if (data_ptr->Status.Load.Status == (HBL_LOAD_STATUS_ENUM_TYPE)HBL_LOAD_STATUS_WAITING_TARGET)
            {
                if (data_ptr->Invert_Pin_Sequence == FALSE)
                {
                	pilot_output[0] |= (uint8)0x20;
                }
                else
                {
					pilot_output[0] |= (uint8)0x10;
                }
            }
        }
        else
        {
            data_ptr->Status.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
        }

        (void)pilot->Request(load, &pilot_output[0]);
        (void)pilot->Process(load);
        if (pilot->AsynchProcess != NULL)
        {
            (void)pilot->AsynchProcess(load);
        }
        retval = PASS;
    }

    return(retval);
}

void * HeaterLoad__GetStatus(HBL_LOAD_TYPE * load , HBL_PILOT_HANDLER_HOOK_TYPE * pilot)
{
    void* retval = NULL;

    if (load->Load_Data_Index < HeaterLoad_Allocation)
    {
        retval = &HeaterLoad_Data[load->Load_Data_Index].Status;
    }

    return(retval);
}


BOOL_TYPE *HeaterLoad__GetInvertedSequenceStatusByLoadId(HBL_LOAD_ENUM_TYPE load_Id)
{
	BOOL_TYPE *retval = NULL;
	uint8 i;

	for(i = 0; i < HeaterLoad_Allocation; i++)
	{
		if (HeaterLoad_Data[i].Load_Id == load_Id)
		{
	        retval = &HeaterLoad_Data[i].Invert_Pin_Sequence;
		}
	}

    return(retval);
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


#endif // HBL_LOAD_NUM_HEATER > 0

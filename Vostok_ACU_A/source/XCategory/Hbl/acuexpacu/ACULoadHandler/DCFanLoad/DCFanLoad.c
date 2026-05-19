/**
 *  @file
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------

//lint -e766 Suppress Info: header not used in module 'VirtualGI.c'
#include "SystemConfig.h"
#if (HBL_LOAD_NUM_DCFAN > 0)
#include "DCFanLoad.h"

#include "String.h"
#include "Millis.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
typedef enum
{
    DCFANLOAD_STATE_OFF = 0,
    DCFANLOAD_STATE_RAMPING_UP,
    DCFANLOAD_STATE_ON
}DCFANLOAD_STATE_TYPE;

typedef struct
{
    MILLIS_TIMER_TYPE Timer_Handle;
    uint8 Processed_Speed;
    uint8 Requested_Speed;
    uint8 Accepted_Request;
    DCFANLOAD_STATE_TYPE State;
}DCFANLOAD_DATA_TYPE;

/**
 * Struct to cast the parameters for the FAN Load.
 */
typedef PACKED struct
{
    uint16 Startup_Time;
    uint8 Startup_Speed;
    uint8 reserved;
} DCFANLOAD_PARAMETERS_TYPE;

static DCFANLOAD_DATA_TYPE DCFanLoad_Data[HBL_LOAD_NUM_DCFAN];
static uint8 DCFanLoad_Allocation;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module DCFanLoad and its variables
 *
 */
void DCFanLoad__Initialize(void)
{
    DCFanLoad_Allocation = 0;
    memset(DCFanLoad_Data,0,sizeof(DCFanLoad_Data));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief -
 * @return
 */
uint8 DCFanLoad__Allocate(void)
{
    uint8 retval;
    retval = 0xFF;
    if (DCFanLoad_Allocation < HBL_LOAD_NUM_DCFAN)
    {
        retval = DCFanLoad_Allocation;
        DCFanLoad_Allocation++;

    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief -
 *
 * @param load
 * @return
 */
PASS_FAIL_TYPE DCFanLoad__InitializeInstance(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE ret_val = FAIL;

    if ( load->Load_Data_Index < DCFanLoad_Allocation )
    {
        DCFanLoad_Data[load->Load_Data_Index].Processed_Speed  = 0;
        DCFanLoad_Data[load->Load_Data_Index].Requested_Speed  = 0;
        DCFanLoad_Data[load->Load_Data_Index].Accepted_Request = 0;

        //Reset the timer
        Millis__Reset(&DCFanLoad_Data[load->Load_Data_Index].Timer_Handle);

        ret_val = PASS;
    }
    return ( ret_val );
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief -
 *
 * @param load
 * @param data
 * @return
 */
PASS_FAIL_TYPE DCFanLoad__Request(HBL_LOAD_TYPE * load,void * data)
{
    if (load->Load_Data_Index < DCFanLoad_Allocation)
    {
        DCFanLoad_Data[load->Load_Data_Index].Requested_Speed = *((uint8 *)data);
        return (PASS);
    }
    else
    {
        return (FAIL);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief -
 *
 * @param load
 * @param pilot
 * @return
 */
PASS_FAIL_TYPE DCFanLoad__Process(HBL_LOAD_TYPE * load, HBL_PILOT_HANDLER_HOOK_TYPE * pilot)
{
    PASS_FAIL_TYPE retval;

    retval = FAIL;

    if (load->Load_Data_Index < DCFanLoad_Allocation)
    {
    	if(DCFanLoad_Data[load->Load_Data_Index].Accepted_Request != DCFanLoad_Data[load->Load_Data_Index].Requested_Speed)
    	{
    		DCFanLoad_Data[load->Load_Data_Index].Accepted_Request = DCFanLoad_Data[load->Load_Data_Index].Requested_Speed;
    	}
    	retval = PASS;
    }

    return(retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Getter for the Requested speed.
 * @param load compressor from which to get the requested speed.
 * @return NULL in case of failure. Otherwise the pointer to the requested speed. Cast it using *((COMPRESSORBASIC_SPEED_TYPE *)ptr) to get the value.
 */
void * DCFanLoad__GetRequested(HBL_LOAD_TYPE * load)
{
    if (load->Load_Data_Index < DCFanLoad_Allocation)
    {
        return (&DCFanLoad_Data[load->Load_Data_Index].Requested_Speed);
    }
    else
    {
        return (NULL);
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief -
 *
 * @param load
 * @param pilot
 */
void * DCFanLoad__GetProcessed(HBL_LOAD_TYPE * load, HBL_PILOT_HANDLER_HOOK_TYPE * pilot)
{
    if (load->Load_Data_Index < DCFanLoad_Allocation)
    {
        return (pilot->GetProcessed(load));
    }
    else
    {
        return (NULL);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief -
 * @param load
 * @param pilot
 * @return
 */
PASS_FAIL_TYPE DCFanLoad__AsyncProcess(HBL_LOAD_TYPE * load, HBL_PILOT_HANDLER_HOOK_TYPE * pilot)
{
    DCFANLOAD_PARAMETERS_TYPE *parameters = (DCFANLOAD_PARAMETERS_TYPE *)(void*)load->Load_Parameter_Ptr.Data;

    if (load->Load_Data_Index < DCFanLoad_Allocation)
    {
        switch (DCFanLoad_Data[load->Load_Data_Index].State)
        {
            default:
            case DCFANLOAD_STATE_OFF:
                DCFanLoad_Data[load->Load_Data_Index].Processed_Speed = 0;

                if (DCFanLoad_Data[load->Load_Data_Index].Accepted_Request > 0)
                {
                    DCFanLoad_Data[load->Load_Data_Index].State           = DCFANLOAD_STATE_RAMPING_UP;

                    Millis__Start(&DCFanLoad_Data[load->Load_Data_Index].Timer_Handle);
                    if(parameters->Startup_Speed == 0)
                    {
                    	DCFanLoad_Data[load->Load_Data_Index].Processed_Speed = DCFanLoad_Data[load->Load_Data_Index].Accepted_Request;
                    }
                    else
                    {
                    	DCFanLoad_Data[load->Load_Data_Index].Processed_Speed = parameters->Startup_Speed;
                    }
                }
                break;
            case DCFANLOAD_STATE_RAMPING_UP:
                if ( DCFanLoad_Data[load->Load_Data_Index].Accepted_Request == 0 )
                {
                    // the request is zero, goto to state off
                    DCFanLoad_Data[load->Load_Data_Index].State = DCFANLOAD_STATE_OFF;
                }
                else if ( Millis__ReachedTarget(&DCFanLoad_Data[load->Load_Data_Index].Timer_Handle , ENDIANU16_STON(parameters->Startup_Time)) != FALSE )
                {
                    DCFanLoad_Data[load->Load_Data_Index].Processed_Speed = DCFanLoad_Data[load->Load_Data_Index].Accepted_Request;
                    DCFanLoad_Data[load->Load_Data_Index].State = DCFANLOAD_STATE_ON;
                }
                break;
            case DCFANLOAD_STATE_ON:
                if ( DCFanLoad_Data[load->Load_Data_Index].Accepted_Request == 0 )
                {
                    // the request is zero, goto to state off
                    DCFanLoad_Data[load->Load_Data_Index].State = DCFANLOAD_STATE_OFF;
                }
                else
                {
                    DCFanLoad_Data[load->Load_Data_Index].Processed_Speed = DCFanLoad_Data[load->Load_Data_Index].Accepted_Request;
                }
                break;
        }

        if(pilot->Request != NULL)
        {
        	pilot->Request(load,&(DCFanLoad_Data[load->Load_Data_Index].Processed_Speed));
        }
		if(pilot->Process != NULL)
		{
			pilot->Process(load);
		}
		if(pilot->AsynchProcess != NULL)
		{
			pilot->AsynchProcess(load);
		}

        return (PASS);
    }
    else
    {
        return (FAIL);
    }
}
/**
 * Returns the status of what is on the Pilot if the pilot has the GetStatus function implemented.
 * Otherwise returns the Processed_Speed.
 * @param load
 * @param pilot
 * @return uint8 with the Processed_Speed or the status of the Pilot if the Pilot is having the GetStatus implemented.
 */
void * DCFanLoad__GetStatus	(HBL_LOAD_TYPE * load, HBL_PILOT_HANDLER_HOOK_TYPE * pilot)
{
	if((load->Load_Data_Index >= DCFanLoad_Allocation) ||
	   (pilot->GetStatus == NULL))
	{
		return (&DCFanLoad_Data[load->Load_Data_Index].Processed_Speed);
	}
	else
	{
		return (pilot->GetStatus(load));
	}
}
#endif

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================



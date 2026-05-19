/**
 *  @file       
 *
 *  @brief      This is Damper ON/OFF type
 *
 *
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "SteadyPwmProfilePilot.h"

#if (HBL_PILOT_NUM_STEADYPWMPROFILE > 0)

#include "Millis.h"
#include "string.h"
#include "Timer.h"
#include "VirtualPin.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#define STEADYPWMPROFILE_MAXIMUM_PROFILE_SUPPORTED      16
#define ABSOLUTE_VALUE(x,y)                             (x>y? x-y:y-x)

PACKED typedef struct
{
    uint8 Maximum_Duty_Step;
    uint8 Minimum_Duty_Step;
    uint8 Valid_Minimum_Duty;       //minimum initial speed required for the motor to spin
    uint8 Unused;
    uint16 Step_Interval_Time;
    uint16 Base_Frequency;
    uint8 Profile_Number;
    uint8 Duty_Profile;
} STEADYPWMPROFILEPILOT_PARAMETERS_TYPE;

typedef struct
{
    MILLIS_TIMER_TYPE Timer_Handler;
    TIMER_PARAMETER_TYPE Timer_Parameter;
    uint8 Requested;
    uint8 Processed;
    HBL_LOAD_STATUS_TYPE Status;
    uint8 Processed_Duty;
    BOOL_TYPE Frequency_Configuration_Needed;
} STEADYPWMPROFILEPILOT_DATA_TYPE;

static STEADYPWMPROFILEPILOT_DATA_TYPE SteadyPwmProfilePilot_Data[HBL_PILOT_NUM_STEADYPWMPROFILE];

static uint8 SteadyPwmProfilePilot_Allocation;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module SteadyPwmProfilePilot and its variables
 *
 */
void SteadyPwmProfilePilot__Initialize(void)
{
    SteadyPwmProfilePilot_Allocation = 0;
    memset(SteadyPwmProfilePilot_Data,0,sizeof(SteadyPwmProfilePilot_Data));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @return
 */
uint8 SteadyPwmProfilePilot__Allocate(void)
{
    uint8 retval = HBL_INDEX_INVALID;

    if (SteadyPwmProfilePilot_Allocation < HBL_PILOT_NUM_STEADYPWMPROFILE)
    {
        retval = SteadyPwmProfilePilot_Allocation;
        SteadyPwmProfilePilot_Allocation++;
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param load
 */
PASS_FAIL_TYPE SteadyPwmProfilePilot__InitializeInstance(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval = FAIL;

    if (load->Pilot_Data_Index < SteadyPwmProfilePilot_Allocation)
    {
    	STEADYPWMPROFILEPILOT_DATA_TYPE *data = &SteadyPwmProfilePilot_Data[load->Pilot_Data_Index];
    	STEADYPWMPROFILEPILOT_PARAMETERS_TYPE *parameters  = (void *)load->Parameter_Ptr.Data;
        uint8 virtual_pin = load->Pin_Ptr[0];

        data->Requested          = 0;
        data->Processed          = 0;
        data->Status.Load.Status = HBL_LOAD_STATUS_INITIALIZE;
        data->Status.Load.Cur_Reg= (uint8) OFF;
        data->Processed_Duty     = 0;
        data->Frequency_Configuration_Needed       = TRUE;

        if (parameters->Step_Interval_Time != 0xFFFF)
        {
            Millis__Reset(&data->Timer_Handler);
            Millis__SetElapsed(&data->Timer_Handler, ENDIANU16_STON(parameters->Step_Interval_Time));
        }

        data->Timer_Parameter.Timer         = (TIMER_TYPE)VIRTUALPIN_TABLE[virtual_pin].device;
        data->Timer_Parameter.Channel       = (TIMER_CHANNEL_TYPE)VIRTUALPIN_TABLE[virtual_pin].param0;
        data->Timer_Parameter.Mode          = TIMER_MODE_PWM;
        data->Timer_Parameter.Port          = VIRTUALPIN_TABLE[virtual_pin].port;
        data->Timer_Parameter.Pin           = VIRTUALPIN_TABLE[virtual_pin].pin;
        data->Timer_Parameter.Pin_ReMap     = VIRTUALPIN_TABLE[virtual_pin].pin_remap;
        data->Timer_Parameter.Polarity      = TIMER_POLARITY_ACTIVE_HIGH;

        retval = Timer__Config(&data->Timer_Parameter);
        Timer__PwmStop(&data->Timer_Parameter, (BOOL_TYPE)load->Hi_Low_Activation_Bitmap);
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @param load
 * @param data
 * @return
 */
PASS_FAIL_TYPE SteadyPwmProfilePilot__Request(HBL_LOAD_TYPE * load, void * data)
{
    PASS_FAIL_TYPE retval = FAIL;

    if (load->Pilot_Data_Index < SteadyPwmProfilePilot_Allocation)
    {
        STEADYPWMPROFILEPILOT_PARAMETERS_TYPE *parameters = (void *)load->Parameter_Ptr.Data;
    	if(parameters->Profile_Number >= (*(uint8 *)data))
        {
        	SteadyPwmProfilePilot_Data[load->Pilot_Data_Index].Requested = *((uint8 *)data);
            retval = PASS;
        }
    }
	return(retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param load
 * @param data
 * @return
 */
PASS_FAIL_TYPE SteadyPwmProfilePilot__Process(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval = FAIL;

    if (load->Pilot_Data_Index < SteadyPwmProfilePilot_Allocation)
    {
    	STEADYPWMPROFILEPILOT_DATA_TYPE *data = &SteadyPwmProfilePilot_Data[load->Pilot_Data_Index];
        data->Processed = data->Requested;
        retval = PASS;
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param load
 * @param data_out
 * @return
 */
void * SteadyPwmProfilePilot__GetRequested(HBL_LOAD_TYPE * load)
{
    void *retval = NULL;
    if (load->Pilot_Data_Index < SteadyPwmProfilePilot_Allocation)
    {
        retval = &SteadyPwmProfilePilot_Data[load->Pilot_Data_Index].Requested;
    }
	return(retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param load
 * @param data_out
 * @return
 */
void * SteadyPwmProfilePilot__GetProcessed(HBL_LOAD_TYPE * load)
{
    void *retval = NULL;
    if (load->Pilot_Data_Index < SteadyPwmProfilePilot_Allocation)
    {
        retval = &SteadyPwmProfilePilot_Data[load->Pilot_Data_Index].Processed;
    }
	return(retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param load
 * @return
 */
PASS_FAIL_TYPE SteadyPwmProfilePilot__OneMsHandler(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval;
    STEADYPWMPROFILEPILOT_PARAMETERS_TYPE * parameters;
    uint8 output_duty;
    uint8 requested_duty;

    parameters  = (void *)load->Parameter_Ptr.Data;
    output_duty = 0;
    retval      = FAIL;

    if (load->Pilot_Data_Index < SteadyPwmProfilePilot_Allocation)
    {
    	STEADYPWMPROFILEPILOT_DATA_TYPE *data = &SteadyPwmProfilePilot_Data[load->Pilot_Data_Index];
    	HBL_LOAD_STATUS_TYPE load_status;
        load_status.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
        load_status.Load.Cur_Reg = (uint8) ON;

    	if (data->Processed != 0)
        {
            requested_duty = (&parameters->Duty_Profile)[data->Processed - 1];
            output_duty = data->Processed_Duty;

            if (requested_duty != data->Processed_Duty)
            {
                //The below logic supports for the the Acceleration management of the loads
                if (parameters->Step_Interval_Time != 0xFFFF)
                {
                    if (Millis__ReachedTarget(&data->Timer_Handler , ENDIANU16_STON(parameters->Step_Interval_Time)) != FALSE)
                    {
                    	uint8 delta = ABSOLUTE_VALUE(requested_duty, data->Processed_Duty);

                    	Millis__Start(&data->Timer_Handler);
                    	// Duty lower than Valid_Minimum_Duty cannot be set - force it to 0
                        if (requested_duty < parameters->Valid_Minimum_Duty)
                        {
                            output_duty = 0;
                        }
                    	// Duty distance lower than Minimum_Duty_Step means it's close to target - set it at target
                        else if(delta < parameters->Minimum_Duty_Step)
                        {
                        	output_duty = requested_duty;
                        }
                        else
                        {
                        	// Limit delta to Maximum_Duty_Step - it cannot be higher than this parameter
                            if(delta > parameters->Maximum_Duty_Step)
                            {
                            	delta = parameters->Maximum_Duty_Step;
                            }

                            if (requested_duty > data->Processed_Duty)
                            {
                                output_duty = data->Processed_Duty + delta;
                            }
                            else
                            {
                                output_duty = data->Processed_Duty - delta;
                            }

                            load_status.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
                        }
                    }
                    else
                    {
                    	// Keep last Load status set
                    	load_status.Load.Status = data->Status.Load.Status;
                    }
                }
                else
                {
                    output_duty = requested_duty;
                }
            }
        }
    	else
    	{
    		output_duty = 0;
    	}

    	// Set Status Cur_Reg to OFF if Output Duty is 0
    	if (output_duty == 0)
    	{
            load_status.Load.Cur_Reg = (uint8) OFF;
    	}

    	// Update PWM Duty only if it has changed since last time
    	if (data->Processed_Duty != output_duty)
    	{
    		if (output_duty != 0)
    		{
    		    uint8 polarized_duty = output_duty;

    		    // Depending on Hi_Low_Activation_Bitmap flag: if High, 100-complement duty can be applied on Timer
    		    // since pin will be set High to turn OFF, Low to turn ON.
    		    if(load->Hi_Low_Activation_Bitmap == TRUE)
    		    {
    		        polarized_duty = 100 - output_duty;
    		    }

    		    //Calling Timer__PwmStart would affect on the ongoing frequency. In order to avoid this the below logic will call the Timer__PwmSetDutyCycle
    		    if (data->Frequency_Configuration_Needed != FALSE)
    		    {
    		        data->Frequency_Configuration_Needed = FALSE;
                    retval = Timer__PwmStart(&data->Timer_Parameter, ENDIANU16_STON(parameters->Base_Frequency), (polarized_duty *100));
    		    }
    		    else
    		    {
                    Timer__PwmSetDutyCycle(&data->Timer_Parameter, (polarized_duty *100));
    		    }
    		}
    		else
    		{
    		    data->Frequency_Configuration_Needed = TRUE;
                retval = (PASS_FAIL_TYPE) Timer__PwmStop(&data->Timer_Parameter, (BOOL_TYPE)load->Hi_Low_Activation_Bitmap);
    		}
    		data->Processed_Duty = output_duty;
    	}

    	data->Status.Byte = load_status.Byte;
    	retval = PASS;
    }

    return retval;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param load
 * @param data
 * @return
 */
void *SteadyPwmProfilePilot__GetStatus(HBL_LOAD_TYPE * load)
{
    void *retval = NULL;

    if (load->Pilot_Data_Index < SteadyPwmProfilePilot_Allocation)
    {
    	retval = &SteadyPwmProfilePilot_Data[load->Pilot_Data_Index].Status;
    }

    return (retval);
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


#endif

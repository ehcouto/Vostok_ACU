/**
 *  @file  AlternateLoad.c
 *
 *  @brief      Basic description of file contents
 *
 *  @details     This module Defines & controls the Valve position
 *               This module process the valve position as the request came from application & set the valve position 1,2 or 3.
 *               After reset it completes the one rotation and hit to the initial position.
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header:    AlternateLoad.c KAVEH RAZZAGHI (RAZZAK) Exp  $
 *
 *  @copyright  Copyright 2016.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "AlternateLoad.h"

#if (HBL_LOAD_NUM_ALTERNATE > 0)
#include "VirtualPin.h"
#include "String.h"
#include "Fault.h"
#include "Gpio.h"
#include "Selector.h"
#include "Interpreter.h"
#include "SystemTimers.h"
#include "Hbl.h"
#include "Mode.h"
#include "Micro.h"
#include "Variable.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#define 	HIGH_CAM		0
#define		LOW_CAM			1
#define 	FB_INIT_VAL  0xFF

typedef enum
{
	ALTERNATELOAD_IDLE,
	ALTERNATELOAD_DRIVE_TO_REF,
	ALTERNATELOAD_DRIVE_INC
} ALTERNATELOAD_COMMAND_TYPE;

typedef enum
{
    WAIT_FOR_COMMAND,
    MOVEMENT_ONGOING,
	POS_REACHED
} ALT_MOV_TYPE;

typedef struct {
    uint16 Alt_Engine_Is_Present      :1;
    uint16 Alt_Engine_Req_Sync        :1;
    uint16 Wait_For_Sync_Pos          :1;
    uint16 Init_Inc_Pos               :1;
    uint16 Alt_Analog_Fb_Is_Present   :1;
    uint16 Wait_For_Movement          :2;
    uint16 Cam_Increase               :1;
    uint16 Failure_Rotate			  :1;
}FLAGS_STRUCT_TYPE;

typedef union {
    uint16 				Unused;
    FLAGS_STRUCT_TYPE 	Bit;
}ALTERNATELOAD_FLAGS_TYPE;

PACKED typedef struct
{
	ALTERNATELOAD_REQUEST_TYPE   		Processed; 				 		//!< Processed value
    ALTERNATELOAD_REQUEST_TYPE   		Requested; 				  		//!< Requested value
	uint16 								Time_Low_Cam;
	uint16 								Time_High_Cam;
	uint8 								Num_Attempt;
	uint8 								Num_Cams;
	uint8								Command;
	uint8 								Triac_Off_Count;
	uint8 								Old_Feedback_Value;
    uint8 								Alt_Engine_Pos_Req;
    HBL_LOAD_STATUS_TYPE				Alt_Status;
    ALTERNATELOAD_FLAGS_TYPE		 	Flags;
} ALTERNATELOAD_DATA_TYPE;

//! Struct to cast the parameters for the Alternate pilot.
PACKED typedef struct
{
	uint8   Max_Attempts_Relative;
	uint8   Max_Attempts_Absolute;
	uint8	Max_Cam_Number;
	uint8   Motor_Cam_Number;
	uint8	Time_Off_Triac;
	uint8	Self_Switch						:1;
	uint8	Stop_Next_Cam					:1;
	uint8	Run_After_Fault					:1;
	uint8   Unused0							:5;
	uint16	Long_Low_Cam_Threshold;
	uint16	Low_Cam_Timeout;
	uint16	High_Cam_Timeout;
	uint16	High_Cam_Threshold; // or Diverter Stop Delay if Stop_Next_Cam == 0
    uint8   Rotation_Time;
    uint8   Unused;
} ALTERNATELOAD_PARAMETERS_TYPE;


// -- Private Variable Definitions --

static ALTERNATELOAD_DATA_TYPE AlternateLoad_Data[HBL_LOAD_NUM_ALTERNATE];
static uint8 AlternateLoad_Allocation;
static BOOL_TYPE Run_After_Fault_Flag;
static uint8 Diverter_Rotation_Time;
static uint8 Diverter_Cam_Number;
static BOOL_TYPE Diverter_Rotate_Always;
volatile BOOL_TYPE Diverter_Motor_Status;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static uint8 AlternateLoadManager(HBL_LOAD_TYPE * load , HBL_PILOT_HANDLER_HOOK_TYPE * pilot);
static void AlternateLoadEngineDrive(HBL_LOAD_TYPE * load , HBL_PILOT_HANDLER_HOOK_TYPE * pilot);
static void AlternateLoadSetCommand(HBL_LOAD_TYPE * load , uint8 command);

static BOOL_TYPE AlternateLoad_Search_Home;
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

/**
 * @brief Initialize all instances of this load type.
 *        Initialize static RAM used by the module.
 * @return
 */
void AlternateLoad__Initialize(void)
{
    AlternateLoad_Allocation = 0;
    Diverter_Rotation_Time = 0;
    Diverter_Cam_Number = 0;
    Diverter_Rotate_Always = FALSE;

    if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
    {
    	memset (AlternateLoad_Data , 0 , sizeof(AlternateLoad_Data));
    	AlternateLoad__SetAlternateLoadSearchHome(TRUE);
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Reserve memory for this load instance.
 * @return The index of the allocated load.
 */
uint8 AlternateLoad__Allocate(void)
{
    uint8 retval;
    retval = 0xFF;
    if (AlternateLoad_Allocation < (uint8)HBL_LOAD_NUM_ALTERNATE)
    {
        retval = AlternateLoad_Allocation;
        AlternateLoad_Allocation++;

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
PASS_FAIL_TYPE AlternateLoad__InitializeInstance(HBL_LOAD_TYPE * load)
{
    ALTERNATELOAD_DATA_TYPE       *data;
    ALTERNATELOAD_PARAMETERS_TYPE *parameters;
    PASS_FAIL_TYPE retval;

    retval = FAIL;

    if (load->Load_Data_Index < AlternateLoad_Allocation)
    {
        parameters = (ALTERNATELOAD_PARAMETERS_TYPE*)(void*)load->Load_Parameter_Ptr.Data;
        Diverter_Rotation_Time = parameters->Rotation_Time;
        Diverter_Cam_Number = parameters->Motor_Cam_Number;

        data = &AlternateLoad_Data[load->Load_Data_Index];

        data->Flags.Bit.Alt_Engine_Is_Present = TRUE;

        // Requested for search home position at Cold Reset
        //data->Alt_Engine_Pos_Req = parameters->Motor_Cam_Number;     //Motor_Cam_Number=6
        //data->Alt_Status.Load.Cur_Reg =parameters->Motor_Cam_Number;
        //data->Alt_Status.Load.Status = HBL_LOAD_STATUS_INITIALIZE;

        data->Alt_Status.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
        data->Flags.Bit.Alt_Analog_Fb_Is_Present = TRUE;
//        data->Flags.Bit.Wait_For_Movement = (ALT_MOV_TYPE)WAIT_FOR_COMMAND;

        retval = PASS;
    }

    return (retval);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This function is called each time a new request is made to control the output hardware.
 *        It Update the requested state of the output controlled by this load instance.
 * @param load  A pointer to the HBL_LOAD_TYPE structure with load and load instance information.
 * @param data  A pointer to memory that contains the new requested load state.
 * @return PASS if the requested load state is successfully saved; otherwise FAIL.
 */
PASS_FAIL_TYPE AlternateLoad__Request(HBL_LOAD_TYPE * load,void * data)
{
	PASS_FAIL_TYPE request;

    if (load->Load_Data_Index < AlternateLoad_Allocation)
    {
    	AlternateLoad_Data[load->Load_Data_Index].Requested = *(ALTERNATELOAD_REQUEST_TYPE *)data;
    	request = (PASS);
    }
    else
    {
    	request = (FAIL);
    }

    return request;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This function is called  when it is time to update the output hardware.
 *        This typically occurs every 100ms near the end of the application thread execution.
 * @param load  A pointer to the HBL_LOAD_TYPE structure with load and load instance information.
 * @return PASS if the controlled outputs are successfully updated; otherwise FAIL.
 */
PASS_FAIL_TYPE AlternateLoad__Process(HBL_LOAD_TYPE * load , HBL_PILOT_HANDLER_HOOK_TYPE * pilot)
{
    if (load->Load_Data_Index < (uint8)HBL_LOAD_NUM_ALTERNATE)
    {
        AlternateLoad_Data[load->Load_Data_Index].Flags.Bit.Alt_Engine_Req_Sync =
                (((AlternateLoad_Data[load->Load_Data_Index].Requested.Alt_Request_Sync) == (uint16)1) ? TRUE : FALSE);

        AlternateLoad_Data[load->Load_Data_Index].Flags.Bit.Failure_Rotate =
                        (((AlternateLoad_Data[load->Load_Data_Index].Requested.Alt_Request_Sync) == (uint16)2) ? TRUE : FALSE);

        if ( (AlternateLoad_Data[load->Load_Data_Index].Requested.Alt_Request_Sync) > (uint8)0)
        {
        	 AlternateLoad_Data[load->Load_Data_Index].Requested.Alt_Request_Sync = 0;
        }

        AlternateLoad_Data[load->Load_Data_Index].Alt_Engine_Pos_Req = AlternateLoad_Data[load->Load_Data_Index].Requested.Alt_Request_Pos;
        /*Alt_Request_Pos-      this is request which coming from self clean
          Alt_Engine_Pos_Req-    In this request is stored which come from self clean
          Alt_Engine_Pos_Req =0 when we pause the self clean cycle*/
        AlternateLoad_Data[load->Load_Data_Index].Processed = AlternateLoad_Data[load->Load_Data_Index].Requested;
    }
    return (PASS);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This function get the requested state of the output controlled by this load instance.
 * @param load A pointer to the HBL_LOAD_TYPE structure with load and load instance information.
 * @return This function return the requested state of the output.
 */
void * AlternateLoad__GetRequested(HBL_LOAD_TYPE * load)
{
	void * requested;

    if (load->Load_Data_Index < AlternateLoad_Allocation)
    {
    	requested = (&AlternateLoad_Data[load->Load_Data_Index].Requested);
    }
    else
    {
    	requested = (NULL);
    }

    return requested;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This function get the state of the output controlled by this load instance.
 * @param load A pointer to the HBL_LOAD_TYPE structure with load and load instance information.
 * @return This function return the the state of the output.
 */
void * AlternateLoad__GetProcessed(HBL_LOAD_TYPE * load , HBL_PILOT_HANDLER_HOOK_TYPE * pilot)
{
	void * processed;

    if (load->Load_Data_Index < AlternateLoad_Allocation)
    {
    	processed = (&AlternateLoad_Data[load->Load_Data_Index].Processed);
    }
    else
    {
    	processed = (NULL);
    }

    return processed;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This function handle the behaviors that must be implemented periodically by this load instance.
 *        This function is called every 5ms.
 * @param load A pointer to the HBL_LOAD_TYPE structure with load and load instance information.
 * @return PASS if this function handle the behaviors successfully; otherwise FAIL.
 */
PASS_FAIL_TYPE AlternateLoad__AsyncProcess(HBL_LOAD_TYPE * load , HBL_PILOT_HANDLER_HOOK_TYPE * pilot)
{
    PASS_FAIL_TYPE                  retval;

    retval = FAIL;

    if (load->Load_Data_Index < AlternateLoad_Allocation)
    {
        AlternateLoadEngineDrive(load , pilot);

        retval = PASS;
    }

    return (retval);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This function get the status of this load instance.
 * @param load A pointer to the HBL_LOAD_TYPE structure with load and load instance information.
 * @return This function return the status of the load.
 */
void * AlternateLoad__GetStatus(HBL_LOAD_TYPE * load , HBL_PILOT_HANDLER_HOOK_TYPE * pilot)
{
	void * status;

    if (load->Load_Data_Index < AlternateLoad_Allocation)
    {
    	status = ((void *) &AlternateLoad_Data[load->Load_Data_Index].Alt_Status);
    }
    else
    {
    	status = (NULL);
    }

    return status;
}

BOOL_TYPE AlternateLoad__GetAlternateLoadSearchHome(void)
{
	return AlternateLoad_Search_Home;
}

void AlternateLoad__SetAlternateLoadSearchHome(BOOL_TYPE value)
{
	AlternateLoad_Search_Home = value;
}
BOOL_TYPE AlternateLoad__GetRunAfterFaultFlag(void)
{
	return Run_After_Fault_Flag;
}

uint8 AlternateLoad__GetRotationTime(void)
{
    return Diverter_Rotation_Time;
}

uint8 AlternateLoad__GetCamNumber(void)
{
    return Diverter_Cam_Number;
}

BOOL_TYPE AlternateLoad__GetDiverterRotateAlwaysFlag(void)
{
    return Diverter_Rotate_Always;
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This function check for the Load Driving from the Reference position;or it is ongoing with the ref of current position.
 *        It check for the Consistency value ( to summarize if a load is working in a consistent way or not).
 *        It also check the valid position is reached or not & count the number of attempts to get the valid position.
 * @param load A pointer to the HBL_LOAD_TYPE structure with load and load instance information.
 *        Pilot
 * @return This function returns PASS it is of type uint8 if position is reached successfully;Otherwise FAIL.
 */
static uint8 AlternateLoadManager(HBL_LOAD_TYPE * load , HBL_PILOT_HANDLER_HOOK_TYPE * pilot)
{
    ALTERNATELOAD_PARAMETERS_TYPE  *parameters;
    ALTERNATELOAD_DATA_TYPE        *data;
    uint8                           pos_reached;
    uint8                           pilot_value;
    HBL_CONSISTENCY_TYPE            fbk_val;

    pos_reached = (uint8)FALSE;
    //parameters = (ALTERNATELOAD_PARAMETERS_TYPE*) &Alternate_ParamData;
    parameters = (ALTERNATELOAD_PARAMETERS_TYPE*)(void*)load->Load_Parameter_Ptr.Data;
    data = &AlternateLoad_Data[load->Load_Data_Index];
    fbk_val.Byte = Hbl__GetOwnLoadConsistency(load, data->Alt_Engine_Pos_Req);

    if( data->Triac_Off_Count != (uint8)0 )
    {
        data->Triac_Off_Count--;
        pilot_value = INACTIVE;
    }
    else
    {
        pilot_value = ACTIVE;

        if(((ALTERNATELOAD_COMMAND_TYPE)data->Command == (ALTERNATELOAD_COMMAND_TYPE)ALTERNATELOAD_DRIVE_TO_REF ) &&
            ( fbk_val.Consistency.Value != (HBL_CONSISTENCY_VALUE_ENUM_TYPE)HBL_CONSISTENCY_INVALID_VALUE) &&
            ( (HBL_CONSISTENCY_VALUE_ENUM_TYPE)data->Old_Feedback_Value != fbk_val.Consistency.Value))
        {
            data->Num_Attempt = parameters->Max_Attempts_Absolute;//It takes the 12 attempts for getting ref position
        }

        if( fbk_val.Consistency.Value == HIGH_CAM )
        {
        	/*High_Cam_Timeout means max high time period of other position */
            data->Time_High_Cam++;
            if( data->Time_High_Cam >= parameters->High_Cam_Timeout )
            {
                data->Time_High_Cam = 0;

                data->Triac_Off_Count = parameters->Time_Off_Triac;

                data->Num_Attempt--;

                if( data->Num_Attempt == (uint8)0 )
                {
                	pos_reached = (uint8)TRUE;
					AlternateLoad__SetAlternateLoadSearchHome(TRUE);
					// Declare fault FAULT_ID_DIVERTER_CANNOT_FIND_POSITIONS if happen in run state
					if (Selector__GetCycleState(0, 0, (Interpreter__GetCurrentStatement(0)).Cycle) == CYCLE_STATE_RUN)
					{
						Fault__ConditionDetected(FAULT_ID_DIVERTER_CANNOT_FIND_POSITIONS);
					}
                }
            }


            if ((( (uint16)data->Flags.Bit.Wait_For_Movement == (uint16)MOVEMENT_ONGOING ) && ((uint8)parameters->Stop_Next_Cam == (uint8)FALSE)) ||
            	(( (uint16)data->Flags.Bit.Wait_For_Movement == (uint16)MOVEMENT_ONGOING ) && ( data->Time_High_Cam >= parameters->High_Cam_Threshold) && ((uint8)parameters->Stop_Next_Cam == (uint8)TRUE)))
            {
                /*High_Cam_Threshold means max high time period of initial condition.
                 This is for just checking the initial condition of high cam*/

                if( (ALTERNATELOAD_COMMAND_TYPE)data->Command == (ALTERNATELOAD_COMMAND_TYPE)ALTERNATELOAD_DRIVE_TO_REF )
                {
                    /*if( (data->Num_Cams == 0) && (data->Flags.Bit.Cam_Increase == 1) )
                    {
                        data->Num_Cams++;
                    }
                    else*/
                    {
                        if( data->Time_Low_Cam <= (uint16) (parameters->Long_Low_Cam_Threshold) )
                        {
                            /*Long_Low_Cam_Threshold means max low time period of initial condition.
                             This is for just checking the initial condition of low cam*/

                            if((uint16)data->Flags.Bit.Cam_Increase == (uint16)TRUE)
                            {
                                data->Num_Cams++;
                            }
                            if( data->Num_Cams > parameters->Max_Cam_Number)
                            {
                                data->Flags.Bit.Wait_For_Movement = POS_REACHED;
                            }
                        }
                        else
                        {
                            data->Flags.Bit.Wait_For_Movement = POS_REACHED;
                        }
                    }
                }
                else if ((uint8)parameters->Stop_Next_Cam == (uint8)TRUE)
				{
					data->Flags.Bit.Wait_For_Movement = POS_REACHED;
				}

                data->Time_Low_Cam = 0;
                data->Flags.Bit.Cam_Increase = FALSE;
            }

            if ( ((uint16)data->Flags.Bit.Wait_For_Movement == (uint16)POS_REACHED ) &&
                 (data->Time_High_Cam >= parameters->High_Cam_Threshold) && ((uint8)parameters->Stop_Next_Cam == (uint8)FALSE))
			{
				pos_reached = (uint8)TRUE;
				data->Flags.Bit.Wait_For_Movement = WAIT_FOR_COMMAND;
			}

            if ( ((uint16)data->Flags.Bit.Wait_For_Movement == (uint16)POS_REACHED ) &&
				 (data->Time_High_Cam >= parameters->High_Cam_Threshold) && ((uint8)parameters->Self_Switch == (uint8)TRUE))
			{
            	pilot_value = INACTIVE;
			}
        }
        else if(fbk_val.Consistency.Value == LOW_CAM )
        {
        	data->Flags.Bit.Cam_Increase = TRUE;

            if (( (uint16)data->Flags.Bit.Wait_For_Movement == (uint16)POS_REACHED ) && ((uint8)parameters->Stop_Next_Cam == (uint8)TRUE))
			{
			    pos_reached = (uint8)TRUE;
			    data->Flags.Bit.Wait_For_Movement = WAIT_FOR_COMMAND;
			}

            data->Time_Low_Cam++;

            if( data->Time_Low_Cam >= (uint16)(parameters->Low_Cam_Timeout) )
            {
                /*Low_Cam_Timeout means max low time period of other position  */

                /*If Time_Low_Cam is greater than Low_Cam_Timeout means its not getting high pulse it stuck to the low cam period*/

                data->Time_Low_Cam = 0;

                data->Triac_Off_Count = parameters->Time_Off_Triac;

                data->Num_Attempt--;
                if( data->Num_Attempt == (uint8)0 )
                {
                	pos_reached = (uint8)TRUE;
					AlternateLoad__SetAlternateLoadSearchHome(TRUE);
					// Declare fault FAULT_ID_DIVERTER_CANNOT_FIND_POSITIONS if happen in run state
					if (Selector__GetCycleState(0, 0, (Interpreter__GetCurrentStatement(0)).Cycle) == CYCLE_STATE_RUN)
					{
						Fault__ConditionDetected(FAULT_ID_DIVERTER_CANNOT_FIND_POSITIONS);
					}
                }
            }

            if(( (uint16)data->Flags.Bit.Wait_For_Movement == (uint16)MOVEMENT_ONGOING ) &&
			   ( (ALTERNATELOAD_COMMAND_TYPE)data->Command == (ALTERNATELOAD_COMMAND_TYPE)ALTERNATELOAD_DRIVE_INC ) && ((uint8)parameters->Stop_Next_Cam == (uint8)FALSE))
			{
				if ( data->Time_Low_Cam > (uint16)80) /* 400ms at list */
				{
					data->Flags.Bit.Wait_For_Movement = POS_REACHED;
				}
			}

            data->Time_High_Cam = 0;
        }

        data->Old_Feedback_Value = (uint8)fbk_val.Consistency.Value;
    }

    if (pos_reached == (uint8)TRUE)
    {
    	pilot_value = INACTIVE;
    }

    if (pilot->Request != NULL)
    {
    	(void)pilot->Request(load , &pilot_value);
    }

    if (pilot->Process != NULL)
    {
    	(void)pilot->Process(load);
    }

    Diverter_Motor_Status = (BOOL_TYPE)pilot_value;

    return (pos_reached);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This function check for the reset condition of the load and it update the valve position as per the request come from the Application.
 * @param load A pointer to the HBL_LOAD_TYPE structure with load and load instance information.
 * @return
 */
static void AlternateLoadEngineDrive(HBL_LOAD_TYPE * load , HBL_PILOT_HANDLER_HOOK_TYPE * pilot)
{
    ALTERNATELOAD_PARAMETERS_TYPE *parameters;
    ALTERNATELOAD_DATA_TYPE        *data;
    MASTER_LOAD_TYPE value_state;
    CYCLE_STATE_TYPE label;

    data = &AlternateLoad_Data[load->Load_Data_Index];
    //parameters = (ALTERNATELOAD_PARAMETERS_TYPE*) &Alternate_ParamData;
    parameters = (ALTERNATELOAD_PARAMETERS_TYPE*)(void*)load->Load_Parameter_Ptr.Data;
    Run_After_Fault_Flag = (BOOL_TYPE)parameters->Run_After_Fault;
    Diverter_Rotate_Always = FALSE;

    if ((uint16)data->Flags.Bit.Alt_Engine_Is_Present == (uint16)FALSE)
    {
        // Nothing to do
    }
    else
    {
		if (((uint16)data->Flags.Bit.Failure_Rotate == (uint16)TRUE)||
			((uint8)Fault__IsInFaultState(FAULT_ID_DIVERTER_CANNOT_FIND_POSITIONS) == (uint8)TRUE)||
			((uint8)Fault__IsInFaultState(FAULT_ID_DIVERTER_LEAK) == (uint8)TRUE))
		{
			uint8 pilot_value = INACTIVE;

			if ((uint16)data->Flags.Bit.Failure_Rotate == (uint16)TRUE)
			{
				pilot_value = ACTIVE;

				Diverter_Rotate_Always = TRUE;
				label = Selector__GetCycleState(0, 0, Interpreter__GetCurrentStatement(0).Cycle);

				if ((label == CYCLE_STATE_RUN) || (Mode__GetSubMode() == SUBMODE_NORMAL_EXT_CONTROL_ENGAGED))
				{
					value_state = LOAD_STATE_ON;
					(void)Hbl__SetLoadRequestByLoadID(HBL_LOAD_PILOT_RELAY, HBL_PRIORITY_DIVERTER, (uint8 *) &value_state);
					(void)Hbl__SetLoadRequestByLoadID(HBL_LOAD_RELAY_ENABLE, HBL_PRIORITY_DIVERTER, (uint8 *) &value_state);
				}
			}

			(void)pilot->Request(load , &pilot_value);
			(void)pilot->Process(load);

			Diverter_Motor_Status = (BOOL_TYPE)pilot_value;

			AlternateLoad__SetAlternateLoadSearchHome(TRUE);
			data->Alt_Status.Load.Cur_Reg = data->Alt_Engine_Pos_Req;
			data->Flags.Bit.Alt_Engine_Req_Sync = FALSE;
		}
		else
		{

			if ((uint16)data->Flags.Bit.Alt_Engine_Req_Sync == (uint16)TRUE)
			{
				AlternateLoadSetCommand(load , ALTERNATELOAD_DRIVE_TO_REF);
				data->Flags.Bit.Wait_For_Sync_Pos = TRUE;
				data->Alt_Status.Load.Cur_Reg =parameters->Motor_Cam_Number;
				data->Flags.Bit.Alt_Engine_Req_Sync = FALSE;
			}

			if ((uint16)data->Flags.Bit.Wait_For_Sync_Pos == (uint16)TRUE)
			{
				if (AlternateLoadManager(load , pilot) == (uint8)TRUE)
				{
					data->Alt_Status.Load.Cur_Reg = 0;
					data->Alt_Status.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
					data->Flags.Bit.Init_Inc_Pos = FALSE;
					data->Flags.Bit.Wait_For_Sync_Pos = FALSE;
					AlternateLoad__SetAlternateLoadSearchHome(FALSE);
				}
				else
				{
					label = Selector__GetCycleState(0, 0, Interpreter__GetCurrentStatement(0).Cycle);

					if ((label == CYCLE_STATE_RUN) || (Mode__GetSubMode() == SUBMODE_NORMAL_EXT_CONTROL_ENGAGED))
					{
						value_state = LOAD_STATE_ON;
						(void)Hbl__SetLoadRequestByLoadID(HBL_LOAD_PILOT_RELAY, HBL_PRIORITY_DIVERTER, (uint8 *) &value_state);
						(void)Hbl__SetLoadRequestByLoadID(HBL_LOAD_RELAY_ENABLE, HBL_PRIORITY_DIVERTER, (uint8 *) &value_state);
						data->Alt_Status.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
					}
					else
					{
						data->Alt_Status.Load.Cur_Reg = 0;
						data->Alt_Status.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
						data->Flags.Bit.Init_Inc_Pos = FALSE;
						data->Flags.Bit.Wait_For_Sync_Pos = FALSE;
						AlternateLoad__SetAlternateLoadSearchHome(TRUE);
					}

				}

			}
			else
			{
				if (  ((data->Alt_Engine_Pos_Req != data->Alt_Status.Load.Cur_Reg ) ||
					  ((data->Alt_Engine_Pos_Req == (uint8)data->Alt_Status.Load.Cur_Reg) &&
					  ((HBL_LOAD_STATUS_ENUM_TYPE)data->Alt_Status.Load.Status == (HBL_LOAD_STATUS_ENUM_TYPE)HBL_LOAD_STATUS_WAITING_TARGET)) ) &&
					  ( data->Alt_Engine_Pos_Req < parameters->Motor_Cam_Number ))
				{
					if ((data->Alt_Status.Load.Cur_Reg >= parameters->Motor_Cam_Number) ||
						(data->Alt_Status.Load.Cur_Reg > data->Alt_Engine_Pos_Req))
					{
						data->Flags.Bit.Alt_Engine_Req_Sync = TRUE;

						uint8 pilot_value = INACTIVE;
						(void)pilot->Request(load , &pilot_value);
						(void)pilot->Process(load);
						Diverter_Motor_Status = (BOOL_TYPE)pilot_value;
					}
					else
					{
						if ( (uint16)data->Flags.Bit.Init_Inc_Pos == (uint16)FALSE )
						{
							AlternateLoadSetCommand(load , ALTERNATELOAD_DRIVE_INC);
							data->Flags.Bit.Init_Inc_Pos = TRUE;

							uint8 pilot_value = INACTIVE;
							(void)pilot->Request(load , &pilot_value);
							(void)pilot->Process(load);
							Diverter_Motor_Status = (BOOL_TYPE)pilot_value;
						}
						else
						{
							if ( AlternateLoadManager(load , pilot) == (uint8)TRUE )
							{
								data->Alt_Status.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
								if( data->Alt_Status.Load.Cur_Reg < (parameters->Motor_Cam_Number - (uint8)1) )
								{
									data->Alt_Status.Load.Cur_Reg ++;
								}
								else
								{
									data->Alt_Status.Load.Cur_Reg = 0;
								}
								data->Flags.Bit.Init_Inc_Pos = FALSE;
							}
							else
							{
								label = Selector__GetCycleState(0, 0, Interpreter__GetCurrentStatement(0).Cycle);

								if ((label == CYCLE_STATE_RUN) || (Mode__GetSubMode() == SUBMODE_NORMAL_EXT_CONTROL_ENGAGED))
								{
									data->Alt_Status.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
								}
								else
								{
									data->Alt_Status.Load.Cur_Reg = data->Alt_Engine_Pos_Req;
									data->Alt_Status.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
									data->Flags.Bit.Init_Inc_Pos = FALSE;
									data->Flags.Bit.Wait_For_Sync_Pos = FALSE;
									AlternateLoad__SetAlternateLoadSearchHome(TRUE);
								}
							}
						}
					}
				}
				else
				{
					uint8 pilot_value = INACTIVE;
					(void)pilot->Request(load , &pilot_value);
					(void)pilot->Process(load);
					Diverter_Motor_Status = (BOOL_TYPE)pilot_value;
				}
			}
		}
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param load A pointer to the HBL_LOAD_TYPE structure with load and load instance information.
 *        command Position from where to start the alternate load module.
 * @return This function returns the number of attempts require to come back to the same position with respect to its current position.
 */
static void AlternateLoadSetCommand(HBL_LOAD_TYPE * load , uint8 command)
{
    ALTERNATELOAD_DATA_TYPE        *data;
    ALTERNATELOAD_PARAMETERS_TYPE  *parameters;

    data = &AlternateLoad_Data[load->Load_Data_Index];
    //parameters = (ALTERNATELOAD_PARAMETERS_TYPE*) &Alternate_ParamData;
    parameters = (ALTERNATELOAD_PARAMETERS_TYPE*)(void*)load->Load_Parameter_Ptr.Data;

    switch (command)
    {
        case ALTERNATELOAD_DRIVE_INC:       /* Target position: Next */

            if( (uint16)data->Flags.Bit.Alt_Analog_Fb_Is_Present == (uint16)TRUE)
            {
                /* Target position: Next : If rotation starts from any position then it come back to same position after one rotation.for that it takes 6 attempts */

                data->Num_Attempt = parameters->Max_Attempts_Relative;
            }

            break;

        case ALTERNATELOAD_DRIVE_TO_REF:    /* Target position: Ref */

            if( (uint16)data->Flags.Bit.Alt_Analog_Fb_Is_Present == (uint16)TRUE)
            {
                /* Target position: Ref: If cam starts after reset  from initial condition then it takes two rotation & stuck to the initial condition.
                 For that it takes 12 attempts to come back to initial position after two rotation*/

                data->Num_Attempt = parameters->Max_Attempts_Absolute;
            }

            break;

        default:
        	// nothing to do
            break;
    }

	data->Time_Low_Cam = 0;
	data->Time_High_Cam = 0;
	data->Flags.Bit.Wait_For_Movement = MOVEMENT_ONGOING;
	data->Flags.Bit.Cam_Increase = FALSE;
	data->Num_Cams = 0;
	data->Command = command;
	data->Old_Feedback_Value = FB_INIT_VAL;
}

#endif

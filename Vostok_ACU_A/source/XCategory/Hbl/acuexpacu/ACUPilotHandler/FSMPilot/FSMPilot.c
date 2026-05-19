/**
 *  @file
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>
 *
 *  $Header: 	FSMPilot.c 27/04/2016 KAVEH RAZZAGHI (RAZZAK) Exp  $
 *
 *  @copyright  Copyright 2016.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "FSMPilot.h"

#if (HBL_PILOT_NUM_FSM > 0)
#include "VirtualPin.h"
#include "String.h"
#include "Gpio.h"
#include "SystemTimers.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
// -- Private Constant Declarations --
#define FSMPILOT_STATEMACHINE_TIMING_INTERVAL	10	//Interval in MS

//! State machine values
typedef enum
{
    FSMPILOT_SM_IDLE = 0,
    FSMPILOT_SM_INIT,
    FSMPILOT_SM_SET_POINT_MONITORING,
    FSMPILOT_SM_T3,
    FSMPILOT_SM_T2
} FSMPILOT_STATE_MACHINE_VAL_TYPE;

//! Speed sign
typedef sint8 FSMPILOT_SPEED_SIGN_TYPE;

//! Target velocity
typedef sint8 FSMPILOT_VELOCITY_TYPE;

//! T2
typedef uint8 FSMPILOT_T2_TYPE;

//! T3
typedef uint16 FSMPILOT_T3_TYPE;

//! State machine manager
PACKED typedef struct
{
    FSMPILOT_VELOCITY_TYPE   		Processed; 				 		//!< Processed Speed value
    FSMPILOT_VELOCITY_TYPE   		Requested; 				  		//!< Requested Speed value
    FSMPILOT_VELOCITY_TYPE   		Current_Velocity; 		  		//!< Current Speed value
    FSMPILOT_T3_TYPE                T3_Cnt;                   		//!< T3 counter
    FSMPILOT_T2_TYPE                T2_Cnt;                   		//!< T2 counter
    uint8							Sense_Of_Rotation_Pin_Value;	//!< Target value for sor pin
    uint8							Power_Enable_Pin_1_Value;		//!< Target value for power enable pin 1
    uint8							Power_Enable_Pin_2_Value;		//!< Target value for power enable pin 2
    uint8							Monitoring_Flag;		  		//!< Indicating if the state machine is in monitoring state (0)
    FSMPILOT_STATE_MACHINE_VAL_TYPE State;      			  		//!< State machine value
} FSMPILOT_DATA_TYPE;

//! Struct to cast the parameters for the Fixed Speed Motor.
PACKED typedef struct
{
	uint8 T2_Val;
	uint8 T3_Val;
} FSMPILOT_PARAMETERS_TYPE;

//! Struct to cast the io pilot pins for the Fixed Speed Motor.
PACKED typedef struct
{
	uint8 Sense_Of_Rotation_Pin;
	uint8 Power_Enable_Pin_1;
	uint8 Power_Enable_Pin_2;
} FSMPILOT_PIN_MAP_TYPE;

// -- Private Variable Definitions --

static FSMPILOT_DATA_TYPE FSMPilot_Data[HBL_PILOT_NUM_FSM];
static uint8 FSMPilot_Allocation;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void FSMPilotStateManager(HBL_LOAD_TYPE *load);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

/**
 * @brief
 * @return
 */
void FSMPilot__Initialize(void)
{
    FSMPilot_Allocation = 0;
    memset (FSMPilot_Data , 0 , sizeof(FSMPilot_Data));

}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @return
 */
uint8 FSMPilot__Allocate(void)
{
    uint8 retval;
    retval = 0xFF;
    if (FSMPilot_Allocation < HBL_PILOT_NUM_FSM)
    {
        retval = FSMPilot_Allocation;
        FSMPilot_Allocation++;

    }
    return (retval);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param load
 */
PASS_FAIL_TYPE FSMPilot__InitializeInstance(HBL_LOAD_TYPE * load)
{
	FSMPILOT_DATA_TYPE *data;
	PASS_FAIL_TYPE retval;
    uint8 virtual_pin;
    uint8 loop_index;
    retval = FAIL;

    if (load->Pilot_Data_Index < FSMPilot_Allocation)
    {
        data = &FSMPilot_Data[load->Pilot_Data_Index];
        data->State = FSMPILOT_SM_INIT; // set state machine val

        for (loop_index = 0; loop_index < 3; loop_index++)
        {
        	virtual_pin = load->Pin_Ptr[loop_index];

            if (VIRTUALPIN_TABLE[virtual_pin].port != 0xFF)
            {
                Gpio__PinConfig(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin, VIRTUALPIN_TABLE[virtual_pin].pin_config);
            }
        }

        retval = PASS;
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
PASS_FAIL_TYPE FSMPilot__Request(HBL_LOAD_TYPE * load,void * data)
{
    if (load->Pilot_Data_Index < FSMPilot_Allocation)
    {
    	FSMPilot_Data[load->Pilot_Data_Index].Requested = *(FSMPILOT_VELOCITY_TYPE *)data;
        return (PASS);
    }
    else
    {
    	return (FAIL);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param load
 * @return
 */
PASS_FAIL_TYPE FSMPilot__Process(HBL_LOAD_TYPE * load)
{
    if (load->Pilot_Data_Index < HBL_PILOT_NUM_FSM)
    {
    	FSMPilot_Data[load->Pilot_Data_Index].Processed = FSMPilot_Data[load->Pilot_Data_Index].Requested;
    }
    return (PASS);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param load
 * @return
 */
void * FSMPilot__GetRequested(HBL_LOAD_TYPE * load)
{
    if (load->Pilot_Data_Index < FSMPilot_Allocation)
    {
        return (&FSMPilot_Data[load->Pilot_Data_Index].Requested);
    }
    else
    {
        return (NULL);
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param load
 * @return
 */
void * FSMPilot__GetProcessed(HBL_LOAD_TYPE * load)
{
    if (load->Pilot_Data_Index < FSMPilot_Allocation)
    {
        return (&FSMPilot_Data[load->Pilot_Data_Index].Processed);
    }
    else
    {
        return (NULL);
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param load
 * @return
 */
PASS_FAIL_TYPE FSMPilot__AsyncProcess(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval;
    retval = FAIL;

    if (load->Pilot_Data_Index < FSMPilot_Allocation)
    {
    	FSMPilotStateManager(load);
    	retval = PASS;
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param load
 * @return
 */
void * FSMPilot__GetStatus(HBL_LOAD_TYPE * load)
{
    if (load->Pilot_Data_Index < FSMPilot_Allocation)
    {
        return (&FSMPilot_Data[load->Pilot_Data_Index].Monitoring_Flag);
    }
    else
    {
        return (NULL);
    }
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       FSM Module manager: processes the command (set point) and executes the state machine to drive the motor.
 * @details     attention: It is assumed that it's called every 5ms.
                attention: Before calling this method, FSMPilot__InitializeInstance() must be executed.
                attention: So that the state machine is properly executed.
                note: state machine implements a protection against the inversion of the sense of rotation.

 * @param       load
 * @return 	    none
*/
static void FSMPilotStateManager(HBL_LOAD_TYPE *load)
{
    FSMPILOT_SPEED_SIGN_TYPE sign;
    FSMPILOT_PARAMETERS_TYPE *parameters_pointer;
    FSMPILOT_PIN_MAP_TYPE *pin_map_pointer;
    parameters_pointer = (FSMPILOT_PARAMETERS_TYPE*)load->Parameter_Ptr.Data;
    pin_map_pointer = (FSMPILOT_PIN_MAP_TYPE*)load->Pin_Ptr;
	switch(FSMPilot_Data[load->Pilot_Data_Index].State)
	{
		case FSMPILOT_SM_IDLE:
			break;

		case FSMPILOT_SM_INIT:
			FSMPilot_Data[load-> Pilot_Data_Index].State = FSMPILOT_SM_SET_POINT_MONITORING;
			// LS3 OFF
			FSMPilot_Data[load->Pilot_Data_Index].Sense_Of_Rotation_Pin_Value = INACTIVE;
			FSMPilot_Data[load->Pilot_Data_Index].Power_Enable_Pin_1_Value 	  = INACTIVE;
			FSMPilot_Data[load->Pilot_Data_Index].Power_Enable_Pin_2_Value 	  = INACTIVE;
			break;

		case FSMPILOT_SM_SET_POINT_MONITORING:
			sign = FSMPILOT_VELOCITY_ZERO;

			if(FSMPilot_Data[load->Pilot_Data_Index].Processed > 0)
			{
				sign = FSMPILOT_VELOCITY_POSITIVE;
			}

			if(FSMPilot_Data[load->Pilot_Data_Index].Processed < 0)
			{
				sign = FSMPILOT_VELOCITY_NEGATIVE;
			}

			if(FSMPilot_Data[load->Pilot_Data_Index].Current_Velocity != sign)
			{
				FSMPilot_Data[load->Pilot_Data_Index].Monitoring_Flag = TRUE;
				FSMPilot_Data[load->Pilot_Data_Index].Current_Velocity = sign;

				FSMPilot_Data[load->Pilot_Data_Index].State = FSMPILOT_SM_T3;

				// Timing variable initialization
				FSMPilot_Data[load->Pilot_Data_Index].T3_Cnt = (parameters_pointer->T3_Val) * 10 * 2;
				FSMPilot_Data[load->Pilot_Data_Index].T2_Cnt = (parameters_pointer->T2_Val) * 2;

				// LS3 (power supply) OFF
				FSMPilot_Data[load->Pilot_Data_Index].Power_Enable_Pin_1_Value = INACTIVE;
				FSMPilot_Data[load->Pilot_Data_Index].Power_Enable_Pin_2_Value = INACTIVE;
			}
			else
			{
				FSMPilot_Data[load->Pilot_Data_Index].Monitoring_Flag = FALSE;
			}
			break;

		case FSMPILOT_SM_T3:
			if(!FSMPilot_Data[load->Pilot_Data_Index].T3_Cnt)
			{
				// SET LS1
				if(FSMPilot_Data[load->Pilot_Data_Index].Current_Velocity > 0)
				{
					FSMPilot_Data[load->Pilot_Data_Index].Sense_Of_Rotation_Pin_Value = ACTIVE;
				}
				else
				{
					FSMPilot_Data[load->Pilot_Data_Index].Sense_Of_Rotation_Pin_Value = INACTIVE;
				}

				if(FSMPilot_Data[load->Pilot_Data_Index].Current_Velocity)
				{
					FSMPilot_Data[load->Pilot_Data_Index].State = FSMPILOT_SM_T2;
				}
				else
				{
					FSMPilot_Data[load->Pilot_Data_Index].State = FSMPILOT_SM_SET_POINT_MONITORING;
				}
			}
			else
			{
				FSMPilot_Data[load->Pilot_Data_Index].T3_Cnt--;;
			}
			break;

		case FSMPILOT_SM_T2:

			if(!FSMPilot_Data[load->Pilot_Data_Index].T2_Cnt)
			{
				// LS3 ON
				FSMPilot_Data[load->Pilot_Data_Index].Power_Enable_Pin_1_Value 	  = ACTIVE;
				FSMPilot_Data[load->Pilot_Data_Index].Power_Enable_Pin_2_Value 	  = ACTIVE;

				FSMPilot_Data[load->Pilot_Data_Index].State = FSMPILOT_SM_SET_POINT_MONITORING;
			}
			else
			{
				FSMPilot_Data[load->Pilot_Data_Index].T2_Cnt--;
			}
			break;

		default:
			FSMPilot_Data[load->Pilot_Data_Index].State = FSMPILOT_SM_SET_POINT_MONITORING;
			break;
	}

	Gpio__PinWrite(  VIRTUALPIN_TABLE[pin_map_pointer->Sense_Of_Rotation_Pin].port,
					 VIRTUALPIN_TABLE[pin_map_pointer->Sense_Of_Rotation_Pin].pin,
					 (BOOL_TYPE)((FSMPilot_Data[load->Pilot_Data_Index].Sense_Of_Rotation_Pin_Value
					 ^ ((~load->Hi_Low_Activation_Bitmap) >> 0)) & 0x01));

	Gpio__PinWrite(  VIRTUALPIN_TABLE[pin_map_pointer->Power_Enable_Pin_1].port,
					 VIRTUALPIN_TABLE[pin_map_pointer->Power_Enable_Pin_1].pin,
					 (BOOL_TYPE)((FSMPilot_Data[load->Pilot_Data_Index].Power_Enable_Pin_1_Value
					 ^ ((~load->Hi_Low_Activation_Bitmap) >> 1)) & 0x01));

	Gpio__PinWrite(  VIRTUALPIN_TABLE[pin_map_pointer->Power_Enable_Pin_2].port,
					 VIRTUALPIN_TABLE[pin_map_pointer->Power_Enable_Pin_2].pin,
					 (BOOL_TYPE)((FSMPilot_Data[load->Pilot_Data_Index].Power_Enable_Pin_2_Value
					 ^ ((~load->Hi_Low_Activation_Bitmap) >> 2)) & 0x01));
    return;
}

#endif

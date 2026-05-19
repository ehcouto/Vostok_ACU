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
 *  $Header: ZCRelayPilot.c 1.9 2015/07/14 17:37:54EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/14 17:37:54EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "ZCRelayPilot.h"

#if (HBL_PILOT_NUM_ZCRELAY > 0)
#include "VirtualPin.h"
#include "Gpio.h"
#include "String.h"
#include "ZeroCross.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

typedef enum
{
    RELAY_OPENED_STATE,
    RELAY_CLOSING_STATE,
    RELAY_CLOSED_STATE,
    RELAY_OPENING_STATE
}RELAY_WALKING_CYCLE_TYPE;

typedef struct
{
    uint8 Requested;
    uint8 Processed;
    uint16 Triggering_Angle;
    RELAY_WALKING_CYCLE_TYPE State;
} ZCRELAYPILOT_DATA_TYPE;

static ZCRELAYPILOT_DATA_TYPE ZCRelayPilot_Data[HBL_PILOT_NUM_ZCRELAY];

static uint8 ZCRelayPilot_Allocation;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
void ZCRelayPilot__Initialize(void)
{
    ZCRelayPilot_Allocation         = 0;

    memset(ZCRelayPilot_Data,0,sizeof(ZCRelayPilot_Data));
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @return
 */
uint8 ZCRelayPilot__Allocate(void)
{
    uint8 retval;
    retval = 0xFF;
    if (ZCRelayPilot_Allocation < HBL_PILOT_NUM_ZCRELAY)
    {
        retval = ZCRelayPilot_Allocation;
        ZCRelayPilot_Allocation++;
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param load
 */
PASS_FAIL_TYPE ZCRelayPilot__InitializeInstance(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval;
    uint8 virtual_pin;
    BOOL_TYPE state;
    retval = FAIL;

    if (load->Pilot_Data_Index < ZCRelayPilot_Allocation)
    {
        virtual_pin = load->Pin_Ptr[0];
        if (VIRTUALPIN_TABLE[virtual_pin].port != 0xFF)
        {
        	state = (BOOL_TYPE)(ZCRelayPilot_Data[load->Pilot_Data_Index].Processed & 0x01);

        	if (state == FALSE)
        	{
        		ZCRelayPilot_Data[load->Pilot_Data_Index].State = RELAY_OPENED_STATE;
        	}
        	else
        	{
        		ZCRelayPilot_Data[load->Pilot_Data_Index].State = RELAY_CLOSED_STATE;
        	}

            Gpio__PinWrite(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin, (BOOL_TYPE)(((~load->Hi_Low_Activation_Bitmap) ^ state) & 0x01));
            Gpio__PinConfig(VIRTUALPIN_TABLE[virtual_pin].port,VIRTUALPIN_TABLE[virtual_pin].pin,VIRTUALPIN_TABLE[virtual_pin].pin_config);

            retval = PASS;
        }
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
PASS_FAIL_TYPE ZCRelayPilot__Request(HBL_LOAD_TYPE * load,void * data)
{
    if (load->Pilot_Data_Index < ZCRelayPilot_Allocation)
    {
        ZCRelayPilot_Data[load->Pilot_Data_Index].Requested = *((uint8 *)data);
        return (PASS);
    }
    else
    {
        return (FAIL);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @param load
 * @param data
 * @return
 */
PASS_FAIL_TYPE ZCRelayPilot__Process(HBL_LOAD_TYPE * load)
{
    if (load->Pilot_Data_Index < ZCRelayPilot_Allocation)
    {
    	ZCRelayPilot_Data[load->Pilot_Data_Index].Processed = ZCRelayPilot_Data[load->Pilot_Data_Index].Requested;
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
 * @param data
 * @return
 */
PASS_FAIL_TYPE ZCRelayPilot__AsynchProcess(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval;
    uint8 virtual_pin;
    uint8 requested_state;
    uint16 zc_angle;

    retval = FAIL;
    requested_state = 0;

    if (load->Pilot_Data_Index < ZCRelayPilot_Allocation)
    {
        requested_state = ZCRelayPilot_Data[load->Pilot_Data_Index].Processed;
        virtual_pin = load->Pin_Ptr[0];

        //Read the current zero cross angle
        zc_angle = ZeroCross__GetAngle();

        // If ZeroCross is not present, align zc_angle to make it work
        if (ZeroCross__GetFrequency() == 0)
        {
        	zc_angle = ZCRelayPilot_Data[load->Pilot_Data_Index].Triggering_Angle;
        }

        if (VIRTUALPIN_TABLE[virtual_pin].port != 0xFF)
        {
            switch (ZCRelayPilot_Data[load->Pilot_Data_Index].State)
            {
                case RELAY_OPENED_STATE:
                    if (requested_state != 0)
                    {
                        ZCRelayPilot_Data[load->Pilot_Data_Index].State = RELAY_CLOSING_STATE;
                    }
                    break;

                case RELAY_CLOSING_STATE:
                    if (ZCRelayPilot_Data[load->Pilot_Data_Index].Triggering_Angle == zc_angle)
                    {
                        Gpio__PinWrite(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin, (BOOL_TYPE)((load->Hi_Low_Activation_Bitmap) & 0x01));

                        ZCRelayPilot_Data[load->Pilot_Data_Index].State = RELAY_CLOSED_STATE;
                    }
                    break;

                case RELAY_CLOSED_STATE:
                    if (requested_state == 0)
                    {
                        ZCRelayPilot_Data[load->Pilot_Data_Index].State = RELAY_OPENING_STATE;
                    }
                    break;

                case RELAY_OPENING_STATE:
                    if (ZCRelayPilot_Data[load->Pilot_Data_Index].Triggering_Angle == zc_angle)
                    {
                        Gpio__PinWrite(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin, (BOOL_TYPE)((~load->Hi_Low_Activation_Bitmap) & 0x01));

                        ZCRelayPilot_Data[load->Pilot_Data_Index].State = RELAY_OPENED_STATE;

						// ZCRelayPilot_Data[load->Pilot_Data_Index].Triggering_Angle += ZEROCROSS_ANGLE_RESOLUTION;

						if (ZCRelayPilot_Data[load->Pilot_Data_Index].Triggering_Angle >= ZEROCROSS_MAXIMUM_ANGLE)
						{
							ZCRelayPilot_Data[load->Pilot_Data_Index].Triggering_Angle = ZEROCROSS_MINIMUM_ANGLE;
						}
                    }
                    break;

                default:
                    Gpio__PinWrite(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin, (BOOL_TYPE)((~load->Hi_Low_Activation_Bitmap) & 0x01));
                    ZCRelayPilot_Data[load->Pilot_Data_Index].State = RELAY_OPENED_STATE;
                    break;
            }
            retval = PASS;
        }
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
void * ZCRelayPilot__GetRequested(HBL_LOAD_TYPE * load)
{
    if (load->Pilot_Data_Index < ZCRelayPilot_Allocation)
    {
        return (&ZCRelayPilot_Data[load->Pilot_Data_Index].Requested);
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
 * @param data_out
 * @return
 */
void * ZCRelayPilot__GetProcessed(HBL_LOAD_TYPE * load)
{
    uint8 virtual_pin;
    uint8 * processed;

    processed = NULL;

    if (load->Pilot_Data_Index < ZCRelayPilot_Allocation)
    {
        virtual_pin = load->Pin_Ptr[0];

        if (VIRTUALPIN_TABLE[virtual_pin].port != 0xFF)
        {
            processed = (&ZCRelayPilot_Data[load->Pilot_Data_Index].Processed);
        }
    }

    return(processed);
}
//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


#endif

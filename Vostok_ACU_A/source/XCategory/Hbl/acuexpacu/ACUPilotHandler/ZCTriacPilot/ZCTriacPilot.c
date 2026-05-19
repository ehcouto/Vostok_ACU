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
#include "ZCTriacPilot.h"

#if (HBL_PILOT_NUM_ZCTRIAC > 0)
#include "VirtualPin.h"
#include "Gpio.h"
#include "String.h"
#include "ZeroCross.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

typedef struct
{
	uint16 Gpio_Id;
    uint8 Requested;
    uint8 Processed;
} ZCTRIACPILOT_DATA_TYPE;

typedef enum
{
    TRIAC_BOTH_EDGES = 0,
    TRIAC_RISE_EDGE = 1,
    TRIAC_FALL_EDGE = 2,
}TRIAC_EDGE_TYPE;

typedef struct
{
	TRIAC_EDGE_TYPE Activation_Edge;
	uint8           Activation_Percent;
	uint8           Min_Deactivation_Percent;
	uint8			Min_Activation_Percent;
} ZCTRIACPILOT_PARAM_TYPE;

static ZCTRIACPILOT_DATA_TYPE ZCTriacPilot_Data[HBL_PILOT_NUM_ZCTRIAC];
static ZCTRIACPILOT_PARAM_TYPE * ZCTriacPilot__Params[HBL_PILOT_NUM_ZCTRIAC];
static uint8 ZCTriacPilot_Allocation;


static const ZCTRIACPILOT_PARAM_TYPE ZCTRIACPILOT_DEFAULT_PARAMS =
{
                                                                    TRIAC_BOTH_EDGES,
                                                                    40,
                                                                    15,
                                                                    15
};

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
void ZCTriacPilot__Initialize(void)
{
    ZCTriacPilot_Allocation         = 0;
    memset(ZCTriacPilot_Data,0,sizeof(ZCTriacPilot_Data));
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @return
 */
uint8 ZCTriacPilot__Allocate(void)
{
    uint8 retval;
    retval = 0xFF;
    if (ZCTriacPilot_Allocation < HBL_PILOT_NUM_ZCTRIAC)
    {
        retval = ZCTriacPilot_Allocation;
        ZCTriacPilot_Allocation++;
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param load
 */
PASS_FAIL_TYPE ZCTriacPilot__InitializeInstance(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval;
    retval = FAIL;
    uint8 virtual_pin = load->Pin_Ptr[0];

    if ((load->Pilot_Data_Index < ZCTriacPilot_Allocation) &&
    	(virtual_pin < VirtualPin__GetNumOf()))
    {
		uint8 pin_val = ZCTriacPilot_Data[load->Pilot_Data_Index].Processed;
		// Invert pin value according to Hi Low Activation Bitmap
		pin_val ^= ~load->Hi_Low_Activation_Bitmap;
		pin_val &= 0x01;

        if (VIRTUALPIN_TABLE[virtual_pin].port != 0xFF)
        {
        	uint16 id = ZeroCross__RegisterGpioActivity(VIRTUALPIN_TABLE[virtual_pin].port,VIRTUALPIN_TABLE[virtual_pin].pin);
        	if (id != ZEROCROSS_ID_INVALID)
        	{
        		ZCTriacPilot_Data[load->Pilot_Data_Index].Gpio_Id = id;
            	Gpio__PinWrite(VIRTUALPIN_TABLE[virtual_pin].port,VIRTUALPIN_TABLE[virtual_pin].pin, (BOOL_TYPE) pin_val);
                Gpio__PinConfig(VIRTUALPIN_TABLE[virtual_pin].port,VIRTUALPIN_TABLE[virtual_pin].pin,VIRTUALPIN_TABLE[virtual_pin].pin_config);


                if (load->Parameter_Ptr.Length != 0)
                {
                    ZCTriacPilot__Params[load->Pilot_Data_Index] = (ZCTRIACPILOT_PARAM_TYPE *)load->Parameter_Ptr.Data;
                }
                else //if there's no parameters defined, load the default.
                {
                    ZCTriacPilot__Params[load->Pilot_Data_Index] = (ZCTRIACPILOT_PARAM_TYPE *)&ZCTRIACPILOT_DEFAULT_PARAMS;
                }

                retval = PASS;
        	}
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
PASS_FAIL_TYPE ZCTriacPilot__Request(HBL_LOAD_TYPE * load,void * data)
{
    PASS_FAIL_TYPE retval = FAIL;
    uint8 max_value;

    if (load->Pilot_Data_Index < ZCTriacPilot_Allocation)
    {
    	uint8 new_value = *((uint8 *)data);

    	// Values accepted are from 0 to 100; other values are discarded
        if (new_value <= 100)
        {
        	// Minimum percentage applied is set on setting file.
        	// Except for 0 (OFF) it cannot be lower than Activation time plus a Minimum threshold time
        	// that prevents pin activated during zero cross event

            if(ZCTriacPilot__Params[load->Pilot_Data_Index]->Min_Activation_Percent == 0)
            {
                max_value = ZCTriacPilot__Params[load->Pilot_Data_Index]->Min_Deactivation_Percent+ZCTriacPilot__Params[load->Pilot_Data_Index]->Activation_Percent;
            }
            else
            {
                max_value = ZCTriacPilot__Params[load->Pilot_Data_Index]->Min_Deactivation_Percent+ZCTriacPilot__Params[load->Pilot_Data_Index]->Min_Activation_Percent;
            }

			if ((new_value > 0) &&
				(new_value < max_value))
			{
				new_value = max_value;
			}

            ZCTriacPilot_Data[load->Pilot_Data_Index].Requested = new_value;
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
PASS_FAIL_TYPE ZCTriacPilot__Process(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval = FAIL;
    if (load->Pilot_Data_Index < ZCTriacPilot_Allocation)
    {
    	ZCTriacPilot_Data[load->Pilot_Data_Index].Processed = ZCTriacPilot_Data[load->Pilot_Data_Index].Requested;
        retval = PASS;
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief -
 * @param load
 * @return
 */
PASS_FAIL_TYPE ZCTriacPilot__ZCProcess(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval;
    retval = FAIL;

    if (load->Pilot_Data_Index < ZCTriacPilot_Allocation)
    {
    	uint8 active_pin_value = load->Hi_Low_Activation_Bitmap & 0x01;
        uint8 virtual_pin = load->Pin_Ptr[0];
        ZEROCROSS_GPIO_REQUEST_TYPE activity_request;
    	// Replace with (ZCTRIACPILOT_PARAM_TYPE *) load->Load_Parameter_Ptr.Data;

        activity_request.Gpio_Id = ZCTriacPilot_Data[load->Pilot_Data_Index].Gpio_Id;
        activity_request.Active_value = (BOOL_TYPE) active_pin_value;

        if (ZCTriacPilot_Data[load->Pilot_Data_Index].Processed > 0)
        {
        	uint16 degree = (100 - ZCTriacPilot_Data[load->Pilot_Data_Index].Processed) * ZEROCROSS_HALF_WAVE_ANGLE /100;
        	activity_request.Active_Degree = degree;

        	if (ZCTriacPilot_Data[load->Pilot_Data_Index].Processed <  ZCTriacPilot__Params[load->Pilot_Data_Index]->Activation_Percent + ZCTriacPilot__Params[load->Pilot_Data_Index]->Min_Deactivation_Percent)
        	{
        	    degree = (100 - ZCTriacPilot__Params[load->Pilot_Data_Index]->Min_Deactivation_Percent) * ZEROCROSS_HALF_WAVE_ANGLE /100;
        	}
        	else
        	{
        	    degree = (100 + ZCTriacPilot__Params[load->Pilot_Data_Index]->Activation_Percent - ZCTriacPilot_Data[load->Pilot_Data_Index].Processed) * ZEROCROSS_HALF_WAVE_ANGLE /100;
        	}

        	activity_request.Deactive_Degree = degree;
        	retval = (ZeroCross__RequestGpioActivity(&activity_request) == TRUE) ? PASS : FAIL;
        }
        else
        {
        	// Force pin OFF
        	active_pin_value ^= 0x01;
           	Gpio__PinWrite(VIRTUALPIN_TABLE[virtual_pin].port,VIRTUALPIN_TABLE[virtual_pin].pin, (BOOL_TYPE) active_pin_value);
           	retval = PASS;
        }

    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get Zero Cross sequence current status.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to MultiSequence load instance.
 *
 *  @return     void *status - pointer to status of pilot instance. Cast it using *((COMPLETE_TYPE *)ptr) to get value.
 *  @retval     COMPLETE   - Requested sequence is complete and reached to end
 *  @retval     INCOMPLETE - Requested sequence is In Progress
 */
void * ZCTriacPilot__GetStatus(HBL_LOAD_TYPE *load)
{
    void *status;

    status = NULL;

    if (load->Pilot_Data_Index < ZCTriacPilot_Allocation)
    {
//            status = (void *)(&MultiSequencePilot_Data[load->Pilot_Data_Index].Sequence_End);
    }

    return (status);
}
//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param load
 * @param data_out
 * @return
 */
void * ZCTriacPilot__GetRequested(HBL_LOAD_TYPE * load)
{
    if (load->Pilot_Data_Index < ZCTriacPilot_Allocation)
    {
        return (&ZCTriacPilot_Data[load->Pilot_Data_Index].Requested);
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
void * ZCTriacPilot__GetProcessed(HBL_LOAD_TYPE * load)
{
    uint8 * processed = NULL;

    if (load->Pilot_Data_Index < ZCTriacPilot_Allocation)
    {
    	processed = &ZCTriacPilot_Data[load->Pilot_Data_Index].Processed;
    }

    return (processed);
}
//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


#endif

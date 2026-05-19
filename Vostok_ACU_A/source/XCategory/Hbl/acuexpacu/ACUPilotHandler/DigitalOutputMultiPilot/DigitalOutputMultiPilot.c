/**
 *  @file       DigitalOutputMultiPilot.c
 *
 *  @brief      Pilot Handler for DigitalOutputMulti
 *
 *  @details    It handles 2 pin output configuration only; they are driven together
 *  			according to Processed value and each Pin High flag set.
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/14 17:37:52EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "DigitalOutputMultiPilot.h"

#if (HBL_PILOT_NUM_DIGITALOUTPUTMULTI > 0)
#include "VirtualPin.h"
#include "Gpio.h"
#include "String.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
PACKED typedef struct
{
    uint8 Requested;
    uint8 Processed;
    uint8 Status;
} DIGITALOUTPUTMULTIPILOT_DATA_TYPE;

static DIGITALOUTPUTMULTIPILOT_DATA_TYPE DigitalOutputMultiPilot_Data[HBL_PILOT_NUM_DIGITALOUTPUTMULTI];
static uint8 DigitalOutputMultiPilot_Allocation;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void DoPinsUpdate(HBL_LOAD_TYPE * load);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module DigitalOutputMultiPilot and its variables
 *
 */
void DigitalOutputMultiPilot__Initialize(void)
{
    DigitalOutputMultiPilot_Allocation = 0;
    memset(DigitalOutputMultiPilot_Data,0,sizeof(DigitalOutputMultiPilot_Data));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Allocate pilot instance for DigitalOutputMultiPilot.
 *
 *  @return     uint8 result - Allocation result.
 *  @retval     0-254 - Pilot allocation instance.
 *  @retval     0xFF - Allocation failed.
 */
uint8 DigitalOutputMultiPilot__Allocate(void)
{
    uint8 retval = 0xFF;

	if (DigitalOutputMultiPilot_Allocation < HBL_PILOT_NUM_DIGITALOUTPUTMULTI)
	{
		retval = DigitalOutputMultiPilot_Allocation;
		DigitalOutputMultiPilot_Allocation++;
	}
    return(retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Initialize DigitalOutputMultiPilot instance.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated digital output load instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of DigitalOutputMultiPilot instance initialization.
 *  @retval     PASS - Instance initialization succeeded.
 *  @retval     FAIL - Instance initialization failed.
 */
PASS_FAIL_TYPE DigitalOutputMultiPilot__InitializeInstance(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval;
    uint8 virtual_pin;
    uint8 virtual_pin2;
    retval = FAIL;
    virtual_pin = load->Pin_Ptr[0];
    virtual_pin2 = load->Pin_Ptr[1];
    if ((VIRTUALPIN_TABLE[virtual_pin].port != 0xFF) &&
        (VIRTUALPIN_TABLE[virtual_pin2].port != 0xFF))
    {
        Gpio__PinConfig(VIRTUALPIN_TABLE[virtual_pin].port,VIRTUALPIN_TABLE[virtual_pin].pin,VIRTUALPIN_TABLE[virtual_pin].pin_config);
        Gpio__PinConfig(VIRTUALPIN_TABLE[virtual_pin2].port,VIRTUALPIN_TABLE[virtual_pin2].pin,VIRTUALPIN_TABLE[virtual_pin2].pin_config);

        // Shortcut to set pin output values according to load status at initialization
        DoPinsUpdate(load);
        retval = PASS;
    }

    return (retval);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Set DigitalOutputMultiPilot instance request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated DigitalOutputMultiPilot instance.
 *
 *  @param      void *data - Pointer to request data. Pass argument as uint8 *.
 *
 *  @return     PASS_FAIL_TYPE result - Result of DigitalOutputMultiPilot request.
 *  @retval     PASS - Instance request succeeded.
 *  @retval     FAIL - Instance request failed.
 */
PASS_FAIL_TYPE DigitalOutputMultiPilot__Request(HBL_LOAD_TYPE * load,void * data)
{
    PASS_FAIL_TYPE retval = FAIL;

    if (load->Pilot_Data_Index < DigitalOutputMultiPilot_Allocation)
    {
    	DigitalOutputMultiPilot_Data[load->Pilot_Data_Index].Requested = *((uint8 *)data);
        retval = PASS;
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Process DigitalOutputMultiPilot instance.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to DigitalOutputMultiPilot instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of DigitalOutputMultiPilot instance process.
 *  @retval     PASS - Instance process succeeded.
 *  @retval     FAIL - Instance process failed.
 */
PASS_FAIL_TYPE DigitalOutputMultiPilot__Process(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval = FAIL;
    HBL_LOAD_STATUS_TYPE new_status;

    if (load->Pilot_Data_Index < DigitalOutputMultiPilot_Allocation)
    {
    	DIGITALOUTPUTMULTIPILOT_DATA_TYPE *ptr = &DigitalOutputMultiPilot_Data[load->Pilot_Data_Index];

    	ptr->Processed = ptr->Requested;
    	if ((ptr->Processed >> 4) != 0)
    	{
    		new_status.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
    	}
    	else
    	{
    		new_status.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
    	}
    	new_status.Load.Cur_Reg = ptr->Processed;
    	ptr->Status = new_status.Byte;
		retval = PASS;
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get current DigitalOutputMultiPilot instance request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated DigitalOutputMultiPilot instance.
 *
 *  @return     void *requested - Requested instance data.
 *  @retval     Address of requested instance data. Cast it using *((uint8 *)ptr) to get values.
 *  @retval     NULL - Invalid load index.
 */
void * DigitalOutputMultiPilot__GetRequested(HBL_LOAD_TYPE * load)
{
	void *ptr = NULL;

    if (load->Pilot_Data_Index < DigitalOutputMultiPilot_Allocation)
    {
        ptr = &DigitalOutputMultiPilot_Data[load->Pilot_Data_Index].Requested;
    }
    return(ptr);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get DigitalOutputMultiPilot instance processed request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated DigitalOutputMultiPilot instance.
 *
 *  @return     void * processed - Processed instance data.
 *  @retval     Address of processed instance data. Cast it using *((uint8 *)ptr) to get values.
 *  @retval     NULL - Invalid load index.
 */
void * DigitalOutputMultiPilot__GetProcessed(HBL_LOAD_TYPE * load)
{
	void *ptr = NULL;

    if (load->Pilot_Data_Index < DigitalOutputMultiPilot_Allocation)
    {
        ptr = &DigitalOutputMultiPilot_Data[load->Pilot_Data_Index].Processed;
    }
    return(ptr);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get DigitalOutputMultiPilot instance status.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated DigitalOutputMultiPilot instance.
 *
 *  @return     void * status - Processed instance data.
 *  @retval     Address of processed instance data. Cast it using *((HBL_LOAD_STATUS_TYPE *)ptr) to get values.
 *  @retval     NULL - Invalid load index.
 */
void * DigitalOutputMultiPilot__GetStatus(HBL_LOAD_TYPE * load)
{
	void *ptr = NULL;

    if (load->Pilot_Data_Index < DigitalOutputMultiPilot_Allocation)
    {
        ptr = &DigitalOutputMultiPilot_Data[load->Pilot_Data_Index].Status;
    }
    return(ptr);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Asynchronous process for DigitalOutputMultiPilot.
 *
 *	@detail     Asynchronous process handles status transition and consistency check for door lock status
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to DigitalOutputMultiPilot instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of DigitalOutputMultiPilot asynch execution.
 *  @retval     PASS - Execution succeeded.
 *  @retval     FAIL - Execution failed.
 */
PASS_FAIL_TYPE DigitalOutputMultiPilot__AsynchProcess(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval = FAIL;

    if (load->Pilot_Data_Index < DigitalOutputMultiPilot_Allocation)
    {
        DoPinsUpdate(load);
        retval = PASS;
    }
    return(retval);
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief	Updates output pins according to Processed value
 *
 *
 *
 *
 */
void DoPinsUpdate(HBL_LOAD_TYPE * load)
{
	DIGITALOUTPUTMULTIPILOT_DATA_TYPE *ptr = &DigitalOutputMultiPilot_Data[load->Pilot_Data_Index];
    uint8 virtual_pin = load->Pin_Ptr[0];
    uint8 virtual_pin2 = load->Pin_Ptr[1];
    uint8 pin_val;

	if ((VIRTUALPIN_TABLE[virtual_pin].port != 0xFF) &&
		(VIRTUALPIN_TABLE[virtual_pin2].port != 0xFF))
	{
		pin_val = ptr->Processed;
		// Config bit 0 (0x10) inverts status on pin 1 only
		pin_val ^= ptr->Processed >> 4;
		// Invert pin value according to Hi Low Activation Bitmap
		pin_val ^= ~load->Hi_Low_Activation_Bitmap;
		pin_val &= 0x01;
		Gpio__PinWrite(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin, (BOOL_TYPE)pin_val);

		pin_val = DigitalOutputMultiPilot_Data[load->Pilot_Data_Index].Processed;
		// Config bit 1 (0x20) inverts status on pin 2 only
		pin_val ^= DigitalOutputMultiPilot_Data[load->Pilot_Data_Index].Processed >> 5;
		// Invert pin value according to Hi Low Activation Bitmap
		pin_val ^= ~(load->Hi_Low_Activation_Bitmap >> 1);
		pin_val &= 0x01;
		Gpio__PinWrite(VIRTUALPIN_TABLE[virtual_pin2].port, VIRTUALPIN_TABLE[virtual_pin2].pin,(BOOL_TYPE)pin_val);
	}

}

#endif

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
 *  $Header: ChargePumpPilot.c 1.8 2015/07/14 17:37:51EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/14 17:37:51EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "ChargePumpPilot.h"

#if (HBL_PILOT_NUM_CHARGEPUMP > 0)
#include "VirtualPin.h"
#include "String.h"
#include "Gpio.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
typedef struct
{
    ON_OFF_TYPE ChargePump_State;
    ON_OFF_TYPE Requested_State;
    BOOL_TYPE   Pin_State       :   1;
    uint8       Unused          :   7;
}CHARGEPUMPPILOT_DATA_TYPE;

static CHARGEPUMPPILOT_DATA_TYPE ChargePumpPilot_Data[HBL_PILOT_NUM_CHARGEPUMP];
static uint8 ChargePumpPilot_Allocation;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

/**
 *
 */
void ChargePumpPilot__Initialize(void)
{
    ChargePumpPilot_Allocation = 0;
    memset (ChargePumpPilot_Data,0,sizeof(ChargePumpPilot_Data));

}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @return
 */
uint8 ChargePumpPilot__Allocate(void)
{
    uint8 retval;
    retval = 0xFF;
    if (ChargePumpPilot_Allocation < HBL_PILOT_NUM_CHARGEPUMP)
    {
        retval = ChargePumpPilot_Allocation;
        ChargePumpPilot_Allocation++;

    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param load
 */
PASS_FAIL_TYPE ChargePumpPilot__InitializeInstance(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval;
    uint8 virtual_pin;
    retval = FAIL;
    virtual_pin = (*load->Pin_Ptr);
    if (VIRTUALPIN_TABLE[virtual_pin].port != PORT_NULL)
    {
        Gpio__PinWrite(  VIRTUALPIN_TABLE[virtual_pin].port,
            VIRTUALPIN_TABLE[virtual_pin].pin,
            (BOOL_TYPE) ((~load->Hi_Low_Activation_Bitmap) & 0x01));
        Gpio__PinConfig(VIRTUALPIN_TABLE[virtual_pin].port,VIRTUALPIN_TABLE[virtual_pin].pin,VIRTUALPIN_TABLE[virtual_pin].pin_config);
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
PASS_FAIL_TYPE ChargePumpPilot__Request(HBL_LOAD_TYPE * load,void * data)
{

    if (load->Pilot_Data_Index < HBL_PILOT_NUM_CHARGEPUMP)
    {
        if (*((uint8 *)data) > 0)
        {
            ChargePumpPilot_Data[load->Pilot_Data_Index].Requested_State = ON;
        }
        else
        {
            ChargePumpPilot_Data[load->Pilot_Data_Index].Requested_State = OFF;
        }
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
PASS_FAIL_TYPE ChargePumpPilot__Process(HBL_LOAD_TYPE * load)
{
    if (load->Pilot_Data_Index < HBL_PILOT_NUM_CHARGEPUMP)
    {
        ChargePumpPilot_Data[load->Pilot_Data_Index].ChargePump_State = ChargePumpPilot_Data[load->Pilot_Data_Index].Requested_State;
    }
    return (PASS);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param load
 * @param data_out
 * @return
 */
void * ChargePumpPilot__GetRequested(HBL_LOAD_TYPE * load)
{
    if (load->Pilot_Data_Index < HBL_PILOT_NUM_CHARGEPUMP)
    {
        return (&ChargePumpPilot_Data[load->Pilot_Data_Index].Requested_State);
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
void * ChargePumpPilot__GetProcessed(HBL_LOAD_TYPE * load)
{
    if (load->Pilot_Data_Index < HBL_PILOT_NUM_CHARGEPUMP)
    {
        return (&ChargePumpPilot_Data[load->Pilot_Data_Index].ChargePump_State);
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
PASS_FAIL_TYPE ChargePumpPilot__Handler1ms(HBL_LOAD_TYPE * load)
{
    uint8 virtual_pin;
    PASS_FAIL_TYPE retval;

    retval = FAIL;
    if (load->Pilot_Data_Index < HBL_PILOT_NUM_CHARGEPUMP)
    {

        if (ChargePumpPilot_Data[load->Pilot_Data_Index].ChargePump_State == ON)
        {
            ChargePumpPilot_Data[load->Pilot_Data_Index].Pin_State = (BOOL_TYPE)(ChargePumpPilot_Data[load->Pilot_Data_Index].Pin_State ^ 1);
        }
        else
        {
            //TODO To check the off state from the setting file
            ChargePumpPilot_Data[load->Pilot_Data_Index].Pin_State = (BOOL_TYPE) ((~load->Hi_Low_Activation_Bitmap) & 0x01);
        }

        virtual_pin = (*load->Pin_Ptr);

        if (VIRTUALPIN_TABLE[virtual_pin].port != 0xFF)
        {

           Gpio__PinWrite(  VIRTUALPIN_TABLE[virtual_pin].port,
                            VIRTUALPIN_TABLE[virtual_pin].pin,
                            ChargePumpPilot_Data[load->Pilot_Data_Index].Pin_State);
            retval = PASS;
        }

    }
    return (retval);

}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


#endif

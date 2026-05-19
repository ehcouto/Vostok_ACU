/**
 *  @file
 *
 *  @brief      Pilot handling ZC synchronized Digital output
 *
 *  @details    It handles multiple instances of a ZCDigital pilot that drives a single output in sync with ZeroCross.
 *
 *  @copyright  Copyright 2022- Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 *
*/
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------


#include "SystemConfig.h"

#if (HBL_PILOT_NUM_TRIACSWITCH > 0)
#include "TriacSwitchPilot.h"
#include "Gpio.h"
#include "VirtualPin.h"
#include "ZeroCross.h"
#include <string.h>

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#define     TRIAC_SWITCH_OFF_DELAY_MS       5U

typedef enum
{
    TRIACSWITCH_OFF_STATE = 0,
    TRIACSWITCH_SYNC_STATE,
    TRIACSWITCH_ON_STATE,
    TRIACSWITCH_TURNING_OFF,
    TRIACSWITCH_END_STATE,
}TRIACSWITCHPILOT_STATE_TYPE;


typedef struct
{
    uint8                           Requested;
    uint8                           Processed;
    uint16                          Timer_Counter;
    TRIACSWITCHPILOT_STATE_TYPE     State;
    TRIACSWITCHPILOT_STATUS_TYPE    Pilot_Status;
} TRIACSWITCHPILOT_DATA_TYPE;

static TRIACSWITCHPILOT_DATA_TYPE Triac_Switch_Data[HBL_PILOT_NUM_TRIACSWITCH];

/* Current TriacSwitchPilot allocated */
static uint8 Triac_Switch_Allocation;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module TriacSwitchPilot and its variables
 *
 */
void TriacSwitchPilot__Initialize(void)
{
    Triac_Switch_Allocation = 0;
    memset(Triac_Switch_Data,0,sizeof(Triac_Switch_Data));
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Allocate pilot instance for TriacSwitchPilot.
 *
 *  @return     uint8 result - Allocation result.
 *  @retval     0-254 - Pilot allocation instance.
 *  @retval     0xFF - Allocation failed.
 */
uint8 TriacSwitchPilot__Allocate(void)
{
    uint8 retval = 0xFF;
    if (Triac_Switch_Allocation < (uint8) HBL_PILOT_NUM_TRIACSWITCH)
    {
        retval = Triac_Switch_Allocation;
        Triac_Switch_Allocation++;
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Initialize TriacSwitchPilot instance.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated digital output load instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of TriacSwitchPilot instance initialization.
 *  @retval     PASS - Instance initialization succeeded.
 *  @retval     FAIL - Instance initialization failed.
 */
PASS_FAIL_TYPE TriacSwitchPilot__InitializeInstance(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval;
    retval = FAIL;
    uint8 virtual_pin = load->Pin_Ptr[0];

    if ((load->Pilot_Data_Index < Triac_Switch_Allocation) &&
        (virtual_pin < VirtualPin__GetNumOf()))
    {
        uint8 pin_val = Triac_Switch_Data[load->Pilot_Data_Index].Processed;

        Triac_Switch_Data[load->Pilot_Data_Index].State = TRIACSWITCH_OFF_STATE;

        // Invert pin value according to Hi Low Activation Bitmap
        pin_val ^= ~load->Hi_Low_Activation_Bitmap;
        pin_val &= 0x01U;

        if (VIRTUALPIN_TABLE[virtual_pin].port != PORT_NULL)
        {
            Gpio__PinWrite(VIRTUALPIN_TABLE[virtual_pin].port,VIRTUALPIN_TABLE[virtual_pin].pin, (BOOL_TYPE) pin_val);
            Gpio__PinConfig(VIRTUALPIN_TABLE[virtual_pin].port,VIRTUALPIN_TABLE[virtual_pin].pin,VIRTUALPIN_TABLE[virtual_pin].pin_config);
            Triac_Switch_Data[load->Pilot_Data_Index].Pilot_Status.Hbl_Status.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
            Triac_Switch_Data[load->Pilot_Data_Index].Pilot_Status.Hbl_Status.Load.Cur_Reg = 0U;
            Triac_Switch_Data[load->Pilot_Data_Index].Pilot_Status.Pin_Value = 0U;
            retval = PASS;
        }
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Set TriacSwitchPilot instance request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated TriacSwitchPilot instance.
 *
 *  @param      void *data - Pointer to request data. Pass argument as uint8 *.
 *
 *  @return     PASS_FAIL_TYPE result - Result of TriacSwitchPilot request.
 *  @retval     PASS - Instance request succeeded.
 *  @retval     FAIL - Instance request failed.
 */
PASS_FAIL_TYPE TriacSwitchPilot__Request(HBL_LOAD_TYPE * load, void * data)
{
    PASS_FAIL_TYPE retval = FAIL;
    if (load->Pilot_Data_Index < Triac_Switch_Allocation)
    {
        Triac_Switch_Data[load->Pilot_Data_Index].Requested = *((uint8 *)data);
        retval = PASS;
    }
    return(retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Process TriacSwitchPilot instance.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to TriacSwitchPilot instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of TriacSwitchPilot instance process.
 *  @retval     PASS - Instance process succeeded.
 *  @retval     FAIL - Instance process failed.
 */
PASS_FAIL_TYPE TriacSwitchPilot__Process(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval = FAIL;
    if (load->Pilot_Data_Index < Triac_Switch_Allocation)
    {
        // Transition to a new state
        if (Triac_Switch_Data[load->Pilot_Data_Index].Requested != Triac_Switch_Data[load->Pilot_Data_Index].Processed)
        {
            //if(Triac_Switch_Data[load->Pilot_Data_Index].Requested)
            Triac_Switch_Data[load->Pilot_Data_Index].Processed = Triac_Switch_Data[load->Pilot_Data_Index].Requested;
            Triac_Switch_Data[load->Pilot_Data_Index].Pilot_Status.Hbl_Status.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
            Triac_Switch_Data[load->Pilot_Data_Index].Pilot_Status.Hbl_Status.Load.Cur_Reg = Triac_Switch_Data[load->Pilot_Data_Index].Requested;
        }
        retval = PASS;
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Zero-Cross triggered process for TriacSwitchPilot.
 *
 *  @detail     Low level feedback probe to detect frequency and pin activation in sync with feedback
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to TriacSwitchPilot instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of TriacSwitchPilot ZC execution.
 *  @retval     PASS - Execution succeeded.
 *  @retval     FAIL - Execution failed.
 */
PASS_FAIL_TYPE TriacSwitchPilot__ZCProcess(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval = FAIL;

    if (load->Pilot_Data_Index < Triac_Switch_Allocation)
    {
        uint8 active_pin_value = load->Hi_Low_Activation_Bitmap & 0x01U;
        uint8 virtual_pin = load->Pin_Ptr[0];

        //Read the current zero cross angle
        switch(Triac_Switch_Data[load->Pilot_Data_Index].State)
        {

            case TRIACSWITCH_OFF_STATE:
                if(Triac_Switch_Data[load->Pilot_Data_Index].Processed != 0U)
                {
                    Gpio__PinWrite(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin, (BOOL_TYPE)((load->Hi_Low_Activation_Bitmap) & 0x01U));
                    Gpio__PinConfig(VIRTUALPIN_TABLE[virtual_pin].port,VIRTUALPIN_TABLE[virtual_pin].pin,VIRTUALPIN_TABLE[virtual_pin].pin_config);
                    Triac_Switch_Data[load->Pilot_Data_Index].Pilot_Status.Hbl_Status.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
                    Triac_Switch_Data[load->Pilot_Data_Index].Pilot_Status.Pin_Value = 1U;
                    Triac_Switch_Data[load->Pilot_Data_Index].State = TRIACSWITCH_ON_STATE;
                }
                break;

            case TRIACSWITCH_ON_STATE:
                if(Triac_Switch_Data[load->Pilot_Data_Index].Processed == 0U)
                {
                    Triac_Switch_Data[load->Pilot_Data_Index].Timer_Counter = TRIAC_SWITCH_OFF_DELAY_MS;
                    Triac_Switch_Data[load->Pilot_Data_Index].State = TRIACSWITCH_TURNING_OFF;
                }
                break;

            case TRIACSWITCH_TURNING_OFF:
                break;

            default:
                // Force pin OFF
                active_pin_value ^= 0x01U;
                Gpio__PinWrite(VIRTUALPIN_TABLE[virtual_pin].port,VIRTUALPIN_TABLE[virtual_pin].pin, (BOOL_TYPE) active_pin_value);
                Gpio__PinConfig(VIRTUALPIN_TABLE[virtual_pin].port,VIRTUALPIN_TABLE[virtual_pin].pin,VIRTUALPIN_TABLE[virtual_pin].pin_config);
                Triac_Switch_Data[load->Pilot_Data_Index].Pilot_Status.Hbl_Status.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
                Triac_Switch_Data[load->Pilot_Data_Index].Pilot_Status.Hbl_Status.Load.Cur_Reg = 0U;
                Triac_Switch_Data[load->Pilot_Data_Index].Pilot_Status.Pin_Value = 0U;
                break;

        }
        retval = PASS;
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      One millisecond process for TriacSwitchPilot.
 *
 *  @detail     Low level feedback probe to detect frequency and pin activation in sync with feedback
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to TriacSwitchPilot instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of TriacSwitchPilot 1ms execution.
 *  @retval     PASS - Execution succeeded.
 *  @retval     FAIL - Execution failed.
 */
PASS_FAIL_TYPE TriacSwitchPilot__OneMsHandler(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval = FAIL;

    if (load->Pilot_Data_Index < Triac_Switch_Allocation)
    {
        switch(Triac_Switch_Data[load->Pilot_Data_Index].State)
        {
            case TRIACSWITCH_TURNING_OFF:
                if(Triac_Switch_Data[load->Pilot_Data_Index].Timer_Counter == 0U)
                {
                    uint8 virtual_pin = load->Pin_Ptr[0];
                    uint8 active_pin_value = load->Hi_Low_Activation_Bitmap & 0x01U;
                    active_pin_value ^= 0x01U;
                    Gpio__PinWrite(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin, (BOOL_TYPE) active_pin_value);
                    Gpio__PinConfig(VIRTUALPIN_TABLE[virtual_pin].port,VIRTUALPIN_TABLE[virtual_pin].pin,VIRTUALPIN_TABLE[virtual_pin].pin_config);
                    Triac_Switch_Data[load->Pilot_Data_Index].Pilot_Status.Hbl_Status.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
                    Triac_Switch_Data[load->Pilot_Data_Index].Pilot_Status.Pin_Value = 0U;
                    Triac_Switch_Data[load->Pilot_Data_Index].State = TRIACSWITCH_OFF_STATE;
                }
                else
                {
                    Triac_Switch_Data[load->Pilot_Data_Index].Timer_Counter--;
                }
                break;
            default:
                // Nothing to do
                break;
        }
        retval = PASS;
    }
    return(retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get TriacSwitchPilot instance status.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to TriacSwitchPilot instance.
 *
 *  @return     void *status - instance Status.
 *  @retval     Address of instance Status. Cast it using *((TRIACSWITCHPILOT_STATUS_TYPE *)ptr) to get the value.
 *  @retval     NULL - Invalid load index.
 */
void * TriacSwitchPilot__GetStatus(HBL_LOAD_TYPE *load)
{
    void *status_ptr = NULL;
    if (load->Pilot_Data_Index < Triac_Switch_Allocation)
    {
    	status_ptr = &Triac_Switch_Data[load->Pilot_Data_Index].Pilot_Status;
    }
    return(status_ptr);
}
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get current TriacSwitchPilot instance request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated TriacSwitchPilot instance.
 *
 *  @return     void *requested - Requested instance data.
 *  @retval     Address of requested instance data. Cast it using *((uint8 *)ptr) to get values.
 *  @retval     NULL - Invalid load index.
 */
void * TriacSwitchPilot__GetRequested(HBL_LOAD_TYPE * load)
{
    void *requested_ptr = NULL;
    if (load->Pilot_Data_Index < Triac_Switch_Allocation)
    {
        requested_ptr = &Triac_Switch_Data[load->Pilot_Data_Index].Requested;
    }
    return(requested_ptr);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get TriacSwitchPilot instance processed request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated TriacSwitchPilot instance.
 *
 *  @return     void * processed - Processed instance data.
 *  @retval     Address of processed instance data. Cast it using *((uint8 *)ptr) to get values.
 *  @retval     NULL - Invalid load index.
 */
void * TriacSwitchPilot__GetProcessed(HBL_LOAD_TYPE * load)
{
    void * processed_ptr = NULL;
    if (load->Pilot_Data_Index < Triac_Switch_Allocation)
    {
    	processed_ptr = &Triac_Switch_Data[load->Pilot_Data_Index].Processed;
    }
    return (processed_ptr);
}
//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


#endif

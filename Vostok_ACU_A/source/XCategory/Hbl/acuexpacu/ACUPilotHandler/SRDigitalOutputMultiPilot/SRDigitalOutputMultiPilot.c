/**
 *  @file       SRDigitalOutputMultiPilot.c
 *
 *  @brief      Pilot Handler for DigitalOutputMulti
 *
 *  @details    It handles 2 pin output configuration only; they are driven together
 *  			according to Processed value and each Pin High flag set.
 *
 *	@defgroup	CLASS_B
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: SRDigitalOutputMultiPilot.c 1.8 2015/07/14 17:37:52EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/14 17:37:52EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "SRDigitalOutputMultiPilot.h"

#if (HBL_PILOT_NUM_SRDIGITALOUTPUTMULTI > 0)
#include "VirtualPin.h"
#include "Gpio.h"
#include "SRData.h"
#include "SRException.h"
#include "SRTaskSequence.h"
#include "String.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
PACKED typedef struct
{
    uint8 Requested;
    uint8 Processed;
    uint8 Status;
    uint8 Last_Pin_1_Value;
    uint8 Last_Pin_2_Value;
} SRDIGITALOUTPUTMULTIPILOT_DATA_TYPE;

static SRDIGITALOUTPUTMULTIPILOT_DATA_TYPE SRDigitalOutputMultiPilot_Data[HBL_PILOT_NUM_SRDIGITALOUTPUTMULTI];
static uint8 SRDigitalOutputMultiPilot_Allocation;

static SRDIGITALOUTPUTMULTIPILOT_DATA_TYPE N_SRDigitalOutputMultiPilot_Data[HBL_PILOT_NUM_SRDIGITALOUTPUTMULTI];
static uint8 N_SRDigitalOutputMultiPilot_Allocation;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void DoPinsUpdate(HBL_LOAD_TYPE * load);
static BOOL_TYPE CheckItem(uint8 index);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module SRDigitalOutputMultiPilot and its variables
 *
 */
void SRDigitalOutputMultiPilot__Initialize(void)
{
    SRData__UpdateByte(&SRDigitalOutputMultiPilot_Allocation, &N_SRDigitalOutputMultiPilot_Allocation , 0);
    memset(SRDigitalOutputMultiPilot_Data,0,sizeof(SRDigitalOutputMultiPilot_Data));
    memset(N_SRDigitalOutputMultiPilot_Data,0xFF,sizeof(N_SRDigitalOutputMultiPilot_Data));
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_INIT, SRTASKSEQUENCE_APPL_INIT_SRDIGITALOUTPUTMULTIPILOT);
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Allocate pilot instance for SRDigitalOutputMultiPilot.
 *
 *  @return     uint8 result - Allocation result.
 *  @retval     0-254 - Pilot allocation instance.
 *  @retval     0xFF - Allocation failed.
 */
uint8 SRDigitalOutputMultiPilot__Allocate(void)
{
    uint8 retval = 0xFF;

    if (SRData__CheckByte(&SRDigitalOutputMultiPilot_Allocation, &N_SRDigitalOutputMultiPilot_Allocation) == SRDATA_OK)
    {
        if (SRDigitalOutputMultiPilot_Allocation < HBL_PILOT_NUM_SRDIGITALOUTPUTMULTI)
        {
            retval = SRDigitalOutputMultiPilot_Allocation;
            SRData__UpdateByte(&SRDigitalOutputMultiPilot_Allocation, &N_SRDigitalOutputMultiPilot_Allocation, SRDigitalOutputMultiPilot_Allocation+1);
        }
    }
    else
    {
        SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, HBL_PILOT_DIGITAL_OUTPUT_MULTI_2_PIN, 0xFF );
    }

    return(retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Initialize SRDigitalOutputMultiPilot instance.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated digital output load instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of SRDigitalOutputMultiPilot instance initialization.
 *  @retval     PASS - Instance initialization succeeded.
 *  @retval     FAIL - Instance initialization failed.
 */
PASS_FAIL_TYPE SRDigitalOutputMultiPilot__InitializeInstance(HBL_LOAD_TYPE * load)
 {
    PASS_FAIL_TYPE retval;
    uint8 virtual_pin;
    uint8 virtual_pin2;
    BOOL_TYPE pin1_val;
    BOOL_TYPE pin2_val;
    retval = FAIL;

    if (CheckItem(load->Pilot_Data_Index))
    {
        SRDIGITALOUTPUTMULTIPILOT_DATA_TYPE *ptr = &SRDigitalOutputMultiPilot_Data[load->Pilot_Data_Index];
        virtual_pin = load->Pin_Ptr[0];
        virtual_pin2 = load->Pin_Ptr[1];
        if ((VIRTUALPIN_TABLE[virtual_pin].port != 0xFF) && (VIRTUALPIN_TABLE[virtual_pin2].port != 0xFF))
        {
            // Shortcut to set pin output values according to load status at initialization
            DoPinsUpdate(load);
            Gpio__PinConfig(VIRTUALPIN_TABLE[virtual_pin].port,	VIRTUALPIN_TABLE[virtual_pin].pin, VIRTUALPIN_TABLE[virtual_pin].pin_config);
            Gpio__PinConfig(VIRTUALPIN_TABLE[virtual_pin2].port, VIRTUALPIN_TABLE[virtual_pin2].pin, VIRTUALPIN_TABLE[virtual_pin2].pin_config);
        }

        pin1_val = (BOOL_TYPE)ptr->Last_Pin_1_Value;
        pin2_val = (BOOL_TYPE)ptr->Last_Pin_2_Value;

        if ((Gpio__CheckConfig(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin, VIRTUALPIN_TABLE[virtual_pin].pin_config, pin1_val) == TRUE) &&
            (Gpio__CheckConfig(VIRTUALPIN_TABLE[virtual_pin2].port, VIRTUALPIN_TABLE[virtual_pin2].pin, VIRTUALPIN_TABLE[virtual_pin2].pin_config, pin2_val) == TRUE))
        {
            retval = PASS;
        }

    }
    else
    {
        SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, HBL_PILOT_DIGITAL_OUTPUT_MULTI_2_PIN, load->Pilot_Data_Index);
    }

    return (retval);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Set SRDigitalOutputMultiPilot instance request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated SRDigitalOutputMultiPilot instance.
 *
 *  @param      void *data - Pointer to request data. Pass argument as uint8 *.
 *
 *  @return     PASS_FAIL_TYPE result - Result of SRDigitalOutputMultiPilot request.
 *  @retval     PASS - Instance request succeeded.
 *  @retval     FAIL - Instance request failed.
 */
PASS_FAIL_TYPE SRDigitalOutputMultiPilot__Request(HBL_LOAD_TYPE * load,void * data)
{
    PASS_FAIL_TYPE retval = FAIL;

    if (CheckItem(load->Pilot_Data_Index))
    {
        SRDIGITALOUTPUTMULTIPILOT_DATA_TYPE *ptr = &SRDigitalOutputMultiPilot_Data[load->Pilot_Data_Index];
        SRDIGITALOUTPUTMULTIPILOT_DATA_TYPE *n_ptr = &N_SRDigitalOutputMultiPilot_Data[load->Pilot_Data_Index];

        SRData__UpdateByte(&ptr->Requested, &n_ptr->Requested, *((uint8 *)data));
        retval = PASS;
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Process SRDigitalOutputMultiPilot instance.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to SRDigitalOutputMultiPilot instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of SRDigitalOutputMultiPilot instance process.
 *  @retval     PASS - Instance process succeeded.
 *  @retval     FAIL - Instance process failed.
 */
PASS_FAIL_TYPE SRDigitalOutputMultiPilot__Process(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval = FAIL;
    HBL_LOAD_STATUS_TYPE new_status;

    if (CheckItem(load->Pilot_Data_Index))
    {
        SRDIGITALOUTPUTMULTIPILOT_DATA_TYPE *ptr = &SRDigitalOutputMultiPilot_Data[load->Pilot_Data_Index];
        SRDIGITALOUTPUTMULTIPILOT_DATA_TYPE *n_ptr = &N_SRDigitalOutputMultiPilot_Data[load->Pilot_Data_Index];

        SRData__UpdateByte(&ptr->Processed, &n_ptr->Processed, ptr->Requested);
        if ((ptr->Processed >> 4) != 0)
        {
            new_status.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
        }
        else
        {
            new_status.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
        }
        new_status.Load.Cur_Reg = ptr->Processed;
        SRData__UpdateByte(&ptr->Status, &n_ptr->Status, new_status.Byte);
        retval = PASS;
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get current SRDigitalOutputMultiPilot instance request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated SRDigitalOutputMultiPilot instance.
 *
 *  @return     void *requested - Requested instance data.
 *  @retval     Address of requested instance data. Cast it using *((uint8 *)ptr) to get values.
 *  @retval     NULL - Invalid load index.
 */
void * SRDigitalOutputMultiPilot__GetRequested(HBL_LOAD_TYPE * load)
{
    void *ptr = NULL;

    if (CheckItem(load->Pilot_Data_Index))
    {
        ptr = &SRDigitalOutputMultiPilot_Data[load->Pilot_Data_Index].Requested;
    }
    return(ptr);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get SRDigitalOutputMultiPilot instance processed request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated SRDigitalOutputMultiPilot instance.
 *
 *  @return     void * processed - Processed instance data.
 *  @retval     Address of processed instance data. Cast it using *((uint8 *)ptr) to get values.
 *  @retval     NULL - Invalid load index.
 */
void * SRDigitalOutputMultiPilot__GetProcessed(HBL_LOAD_TYPE * load)
{
	void *ptr = NULL;

    if (CheckItem(load->Pilot_Data_Index))
    {
        ptr = &SRDigitalOutputMultiPilot_Data[load->Pilot_Data_Index].Processed;
    }
    return(ptr);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get SRDigitalOutputMultiPilot instance status.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated SRDigitalOutputMultiPilot instance.
 *
 *  @return     void * status - Processed instance data.
 *  @retval     Address of processed instance data. Cast it using *((HBL_LOAD_STATUS_TYPE *)ptr) to get values.
 *  @retval     NULL - Invalid load index.
 */
void * SRDigitalOutputMultiPilot__GetStatus(HBL_LOAD_TYPE * load)
{
    void *ptr = NULL;

    if (CheckItem(load->Pilot_Data_Index))
    {
        ptr = &SRDigitalOutputMultiPilot_Data[load->Pilot_Data_Index].Status;
    }
    return(ptr);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Asynchronous process for SRDigitalOutputMultiPilot.
 *
 *	@detail     Asynchronous process handles status transition and consistency check for door lock status
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to SRDigitalOutputMultiPilot instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of SRDigitalOutputMultiPilot asynch execution.
 *  @retval     PASS - Execution succeeded.
 *  @retval     FAIL - Execution failed.
 */
PASS_FAIL_TYPE SRDigitalOutputMultiPilot__AsynchProcess(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval = FAIL;

    if (CheckItem(load->Pilot_Data_Index))
    {
        SRDIGITALOUTPUTMULTIPILOT_DATA_TYPE *ptr = &SRDigitalOutputMultiPilot_Data[load->Pilot_Data_Index];
        uint8 virtual_pin = load->Pin_Ptr[0];
        uint8 virtual_pin2 = load->Pin_Ptr[1];

        if ((VIRTUALPIN_TABLE[virtual_pin].port != 0xFF) &&
            (VIRTUALPIN_TABLE[virtual_pin2].port != 0xFF))
        {
            BOOL_TYPE pin1_val = (BOOL_TYPE) ptr->Last_Pin_1_Value;
            BOOL_TYPE pin2_val = (BOOL_TYPE) ptr->Last_Pin_2_Value;

            if ((Gpio__CheckConfig(VIRTUALPIN_TABLE[virtual_pin].port,VIRTUALPIN_TABLE[virtual_pin].pin,VIRTUALPIN_TABLE[virtual_pin].pin_config, pin1_val) == TRUE) &&
                (Gpio__CheckConfig(VIRTUALPIN_TABLE[virtual_pin2].port,VIRTUALPIN_TABLE[virtual_pin2].pin,VIRTUALPIN_TABLE[virtual_pin2].pin_config, pin2_val)	== TRUE))
            {
                DoPinsUpdate(load);
                retval = PASS;
            }
    		else
    		{
                SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, HBL_PILOT_DIGITAL_OUTPUT_MULTI_2_PIN, load->Pilot_Data_Index);
    		}
        }
    }
    return(retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief	Handler for SRDigitalOutputMultiPilot execution
 *
 * It checks SRDigitalOutputMultiPilot_Allocation integrity and all SRDigitalOutputMultiPilot_Data instances allocated
 * It sets SRTASKSEQUENCE_APPL_SRDIGITALOUTPUTMULTIPILOT_CHECKS step in SRTASKSEQUENCE_APPL_SAFETY_THREAD sequence
 *
 */
void SRDigitalOutputMultiPilot__Handler(void)
{
    if (SRData__CheckByte(&SRDigitalOutputMultiPilot_Allocation, &N_SRDigitalOutputMultiPilot_Allocation) != SRDATA_OK)
    {
    	SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, HBL_PILOT_DIGITAL_OUTPUT_MULTI_2_PIN, 0xFF);
    }
    else
    {
        uint8 index;

        for(index = 0; index < HBL_PILOT_NUM_SRDIGITALOUTPUTMULTI; index++)
        {
        	if (SRData__CheckArray(&SRDigitalOutputMultiPilot_Data[index], &N_SRDigitalOutputMultiPilot_Data[index], sizeof(SRDIGITALOUTPUTMULTIPILOT_DATA_TYPE)) != SRDATA_OK)
            {
                SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, HBL_PILOT_DIGITAL_OUTPUT_MULTI_2_PIN, index);
                break;
            }
        }
    }

#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_SRDIGITALOUTPUTMULTIPILOT_CHECKS);
#endif


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
    SRDIGITALOUTPUTMULTIPILOT_DATA_TYPE *ptr = &SRDigitalOutputMultiPilot_Data[load->Pilot_Data_Index];
    SRDIGITALOUTPUTMULTIPILOT_DATA_TYPE *n_ptr = &N_SRDigitalOutputMultiPilot_Data[load->Pilot_Data_Index];
    uint8 virtual_pin = load->Pin_Ptr[0];
    uint8 virtual_pin2 = load->Pin_Ptr[1];
    uint8 pin_val;

    pin_val = ptr->Processed;
    // Config bit 0 (0x10) inverts status on pin 1 only
    pin_val ^= ptr->Processed >> 4;
    // Invert pin value according to Hi Low Activation Bitmap
    pin_val ^= ~load->Hi_Low_Activation_Bitmap;
    pin_val &= 0x01;
    Gpio__PinWrite(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin, (BOOL_TYPE)pin_val);
    SRData__UpdateByte(&ptr->Last_Pin_1_Value, &n_ptr->Last_Pin_1_Value, pin_val);

    pin_val = ptr->Processed;
    // Config bit 1 (0x20) inverts status on pin 2 only
    pin_val ^= ptr->Processed >> 5;
    // Invert pin value according to Hi Low Activation Bitmap
    pin_val ^= ~(load->Hi_Low_Activation_Bitmap >> 1);
    pin_val &= 0x01;
    Gpio__PinWrite(VIRTUALPIN_TABLE[virtual_pin2].port, VIRTUALPIN_TABLE[virtual_pin2].pin,(BOOL_TYPE)pin_val);
    SRData__UpdateByte(&ptr->Last_Pin_2_Value, &n_ptr->Last_Pin_2_Value, pin_val);

}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief	Handler for SRDigitalOutputMultiPilot execution
 *
 * It checks SRDigitalOutputMultiPilot_Allocation integrity and all SRDigitalOutputMultiPilot_Data instances allocated
 * It sets SRTASKSEQUENCE_APPL_SRDIGITALOUTPUTMULTIPILOT_CHECKS step in SRTASKSEQUENCE_APPL_SAFETY_THREAD sequence
 *
 */
BOOL_TYPE CheckItem(uint8 index)
{
    BOOL_TYPE retval = FALSE;

    if (SRData__CheckByte(&SRDigitalOutputMultiPilot_Allocation, &N_SRDigitalOutputMultiPilot_Allocation) != SRDATA_OK)
    {
        SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, HBL_PILOT_DIGITAL_OUTPUT_MULTI_2_PIN, 0xFF);
    }
    else
    {
        if (index < SRDigitalOutputMultiPilot_Allocation)
        {
            if (SRData__CheckArray(&SRDigitalOutputMultiPilot_Data[index], &N_SRDigitalOutputMultiPilot_Data[index], sizeof(SRDIGITALOUTPUTMULTIPILOT_DATA_TYPE)) != SRDATA_OK)
            {
                SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, HBL_PILOT_DIGITAL_OUTPUT_MULTI_2_PIN, index);
            }
            else
            {
                retval = TRUE;
            }
        }
    }

    return (retval);
}

#endif

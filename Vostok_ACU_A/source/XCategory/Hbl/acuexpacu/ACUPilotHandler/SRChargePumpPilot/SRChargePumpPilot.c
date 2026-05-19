/**
 *  @file       
 *  @defgroup    CLASS_B
 *
 * @brief       Implements a Class B pilot that drives a Charge pump
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/14 17:37:51EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SRChargePumpPilot.h"

#if (HBL_PILOT_NUM_SRCHARGEPUMP > 0)
#include "VirtualPin.h"
#include <String.h>
#include "Gpio.h"
#include "SRData.h"
#include "SRException.h"
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    #include "SRTaskSequence.h"
#endif

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
// -- Private Structure Type Declarations --

//! Volatile data required for each instance of the pilot.
PACKED typedef struct
{
    //! The value requested by the application modules.
    ON_OFF_TYPE Requested;

    //! The value accepted and processed.
    ON_OFF_TYPE Processed;

    //! The current state of the pin
    BOOL_TYPE   Pin_State;
}SR_CHARGEPUMPPILOT_INSTANCE_DATA_TYPE;


//! Volatile data required for the pilot module.
PACKED typedef struct
{
    SR_CHARGEPUMPPILOT_INSTANCE_DATA_TYPE Data[HBL_PILOT_NUM_SRCHARGEPUMP];

    //! Tracks the number of digital output pilot allocations.
    uint8 Allocation;
}SR_CHARGEPUMPPILOT_DATA_TYPE;


//! All the volatile data used by this module.
static SR_CHARGEPUMPPILOT_DATA_TYPE SRChargePumpPilot_Data;

//! An inverted copy of the volatile data used by this module for data integrity purposes.
static SR_CHARGEPUMPPILOT_DATA_TYPE N_SRChargePumpPilot_Data;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * Initialize all instances of this Pilot type.
 *  - Initialize static RAM used by the module.
 *  - Initialize any corrupted NVData used by the module.
 */
void SRChargePumpPilot__Initialize(void)
{
    SR_FILL(SRChargePumpPilot_Data, 0, sizeof(SRChargePumpPilot_Data));

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_INIT, SRTASKSEQUENCE_APPL_INIT_SRCHARGEPUMP_PILOT);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Reserve memory for this pilot instance.
 * @return The index of the allocated Pilot.
 *         Return 0xFF if there is no space allocated for another Pilot of this type.
 */
uint8 SRChargePumpPilot__Allocate(void)
{
    uint8 pilot_index = 0xFF;

    SR_CHECK_BYTE(SRChargePumpPilot_Data.Allocation);

    if (SRChargePumpPilot_Data.Allocation < HBL_PILOT_NUM_SRCHARGEPUMP)
    {
        pilot_index = SRChargePumpPilot_Data.Allocation;
        SR_SET_BYTE(SRChargePumpPilot_Data.Allocation, SRChargePumpPilot_Data.Allocation + 1);
    }
    return (pilot_index);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Initialize this pilot instance.
 * @param load = A pointer to the HBL_LOAD_TYPE structure with load and pilot instance information.
 * @return PASS if the Pilot instance is successfully initialized; otherwise FAIL.
 */
PASS_FAIL_TYPE SRChargePumpPilot__InitializeInstance(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE result = FAIL;

    if (load != NULL)
    {
        uint8 virtual_pin = (*load->Pin_Ptr);;

        SR_CHECK_BYTE(SRChargePumpPilot_Data.Allocation);

        if ((load->Pilot_Data_Index < SRChargePumpPilot_Data.Allocation) &&
            (virtual_pin < VirtualPin__GetNumOf()) &&
            (VIRTUALPIN_TABLE[virtual_pin].port != PORT_NULL))
        {
            const VIRTUALPIN_TYPE* vp = &VIRTUALPIN_TABLE[virtual_pin];
            GPIO_PORT_TYPE port = vp->port;
            uint8 pin = vp->pin;
            if ((Gpio__Validate(port, pin) == PASS) &&
                (vp->pin_config == OUTPUT_PUSHPULL))
            {
                SRData__CheckByte((uint8 *)&SRChargePumpPilot_Data.Data[load->Pilot_Data_Index].Pin_State,
                    (uint8 *)&N_SRChargePumpPilot_Data.Data[load->Pilot_Data_Index].Pin_State);
                SRData__UpdateByte((uint8 *)&SRChargePumpPilot_Data.Data[load->Pilot_Data_Index].Pin_State,
                    (uint8 *)&N_SRChargePumpPilot_Data.Data[load->Pilot_Data_Index].Pin_State,
                    (~load->Hi_Low_Activation_Bitmap) & 0x01);
                Gpio__PinWrite(port, pin, (BOOL_TYPE) SRChargePumpPilot_Data.Data[load->Pilot_Data_Index].Pin_State);
                Gpio__PinConfig(port, pin, vp->pin_config);
                result = PASS;
            }
        }
    }

    return (result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Update the requested state of the output controlled by this pilot instance.
 * This function must not set the output.
 * @param load = A pointer to the HBL_LOAD_TYPE structure with load and pilot instance information.
 * @param data = A pointer to memory that contains the new requested pilot state.
 * @return PASS if the requested pilot state is successfully saved; otherwise FAIL.
 */
PASS_FAIL_TYPE SRChargePumpPilot__Request(HBL_LOAD_TYPE * load,void * data)
{
    PASS_FAIL_TYPE result = FAIL;

    if (load != NULL)
    {
        uint8 pilot_index = load->Pilot_Data_Index;

        SR_CHECK_BYTE(SRChargePumpPilot_Data.Allocation);

        if (load->Pilot_Data_Index < SRChargePumpPilot_Data.Allocation)
        {
            BOOL_TYPE drive_request = (BOOL_TYPE)OFF;
            if (*((uint8 *)data) > 0)
            {
                drive_request = (BOOL_TYPE)ON;
            }

            SRData__UpdateByte((uint8 *)&SRChargePumpPilot_Data.Data[pilot_index].Requested, (uint8 *)&N_SRChargePumpPilot_Data.Data[pilot_index].Requested, drive_request);
            result = PASS;
        }
    }

    return result;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Update the output controlled by this pilot instance based on previous requests.
 * @param load = A pointer to the HBL_LOAD_TYPE structure with load and pilot instance information.
 * @return PASS if the controlled output is successfully updated; otherwise FAIL.
 */
PASS_FAIL_TYPE SRChargePumpPilot__Process(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE result = FAIL;
    if (load != NULL)
    {
        uint8 pilot_index = load->Pilot_Data_Index;
        SR_CHECK_BYTE(SRChargePumpPilot_Data.Allocation);
        if (pilot_index < SRChargePumpPilot_Data.Allocation)
        {
            SRData__CheckByte((uint8 *)&SRChargePumpPilot_Data.Data[pilot_index].Requested, (uint8 *)&N_SRChargePumpPilot_Data.Data[pilot_index].Requested);
            SRData__UpdateByte((uint8 *)&SRChargePumpPilot_Data.Data[pilot_index].Processed, (uint8 *)&N_SRChargePumpPilot_Data.Data[pilot_index].Processed, SRChargePumpPilot_Data.Data[pilot_index].Requested);

            result = PASS;
        }
    }

    return result;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the requested state of the output controlled by this pilot instance.
 * This function does not necessarily return the current state of the output.
 * @param load = A pointer to the HBL_LOAD_TYPE structure with load and pilot instance information.
 * @return A pointer to data that describes the requested state of the output.
 */
void * SRChargePumpPilot__GetRequested(HBL_LOAD_TYPE * load)
{
    void* requested = NULL;
    if (load != NULL)
    {
        uint8 pilot_index = load->Pilot_Data_Index;
        SR_CHECK_BYTE(SRChargePumpPilot_Data.Allocation);
        if (pilot_index < SRChargePumpPilot_Data.Allocation)
        {
            SRData__CheckByte((uint8 *)&SRChargePumpPilot_Data.Data[pilot_index].Requested, (uint8 *)&N_SRChargePumpPilot_Data.Data[pilot_index].Requested);

            requested = &SRChargePumpPilot_Data.Data[pilot_index].Requested;
        }
    }
    return requested;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the state of the output controlled by this pilot instance.
 * @param load = A pointer to the HBL_LOAD_TYPE structure with load and pilot instance information.
 * @return A pointer to data that describes the state of the output.
 */
void * SRChargePumpPilot__GetProcessed(HBL_LOAD_TYPE * load)
{
    void* processed = NULL;
    if (load != NULL)
    {
        uint8 pilot_index = load->Pilot_Data_Index;
        SR_CHECK_BYTE(SRChargePumpPilot_Data.Allocation);
        if (pilot_index < SRChargePumpPilot_Data.Allocation)
        {
            SRData__CheckByte((uint8 *)&SRChargePumpPilot_Data.Data[pilot_index].Processed, (uint8 *)&N_SRChargePumpPilot_Data.Data[pilot_index].Processed);
            processed = &SRChargePumpPilot_Data.Data[load->Pilot_Data_Index].Processed;
        }
    }
    return processed;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This API been called every 1ms periodically by the HBL
 * @param load = A pointer to the HBL_LOAD_TYPE structure with load and pilot instance information.
 * @return PASS if the Pilot is called successfully or else fail
 */
PASS_FAIL_TYPE SRChargePumpPilot__Handler1ms(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE result = FAIL;
    if (load != NULL)
    {
        uint8 pilot_index = load->Pilot_Data_Index;
        uint8 virtual_pin = load->Pin_Ptr[0];

        SR_CHECK_BYTE(SRChargePumpPilot_Data.Allocation);

        if ((pilot_index < SRChargePumpPilot_Data.Allocation) &&
            (virtual_pin < VirtualPin__GetNumOf()) &&
            (VIRTUALPIN_TABLE[virtual_pin].port != PORT_NULL))
        {
            // Determine the port/pin information.
            const VIRTUALPIN_TYPE* vp = &VIRTUALPIN_TABLE[virtual_pin];
            GPIO_PORT_TYPE port = vp->port;
            uint8 pin = vp->pin;

            SRData__CheckByte((uint8 *)&SRChargePumpPilot_Data.Data[pilot_index].Processed, (uint8 *)&N_SRChargePumpPilot_Data.Data[pilot_index].Processed);
            SRData__CheckByte((uint8 *)&SRChargePumpPilot_Data.Data[pilot_index].Pin_State, (uint8 *)&N_SRChargePumpPilot_Data.Data[pilot_index].Pin_State);

            BOOL_TYPE previous_pin_state = SRChargePumpPilot_Data.Data[pilot_index].Pin_State;

            if (SRChargePumpPilot_Data.Data[pilot_index].Processed == ON)
            {
                SRData__UpdateByte((uint8 *)&SRChargePumpPilot_Data.Data[pilot_index].Pin_State, (uint8 *)&N_SRChargePumpPilot_Data.Data[pilot_index].Pin_State, ((SRChargePumpPilot_Data.Data[pilot_index].Pin_State ^ 1) & 0x01));
            }
            else
            {
                SRData__UpdateByte((uint8 *)&SRChargePumpPilot_Data.Data[pilot_index].Pin_State, (uint8 *)&N_SRChargePumpPilot_Data.Data[pilot_index].Pin_State, (~load->Hi_Low_Activation_Bitmap) & 0x01);
            }


            if (Gpio__CheckConfig(port, pin, vp->pin_config, previous_pin_state) == TRUE)
            {
                SRData__CheckByte((uint8 *)&SRChargePumpPilot_Data.Data[pilot_index].Pin_State, (uint8 *)&N_SRChargePumpPilot_Data.Data[pilot_index].Pin_State);
                Gpio__PinWrite(port, pin, SRChargePumpPilot_Data.Data[load->Pilot_Data_Index].Pin_State);
                result = PASS;
            }
            else
            {
                SRException__Queue(SREXCEPTION_GPIO_CONFIGURATION_CORRUPTION, load->Load_Id, vp->pin_config);
            }
        }
    }

    return result;
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


#endif

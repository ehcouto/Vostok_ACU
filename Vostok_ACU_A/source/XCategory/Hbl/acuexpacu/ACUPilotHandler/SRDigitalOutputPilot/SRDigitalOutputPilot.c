/**
 * @file
 * @defgroup    CLASS_B
 * @brief       Implements a Class B pilot that drives a single output pin.
 *
 * @copyright   Copyright 2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "SRSystemConfig.h"
#if (HBL_PILOT_NUM_SRDIGITALOUTPUT > 0)
#include "SRDigitalOutputPilot.h"

// -- Other Modules --
#include "Gpio.h"
#include "Hbl.h"
#include "SRData.h"
#include "SRException.h"
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    #include "SRTaskSequence.h"
#endif
#include "VirtualPin.h"


//  --- Private Declarations ------------------------------------------------------------------------------------------


// -- Private Structure Type Declarations --

//! Volatile data required for each instance of the pilot.
typedef struct SR_DIGITAL_OUTPUT_PILOT_INSTANCE_STRUCT
{
    //! The value requested for the digital output.
    uint8 Requested;

    //! The value accepted and processed for the digital output.
    uint8 Processed;

    //! The most recently written value to the digital output pin.
    uint8 Last_Pin_Value;
} SR_DIGITAL_OUTPUT_PILOT_INSTANCE_TYPE;

//! Volatile data required for the pilot module.
typedef struct SR_DIGITAL_OUTPUT_PILOT_DATA_STRUCT
{
    SR_DIGITAL_OUTPUT_PILOT_INSTANCE_TYPE Data[HBL_PILOT_NUM_SRDIGITALOUTPUT];
    //! Tracks the number of digital output pilot allocations.
    uint8 Allocation;
} SR_DIGITAL_OUTPUT_PILOT_DATA_TYPE;


//  --- Private Definitions -------------------------------------------------------------------------------------------


// -- Private Variable Definitions --

//! All the volatile data used by this module.
static SR_DIGITAL_OUTPUT_PILOT_DATA_TYPE Digital_Output;

//! An inverted copy of the volatile data used by this module for data integrity purposes.
static SR_DIGITAL_OUTPUT_PILOT_DATA_TYPE N_Digital_Output;


//  --- Private Function Prototypes -----------------------------------------------------------------------------------

static void DoPinUpdate(HBL_LOAD_TYPE* load, GPIO_PORT_TYPE port, uint8 pin);


//=====================================================================================================================
//  --- Public Functions ----------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Initialize all instances of this Pilot type.
 *  - Initialize static RAM used by the module.
 *  - Initialize any corrupted NVData used by the module.
 */
void SRDigitalOutputPilot__Initialize(void)
{
    SR_FILL(Digital_Output, 0, sizeof(Digital_Output));

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_INIT, SRTASKSEQUENCE_APPL_INIT_SRDIGITALOUTPUT_PILOT);
    #endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Reserve memory for this pilot instance.
 * @return The index of the allocated Pilot.
 *         Return 0xFF if there is no space allocated for another Pilot of this type.
 */
uint8 SRDigitalOutputPilot__Allocate(void)
{
    uint8 pilot_index = 0xFF;
    SR_CHECK_BYTE(Digital_Output.Allocation);
    if (Digital_Output.Allocation < HBL_PILOT_NUM_SRDIGITALOUTPUT)
    {
        pilot_index = Digital_Output.Allocation;
        SR_SET_BYTE(Digital_Output.Allocation, Digital_Output.Allocation + 1);
    }
    return pilot_index;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Initialize this pilot instance.
 * @param load = A pointer to the HBL_LOAD_TYPE structure with load and pilot instance information.
 * @return PASS if the Pilot instance is successfully initialized; otherwise FAIL.
 */
PASS_FAIL_TYPE SRDigitalOutputPilot__InitializeInstance(HBL_LOAD_TYPE* load)
{
    PASS_FAIL_TYPE result = FAIL;
    if (load != NULL)
    {
        SR_CHECK_BYTE(Digital_Output.Allocation);
        uint8 virtual_pin = load->Pin_Ptr[0];
        if ((load->Pilot_Data_Index < Digital_Output.Allocation) &&
            (virtual_pin < VirtualPin__GetNumOf()) &&
            (VIRTUALPIN_TABLE[virtual_pin].port != PORT_NULL))
        {
            const VIRTUALPIN_TYPE* vp = &VIRTUALPIN_TABLE[virtual_pin];
            GPIO_PORT_TYPE port = vp->port;
            uint8 pin = vp->pin;
            if (Gpio__Validate(port, pin) == PASS)
            {
                DoPinUpdate(load, port, pin);
                Gpio__PinConfig(port, pin, vp->pin_config);
                result = PASS;
            }
        }
    }
    return result;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Verify that the RAM used by this module is not corrupted.
 * Report a sequence step to SRTaskSequence.
 */
void SRDigitalOutputPilot__CheckDataIntegrity(void)
{
    SR_CHECK_BYTE(Digital_Output.Allocation);
    SR_CHECK_ARRAY(Digital_Output.Data);

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_SRDIGITALOUTPUTPILOT_CHECKS);
    #endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Update the requested state of the output controlled by this pilot instance.
 * This function must not set the output.
 * @param load = A pointer to the HBL_LOAD_TYPE structure with load and pilot instance information.
 * @param data = A pointer to memory that contains the new requested pilot state.
 * @return PASS if the requested pilot state is successfully saved; otherwise FAIL.
 */
PASS_FAIL_TYPE SRDigitalOutputPilot__Request(HBL_LOAD_TYPE* load, void* data)
{
    PASS_FAIL_TYPE result = FAIL;
    if (load != NULL)
    {
        uint8 pilot_index = load->Pilot_Data_Index;
        SR_CHECK_BYTE(Digital_Output.Allocation);
        if (pilot_index < Digital_Output.Allocation)
        {
            SR_SET_BYTE(Digital_Output.Data[pilot_index].Requested, *((uint8*)data));
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
PASS_FAIL_TYPE SRDigitalOutputPilot__Process(HBL_LOAD_TYPE* load)
{
    PASS_FAIL_TYPE result = FAIL;
    if (load != NULL)
    {
        uint8 pilot_index = load->Pilot_Data_Index;
        SR_CHECK_BYTE(Digital_Output.Allocation);
        if (pilot_index < Digital_Output.Allocation)
        {
            SR_CHECK_BYTE(Digital_Output.Data[pilot_index].Requested);
            SR_SET_BYTE(Digital_Output.Data[pilot_index].Processed, Digital_Output.Data[pilot_index].Requested);
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
void* SRDigitalOutputPilot__GetRequested(HBL_LOAD_TYPE* load)
{
    void* requested = NULL;
    if (load != NULL)
    {
        uint8 pilot_index = load->Pilot_Data_Index;
        SR_CHECK_BYTE(Digital_Output.Allocation);
        if (pilot_index < Digital_Output.Allocation)
        {
            SR_CHECK_BYTE(Digital_Output.Data[pilot_index].Requested);
            requested = &Digital_Output.Data[pilot_index].Requested;
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
void* SRDigitalOutputPilot__GetProcessed(HBL_LOAD_TYPE* load)
{
    void* processed = NULL;
    if (load != NULL)
    {
        uint8 pilot_index = load->Pilot_Data_Index;
        SR_CHECK_BYTE(Digital_Output.Allocation);
        if (pilot_index < Digital_Output.Allocation)
        {
            SR_CHECK_BYTE(Digital_Output.Data[pilot_index].Processed);
            processed = &Digital_Output.Data[pilot_index].Processed;
        }
    }
    return processed;
}


//------------------------------------------------------------------------------------------------
/**
 * Handle behaviors that must be implemented periodically by this pilot instance.
 * This function is called every 5ms.
 * @param load = A pointer to the HBL_LOAD_TYPE structure with load and pilot instance information.
 * @return PASS or FAIL.
 */
PASS_FAIL_TYPE SRDigitalOutputPilot__AsynchProcess(HBL_LOAD_TYPE* load)
{
    PASS_FAIL_TYPE result = FAIL;
    if (load != NULL)
    {
        uint8 pilot_index = load->Pilot_Data_Index;
        SR_CHECK_BYTE(Digital_Output.Allocation);
        if (pilot_index < Digital_Output.Allocation)
        {
            // Determine the port/pin information.
            uint8 virtual_pin = load->Pin_Ptr[0];
            const VIRTUALPIN_TYPE* vp = &VIRTUALPIN_TABLE[virtual_pin];
            GPIO_PORT_TYPE port = vp->port;
            uint8 pin = vp->pin;

            // Check the current configuration of the pin.
            SR_CHECK_BYTE(Digital_Output.Data[pilot_index].Last_Pin_Value);
            if (Gpio__CheckConfig(port, pin, vp->pin_config,
                                  (BOOL_TYPE)(Digital_Output.Data[pilot_index].Last_Pin_Value)) == TRUE)
            {
                DoPinUpdate(load, port, pin);
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
//  --- Private Functions ---------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Update the output pin based on the requested value.
 * Before updating, the function checks that related RAM is consistent.
 * Before updating, the function checks that no other function changed the output pin.
 * @param load = Data structure for the Pilot.
 * @param requested = The requested output value.
 * @return PASS if there were no problems updating the output pin; otherwise FAIL.
 */
static void DoPinUpdate(HBL_LOAD_TYPE* load, GPIO_PORT_TYPE port, uint8 pin)
{
    // Verify RAM integrity.
    uint8 pilot_index = load->Pilot_Data_Index;
    SR_CHECK_BYTE(Digital_Output.Data[pilot_index].Processed);

    // Set the new value for the pin.
    uint8 new_pin_value = Digital_Output.Data[pilot_index].Processed;
    new_pin_value ^= ~load->Hi_Low_Activation_Bitmap;
    new_pin_value &= 1;

    Gpio__PinWrite(port, pin, (BOOL_TYPE)new_pin_value);
    SR_SET_BYTE(Digital_Output.Data[pilot_index].Last_Pin_Value, new_pin_value);
}


#endif      // #if (HBL_PILOT_NUM_SRDIGITALOUTPUT > 0)

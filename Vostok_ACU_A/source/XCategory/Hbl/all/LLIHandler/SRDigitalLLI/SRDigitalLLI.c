/**
 *  @file       
 *
 *  @brief      Handles a Safety Relevant low level digital input.
 *
 *  @defgroup   CLASS_B
 *
@startuml{SRDigitalLLI_Class_Diagram.png}

        hide circle
        hide empty attributes
        hide empty methods
        skinparam classAttributeIconSize 0

    class SRDigitalLLI {
        + void SRDigitalLLI__Initialize(void);
        + uint8 SRDigitalLLI__Allocate(void);
        + PASS_FAIL_TYPE SRDigitalLLI__InitializeInstance(HBL_LLI_TYPE * input);
        + void * SRDigitalLLI__Get(HBL_LLI_TYPE * input);
        + BOOL_TYPE SRDigitalLLI__AsynchProcess(HBL_LLI_TYPE * input);
        }

    class SRSystemConfig {
        +SYSTEM_ENABLE_TASKSEQUENCE
        +HBL_LLI_NUM_SRDIGITAL
    }

    class Micro_Defs {
        +MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL
        +MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL
        +MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL
    }

    SRDigitalLLI ..> Gpio
    SRDigitalLLI ..> VirtualPin
    SRDigitalLLI ..> Micro_Defs
    SRDigitalLLI ..> SRData
    SRDigitalLLI ..> SRTaskSequence
    SRDigitalLLI ..> SRException
    SRDigitalLLI ..> SRSystemConfig

 @enduml
 *  @copyright  Copyright 2017 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SRSystemConfig.h"
#include "SRDigitalLLI.h"

#if (HBL_LLI_NUM_SRDIGITAL > 0)
#include "VirtualPin.h"
#include "Gpio.h"
#include "Micro_Defs.h"
#include "SRData.h"
#include "SRException.h"
#include "SRTaskSequence.h"

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------


//! Data for each SRDigital LLI
static SRDIGITALLLI_DATA_TYPE SRDigitalLLI_Data[HBL_LLI_NUM_SRDIGITAL];
//! Inverted memory for the Class B variable
static SRDIGITALLLI_DATA_TYPE N_SRDigitalLLI_Data[HBL_LLI_NUM_SRDIGITAL];

//! Keeps track of how many SRDigital LLIs have been allocated by the application
static uint8 SRDigitalLLI_Allocation;
//! Inverted memory for the Class B variable
static uint8 N_SRDigitalLLI_Allocation;

#define SRDIGITALLOWINPUT_CFG0_NREAD_MASK      0x07     //!< Mask to read the setting file configuration for the number of readings

#define SRDIGITALLOWINPUT_CFG1_INVERT          0x80     //!< Mask to read the setting file configuration for the "Invert" flag, which inverts the value read from the input
#define SRDIGITALLOWINPUT_CFG1_PULLUP          0x20     //!< Mask to read the setting file configuration for the "Pullup" flag; 1 = internal pullup resistor enabled, else no internal pullup resistor used

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Initializes the module SRDigitalLLI and its variables
 */
void SRDigitalLLI__Initialize(void)
{
    SRData__Fill(&SRDigitalLLI_Data, &N_SRDigitalLLI_Data, 0, sizeof(SRDigitalLLI_Data[0]) * HBL_LLI_NUM_SRDIGITAL);

    SR_SET_BYTE(SRDigitalLLI_Allocation, 0);

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_INIT, SRTASKSEQUENCE_APPL_INIT_SRDIGITAL_LLI);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Determines the destination of the data for the input.
 * @return  Index to a local buffer where the data will reside.
 */
uint8 SRDigitalLLI__Allocate(void)
{
    uint8 retval = HBL_INDEX_INVALID;

    if ((SR_CHECK_BYTE(SRDigitalLLI_Allocation) == SRDATA_OK) &&
        (SRDigitalLLI_Allocation < HBL_LLI_NUM_SRDIGITAL))
    {
        retval = SRDigitalLLI_Allocation;
        SR_SET_BYTE(SRDigitalLLI_Allocation, SRDigitalLLI_Allocation + 1);
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Initialize an instance of the input.
 * @param   input: pointer to various data for the LLI, managed by the Hbl module
 * @return  PASS if the instance of the LLI was successfully initialized, else FALSE
 */
PASS_FAIL_TYPE SRDigitalLLI__InitializeInstance(HBL_LLI_TYPE * input)
{
    GPIO_PIN_MODE_TYPE type;
    uint8 digitalcfg1;
    uint8 virtual_pin;
    PASS_FAIL_TYPE retval = FAIL;

    if ((SR_CHECK_BYTE(SRDigitalLLI_Allocation) == SRDATA_OK) &&
        (input->Data_Index < SRDigitalLLI_Allocation))
    {
        virtual_pin = input->Pin_Ptr[0];

        // Initialize the protected variables for this instance of the SRDigital LLI
        SRData__UpdateByte((uint8 *)(void *)&SRDigitalLLI_Data[input->Data_Index].value, (uint8 *)(void *)&N_SRDigitalLLI_Data[input->Data_Index].value, FALSE);
        SRData__UpdateByte((uint8 *)(void *)&SRDigitalLLI_Data[input->Data_Index].index, (uint8 *)(void *)&N_SRDigitalLLI_Data[input->Data_Index].index, 0);
        SRData__UpdateByte((uint8 *)(void *)&SRDigitalLLI_Data[input->Data_Index].level_cnt, (uint8 *)(void *)&N_SRDigitalLLI_Data[input->Data_Index].level_cnt, 0);

        // Grab the second byte of configuration data from the setting file
        if(input->Parameter_Ptr.Data != NULL)
        {
            digitalcfg1 = input->Parameter_Ptr.Data[1];
        }
        else
        {
            digitalcfg1 = 0;
        }

        // Determine if the input has an internal pullup resistor enabled, else it is just floating
        if (digitalcfg1 & SRDIGITALLOWINPUT_CFG1_PULLUP)
        {
            type = INPUT_PULLUP;
        }
        else
        {
            type = INPUT_FLOAT;
        }

        // Configure the gpio pin for the input
        Gpio__PinConfig(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin, type);

        // Verify the configuration of the pin was set as expected
        if(Gpio__CheckConfig(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin, type, FALSE) == TRUE)
        {
            retval = PASS;
        }
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 */
void SRDigitalLLI__Handler(void)
{
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    if (SR_CHECK_BYTE(SRDigitalLLI_Allocation) != SRDATA_OK)
    {
        SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, 0x00, 0xAB);
        // System will reset after this exception occurs
    }
    else
    {
        for (uint8 index = 0; index < HBL_LLI_NUM_SRDIGITAL; index++)
        {
            MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
            if (SRData__CheckArray((uint8 *)(void *)&SRDigitalLLI_Data[index], (uint8 *)(void *)&N_SRDigitalLLI_Data[index], sizeof(SRDigitalLLI_Data[0])) != SRDATA_OK)
            {
                SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, 0x01, index);
                // System will reset after this exception occurs
            }
            MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
        }
    }

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_SRDIGITAL_CHECK);
    #endif

}
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Gets the data for one instance of an input.
 * @param   input: pointer to various data for the LLI, managed by the Hbl module
 * @return  Pointer to the SRDigitalLLI_Data buffer that contains the current state of the instance of the SRDigital LLI.
 */
void * SRDigitalLLI__Get(HBL_LLI_TYPE * input)
{
    if ((SR_CHECK_BYTE(SRDigitalLLI_Allocation) == SRDATA_OK) &&
        (SRData__CheckArray((uint8 *)(void *)&SRDigitalLLI_Data[input->Data_Index], (uint8 *)(void *)&N_SRDigitalLLI_Data[input->Data_Index], sizeof(SRDigitalLLI_Data[0])) == SRDATA_OK) &&
        (input->Data_Index < SRDigitalLLI_Allocation))
    {
        return ((void *)&SRDigitalLLI_Data[input->Data_Index].value);
    }
    else
    {
        return (NULL);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Processing of the input state.
 * @param   input: pointer to various data for the LLI managed by the HblHmi module
 * @return  Always TRUE
 */
BOOL_TYPE SRDigitalLLI__AsynchProcess(HBL_LLI_TYPE * input)
{
    SRDIGITALLLI_DATA_TYPE data;
    BOOL_TYPE retval;
    BOOL_TYPE pinval;
    uint8 digitalcfg0;
    uint8 digitalcfg1;
    uint8 virtual_pin;
    uint8 nsamples;
    GPIO_PIN_MODE_TYPE gpio_type;

    retval = FALSE;
    digitalcfg0 = 0;
    digitalcfg1 = 0;

    if ((SR_CHECK_BYTE(SRDigitalLLI_Allocation) == SRDATA_OK) &&
        (input->Data_Index < SRDigitalLLI_Allocation))
    {
        virtual_pin = input->Pin_Ptr[0];

        // Grab the configuration from the setting file
        if(input->Parameter_Ptr.Data != NULL)
        {
            digitalcfg0 = input->Parameter_Ptr.Data[0];
            digitalcfg1 = input->Parameter_Ptr.Data[1];
        }

        // Determine if the input has an internal pull up resistor enabled, else it is just floating
        if (digitalcfg1 & SRDIGITALLOWINPUT_CFG1_PULLUP)
        {
            gpio_type = INPUT_PULLUP;
        }
        else
        {
            gpio_type = INPUT_FLOAT;
        }

        if (SRData__CheckArray(&SRDigitalLLI_Data[input->Data_Index], &N_SRDigitalLLI_Data[input->Data_Index],  sizeof(SRDigitalLLI_Data[0])) == SRDATA_OK)
        {
            data = SRDigitalLLI_Data[input->Data_Index];

            // Verify the configuration of the pin has not been changed before reading the state of the pin
            if(Gpio__CheckConfig(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin, gpio_type, FALSE) == FALSE)
            {
                SRException__Queue(SREXCEPTION_GPIO_CONFIGURATION_CORRUPTION, VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin);
            }
            else
            {
                // Get the current state of the input from the gpio pin
                pinval = Gpio__PinRead(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin);

                // Setup the number of samples that will be required before updating the current state of the SRDigital LLI
                nsamples = 1 << (digitalcfg0 & SRDIGITALLOWINPUT_CFG0_NREAD_MASK);

                // Inverted logic flag enabled - swap TRUE to FALSE
                if (digitalcfg1 & SRDIGITALLOWINPUT_CFG1_INVERT)
                {
//                    pinval ^= TRUE;
                    pinval = (BOOL_TYPE)(pinval ^ TRUE);
                }

                if (nsamples == 1)                  // If only 1 sample is required, update the current state of the SRDigital LLI
                {
                    data.value = pinval;
                    retval = TRUE;
                }
                else                                // If more than 1 sample is required, manage multiple readings
                {
                    data.index++;                   // Increment the index each time a new sample is taken

                    // Level detection
                    if (pinval == TRUE)             // Increment the level counter each time the sample is high
                    {
                        data.level_cnt++;
                    }

                    if (data.index >= nsamples)     // Wait until the required number of samples has been taken before updating the current state of the SRDigital LLI
                    {
                        if (data.level_cnt >= (nsamples - (nsamples / 4)))
                        {
                            data.value = TRUE;      // Update the current state of the SRDigital LLI to "TRUE" if at least 75% of the samples were high
                        }
                        else if (data.level_cnt <= (nsamples / 4))
                        {
                            data.value = FALSE;     // Update the current state of the SRDigital LLI to "FALSE" if 25% or less of the samples were low
                        }
                        else
                        {
                            /* Keep old state if the input signal is not steady based on the previous logic*/
                        }

                        // Reset variables to manage multiple readings
                        data.level_cnt = 0;
                        data.index = 0;

                        retval = TRUE;
                    }
                }

                // Update protected variables that holds the data for the input
                SRData__UpdateArray(&SRDigitalLLI_Data[input->Data_Index], &N_SRDigitalLLI_Data[input->Data_Index], &data, sizeof(SRDigitalLLI_Data[0]));
            }
        }
    }

    return (retval);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#endif

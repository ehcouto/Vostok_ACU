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
 *  $Header: DigitalLowInput.c 1.5 2015/07/24 11:27:57EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/24 11:27:57EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "DigitalLLI.h"

#if (HBL_LLI_NUM_DIGITAL > 0)
#include "VirtualPin.h"
#include "Gpio.h"
#include "string.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

typedef struct
{
    BOOL_TYPE value;              /* Current Digital sample value */
    uint8 index;               /* Number of Samples collected */
    uint8 level_cnt;
} DIGITALLLI_DATA_TYPE;

static DIGITALLLI_DATA_TYPE DigitalLLI_Data[HBL_LLI_NUM_DIGITAL];
static uint8 DigitalLLI_Allocation;

#define DIGITALLOWINPUT_CFG0_NREAD_MASK      0x07
#define DIGITALLOWINPUT_CFG0_AUTO_MASK       0x80

#define DIGITALLOWINPUT_CFG1_INVERT          0x80
#define DIGITALLOWINPUT_CFG1_PULLUP          0x20

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      It Initializes the module DigitalLowLevel and its variables
 */
void DigitalLLI__Initialize(void)
{
    memset(&DigitalLLI_Data,0,sizeof(DigitalLLI_Data));
    DigitalLLI_Allocation = 0;
}



//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @return
 */
uint8 DigitalLLI__Allocate(void)
{
    uint8 retval;
    retval = 0xFF;
    if (DigitalLLI_Allocation < HBL_LLI_NUM_DIGITAL)
    {
        retval = DigitalLLI_Allocation;
        DigitalLLI_Allocation++;

    }
    return (retval);
}
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief
 *
 */
PASS_FAIL_TYPE DigitalLLI__InitializeInstance(HBL_LLI_TYPE * input)
{
    DIGITALLLI_DATA_TYPE *data;
    GPIO_PIN_MODE_TYPE type;
    uint8 digitalcfg1;
    uint8 virtual_pin;

    if (input->Data_Index < DigitalLLI_Allocation)
    {

         type = INPUT_FLOAT;
         virtual_pin = input->Pin_Ptr[0];
         digitalcfg1 = 0;


         data = &DigitalLLI_Data[input->Data_Index];
         data->value = FALSE;
         data->index = 0;
         data->level_cnt = 0;

         if(input->Parameter_Ptr.Data != NULL)
         {
             digitalcfg1 = input->Parameter_Ptr.Data[1];
         }

         if (digitalcfg1 & DIGITALLOWINPUT_CFG1_PULLUP)
         {
             type = INPUT_PULLUP;
         }

         Gpio__PinConfig(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin, type);

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
 * @param input
 * @param data
 * @return
 */
void * DigitalLLI__Get(HBL_LLI_TYPE * input)
{
    if (input->Data_Index < DigitalLLI_Allocation)
    {
        return ((void *) &DigitalLLI_Data[input->Data_Index].value);
    }
    else
    {
        return (NULL);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param input
 * @return
 */
BOOL_TYPE DigitalLLI__AsynchProcess(HBL_LLI_TYPE * input)
{
    DIGITALLLI_DATA_TYPE *data;
    BOOL_TYPE retval;
    BOOL_TYPE pinval;
    uint8 digitalcfg0;
    uint8 digitalcfg1;
    uint8 virtual_pin;
    uint8 nsamples;

    retval = FALSE;
    digitalcfg0 = 0;
    digitalcfg1 = 0;

    if (input->Data_Index < DigitalLLI_Allocation)
    {
        virtual_pin = input->Pin_Ptr[0];

        if(input->Parameter_Ptr.Data != NULL)
        {
            digitalcfg0 = input->Parameter_Ptr.Data[0];
            digitalcfg1 = input->Parameter_Ptr.Data[1];
        }

        data = &DigitalLLI_Data[input->Data_Index];
        pinval = Gpio__PinRead(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin);

        nsamples = 1 << (digitalcfg0 & DIGITALLOWINPUT_CFG0_NREAD_MASK);

        // Inverted logic flag enabled - swap TRUE to FALSE
        if (digitalcfg1 & DIGITALLOWINPUT_CFG1_INVERT)
        {
            pinval ^= TRUE;
        }

        if (nsamples == 1)
        {
            data->value = pinval;
            retval = TRUE;
        }
        else
        {
            data->index++;

            // Level detection
            if (pinval == TRUE)
            {
                data->level_cnt++;
            }

            if (data->index >= nsamples)
            {
                if (data->level_cnt >= (nsamples - (nsamples / 4)))
                {
                    data->value = TRUE;
                }
                else if (data->level_cnt <= (nsamples / 4))
                {
                    data->value = FALSE;
                }
                else
                {
                    /* Keep old value */
                }

                data->level_cnt = 0;
                data->index = 0;
                retval = TRUE;
            }
        }
    }


    return (retval);
}



//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


#endif

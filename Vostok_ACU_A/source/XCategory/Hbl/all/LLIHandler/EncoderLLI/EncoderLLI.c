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
 *  $Header: EncoderLowInput.c 1.5 2016/03/16 11:27:57EDT G.Ottaviani (OTTAVG) Exp  $
 *
 *  @copyright  Copyright 2016-$Date: 2016/03/16 11:27:57EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"
#include "EncoderLLI.h"

#if (HBL_LLI_NUM_ENCODER > 0)
#include "Gpio.h"
#include <string.h>
#include "VirtualPin.h"

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

typedef enum
{
	ENCODER_MACHINE_RESET = 0,
	ENCODER_MACHINE_WAIT,
	ENCODER_MACHINE_DONE
} ENCODER_MACHINE_TYPE;

typedef struct
{
	uint8 Pin_CH1_Old;
	uint8 Pin_CH2_Old;
	uint8 Latch_CHx   				:1;
	uint8 Rotation_Type    			:2;  // Store temporary ClockWise versus.
	uint8 Machine_Enc_Filter_Steps  :5;
} ARRAY_PINS_TRANSITION_TYPE;

static ARRAY_PINS_TRANSITION_TYPE  Array_Pin_CH_Old [HBL_LLI_NUM_ENCODER];

typedef struct
{
	uint8 Filter_Sample;
	uint8 Read_Attempt;
	uint8 R_flag:1;
	uint8 Channel:1;
} ENCODER_PARAMETER_TYPE;

static ENCODER_PARAMETER_TYPE Encoder_Input_Param [HBL_LLI_NUM_ENCODER];

static ENCODER_DATA_TYPE EncoderLLI_Data [HBL_LLI_NUM_ENCODER];

static uint8 EncoderLLI_Allocation;

#define ENCODERLOWINPUT_CFG0_NREAD_MASK      0x07
#define ENCODERLOWINPUT_CFG0_AUTO_MASK       0x80

#define ENCODERLOWINPUT_CFG1_INVERT          0x80
#define ENCODERLOWINPUT_CFG1_RFLAG           0x40
#define ENCODERLOWINPUT_CFG1_PULLUP          0x20

#define TYPE_ROTATION_1						1
#define TYPE_ROTATION_2						2

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void ResetArrayPinChOld (HBL_LLI_TYPE * input, uint8 pinvalLoc, uint8 pinval2Loc);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      It Initializes the module DigitalLowLevel and its variables
 */

void EncoderLLI__Initialize (void)
{
    memset(&EncoderLLI_Data,0,sizeof(EncoderLLI_Data));
    EncoderLLI_Allocation = 0;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @return
 */
uint8 EncoderLLI__Allocate (void)
{
    uint8 retval = HBL_ALLOCATION_INVALID;

    if (EncoderLLI_Allocation < HBL_LLI_NUM_ENCODER)
    {
        retval = EncoderLLI_Allocation;
        EncoderLLI_Allocation++;
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief
 *
 */
PASS_FAIL_TYPE EncoderLLI__InitializeInstance (HBL_LLI_TYPE * input)
{
    GPIO_PIN_MODE_TYPE type;
    uint8 virtual_pin;
    uint8 virtual_pin2;
    uint8 pinval;
    uint8 pinval2;
    uint8 digitalcfg1;

    if (input->Data_Index < EncoderLLI_Allocation)
    {
		type = INPUT_FLOAT;

		virtual_pin  = input->Pin_Ptr[0];
		virtual_pin2 = input->Pin_Ptr[1];

		digitalcfg1 = input->Parameter_Ptr.Data[1];

		if (digitalcfg1 & ENCODERLOWINPUT_CFG1_PULLUP)
		{   // This Flag is important in case we have Encoder with internal PullUp you don't have check.
			type = INPUT_PULLUP;
		}

		Gpio__PinConfig(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin, type);
		Gpio__PinConfig(VIRTUALPIN_TABLE[virtual_pin2].port, VIRTUALPIN_TABLE[virtual_pin2].pin, type);

		pinval  = Gpio__PinRead(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin);
		pinval2 = Gpio__PinRead(VIRTUALPIN_TABLE[virtual_pin2].port, VIRTUALPIN_TABLE[virtual_pin2].pin);

		Array_Pin_CH_Old [input->Data_Index].Pin_CH1_Old = pinval;
		Array_Pin_CH_Old [input->Data_Index].Pin_CH2_Old = pinval2;
		Array_Pin_CH_Old [input->Data_Index].Latch_CHx  = 0;
		Array_Pin_CH_Old [input->Data_Index].Rotation_Type 	 = 0;
		Array_Pin_CH_Old [input->Data_Index].Machine_Enc_Filter_Steps = ENCODER_MACHINE_RESET;

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
void * EncoderLLI__Get (HBL_LLI_TYPE * input)
{
    if (input->Data_Index < EncoderLLI_Allocation)
    {
        return ((void *) &EncoderLLI_Data[input->Data_Index]);
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
BOOL_TYPE EncoderLLI__AsynchProcess (HBL_LLI_TYPE * input)
{
	BOOL_TYPE retval;
    uint8 pinval;
    uint8 pinval2;
    uint8 digitalcfg0;
    uint8 digitalcfg1;
    uint8 virtual_pin;
    uint8 virtual_pin2;
    uint8 nsamples;

    retval = FALSE;

    if (input->Data_Index < EncoderLLI_Allocation)
    {
        virtual_pin = input->Pin_Ptr[0];
        virtual_pin2 = input->Pin_Ptr[1];

        digitalcfg0 = input->Parameter_Ptr.Data[0];
        digitalcfg1 = input->Parameter_Ptr.Data[1];

        pinval = Gpio__PinRead(VIRTUALPIN_TABLE[virtual_pin].port, VIRTUALPIN_TABLE[virtual_pin].pin);
        pinval2 = Gpio__PinRead(VIRTUALPIN_TABLE[virtual_pin2].port, VIRTUALPIN_TABLE[virtual_pin2].pin);

        nsamples = 1 << (digitalcfg0 & ENCODERLOWINPUT_CFG0_NREAD_MASK);

        Encoder_Input_Param[input->Data_Index].Filter_Sample = nsamples;

        // Inverted logic flag enabled - swap TRUE to FALSE
        if (digitalcfg1 & ENCODERLOWINPUT_CFG1_INVERT)
        {
            pinval ^= TRUE;
            pinval2 ^= TRUE;
        }

        if (digitalcfg1 & ENCODERLOWINPUT_CFG1_RFLAG)
        {
            Encoder_Input_Param[input->Data_Index].R_flag = 1;
        }

        if ((Array_Pin_CH_Old[input->Data_Index].Pin_CH1_Old != pinval) &&
            (Array_Pin_CH_Old[input->Data_Index].Pin_CH2_Old == pinval2) &&
            (Array_Pin_CH_Old[input->Data_Index].Latch_CHx == 0) )
        {
            Array_Pin_CH_Old[input->Data_Index].Machine_Enc_Filter_Steps = ENCODER_MACHINE_WAIT;
            Array_Pin_CH_Old[input->Data_Index].Rotation_Type = TYPE_ROTATION_1;
            Encoder_Input_Param[input->Data_Index].Read_Attempt++;

            if (Encoder_Input_Param[input->Data_Index].Read_Attempt >= Encoder_Input_Param[input->Data_Index].Filter_Sample)
            {
                Array_Pin_CH_Old[input->Data_Index].Pin_CH1_Old = pinval;
                Array_Pin_CH_Old[input->Data_Index].Latch_CHx = 1;
                Encoder_Input_Param[input->Data_Index].Read_Attempt = 0;
            }

            retval = FALSE;
	    }
        else if ((Array_Pin_CH_Old[input->Data_Index].Pin_CH2_Old != pinval2) &&
                 (Array_Pin_CH_Old[input->Data_Index].Pin_CH1_Old == pinval) &&
                 (Array_Pin_CH_Old[input->Data_Index].Latch_CHx == 0))
        {
            Array_Pin_CH_Old[input->Data_Index].Machine_Enc_Filter_Steps = ENCODER_MACHINE_WAIT;

            Array_Pin_CH_Old[input->Data_Index].Rotation_Type = TYPE_ROTATION_2;

            Encoder_Input_Param[input->Data_Index].Read_Attempt++;

            if (Encoder_Input_Param[input->Data_Index].Read_Attempt >= Encoder_Input_Param[input->Data_Index].Filter_Sample)
            {
                Array_Pin_CH_Old[input->Data_Index].Pin_CH2_Old = pinval2;
                Array_Pin_CH_Old[input->Data_Index].Latch_CHx = 1;
                Encoder_Input_Param[input->Data_Index].Read_Attempt = 0;
            }

            retval = FALSE;
        }
        else
        {
            ResetArrayPinChOld(input, pinval, pinval2);
        }

        if ((Array_Pin_CH_Old[input->Data_Index].Machine_Enc_Filter_Steps == ENCODER_MACHINE_WAIT) &&
            (Array_Pin_CH_Old[input->Data_Index].Rotation_Type == TYPE_ROTATION_1) &&
            (pinval == pinval2))
        {
            Array_Pin_CH_Old[input->Data_Index].Machine_Enc_Filter_Steps = ENCODER_MACHINE_DONE;
            Encoder_Input_Param[input->Data_Index].Channel = 1;

            Array_Pin_CH_Old[input->Data_Index].Pin_CH1_Old = pinval;
            Array_Pin_CH_Old[input->Data_Index].Pin_CH2_Old = pinval2;
            Array_Pin_CH_Old[input->Data_Index].Latch_CHx = 0;
            Encoder_Input_Param[input->Data_Index].Read_Attempt = 0;
        }
        else if ((Array_Pin_CH_Old[input->Data_Index].Machine_Enc_Filter_Steps == ENCODER_MACHINE_WAIT) &&
                 (Array_Pin_CH_Old[input->Data_Index].Rotation_Type == TYPE_ROTATION_2) &&
                 (pinval == pinval2))
        {
            Array_Pin_CH_Old[input->Data_Index].Machine_Enc_Filter_Steps = ENCODER_MACHINE_DONE;
            Encoder_Input_Param[input->Data_Index].Channel = 0;

            Array_Pin_CH_Old[input->Data_Index].Pin_CH1_Old = pinval;
            Array_Pin_CH_Old[input->Data_Index].Pin_CH2_Old = pinval2;
            Array_Pin_CH_Old[input->Data_Index].Latch_CHx = 0;
            Encoder_Input_Param[input->Data_Index].Read_Attempt = 0;
        }

        if (Array_Pin_CH_Old[input->Data_Index].Machine_Enc_Filter_Steps == ENCODER_MACHINE_DONE)
        {
            EncoderLLI_Data[input->Data_Index].Pending = 1;

            if (Encoder_Input_Param[input->Data_Index].R_flag == TRUE)
            {
                EncoderLLI_Data[input->Data_Index].Rotation = ~Encoder_Input_Param[input->Data_Index].Channel;
            }
            else
            {
                EncoderLLI_Data[input->Data_Index].Rotation = Encoder_Input_Param[input->Data_Index].Channel;
            }

            ResetArrayPinChOld(input, pinval, pinval2);

            retval = TRUE;
        }
        else
        {
            ResetArrayPinChOld(input, pinval, pinval2);
            retval = FALSE;
        }
    }

    return (retval);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @param   input
 * @param   pinvalLoc
 * @param   pinval2Loc
 */
static void ResetArrayPinChOld (HBL_LLI_TYPE * input, uint8 pinvalLoc, uint8 pinval2Loc)
{
	Array_Pin_CH_Old[input->Data_Index].Machine_Enc_Filter_Steps = ENCODER_MACHINE_RESET;
	Array_Pin_CH_Old[input->Data_Index].Rotation_Type = 0;
	Array_Pin_CH_Old[input->Data_Index].Pin_CH1_Old = pinvalLoc;
	Array_Pin_CH_Old[input->Data_Index].Pin_CH2_Old = pinval2Loc;
	Array_Pin_CH_Old[input->Data_Index].Latch_CHx = 0;
	Encoder_Input_Param[input->Data_Index].Read_Attempt = 0;
}

#endif

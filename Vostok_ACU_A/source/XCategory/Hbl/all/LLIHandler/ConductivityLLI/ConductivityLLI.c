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
 *  $Header: ConductivityLLI.c 1.9 2015/08/10 15:48:11EDT Dasar Hanumanth (DASARHB) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/08/10 15:48:11EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "ConductivityLLI.h"

#if (HBL_LLI_NUM_CONDUCTIVITY > 0)
#include "VirtualPin.h"
#include "Gpio.h"
#include "string.h"
#include "Timer.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define CONDUCTIVITYLLI_CFG0_PULLUP                             0x20
#define CONDUCTIVITYLLI_CFG1_AUTO                               0x80

#define CONDUCTIVITYLLI_CFG1_N_READ_MASK                        0x07

#define CONDUCTIVITY_VALID_FREQUENCY                            200
#define CONDUCTIVITY_FREQUENCY_RESOLUTION_PERCENTAGE            10
#define CONDUCTIVITYLLI_N_READ                                  100

//Valid Duty boundaries in percentage
#define CONDUCTIVITY_VALID_MINIMUM_DUTY                         850
#define CONDUCTIVITY_VALID_MAXIMUM_DUTY                         8500
#define CONDUCTIVITY_THRESHOLD_DUTY                             3800

#define CONDUCTIVITY_VALID_MINIMUM_FREQUENCY                    (CONDUCTIVITY_VALID_FREQUENCY - ((CONDUCTIVITY_VALID_FREQUENCY * CONDUCTIVITY_FREQUENCY_RESOLUTION_PERCENTAGE)/100))
#define CONDUCTIVITY_VALID_MAXIMUM_FREQUENCY                    (CONDUCTIVITY_VALID_FREQUENCY + ((CONDUCTIVITY_VALID_FREQUENCY * CONDUCTIVITY_FREQUENCY_RESOLUTION_PERCENTAGE)/100))



static CONDUCTIVITY_DATA_TYPE ConductivityLLI_Data[HBL_LLI_NUM_CONDUCTIVITY];
static uint8 ConductivityLLI_Allocation;
static TIMER_PARAMETER_TYPE ConductivityLLI_Timer_Parameter[HBL_LLI_NUM_CONDUCTIVITY];


//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void InitConductivityData(CONDUCTIVITY_DATA_TYPE *data);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      It Initializes the module DigitalLowLevel and its variables
 */
void ConductivityLLI__Initialize(void)
{
    uint8 index;
    for (index=0; index< HBL_LLI_NUM_CONDUCTIVITY; index++)
    {
        InitConductivityData(&ConductivityLLI_Data[index]);
    }
    ConductivityLLI_Allocation = 0;
}



//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @return
 */
uint8 ConductivityLLI__Allocate(void)
{
    uint8 retval;
    retval = 0xFF;
    if (ConductivityLLI_Allocation < HBL_LLI_NUM_CONDUCTIVITY)
    {
        retval = ConductivityLLI_Allocation;
        ConductivityLLI_Allocation++;

    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module DigitalLowLevel and its variables
 *
 */
PASS_FAIL_TYPE ConductivityLLI__InitializeInstance(HBL_LLI_TYPE * input)
{
    uint8 virtual_pin;

    if (input->Data_Index < HBL_LLI_NUM_CONDUCTIVITY)
    {
        virtual_pin = input->Pin_Ptr[0];

        ConductivityLLI_Timer_Parameter[input->Data_Index].Timer      = (TIMER_TYPE)VIRTUALPIN_TABLE[virtual_pin].device;
        ConductivityLLI_Timer_Parameter[input->Data_Index].Channel    = (TIMER_CHANNEL_TYPE)VIRTUALPIN_TABLE[virtual_pin].param0;
        ConductivityLLI_Timer_Parameter[input->Data_Index].Mode       = TIMER_MODE_INPUT_CAPTURE;
        ConductivityLLI_Timer_Parameter[input->Data_Index].Port       = VIRTUALPIN_TABLE[virtual_pin].port;
        ConductivityLLI_Timer_Parameter[input->Data_Index].Pin        = VIRTUALPIN_TABLE[virtual_pin].pin;
        ConductivityLLI_Timer_Parameter[input->Data_Index].Pin_ReMap  = VIRTUALPIN_TABLE[virtual_pin].pin_remap;
        ConductivityLLI_Timer_Parameter[input->Data_Index].Polarity   = TIMER_POLARITY_ACTIVE_HIGH;

        Timer__Config(&ConductivityLLI_Timer_Parameter[input->Data_Index]);
        Timer__CaptureStart(&ConductivityLLI_Timer_Parameter[input->Data_Index], TIMER_CAPTURE_CYCLE, TIMER_CAPTUREON_RISING_EDGE);

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
void * ConductivityLLI__Get(HBL_LLI_TYPE * input)
{
    if (input->Data_Index < HBL_LLI_NUM_CONDUCTIVITY)
    {
        return ((void *)&ConductivityLLI_Data[input->Data_Index]);
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
BOOL_TYPE ConductivityLLI__ICProcess(HBL_LLI_TYPE * input)
{
    CONDUCTIVITY_DATA_TYPE *data;
    BOOL_TYPE retval;
    uint8 virtual_pin;
    uint8 n_read_cfg;
    uint32 frequency;
    uint16 duty;

    retval = FALSE;

    if (input->Data_Index < HBL_LLI_NUM_CONDUCTIVITY)
    {
        virtual_pin = input->Pin_Ptr[0];

        //TODO: the below number shall come from the editor tool
        n_read_cfg = CONDUCTIVITYLLI_N_READ;
        //n_read_cfg          = 1 << (input->Parameter_Ptr.Data[1] & CONDUCTIVITYLLI_CFG1_N_READ_MASK);

        data = &ConductivityLLI_Data[input->Data_Index];

        //if its the new sampling
        if(data->Private_Current_Count >= n_read_cfg)
        {
            //Copy previous sampling data
            data->Average_Duty              = (uint16)data->Private_Duty.Filtered_Value;
            data->Maximum_Duty              = data->Private_Maximum_Duty;
            data->Valid_Sample_Count        = data->Private_Valid_Sample_Count;
            data->InValid_Sample_Count      = data->Private_Invalid_Sample_Count;
            data->Threshold_Sample_Count    = data->Private_Threshold_Sample_Count;

            //new readings available
            data->Reading_Sync_Count++;

            //Prepare for reading new sampling
            InitConductivityData(data);

            //Data ready after the filter
            retval = TRUE;
        }

        data->Private_Current_Count++;

        frequency = Timer__GetCaptureFrequcncy(&ConductivityLLI_Timer_Parameter[input->Data_Index]);

        //validate the frequency
        if ((frequency >= CONDUCTIVITY_VALID_MINIMUM_FREQUENCY) &&
            (frequency <= CONDUCTIVITY_VALID_MAXIMUM_FREQUENCY))
        {
            duty = Timer__GetCaptureDuty(&ConductivityLLI_Timer_Parameter[input->Data_Index]);

            //validate the duty
            if ((duty >= CONDUCTIVITY_VALID_MINIMUM_DUTY) &&
                (duty <= CONDUCTIVITY_VALID_MAXIMUM_DUTY))
            {
                data->Private_Valid_Sample_Count++;
                Filter__Update(&data->Private_Duty, (FILTER_DATA_TYPE)duty, FILTER_IIR);

                if (duty > CONDUCTIVITY_THRESHOLD_DUTY)
                {
                    data->Private_Threshold_Sample_Count++;
                }

                if (duty > data->Private_Maximum_Duty)
                {
                    data->Private_Maximum_Duty = duty;
                }
            }
            else
            {
                data->Private_Invalid_Sample_Count++;
            }
        }
        else
        {
            data->Private_Invalid_Sample_Count++;
        }

    }



    return (retval);
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

static void InitConductivityData(CONDUCTIVITY_DATA_TYPE *data)
{
    data->Private_Duty.Inverted_Alpha                   = 2;
    Filter__Initialize(&data->Private_Duty, CONDUCTIVITY_VALID_MINIMUM_DUTY, FILTER_IIR);
    data->Private_Maximum_Duty                          = 0;
    data->Private_Valid_Sample_Count                    = 0;
    data->Private_Invalid_Sample_Count                  = 0;
    data->Private_Threshold_Sample_Count                = 0;
    data->Private_Current_Count                         = 0;
}


#endif

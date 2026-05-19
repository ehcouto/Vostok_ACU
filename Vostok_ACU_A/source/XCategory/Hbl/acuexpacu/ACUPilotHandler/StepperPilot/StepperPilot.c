/**
 *  @file       
 *
 *  @brief      Implements Stepper Pilot
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//  --- Include Files -------------------------------------------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "StepperPilot.h"

// -- Other Modules --
#include "Gpio.h"
#include "Micro_Defs.h"
#include "SettingFile.h"
#include "VirtualPin.h"

#include <String.h>

#if (HBL_PILOT_NUM_STEPPER > 0)
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//! Stepper pilot setting file parameters type definition.
typedef struct STEPPER_PARAMETERS_STRUCT
{
    uint8 Number_Of_Sequences;
    uint8 Sequences[8];
} STEPPER_PARAMETERS_TYPE;

//! Stepper pilot data type definition.
PACKED typedef struct STEPPER_PILOT_DATA_STRUCT
{
    STEPPER_PILOT_REQUEST_TYPE Requested;
    STEPPER_PILOT_REQUEST_TYPE Processed;
    STEPPER_PARAMETERS_TYPE    Parameters;          //!< Stepper pilot parameters from the setting file.
    COMPLETE_TYPE Status;                           //!< Status of request.
    uint16 Step;                                    //!< Current Step.
    uint16 Pulse_Width;                             //!< Requested step pulse width (speed).
    uint16 Pulse_Counter;                           //!< Counter to control pulse width.
    uint8  Stepper_Array_Index;                     //!< Index into Sequence Array.
    BOOL_TYPE Request_Updated;                      //!< Flag to check for updated request.
} STEPPER_PILOT_DATA_TYPE;

//! Tracks the number of ice dispenser door load allocations.
static uint8 Stepper_Pilot_Allocation;

//! Stepper pilot data array. One entry per stepper instance.
static STEPPER_PILOT_DATA_TYPE Stepper_Pilot_Data[HBL_PILOT_NUM_STEPPER];

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void StepperPilotReadParameters(HBL_LOAD_TYPE *load);

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module StepperPilot and its variables
 */
void StepperPilot__Initialize(void)
{
    Stepper_Pilot_Allocation = 0;
    memset(Stepper_Pilot_Data, 0, sizeof(Stepper_Pilot_Data));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Allocate pilot instance for stepper.
 *
 *  @return     uint8 result - Allocation result.
 *  @retval     0-254 - Pilot allocation instance.
 *  @retval     0xFF - Allocation failed.
 */
uint8 StepperPilot__Allocate(void)
{
    uint8 result;

    result = 0xFF;

    if (Stepper_Pilot_Allocation < HBL_PILOT_NUM_STEPPER)
    {
        result = Stepper_Pilot_Allocation;
        Stepper_Pilot_Allocation++;
    }

    return (result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Initialize stepper pilot instance.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated stepper load instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of stepper pilot instance initialization.
 *  @retval     PASS - Instance initialization succeeded.
 *  @retval     FAIL - Instance initialization failed.
 */
PASS_FAIL_TYPE StepperPilot__InitializeInstance(HBL_LOAD_TYPE *load)
{
    PASS_FAIL_TYPE result;
    STEPPER_PILOT_DATA_TYPE *data;
    uint8 loop_index;
    uint8 table_index;

    result = FAIL;

    if (load->Pilot_Data_Index < Stepper_Pilot_Allocation)
    {
        data = &Stepper_Pilot_Data[load->Pilot_Data_Index];
        StepperPilotReadParameters(load);

        data->Step = 0;
        data->Pulse_Width = 1;
        data->Stepper_Array_Index = data->Parameters.Number_Of_Sequences;

        for (loop_index = 0; loop_index < 4; loop_index++)
        {
            table_index = load->Pin_Ptr[loop_index];

            if (VIRTUALPIN_TABLE[table_index].port != 0xFF)
            {
                Gpio__PinConfig(VIRTUALPIN_TABLE[table_index].port, VIRTUALPIN_TABLE[table_index].pin, VIRTUALPIN_TABLE[table_index].pin_config);
            }
        }

        result = PASS;
    }

    return (result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Set stepper pilot instance request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated stepper load instance.
 *
 *  @param      void *data - Pointer to request data. Pass argument as STEPPER_PILOT_REQUEST_TYPE *.
 *
 *  @return     PASS_FAIL_TYPE result - Result of stepper pilot request.
 *  @retval     PASS - Instance request succeeded.
 *  @retval     FAIL - Instance request failed.
 */
PASS_FAIL_TYPE StepperPilot__Request(HBL_LOAD_TYPE *load, void *data)
{
    PASS_FAIL_TYPE result;

    result = FAIL;

    if (load->Pilot_Data_Index < Stepper_Pilot_Allocation)
    {
        Stepper_Pilot_Data[load->Pilot_Data_Index].Requested = *((STEPPER_PILOT_REQUEST_TYPE *)data);
        Stepper_Pilot_Data[load->Pilot_Data_Index].Request_Updated = TRUE;

        result = PASS;
    }

    return (result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Process stepper pilot instance.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to ice dispenser door load instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of stepper pilot instance process.
 *  @retval     PASS - Instance process succeeded.
 *  @retval     FAIL - Instance process failed.
 */
PASS_FAIL_TYPE StepperPilot__Process(HBL_LOAD_TYPE *load)
{
    PASS_FAIL_TYPE result;
    STEPPER_PILOT_DATA_TYPE *data;

    result = FAIL;

    if (load->Pilot_Data_Index < Stepper_Pilot_Allocation)
    {
        data = &Stepper_Pilot_Data[load->Pilot_Data_Index];

        if (data->Request_Updated == TRUE)
        {
            MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
            data->Request_Updated = FALSE;
            data->Processed = data->Requested;
            data->Pulse_Width = data->Requested.Pulse_Width;

            // The following variables are also written in StepperPilot__Handler1ms() which is an interrupt routine.
            MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
            data->Step = data->Requested.Number_Of_Steps / data->Parameters.Number_Of_Sequences;
            data->Pulse_Counter = 1;
            data->Stepper_Array_Index = 0;
            data->Status = INCOMPLETE;
            MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
        }

        result = PASS;
    }

    return (result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get current stepper pilot instance request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated stepper load instance.
 *
 *  @return     void *requested - Requested instance data.
 *  @retval     Address of requested instance data. Cast it using *((STEPPER_PILOT_REQUEST_TYPE *)ptr) to get values.
 *  @retval     NULL - Invalid load index.
 */
void * StepperPilot__GetRequested(HBL_LOAD_TYPE *load)
{
    void *requested;

    requested = NULL;

    if (load->Load_Data_Index < Stepper_Pilot_Allocation)
    {
        requested = &(Stepper_Pilot_Data[load->Pilot_Data_Index].Requested);
    }

    return (requested);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get stepper pilot instance processed request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated stepper load instance.
 *
 *  @return     void * processed - Processed instance data.
 *  @retval     Address of processed instance data. Cast it using *((STEPPER_PILOT_REQUEST_TYPE *)ptr) to get values.
 *  @retval     NULL - Invalid load index.
 */
void * StepperPilot__GetProcessed(HBL_LOAD_TYPE *load)
{
    void *processed;

    processed = NULL;

    if (load->Pilot_Data_Index < Stepper_Pilot_Allocation)
    {
        processed = &(Stepper_Pilot_Data[load->Pilot_Data_Index].Processed);
    }

    return (processed);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Stepper pilot control based on 1 millisecond call rate.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to stepper load instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of stepper pilot handler.
 *  @retval     PASS - Instance handler succeeded.
 *  @retval     FAIL - Instance handler failed.
 */
PASS_FAIL_TYPE StepperPilot__Handler1ms (HBL_LOAD_TYPE *load)
{
    PASS_FAIL_TYPE result;
    STEPPER_PILOT_DATA_TYPE *data;
    uint8 sequence;
    uint8 mask;
    uint8 loop_index;
    uint8 table_index;

    result = PASS;
    data = &(Stepper_Pilot_Data[load->Load_Data_Index]);

    if (data->Pulse_Counter < data->Pulse_Width)
    {
       data->Pulse_Counter++;
    }
    else
    {
        data->Pulse_Counter = 1;

        if (data->Stepper_Array_Index > data->Parameters.Number_Of_Sequences - 1)
        {
            if (data->Step != 0)
            {
                data->Step--;
            }

            if (data->Step != 0)
            {
                // Reset the array index and do another pass
                data->Stepper_Array_Index = 0;
            }
            else
            {
                // Indicate request is complete and turn off all stepper outputs
                data->Status = COMPLETE;

                for (loop_index = 0; loop_index < 4;  loop_index++)
                {
                    table_index = load->Pin_Ptr[loop_index];
                    Gpio__PinWrite(VIRTUALPIN_TABLE[table_index].port, VIRTUALPIN_TABLE[table_index].pin, FALSE);
                }
            }
        }

        // If we are within the step array, do the next step
        if (data->Stepper_Array_Index < data->Parameters.Number_Of_Sequences)
        {
            if (data->Processed.Direction == STEPPER_DIRECTION_FORWARD)
            {
                sequence = data->Parameters.Sequences[data->Stepper_Array_Index];
            }
            else
            {
                sequence = data->Parameters.Sequences[data->Parameters.Number_Of_Sequences - data->Stepper_Array_Index - 1];
            }

            mask = 0x08;
            for (loop_index = 0; loop_index < 4; loop_index++)
            {
                table_index = load->Pin_Ptr[loop_index];

                if (mask & sequence)
                {
                    Gpio__PinWrite(VIRTUALPIN_TABLE[table_index].port, VIRTUALPIN_TABLE[table_index].pin, TRUE);
                }
                else
                {
                    Gpio__PinWrite(VIRTUALPIN_TABLE[table_index].port, VIRTUALPIN_TABLE[table_index].pin, FALSE);
                }

                mask >>= 1;
            }

            data->Stepper_Array_Index++;
        }
    }

    return (result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get stepper pilot instance status.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to stepper load instance.
 *
 *  @return     void *status - pointer to status of pilot instance. Cast it using *((COMPLETE_TYPE *)ptr) to get value.
 *  @retval     COMPLETE - instance request is complete
 *  @retval     INCOMPLETE - instance request in process
 */
void * StepperPilot__GetStatus(HBL_LOAD_TYPE *load)
{
    void *status;

    status = &(Stepper_Pilot_Data[load->Pilot_Data_Index].Status);

    return (status);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Read parameters from setting file.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to stepper load instance.
 *
 */
static void StepperPilotReadParameters(HBL_LOAD_TYPE * load)
{
    STEPPER_PARAMETERS_TYPE *params;

    params = &(Stepper_Pilot_Data[load->Pilot_Data_Index].Parameters);

    params->Number_Of_Sequences = (load->Parameter_Ptr.Data[5]) & 0x0F;

    params->Sequences[0] = load->Parameter_Ptr.Data[6] >> 4;
    params->Sequences[1] = load->Parameter_Ptr.Data[6] & 0x0F;
    params->Sequences[2] = load->Parameter_Ptr.Data[7] >> 4;
    params->Sequences[3] = load->Parameter_Ptr.Data[7] & 0x0F;

    if (params->Number_Of_Sequences == 8)
    {
        params->Sequences[4] = load->Parameter_Ptr.Data[8] >> 4;
        params->Sequences[5] = load->Parameter_Ptr.Data[8] & 0x0F;
        params->Sequences[6] = load->Parameter_Ptr.Data[9] >> 4;
        params->Sequences[7] = load->Parameter_Ptr.Data[9] & 0x0F;
    }
}

#endif /* (HBL_PILOT_NUM_STEPPER > 0) */

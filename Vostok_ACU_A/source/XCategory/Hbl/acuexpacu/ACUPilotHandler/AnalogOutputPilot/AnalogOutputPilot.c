/**
 *  @file       AnalogOutputPilot.c
 *
 *  @brief      Implements AnalogOutputPilot module.
 *
 *  @details    This module translates a percentage level to Dtoa
 *  			Load parameter value is represented as Q15 (0-32767)
 *  			Setting parameters:
 *  				- Minimum and Maximum values as two digits percentage (0-100.00%)
 *  				- High-Low Activation pin shall be used to set reference value to GND (if set, values are increasing) or VCC (if not set, values are decreasing)
 *  			Dtoa value would depend on Dtoa resolution avaliable - it can be configured on VirtualPin table
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/14 17:37:54EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "AnalogOutputPilot.h"

// -- Other Modules --
#if (HBL_PILOT_NUM_ANALOGOUTPUT > 0)
#include "Dtoa.h"
#include "VirtualPin.h"

#include <String.h>

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define	ANALOGOUTPUTPILOT_PARAM_MAX			32768
#define PARAM_MAX_PERCENT					10000

//! Tracks the number of Analog output pilot allocations.
static uint8 AnalogOutputPilot_Allocation;

//! Analog output pilot data type definition.
typedef PACKED struct
{
	uint16 Requested;
	uint16 Processed;
    uint16 Max_Percent;
    uint16 Min_Percent;
    HBL_LOAD_STATUS_TYPE Status;
} ANALOGOUTPUTPILOT_DATA_TYPE;

typedef PACKED struct
{
	uint16 Min_Percent;
	uint16 Max_Percent;
} ANALOGOUTPUTPILOT_PARAM_TYPE;

//! Analog output pilot data array. One entry per pilot instance.
static ANALOGOUTPUTPILOT_DATA_TYPE AnalogOutputPilot_Data[HBL_PILOT_NUM_ANALOGOUTPUT];

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static uint16 DtoaValueFromPercent(uint16 percent, uint16 max_value);

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module StepperPilot and its variables
 */
void AnalogOutputPilot__Initialize(void)
{
    AnalogOutputPilot_Allocation = 0;
    memset(AnalogOutputPilot_Data,0,sizeof(AnalogOutputPilot_Data));
    Dtoa__Initialize();
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Allocate pilot instance for Analog output.
 *
 *  @return     uint8 result - Allocation result.
 *  @retval     0-254 - Pilot allocation instance.
 *  @retval     0xFF - Allocation failed.
 */
uint8 AnalogOutputPilot__Allocate(void)
{
    uint8 retval = 0xFF;

    if (AnalogOutputPilot_Allocation < HBL_PILOT_NUM_ANALOGOUTPUT)
    {
        retval = AnalogOutputPilot_Allocation;
        AnalogOutputPilot_Allocation++;
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Initialize Analog output pilot instance.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated Analog output load instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of Analog output pilot instance initialization.
 *  @retval     PASS - Instance initialization succeeded.
 *  @retval     FAIL - Instance initialization failed.
 */
PASS_FAIL_TYPE AnalogOutputPilot__InitializeInstance(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval = FAIL;

    if (load->Pilot_Data_Index < AnalogOutputPilot_Allocation)
    {
        uint8 virtual_pin = load->Pin_Ptr[0];

    	if ( (virtual_pin < VirtualPin__GetNumOf())
    		&& (VIRTUALPIN_TABLE[virtual_pin].port != PORT_NULL) )
    	{
        	ANALOGOUTPUTPILOT_DATA_TYPE *data_ptr = &AnalogOutputPilot_Data[load->Pilot_Data_Index];
        	ANALOGOUTPUTPILOT_PARAM_TYPE *param_ptr = (ANALOGOUTPUTPILOT_PARAM_TYPE *)load->Parameter_Ptr.Data;
        	uint32 max_value = (1 << VIRTUALPIN_TABLE[virtual_pin].param0);

        	if (param_ptr != NULL)
            {
            	data_ptr->Max_Percent = ENDIANU16_STON(param_ptr->Max_Percent);
            	data_ptr->Min_Percent = ENDIANU16_STON(param_ptr->Min_Percent);

            	if (data_ptr->Max_Percent > data_ptr->Min_Percent)
            	{
            		retval = PASS;
            	}

            }

        	// Initialization succeeded
            if (retval == PASS)
            {
                uint32 start_value = 0;

                // Invert DAC logic if Hi_Low_Activation_Bitmap is not set
                if ((load->Hi_Low_Activation_Bitmap & 0x01) == 0)
                {
                	start_value = max_value - 1;
                }

                Dtoa__Convert((DTOA_CHANNEL_DEF ) VIRTUALPIN_TABLE[virtual_pin].device, (DTOA_CHANNEL_RESOLUTION_DEF) VIRTUALPIN_TABLE[virtual_pin].param0, 0,
        				(uint32) &start_value, 1); //lint !e923 Suppress Info:  cast from pointer to unsigned long [Encompasses MISRA 2004 Rule 11.1], [MISRA 2004 Rule 11.3]
                data_ptr->Status.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
            }
            else
            {
                data_ptr->Status.Load.Status = HBL_LOAD_STATUS_FAILURE;
            }
    	}
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Set Analog output pilot instance request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated Analog output load instance.
 *
 *  @param      void *data - Pointer to request data. Pass argument as uint8 *.
 *
 *  @return     PASS_FAIL_TYPE result - Result of Analog output pilot request.
 *  @retval     PASS - Instance request succeeded.
 *  @retval     FAIL - Instance request failed.
 */
PASS_FAIL_TYPE AnalogOutputPilot__Request(HBL_LOAD_TYPE * load, void * data)
{
    PASS_FAIL_TYPE result = FAIL;

    if (load->Pilot_Data_Index < AnalogOutputPilot_Allocation)
    {
    	uint16 new_value = *((PACKED uint16 *)data);
    	if (new_value >= ANALOGOUTPUTPILOT_PARAM_MAX)
    	{
    		new_value = ANALOGOUTPUTPILOT_PARAM_MAX - 1;
    	}
        AnalogOutputPilot_Data[load->Pilot_Data_Index].Requested = new_value;
        result = PASS;
    }

    return (result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Process Analog output pilot instance.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to Analog output load instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of Analog output pilot instance process.
 *  @retval     PASS - Instance process succeeded.
 *  @retval     FAIL - Instance process failed.
 */
PASS_FAIL_TYPE AnalogOutputPilot__Process(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval = FAIL;

    if (load->Pilot_Data_Index < AnalogOutputPilot_Allocation)
    {
    	ANALOGOUTPUTPILOT_DATA_TYPE *data_ptr = &AnalogOutputPilot_Data[load->Pilot_Data_Index];
        data_ptr->Processed = data_ptr->Requested;
        data_ptr->Status.Load.Cur_Reg = (data_ptr->Processed != 0);
        if (data_ptr->Status.Load.Status != HBL_LOAD_STATUS_FAILURE)
        {
            data_ptr->Status.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
        }
        retval = PASS;
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Asynchronous process for AnalogOutputPilot.
 *
 *	@detail     Asynchronous process handles update do Dtoa driver
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to AnalogOutputPilot instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of AnalogOutputPilot asynch execution.
 *  @retval     PASS - Execution succeeded.
 *  @retval     FAIL - Execution failed.
 */
PASS_FAIL_TYPE AnalogOutputPilot__AsynchProcess(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval = FAIL;

    if (load->Pilot_Data_Index < AnalogOutputPilot_Allocation)
    {
        uint8 virtual_pin = load->Pin_Ptr[0];

        if (virtual_pin < VirtualPin__GetNumOf())
        {
        	ANALOGOUTPUTPILOT_DATA_TYPE *data_ptr = &AnalogOutputPilot_Data[load->Pilot_Data_Index];
            if(data_ptr->Status.Load.Status == HBL_LOAD_STATUS_WAITING_TARGET)
            {
                uint32 max_adc_value = (1 << VIRTUALPIN_TABLE[virtual_pin].param0);
            	uint32 new_target = (data_ptr->Max_Percent - data_ptr->Min_Percent) * data_ptr->Processed;
				new_target += (ANALOGOUTPUTPILOT_PARAM_MAX / 2);
				new_target >>= 15; // Divide by 32768
				new_target += data_ptr->Min_Percent;
				new_target = DtoaValueFromPercent(new_target, max_adc_value);

				// Invert DAC logic if Hi_Low_Activation_Bitmap is not set
				if ((load->Hi_Low_Activation_Bitmap & 0x01) == 0)
				{
					new_target = max_adc_value - new_target - 1;
				}

				Dtoa__Convert((DTOA_CHANNEL_DEF ) VIRTUALPIN_TABLE[virtual_pin].device, (DTOA_CHANNEL_RESOLUTION_DEF) VIRTUALPIN_TABLE[virtual_pin].param0, 0,
						(uint32) &new_target, 1); //lint !e923 Suppress Info:  cast from pointer to unsigned long [Encompasses MISRA 2004 Rule 11.1], [MISRA 2004 Rule 11.3]

        		data_ptr->Status.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
        	}
            retval = PASS;
        }
    }
    return(retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get current Analog output pilot instance request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated Analog output load instance.
 *
 *  @return     void *requested - Requested instance data.
 *  @retval     Address of requested instance data. Cast it using *((uint8 *)ptr) to get values.
 *  @retval     NULL - Invalid load index.
 */
void * AnalogOutputPilot__GetRequested(HBL_LOAD_TYPE * load)
{
    void * requested = NULL;

    if (load->Pilot_Data_Index < AnalogOutputPilot_Allocation)
    {
        requested =  &(AnalogOutputPilot_Data[load->Pilot_Data_Index].Requested);
    }

    return (requested);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get Analog output pilot instance processed request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated Analog output load instance.
 *
 *  @return     void * processed - Processed instance data.
 *  @retval     Address of processed instance data. Cast it using *((uint8 *)ptr) to get values.
 *  @retval     NULL - Invalid load index.
 */
void * AnalogOutputPilot__GetProcessed(HBL_LOAD_TYPE * load)
{
    void * processed = NULL;

    if (load->Pilot_Data_Index < AnalogOutputPilot_Allocation)
    {
        processed =  &(AnalogOutputPilot_Data[load->Pilot_Data_Index].Processed);
    }

    return (processed);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get Analog output instance status.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to Analog output load instance.
 *
 *  @return     void *status - instance Status.
 *  @retval     Address of instance Status. Cast it using *((uint8 *)ptr) to get the value.
 *  @retval     NULL - Invalid load index.
 */
void * AnalogOutputPilot__GetStatus(HBL_LOAD_TYPE *load)
{
    void *status = NULL;

    if (load != NULL)
    {
        if (load->Pilot_Data_Index < AnalogOutputPilot_Allocation)
        {
            status = &(AnalogOutputPilot_Data[load->Pilot_Data_Index].Status.Byte);
        }
    }

    return (status);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Provides Dtoa value from a percentage
 *
 *  @param      uint16 percent - Percentage value, from 0 to PARAM_MAX_PERCENT
 *  @param      uint16 max_vlaue - Maximum Dtoa value (depends on resolution)
 *
 *  @return     uint16 dtoa_value - Value in Dtoa resolution
 */

uint16 DtoaValueFromPercent(uint16 percent, uint16 max_value)
{
	uint16 dtoa_value = (((percent * max_value) + (PARAM_MAX_PERCENT/2)) / PARAM_MAX_PERCENT);

	if (dtoa_value >= max_value)
	{
		dtoa_value = max_value - 1;
	}

	return(dtoa_value);
}

#endif

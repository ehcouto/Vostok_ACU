/**
 *  @file       
 *
 *  @brief      This is the Pilot to drive a pin on frequency.
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"

#if (HBL_PILOT_NUM_PWM_OUTPUT > 0)

#include "PwmPilot.h"
#include "Timer.h"
#include "VirtualPin.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//! constant = 1s update time for the acceleration manager.
#define PWM_UPDATE_TIME_MS						1000
//! Constant to emulate the pwm.
#define PWMPILOT_EMULATED_BASE_FREQUENCY		1000		//!< in Hz
/**
 * data structure from settings file.
 */
typedef PACKED struct PWM_PARAMETER_STRUCT
{
    //Base frequency.
    uint16 Base_Frequency;
    uint16 Speed_Min;
    uint16 Delta_Max;
    uint16 Delta_Min;
    //Is Frequency or duty?
    uint8  Is_Frequency_Driven	:1;
    uint8  Acceleration_Feature :1;
    uint8  Unused1				:6;
    //One empty byte here
    uint8 Unused2;
} PWMPILOT_PARAMETER_TYPE;

/**
 * This is the data definition. Depending on the SF we can drive
 * the output on frequency or duty.
 */
typedef PACKED struct
{
    PWMPILOT_PARAMETER_TYPE *Parameters;
    TIMER_PARAMETER_TYPE Timer_Parameter;
    uint16 Timer_Counter;
    uint16 Number_Of_Steps;
    uint16 Number_Of_Steps_On;
    uint16 Steps_For_Emulation;
    uint8 Requested;
    uint8 Processed;
    uint8 Previous;
    PWMPILOT_STATUS_TYPE Pilot_Status;
} PWMPILOT_DATA_TYPE;

static PWMPILOT_DATA_TYPE PwmPilot_Data[HBL_PILOT_NUM_PWM_OUTPUT];
static uint8 PwmPilot_Allocation;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void AccelerationManager(HBL_LOAD_TYPE * load);
static PASS_FAIL_TYPE EmulatePwm		(HBL_LOAD_TYPE * load);
static PASS_FAIL_TYPE DriveTimerDevice  (HBL_LOAD_TYPE * load);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module FrequencyPilot and its variables
 *
 */
void PwmPilot__Initialize(void)
{
    PwmPilot_Allocation = 0;
}
/**
 * Here we allocate one instance and return its index.
 * @return index of the instance allocated. 0xFF in case of an error.
 */
uint8 PwmPilot__Allocate(void)
{
    uint8 retval = 0xFF;

    if (PwmPilot_Allocation < (uint8) HBL_PILOT_NUM_PWM_OUTPUT)
    {
        retval = PwmPilot_Allocation;
        PwmPilot_Allocation++;
    }
    return (retval);
}

/**
 *
 * @param load
 * @return
 */
PASS_FAIL_TYPE PwmPilot__InitializeInstance(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval = FAIL;

    if ( load->Pilot_Data_Index < PwmPilot_Allocation )
    {
        BOOL_TYPE opposite_polarity = (BOOL_TYPE) (((uint8) load->Hi_Low_Activation_Bitmap & 0x01U) != 0U) ? FALSE : TRUE;
        uint8 virtual_pin = load->Pin_Ptr[0];
        uint8 index = load->Pilot_Data_Index;

        //Update the parameters and internal vars.
        PwmPilot_Data[index].Requested = 0;
        PwmPilot_Data[index].Processed = 0;
        PwmPilot_Data[index].Previous = 0;
        PwmPilot_Data[index].Timer_Counter = 1;
        PwmPilot_Data[index].Pilot_Status.Current = 0;
        PwmPilot_Data[index].Pilot_Status.Hbl_Status.Load.Cur_Reg = 0;
        PwmPilot_Data[index].Pilot_Status.Hbl_Status.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
        PwmPilot_Data[index].Parameters = (void *) load->Parameter_Ptr.Data;
		PwmPilot_Data[index].Steps_For_Emulation = 0;
		PwmPilot_Data[index].Number_Of_Steps = 0;
        //
        PwmPilot_Data[index].Timer_Parameter.Timer         = (TIMER_TYPE)VIRTUALPIN_TABLE[virtual_pin].device;
        PwmPilot_Data[index].Timer_Parameter.Channel       = (TIMER_CHANNEL_TYPE)VIRTUALPIN_TABLE[virtual_pin].param0;
        PwmPilot_Data[index].Timer_Parameter.Mode          = TIMER_MODE_PWM;
        PwmPilot_Data[index].Timer_Parameter.Port          = VIRTUALPIN_TABLE[virtual_pin].port;
        PwmPilot_Data[index].Timer_Parameter.Pin           = VIRTUALPIN_TABLE[virtual_pin].pin;
        PwmPilot_Data[index].Timer_Parameter.Pin_ReMap     = VIRTUALPIN_TABLE[virtual_pin].pin_remap;
        // PwmPilot shall always use Timer as Active High
        PwmPilot_Data[index].Timer_Parameter.Polarity      = TIMER_POLARITY_ACTIVE_HIGH;

        switch ( (uint8) PwmPilot_Data[index].Timer_Parameter.Timer )
        {
            case PWM_PILOT_EMULATE_PWM_PROCESS:
                retval = PASS;
                break;
            default:
                retval = Timer__Config(&PwmPilot_Data[index].Timer_Parameter);
                (void)Timer__PwmStop(&PwmPilot_Data[index].Timer_Parameter, opposite_polarity);
                break;
        }
    }

    return ( retval );
}
/**
 *
 * @param load
 * @param data
 * @return
 */
PASS_FAIL_TYPE PwmPilot__Request(HBL_LOAD_TYPE * load, void * data)
{
    PASS_FAIL_TYPE retval = FAIL;

    if (load->Pilot_Data_Index < PwmPilot_Allocation)
    {
        uint8 new_request = *(uint8 *) data;
        PwmPilot_Data[load->Pilot_Data_Index].Requested = new_request;
        retval = PASS;
    }

    return ( retval );
}
/**
 *
 * @param load
 * @return
 */
PASS_FAIL_TYPE PwmPilot__Process(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval = FAIL;

    if ( load->Pilot_Data_Index < PwmPilot_Allocation )
    {
        if( PwmPilot_Data[load->Pilot_Data_Index].Processed != PwmPilot_Data[load->Pilot_Data_Index].Requested )
        {
            uint8 new_reg;
            PwmPilot_Data[load->Pilot_Data_Index].Processed = PwmPilot_Data[load->Pilot_Data_Index].Requested;
            new_reg = ((PwmPilot_Data[load->Pilot_Data_Index].Processed > 0U) &&
                (PwmPilot_Data[load->Pilot_Data_Index].Processed >= PwmPilot_Data[load->Pilot_Data_Index].Parameters->Speed_Min)) ? 1U : 0U;
            PwmPilot_Data[load->Pilot_Data_Index].Pilot_Status.Hbl_Status.Load.Cur_Reg = new_reg;
            PwmPilot_Data[load->Pilot_Data_Index].Pilot_Status.Hbl_Status.Load.Status = HBL_LOAD_STATUS_WAITING_TARGET;
        }
        retval = PASS;
    }
    return ( retval );
}
/**
 *
 * @param load
 */
void * PwmPilot__GetRequested(HBL_LOAD_TYPE * load)
{
    void * ret_val = NULL;
    if ( load->Pilot_Data_Index < PwmPilot_Allocation )
    {
        ret_val = &PwmPilot_Data[load->Pilot_Data_Index].Requested;
    }
    return ( ret_val );
}

/**
 *
 * @param load
 */
void * PwmPilot__GetProcessed(HBL_LOAD_TYPE * load)
{
    void * ret_val = NULL;
    if ( load->Pilot_Data_Index < PwmPilot_Allocation)
    {
        ret_val = &PwmPilot_Data[load->Pilot_Data_Index].Processed;
    }
    return ( ret_val );
}

/**
 *
 * @param load
 * @return
 */
PASS_FAIL_TYPE PwmPilot__OneMsHandler(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval = FAIL;

    if ( load->Pilot_Data_Index < PwmPilot_Allocation )
    {
        AccelerationManager(load);
        switch ( (uint8) PwmPilot_Data[load->Pilot_Data_Index].Timer_Parameter.Timer )
        {
            case PWM_PILOT_EMULATE_PWM_PROCESS:
                retval = EmulatePwm(load);
                break;
            default:
                retval = DriveTimerDevice(load);
                break;
        }
    }
    return ( retval );
}

/**
 *
 * @param load
 */
void * PwmPilot__GetStatus(HBL_LOAD_TYPE * load)
{
    void * ret_val = NULL;
    if ( load->Pilot_Data_Index < PwmPilot_Allocation)
    {
        ret_val = &PwmPilot_Data[load->Pilot_Data_Index].Pilot_Status;
    }
    return ( ret_val );
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------

/**
 * We just drive in duty when the virtual pin has no TIMER associated with it.
 */
static PASS_FAIL_TYPE EmulatePwm(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval = PASS;
    uint8 index = load->Pilot_Data_Index;
    BOOL_TYPE polarity 			= (BOOL_TYPE) (((uint8) load->Hi_Low_Activation_Bitmap & 0x01U) != 0U) ? TRUE : FALSE;
    BOOL_TYPE oposite_polarity  = polarity ? FALSE : TRUE;

    //
    //loop for on the number of steps to get the correct frequency output.
    if(PwmPilot_Data[index].Steps_For_Emulation < PwmPilot_Data[index].Number_Of_Steps)
    {
        if(PwmPilot_Data[index].Steps_For_Emulation < PwmPilot_Data[index].Number_Of_Steps_On)
        {
            Gpio__PinWrite(PwmPilot_Data[index].Timer_Parameter.Port, PwmPilot_Data[index].Timer_Parameter.Pin, polarity);
        }
        else
        {
            Gpio__PinWrite(PwmPilot_Data[index].Timer_Parameter.Port, PwmPilot_Data[index].Timer_Parameter.Pin, oposite_polarity);
        }
        //
        PwmPilot_Data[index].Steps_For_Emulation++;
    }
    else
    {
        sint32 number_of_steps_on = 0;
        //
        PwmPilot_Data[index].Steps_For_Emulation = 0U;
        if(PwmPilot_Data[index].Previous != PwmPilot_Data[index].Pilot_Status.Current)
        {
            uint16 new_steps = 0U;
            if(PwmPilot_Data[index].Pilot_Status.Current > 0U)
            {
                new_steps = PWMPILOT_EMULATED_BASE_FREQUENCY;
                if(PwmPilot_Data[index].Parameters->Base_Frequency <= (uint16) PWMPILOT_EMULATED_BASE_FREQUENCY)
                {
                    new_steps /= PwmPilot_Data[index].Parameters->Base_Frequency;
                }
            }
            PwmPilot_Data[index].Number_Of_Steps = new_steps;
        }
        else
        {
            PwmPilot_Data[index].Steps_For_Emulation++;
        }

        if(PwmPilot_Data[index].Number_Of_Steps > 0U)
        {
            Gpio__PinWrite(PwmPilot_Data[index].Timer_Parameter.Port, PwmPilot_Data[index].Timer_Parameter.Pin, polarity);
            number_of_steps_on = PwmPilot_Data[index].Pilot_Status.Current;
            number_of_steps_on *= (sint32) PwmPilot_Data[index].Number_Of_Steps;
            number_of_steps_on /= 100;
        }
        else
        {
            Gpio__PinWrite(PwmPilot_Data[index].Timer_Parameter.Port, PwmPilot_Data[index].Timer_Parameter.Pin, oposite_polarity);
        }

        if(PwmPilot_Data[index].Previous != PwmPilot_Data[index].Pilot_Status.Current)
        {
            Gpio__PinConfig(PwmPilot_Data[index].Timer_Parameter.Port, PwmPilot_Data[index].Timer_Parameter.Pin, OUTPUT_PUSHPULL);
        }
        PwmPilot_Data[index].Number_Of_Steps_On = (uint16) number_of_steps_on;
        PwmPilot_Data[index].Previous = PwmPilot_Data[index].Pilot_Status.Current;
    }
    //
    return retval;
}

/**
 *
 */
static PASS_FAIL_TYPE DriveTimerDevice(HBL_LOAD_TYPE * load)
{
    uint8 index = load->Pilot_Data_Index;
    PASS_FAIL_TYPE retval = PASS;
    // is a frequency driven approach (TRUE) or duty driven.
    BOOL_TYPE is_freq = (PwmPilot_Data[index].Parameters->Is_Frequency_Driven) ? TRUE : FALSE;
    //Update Duty to refresh registers.
    if(PwmPilot_Data[index].Previous != PwmPilot_Data[index].Pilot_Status.Current)
    {
        if(PwmPilot_Data[index].Pilot_Status.Current == 0U)
        {
            BOOL_TYPE opposite_polarity = (BOOL_TYPE) (((uint8) load->Hi_Low_Activation_Bitmap & 0x01U) != 0U) ? FALSE : TRUE;
            retval = (PASS_FAIL_TYPE)Timer__PwmStop(&PwmPilot_Data[index].Timer_Parameter, opposite_polarity);
        }
        else if(is_freq != FALSE)
        {
            //If frequency... update the pwm frequency throught PwmStart.
            retval = Timer__PwmStart(&PwmPilot_Data[index].Timer_Parameter, PwmPilot_Data[index].Pilot_Status.Current, 5000);
        }
        else
        {
            uint16 current = PwmPilot_Data[index].Pilot_Status.Current;
            if(current > 100U)
            {
                current = 100U;
            }
            // If High/Low Activation flag is not set, invert duty cycle
            if (((uint8) load->Hi_Low_Activation_Bitmap & 0x01U) == 0U)
            {
                current = (100U - current);
            }
            current *= 100U;
            if(PwmPilot_Data[index].Previous == 0U)
            {
                retval = Timer__PwmStart(&PwmPilot_Data[index].Timer_Parameter,
                                ENDIANU16_STON(PwmPilot_Data[load->Pilot_Data_Index].Parameters->Base_Frequency),
                                current);	// Duty Cycle (0-100)
            }
            else
            {
                retval = Timer__PwmSetDutyCycle(&PwmPilot_Data[index].Timer_Parameter, current); // Duty Cycle (0-100)
            }
        }
        PwmPilot_Data[index].Previous = PwmPilot_Data[index].Pilot_Status.Current;
    }

    return retval;
}

/**
 * @brief  refer to W10493718    SPEC-MODULE, ACCELERATION MANAGER   Released A
 * @param load
 */
static void AccelerationManager(HBL_LOAD_TYPE * load)
{
    uint8 index    = load->Pilot_Data_Index;

    if((uint8) PwmPilot_Data[index].Parameters->Acceleration_Feature != 0U)
    {
        uint8 processed_filtered = PwmPilot_Data[index].Processed;
        if(processed_filtered < (uint8) PwmPilot_Data[index].Parameters->Speed_Min)
        {
            processed_filtered = 0U;
        }

        if(PwmPilot_Data[index].Pilot_Status.Current != processed_filtered)
        {
            uint16 new_timer = PwmPilot_Data[index].Timer_Counter;
            //Adjusting the pace.
            new_timer--;
            if(new_timer == 0U)
            {
                sint32 new_current = processed_filtered;
                sint32 delta_speed = 0;

                if(new_current > 0)
                {
                    delta_speed = (sint32) processed_filtered - (sint32) PwmPilot_Data[index].Previous;
                }

                // Get delta_speed absolute value
                if(delta_speed < 0)
                {
                    delta_speed = -delta_speed;
                }

                // Limit delta_speed if higher than Delta_Max first, then Delta_Min
                if (delta_speed >= (sint32) PwmPilot_Data[index].Parameters->Delta_Max)
                {
                    delta_speed = (sint32) PwmPilot_Data[index].Parameters->Delta_Max;
                }
                else if (delta_speed >= (sint32) PwmPilot_Data[index].Parameters->Delta_Min)
                {
                    delta_speed = (sint32) PwmPilot_Data[index].Parameters->Delta_Min;
                }

                if(delta_speed > 0)
                {
                    if (processed_filtered > (uint16) PwmPilot_Data[index].Previous)
                    {
                        new_current = (sint32) PwmPilot_Data[index].Previous + delta_speed;
                        if(new_current < (sint32) PwmPilot_Data[index].Parameters->Speed_Min)
                        {
                            new_current = (sint32) PwmPilot_Data[index].Parameters->Speed_Min;
                        }
                    }
                    else
                    {
                        new_current = (sint32) PwmPilot_Data[index].Previous - delta_speed;
                        if(new_current < (sint32) PwmPilot_Data[index].Parameters->Speed_Min)
                        {
                            new_current = 0;
                        }
                    }
                }

                new_timer = PWM_UPDATE_TIME_MS;
                if(new_current == (sint32) processed_filtered)
                {
                    PwmPilot_Data[index].Pilot_Status.Hbl_Status.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
                    new_timer = 1;
                }
                PwmPilot_Data[index].Pilot_Status.Current = (uint8) new_current;
            }
            PwmPilot_Data[index].Timer_Counter = new_timer;
        }
        else
        {
            PwmPilot_Data[index].Pilot_Status.Hbl_Status.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
            PwmPilot_Data[index].Timer_Counter = 1;
        }
    }
    else if (PwmPilot_Data[index].Processed < PwmPilot_Data[index].Parameters->Speed_Min)
    {
        PwmPilot_Data[index].Pilot_Status.Current = 0;
        PwmPilot_Data[index].Pilot_Status.Hbl_Status.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
        PwmPilot_Data[index].Pilot_Status.Hbl_Status.Load.Cur_Reg = 0;
    }
    else
    {
        PwmPilot_Data[index].Pilot_Status.Current = PwmPilot_Data[index].Processed;
        PwmPilot_Data[index].Pilot_Status.Hbl_Status.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
        PwmPilot_Data[index].Pilot_Status.Hbl_Status.Load.Cur_Reg = 1;
    }
}

#endif // HBL_PILOT_NUM_PWM_OUTPUT

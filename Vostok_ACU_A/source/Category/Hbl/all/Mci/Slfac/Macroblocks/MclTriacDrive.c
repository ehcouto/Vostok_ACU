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
 *  $Header: $
 *
 *  @copyright  Copyright 2019-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "MclTriacDrive.h"
#include "C_Extensions.h"
#include "Mcl.h"
#include "MclZeroCross.h"
#include "Mcl_prv.h"
#include "timeCapture.h"
#include "Gpio.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
static BOOL_TYPE Line_Voltage_Ok;               //!< Line voltage check
static sint16 Volt_At_Cycle_End_Prev;           //!< Voltage at cycle end previous
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void DriveTriac(MCL_TRIAC_DRIVE_IO_TYPE *io, MCL_PARAMS_TYPE* params, BOOL_TYPE activate_triac);
static BOOL_TYPE TriggerTriac(MCL_TRIAC_DRIVE_IO_TYPE *io, MCL_PARAMS_TYPE* params);
static uint8 UnderVoltage_Deb;

#ifndef UNDERVOLTAGE_DEBOUNCE
	#define UNDERVOLTAGE_DEBOUNCE    3
#endif
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module MciTriacDrive and its variables
 *
 */
void MclTriacDrive__Initialize(MCL_TRIAC_DRIVE_IO_TYPE *io, MCL_PARAMS_TYPE* params)
{
    Gpio__PinConfig(MOTOR_GPIO_PORT, MOTOR_GPIO_PIN, OUTPUT_PUSHPULL);
    MclTriacDrive__ControlTriac(FALSE);

    Line_Voltage_Ok = FALSE;
    Volt_At_Cycle_End_Prev = 0;

    UnderVoltage_Deb = UNDERVOLTAGE_DEBOUNCE;
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It runs at the fast handler to drive the triac according to the output of the control
 *
 */
void MclTriacDrive__FastHandler(MCL_TRIAC_DRIVE_IO_TYPE *io, MCL_PARAMS_TYPE* params)
{
	BOOL_TYPE activate_triac = FALSE;

	activate_triac = TriggerTriac(io, params);
	DriveTriac(io, params, activate_triac);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Fire Angle Triac Interruption
 *
 */
void MclTriacDrive__TriacFireHandler(MCL_TRIAC_DRIVE_IO_TYPE *io, MCL_PARAMS_TYPE* params, TC_EVENT_TYPE event)
{
	BOOL_TYPE activate_triac;

    if((MCISLFAC_OPEN_LOOP == io->ctrl_specific->Control_Mode) ||
       (MCISLFAC_CLOSE_LOOP == io->ctrl_specific->Control_Mode))
    {
        // From this moment DriveTriac will also check trigger condition and drive triac at timer interrupt.
        *io->Triac_Trigger_Event = TRUE;
        activate_triac = TriggerTriac(io, params);
        DriveTriac(io, params, activate_triac);
    }
    else if((MCISLFAC_STARTUP_PULSE_MEAS == io->ctrl_specific->Control_Mode) ||
            (MCISLFAC_STARTUP_PULSE_MEAS_OPPOSITE == io->ctrl_specific->Control_Mode) ||
            (MCISLFAC_STARTUP_PULSE_ROTATE == io->ctrl_specific->Control_Mode) ||
            (MCISLFAC_STARTUP_PULSE_ROTATE_AUX == io->ctrl_specific->Control_Mode) ||
            (MCISLFAC_STARTUP_PULSE_ROTATE_AUX_2 == io->ctrl_specific->Control_Mode) )
    {
        if(*io->Triac_Trigger_VZC_State == *io->MciSlfac_Prev_VZC_State)
        {
            DriveTriac(io, params, TRUE);
        }
    }
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Interface to activate/deactivate the Triac
 *
 */
void MclTriacDrive__ControlTriac(BOOL_TYPE state)
{
	Gpio__PinWrite(MOTOR_GPIO_PORT, MOTOR_GPIO_PIN, state);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * Trigger triac
 *
 */
static BOOL_TYPE TriggerTriac(MCL_TRIAC_DRIVE_IO_TYPE *io, MCL_PARAMS_TYPE* params)
{
    BOOL_TYPE activate_triac;
    ROTOR_STATE_TYPE rotor_state;
    uint16 timer_value;
    uint16 max_trigger;
    uint16 volt_sym;
    uint16 trigger_debounce;

    activate_triac = FALSE;
    trigger_debounce = 0;

    if(TRUE == *io->Check_Bemf)
    {
        rotor_state = *io->Direct_BEMF_State;
    }
    else
    {
#ifdef PW_MOTOR_ENABLED
        if(*io->MciSlfac_Wash_Motor_Type == WM_SINGLE_SPEED_SYNCH_SPH_PW)
        {
             rotor_state = *io->Rotor_State;
        }
        else
#endif
        {
            // Check if bemf from observer is correct when zero current detected
            // If observer error detected set unknown position
            if(((*io->Filt_Volt_Increase > 4) && (ROTOR_POSITIVE == *io->Rotor_State)) ||
               ((*io->Filt_Volt_Increase < (-4)) && (ROTOR_NEGATIVE == *io->Rotor_State)) ||
               (*io->Zero_Current_Detected == FALSE))
            {
                rotor_state = *io->Rotor_State;
            }
            else
            {
                rotor_state = ROTOR_UNKNOWN;
            }
            // Check if direct bemf position is ready and allow for trigger. If yes use it.
            // Error risk for direct bemf is always lower than observer.
            if(((TRUE  == *io->MciSlfac_Prev_VZC_State) && (ROTOR_POSITIVE == *io->Direct_BEMF_State)) ||
               ((FALSE == *io->MciSlfac_Prev_VZC_State) && (ROTOR_NEGATIVE == *io->Direct_BEMF_State)) ||
               (*io->Bemf_Aligned == TRUE))
            {
                rotor_state = *io->Direct_BEMF_State;
            }
        }
    }

    if(MCISLFAC_OPEN_LOOP == io->ctrl_specific->Control_Mode)
    {
        max_trigger = MCISLFAC_PERCENT_OF_SEMI_PERIOD(60);
    }
    else
    {
        max_trigger = MCISLFAC_PERCENT_OF_SEMI_PERIOD(params->Triac_Trigger_Angle_Limit); //TRIAC_TRIGGER_ANGLE_LIMIT
    }

    // For first trigger use observer and direct bemf feedback to calculate triac trigger condition
    // For all other triggers use observer only
    if(((TRUE  == *io->MciSlfac_Prev_VZC_State) && (ROTOR_POSITIVE == rotor_state)) ||
       ((FALSE == *io->MciSlfac_Prev_VZC_State) && (ROTOR_NEGATIVE == rotor_state)))
    {
        timer_value = TimeCapture__ReadTimerCnt(TRIAC_FIRE_CHANNEL);
        // Do not fire triac in 2ms before zerocrossing
        if((TRUE == *io->Triac_Trigger_Event) &&
           (timer_value <= max_trigger))
        {
        	*io->Triac_Trigger_Event = FALSE;
            *io->Check_Bemf = FALSE;

            if(*io->Triac_Fired == FALSE)
            {
                if(MCISLFAC_OPEN_LOOP == io->ctrl_specific->Control_Mode)
                {
                    trigger_debounce = *io->Correct_Trigger_Debounce;
                	trigger_debounce++;
                	*io->Correct_Trigger_Debounce = trigger_debounce;
                }

                if((*io->Volt_Max != 0) && (*io->Triac_Fired_Prev == TRUE))
                {
                    if(*io->Prev_Volt_Max_Ready == TRUE)
                    {
                        if(TRUE == *io->MciSlfac_Prev_VZC_State)
                        {
                            volt_sym = CONVERT_TO_OBSERVER_FORMAT((sint16)(*io->Prev_Volt_Max) - (sint16)(*io->Volt_Max));
                        }
                        else
                        {
                            volt_sym = CONVERT_TO_OBSERVER_FORMAT((sint16)(*io->Volt_Max) - (sint16)(*io->Prev_Volt_Max));
                        }
                        *io->Filt_Volt_Sym = IIRFilterLong(io->IIR_Prev_Volt_Symmetry, volt_sym, 2);

                        if(volt_sym > CONVERT_TO_OBSERVER_FORMAT(-200))
                        {
                        	*io->Bemf_Aligned = FALSE;
                        }
                        // When bemf symmetry value is low it means that BEMF peak is shifted around 90deg with peak of line voltage
                        // Filtered value has priority over raw value to not exit bemf aligned after single nopise sample
                        if(*io->Filt_Volt_Sym < CONVERT_TO_OBSERVER_FORMAT(-350))
                        {
                        	*io->Bemf_Aligned = TRUE;
                        }

                    }
                    *io->Prev_Volt_Max = *io->Volt_Max;
                    *io->Prev_Volt_Max_Ready = TRUE;
                }
                else
                {
                    // Auto exit from to high angles to enbled symmetry measurement
                    if(*io->Volt_Max == 0)
                    {
                        if(*io->Filt_Volt_Sym > CONVERT_TO_OBSERVER_FORMAT(-800))
                        {
                        	*io->Filt_Volt_Sym -= 17;
                        	volt_sym = *io->Filt_Volt_Sym;
                            *io->IIR_Prev_Volt_Symmetry = volt_sym<<2;
                        }
                    }
                    *io->Prev_Volt_Max_Ready = FALSE;
                }
            }

            *io->Triac_Fired = TRUE;
            activate_triac = TRUE;
        }
    }
    return activate_triac;
}



//---------------------------------------------------------------------------------------------------------------------
/**
 * Drive triac
 *
 */
static void DriveTriac(MCL_TRIAC_DRIVE_IO_TYPE *io, MCL_PARAMS_TYPE* params, BOOL_TYPE activate_triac)
{
	uint16 temp;
    //if((BOOL_TYPE)Gpio__PinRead(MOTOR_GPIO_PORT, MOTOR_GPIO_PIN) == MOTOR_TRIAC_ON_STATE)
    if(*io->Triac_State == TRUE)
    {
    	temp = MCISLFAC_PERCENT_OF_SEMI_PERIOD(params->Triac_Trigger_Angle_Limit); //TRIAC_TRIGGER_ANGLE_LIMIT
        if((TimeCapture__ReadTimerCnt(TRIAC_FIRE_CHANNEL) > temp) ||
            // If voltage is too close to zero then switch off triac to not trigger it by current in next cycle
            // In this place "prev" means current sample
           ((Line_Voltage_Ok == TRUE) && (MATHCALC__ABS(*io->Prev_Volt - *io->Offset_Volt) < params->Voltage_Base)))
        {
        	MclTriacDrive__ControlTriac(FALSE);
            *io->Triac_State = FALSE;
            if(*io->MciSlfac_Prev_VZC_State == FALSE)
            {
                *io->Volt_At_Cycle_End_Inc = *io->Prev_Volt - Volt_At_Cycle_End_Prev;
                Volt_At_Cycle_End_Prev = *io->Prev_Volt;
            }
            if((Line_Voltage_Ok == FALSE) ||
               (TimeCapture__ReadTimerCnt(TRIAC_FIRE_CHANNEL) < temp))
            {
            	UnderVoltage_Deb--;
            	if(UnderVoltage_Deb == 0)
            	{
            		*io->Mcl_Fault = UNDER_VOLTAGE;
            		//FailureHandlerRecover(UNDER_VOLTAGE);
            	}
            }
        }
        else
        {
        	temp = MATHCALC__ABS(*io->Prev_Volt - *io->Offset_Volt);
            if(TimeCapture__ReadTimerCnt(TRIAC_FIRE_CHANNEL) > MCISLFAC_PERCENT_OF_SEMI_PERIOD(100 - params->Triac_Trigger_Angle_Limit)) //TRIAC_TRIGGER_ANGLE_LIMIT
            {
                if((temp < params->Power_Base) &&
                   (Line_Voltage_Ok == TRUE))
                {
                    // Problem with voltage. Try to recovery by detecting power drop.
                    *io->Power_Drop_Enter_Deb = params->Power_Drop_Thr;
                }
            }
            // Check if line voltage is ok
            if(temp > 800)
            {
                Line_Voltage_Ok = TRUE;
            }
            *io->Zero_Current_Detected = FALSE;
            *io->Triac_State = TRUE;
        }
    }
    else
    {
        if(TRUE == activate_triac)
        {
            if(TimeCapture__ReadTimerCnt(TRIAC_FIRE_CHANNEL) <= MCISLFAC_PERCENT_OF_SEMI_PERIOD(params->Bemf_Shift_PI_Max_Out)) //BEMF_SHIFT_PI_MAX_OUT
            {
                Line_Voltage_Ok = FALSE;
            }
            else
            {
                // Trigger angle is too late. Disable voltage check.
                Line_Voltage_Ok = TRUE;
            }
            *io->Zero_Current_Detected = FALSE;
            MclTriacDrive__ControlTriac(TRUE);
            *io->Triac_State = TRUE;
            UnderVoltage_Deb = UNDERVOLTAGE_DEBOUNCE;
        }
        else
        {
        	MclTriacDrive__ControlTriac(FALSE);
            *io->Triac_State = FALSE;
        }
    }
}

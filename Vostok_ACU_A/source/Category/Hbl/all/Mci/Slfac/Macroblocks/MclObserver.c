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
#include "C_Extensions.h"
#include "MclObserver.h" 
#include "Pi.h"
#include "Atod.h"
#include "Mcl_prv.h"

#include <stdlib.h>

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
static uint16 IIR_Prev_Active_Power;            //!< Previous value for active power iir filter
static uint16 IIR_Prev_Volt;                    //!< Previous value for voltage iir filter
#ifdef PW_MOTOR_ENABLED
static uint16 Filt_Bemf_Volt_Light;             //!< Light filtered BEMF voltage
static uint32 IIR_Prev_Bemf_Light;
#endif
static uint16 Bemf_Volt;                        //!< BEMF voltage
static uint16 Prev_Filt_Volt;                   //!< Previous filt voltage value
static ROTOR_STATE_TYPE Direct_BEMF_State_Prev; //!< Prev direct BEMF state
static uint8 Direct_BEMF_State_Deb;             //!< Direct BEMF state debounce counter
static sint32 Prescaler_Param;                  //!< Initialize Parameter for Active Power Calculation
static sint16 Pi_Param;                         //!< Initialize Parameter for Pi Max
static sint32 MciSlfac_Active_Power_Temp;       //!< Temporary active power accumulator

//BEMF Filtering
static uint32 IIR_Prev_Bemf;
static uint32 IIR_Prev_Bemf_2;

//BEMF Parameters
static sint32 FactorCurr;                       //!< Current factor used in observer
static sint32 FactorDeltaCurr;                  //!< Delta current factor used in observer
static sint32 FactorDeltaCurr_For_High_Current; //!< Delta current factor for high current used in observer


//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module MclObserver and its variables
 *
 */
void MclObserver__Initialize(MCL_OBSERVER_IO_TYPE* io, MCL_PARAMS_TYPE* params)
{
	IIR_Prev_Active_Power = 0;
	IIR_Prev_Volt = 0;
#ifdef PW_MOTOR_ENABLED
	Filt_Bemf_Volt_Light = 0;
#endif
	Bemf_Volt = 0;
	Prev_Filt_Volt = 0;
	Direct_BEMF_State_Prev = ROTOR_UNKNOWN;
	Direct_BEMF_State_Deb = 0;
	Prescaler_Param = ((sint32)params->Prescaler_for_Active_Power * (sint32)params->Mcisflac_Fast_Handler_Frequency)/60;
	Pi_Param = CONVERT_TO_PI_FORMAT(MCISLFAC_PERCENT_OF_SEMI_PERIOD_60HZ(params->Bemf_Shift_PI_Max_Out));
	MciSlfac_Active_Power_Temp = 0;

	FactorCurr = FLOAT_TO_3F16((params->Winding_Resistance * params->Current_Sensor_Gain) / params->Voltage_Sensor_Gain); //ADC_TO_CURR_FACTOR //ADC_TO_VOLT_FACTOR --->>> IQ21
	FactorDeltaCurr = FLOAT_TO_6F13((params->Winding_Inductance * (float)params->Mcisflac_Fast_Handler_Frequency * params->Current_Sensor_Gain) / params->Voltage_Sensor_Gain);
	FactorDeltaCurr_For_High_Current = FactorDeltaCurr*250;

	MclObserver__ResetState(params);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module MclObserver and its variables
 *
 */
void MclObserver__RunningHandler(MCL_OBSERVER_IO_TYPE* io, MCL_PARAMS_TYPE* params)
{
    sint16 sig_filt_volt;
    sint32 factor_delta_curr;
    ROTOR_STATE_TYPE bemf_state;
    uint16 temp_u16;

    bemf_state         = ROTOR_UNKNOWN;
    sig_filt_volt      = 0;
    factor_delta_curr  = 0;

    //Active Power Accumulation
    MciSlfac_Active_Power_Temp += (sint32)(*io->sig_raw_curr) * (sint32)(*io->sig_raw_volt);

    // Vreal: the actual input voltage of the motor stator
    // Ireal: the current actual current of the motor stator during the current sampling period
    // Ireal_last: the last actual current of the motor stator during the last sampling period
    // f: sampling frequency
    // R: motor resistance
    // L:motor inductance
    // So, c language (floating point) should be like this
    // e_backemf = Vreal -R*Ireal-L*(Ireal-Ireal_last)/dt;
    // Vreal_last = Vreal;
    //
    // bemf fp after rounding = (U-Uoffset)
    // - round((F1fp*(I-Ioffset))/F1fpdiv)
    // - round((F2fp*(dI))/F2div)

    // Do not calculate observer for very high current or current increase because dI component generates error.
    if((*io->abs_raw_curr < params->Observer_Operation_Max_Limit) && (MATHCALC__ABS(*io->dif_raw_curr) < 100)) //OBSERVER_OPERATION_MAX_LIMIT
    {
        if(FALSE == *io->zero_current_prev)
        {
            // When current is low and we detect significant voltage drop caused be triac off
            // then current is close to zero. Or when current is zero.
            if((((*io->abs_raw_curr < params->ZC_Current_Detection_Thr) &&
                 (((TRUE == *io->MciSlfac_Prev_VZC_State) && ((sint16)(*io->Prev_Volt - *io->raw_volt) > (sint16)params->ZC_Voltage_Detection_Thr)) ||
                  ((FALSE == *io->MciSlfac_Prev_VZC_State) && ((sint16)(*io->raw_volt - *io->Prev_Volt) > (sint16)params->ZC_Voltage_Detection_Thr)))) ||
                (*io->abs_raw_curr < params->Current_Base)) &&
               (*io->Triac_State == FALSE)) //ZC_CURENT_DETECTION_THR   //ZC_VOLTAGE_DETECTION_THR

            {
                IIR_Prev_Volt = 0;
                *io->Zero_Current_Detected = TRUE;
            }
            else
            {
                // I*dI is better determine if normalization is necessary. High current not always mean that normalization is needed.
                // But when also dI is high we need it.
                if((MATHCALC__ABS(*io->abs_raw_curr * (sint32)(*io->dif_raw_curr)) / 16) < 1200)
                {
                    factor_delta_curr = FactorDeltaCurr;
                }
#ifdef PW_MOTOR_ENABLED
                else if(((MATHCALC__ABS(*io->abs_raw_curr * (sint32)(*io->dif_raw_curr)) / 16) < 2000) && (WM_SINGLE_SPEED_SYNCH_SPH_PW == *io->MciSlfac_Wash_Motor_Type))
                {
                    // Try normalization
                    factor_delta_curr = FactorDeltaCurr_For_High_Current / *io->abs_raw_curr;
                    *io->bemf_volt_tmp = (sint32)(*io->sig_raw_volt) -
                        (((sint32)((sint32)(*io->FactorCurr) * (sint32)(*io->sig_raw_curr)) + (sint32)(DIV_3F16/2)) / (sint32)DIV_3F16) -
                        (((sint32)((sint32)factor_delta_curr * (sint32)(*io->dif_raw_curr)) + (sint32)(DIV_6F13/2)) / (sint32)DIV_6F13);

                    // If normalization creates observer error then disable normalization
                    if(((Filt_Bemf_Volt_Light < CONVERT_TO_OBSERVER_FORMAT(0)) && (*io->bemf_volt_tmp > 0)) ||
                       ((Filt_Bemf_Volt_Light > CONVERT_TO_OBSERVER_FORMAT(0)) && (*io->bemf_volt_tmp < 0)) )
                    {
                        factor_delta_curr = *io->FactorDeltaCurr;
                    }
                }
#endif
                else
                {
                     // Normalize dI component factor for high current
                    factor_delta_curr = FactorDeltaCurr_For_High_Current / *io->abs_raw_curr;
                }

                *io->bemf_volt_tmp = (sint32)(*io->sig_raw_volt) -
                    (((sint32)((sint32)(FactorCurr) * (sint32)(*io->sig_raw_curr)) + (sint32)(DIV_3F16/2)) / (sint32)DIV_3F16) -
                    (((sint32)((sint32)factor_delta_curr * (sint32)(*io->dif_raw_curr)) + (sint32)(DIV_6F13/2)) / (sint32)DIV_6F13);

                Bemf_Volt = CONVERT_TO_OBSERVER_FORMAT(*io->bemf_volt_tmp);
            }
        }
        else
        {
            // For zero current read bemf directly
        	*io->bemf_volt_tmp = (sint32)(*io->sig_raw_volt);
            Bemf_Volt = CONVERT_TO_OBSERVER_FORMAT(*io->bemf_volt_tmp);

            if(0 == IIR_Prev_Volt)
            {
                // Init filter
            	temp_u16 = *io->raw_volt;
                IIR_Prev_Volt = temp_u16<<2;
                Prev_Filt_Volt = *io->raw_volt;
            }
#ifdef PW_MOTOR_ENABLED
            if(*io->MciSlfac_Wash_Motor_Type == WM_SINGLE_SPEED_SYNCH_SPH_PW)
            {
            	*io->filt_volt = IIRFilter(&IIR_Prev_Volt, *io->raw_volt, 2);
            }
            else
#endif
            {
                // Additional filtering
            	*io->filt_volt = (IIRFilter(&IIR_Prev_Volt, *io->raw_volt, 2) + Prev_Filt_Volt) / 2;
            }
            *io->Filt_Volt_Increase = *io->filt_volt - Prev_Filt_Volt;
            Prev_Filt_Volt = *io->filt_volt;

            sig_filt_volt = (sint16)*io->filt_volt - (sint16)(*io->Offset_Volt);
#ifdef PW_MOTOR_ENABLED
            if(*io->MciSlfac_Wash_Motor_Type == WM_SINGLE_SPEED_SYNCH_SPH_PW)
            {
                // TODO: Merge it to AW
                // When BEMF is close to ZC point check also rate of change. For fast changing signals change ZC detection threshold.
                if(((sig_filt_volt > 0) && (*io->Filt_Volt_Increase >= 0)) ||
                   ((sig_filt_volt > (-100)) && (*io->Filt_Volt_Increase >= 6)))
                {
                    bemf_state = ROTOR_POSITIVE;
                }
                else if(((sig_filt_volt < 0) && (*io->Filt_Volt_Increase <= 0)) ||
                        ((sig_filt_volt < 100) && (*io->Filt_Volt_Increase <= (-6))))
                {
                    bemf_state = ROTOR_NEGATIVE;
                }
            }
            else if(*io->Bemf_Aligned == FALSE)
#else
            if(*io->Bemf_Aligned == FALSE)
#endif
            {
                // When BEMF is close to ZC point check also rate of change. For fast changing signals change ZC detection threshold.
                if(((sig_filt_volt > 0) && (*io->Filt_Volt_Increase >= 0)) ||
                   ((sig_filt_volt > (-150)) && (*io->Filt_Volt_Increase >= 2)) ||
                   ((sig_filt_volt > 150) && (*io->Filt_Volt_Increase >= (-1))))
                {
                    bemf_state = ROTOR_POSITIVE;
                }
                else if(((sig_filt_volt < 0) && (*io->Filt_Volt_Increase <= 0)) ||
                        ((sig_filt_volt < 150) && (*io->Filt_Volt_Increase <= (-2))) ||
                        ((sig_filt_volt < (-150)) && (*io->Filt_Volt_Increase <= 1)))
                {
                    bemf_state = ROTOR_NEGATIVE;
                }
            }
            else
            {
                // If BEMF is aligned with line voltage then change condition. Condition is around 90 deg shifted.
                if(((sig_filt_volt > 0) && (*io->Filt_Volt_Increase >= 0)) ||
                   (sig_filt_volt > 250) )
                {
                    bemf_state = ROTOR_POSITIVE;
                }
                else if(((sig_filt_volt < 0) && (*io->Filt_Volt_Increase <= 0))||
                        (sig_filt_volt < (-250)))
                {
                    bemf_state = ROTOR_NEGATIVE;
                }
            }
        }
#ifdef PW_MOTOR_ENABLED
        Filt_Bemf_Volt_Light = IIRFilterLong(io->IIR_Prev_Bemf_Light, Bemf_Volt, 3);
#endif

        // Use filter chain for observer filtration. It provides good signal dynamics and good noise cancelling.
        *io->Filt_Bemf_Volt = IIRFilterLong(&IIR_Prev_Bemf, Bemf_Volt, params->Bemf_Alpha_IIR); //BEMF_ALPHA_IIR
        *io->Filt_Bemf_Volt = IIRFilterLong(&IIR_Prev_Bemf_2, *io->Filt_Bemf_Volt, params->Bemf_Alpha_2_IIR); //BEMF_2_ALPHA_IIR
    }

    if(Direct_BEMF_State_Prev == bemf_state)
    {
        Direct_BEMF_State_Deb ++;
        if(Direct_BEMF_State_Deb >= 3)
        {
            *io->Direct_BEMF_State = bemf_state;
        }

    }
    else
    {
        Direct_BEMF_State_Deb = 0;
        Direct_BEMF_State_Prev = bemf_state;
    }

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief
 *
 */
void MclObserver__ActivePowerEstimation(MCL_OBSERVER_IO_TYPE* io, MCL_PARAMS_TYPE* params)
{
	uint16 temp_active_power;
	uint16 filt_active_power;

#ifdef PW_MOTOR_ENABLED
    if((0 != (*io->Mcl_Main_Counter - *io->VZC_Time_P)) && (TRUE == *io->Triac_Fired_Prev) && (TRUE == *io->Triac_Fired))
#else
    if((TRUE == *io->Triac_Fired_Prev) && (TRUE == *io->Triac_Fired))
#endif
    {
        if(MciSlfac_Active_Power_Temp < 0)
        {
            MciSlfac_Active_Power_Temp = 0;
        }

        temp_active_power = (uint16)(MciSlfac_Active_Power_Temp / Prescaler_Param); //PRESCALER_FOR_ACTIVE_POWER *
        filt_active_power = IIRFilter(&IIR_Prev_Active_Power, temp_active_power, 2);

        //if(Pi__GetOutput(io->Pi_Bemf_Shift) == Pi_Param) //BEMF_SHIFT_PI_MAX_OUT
        if(io->Pi_Bemf_Shift->Output == Pi_Param) //BEMF_SHIFT_PI_MAX_OUT
        {
            // When pi is starurated use filtred active power to decrease variance.
            *io->MciSlfac_Active_Power = filt_active_power;
        }
        else
        {
            *io->MciSlfac_Active_Power = temp_active_power;
        }

        *io->MciSlfac_Active_Power_Prev = filt_active_power;
    }
    MciSlfac_Active_Power_Temp = 0;

    if(*io->Power_Drop_Enter_Deb > 0)
    {
    	*io->Power_Drop_Enter_Deb -=1;
    }

    if(*io->Power_Drop_Exit_Deb > 0)
    {
    	*io->Power_Drop_Exit_Deb -=1;
        if(*io->Power_Drop_Exit_Deb == 0)
        {
            // Restore previous Triac_Start_Time
            *io->Triac_Start_Time = *io->Triac_Start_Time_Init;
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module MclObserver and its variables
 *
 */
void MclObserver__ResetState(MCL_PARAMS_TYPE* params)
{
    IIR_Prev_Bemf = (CONVERT_TO_OBSERVER_FORMAT(0))<<params->Bemf_Alpha_IIR; //BEMF_ALPHA_IIR
    IIR_Prev_Bemf_2 = (CONVERT_TO_OBSERVER_FORMAT(0))<<params->Bemf_Alpha_2_IIR; //BEMF_2_ALPHA_IIR
#ifdef PW_MOTOR_ENABLED
    IIR_Prev_Bemf_Light = (CONVERT_TO_OBSERVER_FORMAT(0))<<3;
#endif
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

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
#include "MclFireAngleCtrl.h"
#include "Pi.h"
#include "Mcl_prv.h"
#include "PeakDetector.h"
#include "MclZeroCross.h"
#include "MclObserver.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
static uint16 Bemf_Not_Changed_Event;           //!< Time to detect bemf not changing event
static uint8 Rotor_State_Deb;                   //!< Rotor state debounce counter
static uint16 Reset_Param; //!< Reset Unexpected Motor Stop Param
static uint16 Wait_Param;  //!< Wait Time Param

static uint32 IIR_Prev_Curr_Offset;     //!< IIR Filter Current Offset
static uint32 IIR_Prev_Volt_Offset;     //!< IIR Filter Voltage Offset

static uint16 Initial_Pulse_Current_Max;    //!< Max Pulse Current
static uint16 Initial_Pulse_Current_Min;    //!< Min Pulse Current

#define Filt_Volt_Max    Initial_Pulse_Current_Max   //!< Max Filtered Voltage (BEMF)
#define Filt_Volt_Min    Initial_Pulse_Current_Min   //!< Min Filtered Voltage (BEMF)

static sint32 Initial_Pulse_Current_Meas;   //!< Initial Current Pulse Measurement
#ifdef PW_MOTOR_ENABLED
static uint16 IIR_Prev_Bemf_Shift_Time;         //!< Previous value for bemf shift iir filter
static uint16 Bemf_Shift_Time;                  //!< Shift between VZC and motor BZC
#endif

PI_TYPE Pi_Bemf_Shirt_Limiter;           //!< PI regulator for shift control upper limit

static uint16 Initial_Pulse_Voltag_Acc;         //!< Initial pulse voltage accumulator
static uint16 MciSlfac_Active_Power_Previous;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void UpdatePiOutput(MCL_FIREANGLE_CTRL_IO_TYPE *io, sint8 percent_of_semicycle);
static void GoToOpenLoop(MCL_FIREANGLE_CTRL_IO_TYPE *io, MCL_PARAMS_TYPE* params);
static void ResetUnexpectedMotorStop(MCL_FIREANGLE_CTRL_IO_TYPE *io, MCL_PARAMS_TYPE* params);
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module MclFireAngleCtrl and its variables
 *
 */
void MclFireAngleCtrl__Initialize(MCL_FIREANGLE_CTRL_IO_TYPE* io, MCL_PARAMS_TYPE* params)
{
    Bemf_Not_Changed_Event = 0;
    Rotor_State_Deb = 0;
    Reset_Param = MCISLFAC_MILI_SECONDS_TO_TIMER_TICK(params->Motor_Stop_Detection_Time, params->Mcisflac_Fast_Handler_Frequency);
    Wait_Param = MCISLFAC_MILI_SECONDS_TO_TIMER_TICK(params->Wait_Between_Meas_Time, params->Mcisflac_Fast_Handler_Frequency);

    MclFireAngleCtrl__ResetState(io, params);

#ifdef PW_MOTOR_ENABLED
    IIR_Prev_Bemf_Shift_Time = params->Bemf_Shift_Init_Value<<params->Bemf_Shift_Alpha_IIR; //BEMF_SHIFT_INIT_VALUE //BEMF_SHIFT_ALPHA_IIR
    Bemf_Shift_Time = 0;
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 *
 */
void MclFireAngleCtrl__ResetState(MCL_FIREANGLE_CTRL_IO_TYPE *io, MCL_PARAMS_TYPE* params)
{
    IIR_Prev_Curr_Offset = 0;
    IIR_Prev_Volt_Offset = 0;

    Initial_Pulse_Current_Max = 0;
    Initial_Pulse_Current_Min = 4096;

    Initial_Pulse_Current_Meas = 0;

    Initial_Pulse_Voltag_Acc = 0;
    MciSlfac_Active_Power_Previous = 0;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 *
 */
void MclFireAngleCtrl__ZeroCrossHandler(MCL_FIREANGLE_CTRL_IO_TYPE *io,  MCL_PARAMS_TYPE* params)
{
	uint16 triac_time_interrupt;
	uint16 triac_time;
	sint16 pi_output_temp;
    sint16 active_power_inc;
    sint16 filt_active_power_inc;

   	 switch(io->ctrl_specific->Control_Mode)
	        {
	            case MCISLFAC_STARTUP:
	                if(FALSE == *io->MciSlfac_Prev_VZC_State)
	                {
	                	*io->Triac_Trigger_VZC_State = FALSE;
	                	MclFireAngleCtrl__ResetState(io, params);

                        triac_time_interrupt = MCISLFAC_PERCENT_OF_SEMI_PERIOD(params->Commom_Meas_Pulse_Angle); //ASKOLL_PW_MEAS_PULSE_ANGLE AW: COMMON_MEAS_PULSE_ANGLE

	                    TimeCapture__SetPostEventInterruptDelay(TRIAC_FIRE_CHANNEL, triac_time_interrupt);

	                    io->ctrl_specific->Control_Mode = MCISLFAC_STARTUP_PULSE_MEAS;
	                }
	                break;

	            case MCISLFAC_STARTUP_PULSE_MEAS:
	                if(FALSE == *io->MciSlfac_Prev_VZC_State)
	                {
	                    *io->Triac_Trigger_VZC_State = TRUE;
	                    *io->State_Timeout = *io->Mcl_Main_Counter + Wait_Param; //WAIT_BETWEEN_MEAS_TIME
	                    io->ctrl_specific->Control_Mode = MCISLFAC_STARTUP_WAIT;
	                }
	                break;

	            case MCISLFAC_STARTUP_INIT_PULSE_MEAS_OPPOSITE:
	                if(*io->Triac_Trigger_VZC_State == *io->MciSlfac_Prev_VZC_State)
	                {
	                	io->ctrl_specific->Control_Mode = MCISLFAC_STARTUP_PULSE_MEAS_OPPOSITE;
	                }
	                break;

	            case MCISLFAC_STARTUP_PULSE_MEAS_OPPOSITE:
	                if(*io->Triac_Trigger_VZC_State == *io->MciSlfac_Prev_VZC_State)
	                {
	                    // Current peak is higher if rotor is one polarity and lower when it in other polarity.
	                    // By comparing current peak between two initial pulses we could detect rotor polarity.
	                    if(((((Initial_Pulse_Current_Max + Initial_Pulse_Current_Min)>>1) < *io->Offset_Curr) && (TRUE == *io->Triac_Trigger_VZC_State)) ||
	                       ((((Initial_Pulse_Current_Max + Initial_Pulse_Current_Min)>>1) > *io->Offset_Curr) && (FALSE == *io->Triac_Trigger_VZC_State)))
	                    {
#ifdef PW_MOTOR_ENABLED
	                        if(WM_SINGLE_SPEED_SYNCH_SPH_PW == *io->MciSlfac_Wash_Motor_Type)
	                        {
	                            if(TRUE == *io->Triac_Trigger_VZC_State)
	                            {
	                                Initial_Pulse_Current_Meas = Initial_Pulse_Current_Max - *io->Offset_Curr;
	                            }
	                            else
	                            {
	                            	Initial_Pulse_Current_Meas = *io->Offset_Curr - Initial_Pulse_Current_Min;
	                            }

	                            // Calculate first trigger angle based on current
	                            triac_time = (sint16)(Initial_Pulse_Current_Meas / (params->First_Pulse_Compensation_A)) - (sint16)(params->First_Pulse_Compensation_B); //ASKOLL_PW_FIRST_PULSE_COMPENSATION_A //ASKOLL_PW_FIRST_PULSE_COMPENSATION_B

	                            if(triac_time > params->First_Pulse_Max_Limit) //ASKOLL_PW_FIRST_PULSE_MAX_LIMIT
	                            {
	                                triac_time = params->First_Pulse_Max_Limit;
	                            }
	                            else if(triac_time < COMMON_FIRST_PULSE_MIN_LIMIT) //COMMON_FIRST_PULSE_MIN_LIMIT
	                            {
	                                triac_time = COMMON_FIRST_PULSE_MIN_LIMIT;
	                            }
	                            TimeCapture__SetPostEventInterruptDelay(TRIAC_FIRE_CHANNEL, MCISLFAC_PERCENT_OF_SEMI_PERIOD(triac_time));

	                            triac_time = triac_time + params->Open_Loop_Angle_Limit_Add; //ASKOLL_PW_OPEN_LOOP_ANGLE_LIMIT_ADD
	                            if(triac_time > params->Open_Loop_Max_Limit) //ASKOLL_PW_OPEN_LOOP_ANGLE_MAX_LIMIT
	                            {
	                                triac_time = params->Open_Loop_Max_Limit;
	                            }
	                            *io->Triac_Start_Time = CONVERT_TO_PI_FORMAT(MCISLFAC_PERCENT_OF_SEMI_PERIOD(triac_time));
	                            *io->Triac_Start_Time_Init = *io->Triac_Start_Time;
	                            io->ctrl_specific->Control_Mode = MCISLFAC_STARTUP_PULSE_ROTATE;
	                        }
	                        else
#endif
	                        {
	                            // Accumulate current from two pulses
	                            if(TRUE == *io->Triac_Trigger_VZC_State)
	                            {
	                            	Initial_Pulse_Current_Meas += Initial_Pulse_Current_Max - *io->Offset_Curr;
	                            }
	                            else
	                            {
	                            	Initial_Pulse_Current_Meas += *io->Offset_Curr - Initial_Pulse_Current_Min;
	                            }

	                            Initial_Pulse_Current_Max = 0;
	                            Initial_Pulse_Current_Min = 4096;

	                            // Setup second meas trigger interrupt
	                            TimeCapture__SetPostEventInterruptDelay(TRIAC_FIRE_CHANNEL, MCISLFAC_PERCENT_OF_SEMI_PERIOD(params->Meas_Pulse_Angle)); //ASKOLL_AW_SECOND_MEAS_PULSE_ANGLE
	                            io->ctrl_specific->Control_Mode = MCISLFAC_STARTUP_PULSE_ROTATE_AUX;
	                        }
	                    }
	                    else
	                    {
	                        if(TRUE == *io->Triac_Trigger_VZC_State)
	                        {
	                        	IIR_Prev_Curr_Offset = 0;
	                        	IIR_Prev_Volt_Offset = 0;
	                            // Rotor polarity is different than last pulse. We should wait and use opposite pulse to start motor
	                            Initial_Pulse_Current_Min = 4096;
	                            *io->Triac_Trigger_VZC_State = FALSE;
	                            io->ctrl_specific->Control_Mode = MCISLFAC_STARTUP_WAIT;
	                            *io->State_Timeout = *io->Mcl_Main_Counter + Wait_Param; //WAIT_BETWEEN_MEAS_TIME
	                        }
	                        else
	                        {
	                            // It is not possible to detect polarity after two different polarity attempts
	                        	*io->Mcl_Fault = UNABLE_TO_DETECT_FIRST_ROTATION;
	                            //Mcl__FailureHandler();
	                        }
	                    }
	                }
	                break;

	            case MCISLFAC_STARTUP_PULSE_ROTATE:
	                if(*io->Triac_Trigger_VZC_State == *io->MciSlfac_Prev_VZC_State)
	                {
	                    if(WM_SINGLE_SPEED_SYNCH_SPH_AW == *io->MciSlfac_Wash_Motor_Type)
	                    {
	                        // Initial pulses are to weak. Add next pulse.
	                        if((Initial_Pulse_Voltag_Acc < 3500) &&
	                           ((Initial_Pulse_Current_Max - Initial_Pulse_Current_Min) < params->Flat_Bemf_Detection_Thr)) //FLAT_BEMF_DETECTION_THR
	                        {
	                            if(Initial_Pulse_Voltag_Acc < 1000)
	                            {
	                                TimeCapture__SetPostEventInterruptDelay(TRIAC_FIRE_CHANNEL, MCISLFAC_PERCENT_OF_SEMI_PERIOD(30));
	                            }
	                            else
	                            {
	                                TimeCapture__SetPostEventInterruptDelay(TRIAC_FIRE_CHANNEL, MCISLFAC_PERCENT_OF_SEMI_PERIOD(55));
	                            }
	                            io->ctrl_specific->Control_Mode = MCISLFAC_STARTUP_PULSE_ROTATE_AUX_2;
	                        }
	                        else
	                        {
	                        	GoToOpenLoop(io, params);
	                        }
	                    }
	                    else
	                    {
	                        if((Initial_Pulse_Voltag_Acc < 3500) &&
	                           ((Initial_Pulse_Current_Max - Initial_Pulse_Current_Min) < params->Flat_Bemf_Detection_Thr)) //FLAT_BEMF_DETECTION_THR
	                        {
	                            if(Initial_Pulse_Voltag_Acc < 1000)
	                            {
	                                TimeCapture__SetPostEventInterruptDelay(TRIAC_FIRE_CHANNEL, MCISLFAC_PERCENT_OF_SEMI_PERIOD(30));
	                            }
	                            else
	                            {
	                                TimeCapture__SetPostEventInterruptDelay(TRIAC_FIRE_CHANNEL, MCISLFAC_PERCENT_OF_SEMI_PERIOD(60));
	                            }
	                            io->ctrl_specific->Control_Mode = MCISLFAC_STARTUP_PULSE_ROTATE_AUX_2;
	                        }
	                        else
	                        {
	                        	GoToOpenLoop(io, params);
	                        }
	                    }
	                }
	                else
	                {
	                	Initial_Pulse_Voltag_Acc = 0;
	                    ResetUnexpectedMotorStop(io, params);
	                }
	                break;

	            case MCISLFAC_STARTUP_PULSE_ROTATE_AUX_2:
	                if(*io->Triac_Trigger_VZC_State == *io->MciSlfac_Prev_VZC_State)
	                {
	                	GoToOpenLoop(io, params);
	                }
	                break;

	            case MCISLFAC_STARTUP_PULSE_ROTATE_AUX:
	                if(*io->Triac_Trigger_VZC_State == *io->MciSlfac_Prev_VZC_State)
	                {
	                    // Accumulate current from two pulses
	                    if(TRUE == *io->Triac_Trigger_VZC_State)
	                    {
	                        Initial_Pulse_Current_Meas += Initial_Pulse_Current_Max - *io->Offset_Curr;
	                    }
	                    else
	                    {
	                        Initial_Pulse_Current_Meas += *io->Offset_Curr - Initial_Pulse_Current_Min;
	                    }

	                    // There are two compensation equations
	                    if(Initial_Pulse_Current_Meas > ASKOLL_AW_COMPENSATION_CHANGE_THR)
	                    {
	                        triac_time = ((Initial_Pulse_Current_Meas * FLOAT_TO_16F16(ASKOLL_AW_FIRST_PULSE_COMPENSATION_HIGH_A))
	                                     - FLOAT_TO_16F16(ASKOLL_AW_FIRST_PULSE_COMPENSATION_HIGH_B) + (sint32)(DIV_16F16>>1))/(sint32)DIV_16F16;
	                    }
	                    else
	                    {
	                        triac_time = ((Initial_Pulse_Current_Meas * FLOAT_TO_16F16(ASKOLL_AW_FIRST_PULSE_COMPENSATION_LOW_A))
	                                     - FLOAT_TO_16F16(ASKOLL_AW_FIRST_PULSE_COMPENSATION_LOW_B) + (sint32)(DIV_16F16>>1))/(sint32)DIV_16F16;
	                    }

	                    triac_time = MATHCALC__SATURATE_DIRECT(COMMON_FIRST_PULSE_MIN_LIMIT, triac_time, params->First_Pulse_Max_Limit);

	                    TimeCapture__SetPostEventInterruptDelay(TRIAC_FIRE_CHANNEL, MCISLFAC_PERCENT_OF_SEMI_PERIOD(triac_time));

                        triac_time += params->Open_Loop_Angle_Limit_Add; //ASKOLL_AW_OPEN_LOOP_ANGLE_LIMIT_ADD
                        triac_time = MATHCALC__SATURATE_DIRECT(0, triac_time, params->Open_Loop_Max_Limit);
	                    *io->Triac_Start_Time = CONVERT_TO_PI_FORMAT(MCISLFAC_PERCENT_OF_SEMI_PERIOD(triac_time));
	                    *io->Triac_Start_Time_Init = *io->Triac_Start_Time;
	                    io->ctrl_specific->Control_Mode = MCISLFAC_STARTUP_PULSE_ROTATE;
	                }
	                break;

	            case MCISLFAC_OPEN_LOOP:
	                if(TRUE == *io->Triac_Fired)
	                {
	                    // If number of correct tirggers was presnet and current failure is not debounced then go to close loop
	                    if(*io->Correct_Trigger_Debounce > params->Correct_Trigger_Debounce) //CORRECT_TIRGGER_DEBOUNCE
	                    {
	                    	io->ctrl_specific->Control_Mode = MCISLFAC_CLOSE_LOOP;
	                        //GoToCloseLoop();
	                    }
	                }
	                else
	                {
	                    *io->Check_Bemf = TRUE;
	                    *io->Bemf_Aligned = FALSE;
	                    // If triac was not trigger in last semi cycle reset correct debounce cnt
	                    *io->Correct_Trigger_Debounce = 0;
	                }
	                break;

	            case MCISLFAC_CLOSE_LOOP:
	                if(TRUE == *io->Triac_Fired)
	                {
	                    if(TRUE == *io->MciSlfac_Prev_VZC_State)
	                    {
	                        //pi_output_temp = Pi__GetOutput(io->Pi_Bemf_Shift);
	                        pi_output_temp = io->Pi_Bemf_Shift->Output;
#ifdef PW_MOTOR_ENABLED
	                        if(*io->MciSlfac_Wash_Motor_Type == WM_SINGLE_SPEED_SYNCH_SPH_PW)
	                        {
	                            Pi__Calculate(io->Pi_Bemf_Shift, Bemf_Shift_Time);
	                        }
	                        else
#endif
	                        {
	                            if((pi_output_temp >= (io->Pi_Bemf_Shift->Output_Max - CONVERT_TO_PI_FORMAT(MCISLFAC_FRAQ_PERCENT_OF_SEMI_PERIOD_60HZ(50)))) ||
	                               (*io->MciSlfac_Active_Power_Prev < 15))
	                            {
	                                // When Pi_Bemf_Shift is close to limit or power is below limiter reference then start
	                                // stabilize power at nominal Pi_Bemf_Shirt_Limiter reference
	                                //Pi__SetReference(io->Pi_Bemf_Shirt_Limiter, 15);
	                                Pi_Bemf_Shirt_Limiter.Reference = 15;
	                            }
	                            else
	                            {
	                                // When Pi_Bemf_Shift operates correctly at higher power then minimize upper limit
	                                // by setting reference that cannot be reached.
	                                //Pi__SetReference(io->Pi_Bemf_Shirt_Limiter, *io->MciSlfac_Active_Power_Prev + 1);
	                                Pi_Bemf_Shirt_Limiter.Reference = *io->MciSlfac_Active_Power_Prev + 1;
	                            }
	                            Pi__Calculate(&Pi_Bemf_Shirt_Limiter, *io->MciSlfac_Active_Power_Prev);
	                            //io->Pi_Bemf_Shift->Output_Max = Pi__GetOutput(io->Pi_Bemf_Shirt_Limiter);
	                            io->Pi_Bemf_Shift->Output_Max = Pi_Bemf_Shirt_Limiter.Output;
	                            Pi__Calculate(io->Pi_Bemf_Shift, *io->Filt_Volt_Sym - 32000);
	                        }

	                        // When voltage drop detected at cycle end then set power drop counter
	                        // After this event also power drop is detected. If not check then check for power drop failure
	                        if(*io->Volt_At_Cycle_End_Inc > params->Volt_At_Cycle_End_Thr)
	                        {
	                            *io->Power_Drop_Enter_Deb = params->Power_Drop_Thr;
	                        }
	                        // Calculate change
	                        //pi_output_temp = Pi__GetOutput(io->Pi_Bemf_Shift)- pi_output_temp;
	                        pi_output_temp = io->Pi_Bemf_Shift->Output - pi_output_temp;

	                        active_power_inc = *io->MciSlfac_Active_Power - MciSlfac_Active_Power_Previous;
	                        filt_active_power_inc = *io->MciSlfac_Active_Power - *io->MciSlfac_Active_Power_Prev;

	                        // Check if change is negative and lower than thr
	                        if(pi_output_temp < (-CONVERT_TO_PI_FORMAT(MCISLFAC_FRAQ_PERCENT_OF_SEMI_PERIOD_60HZ(13))))
	                        {
	                            UpdatePiOutput(io,-1);
	                        }
	                        else
	                        {
	                            if((active_power_inc < -10) &&
	                               (filt_active_power_inc < -10) &&
	                               (*io->Power_Drop_Enter_Deb != 0) &&
	                               (*io->Triac_Fired_Prev == TRUE))
	                            {
	                            	*io->Power_Drop_Enter_Deb = 0;
	                                *io->Power_Drop_Exit_Deb = params->Power_Drop_Thr;
	                                if(active_power_inc < -40)
	                                {
	                                    // Decrease PI update limit to speed up recovery
	                                    *io->Triac_Start_Time = *io->Triac_Start_Time_Init - CONVERT_TO_PI_FORMAT(MCISLFAC_PERCENT_OF_SEMI_PERIOD_60HZ(4));
	                                    UpdatePiOutput(io,-9);
	                                }
	                                else if(active_power_inc < -30)
	                                {
	                                    // Decrease PI update limit to speed up recovery
	                                    *io->Triac_Start_Time = *io->Triac_Start_Time_Init - CONVERT_TO_PI_FORMAT(MCISLFAC_PERCENT_OF_SEMI_PERIOD_60HZ(2));
	                                    UpdatePiOutput(io,-7);
	                                }
	                                else if(active_power_inc < -14)
	                                {
	                                    // Decrease PI update limit to speed up recovery
	                                    *io->Triac_Start_Time = *io->Triac_Start_Time_Init - CONVERT_TO_PI_FORMAT(MCISLFAC_PERCENT_OF_SEMI_PERIOD_60HZ(2));
	                                    UpdatePiOutput(io,-5);
	                                }
	                                else
	                                {
	                                    UpdatePiOutput(io,-2);
	                                }
	                            }
	                        }

#ifdef PW_MOTOR_ENABLED
	                        if((io->Pi_Bemf_Shift->Error == 0) && (*io->MciSlfac_Wash_Motor_Type == WM_SINGLE_SPEED_SYNCH_SPH_PW))
	                        {
	                            if(*io->Filt_Volt_Sym > CONVERT_TO_OBSERVER_FORMAT(params->Bemf_Shift_PI_Ref_Update_Thr)) //BEMF_SHIFT_PI_REF_UPDATE_THR
	                            {
	                                if(io->Pi_Bemf_Shift->Reference > 32)
	                                {
	                                	io->Pi_Bemf_Shift->Reference--;
	                                }
	                            }
	                            else if(*io->Filt_Volt_Sym < CONVERT_TO_OBSERVER_FORMAT(-params->Bemf_Shift_PI_Ref_Update_Thr)) //BEMF_SHIFT_PI_REF_UPDATE_THR
	                            {

	                                if(io->Pi_Bemf_Shift->Reference < (uint16)params->Bemf_Shift_Reference)  //ASKOLL_PW_BEMF_SHIFT_REFERENCE
	                                {
	                                	io->Pi_Bemf_Shift->Reference++;
	                                }
	                            }
	                        }
#endif
	                        TimeCapture__SetPostEventInterruptDelay(TRIAC_FIRE_CHANNEL, CONVERT_FROM_PI_FORMAT(io->Pi_Bemf_Shift->Output));
	                    }
	                }
	                else
	                {
	                    if((*io->Power_Drop_Exit_Deb != 0) ||
	                       (*io->Power_Drop_Enter_Deb != 0))
	                    {
	                    	*io->Mcl_Fault = POWER_DROP;
	                        //Mcl__FailureHandlerRecover(POWER_DROP);
	                    }
	                    else
	                    {
	                        *io->Check_Bemf = TRUE;
	                        *io->Bemf_Aligned = FALSE;
#ifdef PW_MOTOR_ENABLED
	                        if(*io->MciSlfac_Wash_Motor_Type == WM_SINGLE_SPEED_SYNCH_SPH_PW)
	                        {
	                            UpdatePiOutput(io,-3);
	                            TimeCapture__SetPostEventInterruptDelay(TRIAC_FIRE_CHANNEL, CONVERT_FROM_PI_FORMAT(Pi__GetOutput(io->Pi_Bemf_Shift)));
	                        }
#endif
	                    }
	                }
	                break;

	            default:
	                break;
	        };
	        *io->Triac_Fired_Prev = *io->Triac_Fired;
	        *io->Triac_Fired = FALSE;
	        MciSlfac_Active_Power_Previous = *io->MciSlfac_Active_Power;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 *
 */
void MclFireAngleCtrl__MotorControlHandler(MCL_FIREANGLE_CTRL_IO_TYPE *io, MCL_PARAMS_TYPE* params)
{
	uint32 temp_u32;
	sint16 volt_sym;
#ifdef PW_MOTOR_ENABLED
	uint16 bemf_shift_temp = 0;
#endif

    volt_sym = 0;

    switch(io->ctrl_specific->Control_Mode)
    {
        case MCISLFAC_STARTUP_PULSE_MEAS:
        case MCISLFAC_STARTUP_PULSE_MEAS_OPPOSITE:
        case MCISLFAC_STARTUP_PULSE_ROTATE_AUX:
            if((FALSE == *io->Zero_Current_Detected) && (*io->Triac_Trigger_VZC_State == *io->MciSlfac_Prev_VZC_State))
            {
                if(*io->raw_curr > Initial_Pulse_Current_Max)
                {
                    Initial_Pulse_Current_Max = *io->raw_curr;
                }

                if(*io->raw_curr < Initial_Pulse_Current_Min)
                {
                    Initial_Pulse_Current_Min = *io->raw_curr;
                }
            }
            break;

        case MCISLFAC_STARTUP_PULSE_ROTATE:
            if(TRUE == *io->zero_current_prev)
            {
                Initial_Pulse_Voltag_Acc += MATHCALC__ABS(*io->bemf_volt_tmp);
                if(*io->filt_volt > Filt_Volt_Max)
                {
                	Filt_Volt_Max = *io->filt_volt;
                }

                if(*io->filt_volt < Filt_Volt_Min)
                {
                	Filt_Volt_Min = *io->filt_volt;
                }
            }
            else if(*io->zero_current_prev != *io->Zero_Current_Detected)
            {
            	MclObserver__ResetState(params);
            }
            break;

        case MCISLFAC_STARTUP_WAIT:
            // Calculate average value to get offset
            IIR_Prev_Curr_Offset += *io->raw_curr;
            IIR_Prev_Volt_Offset += *io->raw_volt;
            if(*io->State_Timeout == *io->Mcl_Main_Counter)
            {
            	temp_u32 = (uint32)Wait_Param;
                *io->Offset_Curr = (uint16)(IIR_Prev_Curr_Offset / temp_u32); //WAIT_BETWEEN_MEAS_TIME
                *io->Offset_Volt = (uint16)(IIR_Prev_Volt_Offset / temp_u32); //WAIT_BETWEEN_MEAS_TIME
                // Init filters with precise value
                IIR_Prev_Curr_Offset = *io->Offset_Curr << params->Offset_Alpha_IIR; //OFFSET_ALPHA_IIR
                IIR_Prev_Volt_Offset = *io->Offset_Volt << params->Offset_Alpha_IIR; //OFFSET_ALPHA_IIR
                io->ctrl_specific->Control_Mode = MCISLFAC_STARTUP_INIT_PULSE_MEAS_OPPOSITE;
            }
            break;

        case MCISLFAC_OPEN_LOOP:
            if(*io->State_Timeout == *io->Mcl_Main_Counter)
            {
                // For no load dont detect UNABLE_TO_CLOSE_LOOP failure because reverse direction start is acceptable
#ifdef PW_MOTOR_ENABLED
                if((*io->Bemf_Aligned == FALSE) || (*io->MciSlfac_Wash_Motor_Type == WM_SINGLE_SPEED_SYNCH_SPH_PW))
#else
                if(*io->Bemf_Aligned == FALSE)
#endif
                {
                    *io->Mcl_Fault = UNABLE_TO_CLOSE_LOOP;
                    //FailureHandler();
                }
                else
                {
                	io->ctrl_specific->Control_Mode = MCISLFAC_CLOSE_LOOP;
                    //GoToCloseLoop();
                }
            }
            //lint -fallthrough
            //no break
        case MCISLFAC_CLOSE_LOOP:
        	*io->Offset_Curr = IIRFilterLong(&IIR_Prev_Curr_Offset, *io->raw_curr, params->Offset_Alpha_IIR); //OFFSET_ALPHA_IIR
        	*io->Offset_Volt = IIRFilterLong(&IIR_Prev_Volt_Offset, *io->raw_volt, params->Offset_Alpha_IIR); //OFFSET_ALPHA_IIR

            if(TRUE == *io->zero_current_prev)
            {
                if(*io->filt_volt > Filt_Volt_Max)
                {
                	Filt_Volt_Max = *io->filt_volt;
                }

                if(*io->filt_volt < Filt_Volt_Min)
                {
                	Filt_Volt_Min = *io->filt_volt;
                }

                if(*io->raw_volt > *io->Volt_Max)
                {
                	*io->Volt_Max = *io->raw_volt;

                    if(*io->Prev_Volt_Max_Ready == TRUE)
                    {
                        if(TRUE == *io->MciSlfac_Prev_VZC_State)
                        {
                        	volt_sym = (sint16)(*io->Prev_Volt_Max) - (sint16)*io->raw_volt;
                        }
                        else
                        {
                        	volt_sym = (sint16)*io->raw_volt - (sint16)(*io->Prev_Volt_Max);
                        }

                        // When bemf symmetry value is very low it means that BEMF peak is aligned with peak of line voltage
                        if(volt_sym < (-800))
                        {
                        	*io->Bemf_Aligned = TRUE;
                        }
                    }
                }

                if(Bemf_Not_Changed_Event == *io->Mcl_Main_Counter)
                {
                    if((Filt_Volt_Max < (*io->Offset_Volt + params->Motor_Stop_Detection_Thr)) &&
                       (Filt_Volt_Min > (*io->Offset_Volt - params->Motor_Stop_Detection_Thr))) //MOTOR_STOP_DETECTION_THR
                    {
                        // Bemf is flat and close to 0. Detect failure.
                        *io->Mcl_Fault = UNEXPECTED_MOTOR_STOP;
                        //FailureHandler();
                    }
                    ResetUnexpectedMotorStop(io, params);
                }
            }
            else
            {
            	*io->Volt_Max = 0;
                ResetUnexpectedMotorStop(io, params);
            }

            if(*io->Filt_Bemf_Volt > CONVERT_TO_OBSERVER_FORMAT(0))
            {
                if(ROTOR_POSITIVE == *io->Rotor_State)
                {
#ifdef PW_MOTOR_ENABLED
                    if(0 == Rotor_State_Deb)
                    {
                    	bemf_shift_temp = (uint16)(*io->Mcl_Main_Counter - *io->VZC_Time_P);
                        Bemf_Shift_Time = IIRFilter(&IIR_Prev_Bemf_Shift_Time, bemf_shift_temp, params->Bemf_Shift_Alpha_IIR); //BEMF_SHIFT_ALPHA_IIR
                    }
#endif
                    Rotor_State_Deb ++;
                    if(Rotor_State_Deb >= params->Rotor_Debounce_Thr) //ROTOR_DEBOUNCE_THR
                    {
                    	*io->Rotor_State = ROTOR_NEGATIVE;
                    }
                }
                else
                {
                    Rotor_State_Deb = 0;
                }
            }
            else if(*io->Filt_Bemf_Volt < CONVERT_TO_OBSERVER_FORMAT(0))
            {
                if(ROTOR_NEGATIVE == *io->Rotor_State)
                {
#ifdef PW_MOTOR_ENABLED
                    if(0 == Rotor_State_Deb)
                    {
                    	bemf_shift_temp = (uint16)(*io->Mcl_Main_Counter - *io->VZC_Time_N);
                        Bemf_Shift_Time = IIRFilter(&IIR_Prev_Bemf_Shift_Time, bemf_shift_temp, params->Bemf_Shift_Alpha_IIR); //BEMF_SHIFT_ALPHA_IIR
                    }
#endif
                    Rotor_State_Deb ++;
                    if(Rotor_State_Deb >= params->Rotor_Debounce_Thr) //ROTOR_DEBOUNCE_THR
                    {
                    	*io->Rotor_State = ROTOR_POSITIVE;
                    }
                }
                else
                {
                    Rotor_State_Deb = 0;
                }
            }

            //activate_triac = TriggerTriac(); --- MclTriacDrive module handles it
            break;

        case MCISLFAC_STOP_CHECK:
            if(PeakDetector__GetPeak2Peak(PEAK_MCI_VOLT_CHANNEL) > STOP_CHECK_EXIT_THRESHOLD)
            {
                *io->Stop_Debounce_Event = *io->Mcl_Main_Counter + (MCISLFAC_MILI_SECONDS_TO_TIMER_TICK(WAIT_TIME_STOP_CHECK_MS, params->Mcisflac_Fast_Handler_Frequency));
            }

            if((*io->State_Timeout == *io->Mcl_Main_Counter) ||
               (*io->Stop_Debounce_Event == *io->Mcl_Main_Counter))
            {
                if(*io->Failure_State == FAILURE_RECOVERY)
                {
                	io->ctrl_specific->Control_Mode = MCISLFAC_STARTUP;
                }
                else
                {
                	io->ctrl_specific->Control_Mode = MCISLFAC_IDLE;
                }

                //Clear the Last Failure...
                *io->MciSlfac_Last_Failure = NO_FAILURE;
            }
            break;

        default:
            break;
    };
    *io->Prev_Volt = *io->raw_volt;
}
//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * Update pi output with proper saturation
 *
 */
static void UpdatePiOutput(MCL_FIREANGLE_CTRL_IO_TYPE *io, sint8 percent_of_semicycle)
{
    sint16 output_change;
    sint16 output;

    output_change = 0;
    output = Pi__GetOutput(io->Pi_Bemf_Shift);

    if(output > *io->Triac_Start_Time)
    {
        output_change = -CONVERT_TO_PI_FORMAT(MCISLFAC_PERCENT_OF_SEMI_PERIOD_60HZ(-percent_of_semicycle));

        output = output + output_change;
        if(output > *io->Triac_Start_Time)
        {
            Pi__SetOutput(io->Pi_Bemf_Shift, output);
        }
        else
        {
            Pi__SetOutput(io->Pi_Bemf_Shift, *io->Triac_Start_Time);
        }
    }
}



//---------------------------------------------------------------------------------------------------------------------
/**
 * Go to open loop state
 *
 */
static void GoToOpenLoop(MCL_FIREANGLE_CTRL_IO_TYPE *io, MCL_PARAMS_TYPE* params)
{
#if(MCISLFAC_DEBUG == ENABLED)
    Debug_Stop_Time = MCISLFAC_GET_EVENT_TIME(MCISLFAC_MILI_SECONDS_TO_TIMER_TICK(55, params->Mcisflac_Fast_Handler_Frequency));
#endif

#ifdef PW_MOTOR_ENABLED
    IIR_Prev_Bemf_Shift_Time = params->Bemf_Shift_Init_Value<<params->Bemf_Shift_Alpha_IIR; //BEMF_SHIFT_INIT_VALUE //BEMF_SHIFT_ALPHA_IIR
#endif
    *io->IIR_Prev_Volt_Symmetry = CONVERT_TO_OBSERVER_FORMAT(0)<<2;
    *io->Filt_Volt_Sym = CONVERT_TO_OBSERVER_FORMAT(0);
    Pi__Initialize(&Pi_Bemf_Shirt_Limiter);
    Pi__SetSaturation(&Pi_Bemf_Shirt_Limiter, CONVERT_TO_PI_FORMAT(MCISLFAC_PERCENT_OF_SEMI_PERIOD_60HZ(params->Bemf_Shift_PI_Max_Out)),
                                              CONVERT_TO_PI_FORMAT(MCISLFAC_PERCENT_OF_SEMI_PERIOD_60HZ(73))); // Value set for AW //BEMF_SHIFT_PI_MAX_OUT
    //Pi__SetCoefficientA(io->Pi_Bemf_Shirt_Limiter, -9000);
    Pi_Bemf_Shirt_Limiter.Coefficient_A = -params->Shirt_Limiter_Coff_A;
    //Pi__SetCoefficientB(io->Pi_Bemf_Shirt_Limiter, 6000);
    Pi_Bemf_Shirt_Limiter.Coefficient_B = params->Shirt_Limiter_Coff_B;
    Pi__SetReference(&Pi_Bemf_Shirt_Limiter, 15);
    Pi_Bemf_Shirt_Limiter.Reference = 15;
    //Pi__SetOutput(io->Pi_Bemf_Shirt_Limiter, CONVERT_TO_PI_FORMAT(MCISLFAC_PERCENT_OF_SEMI_PERIOD_60HZ(params->Bemf_Shift_PI_Max_Out))); //BEMF_SHIFT_PI_MAX_OUT
    Pi_Bemf_Shirt_Limiter.Output = CONVERT_TO_PI_FORMAT(MCISLFAC_PERCENT_OF_SEMI_PERIOD_60HZ(params->Bemf_Shift_PI_Max_Out)); //BEMF_SHIFT_PI_MAX_OUT

    Pi__Initialize(io->Pi_Bemf_Shift);
    //Pi__SetCoefficientC(&Pi_Bemf_Shift, 0);

    Pi__SetSaturation(io->Pi_Bemf_Shift, CONVERT_TO_PI_FORMAT(MCISLFAC_PERCENT_OF_SEMI_PERIOD_60HZ(COMMON_FIRST_PULSE_MIN_LIMIT)),
                                      CONVERT_TO_PI_FORMAT(MCISLFAC_PERCENT_OF_SEMI_PERIOD_60HZ(params->Bemf_Shift_PI_Max_Out))); //COMMON_FIRST_PULSE_MIN_LIMIT //BEMF_SHIFT_PI_MAX_OUT

    //Pi__SetCoefficientA(io->Pi_Bemf_Shift, params->Bemf_Shift_PI_A_Param); //BEMF_SHIFT_PI_A_PARAM / AW = 500     //if feedback is changed by 1 output is changed by: (1*(30000-0))/1024=29
    io->Pi_Bemf_Shift->Coefficient_A = params->Bemf_Shift_PI_A_Param; //BEMF_SHIFT_PI_A_PARAM / AW = 500     //if feedback is changed by 1 output is changed by: (1*(30000-0))/1024=29
    //Pi__SetCoefficientB(io->Pi_Bemf_Shift, params->Bemf_Shift_PI_B_Param); //BEMF_SHIFT_PI_B_PARAM / AW = -333
    io->Pi_Bemf_Shift->Coefficient_B = params->Bemf_Shift_PI_B_Param; //BEMF_SHIFT_PI_B_PARAM / AW = -333
    //Pi__SetReference(io->Pi_Bemf_Shift, (uint16)params->Bemf_Shift_Reference); //ASKOLL_PW_BEMF_SHIFT_REFERENCE / AW = 0
    io->Pi_Bemf_Shift->Reference = (uint16)params->Bemf_Shift_Reference; //ASKOLL_PW_BEMF_SHIFT_REFERENCE / AW = 0

    //Pi__SetOutput(io->Pi_Bemf_Shift, *io->Triac_Start_Time);
    io->Pi_Bemf_Shift->Output = *io->Triac_Start_Time;
    //TimeCapture__SetPostEventInterruptDelay(TRIAC_FIRE_CHANNEL, CONVERT_FROM_PI_FORMAT(Pi__GetOutput(io->Pi_Bemf_Shift)));
    TimeCapture__SetPostEventInterruptDelay(TRIAC_FIRE_CHANNEL, CONVERT_FROM_PI_FORMAT(io->Pi_Bemf_Shift->Output));

    ResetUnexpectedMotorStop(io, params);

    *io->Correct_Trigger_Debounce = 0;
    *io->Check_Bemf = TRUE;
    *io->Bemf_Aligned = FALSE;
    *io->Prev_Volt_Max_Ready = FALSE;
    *io->Volt_Max = 0;
    *io->State_Timeout = *io->Mcl_Main_Counter + (MCISLFAC_MILI_SECONDS_TO_TIMER_TICK(params->Open_Loop_Timeout, params->Mcisflac_Fast_Handler_Frequency)); //OPEN_LOOP_TIMEOUT
    io->ctrl_specific->Control_Mode = MCISLFAC_OPEN_LOOP;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Reset unexpected motor stop failure
 *
 */
static void ResetUnexpectedMotorStop(MCL_FIREANGLE_CTRL_IO_TYPE *io, MCL_PARAMS_TYPE* params)
{
    Bemf_Not_Changed_Event = *io->Mcl_Main_Counter + Reset_Param; //MOTOR_STOP_DETECTION_TIME
    Filt_Volt_Max = 0;
    Filt_Volt_Min = 65535;
}

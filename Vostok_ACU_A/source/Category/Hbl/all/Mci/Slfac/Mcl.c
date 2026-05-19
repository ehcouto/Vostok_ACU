/**
 *  @file
 *  @brief      Motor Control Interface
 *
 *  @details    One interface for any motor present in system
 *
 *  @Module_Owner Leonardo Ricupero / Eduardo H Couto
 *  $Header: Mcl.c 1.00 - July/2020
 *
 *  @copyright  Copyright 2020 - Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "Mcl.h" 
#include "MclZeroCross.h"
#include "MclTriacDrive.h"
#include "MclInputProc.h"
#include "MclObserver.h"
#include "Mci.h"
#include "PeakDetector.h"

#include "Mcl_prv.h"
#include "TimerInterruptExt.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

MCL_PARAMS_TYPE* Mcl_Params;
static uint8 Mci_Params_ID;

MCL_CS_TYPE Mcl_Cs;         //!< MCL control specific variables
MCL_QUANTITIES_TYPE Mcl_Quantities;
MCL_INTERNAL_CL_TYPE Mcl_Internal_ControlLoop;

// Input Output Structures declarations ------ begin
// Input Processing IO
MCL_INPUT_PROC_IO_TYPE Mcl_Input_Proc_IO;

// Observer IO
MCL_OBSERVER_IO_TYPE Mcl_Observer_IO;

// Fire Angle Controller IO
MCL_FIREANGLE_CTRL_IO_TYPE Mcl_FireAngle_Ctrl_IO;

// Triac Drive IO
MCL_TRIAC_DRIVE_IO_TYPE Mcl_Triac_Drive_IO;

// Zero Cross IO
MCL_ZERO_CROSS_IO_TYPE Mcl_Zero_Cross_IO;

#if(MCISLFAC_VWI_DEBUG == ENABLED)
static uint16 MciSlfac_P2P_Voltage;
static uint16 MciSlfac_P2P_Current;
#endif
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//! Converts a value into TRUE or FALSE; 0 is FALSE, TRUE otherwise.

// Input Output Structures declarations ------ end

// Private variables
#if (MCL_DEBUG_STARTUP_TUNING == ENABLED)
uint16 Dbg_Fire_Angle_Startup_Tuning = 0;
uint16 Dbg_Fire_Angle_Open_Loop = 0;
#endif
// Private defines

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void MclInitializeParameters(void);
static void MclZeroCrossControlLoop(void);
static void MclCheckErrorsFastHandler(void);
static void MclResetVariables(void);
static void GoToStopCheck(void);
static void FailureHandler(void);
static void FailureHandlerRecover(MCI_ERROR_TYPE last_failure);

uint16 Total_Restart_Cnt;
uint16 Restart_Cnt;
uint16 Total_Start_Cnt;
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module Mcl and its variables
 *
 */
void Mcl__Initialize(void)
{
    // IO connections
    // Macroblock --> MclInputProc
	Mcl_Input_Proc_IO.ctrl_specific  = &Mcl_Cs;
    Mcl_Input_Proc_IO.Mcl_Fault      = &Mcl_Quantities.Mcl_Fault;
    Mcl_Input_Proc_IO.Offset_Volt     = &Mcl_Quantities.Offset_Volt;
    Mcl_Input_Proc_IO.Offset_Curr     = &Mcl_Quantities.Offset_Curr;
    Mcl_Input_Proc_IO.Zero_Current_Detected     = &Mcl_Quantities.Zero_Current_Detected;

    Mcl_Input_Proc_IO.raw_curr = &Mcl_Internal_ControlLoop.raw_curr;
    Mcl_Input_Proc_IO.raw_volt = &Mcl_Internal_ControlLoop.raw_volt;
    Mcl_Input_Proc_IO.sig_raw_volt = &Mcl_Internal_ControlLoop.sig_raw_volt;
    Mcl_Input_Proc_IO.sig_raw_curr = &Mcl_Internal_ControlLoop.sig_raw_curr;
    Mcl_Input_Proc_IO.abs_raw_curr = &Mcl_Internal_ControlLoop.abs_raw_curr;
    Mcl_Input_Proc_IO.dif_raw_curr = &Mcl_Internal_ControlLoop.dif_raw_curr;
    Mcl_Input_Proc_IO.zero_current_prev = &Mcl_Internal_ControlLoop.zero_current_prev;

    // Macroblock --> MclObserver
    Mcl_Observer_IO.ctrl_specific  = &Mcl_Cs;
    Mcl_Observer_IO.Mcl_Main_Counter = &Mcl_Cs.Mcl_Main_Counter;
    Mcl_Observer_IO.VZC_Time_P = &Mcl_Quantities.VZC_Time_P;
    Mcl_Observer_IO.Triac_Fired_Prev = &Mcl_Quantities.Triac_Fired_Prev;
    Mcl_Observer_IO.Triac_Fired = &Mcl_Quantities.Triac_Fired;
    Mcl_Observer_IO.MciSlfac_Active_Power = &Mcl_Quantities.MciSlfac_Active_Power;
    Mcl_Observer_IO.Pi_Bemf_Shift = &Mcl_Quantities.Pi_Bemf_Shift;
    Mcl_Observer_IO.Power_Drop_Enter_Deb = &Mcl_Quantities.Power_Drop_Enter_Deb;
    Mcl_Observer_IO.Power_Drop_Exit_Deb = &Mcl_Quantities.Power_Drop_Exit_Deb;
    Mcl_Observer_IO.Triac_Start_Time = &Mcl_Quantities.Triac_Start_Time;
    Mcl_Observer_IO.Triac_Start_Time_Init = &Mcl_Quantities.Triac_Start_Time_Init;
    Mcl_Observer_IO.MciSlfac_Prev_VZC_State = &Mcl_Quantities.MciSlfac_Prev_VZC_State;
    Mcl_Observer_IO.Prev_Volt = &Mcl_Quantities.Prev_Volt;
    Mcl_Observer_IO.Triac_State = &Mcl_Quantities.Triac_State;
    Mcl_Observer_IO.MciSlfac_Wash_Motor_Type = &Mcl_Quantities.MciSlfac_Wash_Motor_Type;
    Mcl_Observer_IO.Filt_Volt_Increase = &Mcl_Quantities.Filt_Volt_Increase;
    Mcl_Observer_IO.Offset_Volt = &Mcl_Quantities.Offset_Volt;
    Mcl_Observer_IO.Bemf_Aligned = &Mcl_Quantities.Bemf_Aligned;
    Mcl_Observer_IO.Filt_Bemf_Volt = &Mcl_Quantities.Filt_Bemf_Volt;
    Mcl_Observer_IO.Direct_BEMF_State = &Mcl_Quantities.Direct_BEMF_State;
    Mcl_Observer_IO.Zero_Current_Detected = &Mcl_Quantities.Zero_Current_Detected;
    Mcl_Observer_IO.MciSlfac_Active_Power_Prev = &Mcl_Quantities.MciSlfac_Active_Power_Prev;

    Mcl_Observer_IO.raw_volt = &Mcl_Internal_ControlLoop.raw_volt;
    Mcl_Observer_IO.filt_volt = &Mcl_Internal_ControlLoop.filt_volt;
    Mcl_Observer_IO.sig_raw_volt = &Mcl_Internal_ControlLoop.sig_raw_volt;
    Mcl_Observer_IO.sig_raw_curr = &Mcl_Internal_ControlLoop.sig_raw_curr;
    Mcl_Observer_IO.abs_raw_curr = &Mcl_Internal_ControlLoop.abs_raw_curr;
    Mcl_Observer_IO.dif_raw_curr = &Mcl_Internal_ControlLoop.dif_raw_curr;
    Mcl_Observer_IO.bemf_volt_tmp = &Mcl_Internal_ControlLoop.bemf_volt_tmp;
    Mcl_Observer_IO.zero_current_prev = &Mcl_Internal_ControlLoop.zero_current_prev;

    // Macroblock --> MclZeroCross
    Mcl_Zero_Cross_IO.Zero_Cross_Event  = &Mcl_Quantities.Zero_Cross_Event;
    Mcl_Zero_Cross_IO.MciSlfac_Prev_VZC_State = &Mcl_Quantities.MciSlfac_Prev_VZC_State;
    Mcl_Zero_Cross_IO.Semi_Period_Time_Raw = &Mcl_Quantities.Semi_Period_Time_Raw;

    // Macroblock --> MclFireAngleCtrl
    Mcl_FireAngle_Ctrl_IO.ctrl_specific  = &Mcl_Cs;
    Mcl_FireAngle_Ctrl_IO.Mcl_Fault = &Mcl_Quantities.Mcl_Fault;
    Mcl_FireAngle_Ctrl_IO.MciSlfac_Last_Failure = &Mcl_Quantities.MciSlfac_Last_Failure;
    Mcl_FireAngle_Ctrl_IO.Failure_State = &Mcl_Cs.Failure_State;
    Mcl_FireAngle_Ctrl_IO.Mcl_Main_Counter = &Mcl_Cs.Mcl_Main_Counter;

    Mcl_FireAngle_Ctrl_IO.Zero_Cross_Event  = &Mcl_Quantities.Zero_Cross_Event;
    Mcl_FireAngle_Ctrl_IO.MciSlfac_Prev_VZC_State = &Mcl_Quantities.MciSlfac_Prev_VZC_State;
    Mcl_FireAngle_Ctrl_IO.MciSlfac_Wash_Motor_Type = &Mcl_Quantities.MciSlfac_Wash_Motor_Type;
    Mcl_FireAngle_Ctrl_IO.Triac_Fired = &Mcl_Quantities.Triac_Fired;
    Mcl_FireAngle_Ctrl_IO.Triac_Fired_Prev = &Mcl_Quantities.Triac_Fired_Prev;
    Mcl_FireAngle_Ctrl_IO.Triac_Trigger_VZC_State = &Mcl_Quantities.Triac_Trigger_VZC_State;
    Mcl_FireAngle_Ctrl_IO.Offset_Curr = &Mcl_Quantities.Offset_Curr;
    Mcl_FireAngle_Ctrl_IO.Offset_Volt = &Mcl_Quantities.Offset_Volt;
    Mcl_FireAngle_Ctrl_IO.Triac_Start_Time = &Mcl_Quantities.Triac_Start_Time;
    Mcl_FireAngle_Ctrl_IO.Triac_Start_Time_Init = &Mcl_Quantities.Triac_Start_Time_Init;
    Mcl_FireAngle_Ctrl_IO.Check_Bemf = &Mcl_Quantities.Check_Bemf;
    Mcl_FireAngle_Ctrl_IO.Bemf_Aligned = &Mcl_Quantities.Bemf_Aligned;
    Mcl_FireAngle_Ctrl_IO.Correct_Trigger_Debounce = &Mcl_Quantities.Correct_Trigger_Debounce;
    Mcl_FireAngle_Ctrl_IO.Pi_Bemf_Shift = &Mcl_Quantities.Pi_Bemf_Shift;
    Mcl_FireAngle_Ctrl_IO.MciSlfac_Active_Power_Prev = &Mcl_Quantities.MciSlfac_Active_Power_Prev;
    Mcl_FireAngle_Ctrl_IO.MciSlfac_Active_Power = &Mcl_Quantities.MciSlfac_Active_Power;
    Mcl_FireAngle_Ctrl_IO.Filt_Volt_Sym = &Mcl_Quantities.Filt_Volt_Sym;
    Mcl_FireAngle_Ctrl_IO.Volt_At_Cycle_End_Inc = &Mcl_Quantities.Volt_At_Cycle_End_Inc;
    Mcl_FireAngle_Ctrl_IO.Power_Drop_Enter_Deb = &Mcl_Quantities.Power_Drop_Enter_Deb;
    Mcl_FireAngle_Ctrl_IO.Power_Drop_Exit_Deb = &Mcl_Quantities.Power_Drop_Exit_Deb;
    Mcl_FireAngle_Ctrl_IO.State_Timeout = &Mcl_Quantities.State_Timeout;
    Mcl_FireAngle_Ctrl_IO.IIR_Prev_Volt_Symmetry = &Mcl_Quantities.IIR_Prev_Volt_Symmetry;
    Mcl_FireAngle_Ctrl_IO.Prev_Volt_Max_Ready = &Mcl_Quantities.Prev_Volt_Max_Ready;
    Mcl_FireAngle_Ctrl_IO.Volt_Max = &Mcl_Quantities.Volt_Max;
    Mcl_FireAngle_Ctrl_IO.Prev_Volt = &Mcl_Quantities.Prev_Volt;
    Mcl_FireAngle_Ctrl_IO.Zero_Current_Detected = &Mcl_Quantities.Zero_Current_Detected;
    Mcl_FireAngle_Ctrl_IO.Prev_Volt_Max = &Mcl_Quantities.Prev_Volt_Max;
    Mcl_FireAngle_Ctrl_IO.Filt_Bemf_Volt = &Mcl_Quantities.Filt_Bemf_Volt;
    Mcl_FireAngle_Ctrl_IO.Rotor_State = &Mcl_Quantities.Rotor_State;
    Mcl_FireAngle_Ctrl_IO.VZC_Time_P = &Mcl_Quantities.VZC_Time_P;
    Mcl_FireAngle_Ctrl_IO.VZC_Time_N = &Mcl_Quantities.VZC_Time_N;
    Mcl_FireAngle_Ctrl_IO.Stop_Debounce_Event = &Mcl_Quantities.Stop_Debounce_Event;

    Mcl_FireAngle_Ctrl_IO.zero_current_prev = &Mcl_Internal_ControlLoop.zero_current_prev;
    Mcl_FireAngle_Ctrl_IO.raw_volt = &Mcl_Internal_ControlLoop.raw_volt;
    Mcl_FireAngle_Ctrl_IO.filt_volt = &Mcl_Internal_ControlLoop.filt_volt;
    Mcl_FireAngle_Ctrl_IO.raw_curr = &Mcl_Internal_ControlLoop.raw_curr;
    Mcl_FireAngle_Ctrl_IO.bemf_volt_tmp = &Mcl_Internal_ControlLoop.bemf_volt_tmp;

    // Macroblock --> MclTriacDrive
    Mcl_Triac_Drive_IO.ctrl_specific  = &Mcl_Cs;
    Mcl_Triac_Drive_IO.Mcl_Fault = &Mcl_Quantities.Mcl_Fault;
    Mcl_Triac_Drive_IO.Triac_Trigger_Event = &Mcl_Quantities.Triac_Trigger_Event;
    Mcl_Triac_Drive_IO.Triac_Trigger_VZC_State = &Mcl_Quantities.Triac_Trigger_VZC_State;
    Mcl_Triac_Drive_IO.MciSlfac_Prev_VZC_State = &Mcl_Quantities.MciSlfac_Prev_VZC_State;
    Mcl_Triac_Drive_IO.Check_Bemf = &Mcl_Quantities.Check_Bemf;
    Mcl_Triac_Drive_IO.Direct_BEMF_State = &Mcl_Quantities.Direct_BEMF_State;
    Mcl_Triac_Drive_IO.MciSlfac_Wash_Motor_Type = &Mcl_Quantities.MciSlfac_Wash_Motor_Type;
    Mcl_Triac_Drive_IO.Rotor_State = &Mcl_Quantities.Rotor_State;
    Mcl_Triac_Drive_IO.Filt_Volt_Increase = &Mcl_Quantities.Filt_Volt_Increase;
    Mcl_Triac_Drive_IO.Zero_Current_Detected = &Mcl_Quantities.Zero_Current_Detected;
    Mcl_Triac_Drive_IO.Bemf_Aligned = &Mcl_Quantities.Bemf_Aligned;
    Mcl_Triac_Drive_IO.Triac_Fired = &Mcl_Quantities.Triac_Fired;
    Mcl_Triac_Drive_IO.Correct_Trigger_Debounce = &Mcl_Quantities.Correct_Trigger_Debounce;
    Mcl_Triac_Drive_IO.Volt_Max = &Mcl_Quantities.Volt_Max;
    Mcl_Triac_Drive_IO.Triac_Fired_Prev = &Mcl_Quantities.Triac_Fired_Prev;
    Mcl_Triac_Drive_IO.Prev_Volt_Max_Ready = &Mcl_Quantities.Prev_Volt_Max_Ready;
    Mcl_Triac_Drive_IO.Prev_Volt_Max = &Mcl_Quantities.Prev_Volt_Max;
    Mcl_Triac_Drive_IO.Filt_Volt_Sym = &Mcl_Quantities.Filt_Volt_Sym;
    Mcl_Triac_Drive_IO.IIR_Prev_Volt_Symmetry = &Mcl_Quantities.IIR_Prev_Volt_Symmetry;
    Mcl_Triac_Drive_IO.Prev_Volt = &Mcl_Quantities.Prev_Volt;
    Mcl_Triac_Drive_IO.Offset_Volt = &Mcl_Quantities.Offset_Volt;
    Mcl_Triac_Drive_IO.Triac_State = &Mcl_Quantities.Triac_State;
    Mcl_Triac_Drive_IO.Volt_At_Cycle_End_Inc = &Mcl_Quantities.Volt_At_Cycle_End_Inc;
    Mcl_Triac_Drive_IO.Power_Drop_Enter_Deb = &Mcl_Quantities.Power_Drop_Enter_Deb;

    MclInitializeParameters();

    MathCalc__Initialize();

    // Macroblocks initialization
    MclInputProc__Initialize(&Mcl_Input_Proc_IO, Mcl_Params);
    MclObserver__Initialize(&Mcl_Observer_IO, Mcl_Params);
    MclTriacDrive__Initialize(&Mcl_Triac_Drive_IO, Mcl_Params);
    MclZeroCross__Initialize(&Mcl_Zero_Cross_IO, Mcl_Params);
    MclFireAngleCtrl__Initialize(&Mcl_FireAngle_Ctrl_IO, Mcl_Params);

    // Specific MC driver initialization
    TimerInterruptExt__SetupPerInt((uint16)Mcl_Params->Mcisflac_Fast_Handler_Frequency, 1, 0);

    TimeCapture__ChannelConfig(AC_ZC_SIGNAL_CHANNEL, CHANNEL_CAPTURE_ANY_EDGE, MCISLFAC_TIMER_FREQ_HZ);
    TimeCapture__EnableEventInterrupt(AC_ZC_SIGNAL_CHANNEL);
    TimeCapture__EnablePostEventInterrupt(TRIAC_FIRE_CHANNEL);
    TimeCapture__Start(AC_ZC_SIGNAL_CHANNEL);
    TimeCapture__Start(TRIAC_FIRE_CHANNEL);
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Motor Control Fast handler
 *
 *  @details    This handler is responsible for the main functionalities of the motor control.
 *              It is called at the frequency defined by the parameter MCISLFAC_FAST_HANDLER_FREQUENCY_HZ.
 *
 */
void Mcl__RunningHandler(void)
{
	MclResetVariables();

    if (Mcl_Quantities.Zero_Cross_Event)
    {
        MclZeroCrossControlLoop();
    }
	Mcl_Cs.Mcl_Main_Counter++;

    MclInputProc__RunningHandler(&Mcl_Input_Proc_IO, Mcl_Params);
    MclObserver__RunningHandler(&Mcl_Observer_IO, Mcl_Params);
    MclFireAngleCtrl__MotorControlHandler(&Mcl_FireAngle_Ctrl_IO, Mcl_Params);
    MclTriacDrive__FastHandler(&Mcl_Triac_Drive_IO, Mcl_Params);

    //Checking for faults detected
	if(NO_FAILURE != (MCI_ERROR_TYPE)Mcl_Quantities.Mcl_Fault)
	{
		MclCheckErrorsFastHandler();
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Motor Control Slow Handler
 *
 *  @details    Slow Handler Functionalities
 *
 */
void Mcl__SlowHandler(void)
{
	MclInputProc__CurrentMonitor(&Mcl_Input_Proc_IO, Mcl_Params);

#if(MCISLFAC_VWI_DEBUG == ENABLED)
    MciSlfac_P2P_Voltage = PeakDetector__GetPeak2Peak(PEAK_MCI_VOLT_CHANNEL);
    MciSlfac_P2P_Current = PeakDetector__GetPeak2Peak(PEAK_MCI_CURR_CHANNEL);
#endif // #if(MCISLFAC_VWI_DEBUG == ENABLED)
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Motor Control Zero Cross handler
 *
 *  @details    This handler is called when a zero cross even is detected. Its main tasks are to signal the zero cross event
 *              through a flag to the fast handler, and to provide the value of the semiperiod time to the control loop.
 *
 *  @param[in] event       The zero cross event type
 *  @param[in] ccr_value   The value of the timer counter at the zero cross event, used to measure the semi-period
 *
 */
void Mcl__ZeroCrossHandler(TC_EVENT_TYPE event, uint16 ccr_value)
{
    MCL__ZEROCROSS_HANDLER_BEGIN_CALLBACK();

    if(TC_RISING_EDGE_EVENT == event)
    {
        Mcl_Quantities.MciSlfac_Prev_VZC_State = TRUE;
    }
    else if(TC_FALLING_EDGE_EVENT == event)
    {
    	Mcl_Quantities.MciSlfac_Prev_VZC_State = FALSE;
    }
    // Generate event that will be handled in periodic timer interrupt
    Mcl_Quantities.Zero_Cross_Event = TRUE;
    Mcl_Quantities.Semi_Period_Time_Raw = ccr_value;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Motor Control Triac Fire handler
 *
 *  @details    This handler is called when the control defined time has elapsed from the last zero cross event.
 *              It is used to turn on the motor TRIAC at the requested time. This handler is tied to a timer running at
 *              MCISLFAC_TIMER_FREQ_HZ with a compare threshold which causes this handler to be called.
 *
 */
void Mcl__TriacFireHandler(TC_EVENT_TYPE event)
{
    MclTriacDrive__TriacFireHandler(&Mcl_Triac_Drive_IO, Mcl_Params, event);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * This routine will run the motor with speed_rpm speed and ramp_rpm_per_s acceleration
 *
 * And also, is the acceleration to the next speed. This parameter is POSITIVE the MciSlfac will take care of its
 *           sign if a negative speed or deceleration is needed.
 * @return  MCI_CMD_ACCEPTED - command accepted
 *          MCI_CMD_DENIED - command denied.
 *          MCI_CMD_SPEED_OUT_OF_RANGE - the speed is bigger or smaller than the supported limits.
 *          MCI_CMD_RAMP_OUT_OF_RANGE - the ramp is faster or slower than the supported limits.
 */
MCI_CMD_RESPONSE_TYPE Mcl__Run(MOTOR_ENUM_TYPE motor, sint16 speed_rpm, sint16 ramp_rpm_per_s)
{
    MCI_CMD_RESPONSE_TYPE ret_val;
    ret_val = MCI_CMD_DENIED;

    if((Mcl_Cs.Control_Mode == MCISLFAC_IDLE) &&
       (Mcl_Cs.Failure_State == FAILURE_NOT_DETECTED))
    {
        Total_Start_Cnt++;
        Restart_Cnt = 0;
        Mcl_Cs.Control_Mode = MCISLFAC_STARTUP;
        ret_val = MCI_CMD_ACCEPTED;
    }

    return ret_val;
}



//---------------------------------------------------------------------------------------------------------------------
/**
 * Stop the motor regardless its speed with a desired rate. This commands always accepted.
 *
 * @param   ramp_rpm_per_s
 * @return  MCI_CMD_ACCEPTED - command accepted
 *          MCI_CMD_DENIED - command denied.
 */
MCI_CMD_RESPONSE_TYPE Mcl__Stop(MOTOR_ENUM_TYPE motor, sint16 ramp_rpm_per_s)
{
    MCI_CMD_RESPONSE_TYPE ret_val;
    ret_val = MCI_CMD_DENIED;

    if((Mcl_Cs.Control_Mode != MCISLFAC_IDLE) &&
       (Mcl_Cs.Control_Mode != MCISLFAC_STOP_CHECK))
    {
       	if(MCI_ACC_EXECUTE_IMMEDIATELY == ramp_rpm_per_s)
        {
        	MclTriacDrive__ControlTriac(FALSE);
            Mcl_Cs.Control_Mode = MCISLFAC_IDLE;
        }
        else
        {
        	GoToStopCheck();
        }
       	if(Mcl_Cs.Failure_State == FAILURE_RECOVERY)
       	{
       		Mcl_Cs.Failure_State = FAILURE_NOT_DETECTED;
       	}
        ret_val = MCI_CMD_ACCEPTED;
    }
    return ret_val;
}



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief
 *
 */
MCI_CMD_RESPONSE_TYPE Mcl__ManualControl(MCL_DEBUG_MODE_TYPE mode, sint32 param0, sint32 param1)
{
	return(MCI_CMD_ACCEPTED);
}


BOOL_TYPE Mcl__GetDigital(MCI_DI_TYPE read_bit)
{
	BOOL_TYPE response;
	response = FALSE;

	switch(read_bit)
	{
		case MCI_DI_MOTOR_STOP:
			if((Mcl_Cs.Control_Mode <= MCISLFAC_STARTUP)    ||
			   (Mcl_Cs.Control_Mode == MCISLFAC_STOP_CHECK))
			{
				response = TRUE;
			}
			break;

		case MCI_DI_IDLE:
			if(Mcl_Cs.Control_Mode == MCISLFAC_IDLE)
			{
				response = TRUE;
			}
			break;

		case MCI_DI_MOTOR_RUNNING:
			if((Mcl_Cs.Control_Mode == MCISLFAC_OPEN_LOOP) ||
			   (Mcl_Cs.Control_Mode == MCISLFAC_CLOSE_LOOP))
			{
				response = TRUE;
			}
			break;

		case MCI_DI_REACHED_TARGET_SPEED:
			if(Mcl_Cs.Control_Mode == MCISLFAC_CLOSE_LOOP)
			{
				response = TRUE;
			}
			break;

		case MCI_DI_PARAMETERS_LOADED:
				response = TRUE;
			break;
	}

	return(response);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Read analog data from the MCL
 * @param read_data index to the parameter. See the Mcl.h file
 * @return the parameter value. The default return is -32768. Not implemented functions returns -32768
 */
sint32 Mcl__GetAnalog(MCI_AI_TYPE read_data)
{
	sint32 response;
	response = 0x7FFFFFFE; //Set MAX Positive as default.

	switch(read_data)
	{
		case MCI_AI_CURR_PHASE_P2P:
			response = (sint32)MciSlfac_P2P_Current;
			break;

		case MCI_AI_VOLTAGE_PHASE_P2P:
			response = (sint32)MciSlfac_P2P_Voltage;
			break;

		case MCI_AI_LOAD_TORQUE_S16:
			response = (sint32)Mcl_Quantities.MciSlfac_Active_Power;
			break;

		case MCI_AI_INTERNAL_MCI_STATE:
			response = (sint32)Mcl_Cs.Control_Mode;
			break;
	}

	return(response);
}

/**
 * Read any digital error from the MCL
 * @param input is index of the parameter. See the Mcl.h file
 * @return TRUE/FALSE according to the type of the passed parameter. The default return is FALSE.
 */
BOOL_TYPE Mcl__GetError(MOTOR_ENUM_TYPE motor)
{
	BOOL_TYPE error;
	error = FALSE;

	if(motor == MOTOR0)
	{
		if(Mcl_Cs.Failure_State == FAILURE_DETECTED)
		{
			error = TRUE;
		}
	}

	return(error);
}


/**
 *
 */
uint32 Mcl__GetErrorList(void)
{
	return(Mcl_Quantities.MciSlfac_Last_Failure);
}


/**
 *
 */
uint8 Mcl__ClearError(void)
{
	Mcl_Quantities.MciSlfac_Last_Failure = NO_FAILURE;
	Mcl_Cs.Failure_State = FAILURE_NOT_DETECTED;
	return(TRUE);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Set wash motor type
 *
 */
void Mcl__SetWashMotorType(APL_WASH_MOTOR_TYPE wash_type)
{
	if(wash_type != Mcl_Quantities.MciSlfac_Wash_Motor_Type)
	{
		Mcl_Quantities.MciSlfac_Wash_Motor_Type = wash_type;
	}
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
static void MclZeroCrossControlLoop(void)
{
	Mcl_Quantities.Zero_Cross_Event = FALSE; 	//Clear ZC Flag

    // Any ZC event before MCISLFAC_KEEP_OUT_WINDOW is noise
    if(Mcl_Quantities.Semi_Period_Time_Raw > MCISLFAC_KEEP_OUT_WINDOW)
    {
        Mcl_Quantities.Triac_Trigger_Event = FALSE;
    	MclZeroCross__SemiPeriodFilter(&Mcl_Zero_Cross_IO, Mcl_Params); //Filter the Zero Cross Period

        if(TRUE == Mcl_Quantities.MciSlfac_Prev_VZC_State)
        {
        	MclObserver__ActivePowerEstimation(&Mcl_Observer_IO, Mcl_Params);
        }

#ifdef PW_MOTOR_ENABLED
        if(TRUE == Mcl_Quantities.MciSlfac_Prev_VZC_State)
        {
        	Mcl_Quantities.VZC_Time_P = Mcl_Cs.Mcl_Main_Counter;
        }
        else
        {
        	Mcl_Quantities.VZC_Time_N = Mcl_Cs.Mcl_Main_Counter;
        }
#endif

        //Motor Control at Zero Cross event
        MclFireAngleCtrl__ZeroCrossHandler(&Mcl_FireAngle_Ctrl_IO, Mcl_Params);
    }
    else
    {
    	//If timer reads zero, raise a fault
    	if(Mcl_Quantities.Semi_Period_Time_Raw == 0)
    	{
    		Mcl_Quantities.Mcl_Fault = ZERO_CROSS_NOT_DETECTED;
    	}
    }
}




static void MclCheckErrorsFastHandler(void)
{
	MCI_ERROR_TYPE error;

	error = (MCI_ERROR_TYPE)Mcl_Quantities.Mcl_Fault;

	if(error >= UNDER_VOLTAGE)
	{
		// If error is one of these...
		//UNDER_VOLTAGE
		//UNDER_VOLTAGE_SHORT_CIRCUIT
		//ZERO_CROSS_NOT_DETECTED
		//POWER_DROP
		FailureHandlerRecover(error);
	}
	else
	{
		FailureHandler();
	}

	//Manage Flags
	Mcl_Quantities.MciSlfac_Last_Failure = error;
	Mcl_Quantities.Mcl_Fault = 0;
}




static void MclInitializeParameters(void)
{
#ifndef COMPILE_4_SIMULINK
	GoToStopCheck();
#endif
	Mcl_Quantities.Mcl_Fault = (uint32)NO_FAILURE;
	Mcl_Quantities.MciSlfac_Prev_VZC_State = FALSE;
	Mcl_Quantities.Rotor_State = ROTOR_NEGATIVE;
	Mcl_Quantities.Power_Drop_Exit_Deb = 0;
	Mcl_Quantities.Power_Drop_Enter_Deb = 0;
	Mcl_Quantities.Triac_Fired = FALSE;
	Mcl_Quantities.Zero_Current_Detected = FALSE;
	Mcl_Quantities.Triac_Trigger_VZC_State = FALSE;
	Mcl_Quantities.Triac_Trigger_Event = FALSE;
	Mcl_Quantities.Check_Bemf = FALSE;
	Mcl_Quantities.Bemf_Aligned = FALSE;
	Mcl_Quantities.Prev_Volt_Max_Ready = FALSE;
	Mcl_Quantities.Triac_State = FALSE;
	Mcl_Quantities.Semi_Period_Time_Raw = 50000;
	Mcl_Quantities.Triac_Start_Time = 0;
	Mcl_Quantities.Triac_Start_Time_Init = 0;
	Mcl_Quantities.State_Timeout = 0;
	Mcl_Quantities.Offset_Curr = 0;
	Mcl_Quantities.Offset_Volt = 0;
	Mcl_Quantities.Prev_Volt = 0;
	Mcl_Quantities.Filt_Volt_Increase = 0;
	Mcl_Quantities.Volt_At_Cycle_End_Inc = 0;
	Mcl_Quantities.MciSlfac_Active_Power_Prev = 0;
	Mcl_Quantities.Filt_Bemf_Volt = 0;
	Mcl_Quantities.MciSlfac_Active_Power = 0;
	Mcl_Quantities.Correct_Trigger_Debounce = 0;
	Mcl_Quantities.Volt_Max = 0;
	Mcl_Quantities.Prev_Volt_Max = 0;
	Mcl_Quantities.Filt_Volt_Sym = 0;
	Total_Restart_Cnt = 0;
	Restart_Cnt = 0;
	Mcl_Cs.Mcl_Main_Counter = 0;
	Total_Start_Cnt = 0;
#ifdef PW_MOTOR_ENABLED
	Mcl_Quantities.VZC_Time_P = 0;
	Mcl_Quantities.VZC_Time_N = 0;
#endif

#if(MCISLFAC_VWI_DEBUG == ENABLED)
	MciSlfac_P2P_Voltage = 0;
	MciSlfac_P2P_Current = 0;
#endif

    MclObserver__ResetState(Mcl_Params);
}

static void MclResetVariables(void)
{
#ifdef ENABLE_INTERNAL_RESET
	// Reset all local variables in the motor control handler
    Mcl_Internal_ControlLoop.filt_volt = 0;
    Mcl_Internal_ControlLoop.bemf_volt_tmp = 0;

    Mcl_Internal_ControlLoop.zero_current_prev = FALSE;
    Mcl_Internal_ControlLoop.raw_volt = 0;
    Mcl_Internal_ControlLoop.sig_raw_volt = 0;
    Mcl_Internal_ControlLoop.sig_filt_volt = 0;
    Mcl_Internal_ControlLoop.raw_curr = 0;
    Mcl_Internal_ControlLoop.sig_raw_curr = 0;
    Mcl_Internal_ControlLoop.abs_raw_curr = 0;
    Mcl_Internal_ControlLoop.dif_raw_curr = 0;
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Failure handler
 *
 */
static void FailureHandler(void)
{
    Total_Restart_Cnt++;
    Restart_Cnt++;
    if(Restart_Cnt >= 2)
    {
    	Mcl_Cs.Failure_State = FAILURE_DETECTED;
    }
    else
    {
    	Mcl_Cs.Failure_State = FAILURE_RECOVERY;
    }
    GoToStopCheck();
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Failure handler with recovery only
 *
 */
static void FailureHandlerRecover(MCI_ERROR_TYPE last_failure)
{
#ifdef RECOVERY_AT_STARTUP
   if((Mcl_Cs.Control_Mode != MCISLFAC_IDLE) &&
      (Mcl_Cs.Control_Mode != MCISLFAC_STOP_CHECK) &&
      (Mcl_Cs.Control_Mode != MCISLFAC_STARTUP) &&
      (Mcl_Cs.Control_Mode != MCISLFAC_STARTUP_PULSE_MEAS) &&
      (Mcl_Cs.Control_Mode != MCISLFAC_STARTUP_WAIT))
#else
    if(Mcl_Cs.Control_Mode > MCISLFAC_IDLE)
#endif
    {
        Total_Restart_Cnt++;
        Mcl_Cs.Failure_State = FAILURE_RECOVERY;
        GoToStopCheck();
    }
}



static void GoToStopCheck(void)
{
    Mcl_Quantities.Stop_Debounce_Event = Mcl_Cs.Mcl_Main_Counter + (MCISLFAC_MILI_SECONDS_TO_TIMER_TICK(WAIT_TIME_STOP_CHECK_MS, Mcl_Params->Mcisflac_Fast_Handler_Frequency));
    Mcl_Quantities.State_Timeout = Mcl_Cs.Mcl_Main_Counter + (MCISLFAC_MILI_SECONDS_TO_TIMER_TICK(2000, Mcl_Params->Mcisflac_Fast_Handler_Frequency));
    Mcl_Cs.Control_Mode = MCISLFAC_STOP_CHECK;
}




#if (API220_DEBUG_MODE == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Linked to MciSlfac_debug module - Simulate motor faults
 *
 */
void Mci__Emulate_Error(uint8 fault)
{
	Mcl_Quantities.MciSlfac_Last_Failure = (MCI_ERROR_TYPE)fault;

    if(Mcl_Quantities.MciSlfac_Last_Failure >= UNDER_VOLTAGE)
    {
    	Mcl_Cs.Control_Mode = MCISLFAC_CLOSE_LOOP;
        FailureHandlerRecover(Mcl_Quantities.MciSlfac_Last_Failure);
    }
    else
    {
        FailureHandler();
    }
}
#endif




#if (SIL_MODE == ENABLED)
sint32 Mcl__GetDataSIL(MCL_GET_DATA_SIL_TYPE data)
{
    sint32 ret_val = 0xFFFFFFFF;

    switch(data)
    {
        case MCL_GD_FAILURE_STATE:
            ret_val = (sint32)Mcl_Cs.Failure_State;
            break;

        case MCL_GD_BACK_EMF:
            // Dummy read to removed compiler warning
            ret_val = (sint32)Mcl_Internal_ControlLoop.bemf_volt_tmp;
            break;

        case MCL_GD_MAIN_COUNTER:
             // Dummy read to removed compiler warning
        	ret_val = (sint32)Mcl_Cs.Mcl_Main_Counter;
            break;

        case MCL_GD_MOTOR_TYPE:
            // Dummy read to removed compiler warning
            ret_val = (sint32)Mcl_Quantities.MciSlfac_Wash_Motor_Type;
            break;

        default:
            break;
    };

    return ret_val;
}
#endif




//---------------------------------------------------------------------------------------------------------------------
/**
 * Getting parameters from MCI Interface
 *
 */
BOOL_TYPE Mcl__SetParams(MCL_PARAMS_TYPE* params)
{
	//Initialize Params ID
	Mci_Params_ID = 0xFF;

    if(params != NULL)
    {
        Mcl_Params = params;
        Mci_Params_ID = Mcl_Params->Mci_Params_ID; //Get Params ID
        return(TRUE);
    }
    return(FALSE);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * Get MCI Parameters ID
 *
 */
uint16 Mcl__GetParamsID(void)
{
	return((uint16)Mci_Params_ID);
}

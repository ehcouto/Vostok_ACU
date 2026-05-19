/**
 *  @file       Vwi.c
 *  @brief      Virtual Water Indicator (VWI) sense water inside appliance base on monitored wash motor parameters
 *
 *  @details    There are several different input parameters like motor current, power or phase shift between
 *              voltage and current, suitable for different motors. To one of those input parameters,
 *              two low pass filters are applied to calculate signal average value and variance.
 *              For all kind of signals, calibration is applied which “moves” average value to expected level for water.
 *              For some signals supply voltage and frequency compensation is applied to remove their influence
 *              to average value. Variance is relative measurement and it is not impacted by calibration
 *              and compensation. Based on average and variance water state is set. There is different water state
 *              calculation for MPH and for other motors. For Smart motor reverse rotation detection algorithm
 *              is implemented.
 *
 *
 *  @Copyright  Copyright 2012 - $Date: 2016/03/09 20:16:01CET $  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

//  --- Include Files -------------------------------------------------------------------------------------------------
#include "C_Extensions.h"
#include "SystemConfig.h"
#include "Vwi.h"
#include "Vwi_prv.h"

// -- ANSI/ISO --
#include <limits.h>
#include "String.h"
#include "stdlib.h"

#ifndef VWI_FEATURE
#define VWI_FEATURE	ENABLED
#endif

VWI_WATER_SENSE_TYPE Vwi_Status;
#if (VWI_FEATURE == ENABLED)
// -- Project --
#include "Mode.h"
#if(ACU_MOTOR_CONTROL == ENABLED)
#include "MotorTypeDefs.h"
#endif
#include "SystemTimers.h"
#if (VWI_INTERNAL_COMMUNICATION == DISABLED)
#include "API221MotionCtrl.h"
#endif
#if((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))
#include "IIRFilter.h"
#endif

#if(ACU_MOTOR_CONTROL == ENABLED) && ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))
#include "Mci.h"
//#include "MciSlfac.h" Update Mci for GWS motor
#endif

#if ((VWI_INTERNAL_COMMUNICATION == ENABLED) && (VWI_SINGLE_SPEED_MOTOR_TYPE == ENABLED))
#include "Hbl.h"
#endif

#if ((VWI_CLIENT_FEATURE == ENABLED) || (VWI_SERVER_FEATURE == ENABLED))
typedef struct
{
	uint8 Channel;
	void *Variable_Pointer;
	uint8 Bit_Shift;
	uint8 Size;
} VWI_REMOTE_DATA_FRAME;

typedef struct
{
	uint8 Channel;
	uint16 Period;
	BOOL_TYPE IsActive;
}  VWI_REMOTE_CHANNEL;
#endif //#if ((VWI_CLIENT_FEATURE == ENABLED) || (VWI_SERVER_FEATURE == ENABLED))

CT_ASSERT((VWI_CALIBRATION == ENABLED) || (VWI_CALIBRATION == DISABLED));

#if((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))

#pragma location = "_appliance_segment"     // Need to save data on a black out event
static unsigned short Vwi_Calibration_Correction_NVData[MOTOR_NUM];

signed char Vwi_Calibration_Correction;
static unsigned short Vwi_Signal_Backup[MOTOR_NUM];

#endif // #if((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))

//  --- Public Variables ----------------------------------------------------------------------------------------------

//  --- Private Properties --------------------------------------------------------------------------------------------

#define SQRT_OF_CHAR_MAX            		(0x000F)      	//!< Square root of max unsigned char value
#define DECREASE_RESOLUTION_BY_4_BITS(x)   	((x + 8) >> 4) 	//!< Decrease resolution by 4 bits with rounding. Must be used only for unsigned variables.
#define VWI_HEAVY_FILTER_QUEUE_SIZE 		(4)				//!< Number of samples taken to calculate Heavy filter signal mean value

//! The name of this module for use by the Log module.
#define MODULE_NAME MODULE_VWI

#if ((VWI_THREE_PHASE_MOTOR_TYPE == ENABLED) && ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED)))
static unsigned short Vwi_Signal_Multiplier;
static unsigned short Vwi_Signal_MCU_Raw;
static unsigned short Vwi_Signal_MCU_Mlp;
#endif // ((VWI_THREE_PHASE_MOTOR_TYPE== ENABLED) && ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED)))

// Wash pump signal variables
static unsigned short Vwi_Signal;                       //!< Motor signal to be analyzed
static unsigned short Vwi_Heavy_Filt_Signal;            //!< Heavy filtred Vwi_Signal

// Wash pump signal variance variables
static unsigned short Vwi_Variance;                     //!< Motor signal variance

// Water state variables
              		//!< Final water state
static VWI_WATER_SENSE_TYPE Vwi_Last_Valid_Status;      //!< Last valid water status when measure was active

static uint8 Vwi_Motor_current;
#if((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))
static uint8 Vwi_Motor_prev;
#endif // ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))
#if(VWI_CLIENT_FEATURE == ENABLED)
static uint8 Vwi_Motor_Use;
#endif

#if ((VWI_CLIENT_FEATURE == ENABLED) || (VWI_SERVER_FEATURE == ENABLED))
const VWI_REMOTE_DATA_FRAME Remote_Data[] = DATA_CHANNEL_CONFIG;
VWI_REMOTE_CHANNEL Remote_Data_Channels[] = DATA_CHANNEL_PERIOD_CONFIG;
#endif //#if ((VWI_CLIENT_FEATURE == ENABLED) || (VWI_SERVER_FEATURE == ENABLED))

const uint8 Motor_Settings_Displ[MOTOR_NUM] = VWI_MOTOR_SF_SETTINGS;

#if((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))

typedef PACKED struct VWI_CONTROL_REGISTER_STRUCT
{
	uint8 VWI_Present:1;
	uint8 VWI_Speed_Comp:1;
	uint8 VWI_Calibration:1;
	uint8 VWI_Reserved:5;
}VWI_CONTROL_REGISTER;

//! The data structure of the Heat process parameters as they are found in the setting file.
typedef PACKED struct VWI_PARAMETER_STRUCT
{
    uint8 VWI_Calibration_Time;
    VWI_CONTROL_REGISTER VWI_Control_Reg;
    uint8 VWI_Sig_Water_Threshold;
    uint8 VWI_Sig_Variance_Threshold;
    uint8 VWI_Signal_Low_Water_Threshold;
    uint8 VWI_Signal_Calibration_Target;
#if(ACU_MOTOR_CONTROL == ENABLED)
    APL_WASH_MOTOR_TYPE VWI_Wash_Motor_Type;
#else
    uint8 VWI_Wash_Motor_Type;
#endif
    uint8 VWI_Data_Channel_Period;
    uint32 Vwi_Signal_Max;
    uint16 Speed_Comp_Zero_Level;
    uint16 Min_Comp_Speed;
    uint16 Max_Comp_Speed;
    uint16 Speed_Comp_Step;
    uint16 Sig_Speed_Comp[20];
} VWI_PARAMETER_TYPE;


#if (SPEED_CHANGES_COMP == ENABLED)
static uint16 Speed_Comp_Zero_Level_WM;
static uint16 Min_Comp_Speed;
static uint16 Max_Comp_Speed;
static uint16 Speed_Comp_Step;
static uint16 Sig_Speed_Comp_WM[20];
#endif  //SPEED_CHANGES_COMP

//! A list of log message IDs that are generated by this module.
typedef enum MODULE_VWI_LOG_MESSAGE_ID_ENUM
{
    ERROR_LOADING_PARAMETERS       = 0,
} MODULE_VWI_LOG_MESSAGE_ID_TYPE;

#ifdef VWI_DEBUG
static unsigned char Vwi_Debug_Counter;
#endif //VWI_DEBUG

// Water state variables
static VWI_WATER_SENSE_TYPE Vwi_Status_Raw;             //!< Water state in last measure period
static VWI_WATER_SENSE_TYPE Vwi_Prev_Status_Raw;        //!< Previous water state

static VWI_ANALYZE_PHASE_TYPE  Vwi_Analyze_Phase;       //!< Analyze phase

#if(ACU_MOTOR_CONTROL == ENABLED)
static APL_WASH_MOTOR_TYPE     Vwi_Motor_Type;          //!< Motor type
#endif

static char VWI_Signal_Cal_Target;
unsigned char VWI_Sig_Low_Water_Threshold;
#if (VWI_THREE_PHASE_MOTOR_TYPE == ENABLED)
static uint32 Vwi_Signal_Max;							//!< Parameter to be used to scale the Vwi signal [Q15]
#endif
BOOL_TYPE VWI_Is_Present;
BOOL_TYPE VWI_Is_CalibrationActive;
BOOL_TYPE VWI_Is_SpeedCompActive;

// Wash pump signal variables
static unsigned short Vwi_Light_Filt_Signal;            //!< Light filtred Vwi_Signal

// Warning:  variable "Vwi_Heavy_Filt_Sig_No_Comp" was set but never used
// Used for debug
static unsigned short Vwi_Heavy_Filt_Sig_No_Comp;       //!< Motor signal without compensation applied

static unsigned short Vwi_HeavySignalQueue[VWI_HEAVY_FILTER_QUEUE_SIZE];
static uint8 VWI_Last_Index;

// Wash pump signal variance variables
static unsigned short Vwi_Heavy_Filt_Variance;          //!< Heavy filtred Vwi_Variance

// Signal correction
// PC-lint; 0 Info 728: Symbol 'Vwi_Signal_Correction' not explicitly initialized
static signed char    Vwi_Signal_Correction;            //!< Sum of frequency and voltage compensations
                                                        //!< and signal calibration
static signed char    Vwi_Calibration_Temp_Correction;  //!< Temporary signal correction from calibration
static unsigned short Vwi_Calibration_First_Read;       //!< First read in calibration

// IIR variables
static unsigned short Vwi_Prev_Light_Filt_Signal;       //!< Previous value of Vwi_Light_Filt_Signal
static unsigned short Vwi_Prev_Heavy_Filt_Signal;       //!< Previous value of Vwi_Heavy_Filt_Signal
static unsigned short Vwi_Prev_Heavy_Filt_Sig_No_Comp;  //!< Previous value of Vwi_Heavy_Filt_Sig_No_Comp
static unsigned short Vwi_Prev_Heavy_Filt_Variance;     //!< Previous value of Vwi_Prev_Heavy_Filt_Variance

// Supply parameters
// Parameters
static unsigned short Vwi_Calibration_Time;             //!< Calibration Time SF parameter
static unsigned char  Vwi_Water_Signal_Thr;             //!< Signal Threshold for water SF parameter
static unsigned char  Vwi_Stable_Signal_Variance_Thr;   //!< Stable Signal Variance Thershold SF parameter

static uint16 Vwi_Timer_Handle;
static uint16 Vwi_Debounce_Timer_Handle;

//  --- Private Function Prototypes -----------------------------------------------------------------------------------


static void SetWaterState(void);
static void ApplySignalCorrection(uint8 motor);
static void DebounceWaterState(void);
static void ResetAlgorithm(void);
static void ResetCalibration(void);
static void Calibrate(void);
static void ReadProcessParameters(uint8 motor);
static void ReadVWIParameters(SETTINGFILE_LOADER_TYPE* loader);
static void Reinit(void);
static void VwiMainHandler(uint8 motor);
static void VwiReloadMotor(uint8 motor);

#if ((VWI_INTERNAL_COMMUNICATION == ENABLED) && (VWI_SINGLE_SPEED_MOTOR_TYPE == ENABLED))
BOOL_TYPE IsMotorRotate(void);
#endif

#else // ((VWI_CLIENT_FEATURE == ENABLED)

static BOOL_TYPE IsChannelActive(uint8 channel);
static void GetRemoteData(uint8 motor);
static void StoreData(VWI_REMOTE_DATA_FRAME frame, sint32 data);
static void UpdateRemoteChannels(uint8 motor);

#endif // ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))

#endif // (VWI_FEATURE == ENABLED)

//=====================================================================================================================
//  --- Public Functions ----------------------------------------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * Initialization of VWI functionality. All global variables must be set to 0 before this function call when VWI_INIT_TO_ZERO not defined.
 */
void Vwi__Initialize(void)
{

#if((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))
#if (VWI_FEATURE == ENABLED)
    Vwi_Analyze_Phase = INITIALIZATION;
	ReadProcessParameters(0xFF); //Read default parameters
    VWI_Last_Index = 0;
    memset(Vwi_HeavySignalQueue, 0 , sizeof(Vwi_HeavySignalQueue));

    if(TRUE == VWI_Is_Present)
    {
        Vwi_Timer_Handle = SystemTimers__GetHandle(SYSTEMTIMERS_TYPE_DOWN_COUNTER);
        Vwi_Debounce_Timer_Handle = SystemTimers__GetHandle(SYSTEMTIMERS_TYPE_DOWN_COUNTER);

		SystemTimers__SetMs(Vwi_Timer_Handle, VWI_STABILIZATION_TIME_MS);
		// Set initial calibration value as part of previously calculated
		Vwi_Calibration_Temp_Correction = Vwi_Calibration_Correction / VWI_INIT_CAL_CORRECTION_DIV;
		ResetAlgorithm();
    }
#endif
	Vwi_Motor_current = 0xFF;
    Vwi_Motor_prev = 0xFF;
#if(VWI_CLIENT_FEATURE == ENABLED)
	Vwi_Motor_Use = 0xFF;
#endif
#if (VWI_THREE_PHASE_MOTOR_TYPE == ENABLED)
	Vwi_Signal_Max = MAX_31BIT_POSITIVE;
#endif
#if (VWI_THREE_PHASE_MOTOR_TYPE== ENABLED)
	Vwi_Signal_Multiplier = 1*(1<<12); //4096 in I4Q12 means 1
	Vwi_Signal_MCU_Raw = 0;
	Vwi_Signal_MCU_Mlp = 0;
#endif // (VWI_THREE_PHASE_MOTOR_TYPE== ENABLED)
#endif // ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))

    
}



//---------------------------------------------------------------------------------------------------------------------
/**
 * Assign water status according to filtered wash pump current, power or phase shift value.
 *
 * Call rate is 25ms.
 * It is possible to detect which motor is connected by measuring current and phase shift.
 * If phase shift is higher than phase shift threshold (150) or if phase shift is below threshold and current
 * is below current threshold (60), smart motor is connected. This check should be done in calibration state.
 */
void Vwi__Handler(void)
{
#if(VWI_CLIENT_FEATURE == ENABLED)
	  GetRemoteData(Vwi_Motor_Use);
#endif

#if((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))

#if (VWI_FEATURE == ENABLED)
    Vwi_Motor_current = VWI_GET_MOTOR_ID();

    if(Vwi_Motor_current < MOTOR_NUM)
    {
    	if(Vwi_Motor_current != Vwi_Motor_prev)
    	{
    		VwiReloadMotor(Vwi_Motor_current);
    	}
    	else
    	{
    		if(TRUE == VWI_Is_Present)
    		{
    			VwiMainHandler(Vwi_Motor_current);
    		}
    	}
    }
    else //VWI is disabled
    {
        Vwi_Status = WS_NOT_VALID;
    }
#endif // (VWI_FEATURE == ENABLED)


#endif // ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))
}

#if((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))
/**
 *
 * 	Reload SettingFile parameters for specified motor
 *
 * @param motor - id of current motor
 */
static void VwiReloadMotor(uint8 motor)
{
	Vwi_Status = WS_NOT_VALID;
	Vwi_Last_Valid_Status = WS_NOT_VALID;
	Vwi_Analyze_Phase = INITIALIZATION;

	//Update motor index
	Vwi_Motor_prev = Vwi_Motor_current;

	//Reload motor paramiter
	ReadProcessParameters(Vwi_Motor_current);

	//Initialize all variables with zero
	Vwi_Signal_Correction = 0;
	Vwi_Heavy_Filt_Variance = 0;
	Vwi_Light_Filt_Signal = 0;
	Vwi_Heavy_Filt_Sig_No_Comp = 0;
	Vwi_Heavy_Filt_Signal = 0;
	Vwi_Variance = 0;

#if (VWI_THREE_PHASE_MOTOR_TYPE == ENABLED)
	Vwi_Signal_MCU_Raw = 0;
	Vwi_Signal_MCU_Mlp = 0;
#endif // (VWI_THREE_PHASE_MOTOR_TYPE== ENABLED)

	memset(Vwi_HeavySignalQueue, 0 , sizeof(Vwi_HeavySignalQueue));
	VWI_Last_Index = 0;

	//Restore from backup Vwi_Signal value
	Vwi_Signal = Vwi_Signal_Backup[motor];

	//Backup current Vwi_Calibration_Correction value
	Vwi_Calibration_Correction_NVData[Vwi_Motor_prev] = Vwi_Calibration_Correction;

	//Restore Vwi_Calibration_Correction from backup
	Vwi_Calibration_Correction = Vwi_Calibration_Correction_NVData[motor];
}
#endif // ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))


#if((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))
/**
 *
 * 	Main handler for VWI algorithm
 *
 * @param motor - id of current motor
 */
void VwiMainHandler(uint8 motor)
{
    if (TRUE == VWI_IS_MOTOR_ROTATE(motor))
    {
        // Initialize algorithm based on read thresholds and vwi type
        if( Vwi_Analyze_Phase == INITIALIZATION )
        {
            Reinit();
            Vwi_Analyze_Phase = STABILIZATION;
        }

        // TODO
		#if(VWI_SETTINGFILE_MOTOR_TYPE == ENABLED)
		switch (Vwi_Motor_Type)
		{
			case WM_VARIAB_SPEED_3PH:

				Vwi_Signal = VWI_GET_MOTOR_SIGNAL(motor);
				break;

			case WM_SINGLE_SPEED_ASYNCH_SPH_PSC:
			case WM_SINGLE_SPEED_SYNCH_SPH_PW:
			case WM_SINGLE_SPEED_SYNCH_SPH_AW:
			default:
				// 8 bits resolution
				Vwi_Signal = VWI_GET_MOTOR_SIGNAL(motor);
				if(Vwi_Signal > 255)
				{
					Vwi_Signal = 255;
				}
				break;
		}
		#endif

		//Single speed motor type
		#if(VWI_SINGLE_SPEED_MOTOR_TYPE == ENABLED)
			// 8 bits resolution
		   Vwi_Signal = VWI_GET_MOTOR_SIGNAL(motor);
		   if(Vwi_Signal > 255)
		   {
			   Vwi_Signal = 255;
		   }
		#endif

		//VSM motor type
		#if(VWI_THREE_PHASE_MOTOR_TYPE== ENABLED)
				#if (SPEED_CHANGES_COMP == ENABLED)

		   	    Vwi_Signal_MCU_Raw = VWI_GET_MOTOR_SIGNAL(motor);

		   	    Vwi_Signal_MCU_Mlp = (IQMATH__IQ2IQ((uint32)(Vwi_Signal_MCU_Raw * Vwi_Signal_Multiplier), IQ24, IQ12));

		   	    Vwi_Signal = Vwi_Signal_MCU_Mlp;

		   	    if(Vwi_Signal >= 4096) // To keep 8 bit resolution after removing last 4 bits, saturate current value
				{
					Vwi_Signal = 4096;
				}
				else
				{
					Vwi_Signal = DECREASE_RESOLUTION_BY_4_BITS(Vwi_Signal); // Decrease resolution by 4 bits
				}
				#else
				Vwi_Signal = VWI_GET_MOTOR_SIGNAL(motor);
				#endif
		#endif



        if (SYSTEMTIMERS_STATE_DONE == SystemTimers__GetState(Vwi_Timer_Handle))
        {
            // Stay in STABILIZATION state during sensed fill
            if( (Vwi_Analyze_Phase == STABILIZATION) /*&& (FALSE == Lsi__GetDigitalOutput(DO_FILL_VALVE))*/ )
            {
                    // Go to calibration when fill interval finished and timer stabilization time reached
                    Vwi_Analyze_Phase = CALIBRATION;
            }
            else if(( Vwi_Analyze_Phase == CALIBRATION ) && ( VWI_CALIBRATION == ENABLED ) )
            {
                // Call calibration handler with Vwi_Calibration_Time period
                SystemTimers__SetMs(Vwi_Timer_Handle, Vwi_Calibration_Time);
                Calibrate();
            }
        }

        // Filter signal without compensation
        Vwi_Heavy_Filt_Sig_No_Comp = IIRFilter(&(Vwi_Prev_Heavy_Filt_Sig_No_Comp),
                                                 Vwi_Signal,
                                                 HEAVY_FILTRATION_ALPHA_BITS);
        // Modify Vwi_Signal
        ApplySignalCorrection(motor);



		Vwi_Light_Filt_Signal = IIRFilter(&(Vwi_Prev_Light_Filt_Signal),
											Vwi_Signal,
											LIGHT_FILTRATION_ALPHA_BITS);


        Vwi_Heavy_Filt_Signal = IIRFilter(&(Vwi_Prev_Heavy_Filt_Signal),
                                            Vwi_Signal,
                                            HEAVY_FILTRATION_ALPHA_BITS);


        Vwi_Signal_Backup[motor] = Vwi_Signal;
        Vwi_HeavySignalQueue[VWI_Last_Index] = Vwi_Heavy_Filt_Signal;
        VWI_Last_Index = VWI_Last_Index + 1;
        if (VWI_Last_Index >= VWI_HEAVY_FILTER_QUEUE_SIZE)
        {
            VWI_Last_Index = 0;
        }


        // Estimate variance of Vwi_Signal
        Vwi_Variance = (unsigned short)abs((signed short)(Vwi_Heavy_Filt_Signal - Vwi_Light_Filt_Signal));

        if( Vwi_Variance >= SQRT_OF_CHAR_MAX )
        {
            Vwi_Variance = UCHAR_MAX;
        }
        else
        {
            Vwi_Variance = (unsigned char)(((unsigned char)Vwi_Variance) * ((unsigned char)Vwi_Variance));
        }

        // Filter Variance
        Vwi_Heavy_Filt_Variance = IIRFilter(&(Vwi_Prev_Heavy_Filt_Variance),
                                              Vwi_Variance,
                                              HEAVY_FILTRATION_ALPHA_BITS);


		// Saturate Heavy_Filt_Variance
		if( Vwi_Heavy_Filt_Variance > UCHAR_MAX )
		{
			Vwi_Heavy_Filt_Variance = UCHAR_MAX;
			Vwi_Prev_Heavy_Filt_Variance = UCHAR_MAX << HEAVY_FILTRATION_ALPHA_BITS;
		}


		SetWaterState();

		if (VWI_Is_CalibrationActive == TRUE)
		{
			Vwi_Signal_Correction = Vwi_Calibration_Temp_Correction;
		}
		else
		{
			Vwi_Signal_Correction = 0;
		}

        DebounceWaterState();

        if (TRUE == VWI_IS_MOTOR_ROTATE())
        {
            // Record last status when wash motor is on
            Vwi_Last_Valid_Status = Vwi_Status;
        }
    }
    else
    {
        Vwi_Status = WS_NOT_VALID;
        Vwi_Analyze_Phase = INITIALIZATION;
    }
}
#endif // ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get actual VWI Heavy Filtered Signal without compensation
 *
 * @return  Vwi_Heavy_Filt_Sig_No_Comp
 */
unsigned short Vwi__GetHeavyFiltSignalNoComp(void)
{
#if (VWI_FEATURE == ENABLED) && ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))
    return Vwi_Heavy_Filt_Sig_No_Comp;
#else
    return(0);
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get actual VWI Heavy Filtered Signal
 *
 * @return  Vwi_Heavy_Filt_Signal
 */
unsigned short Vwi__GetHeavyFiltSignal(void)
{
#if (VWI_FEATURE == ENABLED)
    return Vwi_Heavy_Filt_Signal;
#else
    return(0);
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get Previus VWI Heavy Filtered Signal
 *
 * @return  Vwi_Prev_Heavy_Filt_Signal
 */
unsigned short Vwi__GetPrevHeavyFiltSignal(void)
{
#if (VWI_FEATURE == ENABLED) && ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))
    return Vwi_Prev_Heavy_Filt_Signal;
#else
    return(0);
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get average of VWI Heavy Filtered Signal from last 100 ms
 *
 * @return  Vwi_Heavy_Filt_Signal average
 */
unsigned short Vwi__GetHeavyFiltSignalAverage(void)
{
#if (VWI_FEATURE == ENABLED) && ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))
    unsigned short ret_value;
    uint8 i;
    ret_value = 0;
    for (i=0; i < VWI_HEAVY_FILTER_QUEUE_SIZE; i++)
    {
        ret_value = ret_value + Vwi_HeavySignalQueue[i];
    }
    return (ret_value/VWI_HEAVY_FILTER_QUEUE_SIZE);
#else
    return(0);
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get actual VWI Light Filtered Signal
 *
 * @return  Vwi_Light_Filt_Signal
 */
unsigned short Vwi__GetLightFiltSignal(void)
{
#if (VWI_FEATURE == ENABLED) && ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))
    return Vwi_Light_Filt_Signal;
#else
    return(0);
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get actual Heavy Filtered variance
 *
 * @return  Vwi_Heavy_Filt_Variance
 */
unsigned short Vwi__GetHeavyFiltVariance(void)
{
#if (VWI_FEATURE == ENABLED) && ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))
    return Vwi_Heavy_Filt_Variance;
#else
    return(0);
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get actual water status.
 *
 * @return  Vwi_Status, possible value is one #VWI_WATER_SENSE_TYPE
 */
VWI_WATER_SENSE_TYPE Vwi__GetWaterStatus(void)
{
#if (VWI_FEATURE == ENABLED)
	return Vwi_Status;
#else
    return(WS_NOT_VALID);
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get last valid water status.
 *
 * @return  Vwi_Status, possible value is one #VWI_WATER_SENSE_TYPE
 */
VWI_WATER_SENSE_TYPE Vwi__GetLastValidWaterStatus(void)
{
#if (VWI_FEATURE == ENABLED)
	return Vwi_Last_Valid_Status;
#else
    return(WS_NOT_VALID);
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get actual Variance
 *
 * @return  Vwi_Variance
 */
unsigned short Vwi__GetVariance(void)
{
#if (VWI_FEATURE == ENABLED)
    return Vwi_Variance;
#else
    return(0);
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get actual Signal
 *
 * @return  Vwi_Signal
 */
unsigned short Vwi__GetSignal(void)
{
#if (VWI_FEATURE == ENABLED)
    return Vwi_Signal;
#else
    return(0);
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get actual driven motor
 *
 * @return  Vwi_Motor_current
 */
uint8 Vwi__GetCurrentMotor(void)
{
#if (VWI_FEATURE == ENABLED)
    return Vwi_Motor_current;
#else
    return(0);
#endif
}

//---------------------------------------------------------------------------------------------------------------------
#if(VWI_SERVER_FEATURE == ENABLED)
/**
 * @brief	Function used in server configuration. In api221 interface to provide information about variables to client
 *
 * @param channel - number of channel used in API221
 * @return - Frame with pack of variables used in API221 module
 */
sint32 Vwi__ServerRemoteData(uint8 channel)
{
	uint8 idx;
	sint32 retval;

	retval = 0;

	for(idx=0; idx< (sizeof(Remote_Data)/sizeof(VWI_REMOTE_DATA_FRAME)); idx++)
	{
		if(Remote_Data[idx].Channel == channel)
		{
			uint16 *ptr = Remote_Data[idx].Variable_Pointer;

			uint16 value = *ptr;
			value &= ((1<<Remote_Data[idx].Size)-1);
			retval |= (value<<(Remote_Data[idx].Bit_Shift));
		}
	}

	return retval;
}
#endif //(VWI_SERVER_FEATURE == ENABLED)

//=====================================================================================================================
//  --- Private Functions ---------------------------------------------------------------------------------------------
//=====================================================================================================================

#if (VWI_FEATURE == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Set water state based on #Vwi_Heavy_Filt_Signal, #Vwi_Light_Filt_Signal, #Vwi_Heavy_Filt_Variance.
 *
 * This function is called for non MPH motors.
 */
#if((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))
static void SetWaterState(void)
{
    Vwi_Status_Raw = WS_NOT_VALID;
    if( (unsigned char)Vwi_Heavy_Filt_Variance < Vwi_Stable_Signal_Variance_Thr )
    {
        if ( (unsigned char)Vwi_Heavy_Filt_Signal >= Vwi_Water_Signal_Thr )
        {
            Vwi_Status_Raw = WS_WATER;
        }
        else if( (unsigned char)Vwi_Heavy_Filt_Signal >= VWI_Sig_Low_Water_Threshold)//AplFixed__GetConfigByte(APL_VWI_SIGNAL_LOW_WATER_THR) )
        {
            Vwi_Status_Raw = WS_UNSTABLE_WATER;
        }
        else
        {
            Vwi_Status_Raw = WS_NO_WATER;
        }
    }
    else
    {
        if ((unsigned char)Vwi_Heavy_Filt_Signal >= Vwi_Water_Signal_Thr)
        {
            Vwi_Status_Raw = WS_UNSTABLE_WATER;
        }
        else
        {
            Vwi_Status_Raw = WS_UNSTABLE_NO_WATER;
        }

        // While STABILIZATION state or when drain motor is on, in case of unstable conditions reset algorithm
        // It will speed up unstable condition exiting which improve sensed fill and sensed drain

        if ( ( (STABILIZATION == Vwi_Analyze_Phase) /*|| (TRUE == DrainMotor__IsHardwareOn())*/ ) &&
                ((unsigned char)Vwi_Heavy_Filt_Variance > (Vwi_Stable_Signal_Variance_Thr + VWI_INIT_VARIANCE_ADD)) )
        {
            ResetAlgorithm();
        }

    }
}
#endif // ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))

//---------------------------------------------------------------------------------------------------------------------
/**
 * Apply signal correction
 *
 */
#if((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))
static void ApplySignalCorrection(uint8 motor)
{

#if (SPEED_CHANGES_COMP == ENABLED)
    signed short speed;
    unsigned short divider;
    unsigned short signal;
    signed char index;
    const unsigned short* sig_speed_comp;
    unsigned short zero_level;

    //Apply speed compensation only when it's activated
	if(VWI_Is_SpeedCompActive == TRUE)
	{
		speed = 0;
		divider = 0;
		signal = 0;
		index = 0;
		zero_level = 0;

		speed = VWI_GET_MOTOR_SPEED(motor);

		sig_speed_comp = Sig_Speed_Comp_WM;
		zero_level = Speed_Comp_Zero_Level_WM;

		if( Vwi_Signal > zero_level )
		{
			if(speed < Min_Comp_Speed)
			{
				speed = Min_Comp_Speed;
			}
			else if(speed > Max_Comp_Speed)
			{
				speed = Max_Comp_Speed;
			}

			speed -= Min_Comp_Speed;            // Convert speed from absolute to table begin relative
			index = speed / Speed_Comp_Step;    // Convert relative speed to table index

			// If speed is between table values, calculate linear interpolation to get precise divider for actual speed
			// + Speed_Comp_Step / 2 is for rounding
			// For highest of index, sig_speed_comp[index + 1] will have value outside of table but speed % Speed_Comp_Step
			// will be equal to 0 and calculation will be correct.
			divider = sig_speed_comp[index] +
					  ((((sig_speed_comp[index + 1] - sig_speed_comp[index]) * (speed % Speed_Comp_Step)) + Speed_Comp_Step / 2) / Speed_Comp_Step);

			if(divider == 0)
			{
				divider++;
			}

			// Calculate signal after compensation
			// + divider / 2 is for rounding
			signal = (((Vwi_Signal - zero_level) * SPEED_COMP_MULTIPLIER) + divider / 2) / divider;

			// Saturate signal
			if(signal > CHAR_MAX) // @suppress("Symbol is not resolved")
			{
				Vwi_Signal = CHAR_MAX; // @suppress("Symbol is not resolved")
			}
			else
			{
				Vwi_Signal = signal;
			}
		}
		else
		{
			// Set always 0 for negative compensation
			Vwi_Signal = 0;
		}

	}
    #endif //SPEED_CHANGES_COMP == ENABLED


    // Apply signal correction only when correction is valid and Calibration is turned on
    if(((signed short)Vwi_Signal > (signed short)Vwi_Signal_Correction) &&
       (VWI_Is_CalibrationActive == TRUE))
    {
        Vwi_Signal = (unsigned short)(((signed short)Vwi_Signal) - (signed short)Vwi_Signal_Correction);
    }
}
#endif // ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))

//---------------------------------------------------------------------------------------------------------------------
#if((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))
/**
 * Debounce water state and assign #Vwi_Status.
 */
static void DebounceWaterState(void)
{
    if ( Vwi_Prev_Status_Raw == Vwi_Status_Raw )
    {
        if (SYSTEMTIMERS_STATE_DONE == SystemTimers__GetState(Vwi_Debounce_Timer_Handle))
        {
            SystemTimers__SetMs(Vwi_Debounce_Timer_Handle, VWI_DEBOUNCE_TIME_MS);
            Vwi_Status = Vwi_Status_Raw;

            if ( Vwi_Analyze_Phase == STABILIZATION )
            {
                if ( Vwi_Status == WS_UNSTABLE_NO_WATER || Vwi_Status == WS_UNSTABLE_WATER )
                {
                    Vwi_Status = WS_NOT_VALID;
                }
            }
            else if( Vwi_Analyze_Phase == CALIBRATION )
            {
                if( Vwi_Status != WS_WATER )
                {
                    // Reset calibration if state different than water is detected
                    ResetCalibration();
                }
            }
        }
    }
    else
    {
        SystemTimers__SetMs(Vwi_Debounce_Timer_Handle, VWI_DEBOUNCE_TIME_MS);
        Vwi_Prev_Status_Raw = Vwi_Status_Raw;
    }
}
#endif // ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))

//---------------------------------------------------------------------------------------------------------------------
#if((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))
/**
 * Reset whole algorithm and initialize previous filter values to current signal readings.
 */
static void ResetAlgorithm(void)
{
    unsigned short temp_signal;

    SystemTimers__SetMs(Vwi_Debounce_Timer_Handle, VWI_DEBOUNCE_TIME_MS);
    Vwi_Status = WS_NOT_VALID;
    Vwi_Status_Raw = WS_NOT_VALID;
    Vwi_Prev_Status_Raw = WS_NOT_VALID;
    Vwi_Calibration_First_Read = 0;

    temp_signal = Vwi_Signal << LIGHT_FILTRATION_ALPHA_BITS;

    Vwi_Prev_Light_Filt_Signal = temp_signal;
    Vwi_Prev_Heavy_Filt_Signal = Vwi_Signal << HEAVY_FILTRATION_ALPHA_BITS;
    Vwi_Prev_Heavy_Filt_Sig_No_Comp = Vwi_Prev_Heavy_Filt_Signal;

    Vwi_Prev_Heavy_Filt_Variance = (Vwi_Stable_Signal_Variance_Thr + VWI_INIT_VARIANCE_ADD) << HEAVY_FILTRATION_ALPHA_BITS;
}
#endif // ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))

//---------------------------------------------------------------------------------------------------------------------
#if((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))
/**
 * Reset calibration by clearing #Vwi_Calibration_First_Read.
 */
static void ResetCalibration(void)
{
    SystemTimers__SetMs(Vwi_Timer_Handle, Vwi_Calibration_Time);
    Vwi_Calibration_First_Read = 0;
}
#endif // ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))

//---------------------------------------------------------------------------------------------------------------------
#if((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))
/**
 * Calculate signal correction caused by constant environmental factors like temperature and measure circuit variations.
 *
 * Signal correction is evaluated based on expected signal value for water defined by APL.
 */
static void Calibrate(void)
{
    unsigned char  signal_cal_target_for_water;
    signed char    calibration_correction_max;

    signal_cal_target_for_water = VWI_Signal_Cal_Target;

    // First step of calibration
    if ( Vwi_Calibration_First_Read == 0 )
    {
        Vwi_Calibration_First_Read = Vwi_Heavy_Filt_Signal;
    }
    // Second step of calibration
    else
    {
        // Check if signal is stable
        if ( Vwi_Heavy_Filt_Signal == Vwi_Calibration_First_Read )
        {

            {
                Vwi_Calibration_Temp_Correction = (signed char)( (signed short)Vwi_Heavy_Filt_Signal +
                    (signed short)Vwi_Calibration_Temp_Correction - (signed short)signal_cal_target_for_water );
                calibration_correction_max = (signed char)(signal_cal_target_for_water - Vwi_Water_Signal_Thr);
            }

            // Safety correction check
            if ( calibration_correction_max > 0)
            {
                // Saturate calibration correction. When it too high or low set limit value.
                if ( Vwi_Calibration_Temp_Correction > calibration_correction_max)
                {
                    Vwi_Calibration_Temp_Correction = calibration_correction_max;
                }
                else if ( Vwi_Calibration_Temp_Correction < -calibration_correction_max )
                {
                    Vwi_Calibration_Temp_Correction = -calibration_correction_max;
                }
            }
            else
            {
                // Calibration is disabled if APL_VWI_SIGNAL_CALIBRATION_TARGET or APL_VWI_PHASE_SHIFT_CALIBRATION_TARGET
                // are set incorrectly.
                Vwi_Calibration_Temp_Correction = 0;
            }

            Vwi_Calibration_Correction = Vwi_Calibration_Temp_Correction;
        }
        // Go to first step
        Vwi_Calibration_First_Read = 0;
    }
}
#endif // ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))

//---------------------------------------------------------------------------------------------------------------------
#if((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))
/**
 * @brief Load data from SettingFile
 */
static void ReadProcessParameters(uint8 motor)
{
    SETTINGFILE_LOADER_TYPE loader;
    uint8 Vwi_Setting_Displ;

	if (motor < MOTOR_NUM)
	{
		Vwi_Setting_Displ = Motor_Settings_Displ[motor];
	}
	else
	{
		Vwi_Setting_Displ = VWI_SETTING_FILE_DISPL_DEFAULT;
	}

	VWI_Is_Present = FALSE;

	if (Mode__GetMode() == MODE_NORMAL)
	{
	    // Get the VWI parameters from the setting file.
	    if (SettingFile__BasicLoader(VWI_SETTING_FILE_PTR,
	    						     Vwi_Setting_Displ,
	                                 &loader) == FAIL)
	    {
	        LOG_ADD_EXCEPTION(ERROR_LOADING_PARAMETERS, 0);
	    }
	    else
	    {
	        // Read the setting file data into local variables for convenience.
	        ReadVWIParameters(&loader);
	    }
	}
}
#endif // ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))

//---------------------------------------------------------------------------------------------------------------------
#if((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))
/**
 * @brief	Load module settings from SettingFile
 *
 * @param loader
 */
static void ReadVWIParameters(SETTINGFILE_LOADER_TYPE* loader)
{
#if(SPEED_CHANGES_COMP == ENABLED)
	uint8 array_length;
#endif //(SPEED_CHANGES_COMP == ENABLED)
    VWI_PARAMETER_TYPE* parameter;

    parameter = (VWI_PARAMETER_TYPE*)loader->Data; //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]

    Vwi_Calibration_Time = parameter->VWI_Calibration_Time*1000;
    Vwi_Water_Signal_Thr = parameter->VWI_Sig_Water_Threshold;
    Vwi_Stable_Signal_Variance_Thr = parameter->VWI_Sig_Variance_Threshold;
    VWI_Signal_Cal_Target = parameter->VWI_Signal_Calibration_Target;
    VWI_Sig_Low_Water_Threshold = parameter->VWI_Signal_Low_Water_Threshold;
#if (VWI_THREE_PHASE_MOTOR_TYPE == ENABLED)
    Vwi_Signal_Max = parameter->Vwi_Signal_Max;
#endif
    //VWI control register
    VWI_Is_Present = (BOOL_TYPE)parameter->VWI_Control_Reg.VWI_Present;

    //Read flag with inversion
    VWI_Is_CalibrationActive = (BOOL_TYPE)(!parameter->VWI_Control_Reg.VWI_Calibration);
	VWI_Is_SpeedCompActive   = (BOOL_TYPE)(!parameter->VWI_Control_Reg.VWI_Speed_Comp);

#if(ACU_MOTOR_CONTROL == ENABLED)
    Vwi_Motor_Type = parameter->VWI_Wash_Motor_Type;
    VWI_SET_WASH_MOTOR_TYPE(Vwi_Motor_Type);
#endif

#if(SPEED_CHANGES_COMP == ENABLED)
    if (VWI_Is_SpeedCompActive == TRUE)
    {
		Speed_Comp_Zero_Level_WM = parameter->Speed_Comp_Zero_Level;
		Min_Comp_Speed = parameter->Min_Comp_Speed;
		Max_Comp_Speed = parameter->Max_Comp_Speed;
		Speed_Comp_Step = parameter->Speed_Comp_Step;

		array_length = ((Max_Comp_Speed - Min_Comp_Speed) / Speed_Comp_Step) + 1;
		memcpy(&Sig_Speed_Comp_WM, parameter->Sig_Speed_Comp, (sizeof(uint16) * array_length));
    }
#endif //(SPEED_CHANGES_COMP == ENABLED)
}
#endif // ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))

//---------------------------------------------------------------------------------------------------------------------
#if((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))
/**
 * @brief Reinitialization of VWI functionality. All global variables must be set to 0 before this function call when VWI_INIT_TO_ZERO not defined.
 */
static void Reinit(void)
{
   SystemTimers__SetMs(Vwi_Timer_Handle, VWI_STABILIZATION_TIME_MS);
   // Set initial calibration value as part of previously calculated
   Vwi_Calibration_Temp_Correction = Vwi_Calibration_Correction / VWI_INIT_CAL_CORRECTION_DIV;
   ResetAlgorithm();
}
#endif // ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED))

#endif //(VWI_FEATURE == ENABLED)

//---------------------------------------------------------------------------------------------------------------------
#if(VWI_CLIENT_FEATURE == ENABLED)
/**
 * @brief	Function which provide information if API221 channel is active or not
 *
 * @param 	channel - variable channel
 * @return 	TRUE  - channel is active
 * 			FALSE - channel is inactive
 */
BOOL_TYPE IsChannelActive(uint8 channel)
{
	uint8 idx;
	BOOL_TYPE retval;

	retval = FALSE;

	for(idx=0; idx< (sizeof(Remote_Data_Channels)/sizeof(VWI_REMOTE_CHANNEL)); idx++)
	{
		if(Remote_Data_Channels[idx].Channel == channel)
		{
			if(Remote_Data_Channels[idx].IsActive == TRUE)
			{
				retval = TRUE;
			}
		}
	}

	return retval;
}
#endif //(VWI_FEATURE == ENABLED)

//---------------------------------------------------------------------------------------------------------------------
#if(VWI_CLIENT_FEATURE == ENABLED)
/**
 * @brief	Function used in client configuration. To retrieve information about variables from server
 *
 */
static void GetRemoteData(uint8 motor)
{
	uint8 idx;
	sint32 temp_data;

	for(idx = 0; idx < (sizeof(Remote_Data)/sizeof(VWI_REMOTE_DATA_FRAME)); idx++)
	{
		if(IsChannelActive(Remote_Data[idx].Channel) == TRUE)
		{
			if(API221MotionCtrl__GetData(motor, Remote_Data[idx].Channel, &temp_data))
			{
				StoreData(Remote_Data[idx], temp_data);
			}
		}
		else
		{
			StoreData(Remote_Data[idx], 0);
		}
	}
}
#endif //(VWI_CLIENT_FEATURE == ENABLED)

//---------------------------------------------------------------------------------------------------------------------
#if(VWI_CLIENT_FEATURE == ENABLED)
/**
 * @brief	Function used in client configuration. To update variables in RAM
 *
 * @param frame - Remote data frame information
 * @param data - data received from API 221
 */
static void StoreData(VWI_REMOTE_DATA_FRAME frame, sint32 data)
{
	if (frame.Size == 4)
	{
		uint8 *ptr = frame.Variable_Pointer;
		*ptr = (uint8) (((data) >> frame.Bit_Shift) & ((1<<frame.Size)-1));
	}
	else if (frame.Size == 8)
	{
		uint8 *ptr = frame.Variable_Pointer;
		*ptr = (uint8) (((data) >> frame.Bit_Shift) & ((1<<frame.Size)-1));
	}
	else if (frame.Size == 16)
	{
		uint16 *ptr = frame.Variable_Pointer;
		*ptr = (uint16) (((data) >> frame.Bit_Shift) & ((1<<frame.Size)-1));
	}
}
#endif //(VWI_CLIENT_FEATURE == ENABLED)

//---------------------------------------------------------------------------------------------------------------------
#if(VWI_CLIENT_FEATURE == ENABLED)
/**
 * @brief Function used to update subscribed channels
 *
 */
static void UpdateRemoteChannels(uint8 motor)
{
	uint8 idx;
	uint32 Channel_Bit_Mask = 0;
	uint32 Channel_Bit_Mask_Inactive = 0;
	uint16 Period = SHRT_MAX;


	for(idx=0; idx< (sizeof(Remote_Data_Channels)/sizeof(VWI_REMOTE_CHANNEL)); idx++)
	{
		if(Remote_Data_Channels[idx].IsActive)
		{
			Channel_Bit_Mask |= (1<<Remote_Data_Channels[idx].Channel);

			if(Remote_Data_Channels[idx].Period < Period)
			{
				Period = Remote_Data_Channels[idx].Period;
			}
		}
		else
		{
			Channel_Bit_Mask_Inactive |= (1<<Remote_Data_Channels[idx].Channel);
		}
	}

    API221MotionCtrl__SetDataPublicationPeriod(motor, Period);
	API221MotionCtrl__RemovePeriodicDataChannels(motor, Channel_Bit_Mask_Inactive);
	API221MotionCtrl__AddPeriodicDataChannels(motor, Channel_Bit_Mask);
}
#endif //(VWI_CLIENT_FEATURE == ENABLED)

//---------------------------------------------------------------------------------------------------------------------
#if(VWI_CLIENT_FEATURE == ENABLED)
/**
 * @brief Function used in client configuration. To subscribe information about variables from server
 *
 * @param channel - channel number
 * @param enable
 */
void Vwi__RemoteChannelEnable(uint8 motor, uint8 channel, BOOL_TYPE enable)
{
	uint8 idx;

	if(enable == TRUE)
	{
		Vwi_Motor_Use = motor;
	}

	for(idx=0; idx< (sizeof(Remote_Data_Channels)/sizeof(VWI_REMOTE_CHANNEL)); idx++)
	{
		if(Remote_Data_Channels[idx].Channel == channel)
		{
			Remote_Data_Channels[idx].IsActive = enable;
		}
	}

	UpdateRemoteChannels(Vwi_Motor_Use);
}
#endif //(VWI_CLIENT_FEATURE == ENABLED)

//---------------------------------------------------------------------------------------------------------------------
#if(VWI_CLIENT_FEATURE == ENABLED)
/**
 * @brief Function used in client configuration. To subscribe information about variables from server
 *
 * @param channel - channel number
 * @param enable
 */
void Vwi__RemoteChannelPeriod(uint8 channel, uint16 period)
{
	uint8 idx;

	for(idx=0; idx< (sizeof(Remote_Data_Channels)/sizeof(VWI_REMOTE_CHANNEL)); idx++)
	{
		if((Remote_Data_Channels[idx].Channel == channel) || (channel == 0xFF))
		{
			Remote_Data_Channels[idx].Period = period;
		}
	}

	UpdateRemoteChannels(Vwi_Motor_Use);
}
#endif //(VWI_CLIENT_FEATURE == ENABLED)

//---------------------------------------------------------------------------------------------------------------------
#if ((VWI_INTERNAL_COMMUNICATION == ENABLED) && (VWI_SINGLE_SPEED_MOTOR_TYPE == ENABLED))
/**
 * @brief Function is used to check if motor is rotate or not. Used in FIN board.
 */
BOOL_TYPE IsMotorRotate(void)
{
	uint8 return_value;

	return_value = FALSE;

	switch(Vwi_Motor_Type)
	{
		case WM_SINGLE_SPEED_ASYNCH_SPH_PSC:
			 return_value = *((uint8*)Hbl__GetProcessedLoad(Hbl__GetLoadIndexByLoadID(HBL_LOAD_WASH_PUMP)));
			break;

		case WM_SINGLE_SPEED_SYNCH_SPH_PW:
		case WM_SINGLE_SPEED_SYNCH_SPH_AW:
			if(FALSE == MCI_GETDIGITAL(MOTOR0, MCI_DI_MOTOR_STOP))
			{
				return_value = TRUE;
			}
			break;
	}

	return (BOOL_TYPE)return_value;
}
#endif //(VWI_INTERNAL_COMMUNICATION == ENABLED)



#if(VWI_INTERNAL_COMMUNICATION == ENABLED)
/**
 * @brief Function provide information about current VWI calibration phase
 *
 * @return Vwi_Analyze_Phase
 */
VWI_ANALYZE_PHASE_TYPE  Vwi__GetAnalyzePhase(void)
{
#if (VWI_FEATURE == ENABLED)
    return Vwi_Analyze_Phase;
#else
    return (INITIALIZATION);
#endif
}
#endif

#if ((VWI_THREE_PHASE_MOTOR_TYPE== ENABLED) && ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED)))

void Vwi_SetInputSignalMultiplier(uint16 multiplier)
{
	Vwi_Signal_Multiplier = multiplier;
}


unsigned short Vwi__SignalMCURaw(void)
{
	return Vwi_Signal_MCU_Raw;
}


unsigned short Vwi__SignalMCUMlp(void)
{
	return Vwi_Signal_MCU_Mlp;
}
#endif // ((VWI_THREE_PHASE_MOTOR_TYPE== ENABLED) && ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED)))

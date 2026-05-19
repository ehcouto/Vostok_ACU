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
#include "MclInputProc.h" 
#include "Atod.h"
#include "Mcl_prv.h"
#include "Gpio.h"

#include "PeakDetector.h"
#include "GlitchFilter.h"

#ifndef MCLINPUTPROC_OFFSET
	#define MCLINPUTPROC_OFFSET        45
#endif

#ifndef MCLINPUTPROC_MIN_GAIN
	#define MCLINPUTPROC_MIN_GAIN      30310
#endif

#ifndef MCLINPUTPROC_MAX_GAIN
	#define MCLINPUTPROC_MAX_GAIN      35225
#endif

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
static sint32 MciInputProc_Current_Cnt;
static uint8 Short_Circuit_Deb;                 //!< Short circuit debounce
static sint16 Prev_Curr;                        //!< Previous raw current value
static GLITCHFILTER_MEASURE_TYPE Curr_Channel;  //!< Current channel

#if(MCISLFAC_ADC_ERROR_SIM == ENABLED)
static uint8 Adc_Error_Sim;
#endif
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module MclInputProc and its variables
 *
 */
void MclInputProc__Initialize(MCL_INPUT_PROC_IO_TYPE* io,  MCL_PARAMS_TYPE* params)
{
	MciInputProc_Current_Cnt = 0;
	Short_Circuit_Deb = 0;

    PeakDetector__Initialize();
	PeakDetector__Setup(PEAK_MCI_VOLT_CHANNEL, (uint16)(params->P2P_Detection_Period * params->Mcisflac_Fast_Handler_Frequency));
	PeakDetector__Setup(PEAK_MCI_CURR_CHANNEL, (uint16)(params->P2P_Detection_Period * params->Mcisflac_Fast_Handler_Frequency));

	GlitchFilter__Initialize(&Curr_Channel,255,2);

#if(MCISLFAC_ADC_ERROR_SIM == ENABLED)
	Adc_Error_Sim = params->Current_Voltage_Sensor_Mode;
#endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief
 *
 */
void MclInputProc__RunningHandler(MCL_INPUT_PROC_IO_TYPE* io,  MCL_PARAMS_TYPE* params)
{
    uint16 raw_volt;
    sint16 sig_raw_volt;
    uint16 raw_curr;
    sint16 sig_raw_curr;
    uint16 abs_raw_curr;

    raw_volt = Atod__Convert(MCI_VOLTAGE_CHANNEL,ATOD_RESOLUTION_12BITS);//Atod__Read(MCI_VOLTAGE_CHANNEL);
#if(MCISLFAC_ADC_ERROR_SIM == ENABLED)
    sint32 temp;

    if((Adc_Error_Sim == 1)  ||
       (Adc_Error_Sim == 10) ||
       (Adc_Error_Sim == 11))
    {
    	temp = (sint32)(((sint32)raw_volt * MCLINPUTPROC_MIN_GAIN)>>15) + MCLINPUTPROC_OFFSET;
    	raw_volt = (uint16)MATHCALC__SATURATE_DIRECT(0, temp, 4095);
    }
    if((Adc_Error_Sim == 2)  ||
       (Adc_Error_Sim == 20) ||
	   (Adc_Error_Sim == 21))
    {
    	temp = (sint32)(((sint32)raw_volt * MCLINPUTPROC_MAX_GAIN)>>15) - MCLINPUTPROC_OFFSET;
    	raw_volt = (uint16)MATHCALC__SATURATE_DIRECT(0, temp, 4095);
    }
#endif //#if(MCISLFAC_ADC_ERROR_SIM == ENABLED)
    sig_raw_volt = (sint16)raw_volt -  (sint16)*io->Offset_Volt;

    raw_curr = Atod__Convert(MCI_CURRENT_CHANNEL,ATOD_RESOLUTION_12BITS); //Atod__Read(MCI_CURRENT_CHANNEL);
#if(MCISLFAC_ADC_ERROR_SIM == ENABLED)
    if((Adc_Error_Sim == 3)  ||
       (Adc_Error_Sim == 10) ||
	   (Adc_Error_Sim == 20))
    {
    	temp = (sint32)(((sint32)raw_curr * MCLINPUTPROC_MIN_GAIN)>>15) + MCLINPUTPROC_OFFSET;
    	raw_curr = (uint16)MATHCALC__SATURATE_DIRECT(0, temp, 4095);
    }
    if((Adc_Error_Sim == 4)  ||
       (Adc_Error_Sim == 11) ||
	   (Adc_Error_Sim == 21))
    {
    	temp = (sint32)(((sint32)raw_curr * MCLINPUTPROC_MAX_GAIN)>>15) - MCLINPUTPROC_OFFSET;
    	raw_curr = (uint16)MATHCALC__SATURATE_DIRECT(0, temp, 4095);
    }
#endif //#if(MCISLFAC_ADC_ERROR_SIM == ENABLED)
    raw_curr = GlitchFilter__Process(&Curr_Channel, raw_curr);

    sig_raw_curr = (sint16)raw_curr - (sint16)(*io->Offset_Curr);
    abs_raw_curr = MATHCALC__ABS(sig_raw_curr);
    *io->dif_raw_curr = (sint16)raw_curr - Prev_Curr;
    PeakDetector__SetValue(PEAK_MCI_CURR_CHANNEL, raw_curr);
    PeakDetector__SetValue(PEAK_MCI_VOLT_CHANNEL, raw_volt);
    *io->zero_current_prev = *io->Zero_Current_Detected;

    if((abs_raw_curr > 100) && (MATHCALC__ABS(sig_raw_volt) < 30))
    {
        Short_Circuit_Deb++;
        if(Short_Circuit_Deb > 3)
        {
        	*io->Mcl_Fault = UNDER_VOLTAGE_SHORT_CIRCUIT;
            //FailureHandlerRecover(UNDER_VOLTAGE_SHORT_CIRCUIT);
        }
    }
    else
    {
        Short_Circuit_Deb = 0;
    }

    Prev_Curr = raw_curr;

    *io->raw_curr = raw_curr;
    *io->raw_volt = raw_volt;
    *io->sig_raw_volt = sig_raw_volt;
    *io->sig_raw_curr = sig_raw_curr;
    *io->abs_raw_curr = abs_raw_curr;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief
 *
 */
void MclInputProc__ResetState(MCL_INPUT_PROC_IO_TYPE* io,  MCL_PARAMS_TYPE* params)
{

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief
 *
 */
void MclInputProc__CurrentMonitor(MCL_INPUT_PROC_IO_TYPE* io,  MCL_PARAMS_TYPE* params)
{
    uint16 p2p_current_min;
    uint16 p2p_current_max;
    uint16 offset_curr;

    if((MCISLFAC_OPEN_LOOP == io->ctrl_specific->Control_Mode) ||  (MCISLFAC_CLOSE_LOOP == io->ctrl_specific->Control_Mode))
    {
    	offset_curr = (uint16)*io->Offset_Curr;
        p2p_current_min = PeakDetector__GetMin(PEAK_MCI_CURR_CHANNEL);
        if(p2p_current_min < offset_curr)
        {
            p2p_current_min = offset_curr - p2p_current_min;
        }
        else
        {
            p2p_current_min = 0;
        }

        p2p_current_max = PeakDetector__GetMax(PEAK_MCI_CURR_CHANNEL);
        if(p2p_current_max > offset_curr)
        {
            p2p_current_max = p2p_current_max - offset_curr;
        }
        else
        {
            p2p_current_max = 0;
        }

        if((p2p_current_max < params->Under_Curr_Thr) || (p2p_current_max > params->Over_Curr_Thr) ||
           (p2p_current_min < params->Under_Curr_Thr) || (p2p_current_min > params->Over_Curr_Thr)) //UNDER_CURRENT_THR //OVER_CURRENT_THR
        {
        	MciInputProc_Current_Cnt ++;

            if(MciInputProc_Current_Cnt > (uint16)(params->Current_Failure_Debounce_Time/25)) //CURRENT_FAILURE_DEBOUNCE_TIME
            {
                if((p2p_current_max < params->Under_Curr_Thr) || (p2p_current_min < params->Under_Curr_Thr)) //UNDER_CURRENT_THR
                {
                    *io->Mcl_Fault = (uint32)UNDER_CURRENT;
                }
                else
                {
                	*io->Mcl_Fault = (uint32)OVER_CURRENT;
                }

                //Mcl__FailureHandler();
            }
        }
        else
        {
        	MciInputProc_Current_Cnt = 0;
        }
    }
    else
    {
    	MciInputProc_Current_Cnt = 0;
    }
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================



/**
 *  @file
 *  @brief      Detect max, min and peak to peak value taken from defined number of samples.
 *
 *  @details    Extremes searching is performed within each handled in function #PeakDetector__SetValue period.
 *              New calculation (min, max and peak to peak values) are available after defined (in function #PeakDetector__Setup)
 *              number of period samples.
 *              Each new calculation will be available thought function
 *              #PeakDetector__GetMin, #PeakDetector__GetMax, #PeakDetector__GetPeak2Peak.
 *
 *              e.g. If each new value is set every 1ms,
 *              and period value is defined to 20 samples than every new calculation will be available
 *              every 20ms.
 *
 *  @Module_Owner{Lukasz Ochromowicz}
 *  $Revision: 1.9 $
 *
 *  @copyright  Copyright 2011-$Date: 2016/06/24 11:00:33BRT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "C_Extensions.h"
#include "PeakDetector.h"

#ifndef PEAKDETECTOR_SR
#define PEAKDETECTOR_SR DISABLED
#endif //PEAKDETECTOR_SR

#if (PEAKDETECTOR_SR == ENABLED)
#include "NSRPeakDetector_prm.h"
#endif //PEAKDETECTOR_SR

// -- ANSI/ISO --
#include <string.h>

// -- Project --
#if (PEAKDETECTOR_SR == ENABLED)
#include "SRData.h"
#endif //PEAKDETECTOR_SR

//  --- Public Variables ----------------------------------------------------------------------------------------------

//  --- Private Properties --------------------------------------------------------------------------------------------
//! Peak detector type definition - Contain all required variables for one channel
typedef struct
{
	uint16   Min_Peak_Out;    //!< Min peaks value (taken from Period_Counter_Reload number of samples)
	uint16   Max_Peak_Out;    //!< Max peaks value (taken from Period_Counter_Reload number of samples)
	uint16   Min_Peak_Rough;  //!< Min rough sample values use for calculation
	uint16   Max_Peak_Rough;  //!< Max rough sample value use for calculation
	uint16   Period_Counter_Reload;//!< Number of samples needed for calculate min/max value
	uint16   Period_Counter_Rough; //!< Current number of used samples
}PEAK_DETECTOR_TYPE;

static PEAK_DETECTOR_TYPE Peak_Detector[PEAK_CHANNEL_NUM]; //!< Contain all required variables for all supported channel
#if (PEAKDETECTOR_SR == ENABLED)
static PEAK_DETECTOR_TYPE N_Peak_Detector[PEAK_CHANNEL_NUM]; //!< Contain all required variables for all supported channel
#endif //PEAKDETECTOR_SR
//  --- Private Function Prototypes -----------------------------------------------------------------------------------

//=====================================================================================================================
//  --- Public Functions ----------------------------------------------------------------------------------------------
//=====================================================================================================================//-------------------------------------- Include Files ----------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
/**
 * It Initializes the module PeakDetector and its variables.
 */
void PeakDetector__Initialize(void)
{
    uint8 channel;
    PEAK_DETECTOR_TYPE *peak_detector;

#if (PEAKDETECTOR_SR == ENABLED)
    PEAK_DETECTOR_TYPE *n_peak_detector;

    PEAKDETECTION_INITIALIZE_FLOW();
    
    for(channel = 0; channel < PEAK_CHANNEL_NUM; channel++)
    {
        peak_detector = &Peak_Detector[channel];
        n_peak_detector = &N_Peak_Detector[channel];

        SRData__UpdateShort(&(peak_detector->Min_Peak_Out), &(n_peak_detector->Min_Peak_Out), 0);

        SRData__UpdateShort(&(peak_detector->Max_Peak_Out), &(n_peak_detector->Max_Peak_Out), 0);

        SRData__UpdateShort(&(peak_detector->Min_Peak_Rough), &(n_peak_detector->Min_Peak_Rough), USHRT_MAX);

        SRData__UpdateShort(&(peak_detector->Max_Peak_Rough), &(n_peak_detector->Max_Peak_Rough), 0);

        SRData__UpdateShort(&(peak_detector->Period_Counter_Reload), &(n_peak_detector->Period_Counter_Reload), 0);

        SRData__UpdateShort(&(peak_detector->Period_Counter_Rough), &(n_peak_detector->Period_Counter_Rough), 0);
    }

#else //PEAKDETECTOR_SR
    memset(&Peak_Detector, 0, sizeof(Peak_Detector));
    
    for(channel = 0; channel < PEAK_CHANNEL_NUM; channel++)
    {
        peak_detector = &Peak_Detector[channel];
    
        // Reset Min and Max separately
        peak_detector->Min_Peak_Rough = 65535;
        peak_detector->Max_Peak_Rough = 0;
    }
#endif //PEAKDETECTOR_SR
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Setup period value for extremes searching, LSB is dependent on handler execution time (usually 1ms).
 *
 * @param    channel - Channel number taken from #PEAK_DETECTOR_CHANNEL_TYPE list.
 * @param    period  - Number of samples needed for calculate min/max value. For 0 and 1 period is the same and equal to 1 sample.
 */
void PeakDetector__Setup(PEAK_DETECTOR_CHANNEL_TYPE channel, uint16 period)
{
#if (PEAKDETECTOR_SR == ENABLED)

    if( channel < PEAK_CHANNEL_NUM )
    {
        if( period < 1 )
        {
            period = 1;
        }
        
        SRData__UpdateShort(&(Peak_Detector[channel].Period_Counter_Reload), &(N_Peak_Detector[channel].Period_Counter_Reload), period - 1);
    }
    
#else //PEAKDETECTOR_SR
    if( channel < PEAK_CHANNEL_NUM )
    {
        if( period < 1 )
        {
            period = 1;
        }
        
        Peak_Detector[channel].Period_Counter_Reload = period - 1;
        Peak_Detector[channel].Period_Counter_Rough = Peak_Detector[channel].Period_Counter_Reload;
    }

#endif //PEAKDETECTOR_SR
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Reset channel
 *
 * @param    channel - Channel number taken from #PEAK_DETECTOR_CHANNEL_TYPE list.
 */
void PeakDetector__Reset(PEAK_DETECTOR_CHANNEL_TYPE channel)
{
    PEAK_DETECTOR_TYPE *peak_detector;

#if (PEAKDETECTOR_SR == ENABLED)
    PEAK_DETECTOR_TYPE *n_peak_detector;    

    peak_detector = &Peak_Detector[channel];
    n_peak_detector = &N_Peak_Detector[channel];

    SRData__UpdateShort(&(peak_detector->Min_Peak_Out), &(n_peak_detector->Min_Peak_Out), 0);

    SRData__UpdateShort(&(peak_detector->Max_Peak_Out), &(n_peak_detector->Max_Peak_Out), 0);

    SRData__UpdateShort(&(peak_detector->Min_Peak_Rough), &(n_peak_detector->Min_Peak_Rough), 65535);

    SRData__UpdateShort(&(peak_detector->Max_Peak_Rough), &(n_peak_detector->Max_Peak_Rough), 0);

    SRData__UpdateShort(&(peak_detector->Period_Counter_Rough), &(n_peak_detector->Period_Counter_Rough), 0);

#else //PEAKDETECTOR_SR
    if( channel < PEAK_CHANNEL_NUM )
    {
        peak_detector = &Peak_Detector[channel];
        peak_detector->Min_Peak_Out = 0;
        peak_detector->Max_Peak_Out = 0;
        peak_detector->Min_Peak_Rough = 65535;
        peak_detector->Max_Peak_Rough = 0;
        peak_detector->Period_Counter_Rough = peak_detector->Period_Counter_Reload;
    }
#endif //PEAKDETECTOR_SR
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Peak detector set current value and handle it.
 *
 * Set new rough value used for peak detector calculation, and handle it.
 * New calculation are available after defined (in function #PeakDetector__Setup)
 * number of period samples are set.
 * Each new calculation will be available thought function
 * #PeakDetector__GetMin, #PeakDetector__GetMax, #PeakDetector__GetPeak2Peak.
 *
 * e.g. If each new value is set every 1ms,
 * and period value is defined to 20 samples than every new calculation will be available
 * every 20ms.
 *
 * @param    channel        - Channel number taken from #PEAK_DETECTOR_CHANNEL_TYPE list.
 * @param    current_value  - New rough value used for peak detector calculation.
 */
void PeakDetector__SetValue(PEAK_DETECTOR_CHANNEL_TYPE channel, uint16 current_value)
{
#if (PEAKDETECTOR_SR == ENABLED)

    PEAK_DETECTOR_TYPE *peak_detector;
    PEAK_DETECTOR_TYPE *n_peak_detector;

    if( channel < PEAK_CHANNEL_NUM )
    {
        peak_detector = &Peak_Detector[channel];
        n_peak_detector = &N_Peak_Detector[channel];
        
        if( (SRDATA_OK == SRData__CheckShort(&(peak_detector->Max_Peak_Rough), &(n_peak_detector->Max_Peak_Rough))) &&
            (SRDATA_OK == SRData__CheckShort(&(peak_detector->Min_Peak_Rough), &(n_peak_detector->Min_Peak_Rough))) &&
            (SRDATA_OK == SRData__CheckShort(&(peak_detector->Period_Counter_Rough), &(n_peak_detector->Period_Counter_Rough))) &&
            (SRDATA_OK == SRData__CheckShort(&(peak_detector->Period_Counter_Reload), &(n_peak_detector->Period_Counter_Reload))) )
        {
            // Search for Max and Min
            if( current_value > peak_detector->Max_Peak_Rough )
            {
                SRData__UpdateShort(&(peak_detector->Max_Peak_Rough), &(n_peak_detector->Max_Peak_Rough), current_value);
            }
            if( current_value <  peak_detector->Min_Peak_Rough )
            {
                SRData__UpdateShort(&(peak_detector->Min_Peak_Rough), &(n_peak_detector->Min_Peak_Rough), current_value);
            }

            if( peak_detector->Period_Counter_Rough == 0)
            {
                SRData__UpdateShort(&(peak_detector->Max_Peak_Out), &(n_peak_detector->Max_Peak_Out), peak_detector->Max_Peak_Rough);
                SRData__UpdateShort(&(peak_detector->Min_Peak_Out), &(n_peak_detector->Min_Peak_Out), peak_detector->Min_Peak_Rough);

                // Reset Min and Max
                SRData__UpdateShort(&(peak_detector->Min_Peak_Rough), &(n_peak_detector->Min_Peak_Rough), 65535);
                SRData__UpdateShort(&(peak_detector->Max_Peak_Rough), &(n_peak_detector->Max_Peak_Rough), 0);

                SRData__UpdateShort(&(peak_detector->Period_Counter_Rough), &(n_peak_detector->Period_Counter_Rough), peak_detector->Period_Counter_Reload);
            }
            else
            {
                SRData__UpdateShort(&(peak_detector->Period_Counter_Rough), &(n_peak_detector->Period_Counter_Rough), (peak_detector->Period_Counter_Rough) - 1);
            }
        }
    }

#else //PEAKDETECTOR_SR

    PEAK_DETECTOR_TYPE *peak_detector;

    if( channel < PEAK_CHANNEL_NUM )
    {
        peak_detector = &Peak_Detector[channel];
        // Search for Max and Min
        if( current_value > peak_detector->Max_Peak_Rough )
        {
            peak_detector->Max_Peak_Rough = current_value;
        }
        if( current_value <  peak_detector->Min_Peak_Rough )
        {
            peak_detector->Min_Peak_Rough = current_value;
        }

        if( peak_detector->Period_Counter_Rough == 0)
        {
            peak_detector->Max_Peak_Out = peak_detector->Max_Peak_Rough;
            peak_detector->Min_Peak_Out = peak_detector->Min_Peak_Rough;

            // Reset Min and Max
            peak_detector->Min_Peak_Rough = 65535;
            peak_detector->Max_Peak_Rough = 0;

            peak_detector->Period_Counter_Rough = peak_detector->Period_Counter_Reload;
        }
        else
        {
            peak_detector->Period_Counter_Rough--;
        }
    }

#endif //PEAKDETECTOR_SR
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get minimal value from previous samples set.
 *
 * @param    channel - Channel number taken from #PEAK_DETECTOR_CHANNEL_TYPE list.
 *
 * @return   Minimal value taken from period samples number.
 */
uint16 PeakDetector__GetMin(PEAK_DETECTOR_CHANNEL_TYPE channel)
{
#if (PEAKDETECTOR_SR == ENABLED)

	uint16 retval;

    retval = 0;

    if( channel < PEAK_CHANNEL_NUM )
    {
        if( SRDATA_OK == SRData__CheckShort(&(Peak_Detector[channel].Min_Peak_Out), &(N_Peak_Detector[channel].Min_Peak_Out)) )
        {
            retval =  Peak_Detector[channel].Min_Peak_Out;
        }
    }
    return retval;

#else //PEAKDETECTOR_SR

    uint16 retval;

    retval = 0;

    if( channel < PEAK_CHANNEL_NUM )
    {
        retval = Peak_Detector[channel].Min_Peak_Out;
    }
    return retval;

#endif //PEAKDETECTOR_SR
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get maximal value from previous samples set.
 *
 * @param    channel - Channel number taken from #PEAK_DETECTOR_CHANNEL_TYPE list.
 *
 * @return   Maximal value taken from period samples number.
 */
uint16 PeakDetector__GetMax(PEAK_DETECTOR_CHANNEL_TYPE channel)
{
#if (PEAKDETECTOR_SR == ENABLED)

	uint16 retval;

    retval = 0;

    if( channel < PEAK_CHANNEL_NUM )
    {
        if( SRDATA_OK == SRData__CheckShort(&(Peak_Detector[channel].Max_Peak_Out), &(N_Peak_Detector[channel].Max_Peak_Out)) )
        {
            retval =  Peak_Detector[channel].Max_Peak_Out;
        }
    }
    return retval;

#else //PEAKDETECTOR_SR

    uint16 retval;

    retval = 0;

    if( channel < PEAK_CHANNEL_NUM )
    {
        retval = Peak_Detector[channel].Max_Peak_Out;
    }
    return retval;

#endif //PEAKDETECTOR_SR
}
//---------------------------------------------------------------------------------------------------------------------
/**
 * Get peak to peak value from previous samples set.
 *
 * @param    channel - Channel number taken from #PEAK_DETECTOR_CHANNEL_TYPE list.
 *
 * @return   Peak to peak value taken from period samples number.
 */
uint16 PeakDetector__GetPeak2Peak(PEAK_DETECTOR_CHANNEL_TYPE channel)
{
#if (PEAKDETECTOR_SR == ENABLED)

	uint16 retval;
    PEAK_DETECTOR_TYPE *peak_detector;
    PEAK_DETECTOR_TYPE *n_peak_detector;

    retval = 0;

    if( channel < PEAK_CHANNEL_NUM )
    {
        peak_detector = &Peak_Detector[channel];
        n_peak_detector = &N_Peak_Detector[channel];
        
        if( (SRDATA_OK == SRData__CheckShort(&(peak_detector->Max_Peak_Out), &(n_peak_detector->Max_Peak_Out))) &&
            (SRDATA_OK == SRData__CheckShort(&(peak_detector->Min_Peak_Out), &(n_peak_detector->Min_Peak_Out))) )
        {
            if( peak_detector->Max_Peak_Out > peak_detector->Min_Peak_Out )
            {
                retval =  peak_detector->Max_Peak_Out - peak_detector->Min_Peak_Out;
            }
        }
    }
    return retval;

#else //PEAKDETECTOR_SR

    uint16 retval;
    PEAK_DETECTOR_TYPE *peak_detector;

    retval = 0;
    peak_detector = &Peak_Detector[channel];

    if( (channel < PEAK_CHANNEL_NUM) &&
        (peak_detector->Max_Peak_Out > peak_detector->Min_Peak_Out) )
    {
        retval = peak_detector->Max_Peak_Out - peak_detector->Min_Peak_Out;
    }
    return retval;
    
#endif //PEAKDETECTOR_SR
}

//=====================================================================================================================
//  --- Private Functions ---------------------------------------------------------------------------------------------
//=====================================================================================================================

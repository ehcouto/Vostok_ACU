/**
 *  @file
 *  @brief      Public interface to PeakDetector module.
 *
 *  $Revision: 1.6 $
 *
 *  Copyright 2011 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
#ifndef PEAKDETECTOR_H_
#define PEAKDETECTOR_H_

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "PeakDetector_prm.h"

#ifndef PEAKDETECTOR_SR
#define PEAKDETECTOR_SR DISABLED
#endif //PEAKDETECTOR_SR

#if (PEAKDETECTOR_SR == ENABLED)
#include "NSRPeakDetector_prm.h"
#endif //PEAKDETECTOR_SR

// -- ANSI/ISO --

// -- Project --


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================

typedef enum
{
    PEAK_CHANNEL_NAME_LIST,         //!< Basic channels list (when PEAKDETECTOR_SR is ENABLED it is SR list)
#if (PEAKDETECTOR_SR == ENABLED)
    PEAK_CHANNEL_NAME_LIST_NSR,    //!< List from NSR software
#endif //PEAKDETECTOR_SR
    PEAK_CHANNEL_NUM
}PEAK_DETECTOR_CHANNEL_TYPE;

//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================

extern void PeakDetector__Initialize(void);

extern void PeakDetector__Setup(PEAK_DETECTOR_CHANNEL_TYPE channel, uint16 period);
extern void PeakDetector__Reset(PEAK_DETECTOR_CHANNEL_TYPE channel);
extern void PeakDetector__SetValue(PEAK_DETECTOR_CHANNEL_TYPE channel, uint16 current_value);

extern uint16 PeakDetector__GetMin(PEAK_DETECTOR_CHANNEL_TYPE channel);
extern uint16 PeakDetector__GetMax(PEAK_DETECTOR_CHANNEL_TYPE channel);
extern uint16 PeakDetector__GetPeak2Peak(PEAK_DETECTOR_CHANNEL_TYPE channel);


#endif // PEAKDETECTOR_H_

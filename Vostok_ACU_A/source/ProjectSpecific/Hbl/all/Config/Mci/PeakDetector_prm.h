/**
 *  @file
 *  @brief      Public parameters for PeakDetector module.
 *
 *  $Revision: 1.5 $
 *
 *  @copyright  Copyright 2011-$Date: 2014/12/01 10:29:04BRST $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
#ifndef PEAKDETECTOR_PRM_H_
#define PEAKDETECTOR_PRM_H_

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- ANSI/ISO --

// -- Project --


//=====================================================================================================================
//  --- Public Parameters ---------------------------------------------------------------------------------------------
//=====================================================================================================================

//! To enable SR check in PeakDetector module set it to ENABLED
#define PEAKDETECTOR_SR  DISABLED

//! Define channels of type PEAK_DETECTOR_CHANNEL_TYPE
//! Order of channels PEAK_CURRENT_PHASE_U_SHORT, PEAK_CURRENT_PHASE_V_SHORT, PEAK_CURRENT_PHASE_W_SHORT 
//! must be the same like in #SRMOTORGUARD_CURRENT_CHANNEL_U, #SRMOTORGUARD_CURRENT_CHANNEL_V, #SRMOTORGUARD_CURRENT_CHANNEL_W
#define PEAK_CHANNEL_NAME_LIST \
    PEAK_MCI_CHANNEL_VOLT, \
    PEAK_MCI_CHANNEL_CURR
    
//! This macro is called if at PeakDetector__Initialize.
#define PEAKDETECTION_INITIALIZE_FLOW()    SRFlow__InitLogEvent(PEAKDETECTION_INITIALIZE)
//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================

#endif  // PEAKDETECTOR_PRM_H_

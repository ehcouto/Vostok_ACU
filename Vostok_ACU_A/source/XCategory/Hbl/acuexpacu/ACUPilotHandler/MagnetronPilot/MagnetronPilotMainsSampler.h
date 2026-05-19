/**
 *  @file
 *
 *  @brief      Public interface for the Magnetron Pilot Mains Sampler.
 *
 *  @copyright  Copyright 2018 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef MAGNETRONPILOTMAINSSAMPLER_H
	#define MAGNETRONPILOTMAINSSAMPLER_H

#ifndef HBL_PILOT_NUM_MAGNETRON
    #define HBL_PILOT_NUM_MAGNETRON    0
#endif

#if (HBL_PILOT_NUM_MAGNETRON > 0)

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//! Enumeration for mains frequency state
typedef enum
{
    MAINS_FREQ_ERROR,       //!< Mains signal frequency outside limits
    MAINS_FREQ_OK           //!< Mains signal frequency ok
}MAINS_FREQUENCY_TYPE;

//! Enumeration for mains event type
typedef enum
{
    MAINS_NO_EVENT = 0,
    MAINS_ANGLE_0_EVENT,
    MAINS_ANGLE_90_EVENT,
    MAINS_ANGLE_180_EVENT,
    MAINS_ANGLE_270_EVENT
}MAINS_EVENT_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void MagnetronPilotMainsSampler__Initialize(void);
void MagnetronPilotMainsSampler__InitializeInstance(uint8 Parent_Pilot_Index, uint8 AC_Mains_Same_Phase, uint8 Mains_Feedback_Pin);
void MagnetronPilotMainsSampler__200usHandler(uint8 Parent_Pilot_Index);
void MagnetronPilotMainsSampler__AsynchHandler(uint8 Parent_Pilot_Index);
MAINS_FREQUENCY_TYPE MagnetronPilotMainsSampler__GetMainsFrequencyStatus(uint8 Parent_Pilot_Index);

#endif //(HBL_PILOT_NUM_MAGNETRON > 0)

#endif // MAGNETRONPILOTMAINSSAMPLER_H



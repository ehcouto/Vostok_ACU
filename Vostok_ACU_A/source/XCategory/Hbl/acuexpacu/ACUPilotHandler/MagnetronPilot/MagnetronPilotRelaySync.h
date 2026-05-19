/**
 *  @file
 *
 *  @brief      Public interface for the Magnetron Pilot Relay Sync.
 *
 *  @copyright  Copyright 2018 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef MAGNETRONPILOTRELAYSYNC_H
#define MAGNETRONPILOTRELAYSYNC_H

#ifndef HBL_PILOT_NUM_MAGNETRON
    #define HBL_PILOT_NUM_MAGNETRON       0
#endif

#if (HBL_PILOT_NUM_MAGNETRON > 0)
#include "MagnetronPilot.h"
#include "MagnetronPilotMainsSampler.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void MagnetronPilotRelaySync__Initialize(void);
void MagnetronPilotRelaySync__InitializeInstance(uint8 Parent_Pilot_Index, uint8 MW_Feedback_Pin, uint8 MW_FB_Active_High, uint8 MW_Relay_Pin, uint8 Turbo_Enable, uint8 Turbo_Relay_Pin, uint8 Turbo_On_Off_Delay, uint8 Door_GI_Index);
void MagnetronPilotRelaySync__200usHandler(uint8 Parent_Pilot_Index);
void MagnetronPilotRelaySync__AsynchHandler(uint8 Parent_Pilot_Index, MAGNETRON_PILOT_REQUEST_TYPE Pilot_Request);
void MagnetronPilotRelaySync__MainsPeriodSync(uint8 Parent_Pilot_Index, uint8 Period);
void MagnetronPilotRelaySync__MainsEventHandler(uint8 Parent_Pilot_Index, MAINS_EVENT_TYPE Event_Type);

#endif //(HBL_PILOT_NUM_MAGNETRON > 0)

#endif // MAGNETRONPILOTRELAYSYNC_H



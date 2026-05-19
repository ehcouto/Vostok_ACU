 /**
 *  @file       Vwi.h
 *  @brief      Public interface to Vwi module.
 *
 *  Copyright 2008-2011 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
#ifndef SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_VWI_VWIH_
#define SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_VWI_VWIH_

//  --- Include Files -------------------------------------------------------------------------------------------------
#include "Vwi_prv.h"
#if(VWI_CLIENT_FEATURE == ENABLED)
#include "Process.h"
#endif
#include "Log.h"
#include "SettingFile.h"
// -- ANSI/ISO --

// -- Project --


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================

//! Possible water state detected by VWI
typedef enum
{
    WS_NOT_VALID = 0,
    WS_NO_WATER,
    WS_UNSTABLE_NO_WATER,
    WS_UNSTABLE_WATER,
    WS_WATER
} VWI_WATER_SENSE_TYPE;

//! VWI phase of calibration
typedef enum
{
    INITIALIZATION,                             //!< Initial state, algorithm is reset
    STABILIZATION,                              //!< Stabilization time, algorithm can be rest again in this state
    CALIBRATION,                                //!< State in which calibration is active
    NO_CALIBRATION                              //!< State in which calibration is inactive
}VWI_ANALYZE_PHASE_TYPE;

extern VWI_WATER_SENSE_TYPE Vwi_Status;

//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================



void Vwi__Initialize(void);
void Vwi__Handler(void);
uint16 Vwi__GetHeavyFiltSignalNoComp(void);
uint16 Vwi__GetHeavyFiltSignal(void);
uint16 Vwi__GetPrevHeavyFiltSignal(void);
uint16 Vwi__GetHeavyFiltSignalAverage(void);
uint16 Vwi__GetLightFiltSignal(void);
uint16 Vwi__GetHeavyFiltVariance(void);
uint16 Vwi__GetVariance(void);
uint16 Vwi__GetSignal(void);
VWI_WATER_SENSE_TYPE Vwi__GetWaterStatus(void);
VWI_WATER_SENSE_TYPE Vwi__GetLastValidWaterStatus(void);
uint8 Vwi__GetCurrentMotor(void);
sint32 Vwi__ServerRemoteData(uint8 channel);
void Vwi__RemoteChannelEnable(uint8 motor, uint8 channel, BOOL_TYPE enable);
void Vwi__RemoteChannelPeriod(uint8 channel, uint16 period);

#if ((VWI_THREE_PHASE_MOTOR_TYPE== ENABLED) && ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED)))
void Vwi_SetInputSignalMultiplier(uint16 multiplier);
uint16 Vwi__SignalMCURaw(void);
uint16 Vwi__SignalMCUMlp(void);
#endif // ((VWI_THREE_PHASE_MOTOR_TYPE== ENABLED) && ((VWI_SERVER_FEATURE == ENABLED) || (VWI_INTERNAL_COMMUNICATION == ENABLED)))



#if(VWI_INTERNAL_COMMUNICATION == ENABLED)
VWI_ANALYZE_PHASE_TYPE  Vwi__GetAnalyzePhase(void);
#endif

#endif // SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_VWI_VWIH_


/**
 *  @file       
 *
 *  @brief      Basic description of file contents
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
#ifndef MCLOBSERVER_H_
#define MCLOBSERVER_H_

#include "MclTypes.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef struct
{
    sint32  Rpd_Flux_Threshold_Low;
    sint32  Rpd_Flux_Threshold_High;

    uint8   Active_Power_Filter_Taps;
    uint8   Load_Angle_Filter_Taps;
    uint32  Flux_Saturation_Thr;            //!< Total flux threshold to use the Motor_Saturated_Inductance for magnet flux calculation - [I17Q15]

    uint32  Motor_Inductance;               //!< Motor inductance - [I17Q15]
    uint32  Motor_Saturated_Inductance;     //!< Motor saturated inductance, used when the total flux is higher than Flux_Saturation_Thr - [I17Q15]

    uint32 Gain;

    sint32 Rpd_Bemf_Threshold_Low;
    sint32 Rpd_Bemf_Threshold_High;

} MCL_OBSERVER_PARAMS_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void MclObserver__Initialize(MCL_OBSERVER_IO_TYPE* io, MCL_PARAMS_TYPE* params);
void MclObserver__ResetState(MCL_PARAMS_TYPE* params);
void MclObserver__RunningHandler(MCL_OBSERVER_IO_TYPE* io, MCL_PARAMS_TYPE* params);
void MclObserver__ActivePowerEstimation(MCL_OBSERVER_IO_TYPE* io, MCL_PARAMS_TYPE* params);

#endif // MCLOBSERVER_H_



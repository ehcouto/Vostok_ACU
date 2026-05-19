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
#ifndef MCLFIREANGLECTRL_H_
#define MCLFIREANGLECTRL_H_

#include "MclTypes.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef struct
{
    sint32  Fire_Angle_Pi_Kp;
    sint32  Fire_Angle_Pi_Ki;
    sint32  Fire_Angle_Pi_Upper_Limit;
    sint32  Fire_Angle_Pi_Lower_Limit;
    sint32  Load_Angle_Sin_Ref;         //!< Reference value for the sinus of the power angle
    uint32  Fire_Angle_Perc_Init;
} MCL_FIREANGLE_CTRL_PARAMS_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void MclFireAngleCtrl__Initialize(MCL_FIREANGLE_CTRL_IO_TYPE* io, MCL_PARAMS_TYPE* params);
void MclFireAngleCtrl__ZeroCrossHandler(MCL_FIREANGLE_CTRL_IO_TYPE *io, MCL_PARAMS_TYPE* params);
void MclFireAngleCtrl__MotorControlHandler(MCL_FIREANGLE_CTRL_IO_TYPE *io, MCL_PARAMS_TYPE* params);
void MclFireAngleCtrl__ResetState(MCL_FIREANGLE_CTRL_IO_TYPE *io, MCL_PARAMS_TYPE* params);

#endif // MCLFIREANGLECTRL_H_



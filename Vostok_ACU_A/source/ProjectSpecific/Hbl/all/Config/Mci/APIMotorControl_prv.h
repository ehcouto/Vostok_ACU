
/**
 *  @file       APIMotorControl_prv.h
 *
 *  @brief      Private parameters used in the APIMotorControl.
 *
 *  @copyright  Copyright 2013-2018. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef APIMOTORCONTROL_PRV_H_
    #define APIMOTORCONTROL_PRV_H_


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "Mci.h"

//=====================================================================================================================
//  --- Private Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================

typedef BOOL_TYPE T_BOOL;

// -- Private Macro Declarations --
#define MANUAL_INJECTION_RUN(motor_index, method, level_target, level_rate, parameter, param_rate)              Mci__Manual_Injection(motor_index, method, level_target, level_rate, parameter, param_rate)

#define FVT_START(motor_index)
#define FVT_STOP(motor_index)

#endif // APIMOTORCONTROL_PRV_H_


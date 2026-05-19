/**
 *  @file       Vwi_prv.h
 *  @brief      Private parameters for Vwi module.
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2011.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#ifndef SOURCE_PROJECTSPECIFIC_COMMON_CONFIG_VWI_PRV_H_
#define SOURCE_PROJECTSPECIFIC_COMMON_CONFIG_VWI_PRV_H_

//  --- Include Files -------------------------------------------------------------------------------------------------
#include "limits.h"
#include "SystemConfig.h"

//=====================================================================================================================
//  --- Private Parameters --------------------------------------------------------------------------------------------
//=====================================================================================================================

//! Module settings
#define VWI_CLIENT_FEATURE 		(DISABLED)
#define VWI_SERVER_FEATURE 		(DISABLED)


#ifndef VWI_CLIENT_FEATURE
	#error "VWI_CLIENT_FEATURE must be defined as ENABLED or DISABLED."
#endif

#ifndef VWI_SERVER_FEATURE
	#error "VWI_SERVER_FEATURE must be defined as ENABLED or DISABLED."
#endif

#if ((VWI_CLIENT_FEATURE == ENABLED) && (VWI_SERVER_FEATURE == ENABLED))
    #error "There is currently no support for the VWI Client and Server on the same time."
#endif

#if ((VWI_CLIENT_FEATURE == DISABLED) && (VWI_SERVER_FEATURE == DISABLED))
    #define VWI_INTERNAL_COMMUNICATION	(ENABLED)
	#define ACU_MOTOR_CONTROL			(ENABLED)
#else
    #define VWI_INTERNAL_COMMUNICATION	(DISABLED)
	#define ACU_MOTOR_CONTROL			(DISABLED)
#endif

//! Algorithm settings
#define	VWI_SINGLE_SPEED_MOTOR_TYPE				  (ENABLED)
#define	VWI_THREE_PHASE_MOTOR_TYPE				  (DISABLED)
#define VWI_SETTINGFILE_MOTOR_TYPE                (DISABLED)

#if((VWI_SINGLE_SPEED_MOTOR_TYPE == ENABLED) && (VWI_THREE_PHASE_MOTOR_TYPE == ENABLED))
	#error "Only one type of motor can be enabled"
#endif


#define MOTOR_NUM	(2)

//! SettingFile pointer and displacement
#define VWI_SETTING_FILE_PTR				(SF_PTR_ACU_ALGORITHM_PARAMETERS)
#define VWI_SETTING_FILE_DISPL_DEFAULT		(DISPL_VWI_WASH_PUMP)


#define VWI_MOTOR_SF_SETTINGS {  \
									DISPL_VWI_WASH_PUMP, \
									DISPL_VWI_DRAIN_PUMP \
							  }  \



//! Enable options
#define SPEED_CHANGES_COMP                DISABLED   //!< Speed changes compensation for VSM motor ENABLED
#define VWI_CALIBRATION               	  ENABLED   //!< Vwi calibration

//! Gnereral parameters
#define VWI_STABILIZATION_TIME_MS           2000    //!< Stabilization time after motor is ON
#define VWI_DEBOUNCE_TIME_MS                 500    //!< #Vwi_Status debounce time
#define VWI_INIT_CAL_CORRECTION_DIV            2    //!< Divider for initial #Vwi_Calibration_Correction calculation
#define VWI_INIT_VARIANCE_ADD                  7    //!< This value is added to variance thr to get variance initialization value

//! Filtering parameters
#define HEAVY_FILTRATION_ALPHA_BITS            7    //!< Alpha bits for heavy filter
#define LIGHT_FILTRATION_ALPHA_BITS            2    //!< Alpha bits for light filter

#define SPEED_COMP_MULTIPLIER                1024    //!< Signal is multiplied by this value and divided by proper value from table


//=====================================================================================================================
//  --- Private Methods -----------------------------------------------------------------------------------------------
//=====================================================================================================================

#define VWI_GET_MOTOR_SIGNAL(motor)				(Mci__GetAnalog(MOTOR0, MCI_AI_LOAD_TORQUE_S16)) //!< Function witch provide signal for single speed motors
#define VWI_IS_MOTOR_ROTATE(motor)				(IsMotorRotate())
#define VWI_GET_MOTOR_ID()						(MOTOR0)
#define VWI_SET_WASH_MOTOR_TYPE(motor_type)		Mci__SetWashMotorType(motor_type)


#endif // SOURCE_PROJECTSPECIFIC_COMMON_CONFIG_VWI_PRV_H_







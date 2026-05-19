/**
 *  @file
 *
 *  @brief      Basic description of file contents
 *
 *  @copyright  Copyright 2015-2016 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef CONDITION_H_
#define CONDITION_H_

#include "SystemConfig.h"
#include "SettingFile.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
/**
 * Possible return types for Condition.
 * We return FALSE or TRUE if all went okay.
 * If during runtime there is an error on the settings file or the
 * displacement is empty, we return 2 errors so the caller can manage. No need to load the settings file
 * on the caller to consist those conditions.
 */
typedef enum CONDITION_RESULT_ENUM
{
	CONDITION_RESULT_FALSE 			= 0,      	//!< CONDITION_RESULT_FALSE
	CONDITION_RESULT_TRUE 			= 1,       	//!< CONDITION_RESULT_TRUE
	CONDITION_ERROR_ON_SETTING_FILE = 3,		//!< CONDITION_ERROR_ON_SETTING_FILE
	CONDITION_ERROR_NO_DISPLACEMENT = 4 		//!< CONDITION_ERROR_NO_DISPLACEMENT
}CONDITION_RESULT_TYPE;
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#define CONDITION_LEGACY_EVAL(displacement)         Condition__Evaluate(SF_PTR_ACU_CONDITION_TABLE, displacement)

CONDITION_RESULT_TYPE Condition__Evaluate(SETTINGFILE_OFFSET_TYPE pointer, uint16 displacement);

#endif // CONDITION_H_



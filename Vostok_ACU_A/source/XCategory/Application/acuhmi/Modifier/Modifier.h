/**
 *  @file
 *
 *  @brief      Basic description of file contents
 *
 *  @copyright  Copyright 2015-2016 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef MODIFIER_H_
#define MODIFIER_H_

#include "SystemConfig.h"
#include "SettingFile.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
/**
 * Enumerator that defines which Operator will be applied to the Original Value.
 */
typedef enum MODIFIER_OPERATOR_ENUM
{
	MODIFIER_OPERATOR_REPLACE  = 0,                 //!< Modified Value = Operand
	MODIFIER_OPERATOR_MULTIPLY = 1,                 //!< Modified Value = Original Value * Operand
	MODIFIER_OPERATOR_ADD      = 2,                 //!< Modified Value = Original Value + Operand
	MODIFIER_OPERATOR_SUBTRACT = 3,                 //!< Modified Value = Original Value - Operand
	MODIFIER_OPERATOR_MAX      = 4,                 //!< Modified Value = Max(Original Value, Operand)
	MODIFIER_OPERATOR_MIN      = 5,                 //!< Modified Value = Min(Original Value, Operand)
	MODIFIER_OPERATOR_RESERVED
}MODIFIER_OPERATOR_TYPE;
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================


#if (SYSTEM_ACU_HANDLING == ENABLED)
    // These interfaces are provided for backward compatibility on ACUs.
    MODIFIER_OPERATOR_TYPE Modifier__GetOperator(uint16 modifier_displacement);
    sint32 Modifier__GetValue(sint32 original_value, uint16 modifier_displacement);
#endif
    float64 Modifier__ModifyValue(float64 original_value, SETTINGFILE_OFFSET_TYPE pointer, uint16 modifier_displacement);


#endif // MODIFIER_H_



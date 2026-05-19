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
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef VOLTAGERANGE_H_
#define VOLTAGERANGE_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

typedef enum VOLTAGERANGE_STATE_ENUM
{
	VOLTAGERANGE_STATE_UNDEFINED,
	VOLTAGERANGE_STATE_GOODRANGE,
	VOLTAGERANGE_STATE_LOWVOLTAGE,
	VOLTAGERANGE_STATE_HIGHVOLTAGE,
} VOLTAGERANGE_STATE_TYPE;

extern VOLTAGERANGE_STATE_TYPE Voltage_Range_Status;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void VoltageRangeMonitor__Initialize(void);
void VoltageRangeMonitor__Execution(void);

VOLTAGERANGE_STATE_TYPE VoltageRangeMonitor__GetStatus(void);
BOOL_TYPE VoltageRangeMonitor__IsGoodRange(void);

#endif // VOLTAGERANGE_H_



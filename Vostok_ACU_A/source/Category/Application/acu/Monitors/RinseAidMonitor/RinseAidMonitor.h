/**
 *  @file       RInseAidMonitor.h
 *  @brief      Basic description of file contents
 *
 *---------------------------------------------------------------------------------------------------------------------
 *------------------- Copyright 2011.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL ---------------------
 *---------------------------------------------------------------------------------------------------------------------
 */
#ifndef SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_RINSEAIDMONITOR_RINSEAIDMONITOR_H_
#define SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_RINSEAIDMONITOR_RINSEAIDMONITOR_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//! Setting file data structure for pointer SF_PTR_ACU_ALGORITHM_PARAMETERS

typedef PACKED struct RINSE_AID_MONITOR_INSTANCE_STRUCT
{
	uint8 Door_Close_Timer;
	uint8 Threshold_1;
	uint8 Threshold_2;
} RINSE_AID_MONITOR_INSTANCE_TYPE;

extern uint8 Rinse_Aid_Status;
extern uint8 RinseAid_Dispenser_Use_Cnt;
extern RINSE_AID_MONITOR_INSTANCE_TYPE Sf_RinseAid_Data;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void RinseAidMonitor__Initialize(void);
void RinseAidMonitor__Execution(void);
void RinseAidMonitor__CheckEvent(void);

#endif // SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_RINSEAIDMONITOR_RINSEAIDMONITOR_H_



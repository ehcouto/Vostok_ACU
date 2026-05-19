/**
 *  @file       SRIsrMonitor_prv.h
 *
 *  @brief      Private (prv) configuration file for SRIsrMonitor module
 *
 *  @defgroup	CLASS_B
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRISRMONITOR_PRM_H_
#define SRISRMONITOR_PRM_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

// It defines maximum number of interrupts allowed that may occurs between two SRIsrMonitor__Handler() calls
// This value can be even an uint32; for diagnostic needs it can be set to a very high value to prevent exception.
// WARNING: If set to 0 disables SRIsrMonitor module at all - handle with care!
#define SRISRMONITOR_MAX				200

// It defines minimum number of interrupts that may occurs between two SRIsrMonitor__Handler() calls
// This value must be lower than SRISRMONITOR_MAX (except if module is disabled by setting SRISRMONITOR_MAX is set to 0)
#define SRISRMONITOR_MIN				4

// It defines maximum number of interrupts allowed that may occurs between two SRIsrMonitor__Handler() calls in Low Speed mode.
// This value can be even an uint32; for diagnostic needs it can be set to a very high value to prevent exception.
// WARNING: If not defined, SRISRMONITOR_MAX will be used instead!
#define SRISRMONITOR_LOWSPEED_MAX                100

// It defines minimum number of interrupts that may occurs between two SRIsrMonitor__Handler() calls
// This value must be lower than SRISRMONITOR_LOWSPEED_MAX
// WARNING: If not defined, SRISRMONITOR_MIN will be used instead!
#define SRISRMONITOR_LOWSPEED_MIN                1

// It defines maximum number of consecutive interrupts lower than minimum value are allowed before exception;
// 0 means an immediate action at first wrong value.
// Usually it's better to set it to 1 to prevent a false reset at startup - first iteration may not reach
// minimum number of interrupts set by SRISRMONITOR_MIN.
#define SRISRMONITOR_MAX_ATTEMPTS		1

// This option enables some statistic value - maximum and minimum interrupt counted and attempts done.
#define SRISRMONITOR_STATS				ENABLED

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif // SRISRMONITOR_PRM_H_

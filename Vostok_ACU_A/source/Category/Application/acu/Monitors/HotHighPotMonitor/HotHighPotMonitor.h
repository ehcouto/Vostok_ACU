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
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_HOTHIGHPOTMONITOR_HOTHIGHPOTMONITOR_H_
#define SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_HOTHIGHPOTMONITOR_HOTHIGHPOTMONITOR_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef PACKED struct HOTHIGHPOT_MONITOR_INSTANCE_STRUCT
{
    uint8 Events;
    uint8 Time;
    uint8 Autostart_Factory_Cycle_Procedure     : 1;
    uint8 										: 7;
    uint8 Filler;
} HOTHIGHPOT_MONITOR_INSTANCE_TYPE;

typedef enum HEATER_DIGITAL_OUTPUT_MULTI_2_STATE_ENUM
{	HDOM2_LOAD_STATE_PIN_1 = 0x21,
	HDOM2_LOAD_STATE_PIN_2 = 0x11
} HEATER_DIGITAL_OUTPUT_MULTI_2_STATE_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void HotHighPotMonitor__Initialize(void);
void HotHighPotMonitor__Execution(void);
BOOL_TYPE HotHighPotMonitor__GetHotHighPotDone(void);
#endif // SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_HOTHIGHPOTMONITOR_HOTHIGHPOTMONITOR_H_



/**
 *  @file
 *
 *  @brief      Private non-safety parameters for the Hbl module.
 *
 *  @copyright  Copyright 2015-$Date$ Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef HBL_PRV_H_
#define HBL_PRV_H_

#include "HblDefs.h"
#include "SystemConfig.h"

//=====================================================================================================================
//-------------------------------------- PRE-PROCESSOR ----------------------------------------------------------------
//=====================================================================================================================
//Example HBL pre-processor load control. Uncomment and customize as required.
//static const uint8 hbl_on   = 1;
//#define HBL__PREPROCESSLOADS() \
//{ \
//    Hbl__SetLoadRequestByLoadID(HBL_LOAD_DUMMY_1,HBL_PRIORITY_PC_CONTROL,(uint8 *)&hbl_on); \
//    if (Hbl_Led_Blink == TRUE) \
//    { \
//        Hbl__SetLoadRequestByLoadID(HBL_LOAD_LED1,HBL_PRIORITY_PC_CONTROL,(uint8 *)&hbl_on); \
//        Hbl_Led_Blink = FALSE; \
//    }  \
//    else \
//    { \
//        Hbl_Led_Blink = TRUE; \
//    } \
//}

//=====================================================================================================================
//-------------------------------------- LOADS ------------------------------------------------------------------------
//=====================================================================================================================


/** Definition of Direct Loads (where the Load is the pilot without any special treatment)
 *
 *  Define this macros as a list of HBL_LOAD_ENUM_TYPE items separated by a comma except the last one;
 * it's recommended to assign the expected value to each item.
 */
#define HBL_DIRECT_LOAD_LIST        \
    HBL_LOAD_DRAIN_PUMP, \
    HBL_LOAD_DRYING_HEATER, \
    HBL_LOAD_FILL_VALVE, \
    HBL_LOAD_REGENERATION_VALVE, \
    HBL_LOAD_LOWER_SPRAY_ARM_MOTOR, \
    HBL_LOAD_WATER_TANK, \
    HBL_LOAD_VENT, \
	HBL_LOAD_LOWER_TANK_VALVE, \
	HBL_LOAD_UPPER_TANK_VALVE, \
    HBL_LOAD_DISPENSER, \
    HBL_LOAD_OZONO, \
    HBL_LOAD_SALT_INPUT_ACTIVATION, \
	HBL_POR_TIME_CAPTURE_CIRCUIT, \
	HBL_LOAD_MCU_ENABLE, \
	HBL_LOAD_DOOR_OPENING_SYSTEM, \
	HBL_LOAD_RINSE_AID_INPUT_ACTIVATION, \
	HBL_WIN_12V_ACTIVATION, \
    HBL_LOAD_AUTO_KILL, \
    HBL_LOAD_PILOT_RELAY, \
    HBL_LOAD_RELAY_ENABLE, \
    HBL_LOAD_DRY_FAN, \
    HBL_LOAD_LITT, \
    HBL_LOAD_LOTF, \
    HBL_LOAD_OWI, \
    HBL_LOAD_DUMMY_0, \
    HBL_LOAD_DUMMY_1, \
    HBL_LOAD_DUMMY_2, \
    HBL_LOAD_DUMMY_3, \
    HBL_LOAD_DUMMY_4, \
    HBL_LOAD_DUMMY_5, \
    HBL_LOAD_DUMMY_6, \
    HBL_LOAD_DUMMY_7, \
    HBL_LOAD_DUMMY_8, \
    HBL_LOAD_DUMMY_9, \
    HBL_LOAD_DUMMY_10, \
    HBL_LOAD_DUMMY_11, \
    HBL_LOAD_DUMMY_12, \
    HBL_LOAD_DUMMY_13, \
    HBL_LOAD_DUMMY_14


#if (HBL_LOAD_HANDLER_FEATURE == ENABLED)
#if (SYSTEM_ACU_HANDLING == ENABLED)
    #include "AlternateLoad.h"
	#include "TriacMci.h"
	#include "HeaterLoad.h"
#endif


/** Definition of project specific Load handlers.
 *
 * Define this macros as a list of pairs { LOAD_REGISTER_MACRO, HBL_LOAD_ENUM_TYPE } separated by a comma except the last one.
 *
 */
#if (SYSTEM_ACU_HANDLING == ENABLED)
#define HBL_LOAD_HANDLERS_LIST \
    { ALTERNATELOAD_REGISTER    ,HBL_LOAD_DIVERTER }, \
	{ TRIACMCI_REGISTER         ,HBL_LOAD_WASH_PUMP}, \
    { HEATERLOAD_REGISTER       ,HBL_LOAD_WASHING_HEATER}
#else
#define HBL_LOAD_HANDLERS_LIST \
    { HEATERLOAD_REGISTER         ,HBL_LOAD_WASHING_HEATER}
#endif
#endif

/**
 * Cross Loads feature
 * Define HBL_CROSS_LOADS_FEATURE as ENABLED to enable Cross Load feature; default is DISABLED
 * If HBL_CROSS_LOADS_FEATURE is ENABLED:
 * - HBL_XLOAD_NUM_CROSSLOAD                    defines the maximum number of Cross Loads that can be handled; it must be defined
 * - HBL_XLOAD_MASTER_RELAY_SUPPORT             enables the Master Load feature - deafult: DISABLED
 * - HBL_XLOAD_SWITCH_RELAY_SUPPORT             enables the Swtich Load feature - deafult: DISABLED
 * - HBL_XLOAD_REVERSE_SWITCH_RELAY_SUPPORT     enables the Reverse Switch Load feature - deafult: DISABLED
 * - HBL_XLOAD_TRANSITION_RELAY_SUPPORT         enables the Transition Load feature - deafult: DISABLED
 */
#define HBL_CROSS_LOADS_FEATURE					ENABLED
#define HBL_XLOAD_NUM_CROSSLOAD					4
#define HBL_XLOAD_MASTER_RELAY_SUPPORT			ENABLED
#define HBL_XLOAD_SWITCH_RELAY_SUPPORT			ENABLED
#define HBL_XLOAD_REVERSE_SWITCH_RELAY_SUPPORT	DISABLED
#define HBL_XLOAD_TRANSITION_RELAY_SUPPORT		ENABLED

/*
 * Keep Loads OFF feature - Set OFF all loads listed when requested - e.g. on a Brownout event
 * if ENABLED, HBL_KEEP_LOADS_OFF_LIST_DEF must be defined with list of Load IDs that can be set OFF
 */
#define HBL_KEEP_LOADS_OFF_FEATURE					ENABLED
#if (HBL_KEEP_LOADS_OFF_FEATURE == ENABLED)
	#define HBL_KEEP_LOADS_OFF_PRIORITY         	(HBL_PRIORITY_PC_CONTROL - 1)
	#define HBL_KEEP_LOADS_OFF_EXCLUDE_FEATURE		DISABLED
//	#define HBL_KEEP_LOADS_OFF_EXCLUDE_LIST_DEF		HBL_LOAD_PILOT_RELAY
#endif

#define HBL_LOADS_QUEUE_FEATURE ENABLED

//=====================================================================================================================
//-------------------------------------- PILOTS -----------------------------------------------------------------------
//=====================================================================================================================
#include "DigitalOutputPilot.h"
#include "DigitalOutputMultiPilot.h"
#include "ChargePumpPilot.h"
#include "ZCRelayPilot.h"
#include "SteadyPwmProfilePilot.h"
#include "MultiSequencePilot.h"
#include "PwmPilot.h"
#include "PwmHiResPilot.h"


/** Definition of project specific Pilot handlers.
 *
 * Define this macros as a list of pairs { PILOT_REGISTER_MACRO, HBL_PILOT_ENUM_TYPE } separated by a comma except the last one.
 *
 */
#if (SYSTEM_ACU_HANDLING == ENABLED)
#define HBL_PILOT_HANDLERS_LIST \
    { DIGITALOUTPUTPILOT_REGISTER       ,HBL_PILOT_DIGITAL }, \
    { CHARGEPUMPPILOT_REGISTER          ,HBL_PILOT_CHARGEPUMP }, \
    { ZCRELAYPILOT_REGISTER				,HBL_PILOT_ZEROCROSSING_RELAY}, \
	{ STEADYPWMPROFILEPILOT_REGISTER    ,HBL_PILOT_STEADY_PWM_PROFILE}, \
    { DIGITALOUTPUTMULTIPILOT_REGISTER  ,HBL_PILOT_DIGITAL_OUTPUT_MULTI_2_PIN}, \
	{ PWMPILOT_REGISTER   				,HBL_PILOT_PWM}, \
	{ PWMHIRESPILOT_REGISTER   			,HBL_PILOT_PWM_HI_RES}, \
    { DIGITALOUTPUTPILOT_REGISTER       ,HBL_PILOT_TRIAC}

#else
#define HBL_PILOT_HANDLERS_LIST \
    { DIGITALOUTPUTPILOT_REGISTER       ,HBL_PILOT_DIGITAL }, \
    { CHARGEPUMPPILOT_REGISTER          ,HBL_PILOT_CHARGEPUMP }, \
    { ZCRELAYPILOT_REGISTER             ,HBL_PILOT_ZEROCROSSING_RELAY}, \
    { STEADYPWMPROFILEPILOT_REGISTER    ,HBL_PILOT_STEADY_PWM_PROFILE}, \
    { MULTISEQUENCEPILOT_REGISTER       ,HBL_PILOT_MULTISEQUENCE_1}, \
    { DIGITALOUTPUTMULTIPILOT_REGISTER  ,HBL_PILOT_DIGITAL_OUTPUT_MULTI_2_PIN}, \
    { PWMPILOT_REGISTER                 ,HBL_PILOT_PWM}, \
    { DIGITALOUTPUTPILOT_REGISTER       ,HBL_PILOT_TRIAC}

#endif

//=====================================================================================================================
//-------------------------------------- LOW LEVEL INPUTS -------------------------------------------------------------
//=====================================================================================================================
/*
 *  Allocates RAM for the GESE Settings file Low Level Inputs.
 *  Must be greater than or equal to the number of Low Level Inputs defined in the Setting File.
 */
#define HBL_MAX_NUM_LOWLEVELINPUT           22

#include "AnalogLLI.h"
#include "DigitalLLI.h"
#include "ExpansionLLI.h"
#include "FeedbackLLI.h"
#include "PulseCounterLLI.h"

/** Definition of project specific LLI handlers.
 *
 * Define this macros as a list of pairs { LLI_REGISTER_MACRO, HBL_LLI_ENUM_TYPE } separated by a comma except the last one.
 *
 * IMPORTANT NOTE! P1 (ACU+HMI) shall configure here all entries that should be in the HBLHMI_LLI_HANDLERS list.
 *
 */
#define HBL_LLI_HANDLERS_LIST \
    { DIGITAL_LLI_REGISTER            ,HBL_LLI_DIGITAL }, \
    { ANALOG_LLI_ASYNCH_REGISTER      ,HBL_LLI_ANALOG }, \
    { ANALOG_LLI_ONEMS_PEAKTOPEAK_REGISTER  ,HBL_LLI_PEAK_ANALOG}, \
    { FEEDBACK_LLI_REGISTER           ,HBL_LLI_FEEDBACK }, \
    { PULSECOUNTER_LLI_REGISTER       ,HBL_LLI_PULSE_COUNTER }, \
    { AC_LLI_REGISTER                 ,HBL_LLI_ZEROCROSS }, \
	{ EXPANSION_LLI_REGISTER          ,HBL_LLI_EXPANSION }

//=====================================================================================================================
//-------------------------------------- GENERIC INPUTS -------------------------------------------------------------
//=====================================================================================================================
#include "WaterCounterGI.h"
#include "ConvertGI.h"
#include "ProbeInput.h"

/** Definition of project specific GI handlers.
 *
 * Define this macros as a list of pairs { GI_REGISTER_MACRO, HBL_GI_ENUM_TYPE } separated by a comma except the last one.
 *
 * IMPORTANT NOTE! P1 (ACU+HMI) shall configure here all entries that should be in the HBLHMI_LLI_HANDLERS list.
 *
 */
#define HBL_GI_HANDLERS_LIST \
	{ WATERCOUNTER_GI_REGISTER  ,HBL_GI_WATER_COUNTER }, \
    { CONVERT_GI_REGISTER    	,HBL_GI_DOOR }, \
    { CONVERT_GI_REGISTER       ,HBL_GI_WASHING_PROBE }, \
    { CONVERT_GI_REGISTER       ,HBL_GI_RTN_LINE }, \
    { CONVERT_GI_REGISTER       ,HBL_GI_AC_LINE }, \
    { PROBE_GI_REGISTER         ,HBL_GI_TURBIDITY_SENSOR_OWI }, \
    { CONVERT_GI_REGISTER       ,HBL_GI_TURBIDITY_SENSOR_OWI_VRO }, \
    { CONVERT_GI_REGISTER       ,HBL_GI_RINSE_AID }, \
    { CONVERT_GI_REGISTER    	,HBL_GI_OVERFLOW }, \
    { CONVERT_GI_REGISTER       ,HBL_GI_ENTRAPMENT_INPUT_POR }, \
    { CONVERT_GI_REGISTER       ,HBL_GI_LIFE_TEST_MODE_SHORTCUT }, \
    { CONVERT_GI_REGISTER    	,HBL_GI_I_DIAGNOSTIC }, \
    { CONVERT_GI_REGISTER    	,HBL_GI_I_HEATER }, \
    { CONVERT_GI_REGISTER    	,HBL_GI_I_DC_FAN }, \
	{ CONVERT_GI_REGISTER    	,HBL_GI_I_DRAIN_PUMP }, \
	{ CONVERT_GI_REGISTER    	,HBL_GI_FLOW_METER_DIGITAL }, \
    { CONVERT_GI_REGISTER       ,HBL_GI_SALT }, \
	{ CONVERT_GI_REGISTER       ,HBL_GI_DIVERTER_FEEDBACK }, \
	{ CONVERT_GI_REGISTER       ,HBL_GI_V_DIAGNOSTIC },\
	{ CONVERT_GI_REGISTER    	,HBL_CARTRIDGE_SWITCH_1 }, \
	{ CONVERT_GI_REGISTER    	,HBL_CARTRIDGE_SWITCH_2 }, \
	{ CONVERT_GI_REGISTER    	,HBL_CARTRIDGE_DOOR}

/**
 * Enable use of GIs that can be handled by an Expansion ACU Board (Expansion GI, associated to an Expansion LLI)
 * Required (and recommended) to ENABLE it if Expansion ACU are used; if ENABLED, ensure that
 * - HBL_LLI_NUM_EXPANSION is properly set with a suitable number
 * - HBL_LLI_EXPANSION is registered using EXPANSION_LLI_REGISTER - provided by ExpansionLLI.h
 *
 * If sure that no Expansion board will be used at all, it can be DISABLED to save some space.
 */
#define HBL_USES_EXPANSION_GIS ENABLED

#endif // HBL_PRV_H_

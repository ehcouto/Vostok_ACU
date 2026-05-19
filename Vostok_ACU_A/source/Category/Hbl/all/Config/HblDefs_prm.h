/**
 *  @file
 *
 *  @brief  Public and configurable definitions for Hbl layer
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

#ifndef HBLDEFS_PRM_H_
#define HBLDEFS_PRM_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


/**
 * @brief HBL_PRIORITY_TYPE items used in Application modules
 *
 * @details Populate with the list of Load priority reference items.
 *
 * HBL_PRIORITY_PC_CONTROL is already defined in HblDefs.h, it should not be defined here
 * Define this macros as a list of enumeration items separated by a comma except the last one;
 * it's recommended to assign the expected value to each item.
 *
 */
#define HBL_LOAD_PRIORITY_LIST               \
    HBL_PRIORITY_APPLICATION_LOW         = 1, \
    HBL_PRIORITY_APPLICATION_MID         = 2, \
    HBL_PRIORITY_APPLICATION_HI          = 3, \
	HBL_PRIORITY_ACU_EXP_LOAD_MONITOR    = 4, \
	HBL_PRIORITY_LOADS_CONTROLS          = 5, \
	HBL_PRIORITY_DOOR_OPENING_SYSTEM     = 6, \
	HBL_PRIORITY_HEATER_FAILURES_MONITOR = 7, \
	HBL_PRIORITY_HEAT_MONITOR            = 8, \
	HBL_PRIORITY_SUPERVISOR_EXT_MONITOR  = 9, \
	HBL_PRIORITY_DIVERTER                = 10, \
	HBL_PRIORITY_HOT_HIGH_POT            = 11, \
	HBL_PRIORITY_POR_TIME_MONITOR        = 12, \
	HBL_PRIORITY_FAN_MOTOR_FAULT_MONITOR = 13, \
	HBL_PRIORITY_FILL_STUCK_ON_MONITOR   = 14, \
	HBL_PRIORITY_STANDBY_MANAGER_MONITOR = 15

/**
 * @brief HBL_LOAD_ENUM_TYPE items used in Application modules
 *
 * @details Define this macros as a list of enumeration items separated by a comma except the last one;
 * it's recommended to assign the expected value to each item.
 *
 */
#define HBL_LOAD_ID_LIST \
    HBL_LOAD_WASH_PUMP                    = 0x00, \
	HBL_LOAD_DRAIN_PUMP		              = 0x01, \
	HBL_LOAD_WASHING_HEATER               = 0x10, \
	HBL_LOAD_DRYING_HEATER                = 0x11, \
	HBL_LOAD_FILL_VALVE                   = 0x20, \
	HBL_LOAD_REGENERATION_VALVE           = 0x21, \
	HBL_LOAD_LOWER_SPRAY_ARM_MOTOR        = 0x22, \
	HBL_LOAD_WATER_TANK                   = 0x23, \
	HBL_LOAD_VENT		                  = 0x24, \
	HBL_LOAD_LOWER_TANK_VALVE             = 0x25, \
	HBL_LOAD_UPPER_TANK_VALVE             = 0x26, \
	HBL_LOAD_DISPENSER                    = 0x30, \
	HBL_LOAD_OZONO		                  = 0x31, \
	HBL_LOAD_SALT_INPUT_ACTIVATION        = 0x32, \
	HBL_POR_TIME_CAPTURE_CIRCUIT          = 0x33, \
	HBL_LOAD_MCU_ENABLE                   = 0x34, \
	HBL_LOAD_DOOR_OPENING_SYSTEM          = 0x35, \
	HBL_LOAD_RINSE_AID_INPUT_ACTIVATION   = 0x36, \
	HBL_WIN_12V_ACTIVATION                = 0x37, \
	HBL_BULK_DISPENSER_1                  = 0x38, \
	HBL_BULK_DISPENSER_2                  = 0x39, \
	HBL_BULK_DISPENSER_3                  = 0x3A, \
	HBL_NFC_ANTENNA_1                     = 0x3B, \
	HBL_NFC_ANTENNA_2                     = 0x3C, \
	HBL_LOAD_DIVERTER                     = 0x40, \
	HBL_LOAD_AUTO_KILL                    = 0x50, \
	HBL_LOAD_PILOT_RELAY                  = 0x51, \
	HBL_LOAD_RELAY_ENABLE                 = 0x52, \
	HBL_LOAD_DRY_FAN                      = 0x60, \
	HBL_LOAD_LITT				          = 0x70, \
	HBL_LOAD_LOTF                         = 0x71, \
	HBL_LOAD_OWI                          = 0x72, \
	HBL_LOAD_DUMMY_0                      = 0xF0, \
	HBL_LOAD_DUMMY_1                      = 0xF1, \
	HBL_LOAD_DUMMY_2                      = 0xF2, \
	HBL_LOAD_DUMMY_3                      = 0xF3, \
	HBL_LOAD_DUMMY_4                      = 0xF4, \
	HBL_LOAD_DUMMY_5                      = 0xF5, \
	HBL_LOAD_DUMMY_6                      = 0xF6, \
	HBL_LOAD_DUMMY_7                      = 0xF7, \
	HBL_LOAD_DUMMY_8                      = 0xF8, \
	HBL_LOAD_DUMMY_9                      = 0xF9, \
	HBL_LOAD_DUMMY_10                     = 0xFA, \
	HBL_LOAD_DUMMY_11                     = 0xFB, \
	HBL_LOAD_DUMMY_12                     = 0xFC, \
	HBL_LOAD_DUMMY_13                     = 0xFD, \
	HBL_LOAD_DUMMY_14                     = 0xFE

/**
 * @brief HBL_GI_ENUM_TYPE items used in Application modules
 *
 * @details Define this macros as a list of enumeration items separated by a comma except the last one;
 * it's recommended to assign the expected value to each item.
 *
 */
#define HBL_GI_LIST \
	HBL_GI_WASHING_PROBE                    = 0x00, \
	HBL_GI_OVERFLOW			                = 0x10, \
	HBL_GI_DOOR				                = 0x11, \
	HBL_GI_SALT				                = 0x12, \
	HBL_GI_RINSE_AID		                = 0x13, \
	HBL_GI_WAKEUP_SOURCE	                = 0x14, \
	HBL_GI_ENTRAPMENT_INPUT_POR             = 0x15, \
	HBL_GI_LIFE_TEST_MODE_SHORTCUT          = 0x16, \
	HBL_GI_FLOW_METER_DIGITAL	            = 0x17, \
	HBL_GI_DIVERTER_FEEDBACK                = 0x18, \
	HBL_CARTRIDGE_DOOR						= 0X19, \
	HBL_CARTRIDGE_SWITCH_1					= 0X1A, \
	HBL_CARTRIDGE_SWITCH_2					= 0X1B, \
	HBL_NFC_READ_TRIGGER					= 0X1C, \
	HBL_GI_WATER_COUNTER		            = 0x20, \
	HBL_GI_TURBIDITY_SENSOR_OWI             = 0x30, \
	HBL_GI_TURBIDITY_SENSOR_OWI_VRO         = 0x34, \
	HBL_GI_AC_LINE                          = 0x40, \
	HBL_GI_RTN_LINE					        = 0x41, \
	HBL_GI_V_DIAGNOSTIC				        = 0x42, \
	HBL_GI_I_DIAGNOSTIC                     = 0x48, \
	HBL_GI_I_HEATER					        = 0x49, \
	HBL_GI_I_DC_FAN					        = 0x4A, \
	HBL_GI_I_DRAIN_PUMP 				    = 0x4B, \
	HBL_GI_GENERIC_0                        = 0xF0, \
	HBL_GI_GENERIC_1                        = 0xF1, \
	HBL_GI_GENERIC_2                        = 0xF2, \
	HBL_GI_GENERIC_3                        = 0xF3, \
	HBL_GI_GENERIC_4                        = 0xF4, \
	HBL_GI_GENERIC_5                        = 0xF5, \
	HBL_GI_GENERIC_6                        = 0xF6, \
	HBL_GI_GENERIC_7                        = 0xF7, \
	HBL_GI_GENERIC_8                        = 0xF8, \
	HBL_GI_GENERIC_9                        = 0xF9, \
	HBL_GI_GENERIC_10                       = 0xFA, \
	HBL_GI_GENERIC_11                       = 0xFB, \
	HBL_GI_GENERIC_12                       = 0xFC, \
	HBL_GI_GENERIC_13                       = 0xFD, \
	HBL_GI_GENERIC_14                       = 0xFE



/** -------------------------------------------------------------------------------------------------------------------
 * @brief Enumerated list of LED_FUNC_ID_TYPE
 *
 * @details Lists all of the category possible LED_FUNC_ID types to be played by the application
 *          This list shall match the GESE Data base
 *
 *          bit 0- bit 11   - Used for LED Functions
 *          bit 12 - bit 15 - Used for Compartment ID
 */
typedef enum
{
    LED_FUNC_ID_INVALID                   = 0xFFFF,
}LED_FUNC_ID_TYPE;


/** -------------------------------------------------------------------------------------------------------------------
 * @brief Enumerated list of SOUND_FUNC_ID_TYPE
 *
 * @details Lists all of the category possible SOUND_FUNC_ID_TYPE types to be played by the application
 *          This list shall match the GESE Data base
 *
 *          bit 0- bit 11   - Used for SOUND Functions
 *          bit 12 - bit 15 - Used for Compartment ID
 */
typedef enum
{
    SOUND_FUNC_ID_INVALID                   = 0xFFFF,
}SOUND_FUNC_ID_TYPE;

/** -------------------------------------------------------------------------------------------------------------------
 * @brief Enumerated list of ANIMATION_FUNC_ID_TYPE
 *
 * @details Lists all of the category possible ANIMATION_FUNC_ID_TYPE types to be played by the application
 *          This list shall match the GESE Data base
 *
 *          bit 0- bit 11   - Used for ANIMATION Function ID
 *          bit 12 - bit 15 - Used for Compartment ID
 */
typedef enum
{
    ANIMATION_FUNC_ID_INVALID   = 0xFFFF,
}ANIMATION_FUNC_ID_TYPE;

#endif // HBLDEFS_PRM_H_



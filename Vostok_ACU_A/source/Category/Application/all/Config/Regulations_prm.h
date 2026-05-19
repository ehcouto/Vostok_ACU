/**
 *  @file
 *
 *  @brief      Public parameters for the Regulations module.
 *
 *  @details    Contains the function IDs.
 *
 *  @copyright  Copyright 2015-2021. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef REGULATIONS_PRM_H_
#define REGULATIONS_PRM_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//! Function IDs that are configurable per category
typedef enum
{
	FUNC_ID_GENERIC                 =   0,
    FUNC_ID_SELECTOR                =   1,
    FUNC_ID_START                   =   2,
	FUNC_ID_MODE					=	13,
    FUNC_ID_SELECT                  =   14,
	FUNC_ID_SELECT_2                =   15,
    FUNC_ID_CHILD_LOCK              =   16,
    FUNC_ID_ON_OFF                  =   17,
	FUNC_ID_3D_WASH					=	18,
	FUNC_ID_DELAY_HH                =   19,         // Delay hours
	FUNC_ID_DELAY_MM                =   20,         // Delay minutes
	FUNC_ID_DELAY_SS                =   21,         // Delay second
	FUNC_ID_CONFIRM					=	22,
	FUNC_ID_DEMO               		=   23,
	FUNC_ID_RAPID_ADVANCE_MODE      =   24,
	FUNC_ID_TEST_CYCLE			    =   25,
	FUNC_ID_LIFE_TEST_MODE		    =   26,
	FUNC_ID_LIGHT_IN_THE_TUB	    =   27,
	FUNC_ID_RINSE_AID			    =   28,
	FUNC_ID_SOUND				    =   29,
	FUNC_ID_LANGUAGE			    =   30,
	FUNC_ID_KOSHER_FRIENDLY			=	31,
	FUNC_ID_SILENT					=	32,
	FUNC_ID_LIGHT_ON_THE_FLOOR 		= 	33,
	FUNC_ID_ADVANCE_STEP 			=   35,
    FUNC_ID_ADVANCE_PHASE           =   36,
    FUNC_ID_SANI_RINSE              =   37,
	FUNC_ID_TIME_ON_THE_FLOOR	 	= 	38,
	FUNC_ID_MIDDLE_RACK_ONLY        =   39,
	FUNC_ID_LOWER_RACK_ONLY         =   40,
	FUNC_ID_TURBO_ZONE    	        =   41,
	FUNC_ID_BOTTLE_WASH_ZONE        =   42,
	FUNC_ID_TUBE_WASH_ZONE          =   43,
	FUNC_ID_EXTRA_DRY		        =   45,
	FUNC_ID_HEATED_DRY		        =   47,
	FUNC_ID_DETERGENT 				= 	48,
	FUNC_ID_STEAM			        =   49,
	FUNC_ID_HIGH_TEMP 		        =   50,
	FUNC_ID_HALF_LOAD 		        =   51,
	FUNC_ID_TIME_SHORTENING	        =   52,
	FUNC_ID_POWER_BOOST		        =   53,
	FUNC_ID_SETTINGMENU 			=   54,
	FUNC_ID_POWER_CLEAN		        =   57,
	FUNC_ID_DOS				        =   58,
	FUNC_ID_FAVORITE_SET 			= 	59,
	FUNC_ID_WATER_HARDNESS_LEVEL	=   60,
	FUNC_ID_LAST_CYCLE_MEMORY 		=   61,
	FUNC_ID_FAVORITE_SAVE 			= 	62,

	FUNC_ID_ESCAPE 					=   98,
	FUNC_ID_DELAY_PAUSE             =   99

    //NOTE: Max function ID is 127 due to setting file data structure limitation in function configuration

} REGULATIONS_FUNCTION_ID_TYPE;

#define REGULATION_MAX_NUM      41

#endif // REGULATIONS_PRM_H_

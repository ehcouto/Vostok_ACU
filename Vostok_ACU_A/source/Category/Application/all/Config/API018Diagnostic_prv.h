/**
 * @file
 * @brief       Private parameters for the API018Diagnostic module.
 *
 * @details     Refer to the C source file for more detailed information.
 *
 * @copyright   Copyright 2016 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef API018DIAGNOSTIC_PRV_H_
#define API018DIAGNOSTIC_PRV_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --


//=====================================================================================================================
//  --- Private Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Private Constant Declarations --

#if (API018_SERVER_FEATURE == DISABLED)
    //! The maximum number of fault error codes that the API018_CLIENT will hold.
    //! These fault error codes are transferred from the API018_SERVER at run-time.
    #define API018_MAX_STORED_FAULTS    4
#endif


// -- Private Flash-Based Constant Definitions --

#if (API018_SERVER_FEATURE == ENABLED)
    //! Array that maps cross-platform fault keys to platform-specific fault IDs.
    //! If new cross-platform fault keys are required, they must be added to API018Diagnostic.h.
    //! Only the cross-platform fault keys used by the platform have to be included in this table.
    static const API018_FAULT_KEY_ID_PAIR_TYPE MAP_FAULT_KEYS_TO_FAULT_IDS[] =
    {
    //  {API018_FAULT_KEY_TYPE,                           Node Number,  FAULT_ID_TYPE},

        // This entry should be deleted once there are platform-specific entries defined.
        // It is placed here to avoid a compiler error because of an empty array.
		//{FAULT_KEY_NONE,						      WIN_ACU_ADDRESS, FAULT_ID_NONE},
		{FAULT_KEY_DOOR_SWITCH_STUCK_CLOSED,          WIN_UI_ADDRESS,  FAULT_ID_DOOR_SWITCH_STUCK_CLOSED},
		{FAULT_KEY_DOOR_SWITCH_STUCK_OPEN,		      WIN_UI_ADDRESS,  FAULT_ID_DOOR_SWITCH_STUCK_OPEN},
		{FAULT_KEY_STUCK_KEY,		      			  WIN_UI_ADDRESS,  FAULT_ID_KEY_STUCK_CLOSED},
		{FAULT_KEY_MODEL_NUMBER_MISMATCH,     	      WIN_UI_ADDRESS,  FAULT_ID_MODEL_NUMBER_MISMATCH},
		{FAULT_KEY_MODEL_NUMBER_MISSING,		      WIN_UI_ADDRESS,  FAULT_ID_MODEL_NUMBER_MISSING},
		{FAULT_KEY_REGULATION_REJECTED,		      	  WIN_UI_ADDRESS,  FAULT_ID_REGULATION_REJECTED},
    };
#endif      // #if (API018_SERVER_FEATURE == ENABLED)


//=====================================================================================================================
//  --- Private Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


#endif      // API018DIAGNOSTIC_PRV_H_

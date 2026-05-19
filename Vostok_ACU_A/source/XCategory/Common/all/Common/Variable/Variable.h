/**
 *  @file       Variable.h
 *
 *  @brief      Module that stores references and definitions to the meaningful variables on the platform.
 *
 *  @section    Applicable_Documents
 *				Link- ccm.whirlpool.oncloudone.com/ccm/web/projects/Nucleus%20%28Change%20Management%29#action=com.ibm.team.workitem.viewWorkItem&id=688&tab=com.ibm.team.workitem.tab.links. <tr>
 *
 *  $Header: Variable.h 1.10 2015/07/30 09:49:09EDT VALMOR ADAMI JUNIOR (ADAMIV) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/30 09:49:09EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef VARIABLE_H_
#define VARIABLE_H_

#include "SystemConfig.h"
#include "Variable_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

/*Pre-processing #defines to assign the default values.*/
/*Default for this one is ENABLED.*/
#ifndef VARIABLES_REGULATIONS_FEATURE
    #define VARIABLES_REGULATIONS_FEATURE  ENABLED
#endif
/* Default value for this #define is DISABLED.*/
#ifndef VARIABLES_HAVE_KVP_ID
	#define VARIABLES_HAVE_KVP_ID	DISABLED
#endif

#ifndef HMI_VARIABLE_FEATURE
    #define HMI_VARIABLE_FEATURE    ENABLED
#endif

#if(HMI_VARIABLE_FEATURE == ENABLED)
    #ifndef HMI_VARIABLE_ID_USERS_ENUM
        // HMI_VARIABLE_ID_USERS_ENUM should be defined to use the HMI variable feature.
        #error HMI_VARIABLE_ID_USERS_ENUM must be defined in Variable_prm.h
    #endif
#endif

// Extern the array of sizes for each data type defined on C_Extensions.
extern uint8 const VARIABLE_SIZE[];

/**
 * Defining the Indexes to access the variables on the ACU_DATA_HOLDER[] array.
 * User can add Indexes here by using a macro from the PRM file.
 */
typedef enum VARIABLE_ID_ENUM
{
	 VARIABLE_REGULATION_HUB             = 0
	,VARIABLE_CYCLE                      = 1
	,VARIABLE_PHASE                      = 2
	,VARIABLE_STEP                       = 3
	,VARIABLE_STEP_SECONDS               = 4
	,VARIABLE_SUB_CYCLE                  = 5
	,VARIABLE_SUB_PHASE                  = 6
	,VARIABLE_SUB_STEP                   = 7
	,VARIABLE_SECONDS_SINCE_RESET        = 8
	,VARIABLE_CYCLES_COMPLETED           = 9
	,VARIABLE_ALWAYS_TRUE                = 10
	,VARIABLE_USER_WORD                  = 11
	,VARIABLE_USER_FLAGS                 = 12
	,VARIABLE_PREFAULT                   = 13
	,VARIABLE_FAULT                      = 14
	,VARIABLE_SUPERVISOR_STATE           = 15
#ifdef ACU_VARIABLE_ID_USERS_ENUM
	//! This is the macro users can fill up with their variable IDs.
	,ACU_VARIABLE_ID_USERS_ENUM
#endif
	,VARIABLE_NUM_ACU_VARIABLES
	//!Indexes for VISUAL MACRO VARIABLES used in HMI only Variables
    ,VARIABLE_HMI_TIME_ESTIMATION        = 252
    ,VARIABLE_HMI_GENERAL_PURPOSE        = 253
    ,VARIABLE_HMI_TIMERS                 = 254
    //! Invalid Index

    ,VARIABLE_INVALID_INDEX              = 255
    //!Indexes for HMI only Variables
#ifdef HMI_VARIABLE_ID_USERS_ENUM
    //! This is the macro users can fill up with their variable IDs.
    ,HMI_VARIABLE_ID_USERS_ENUM
#endif
    ,VARIABLE_END_HMI_VARIABLES
    ,VARIABLE_HMI_LIMIT                  = 512

} VARIABLE_ID_TYPE;


typedef uint8 ACU_VARIABLE_ID_TYPE;     //! Follow Story 1259741 for detail information

/**
 * Define the first VARIABLE index reserved to HMI only variables.
 * Adding HMI variables from bottom to top, so first added is at INDEX 254, since 255 is invalid.
 */
#define VARIABLE_HMI_RESERVED_INDEX  (252)
#define VARIABLE_HMI_START_INDEX     (256U)

#ifdef HMI_VARIABLE_ID_USERS_ENUM
    //! VARIABLE_NUM_HMI_VARIABLES will be non zero when HMI_VARIABLE_ID_USERS_ENUM is defined
    #define VARIABLE_NUM_HMI_VARIABLES   (uint8)((uint16)VARIABLE_END_HMI_VARIABLES - (uint16)VARIABLE_HMI_START_INDEX)
#else
    #define VARIABLE_NUM_HMI_VARIABLES      0U
#endif


/**
 * Structure that defines how this module manipulates the Variables. Getters will return
 * information using this structure.
 */
PACKED typedef struct VARIABLE_STRUCT
{
	//! Address for the Variable on the memory. You need to cast to the proper type to use it.
	void * Address;
	//! Length is the number of elements on an Array. If your variable is a single variable, the value is 1.
	uint8 Length;
	//! Type of the variable DATA_DEFINITION_TYPE.
	DATATYPE_TYPE Type;
	//! Indicates we can write to the variable.
	BOOL_TYPE Is_Writable;
}VARIABLE_TYPE;

#if (VARIABLES_HAVE_KVP_ID == ENABLED)
	/**
	 * This is the data type that defines the translation table from KVP_ID to Variable Index and Offset.
	 */
	PACKED typedef struct KVP_TO_VARIABLE_TRANSLATION_STRUCT
	{
		//! Kvp_Id is the 32 bits indexer for any data on the connectivity domain.
		uint32 Kvp_Id;
		//! Variable Index as defined on the VARIABLE_TYPE and configured on the VARIABLE_HOLDER_INITIALIZE.
		VARIABLE_ID_TYPE Variable_Index;
		//! Offset to a specific address on the Variable Index domain.
		uint8 Offset;
	}KVP_TO_VARIABLE_TRANSLATION_TYPE;
#endif
/**
 * Operations to be used for function to change the value for the variables.
 */
typedef enum VARIABLE_SET_OPERATION_ENUM
{
	VARIABLE_OPERATION_SET 		= 0,   		//!< VARIABLE_OPERATION_SET
	VARIABLE_OPERATION_ADD			,       //!< VARIABLE_OPERATION_ADD
	VARIABLE_OPERATION_SUBTRACT		,  		//!< VARIABLE_OPERATION_SUBTRACT
	VARIABLE_OPERATION_INCREMET		,  		//!< VARIABLE_OPERATION_INCREMET
	VARIABLE_OPERATION_DECREMENT	, 		//!< VARIABLE_OPERATION_DECREMENT
	VARIABLE_OPERATION_BIT_SET		,   	//!< VARIABLE_OPERATION_BIT_SET
	VARIABLE_OPERATION_BIT_CLEAR	, 		//!< VARIABLE_OPERATION_BIT_CLEAR
    VARIABLE_OPERATION_MULTIPLY     ,       //!< VARIABLE_OPERATION_MULTIPLY
    VARIABLE_OPERATION_DIVIDE       ,       //!< VARIABLE_OPERATION_DIVIDE
	VARIABLE_OPERATION_COUNT      			//!< VARIABLE_OPERATION_COUNT
}VARIABLE_SET_OPERATION_TYPE;

//! System State definition.
//! bits 6-0: define the system state value
//! bit 7: reserved for the Not_Ok_To_Start_Bit,
//!     1 = cycle cannot start
//!     0 = cycle can start
#define SYSTEM_STATE_NOT_OK_TO_START_BIT    (7u)
typedef enum SYSTEM_STATE_ENUM
{
    // bits 6-0 are reserved for the system state value
    SYSTEM_STATE_NONE           = 0u,
    SYSTEM_STATE_PROGRAMMING,   // 1
    SYSTEM_STATE_RUN,           // 2
    SYSTEM_STATE_DELAY,         // 3
    SYSTEM_STATE_END_OF_CYCLE,  // 4
    SYSTEM_STATE_PAUSE,         // 5
    SYSTEM_STATE_FAULT,         // 6
    SYSTEM_STATE_DELAY_PAUSE,   // 7
    SYSTEM_STATE_CANCELLING,    // 8

    SYSTEM_STATE_PROGRAMMING_NOT_OK_TO_START    = (1u << SYSTEM_STATE_NOT_OK_TO_START_BIT) + (uint8)SYSTEM_STATE_PROGRAMMING,  // 0x81
    SYSTEM_STATE_DELAY_NOT_OK_TO_START          = (1u << SYSTEM_STATE_NOT_OK_TO_START_BIT) + (uint8)SYSTEM_STATE_DELAY,        // 0x83
    SYSTEM_STATE_PAUSE_NOT_OK_TO_START          = (1u << SYSTEM_STATE_NOT_OK_TO_START_BIT) + (uint8)SYSTEM_STATE_PAUSE,        // 0x85
    SYSTEM_STATE_DELAY_PAUSE_NOT_OK_TO_START    = (1u << SYSTEM_STATE_NOT_OK_TO_START_BIT) + (uint8)SYSTEM_STATE_DELAY_PAUSE   // 0x87
} SYSTEM_STATE_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
BOOL_TYPE     Variable__GetByIndex			(VARIABLE_ID_TYPE index, VARIABLE_TYPE *to_fill_up);
BOOL_TYPE     Variable__GetValue			(VARIABLE_ID_TYPE index, uint8 offset, sint32 *value);
BOOL_TYPE     Variable__GetTranslatedValue	(VARIABLE_ID_TYPE index, uint8 offset, sint32 *value);
DATATYPE_TYPE Variable__GetTypeByIndex  	(VARIABLE_ID_TYPE index);

BOOL_TYPE	  Variable__SetValue			(VARIABLE_SET_OPERATION_TYPE operation, VARIABLE_ID_TYPE index, uint8 offset, sint32 value);

#if(VARIABLES_HAVE_KVP_ID == ENABLED)
	// Functions related to the KVP_ID feature.
	BOOL_TYPE     Variable__GetValuebyKVP			(uint32 kvp_id, sint32 *value);
	DATATYPE_TYPE Variable__GetTypeByKVP  			(uint32 kvp_id);
	BOOL_TYPE	  Variable__SetValueByKVP			(VARIABLE_SET_OPERATION_TYPE operation, uint32 kvp_id, sint32 value);
	uint32 		  Variable__GetKVPByIndex			(VARIABLE_ID_TYPE variable_index);
#endif


#endif      // VARIABLE_H_



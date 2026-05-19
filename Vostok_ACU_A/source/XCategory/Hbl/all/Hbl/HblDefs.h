/**
 *  @file
 *
 *  @brief      Hbl standard definitions
 *
 *  @defgroup CLASS_B
 *
 *  @copyright  Copyright 2015. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

// test

#ifndef HBLDEFS_H_
#define HBLDEFS_H_

#include "SRHblDefs_prm.h"
#include "SettingFile.h"

#include "HblDefs_def.h"
#include "HblDefs_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

/*
 * HBL LOAD / PILOT PRIORITY ENUM
 *
 * All priorities handled by code should be present here; please take care of these common guidelines:
 * - priority name should begin with HBL_PRIORITY_; for Class B you can use SRHBL_PRIORITY_CLASSB_
 * - SRHBL_PRIORITY_CLASSB_LO must be defined in SRHblDefs_prm.h: higher values will not be allowed to be used in non Safety Application
 * - HBL_PRIORITY_PC_CONTROL will be defined as the maximum allowed non Safety Application (SRHBL_PRIORITY_CLASSB_LO-1)
 *
 * Dedicated list of Safety Relevant priority must be defined in SRHblDefs_prm.h through SRHBL_LOAD_PRIORITY_LIST macro
 * Dedicated list of non Safety Relevant priority must be defined in HblDefs_prm.h through HBL_LOAD_PRIORITY_LIST macro
 *
 */
typedef enum
{
    SRHBL_PRIORITY_MINIMUM   = 0,
    SRHBL_PRIORITY_MAXIMUM   = 255,
    // Safety Priority references must be defined in SRHblDefs_def.h, if required
    #ifdef SRHBL_LOAD_PRIORITY_LIST
        SRHBL_LOAD_PRIORITY_LIST,
    #endif
    HBL_PRIORITY_PC_CONTROL  = ( SRHBL_PRIORITY_CLASSB_LO - 1),
    // Non Safety Priority references must be defined in HblDefs_def.h, if required
    #ifdef HBL_LOAD_PRIORITY_LIST
        HBL_LOAD_PRIORITY_LIST
    #endif
} HBL_PRIORITY_TYPE;

/**
 * HBL PILOT ENUM
 */
typedef enum
{
    HBL_PILOT_SR_DIGITAL                    = 128,
    HBL_PILOT_SR_DOORLOCK_EM                = 141,
    HBL_PILOT_SR_DOORLOCK_WAX               = 143,
    HBL_PILOT_SR_DIGITAL_OUTPUT_MULTI_2_PIN = 157,
    HBL_PILOT_SR_CHARGEPUMP                 = 180,
    HBL_PILOT_INVALID                       = 255,
    // non Safety Pilot references must be defined in HblDefs_def.h
    HBL_PILOT_LIST
}HBL_PILOT_ENUM_TYPE;


/**
 * HBL LOAD ID References
 */
typedef enum
{
    HBL_LOAD_INVALID                        = 0xFF,
    // Safety Load IDreferences must be defined in SRHblDefs_def.h, if required
    #ifdef SRHBL_LOAD_ID_LIST
        SRHBL_LOAD_ID_LIST,
    #endif
    // Non Safety Load IDreferences must be defined in HblDefs_def.h, if required
    #ifdef HBL_LOAD_ID_LIST
        HBL_LOAD_ID_LIST
    #endif
} HBL_LOAD_ENUM_TYPE;

/*
 * HBL Index references
 */
#define HBL_LOAD_INDEX_DIRECT               0xFEU
#define HBL_LOAD_INDEX_INVALID              0xFFU

#define HBL_INDEX_INVALID                   0xFFU
#define HBL_ALLOCATION_INVALID              0xFFU

#define HBL_ENCODER_UNKNOWN_POSITION        0xFFU

/*
 * HBL_LOAD_STATUS_TYPE is the status
 */
typedef enum
{
	HBL_LOAD_STATUS_TARGET_COMPLETED        = 0,    // Load has reached the Processed position
	HBL_LOAD_STATUS_TARGET_ALT_TABLE        = 1,    // Load has reached the Processed position; for consistency use alternate table if not OFF (e.g. partialized triac set to a low angle)
	HBL_LOAD_STATUS_WAITING_TARGET          = 2,    // Load has not yet reached Processed position
	HBL_LOAD_STATUS_FAILURE                 = 3,    // Load is unable to reach Processed position
	HBL_LOAD_STATUS_INITIALIZE              = 15,    // Load initialize in progress
} HBL_LOAD_STATUS_ENUM_TYPE;

/*
 * HBL_CONSISTENCY_TABLE_TYPE refers to consistency table to correlate
 * feedback value and identify consistency result.
 * This table is related to Load Status.
 */
typedef enum
{
	HBL_CONSISTENCY_TABLE_OFF               = 0,
	HBL_CONSISTENCY_TABLE_ON                = 1,
	HBL_CONSISTENCY_TABLE_WAITING           = 2,
	HBL_CONSISTENCY_TABLE_ON_ALTERNATE      = 3,
	HBL_CONSISTENCY_TABLE_INVALID           = 15,
} HBL_CONSISTENCY_TABLE_ENUM_TYPE;

/*
 * HBL_CONSISTENCY_VALUE_TYPE refers to consistency values that can be
 * used in GESE Load Feedback configuration.
 */
typedef enum
{
	HBL_CONSISTENCY_OK                      = 0,
	HBL_CONSISTENCY_GENERIC_ERROR           = 1,
	HBL_CONSISTENCY_FEEDBACK_INVALID        = 2,
	HBL_CONSISTENCY_SHORT_CIRCUIT           = 3,
	HBL_CONSISTENCY_OPEN_CIRCUIT            = 4,
	HBL_CONSISTENCY_DISCONNECTED            = 5,
	HBL_CONSISTENCY_DIODE_MODE              = 6,
	HBL_CONSISTENCY_CUSTOM_1                = 7,
	HBL_CONSISTENCY_CUSTOM_2                = 8,
	HBL_CONSISTENCY_CUSTOM_3                = 9,
	HBL_CONSISTENCY_CUSTOM_4                = 10,
	HBL_CONSISTENCY_CUSTOM_5                = 11,
	HBL_CONSISTENCY_CUSTOM_6                = 12,
	HBL_CONSISTENCY_CUSTOM_7                = 13,
	HBL_CONSISTENCY_CUSTOM_8                = 14,
	HBL_CONSISTENCY_INVALID_VALUE           = 15,
} HBL_CONSISTENCY_VALUE_ENUM_TYPE;

/*
 * HBL_LOAD_PROFILE_TYPE is the structure to use on Loads that can be treated as a profile:
 * Reg is regulation value; values are 0 to 15 depending on load
 *     - 0 must be considered as OFF
 *     - 1 must be considered as ON; for a multi-level profile it should be used for maximum level
 * Config can be used to handle some variations that can be applied to the Reg value; some examples may be
 *     - set ON only one pin in a Pilot that handles 2 pins
 *     - set the target profile Reg without any transitions
 */
typedef union
{
	uint8 Byte;
	struct
	{
		uint8 Reg:4;
		uint8 Config:4;
	} Load;
} HBL_LOAD_PROFILE_TYPE;

/*
 * HBL_LOAD_STATUS_TYPE is the structure for Load status:
 * Cur_Reg is regulation value; values are 0 to 15 depending on load
 *     - 0 must be considered as OFF
 *     - 1 must be considered as ON; for a multi-level profile it should be used for maximum level
 * Status is the Load status according to HBL_LOAD_STATUS_ENUM_TYPE
 */
typedef union HBL_LOAD_STATUS
{
	uint8 Byte;
	struct
	{
		uint8 Cur_Reg:4;
		HBL_LOAD_STATUS_ENUM_TYPE Status:4;
	} Load;
} HBL_LOAD_STATUS_TYPE;

/*
 * HBL_CONSISTENCY_TYPE is the structure for Load status:
 * Cur_Reg is regulation value; values are 0 to 15 depending on load
 *     - 0 must be considered as OFF
 *     - 1 must be considered as ON; for a multi-level profile it should be used for maximum level
 * Table is the Consistency Table to use as in HBL_CONSISTENCY_TABLE_ENUM_TYPE
 */
typedef union HBL_CONSISTENCY
{
	uint8 Byte;
	struct
	{
		HBL_CONSISTENCY_VALUE_ENUM_TYPE Value:4;
		HBL_CONSISTENCY_TABLE_ENUM_TYPE Table:4;
	} Consistency;
} HBL_CONSISTENCY_TYPE;

/**
 * HBL Load Pilot Config Struct
 */
PACKED typedef struct
{
    uint8 * Pin_Ptr;
    SETTINGFILE_LOADER_TYPE Parameter_Ptr;
    SETTINGFILE_LOADER_TYPE Load_Parameter_Ptr;

    uint8 Pilot_Data_Index;
    uint8 Load_Data_Index;

    HBL_LOAD_ENUM_TYPE Load_Id;     // Note: The order of Load_Id to Load_Type_Params_Offset should not
    uint8 Feedback_Params_Offset;   // be modified because it matches the structure in the setting file
    HBL_PILOT_ENUM_TYPE Pilot_Type;
    uint8 Pin_Table_Offset;
    uint8 Pilot_Type_Params_Offset;
    uint8 Priority_Bitmap;
    uint8 Max_Regulation :4;
    uint8 Hi_Low_Activation_Bitmap :4;
    uint8 Load_Type_Params_Offset;

    uint8 Pilot_Handler_Index;
    uint8 Load_Handler_Index;
}HBL_LOAD_TYPE;

typedef void (* const HBL__PILOTINITIALIZE)(void);
typedef uint8 (* const HBL__PILOTALLOCATE)(void);
typedef PASS_FAIL_TYPE (* const HBL__PILOTINITIALIZEINSTANCE)(HBL_LOAD_TYPE * load);
typedef PASS_FAIL_TYPE (* const HBL__PILOTREQUEST)(HBL_LOAD_TYPE * load, void * data);
typedef PASS_FAIL_TYPE (* const HBL__PILOTPROCESS)(HBL_LOAD_TYPE * load);

typedef void * (* const HBL__PILOTGET)(HBL_LOAD_TYPE * load);

typedef PASS_FAIL_TYPE (* const HBL__PILOTZCPROCESS)(HBL_LOAD_TYPE * load);
typedef PASS_FAIL_TYPE (* const HBL__PILOTASYNCPROCESS)(HBL_LOAD_TYPE * load);
typedef PASS_FAIL_TYPE (* const HBL__PILOTOCPROCESS)(HBL_LOAD_TYPE * load);
typedef PASS_FAIL_TYPE (* const HBL__PILOT1MS)(HBL_LOAD_TYPE * load);
typedef void * (* const HBL__PILOTGETSTATUS)(HBL_LOAD_TYPE * load);

/**
 * HBL Pilot Hook Struct
 */
PACKED typedef struct
{
    HBL__PILOTINITIALIZE Initialize;					//!< (Mandatory) Pilot Handler Initialize method
    HBL__PILOTALLOCATE Allocate;						//!< (Mandatory) Try to allocate space for a new instance of that Pilot
    HBL__PILOTINITIALIZEINSTANCE InitializeInstance;	//!< (Mandatory) Pilot Handler instance Initialize method: this will configure I/O and Pilot data
    HBL__PILOTREQUEST Request;							//!< (Mandatory) Save a new Request set point for that Pilot
    HBL__PILOTPROCESS Process;							//!< (Mandatory) Process new Pilot set point
    HBL__PILOTGET GetRequested;							//!< (Mandatory) Provides current Requested set point
    HBL__PILOTGET GetProcessed;							//!< (Mandatory) Provides current Processed set point
    HBL__PILOTZCPROCESS ZcProcess;						//!< (Optional) Handler called to perform activities every Zerocross event
    HBL__PILOTASYNCPROCESS AsynchProcess;				//!< (Optional) Handler called to perform activities into Asynch Task (every 5ms)
    HBL__PILOTOCPROCESS OCProcess;						//!< (Optional) Handler called to perform activities on an Output Compare event
    HBL__PILOT1MS OneMsHandler;							//!< (Optional) Handler called to perform activities in System tick (every 1ms)
    HBL__PILOTGETSTATUS GetStatus;						//!< (Mandatory) Provides current Pilot status
    uint8 Parameter_Size;								//!< Pilot Requested/Processed set point parameters size
    HBL_PILOT_ENUM_TYPE Pilot;							//!< Pilot Type ID
}HBL_PILOT_HANDLER_HOOK_TYPE;



typedef void (* const HBL__LOADINITIALIZE)(void);
typedef uint8 (* const HBL__LOADALLOCATE)(void);
typedef PASS_FAIL_TYPE (* const HBL__LOADINITIALIZEINSTANCE)(HBL_LOAD_TYPE * load);
typedef PASS_FAIL_TYPE (* const HBL__LOADREQUEST)(HBL_LOAD_TYPE * load, void * data);
typedef PASS_FAIL_TYPE (* const HBL__LOADPROCESS)(HBL_LOAD_TYPE * load, HBL_PILOT_HANDLER_HOOK_TYPE * pilot);
typedef void * (* const HBL__LOADGETREQUESTED)(HBL_LOAD_TYPE * load);
typedef void * (* const HBL__LOADGETPROCESSED)(HBL_LOAD_TYPE * load, HBL_PILOT_HANDLER_HOOK_TYPE * pilot);
typedef PASS_FAIL_TYPE (* const HBL__LOADASYNCPROCESS)(HBL_LOAD_TYPE * load,HBL_PILOT_HANDLER_HOOK_TYPE * pilot);
typedef void * (* const HBL__LOADGETSTATUS)(HBL_LOAD_TYPE * load, HBL_PILOT_HANDLER_HOOK_TYPE * pilot);

/**
 * HBL Load Hook Struct
 */
PACKED typedef struct
{
    HBL__LOADINITIALIZE Initialize;						//!< (Mandatory) Load Handler Initialize method
    HBL__LOADALLOCATE Allocate;							//!< (Mandatory) Try to allocate space for a new instance of that Load
    HBL__LOADINITIALIZEINSTANCE InitializeInstance;		//!< (Mandatory) Load Handler instance Initialize method: this will configure Load data
    HBL__LOADREQUEST Request;							//!< (Mandatory) Save a new Request set point for that Load
    HBL__LOADPROCESS Process;							//!< (Mandatory) Process new Pilot set point
    HBL__LOADGETREQUESTED GetRequested;					//!< (Mandatory) Provide current Requested set point
    HBL__LOADGETPROCESSED GetProcessed;					//!< (Mandatory) Provide current Processed set point
    HBL__LOADASYNCPROCESS AsynchProcess;				//!< (Optional) Handler called to perform activities into Asynch Task (every 5ms)
    HBL__LOADGETSTATUS GetStatus;						//!< (Mandatory) Provides current Load status
    uint8 Parameter_Size;								//!< Load Requested/Processed set point parameters size
    HBL_LOAD_ENUM_TYPE Load_ID;							//!< Load ID
}HBL_LOAD_HANDLER_HOOK_TYPE;

//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------

/**
 * HBL LLI Enum
 */
typedef enum
{
    HBL_LLI_SRTOUCH_KEY            = 26,
    HBL_LLI_SRDIGITAL              = 27,
    HBL_LLI_SRANALOG               = 129,
    HBL_LLI_SRFEEDBACK             = 130,
    HBL_LLI_SRAC_INPUT             = 133,
    HBL_LLI_SRTRIAC_FEEDBACK       = 135,
    HBL_LLI_SRINPUTCAPTURE         = 137,
    HBL_LLI_SRZEROCROSS            = 138,
    HBL_LLI_SRPEAK_ANALOG          = 141,
    HBL_LLI_SRCHARGEPUMP_FEEDBACK  = 153,
    HBL_LLI_SRI2C_PRESSURE_SENSOR  = 154,
    HBL_LLI_INVALID                = 255,
    // non Safety LLI (Low Level Input) references must be defined in HblDefs_def.h
    HBL_LLI_LIST
}HBL_LLI_ENUM_TYPE;

/**
 * HBL LLI Config Struct
 */
PACKED typedef struct
{
    uint8 * Pin_Ptr;
    SETTINGFILE_LOADER_TYPE Parameter_Ptr;
    uint8 Data_Index;
    HBL_LLI_ENUM_TYPE LLI_ID;
    uint8 LLI_Position  : 7;
    uint8 LLI_Mux       : 1;
    uint8 LLI_Handler_Index;
}HBL_LLI_TYPE;


typedef void (* const HBL__LLIINITIALIZE)(void);
typedef uint8 (* const HBL__LLIALLOCATE)(void);
// PASS if InitializeInstance completed successfully, FAIL otherwise
typedef PASS_FAIL_TYPE (* const HBL__LLIINITIALIZEINSTANCE)(HBL_LLI_TYPE * input);
typedef void * (* const HBL__LLIGET)(HBL_LLI_TYPE * input);
// TRUE if a new sample is ready, FALSE otherwise
typedef BOOL_TYPE (* const HBL__LLIASYNCHPROCESS)(HBL_LLI_TYPE * input);
typedef BOOL_TYPE (* const HBL__LLIPEAKPROCESS)(HBL_LLI_TYPE * input,BOOL_TYPE peak_polarity);
typedef BOOL_TYPE (* const HBL__LLIICPROCESS)(HBL_LLI_TYPE * input);
typedef BOOL_TYPE (* const HBL__LLI1MSPROCESS)(HBL_LLI_TYPE * input);

/**
 * ACU LLI HOOK STRUCTURE
 */
PACKED typedef struct
{
    HBL__LLIINITIALIZE Initialize;						//!< (Mandatory) Low Level Input Handler Initialize method
    HBL__LLIALLOCATE Allocate;							//!< (Mandatory) Try to allocate space for a new instance of that Low Level Input
    HBL__LLIINITIALIZEINSTANCE InitializeInstance;		//!< (Mandatory) Load Handler instance Initialize method: this will configure Low Level Input data
    HBL__LLIGET Get;									//!< (Mandatory) Provides pointer to current Low Level Input value
    HBL__LLIASYNCHPROCESS AsynchProcess;				//!< (Optional) Handler called to perform activities into Asynch Task (every 5ms)
    HBL__LLIPEAKPROCESS PeakProcess;					//!< (Optional) Handler called to perform activities every Zerocross Peak event
    HBL__LLIICPROCESS ICProcess;						//!< (Optional) Handler called to perform activities on an Input Capture event
    HBL__LLI1MSPROCESS OneMSHandler;					//!< (Optional) Handler called to perform activities in System tick (every 1ms)
    uint8 Parameter_Size;								//!< Low Level Input data parameters size
    HBL_LLI_ENUM_TYPE LLI_ID;							//!< Low Level ID
}HBL_LLI_HANDLER_HOOK_TYPE;

//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------

/**
 * HBL GENERIC INPUT ENUM
 */
typedef enum
{
    HBL_GI_INVALID                          = 0xFF,
    // Safety Generic Input references must be defined in SRHblDefs_def.h, if required
    #ifdef SRHBL_GI_LIST
        SRHBL_GI_LIST,
    #endif
    // Non Safety Generic Input references must be defined in SRHblDefs_def.h, if required
    #ifdef HBL_GI_LIST
        HBL_GI_LIST
    #endif
} HBL_GENERIC_INPUT_ENUM_TYPE;

/**
 * HBL ACU GI Config Struct
 */
PACKED typedef struct
{
    HBL_LLI_ENUM_TYPE LLI_ID;
    HBL_GENERIC_INPUT_ENUM_TYPE GI_ID;
    uint8 GI_Param_Offset;
    uint8 LLI_Config_Index;
    uint8 GI_Handler_Index;
    uint8 Parameter_Size;
    uint8 LLI_Parameter_Size;
    uint8 Data_Index;
}HBL_GI_TYPE;


typedef void (* const HBL__GIINITIALIZE)(void);
typedef uint8 (* const HBL__GIALLOCATE)(void);
// PASS if InitializeInstance completed successfully, FAIL otherwise
typedef PASS_FAIL_TYPE (* const HBL__GIINITIALIZEINSTANCE)(HBL_GI_TYPE * generic_input, uint8 lli_sequence);
typedef void * (* const HBL__GIGET)(HBL_GI_TYPE * generic_input,void * lli_data);
// TRUE if a new sample is ready, FALSE otherwise
typedef BOOL_TYPE (* const HBL__GIASYNCHPROCESS)(HBL_GI_TYPE * generic_input,void * lli_data, uint8 lli_sequence);
typedef sint32 (* const HBL__GETCONVERTED)(HBL_GI_TYPE * generic_input,void * lli_data);

/**
 * ACU GI HOOK STRUCTURE
 */
PACKED typedef struct
{
    HBL__GIINITIALIZE Initialize;						//!< (Mandatory) Generic Input Handler Initialize method
    HBL__GIALLOCATE Allocate;							//!< (Mandatory) Try to allocate space for a new instance of that Generic Input
    HBL__GIINITIALIZEINSTANCE InitializeInstance;		//!< (Mandatory) Load Handler instance Initialize method: this will configure Generic Input data
    HBL__GIGET Get;										//!< (Mandatory) Provides pointer to current Generic Input data value
    HBL__GIASYNCHPROCESS AsynchProcess;					//!< (Optional) Handler called to perform activities into Asynch Task (every 5ms)
    HBL__GETCONVERTED GetConverted;						//!< (Mandatory) Provides converted value of Generic Input data value
    uint8 Parameter_Size;								//!< Generic Input data parameters size
    HBL_GENERIC_INPUT_ENUM_TYPE GI_ID;					//!< Generic Input ID
}HBL_GI_HANDLER_HOOK_TYPE;



//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------------------------
/**
 * HBL HMI GI Enum
 */
typedef enum
{
    HBLHMI_GI_SRKEY                     = 0x09, //!< HBLHMI_GI_SRKEY
    HBLHMI_GI_ANALOG_SRPOTENTIOMETER    = 0x0A, //!< HBLHMI_GI_ANALOG_SRPOTENTIOMETER
    HBLHMI_GI_DISCRETE_SRPOTENTIOMETER  = 0x0B, //!< HBLHMI_GI_DISCRETE_SRPOTENTIOMETER
    HBLHMI_GI_INVALID                   = 0xFF, //!< HBLHMI_GI_INVALID
    HBLHMI_GI_LIST
}HBLHMI_GI_ENUM_TYPE;

/**
 * GI Reading Type for HMI
 */
typedef enum
{
    HBLHMI_GI_READ_DIGITAL = 0,//!< HBLHMI_GI_READ_DIGITAL
    HBLHMI_GI_READ_ANALOG  = 1,//!< HBLHMI_GI_READ_ANALOG
    HBLHMI_GI_READ_ENCODER = 2 //!< HBLHMI_GI_READ_ENCODER
}HBLHMI_GI_READ_TYPE;

/**
 * HBL GI Config struct
 */
PACKED typedef struct
{
    HBLHMI_GI_ENUM_TYPE GI_ID;
    HBL_LLI_ENUM_TYPE LLI_ID;
    uint8 GI_Position : 7;
    uint8 Unused_1    : 1;
    HBLHMI_GI_READ_TYPE GI_Read_Type : 2;
    SETTINGFILE_LOADER_TYPE Parameter_Ptr;      // Points to the device parameters for the GI
    uint8 LLI_position : 7;
    uint8 Unused_2     : 1;
    uint8 LLI_Config_Index;
    uint8 GI_Handler_Index;
    uint8 Parameter_Size;
    uint8 Data_Index;
}HBLHMI_GI_TYPE;


typedef void (* const HBLHMI__GIINITIALIZE)(void);
typedef uint8 (* const HBLHMI__GIALLOCATE)(void);
// PASS if initialize completed successfully, FAIL otherwise
typedef PASS_FAIL_TYPE (* const HBLHMI__GIINITIALIZEINSTANCE)(HBLHMI_GI_TYPE * generic_input, uint8 lli_sequence);
typedef void * (* const HBLHMI__GIGET)(HBLHMI_GI_TYPE * generic_input, void * lli_data);
typedef void (* const HBLHMI__SET)(HBLHMI_GI_TYPE * generic_input, uint8 desired_state);
// TRUE if a new sample is ready, FALSE otherwise
typedef BOOL_TYPE (* const HBLHMI__GIASYNCHPROCESS)(HBLHMI_GI_TYPE * generic_input, void * lli_data, uint8 lli_sequence);
typedef sint32 (* const HBLHMI__GETCONVERTED)(HBLHMI_GI_TYPE * generic_input, void * lli_data);

/**
 * UI GIHOOK  STRUCTURE
 */
PACKED typedef struct
{
    HBLHMI__GIINITIALIZE Initialize;					//!< (Mandatory) Generic Input Handler Initialize method
    HBLHMI__GIALLOCATE Allocate;						//!< (Mandatory) Try to allocate space for a new instance of that Generic Input
    HBLHMI__GIINITIALIZEINSTANCE InitializeInstance;	//!< (Mandatory) Load Handler instance Initialize method: this will configure Generic Input data
    HBLHMI__GIGET Get;									//!< (Mandatory) Provides pointer to current Generic Input data value
    HBLHMI__SET Set;									//!< (Mandatory) Set new Generic Input data value
    HBLHMI__GIASYNCHPROCESS AsynchProcess;				//!< (Optional) Handler called to perform activities into Asynch Task (every 5ms)
    HBLHMI__GETCONVERTED GetConverted;					//!< (Deprecated) Not used
    uint8 Parameter_Size;								//!< Generic Input data parameters size
    HBLHMI_GI_ENUM_TYPE GI_ID;							//!< Generic Input ID
}HBLHMI_GI_HANDLER_HOOK_TYPE;

/**
 * HBL Reading Type
 */
PACKED typedef struct
{
    uint8 * Data;
    uint8 Length;
} HBL_READING_DATA_TYPE;

/**
 * @brief    - HBL_IO_DEVICE_DATA_CONVERT_TABLE_HEADER
 *
 * @details  - Search method
 *             The unused bit 14 is used to extend the search methods.
 *             Unused field could be filled with 0 or 1 value. If the values are filled with 1 then ignore the unused filed if unused bit 15 filled with 0 then
 *             unused bit 14 can be used for the search method
 *
 *             ----------------------------------------------------------------
 *             |     Bit 15    |     Bit 14    |    Bit 13     |               |
 *             |---------------|---------------|---------------| Search Method |
 *             | Unused Bit 15 | Unused Bit 15 | Interpolation |               |
 *             |---------------|---------------|---------------|---------------|
 *             |       1       |       X       |      0        | Ceiling       |
 *             |---------------|---------------|---------------|---------------|
 *             |       1       |       X       |      1        | Interpolation |
 *             |---------------|---------------|---------------|---------------|
 *             |       0       |       1       |      0        | Exact match   |
 *             |---------------|---------------|---------------|---------------|
 *             |       0       |       1       |      1        | Floor         |
 *              ----------------------------------------------------------------
 *
 */
 typedef struct
{
    uint8 Pd               : 1;			//!< PullDown Probe
    uint8 S1               : 2;			//!< Size 1 (Input)
    uint8 S2               : 2;			//!< Size 2 (Output)
    uint8 Interpolation    : 1;			//!< Interpolation flag
    uint8 Unused           : 2;         //!< unused bit 0 is used for the extended search method
    uint8 Point_Number;					//!< Number of points (item)
}HBL_IO_DEVICE_DATA_CONVERT_TABLE_HEADER;

//! The states for a key
typedef enum
{
    HBL_KEY_RELEASED = 0,
    HBL_KEY_PRESSED  = 1,
    HBL_KEY_UNKNOWN  = 3
}HBL_KEY_TYPE;


//! Encoder direction events
typedef enum
{
    HBL_ENC_CW = 0,
    HBL_ENC_CCW,
    HBL_ENC_UNKNOWN
}HBL_ENC_DIRECTION_TYPE;

//! Definition of data for a Key GI
typedef struct
{
    HBL_KEY_TYPE  Current_State  :2;
    HBL_KEY_TYPE  Previous_State :2;
    uint8         Unused         :4;
}KEYGI_DATA_TYPE;

//! Definition of data for a Potentiometer GI
typedef struct
{
    uint8 Current_Value;
    uint8 Previous_Value;
}ANALOGGI_DATA_TYPE;

//! Definition of data for an Encoder GI
typedef struct
{
    uint8 Current_Enc_Value;
    uint8 Previous_Enc_Value;
    HBL_ENC_DIRECTION_TYPE Direction;
}ENCGI_DATA_TYPE;

//! Definition of external module events
typedef struct
{
    uint8               GI_Position  :6;
    HBLHMI_GI_READ_TYPE GI_Read_Type :2;
}HBLHMI_GI_EVENT_TYPE;

#define INVALID_GI_EVENT            0xFF
#define INVALID_GI_POSITION         0x3F
#define INVALID_GI_READ_TYPE        ((HBLHMI_GI_READ_TYPE)0x03)

typedef void (*HBLHMI_GI_EVENT_HANDLER_TYPE)(HBLHMI_GI_EVENT_TYPE event);


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif // HBLDEFS_H_



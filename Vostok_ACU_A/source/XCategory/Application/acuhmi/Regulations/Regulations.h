/**
 *  @file       
 *
 *  @brief      Public interface for the Regulations module.
 *
 *  @copyright  Copyright 2016-$Date: 2015/07/24 15:56:23EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef REGULATIONS_H_
#define REGULATIONS_H_

#include "Regulations_prm.h"
#include "Callback.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//! Standard main/visual values for the Start function
typedef enum
{
    FS_DONT_CARE            =   0,
    FS_START_CONDITION      =   1,
    FS_RESET_CONDITION      =   2,
    FS_PAUSE_CONDITION      =   3,
    FS_RESET_FAULT          =   4,
    FS_RESTART_CONDITION    =   5,
    FS_END_CONDITION        =   6
} REGULATIONS_START_COMMAND_TYPE;

//! Function configuration structure
typedef struct
{
    // Word 1
    uint8 Function_ID           :7;
    uint8 To_Main               :1;

    uint8 Compartment_Extension :3;     // Used to compose the compartment associated with the function. Used for most significant bits; See "Compartment" bits for least significant bits
    uint8 Down                  :1;
    uint8 Push                  :1;
    uint8 Indirect              :1;
    uint8 Compartment           :2;     // Used to compose the compartment associated with the function Used for least significant bits; See "Compartment_Extension" bits for most significant bits

    //Word 2
    uint8 GI_Position       :6;
    uint8 GI_Read_Type      :2;

    uint8 Factory_Restore_Index;

    //Word 3
    uint8 TimeOut               :1;     // To be used in conjunction with the Push flag; If enabled, the "pushed" function can only be regulated for a certain amount of time. Each regulation attempt resets the timer.
    uint8 Inversion             :1;     // If enabled, inverts the direction of the regulation when the cycle is in the Delay or Running state (logical negation of the Down flag)
    uint8 Sync_Indirects        :1;     // If enabled, when function is regulated, all other functions that are indirect and point to the same slave function will be reset (set to index 1)
    uint8 Auto_Repeat           :1;     // If enabled, the function will be automatically regulated while a key is pressed, as if the user was repeatedly pressing the key
    uint8 Second_Function       :1;     // If enabled, indicates the function should be activated as a secondary function when a key is released within a certain time window after a key press
    uint8 Step                  :1;     // If enabled, indicates that the regulation table for the function was generated as a "step" regulation instead of a table of discrete values
    uint8 Overflow              :1;     // If enabled, increments the regulation index of a secondary function when the maximum regulation index is exceeded
    uint8 Ignore_Visual_Value   :1;     // If enabled, the visual value in the regulation table is not used to set the function pointed to by the GI_Read_Type_Indirect and GI_Position_Indirect values. This flag can only be used when the Indirect flag is enabled.

    uint8 Unused5           :2;
    uint8 Default           :1;     // If enabled, the default value can be replaced by the new user value (if any) after the cycle has been run (each cycle can have each own different function values).
    uint8 Unused6           :1;
    uint8 Acceleration      :1;     // To be used in conjunction with the Auto_Repeat flag; If enabled, increases the speed at which the function is regulated after a certain amount of time
    uint8 On_Release        :1;     // If enabled, the associated Regulation update will be handled only on Key GI (Read Type: DIGITAL) release event, it has no functionality for other GI types.
    uint8 Timed_Modify      :1;     // If enabled, first attempt to regulate the function will be ignored and instead a time window is created where the next attempt to regulate the function will be accepted
    uint8 Limit_Switch      :1;     // If enabled, prevents the regulation index of the function from looping (will be either from max value to min value or vice versa based on the Down flag)

    //Word 4
    uint8 Num_Regulations;

    uint8 UI_Regulation_Table;

    //Word 5
    uint8 Main_Regulation_Table;

    uint8 GI_Position_Indirect      :6;
    uint8 GI_Read_Type_Indirect     :2;

}REGULATIONS_CONFIG_TYPE;

// Offset related to the address of the begin of the Table Function Heading
#define REGULATION_FUNCTION_ID_OFFSET           0
#define REGULATION_MODIFIERS_OFFSET             0
#define REGULATION_GI_DATA_OFFSET               0
#define REGULATION_TIME_MODIFIER_INDEX_OFFSET   0
#define REGULATION_TIME_MODIFIER_SIG_OFFSET     0
#define REGULATION_FLAGS1_OFFSET                0
#define REGULATION_FLAGS2_OFFSET                0

#define REGULATION_NUM_REG_OFFSET               0
#define REGULATION_UI_REG_TABLE_OFFSET          0
#define REGULATION_MAIN_REG_TABLE_OFFSET        0

#define REGULATION_GI_INDIRECT_OFFSET           0

/**
 *  @brief Returns the compartment associated with the regulation at a given position
 *  @param position: position/index of the regulation in the setting file data
 */
#define REGULATION_COMPARTMENT(position)    ((Regulations__GetRegulationConfig(position) == NULL ? 0 : (Regulations__GetRegulationConfig(position)->Compartment_Extension << 2) | Regulations__GetRegulationConfig(position)->Compartment))

#define FUNCTION_ID_MASK        0x007F  // (b6|b5|b4|b3|b2|b1|b0)
#define FUNCTION_ID_TO_MAIN_MASK 0x0080

#define STEP_REG_MASK           0x0020
#define NUM_REG_OFFSET          2
#define MAIN_REG_TAB_OFFSET     4

#define REGULATION_INVALID      0xFF

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================


//=====================================================================================================================
//     Methods used by Hmi and ACU
//=====================================================================================================================

// THis method initializes the Regulations
void Regulations__Initialize(void);

// This function returns the index of a given function Id in the array of Regulations
uint8 Regulations__GetPositionByFunctionID(uint8 function_id, uint8 compartment);

// This function returns the index of a given indirect function Id in the array of Regulations
uint8 Regulations__GetPositionByIndirectFunctionID(uint8 function_id, uint8 compartment);

// This function returns the index of a given indirect and push function Id in the array of Regulations
uint8 Regulations__GetPositionByIndirectPushFunctionID(uint8 function_id, uint8 compartment);

// This function returns the index of a given down function Id in the array of Regulations
uint8 Regulations__GetPositionByDownFunctionID(uint8 function_id, uint8 compartment);

// This function returns the index of a function with a given generic input in the array of Regulations
uint8 Regulations__GetFunctionPositionByGi(uint8 gi_position, uint8 gi_type, uint8 compartment);

// This method returns the Function Id by position
uint8 Regulations__GetFunctionIDByPosition(uint8 position);

//This function returns the total size of the Regulations buffer.
uint8 Regulations__GetTotalSize(void);

// This function returns the total size of the Regulations set To Main
uint8 Regulations__GetToMainSize(void);

REGULATIONS_CONFIG_TYPE * Regulations__GetRegulationConfig(uint8 position);

uint8 Regulations__GetIndexValueByPosition(uint8 position);

uint8 Regulations__GetRequestedIndexValueByPosition(uint8 position);

void Regulations__AcceptRequestByPosition(uint8 position);

void Regulations__RejectRequestByPosition(uint8 position);

// This function returns the index of the visual value passed to the function
uint8 Regulations__GetIndexOfVisualValueByPosition(sint32 visual_value, uint8 position);

uint8 Regulations__GetIndexOfMainValueByPosition(sint32 main_value, uint8 position);

void Regulations__RegisterChangeEventHandler(CALLBACK_HANDLER_TYPE event_handler);
void Regulations__UnregisterChangeEventHandler(CALLBACK_HANDLER_TYPE event_handler);

//=====================================================================================================================
//     Methods used by ACU Only
//=====================================================================================================================

// Get the current Regulations Indexes
const uint8 * Regulations__GetIndexBufferPtr(void);

// Request Regulations Indexes
void Regulations__SetRequestIndexBuffer(const uint8 * value_ptr, uint8 value_size);

BOOL_TYPE Regulations__IsRegulationChanged(uint8 position);

sint32 Regulations__GetMainValueByPosition(uint8 position);
sint32 Regulations__GetMainValueByPositionPlusIndex(uint8 position, uint8 index);
sint32 Regulations__GetRequestedMainValueByPosition(uint8 position);
DATATYPE_TYPE Regulations__GetMainValueTypeByPosition(uint8 position);

//=====================================================================================================================
//     Methods used by Hmi Only
//=====================================================================================================================

// Get the Requested Regulations Indexes
const uint8 * Regulations__GetIndexRequestBufferPtr(void);

// Method used by the Hmi to update the Regulations received from the Hmi
void Regulations__SetIndexBuffer(const uint8 * value_ptr, uint8 value_size);

// Method used by the Hmi to set the value of a Requested regulation to be sent to the ACU
BOOL_TYPE Regulations__SetRequedIndex(uint8 position, uint8 value);

sint32 Regulations__GetVisualValueByPosition(uint8 position);
sint32 Regulations__GetRequestedVisualValueByPosition(uint8 position);
sint32 Regulations__GetVisualValueByPositionPlusIndex(uint8 position, uint8 index);
DATATYPE_TYPE Regulations__GetVisualValueTypeByPosition(uint8 position);

#endif // REGULATION_H_

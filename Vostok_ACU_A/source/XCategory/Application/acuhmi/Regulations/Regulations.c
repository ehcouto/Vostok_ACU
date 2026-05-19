/**
 *  @file       
 *
 *  @brief      Implements the interfaces for regulations. Some interfaces can be used on both ACU and HMI. Some interfaces
 *              are meant for only ACU or only HMI.
 *
 *  @details    Common interfaces:
 *              - Get the position of regulation
 *              - Get number of regulations defined in the setting file
 *              - Get number of regulations that are "to main" (considered by ACU)
 *              - Get configuration of a regulation (function ID or other behavioral flags)
 *              - Get current index of regulations
 *
 *              ACU only interfaces:
 *              - Accept/Reject regulations
 *              - Check if a regulation has changed (new request from HMI)
 *              - Get main values of a regulation
 *              - Get main value type
 *
 *              HMI only interfaces:
 *              - Register a callback function on regulation changes
 *              - Set regulation request
 *              - Get visual values of a regulation
 *              - Get visual value type
 *
 *  @copyright  Copyright 2015-2019 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"
#include "Regulations.h"

#include "Callback.h"
#include "Log.h"
#include "Micro.h"
#include "Mode.h"
#if (SYSTEM_ACU_HANDLING == ENABLED)
    #include "NVData.h"
    #include "TestMgr.h"
#endif
#include "SettingFile.h"
#include <string.h>
#include "Reveal.h"

//-------------------------------------- IDs FOR LOG messages----------------------------------------------------------
//! IDs for Log messages
typedef enum
{
    GI_POSITION_AND_TYPE_DOES_NOT_EXIST = 0,    //data: Upper Byte: GI position, Lower Byte: GI Read Type
    POSITION_OUT_OF_RANGE,                      //data: Upper Byte: Source of message, Lower Byte: Regulation position that is out of range
    TOO_MANY_REGULATIONS_DEFINED,               //data: Upper Byte: Number of regulations handled by this module(REGULATION_MAX_NUM); Lower byte: Number of regulations defined in the setting file
    INDEX_OUT_OF_RANGE,                         //data: Upper Byte: out of boundary index, Lower Byte: Regulation position that is out of range
    CONFIG_NOT_FOUND                            //data: Regulation position that is out of range
} MODULE_REGULATIONS_LOG_MESSAGE_ID_TYPE;
//! ID for this module on the Log module.
#define MODULE_NAME     MODULE_REGULATIONS


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
static uint8 Num_Functions_To_Main; //!< Number of functions that must be accepted or rejected by the ACU, otherwise a function is always accepted
static uint8 Num_Functions_Total;   //!< Total number of functions defined in the setting file

typedef enum
{
    REGULATION_PARAM_UINT8,
    REGULATION_PARAM_SINT8,
    REGULATION_PARAM_UINT16,
    REGULATION_PARAM_SINT16
} REGULATION_PARAM_TYPE;

typedef struct
{
    uint8 Unused1                   :6;
    REGULATION_PARAM_TYPE Type      :2;
    uint8 Step;
}REGULATION_PARAM_HEADER_TYPE;

//! Conversion table that converts the main/visual value type in the regulation table to the DATATYPE_TYPE typedef
static const DATATYPE_TYPE REGULATION_HEADER_TYPE_CONVERSION[] =
{
  /* DATATYPE_TYPE */   /* REGULATION_PARAM_HEADER_TYPE */
    DATATYPE_UINT8,     // REGULATION_PARAM_UINT8
    DATATYPE_SINT8,     // REGULATION_PARAM_SINT8
    DATATYPE_UINT16,    // REGULATION_PARAM_UINT16
    DATATYPE_SINT16,    // REGULATION_PARAM_SINT16
};

//! Maximum number of regulations that can be handled by the module. Can be overridden for project specific purposes.
//! This value is validated during initialization to ensure the firmware can handle the number of regulations defined
//! in the setting file.
#ifndef REGULATION_MAX_NUM
    #define REGULATION_MAX_NUM      41
#endif
//! Ensure number of regulations does not exceed the max payload size for Reveal to prevent the need for multiple messages
//! Reducing the reveal payload by 5 to allow for additional bytes in the payload that aren't a regulation index
CT_ASSERT(REGULATION_MAX_NUM <= REVEAL_PAYLOAD_SIZE - 5);

static REGULATIONS_CONFIG_TYPE * Regulations_Config;

#if (SYSTEM_ACU_HANDLING == ENABLED)
    #pragma location = "_appliance_segment"
    static uint8 Regulations_Index_Buffer[REGULATION_MAX_NUM];

    #pragma location = "_appliance_segment"
    static uint8 Regulations_Index_Request_Buffer[REGULATION_MAX_NUM];

#else
    static uint8 Regulations_Index_Buffer[REGULATION_MAX_NUM];
    static uint8 Regulations_Index_Request_Buffer[REGULATION_MAX_NUM];
#endif

//! Callback structure
#ifndef REGULATION_CALLBACK_REGISTER_SIZE
    #define REGULATION_CALLBACK_REGISTER_SIZE 5
#endif
DEFINE_CALLBACK_CONTROL_STRUCTURE(Regulation_Callback_Control_Structure, REGULATION_CALLBACK_REGISTER_SIZE);

#ifndef REGULATIONS_INITIALIZE_FACTORY_RESTORE_INDEX_FEATURE
    #define REGULATIONS_INITIALIZE_FACTORY_RESTORE_INDEX_FEATURE DISABLED   // Feature is disabled by default
#endif

#if (SYSTEM_ACU_HANDLING == DISABLED)
    static BOOL_TYPE Regulations_Start;
#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static sint32 GetRegValue(SETTINGFILE_OFFSET_TYPE type, uint8 position, uint8 index_value);
static DATATYPE_TYPE GetRegulationTableValueType(SETTINGFILE_OFFSET_TYPE sf_pointer, uint8 regulation_position);
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================


//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
// Methods used by ACU and UI
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module Regulation and its variables
 *
 */
void Regulations__Initialize(void)
{
    SETTINGFILE_LOADER_TYPE aux_loader;

    Regulations_Config = NULL;

    if (SettingFile__BasicLoader(SF_PTR_UI_FUNCTION_HEADING, 0, &aux_loader) == PASS )
    {
        Regulations_Config = (REGULATIONS_CONFIG_TYPE *)(void *)aux_loader.Data;
    }

    if (SettingFile__BasicLoader(SF_PTR_UI_PRODUCT_CONFIG, SF_DISPL_FUNCTIONS_NUMBER, &aux_loader) == PASS )
    {
        Num_Functions_To_Main = aux_loader.Data[0];
        Num_Functions_Total = aux_loader.Data[1];

        if (Num_Functions_Total > REGULATION_MAX_NUM)
        {
            LOG_ADD_EXCEPTION(TOO_MANY_REGULATIONS_DEFINED, COMBINE_BYTES(REGULATION_MAX_NUM, Num_Functions_Total));
            Mode__SetModeExtended(MODE_NORMAL, SUBMODE_NORMAL_ERROR);               // Module will not be able to handle all the regulations defined in the
        }                                                                           // setting file. Put machine in a safe state.
    }

#if (SYSTEM_ACU_HANDLING == ENABLED)
    // ACU Handling
    if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)         // If this is a cold reset,
    {
        if (NVData__GetLastRestoreStatus()->Bit.Appliance_Data_Recovered != NVDATA_RESTORE_OKAY) // and Data is not restored correctly
        {
            // Force all regulation values to 1
            memset (Regulations_Index_Buffer,1,sizeof(Regulations_Index_Buffer));
            memset (Regulations_Index_Request_Buffer,1,sizeof(Regulations_Index_Request_Buffer));

            // Apply factory restore index values to appropriate regulations
            for (uint8 index = 0; index < Num_Functions_Total; index++)
            {
                if (Regulations_Config[index].Factory_Restore_Index != 0)
                {
                    Regulations_Index_Buffer[index] = Regulations_Config[index].Factory_Restore_Index;
                    Regulations_Index_Request_Buffer[index] = Regulations_Config[index].Factory_Restore_Index;
                }
            }
        }
    }

    // Test Cycle regulation must always be set to an index that ensures the test cycle does not get started unintentionally after a power cycle
    // Look up position of test cycle regulation and set both the regulation index buffers to ensure HMI gets the same index
    uint8 test_cycle_regulation_position = Regulations__GetPositionByFunctionID(FUNC_ID_TEST_CYCLE, 0);
    if (test_cycle_regulation_position != REGULATION_INVALID)
    {
        uint8 test_cycle_regulation_index = Regulations__GetIndexOfMainValueByPosition(TESTMGR_CYCLE_NONE, test_cycle_regulation_position);
        if (test_cycle_regulation_index != REGULATION_INVALID)
        {
            Regulations_Index_Buffer[test_cycle_regulation_position] = test_cycle_regulation_index;
            Regulations_Index_Request_Buffer[test_cycle_regulation_position] = test_cycle_regulation_index;
        }
    }

#else
    //Hmi only handling
    if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD) // If this is a cold reset, force defaults to 1 or the factory restore index
    {
        memset (Regulations_Index_Buffer,1,sizeof(Regulations_Index_Buffer));
        memset (Regulations_Index_Request_Buffer,1,sizeof(Regulations_Index_Request_Buffer));

#if (REGULATIONS_INITIALIZE_FACTORY_RESTORE_INDEX_FEATURE == ENABLED)
        // Apply factory restore index values to regulations with that index configured
        // This index is only applicable until the HMI receives regulations from the ACU
        // The factory restore index is often times a better initialization value than an index of 1
        for (uint8 index = 0; index < Num_Functions_Total; index++)
        {
            if (Regulations_Config[index].Factory_Restore_Index != 0)
            {
                Regulations_Index_Buffer[index] = Regulations_Config[index].Factory_Restore_Index;
                Regulations_Index_Request_Buffer[index] = Regulations_Config[index].Factory_Restore_Index;
            }
        }
#endif  // REGULATIONS_APPLY_FACTORY_RESTORE_INDEX == ENABLED
    }
#endif

    Callback__Initialize(&Regulation_Callback_Control_Structure);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns the Position for a given Function ID
 * @param funciton_id
 * @return position
 */
uint8 Regulations__GetPositionByFunctionID(uint8 function_id, uint8 compartment)
{
    uint8 retval;
    uint8 i;

    retval = REGULATION_INVALID;
    for (i=0; i<Num_Functions_Total; i++ )
    {
        if ((Regulations_Config[i].Function_ID == function_id) &&
            (REGULATION_COMPARTMENT(i) == compartment))
        {
            retval = i;
            break;
        }
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns the Position for a given indirect Function ID
 * @param funciton_id
 * @return position
 */
uint8 Regulations__GetPositionByIndirectFunctionID(uint8 function_id, uint8 compartment)
{
    uint8 retval;
    uint8 i;

    retval = REGULATION_INVALID;
    for (i=0; i<Num_Functions_Total; i++ )
    {
        if ((Regulations_Config[i].Function_ID == function_id) &&
            (REGULATION_COMPARTMENT(i) == compartment) &&
            (Regulations_Config[i].Indirect == TRUE) )
        {

            retval = i;
            break;
        }
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns the Position for a given indirect Function ID
 * @param funciton_id
 * @return position
 */
uint8 Regulations__GetPositionByIndirectPushFunctionID(uint8 function_id, uint8 compartment)
{
    uint8 retval;
    uint8 i;

    retval = REGULATION_INVALID;
    for (i=0; i<Num_Functions_Total; i++ )
    {
        if ((Regulations_Config[i].Function_ID == function_id) &&
            (REGULATION_COMPARTMENT(i) == compartment) &&
            (Regulations_Config[i].Indirect == TRUE) &&
            (Regulations_Config[i].Push == TRUE))
        {
            retval = i;
            break;
        }
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns the Position for a given down Function ID
 * @param funciton_id
 * @return position
 */
uint8 Regulations__GetPositionByDownFunctionID(uint8 function_id, uint8 compartment)
{
    uint8 retval;
    uint8 i;

    retval = REGULATION_INVALID;
    for (i=0; i<Num_Functions_Total; i++ )
    {
        if ((Regulations_Config[i].Function_ID == function_id) &&
            (REGULATION_COMPARTMENT(i) == compartment) &&
            (Regulations_Config[i].Down == TRUE) )
        {

            retval = i;
            break;
        }
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns the Function ID for a given Regulation Position.
 * @param position
 * @return function_id
 */
uint8 Regulations__GetFunctionIDByPosition(uint8 position)
{
    uint8 function_id = REGULATION_INVALID;

    if (position >= Num_Functions_Total)
    {
        LOG_ADD_EXCEPTION(POSITION_OUT_OF_RANGE, COMBINE_BYTES(0, position));
    }
    else if (Regulations_Config != NULL)
    {
        function_id = Regulations_Config[position].Function_ID;
    }

    return (function_id);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method retrieves the total number of regulations are present.
 * @return number of regulations
 */
uint8 Regulations__GetTotalSize(void)
{
    return (Num_Functions_Total);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This function retrieves how many regulation to Main are present.
 * @return number of regulations
 */
uint8 Regulations__GetToMainSize(void)
{
    return (Num_Functions_To_Main);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method exposes the data related to the configuration of a Regulation.
 * @param position
 * @return pointer to the Regulation config in the Setting File.
 */
REGULATIONS_CONFIG_TYPE * Regulations__GetRegulationConfig(uint8 position)
{
    REGULATIONS_CONFIG_TYPE * regulation_config = NULL;

    if (position >= Num_Functions_Total)
    {
        LOG_ADD_EXCEPTION(CONFIG_NOT_FOUND, position);
    }
    else if (Regulations_Config != NULL)
    {
        regulation_config = &Regulations_Config[position];
    }

    return (regulation_config);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method retrieves the Index Value of a Regulation by Position
 * @param position
 * @return Index Value
 */
uint8 Regulations__GetIndexValueByPosition(uint8 position)
{
    if (position < Num_Functions_Total)
    {
        return(Regulations_Index_Buffer[position]);
    }
    else
    {
        LOG_ADD_EXCEPTION(POSITION_OUT_OF_RANGE, COMBINE_BYTES(1, position));
        return (REGULATION_INVALID);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method retrieves the Index Value of a Requested Regulation by Position
 * @param position
 * @return Requested Index Value
 */
uint8 Regulations__GetRequestedIndexValueByPosition(uint8 position)
{
    if (position < Num_Functions_Total)
    {
        return(Regulations_Index_Request_Buffer[position]);
    }
    else
    {
        LOG_ADD_EXCEPTION(POSITION_OUT_OF_RANGE, COMBINE_BYTES(2, position));
        return (REGULATION_INVALID);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method checks if there is a Regulation change pending by Position
 * @param position
 * @return TRUE (pending change) FALSE (no pending change)
 */
BOOL_TYPE Regulations__IsRegulationChanged(uint8 position)
{
    if (position < Num_Functions_Total)
    {
        if (Regulations_Index_Buffer[position] != Regulations_Index_Request_Buffer[position])
        {
            return (TRUE);
        }
        else
        {
            return (FALSE);
        }
    }
    else
    {
        LOG_ADD_EXCEPTION(POSITION_OUT_OF_RANGE, COMBINE_BYTES(3, position));
        return (FALSE);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method Accepts a requested regulation.
 * @param function_id  (0 to Num_Functions_Total)
 */
void Regulations__AcceptRequestByPosition(uint8 position)
{
    if (position < Num_Functions_Total)
    {
        if( (Regulations_Index_Request_Buffer[position] > 0) &&                                             //regulation index 0 is also invalid
            (Regulations_Index_Request_Buffer[position] <= Regulations_Config[position].Num_Regulations))
        {
            Regulations_Index_Buffer[position] = Regulations_Index_Request_Buffer[position];    //accept the request
        }
        else
        {
            LOG_ADD_EXCEPTION(INDEX_OUT_OF_RANGE, COMBINE_BYTES(Regulations_Index_Request_Buffer[position], position));
            Regulations_Index_Request_Buffer[position] = Regulations_Index_Buffer[position];    //replace the request with current
        }
    }
    else
    {
        LOG_ADD_EXCEPTION(POSITION_OUT_OF_RANGE, COMBINE_BYTES(4, position));
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method Rejects a requested regulation.
 * @param function_id (0 to Num_Functions_Total)
 */
void Regulations__RejectRequestByPosition(uint8 position)
{
    if (position < Num_Functions_Total)
    {
        Regulations_Index_Request_Buffer[position] = Regulations_Index_Buffer[position];
    }
    else
    {
        LOG_ADD_EXCEPTION(POSITION_OUT_OF_RANGE, COMBINE_BYTES(5, position));
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Returns the index of the Start regulation that matches the visual value passed to the function.
 * @param visual_value
 * @param position
 */
uint8 Regulations__GetIndexOfVisualValueByPosition(sint32 visual_value, uint8 position)
{
    uint8 regulation_index;
    REGULATIONS_CONFIG_TYPE * regulation;
    uint8 retval =  REGULATION_INVALID;

    regulation = Regulations__GetRegulationConfig(position);

    if(regulation != NULL)
    {
        for(regulation_index = 1; regulation_index <= regulation->Num_Regulations; regulation_index++)
        {
            if(Regulations__GetVisualValueByPositionPlusIndex(position, regulation_index) == visual_value)
            {
                retval = regulation_index;
                break;
            }
        }
    }

    return(retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Returns the index of the regulation that matches the main value passed to the function.
 * @param main_value: references the main value from a regulation table
 * @param position: position of the regulation in the list of all regulations (based on setting file)
 */
uint8 Regulations__GetIndexOfMainValueByPosition(sint32 main_value, uint8 position)
{
    uint8 regulation_index;
    REGULATIONS_CONFIG_TYPE * regulation;
    uint8 retval =  REGULATION_INVALID;

    regulation = Regulations__GetRegulationConfig(position);

    if(regulation != NULL)
    {
        for(regulation_index = 1; regulation_index <= regulation->Num_Regulations; regulation_index++)
        {
            if(Regulations__GetMainValueByPositionPlusIndex(position, regulation_index) == main_value)
            {
                retval = regulation_index;
                break;
            }
        }
    }

    return(retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Registers a regulation event handler with the module.
 * @param   event_handler: pointer to the function that handles the event
 */
void Regulations__RegisterChangeEventHandler(CALLBACK_HANDLER_TYPE event_handler)
{
    Callback__Register(&Regulation_Callback_Control_Structure, event_handler);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Unregisters a regulation event handler with the module.
 * @param   event_handler: pointer to the function that handles the event
 */
void Regulations__UnregisterChangeEventHandler(CALLBACK_HANDLER_TYPE event_handler)
{
    Callback__Unregister(&Regulation_Callback_Control_Structure, event_handler);
}

//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
// Methods used by ACU Only
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  This method retrieves the pointer to the Regulation Index Buffer.
 * @return pointer to the Regulation index buffer.
 */
const uint8 * Regulations__GetIndexBufferPtr(void)
{
    return ((const uint8 *)Regulations_Index_Buffer);       //lint !e926 cast from pointer to pointer [MISRA 2004 Rule 11.4]
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method sets the buffer used to track requested regulations from the HMI
 * @param value_ptr:  Points to array of regulation indexes
 * @param value_size: Number of regulations sent from the HMI
 */
void Regulations__SetRequestIndexBuffer(const uint8 * value_ptr, uint8 value_size)
{
    if (value_size == Num_Functions_Total)      // Only update requested regulations buffer if the number of regulations matches the expected number
    {
        // Copy all regulations into the request buffer
        memcpy(Regulations_Index_Request_Buffer, value_ptr, value_size);

        // Copy regulations that are NOT "to_main" into the regular buffer to automatically accept the value
        memcpy(&(Regulations_Index_Buffer[Num_Functions_To_Main]), &(value_ptr[Num_Functions_To_Main]), (Num_Functions_Total - Num_Functions_To_Main));
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method retrieves the Main Value of a Regulation by position.
 * @param  position: position of the regulation in the list of all regulations (based on setting file)
 * @return Regulation Main Value
 */
sint32 Regulations__GetMainValueByPosition(uint8 position)
{
    if (position < Num_Functions_Total)
    {
        return (GetRegValue(SF_PTR_ACU_REG_PARAM_TABLE, Regulations_Config[position].Main_Regulation_Table,Regulations_Index_Buffer[position]));
    }
    else
    {
        LOG_ADD_EXCEPTION(POSITION_OUT_OF_RANGE, COMBINE_BYTES(6, position));
        return (0);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  This method retrieves the Main Value of a Regulation by position at a specific regulation index.
 * @param  position:  position of the regulation in the list of all regulations (based on setting file)
 * @return Regulation Main Value
 */
sint32 Regulations__GetMainValueByPositionPlusIndex(uint8 position, uint8 index)
{
    if (position < Num_Functions_Total)
    {
        return (GetRegValue(SF_PTR_ACU_REG_PARAM_TABLE, Regulations_Config[position].Main_Regulation_Table, index));
    }
    else
    {
        LOG_ADD_EXCEPTION(POSITION_OUT_OF_RANGE, COMBINE_BYTES(12, position));
        return (0);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  This method retrieves the Main Value of a Requested Regulation by position.
 * @param  position: position of the regulation in the list of all regulations (based on setting file)
 * @return Regulation Requested Main Value
 */
sint32 Regulations__GetRequestedMainValueByPosition(uint8 position)
{
    if (position < Num_Functions_Total)
    {
        return (GetRegValue(SF_PTR_ACU_REG_PARAM_TABLE,Regulations_Config[position].Main_Regulation_Table,Regulations_Index_Request_Buffer[position]));
    }
    else
    {
        LOG_ADD_EXCEPTION(POSITION_OUT_OF_RANGE, COMBINE_BYTES(7, position));
        return (0);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  This method retrieves the type of the Main Value of a Requested Regulation by position.
 * @param  position: position of the regulation in the list of all regulations (based on setting file)
 * @return Regulation Requested Main Value Type
 */
DATATYPE_TYPE Regulations__GetMainValueTypeByPosition(uint8 position)
{
    return(GetRegulationTableValueType(SF_PTR_ACU_REG_PARAM_TABLE, Regulations_Config[position].Main_Regulation_Table));
}

//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
// Methods used by HMI Only
//=====================================================================================================================
//=====================================================================================================================
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  This method retrieves the pointer to the Requested Regulation Index Buffer.
 * @return pointer to the Requested Regulation index buffer.
 */
const uint8 * Regulations__GetIndexRequestBufferPtr(void)
{
    return ((const uint8 *)Regulations_Index_Request_Buffer);   //lint !e926 cast from pointer to pointer [MISRA 2004 Rule 11.4]
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method is used by the Hmi to set the Regulations buffer received from the ACU
 * @param value_ptr
 * @param value_size
 */
void Regulations__SetIndexBuffer(const uint8 * value_ptr, uint8 value_size)
{
    if (value_size == Num_Functions_Total)
    {
        memcpy(Regulations_Index_Buffer,value_ptr,value_size);

        #if (SYSTEM_ACU_HANDLING == FALSE)
            if (Regulations_Start == TRUE)
            {
                Regulations_Start = FALSE;
                memcpy(Regulations_Index_Request_Buffer,Regulations_Index_Buffer,value_size);
            }
        #endif
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method Set one position Data into the Request Index Buffer
 * @param position: position of the regulation in the list of all regulations (based on setting file)
 * @param value: desired index of the regulation
 * @return TRUE if the requested index is set to the desired index, else FALSE.
 */
BOOL_TYPE Regulations__SetRequedIndex(uint8 position, uint8 value)
{
    BOOL_TYPE change_requested = FALSE;
    BOOL_TYPE requested_index_set = FALSE;

    if (position < Num_Functions_Total)
    {
        if (Regulations_Index_Request_Buffer[position] != value)
        {
            change_requested = TRUE;
        }

        Regulations_Index_Request_Buffer[position] = value;
        requested_index_set = TRUE;

        if (change_requested == TRUE)
        {
            Callback__Notify(&Regulation_Callback_Control_Structure, position);
        }
    }
    else
    {
        LOG_ADD_EXCEPTION(POSITION_OUT_OF_RANGE, COMBINE_BYTES(8, position));
    }

    return (requested_index_set);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  This method retrieves the Visual Value of a Regulation by position.
 * @param  position: position of the regulation in the list of all regulations (based on setting file)
 * @return Regulation Visual Value
 */
sint32 Regulations__GetVisualValueByPosition(uint8 position)
{
    if (position < Num_Functions_Total)
    {
        return (GetRegValue(SF_PTR_UI_DISPLAY_REGULATION_TABLE,Regulations_Config[position].UI_Regulation_Table,Regulations_Index_Buffer[position]));
    }
    else
    {
        LOG_ADD_EXCEPTION(POSITION_OUT_OF_RANGE, COMBINE_BYTES(9, position));
        return (0);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  This method retrieves the Visual Value of a by position.
 * @param  position: position of the regulation in the list of all regulations (based on setting file)
 * @return Regulation Requested Visual Value
 */
sint32 Regulations__GetRequestedVisualValueByPosition(uint8 position)
{
    if (position < Num_Functions_Total)
    {
        return (GetRegValue(SF_PTR_UI_DISPLAY_REGULATION_TABLE,Regulations_Config[position].UI_Regulation_Table,Regulations_Index_Request_Buffer[position]));
    }
    else
    {
        LOG_ADD_EXCEPTION(POSITION_OUT_OF_RANGE, COMBINE_BYTES(10, position));
        return (0);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  This method retrieves the Visual Value of a regulation by position and index.
 * @param  position: position of the regulation in the list of all regulations (based on setting file)
 * @return Regulation Requested Visual Value
 */
sint32 Regulations__GetVisualValueByPositionPlusIndex(uint8 position, uint8 index)
{
    if (position < Num_Functions_Total)
    {
        return (GetRegValue(SF_PTR_UI_DISPLAY_REGULATION_TABLE, Regulations_Config[position].UI_Regulation_Table, index));
    }
    else
    {
        LOG_ADD_EXCEPTION(POSITION_OUT_OF_RANGE, COMBINE_BYTES(11, position));
        return (0);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  This method retrieves the type of the Visual Value of a Requested Regulation by position.
 * @param  position: position of the regulation in the list of all regulations (based on setting file)
 * @return Regulation Requested Visual Value Type
 */
DATATYPE_TYPE Regulations__GetVisualValueTypeByPosition(uint8 position)
{
    return(GetRegulationTableValueType(SF_PTR_UI_DISPLAY_REGULATION_TABLE, Regulations_Config[position].UI_Regulation_Table));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   This method returns the position of a function with a given generic input position and type
 * @param   gi_position: position of the GI
 * @param   gi_type: read type of the GI
 * @param   compartment: compartment assigned to the function
 * @return  Position of the function with the given position and type of the generic input; If not found, returns REGULATION_INVALID
 */
uint8 Regulations__GetFunctionPositionByGi(uint8 gi_position, uint8 gi_type, uint8 compartment)
{
    uint8 retval;
    uint8 index;

    retval = REGULATION_INVALID;

    if (Regulations_Config != NULL)
    {
        for (index=0; index<Num_Functions_Total; index++ )
        {
            if ((Regulations_Config[index].GI_Position == gi_position) &&
                (Regulations_Config[index].GI_Read_Type == gi_type) )
            {
                retval =  index;
                break;
            }
        }

        if (retval == REGULATION_INVALID)
        {
            LOG_ADD_EXCEPTION(GI_POSITION_AND_TYPE_DOES_NOT_EXIST, COMBINE_BYTES(gi_position, gi_type));
        }
    }

    return (retval);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @param type: setting file displacement (SF_PTR_MAIN_REG_PARAM_TABLE or SF_PTR_UI_REGULATION_PARAMS)
 * @param position: position of the regulation in the list of all regulations (based on setting file)
 * @param index_value
 * @return
 */
static sint32 GetRegValue(SETTINGFILE_OFFSET_TYPE type, uint8 position, uint8 index_value)
{
    sint32 retval;
    SETTINGFILE_LOADER_TYPE aux_loader;
    REGULATION_PARAM_HEADER_TYPE * header;
    uint16 v_uint16;
    sint16 v_sint16;

    retval = 0;

    if (position < REGULATION_MAX_NUM)
    {

        if (SettingFile__BasicLoader(type ,position ,&aux_loader) == PASS )
        {
            header = (REGULATION_PARAM_HEADER_TYPE *)aux_loader.Data;   //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]

            // Prevents inde_value = 0
            if (index_value > 0)
            {
                index_value--;    // 1 will be adjusted the first index (1) means 0 in the math / table.
            }

            if (header->Step > 0)
            {
                // Byte 0
                // Header - Type
                // Byte 1
                // Header - Step
                //
                // For 8 bits
                // Byte 2 none
                // Byte 3 initial value
                //
                // For 16 bits
                // Byte 2 = MSB data
                // Byte 3 = LSB data
                // Position defined the regulation
                // Index is the Main index value.

                // Step Regulation
                switch (header->Type)
                {
                    default:
                    case REGULATION_PARAM_UINT8:
                        retval = (sint32)(aux_loader.Data[3] + (header->Step * index_value));

                        break;
                    case REGULATION_PARAM_SINT8:
                        retval = (sint32)((sint8)(aux_loader.Data[3]) + (sint8)( header->Step *index_value));
                        break;
                    case REGULATION_PARAM_UINT16:
                        v_uint16 = *((uint16 PACKED *)&aux_loader.Data[2]);     //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
                        v_uint16 = ENDIANU16_STON(v_uint16);
                        retval = (sint32)(v_uint16 + (uint16)(header->Step * index_value));
                        break;
                    case REGULATION_PARAM_SINT16:
                        v_sint16 = *((sint16 PACKED *)&aux_loader.Data[2]);     //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
                        v_sint16 = ENDIANS16_STON(v_sint16);
                        retval = (sint32)(v_sint16 + (sint16)(header->Step * index_value));
                        break;
                }
            }
            else
            {
                // table regulation
                // Byte 0
                // Header - Type
                // Byte 1
                // Header - Step
                //
                // For 8 bits
                // Byte 2 data0
                // Byte 3 data1
                // byte 4 ...
                //
                // For 16 bits
                // Byte 2 = MSB data0
                // Byte 3 = LSB data0
                // Byte 4 = MSB data1
                // Byte 5 = LSB data1
                // ...
                // ...

                // Step Regulation

                switch (header->Type)
                {
                    default:
                    case REGULATION_PARAM_UINT8:
                        retval = (sint32)(aux_loader.Data[2+index_value]);
                        break;
                    case REGULATION_PARAM_SINT8:
                        retval = (sint32)( *((sint8 *)&aux_loader.Data[2+index_value]) );       //lint !e926 cast from pointer to pointer [MISRA 2004 Rule 11.4]
                        break;
                    case REGULATION_PARAM_UINT16:
                        v_uint16 = *((uint16 PACKED *)&aux_loader.Data[2+ (index_value * 2)]);  //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
                        v_uint16 = ENDIANU16_STON(v_uint16);
                        retval = (sint32)v_uint16;
                        break;
                    case REGULATION_PARAM_SINT16:
                        v_sint16 = *((sint16 PACKED *)&aux_loader.Data[2+ (index_value * 2)]);  //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
                        v_sint16 = ENDIANS16_STON(v_sint16);
                        retval = (sint32)v_sint16;
                        break;
                }
            }
        }
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   This method retrieves the type of the Main or Visual Value of a Requested Regulation by position.
 * @param   sf_pointer: setting file pointer that will determine if the lookup is for the data type of the ACU or HMI
 *                    value in the regulation table
 * @param   position: position of the regulation in the setting file data
 * @return  Regulation Requested Main/Visual Value Type
 */
static DATATYPE_TYPE GetRegulationTableValueType(SETTINGFILE_OFFSET_TYPE sf_pointer, uint8 regulation_position)
{
    SETTINGFILE_LOADER_TYPE sf_loader;
    REGULATION_PARAM_HEADER_TYPE * header;
    DATATYPE_TYPE regulation_value_type = DATATYPE_INVALID;

    if (regulation_position < REGULATION_MAX_NUM)
    {
        if (SettingFile__BasicLoader(sf_pointer, regulation_position, &sf_loader) == PASS )
        {
            header = (REGULATION_PARAM_HEADER_TYPE *)(void *)sf_loader.Data;

            regulation_value_type = REGULATION_HEADER_TYPE_CONVERSION[header->Type];
        }
    }

    return(regulation_value_type);
}

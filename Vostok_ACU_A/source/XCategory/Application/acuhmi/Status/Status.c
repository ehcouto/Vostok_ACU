/**
 *  @file
 *
 *  @brief      For the ACU, this module builds the data for a Status message to send to the HMI.
 *              For the HMI, this module takes the data from the Status message and populates the status variables in the project.
 *
 *  @copyright  Copyright 2016-$Date: 2015/07/17 13:44:30EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"
#include "Status.h"

#include "Crc16.h"
#include "Log.h"
#include "Reveal.h"
#include "SettingFile.h"
#include <string.h>
#include "Utilities.h"


//-------------------------------------- IDs FOR LOG messages----------------------------------------------------------
//! IDs FOR LOG messages
typedef enum
{
    STATUS_LOG_BAD_SETTINGFILE = 0,
    RECEIVED_INCORRECT_STATUS_MESSAGE_LENGTH,
    VARIABLE_MISSING,
    INVALID_DATA_TYPE
} MODULE_STATUS_LOG_MESSAGE_ID_TYPE;

//! ID for this module on the Log module.
#define MODULE_NAME     MODULE_STATUS

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//! Holds the information related to the status variables
typedef struct
{
    ACU_VARIABLE_ID_TYPE  ID;
    uint8             Offset;
}STATUS_VARIABLE_TYPE;

static STATUS_VARIABLE_TYPE * Status_Variables; //! Pointer to the status variable configuration from the setting file
static uint16 Crc_Calc_16_Prev;					//! Variable to store the current crc for the status variables. It is used on the Status__HasChanged() function.
static uint8 Num_Status_Variables;              //!< Number of status variables defined in the setting file
static BOOL_TYPE Status_Configured;             //!< TRUE if the status variables were successfully retrieved from the setting file, else FALSE
static uint8 Status_Variables_Size;             //!< This variable holds the total size of the status variable and calculated during the initialization
#if (RECEIVE_STATUS_VARIABLES_FEATURE == ENABLED)
    static BOOL_TYPE Status_Valid;              //!< TRUE if the status data has been successfully received, else FALSE
#endif

#ifndef STATUSCHANGE_CALLBACK_REGISTER_SIZE
    #define STATUSCHANGE_CALLBACK_REGISTER_SIZE 5
#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
DEFINE_CALLBACK_CONTROL_STRUCTURE(StatusChange_Callback_Control_Structure, STATUSCHANGE_CALLBACK_REGISTER_SIZE);

static uint8 StatusGetSize(void);
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method initializes the variable related to Status.
 */
void Status__Initialize(void)
{
    Status_Configured = FALSE;
    Num_Status_Variables = 0;
    Status_Variables_Size = 0;
    Crc_Calc_16_Prev = 0;
    #if (RECEIVE_STATUS_VARIABLES_FEATURE == ENABLED)
        Status_Valid = FALSE;
    #endif
    Callback__Initialize(&StatusChange_Callback_Control_Structure);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  This method loads the Status configuration from the Setting File.
 */
void Status__Config(void)
{
    SETTINGFILE_LOADER_TYPE aux_loader;
    uint16 data_size;

    Num_Status_Variables = 0;
    Status_Variables = NULL;

    if (SettingFile__BasicLoader(SF_PTR_UI_IO_CONFIG,5,&aux_loader) == PASS )
    {
        data_size = (uint8)aux_loader.Length;
        Num_Status_Variables = data_size / sizeof(STATUS_VARIABLE_TYPE);

        Status_Variables = (STATUS_VARIABLE_TYPE *)aux_loader.Data;     //lint !e927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
        Status_Configured = TRUE;   // This line must come before the call to Status__GetSize()

        Status_Variables_Size = StatusGetSize();
    }
    else
    {
        LOG_ADD_ERROR(STATUS_LOG_BAD_SETTINGFILE, 0);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method checks is the Received Status is valid.
 * @return
 */
BOOL_TYPE Status__IsValid(void)
{
    #if (RECEIVE_STATUS_VARIABLES_FEATURE == ENABLED)
        return ((BOOL_TYPE)(Status_Valid & Status_Configured));
    #else
        return (Status_Configured);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   This method returns the total size of the Status to be used in the Reveal Exposing status method.
 * @return  Size of the Status structure
 */
uint8 Status__GetSize(void)
{
    if (Status_Configured == TRUE)
    {
        return Status_Variables_Size;
    }
    else
    {
        return 0;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method builds a package with the data from the Status of the ACU.
 * @param ptr                       A Reference where the messages are build
 * @param variable_list_index_track This builds the message from the requested variable index
 * @param variables_pending         It returns TRUE if more variables are pending, FALSE if no more messages are pending
 * @param message_size              How many bytes data needs to be build in the message
 * @return
 *  @retval PASS(1) Successfully built the message
 *  @retval FAIL(0) Could not be able to built the message
 */
PASS_FAIL_TYPE Status__BuildPackage(REVEAL_COMMAND_TYPE * ptr, uint8 *variable_list_index_track, BOOL_TYPE *variables_pending, uint8 message_size)
{
    VARIABLE_TYPE to_fill_up;
    PASS_FAIL_TYPE status = FAIL;
    uint8 requested_variable_list_index = *variable_list_index_track;
    uint8 payload_index = 0;

    if (Status_Configured == TRUE)
    {
        for (; requested_variable_list_index < Num_Status_Variables; requested_variable_list_index++ )
        {
            if (Variable__GetByIndex((VARIABLE_ID_TYPE)Status_Variables[requested_variable_list_index].ID, &to_fill_up) == TRUE)
            {
                payload_index += VARIABLE_SIZE[to_fill_up.Type];

                if (message_size > payload_index)
                {
                    //Build the message
                    switch (to_fill_up.Type)
                    {
                        case DATATYPE_UINT8:
                        case DATATYPE_SINT8:
                            status = Reveal__AddDataU8(ptr, ((uint8 *)to_fill_up.Address)[Status_Variables[requested_variable_list_index].Offset]);
                            break;
                        case DATATYPE_UINT16:
                        case DATATYPE_SINT16:
                        case DATATYPE_IS_Q15_CELSIUS:
                            status = Reveal__AddDataU16(ptr, ((uint16 PACKED *)to_fill_up.Address)[Status_Variables[requested_variable_list_index].Offset]);
                            break;
                        case DATATYPE_UINT32:
                        case DATATYPE_SINT32:
                        case DATATYPE_FLOAT32:
                            status = Reveal__AddDataU32(ptr, ((uint32 PACKED *)to_fill_up.Address)[Status_Variables[requested_variable_list_index].Offset]);
                            break;
                        case DATATYPE_IS_ARRAY:
                            status = Reveal__AddDataU8Buffer(ptr, &((uint8 *)to_fill_up.Address)[Status_Variables[requested_variable_list_index].Offset *  VARIABLE_SIZE[to_fill_up.Type]], VARIABLE_SIZE[to_fill_up.Type]);
                            break;
                        default:
                            status = FAIL;
                            break;
                    }

                    if (status == FAIL) //Issue found during formation of the message
                    {
                        break;
                    }
                }
                else    //Reached to the maximum message size
                {
                    break;
                }
            }
        }


        if (Num_Status_Variables > requested_variable_list_index)
        {
            *variables_pending = TRUE;
        }
        else
        {
            *variables_pending = FALSE;
        }

        //update the variable list track
        *variable_list_index_track = requested_variable_list_index;
    }

    return status;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method builds a package with the data from the Status of the aCU.
 * @param ptr
 */
void Status__BuildStatusInfoPackage(REVEAL_COMMAND_TYPE * ptr)
{
    uint8 index;
    VARIABLE_TYPE to_fill_up;

    if (Status_Configured == TRUE)
    {
        Reveal__AddDataU8(ptr, Num_Status_Variables);

        for (index=0; index < Num_Status_Variables; index++ )
        {
            if (Variable__GetByIndex((VARIABLE_ID_TYPE)Status_Variables[index].ID, &to_fill_up) == TRUE)
            {
                Reveal__AddDataU8(ptr, Status_Variables[index].ID);
                Reveal__AddDataU8(ptr, to_fill_up.Type);
                Reveal__AddDataU8(ptr, to_fill_up.Length);
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Registers a status variable change event handler with the module.
 * @param   event_handler: pointer to the function that handles the event
 */
void Status__RegisterChangeEventHandler(CALLBACK_HANDLER_TYPE event_handler)
{
    Callback__Register(&StatusChange_Callback_Control_Structure, event_handler);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Unregisters a status variable change event handler with the module.
 * @param   event_handler: pointer to the function that handles the event
 */
void Status__UnregisterChangeEventHandler(CALLBACK_HANDLER_TYPE event_handler)
{
    Callback__Unregister(&StatusChange_Callback_Control_Structure, event_handler);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This module receives a package from Reveal and updates the variables on the Hmi with the data coming from the ACU.
 * @param variable_list_index_track: Variable_list_index_track provides which variables index shall needs to be updated
 * @param data: pointer to the variable data
 * @param variable_message_size: amount of byes in the variable data to be updated
 *
 * @return
 *  @retval PASS(1) Successfully built the message
 *  @retval FAIL(0) Could not be able to built the message
 */
void Status__SetACUStatus(uint8 variable_list_index_track, uint8 * data, uint8 variable_message_size)
{
    #if (RECEIVE_STATUS_VARIABLES_FEATURE == ENABLED)
        uint16 data16;
        uint32 data32;
        uint8 data_index = 0;   // Points to the first byte of data
        BOOL_TYPE variable_changed;
        VARIABLE_TYPE to_fill_up;

        if ((Status_Configured == TRUE) &&                      // Check that configuration of status variable has been read
            (variable_message_size == Status_Variables_Size))   // Check that the message matches the expected size to avoid writing corrupt/improper data to the variables
        {
            for (; (variable_list_index_track < Num_Status_Variables) && (variable_message_size > data_index); variable_list_index_track++ )
            {
                if (Variable__GetByIndex((VARIABLE_ID_TYPE)Status_Variables[variable_list_index_track].ID, &to_fill_up) == TRUE)
                {
                    variable_changed = FALSE;

                    switch (to_fill_up.Type)
                    {
                        case DATATYPE_UINT8:
                            if (((uint8 *)to_fill_up.Address)[Status_Variables[variable_list_index_track].Offset] != data[data_index])
                            {
                                variable_changed = TRUE;
                            }
                            ((uint8 *)to_fill_up.Address)[Status_Variables[variable_list_index_track].Offset] = data[data_index];
                            break;
                        case DATATYPE_SINT8:
                            if (((uint8 *)to_fill_up.Address)[Status_Variables[variable_list_index_track].Offset] != *((sint8 *)(&data[data_index]))) //lint !e926 cast from pointer to pointer [MISRA 2004 Rule 11.4]
                            {
                                variable_changed = TRUE;
                            }
                            ((uint8 *)to_fill_up.Address)[Status_Variables[variable_list_index_track].Offset] = *((sint8 *)(&data[data_index]));    //lint !e926 cast from pointer to pointer [MISRA 2004 Rule 11.4]
                            break;
                        case DATATYPE_UINT16:
                            if (((uint16 PACKED *)to_fill_up.Address)[Status_Variables[variable_list_index_track].Offset] != Utilities__ConvertArrayTo16bits(&data[data_index]))
                            {
                                variable_changed = TRUE;
                            }
                            ((uint16 PACKED *)to_fill_up.Address)[Status_Variables[variable_list_index_track].Offset] = Utilities__ConvertArrayTo16bits(&data[data_index]);
                            break;
                        case DATATYPE_SINT16:
                        case DATATYPE_IS_Q15_CELSIUS:
                            data16 = Utilities__ConvertArrayTo16bits(&data[data_index]);
                            if (((sint16 PACKED *)to_fill_up.Address)[Status_Variables[variable_list_index_track].Offset] != *((sint16 *)&data16)) //lint !e929 Lint: 929 cast from pointer to pointer [MISRA 2004 Rule 11.4]
                            {
                                variable_changed = TRUE;
                            }
                            ((sint16 PACKED *)to_fill_up.Address)[Status_Variables[variable_list_index_track].Offset] = *((sint16 *)&data16);       //lint !e929 cast from pointer to pointer [MISRA 2004 Rule 11.4]
                            break;
                        case DATATYPE_UINT32:
                            if (((uint32 PACKED *)to_fill_up.Address)[Status_Variables[variable_list_index_track].Offset] != Utilities__ConvertArrayTo32bits(&data[data_index]))
                            {
                                variable_changed = TRUE;
                            }
                            ((uint32 PACKED *)to_fill_up.Address)[Status_Variables[variable_list_index_track].Offset] = Utilities__ConvertArrayTo32bits(&data[data_index]);
                            break;
                        case DATATYPE_SINT32:
                            data32 = Utilities__ConvertArrayTo32bits(&data[data_index]);
                            if (((sint32 PACKED *)to_fill_up.Address)[Status_Variables[variable_list_index_track].Offset] != *((sint32 *)&data32)) //lint !e929 Lint: 929 cast from pointer to pointer [MISRA 2004 Rule 11.4]
                            {
                                variable_changed = TRUE;
                            }
                            ((sint32 PACKED *)to_fill_up.Address)[Status_Variables[variable_list_index_track].Offset] = *((sint32 *)&data32);       //lint !e929 cast from pointer to pointer [MISRA 2004 Rule 11.4]
                            break;
                        case DATATYPE_FLOAT32:
                            data32 = Utilities__ConvertArrayTo32bits(&data[data_index]);
                            if (((float32 PACKED *)to_fill_up.Address)[Status_Variables[variable_list_index_track].Offset] != *((float32 *)&data32)) //lint !e929 Lint: 929 cast from pointer to pointer [MISRA 2004 Rule 11.4]
                            {
                                variable_changed = TRUE;
                            }
                            ((float32 PACKED *)to_fill_up.Address)[Status_Variables[variable_list_index_track].Offset] = *((float32 *)&data32);     //lint !e929 cast from pointer to pointer [MISRA 2004 Rule 11.4]
                            break;
                        case DATATYPE_IS_ARRAY:
                            if (memcmp(&(((uint8 *)to_fill_up.Address)[Status_Variables[variable_list_index_track].Offset * VARIABLE_SIZE[to_fill_up.Type]]),&data[data_index],VARIABLE_SIZE[to_fill_up.Type]) != 0)
                            {
                                variable_changed = TRUE;
                            }
                            memcpy(&(((uint8 *)to_fill_up.Address)[Status_Variables[variable_list_index_track].Offset * VARIABLE_SIZE[to_fill_up.Type]]),&data[data_index],VARIABLE_SIZE[to_fill_up.Type]);
                            break;
                        default:
                            Status_Valid = FALSE;
                            LOG_ADD_ERROR(INVALID_DATA_TYPE, variable_list_index_track);
                            break;
                    }

                    Status_Valid = TRUE;

                    data_index += VARIABLE_SIZE[to_fill_up.Type];

                    if(variable_changed == TRUE)
                    {
                        Callback__Notify(&StatusChange_Callback_Control_Structure, (CALLBACK_EVENT_TYPE)Status_Variables[variable_list_index_track].ID); // Callback event for status variable change
                    }
                }
                else
                {
                    Status_Valid = FALSE;
                    LOG_ADD_ERROR(VARIABLE_MISSING, variable_list_index_track);
                    break;
                }

            }   //for loop for processing each variable loading
        }
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Function that compares the current crc of the Status variables and returns TRUE or FALSE to identify if any content has changed.
 * @return TRUE to tell the caller there was a change on the Status variables.
 */
BOOL_TYPE Status__HasChanged(void)
{
    uint16 crc_calc_16;
    uint8 index;
    BOOL_TYPE response;
    VARIABLE_TYPE to_fill_up;

    response = FALSE;

    if (Status_Configured == TRUE)
    {
        crc_calc_16 = 0xFFFF;

        for (index=0; index < Num_Status_Variables; index++ )
        {
            if (Variable__GetByIndex((VARIABLE_ID_TYPE)Status_Variables[index].ID, &to_fill_up) == TRUE)
            {
                crc_calc_16 = Crc16__RangeProcess(&((uint8 *)to_fill_up.Address)[Status_Variables[index].Offset *  VARIABLE_SIZE[to_fill_up.Type]], crc_calc_16, VARIABLE_SIZE[to_fill_up.Type]);
            }
        }

        if (crc_calc_16 != Crc_Calc_16_Prev)
        {
        	Crc_Calc_16_Prev = crc_calc_16;
            response = TRUE;
        }
    }
    return response;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Search for the specified variable ID in the status variables.
 * @param var_id ID of the variable to be checked if is status variable.
 * @return FALSE/TRUE depending on variable not found or found.
 * @retval FALSE variable is not status variable.
 * @retval TRUE variable is status variable.
 */
BOOL_TYPE Status__IsStatusVariable(VARIABLE_ID_TYPE var_id)
{
	BOOL_TYPE search_result = FALSE;

	if(TRUE == Status__IsValid())
	{
		for(uint8 i = 0; i < Num_Status_Variables; i++)
		{
			if((VARIABLE_ID_TYPE)Status_Variables[i].ID == var_id)
			{
				search_result = TRUE;
				break;
			}
		}
	}

	return search_result;
}
//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   This method returns the total size of the Status to be used in the Reveal Exposing status method.
 * @return  Size of the Status structure
 */
static uint8 StatusGetSize(void)
{
    uint8 size;
    uint8 index;
    VARIABLE_TYPE to_fill_up;

    size = 0;
    if (Status_Configured == TRUE)
    {
        for (index=0; index < Num_Status_Variables; index++ )
        {
            if (Variable__GetByIndex((VARIABLE_ID_TYPE)Status_Variables[index].ID, &to_fill_up) == TRUE)
            {
                size += VARIABLE_SIZE[to_fill_up.Type];
            }
        }
    }
    return (size);
}

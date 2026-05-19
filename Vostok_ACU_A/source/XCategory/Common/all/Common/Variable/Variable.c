/**
 *  @file
 *
 *  @brief      Module that stores references and definitions to the meaningful variables on the platform.
 *
 *  @details    This is a hub module from where you can access variables using an index. To make it happen you need to configure the PRV file with
 *  			the proper data: It's necessary to declare extern variables that you'll add to DATA_HOLDER_INITIALIZE. On this constant, you add the elements
 *  			using DATATYPE_TYPE as reference. Fill up all fields.
 *  			Important: this module manages 8 and 16 bits variables and Regulations using the VARIABLE_REGULATION_HUB position and the Regulation module.
 *
 *  @copyright  Copyright 2015-2016 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"
#include <string.h>
#include "Variable.h"
#include "Variable_prv.h"
// Protecting the include. If we are on an expansion board, we don't need the variable module to compile.
#if ((SYSTEM_ACU_HANDLING == ENABLED) || (SYSTEM_HMI_HANDLING == ENABLED))
    #include "Variable_defs.h"
#endif

#include "Log.h"
#if (VARIABLES_REGULATIONS_FEATURE == ENABLED)
    #include "Regulations.h"
#endif

#ifndef VISUAL_MACRO_VARIABLES_FEATURE
    #define VISUAL_MACRO_VARIABLES_FEATURE DISABLED
#elif (VISUAL_MACRO_VARIABLES_FEATURE == ENABLED)
    #include "HmiVariable.h"
#endif

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//! Array of sizes for data types defined in C_Extensions.
uint8 const VARIABLE_SIZE[] =
{
   1,          //!< DATATYPE_INVALID
   1,          //!< DATATYPE_UINT8
   2,          //!< DATATYPE_UINT16
   4,          //!< DATATYPE_UINT32
   8,          //!< DATATYPE_UINT64
   1,          //!< DATATYPE_SINT8
   2,          //!< DATATYPE_SINT16
   4,          //!< DATATYPE_SINT32
   8,          //!< DATATYPE_SINT64
   4,          //!< DATATYPE_FLOAT32
   8,          //!< DATATYPE_FLOAT64
   1,          //!< DATATYPE_IS_ARRAY
   1,          //!< DATATYPE_IS_REGULATION
   2		   //!< DATATYPE_IS_Q15_CELSIUS
};
#if ((SYSTEM_ACU_HANDLING == ENABLED) || (SYSTEM_HMI_HANDLING == ENABLED))
	/**
	 * Defines how many different variables this module will manage.
	 */
	#define VARIABLE_MAX_NUMBER_OF_ACU_VARIABLES   ELEMENTS_IN_ARRAY(ACU_DATA_HOLDER)
    #define VARIABLE_MAX_NUMBER_OF_HMI_VARIABLES   ELEMENTS_IN_ARRAY(HMI_DATA_HOLDER)


	// The VARIABLE_NUM_ACU_VARIABLES is a define in Variables_prm.h which is public , required by API010PollVar
	CT_ASSERT (VARIABLE_NUM_ACU_VARIABLES == VARIABLE_MAX_NUMBER_OF_ACU_VARIABLES);
	
	//There is a requirement to limit the number of fixed variables you can expose to 300.
	CT_ASSERT (VARIABLE_MAX_NUMBER_OF_ACU_VARIABLES < 200);
#endif
// To warn and advice what to do.
// To assert that the Regulation HUB is index Zero on ACU_DATA_HOLDER.
// Define VARIABLE_REGULATION_HUB on Variable_prm.h and Regulation_Dummy shall be on the ACU_DATA_HOLDER position Zero.
CT_ASSERT(VARIABLE_REGULATION_HUB == 0);

// Protecting the function calls when the board isn't a main HMI or ACU
#if ((SYSTEM_ACU_HANDLING != ENABLED) && (SYSTEM_HMI_HANDLING != ENABLED))
	BOOL_TYPE     Variable__GetByIndex(VARIABLE_ID_TYPE index, VARIABLE_TYPE *to_fill_up)
	{
		return FALSE;
	}
	BOOL_TYPE     Variable__GetValue(VARIABLE_ID_TYPE index, uint8 offset, sint32 *value)
	{
		return FALSE;
	}
	BOOL_TYPE     Variable__GetTranslatedValue(VARIABLE_ID_TYPE index, uint8 offset, sint32 *value)
	{
		return FALSE;
	}
	DATATYPE_TYPE Variable__GetTypeByIndex(VARIABLE_ID_TYPE index)
	{
		return DATATYPE_INVALID;
	}
	BOOL_TYPE	  Variable__SetValue(VARIABLE_SET_OPERATION_TYPE operation, VARIABLE_ID_TYPE index, uint8 offset, sint32 value)
	{
		return FALSE;
	}
#else

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void SetVariableValue(VARIABLE_ID_TYPE index, uint8 offset, sint32 value);
static void SetVariableMathValue(VARIABLE_ID_TYPE index, uint8 offset, sint32 value, VARIABLE_SET_OPERATION_TYPE operation);

static BOOL_TYPE ConvertToSint32(VARIABLE_ID_TYPE index, uint8 offset, sint32 *value);
#if(VARIABLES_HAVE_KVP_ID == ENABLED)
	static BOOL_TYPE GetVariablePositionByKVP(uint32 kvp_id, VARIABLE_ID_TYPE *index, uint8 *offset);
#endif

//-------------------------------------- IDs FOR LOG messages----------------------------------------------------------
//! IDs FOR LOG messages
typedef enum
{
	BOUNDARY_CHECK_INDEX = 0,
	BOUNDARY_CHECK_OFFSET,
	BOUNDARY_1_CHECK_INDEX,
	INVALID_INDEX,
	INVALID_SETVARIABLE,
	REGULATION_ERROR,
	DATATYPE_ERROR,
	POINTER_1_NULL,
	POINTER_2_NULL,
	BOUNDARY_CHECK_FOR_ESTIMATE,
	OFFSET_CHECK_FOR_ESTIMATE,
	NULL_POINTER_FOR_ESTIMATE,
	NULL_POINTERS_ON_KVP,
	NOT_WRITABLE,
	ENCOUNTER_DIVIDE_BY_ZERO,
	INVALID_MATH_OPERATION,
	VARIABLES_FEATURE_DISABLED
} MODULE_VARIABLE_MESSAGE_ID_TYPE;
//! ID for this module on the Log module.
#define MODULE_NAME		MODULE_VARIABLE
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Getter for the variable with the desired index.
 * You must pass the reference for the structure where you want this function to load the information
 * about the variable you want.
 * It will return the Variable info using the index you pass as reference. It'll use offset equals zero.
 * @param index			index that identifies the variable you want to get.
 * @param to_fill_up	reference of the variable you want to fill up with the info.
 * @return	TRUE means the parameters where loaded, FALSE otherwise.
 */
BOOL_TYPE Variable__GetByIndex(VARIABLE_ID_TYPE index, VARIABLE_TYPE *to_fill_up)
{
	BOOL_TYPE ret;

	ret = FALSE;
	if(to_fill_up != NULL)
	{
	    if(index < VARIABLE_MAX_NUMBER_OF_ACU_VARIABLES)  //ACU variables
	    {
	        memcpy(to_fill_up, &ACU_DATA_HOLDER[index], sizeof(VARIABLE_TYPE));
	        //
	        ret = TRUE;
	    }
    #if(HMI_VARIABLE_FEATURE == ENABLED)
	    else if((index > VARIABLE_INVALID_INDEX) && (index < VARIABLE_END_HMI_VARIABLES))  //HMI variables
	    {
	        memcpy(to_fill_up, &HMI_DATA_HOLDER[index- VARIABLE_HMI_START_INDEX], sizeof(VARIABLE_TYPE));
	        ret = TRUE;
	    }
    #endif
    #if(VISUAL_MACRO_VARIABLES_FEATURE == ENABLED)         //Visual macro variables
	    else if((index >= VARIABLE_HMI_RESERVED_INDEX) && (index < VARIABLE_INVALID_INDEX))
	    {
	        index -=(VARIABLE_ID_TYPE)VARIABLE_HMI_RESERVED_INDEX;
	        memcpy(to_fill_up, &VISUAL_MACRO_VARIABLES_DATA_HOLDER[index], sizeof(VARIABLE_TYPE));
	        ret = TRUE;
	    }
    #endif
	}
	return (ret);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Getter that returns the value of a variable (no translation if the variable IS_REGULATION). Adding the Capability of navigating on arrays.
 * @param index   Variable Index (to access ACU_DATA_HOLDER using the Variable module)
 * @param offset  Variable Offset(to access ACU_DATA_HOLDER using the Variable module)
 * @param pointer where the function will load the value converted to sint32.
 * @return TRUE means all was okay. FALSE tells to look @ log data to identify the issue.
 */
BOOL_TYPE Variable__GetValue(VARIABLE_ID_TYPE index, uint8 offset, sint32 *value)
{
	BOOL_TYPE result = FALSE;

	if(value == NULL)
	{
		LOG_ADD_EXCEPTION(POINTER_1_NULL, index);
	}
	else
	{
	    if(index == VARIABLE_REGULATION_HUB)
	    {
        #if (VARIABLES_REGULATIONS_FEATURE == ENABLED)
	        uint8 aux = Regulations__GetIndexValueByPosition(offset);
	        if( aux == REGULATION_INVALID)
	        {
	            LOG_ADD_EXCEPTION(REGULATION_ERROR, offset);
	        }
	        else
	        {
	            *value = (sint32) aux;
	            result = TRUE;
	        }
        #else
	        LOG_ADD_EXCEPTION(VARIABLES_FEATURE_DISABLED, index);
        #endif
	    }
	    else if(index < VARIABLE_MAX_NUMBER_OF_ACU_VARIABLES)   //Index for an ACU variable   (0 - 251)
	    {
	        if(offset < ACU_DATA_HOLDER[index].Length)          // the offset requested is valid *lesser than the array length.
	        {
	            result = ConvertToSint32(index, offset, value);
	        }
	        else
	        {
	            LOG_ADD_EXCEPTION(BOUNDARY_CHECK_OFFSET, (index << 8 | offset));
	        }
	    }
	    else if((index >= VARIABLE_HMI_TIME_ESTIMATION)&&(index < VARIABLE_INVALID_INDEX))  // (252 - 254)
	    {
	        #if(VISUAL_MACRO_VARIABLES_FEATURE == ENABLED)
                index -= (VARIABLE_ID_TYPE)VARIABLE_HMI_RESERVED_INDEX;
                result = HmiVariable__Get(index, offset, value);
            #else
                LOG_ADD_EXCEPTION(VARIABLES_FEATURE_DISABLED, index);
            #endif
	    }
	    else if((index > VARIABLE_INVALID_INDEX) && (index < VARIABLE_END_HMI_VARIABLES)) // (256 - 511)
	    {
	        if(offset < HMI_DATA_HOLDER[index - VARIABLE_HMI_START_INDEX].Length)          // the offset requested is valid *lesser than the array length.
            {
                result = ConvertToSint32(index, offset, value);
            }
            else
            {
                LOG_ADD_EXCEPTION(BOUNDARY_CHECK_OFFSET, (index << 8 | offset));
            }
	    }
        else                                               
        {
            LOG_ADD_EXCEPTION(BOUNDARY_CHECK_INDEX, index);
        }
	}

	return result;
}
/**
 * Getter that returns the translated value (translated in case the variable type requires it) of a variable content. Adding the Capability of navigating on arrays.
 * @param index		Variable Index (to access ACU_DATA_HOLDER using the Variable module)
 * @param offset	Variable Offset(to access ACU_DATA_HOLDER using the Variable module)
 * @param pointer where the function will load the value converted to sint32.
 * @return TRUE means all was okay. FALSE tells to look @ log data to identify the issue.
 */
BOOL_TYPE Variable__GetTranslatedValue (VARIABLE_ID_TYPE index, uint8 offset, sint32 *value)
{
	BOOL_TYPE result;

	result = FALSE;
#if (VARIABLES_REGULATIONS_FEATURE == ENABLED)
	if(value != NULL)
	{
		// If the index is valid we check for Regulation translation or the raw value.
		if((index < VARIABLE_MAX_NUMBER_OF_ACU_VARIABLES)||((index < VARIABLE_END_HMI_VARIABLES) && (index > VARIABLE_INVALID_INDEX)))
		{
			// If the type is DATATYPE_IS_REGULATION, we solve using Regulation module...otherwise we get the raw.
			if(index == VARIABLE_REGULATION_HUB)
			{
				*value = Regulations__GetMainValueByPosition(offset);
				result = TRUE;
			}
			else
			{
				result = Variable__GetValue(index, offset, value);
			}
		}
		else
		{
			LOG_ADD_EXCEPTION(BOUNDARY_1_CHECK_INDEX, index);
		}
	}
	else
	{
		LOG_ADD_EXCEPTION(POINTER_2_NULL, index);
	}
#else
	LOG_ADD_EXCEPTION(VARIABLES_FEATURE_DISABLED, index);
#endif
	return result;
}
/**
 * Getter to the type of a variable by index.
 * @param index	to one line on the ACU_DATA_HOLDER array.
 * @return	type of the variable on ACU_DATA_HOLDER.
 */
DATATYPE_TYPE Variable__GetTypeByIndex  (VARIABLE_ID_TYPE index)
{
	DATATYPE_TYPE result;

	result = DATATYPE_INVALID;

	if(index < VARIABLE_MAX_NUMBER_OF_ACU_VARIABLES)
	{
		return ACU_DATA_HOLDER[index].Type;
	}
#if(HMI_VARIABLE_FEATURE == ENABLED)
	else if((index > VARIABLE_INVALID_INDEX) && (index < VARIABLE_END_HMI_VARIABLES))
	{
	    return HMI_DATA_HOLDER[index- VARIABLE_HMI_START_INDEX].Type;
	}
#endif
#if(VISUAL_MACRO_VARIABLES_FEATURE == ENABLED)
    else if((index >= VARIABLE_HMI_RESERVED_INDEX) && (index < VARIABLE_INVALID_INDEX))
    {
        index -= (VARIABLE_ID_TYPE)VARIABLE_HMI_RESERVED_INDEX;
        return VISUAL_MACRO_VARIABLES_DATA_HOLDER[index].Type;
    }
#endif
	return result;
}
/**
 * It performs a simple value attribution Variable = Operand.
 * Does not accept writing on Regulations (VARIABLE_REGULATION_HUB).
 * @param operation Info to the operation to be performed.
 * @param index		Variable Index (to access ACU_DATA_HOLDER using the Variable module)
 * @param offset	Variable Offset(to access ACU_DATA_HOLDER using the Variable module)
 * @param value		Depending on the operation it can be a value to use on a math or a position to set or clear a bit.
 * @return	FALSE if index is not pointing to any variable, if offset is also wrong or operation is invalid or not accepted.
 */
BOOL_TYPE Variable__SetValue (VARIABLE_SET_OPERATION_TYPE operation, VARIABLE_ID_TYPE index, uint8 offset, sint32 value)
{
	BOOL_TYPE result;

	result = TRUE;

	if((index == VARIABLE_REGULATION_HUB) || (operation >= VARIABLE_OPERATION_COUNT))
	{
		LOG_ADD_EXCEPTION(INVALID_INDEX, (operation << 16 | index << 8 | offset));
		result = FALSE;
	}
#if(VISUAL_MACRO_VARIABLES_FEATURE == ENABLED)
	else if((index >= VARIABLE_HMI_RESERVED_INDEX) && (index < VARIABLE_INVALID_INDEX)) //HMI variable Index
	{
	    index -= (VARIABLE_ID_TYPE)VARIABLE_HMI_RESERVED_INDEX;
	    result = HmiVariable__Set(index,offset,value);
	}
#endif
	else if ((index >= VARIABLE_END_HMI_VARIABLES) ||
	        ((index >= VARIABLE_MAX_NUMBER_OF_ACU_VARIABLES) &&
	            (index <= VARIABLE_INVALID_INDEX)))
	{
        LOG_ADD_EXCEPTION(INVALID_INDEX, (operation << 16 | index << 8 | offset));
        result = FALSE;
    }
    else if ((index < VARIABLE_MAX_NUMBER_OF_ACU_VARIABLES) &&
        (offset >= ACU_DATA_HOLDER[index].Length))
    {
        LOG_ADD_EXCEPTION(INVALID_INDEX, (operation << 16 | index << 8 | offset));
        result = FALSE;
    }
    else if ((index >= VARIABLE_HMI_START_INDEX) &&
        (offset >= HMI_DATA_HOLDER[index - VARIABLE_HMI_START_INDEX].Length))
    {
        LOG_ADD_EXCEPTION(INVALID_INDEX, (operation << 16 | index << 8 | offset));
        result = FALSE;
    }
    else if((index < VARIABLE_MAX_NUMBER_OF_ACU_VARIABLES) &&
        (ACU_DATA_HOLDER[index].Is_Writable == FALSE))
    {
        LOG_ADD_EXCEPTION(NOT_WRITABLE, (index << 8 | offset));
        result = FALSE;
    }
    else if ((index >= VARIABLE_HMI_START_INDEX) &&
        (HMI_DATA_HOLDER[index - VARIABLE_HMI_START_INDEX].Is_Writable == FALSE))
     {
        LOG_ADD_EXCEPTION(NOT_WRITABLE, (index << 8 | offset));
        result = FALSE;
     }

	else                                                                                //ACU variable index
	{
		switch(operation)
		{
			case VARIABLE_OPERATION_SET 		:
				SetVariableValue(index, offset, value);
				break;
			case VARIABLE_OPERATION_ADD			:
			    SetVariableMathValue(index, offset, value, VARIABLE_OPERATION_ADD);
				break;
			case VARIABLE_OPERATION_SUBTRACT	:
                SetVariableMathValue(index, offset, value, VARIABLE_OPERATION_SUBTRACT);
				break;
			case VARIABLE_OPERATION_INCREMET	:
			    SetVariableMathValue(index, offset, 1, VARIABLE_OPERATION_ADD);
				break;
			case VARIABLE_OPERATION_DECREMENT	:
			    SetVariableMathValue(index, offset, 1, VARIABLE_OPERATION_SUBTRACT);
				break;
			case VARIABLE_OPERATION_BIT_SET		:
			    if(index < VARIABLE_NUM_ACU_VARIABLES)
			    {
                    switch (ACU_DATA_HOLDER[index].Type)
                    {
                        case DATATYPE_UINT32:
                            BIT_SET(((uint32*)ACU_DATA_HOLDER[index].Address)[offset], (uint8)value);
                            break;
                        case DATATYPE_UINT16:
                            BIT_SET16(((uint16*)ACU_DATA_HOLDER[index].Address)[offset], (uint8)value);
                            break;
                        case DATATYPE_UINT8:
                            BIT_SET8(((uint8*)ACU_DATA_HOLDER[index].Address)[offset], (uint8)value);
                            break;
                        default:
                            result = FALSE;
                            break;
                    }
			    }
			    else
			    {
			        switch (HMI_DATA_HOLDER[index-VARIABLE_HMI_START_INDEX].Type)
                    {
                        case DATATYPE_UINT32:
                            BIT_SET(((uint32*)HMI_DATA_HOLDER[index-VARIABLE_HMI_START_INDEX].Address)[offset], (uint8)value);
                            break;
                        case DATATYPE_UINT16:
                            BIT_SET16(((uint16*)HMI_DATA_HOLDER[index-VARIABLE_HMI_START_INDEX].Address)[offset], (uint8)value);
                            break;
                        case DATATYPE_UINT8:
                            BIT_SET8(((uint8*)HMI_DATA_HOLDER[index-VARIABLE_HMI_START_INDEX].Address)[offset], (uint8)value);
                            break;
                        default:
                            result = FALSE;
                            break;
                    }
			    }
				break;
			case VARIABLE_OPERATION_BIT_CLEAR	:
			    if(index < VARIABLE_NUM_ACU_VARIABLES)
			    {
                    switch (ACU_DATA_HOLDER[index].Type)
                    {
                        case DATATYPE_UINT32:
                            BIT_CLR(((uint32*)ACU_DATA_HOLDER[index].Address)[offset], (uint8)value);
                            break;
                        case DATATYPE_UINT16:
                            BIT_CLR16(((uint16*)ACU_DATA_HOLDER[index].Address)[offset], (uint8)value);
                            break;
                        case DATATYPE_UINT8:
                            BIT_CLR8(((uint8*)ACU_DATA_HOLDER[index].Address)[offset], (uint8)value);
                            break;
                        default:
                            result = FALSE;
                            break;
                    }
			    }
			    else
			    {
			        switch (HMI_DATA_HOLDER[index-VARIABLE_HMI_START_INDEX].Type)
                    {
                        case DATATYPE_UINT32:
                            BIT_CLR(((uint32*)HMI_DATA_HOLDER[index-VARIABLE_HMI_START_INDEX].Address)[offset], (uint8)value);
                            break;
                        case DATATYPE_UINT16:
                            BIT_CLR16(((uint16*)HMI_DATA_HOLDER[index-VARIABLE_HMI_START_INDEX].Address)[offset], (uint8)value);
                            break;
                        case DATATYPE_UINT8:
                            BIT_CLR8(((uint8*)HMI_DATA_HOLDER[index-VARIABLE_HMI_START_INDEX].Address)[offset], (uint8)value);
                            break;
                        default:
                            result = FALSE;
                            break;
                    }
			    }
				break;

            case VARIABLE_OPERATION_MULTIPLY         :
                SetVariableMathValue(index, offset, value, VARIABLE_OPERATION_MULTIPLY);
                break;

            case VARIABLE_OPERATION_DIVIDE         :
                SetVariableMathValue(index, offset, value, VARIABLE_OPERATION_DIVIDE);
                break;

			default:
				result = FALSE;
				break;
		}
	}

	return result;
}

//=====================================================================================================================
//-------------------------------------- Getters and Setters using KVP Ids---------------------------------------------
//=====================================================================================================================
#if(VARIABLES_HAVE_KVP_ID == ENABLED)
	/**
	 * Getter to the content of the variable. Access it using Kvp_ID.
	 * @param kvp_id	32 bits ID for the variable.
	 * @param value		value of the variable. if the return of the function is TRUE this value was retrieved properly.
	 * @return	TRUE means the value is valid: it was possible to retrieve it. FALSE means you should NOT use the value.
	 */
	BOOL_TYPE Variable__GetValuebyKVP(uint32 kvp_id, sint32 *value)
	{
		VARIABLE_ID_TYPE index;
		uint8 offset;

		if(GetVariablePositionByKVP(kvp_id, &index, &offset) == TRUE)
		{
			return Variable__GetValue(index, offset, value);
		}
		return FALSE;
	}
	/**
	 * Getter to the DATATYPE of the variable referenced by the KVP_ID
	 * @param kvp_id 32 bits ID for the variable.
	 * @return TRUE means it was possible to retrieve the DATATYPE_TYPE for the given index and offset. FALSE means no valid return.
	 */
	DATATYPE_TYPE Variable__GetTypeByKVP(uint32 kvp_id)
	{
		VARIABLE_ID_TYPE index;
		uint8 offset;

		if(GetVariablePositionByKVP(kvp_id, &index, &offset) == TRUE)
		{
			return Variable__GetTypeByIndex(index);
		}
		return DATATYPE_INVALID;
	}
	/**
	 * Setter to the value of the variable referenced by the given KVP_ID.
	 * @param operation one of the possible operations to be performed on a variable.
	 * @param kvp_id 32 bits ID for the variable.
	 * @param value 32 bits value you want to set. For Increment/Decrement operations the value is not used. you can pass any constant.
	 * @return TRUE if the requested operation was performed. FALSE means it was not possible and reason will be on the Log.
	 */
	BOOL_TYPE Variable__SetValueByKVP(VARIABLE_SET_OPERATION_TYPE operation, uint32 kvp_id, sint32 value)
	{
		VARIABLE_ID_TYPE index;
		uint8 offset;

		if(GetVariablePositionByKVP(kvp_id, &index, &offset) == TRUE)
		{
			return Variable__SetValue(operation, index, offset, value);
		}
		return FALSE;
	}
	/**
		 * It receives the index for the variable and returns the related KVP.
		 * @param variable_index as per the constants defined on this module.
		 * @return The KVP Index for the variable or FFFFFFFF if the variable is not found.
		 */
		uint32 Variable__GetKVPByIndex(VARIABLE_ID_TYPE variable_index)
		{
			uint32 kvp_id;
			uint8 key;

			kvp_id = 0xFFFFFFFF;
			for(key = 0; key < ELEMENTS_IN_ARRAY(Kvp_To_Variable_Table); key++)
			{
				if(variable_index == Kvp_To_Variable_Table[key].Variable_Index)
				{
					kvp_id = Kvp_To_Variable_Table[key].Kvp_Id;
					key = ELEMENTS_IN_ARRAY(Kvp_To_Variable_Table);
				}
			}
			return kvp_id;
		}
	/**
	 * Function that returns the Index and Offset of the variable on the ACU_DATA_HOLDER array.
	 * It receives the KVP ID and if it is present on the KVP_TO_VARIABLE_TRANSLATION_TABLE, we return the index and offset to that variable.
	 * @param kvp_id	unique uint32 index for the variable. It is used for connectivity.
	 * @param *index	it will be loaded with the Index for the Variable only if the function returns TRUE this is a valid Index.
	 * @param *offset	it will be loaded with the Offset for the Variable only if the function returns TRUE this is a valid Offset.
	 * @return TRUE if we were able to retrieve the information.
	 */
	static BOOL_TYPE GetVariablePositionByKVP(uint32 kvp_id, VARIABLE_ID_TYPE *index, uint8 *offset)
	{
		BOOL_TYPE ret;
		uint8 key;

		ret = FALSE;
		// If we are receiving valid addresses to load with the info, we proceed.
		if((index == NULL) || (offset == NULL))
		{
			LOG_ADD_EXCEPTION(NULL_POINTERS_ON_KVP, kvp_id);
		}
		else
		{
			for(key = 0; key < ELEMENTS_IN_ARRAY(Kvp_To_Variable_Table); key++)
			{
				if(kvp_id == Kvp_To_Variable_Table[key].Kvp_Id)
				{
					if(Kvp_To_Variable_Table[key].Variable_Index < VARIABLE_MAX_NUMBER_OF_ACU_VARIABLES)
					{
						ret = TRUE;
						*index  = Kvp_To_Variable_Table[key].Variable_Index;
						*offset = Kvp_To_Variable_Table[key].Offset;
					}
				}
			}
		}
		return ret;
	}
#endif

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
/**
 * Function that sets the given value to a variable on the ACU_DATA_HOLDER structure.
 * @param index		Variable Index (to access ACU_DATA_HOLDER using the Variable module)
 * @param offset	Variable Offset(to access ACU_DATA_HOLDER using the Variable module)
 * @param value		new value to the variable.
 */
static void SetVariableValue(VARIABLE_ID_TYPE index, uint8 offset, sint32 value)
{
    if(index < VARIABLE_NUM_ACU_VARIABLES)
    {
        switch(ACU_DATA_HOLDER[index].Type)
        {
            case DATATYPE_UINT8	   :
                ((uint8 *) ACU_DATA_HOLDER[index].Address)[offset] = (uint8) value;
                break;
            case DATATYPE_SINT8    :
                ((sint8 *) ACU_DATA_HOLDER[index].Address)[offset] = (sint8) value;
                break;
            case DATATYPE_UINT16   :
                ((uint16 *) ACU_DATA_HOLDER[index].Address)[offset] = (uint16) value;
                break;
            case DATATYPE_IS_Q15_CELSIUS:
                //lint -fallthrough
            case DATATYPE_SINT16   :
                ((sint16*) ACU_DATA_HOLDER[index].Address)[offset] = (sint16) value;
                break;
            case DATATYPE_UINT32   :
                ((uint32*) ACU_DATA_HOLDER[index].Address)[offset] = (uint32) value;
                break;
            case DATATYPE_SINT32   :
                ((sint32*) ACU_DATA_HOLDER[index].Address)[offset] = value;
                break;
            default:
                memcpy(&((uint8*)ACU_DATA_HOLDER[index].Address)[offset], &value, VARIABLE_SIZE[ACU_DATA_HOLDER[index].Type]);
                LOG_ADD_EXCEPTION(DATATYPE_ERROR, ACU_DATA_HOLDER[index].Type);
                break;
	    }
    }

    else if((index > VARIABLE_INVALID_INDEX) && (index < VARIABLE_END_HMI_VARIABLES))
    {
        switch(HMI_DATA_HOLDER[index-VARIABLE_HMI_START_INDEX].Type)
        {
            case DATATYPE_UINT8    :
                ((uint8 *) HMI_DATA_HOLDER[index-VARIABLE_HMI_START_INDEX].Address)[offset] = (uint8) value;
                break;
            case DATATYPE_SINT8    :
                ((sint8 *) HMI_DATA_HOLDER[index-VARIABLE_HMI_START_INDEX].Address)[offset] = (sint8) value;
                break;
            case DATATYPE_UINT16   :
                ((uint16 *) HMI_DATA_HOLDER[index-VARIABLE_HMI_START_INDEX].Address)[offset] = (uint16) value;
                break;
            case DATATYPE_IS_Q15_CELSIUS:
                //lint -fallthrough
            case DATATYPE_SINT16   :
                ((sint16*) HMI_DATA_HOLDER[index-VARIABLE_HMI_START_INDEX].Address)[offset] = (sint16) value;
                break;
            case DATATYPE_UINT32   :
                ((uint32*) HMI_DATA_HOLDER[index-VARIABLE_HMI_START_INDEX].Address)[offset] = (uint32) value;
                break;
            case DATATYPE_SINT32   :
                ((sint32*) HMI_DATA_HOLDER[index-VARIABLE_HMI_START_INDEX].Address)[offset] = value;
                break;
            default:
                memcpy(&((uint8*)HMI_DATA_HOLDER[index-VARIABLE_HMI_START_INDEX].Address)[offset], &value, VARIABLE_SIZE[HMI_DATA_HOLDER[index].Type]);
                LOG_ADD_EXCEPTION(DATATYPE_ERROR, HMI_DATA_HOLDER[index].Type);
                break;
        }
    }

}

/**
 * Function that performs the math(which are Add, Subtract, Multiply or Divide) a value to a variable.
 * @param index     Variable Index (to access ACU_DATA_HOLDER using the Variable module)
 * @param offset    Variable Offset(to access ACU_DATA_HOLDER using the Variable module)
 * @param value     value to be added.
 */
static void SetVariableMathValue(VARIABLE_ID_TYPE index, uint8 offset, sint32 value, VARIABLE_SET_OPERATION_TYPE operation)
{
    sint32 intermediate;
    if(index <= VARIABLE_MAX_NUMBER_OF_ACU_VARIABLES)
    {
        if(offset >= ACU_DATA_HOLDER[index].Length)
        {
            LOG_ADD_EXCEPTION(INVALID_SETVARIABLE, index << 8 | offset);
        }
        else
        {
            ConvertToSint32(index, offset, &intermediate);

            switch(operation)
            {
                case VARIABLE_OPERATION_ADD:
                    intermediate += value;
                    break;

                case VARIABLE_OPERATION_SUBTRACT:
                    intermediate -= value;
                    break;

                case VARIABLE_OPERATION_MULTIPLY:
                    intermediate *= value;
                    break;

                case VARIABLE_OPERATION_DIVIDE:
                    if (value != 0)
                    {
                        intermediate /= value;
                    }
                    else
                    {
                        LOG_ADD_EXCEPTION(ENCOUNTER_DIVIDE_BY_ZERO, index << 8 | offset);
                    }
                    break;

                default:
                    LOG_ADD_EXCEPTION(INVALID_MATH_OPERATION, index << 8 | offset);
                    break;
            }
            SetVariableValue(index, offset, intermediate);
        }
    }
    else if((index > VARIABLE_INVALID_INDEX) && (index < VARIABLE_END_HMI_VARIABLES))
    {
        if(offset >= HMI_DATA_HOLDER[index-VARIABLE_HMI_START_INDEX].Length)
        {
            LOG_ADD_EXCEPTION(INVALID_SETVARIABLE, index << 8 | offset);
        }
        else
        {
            ConvertToSint32(index, offset, &intermediate);

            switch(operation)
            {
                case VARIABLE_OPERATION_ADD:
                    intermediate += value;
                    break;

                case VARIABLE_OPERATION_SUBTRACT:
                    intermediate -= value;
                    break;

                case VARIABLE_OPERATION_MULTIPLY:
                    intermediate *= value;
                    break;

                case VARIABLE_OPERATION_DIVIDE:
                    if (value != 0)
                    {
                        intermediate /= value;
                    }
                    else
                    {
                        LOG_ADD_EXCEPTION(ENCOUNTER_DIVIDE_BY_ZERO, index << 8 | offset);
                    }
                    break;

                default:
                    LOG_ADD_EXCEPTION(INVALID_MATH_OPERATION, index << 8 | offset);
                    break;
            }
            SetVariableValue(index, offset, intermediate);
        }
    }
    else
    {
        LOG_ADD_EXCEPTION(INVALID_SETVARIABLE, index << 8 | offset);
    }
}

/**
 * A little function to convert the value of a variable to sint32 to use on the other local functions.
 * @return the value of the variable converted to sint32.
 */
static BOOL_TYPE ConvertToSint32(VARIABLE_ID_TYPE index, uint8 offset, sint32 *value)
{
	BOOL_TYPE result = TRUE;
	if(index < VARIABLE_NUM_ACU_VARIABLES )
	{
        switch(ACU_DATA_HOLDER[index].Type)
        {
            case DATATYPE_UINT8	   :
                *value = (sint32) ((uint8*) ACU_DATA_HOLDER[index].Address)[offset];
                break;
            case DATATYPE_SINT8    :
                *value = (sint32) ((sint8*) ACU_DATA_HOLDER[index].Address)[offset];
                break;
            case DATATYPE_UINT16   :
                *value = (sint32) ((uint16*) ACU_DATA_HOLDER[index].Address)[offset];
                break;
            case DATATYPE_IS_Q15_CELSIUS:
                //lint -fallthrough
            case DATATYPE_SINT16   :
                *value  = (sint32) ((sint16*) ACU_DATA_HOLDER[index].Address)[offset];
                break;
            case DATATYPE_UINT32   :
                *value  = (sint32) ((uint32*) ACU_DATA_HOLDER[index].Address)[offset];
                break;
            case DATATYPE_SINT32   :
                *value  = ((sint32*) ACU_DATA_HOLDER[index].Address)[offset];
                break;
            default:
                result = FALSE;
                LOG_ADD_EXCEPTION(DATATYPE_ERROR, ACU_DATA_HOLDER[index].Type);
                break;
        }
	}

	else if((index > VARIABLE_INVALID_INDEX) && (index < VARIABLE_END_HMI_VARIABLES))
    {
        switch(HMI_DATA_HOLDER[index-VARIABLE_HMI_START_INDEX].Type)
        {
            case DATATYPE_UINT8    :
                *value = (sint32) ((uint8*) HMI_DATA_HOLDER[index-VARIABLE_HMI_START_INDEX].Address)[offset];
                break;
            case DATATYPE_SINT8    :
                *value = (sint32) ((sint8*) HMI_DATA_HOLDER[index-VARIABLE_HMI_START_INDEX].Address)[offset];
                break;
            case DATATYPE_UINT16   :
                *value = (sint32) ((uint16*) HMI_DATA_HOLDER[index-VARIABLE_HMI_START_INDEX].Address)[offset];
                break;
            case DATATYPE_IS_Q15_CELSIUS:
                //lint -fallthrough
            case DATATYPE_SINT16   :
                *value  = (sint32) ((sint16*) HMI_DATA_HOLDER[index-VARIABLE_HMI_START_INDEX].Address)[offset];
                break;
            case DATATYPE_UINT32   :
                *value  = (sint32) ((uint32*) HMI_DATA_HOLDER[index-VARIABLE_HMI_START_INDEX].Address)[offset];
                break;
            case DATATYPE_SINT32   :
                *value  = ((sint32*) HMI_DATA_HOLDER[index-VARIABLE_HMI_START_INDEX].Address)[offset];
                break;
            default:
                result = FALSE;
                LOG_ADD_EXCEPTION(DATATYPE_ERROR, HMI_DATA_HOLDER[index-VARIABLE_HMI_START_INDEX].Type);
                break;
        }
    }

	//
	return result;
}

//
#endif 			//#if ((SYSTEM_ACU_HANDLING != ENABLED) && (SYSTEM_HMI_HANDLING != ENABLED))

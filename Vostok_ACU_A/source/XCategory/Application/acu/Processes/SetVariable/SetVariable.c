/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "SetVariable.h"
#include "SettingFile.h"
#include "Variable.h"
#include "Mode.h"
#include "Log.h"
#include "Modifier.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
/**
 * Using those constants to compare with the info from the settings file to
 * select which version of the data structure we'll use and the operation to be performed.
 */
typedef enum SETVARIABLE_F1F0_EMUN
{
	SETVARIABLE_OPTION_DEFAULT 			= 0,
	SETVARIABLE_OPTION_INCREMENT 		= 1,
	SETVARIABLE_OPTION_DECREMENT		= 2,
	SETVARIABLE_OPTION_BIT_OPERATION	= 3,
	SETVARIABLE_OPTION_COUNT
}SETVARIABLE_F1F0_TYPE;
/**
 * Operation: tells us what to do with the variable and Values.
 */
typedef enum SETVARIABLE_OPERATION_ENUM
{
	SV_OPERATION_SET            = 0,
	SV_OPERATION_ADD_SET        = 1,
	SV_OPERATION_SUB_SET        = 2,
    SV_OPERATION_MULTIPLY_SET   = 3,
    SV_OPERATION_DIVIDE_SET     = 4
}SETVARIABLE_OPERATION_TYPE;

/**
 * It performs a simple value attribution Variable = Operand or simple math like adding or subtracting.
 */
typedef struct SETVARIABLE_DEFAULT_STRUCT
{
	//! First word
	SETVARIABLE_OPERATION_TYPE Operation	:4;
	uint8 Size_Of_This_Struct				:4;	//! This is here to calculate the number of loops needed to process all the data on the displacement. If it is zero, we use sizeof instead.

	uint8 unused_0;
	//! Second word
	ACU_VARIABLE_ID_TYPE Variable_Index;		//! Index to the Variable on the Variable module (Look at Variable_prm.h and Variable_prv.h)
	uint8 Variable_Offset;						//! Offset to access a Variable on the Variable Module. If Variable is an Array, the offset is the index of the Variable inside the Array. If it is a regulation, it is the position on the Regulation Array.
	//! Third word.
	union
	{
		sint16 Sint16;
		uint16 Uint16;
	}Operand;

	//! Fourth Word
    uint8 Modifier_Index;
    uint8 unused_1;

}SETVARIABLE_DEFAULT_TYPE;
/**
 * It performs Variable++ or Variable--
 */
typedef struct SETVARIABLE_INC_DEC_STRUCT
{
    ACU_VARIABLE_ID_TYPE Variable_Index;		//! Index to the Variable on the Variable module (Look at Variable_prm.h and Variable_prv.h)
	uint8 Variable_Offset;
}SETVARIABLE_INC_DEC_TYPE;
/**
 * It performs bit operations on the variable.
 * Depending on the value of bit Set it will perform the bit operation.
 */
typedef struct SETVARIABLE_BIT_OPERATION_STRUCT
{
	//! First word
	uint8 Set_or_Clear 				:1;
	uint8 Unused					:3;
	uint8 Size_Of_This_Struct		:4;			//! This is here to calculate the number of loops needed to process all the data on the displacement. If it is zero, we use sizeof instead.

	uint8 Bit_Position;
	//! Second word
	ACU_VARIABLE_ID_TYPE Variable_Index;		//! Index to the Variable on the Variable module (Look at Variable_prm.h and Variable_prv.h)
	uint8 Variable_Offset;						//! Offset to access a Variable on the Variable Module. If Variable is an Array, the offset is the index of the Variable inside the Array. If it is a regulation, it is the position on the Regulation Array.
}SETVARIABLE_BIT_OPERATION_TYPE;


//! IDs FOR LOG messages
typedef enum
{
	INVALID_PROCESS_CONTROL 	    = 0,
	INVALID_PROCESS_PARAMETERS 	    = 1,
	INVALID_PROCESS_OPTION		    = 2,
	INVALID_SETTINGFILE_PARAMETERS  = 3,
	INVALID_VARIABLE
} MODULE_SETVARIABLE_LOG_MESSAGE_ID_TYPE;
//! ID for this module on the Log module.
#define MODULE_NAME		MODULE_SETVARIABLE

static VARIABLE_SET_OPERATION_TYPE const VARIABLE_SET_OPERATION_MAP[] =
{
    VARIABLE_OPERATION_SET,         // SV_OPERATION_SET
    VARIABLE_OPERATION_ADD,         // SV_OPERATION_ADD_SET
    VARIABLE_OPERATION_SUBTRACT,    // SV_OPERATION_SUB_SET
    VARIABLE_OPERATION_MULTIPLY,    // SV_OPERATION_MULTIPLY_SET
    VARIABLE_OPERATION_DIVIDE      // SV_OPERATION_DIVIDE_SET
};
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *	Function that evaluates the Setting File Control structure and run the proper commands to set a value to a Variable.
 *	It depends on the Variable module.
 *	The variables are set during the Step Initialization of the process. we don't check and change during the execution.
 */
PROCESS_RESULT_TYPE SetVariable__Process(PROCESS_CONTROL_TYPE* control)
{
	PROCESS_RESULT_TYPE result;
	SETTINGFILE_LOADER_TYPE loader;
	SETVARIABLE_F1F0_TYPE f1f0;
	SETVARIABLE_DEFAULT_TYPE *default_type;
	SETVARIABLE_INC_DEC_TYPE *inc_dec_type;
	SETVARIABLE_BIT_OPERATION_TYPE *bit_op_type;
	VARIABLE_SET_OPERATION_TYPE operation;
	uint8 index;
	uint8 number_of_loops;
	//
	operation = VARIABLE_OPERATION_SET;			//Default operation is set.
	result = PROCESS_COMPLETE;
	// Check for valid data coming as parameter.
	if((control == NULL) || (control->F1_F0_Flags >= (uint8)SETVARIABLE_OPTION_COUNT))
	{
		LOG_ADD_EXCEPTION(INVALID_PROCESS_CONTROL, (control != NULL));
	}
	else if(control->Step_Initializing == TRUE)
	{
		// Read process parameters.
		if (SettingFile__MSettingBasicLoader(control->Setting_File_Index,
				SF_PTR_ACU_PROCESS_SET_VARIABLE,
				control->Data_Field_Index,
				&loader) == FAIL)
		{
			LOG_ADD_EXCEPTION(INVALID_PROCESS_PARAMETERS, control->Data_Field_Index);
		}
		else
		{
			f1f0 = (SETVARIABLE_F1F0_TYPE)control->F1_F0_Flags;
			switch(f1f0)
			{
			case SETVARIABLE_OPTION_DEFAULT:
				//						SETVARIABLE_DEFAULT_TYPE
				default_type = ((SETVARIABLE_DEFAULT_TYPE *)(void *)loader.Data);

				//As per the new requirement we are added an extra parameter Modifier_Index
				//This change is dependent on the GESE tool
				//Both GESE and Firmware needs to have the changes together
				if ((loader.Length % sizeof(SETVARIABLE_DEFAULT_TYPE)) != 0)
				{
		            LOG_ADD_EXCEPTION(INVALID_SETTINGFILE_PARAMETERS, loader.Length);

		            Mode__SetModeExtended(Mode__GetMode(), SUBMODE_NORMAL_ERROR);
				}
				else
				{
	                if(default_type->Size_Of_This_Struct == 0)
	                {
	                    number_of_loops = loader.Length / sizeof(SETVARIABLE_DEFAULT_TYPE);
	                }
	                else
	                {
	                    number_of_loops = loader.Length / ((default_type->Size_Of_This_Struct) << 1);
	                }
	                for(index = 0; index < number_of_loops; index++)
	                {
                        sint32 modified_value = Modifier__GetValue((sint32)ENDIANS16_STON(default_type[index].Operand.Sint16),
                                                                   default_type[index].Modifier_Index);
	                    Variable__SetValue(VARIABLE_SET_OPERATION_MAP[default_type[index].Operation],
	                                       (VARIABLE_ID_TYPE)default_type[index].Variable_Index,
	                                       default_type[index].Variable_Offset,
	                                       modified_value);
	                }
				}
				break;

			case SETVARIABLE_OPTION_INCREMENT:
				//						SETVARIABLE_INC_DEC_TYPE
				//lint-fallthrough
			case SETVARIABLE_OPTION_DECREMENT:
				operation = (f1f0 == SETVARIABLE_OPTION_INCREMENT)?VARIABLE_OPERATION_INCREMET:VARIABLE_OPERATION_DECREMENT;
                inc_dec_type = ((SETVARIABLE_INC_DEC_TYPE *)(void *)loader.Data);

                // the size of this is always the amount of memory required to index a variable.
				number_of_loops = loader.Length / sizeof(SETVARIABLE_INC_DEC_TYPE);
				for(index = 0; index < number_of_loops; index++)
				{
					Variable__SetValue(operation, (VARIABLE_ID_TYPE)inc_dec_type[index].Variable_Index, inc_dec_type[index].Variable_Offset, 0);
				}
				break;

			case SETVARIABLE_OPTION_BIT_OPERATION:
				//						SETVARIABLE_BIT_OPERATION_TYPE
				bit_op_type = ((SETVARIABLE_BIT_OPERATION_TYPE *)(void *)loader.Data);
				if(bit_op_type->Size_Of_This_Struct == 0)
				{
					number_of_loops = loader.Length / sizeof(SETVARIABLE_BIT_OPERATION_TYPE);
				}
				else
				{
					number_of_loops = loader.Length / ((bit_op_type->Size_Of_This_Struct) << 1);
				}
				for(index = 0; index < number_of_loops; index++)
				{
					Variable__SetValue((bit_op_type[index].Set_or_Clear) ? VARIABLE_OPERATION_BIT_SET : VARIABLE_OPERATION_BIT_CLEAR,
					                   (VARIABLE_ID_TYPE)bit_op_type[index].Variable_Index,
					                   bit_op_type[index].Variable_Offset,
					                   (sint32)bit_op_type[index].Bit_Position);
				}
				break;

			default:
				LOG_ADD_EXCEPTION(INVALID_PROCESS_OPTION, f1f0);
				break;
			}
		}
	}

	return result;
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================



/**
 *  @file
 *
 *  @brief      Data Types, constants and code to process Condition blocks present on the Settings File.
 *
 *  @details    Module gets one displacement as parameter, evaluates if it's valid and then loop over all comparisons
 *              present on the Condition block to evaluate them and return a BOOL_TYPE as result.
 *
 *  @copyright  Copyright 2015-2016 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"
#include "Condition.h"
#include "Log.h"
#include "SettingFile.h"
#include "Variable.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//! ID for this module on the Log module.
#define MODULE_NAME     MODULE_CONDITION

//! IDs FOR LOG messages
typedef enum MODULE_CONDITION_LOG_MESSAGE_ID_ENUM
{
    EVAL_ERROR = 0,
    LOAD_CONDITION_TABLE,
    LOAD_VARIABLE_ERROR,
    LOAD_VARIABLE_AS_OPERAND_ERROR,
    OPERAND_ERROR,
    OPERATOR_ERROR
} MODULE_CONDITION_LOG_MESSAGE_ID_TYPE;

/**
 * Supported operators.
 */
typedef enum OPERATOR_ENUM
{
    OPERATOR_RESERVED_LOW        = 0,
    OPERATOR_MORE_THAN           = 1,               //!< TRUE if left > right.
    OPERATOR_LESS_THAN           = 2,               //!< TRUE if left < right.
    OPERATOR_NOT_EQUAL           = 3,               //!< TRUE if left != right.
    OPERATOR_EQUAL               = 4,               //!< TRUE if left = right.
    OPERATOR_MORE_THAN_OR_EQUAL  = 5,               //!< TRUE if left >= right.
    OPERATOR_LESS_THAN_OR_EQUAL  = 6,               //!< TRUE if left <= right.
    OPERATOR_RESERVED_UP         = 7
} CONDITION_OPERATOR_TYPE;

/**
 * Structure of a condition.
 * This is coming from the Setting File definition.
 */
typedef struct CONDITION_STRUCT
{
    // Byte #0
    CONDITION_OPERATOR_TYPE Operator    : 3;        //!< Operator to be used on the comparison.
    uint8 Unused                        : 3;        //!< 2 unused bits. Will be zero.
    BOOL_TYPE Is_Operand_A_Variable     : 1;        //!< Bool to tell the Operator will be applied to 2 variables so Operand holds another index to a Variable.
    BOOL_TYPE Is_Or                     : 1;        //!< Indicates how to combine this statement with the next one, if present.
    // Byte #1
    uint8 Length                        : 3;        //!< Indicates The length of 1 condition in order to find the number of conditions
    uint8 Free                          : 5;
    // Byte #2-3
    ACU_VARIABLE_ID_TYPE Variable_Index;                //!< Index to the Variable on the Variable module (Look at Variable_prm.h and Variable_prv.h)
    uint8 Variable_Offset;                          //!< Offset to access a Variable on the Variable Module. If Variable is an Array, the offset is the index of the Variable inside the Array. If it is a regulation, it is the position on the Regulation Array.
    // Bytes #3-4
    union
    {
        sint16 Sint16;                              //!< access to signed word
        uint16 Uint16;                              //!< access to unsigned word
        //! access to the data as an index to variable.
        struct OP_IS_VARIABLE_STRUCT
        {
            ACU_VARIABLE_ID_TYPE Second_Variable_Index; //!< Index to the Variable that will act as a Operand. Understanding also depends on the flag Is_Operand_A_Variable.
            uint8 Second_Variable_Offset;           //!< Offset to access the Variable on the Variable module.
        } Operand_Is_Variable;
    } Operand;                                      //!< Word operand
    // Bytes #5-6
    uint16 Mask;                                    //!< If bit Is_Mask_Needed is TRUE, Operands are masked.
} CONDITION_TYPE;


//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

static BOOL_TYPE LoopComparisons(SETTINGFILE_LOADER_TYPE* expression_header);
static BOOL_TYPE GetOperands(CONDITION_TYPE* comparison, sint32* left, sint32* right);
static BOOL_TYPE DoEvaluate(CONDITION_OPERATOR_TYPE oper, sint32 left, sint32 right);


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This function evaluates the comparisons indexed by the condition_displacement. This function loads the data from the Setting File.
 * @param pointer = Pointer that contains the condition displacement data.
 * @param displacement = Index for the comparisons to be evaluated. This module reads the Data from the settings file.
 * @return the result of Evaluating all comparisons presents on a given displacement.
 */
CONDITION_RESULT_TYPE Condition__Evaluate(SETTINGFILE_OFFSET_TYPE pointer, uint16 displacement)
{
    //Initializations
    CONDITION_RESULT_TYPE result = CONDITION_RESULT_FALSE;

    // Consisting the displacement here and returning if it is not present.
    if(displacement != 0xFF)
    {
        // Get Data from the Settings file based on the displacement.
        SETTINGFILE_LOADER_TYPE loader;
        PASS_FAIL_TYPE load_status = SettingFile__BasicLoader(pointer, displacement, &loader);

        if(load_status == PASS)
        {
            result = (CONDITION_RESULT_TYPE)LoopComparisons(&loader);
        }
        else
        {
            LOG_ADD_EXCEPTION(LOAD_CONDITION_TABLE, displacement);
            result = CONDITION_ERROR_ON_SETTING_FILE;
        }
    }
    else
    {
        result = CONDITION_ERROR_NO_DISPLACEMENT;
    }

    return result;
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Function that loops all the Comparisons on a Condition Statement and evaluates each one returning the result at the end.
 * @param expression_header It is the header for the Condition Statement loaded from the Setting File.
 * @return FALSE or TRUE depending on the result. FALSE also can represent an error on the Setting File. In this case a LOG message will be present.
 */
static BOOL_TYPE LoopComparisons(SETTINGFILE_LOADER_TYPE* expression_header)
{
    CONDITION_TYPE* comparison;
    uint16* comparison_pointer;
    uint16 comparison_word;
    uint16 word_length;
    BOOL_TYPE aux;
    BOOL_TYPE result;
    sint32 left;
    sint32 right;
    result = FALSE;

    //loading the locals with info to start evaluating.
    comparison_pointer= (uint16*)expression_header->Data;  //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    comparison = (CONDITION_TYPE*) comparison_pointer;      //lint !e929 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    comparison_word =comparison->Length;
    word_length=expression_header->Length/2;

    if (comparison_word <= word_length)
    {
        // Loading with the neutral operand for the first statement.
        if(comparison->Is_Or)
        {
            result = FALSE;
        }
        else
        {
            result = TRUE;
        }

        do
        {
            //getting the operands
            if(TRUE == GetOperands(comparison, &left, &right))
            {
                aux = DoEvaluate(comparison->Operator, left, right);
                if(comparison->Is_Or)
                {
                    if(result || aux)
                    {
                        result =  TRUE;
                    }
                    else
                    {
                        result = FALSE;
                    }
                }
                else
                {
                    if(result && aux)
                    {
                        result = TRUE;
                    }
                    else
                    {
                        result = FALSE;
                    }
                }
            }


            //...navigating on the conditions.
            if (comparison_word < word_length)
            {// increment the pointer only if we don't reach the end of the structure
                comparison_pointer += comparison->Length;
                comparison = (CONDITION_TYPE*) comparison_pointer;  //lint !e929 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                comparison_word+=comparison->Length;
            }else
            {
                //end of comparison
                comparison_word=0;

            }

        }while ((comparison_word<=word_length)&&(comparison_word));
    }
    else
    {
        LOG_ADD_EXCEPTION(EVAL_ERROR, expression_header->Length);
    }

    return result;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Function to load the operands from the condition to the left and right operators.
 * No matter if we get one variable and a constant or another variable.
 *
 * @param condition index to the Condition struct on the setting file.
 * @param left  left operand is a pointer to a variable
 * @param right right operand is a pointer to either a variable or a constant.
 * @return  TRUE if all works fine.
 */
static BOOL_TYPE GetOperands(CONDITION_TYPE* comparison, sint32* left, sint32* right)
{
    DATATYPE_TYPE type;
    BOOL_TYPE result = TRUE;

    // Load the first (left) value which is always a variable.
    VARIABLE_ID_TYPE var_id = (VARIABLE_ID_TYPE)comparison->Variable_Index;
    uint8 var_offset = comparison->Variable_Offset;
    if (Variable__GetValue(var_id, var_offset, left) == FALSE)
    {
        LOG_ADD_EXCEPTION(LOAD_VARIABLE_ERROR, COMBINE_BYTES(var_id, var_offset));
        result = FALSE;
    }
    else
    {
        // Apply the mask only if the value is unsigned.
        type = Variable__GetTypeByIndex(var_id);
        if ((type == DATATYPE_UINT8) || (type == DATATYPE_UINT16) || (type == DATATYPE_IS_REGULATION))
        {
            *left &= comparison->Mask;
        }

        // Determine whether the second (right) value is a variable or a constant.
        if (comparison->Is_Operand_A_Variable)
        {
            // Load the second value as a variable.
            var_id = (VARIABLE_ID_TYPE)comparison->Operand.Operand_Is_Variable.Second_Variable_Index;
            var_offset = comparison->Operand.Operand_Is_Variable.Second_Variable_Offset;
            if (Variable__GetValue(var_id, var_offset, right) == FALSE)
            {
                LOG_ADD_EXCEPTION(LOAD_VARIABLE_AS_OPERAND_ERROR, COMBINE_BYTES(var_id, var_offset));
                result = FALSE;
            }
            else
            {
                // Apply the mask only if the value is unsigned.
                type = Variable__GetTypeByIndex(var_id);
                if ((type == DATATYPE_UINT8) || (type == DATATYPE_UINT16) || (type == DATATYPE_IS_REGULATION))
                {
                    *right &= comparison->Mask;
                }
            }
        }
        else
        {
            // Load the second value as a constant.
            type = Variable__GetTypeByIndex((VARIABLE_ID_TYPE)comparison->Variable_Index);
            switch (type)
            {
                case DATATYPE_UINT8:
                case DATATYPE_IS_REGULATION:
                case DATATYPE_UINT16:
                    *right = (sint32)ENDIANU16_STON(comparison->Operand.Uint16);
                    // Apply the mask only if the value is unsigned.
                    *right &= comparison->Mask;
                    break;
                case DATATYPE_SINT8:
                case DATATYPE_SINT16:
                case DATATYPE_IS_Q15_CELSIUS:
                    *right = (sint32)ENDIANS16_STON(comparison->Operand.Sint16);
                    break;
                default:
                    LOG_ADD_EXCEPTION(OPERAND_ERROR, type);
                    result = FALSE;
                    break;
            }
        }
    }

    return result;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Function that performs the comparison between the two operands.
 * @param operator  which operation will be performed.
 * @param left  left operand (always coming from a variable).
 * @param right can be a constant or a variable.
 * @return the result of the evaluation.
 */
static BOOL_TYPE DoEvaluate(CONDITION_OPERATOR_TYPE oper, sint32 left, sint32 right)
{
    BOOL_TYPE ret;
    ret = FALSE;

    switch(oper)
    {
        case OPERATOR_MORE_THAN          :
            ret = (BOOL_TYPE) (left > right);
            break;
        case OPERATOR_LESS_THAN          :
            ret = (BOOL_TYPE) (left < right);
            break;
        case OPERATOR_NOT_EQUAL          :
            ret = (BOOL_TYPE) (left != right);
            break;
        case OPERATOR_EQUAL              :
            ret = (BOOL_TYPE) (left == right);
            break;
        case OPERATOR_MORE_THAN_OR_EQUAL :
            ret = (BOOL_TYPE) (left >= right);
            break;
        case OPERATOR_LESS_THAN_OR_EQUAL :
            ret = (BOOL_TYPE) (left <= right);
            break;
        default:
            LOG_ADD_EXCEPTION(OPERATOR_ERROR, oper);
            break;
    }

    return ret;
}

/**
 *  @file
 *
 *  @brief      Implements Nucleus modifiers.
 *
 *  @details    A modifier is used to modify an existing original value into a resulting value.
 *
 *  A modifier applies an overall operation (#MODIFIER_OVERALL_OPERATION_TYPE) to combine multiple
 *  modifications. Each modification is based on an action (#MODIFIER_OPERATOR_TYPE). Together,
 *  these operations result in a final resulting value.
 *
 *  Definitions:
 *  - Original Value = The value that will be modified.
 *  - Modified Value = The result of a single modification of the Original Value.
 *  - Resulting Value = The final result of the modifier.
 *  - Overall Operation: The method used to combine the modifications into a single Resulting Value.
 *
 *  Displacement Data for the specified pointer (or SF_PTR_ACU_MODIFIERS on legacy code):
 *
 *      Modifier displacement data for:
 *          - All modifiers of 8-bit and 16-bit values
 *          - Table modifiers of 32-bit and float values
 *          - Variable modifiers of 32-bit and float values
 *      |--------|---------------------------------------|---------------------------------------|
 *      |        |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |
 *      | +0000h |More|  0 | Overall Oper |      DTV     |        Type       |  % |    Action    |
 *      | +0001h |             Variable Index            |            Variable Offset            |
 *      |        | 15 | 14 | 13 | 12 | 11 | 10 |  9 |  8 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |
 *      | +0002h |           Constant Operand (sint16 or uint16) or Table Displacement           |
 *      |--------|-------------------------------------------------------------------------------|
 *
 *      Modifier displacement data for:
 *          - Direct modifiers of 32-bit and float values
 *      |--------|---------------------------------------|---------------------------------------|
 *      |        |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |
 *      | +0000h |More|  0 | Overall Oper |      DTV     |        Type       |  % |    Action    |
 *      | +0001h |             Variable Index            |            Variable Offset            |
 *      |        | 15 | 14 | 13 | 12 | 11 | 10 |  9 |  8 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |
 *      | +0002h |                  Constant Operand (sint32, uint32, or float)                  |
 *      | +0003h |                                                                               |
 *      |--------|-------------------------------------------------------------------------------|
 *
 *  If the DTV field is set to #MODIFIER_DTV_TABLE (1), then a second displacement in the same
 *  pointer is referenced in the Table Displacement. The second displacement contains table data
 *  for the conversion.
 *
 *  Refer to the following website for more details:
 *  "https://sites.google.com/a/whirlpool.com/software-architecture-infrastructure/nucleus/application/modifiers"
 *
 *  @copyright  Copyright 2015-2018 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "C_Extensions.h"
#include "Modifier.h"

// -- Other Modules --
#include "Convert.h"
#include <limits.h>
#include "Log.h"
#include <math.h>
#include "Regulations.h"
#include "Variable.h"


//  --- Private Declarations ------------------------------------------------------------------------------------------


// -- Private Constant Declarations --

//! ID for this module in the Log module.
#define MODULE_NAME MODULE_MODIFIER

//! A value that indicates that the modified_value is invalid or inactive.
//! The value is also used to set the initial value for the min/max overall operations.
#define MODIFY_VALUE_IS_OUT_OF_BOUNDS 1E39


// -- Private Enumerated Constant Declarations --

//! Identifies which type of Modifier is being evaluated.
typedef enum MODIFIER_DTV_ENUM
{
    MODIFIER_DTV_DIRECT             = 0,
    MODIFIER_DTV_TABLE              = 1,
    MODIFIER_DTV_VARIABLE           = 2
} MODIFIER_DTV_TYPE;

//! Enumerator that defines Overall Operation. This info indicates which operation will be performed
//! on the Intermediate (Modified) Values.
typedef enum MODIFIER_OVERALL_OPERATION_ENUM
{
    MODIFIER_OVERALL_INVALID        = 0,
    MODIFIER_OVERALL_MAX            = 1,            //!< The resulting value is the maximum of the modified values.
    MODIFIER_OVERALL_MIN            = 2,            //!< The resulting value is the minimum of the modified values.
    MODIFIER_OVERALL_SUM            = 3,            //!< The resulting value is the sum of the modified values.
    MODIFIER_OVERALL_PRIORITY       = 4,            //!< The resulting value is the first modified value that is active.
    MODIFIER_OVERALL_SEQUENCE       = 5             //!< The resulting value is the result of all modifications applied sequentially.
} MODIFIER_OVERALL_OPERATION_TYPE;

//! Message IDs for log messages.
typedef enum MODULE_MODIFIER_LOG_MESSAGE_ID_ENUM
{
    CANNOT_LOAD_PARAMETERS = 1,
    INVALID_OVERALL_OPERATION,
    INVALID_DTV_OPTION,
    CONVERT_STATUS_FAILED,
    MISSING_TABLE_DATA,
    NO_32BIT_TABLE_SUPPORT_AVAILABLE,
    CANNOT_GET_VARIABLE_VALUE,
    INVALID_ACTION,
    DATATYPE_NOT_SUPPORTED
} MODULE_MODIFIER_LOG_MESSAGE_ID_TYPE;


// -- Private Structure Type Declarations --

//! A union of all data types (up to 32-bit).
//! Note: This works because the micro is little-endian.
//! Allows a float32 type to be carried in an sint32 type.
typedef union MODIFIER_DATA32_UNION
{
    uint8 UInt8;
    uint16 UInt16;
    uint32 UInt32;
    sint8 SInt8;
    sint16 SInt16;
    sint32 SInt32;
    float32 Float32;
} MODIFIER_DATA32_TYPE;
CT_ASSERT(sizeof(MODIFIER_DATA32_TYPE) == sizeof(sint32));

//! Setting file parameters used by the Modifier module.
typedef PACKED const struct MODIFIER_SF_PARAMETERS_STRUCT
{
    MODIFIER_DTV_TYPE Dtv                             : 3;
    MODIFIER_OVERALL_OPERATION_TYPE Overall_Operation : 3;
    uint8 Unused1                                     : 1;
    BOOL_TYPE More_Modifications                      : 1;

    MODIFIER_OPERATOR_TYPE Action   : 3;
    BOOL_TYPE Is_Percentage         : 1;            //!< 1 = Indicates that the Operand is a percentage.
    DATATYPE_TYPE Type              : 4;            //!< The type of the operand.

    ACU_VARIABLE_ID_TYPE Variable_Index;
    uint8 Variable_Offset;

    MODIFIER_DATA32_TYPE Operand;
} MODIFIER_SF_PARAMETERS_TYPE;

//! Struct that represents the Level 2 table for a Modifier.
//! When needed it is referenced on the Level 1 table.
typedef PACKED const struct MODIFIER_SF_TABLE_HEADER_STRUCT
{
    uint8 Search_Method             : 3;            //!< Search method to find the operand (x) in the table data.
    uint8 Unused1                   : 5;
    uint8 Number_Of_Items;                          //!< Number of (x,y) points present in the table data.
} MODIFIER_SF_TABLE_HEADER_TYPE;


//  --- Private Function Prototypes -----------------------------------------------------------------------------------

static double DirectModification(double original_value, MODIFIER_SF_PARAMETERS_TYPE* params, uint16 displacement);
static double TableModification(double original_value, MODIFIER_SF_PARAMETERS_TYPE* params, SETTINGFILE_OFFSET_TYPE pointer, uint16 displacement);
static double VariableModification(double original_value, MODIFIER_SF_PARAMETERS_TYPE* params, uint16 displacement);
static double ApplyModificationAction(double original_value, MODIFIER_SF_PARAMETERS_TYPE* params, double operand, uint16 displacement);
static double GetUniversalValue(MODIFIER_DATA32_TYPE data, DATATYPE_TYPE type, uint16 displacement);


//=====================================================================================================================
//  --- Public Functions ----------------------------------------------------------------------------------------------
//=====================================================================================================================


#if (SYSTEM_ACU_HANDLING == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the action associated with the first modification in the modifier.
 * @param modifier_displacement = The displacement for the modifier data in the setting file.
 * @return The action associated with the first modification in the modifier.
 *         Returns MODIFIER_OPERATOR_RESERVED is case something goes wrong.
 */
MODIFIER_OPERATOR_TYPE Modifier__GetOperator(uint16 modifier_displacement)
{
    SETTINGFILE_LOADER_TYPE loader;
    MODIFIER_OPERATOR_TYPE action = MODIFIER_OPERATOR_RESERVED;

    if ((modifier_displacement < UCHAR_MAX) &&
        (SettingFile__BasicLoader(SF_PTR_ACU_MODIFIERS, modifier_displacement, &loader) == PASS))
    {
        MODIFIER_SF_PARAMETERS_TYPE* params = (MODIFIER_SF_PARAMETERS_TYPE*)(void*)loader.Data;
        action = params->Action;
    }

    return action;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Modify the original_value using the modifier parameters referenced by the modifier_displacement.
 * Do not use this function to modify uint32 or float32 original_values.
 * @param original_value = The value that is modified by the modifier.
 * @param modifier_displacement = Displacement to get the modifier parameters in the setting file.
 * @return The resulting_value after the modifier is applied to the original_value.
 *         Return the original_value if a problem occurs.
 */
sint32 Modifier__GetValue(sint32 original_value, uint16 modifier_displacement)
{
    return lround(Modifier__ModifyValue((double)original_value, SF_PTR_ACU_MODIFIERS, modifier_displacement));
}
#endif


//---------------------------------------------------------------------------------------------------------------------
/**
 * Modify the original_value using the modifier parameters referenced by the modifier_displacement.
 * This function will support most original_value data types including float32, sint32, and uint32.
 * @param original_value = The value that is modified by the modifier.
 * @param modifier_displacement = Displacement to get the modifier parameters in the setting file.
 * @return The resulting_value after the modifier is applied to the original_value.
 *         Return the original_value if a problem occurs.
 */
double Modifier__ModifyValue(double original_value, SETTINGFILE_OFFSET_TYPE pointer, uint16 modifier_displacement)
{
    double resulting_value = original_value;        // Start with the original value.

    // Checking if we are receiving a valid displacement.
    if (modifier_displacement != 0xFF)
    {
        SETTINGFILE_LOADER_TYPE loader;
        if (SettingFile__BasicLoader(pointer, modifier_displacement, &loader) == FAIL)
        {
            LOG_ADD_EXCEPTION(CANNOT_LOAD_PARAMETERS, modifier_displacement);
        }
        else
        {
            // Get the setting file parameters for the modifier.
            MODIFIER_SF_PARAMETERS_TYPE* params = (MODIFIER_SF_PARAMETERS_TYPE*)(void*)loader.Data;

            // Initialize the resulting_value based on the overall operation.
            MODIFIER_OVERALL_OPERATION_TYPE overall_operation = params->Overall_Operation;
            switch (overall_operation)
            {
                case MODIFIER_OVERALL_SUM:
                    // Start with an initial sum of zero.
                    resulting_value = 0.0;
                    break;
                case MODIFIER_OVERALL_MAX:
                case MODIFIER_OVERALL_MIN:
                case MODIFIER_OVERALL_PRIORITY:
                case MODIFIER_OVERALL_SEQUENCE:
                    break;
                default:
                    LOG_ADD_EXCEPTION(INVALID_OVERALL_OPERATION, modifier_displacement);
                    break;
            }

            // Iterate through the modifications in the modifier parameter data.
            BOOL_TYPE more_modifications = TRUE;
            while (more_modifications == TRUE)
            {
                // Determine if there are more modifications after this one.
                // This may be overridden to FALSE if the MODIFIER_OVERALL_PRIORITY overall operation is
                // used and the current modification is active.
                more_modifications = params->More_Modifications;

                // Get the modified_value using the current modification.
                double modified_value;
                switch (params->Dtv)
                {
                    case MODIFIER_DTV_DIRECT:
                        modified_value = DirectModification(original_value, params, modifier_displacement);
                        break;
                    case MODIFIER_DTV_TABLE:
                        modified_value = TableModification(original_value, params, pointer, modifier_displacement);
                        break;
                    case MODIFIER_DTV_VARIABLE:
                        modified_value = VariableModification(original_value, params, modifier_displacement);
                        break;
                    default:
                        LOG_ADD_EXCEPTION(INVALID_DTV_OPTION, modifier_displacement);
                        modified_value = original_value;
                        more_modifications = FALSE;
                        break;
                }

                // Check if the modified_value is valid.
                if (modified_value != MODIFY_VALUE_IS_OUT_OF_BOUNDS)
                {
                    // Process the modified_value using the overall_operator.
                    switch (overall_operation)
                    {
                        case MODIFIER_OVERALL_SEQUENCE:
                            // Set the resulting_value as the result of the sequence so far.
                            resulting_value = modified_value;
                            // Set the original value so that the next modification can build from here.
                            original_value = modified_value;
                            break;
                        case MODIFIER_OVERALL_PRIORITY:
                            // Set the resulting_value based on the result of this modification only.
                            resulting_value = modified_value;
                            // Cancel evaluation of additional modifications.
                            more_modifications = FALSE;
                            break;
                        case MODIFIER_OVERALL_MAX:
                            // Check for a new maximum value.
                            if (resulting_value < modified_value)
                            {
                                // Set the resulting_value to the highest modified_value so far.
                                resulting_value = modified_value;
                            }
                            break;
                        case MODIFIER_OVERALL_MIN:
                            // Check for a new minimum value.
                            if (resulting_value > modified_value)
                            {
                                // Set the resulting_value to the lowest modified_value so far.
                                resulting_value = modified_value;
                            }
                            break;
                        case MODIFIER_OVERALL_SUM:
                            // Update the resulting_value as the sum of all the modified_values.
                            resulting_value += modified_value;
                            break;
                        default:
                            more_modifications = FALSE;
                            break;
                    }
                }

                // Move to the next Modification's parameters.
                if (more_modifications == TRUE)
                {
                    if ((params->Dtv == MODIFIER_DTV_DIRECT) &&
                        ((params->Type == DATATYPE_UINT32) ||
                         (params->Type == DATATYPE_SINT32) ||
                         (params->Type == DATATYPE_FLOAT32)))
                    {
                        // The current modification consumes the full
                        // sizeof(MODIFIER_SF_PARAMETERS_TYPE) bytes.
                        params++;
                    }
                    else
                    {
                        // The current modification consumes less than the full
                        // sizeof(MODIFIER_SF_PARAMETERS_TYPE) bytes.
                        // The Operand field contains a 2-byte value (instead of a 4-byte value).
                        // Step 1: Get a pointer to the last field (Operand).
                        uint8* ptr = (uint8*)(void*)&params->Operand;
                        // Step 2: Add the size of the smaller Operand.
                        params = (MODIFIER_SF_PARAMETERS_TYPE*)(void*)&ptr[sizeof(uint16)];
                    }
                }
            }
        }
    }
    else
    {
        // Displacement of 0xFF indicates no displacement which means no modifier.
        // The original_value with no modifier is simply the original_value.
        // Do nothing.
    }

    return resulting_value;
}


//=====================================================================================================================
//  --- Private Functions ---------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Determine the modified_value based on a modification with DTV = Direct.
 * @param original_value = The value that is modified by the modification.
 * @param params = Pointer to the modification parameters in the setting file.
 * @return Return the modified_value if the modification is active.
 *         Return MODIFY_VALUE_IS_OUT_OF_BOUNDS if the modification is inactive or invalid.
 */
static double DirectModification(double original_value, MODIFIER_SF_PARAMETERS_TYPE* params, uint16 displacement)
{
    double modified_value = MODIFY_VALUE_IS_OUT_OF_BOUNDS;
    MODIFIER_DATA32_TYPE value;
    if (Variable__GetTranslatedValue((VARIABLE_ID_TYPE)params->Variable_Index, params->Variable_Offset, &value.SInt32) == TRUE)
    {
        if (value.SInt32 != 0L/*FALSE*/)
        {
            // Determine the operand.
            double operand = GetUniversalValue(params->Operand, params->Type, displacement);
            // Calculate the modified_value for this modification step.
            if (operand != MODIFY_VALUE_IS_OUT_OF_BOUNDS)
            {
                modified_value = ApplyModificationAction(original_value, params, operand, displacement);
            }
        }
    }
    else
    {
        // Variable is inaccessible. The Direct Modification does not occur. (Same as var==FALSE)
        LOG_ADD_EXCEPTION(CANNOT_GET_VARIABLE_VALUE, displacement);
    }
    return modified_value;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Determine the modified_value based on a modification with DTV = Direct.
 * @param original_value = The value that is modified by the modification.
 * @param params = Pointer to the modification parameters in the setting file.
 * @return Return the modified_value if the modification is active.
 *         Return MODIFY_VALUE_IS_OUT_OF_BOUNDS if the modification is inactive or invalid.
 */
static double TableModification(double original_value, MODIFIER_SF_PARAMETERS_TYPE* params, SETTINGFILE_OFFSET_TYPE pointer, uint16 displacement)
{
    double modified_value = MODIFY_VALUE_IS_OUT_OF_BOUNDS;
    MODIFIER_DATA32_TYPE value;
    if (Variable__GetTranslatedValue((VARIABLE_ID_TYPE)params->Variable_Index, params->Variable_Offset, &value.SInt32) == TRUE)
    {
        DATATYPE_TYPE type = Variable__GetTypeByIndex((VARIABLE_ID_TYPE)params->Variable_Index);
        if (type == DATATYPE_IS_REGULATION)
        {
            // Regulation data types always use the regulation index (not the value).
            type = DATATYPE_UINT8;
        }

        if ((VARIABLE_SIZE[type] <= 2) && (VARIABLE_SIZE[params->Type] <= 2))
        {
            // Get the setting file parameters with the table data.
            SETTINGFILE_LOADER_TYPE loader;
            if (SettingFile__BasicLoader(pointer, params->Operand.UInt16, &loader) == PASS)
            {
                MODIFIER_SF_TABLE_HEADER_TYPE* mod_table = (MODIFIER_SF_TABLE_HEADER_TYPE*)(void*)loader.Data;

                // Determine the offset in the setting file data to the table's output data.
                uint16 index_to_output_data = sizeof(MODIFIER_SF_TABLE_HEADER_TYPE) + VARIABLE_SIZE[type] * mod_table->Number_Of_Items;
                if ((VARIABLE_SIZE[type] == 1) &&               // if each input value consumes one byte, and ...
                    ((mod_table->Number_Of_Items & 1) == 1) &&  // ... there is an odd number of items, and ...
                    (VARIABLE_SIZE[params->Type] >= 4))         // ... each output value consumes 4 bytes or more, then...
                {
                    // Account for a padding byte in the data.
                    index_to_output_data++;
                }

                // Build the data structure used by the Convert module for converting the value.
                CONVERT_TABLE_TYPE table;
                table.input_table_1 = &loader.Data[sizeof(MODIFIER_SF_TABLE_HEADER_TYPE)];
                table.input_table_2 = NULL;
                table.output_table = &loader.Data[index_to_output_data];
                table.input_type = type;
                table.output_type = params->Type;
                table.table_size = mod_table->Number_Of_Items;
                table.search_method = (CONVERT_SEARCH_TYPE)(mod_table->Search_Method);

                // Call Convert to do the Search.
                MODIFIER_DATA32_TYPE converted;
                CONVERT_STATUS_TYPE result = Convert__GetValue(value.SInt32, &table, &converted.SInt32);
                if ((result == CONVERT_STATUS_PASS) ||
                    (result == CONVERT_STATUS_UNDER_RANGE) ||
                    (result == CONVERT_STATUS_OVER_RANGE))
                {
                    double operand = GetUniversalValue(converted, params->Type, displacement);
                    // Calculate the modified_value for this modification step.
                    // Changing the Original Value using the Action and the Operand.
                    modified_value = ApplyModificationAction(original_value, params, operand, displacement);
                }
                else if (result != CONVERT_STATUS_NO_MATCH_FOUND)
                {
                    LOG_ADD_EXCEPTION(CONVERT_STATUS_FAILED, params->Operand.UInt16);
                }
            }
            else
            {
                LOG_ADD_EXCEPTION(MISSING_TABLE_DATA, params->Operand.UInt16);
            }
        }
        else
        {
            LOG_ADD_EXCEPTION(NO_32BIT_TABLE_SUPPORT_AVAILABLE, displacement);
        }
    }

    return modified_value;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Determine the modified_value based on a modification with DTV = Variable.
 * @param original_value = The value that is modified by the modification.
 * @param params = Pointer to the modification parameters in the setting file.
 * @return The value of the referenced variable or MODIFY_VALUE_IS_OUT_OF_BOUNDS if the variable
 *         value cannot be determined.
 */
static double VariableModification(double original_value, MODIFIER_SF_PARAMETERS_TYPE* params, uint16 displacement)
{
    double modified_value;
    MODIFIER_DATA32_TYPE value;
    if (Variable__GetValue((VARIABLE_ID_TYPE)params->Variable_Index, params->Variable_Offset, &value.SInt32) == TRUE)
    {
        DATATYPE_TYPE variable_type = Variable__GetTypeByIndex((VARIABLE_ID_TYPE)params->Variable_Index);
        #if (SYSTEM_ACU_HANDLING == ENABLED)
            if (variable_type == DATATYPE_IS_REGULATION)
            {
                variable_type = Regulations__GetMainValueTypeByPosition(params->Variable_Offset);
            }
        #elif (SYSTEM_HMI_HANDLING == ENABLED)
            if (variable_type == DATATYPE_IS_REGULATION)
            {
                variable_type = Regulations__GetVisualValueTypeByPosition(params->Variable_Offset);
            }
        #endif
        double variable_value = GetUniversalValue(value, variable_type, displacement);
        modified_value = ApplyModificationAction(original_value, params, variable_value, displacement);
    }
    else
    {
        // Variable is inaccessible. Use the original_value as the modified_value.
        modified_value = MODIFY_VALUE_IS_OUT_OF_BOUNDS;
        LOG_ADD_EXCEPTION(CANNOT_GET_VARIABLE_VALUE, displacement);
    }
    return modified_value;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Apply the Modification's action to the original_value and the operand to determine the modified_value.
 * @param original_value = The value that is modified by the modification.
 * @param params = Pointer to the modification parameters in the setting file.
 * @param operand = The value that will be combined with the original value to return the modified_value.
 * @return The modified_value result of the single modification (which is part of the modifier).
 */
static double ApplyModificationAction(double original_value, MODIFIER_SF_PARAMETERS_TYPE* params, double operand, uint16 displacement)
{
    double temporary = operand;
    if (params->Is_Percentage == TRUE)
    {
        temporary = original_value * operand / 100.0;
    }

    double modified_value;
    switch (params->Action)
    {
        case MODIFIER_OPERATOR_MULTIPLY:
            modified_value = original_value * operand;
            if (params->Is_Percentage == TRUE)
            {
                modified_value /= 100.0;
            }
            break;
        case MODIFIER_OPERATOR_ADD:
            modified_value = original_value + temporary;
            break;
        case MODIFIER_OPERATOR_SUBTRACT:
            modified_value = original_value - temporary;
            break;
        case MODIFIER_OPERATOR_MAX:
            modified_value = (original_value > operand) ? original_value : operand;
            break;
        case MODIFIER_OPERATOR_MIN:
            modified_value = (original_value < operand) ? original_value : operand;
            break;
        case MODIFIER_OPERATOR_REPLACE:
            modified_value = temporary;
            break;
        default:
            modified_value = temporary;
            LOG_ADD_EXCEPTION(INVALID_ACTION, displacement);
            break;
    }

    return modified_value;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get a value that represents the provided value with all of its precision and resolution.
 * @param value = Value to convert to a universal value.
 * @param type = The type of the value to convert.
 * @return A double that contains the exact same value as the provided value.
 */
static double GetUniversalValue(MODIFIER_DATA32_TYPE data, DATATYPE_TYPE type, uint16 displacement)
{
    double value;
    switch (type)
    {
        case DATATYPE_UINT8:
            value = data.UInt8;
            break;
        case DATATYPE_UINT16:
            value = data.UInt16;
            break;
        case DATATYPE_UINT32:
            value = data.UInt32;
            break;
        case DATATYPE_SINT8:
            value = data.SInt8;
            break;
        case DATATYPE_SINT16:
        case DATATYPE_IS_Q15_CELSIUS:
            value = data.SInt16;
            break;
        case DATATYPE_SINT32:
            value = data.SInt32;
            break;
        case DATATYPE_FLOAT32:
            value = data.Float32;
            break;
        case DATATYPE_INVALID:
        case DATATYPE_UINT64:
        case DATATYPE_SINT64:
        case DATATYPE_FLOAT64:
        case DATATYPE_IS_ARRAY:
        case DATATYPE_IS_REGULATION:
        default:
            value = MODIFY_VALUE_IS_OUT_OF_BOUNDS;
            LOG_ADD_EXCEPTION(DATATYPE_NOT_SUPPORTED, displacement);
            break;
    }
    return value;
}


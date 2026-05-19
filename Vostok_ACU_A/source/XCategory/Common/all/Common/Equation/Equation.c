/**
 * @file
 * @brief       Evaluate a post-fix equation defined in a setting file displacement.
 *
 * @details     This module evaluates a mathematical equation defined in the setting file and
 *              returns the result. The result can be used as a TRUE/FALSE condition.
 *
 *              If any error occurs during the evaluation, the module returns 0 (FALSE) and
 *              logs an exception message.
 *
 * @copyright   Copyright 2020 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "SystemConfig.h"
#include "Equation.h"
#include "Equation_prv.h"

// -- Other Modules --
#include <fenv.h>
#if (SYSTEM_HMI_HANDLING == ENABLED)
    #include "HmiVariable.h"
#endif
#include <limits.h>
#include "Log.h"
#include <math.h>
#include <stdlib.h>
#if ((SYSTEM_HMI_HANDLING == ENABLED) && (HMI_RECIPE_ENGINE_FEATURE == ENABLED))
    #include "StepData.h"
#endif
#include <string.h>
#include "Variable.h"


//  --- Private Declarations ------------------------------------------------------------------------------------------


// -- Compile Options --

#ifndef EQUATION_MAX_MAP_DIMENSIONS
    //! The maximum number of dimensions supported in look-up tables.
    //! Set this value to 0 to disable ELEMENT_MAP in equations.
    //!     0 = Look-up tables (ELEMENT_MAP) not supported
    //!     2 = Support 1-D and 2-D look-up tables
    //!     5 = Support 1-D, 2-D, 3-D, 4-D, and 5-D look-up tables
    //! Example: A 3-D look-up table solves Y = f(x1, x2, x3)
    #define EQUATION_MAX_MAP_DIMENSIONS 2
#endif
CT_ASSERT((0 <= EQUATION_MAX_MAP_DIMENSIONS) && (EQUATION_MAX_MAP_DIMENSIONS <= 10));

#ifndef EQUATION_STACK_SIZE
    //! The number of values that can be pushed onto the stack.
    //! Memory allocated = 4 * EQUATION_STACK_SIZE
    #define EQUATION_STACK_SIZE 6
#endif


// -- Private Constant Declarations --

//! Reuse the invalid data type as a equation displacement data type
#define DATATYPE_DISPLACEMENT DATATYPE_INVALID

// The List_Displacements[] array in the SF_LOOK_UP_TABLE_TYPE structure includes:
//      A 2-byte displacement for each dimension
//      A 2-byte displacement for the output
//      A 4-bit method for each dimension
#define DISPLACEMENTS_ARRAY_SIZE (EQUATION_MAX_MAP_DIMENSIONS + 1 + (EQUATION_MAX_MAP_DIMENSIONS + 3) / 4)

//! Interpolation index precision (1 / INDEX_SCALE)
#define INDEX_SCALE 256

//! The maximum number of input elements allowed in an Xn Values List
#define MAX_INPUT_LIST_ELEMENTS 255u

//! The maximum number of output elements allowed in the Y Values List
#define MAX_OUTPUT_LIST_ELEMENTS 4095u

//! The maximum size of the data for the output list values (in bytes)
#define MAX_OUTPUT_LIST_SIZE (sizeof(sint32) * MAX_OUTPUT_LIST_ELEMENTS)

//! The name of this module for use by the Log module.
#define MODULE_NAME MODULE_EQUATION


// -- Private Enumerated Constant Declarations --

//! The types of comparisons that can be made between two values on the equation stack.
typedef enum COMPARE_ENUM
{
    CMP_LT = 1,
    CMP_EQ = 2,
    CMP_GT = 4,
    CMP_LE = CMP_LT + CMP_EQ,
    CMP_GE = CMP_GT + CMP_EQ,
    CMP_NE = CMP_LT + CMP_GT
} COMPARE_TYPE;

//! The elements that make up equations.
typedef enum ELEMENT_ENUM
{
    ELEMENT_ZERO                    = 0x00,         // push(0)
    ELEMENT_ONE                     = 0x01,         // push(1)
    ELEMENT_NEGATIVE_ONE            = 0x02,         // push(-1)
    ELEMENT_ACU_VAR                 = 0x08,         // push(ACU_Var(index, offset))
    ELEMENT_HMI_VAR                 = 0x09,         // push(HMI_Var(index, offset))
    ELEMENT_STEP_VAR                = 0x0A,         // push(Step_Var(index))
    ELEMENT_CONST_UINT8             = 0x11,         // push(constant uint8)
    ELEMENT_CONST_UINT16            = 0x12,         // push(constant uint16)
    ELEMENT_CONST_UINT31            = 0x13,         // push(constant uint31)
    ELEMENT_CONST_SINT8             = 0x15,         // push(constant sint8)
    ELEMENT_CONST_SINT16            = 0x16,         // push(constant sint16)
    ELEMENT_CONST_SINT32            = 0x17,         // push(constant sint32)
    ELEMENT_BOOLEAN                 = 0x40,         // pop(a), push(BOOL(a))
    ELEMENT_NOT                     = 0x41,         // pop(a), push(NOT BOOL(a))
    ELEMENT_INVERT                  = 0x42,         // pop(a), push(~a)
    ELEMENT_NEGATE                  = 0x43,         // pop(a), push(-a)
    ELEMENT_ABSOLUTE_VALUE          = 0x44,         // pop(a), push(|a|)
    ELEMENT_SCALE                   = 0x45,         // pop(a), push(lroundf(constant float32 x a))
    ELEMENT_LESS_THAN               = 0x81,         // pop(b), pop(a), push(a < b)
    ELEMENT_EQUAL                   = 0x82,         // pop(b), pop(a), push(a == b)
    ELEMENT_LESS_OR_EQUAL           = 0x83,         // pop(b), pop(a), push(a <= b)
    ELEMENT_GREATER_THAN            = 0x84,         // pop(b), pop(a), push(a > b)
    ELEMENT_NOT_EQUAL               = 0x85,         // pop(b), pop(a), push(a != b)
    ELEMENT_GREATER_OR_EQUAL        = 0x86,         // pop(b), pop(a), push(a >= b)
    ELEMENT_MINIMUM                 = 0x88,         // pop(b), pop(a), push(min(a, b))
    ELEMENT_MAXIMUM                 = 0x89,         // pop(b), pop(a), push(max(a, b))
    ELEMENT_AND                     = 0x90,         // pop(b), pop(a), push(a AND b)
    ELEMENT_OR                      = 0x91,         // pop(b), pop(a), push(a OR b)
    ELEMENT_XOR                     = 0x92,         // pop(b), pop(a), push(a XOR b)
    ELEMENT_MULTIPLY                = 0xC0,         // pop(b), pop(a), push(a x b)
    ELEMENT_DIVIDE                  = 0xC1,         // pop(b), pop(a), push(a / b)
    ELEMENT_MODULUS                 = 0xC2,         // pop(b), pop(a), push(a % b)
    ELEMENT_ADD                     = 0xC3,         // pop(b), pop(a), push(a + b)
    ELEMENT_SUBTRACT                = 0xC4,         // pop(b), pop(a), push(a - b)
    ELEMENT_MAP                     = 0xE1,         // pop(Xn) for each dimension, y=map(X1,X2,...), push(y)
    ELEMENT_INVALID                 = 0xFF
} ELEMENT_TYPE;

//! Methods used to look-up values in lists.
typedef enum LOOK_UP_METHOD_ENUM
{
    LOOK_UP_METHOD_DIRECT_VALUE     = 0,
    LOOK_UP_METHOD_ROUND_UP         = 1,
    LOOK_UP_METHOD_ROUND_DOWN       = 2,
    LOOK_UP_METHOD_INTERPOLATION    = 3,
    LOOK_UP_METHOD_NEAREST          = 6
} LOOK_UP_METHOD_TYPE;

//! A list of potential errors generated by the Equation module.
typedef enum MODULE_EQUATION_LOG_MESSAGE_ID_ENUM
{
    EQ_NO_ERROR                     = 0,
    EQ_INVALID_SF_EQUATION          = 1,
    EQ_UNSUPPORTED_EQUATION_ELEMENT = 2,
    EQ_UNBALANCED_EQUATION          = 3,
    EQ_STACK_UNDERFLOW              = 4,
    EQ_STACK_OVERFLOW               = 5,
    EQ_ARITHMETIC_OVERFLOW          = 6,
    EQ_DIVIDE_BY_ZERO               = 7,
    EQ_INVALID_VARIABLE             = 8,

    // Look-up table errors
    EQ_MAP_INVALID_SF_LOOK_UP_TABLE = 16,
    EQ_MAP_INVALID_INPUT_MIN_MAX    = 17,
    EQ_MAP_INVALID_SF_INPUT_LIST    = 18,
    EQ_MAP_INVALID_SF_OUTPUT_LIST   = 19,
    EQ_MAP_OUTPUT_LIST_WRONG_SIZE   = 20
} MODULE_EQUATION_LOG_MESSAGE_ID_TYPE;

//! Enumeration used to evaluate sign bits of left and right operands.
typedef enum SIGNS_ENUM
{
    LEFT_IS_POS_RIGHT_IS_POS,
    LEFT_IS_POS_RIGHT_IS_NEG,
    LEFT_IS_NEG_RIGHT_IS_POS,
    LEFT_IS_NEG_RIGHT_IS_NEG
} SIGNS_TYPE;


// -- Private Function Type Declarations --


// -- Private Structure Type Declarations --

//! Structure that holds variables for evaluating an equation.
//! Variables include the stack.
typedef struct EQ_STRUCT
{
    //! Holds 32-bit values pushed onto the stack as part of the equation evaluation.
    sint32 Data[EQUATION_STACK_SIZE];

    //! Pointer where the setting file data for this equation is located.
    SETTINGFILE_OFFSET_TYPE Pointer;

    //! Index to the element on the top of the stack's Data[] array.
    sint8 Index;

    //! Error associated with the equation evaluation.
    MODULE_EQUATION_LOG_MESSAGE_ID_TYPE Error;

    //! TRUE if the operation is part of a prediction; otherwise FALSE.
    //! Will use pre-determined predicted values for predictions.
    BOOL_TYPE Predict;
} EQ_TYPE;

//! The parameter sent to all operator functions.
typedef struct OPERATOR_PARAM_STRUCT
{
    uint8* Element_Params;
    sint32* Values;
    EQ_TYPE* Equation;
    sint8 Extra_Param;
} EQ_OPERATOR_PARAMS_TYPE;

//! A function type used by all operator functions.
typedef void (*FUNC_OPERATOR) (EQ_OPERATOR_PARAMS_TYPE params);

//! A structure that describes one of the input dimensions
typedef struct MAPPED_INPUT_STRUCT
{
    uint16 Index;                                   //!< A fixed-point index using INDEX_SCALE
    uint8 Size;                                     //!< The number of elements in the input dimension
} MAPPED_INPUT_TYPE;

//! The result of a binary search
typedef struct NEAR_VALUES_STRUCT
{
    sint32 Value_Lo;                                //!< The value from the list at Index_Lo
    sint32 Value_Hi;                                //!< The value from the list at Index_Hi
    uint8 Index_Lo;                                 //!< The highest index whose value is <= the input
    uint8 Index_Hi;                                 //!< The lowest index whose value is >= the input
} NEAR_VALUES_TYPE;

//! A structure used to describe operator elements that can be used in equations.
typedef struct OPERATOR_STRUCT
{
    FUNC_OPERATOR Function;
    ELEMENT_TYPE Element;
    uint8 Num_Of_Element_Params;
    sint8 Num_Of_Stack_Inputs;
    sint8 Extra_Param;
} OPERATOR_TYPE;

//! Setting file displacement data for an equation.
typedef struct EQUATION_STRUCT
{
    uint8 Size      : 6;
    uint8 Version   : 2;
    uint8 Data[63];
} SF_EQUATION_TYPE;

//! The setting file data structure for a list of values
typedef struct SF_LOOK_UP_LIST_STRUCT
{
    uint16 Num_Of_Values            : 12;
    uint16 Data_Type                : 4;            //!< Value is of type DATATYPE_TYPE from C_Extensions.h
    sint16 Values[MAX_OUTPUT_LIST_SIZE / sizeof(sint16)];
} SF_LOOK_UP_LIST_TYPE;

//! Setting file displacement data for a look-up table.
typedef struct LOOK_UP_TABLE_STRUCT
{
    uint8 Number_Of_Dimensions      : 4;
    uint8 Version                   : 4;
    LOOK_UP_METHOD_TYPE Method;
    uint16 List_Displacements[DISPLACEMENTS_ARRAY_SIZE];
} SF_LOOK_UP_TABLE_TYPE;

//! Part of the setting file for a multi-dimensional table that holds input search methods
typedef struct SF_METHOD_PAIR_STRUCT
{
    LOOK_UP_METHOD_TYPE Method2     : 4;
    LOOK_UP_METHOD_TYPE Method1     : 4;
} SF_METHOD_PAIR_TYPE;


// -- Private Macro Declarations --

//! Macro that returns the number of values that are on the equation stack.
#define STACK_USED(eq)              ((eq)->Index + 1)

//! Macro that returns TRUE if the equation stack is full.
#define STACK_IS_FULL(eq)           ((eq)->Index >= EQUATION_STACK_SIZE - 1)


//  --- Private Function Prototypes -----------------------------------------------------------------------------------

static sint32 Evaluate(SETTINGFILE_OFFSET_TYPE pointer, uint16 displacement, BOOL_TYPE predict, MODULE_EQUATION_LOG_MESSAGE_ID_TYPE* error_ptr);
static OPERATOR_TYPE FindOperator(ELEMENT_TYPE element);
static uint16 GetElement16Bit(uint8* element);
static uint32 GetElement32Bit(uint8* element);
static BOOL_TYPE GetPredictedValue(const MOCKED_VALUES_TYPE* predicted, uint8 index, sint32* value);
static SF_EQUATION_TYPE* GetSfEquationData(SETTINGFILE_OFFSET_TYPE pointer, uint16 displacement);
static SIGNS_TYPE GetSigns(sint32* values);
static void OpAbsoluteValue(EQ_OPERATOR_PARAMS_TYPE params);
static void OpAcuVariable(EQ_OPERATOR_PARAMS_TYPE params);
static void OpAdd(EQ_OPERATOR_PARAMS_TYPE params);
static void OpAnd(EQ_OPERATOR_PARAMS_TYPE params);
static void OpBoolean(EQ_OPERATOR_PARAMS_TYPE params);
static void OpCompare(EQ_OPERATOR_PARAMS_TYPE params);
static void OpConst(EQ_OPERATOR_PARAMS_TYPE params);
static void OpConstSInt8(EQ_OPERATOR_PARAMS_TYPE params);
static void OpConstSInt16(EQ_OPERATOR_PARAMS_TYPE params);
static void OpConstSInt32(EQ_OPERATOR_PARAMS_TYPE params);
static void OpConstUInt8(EQ_OPERATOR_PARAMS_TYPE params);
static void OpConstUInt16(EQ_OPERATOR_PARAMS_TYPE params);
static void OpConstUInt31(EQ_OPERATOR_PARAMS_TYPE params);
static void OpDivide(EQ_OPERATOR_PARAMS_TYPE params);
static void OpInvert(EQ_OPERATOR_PARAMS_TYPE params);

#if (EQUATION_MAX_MAP_DIMENSIONS > 0)
    static void OpMap(EQ_OPERATOR_PARAMS_TYPE params);
#endif

static void OpMaximum(EQ_OPERATOR_PARAMS_TYPE params);
static void OpMinimum(EQ_OPERATOR_PARAMS_TYPE params);
static void OpModulus(EQ_OPERATOR_PARAMS_TYPE params);
static void OpMultiply(EQ_OPERATOR_PARAMS_TYPE params);
static void OpNegate(EQ_OPERATOR_PARAMS_TYPE params);
static void OpNot(EQ_OPERATOR_PARAMS_TYPE params);
static void OpOr(EQ_OPERATOR_PARAMS_TYPE params);
static void OpScale(EQ_OPERATOR_PARAMS_TYPE params);
static void OpSubtract(EQ_OPERATOR_PARAMS_TYPE params);
static void OpXor(EQ_OPERATOR_PARAMS_TYPE params);

#if (SYSTEM_HMI_HANDLING == ENABLED)
    static void OpHmiVariable(EQ_OPERATOR_PARAMS_TYPE params);
#endif
#if ((SYSTEM_HMI_HANDLING == ENABLED) && (HMI_RECIPE_ENGINE_FEATURE == ENABLED))
    static void OpStepVariable(EQ_OPERATOR_PARAMS_TYPE params);
#endif

    static uint8 ProcessEquationElement(EQ_TYPE* equation, uint8* elements);

#if (EQUATION_MAX_MAP_DIMENSIONS > 0)
    static NEAR_VALUES_TYPE BSearchValueInList(sint32 input, SF_LOOK_UP_LIST_TYPE* list);
    static uint16 CalculateIndexFromNearValues(LOOK_UP_METHOD_TYPE method, NEAR_VALUES_TYPE near, sint32 input);
    static sint32 CalculateOutputFromNearValues(sint32 value_lo, sint32 value_hi, LOOK_UP_METHOD_TYPE method, sint16 weight);
    static sint32* GetAddressOfTopStackValues(EQ_TYPE* equation, uint8 count);
    static sint32 GetListValueByIndex(SF_LOOK_UP_LIST_TYPE* list, uint16 index);
    static LOOK_UP_METHOD_TYPE GetLookUpMethod(SF_METHOD_PAIR_TYPE* methods, uint8 input_index);
    static SF_LOOK_UP_LIST_TYPE* GetSfListData(SETTINGFILE_OFFSET_TYPE pointer, uint16 displacement);
    static SF_LOOK_UP_TABLE_TYPE* GetSfTableData(SETTINGFILE_OFFSET_TYPE pointer, uint16 displacement);
    static sint32 MapIndicesToOutput(uint8 dimensions, uint8* sizes, uint16* indices, SF_LOOK_UP_LIST_TYPE* list, LOOK_UP_METHOD_TYPE method);
    static void MapInputsToOutput(EQ_TYPE* equation, SF_LOOK_UP_TABLE_TYPE* table);
    static MAPPED_INPUT_TYPE MapInputToIndex(EQ_TYPE* equation, uint16 displacement, sint32 input, LOOK_UP_METHOD_TYPE method);
#endif


//  --- Private Definitions -------------------------------------------------------------------------------------------


// -- Private Flash-Based Constant Definitions --

#if (EQUATION_MAX_MAP_DIMENSIONS > 0)
    //! A list of sizes for the various DATATYPE_TYPE types defined in C_Extensions.h
    static const uint8 DATATYPE_SIZES[] =
    {
       2,          //!< DATATYPE_DISPLACEMENT
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
       2           //!< DATATYPE_IS_Q15_CELSIUS
    };
#endif      // #if (EQUATION_MAX_MAP_DIMENSIONS > 0)

//! An array of data structure that describe the characteristics of each equation element's behavior.
static const OPERATOR_TYPE OPERATOR_FUNCTIONS[] =
{
//  { Function,         Element,                    Size,   Inputs, Extra   },  // Operator Description
    { OpConst,          ELEMENT_ZERO,               1,      0,      0       },  // push(0)
    { OpConst,          ELEMENT_ONE,                1,      0,      1       },  // push(1)
    { OpConst,          ELEMENT_NEGATIVE_ONE,       1,      0,     -1       },  // push(-1)
    { OpAcuVariable,    ELEMENT_ACU_VAR,            3,      0,      0       },  // push(ACU_Var(index, offset))
#if (SYSTEM_HMI_HANDLING == ENABLED)
    { OpHmiVariable,    ELEMENT_HMI_VAR,            3,      0,      0       },  // push(HMI_Var(index, offset))
#endif
#if ((SYSTEM_HMI_HANDLING == ENABLED) && (HMI_RECIPE_ENGINE_FEATURE == ENABLED))
    { OpStepVariable,   ELEMENT_STEP_VAR,           3,      0,      0       },  // push(Step_Var(index))
#endif
    { OpConstUInt8,     ELEMENT_CONST_UINT8,        2,      0,      0       },  // push(constant uint8)
    { OpConstUInt16,    ELEMENT_CONST_UINT16,       3,      0,      0       },  // push(constant uint16)
    { OpConstUInt31,    ELEMENT_CONST_UINT31,       5,      0,      0       },  // push(constant uint31)
    { OpConstSInt8,     ELEMENT_CONST_SINT8,        2,      0,      0       },  // push(constant sint8)
    { OpConstSInt16,    ELEMENT_CONST_SINT16,       3,      0,      0       },  // push(constant sint16)
    { OpConstSInt32,    ELEMENT_CONST_SINT32,       5,      0,      0       },  // push(constant sint32)
    { OpBoolean,        ELEMENT_BOOLEAN,            1,      1,      0       },  // pop(a), push(BOOL(a))
    { OpNot,            ELEMENT_NOT,                1,      1,      0       },  // pop(a), push(NOT BOOL(a))
    { OpInvert,         ELEMENT_INVERT,             1,      1,      0       },  // pop(a), push(~a)
    { OpNegate,         ELEMENT_NEGATE,             1,      1,      0       },  // pop(a), push(-a)
    { OpAbsoluteValue,  ELEMENT_ABSOLUTE_VALUE,     1,      1,      0       },  // pop(a), push(|a|)
    { OpScale,          ELEMENT_SCALE,              5,      1,      0       },  // pop(a), push(lroundf(constant float32 x a))
    { OpCompare,        ELEMENT_LESS_THAN,          1,      2,      CMP_LT  },  // pop(b), pop(a), push(a < b)
    { OpCompare,        ELEMENT_EQUAL,              1,      2,      CMP_EQ  },  // pop(b), pop(a), push(a == b)
    { OpCompare,        ELEMENT_LESS_OR_EQUAL,      1,      2,      CMP_LE  },  // pop(b), pop(a), push(a <= b)
    { OpCompare,        ELEMENT_GREATER_THAN,       1,      2,      CMP_GT  },  // pop(b), pop(a), push(a > b)
    { OpCompare,        ELEMENT_NOT_EQUAL,          1,      2,      CMP_NE  },  // pop(b), pop(a), push(a != b)
    { OpCompare,        ELEMENT_GREATER_OR_EQUAL,   1,      2,      CMP_GE  },  // pop(b), pop(a), push(a >= b)
    { OpMinimum,        ELEMENT_MINIMUM,            1,      2,      0       },  // pop(b), pop(a), push(min(a, b))
    { OpMaximum,        ELEMENT_MAXIMUM,            1,      2,      0       },  // pop(b), pop(a), push(max(a, b))
    { OpAnd,            ELEMENT_AND,                1,      2,      0       },  // pop(b), pop(a), push(a AND b)
    { OpOr,             ELEMENT_OR,                 1,      2,      0       },  // pop(b), pop(a), push(a OR b)
    { OpXor,            ELEMENT_XOR,                1,      2,      0       },  // pop(b), pop(a), push(a XOR b)
    { OpMultiply,       ELEMENT_MULTIPLY,           1,      2,      0       },  // pop(b), pop(a), push(a x b)
    { OpDivide,         ELEMENT_DIVIDE,             1,      2,      0       },  // pop(b), pop(a), push(a / b)
    { OpModulus,        ELEMENT_MODULUS,            1,      2,      0       },  // pop(b), pop(a), push(a % b)
    { OpAdd,            ELEMENT_ADD,                1,      2,      0       },  // pop(b), pop(a), push(a + b)
    { OpSubtract,       ELEMENT_SUBTRACT,           1,      2,      0       },  // pop(b), pop(a), push(a - b)
#if (EQUATION_MAX_MAP_DIMENSIONS > 0)
    { OpMap,            ELEMENT_MAP,                3,      1,      0       }   // pop(Xn) for each dimension, y=map(X1,X2,...), push(y)
#endif
};


// -- Private Variable Definitions --


//=====================================================================================================================
//  --- Public Functions ----------------------------------------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * Evaluate an equation as a condition with a Boolean result.
 * @param pointer = Setting file pointer with the equation displacement data.
 * @param displacement = Setting file displacement with the equation definition.
 * @return The Boolean result of the condition.
 */
BOOL_TYPE Equation__CheckCondition(SETTINGFILE_OFFSET_TYPE pointer, uint16 displacement)
{
    return Evaluate(pointer, displacement, FALSE, NULL) == 0 ? FALSE : TRUE;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Evaluate an equation as a condition based on mocked variables with a Boolean result.
 * @param pointer = Setting file pointer with the equation displacement data.
 * @param displacement = Setting file displacement with the equation definition.
 * @return The Boolean result of the condition.
 */
BOOL_TYPE Equation__PredictCondition(SETTINGFILE_OFFSET_TYPE pointer, uint16 displacement)
{
    return Evaluate(pointer, displacement, TRUE, NULL) == 0 ? FALSE : TRUE;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Evaluate an equation.
 * @param pointer = Setting file pointer with the equation displacement data.
 * @param displacement = Setting file displacement with the equation definition.
 * @return The result of the equation.
 */
sint32 Equation__Evaluate(SETTINGFILE_OFFSET_TYPE pointer, uint16 displacement)
{
    return Evaluate(pointer, displacement, FALSE, NULL);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Evaluate an equation based on mocked variables.
 * @param pointer = Setting file pointer with the equation displacement data.
 * @param displacement = Setting file displacement with the equation definition.
 * @return The Boolean result of the condition.
 */
sint32 Equation__Predict(SETTINGFILE_OFFSET_TYPE pointer, uint16 displacement)
{
    return Evaluate(pointer, displacement, TRUE, NULL);
}


//=====================================================================================================================
//  --- Private Functions ---------------------------------------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * Evaluate an equation and return the result.
 * @param pointer = Setting file pointer with the equation displacement data.
 * @param displacement = Setting file displacement with the equation definition.
 * @param predict = If TRUE, then predicted values will replace variable values where appropriate.
 * @param error_ptr = Pointer to byte to fill with an error code if an error occurs.
 *                    Use NULL to ignore errors.
 *                    Set byte to EQ_NO_ERROR (0) before calling to track errors.
 *                    If no error occurs, byte is left unchanged.
 * @return The result of the equation.
 */
static sint32 Evaluate(SETTINGFILE_OFFSET_TYPE pointer, uint16 displacement, BOOL_TYPE predict, MODULE_EQUATION_LOG_MESSAGE_ID_TYPE* error_ptr)
{
    EQ_TYPE equation;
    equation.Predict = predict;
    equation.Error = EQ_NO_ERROR;

    SF_EQUATION_TYPE* sf_equation = GetSfEquationData(pointer, displacement);
    if (sf_equation == NULL)
    {
        equation.Error = EQ_INVALID_SF_EQUATION;
    }
    else
    {
        equation.Pointer = pointer;
        equation.Index = -1;
        uint8 eq_index = 0;
        while ((equation.Error == EQ_NO_ERROR) && (eq_index < sf_equation->Size))
        {
            uint8 consumed = ProcessEquationElement(&equation, &sf_equation->Data[eq_index]);
            eq_index += consumed;
        }

        if ((equation.Error == EQ_NO_ERROR) &&
            ((eq_index != sf_equation->Size) || (equation.Index != 0)))
        {
            equation.Error = EQ_UNBALANCED_EQUATION;
        }
    }
    if (error_ptr != NULL)
    {
        *error_ptr = equation.Error;
    }
    else if (equation.Error != EQ_NO_ERROR)
    {
        LOG_ADD_EXCEPTION(equation.Error, displacement);
    }
    return (equation.Error == EQ_NO_ERROR) ? equation.Data[0] : 0L;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Find the Operator specified by element in the OPERATOR_FUNCTIONS[] array.
 * @param element = The element to match to the 'Element' field in the array.
 * @return The entry from the array.
 */
static OPERATOR_TYPE FindOperator(ELEMENT_TYPE element)
{
    OPERATOR_TYPE op;
    op.Element = ELEMENT_INVALID;
    op.Num_Of_Element_Params = 12;
    uint8 index_lo = 0;
    uint8 index_hi = ELEMENTS_IN_ARRAY(OPERATOR_FUNCTIONS) - 1;
    while (index_lo <= index_hi)
    {
        uint8 index_mid = (index_lo + index_hi) / 2;
        ELEMENT_TYPE elem = OPERATOR_FUNCTIONS[index_mid].Element;
        if (element < elem)
        {
            index_hi = index_mid - 1;
        }
        else if (element > elem)
        {
            index_lo = index_mid + 1;
        }
        else
        {
            op = OPERATOR_FUNCTIONS[index_mid];
            break;
        }
    }
    return op;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get a 16-bit value from the equation elements regardless of memory alignment.
 * @param element = Pointer to the first element of the value.
 * @return A memory-aligned 16-bit value
 */
static uint16 GetElement16Bit(uint8* element)
{
    return (uint16)COMBINE_BYTES(element[1], element[0]);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get a 32-bit value from the equation elements regardless of memory alignment.
 * @param element = Pointer to the first element of the value.
 * @return A memory-aligned 32-bit value
 */
static uint32 GetElement32Bit(uint8* element)
{
    return (uint32)COMBINE_WORDS(COMBINE_BYTES(element[3], element[2]),
                                 COMBINE_BYTES(element[1], element[0]));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the mocked variable values used for predicting equation results.
 * @param predicted = The list of predicted values of a given type (i.e. ACU, HMI, Recipe).
 * @param index = The index of the variable.
 * @param value = The address of the variable to set.
 * @return The mocked variable values used for predicting equation results.
 */
static BOOL_TYPE GetPredictedValue(const MOCKED_VALUES_TYPE* predicted, uint8 index, sint32* value)
{
    uint8* ptr_index = memchr(predicted->Indices, index, predicted->Num_Of_Values);
    if (ptr_index != NULL)
    {
        *value = predicted->Values[ptr_index - predicted->Indices];
        return TRUE;
    }
    return FALSE;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Read an equation definition from the setting file displacement data.
 * @param pointer = Setting file pointer with the equation displacement data.
 * @param displacement = Displacement index to read.
 * @return Pointer to the displacement data or NULL if an error occurs.
 */
static SF_EQUATION_TYPE* GetSfEquationData(SETTINGFILE_OFFSET_TYPE pointer, uint16 displacement)
{
    SF_EQUATION_TYPE* sf_equation = NULL;
    SETTINGFILE_LOADER_TYPE loader;
    if (SettingFile__BasicLoader(pointer, displacement, &loader) == PASS)
    {
        sf_equation = (void*)loader.Data;
        if (sf_equation->Version != 0)
        {
            sf_equation = NULL;
        }
    }
    return sf_equation;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the signs (positive or negative) of the two signed values values[0] and values[1].
 * @param values = Pointer to the two signed values to evaluate.
 * @return An enumeration that describes the signs of the two signed values.
 */
static SIGNS_TYPE GetSigns(sint32* values)
{
    SIGNS_TYPE signs;
    if (values[0] < 0)
    {
        signs = (values[1] < 0) ? LEFT_IS_NEG_RIGHT_IS_NEG : LEFT_IS_NEG_RIGHT_IS_POS;
    }
    else
    {
        signs = (values[1] < 0) ? LEFT_IS_POS_RIGHT_IS_NEG : LEFT_IS_POS_RIGHT_IS_POS;
    }
    return signs;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Replace the value on the top of the equation stack with its absolute value.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpAbsoluteValue(EQ_OPERATOR_PARAMS_TYPE params)
{
    if (params.Values[0] > LONG_MIN)
    {
        params.Values[0] = labs(params.Values[0]);
    }
    else
    {
        params.Equation->Error = EQ_ARITHMETIC_OVERFLOW;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Add an ACU Variable's value onto the equation stack.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpAcuVariable(EQ_OPERATOR_PARAMS_TYPE params)
{
    uint8 index = params.Element_Params[0];
    if ((params.Equation->Predict == FALSE) ||
        (GetPredictedValue(&MOCKED_ACU_VAR_VALUES, index, params.Values) == FALSE))
    {
        uint8 offset = params.Element_Params[1];
        if (Variable__GetValue((VARIABLE_ID_TYPE)index, offset, params.Values) == FALSE)
        {
            params.Equation->Error = EQ_INVALID_VARIABLE;
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Replace two values on the top of the equation stack with their sum.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpAdd(EQ_OPERATOR_PARAMS_TYPE params)
{
    SIGNS_TYPE signs = GetSigns(params.Values);
    if ((signs == LEFT_IS_NEG_RIGHT_IS_NEG) && (params.Values[0] < LONG_MIN - params.Values[1]))
    {
        params.Equation->Error = EQ_ARITHMETIC_OVERFLOW;
    }
    else if ((signs == LEFT_IS_POS_RIGHT_IS_POS) && (params.Values[0] > LONG_MAX - params.Values[1]))
    {
        params.Equation->Error = EQ_ARITHMETIC_OVERFLOW;
    }
    else
    {
        params.Values[0] = params.Values[0] + params.Values[1];
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Replace two values on the top of the equation stack with their logical AND.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpAnd(EQ_OPERATOR_PARAMS_TYPE params)
{
    params.Values[0] = params.Values[0] & params.Values[1];
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Convert the value on the top of the equation stack to its Boolean equivalent.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpBoolean(EQ_OPERATOR_PARAMS_TYPE params)
{
    if (params.Values[0] != 0)
    {
        params.Values[0] = 1;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Replace two values on the top of the equation stack with the Boolean result of their comparison.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpCompare(EQ_OPERATOR_PARAMS_TYPE params)
{
    uint8 result = 0;
    if (params.Extra_Param & CMP_LT)
    {
        result |= (params.Values[0] < params.Values[1]);
    }
    if (params.Extra_Param & CMP_EQ)
    {
        result |= (params.Values[0] == params.Values[1]);
    }
    if (params.Extra_Param & CMP_GT)
    {
        result |= (params.Values[0] > params.Values[1]);
    }
    params.Values[0] = result;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Add a constant to the top of the equation stack.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpConst(EQ_OPERATOR_PARAMS_TYPE params)
{
    params.Values[0] = params.Extra_Param;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Add a constant to the top of the equation stack.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpConstSInt8(EQ_OPERATOR_PARAMS_TYPE params)
{
    params.Values[0] = (sint8)params.Element_Params[0];
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Add a constant to the top of the equation stack.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpConstSInt16(EQ_OPERATOR_PARAMS_TYPE params)
{
    params.Values[0] = (sint16)GetElement16Bit(params.Element_Params);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Add a constant to the top of the equation stack.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpConstSInt32(EQ_OPERATOR_PARAMS_TYPE params)
{
    params.Values[0] = (sint32)GetElement32Bit(params.Element_Params);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Add a constant to the top of the equation stack.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpConstUInt8(EQ_OPERATOR_PARAMS_TYPE params)
{
    params.Values[0] = params.Element_Params[0];
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Add a constant to the top of the equation stack.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpConstUInt16(EQ_OPERATOR_PARAMS_TYPE params)
{
    params.Values[0] = (uint16)GetElement16Bit(params.Element_Params);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Add a constant to the top of the equation stack.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpConstUInt31(EQ_OPERATOR_PARAMS_TYPE params)
{
    uint32 value = (uint32)GetElement32Bit(params.Element_Params);
    if (BIT_TEST(value, 31))
    {
        params.Equation->Error = EQ_ARITHMETIC_OVERFLOW;
    }
    else
    {
        params.Values[0] = value;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Replace two values on the top of the equation stack with their quotient.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpDivide(EQ_OPERATOR_PARAMS_TYPE params)
{
    if (params.Values[1] == 0)
    {
        params.Equation->Error = EQ_DIVIDE_BY_ZERO;
    }
    else if ((params.Values[0] == LONG_MIN) && (params.Values[1] == -1))
    {
        params.Equation->Error = EQ_ARITHMETIC_OVERFLOW;
    }
    else
    {
        sint32 quotient = params.Values[0] / params.Values[1];
        sint32 remainder = params.Values[0] % params.Values[1];
        if (labs(remainder) >= labs(params.Values[1] / 2))
        {
            SIGNS_TYPE signs = GetSigns(params.Values);
            if ((signs == LEFT_IS_POS_RIGHT_IS_NEG) || (signs == LEFT_IS_NEG_RIGHT_IS_POS))
            {
                quotient--;
            }
            else
            {
                quotient++;
            }
        }
        params.Values[0] = quotient;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Replace the value on the top of the equation stack with its bitwise inversion.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpInvert(EQ_OPERATOR_PARAMS_TYPE params)
{
    params.Values[0] = ~(params.Values[0]);
}

#if (EQUATION_MAX_MAP_DIMENSIONS > 0)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Replace one or more values from the equation stack with the result from a multi-dimensional look-up table.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpMap(EQ_OPERATOR_PARAMS_TYPE params)
{
    uint16 displacement = GetElement16Bit(params.Element_Params);
    SF_LOOK_UP_TABLE_TYPE* table = GetSfTableData(params.Equation->Pointer, displacement);
    if (table == NULL)
    {
        params.Equation->Error = EQ_MAP_INVALID_SF_LOOK_UP_TABLE;
    }
    else
    {
        MapInputsToOutput(params.Equation, table);
    }
}
#endif      // #if (EQUATION_MAX_MAP_DIMENSIONS > 0)

//---------------------------------------------------------------------------------------------------------------------
/**
 * Replace two values on the top of the equation stack with their maximum.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpMaximum(EQ_OPERATOR_PARAMS_TYPE params)
{
    if (params.Values[1] > params.Values[0])
    {
        params.Values[0] = params.Values[1];
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Replace two values on the top of the equation stack with their minimum.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpMinimum(EQ_OPERATOR_PARAMS_TYPE params)
{
    if (params.Values[1] < params.Values[0])
    {
        params.Values[0] = params.Values[1];
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Replace two values on the top of the equation stack with their remainder after a division.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpModulus(EQ_OPERATOR_PARAMS_TYPE params)
{
    if (params.Values[1] == 0)
    {
        params.Equation->Error = EQ_DIVIDE_BY_ZERO;
    }
    else
    {
        if (labs(params.Values[1]) == 1)
        {
            params.Values[0] = 0;
        }
        else
        {
            params.Values[0] = params.Values[0] % params.Values[1];
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Replace two values on the top of the equation stack with their product after a multiplication.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpMultiply(EQ_OPERATOR_PARAMS_TYPE params)
{
    if ((params.Values[0] == 0) || (params.Values[1] == 0))
    {
        params.Values[0] = 0;
    }
    else
    {
        SIGNS_TYPE signs = GetSigns(params.Values);
        switch (signs)
        {
            case LEFT_IS_POS_RIGHT_IS_POS:
                if (params.Values[0] > LONG_MAX / params.Values[1])
                {
                    params.Equation->Error = EQ_ARITHMETIC_OVERFLOW;
                }
                break;
            case LEFT_IS_POS_RIGHT_IS_NEG:
                if (params.Values[0] > LONG_MIN / params.Values[1])
                {
                    params.Equation->Error = EQ_ARITHMETIC_OVERFLOW;
                }
                break;
            case LEFT_IS_NEG_RIGHT_IS_POS:
                if (params.Values[0] < LONG_MIN / params.Values[1])
                {
                    params.Equation->Error = EQ_ARITHMETIC_OVERFLOW;
                }
                break;
            case LEFT_IS_NEG_RIGHT_IS_NEG:
            default:
                if (params.Values[0] < LONG_MAX / params.Values[1])
                {
                    params.Equation->Error = EQ_ARITHMETIC_OVERFLOW;
                }
                break;
        }

        if (params.Equation->Error == EQ_NO_ERROR)
        {
            params.Values[0] = params.Values[0] * params.Values[1];
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Replace the value on the top of the equation stack with its negative value.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpNegate(EQ_OPERATOR_PARAMS_TYPE params)
{
    if (params.Values[0] > LONG_MIN)
    {
        params.Values[0] = -(params.Values[0]);
    }
    else
    {
        params.Equation->Error = EQ_ARITHMETIC_OVERFLOW;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Replace the value on the top of the equation stack with its inverted Boolean value.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpNot(EQ_OPERATOR_PARAMS_TYPE params)
{
    params.Values[0] = (params.Values[0] == 0) ? 1 : 0;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Replace two values on the top of the equation stack with their logical OR.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpOr(EQ_OPERATOR_PARAMS_TYPE params)
{
    params.Values[0] = params.Values[0] | params.Values[1];
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Replace the value on the top of the equation stack after multiplying it by a coefficient.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpScale(EQ_OPERATOR_PARAMS_TYPE params)
{
    union FLOAT_INT_UNION
    {
        float F;
        uint32 I;
    } overlap;
    overlap.I = GetElement32Bit(params.Element_Params);
    float scale = overlap.F;
    feclearexcept(FE_ALL_EXCEPT);
    float32 product = scale * (params.Values[0]);
    #ifdef FE_OVERFLOW
    if (fetestexcept(FE_OVERFLOW))
    {
        params.Equation->Error = EQ_ARITHMETIC_OVERFLOW;
    }
    else
    #endif
    {
        params.Values[0] = lroundf(product);
        #ifdef FE_INVALID
        if (fetestexcept(FE_INVALID))
        {
            params.Equation->Error = EQ_ARITHMETIC_OVERFLOW;
        }
        #endif
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Replace two values on the top of the equation stack with their difference.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpSubtract(EQ_OPERATOR_PARAMS_TYPE params)
{
    SIGNS_TYPE signs = GetSigns(params.Values);
    if ((signs == LEFT_IS_POS_RIGHT_IS_NEG) && (params.Values[0] > LONG_MAX + params.Values[1]))
    {
        params.Equation->Error = EQ_ARITHMETIC_OVERFLOW;
    }
    else if ((signs == LEFT_IS_NEG_RIGHT_IS_POS) && (params.Values[0] < LONG_MIN + params.Values[1]))
    {
        params.Equation->Error = EQ_ARITHMETIC_OVERFLOW;
    }

    if (params.Equation->Error == EQ_NO_ERROR)
    {
        params.Values[0] = params.Values[0] - params.Values[1];
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Replace two values on the top of the equation stack with their logical XOR.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpXor(EQ_OPERATOR_PARAMS_TYPE params)
{
    params.Values[0] = params.Values[0] ^ params.Values[1];
}

#if (SYSTEM_HMI_HANDLING == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Add an HMI Variable's value onto the equation stack.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpHmiVariable(EQ_OPERATOR_PARAMS_TYPE params)
{
    uint8 index = params.Element_Params[0];
    uint8 offset = params.Element_Params[1];
    if ((params.Equation->Predict == FALSE) ||
        (index != HMI_VARIABLE_TYPE_VARIABLE) ||
        (GetPredictedValue(&MOCKED_HMI_VAR_VALUES, offset, params.Values) == FALSE))
    {
        if (HmiVariable__Get(index, offset, params.Values) == FALSE)
        {
            params.Equation->Error = EQ_INVALID_VARIABLE;
        }
    }
}
#endif      // #if (SYSTEM_HMI_HANDLING == ENABLED)

#if ((SYSTEM_HMI_HANDLING == ENABLED) && (HMI_RECIPE_ENGINE_FEATURE == ENABLED))
//---------------------------------------------------------------------------------------------------------------------
/**
 * Add an Step Variable's value onto the equation stack.
 * @param params = Standard parameters used by all equation elements.
 */
static void OpStepVariable(EQ_OPERATOR_PARAMS_TYPE params)
{
    uint8 index = params.Element_Params[0];
    if ((params.Equation->Predict == FALSE) ||
        (GetPredictedValue(&MOCKED_STEP_VAR_VALUES, index, params.Values) == FALSE))
    {
        uint8 offset = params.Element_Params[1];
        params.Values[0] = StepData__GetVariable(index, offset);
    }
}
#endif      // #if ((SYSTEM_HMI_HANDLING == ENABLED) && (HMI_RECIPE_ENGINE_FEATURE == ENABLED))

//---------------------------------------------------------------------------------------------------------------------
/**
 * Process an equation element.
 * @param equation = The equation with the element to process (if any).
 *                   The output from the equation element (if any) is placed on the stack.
 * @param elements = Pointer to the equation element and parameters to process.
 * @return The number of equation element bytes consumed.
 */
static uint8 ProcessEquationElement(EQ_TYPE* equation, uint8* elements)
{
    ELEMENT_TYPE element = (ELEMENT_TYPE)elements[0];
    OPERATOR_TYPE op = FindOperator(element);
    if (op.Element == ELEMENT_INVALID)
    {
        equation->Error = EQ_UNSUPPORTED_EQUATION_ELEMENT;
    }
    else if (op.Num_Of_Stack_Inputs > STACK_USED(equation))
    {
        equation->Error = EQ_STACK_UNDERFLOW;
    }
    else if ((op.Num_Of_Stack_Inputs == 0) &&
             (STACK_IS_FULL(equation)))
    {
        equation->Error = EQ_STACK_OVERFLOW;
    }
    else
    {
        EQ_OPERATOR_PARAMS_TYPE params;
        params.Element_Params = &elements[1];
        params.Values = &equation->Data[equation->Index - op.Num_Of_Stack_Inputs + 1];
        params.Equation = equation;
        params.Extra_Param = op.Extra_Param;
        op.Function(params);
        if (op.Num_Of_Stack_Inputs != 1)
        {
            equation->Index -= op.Num_Of_Stack_Inputs - 1;
        }
    }
    return op.Num_Of_Element_Params;
//    DELETE: return ((equation->Error == EQ_NO_ERROR) ? op.Num_Of_Element_Params : 0);
}

#if (EQUATION_MAX_MAP_DIMENSIONS > 0)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Perform a binary search for the specified input value in the specified list.
 * Finds the upper and lower adjacent values in the specified list.
 * If the specified input's exact value is in the list, then the adjacent values are the same.
 * @param input = The input to find in the list.
 * @param list = A pointer to a list in the setting file.
 * @return The upper and lower adjacent values from the list.
 */
static NEAR_VALUES_TYPE BSearchValueInList(sint32 input, SF_LOOK_UP_LIST_TYPE* list)
{
    uint8 index_lo = 0;
    sint32 value_lo = GetListValueByIndex(list, index_lo);
    uint8 index_hi = (uint8)(list->Num_Of_Values - 1);
    sint32 value_hi = GetListValueByIndex(list, index_hi);
    if (input <= value_lo)
    {
        index_hi = index_lo;
        value_hi = value_lo;
    }
    else if (input >= value_hi)
    {
        index_lo = index_hi;
        value_lo = value_hi;
    }
    else
    {
        while (index_lo + 1 < index_hi)
        {
            uint16 index_mid = (index_lo + index_hi) / 2;
            sint32 value_mid = GetListValueByIndex(list, index_mid);
            if (input > value_mid)
            {
                index_lo = index_mid;
                value_lo = value_mid;
            }
            else if (input < value_mid)
            {
                index_hi = index_mid;
                value_hi = value_mid;
            }
            else
            {
                index_lo = index_mid;
                value_lo = value_mid;
                index_hi = index_mid;
                value_hi = value_mid;
            }
        }
    }
    NEAR_VALUES_TYPE result = { value_lo, value_hi, index_lo, index_hi };
    return result;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Determine the fixed point index for the specified input value compared to the specified near_values.
 * @param method = The method used to determine the fixed point index.
 *                 ROUND_DOWN: Choose the lower index unless the input value matches the upper value.
 *                 ROUND_UP: Choose the upper index unless the input value matches the lower value.
 *                 INTERPOLATION: Calculate an interpolated index. The ratio of the interpolated index
 *                                to the upper and lower indices will match the ratio of the input
 *                                value to the upper and lower values.
 *                 NEAREST: Choose the lower index if the input value is closer to the lower value.
 *                          Choose the upper index if the input value is closer to the upper value.
 * @param near_values = Upper and lower values with associated upper and lower indices
 * @param input = The input to compare to the upper and lower values.
 * @return The fixed-point index for the specified input value compared to the specified near_values.
 */
static uint16 CalculateIndexFromNearValues(LOOK_UP_METHOD_TYPE method, NEAR_VALUES_TYPE near_values, sint32 input)
{
    uint16 result;
    if ((near_values.Value_Lo == near_values.Value_Hi) ||
        (method == LOOK_UP_METHOD_ROUND_DOWN) ||
        ((method == LOOK_UP_METHOD_NEAREST) &&
         (input - near_values.Value_Lo < near_values.Value_Hi - input)))
    {
        result = INDEX_SCALE * near_values.Index_Lo;
    }
    else if (method == LOOK_UP_METHOD_INTERPOLATION)
    {
        // if statements guarantee that one of the following is true:
        //      near_values.Value_Lo < input && input < near_values.Value_Hi
        //      near_values.Value_Hi < input && input < near_values.Value_Lo
        // weight (U0.8) is the ratio: (input - near_values.Value_Lo) / (near_values.Value_Hi - near_values.Value_Lo)
        // The following calculation also rounds to the nearest 1/256 increment
        result = INDEX_SCALE * near_values.Index_Lo +
                 (2 * INDEX_SCALE * (input - near_values.Value_Lo) / (near_values.Value_Hi - near_values.Value_Lo) + 1) / 2;
    }
    else
    {
        result = INDEX_SCALE * near_values.Index_Hi;
    }
    return result;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Determine the output value from the specified near values using the specified method and weight
 * @param value_lo = A lower adjacent value
 * @param value_hi = An upper adjacent value
 * @param method = The method used to determine the output value.
 *                 ROUND_DOWN: Choose the lower adjacent value (weight is never 1.0)
 *                 ROUND_UP: Choose the upper adjacent value (weight is never 0.0)
 *                 INTERPOLATION: Calculate an interpolated value. The ratio of the interpolated value
 *                                to the upper and lower indices will match the weight
 *                 NEAREST: Choose the lower adjacent value if the weight < 0.5
 *                          Choose the upper adjacent value if the weight >= 0.5
 * @param weight = A fixed-point weight between 0.0 and 1.0 (not include 0.0 or 1.0)
 * @return The calculated output value
 */
static sint32 CalculateOutputFromNearValues(sint32 value_lo, sint32 value_hi, LOOK_UP_METHOD_TYPE method, sint16 weight)
{
    sint32 result;
    switch (method)
    {
        case LOOK_UP_METHOD_ROUND_UP:
            result = value_hi;
            break;
        case LOOK_UP_METHOD_ROUND_DOWN:
            result = value_lo;
            break;
        case LOOK_UP_METHOD_INTERPOLATION:
            // result is a value between the Y values (value_lo and value_hi) based on the weigth.
            // The following calculation also rounds to the nearest value.
            result = 2 * weight * (value_hi - value_lo) / INDEX_SCALE;
            result += (result > 0) - (result < 0);
            result = value_lo + result / 2;
            break;
        case LOOK_UP_METHOD_NEAREST:
        default:
            result = (weight < INDEX_SCALE / 2) ? value_lo : value_hi;
            break;
    }
    return result;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the address of the specified number of values on the top of the stack.
 * @param equation = The equation with the values.
 * @param count = The number of values to retrieve.
 * @return = The address of the specified number of values on the top of the stack.
 */
static sint32* GetAddressOfTopStackValues(EQ_TYPE* equation, uint8 count)
{
    sint8 offset = count - 1;
    if (equation->Index < offset)
    {
        equation->Error = EQ_STACK_UNDERFLOW;
        return NULL;
    }
    return &equation->Data[equation->Index - offset];
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Extract a value from the specified list of values at the specified index
 * The value's data type (and therefore size) is specified in the list.
 * @param list = Pointer to a list of values
 * @param index = The index used to select a value from the list
 * @return A value from the specified list of values at the specified index
 */
static sint32 GetListValueByIndex(SF_LOOK_UP_LIST_TYPE* list, uint16 index)
{
    sint32 value;
    void* values = list->Values;
    switch ((DATATYPE_TYPE)(list->Data_Type))
    {
        default:
        case DATATYPE_UINT8:
            value = ((uint8*)values)[index];
            break;
        case DATATYPE_DISPLACEMENT:
        case DATATYPE_UINT16:
            value = ((uint16*)values)[index];
            break;
        case DATATYPE_UINT32:
            // Setting file memory is only guaranteed to be 2-byte aligned (not 4-byte aligned).
            index = 2 * index;
            value = ((uint16*)values)[index + 1] << 16;
            value |= ((uint16*)values)[index];
            break;
        case DATATYPE_SINT8:
            value = ((sint8*)values)[index];
            break;
        case DATATYPE_SINT16:
        case DATATYPE_IS_Q15_CELSIUS:
            value = ((sint16*)values)[index];
            break;
        case DATATYPE_SINT32:
            // Setting file memory is only guaranteed to be 2-byte aligned (not 4-byte aligned).
            index = 2 * index;
            value = ((sint16*)values)[index + 1] << 16;
            value |= ((uint16*)values)[index];
            break;
    }
    return value;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Use the specified input_index to find the input's search method in the specified array of search methods
 * @param methods = An array of search methods as stored in the setting file
 * @param input_index = The index used to select the input's search method
 * @return The input's search method
 */
static LOOK_UP_METHOD_TYPE GetLookUpMethod(SF_METHOD_PAIR_TYPE* methods, uint8 input_index)
{
    uint8 index = input_index / 2;
    LOOK_UP_METHOD_TYPE method;
    if (input_index & 1)
    {
        method = methods[index].Method2;
    }
    else
    {
        method = methods[index].Method1;
    }
    return method;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get a reference to the list of values specified by the setting file displacement.
 * @param pointer = The setting file pointer that contains equation data.
 * @param displacement = The displacement for the requested list of values
 * @return A reference to the list of values specified by the setting file displacement.
 *         Return NULL if the list of values is unavailable or incomplete.
 */
static SF_LOOK_UP_LIST_TYPE* GetSfListData(SETTINGFILE_OFFSET_TYPE pointer, uint16 displacement)
{
    SF_LOOK_UP_LIST_TYPE* list = NULL;
    SETTINGFILE_LOADER_TYPE loader;
    if (SettingFile__BasicLoader(pointer, displacement, &loader) == PASS)
    {
        list = (void*)loader.Data;
        uint16 expected_length = list->Num_Of_Values * DATATYPE_SIZES[list->Data_Type] + 2;
        if (loader.Length < expected_length)
        {
            list = NULL;
        }
    }
    return list;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get a reference to the look-up table specified by the setting file displacement.
 * @param pointer = The setting file pointer that contains table data.
 * @param displacement = The displacement for the requested look-up table
 * @return A reference to the look-up table specified by the setting file displacement.
 *         Return NULL if the look-up table is unavailable, unsupported, or invalid.
 */
static SF_LOOK_UP_TABLE_TYPE* GetSfTableData(SETTINGFILE_OFFSET_TYPE pointer, uint16 displacement)
{
    SF_LOOK_UP_TABLE_TYPE* table = NULL;
    SETTINGFILE_LOADER_TYPE loader;
    if (SettingFile__BasicLoader(pointer, displacement, &loader) == PASS)
    {
        table = (void*)loader.Data;
        // Expected displacement data size by dimension:
        //  1D:7, 2D:9, 3D:12, 4D:14, 5D:17, 6D:19, 7D:22, 8D:24, 9D:27, 10D:29
        uint8 expected_length = 5 * (table->Number_Of_Dimensions + 1) / 2 + 2;
        if ((loader.Length < expected_length) ||
            (table->Version != 0) ||
            (table->Number_Of_Dimensions == 0) ||
            (table->Number_Of_Dimensions > EQUATION_MAX_MAP_DIMENSIONS))
        {
            table = NULL;
        }
    }
    return table;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Determine the output value
 * Uses multiple fixed-point input indices to select values in a multi-dimensional list of Y values.
 * Applies the specified search method to convert the near Y values from the array to a single output.
 * Sets an error code if an error occurs.
 * @param dimensions = The number of dimensions in the multi-dimensional array
 * @param sizes = The sizes of each dimension in the multi-dimensional array
 * @param indices = The fixed-point indices (using INDEX_SCALE) for each dimension of the multi-dimensional Y values.
 * @param list = The multi-dimensional array of Y values
 * @param method = The search method used to reduce the near values to a single output
 * @return The output value
 */
static sint32 MapIndicesToOutput(uint8 dimensions, uint8* sizes, uint16* indices, SF_LOOK_UP_LIST_TYPE* list, LOOK_UP_METHOD_TYPE method)
{
    sint32 values[1 << EQUATION_MAX_MAP_DIMENSIONS];
    uint16 near_points = (1 << dimensions);
    for (uint16 y = 0; y < near_points; y++)
    {
        uint16 index_y = 0;
        for (uint8 x = 0; x < dimensions; x++)
        {
            index_y *= sizes[x];
            index_y += indices[x] / INDEX_SCALE;
            uint16 weight = indices[x] % INDEX_SCALE;
            if (BIT_TEST(y, x) && (weight > 0))
            {
                index_y++;
            }
        }
        values[y] = GetListValueByIndex(list, index_y);
    }
    for (uint8 x = dimensions - 1; x < dimensions; x--)
    {
        sint16 weight = indices[x] % INDEX_SCALE;
        if (weight > 0)
        {
            uint16 delta_index = 1 << x;
            for (uint16 y = 0; y < delta_index; y++)
            {
                sint32 value_lo = values[y];        //lint !e771 Suppress Info: Symbol 'values' conceivably not initialized
                sint32 value_hi = values[y + delta_index];
                values[y] = CalculateOutputFromNearValues(value_lo, value_hi, method, weight);
            }
        }
    }
    return values[0];
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Look up the output from a multi-dimensional table based on the inputs provided
 * @param equation = The equation with the input values to use in the look-up table operation.
 *                   The result of the look-up table is replaces the input values on the stack.
 * @param table = Pointer to a multi-dimensional table from the setting file
 */
static void MapInputsToOutput(EQ_TYPE* equation, SF_LOOK_UP_TABLE_TYPE* table)
{
    uint8 dims = table->Number_Of_Dimensions;
    sint32* inputs = GetAddressOfTopStackValues(equation, dims);
    if (inputs != NULL)
    {
        uint32 size_y = 1;
        uint16 index_x[EQUATION_MAX_MAP_DIMENSIONS];
        uint8 size_x[EQUATION_MAX_MAP_DIMENSIONS];
        for (uint8 index = 0; index < dims; index++)
        {
            LOOK_UP_METHOD_TYPE method = GetLookUpMethod((void*)&table->List_Displacements[dims + 1], index);
            MAPPED_INPUT_TYPE input = MapInputToIndex(equation, table->List_Displacements[index], inputs[index], method);
            if (equation->Error != EQ_NO_ERROR)
            {
                break;
            }
            size_y *= input.Size;
            if (size_y > MAX_OUTPUT_LIST_ELEMENTS)
            {
                equation->Error = EQ_MAP_OUTPUT_LIST_WRONG_SIZE;
                break;
            }
            size_x[index] = input.Size;
            index_x[index] = input.Index;
        }
        if (equation->Error == EQ_NO_ERROR)
        {
            SF_LOOK_UP_LIST_TYPE* list = GetSfListData(equation->Pointer, table->List_Displacements[dims]);
            if (list == NULL)
            {
                equation->Error = EQ_MAP_INVALID_SF_OUTPUT_LIST;
            }
            else if (list->Num_Of_Values != size_y)
            {
                equation->Error = EQ_MAP_OUTPUT_LIST_WRONG_SIZE;
            }
            else
            {
                sint32* output = inputs;
                *output = MapIndicesToOutput(dims, size_x, index_x, list, table->Method);
                equation->Index -= dims - 1;
                if (list->Data_Type == DATATYPE_DISPLACEMENT)
                {
                    *output = Evaluate(equation->Pointer, *output, FALSE, &equation->Error);
                }
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Convert an input value to an input fixed-point index
 * @param equation = The equation parameters. Used to access the pointer with list data and record errors.
 * @param displacement = Displacement index to the list of values in the setting file
 * @param input = The input value to evaluate
 * @param method = The method to determine the index from the near values
 * @return The fixed-point index that describes the relative position of the input value in the list of values
 */
static MAPPED_INPUT_TYPE MapInputToIndex(EQ_TYPE* equation, uint16 displacement, sint32 input, LOOK_UP_METHOD_TYPE method)
{
    MAPPED_INPUT_TYPE result = { 0, 0 };
    if (method == LOOK_UP_METHOD_DIRECT_VALUE)
    {
        uint8 min_index = HIBYTE(displacement);
        uint8 max_index = LOBYTE(displacement);
        if (min_index > max_index)
        {
            equation->Error = EQ_MAP_INVALID_INPUT_MIN_MAX;
        }
        else
        {
            result.Size = max_index - min_index + 1;
            if (input > max_index)
            {
                result.Index = INDEX_SCALE * (max_index - min_index);
            }
            else if (input > min_index)
            {
                result.Index = INDEX_SCALE * (input - min_index);
            }
        //  else result.Index = 0; // 0 already set during variable initialization
        }
    }
    else
    {
        SF_LOOK_UP_LIST_TYPE* list = GetSfListData(equation->Pointer, displacement);
        if ((list == NULL) || (list->Num_Of_Values > MAX_INPUT_LIST_ELEMENTS))
        {
            equation->Error = EQ_MAP_INVALID_SF_INPUT_LIST;
        }
        else
        {
            NEAR_VALUES_TYPE near_values = BSearchValueInList(input, list);
            result.Size = list->Num_Of_Values;
            result.Index = CalculateIndexFromNearValues(method, near_values, input);
        }
    }
    return result;
}
#endif      // #if (EQUATION_MAX_MAP_DIMENSIONS > 0)

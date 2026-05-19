/**
 *    @file
 *    @defgroup   CLASS_B
 *
 *    @brief    This file contains the definition of commonly used Macros and Enumerations
 *
 *  $Header: C_Extensions.h 1.10 2015/07/28 09:57:53EDT Dasar Hanumanth (DASARHB) Exp  $
 *
 *   @copyright  Copyright 2007-$Date: 2015/07/28 09:57:53EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
#ifndef C_EXTENSIONS_H
    #define C_EXTENSIONS_H

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
//--------------------------------------------- STANDARD SYSTEM DEFINITIONS -------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

typedef enum _BOOL_TYPE
{
    FALSE           = 0,
    TRUE            = 1
} BOOL_TYPE;

typedef enum _ON_OFF_TYPE
{
    OFF             = 0,
    ON              = 1
} ON_OFF_TYPE;

typedef enum _PASS_FAIL_TYPE
{
    FAIL            = 0,
    PASS            = 1
} PASS_FAIL_TYPE;

#define OK              ((PASS_FAIL_TYPE)1)

typedef enum _COMPLETE_TYPE
{
    INCOMPLETE      = 0,
    COMPLETE        = 1
} COMPLETE_TYPE;

typedef enum _ACTIVE_TYPE
{
    INACTIVE        = 0,
    ACTIVE          = 1
} ACTIVE_TYPE;


//A constant pointer to a function with no return value or parameters.
typedef void (* const FUNCTION_TYPE)(void);

//A constant pointer to a list of TASK_TYPESs.
typedef void (* const * const FUNCTION_LIST_TYPE)(void);

//A NULL pointer to use as a terminator for function lists.
#define NULL_TASK ((FUNCTION_TYPE)0)


#ifndef ENABLED
    #define ENABLED         1
#endif //ENABLED

#ifndef DISABLED
    #define DISABLED        0
#endif //DISABLED

#ifndef NULL
    #define NULL            ((void *)0)
#endif //NULL

#ifndef UNAVAILABLE
    #define UNAVAILABLE     10
#endif //UNAVAILABLE

#ifndef AVAILABLE
    #define AVAILABLE       11
#endif //AVAILABLE

//---------------------------------------------------------------------------------------------------------------------
//--------------------------------------------- DATA ALIAS ------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
typedef unsigned char       uint8;
typedef unsigned short int  uint16;
typedef unsigned long int   uint32;
typedef unsigned long long  uint64;

typedef signed char         sint8;
typedef signed short int    sint16;
typedef signed long int     sint32;
typedef signed long long    sint64;

typedef float               float32;
typedef double              float64;

typedef enum
{
    DATATYPE_INVALID = 0,   //!< DATATYPE_INVALID
    DATATYPE_UINT8,         //!< DATATYPE_UINT8
    DATATYPE_UINT16,        //!< DATATYPE_UINT16
    DATATYPE_UINT32,        //!< DATATYPE_UINT32
    DATATYPE_UINT64,        //!< DATATYPE_UINT64
    DATATYPE_SINT8,         //!< DATATYPE_SINT8
    DATATYPE_SINT16,        //!< DATATYPE_SINT16
    DATATYPE_SINT32,        //!< DATATYPE_SINT32
    DATATYPE_SINT64,        //!< DATATYPE_SINT64
    DATATYPE_FLOAT32,       //!< DATATYPE_FLOAT32
    DATATYPE_FLOAT64,       //!< DATATYPE_FLOAT64
    DATATYPE_IS_ARRAY,      //!< DATATYPE_IS_ARRAY Use this type if you want to extern an array. Receiver will need to understand what this array is.
    DATATYPE_IS_REGULATION,  //!< DATATYPE_IS_REGULATION Use this type to extern the Regulation array.
	DATATYPE_IS_Q15_CELSIUS //!< DATATYPE_IS_Q15_CELSIUS is the special type of SINT16 to represent temperatures in celsius on the Q15 format.
}DATATYPE_TYPE;


//---------------------------------------------------------------------------------------------------------------------
//------------------- --------------------------BYTE AND WORD ACCESS --------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

#define LOWORD(mem)         ((uint16)(mem))   ///< This macro is for extracting lower word from a double word type of data
#define LOBYTE(mem)         ((uint8)(mem))    ///< This macro is for extracting lower byte from the word type of data
#define HIWORD(mem)         (uint16)(((uint32)(mem) >> 16 ) & 0xFFFFU)  ///< This macro is for extracting higher word from a double word type of data
#define HIBYTE(mem)         (uint8)(((uint16)(mem) >> 8) & 0xFFU)      ///<This macro is for extracting higher byte from a word type of data

#define ELEMENTS_IN_ARRAY(a)        (sizeof(a) / sizeof((a)[0]))
#define COMBINE_NIBBLES(hi,lo)      ((((uint8)(hi) << 4) & 0xF0U) | ((uint8)(lo) & 0x0FU))
#define COMBINE_BYTES(hi,lo)        ((((uint16)(hi) << 8) & 0xFF00U) | (uint8)(lo))
#define COMBINE_WORDS(hi,lo)        ((((uint32)(hi) << 16) & 0xFFFF0000U) | (uint16)(lo))


//---------------------------------------------------------------------------------------------------------------------
//--------------------------------------------- BIT MANIPULATION ------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

#define BIT_TEST(mem,bit)               (((uint32)(mem)>>(bit))&(1UL))    ///< This macro is used to perform test on bit of the data
#define BIT_SET(mem,bit)                ((mem)|=(1UL<<(bit)))   ///< This macro is used to set the bit in data of 32 bits
#define BIT_CLR(mem,bit)                ((mem)&=~(1UL<<(bit)))  ///< This macro is used to clear the bit in data of 32 bits
#define BIT_TOGGLE(mem,bit)             ((mem)^=(1UL<<(bit)))   ///< This macro is used to toggle the bit in data of 32 bits
#define BIT_STORAGE_SIZE(num_of_bits)   ((size_t)(((num_of_bits) + 7U) / 8U)) ///< this macro returns the number of bytes needed to store the amount bits requested.

#define BIT_SET16(mem,bit)              ((mem)=(uint16)((mem) | (1UL << (bit))))      ///< This macro is used to set the bit in data of 16 bits
#define BIT_CLR16(mem,bit)              ((mem)=(uint16)((mem) & (~(1UL << (bit)))))   ///< This macro is used to clear the bit in data of 16 bits
#define BIT_TOGGLE16(mem,bit)           ((mem)=(uint16)((mem)^(1UL << (bit))))        ///< This macro is used to clear the bit in data of 16 bits

#define BIT_SET8(mem,bit)               ((mem)=(uint8)((mem) | (1UL << (bit))))     ///< This macro is used to set the bit in data of 8 bits
#define BIT_CLR8(mem,bit)               ((mem)=(uint8)((mem) & (~(1UL << (bit)))))  ///< This macro is used to clear the bit in data of 8 bits
#define BIT_TOGGLE8(mem,bit)            ((mem)=(uint8)((mem)^(1UL << (bit))))       ///< This macro is used to clear the bit in data of 8 bits

//---------------------------------------------------------------------------------------------------------------------
//--------------------------------------------- CTAssert ------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

#ifndef CT_ASSERT   // to ensure it is not defined by the application somewhere else
    #ifdef __IAR_SYSTEMS_ICC__
        /**
         * Compile-time check to see if test condition is valid or not.
         *
         * This macro, after expansion of the helper macros, generates a compile-time error if the test is invalid. This error
         * will be your compiler's flavor of "invalid array dimension"; if you see this error at the place where you're doing
         * the test, you will know the test failed. \see _CTASSERT() and __CTASSERT().
         *
         * @param test - the logical test that is to be checked for validity.
         *
         * Usage: CT_ASSERT(<logical_text>);
         *
         * Example: CT_ASSERT((NUM_OF_ITEMS <= 255) && (sizeof(buffer) >= NUM_OF_ITEMS));
         *
         * @note
         *      - Macro only works for compile-time checking.  Run-time checks will not work.
         *      - This can only work where it is legal to declare a variable. Thus, for example, you
         *        cannot use it in a function after a line of executable code.
         */
        #define CT_ASSERT(test)              _CTASSERT(__LINE__, test)

        /**
         * First-level support macro, designed to work with CT_ASSERT(). It is not intended for direct use in your code.
         *
         * The purpose for this macro is solely and exclusively for the expansion of the __LINE__ argument passed by
         * CT_ASSERT(). The ANSI standard stipulates that when macros are concatenated,  the concatenation happens first, and
         * then the expansion; this means that this macro converts "__LINE__" to a numerical value, which is then passed to the
         * next macro. Without this intermediate macro, the end symbol would be "_CtAssert__LINE__[]", which is not the unique
         * symbol desired.
         *
         * @param UniqueId - a unique identifier, which is used to form the name of the external array.
         * @param test - the logical test that is to be checked for validity.
         */
        #define _CTASSERT(UniqueId, test)   __CTASSERT(UniqueId, test)

        /**
         * Second-level support macro, designed to work with CT_ASSERT(). It is not intended for direct use in your code.
         *
         * This macro uses the unique identifier passed to it, as part of the declaration of an external array. It then uses the
         * \b logical test as the size of the array. If the test is valid, it will evaluate to "1", and so the compiler will
         * see a declaration of an external array of size 1. If the test is invalid, the compiler will see a declaration of an
         * external array of size 0, which is not legal, and will cause a compilation error. Please note that the error emitted,
         * which should relate to invalid array dimension, will not relate to the thing being tested; the user should know that
         * when he receives this error, that the test was invalid.
         *
         * Please note that this macro relies on an intermediate macro, located between the API that is to be used in the source
         * code, and this macro, to properly expand all preprocessor symbols. In particular, if the symbol __LINE__ is used, it
         * must be expanded to a line number before being passed to this macro. Without the expansion, this macro will form a
         * symbol name of "_CtAssert__LINE__[]", which is not unique if the CT_ASSERT() macro is used multiple times within the
         * same source file.
         *
         * @param UniqueId - a unique identifier, which is used to form the name of the external array.
         * @param test - the logical test that is to be checked for validity.
         *
         * @note Regarding the suppressed PC-Lint warnings:
         * - The 506, suppresses "Constant value Boolean".
         * - The 515, suppresses "Symbol 'x' has arg. count conflict". The placement here is a convenience, as this frequently
         *   happens with keyword "defined()" which is used in quite a few modules.
         * - The #752, suppresses a "local declarator 'Symbol' (Location) not referenced" warning, caused because we
         *   declare an external symbol and then make no reference to it.
         * - The 762, suppresses "Redundantly declared symbol 'Symbol' previously declared at <Location>" which can
         *   happen if, for example, you would place multiple CT_ASSERT() statements on the same line, or if the same check
         *   appeared at the same line number in two different files.
         * - The 18, suppresses "redeclared w/ different size"
         */
        #define __CTASSERT(UniqueId, test)    /*lint -e{506} -e{515} --e{752} -e{762} -e{18}*/extern uint8 _CtAssert##UniqueId[(test)]
    #else
        #define CT_ASSERT(test)
    #endif
#endif



#include "Compiler_Defs.h"
#include "C_Extensions_prm.h"

#define BIT0_MASK   0x01            /**< \hideinitializer bit 0*/
#define BIT1_MASK   0x02            /**< \hideinitializer bit 1*/
#define BIT2_MASK   0x04            /**< \hideinitializer bit 2*/
#define BIT3_MASK   0x08            /**< \hideinitializer bit 3*/
#define BIT4_MASK   0x10            /**< \hideinitializer bit 4*/
#define BIT5_MASK   0x20            /**< \hideinitializer bit 5*/
#define BIT6_MASK   0x40            /**< \hideinitializer bit 6*/
#define BIT7_MASK   0x80            /**< \hideinitializer bit 7*/
#define ALLBIT_MASK 0xFF            /**< \hideinitializer all bit of a byte selected*/


/**
 * @brief  - Definition of the Endianess of the Micro
 *           1 = LITTLE_ENDIAN, 0 = BIG_ENDIAN
 */
#ifdef LITTLE_ENDIAN
    #define MICRO_IS_LITTLE_ENDIAN     1
#else
    #ifdef BIG_ENDIAN
        #define MICRO_IS_LITTLE_ENDIAN     0
    #endif
#endif

#define ENDIAN32_SWAP(value) \
         ((((uint32)((value) & 0x000000FFU)) << 24) | \
         (((uint32)((value) & 0x0000FF00U)) << 8) | \
         (((uint32)((value) & 0x00FF0000U)) >> 8) | \
         (((uint32)((value) & 0xFF000000U)) >> 24))


#define ENDIAN16_SWAP(value)   (uint16)(((value & (uint16)0x00FF) << 8) | ((value & (uint16)0xFF00) >> 8))


#if (MICRO_IS_LITTLE_ENDIAN == 1)
    #define EXTRACTU32(ptr)                     ((((uint32)ptr[3]) << 24) + (((uint32)ptr[2]) << 16) + (((uint32)ptr[1]) << 8 ) + ((uint32)ptr[0]))
    #define ENDIANS16_LTOL(value)               (value)
    #define ENDIANS16_LTON(value)               (value)
    #define ENDIANS16_NTOL(value)               (value)
    #define ENDIANU16_LTOL(value)               (value)
    #define ENDIANU16_LTON(value)               (value)
    #define ENDIANU16_NTOL(value)               (value)
    #define ENDIANS32_LTOL(value)               (value)
    #define ENDIANS32_LTON(value)               (value)
    #define ENDIANU32_LTOL(value)               (value)
    #define ENDIANU32_LTON(value)               (value)

/*    #define ENDIANS64_ToL(value)             (value)
    #define ENDIANS64_ToN(value)               (value)
    #define ENDIANU64_ToL(value)               (value)
    #define ENDIANU64_ToN(value)               (value)*/
#else


    #define EXTRACTU32(ptr)                     ((((uint32)ptr[0]) << 24) + (((uint32)ptr[1]) << 16) + (((uint32)ptr[2]) << 8 ) + ((uint32)ptr[3]))

    #define ENDIANS16_BTOB(value)               (value)
    #define ENDIANS16_BTON(value)               (value)
    #define ENDIANS16_NTOB(value)               (value)
    #define ENDIANU16_BTOB(value)               (value)
    #define ENDIANU16_BTON(value)               (value)
    #define ENDIANU16_NTOB(value)               (value)
    #define ENDIANS32_BTOB(value)               (value)
    #define ENDIANS32_BTON(value)               (value)
    #define ENDIANU32_BTOB(value)               (value)
    #define ENDIANU32_BTON(value)               (value)
/*    #define ENDIANS64_TOB(value)               (value)
    #define ENDIANS64_TON(value)               (value)
    #define ENDIANU64_TOB(value)               (value)
    #define ENDIANU64_TON(value)               (value)*/
#endif
/*
    Map native TO actual
*/
#if (MICRO_IS_LITTLE_ENDIAN == 1)
    #define ENDIANS16_LTOB(value)               (sint16)ENDIAN16_SWAP((uint16)(value))
    #define ENDIANS16_BTON(value)               (sint16)ENDIAN16_SWAP((uint16)(value))
    #define ENDIANS16_NTOB(value)               (sint16)ENDIAN16_SWAP((uint16)(value))
    #define ENDIANU16_BTON(value)               ENDIAN16_SWAP((value))
    #define ENDIANU16_LTOB(value)               ENDIAN16_SWAP((value))
    #define ENDIANU16_NTOB(value)               ENDIAN16_SWAP((value))
    #define ENDIANS32_LTOB(value)               ENDIAN32_SWAP((value))
    #define ENDIANU32_LTOB(value)               ENDIAN32_SWAP((value))
#else
    #define ENDIANS16_BTOL(value)               ENDIAN16_SWAP((value))
    #define ENDIANS16_LTON(value)               ENDIAN16_SWAP((value))
    #define ENDIANS16_NTOL(value)               ENDIAN16_SWAP((value))
    #define ENDIANU16_BTOL(value)               ENDIAN16_SWAP((value))
    #define ENDIANU16_LTON(value)               ENDIAN16_SWAP((value))
    #define ENDIANU16_NTOL(value)               ENDIAN16_SWAP((value))
    #define ENDIANS32_BTOL(value)               ENDIAN32_SWAP((value))
    #define ENDIANU32_BTOL(value)               ENDIAN32_SWAP((value))
    #define ENDIANU32_LTON(value)               ENDIAN32_SWAP((value))
#endif

/**
 * Macros to swap bytes in the setting file data to the native ("STON") micro endianness
 * NOTE: Only little endian setting files are allowed
 */
#if (MICRO_IS_LITTLE_ENDIAN == 1)
    #define ENDIANS16_STON(value)               (sint16)(value)
    #define ENDIANU16_STON(value)               (uint16)(value)
#else
    #define ENDIANS16_STON(value)               (sint16)ENDIAN16_SWAP((uint16)(value))
    #define ENDIANU16_STON(value)               (uint16)ENDIAN16_SWAP((uint16)(value))
#endif


#define CATEGORY_UNDEFINED          0

#define CATEGORY_REFRIGERATION_SBS  1
#define CATEGORY_REFRIGERATION_BM   2
#define CATEGORY_REFRIGERATION_TM   3
#define CATEGORY_REFRIGERATION_FD   4

#define CATEGORY_DRYER_ELECTRIC     10
#define CATEGORY_DRYER_GAS          11
#define CATEGORY_WASHER_VA          15
#define CATEGORY_WASHER_HA          16

#define CATEGORY_OVEN               20
#define CATEGORY_MW_OVEN            25
#define CATEGORY_COOKTOP            30
#define CATEGORY_IND_COOKTOP        35



/**
 * This macro shall be used to report to the SW05 check list
 */
#define STR_B(x) #x
#define STR(x) STR_B(x)
#define SW05_CLASSA "CLASS A"
#define SW05_CLASSB "CLASS B"
#define SW05_WARNING(number, error_message, class) _Pragma(STR(message(__FILE__ " (" STR(__LINE__) "): SW05_Warning[" STR_B(number) "](" class "): " error_message)))

#endif //C_EXTENSIONS_H

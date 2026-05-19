/**
 * @file: rtwtypes.h
 *
 * @model: Cross-Model - Dish Category
 *
 * @brief RTW Types for code gen models
 *
 * @modelversion:  NA
 *
 * @coderversion:  9.3 (R2020a) 18-Nov-2019
 *
 * C/C++ source code generated on : Wed Jul 29 11:35:38 2020
 *
 * @copyright  Copyright 2018. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//-------------------

//--------------------
#ifndef RTWTYPES_H
#define RTWTYPES_H

#include "C_Extensions.h"

/* Logical type definitions */
#if (!defined(__cplusplus))
#  ifndef false
#   define false                       (FALSE)
#  endif

#  ifndef true
#   define true                        (TRUE)
#  endif
#endif

/*=======================================================================*
 * Target hardware information
 *   Device type: ARM Compatible->ARM Cortex
 *   Number of bits:     char:   8    short:   16    int:  32
 *                       long:  32
 *                       native word size:  32
 *   Byte ordering: LittleEndian
 *   Signed integer division rounds to: Zero
 *   Shift right on a signed integer as arithmetic shift: on
 *=======================================================================*/
#ifdef PORTABLE_WORDSIZES              /* PORTABLE_WORDSIZES defined */

/*=======================================================================*
 * Host information
 *   Number of bits:     char:   8    short:   16    int:  32
 *                       long:  32
 *                       long long:  64
 *                       native word size:  64
 *=======================================================================*/

/*=======================================================================*
 * Fixed width word size data types:                                     *
 *   int8_T, int16_T, int32_T     - signed 8, 16, or 32 bit integers     *
 *   uint8_T, uint16_T, uint32_T  - unsigned 8, 16, or 32 bit integers   *
 *=======================================================================*/
typedef signed char int8_T;
typedef unsigned char uint8_T;
typedef short int16_T;
typedef unsigned short uint16_T;
typedef int int32_T;
typedef unsigned int uint32_T;

/*===========================================================================*
 * Generic type definitions: boolean_T, char_T, byte_T, int_T, uint_T,       *
 *                           real_T, time_T, ulong_T.                        *
 *===========================================================================*/
typedef double real_T;
typedef double time_T;
typedef unsigned char boolean_T;
typedef int int_T;
typedef unsigned int uint_T;
typedef unsigned int ulong_T;
typedef char char_T;
typedef unsigned char uchar_T;
typedef char_T byte_T;

#else                                  /* PORTABLE_WORDSIZES not defined */

/*=======================================================================*
 * Fixed width word size data types:                                     *
 *   int8_T, int16_T, int32_T     - signed 8, 16, or 32 bit integers     *
 *   uint8_T, uint16_T, uint32_T  - unsigned 8, 16, or 32 bit integers   *
 *=======================================================================*/
typedef signed char int8_T;
typedef unsigned char uint8_T;
typedef short int16_T;
typedef unsigned short uint16_T;
typedef int int32_T;
typedef unsigned int uint32_T;

/*===========================================================================*
 * Generic type definitions: boolean_T, char_T, byte_T, int_T, uint_T,       *
 *                           real_T, time_T, ulong_T.                        *
 *===========================================================================*/
typedef double real_T;
typedef double time_T;
typedef unsigned char boolean_T;
typedef int int_T;
typedef unsigned int uint_T;
typedef unsigned long ulong_T;
typedef char char_T;
typedef unsigned char uchar_T;
typedef char_T byte_T;

#endif                                 /* PORTABLE_WORDSIZES */

/*=======================================================================*
 * Min and Max:                                                          *
 *   int8_T, int16_T, int32_T     - signed 8, 16, or 32 bit integers     *
 *   uint8_T, uint16_T, uint32_T  - unsigned 8, 16, or 32 bit integers   *
 *=======================================================================*/
#define MAX_int8_T                     ((int8_T)(127))
#define MIN_int8_T                     ((int8_T)(-128))
#define MAX_uint8_T                    ((uint8_T)(255U))
#define MAX_int16_T                    ((int16_T)(32767))
#define MIN_int16_T                    ((int16_T)(-32768))
#define MAX_uint16_T                   ((uint16_T)(65535U))
#define MAX_int32_T                    ((int32_T)(2147483647L))
#define MIN_int32_T                    ((int32_T)(-2147483647L-1L))
#define MAX_uint32_T                   ((uint32_T)(0xFFFFFFFFUL))
#define MAX_int64_T                    ((int64_T)(9223372036854775807LL))
#define MIN_int64_T                    ((int64_T)(-9223372036854775807LL-1LL))
#define MAX_uint64_T                   ((uint64_T)(0xFFFFFFFFFFFFFFFFULL))

/* Block D-Work pointer type */
typedef void * pointer_T;

/* Define Simulink Coder replacement data types. */
#include "C_Extensions.h"
/* User defined replacement datatype for int8_T uint8_T int16_T uint16_T int32_T uint32_T boolean_T  */

/* Import type limit identifier replacement definitions. */
#include "limits.h"
#endif                                 /* RTWTYPES_H */

/**
 * @file: rt_defines.h
 *
 * @model: Cross-Model - Dish Category
 *
 * @brief RT Definitions for code gen models
 *
 * @modelversion:  NA
 *
 * @coderversion:  9.3 (R2020a) 18-Nov-2019
 *
 * C/C++ source code generated on : Wed Jul 15 14:11:07 2020
 *
 * @copyright  Copyright 2018. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
//-------------------
#ifndef RTW_HEADER_rt_defines_h_
#define RTW_HEADER_rt_defines_h_

//--------------------

/*===========*
 * Constants *
 *===========*/
#define RT_PI                          3.14159265358979323846
#define RT_PIF                         3.1415927F
#define RT_LN_10                       2.30258509299404568402
#define RT_LN_10F                      2.3025851F
#define RT_LOG10E                      0.43429448190325182765
#define RT_LOG10EF                     0.43429449F
#define RT_E                           2.7182818284590452354
#define RT_EF                          2.7182817F

/*
 * UNUSED_PARAMETER(x)
 *   Used to specify that a function parameter (argument) is required but not
 *   accessed by the function body.
 */
#ifndef UNUSED_PARAMETER
# if defined(__LCC__)
#   define UNUSED_PARAMETER(x)                                   /* do nothing */
# else

/*
 * This is the semi-ANSI standard way of indicating that an
 * unused function parameter is required.
 */
#   define UNUSED_PARAMETER(x)         (void) (x)
# endif
#endif
#endif                                 /* RTW_HEADER_rt_defines_h_ */

/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef MATHCALC_H_
#define MATHCALC_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#define MATHCALC__ABS(value) (((value) < 0) ? (-(value)) : (value))
#define MATHCALC__SATURATE(lim_inf, value, lim_sup)                                                 \
((lim_sup) >= (lim_inf)) ?                                                                          \
 (((value) >= (lim_sup)) ? (lim_sup) : ( ((value) <= (lim_inf)) ? (lim_inf) : (value) )) :          \
 (((value) >= (lim_inf)) ? (lim_inf) : ( ((value) <= (lim_sup)) ? (lim_sup) : (value) ))

#define MATHCALC__SATURATE_DIRECT(lim_inf, value, lim_sup)                                          \
 (((value) >= (lim_sup)) ? (lim_sup) : ( ((value) <= (lim_inf)) ? (lim_inf) : (value) ))            \

#define MATHCALC__SIGN(value) (((value) < 0) ? (-1) : (1))
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void MathCalc__Initialize(void);
sint32 MathCalc__Sqrt(sint32 value);
#endif // MATHCALC_H_



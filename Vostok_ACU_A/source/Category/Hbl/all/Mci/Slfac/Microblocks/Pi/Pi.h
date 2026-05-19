 /**
 *  @file
 *  @brief      Public interface to Pi module.
 *
 *  $Revision: 1.2 $
 *
 *  Copyright 2011 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
#ifndef PI_H_
#define PI_H_

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- ANSI/ISO --

// -- Project --


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================
typedef struct
{
    sint16    Coefficient_A;      //!< PI coefficient A. Must be divided by PI_QMATH_DIVIDER.
    sint16    Coefficient_B;      //!< PI coefficient B. Must be divided by PI_QMATH_DIVIDER.
    sint16    Coefficient_C;      //!< PI coefficient C. Must be divided by PI_QMATH_DIVIDER.
    sint16    Reference;          //!< Feedback target value. PI adjust Output to change
                                        //!< feedback toward Reference.
    sint16    Error;              //!< Actual value of difference Reference - feedback
    sint16    Saturation_Error;   //!< Actual value of difference between output and max or min
    sint16    Output_Max;         //!< Maximum value of output
    sint16    Output_Min;         //!< Minimal value of output
    sint16    Output;             //!< Actual PI output
} PI_TYPE;

//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================

extern void Pi__Initialize(PI_TYPE *pi_state);
extern void Pi__Calculate(PI_TYPE *pi_state, sint16  feedback);

extern void Pi__SetCoefficientA(PI_TYPE *pi_state, sint16 value);
extern void Pi__SetCoefficientB(PI_TYPE *pi_state, sint16 value);
extern void Pi__SetCoefficientC(PI_TYPE *pi_state, sint16 value);
extern void Pi__SetOutput(PI_TYPE *pi_state, sint16 out);
extern void Pi__SetReference(PI_TYPE *pi_state, sint16  reference);
extern void Pi__SetSaturation(PI_TYPE *pi_state, sint16 out_min, sint16 out_max);

extern sint16 Pi__GetCoefficientA(PI_TYPE *pi_state);
extern sint16 Pi__GetCoefficientB(PI_TYPE *pi_state);
extern sint16 Pi__GetCoefficientC(PI_TYPE *pi_state);
extern sint16 Pi__GetOutput(PI_TYPE *pi_state);

#endif  // PI_H_

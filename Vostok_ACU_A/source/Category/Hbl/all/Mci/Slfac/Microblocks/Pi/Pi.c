/**
 *  @file
 *  @brief      Proportional-Integral Controller
 *
 *  @details    A PI-Lead controller is a proportional gain in parallel with an integrator;
 *              both in series with a lead controller. The proportional gain provides fast error response.
 *              The integrator drives the system to a 0 steady-state error.
 *              See Pe13-1_Odra_Mci.doc for theoretical basis and how to derive A, B, C coefficients.
 *
 *  @Module_Owner{Lucien Lee from Delta}
 *  $Revision: 1.2 $
 *
 *  Copyright 2011 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "C_Extensions.h"
#include "Pi.h"

// -- ANSI/ISO --
#include <string.h>

// -- Project --

//  --- Public Variables ----------------------------------------------------------------------------------------------

//  --- Private Properties --------------------------------------------------------------------------------------------

//! Number of bites of fractional part of PI coefficients
#define PI_QMATH                    10    
//! QMath divider. Used for conversion from QMath format.                   
#define PI_QMATH_DIVIDER            (sint16)(1U << PI_QMATH)
//! Half of QMath divider. Used for rounding
#define PI_HALF_OF_QMATH_DIVIDER    (sint16)(1U << (PI_QMATH - 1))

//=====================================================================================================================
//  --- Public Functions ----------------------------------------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * It Initializes the module Pi and its variables.
 */
extern void Pi__Initialize(PI_TYPE *pi_state)
{
    memset(pi_state, 0, sizeof(PI_TYPE));
}
//---------------------------------------------------------------------------------------------------------------------
/**
 * Calculates PI output based on feedback according to parameters.
 *
 * Call rate depends on desired response speed
 *
 * @param   feedback - actual feedback value, PI will change output to set feedback as close as possible to pi_state.Reference
 */
extern void Pi__Calculate(PI_TYPE *pi_state, sint16 feedback)
{
    sint32	term_1;
    sint32	term_2;
    sint32	term_3;
    sint32	out;

    term_1 = 0;
    term_2 = 0;
    term_3 = 0;
    out = 0;

    // term_2 bases on previous error
    term_2 = pi_state->Coefficient_B * pi_state->Error;

    // Calculate actual error
    pi_state->Error = pi_state->Reference - feedback;

    // term_1 bases on actual error
    term_1 = pi_state->Coefficient_A * pi_state->Error;


    // term_3 is based on previous saturation error
    term_3 = pi_state->Coefficient_C * pi_state->Saturation_Error;

    out = term_1 + term_2 + term_3;

    // Round value of out
    if( out >= 0 )
    {
        out += PI_HALF_OF_QMATH_DIVIDER;
    }
    else
    {
        out -= PI_HALF_OF_QMATH_DIVIDER;
    }

    // Calculate pre saturation output
    out = pi_state->Output + (out / PI_QMATH_DIVIDER);

    // Saturate output
    if( out > pi_state->Output_Max )
    {
        pi_state->Output = pi_state->Output_Max;

        // out is long so check is max of unsint16 was not reached
        if( out < 32767 )
        {
            pi_state->Saturation_Error = pi_state->Output_Max - out;
        }
        else
        {
            pi_state->Saturation_Error = pi_state->Output_Max - 32767;
        }
    }
    else if( out < pi_state->Output_Min )
    {
        pi_state->Output = pi_state->Output_Min;

        if( out > -32768 )
        {
            pi_state->Saturation_Error = pi_state->Output_Min - out;
        }
        else
        {
            pi_state->Saturation_Error = pi_state->Output_Min - (-32768);
        }
    }
    else
    {
        pi_state->Output = out;
        pi_state->Saturation_Error = 0;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Sets A PI coefficient.
 *
 * @param   value - value in PI_QMATH format
 */
extern void Pi__SetCoefficientA(PI_TYPE *pi_state, sint16 value)
{
    pi_state->Coefficient_A = value;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Sets B PI coefficient.
 *
 * @param   value - value in PI_QMATH format
 */
extern void Pi__SetCoefficientB(PI_TYPE *pi_state, sint16 value)
{
    pi_state->Coefficient_B = value;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Sets B PI coefficient.
 *
 * @param   value - value in PI_QMATH format
 */
extern void Pi__SetCoefficientC(PI_TYPE *pi_state, sint16 value)
{
    pi_state->Coefficient_C = value;
}
//---------------------------------------------------------------------------------------------------------------------
/**
 * Sets PI output value.
 *
 * @param   out - output value to force
 */
extern void Pi__SetOutput(PI_TYPE *pi_state, sint16 out)
{
    pi_state->Output = out;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Sets PI reference (target) value.
 *
 * @param   reference - target value of feedback
 */
extern void Pi__SetReference(PI_TYPE *pi_state, sint16 reference)
{
    pi_state->Reference = reference;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Sets min and max of saturation.
 *
 * @param   out_min - minimum value of output
 * @param   out_max - maximum value of output
 */
extern void Pi__SetSaturation(PI_TYPE *pi_state, sint16 out_min, sint16 out_max)
{
    pi_state->Output_Min = out_min;
    pi_state->Output_Max = out_max;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Gets A PI coefficients.
 *
 * @return  value of coefficient in PI_QMATH format
 */
extern sint16 Pi__GetCoefficientA(PI_TYPE *pi_state)
{
    return pi_state->Coefficient_A;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Gets B PI coefficients.
 *
 * @return  value of coefficient in PI_QMATH format
 */
extern sint16 Pi__GetCoefficientB(PI_TYPE *pi_state)
{
    return pi_state->Coefficient_B;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Gets C PI coefficients.
 *
 * @return  value of coefficient in PI_QMATH format
 */
extern sint16 Pi__GetCoefficientC(PI_TYPE *pi_state)
{
    return pi_state->Coefficient_C;
}
//---------------------------------------------------------------------------------------------------------------------
/**
 * Gets PI reference (target) value.
 *
 * @return  target value of feedback
 */
extern sint16 Pi__GetOutput(PI_TYPE *pi_state)
{
    return pi_state->Output;
}

/**
 *  @file
 *  @brief      Calculates IIR filters
 *
 * <dl><dt><b>Module Owner:</b></dt><dd>Nelson Ferragut</dd></dl>
 * $Revision: 1.4 $
 * $Author: Nelson Ferragut II (FERRANJ) $
 *
 * Copyright 2009-2011  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL
 */

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "C_Extensions.h"
#include "IIRFilter.h"

// -- ANSI/ISO --

// -- Project --


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -----------------------------------------------------------

#if !defined(ADDR_SIZE_IIRFILTER)
// Use the project default memory model if not overridden in the parameter file
#define ADDR_SIZE_IIRFILTER         /* nothing */
#endif


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Determine a filtered value based on an input value.
 *  @details    The floating-point equation for an IIR filter is:
 *              <pre>
 *                  V = (alpha * new_value) + ((1 - alpha) * V)
 *                  alpha is in the range (0..1]
 *              </pre>
 *              The equation can be rewritten as:
 *              <pre>
 *                  V = (alpha * new_value) + V - (alpha * V)
 *                  V = V + (alpha * (new_value - V))
 *              </pre>
 *
 *              This function assumes that alpha is a power of 2 (i.e. 1/2, 1/4, 1/8, etc).
 *              Therefore, alpha can be fully described by the power of 2 or the number of bits
 *              involved.  Refer to the alpha_bits parameter for details.
 *
 *              This function uses fixed-point calculations.  To do this without rounding errors,
 *              the stored value must have more precision than the value used in the application.
 *              The fixed-point equation looks like this:
 *              <pre>
 *                  V = Stored Value
 *                  V = V + (new_value - (V >> alpha_bits))
 *
 *                  va = Value used by the application
 *                  va = Round(V >> alpha_bits)
 *              </pre>
 *
 *  @param      iir_value = A value that stores the current filtered value with additional precision.
 *                      The additional precision required depends on the alpha_bits.
 *  @param      new_value = An input to the filter system with normal precision.
 *  @param      alpha_bits = Exponent of alpha.  alpha = 2 ^ (-alpha_bits).
 *
 *              Sum of new_value resolution and alpha_bits must not be higher than 16 bits
 *
 *  @return     Returns the updated filtered value after rounding.
 */
ADDR_SIZE_IIRFILTER uint16 IIRFilter(uint16 *iir_value, uint16 new_value, uint8 alpha_bits)
{
    *iir_value += new_value - (((*iir_value >> (alpha_bits - 1)) + 1) >> 1);

    return (((*iir_value >> (alpha_bits - 1)) + 1) >> 1);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Determine a filtered value based on an input value.
 *
 *  @param      iir_value = A value that stores the current filtered value with additional precision.
 *                      The additional precision required depends on the alpha_bits.
 *  @param      new_value = An input to the filter system with normal precision.
 *  @param      alpha_bits = Exponent of alpha.  alpha = 2 ^ (-alpha_bits).
 *
 *              Sum of new_value resolution and alpha_bits must not be higher than 32 bits
 *
 *  @return     Returns the updated filtered value after rounding.
 */
ADDR_SIZE_IIRFILTER uint16 IIRFilterLong(uint32 *iir_value, uint16 new_value, uint8 alpha_bits)
{
    *iir_value += new_value - (((*iir_value >> (alpha_bits - 1)) + 1) >> 1);
    // lint Info 712: Loss of precision (return) (uint32 to uint16)
    return (((*iir_value >> (alpha_bits - 1)) + 1) >> 1);
}

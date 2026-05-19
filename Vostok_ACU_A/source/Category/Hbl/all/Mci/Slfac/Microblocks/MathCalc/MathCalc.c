/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
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


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "MathCalc.h" 
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define IQ30         30
#define IQ15         15

#define   IQMATH__IQXtoIQY(x, qx, qy)     ( ((qx)>(qy)) ?         \
                                              ((x) >> ((qx)-(qy)))    \
                                            : ((signed long int)(x) << ((qy)-(qx))))

#define SQRT_ITERATION_NUMBER          5
#define SQRT_METHOD                    2
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module MathCalc and its variables
 *
 */
void MathCalc__Initialize(void)
{

}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Square Root Function
 *
 */
sint32 MathCalc__Sqrt(sint32 value)
{
#if (SQRT_METHOD == 1) // Original
    // ~ 4.00us in RX130 @ 32Mhz @ 4 iterations - ok
    // ~ 4.65us in RX130 @ 32Mhz @ 5 iterations - ok
    // ~ 5.40us in RX130 @ 32Mhz @ 6 iterations - ok
    // ~ 6.75us in RX130 @ 32Mhz @ 8 iterations - ok
    // ~ 8.10us in RX130 @ 32Mhz @ 10 iterations - ok

    uint8 i;
    sint32 ret_val;
    sint32 x1;

    ret_val = 0;
    //truncating
    value = IQMATH__IQXtoIQY(value, IQ30, IQ15);

    if(value > 0)
    {
        x1 = value;
        ret_val = ( x1 + 1 )>>1; /* +1 for rounding */
        for (i = SQRT_ITERATION_NUMBER; i > 0; i--)
        {
            ret_val = (ret_val + ((x1<<15) / ret_val))>>1;
        }
    }
#endif
#if (SQRT_METHOD == 2) //Modified
    // ~ 4.00us in RX130 @ 32Mhz @ 5 iterations - ok

    uint8 i;
    sint32 ret_val;
//    sint32 x1;


    ret_val = 0;
    //truncating
//    value = IQMATH__IQXtoIQY(value, IQ30, IQ15);

    if(value > 0)
    {
//        x1 = value;
        ret_val = ( value + 1 )>>1; /* +1 for rounding */
        for (i = SQRT_ITERATION_NUMBER; i > 0; i--)
        {
            ret_val = (ret_val + ((value<<15) / ret_val))>>1;
        }
    }
#endif
#if (SQRT_METHOD == 3)
    ret_val = 0;
    // ~23us in RX130 @ 32Mhz
    ret_val = (sint32)sqrt((float)value);
#endif
    return ret_val;
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

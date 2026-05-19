/*
 * MclBasicTypes.h
 *
 *  Created on: 12 apr 2018
 *      Author: lab.motori
 */

#ifndef MCLBASICTYPES_H_
#define MCLBASICTYPES_H_
#include "C_Extensions.h"
#include "MclConfig.h"

/*--------------------------------------------------*/
/* backward compatibility with fixed point notation */
#ifndef LONG_LUT
    #define LONG_LUT 1
#endif
#ifndef LUT_SIZE
    #define LUT_SIZE LONG_LUT
#endif
/*--------------------------------------------------*/

typedef struct
{
    sint32 A;                                       //!< Q17.15 - a component
    sint32 B;                                       //!< Q17.15 - b component
    sint32 C;                                       //!< Q17.15 - c component
} ABC_COOR_SYST_TYPE;                               //!< 3 coordinates static frame: a-b-c


typedef struct
{
    sint32 ZeroSequence;
    sint32 Alpha;                                   //!< Q17.15 - alpha component
    sint32 Beta;                                    //!< Q17.15 - beta  component
} ALPHA_BETA_COOR_SYST_TYPE;                        //!< 2 coordinates static frame: alpha-beta


typedef struct
{
    sint32 ZeroSequence;
    sint32 D;                                       //!< Q17.15 - d component
    sint32 Q;                                       //!< Q17.15 - q component
} DQ_COOR_SYST_TYPE;                                //!< 2 coordinates rotating frame: d-q


typedef struct
{
    sint32 Sin;                                     //!< Q1.15 - sin value of an angle
    sint32 Cos;                                     //!< Q1.15 - cos value of an angle
} SIN_COS_TYPE;                                     //!< type containing sine and cosine of an angle

typedef struct
{
    sint8  IntegGain;
    sint8  PropGain;
} SHIFT_CONTROLLER_STRUCTURE_TYPE;       // structure for: current, speed, voltage and ovt regulators


typedef struct
{
   sint16   KiMant;        //!< Q1.15 - Integral Gain mantissa value
   sint32   UpperLimit;    //!< Q2.30 - Integrator upper limit
   sint32   LowerLimit;    //!< Q2.30 - Integrator lower limit
   uint16   KiExp;         //!< Integral Gain exponent value (used to represent small numbers)
} INTEGRATOR_PARAMS_TYPE;   //!< Integrator parameters (gain and saturation limits)

#if (LUT_SIZE == LONG_LUT)
typedef struct
{
    signed long input;
    signed long output;
} MATHCALC_LUT_TYPE;

typedef struct
{
    MATHCALC_LUT_TYPE *ptr_LUT;
    sint32 *step_inv;
    sint32 *sizeof_lut;
}MATHCALC_LUT_EXT_TYPE;

#else
typedef struct
{
    signed short input;
    signed short output;
} MATHCALC_LUT_TYPE;

typedef struct
{
    MATHCALC_LUT_TYPE *ptr_LUT;
    sint16 *step_inv;
    sint16 *sizeof_lut;
}MATHCALC_LUT_EXT_TYPE;

#endif


typedef struct
{
    signed long input;
    signed long output1;
    signed long output2;
} MATHCALC_LUT2_TYPE;


typedef struct
{
    MATHCALC_LUT2_TYPE *ptr_LUT;
    sint32 *step_inv;
    sint32 *sizeof_lut;
}MATHCALC_LUT2_EXT_TYPE;

typedef struct
{
    sint32 Alpha;
    sint32 Beta;
    sint32 Mod;
    sint32 Sin;
    sint32 Cos;
} MATHCALC_MOD_SIN_COS_TYPE;

// Floating point types
typedef struct
{
    float32 A;                                       //!< a component
    float32 B;                                       //!< b component
    float32 C;                                       //!< c component
} ABC_COOR_SYST_F_TYPE;                            //!< 3 coordinates static frame: a-b-c


typedef struct
{
    float32 ZeroSequence;
    float32 Alpha;                                   //!< alpha component
    float32 Beta;                                    //!< beta  component
} ALPHA_BETA_COOR_SYST_F_TYPE;                     //!< 2 coordinates static frame: alpha-beta


typedef struct
{
    float32 ZeroSequence;
    float32 D;                                       //!< d component
    float32 Q;                                       //!< q component
} DQ_COOR_SYST_F_TYPE;                             //!< 2 coordinates rotating frame: d-q


typedef struct
{
    float32 Sin;                                     //!< sin value of an angle
    float32 Cos;                                     //!< cos value of an angle
} SIN_COS_F_TYPE;                                  //!< type containing sine and cosine of an angle


typedef struct
{
   float32 Ki;            //!< Integral Gain
   float32 UpperLimit;    //!< Integrator upper limit
   float32 LowerLimit;    //!< Integrator lower limit
} INTEGRATOR_PARAMS_F_TYPE;   //!< Integrator parameters (gain and saturation limits)

typedef struct
{
    float32 input;
    float32 output;
} MATHCALC_LUT_F_TYPE;

typedef struct
{
    MATHCALC_LUT_F_TYPE *ptr_LUT;
    float32 *step_inv;
    sint32 *sizeof_lut;
} MATHCALC_LUT_EXT_F_TYPE;

typedef struct
{
    float32 input;
    float32 output1;
    float32 output2;
} MATHCALC_LUT2_F_TYPE;


typedef struct
{
    MATHCALC_LUT2_F_TYPE *ptr_LUT;
    float32 *step_inv;
    float32 *sizeof_lut;
} MATHCALC_LUT2_EXT_F_TYPE;

typedef struct
{
    float32 Alpha;
    float32 Beta;
    float32 Mod;
    float32 Sin;
    float32 Cos;
} MATHCALC_MOD_SIN_COS_F_TYPE;


#ifndef COMPILE_4_SIMULINK
typedef sint32 addr_t;
#else
typedef sint64 addr_t;
#endif


#ifndef MC_FLOATING_POINT
    typedef sint16 mc_sint16;
    typedef sint32 mc_sint32;
    typedef sint64 mc_sint64;
    typedef uint16 mc_uint16;  // for parameters
#else
    typedef float mc_sint16;
    typedef float mc_sint32;
    typedef float mc_sint64;
    typedef float mc_uint16;  // for parameters
#endif

#if defined MC_FIXED_POINT && defined MC_FLOATING_POINT
    #error "Define only one type of Math Logic"
#endif



#endif /* MCLBASICTYPES_H_ */

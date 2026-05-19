/**@addtogroup SRICTachometerLLI
 *  @{
 *  @file      SRICTachometerLLI_Defs.h 
 *
 *  @brief     Common interface of SRICTachometerLLI module.
 *
 *  @defgroup   CLASS_B
 *
 *  @copyright  Copyright 2018 - Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 *  }@
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

#ifndef SRICTACHOMETERLLI_DEFS_H_
#define SRICTACHOMETERLLI_DEFS_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//! Default Timer Counts
#define TACHO_ZERO_RPM_TIMER_COUNTS              468750 // Timer counts equivalent to ~0 rpm.

#ifndef SRICTACHOMETERLLI_AVERAGE_SAMPLES
    #define SRICTACHOMETERLLI_AVERAGE_SAMPLES 5
#else
    #if  (SRICTACHOMETERLLI_AVERAGE_SAMPLES < 5)
        #error "SRICTACHOMETERLLI_AVERAGE_SAMPLES should be at least 5"
    #endif
#endif

PACKED typedef struct
{
    uint32 Filtered_Period;                                     // Average of "Raw_Period".
    uint32 Raw_Period;									        // Raw value after the clamp filter (in raw counts).
} SRICTACHOMETER_PUBLIC_TYPE;


#endif /* SRICTACHOMETERLLI_DEFS_H_ */


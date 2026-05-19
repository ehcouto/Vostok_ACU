/**
 *  @file
 *  @brief  Interface definition for the IirFilter module.
 *
 * $Revision: 1.5 $
 * $Author: Nelson Ferragut II (FERRANJ) $
 *
 * Copyright 2009-2011  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL
 */
#ifndef IIRFILTER_H_
#define IIRFILTER_H_

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "IirFilter_prm.h"

// -- ANSI/ISO --

// -- Project --


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#if !defined(ADDR_SIZE_IIRFILTER)
// Use the project default memory model if not overridden in the parameter file
#define ADDR_SIZE_IIRFILTER         /* nothing */
#endif

enum
{
    FILTER_SHIFT_FS_5KHZ_FC_600HZ = 1,  //1
    FILTER_SHIFT_FS_5KHZ_FC_233HZ,      //2
    FILTER_SHIFT_FS_5KHZ_FC_108HZ,      //3
    FILTER_SHIFT_FS_5KHZ_FC_52HZ,       //4
    FILTER_SHIFT_FS_5KHZ_FC_25HZ,       //5
    FILTER_SHIFT_FS_5KHZ_FC_13HZ,       //6
    FILTER_SHIFT_FS_5KHZ_FC_6HZ,        //7
    FILTER_SHIFT_FS_5KHZ_FC_3HZ         //8
};


typedef struct
{
    uint8 Value;    //!< FIR Filter taps (must be a power of 2)
    uint8 Log;      //!< FIR Filter taps expressed as log2
} FIR_TAPS_TYPE;    //!< FIR Filter Taps structure



/*
    FILTER ORDER LOW PASS FILTER
    ============================

    Equation:
        sum = sum - (sum >> shift) + input
        output = sum >> shift

|---------------------------------------------------------|
|           CUT OFF FREQUENCY TABLE FOR FILTER            |
|---------------------------------------------------------|
|SLOTS (ms) || 0.2000  | 0.25  | 1     |  5      |  25    |
|---------------------------------------------------------|
|K | fs(Hz) || 5000    | 4000  | 1000  |  200    |  40    |
|---------------------------------------------------------|
|1 | 0.1197 || 1915.2  | 478.8 | 119.7 |  23.94  |  4.788 |
|2 | 0.0466 || 745.6   | 186.4 | 46.6  |  9.32   |  1.864 |
|3 | 0.0217 || 347.2   | 86.8  | 21.7  |  4.34   |  0.868 |
|4 | 0.0104 || 166.4   | 41.6  | 10.4  |  2.08   |  0.416 |
|5 | 0.0051 || 81.6    | 20.4  | 5.1   |  1.02   |  0.204 |
|6 | 0.0026 || 41.6    | 10.4  | 2.6   |  0.52   |  0.104 |
|7 | 0.0012 || 19.2    | 4.8   | 1.2   |  0.24   |  0.048 |
|8 | 0.0007 || 11.2    | 2.8   | 0.7   |  0.14   |  0.028 |
|---------------------------------------------------------|

*/
// Filter
#define FILTERS__FILTER_CALC(new_input, sum, shift) (sum += (-sum>>shift) + new_input)
#define FILTERS__FILTER_GET_FILTERED_VAR(sum, shift) (sum>>shift)

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

extern ADDR_SIZE_IIRFILTER uint16 IIRFilter(uint16 *iir_value, uint16 new_value, uint8 alpha_bits);
extern ADDR_SIZE_IIRFILTER uint16 IIRFilterLong(uint32 *iir_value, uint16 new_value, uint8 alpha_bits);


#endif  // IIRFILTER_H_

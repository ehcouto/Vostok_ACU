/**
 *  @file
 *
 *  @brief      Public interface to the Filters module.
 *
 *  @details    Refer to the Filters.c source file for more detailed information.
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: Filter.h
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef FILTERS_H_
#define FILTERS_H_
#include "Filter_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


// -- Public Type Declarations --

#if (FILTER_FLOATING_POINT_CAPABLE == ENABLED)
    typedef float32 FILTER_DATA_TYPE;
#else
    typedef sint32 FILTER_DATA_TYPE;
#endif

//! List of filters that are supported by the Filters module.
typedef enum
{
    FILTER_IIR      = 1,
    FILTER_GLITCH   = 2
}FILTER_TYPE;

//! A structure that holds the variables required for the IIR filter functionality.
PACKED typedef struct IIR_FILTER_MEASURE_STRUCT
{
    //! The filtered value provided by the filter. This value is always available after initialization.
    FILTER_DATA_TYPE Filtered_Value;
#if (FILTER_FLOATING_POINT_CAPABLE == DISABLED)
    //! The accumulator value contains a fixed-point version of the filtered value.
    //! This fixed-point value uses higher precision based on the alpha coefficient used.
    FILTER_DATA_TYPE Accumulator;
#endif
    //! This value is initially the inverted alpha coefficient. After the Filter__Initialize()
    //! function is called, this value is converted to the number of bits to shift between the
    //! accumulator and the filtered value.
    uint16 Inverted_Alpha;
} IIR_FILTER_MEASURE_TYPE;

//! A structure that holds the variables required for the glitch filter functionality.
PACKED typedef struct GLITCH_FILTER_MEASURE_STRUCT
{
    FILTER_DATA_TYPE Filtered_Value;            //<! Value of previous measure (with already removed high slope values).
    BOOL_TYPE Was_Measure_Init;                 //<! Flag indicating if measure channel was initialized by first call of
                                                //<! GlitchRemoval__SetValue for specific channel.
    uint8 Remove_Threshold;                     //<! Amplitude absolute threshold remove from measure.
    uint8 Debounce_Cnt;                         //<! Debounce counter of following removed measures. Value to monitor if
                                                //<! measure signal glitch and it shall be removed or measure changed to
                                                //<! new value with high changing slope and then signal shall be set to
                                                //<! new correct value.
    uint8 Debounce_Threshold;                   //<! Threshold of debounce counter for specific measure.
                                                //<! If Debounce_Cnt value is higher or equals this value then
                                                //<! measure is not treat as glitch no more and it is not removed.
                                                //<! When this variable is set to 2 then up to 2 following measures (no 0,1)
                                                //<! with value of absolute difference to previous value will be removed.
                                                //<! Instead previous value (value before glitch remove threshold difference
                                                //<! detection) will be used.
} GLITCH_FILTER_MEASURE_TYPE;


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void Filter__Initialize(void* filter_struct, FILTER_DATA_TYPE initial_value, FILTER_TYPE filter_type);
void Filter__Update(void* filter_struct, FILTER_DATA_TYPE new_sample_value, FILTER_TYPE filter_type);


#endif // FILTERS_H_



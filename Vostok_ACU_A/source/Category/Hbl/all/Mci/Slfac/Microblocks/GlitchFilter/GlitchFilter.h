/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: GlitchFilter.h 1.2 2014/09/03 12:02:42BRT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2014-$Date: 2014/09/03 12:02:42BRT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef GLITCHFILTER_H_
#define GLITCHFILTER_H_
#include "C_Extensions.h"
#include "GlitchFilter_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

typedef struct
{
    GLITCHFILTER_DATA_TYPE Previous_Measure;    //<! Value of previous measure (with already removed high slope values).
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

} GLITCHFILTER_MEASURE_TYPE;


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void GlitchFilter__Initialize(GLITCHFILTER_MEASURE_TYPE * data_ptr,uint8 remove_threshold, uint8 debounce_threshold);
GLITCHFILTER_DATA_TYPE GlitchFilter__Process(GLITCHFILTER_MEASURE_TYPE * data_ptr,GLITCHFILTER_DATA_TYPE new_data);

#endif // GLITCHFILTER_H_



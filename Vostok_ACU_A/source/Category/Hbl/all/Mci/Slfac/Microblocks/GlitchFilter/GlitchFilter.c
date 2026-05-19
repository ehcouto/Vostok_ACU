/**
 *  @file       
 *
 *  @brief      Glitch Filter
 *
 *  @details    This module removes Glitches of signals.
 *  <pre>
 *      Basically if the input signal comes above or below the last measurement by more than a configurable threshold
 *      the output signal will still the same as the previews sample for a configurable number of iterations.
 *      If the signal stays out of the threshold for long enough them the output signal will be updated by the input signal.
 *  </pre>
 *
 *
 *  $Header: GlitchFilter.c 1.2 2014/09/03 12:02:41BRT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2014-$Date: 2014/09/03 12:02:41BRT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "GlitchFilter.h" 
#include "string.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief It Initializes the module GlitchFilter and its variables
 * @param data_ptr
 * @param remove_threshold
 * @param debounce_threshold
 */
void GlitchFilter__Initialize(GLITCHFILTER_MEASURE_TYPE * data_ptr,uint8 remove_threshold, uint8 debounce_threshold)
{
    memset(data_ptr, 0, sizeof(GLITCHFILTER_MEASURE_TYPE));
    data_ptr->Debounce_Threshold  = debounce_threshold;
    data_ptr->Remove_Threshold    = remove_threshold;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method processes one iteration of the Filter.
 * @param data_ptr pointer t the local struct held by the module calling this functions
 * @param new_data new data sample
 * @return filtered data which is also available in the data struct under the Previous_Measure field
 */
GLITCHFILTER_DATA_TYPE GlitchFilter__Process(GLITCHFILTER_MEASURE_TYPE * data_ptr,GLITCHFILTER_DATA_TYPE new_data)
{
    GLITCHFILTER_DATA_TYPE delta;
    if (data_ptr->Was_Measure_Init == TRUE)
    {
        // Calculate the ABS delta
        if (data_ptr->Previous_Measure > new_data)
        {
            delta = data_ptr->Previous_Measure - new_data;
        }
        else
        {
            delta = new_data - data_ptr->Previous_Measure;
        }


        if ((data_ptr->Debounce_Cnt < data_ptr->Debounce_Threshold)
            &&
            (delta >= (GLITCHFILTER_DATA_TYPE)data_ptr->Remove_Threshold))
        {
            new_data   =   data_ptr->Previous_Measure;
            data_ptr->Debounce_Cnt++;
        }
        else
        {
            data_ptr->Debounce_Cnt = 0;
        }
    }
    data_ptr->Previous_Measure = new_data;
    data_ptr->Was_Measure_Init = TRUE;

    return (data_ptr->Previous_Measure);
}



//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================



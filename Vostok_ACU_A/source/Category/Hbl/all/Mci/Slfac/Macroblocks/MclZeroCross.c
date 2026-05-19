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
 *  @copyright  Copyright 2019-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "MclZeroCross.h"
#include "TimerInterruptExt.h"
#include "C_Extensions.h"
#include "Mcl_prv.h"
#include "Mcl.h"
#include "Pi.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
static uint16 Semi_Period_Time;                 //!< Line voltage semi period time
static uint32 IIR_Prev_Semi_Period;             //!< Previous value for semi period iir filter
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module MciZeroCross and its variables
 *
 */
void MclZeroCross__Initialize(MCL_ZERO_CROSS_IO_TYPE *io, MCL_PARAMS_TYPE* params)
{
	*io->Zero_Cross_Event = FALSE;
	Semi_Period_Time = MCISLFAC_TIMER_VALUE_FOR_SEMI_PERIOD_60HZ;
	IIR_Prev_Semi_Period = MCISLFAC_TIMER_VALUE_FOR_SEMI_PERIOD_60HZ<<params->Semi_Period_Alpha_IIR; //SEMI_PERIOD_ALPHA_IIR
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Voltage Zero Cross event handler
 *
 */
void MclZeroCross__ZeroCrossHandler(MCL_ZERO_CROSS_IO_TYPE *io, TC_EVENT_TYPE event, uint16 ccr_value)
{

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 *
 */
void MclZeroCross__FastHandler(MCL_ZERO_CROSS_IO_TYPE *io)
{
    TimerInterruptExt__ClearPerIntF();      // Clear interrupt pending flag
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 *
 */
void MclZeroCross__SemiPeriodFilter(MCL_ZERO_CROSS_IO_TYPE *io, MCL_PARAMS_TYPE* params)
{
	Semi_Period_Time = IIRFilterLong(&IIR_Prev_Semi_Period, *io->Semi_Period_Time_Raw, params->Semi_Period_Alpha_IIR); //SEMI_PERIOD_ALPHA_IIR
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 *
 */
uint16 MclZeroCross__SemiPeriodPercent(uint16 percent)
{
	return((uint16)(((uint32)(percent * Semi_Period_Time) + 50) / 100));
}
//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

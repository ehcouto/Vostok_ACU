/**
 *  @file       SRTimerReference.c
 *
 *  @brief      Class B Timing reference module
 *
 *  @details    This module provides an 1ms timer Class B compliant
 *
 *  @defgroup   CLASS_B
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
#include "SRSystemConfig.h"
#include "SRTimerReference.h"
#include "SRData.h"
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    #include "SRTaskSequence.h"
#endif

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
static uint32 SRTimerReference_Counter;
static uint32 N_SRTimerReference_Counter;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module SRTimerReference and its variables
 *  @detail     SRTimerReference_Counter is set to 0 at every reset
 */
void SRTimerReference__Initialize(void)
{
	SRData__UpdateLong(&SRTimerReference_Counter, &N_SRTimerReference_Counter, 0);

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_INIT_PHASE, SRTASKSEQUENCE_INIT_SRTIMERREFERENCE);
    #endif
}

/**
 *  @brief      SRTimerReference 1ms Handler
 *  @detail     SRTimerReference_Counter integrity is performed, then incremented
 */
void SRTimerReference__Handler1ms(void)
{
	if(SRData__CheckLong(&SRTimerReference_Counter, &N_SRTimerReference_Counter) == SRDATA_OK)
	{
		SRData__UpdateLong(&SRTimerReference_Counter, &N_SRTimerReference_Counter, SRTimerReference_Counter+1);
	}
    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_TICK_HANDLER, SRTASKSEQUENCE_TICK_SRTIMERREFERENCE);
    #endif
}

/**
 *  @brief      Get actual Timer reference
 *  @return     current timer reference counter
 */
uint32 SRTimerReference__GetTimer(void)
{
	return(SRTimerReference_Counter);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

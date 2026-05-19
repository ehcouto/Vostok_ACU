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
 *  $Header: Timeout.c 1.5 2014/12/08 14:00:01EST Dasar Hanumanth (DASARHB) Exp  $
 *
 *  @copyright  Copyright 2014-$Date: 2014/12/08 14:00:01EST $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "Timeout.h" 
#include "uc.h"
#include "Micro.h"
#include "String.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method initializes the Timer module
 *    @details  Configures the timer to increment count on every 1ms resolution
 */
void Timeout__Initialize(void)
{}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       It sets the time for the specified label
 *
 * @param TIMER_LABEL   - Labels defined in the TIMEOUT_LABEL enum
 * @param time_ms       - Time out time in milli second
 */
void Timeout__SetTime(TIMEOUT_LABEL tim, uint16 time_ms)
{}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       It reads the current count of the free running timer
 *
 * @return current count of the free running timer
 */
uint16 Timeout__GetCount(void)
{}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       It reads the status of the specified label
 *
 * @param TIMER_LABEL   - Labels defined in the TIMEOUT_LABEL enum
 * @return BOOL_TYPE
 *      @retval TRUE(1) - Timer is expired//Idle Condition
 *      @retval FALSE(0)- Timer is running
 */
BOOL_TYPE Timeout__GetStatus(TIMEOUT_LABEL tim)
{}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================



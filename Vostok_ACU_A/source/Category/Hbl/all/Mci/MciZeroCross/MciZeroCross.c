/**
 *  @file
 *
 *  @brief      Zero cross interface for motor control
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $   Eduardo Henrique Couto (Henrie2)
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "MciZeroCross.h"
#include "MciZeroCross_prv.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
TC_EVENT_TYPE Last_Event;     // 1 - Rising / 0 - Falling / 2 - Unknown

#ifdef MCI_POOLING_ZC_HW_SIGNAL
static BOOL_TYPE Previous_Input;

#ifndef MCI_ZEROCROSS_PIN_CFG
	#define MCI_ZEROCROSS_PIN_CFG()
#endif
#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
#ifndef GET_TIMER_CNT
	#define GET_TIMER_CNT()     0
#endif


//---------------------------------------------------------------------------------------------------------------------
/**
*  @brief      Initialize module.
*  @details    This routine initializes main variables used in algorithm;
*
*
*  @param[in]
*  @param[out]   
*  @return		BOOL_TYPE
*/
void MciZeroCross__Initialize(void)
{
	Last_Event = TC_UNKNOWN_EVENT;

#ifdef MCI_POOLING_ZC_HW_SIGNAL
	Previous_Input = FALSE;
	MCI_ZEROCROSS_PIN_CFG();
#endif
}




//---------------------------------------------------------------------------------------------------------------------
/**
*  @brief       Get zero crossing input
*  @details
*
*
*  @param[in]
*  @param[out]
*  @return
*/
void MciZeroCross__GetEvent(uint8 zc_event)
{
    uint16 ccr_timer;

#ifdef MCI_POOLING_ZC_HW_SIGNAL
    BOOL_TYPE input;

    input = Gpio__PinRead(PORT_MCI_ZEROCROSS, PIN_MCI_ZEROCROSS);

    if(input != Previous_Input)
    {
        if(input == FALSE)
        {
        	Last_Event = TC_FALLING_EDGE_EVENT;
        }
        else
        {
        	Last_Event = TC_RISING_EDGE_EVENT;
        }

        ccr_timer = GET_TIMER_CNT();
        MCI_ZEROCROSSING_EVENT(Last_Event,ccr_timer);
        Previous_Input = input;
    }
#else
    ZEROCROSS_EVENT_TYPE event;

    event = (ZEROCROSS_EVENT_TYPE)zc_event;

    if(event == ZEROCROSS_ANGLE_0_EVENT)
    {
        ccr_timer = GET_TIMER_CNT();
        MCI_ZEROCROSSING_EVENT(TC_RISING_EDGE_EVENT,ccr_timer);
        Last_Event = TC_RISING_EDGE_EVENT;
    }
    else if (event == ZEROCROSS_ANGLE_180_EVENT)
    {
        ccr_timer = GET_TIMER_CNT();
        MCI_ZEROCROSSING_EVENT(TC_FALLING_EDGE_EVENT,ccr_timer);
        Last_Event = TC_FALLING_EDGE_EVENT;
    }
#endif
}





//---------------------------------------------------------------------------------------------------------------------
/**
*  @brief       Get zero crossing Event
*  @details
*
*
*  @param[in]
*  @param[out]
*  @return
*/
uint8 MciZeroCross__ReadLastEvent(void)
{
    uint8 temp;

    temp = (uint8)Last_Event;
    Last_Event = TC_UNKNOWN_EVENT;
    return(temp);
}

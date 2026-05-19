/**
 * @file
 * @brief       Public interface to the FdmTimer module.
 *
 * @details     Refer to the FdmTimer.c source file for more detailed information.
 *
 * @copyright   Copyright 2016,2018 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef FDMTIMER_H_
#define FDMTIMER_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --
#include "FaultDebounceInterface.h"


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


#define FDM_TIMER_REGISTER \
{ \
    FdmTimer__Setup, \
    FdmTimer__ConditionDetected, \
    FdmTimer__ConditionRemoved, \
    FdmTimer__TimePassed \
}


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


FAULT_DEBOUNCE_EVENT_TYPE FdmTimer__Setup(FAULT_DEBOUNCE_TYPE* config, FAULT_DEBOUNCE_PROPERTIES_TYPE* data);
FAULT_DEBOUNCE_EVENT_TYPE FdmTimer__ConditionDetected(FAULT_DEBOUNCE_TYPE* config, FAULT_DEBOUNCE_PROPERTIES_TYPE* data);
FAULT_DEBOUNCE_EVENT_TYPE FdmTimer__ConditionRemoved(FAULT_DEBOUNCE_TYPE* config, FAULT_DEBOUNCE_PROPERTIES_TYPE* data);
FAULT_DEBOUNCE_EVENT_TYPE FdmTimer__TimePassed(FAULT_DEBOUNCE_TYPE* config, FAULT_DEBOUNCE_PROPERTIES_TYPE* data);
void FdmTimer__DebounceDataSize(uint8 debounce_data_size);


#endif      /* FDMTIMER_H_ */

/**
 * @file
 * @brief       Public interface to the FdmCounter module.
 *
 * @details     Refer to the FdmCounter.c source file for more detailed information.
 *
 * @copyright   Copyright 2016 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef FDMCOUNTER_H_
#define FDMCOUNTER_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --
#include "FaultDebounceInterface.h"


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


#define FDM_COUNTER_REGISTER \
{ \
    FdmCounter__Setup, \
    FdmCounter__ConditionDetected, \
    FdmCounter__ConditionRemoved, \
    FdmCounter__TimePassed \
}


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


FAULT_DEBOUNCE_EVENT_TYPE FdmCounter__Setup(FAULT_DEBOUNCE_TYPE* config, FAULT_DEBOUNCE_PROPERTIES_TYPE* data);
FAULT_DEBOUNCE_EVENT_TYPE FdmCounter__ConditionDetected(FAULT_DEBOUNCE_TYPE* config, FAULT_DEBOUNCE_PROPERTIES_TYPE* data);
FAULT_DEBOUNCE_EVENT_TYPE FdmCounter__ConditionRemoved(FAULT_DEBOUNCE_TYPE* config, FAULT_DEBOUNCE_PROPERTIES_TYPE* data);
FAULT_DEBOUNCE_EVENT_TYPE FdmCounter__TimePassed(FAULT_DEBOUNCE_TYPE* config, FAULT_DEBOUNCE_PROPERTIES_TYPE* data);


#endif      /* FDMCOUNTER_H_ */

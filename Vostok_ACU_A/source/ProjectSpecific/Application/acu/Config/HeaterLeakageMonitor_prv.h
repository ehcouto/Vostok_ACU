/**
 * @file
 * @brief       Private parameters for the HeaterLeakageMonitor module.
 *
 * @details     Refer to the C source file for more detailed information.
 *
 * @copyright   Copyright 2016-2020 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef HEATERLEAKAGEMONITOR_PRV_H_
#define HEATERLEAKAGEMONITOR_PRV_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "SystemConfig.h"
#include "WashHeaterFaultMonitor.h"

// -- Other Modules --


//=====================================================================================================================
//  --- Private Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Private Constant Declarations --


//! Fault condition. It also covers the case of neutral relay stuck closed, line relay stuck closed and both relays stuck closed.
#define HEATER_LEAKAGE_FAULT_CONDITION 	((WashHeaterFaultMonitor__GetHeaterConsistency() == COMBINE_NIBBLES(HBL_CONSISTENCY_TABLE_OFF, HBL_CONSISTENCY_SHORT_CIRCUIT)) || \
										(WashHeaterFaultMonitor__GetHeaterConsistency() == COMBINE_NIBBLES(HBL_CONSISTENCY_TABLE_OFF, HBL_CONSISTENCY_CUSTOM_2)))


//=====================================================================================================================
//  --- Private Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


#endif // HEATERLEAKAGEMONITOR_PRV_H_

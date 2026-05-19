/**
 * @file
 * @brief       Private parameters for the HeaterNeutralRelayOpenMonitor module.
 *
 * @details     Refer to the C source file for more detailed information.
 *
 * @copyright   Copyright 2016-2020 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef HEATERNEUTRALRELAYOPENMONITOR_PRV_H_
#define HEATERNEUTRALRELAYOPENMONITOR_PRV_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "SystemConfig.h"
#include "WashHeaterFaultMonitor.h"

// -- Other Modules --


//=====================================================================================================================
//  --- Private Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Private Constant Declarations --


//! Fault condition. It also covers the case of:
//! - both relays stuck open;
//! - both relays stuck open and heater open;
//! - neutral relay stuck open and heater open;
#define HEATER_N_RELAY_FAULT_CONDITION 	((WashHeaterFaultMonitor__GetHeaterConsistency() == COMBINE_NIBBLES(HBL_CONSISTENCY_TABLE_ON, HBL_CONSISTENCY_OPEN_CIRCUIT)) || \
										 (WashHeaterFaultMonitor__GetHeaterConsistency() == COMBINE_NIBBLES(HBL_CONSISTENCY_TABLE_ON, HBL_CONSISTENCY_CUSTOM_3)))

//=====================================================================================================================
//  --- Private Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


#endif // HEATERNEUTRALRELAYOPENMONITOR_PRV_H_

/**
 * @file
 * @brief       Private parameters for the HeaterLineRelayOpenMonitor module.
 *
 * @details     Refer to the C source file for more detailed information.
 *
 * @copyright   Copyright 2016-2020 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef HEATERLINERELAYOPENMONITOR_PRM_H_
#define HEATERLINERELAYOPENMONITOR_PRM_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "SystemConfig.h"
#include "WashHeaterFaultMonitor.h"

// -- Other Modules --


//=====================================================================================================================
//  --- Private Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Private Constant Declarations --

#define HEATER_LINE_RELAY_FAULT_ID		FAULT_ID_RELAY_HEATER_L_STUCK_OPEN

//! Fault condition.
#define HEATER_L_RELAY_OPEN_FAULT_CONDITION (WashHeaterFaultMonitor__GetHeaterConsistency() == COMBINE_NIBBLES(HBL_CONSISTENCY_TABLE_ON_ALTERNATE, HBL_CONSISTENCY_OPEN_CIRCUIT))


//=====================================================================================================================
//  --- Private Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


#endif // HEATERLINERELAYOPENMONITOR_PRM_H_

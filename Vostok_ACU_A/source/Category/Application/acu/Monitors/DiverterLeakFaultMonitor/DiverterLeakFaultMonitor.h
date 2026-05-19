/**
 *  @file
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

#ifndef DIVERTERLEAKFAULTMONITOR_H_
#define DIVERTERLEAKFAULTMONITOR_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================

// -- Public Constant Declarations --


// -- Public Type Declarations --

// -- Extern Declarations --
extern BOOL_TYPE DLFM_Diverter_Leak_Happened;  /* Trigger for diverter leak range detected */
extern uint8 DLFM_Diverter_Leak_Range_Counter; /* Counter to solve the side effect that during transition High-Low the monitor catch the leak range*/

//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================

void DiverterLeakFaultMonitor__Initialize(void);
void DiverterLeakFaultMonitor__Execution(void);
void DiverterLeakFaultMonitor__ResetFaultObserver(void);

#endif      /* DIVERTERLEAKFAULTMONITOR_H */

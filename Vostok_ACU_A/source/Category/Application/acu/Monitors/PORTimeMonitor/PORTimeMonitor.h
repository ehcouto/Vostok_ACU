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

#ifndef PORTIMEMONITOR_H_
#define PORTIMEMONITOR_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================
extern volatile uint16 POR_LLI_Data; // Used for diagnostic

// -- Public Constant Declarations --


// -- Public Type Declarations --

//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================

void PORTimeMonitor__Initialize(void);
void PORTimeMonitor__Execution(void);

#endif      /* PORTIMEMONITOR_H_ */

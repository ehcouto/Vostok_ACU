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
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

#ifndef THERMISTOROPENFAULTMONITOR_H_
#define THERMISTOROPENFAULTMONITOR_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void ThermistorOpenFaultMonitor__Initialize(void);
void ThermistorOpenFaultMonitor__Execution(void);
void ThermistorOpenFaultMonitor__ExitFault(void);

#endif /* THERMISTOROPENFAULTMONITOR_H_ */

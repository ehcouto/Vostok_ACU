/**
 *  @file       
 *  @defgroup   CLASS_B
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRISRMONITOR_H_
#define SRISRMONITOR_H_

#include "C_Extensions.h"
#include "SRSystemConfig.h"
#include "SRisrMonitor_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef struct
{
    uint16 Isr_Min_Cntr;
    uint16 Isr_Max_Cntr;
    uint8 Isr_Attempt_Min_Cntr;
    uint8 Isr_Attempt_Max_Cntr;
}SRISR_STATISTICS_DATA_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SRIsrMonitor__Initialize(void);
void SRIsrMonitor__EnterIsr(void);
void SRIsrMonitor__ExitIsr(void);
void SRIsrMonitor__Handler(void);

SRISR_STATISTICS_DATA_TYPE * SRIsrMonitor__GetStatisticsData(void);

#endif // SRISRMONITOR_H_

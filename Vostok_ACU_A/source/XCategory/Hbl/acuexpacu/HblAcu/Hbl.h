/**
 *  @file       
 *
 *  @brief      Hardware Base Line - Abstracts the Hardware into GI and Loads
 *
 *  @defgroup CLASS_B
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef HBL_H_
#define HBL_H_

#include "C_Extensions.h"
#include "SRProductMgr_Defs.h"
#include "SRHbl_prm.h"
#include "HblCommon.h"
#include "Hbl_prm.h"
#include "limits.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void Hbl__Initialize(void);

BOOL_TYPE Hbl__IsZeroCrossPresent(void);
uint32 Hbl__GetZeroCrossCycleCount(void);

void Hbl__Handler1ms(void);
void Hbl__AsynchHandler(void);
void Hbl__AnalogHandler(void);
void Hbl__ExtIntHandler(uint8 channel);
void Hbl__FastHandler(void);
void Hbl__OCIntHandler(uint8 timer, uint8 channel);
void Hbl__ICIntHandler(uint8 timer, uint8 channel);




//-------------------------------------------------------------------------------------------------
// Loads  Handling
//-------------------------------------------------------------------------------------------------
const uint8 * 		Hbl__GetFlushBuffer(void);
void                Hbl__FlushRequestBuffer(void);
uint8               Hbl__GetNumLoads(void);

void                Hbl__KeepLoadsOff(BOOL_TYPE disable);
BOOL_TYPE           Hbl__IsKeepLoadsOffEnabled(void);

uint8 Hbl__GetLoadIndexByLoadID(HBL_LOAD_ENUM_TYPE load_id);
HBL_LOAD_ENUM_TYPE Hbl__GetLoadIDByIndex(uint8 load_index);

HBL_PILOT_ENUM_TYPE Hbl__GetPilotIDByIndex(uint8 load_index);

// Set Load Request
PASS_FAIL_TYPE Hbl__SetLoadRequestByIndex(uint8 load_inded, HBL_PRIORITY_TYPE priority, uint8 * data);
PASS_FAIL_TYPE Hbl__SetLoadRequestByLoadID(HBL_LOAD_ENUM_TYPE load_id, HBL_PRIORITY_TYPE priority, uint8 * data);

// Load requested / processed data buffer and its data size
uint8 Hbl__GetLoadDataSize(uint8 load_index);
void * Hbl__GetRequestedLoad(uint8 load_index);
void * Hbl__GetProcessedLoad(uint8 load_index);

// Load Status - if GetStatus() is not available, it returns Processed Load
void *  Hbl__GetLoadStatusByIndex(uint8 load_index);
void *  Hbl__GetLoadStatusByLoadID(HBL_LOAD_ENUM_TYPE load_id);

// Pilot Status - if GetStatus() is not available, it returns Processed Pilot
// for HBL_LOAD_DIRECT Loads it returns same result of GetLoadStatus methods
void *  Hbl__GetPilotStatusByLoadIndex(uint8 load_index);
void *  Hbl__GetPilotStatusByLoadID(HBL_LOAD_ENUM_TYPE load_id);

// Load Consistency - refer to HBL_CONSISTENCY_TYPE
uint8 Hbl__GetLoadConsistency(uint8 load_index);

//
uint8 Hbl__GetLLIIndexByOwnLoadConsistencyType(HBL_LOAD_TYPE *load_index, HBL_LLI_ENUM_TYPE lli_id);
uint8 Hbl__GetOwnLoadConsistency(HBL_LOAD_TYPE *load, uint8 load_value);

//-------------------------------------------------------------------------------------------------
// Inputs Handling
//-------------------------------------------------------------------------------------------------

// LLI
uint8 Hbl__GetNumLLI(void);
uint8 HBl__GetLLIIndexByID(HBL_LLI_ENUM_TYPE lli_type, uint8 position);
uint8 Hbl__GetLLIDataSizeByIndex(uint8 index);
void * Hbl__GetLLIDataByIndex(uint8 index);
HBL_LLI_ENUM_TYPE Hbl__GetLLITypeByIndex(uint8 index);
uint8 Hbl__GetLLIPositionByIndex(uint8 index);
HBL_LLI_ENUM_TYPE Hbl__GetLLITypeByGIIndex(uint8 index);
uint8 Hbl__GetLLIPositionByGIIndex(uint8 index);
uint8 Hbl__GetLLIIndexByGIIndex(uint8 index);
uint8 *Hbl__GetLLIPinTabPtr(uint8 index);
uint8 Hbl__GetLLISequenceIDByIndex(uint8 index);

// GI
uint8 Hbl__GetNumGI(void);
uint8 Hbl__GetGIIndexByGIID(HBL_GENERIC_INPUT_ENUM_TYPE input);
HBL_GENERIC_INPUT_ENUM_TYPE Hbl__GetGIIDByIndex(uint8 index);
uint8 Hbl__GetGIDataSizeByIndex(uint8 index);
void * Hbl__GetGIDataByIndex(uint8 index);
void * Hbl__GetGIDataByID(HBL_GENERIC_INPUT_ENUM_TYPE input);
sint32 Hbl__GetGIDataConvertedByIndex(uint8 index);
uint8 Hbl__GetGISequenceIDByIndex(uint8 index);

//-------------------------------------------------------------------------------------------------
// SRProductMgr interface
//-------------------------------------------------------------------------------------------------

PASS_FAIL_TYPE	Hbl__GetSRCheckStatus(uint8 index, SRPRODUCTMGR_CHECK_STATUS_TYPE *status);
SRPRODUCTMGR_STATUS_TYPE	Hbl__GetSRFaultIDStatus(uint8 fault_id);
void Hbl__IssueSRCheckCmd(SRPRODUCTMGR_CHECK_CMD_TYPE cmd);
SRPRODUCTMGR_ACTION_TYPE	Hbl__GetSRActionStatus(uint8 index);
BOOL_TYPE    Hbl__IsSRActionRunning(uint8 index);
BOOL_TYPE    Hbl__IsSRActionRequested(uint8 index);
BOOL_TYPE    Hbl__IsSRActionCompleted(uint8 index);
PASS_FAIL_TYPE	Hbl__StartSRAction(uint8 index, uint8 priority);
PASS_FAIL_TYPE	Hbl__StopSRAction(uint8 index, uint8 priority);

#endif // HBL_H_



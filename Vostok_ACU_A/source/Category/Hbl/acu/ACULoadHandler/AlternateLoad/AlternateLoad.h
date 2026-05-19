/**
 *  @file
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>
 *
 *  $Header: 	AlternateLoad.h KAVEH RAZZAGHI (RAZZAK) Exp  $
 *
 *  @copyright  Copyright 2016.  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef ALTERNATELOAD_H_
#define ALTERNATELOAD_H_

#include "C_Extensions.h"
#include "SystemConfig.h"

#ifndef HBL_LOAD_NUM_ALTERNATE
    #define HBL_LOAD_NUM_ALTERNATE 0
#endif

#if (HBL_LOAD_NUM_ALTERNATE > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef enum MASTER_LOAD_ENUM
{
	LOAD_STATE_OFF = 0,
	LOAD_STATE_ON  = 1,

}MASTER_LOAD_TYPE;

typedef struct
{
	uint8	Alt_Request_Pos		: 4 ;
	uint8	Alt_Request_Sync	: 4 ;
} ALTERNATELOAD_REQUEST_TYPE;

typedef struct {
    uint8 								Alt_Engine_Pos_Curr;
    uint8 								Num_Alt_Positions;
    uint8 								Alt_Position_Not_Def;
    uint8								Pos_Reached;
} ALTERNATELOAD_STATUS_TYPE;

#define ALTERNATELOAD_REGISTER            	AlternateLoad__Initialize,            \
											AlternateLoad__Allocate,              \
											AlternateLoad__InitializeInstance,    \
											AlternateLoad__Request,     	      \
											AlternateLoad__Process,               \
											AlternateLoad__GetRequested,          \
											AlternateLoad__GetProcessed,          \
											AlternateLoad__AsyncProcess,          \
											AlternateLoad__GetStatus,        	  \
                                            1

extern volatile BOOL_TYPE Diverter_Motor_Status;
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void AlternateLoad__Initialize(void);
PASS_FAIL_TYPE AlternateLoad__InitializeInstance(HBL_LOAD_TYPE * load);
uint8 AlternateLoad__Allocate(void);
PASS_FAIL_TYPE AlternateLoad__Request(HBL_LOAD_TYPE * load,void * data);
PASS_FAIL_TYPE AlternateLoad__Process(HBL_LOAD_TYPE * load , HBL_PILOT_HANDLER_HOOK_TYPE * pilot);
void * AlternateLoad__GetRequested(HBL_LOAD_TYPE * load);
void * AlternateLoad__GetProcessed(HBL_LOAD_TYPE * load , HBL_PILOT_HANDLER_HOOK_TYPE * pilot);
PASS_FAIL_TYPE AlternateLoad__AsyncProcess(HBL_LOAD_TYPE * load , HBL_PILOT_HANDLER_HOOK_TYPE * pilot);
void * AlternateLoad__GetStatus(HBL_LOAD_TYPE * load , HBL_PILOT_HANDLER_HOOK_TYPE * pilot);

BOOL_TYPE AlternateLoad__GetAlternateLoadSearchHome(void);
void AlternateLoad__SetAlternateLoadSearchHome(BOOL_TYPE value);
BOOL_TYPE AlternateLoad__GetRunAfterFaultFlag(void);

uint8 AlternateLoad__GetRotationTime(void);
uint8 AlternateLoad__GetCamNumber(void);
BOOL_TYPE AlternateLoad__GetDiverterRotateAlwaysFlag(void);
#endif

#endif /* ALTERNATELOAD_H_ */

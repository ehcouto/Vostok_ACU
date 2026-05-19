/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: DoorSwitchGI.h 1.3 2015/07/24 11:35:19EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/24 11:35:19EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef DOORSWITCHGI_H_
#define DOORSWITCHGI_H_
#include "C_Extensions.h"
#include "SystemConfig.h"

#ifndef HBL_GI_NUM_DOORSWITCH
    #define HBL_GI_NUM_DOORSWITCH 0
#endif

#if (HBL_GI_NUM_DOORSWITCH > 0)
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#define DOORSWITCH_GI_REGISTER              DoorSwitchGI__Initialize,           \
                                            DoorSwitchGI__Allocate,             \
                                            DoorSwitchGI__InitializeInstance,   \
                                            DoorSwitchGI__Get,                  \
                                            DoorSwitchGI__AsynchProcess,        \
                                            DoorSwitchGI__GetConverted,         \
                                            1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void DoorSwitchGI__Initialize(void);
uint8 DoorSwitchGI__Allocate(void);
PASS_FAIL_TYPE DoorSwitchGI__InitializeInstance(HBL_GI_TYPE * generic_input, uint8 lli_sequence);
void * DoorSwitchGI__Get(HBL_GI_TYPE * generic_input,void * lli_data);
BOOL_TYPE DoorSwitchGI__AsynchProcess(HBL_GI_TYPE * generic_input,void * lli_data, uint8 lli_sequence);
sint32 DoorSwitchGI__GetConverted(HBL_GI_TYPE * generic_input,void * lli_data);

#endif

#endif // DOORSWITCHGI_H_



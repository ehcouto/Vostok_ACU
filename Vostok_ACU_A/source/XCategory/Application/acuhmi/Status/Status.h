/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: Status.h 1.7 2015/07/09 16:38:28EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/09 16:38:28EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef STATUS_H_
#define STATUS_H_
#include "C_Extensions.h"
#include "Reveal.h"
#include "SystemConfig.h"
#include "Status_prm.h"
#include "Callback.h"
#include "Variable.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void Status__Initialize(void);
void Status__Config(void);
BOOL_TYPE Status__IsValid(void);

uint8 Status__GetSize(void);
PASS_FAIL_TYPE Status__BuildPackage(REVEAL_COMMAND_TYPE * ptr, uint8 *variable_list_index_track, BOOL_TYPE *variables_pending, uint8 message_size);
void Status__BuildStatusInfoPackage(REVEAL_COMMAND_TYPE * ptr);
void Status__SetACUStatus(uint8 variable_list_index_track, uint8 * data, uint8 variable_message_size);
BOOL_TYPE Status__HasChanged(void);
void Status__RegisterChangeEventHandler(CALLBACK_HANDLER_TYPE event_handler);
void Status__UnregisterChangeEventHandler(CALLBACK_HANDLER_TYPE event_handler);
BOOL_TYPE Status__IsStatusVariable(VARIABLE_ID_TYPE var_id);
#endif // STATUS_H_



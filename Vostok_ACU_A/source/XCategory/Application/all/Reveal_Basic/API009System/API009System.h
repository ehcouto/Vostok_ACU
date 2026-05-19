/**
 *  @file       
 *
 *  @brief      This module manages System Reset and System info
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef API009SYSTEM_H_
#define API009SYSTEM_H_

#include "SystemConfig.h"

#ifndef API009SYSTEM_FEATURE
    #define API009SYSTEM_FEATURE      ENABLED
#endif

#if (API009SYSTEM_FEATURE == ENABLED)
#include "Reveal.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#define API009SYSTEM_NUM       0x9
#define API009SYSTEM_TYPE      5
#define API009SYSTEM_VERSION   1
#define API009SYSTEM_INSTANCES 1		

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void API009System__Initialize(void);
REVEAL_RECIPE_STATUS_TYPE API009System__CommandParser(REVEAL_MSG_TYPE * buffer);
void API009System__FeedbackParser(REVEAL_MSG_TYPE * buffer);
void API009System__AckParser(uint8 address,uint8 api,uint8 opcode, REVEAL_RECIPE_STATUS_TYPE reasoncode);

void API009System__Handler25ms(void);
BOOL_TYPE API009System__ForceReset(uint8 node,uint16 time_ms);
BOOL_TYPE API009System__RequestResetStatus(uint8 node);

BOOL_TYPE API009System__SendGoLowPower(uint8 node);
BOOL_TYPE API009System__IsSystemGoingToSleep(void);
BOOL_TYPE API009System__RequestWakeUpSource(void);



#endif  //API009SYSTEM_FEATURE
#endif // API009SYSTEM_H_



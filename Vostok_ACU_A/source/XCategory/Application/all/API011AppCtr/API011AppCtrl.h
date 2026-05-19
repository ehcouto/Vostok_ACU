/**
 *  @file       
 *
 *  @brief      API011 API
 *
 *
 *  @copyright  Copyright 2015 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef API011APPCTRL_H_
#define API011APPCTRL_H_

#include "C_Extensions.h"
#include "Reveal.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


#define API011APPCTRL_NUM   		0xB
#define API011APPCTRL_TYPE  		0x6
#define API011APPCTRL_VERSION  	    0x0
#define API011APPCTRL_INSTANCES     0x1
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void API011AppCtrl__Initialize(void);
void API011AppCtrl__Handler(void);
REVEAL_RECIPE_STATUS_TYPE API011AppCtrl__CommandParser(REVEAL_MSG_TYPE * buffer);
void API011AppCtrl__FeedbackParser(REVEAL_MSG_TYPE * buffer);

BOOL_TYPE API011AppCtrl__RequestStatus(uint8 address);
BOOL_TYPE API011AppCtrl__RequestRegulations(uint8 address);
BOOL_TYPE API011AppCtrl__SetRegulations(uint8 address);
BOOL_TYPE API011AppCtrl__PubMode(void);
BOOL_TYPE API011AppCtrl__PubStatus(void);
BOOL_TYPE API011AppCtrl__PubRegulations(uint8 node_id);

void API011AppCtrl__SetRegStatusPolling(ON_OFF_TYPE state);

#endif // API011APPCTRL_H_



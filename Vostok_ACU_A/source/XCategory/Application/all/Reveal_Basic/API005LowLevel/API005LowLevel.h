/**
 *  @file       
 *
 *  @brief      API 005 Low Level Reveal API.
 *
 *
 *  $Header: API005LowLevel.h 1.3 2015/05/01 18:45:42EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2013-$Date: 2015/05/01 18:45:42EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef API005LOWLEVEL_H_
#define API005LOWLEVEL_H_

#include "SystemConfig.h"

#ifndef API005LOWLEVEL_FEATURE
    #define API005LOWLEVEL_FEATURE      ENABLED
#endif

#if (API005LOWLEVEL_FEATURE == ENABLED)
#include "Reveal.h"


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#define API005LOWLEVEL_NUM        	    5
#define API005LOWLEVEL_TYPE  		    5
#define API005LOWLEVEL_VERSION  	    2
#define API005LOWLEVEL_INSTANCES        1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void API005LowLevel__Initialize(void);
REVEAL_RECIPE_STATUS_TYPE API005LowLevel__CommandParser(REVEAL_MSG_TYPE * buffer);
void API005LowLevel__PublishEvent(uint8 * data, uint8 size);
void API005LowLevel__Handler(void);

#endif  //API005LOWLEVEL_FEATURE

#endif // API005LOWLEVEL_H_



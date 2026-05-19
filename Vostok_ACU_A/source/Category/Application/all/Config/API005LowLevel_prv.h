/**
 *  @file       
 *
 *  @brief      Private configuration file for the API005LowLevel module.
 *
 *  @copyright  Copyright 2015-$Date: 2015/06/24 18:38:35EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef API005LOWLEVEL_PRV_H_
#define API005LOWLEVEL_PRV_H_

#include "C_Extensions.h"
#include "Debug.h"




/*
 *   Application specific table of functions and labels.
 *   Each element in the table is of type API005LOWLEVEL_DEBUG_TABLE_ELEMENT_TYPE.
 */
#define API005LOWLEVEL_DEBUG_TABLE_APP_SPECIFIC \
    /* Function             Label  */ \
    { Debug__Func00         ,"FVT LEDS ON1"   },\
    { Debug__Func01         ,"FVT LEDS ON2"   },\
    { Debug__Func02         ,"FVT LEDS ON"           },\
    { Debug__Func03         ,"FVT LEDS OFF"           },\
    { Debug__Func04         ,"FVT PLAY 3KHZ"           },\
    { Debug__Func05         ,"FVT PLAY 4KHZ"           },\
    { Debug__Func06         ,"FVT PLAY 5KHZ" },\
    { Debug__Func07         ,"FVT BUZZER OFF" }




#define API005LOWLEVEL_SET_DEBUG_VAR(index,var32)

//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================

#endif // API005LOWLEVEL_PRV_H_

/**
 *  @file       
 *
 *  @brief      Public interface file for the StatusLed module.
 *

    @startuml{StatusLed_System_Diagram.jpg}
        title StatusLed System Diagram

                hide empty attributes
        hide empty methods
        hide circle
        skinparam classAttributeIconSize 0

        class StatusLed <<module>> {
            +StatusLed__Initialize(void) : void
            +StatusLed__Execute(void) : void
        }

        SRMain_prv_h ..> StatusLed
        StatusLed ..> Gpio
        StatusLed ..> Log
        StatusLed ..> Mode
        StatusLed ..> SettingFile
        StatusLed ..> VirtualPin
    @enduml

 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef STATUSLED_H_
#define STATUSLED_H_

#include "SystemConfig.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#ifndef STATUS_LED_FEATURE
    #define STATUS_LED_FEATURE      DISABLED
#endif

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void StatusLed__Initialize(void);
void StatusLed__Execute(void);
void StatusLed__Suspend(void);
void StatusLed__Resume(void);
void StatusLed__GoToOffProfile(BOOL_TYPE go_off);

#endif // STATUSLED_H_

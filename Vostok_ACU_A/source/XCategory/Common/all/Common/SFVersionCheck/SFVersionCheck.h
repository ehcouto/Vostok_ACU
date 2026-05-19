/**
 *  @file       
 *
 *  @brief      Public interface file for the SFVersionCheck module.
 *

    @startuml{SFVersionCheck System Diagram.jpg}
        title SFVersionCheck System Diagram

        hide empty attributes
        hide empty methods
        hide circle
        skinparam classAttributeIconSize 0

        class SRMain_prv <<module>> {
            -SRMainNormalInitialization_Tasks
        }

        class SFVersionCheck <<module>> {
            +SFVersionCheck__Verify(void) : void
        }

        class Mode <<module>> {
            +Mode__SetModeExtended(MODE_NORMAL, SUBMODE_NORMAL_ERROR) : void
        }

        class SettingFile <<module>> {
        }

        SRMain_prv ..> SFVersionCheck
        SFVersionCheck ..> API019_PROJECT_DATA_TYPE
        SFVersionCheck ..> Mode
        SFVersionCheck ..> SettingFile

    @enduml

 *
 *  @copyright  Copyright 2017 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SFVERSIONCHECK_H_
#define SFVERSIONCHECK_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void SFVersionCheck__Verify(void);

#endif // SFVERSIONCHECK_H_



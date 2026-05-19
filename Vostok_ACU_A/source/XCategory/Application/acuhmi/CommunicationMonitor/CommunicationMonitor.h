/**
 *  @file
 *
 *  @brief      PUblic interface file for the ACUCommFault module.
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>
 *

    @startuml{ACUCommFault System Diagram.jpg}
        title ACUCommFault System Diagram

        hide empty attributes
        hide empty methods
        hide circle
        skinparam classAttributeIconSize 0

        package Monitors <<Folder>> {
            class ACUCommFault <<module>> {
                +ACUCommFault__Initialize(void) : void
                +ACUCommFault__Execute(void) : void
            }
        }

        package "Unknown " <<Folder>>{
        	class UNKNOWN <<enumeration>> {
        		+UNKNOWN_ENUM
        		}

        	class Unknown <<module>> {
        		+Unknown__Unknown(Unknown) : unknown
        		}

        }

        package "Fault " <<Folder>> {
            class FAULT_ID_TYPE <<enumeration>> {
                +FAULT_ID_ACU_COMMUNICATION
            }

            class Fault <<module>> {
                +Fault__ConditionDetected(FAULT_ID_TYPE fault_id) : void
                +Fault__ConditionRemoved(FAULT_ID_TYPE fault_id) : void
            }
        }

        ApplianceMgr ..> ACUCommFault
        ACUCommFault ..> Unknown
        ACUCommFault ..> Fault
        ACUCommFault --> FAULT_ID_TYPE


    @enduml

 *  $Header: $
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef COMMUNICATIONMONITOR_H
	#define COMMUNICATIONMONITOR_H

#include "SystemConfig.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void CommunicationMonitor__Initialize(void);
void CommunicationMonitor__Execute(void);

BOOL_TYPE CommunicationMonitor__IsACUNodeInFault(void);
BOOL_TYPE CommunicationMonitor__HasNodeInFault(void);

void CommunicationMonitor__Enable(uint8 node);
void CommunicationMonitor__Disable(uint8 node);

#endif /* COMMUNICATIONMONITOR_H */

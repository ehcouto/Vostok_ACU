/**
 *  @file       
 *  @defgroup   CLASS_B
 *  @brief      Public interface file for the SRPowerMgr module.
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
	@startuml{SRPowerMgr_Class_Diagram.png}
		title SRPowerMgr Class Diagram

		hide empty attributes
		hide empty methods
		hide circle
		skinparam classAttributeIconSize 0

		package "SRPowerMgr "{

			class SRPowerMgr {
				+ SRPowerMgr__Initialize(void) : void
				+ SRPowerMgr__GoLowPowerMode(void) : BOOL_TYPE
				+ SRPowerMgr__GoNormalMode(void) : void
				+ SRPowerMgr__GetMode(void) : SRPOWERMGR_MODE
				+ SRPowerMgr__Handler(void) : void
				+ SRPowerMgr__IsThisNodeWakeUpSource(void) : BOOL_TYPE
				+ SRPowerMgr__ReceivedWakeUpNodeID(uint8 node) : void
			}

			enum SRPOWERMGR_MODE {
				SRPOWERMGR_NORMAL_SPEED_MODE = 0,
				SRPOWERMGR_DEEP_SLEEP_MODE,
				SRPOWERMGR_AUTO_WAKEUP_MODE,
				SRPOWERMGR_LOW_SPEED_MODE
			}

			class SRPOWERMGR_WAKEUP_TYPE {
				WakeUp_Node	: uint8
				WakeUp_Port : GPIO_PORT_TYPE
				WakeUp_Pin : uint8
				WakeUp_Value : BOOL_TYPE
				WakeUp_Func : ACTIVATION_FUNC
			}

			SRPowerMgr *-- SRPOWERMGR_MODE
			SRPowerMgr *-- SRPOWERMGR_WAKEUP_TYPE
		}

	@enduml
*/
#ifndef SRPOWERMGR_H
#define SRPOWERMGR_H

#include "SRSystemConfig.h"
#include "C_Extensions.h"

#if (LOW_POWER_FEATURE == ENABLED)

#include "SRPowerMgr_prm.h"
#include "Gpio.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef enum
{
    SRPOWERMGR_NORMAL_SPEED_MODE = 0,
    SRPOWERMGR_DEEP_SLEEP_MODE,
    SRPOWERMGR_AUTO_WAKEUP_MODE,
    SRPOWERMGR_LOW_SPEED_MODE
}SRPOWERMGR_MODE;

typedef void (*const ACTIVATION_FUNC)(void);
#define SRPOWER_ACTIVATION_FUNC_NULL    (ACTIVATION_FUNC)NULL

typedef struct
{
    uint8 WakeUp_Node;
    GPIO_PORT_TYPE WakeUp_Port;
    uint8 WakeUp_Pin;
    BOOL_TYPE WakeUp_Value;
    ACTIVATION_FUNC WakeUp_Func;
}SRPOWERMGR_WAKEUP_TYPE;


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SRPowerMgr__Initialize(void);
BOOL_TYPE SRPowerMgr__GoLowPowerMode(void);
void SRPowerMgr__GoNormalMode(void);
SRPOWERMGR_MODE SRPowerMgr__GetMode(void);
void SRPowerMgr__Handler(void);
BOOL_TYPE SRPowerMgr__IsThisNodeWakeUpSource(void);
void SRPowerMgr__ReceivedWakeUpNodeID(uint8 node);

#endif  //LOW_POWER_FEATURE

#endif // SRPOWERMGR_H



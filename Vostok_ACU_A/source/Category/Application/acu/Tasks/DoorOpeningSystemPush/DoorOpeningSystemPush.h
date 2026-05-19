/**
 * @file		DoorOpeningSystemPush.h
 * @brief       Public interface to the Door Opening System Push task module.
 *
 */

#ifndef DOOROPENINGSYSTEMPUSH_H_
#define DOOROPENINGSYSTEMPUSH_H_

//  --- Include Files -------------------------------------------------------------------------------------------------
#include "C_Extensions.h"


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

ACTIVE_TYPE DoorOpeningSystemPush__Initialize(uint8 compartment, uint8 sf_index, uint8 displacement);
ACTIVE_TYPE DoorOpeningSystemPush__Task(uint8 compartment);
void DoorOpeningSystemPush__NVDataRestore(uint8 compartment);
void DoorOpeningSystemPush__Terminate(uint8 compartment);
void DoorOpeningSystemPush__StartPush(uint8 time);


#endif /* DOOROPENINGSYSTEMPUSH_H_ */

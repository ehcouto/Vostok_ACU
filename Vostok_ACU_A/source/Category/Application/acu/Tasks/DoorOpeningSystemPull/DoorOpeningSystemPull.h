/**
 * @file		DoorOpeningSystemPull.h
 * @brief       Public interface to the Door Opening System Pull task module.
 *
 */

#ifndef DOOROPENINGSYSTEMPULL_H_
#define DOOROPENINGSYSTEMPULL_H_

//  --- Include Files -------------------------------------------------------------------------------------------------
#include "C_Extensions.h"


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
ACTIVE_TYPE DoorOpeningSystemPull__Initialize(uint8 compartment, uint8 sf_index, uint8 displacement);
ACTIVE_TYPE DoorOpeningSystemPull__Task(uint8 compartment);
void DoorOpeningSystemPull__NVDataRestore(uint8 compartment);
void DoorOpeningSystemPull__Terminate(uint8 compartment);


#endif /* DOOROPENINGSYSTEMPULL_H_ */

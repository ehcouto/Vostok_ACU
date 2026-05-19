/**
 * @file
 * @brief       Public interface to the Calibration module.
 *
 * @details     Refer to the Calibration.c source file for more detailed information.
 *
 * @copyright   Copyright 2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef CALIBRATION_H_
#define CALIBRATION_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "Calibration_prm.h"

// -- Other Modules --
#include "Hbl.h"


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public Constant Declarations --


// -- Public Enumerated Constant Declarations --


// -- Public Type Declarations --


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


void Calibration__Initialize(void);
sint32 Calibration__ApplyByGI(HBL_GENERIC_INPUT_ENUM_TYPE gi, sint32 input);
sint32 Calibration__ApplyByItemID(CALIBRATION_ITEM_ID_TYPE id, sint32 input);
uint8 Calibration__GetDataSize(CALIBRATION_ITEM_ID_TYPE id);
void* Calibration__GetDataPointer(CALIBRATION_ITEM_ID_TYPE id);


#endif      // CALIBRATION_H_

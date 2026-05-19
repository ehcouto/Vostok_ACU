/**
 *  @file
 *
 *  @brief      Public interface for the AccelDiscover module.
 *
 *  @copyright  Copyright 2022. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef ACCELDISCOVER_H_
#define ACCELDISCOVER_H_

#if (HBL_LLI_NUM_ACCELEROMETER > 0)

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void AccelDiscover__Initialize(uint8 port, uint8 address);
void AccelDiscover__Execute(void);
uint8 AccelDiscover__GetId(void);
BOOL_TYPE AccelDiscover__IsComplete(void);

#endif // (HBL_LLI_NUM_ACCELEROMETER > 0)
#endif // ACCELDISCOVER_H_

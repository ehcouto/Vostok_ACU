/**
 *  @file       
 *
 *  @brief      Hbl public definitions for safety
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef HBL_PRM_H_
#define HBL_PRM_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

/*
 *  Allocates RAM for the GESE Settings file Loads.
 *  Must be greater than or equal to the number of Loads defined in the Settings file.
 */
#define HBL_MAX_NUM_LOADS                   24

/*
 *  Allocates RAM for the GESE Settings file Generic Inputs.
 *  Must be greater than or equal to the number of Generic Inputs defined in the Settings file.
 *  This definition must be public because it is used by API012.
 */
#define HBL_MAX_NUM_GENERICINPUT            20

/*
 *  Allocates RAM for Low Level Inputs that require Input Capture feature.
 *  Must be equal to or greater than the number of LLI with that feature that can be defined on the Setting file.
 */
#define HBL_MAX_NUM_INPUTCAPTURES_USED      1

/*
 *  Allocates RAM for Pilots that require Output Compare feature.
 *  Must be greater than or equal to the number of Pilots with that feature that can be defined on the Setting file.
 */
#define HBL_MAX_NUM_OUTPUTCOMPARES_USED     0


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif // HBL_PRM_H_

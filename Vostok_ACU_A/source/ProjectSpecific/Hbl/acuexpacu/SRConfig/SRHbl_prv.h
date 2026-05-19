/**
 *  @file
 *
 *  @brief      Private safety parameters for the Hbl module.
 *
 *  @defgroup   CLASS_B
 *
 *  @copyright  Copyright 2015-2016 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRHBL_PRV_H_
#define SRHBL_PRV_H_

#include "HblDefs.h"
#include "SRSystemConfig.h"

//=====================================================================================================================
//-------------------------------------- LOADS ------------------------------------------------------------------------
//=====================================================================================================================
/** Definition of Safety related Direct Loads (where the Load is the pilot without any special treatment)
 *
 *  Define this macros as a list of HBL_LOAD_ENUM_TYPE items separated by a comma except the last one;
 * it's recommended to assign the expected value to each item.
 */
//#define SRHBL_DIRECT_LOAD_LIST \
    HBL_LOAD_STATIC_HEATER, \
    HBL_LOAD_DUMMY_1


//    #include "DCFanLoad.h"
//

/** Definition of project specific Safety related Load handlers.
 *
 * Define this macros as a list of pairs { LOAD_REGISTER_MACRO, HBL_LOAD_ENUM_TYPE } separated by a comma except the last one.
 *
 */

//#define SRHBL_LOAD_HANDLERS_LIST \
      {DCFANLOAD_REGISTER, HBL_LOAD_FAN_RC}, \
      {DCFANLOAD_REGISTER, HBL_LOAD_FAN_FC}

//=====================================================================================================================
//-------------------------------------- PILOTS -----------------------------------------------------------------------
//=====================================================================================================================
#include "SRDigitalOutputPilot.h"
#include "SRChargePumpPilot.h"

/** Definition of project specific Safety related Pilot handlers.
 *
 * Define this macros as a list of pairs { PILOT_REGISTER_MACRO, HBL_PILOT_ENUM_TYPE } separated by a comma except the last one.
 *
 */
//#define SRHBL_PILOT_HANDLERS_LIST \
//    {SRDIGITALOUTPUTPILOT_REGISTER, HBL_PILOT_SR_DIGITAL}, \
//    {SRCHARGEPUMPPILOT_REGISTER,    HBL_PILOT_SR_CHARGEPUMP}


//=====================================================================================================================
//-------------------------------------- LOW LEVEL INPUTS -------------------------------------------------------------
//=====================================================================================================================
#include "SRAnalogLLI.h"
#include "SRDigitalLLI.h"

/** Definition of project specific Safety related LLI handlers.
 *
 * Define this macros as a list of pairs { LLI_REGISTER_MACRO, HBL_LLI_ENUM_TYPE } separated by a comma except the last one.
 *
 * IMPORTANT NOTE! P1 (ACU+HMI) shall configure here all entries that should be in the HBLHMI_LLI_HANDLERS list.
 *
 */
//#define SRHBL_LLI_HANDLERS_LIST \
//    {SRDIGITAL_LLI_REGISTER, HBL_LLI_SR_DIGITAL}, \
//    {SRANALOG_LLI_REGISTER,  HBL_LLI_SR_ANALOG}

//=====================================================================================================================
//-------------------------------------- GENERIC INPUTS -------------------------------------------------------------
//=====================================================================================================================
//#include "ProbeInput.h"
//#include "DoorSwitchGI.h"

/** Definition of project specific Safety related GI handlers.
 *
 * Define this macros as a list of pairs { GI_REGISTER_MACRO, HBL_GI_ENUM_TYPE } separated by a comma except the last one.
 *
 * IMPORTANT NOTE! P1 (ACU+HMI) shall configure here all entries that should be in the HBLHMI_LLI_HANDLERS list.
 *
 */
// #define SRHBL_GI_HANDLERS_LIST \
     {PROBE_GI_REGISTER,      HBL_GI_FRONT_TEMP_PROBE}, \
     {DOORSWITCH_GI_REGISTER, HBL_GI_DOOR_SWITCH}

#endif // SRHBL_PRV_H_

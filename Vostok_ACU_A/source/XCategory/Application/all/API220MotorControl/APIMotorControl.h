/**
 *  @file       APIMotorControl.h
 *
 *  @brief      Define the header of the API implementation of the Motor Control.
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: APIMotorControl.h 1.8 2016/02/16 17:49:25BRST Marcelo Silva (SILVAMC) Exp  $
 *
 *  @copyright  Copyright 2013-$Date: 2016/02/16 17:49:25BRST $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef APIMOTORCONTROL_H_
    #define APIMOTORCONTROL_H_

#include "API220_MotorControl.h"                    //!< Reveal API header of this API implementation
#include "Mci.h"
#include "Reveal.h"
#include "APIMotorControl_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void APIMotorControl__Initialize(void);
void APIMotorControl__SlowHandler(void);

REVEAL_RECIPE_STATUS_TYPE APIMotorControl__CommandParser(REVEAL_MSG_TYPE * buffer);
void APIMotorControl__FeedbackParser(REVEAL_MSG_TYPE * buffer);
void APIMotorControl__AckParser(unsigned char address,unsigned char api,unsigned char opcode, REVEAL_RECIPE_STATUS_TYPE reasoncode);
void APIMotorControl__PublishRun(MOTOR_ENUM_TYPE motor, sint32 target, sint32 ramp, MCI_CMD_RESPONSE_TYPE mci_feedback);
void APIMotorControl__PublishManualInjection(MOTOR_ENUM_TYPE motor, MCI_INJECTION_TYPE method, sint32 level_target, sint32 level_rate, sint32 parameter, sint32 param_rate, MCI_CMD_RESPONSE_TYPE mci_feedback);
void APIMotorControl__PublishStop(MOTOR_ENUM_TYPE motor, sint32 ramp, MCI_CMD_RESPONSE_TYPE mci_feedback);
void APIMotorControl__PublishSetDigital(MOTOR_ENUM_TYPE motor, MCI_DO_TYPE write_data, uint8 value, uint8 mci_feedback);
void APIMotorControl__PublishSetAnalog(MOTOR_ENUM_TYPE motor, MCI_AO_TYPE write_data, sint32 value, uint8 mci_feedback);
#if(API220_DEBUG_MODE == ENABLED)
void Mci__Emulate_Error(uint8 emulate_this_error);
#endif


#endif // APIMOTORCONTROL_H_



/**
 *  @file       APIMotorControl.c
 *
 *  @brief      This module provides a link between the Reveal and the Mci API.
 *
 *  @details    Parse all commands from Reveal, link with Mci API, build up the data accordingly and
 *              provides the related feedback to the Reveal protocol.
 *              There is no product action/reaction contained in this module.
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: APIMotorControl.c 1.15.1.2 2016/03/29 15:57:54BRT Marcelo Silva (SILVAMC) Exp  $
 *
 *  @copyright  Copyright 2013-$Date: 2016/03/29 15:57:54BRT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"                           //!< Standard file required by this code
#include "Mci.h"                                    //!< Motor Control API header file
#include "Reveal.h"                                 //!< Reveal stack API
#include "APIMotorControl.h"                        //!< Motor Control API implementation
#include "APIMotorControl_prv.h"
#include <string.h>                                 //!< Include the string intrinsic library

#ifndef API220_DEBUG_MODE
    #define API220_DEBUG_MODE   DISABLED
#endif

#ifndef API220_THERMALMODEL_DEBUG
    #define API220_THERMALMODEL_DEBUG   DISABLED
#endif

#ifndef MCI_FVT_TESTS
    #define MCI_FVT_TESTS   DISABLED
#endif


#if(API220_THERMALMODEL_DEBUG == ENABLED)
#include "ThermalModelCore.h"
#include "SRMotorSafetyMgr.h"                       //!< Include SR Motor safety manager
#endif




#if(MCI_FVT_TESTS == ENABLED)
#include "mclfvt.h"
#endif

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//! Sanity check on user's parameters.
#if(!defined API220_NR_OF_CONNECTED_MOTORS) || (API220_NR_OF_CONNECTED_MOTORS == 0)
    #error This parameters has to be defined in the prm file and shall be different of zero!
#endif


//! Structure for periodic analog data publishing.
typedef struct
{
    uint8 Publish_Requested;                        //!< Flags if the data is requested to be published
    uint16 Time_Threshold;                          //!< Time threshold; multiple of 25 ms. Note: this value shall be adjusted according to the amount of motors
    uint16 Time_Counter;                            //!< Instantaneous timer counter.
} API220_PUBLISH_ANALOG_DATA_PERIOD_TYPE;


//! Converts a value into TRUE or FALSE; 0 is FALSE, TRUE otherwise.
#ifndef GET_TRUE_FALSE
    #define GET_TRUE_FALSE(value)           (BOOL_TYPE)(((value) == 0) ? (FALSE) : (TRUE))
#endif


#if(API220_DEBUG_MODE == ENABLED)
#include "Mci_prv.h"                                //!< Include Mci private

    typedef enum
    {
        CMD_API_FORCE_ERROR 					= 0,                //!< 0 - Command to emulate/force errors in the Mci
        CMD_CLASSB_THERMAL_MODEL_DEBUG			= 1,				//!< 1 - OpCode to debug Class B thermal model
		CMD_CLASSA_THERMAL_MODEL_DEBUG			= 2,				//!< 2 - OpCode to debug Class A thermal model
		CMD_TUMBLING							= 15
    } COMMAND_LIST_API_DEBUG_MODE_TYPE;

#endif

#ifndef MANUAL_INJECTION_RUN
    #define MANUAL_INJECTION_RUN(motor_index, method, level_target, level_rate, parameter, param_rate)  Mci__Manual_Injection(motor_index, method, level_target, level_rate, parameter, param_rate)
#endif

#ifndef FVT_START
    #define FVT_START(motor_index)  Mci__FvtStart(motor_index)
#endif

#ifndef FVT_STOP
    #define FVT_STOP(motor_index)   Mci__FvtStop(motor_index)
#endif

#ifndef FVT_GET_MOTOR
    #define FVT_GET_MOTOR()         (MOTOR_ENUM_TYPE)0
#endif


#ifndef API220_MCI_STOP_CALLBACK
    #define API220_MCI_STOP_CALLBACK()
#endif

#ifndef API220_MCI_RUN_CALLBACK
    #define API220_MCI_RUN_CALLBACK()
#endif

#ifndef API220_MANUAL_INJECTION_CALLBACK
    #define API220_MANUAL_INJECTION_CALLBACK()
#endif

#ifndef API220_FVT_START_CALLBACK
    #define API220_FVT_START_CALLBACK()
#endif

#ifndef API220_FVT_STOP_CALLBACK
    #define API220_FVT_STOP_CALLBACK()
#endif

#ifndef MCLFVT_INJECTION_POSITION_STEPS_MAX
	#define MCLFVT_INJECTION_POSITION_STEPS_MAX    3
#endif

#ifndef MCLFVT_OC_POSITION_STEPS_MAX
	#define MCLFVT_OC_POSITION_STEPS_MAX           3
#endif

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//! Digital input log, these variable stores the most recent events.
static uint32 Mci_Digital_Inputs[API220_NR_OF_CONNECTED_MOTORS];

//! Error log, these variable stores the most recent error events.
static uint32 Mci_Digital_Error[API220_NR_OF_CONNECTED_MOTORS];

//! Define the structure to control the periodic data publishing.
static API220_PUBLISH_ANALOG_DATA_PERIOD_TYPE Api220_Per_Analog_Data[API220_NR_OF_CONNECTED_MOTORS][(1+MCI_AI_NR_OF_AI)];

//! Last speed set by user; used to generate Reveal events
static sint32 Last_Speed_Target[API220_NR_OF_CONNECTED_MOTORS];
//! Last ramp set by user; used to generate Reveal events
static sint32 Last_Ramp_Target[API220_NR_OF_CONNECTED_MOTORS];
//! Last manual injection method selected
static uint8 Last_Manual_Injection_Method[API220_NR_OF_CONNECTED_MOTORS];
//! Last manual injection level target
static sint32 Last_Manual_Injection_Level_Target[API220_NR_OF_CONNECTED_MOTORS];
//! Last manual injection parameter target
static sint32 Last_Manual_Injection_Parameter[API220_NR_OF_CONNECTED_MOTORS];

#if(API220_THERMALMODEL_DEBUG == ENABLED)

	extern ExtU_ThermalModelCore_T ThermalModelCore_U;
	extern ExtY_ThermalModelCore_T ThermalModelCore_Y;

#endif

//! Last digital output set or reset by user; used to generate Reveal events
static uint8 Last_Digital_Output[API220_NR_OF_CONNECTED_MOTORS];
//! Last analog output written by user; used to generate Reveal events
static sint32 Last_Analog_Output[API220_NR_OF_CONNECTED_MOTORS][MCI_AO_NR_OF_AO];

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
#ifndef API220_FVT_ONLY
static uint8 Run_Motor(REVEAL_MSG_TYPE *buffer);
static uint8 Stop_Motor(REVEAL_MSG_TYPE *buffer);
static uint8 Get_Digital(REVEAL_MSG_TYPE *buffer);
static uint8 Get_Error(REVEAL_MSG_TYPE *buffer);
static uint8 Set_Digital(REVEAL_MSG_TYPE *buffer);
static uint8 Get_Analog(REVEAL_MSG_TYPE *buffer);
static uint8 Publish_Analog_Input(MOTOR_ENUM_TYPE motor_index, MCI_AI_TYPE analog_data_index);
static uint8 Set_Analog(REVEAL_MSG_TYPE *buffer);
static void Periodically_Publish_Analog(void);
static uint8 Set_Data_Periodically_Publish(REVEAL_MSG_TYPE *buffer);
static uint8 Manual_Control(REVEAL_MSG_TYPE *buffer);
#endif
static void Update_Digital_Inputs(void);
static void Update_Error(void);
static T_BOOL Publish_Digital_Input(MOTOR_ENUM_TYPE motor_index, uint32 digital_input);
static T_BOOL Publish_Error(MOTOR_ENUM_TYPE motor_index, uint32 errors);
#if(API220_DEBUG_MODE == ENABLED)
static uint8 Command_Api220_Debug_Mode(REVEAL_MSG_TYPE *buffer);
#endif

#if(API220_THERMALMODEL_DEBUG == ENABLED)
    static uint8 Stack_Class_B_Debug_Data(MOTOR_ENUM_TYPE motor_index);
    static uint8 Stack_Class_A_Debug_Data(MOTOR_ENUM_TYPE motor_index);
#endif

#if (MCI_FVT_TESTS == ENABLED)

    static uint8 Api220_Fvt_Result_Published;
    static uint8 Api220_Fvt_Mode(REVEAL_MSG_TYPE *buffer);
    static uint8 Publish_Fvt_Result(void);

#endif

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module CompressorLink and its variables
 *
 */
void APIMotorControl__Initialize(void)
{
    memset(&Api220_Per_Analog_Data, 0, sizeof(Api220_Per_Analog_Data));
    memset(&Mci_Digital_Inputs, 0, sizeof(Mci_Digital_Inputs));
    memset(&Mci_Digital_Error, 0, sizeof(Mci_Digital_Error));

    memset(&Last_Speed_Target, 0, sizeof(Last_Speed_Target));
    memset(&Last_Ramp_Target, 0, sizeof(Last_Ramp_Target));
    memset(&Last_Digital_Output, 0, sizeof(Last_Digital_Output));
    memset(&Last_Analog_Output, 0, sizeof(Last_Analog_Output));

    memset(&Last_Manual_Injection_Method, 0, sizeof(Last_Manual_Injection_Method));
    memset(&Last_Manual_Injection_Level_Target, 0, sizeof(Last_Manual_Injection_Level_Target));
    memset(&Last_Manual_Injection_Parameter, 0, sizeof(Last_Manual_Injection_Parameter));

#if (MCI_FVT_TESTS == ENABLED)

    Api220_Fvt_Result_Published = FALSE;

#endif

}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Slow handler (rate: 25 ms)
 *  @details    Some implementation of this module might need to handle some slow timings, this is the place
 *              for this handling.
 */
void APIMotorControl__SlowHandler(void)
{
    Update_Digital_Inputs();
    Update_Error();
#ifndef API220_FVT_ONLY
    Periodically_Publish_Analog();
#endif
    // Check and publishes if FVT was performed and there is an event
    // to publish the fvt results.
#if (MCI_FVT_TESTS == ENABLED)

    if(!Api220_Fvt_Result_Published)
    {
        if(Mci__FvtGetStatus(MOTOR0, MCI_FVT_STATUS_FINISHED))
        {
            Api220_Fvt_Result_Published = Publish_Fvt_Result();
        }
    }

#endif

#if(API220_DEBUG_MODE == ENABLED)

//    Periodically_Publish_Debug_Data();

#endif
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Parses a command arrived from Reveal.
 *  @param      buffer - Pointer to the arrived buffer from Reveal.
 *  @return     return the status of the requested OpCode.
 */
REVEAL_RECIPE_STATUS_TYPE APIMotorControl__CommandParser(REVEAL_MSG_TYPE *buffer)
{
    REVEAL_RECIPE_STATUS_TYPE retval;

    retval = NVO_RS_REJECTED;

    switch (buffer->op_code)
    {
#ifndef API220_FVT_ONLY
        case API220_OPCODE_MOTOR_CONTROL_RUN:
            if(Run_Motor(buffer) == TRUE)
            {
                retval = NVO_RS_READY;
            }
            break;

        case API220_OPCODE_MOTOR_CONTROL_STOP:
            if(Stop_Motor(buffer) == TRUE)
            {
                retval = NVO_RS_READY;
            }
            break;


        case API220_OPCODE_MOTOR_CONTROL_GET_DIGITAL:
            if(Get_Digital(buffer) == TRUE)
            {
                retval = NVO_RS_READY;
            }
            break;

        case API220_OPCODE_MOTOR_CONTROL_GET_ERROR:
            if(Get_Error(buffer) == TRUE)
            {
                retval = NVO_RS_READY;
            }
            break;

        case API220_OPCODE_MOTOR_CONTROL_SET_DIGITAL:
            if(Set_Digital(buffer) == TRUE)
            {
                retval = NVO_RS_READY;
            }
            break;


        case API220_OPCODE_MOTOR_CONTROL_GET_ANALOG:
            if(Get_Analog(buffer) == TRUE)
            {
                retval = NVO_RS_READY;
            }
            break;


        case API220_OPCODE_MOTOR_CONTROL_SET_ANALOG:
            if(Set_Analog(buffer) == TRUE)
            {
                retval = NVO_RS_READY;
            }
            break;


        case API220_OPCODE_MOTOR_CONTROL_MANUAL_CONTROL:
            if(Manual_Control(buffer) == TRUE)
            {
                retval = NVO_RS_READY;
            }
            break;


        case API220_OPCODE_MOTOR_CONTROL_PUBLISH_PERIOD:
            if(Set_Data_Periodically_Publish(buffer) == TRUE)
            {
                retval = NVO_RS_READY;
            }
            break;
#endif

#if (MCI_FVT_TESTS == ENABLED)

        case API220_OPCODE_MOTOR_CONTROL_FVT:
            if(Api220_Fvt_Mode(buffer) == TRUE)
            {
                retval = NVO_RS_READY;
            }
            break;

#endif

#if(API220_DEBUG_MODE == ENABLED)

        case API220_OPCODE_MOTOR_CONTROL_DEBUG:
            if(Command_Api220_Debug_Mode(buffer) == TRUE)
            {
                retval = NVO_RS_READY;
            }
            break;

#endif

        default:
            retval = NVO_RS_UNSUP_OP_CODE;
            break;
    }

    return(retval);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Parses a feedback arrived from Reveal.
 *  @param      buffer - Pointer to the arrived buffer from Reveal.
 */
void APIMotorControl__FeedbackParser(REVEAL_MSG_TYPE *buffer)
{

}




//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Parses a acknowledge arrived from Reveal.
 *  @param      address - source address
 *  @param      api - received api
 *  @param      opcode - received opcode
 *  @param      reasoncode - recipe status
 */
void APIMotorControl__AckParser(uint8 address, uint8 api, uint8 opcode, REVEAL_RECIPE_STATUS_TYPE reasoncode)
{

}




//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Run the motor and publishes the request on Reveal protocol
 * @param       motor indexer to the desired motor
 * @param       target define the target speed
 * @param       ramp define the acceleration of the motor
 * @return      MCI_CMD_RESPONSE_TYPE ret_val: Returns whether Mci has accepted
 */
void APIMotorControl__PublishRun(MOTOR_ENUM_TYPE motor, sint32 target, sint32 ramp, MCI_CMD_RESPONSE_TYPE mci_feedback)
{
    REVEAL_COMMAND_TYPE *command;

    // Check if the new command is different of the previous one
    if( (Last_Speed_Target[motor] != target)   ||
        (Last_Ramp_Target[motor] != ramp)      )
    {
        // Either speed target or acceleration is different, so, publish the new request
        // Check and stack if available a new Reveal feedback
        command = Reveal__NewFeedback(API220_MOTOR_CONTROL_NUM, API220_OPCODE_MOTOR_CONTROL_RUN, REVEAL_BROADCAST_NODE, 6);
        if(command != NULL)
        {
            // Build and enqueue the Reveal buffer with the required feedback
            Reveal__AddDataU8(command, motor);
            Reveal__AddDataU16(command, target);
            Reveal__AddDataU16(command, ramp);
            Reveal__AddDataU8(command, (uint8)mci_feedback);

            Reveal__QueueCommand(command);
        }

        Last_Speed_Target[motor] = target;
        Last_Ramp_Target[motor] = ramp;
    }
}



//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Run the motor and publishes the request on Reveal protocol
 * @param       motor indexer to the desired motor
 * @param       target define the target speed
 * @param       ramp define the acceleration of the motor
 * @return      MCI_CMD_RESPONSE_TYPE ret_val: Returns whether Mci has accepted
 */
void APIMotorControl__PublishManualInjection(MOTOR_ENUM_TYPE motor, MCI_INJECTION_TYPE method, sint32 level_target, sint32 level_rate, sint32 parameter, sint32 param_rate, MCI_CMD_RESPONSE_TYPE mci_feedback)
{
    REVEAL_COMMAND_TYPE *command;

    // Check if the new command is different of the previous one
    if( (Last_Manual_Injection_Method[motor] != method)   ||
        (Last_Manual_Injection_Level_Target[motor] != level_target)   ||
        (Last_Manual_Injection_Parameter[motor] != parameter)      )
    {
        // Either speed target or acceleration is different, so, publish the new request
        // Check and stack if available a new Reveal feedback
        command = Reveal__NewFeedback(API220_MOTOR_CONTROL_NUM, API220_OPCODE_MOTOR_CONTROL_MANUAL_CONTROL, REVEAL_BROADCAST_NODE, 3);
        if(command != NULL)
        {
            // Build and enqueue the Reveal buffer with the required feedback
            Reveal__AddDataU8(command, motor);
            Reveal__AddDataU8(command, method);
            Reveal__AddDataU8(command, (uint8)mci_feedback);

            Reveal__QueueCommand(command);
        }

        Last_Manual_Injection_Method[motor] = method;
        Last_Manual_Injection_Level_Target[motor] = level_target;
        Last_Manual_Injection_Parameter[motor] = parameter;
    }
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Request to stop the motor regardless the free spot on the Reveal stack.
 *              E.g., the stop command will be performed even if the Reveal stack is full.
 * @param       motor indexer to the desired motor
 * @param       ramp define the deceleration of the motor
 * @return      MCI_CMD_RESPONSE_TYPE ret_val: Returns whether Mci has accepted
 */
void APIMotorControl__PublishStop(MOTOR_ENUM_TYPE motor, sint32 ramp, MCI_CMD_RESPONSE_TYPE mci_feedback)
{
    REVEAL_COMMAND_TYPE *command;

    // Check if the new command is different of the previous one
    if( (Last_Speed_Target[motor] != 0)   ||
        (Last_Ramp_Target[motor] != ramp)      )
    {
        // if so, publish the event to Reveal network
        command = Reveal__NewFeedback(API220_MOTOR_CONTROL_NUM, API220_OPCODE_MOTOR_CONTROL_STOP, REVEAL_BROADCAST_NODE, 4);
        if(command != NULL)
        {
            // Build and enqueue the Reveal buffer with the required feedback
            Reveal__AddDataU8(command, motor);
            Reveal__AddDataU16(command, (uint16)ramp);
            Reveal__AddDataU8(command, (uint8)mci_feedback);

            Reveal__QueueCommand(command);
        }

        Last_Speed_Target[motor] = 0;
        Last_Ramp_Target[motor] = ramp;
    }
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Set digital command - this command is used to set or reset a flag inside the Mci layer.
 *              Note: the set digital command shall be performed only in the case that a free spot was
 *              found in the Reveal queue, otherwise, the set command shall be denied. This action is required
 *              to proper synchronize integrated Mci requests with Reveal event generation.
 * @param       motor indexer to the desired motor
 * @param       write_data selects which flags is desired to be written
 * @param       value TRUE sets a flag; FALSE resets a flag
 * @return      TRUE parameters were accepted; FALSE parameter write is denied
 */
void APIMotorControl__PublishSetDigital(MOTOR_ENUM_TYPE motor, MCI_DO_TYPE write_data, uint8 value, uint8 mci_feedback)
{
    REVEAL_COMMAND_TYPE *command;
    BOOL_TYPE temp;

    if(write_data < MCI_DO_NR_OF_DO)
    {
        temp = GET_TRUE_FALSE(value);

        if(GET_TRUE_FALSE(BIT_TEST(Last_Digital_Output[motor], write_data)) != temp)
        {
            // Check if there is a free spot in the Reveal queue
            command = Reveal__NewFeedback(API220_MOTOR_CONTROL_NUM, API220_OPCODE_MOTOR_CONTROL_SET_DIGITAL, REVEAL_BROADCAST_NODE, 4);
            if(command != NULL)
            {
                // Build and enqueue the Reveal buffer with the required feedback
                Reveal__AddDataU8(command, motor);
                Reveal__AddDataU8(command, write_data);
                Reveal__AddDataU8(command, value);
                Reveal__AddDataU8(command, mci_feedback);

                Reveal__QueueCommand(command);

                if(!value)
                {
                    BIT_CLR(Last_Digital_Output[motor], write_data);
                }
                else
                {
                    BIT_SET(Last_Digital_Output[motor], write_data);
                }
            }
        }
    }
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Set analog command - this command is used to write an analog value inside the Mci layer.
 *              Note: the set analog command shall be performed only in the case that a free spot was
 *              found in the Reveal queue, otherwise, the set command shall be denied. This action is required
 *              to proper synchronize integrated Mci requests with Reveal event generation.
 * @param       motor indexer to the desired motor
 * @param       write_data selects which data is desired to be written
 * @param       value defines the value to be written to the variable
 * @return      TRUE command and parameters were accepted; FALSE either command or parameter write is denied
 */
void APIMotorControl__PublishSetAnalog(MOTOR_ENUM_TYPE motor, MCI_AO_TYPE write_data, sint32 value, uint8 mci_feedback)
{
    REVEAL_COMMAND_TYPE *command;

    if(write_data < MCI_AO_NR_OF_AO)
    {
        if(Last_Analog_Output[motor][write_data] != value)
        {
            // Check if there is a free spot in the Reveal queue
            command = Reveal__NewFeedback(API220_MOTOR_CONTROL_NUM, API220_OPCODE_MOTOR_CONTROL_SET_ANALOG , REVEAL_BROADCAST_NODE, 5);
            if(command != NULL)
            {
                // and build and enqueue the Reveal buffer with the required feedback
                Reveal__AddDataU8(command, motor);
                Reveal__AddDataU8(command, write_data);
                Reveal__AddDataS16(command, value);
                Reveal__AddDataU8(command, mci_feedback);

                Reveal__QueueCommand(command);

                Last_Analog_Output[motor][write_data] = value;
            }
        }
    }
}




//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================



#ifndef API220_FVT_ONLY
//---------------------------------------------------------------------------------------------------------------------
/*
 * @brief       Set a new target speed and invoke the Mci run command.
 * @param       buffer - a pointer to the received Reveal buffer.
 * @return      Returns whether Mci has accepted or not the command; TRUE = accepted; FALSE = denied
 */
uint8 Run_Motor(REVEAL_MSG_TYPE *buffer)
{
#ifdef API220_ENABLE_MCI_CALLS

    uint8 ret_val;
    MOTOR_ENUM_TYPE motor_index;
    sint16 target_speed;
    sint16 target_accel;
    #ifndef ENABLE_API220_CALLBACKS
        MCI_CMD_RESPONSE_TYPE feedback;
        REVEAL_COMMAND_TYPE *command;
    #endif

    ret_val = FALSE;                                // Default answer is FALSE

    if(buffer->sdu_length == 7)                     // This command requests receiving this amount of bytes, include API and Opcode bytes.
    {
        // custom code
        API220_MCI_RUN_CALLBACK();

        // Build up the data from the Reveal buffer
        motor_index = (MOTOR_ENUM_TYPE)buffer->payload[0];
        target_speed = ((sint16)(buffer->payload[1])<<8) + buffer->payload[2];
        target_accel = ((sint16)(buffer->payload[3])<<8) + buffer->payload[4];

        #ifdef ENABLE_API220_CALLBACKS

            // Invoke Mci
            Mci__Run(motor_index, target_speed, target_accel);

            ret_val = TRUE;

        #else

            // Invoke Mci run and read its feedback
            feedback = Mci__Run(motor_index, target_speed, target_accel);

            command = Reveal__NewFeedback(API220_MOTOR_CONTROL_NUM, API220_OPCODE_MOTOR_CONTROL_RUN, REVEAL_BROADCAST_NODE, 6);
            if(command != NULL)
            {
                // Build and enqueue the Reveal buffer with the required feedback
                Reveal__AddDataU8(command, motor_index);
                Reveal__AddDataU16(command, target_speed);
                Reveal__AddDataU16(command, target_accel);
                Reveal__AddDataU8(command, (uint8)feedback);

                ret_val = Reveal__QueueCommand(command);
            }

        #endif

        // Set the last commands to internal variables.
        Last_Speed_Target[motor_index] = target_speed;
        Last_Ramp_Target[motor_index] = target_accel;
    }

    return(ret_val);

#else

    return(FALSE);

#endif
}




//---------------------------------------------------------------------------------------------------------------------
/*
 * @brief       Request to stop the motor by invoking the Mci stop command.
 * @param       buffer - a pointer to the received Reveal buffer.
 * @return      Returns whether Mci has accepted or not the command; TRUE = accepted; FALSE = denied
 */
volatile unsigned short stop_motor_counter;
volatile unsigned short stop_motor_counter1;
uint8 Stop_Motor(REVEAL_MSG_TYPE *buffer)
{
#ifdef API220_ENABLE_MCI_CALLS

    uint8 ret_val;
    MOTOR_ENUM_TYPE motor_index;
    sint16 target_accel;
    #ifndef ENABLE_API220_CALLBACKS
        MCI_CMD_RESPONSE_TYPE feedback;
        REVEAL_COMMAND_TYPE *command;
    #endif

    ret_val = FALSE;                                // Default answer is FALSE

    stop_motor_counter++;

    if(buffer->sdu_length == 5)                     // This command requests receiving this amount of bytes, include API and Opcode bytes.
    {
        //custom code
        API220_MCI_STOP_CALLBACK();

        stop_motor_counter1++;

        // Build up the data from the Reveal buffer
        motor_index = (MOTOR_ENUM_TYPE)buffer->payload[0];
        target_accel = ((sint16)(buffer->payload[1])<<8) + buffer->payload[2];

        // Set the last commands to internal variables.
        Last_Speed_Target[motor_index] = 0;
        Last_Ramp_Target[motor_index] = target_accel;

        #ifdef ENABLE_API220_CALLBACKS

            // Invoke Mci stop
            Mci__Stop(motor_index, target_accel);

        #else

            // Invoke Mci stop and read its feedback
            feedback = Mci__Stop(motor_index, target_accel);

            command = Reveal__NewFeedback(API220_MOTOR_CONTROL_NUM, API220_OPCODE_MOTOR_CONTROL_STOP, REVEAL_BROADCAST_NODE, 4);
            if(command != NULL)
            {
                // Build and enqueue the Reveal buffer with the required feedback
                Reveal__AddDataU8(command, motor_index);
                Reveal__AddDataU16(command, target_accel);
                Reveal__AddDataU8(command, (uint8)feedback);

                ret_val = Reveal__QueueCommand(command);
            }
        #endif
    }
    return(ret_val);

#else

    return(FALSE);

#endif
}

volatile MOTOR_ENUM_TYPE motor_index;
#pragma optimize = none

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Get all the digital inputs. It parses the command from Reveal, builds up the
 *              buffer and sends it back to the communication buffer.
 * @param       buffer a point to the incoming Reveal buffer
 * @return      TRUE: The variable was successfully changed.
 *              FALSE: The variable could not be changed.
 */
uint8 Get_Digital(REVEAL_MSG_TYPE *buffer)
{
    uint8 ret_val;
    MOTOR_ENUM_TYPE motor_index;

    ret_val = FALSE;                                // By default, the feedback is always false.

    if(buffer->sdu_length == 3)                     // This command requests receiving this amount of bytes, include API and Opcode bytes.
    {
        // Recover the data from the Reveal buffer
        motor_index = (MOTOR_ENUM_TYPE)buffer->payload[0];

        // Check the sanity of the motor index
        if(motor_index < API220_NR_OF_CONNECTED_MOTORS)
        {
            // Invoke the publishing method with the required feedback
            Publish_Digital_Input(motor_index, Mci_Digital_Inputs[motor_index]);

            ret_val = TRUE;                         // Command is accepted
        }
    }

    return(ret_val);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Get all the digital inputs. It parses the command from Reveal, builds up the
 *              buffer and sends it back to the communication buffer.
 * @param       buffer a point to the incoming Reveal buffer
 * @return      TRUE: The variable was successfully changed.
 *              FALSE: The variable could not be changed.
 */
uint8 Get_Error(REVEAL_MSG_TYPE *buffer)
{
    uint8 ret_val;
    MOTOR_ENUM_TYPE motor_index;
    uint8 param_value;

    ret_val = FALSE;                                // By default, the feedback is always false.

    if(buffer->sdu_length == 4)                     // This command requests receiving this amount of bytes, include API and Opcode bytes.
    {
        // Recover the data from the Reveal buffer
        motor_index = (MOTOR_ENUM_TYPE)buffer->payload[0];
        param_value = (uint8)buffer->payload[1];

        // Check the sanity of the motor index
        if(motor_index < API220_NR_OF_CONNECTED_MOTORS  &&  param_value == 0)
        {
            // Invoke the publishing method with the required feedback
            Publish_Error(motor_index, Mci_Digital_Error[motor_index]);

            ret_val = TRUE;                         // Command is accepted
        }else if(motor_index < API220_NR_OF_CONNECTED_MOTORS  &&  param_value == 255)
        {
        	// Clear error holder
        	 Mci__ClearError(motor_index);
            // Update the error
        	 Mci_Digital_Error[motor_index] = Mci__GetErrorList(motor_index);
        	 // Invoke the publishing method with the required feedback
        	 Publish_Error(motor_index, Mci_Digital_Error[motor_index]);
        	 ret_val = TRUE;                         // Command is accepted
        }
    }

    return(ret_val);
}
#endif

//---------------------------------------------------------------------------------------------------------------------
/*
 * @brief       Periodically update the digital inputs log from Mci. In the case any bit is changed in the log status,
 *              then the event needs to be published over the Reveal stack.
 */
void Update_Digital_Inputs(void)
{

    uint32 digital_in_temp[API220_NR_OF_CONNECTED_MOTORS];
    static MOTOR_ENUM_TYPE motor_index = MOTOR0;

    // Motor index is handled in a such way that no nested for loops are performed,
    // as the computational effort is decreased, it saves cpu resources, e.g., cpu usage
    // by having only the side effect to take more time to sweep all digital failures
    // configured in the Mci look up tables.
    motor_index++;
    if(motor_index >= API220_NR_OF_CONNECTED_MOTORS)
    {
        motor_index = MOTOR0;
    }

    digital_in_temp[motor_index] = Mci__GetDigitalList(motor_index);

    // Check for any differences between the previous log status to the actual one...
    if(Mci_Digital_Inputs[motor_index] != digital_in_temp[motor_index])
    {
    	// If any bit is inverted, then check if the command was properly enqueued on the Reveal buffer
    	// if the command was not enqueued, then, keep trying until the value is sent.
    	if(Publish_Digital_Input(motor_index, digital_in_temp[motor_index]) == TRUE)
    	{
    		// Publish the digital inputs to the Reveal bus
    		Mci_Digital_Inputs[motor_index] = digital_in_temp[motor_index];
    	}
    }
}



//---------------------------------------------------------------------------------------------------------------------
/*
 * @brief       Periodically update the digital inputs log from Mci. In the case any bit is changed in the log status,
 *              then the event needs to be published over the Reveal stack.
 */
void Update_Error(void)
{

    uint32 error_temp[API220_NR_OF_CONNECTED_MOTORS];
    static MOTOR_ENUM_TYPE motor_index = MOTOR0;

    // Motor index is handled in a such way that no nested for loops are performed,
    // as the computational effort is decreased, it saves cpu resources, e.g., cpu usage
    // by having only the side effect to take more time to sweep all digital failures
    // configured in the Mci look up tables.
    motor_index++;
    if(motor_index >= API220_NR_OF_CONNECTED_MOTORS)
    {
        motor_index = MOTOR0;
    }

    error_temp[motor_index] = Mci__GetErrorList(motor_index);

    // Check for any differences between the previous log status to the actual one...
    if(Mci_Digital_Error[motor_index] != error_temp[motor_index])
    {
    	// If any bit is inverted, then check if the command was properly enqueued on the Reveal buffer
    	// if the command was not enqueued, then, keep trying until the value is sent.
    	if(Publish_Error(motor_index, error_temp[motor_index]) == TRUE)
    	{
    		// Publish the digital inputs to the Reveal bus
    		Mci_Digital_Error[motor_index] = error_temp[motor_index];
    	}
    }
}





//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Publish the digital data with the latest log
 * @param       motor_index motor indexer between 0 and user defined
 * @param       digital_input the bitwise flags read from the Mci digital data
 * @return      TRUE: command was properly enqueued.
 *              FALSE: command was not enqueued; feedback from Reveal stack.
 */
T_BOOL Publish_Digital_Input(MOTOR_ENUM_TYPE motor_index, uint32 digital_input)
{
    T_BOOL ret_val;
    REVEAL_COMMAND_TYPE *command;

    ret_val = FALSE;                                // Default answer is false.

    command = Reveal__NewFeedback(API220_MOTOR_CONTROL_NUM, API220_OPCODE_MOTOR_CONTROL_GET_DIGITAL, REVEAL_BROADCAST_NODE, 5);
    if(command != NULL)
    {
        // Build and enqueue the Reveal buffer with the required feedback
        Reveal__AddDataU8(command, motor_index);
        Reveal__AddDataU32(command, digital_input);

        ret_val = (T_BOOL)Reveal__QueueCommand(command);
    }

    return(ret_val);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Publish the digital data with the latest log
 * @param       motor_index motor indexer between 0 and user defined
 * @param       digital_input the bitwise flags read from the Mci digital data
 * @return      TRUE: command was properly enqueued.
 *              FALSE: command was not enqueued; feedback from Reveal stack.
 */
T_BOOL Publish_Error(MOTOR_ENUM_TYPE motor_index, uint32 errors)
{
    T_BOOL ret_val;
    REVEAL_COMMAND_TYPE *command;

    ret_val = FALSE;                                // Default answer is false.

    command = Reveal__NewFeedback(API220_MOTOR_CONTROL_NUM, API220_OPCODE_MOTOR_CONTROL_GET_ERROR, REVEAL_BROADCAST_NODE, 5);
    if(command != NULL)
    {
        // Build and enqueue the Reveal buffer with the required feedback
        Reveal__AddDataU8(command, motor_index);
        Reveal__AddDataU32(command, errors);

        ret_val = (T_BOOL)Reveal__QueueCommand(command);
    }

    return(ret_val);
}


#ifndef API220_FVT_ONLY
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Set digital data in the Mci. The data comes from Reveal, is parsed within this method. If there is a
 *              free spot in the Reveal stack, then the Mci set digital command is performed
 * @param       buffer a pointer to the incoming Reveal buffer
 * @return      TRUE: The variable was successfully changed.
 *              FALSE: The variable could not be changed.
 */
uint8 Set_Digital(REVEAL_MSG_TYPE *buffer)
{
    uint8 dig_feedback;
    uint8 ret_val;
    MOTOR_ENUM_TYPE motor_index;
    MCI_DO_TYPE dig_index;
    uint8 dig_data;
    REVEAL_COMMAND_TYPE *command;

    ret_val = FALSE;                                // Default answer is false.

    if(buffer->sdu_length == 5)                     // This command requests receiving this amount of bytes, include API and Opcode bytes.
    {
        command = Reveal__NewFeedback(API220_MOTOR_CONTROL_NUM, API220_OPCODE_MOTOR_CONTROL_SET_DIGITAL, REVEAL_BROADCAST_NODE, 4);
        if(command != NULL)
        {
            // Recover the data from the serial communication.
            motor_index = (MOTOR_ENUM_TYPE)buffer->payload[0];
            dig_index = (MCI_DO_TYPE)buffer->payload[1];
            dig_data = (MCI_DO_TYPE)buffer->payload[2];
            dig_data = GET_TRUE_FALSE(dig_data);

#ifdef API220_ENABLE_MCI_CALLS
            // Invoke the Mci Command.
            dig_feedback = Mci__SetDigital(motor_index, dig_index, (T_BOOL)dig_data);
#else
            dig_feedback = FALSE;
#endif

            // Build and enqueue the Reveal buffer with the required feedback
            Reveal__AddDataU8(command, motor_index);
            Reveal__AddDataU8(command, dig_index);
            Reveal__AddDataU8(command, dig_data);
            Reveal__AddDataU8(command, dig_feedback);

            ret_val = Reveal__QueueCommand(command);
        }
    }

    return(ret_val);
}



//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Get analog data from Mci according to the incoming Reveal buffer.
 * @param       buffer a pointer to the incoming Reveal buffer
 * @return      TRUE data is read and has properly fedback the Reveal protocol
 *              FALSE data wasn't read nor built in the Reveal stack
 */
uint8 Get_Analog(REVEAL_MSG_TYPE *buffer)
{
    uint8 ret_val;
    MOTOR_ENUM_TYPE motor_index;
    MCI_AI_TYPE analog_data_index;

    ret_val = FALSE;                                // Default answer is false.

    if(buffer->sdu_length == 4)                     // This command requests receiving this amount of bytes, include API and Opcode bytes.
    {
        motor_index = (MOTOR_ENUM_TYPE)buffer->payload[0];
        analog_data_index = (MCI_AI_TYPE)buffer->payload[1];

        ret_val = Publish_Analog_Input(motor_index, analog_data_index);
    }

    return(ret_val);
}




//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Publish on the Reveal bus the requested analog data.
 * @param       motor_index is the requested motor index connected to the Mci API implementation.
 * @param       analog_data_index indexes the requested analog data from the Mci API implementation
 * @return      TRUE data was properly read and set in the Reveal buffer
 *              FALSE data wasn't able to be read nor be built on the on the Reveal buffer
 */
uint8 Publish_Analog_Input(MOTOR_ENUM_TYPE motor_index, MCI_AI_TYPE analog_data_index)
{
    uint16 temp_u16;
    uint8 ret_val;
    sint32 analog_data;
    REVEAL_COMMAND_TYPE *command;

    ret_val = FALSE;                            // By default, the feedback is FALSE.

    // Check and, if available, enqueue a new Reveal feedback.
    command = Reveal__NewFeedback(API220_MOTOR_CONTROL_NUM, API220_OPCODE_MOTOR_CONTROL_GET_ANALOG, REVEAL_BROADCAST_NODE, 6);
    if(command != NULL)
    {
        if(analog_data_index < MCI_AI_NR_OF_AI)
        {
         	// Invoke Mci with required analog data to be read
        	analog_data = (sint32)Mci__GetAnalog(motor_index, analog_data_index);

        	// Build and enqueue the Reveal buffer with the required feedback
        	Reveal__AddDataU8(command, motor_index);
        	Reveal__AddDataU8(command, analog_data_index);
        	Reveal__AddDataU32(command, (uint32)analog_data);        
        }
        else
        {
            // Build and enqueue the Reveal buffer with the required feedback
            Reveal__AddDataU8(command, motor_index);
            Reveal__AddDataU8(command, analog_data_index);

            temp_u16 = (uint16)(Mci__GetAnalog(MOTOR0, MCI_AI_SPEED_REFERENCE_RPM_S16)>>16);
            Reveal__AddDataU16(command, temp_u16);

            temp_u16 = (uint16)(Mci__GetAnalog(MOTOR0, MCI_AI_ACTUAL_SPEED_S16)>>16);
            Reveal__AddDataU16(command, temp_u16);

            temp_u16 = (uint16)Mci__GetAnalog(MOTOR0, MCI_AI_INTERNAL_MCI_STATE);
            Reveal__AddDataU8(command, (uint8)temp_u16);

            temp_u16 = (uint16)(Mci__GetAnalog(MOTOR0, MCI_AI_BUS_VOLTAGE_S16)>>16);
            Reveal__AddDataU16(command, temp_u16);
        }

        ret_val = Reveal__QueueCommand(command);
    }

    return(ret_val);
}




//---------------------------------------------------------------------------------------------------------------------
/*
 * @brief       Set analog data in the Mci.
 * @param       buffer a pointer to the incoming Reveal buffer
 * @return      TRUE: The variable was successfully changed.
 *              FALSE: The variable could not be changed.
 */
uint8 Set_Analog(REVEAL_MSG_TYPE *buffer)
{
    uint8 ret_val;
    uint8 feedback;
    MOTOR_ENUM_TYPE motor_index;
    MCI_AO_TYPE analog_data_index;
    uint32 analog_data;
    REVEAL_COMMAND_TYPE *command;

    ret_val = FALSE;                                // Default answer is FALSE

    if(buffer->sdu_length == 8)                     // This command requests receiving this amount of bytes, include API and Opcode bytes.
    {
        // Check and, if available, enqueue a new Reveal feedback.
        command = Reveal__NewFeedback(API220_MOTOR_CONTROL_NUM, API220_OPCODE_MOTOR_CONTROL_SET_ANALOG, REVEAL_BROADCAST_NODE, 7);
        if(command != NULL)
        {
            motor_index = (MOTOR_ENUM_TYPE)buffer->payload[0];
            analog_data_index = (MCI_AO_TYPE)buffer->payload[1];
            analog_data =   (((uint32)buffer->payload[2])<<24) +
                            (((uint32)buffer->payload[3])<<16) +
                            (((uint32)buffer->payload[4])<<8)  +
                            (((uint32)buffer->payload[5]));

#ifdef API220_ENABLE_MCI_CALLS
            // Invoke Mci to write the analog data
            feedback = Mci__SetAnalog(motor_index, analog_data_index, analog_data);
#else
            feedback = FALSE;
#endif

            // Update last user analog write request
            Last_Analog_Output[motor_index][analog_data_index] = analog_data;

            // Build and enqueue the Reveal buffer with the required feedback
            Reveal__AddDataU8(command, motor_index);
            Reveal__AddDataU8(command, analog_data_index);
            Reveal__AddDataU32(command, analog_data);
            Reveal__AddDataU8(command, feedback);

            ret_val = Reveal__QueueCommand(command);
        }
    }

    return(ret_val);
}




//---------------------------------------------------------------------------------------------------------------------
/*
 * @brief       Parse the buffer that sets the parameters to periodically publish the analog data.
 * @param       buffer a Reveal buffer to be parsed
 * @return      TRUE the command accepted with all parameters correct. FALSE something was wrong in the requested data
 */
uint8 Set_Data_Periodically_Publish(REVEAL_MSG_TYPE *buffer)
{
    MOTOR_ENUM_TYPE motor_index;
    MCI_AI_TYPE analog_data_index;
    uint8 publish_rate;
    uint8 ret_val;

    ret_val = FALSE;                                // Default answer is FALSE

    if(buffer->sdu_length == 5)                     // This command requests receiving this amount of bytes, include API and Opcode bytes.
    {
        // Build the data from the buffer
        motor_index = (MOTOR_ENUM_TYPE)buffer->payload[0];
        analog_data_index = (MCI_AI_TYPE)buffer->payload[1];
        publish_rate = buffer->payload[2];

        // Sanity check on the given parameters
        if( (motor_index < API220_NR_OF_CONNECTED_MOTORS) &&
            (analog_data_index < MCI_AI_NR_OF_AI)   )
        {
            // If the rate is different than zero, it means that something has to be published periodically
            if(publish_rate)
            {
                // Enable the periodic publishing according to the received bytes.
                Api220_Per_Analog_Data[motor_index][analog_data_index].Publish_Requested = TRUE;
                Api220_Per_Analog_Data[motor_index][analog_data_index].Time_Threshold = (publish_rate * 40)/API220_NR_OF_CONNECTED_MOTORS;
                Api220_Per_Analog_Data[motor_index][analog_data_index].Time_Counter = 0;
            }
            else // otherwise, it is requested to stop publishing
            {
                // Disable the periodic publishing.
                Api220_Per_Analog_Data[motor_index][analog_data_index].Publish_Requested = FALSE;
                Api220_Per_Analog_Data[motor_index][analog_data_index].Time_Threshold = 0;
                Api220_Per_Analog_Data[motor_index][analog_data_index].Time_Counter = 0;
            }

            ret_val = TRUE;                         // command accepted.
        }

    }

    return(ret_val);
}
#endif
#if (MCI_FVT_TESTS == ENABLED)




//---------------------------------------------------------------------------------------------------------------------
/*
 * @brief       Parse commands required by FVT mode.
 * @details     Parse and communicates to FVT tests, start, stop, request and publish results.
 * @param       buffer a Reveal buffer to be parsed
 * @return      TRUE the command accepted with all parameters correct. FALSE something was wrong in the requested data
 */


static uint8 Api220_Fvt_Mode(REVEAL_MSG_TYPE *buffer)
{
    API220_FVT_COMMANDS_TYPE fvt_command;
    uint8 feedback;
    MOTOR_ENUM_TYPE motor_index;
    REVEAL_COMMAND_TYPE *command;
    uint8 ret_val;

    ret_val = FALSE;                                // Default answer is FALSE

    // parse incoming package
    fvt_command = (API220_FVT_COMMANDS_TYPE)buffer->payload[0];
    motor_index = (MOTOR_ENUM_TYPE)buffer->payload[1];

    switch(fvt_command)
    {
        case API220_FVT_START:

            //custom code
            API220_FVT_START_CALLBACK();

            // Check and, if available, enqueue a new Reveal feedback.
            command = Reveal__NewFeedback(API220_MOTOR_CONTROL_NUM, API220_OPCODE_MOTOR_CONTROL_FVT, REVEAL_BROADCAST_NODE, 6);
            if(command != NULL)
            {
                feedback = FVT_START(motor_index);

                Api220_Fvt_Result_Published = FALSE;

                Reveal__AddDataU8(command, fvt_command);
                Reveal__AddDataU8(command, motor_index);
                Reveal__AddDataU8(command, feedback);

                ret_val = Reveal__QueueCommand(command);
            }
            break;

        case API220_FVT_STOP:

            //custom code
            API220_FVT_STOP_CALLBACK();

            // Check and, if available, enqueue a new Reveal feedback.
            command = Reveal__NewFeedback(API220_MOTOR_CONTROL_NUM, API220_OPCODE_MOTOR_CONTROL_FVT, REVEAL_BROADCAST_NODE, 6);
            if(command != NULL)
            {
                feedback = FVT_STOP(motor_index);

                Reveal__AddDataU8(command, fvt_command);
                Reveal__AddDataU8(command, motor_index);
                Reveal__AddDataU8(command, feedback);

                ret_val = Reveal__QueueCommand(command);
            }
            break;

        case API220_FVT_REQUEST_RESULT:
            ret_val = Publish_Fvt_Result();
            break;

        default:
            // Left blank intentionally
            break;
    }



    return(ret_val);
}

uint8 Publish_Fvt_Result(void)
{
    REVEAL_COMMAND_TYPE *command;
    MCI_FVT_TEST_RESULT_TYPE fvt_result;
    MCL_FVT_PARAMS_TYPE *fvt_parameters;
    MOTOR_ENUM_TYPE motor_index;
    uint8 ret_val;

    ret_val = FALSE;

#ifdef MCL_FVT_LEGACY
    // Check and, if available, enqueue a new Reveal feedback.
    command = Reveal__NewFeedback(API220_MOTOR_CONTROL_NUM, API220_OPCODE_MOTOR_CONTROL_FVT, REVEAL_BROADCAST_NODE, 6);
    if(command != NULL)
    {
        MclFvt__GetResult(&fvt_result);
        fvt_parameters = MclFvt__GetParameters();
        motor_index = FVT_GET_MOTOR();

        Reveal__AddDataU8(command, API220_FVT_REQUEST_RESULT);
        Reveal__AddDataU8(command, motor_index);

        Reveal__AddDataU8(command, fvt_result.Error_List);

        for(uint8 i=0; i< (fvt_parameters->ADC_Injection_Steps); i++)
        {
            Reveal__AddDataU16(command, (uint16)(fvt_result.current_inj[i].Sensors_Test_Avg_DC_Bus_Min));
            Reveal__AddDataU16(command, (uint16)(fvt_result.current_inj[i].Sensors_Test_Avg_Phase_Volt_A));
            Reveal__AddDataU16(command, (uint16)(fvt_result.current_inj[i].Sensors_Test_Avg_Phase_Volt_B));
            Reveal__AddDataU16(command, (uint16)(fvt_result.current_inj[i].Sensors_Test_Avg_Phase_Volt_C));
            Reveal__AddDataU16(command, (uint16)(fvt_result.current_inj[i].Sensors_Test_Avg_Curr_A));
            Reveal__AddDataU16(command, (uint16)(fvt_result.current_inj[i].Sensors_Test_Avg_Curr_B));
            Reveal__AddDataU16(command, (uint16)(fvt_result.current_inj[i].Sensors_Test_Avg_Curr_C));
        }

        Reveal__AddDataU16(command, (uint16)(fvt_result.Surge_Test_Avg_DC_Bus_Volt_Open));
        Reveal__AddDataU16(command, (uint16)(fvt_result.Surge_Test_Avg_DC_Bus_Volt_Close));

        Reveal__AddDataU8(command, fvt_result.OC_Test_Found[0]);

        // first packet
        ret_val = Reveal__QueueCommand(command);
    }

#else

    // Check and, if available, enqueue a new Reveal feedback.
    command = Reveal__NewFeedback(API220_MOTOR_CONTROL_NUM, API220_OPCODE_MOTOR_CONTROL_FVT, REVEAL_BROADCAST_NODE, 6);
    if(command != NULL)
    {
        MclFvt__GetResult(&fvt_result);
        fvt_parameters = MclFvt__GetParameters();
        motor_index = FVT_GET_MOTOR();

        Reveal__AddDataU8(command, API220_FVT_REQUEST_RESULT_FVT2_1);
        Reveal__AddDataU8(command, motor_index);
        Reveal__AddDataU8(command, fvt_result.Error_List);


        Reveal__AddDataU16(command, (uint16)(fvt_result.adc_idle.Sensors_Test_Avg_DC_Bus_Idle));
        Reveal__AddDataU16(command, (uint16)(fvt_result.adc_idle.Sensors_Test_Avg_Inverter_Temp));
        Reveal__AddDataU16(command, (uint16)(fvt_result.adc_idle.Sensors_Test_Avg_Micro_Temp));
        Reveal__AddDataU16(command, (uint16)(fvt_result.adc_idle.Sensors_Test_Avg_Delta_Temp));


        for(uint8 i=0; i<MCLFVT_INJECTION_POSITION_STEPS_MAX; i++)
        {
            Reveal__AddDataU16(command, (uint16)(fvt_result.current_inj[i].Sensors_Test_Avg_DC_Bus_Min));
            Reveal__AddDataU16(command, (uint16)(fvt_result.current_inj[i].Sensors_Test_Avg_DC_Bus_Max));
            Reveal__AddDataU16(command, (uint16)(fvt_result.current_inj[i].Sensors_Test_Avg_DC_Bus_Ripple));
            Reveal__AddDataU16(command, (uint16)(fvt_result.current_inj[i].Sensors_Test_Avg_Phase_Volt_A));
            Reveal__AddDataU16(command, (uint16)(fvt_result.current_inj[i].Sensors_Test_Avg_Phase_Volt_B));
            Reveal__AddDataU16(command, (uint16)(fvt_result.current_inj[i].Sensors_Test_Avg_Phase_Volt_C));
            Reveal__AddDataU16(command, (uint16)(fvt_result.current_inj[i].Sensors_Test_Avg_Curr_A));
            Reveal__AddDataU16(command, (uint16)(fvt_result.current_inj[i].Sensors_Test_Avg_Curr_B));
            Reveal__AddDataU16(command, (uint16)(fvt_result.current_inj[i].Sensors_Test_Avg_Curr_C));
        }

        // first packet
        ret_val = Reveal__QueueCommand(command);
    }


    command = Reveal__NewFeedback(API220_MOTOR_CONTROL_NUM, API220_OPCODE_MOTOR_CONTROL_FVT, REVEAL_BROADCAST_NODE, 6);

    if(command != NULL)
    {
        Reveal__AddDataU8(command, API220_FVT_REQUEST_RESULT_FVT2_2);
        Reveal__AddDataU8(command, motor_index);

        Reveal__AddDataU16(command, (uint16)(fvt_result.Surge_Test_Avg_DC_Bus_Volt_Open));
        Reveal__AddDataU16(command, (uint16)(fvt_result.Surge_Test_Avg_DC_Bus_Volt_Close));

        for(uint8 i= 0; i<MCLFVT_OC_POSITION_STEPS_MAX; i++)
        {
            Reveal__AddDataU8(command, (uint8)(fvt_result.OC_Test_Found[i]));
        }
        // second packet
        ret_val = Reveal__QueueCommand(command);
    }
#endif

    return(ret_val);
}

#endif
#ifndef API220_FVT_ONLY
//---------------------------------------------------------------------------------------------------------------------
/*
 * @brief       Handle the algorithms for periodic analog data publishing.
 */
void Periodically_Publish_Analog(void)
{
    static MOTOR_ENUM_TYPE motor_index;
    uint8 analog_data_index;

    // Handle the connected motor amount, it shall sweep of number of the connected motors
    motor_index++;
    if(motor_index >= API220_NR_OF_CONNECTED_MOTORS)
    {
        motor_index = MOTOR0;
    }

    // Sweeps all the requested analog data for publishing
    for(analog_data_index = 0; analog_data_index < (1+MCI_AI_NR_OF_AI); analog_data_index++)
    {
        // Check if the data defined in the PUBLISH_ANALOG_DATA_TYPE is requested for periodic publishing
        if(Api220_Per_Analog_Data[motor_index][analog_data_index].Publish_Requested == TRUE)
        {
            // If the publishing time is not yet over, then
            if(Api220_Per_Analog_Data[motor_index][analog_data_index].Time_Counter)
            {
                // count the time.
                Api220_Per_Analog_Data[motor_index][analog_data_index].Time_Counter--;
            }
            else // otherwise
            {
                // attempt to build and stack the command on the wide, if it was stacked,
                if(Publish_Analog_Input(motor_index, (MCI_AI_TYPE)analog_data_index) == TRUE)
                {
                    // then reload the time for publishing.
                    Api220_Per_Analog_Data[motor_index][analog_data_index].Time_Counter = Api220_Per_Analog_Data[motor_index][analog_data_index].Time_Threshold;
                }
            }
        }
    }
}




//---------------------------------------------------------------------------------------------------------------------
/*
 * @brief       Manual control of the inverter output
 * @detail      The output can be manually controlled, this output might be the voltage vector or the current vector.
 * @param       buffer - a pointer to the received Reveal buffer.
 * @return      Returns whether Mci has accepted or not the command; TRUE = accepted; FALSE = denied
 */
uint8 Manual_Control(REVEAL_MSG_TYPE *buffer)
{
#ifdef API220_ENABLE_MCI_CALLS

    uint8 ret_val;
    MOTOR_ENUM_TYPE motor_index;
    MCI_INJECTION_TYPE method;
    sint32 level_target;
    sint32 level_rate;
    sint32 parameter;
    sint32 param_rate;

    #ifndef ENABLE_API220_CALLBACKS
        MCI_CMD_RESPONSE_TYPE feedback;
        REVEAL_COMMAND_TYPE *command;
    #endif

    ret_val = FALSE;                                // Default answer is FALSE

    if(buffer->sdu_length == 12)                    // This command requests receiving this amount of bytes, include API and Opcode bytes.
    {
        API220_MANUAL_INJECTION_CALLBACK();

        // Build up the data from the Reveal buffer
        motor_index = (MOTOR_ENUM_TYPE)buffer->payload[0];
        method = (MCI_INJECTION_TYPE)buffer->payload[1];
        level_target = (((sint16)buffer->payload[2])<<8) + buffer->payload[3];
        level_rate   = (((sint16)buffer->payload[4])<<8) + buffer->payload[5];
        parameter    = (((sint16)buffer->payload[6])<<8) + buffer->payload[7];
        param_rate   = (((sint16)buffer->payload[8])<<8) + buffer->payload[9];

        #ifdef ENABLE_API220_CALLBACKS

            // Invoke Mci
            Mci__Manual_Injection(motor_index, method, level_target, level_rate, parameter, param_rate);

            ret_val = TRUE;

        #else

            // Invoke Mci manual control and read its feedback
            feedback = MANUAL_INJECTION_RUN(motor_index, method, level_target, level_rate, parameter, param_rate);

            command = Reveal__NewFeedback(API220_MOTOR_CONTROL_NUM, API220_OPCODE_MOTOR_CONTROL_MANUAL_CONTROL, REVEAL_BROADCAST_NODE, 3);
            if(command != NULL)
            {
                // Build and enqueue the Reveal buffer with the required feedback
                Reveal__AddDataU8(command, motor_index);
                Reveal__AddDataU8(command, method);
                Reveal__AddDataU8(command, (uint8)feedback);

                ret_val = Reveal__QueueCommand(command);
            }

        #endif

        // Set the last commands to internal variables.
        Last_Manual_Injection_Method[motor_index] = method;
        Last_Manual_Injection_Level_Target[motor_index] = level_target;
        Last_Manual_Injection_Parameter[motor_index] = parameter;
    }

    return(ret_val);

#else

    return(FALSE);

#endif
}
#endif


#if(API220_DEBUG_MODE == ENABLED)
uint8 Command_Api220_Debug_Mode(REVEAL_MSG_TYPE *buffer)
{
    uint8 ret_val;
    MOTOR_ENUM_TYPE motor_index;
    COMMAND_LIST_API_DEBUG_MODE_TYPE debug_command;
    uint8 emulate_this_failure_index;

    ret_val = FALSE;                                // Default answer is FALSE

    // Build up the data from the Reveal buffer
    motor_index = (MOTOR_ENUM_TYPE)buffer->payload[0];
    debug_command = (COMMAND_LIST_API_DEBUG_MODE_TYPE)buffer->payload[1];

    switch(debug_command)
    {
        case CMD_API_FORCE_ERROR:
            emulate_this_failure_index = buffer->payload[2];
            Mci__Emulate_Error(emulate_this_failure_index);
            break;

#if(API220_THERMALMODEL_DEBUG == ENABLED)
        case CMD_CLASSB_THERMAL_MODEL_DEBUG:
        	ret_val = Stack_Class_B_Debug_Data(motor_index);
        	break;


        case CMD_CLASSA_THERMAL_MODEL_DEBUG:
        	ret_val = Stack_Class_A_Debug_Data(motor_index);
        	break;
#endif

        default:
            //
            break;
    };

    return(ret_val);
}
#endif




#if(API220_THERMALMODEL_DEBUG == ENABLED)
uint8 Stack_Class_B_Debug_Data(MOTOR_ENUM_TYPE motor_index)
{
	REVEAL_COMMAND_TYPE *command;
    uint8 ret_val;
    float temp_f;

	ret_val = FALSE;

    command = Reveal__NewFeedback(API220_MOTOR_CONTROL_NUM, API220_OPCODE_MOTOR_CONTROL_DEBUG, REVEAL_BROADCAST_NODE, 9);
    if(command != NULL)
    {
        // Build and enqueue the Reveal buffer with the required feedback
        Reveal__AddDataU8(command, (uint8)motor_index);

        Reveal__AddDataU8(command, (uint8)CMD_CLASSB_THERMAL_MODEL_DEBUG);

    	temp_f = 65536.0f * SRMotorSafetyMgr__GetVoltageFrequency();
        Reveal__AddDataU32(command, (uint32)temp_f);

    	temp_f = 65536.0f * SRMotorSafetyMgr__GetMotorTemperature();
        Reveal__AddDataU32(command, (uint32)temp_f);

        temp_f = 65536.0f * SRMotorSafetyMgr__GetSquaredCurrents();
        Reveal__AddDataU32(command, (uint32)temp_f);

        ret_val = Reveal__QueueCommand(command);
    }


	return ret_val;
}



uint8 Stack_Class_A_Debug_Data(MOTOR_ENUM_TYPE motor_index)
{
	REVEAL_COMMAND_TYPE *command;
    uint8 ret_val;
    float temp_f;

	ret_val = FALSE;

    command = Reveal__NewFeedback(API220_MOTOR_CONTROL_NUM, API220_OPCODE_MOTOR_CONTROL_DEBUG, REVEAL_BROADCAST_NODE, 9);
    if(command != NULL)
    {
        // Build and enqueue the Reveal buffer with the required feedback
        Reveal__AddDataU8(command, (uint8)motor_index);

        Reveal__AddDataU8(command, (uint8)CMD_CLASSA_THERMAL_MODEL_DEBUG);

        temp_f = 65536.0f * ThermalModelCore_Y.WindingTemperature;
        Reveal__AddDataU32(command, (uint32)temp_f);

        temp_f = 65536.0f * ThermalModelCore_Y.AmbientTemperature;
        Reveal__AddDataU32(command, (uint32)temp_f);

        temp_f = 65536.0f * ThermalModelCore_Y.MagnetTemperature;
        Reveal__AddDataU32(command, (uint32)temp_f);

        temp_f = 65536.0f * ThermalModelCore_U.Temperature;
        Reveal__AddDataU32(command, (uint32)temp_f);

        temp_f = 65536.0f * ThermalModelCore_U.Current;
        Reveal__AddDataU32(command, (uint32)temp_f);

        temp_f = 65536.0f * ThermalModelCore_U.Speed;
        Reveal__AddDataU32(command, (uint32)temp_f);

        ret_val = Reveal__QueueCommand(command);
    }


	return ret_val;
}
#endif

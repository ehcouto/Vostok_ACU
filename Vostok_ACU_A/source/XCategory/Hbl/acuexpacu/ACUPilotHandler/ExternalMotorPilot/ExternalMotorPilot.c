/**
 * @file
 * @brief       Implements a motor pilot based on API221 and an externally controlled motor.
 *
 * @details     This module uses API221MotionCtrl to control an external motor controller. It
 *              accepts two types of commands: simple and normal commands. They are identified
 *              using bit 7 of the first byte of the command. 0 = Simple command. 1 = Custom
 *              command.
 *
 *              Simple Commands:
 *                  EXTERNAL_MOTOR_NO_COMMAND: 0x00
 *                  This command indicates that no module requested anything for the external
 *                  motor. In this case, the pilot will turn off the motor.
 *
 *                  EXTERNAL_MOTOR_NO_CHANGE: 0x01
 *                  This command indicates that the application requested that the pilot maintain
 *                  the most recent command. In this case, the pilot will not do anything.
 *
 *                  EXTERNAL_MOTOR_STOP: 0x02, deceleration (lo,hi)
 *                  The pilot will send the Stop command with the specified deceleration. However,
 *                  if this command is a copy of the previously sent command, then this command
 *                  will do nothing.
 *
 *                  EXTERNAL_MOTOR_RUN: 0x03, velocity (lo,hi), acceleration (lo,hi)
 *                  The pilot will send the Run command with the specified velocity and
 *                  acceleration. However, if this command is a copy of the previously sent
 *                  command, then this command will do nothing.
 *
 *              Custom commands:
 *                  Always the same format: 0x80 + size, buffer
 *              These commands are used to perform custom platform-specific behaviors. Neither
 *              this module, nor the API221 module have any code to implement these custom
 *              behaviors. The platform-specific code on the API221 server will parse the buffer.
 *
 *              Custom commands are only expected to be set on change. In between changes, the
 *              EXTERNAL_MOTOR_NO_CHANGE simple command should be sent.
 *
 *              If the external motor control node is not connected on the Reveal communications
 *              bus, then the ExternalMotorPilot will save the command until the node connects. It will
 *              then send the most recent command.
 *
 * Setting file data structures for ExternalMotorPilot
 *      ExternalMotorPilot Configuration
 *              | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 | 7 | 6 | 5 | 4 | 3 | 2 | 1 | 0 |
 *      +0000h  |        Refresh_Seconds        |          Motor_Index          |
 *      +0001h  |         Deceleration          | 0 | 0 | 0 | 0 | 0 | 0 | 0 | 0 |
 *
 *          Refer to the #EXTERNAL_MOTOR_PILOT_SF_CONFIG_TYPE for descriptions of the fields.
 *
    @startuml{ExternalMotorPilot_sm.png}
        title External Motor Pilot State Machine
        ' Use state = Emp_This->State

        [*] --> Disconnected

        Disconnected : do / CheckForMcuConnect()
        Disconnected --> Connected : evMcuConnected\n/ SendProcessed()

        Connected : do / CheckForMcuDisconnect()
        Connected --> Disconnected : evMcuDisconnected
    @enduml
 *
 * @copyright   Copyright 2016 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
//lint -e766 Suppress Info: 'SystemConfig.h' header not used in module
#include "SystemConfig.h"
#if (HBL_PILOT_NUM_EXTERNAL_MOTOR > 0)
#include "ExternalMotorPilot.h"

// -- Other Modules --
#include "API221MotionCtrl.h"
#include <limits.h>
#include "Log.h"
#include <string.h>
#include "SystemTimers.h"


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for External Motor Pilot State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the External Motor Pilot state machine.
typedef enum EXTERNAL_MOTOR_PILOT_STATE_ENUM
{
    STATE_EXTERNAL_MOTOR_PILOT_NONE         = 0,

    STATE_EXTERNAL_MOTOR_PILOT_DISCONNECTED = 1,    //!< (initial_state)
    STATE_EXTERNAL_MOTOR_PILOT_CONNECTED    = 2,

    STATE_EXTERNAL_MOTOR_PILOT_END          = 3
} EXTERNAL_MOTOR_PILOT_STATE_TYPE;

// -- Private Function Prototypes used by the state machine for customization functions --
static void CheckForMcuConnect(void);
static void CheckForMcuDisconnect(void);
static void SendProcessed(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventMcuConnected(void);
static void EventMcuDisconnected(void);
static void ExternalMotorPilotExecute(void);
static void ExternalMotorPilotInitialize(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


//  --- Private Declarations ------------------------------------------------------------------------------------------


// -- Private Constant Declarations --

//! The name of this module for use by the Log module.
#define MODULE_NAME MODULE_EXTERNAL_MOTOR_PILOT


// -- Private Enumerated Constant Declarations --

//! A list of log message IDs that are generated by this module.
typedef enum MODULE_EXTERNAL_MOTOR_PILOT_LOG_MESSAGE_ID_ENUM
{
    INVALID_MOTOR_REQUEST = 1,                      // Data: HiByte = Motor index, LoByte = Request first byte.
    DETECTED_DISCONNECTED_MOTOR_NODE                // Data = Motor index.
} MODULE_EXTERNAL_MOTOR_PILOT_LOG_MESSAGE_ID_TYPE;


// -- Private Structure Type Declarations --

//! The setting file data associated with an external motor pilot.
typedef PACKED struct EXTERNAL_MOTOR_PILOT_SF_CONFIG_STRUCT
{
    //! The client-side motor index to use when sending a command.
    API221_CLIENT_MOTOR_INDEX_TYPE Motor_Index : 3;

    uint8 Reserved : 1;

    //! Refresh time between sending "Keep Running" commands to the MCU.
    //! Set to 0 to disable the "Keep Running" commands to the MCU.
    uint8 Refresh_Seconds : 4;

    //! The deceleration to use when stopping after no command is given.
    uint8 Deceleration;
} EXTERNAL_MOTOR_PILOT_SF_CONFIG_TYPE;

//! Motor pilot parameters type definition.
typedef struct EXTERNAL_MOTOR_PILOT_DATA_STRUCT
{
    //! The currently requested command for the external motor.
    EXTERNAL_MOTOR_COMMAND_TYPE Requested;

    //! The previously processed command for the external motor.
    EXTERNAL_MOTOR_COMMAND_TYPE Processed;

    //! The current state of the external motor pilot.
    uint8 State;
} EXTERNAL_MOTOR_PILOT_DATA_TYPE;


// -- Private Macro Declarations --

//! The acceleration (or deceleration) to use when processing a simple command to run or stop.
//! This macro will use a default value if none is provided in the setting file.
#define EMP_DECELERATION()          ((Emp_Sf == NULL) ? 50 : (Emp_Sf->Deceleration))

//! Access the motor index from the setting file data.
//! This macro will use a default value if none is provided in the setting file.
#define EMP_MOTOR_INDEX()           ((Emp_Sf == NULL) ? ((API221_CLIENT_MOTOR_INDEX_TYPE)0) : (Emp_Sf->Motor_Index))


//  --- Private Definitions -------------------------------------------------------------------------------------------


// -- Private Variable Definitions --

//! Motor pilot data array. One entry per motor pilot instance.
static EXTERNAL_MOTOR_PILOT_DATA_TYPE Emp[HBL_PILOT_NUM_EXTERNAL_MOTOR];

//! Tracks the number of motor pilot allocations.
static uint8 Emp_Allocation;

//! A pointer to the external motor pilot setting file data for the state machine.
static EXTERNAL_MOTOR_PILOT_SF_CONFIG_TYPE* Emp_Sf;

//! A pointer to the external motor pilot parameters for the state machine.
static EXTERNAL_MOTOR_PILOT_DATA_TYPE* Emp_This;

//! Temporarily holds the status information that indicates whether the motor is connected.
//! FALSE if disconnected. TRUE if connected.
static EXTERNAL_MOTOR_STATUS_TYPE Emp_Status;


//  --- Private Function Prototypes -----------------------------------------------------------------------------------

static uint8 GetCommandSize(uint8 command);


//=====================================================================================================================
//  --- Public Functions ----------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Initialize all the instances of this pilot type.
 *  - Initialize static RAM used by the module.
 *  - Initialize any corrupted NVData used by the module.
 */
void ExternalMotorPilot__Initialize(void)
{
    memset(Emp, 0, sizeof(Emp));
    Emp_Allocation = 0;
    Emp_Sf = NULL;
    Emp_This = NULL;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Reserve memory for this pilot instance.
 * @return The index of the allocated pilot.
 *         Return UCHAR_MAX if there is no space allocated for another pilot of this type.
 */
uint8 ExternalMotorPilot__Allocate(void)
{
    uint8 pilot_index = UCHAR_MAX;
    if (Emp_Allocation < HBL_PILOT_NUM_EXTERNAL_MOTOR)
    {
        pilot_index = Emp_Allocation;
        Emp_Allocation++;
    }
    return pilot_index;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Initialize this pilot instance.
 * @param load = A pointer to the HBL_LOAD_TYPE structure with load and pilot instance information.
 * @return Always returns PASS.
 *         The return value is ignored by HBL.
 */
PASS_FAIL_TYPE ExternalMotorPilot__InitializeInstance(HBL_LOAD_TYPE* load)
{
    if (load != NULL)
    {
        uint8 pilot_index = load->Pilot_Data_Index;
        if (pilot_index < Emp_Allocation)
        {
            // Start the state machine for the pilot.
            Emp_This = &Emp[pilot_index];
            Emp_This->Requested.Standard.Command = EXTERNAL_MOTOR_NO_CHANGE;
            Emp_This->Processed.Standard.Command = EXTERNAL_MOTOR_NO_CHANGE;
            ExternalMotorPilotInitialize();
            return PASS;
        }
    }
    return FAIL;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Update the requested state of the output controlled by this pilot instance.
 * If 'load' correctly identifies a pilot and if 'data' represents an invalid command, then this
 * function will generate a EXTERNAL_MOTOR_NO_COMMAND command.
 * @param load = A pointer to the HBL_LOAD_TYPE structure with load and pilot instance information.
 * @param data = A pointer to memory that contains the new requested pilot state.
 * @return Always returns PASS.
 *         The return value is ignored by HBL.
 */
PASS_FAIL_TYPE ExternalMotorPilot__Request(HBL_LOAD_TYPE* load, void* data)
{
    if ((load != NULL) && (data != NULL))
    {
        // Determine if the pilot index is in range.
        uint8 pilot_index = load->Pilot_Data_Index;
        if (pilot_index < Emp_Allocation)
        {
            // Determine the size of the data.
            EXTERNAL_MOTOR_COMMAND_TYPE* cmd = (EXTERNAL_MOTOR_COMMAND_TYPE*)data;
            uint8 size = GetCommandSize(cmd->Common.Command);

            // Determine if command size is in range.
            if ((1 <= size) && (size <= EXTERNAL_MOTOR_MAX_COMMAND_SIZE))
            {
                // Save the external motor command for later processing.
                memcpy(&Emp[pilot_index].Requested, cmd, size);
                return PASS;
            }
            else
            {
                // Invalid command -- stop the motor.
                LOG_ADD_EXCEPTION(INVALID_MOTOR_REQUEST, COMBINE_BYTES(load->Load_Id, cmd->Standard.Command));
                Emp[pilot_index].Requested.Standard.Command = EXTERNAL_MOTOR_NO_COMMAND;
            }
        }
    }
    return FAIL;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Update the output(s) controlled by this pilot instance based on previous requests.
 *  - If the command is a custom command, then it should always be sent. The same custom
 *      platform-specific commands may need to be sent sequentially more than once.
 *  - If the command is an EXTERNAL_MOTOR_NO_CHANGE command, then there is nothing to do.
 *      The previously processed command should be preserved.
 *  - If the command is a standard command, and it is different from the previously processed
 *      command, then send the new standard command.
 *  - If the command is a standard command, and it is the same as the previously processed
 *      command, then there is nothing to do. Let the Keep Running mechanism preserve the original
 *      standard command.
 * @param load = A pointer to the HBL_LOAD_TYPE structure with load and pilot instance information.
 * @return Always returns PASS.
 *         The return value is ignored by HBL.
 */
PASS_FAIL_TYPE ExternalMotorPilot__Process(HBL_LOAD_TYPE* load)
{
    if (load != NULL)
    {
        uint8 pilot_index = load->Pilot_Data_Index;
        if (pilot_index < Emp_Allocation)
        {
            // Setup the RAM and SettingFile access variables.
            Emp_This = &Emp[pilot_index];
            Emp_Sf = (EXTERNAL_MOTOR_PILOT_SF_CONFIG_TYPE*)(void*)load->Parameter_Ptr.Data;

            // Check if the command needs to be processed.
            EXTERNAL_MOTOR_COMMAND_TYPE* processed = &Emp_This->Processed;
            EXTERNAL_MOTOR_COMMAND_TYPE* cmd = &Emp_This->Requested;
            uint8 size = GetCommandSize(cmd->Common.Command);
            BOOL_TYPE changed = (BOOL_TYPE)((cmd->Standard.Command != EXTERNAL_MOTOR_NO_CHANGE) &&
                                            (memcmp(processed, cmd, size) != 0));
            if ((changed == TRUE) || (cmd->Custom.Is_Custom_Command == TRUE))
            {
                // Process the command.
                memcpy(processed, cmd, size);
                if (Emp_This->State == STATE_EXTERNAL_MOTOR_PILOT_CONNECTED)
                {
                    SendProcessed();
                }
            }

            // Run the pilot state machine.
            if ((Emp_This != NULL) && (Emp_Sf != NULL))
            {
                ExternalMotorPilotExecute();
            }
        }
    }
    return PASS;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the requested state of the output controlled by this pilot instance.
 * This function does not necessarily return the current state of the output.
 * @param load = A pointer to the HBL_LOAD_TYPE structure with load and pilot instance information.
 * @return A pointer to data that describes the requested state of the output.
 */
void* ExternalMotorPilot__GetRequested(HBL_LOAD_TYPE* load)
{
    if (load != NULL)
    {
        uint8 pilot_index = load->Pilot_Data_Index;
        if (pilot_index < Emp_Allocation)
        {
            EXTERNAL_MOTOR_PILOT_DATA_TYPE* pilot = &Emp[pilot_index];
            if (pilot->Requested.Standard.Command == EXTERNAL_MOTOR_NO_CHANGE)
            {
                // Since the request is to not change anything, return the processed.
                return (&pilot->Processed);
            }
            else if (GetCommandSize(pilot->Requested.Common.Command) > 0)
            {
                // Return the latest request.
                return (&pilot->Requested);
            }
        }
    }
    return NULL;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the state of the output controlled by this pilot instance.
 * @param load = A pointer to the HBL_LOAD_TYPE structure with load and pilot instance information.
 * @return A pointer to data that describes the state of the output.
 */
void* ExternalMotorPilot__GetProcessed(HBL_LOAD_TYPE* load)
{
    if (load != NULL)
    {
        uint8 pilot_index = load->Pilot_Data_Index;
        if (pilot_index < Emp_Allocation)
        {
            return (&Emp[pilot_index].Processed);
        }
    }
    return NULL;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the status of this pilot instance.
 * @param load = A pointer to the HBL_LOAD_TYPE structure with load and pilot instance information.
 * @return A pointer to #BOOL_TYPE value that indicates whether the motor is connected.
 *         FALSE = Disconnected. TRUE = Connected.
 */
void* ExternalMotorPilot__GetStatus(HBL_LOAD_TYPE* load)
{
    if (load != NULL)
    {
        Emp_Sf = (EXTERNAL_MOTOR_PILOT_SF_CONFIG_TYPE*)(void*)load->Parameter_Ptr.Data;
        Emp_Status.Motor_Index = (uint8)EMP_MOTOR_INDEX();
        Emp_Status.Is_Connected = API221MotionCtrl__IsMotorPresentOnBus((API221_CLIENT_MOTOR_INDEX_TYPE)Emp_Status.Motor_Index);
        return &Emp_Status;
    }
    return NULL;
}


//=====================================================================================================================
//  --- Private Functions ---------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the size of the referenced command.
 * @param cmd = A command for the external motor controller.
 * @return The size of the referenced command.
 */
static uint8 GetCommandSize(uint8 command)
{
    uint8 size = 0;
    switch (command)
    {
        case EXTERNAL_MOTOR_NO_COMMAND:
        case EXTERNAL_MOTOR_NO_CHANGE:
            size = 1;
            break;

        case EXTERNAL_MOTOR_STOP:
            size = 1 + sizeof(EMP_STOP_PARAM_TYPE);
            break;

        case EXTERNAL_MOTOR_RUN:
            size = 1 + sizeof(EMP_RUN_PARAM_TYPE);
            break;

        case EXTERNAL_MOTOR_WASH:
            size = 1 + sizeof(EMP_WASH_PARAM_TYPE);
            break;

        case EXTERNAL_MOTOR_ROTATE_X_DEGREES:
            size = 1 + sizeof(EMP_ROTATE_PARAM_TYPE);
            break;

        case EXTERNAL_MOTOR_PULSE_IMMEDIATE:
        case EXTERNAL_MOTOR_PULSE_MODIFY:
        case EXTERNAL_MOTOR_PULSE_QUEUE:
            size = 1 + sizeof(EMP_PULSE_PARAM_TYPE);
            break;

        default:
            if (BIT_TEST(command, 7) == 1)
            {
                size = command & 0x7F;
                if (size > EXTERNAL_MOTOR_MAX_COMMAND_SIZE)
                {
                    size = 0;
                }
            }
            break;
    }

    return size;
}


//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Wait for Reveal to detect the MCU node on the communications bus.
 */
static void CheckForMcuConnect(void)
{
    if (API221MotionCtrl__IsMotorPresentOnBus(EMP_MOTOR_INDEX()) == TRUE)
    {
        EventMcuConnected();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Check that the MCU is still connected on the communications bus.
 */
static void CheckForMcuDisconnect(void)
{
    API221_CLIENT_MOTOR_INDEX_TYPE motor_index = EMP_MOTOR_INDEX();
    if (API221MotionCtrl__IsMotorPresentOnBus(motor_index) == FALSE)
    {
        LOG_ADD_EVENT(DETECTED_DISCONNECTED_MOTOR_NODE, motor_index);
        EventMcuDisconnected();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the command for the currently running pilot by sending an API221 command.
 * If no command has been processed since reset, then no API221 command is sent.
 */
static void SendProcessed(void)
{
    // Get the size of the command.
    uint8 size = GetCommandSize(Emp_This->Processed.Common.Command);
    if (size > 0)
    {
        switch (Emp_This->Processed.Common.Command)
        {
            case EXTERNAL_MOTOR_NO_COMMAND:
                // This is a simple stop command.
                // This is also the default command if no command is provided to HBL.
                API221MotionCtrl__DeviceStop(EMP_MOTOR_INDEX(), EMP_DECELERATION());
                break;

            case EXTERNAL_MOTOR_NO_CHANGE:
                // The EXTERNAL_MOTOR_NO_CHANGE value will only be in the Processed structure
                // immediately after initialization. After that, maintaining the previous
                // command is done by not doing anything at all. The previous command was
                // already sent and is still the active command.
                break;

            case EXTERNAL_MOTOR_STOP:
                API221MotionCtrl__DeviceStop(EMP_MOTOR_INDEX(),
                                             Emp_This->Processed.Standard.Params.Stop.Deceleration);
                break;

            case EXTERNAL_MOTOR_RUN:
                API221MotionCtrl__DeviceRun(EMP_MOTOR_INDEX(),
                                            Emp_This->Processed.Standard.Params.Run.Velocity,
                                            Emp_This->Processed.Standard.Params.Run.Acceleration);
                break;

            case EXTERNAL_MOTOR_WASH:
                API221MotionCtrl__DeviceWash(EMP_MOTOR_INDEX(),
                                             &Emp_This->Processed.Standard.Params.Wash);
                break;

            case EXTERNAL_MOTOR_ROTATE_X_DEGREES:
                API221MotionCtrl__DeviceRotate(EMP_MOTOR_INDEX(),
                                               &Emp_This->Processed.Standard.Params.Rotate);
                break;

            case EXTERNAL_MOTOR_PULSE_IMMEDIATE:
            case EXTERNAL_MOTOR_PULSE_MODIFY:
            case EXTERNAL_MOTOR_PULSE_QUEUE:
                API221MotionCtrl__DevicePulse(EMP_MOTOR_INDEX(),
                                              Emp_This->Processed.Standard.Command,
                                              &Emp_This->Processed.Standard.Params.Pulse);
                break;

            default:
                API221MotionCtrl__DeviceMotion(EMP_MOTOR_INDEX(),
                                               (uint8*)&Emp_This->Processed.Custom.Params,
                                               size - 1);
                break;
        }
    }
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for External Motor Pilot State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Mcu Connected Event for the External Motor Pilot state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventMcuConnected(void)
{
    switch (Emp_This->State)
    {
        case STATE_EXTERNAL_MOTOR_PILOT_DISCONNECTED:
            SendProcessed();                        // Transition action function.
            Emp_This->State = STATE_EXTERNAL_MOTOR_PILOT_CONNECTED;
            break;

        default:
            // Event is ignored for all other values of Emp_This->State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Mcu Disconnected Event for the External Motor Pilot state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventMcuDisconnected(void)
{
    switch (Emp_This->State)
    {
        case STATE_EXTERNAL_MOTOR_PILOT_CONNECTED:
            Emp_This->State = STATE_EXTERNAL_MOTOR_PILOT_DISCONNECTED;
            break;

        default:
            // Event is ignored for all other values of Emp_This->State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the External Motor Pilot state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void ExternalMotorPilotExecute(void)
{
    switch(Emp_This->State)
    {
        case STATE_EXTERNAL_MOTOR_PILOT_DISCONNECTED:
            CheckForMcuConnect();                   // Do function.
            break;

        case STATE_EXTERNAL_MOTOR_PILOT_CONNECTED:
            CheckForMcuDisconnect();                // Do function.
            break;

        default:
            // There is nothing to do for all other values of Emp_This->State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the External Motor Pilot state machine.
 * This function resets the state machine to the initial state.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void ExternalMotorPilotInitialize(void)
{
    Emp_This->State = STATE_EXTERNAL_MOTOR_PILOT_DISCONNECTED;
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


#endif      // #if (HBL_PILOT_NUM_EXTERNAL_MOTOR > 0)

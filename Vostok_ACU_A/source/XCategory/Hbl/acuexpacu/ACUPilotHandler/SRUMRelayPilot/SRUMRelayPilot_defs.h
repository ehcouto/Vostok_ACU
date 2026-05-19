/**
 *  @file       SRUMRelayPilot_defs.h
 *
 *  @brief      Standard Universal Motor data definition
 *
 *  @defgroup   CLASS_B
 *
 *  @copyright  Copyright 2019. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRUMRELAYPILOT_DEFS_H_
#define SRUMRELAYPILOT_DEFS_H_
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
// SRUMRelayPilot states
//---------------------------------------------------------------------------------------------------------------------
typedef enum SRUMRELAYPILOT_STATE_ENUM
{
    SRUMRELAYPILOT_INITALIZE = 0,         //!< Module initialization; this state persists on an Initialize failure
    SRUMRELAYPILOT_IDLE,                  //!< Motor initialized properly and ready to start when Target_Speed > 0
    SRUMRELAYPILOT_START,                 //!< One of Direction Relay ON depending on Target_Speed sign; wait to validate Load feedback
    SRUMRELAYPILOT_RUN,                   //!< One of Direction Relay ON and Triac able to run, depending on Firing_Angle_S15
    SRUMRELAYPILOT_SUSPEND,               //!< Triac firing temporary disabled to allow Tapped field relay switch
    SRUMRELAYPILOT_STOP                   //!< Triac firing disabled; wait to validate Load feedback before going to IDLE
} SRUMRELAYPILOT_STATE_TYPE;

//---------------------------------------------------------------------------------------------------------------------
// SRUMRelayPilot Load parameters (Payload)
//---------------------------------------------------------------------------------------------------------------------
typedef PACKED struct SRUMRELAYPILOT_PARAM_STRUCT
{
    sint16 Target_Speed;                  //!< Requested (Drum/Shaft depends by Motor Control) final target speed
    uint16 Acceleration;                  //!< Requested (Drum/Shaft depends by Motor Control) acceleration to reach Target Speed
    union
    {
        uint16 Short;
        struct {
            uint16 Tap_Field_Custom_Speed_Div10:8;  //!< Custom Speed to trigger Tapped Field switch
            uint16 Idle_Relays_On:1;                //!< Set both Direction Relays ON when in IDLE state
            uint16 Set_Firing_Angle:1;              //!< Bypass Motor Control to set a fixed Firing Angle using range [-1000..1000]
                                                    //!     on Target_speed to set direction; 0 means motor stopped, -1000/1000 maximum power
            uint16 Disable_Triac:1;                 //!< Keep Triac firing disabled
            uint16 Force_Tap_Field_On:1;            //!< Forces Tap Field relay ON regardless of Motor Control output
            uint16 Unused_1:4;
        } Data;
    } Custom_Param;
} SRUMRELAYPILOT_PARAM_TYPE;

//---------------------------------------------------------------------------------------------------------------------
// SRUMRelayPilot Load Status (Payload)
//---------------------------------------------------------------------------------------------------------------------
typedef PACKED struct SRUMRELAYPILOT_PILOT_STATUS_STRUCT
{
    HBL_LOAD_STATUS_TYPE Load;
    BOOL_TYPE Is_Running;                       //!< Flag : motor is expected to run or not
    BOOL_TYPE Is_Triac_Firing;                  //!< Flag : is Triac firing process  running or not
    BOOL_TYPE Is_Speed_Reached;                 //!< Flag (from Motor Control): Target speed (as in Processed) has been reached or not
    BOOL_TYPE Has_Start_Failed;                 //!< Flag (from Motor Control): is Motor stopped due to a Start failure
    ON_OFF_TYPE Cw_Relay;                       //!< Clockwise Relay status (ON/OFF)
    ON_OFF_TYPE Ccw_Relay;                      //!< Counterclockwise Relay status (ON/OFF)
    ON_OFF_TYPE Tapped_Field_Relay;             //!< Tapped field Relay status (ON/OFF)
    uint8 Fault;                                //!< Flag (from Motor Control): Fault code - reason motor is not moving
    uint16 Firing_Angle_S15;                    //!< (from Motor Control) Current firing angle (in S15 format: 0..32767)
    sint32 Motor_Speed_S15;                     //!< (from Motor Control) Current Motor Speed (in S15 format: divide by 32768)
    sint32 Motor_Speed_Ref_S15;                 //!< (from Motor Control) Current Motor Reference Speed (in S15 format: divide by 32768)
} SRUMRELAYPILOT_STATUS_TYPE;

//---------------------------------------------------------------------------------------------------------------------
// SRUMRelayPilot Parameter interface for Motor Control module
//---------------------------------------------------------------------------------------------------------------------
typedef PACKED struct SRUMRELAYPILOT_CONTROL_PARAM_STRUCT
{
    SRUMRELAYPILOT_PARAM_TYPE Processed;        //!< Set point
    SRUMRELAYPILOT_STATE_TYPE State;            //!< Current module state
    uint8 Tacho_Index;                          //!< LLI Index where tacho signal is
    uint8 ZeroCross_Sequence;                   //!< Sequence counter of last ZeroCross event - either 0 or 180 degrees
    BOOL_TYPE Is_Running;                       //!< Flag : motor is expected to run or not
    BOOL_TYPE Is_Tacho_Signal;                  //!< Flag : tacho signal is updated or not - indicates that drum/motor is moving
    BOOL_TYPE Is_Triac_Firing;                  //!< Flag : Triac firing process is running or not
} SRUMRELAYPILOT_CONTROL_PARAM_TYPE;

//---------------------------------------------------------------------------------------------------------------------
// SRUMRelayPilot Status interface from Motor Control module
//---------------------------------------------------------------------------------------------------------------------
typedef PACKED struct SRUMRELAYPILOT_CONTROL_STATUS_STRUCT
{
    uint16 Firing_Angle_S15;                    //!< Current firing angle (in S15 format: 0..32767)
    sint32 Motor_Speed_S15;                     //!< Current Motor Speed (in S15 format: divide by 32768)
    sint32 Motor_Speed_Ref_S15;                 //!< Current Motor Reference Speed (in S15 format: divide by 32768)
    ON_OFF_TYPE Cw_Relay;                       //!< Clockwise Relay status (ON/OFF)
    ON_OFF_TYPE Ccw_Relay;                      //!< Counterclockwise Relay status (ON/OFF)
    ON_OFF_TYPE Tapped_Field_Relay;             //!< Tapped field Relay status (ON/OFF)
    BOOL_TYPE Is_Running;                       //!< Flag : motor is expected to run or not
    BOOL_TYPE Is_Speed_Reached;                 //!< Flag : Target speed (as in Processed) has been reached or not
    BOOL_TYPE Has_Start_Failed;                 //!< Flag : is Motor stopped due to a Start failure
    uint8 Fault;
} SRUMRELAYPILOT_CONTROL_STATUS_TYPE;


#endif // SRUMRELAYPILOT_DEFS_H_

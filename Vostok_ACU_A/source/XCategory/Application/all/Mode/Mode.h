/**
 *  @file       
 *
 *  @brief      Public interface file for the Mode module.
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/01 13:51:11EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef MODE_H_
#define MODE_H_
#include "C_Extensions.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef enum
{

    MODE_NORMAL                     = 0,
    MODE_PROGRAMMING                = 1,
    MODE_SETTINGFILE_NOT_CONSISTENT = 2, // Setting file can not be use. This mode can not be set from external.
                                         // it only is set by the system itself when it checks the settingfile CRC
    MODE_LOWPOWER                   = 3 // In this mode the code is running a very small task to check if it should

}MODE_TYPE;

typedef enum
{
    SUBMODE_NORMAL_INITIALIZE           = 0x00,
    SUBMODE_NORMAL_SUPERVISOR           = 0x01,  // Supervisor in control
    SUBMODE_NORMAL_ERROR                = 0x02,
    SUBMODE_NORMAL_EXT_CONTROL_ENTERING = 0x03,   // change from PC To EXT
    SUBMODE_NORMAL_EXT_CONTROL_ENGAGED  = 0x04
}SUBMODE_TYPE;



typedef enum
{
    MODE_SPECIAL_HEARTBEAT_PC_CONTROL   =  0xAA//!< MODE_SPECIAL_HEARTBEAT_PC_CONTROL
}MODE_SPECIAL_HEARTBEAT_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void Mode__Initialize(void);
void Mode__Handler(void);

void Mode__SetMode(MODE_TYPE mode);
void Mode__SetModeExtended(MODE_TYPE mode,SUBMODE_TYPE sub_mode);
void Mode__SpecialHeartBeat(MODE_SPECIAL_HEARTBEAT_TYPE heartbeat_code);
MODE_TYPE Mode__GetMode(void);
SUBMODE_TYPE Mode__GetSubMode(void);
BOOL_TYPE Mode__IsPCControlEngaged(void);
BOOL_TYPE Mode__IsProgrammingEngaged(void);

void Mode__SaveRemoteMode(MODE_TYPE mode);
MODE_TYPE Mode__GetRemoteMode(void);

void Mode__SaveRemoteSubMode(SUBMODE_TYPE submode);
SUBMODE_TYPE Mode__GetRemoteSubMode(void);

#endif // MODE_H_



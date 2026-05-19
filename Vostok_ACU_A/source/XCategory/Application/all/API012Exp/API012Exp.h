/**
 *  @file
 *
 *  @brief      Public interface for API012Exp module.
 *
 *  @copyright  Copyright 2015 - 2020. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef API012EXP_H_
#define API012EXP_H_

#if (API012EXP_FEATURE == ENABLED)
#include "SystemConfig.h"
#include "Reveal.h"
#include "HblDefs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
#define API012EXP_NUM           0xC
#define API012EXP_TYPE          0x5
#define API012EXP_VERSION       0x0
#define API012EXP_INSTANCES     1

//! Definition of data for an Encoder GI
typedef struct
{
    HBLHMI_GI_READ_TYPE GI_Read_Type;
    uint8 GI_Position;
    uint8 GI_Data_Size;
    uint8 Current_Enc_Value;
    uint8 Previous_Enc_Value;
    HBL_ENC_DIRECTION_TYPE Direction;
}API12_ENCODER_BUFFER_TYPE;

//! Reveal Priority Queue
#ifndef API0012EXP_USE_PRIORITY_QUEUE
    #define API0012EXP_USE_PRIORITY_QUEUE   DISABLED
#endif

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void    API012Exp__Initialize               (void);

REVEAL_RECIPE_STATUS_TYPE   API012Exp__CommandParser    (REVEAL_MSG_TYPE * buffer);
void                        API012Exp__FeedbackParser   (REVEAL_MSG_TYPE * buffer);
void                        API012Exp__AckParser        (uint8 address, uint8 api, uint8 opcode,
                                                         REVEAL_RECIPE_STATUS_TYPE reasoncode);

void    API012Exp__Handler25ms              (void);
void    API012Exp__ForcePublish             (BOOL_TYPE immediately);
void    API012Exp__UpdateLeds               (uint8 * buffer, uint8 size);
uint8 * API012Exp__GetKeyBuffer             (void);
API12_ENCODER_BUFFER_TYPE * API012Exp__GetEncoderBuffer(HBLHMI_GI_READ_TYPE read_type, uint8 gi_position);
void    API012Exp__PlaySound                (uint8 node, uint8 sound_index, uint8 volume);
void    API012Exp__SetVolume                (uint8 node, uint8 volume);
void    API012Exp__StopAllSound             (uint8 node);
void    API012Exp__PlayAnimation            (uint8 node, uint8 animation_index, uint8 led_group, uint32 pattern, uint8 repeats);
void    API012Exp__StopAnimation            (uint8 node, uint8 led_group, uint32 pattern);

sint32  API012Exp__GetGIByIndex             (uint8 index);
uint8   API012Exp__GetGISequenceIDByIndex   (uint8 index);
uint8   API012Exp__GetNumCommLoads          (void);
uint8 * API012Exp__GetCommLoads             (void);
void    API012Exp__SetExpansionLoad         (uint8 index, uint8 value);
uint8 * API012Exp__GetExpansionLoadsStatus  (void);
HBLHMI_GI_EVENT_TYPE API012Exp__GetLastGiEvent(void);

#if (SYSTEM_ACU_HANDLING == ENABLED)
    void        API012Exp__SetTestCycleEngaged   (BOOL_TYPE engaged);
#endif

#if (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED)
    BOOL_TYPE   API012Exp__IsTestCycleEngaged    (void);
#endif

#endif // API012EXP_FEATURE == ENABLED
#endif // API012EXP_H_

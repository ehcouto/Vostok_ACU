/**
 *  @file
 *
 *  @brief      Private parameters for API012 Expansion module.
 *
 *  @copyright  Copyright 2015-2018 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//---------------------------------------------------------------------------------------------------------------------
#ifndef API012EXP_PRV_H_
#define API012EXP_PRV_H_

//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================
#if (SYSTEM_ACU_HANDLING == ENABLED)
    //! Maximum number of Expansion ACUs for the system.
    //! Allocates RAM for each Expansion ACU GI status buffer on the Main ACU.
    #define API012EXP_MAX_BOARDS 1

    //! Maximum number of expansion GIS for the system.
    //! For expansion boards, set to the total number of Expansion GIs
    //! For the Main ACU, set to the value form the Expansion Board with the largest number of Expansion GIs.
    #define API12_ACU_MAX_GIS         6
#endif

#if ((SYSTEM_ACU_EXPANSION_HANDLING == ENABLED) || (SYSTEM_ACU_HANDLING == ENABLED))
//! Maximum number of expansion loads in the system.
//! Allocates RAM buffers for loads control and load status on the Main ACU or Expansion ACU.
//! For the Expansion ACU set the value to the number of loads on the board.
//! For the Main ACU set the value to the total number of expansion loads on on all Expansion ACUs.
    #define API12_ACU_MAX_LOADS       3
#endif

#if (SYSTEM_HMI_EXPANSION_HANDLING == ENABLED)
    //! Link this macro to the module that will take care of the animations. This is the macro called when it is needed to play an animation.
    #define API012EXP__PLAY_THIS_ANIMATION(animation_index)

    //! Link this macro to the module that will take care of stopping an animation.
    #define API012EXP__STOP_THIS_ANIMATION(animation_index)
#endif

#if ((SYSTEM_HMI_HANDLING == ENABLED) || (SYSTEM_HMI_EXPANSION_HANDLING == ENABLED))
    //! Used on Main HMI to know the number of expansion LEDs to drive.
    //! Define as 1 on Expansion HMI's.
    //! Define as 0 on Main ACU and Expansion ACUs.
    #define API012EXP_NUM_OF_EXTERNAL_LEDS 0

    #define HMI_EXP_GI_ENCODER_BUFFER_SIZE 2
#endif

//! Use this macro if you need a GI value on the application layer. This is called when a broadcasted message was received.
#define API012EXP__PARSE_GI(node_id, payload, size)

#endif // API012EXP_PRV_H_

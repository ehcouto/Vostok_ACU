/**
 *  @file       SRPowerMgr_prm.h
 *
 *  @brief      Public (prm) configuration file for SRPowerMgr module
 *
 *  @defgroup	CLASS_B
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *

    @startuml{LowerPower_Sequence_Diagram.jpg}
        title LowerPower Sequence Diagram

        participant  HMI
        participant  ACU

        == Going into Low Power Mode ==

        ACU -> ACU : Mode = MODE_NORMAL
        HMI -> ACU : API009 Opcode 3

        alt ACU can enter low power mode
            ACU -> ACU : Mode = MODE_LOWERPOWER
            ACU -> HMI : API009 Opcode 8 Payload(0x01)
            alt SRPOWERMGR_LOWPOWER_MODE = 0

            else SRPOWERMGR_LOWPOWER_MODE = 1

            else SRPOWERMGR_LOWPOWER_MODE = 2
                ACU -> ACU : set I/O pin that will disable low voltage circuit, cutting power to the micro
            else SRPOWERMGR_LOWPOWER_MODE = 3
            end
            ACU -> ACU : ACU blackout
        else ACU cannot enter low power mode
            ACU -> HMI : API009 Opcode 8 Payload(0x00)
            ACU -> ACU : Mode = MODE_NORMAL
        end

        == Waking up from Low Power Mode ==
        alt wakeup due to user on HMI
            HMI -> HMI : user presses button
            HMI -> ACU : Wake Up (Tasto signal on schematic) shorted to GND
            ACU -> ACU : hardware reasserts low voltage power circuit
            ACU -> ACU : ACU powers up
            ACU -> ACU : Mode = MODE_NORMAL
        else door opened
            ACU -> ACU : hardware reasserts low voltage power circuit (Wake_up_Door signal on schematic)
            ACU -> ACU : ACU powers up
            ACU -> ACU : Mode = MODE_NORMAL
        end


    @enduml

 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRPOWERMGR_PRM_H
#define SRPOWERMGR_PRM_H


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  - Defines the low power method used by the node to transit from Normal mode to Low power mode
 *    @details-
 *
 *    @param - SRPOWERMGR_FEATURE_DISABLE           =   0
 *             SRPOWERMGR_DEEP_SLEEP_MODE           =   1       requires an external hardware interrupt pin to wake the micro up from "deep sleep"
 *             SRPOWERMGR_AUTO_WAKEUP_MODE          =   2       uses the hibernation circuit, wake up by WIN send break which is holding the WIN BUS high for 300 milliseconds
 *             SRPOWERMGR_LOW_SPEED_MODE            =   3       runs the micro at a lower speed
 */
#define SRPOWERMGR_LOWPOWER_MODE    3

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  - The below function is called during the SRPowerMgr__Initialize() API
 *    @details-
 *
 *    @param -
 */
#define SRPOWERMGR_INITIALIZE()  {/* StandbyManager__Execute();return; */}


//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  - Below macros are used if this micro is responsible for waking up the other nodes in the network
 *    @details- SRPOWERMGR_IS_WAKEUP_CONDITION_MET - Condition to be defined here to differentiate the non intended wakeup and intended wakeup
 *
 *    @param -
 */
#define SRPOWERMGR_WAKEUP_SEQUENCE_GENERATOR                    ENABLED
#define SRPOWERMGR_IS_THIS_IS_INTERNAL_WAKEUP()                 DISABLED
#if (SRPOWERMGR_WAKEUP_SEQUENCE_GENERATOR == ENABLED)
    #define SRPOWERMGR_IS_WAKEUP_CONDITION_MET()                    TRUE
    #define SRPOWERMGR_WAKEUP_RETRY                                 3
#endif


#if (SRPOWERMGR_LOWPOWER_MODE == 1)

    //---------------------------------------------------------------------------------------------------------------------
    /**
     *    @brief  - The below macros must be configured if the SRPOWERMGR_DEEP_SLEEP_MODE is used.
     *              External interrupt is used to wakeup the micro-controller if its in Deep sleep mode
     *    @details- Configure the External Interrupt pin used to wakeup the system
     *
     *    @param -  #define SRPOWERMGR_DEEP_SLEEP_EXTERNAL_INTERRUPT_CHANNEL        EXTINT_CH0
     *              #define SRPOWERMGR_DEEP_SLEEP_EXTERNAL_INTERRUPT_PORT           PORTC
     *              #define SRPOWERMGR_DEEP_SLEEP_EXTERNAL_INTERRUPT_PIN            5
     *              #define SRPOWERMGR_DEEP_SLEEP_EXTERNAL_INTERRUPT_PIN_REMAP      0
     *              #define SRPOWERMGR_DEEP_SLEEP_EXTERNAL_INTERRUPT_TRIGGER_MODE   EXTINT_TRG_RISE
     *
     */
    #define SRPOWERMGR_DEEP_SLEEP_EXTERNAL_INTERRUPT_CHANNEL        EXTINT_CH0
    #define SRPOWERMGR_DEEP_SLEEP_EXTERNAL_INTERRUPT_PORT           PORTC
    #define SRPOWERMGR_DEEP_SLEEP_EXTERNAL_INTERRUPT_PIN            5
    #define SRPOWERMGR_DEEP_SLEEP_EXTERNAL_INTERRUPT_PIN_REMAP      0
    #define SRPOWERMGR_DEEP_SLEEP_EXTERNAL_INTERRUPT_TRIGGER_MODE   EXTINT_TRG_RISE

#elif (SRPOWERMGR_LOWPOWER_MODE == 2)

    //---------------------------------------------------------------------------------------------------------------------
    /**
     *    @brief  - The below macros must be configured if the SRPOWERMGR_AUTO_WAKEUP_MODE is used.
     *              Port-Pin is used to wakeup the micro-controller if its in Deep sleep mode
     */
    #define SRPOWERMGR_AUTO_WAKEUP_HW_PORT          PORTC
    #define SRPOWERMGR_AUTO_WAKEUP_HW_PIN           7
    #define SRPOWERMGR_AUTO_WAKEUP_HW_PIN_DRIVE     0

#elif (SRPOWERMGR_LOWPOWER_MODE == 3)

    #include "LowPower.h"

    //---------------------------------------------------------------------------------------------------------------------
    /**
     *    @brief  - The below macros must be configured if the SRPOWERMGR_LOW_SPEED_MODE is used.
     *              Micro -controller wake-up if the conditions met
     *
     *    @details- Non zero value is considered the micro is ready to transit from low power mode to normal mode
     *
     *    @param -  #define SRPOWERMGR_LOW_SPEED_MODE_IS_READY_TO_WAKEUP()          (!GPIO__PIN_READ(PORTB, 10))
     *
     */
    #define SRPOWERMGR_LOW_SPEED_MODE_IS_READY_TO_WAKEUP()          LowPower__IsWakeupValid()

#endif

#endif // SRPOWERMGR_PRM_H

/**
 *  @file       
 *  @defgroup   CLASS_B
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SRPowerMgr.h" 

#if (LOW_POWER_FEATURE == ENABLED)
#include "Micro.h"
#include "TimerInterrupt.h"
#include "API009System.h"
#include "Win.h"
#include "Reveal.h"
#include "Mode.h"
#include "ExtInterrupt.h"
#include "SRPowerMgr_prv.h"
#include "SRData.h"
#include "SRException.h"
#include "SystemConfig.h"

#if (SYSTEM_ACU_HANDLING == ENABLED)
    #include "NVData.h"
#endif
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#ifndef SRPOWERMGR_LOWPOWER_MODE
    #define SRPOWERMGR_LOWPOWER_MODE    0
#endif

#ifndef SRPOWERMGR_INITIALIZE
    #define SRPOWERMGR_INITIALIZE()
#endif

#ifndef SRPOWERMGR_WAKEUP_SEQUENCE_COMPLETE
    #define SRPOWERMGR_WAKEUP_SEQUENCE_COMPLETE();
#endif

#ifndef SRPOWERMGR__GO_LOW_POWER_RECEIVED
    #define SRPOWERMGR__GO_LOW_POWER_RECEIVED()
#endif

#ifndef MICRO_SRPOWERMGR_FEATURE
    #define MICRO_SRPOWERMGR_FEATURE    DISABLED
#endif

#ifndef SRPOWERMGR_FAILED_TO_WAKEUP_NETWORK
    #define SRPOWERMGR_FAILED_TO_WAKEUP_NETWORK()
#endif

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  - Time the Master node executes the wake up sequence defined for each node in the SRPOWERMGR_WAKEUP_LIST[] list
 *              which could be either by sending WIN Break Signal, Asserting GPIO pin to wake up the other node or
 *              applying project specific method
 *
 *              Modifies this time only if your board requires longer time of assertion
 *    @details- Resolution 10, value can not be 0
 */
#ifndef SRPOWERMGR_EXECUTE_WAKEUP_SEQUENCE_IN_MS
    #define SRPOWERMGR_EXECUTE_WAKEUP_SEQUENCE_IN_MS                150
#elif (SRPOWERMGR_EXECUTE_WAKEUP_SEQUENCE_IN_MS == 0)
    #error: "SRPOWERMGR_EXECUTE_WAKEUP_SEQUENCE_IN_MS shall be non zero value with resolution of 10"
#endif


//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  - Node response/communication/activation time after performing the wake up sequence(WIN Break Signal,
 *              GPIO Pin assertion or project specific  function call) to communicate over the network
 *
 *              This depends on the boot up time of the individual node.
 *
 *              If one of the node failed to response then the module try the wake up sequence for 3 times
 *    @details- Resolution 10, value can not be 0
 */
#ifndef SRPOWERMGR_NODE_RESPONSE_TIMEOUT_AFTER_WAKEUP_SEQUENCE_IN_MS
    #define SRPOWERMGR_NODE_RESPONSE_TIMEOUT_AFTER_WAKEUP_SEQUENCE_IN_MS                1000
#elif (SRPOWERMGR_NODE_RESPONSE_TIMEOUT_AFTER_WAKEUP_SEQUENCE_IN_MS == 0)
    #error: "SRPOWERMGR_NODE_RESPONSE_TIMEOUT_AFTER_WAKEUP_SEQUENCE_IN_MS shall be non zero value with resolution of 10"
#endif


//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  - After coming from low power to the normal power mode the communication line shall be idle within
 *              SRPOWERMGR_COM_CHECK_TIMEOUT_IN_MS time to determine the
 *    @details- Resolution 10, value can not be 0
 */
#ifndef SRPOWERMGR_COM_CHECK_TIMEOUT_IN_MS
    #define SRPOWERMGR_COM_CHECK_TIMEOUT_IN_MS                2000
#elif (SRPOWERMGR_COM_CHECK_TIMEOUT_IN_MS == 0)
    #error: "SRPOWERMGR_COM_CHECK_TIMEOUT_IN_MS shall be non zero value with resolution of 10"
#endif


//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  - Node to send the feedback for the Command message within SRPOWERMGR_FEEDBACK_MESSAGE_TIMEOUT_IN_MS timeout
 *    @details- Resolution 10, value can not be 0
 */
#ifndef SRPOWERMGR_FEEDBACK_MESSAGE_TIMEOUT_IN_MS
    #define SRPOWERMGR_FEEDBACK_MESSAGE_TIMEOUT_IN_MS                2000
#elif (SRPOWERMGR_FEEDBACK_MESSAGE_TIMEOUT_IN_MS == 0)
    #error: "SRPOWERMGR_FEEDBACK_MESSAGE_TIMEOUT_IN_MS shall be non zero value with resolution of 10"
#endif

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief  - Applies only for AUTO WAKEUP scheme node
 *              SRPOWERMGR_AUTO_WAKEUP_HW_PORT_NOP_DELAY number of NOP instructions are executed after deactivating the power.
 *              Expectation is micro to die while executing the delay instructions
 *
 *              These number needs to be adjusted based on the hold up circuitry
 *
 *    @details- Resolution 10, value can not be 0
 */

#ifndef SRPOWERMGR_AUTO_WAKEUP_HW_PORT_NOP_DELAY
    #define SRPOWERMGR_AUTO_WAKEUP_HW_PORT_NOP_DELAY    10000
#endif

#define SRPOWERMGR_INVALID_WAKEUP_NODE              0xFF
#define SRPOWERMGR_MAIN_THREAD_INTERVAL             10

#define SRPOWERMGR_EXECUTE_WAKEUP_SEQUENCE_IN_COUNT                         (SRPOWERMGR_EXECUTE_WAKEUP_SEQUENCE_IN_MS/SRPOWERMGR_MAIN_THREAD_INTERVAL)
#define SRPOWERMGR_NODE_RESPONSE_TIMEOUT_AFTER_WAKEUP_SEQUENCE_IN_COUNT     (SRPOWERMGR_NODE_RESPONSE_TIMEOUT_AFTER_WAKEUP_SEQUENCE_IN_MS/SRPOWERMGR_MAIN_THREAD_INTERVAL)
#define SRPOWERMGR_COM_CHECK_TIMEOUT_IN_COUNT                               (SRPOWERMGR_COM_CHECK_TIMEOUT_IN_MS/SRPOWERMGR_MAIN_THREAD_INTERVAL)
#define SRPOWERMGR_FEEDBACK_MESSAGE_TIMEOUT_COUNT                           (SRPOWERMGR_FEEDBACK_MESSAGE_TIMEOUT_IN_MS/SRPOWERMGR_MAIN_THREAD_INTERVAL)


typedef enum
{
    SRPOWERMGR_STATE_IDLE,
    SRPOWERMGR_STATE_WAKEUP_ITSELF,
    SRPOWERMGR_STATE_CHECK_COM_STATUS,
    SRPOWERMGR_STATE_REQUEST_WAKEUPSOURCCE,
    SRPOWERMGR_STATE_CHECK_WAKEUPSOURCCE,
    SRPOWERMGR_STATE_CHECK_NETWORK_STATUS,
    SRPOWERMGR_STATE_WAKEUP_OTHERS_SEQ1,
    SRPOWERMGR_STATE_WAKEUP_OTHERS_SEQ2,
    SRPOWERMGR_STATE_CHECK_OTHER_NODES,
    SRPOWERMGR_STATE_FAILED_TO_WAKEUP_NETWORK
}SRPOWERMGR_STATE_TYPE;

typedef enum
{
    SRPOWERMGR_WAKEUP_TRIG_EXTERNAL,
    SRPOWERMGR_WAKEUP_TRIG_INTERNAL
}SRPOWERMGR_WAKEUP_TRIG_TYPE;

#if (SRPOWERMGR_LOWPOWER_MODE != 0)
    static SRPOWERMGR_MODE SRPowerMgr_Mode;

    #if (SRPOWERMGR_WAKEUP_SEQUENCE_GENERATOR == ENABLED)
        static uint16 SRPowerMgr_Timeout;
        static SRPOWERMGR_STATE_TYPE SRPowerMgr_State;
        static BOOL_TYPE SRPowerMgr_IsThisWakeUpSource;
        static uint8 SRPowerMgr__ReceivedWakeUpNode;
        static uint8 SRPowerMgr_WakeUpRetry;
        static SRPOWERMGR_WAKEUP_TRIG_TYPE SRPowerMgr_WakeUpTrig;
    #endif
#endif

#if (MICRO_SR_FEATURE_TEST == ENABLED)
    #if (MICRO_SRPOWERMGR_FEATURE == ENABLED)
        extern MICRO_FLOW_TYPE Micro_Flow;
        extern MICRO_FLOW_TYPE N_Micro_Flow;
    #endif
#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module SRPowerMgr and its variables
 *
 */
void SRPowerMgr__Initialize(void)
{
    #if (MICRO_SR_FEATURE_TEST == ENABLED)
        #if (MICRO_SRPOWERMGR_FEATURE == ENABLED)
            if ((Micro_Flow != MICRO_FLOW_SRPOWERMGR_INIT) &&
                (N_Micro_Flow != SRDATA__COMPLEMENT(MICRO_FLOW_SRPOWERMGR_INIT)))
            {
                Micro__Trap();
            }
        #endif
    #endif

    #if (SRPOWERMGR_LOWPOWER_MODE != 0)
        SRPOWERMGR_INITIALIZE();

        #if (SRPOWERMGR_WAKEUP_SEQUENCE_GENERATOR == ENABLED)
            SRPowerMgr_Timeout = 0;
            SRPowerMgr_IsThisWakeUpSource = FALSE;
            SRPowerMgr__ReceivedWakeUpNode  = SRPOWERMGR_INVALID_WAKEUP_NODE;
            SRPowerMgr_WakeUpRetry = 0;

            if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
            {
                SRPowerMgr_State      = SRPOWERMGR_STATE_IDLE;
                SRPowerMgr_WakeUpTrig = SRPOWERMGR_WAKEUP_TRIG_EXTERNAL;
            }
        #endif

        #if (SRPOWERMGR_LOWPOWER_MODE == 3)
            MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
        #endif

        #if (SRPOWERMGR_LOWPOWER_MODE == 1)
            if (Mode__GetMode() == MODE_LOWPOWER)
            {
                //SRPOWERMGR_DEEP_SLEEP_MODE
                Micro__DisableWarmResetAction(MICRO_WARM_RESET_ACTION_ENTER_LOWPOWER);
                DISABLE_INTERRUPTS();
                Gpio__Initialize();
                ExtInterrupt__Initialize();
                ExtInterrupt__Config(SRPOWERMGR_DEEP_SLEEP_EXTERNAL_INTERRUPT_CHANNEL,
                                     SRPOWERMGR_DEEP_SLEEP_EXTERNAL_INTERRUPT_PORT,
                                     SRPOWERMGR_DEEP_SLEEP_EXTERNAL_INTERRUPT_PIN,
                                     SRPOWERMGR_DEEP_SLEEP_EXTERNAL_INTERRUPT_PIN_REMAP,
                                     SRPOWERMGR_DEEP_SLEEP_EXTERNAL_INTERRUPT_TRIGGER_MODE);
                ENABLE_INTERRUPTS();
                DEEP_SLEEP_MODE();
                DISABLE_INTERRUPTS();
            }
        #elif (SRPOWERMGR_LOWPOWER_MODE == 2)
            if (Mode__GetMode() == MODE_LOWPOWER)
            {
                #if (SYSTEM_ACU_HANDLING == ENABLED)
                    //backup the NVData
                    NVData__Store();
                #endif

                //retry for 3 times
                for (uint8 count = 0; count < 3; count++)
                {
                    //SRPOWERMGR_AUTO_WAKEUP_MODE
                    //this condition because if the micro is reset and running due to LVD
                    Gpio__Initialize();
                    Gpio__PinWrite(SRPOWERMGR_AUTO_WAKEUP_HW_PORT, SRPOWERMGR_AUTO_WAKEUP_HW_PIN, (BOOL_TYPE)(~SRPOWERMGR_AUTO_WAKEUP_HW_PIN_DRIVE & 0x01));
                    Gpio__PinConfig(SRPOWERMGR_AUTO_WAKEUP_HW_PORT, SRPOWERMGR_AUTO_WAKEUP_HW_PIN, OUTPUT_PUSHPULL);

                    //add a delay to die the power
                    Micro__DelayNumNops(SRPOWERMGR_AUTO_WAKEUP_HW_PORT_NOP_DELAY);
                }

                //cold reset if it fails to put the micro in sleep mode
                Micro__ForceReset(MICRO_RESET_MODE_COLD);
            }
            else
            {
                //SRPOWERMGR_AUTO_WAKEUP_MODE
                Micro__DisableWarmResetAction(MICRO_WARM_RESET_ACTION_ENTER_LOWPOWER);

                Gpio__Initialize();
                Gpio__PinWrite(SRPOWERMGR_AUTO_WAKEUP_HW_PORT, SRPOWERMGR_AUTO_WAKEUP_HW_PIN, (BOOL_TYPE)(SRPOWERMGR_AUTO_WAKEUP_HW_PIN_DRIVE & 0x01));
                Gpio__PinConfig(SRPOWERMGR_AUTO_WAKEUP_HW_PORT, SRPOWERMGR_AUTO_WAKEUP_HW_PIN, OUTPUT_PUSHPULL);

                SRPowerMgr_Mode = SRPOWERMGR_NORMAL_SPEED_MODE;
            }

        #elif (SRPOWERMGR_LOWPOWER_MODE == 3)
            if (Mode__GetMode() == MODE_LOWPOWER)
            {
                //SRPOWERMGR_LOW_SPEED_MODE
                MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
                Micro__SetLowSpeedMode();                     // This change the speed of the micro controller to the LowSpeed mode
                TimerInterrupt__SetLongInterval();
                MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();

                SRPowerMgr_Mode = SRPOWERMGR_LOW_SPEED_MODE;
            }
            else
            {
                #if (SRPOWERMGR_WAKEUP_SEQUENCE_GENERATOR == ENABLED)
                    if (SRPowerMgr_State != SRPOWERMGR_STATE_WAKEUP_ITSELF)
                    {
                        SRPowerMgr_State = SRPOWERMGR_STATE_CHECK_NETWORK_STATUS;
                    }
                #endif
                SRPowerMgr_Mode = SRPOWERMGR_NORMAL_SPEED_MODE;
            }
        #else
            SRPowerMgr_Mode = SRPOWERMGR_NORMAL_SPEED_MODE;
        #endif
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param mode
 * @return
 */


BOOL_TYPE SRPowerMgr__GoLowPowerMode(void)
{
    BOOL_TYPE result;

    result = TRUE;

    SRPOWERMGR__GO_LOW_POWER_RECEIVED();

    #if (SRPOWERMGR_LOWPOWER_MODE == 1)
        //SRPOWERMGR_DEEP_SLEEP_MODE
        SRPowerMgr_Mode = SRPOWERMGR_DEEP_SLEEP_MODE;
        Micro__EnableWarmResetAction(MICRO_WARM_RESET_ACTION_ENTER_LOWPOWER);
        Micro__GoSleep();
    #elif (SRPOWERMGR_LOWPOWER_MODE == 2)
        //SRPOWERMGR_AUTO_WAKEUP_MODE
        SRPowerMgr_Mode = SRPOWERMGR_AUTO_WAKEUP_MODE;
        Micro__EnableWarmResetAction(MICRO_WARM_RESET_ACTION_ENTER_LOWPOWER);
        Mode__SetMode(MODE_LOWPOWER);
    #elif (SRPOWERMGR_LOWPOWER_MODE == 3)
        SRPowerMgr_Mode = SRPOWERMGR_LOW_SPEED_MODE;
        Micro__EnableWarmResetAction(MICRO_WARM_RESET_ACTION_ENTER_LOWPOWER);
        Mode__SetMode(MODE_LOWPOWER);
    #else
        result = FALSE;
    #endif

    return result;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module SRPowerMgr and its variables
 *
 */
void SRPowerMgr__GoNormalMode(void)
{
    #if (SRPOWERMGR_LOWPOWER_MODE != 0)

        SRPowerMgr_Mode = SRPOWERMGR_NORMAL_SPEED_MODE;
        Micro__DisableWarmResetAction(MICRO_WARM_RESET_ACTION_ENTER_LOWPOWER);

        #if (SRPOWERMGR_IS_THIS_IS_INTERNAL_WAKEUP() == ENABLED)
            SRPowerMgr_WakeUpTrig = SRPOWERMGR_WAKEUP_TRIG_INTERNAL;
        #endif

        #if (SRPOWERMGR_WAKEUP_SEQUENCE_GENERATOR == ENABLED)
            SRPowerMgr_State = SRPOWERMGR_STATE_WAKEUP_ITSELF;
        #else
            Mode__SetMode(MODE_NORMAL);
        #endif
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module SRPowerMgr and its variables
 * @return
 */
SRPOWERMGR_MODE SRPowerMgr__GetMode(void)
{
    #if (SRPOWERMGR_LOWPOWER_MODE != 0)
        return SRPowerMgr_Mode;
    #else
        return (SRPOWERMGR_NORMAL_SPEED_MODE);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module SRPowerMgr and its variables
 *
 * @return
 */
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This API is called to check if this node was the source of the wake-up signal
 * @details This function should return FALSE if the wake-up wasn't generated in this board and TRUE is it was generated in this board
 */

BOOL_TYPE SRPowerMgr__IsThisNodeWakeUpSource(void)
{
    #if ((SRPOWERMGR_WAKEUP_SEQUENCE_GENERATOR == ENABLED) && (SRPOWERMGR_LOWPOWER_MODE != 0))
        return (SRPowerMgr_IsThisWakeUpSource);
    #else
            return(FALSE);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This function is called once another node publish a message letting the other nodes know that it generated the wake-up
 * @param node the parameter is the source of the wake-up signal
 */
void SRPowerMgr__ReceivedWakeUpNodeID(uint8 node)
{
    #if ((SRPOWERMGR_WAKEUP_SEQUENCE_GENERATOR == ENABLED) && (SRPOWERMGR_LOWPOWER_MODE != 0))
        SRPowerMgr__ReceivedWakeUpNode = node;
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module SRPowerMgr and its variables
 */
void SRPowerMgr__Handler(void)
{
    #if (SRPOWERMGR_LOWPOWER_MODE != 0)

        uint16 node_status;
        uint8 aux;
        BOOL_TYPE network_status;

        if (Mode__GetMode() == MODE_LOWPOWER)
        {
            #if (SRPOWERMGR_LOWPOWER_MODE == 1)
                //SRPOWERMGR_DEEP_SLEEP_MODE
            #elif (SRPOWERMGR_LOWPOWER_MODE == 2)
                //SRPOWERMGR_AUTO_WAKEUP_MODE
            #elif (SRPOWERMGR_LOWPOWER_MODE == 3)
                //SRPOWERMGR_LOW_SPEED_MODE
                if (SRPOWERMGR_LOW_SPEED_MODE_IS_READY_TO_WAKEUP() != 0)
                {
                    SRPowerMgr__GoNormalMode();
                }
            #else
            #endif
        }

        #if (SRPOWERMGR_WAKEUP_SEQUENCE_GENERATOR == ENABLED)
            SRPowerMgr_Timeout++;

            switch(SRPowerMgr_State)
            {
                case SRPOWERMGR_STATE_IDLE:
                    if (Mode__GetMode() == MODE_NORMAL)
                    {
                        node_status = Reveal__GetNodestatus();
                        network_status = TRUE;

                        for (aux = 0; aux < sizeof(SRPOWERMGR_WAKEUP_LIST)/sizeof(SRPOWERMGR_WAKEUP_TYPE); aux++)
                        {
                            if(BIT_TEST(node_status, SRPOWERMGR_WAKEUP_LIST[aux].WakeUp_Node) == 0)
                            {
                                network_status = FALSE;
                                break;
                            }
                        }

                        if (network_status == FALSE)
                        {
                            SRPowerMgr_Timeout = 0;
                            SRPowerMgr_State = SRPOWERMGR_STATE_CHECK_NETWORK_STATUS;
                        }
                    }
                    break;

                case SRPOWERMGR_STATE_WAKEUP_ITSELF:
                    if (Mode__GetMode() == MODE_LOWPOWER)
                    {
                        Mode__SetMode(MODE_NORMAL);
                    }
                    else
                    {
                        SRPowerMgr_State = SRPOWERMGR_STATE_CHECK_COM_STATUS;
                    }

                    SRPowerMgr_Timeout = 0;
                    break;

                case SRPOWERMGR_STATE_CHECK_COM_STATUS:
                    if (Win__GetBusStatus(WIN_BUS_STATUS_RX_PIN) == TRUE)
                    {
                        if (SRPowerMgr_WakeUpTrig == SRPOWERMGR_WAKEUP_TRIG_INTERNAL)
                        {
                            SRPowerMgr_Timeout = SRPOWERMGR_NODE_RESPONSE_TIMEOUT_AFTER_WAKEUP_SEQUENCE_IN_COUNT;
                            SRPowerMgr_State = SRPOWERMGR_STATE_CHECK_NETWORK_STATUS;
                        }
                        else
                        {
                            SRPowerMgr_State = SRPOWERMGR_STATE_REQUEST_WAKEUPSOURCCE;
                        }
                    }
                    else
                    {
                        if (SRPowerMgr_Timeout > SRPOWERMGR_COM_CHECK_TIMEOUT_IN_COUNT)
                        {
                            SRPowerMgr_State = SRPOWERMGR_STATE_IDLE;
                        }
                    }
                    break;

                case SRPOWERMGR_STATE_REQUEST_WAKEUPSOURCCE:
                    API009System__RequestWakeUpSource();
                    SRPowerMgr_Timeout = 0;
                    SRPowerMgr__ReceivedWakeUpNode  = SRPOWERMGR_INVALID_WAKEUP_NODE;
                    SRPowerMgr_State = SRPOWERMGR_STATE_CHECK_WAKEUPSOURCCE;
                    break;

                case SRPOWERMGR_STATE_CHECK_WAKEUPSOURCCE:
                    if (SRPowerMgr_Timeout < SRPOWERMGR_FEEDBACK_MESSAGE_TIMEOUT_COUNT)
                    {
                        if (SRPowerMgr__ReceivedWakeUpNode  != SRPOWERMGR_INVALID_WAKEUP_NODE)
                        {
                            //found the wakeup source
                            SRPowerMgr_State = SRPOWERMGR_STATE_IDLE;
                        }
                    }
                    else
                    {
                        //I am the wakeup source
                        if (SRPOWERMGR_IS_WAKEUP_CONDITION_MET())
                        {
                            SRPowerMgr_IsThisWakeUpSource = TRUE;
                            SRPowerMgr_State = SRPOWERMGR_STATE_CHECK_NETWORK_STATUS;
                        }
                        else
                        {
                            //then its an error
                            SRPowerMgr_State = SRPOWERMGR_STATE_IDLE;
                            SRPowerMgr__GoLowPowerMode();
                        }
                    }

                    SRPowerMgr_WakeUpRetry = 0;
                    break;

                case SRPOWERMGR_STATE_CHECK_NETWORK_STATUS:
                    //This time is to wakeup the other nodes after the Wakeup sequence
                    node_status = Reveal__GetNodestatus();
                    network_status = TRUE;

                    for (aux = 0; aux < sizeof(SRPOWERMGR_WAKEUP_LIST)/sizeof(SRPOWERMGR_WAKEUP_TYPE); aux++)
                    {
                        if(BIT_TEST(node_status, SRPOWERMGR_WAKEUP_LIST[aux].WakeUp_Node) == 0)
                        {
                            network_status = FALSE;
                            break;
                        }
                    }

                    if (network_status == TRUE)
                    {
                        //All are alive
                        SRPOWERMGR_WAKEUP_SEQUENCE_COMPLETE();
                        SRPowerMgr_State = SRPOWERMGR_STATE_IDLE;
                    }
                    else if (SRPowerMgr_Timeout > SRPOWERMGR_NODE_RESPONSE_TIMEOUT_AFTER_WAKEUP_SEQUENCE_IN_COUNT)
                    {
                        SRPowerMgr_State = SRPOWERMGR_STATE_WAKEUP_OTHERS_SEQ1;
                    }

                    break;

                case SRPOWERMGR_STATE_WAKEUP_OTHERS_SEQ1:
                    for (aux = 0; aux < sizeof(SRPOWERMGR_WAKEUP_LIST)/sizeof(SRPOWERMGR_WAKEUP_TYPE); aux++)
                    {
                        if (Win__GetBusStatus(WIN_BUS_STATUS_BREAK_OUT) == FALSE)
                        {
                            Win__SendBreak();
                        }

                        if (SRPOWERMGR_WAKEUP_LIST[aux].WakeUp_Port != PORT_NULL)
                        {
                            //De assert
                            Gpio__PinWrite(SRPOWERMGR_WAKEUP_LIST[aux].WakeUp_Port, SRPOWERMGR_WAKEUP_LIST[aux].WakeUp_Pin, (BOOL_TYPE)((SRPOWERMGR_WAKEUP_LIST[aux].WakeUp_Value ^ 0x01) & 0x01));

                            Gpio__PinConfig(SRPOWERMGR_WAKEUP_LIST[aux].WakeUp_Port, SRPOWERMGR_WAKEUP_LIST[aux].WakeUp_Pin, OUTPUT_PUSHPULL);

                            //assert
                            Gpio__PinWrite(SRPOWERMGR_WAKEUP_LIST[aux].WakeUp_Port, SRPOWERMGR_WAKEUP_LIST[aux].WakeUp_Pin, SRPOWERMGR_WAKEUP_LIST[aux].WakeUp_Value);
                        }

                        if (SRPOWERMGR_WAKEUP_LIST[aux].WakeUp_Func != SRPOWER_ACTIVATION_FUNC_NULL)
                        {
                            SRPOWERMGR_WAKEUP_LIST[aux].WakeUp_Func();
                        }
                    }

                    SRPowerMgr_Timeout  = 0;
                    SRPowerMgr_State = SRPOWERMGR_STATE_WAKEUP_OTHERS_SEQ2;

                    break;

                case SRPOWERMGR_STATE_WAKEUP_OTHERS_SEQ2:
                    if (SRPowerMgr_Timeout > SRPOWERMGR_EXECUTE_WAKEUP_SEQUENCE_IN_COUNT)
                    {
                        Win__StopBreak();

                        for (aux = 0; aux < sizeof(SRPOWERMGR_WAKEUP_LIST)/sizeof(SRPOWERMGR_WAKEUP_TYPE); aux++)
                        {
                            if (SRPOWERMGR_WAKEUP_LIST[aux].WakeUp_Port != PORT_NULL)
                            {
                                //De assert
                                Gpio__PinWrite(SRPOWERMGR_WAKEUP_LIST[aux].WakeUp_Port, SRPOWERMGR_WAKEUP_LIST[aux].WakeUp_Pin, (BOOL_TYPE)((SRPOWERMGR_WAKEUP_LIST[aux].WakeUp_Value ^ 0x01) & 0x01));
                                Gpio__PinConfig(SRPOWERMGR_WAKEUP_LIST[aux].WakeUp_Port, SRPOWERMGR_WAKEUP_LIST[aux].WakeUp_Pin, OUTPUT_PUSHPULL);
                            }
                        }

                        SRPowerMgr_WakeUpRetry++;
                        if (SRPowerMgr_WakeUpRetry >= SRPOWERMGR_WAKEUP_RETRY)
                        {
                            //Failed to wakeup the network
                            SRPOWERMGR_FAILED_TO_WAKEUP_NETWORK();

                            SRPowerMgr_State = SRPOWERMGR_STATE_FAILED_TO_WAKEUP_NETWORK;
                        }
                        else
                        {
                            SRPowerMgr_Timeout = 0;
                            SRPowerMgr_State = SRPOWERMGR_STATE_CHECK_NETWORK_STATUS;
                        }
                    }
                    break;

                case SRPOWERMGR_STATE_FAILED_TO_WAKEUP_NETWORK:
                    break;

                default:
                    SRPowerMgr_State = SRPOWERMGR_STATE_IDLE;
                    break;
            }
        #endif
    #endif
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
#endif  //LOW_POWER_FEATURE

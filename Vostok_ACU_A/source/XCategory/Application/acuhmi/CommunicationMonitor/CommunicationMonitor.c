/**
 *  @file
 *
 *  @brief      Source file for the Communication Monitor module.
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *

    @startuml{CommunicationMonitor State Machine.jpg}
        title CommunicationMonitor State Machine

        'use state = CommunicationMonitor_Data.State[CommunicationMonitor_Data.Data_Index]

        [*] --> FAULT_MONITORING

        FAULT_MONITORING : do /\t CheckRevealNodeStatus() \n RemoveFault()
        FAULT_MONITORING --> FAULT_RECOVERING  : evFaultDetected
        FAULT_MONITORING --> DISABLED  : evMonitoringDisabled

        FAULT_RECOVERING : do /\t CheckRevealNodeStatus() \n ReportFault()
        FAULT_RECOVERING --> FAULT_MONITORING  : evFaultRecovered
        FAULT_RECOVERING --> DISABLED  : evMonitoringDisabled

        DISABLED --> FAULT_MONITORING      : evMonitoringEnabled


    @enduml

 *  $Header: $
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "CommunicationMonitor.h"
#include <string.h>

#if (COMMUNICATION_MONITOR_FEATURE == ENABLED)
#include "Reveal.h"

#if(FAULT_MANAGEMENT_FEATURE == ENABLED)
    #include "Fault.h"
#endif

#define MAXIMUM_NUMBER_OF_NODES_TO_MONITOR                  (15)

#define PERIODIC_CHECK_AT_EVERY_FIVE_SECONDS                (5000/100)
#define CHECK_COMMUNICATION_AFTER_15S_DELAY                 (15000/100)

#define INVALID_NODE_ID                                     (0xFF)

#endif // #if (COMMUNICATION_MONITOR_FEATURE == ENABLED)


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#if (COMMUNICATION_MONITOR_FEATURE == ENABLED)


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Communication Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Communication Monitor state machine.
typedef enum COMMUNICATION_MONITOR_STATE_ENUM
{
    STATE_COMMUNICATION_MONITOR_NONE             = 0,

    STATE_COMMUNICATION_MONITOR_FAULT_MONITORING = 1, //!< (initial_state)
    STATE_COMMUNICATION_MONITOR_FAULT_RECOVERING = 2,
    STATE_COMMUNICATION_MONITOR_DISABLED         = 3,

    STATE_COMMUNICATION_MONITOR_END              = 4
} COMMUNICATION_MONITOR_STATE_TYPE;

//! The transition variable for the Communication Monitor state machine.
//! Flag used by the state machine to track when a transition occurs in the middle of a state's
//! Entry() or Do() action that contains more than one function.
static BOOL_TYPE Communication_Monitor_Transitioned;

// -- Private Function Prototypes used by the state machine for customization functions --
static void CheckRevealNodeStatus(void);
static void RemoveFault(void);
static void ReportFault(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void CommunicationMonitorExecute(void);
static void CommunicationMonitorInitialize(void);
static void EventFaultDetected(void);
static void EventFaultRecovered(void);
static void EventMonitoringDisabled(void);
static void EventMonitoringEnabled(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.
typedef PACKED struct
{
    uint8 Node;
    uint8 Fault_Id;     //! Same structure is used for HMI side and HMI Does not know about the FAULT_ID scheme so its intentionally declared as uint8
} COMMUNICATION_MONITOR_NODES_TO_MONITOR_TYPE;

#if(FAULT_MANAGEMENT_FEATURE == ENABLED)
    static const COMMUNICATION_MONITOR_NODES_TO_MONITOR_TYPE COMMUNICATION_MONITOR_NODE_LIST[] =
    {
        COMMUNICATION_MONITOR_NODES_TO_MONITOR
    };
#else

    #ifndef COMMUNICATION_MONITOR_FAULT_ID_COMMUNICATION_ACU
        #error "Define this macro in the SystemConfig_prm.h file. This macro shall have the FAULT_ID used for in case of ACU communication error";
    #endif

    static const COMMUNICATION_MONITOR_NODES_TO_MONITOR_TYPE COMMUNICATION_MONITOR_NODE_LIST[] =
    {
        { WIN_ACU_ADDRESS        ,COMMUNICATION_MONITOR_FAULT_ID_COMMUNICATION_ACU  }
    };

    BOOL_TYPE CommunicationMonitor_IsACUNodeInFault;
#endif

#define NUMBER_OF_NODES_TO_MONITOR  (ELEMENTS_IN_ARRAY(COMMUNICATION_MONITOR_NODE_LIST))

static uint16 CommunicationMonitor_HasNodeInFault;

//! module static variables
static PACKED struct
{
    uint8                            Data_Index;
    COMMUNICATION_MONITOR_STATE_TYPE State[NUMBER_OF_NODES_TO_MONITOR];
} CommunicationMonitor_Data;

static uint8 CommunicationMonitor_Timer_Tick;

#endif // #if (COMMUNICATION_MONITOR_FEATURE == ENABLED)

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module ACUCommFault and its variables
 *
 */
void CommunicationMonitor__Initialize(void)
{
    #if (COMMUNICATION_MONITOR_FEATURE == ENABLED)
        memset(&CommunicationMonitor_Data, 0, sizeof(CommunicationMonitor_Data));

        CommunicationMonitor_Timer_Tick = CHECK_COMMUNICATION_AFTER_15S_DELAY;

        #if(FAULT_MANAGEMENT_FEATURE == DISABLED)
            CommunicationMonitor_IsACUNodeInFault = FALSE;
        #endif

        for ( CommunicationMonitor_Data.Data_Index = 0; CommunicationMonitor_Data.Data_Index < NUMBER_OF_NODES_TO_MONITOR; CommunicationMonitor_Data.Data_Index++ )
        {
            CommunicationMonitorInitialize();
        }

        CommunicationMonitor_HasNodeInFault = 0;

    #endif // #if (COMMUNICATION_MONITOR_FEATURE == ENABLED)
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This API shall be called periodically every 100ms
 *
 */
void CommunicationMonitor__Execute(void)
{
    #if (COMMUNICATION_MONITOR_FEATURE == ENABLED)

        CommunicationMonitor_Timer_Tick--;

        if ( CommunicationMonitor_Timer_Tick == 0)
        {
            CommunicationMonitor_Timer_Tick = PERIODIC_CHECK_AT_EVERY_FIVE_SECONDS;

            for ( CommunicationMonitor_Data.Data_Index = 0; CommunicationMonitor_Data.Data_Index < NUMBER_OF_NODES_TO_MONITOR; CommunicationMonitor_Data.Data_Index++ )
            {
                CommunicationMonitorExecute();
            }
        }

    #endif // #if (COMMUNICATION_MONITOR_FEATURE == ENABLED)
}

#if(FAULT_MANAGEMENT_FEATURE == DISABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This API returns weather the ACU is alive in the communication or not
 *
 *        Shall be used only at the HMI side only
 * @return
 */
BOOL_TYPE CommunicationMonitor__IsACUNodeInFault(void)
{
    #if (COMMUNICATION_MONITOR_FEATURE == ENABLED)
        return CommunicationMonitor_IsACUNodeInFault;
    #else
        return FALSE;
    #endif
}
#endif


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Returns if CommunicationMonitor has found any faulty node
 * @details Can be used by any node (ACU, HMI, etc)
 * @return
 *  @retval TRUE (1) One of the node in the network is in Fault
 *  @retval FALSE(0) None of the nodes are in fault
 */
BOOL_TYPE CommunicationMonitor__HasNodeInFault(void)
{
    #if (COMMUNICATION_MONITOR_FEATURE == ENABLED)
        return ((CommunicationMonitor_HasNodeInFault != 0)?TRUE:FALSE);
    #else
        return FALSE;
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Enable the monitoring of a node.
 *
 *  @param  node    the node to enable
 */
void CommunicationMonitor__Enable(uint8 node)
{
    #if (COMMUNICATION_MONITOR_FEATURE == ENABLED)
    uint8 temp_data_index;

    for ( uint8 data_index = 0; data_index < NUMBER_OF_NODES_TO_MONITOR; data_index++ )
    {
        if ( node == COMMUNICATION_MONITOR_NODE_LIST[data_index].Node )
        {
            if (CommunicationMonitor_Data.State[data_index] == STATE_COMMUNICATION_MONITOR_DISABLED)
            {
                //Save the current node index
                temp_data_index = CommunicationMonitor_Data.Data_Index;

                // Assign the node to be enabled
                CommunicationMonitor_Data.Data_Index = data_index;

                EventMonitoringEnabled();

                // restore the current node index
                CommunicationMonitor_Data.Data_Index = temp_data_index;
            }
        }
    }
    #endif // #if (COMMUNICATION_MONITOR_FEATURE == ENABLED)
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Disable the monitoring of a node.
 *
 *  @param  node    the node to disable
 */
void CommunicationMonitor__Disable(uint8 node)
{
    #if (COMMUNICATION_MONITOR_FEATURE == ENABLED)
    for ( uint8 data_index = 0; data_index < NUMBER_OF_NODES_TO_MONITOR; data_index++ )
    {
        if ( node == COMMUNICATION_MONITOR_NODE_LIST[data_index].Node )
        {
            //Save the current node index
            uint8 temp_data_index = CommunicationMonitor_Data.Data_Index;

            // Assign the node to be disabled
            CommunicationMonitor_Data.Data_Index = data_index;

            EventMonitoringDisabled();

            // restore the current node index
            CommunicationMonitor_Data.Data_Index = temp_data_index;
        }
    }
    #endif // #if (COMMUNICATION_MONITOR_FEATURE == ENABLED)
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
#if (COMMUNICATION_MONITOR_FEATURE == ENABLED)

//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Report the loss of communication Fault
 */
static void ReportFault(void)
{
    #if(FAULT_MANAGEMENT_FEATURE == ENABLED)
        Fault__ConditionDetected((FAULT_ID_TYPE)COMMUNICATION_MONITOR_NODE_LIST[CommunicationMonitor_Data.Data_Index].Fault_Id);
    #else
        CommunicationMonitor_IsACUNodeInFault = TRUE;
    #endif

    BIT_SET(CommunicationMonitor_HasNodeInFault, COMMUNICATION_MONITOR_NODE_LIST[CommunicationMonitor_Data.Data_Index].Node);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void CheckRevealNodeStatus(void)
{
    uint16 reveal_node_status = Reveal__GetNodestatus();

    if (BIT_TEST(reveal_node_status, COMMUNICATION_MONITOR_NODE_LIST[CommunicationMonitor_Data.Data_Index].Node) == 0)
    {
        EventFaultDetected();
    }
    else
    {
        EventFaultRecovered();
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief - This function removes the fault and also clears the flag from the CommunicationMonitor_HasNodeInFault
 *
 */
static void RemoveFault(void)
{
    #if(FAULT_MANAGEMENT_FEATURE == ENABLED)
        Fault__ConditionRemoved((FAULT_ID_TYPE)COMMUNICATION_MONITOR_NODE_LIST[CommunicationMonitor_Data.Data_Index].Fault_Id);
    #else
        CommunicationMonitor_IsACUNodeInFault = FALSE;
    #endif

    BIT_CLR(CommunicationMonitor_HasNodeInFault, COMMUNICATION_MONITOR_NODE_LIST[CommunicationMonitor_Data.Data_Index].Node);
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Communication Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Communication Monitor state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void CommunicationMonitorExecute(void)
{
    Communication_Monitor_Transitioned = FALSE;
    switch(CommunicationMonitor_Data.State[CommunicationMonitor_Data.Data_Index])
    {
        case STATE_COMMUNICATION_MONITOR_FAULT_MONITORING:
            CheckRevealNodeStatus();                // Do function.
            if (Communication_Monitor_Transitioned == FALSE)
            {
                RemoveFault();                      // Do function.
            }
            break;

        case STATE_COMMUNICATION_MONITOR_FAULT_RECOVERING:
            CheckRevealNodeStatus();                // Do function.
            if (Communication_Monitor_Transitioned == FALSE)
            {
                ReportFault();                      // Do function.
            }
            break;

        default:
            // There is nothing to do for all other values of CommunicationMonitor_Data.State[CommunicationMonitor_Data.Data_Index].
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Communication Monitor state machine.
 * This function resets the state machine to the initial state.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void CommunicationMonitorInitialize(void)
{
    CommunicationMonitor_Data.State[CommunicationMonitor_Data.Data_Index] = STATE_COMMUNICATION_MONITOR_FAULT_MONITORING;
    Communication_Monitor_Transitioned = TRUE;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Fault Detected Event for the Communication Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventFaultDetected(void)
{
    switch (CommunicationMonitor_Data.State[CommunicationMonitor_Data.Data_Index])
    {
        case STATE_COMMUNICATION_MONITOR_FAULT_MONITORING:
            CommunicationMonitor_Data.State[CommunicationMonitor_Data.Data_Index] = STATE_COMMUNICATION_MONITOR_FAULT_RECOVERING;
            Communication_Monitor_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of CommunicationMonitor_Data.State[CommunicationMonitor_Data.Data_Index].
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Fault Recovered Event for the Communication Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventFaultRecovered(void)
{
    switch (CommunicationMonitor_Data.State[CommunicationMonitor_Data.Data_Index])
    {
        case STATE_COMMUNICATION_MONITOR_FAULT_RECOVERING:
            CommunicationMonitor_Data.State[CommunicationMonitor_Data.Data_Index] = STATE_COMMUNICATION_MONITOR_FAULT_MONITORING;
            Communication_Monitor_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of CommunicationMonitor_Data.State[CommunicationMonitor_Data.Data_Index].
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Monitoring Disabled Event for the Communication Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventMonitoringDisabled(void)
{
    switch (CommunicationMonitor_Data.State[CommunicationMonitor_Data.Data_Index])
    {
        case STATE_COMMUNICATION_MONITOR_FAULT_MONITORING:
        case STATE_COMMUNICATION_MONITOR_FAULT_RECOVERING:
            CommunicationMonitor_Data.State[CommunicationMonitor_Data.Data_Index] = STATE_COMMUNICATION_MONITOR_DISABLED;
            Communication_Monitor_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of CommunicationMonitor_Data.State[CommunicationMonitor_Data.Data_Index].
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Monitoring Enabled Event for the Communication Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventMonitoringEnabled(void)
{
    switch (CommunicationMonitor_Data.State[CommunicationMonitor_Data.Data_Index])
    {
        case STATE_COMMUNICATION_MONITOR_DISABLED:
            CommunicationMonitor_Data.State[CommunicationMonitor_Data.Data_Index] = STATE_COMMUNICATION_MONITOR_FAULT_MONITORING;
            Communication_Monitor_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of CommunicationMonitor_Data.State[CommunicationMonitor_Data.Data_Index].
            break;
    }
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


#endif


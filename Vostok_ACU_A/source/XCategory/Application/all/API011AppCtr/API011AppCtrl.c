/**
 *  @file       
 *
 *  @brief      API011 Implementation
 *
 *  @details    This API has 2 main functions:
 *                  - Implements the main communication between ACU and HMI (Regulations, Status)
 *                  - Implements get/set for the appliance Mode
 *              There is also support to get the number of cycles, the Status variables configuration
 *              and the position of a function in the function configuration.
 *
 @startuml{API011_Sequence_Diagram.png}
 hide footbox
 autonumber
 title API011 Sequence Diagram
 box "HMI"
 participant Application as app
 participant API011_HMI as hmi
 end box

 box "ACU"
 participant API011_ACU as acu
 end box

 alt Normal operation, no user activity or application changing regulations.
     loop 10s
     hmi -> acu : request regulations (API011 opcode: 4)
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Regulations_Requests = 1</b>
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Status_Requests = 0</b>
     acu -> hmi : publish regulations (API011 opcode: 4, feedback)
     ... 5 secounds later ...
     hmi -> acu : request status (API011 opcode: 3)
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Status_Requests = 1</b>
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Regulations_Requests = 0</b>
     acu -> hmi : publish status (API011 opcode: 3, feedback)
     end


 else ACU detect state change

     hmi -> acu : request regulations (API011 opcode: 4)
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Regulations_Requests++</b>
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Status_Requests = 0</b>
     acu -> hmi : publish regulations (API011 opcode: 4, feedback)
     acu -> hmi : status change detected on ACU, publish status (API011 opcode: 3, feedback)
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Regulations_Requests = 0 </b>
     ... 5 secounds later ...
     hmi -> acu : request regulations (API011 opcode: 4)
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Regulations_Requests++</b>
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Status_Requests = 0</b>
     acu -> hmi : publish regulations (API011 opcode: 4, feedback)
     acu -> hmi : status change detected on ACU, publish status (API011 opcode: 3, feedback)
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Regulations_Requests = 0 </b>
     loop Standard loop, see "Normal operation, no user activity or application changing regulations."
     end


 else HMI sets regulations
     hmi -> acu : set regulations (API011 opcode: 5)
     hmi -> hmi : API011_Num_Of_Consecutive_Regulations_Sets++
     hmi -> hmi : 100ms timeout
     hmi -> acu : request regulations (API011 opcode: 4)
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Regulations_Sets = 0</b>
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Regulations_Requests++</b>
     acu -> hmi : publish regulations (API011 opcode: 4, feedback)
     loop Standard loop, see "Normal operation, no user activity or application changing regulations."
     end

 else <b> ABNORMAL SITUATION! SHALL BE AVOIDED </b> \n HMI sets regulations continuously. \n HMI sets regulations with period greater than 100ms and less than 5s.
     loop 5times
     hmi -> acu : set regulations (API011 opcode: 5)
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Regulations_Sets++</b>
     ... 100 milliseconds later ...
     hmi -> acu : request regulations (API011 opcode: 4)
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Regulations_Sets = 0</b>
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Regulations_Requests++ (1,2,3,4,5)</b>
     acu -> hmi : publish regulations (API011 opcode: 4, feedback)
     end
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Regulations_Requests >= </b> \n  <b> MAX_NUM_OF_CONSECUTIVE_REQUESTS </b>
     hmi -> acu : <b>request status (API011 opcode: 3) </b>
     acu -> hmi : <b>publish status (API011 opcode: 3, feedback) </b>
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Regulations_Requests = 0</b>
     ... 100 milliseconds later ...
     hmi -> acu : request regulations (API011 opcode: 4)
     acu -> hmi : publish regulations (API011 opcode: 4, feedback)

 else <b> ABNORMAL SITUATION! SHALL BE AVOIDED </b> \n HMI sets regulations continuously. \n HMI sets regulations with period less than 100ms.
     loop 5 times
     hmi -> acu : set regulations (API011 opcode: 5)
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Regulations_Sets++ (1)</b>
     hmi -> acu : set regulations (API011 opcode: 5)
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Regulations_Sets++ (2)</b>
     hmi -> acu : set regulations (API011 opcode: 5)
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Regulations_Sets++ (3)</b>
     hmi -> acu : set regulations (API011 opcode: 5)
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Regulations_Sets++ (4)</b>
     hmi -> acu : set regulations (API011 opcode: 5)
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Regulations_Sets++ (5)</b>
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Regulations_Sets >= </b> \n  <b> MAX_NUM_OF_CONSECUTIVE_REQUESTS </b>
     hmi -> acu : request regulations (API011 opcode: 4)
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Regulations_Sets = 0</b>
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Regulations_Requests++ (1,2,3,4,5)</b>
     acu -> hmi : publish regulations (API011 opcode: 4, feedback)
     end
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Regulations_Requests >= </b> \n  <b> MAX_NUM_OF_CONSECUTIVE_REQUESTS </b>
     hmi -> acu : <b>request status (API011 opcode: 3) </b>
     acu -> hmi : <b>publish status (API011 opcode: 3, feedback) </b>
     hmi -> hmi : <b>API011_Num_Of_Consecutive_Regulations_Requests = 0</b>
     ... 100 milliseconds later ...
     hmi -> acu : request regulations (API011 opcode: 4)
     acu -> hmi : publish regulations (API011 opcode: 4, feedback)
 end
 @enduml

 *  @copyright  Copyright 2015 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"
#include "Reveal.h"
#include "API011AppCtrl.h" 
#include "API011AppCtrl_prv.h"

#if (SYSTEM_ACU_HANDLING == ENABLED)
    #include "ApplianceMgr.h"
    #include "Selector.h"
    #include <math.h>
#endif

#if (NVDATA_STORE_APPLIANCE_AND_STATISTICS_FEATURE == ENABLED)
    #include "NVData.h"
#endif

#if (SYSTEM_ACU_HANDLING == ENABLED) || (SYSTEM_HMI_HANDLING == ENABLED)
    #include "Regulations.h"
    #include "Status.h"
#endif

#ifndef API011APPCTRL_LOG_FEATURE
#define API011APPCTRL_LOG_FEATURE ENABLED
#endif

#if (SYSTEM_HMI_HANDLING == ENABLED)
    #include "Millis.h"
    #if(API011APPCTRL_LOG_FEATURE == ENABLED)
        #include "Log.h"
    #endif
#endif

#include "Micro.h"
#include "Mode.h"
#include "SettingFile.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

#ifndef API0011APPCTRL_USE_PRIORITY_QUEUE
    #define API0011APPCTRL_USE_PRIORITY_QUEUE DISABLED
#endif

#ifndef API011APPCTRL__RECEIVED_ERROR
    #define API011APPCTRL__RECEIVED_ERROR(node,error)
#endif

#if (SYSTEM_HMI_HANDLING == ENABLED)
    #ifndef API011APPCTRL_REGULATION_RX_NOTIFICATION
        #error "API011APPCTRL_REGULATION_RX_NOTIFICATION must be defined in API011AppCtrl_prv.h. This macro is used to notify when Regulations are received from the ACU"
    #endif
#endif

#if (API0011APPCTRL_USE_PRIORITY_QUEUE == ENABLED)
        #define API011REVEAL__NEWFEEDBACK(api,opcode,node,size)   Reveal__PriorityNewFeedback(api,opcode,node,size)
        #define API011REVEAL__NEWCOMMAND(api,opcode,node,size)    Reveal__PriorityNewCommand(api,opcode,node,size)
#else
        #define API011REVEAL__NEWFEEDBACK(api,opcode,node,size)   Reveal__NewFeedback(api,opcode,node,size)
        #define API011REVEAL__NEWCOMMAND(api,opcode,node,size)    Reveal__NewCommand(api,opcode,node,size)
#endif
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//! API011APPCTRL opcodes
typedef enum API011APPCTRL_OPCODE_ENUM
{
    API011APPCTRL_OPCODE_REQ_PUB_STATUS                     = 3,
    API011APPCTRL_OPCODE_REQ_PUB_REGULATIONS                = 4,
    API011APPCTRL_OPCODE_SET_REGULATIONS                    = 5,
    API011APPCTRL_OPCODE_REQ_PUB_MODE                       = 6,
    API011APPCTRL_OPCODE_SET_MODE                           = 7,
    API011APPCTRL_OPCODE_MODE_HEARTBEAT                     = 8,
    API011APPCTRL_OPCODE_REG_POSITION_BY_ID                 = 9,
    API011APPCTRL_OPCODE_REG_NUM_CYCLES                     = 10,
    API011APPCTRL_OPCODE_REG_PUB_STATUS_STRUCTURE           = 11,
    API011APPCTRL_OPCODE_REQ_PUB_FUNCTION_IDS               = 12,
    API011APPCTRL_OPCODE_REQ_PUB_CYCLE_NAMES                = 13,
    API011APPCTRL_OPCODE_RESTORE_FACTORY_REGULATION_INDEX   = 14,
    API011APPCTRL_OPCODE_REQ_PUB_PHASE_NAMES                = 15,

    API011APPCTRL_OPCODE_ERROR = 31
} API011APPCTRL_OPCODE_TYPE;

//! API011APPCTRL errors
enum API011APPCTRL_ERROR_ENUM
{
    API011APPCTRL_ERROR_NONE                    = 0,
    API011APPCTRL_ERROR_STATUS_NOT_LOADED       = 1,
    API011APPCTRL_ERROR_REGULATION_NOT_LOADED   = 2
};

#if (SYSTEM_HMI_HANDLING == ENABLED)
    //! List of possible content for the main HMI to request from the main ACU
    typedef enum API011APPCTRL_MESSAGE_REQUEST_ENUM
    {
        API011APPCTRL_MESSAGE_REQUEST_REGULATIONS = 0,
        API011APPCTRL_MESSAGE_REQUEST_STATUS      = 1
    }API011APPCTRL_MESSAGE_REQUEST_TYPE;

    //! Period of standard API011AppCtrl requests.
    #define API011APPCTRL_STANDARD_REQUEST_PERIOD   ((uint32)5000U)
    //! Timeout to next request after set regulations event or abnormal situation detection (stuck sending same requests continuously).
    #define API011APPCTRL_TIMEOUT_TO_NEXT_REQUEST   ((uint32)100U)
    //! Initial timeout for module timer
    #define API011APPCTRL_INITIAL_TIMEOUT           ((uint32)10U)
    //! Period of elapsed time to be set in case of initialization event, having a time base of 5000 ms
    #define API011APPCTRL_INITIAL_TIMEOUT_ELAPSED_TIME  (uint32)(API011APPCTRL_STANDARD_REQUEST_PERIOD-API011APPCTRL_INITIAL_TIMEOUT)

    //! Timer used to set the elapsed time referred to a time base of 5000ms.
    static MILLIS_TIMER_TYPE API011_Elapsed_Time;
    //! Flag that indicates if polling is on or off
    static ON_OFF_TYPE API011_Hmi_Poll_Reg_Status;
    //! Additional timeout before polling is started
    static uint8 API011_Poll_Timeout;
    //! This is a flag to make the API alternate the request for Regulations and Status on the handler. TRUE requests Regulations.
    static API011APPCTRL_MESSAGE_REQUEST_TYPE API011AppCtrl_Request_Message_Content;

#ifndef MAX_NUM_OF_CONSECUTIVE_REQUESTS
    //! Defines maximum number of consecutive request of same type.
    #define MAX_NUM_OF_CONSECUTIVE_REQUESTS         (5U)
#elif(MAX_NUM_OF_CONSECUTIVE_REQUESTS < 2)
    #error "MAX_NUM_OF_CONSECUTIVE_REQUESTS shall be greater than 1. Recommended value is 5."
#endif

    //! Counter used to count number of consecutive regulations requests.
    static uint8 API011_Num_Of_Consecutive_Regulations_Requests;
    //! Counter used to count number of consecutive status requests.
    static uint8 API011_Num_Of_Consecutive_Status_Requests;
    //! Counter used to count number of consecutive regulations sets.
    static uint8 API011_Num_Of_Consecutive_Regulations_Sets;

    //! Flag used to request regulations and status once at the initialization once ACU is alive.
    static BOOL_TYPE First_Status_Regulations_Requested;

    //! Flag used to track the HMI has received at least one regulations packet from the ACU
    static BOOL_TYPE API011_Received_Regulations;

    #if(API011APPCTRL_LOG_FEATURE == ENABLED)
    //! IDs for Log messages
    typedef enum
    {
        TO_MANY_CONSECUTIVE_REGULATIONS_SETS = 0,       //data: 0
        TO_MANY_CONSECUTIVE_REGULATIONS_REQUESTS,       //data: 0
        TO_MANY_CONSECUTIVE_STATUS_REQUESTS,            //data: 0
    } MODULE_API011APPCTRL_LOG_MESSAGE_ID_TYPE;
    //! ID for this module on the Log module.
    #define MODULE_NAME     MODULE_API011APPCTRL
    #endif //(API011APPCTRL_LOG_FEATURE == ENABLED)
#endif //(SYSTEM_HMI_HANDLING == ENABLED)

#if (SYSTEM_ACU_HANDLING == ENABLED)
//! Number of bytes for each function in the message that publishes function IDs (Opcode 12)
#define API011_FUNCTION_ID_DATA_SIZE   2U

//! Invalid index reference - used for Request Cycle Name and Request Cycle Phase
#define INVALID_INDEX                   0xFFU

//! Maximum number of consecutive invalid cycles to stop publish Cycle Names
#define API011APPCTRL_MAX_INVALID_CYCLES        8U

//! Keeps track of the last value reported from ApplianceMgr module on whether it is stable.
//! Used to detect transition from un-stable to stable so status can be published
static BOOL_TYPE API011_Status_Ready;

//! Compartment parameter used when a Request Cycle Names command is processed - INVALID_INDEX value is set when no requests are in progress
static uint8 Request_Cycle_Names_Compartment;
//! Selector Regulation index used when a Request Cycle Names command is processed - INVALID_INDEX value is set when no requests are in progress
static uint8 Request_Cycle_Names_Selector_Regulation;
//! Current Cycle index used when a Request Cycle Names command is processed
static uint8 Request_Cycle_Names_Current_Index;
//! Maximum Cycle index reference when a Request Cycle Names command is processed
static uint8 Request_Cycle_Names_Max_Index;
//! Number of consecutive invalid cycle references detected when a Request Cycle Names command is processed - it stops the request when it reaches API011APPCTRL_MAX_INVALID_CYCLES
static uint8 Request_Cycle_Names_Invalid_Cycles_Counter;

//! Compartment parameter used when a Request Phase Names command is processed - INVALID_INDEX value is set when no requests are in progress
static uint8 Request_Phase_Names_Compartment;
//! Cycle index parameter used when a Request Phase Names command is processed - INVALID_INDEX value is set when no requests are in progress
static uint8 Request_Phase_Names_Cycle;
//! Current Cycle index used when a Request Phase Names command is processed
static uint8 Request_Phase_Names_Current_Index;
//! Maximum Cycle index reference when a Request Phase Names command is processed
static uint8 Request_Phase_Names_Max_Index;

#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
#if ((SYSTEM_ACU_HANDLING == ENABLED) && (SYSTEM_HMI_HANDLING == DISABLED))
    static void API011AppCtrlPublishStatus(void);
#endif
#if (SYSTEM_ACU_HANDLING == ENABLED)
    static void API011AppCtrlPublishCycleNames(void);
    static void API011AppCtrlPublishPhaseNames(void);
#endif

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      It Initializes the module API011AppCtrl and its variables
 *
 */
void API011AppCtrl__Initialize(void)
{
    (void)API011AppCtrl__PubMode();

    #if (SYSTEM_HMI_HANDLING == ENABLED)

        Millis__Start(&API011_Elapsed_Time);
        Millis__SetElapsed(&API011_Elapsed_Time, API011APPCTRL_INITIAL_TIMEOUT_ELAPSED_TIME);

        if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
        {
            API011_Hmi_Poll_Reg_Status = ON;
        }
        else
        {
            API011_Hmi_Poll_Reg_Status = OFF;
        }

        API011_Poll_Timeout = 200;

        API011AppCtrl_Request_Message_Content = API011APPCTRL_MESSAGE_REQUEST_REGULATIONS;
        API011_Num_Of_Consecutive_Regulations_Requests = 0;
        API011_Num_Of_Consecutive_Status_Requests = 0;
        API011_Num_Of_Consecutive_Regulations_Sets = 0;
        First_Status_Regulations_Requested = FALSE;
        API011_Received_Regulations = FALSE;
    #endif
    
    #if (SYSTEM_ACU_HANDLING == ENABLED)
        API011_Status_Ready = FALSE;
        Request_Cycle_Names_Selector_Regulation = INVALID_INDEX;
        Request_Cycle_Names_Compartment = INVALID_INDEX;
        Request_Cycle_Names_Invalid_Cycles_Counter = 0U;
        Request_Cycle_Names_Current_Index = 0U;
        Request_Cycle_Names_Max_Index = 0U;
        Request_Phase_Names_Compartment = INVALID_INDEX;
        Request_Phase_Names_Cycle = INVALID_INDEX;
        Request_Phase_Names_Current_Index = 0U;
        Request_Phase_Names_Max_Index = 0U;
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
void API011AppCtrl__Handler(void)
{
    #if (SYSTEM_HMI_HANDLING == ENABLED)

        if(FALSE == First_Status_Regulations_Requested)
        {
            if ((((Reveal__GetNodestatus()) >> ((uint16)WIN_ACU_ADDRESS)) & (1U)) == 1U)
            {
                /*
                   Request status and regulations once ACU is detected alive.
                   Reset API011_Comm_Handle timer to avoid multiple, redundant requests at the same time.
                   Set API011AppCtrl_Request_Message_Content to API011APPCTRL_MESSAGE_REQUEST_REGULATIONS
                   to reinitialize standard request mechanism.
                 */
                (void)API011AppCtrl__RequestStatus(WIN_ACU_ADDRESS);
                (void)API011AppCtrl__RequestRegulations(WIN_ACU_ADDRESS);
                First_Status_Regulations_Requested = TRUE;
                Millis__RestartPeriod(&API011_Elapsed_Time, API011APPCTRL_STANDARD_REQUEST_PERIOD);
                API011AppCtrl_Request_Message_Content = API011APPCTRL_MESSAGE_REQUEST_REGULATIONS;
            }
        }

        if (API011_Poll_Timeout > 0U)
        {
            API011_Poll_Timeout--;
        }
        else
        {
            API011_Hmi_Poll_Reg_Status = ON;
        }

        if ((Mode__GetRemoteMode() == MODE_NORMAL) &&
            (Mode__GetMode() == MODE_NORMAL) &&
            (Mode__GetSubMode() != SUBMODE_NORMAL_EXT_CONTROL_ENGAGED) &&
            (API011_Hmi_Poll_Reg_Status == ON))
        {
            if(Millis__ReachedTarget(&API011_Elapsed_Time, API011APPCTRL_STANDARD_REQUEST_PERIOD))
            {
                Millis__RestartPeriod(&API011_Elapsed_Time,API011APPCTRL_STANDARD_REQUEST_PERIOD);
                // Checking the flag to decide if it is time to request Regulations or Status.
                if(API011AppCtrl_Request_Message_Content == API011APPCTRL_MESSAGE_REQUEST_REGULATIONS)
                {
                    API011_Num_Of_Consecutive_Regulations_Requests++;   // Increment number of regulations requests.
                    API011_Num_Of_Consecutive_Status_Requests = 0;      // Clear counter for status requests.
                    API011_Num_Of_Consecutive_Regulations_Sets = 0;     // Clear counter for regulations sets.
                    (void)API011AppCtrl__RequestRegulations(WIN_ACU_ADDRESS);
                    API011AppCtrl_Request_Message_Content = API011APPCTRL_MESSAGE_REQUEST_STATUS;
                }
                else
                {
                    API011_Num_Of_Consecutive_Status_Requests++;        // Increment number of status requests.
                    API011_Num_Of_Consecutive_Regulations_Requests = 0; // Clear counter for regulations requests.
                    (void)API011AppCtrl__RequestStatus(WIN_ACU_ADDRESS);
                    API011AppCtrl_Request_Message_Content = API011APPCTRL_MESSAGE_REQUEST_REGULATIONS;
                }
            }
            else
            {
                /*
                 * Verify if any of counters reached the limit. If yes act immediately.
                 */
                if((API011_Num_Of_Consecutive_Regulations_Sets >= MAX_NUM_OF_CONSECUTIVE_REQUESTS) ||
                   (API011_Num_Of_Consecutive_Status_Requests >= MAX_NUM_OF_CONSECUTIVE_REQUESTS))
                {
                    /*
                     * To many regulations sets without requesting regulations OR
                     * to many status requests without requesting regulations.
                     * In both cases Request Regulations packet needs to be sent to ACU.
                     */
                    #if(API011APPCTRL_LOG_FEATURE == ENABLED)
                    if(API011_Num_Of_Consecutive_Regulations_Sets >= MAX_NUM_OF_CONSECUTIVE_REQUESTS)
                    {
                        // Reaching this point indicates abnormal behavior. This shall be avoided.
                        LOG_ADD_EXCEPTION(TO_MANY_CONSECUTIVE_REGULATIONS_SETS, 0);
                    }
                    else
                    {
                        // Reaching this point indicates abnormal behavior. This shall be avoided.
                        LOG_ADD_EXCEPTION(TO_MANY_CONSECUTIVE_STATUS_REQUESTS, 0);
                    }
                    #endif //(API011APPCTRL_LOG_FEATURE == ENABLED)

                    if(API011AppCtrl__RequestRegulations(WIN_ACU_ADDRESS))                              // Request regulations.
                    {
                        API011_Num_Of_Consecutive_Regulations_Sets = 0;                                 // Clear counter for regulations sets.
                        API011_Num_Of_Consecutive_Status_Requests = 0;                                  // Clear counter for status requests.
                        API011_Num_Of_Consecutive_Regulations_Requests++;                               // Increment number of regulations requests.
                        Millis__RestartPeriod(&API011_Elapsed_Time,API011APPCTRL_TIMEOUT_TO_NEXT_REQUEST);   // Set timer to get status in 100ms.
                        API011AppCtrl_Request_Message_Content = API011APPCTRL_MESSAGE_REQUEST_STATUS;   // Regulations just requested. Change content to status request.
                    }
                }
                else if(API011_Num_Of_Consecutive_Regulations_Requests >= MAX_NUM_OF_CONSECUTIVE_REQUESTS)
                {
                    /*
                     * To many regulation requests without status request.
                     * In that case Request Status packet needs to be sent to ACU.
                     */
                    if(API011AppCtrl__RequestStatus(WIN_ACU_ADDRESS))                                       // Request status
                    {
                        API011_Num_Of_Consecutive_Regulations_Requests = 0;                                 // Clear counter for regulations requests.
                        API011_Num_Of_Consecutive_Status_Requests++;                                        // Increment counter for status requests.
                        Millis__RestartPeriod(&API011_Elapsed_Time,API011APPCTRL_TIMEOUT_TO_NEXT_REQUEST);       // Set timer to get regulations in 100ms.
                        API011AppCtrl_Request_Message_Content = API011APPCTRL_MESSAGE_REQUEST_REGULATIONS;  // Status just requested. Change content to regulations request.
                    }
                    #if(API011APPCTRL_LOG_FEATURE == ENABLED)
                    // Reaching this point indicates abnormal behavior. This shall be avoided.
                    LOG_ADD_EXCEPTION(TO_MANY_CONSECUTIVE_REGULATIONS_REQUESTS, 0);
                    #endif //(API011APPCTRL_LOG_FEATURE == ENABLED)
                }
            }
        }
    #elif (SYSTEM_ACU_HANDLING == ENABLED)
        API011AppCtrlPublishStatus();
        if(Request_Cycle_Names_Selector_Regulation != INVALID_INDEX)
        {
            API011AppCtrlPublishCycleNames();
        }
        if((Request_Phase_Names_Compartment != INVALID_INDEX) &&
            (Request_Phase_Names_Cycle != INVALID_INDEX))
        {
            API011AppCtrlPublishPhaseNames();
        }
    #endif
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief   This method parses messages target to the API017 sent to a broadcast or exclusively to this node for Reveal 4.x.
 * @param   buffer contains the Wide package that contains the message
 * @return  REVEAL_RECIPE_STATUS_TYPE that express the result of the parsing.
 */
REVEAL_RECIPE_STATUS_TYPE API011AppCtrl__CommandParser(REVEAL_MSG_TYPE * buffer)
{
    REVEAL_RECIPE_STATUS_TYPE retval;
    #if (SYSTEM_ACU_HANDLING == ENABLED)
        REVEAL_COMMAND_TYPE * ptr;
        uint16 remaining_bytes;
        uint8 function_position;
        uint8 size;
    #endif

    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    retval = NVO_RS_READY;

    // data is a pointer that points to the payload
    switch ((API011APPCTRL_OPCODE_TYPE)buffer->op_code)
    {
        case API011APPCTRL_OPCODE_REQ_PUB_MODE:
            if (API011AppCtrl__PubMode() == FALSE)
            {
                retval = NVO_RS_BUSY;
            }
            break;
        case API011APPCTRL_OPCODE_SET_MODE:
            if (buffer->size == 1U)
            {
                Mode__SetMode((MODE_TYPE)buffer->payload[0]);
            }
            else if (buffer->size == 2U)
            {
                Mode__SetModeExtended((MODE_TYPE)buffer->payload[0], (SUBMODE_TYPE)buffer->payload[1]);
            }
            else
            {
                retval = NVO_RS_UNSUP_INVALID_PARAM;
            }
            break;
        case API011APPCTRL_OPCODE_MODE_HEARTBEAT:
            //buffer->payload[0] = heartbeat signature
            Mode__SpecialHeartBeat(MODE_SPECIAL_HEARTBEAT_PC_CONTROL);
            break;
    #if (SYSTEM_ACU_HANDLING == ENABLED)
        case API011APPCTRL_OPCODE_REQ_PUB_STATUS:
            if (API011AppCtrl__PubStatus() == FALSE)
            {
                retval = NVO_RS_REJECTED;
            }
            break;
        case API011APPCTRL_OPCODE_REQ_PUB_REGULATIONS:
            (void)API011AppCtrl__PubRegulations(buffer->node_id);
            break;
        case API011APPCTRL_OPCODE_SET_REGULATIONS:
            Regulations__SetRequestIndexBuffer(&buffer->payload[1], buffer->size - 1U);
            break;
        case API011APPCTRL_OPCODE_REG_POSITION_BY_ID:

            if (buffer->size == 1U)
            {
                ptr = API011REVEAL__NEWFEEDBACK(API011APPCTRL_NUM, API011APPCTRL_OPCODE_REG_POSITION_BY_ID, REVEAL_BROADCAST_NODE,2);
                if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
                {
                    (void)Reveal__AddDataU8(ptr, buffer->payload[0]); // Reg ID
                    (void)Reveal__AddDataU8(ptr, Regulations__GetPositionByFunctionID(buffer->payload[0],0)); // Reg Position
                    (void)Reveal__QueueCommand(ptr);
                }
            }
            else if (buffer->size == 2U)
            {
                ptr = API011REVEAL__NEWFEEDBACK(API011APPCTRL_NUM, API011APPCTRL_OPCODE_REG_POSITION_BY_ID, REVEAL_BROADCAST_NODE,3);
                if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
                {
                    (void)Reveal__AddDataU8(ptr, buffer->payload[0]); // Reg ID
                    (void)Reveal__AddDataU8(ptr, buffer->payload[1]); // Compartment
                    (void)Reveal__AddDataU8(ptr, Regulations__GetPositionByFunctionID(buffer->payload[0],buffer->payload[1]));
                    (void)Reveal__QueueCommand(ptr);
                }
            }
            else
            {
                retval = NVO_RS_UNSUP_INVALID_PARAM;
            }

            break;
        case API011APPCTRL_OPCODE_REG_NUM_CYCLES:
            ptr = API011REVEAL__NEWFEEDBACK(API011APPCTRL_NUM, API011APPCTRL_OPCODE_REG_NUM_CYCLES, REVEAL_BROADCAST_NODE,APPLIANCE_MAX_COMPARTMENTS);
            if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
            {
                uint8 compartment_index;

                for (compartment_index = 0U; compartment_index < APPLIANCE_MAX_COMPARTMENTS; compartment_index++)    // Add number of cycles in each compartment
                {
                    (void)Reveal__AddDataU8(ptr, Selector__GetNumberOfCycles(PRIMARY_SETTING_FILE, compartment_index));
                }

                (void)Reveal__QueueCommand(ptr);
            }
            break;
        case API011APPCTRL_OPCODE_REG_PUB_STATUS_STRUCTURE:
            size = Status__GetSize();

            if (size > 0U)
            {
                size = (uint8)((size * 3U) + 1U);

                ptr = API011REVEAL__NEWFEEDBACK(API011APPCTRL_NUM, API011APPCTRL_OPCODE_REG_PUB_STATUS_STRUCTURE, REVEAL_BROADCAST_NODE, size + 1U);
                if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
                {
                    Status__BuildStatusInfoPackage(ptr);
                    (void)Reveal__QueueCommand(ptr);
                }
            }
            else
            {
                ptr = API011REVEAL__NEWFEEDBACK(API011APPCTRL_NUM, API011APPCTRL_OPCODE_ERROR, REVEAL_BROADCAST_NODE, 1);
                if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
                {
                    (void)Reveal__AddDataU8(ptr, API011APPCTRL_ERROR_STATUS_NOT_LOADED);
                    (void)Reveal__QueueCommand(ptr);
                }
            }
            break;
        case API011APPCTRL_OPCODE_REQ_PUB_FUNCTION_IDS:
            function_position = 0;                                                                  // Start with the first function
            remaining_bytes = (uint16)Regulations__GetTotalSize() * API011_FUNCTION_ID_DATA_SIZE;   // Multiplying since x bytes are added to the payload for each function (function ID and compartment)

            while (remaining_bytes > 0U)
            {
                if (remaining_bytes > (uint16) REVEAL_PAYLOAD_SIZE)
                {
                    size = REVEAL_PAYLOAD_SIZE;
                }
                else
                {
                    size = (uint8)remaining_bytes;
                }

                ptr = API011REVEAL__NEWFEEDBACK(API011APPCTRL_NUM, API011APPCTRL_OPCODE_REQ_PUB_FUNCTION_IDS, REVEAL_BROADCAST_NODE, size);

                if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
                {
                    REGULATIONS_CONFIG_TYPE * regulation_config;
                    for (uint8 payload_index = 0; payload_index < (REVEAL_PAYLOAD_SIZE - 1U); payload_index+=API011_FUNCTION_ID_DATA_SIZE)    // Add function ID and compartment for each function position
                    {
                        regulation_config = Regulations__GetRegulationConfig(function_position);
                        if (regulation_config != NULL)
                        {
                            (void)Reveal__AddDataU8(ptr, regulation_config->Function_ID);
                            uint8 compartment = (regulation_config->Compartment_Extension << 2) | regulation_config->Compartment;
                            (void)Reveal__AddDataU8(ptr, compartment);
                            function_position++;                                // Move to next function
                            remaining_bytes -= API011_FUNCTION_ID_DATA_SIZE;    // Subtract x bytes that were added (function ID and compartment)
                        }
                        else
                        {
                            // Encountered NULL pointer for a regulation; end the message
                            remaining_bytes = 0;
                        }

                        if (remaining_bytes == 0U)
                        {
                            break;                                          // Exit for loop, there are no more bytes to send
                        }
                    }

                    // Set the MMP flag if there will be more messages to send
                    if (remaining_bytes > 0U)
                    {
                        ptr->opcode |= REVEAL_MMP_BIT;
                    }

                    (void)Reveal__QueueCommand(ptr);
                }
                else
                {
                    break;  // Exit while loop if unable to get Reveal message
                }
            }
            break;
        case API011APPCTRL_OPCODE_REQ_PUB_CYCLE_NAMES:
            if(Request_Cycle_Names_Selector_Regulation == INVALID_INDEX)
            {
                uint8 compartment = buffer->payload[0];
                Request_Cycle_Names_Selector_Regulation = Regulations__GetPositionByFunctionID(FUNC_ID_SELECTOR, compartment);

                if(Request_Cycle_Names_Selector_Regulation != INVALID_INDEX)
                {
                    Request_Cycle_Names_Compartment = compartment;
                    Request_Cycle_Names_Max_Index = Regulations__GetRegulationConfig(Request_Cycle_Names_Selector_Regulation)->Num_Regulations;
                    Request_Cycle_Names_Invalid_Cycles_Counter = 0U;
                    Request_Cycle_Names_Current_Index = 1U;
                }
                else
                {
                    retval = NVO_RS_UNSUP_INVALID_PARAM;
                }
            }
            else
            {
                retval = NVO_RS_BUSY;
            }
            break;

        case API011APPCTRL_OPCODE_REQ_PUB_PHASE_NAMES:
            if((Request_Phase_Names_Compartment == INVALID_INDEX) ||
                (Request_Phase_Names_Cycle == INVALID_INDEX))
            {
                uint8 compartment = buffer->payload[0];
                uint16 cycle_name_id = COMBINE_BYTES((buffer->payload[1]), (buffer->payload[2]));
                uint8 cycle_index = Selector__GetCycleByName(PRIMARY_SETTING_FILE, compartment, (CYCLE_NAME_TYPE)cycle_name_id);
                PHASE_DATA_TYPE phase_data;

                if ((cycle_index != 0xFFU) &&                                                                           // Check if cycle exists
                    (Selector__LoadPhaseData(PRIMARY_SETTING_FILE, compartment, cycle_index, 0, &phase_data) == PASS))  // Get number of phases
                {
                    Request_Phase_Names_Compartment = compartment;
                    Request_Phase_Names_Cycle = cycle_index;
                    Request_Phase_Names_Max_Index = phase_data.Number_Of_Cycle_Phases;
                    Request_Phase_Names_Current_Index = 0U;
                }
                else
                {
                    retval = NVO_RS_UNSUP_INVALID_PARAM;
                }
            }
            else
            {
                retval = NVO_RS_BUSY;
            }
            break;
    #endif //(SYSTEM_ACU_HANDLING == ENABLED)

        case API011APPCTRL_OPCODE_RESTORE_FACTORY_REGULATION_INDEX:

            //The blow code shall be protected. No other Threads shall be serviced here
            MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

            #if (NVDATA_STORE_APPLIANCE_AND_STATISTICS_FEATURE == ENABLED)
                //Store the Data
                (void)NVData__SetApplianceSegmentCorrupt();
            #endif

            Micro__ForceReset(MICRO_RESET_MODE_COLD);
            MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
            break;

        default:
            retval = NVO_RS_UNSUP_OP_CODE;
            break;
    }

    return (retval);
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method parses feedbacks to the API011AppCtrl sent to a broadcast or exclusively to this node.
 * @param buffer contains the Wide package that contains the message
 */
void API011AppCtrl__FeedbackParser(REVEAL_MSG_TYPE * buffer)
{
    switch ((API011APPCTRL_OPCODE_TYPE)buffer->op_code)
    {
    #if (SYSTEM_HMI_HANDLING == ENABLED)
        case API011APPCTRL_OPCODE_REQ_PUB_STATUS:
            if (API011AppCtrl_Request_Message_Content == API011APPCTRL_MESSAGE_REQUEST_STATUS)
            {
                Millis__RestartPeriod(&API011_Elapsed_Time,API011APPCTRL_STANDARD_REQUEST_PERIOD);
                API011AppCtrl_Request_Message_Content = API011APPCTRL_MESSAGE_REQUEST_REGULATIONS;
            }
            API011_Num_Of_Consecutive_Regulations_Requests = 0;

            #define VARIABLE_PAYLOAD_SIZE       (buffer->size - 1U)

            Status__SetACUStatus(buffer->payload[0], &buffer->payload[1], VARIABLE_PAYLOAD_SIZE);

            break;
        case API011APPCTRL_OPCODE_REQ_PUB_REGULATIONS:
            if (API011AppCtrl_Request_Message_Content == API011APPCTRL_MESSAGE_REQUEST_REGULATIONS)
            {
                Millis__RestartPeriod(&API011_Elapsed_Time,API011APPCTRL_STANDARD_REQUEST_PERIOD);
                API011AppCtrl_Request_Message_Content = API011APPCTRL_MESSAGE_REQUEST_STATUS;
            }
            Regulations__SetIndexBuffer(&buffer->payload[1], buffer->size - 1U);
            API011APPCTRL_REGULATION_RX_NOTIFICATION();
            API011_Received_Regulations = TRUE;
            break;
    #endif
        case API011APPCTRL_OPCODE_REQ_PUB_MODE:
            Mode__SaveRemoteMode((MODE_TYPE)buffer->payload[0]);
            Mode__SaveRemoteSubMode((SUBMODE_TYPE)buffer->payload[1]);
            break;
        case API011APPCTRL_OPCODE_ERROR:
            API011APPCTRL__RECEIVED_ERROR(buffer->node_id,buffer->payload[1]);
            break;
        default:
            // Take no action on opcode that is not handled
            break;
    }
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method is used from the Application to request the status from a given node.
 * @param address
 * @return
 */
BOOL_TYPE API011AppCtrl__RequestStatus(uint8 address)
{
    #if (SYSTEM_HMI_HANDLING == ENABLED)
        return (Reveal__QueueMessageNoData((uint8)API011APPCTRL_NUM,(uint8)API011APPCTRL_OPCODE_REQ_PUB_STATUS,address,TRUE,(BOOL_TYPE)API0011APPCTRL_USE_PRIORITY_QUEUE));
    #else
        return (FALSE);
    #endif
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method is used to Request Regulations from a given Node.
 * @param address
 * @return
 */
BOOL_TYPE API011AppCtrl__RequestRegulations(uint8 address)
{
    #if (SYSTEM_HMI_HANDLING == ENABLED)
        return (Reveal__QueueMessageNoData((uint8)API011APPCTRL_NUM,(uint8)API011APPCTRL_OPCODE_REQ_PUB_REGULATIONS,address,TRUE,(BOOL_TYPE)API0011APPCTRL_USE_PRIORITY_QUEUE));
    #else
        return (FALSE);
    #endif
}


//----------------------------------------------------------------------------------------------------------
/**
 * @brief   Method to set regulations on the ACU.
 * @details This method should not be called from any project specific module. It should only be called
 *          from the RegulationFilter module to avoid any side effects. This method prevents a regulation 
 *          message from being sent before regulations have been received by this node.
 * @param   address: node ID to send the regulations to
 * @retval  TRUE:  Regulations were successfully sent
 * @retval  FALSE: Regulations have not been received from the ACU yet, no regulations are defined or
 *                 no reveal message could be created
 */
BOOL_TYPE API011AppCtrl__SetRegulations(uint8 address)
{
    #if (SYSTEM_HMI_HANDLING == ENABLED)
        BOOL_TYPE retval = FALSE;
        if (API011_Received_Regulations)        // Only send regulations once they've been received from the ACU
        {
            uint8 * ptd = (uint8 *)Regulations__GetIndexRequestBufferPtr(); //lint !e926 cast from pointer to pointer [MISRA 2004 Rule 11.4]
            uint8 size = Regulations__GetTotalSize();

            if ((size > 0U) &&
                (ptd != NULL))
            {
                REVEAL_COMMAND_TYPE * ptr = API011REVEAL__NEWCOMMAND(API011APPCTRL_NUM, API011APPCTRL_OPCODE_SET_REGULATIONS, address, size + 1U);
                if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
                {
                    (void)Reveal__AddDataU8(ptr, 0);      // The requirement for this data ("product type") has not yet been defined.
                    (void)Reveal__AddDataU8Buffer(ptr, ptd,size);
                    (void)Reveal__QueueCommand(ptr);
                    retval = TRUE;
                    API011_Num_Of_Consecutive_Regulations_Sets++;
                    Millis__SetElapsed(&API011_Elapsed_Time,API011APPCTRL_STANDARD_REQUEST_PERIOD - API011APPCTRL_TIMEOUT_TO_NEXT_REQUEST);
                    API011AppCtrl_Request_Message_Content = API011APPCTRL_MESSAGE_REQUEST_REGULATIONS;
                }
            }
        }
        return (retval);
    #else
        return (FALSE);
    #endif
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method is used to publish the current Mode as a broadcast feedback.
 * @param mode
 * @return
 */
BOOL_TYPE API011AppCtrl__PubMode(void)
{
    BOOL_TYPE retval = FALSE;
    REVEAL_COMMAND_TYPE * ptr = API011REVEAL__NEWFEEDBACK(API011APPCTRL_NUM, API011APPCTRL_OPCODE_REQ_PUB_MODE, REVEAL_BROADCAST_NODE, 2);

    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        (void)Reveal__AddDataU8(ptr, Mode__GetMode());
        (void)Reveal__AddDataU8(ptr, Mode__GetSubMode());
        (void)Reveal__QueueCommand(ptr);
        retval = TRUE;
    }

    return(retval);
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method published the Status when requested, via command or function call.
 * @details Multiple messages are sent if the data does not fit in the messages
 * @return
 *  @retval - TRUE(1) - Able to build the message with all the status data
 *  @retval - FALSE(0) - Failed to build the message
 */
BOOL_TYPE API011AppCtrl__PubStatus(void)
{
    #if (SYSTEM_ACU_HANDLING == ENABLED)
        REVEAL_COMMAND_TYPE * ptr           = NULL;
        uint8 variable_list_track           = 0;
        BOOL_TYPE status                    = FALSE;
        BOOL_TYPE variables_pending         = FALSE;

        if (API011_Status_Ready &&      // Only publish status when the ApplianceMgr module has reported that it is stable AND
            Status__IsValid())          // status configuration is valid
        {
            //The below logic sends multiple messages if all the data does not fit in one message
            do
            {
                #define VARIABLE_PAYLOAD_ALLOWED_LENGTH         (REVEAL_PAYLOAD_SIZE - 1U)

                ptr = Reveal__NewFeedback(API011APPCTRL_NUM, API011APPCTRL_OPCODE_REQ_PUB_STATUS, REVEAL_BROADCAST_NODE, VARIABLE_PAYLOAD_ALLOWED_LENGTH);
                if (ptr != (REVEAL_COMMAND_TYPE *)NULL)
                {
                    (void)Reveal__AddDataU8(ptr, variable_list_track);
                    if (Status__BuildPackage(ptr, &variable_list_track, &variables_pending, VARIABLE_PAYLOAD_ALLOWED_LENGTH) == PASS)
                    {
                        if (variables_pending)
                        {
                            //set the mmp bit if their are more than one message needs to be send
                            ptr->opcode |= REVEAL_MMP_BIT;
                        }
                        status = Reveal__QueueCommand(ptr);
                    }
                    else
                    {
                        status = FALSE;
                    }
                }
                else
                {
                    status = FALSE;
                }
            }while(variables_pending && status);
        }

        return(status);
    #else
        return (FALSE);
    #endif
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method published the Regulations when requested, via command of function call.
 * @return
 */
BOOL_TYPE API011AppCtrl__PubRegulations(uint8 node_id)
{
    #if (SYSTEM_ACU_HANDLING == ENABLED)
        REVEAL_COMMAND_TYPE * ptr;
        uint8 size;
        uint8 * ptd;
        BOOL_TYPE retval;

        retval = FALSE;
        size = Regulations__GetTotalSize();
        ptd = (uint8 *)Regulations__GetIndexBufferPtr();

        if (ptd != NULL)
        {
            ptr = API011REVEAL__NEWFEEDBACK(API011APPCTRL_NUM, API011APPCTRL_OPCODE_REQ_PUB_REGULATIONS, node_id, size + 1U);
            if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
            {
                (void)Reveal__AddDataU8(ptr, 0);  // The requirement for this data ("product type") has not yet been defined.
                (void)Reveal__AddDataU8Buffer(ptr, ptd,size);
                (void)Reveal__QueueCommand(ptr);
                retval = TRUE;
            }
        }

        if ((size == 0U) ||
            (ptd == NULL))
        {
            ptr = API011REVEAL__NEWFEEDBACK(API011APPCTRL_NUM, API011APPCTRL_OPCODE_ERROR, node_id,1);
            if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
            {
                (void)Reveal__AddDataU8(ptr, API011APPCTRL_ERROR_REGULATION_NOT_LOADED);
                (void)Reveal__QueueCommand(ptr);
            }
        }

        return (retval);
    #else
        return (FALSE);
    #endif
}

//----------------------------------------------------------------------------------------------------------
/**
 * @brief This method is used to control the polling process from the HMI to the ACU.
 * @param state
 */
void API011AppCtrl__SetRegStatusPolling(ON_OFF_TYPE state)
{
    #if (SYSTEM_HMI_HANDLING == ENABLED)
        API011_Hmi_Poll_Reg_Status = state;
    #endif
}
//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#if ((SYSTEM_ACU_HANDLING == ENABLED) && (SYSTEM_HMI_HANDLING == DISABLED))
//----------------------------------------------------------------------------------------------------------
/**
 * Local function to allow publishing Status on Event base. (ON CHANGE)
 * Here we check if the CRC of the Status variables has changed using a CRC16.
 */
static void API011AppCtrlPublishStatus(void)
{
    if (API011_Status_Ready != FALSE)                 // Wait for status variables to be ready before publishing
    {
        if (Status__HasChanged() != FALSE)            // Publish if any status variable has changed
        {                                            // Note: Calling Status__HasChanged() affects the logic that determines if the status has changed
            (void) API011AppCtrl__PubStatus();
        }
    }
    else if ((ApplianceMgr__IsStable() != FALSE) &&
             (Status__IsValid() != FALSE))           // Don't want to send status message if the setting file configuration is not valid
    {
        API011_Status_Ready = TRUE;
        API011AppCtrl__PubStatus();
        Status__HasChanged();                       // Update logic in Status module so it doesn't repeat the same message
    }
}
#endif

#if (SYSTEM_ACU_HANDLING == ENABLED)

//----------------------------------------------------------------------------------------------------------
/**
 * Local function to prepare a Publish Cycle Names message at a time.
 *
 *      Payload content:
 *
 *      [0]: Compartment ID
 *      [1,2]: Cycle Name of cycle indicated by the Main Value at index 1 in the regulation table of Selector regulation
 *      [3,4]: Cycle Name of cycle indicated by the Main Value at index 2 in the regulation table of Selector regulation.
 *      ......
 *
 */
static void API011AppCtrlPublishCycleNames(void)
{
    uint8 max_cycles = Selector__GetNumberOfCycles(PRIMARY_SETTING_FILE, Request_Cycle_Names_Compartment);
    uint8 cycles_published = 0U;
    REVEAL_COMMAND_TYPE * ptr;
    uint16 remaining_bytes;

    remaining_bytes = Request_Cycle_Names_Max_Index;
    remaining_bytes -= Request_Cycle_Names_Current_Index;
    remaining_bytes++;
    remaining_bytes *= 2U;
    if(Request_Cycle_Names_Current_Index == 1U)
    {
        remaining_bytes++;
    }

    if (remaining_bytes > (uint16) REVEAL_PAYLOAD_SIZE)
    {
        remaining_bytes = REVEAL_PAYLOAD_SIZE;
    }

    ptr = API011REVEAL__NEWFEEDBACK(API011APPCTRL_NUM, API011APPCTRL_OPCODE_REQ_PUB_CYCLE_NAMES, REVEAL_BROADCAST_NODE, (uint8) remaining_bytes);
    if (ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        BOOL_TYPE sending_invalid_cycles = FALSE;
        uint8 cycle_index = INVALID_INDEX;

        if(Request_Cycle_Names_Current_Index <= 1U)
        {
            (void)Reveal__AddDataU8(ptr, Request_Cycle_Names_Compartment);
            remaining_bytes--;
        }

        while(remaining_bytes >= 2U)
        {
            // Prevents Regulations__GetMainValueByPositionPlusIndex() call if multiple invalid cycles should be sent before selector_index
            if(sending_invalid_cycles == FALSE)
            {
                cycle_index = (uint8)Regulations__GetMainValueByPositionPlusIndex(Request_Cycle_Names_Selector_Regulation, Request_Cycle_Names_Current_Index);
            }

            // Publish cycle name only if index refers to valid cycle
            if(cycle_index < max_cycles)
            {
                if(Request_Cycle_Names_Invalid_Cycles_Counter > 0U)
                {
                    // There are invalid cycles between valid cycles - publish them
                    // Do not increment selector index since invalid cycles have not been published earlier
                    (void)Reveal__AddDataU16(ptr, CYCLE_NAME_INVALID);
                    sending_invalid_cycles = TRUE;
                    Request_Cycle_Names_Invalid_Cycles_Counter--;
                }
                else
                {
                    // Get cycle name and publish it, go to next selector index
                    uint16 cycle_name = Selector__GetCycleNameId(PRIMARY_SETTING_FILE, Request_Cycle_Names_Compartment, cycle_index);
                    (void)Reveal__AddDataU16(ptr, cycle_name);
                    sending_invalid_cycles = FALSE;
                    Request_Cycle_Names_Current_Index++;
                }
                cycles_published++;
                remaining_bytes -= 2U;
            }
            else
            {
                // Increment number of invalid cycles collected and go to next selector index
                Request_Cycle_Names_Invalid_Cycles_Counter++;
                if(Request_Cycle_Names_Invalid_Cycles_Counter >= API011APPCTRL_MAX_INVALID_CYCLES)
                {
                    // Stop after maximum consecutive invalid cycles detected
                    Request_Cycle_Names_Current_Index = Request_Cycle_Names_Max_Index;
                }
                else
                {
                    Request_Cycle_Names_Current_Index++;
                }
            }

            // End if all items in the selector regulation have been processed
            if(Request_Cycle_Names_Current_Index >= Request_Cycle_Names_Max_Index)
            {
                Request_Cycle_Names_Selector_Regulation = INVALID_INDEX;
                remaining_bytes = 0U;
            }
        }
        // Set the MMP flag if there will be more messages to send
        if(cycles_published > 0U)
        {
            if(Request_Cycle_Names_Current_Index < Request_Cycle_Names_Max_Index)
            {
                ptr->opcode |= REVEAL_MMP_BIT;
            }
            (void) Reveal__QueueCommand(ptr);
        }
    }
}

//----------------------------------------------------------------------------------------------------------
/**
 * Local function to prepare a Publish Phase Names message at a time.
 *
 *      Payload content:
 *
 *      [0]: Compartment ID
 *      [1,2]: Cycle Name
 *      [3,4]: Phase Name for Phase 0
 *      [5,6]: Phase Name for Phase 1
 *      ......
 *
 */
static void API011AppCtrlPublishPhaseNames(void)
{
    REVEAL_COMMAND_TYPE * ptr;
    uint16 remaining_bytes;
    

    remaining_bytes = Request_Phase_Names_Max_Index;
    remaining_bytes -= Request_Phase_Names_Current_Index;
    remaining_bytes *= 2U;
    if(Request_Phase_Names_Current_Index == 0U)
    {
        remaining_bytes += 3U;
    }

    // Append user phase names (this may require more than one Reveal message)
    if (remaining_bytes > (uint16) REVEAL_PAYLOAD_SIZE)
    {
        remaining_bytes = REVEAL_PAYLOAD_SIZE;
    }

    ptr = API011REVEAL__NEWFEEDBACK(API011APPCTRL_NUM, API011APPCTRL_OPCODE_REQ_PUB_PHASE_NAMES, REVEAL_BROADCAST_NODE, (uint8) remaining_bytes);
    if(ptr != (REVEAL_COMMAND_TYPE *) NULL)
    {
        // Append compartment and cycle name to beginning of the first feedback message
        if(Request_Phase_Names_Current_Index == 0U)
        {
            uint16 cycle_name_id = Selector__GetCycleNameId(PRIMARY_SETTING_FILE, Request_Phase_Names_Compartment, Request_Phase_Names_Cycle);
            (void) Reveal__AddDataU8(ptr, Request_Phase_Names_Compartment);
            (void) Reveal__AddDataU16(ptr, cycle_name_id);
            remaining_bytes -= 3U;                              // Subtract 3 bytes for compartment and cycle name
        }

        // Add the 2-byte user phase name for each phase in the cycle
        while(remaining_bytes >= 2U)
        {
            (void) Reveal__AddDataU16(ptr, Selector__GetUserPhaseName(PRIMARY_SETTING_FILE, Request_Phase_Names_Compartment, Request_Phase_Names_Cycle, Request_Phase_Names_Current_Index));
            Request_Phase_Names_Current_Index++;
            remaining_bytes -= 2U;                          // Subtract 2 bytes for user phase name

            if(Request_Phase_Names_Current_Index >= Request_Phase_Names_Max_Index)
            {
                Request_Phase_Names_Compartment = INVALID_INDEX;
                Request_Phase_Names_Cycle = INVALID_INDEX;
                remaining_bytes = 0U;
                break;                                      // Exit loop, there are no more bytes to send
            }
        }

        // Check that while() ends properly - this removes also warning 438 - Misra rule 2.2 - on remaining_bytes
        if(remaining_bytes < 2U)
        {
            // Set the MMP flag if there will be more messages to send
            if(Request_Phase_Names_Current_Index < Request_Phase_Names_Max_Index)
            {
                ptr->opcode |= REVEAL_MMP_BIT;
            }
            (void) (BOOL_TYPE)Reveal__QueueCommand(ptr);
        }
    }
}

#endif

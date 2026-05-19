/**
 *  @file       
 *
 *  @brief      HBL - Hardware Base Line
 *
 *  @details    This module implements the generic handler for Hbl for ACU profiles where it handles LOADS, PILOTS, GIs and LLIs
 *              Features that can be configured as ENABLED/DISABLED in Hbl_prv.h:
 *              HBL_USES_EXPANSION_GIS                  - Enable use of API012Exp to handle Expansion GIs
 *              HBL_CROSS_LOADS_FEATURE                 - Enable Cross Load Feature - handled by CrossLoads module
 *              HBL_XLOAD_MASTER_RELAY_SUPPORT          - Enable Master Relay Cross Load feature - HBL_CROSS_LOADS_FEATURE should be ENABLED
 *              HBL_XLOAD_SWITCH_RELAY_SUPPORT          - Enable Master Relay Cross Load feature - HBL_CROSS_LOADS_FEATURE should be ENABLED
 *              HBL_XLOAD_REVERSE_SWITCH_RELAY_SUPPORT  - Enable Master Relay Cross Load feature - HBL_CROSS_LOADS_FEATURE should be ENABLED
 *              HBL_XLOAD_TRANSITION_RELAY_SUPPORT      - Enable Master Relay Cross Load feature - HBL_CROSS_LOADS_FEATURE should be ENABLED
 *              HBL_LOADS_QUEUE_FEATURE                 - Enable Loads Queue feature to get commands from Api005 and queue in SRHbl__Process()
 *              HBL_LOADS_QUEUE_CALLED_FROM_HBL         - (ENABLED by default) Perform all queue commands into Hbl module
 *              HBL_USE_DIRECT_LOAD_OVERRIDE            - (ENABLED by default) Allows Direct Load configuration using HBL_DIRECT_LOAD[] array;
 *                                                          Load ID listed there will be forced as Direct Loads; those not listed there
 *                                                          and with no Load Handler should not have Load Parameters to be configured as Direct Loads.
 *              HBL_KEEP_LOADS_OFF_FEATURE              - (DISABLED by default) Feature to set Loads requests OFF when a fast reaction is required,
 *                                                          e.g. a Brownout condition has been detected.
 *              HBL_KEEP_LOADS_OFF_PRIORITY             - Priority level to consider for Keep Loads OFF: all loads set with a priority lower than it will be forced OFF.
 *                                                          Default is minimum Class B (SRHBL_PRIORITY_CLASSB_LO)
 *                                                          If it set higher, it will be limited to minimum Class B value anyway.
 *              HBL_KEEP_LOADS_OFF_EXCLUDE_FEATURE      - (DISABLED by default) Allows to set a list of load ID to exclude from Keep Loads Off action at all.
 *              HBL_KEEP_LOADS_OFF_EXCLUDE_LIST_DEF     - Comma separated list of HBL Load ID to skip keep OFF process, regardless of Priority.
 *                                                          If not defined, all loads will be turned OFF.
 *              HBLACU__EXTINT_CALLBACK(channel)        - HANDLE WITH CARE! (Blank - no function called by default.) It allows to configure an ExtIntHandler
 *                                                          callback for ExtInterrupt events enabled but not related to ZeroCross - ZeroCross pin
 *                                                          will not call this function.
 *                                                          "channel" is the ExtInt channel that generated interrupt.
 *
 *	@defgroup	CLASS_B
 *
 *  @copyright  Copyright 2015 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SRSystemConfig.h"
#include "SRHbl.h"
#include "SRData.h"
#include "SRMain.h"
#include "SRException.h"

#ifndef SYSTEM_ENABLE_TASKSEQUENCE
#define SYSTEM_ENABLE_TASKSEQUENCE DISABLED
#endif

#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
#include "SRTaskSequence.h"
#endif

#if(SRPRODUCTMGR_FEATURE == ENABLED)
    #include "SRProductMgr.h"
#endif

#include "Hbl.h"
#include "SRHbl_prv.h"
#include "SettingFile.h"
#include "Crc8.h"
#include "VirtualPin.h"
#include "Timer.h"
#include "SystemConfig.h"
#include "Utilities.h"
#include "Hbl_prv.h"
#include "Mode.h"
#include "Convert.h"
#include "Log.h"
#include "ExtInterrupt.h"
#include "string.h"

#if (HBLACU_ZEROCROSS_FEATURE == ENABLED)
    #include "ZeroCross.h"
#endif

#if (HBL_USES_EXPANSION_GIS == ENABLED)
    #include "API012Exp.h"
#endif

//------- Cross Loads Management ------------------------------------------------
#ifndef HBL_CROSS_LOADS_FEATURE
    #define HBL_CROSS_LOADS_FEATURE DISABLED
#endif

#if (HBL_CROSS_LOADS_FEATURE == ENABLED)
    #include "CrossLoads.h"
#endif

//------- Loads Queue Management ------------------------------------------------
#ifndef HBL_LOADS_QUEUE_FEATURE
#define HBL_LOADS_QUEUE_FEATURE DISABLED
#endif

#if (HBL_LOADS_QUEUE_FEATURE == ENABLED)
    #include "SetLoadQueue.h"
    #ifndef HBL_LOADS_QUEUE_CALLED_FROM_HBL
    #define HBL_LOADS_QUEUE_CALLED_FROM_HBL ENABLED
    #endif
#endif


#ifndef HBL_ZC_TIMEOUT_VALUE
    // If HBL_ZC_TIMEOUT_VALUE is not set, use a default value of 10 ( => 50 ms)
    #define HBL_ZC_TIMEOUT_VALUE      10
#endif

#if (HBLACU_ZEROCROSS_FEATURE == ENABLED)
    //ZC_DEBOUNCE_CYCLE is in cycles were each cycle is of 20ms(for 50Hz) or 16.67ms(for 60Hz)
    #define HBL_ZC_DEBOUNCE_VALUE     (ZC_DEBOUNCE_CYCLE * 4 + HBL_ZC_TIMEOUT_VALUE)
#else
    //ZC_DEBOUNCE_CYCLE is in cycles were each cycle is of 20ms(for 50Hz) or 16.67ms(for 60Hz)
    #define HBL_ZC_DEBOUNCE_VALUE     0
#endif


#ifndef HBL_KEEP_LOADS_OFF_FEATURE
    #define HBL_KEEP_LOADS_OFF_FEATURE              DISABLED
#endif

#ifndef HBL_KEEP_LOADS_OFF_EXCLUDE_FEATURE
    #define HBL_KEEP_LOADS_OFF_EXCLUDE_FEATURE      DISABLED
#endif

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------



//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define NO_PARAMETER_OFFSET                 0xFF
#define CRC8_INITIAL_SEED                   0xFF

#ifndef HBLACU__EXTINT_CALLBACK
#define HBLACU__EXTINT_CALLBACK(channel)
#endif

//------- Log Module messages ---------------------------------------
//! A list of log message IDs that are generated by this module.
typedef enum
{
    INVALID_GI_HANDLER_INDEX = 0,                                   //!< data: the gi index
    INVALID_LLI_HANDLER_INDEX,                                      //!< data: the gi index
    INVALID_LOAD_HANDLER_INDEX,                                     //!< data: the load index
    INVALID_PILOT_HANDLER_INDEX,                                    //!< data: the load index
    GI_ALLOCATION_FAILED,                                           //!< data: the gi index
    GI_INSTANCE_INITIALIZATION_FAILED,                              //!< data: the gi index
    LLI_ALLOCATION_FAILED,                                          //!< data: the lli index
	LLI_INSTANCE_INITIALIZATION_FAILED,								//!< data: the lli index
	LLI_INPUTCAPTURE_ALLOCATION_FAILED,								//!< data: the lli index
    LOAD_ALLOCATION_FAILED,                                         //!< data: the load index
    LOAD_INSTANCE_INITIALIZATION_FAILED,                            //!< data: the load index
	LOAD_OUTPUTCOMPARE_ALLOCATION_FAILED,                           //!< data: the load index value
    NUM_OF_SETTING_GIS_EXCEEDEDS_HBL_MAX_NUM_GENERICINPUT,          //!< data: the number of GIs defined in the GESE setting file
    NUM_OF_SETTING_LLIS_EXCEEDEDS_HBL_MAX_NUM_LOWLEVELINPUT,        //!< data: the number of LLIs defined in the GESE setting
    NUM_OF_SETTING_LOADS_EXCEEDEDS_HBL_MAX_NUM_LOADS,               //!< data: the number of Loads defined in the GESE setting
    PILOT_ALLOCATION_FAILED,                                        //!< data: the load index
    PILOT_INSTANCE_INITIALIZATION_FAILED,                           //!< data: the load index
    LOAD_DIRECT_HANDLER_REMAP,                                      //!< data: the load index
} MODULE_HBL_LOG_MESSAGE_ID_TYPE;

#define MODULE_NAME MODULE_HBL      //!< The name of this module for use by the Log module.


//------- Loads Management -------------------------------------------
#ifndef HBL_USE_DIRECT_LOAD_OVERRIDE
#define HBL_USE_DIRECT_LOAD_OVERRIDE    ENABLED
#endif

#define HBL_NUM_PILOT_HANDLERS  (sizeof(HBL_PILOT_HANDLERS) / sizeof(HBL_PILOT_HANDLER_HOOK_TYPE))
#define HBL_NUM_LOADS_HANDLERS  (sizeof(HBL_LOAD_HANDLERS) / sizeof(HBL_LOAD_HANDLER_HOOK_TYPE))
#define HBL_NUM_LOAD_PARAMETERS 8   //!< This is the number of parameters in the setting file for a load

#if (HBL_LOAD_HANDLER_FEATURE == ENABLED)
    #if (defined(SRHBL_LOAD_HANDLERS_LIST) || defined(HBL_LOAD_HANDLERS_LIST))
        // Load Handlers table
        static const HBL_LOAD_HANDLER_HOOK_TYPE HBL_LOAD_HANDLERS[] =
        {
            #ifdef HBL_LOAD_HANDLERS_LIST
            HBL_LOAD_HANDLERS_LIST,
            #endif
            #ifdef SRHBL_LOAD_HANDLERS_LIST
            SRHBL_LOAD_HANDLERS_LIST
            #endif
        };
    #else
        #error "Define at least one of SRHBL_LOAD_HANDLERS_LIST or HBL_LOAD_HANDLERS_LIST or disable HBL_LOAD_HANDLER_FEATURE"
    #endif

    #if(HBL_USE_DIRECT_LOAD_OVERRIDE == ENABLED)
        #define HBL_NUM_DIRECT_LOAD     (sizeof(HBL_DIRECT_LOAD) / sizeof(HBL_LOAD_ENUM_TYPE))

        #if (defined(SRHBL_DIRECT_LOAD_LIST) || defined(HBL_DIRECT_LOAD_LIST))
            // Direct Loads table
            static HBL_LOAD_ENUM_TYPE const HBL_DIRECT_LOAD[] =
            {
                #ifdef HBL_DIRECT_LOAD_LIST
                    HBL_DIRECT_LOAD_LIST,
                #endif
                #ifdef SRHBL_DIRECT_LOAD_LIST
                    SRHBL_DIRECT_LOAD_LIST
                #endif
            };
        #else
            #error "Define at least one of SRHBL_DIRECT_LOAD_LIST or HBL_DIRECT_LOAD_LIST or disable HBL_USE_DIRECT_LOAD_OVERRIDE"
        #endif

    #endif
#endif

#if (defined(SRHBL_PILOT_HANDLERS_LIST) || defined(HBL_PILOT_HANDLERS_LIST))
    // Pilot Handlers table
    static const HBL_PILOT_HANDLER_HOOK_TYPE HBL_PILOT_HANDLERS[] =
    {
        #ifdef HBL_PILOT_HANDLERS_LIST
            HBL_PILOT_HANDLERS_LIST,
        #endif
        #ifdef SRHBL_PILOT_HANDLERS_LIST
            SRHBL_PILOT_HANDLERS_LIST
        #endif
    };
#else
    #error "Define at least one of SRHBL_PILOT_HANDLERS_LIST or HBL_PILOT_HANDLERS_LIST"
#endif

//------- Keep OFF Loads Feature -------------------------------------------
#if(HBL_KEEP_LOADS_OFF_FEATURE == ENABLED)

    #ifndef HBL_KEEP_LOADS_OFF_PRIORITY
        #define HBL_KEEP_LOADS_OFF_PRIORITY     SRHBL_PRIORITY_CLASSB_LO
    #endif

    #if (HBL_KEEP_LOADS_OFF_EXCLUDE_FEATURE == ENABLED)
        static uint8 const Hbl_Load_Exclude_List[] = { HBL_KEEP_LOADS_OFF_EXCLUDE_LIST_DEF };
        static uint8 Hbl_Exclude_Loads_Off[HBL_MAX_NUM_LOADS];
        static uint8 N_Hbl_Exclude_Loads_Off[HBL_MAX_NUM_LOADS];
    #endif

    static uint8 Hbl_Keep_Loads_Off;
    static uint8 Hbl_Load_Requests_Progress;
    static uint8 N_Hbl_Keep_Loads_Off;
    static uint8 N_Hbl_Load_Requests_Progress;

    void KeepLoadsOffInitialize(void);
    void KeepLoadsOffHandler(void);
#endif

/**
 * Pilot OC Structure
 */
typedef struct
{
    TIMER_TYPE Timer;
    TIMER_CHANNEL_TYPE Timer_Channel;
    uint8 Pilot_Cfg_Index;
}HBL_PILOT_OC_TYPE;

static const uint8 HBL_FLUSH_BUFFER[] = {0,0,0,0,0,0,0,0,0,0};

static HBL_LOAD_TYPE Hbl_Load_Cfg[HBL_MAX_NUM_LOADS];
static uint8 Hbl_Load_Current_Priority[HBL_MAX_NUM_LOADS];
static uint8 Hbl_Num_Loads;
static uint8 Hbl_Load_Cfg_Crc[HBL_MAX_NUM_LOADS];
static uint8 N_Hbl_Num_Loads;
static uint8 N_Hbl_Load_Current_Priority[HBL_MAX_NUM_LOADS];

#if ((HBL_LOADS_QUEUE_FEATURE == ENABLED) && (HBL_LOADS_QUEUE_CALLED_FROM_HBL == ENABLED))
    static BOOL_TYPE Hbl_Is_Load_Queue_Running;
#endif

#if (HBL_MAX_NUM_OUTPUTCOMPARES_USED > 0)
	static uint8 Hbl_Num_OCProcess_Pilot;
    static HBL_PILOT_OC_TYPE Hbl_Pilot_OCProcess_List[HBL_MAX_NUM_OUTPUTCOMPARES_USED];
    static uint8 N_Hbl_Num_OCProcess_Pilot;
    static HBL_PILOT_OC_TYPE N_Hbl_Pilot_OCProcess_List[HBL_MAX_NUM_OUTPUTCOMPARES_USED];
#endif

//------- Consistency Management -------------------------------------------
typedef struct
{
    struct {
        uint8 N_Fbk:1;
        uint8 N_States:2;
        uint8 Unused0:5;
        uint8 Conv_Data_Size;
        HBL_LLI_ENUM_TYPE Type_Fbk0;
        uint8 Pos_Fbk0;
    } Config;
    union
    {
        struct {
            uint8 convert_data;
        } Single_Fb;
        struct {
            HBL_LLI_ENUM_TYPE Type_Fbk1;
            uint8 Pos_Fbk1;
            uint8 convert_data;
        } Double_Fb;
    } Fb_Header;
} HBL_CONSISTENCY_PARAM_HEAD_TYPE;

static uint8 Hbl_Load_Consistency[HBL_MAX_NUM_LOADS];
static uint8 Hbl_Load_Consistency_LLI_SeqID[HBL_MAX_NUM_LOADS][2];
static uint8 N_Hbl_Load_Consistency[HBL_MAX_NUM_LOADS];
static uint8 N_Hbl_Load_Consistency_LLI_SeqID[HBL_MAX_NUM_LOADS][2];

//------- Low Level Input Management ------------------------------------------------
#define HBL_NUM_LLI_HANDLERS  (sizeof(HBL_LLI_HANDLERS) / sizeof(HBL_LLI_HANDLER_HOOK_TYPE))

#if (defined(SRHBL_LLI_HANDLERS_LIST) || defined(HBL_LLI_HANDLERS_LIST))
    // Low Level Input Handlers table
    static HBL_LLI_HANDLER_HOOK_TYPE const HBL_LLI_HANDLERS[] =
    {
        #ifdef SRHBL_LLI_HANDLERS_LIST
            SRHBL_LLI_HANDLERS_LIST,
        #endif
        #ifdef HBL_LLI_HANDLERS_LIST
            HBL_LLI_HANDLERS_LIST
        #endif
    };
#else
    #error "Define at least one of SRHBL_LLI_HANDLERS_LIST or HBL_LLI_HANDLERS_LIST"
#endif

/**
 * LLI IC Structure
 */
PACKED typedef struct
{
    TIMER_TYPE Timer;
    TIMER_CHANNEL_TYPE Timer_Channel;
    uint8 LLI_Cfg_Index;
}HBL_LLI_IC_TYPE;

static uint8 Hbl_Number_LLI;
static HBL_LLI_TYPE Hbl_LLI_Cfg[HBL_MAX_NUM_LOWLEVELINPUT];
static uint8 N_Hbl_Number_LLI;
static uint8 Hbl_LLI_Cfg_Crc[HBL_MAX_NUM_LOWLEVELINPUT];

static uint8 Hbl_LLI_Sequence[HBL_MAX_NUM_LOWLEVELINPUT];
static uint8 Hbl_GI_Sequence[HBL_MAX_NUM_GENERICINPUT];
static uint8 N_Hbl_LLI_Sequence[HBL_MAX_NUM_LOWLEVELINPUT];
static uint8 N_Hbl_GI_Sequence[HBL_MAX_NUM_GENERICINPUT];

#if (HBL_MAX_NUM_INPUTCAPTURES_USED > 0)
	static uint8 Hbl_Num_ICProcess_LLI;
    static HBL_LLI_IC_TYPE Hbl_LLI_ICProcess_List[HBL_MAX_NUM_INPUTCAPTURES_USED];
    static uint8 N_Hbl_Num_ICProcess_LLI;
    static HBL_LLI_IC_TYPE N_Hbl_LLI_ICProcess_List[HBL_MAX_NUM_INPUTCAPTURES_USED];
#endif

#if (HBLACU_ZEROCROSS_FEATURE == ENABLED)
    static uint8 Hbl_ZC_Virtual_Pin;
#endif


//------- Generic Input Management ------------------------------------------------
#define HBL_NUM_GI_HANDLERS  (sizeof(HBL_GI_HANDLERS) / sizeof(HBL_GI_HANDLER_HOOK_TYPE))
#define HBL_NUM_GI_PARAMETERS 4   //!< This is the number of parameters in the setting file for a GI

#if (defined(SRHBL_GI_HANDLERS_LIST) || defined(HBL_GI_HANDLERS_LIST))
    // Generic Input Handlers table
    static HBL_GI_HANDLER_HOOK_TYPE const HBL_GI_HANDLERS[] =
    {
        #ifdef SRHBL_GI_HANDLERS_LIST
            SRHBL_GI_HANDLERS_LIST,
        #endif
        #ifdef HBL_GI_HANDLERS_LIST
            HBL_GI_HANDLERS_LIST
        #endif
    };
#else
    #error "Define at least one of SRHBL_GI_HANDLERS_LIST or HBL_GI_HANDLERS_LIST"
#endif

static uint8 Hbl_Number_GI;
static HBL_GI_TYPE Hbl_GI_Cfg[HBL_MAX_NUM_GENERICINPUT];
static uint8 N_Hbl_Number_GI;
static uint8 Hbl_GI_Cfg_Crc[HBL_MAX_NUM_GENERICINPUT];

#if (HBL_USES_EXPANSION_GIS == ENABLED)
    //------- Expansion GI Data Type management ----------------------------------------
    uint8   Hbl_GI_Exp_Last_Seq_ID[HBL_MAX_NUM_GENERICINPUT];
#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
#if (HBLACU_ZEROCROSS_FEATURE == ENABLED)
    static void ZeroCrossHandler(ZEROCROSS_EVENT_TYPE angle_event);
#endif

static uint8 GetPilotHandler(HBL_PILOT_ENUM_TYPE  pilot);
static uint8 GetLoadHandler(HBL_LOAD_ENUM_TYPE  load_id);
static uint8 GetGenericInputHandler(HBL_GENERIC_INPUT_ENUM_TYPE  generic_input);
static uint8 GetLowLevelHandler(HBL_LLI_ENUM_TYPE  input);
static uint8 ConsistencyEval(uint8 load_index, HBL_LOAD_STATUS_TYPE load_value, BOOL_TYPE update_seq);
static void ConsistencyDoUpdate(uint8 load_index);
static uint16 ConsistencyGetFeedbackValue(HBL_LLI_ENUM_TYPE type, uint8 position);

static void LoadsInitialize(void);

static void LLIInitialize(void);
static void GIInitialize(void);
static BOOL_TYPE CheckLoadHandlers(uint8 index);
static void UpdateLLISequence(uint8 index);
static void UpdateGISequence(uint8 index);
static void ConsistencyInitialize(void);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief It Initializes the module Hbl and its variables
 *
 */
void Hbl__Initialize(void)
{
    Timer__Initialize();
    ExtInterrupt__Initialize();
    #if (HBLACU_ZEROCROSS_FEATURE == ENABLED)
        Hbl_ZC_Virtual_Pin = HBL_INDEX_INVALID;
        ZeroCross__Initialize();
    #endif


    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_INIT, SRTASKSEQUENCE_APPL_INIT_HBL_INITIALIZE);
    #endif

    LLIInitialize();
    GIInitialize();
    ConsistencyInitialize();

	#if (HBLACU_ZEROCROSS_FEATURE == ENABLED)
    	// ZeroCross Configuration
		if (Hbl_ZC_Virtual_Pin < HBL_INDEX_INVALID)
		{
			ZeroCross__Config(  (EXTINT_CHNL_TYPE)VIRTUALPIN_TABLE[Hbl_ZC_Virtual_Pin].device,
								VIRTUALPIN_TABLE[Hbl_ZC_Virtual_Pin].port,
								VIRTUALPIN_TABLE[Hbl_ZC_Virtual_Pin].pin,
								VIRTUALPIN_TABLE[Hbl_ZC_Virtual_Pin].pin_remap,
								(TIMER_TYPE)VIRTUALPIN_TABLE[Hbl_ZC_Virtual_Pin].param0,
								(TIMER_CHANNEL_TYPE)VIRTUALPIN_TABLE[Hbl_ZC_Virtual_Pin].param1);

		}
	#endif

    LoadsInitialize();
    #if (HBL_CROSS_LOADS_FEATURE == ENABLED)
        CrossLoads__Initialize();
    #endif
    SRHbl__ResetLoadPriority();
    Hbl__FlushRequestBuffer();

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_INIT, SRTASKSEQUENCE_APPL_INIT_HBL_IO_INITIALIZE);
    #endif

    #if(SRPRODUCTMGR_FEATURE == ENABLED)
        SRProductMgr__Initialize();
        #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
            SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_INIT, SRTASKSEQUENCE_APPL_INIT_SRPRODUCTMGR_DONE);
        #endif
    #endif

}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Method to be invoke to check the presence of the ZeroCross signal.
 * @return ZeroCros signal
 *   @retval TRUE = PRESENT
 *   @retval FALSE - ABSENT
 */
BOOL_TYPE Hbl__IsZeroCrossPresent(void)
{
    #if (HBLACU_ZEROCROSS_FEATURE == ENABLED)
        return ((BOOL_TYPE)(ZeroCross__GetFrequency() != 0));
    #else
        return FALSE;
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Method to be invoke to get ZeroCross full cycle count.
 * @return ZeroCros cycle count value, 0 if not present
 */
uint32 Hbl__GetZeroCrossCycleCount(void)
{
    #if (HBLACU_ZEROCROSS_FEATURE == ENABLED)
        return ZeroCross__GetCycleCount();
    #else
        return 0;
    #endif
}
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Handler to be executed every 1ms out of the SystemTick timer
 */
void Hbl__Handler1ms(void)
{
    uint8 index;
    uint8 idx;

    if (SRDATA_OK == SRData__CheckByte(&Hbl_Number_LLI, &N_Hbl_Number_LLI))
    {
        for (index=0; index < Hbl_Number_LLI; index++)
        {
            idx = Hbl_LLI_Cfg[index].LLI_Handler_Index;

            if ( (idx < HBL_NUM_LLI_HANDLERS) &&
                (HBL_LLI_HANDLERS[idx].OneMSHandler != NULL))
            {
                if (HBL_LLI_HANDLERS[idx].OneMSHandler(&Hbl_LLI_Cfg[index]) == TRUE)
                {
                    UpdateLLISequence(index);
                }
            }
        }
    }

    for (index=0; index < Hbl_Num_Loads; index++)
    {
        idx = Hbl_Load_Cfg[index].Pilot_Handler_Index;

        if (CheckLoadHandlers(index) &&
            (HBL_PILOT_HANDLERS[idx].OneMsHandler != NULL))
        {
            HBL_PILOT_HANDLERS[idx].OneMsHandler(&Hbl_Load_Cfg[index]);
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Handler to be executed once every 5ms to 10ms.
 */
void Hbl__AsynchHandler(void)
{
    uint8 index;
    uint8 idx;
    uint8 idx_lli;
    uint8 * lli_data;

    //---------------------------------------------------------Low Level Management -------------------------------------------------------------------------
    if (SRDATA_OK == SRData__CheckByte(&Hbl_Number_LLI, &N_Hbl_Number_LLI))
    {
        for (index=0; index < Hbl_Number_LLI; index++)
        {
            idx = Hbl_LLI_Cfg[index].LLI_Handler_Index;
            if ((idx < HBL_NUM_LLI_HANDLERS) &&
                (HBL_LLI_HANDLERS[idx].AsynchProcess != NULL))
            {
                if (HBL_LLI_HANDLERS[idx].AsynchProcess(&Hbl_LLI_Cfg[index]) == TRUE)
                {
                    UpdateLLISequence(index);
                }
            }
        }
    }

    //---------------------------------------------------------Generic Input Management -------------------------------------------------------------------------
    if (SRDATA_OK == SRData__CheckByte(&Hbl_Number_GI, &N_Hbl_Number_GI))
    {
        for (index=0; index < Hbl_Number_GI ; index++)
        {
            idx = Hbl_GI_Cfg[index].GI_Handler_Index;
            idx_lli = GetLowLevelHandler(Hbl_GI_Cfg[index].LLI_ID);

            if ((idx < HBL_NUM_GI_HANDLERS) &&
                (idx_lli < HBL_NUM_LLI_HANDLERS) &&
                (HBL_GI_HANDLERS[idx].AsynchProcess != NULL) &&
                (HBL_LLI_HANDLERS[idx_lli].Get != NULL))
            {
                lli_data = Hbl__GetLLIDataByIndex(Hbl_GI_Cfg[index].LLI_Config_Index);
                if(lli_data != NULL)
                {
                    #if (HBL_USES_EXPANSION_GIS == ENABLED)
                        //------- Expansion GI Sequence ID check to update GI and LLI ----------------------------------------
                        if(Hbl_LLI_Cfg[Hbl_GI_Cfg[index].LLI_Config_Index].LLI_ID == HBL_LLI_EXPANSION)
                        {
                            uint8 exp_idx = *lli_data;
                            uint8 api012_seq_id = API012Exp__GetGISequenceIDByIndex(exp_idx);
                            if(Hbl_GI_Exp_Last_Seq_ID[index] != api012_seq_id)
                            {
                                UpdateLLISequence(Hbl_GI_Cfg[index].LLI_Config_Index);
                                UpdateGISequence(index);
                                Hbl_GI_Exp_Last_Seq_ID[index] = api012_seq_id;
                            }
                        }
                        else
                    #endif
                    {
                        if(HBL_GI_HANDLERS[idx].AsynchProcess(&Hbl_GI_Cfg[index],lli_data, Hbl_LLI_Sequence[Hbl_GI_Cfg[index].LLI_Config_Index]) == TRUE)
                        {
                            UpdateGISequence(index);
                        }
                    }
                }
            }
        }
    }

    //------------------------------------------------------------ Loads Management ------------------------------------------------------------------------
    #if(HBL_KEEP_LOADS_OFF_FEATURE == ENABLED)
        // Call KeepLoadsOffHandler to speed up loads to be set OFF
        KeepLoadsOffHandler();
    #endif

    for (index=0; index < Hbl_Num_Loads; index++)
    {
        if (CheckLoadHandlers(index))
        {
            //------------------------------------------------------------ Consistency Management ------------------------------------------------------------------------
            if (Hbl_Load_Cfg[index].Feedback_Params_Offset != 0xFF)
            {
                ConsistencyDoUpdate(index);
            }

            idx = Hbl_Load_Cfg[index].Load_Handler_Index;

            #if (HBL_LOAD_HANDLER_FEATURE == ENABLED)

                // IF this is a valid Load
                if (idx != HBL_LOAD_INDEX_DIRECT)
                {
                    // If this is not a direct load
                    if (HBL_LOAD_HANDLERS[idx].AsynchProcess != NULL)
                    {
                        HBL_LOAD_HANDLERS[idx].AsynchProcess(&Hbl_Load_Cfg[index],(HBL_PILOT_HANDLER_HOOK_TYPE *)&HBL_PILOT_HANDLERS[Hbl_Load_Cfg[index].Pilot_Handler_Index]); //lint !e929 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                    }
                }
                else
            #endif
            {
                idx = Hbl_Load_Cfg[index].Pilot_Handler_Index;
                // For direct load it drives straight to the Pilot
                if (HBL_PILOT_HANDLERS[idx].AsynchProcess != NULL)
                {
                    HBL_PILOT_HANDLERS[idx].AsynchProcess(&Hbl_Load_Cfg[index]);
                }
            }
        }
    }

    #if (HBLACU_ZEROCROSS_FEATURE == ENABLED)
        //--------ZeroCross Async Handler and Angle event simulation - only if ZeroCross pin has not been configured---------
        ZeroCrossHandler(ZeroCross__AsyncHandler());
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Empty Handler for backward compatibility; - it was reading analog channels which is called event based from the analog Thread.
 */
void Hbl__AnalogHandler(void)
{

}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method is called from the Extern Interrupt Handle to handle the EXT interrupts linked to Hbl (For instance ZeroCross).
 * @param channel
 */
void Hbl__ExtIntHandler(uint8 channel)
{
    #if ((HBLACU_ZEROCROSS_FEATURE == ENABLED) && (ZEROCROSS_POLLING_MODE == DISABLED))
        if ((Hbl_ZC_Virtual_Pin < HBL_INDEX_INVALID) &&
            ((EXTINT_CHNL_TYPE)channel == (EXTINT_CHNL_TYPE)VIRTUALPIN_TABLE[Hbl_ZC_Virtual_Pin].device))
        {
            ZeroCross__SignalEvent();
        }
        else
    #endif
    {
        HBLACU__EXTINT_CALLBACK(channel);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method is called from the Fast Handler execution (Timer Interrupt Tick < 1ms) to handle ZeroCross events
 * @param timer
 * @param channel
 */
void Hbl__FastHandler(void)
{
    #if (HBLACU_ZEROCROSS_FEATURE == ENABLED)
        if(Hbl_ZC_Virtual_Pin < HBL_INDEX_INVALID)
        {
            ZeroCrossHandler(ZeroCross__FastHandler());
        }
        #if ((FAST_HANDLER_FEATURE == ENABLED) && (ZEROCROSS_GPIO_REQUEST_NUMBER > 0))
            ZeroCross__GpioRequestHandler();
        #endif
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method is called from the Output compare Interrupt Handle to handle the Output Compare interrupts linked to Hbl (For instance ZeroCross).
 * @param timer
 * @param channel
 */
void Hbl__OCIntHandler(uint8 timer, uint8 channel)
{

    #if ((HBLACU_ZEROCROSS_FEATURE == ENABLED) && (ZEROCROSS_POLLING_MODE == DISABLED))
        if ((Hbl_ZC_Virtual_Pin < HBL_INDEX_INVALID) &&
            ((TIMER_TYPE)timer == (TIMER_TYPE)VIRTUALPIN_TABLE[Hbl_ZC_Virtual_Pin].param0) &&
            ((TIMER_CHANNEL_TYPE)channel == (TIMER_CHANNEL_TYPE)VIRTUALPIN_TABLE[Hbl_ZC_Virtual_Pin].param1))
        {
            ZeroCrossHandler(ZeroCross__AngleEvent());
        }
        else
    #endif
    {
        #if (HBL_MAX_NUM_OUTPUTCOMPARES_USED > 0)
            if (SRDATA_OK == SRData__CheckByte(&Hbl_Num_OCProcess_Pilot, &N_Hbl_Num_OCProcess_Pilot))
            {
                uint8 oc_index;
                uint8 idx;
                HBL_PILOT_OC_TYPE *oc_ptr;

                // Process the Output Compare interrupts
                for (oc_index = 0; oc_index < Hbl_Num_OCProcess_Pilot; oc_index++)
                {
                    oc_ptr = &Hbl_Pilot_OCProcess_List[oc_index];

                    if ((oc_ptr->Timer == timer) &&
                        (oc_ptr->Timer_Channel == channel) &&
                        (oc_ptr->Pilot_Cfg_Index < Hbl_Num_Loads))
                    {
                        idx = Hbl_Load_Cfg[oc_ptr->Pilot_Cfg_Index].Pilot_Handler_Index;
                        if ((idx < HBL_NUM_PILOT_HANDLERS) &&
                            (HBL_PILOT_HANDLERS[idx].OCProcess != NULL))
                        {
                            HBL_PILOT_HANDLERS[idx].OCProcess(&Hbl_Load_Cfg[oc_ptr->Pilot_Cfg_Index]);
                        }
                        break;
                    }
                }
            }
        #endif
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Input Capture Handler.
 * @details This method is invoked when an input capture interrupt is detected.
 * @param timer
 * @param channel
 */
void Hbl__ICIntHandler(uint8 timer, uint8 channel)
{
    #if (HBL_MAX_NUM_INPUTCAPTURES_USED > 0)
        uint8 ic_index;

        if (SRDATA_OK == SRData__CheckByte(&Hbl_Num_ICProcess_LLI, &N_Hbl_Num_ICProcess_LLI))
        {
            for (ic_index = 0; ic_index < Hbl_Num_ICProcess_LLI; ic_index++)
            {
                HBL_LLI_IC_TYPE *ic_ptr = &Hbl_LLI_ICProcess_List[ic_index];

                if ((ic_ptr->Timer == timer) &&
                    (ic_ptr->Timer_Channel == channel) &&
                    (ic_ptr->LLI_Cfg_Index < Hbl_Number_LLI) &&
                    (Hbl_LLI_Cfg[ic_ptr->LLI_Cfg_Index].LLI_Handler_Index < HBL_NUM_LLI_HANDLERS) &&
                    (HBL_LLI_HANDLERS[Hbl_LLI_Cfg[ic_ptr->LLI_Cfg_Index].LLI_Handler_Index].ICProcess != NULL))
                {
                    if (HBL_LLI_HANDLERS[Hbl_LLI_Cfg[ic_ptr->LLI_Cfg_Index].LLI_Handler_Index].ICProcess(&Hbl_LLI_Cfg[ic_ptr->LLI_Cfg_Index]) == TRUE)
                    {
                        UpdateLLISequence(ic_ptr->LLI_Cfg_Index);
                    }
                    break;
                }
            }
        }
    #endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns a pointer to the HBL_FLUSH_BUFFER.
 *
 * @return	pointer to the HBL_FLUSH_BUFFER
 */
const uint8 * Hbl__GetFlushBuffer(void)
{
	return ( HBL_FLUSH_BUFFER );
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method clears all bytes of the load request buffer.
 */
void Hbl__FlushRequestBuffer(void)
{
	for (uint8 index=0; index < Hbl_Num_Loads; index++)
	{
		(void)SRHbl__SetLoadRequestByIndex(index, SRHBL_PRIORITY_MINIMUM, (uint8 *)HBL_FLUSH_BUFFER);   //lint !e926 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns the number of loads.
 * @return Number of Loads.
 */
uint8 Hbl__GetNumLoads(void)
{
    return (Hbl_Num_Loads);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method is used to retrieve The load Index of a given Load ID
 * @param load_id
 * @return Load index
 */
uint8 Hbl__GetLoadIndexByLoadID(HBL_LOAD_ENUM_TYPE load_id)
{
    uint8 retval;
    uint8 index;
    retval = HBL_INDEX_INVALID;
    for (index=0; index < Hbl_Num_Loads; index++)
    {
        if ( Hbl_Load_Cfg[index].Load_Id == load_id)
        {
            retval = index;
            break;
        }
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method is used to retrieve Load ID for a given Load index.
 * @param load_index
 * @return Load ID
 */
HBL_LOAD_ENUM_TYPE Hbl__GetLoadIDByIndex(uint8 load_index)
{
    if (load_index < Hbl_Num_Loads)
    {
        return (Hbl_Load_Cfg[load_index].Load_Id);
    }
    else
    {
        return (HBL_LOAD_INVALID);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method extracts the Pilot Type based on the Load Index
 * @param load_index Defines the enumerations of the load.
 * @return HBL_PILOT_ENUM_TYPE
 */
HBL_PILOT_ENUM_TYPE Hbl__GetPilotIDByIndex(uint8 load_index)
{
    if (load_index < Hbl_Num_Loads)
    {
        return (Hbl_Load_Cfg[load_index].Pilot_Type);
    }
    else
    {
        return (HBL_PILOT_INVALID);
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method is used to Set the request buffer from a given load by index;
 * @param load_index
 * @param priority
 * @param data
 * @return result of the request
 *  @retval PASS  if the method was able to set the data.
 *  @retval FAIL if the method wasn't able to set the data.
 */
PASS_FAIL_TYPE Hbl__SetLoadRequestByIndex(uint8 load_index, HBL_PRIORITY_TYPE priority, uint8 * data)
{
    PASS_FAIL_TYPE retval;
    retval = FAIL;

    if (priority < SRHBL_PRIORITY_CLASSB_LO)
    {
        #if ((HBL_LOADS_QUEUE_FEATURE == ENABLED) && (HBL_LOADS_QUEUE_CALLED_FROM_HBL == ENABLED))
            if ((Mode__IsPCControlEngaged() == TRUE) &&
                (Hbl_Is_Load_Queue_Running == FALSE) &&
                (priority == HBL_PRIORITY_PC_CONTROL))
            {
                retval = SetLoadQueue__SetLoadRequestByIndex(load_index, priority, data);
            }
            else
        #endif
        {
            retval = SRHbl__SetLoadRequestByIndex(load_index, priority, data);
        }
    }

    return (retval);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method sets the data related to the specific Load into the Load Request Buffer
 * @param load
 * @param priority
 * @param data
 * @return result of the request
 *  @retval PASS  if the method was able to set the data.
 *  @retval FAIL if the method wasn't able to set the data.
 */
PASS_FAIL_TYPE Hbl__SetLoadRequestByLoadID(HBL_LOAD_ENUM_TYPE load_id, HBL_PRIORITY_TYPE priority, uint8 * data)
{
    return( Hbl__SetLoadRequestByIndex(Hbl__GetLoadIndexByLoadID(load_id),priority,data));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   This method exposes the status of a Load by its index
 * @details If the load doesn't support the GetStatus() method it will return Processed buffer.
 *          HBL_LOAD_INDEX_DIRECT loads will return Pilot Status (or Processed if GetStatus() is not supported)
 *          If the load is not present, it will return NULL.
 *          The return has to be tested against NULL before be used.
 *
 * @param   load_index  is a valid Load index within  the range defined.
 */
void *Hbl__GetLoadStatusByIndex(uint8 load_index)
{
    void * retval = NULL;

    if (CheckLoadHandlers(load_index))
    {
        HBL_LOAD_TYPE *load_ptr = (void *)&Hbl_Load_Cfg[load_index];

        if(load_ptr->Pilot_Data_Index != HBL_INDEX_INVALID)
        {
            uint8 idx = load_ptr->Pilot_Handler_Index;
            HBL_PILOT_HANDLER_HOOK_TYPE *pilot_hook_ptr = (void *) &HBL_PILOT_HANDLERS[idx];
            #if (HBL_LOAD_HANDLER_FEATURE == ENABLED)
                if ((load_ptr->Load_Handler_Index != HBL_LOAD_INDEX_DIRECT) &&
                    (load_ptr->Load_Data_Index != HBL_INDEX_INVALID))
                {
                    HBL_LOAD_HANDLER_HOOK_TYPE *load_hook_ptr = (void *) &HBL_LOAD_HANDLERS[load_ptr->Load_Handler_Index];
                    if (load_hook_ptr->GetStatus != NULL)
                    {
                        retval = load_hook_ptr->GetStatus(load_ptr, pilot_hook_ptr);
                    }
                    else
                    {
                        retval = load_hook_ptr->GetProcessed(load_ptr, pilot_hook_ptr);
                    }
                }
                else
            #endif
            {
                if (pilot_hook_ptr->GetStatus != NULL)
                {
                    retval = pilot_hook_ptr->GetStatus(load_ptr);
                }
                else
                {
                    retval = pilot_hook_ptr->GetProcessed(load_ptr);
                }
            }
        }
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   This method exposes the Load Status by the Load ID.
 * @details If the load doesn't support the GetStatus() method it will return Processed buffer.
 *          HBL_LOAD_INDEX_DIRECT loads will return Pilot Status (or Processed if GetStatus() is not supported)
 *          If the load is not present, it will return NULL.
 *          The return has to be tested against NULL before be used.
 *
 * @param   load_id     existing load ID
 */
void *  Hbl__GetLoadStatusByLoadID(HBL_LOAD_ENUM_TYPE load_id)
{
    return( Hbl__GetLoadStatusByIndex(Hbl__GetLoadIndexByLoadID(load_id)));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   This method exposes the status of a Pilot associated to Load by its index
 * @details If the Pilot doesn't support the GetStatus() method it will return Processed buffer.
 *          Except for specific needs, recommended to use GetLoadStatus() methods instead.
 *          If the load is not present, it will return NULL.
 *          The return has to be tested against NULL before be used.
 *
 * @param   load_index  is a valid Load index within  the range defined.
 */
void *Hbl__GetPilotStatusByLoadIndex(uint8 load_index)
{
    void *retval = NULL;

    if (CheckLoadHandlers(load_index))
    {
        HBL_LOAD_TYPE *load_ptr = (void *)&Hbl_Load_Cfg[load_index];
        uint8 idx = load_ptr->Pilot_Handler_Index;

        if (load_ptr->Pilot_Data_Index != HBL_INDEX_INVALID)
        {
            HBL_PILOT_HANDLER_HOOK_TYPE *pilot_hook_ptr = (void *) &HBL_PILOT_HANDLERS[idx];
            if (pilot_hook_ptr->GetStatus != NULL)
            {
                retval = pilot_hook_ptr->GetStatus(load_ptr);
            }
            else
            {
                retval = pilot_hook_ptr->GetProcessed(load_ptr);
            }
        }
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method exposes the Pilot Status associated to a Load by the Load ID.
 * @details If the Pilot doesn't support the GetStatus() method it will return Processed buffer.
 *          Except for specific needs, recommended to use GetLoadStatus() methods instead.
 *          If the load is not present, it will return NULL.
 *          The return has to be tested against NULL before be used.
 *
 * @param load_id existing load ID
 */
void *  Hbl__GetPilotStatusByLoadID(HBL_LOAD_ENUM_TYPE load_id)
{
    return( Hbl__GetPilotStatusByLoadIndex(Hbl__GetLoadIndexByLoadID(load_id)));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method extract the size of the data array passed/retrieve as data for the given load.
 * @param load_index Index for the specific load.
 * @return size of the data used by the load.
 */
uint8 Hbl__GetLoadDataSize(uint8 load_index)
{
    if (CheckLoadHandlers(load_index))
    {
#if (HBL_LOAD_HANDLER_FEATURE == ENABLED)
        if (Hbl_Load_Cfg[load_index].Load_Handler_Index != HBL_LOAD_INDEX_DIRECT)

        {
            return (HBL_LOAD_HANDLERS[Hbl_Load_Cfg[load_index].Load_Handler_Index].Parameter_Size);
        }
        else
#endif
        {
            return (HBL_PILOT_HANDLERS[Hbl_Load_Cfg[load_index].Pilot_Handler_Index].Parameter_Size);
        }
    }
    else
    {
        return (0);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns the data related to the load in the Request buffer.
 * @param load_index
 * @return pointer to the data if request was handeld. NULL if not possibe
 */
void * Hbl__GetRequestedLoad(uint8 load_index)
{
    void * retval = NULL;
    void *load_data_ptr;
    uint8 load_data_size;
    uint8 idx;


    if (CheckLoadHandlers(load_index))
    {
#if (HBL_LOAD_HANDLER_FEATURE == ENABLED)
        idx = Hbl_Load_Cfg[load_index].Load_Handler_Index;
        if (idx != HBL_LOAD_INDEX_DIRECT)
        {
            load_data_size = HBL_LOAD_HANDLERS[idx].Parameter_Size;
            load_data_ptr = HBL_LOAD_HANDLERS[idx].GetRequested(&Hbl_Load_Cfg[load_index]);

            if ((load_data_ptr != NULL) &&
                (load_data_size > 0))
                {
                    retval = load_data_ptr;
                }
        }
        else

#endif
        {
            idx = Hbl_Load_Cfg[load_index].Pilot_Handler_Index;
            load_data_size = HBL_PILOT_HANDLERS[idx].Parameter_Size;
            load_data_ptr = HBL_PILOT_HANDLERS[idx].GetRequested(&Hbl_Load_Cfg[load_index]);;

            if ((load_data_ptr != NULL) &&
                (load_data_size > 0))
                {
                    retval = load_data_ptr;
                }
        }
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns the data related to the load in the processed buffer which has been applied to the loads.
 * @param load_id ID for the specific load.
 * @param data pointer which will carry the return data if the function call succeed.
 * @return result of the request
 *  @retval PASS  if the method was able to read the data.
 *  @retval FAIL if the method wasn't able to read the data.
 */
void * Hbl__GetProcessedLoad(uint8 load_index)
{
    void * retval = NULL;
    void *load_data_ptr;
    uint8 load_data_size;
    uint8 idx;


    if (CheckLoadHandlers(load_index))
    {
#if (HBL_LOAD_HANDLER_FEATURE == ENABLED)
        idx = Hbl_Load_Cfg[load_index].Load_Handler_Index;
        if (idx != HBL_LOAD_INDEX_DIRECT)
        {
            load_data_size = HBL_LOAD_HANDLERS[idx].Parameter_Size;
            load_data_ptr = HBL_LOAD_HANDLERS[idx].GetProcessed(&Hbl_Load_Cfg[load_index],
                (HBL_PILOT_HANDLER_HOOK_TYPE *)&HBL_PILOT_HANDLERS[Hbl_Load_Cfg[load_index].Pilot_Handler_Index]); //lint !e929 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]

            if ((load_data_ptr != NULL) &&
                (load_data_size > 0))
                {
                    retval = load_data_ptr;
                }
        }
        else

#endif
        {
            idx = Hbl_Load_Cfg[load_index].Pilot_Handler_Index;
            load_data_size = HBL_PILOT_HANDLERS[idx].Parameter_Size;
            load_data_ptr = HBL_PILOT_HANDLERS[idx].GetProcessed(&Hbl_Load_Cfg[load_index]);;

            if ((load_data_ptr != NULL) &&
                (load_data_size > 0))
                {
                    retval = load_data_ptr;
                }
        }
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns consistency value of a load by its index.
 * @param load_index  Index of load to get consistency.
 * @return consistency value related to that load - 0xFF if undefined
 */
uint8 Hbl__GetLoadConsistency(uint8 load_index)
{
    uint8 consistency_val;

    consistency_val = 0xFF;
    if (load_index < Hbl_Num_Loads)
    {
        consistency_val = Hbl_Load_Consistency[load_index];
    }
    return (consistency_val);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns consistency value to a load or pilot handler.
 * @param load Pointer to load structure.
 * @return consistency value related to that load - 0xFF if undefined
 */
uint8 Hbl__GetOwnLoadConsistency(HBL_LOAD_TYPE *load, uint8 load_value)
{
    HBL_LOAD_STATUS_TYPE load_status;
    uint8 consistency_val;
    uint8 load_index;

    consistency_val = 0xFF;
    load_index = (uint8) (load - Hbl_Load_Cfg);

    if ((load->Feedback_Params_Offset != 0xFF) &&
        (load_index < Hbl_Num_Loads))
    {
        load_status.Byte = load_value;
        consistency_val = ConsistencyEval(load_index, load_status, FALSE);
    }
    return consistency_val;
}

//----------------------------------------------------------------------------------------------------------------------
/**
 * This method set Disable flag to disable loads defined in HBL_KEEP_LOADS_OFF_LIST
 * If not defined, it does nothing
 */
void Hbl__KeepLoadsOff(BOOL_TYPE disable)
{
    #if(HBL_KEEP_LOADS_OFF_FEATURE == ENABLED)
        if(SRDATA_OK == SRData__CheckByte(&Hbl_Keep_Loads_Off, &N_Hbl_Keep_Loads_Off))
        {
            uint8 current = Hbl_Keep_Loads_Off;
            SRData__UpdateByte(&Hbl_Keep_Loads_Off, &N_Hbl_Keep_Loads_Off, disable);
            if((current == TRUE) &&
                (disable != FALSE))
            {
                KeepLoadsOffHandler();
            }
        }
    #endif
}

//----------------------------------------------------------------------------------------------------------------------
/**
 * This method return Keep loads OFF process status
 */
BOOL_TYPE Hbl__IsKeepLoadsOffEnabled(void)
{
    BOOL_TYPE ret_val = FALSE;
    #if(HBL_KEEP_LOADS_OFF_FEATURE == ENABLED)
        if(SRDATA_OK == SRData__CheckByte(&Hbl_Keep_Loads_Off, &N_Hbl_Keep_Loads_Off))
        {
            ret_val = ((BOOL_TYPE) Hbl_Keep_Loads_Off);
        }
    #endif
        return(ret_val);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns the number of GIs
 * @return
 */
uint8 Hbl__GetNumGI(void)
{
    return (Hbl_Number_GI);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns the number of LLIs
 * @return
 */
uint8 Hbl__GetNumLLI(void)
{
    return (Hbl_Number_LLI);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns the LLI Index for a given LLI ID (LLI Type + LLI Position)
 * @param lli_type      LLI Type (get from HBL_LLI_ENUM_TYPE)
 * @param position      LLI Position - unique identifier within same LLI Type
 * @return index, 0xFF means not possible to return position.
 */
uint8 HBl__GetLLIIndexByID(HBL_LLI_ENUM_TYPE lli_type, uint8 position)
{
    uint8 retval;
    uint8 index;
    retval = HBL_INDEX_INVALID;
    for (index=0; index < Hbl_Number_LLI; index++)
    {
        if ((Hbl_LLI_Cfg[index].LLI_ID == lli_type) &&
            (Hbl_LLI_Cfg[index].LLI_Position == position))
        {
            retval = index;
            break;
        }
    }
    return (retval);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns the GI ID by a given GI index.
 * @param index
 * @return GI ID, 0xFF if invalid.
 */
HBL_GENERIC_INPUT_ENUM_TYPE Hbl__GetGIIDByIndex(uint8 index)
{
    if (index < Hbl_Number_GI)
    {
        return (Hbl_GI_Cfg[index].GI_ID);
    }
    else
    {
        return (HBL_GI_INVALID);
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns GI Index by a GI ID.
 * @param input
 * @return  Index, 0xFF if invalid.
 */
uint8 Hbl__GetGIIndexByGIID(HBL_GENERIC_INPUT_ENUM_TYPE input)
{
    uint8 retval;
    uint8 index;
    retval = HBL_INDEX_INVALID;
    for (index=0; index < Hbl_Number_GI; index++)
    {
        if ( Hbl_GI_Cfg[index].GI_ID == input)
        {
            retval = index;
            break;
        }
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns the LLI Type by LLI position / index.
 * @param index
 * @return LLI Type, 0255 if not valid.
 */
HBL_LLI_ENUM_TYPE Hbl__GetLLITypeByIndex(uint8 index)
{
    if (index < Hbl_Number_LLI)
    {
        return (Hbl_LLI_Cfg[index].LLI_ID);
    }
    else
    {
        return (HBL_LLI_INVALID);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns the LLI position by LLI index.
 * @param index
 * @return LLI Position, 0255 if not valid.
 */
uint8 Hbl__GetLLIPositionByIndex(uint8 index)
{
    if (index < Hbl_Number_LLI)
    {
        return (Hbl_LLI_Cfg[index].LLI_Position);
    }
    else
    {
        return (HBL_INDEX_INVALID);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns the LLI Index by GI Index
 * @param index GI Index
 * @return LLI Index, 0xFF if not valid.
 */
uint8 Hbl__GetLLIIndexByGIIndex(uint8 index)
{
    if (index < Hbl_Number_GI)
    {
        return (Hbl_GI_Cfg[index].LLI_Config_Index);
    }
    else
    {
        return (HBL_INDEX_INVALID);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns LLI Type by GI Index.
 * @param index GI Index
 * @return LLI Type , 0xFF if not valid.
 */
HBL_LLI_ENUM_TYPE Hbl__GetLLITypeByGIIndex(uint8 index)
{
    if (index < Hbl_Number_GI)
    {
        return (Hbl_GI_Cfg[index].LLI_ID);
    }
    else
    {
        return (HBL_LLI_INVALID);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns LLI Position by GI Index.
 * @param index GI Index
 * @return LLI Position , 0xFF if not valid.
 */
uint8 Hbl__GetLLIPositionByGIIndex(uint8 index)
{
	uint8 lli_index;

	lli_index = HBL_INDEX_INVALID;

    if (index < Hbl_Number_GI)
    {
    	if (Hbl_GI_Cfg[index].LLI_Config_Index != HBL_INDEX_INVALID)
    	{
    		lli_index = Hbl_LLI_Cfg[Hbl_GI_Cfg[index].LLI_Config_Index].LLI_Position;
    	}
    }

    return (lli_index);
}
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns LLI Data Size by LLI Index.
 * @param index
 * @return LLI Data size, 0 if invalid.
 */
uint8 Hbl__GetLLIDataSizeByIndex(uint8 index)
{
    uint8 idx;
    uint8 size;

    size = 0;
    if (index < Hbl_Number_LLI)
    {
        idx = Hbl_LLI_Cfg[index].LLI_Handler_Index;
        if (idx != HBL_INDEX_INVALID)
        {
            size = (HBL_LLI_HANDLERS[idx].Parameter_Size);
        }
    }

    return(size);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns LLI Sequence ID by LLI Index.
 * @param index
 * @return LLI Sequence ID, 0 if invalid.
 */
uint8 Hbl__GetLLISequenceIDByIndex(uint8 index)
{
    uint8 seq_id;

    seq_id = 0;

    if (index < Hbl_Number_LLI)
    {
        seq_id = Hbl_LLI_Sequence[index];
    }

    return(seq_id);
}

uint8 Hbl__GetLLIIndexByOwnLoadConsistencyType(HBL_LOAD_TYPE *load, HBL_LLI_ENUM_TYPE lli_type)
{
    SETTINGFILE_LOADER_TYPE fbk_data;
    uint8 load_index;
    uint8 lli_index = HBL_INDEX_INVALID;

    load_index = (uint8) (load - Hbl_Load_Cfg);
    if (load_index < Hbl_Num_Loads)
    {
        if (PASS == SettingFile__BasicLoader(SF_PTR_ACU_IO_DEVICE, Hbl_Load_Cfg[load_index].Feedback_Params_Offset, &fbk_data))
        {
            HBL_CONSISTENCY_PARAM_HEAD_TYPE *cons_param_ptr = (HBL_CONSISTENCY_PARAM_HEAD_TYPE *)fbk_data.Data;      //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]

            if (cons_param_ptr->Config.Type_Fbk0 == lli_type)
            {
                lli_index = HBl__GetLLIIndexByID(cons_param_ptr->Config.Type_Fbk0, cons_param_ptr->Config.Pos_Fbk0);
            }
            else if ( (cons_param_ptr->Config.N_Fbk) &&
                (cons_param_ptr->Fb_Header.Double_Fb.Type_Fbk1 == lli_type))
            {
                lli_index = HBl__GetLLIIndexByID(cons_param_ptr->Fb_Header.Double_Fb.Type_Fbk1, cons_param_ptr->Fb_Header.Double_Fb.Pos_Fbk1);
            }
        }
    }

    return(lli_index);
}

uint8 *Hbl__GetLLIPinTabPtr(uint8 index)
{
    uint8 *ptr_pintab = NULL;

    if ((index < Hbl_Number_LLI)&&
        (Hbl_LLI_Cfg[index].Pin_Ptr != NULL))
    {
        ptr_pintab = Hbl_LLI_Cfg[index].Pin_Ptr;
    }

    return ptr_pintab;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns GI Data Size by GI Index.
 * @param index
 * @return GI Data size, 0 if invalid.
 */
uint8 Hbl__GetGIDataSizeByIndex(uint8 index)
{
    uint8 idx;
    uint8 size;

    size = 0;

    if (index < Hbl_Number_GI)
    {
        idx = Hbl_GI_Cfg[index].GI_Handler_Index;
        if (idx != HBL_INDEX_INVALID)
        {
            size = (HBL_GI_HANDLERS[idx].Parameter_Size);
        }
    }

    return(size);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns LLI Data by LLI Index
 * @param index LLI Index
 * @return pointer to the data or NULL if invalid.
 */
void * Hbl__GetLLIDataByIndex(uint8 index)
{
    uint8 * retval;
    uint8 idx;
    retval = NULL;

    if (index < Hbl_Number_LLI)
    {
        idx = Hbl_LLI_Cfg[index].LLI_Handler_Index;
        if ( idx != HBL_INDEX_INVALID)
        {
            retval = HBL_LLI_HANDLERS[idx].Get(&Hbl_LLI_Cfg[index]);
        }
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns the GI Data by GI Index.
 * @param index GI Index
 * @return pointer to data or NULL if invalid.
 */
void * Hbl__GetGIDataByIndex(uint8 index)
{
    void * retval = NULL;

    if (index < Hbl_Number_GI)
    {
        uint8 idx = Hbl_GI_Cfg[index].GI_Handler_Index;
        if (idx != HBL_INDEX_INVALID)
        {
            void *lli_data = Hbl__GetLLIDataByIndex(Hbl_GI_Cfg[index].LLI_Config_Index);
            if (lli_data != NULL)
            {
                #if ((HBL_USES_EXPANSION_GIS == ENABLED) && (HBL_LLI_NUM_EXPANSION > 0))
                    if (Hbl_GI_Cfg[index].LLI_ID == HBL_LLI_EXPANSION)
                    {
                        static sint32 data_value[HBL_LLI_NUM_EXPANSION];
                        static uint8 data_index = 0;
                        uint8 cur_index = data_index;
                        data_index = (data_index + 1) % HBL_LLI_NUM_EXPANSION;
                        data_value[cur_index] = API012Exp__GetGIByIndex(*(uint8 *)lli_data); //lint !e926 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                        retval = &data_value[cur_index];
                    }
                    else
                #endif
                {
                   retval = HBL_GI_HANDLERS[idx].Get(&Hbl_GI_Cfg[index],lli_data);
                }
            }
        }
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns the value of a Generic Input after converting it via Convert Module
 * @param index GI Index
 * @return Data as signed 32bits.
 */
sint32 Hbl__GetGIDataConvertedByIndex(uint8 index)
{
    sint32 retval = LONG_MAX;

    if (index < Hbl_Number_GI)
    {
        uint8 lli_index = Hbl_GI_Cfg[index].LLI_Config_Index;
        uint8 gi_handler_index = Hbl_GI_Cfg[index].GI_Handler_Index;
        if ((gi_handler_index < HBL_NUM_GI_HANDLERS) &&
            (lli_index < Hbl_Number_LLI))
        {
            uint8 * lli_data = NULL;
            uint8 lli_handler_index = Hbl_LLI_Cfg[lli_index].LLI_Handler_Index;

            if (lli_handler_index < HBL_NUM_LLI_HANDLERS)
            {
                lli_data = HBL_LLI_HANDLERS[lli_handler_index].Get(&Hbl_LLI_Cfg[lli_index]);
            }

            if (lli_data != NULL)
            {
                #if (HBL_USES_EXPANSION_GIS == ENABLED)
                    if (Hbl_GI_Cfg[index].LLI_ID == HBL_LLI_EXPANSION)
                    {
                        retval = API012Exp__GetGIByIndex(*(uint8 *)lli_data);  //lint !e926 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                    }
                    else
                #endif
                {
                    if (HBL_GI_HANDLERS[gi_handler_index].GetConverted != NULL)
                    {
                        retval = HBL_GI_HANDLERS[gi_handler_index].GetConverted(&Hbl_GI_Cfg[index], lli_data);
                    }
                }
            }
        }
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns the GI Data byGI ID
 * @param input
 * @return pointer to the data or NULL if invalid.
 */
void * Hbl__GetGIDataByID(HBL_GENERIC_INPUT_ENUM_TYPE input)
{
   return (Hbl__GetGIDataByIndex(Hbl__GetGIIndexByGIID((HBL_GENERIC_INPUT_ENUM_TYPE)input)) );
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns GI Sequence ID by GI Index.
 * @param index
 * @return GI Sequence ID, 0 if invalid.
 */
uint8 Hbl__GetGISequenceIDByIndex(uint8 index)
{
    uint8 seq_id;

    seq_id = 0;

    if (index < Hbl_Number_GI)
    {
        seq_id = Hbl_GI_Sequence[index];
    }

    return(seq_id);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//          Interface for Application layer to SRProductMgr module
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method Starts a SR Action from Class A code, when this action is used for a Class A purpose. The priority is always lower than class B requests.
 * @param index SRAction Index.
 * @param priority SRAction priority.
 * @return PASS_FAIL_TYPE
 */
PASS_FAIL_TYPE  Hbl__StartSRAction(uint8 index, uint8 priority)
{
    PASS_FAIL_TYPE retval = FAIL;

    #if((SRPRODUCTMGR_FEATURE == ENABLED) && (SRPRODUCTMGR_SRACTION_FEATURE == ENABLED))
        if (priority < SRHBL_PRIORITY_CLASSB_LO)
        {
            retval = SRProductMgr__StartSRAction(index, priority);
        }
    #endif
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method Stops a SR Action from a Class A code, when this SR Action is being handled from Class A code. The priority is always lower than Class B requests.
 * @param index SRAction index
 * @param priority SRACtion priority.
 * @return PASS_FAIL_TYPE
 */
PASS_FAIL_TYPE  Hbl__StopSRAction(uint8 index, uint8 priority)
{
    PASS_FAIL_TYPE retval = FAIL;

    #if((SRPRODUCTMGR_FEATURE == ENABLED) && (SRPRODUCTMGR_SRACTION_FEATURE == ENABLED))
        if (priority < SRHBL_PRIORITY_CLASSB_LO)
        {
            retval = SRProductMgr__StopSRAction(index, priority);
        }
    #endif
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns the status of a SRAction.
 * @param index SRAction index
 * @return Status
 */
SRPRODUCTMGR_ACTION_TYPE    Hbl__GetSRActionStatus(uint8 index)
{
    #if((SRPRODUCTMGR_FEATURE == ENABLED) && (SRPRODUCTMGR_SRACTION_FEATURE == ENABLED))
        return (SRProductMgr__GetSRActionStatus(index));
    #else
        return(SRPRODUCTMGR_ACTION_IDLE);
    #endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns if an SRAction request has been set.
 * @param index SRAction index
 * @return TRUE if SRAction is requrested, FALSE otherwise
 */
BOOL_TYPE    Hbl__IsSRActionRequested(uint8 index)
{
    BOOL_TYPE is_requested = FALSE;

    #if((SRPRODUCTMGR_FEATURE == ENABLED) && (SRPRODUCTMGR_SRACTION_FEATURE == ENABLED))
        SRPRODUCTMGR_ACTION_TYPE status = SRProductMgr__GetSRActionStatus(index);
        if ( (status != SRPRODUCTMGR_ACTION_IDLE) &&
            (status != SRPRODUCTMGR_ACTION_INVALID) )
        {
            is_requested = TRUE;
        }
    #endif
    return(is_requested);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns if a SRAction is running.
 * @param index SRAction index
 * @return TRUE if SRAction is requrested, FALSE otherwise
 */
BOOL_TYPE    Hbl__IsSRActionRunning(uint8 index)
{
	BOOL_TYPE is_running = FALSE;

    #if((SRPRODUCTMGR_FEATURE == ENABLED) && (SRPRODUCTMGR_SRACTION_FEATURE == ENABLED))
        SRPRODUCTMGR_ACTION_TYPE status = SRProductMgr__GetSRActionStatus(index);
        if ( (status != SRPRODUCTMGR_ACTION_IDLE) &&
            (status != SRPRODUCTMGR_ACTION_COMPLETED) &&
            (status != SRPRODUCTMGR_ACTION_INVALID) )
        {
            is_running = TRUE;
        }
    #endif
    return(is_running);
}

/**
 * @brief This method returns if an SRAction request has been completed.
 * @param index SRAction index
 * @return TRUE if SRAction is requrested, FALSE otherwise
 */
BOOL_TYPE    Hbl__IsSRActionCompleted(uint8 index)
{
	BOOL_TYPE is_completed = FALSE;

    #if((SRPRODUCTMGR_FEATURE == ENABLED) && (SRPRODUCTMGR_SRACTION_FEATURE == ENABLED))
        SRPRODUCTMGR_ACTION_TYPE status = SRProductMgr__GetSRActionStatus(index);
        if (status == SRPRODUCTMGR_ACTION_COMPLETED)
        {
            is_completed = TRUE;
        }
    #endif
    return(is_completed);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method returns the status of a SRCheck.
 * @param index SRCheck Status
 * @param status [out] pointer to SRCheck Status where info requested will be stored
 * @return PASS_FAIL_TYPE
 */
PASS_FAIL_TYPE  Hbl__GetSRCheckStatus(uint8 index, SRPRODUCTMGR_CHECK_STATUS_TYPE *status)
{
    #if(SRPRODUCTMGR_FEATURE == ENABLED)
        return (SRProductMgr__GetSRCheckStatus(index, status));
    #else
        return(FAIL);
    #endif
}

/**
 * @brief This method returns the SRCheck status of a fault ID.
 * @param fault_id	Fault ID to get status
 * @return SRPRODUCTMGR_STATUS_TYPE  status of the module if it's going to handle that fault ID;
 * 		SRPROSRPRODUCTMGR_STATUS_IDLE if fault ID is not active
 */
SRPRODUCTMGR_STATUS_TYPE	Hbl__GetSRFaultIDStatus(uint8 fault_id)
{
    #if(SRPRODUCTMGR_FEATURE == ENABLED)
        return (SRProductMgr__GetSRCheckStatusByFaultID(fault_id));
    #else
        return(SRPRODUCTMGR_STATUS_IDLE);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This command issues an SR Check Command.
 * @param cmd
 */
void Hbl__IssueSRCheckCmd(SRPRODUCTMGR_CHECK_CMD_TYPE cmd)
{
    #if(SRPRODUCTMGR_FEATURE == ENABLED)
        SRProductMgr__IssueSRCheckCmd(cmd);
    #endif
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//          Class B Methods
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method checks the consistency of the Hbl data for Loads/Pilots, GIs, LLIS, them process all the sRChecks
 * and Actions via SRProductMgr and them processes all the loads which really apply the laod state to each load.
 * @details This method calls SRProductMgr and processes loads
 */
void SRHbl__Process(void)
{
    BOOL_TYPE integrity_check;
    uint8 index;

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_SRHBL_START);
    #endif

    #ifdef HBL__PREPROCESSLOADS
        HBL__PREPROCESSLOADS();
    #endif

    #if (HBL_LOADS_QUEUE_FEATURE == ENABLED)
        #if (HBL_LOADS_QUEUE_CALLED_FROM_HBL == ENABLED)
            Hbl_Is_Load_Queue_Running = TRUE;
        #endif
        SetLoadQueue__Handler();
    #endif

    #if (HBL_CROSS_LOADS_FEATURE == ENABLED)
        CrossLoads__Process(Hbl_Load_Current_Priority);
    #endif

    #if ((HBL_LOADS_QUEUE_FEATURE == ENABLED) && (HBL_LOADS_QUEUE_CALLED_FROM_HBL == ENABLED))
        Hbl_Is_Load_Queue_Running = FALSE;
    #endif

    #if(HBL_KEEP_LOADS_OFF_FEATURE == ENABLED)
        KeepLoadsOffHandler();
    #endif

    // Check consistency of Hbl Load Config Data.
    integrity_check = (BOOL_TYPE) (SRDATA_OK == SRData__CheckByte(&Hbl_Num_Loads, &N_Hbl_Num_Loads));
    integrity_check &= (BOOL_TYPE) (SRDATA_OK == SRData__CheckArrayNonBlocking(&Hbl_Load_Current_Priority, &N_Hbl_Load_Current_Priority, sizeof(Hbl_Load_Current_Priority)));
    integrity_check &= (BOOL_TYPE) (SRDATA_OK == SRData__CheckArrayNonBlocking(&Hbl_Load_Consistency, &N_Hbl_Load_Consistency, sizeof(Hbl_Load_Consistency)));
    integrity_check &= (BOOL_TYPE) (SRDATA_OK == SRData__CheckArrayNonBlocking(&Hbl_Load_Consistency_LLI_SeqID, &N_Hbl_Load_Consistency_LLI_SeqID, sizeof(Hbl_Load_Consistency_LLI_SeqID)));
    for (index=0; index < HBL_MAX_NUM_LOADS; index++)
    {
        integrity_check &= (BOOL_TYPE) (Hbl_Load_Cfg_Crc[index] == Crc8(CRC8_INITIAL_SEED,(uint8 *) &Hbl_Load_Cfg[index], sizeof(Hbl_Load_Cfg[0]))); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
        if(integrity_check == FALSE)
        {
            SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT,0xFF,index);
        }
        // Check and limit Hbl_Load_Current_Priority to Class A range
        if (Hbl_Load_Current_Priority[index] >= SRHBL_PRIORITY_CLASSB_LO)
        {
            SRData__UpdateByte(&Hbl_Load_Current_Priority[index], &N_Hbl_Load_Current_Priority[index], SRHBL_PRIORITY_CLASSB_LO -1);
        }
    }
#if (HBL_MAX_NUM_OUTPUTCOMPARES_USED > 0)
    integrity_check &= (BOOL_TYPE) (SRDATA_OK == SRData__CheckByte(&Hbl_Num_OCProcess_Pilot, &N_Hbl_Num_OCProcess_Pilot));
    integrity_check &= (BOOL_TYPE) (SRDATA_OK == SRData__CheckArrayNonBlocking(&Hbl_Pilot_OCProcess_List, &N_Hbl_Pilot_OCProcess_List, sizeof(Hbl_Pilot_OCProcess_List)));
    if (integrity_check == FALSE)
    {
        SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT,0xFA,index);
    }
#endif

    // Check consistency of Hbl LLI Config Data.
    integrity_check = (BOOL_TYPE) (SRDATA_OK == SRData__CheckByte(&Hbl_Number_LLI, &N_Hbl_Number_LLI));
    integrity_check &= (BOOL_TYPE) (SRDATA_OK == SRData__CheckArrayNonBlocking(&Hbl_LLI_Sequence, &N_Hbl_LLI_Sequence, sizeof(Hbl_LLI_Sequence)));
    for (index=0; index < HBL_MAX_NUM_LOWLEVELINPUT; index++)
    {
        integrity_check &= (BOOL_TYPE) (Hbl_LLI_Cfg_Crc[index] == Crc8(CRC8_INITIAL_SEED,(uint8 *)&Hbl_LLI_Cfg[index],sizeof(HBL_LLI_TYPE))); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
        if (integrity_check == FALSE)
        {
            SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT,0xFD,index);
        }
    }


#if (HBL_MAX_NUM_INPUTCAPTURES_USED > 0)
    // Check consistency of Hbl InputCapture Data.
    integrity_check = (BOOL_TYPE) (SRDATA_OK == SRData__CheckByte(&Hbl_Num_ICProcess_LLI, &N_Hbl_Num_ICProcess_LLI));
    integrity_check &= (BOOL_TYPE) (SRDATA_OK == SRData__CheckArrayNonBlocking( &Hbl_LLI_ICProcess_List,
                        &N_Hbl_LLI_ICProcess_List,
                        sizeof(Hbl_LLI_ICProcess_List)));
    if (integrity_check == FALSE)
    {
        SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT,0xFC,index);
    }
#endif

    // Check consistency of Hbl GI Config Data
    integrity_check = (BOOL_TYPE) (SRDATA_OK == SRData__CheckByte(&Hbl_Number_GI, &N_Hbl_Number_GI));
    integrity_check &= (BOOL_TYPE) (SRDATA_OK == SRData__CheckArrayNonBlocking(&Hbl_GI_Sequence, &N_Hbl_GI_Sequence, sizeof(Hbl_GI_Sequence)));
    for (index=0; index < HBL_MAX_NUM_GENERICINPUT; index++)
    {
        integrity_check &= (BOOL_TYPE) (Hbl_GI_Cfg_Crc[index] == Crc8(CRC8_INITIAL_SEED,(uint8 *)&Hbl_GI_Cfg[index],sizeof(HBL_GI_TYPE))); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
        if (integrity_check == FALSE)
        {
            SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT,0xFB,index);
        }
    }

    #if (HBL_KEEP_LOADS_OFF_EXCLUDE_FEATURE == ENABLED)
        integrity_check = (BOOL_TYPE) (SRDATA_OK == SRData__CheckArrayNonBlocking(&Hbl_Exclude_Loads_Off, &N_Hbl_Exclude_Loads_Off, sizeof(Hbl_Exclude_Loads_Off)));
        if (integrity_check == FALSE)
        {
            SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT,0xFA,index);
        }
    #endif

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_SRHBL_CFG_CHECKS);
    #endif

    #if(SRPRODUCTMGR_FEATURE == ENABLED)
        // PRocesses all SRChecks and SRActions.
        SRProductMgr__Handler();
        #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
            SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_SRPRODUCTMGR_DONE);
        #endif
    #endif

    // Processes all the loads.
    for (index=0; index < Hbl_Num_Loads; index++)
    {
        if (CheckLoadHandlers(index))
        {
            HBL_LOAD_TYPE *load_ptr = &Hbl_Load_Cfg[index];
            #if(HBL_KEEP_LOADS_OFF_FEATURE == ENABLED)
                // Force request to OFF if Keep Loads OFF feature is active
                // and priority is lower than both HBL_KEEP_LOADS_OFF_PRIORITY and SRHBL_PRIORITY_CLASSB_LO
                if((Hbl_Keep_Loads_Off == TRUE) &&
                    #if (HBL_KEEP_LOADS_OFF_EXCLUDE_FEATURE == ENABLED)
                        (Hbl_Exclude_Loads_Off[index] == FALSE) &&
                    #endif
                    (Hbl_Load_Current_Priority[index] < SRHBL_PRIORITY_CLASSB_LO) &&
                    (Hbl_Load_Current_Priority[index] < HBL_KEEP_LOADS_OFF_PRIORITY))
                {
                    #if (HBL_LOAD_HANDLER_FEATURE == ENABLED)
                        if(load_ptr->Load_Handler_Index != HBL_LOAD_INDEX_DIRECT)
                        {
                            HBL_LOAD_HANDLERS[load_ptr->Load_Handler_Index].Request(load_ptr, (void *) HBL_FLUSH_BUFFER);
                        }
                        else
                    #endif
                    {
                        HBL_PILOT_HANDLERS[load_ptr->Pilot_Handler_Index].Request(load_ptr, (void *) HBL_FLUSH_BUFFER);
                    }
                }
            #endif

            #if (HBL_LOAD_HANDLER_FEATURE == ENABLED)
                if (load_ptr->Load_Handler_Index != HBL_LOAD_INDEX_DIRECT)
                {
                    HBL_LOAD_HANDLERS[load_ptr->Load_Handler_Index].Process(load_ptr, (HBL_PILOT_HANDLER_HOOK_TYPE *)&HBL_PILOT_HANDLERS[load_ptr->Pilot_Handler_Index]); //lint !e929 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                }
                else
            #endif
            {
                HBL_PILOT_HANDLERS[load_ptr->Pilot_Handler_Index].Process(load_ptr);
            }
        }
    }

    #if(HBL_KEEP_LOADS_OFF_FEATURE == ENABLED)
        if(SRDATA_OK == SRData__CheckByte(&Hbl_Load_Requests_Progress, &N_Hbl_Load_Requests_Progress))
        {
            SRData__UpdateByte(&Hbl_Load_Requests_Progress, &N_Hbl_Load_Requests_Progress, FALSE);
        }
    #endif

#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_SRHBL_LOADS_PROCESSED);
#endif

}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method allows Class B monitors to for the state of loads as needed to keep the product in a safe state.
 * @param load_index
 * @param priority
 * @param data
 * @return
 */
PASS_FAIL_TYPE SRHbl__SetLoadRequestByIndex(uint8 load_index, HBL_PRIORITY_TYPE priority, uint8 * data)
{
    PASS_FAIL_TYPE retval = FAIL;

    if (load_index < Hbl_Num_Loads)
    {
        if ((SRDATA_OK == SRData__CheckByte(&Hbl_Load_Current_Priority[load_index], &N_Hbl_Load_Current_Priority[load_index])) &&
            (Hbl_Load_Current_Priority[load_index] <= priority) &&
            CheckLoadHandlers(load_index))
        {
            uint8 idx;

            #if (HBL_LOAD_HANDLER_FEATURE == ENABLED)
            if (Hbl_Load_Cfg[load_index].Load_Handler_Index != HBL_LOAD_INDEX_DIRECT)
            {
                idx = Hbl_Load_Cfg[load_index].Load_Handler_Index;
                if ((Hbl_Load_Cfg[load_index].Load_Data_Index != HBL_INDEX_INVALID) &&
                    (Hbl_Load_Cfg[load_index].Pilot_Data_Index != HBL_INDEX_INVALID))
                {
                    SRData__UpdateByte(&Hbl_Load_Current_Priority[load_index], &N_Hbl_Load_Current_Priority[load_index], priority);
                    retval = HBL_LOAD_HANDLERS[idx].Request(&Hbl_Load_Cfg[load_index],data);
                }
            }
            else
#endif
            {
                idx = Hbl_Load_Cfg[load_index].Pilot_Handler_Index;
                if (Hbl_Load_Cfg[load_index].Pilot_Data_Index != HBL_INDEX_INVALID)
                {
                    SRData__UpdateByte(&Hbl_Load_Current_Priority[load_index], &N_Hbl_Load_Current_Priority[load_index], priority);
                    retval = HBL_PILOT_HANDLERS[idx].Request(&Hbl_Load_Cfg[load_index],data);
                }
            }
        }
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method Set the Request of a Load as Class B, allowing any priority.
 * @details This method is called from the Calls A Set Request method but limiting the priority.
 * @param load_id
 * @param priority
 * @param data
 * @return PASS_FAIL_TYPE
 */
PASS_FAIL_TYPE SRHbl__SetLoadRequestByLoadID(HBL_LOAD_ENUM_TYPE load_id, HBL_PRIORITY_TYPE priority, uint8 * data)
{
    return( SRHbl__SetLoadRequestByIndex(Hbl__GetLoadIndexByLoadID(load_id), priority,data));
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method resets all Load Priorities
 * @details This method is called in Application Thread at beginning of its loop execution.
 */
void SRHbl__ResetLoadPriority(void)
{
    SRData__Fill(&Hbl_Load_Current_Priority, &N_Hbl_Load_Current_Priority,SRHBL_PRIORITY_MINIMUM,HBL_MAX_NUM_LOADS);
    #if(HBL_KEEP_LOADS_OFF_FEATURE == ENABLED)
        if(SRDATA_OK == SRData__CheckByte(&Hbl_Load_Requests_Progress, &N_Hbl_Load_Requests_Progress))
        {
            SRData__UpdateByte(&Hbl_Load_Requests_Progress, &N_Hbl_Load_Requests_Progress, TRUE);
        }
    #endif
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#if (HBLACU_ZEROCROSS_FEATURE == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Handler to be executed out of the ZeroCross Detection.
 * @param angle
 * @details In principle the ZeroCross  module will call this function on 0, 90, 180, 270 degrees,
 * internally we will decide what to do in each angle.
 */
static void ZeroCrossHandler(ZEROCROSS_EVENT_TYPE angle_event)
{
    uint8 index;
    uint8 idx;

    switch(angle_event)
    {
		case ZEROCROSS_ANGLE_0_EVENT:
		case ZEROCROSS_ANGLE_180_EVENT:
		// Zero crossing events
		if (SRDATA_OK == SRData__CheckByte(&Hbl_Num_Loads, &N_Hbl_Num_Loads))
		{
			for (index=0; index < Hbl_Num_Loads; index++)
			{
				idx = Hbl_Load_Cfg[index].Pilot_Handler_Index;
				if ((idx != HBL_INDEX_INVALID) &&
					(HBL_PILOT_HANDLERS[idx].ZcProcess != NULL))
				{
					HBL_PILOT_HANDLERS[idx].ZcProcess(&Hbl_Load_Cfg[index]);
				}
			}
		}
		break;

		case ZEROCROSS_ANGLE_90_EVENT:
        case ZEROCROSS_ANGLE_270_EVENT:
		// Positive Peak
		if (SRDATA_OK == SRData__CheckByte(&Hbl_Number_LLI, &N_Hbl_Number_LLI))
		{
		    BOOL_TYPE zc_edge = (BOOL_TYPE) (angle_event == ZEROCROSS_ANGLE_90_EVENT);

			for (index=0; index < Hbl_Number_LLI; index++)
			{
				idx = Hbl_LLI_Cfg[index].LLI_Handler_Index;
				if ((idx != HBL_INDEX_INVALID) &&
					(HBL_LLI_HANDLERS[idx].PeakProcess != NULL))
				{
					if (HBL_LLI_HANDLERS[idx].PeakProcess(&Hbl_LLI_Cfg[index],zc_edge) == TRUE)
					{
						UpdateLLISequence(index);
					}
				}
			}
            SRMain__UnlockAnalogTask();
		}
		break;

		default:
			break;
    }
}
#endif

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This module retrieves the index of the Load Handler based on a Load ID
 *
 * @param load_id
 *
 * @return  0 - 253                 valid load handler index
 * @return  HBL_LOAD_INDEX_DIRECT
 * @return  HBL_LOAD_INDEX_INVALID
 */
static uint8 GetLoadHandler(HBL_LOAD_ENUM_TYPE  load_id)
{
#if (HBL_LOAD_HANDLER_FEATURE == ENABLED)
    uint8 retval = HBL_LOAD_INDEX_INVALID;
    uint8 index;

#if (HBL_USE_DIRECT_LOAD_OVERRIDE == ENABLED)
    for (index = 0; index < HBL_NUM_DIRECT_LOAD; index++)
    {
        //! HBL_DIRECT_LOAD shall contain Loads that are forced to be Direct - mandatory if that Load ID has parameters.
        if (HBL_DIRECT_LOAD[index] == load_id)
        {
            retval = HBL_LOAD_INDEX_DIRECT;
            break;
        }
    }
#endif

    if (retval == HBL_LOAD_INDEX_INVALID)
    {
        for (index=0; index < HBL_NUM_LOADS_HANDLERS; index++)
        {
            if (HBL_LOAD_HANDLERS[index].Load_ID == load_id)
            {
                retval = index;
                break;
            }
        }
    }
    return (retval);
#else
    return( HBL_LOAD_INDEX_DIRECT);
#endif

}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method retrieves the pilot handler index based on the Pilot ID
 *
 * @param pilot
 *
 * @return  0 - 254             valid pilot handler index
 * @return  HBL_INDEX_INVALID
 */
static uint8 GetPilotHandler(HBL_PILOT_ENUM_TYPE  pilot)
{
    uint8 retval;
    uint8 index;

    retval = HBL_INDEX_INVALID;
    for (index=0; index < HBL_NUM_PILOT_HANDLERS; index++)
    {
        if (HBL_PILOT_HANDLERS[index].Pilot == pilot)
        {
            retval = index;
            break;
        }
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method retrieves the GI handler based on the GI ID.
 * @param generic_input
 * @return
 */
static uint8 GetGenericInputHandler(HBL_GENERIC_INPUT_ENUM_TYPE  generic_input)
{
    uint8 retval;
    uint8 index;

    retval = HBL_INDEX_INVALID;
    for (index=0; index < HBL_NUM_GI_HANDLERS; index++)
    {
        if (HBL_GI_HANDLERS[index].GI_ID == generic_input)
        {
            retval = index;
            break;
        }
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method retrieves the LLI handler based on the LLI ID.
 * @param input
 * @return
 */
static uint8 GetLowLevelHandler(HBL_LLI_ENUM_TYPE  input)
{
    uint8 retval;
    uint8 index;

    retval = HBL_INDEX_INVALID;
    for (index=0; index < HBL_NUM_LLI_HANDLERS; index++)
    {
        if (HBL_LLI_HANDLERS[index].LLI_ID == input)
        {
            retval = index;
            break;
        }
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method loads the Hbl Configuration for Loads from the Setting File.
 */
static void LoadsInitialize(void )
{
    SETTINGFILE_LOADER_TYPE load_table = { NULL, 0 };
    SETTINGFILE_LOADER_TYPE load_pin_table = { NULL, 0 };

    SRData__UpdateByte(&Hbl_Num_Loads, &N_Hbl_Num_Loads, 0);

    #if (HBL_MAX_NUM_OUTPUTCOMPARES_USED > 0)
        SRData__UpdateByte(&Hbl_Num_OCProcess_Pilot, &N_Hbl_Num_OCProcess_Pilot, 0);
        SRData__Fill(&Hbl_Pilot_OCProcess_List, &N_Hbl_Pilot_OCProcess_List, 0xFF, sizeof(Hbl_Pilot_OCProcess_List));
    #endif

    // The check of this condition is excluded as projects may have only external pilot configuration
    SettingFile__BasicLoader(SF_PTR_ACU_IO_CONFIG, SF_DISPL_MODULE_PIN_MAP, &load_pin_table);

    if (SettingFile__BasicLoader(SF_PTR_ACU_IO_CONFIG, SF_DISPL_OUTPUTS_MAP, &load_table) == PASS)
    {
        uint8 num_loads = (load_table.Length / HBL_NUM_LOAD_PARAMETERS);
        if (num_loads > HBL_MAX_NUM_LOADS)
        {
            LOG_ADD_ERROR(NUM_OF_SETTING_LOADS_EXCEEDEDS_HBL_MAX_NUM_LOADS, Hbl_Num_Loads);
            Mode__SetModeExtended(MODE_NORMAL, SUBMODE_NORMAL_ERROR);

            //The system is forced to error condition but the HBL_MAX_NUM_LOADS is enforced too see if any other LLI related error exists in the initialization part
            num_loads = HBL_MAX_NUM_LOADS;
        }
        SRData__UpdateByte(&Hbl_Num_Loads, &N_Hbl_Num_Loads, num_loads);
    }

    for (uint8 index = 0; index < HBL_NUM_PILOT_HANDLERS; index++)
    {
        HBL_PILOT_HANDLERS[index].Initialize();
    }

    #if (HBL_LOAD_HANDLER_FEATURE == ENABLED)
        for (uint8 index = 0; index < HBL_NUM_LOADS_HANDLERS; index++)
        {
            HBL_LOAD_HANDLERS[index].Initialize();
        }
    #endif

    SRData__Fill(Hbl_Load_Current_Priority, N_Hbl_Load_Current_Priority, SRHBL_PRIORITY_MINIMUM, sizeof(Hbl_Load_Current_Priority));

    for (uint8 index = 0; index < HBL_MAX_NUM_LOADS; index++)
    {
        HBL_LOAD_TYPE *load_cfg_ptr = &Hbl_Load_Cfg[index];

        load_cfg_ptr->Load_Id = HBL_LOAD_INVALID;
        load_cfg_ptr->Load_Handler_Index = HBL_INDEX_INVALID;
        load_cfg_ptr->Pilot_Handler_Index = HBL_INDEX_INVALID;
        load_cfg_ptr->Pilot_Type = HBL_PILOT_INVALID;
        load_cfg_ptr->Pilot_Data_Index = HBL_INDEX_INVALID;
        load_cfg_ptr->Load_Data_Index = HBL_INDEX_INVALID;
        load_cfg_ptr->Pin_Ptr = NULL;

        if (index < Hbl_Num_Loads)
        {
            uint8 pilot_handler_index;
            uint8 load_handler_index;

            memcpy((uint8 *) (&load_cfg_ptr->Load_Id), &load_table.Data[index * HBL_NUM_LOAD_PARAMETERS], HBL_NUM_LOAD_PARAMETERS);    //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
            //Getting the LOAD parameters from the Settings File.
            if (load_cfg_ptr->Load_Type_Params_Offset != NO_PARAMETER_OFFSET)
            {
                // GESE prevents the generation of the setting file if the below parameters are not defined
                SettingFile__BasicLoader(SF_PTR_ACU_IO_DEVICE, load_cfg_ptr->Load_Type_Params_Offset,
                &load_cfg_ptr->Load_Parameter_Ptr);
            }
            else
            {
                load_cfg_ptr->Load_Parameter_Ptr.Data = NULL;
                load_cfg_ptr->Load_Parameter_Ptr.Length = 0;
            }

            if ((load_pin_table.Data != NULL ) &&
                (load_cfg_ptr->Pin_Table_Offset < load_pin_table.Length))
            {
                load_cfg_ptr->Pin_Ptr = &load_pin_table.Data[load_cfg_ptr->Pin_Table_Offset];
            }

            if (load_cfg_ptr->Pilot_Type_Params_Offset != NO_PARAMETER_OFFSET)
            {
                // GESE prevents the generation of the setting file if the below parameters are not defined
                SettingFile__BasicLoader(SF_PTR_ACU_IO_DEVICE, load_cfg_ptr->Pilot_Type_Params_Offset, &load_cfg_ptr->Parameter_Ptr);
            }
            else
            {
                load_cfg_ptr->Parameter_Ptr.Data = NULL;
                load_cfg_ptr->Parameter_Ptr.Length = 0;
            }

            // Loads
            load_handler_index = GetLoadHandler(load_cfg_ptr->Load_Id);

            #if (HBL_LOAD_HANDLER_FEATURE == ENABLED)
                if (load_handler_index == HBL_LOAD_INDEX_INVALID)
                {
                    if(load_cfg_ptr->Load_Parameter_Ptr.Data == NULL)
                    {
                        // Direct load not present into HBL_DIRECT_LOAD and with no parameters can be configured as Direct.
                        load_handler_index = HBL_LOAD_INDEX_DIRECT;
                        #if (HBL_USE_DIRECT_LOAD_OVERRIDE == ENABLED)
                            LOG_ADD_INFO(LOAD_DIRECT_HANDLER_REMAP, index);
                        #endif
                    }
                    else
                    {
                        // Direct load not present into HBL_DIRECT_LOAD and with parameters is an invalid option.
                        LOG_ADD_ERROR(INVALID_LOAD_HANDLER_INDEX, index);
                        Mode__SetModeExtended(MODE_NORMAL, SUBMODE_NORMAL_ERROR);
                    }
                }
            #endif

            // Pilots
            pilot_handler_index = GetPilotHandler(load_cfg_ptr->Pilot_Type);
            if (pilot_handler_index == HBL_INDEX_INVALID)
            {
                LOG_ADD_ERROR(INVALID_PILOT_HANDLER_INDEX, index);
                Mode__SetModeExtended(MODE_NORMAL, SUBMODE_NORMAL_ERROR);
            }

            if ((load_handler_index != HBL_LOAD_INDEX_INVALID) && (pilot_handler_index != HBL_INDEX_INVALID))
            {
                HBL_PILOT_HANDLER_HOOK_TYPE *pilot_hook_ptr = (HBL_PILOT_HANDLER_HOOK_TYPE *) &HBL_PILOT_HANDLERS[pilot_handler_index]; //lint !e929 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]

                //Pilot Handler Initialization
                load_cfg_ptr->Pilot_Data_Index = pilot_hook_ptr->Allocate();
                if (load_cfg_ptr->Pilot_Data_Index == HBL_ALLOCATION_INVALID)
                {
                    LOG_ADD_ERROR(PILOT_ALLOCATION_FAILED, index);
                    Mode__SetModeExtended(MODE_NORMAL, SUBMODE_NORMAL_ERROR);
                }
                else if (pilot_hook_ptr->InitializeInstance(load_cfg_ptr) == PASS)
                {
                    load_cfg_ptr->Pilot_Handler_Index = pilot_handler_index;

                    // Initialize the pilots handling the Output compare feature
                    if (HBL_PILOT_HANDLERS[pilot_handler_index].OCProcess != NULL)
                    {
                        #if (HBL_MAX_NUM_OUTPUTCOMPARES_USED > 0)

                            if (SRDATA_OK == SRData__CheckByte(&Hbl_Num_OCProcess_Pilot, &N_Hbl_Num_OCProcess_Pilot) &&
                                (Hbl_Num_OCProcess_Pilot < HBL_MAX_NUM_OUTPUTCOMPARES_USED))
                            {
                                SRData__UpdateByte((uint8 *) &Hbl_Pilot_OCProcess_List[Hbl_Num_OCProcess_Pilot].Timer, //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                                (uint8 *) &N_Hbl_Pilot_OCProcess_List[Hbl_Num_OCProcess_Pilot].Timer, //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                                (TIMER_TYPE) VIRTUALPIN_TABLE[(*(uint8 *) (Hbl_Load_Cfg[index].Pin_Ptr))].device); //lint !e926 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                                SRData__UpdateByte((uint8 *) &Hbl_Pilot_OCProcess_List[Hbl_Num_OCProcess_Pilot].Timer_Channel, //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                                (uint8 *) &N_Hbl_Pilot_OCProcess_List[Hbl_Num_OCProcess_Pilot].Timer_Channel, //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                                (TIMER_CHANNEL_TYPE) VIRTUALPIN_TABLE[(*(uint8 *) (Hbl_Load_Cfg[index].Pin_Ptr))].param0); //lint !e926 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                                SRData__UpdateByte((uint8 *) &Hbl_Pilot_OCProcess_List[Hbl_Num_OCProcess_Pilot].Pilot_Cfg_Index, //lint !e926 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                                (uint8 *) &N_Hbl_Pilot_OCProcess_List[Hbl_Num_OCProcess_Pilot].Pilot_Cfg_Index, index); //lint !e926 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                                SRData__UpdateByte(&Hbl_Num_OCProcess_Pilot, &N_Hbl_Num_OCProcess_Pilot, Hbl_Num_OCProcess_Pilot+1);
                            }
                            else
                        #endif
                        {
                            LOG_ADD_ERROR(LOAD_OUTPUTCOMPARE_ALLOCATION_FAILED, index);
                            Mode__SetModeExtended(MODE_NORMAL, SUBMODE_NORMAL_ERROR);
                        }
                    }
                }
                else
                {
                    LOG_ADD_ERROR(PILOT_INSTANCE_INITIALIZATION_FAILED, index);
                    Mode__SetModeExtended(MODE_NORMAL, SUBMODE_NORMAL_ERROR);
                }

                //Load Handler Initialization
                #if (HBL_LOAD_HANDLER_FEATURE == ENABLED)
                    if (load_handler_index != HBL_LOAD_INDEX_DIRECT)
                    {
                        HBL_LOAD_HANDLER_HOOK_TYPE *load_hook_ptr = (HBL_LOAD_HANDLER_HOOK_TYPE *) &HBL_LOAD_HANDLERS[load_handler_index]; //lint !e929 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                        load_cfg_ptr->Load_Data_Index = load_hook_ptr->Allocate();
                        if (load_cfg_ptr->Load_Data_Index == HBL_ALLOCATION_INVALID)
                        {
                            LOG_ADD_ERROR(LOAD_ALLOCATION_FAILED, index);
                            Mode__SetModeExtended(MODE_NORMAL, SUBMODE_NORMAL_ERROR);
                        }
                        else if (load_hook_ptr->InitializeInstance(load_cfg_ptr) == PASS)
                        {
                            load_cfg_ptr->Load_Handler_Index = load_handler_index;
                        }
                        else
                        {
                            LOG_ADD_ERROR(LOAD_INSTANCE_INITIALIZATION_FAILED, index);
                            Mode__SetModeExtended(MODE_NORMAL, SUBMODE_NORMAL_ERROR);
                        }
                    }
                    else
                #endif
                {
                    load_cfg_ptr->Load_Handler_Index = HBL_LOAD_INDEX_DIRECT;
                }
            }
        }

        Hbl_Load_Cfg_Crc[index] = Crc8(CRC8_INITIAL_SEED, (uint8 *) load_cfg_ptr, sizeof(HBL_LOAD_TYPE)); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    }
    #if (HBL_LOADS_QUEUE_FEATURE == ENABLED)
        SetLoadQueue__Initialize();
        #if (HBL_LOADS_QUEUE_CALLED_FROM_HBL == ENABLED)
            Hbl_Is_Load_Queue_Running = FALSE;
        #endif
    #endif

    #if(HBL_KEEP_LOADS_OFF_FEATURE == ENABLED)
        KeepLoadsOffInitialize();
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method loads the LLIs configuration from the setting File.
 */
static void LLIInitialize(void)
{
    uint8 * ptr;
    SETTINGFILE_LOADER_TYPE lli_table;
    SETTINGFILE_LOADER_TYPE lli_pin_table;

    //Initialization of the LLI related variables
    SRData__UpdateByte(&Hbl_Number_LLI, &N_Hbl_Number_LLI, 0);
    SRData__Fill(Hbl_LLI_Sequence, N_Hbl_LLI_Sequence, 0, sizeof(Hbl_LLI_Sequence));

	#if (HBL_MAX_NUM_INPUTCAPTURES_USED > 0)
		SRData__UpdateByte(&Hbl_Num_ICProcess_LLI, &N_Hbl_Num_ICProcess_LLI, 0);
		SRData__Fill(&Hbl_LLI_ICProcess_List, &N_Hbl_LLI_ICProcess_List, 0xFF, sizeof(Hbl_LLI_ICProcess_List));
	#endif

	// The check of this condition is excluded as projects may have only external LLI configuration
	SettingFile__BasicLoader(SF_PTR_ACU_IO_CONFIG, SF_DISPL_MODULE_PIN_MAP, &lli_pin_table);

    if (SettingFile__BasicLoader(SF_PTR_ACU_IO_CONFIG, SF_DISPL_LOW_LEVEL_INPUT_MAP, &lli_table) == PASS)
    {
        uint8 lli_number = lli_table.Length / 4;

        if ( lli_number > HBL_MAX_NUM_LOWLEVELINPUT )
        {
            //The system is forced to error condition but the HBL_MAX_NUM_LOWLEVELINPUT is enforced too see if any other LLI related error exists in the initialization part
            lli_number = HBL_MAX_NUM_LOWLEVELINPUT;
            LOG_ADD_ERROR(NUM_OF_SETTING_LLIS_EXCEEDEDS_HBL_MAX_NUM_LOWLEVELINPUT, Hbl_Number_LLI);
            Mode__SetModeExtended(MODE_NORMAL, SUBMODE_NORMAL_ERROR);
        }

        SRData__UpdateByte(&Hbl_Number_LLI, &N_Hbl_Number_LLI, lli_number);
    }


    for (uint8 index=0; index < HBL_NUM_LLI_HANDLERS; index++)
    {
        HBL_LLI_HANDLERS[index].Initialize();
    }

    for (uint8 index=0; index < HBL_MAX_NUM_LOWLEVELINPUT; index++)
    {
        Hbl_LLI_Cfg[index].LLI_ID 				= HBL_LLI_INVALID;
        Hbl_LLI_Cfg[index].Data_Index 			= HBL_INDEX_INVALID;
        Hbl_LLI_Cfg[index].LLI_Handler_Index 	= HBL_INDEX_INVALID;

        if (index < Hbl_Number_LLI)
        {
        	//Extract the configuration from the setting file
            ptr = &lli_table.Data[4*index];

            Hbl_LLI_Cfg[index].LLI_ID = (HBL_LLI_ENUM_TYPE)(ptr[0]);
            Hbl_LLI_Cfg[index].Pin_Ptr = &lli_pin_table.Data[ptr[1]];

            #if (HBLACU_ZEROCROSS_FEATURE == ENABLED)
                if ((Hbl_LLI_Cfg[index].LLI_ID == HBL_LLI_SRZEROCROSS) ||
                (Hbl_LLI_Cfg[index].LLI_ID == HBL_LLI_ZEROCROSS))
                {
                    Hbl_ZC_Virtual_Pin = *(Hbl_LLI_Cfg[index].Pin_Ptr);
                }
            #endif

            if (ptr[2] != NO_PARAMETER_OFFSET)
            {
                SettingFile__BasicLoader(SF_PTR_ACU_IO_DEVICE, ptr[2], &Hbl_LLI_Cfg[index].Parameter_Ptr);
            }
            else
            {
                Hbl_LLI_Cfg[index].Parameter_Ptr.Data = NULL;
                Hbl_LLI_Cfg[index].Parameter_Ptr.Length = 0;
            }

            Hbl_LLI_Cfg[index].LLI_Position =  (ptr[3] & 0x7F);

            uint8 lli_handler_index = GetLowLevelHandler(Hbl_LLI_Cfg[index].LLI_ID);


            if ( lli_handler_index == HBL_INDEX_INVALID )
            {
                //Force the system to error mode if the initialization of the module fails
                LOG_ADD_ERROR(INVALID_LLI_HANDLER_INDEX, index);
                Mode__SetModeExtended(MODE_NORMAL, SUBMODE_NORMAL_ERROR);
            }
            else
            {
                Hbl_LLI_Cfg[index].Data_Index =  HBL_LLI_HANDLERS[lli_handler_index].Allocate();

                if ( Hbl_LLI_Cfg[index].Data_Index >= HBL_INDEX_INVALID )
                {
                    //Force the system to error mode if the initialization of the module fails
                    LOG_ADD_ERROR(LLI_ALLOCATION_FAILED, index);
                    Mode__SetModeExtended(MODE_NORMAL, SUBMODE_NORMAL_ERROR);
                }
                else if (HBL_LLI_HANDLERS[lli_handler_index].InitializeInstance(&Hbl_LLI_Cfg[index]) == PASS)
                {
                    Hbl_LLI_Cfg[index].LLI_Handler_Index = lli_handler_index;

                    if (HBL_LLI_HANDLERS[lli_handler_index].ICProcess != NULL)
                    {
                        #if (HBL_MAX_NUM_INPUTCAPTURES_USED > 0)
                            SRData__CheckByte(&Hbl_Num_ICProcess_LLI, &N_Hbl_Num_ICProcess_LLI);
                            if(Hbl_Num_ICProcess_LLI < HBL_MAX_NUM_INPUTCAPTURES_USED)
                            {
                                HBL_LLI_IC_TYPE new_item;

                                SRData__CheckArray( &Hbl_LLI_ICProcess_List[Hbl_Num_ICProcess_LLI],
                                                    &N_Hbl_LLI_ICProcess_List[Hbl_Num_ICProcess_LLI],
                                                    sizeof(Hbl_LLI_ICProcess_List[0]));

                                new_item.Timer = (TIMER_TYPE)VIRTUALPIN_TABLE[(*(uint8 *)(Hbl_LLI_Cfg[index].Pin_Ptr))].device;            //lint !e926 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                                new_item.Timer_Channel          = (TIMER_CHANNEL_TYPE)VIRTUALPIN_TABLE[(*(uint8 *)(Hbl_LLI_Cfg[index].Pin_Ptr))].param0;    //lint !e926 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                                new_item.LLI_Cfg_Index          = index;

                                SRData__UpdateArray(&Hbl_LLI_ICProcess_List[Hbl_Num_ICProcess_LLI],
                                                    &N_Hbl_LLI_ICProcess_List[Hbl_Num_ICProcess_LLI],
                                                    &new_item,
                                                    sizeof(Hbl_LLI_ICProcess_List[0]));
                                SRData__UpdateByte(&Hbl_Num_ICProcess_LLI, &N_Hbl_Num_ICProcess_LLI, Hbl_Num_ICProcess_LLI+1);
                            }
                            else
                        #endif
                        {
                            //Force the system to error mode if the initialization of the module fails
                            LOG_ADD_ERROR(LLI_INPUTCAPTURE_ALLOCATION_FAILED, index);
                            Mode__SetModeExtended(MODE_NORMAL, SUBMODE_NORMAL_ERROR);
                        }
                    }
                }
                else
                {
                    //Force the system to error mode if the initialization of the module fails
                    LOG_ADD_ERROR(LLI_INSTANCE_INITIALIZATION_FAILED, index);
                    Mode__SetModeExtended(MODE_NORMAL, SUBMODE_NORMAL_ERROR);
                }
            }
        }
        Hbl_LLI_Cfg_Crc[index] = Crc8(CRC8_INITIAL_SEED,(uint8 *)&Hbl_LLI_Cfg[index],sizeof(HBL_LLI_TYPE));      //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method loads the GIs configuration from the SettinFile.
 */
static void GIInitialize(void)
{
    SETTINGFILE_LOADER_TYPE gi_table;

    //Initialization of the GI related variables
    SRData__Fill(Hbl_GI_Sequence, N_Hbl_GI_Sequence, 0, sizeof(Hbl_GI_Sequence));
    SRData__UpdateByte(&Hbl_Number_GI, &N_Hbl_Number_GI, 0);
    #if (HBL_USES_EXPANSION_GIS == ENABLED)
        //------- Expansion GI Data Type management ----------------------------------------
        memset(Hbl_GI_Exp_Last_Seq_ID, 0, sizeof(Hbl_GI_Exp_Last_Seq_ID));
    #endif

    if (SettingFile__BasicLoader(SF_PTR_ACU_IO_CONFIG, SF_DISPL_GENERIC_INPUT_MAP, &gi_table) == PASS)
    {
        uint8 gi_number;

        for (uint8 index=0; index < HBL_NUM_GI_HANDLERS; index++)
        {
            HBL_GI_HANDLERS[index].Initialize();
        }

        gi_number = gi_table.Length / HBL_NUM_GI_PARAMETERS;
        if ( gi_number > HBL_MAX_NUM_GENERICINPUT )
        {
            LOG_ADD_ERROR(NUM_OF_SETTING_GIS_EXCEEDEDS_HBL_MAX_NUM_GENERICINPUT, Hbl_Number_GI);

            //Force the system to error mode if the number of GI configured are more than the allocated
            Mode__SetModeExtended(MODE_NORMAL, SUBMODE_NORMAL_ERROR);

            //The system is forced to error condition but the HBL_MAX_NUM_GENERICINPUT is enforced too see if any other GI related error exists in the initialization part
            gi_number = HBL_MAX_NUM_GENERICINPUT;
        }
        SRData__UpdateByte(&Hbl_Number_GI, &N_Hbl_Number_GI, gi_number);
    }

    //Allocate and Initialize instance of each GI
    for (uint8 index=0; index < HBL_MAX_NUM_GENERICINPUT; index++)
    {
        Hbl_GI_Cfg[index].GI_Handler_Index	= HBL_INDEX_INVALID;
        Hbl_GI_Cfg[index].Data_Index 		= HBL_INDEX_INVALID;
        Hbl_GI_Cfg[index].GI_ID				= HBL_GI_INVALID;

        if (index < Hbl_Number_GI)
        {
            uint8 *ptr = &gi_table.Data[HBL_NUM_GI_PARAMETERS * index];        //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]

            //extract the GI related configurations from the settig file
            Hbl_GI_Cfg[index].LLI_ID = (HBL_LLI_ENUM_TYPE)ptr[0];
            Hbl_GI_Cfg[index].GI_ID = (HBL_GENERIC_INPUT_ENUM_TYPE)ptr[1];
            Hbl_GI_Cfg[index].GI_Param_Offset = ptr[2];
            uint8 position =  ptr[3] & 0x7F;

            Hbl_GI_Cfg[index].LLI_Config_Index = HBl__GetLLIIndexByID(Hbl_GI_Cfg[index].LLI_ID, position);
            Hbl_GI_Cfg[index].LLI_Parameter_Size = Hbl__GetLLIDataSizeByIndex(Hbl_GI_Cfg[index].LLI_Config_Index);
            uint8 gi_handler_index = GetGenericInputHandler(Hbl_GI_Cfg[index].GI_ID);

            if ((gi_handler_index == HBL_INDEX_INVALID ) ||
                (Hbl_GI_Cfg[index].LLI_Config_Index == HBL_INDEX_INVALID))	//Failing of LLI_Config_Index is impossible as GESE takes care of that
            {
                //Force the system to error mode if the initialization of the module fails
                LOG_ADD_ERROR(INVALID_GI_HANDLER_INDEX, index);
                Mode__SetModeExtended(MODE_NORMAL, SUBMODE_NORMAL_ERROR);
            }
            else
            {
                Hbl_GI_Cfg[index].Data_Index = HBL_GI_HANDLERS[gi_handler_index].Allocate();

                if ( Hbl_GI_Cfg[index].Data_Index >= HBL_INDEX_INVALID )
                {
                    //Force the system to error mode if the initialization of the module fails
                    LOG_ADD_ERROR(GI_ALLOCATION_FAILED, index);
                    Mode__SetModeExtended(MODE_NORMAL, SUBMODE_NORMAL_ERROR);
                }
                else
                {
                    Hbl_GI_Cfg[index].Parameter_Size = HBL_GI_HANDLERS[gi_handler_index].Parameter_Size;

                    if (HBL_GI_HANDLERS[gi_handler_index].InitializeInstance(&Hbl_GI_Cfg[index], Hbl_LLI_Sequence[Hbl_GI_Cfg[index].LLI_Config_Index]) == FAIL)
                    {
                        //Force the system to error mode if the initialization of the module fails
                        LOG_ADD_ERROR(GI_INSTANCE_INITIALIZATION_FAILED, index);
                        Mode__SetModeExtended(MODE_NORMAL, SUBMODE_NORMAL_ERROR);
                    }
                    else
                    {
                        Hbl_GI_Cfg[index].GI_Handler_Index = gi_handler_index;
                    }
                }
            }
        }

        Hbl_GI_Cfg_Crc[index] = Crc8(CRC8_INITIAL_SEED,(uint8 *)&Hbl_GI_Cfg[index],sizeof(HBL_GI_TYPE));     //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief this method checks if a Load is in the range of valid loads and if the Load and Pilot handlers are valid.
 * @param index Load index.
 * @return TRUS for valid, FALSE for invalid.
 */
static BOOL_TYPE CheckLoadHandlers(uint8 index)
{
    BOOL_TYPE is_valid = TRUE;

    if (index >= Hbl_Num_Loads)
    {
        is_valid = FALSE;
    }
    else
    {
        HBL_LOAD_TYPE *load_cfg = &Hbl_Load_Cfg[index];
        BOOL_TYPE load_handler_check = (BOOL_TYPE) (load_cfg->Load_Handler_Index == HBL_LOAD_INDEX_DIRECT);
        #if (HBL_LOAD_HANDLER_FEATURE == ENABLED)
            load_handler_check |= (BOOL_TYPE) (load_cfg->Load_Handler_Index < HBL_NUM_LOADS_HANDLERS);
        #endif

        if ((load_handler_check == FALSE) ||
        	(load_cfg->Pilot_Handler_Index >= HBL_NUM_PILOT_HANDLERS))
        {
            is_valid = FALSE;
        }
    }

    return(is_valid);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method initializes the Consistency check
 */
static void ConsistencyInitialize(void)
{
    SRData__Fill(&Hbl_Load_Consistency, &N_Hbl_Load_Consistency, 255, sizeof(Hbl_Load_Consistency));
    SRData__Fill(&Hbl_Load_Consistency_LLI_SeqID, &N_Hbl_Load_Consistency_LLI_SeqID, 0, sizeof(Hbl_Load_Consistency_LLI_SeqID));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method performs consistency update for a given load.
 * @param load_index Index for the load to update
 *
 */
static void ConsistencyDoUpdate(uint8 load_index)
{
    uint8 * load_status;
    HBL_LOAD_STATUS_TYPE load_value;

    if ((load_index < Hbl_Num_Loads) &&
        (SRDATA_OK == SRData__CheckByte(&Hbl_Load_Consistency[load_index],
            &N_Hbl_Load_Consistency[load_index])))
    {
        load_status = (uint8 *) Hbl__GetLoadStatusByIndex(load_index);

        // Convert current load status to row selected
        load_value.Byte = load_status[0];
        (void) ConsistencyEval(load_index, load_value, TRUE);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method performs consistency update for a given load.
 * @param load_index Index for the load to update
 *
 */
static uint8 ConsistencyEval(uint8 load_index, HBL_LOAD_STATUS_TYPE load_value, BOOL_TYPE update_seq)
{
    HBL_CONSISTENCY_TYPE temp_res = { 0xFF };
    uint8 *convert_ptr = NULL;
    uint16 fbk_val = 0xFFFF;
    uint8 lli2_sequence = 0;
    uint8 lli_sequence = 0;
    uint8 lli2_index = HBL_INDEX_INVALID;
    uint8 lli_index = HBL_INDEX_INVALID;
    HBL_CONSISTENCY_PARAM_HEAD_TYPE *cons_param_ptr;
    SETTINGFILE_LOADER_TYPE fbk_data;
    uint8 fbk_num;

    if ((SRDATA_OK == SRData__CheckByte(&Hbl_Load_Consistency_LLI_SeqID[load_index][0], &N_Hbl_Load_Consistency_LLI_SeqID[load_index][0]))&&
    (SRDATA_OK == SRData__CheckByte(&Hbl_Load_Consistency_LLI_SeqID[load_index][1], &N_Hbl_Load_Consistency_LLI_SeqID[load_index][1])))
    {
        SettingFile__BasicLoader(SF_PTR_ACU_IO_DEVICE, Hbl_Load_Cfg[load_index].Feedback_Params_Offset, &fbk_data);

        cons_param_ptr = (HBL_CONSISTENCY_PARAM_HEAD_TYPE *)fbk_data.Data;      //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
        fbk_num = 1 + cons_param_ptr->Config.N_Fbk;

        if (fbk_num == 2)
        {
            // Two feedbacks configuration: second feedback chooses which conversion table to use in parameters data
            lli2_index = HBl__GetLLIIndexByID(cons_param_ptr->Fb_Header.Double_Fb.Type_Fbk1, cons_param_ptr->Fb_Header.Double_Fb.Pos_Fbk1);
            if (lli2_index < Hbl__GetNumLLI())
            {
                lli2_sequence = Hbl__GetLLISequenceIDByIndex(lli2_index);
                if ( (lli2_sequence == Hbl_Load_Consistency_LLI_SeqID[load_index][1]) &&
                    (update_seq == TRUE))
                {
                    update_seq = FALSE;
                }
            }
            else
            {
                update_seq = TRUE;
            }

            // Prevent consistency update if Sequence ID is 0 - LLI not ready yet
            if (lli2_sequence > 0)
            {
                // Round up Data size to 2 bytes
                uint16 data_size = cons_param_ptr->Config.Conv_Data_Size;
                data_size += (data_size & 0x01);
                fbk_val = ConsistencyGetFeedbackValue(cons_param_ptr->Fb_Header.Double_Fb.Type_Fbk1, cons_param_ptr->Fb_Header.Double_Fb.Pos_Fbk1);

                // Boundary check: fbk_val has to point inside parameter data
                if ((fbk_data.Length / data_size) > fbk_val)
                {
                    convert_ptr = &cons_param_ptr->Fb_Header.Double_Fb.convert_data + (fbk_val * data_size);
                }
            }
        }
        else
        {
            convert_ptr = &cons_param_ptr->Fb_Header.Single_Fb.convert_data;
        }

        if (convert_ptr != NULL)             // Update consistency only if a convert table is found
        {
            //Check if Feedback 0 LLI Sequence
            lli_index = HBl__GetLLIIndexByID(cons_param_ptr->Config.Type_Fbk0, cons_param_ptr->Config.Pos_Fbk0);
            if (lli_index < Hbl__GetNumLLI())
            {
                lli_sequence = Hbl__GetLLISequenceIDByIndex(lli_index);
                if ((lli_sequence == Hbl_Load_Consistency_LLI_SeqID[load_index][0]) &&
                            (update_seq == TRUE))
                {
                    update_seq = FALSE;
                }
            }
            else
            {
                update_seq = FALSE;
            }

            // Prevent any consistency update if Sequence ID is 0 - LLI not ready yet
            if (lli_sequence  > 0)
            {
                fbk_val = ConsistencyGetFeedbackValue(cons_param_ptr->Config.Type_Fbk0, cons_param_ptr->Config.Pos_Fbk0);
            }
            else
            {
                fbk_val = 0xFFFF;
            }

            if (fbk_val != 0xFFFF)           // Update consistency only if fbk_val is valid
            {
                temp_res.Consistency.Table = HBL_CONSISTENCY_TABLE_INVALID;
                temp_res.Consistency.Value = HBL_CONSISTENCY_INVALID_VALUE;

                if (load_value.Load.Status != HBL_LOAD_STATUS_INITIALIZE)
                {
                    if ((load_value.Load.Status == HBL_LOAD_STATUS_WAITING_TARGET) && (cons_param_ptr->Config.N_States > 0))
                    {
                        temp_res.Consistency.Table = HBL_CONSISTENCY_TABLE_WAITING;
                    }
                    else if (load_value.Load.Cur_Reg != 0)
                    {
                        if ((load_value.Load.Status == HBL_LOAD_STATUS_TARGET_ALT_TABLE) && (cons_param_ptr->Config.N_States > 1))
                        {
                            temp_res.Consistency.Table = HBL_CONSISTENCY_TABLE_ON_ALTERNATE;
                        }
                        else
                        {
                            temp_res.Consistency.Table = HBL_CONSISTENCY_TABLE_ON;
                        }
                    }
                    else
                    {
                        temp_res.Consistency.Table = HBL_CONSISTENCY_TABLE_OFF;
                    }
                }

                if (temp_res.Consistency.Table != HBL_CONSISTENCY_TABLE_INVALID)
                {
                    //This method gets converted value from consistency table
                    fbk_val = (uint16)HblCommon__ConvertIODeviceData(fbk_val, (HBL_IO_DEVICE_DATA_CONVERT_TABLE_HEADER *)convert_ptr); //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
                    temp_res.Consistency.Value = (HBL_CONSISTENCY_VALUE_ENUM_TYPE) (fbk_val >> (4*temp_res.Consistency.Table));
                    if (update_seq == TRUE)
                    {
                        // Update Feedback 1 LLI Sequence
                        if (lli_index < Hbl__GetNumLLI())
                        {
                            SRData__UpdateByte(&Hbl_Load_Consistency_LLI_SeqID[load_index][0], &N_Hbl_Load_Consistency_LLI_SeqID[load_index][0], lli_sequence);
                        }
                        // Update Feedback 2 LLI Sequence
                        if ((fbk_num == 2) &&
                            (lli2_index < Hbl__GetNumLLI()))
                        {
                            SRData__UpdateByte(&Hbl_Load_Consistency_LLI_SeqID[load_index][1], &N_Hbl_Load_Consistency_LLI_SeqID[load_index][1], lli2_sequence);
                        }
                        SRData__UpdateByte(&Hbl_Load_Consistency[load_index],
                            &N_Hbl_Load_Consistency[load_index],
                            temp_res.Byte);
                    }
                }
            }
        }
    }

    return(temp_res.Byte);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method gets LLI or Load Status value for consistency.
 * @param type LLI type ID or Load ID if HBL_LLI_INVALID
 * @param position LLI type position or Load position if type is HBL_LLI_INVALID
 *
 */
static uint16 ConsistencyGetFeedbackValue(HBL_LLI_ENUM_TYPE type, uint8 position)
{
    uint16 ret_val = 0xFFFF;
    uint8 *temp_res;

    if (type == HBL_LLI_INVALID)
    {
        HBL_LOAD_STATUS_TYPE temp_val;

        temp_res = (uint8 *) Hbl__GetLoadStatusByIndex(position);
        temp_val.Byte = *temp_res;
        if (temp_val.Load.Cur_Reg != 0)
        {
            ret_val = (uint16) ON;
        }
        else
        {
            ret_val = (uint16) OFF;
        }
    }
    else
    {
        uint8 index = HBl__GetLLIIndexByID(type, position);
        uint8 data_size = Hbl__GetLLIDataSizeByIndex(index);

        temp_res = (uint8 *) Hbl__GetLLIDataByIndex(index);
        if (data_size == 2)
        {
            ret_val = *((uint16 *)temp_res);        //lint !e927 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
        }
        else if (data_size == 1)
        {
            ret_val = (uint16) *temp_res;
        }
        else
        {
            // No other data sizes are allowed
        }
    }

    return(ret_val);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief -
 * @param index
 */
static void UpdateLLISequence(uint8 index)
{
    uint8 *seq_ptr = &Hbl_LLI_Sequence[index];
    uint8 *n_seq_ptr = &N_Hbl_LLI_Sequence[index];

    if (SRDATA_OK == SRData__CheckByte(seq_ptr, n_seq_ptr))
    {
        uint8 new_value = *seq_ptr;
        new_value++;
        if (new_value == 0)
        {
            new_value = 1;
        }
        SRData__UpdateByte(seq_ptr, n_seq_ptr, new_value);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief -
 * @param index
 */
static void UpdateGISequence(uint8 index)
{
    uint8 *seq_ptr = &Hbl_GI_Sequence[index];
    uint8 *n_seq_ptr = &N_Hbl_GI_Sequence[index];

    if (SRDATA_OK == SRData__CheckByte(seq_ptr, n_seq_ptr))
    {
        uint8 new_value = *seq_ptr;
        new_value++;
        if (new_value == 0)
        {
            new_value = 1;
        }
        SRData__UpdateByte(seq_ptr, n_seq_ptr, new_value);
    }
}

#if(HBL_KEEP_LOADS_OFF_FEATURE == ENABLED)

//---------------------------------------------------------------------------------------------------------------------
/**
 * This method Initializes the Disable Loads feature; it looks load index for those listed in HBL_KEEP_LOADS_OFF_LIST
 * and set Disable feature as Disabled
 */
void KeepLoadsOffInitialize(void)
{
    SRData__UpdateByte(&Hbl_Keep_Loads_Off, &N_Hbl_Keep_Loads_Off, FALSE);
    SRData__UpdateByte(&Hbl_Load_Requests_Progress, &N_Hbl_Load_Requests_Progress, FALSE);
    #if (HBL_KEEP_LOADS_OFF_EXCLUDE_FEATURE == ENABLED)
        SRData__Fill(Hbl_Exclude_Loads_Off, N_Hbl_Exclude_Loads_Off, FALSE, sizeof(Hbl_Exclude_Loads_Off));
        for(uint8 index = 0; index < ELEMENTS_IN_ARRAY(Hbl_Load_Exclude_List); index++)
        {
            uint8 load_index = Hbl__GetLoadIndexByLoadID((HBL_LOAD_ENUM_TYPE) Hbl_Load_Exclude_List[index]);
            if(load_index < Hbl_Num_Loads)
            {
                SRData__UpdateByte(&Hbl_Exclude_Loads_Off[load_index], &N_Hbl_Exclude_Loads_Off[load_index], TRUE);
            }
        }
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * This method checks if Disable Loads event occur and set load request for loads listed in HBL_KEEP_LOADS_OFF_LIST to OFF
 *
 */
void KeepLoadsOffHandler(void)
{
    if((SRDATA_OK == SRData__CheckByte(&Hbl_Keep_Loads_Off, &N_Hbl_Keep_Loads_Off)) &&
        (SRDATA_OK == SRData__CheckByte(&Hbl_Load_Requests_Progress, &N_Hbl_Load_Requests_Progress)) &&
        (Hbl_Keep_Loads_Off == TRUE) &&
        (Hbl_Load_Requests_Progress == FALSE))
    {
        for(uint8 index = 0; index < HBL_MAX_NUM_LOADS; index++)
        {
            if((SRDATA_OK == SRData__CheckByte(&Hbl_Load_Current_Priority[index], &N_Hbl_Load_Current_Priority[index])) &&
                #if (HBL_KEEP_LOADS_OFF_EXCLUDE_FEATURE == ENABLED)
                    (SRDATA_OK == SRData__CheckByte(&Hbl_Exclude_Loads_Off[index], &N_Hbl_Exclude_Loads_Off[index])) &&
                    (Hbl_Exclude_Loads_Off[index] == FALSE) &&
                #endif
                (index < Hbl_Num_Loads) &&
                (Hbl_Load_Current_Priority[index] < SRHBL_PRIORITY_CLASSB_LO) &&
                (Hbl_Load_Current_Priority[index] < HBL_KEEP_LOADS_OFF_PRIORITY) )
            {
                HBL_LOAD_TYPE *load_ptr = &Hbl_Load_Cfg[index];
                HBL_PILOT_HANDLER_HOOK_TYPE *pilot_handler_ptr = (void *) &HBL_PILOT_HANDLERS[load_ptr->Pilot_Handler_Index];

                #if (HBL_LOAD_HANDLER_FEATURE == ENABLED)
                if (load_ptr->Load_Handler_Index < ELEMENTS_IN_ARRAY(HBL_LOAD_HANDLERS))
                    {
                        HBL_LOAD_HANDLER_HOOK_TYPE *load_handler_ptr = (void *) &HBL_LOAD_HANDLERS[load_ptr->Load_Handler_Index];
                        load_handler_ptr->Request(load_ptr, (void *) HBL_FLUSH_BUFFER);
                        load_handler_ptr->Process(load_ptr, pilot_handler_ptr);
                    }
                    else
                #endif
                {
                    pilot_handler_ptr->Request(load_ptr, (void *) HBL_FLUSH_BUFFER);
                    pilot_handler_ptr->Process(load_ptr);
                }
            }
        }
    }
}

#endif

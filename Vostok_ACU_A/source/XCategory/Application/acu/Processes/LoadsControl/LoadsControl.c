/**
 * @file
 * @brief       A cross-category process for controlling loads in a cycle.
 *
 * @details     The Loads Control process requests the selected loads on or off based on the F1F0
 *              flags and the specified conditions (if any). The LoadsControl module will request
 *              loads on and off using the LoadsControl process priority.
 *
 *  Control Table:
 *      ---------------------------------------------------------------------------------------
 *      |               | Activation | Activation | Deactivation | Deactivation |             |
 *      |               |  Condition |  Condition |   Condition  |   Condition  |             |
 *      |  F1F0 Option  |  Specified |  Satisfied |   Specified  |   Satisfied  | HBL Request |
 *      ---------------------------------------------------------------------------------------
 *      |               |      No    |     n/a    |       No     |      n/a     |      On     |
 *      |               |     Yes    |      No    |       No     |      n/a     |  No Request |
 *      |               |     Yes    |     Yes    |       No     |      n/a     |      On     |
 *      |               |      No    |     n/a    |      Yes     |       No     |      On     |
 *      |    Activate   |      No    |     n/a    |      Yes     |      Yes     |     Off     |
 *      |               |     Yes    |      No    |      Yes     |       No     | Req Previous|
 *      |               |     Yes    |      No    |      Yes     |      Yes     |     Off     |
 *      |               |     Yes    |     Yes    |      Yes     |       No     |      On     |
 *      |               |     Yes    |     Yes    |      Yes     |      Yes     |      On     |
 *      |-------------------------------------------------------------------------------------|
 *      |               |      The Activation     |       No     |      n/a     |     Off     |
 *      |  Deactivate   |     Condition cannot    |      Yes     |       No     |  No Request |
 *      |               |       be specified      |      Yes     |      Yes     |     Off     |
 *      |-------------------------------------------------------------------------------------|
 *
 *      |----------------------------------------------------------------------------------|
 *      |               |            |  Deactivate  |   Activate   |                       |
 *      |               |  Start Off |     Only     |     Only     |                       |
 *      |----------------------------------------------------------------------------------|
 *      | Pulse Control |      No    |       No     |       No     |    On, Off, Repeat    |
 *      |               |      No    |       No     |      Yes     | On, No Request, Repeat|
 *      |               |      No    |      Yes     |       No     |No Request, Off, Repeat|
 *      |               |     Yes    |       No     |       No     |    Off, On, Repeat    |
 *      |               |     Yes    |       No     |      Yes     | No Request, On, Repeat|
 *      |               |     Yes    |      Yes     |       No     |Off, No Request, Repeat|
 *      |----------------------------------------------------------------------------------|
 *
    @startuml LoadsControl_sm.jpg
        title Loads Control State Machine
        ' Use state = This->State

        [H] --> Initialize

        Initialize: do / SelectOption()
        Initialize --> Activate : evActivate
        Initialize --> Deactivate : evDeactivate
        Initialize -> HysteresisOn : evHysteresisOn
        Initialize --> HysteresisOff : evHysteresisOff
        Initialize -l-> PulseOn : evPulseOn
        Initialize --> PulseOff : evPulseOff

        Activate: do / LoadsByOnCondition()

        Deactivate: do / LoadsByOffCondition()

        HysteresisOn: entry / LoadsOn()
        HysteresisOn: do / CheckForOffCondition()\n\t LoadsOn()
        HysteresisOn --> HysteresisOff : evHysteresisOff

        HysteresisOff: entry / LoadsOff()
        HysteresisOff: do / CheckForOnCondition()\n\t LoadsOff()
        HysteresisOff --> HysteresisOn : evHysteresisOn

        PulseOn: entry / LoadsOn()
        PulseOn: do / CheckForPulseDone()\n\t LoadsOn()
        PulseOn --> PulseOff : evPulseOff

        PulseOff: entry / LoadsOff()
        PulseOff: do / CheckForPulseDone()\n\t LoadsOff()
        PulseOff --> PulseOn : evPulseOn
    @enduml
 *
 *  LoadsControl Process: LOADS_CONTROL_OPTION_ACTIVATE (F1F0 = 00)
 *      Pointer = SF_PTR_ACU_PROCESS_LOADS_CONTROL, Displacement = Lo Statement
 *      |--------|---------------------------------------|---------------------------------------|
 *      |        | 15 | 14 | 13 | 12 | 11 | 10 |  9 |  8 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |
 *      | +0000h | Ext|                           Bitmap of loads                                |
 *      | +0001h | Ext|                   Bitmap extension for more loads                        |  (Optional)
 *      | +0002h |  0 |                Final bitmap extension for more loads                     |  (Optional)
 *      |        |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |
 *      |  W+00h |       Activation Condition Index      |              Loads Index              |  (Optional)
 *      |  W+01h |      Deactivation Condition Index     | NR |                0                 |  (Optional)
 *      |--------|---------------------------------------|---------------------------------------|
 *
 *  LoadsControl Process: LOADS_CONTROL_OPTION_DEACTIVATE (F1F0 = 01)
 *      Pointer = SF_PTR_ACU_PROCESS_LOADS_CONTROL, Displacement = Lo Statement
 *      |--------|---------------------------------------|---------------------------------------|
 *      |        | 15 | 14 | 13 | 12 | 11 | 10 |  9 |  8 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |
 *      | +0000h | Ext|                           Bitmap of loads                                |
 *      | +0001h | Ext|                   Bitmap extension for more loads                        |  (Optional)
 *      | +0002h |  0 |                Final bitmap extension for more loads                     |  (Optional)
 *      |        |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |
 *      |  W+01h |      Deactivation Condition Index     |                   0                   |  (Optional)
 *      |--------|---------------------------------------|---------------------------------------|
 *
 *  LoadsControl Process: LOADS_CONTROL_OPTION_PULSE (F1F0 = 10)
 *      Pointer = SF_PTR_ACU_PROCESS_LOADS_CONTROL, Displacement = Lo Statement
 *      |--------|---------------------------------------|---------------------------------------|
 *      |        | 15 | 14 | 13 | 12 | 11 | 10 |  9 |  8 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |
 *      | +0000h | Ext|                           Bitmap of loads                                |
 *      | +0001h | Ext|                   Bitmap extension for more loads                        |  (Optional)
 *      | +0002h |  0 |                Final bitmap extension for more loads                     |  (Optional)
 *      |  W+00h |                                 Pulse On Time                                 |
 *      |  W+00h |                                Pulse Off Time                                 |
 *      |        |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |
 *      |  W+02h |              Pilots Index             | OFF|  Drive  | Res|         0         |  (Optional)
 *      |--------|---------------------------------------|---------------------------------------|
 *
 *  Pilots: Pointer = SF_PTR_ACU_PROCESS_LOADS_CONTROL, Displacement = Pilots Index                 (Optional)
 *      |--------|---------------------------------------|---------------------------------------|
 *      |        |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |
 *      | +0000h |        Load Structure Size (3)        |           Load #0 Position            |
 *      | +0001h |          Load #0 Regulation           |   Load #0 Regulation Modifier Index   |
 *      | +0002h |           Load #1 Position            |          Load #1 Regulation           |  (Optional)
 *      | +0003h |   Load #1 Regulation Modifier Index   |                  etc                  |  (Optional)
 *      |--------|---------------------------------------|---------------------------------------|
 *
 * @copyright   Copyright 2015-2020 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "SystemConfig.h"
#include "LoadsControl.h"
#include "LoadsControl_prv.h"

// -- Other Modules --
#include "Condition.h"
#include "Hbl.h"
#include <limits.h>
#include "Log.h"
#include "Modifier.h"
#include "SettingFile.h"
#include <string.h>
#ifndef LOADS_CONTROL_RESUME_PULSING
    #define LOADS_CONTROL_RESUME_PULSING DISABLED
#elif (LOADS_CONTROL_RESUME_PULSING == ENABLED)
    #include "Crc16.h"
    #include "NVData.h"
    #include "Variable.h"
#endif


// There must be support for at least one (1) LoadsControl process in any step.
CT_ASSERT(LOADS_CONTROL_MAX_INSTANCES >= 1U);


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Loads Control State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Loads Control state machine.
typedef enum LOADS_CONTROL_STATE_ENUM
{
    STATE_LOADS_CONTROL_NONE           = 0,

    STATE_LOADS_CONTROL_INITIALIZE     = 1,         //!< (initial_state)
    STATE_LOADS_CONTROL_ACTIVATE       = 2,
    STATE_LOADS_CONTROL_DEACTIVATE     = 3,
    STATE_LOADS_CONTROL_HYSTERESIS_ON  = 4,
    STATE_LOADS_CONTROL_HYSTERESIS_OFF = 5,
    STATE_LOADS_CONTROL_PULSE_ON       = 6,
    STATE_LOADS_CONTROL_PULSE_OFF      = 7,

    STATE_LOADS_CONTROL_END            = 8
} LOADS_CONTROL_STATE_TYPE;

//! The transition variable for the Loads Control state machine.
//! Flag used by the state machine to track when a transition occurs in the middle of a state's
//! Entry() or Do() action that contains more than one function.
static BOOL_TYPE Loads_Control_Transitioned;

// -- Private Function Prototypes used by the state machine for customization functions --
static void CheckForOffCondition(void);
static void CheckForOnCondition(void);
static void CheckForPulseDone(void);
static void LoadsByOffCondition(void);
static void LoadsByOnCondition(void);
static void LoadsOff(void);
static void LoadsOn(void);
static void SelectOption(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventActivate(void);
static void EventDeactivate(void);
static void EventHysteresisOff(void);
static void EventHysteresisOn(void);
static void EventPulseOff(void);
static void EventPulseOn(void);
static void LoadsControlExecute(void);
static void LoadsControlInitialize(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


//  --- Private Declarations ------------------------------------------------------------------------------------------


// -- Precompiler Directive Declarations --

#ifndef LOADSCONTROL_HIGH_REQUEST_WINS
    //! Set to ENABLED to make the priority of the request based on the load value being set.
    //! For On/Off loads, this will make the action to turn a load on (value of 1) a higher priority
    //! than the action to turn a load off (value of 0).
    #define LOADSCONTROL_HIGH_REQUEST_WINS DISABLED
#endif


// -- Private Constant Declarations --

//! The maximum number of times that the LoadsControl process can be used in the same step.
#ifndef LOADS_CONTROL_MAX_INSTANCES
    //! Set this value to the maximum number of instances of the LoadsControl process that can be
    //! executed in a single step of a cycle.
    //! For example, "2" means that the same step can use two instances of the LoadsControl process.
    //! One instance could control a load with hysteresis while the other turned loads on.
    #define LOADS_CONTROL_MAX_INSTANCES 1U
#endif

#if (LOADSCONTROL_HIGH_REQUEST_WINS == ENABLED)
    #ifndef LOADS_CONTROL_MAX_PRIORITY_INCREASE
        //! Set the number of priority levels used by LoadsControl.
        //! Be sure to reserve this number of priorities in the HBL_LOAD_PRIORITY_LIST.
        //! HBL_LOAD_PRIORITY_LIST can be found in HblDefs_prm.h.
        #define LOADS_CONTROL_MAX_PRIORITY_INCREASE 0U
    #endif
#endif

#if (LOADS_CONTROL_RESUME_PULSING == ENABLED)
    //! 11 bits associated with the Id field of the SAVED_INFO_TYPE.
    #define MASK_ID_FIELD (0x07FFU)

    //! The maximum number of resumable nested loads control processes.
    #define RESUME_CYCLE_COUNT 3U

    #ifndef RESUME_ON_TIME_MIN
        //! Minimum number of deciseconds that a pulsed load can be resumed.
        #define RESUME_ON_TIME_MIN 20
    #endif

#endif

//! The name of this module for use by the Log module.
#define MODULE_NAME MODULE_LOADS_CONTROL

//! The number of bits that can represent loads on a 16-bit word in the Setting File Loads_Bitmap.
#define NUM_OF_LOAD_BITS_PER_WORD 15U

//! Byte offset into the pilot parameters (analog configuration) displacement data to
//! the number of bytes consumed by each configured pilot.
#define OFFSET_ANALOG_CONFIG_SIZE 0

//! Byte offset into the pilot parameters (analog configuration) displacement data to
//! the first pilot's configuration.
#define OFFSET_FIRST_ANALOG_CONFIG 1

//! Value to pass to GetNextLoadIndex() to get the first load.
#define REQUEST_FIRST_INDEX ((uint8)-1)

//! Method used by LoadsControl to drive loads.
#define DRIVE_ON_AND_OFF 0U
#define DRIVE_ON_ONLY    1U
#define DRIVE_OFF_ONLY   2U


// -- Private Enumerated Constant Declarations --


//! The types of LoadsControl variations that are available.
typedef enum LOADS_CONTROL_OPTION_ENUM
{
    LOADS_CONTROL_OPTION_ACTIVATE,
    LOADS_CONTROL_OPTION_DEACTIVATE,
    LOADS_CONTROL_OPTION_PULSE_CONTROL,
    LOADS_CONTROL_OPTION_COUNT
} LOADS_CONTROL_OPTION_TYPE;

//! The possible types of requests for a LoadsControl process.
typedef enum LOADS_CONTROL_REQUEST_ENUM
{
    LOADS_CONTROL_REQUEST_OFF       = -1,
    LOADS_CONTROL_REQUEST_MAINTAIN  =  0,
    LOADS_CONTROL_REQUEST_ON        =  1
} LOADS_CONTROL_REQUEST_TYPE;

//! A list of log message IDs that are generated by this module.
typedef enum MODULE_LOADS_CONTROL_LOG_MESSAGE_ID_ENUM
{
    PROCESS_CONTROL_IS_NULL         = 1,
    PROCESS_CONTROL_PARAMS_EXCEED_LIMITS,
    SF_BAD_DISPLACEMENT,
    SF_BAD_LOADS_DEFINITION,
    SF_PULSE_TIMES_MISSING,
    SF_BAD_ANALOG_CONFIG_DEFINITION,
    TRACE_STATE                     = 128
} MODULE_LOADS_CONTROL_LOG_MESSAGE_ID_TYPE;


// -- Private Structure Type Declarations --

#if (LOADS_CONTROL_RESUME_PULSING == ENABLED)
    //! The non-volatile data required to resume the pulsed Loads Control process.
    typedef struct SAVED_INFO_STRUCT
    {
        uint16 Deciseconds;
        uint16 State        : 4;
        uint16 Result       : 1;
        uint16 Id           : 11;
    } SAVED_INFO_TYPE;
#endif

//! The recoverable RAM data structure required for each instance of the Loads Control process.
typedef struct LOADS_CONTROL_INSTANCE_STRUCT
{
    uint16* Loads_Bitmap;
    uint8* Analog_Config;
    uint16 Deciseconds;
    uint8 Off_Condition;
    uint8 On_Condition;
    uint8 Number_Of_Load_Words;
    uint8 Number_Of_Analog_Loads;
    LOADS_CONTROL_STATE_TYPE State;
    uint8 F1_F0_Option              : 2;
    uint8 Drive_Method              : 2;
    uint8 Process_Can_Resume        : 1;
    uint8 Result                    : 1;
    #if (LOADS_CONTROL_RESUME_PULSING == ENABLED)
        uint8 Resume_Truncate_On        : 1;
    #else
        uint8 Unused                    : 1;
    #endif
    uint8 Start_Pulse_Off           : 1;
} LOADS_CONTROL_INSTANCE_TYPE;

//! The start of the pilot parameters (analog configuration) displacement data for each pilot.
typedef struct SF_ANALOG_LOAD_STRUCT
{
    uint8 Index;
    uint8 Value_Lo;
    uint8 Modifier;
    uint8 Value_Hi;
} SF_ANALOG_CONFIG_TYPE;

//! Parameters for F1F0 = LOADS_CONTROL_OPTION_ACTIVATE after the Loads_Bitmap section.
typedef struct SF_PARAMS_ACTIVATE_STRUCT
{
    uint8 On_Condition;
    uint8 Analog_Config_Displ;
    uint8 Off_Condition;
    uint8 Unused                    : 7;
    uint8 Drive_Method              : 1;
} SF_PARAMS_ACTIVATE_TYPE;

//! Parameters for F1F0 = LOADS_CONTROL_OPTION_PULSE_CONTROL after the Loads_Bitmap section.
typedef struct SF_PARAMS_PULSE_STRUCT
{
    uint16 Deciseconds_On;
    uint16 Deciseconds_Off;
    uint8 Analog_Config_Displ;
    uint8 Unused1                   : 4;
    uint8 Process_Can_Resume        : 1;
    uint8 Drive_Method              : 2;
    uint8 Start_Off                 : 1;
} SF_PARAMS_PULSE_TYPE;


#ifdef LOADS_CONTROL_PRIORITY_LIST
//! The structure required to associate the load id and priority in the LOADS_CONTROL_PRIORITY_LIST table.
    typedef struct LOADS_CONTROL_PRIORITY_STRUCT
    {
        HBL_LOAD_ENUM_TYPE Load_Id;
        HBL_PRIORITY_TYPE Load_Priority;
    } LOADS_CONTROL_PRIORITY_TYPE;
#endif

//  --- Private Definitions -------------------------------------------------------------------------------------------


// -- Private Variable Definitions --

//! The volatile data required by the LoadsControl module.
static LOADS_CONTROL_INSTANCE_TYPE Loads_Control[APPLIANCE_MAX_COMPARTMENTS][LOADS_CONTROL_MAX_INSTANCES];

//! A pointer to the LoadsControl data instance related to the call to LoadsControl__Process().
static LOADS_CONTROL_INSTANCE_TYPE* This;

#if (LOADS_CONTROL_RESUME_PULSING == ENABLED)
    #pragma location = "_appliance_segment"
    //! The non-volatile data required by the LoadsControl module.
    static SAVED_INFO_TYPE Loads_Control_Nv[APPLIANCE_MAX_COMPARTMENTS][LOADS_CONTROL_MAX_INSTANCES][RESUME_CYCLE_COUNT];

    //! Save/restore information for an instance of the Pulse LoadsControl process.
    //! Saved points to a stack-based structure where the first element (index 0) is active
    //! and the other elements may be restored in the future.
    static SAVED_INFO_TYPE* Saved;
#endif

//! A constant required to get the priority of the load defined in the LOADS_CONTROL_PRIORITY_LIST.
#ifdef LOADS_CONTROL_PRIORITY_LIST
    static const LOADS_CONTROL_PRIORITY_TYPE LOADS_PRIORITY[] = {
                                                                    LOADS_CONTROL_PRIORITY_LIST
                                                                };
#endif
//  --- Private Function Prototypes -----------------------------------------------------------------------------------

static LOADS_CONTROL_REQUEST_TYPE CheckHysteresisConditions(void);
static uint8 CountNumOfLoadWords(uint8 max_num_of_load_words);
static void DriveSelectedLoads(uint8 requested_load_value);
static sint32 GetAnalogValue(uint8 load_index);
static uint8 GetNextLoadIndex(uint8 previous_load_index);
static uint16 GetPulseDeciseconds(ON_OFF_TYPE pulse);
static void ReadLoadsControlParameters(PROCESS_CONTROL_TYPE* control);
static uint8 SetActivateParams(void* params, uint8 params_size);
static void SetDeactivateParams(void* params, uint8 params_size);
static uint8 SetPulseParams(void* params, uint8 params_size);
static BOOL_TYPE ValidateProcessControl(PROCESS_CONTROL_TYPE* control);
static HBL_PRIORITY_TYPE FindLoadPriority(uint8 load_index);
#if (LOADS_CONTROL_RESUME_PULSING == ENABLED)
    static uint16 CalculateHash(PROCESS_CONTROL_TYPE* control);
    static BOOL_TYPE SavedStateFind(PROCESS_CONTROL_TYPE* control, uint16 id);
    static void SavedStatePop(void);
    static void SavedStatePush(void);
#endif


//=====================================================================================================================
//  --- Public Functions ----------------------------------------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * Initialize the LoadsControl module's non-volatile data.
 * Called after reset.
 */
void LoadsControl__Initialize(void)
{
#if (LOADS_CONTROL_RESUME_PULSING == ENABLED)
    NVDATA_RESTORE_TYPE nvdata_restore = NVData__GetLastRestoreStatus()->Bit.Appliance_Data_Recovered;
    if (nvdata_restore != NVDATA_RESTORE_OKAY)
    {
        memset(Loads_Control_Nv, 0, sizeof(Loads_Control_Nv));
    }
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * The LoadsControl Process
 *
 * @param control = The control parameters provided by the Interpreter.
 * @retval PROCESS_COMPLETE = The process is ready for the Interpreter to proceed to the next step.
 * @retval PROCESS_BUSY = The process is not ready for the Interpreter to proceed to the next step.
 */
PROCESS_RESULT_TYPE LoadsControl__Process(PROCESS_CONTROL_TYPE* control)
{
    PROCESS_RESULT_TYPE result = PROCESS_COMPLETE;
    if (ValidateProcessControl(control))
    {
        This = &Loads_Control[control->Compartment][control->Process_Instance];
#if (LOADS_CONTROL_RESUME_PULSING == ENABLED)
        Saved = Loads_Control_Nv[control->Compartment][control->Process_Instance];
#endif
        if (((uint8)control->Step_Initializing == 1U) ||
            ((uint8)control->Resume_From_Pause == 1U) ||
            ((uint8)control->Resume_From_Reset == 1U))
        {
            memset(This, 0, sizeof(LOADS_CONTROL_INSTANCE_TYPE));
            This->F1_F0_Option = control->F1_F0_Flags;
            ReadLoadsControlParameters(control);
            if (This->Number_Of_Load_Words > 0U)
            {
#if (LOADS_CONTROL_RESUME_PULSING == ENABLED)
                uint16 id = CalculateHash(control);
                if (((uint8)This->Process_Can_Resume == 1U) &&
                    (((uint8)control->Resume_From_Pause == 1U) ||
                     ((uint8)control->Resume_From_Reset == 1U)) &&
                    (SavedStateFind(control, id)))
                {
                    // Restore pulse state information to resume pulsing now.
                    This->Deciseconds = Saved->Deciseconds;
                    This->State = (LOADS_CONTROL_STATE_TYPE)(Saved->State);
                    This->Result = Saved->Result;
                    This->Resume_Truncate_On = ((This->State == STATE_LOADS_CONTROL_PULSE_ON) &&
                                                (1U < This->Deciseconds) &&
                                                (This->Deciseconds <= (uint16)RESUME_ON_TIME_MIN));
                    if ((uint8)This->Resume_Truncate_On == 1U)
                    {
                        This->Deciseconds = 1;
                    }
                }
                else
#endif
                {
#if (LOADS_CONTROL_RESUME_PULSING == ENABLED)
                    if ((uint8)This->Process_Can_Resume == 1U)
                    {
                        SavedStatePush();
                        Saved->Id = id;
                    }
#endif
                    This->Result = (BOOL_TYPE)PROCESS_BUSY;
                    LoadsControlInitialize();
                }
            }
        }

        if (This->Number_Of_Load_Words > 0U)
        {
            LoadsControlExecute();
            result = (PROCESS_RESULT_TYPE)(This->Result);

#if (LOADS_CONTROL_RESUME_PULSING == ENABLED)
            if ((uint8)This->Process_Can_Resume == 1U)
            {
                // Save pulse state information in case it must be resumed later.
                Saved->Deciseconds = This->Deciseconds;
                Saved->State = This->State;
                Saved->Result = This->Result;
            }
#endif
        }
    }
    return result;
}


//=====================================================================================================================
//  --- Private Functions ---------------------------------------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * Determine how the Hysteresis control should behave.
 * This function should only be called if both the On_Condition and the Off_Condition have been
 * specified in the LoadsControl parameters.
 * @retval LOADS_CONTROL_REQUEST_OFF = Loads should be turned off
 * @retval LOADS_CONTROL_REQUEST_ON = Loads should be turned on
 * @retval LOADS_CONTROL_REQUEST_MAINTAIN = Loads should be left unchanged
 */
static LOADS_CONTROL_REQUEST_TYPE CheckHysteresisConditions(void)
{
    LOADS_CONTROL_REQUEST_TYPE result = LOADS_CONTROL_REQUEST_MAINTAIN;

    if (Condition__Evaluate(SF_PTR_ACU_CONDITION_TABLE, This->On_Condition) == CONDITION_RESULT_TRUE)
    {
        result = LOADS_CONTROL_REQUEST_ON;
    }
    else if (Condition__Evaluate(SF_PTR_ACU_CONDITION_TABLE, This->Off_Condition) == CONDITION_RESULT_TRUE)
    {
        result = LOADS_CONTROL_REQUEST_OFF;
    }

    return result;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the number of words that make up the full bitmap of loads for this LoadsControl process.
 * @param max_num_of_load_words = The maximum number of words based on the size of the setting file data.
 * @return The number of words that make up the full bitmap of loads for this LoadsControl process.
 *         If the data is formatted incorrectly, returns 0.
 */
static uint8 CountNumOfLoadWords(uint8 max_num_of_load_words)
{
    uint8 result = 0;
    for (uint8 word_index = 0; word_index < max_num_of_load_words; word_index++)
    {
        uint16 loads_bitmap = This->Loads_Bitmap[word_index];
        if (BIT_TEST(loads_bitmap, NUM_OF_LOAD_BITS_PER_WORD) == 0U)
        {
            result = word_index + 1U;
            break;
        }
    }
    return result;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Update the HBL loads controlled by this process.
 * If loads are requested ON, then applies any specified analog values and modifiers.
 * Respects the specified Drive_Method.
 * @param requested_load_value = ON if the loads should be turned on. OFF if the loads should be turned off.
 */
static void DriveSelectedLoads(uint8 requested_load_value)
{
    uint8 load_index = GetNextLoadIndex(REQUEST_FIRST_INDEX);
    while (load_index != HBL_INDEX_INVALID)
    {
        sint32 load_value = requested_load_value;
        if ((load_value != (sint32)OFF) && (This->Number_Of_Analog_Loads > 0U))
        {
            load_value = GetAnalogValue(load_index);
        }

        uint8 priority = FindLoadPriority(load_index);
#if (LOADSCONTROL_HIGH_REQUEST_WINS == ENABLED)
        if (load_value > (sint32)LOADS_CONTROL_MAX_PRIORITY_INCREASE)
        {
            priority = priority + LOADS_CONTROL_MAX_PRIORITY_INCREASE;
        }
        else if ((load_value > 0) && (load_value <= 0xFF))
        {
            priority = (load_value == 0xFF) ?  0xFFU : (priority + (uint8)load_value);
        }
#endif
        if (((load_value == 0) && (This->Drive_Method != DRIVE_ON_ONLY)) ||
            ((load_value != 0) && (This->Drive_Method != DRIVE_OFF_ONLY)))
        {
            (void)Hbl__SetLoadRequestByIndex(load_index, (HBL_PRIORITY_TYPE)priority, (uint8*)&load_value);
        }
        load_index = GetNextLoadIndex(load_index);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the analog value (if any) for the specified load_index.
 * Pre-conditions for calling this function:
 *      1. The load is being driven ON.
 *      2. LoadsControl parameters have analog configuration data.
 * @param load_index = The index of the load to find in the analog configuration.
 * @return The analog value (if any) for the specified load_index.
 *         If there is no analog value, returns the original ON value.
 */
static sint32 GetAnalogValue(uint8 load_index)
{
    sint32 final_load_value = ON;
    uint8 analog_config_size = This->Analog_Config[OFFSET_ANALOG_CONFIG_SIZE];
    uint8* config = &This->Analog_Config[OFFSET_FIRST_ANALOG_CONFIG];
    for (uint8 analog_index = 0; analog_index < This->Number_Of_Analog_Loads; analog_index++)
    {
        SF_ANALOG_CONFIG_TYPE* analog_config = (SF_ANALOG_CONFIG_TYPE*)(void*)config;
        if (analog_config->Index == load_index)
        {
            if (analog_config_size > 3U)
            {
                uint16 load_value = COMBINE_BYTES(analog_config->Value_Hi, analog_config->Value_Lo);
                final_load_value = (sint32)load_value;
            }
            else
            {
                final_load_value = analog_config->Value_Lo;
            }
            if (analog_config->Modifier != (uint8)SF_DISPL_INVALID_VALUE)
            {
                final_load_value = (sint32)Modifier__ModifyValue((float64)final_load_value,
                                                                 SF_PTR_ACU_MODIFIERS,
                                                                 analog_config->Modifier);
            }
            break;                                  // Break out of the for() loop.
        }
        config += analog_config_size;
    }
    return final_load_value;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Determine the load index for the next load in This->Loads_Bitmap.
 * @param previous_load_index = The most recently used load_index or REQUEST_FIRST_INDEX.
 * @return The load index for the next load in the Loads_Bitmap.
 */
static uint8 GetNextLoadIndex(uint8 previous_load_index)
{
    uint8 result = HBL_INDEX_INVALID;
    previous_load_index++;
    uint8 word_index = previous_load_index / NUM_OF_LOAD_BITS_PER_WORD;
    uint8 first_bit = previous_load_index % NUM_OF_LOAD_BITS_PER_WORD;
    BOOL_TYPE break_the_loop = FALSE;
    for (; word_index < This->Number_Of_Load_Words; word_index++)
    {
        uint16 load_bits = This->Loads_Bitmap[word_index];
        for (uint8 bit_index = first_bit; bit_index < NUM_OF_LOAD_BITS_PER_WORD; bit_index++)
        {
            if (BIT_TEST((uint32)load_bits, bit_index) != 0U)
            {
                result = (uint8)((word_index * NUM_OF_LOAD_BITS_PER_WORD) + bit_index);
                break_the_loop = TRUE;
                break;
            }
        }

        if(break_the_loop)
        {
            break;
        }
        first_bit = 0;
    }
    return result;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the requested pulse time (ON or OFF) in deciseconds.
 * @param pulse = ON to get the pulse on time. OFF to get the pulse OFF time.
 * @return The requested pulse time (ON or OFF) in deciseconds.
 */
static uint16 GetPulseDeciseconds(ON_OFF_TYPE pulse)
{
    SF_PARAMS_PULSE_TYPE* pulse_params = (void*)&This->Loads_Bitmap[This->Number_Of_Load_Words];
    uint16 result = pulse_params->Deciseconds_Off;
    if (pulse == ON)
    {
        result = pulse_params->Deciseconds_On;
    }

    return result;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Read the setting file parameters for this instance of the Loads Control process.
 * @param control = The process control parameters from the Interpreter.
 */
static void ReadLoadsControlParameters(PROCESS_CONTROL_TYPE* control)
{
    SETTINGFILE_LOADER_TYPE loader;
    PASS_FAIL_TYPE result = SettingFile__MSettingBasicLoader(control->Setting_File_Index,
                                                             SF_PTR_ACU_PROCESS_LOADS_CONTROL,
                                                             control->Data_Field_Index,
                                                             &loader);
    if (result == FAIL)
    {
        This->Number_Of_Load_Words = 0;
        LOG_ADD_EXCEPTION(SF_BAD_DISPLACEMENT, control->Data_Field_Index);
    }
    else
    {
        This->Loads_Bitmap = (uint16*)(void*)loader.Data;
        This->Number_Of_Load_Words = CountNumOfLoadWords((uint8)(loader.Length / sizeof(uint16)));
        if (This->Number_Of_Load_Words == 0U)
        {
            LOG_ADD_EXCEPTION(SF_BAD_LOADS_DEFINITION, control->Data_Field_Index);
        }
        else
        {
            uint8 params_size = (uint8)loader.Length - (uint8)(2UL * This->Number_Of_Load_Words);
            void* params = &This->Loads_Bitmap[This->Number_Of_Load_Words];
            uint8 analog_config_displ = SF_DISPL_INVALID_VALUE;
            switch (control->F1_F0_Flags)
            {
                case LOADS_CONTROL_OPTION_ACTIVATE:
                    analog_config_displ = SetActivateParams(params, params_size);
                    break;
                case LOADS_CONTROL_OPTION_DEACTIVATE:
                    SetDeactivateParams(params, params_size);
                    break;
                case LOADS_CONTROL_OPTION_PULSE_CONTROL:
                    analog_config_displ = SetPulseParams(params, params_size);
                    if (This->Number_Of_Load_Words == 0U)
                    {
                        LOG_ADD_EXCEPTION(SF_PULSE_TIMES_MISSING, control->Data_Field_Index);
                    }
                    break;
                default:
                    //Do nothing
                    break;
            }

            This->Number_Of_Analog_Loads = 0;
            if (analog_config_displ != (uint8)SF_DISPL_INVALID_VALUE)
            {
                result = SettingFile__MSettingBasicLoader(control->Setting_File_Index,
                                                          SF_PTR_ACU_PROCESS_LOADS_CONTROL,
                                                          analog_config_displ,
                                                          &loader);
                if ((result == FAIL) || (loader.Length < 4U))
                {
                    This->Number_Of_Load_Words = 0;
                    LOG_ADD_EXCEPTION(SF_BAD_ANALOG_CONFIG_DEFINITION, analog_config_displ);
                }
                else
                {
                    This->Analog_Config = loader.Data;
                    This->Number_Of_Analog_Loads = ((uint8)loader.Length - 1U) / loader.Data[0];
                }
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Set the LoadsControl parameters based on configuration data specific to LOADS_CONTROL_OPTION_ACTIVATE.
 * @param params = Pointer to the configuration data.
 * @param params_size = Number of bytes in the configuration data.
 * @return The displacement index for the analog configuration (or pilot parameters).
 */
static uint8 SetActivateParams(void* params, uint8 params_size)
{
    uint8 analog_config_displ = SF_DISPL_INVALID_VALUE;
    if (params_size >= 2U)
    {
        SF_PARAMS_ACTIVATE_TYPE* act_params = params;
        This->On_Condition = act_params->On_Condition;
        analog_config_displ = act_params->Analog_Config_Displ;
        if (params_size >= 4U)
        {
            This->Off_Condition = act_params->Off_Condition;
            This->Drive_Method = (uint8)act_params->Drive_Method;
        }
        else
        {
            This->Off_Condition = SF_DISPL_INVALID_VALUE;
            This->Drive_Method = DRIVE_ON_AND_OFF;
        }
    }
    else
    {
        This->On_Condition = SF_DISPL_INVALID_VALUE;
        This->Off_Condition = SF_DISPL_INVALID_VALUE;
        This->Drive_Method = DRIVE_ON_AND_OFF;
    }
    return analog_config_displ;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Set the LoadsControl parameters based on configuration data specific to LOADS_CONTROL_OPTION_DEACTIVATE.
 * @param params = Pointer to the configuration data.
 * @param params_size = Number of bytes in the configuration data.
 */
static void SetDeactivateParams(void* params, uint8 params_size)
{
    if (params_size >= 1U)
    {
        This->Off_Condition = *(uint8*)params;
    }
    else
    {
        This->Off_Condition = SF_DISPL_INVALID_VALUE;
    }
    This->On_Condition = SF_DISPL_INVALID_VALUE;
    This->Drive_Method = DRIVE_OFF_ONLY;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Set the LoadsControl parameters based on configuration data specific to LOADS_CONTROL_OPTION_PULSE_CONTROL.
 * @param params = Pointer to the configuration data.
 * @param params_size = Number of bytes in the configuration data.
 * @return The displacement index for the analog configuration (or pilot parameters).
 */
static uint8 SetPulseParams(void* params, uint8 params_size)
{
    uint8 analog_config_displ = SF_DISPL_INVALID_VALUE;
    if (params_size < 4U)
    {
        This->Number_Of_Load_Words = 0;
    }
    else if (params_size >= 6U)
    {
        SF_PARAMS_PULSE_TYPE* pulse_params = params;
        analog_config_displ = pulse_params->Analog_Config_Displ;
        This->Start_Pulse_Off = pulse_params->Start_Off;
        This->Drive_Method = pulse_params->Drive_Method;
        This->Process_Can_Resume = pulse_params->Process_Can_Resume;
    }
    else
    {
        This->Start_Pulse_Off = FALSE;
        This->Drive_Method = DRIVE_ON_AND_OFF;
        This->Process_Can_Resume = FALSE;
    }
    return analog_config_displ;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Validate the PROCESS_CONTROL_TYPE data structure parameters.
 * @param control = Pointer to the PROCESS_CONTROL_TYPE to validate.
 * @return TRUE if the data structure has valid parameters.
 */
static BOOL_TYPE ValidateProcessControl(PROCESS_CONTROL_TYPE* control)
{
    BOOL_TYPE is_ok = FALSE;
    if (control == NULL)
    {
        LOG_ADD_EXCEPTION(PROCESS_CONTROL_IS_NULL, 0);
    }
    else if ((control->Compartment >= APPLIANCE_MAX_COMPARTMENTS) ||
             (control->F1_F0_Flags >= (uint8)LOADS_CONTROL_OPTION_COUNT) ||
             (control->Process_Instance >= LOADS_CONTROL_MAX_INSTANCES))
    {
        if ((uint8)control->Step_Initializing == 1U)
        {
            LOG_ADD_EXCEPTION(PROCESS_CONTROL_PARAMS_EXCEED_LIMITS, 0);
        }
    }
    else
    {
        is_ok = TRUE;
    }
    return is_ok;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Find the load priority from the LOADS_CONTROL_PRIORITY_LIST if the table is defined.
 * @param load_index = Index of the load.
 * @return HBL_PRIORITY_TYPE. The priority associated with the load.
 */
static HBL_PRIORITY_TYPE FindLoadPriority(uint8 load_index)
{
    HBL_PRIORITY_TYPE load_priority = HBL_PRIORITY_LOADS_CONTROLS;

#ifdef LOADS_CONTROL_PRIORITY_LIST

    HBL_LOAD_ENUM_TYPE load_id = Hbl__GetLoadIDByIndex(load_index);

    for (uint8 index = 0; index < ELEMENTS_IN_ARRAY(LOADS_PRIORITY); index++)
    {
      if(load_id == LOADS_PRIORITY[index].Load_Id)
      {
          load_priority = LOADS_PRIORITY[index].Load_Priority;
          break;
      }
    }
#endif
    return load_priority;
}

#if (LOADS_CONTROL_RESUME_PULSING == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 * Use a hash value based on CRC8 as an ID for the specified process control structure.
 * @param control = The specified process control structure
 * @return A hash value based on CRC8 for the specified process control structure.
 */
static uint16 CalculateHash(PROCESS_CONTROL_TYPE* control)
{
    uint16 hash = MASK_ID_FIELD;
    hash = Crc16__ByteProcess(hash, LOBYTE(control->Data_Field_Index));
    hash = Crc16__ByteProcess(hash, HIBYTE(control->Data_Field_Index));
    hash = Crc16__ByteProcess(hash, (uint8)control->Sub_Cycle_Running);
    sint32 state;
    if (Variable__GetValue(VARIABLE_SUPERVISOR_STATE, control->Compartment, &state) == FALSE)
    {
        state = 0;
    }
    hash = Crc16__ByteProcess(hash, LOBYTE(state));
    return (hash & MASK_ID_FIELD);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Scan the Saved stack for data that matches the current Loads Control process.
 * If a match is found, pop that element to the top of the Saved stack.
 * @param control = Specifies which Loads Control process to find.
 * @param id = An identifier for the Loads Control process.
 * @return TRUE if the specified Loads Control process was found and moved to the top of the stack.
 */
static BOOL_TYPE SavedStateFind(PROCESS_CONTROL_TYPE* control, uint16 id)
{
    BOOL_TYPE saved_state_found = FALSE;
    uint8 save_index;
    for (save_index = 0; save_index < RESUME_CYCLE_COUNT; save_index++)
    {
        if ((Saved[save_index].State != (uint16)STATE_LOADS_CONTROL_NONE) &&
            (Saved[save_index].Id == id))
        {
            while (save_index > 0U)
            {
                SavedStatePop();
                save_index--;
            }
            saved_state_found = TRUE;
            break;                                  // Break out of the for() loop.
        }
    }
    return saved_state_found;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Pop (delete) the current Pulse LoadsControl data off of the Saved stack.
 * Use the next Pulse LoadsControl element for the current process.
 */
static void SavedStatePop(void)
{
    Saved[0] = Saved[1];
    Saved[1] = Saved[2];
    memset(&Saved[2], 0, sizeof(SAVED_INFO_TYPE));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Push the current Pulse LoadsControl data down a level on the Saved stack.
 * Create a new Saved stack element for the current Pulse LoadsControl process.
 */
static void SavedStatePush(void)
{
    Saved[2] = Saved[1];
    Saved[1] = Saved[0];
    memset(&Saved[0], 0, sizeof(SAVED_INFO_TYPE));
}
#endif      // #if (LOADS_CONTROL_RESUME_PULSING == ENABLED)


//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * During Hysteresis control, while loads are on, check if the loads should be turned off.
 */
static void CheckForOffCondition(void)
{
    if (CheckHysteresisConditions() == LOADS_CONTROL_REQUEST_OFF)
    {
        EventHysteresisOff();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * During Hysteresis control, while loads are off, check if the loads should be turned on.
 */
static void CheckForOnCondition(void)
{
    if (CheckHysteresisConditions() == LOADS_CONTROL_REQUEST_ON)
    {
        This->Result = (BOOL_TYPE)PROCESS_COMPLETE;
        EventHysteresisOn();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Check if the time for the current pulse (high or low) has completed.
 * If the current pulse has completed, then switch to the other pulse state.
 */
static void CheckForPulseDone(void)
{
    This->Deciseconds--;
    if (This->Deciseconds == 0U)
    {
        ON_OFF_TYPE next_pulse_polarity = (This->State == STATE_LOADS_CONTROL_PULSE_OFF) ? ON : OFF;
        This->Deciseconds = GetPulseDeciseconds(next_pulse_polarity);

        if ((uint8)next_pulse_polarity != (uint8)This->Start_Pulse_Off)
        {
#if (LOADS_CONTROL_RESUME_PULSING == ENABLED)
            if ((uint8)This->Resume_Truncate_On == 0U)
            {
                This->Result = (BOOL_TYPE)PROCESS_COMPLETE;
            }
            else
            {
                This->Resume_Truncate_On = FALSE;
            }
#else
            This->Result = (BOOL_TYPE)PROCESS_COMPLETE;
#endif
        }

        if (next_pulse_polarity == ON)
        {
            EventPulseOn();
        }
        else
        {
            EventPulseOff();
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Request that loads are turned on and/or off based on the Off_Condition.
 */
static void LoadsByOffCondition(void)
{
    uint8 condition = This->Off_Condition;
    if ((condition == (uint8)SF_DISPL_INVALID_VALUE) ||
        (Condition__Evaluate(SF_PTR_ACU_CONDITION_TABLE, condition) == CONDITION_RESULT_TRUE))
    {
        DriveSelectedLoads(OFF);
        if (This->F1_F0_Option == (uint8)LOADS_CONTROL_OPTION_DEACTIVATE)
        {
            This->Result = (BOOL_TYPE)PROCESS_COMPLETE;
        }
    }
    else
    {
        DriveSelectedLoads(ON);
        if (This->F1_F0_Option == (uint8)LOADS_CONTROL_OPTION_ACTIVATE)
        {
            This->Result = (BOOL_TYPE)PROCESS_COMPLETE;
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Request that loads are turned on and/or off based on the On_Condition.
 */
static void LoadsByOnCondition(void)
{
    uint8 condition = This->On_Condition;
    if ((condition == (uint8)SF_DISPL_INVALID_VALUE) ||
        (Condition__Evaluate(SF_PTR_ACU_CONDITION_TABLE, condition) == CONDITION_RESULT_TRUE))
    {
        DriveSelectedLoads(ON);
        This->Result = (BOOL_TYPE)PROCESS_COMPLETE;
    }
    else
    {
        DriveSelectedLoads(OFF);
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Drive all loads selected in the Loads_Bitmap OFF.
 * This function will use analog values and modified analog values where appropriate.
 * This function will respect the Drive_Method.
 */
static void LoadsOff(void)
{
    DriveSelectedLoads(OFF);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Drive all loads selected in the Loads_Bitmap ON.
 * This function will use analog values and modified analog values where appropriate.
 * This function will respect the Drive_Method.
 */
static void LoadsOn(void)
{
    DriveSelectedLoads(ON);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Cause a state transition to the state that handles the requested LoadsControl (F1F0) option.
 */
static void SelectOption(void)
{
    switch ((LOADS_CONTROL_OPTION_TYPE)(This->F1_F0_Option))
    {
        default:
        case LOADS_CONTROL_OPTION_ACTIVATE:
            if ((This->Off_Condition != (uint8)SF_DISPL_INVALID_VALUE) && (This->On_Condition != (uint8)SF_DISPL_INVALID_VALUE))
            {
                if (CheckHysteresisConditions() == LOADS_CONTROL_REQUEST_OFF)
                {
                    EventHysteresisOff();
                }
                else
                {
                    This->Result = (BOOL_TYPE)PROCESS_COMPLETE;
                    EventHysteresisOn();
                }
            }
            else if (This->Off_Condition != (uint8)SF_DISPL_INVALID_VALUE)
            {
                LoadsByOffCondition();
                EventDeactivate();
            }
            else
            {
                LoadsByOnCondition();
                EventActivate();
            }
            break;

        case LOADS_CONTROL_OPTION_DEACTIVATE:
            LoadsByOffCondition();
            EventDeactivate();
            break;

        case LOADS_CONTROL_OPTION_PULSE_CONTROL:
            if ((uint8)This->Start_Pulse_Off == 1U)
            {
                This->Deciseconds = GetPulseDeciseconds(OFF);
                EventPulseOff();
            }
            else
            {
                This->Deciseconds = GetPulseDeciseconds(ON);
                EventPulseOn();
            }
            break;
    }
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Loads Control State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Activate Event for the Loads Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventActivate(void)
{
    switch (This->State)
    {
        case STATE_LOADS_CONTROL_INITIALIZE:
            This->State = STATE_LOADS_CONTROL_ACTIVATE;
            Loads_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of This->State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Deactivate Event for the Loads Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDeactivate(void)
{
    switch (This->State)
    {
        case STATE_LOADS_CONTROL_INITIALIZE:
            This->State = STATE_LOADS_CONTROL_DEACTIVATE;
            Loads_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of This->State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Hysteresis Off Event for the Loads Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventHysteresisOff(void)
{
    switch (This->State)
    {
        case STATE_LOADS_CONTROL_INITIALIZE:
        case STATE_LOADS_CONTROL_HYSTERESIS_ON:
            This->State = STATE_LOADS_CONTROL_HYSTERESIS_OFF;
            LoadsOff();                             // Entry Function.
            Loads_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of This->State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Hysteresis On Event for the Loads Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventHysteresisOn(void)
{
    switch (This->State)
    {
        case STATE_LOADS_CONTROL_INITIALIZE:
        case STATE_LOADS_CONTROL_HYSTERESIS_OFF:
            This->State = STATE_LOADS_CONTROL_HYSTERESIS_ON;
            LoadsOn();                              // Entry Function.
            Loads_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of This->State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Pulse Off Event for the Loads Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventPulseOff(void)
{
    switch (This->State)
    {
        case STATE_LOADS_CONTROL_INITIALIZE:
        case STATE_LOADS_CONTROL_PULSE_ON:
            This->State = STATE_LOADS_CONTROL_PULSE_OFF;
            LoadsOff();                             // Entry Function.
            Loads_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of This->State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Pulse On Event for the Loads Control state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventPulseOn(void)
{
    switch (This->State)
    {
        case STATE_LOADS_CONTROL_INITIALIZE:
        case STATE_LOADS_CONTROL_PULSE_OFF:
            This->State = STATE_LOADS_CONTROL_PULSE_ON;
            LoadsOn();                              // Entry Function.
            Loads_Control_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of This->State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Loads Control state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void LoadsControlExecute(void)
{
    Loads_Control_Transitioned = FALSE;
    switch(This->State)
    {
        case STATE_LOADS_CONTROL_INITIALIZE:
            SelectOption();                         // Do function.
            break;

        case STATE_LOADS_CONTROL_ACTIVATE:
            LoadsByOnCondition();                   // Do function.
            break;

        case STATE_LOADS_CONTROL_DEACTIVATE:
            LoadsByOffCondition();                  // Do function.
            break;

        case STATE_LOADS_CONTROL_HYSTERESIS_ON:
            CheckForOffCondition();                 // Do function.
            if (Loads_Control_Transitioned == FALSE)
            {
                LoadsOn();                          // Do function.
            }
            break;

        case STATE_LOADS_CONTROL_HYSTERESIS_OFF:
            CheckForOnCondition();                  // Do function.
            if (Loads_Control_Transitioned == FALSE)
            {
                LoadsOff();                         // Do function.
            }
            break;

        case STATE_LOADS_CONTROL_PULSE_ON:
            CheckForPulseDone();                    // Do function.
            if (Loads_Control_Transitioned == FALSE)
            {
                LoadsOn();                          // Do function.
            }
            break;

        case STATE_LOADS_CONTROL_PULSE_OFF:
            CheckForPulseDone();                    // Do function.
            if (Loads_Control_Transitioned == FALSE)
            {
                LoadsOff();                         // Do function.
            }
            break;

        default:
            // There is nothing to do for all other values of This->State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Loads Control state machine.
 * This function resets the state machine to the initial state.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void LoadsControlInitialize(void)
{
    This->State = STATE_LOADS_CONTROL_INITIALIZE;
    Loads_Control_Transitioned = TRUE;
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

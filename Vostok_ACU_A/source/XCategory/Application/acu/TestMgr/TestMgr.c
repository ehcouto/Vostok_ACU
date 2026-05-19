/**
 *  @file
 *
 *  @brief      Module responsible to engage EOL and Service routines requested from API005 or FUNC_ID_TEST_CYCLE regulation.
 *              Also responsible of monitoring the execution of the Test Cycles and store the result.
 *

    @startuml{TestMgr_SM.png}
        title Test Manager State Machine
        '' Trace = EVENT
        ' use state = Test_Manager_State

        ' Each state shall check for request to allow switching from EOL to Service or Service to EOL.
        State TEST_CYCLE_INACTIVE
        State TEST_CYCLE_SERVICE_ACTIVE
        State TEST_CYCLE_EOL_ACTIVE

        [*] --> TEST_CYCLE_INACTIVE

        ' TEST_CYCLE_INACTIVE State
            TEST_CYCLE_INACTIVE: do /CheckForRequest()
            TEST_CYCLE_INACTIVE --> TEST_CYCLE_SERVICE_ACTIVE: evEngageService
            TEST_CYCLE_INACTIVE --> TEST_CYCLE_EOL_ACTIVE: evEngageEol

        ' TEST_CYCLE_SERVICE_ACTIVE State
            TEST_CYCLE_SERVICE_ACTIVE: do / CheckForRequest() \n\t CheckForCycleEnd()
            TEST_CYCLE_SERVICE_ACTIVE: entry ActiveEntry
            TEST_CYCLE_SERVICE_ACTIVE: exit / ServiceExit
            TEST_CYCLE_SERVICE_ACTIVE --> TEST_CYCLE_INACTIVE: evEnd
            TEST_CYCLE_SERVICE_ACTIVE --> TEST_CYCLE_EOL_ACTIVE: evEngageEol

        ' TEST_CYCLE_EOL_ACTIVE State
            TEST_CYCLE_EOL_ACTIVE: do / CheckForRequest() \n\t CheckForCycleEnd()
            TEST_CYCLE_EOL_ACTIVE: entry ActiveEntry
            TEST_CYCLE_EOL_ACTIVE: exit / EolExit
            TEST_CYCLE_EOL_ACTIVE --> TEST_CYCLE_INACTIVE: evEnd
            TEST_CYCLE_EOL_ACTIVE --> TEST_CYCLE_SERVICE_ACTIVE: evEngageService

    @enduml

 *  @copyright  Copyright 2015-2017 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"
#include "TestMgr.h"

#include "API019ProductInfo.h"
#include "Micro.h"
#include "Millis.h"
#include "NVData.h"
#include "Regulations.h"
#include "Selector.h"
#include "Supervisor.h"
#include "Variable.h"

#include <String.h>

//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Test Manager State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Test Manager state machine.
typedef enum TEST_MANAGER_STATE_ENUM
{
    STATE_TEST_MANAGER_NONE                      = 0,

    STATE_TEST_MANAGER_TEST_CYCLE_INACTIVE       = 1, //!< (initial_state)
    STATE_TEST_MANAGER_TEST_CYCLE_SERVICE_ACTIVE = 2,
    STATE_TEST_MANAGER_TEST_CYCLE_EOL_ACTIVE     = 3,

    STATE_TEST_MANAGER_END                       = 4
} TEST_MANAGER_STATE_TYPE;

//! The transition variable for the Test Manager state machine.
//! Flag used by the state machine to track when a transition occurs in the middle of a state's
//! Entry() or Do() action that contains more than one function.
static BOOL_TYPE Test_Manager_Transitioned;

// -- Private Function Prototypes used by the state machine for customization functions --
static void ActiveEntry(void);
static void CheckForCycleEnd(void);
static void CheckForRequest(void);
static void EolExit(void);
static void ServiceExit(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventEnd(void);
static void EventEngageEol(void);
static void EventEngageService(void);
static void TestManagerExecute(void);
static void TestManagerInitialize(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

#if (VARIABLE_NUMBER_OF_GENERAL_PURPOSE < 6)
    #error:"The size of the VARIABLE_NUMBER_OF_GENERAL_PURPOSE macro shall be more than 6"
#endif

#define TESTMGR_RESULT_ARRAY_SIZE   6

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//! The state variable for the Test Manager state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
#pragma location = "_appliance_segment"
static TEST_MANAGER_STATE_TYPE Test_Manager_State;

//! Indicates index for EOL test result storage.
#pragma location = "_appliance_segment"
static uint8 Eol_Test_Index;

//! Stores test cycle requested by API005 in communication thread. Managed in Application Thread.
static TESTMGR_CYCLE_TYPE Requested_Test_Cycle;

//! Indicates if test data is stored at end of test.
#pragma location = "_appliance_segment"
static BOOL_TYPE TestMgr_Storage;

//! Stores test elapsed time.
#pragma location = "_appliance_segment"
static uint32 TestMgr_Timer;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module TestMgr and its variables
 *
 */
void TestMgr__Initialize(void)
{
    Requested_Test_Cycle = TESTMGR_CYCLE_NONE;

    if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
    {
        if (NVData__GetLastRestoreStatus()->Bit.Appliance_Data_Recovered != NVDATA_RESTORE_OKAY)
        {
            Eol_Test_Index = 0;
            TestMgr_Storage = FALSE;
            TestMgr_Timer = 0;
            TestManagerInitialize();
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Handler method which monitors the current cycle and phase and the test result variables.
 */
void TestMgr__Handler(void)
{
    TestManagerExecute();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Method used to start a test cycle.
 * @param test
 * @return
 */
PASS_FAIL_TYPE TestMgr__EngageTest(uint8 compartment, TESTMGR_CYCLE_TYPE test)
{
    PASS_FAIL_TYPE retval = FAIL;

    if (compartment == 0)
    {
        switch (test)
        {
            case TESTMGR_CYCLE_EOL_0:
            case TESTMGR_CYCLE_EOL_1:
            case TESTMGR_CYCLE_EOL_2:
            case TESTMGR_CYCLE_SERVICE_0:
            case TESTMGR_CYCLE_SERVICE_1:
                // This function is called in the communication thread by API005. Only the requested cycle is set here.
                // Engaging the test cycle is delayed in order to run it in the application thread.
                Requested_Test_Cycle = test;
                retval = PASS;
                break;

            case TESTMGR_DISENGAGE:
                retval = Supervisor__SetStart(0, FS_RESET_CONDITION);
                Requested_Test_Cycle = TESTMGR_CYCLE_NONE;
                break;

            default:
                break;
        }
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Method user to set the Phase of a current cycle
 * @param phase
 * @return
 */
PASS_FAIL_TYPE TestMgr__SetPhase(uint8 compartment, TESTMGR_PHASE_TYPE phase)
{
    // Set Phase
    return (Supervisor__SetPhase(compartment, phase));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Method user to set the Step of a current phase
 * @param step
 * @return
 */
PASS_FAIL_TYPE TestMgr__SetStep(uint8 compartment, TESTMGR_PHASE_TYPE step)
{
    // Set Phase
    return (Supervisor__SetStep(compartment, step));
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Method used to disable the internal automatic fill of the Test Result information
 * @param state
 */
void TestMgr__SetStorage(BOOL_TYPE state)
{
    TestMgr_Storage = state;
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void CheckForRequest(void)
{
    // Test cycles only run from compartment 0.
    uint8 position = Regulations__GetPositionByFunctionID(FUNC_ID_TEST_CYCLE, 0);
    PASS_FAIL_TYPE result = FAIL;
    BOOL_TYPE regulation_changed = FALSE;

    if (position != REGULATION_INVALID)
    {
        if (Regulations__IsRegulationChanged(position))
        {
            TESTMGR_CYCLE_TYPE value = (TESTMGR_CYCLE_TYPE) Regulations__GetRequestedMainValueByPosition(position);
            if (value == TESTMGR_CYCLE_NONE)
            {
                Requested_Test_Cycle = TESTMGR_CYCLE_NONE;
                Regulations__AcceptRequestByPosition(position);
            }
            else if (value <= TESTMGR_DISENGAGE)
            {
                regulation_changed = TRUE;
                Requested_Test_Cycle = value;
            }
            else
            {
                Regulations__RejectRequestByPosition(position);
            }
        }
    }

    if (Requested_Test_Cycle != TESTMGR_CYCLE_NONE)
    {
        switch (Requested_Test_Cycle)
        {
            case TESTMGR_CYCLE_EOL_0:
            case TESTMGR_CYCLE_EOL_1:
            case TESTMGR_CYCLE_EOL_2:
                result = Supervisor__EngageTest(Selector__GetCycleByName(0,  0, (CYCLE_NAME_TYPE)((CYCLE_NAME_TYPE)Requested_Test_Cycle + CYCLE_NAME_EOL_0)));
                if (result == PASS)
                {
                    EventEngageEol();
                    Eol_Test_Index = Requested_Test_Cycle;
                }
                break;

            case TESTMGR_CYCLE_SERVICE_0:
            case TESTMGR_CYCLE_SERVICE_1:
                result = Supervisor__EngageTest(Selector__GetCycleByName(0, 0, (CYCLE_NAME_TYPE)((Requested_Test_Cycle - TESTMGR_CYCLE_SERVICE_0) + CYCLE_NAME_SERVICE_0)));
                if (result == PASS)
                {
                    EventEngageService();
                }
                break;

            case TESTMGR_DISENGAGE:
                result = Supervisor__SetStart(0, FS_RESET_CONDITION);
                break;

            default:
                break;
        }

        if (regulation_changed == TRUE)
        {
            if (result == PASS)
            {
                Regulations__AcceptRequestByPosition(position);
            }
            else
            {
                Regulations__RejectRequestByPosition(position);
            }
        }

        Requested_Test_Cycle = TESTMGR_CYCLE_NONE;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void CheckForCycleEnd(void)
{
    if (Supervisor__IsTestCycleActive() == FALSE)
    {
        EventEnd();
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void ActiveEntry(void)
{
    TestMgr_Storage = TRUE;
    TestMgr_Timer = Millis__GetFreeRunningCounter();

    //Initialize the "Variable_General_Purpose"
    for (uint8 index = 0; index < TESTMGR_RESULT_ARRAY_SIZE; index++)
    {
        Variable__SetValue(VARIABLE_OPERATION_SET, VARIABLE_USER_WORD, index, 0);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void EolExit(void)
{
    if (TestMgr_Storage == TRUE)
    {
        API019_TEST_DATA_TYPE *pt_eol = API019ProductInfo__GetEOLDataReference(Eol_Test_Index);
        if (pt_eol != NULL)
        {
            pt_eol->Tester_ID = 0xFE;
            pt_eol->Tester_Version[0] = 0xFE;
            pt_eol->Tester_Version[1] = 0xFE;
            pt_eol->Tester_Version[2] = 0xFE;
            memset(pt_eol->Test_Date,0,sizeof(pt_eol->Test_Date));

            //copy the "Variable_General_Purpose" to the NVData
            sint32 test_result = 0;
            uint16 * result_ptr = (uint16 *)(void *)&(pt_eol->Test_Result);
            for (uint8 index = 0; index < TESTMGR_RESULT_ARRAY_SIZE; index++)
            {
                Variable__GetValue(VARIABLE_USER_WORD, index, &(test_result));
                // Store as big endian for Reveal
                result_ptr[index] = ENDIAN16_SWAP((uint16)test_result);
            }

            #if (NVDATA_STORE_STATIC_FEATURE == ENABLED)
                NVData__StoreStaticData();
            #endif
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void ServiceExit(void)
{
    if (TestMgr_Storage == TRUE)
    {
        // End of Execution of the Test Cycle
        API019_SERVICE_DATA_TYPE *pt_service = API019ProductInfo__GetServiceDataReference();
        if (pt_service != NULL)
        {
            pt_service->Test_Duration = (uint16)((Millis__GetFreeRunningCounter() - TestMgr_Timer) / 1000);
            pt_service->Test_Age = 0;
            memset(pt_service->Test_Date, 0, sizeof(pt_service->Test_Date));
            pt_service->Tester_ID = 0xFE;
            pt_service->Tester_Version[0] = 0xFE;
            pt_service->Tester_Version[1] = 0xFE;
            pt_service->Tester_Version[2] = 0xFE;
            memset(pt_service->Test_Result, 0, sizeof(pt_service->Test_Result));


            //copy the "Variable_General_Purpose" to the NVData
            sint32 test_result = 0;
            uint16 * result_ptr = (uint16 *)(void *)&(pt_service->Test_Result);
            for (uint8 index = 0; index < TESTMGR_RESULT_ARRAY_SIZE; index++)
            {
                Variable__GetValue(VARIABLE_USER_WORD, index, &(test_result));
                // Store as big endian for Reveal
                result_ptr[index] = ENDIAN16_SWAP((uint16)test_result);
            }

            NVData__ForceBlockingStore();
        }
    }
}

//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Test Manager State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the End Event for the Test Manager state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventEnd(void)
{
    switch (Test_Manager_State)
    {
        case STATE_TEST_MANAGER_TEST_CYCLE_SERVICE_ACTIVE:
            ServiceExit();                          // Exit function.
            Test_Manager_State = STATE_TEST_MANAGER_TEST_CYCLE_INACTIVE;
            Test_Manager_Transitioned = TRUE;
            break;

        case STATE_TEST_MANAGER_TEST_CYCLE_EOL_ACTIVE:
            EolExit();                              // Exit function.
            Test_Manager_State = STATE_TEST_MANAGER_TEST_CYCLE_INACTIVE;
            Test_Manager_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Test_Manager_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Engage Eol Event for the Test Manager state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventEngageEol(void)
{
    switch (Test_Manager_State)
    {
        case STATE_TEST_MANAGER_TEST_CYCLE_INACTIVE:
            Test_Manager_State = STATE_TEST_MANAGER_TEST_CYCLE_EOL_ACTIVE;
            ActiveEntry();                          // Entry Function.
            Test_Manager_Transitioned = TRUE;
            break;

        case STATE_TEST_MANAGER_TEST_CYCLE_SERVICE_ACTIVE:
            ServiceExit();                          // Exit function.
            Test_Manager_State = STATE_TEST_MANAGER_TEST_CYCLE_EOL_ACTIVE;
            ActiveEntry();                          // Entry Function.
            Test_Manager_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Test_Manager_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Engage Service Event for the Test Manager state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventEngageService(void)
{
    switch (Test_Manager_State)
    {
        case STATE_TEST_MANAGER_TEST_CYCLE_INACTIVE:
            Test_Manager_State = STATE_TEST_MANAGER_TEST_CYCLE_SERVICE_ACTIVE;
            ActiveEntry();                          // Entry Function.
            Test_Manager_Transitioned = TRUE;
            break;

        case STATE_TEST_MANAGER_TEST_CYCLE_EOL_ACTIVE:
            EolExit();                              // Exit function.
            Test_Manager_State = STATE_TEST_MANAGER_TEST_CYCLE_SERVICE_ACTIVE;
            ActiveEntry();                          // Entry Function.
            Test_Manager_Transitioned = TRUE;
            break;

        default:
            // Event is ignored for all other values of Test_Manager_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Test Manager state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void TestManagerExecute(void)
{
    Test_Manager_Transitioned = FALSE;
    switch(Test_Manager_State)
    {
        case STATE_TEST_MANAGER_TEST_CYCLE_INACTIVE:
            CheckForRequest();                      // Do function.
            break;

        case STATE_TEST_MANAGER_TEST_CYCLE_SERVICE_ACTIVE:
        case STATE_TEST_MANAGER_TEST_CYCLE_EOL_ACTIVE:
            CheckForRequest();                      // Do function.
            if (Test_Manager_Transitioned == FALSE)
            {
                CheckForCycleEnd();                 // Do function.
            }
            break;

        default:
            // There is nothing to do for all other values of Test_Manager_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Test Manager state machine.
 * This function resets the state machine to the initial state.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void TestManagerInitialize(void)
{
    Test_Manager_State = STATE_TEST_MANAGER_TEST_CYCLE_INACTIVE;
    Test_Manager_Transitioned = TRUE;
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

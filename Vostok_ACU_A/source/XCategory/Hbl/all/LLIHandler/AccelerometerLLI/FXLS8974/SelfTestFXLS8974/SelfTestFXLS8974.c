/**
 *  @file       
 *
 *  @brief      Performs the self test procedure for the FXLS8974 accelerometer.
 *              Procedure (per axis):
 *                  - Read Axis Positive value
 *                  - Read Axis Negative value
 *                  - Calculate Self Test Output Change
 *                      - If within the specification for the given axis and FSR g range, continue test
 *                      - If not within the specification, test ends and fails
 *
 *  @section    Applicable_Documents
 *                  AN13193 Self-test procedure for FXLS896xAF and FXLS897xCF (Rev 2.1)
 *                  FXLS8974CF Datasheet (Rev 1.3)
 *  @details    
 *

    @startuml{FXLS8974_sm.png}
        title SelfTestFXLS8974 State-Machine

        [*] --> ReadPositiveXAxis

'Test X Axis
        ReadPositiveXAxis : entry / SetPositiveXAxis()
        ReadPositiveXAxis : do / MeasurePositiveXAxis()
        ReadPositiveXAxis --> ReadNegativeXAxis : evDataReceived

        ReadNegativeXAxis : entry / SetNegativeXAxis()
        ReadNegativeXAxis : do / MeasureNegativeXAxis()
        ReadNegativeXAxis --> ReadPositiveYAxis : evPassed
        ReadNegativeXAxis --> [*]               : evFailed

'Test Y Axis
        ReadPositiveYAxis : entry / SetPositiveYAxis()
        ReadPositiveYAxis : do / MeasurePositiveYAxis()
        ReadPositiveYAxis --> ReadNegativeYAxis : evDataReceived

        ReadNegativeYAxis : entry / SetNegativeYAxis()
        ReadNegativeYAxis : do / MeasureNegativeYAxis()
        ReadNegativeYAxis --> ReadPositiveZAxis : evPassed
        ReadNegativeYAxis --> [*]               : evFailed

'Test Z Axis
        ReadPositiveZAxis : entry / SetPositiveZAxis()
        ReadPositiveZAxis : do / MeasurePositiveZAxis()
        ReadPositiveZAxis --> ReadNegativeZAxis : evDataReceived

        ReadNegativeZAxis : entry / SetNegativeZAxis()
        ReadNegativeZAxis : do / MeasureNegativeZAxis()
        ReadNegativeZAxis --> [*] : evPassed / ClearSelfTestErrors()
        ReadNegativeZAxis --> [*] : evFailed / IncrementSelfTestErrors()

    @enduml
 *
 *  @copyright  Copyright 2022. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"

#if (HBL_LLI_NUM_ACCELEROMETER > 0)
#include "SelfTestFXLS8974.h"

#include "FXLS8974.h"

//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Self Test FXLS8974 State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Self Test FXLS8974 state machine.
typedef enum SELF_TEST_FXLS8974_STATE_ENUM
{
    STATE_SELF_TEST_FXLS8974_NONE                 = 0,

    STATE_SELF_TEST_FXLS8974_READ_POSITIVE_X_AXIS = 1, //!< (initial_state)
    STATE_SELF_TEST_FXLS8974_READ_NEGATIVE_X_AXIS = 2,
    STATE_SELF_TEST_FXLS8974_READ_POSITIVE_Y_AXIS = 3,
    STATE_SELF_TEST_FXLS8974_READ_NEGATIVE_Y_AXIS = 4,
    STATE_SELF_TEST_FXLS8974_READ_POSITIVE_Z_AXIS = 5,
    STATE_SELF_TEST_FXLS8974_READ_NEGATIVE_Z_AXIS = 6,

    STATE_SELF_TEST_FXLS8974_END                  = 7
} SELF_TEST_FXLS8974_STATE_TYPE;

//! The state variable for the Self Test FXLS8974 state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static SELF_TEST_FXLS8974_STATE_TYPE Self_Test_FXLS8974_State;

// -- Private Function Prototypes used by the state machine for customization functions --
static void ClearSelfTestErrors(void);
static void IncrementSelfTestErrors(void);
static void MeasureNegativeXAxis(void);
static void MeasureNegativeYAxis(void);
static void MeasureNegativeZAxis(void);
static void MeasurePositiveXAxis(void);
static void MeasurePositiveYAxis(void);
static void MeasurePositiveZAxis(void);
static void SetNegativeXAxis(void);
static void SetNegativeYAxis(void);
static void SetNegativeZAxis(void);
static void SetPositiveXAxis(void);
static void SetPositiveYAxis(void);
static void SetPositiveZAxis(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventDataReceived(void);
static void EventFailed(void);
static void EventPassed(void);
static void SelfTestFXLS8974Execute(void);
static void SelfTestFXLS8974Initialize(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

//! Module data
static struct
{
    uint8  Self_Test_Errors;
    sint16 Positive_Measurements[3];
    sint16 Negative_Measurements[3];
}SelfTestFXLS8974;

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Initializes the module variables
 */
void SelfTestFXLS8974__Start(void)
{
    SelfTestFXLS8974Initialize();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Execute the state machine for the self test
 */
void SelfTestFXLS8974__Execute(void)
{
    SelfTestFXLS8974Execute();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Returns the number of self test errors that have occurred. This value is reset when the self test is
 *          executed with no errors.
 */
uint8 SelfTestFXLS8974__GetSelfTestErrors(void)
{
    return(SelfTestFXLS8974.Self_Test_Errors);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Returns if the self test is still active
 */
BOOL_TYPE SelfTestFXLS8974__IsActive(void)
{
    BOOL_TYPE is_active = TRUE;

    if ((Self_Test_FXLS8974_State == STATE_SELF_TEST_FXLS8974_NONE) ||
        (Self_Test_FXLS8974_State == STATE_SELF_TEST_FXLS8974_END))
    {
        is_active = FALSE;
    }

    return(is_active);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================


//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 *  Clear any previous self test errors that have occurred
 */
static void ClearSelfTestErrors(void)
{
    SelfTestFXLS8974.Self_Test_Errors = 0;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  Increment the number of self test errors that have occurred
 */
static void IncrementSelfTestErrors(void)
{
    SelfTestFXLS8974.Self_Test_Errors++;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  Measure the Negative X axis self test value and calculate the self test output change (STOC)
 *      STOC = (Positive Output - Negative Output) / 2
 *  If the STOC is within the range per the specification, continue to the Y Axis test, else fail the self test.
 */
static void MeasureNegativeXAxis(void)
{
    sint16 axis_data[3];
    if (FXLS8974__Read((sint16 *)(&axis_data)) == TRUE)
    {
        // Set the Negative X Axis value
        SelfTestFXLS8974.Negative_Measurements[0] = axis_data[0];

        // Calculate Self Test Output Change (STOC): (Positive Output - Negative Output) / 2
        sint16 self_test_output_change = ((SelfTestFXLS8974.Positive_Measurements[0] - SelfTestFXLS8974.Negative_Measurements[0]) >> 1);

        // Compare STOC to data sheet specification for the given axis and 16g range (Defined in FXLS8974 module)
        if ((self_test_output_change < -52) ||
            (self_test_output_change > -19))
        {
            EventFailed();
        }
        else
        {
            EventPassed();
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  Measure the Negative Y axis self test value and calculate the self test output change (STOC)
 *      STOC = (Positive Output - Negative Output) / 2
 *  If the STOC is within the range per the specification, continue to the X Axis test, else fail the self test.
 */
static void MeasureNegativeYAxis(void)
{
    sint16 axis_data[3];
    if (FXLS8974__Read((sint16 *)(&axis_data)) == TRUE)
    {
        // Set the Negative Y Axis value
        SelfTestFXLS8974.Negative_Measurements[1] = axis_data[1];

        // Calculate Self Test Output Change (STOC): (Positive Output - Negative Output) / 2
        sint16 self_test_output_change = ((SelfTestFXLS8974.Positive_Measurements[1] - SelfTestFXLS8974.Negative_Measurements[1]) >> 1);

        // Compare STOC to data sheet specification for the given axis and 16g range (Defined in FXLS8974 module)
        if ((self_test_output_change < 19) ||
            (self_test_output_change > 52))
        {
            EventFailed();
        }
        else
        {
            EventPassed();
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  Measure the Negative Z axis self test value and calculate the self test output change (STOC)
 *      STOC = (Positive Output - Negative Output) / 2
 *  If the STOC is outside the range per the specification, else fail the self test.
 */
static void MeasureNegativeZAxis(void)
{
    sint16 axis_data[3];
    if (FXLS8974__Read((sint16 *)(&axis_data)) == TRUE)
    {
        // Set the Negative X Axis value
        SelfTestFXLS8974.Negative_Measurements[2] = axis_data[2];

        // Calculate Self Test Output Change (STOC): (Positive Output - Negative Output) / 2
        sint16 self_test_output_change = ((SelfTestFXLS8974.Positive_Measurements[2] - SelfTestFXLS8974.Negative_Measurements[2]) >> 1);

        // Compare STOC to data sheet specification for the given axis and 16g range (Defined in FXLS8974 module)
        if ((self_test_output_change < -178) ||
            (self_test_output_change > -35))
        {
            EventFailed();
        }
        else
        {
            EventPassed();
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  Measure the Positive X axis self test value and store it for calculating the self test output change later
 */
static void MeasurePositiveXAxis(void)
{
    sint16 axis_data[3];
    if (FXLS8974__Read((sint16 *)(&axis_data)) == TRUE)
    {
        // Measure the Positive X Axis value
        SelfTestFXLS8974.Positive_Measurements[0] = axis_data[0];   // Remove the upper byte

        EventDataReceived();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  Measure the Positive Y axis self test value and store it for calculating the self test output change later
 */
static void MeasurePositiveYAxis(void)
{
    sint16 axis_data[3];
    if (FXLS8974__Read((sint16 *)(&axis_data)) == TRUE)
    {
        // Measure the Positive Y Axis value
        SelfTestFXLS8974.Positive_Measurements[1] = axis_data[1];   // Remove the upper byte

        EventDataReceived();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  Measure the Positive Z axis self test value and store it for calculating the self test output change later
 */
static void MeasurePositiveZAxis(void)
{
    sint16 axis_data[3];
    if (FXLS8974__Read((sint16 *)(&axis_data)) == TRUE)
    {
        // Measure the Positive Z Axis value
        SelfTestFXLS8974.Positive_Measurements[2] = axis_data[2];   // Remove the upper byte

        EventDataReceived();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  Start the self test on the negative X axis
 */
static void SetNegativeXAxis(void)
{
    FXLS8974__Enable(FXLS8974_MODE_ST_NEGATIVE_X);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  Start the self test on the negative Y axis
 */
static void SetNegativeYAxis(void)
{
    FXLS8974__Enable(FXLS8974_MODE_ST_NEGATIVE_Y);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  Start the self test on the negative Z axis
 */
static void SetNegativeZAxis(void)
{
    FXLS8974__Enable(FXLS8974_MODE_ST_NEGATIVE_Z);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  Start the self test on the positive X axis
 */
static void SetPositiveXAxis(void)
{
    FXLS8974__Enable(FXLS8974_MODE_ST_POSITIVE_X);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  Start the self test on the positive Y axis
 */
static void SetPositiveYAxis(void)
{
    FXLS8974__Enable(FXLS8974_MODE_ST_POSITIVE_Y);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  Start the self test on the positive Z axis
 */
static void SetPositiveZAxis(void)
{
    FXLS8974__Enable(FXLS8974_MODE_ST_POSITIVE_Z);
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Self Test FXLS8974 State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Data Received Event for the Self Test FXLS8974 state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDataReceived(void)
{
    switch (Self_Test_FXLS8974_State)
    {
        case STATE_SELF_TEST_FXLS8974_READ_POSITIVE_X_AXIS:
            Self_Test_FXLS8974_State = STATE_SELF_TEST_FXLS8974_READ_NEGATIVE_X_AXIS;
            SetNegativeXAxis();                     // Entry Function.
            break;

        case STATE_SELF_TEST_FXLS8974_READ_POSITIVE_Y_AXIS:
            Self_Test_FXLS8974_State = STATE_SELF_TEST_FXLS8974_READ_NEGATIVE_Y_AXIS;
            SetNegativeYAxis();                     // Entry Function.
            break;

        case STATE_SELF_TEST_FXLS8974_READ_POSITIVE_Z_AXIS:
            Self_Test_FXLS8974_State = STATE_SELF_TEST_FXLS8974_READ_NEGATIVE_Z_AXIS;
            SetNegativeZAxis();                     // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Self_Test_FXLS8974_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Failed Event for the Self Test FXLS8974 state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventFailed(void)
{
    switch (Self_Test_FXLS8974_State)
    {
        case STATE_SELF_TEST_FXLS8974_READ_NEGATIVE_X_AXIS:
        case STATE_SELF_TEST_FXLS8974_READ_NEGATIVE_Y_AXIS:
            Self_Test_FXLS8974_State = STATE_SELF_TEST_FXLS8974_NONE; // Terminate Self Test FXLS8974 state machine execution.
            break;

        case STATE_SELF_TEST_FXLS8974_READ_NEGATIVE_Z_AXIS:
            IncrementSelfTestErrors();              // Transition action function.
            Self_Test_FXLS8974_State = STATE_SELF_TEST_FXLS8974_NONE; // Terminate Self Test FXLS8974 state machine execution.
            break;

        default:
            // Event is ignored for all other values of Self_Test_FXLS8974_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Passed Event for the Self Test FXLS8974 state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventPassed(void)
{
    switch (Self_Test_FXLS8974_State)
    {
        case STATE_SELF_TEST_FXLS8974_READ_NEGATIVE_X_AXIS:
            Self_Test_FXLS8974_State = STATE_SELF_TEST_FXLS8974_READ_POSITIVE_Y_AXIS;
            SetPositiveYAxis();                     // Entry Function.
            break;

        case STATE_SELF_TEST_FXLS8974_READ_NEGATIVE_Y_AXIS:
            Self_Test_FXLS8974_State = STATE_SELF_TEST_FXLS8974_READ_POSITIVE_Z_AXIS;
            SetPositiveZAxis();                     // Entry Function.
            break;

        case STATE_SELF_TEST_FXLS8974_READ_NEGATIVE_Z_AXIS:
            ClearSelfTestErrors();                  // Transition action function.
            Self_Test_FXLS8974_State = STATE_SELF_TEST_FXLS8974_NONE; // Terminate Self Test FXLS8974 state machine execution.
            break;

        default:
            // Event is ignored for all other values of Self_Test_FXLS8974_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Self Test FXLS8974 state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void SelfTestFXLS8974Execute(void)
{
    switch(Self_Test_FXLS8974_State)
    {
        case STATE_SELF_TEST_FXLS8974_READ_POSITIVE_X_AXIS:
            MeasurePositiveXAxis();                 // Do function.
            break;

        case STATE_SELF_TEST_FXLS8974_READ_NEGATIVE_X_AXIS:
            MeasureNegativeXAxis();                 // Do function.
            break;

        case STATE_SELF_TEST_FXLS8974_READ_POSITIVE_Y_AXIS:
            MeasurePositiveYAxis();                 // Do function.
            break;

        case STATE_SELF_TEST_FXLS8974_READ_NEGATIVE_Y_AXIS:
            MeasureNegativeYAxis();                 // Do function.
            break;

        case STATE_SELF_TEST_FXLS8974_READ_POSITIVE_Z_AXIS:
            MeasurePositiveZAxis();                 // Do function.
            break;

        case STATE_SELF_TEST_FXLS8974_READ_NEGATIVE_Z_AXIS:
            MeasureNegativeZAxis();                 // Do function.
            break;

        default:
            // There is nothing to do for all other values of Self_Test_FXLS8974_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Self Test FXLS8974 state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void SelfTestFXLS8974Initialize(void)
{
    Self_Test_FXLS8974_State = STATE_SELF_TEST_FXLS8974_READ_POSITIVE_X_AXIS;
    SetPositiveXAxis();                             // Entry Function.
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


#endif // (HBL_LLI_NUM_ACCELEROMETER > 0)

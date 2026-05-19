/**
 * @file
 * @brief
 *
 * @details
 *
 *  Accelerometer Setting File Data Structure:
 *      Pointer = SF_PTR_MCU_MOTOR_APPLICATION_GENERIC_DATA, Displacement = Accelerometer Data (2)
 *      |--------|---------------------------------------|---------------------------------------|
 *      |        |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |
 *      | +0000h |  0 |  0 |  0 |  0 |  BL Average Count |   BL Max Errors   |   BL Max Retries  |
 *      | +0001h |Inv2|  0 |  Dest2  |Inv1|  0 |  Dest1  |   ST Max Errors   |Inv3|  0 |  Dest3  |
 *      |        | 15 | 14 | 13 | 12 | 11 | 10 |  9 |  8 |  7 |  6 |  5 |  4 |  3 |  2 |  1 |  0 |
 *      | +0002h |                                Average Samples                                |
 *      | +0003h |                            Baseline Range Threshold                           |
 *      | +0004h |                               Baseline Minimum X                              |
 *      | +0005h |                               Baseline Minimum Y                              |
 *      | +0006h |                               Baseline Minimum Z                              |
 *      | +0007h |                               Baseline Maximum X                              |
 *      | +0008h |                               Baseline Maximum Y                              |
 *      | +0009h |                               Baseline Maximum Z                              |
 *      |--------|-------------------------------------------------------------------------------|
 *
    @startuml{AccelBaseline_sm.png}
        title Accel Baseline State Machine

        [*] --> Idle

        Idle : entry / LISxDH__Disable()
        Idle : exit / StartTimer()
        Idle --> MeasureBaseline : evBaseline\n/ BaselineSetup()
        Idle --> SelfTestBaseline : evSelfTest

        MeasureBaseline : entry / AverageSetup()
        MeasureBaseline : do / AverageMeasure()
        MeasureBaseline --> MeasureBaseline : evNextAverage
        MeasureBaseline --> Idle : evPassed
        MeasureBaseline --> WaitForRecoveredData : evFailed\n/ FailBaseline()

        WaitForRecoveredData : do / WaitForAllAxesSet()
        WaitForRecoveredData --> Idle : evDataRecovered

        SelfTestBaseline : entry / SelfTestBaselineSetup()
        SelfTestBaseline : do / SelfTestMeasure()
        SelfTestBaseline --> SelfTest0 : evPassed
        SelfTestBaseline --> Idle : evFailed\n/ FailSelfTest()

        SelfTest0 : entry / SelfTest0Setup()
        SelfTest0 : do / SelfTestMeasure()
        SelfTest0 --> SelfTest1 : evPassed
        SelfTest0 --> Idle : evFailed\n/ FailSelfTest()

        SelfTest1 : entry / SelfTest1Setup()
        SelfTest1 : do / SelfTestMeasure()
        SelfTest1 --> Idle : evPassed\n/ ClearSelfTestErrors()
        SelfTest1 --> Idle : evFailed\n/ FailSelfTest()
    @enduml
 *
 * @copyright   Copyright 2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "SystemConfig.h"

#if (HBL_LLI_NUM_ACCELEROMETER > 0)
#include "AccelBaseline.h"

// -- Other Modules --
#include "API018Diagnostic.h"
#include "LISxDH.h"
#include <limits.h>
#include "Log.h"
#include "Millis.h"
#include "SettingFile.h"
#include <stdlib.h>
#include <string.h>

//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Accel Baseline State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Accel Baseline state machine.
typedef enum ACCEL_BASELINE_STATE_ENUM
{
    STATE_ACCEL_BASELINE_NONE                    = 0,

    STATE_ACCEL_BASELINE_IDLE                    = 1, //!< (initial_state)
    STATE_ACCEL_BASELINE_MEASURE_BASELINE        = 2,
    STATE_ACCEL_BASELINE_SELF_TEST_BASELINE      = 3,
    STATE_ACCEL_BASELINE_WAIT_FOR_RECOVERED_DATA = 4,
    STATE_ACCEL_BASELINE_SELF_TEST0              = 5,
    STATE_ACCEL_BASELINE_SELF_TEST1              = 6,

    STATE_ACCEL_BASELINE_END                     = 7
} ACCEL_BASELINE_STATE_TYPE;

//! The state variable for the Accel Baseline state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static ACCEL_BASELINE_STATE_TYPE Accel_Baseline_State;

// -- Private Function Prototypes used by the state machine for customization functions --
static void AverageMeasure(void);
static void AverageSetup(void);
static void BaselineSetup(void);
static void ClearSelfTestErrors(void);
static void FailBaseline(void);
static void FailSelfTest(void);
static void SelfTest0Setup(void);
static void SelfTest1Setup(void);
static void SelfTestBaselineSetup(void);
static void SelfTestMeasure(void);
static void StartTimer(void);
static void WaitForAllAxesSet(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void AccelBaselineExecute(void);
static void AccelBaselineInitialize(void);
static void EventBaseline(void);
static void EventDataRecovered(void);
static void EventFailed(void);
static void EventNextAverage(void);
static void EventPassed(void);
static void EventSelfTest(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


//  --- Private Declarations ------------------------------------------------------------------------------------------


// -- Private Constant Declarations --

//! The number of accelerometer samples that must be discarded when running the self-test.
#define ACCEL_SELF_TEST_DISCARD_COUNT 16

//! The maximum deflection expected during the self-test.
#define ACCEL_SELF_TEST_MAX_DEFLECTION 360U

//! The minimum deflection expected during the self-test.
#define ACCEL_SELF_TEST_MIN_DEFLECTION 17U

//! The number of samples to use while running the self-test.
#define ACCEL_SELF_TEST_SAMPLES 200

//! The value in Accel_Baseline.Axis_Baseline_Updated when X, Y, and Z axis baselines have been updated.
#define ALL_AXES_UPDATED 7U

//! The maximum number of averages that can be used for calculating the baseline.
#define MAX_AVERAGE_COUNT 16

//! The name of this module for use by the Log module.
#define MODULE_NAME MODULE_ACCEL_BASELINE

//! The value used to represent one gravity (1G) of acceleration.
#define ONE_GRAVITY 256


// -- Private Enumerated Constant Declarations --

//! A list of log message IDs that are generated by this module.
typedef enum MODULE_ACCEL_BASELINE_LOG_MESSAGE_ID_ENUM
{
    INVALID_ACCELEROMETER_PARAMETERS = 1,//!< INVALID_ACCELEROMETER_PARAMETERS
    COMMAND_TOOK_TOO_LONG,               //!< COMMAND_TOOK_TOO_LONG
    SELF_TEST_NOT_AVAILABLE,             //!< SELF_TEST_NOT_AVAILABLE
    VALIDATION_TEST_NOT_AVAILABLE,       //!< VALIDATION_TEST_NOT_AVAILABLE
    BASELINE_MEASUREMENT_NOT_AVAILABLE,  //!< BASELINE_MEASUREMENT_NOT_AVAILABLE
    VALIDATION_TEST_FAILED,              //!< VALIDATION_TEST_FAILED
    SELF_TEST_FAILED                     //!< SELF_TEST_FAILED
} MODULE_ACCEL_BASELINE_LOG_MESSAGE_ID_TYPE;


// -- Private Structure Type Declarations --

//! The setting file data structure used by the AccelBaseline module.
// NOTICE: This structure is different than what HA uses on the MCU (removed map parameters since the function to set the map was removed in the acceleromter module)
typedef PACKED struct SF_ACCEL_BASELINE_STRUCT
{
    uint8 Baseline_Max_Retries      : 4;
    uint8 Baseline_Average_Count    : 4;
    uint8 Self_Test_Max_Errors      : 4;
    uint8 Baseline_Max_Errors       : 4;
    uint16 Average_Samples;
    uint16 Baseline_Range_Threshold;
    sint16 Baseline_Minimum[3];
    sint16 Baseline_Maximum[3];
} SF_ACCEL_BASELINE_TYPE;

//! A structure that holds the variables used by this module.
typedef struct ACCEL_BASELINE_VARS_STRUCT
{
    //! A pointer to the setting file data for the accelerometer.
    SF_ACCEL_BASELINE_TYPE* Sf;

    //! The sum of raw accelerometer points used to calculate an average measurement.
    sint32 Sum[3];

    //! A variable used to count time and trigger time-outs.
    uint32 Start_Time;

    //! The maximum amount of time that the active command should take.
    uint32 Timeout_Milliseconds;

    //! The sequential XYZ points measured during the baseline measurement.
    sint16 Point[MAX_AVERAGE_COUNT][3];

    //! The result from a successful baseline measurement attempt.
    sint16 Baseline[3];

    //! The number of raw samples while calculating the average measurement.
    sint16 Samples;

    //! The number of averages that have been calculated.
    uint8 Average_Count;

    //! The 1nd index into the Point[][] array.
    uint8 Average_Index;

    //! Bits that are set to 1 when a baseline has been recovered (from ACU nonvolatile memory).
    uint8 Axis_Baseline_Recovered;

    //! Bits that are set to 1 when a baseline has been updated.
    uint8 Axis_Baseline_Updated;

    //! The number of consecutive times that the self-test has failed.
    uint8 Self_Test_Errors;

    //! The number of consecutive times that the baseline measurement has failed.
    uint8 Baseline_Errors;
} ACCEL_BASELINE_VARS_TYPE;


//  --- Private Definitions -------------------------------------------------------------------------------------------


// -- Private Flash-Based Constant Definitions --

//! Temporary definition of setting file data to use before setting file is updated.
static PACKED const SF_ACCEL_BASELINE_TYPE ACCEL_PARAMS =
{
    4,                                              // uint8 Baseline_Max_Retries      : 4;
    3,                                              // uint8 Baseline_Average_Count    : 4;
    3,                                              // uint8 Self_Test_Max_Errors      : 4;
    3,                                              // uint8 Baseline_Max_Errors       : 4;
    3000,                                           // uint16 Average_Samples;
    25,                                             // uint16 Baseline_Range_Threshold;
    {  -25, -25, 231 },                             // sint16 Baseline_Minimum[3];
    {   25,  25, 281 }                              // sint16 Baseline_Maximum[3];
};


// -- Private Variable Definitions --

//! The variables used by this module.
static ACCEL_BASELINE_VARS_TYPE Accel_Baseline;
CT_ASSERT(ACCEL_BASELINE_SIZE == sizeof(Accel_Baseline.Baseline));


//  --- Private Function Prototypes -----------------------------------------------------------------------------------


//=====================================================================================================================
//  --- Public Functions ----------------------------------------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Initialize the AccelBaseline module variables.
 * Read setting file parameters.
 * Orient the Accelerometer axes coordinate system based on the configuration.
 * Start the AccelBaseline state machine if parameters are successfully read.
 */
void AccelBaseline__Initialize(void)
{
    // Set all module variables to 0.
    memset(&Accel_Baseline, 0, sizeof(Accel_Baseline));
    Accel_Baseline.Baseline[2] = ONE_GRAVITY;
    Accel_Baseline.Sf = (void*)&ACCEL_PARAMS;   // Implementation from HA used setting file pointer in MCU section only; The configuration will be done in this module until there is a reason to move it
    AccelBaselineInitialize();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Run the AccelBaseline module's state machine.
 * Check for a command that is taking too long to run and re-initialize the module if necessary.
 */
void AccelBaseline__Background5ms(void)
{
    if (Accel_Baseline_State != STATE_ACCEL_BASELINE_NONE)
    {
        AccelBaselineExecute();
        if (Accel_Baseline_State != STATE_ACCEL_BASELINE_IDLE)
        {
            uint32 elapsed_time = Millis__GetFreeRunningCounter() - Accel_Baseline.Start_Time;
            if (elapsed_time >= Accel_Baseline.Timeout_Milliseconds)
            {
                LOG_ADD_EXCEPTION(COMMAND_TOOK_TOO_LONG, Accel_Baseline_State);
                EventFailed();
            }
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the measured baseline.
 * @param vector = Pointer to fill with the measured baseline data.
 */
void AccelBaseline__GetBaseline(sint16* vector)
{
    memcpy(vector, Accel_Baseline.Baseline, sizeof(Accel_Baseline.Baseline));
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the address of the measured baseline.
 * @return The address of the measured baseline.
 */
uint8* AccelBaseline__GetBaselineAddress(void)
{
    return (uint8*)(void*)Accel_Baseline.Baseline;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the number of times that the baseline measurement has failed sequentially.
 * The error counter is reset to 0 after a reset.
 * The error counter is reset to 0 each time the baseline measurement passes.
 * The error counter is reset to 0 each time the baseline measurement generates a fault.
 * @return The number of times that the baseline measurement has failed.
 */
uint8 AccelBaseline__GetBaselineErrors(void)
{
    return Accel_Baseline.Baseline_Errors;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the number of times that the self-test has failed sequentially.
 * The error counter is reset to 0 after a reset.
 * The error counter is reset to 0 each time the self-test passes.
 * The error counter is reset to 0 each time the self-test generates a fault.
 * @return The number of times that the self-test has failed.
 */
uint8 AccelBaseline__GetSelfTestErrors(void)
{
    return Accel_Baseline.Self_Test_Errors;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the status of the AccelBaseline module.
 * @return The status of the AccelBaseline module.
 */
ACCEL_BASELINE_STATUS_TYPE AccelBaseline__GetStatus(void)
{
    ACCEL_BASELINE_STATUS_TYPE status;
    switch (Accel_Baseline_State)
    {
        case STATE_ACCEL_BASELINE_IDLE:
            status = ACCEL_IDLE;
            break;
        case STATE_ACCEL_BASELINE_MEASURE_BASELINE:
            status = ACCEL_MEASURING_BASELINE;
            break;
        case STATE_ACCEL_BASELINE_SELF_TEST_BASELINE:
        case STATE_ACCEL_BASELINE_SELF_TEST0:
        case STATE_ACCEL_BASELINE_SELF_TEST1:
            status = ACCEL_RUNNING_SELF_TEST;
            break;
        default:
            status = ACCEL_BASELINE_DISABLED;
            break;
    }

    return(status);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Request that the module measure the baseline acceleration of the machine.
 * @return TRUE if the baseline measurement has been scheduled; otherwise, FALSE.
 */
BOOL_TYPE AccelBaseline__MeasureBaseline(void)
{
    BOOL_TYPE is_measurement_started = FALSE;

    if ((Accel_Baseline_State == STATE_ACCEL_BASELINE_IDLE) &&
        (LISxDH__GetStatus() == ACCELEROMETER_DISABLED))
    {
        EventBaseline();
        is_measurement_started = TRUE;
    }
    else
    {
        LOG_ADD_EXCEPTION(BASELINE_MEASUREMENT_NOT_AVAILABLE, Accel_Baseline_State);
        is_measurement_started = FALSE;
    }

    return(is_measurement_started);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Request that the module run the accelerometer self-test.
 * It is recommended that the self-test only be run after the baseline measurement is completed.
 * @return TRUE if the accelerometer self-test is scheduled; otherwise, FALSE.
 */
BOOL_TYPE AccelBaseline__RunSelfTest(void)
{
    BOOL_TYPE is_self_test_started = FALSE;

    if ((Accel_Baseline_State == STATE_ACCEL_BASELINE_IDLE) &&
        (LISxDH__GetStatus() == ACCELEROMETER_DISABLED))
    {
        EventSelfTest();
        is_self_test_started = TRUE;
    }
    else
    {
        LOG_ADD_EXCEPTION(SELF_TEST_NOT_AVAILABLE, Accel_Baseline_State);
        is_self_test_started = FALSE;
    }

    return(is_self_test_started);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the address of the measured baseline.
 * @return The address of the measured baseline.
 */
void AccelBaseline__SetInitialBaseline(uint8* array, uint8 length)
{
    sint16* nv_baseline = (sint16*)(void*)array;
    for (uint8 axis = 0; axis < 3U; axis++)
    {
        if (length >= sizeof(sint16))
        {
            if (BIT_TEST(Accel_Baseline.Axis_Baseline_Updated, axis) == 0)
            {
                Accel_Baseline.Baseline[axis] = nv_baseline[axis];
                BIT_SET(Accel_Baseline.Axis_Baseline_Recovered, axis);
            }
            length -= sizeof(sint16);
        }
        else
        {
            break;                              // Break out of the for() loop.
        }
    }
}


//=====================================================================================================================
//  --- Private Functions ---------------------------------------------------------------------------------------------
//=====================================================================================================================


//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * Capture accelerometer points to calculate the baseline measurement.
 */
static void AverageMeasure(void)
{
    sint16 vector[3];
    while ((Accel_Baseline.Samples <= Accel_Baseline.Sf->Average_Samples) &&
           (LISxDH__Read(vector)))
    {
        // Add the new accelerometer XYZ point to the running sum of raw points.
        for (uint8 axis = 0; axis < 3U; axis++)
        {
            Accel_Baseline.Sum[axis] += vector[axis];
        }
        Accel_Baseline.Samples++;
    }

    // Check if there is enough data for an average measurement.
    if (Accel_Baseline.Samples >= Accel_Baseline.Sf->Average_Samples)
    {
        // Calculate and save the average measurement data
        for (uint8 axis = 0; axis < 3U; axis++)
        {
            // Skip the axes whose baseline is already updated.
            if (BIT_TEST(Accel_Baseline.Axis_Baseline_Updated, axis) == 0)
            {
                vector[axis] = Accel_Baseline.Sum[axis] / Accel_Baseline.Samples;
                Accel_Baseline.Point[Accel_Baseline.Average_Index][axis] = vector[axis];
            }
        }

        // Increment counters to indicate that there is a new average measurement.
        uint8 count = Accel_Baseline.Sf->Baseline_Average_Count;
        Accel_Baseline.Average_Count++;
        Accel_Baseline.Average_Index++;
        if (Accel_Baseline.Average_Index >= count)
        {
            Accel_Baseline.Average_Index = 0;
        }

        // Determine if there are enough average measurements to calculate a baseline.
        if (Accel_Baseline.Average_Count >= count)
        {
            // Check if the average variation is low enough to update the baselines.
            for (uint8 axis = 0; axis < 3U; axis++)
            {
                // Skip the axes whose baseline is already updated.
                if (BIT_TEST(Accel_Baseline.Axis_Baseline_Updated, axis) == 0)
                {
                    // Calculate the min and max for the last 'count' measurements.
                    sint16 min = SHRT_MAX;
                    sint16 max = SHRT_MIN;
                    for (uint8 index = 0; index < count; index++)
                    {
                        if (min > Accel_Baseline.Point[index][axis])
                        {
                            min = Accel_Baseline.Point[index][axis];
                        }
                        if (max < Accel_Baseline.Point[index][axis])
                        {
                            max = Accel_Baseline.Point[index][axis];
                        }
                    }

                    // Check if the min/max range is within a small enough range.
                    if ((max - min) < Accel_Baseline.Sf->Baseline_Range_Threshold)
                    {
                        sint32 sum = 0;
                        for (uint8 index = 0; index < count; index++)
                        {
                            sum += Accel_Baseline.Point[index][axis];
                        }
                        vector[axis] = sum / count;

                        // Validate the proposed baseline measurement value.
                        min = Accel_Baseline.Sf->Baseline_Minimum[axis];
                        max = Accel_Baseline.Sf->Baseline_Maximum[axis];
                        if ((min <= vector[axis]) && (vector[axis] <= max))
                        {
                            Accel_Baseline.Baseline[axis] = vector[axis];
                            BIT_SET(Accel_Baseline.Axis_Baseline_Updated, axis);
                        }
                    }
                }
            }

            // Check if all baselines have been updated.
            if (Accel_Baseline.Axis_Baseline_Updated == ALL_AXES_UPDATED)
            {
                Accel_Baseline.Baseline_Errors = 0;
                EventPassed();
            }
            // Check if the baseline measurement attempt failed.
            else if (Accel_Baseline.Average_Count >= (count + Accel_Baseline.Sf->Baseline_Max_Retries))
            {
                EventFailed();
            }
            else
            {
                // Need more average measurements for a baseline with a lower noise threshold.
                EventNextAverage();
            }
        }                                           // End baseline measurement calculation attempt.
        else
        {
            // Need more average measurements to ensure a stable signal.
            EventNextAverage();
        }
    }                                               // End new average measurement.
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Setup variables to make an average measurement.
 */
static void AverageSetup(void)
{
    // Reset the variables used to average the accelerometer data.
    Accel_Baseline.Samples = 0;
    memset(Accel_Baseline.Sum, 0, sizeof(Accel_Baseline.Sum));
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Setup variables to make a baseline measurement.
 */
static void BaselineSetup(void)
{
    memset(Accel_Baseline.Point, 0, sizeof(Accel_Baseline.Point));
    Accel_Baseline.Average_Count = 0;
    Accel_Baseline.Average_Index = 0;
    Accel_Baseline.Axis_Baseline_Updated = 0;

    // Setup the accelerometer for the baseline measurement.
    LISxDH__Enable(LISXDH_MODE_NORMAL);

    // Set the baseline measurement timeout.
    // For each average sampling: Accel_Baseline.Sf->Average_Samples
    // Number of averages: Accel_Baseline.Sf->Baseline_Average_Count + Accel_Baseline.Sf->Baseline_Max_Retries
    #define ACCEL_BASELINE_TOTAL_SAMPLES (Accel_Baseline.Sf->Average_Samples * (Accel_Baseline.Sf->Baseline_Average_Count + Accel_Baseline.Sf->Baseline_Max_Retries))
    // Sample rate is based on LISXDH_MODE_400HZ.
    #define ACCEL_BASELINE_MILLISECONDS_PER_SAMPLE (1000.0 / 400.0)
    // Add 25% time for communication failure buffer.
    Accel_Baseline.Timeout_Milliseconds = (uint32)(1.25 * ACCEL_BASELINE_MILLISECONDS_PER_SAMPLE * ACCEL_BASELINE_TOTAL_SAMPLES);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Reset the self-test error counter after running the test and passing.
 */
static void ClearSelfTestErrors(void)
{
    Accel_Baseline.Self_Test_Errors = 0;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The baseline measurement failed.
 * Count the error. If too many errors, then generate a system fault.
 */
static void FailBaseline(void)
{
    if (Accel_Baseline.Baseline_Errors >= Accel_Baseline.Sf->Baseline_Max_Errors)
    {
        Accel_Baseline.Baseline_Errors = 0;
    }
    Accel_Baseline.Baseline_Errors++;
    if (Accel_Baseline.Baseline_Errors >= Accel_Baseline.Sf->Baseline_Max_Errors)
    {
        (void)API018Diagnostic__GenerateFault(FAULT_KEY_ACCELEROMETER_ORIENTATION);
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The self-test failed.
 * Count the error. If too many errors, then generate a system fault.
 */
static void FailSelfTest(void)
{
    if (Accel_Baseline.Self_Test_Errors >= Accel_Baseline.Sf->Self_Test_Max_Errors)
    {
        Accel_Baseline.Self_Test_Errors = 0;
    }
    Accel_Baseline.Self_Test_Errors++;
    if (Accel_Baseline.Self_Test_Errors >= Accel_Baseline.Sf->Self_Test_Max_Errors)
    {
        (void)API018Diagnostic__GenerateFault(FAULT_KEY_ACCELEROMETER_SELF_TEST);
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Setup variables to do the first self-test.
 * The first self-test applies a positive force to the accelerometer X, Y, and Z sensors.
 */
static void SelfTest0Setup(void)
{
    Accel_Baseline.Average_Index = 1;
    Accel_Baseline.Samples = -ACCEL_SELF_TEST_DISCARD_COUNT;
    memset(Accel_Baseline.Sum, 0, sizeof(Accel_Baseline.Sum));
    LISxDH__Enable(LISXDH_MODE_ST_0);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Setup variables to do the second self-test.
 * The second self-test applies a negative force to the accelerometer X, Y, and Z sensors.
 */
static void SelfTest1Setup(void)
{
    Accel_Baseline.Average_Index = 2;
    Accel_Baseline.Samples = -ACCEL_SELF_TEST_DISCARD_COUNT;
    memset(Accel_Baseline.Sum, 0, sizeof(Accel_Baseline.Sum));
    LISxDH__Enable(LISXDH_MODE_ST_1);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Setup variables to do a quick baseline measurement for the self-test.
 */
static void SelfTestBaselineSetup(void)
{
    Accel_Baseline.Average_Index = 0;
    Accel_Baseline.Samples = -ACCEL_SELF_TEST_DISCARD_COUNT;
    memset(Accel_Baseline.Sum, 0, sizeof(Accel_Baseline.Sum));
    LISxDH__Enable(LISXDH_MODE_ST_REF);

    // Set the self-test timeout.
    // For baseline: ACCEL_SELF_TEST_DISCARD_COUNT + ACCEL_SELF_TEST_SAMPLES
    // For positive force test: ACCEL_SELF_TEST_DISCARD_COUNT + ACCEL_SELF_TEST_SAMPLES
    // For negative force test: ACCEL_SELF_TEST_DISCARD_COUNT + ACCEL_SELF_TEST_SAMPLES
    #define ACCEL_SELF_TEST_TOTAL_SAMPLES (3.0 * (ACCEL_SELF_TEST_DISCARD_COUNT + ACCEL_SELF_TEST_SAMPLES))
    // Sample rate is based on LISXDH_MODE_ST_REF, LISXDH_MODE_ST_0, and LISXDH_MODE_ST_1.
    #define ACCEL_SELF_TEST_MILLISECONDS_PER_SAMPLE (1000.0 / 400.0)
    // Add 25% time for communication failure buffer.
    Accel_Baseline.Timeout_Milliseconds = (uint32)(1.25 * ACCEL_SELF_TEST_MILLISECONDS_PER_SAMPLE * ACCEL_SELF_TEST_TOTAL_SAMPLES);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Capture accelerometer points and perform the self-test.
 */
static void SelfTestMeasure(void)
{
    sint16 vector[3];
    while ((Accel_Baseline.Samples < ACCEL_SELF_TEST_SAMPLES) &&
           (LISxDH__Read(vector)))
    {
        Accel_Baseline.Samples++;
        if (0 < Accel_Baseline.Samples)             // Discard first samples while accelerometer stabilizes.
        {
            // Add the new accelerometer XYZ point to the running sum of raw points.
            for (uint8 axis = 0; axis < 3U; axis++)
            {
                Accel_Baseline.Sum[axis] += vector[axis];
            }
        }
    }
    if (Accel_Baseline.Samples >= ACCEL_SELF_TEST_SAMPLES)
    {
        // Save the self-test point in one of the Point[][] positions.
        for (uint8 axis = 0; axis < 3U; axis++)
        {
            Accel_Baseline.Point[Accel_Baseline.Average_Index][axis] = Accel_Baseline.Sum[axis] / Accel_Baseline.Samples;
        }

        BOOL_TYPE test_passed = TRUE;
        if (Accel_Baseline.Average_Index > 0U)       // If 0, then it is the baseline, so skip the test.
        {
            // Compare the new point to the baseline Point[0].
            for (uint8 axis = 0; axis < 3U; axis++)
            {
                // Check that the test's displacement is in the expected range.
                uint16 delta = abs(Accel_Baseline.Point[Accel_Baseline.Average_Index][axis] - Accel_Baseline.Point[0][axis]);
                if ((delta < ACCEL_SELF_TEST_MIN_DEFLECTION) || (ACCEL_SELF_TEST_MAX_DEFLECTION < delta))
                {
                    test_passed = FALSE;
                }
            }
        }

        // Manage test result.
        if (test_passed)
        {
            EventPassed();
        }
        else
        {
            EventFailed();
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Start a timer to time the activities of the current state.
 */
static void StartTimer(void)
{
    Accel_Baseline.Start_Time = Millis__GetFreeRunningCounter();
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Verify for nonvolatile data to set the baseline axes whose measurements failed.
 */
static void WaitForAllAxesSet(void)
{
    if ((Accel_Baseline.Axis_Baseline_Updated | Accel_Baseline.Axis_Baseline_Recovered) == ALL_AXES_UPDATED)
    {
        EventDataRecovered();
    }
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Accel Baseline State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Accel Baseline state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void AccelBaselineExecute(void)
{
    switch(Accel_Baseline_State)
    {
        case STATE_ACCEL_BASELINE_MEASURE_BASELINE:
            AverageMeasure();                       // Do function.
            break;

        case STATE_ACCEL_BASELINE_SELF_TEST_BASELINE:
        case STATE_ACCEL_BASELINE_SELF_TEST0:
        case STATE_ACCEL_BASELINE_SELF_TEST1:
            SelfTestMeasure();                      // Do function.
            break;

        case STATE_ACCEL_BASELINE_WAIT_FOR_RECOVERED_DATA:
            WaitForAllAxesSet();                    // Do function.
            break;

        default:
            // There is nothing to do for all other values of Accel_Baseline_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Accel Baseline state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void AccelBaselineInitialize(void)
{
    Accel_Baseline_State = STATE_ACCEL_BASELINE_IDLE;
    LISxDH__Disable();                       // Entry Function.
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Baseline Event for the Accel Baseline state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventBaseline(void)
{
    switch (Accel_Baseline_State)
    {
        case STATE_ACCEL_BASELINE_IDLE:
            StartTimer();                           // Exit function.
            BaselineSetup();                        // Transition action function.
            Accel_Baseline_State = STATE_ACCEL_BASELINE_MEASURE_BASELINE;
            AverageSetup();                         // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Accel_Baseline_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Data Recovered Event for the Accel Baseline state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDataRecovered(void)
{
    switch (Accel_Baseline_State)
    {
        case STATE_ACCEL_BASELINE_WAIT_FOR_RECOVERED_DATA:
            Accel_Baseline_State = STATE_ACCEL_BASELINE_IDLE;
            LISxDH__Disable();               // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Accel_Baseline_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Failed Event for the Accel Baseline state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventFailed(void)
{
    switch (Accel_Baseline_State)
    {
        case STATE_ACCEL_BASELINE_MEASURE_BASELINE:
            FailBaseline();                         // Transition action function.
            Accel_Baseline_State = STATE_ACCEL_BASELINE_WAIT_FOR_RECOVERED_DATA;
            break;

        case STATE_ACCEL_BASELINE_SELF_TEST_BASELINE:
        case STATE_ACCEL_BASELINE_SELF_TEST0:
        case STATE_ACCEL_BASELINE_SELF_TEST1:
            FailSelfTest();                         // Transition action function.
            Accel_Baseline_State = STATE_ACCEL_BASELINE_IDLE;
            LISxDH__Disable();               // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Accel_Baseline_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Next Average Event for the Accel Baseline state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNextAverage(void)
{
    switch (Accel_Baseline_State)
    {
        case STATE_ACCEL_BASELINE_MEASURE_BASELINE:
            Accel_Baseline_State = STATE_ACCEL_BASELINE_MEASURE_BASELINE;
            AverageSetup();                         // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Accel_Baseline_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Passed Event for the Accel Baseline state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventPassed(void)
{
    switch (Accel_Baseline_State)
    {
        case STATE_ACCEL_BASELINE_MEASURE_BASELINE:
            Accel_Baseline_State = STATE_ACCEL_BASELINE_IDLE;
            LISxDH__Disable();               // Entry Function.
            break;

        case STATE_ACCEL_BASELINE_SELF_TEST_BASELINE:
            Accel_Baseline_State = STATE_ACCEL_BASELINE_SELF_TEST0;
            SelfTest0Setup();                       // Entry Function.
            break;

        case STATE_ACCEL_BASELINE_SELF_TEST0:
            Accel_Baseline_State = STATE_ACCEL_BASELINE_SELF_TEST1;
            SelfTest1Setup();                       // Entry Function.
            break;

        case STATE_ACCEL_BASELINE_SELF_TEST1:
            ClearSelfTestErrors();                  // Transition action function.
            Accel_Baseline_State = STATE_ACCEL_BASELINE_IDLE;
            LISxDH__Disable();               // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Accel_Baseline_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Self Test Event for the Accel Baseline state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventSelfTest(void)
{
    switch (Accel_Baseline_State)
    {
        case STATE_ACCEL_BASELINE_IDLE:
            StartTimer();                           // Exit function.
            Accel_Baseline_State = STATE_ACCEL_BASELINE_SELF_TEST_BASELINE;
            SelfTestBaselineSetup();                // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Accel_Baseline_State.
            break;
    }
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


#endif // (HBL_LLI_NUM_ACCELEROMETER > 0)

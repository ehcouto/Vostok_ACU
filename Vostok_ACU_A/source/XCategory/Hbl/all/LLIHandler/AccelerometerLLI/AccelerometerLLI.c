/**
 *  @file       
 *
 *  @brief      
 *
 *  @details
     @startuml{AccelerometerLLI_sm.png}
        title AccelerometerLLI State-Machine
        ' Suppress transition checks

        [*] --> Discover

        Discover : entry / InitDiscovery()
        Discover : do / WaitForDiscovery()
        Discover --> Run : evDeviceIdentified / SetDeviceInfo()

        Run : do / FastProcess()

    @enduml
 *
 *  @copyright  Copyright 2022. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"
#include "AccelerometerLLI.h"

#if (HBL_LLI_NUM_ACCELEROMETER > 0)
#if HBL_LLI_NUM_ACCELEROMETER != 1
    #error "The AccelerometerLLI module is not designed to handle multiple instances. HBL_LLI_NUM_ACCELEROMETER can only be 1 or 0"
#endif
#include "AccelBaseline.h"
#include "AccelDiscover.h"
#include "FXLS8974.h"
#include <limits.h>
#include "LISxDH.h"
#include "SelfTestFXLS8974.h"
#include <string.h>

//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Accelerometer LLI State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Accelerometer LLI state machine.
typedef enum ACCELEROMETER_LLI_STATE_ENUM
{
    STATE_ACCELEROMETER_LLI_NONE     = 0,

    STATE_ACCELEROMETER_LLI_DISCOVER = 1,           //!< (initial_state)
    STATE_ACCELEROMETER_LLI_RUN      = 2,

    STATE_ACCELEROMETER_LLI_END      = 3
} ACCELEROMETER_LLI_STATE_TYPE;

//! The state variable for the Accelerometer LLI state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static ACCELEROMETER_LLI_STATE_TYPE Accelerometer_LLI_State;

// -- Private Function Prototypes used by the state machine for customization functions --
static void FastProcess(void);
static void InitDiscovery(void);
static void SetDeviceInfo(void);
static void WaitForDiscovery(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void AccelerometerLLIExecute(void);
static void AccelerometerLLIInitialize(void);
static void EventDeviceIdentified(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//! Module data
static struct
{
    uint8                   Allocation;     // Counts the number of inputs that are allocated for use. Used to know when the limitation in the firmware has been reached.
    BOOL_TYPE               Enabled;        // Tracks whether the accelerometer can be read in this module or if it's be read by another module
    ACCELEROMETER_AXIS_TYPE Point;          // Most recent accelerometer data point (can be used for debugging)
    uint8                   Sampling_Rate;  // Rate the P2P algorithm should be sampling
    uint8                   Self_Test_State;// 0: Idle, 1: Disabling, 2: Running Self Test
    uint32                  Timestamp;      // Time (us) when the acceleration data was written
}AccelerometerLLI;

static ACCELEROMETER_DATA_TYPE AccelerometerLLI_Public;     // Data accessed from the Get() function

//! Pointer to setting file data for the LLI
static LISXDH_SF_PARAM_TYPE * AccelerometerLLI_SF_Param;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void SetOutputDataRate(uint8 data_rate);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Initializes the module variables
 */
void AccelerometerLLI__Initialize(void)
{
    AccelerometerLLI.Allocation = 0;
    AccelerometerLLI_Public.Last_Written_Index = ACCELEROMETER_BUFFER_SIZE;  // Setting index to max so first write occurs at index 0
    AccelerometerLLI_Public.Device = ACCELEROMETER_DEVICE_UNKNOWN;              // Set device to invalid value
    AccelBaseline__Initialize();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Determines the destination (index) of the data for the LLI.
 * @return  Index to a local buffer where the data will reside.
 */
uint8 AccelerometerLLI__Allocate(void)
{
    uint8 retval = HBL_INDEX_INVALID;
    if (AccelerometerLLI.Allocation < HBL_LLI_NUM_ACCELEROMETER)
    {
        retval = AccelerometerLLI.Allocation;
        AccelerometerLLI.Allocation++;
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Initialize an instance of the LLI by extracting parameter data and initializing variables.
 * @param   input: data related to the instance of the LLI
 * @param   PASS if the initialization was successful, else FAIL
 */
PASS_FAIL_TYPE AccelerometerLLI__InitializeInstance(HBL_LLI_TYPE * input)
{
    PASS_FAIL_TYPE retval = FAIL;

    // Verify setting file parameters are available
    if (input->Parameter_Ptr.Length != 0)
    {
        AccelerometerLLI_SF_Param = (LISXDH_SF_PARAM_TYPE *)input->Parameter_Ptr.Data;

        AccelerometerLLIInitialize();

        retval = PASS;
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Retrieves the current value of the LLI
 * @param   input: data related to the instance of the LLI
 * @return  The current value of the LLI
 */
void * AccelerometerLLI__Get(HBL_LLI_TYPE * input)
{
    void * ptr = NULL;

    if (input->Data_Index < AccelerometerLLI.Allocation)
    {
        ptr = ((void *)&AccelerometerLLI_Public);
    }

    return (ptr);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Asynchronous processing of the LLI (5ms)
 * @param   input: data related to the instance of the LLI
 * @return  PASS if the LLI was successfully processed, else FAIL
 */
BOOL_TYPE AccelerometerLLI__AsynchProcess(HBL_LLI_TYPE * input)
{
    BOOL_TYPE retval = FALSE;

    if (input->Data_Index < AccelerometerLLI.Allocation)
    {
        switch (AccelerometerLLI.Self_Test_State)
        {
            default:
            case 0:
                // do nothing
                break;
            case 1:
                if (AccelerometerLLI_Public.Device == ACCELEROMETER_DEVICE_LISxDH)
                {
                    if (LISxDH__GetStatus() == ACCELEROMETER_DISABLED)
                    {
                        AccelBaseline__RunSelfTest();
                        AccelerometerLLI.Self_Test_State++;
                        AccelBaseline__Background5ms();
                    }
                }
                else if (AccelerometerLLI_Public.Device == ACCELEROMETER_DEVICE_FXLS8974)
                {
                    if (FXLS8974__GetStatus() == ACCELEROMETER_DISABLED)
                    {
                        SelfTestFXLS8974__Start();
                        AccelerometerLLI.Self_Test_State++;
                        SelfTestFXLS8974__Execute();
                    }
                }
                break;
            case 2:
                if (AccelerometerLLI_Public.Device == ACCELEROMETER_DEVICE_LISxDH)
                {
                    if (AccelBaseline__GetStatus() == ACCEL_IDLE)
                    {
                        AccelerometerLLI_Public.Self_Test_Errors = AccelBaseline__GetSelfTestErrors();
                        AccelerometerLLI__Enable();
                        AccelerometerLLI.Self_Test_State = 0;
                        AccelerometerLLI_Public.Self_Test_Completed = TRUE;
                    }
                    else
                    {
                        AccelBaseline__Background5ms();
                    }
                }
                else if (AccelerometerLLI_Public.Device == ACCELEROMETER_DEVICE_FXLS8974)
                {
                    if (SelfTestFXLS8974__IsActive() == FALSE)
                    {
                        AccelerometerLLI_Public.Self_Test_Errors = SelfTestFXLS8974__GetSelfTestErrors();
                        AccelerometerLLI__Enable();
                        AccelerometerLLI.Self_Test_State = 0;
                        AccelerometerLLI_Public.Self_Test_Completed = TRUE;
                    }
                    else
                    {
                        SelfTestFXLS8974__Execute();
                    }
                }
                break;
        }

        retval = TRUE;
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  Use this function if just running the background process for the Accelerometer sub-module
 */
void AccelerometerLLI__FastProcess(void)
{
    AccelerometerLLIExecute();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
void AccelerometerLLI__Enable(void)
{
    AccelerometerLLI.Enabled = TRUE;
    if (AccelerometerLLI_Public.Device == ACCELEROMETER_DEVICE_LISxDH)
    {
        LISxDH__Enable(LISXDH_MODE_NORMAL);
    }
    else if (AccelerometerLLI_Public.Device == ACCELEROMETER_DEVICE_FXLS8974)
    {
        FXLS8974__Enable(FXLS8974_MODE_NORMAL);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
void AccelerometerLLI__Disable(void)
{
    AccelerometerLLI.Enabled = FALSE;
    if (AccelerometerLLI_Public.Device == ACCELEROMETER_DEVICE_LISxDH)
    {
        LISxDH__Disable();
    }
    else if (AccelerometerLLI_Public.Device == ACCELEROMETER_DEVICE_FXLS8974)
    {
        FXLS8974__Disable();
    }
    memset(&(AccelerometerLLI_Public.Raw), 0, sizeof(AccelerometerLLI_Public.Raw));  // Clear all accelerometer data
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  Method to start the execution of the self test for the accelerometer
 */
void AccelerometerLLI__RunSelfTest(void)
{
    AccelerometerLLI.Self_Test_State = 1;
    AccelerometerLLI_Public.Self_Test_Completed = FALSE;
    AccelerometerLLI__Disable(); // Must disable reading of the accelerometer done by this module
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  Method to get the latest accelerometer x,y,z data with timestamp.
 */
void AccelerometerLLI__GetDataPointer(ACCELEROMETER_AXIS_TYPE * ptr)
{
    ptr->X_Axis = AccelerometerLLI.Point.X_Axis;
    ptr->Y_Axis = AccelerometerLLI.Point.Y_Axis;
    ptr->Z_Axis = AccelerometerLLI.Point.Z_Axis;
    ptr->Timestamp = AccelerometerLLI.Point.Timestamp;
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
/**
 *
 * @param data_rate Output data rate based on LISxDH data sheet for output data rate (ODR) configuration
 */
static void SetOutputDataRate(uint8 data_rate)
{
    if (AccelerometerLLI_Public.Device == ACCELEROMETER_DEVICE_LISxDH)
    {
        switch(data_rate)
        {
            case 0:
            case 1:
                AccelerometerLLI_Public.Output_Data_Rate = data_rate;
                break;
            case 2:
                AccelerometerLLI_Public.Output_Data_Rate = 10;
                break;
            case 3:
                AccelerometerLLI_Public.Output_Data_Rate = 25;
                break;
            case 4:
                AccelerometerLLI_Public.Output_Data_Rate = 50;
                break;
            case 5:
                AccelerometerLLI_Public.Output_Data_Rate = 100;
                break;
            case 6:
                AccelerometerLLI_Public.Output_Data_Rate = 200;
                break;
            case 7:
                AccelerometerLLI_Public.Output_Data_Rate = 400;
                break;
            case 8:
                AccelerometerLLI_Public.Output_Data_Rate = 1620;
                break;
            case 9:
                AccelerometerLLI_Public.Output_Data_Rate = 1344;
                break;
            default:
                AccelerometerLLI_Public.Output_Data_Rate = 0;   // Unknown data rate
                break;
        }
    }
    else if (AccelerometerLLI_Public.Device == ACCELEROMETER_DEVICE_FXLS8974)
    {
        switch(data_rate)
        {
            case 0:
                AccelerometerLLI_Public.Output_Data_Rate = 0;
                break;
            case 1:
                AccelerometerLLI_Public.Output_Data_Rate = 1;  // TODO: Rate is actually 1.5Hz
                break;
            case 2:
                AccelerometerLLI_Public.Output_Data_Rate = 12; // TODO: Rate is actually 12.5Hz
                break;
            case 3:
                AccelerometerLLI_Public.Output_Data_Rate = 25;
                break;
            case 4:
                AccelerometerLLI_Public.Output_Data_Rate = 50;
                break;
            case 5:
                AccelerometerLLI_Public.Output_Data_Rate = 100;
                break;
            case 6:
                AccelerometerLLI_Public.Output_Data_Rate = 200;
                break;
            case 7:
                AccelerometerLLI_Public.Output_Data_Rate = 400;
                break;
            case 8:
                AccelerometerLLI_Public.Output_Data_Rate = 1600;
                break;
            case 9:
//                AccelerometerLLI_Public.Output_Data_Rate = 3200;
                // TODO: Update setting file to use "8" for 1600Hz (3200Hz was too fast)
                AccelerometerLLI_Public.Output_Data_Rate = 1600;
                break;
            default:
                AccelerometerLLI_Public.Output_Data_Rate = 0;   // Unknown data rate
                break;
        }
    }
}


//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void FastProcess(void)
{
    if (AccelerometerLLI_Public.Device == ACCELEROMETER_DEVICE_LISxDH)
    {
        LISxDH__Background();
        AccelerometerLLI_Public.Status = LISxDH__GetStatus();
    }
    else if (AccelerometerLLI_Public.Device == ACCELEROMETER_DEVICE_FXLS8974)
    {
        FXLS8974__Background();
        AccelerometerLLI_Public.Status = FXLS8974__GetStatus();
    }
    // else, do nothing if device is unknown

    if (AccelerometerLLI.Enabled == TRUE)
    {
        BOOL_TYPE accelerometer_retval = FALSE;
        if (AccelerometerLLI_Public.Device == ACCELEROMETER_DEVICE_LISxDH)
        {
            accelerometer_retval = LISxDH__Read((sint16 *)(&(AccelerometerLLI.Point)));
        }
        else if (AccelerometerLLI_Public.Device == ACCELEROMETER_DEVICE_FXLS8974)
        {
            accelerometer_retval = FXLS8974__Read((sint16 *)(&(AccelerometerLLI.Point)));
        }

        if (accelerometer_retval == TRUE)
        {
            AccelerometerLLI_Public.Last_Written_Index++;                                   // Increment to write at next index
            if (AccelerometerLLI_Public.Last_Written_Index >= ACCELEROMETER_BUFFER_SIZE) // Handle wrap around to beginning of buffer
            {
                AccelerometerLLI_Public.Last_Written_Index = 0;
            }

            uint8 index = AccelerometerLLI_Public.Last_Written_Index;                       // Local variable to increase readability

            AccelerometerLLI_Public.Raw[index].X_Axis = AccelerometerLLI.Point.X_Axis;
            AccelerometerLLI_Public.Raw[index].Y_Axis = AccelerometerLLI.Point.Y_Axis;
            AccelerometerLLI_Public.Raw[index].Z_Axis = AccelerometerLLI.Point.Z_Axis;
            AccelerometerLLI_Public.Raw[index].Timestamp = AccelerometerLLI.Timestamp;       // Set the timestamp of this new sample
            AccelerometerLLI.Point.Timestamp = AccelerometerLLI.Timestamp;
        }
    }
    AccelerometerLLI.Timestamp += FAST_HANDLER_TIMING_USEC;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void SetDeviceInfo(void)
{
    uint8 device_id = AccelDiscover__GetId();

    if (device_id == 0x33)
    {
        AccelerometerLLI_Public.Device = ACCELEROMETER_DEVICE_LISxDH;
        AccelerometerLLI_Public.Id = device_id;
        LISxDH__Initialize(AccelerometerLLI_SF_Param);
    }
    else if (device_id == 0x86)
    {
        AccelerometerLLI_Public.Device = ACCELEROMETER_DEVICE_FXLS8974;
        AccelerometerLLI_Public.Id = device_id;
        FXLS8974__Initialize((FXLS8974_SF_PARAM_TYPE *)AccelerometerLLI_SF_Param);
    }
    else
    {
        // TODO: What to do if the device is not recognized?
    }

    SetOutputDataRate(AccelerometerLLI_SF_Param->Data_Rate);
    // For FVT, need to force the Accelerometer to be enabled; it can be disabled as needed later
    AccelerometerLLI__Enable();
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void WaitForDiscovery(void)
{
    AccelDiscover__Execute();

    if (AccelDiscover__IsComplete() == TRUE)
    {
        EventDeviceIdentified();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void InitDiscovery(void)
{
    AccelDiscover__Initialize(AccelerometerLLI_SF_Param->Port, AccelerometerLLI_SF_Param->I2C_Address);
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Accelerometer LLI State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Accelerometer LLI state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void AccelerometerLLIExecute(void)
{
    switch(Accelerometer_LLI_State)
    {
        case STATE_ACCELEROMETER_LLI_DISCOVER:
            WaitForDiscovery();                     // Do function.
            break;

        case STATE_ACCELEROMETER_LLI_RUN:
            FastProcess();                          // Do function.
            break;

        default:
            // There is nothing to do for all other values of Accelerometer_LLI_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Accelerometer LLI state machine.
 * This function resets the state machine to the initial state and executes the initial state's
 * entry function.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void AccelerometerLLIInitialize(void)
{
    Accelerometer_LLI_State = STATE_ACCELEROMETER_LLI_DISCOVER;
    InitDiscovery();                                // Entry Function.
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Device Identified Event for the Accelerometer LLI state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDeviceIdentified(void)
{
    switch (Accelerometer_LLI_State)
    {
        case STATE_ACCELEROMETER_LLI_DISCOVER:
            SetDeviceInfo();                        // Transition action function.
            Accelerometer_LLI_State = STATE_ACCELEROMETER_LLI_RUN;
            break;

        default:
            // Event is ignored for all other values of Accelerometer_LLI_State.
            break;
    }
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

#endif //(HBL_LLI_NUM_ACCELEROMETER > 0)

/**
 *  @file       TriacVoltageMonitor.c
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *
 *
 *                       ^
 *                       | \
 *              Ymax --- |   o
 *                    |  |     \
 *                    |  |       \
 *                    |  |         \
 *              Yact --- |           o
 *                    |  |             \
 *                    |  |               \
 *                    |  |                 \
 *                    |  |                   \
 *                    |  |                     \
 *              Ymin --- |                       o
 *                       |                         \
 *                       ----------------------------->
 *                           |-------|
 *                           |-------------------|
 *                          Xmin    Xact        Xmax
 *
 *
 *                                             Xact - Xmin
 *              Yact = Ymax - (Ymax - Ymin) * -------------
 *                                             Xmax - Xmin
 *
 *  @section    Applicable_Documents
 *                    List here all the applicable documents if needed. <tr>
 * *
 *  @startuml {TriacVoltageMonitor.png}

            title Triac Voltage Monitor State Machine


            State Idle
            State Request
            State Evaluation
            State Verification

            [*] --> Idle

            Idle --> Request : evPerformRequests

            Request : do / VerifyRequests()
            Request --> Evaluation : evPerformEvaluation

            Evaluation : do / VerifyLoads()
            Evaluation --> ChangeCheck : evPerformChangeCheck

            ChangeCheck : do / VerifyIfRequestsChanged()
            ChangeCheck --> Request : evPerformRequests
            ChangeCheck --> Verification : evPerformVerification

            Verification : do / CompareLoadsAndRequests()
            Verification --> Request : evPerformRequests

    @enduml
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2017-2021. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

#include "SystemConfig.h"
#include "TriacVoltageMonitor.h"
#include "DishAuxVar.h"
#include "Micro.h"
#if (TRIAC_V_DIAG_AC_LINE_FEATURE == DISABLED)
#include "Mci.h"
#endif
#include "SettingFile.h"
#include "SystemTimers.h"
#include "Hbl.h"


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Triac Voltage Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Triac Voltage Monitor state machine.
typedef enum TRIAC_VOLTAGE_MONITOR_STATE_ENUM
{
    STATE_TRIAC_VOLTAGE_MONITOR_NONE         = 0,

    STATE_TRIAC_VOLTAGE_MONITOR_IDLE         = 1,   //!< (initial_state)
    STATE_TRIAC_VOLTAGE_MONITOR_REQUEST      = 2,
    STATE_TRIAC_VOLTAGE_MONITOR_EVALUATION   = 3,
    STATE_TRIAC_VOLTAGE_MONITOR_VERIFICATION = 4,
    STATE_TRIAC_VOLTAGE_MONITOR_CHANGE_CHECK = 5,

    STATE_TRIAC_VOLTAGE_MONITOR_END          = 6
} TRIAC_VOLTAGE_MONITOR_STATE_TYPE;

//! The state variable for the Triac Voltage Monitor state machine.
//! State machines with a history pseudostate may write to this variable during initialization.
//! Other state machines must only write to this variable through auto-generated functions.
static TRIAC_VOLTAGE_MONITOR_STATE_TYPE Triac_Voltage_Monitor_State;

// -- Private Function Prototypes used by the state machine for customization functions --
static void CompareLoadsAndRequests(void);
static void VerifyIfRequestsChanged(void);
static void VerifyLoads(void);
static void VerifyRequests(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventPerformChangeCheck(void);
static void EventPerformEvaluation(void);
static void EventPerformRequests(void);
static void EventPerformVerification(void);
static void TriacVoltageMonitorExecute(void);
static void TriacVoltageMonitorInitialize(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
//! TRUE when a power loss condition has been detected through HVDC Bus signal
BOOL_TYPE Power_Loss_Occurring;

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//! The number of loads that that can be driven at the same time
#define MAX_LOADS_SIMULTANEOUS             (4)

//! Time in milliseconds
#define TRIACDIAGNOSTIC_STABILIZATION_TIME  (2000)
#define TRIACDIAGNOSTIC_POWER_LOSS_TIME     (3000)
//! Samples in the AD to analyze the input for the triac diagnostics
#define TRIACDIAGNOSTICS_SAMPLE_NUMBER      (3)

#if (TRIAC_V_DIAG_AC_LINE_FEATURE == DISABLED)
//! Samples of AC voltage from MCI
#define TRIACDIAGNOSTICS_SUPPLYVOLTAGE_NUMBER      (25)
#endif

//! Macro to read the ad of the triac voltage diagnostic
#define GET_TRIACDIAGNOSTIC_READ()     (*(uint16*)Hbl__GetGIDataByIndex(Hbl__GetGIIndexByGIID(HBL_GI_V_DIAGNOSTIC)));
//! Maximum number of loads in the list to be verified
#define MAXIMUM_NUMBER_OF_LOADS       (6U)

//! Enum with the Voltage range
typedef enum
{
    VOLTAGESENSORGI_110_VOLTS_RANGE = 0,
    VOLTAGESENSORGI_220_VOLTS_RANGE,
    VOLTAGESENSORGI_RANGES_NUMBER,
    VOLTAGESENSORGI_NO_VOLTAGE_DETECTED = 0xFF
} VOLTAGESENSORGI_VOLTAGES_TYPE;


//! Values of the edges of the expected feedback for each
typedef struct
{
    uint16 Feedback_Min;
    uint16 Feedback_Max;
    uint16 Delta;
} TRIACDIAGNOSTIC_VBUS_LEVEL_TYPE;

//! Hold the TriacDiagnostic parameters
typedef struct
{
    uint8 Simultaneous_Amount_Of_Loads;
    uint8 Number_Of_Loads_To_Verify;
    HBL_LOAD_ENUM_TYPE *TriacDiagnostic_Loads;
    TRIACDIAGNOSTIC_VBUS_LEVEL_TYPE *TriacDiagnostic_Atod_Levels;
}TRIACDIAGNOSTIC_PARAM_TYPE;

//! Hold the TriacDiagnostic data
typedef struct
{
    uint32 Ad_Accumulator_Loads;
    uint16 Ad_Values_Loads;
    uint16 Triac_Diagnostic_Timer;
    uint16 Triac_Stabilization_Timer;
    uint16 Estimated_Feedback[MAX_LOADS_SIMULTANEOUS+1];
    uint8 Outputs_On_Last_Counter;
    uint8 Outputs_On_Actual_Counter;
    uint8 Outputs_On_Verified;
    uint8 Ad_Samples_Loads;
} TRIACDIAGNOSTIC_DATA_TYPE;

#if (TRIAC_V_DIAG_AC_LINE_FEATURE == DISABLED)
//! Hold the TriacVoltage sampling data
typedef struct
{
    uint16 Dc_Voltage_Current_Max;
    uint16 Dc_Voltage_Sampled;
    uint8 Dc_Voltage_Samples_Cnt;
}TRIACVOLTAGE_SAMPLING_DATA_TYPE;

//! Setting file HVDC Bus Parameters struct
typedef PACKED struct HVDC_BUS_PARAMS_STRUCT
{
	uint16 HMI_Off_HVDC_Bus_Threshold; //! HVDC Bus threshold below which HMI is informed that a potential power loss is occurring.
} HVDC_BUS_PARAMS_TYPE;
#endif

//! Holds the value the voltage currently used by the diagnostic algorithm
static uint16 TriacDiagnostic_Voltage_Value;

//! Holds the range of the voltage
static VOLTAGESENSORGI_VOLTAGES_TYPE TriacDiagnostic_Voltage_Range;

//! Declare the parameters of the TriacDiagnostic
static TRIACDIAGNOSTIC_DATA_TYPE TriacDiagnostic_Data;

#if (TRIAC_V_DIAG_AC_LINE_FEATURE == DISABLED)
static TRIACVOLTAGE_SAMPLING_DATA_TYPE TriacVoltage_Data;

//! HVDC Bus threshold below which HMI is informed that a potential power loss is occurring.
static uint16 HVDC_Threshold;
#endif

//! Declare the parameters of the TriacDiagnostic
static TRIACDIAGNOSTIC_PARAM_TYPE TriacDiagnostic_Param;

//! Declare the parameters of the TriacDiagnostic
static uint8 TriacDiagnostic_Loads_Status[MAXIMUM_NUMBER_OF_LOADS];

static TRIAC_LOADS_VOLTAGE_SENSOR_TYPE TriacVoltage_Status;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static BOOL_TYPE OutputsOnVerificationEnded(void);
static uint8 NumberOfOutputsOn(void);
static void EstimateRightFeedbackLevel(void);
static BOOL_TYPE RequestsChanged(void);
static uint8 CountOutputsOn(void);
static BOOL_TYPE IsLoadOn(uint8 load_index);
static void TriacDiagnosticStart(void);
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//----------------------------------------------------------------------------
/**
 *  @brief      TriacVoltageMonitor__Initialize
 *  @details    Function used to initialize the Triac Voltage Monitor.
 *
 */
void TriacVoltageMonitor__Initialize(void)
{
    uint8 offset;
    // Load the setting file data into a pointer for use at various times during the algorithm
    SETTINGFILE_LOADER_TYPE sf_loader;

    TriacVoltageMonitorInitialize();

    TriacDiagnostic_Data.Outputs_On_Last_Counter = 0;
    TriacDiagnostic_Data.Outputs_On_Actual_Counter = 0;
    TriacDiagnostic_Voltage_Range = VOLTAGESENSORGI_NO_VOLTAGE_DETECTED;

    TriacDiagnostic_Data.Ad_Accumulator_Loads = 0;
    TriacDiagnostic_Data.Ad_Values_Loads = 0;
    TriacDiagnostic_Data.Ad_Samples_Loads = 0;

    // Get a System timer for timeout
    TriacDiagnostic_Data.Triac_Diagnostic_Timer = SystemTimers__GetHandle(SYSTEMTIMERS_TYPE_DOWN_COUNTER);
    TriacDiagnostic_Data.Triac_Stabilization_Timer = SystemTimers__GetHandle(SYSTEMTIMERS_TYPE_DOWN_COUNTER);
    // Set the timeout timer
    (void)SystemTimers__SetMs(TriacDiagnostic_Data.Triac_Diagnostic_Timer, 1000);
    (void)SystemTimers__SetMs(TriacDiagnostic_Data.Triac_Stabilization_Timer, 1000);

    TriacDiagnostic_Param.Simultaneous_Amount_Of_Loads = 0;
    TriacDiagnostic_Param.Number_Of_Loads_To_Verify = 0;
    TriacDiagnostic_Param.TriacDiagnostic_Loads = NULL;
    TriacDiagnostic_Param.TriacDiagnostic_Atod_Levels = NULL;

    TriacVoltage_Status = TRIAC_LOADS_VOLTAGE_SENSOR_UNKNOWN;

    TriacDiagnostic_Voltage_Value = 0;

#if (TRIAC_V_DIAG_AC_LINE_FEATURE == DISABLED)
    TriacVoltage_Data.Dc_Voltage_Sampled = 0;
    TriacVoltage_Data.Dc_Voltage_Current_Max = 0;
    TriacVoltage_Data.Dc_Voltage_Samples_Cnt = 0;
#endif // (TRIAC_V_DIAG_AC_LINE_FEATURE == DISABLED)

    if (SettingFile__BasicLoader(SF_PTR_ACU_PRODUCT_CONFIGURATION_DATA, DSPL_DIAG_VOLTAGE_PARAMS, &sf_loader) == PASS)
    {
        // Will verify if at least one load should be verified since the first byte is the type of protection
        if (sf_loader.Length > (uint16)1)
        {
            // Get the the number of loads being drived simultaneously and the number of loads that are
            //being monitored in the output
            TriacDiagnostic_Param.Simultaneous_Amount_Of_Loads = sf_loader.Data[0];
            TriacDiagnostic_Param.Number_Of_Loads_To_Verify = sf_loader.Data[1];

            // Get the address of the loads list.
            TriacDiagnostic_Param.TriacDiagnostic_Loads = (HBL_LOAD_ENUM_TYPE *) ((void *) &sf_loader.Data[2]);

            offset = (uint8)3 + TriacDiagnostic_Param.Number_Of_Loads_To_Verify - ((TriacDiagnostic_Param.Number_Of_Loads_To_Verify - (uint8)1) % (uint8)2);
            TriacDiagnostic_Param.TriacDiagnostic_Atod_Levels = (TRIACDIAGNOSTIC_VBUS_LEVEL_TYPE *) ((void *) &sf_loader.Data[offset]);
        }
    }

    // Start the module
    TriacDiagnosticStart();
}

//----------------------------------------------------------------------------
/**
 *  @brief      TriacVoltageMonitor__Execution
 *  @details    Function used to perform the Triac Voltage Monitor.
 *
 */
void TriacVoltageMonitor__Execution(void)
{
    if (AC_LINE_MEASUREMENT_PRESENT)
    {
        TriacDiagnostic_Voltage_Value = GET_ACTUAL_VOLTAGE;

        if ((TriacDiagnostic_Voltage_Value > LOW_EDGE_110V_RANGE) && (TriacDiagnostic_Voltage_Value < HIGH_EDGE_110V_RANGE))
        {
            TriacDiagnostic_Voltage_Range = VOLTAGESENSORGI_110_VOLTS_RANGE;
        }
        else if ((TriacDiagnostic_Voltage_Value > LOW_EDGE_220V_RANGE) && (TriacDiagnostic_Voltage_Value < HIGH_EDGE_220V_RANGE))
        {
            TriacDiagnostic_Voltage_Range = VOLTAGESENSORGI_220_VOLTS_RANGE;
        }
        else
        {
            TriacDiagnostic_Voltage_Range = VOLTAGESENSORGI_NO_VOLTAGE_DETECTED;
        }

        // Execute the TriacDiagnostic handler only if the number of load to verify is equal or higher than 1
        if (   (TriacDiagnostic_Param.Simultaneous_Amount_Of_Loads != 0U)
            && (TriacDiagnostic_Param.Number_Of_Loads_To_Verify != 0U)
            && (HBL_INDEX_INVALID != Hbl__GetGIIndexByGIID(HBL_GI_V_DIAGNOSTIC)))
        {
            TriacVoltageMonitorExecute();
        }
    }
}

#if (TRIAC_V_DIAG_AC_LINE_FEATURE != ENABLED)
void TriacVoltageMonitor__FastHandler1ms(void)
{
    uint16 dc_voltage_sample;

    dc_voltage_sample = ((uint16)(Mci__GetAnalog(MOTOR0, MCI_AI_BUS_VOLTAGE_S16)>>16));

	if (0 == TriacVoltage_Data.Dc_Voltage_Samples_Cnt)
    {
        TriacVoltage_Data.Dc_Voltage_Current_Max = dc_voltage_sample;
    }
    else
    {
        if (dc_voltage_sample > TriacVoltage_Data.Dc_Voltage_Current_Max)
        {
            TriacVoltage_Data.Dc_Voltage_Current_Max = dc_voltage_sample;
        }
    }
    TriacVoltage_Data.Dc_Voltage_Samples_Cnt++;

    if(TriacVoltage_Data.Dc_Voltage_Samples_Cnt >= TRIACDIAGNOSTICS_SUPPLYVOLTAGE_NUMBER)
    {
    	TriacVoltage_Data.Dc_Voltage_Sampled = TriacVoltage_Data.Dc_Voltage_Current_Max;
    	TriacVoltage_Data.Dc_Voltage_Samples_Cnt = 0;
    }

	if (TriacVoltage_Data.Dc_Voltage_Sampled < HVDC_Threshold)
	{
		Power_Loss_Occurring = TRUE;
		//Set the stabilization timer
        (void)SystemTimers__SetMs(TriacDiagnostic_Data.Triac_Stabilization_Timer, TRIACDIAGNOSTIC_POWER_LOSS_TIME);
	}
	else
	{
		Power_Loss_Occurring = FALSE;
    }
}
#endif // (TRIAC_V_DIAG_AC_LINE_FEATURE == DISABLED)

//----------------------------------------------------------------------------
/**
 *  @brief      TriacVoltageMonitor__InitISRVariables
 *  @details    Function used to initialize variables that may be used in SRMain__TimerISR
 *
 */
void TriacVoltageMonitor__InitISRVariables(void)
{
    if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
	{
#if (TRIAC_V_DIAG_AC_LINE_FEATURE == DISABLED)
		SETTINGFILE_LOADER_TYPE loader;

		if (SettingFile__BasicLoader(SF_PTR_ACU_PRODUCT_CONFIGURATION_DATA, DSPL_HVDC_BUS_PARAMS, &loader) == PASS)
		{
			HVDC_Threshold = ENDIANU16_STON(((HVDC_BUS_PARAMS_TYPE *)(void*)loader.Data)->HMI_Off_HVDC_Bus_Threshold);
		}
		else
		{
			HVDC_Threshold = 0;
		}
#endif

		Power_Loss_Occurring = FALSE;
	}
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      TriacDiagnostic start
 *
 *  @detail     The module will start after the initialization
 */
static void TriacDiagnosticStart(void)
{
    EventPerformRequests();
}

//----------------------------------------------------------------------------
/**
 *  @brief      TriacVoltageMonitor__GetLoad_Status
 *  @details    Function used to return voltage sensor status.
 *
 */
TRIAC_LOADS_VOLTAGE_SENSOR_TYPE TriacVoltageMonitor__GetLoad_Status(void)
{
    TRIAC_LOADS_VOLTAGE_SENSOR_TYPE ret_val;
    if (Power_Loss_Occurring == TRUE)
    {
        TriacVoltage_Status =TRIAC_LOADS_VOLTAGE_SENSOR_UNKNOWN;
    }
    ret_val = TriacVoltage_Status;
    return ret_val;
}

//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This function perform the samples read and return TRUE when enough samples were acquired
 *  @return     BOOL_TYPE: TRUE  = The number of samples already reached the number that should be performed
 *                         FALSE = The routine did not enough samples yet
 */
static BOOL_TYPE OutputsOnVerificationEnded(void)
 {
    BOOL_TYPE verification_end;

    verification_end = FALSE;

    // Perform the sample and increment the number of samples
    TriacDiagnostic_Data.Ad_Accumulator_Loads += GET_TRIACDIAGNOSTIC_READ();
    TriacDiagnostic_Data.Ad_Samples_Loads++;

    if(TriacDiagnostic_Data.Ad_Samples_Loads >= (uint8)TRIACDIAGNOSTICS_SAMPLE_NUMBER)
    {
        // If the number of samples is higher than the TRIACDIAGNOSTICS_SAMPLE_NUMBER then the average is calculated
        TriacDiagnostic_Data.Ad_Values_Loads = (uint16) (TriacDiagnostic_Data.Ad_Accumulator_Loads / (uint16)TRIACDIAGNOSTICS_SAMPLE_NUMBER);
        TriacDiagnostic_Data.Ad_Accumulator_Loads = 0;
        TriacDiagnostic_Data.Ad_Samples_Loads = 0;

        // Then the feedback is calculated
        EstimateRightFeedbackLevel();

        verification_end = TRUE;
    }

    return (verification_end);
}





//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This function shall count the number of loads that are on
 *  @return     uint8: The number of loads that are on
 */
static uint8 NumberOfOutputsOn(void)
{
    uint8 loads_tested;
    uint8 measured_outputs;
    uint16 table_min_value;
    uint16 table_max_value;
    uint16 table_min_initial_value;
    uint16 table_max_initial_value;
    TRIACDIAGNOSTIC_VBUS_LEVEL_TYPE *atod_levels;

    measured_outputs = UCHAR_MAX;

    if (TriacDiagnostic_Voltage_Range != VOLTAGESENSORGI_NO_VOLTAGE_DETECTED)
    {
        atod_levels = TriacDiagnostic_Param.TriacDiagnostic_Atod_Levels;

        // Get the range of the number of loads that should be drived
        loads_tested = TriacDiagnostic_Data.Outputs_On_Actual_Counter;
        table_min_initial_value = TriacDiagnostic_Data.Estimated_Feedback[loads_tested] - atod_levels[loads_tested].Delta;
        table_max_initial_value = TriacDiagnostic_Data.Estimated_Feedback[loads_tested] + atod_levels[loads_tested].Delta;

        // Verify if the atod reading is within the range of the number of loads that should be drived
        if ( (TriacDiagnostic_Data.Ad_Values_Loads <= table_max_initial_value) &&
             (TriacDiagnostic_Data.Ad_Values_Loads >= table_min_initial_value) )
        {
            // If the measurement is between the this range then the number of loads is the same that the one in the index
            measured_outputs = loads_tested;
        }
        else
        {
            // If the the range and the number of loads being drived don't match then it will be performed the verification for each
            //quantity of loads being drived at the same time as possible
            for (loads_tested = 0; loads_tested <= (TriacDiagnostic_Param.Simultaneous_Amount_Of_Loads + (uint8)1); loads_tested++)
            {
                // Will just verify if the voltage is detected and if the output have voltage
                if (TriacDiagnostic_Voltage_Range != VOLTAGESENSORGI_NO_VOLTAGE_DETECTED)
                {
                    // Verify the expected values for the specified number of loads
                    if(loads_tested <= TriacDiagnostic_Param.Simultaneous_Amount_Of_Loads)
                    {
                        // Get the ad value from the TriacDiagnostic levels table
                        table_min_value = TriacDiagnostic_Data.Estimated_Feedback[loads_tested] - atod_levels[loads_tested].Delta;
                        table_max_value = TriacDiagnostic_Data.Estimated_Feedback[loads_tested] + atod_levels[loads_tested].Delta;

                        if ( (TriacDiagnostic_Data.Ad_Values_Loads <= table_max_value) &&
                             (TriacDiagnostic_Data.Ad_Values_Loads >= table_min_value) )
                        {
                            // If the measurement is between the this range then the number of loads is the same that the one in
                            //the index
                            measured_outputs = loads_tested;
                            break;
                        }
                    }
                    else
                    {
                        // Verify if the value is higher than the one expected in the tables
                        table_min_value = TriacDiagnostic_Data.Estimated_Feedback[TriacDiagnostic_Param.Simultaneous_Amount_Of_Loads]
                                          + atod_levels[TriacDiagnostic_Param.Simultaneous_Amount_Of_Loads].Delta;

                        if (TriacDiagnostic_Data.Ad_Values_Loads >= table_min_value)
                        {
                            // This is the number of the loads expected when the output is being drived and
                            // number of Loads Observed is smaller than number if Loads Simultaneously On
                            measured_outputs = loads_tested;
                        }
                    }
                }
            }
        }
    }

    return (measured_outputs);
}





//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This function shall estimate the right feedback value for each number of loads
 *
 */
static void EstimateRightFeedbackLevel(void)
{
    uint8 index;
    uint32 delta_y;
    uint16 delta_x1;
    uint16 delta_x2;
    uint16 actual_voltage;
    uint16 voltage_min;
    uint16 voltage_max;
    TRIACDIAGNOSTIC_VBUS_LEVEL_TYPE *atod_levels;

    // Just perform the calculation if the voltage is in a known voltage
    if (TriacDiagnostic_Voltage_Range != VOLTAGESENSORGI_NO_VOLTAGE_DETECTED)
    {
        if (TriacDiagnostic_Voltage_Range == VOLTAGESENSORGI_110_VOLTS_RANGE )
        {
            // Get the parameters from the 110V range
            voltage_min = LOW_EDGE_110V_RANGE;
            voltage_max = HIGH_EDGE_110V_RANGE;
            atod_levels = TriacDiagnostic_Param.TriacDiagnostic_Atod_Levels;
        }
        else //if (TriacDiagnostic_Voltage_Range == VOLTAGESENSORGI_220_VOLTS_RANGE)
        {
            // Get the parameters from the 220V range
            voltage_min = LOW_EDGE_220V_RANGE;
            voltage_max = HIGH_EDGE_220V_RANGE;
            atod_levels = TriacDiagnostic_Param.TriacDiagnostic_Atod_Levels;
        }

        // Get the voltage to calculate the level of feedback
        actual_voltage = TriacDiagnostic_Voltage_Value;

        delta_x1 = actual_voltage - voltage_min;
        delta_x2 = voltage_max - voltage_min;

        // Calculate for every quantity of loads being drived
        for (index = 0; index <= TriacDiagnostic_Param.Simultaneous_Amount_Of_Loads; index++)
        {
            // Get the ad value from the outputs levels table
            // Perform the following formula Yact = Ymax - (Ymax - Ymin) * ((Xact - Xmin)/(Xmax - Xmin))
            delta_y = (((uint32)atod_levels[index].Feedback_Max - (uint32)atod_levels[index].Feedback_Min) * delta_x1);
            delta_y = delta_y/delta_x2;
            TriacDiagnostic_Data.Estimated_Feedback[index] = (uint16)(atod_levels[index].Feedback_Max - delta_y);
        }
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Verify if the load request has changed after the last verification
 *  @return     BOOL_TYPE: TRUE  = The loads requests has changed
 *                         FALSE = The loads requests has not changed
 */
static BOOL_TYPE RequestsChanged(void)
{
    BOOL_TYPE requests_changed_status;

    requests_changed_status = FALSE;

    // Verify the number of outputs that should be on
    TriacDiagnostic_Data.Outputs_On_Actual_Counter = CountOutputsOn();

    if(TriacDiagnostic_Data.Outputs_On_Last_Counter != TriacDiagnostic_Data.Outputs_On_Actual_Counter)
    {
        TriacVoltage_Status = TRIAC_LOADS_VOLTAGE_SENSOR_UNKNOWN;

        // If the number of outputs changed then it will be informed
        TriacDiagnostic_Data.Outputs_On_Last_Counter = TriacDiagnostic_Data.Outputs_On_Actual_Counter;
        requests_changed_status = TRUE;
    }

    return (requests_changed_status);
}


/**
 *  @brief      This function shall count the number of loads that should be on
 *  @return     uint8: The number of loads that should be on
 */
static uint8 CountOutputsOn(void)
{
    BOOL_TYPE outputs_on_buffer[MAXIMUM_NUMBER_OF_LOADS];
    uint8 index;
    uint8 outputs_on;

    outputs_on = 0;

    // Initialize a buffer with the number of loads
    for(index = 0; index < MAXIMUM_NUMBER_OF_LOADS; index++)
    {
        outputs_on_buffer[index] = FALSE;
    }

    // Verify each possible status that can lead to the load to be on and set the load position in the buffer
    for(index = 0; index < TriacDiagnostic_Param.Number_Of_Loads_To_Verify; index++)
    {
        if((uint8)TRUE == (uint8)IsLoadOn(index))
        {
            outputs_on_buffer[index] = TRUE;
        }
    }

    // Count the number of loads that should be on
    for(index = 0; index < TriacDiagnostic_Param.Number_Of_Loads_To_Verify; index++)
    {
        if((uint8)TRUE == (uint8)outputs_on_buffer[index])
        {
            outputs_on++;
        }
    }

    return (outputs_on);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This function should verify if the status of a specific load is the one that is expected to be on
 *  @return     BOOL_TYPE: TRUE  = The load pointed by this index in the register have the output ON
 *                         FALSE = The load pointed by this index in the register have the output OFF
 */
static BOOL_TYPE IsLoadOn(uint8 load_index)
{
    BOOL_TYPE load_is_on;
    Diverter_Position_Type Current_Sense_Diverter_Position;

    // Get the status of the load pointed by this index
    if(TriacDiagnostic_Param.TriacDiagnostic_Loads[load_index] != HBL_LOAD_INVALID)
    {
        if (HBL_LOAD_DIVERTER == TriacDiagnostic_Param.TriacDiagnostic_Loads[load_index])
        {
            Current_Sense_Diverter_Position = DishAuxVar__GetDiverterPosition();
            if ((uint8)Current_Sense_Diverter_Position.Is_Stable == (uint8)TRUE)
            {
                TriacDiagnostic_Loads_Status[load_index] = OFF;
            }
            else
            {
                TriacDiagnostic_Loads_Status[load_index] = ON;
            }
        }
        else
        {
            uint8 *load_state_ptr;

            load_state_ptr = (uint8*)Hbl__GetLoadStatusByLoadID(TriacDiagnostic_Param.TriacDiagnostic_Loads[load_index]);
            if (load_state_ptr != NULL)
            {
            	TriacDiagnostic_Loads_Status[load_index] = *load_state_ptr;
            }
            else
            {
                TriacDiagnostic_Loads_Status[load_index] = OFF;
            }
        }
    }
    else
    {
        TriacDiagnostic_Loads_Status[load_index] = OFF;
    }

    // Verify if the status of this load is the one that is expected
    if(TriacDiagnostic_Loads_Status[load_index] == (uint8)OFF)
    {
        load_is_on = FALSE;
    }
    else
    {
        load_is_on = TRUE;
    }

    return (load_is_on);
}

//---------------------------------------------------------------------------------------------------------------------
/*
 */
static void CompareLoadsAndRequests(void)
{
#if (TRIAC_V_DIAG_AC_LINE_FEATURE == DISABLED)
    uint8 salt_input_activation_load_index;
    salt_input_activation_load_index = Hbl__GetLoadIndexByLoadID(HBL_LOAD_SALT_INPUT_ACTIVATION);

    // Salt input activation load absent or present and activated: supply voltage can be read and diagnostic is reliable
    if ((salt_input_activation_load_index == HBL_INDEX_INVALID) || (*((uint8 *)Hbl__GetProcessedLoad(salt_input_activation_load_index)) != OFF))
#endif

    {
        // Salt input activation load absent or present and activated: supply voltage can be read and diagnostic is reliable
        TriacDiagnostic_Data.Outputs_On_Verified = NumberOfOutputsOn();
        if ((TriacDiagnostic_Data.Outputs_On_Actual_Counter == TriacDiagnostic_Data.Outputs_On_Verified)
            && (SYSTEMTIMERS_STATE_DONE == SystemTimers__GetState(TriacDiagnostic_Data.Triac_Stabilization_Timer)))
        {
            TriacVoltage_Status = TRIAC_LOADS_VOLTAGE_SENSOR_CONFIRMED;
        }
        else
        {
            if ((TriacDiagnostic_Data.Outputs_On_Verified != UCHAR_MAX)
                && (SYSTEMTIMERS_STATE_DONE == SystemTimers__GetState(TriacDiagnostic_Data.Triac_Stabilization_Timer)))
            {
                TriacVoltage_Status = TRIAC_LOADS_VOLTAGE_SENSOR_DECLINED;
            }
            else
            {
                TriacVoltage_Status = TRIAC_LOADS_VOLTAGE_SENSOR_UNKNOWN;
            }
        }
    }
#if (TRIAC_V_DIAG_AC_LINE_FEATURE == DISABLED)
    else
    {
        TriacVoltage_Status = TRIAC_LOADS_VOLTAGE_SENSOR_UNKNOWN;
    }
#endif

    // Go to the Request state
    EventPerformRequests();
}
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Verify the number of outputs that are on
 */
static void VerifyLoads(void)
{
    // If the verification of the loads has finished then it can go to the state where it will be compared with the
    //number of loads that have the request to be on
    if((uint8)TRUE == (uint8)OutputsOnVerificationEnded())
    {
        EventPerformChangeCheck();
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void VerifyRequests(void)
{
    // Verify if the number of outputs that should be on has changed since the last verification
    if((uint8)TRUE == (uint8)RequestsChanged())
    {
        //Set the timeout timer
        (void)SystemTimers__SetMs(TriacDiagnostic_Data.Triac_Diagnostic_Timer, TRIACDIAGNOSTIC_STABILIZATION_TIME);
    }

    // Guarantee that the number of outputs load didn't change in the last TRIACDIAGNOSTIC_STABILIZATION_TIME seconds
    if(SYSTEMTIMERS_STATE_DONE == SystemTimers__GetState(TriacDiagnostic_Data.Triac_Diagnostic_Timer))
    {
        // Change the state to Evaluation
        EventPerformEvaluation();
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void VerifyIfRequestsChanged(void)
{
    // If the number of requests to turn on the loads change then it will return to the Request state
    if((uint8)TRUE == (uint8)RequestsChanged())
    {
        EventPerformRequests();

        //Set the timeout timer
        (void)SystemTimers__SetMs(TriacDiagnostic_Data.Triac_Diagnostic_Timer, TRIACDIAGNOSTIC_STABILIZATION_TIME);
    }
    else
    {
        // If the number of request didn't changed then it will go to the state Verification
        EventPerformVerification();
    }
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Triac Voltage Monitor State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Perform Change Check Event for the Triac Voltage Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventPerformChangeCheck(void)
{
    switch (Triac_Voltage_Monitor_State)
    {
        case STATE_TRIAC_VOLTAGE_MONITOR_EVALUATION:
            Triac_Voltage_Monitor_State = STATE_TRIAC_VOLTAGE_MONITOR_CHANGE_CHECK;
            break;

        default:
            // Event is ignored for all other values of Triac_Voltage_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Perform Evaluation Event for the Triac Voltage Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventPerformEvaluation(void)
{
    switch (Triac_Voltage_Monitor_State)
    {
        case STATE_TRIAC_VOLTAGE_MONITOR_REQUEST:
            Triac_Voltage_Monitor_State = STATE_TRIAC_VOLTAGE_MONITOR_EVALUATION;
            break;

        default:
            // Event is ignored for all other values of Triac_Voltage_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Perform Requests Event for the Triac Voltage Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventPerformRequests(void)
{
    switch (Triac_Voltage_Monitor_State)
    {
        case STATE_TRIAC_VOLTAGE_MONITOR_IDLE:
        case STATE_TRIAC_VOLTAGE_MONITOR_VERIFICATION:
        case STATE_TRIAC_VOLTAGE_MONITOR_CHANGE_CHECK:
            Triac_Voltage_Monitor_State = STATE_TRIAC_VOLTAGE_MONITOR_REQUEST;
            break;

        default:
            // Event is ignored for all other values of Triac_Voltage_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Perform Verification Event for the Triac Voltage Monitor state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventPerformVerification(void)
{
    switch (Triac_Voltage_Monitor_State)
    {
        case STATE_TRIAC_VOLTAGE_MONITOR_CHANGE_CHECK:
            Triac_Voltage_Monitor_State = STATE_TRIAC_VOLTAGE_MONITOR_VERIFICATION;
            break;

        default:
            // Event is ignored for all other values of Triac_Voltage_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Triac Voltage Monitor state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void TriacVoltageMonitorExecute(void)
{
    switch(Triac_Voltage_Monitor_State)
    {
        case STATE_TRIAC_VOLTAGE_MONITOR_REQUEST:
            VerifyRequests();                       // Do function.
            break;

        case STATE_TRIAC_VOLTAGE_MONITOR_EVALUATION:
            VerifyLoads();                          // Do function.
            break;

        case STATE_TRIAC_VOLTAGE_MONITOR_VERIFICATION:
            CompareLoadsAndRequests();              // Do function.
            break;

        case STATE_TRIAC_VOLTAGE_MONITOR_CHANGE_CHECK:
            VerifyIfRequestsChanged();              // Do function.
            break;

        default:
            // There is nothing to do for all other values of Triac_Voltage_Monitor_State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Triac Voltage Monitor state machine.
 * This function resets the state machine to the initial state.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void TriacVoltageMonitorInitialize(void)
{
    Triac_Voltage_Monitor_State = STATE_TRIAC_VOLTAGE_MONITOR_IDLE;
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

/**
 *  @file		OwiVroRead.c
 *
 *  @brief		A process that implements the OWI Voltage Regulated Output
 *
 *  @details
 *
 *
   @startuml{OWI VRO State Machine.jpg}
        title OWI VRO State Machine
        ' Use state = OwiVroRead_Data.State

		[*] --> INITIALIZE

		INITIALIZE --> WAIT_FOR_INITIAL_DELAY : evStartOwiVro
        INITIALIZE --> OWI_VRO_COMPLETE : evSkipOwiVro

		WAIT_FOR_INITIAL_DELAY : entry / StartDelay()
		WAIT_FOR_INITIAL_DELAY : do / WaitForInitialDelay()
		WAIT_FOR_INITIAL_DELAY : exit / InitSamplesAcquisition()
		WAIT_FOR_INITIAL_DELAY --> WAIT_FOR_NEXT_SAMPLE : evInitialDelayExpired

		WAIT_FOR_NEXT_SAMPLE : entry / StartNextSampleAcquisition()
		WAIT_FOR_NEXT_SAMPLE : do / CheckForNewSample()
		WAIT_FOR_NEXT_SAMPLE --> WAIT_FOR_DELAY_BTW_SAMPLES : evNewSampleAcquired
		WAIT_FOR_NEXT_SAMPLE --> SAMPLES_PROCESS : evStopSamplesAcquisition

		WAIT_FOR_DELAY_BTW_SAMPLES: do / WaitForDelayBtwSamples()
		WAIT_FOR_DELAY_BTW_SAMPLES --> WAIT_FOR_NEXT_SAMPLE : evDelayBtwSamplesExpired

		SAMPLES_PROCESS : do / ProcessSamples()
        SAMPLES_PROCESS --> OWI_VRO_COMPLETE : evSampleProcessed

    @enduml
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "OwiVroRead.h"

// -- Other Modules --
#include "DishAuxVar.h"
#include "Fault.h"
#include "limits.h"
#include "Log.h"
#include "Interpreter.h"
#include <math.h>
#include "Micro.h"
#include "Millis.h"
#include "NVData.h"
#include "OWIVoltageRegulatedOutput.h"
#include "SettingFile.h"
#include <stdlib.h>
#include <string.h>
#include "Selector.h"
#include "Variable.h"


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Owi Vro State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Owi Vro state machine.
typedef enum OWI_VRO_STATE_ENUM
{
    STATE_OWI_VRO_NONE                       = 0,

    STATE_OWI_VRO_INITIALIZE                 = 1,   //!< (initial_state)
    STATE_OWI_VRO_WAIT_FOR_INITIAL_DELAY     = 2,
    STATE_OWI_VRO_COMPLETE                   = 3,
    STATE_OWI_VRO_WAIT_FOR_NEXT_SAMPLE       = 4,
    STATE_OWI_VRO_WAIT_FOR_DELAY_BTW_SAMPLES = 5,
    STATE_OWI_VRO_SAMPLES_PROCESS            = 6,

    STATE_OWI_VRO_END                        = 7
} OWI_VRO_STATE_TYPE;

// -- Private Function Prototypes used by the state machine for customization functions --
static void CheckForNewSample(void);
static void InitSamplesAcquisition(void);
static void ProcessSamples(void);
static void StartDelay(void);
static void StartNextSampleAcquisition(void);
static void WaitForDelayBtwSamples(void);
static void WaitForInitialDelay(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventDelayBtwSamplesExpired(void);
static void EventInitialDelayExpired(void);
static void EventNewSampleAcquired(void);
static void EventSampleProcessed(void);
static void EventSkipOwiVro(void);
static void EventStartOwiVro(void);
static void EventStopSamplesAcquisition(void);
static void OwiVroExecute(void);
static void OwiVroInitialize(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
#pragma location = "_appliance_segment"
OWI_STATUS_TYPE OWI_VRO_OWI_Status;


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//! The name of this module for use by the Log module.
#define MODULE_NAME MODULE_OWI_VRO_READ

//! The maximum number of duty cycle samples to be acquired during the process.
#define DUTYCYCLE_SAMPLES_MAX_NUM 					8

// DishAuxVar_AverageWithRejection(), used in Owi Vro Read, support a maximum of AVEARAGE_WITH_REJ_MAX_SUPPORTED_SAMPLES_NUM,
// so the number of samples on which calculate the average has to be less than or equal to that limit.
CT_ASSERT((uint8)DUTYCYCLE_SAMPLES_MAX_NUM <= AVEARAGE_WITH_REJ_MAX_SUPPORTED_SAMPLES_NUM);

//! The maximum number of compensation coefficients to be saved for each setpoint.
#define OWI_VRO_SAVED_COMP_COEFFS 					8

// DishAuxVar_AverageWithRejection(), used in Owi Vro Read, support a maximum of AVEARAGE_WITH_REJ_MAX_SUPPORTED_SAMPLES_NUM,
// so the number of samples on which calculate the average has to be less than or equal to that limit.
CT_ASSERT((uint8)OWI_VRO_SAVED_COMP_COEFFS <= AVEARAGE_WITH_REJ_MAX_SUPPORTED_SAMPLES_NUM);

//! Default value for compensation coefficients.
#define DEFAULT_COMP_COEFFS_VALUE 					1000

//! Number of Voltage setpoints for OWI VRO Algorithm
#define OWI_VRO_SETPOINT_NUM						3

//! Number of saved Soil Measures Non Compensated
#define OWI_VRO_SAVED_SOIL_MEASURES_NON_COMP_NUM	6

//! List of values to compare with the F1_F0_Flags.
enum OWI_VRO_READ_OPTION_ENUM
{
	OWI_VRO_TURBIDITY_READ		= 0x00,	//!< F1=0, F0=0
	OWI_VRO_CALIBRATION_READ	= 0x01,	//!< F1=0, F0=1
	OWI_VRO_VERIFICATION_READ	= 0x02,	//!< F1=1, F0=0

	OWI_VRO_READ_OPTION_COUNT
};

//! A list of log message IDs that are generated by this module.
typedef enum MODULE_OWI_VRO_READ_LOG_MESSAGE_ID_ENUM
{
	INVALID_PROCESS_CONTROL = 1,
	ERROR_MISSING_PROCESS_PARAMETERS,
	ERROR_MISSING_MACHINE_CONFIG_PARAMETERS,
	ERROR_INCONSISTENT_PARAMETER_VALUE,
	ERROR_INCONSISTENT_AVERAGE_DATA,
} MODULE_OWI_VRO_READ_LOG_MESSAGE_ID_TYPE;

typedef PACKED struct OWI_VRO_READ_PROCESS_SF_PARAMS_STRUCT
{
	uint8 Setpoint_Index			:3;	//! Index of Voltage setpoint for the process
	uint8 Unused_b37				:5;

	uint8 Samples_To_Acquire		:4;	//! The number of samples to be acquired by the process
	uint8 Samples_To_Reject			:4;	//! The number of samples to be rejected by the process

	uint8 Initial_Delay;			//! Units = 2s
	uint8 Delay_Btw_Samples;		//! Units = 2s
} OWI_VRO_READ_PROCESS_SF_PARAMS_TYPE;

//! Structure of volatile data for Owi Vro Read Process
typedef struct OWI_VRO_READ_PROCESS_DATA_STRUCT
{
	const OWI_VRO_READ_PROCESS_SF_PARAMS_TYPE* Process_Params_Ptr;
	const OWI_VRO_COMMON_SF_PARAMS_TYPE* Common_Params_Ptr;
	const OWI_VRO_SETPOINT_SF_PARAMS_TYPE* Setpoint_Params_Ptr;
	MILLIS_TIMER_TYPE Timer;
	uint16 DutyCycle_Stable_Samples[DUTYCYCLE_SAMPLES_MAX_NUM];
	uint16 DutyCycle_Comp_Samples[DUTYCYCLE_SAMPLES_MAX_NUM];
	uint16 Avg_DutyCycle_Stable;
	uint16 Avg_DutyCycle_Comp;
	uint8 Process_Config;
	uint8 Setpoint_Index;
	OWI_VRO_STATE_TYPE State;
	uint8 Sample_Sequence_Id;
	uint8 Num_Samples_Acquired;
	BOOL_TYPE Sample_Timeout_Expired;
} OWI_VRO_READ_PROCESS_DATA_TYPE;

//! Structure of non volatile data for Owi Vro Read Process
typedef struct OWI_VRO_READ_PROCESS_NV_DATA_STRUCT
{
	uint8 Cycles_To_Calibration;
	uint8 Cal_Read_Retry_Counter;
	uint8 Completed_Calibration;
} OWI_VRO_READ_PROCESS_NV_DATA_TYPE;

//! Array of displacements index of for i-th setpoint parameters in SF_PTR_ACU_ALGORITHM_PARAMETERS pointer data area
static const uint8 OWI_VRO_Setpoint_Displacements[OWI_VRO_SETPOINT_NUM] = {
	DISPL_OWI_VRO_SETPOINT_1_PARAMS,
	DISPL_OWI_VRO_SETPOINT_2_PARAMS,
	DISPL_OWI_VRO_SETPOINT_3_PARAMS,
};

//! Volatile data for Owi Vro Read Process
static OWI_VRO_READ_PROCESS_DATA_TYPE OwiVroRead_Data;

//! Non volatile data for Owi Vro Read Process
#pragma location = "_appliance_segment"
static OWI_VRO_READ_PROCESS_NV_DATA_TYPE OwiVroRead_NV_Data;

//! FIFO Array of OWI VRO historical compensation coefficients, one array for each setpoint. The stored value is actual value x 1000.
#pragma location = "_appliance_segment"
static uint16 OWI_VRO_Comp_Coeffs_Array[OWI_VRO_SETPOINT_NUM][OWI_VRO_SAVED_COMP_COEFFS];

//! OWI VRO average compensation coefficients, one for each setpoint. The stored value is actual value x 1000.
#pragma location = "_appliance_segment"
static uint16 OWI_VRO_Avg_Coeff_SP[OWI_VRO_SETPOINT_NUM];

//! Array of saved Soil Measures Non Compensated
#pragma location = "_appliance_segment"
static uint16 OWI_VRO_Soil_Measure_NonComp[OWI_VRO_SAVED_SOIL_MEASURES_NON_COMP_NUM];

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void ReadOwiVroParameters(PROCESS_CONTROL_TYPE* control);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
/**
 * Initialize the OWI VRO module's data.
 */
void OwiVroRead__Initialize(void)
{
	if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
	{
		Millis__Reset(&OwiVroRead_Data.Timer);

		if (NVData__GetLastRestoreStatus()->Bit.Appliance_Data_Recovered != (NVDATA_RESTORE_TYPE)NVDATA_RESTORE_OKAY)
		{
			uint8 i, j;
			for (i = 0; i < ELEMENTS_IN_ARRAY(OWI_VRO_Comp_Coeffs_Array); i++)
			{
				for (j = 0; j < ELEMENTS_IN_ARRAY(OWI_VRO_Comp_Coeffs_Array[0]); j++)
				{
					OWI_VRO_Comp_Coeffs_Array[i][j] = DEFAULT_COMP_COEFFS_VALUE;
				}
			}

			for (i = 0; i < ELEMENTS_IN_ARRAY(OWI_VRO_Avg_Coeff_SP); i++)
			{
				OWI_VRO_Avg_Coeff_SP[i] = DEFAULT_COMP_COEFFS_VALUE;
			}

			for (i = 0; i < ELEMENTS_IN_ARRAY(OWI_VRO_Soil_Measure_NonComp); i++)
			{
				OWI_VRO_Soil_Measure_NonComp[i] = 0;
			}

			OwiVroRead_NV_Data.Cal_Read_Retry_Counter = 0;
			OwiVroRead_NV_Data.Cycles_To_Calibration = 0;
			OwiVroRead_NV_Data.Completed_Calibration = 0;

			OWI_VRO_OWI_Status = OWI_STATUS_NEEDS_CALIBRATION;
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The OWI VRO Read process.
 *
 * It implements the OWI VRO Read algorithm
 *
 * @param control = pointer to the process control parameters provided by the Interpreter.
 * @retval PROCESS_COMPLETE = The process is ready for the Interpreter to proceed to the next step.
 * @retval PROCESS_BUSY = The process is not ready for the Interpreter to proceed to the next step.
 */
PROCESS_RESULT_TYPE OwiVroRead__Process(PROCESS_CONTROL_TYPE* control)
{
    PROCESS_RESULT_TYPE result = PROCESS_COMPLETE;

	if ((control == NULL) ||
		(control->F1_F0_Flags >= (uint8)OWI_VRO_READ_OPTION_COUNT) ||
		(control->Compartment >= APPLIANCE_MAX_COMPARTMENTS) ||
		(control->Process_Instance > (uint8)0))
	{
		LOG_ADD_EXCEPTION(INVALID_PROCESS_CONTROL, (control != NULL));
	}
	else
	{
		if (((uint8)control->Step_Initializing == (uint8)TRUE) ||
			((uint8)control->Resume_From_Reset == (uint8)TRUE) ||
			((uint8)control->Resume_From_Pause == (uint8)TRUE))
		{
			// Initialize this instance of the Owi Vro Read process.
			OwiVroInitialize();
			ReadOwiVroParameters(control);
		}
		else
		{
			// Execute the Owi Vro Read Process state machine.
			OwiVroExecute();
		}

		if (OwiVroRead_Data.State != STATE_OWI_VRO_COMPLETE)
		{
			result = PROCESS_BUSY;
		}
	}

	return result;
}


/**
 * It returns the OWI VRO Compensation coefficient for the specified setpoint
 *
 */
uint16 OwiVroRead__GetAvgCoeff(uint8 setpoint_index)
{
	uint16 retvalue = 0;

	if (setpoint_index < ELEMENTS_IN_ARRAY(OWI_VRO_Avg_Coeff_SP))
	{
		retvalue = OWI_VRO_Avg_Coeff_SP[setpoint_index];
	}

	return retvalue;
}


/**
 * Called at the end of cycle, it manages Cycles_To_Calibration and OWI_VRO_OWI_Status
 *
 */
void OwiVroRead__CycleEndEvent(void)
{
	if (OwiVroRead_NV_Data.Cycles_To_Calibration > (uint8)0)
	{
		OwiVroRead_NV_Data.Cycles_To_Calibration --;
	}
	else
	{
		OWI_VRO_OWI_Status = OWI_STATUS_NEEDS_CALIBRATION;
	}
}


/**
 * Called at the start of a cycle, if the cycle is an Eol or Service cycles it resets the Owi Vro Calibration related variables.
 *
 */
void OwiVroRead__CycleStarted(void)
{
    if (Selector__GetCycleGroup(Interpreter__GetCurrentSFIndex(COMPARTMENT_DEFAULT), COMPARTMENT_DEFAULT, Interpreter__GetStatus().Main_Cycle[COMPARTMENT_DEFAULT]) == CYCLE_GROUP_EOL_TEST)
	{
		uint8 i, j;
		for (i = 0; i < ELEMENTS_IN_ARRAY(OWI_VRO_Comp_Coeffs_Array); i++)
		{
			for (j = 0; j < ELEMENTS_IN_ARRAY(OWI_VRO_Comp_Coeffs_Array[0]); j++)
			{
				OWI_VRO_Comp_Coeffs_Array[i][j] = DEFAULT_COMP_COEFFS_VALUE;
			}
		}

		for (i = 0; i < ELEMENTS_IN_ARRAY(OWI_VRO_Avg_Coeff_SP); i++)
		{
			OWI_VRO_Avg_Coeff_SP[i] = DEFAULT_COMP_COEFFS_VALUE;
		}

		OwiVroRead_NV_Data.Cal_Read_Retry_Counter = 0;
		OwiVroRead_NV_Data.Cycles_To_Calibration = 0;
		OwiVroRead_NV_Data.Completed_Calibration = 0;
		OWI_VRO_OWI_Status = OWI_STATUS_NEEDS_CALIBRATION;
    }
}
//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

/**
 * It reads the Owi Vro Read process parameters and other needed Machine Configuration parameters from the setting file.
 *
 * @param control = pointer to the process control parameters provided by the Interpreter.
 */
static void ReadOwiVroParameters(PROCESS_CONTROL_TYPE* control)
{
    SETTINGFILE_LOADER_TYPE loader;
    BOOL_TYPE start_process = FALSE;
    MODULE_OWI_VRO_READ_LOG_MESSAGE_ID_TYPE msg = ERROR_MISSING_MACHINE_CONFIG_PARAMETERS;
    uint16 msg_data = DISPL_OWI_VRO_COMMON_PARAMS;

    if (SettingFile__MSettingBasicLoader(control->Setting_File_Index,
    									 SF_PTR_ACU_PROCESS_OWI_VRO_READ,
                                         control->Data_Field_Index,
                                         &loader) == PASS)
    {
    	OwiVroRead_Data.Process_Params_Ptr = (const OWI_VRO_READ_PROCESS_SF_PARAMS_TYPE *)(void *)loader.Data;
    	OwiVroRead_Data.Process_Config = control->F1_F0_Flags;
    	OwiVroRead_Data.Setpoint_Index = OwiVroRead_Data.Process_Params_Ptr->Setpoint_Index; // For MAC monitoring

		if ((OwiVroRead_Data.Process_Params_Ptr->Samples_To_Acquire > (uint8)0) &&
			(OwiVroRead_Data.Process_Params_Ptr->Samples_To_Acquire <= (uint8)DUTYCYCLE_SAMPLES_MAX_NUM) &&
			(OwiVroRead_Data.Process_Params_Ptr->Samples_To_Acquire > OwiVroRead_Data.Process_Params_Ptr->Samples_To_Reject) &&
			(OwiVroRead_Data.Setpoint_Index < (uint8)OWI_VRO_SETPOINT_NUM))
		{

		    if (SettingFile__MSettingBasicLoader(control->Setting_File_Index,
		    									 SF_PTR_ACU_ALGORITHM_PARAMETERS,
												 DISPL_OWI_VRO_COMMON_PARAMS,
		                                         &loader) == PASS)
		    {
		    	OwiVroRead_Data.Common_Params_Ptr = (const OWI_VRO_COMMON_SF_PARAMS_TYPE *)(void *)loader.Data;

			    if (OwiVroRead_Data.Common_Params_Ptr->Coeffs_To_Acquire <= (uint8)OWI_VRO_SAVED_COMP_COEFFS)
			    {
					if (SettingFile__MSettingBasicLoader(control->Setting_File_Index,
														 SF_PTR_ACU_ALGORITHM_PARAMETERS,
														 OWI_VRO_Setpoint_Displacements[OwiVroRead_Data.Setpoint_Index],
														 &loader) == PASS)
					{
						OwiVroRead_Data.Setpoint_Params_Ptr = (const OWI_VRO_SETPOINT_SF_PARAMS_TYPE *)(void *)loader.Data;

						start_process = TRUE;
					}
					else
					{
						// Setpoint Parameters missing, default log message, only update msg_data
						msg_data = OWI_VRO_Setpoint_Displacements[OwiVroRead_Data.Setpoint_Index];
					}
			    }
			    else
			    {
					// Inconsistent common parameters value
					msg = ERROR_INCONSISTENT_PARAMETER_VALUE;
					msg_data = OwiVroRead_Data.Common_Params_Ptr->Coeffs_To_Acquire;

			    }
		    }
		    else
		    {
				// Machine Configuration Common Parameters missing, default log message data
		    }
		}
		else
		{
			// Inconsistent process parameters value
			msg = ERROR_INCONSISTENT_PARAMETER_VALUE;
			msg_data = 0;
		}
    }
	else
	{
		// Process parameters missing
		msg = ERROR_MISSING_PROCESS_PARAMETERS;
		msg_data = control->Data_Field_Index;
	}

    if ((uint8)start_process == (uint8)TRUE)
    {
    	EventStartOwiVro();
    }
    else
    {
		LOG_ADD_EXCEPTION(msg, msg_data);
		EventSkipOwiVro();
    }
}




//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void CheckForNewSample(void)
{
	if ((uint8)Millis__ReachedTargetSeconds(&OwiVroRead_Data.Timer, OwiVroRead_Data.Common_Params_Ptr->Sample_Max_Time * (uint32)10) == (uint8)TRUE)
	{
		// Sample acquisition timeout expired
		OwiVroRead_Data.Sample_Timeout_Expired = TRUE;

		EventStopSamplesAcquisition();
	}
	else
	{
		if (OWIVoltageRegulatedOutput__GetSequenceId() != OwiVroRead_Data.Sample_Sequence_Id)
		{
			// New sample available

			sint32 duty_cycle_value;
			if (Variable__GetValue(VARIABLE_OWI_VRO_DUTYCYCLE_STABLE, 0, &duty_cycle_value) == FALSE)
			{
				duty_cycle_value = 0;
			}

			OwiVroRead_Data.DutyCycle_Stable_Samples[OwiVroRead_Data.Num_Samples_Acquired] = (uint16)duty_cycle_value;

			if (OwiVroRead_Data.Process_Config != (uint8)OWI_VRO_CALIBRATION_READ)
			{
				if (Variable__GetValue(VARIABLE_OWI_VRO_DUTYCYCLE_COMP, 0, &duty_cycle_value) == FALSE)
				{
					duty_cycle_value = 0;
				}

				OwiVroRead_Data.DutyCycle_Comp_Samples[OwiVroRead_Data.Num_Samples_Acquired] = (uint16)duty_cycle_value;
			}

			OwiVroRead_Data.Num_Samples_Acquired ++;

			// Stop the model
			OWIVoltageRegulatedOutput__RequestStop();

			if (OwiVroRead_Data.Num_Samples_Acquired >= OwiVroRead_Data.Process_Params_Ptr->Samples_To_Acquire)
			{
				// All samples acquired
				EventStopSamplesAcquisition();
			}
			else
			{
				EventNewSampleAcquired();
			}
		}
		else
		{
			OWIVoltageRegulatedOutput__RequestKeepRunning();
		}
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void ProcessSamples(void)
{
	sint32 variable_value;
	uint8 i;

	if (OwiVroRead_Data.Sample_Timeout_Expired == FALSE)
	{
		// Apply Average with Rejection to acquired samples
		if (DishAuxVar_AverageWithRejection(&OwiVroRead_Data.DutyCycle_Stable_Samples[0],
											 ELEMENTS_IN_ARRAY(OwiVroRead_Data.DutyCycle_Stable_Samples),
											 OwiVroRead_Data.Process_Params_Ptr->Samples_To_Acquire,
											 OwiVroRead_Data.Process_Params_Ptr->Samples_To_Acquire - OwiVroRead_Data.Process_Params_Ptr->Samples_To_Reject,
											 &OwiVroRead_Data.Avg_DutyCycle_Stable) == FAIL)
		{
			LOG_ADD_EXCEPTION(ERROR_INCONSISTENT_AVERAGE_DATA, OwiVroRead_Data.Process_Params_Ptr->Samples_To_Acquire);
		}

		if (OwiVroRead_Data.Process_Config != (uint8)OWI_VRO_CALIBRATION_READ)
		{
			if (DishAuxVar_AverageWithRejection(&OwiVroRead_Data.DutyCycle_Comp_Samples[0],
												 ELEMENTS_IN_ARRAY(OwiVroRead_Data.DutyCycle_Comp_Samples),
					  	  	  	  	  	  	  	 OwiVroRead_Data.Process_Params_Ptr->Samples_To_Acquire,
												 OwiVroRead_Data.Process_Params_Ptr->Samples_To_Acquire - OwiVroRead_Data.Process_Params_Ptr->Samples_To_Reject,
												 &OwiVroRead_Data.Avg_DutyCycle_Comp) == FAIL)
			{
				LOG_ADD_EXCEPTION(ERROR_INCONSISTENT_AVERAGE_DATA, OwiVroRead_Data.Process_Params_Ptr->Samples_To_Acquire);
			}
		}
	}

	// Execute calculations on average depending on F1 F0
	switch (OwiVroRead_Data.Process_Config)
	{
		case OWI_VRO_TURBIDITY_READ:
			// Update Soil_Measure_x and Soil_Measure_x_NonComp
			if ((uint8)Variable__GetValue(VARIABLE_SOIL_MEASURE_5, 0, &variable_value) == (uint8)TRUE)
			{
				(void)Variable__SetValue (VARIABLE_OPERATION_SET, VARIABLE_SOIL_MEASURE_6, 0, variable_value);
			}
			if ((uint8)Variable__GetValue(VARIABLE_SOIL_MEASURE_4, 0, &variable_value) == (uint8)TRUE)
			{
				(void)Variable__SetValue (VARIABLE_OPERATION_SET, VARIABLE_SOIL_MEASURE_5, 0, variable_value);
			}
			if ((uint8)Variable__GetValue(VARIABLE_SOIL_MEASURE_3, 0, &variable_value) == (uint8)TRUE)
			{
				(void)Variable__SetValue (VARIABLE_OPERATION_SET, VARIABLE_SOIL_MEASURE_4, 0, variable_value);
			}
			if ((uint8)Variable__GetValue(VARIABLE_SOIL_MEASURE_2, 0, &variable_value) == (uint8)TRUE)
			{
				(void)Variable__SetValue (VARIABLE_OPERATION_SET, VARIABLE_SOIL_MEASURE_3, 0, variable_value);
			}
			if ((uint8)Variable__GetValue(VARIABLE_SOIL_MEASURE_RECENT, 0, &variable_value) == (uint8)TRUE)
			{
				(void)Variable__SetValue (VARIABLE_OPERATION_SET, VARIABLE_SOIL_MEASURE_2, 0, variable_value);
			}

			variable_value = 1; // Default value in case of sample timeout
			if (OwiVroRead_Data.Sample_Timeout_Expired == FALSE)
			{
				variable_value = OwiVroRead_Data.Avg_DutyCycle_Comp;
			}

			(void)Variable__SetValue (VARIABLE_OPERATION_SET, VARIABLE_SOIL_MEASURE_RECENT, 0, variable_value);

			// Update OWI_VRO_Soil_Measure_NonComp[] array
			for (i = (ELEMENTS_IN_ARRAY(OWI_VRO_Soil_Measure_NonComp) - (uint8)1); i > (uint8)0; i --)
			{
				OWI_VRO_Soil_Measure_NonComp[i] = OWI_VRO_Soil_Measure_NonComp[i - (uint8)1];
			}

			if (OwiVroRead_Data.Sample_Timeout_Expired == FALSE)
			{
				variable_value = OwiVroRead_Data.Avg_DutyCycle_Stable;
			}

			OWI_VRO_Soil_Measure_NonComp[0] = (uint16)variable_value;

			break;

		case OWI_VRO_CALIBRATION_READ:
			// Calculate new coefficient from OwiVroRead_Data.Avg_DutyCycle_Stable
			if ((ENDIANU16_STON(OwiVroRead_Data.Setpoint_Params_Ptr->Compensation_Target) > (uint16)0) && (OwiVroRead_Data.Sample_Timeout_Expired == FALSE))
			{
				uint16 *coeffs_ptr = OWI_VRO_Comp_Coeffs_Array[OwiVroRead_Data.Setpoint_Index];
				sint32 diff;
				float32 coeff_local;

				// Calculate new coefficient from NON compensated average duty cycle acquired during this instance of the Calibration process
				// Result is for sure <= 2 * DEFAULT_COMP_COEFFS_VALUE.
				diff = (sint32)ENDIANU16_STON(OwiVroRead_Data.Setpoint_Params_Ptr->Compensation_Target) - (sint32)OwiVroRead_Data.Avg_DutyCycle_Stable;
				coeff_local = (float32)DEFAULT_COMP_COEFFS_VALUE + (((float32)DEFAULT_COMP_COEFFS_VALUE * (float32)(diff))/(float32)ENDIANU16_STON(OwiVroRead_Data.Setpoint_Params_Ptr->Compensation_Target));

				if(coeff_local < 0.0F)
				{
					coeff_local = 0.0F;
				}

				// Shift FIFO array
				for (i = (ELEMENTS_IN_ARRAY(OWI_VRO_Comp_Coeffs_Array[0]) - (uint8)1); i > (uint8)0; i --)
				{
					coeffs_ptr[i] = coeffs_ptr[i - (uint8)1];
				}

				coeffs_ptr[0] = (uint16)lroundf(coeff_local);

				if (OwiVroRead_NV_Data.Completed_Calibration < OwiVroRead_Data.Common_Params_Ptr->Coeffs_To_Acquire)
				{
					OwiVroRead_NV_Data.Completed_Calibration ++;
				}

				if (OwiVroRead_Data.Common_Params_Ptr->Coeffs_To_Acquire > OwiVroRead_Data.Common_Params_Ptr->Coeffs_To_Reject)
				{
					if (DishAuxVar_AverageWithRejection(coeffs_ptr,
														ELEMENTS_IN_ARRAY(OWI_VRO_Comp_Coeffs_Array[0]),
							   	   	   	   	   	   	   	OwiVroRead_NV_Data.Completed_Calibration,
														OwiVroRead_Data.Common_Params_Ptr->Coeffs_To_Acquire - OwiVroRead_Data.Common_Params_Ptr->Coeffs_To_Reject,
														&OWI_VRO_Avg_Coeff_SP[OwiVroRead_Data.Setpoint_Index]) == FAIL)
					{
						LOG_ADD_EXCEPTION(ERROR_INCONSISTENT_AVERAGE_DATA, OwiVroRead_NV_Data.Completed_Calibration);
					}
				}
			}
			break;

		case OWI_VRO_VERIFICATION_READ:
			// Check compensated and non compensated values
			if ((OwiVroRead_Data.Avg_DutyCycle_Stable >= ENDIANU16_STON(OwiVroRead_Data.Setpoint_Params_Ptr->NonComp_Lower_Limit)) &&
				(OwiVroRead_Data.Avg_DutyCycle_Stable <= ENDIANU16_STON(OwiVroRead_Data.Setpoint_Params_Ptr->NonComp_Upper_Limit)) &&
				(OwiVroRead_Data.Avg_DutyCycle_Comp >= ENDIANU16_STON(OwiVroRead_Data.Setpoint_Params_Ptr->Comp_Lower_Limit)) &&
				(OwiVroRead_Data.Avg_DutyCycle_Comp <= ENDIANU16_STON(OwiVroRead_Data.Setpoint_Params_Ptr->Comp_Upper_Limit)) &&
				(OwiVroRead_Data.Sample_Timeout_Expired == FALSE))
			{
				// Verification passed

				OWI_VRO_OWI_Status = OWI_STATUS_CALIBRATED;
				OwiVroRead_NV_Data.Cal_Read_Retry_Counter = 0;
				OwiVroRead_NV_Data.Cycles_To_Calibration = OwiVroRead_Data.Common_Params_Ptr->Cycles_Btw_Calibration;
			}
			else
			{
				// Verification failed

				OWI_VRO_OWI_Status = OWI_STATUS_NEEDS_CALIBRATION;
				OwiVroRead_NV_Data.Cal_Read_Retry_Counter ++;

				if (OwiVroRead_NV_Data.Cal_Read_Retry_Counter >= OwiVroRead_Data.Common_Params_Ptr->Calibration_Read_Retry_Max)
				{
					OWI_VRO_OWI_Status = OWI_STATUS_NON_SENSOR_MODE;
					OwiVroRead_NV_Data.Cal_Read_Retry_Counter = 0;
					OwiVroRead_NV_Data.Cycles_To_Calibration = OwiVroRead_Data.Common_Params_Ptr->Cycles_Btw_Calibration;
		            Fault__ConditionDetected(FAULT_ID_OWI_CALIBRATION_FAILURE);
				}
			}

			break;

		default:
			// Nothing to do
			break;
	}

	EventSampleProcessed();
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void InitSamplesAcquisition(void)
{
	OwiVroRead_Data.Num_Samples_Acquired = 0;
	OwiVroRead_Data.Sample_Timeout_Expired = FALSE;
	memset(&OwiVroRead_Data.DutyCycle_Stable_Samples[0], 0, sizeof(OwiVroRead_Data.DutyCycle_Stable_Samples));
	memset(&OwiVroRead_Data.DutyCycle_Comp_Samples[0], 0, sizeof(OwiVroRead_Data.DutyCycle_Comp_Samples));
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void StartDelay(void)
{
    Millis__Start(&OwiVroRead_Data.Timer);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void WaitForInitialDelay(void)
{
	if ((uint8)Millis__ReachedTargetSeconds(&OwiVroRead_Data.Timer, OwiVroRead_Data.Process_Params_Ptr->Initial_Delay * (uint32)2) == (uint8)TRUE)
	{
		EventInitialDelayExpired();
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void StartNextSampleAcquisition(void)
{
	Millis__Start(&OwiVroRead_Data.Timer);
	OwiVroRead_Data.Sample_Sequence_Id = OWIVoltageRegulatedOutput__GetSequenceId();
	OWIVoltageRegulatedOutput__RequestReset(OwiVroRead_Data.Setpoint_Index);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void WaitForDelayBtwSamples(void)
{
	if ((uint8)Millis__ReachedTargetSeconds(&OwiVroRead_Data.Timer, OwiVroRead_Data.Process_Params_Ptr->Delay_Btw_Samples * (uint32)2) == (uint8)TRUE)
	{
		EventDelayBtwSamplesExpired();
	}
	else
	{
		OWIVoltageRegulatedOutput__RequestKeepRunning();
	}
}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Owi Vro State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Delay Btw Samples Expired Event for the Owi Vro state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDelayBtwSamplesExpired(void)
{
    switch (OwiVroRead_Data.State)
    {
        case STATE_OWI_VRO_WAIT_FOR_DELAY_BTW_SAMPLES:
            OwiVroRead_Data.State = STATE_OWI_VRO_WAIT_FOR_NEXT_SAMPLE;
            StartNextSampleAcquisition();           // Entry Function.
            break;

        default:
            // Event is ignored for all other values of OwiVroRead_Data.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Initial Delay Expired Event for the Owi Vro state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventInitialDelayExpired(void)
{
    switch (OwiVroRead_Data.State)
    {
        case STATE_OWI_VRO_WAIT_FOR_INITIAL_DELAY:
            InitSamplesAcquisition();               // Exit function.
            OwiVroRead_Data.State = STATE_OWI_VRO_WAIT_FOR_NEXT_SAMPLE;
            StartNextSampleAcquisition();           // Entry Function.
            break;

        default:
            // Event is ignored for all other values of OwiVroRead_Data.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the New Sample Acquired Event for the Owi Vro state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventNewSampleAcquired(void)
{
    switch (OwiVroRead_Data.State)
    {
        case STATE_OWI_VRO_WAIT_FOR_NEXT_SAMPLE:
            OwiVroRead_Data.State = STATE_OWI_VRO_WAIT_FOR_DELAY_BTW_SAMPLES;
            break;

        default:
            // Event is ignored for all other values of OwiVroRead_Data.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Sample Processed Event for the Owi Vro state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventSampleProcessed(void)
{
    switch (OwiVroRead_Data.State)
    {
        case STATE_OWI_VRO_SAMPLES_PROCESS:
            OwiVroRead_Data.State = STATE_OWI_VRO_COMPLETE;
            break;

        default:
            // Event is ignored for all other values of OwiVroRead_Data.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Skip Owi Vro Event for the Owi Vro state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventSkipOwiVro(void)
{
    switch (OwiVroRead_Data.State)
    {
        case STATE_OWI_VRO_INITIALIZE:
            OwiVroRead_Data.State = STATE_OWI_VRO_COMPLETE;
            break;

        default:
            // Event is ignored for all other values of OwiVroRead_Data.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Start Owi Vro Event for the Owi Vro state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventStartOwiVro(void)
{
    switch (OwiVroRead_Data.State)
    {
        case STATE_OWI_VRO_INITIALIZE:
            OwiVroRead_Data.State = STATE_OWI_VRO_WAIT_FOR_INITIAL_DELAY;
            StartDelay();                           // Entry Function.
            break;

        default:
            // Event is ignored for all other values of OwiVroRead_Data.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Stop Samples Acquisition Event for the Owi Vro state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventStopSamplesAcquisition(void)
{
    switch (OwiVroRead_Data.State)
    {
        case STATE_OWI_VRO_WAIT_FOR_NEXT_SAMPLE:
            OwiVroRead_Data.State = STATE_OWI_VRO_SAMPLES_PROCESS;
            break;

        default:
            // Event is ignored for all other values of OwiVroRead_Data.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Owi Vro state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void OwiVroExecute(void)
{
    switch(OwiVroRead_Data.State)
    {
        case STATE_OWI_VRO_WAIT_FOR_INITIAL_DELAY:
            WaitForInitialDelay();                  // Do function.
            break;

        case STATE_OWI_VRO_WAIT_FOR_NEXT_SAMPLE:
            CheckForNewSample();                    // Do function.
            break;

        case STATE_OWI_VRO_WAIT_FOR_DELAY_BTW_SAMPLES:
            WaitForDelayBtwSamples();               // Do function.
            break;

        case STATE_OWI_VRO_SAMPLES_PROCESS:
            ProcessSamples();                       // Do function.
            break;

        default:
            // There is nothing to do for all other values of OwiVroRead_Data.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Owi Vro state machine.
 * This function resets the state machine to the initial state.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void OwiVroInitialize(void)
{
    OwiVroRead_Data.State = STATE_OWI_VRO_INITIALIZE;
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

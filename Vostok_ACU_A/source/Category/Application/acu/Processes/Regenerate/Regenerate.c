/**
 *  @file		Regenerate.c
 *
 *  @brief		A process that implements the Water Softener Regeneration algorithm
 *
 *  @details	To neutralize the limestone's effects, the dishwasher is equipped with a water softener whose resins,
 *  			cycle after cycle, gradually run out. To restore the resins' functionality a process called 'regeneration'
 *  			has to be executed, which makes water flow through the salt tank and then through the resins of the the water
 *  			softener.
 *  			This process, when called inside the cycle, checks the conditions to run a regeneration and if they are satisfied
 *  			executes the regeneration itself.
 *
 *
   @startuml{Regenerate State Machine.jpg}
        title Regenerate State Machine
        ' Use state = Regenerate_Data.State

		[*] --> INITIALIZE

		INITIALIZE : do / InitializeDo()
		INITIALIZE --> OPEN_REG_VALVE_1 : evStartRegenerate
        INITIALIZE --> REGENERATION_COMPLETE : evSkipRegeneration

		OPEN_REG_VALVE_1 : entry / OpenRegenerationValve1Entry()
		OPEN_REG_VALVE_1 : do / OpenRegenerationValve1Do()
		OPEN_REG_VALVE_1 --> OPEN_REG_AND_FILL_VALVE : evDelayRvFvOnExpired
        OPEN_REG_VALVE_1 --> REGENERATION_COMPLETE : evSkipRegeneration

		OPEN_REG_AND_FILL_VALVE : entry / OpenRegenerationAndFillValveEntry()
		OPEN_REG_AND_FILL_VALVE : do / OpenRegenerationAndFillValveDo()
		OPEN_REG_AND_FILL_VALVE --> OPEN_REG_VALVE_2 : evRegenerationWaterLoaded
        OPEN_REG_AND_FILL_VALVE --> REGENERATION_COMPLETE : evSkipRegeneration

		OPEN_REG_VALVE_2 : entry / OpenRegenerationValve2Entry()
		OPEN_REG_VALVE_2 : do / OpenRegenerationValve2Do()
		OPEN_REG_VALVE_2 --> REGENERATION_CLOSURE : evDelayFvRvOffExpired
        OPEN_REG_VALVE_2 --> REGENERATION_COMPLETE : evSkipRegeneration

		REGENERATION_CLOSURE : entry / RegenerationClosureEntry()
		REGENERATION_CLOSURE : do / RegenerationClosureDo()
		REGENERATION_CLOSURE --> REGENERATION_CLOSURE : evRestartClosureState
		REGENERATION_CLOSURE --> REGENERATION_COMPLETE : evRegenerationProcessCompleted

    @enduml
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "Regenerate.h"

// -- Other Modules --
#include "DishAuxVar.h"
#include "Log.h"
#include "Hbl.h"
#include "Micro.h"
#include "NVData.h"
#include "Regulations.h"
#include "SaltSensingMonitor.h"
#include "SettingFile.h"
#include "Variable.h"

//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Definitions for Regenerate State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//! Unique names used to identify states of the Regenerate state machine.
typedef enum REGENERATE_STATE_ENUM
{
    STATE_REGENERATE_NONE                    = 0,

    STATE_REGENERATE_INITIALIZE              = 1,   //!< (initial_state)
    STATE_REGENERATE_OPEN_REG_VALVE_1        = 2,
    STATE_REGENERATE_REGENERATION_COMPLETE   = 3,
    STATE_REGENERATE_OPEN_REG_AND_FILL_VALVE = 4,
    STATE_REGENERATE_OPEN_REG_VALVE_2        = 5,
    STATE_REGENERATE_REGENERATION_CLOSURE    = 6,

    STATE_REGENERATE_END                     = 7
} REGENERATE_STATE_TYPE;

// -- Private Function Prototypes used by the state machine for customization functions --
static void InitializeDo(void);
static void OpenRegenerationAndFillValveDo(void);
static void OpenRegenerationAndFillValveEntry(void);
static void OpenRegenerationValve1Do(void);
static void OpenRegenerationValve1Entry(void);
static void OpenRegenerationValve2Do(void);
static void OpenRegenerationValve2Entry(void);
static void RegenerationClosureDo(void);
static void RegenerationClosureEntry(void);

// -- Private Function Prototypes used by the state machine for standard state machine functions --
static void EventDelayFvRvOffExpired(void);
static void EventDelayRvFvOnExpired(void);
static void EventRegenerationProcessCompleted(void);
static void EventRegenerationWaterLoaded(void);
static void EventRestartClosureState(void);
static void EventSkipRegeneration(void);
static void EventStartRegenerate(void);
static void RegenerateExecute(void);
static void RegenerateInitialize(void);

// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
#pragma location = "_appliance_segment"
uint8 RegenerateAndSaltBitmap;

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//! The name of this module for use by the Log module.
#define MODULE_NAME MODULE_REGENERATE

//! The number of times that the Regenerate__Process function will be called every second while running.
#define REGENERATE_PROCESS_CALLS_PER_SECOND 	10

// If the call rate of the process is different from 10, the counters have to be rescaled
CT_ASSERT(REGENERATE_PROCESS_CALLS_PER_SECOND == 10);

//! Conversion factor from Delay_RV_On_FV_On, Reg_Seconds and Delay_FV_Off_RV_Off parameters units (200 ms) to counter units (100 ms)
#define TIME_PARAMATERS_RESCALE_FACTOR		2

//! Conversion factor from Reg_Liters parameter units (5 cl) to cl
#define REG_LITERS_TO_CL_CONVERSION_FACTOR	5

//! List of values to compare with the F1_F0_Flags.
enum REGENERATE_OPTION_ENUM
{
	REGENERATE_OPTION_DEFAULT,		//!< F1=0, F0=0
	REGENERATE_OPTION_COUNT
};

//! A list of log message IDs that are generated by this module.
typedef enum MODULE_REGENERATE_LOG_MESSAGE_ID_ENUM
{
    INVALID_PROCESS_CONTROL = 1,
	ERROR_MISSING_PROCESS_PARAMETERS,
	ERROR_MISSING_WATER_SOFTENER_PARAMETERS,
} MODULE_REGENERATE_LOG_MESSAGE_ID_TYPE;


typedef PACKED struct REGENERATE_PROCESS_SF_PARAMS_STRUCT
{
	uint16 Reg_Seconds;	//! Maximum time the fill valve can be activated in the regeneration. Units = 200 ms
	uint8 Reg_Liters;	//! Amount of water to be filled from the fill valve in the regeneration. Units = 5 cl.
	uint8 Last_Step		:1;	//! If 1, this is supposed to be the last regeneration step in the cycle.
	uint8 Unused_B17	:7;
} REGENERATE_PROCESS_SF_PARAMS_TYPE;

typedef struct REGENERATE_PROCESS_DATA_STRUCT
{
	REGENERATE_PROCESS_SF_PARAMS_TYPE* Process_Params_Ptr;
	WATER_SOFTENER_SF_PARAMS_TYPE* Water_Softener_Params_Ptr;
	uint16 Counter;
	REGENERATE_STATE_TYPE State;
} REGENERATE_PROCESS_DATA_TYPE;

typedef struct REGENERATE_PROCESS_NV_DATA_STRUCT
{
	uint16 Counter;
	uint8 Num_Regeneration;
	BOOL_TYPE Force_Regeneration;
	REGENERATE_STATE_TYPE State;
} REGENERATE_PROCESS_NV_DATA_TYPE;

//! Volatile data for Regenerate Process
static REGENERATE_PROCESS_DATA_TYPE Regenerate_Data;

//! Non volatile data for Regenerate Process
#pragma location = "_appliance_segment"
static REGENERATE_PROCESS_NV_DATA_TYPE Regenerate_Nv_Data;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
void ReadRegenerateParameters(PROCESS_CONTROL_TYPE* control);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
/**
 * Initialize the Regenerate module's non-volatile data if non-volatile data was corrupted.
 */
void Regenerate__Initialize(void)
{
	if ((Micro__GetResetMode() == MICRO_RESET_MODE_COLD) &&
	    (NVData__GetLastRestoreStatus()->Bit.Appliance_Data_Recovered != (NVDATA_RESTORE_TYPE)NVDATA_RESTORE_OKAY))
	{
		RegenerateAndSaltBitmap = 0;
		Regenerate_Nv_Data.Counter = 0;
		Regenerate_Nv_Data.Num_Regeneration = 0xFF;
		Regenerate_Nv_Data.Force_Regeneration = FALSE;
		Regenerate_Nv_Data.State = STATE_REGENERATE_NONE;
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The Regenerate process.
 *
 * It implements the Water Softener Regeneration algorithm
 *
 * @param control = pointer to the process control parameters provided by the Interpreter.
 * @retval PROCESS_COMPLETE = The process is ready for the Interpreter to proceed to the next step.
 * @retval PROCESS_BUSY = The process is not ready for the Interpreter to proceed to the next step.
 */
PROCESS_RESULT_TYPE Regenerate__Process(PROCESS_CONTROL_TYPE* control)
{
    PROCESS_RESULT_TYPE result = PROCESS_COMPLETE;

    if ((control == NULL) ||
        (control->F1_F0_Flags >= (uint8)REGENERATE_OPTION_COUNT) ||
        (control->Compartment >= APPLIANCE_MAX_COMPARTMENTS) ||
        (control->Process_Instance > (uint8)0))
    {
        LOG_ADD_EXCEPTION(INVALID_PROCESS_CONTROL, (control != NULL));
    }
    else
    {
    	Regenerate_Data.State = Regenerate_Nv_Data.State;
    	Regenerate_Data.Counter = Regenerate_Nv_Data.Counter;

    	if (Regenerate_Data.State == STATE_REGENERATE_NONE)
        {
            // The Regenerate Process state machine has never been initialized -- initialize it.
    		RegenerateInitialize();
        }

        if ((uint8)control->Step_Initializing == (uint8)TRUE)
        {
            // Initialize this instance of the Regenerate process.
        	RegenerateInitialize();
    		ReadRegenerateParameters(control);
        }
        else
        {
        	if (((uint8)control->Resume_From_Reset == (uint8)TRUE) || ((uint8)control->Resume_From_Pause == (uint8)TRUE))
        	{
        		ReadRegenerateParameters(control);

        		if (Regenerate_Data.State == STATE_REGENERATE_REGENERATION_CLOSURE)
        		{
        			// STATE_REGENERATE_REGENERATION_CLOSURE has to be restarted after a cold reset
        			// (included the case in which the Process was paused in STATE_REGENERATE_REGENERATION_CLOSURE
        			// and then a cold reset occurred).
        			EventRestartClosureState();
        		}
        	}

        	// Execute the Regenerate Process state machine.
        	RegenerateExecute();
        }

    	Regenerate_Nv_Data.Counter = Regenerate_Data.Counter;
        Regenerate_Nv_Data.State = Regenerate_Data.State;

    	result = ((Regenerate_Data.State == STATE_REGENERATE_REGENERATION_COMPLETE)? (PROCESS_COMPLETE) : (PROCESS_BUSY));
    }

	return result;
}


/**
 * It returns the number of times a 'Last step' regeneration process has been executed since salt sensor
 * indicated salt absent.
 *
 * @return  The number of times a 'Last step' regeneration process has been executed since salt sensor
 */
uint8 Regenerate__GetNumRegeneration(void)
{
	return Regenerate_Nv_Data.Num_Regeneration;
}


/**
 * It sets the required bit in RegenerateAndSaltBitmap variable
 *
 * @param  bit_index = the index of the bit to be set
 *
 * @return  None
 */
void Regenerate__SetBitmapBit(uint8 bit_index)
{
	if (bit_index < (sizeof(RegenerateAndSaltBitmap) * (uint8)8))
	{
		BIT_SET8(RegenerateAndSaltBitmap, bit_index);
	}
}


/**
 * It clears the required bit in RegenerateAndSaltBitmap variable
 *
 * @param  bit_index = the index of the bit to be set
 *
 * @return  None
 */
void Regenerate__ClearBitmapBit(uint8 bit_index)
{
	if (bit_index < (sizeof(RegenerateAndSaltBitmap) * (uint8)8))
	{
		BIT_CLR(RegenerateAndSaltBitmap, bit_index);
	}
}


/**
 * It returns whether the required bit of RegenerateAndSaltBitmap variable is set or not
 *
 * @param  bit_index = the index of the bit to be set
 *
 * @return  The bit status
 * @retval TRUE/FALSE if the bit is 1/0
 *
 */
BOOL_TYPE Regenerate__GetBitmapBit(uint8 bit_index)
{
	BOOL_TYPE ret_value = FALSE;

	if (bit_index < (sizeof(RegenerateAndSaltBitmap) * (uint8)8))
	{
		ret_value = (BIT_TEST(RegenerateAndSaltBitmap, bit_index) != (uint8)0) ? (TRUE) : (FALSE);
	}

	return ret_value;
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

/**
 * It reads the Regenerate process parameters from the setting file.
 *
 * @param control = pointer to the process control parameters provided by the Interpreter.
 */
void ReadRegenerateParameters(PROCESS_CONTROL_TYPE* control)
{
    SETTINGFILE_LOADER_TYPE loader;

	Regenerate_Data.Process_Params_Ptr = (REGENERATE_PROCESS_SF_PARAMS_TYPE *)NULL;
	Regenerate_Data.Water_Softener_Params_Ptr = (WATER_SOFTENER_SF_PARAMS_TYPE *)NULL;

	// Get the Regenerate process parameters from the setting file.
    if (SettingFile__MSettingBasicLoader(control->Setting_File_Index,
    									SF_PTR_ACU_PROCESS_REGENERATE,
                                         control->Data_Field_Index,
                                         &loader) == PASS)
    {
		Regenerate_Data.Process_Params_Ptr = (REGENERATE_PROCESS_SF_PARAMS_TYPE *)(void *)loader.Data;

		// Get the Water softener parameters from the setting file.
		if (SettingFile__BasicLoader(SF_PTR_ACU_ALGORITHM_PARAMETERS, DSPL_WATER_SOFTENER, &loader) == PASS)
		{
			Regenerate_Data.Water_Softener_Params_Ptr = (WATER_SOFTENER_SF_PARAMS_TYPE *)(void *)loader.Data;
		}
		else
		{
			// Parameters missing
			LOG_ADD_EXCEPTION(ERROR_MISSING_WATER_SOFTENER_PARAMETERS, DSPL_WATER_SOFTENER);

			EventSkipRegeneration();
		}
	}
	else
	{
		// Parameters missing
		LOG_ADD_EXCEPTION(ERROR_MISSING_PROCESS_PARAMETERS, control->Data_Field_Index);

		EventSkipRegeneration();
	}
}

//=====================================================================================================================
//  --- Private State Machine Customization Functions -----------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 * It initialize the Regenerate process, testing the conditions to decide whether the process has to be
 * executed or skipped.
 */
static void InitializeDo(void)
{

	// Check conditions to execute a regeneration
	if ((uint8)Regenerate_Data.Process_Params_Ptr->Last_Step != (uint8)0)
	{
		// Last step Regeneration

		if (BIT_TEST(RegenerateAndSaltBitmap, FST_REGENERATION_DONE) != (uint8)0)
		{
			// A first step regeneration was executed
			BIT_CLR(RegenerateAndSaltBitmap, FST_REGENERATION_DONE);

			// Execute "Last step" regeneration
			EventStartRegenerate();
		}
		else
		{
			EventSkipRegeneration();
		}
	}
	else
	{
		// First step Regeneration

		BOOL_TYPE execute_regeneration = FALSE;
		uint8 water_hardness_func_pos;

		if ((BIT_TEST(RegenerateAndSaltBitmap, SALT_RISING_EDGE) == (uint8)0) || (BIT_TEST(RegenerateAndSaltBitmap, SALT_FALLING_EDGE) == (uint8)0))
		{
			// A single edge of the salt GI was triggered
			if (BIT_TEST(RegenerateAndSaltBitmap, SALT_FALLING_EDGE) != (uint8)0)
			{
				Regenerate_Nv_Data.Num_Regeneration = 1;
			}

			if (BIT_TEST(RegenerateAndSaltBitmap, SALT_RISING_EDGE) != (uint8)0)
			{
				Regenerate_Nv_Data.Num_Regeneration = 1;
				Regenerate_Nv_Data.Force_Regeneration = TRUE;
			}

			BIT_CLR(RegenerateAndSaltBitmap, SALT_FALLING_EDGE);
			BIT_CLR(RegenerateAndSaltBitmap, SALT_RISING_EDGE);
		}

		// Evaluate the conditions to execute a regeneration

		water_hardness_func_pos = Regulations__GetPositionByFunctionID(FUNC_ID_WATER_HARDNESS_LEVEL, COMPARTMENT_DEFAULT);

		if (water_hardness_func_pos < Regulations__GetTotalSize())
		{
			// Water Hardness function is present in setting file

			uint16 water_for_regenerate;

			// Get the value of water_for_regenerate
			water_for_regenerate = (uint16)Regulations__GetMainValueByPosition(water_hardness_func_pos); // liters
			if (water_for_regenerate > (uint16)0)
			{
				if (((uint8)Regenerate_Nv_Data.Force_Regeneration == (uint8)TRUE) || (DishAuxVar__GetWaterLoadedSinceLastRegen() >= (water_for_regenerate * (uint16)100)))
				{
					execute_regeneration = TRUE;
				}
			}
			else
			{
				// Water Hardness function ACU Value == 0: regeneration is disabled
			}
		}
		else
		{
			// If Water Hardness function is not present in the setting file, skip evaluation of the related condition
			if ((uint8)Regenerate_Nv_Data.Force_Regeneration == (uint8)TRUE)
			{
				execute_regeneration = TRUE;
			}
		}

		if ((uint8)execute_regeneration == (uint8)TRUE)
		{
			BIT_SET(RegenerateAndSaltBitmap, FST_REGENERATION_DONE);

			// Execute "First step" regeneration
			EventStartRegenerate();
		}
		else
		{
			EventSkipRegeneration();
		}

	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void OpenRegenerationValve1Entry(void)
{
	Regenerate_Data.Counter = (uint16)(ENDIANU16_STON(Regenerate_Data.Water_Softener_Params_Ptr->Delay_RV_On_FV_On) * (uint8)TIME_PARAMATERS_RESCALE_FACTOR);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void OpenRegenerationValve1Do(void)
{
	if (Regenerate_Data.Counter > (uint16)0)
	{
		ON_OFF_TYPE reg_valve_state = ON;

		Regenerate_Data.Counter --;

		BIT_SET(RegenerateAndSaltBitmap, BACKRINSE_REQUIRED);

		(void)Hbl__SetLoadRequestByLoadID(HBL_LOAD_REGENERATION_VALVE, HBL_PRIORITY_APPLICATION_HI, (uint8 *)&reg_valve_state);
	}
	else
	{
		EventDelayRvFvOnExpired();
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void OpenRegenerationAndFillValveEntry(void)
{
	Regenerate_Data.Counter = (uint16)(ENDIANU16_STON(Regenerate_Data.Process_Params_Ptr->Reg_Seconds) * (uint8)TIME_PARAMATERS_RESCALE_FACTOR);

	// Reset water to load from Fill Valve in current regeneration
	DishAuxVar__SetLitersCurrentRegeneration(0);

	// Reset Water Counter reference
	// Keep regeneration valve on.
	if ((Regenerate_Data.Counter > (uint16)0) &&
		(DishAuxVar__GetLitersCurrentRegeneration() < (Regenerate_Data.Process_Params_Ptr->Reg_Liters * (uint16)REG_LITERS_TO_CL_CONVERSION_FACTOR)))
	{
		ON_OFF_TYPE reg_valve_state = ON;

		BIT_SET(RegenerateAndSaltBitmap, BACKRINSE_REQUIRED);

		(void)Hbl__SetLoadRequestByLoadID(HBL_LOAD_REGENERATION_VALVE, HBL_PRIORITY_APPLICATION_HI, (uint8 *)&reg_valve_state);
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void OpenRegenerationAndFillValveDo(void)
{
	if ((Regenerate_Data.Counter > (uint16)0) &&
		(DishAuxVar__GetLitersCurrentRegeneration() < (Regenerate_Data.Process_Params_Ptr->Reg_Liters * (uint16)REG_LITERS_TO_CL_CONVERSION_FACTOR)))
	{
		ON_OFF_TYPE reg_valve_state = ON;
		Regenerate_Data.Counter --;

		(void)Hbl__SetLoadRequestByLoadID(HBL_LOAD_REGENERATION_VALVE, HBL_PRIORITY_APPLICATION_HI, (uint8 *)&reg_valve_state);
		(void)Hbl__SetLoadRequestByLoadID(HBL_LOAD_FILL_VALVE, HBL_PRIORITY_APPLICATION_HI, (uint8 *)&reg_valve_state);
	}
	else
	{
		EventRegenerationWaterLoaded();
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void OpenRegenerationValve2Entry(void)
{
	Regenerate_Data.Counter = (uint16)(ENDIANU16_STON(Regenerate_Data.Water_Softener_Params_Ptr->Delay_FV_Off_RV_Off) * (uint8)TIME_PARAMATERS_RESCALE_FACTOR);

	// Keep regeneration valve on.
	if (Regenerate_Data.Counter > (uint16)0)
	{
		ON_OFF_TYPE reg_valve_state = ON;

		BIT_SET(RegenerateAndSaltBitmap, BACKRINSE_REQUIRED);

		(void)Hbl__SetLoadRequestByLoadID(HBL_LOAD_REGENERATION_VALVE, HBL_PRIORITY_APPLICATION_HI, (uint8 *)&reg_valve_state);
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void OpenRegenerationValve2Do(void)
{
	if (Regenerate_Data.Counter > (uint16)0)
	{
		ON_OFF_TYPE reg_valve_state = ON;

		Regenerate_Data.Counter --;

		(void)Hbl__SetLoadRequestByLoadID(HBL_LOAD_REGENERATION_VALVE, HBL_PRIORITY_APPLICATION_HI, (uint8 *)&reg_valve_state);
	}
	else
	{
		EventDelayFvRvOffExpired();
	}
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void RegenerationClosureEntry(void)
{
	// Wait for the double of the Salt_Input_Stability_Time
	Regenerate_Data.Counter = (Regenerate_Data.Water_Softener_Params_Ptr->Salt_Input_Stability_Time * (uint16)2);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 */
static void RegenerationClosureDo(void)
{
	if (Regenerate_Data.Counter > (uint16)0)
	{
		Regenerate_Data.Counter --;
	}

	if ((uint8)Regenerate_Data.Process_Params_Ptr->Last_Step != (uint8)0)
	{
		// Last step Regeneration
		SALT_SENSOR_STATE_TYPE salt_gi_filtered;

		salt_gi_filtered = SaltSensingMonitor__GetSaltGIFiltered();

		// Wait for salt GI to reach a stable value before evaluating it.
		// If it doesn't reach a stable value within the double of the Salt_Input_Stability_Time,
		// use the last stable value.

		if (((SALT_SENSOR_STATE_TYPE)salt_gi_filtered == (SALT_SENSOR_STATE_TYPE)SALT_SENSOR_INVALID_VALUE) && (Regenerate_Data.Counter == (uint16)0))
		{
			salt_gi_filtered = SaltSensingMonitor__GetPrevSaltGIFiltered();
		}

		if ((salt_gi_filtered < SALT_SENSOR_INVALID_VALUE) || (Regenerate_Data.Counter == (uint16)0))
		{
			if (salt_gi_filtered == SALT_NOT_PRESENT)
			{
				if (Regenerate_Nv_Data.Num_Regeneration < (uint8)__UNSIGNED_CHAR_MAX__)
				{
					Regenerate_Nv_Data.Num_Regeneration ++;
				}
			}

			Regenerate_Nv_Data.Force_Regeneration = FALSE;

			// Reset Water_Loaded_Since_Last_Regen
			DishAuxVar__SetWaterLoadedSinceLastRegen(0);

			EventRegenerationProcessCompleted();
		}

	}
	else
	{
		// First step Regeneration
		EventRegenerationProcessCompleted();
	}

}


//=====================================================================================================================
//=== StateEngineCrank (v2.8) Auto-Generated Functions for Regenerate State Machine
//=====================================================================================================================
// !! StateEngineCrank cannot update this code if you change the header !!


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Delay Fv Rv Off Expired Event for the Regenerate state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDelayFvRvOffExpired(void)
{
    switch (Regenerate_Data.State)
    {
        case STATE_REGENERATE_OPEN_REG_VALVE_2:
            Regenerate_Data.State = STATE_REGENERATE_REGENERATION_CLOSURE;
            RegenerationClosureEntry();             // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Regenerate_Data.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Delay Rv Fv On Expired Event for the Regenerate state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventDelayRvFvOnExpired(void)
{
    switch (Regenerate_Data.State)
    {
        case STATE_REGENERATE_OPEN_REG_VALVE_1:
            Regenerate_Data.State = STATE_REGENERATE_OPEN_REG_AND_FILL_VALVE;
            OpenRegenerationAndFillValveEntry();    // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Regenerate_Data.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Regeneration Process Completed Event for the Regenerate state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventRegenerationProcessCompleted(void)
{
    switch (Regenerate_Data.State)
    {
        case STATE_REGENERATE_REGENERATION_CLOSURE:
            Regenerate_Data.State = STATE_REGENERATE_REGENERATION_COMPLETE;
            break;

        default:
            // Event is ignored for all other values of Regenerate_Data.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Regeneration Water Loaded Event for the Regenerate state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventRegenerationWaterLoaded(void)
{
    switch (Regenerate_Data.State)
    {
        case STATE_REGENERATE_OPEN_REG_AND_FILL_VALVE:
            Regenerate_Data.State = STATE_REGENERATE_OPEN_REG_VALVE_2;
            OpenRegenerationValve2Entry();          // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Regenerate_Data.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Restart Closure State Event for the Regenerate state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventRestartClosureState(void)
{
    switch (Regenerate_Data.State)
    {
        case STATE_REGENERATE_REGENERATION_CLOSURE:
            Regenerate_Data.State = STATE_REGENERATE_REGENERATION_CLOSURE;
            RegenerationClosureEntry();             // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Regenerate_Data.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Skip Regeneration Event for the Regenerate state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventSkipRegeneration(void)
{
    switch (Regenerate_Data.State)
    {
        case STATE_REGENERATE_INITIALIZE:
        case STATE_REGENERATE_OPEN_REG_VALVE_1:
        case STATE_REGENERATE_OPEN_REG_AND_FILL_VALVE:
        case STATE_REGENERATE_OPEN_REG_VALVE_2:
            Regenerate_Data.State = STATE_REGENERATE_REGENERATION_COMPLETE;
            break;

        default:
            // Event is ignored for all other values of Regenerate_Data.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Process the Start Regenerate Event for the Regenerate state machine.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void EventStartRegenerate(void)
{
    switch (Regenerate_Data.State)
    {
        case STATE_REGENERATE_INITIALIZE:
            Regenerate_Data.State = STATE_REGENERATE_OPEN_REG_VALVE_1;
            OpenRegenerationValve1Entry();          // Entry Function.
            break;

        default:
            // Event is ignored for all other values of Regenerate_Data.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The main execution function for the Regenerate state machine.
 * This function should be called periodically while the state machine is running.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void RegenerateExecute(void)
{
    switch(Regenerate_Data.State)
    {
        case STATE_REGENERATE_INITIALIZE:
            InitializeDo();                         // Do function.
            break;

        case STATE_REGENERATE_OPEN_REG_VALVE_1:
            OpenRegenerationValve1Do();             // Do function.
            break;

        case STATE_REGENERATE_OPEN_REG_AND_FILL_VALVE:
            OpenRegenerationAndFillValveDo();       // Do function.
            break;

        case STATE_REGENERATE_OPEN_REG_VALVE_2:
            OpenRegenerationValve2Do();             // Do function.
            break;

        case STATE_REGENERATE_REGENERATION_CLOSURE:
            RegenerationClosureDo();                // Do function.
            break;

        default:
            // There is nothing to do for all other values of Regenerate_Data.State.
            break;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * The initialization function for the Regenerate state machine.
 * This function resets the state machine to the initial state.
 *
 *              >>> DO NOT MODIFY THIS FUNCTION <<<
 *
 * This function was auto-generated and must not be edited manually.
 * Change the UML used to generate this function and regenerate the source code.
 */
static void RegenerateInitialize(void)
{
    Regenerate_Data.State = STATE_REGENERATE_INITIALIZE;
}


// !! StateEngineCrank cannot update this code if you change the footer !!
// End of StateEngineCrank Auto-Generated Code.

/**
 *  @file		StabilityLoopControl.c
 *
 *  @brief		It implements the execution of the Stability Control Loop Monitor.
 *
 *  @details
 *
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"
#include "StabilityControlLoopMonitor.h"

// -- Other Modules --
#include "ApplianceMgr.h"
#include "HeatMonitor.h"
#include "Interpreter.h"
#include "Micro.h"
#include "NVData.h"
#include "Selector.h"
#include "SettingFile.h"
#include "SystemTimers.h"
#include "StabilityControl.h"
#include "StatisticsDish.h"
#include <string.h>


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//! Value used to indicate that there is no limit in the number of subcycle executions
#define INVALID_MAX_REACTION_MAX_NUM			(uint8)255


//! A structure to hold non-volatile data for the Stability Control Loop monitor
typedef struct STABILITY_CONTROL_LOOP_NV_DATA_STRUCT
{
	//! Timer 1 of the Stability Loop Control algorithm
	uint16 Timer_1_Counter;

	//! Timer 2 of the Stability Loop Control algorithm
	uint16 Timer_2_Counter;

	//! Incremented each time the heater is turned off due to VWI conditions
	uint16 Relay_Counter;

	//! Incremented each time the Primary Reaction is triggered
	uint8 Primary_Reaction_Counter;

	//! Incremented each time the Secondary Reaction is triggered
	uint8 Secondary_Reaction_Counter;

	//! If TRUE then the primary reaction is skipped and the secondary reaction is triggered
	BOOL_TYPE Switch_To_Secondary_Reaction;
} STABILITY_CONTROL_LOOP_NV_DATA_TYPE;

//! A structure to hold volatile data for the Stability Control Loop monitor
typedef struct STABILITY_CONTROL_LOOP_DATA_STRUCT
{
	//! Pointer to the setting file parameters for the Stability Control (they include Stability Control Loop parameters)
	const STABILITY_CONTROL_COMMON_PARAMS_TYPE *Loop_Parameters_Ptr;

	//! Timer 1 value (in seconds)
	uint16 Timer_1;

	//! Timer 2 value (in seconds)
	uint16 Timer_2;

	//! Status of Heater Allowed in the previous iteration
	BOOL_TYPE VWI_Heater_Allowed_Prev;

	//! The outcome of the Stability Control Loop (if it is requesting a reaction and which one)
	STABILITY_CONTROL_LOOP_OUTCOME_TYPE Outcome;
} STABILITY_CONTROL_LOOP_DATA_TYPE;

#pragma location = "_appliance_segment"
//! The non-volatile data required by the Stability Control Loop monitor.
static STABILITY_CONTROL_LOOP_NV_DATA_TYPE SCL_NV_Data;

//! The volatile data required by the Stability Control Loop monitor.
static STABILITY_CONTROL_LOOP_DATA_TYPE SCL_Data;



//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//----------------------------------------------------------------------------
/**
 * It initializes the Stability Control Loop Monitor and its data, also loading data from setting file.
 *
 * @return None
 *
 */
void StabilityControlLoopMonitor__Initialize(void)
{
	SETTINGFILE_LOADER_TYPE loader;

	if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
	{
		if (NVData__GetLastRestoreStatus()->Bit.Appliance_Data_Recovered != (NVDATA_RESTORE_TYPE)NVDATA_RESTORE_OKAY)
		{
			memset(&SCL_NV_Data, 0, sizeof(SCL_NV_Data));
		}

		SCL_Data.Timer_1 = SCL_NV_Data.Timer_1_Counter/(uint16)10;
		SCL_Data.Timer_2 = SCL_NV_Data.Timer_2_Counter/(uint16)10;
		SCL_Data.VWI_Heater_Allowed_Prev = FALSE;
		SCL_Data.Outcome = SCL_NO_REACTION_REQUESTED;

		SCL_Data.Loop_Parameters_Ptr = (const STABILITY_CONTROL_COMMON_PARAMS_TYPE *)NULL;

		// Get the Heat Monitor process parameters from the setting file, to read the heater's type.
		if (SettingFile__BasicLoader(SF_PTR_ACU_PRODUCT_CONFIGURATION_DATA,	DISPL_HEATER_DEACTIVATION_CONDITIONS_PARAM, &loader) == PASS)
		{
			if (((HEAT_MONITOR_INSTANCE_TYPE *)(void*)loader.Data)->Heater_Type == (uint8)INTEGRATED_HEATER)
			{
				// Integrated Heater type

				// Get the required parameters from the setting file.

				if (SettingFile__BasicLoader(SF_PTR_ACU_ALGORITHM_PARAMETERS, DISPL_STABILITY_CONTROL, &loader) == PASS)
				{
					if (loader.Length >= (uint16)20)
					{
						if ((((STABILITY_CONTROL_COMMON_PARAMS_TYPE *)(void*)loader.Data)->Timer_1_Threshold != (uint8)0) ||
							(((STABILITY_CONTROL_COMMON_PARAMS_TYPE *)(void*)loader.Data)->Relay_Counter_Threshold != (uint8)0))
						{
							SCL_Data.Loop_Parameters_Ptr = (const STABILITY_CONTROL_COMMON_PARAMS_TYPE *)(void*)(loader.Data);
						}
						else
						{
							// Timer_1_Threshold == 0 && Relay_Counter_Threshold == 0: the algorithm is disabled.
						}
					}
					else
					{
						// Old setting files: the parameters are not present in setting file, the algorithm is disabled
					}
				}
				else
				{
					// Parameters missing, the algorithm is disabled
				}
			}
			else
			{
				// Calrod heater type, the algorithm is disabled
			}
		}
	}
}


//----------------------------------------------------------------------------
/**
 * It implements the execution of the Stability Control Loop Monitor.
 *
 * @return None
 *
 */
void StabilityControlLoopMonitor__Execution(void)
{
	if (SCL_Data.Loop_Parameters_Ptr != NULL)
	{
		CYCLE_STATE_TYPE cycle_state;
		CYCLE_NAME_TYPE subcycle_name;
		BOOL_TYPE cycle_is_paused;
		static BOOL_TYPE cycle_was_paused = FALSE;

		// Check if cycle has to be considered paused or not
		cycle_state = Selector__GetCycleState(Interpreter__GetCurrentSFIndex(COMPARTMENT_DEFAULT),
														 COMPARTMENT_DEFAULT,
														 Interpreter__GetCurrentStatement(COMPARTMENT_DEFAULT).Cycle);

		subcycle_name = CYCLE_NAME_INVALID;
		if ((uint8)Interpreter__IsSubCycleRunning(COMPARTMENT_DEFAULT) == (uint8)TRUE)
		{
			subcycle_name = Selector__GetCycleNameId(Interpreter__GetCurrentSFIndex(COMPARTMENT_DEFAULT),
														COMPARTMENT_DEFAULT,
														Interpreter__GetStatus().Sub_Cycle[COMPARTMENT_DEFAULT]);
		}

		if ((cycle_state == CYCLE_STATE_PAUSE) || (subcycle_name == CYCLE_NAME_SPECIFIC_CYCLE_RESUME))
		{
			cycle_is_paused = TRUE;
			cycle_was_paused = TRUE;
		}
		else
		{
			cycle_is_paused = FALSE;
			if ((cycle_state == CYCLE_STATE_RUN) && ((uint8)cycle_was_paused == (uint8)TRUE))
			{
				// If it is the first time in RUN after a PAUSE/CYCLE_RESUME state, the cycle is still
				// considered paused, to avoid Heater_Requested to be seen as FALSE due to the transient.
				cycle_is_paused = TRUE;
			}
			cycle_was_paused = FALSE;
		}

		if ((uint8)ApplianceMgr__IsStable() == (uint8)TRUE)
		{
			STABILITY_CONTROL_LOOP_OUTCOME_TYPE temporary_outcome = SCL_NO_REACTION_REQUESTED;

			if (StabilityControl__GetStatus() == ACTIVE)
			{
				if (cycle_is_paused == FALSE)
				{
					// Counters update
					if ((uint8)HeatMonitor__IsHeaterRequested() == (uint8)TRUE)
					{
						if ((uint8)HeatMonitor__IsHeaterAllowed() == (uint8)TRUE)
						{
							if ((uint8)HeatMonitor__IsHeaterVWIAllowed() == (uint8)TRUE)
							{
								// Timer 1 paused, but value kept

								// Update Timer 2
								if (SCL_NV_Data.Timer_2_Counter < (uint16)__UNSIGNED_SHORT_MAX__)
								{
									SCL_NV_Data.Timer_2_Counter ++;
								}
							}
							else
							{
								// Update Timer 1
								if (SCL_Data.Loop_Parameters_Ptr->Timer_1_Threshold != (uint8)0)
								{
									if (SCL_NV_Data.Timer_1_Counter < (uint16)__UNSIGNED_SHORT_MAX__)
									{
										SCL_NV_Data.Timer_1_Counter ++;
									}
								}
								else
								{
									// Timer_1_Threshold == 0, the related part of the algorithm is disabled
								}

								// Reset Timer 2
								SCL_NV_Data.Timer_2_Counter = 0;

								// Check VWI_Heater_Allowed transitions
								if ((uint8)SCL_Data.VWI_Heater_Allowed_Prev == (uint8)TRUE)
								{
									if (SCL_Data.Loop_Parameters_Ptr->Relay_Counter_Threshold != (uint8)0)
									{
										if (SCL_NV_Data.Relay_Counter < (uint16)__UNSIGNED_SHORT_MAX__)
										{
											SCL_NV_Data.Relay_Counter ++;
										}
									}
									else
									{
										// Relay_Counter_Threshold == 0, the related part of the algorithm is disabled
									}
								}
							}
						}
						else
						{
							// Timer 1 paused, but value kept

							// Reset Timer 2
							SCL_NV_Data.Timer_2_Counter = 0;
						}
					}
					else
					{
						StabilityControlLoopMonitor__ResetCounters();
					}
				}
				else
				{
					// Cycle is paused, all counters are frozen.
				}

				// Reaction triggering
				if (((SCL_Data.Loop_Parameters_Ptr->Timer_1_Threshold != (uint8)0) && (SCL_NV_Data.Timer_1_Counter >= ((uint16)SCL_Data.Loop_Parameters_Ptr->Timer_1_Threshold * (uint16)100))) ||
					((SCL_Data.Loop_Parameters_Ptr->Relay_Counter_Threshold != (uint8)0) && (SCL_NV_Data.Relay_Counter > SCL_Data.Loop_Parameters_Ptr->Relay_Counter_Threshold)))
				{
					// It's time to trigger a reaction
					STABILITY_CONTROL_REACTIONS_TYPE direct_primary_reaction;

					direct_primary_reaction = StabilityControl__GetLDPrimaryReaction();

					if (direct_primary_reaction != SC_REACTION_NONE)
					{
						STABILITY_CONTROL_REACTIONS_TYPE indirect_primary_reaction;

						indirect_primary_reaction = direct_primary_reaction;
						StabilityControl__UpdateReaction(&indirect_primary_reaction);

						if (indirect_primary_reaction == SC_REACTION_NONE)
						{
							// Primary Reaction is enabled but can't be executed (eg: Primary Reaction == Add Water but the maximum
							// amount of water has already been loaded, or Primary Reaction == Filter Plugged but the maximum number of
							// subcycle executions has already been reached): the Secondary Reaction shall be immediately executed.
							SCL_NV_Data.Switch_To_Secondary_Reaction = TRUE;
						}
					}

					if ((SCL_Data.Loop_Parameters_Ptr->Primary_Reaction_Counter_Threshold != INVALID_MAX_REACTION_MAX_NUM) &&
						(SCL_NV_Data.Primary_Reaction_Counter >= SCL_Data.Loop_Parameters_Ptr->Primary_Reaction_Counter_Threshold))
					{
						// Maximum number of Primary Reaction activations exceeded: the Secondary Reaction shall be immediately executed.
						SCL_NV_Data.Switch_To_Secondary_Reaction = TRUE;
					}

					if ((uint8)SCL_NV_Data.Switch_To_Secondary_Reaction == (uint8)TRUE)
					{
						if (((SCL_NV_Data.Secondary_Reaction_Counter < SCL_Data.Loop_Parameters_Ptr->Secondary_Reaction_Counter_Threshold) ||
							(SCL_Data.Loop_Parameters_Ptr->Secondary_Reaction_Counter_Threshold == INVALID_MAX_REACTION_MAX_NUM)) &&
							(StabilityControl__GetLDSecondaryReaction() != SC_REACTION_NONE))
						{
							// Trigger secondary reaction
							temporary_outcome = SCL_SECONDARY_REACTION_REQUESTED;

							if (SCL_NV_Data.Secondary_Reaction_Counter < (uint16)__UNSIGNED_CHAR_MAX__)
							{
								SCL_NV_Data.Secondary_Reaction_Counter ++;
							}

							// Update flag for cycle statistics
							StatisticsDish__SetSCLReactionTriggered();

							// Update lifetime statistics
							StatisticsDish__IncreaseSCLSecondaryReactionCounter();
						}
						else
						{
							// Secondary reaction disabled or maximum number of activations exceeded
						}
					}
					else
					{
						if (direct_primary_reaction != SC_REACTION_NONE)
						{
							// Trigger Primary reaction
							temporary_outcome = SCL_PRIMARY_REACTION_REQUESTED;

							if (SCL_NV_Data.Primary_Reaction_Counter < (uint16)__UNSIGNED_CHAR_MAX__)
							{
								SCL_NV_Data.Primary_Reaction_Counter ++;
							}

							// Update lifetime statistics
							StatisticsDish__IncreaseSCLPrimaryReactionCounter();
						}
						else
						{
							// Primary Reaction disabled, next time Secondary Reaction will be triggered

							SCL_NV_Data.Switch_To_Secondary_Reaction = TRUE;
						}

						// Update flag for cycle statistics
						StatisticsDish__SetSCLReactionTriggered();
					}

					// Reset counters anyway
					StabilityControlLoopMonitor__ResetCounters();

					// Mark the Loop reaction as triggered
					StabilityControl__SetFlagsMask((uint8)1 << SC_VWI_lOOP_DETECTED);
				}
				else if (SCL_NV_Data.Timer_2_Counter >= (SCL_Data.Loop_Parameters_Ptr->Timer_2_Threshold * (uint16)100))
				{
					StabilityControlLoopMonitor__ResetCounters();
				}
			}
			else
			{
				// Stability Control inactive, stop the algorithm and reset all the counters
				StabilityControlLoopMonitor__ResetCounters();

				SCL_NV_Data.Primary_Reaction_Counter = 0;
				SCL_NV_Data.Secondary_Reaction_Counter = 0;
				SCL_NV_Data.Switch_To_Secondary_Reaction = FALSE;
			}

			// Update timers in seconds
			SCL_Data.Timer_1 = SCL_NV_Data.Timer_1_Counter/(uint16)10;
			SCL_Data.Timer_2 = SCL_NV_Data.Timer_2_Counter/(uint16)10;

			// Align VWI_Heater_Allowed_Prev
			SCL_Data.VWI_Heater_Allowed_Prev = HeatMonitor__IsHeaterVWIAllowed();

			// Update SCL_Reaction_Requested
			SCL_Data.Outcome = temporary_outcome;
		}
		else
		{
			// Freeze all variables and decisions until Supervisor has been executed for the first time
		}
	}
	else
	{
		// Algorithm is disabled
	}
}


/**
 * It returns the value of SCL_Data.Outcome.
 *
 * @return SCL_Outcome (SCL_NO_REACTION_REQUESTED/SCL_PRIMARY_REACTION_REQUESTED/SCL_SECONDARY_REACTION_REQUESTED)
 */
STABILITY_CONTROL_LOOP_OUTCOME_TYPE StabilityControlLoopMonitor__GetOutcome(void)
{
	return SCL_Data.Outcome;
}


/**
 * It resets SCL_NV_Data.Timer_1_Counter, SCL_NV_Data.Timer_2_Counter and SCL_NV_Data.Relay_Counter
 *
 * @return none
 */
void StabilityControlLoopMonitor__ResetCounters(void)
{
	SCL_NV_Data.Timer_1_Counter = 0;
	SCL_NV_Data.Timer_2_Counter = 0;
	SCL_NV_Data.Relay_Counter = 0;
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================




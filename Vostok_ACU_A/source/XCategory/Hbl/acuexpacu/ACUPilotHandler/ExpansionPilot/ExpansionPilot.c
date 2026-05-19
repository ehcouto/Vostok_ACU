/**
 *  @file       
 *
 *  @brief      Implements ExpansionPilot module.
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"
#include "ExpansionPilot.h" 

#include <string.h>

#if (HBL_PILOT_NUM_EXPANSION > 0)
#include "API012Exp.h"

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//! Tracks the number of expansion  pilot allocations.
static uint8 Expansion_Pilot_Allocation;

//! Expansion pilot parameters type definition.
typedef struct EXPANSION_PILOT_DATA_STRUCT
{
    uint8 Requested;
    uint8 Processed;
} EXPANSION_PILOT_DATA_TYPE;

//! Expansion pilot data array. One entry per expansion pilot instance.
static EXPANSION_PILOT_DATA_TYPE Expansion_Pilot_Data[HBL_PILOT_NUM_EXPANSION];

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static uint8 GetExpansionLoadIndex(HBL_LOAD_TYPE *load);

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief       It Initializes the module ExpansionPilot and its variables.
 */
void ExpansionPilot__Initialize(void)
{
    Expansion_Pilot_Allocation = 0;
    memset(&Expansion_Pilot_Data, 0, sizeof(Expansion_Pilot_Data));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Allocate instance for expansion pilot.
 *
 *  @return     uint8 retval - Allocation result.
 *  @retval     0-254 - Pilot allocation instance.
 *  @retval     0xFF - Allocation failed.
 */
uint8 ExpansionPilot__Allocate(void)
{
    uint8 retval;

    retval = 0xFF;

    if (Expansion_Pilot_Allocation < HBL_PILOT_NUM_EXPANSION)
    {
        retval = Expansion_Pilot_Allocation;
        Expansion_Pilot_Allocation++;
    }

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Initialize expansion pilot instance.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated expansion load instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of expansion pilot instance initialization.
 *  @retval     PASS - Instance initialization succeeded.
 *  @retval     FAIL - Instance initialization failed.
 */
PASS_FAIL_TYPE ExpansionPilot__InitializeInstance(HBL_LOAD_TYPE *load)
{
    PASS_FAIL_TYPE result;

    result = FAIL;

    if (load != NULL)
    {
        if (load->Pilot_Data_Index < Expansion_Pilot_Allocation)
        {
            result = PASS;
        }
    }

    return (result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
  *  @brief     Set expansion pilot instance request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated expansion load instance.
 *
 *  @param      void *data - Pointer to request data. Pass argument as uint8 *.
 *
 *  @return     PASS_FAIL_TYPE result - Result of expansion pilot request.
 *  @retval     PASS - Instance request succeeded.
 *  @retval     FAIL - Instance request failed.
*/
PASS_FAIL_TYPE ExpansionPilot__Request(HBL_LOAD_TYPE *load, void *data)
{
    PASS_FAIL_TYPE result;

    result = FAIL;

    if ((load != NULL) && (data != NULL))
    {
        if (load->Pilot_Data_Index < Expansion_Pilot_Allocation)
        {
            Expansion_Pilot_Data[load->Pilot_Data_Index].Requested = *((uint8 *)data);
            result = PASS;
        }
    }

    return (result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Process expansion pilot instance.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to expansion load instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of expansion pilot instance process.
 *  @retval     PASS - Instance process succeeded.
 *  @retval     FAIL - Instance process failed.
 */
PASS_FAIL_TYPE ExpansionPilot__Process(HBL_LOAD_TYPE *load)
{
    PASS_FAIL_TYPE result;
    uint8 expansion_load_index;

    result = FAIL;

    if (load != NULL)
    {
        if (load->Pilot_Data_Index < Expansion_Pilot_Allocation)
        {
            Expansion_Pilot_Data[load->Pilot_Data_Index].Processed =
                Expansion_Pilot_Data[load->Pilot_Data_Index].Requested;

            expansion_load_index = GetExpansionLoadIndex(load);
            API012Exp__SetExpansionLoad(expansion_load_index, Expansion_Pilot_Data[load->Pilot_Data_Index].Processed);

            result = PASS;
        }
    }

    return (result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get current expansion pilot instance request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated expansion load instance.
 *
 *  @return     void *requested - Requested instance data.
 *  @retval     Address of requested instance data. Cast it using *((uint8 *)ptr) to get values.
 *  @retval     NULL - Invalid load index.
 */
void * ExpansionPilot__GetRequested(HBL_LOAD_TYPE *load)
{
    void *requested;

    requested = NULL;

    if (load != NULL)
    {
        if (load->Pilot_Data_Index < Expansion_Pilot_Allocation)
        {
            requested = &(Expansion_Pilot_Data[load->Pilot_Data_Index].Requested);
        }
    }

    return (requested);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get expansion pilot instance processed request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated expansion load instance.
 *
 *  @return     void *processed - Processed instance request.
 *  @retval     Address of processed instance request. Cast it using *((uint8 *)ptr) to get values.
 *  @retval     NULL - Invalid load index.
 */
void * ExpansionPilot__GetProcessed(HBL_LOAD_TYPE *load)
{
    void *processed;

    processed = NULL;

    if (load != NULL)
    {
        if (load->Pilot_Data_Index < Expansion_Pilot_Allocation)
        {
            processed =  &(Expansion_Pilot_Data[load->Pilot_Data_Index].Processed);
        }
    }

    return (processed);
}

void * ExpansionPilot__GetStatus(HBL_LOAD_TYPE *load)
{
    void *status;
    uint8 *commLoads;
    uint8 expansion_load_index;

    status = NULL;

    if (load != NULL)
    {
        if (load->Pilot_Data_Index < Expansion_Pilot_Allocation)
        {
            commLoads = API012Exp__GetExpansionLoadsStatus();
            expansion_load_index = GetExpansionLoadIndex(load);
            status = &commLoads[expansion_load_index];
        }
    }

    return (status);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get the expansion load index for the external load.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to ice dispenser door load instance.
 *
 *  @return     uint8 expansion_load_index - value from setting file for load position in expansion ACU loads buffer.
 */
static uint8 GetExpansionLoadIndex(HBL_LOAD_TYPE *load)
{
    PASS_FAIL_TYPE load_success;
    uint8 expansion_load_index;
    SETTINGFILE_LOADER_TYPE sf_loader_data;

    expansion_load_index = 0;
    load_success = SettingFile__BasicLoader(SF_PTR_ACU_IO_DEVICE,
                                            load->Pilot_Type_Params_Offset,
                                            &sf_loader_data);
    if (load_success == PASS)
    {
        if (sf_loader_data.Length > 0)
        {
            expansion_load_index = sf_loader_data.Data[0];
        }
    }

    return (expansion_load_index);
}

#endif /* (HBL_PILOT_NUM_EXPANSION > 0) */

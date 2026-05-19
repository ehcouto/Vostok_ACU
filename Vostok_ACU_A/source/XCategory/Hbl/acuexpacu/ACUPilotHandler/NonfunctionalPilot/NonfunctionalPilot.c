/**
 *  @file
 *
 *  @brief      Implements the Nonfunctional pilot module.
 *
 *  @details    Pilot for Virtual Loads. Satisfies HBL API. Does not control any physical outputs.
 *              Allows API 012 to send a load command that does not drive a physical output.
 *              The status of the load can be used on an expansion board to enable/disable complex loads (e.g. I2C).
 *
 *  @copyright  Copyright 2018 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */

//-------------------------------------- Include Files ----------------------------------------------------------------

//lint -e766 Suppress Info: header not used in module 'VirtualGI.c'
#include "SystemConfig.h"

#if (HBL_PILOT_NUM_NONFUNCTIONAL > 0)
#include "NonfunctionalPilot.h"
#include <string.h>

//  --- Private Declarations ------------------------------------------------------------------------------------------
//! The structure that holds all the variables used by this module.
typedef struct NONFUNCTIONAL_PILOT_DATA_STRUCT
{
    uint8 Requested[HBL_PILOT_NUM_NONFUNCTIONAL];
    uint8 Processed[HBL_PILOT_NUM_NONFUNCTIONAL];

    //! Tracks the number of Virtual GI instances.
    uint8 Allocation;
} NONFUNCTIONAL_PILOT_DATA_TYPE;

// -- Private Variable Definitions ------------------------------------------------------------------------------------
//! Define storage for module data.
static NONFUNCTIONAL_PILOT_DATA_TYPE Nonfunctional_Data;

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module NonfunctionalPilot and its variables
 */
void NonfunctionalPilot__Initialize(void)
{
    memset(&Nonfunctional_Data, 0, sizeof(Nonfunctional_Data));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Allocate Nonfunctional pilot instance.
 *
 *  @return     uint8 result - Allocation result.
 *  @retval     0-254 - Pilot allocation instance.
 *  @retval     0xFF - Allocation failed.
 */
uint8 NonfunctionalPilot__Allocate(void)
{
    uint8 result;

    result = HBL_INDEX_INVALID ;

    if (Nonfunctional_Data.Allocation < HBL_PILOT_NUM_NONFUNCTIONAL)
    {
        result = Nonfunctional_Data.Allocation;
        Nonfunctional_Data.Allocation++;
    }

    return (result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Initialize Nonfunctional pilot instance.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated virtual load instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of Nonfunctional pilot instance initialization.
 *  @retval     PASS - Instance initialization succeeded.
 *  @retval     FAIL - Instance initialization failed.
 */
PASS_FAIL_TYPE NonfunctionalPilot__InitializeInstance(HBL_LOAD_TYPE *load)
{
    if (load->Pilot_Data_Index < Nonfunctional_Data.Allocation)
    {
        return (PASS);
    }
    return (FAIL);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Set Nonfunctional pilot instance request.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated Virtual load instance.
 *
 *  @param      void *data - Pointer to request data. Not used.
 *
 *  @return     PASS_FAIL_TYPE result - Result of Nonfunctional pilot request.
 *  @retval     PASS - Request instance is allocated.
 *  @retval     FAIL - Request instance is not allocated.
 */
PASS_FAIL_TYPE NonfunctionalPilot__Request(HBL_LOAD_TYPE *load, void *data)
{
    PASS_FAIL_TYPE result = FAIL;

    if (load->Pilot_Data_Index < Nonfunctional_Data.Allocation)
    {
        Nonfunctional_Data.Requested[load->Pilot_Data_Index] = *(uint8 *)data;
        result = PASS;
    }

    return (result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Process Nonfunctional pilot instance.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated Virtual load instance.
 *
 *  @return     PASS_FAIL_TYPE result - Result of Nonfunctional pilot instance process.
 *  @retval     PASS - Request instance is allocated.
 *  @retval     FAIL - Request instance is not allocated.
 */
PASS_FAIL_TYPE NonfunctionalPilot__Process(HBL_LOAD_TYPE *load)
{
    PASS_FAIL_TYPE result = FAIL;

    if (load->Pilot_Data_Index < Nonfunctional_Data.Allocation)
    {
        Nonfunctional_Data.Processed[load->Pilot_Data_Index] = Nonfunctional_Data.Requested[load->Pilot_Data_Index];
        result = PASS;
    }

    return (result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get the number of Nonfunctional pilot instances.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated Virtual load instance.
 *
 *  @return     void * requested - Returns the number of allocated instances.
 *  @retval     Address of allocation tracking value. Cast it using *((uint8 *)ptr) to get value.
 *  @retval     NULL - Invalid load index.
 */
void * NonfunctionalPilot__GetRequested(HBL_LOAD_TYPE *load)
{
    void *requested = NULL;

    if (load->Load_Data_Index < Nonfunctional_Data.Allocation)
    {
        requested = &(Nonfunctional_Data.Requested[load->Pilot_Data_Index]);
    }

    return (requested);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get the number of Nonfunctional pilot instances.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to associated Virtual load instance.
 *
 *  @return     void * processed - Returns the number of allocated instances.
 *  @retval     Address of allocation tracking value. Cast it using *((uint8 *)ptr) to get value.
 *  @retval     NULL - Invalid load index.
 */
void * NonfunctionalPilot__GetProcessed(HBL_LOAD_TYPE *load)
{
    void *processed = NULL;

    if (load->Pilot_Data_Index < Nonfunctional_Data.Allocation)
    {
        processed = &(Nonfunctional_Data.Processed[load->Pilot_Data_Index]);
    }

    return (processed);
}

#endif // (HBL_PILOT_NUM_NONFUNCTIONAL > 0)

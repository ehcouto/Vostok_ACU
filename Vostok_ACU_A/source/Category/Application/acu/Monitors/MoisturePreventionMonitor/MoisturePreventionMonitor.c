/*
 * MoisturePreventionMonitor.c
 *
 *  Created on: 07.05.2018
 *      Author: WAZNYP
 */

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "MoisturePreventionMonitor.h"
#include "DoorMonitor.h"
#include "Hbl.h"
#include "NVData.h"
#include "Micro.h"
#include "Regulations.h"
#include "Interpreter.h"
#include "Selector.h"
#include "String.h"
#include "Fault.h"

typedef enum OUTPUT_STATE_ENUM
{
    OUTPUT_STATE_OFF = 0,
    OUTPUT_STATE_ON
}OUTPUT_STATE_TYPE;

typedef enum MP_ACTIVATED_ENUM
{
    MP_ACTIVATED_IN_OUT_OF_CYCLE = 0,
    MP_ACTIVATED_IN_CYCLE,
}MP_ACTIVATED_TYPE;


//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#define NUMBER_OF_CALLS_PER_SECOND  (uint8)10

#define KOSHER_OPTION_OFF    		1

static uint8 Door_Time;
// todo: consider removing MP_Step and using (SettingFileStructure.NumberOfSlots - MoisturePrevention_Steps_Left) instead
static uint8 MP_Step;
static uint8 Tick;
static BOOL_TYPE MP_Test_POR_Time;
static MP_ACTIVATED_TYPE MP_Activation_Mode; //Variable which differentiate if MP was activated inside cycle or by external cycle macro

// Need to save data on a black out event
#pragma location = "_appliance_segment"
static MoisturePreventionSettingFileStructureType SettingFileStructure;
#pragma location = "_appliance_segment"
static MP_STATUS_TYPE MoisturePreventionMonitorActive;
#pragma location = "_appliance_segment"
static uint8 MoisturePrevention_Steps_Left;
#pragma location = "_appliance_segment"
static uint16 MoisturePrevention_Seconds_Left;
#pragma location = "_appliance_segment"
static MP_PAUSE_SOURCE_TYPE MoisturePrevention_Pause_Source;

static uint8 Kosher_Position;
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
uint16 GetSlotSeconds(uint8 slot_number);
uint16 GetSlotDurationSec(SLOT_DURATION_IDX_TYPE slot_duration_idx);
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================



//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module FillValveStuckOn and its variables
 *
 */
void MoisturePreventionMonitor__Initialize(void)
{
    if (NVData__GetLastRestoreStatus()->Bit.Appliance_Data_Recovered != (NVDATA_RESTORE_TYPE)NVDATA_RESTORE_OKAY)
    {
        MoisturePreventionMonitorActive = MP_DEACTIVATED;
        MoisturePrevention_Pause_Source = MP_PAUSE_INACTIVE;
        MoisturePrevention_Steps_Left = 0;
    }
    if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
    {
        MP_Test_POR_Time = TRUE;
    }
    Tick = 0;
    Door_Time = 50;
    Kosher_Position = Regulations__GetPositionByFunctionID(FUNC_ID_KOSHER_FRIENDLY, 0);
    
    MP_Activation_Mode = MP_ACTIVATED_IN_OUT_OF_CYCLE;
}


void MoisturePreventionMonitor__Execution(void)
{
    DOOR_STATUS_TYPE door_status;
    door_status = DoorMonitor__GetDoorStatus();


    //This is to manage situation where MP is active and loads are activated but FillValveStuckOn
    // module is trying to deactivate them. used for END state and Programming because
    // we cannot start subcycle on prefault and manage this in that way
    if((FAULT_STATE_PRE_FAULT == Fault__GetState(FAULT_ID_FILL_VALVE_TRIAC_SHORTED)) ||
       (FAULT_STATE_PRE_FAULT == Fault__GetState(FAULT_ID_FILL_VALVE_STUCK_ON)))
    {
        if(MoisturePreventionMonitorActive == MP_ACTIVATED)
        {
            MoisturePreventionMonitor__Pause(MP_PAUSE_FILLVALVE_PREFAULT);
        }
    }
    else if((Fault__GetState(FAULT_ID_FILL_VALVE_TRIAC_SHORTED) < FAULT_STATE_PRE_FAULT) &&
            (Fault__GetState(FAULT_ID_FILL_VALVE_STUCK_ON) < FAULT_STATE_PRE_FAULT))
    {

        MoisturePreventionMonitor__Resume(MP_PAUSE_FILLVALVE_PREFAULT);
    }



    if (Door_Time == (uint8)0)
    {
        MoisturePreventionMonitorActive = MP_DEACTIVATED;
    }
    else
    {
        Door_Time--;
    }

    if (DOOR_STATUS_CLOSE == door_status)
    {
        Door_Time = 50;
    }


    if ((uint8)MP_Test_POR_Time == (uint8)TRUE)
    {
        uint8 por_input_gi_index;
        uint8 por_input_sequence_id;
        BOOL_TYPE wait_por_input;
        POR_TIME_STATUS_TYPE last_por_time_status;

        last_por_time_status = POR_TIME_STATUS_LONG;

        por_input_gi_index = Hbl__GetGIIndexByGIID(HBL_GI_ENTRAPMENT_INPUT_POR);
        if (por_input_gi_index != (uint8)HBL_INDEX_INVALID)
        {
            por_input_sequence_id = Hbl__GetGISequenceIDByIndex(por_input_gi_index);

            if (por_input_sequence_id > (uint8)0)
            {
                wait_por_input = FALSE;
                last_por_time_status = (POR_TIME_STATUS_TYPE)Hbl__GetGIDataConvertedByIndex(por_input_gi_index);
            }
            else
            {
                wait_por_input = TRUE;
            }
        }
        else
        {
            wait_por_input = FALSE;
        }

        if (wait_por_input == FALSE)
        {
            if (last_por_time_status == POR_TIME_STATUS_LONG)
            {
                MoisturePreventionMonitorActive = MP_DEACTIVATED;
            }
            MP_Test_POR_Time = FALSE;
        }
    }

    if (Kosher_Position != (uint8)REGULATION_INVALID)
    {
        if (Regulations__GetIndexValueByPosition(Kosher_Position) > (uint8)KOSHER_OPTION_OFF)
        {
            MoisturePreventionMonitorActive = MP_DEACTIVATED;
        }
    }

    if (MoisturePreventionMonitorActive == MP_ACTIVATED)
    {
        OUTPUT_STATE_TYPE output_state;

        if ((MoisturePrevention_Seconds_Left > 0U) && (DOOR_STATUS_CLOSE == door_status))
        {
            Tick++;
            if (Tick >= NUMBER_OF_CALLS_PER_SECOND)
            {
                MoisturePrevention_Seconds_Left--;
                Tick = 0;
            }
        }
        else if (MoisturePrevention_Seconds_Left == 0U)
        {
            MP_Step++;
            MoisturePrevention_Steps_Left--;
            MoisturePrevention_Seconds_Left = GetSlotSeconds(MP_Step);
        }
        else
        {
            // nothing to do
        }

        if (((uint8)0 == MoisturePrevention_Steps_Left) && (0U == MoisturePrevention_Seconds_Left))
        {
            MoisturePreventionMonitorActive = MP_DEACTIVATED;
        }
        // in EOC, door open should OFF the FAN
        if ( (((SettingFileStructure.FanStatusBitMap >> MP_Step) & 0x0001U) != 0U) && (DOOR_STATUS_CLOSE == door_status))
        {
            output_state = OUTPUT_STATE_ON;
        }
        else
        {
            output_state = OUTPUT_STATE_OFF;
        }
        (void)Hbl__SetLoadRequestByLoadID(HBL_LOAD_DRY_FAN, HBL_PRIORITY_APPLICATION_HI, (uint8 *) &output_state);

        if (((SettingFileStructure.VentStatusBitMap >> MP_Step) & 0x0001U) != 0U)
        {
            output_state = OUTPUT_STATE_ON;
        }
        else
        {
            output_state = OUTPUT_STATE_OFF;
        }
        (void)Hbl__SetLoadRequestByLoadID(HBL_LOAD_VENT, HBL_PRIORITY_APPLICATION_HI, (uint8 *) &output_state);

    }

}

void MoisturePreventionMonitor__Activate(MoisturePreventionSettingFileStructureType* SF)
{
    CYCLE_PHASE_STATEMENT_TYPE cps = Interpreter__GetCurrentStatement(COMPARTMENT_DEFAULT);
    CYCLE_STATE_TYPE label = (Selector__GetCycleState(0, COMPARTMENT_DEFAULT, cps.Cycle));

    if(label == CYCLE_STATE_RUN)
    {
        MP_Activation_Mode = MP_ACTIVATED_IN_CYCLE;
    }
    else
    {
        MP_Activation_Mode = MP_ACTIVATED_IN_OUT_OF_CYCLE;
    }

    SettingFileStructure.SlotDuration = SF->SlotDuration;
    SettingFileStructure.SlotDurationSplitted = SF->SlotDurationSplitted;
    SettingFileStructure.NumberOfSlots = SF->NumberOfSlots;
    SettingFileStructure.FanOFFSpeed = SF->FanOFFSpeed;
    SettingFileStructure.FanONSpeed = SF->FanONSpeed;
    SettingFileStructure.VentStatusBitMap = SF->VentStatusBitMap;
    SettingFileStructure.FanStatusBitMap = SF->FanStatusBitMap;

    memcpy(&SettingFileStructure.SlotDurationBank[0], &SF->SlotDurationBank[0], sizeof(SettingFileStructure.SlotDurationBank));

    Door_Time = 50;
    MP_Step = 0;
    MoisturePrevention_Steps_Left = SettingFileStructure.NumberOfSlots;
    MoisturePrevention_Seconds_Left = GetSlotSeconds(MP_Step);
    Tick = 0;
    MoisturePreventionMonitorActive = MP_ACTIVATED;
}

void MoisturePreventionMonitor__Deactivate(void)
{
    CYCLE_PHASE_STATEMENT_TYPE cps = Interpreter__GetCurrentStatement(COMPARTMENT_DEFAULT);
    CYCLE_STATE_TYPE label = (Selector__GetCycleState(0, COMPARTMENT_DEFAULT, cps.Cycle));

    if (MP_Activation_Mode == MP_ACTIVATED_IN_CYCLE) //If MP was activated in cycle we want to stop it all the time
    {
        MoisturePreventionMonitorActive = MP_DEACTIVATED;
    }
    else if((MP_Activation_Mode == MP_ACTIVATED_IN_OUT_OF_CYCLE) &&
            (label != CYCLE_STATE_PROGRAMMING))
    {
        MoisturePreventionMonitorActive = MP_DEACTIVATED; //short door opening in END state should not deactivate MP
    }
}

void MoisturePreventionMonitor__Pause(MP_PAUSE_SOURCE_TYPE source)
{

    if(MoisturePrevention_Pause_Source == MP_PAUSE_INACTIVE)
    {
        MoisturePrevention_Pause_Source = source;
    }
    else
    {
        //If another source have a higher priority we need to allow to overide
        if(MoisturePrevention_Pause_Source < source)
        {
            MoisturePrevention_Pause_Source = source;
        }
    }

    if (MoisturePreventionMonitorActive == MP_ACTIVATED)
    {
        MoisturePreventionMonitorActive = MP_PAUSED;
    }
}

MP_STATUS_TYPE MoisturePreventionMonitor__GetStatus(void)
{
    return MoisturePreventionMonitorActive;
}

void MoisturePreventionMonitor__CycleStarted(void)
{
    MoisturePreventionMonitorActive = MP_DEACTIVATED;
}

void MoisturePreventionMonitor__Resume(MP_PAUSE_SOURCE_TYPE source)
{
    if(MoisturePrevention_Pause_Source == source)
    {
        if (MoisturePreventionMonitorActive == MP_PAUSED)
        {
            MoisturePreventionMonitorActive = MP_ACTIVATED;
        }

        MoisturePrevention_Pause_Source = MP_PAUSE_INACTIVE;
    }
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @brief    Return amount of seconds for specified slot based on SettingFile settings.
 *             This function should be used in other part of this module to estimate slot time
 *
 * @param slot_number - slot id
 * @return number of seconds for slot
 */
uint16 GetSlotSeconds(uint8 slot_number)
{
    SLOT_DURATION_IDX_TYPE slot_duration;

    if((uint8)SettingFileStructure.SlotDurationSplitted == (uint8)TRUE)
    {
        uint8 data;

        //calculate index and extract byte from 64bit variable
        data = SettingFileStructure.SlotDurationBank[(slot_number >> 1)];

        if((slot_number % (uint8)2) == (uint8)0)
        {
            //Get HIGH half-byte in case of even number
            data = (data >> (uint8)4) & (uint8)0x0F;
        }
        else
        {
            //Get LOW half-byte in case of odd number
            data = (data >> (uint8)0) & (uint8)0x0F;
        }

        slot_duration = (SLOT_DURATION_IDX_TYPE) data;
    }
    else
    {
        slot_duration = (SLOT_DURATION_IDX_TYPE)SettingFileStructure.SlotDuration;
    }

    return GetSlotDurationSec(slot_duration);
}

/**
 * @brief    Function which translate logical slot duration index to numeric value
 *
 * @param slot_duration_idx - SettingFile index of slot duration
 * @return numeric value of specified slot index
 */
uint16 GetSlotDurationSec(SLOT_DURATION_IDX_TYPE slot_duration_idx)
{
	uint16 retval;
    switch (slot_duration_idx)
    {
        case SLOT_DURATION_60_SEC:
            retval = 60;
            break;
        case SLOT_DURATION_120_SEC:
            retval =  120;
            break;
        case SLOT_DURATION_300_SEC:
            retval = 300;
           break;
        case SLOT_DURATION_600_SEC:
            retval = 600;
            break;
        case SLOT_DURATION_900_SEC:
            retval = 900;
            break;
        case SLOT_DURATION_1200_SEC:
            retval = 1200;
            break;
        case SLOT_DURATION_1800_SEC:
            retval = 1800;
           break;
        case SLOT_DURATION_3600_SEC:
            retval = 3600;
           break;
        case SLOT_DURATION_7200_SEC:
            retval = 7200;
           break;
        default:
            retval = 300;
            break;
    }

    return retval;
}



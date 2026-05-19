/**
 *  @file		CartridgeDetectionMonitor
 *
 *  @brief		It implements the execution of the CartridgeDetectionMonitor.
 *
 *  @details
 *
 *
 *  @copyright  Copyright 2017-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "CartridgeDetectionMonitor.h"
// -- This Module --
#include "API013RemoteFunction.h"
#include "ApplianceMgr.h"
#include "Hbl.h"
#include "Micro.h"
#include "NVData.h"
#include "RemoteFunctionInterface.h"
#include "SystemConfig.h"
#include <string.h>



//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
static const HBL_LOAD_ENUM_TYPE BulkDispencerLoad[DETERGENT_TYPES_NUM] =
{
	HBL_BULK_DISPENSER_1,
	HBL_BULK_DISPENSER_2,
	HBL_BULK_DISPENSER_3,
};

#pragma location = "_appliance_segment"
static CARTRIDGE_DATA_TYPE CDM_Cartridge_Data;

static TRIDGE_DETERGENT_PARAMETER_TYPE *Cartridge_Paramters_Ptr;

static BOOL_TYPE CartridgeDetectionMonitorEnabled;

static uint8 Cartridge_Switch1_Input_Gi_Index;

static uint8 Cartridge_Door_Input_Gi_Index;

static CARTRIDGE_SWITCH_STATUS_TYPE Cartridge_Switch1_Status;

static CARTRIDGE_SWITCH_STATUS_TYPE Previous_Cartridge_Switch1_Status;

static CARTRIDGE_SWITCH_STATUS_TYPE Cartridge_Door_Status;

CARTRIDGE_HMI_DATA_TYPE Cartridge_HMI_Data;
uint8 Enzime_Tag_Operating_Temperature;
uint8 Bleach_Tag_Operating_Temperature;
uint8 RinseAid_Tag_Operating_Temperature;

static BOOL_TYPE Fst_TagStatusRead_Done;
static BOOL_TYPE Fst_TagStatusRead_Done_Previous;

static uint8 Bulk_Dispencer_State[DETERGENT_TYPES_NUM];
static uint8 Bulk_Dispencer_Previous_State[DETERGENT_TYPES_NUM];
static uint8 Bulk_Dispencer_Release_Time[DETERGENT_TYPES_NUM];
static uint8 Bulk_Dispencer_ML_Dispenced[DETERGENT_TYPES_NUM];
#pragma location = "_appliance_segment"
static uint16 Bulk_Dispencer_ML_Remaining[DETERGENT_TYPES_NUM];
#pragma location = "_appliance_segment"
static uint8 Bulk_Dispencer_Timer_ON[DETERGENT_TYPES_NUM];
#pragma location = "_appliance_segment"
static uint32 Cartridge_Serial_NNumber;

typedef PACKED struct WRITE_TAG_DATA_STRUCT
{
	//!
	DETERGENT_TYPE_TYPE Detergent_Type;
	//!
	uint16 ML_Remaining;
} WRITE_TAG_DATA_TYPE;

#define MAX_DATA_TO_PUSH	5U

static WRITE_TAG_DATA_TYPE WriteTagData[MAX_DATA_TO_PUSH];

typedef enum WRITE_TAG_MACHINE_STATUS_ENUM
{
	WRITE_TAG_MACHINE_STATUS_READY = 0,
	WRITE_TAG_MACHINE_STATUS_PENDING,
	WRITE_TAG_MACHINE_STATUS_VERIFING,
	WRITE_TAG_MACHINE_STATUS_FAILED
}WRITE_TAG_MACHINE_STATUS;

static WRITE_TAG_MACHINE_STATUS WriteTagMachineStatus;
static uint8 WriteTagTimer;
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void CartridgeDetectionMonitor_API013ApplReceivedStatus (uint8 client_function_index);
static uint8 GetDetergentPositionByType(DETERGENT_TYPE_TYPE detergent_type);

static void ReadTag(void);
static void UpdateHmiData(void);
static void WriteTag(void);
static void WriteTagPush(DETERGENT_TYPE_TYPE detergent_type, uint16 ml_remaining);
static void WriteTagPull(void);
static void WriteTagMachine(void);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//----------------------------------------------------------------------------
/**
 * It initializes the CartridgeDetectionMonitor and its data.
 *
 * @return None
 *
 */
void CartridgeDetectionMonitor__Initialize(void)
{
	if (Micro__GetResetMode() == MICRO_RESET_MODE_COLD)
	{
		if (NVData__GetLastRestoreStatus()->Bit.Appliance_Data_Recovered != (NVDATA_RESTORE_TYPE)NVDATA_RESTORE_OKAY)
		{
			memset(&CDM_Cartridge_Data, 0, sizeof(CDM_Cartridge_Data));
			memset(Bulk_Dispencer_ML_Remaining, 0xFF, sizeof(Bulk_Dispencer_ML_Remaining));
			memset(Bulk_Dispencer_Timer_ON, 0, sizeof(Bulk_Dispencer_Timer_ON));
			Cartridge_Serial_NNumber = 0;
		}

		CartridgeDetectionMonitorEnabled = FALSE;

		Cartridge_Switch1_Input_Gi_Index = Hbl__GetGIIndexByGIID(HBL_CARTRIDGE_SWITCH_1);

		Cartridge_Door_Input_Gi_Index = Hbl__GetGIIndexByGIID(HBL_CARTRIDGE_DOOR);

		if (Cartridge_Switch1_Input_Gi_Index != (uint8)HBL_INDEX_INVALID)
		{
			CartridgeDetectionMonitorEnabled = TRUE;

			API013RemoteFunction__RegisterForEvents((API013_EVENT_HANDLER_TYPE)CartridgeDetectionMonitor_API013ApplReceivedStatus);
		}

		Fst_TagStatusRead_Done = FALSE;

		//Cartridge_HMI_Data variable initialization
		memset(&Cartridge_HMI_Data, 0, sizeof(Cartridge_HMI_Data));
		memset(WriteTagData, 0xFF, sizeof(WriteTagData));

		SETTINGFILE_LOADER_TYPE loader;
		Cartridge_Paramters_Ptr = (TRIDGE_DETERGENT_PARAMETER_TYPE *)NULL;
		if (SettingFile__BasicLoader(SF_PTR_ACU_PRODUCT_CONFIGURATION_DATA, DSPL_TRIDGE_SYSTEM_PARAMS, &loader) == PASS)
		{
			Cartridge_Paramters_Ptr = (TRIDGE_DETERGENT_PARAMETER_TYPE *)(void *)loader.Data;

			Bulk_Dispencer_Release_Time[DETERGENT_TYPE_ENZIME] = Cartridge_Paramters_Ptr->Release_Time_Enzime;
			Bulk_Dispencer_Release_Time[DETERGENT_TYPE_BLEACH] = Cartridge_Paramters_Ptr->Release_Time_Bleach;
			Bulk_Dispencer_Release_Time[DETERGENT_TYPE_RINSE_AID] = Cartridge_Paramters_Ptr->Release_Time_RinseAid;
		}
	}
}

//----------------------------------------------------------------------------
/**
 * It implements the execution of the CartridgeDetectionMonitor.
 *
 * @return None
 *
 */
void CartridgeDetectionMonitor__Execution(void)
{
	if (CartridgeDetectionMonitorEnabled != FALSE)
	{
		if ((uint8)ApplianceMgr__IsStable() == (uint8)TRUE)
		{
			ReadTag();

			UpdateHmiData();

			WriteTag();
		}
	}
	else
	{
		// Algorithm is disabled
	}
}

//----------------------------------------------------------------------------
/**
 * It implements the WriteTagMLRemaining of the CartridgeDetectionMonitor.
 *
 * @return None
 *
 */
void CartridgeDetectionMonitor__WriteTagMLRemaining(DETERGENT_TYPE_TYPE detergent_type, uint16 ml_remaining)
{
	uint8 detergent_position = GetDetergentPositionByType(detergent_type);

	if (detergent_position < CDM_Cartridge_Data.Tag_Header.Detergent_Num)
	{
		uint16 position_offset = (uint16)CDM_Cartridge_Data.Tag_Header.Num_Of_Bytes_Per_Detergent * detergent_position;
		uint8 enable_params[7];
		uint16 address;

		address = 	sizeof(TAG_HEADER_TYPE) +
					position_offset +
					sizeof(CDM_Cartridge_Data.Dispenser[0].Detergent_Type) +
					sizeof(CDM_Cartridge_Data.Dispenser[0].PumpNumber) +
					sizeof(CDM_Cartridge_Data.Dispenser[0].DoseAmount_ML) +
					sizeof(CDM_Cartridge_Data.Dispenser[0].Operating_Temperature);

		enable_params[0] = INDEX_ANTENNA_1;
		enable_params[1] = HIBYTE(address);
		enable_params[2] = LOBYTE(address);
		enable_params[3] = HIBYTE(sizeof(ml_remaining)); //lint !e572
		enable_params[4] = LOBYTE(sizeof(ml_remaining));

		// TAG is written in Little Endian
		enable_params[5] = LOBYTE(ml_remaining);
		enable_params[6] = HIBYTE(ml_remaining);

		(void)API013RemoteFunction__SendControl (CLIENT_REMOTE_FUNCTION_WRITE_TAG,  REMOTE_FUNCTION_COMMAND_ENABLE, enable_params, sizeof(enable_params));
	}
}

//----------------------------------------------------------------------------
/**
 * It implements the GetTagData of the CartridgeDetectionMonitor.
 *
 * @return BOOL_TYPE
 *
 */
BOOL_TYPE CartridgeDetectionMonitor__GetTagData(DETERGENT_TYPE_TYPE detergent_type, TAG_DATA_TYPE *tag_data_ptr)
{
	BOOL_TYPE ret_value = FALSE;
	uint8 detergent_position = GetDetergentPositionByType(detergent_type);

	if (detergent_position < ELEMENTS_IN_ARRAY(CDM_Cartridge_Data.Dispenser))
	{
		if (tag_data_ptr != NULL)
		{
			memcpy((uint8 *)tag_data_ptr, (uint8 *)&CDM_Cartridge_Data.Dispenser[detergent_position], sizeof(TAG_DATA_TYPE));
			ret_value = TRUE;
		}
	}

	return ret_value;
}

//----------------------------------------------------------------------------
/**
 * It implements the GetTagPumpNumber of the CartridgeDetectionMonitor.
 *
 * @return BOOL_TYPE
 *
 */
BOOL_TYPE CartridgeDetectionMonitor__GetTagPumpNumber(DETERGENT_TYPE_TYPE detergent_type, uint8 *pump_number_ptr)
{
	BOOL_TYPE ret_value = FALSE;
	uint8 detergent_position = GetDetergentPositionByType(detergent_type);

	if (detergent_position < ELEMENTS_IN_ARRAY(CDM_Cartridge_Data.Dispenser))
	{
		if (pump_number_ptr != NULL)
		{
			*pump_number_ptr = CDM_Cartridge_Data.Dispenser[detergent_position].PumpNumber;
			ret_value = TRUE;
		}
	}

	return ret_value;
}

//----------------------------------------------------------------------------
/**
 * It implements the GetTagDoseAmountML of the CartridgeDetectionMonitor.
 *
 * @return BOOL_TYPE
 *
 */
BOOL_TYPE CartridgeDetectionMonitor__GetTagDoseAmountML(DETERGENT_TYPE_TYPE detergent_type, uint8 *dose_amount_ptr)
{
	BOOL_TYPE ret_value = FALSE;
	uint8 detergent_position = GetDetergentPositionByType(detergent_type);

	if (detergent_position < ELEMENTS_IN_ARRAY(CDM_Cartridge_Data.Dispenser))
	{
		if (dose_amount_ptr != NULL)
		{
			*dose_amount_ptr = CDM_Cartridge_Data.Dispenser[detergent_position].DoseAmount_ML;
			ret_value = TRUE;
		}
	}

	return ret_value;
}

//----------------------------------------------------------------------------
/**
 * It implements the GetTagMLRemaining of the CartridgeDetectionMonitor.
 *
 * @return BOOL_TYPE
 *
 */
BOOL_TYPE CartridgeDetectionMonitor__GetTagMLRemaining(DETERGENT_TYPE_TYPE detergent_type, uint16 *ml_remaining_ptr)
{
	BOOL_TYPE ret_value = FALSE;
	uint8 detergent_position = GetDetergentPositionByType(detergent_type);

	if (detergent_position < ELEMENTS_IN_ARRAY(CDM_Cartridge_Data.Dispenser))
	{
		if (ml_remaining_ptr != NULL)
		{
			*ml_remaining_ptr = CDM_Cartridge_Data.Dispenser[detergent_position].ML_Remaining;
			ret_value = TRUE;
		}
	}

	return ret_value;
}

//----------------------------------------------------------------------------
/**
 * It implements the GetTagMLCapacity of the CartridgeDetectionMonitor.
 *
 * @return BOOL_TYPE
 *
 */
BOOL_TYPE CartridgeDetectionMonitor__GetTagMLCapacity(DETERGENT_TYPE_TYPE detergent_type, uint16 *ml_capacity_ptr)
{
	BOOL_TYPE ret_value = FALSE;
	uint8 detergent_position = GetDetergentPositionByType(detergent_type);

	if (detergent_position < ELEMENTS_IN_ARRAY(CDM_Cartridge_Data.Dispenser))
	{
		if (ml_capacity_ptr != NULL)
		{
			*ml_capacity_ptr = CDM_Cartridge_Data.Dispenser[detergent_position].ML_Capacity;
			ret_value = TRUE;
		}
	}

	return ret_value;
}

//----------------------------------------------------------------------------
/**
 * It implements the GetTagOperatingTemperature of the CartridgeDetectionMonitor.
 *
 * @return BOOL_TYPE
 *
 */
BOOL_TYPE CartridgeDetectionMonitor__GetTagOperatingTemperature(DETERGENT_TYPE_TYPE detergent_type, uint8 *op_temperature_ptr)
{
	BOOL_TYPE ret_value = FALSE;
	uint8 detergent_position = GetDetergentPositionByType(detergent_type);

	if (detergent_position < ELEMENTS_IN_ARRAY(CDM_Cartridge_Data.Dispenser))
	{
		if (op_temperature_ptr != NULL)
		{
			*op_temperature_ptr = CDM_Cartridge_Data.Dispenser[detergent_position].Operating_Temperature;
			ret_value = TRUE;
		}
	}

	return ret_value;
}

//----------------------------------------------------------------------------
/**
 * It implements the GetTagDetergentNum of the CartridgeDetectionMonitor.
 *
 * @return uint8
 *
 */
uint8 CartridgeDetectionMonitor__GetTagDetergentNum(void)
{
	return CDM_Cartridge_Data.Tag_Header.Detergent_Num;
}

//----------------------------------------------------------------------------
/**
 * It implements the GetTagBytesPerDetergent of the CartridgeDetectionMonitor.
 *
 * @return uint8
 *
 */
uint8 CartridgeDetectionMonitor__GetTagBytesPerDetergent(void)
{
	return CDM_Cartridge_Data.Tag_Header.Num_Of_Bytes_Per_Detergent;
}


//----------------------------------------------------------------------------
/**
 * It implements the GetTagSerialNumber of the CartridgeDetectionMonitor.
 *
 * @return uint8
 *
 */
uint32 CartridgeDetectionMonitor__GetTagSerialNumber(void)
{
	return CDM_Cartridge_Data.Tag_Header.Serial_Number;
}


//----------------------------------------------------------------------------
/**
 * It returns Fst_TagStatusRead_Done
 *
 * @return BOOL_TYPE
 *
 */
BOOL_TYPE CartridgeDetectionMonitor__GetFstTagStatusReadDone(void)
{
	return Fst_TagStatusRead_Done;
}

CARTRIDGE_SWITCH_STATUS_TYPE CartridgeDetectionMonitor__GetCartridgeDoorStatus(void)
{
	CARTRIDGE_SWITCH_STATUS_TYPE cartridge_door_status = CARTRIDGE_SWITCH_STATUS_CLOSED;
	if (Cartridge_Door_Input_Gi_Index != (uint8)HBL_INDEX_INVALID)
	{
		cartridge_door_status =  Cartridge_Door_Status;
	}

	return cartridge_door_status;
}

//----------------------------------------------------------------------------
/**
 * It implements the GetLocalMLRemaining of the CartridgeDetectionMonitor.
 *
 * @return BOOL_TYPE
 *
 */
BOOL_TYPE CartridgeDetectionMonitor__GetLocalMLRemaining(DETERGENT_TYPE_TYPE detergent_type, uint16 *ml_remaining_ptr)
{
	BOOL_TYPE ret_value = FALSE;
	uint8 detergent_position = GetDetergentPositionByType(detergent_type);

	if (detergent_position < ELEMENTS_IN_ARRAY(CDM_Cartridge_Data.Dispenser))
	{
		if (ml_remaining_ptr != NULL)
		{
			*ml_remaining_ptr = Bulk_Dispencer_ML_Remaining[detergent_position];
			ret_value = TRUE;
		}
	}

	return ret_value;
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

//----------------------------------------------------------------------------
/**
 * It processes the status received from API013
 *
 * @return none
 *
 */
static void CartridgeDetectionMonitor_API013ApplReceivedStatus (uint8 client_function_index)
{
	const API013_FUNCTION_STATUS_TYPE* received_status;
	received_status = API013RemoteFunction__GetReceivedStatus();

	switch (client_function_index)
	{
		case CLIENT_REMOTE_FUNCTION_READ_TAG:
		{
			if (received_status->Size >= 2U)
			{
				if (received_status->Status[1] == (uint8)TAG_READ_WRITE_SUCCESSFULL)
				{
					if (received_status->Size >= 4U)
					{
						uint16 tag_data_size = COMBINE_BYTES(received_status->Status[2], received_status->Status[3]);
						if ((tag_data_size >= sizeof(TAG_HEADER_TYPE)) && (received_status->Size >= (tag_data_size + 4U)))
						{
							uint8 status_buffer_offset = 4;
							TAG_HEADER_TYPE tag_header;

							memcpy(&tag_header, &received_status->Status[status_buffer_offset], sizeof(TAG_HEADER_TYPE));

							if (tag_data_size >= ((uint16)sizeof(TAG_HEADER_TYPE) + ((uint16)tag_header.Detergent_Num * tag_header.Num_Of_Bytes_Per_Detergent)))
							{
								// Reset stored tag data;
								memset(&CDM_Cartridge_Data, 0x00, sizeof(CARTRIDGE_DATA_TYPE));
								// First update Tag_Header data
								memcpy(&CDM_Cartridge_Data.Tag_Header, &received_status->Status[status_buffer_offset], sizeof(TAG_HEADER_TYPE));

								status_buffer_offset += sizeof(TAG_HEADER_TYPE);

								for (uint8 i = 0; (i < CDM_Cartridge_Data.Tag_Header.Detergent_Num) && (i < ELEMENTS_IN_ARRAY(CDM_Cartridge_Data.Dispenser)); i ++)
								{
									TAG_DATA_TYPE temp_dispenser_data;

									uint8 bytes_to_copy = (CDM_Cartridge_Data.Tag_Header.Num_Of_Bytes_Per_Detergent <= sizeof(TAG_DATA_TYPE)) ? (CDM_Cartridge_Data.Tag_Header.Num_Of_Bytes_Per_Detergent) : (sizeof(TAG_DATA_TYPE));

									memcpy(&temp_dispenser_data, &received_status->Status[status_buffer_offset], bytes_to_copy );

									// Endianess compatibility. Data on Tag are Little Endian
									temp_dispenser_data.ML_Remaining = ENDIANU16_LTON(temp_dispenser_data.ML_Remaining);
									temp_dispenser_data.ML_Capacity = ENDIANU16_LTON(temp_dispenser_data.ML_Capacity);

									memcpy(&CDM_Cartridge_Data.Dispenser[i], (uint8 *)&temp_dispenser_data, bytes_to_copy );
									status_buffer_offset += CDM_Cartridge_Data.Tag_Header.Num_Of_Bytes_Per_Detergent;
								}

								// Mark tag status as read
								Fst_TagStatusRead_Done = TRUE;

								if (WriteTagMachineStatus == WRITE_TAG_MACHINE_STATUS_VERIFING)
								{
									WriteTagPull();
									WriteTagMachineStatus = WRITE_TAG_MACHINE_STATUS_READY;
								}
							}
							else
							{
								// Inconsistent tag data size
							}
						}
						else
						{
							// Inconsistent tag data size
						}

					}
					else
					{
						// Inconsistent status size
					}
				}
				else
				{
					// Todo Manage unsuccessful command
				}
			}
			else
			{
				// Inconsistent status size
			}
		}
		break;
		case CLIENT_REMOTE_FUNCTION_WRITE_TAG:
		{
			if (received_status->Size >= 2U)
			{
				if (received_status->Status[1] == (uint8)TAG_READ_WRITE_SUCCESSFULL)
				{
					/* read tag to update CDM_Cartridge_Data */
					(void)API013RemoteFunction__SendControl (CLIENT_REMOTE_FUNCTION_READ_TAG,  REMOTE_FUNCTION_COMMAND_ENABLE, &received_status->Status[0], sizeof(received_status->Status[0]));
					WriteTagMachineStatus = WRITE_TAG_MACHINE_STATUS_VERIFING;
					WriteTagTimer = 0;
				}
				else
				{
					WriteTagMachineStatus = WRITE_TAG_MACHINE_STATUS_FAILED;
				}
			}
			else
			{
				// Inconsistent status size
			}
		}
		break;
		default:
		// do nothing
		break;
	}
}

//----------------------------------------------------------------------------
/**
 * It returns the position in data tag of the data block related to the detergent_type given in input.
 * If no data block related to the detergent_type is found, it returns 0xFF
 *
 * @return uint8
 *
 */
static uint8 GetDetergentPositionByType(DETERGENT_TYPE_TYPE detergent_type)
{
	uint8 ret_value = 0xFF;

	for (uint8 i = 0; (i < CDM_Cartridge_Data.Tag_Header.Detergent_Num) && (i < ELEMENTS_IN_ARRAY(CDM_Cartridge_Data.Dispenser)); i ++)
	{
		if (CDM_Cartridge_Data.Dispenser[i].Detergent_Type == detergent_type)
		{
			ret_value = i;
			break;
		}
	}

	return ret_value;
}

//----------------------------------------------------------------------------
/**
 * It Read data from tag when the Cartridge_Switch1_Status is closed
 *
 *
 */
static void ReadTag(void)
{
	uint8 Cartridge_Switch1_input_sequence_id;
	Cartridge_Switch1_input_sequence_id = Hbl__GetGISequenceIDByIndex(Cartridge_Switch1_Input_Gi_Index);

	if (Cartridge_Switch1_input_sequence_id > 0U)
	{
		Cartridge_Switch1_Status = (CARTRIDGE_SWITCH_STATUS_TYPE)Hbl__GetGIDataConvertedByIndex(Cartridge_Switch1_Input_Gi_Index);

		if (Cartridge_Switch1_Status == CARTRIDGE_SWITCH_STATUS_CLOSED)
		{
			if (Previous_Cartridge_Switch1_Status == CARTRIDGE_SWITCH_STATUS_OPEN)
			{
				uint8 enable_params = INDEX_ANTENNA_1;;

				(void)API013RemoteFunction__SendControl (CLIENT_REMOTE_FUNCTION_READ_TAG,  REMOTE_FUNCTION_COMMAND_ENABLE, &enable_params, sizeof(enable_params));
			}
		}
		else if (Cartridge_Switch1_Status == CARTRIDGE_SWITCH_STATUS_OPEN)
		{
			Fst_TagStatusRead_Done = FALSE;

			if (Previous_Cartridge_Switch1_Status == CARTRIDGE_SWITCH_STATUS_CLOSED)
			{
				memset(&CDM_Cartridge_Data, 0, sizeof(CDM_Cartridge_Data));
			}
		}
		else
		{
			// Do Nothing
		}

		// Align previous state
		Previous_Cartridge_Switch1_Status = Cartridge_Switch1_Status;

		// Update Cartridge Switch
		Cartridge_HMI_Data.Flags.Cartridge_Inserted = (BOOL_TYPE)Cartridge_Switch1_Status;
	}
}

//----------------------------------------------------------------------------
/**
 * It Update the Cartridge_HMI_Data
 *
 *
 */
static void UpdateHmiData(void)
{
	// Update Cartridge Door Switch
	uint8 Cartridge_Switch2_door_input_sequence_id;
	Cartridge_Switch2_door_input_sequence_id = Hbl__GetGISequenceIDByIndex(Cartridge_Door_Input_Gi_Index);

	if (Cartridge_Switch2_door_input_sequence_id > 0U)
	{
		Cartridge_Door_Status = (CARTRIDGE_SWITCH_STATUS_TYPE)Hbl__GetGIDataConvertedByIndex(Cartridge_Door_Input_Gi_Index);

		Cartridge_HMI_Data.Flags.Cartridge_Door_Closed = (BOOL_TYPE)Cartridge_Door_Status;
	}

	// Update Operating Temporature for GESE
	uint8 op_temp_local;
	if (CartridgeDetectionMonitor__GetTagOperatingTemperature(DETERGENT_TYPE_ENZIME,&op_temp_local) != FALSE)
	{
		Enzime_Tag_Operating_Temperature = op_temp_local;
	}
	if (CartridgeDetectionMonitor__GetTagOperatingTemperature(DETERGENT_TYPE_BLEACH,&op_temp_local) != FALSE)
	{
		Bleach_Tag_Operating_Temperature = op_temp_local;
	}
	if (CartridgeDetectionMonitor__GetTagOperatingTemperature(DETERGENT_TYPE_RINSE_AID,&op_temp_local) != FALSE)
	{
		RinseAid_Tag_Operating_Temperature = op_temp_local;
	}

	// Update HMI data
	uint16 cartridge_capacity =  CDM_Cartridge_Data.Dispenser[DETERGENT_TYPE_ENZIME].ML_Capacity +
								 CDM_Cartridge_Data.Dispenser[DETERGENT_TYPE_BLEACH].ML_Capacity +
								 CDM_Cartridge_Data.Dispenser[DETERGENT_TYPE_RINSE_AID].ML_Capacity;

	uint16 cartridge_ML_remaing = CDM_Cartridge_Data.Dispenser[DETERGENT_TYPE_ENZIME].ML_Remaining +
								  CDM_Cartridge_Data.Dispenser[DETERGENT_TYPE_BLEACH].ML_Remaining +
								  CDM_Cartridge_Data.Dispenser[DETERGENT_TYPE_RINSE_AID].ML_Remaining;

	if (cartridge_capacity >0)
	{
		Cartridge_HMI_Data.Percentage_level = (uint8)((cartridge_ML_remaing * 100U)/cartridge_capacity);
	}
	if (Cartridge_HMI_Data.Percentage_level <= Cartridge_Paramters_Ptr->Empty_Level)
	{
		Cartridge_HMI_Data.Flags.Empty_Level = TRUE;
		Cartridge_HMI_Data.Flags.Low_Level = FALSE;
	}
	else if (Cartridge_HMI_Data.Percentage_level <= Cartridge_Paramters_Ptr->Low_Level)
	{
		Cartridge_HMI_Data.Flags.Empty_Level = FALSE;
		Cartridge_HMI_Data.Flags.Low_Level = TRUE;
	}
	else
	{
		Cartridge_HMI_Data.Flags.Empty_Level = FALSE;
		Cartridge_HMI_Data.Flags.Low_Level = FALSE;
	}

	Cartridge_HMI_Data.Flags.Valid_Data = Fst_TagStatusRead_Done;
}


//----------------------------------------------------------------------------
/**
 * It Write data to the tag when the Pumps are turned off
 *
 *
 */
static void WriteTag(void)
{
	uint8 *current_state_ptr;

	for (uint8 bulk_dispencer_index = (uint8)0; bulk_dispencer_index < (uint8)DETERGENT_TYPES_NUM; bulk_dispencer_index++)
	{
		current_state_ptr = (uint8*)Hbl__GetRequestedLoad(Hbl__GetLoadIndexByLoadID(BulkDispencerLoad[CDM_Cartridge_Data.Dispenser[bulk_dispencer_index].PumpNumber]));
		if (current_state_ptr != NULL)
		{
			Bulk_Dispencer_State[bulk_dispencer_index] = *current_state_ptr;
		}
		else
		{
			Bulk_Dispencer_State[bulk_dispencer_index] = (uint8)OFF;
		}

		// Start Timer
		if ((Bulk_Dispencer_State[bulk_dispencer_index] == (uint8)ON) &&
			(Bulk_Dispencer_Previous_State[bulk_dispencer_index] == (uint8)OFF))
		{
			Bulk_Dispencer_Timer_ON[bulk_dispencer_index]++;
			Bulk_Dispencer_ML_Dispenced[bulk_dispencer_index] = 0;
			Bulk_Dispencer_ML_Remaining[bulk_dispencer_index] = CDM_Cartridge_Data.Dispenser[bulk_dispencer_index].ML_Remaining;
		}
		// Timer Count
		else if ((Bulk_Dispencer_State[bulk_dispencer_index] == (uint8)ON) &&
				(Bulk_Dispencer_Previous_State[bulk_dispencer_index] == (uint8)ON))
		{
			Bulk_Dispencer_Timer_ON[bulk_dispencer_index]++;

			if ((Bulk_Dispencer_Timer_ON[bulk_dispencer_index] % Bulk_Dispencer_Release_Time[CDM_Cartridge_Data.Dispenser[bulk_dispencer_index].Detergent_Type]) == (uint8)0)
			{
				Bulk_Dispencer_Timer_ON[bulk_dispencer_index] = 0;
				Bulk_Dispencer_ML_Dispenced[bulk_dispencer_index]++;
				if (CDM_Cartridge_Data.Dispenser[bulk_dispencer_index].ML_Remaining > Bulk_Dispencer_ML_Dispenced[bulk_dispencer_index])
				{
					Bulk_Dispencer_ML_Remaining[bulk_dispencer_index] = CDM_Cartridge_Data.Dispenser[bulk_dispencer_index].ML_Remaining - Bulk_Dispencer_ML_Dispenced[bulk_dispencer_index];
				}
				else
				{
					Bulk_Dispencer_ML_Remaining[bulk_dispencer_index] = 0;
				}
			}
		}
		// Write Tag
		else if ((Bulk_Dispencer_State[bulk_dispencer_index] == (uint8)OFF) &&
				(Bulk_Dispencer_Previous_State[bulk_dispencer_index] == (uint8)ON))
		{
			if (Bulk_Dispencer_ML_Dispenced[bulk_dispencer_index] > (uint8)0)
			{
				WriteTagPush((DETERGENT_TYPE_TYPE)CDM_Cartridge_Data.Dispenser[bulk_dispencer_index].Detergent_Type, Bulk_Dispencer_ML_Remaining[bulk_dispencer_index]);
			}
		}
		else
		{
			// Do Nothing
		}

		Bulk_Dispencer_Previous_State[bulk_dispencer_index] = Bulk_Dispencer_State[bulk_dispencer_index];
	}

	// Check if the Cartridge is changed the first reading after the cartridge switch is closed or after a cold reset
	if ((Fst_TagStatusRead_Done != FALSE) &&
		(Fst_TagStatusRead_Done_Previous == FALSE))
	{
		if ((CDM_Cartridge_Data.Tag_Header.Serial_Number == Cartridge_Serial_NNumber) && (Cartridge_Serial_NNumber != 0U))
		{
			for (uint8 bulk_dispencer_index = (uint8)DETERGENT_TYPE_ENZIME; bulk_dispencer_index < (uint8)DETERGENT_TYPES_NUM; bulk_dispencer_index++)
			{
				if (Bulk_Dispencer_ML_Remaining[bulk_dispencer_index] != CDM_Cartridge_Data.Dispenser[bulk_dispencer_index].ML_Remaining)
				{
					WriteTagPush((DETERGENT_TYPE_TYPE)CDM_Cartridge_Data.Dispenser[bulk_dispencer_index].Detergent_Type, Bulk_Dispencer_ML_Remaining[bulk_dispencer_index]);
				}
			}
		}
		else // New Cartridge inserted
		{
			Cartridge_Serial_NNumber = CDM_Cartridge_Data.Tag_Header.Serial_Number;
			for (uint8 bulk_dispencer_index = (uint8)DETERGENT_TYPE_ENZIME; bulk_dispencer_index < (uint8)DETERGENT_TYPES_NUM; bulk_dispencer_index++)
			{
				Bulk_Dispencer_ML_Remaining[bulk_dispencer_index] = CDM_Cartridge_Data.Dispenser[bulk_dispencer_index].ML_Remaining;
				Bulk_Dispencer_Timer_ON[bulk_dispencer_index] = 0;
			}
		}
	}
	Fst_TagStatusRead_Done_Previous = Fst_TagStatusRead_Done;

	WriteTagMachine();

}

static void WriteTagPush(DETERGENT_TYPE_TYPE detergent_type, uint16 ml_remaining)
{
	for (uint8 index = 0; index < MAX_DATA_TO_PUSH; index++)
	{
		if (WriteTagData[index].Detergent_Type >= DETERGENT_TYPES_NUM)
		{
			WriteTagData[index].Detergent_Type = detergent_type;
			WriteTagData[index].ML_Remaining = ml_remaining;
			break;
		}
	}
}

static void WriteTagPull(void)
{
	for (uint8 index = 0; index < (MAX_DATA_TO_PUSH - 1U); index++)
	{
		WriteTagData[index].Detergent_Type = WriteTagData[index+1U].Detergent_Type;
		WriteTagData[index].ML_Remaining = WriteTagData[index+1U].ML_Remaining;

		WriteTagData[index+1U].Detergent_Type = (DETERGENT_TYPE_TYPE)__UNSIGNED_CHAR_MAX__;
		WriteTagData[index+1U].ML_Remaining = __UNSIGNED_SHORT_MAX__;
	}
}

static void WriteTagMachine(void)
{
	switch (WriteTagMachineStatus)
	{
		case WRITE_TAG_MACHINE_STATUS_READY:
		{
			if (WriteTagData[0].Detergent_Type < DETERGENT_TYPES_NUM)
			{
				CartridgeDetectionMonitor__WriteTagMLRemaining((DETERGENT_TYPE_TYPE)WriteTagData[0].Detergent_Type, WriteTagData[0].ML_Remaining);
				WriteTagTimer = 0;
				WriteTagMachineStatus = WRITE_TAG_MACHINE_STATUS_PENDING;
			}
		}
		break;

		case WRITE_TAG_MACHINE_STATUS_PENDING:
		case WRITE_TAG_MACHINE_STATUS_VERIFING:
		{
			WriteTagTimer++;
			if (WriteTagTimer >= 100U) // 10 Seconds
			{
				WriteTagMachineStatus = WRITE_TAG_MACHINE_STATUS_FAILED;
			}
		}
		break;
		case WRITE_TAG_MACHINE_STATUS_FAILED: // TO BE DEFINED
		{
			// For the moment we pull the packet as for success behavior
			WriteTagMachineStatus = WRITE_TAG_MACHINE_STATUS_READY;
			WriteTagPull();
		}
		break;
		default:
			// nothing to do
		break;
	}
}


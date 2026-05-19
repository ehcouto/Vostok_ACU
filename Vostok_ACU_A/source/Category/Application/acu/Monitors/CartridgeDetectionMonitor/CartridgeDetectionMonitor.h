/*
 * @file		CartridgeDetectionMonitor.h
 * @brief       Public interface to the CartridgeDetectionMonitor module.
 *
 *  Created on: 1 July 2024
 *      Author: CERINS
 */

#ifndef CARTRIDGEDETECTIONMONITOR_H_
#define CARTRIDGEDETECTIONMONITOR_H_

//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "C_Extensions.h"
// -- Other Modules --

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


typedef enum DETERGENT_TYPE_ENUM
{
	DETERGENT_TYPE_ENZIME = 0,
	DETERGENT_TYPE_BLEACH,
	DETERGENT_TYPE_RINSE_AID,
	DETERGENT_TYPES_NUM
} DETERGENT_TYPE_TYPE;

//!
typedef PACKED struct TAG_HEADER_STRUCT
{
	//!
	uint32 Serial_Number;
	//!
	uint8 Production_date_YY;
	//!
	uint8 Production_date_MM;
	//!
	uint8 Production_date_DD;
	//!
	uint8 Detergent_Num;
	//!
	uint8 Num_Of_Bytes_Per_Detergent;
	//!
	uint8 Unused;
} TAG_HEADER_TYPE;

//! Machine Configurations setting file parameters structure for Tridge Detergent management
typedef PACKED struct TRIDGE_DETERGENT_PARAMETER_STRUCT
{
	uint8 Low_Level;
	uint8 Empty_Level;
	uint8 Release_Time_Enzime;
	uint8 Last_Cycle_Threshold_Enzime;
	uint8 Release_Time_Bleach;
	uint8 Last_Cycle_Threshold_Bleach;
	uint8 Release_Time_RinseAid;
	uint8 Last_Cycle_Threshold_RinseAid;
} TRIDGE_DETERGENT_PARAMETER_TYPE;

//!
//!!!! NOTE: this structure has to be aligned address calculation in CartridgeDetectionMonitor__WriteTagMLRemaining
typedef PACKED struct TAG_DATA_STRUCT
{
	//!
	DETERGENT_TYPE_TYPE Detergent_Type;
	//!
	uint8 PumpNumber;
	//!
	uint8 DoseAmount_ML;
	//!
	uint8 Operating_Temperature;
	//!
	uint16 ML_Remaining;
	//!
	uint16 ML_Capacity;
} TAG_DATA_TYPE;

typedef PACKED struct CARTRIDGE_DATA_STRUCT
{
	//!
	TAG_HEADER_TYPE Tag_Header;
	//!
	TAG_DATA_TYPE Dispenser[DETERGENT_TYPES_NUM];
} CARTRIDGE_DATA_TYPE;

//! Structure for Cartridge Data shared with HMI
typedef struct CARTRIDGE_HMI_DATA
{
	union
	{
		uint8 All;
		struct
		{
			uint8  Low_Level  				:1; 	//!
			uint8  Empty_Level				:1;		//!
			uint8  Cartridge_Inserted   	:1;		//!
			uint8  Cartridge_Door_Closed	:1;		//!
			uint8  Valid_Data				:1;		//!
			uint8  Unused					:3;
		};
	} Flags;

	uint8 Percentage_level;
} CARTRIDGE_HMI_DATA_TYPE;

#define INDEX_ANTENNA_1	0

typedef enum CARTRIDGE_READ_WRITE_STATUS_ENUM
{
	TAG_READ_WRITE_SUCCESSFULL = 0,		// tag was successfully read/write
	TAG_READ_WRITE_CARTRIDGE_REMOVED,	// Cartridge Removed
	TAG_READ_WRITE_FAILED,				// read/Write failed
	TAG_READ_WRITE_BUFFER_ERROR,		// The Remote Function buffer is not long enough to store the rfd tag read data.
	TAG_READ_WRITE_NDEF_ERROR			// It was not possible to extract the Ndef record content from the read data
}CARTRIDGE_READ_WRITE_STATUS;

typedef enum CARTRIDGE_SWITCH_STATUS_ENUM
{
	CARTRIDGE_SWITCH_STATUS_OPEN = 0,
	CARTRIDGE_SWITCH_STATUS_CLOSED,
	CARTRIDGE_SWITCH_STATUS_UNKNOWN
}CARTRIDGE_SWITCH_STATUS_TYPE;


extern CARTRIDGE_HMI_DATA_TYPE Cartridge_HMI_Data;
extern uint8 Enzime_Tag_Operating_Temperature;
extern uint8 Bleach_Tag_Operating_Temperature;
extern uint8 RinseAid_Tag_Operating_Temperature;
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void CartridgeDetectionMonitor__Initialize(void);
void CartridgeDetectionMonitor__Execution(void);

void CartridgeDetectionMonitor__WriteTagMLRemaining(DETERGENT_TYPE_TYPE detergent_type, uint16 ml_remaining);
BOOL_TYPE CartridgeDetectionMonitor__GetTagData(DETERGENT_TYPE_TYPE detergent_type, TAG_DATA_TYPE *tag_data_ptr);
BOOL_TYPE CartridgeDetectionMonitor__GetTagPumpNumber(DETERGENT_TYPE_TYPE detergent_type, uint8 *pump_number_ptr);
BOOL_TYPE CartridgeDetectionMonitor__GetTagDoseAmountML(DETERGENT_TYPE_TYPE detergent_type, uint8 *dose_amount_ptr);
BOOL_TYPE CartridgeDetectionMonitor__GetTagMLRemaining(DETERGENT_TYPE_TYPE detergent_type, uint16 *ml_remaining_ptr);
BOOL_TYPE CartridgeDetectionMonitor__GetTagMLCapacity(DETERGENT_TYPE_TYPE detergent_type, uint16 *ml_capacity_ptr);
BOOL_TYPE CartridgeDetectionMonitor__GetTagOperatingTemperature(DETERGENT_TYPE_TYPE detergent_type, uint8 *op_temperature_ptr);
uint8 CartridgeDetectionMonitor__GetTagDetergentNum(void);
uint8 CartridgeDetectionMonitor__GetTagBytesPerDetergent(void);
uint32 CartridgeDetectionMonitor__GetTagSerialNumber(void);
BOOL_TYPE CartridgeDetectionMonitor__GetFstTagStatusReadDone(void);
CARTRIDGE_SWITCH_STATUS_TYPE CartridgeDetectionMonitor__GetCartridgeDoorStatus(void);
BOOL_TYPE CartridgeDetectionMonitor__GetLocalMLRemaining(DETERGENT_TYPE_TYPE detergent_type, uint16 *ml_remaining_ptr);
#endif /* CARTRIDGEDETECTIONMONITOR_H_ */

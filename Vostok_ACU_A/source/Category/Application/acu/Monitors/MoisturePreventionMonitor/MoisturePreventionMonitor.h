/*
 * MoisturePreventionMonitor.h
 *
 *  Created on: 07.05.2018
 *      Author: WAZNYP
 */

#ifndef SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_MOISTUREPREVENTIONMONITOR_MOISTUREPREVENTIONMONITOR_H_
#define SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_MOISTUREPREVENTIONMONITOR_MOISTUREPREVENTIONMONITOR_H_

typedef enum SLOT_DURATION_IDX_ENUM
{
    SLOT_DURATION_60_SEC    = 0, // 1 min
    SLOT_DURATION_120_SEC   = 1, // 2 min
    SLOT_DURATION_300_SEC   = 2, // 5 min
    SLOT_DURATION_600_SEC   = 3, // 10 min
    SLOT_DURATION_900_SEC   = 4, // 15 min
    SLOT_DURATION_1200_SEC  = 5, // 20 min
    SLOT_DURATION_1800_SEC  = 6, // 30 min
    SLOT_DURATION_3600_SEC  = 7, // 60 min
    SLOT_DURATION_7200_SEC  = 8  // 120 min
} SLOT_DURATION_IDX_TYPE;

typedef PACKED struct MoisturePreventionSettingFileStructure
{
	uint8  SlotDuration  		:3;
    uint8  SlotDurationSplitted :1;
    uint8  NumberOfSlots 		:4;
    uint8  FanOFFSpeed   		:4;
    uint8  FanONSpeed    		:4;
    uint16 VentStatusBitMap;
    uint16 FanStatusBitMap;
    uint8  SlotDurationBank[8];
}MoisturePreventionSettingFileStructureType;

typedef enum MP_STATUS_ENUM
{
    MP_DEACTIVATED = 0,
	MP_PAUSED,
	MP_ACTIVATED
}MP_STATUS_TYPE;

typedef enum MP_PAUSED_SOURCE_ENUM
{
	//Priority MIN
	MP_PAUSE_INACTIVE = 0,
	MP_PAUSE_SUPERVISOR_EVENT,
	MP_PAUSE_FILLVALVE_PREFAULT
	//Priority MAX
} MP_PAUSE_SOURCE_TYPE;

typedef enum POR_TIME_STATUS_ENUM
{
    POR_TIME_STATUS_SHORT = 0,
    POR_TIME_STATUS_LONG
}POR_TIME_STATUS_TYPE;

void MoisturePreventionMonitor__Initialize(void);
void MoisturePreventionMonitor__Execution(void);
void MoisturePreventionMonitor__Activate(MoisturePreventionSettingFileStructureType* SF);
void MoisturePreventionMonitor__Deactivate(void);
MP_STATUS_TYPE MoisturePreventionMonitor__GetStatus(void);
void MoisturePreventionMonitor__CycleStarted(void);
void MoisturePreventionMonitor__Resume(MP_PAUSE_SOURCE_TYPE source);
void MoisturePreventionMonitor__Pause(MP_PAUSE_SOURCE_TYPE source);

#endif /* SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_MOISTUREPREVENTIONMONITOR_MOISTUREPREVENTIONMONITOR_H_ */

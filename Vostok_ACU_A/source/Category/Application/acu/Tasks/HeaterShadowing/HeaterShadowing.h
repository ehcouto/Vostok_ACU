/**
 * @file		HeaterShadowing.h
 * @brief       Public interface to the Heater Shadowing Detection task module.
 *
 */

#ifndef SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_TASKS_HEATERSHADOWING_HEATERSHADOWING_H_
#define SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_TASKS_HEATERSHADOWING_HEATERSHADOWING_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//! Possible values of the variable HeaterShadowing_Status
typedef enum HEATER_SHADOWING_STATUS_ENUM
{
	ALGORITHM_NOT_ACTIVE = 0,
	ALGORITHM_ON_HOLD,
	HEATER_OFF,
	HEATER_NOT_SHADOWED,
	HEATER_SHADOWED,
	UNKNOWN_DIVERTER_POSITION,
	HEATER_SHADOWED_RISK

} HEATER_SHADOWING_STATUS_TYPE;

extern HEATER_SHADOWING_STATUS_TYPE HeaterShadowing_Status;
//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
ACTIVE_TYPE HeaterShadowing__Initialize(uint8 compartment, uint8 sf_index, uint8 displacement);
ACTIVE_TYPE HeaterShadowing__Task(uint8 compartment);
void HeaterShadowing__NVDataRestore(uint8 compartment);
void HeaterShadowing__Terminate(uint8 compartment);
void HeaterShadowing__StatusInitialize(void);
void HeaterShadowing__ResumeCycle(void);
void HeaterShadowing__CycleResumeEnd(uint8 compartment,uint8 subcycle_id);


#endif /* SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_TASKS_HEATERSHADOWING_HEATERSHADOWING_H_ */

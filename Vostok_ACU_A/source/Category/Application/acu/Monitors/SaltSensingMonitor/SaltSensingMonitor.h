/**
 * @file		SaltSensingMonitor.h
 * @brief       Public interface to the Salt Sensing monitor module.
 *
 */

#ifndef SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_SALTSENSINGMONITOR_SALTSENSINGMONITOR_H_
#define SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_SALTSENSINGMONITOR_SALTSENSINGMONITOR_H_

//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --

// -- Other Modules --

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//! Setting file data structure for pointer SF_PTR_ACU_ALGORITHM_PARAMETERS, displacement DSPL_WATER_SOFTENER
typedef PACKED struct WATER_SOFTENER_SF_PARAMS_STRUCT
{
	uint16 Delay_RV_On_FV_On; 	//! Time between turning on the regeneration valve and turning on the fill valve. Units = 200 ms
	uint16 Delay_FV_Off_RV_Off;	//! Time between turning off the fill valve and turning off the regeneration valve. Units = 200 ms
	uint8 N_Reg_Max_On; 		//! Number of regenerations to wait to turn on the Salt Empty alarm LED solid from when the sensor changes to low salt.
	uint8 N_Reg_Max_Blink; //! Number of regenerations to wait to turn on the Salt Empty alarm LED solid from when the sensor changes to low salt.
	uint8 Salt_Input_Stability_Time; //! Time that the signal from the salt sensor must be stable before it is considered valid. Units = 100 ms

	uint8 Do_Not_Reset_Backrinse_At_Fill	:1;
	uint8 Unused_b1b7						:7;
} WATER_SOFTENER_SF_PARAMS_TYPE;

//! States of Salt Sensor
typedef enum SALT_SENSOR_STATE_ENUM
{
	SALT_NOT_PRESENT			= 0,
	SALT_PRESENT				= 1,
	SALT_SENSOR_INVALID_VALUE	= 2
}SALT_SENSOR_STATE_TYPE;

//! States of Salt_Alarm_Led
typedef enum SALT_ALARM_LED_STATE_ENUM
{
    SALT_ALARM_LED_STATE_OFF 		= 0,
	SALT_ALARM_LED_STATE_SOLID_ON 	= 1,
	SALT_ALARM_LED_STATE_BLINK 		= 2
} SALT_ALARM_LED_STATE_TYPE;

extern SALT_ALARM_LED_STATE_TYPE Salt_Alarm_Led;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SaltSensingMonitor__Initialize(void);
void SaltSensingMonitor__Execution(void);
SALT_SENSOR_STATE_TYPE SaltSensingMonitor__GetSaltGIFiltered(void);
SALT_SENSOR_STATE_TYPE SaltSensingMonitor__GetPrevSaltGIFiltered(void);


#endif /* SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_SALTSENSINGMONITOR_SALTSENSINGMONITOR_H_ */

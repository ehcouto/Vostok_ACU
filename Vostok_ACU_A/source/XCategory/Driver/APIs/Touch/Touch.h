/**
 *  @file       Touch.h
 *  @brief      Touch Interface. Provides the public functions prototypes.
 *
 *  $Header: Touch.h 1.36 2016/12/05 13:19:59EST Bharath Kumar Midithuri (MIDITB) Exp  $
 *
 *  @copyright  Copyright 2012-$Date: 2016/12/05 13:19:59EST $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
#ifndef TOUCH_H_
    #define TOUCH_H_

#include "Touch_prm.h"

#ifndef TOUCH_FEATURE
    #define TOUCH_FEATURE   ENABLED
#endif

#if (TOUCH_FEATURE == ENABLED)
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
/**
 * Touch module state
 */
typedef enum
{
    TOUCH_IDLE = 0,     //!< TOUCH_IDLE - Can start a new conversion
    TOUCH_SCANNING,     //!< TOUCH_SCANNING - A scan is in progress
    TOUCH_DATA_READY,   //!< TOUCH_READ - Ready to read
    TOUCH_ERROR,        //!< TOUCH_ERROR - Error state
    TOUCH_DISABLE       //!< TOUCH_DISABLE - Touch is Disabled
}TOUCH_STATE_TYPE;

/**
 * Touch module to state to run. On default it run in Normal mode.
 */
typedef enum
{
    TOUCH_NORMAL_POWERMODE = 0, //!< Touch is in Normal Power mode
    TOUCH_LOW_POWERMODE        //!< Touch module is in Low Power Mode
}TOUCH_POWER_MODE_TYPE;

/**
 * Request to Touch module to execute the following request.
 */
typedef enum
{
	TOUCH_NO_REQUEST,
	TOUCH_CALIB_REQUEST,
	TOUCH_RESTART_REQUEST,
	TOUCH_LOWPOWERMODE_REQUEST,
	TOUCH_NORMALPOWERMODE_REQUEST,
	TOUCH_SENSORS_ENBDIS_REQUEST,
	TOUCH_SENSORS_DYNAMIC_CONFIG_REQUEST,
	TOUCH_CC_DYNAMIC_CALIB_REQUEST
}TOUCH_REQEST_COMMAND_TYPE;

/**
 * Opcodes to set/get touch parameters information. These options will be use by external applications like TouchAPI17 for
 * debugging and monitoring over wide/win/sercom
 */
typedef enum
{
	TOUCH_DYNAMIC_GLOBAL_DI = 0,
	TOUCH_DYNAMIC_GLOBAL_ATCH_DRIFT_RATE,
	TOUCH_DYNAMIC_GLOBAL_TCH_DRIFT_RATE,
	TOUCH_DYNAMIC_GLOBAL_MAX_ON_DURATION,
	TOUCH_DYNAMIC_GLOBAL_DRIFT_HOLD_TIME,
	TOUCH_DYNAMIC_GLOBAL_ATCH_RECAL_DELAY,
	TOUCH_DYNAMIC_GLOBAL_MUTLCAP_CAL_SEQ1_COUNT,
	TOUCH_DYNAMIC_GLOBAL_MUTLCAP_CAL_SEQ2_COUNT,
	TOUCH_DYNAMIC_GLOBAL_ATCH_RECAL_THRESHOLD,
	TOUCH_DYNAMIC_GLOBAL_POST_PROCESS_MODE,
	TOUCH_DYNAMIC_GLOBAL_FREQ_AUTO_TUNE_SIGNAL_STABILITY_LIMIT,
	TOUCH_DYNAMIC_GLOBAL_FREQ_AUTO_TUNE_IN_CNT,
	TOUCH_DYNAMIC_GLOBAL_NOISE_MEAS_SIGNAL_STABILITY_LIMIT,
	TOUCH_DYNAMIC_GLOBAL_NOISE_LIMIT,
	TOUCH_DYNAMIC_GLOBAL_LOCKOUT_SEL,
	TOUCH_DYNAMIC_GLOBAL_LOCKOUT_CNTDOWN,
	TOUCH_DYNAMIC_GLOBAL_FREQ_MODE,
	TOUCH_DYNAMIC_GLOBAL_CLK_PRESCALE,
	TOUCH_DYNAMIC_GLOBAL_SENSE_RESISTOR,
	TOUCH_DYNAMIC_GLOBAL_MUTLCAP_CC_CAL_CLK_PRESCALE,
	TOUCH_DYNAMIC_GLOBAL_MUTLCAP_CC_CAL_SENSE_RESISTOR,
	TOUCH_DYNAMIC_GLOBAL_FILTER_LEVEL,
	TOUCH_DYNAMIC_GLOBAL_AUTO_OS,
	TOUCH_DYNAMIC_GLOBAL_MUTLCAP_QUICK_REBURST_ENABLE,
	TOUCH_DYNAMIC_GLOBAL_FREQ_AUTO_TUNE_ENABLE,
	TOUCH_DYNAMIC_GLOBAL_NOISE_MEAS_ENABLE,
	TOUCH_DYNAMIC_GLOBAL_NOISE_MEAS_BUFFER_COUNT,
	TOUCH_DYNAMIC_GLOBALNUM_MOIS_TOLERANCE_STATUS,
	TOUCH_DYNAMIC_GLOBALNUM_NUM_MOIS_GROUPS,
	TOUCH_DYNAMIC_GLOBAL_FILTER_AVERAGE,
	TOUCH_DYNAMIC_GLOBAL_FILTER_GRASSCUT,
	TOUCH_DYNAMIC_GLOBAL_LOWPOWER_SENSORS_LIST,
	TOUCH_DYNAMIC_GLOBAL_TOUCH_DYNAMIC_GLOBAL_HAWKEYE_DEBUG,
	TOUCH_DYNAMIC_GLOBAL_DEF_HOP_FREQS,
	TOUCH_DYNAMIC_GLOBAL_AVG_FIR_FILTER_SAMPLES,
	TOUCH_DYNAMIC_GLOBAL_AVG_FIR_FILTER_MED_SIZE,
	TOUCH_DYNAMIC_GLOBAL_FIR_FILTER_SAMPLES,
	TOUCH_DYNAMIC_GLOBAL_FIR_FILTER_TAP_A_NUM,
	TOUCH_DYNAMIC_GLOBAL_FIR_FILTER_TAP_B_NUM,
	TOUCH_DYNAMIC_GLOBAL_CALIB_AUTO_TUNING,
	TOUCH_DYNAMIC_GLOBAL_TOUCH_MEASURE_SCAN_RATE,
	TOUCH_DYNAMIC_GLOBAL_FMEA_SCAN_RATE,
	TOUCH_DYNAMIC_GLOBAL_FMEA_GET_FAULT_STATUS,
	TOUCH_DYNAMIC_GLOBAL_FMEA_GET_TEST_NUM,
	TOUCH_DYNAMIC_GLOBAL_FMEA_GET_FAULT_CHANNEL_NUM,
	TOUCH_DYNAMIC_GLOBAL_FMEA_GET_FAULT_OPEN_CHANNEL_NUM,
	TOUCH_DYNAMIC_GLOBAL_MAX_TYPE
}TOUCH_DYNAMIC_GLOBAL_SETTING_TYPE;


typedef enum
{
	TOUCH_DYNAMIC_CHANNEL_X_LINE = 0,
	TOUCH_DYNAMIC_CHANNEL_Y_LINE,
	TOUCH_DYNAMIC_CHANNEL_THRESHOLD,
	TOUCH_DYNAMIC_CHANNEL_GAIN,
	TOUCH_DYNAMIC_CHANNEL_HYSTERESIS,
	TOUCH_DYNAMIC_CHANNEL_AKS,
	TOUCH_DYNAMIC_CHANNEL_MAX_TYPE
}TOUCH_DYNAMIC_CHANNEL_SETTING_TYPE;

typedef enum
{
	TOUCH_GETSTATE,
	TOUCH_GETERROR,
	TOUCH_GETCONFIGSTATE,
	TOUCH_RESTART,
	TOUCH_DISABLE_STATE,
	TOUCH_CALIBRATE,
	TOUCH_DYNAMIC_STATE_MAX_TYPE

}TOUCH_DYNAMIC_STATE_SETTING_TYPE;


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

void Touch__Initialize(void);
TOUCH_STATE_TYPE Touch__GetState(void);
unsigned char Touch__GetErrorCode(void);
void Touch__Disable(void);
void Touch__RestartRequest(void);
BOOL_TYPE Touch__RecalibrateRequest(void);
unsigned char * Touch__GetKeyMap(void);
BOOL_TYPE Touch__IsDataReady(void);
BOOL_TYPE Touch__IsMeasurementReady(void);
BOOL_TYPE Touch__IsMeasuirng(void);
BOOL_TYPE Touch__GetKeyStatus(unsigned char key);
void Touch__SlowHandler(void);
void Touch__LowPowerSlowHandler(void);
BOOL_TYPE Touch__IsLowPowerTouchSensorsPressed(void);
void Touch__FastHandler(void);
PASS_FAIL_TYPE Touch__SetSensorsEnableOrDisable(unsigned char *sensor_list,unsigned char Enb_Dis_Status,unsigned char size);

void Touch__SetLowPowerModeRequest(void);
void Touch__SetNormalPowerModeRequest(void);
TOUCH_POWER_MODE_TYPE Touch__GetTouchPowerMode(void);
unsigned char Touch__GetTouchLowPowerActiveSensor(void);
void Touch__SetTouchLowPowerActiveSensor(unsigned char sensor);


#if (TOUCH_KEYMAP_SUPPORT  == ENABLED)
unsigned char* Touch__GetKeyMapInfo(void);
BOOL_TYPE Touch__SetKeyMapInfo(unsigned char *keyMapArr,unsigned char MaxKeys);
#endif

unsigned short int Touch__GetThreshold(unsigned char key);
unsigned char Touch__GetHysteresis(unsigned char key);
unsigned short int Touch__GetReference(unsigned char key);
unsigned short int Touch__GetRaw(unsigned char key);
short int Touch__GetDelta(unsigned char key);
BOOL_TYPE Touch__IsValidConfiguration(void);
void Touch__SetHawkeyeStatus(BOOL_TYPE status);
unsigned char Touch__GetHawkeyeStatus(void);
unsigned short int * Touch__GetReferenceAddress(void);
unsigned short int * Touch__GetRawAddress(void);
unsigned short int * Touch__GetCalibCCAddress(void);

unsigned char Touch__GetHMIKeyEventDebounceTick(void);

short Touch__GetSliderDelta(unsigned char SliderIndex);
unsigned char Touch__GetSliderPosition(unsigned char SliderIndex);
unsigned char Touch__GetLastSliderPosition(unsigned char slider_index);
short Touch__GetWheelDelta(unsigned char WheelIndex);
unsigned char Touch__GetLastWheelPosition(unsigned char wheel_index);
unsigned char Touch__GetWheelPosition(unsigned char WheelIndex);
unsigned short Touch__GetWheelAngle(unsigned char WheelIndex);
unsigned char* Touch__GetGDMVersion(void);
unsigned char Touch__GetCalibCompleteState(void);
unsigned char Touch__GetCalibStartState(void);

unsigned char Touch__SetKeyParameters(unsigned char key, unsigned char threshold, unsigned char hysteresis);
unsigned char * Touch__GetKeyData(void);
unsigned char Touch__SetKeytoScan(unsigned char key);

BOOL_TYPE Touch__GetDynamicGlobalConfig(TOUCH_DYNAMIC_GLOBAL_SETTING_TYPE Config_Type,unsigned char *data);
BOOL_TYPE Touch__SetDynamicGlobalConfig(TOUCH_DYNAMIC_GLOBAL_SETTING_TYPE Config_Type, unsigned char *data);
unsigned char Touch__GetDynamicChannelEachProperty(TOUCH_DYNAMIC_CHANNEL_SETTING_TYPE Config_Type,unsigned char channel);
BOOL_TYPE Touch__SetDynamicChannelEachProperty(TOUCH_DYNAMIC_CHANNEL_SETTING_TYPE Config_Type, unsigned char channel,unsigned char *data);
unsigned char Touch__GetStateControl(TOUCH_DYNAMIC_STATE_SETTING_TYPE State_Type);
BOOL_TYPE Touch__SetStateControl(TOUCH_DYNAMIC_STATE_SETTING_TYPE State_Type, unsigned char *data);

unsigned char* Touch__GetHMIKeyDebounceKeysAddress(void);
BOOL_TYPE Touch__SetHMIKeyDebounceKeys(unsigned char *);

#endif
#endif // TOUCH_H_



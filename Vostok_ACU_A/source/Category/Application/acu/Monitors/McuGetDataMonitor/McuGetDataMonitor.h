/*
 * McuGetDataMonitor.h
 *
 */


#ifndef MCUGETDATAMONITOR_H
#define MCUGETDATAMONITOR_H


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void McuGetDataMonitor__Initialize(void);
void McuGetDataMonitor__Execution(void);
#if (API221_CLIENT_FEATURE == ENABLED)
sint16 McuGetDataMonitor__GetWashPumpSpeed(void);
sint16 McuGetDataMonitor__GetDrainPumpSpeed(void);
#endif /*(API221_CLIENT_FEATURE == ENABLED)*/
void McuGetDataMonitor__McuReset(uint8 node, uint8 reason, uint8 reset_mode);
#endif /* MCUGETDATAMONITOR_H */

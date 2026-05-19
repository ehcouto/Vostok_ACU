/*
 * @file		VwiZoneCompensationMonitor.h
 * @brief       Public interface to the Vwi Zone Compensation Monitor module.
 *
 */

#ifndef SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_VWIZONECOMPENSATIONMONITOR_VWIZONECOMPENSATIONMONITOR_H_
#define SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_VWIZONECOMPENSATIONMONITOR_VWIZONECOMPENSATIONMONITOR_H_

//-------------------------------------- Include Files ----------------------------------------------------------------
// -- This Module --
#include "SystemConfig.h"

// -- Other Modules --
#include "Vwi_prv.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
#if (VWI_THREE_PHASE_MOTOR_TYPE== ENABLED)
void VwiZoneCompensationMonitor__Initialize(void);
void VwiZoneCompensationMonitor__Execution(void);
#endif // (VWI_THREE_PHASE_MOTOR_TYPE== ENABLED)


#endif /* SOURCE_CATEGORY_APPLICATION_APPLICATIONDISH_MONITORS_VWIZONECOMPENSATIONMONITOR_VWIZONECOMPENSATIONMONITOR_H_ */

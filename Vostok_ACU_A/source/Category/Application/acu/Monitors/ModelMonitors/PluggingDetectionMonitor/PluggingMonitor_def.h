/**
 * @file
 * @brief       Public definition of the data structure used by the Plugging Monitor and supporting modules.
 *
 * @copyright   Copyright 2018 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef PLUGGINGMONITOR_DEF_H_
#define PLUGGINGMONITOR_DEF_H_

#include "SystemConfig.h"

//! Model Data
typedef struct PLUGGING_DATA_STRUCT
{
	/* Stores plugging risk state*/
    uint8 PluggingRisk;

    /* Used to count the plugging risk state */
    uint8 RiskCounter;

    /* Stores de Vwi Heavy Signal value last used in plugging risk*/
	uint16 VwiRiskLastValue;

	/* Flag to define when the diverter is considered stable */
	BOOL_TYPE Diverter_Stable;

	/* Timeout counter for diverter stability */
	uint8 Diverter_TimeoutCounter;

}PLUGGING_MODEL_DATA_TYPE;

//! Setting File Data
typedef PACKED struct PLUGGING_SF_DATA_STRUCT
{
	/* Variance threshold to be checked before getting into plugging risk zone */
	 	uint16 Plugging_VwiVarianceThreshold;

	/* Value to come back from Plugging Risk Zone to Normal state */
		uint16 Plugging_VwiSignalRecoverThreshold;

	/* Defines that any value below it can be considered plugged filter */
	 	uint16 Plugging_VwiSignalLowThreshold;

	/* Defines that any value below it can be considered plugging risk zone */
	 	uint16 Plugging_VwiSignalRiskThreshold;

	/* Number of times we want to detect the VWI signal is decreasing */
	 	uint8 Plugging_RiskMaxCount;

	/* Time (in seconds) to wait after diverter is moving to be safe for transition */
	 	uint8 Plugging_DiverterMovingTimeoutSec;

	/* Setting file filler - for a different size in SF struct to be able to check versions */
	 	uint16 empty;

}SF_PLUGGING_DATA_TYPE;

//! Plugging Status
typedef enum
{
    PL_UNDEFINED = 0,     /* Default value */
    PL_NO_PLUGGING,
    PL_PLUGGING
} PLUGGING_STATUS_TYPE;

//! Algorithm State Machine
typedef enum
{
	None = 0,     /* Default value */
	NORMAL,
	PLUGGING_RISK,
	PLUGGING
} PLUGGING_MONITORSTATE_TYPE;


#endif                                 /* PLUGGINGMONITOR_DEF_H_ */

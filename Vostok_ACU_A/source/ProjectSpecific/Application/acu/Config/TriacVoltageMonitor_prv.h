/*
 * TriacVoltageMonitor_prv.h
 *
 *  Created on: 11 mar 2022
 *      Author: WAZNYP
 */

#ifndef SOURCE_PROJECTSPECIFIC_APPLICATION_CONFIG_TRIACVOLTAGEMONITOR_PRV_H_
#define SOURCE_PROJECTSPECIFIC_APPLICATION_CONFIG_TRIACVOLTAGEMONITOR_PRV_H_

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --



#define TRIAC_V_DIAG_AC_LINE_FEATURE	(ENABLED)

#define GET_ACTUAL_VOLTAGE  	  		((uint16)Hbl__GetGIDataConvertedByIndex(Hbl__GetGIIndexByGIID(HBL_GI_AC_LINE))/100u)

#define AC_LINE_MEASUREMENT_PRESENT		(Hbl__GetGIIndexByGIID(HBL_GI_AC_LINE) != (uint8)HBL_INDEX_INVALID)

#define LOW_EDGE_110V_RANGE				((uint16)96)
#define HIGH_EDGE_110V_RANGE			((uint16)144)
#define LOW_EDGE_220V_RANGE				((uint16)176)    // todo: 187 per slide 16
#define HIGH_EDGE_220V_RANGE			((uint16)264)

#endif /* SOURCE_PROJECTSPECIFIC_APPLICATION_CONFIG_TRIACVOLTAGEMONITOR_PRV_H_ */

/**
 *  @file       
 *
 *  @brief      This module is responsible for managing the NVData(Non-Volatile) by the all types of expansion and HMI boards
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2022. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef API007NVDATA_H_
#define API007NVDATA_H_

#include "API007NVData_prm.h"


//ENABLE for ACU and non ACU boards which wanted to manage the NVData via API007
#ifndef API007DATA_NVDATA_FEATURE
    #define API007DATA_NVDATA_FEATURE               DISABLED
#endif

#if (API007DATA_NVDATA_FEATURE == ENABLED)
    #if ((SYSTEM_ACU_HANDLING == ENABLED) && ((SYSTEM_HMI_HANDLING == ENABLED) || (SYSTEM_HMI_EXPANSION_HANDLING == ENABLED)))  //P1 board or ACU+EXP HMI
        #error : "API007DATA_NVDATA_FEATURE shall be disabled for P1 boards"
    #endif

    #if (SYSTEM_ACU_HANDLING == ENABLED)
        #define API007NVDATA_SERVER_FEATURE         ENABLED
        #define API007NVDATA_CLIENT_FEATURE         DISABLED
    #else
        #define API007NVDATA_SERVER_FEATURE         DISABLED
        #define API007NVDATA_CLIENT_FEATURE         ENABLED
    #endif
#else
    #define API007NVDATA_SERVER_FEATURE         DISABLED
    #define API007NVDATA_CLIENT_FEATURE         DISABLED
#endif

#define API007NVDATA_SERVER_NODE_ID             WIN_ACU_ADDRESS
#define API007NVDATA_CLIENT_NODE_ID             WIN_NODE_DEF

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


/**
 * Return State meaning and application action
 *
 *
 *  API007NVDATA_DATA_RESTORE_STATUS_TYPE | Meaning                                                  | Client Application Recommended Action
 *  -----------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------
 *   API007NVDATA_RESTORE_CORRUPTED(0)    |  Data corruption                                         |   Application can now load their data with the default value
 *                                        |                                                          |
 *   API007NVDATA_RESTORE_OKAY(1)         |  Data Okay                                               |   Application can continue by relying the data
 *                                        |                                                          |
 *                                        |                                                          |
 *   API007NVDATA_RESTORE_IN_PROGRESSu(2) |  On every cold reset                                     |   It will stay in this stay until it finishes reading
 *                                        |  API007NVData tries to read the NVData from the client   |   Client Should wait until it completes its reading operation
 *                                        |                                                          |   This would take around 800ms for 65 bytes of data after the cold reset
 *                                        |                                                          |   If the data is big then it would take more time to read so application needs to make their own choices
 *                                        |                                                          |
 *   API007NVDATA_RESTORE_ERROR(3)        |  Engine Error                                            |   Their is some serious irrecoverable problem with the Client
 *                                        |                                                          |   Check logs to know the reason
 *                                        |                                                          |
 *                                        |                                                          |   Reasons and logs -
 *                                        |                                                          |   ERROR_INSUFFECIENT_API007NVDATA_CLIENT_NUM_BLOCKS,  -  make sure 'API007NVDATA_CLIENT_NUM_BLOCKS' has enough space
 *                                        |                                                          |   ERROR_UNABLE_TO_ALLOCATE_CLAIMED_SIZE_IN_SERVER, - make sure server has enough memory space to accommodate all clients claim for memory
 *                                        |                                                          |
 */

typedef enum
{
    API007NVDATA_RESTORE_CORRUPTED = 0,
    API007NVDATA_RESTORE_OKAY,
    API007NVDATA_RESTORE_IN_PROGRESS,

	API007NVDATA_RESTORE_ERROR,
}API007NVDATA_DATA_RESTORE_STATUS_TYPE;


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
API007NVDATA_DATA_RESTORE_STATUS_TYPE API007NVData__DataRestoreStatus(void);

#endif // API007NVDATA_H_



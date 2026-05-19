/**
 *  @file       
 *
 *  @brief      For more details search in Confluence page as ' How to configure NVData for non-ACU boards (API007NVData)'
 *
 *  @details
 *
 *      Definitions             Details
 *
 *      NVData              -   Non VOlatile Data, Data which can be retrieved even after power failure event
 *                              Project team shall be very cautions defining which data supposed to be NVData and not
 *
 *      Server              -   Management of the Data storing and restoring in the Non Volatile Data Memory
 *                              Performing the NVData Write/Read requests from the client
 *                              Here ACU shall be the Server (As in Nucleus only ACU knows how to store and retrieve the NVData)
 *
 *      Client              -   Management of requesting the NVData from the Server or Sending the NVData to the server to store
 *
 *      Client ID           -   Node ID itself is used as Client ID at the server side
 *
 *      Client Block Data   -   Client NVData memory is divided into blocks
 *                              These each block has its own block crc
 *                              data is continuously monitored to see the changes
 *                              Only that block is sent whose block crc changes, this way we can optimize sending data to the server
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2022. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef API007NVDATA_PRM_H_
#define API007NVDATA_PRM_H_

#include "SystemConfig.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief ENABLE for ACU and non ACU boards which wanted to manage the NVData via API007
 * @param    - ENABLED / DISABLED
 * @note     - "API007DATA_NVDATA_FEATURE       ENABLED"
 *           - "API007DATA_NVDATA_FEATURE       DISABLED
 */
#define API007DATA_NVDATA_FEATURE               DISABLED


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This should be the combined size of all clients + keep some buffer if in future if client needs more data to store
 * @details Applicable only to Server or ACU
 *
 * @param    - non zero value
 * @note     - "#define API007NVDATA_SERVER_DATA_SIZE     30"       30 bytes of data are reserved for all clients combined to store data
 *           - "#define API007NVDATA_SERVER_DATA_SIZE     1000"     1000 bytes of data are reserved for all clients combined to store data
 */
#define API007NVDATA_SERVER_DATA_SIZE                       60


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This should be the total number of clients in the Appliance
 * @details Applicable only to Server or ACU
 *
 * @param    - non zero value
 * @note     - "#define API007NVDATA_SERVER_NUM_CLIENTS_SUPPORT     1"        Only one client is present in the appliance
 *           - "#define API007NVDATA_SERVER_NUM_CLIENTS_SUPPORT     3"        Their are three clients in the appliance
 */
#define API007NVDATA_SERVER_NUM_CLIENTS_SUPPORT         1






//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Total number of block data in the client
 * @details Applicable only to client or non ACU boards
 *
 *          Formula to calculate this is API007NVDATA_CLIENT_NUM_BLOCKS = size of '_appliance_segment, and '_statistics_segment' / DATA_TRANSFER_SEGMENT_SIZE(65)
 *           '_appliance_segment, and '_statistics_segment' size can be easily found in .map file after compilation
 *
 *
 * @param    - non zero value
 * @note     - "#define API007NVDATA_CLIENT_NUM_BLOCKS     1"        This means that NVData size in client is less then 65 bytes
 *           - "#define API007NVDATA_CLIENT_NUM_BLOCKS     10"       This means that NVData size in client is between 585-650 bytes
 */
#define API007NVDATA_CLIENT_NUM_BLOCKS         1

#endif // API007NVDATA_PRM_H_

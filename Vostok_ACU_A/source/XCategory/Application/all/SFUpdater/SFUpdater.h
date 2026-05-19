/**
 *  @file       
 *
 *  @brief      This module manages the process to download the setting file section and displacement data from the ACU
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: SFUpdater.h 1.4 2015/07/01 13:51:13EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/07/01 13:51:13EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SFUPDATER_H_
#define SFUPDATER_H_

#include "C_Extensions.h"
#include "SystemConfig.h"
#include "SettingFile.h"
#include "SFUpdater_defs.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef enum
{
    SFUPDATER_REQUESTER_IDLE,               //! Ready to accept the new requests
    SFUPDATER_REQUESTER_PROCESSING,         //! Processing the Request
    SFUPDATER_REQUESTER_BUSY,               //! Busy in Processing different requests
    SFUPDATER_REQUESTER_REQUEST_DISCARDED,  //! Invalid request
    SFUPDATER_REQUESTER_DATA_READY          //! Data is ready to read and shall be read within 200ms
}SFUPDATER_REQUESTER_STATUS_TYPE;

PACKED typedef struct
{
    SETTINGFILE_OFFSET_TYPE SF_Pointer_ID;
    uint16 SF_Displacement;
    uint16 Size;
    uint16 Crc;
    uint8 *Data_Ptr;
}SFUPDATER_DISPLACEMENT_INFO_TYPE;

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   The below macro needs to be defined in SystemConfig.h file if the board is interested in handling the
 *          Displacement Data over Flash
 *
 * @details List the Displacements to be managed in the Flash memory
 *          By commenting the below macro it disables the Displacement Data Management
 *
 *          PACKED typedef struct
 *          {
 *              SETTINGFILE_OFFSET_TYPE SF_Pointer_ID;
 *              uint16 SF_Displacement;
 *          }SFUPDATER_DISPLACEMENT_LIST_TYPE;
 *
 * @param -
 *
 *          #define SFUPDATER_DISPLACEMENT_LIST_DEF     { \
 *                                                           {SF_PTR_UI_IO_CONFIG, 0x0001}, \
 *                                                           {SF_PTR_UI_IO_CONFIG, 0x0002}, \
 *                                                      }
 */

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SFUpdater__Initialize(void);
BOOL_TYPE SFUpdater__Handler(void);

//! Below API's are used to manage displacement data over RAM memory
SFUPDATER_REQUESTER_STATUS_TYPE SFUpdater__ReadSFDisplacementInfoRAM(SFUPDATER_DISPLACEMENT_INFO_TYPE *sf_displacement_info);
SFUPDATER_REQUESTER_STATUS_TYPE SFUpdater__ReadSFDisplacementDataRAM(SFUPDATER_DISPLACEMENT_INFO_TYPE *sf_displacement_info);

//! Below API is used to read the Displacement Information which is stored in the Flash
BOOL_TYPE SFUpdater__ReadSFDisplacementInfoFLASH(SFUPDATER_DISPLACEMENT_INFO_TYPE *sf_displacement_info);

#endif // SFUPDATER_H_



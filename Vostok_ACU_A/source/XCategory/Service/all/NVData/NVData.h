/**
 *  @file
 *
 *  @brief		Interface for the Non-Volatile Data management module
 *  @details	Manages storage and recovery of non-volatile data.
 *  			In Nucleus Architecture only ACU has the capability to manage the Non-Volatile data
 *
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef NVDATA_H_
#define NVDATA_H_

#include "C_Extensions.h"
#include "SystemConfig.h"

#include "NVData_Def.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

typedef enum
{
    NVDATA_RESTORE_CORRUPTED,
    NVDATA_RESTORE_OKAY,
    NVDATA_RESTORE_OKAY_BACKUP
}NVDATA_RESTORE_TYPE;


typedef union
{
    uint8 Byte;
    struct
    {
        NVDATA_RESTORE_TYPE Appliance_Data_Recovered      :2;
        NVDATA_RESTORE_TYPE Statistics_Data_Recovered     :2;
        uint8 Reserved                      :4;
    } Bit;
} NVDATA_RESTORE_STATUS_TYPE;

#define NVDATA_RESTORE_OK   1

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void NVData__Initialize(void);

#if (NVDATA_STORE_APPLIANCE_AND_STATISTICS_FEATURE == ENABLED)
    void NVData__Handler(void);
    void NVData__BronwOut(BOOL_TYPE bout);
    NVDATA_RESTORE_STATUS_TYPE * NVData__GetLastRestoreStatus(void);
    PASS_FAIL_TYPE NVData__ForceBlockingStore(void);
    PASS_FAIL_TYPE NVData__SetApplianceSegmentCorrupt(void);
#endif

#if (NVDATA_STORE_STATIC_FEATURE == ENABLED)
    PASS_FAIL_TYPE NVData__StoreStaticData(void);
#endif

#endif // NVDATA_H_



/**
 *  @file       
 *
 *  @brief      This module provides an way to manage the Bulk Data transfer.
 *
 *              This file is an template and project team needs to add their project specific contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *
 *  @copyright  Copyright 2013-$Date: 2013/09/13 06:18:50EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef API007BULKDATA_H_
#define API007BULKDATA_H_

#include "API007Data.h"

#if (API007DATA_BULK_DATA_FEATURE == ENABLED)
#include "API007App.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void API007BulkDataMgr__Initialize(void);
void API007BulkDataMgr__Handler(void);
API007DATA_ERROR_TYPE API007BulkDataMgr__ReadErrorType(API007APP_SEGMENT_ID segment_id);
void API007BulkDataMgr__ClearErrorType(API007APP_SEGMENT_ID segment_id);

API007_SEGMENT_INFO_TYPE * API007BulkDataMgr__GetSegmentInfo(API007APP_SEGMENT_ID segment_id);
uint8 API007BulkDataMgr__GetSegmentListSize(void);
API007APP_SEGMENT_ID * API007BulkDataMgr__GetSegmentList(void);
API007_SEGMENT_INFO_TYPE * API007BulkDataMgr__ReadReceivedSegmentInfo(API007APP_SEGMENT_ID segment_id);


void API007BulkDataMgr__WriteSegmentList(uint8 address,uint8 * data, uint8 size);
void API007BulkDataMgr__WriteDetailInfo(uint8 address, API007APP_SEGMENT_ID segment_id,API007_SEGMENT_ROLE_TYPE role, uint16 size, uint16 crc);
void API007BulkDataMgr__WriteErrorType(uint8 address, API007APP_SEGMENT_ID segment_id, API007DATA_ERROR_TYPE error_type);
void API007BulkDataMgr__WriteSegmentData(uint8 address,API007APP_SEGMENT_ID segment_id,uint16 index, uint8 * data, uint8 size);
void API007BulkDataMgr__AckWriteComplete(uint8 address);

BOOL_TYPE API007BulkDataMgr__GetSegmentData(uint8 node, API007APP_SEGMENT_ID segment_id);
void API007BulkDataMgr__ReceivedWriteCompleted(uint8 node);
void API007BulkDataMgr__ReceivedTransmissionCompleted(uint8 node);

#endif //(API007DATA_BULK_DATA_FEATURE == ENABLED)
#endif // API007BULKDATA_H_

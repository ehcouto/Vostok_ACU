/**
 *  @file       
 *
 *  @brief      This file provides the API interface for the FITMgr module
 *
 *  @details    The FITMgr(Flash Integrity Table) is used to verify the integrity of the Flash code by using the FIT Table
 *
 *              FIT Table as below for 3 blocks-
 *
 *               ------------------------------------------------------------------------------------------
 *              |               |  START_ADDRESS      |         END_ADDRESS      |   INTIGRATION_ADDRESS   |
 *              | HEADER_BLOCK  |---------------------|--------------------------|-------------------------|
 *              |               |  0xBAADF00D         |         0xBAADF00D       |      0xBAADF00D         |
 *              |---------------|---------------------|--------------------------|-------------------------|
 *              |               |  START_ADDRESS      |         END_ADDRESS      |   INTIGRATION_ADDRESS   |
 *              |  BLOCK 0      |---------------------|--------------------------|-------------------------|
 *              |               |  0xFFFFFFFF         |         0xFFFFFFFF       |      0xFFFFFFFF         |
 *              |---------------|---------------------|--------------------------|-------------------------|
 *              |               |  START_ADDRESS      |         END_ADDRESS      |   INTIGRATION_ADDRESS   |
 *              |  BLOCK 1      |---------------------|--------------------------|-------------------------|
 *              |               |  0xFFFFFFFF         |         0xFFFFFFFF       |      0xFFFFFFFF         |
 *              |---------------|---------------------|--------------------------|-------------------------|
 *              |               |  START_ADDRESS      |         END_ADDRESS      |   INTIGRATION_ADDRESS   |
 *              |  BLOCK 2      |---------------------|--------------------------|-------------------------|
 *              |               |  0xFFFFFFFF         |         0xFFFFFFFF       |      0xFFFFFFFF         |
 *              |---------------|---------------------|--------------------------|-------------------------|
 *              |               |  START_ADDRESS      |         END_ADDRESS      |   INTIGRATION_ADDRESS   |
 *              | FOOTER_BLOCK  |---------------------|--------------------------|-------------------------|
 *              |               |  0xDEADBEEF         |         0xDEADBEEF       |      0xDEADBEEF         |
 *               ------------------------------------------------------------------------------------------
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: FITMgr.h 1.7 2015/02/19 05:30:12EST Dasar Hanumanth (DASARHB) Exp  $
 *
 *  @copyright  Copyright 2014-$Date: 2015/02/19 05:30:12EST $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef FITMGR_H_
#define FITMGR_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
typedef enum
{
    FITMGR_STATUS_IDLE,
    FITMGR_STATUS_BUSY,
    FITMGR_STATUS_ACCEPTED,
    FITMGR_STATUS_NOT_PROCESSED,
    FITMGR_STATUS_VALID,
    FITMGR_STATUS_INVALID,
    FITMGR_STATUS_INVALID_TABLE,
    FITMGR_STATUS_INVALID_BLOCK
}FITMGR_STATUS_TYPE;

typedef enum
{
    FITMGR_HANDLER_STATUS_IDLE,
    FITMGR_HANDLER_STATUS_BUSY,
    FITMGR_HANDLER_STATUS_ACCEPTED
}FITMGR_HANDLER_STATUS_TYPE;

// Define the address type
typedef uint32  FITMGR_ADDRESS_TYPE;

// Define the Block type
 typedef struct
{
    FITMGR_ADDRESS_TYPE START_ADDRESS;
    FITMGR_ADDRESS_TYPE END_ADDRESS;
    FITMGR_ADDRESS_TYPE INTIGRITY_ADDRESS;
}FITMGR_BLOCK_TYPE;

// HEADER_BLOCK Shall composed of FIT_HEADER
#define FIT_HEADER          ((FITMGR_ADDRESS_TYPE)0xBAADF00DU)

// FOOTER_BLOCK Shall composed of FIT_FOOTER
#define FIT_FOOTER          ((FITMGR_ADDRESS_TYPE)0xDEADBEEFU)

// Keyword to use if CRC check of that block should not be performed
// NOTE: This keyword cannot be used in the block that contain FIT table, it's expected to be the main application block.
//       If that block contain this keyword, IAP will not consider it valid and would not start Application.
#define FIT_SKIP_CRC        ((FITMGR_ADDRESS_TYPE)0xFFFFFFFFU)

//All the FIT Table shall start with the HEADER_BLOCK
//                        START_ADDRESS,  END_ADDRESS,    CRC_ADDRESS
#define HEADER_BLOCK    { FIT_HEADER,     FIT_HEADER,     FIT_HEADER }


//All the FIT Table shall end with the FOOTER_BLOCK
//                        START_ADDRESS,  END_ADDRESS,    CRC_ADDRESS
#define FOOTER_BLOCK    { FIT_FOOTER,     FIT_FOOTER,     FIT_FOOTER }

#include "FITMgr_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void FITMgr__Initialize(void);
void FITMgr__Handler(void);

FITMGR_STATUS_TYPE FITMgr__StartTableValidation(FITMGR_TABLE_TYPE table);
FITMGR_STATUS_TYPE FITMgr__StartBlockValidation(FITMGR_TABLE_TYPE table, FITMGR_ADDRESS_TYPE block_address);

FITMGR_STATUS_TYPE FITMgr__StartTableValidationByRef(uint32 * table_ptr);
FITMGR_STATUS_TYPE FITMgr__StartBlockValidationByRef(FITMGR_ADDRESS_TYPE block_address);

FITMGR_STATUS_TYPE FITMgr__StartTableValidationBlocking(FITMGR_TABLE_TYPE table);
FITMGR_STATUS_TYPE FITMgr__StartBlockValidationBlocking(FITMGR_TABLE_TYPE table, FITMGR_ADDRESS_TYPE block_address);

FITMGR_HANDLER_STATUS_TYPE FITMgr__GetHandlerStatus(void);
FITMGR_BLOCK_TYPE * FITMgr__GettBlockInfo(FITMGR_TABLE_TYPE table, FITMGR_ADDRESS_TYPE block_address);

FITMGR_STATUS_TYPE FITMgr__GetTableStatus(FITMGR_TABLE_TYPE table);
FITMGR_STATUS_TYPE FITMgr__GetBlockStatus(FITMGR_TABLE_TYPE table, FITMGR_ADDRESS_TYPE block_address);

#endif // FITMGR_H_



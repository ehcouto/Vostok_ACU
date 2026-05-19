/**
 *  @file       
 *
 *  @brief      List the programming block in the FIT_APPLICATION_TABLE
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: FITMgr_prv.h 1.2 2015/01/12 12:09:48EST Dasar Hanumanth (DASARHB) Exp  $
 *
 *  @copyright  Copyright 2014-$Date: 2015/01/12 12:09:48EST $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef FITMGR_PRV_H_
#define FITMGR_PRV_H_

//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------

/**
 * @brief   - List the programming block in the below table.
 * @details - This allows block programming. If the Block is not registered then it can not be programmed.
 *            Each block should have its own CRC
 *
 *            The blocks could be - Application Firmware, Setting File etc
 *
 *
 *            Should match the FIT table format
 *            FIT Table format as below for 3 blocks-
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
 */
// Below symbols are exported from the linker file (config.icf)
extern uint32  APPL_BLOCK_1_START;
extern uint32  APPL_BLOCK_1_END;
//extern uint32  APPL_BLOCK_1_CRC_ADDR;

extern uint32  MICRO_FLASH_CRC;

#define FIT_APPLICATION_BLOCK_START_ADDRESS       ((uint32)&APPL_BLOCK_1_START)      // This symbol comes from the linker file
#define FIT_APPLICATION_BLOCK_END_ADDRESS         ((uint32)&APPL_BLOCK_1_END)        // This symbol comes from the linker file
#define FIT_APPLICATION_BLOCK_CRC_ADDRESS         ((uint32)&MICRO_FLASH_CRC)   // This symbol comes from the linker file

#pragma location = "FIT_APPL_TABLE_REGION"
__root const FITMGR_BLOCK_TYPE FIT_APPLICATION_TABLE[] =
{   //START_ADDRESS,                               END_ADDRESS,                              CRC_ADDRESS
    HEADER_BLOCK,
    {FIT_APPLICATION_BLOCK_START_ADDRESS,          FIT_APPLICATION_BLOCK_END_ADDRESS,        FIT_APPLICATION_BLOCK_CRC_ADDRESS},
    FOOTER_BLOCK
};

#endif // FITMGR_PRV_H_

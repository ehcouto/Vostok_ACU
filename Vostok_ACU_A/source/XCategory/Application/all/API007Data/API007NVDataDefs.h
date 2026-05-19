/**
 *  @file       
 *
 *  @brief      This API's are not for Application access, are managed via API007Data module
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2022. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef API007NVDATADEFS_H_
#define API007NVDATADEFS_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================



//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
#if (API007DATA_NVDATA_FEATURE == ENABLED)
    void API007NVData__Initialize(void);
    void API007NVData__Handler(void);

    void API007NVData__ProcessClaimSize(uint8 client_node_id, uint16 size);
    void API007NVData__ProcessReadCmd(uint8 client_node_id, uint16 index, uint16 size);
    void API007NVData__ProcessWriteCmd(uint8 client_node_id, uint16 index ,uint16 size, uint8 * data_ptr);

    void API007NVData__ReceivedClaimSize(uint16 size);
    void API007NVData__ReceivedRead(uint16 index ,uint16 size, uint8 * data_ptr);
    void API007NVData__ReceivedWrite(uint16 index);
#endif


#endif // API007NVDATADEFS_H_



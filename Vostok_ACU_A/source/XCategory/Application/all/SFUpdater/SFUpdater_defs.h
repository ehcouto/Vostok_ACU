/**
 *  @file
 *
 *  @brief      Basic description of file contents
 *
 *  @copyright  Copyright 2016-2017 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SFUPDATER_DEFS_H
	#define SFUPDATER_DEFS_H

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   The below macro needs to be defined in SystemConfig.h file if the board is interested in handling the
 *          Displacement Data over Flash
 *
 * @details List the Displacements to be managed in the Flash memory
 *          By commenting the below macro it disables the Displacement Data Management
 *
 *
 **/

PACKED typedef struct
{
    SETTINGFILE_OFFSET_TYPE SF_Pointer_ID;
    uint16 SF_Displacement;
}SFUPDATER_DISPLACEMENT_LIST_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void SFUpdater__RxMaxPackage(uint8 node, uint8 max_payload);

void SFUpdater__RxSFInfo(uint8 node, SF_ID_TYPE sf_id, uint16 section_crc, uint16 section_size);
void SFUpdater__RxSFData(uint8 node, uint8 size, uint16 data_index, uint8 * data_ptr);

void SFUpdater__RxSFDisplacementInfo(SETTINGFILE_OFFSET_TYPE sf_pointer_id, uint16 sf_displacement, uint16 sf_displacement_crc, uint8 sf_displacement_size);
void SFUpdater__RxSFDisplacementData(SETTINGFILE_OFFSET_TYPE sf_pointer_id, uint16 sf_displacement, uint16 data_index, uint8 sf_displacement_size, uint8 * data_ptr);

void SFUpdater__ValidateFlashDisplacementData(void);

#endif // SFUPDATER_DEFS_H



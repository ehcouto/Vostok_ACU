/**
 *  @file
 *
 *  @brief      Public API for APII019 ProductInfo Module.
 *
 *  @copyright  Copyright 2015-2017 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
#ifndef PRODUCTINFO_H_
    #define PRODUCTINFO_H_

//  --- Include Files -------------------------------------------------------------------------------------------------
#include "SystemConfig.h"
#include "API019ProductInfo_Defs.h"

//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================
// -- Public Type Declarations --
//! List the boards on this WIN Node
//! Always contains the main board and may also list daughter boards.
//! The main board shall collect (via SPI, I2C, USB, etc) and report daughter boards API019 board/project information.
typedef enum
{
    API019_BOARD_MAIN = 0,     //!< PRODUCTINFO_BOARD_MAIN
    // API019_BOARD_DAUGHTER,     //!< List daughter boards here (if exist)
    API019_NUM_BOARDS          //!< PRODUCTINFO_NUM_BOARDS
} API019_BOARD_TYPE;

//---------------------------------------------------------------------------------------------------------------------
//! Macro to inform application of appliance data being set by Reveal.
#define API019_APPLIANCE_DATA_SET()

//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================
API019_PROJECT_DATA_TYPE *  ProductInfo__GetProjectData (API019_BOARD_TYPE board, API019_PROJECT_TYPE project);
uint8                       ProductInfo__GetNumProjects (API019_BOARD_TYPE board);
API019_PROJECT_TYPE *       ProductInfo__GetProjectIDs  (API019_BOARD_TYPE board);
void                        ProductInfo__BoardDataFeedback(uint8 node_id,uint8 board ,API019_BOARD_DATA_TYPE *board_data, API019_PROJECT_TYPE *project_ids);
void                        ProductInfo__ProjectDataFeedback(uint8 node_id,uint8 board,API019_PROJECT_TYPE project,API019_PROJECT_DATA_TYPE *project_data);
#endif // PRODUCTINFO_H_

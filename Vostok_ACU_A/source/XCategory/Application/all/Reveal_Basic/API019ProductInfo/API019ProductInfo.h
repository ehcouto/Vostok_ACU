/**
 *  @file
 *
 *  @brief      Public interface for the API019ProductInfo module.
 *
 *  @copyright  Copyright 2015-2017 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
#ifndef API019PRODUCTINFO_H_
    #define API019PRODUCTINFO_H_

//  --- Include Files -------------------------------------------------------------------------------------------------
#include "SystemConfig.h"

#ifndef API019PRODUCTINFO_FEATURE
    #define API019PRODUCTINFO_FEATURE      ENABLED
#endif

#if (API019PRODUCTINFO_FEATURE == ENABLED)
    #include "API019ProductInfo_Defs.h"
    #include "Reveal.h"
    #include "ProductInfo.h"

    //=====================================================================================================================
    //  --- Public Properties ---------------------------------------------------------------------------------------------
    //=====================================================================================================================
    // -- Public Constant Declarations --
    #define API019PRODUCTINFO_NUM           0x13 //(19)
    #define API019PRODUCTINFO_TYPE          0x5
    #define API019PRODUCTINFO_VERSION       0x0
    #define API019PRODUCTINFO_INSTANCES     1

    // -- Public Type Declarations --

    //=====================================================================================================================
    //  --- Public Methods ------------------------------------------------------------------------------------------------
    //=====================================================================================================================
    void                        API019ProductInfo__Initialize               (void);
    REVEAL_RECIPE_STATUS_TYPE   API019ProductInfo__CommandParser            (REVEAL_MSG_TYPE * buffer);
    void                        API019ProductInfo__FeedbackParser           (REVEAL_MSG_TYPE * buffer);
    void                        API019ProductInfo__AckParser                (uint8 address, uint8 api,
                                                                             uint8 opcode,
                                                                             REVEAL_RECIPE_STATUS_TYPE reasoncode);
    void                        API019ProductInfo__Handler25ms              (void);
    API019_SERVICE_DATA_TYPE *  API019ProductInfo__GetServiceDataReference  (void);
    API019_TEST_DATA_TYPE *     API019ProductInfo__GetEOLDataReference      (uint8 index);
    API019_TEST_DATA_TYPE  const * API019ProductInfo__GetFVTDataReference   (uint8 index);

    API019_APPLIANCE_DATA_TYPE const *  API019ProductInfo__GetApplianceData (void);
    PASS_FAIL_TYPE                      API019ProductInfo__SetApplianceData (API019_APPLIANCE_DATA_TYPE const * appliance_data);
    API019_BOARD_DATA_TYPE const *      API019ProductInfo__GetBoardData     (uint8 board_type);

    PASS_FAIL_TYPE              API019ProductInfo__RequestBoardData         (uint8 node_address, uint8 board_type);
    PASS_FAIL_TYPE              API019ProductInfo__RequestProjectData       (uint8 node_address, uint8 board_type, API019_PROJECT_TYPE project_id);


#endif //API019PRODUCTINFO_FEATURE

#endif // API019PRODUCTINFO_H_

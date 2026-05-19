/**
 * @brief       Public interface to the API004Debug module.
 *
 * @details     Refer to the API004Debug.c source file for more detailed information.
 *
 * @copyright   Copyright 2021 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef API004DEBUG_H_
#define API004DEBUG_H_

#include "API004Debug_defs.h"

#include "Reveal.h"
//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

#define API004DEBUG_NUM        4
#define API004DEBUG_TYPE       1
#define API004DEBUG_VERSION    3
#define API004DEBUG_INSTANCES  1

/**
 * Function ID for SW05 test is fixed
 */
#define FIXED_ARGS_FUNCTION_ID_SW05SOFTWARE    255

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
#if (API004DEBUG_FEATURE == ENABLED)
    REVEAL_RECIPE_STATUS_TYPE API004Debug__CommandParser(REVEAL_MSG_TYPE * buffer);
    void API004Debug__SendFbkForFixedArgsFunctions(uint8 destin_node_id, uint8 length, BOOL_TYPE set_mmp, uint8 *buffer_ptr);
#endif //#if (API004DEBUG_FEATURE == ENABLED)
#endif // API004DEBUG_H_



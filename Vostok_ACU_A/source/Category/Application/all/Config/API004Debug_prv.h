/**
 * @file
 * @brief       Private parameters for the API004Debug module.
 *
 * @details     This file is used to customized the Functions List to be executed.
 * @copyright   Copyright 2021 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef API004DEBUG_PRV_H_
#define API004DEBUG_PRV_H_


//  --- Include Files -------------------------------------------------------------------------------------------------
#if (API004DEBUG_FEATURE == ENABLED)

//#include "Micro.h"


//=====================================================================================================================
//  --- Private Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Private Constant Declarations --

// -- Private Enumerated Constant Declarations --

// -- Private Type Declarations --


// -- Private Flash-Based Constant Definitions --

//#define DEBUG_APIS_LIST     {           /*Label*/           /*Function Pointer*/         /*Parameters Signature*/\
//                               { "Micro__ForceReset",   (void*)Micro__ForceReset,        PARAM1(DATATYPE_UINT8)},\
//                            }



/*
 * Label - Is a character string representing the function pointer
 * Function Pointer - This should be of type DEBUG_FIXED_ARGS_FUNCTION_HANDLER_TYPE
 */
//#define DEBUG_FIXED_ARGS_APIS_LIST  \
//{      /*Label*/                    /*Function Pointer*/ \
//   { "DebugFixedArg__Execute",   (void*)DebugFixedArg__Execute},\
//}

//=====================================================================================================================
//  --- Private Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Private Macro Declarations --



#endif //(API004DEBUG_FEATURE == ENABLED)
#endif      // API004DEBUG_PRV_H_

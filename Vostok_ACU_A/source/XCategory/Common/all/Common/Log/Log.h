/**
 *  @file
 *  @brief      Public interface to the Log module.
 *
 *  $Header: Log.h 1.1 2015/06/03 14:33:00EDT Nelson Ferragut II (FERRANJ) Exp  $
 *
 *  @copyright  Copyright 2013 - $Date: 2015/06/03 14:33:00EDT $  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
#ifndef LOG_H_
#define LOG_H_

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --
#include "Log_defs.h"
#include "Log_prm.h"

// -- Other Modules --


//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Compile Directive Defaults --

// Force a declaration of LOG_MODULE_ENABLE if there isn't one.
#ifndef LOG_MODULE_ENABLE
    #define LOG_MODULE_ENABLE ENABLED
#endif

// -- Type Declarations --

#if (LOG_MODULE_ENABLE == ENABLED)

    /**
     * The data type used to hold general numeric information assigned to individual Log Messages.
     * Specified as a typedef so that it can be changed more easily in the future if necessary.
     */
    typedef unsigned short LOG_DATA_TYPE;


    /**
     * The data type used to hold the Message Identifier assigned to individual Log Messages.
     * Specified as a typedef so that it can be changed more easily in the future if necessary.
     */
    typedef uint8 LOG_MESSAGE_ID_TYPE;


    //=====================================================================================================================
    //  --- Public Methods ------------------------------------------------------------------------------------------------
    //=====================================================================================================================


    // Primary public functions for the Log module.
    void Log__Initialize(void);
    void Log__Background(void);
    void Log__AddMessage(LOG_MODULE_ID_TYPE module_id, LOG_MESSAGE_ID_TYPE message_id, LOG_SEVERITY_ID_TYPE severity_id, LOG_DATA_TYPE data);
    void Log__SetMessageSeverityFilter(LOG_SEVERITY_FILTER_TYPE severities_allowed);
    LOG_SEVERITY_FILTER_TYPE Log__GetMessageSeverityFilter(void);

    // Macros to simplify environment interface.
    /**
     * Macro for adding a Log Message with LOG_INFO Message Severity.
     *
     * Usage of this macro requires definition of 'MODULE_NAME' in the environment module's
     * private scope.
     *
     * @image html "log_sq_add_info_api.png"
     *
     * @param msg = Message Identifier value to use in the 'Id' field of the Log Message.
     * @param dat = Data value to use in the 'Data' field of the Log Message.
     */
    #define LOG_ADD_INFO(msg, dat) \
        Log__AddMessage(MODULE_NAME, (msg), LOG_INFO,        (LOG_DATA_TYPE)(dat))

    /**
     * Macro for adding a Log Message with LOG_EXCEPTION Message Severity.
     *
     * Usage of this macro requires definition of 'MODULE_NAME' in the environment module's
     * private scope.
     *
     * @image html "log_sq_add_exception_api.png"
     *
     * @param msg = Message Identifier value to use in the 'Id' field of the Log Message.
     * @param dat = Data value to use in the 'Data' field of the Log Message.
     */
    #define LOG_ADD_EXCEPTION(msg, dat) \
        Log__AddMessage(MODULE_NAME, (msg), LOG_EXCEPTION,   (LOG_DATA_TYPE)(dat))

    /**
     * Macro for adding a Log Message with LOG_EVENT Message Severity.
     *
     * Usage of this macro requires definition of 'MODULE_NAME' in the environment module's
     * private scope.
     *
     * @image html "log_sq_add_event_api.png"
     *
     * @param msg = Message Identifier value to use in the 'Id' field of the Log Message.
     * @param dat = Data value to use in the 'Data' field of the Log Message.
     */
    #define LOG_ADD_EVENT(msg, dat) \
        Log__AddMessage(MODULE_NAME, (msg), LOG_EVENT,       (LOG_DATA_TYPE)(dat))

    /**
     * Macro for adding a Log Message with LOG_ERROR Message Severity.
     *
     * Usage of this macro requires definition of 'MODULE_NAME' in the environment module's
     * private scope.
     *
     * @image html "log_sq_add_error_api.png"
     *
     * @param msg = Message Identifier value to use in the 'Id' field of the Log Message.
     * @param dat = Data value to use in the 'Data' field of the Log Message.
     */
    #define LOG_ADD_ERROR(msg, dat) \
        Log__AddMessage(MODULE_NAME, (msg), LOG_ERROR,       (LOG_DATA_TYPE)(dat))

#else   // #if (LOG_MODULE_ENABLE == ENABLE)

    // Replace all public methods with empty macros to remove application code.
    #define Log__Initialize()
    #define Log__Background()

    // Primary public functions for the Log module.
    #define Log__AddMessage(mod, msg, sev, dat)
    #define Log__SetMessageSeverityFilter(severities_allowed)
    #define Log__GetMessageSeverityFilter()                         LOG_ALLOW_NONE

    // Macros to simplify environment interface.
    #define LOG_ADD_INFO(msg, dat)
    #define LOG_ADD_EXCEPTION(msg, dat)
    #define LOG_ADD_EVENT(msg, dat)
    #define LOG_ADD_ERROR(msg, dat)

#endif  // #if (LOG_MODULE_ENABLE == ENABLE)

#endif  // LOG_H_


/**
 *  @file
 *  @brief      Core declarations required by the parameter files.
 *
 *  $Header: Log_defs.h 1.1 2015/06/03 14:33:01EDT Nelson Ferragut II (FERRANJ) Exp  $
 *
 *  @copyright  Copyright 2013 - $Date: 2015/06/03 14:33:01EDT $  Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */
#ifndef LOG_DEFS_H_
#define LOG_DEFS_H_

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --


//=====================================================================================================================
//  --- Public Parameters ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Public Constant Declarations --

// -- Public Type Declarations --

//! Severities that are assigned to individual Log Messages.
typedef enum LOG_SEVERITY_ID_ENUM
{
    /**
     * Use LOG_INFO severity Log Messages to report debugging information of normal operation
     * within a single module.
     *
     * Example: Report the state of a module each time that the state changes.
     */
    LOG_INFO = 0,

    /**
     * Use LOG_EXCEPTION severity Log Messages to report unexpected situations that are most
     * likely due to defects in the source code.
     *
     * Example: Report that a parameter to a public function was discovered to be out of range.
     */
    LOG_EXCEPTION,

    /**
     * Use LOG_EVENT severity Log Messages to report debugging information of normal transactions
     * between project modules.
     *
     * Example: The module that senses a door position may report that the door just opened/closed.
     */
    LOG_EVENT,

    /**
     * Use LOG_ERROR severity Log Messages to report that the module was not able to do what it
     * was designed to do because of some system limitation. This is not a software defect. This
     * is not necessarily a service error.
     *
     * Example: The module that senses the lock position cannot sense the lock position because of
     * a hardware fault.
     */
    LOG_ERROR
} LOG_SEVERITY_ID_TYPE;


/**
 * Filters used to specify a set of Message Severities.
 * A unique combination of Message Severities can be set by adding multiple enumerated values.
 *
 * Example: A filter to allow EVENTS and ERRORS is (#LOG_ALLOW_EVENT | #LOG_ALLOW_ERROR).
 */
typedef enum LOG_SEVERITY_FILTER_ENUM
{
    //! This module cannot log any messages.
    LOG_ALLOW_NONE      = 0,

    //! This module can log messages with (Message Severity == LOG_INFO).
    LOG_ALLOW_INFO      = (1U << LOG_INFO),

    //! This module can log messages with (Message Severity == LOG_EXCEPTION).
    LOG_ALLOW_EXCEPTION = (1U << LOG_EXCEPTION),

    //! This module can log messages with (Message Severity == LOG_EVENT).
    LOG_ALLOW_EVENT     = (1U << LOG_EVENT),

    //! This module can log messages with (Message Severity == LOG_ERROR).
    LOG_ALLOW_ERROR     = (1U << LOG_ERROR),

    //! This module can log any message.
    LOG_ALLOW_ALL       = (LOG_ALLOW_INFO + LOG_ALLOW_EXCEPTION + LOG_ALLOW_EVENT + LOG_ALLOW_ERROR)
} LOG_SEVERITY_FILTER_TYPE;


//=====================================================================================================================
//  --- Public Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


#endif  // LOG_DEFS_H_


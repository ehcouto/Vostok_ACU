/**
 * @file
 * @brief       Private parameters for the API010PollVar module.
 *
 * @details     Refer to the C source file for more detailed information.
 *
 * @copyright   Copyright 2017 Whirlpool Corporation.  All rights reserved - CONFIDENTIAL.
 */

#ifndef API010POLLVAR_PRV_H_
#define API010POLLVAR_PRV_H_


//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- Other Modules --


//=====================================================================================================================
//  --- Private Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================


// -- Private Compile-Time Directives --

//! Specify whether API010 should support publishing data directly on an SPI channel.
#define API010_CAN_USE_SPI_MODULE       DISABLED

//! Specify whether API010 should support publishing data through an SPI device.
#define API010_CAN_USE_SPIMGR_MODULE    DISABLED

//! Specify whether the API010_CMD_REQUEST_ACQUISITION_CONFIGURATION OpCode is supported.
//! This OpCode should only be required for debugging purposes.
#define API010_DEBUG_ACQ_CONFIG         DISABLED


// -- Private Constant Declarations --

//! Specify the maximum number of memory-mapped variables supported by API010.
//! Set this value to 0 to disable memory-mapped variables.
#define API010_MMAP_VARS_COUNT          (20U)

//! The size of the memory buffer used to detect changes in variable values when "on change" monitoring is active.
//! Set this value to 0 to disable the "on change" feature.
//! If this value is not 0, then the module will allocate RAM for the "on change" monitoring feature.
//! RAM allocation will be API010_ON_CHANGE_BUFFER_SIZE plus a byte for each fixed and memory-mapped variable.
#define API010_ON_CHANGE_ACU_VARIABLE_BUFFER_SIZE    0U
#define API010_ON_CHANGE_HMI_VARIABLE_BUFFER_SIZE    0U

//! The number of bytes to allocate for the stream of API010 data that is published.
//! The data stream is potentially published across multiple Reveal packets.
//! If this value is left undefined, then the module will make a guess based on the number of variables.
#define API010_STREAM_BUFFER_SIZE       360U

#if (API010_CAN_USE_SPI_MODULE == ENABLED)
    //! Define the SPI channel that is used when API010 publishes on SPI.
    #define API010POLLVAR_SPI_CHANNEL   SPI0
#endif

#if (API010_CAN_USE_SPIMGR_MODULE == ENABLED)
    //! Define the SpiMgr device that is used when API010 publishes on SPI.
    #define API010_SPIMGR_DEVICE        SPIMGR_DEBUG
#endif


//=====================================================================================================================
//  --- Private Methods ------------------------------------------------------------------------------------------------
//=====================================================================================================================


#endif      // API010POLLVAR_PRV_H_

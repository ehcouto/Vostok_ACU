/**
 *  @file
 *    @brief    This module implements the standard Data Flash API (Application Programming Interface)
 *              for STM32F0x
 *    @details  As STM32G0 does not have seperate DATA memory so we use FLASH memory as DATA memory
 *              This module referes the Flash.h module
 *
 *   @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2021-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "DataFlash.h"
#include "Flash.h"

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//=====================================================================================================================
/**
 *    @brief  - Memory is addressed as relative or absolute address mode.
 *    @details- Absolute address –
 *              means address must be between
 *
 *
 *    @param  - ENABLED(1) - Relative address mode
 *              DISABLED(0) – Absolute address mode
 *
 *    @note   - Following are the Examples\n
 *              "#define DATAFLASH_RELATIVE_ADDRESS   ENABLED" // Relative address mode enabled
 *              "#define DATAFLASH_RELATIVE_ADDRESS   DISABLED"// Absolute address mode enabled
 */
#ifndef DATAFLASH_RELATIVE_ADDRESS
    #define DATAFLASH_RELATIVE_ADDRESS      ENABLED
#endif

static DATAFLASH_STATE_TYPE DataFlash_Status;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method initializes the Data flash module.
 *    @details  This method initializes data flash related variables.
 */
void DataFlash__Initialize(void)
{
    Flash__Initialize();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method Copies data from the RAM to the DataFlash as per the size and returns
 *              the response for write operation.
 *    @details  This method writes data from source (RAM) to destination (Data Flash) as per the size
 *              and returns the response for the write operation.
 *            Note -
 *                  1. Implemented in this API - Dataflash (EEPROM) byte must be in erased state before being programmed.
 *                  2. Controller supports byte by byte and 1 long write.
 *                  3. The software design supports for both even and odd size write.
 *
 *    @param    destination : Defines the address of the Data flash where data to be written
 *    @param    source      : Defines the address of RAM from where data to be written to flash
 *    @param    size        : Number of bytes to be written
 *    @return   enum DATAFLASH_RESPONSE_TYPE
 *      @retval DATAFLASH_RESPONSE_DONE(0)                 : If the write operation is complete.
 *      @retval DATAFLASH_RESPONSE_WRITE_ERROR(3)          : If the write operation is unsuccessful.
 *      @retval DATAFLASH_RESPONSE_ERROR_OUT_OF_RANGE(4)   : If the requested memory is out of range
 *      @retval DATAFLASH_RESPONSE_INVALID_SIZE(5)         : If the size is invalid i.e. odd size
 *
 */
DATAFLASH_RESPONSE_TYPE DataFlash__Write(ADDRESS_TYPE destination, uint8 * source, SIZE_TYPE size)
 {
    DATAFLASH_RESPONSE_TYPE response;

    response = DATAFLASH_RESPONSE_ERROR_OUT_OF_RANGE;

    #if (DATAFLASH_RELATIVE_ADDRESS == ENABLED)
        destination += DATAFLASH_LOW_LIMIT;
    #endif
        response = (DATAFLASH_RESPONSE_TYPE) Flash__Write(destination,source,size);
    return (response);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method reads data from source (Data Flash) and copies to destination (RAM) as per the size
*               and returns the response for the read operation
*     @param    destination : Defines the address of the RAM where data to be copied
*     @param    source      : Defines the address of Data Flash from where data to be read
*     @param    size        : Number of bytes to be read
*     @return   enum DATAFLASH_RESPONSE_TYPE
*       @retval DATAFLASH_RESPONSE_DONE(0)                 : Read operation is performed successfully
*       @retval DATAFLASH_RESPONSE_ERROR_OUT_OF_RANGE(4)   : If the requested memory is out of range
 */
DATAFLASH_RESPONSE_TYPE DataFlash__Read(uint8 * destination, ADDRESS_TYPE source, SIZE_TYPE size)
{
    DATAFLASH_RESPONSE_TYPE response;

    response = DATAFLASH_RESPONSE_ERROR_OUT_OF_RANGE;

    //Check for the requested size
    if (size > 0)
    {
        #if (DATAFLASH_RELATIVE_ADDRESS == ENABLED)     //For relative address
              source += DATAFLASH_LOW_LIMIT;
        #endif

        response = (DATAFLASH_RESPONSE_TYPE) Flash__Read(destination,source,size);
    }
    else
    {
        response = DATAFLASH_RESPONSE_INVALID_SIZE;
    }

    return (response);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method performs erase/format operation on particular page.
 *    @param    page_init :  address within page which to be formatted.
 *
 *    @note     For Freescale Kinetis MKE01x their is Flash of 2048 byte it means only one page
 *
 *
 *    @return   enum DATAFLASH_RESPONSE_TYPE
 *    @retval DATAFLASH_RESPONSE_DONE(0)                 : If the write operation is complete.
 *    @retval DATAFLASH_RESPONSE_WRITE_ERROR(3)          : If the write operation is unsuccessful.
 *    @retval DATAFLASH_RESPONSE_ERROR_OUT_OF_RANGE(4)   : If the requested memory is out of range
 */
DATAFLASH_RESPONSE_TYPE DataFlash__FormatPage(ADDRESS_TYPE page_init)
{
    DATAFLASH_RESPONSE_TYPE response;
    response = DATAFLASH_RESPONSE_ERROR_OUT_OF_RANGE;
    DataFlash_Status = DATAFLASH_STATE_BUSY;

    #if (DATAFLASH_RELATIVE_ADDRESS == ENABLED)
        page_init += DATAFLASH_LOW_LIMIT;
    #endif

    response = (DATAFLASH_RESPONSE_TYPE) Flash__FormatPage (page_init);
    if (DATAFLASH_RESPONSE_DONE == response)
    {
        DataFlash_Status = DATAFLASH_STATE_FREE;
    }

    return (response);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method provides status of Data flash whether busy/free/error
 *    @return   enum DATAFLASH_STATE_TYPE
 *    @retval   DATAFLASH_STATE_BUSY    : Data Flash is busy for read write
 *              DATAFLASH_STATE_FREE    : Data Flash is free for read write
 *              DATAFLASH_STATE_ERROR   : Data Flash is encountered error condition
 */
DATAFLASH_STATE_TYPE DataFlash__GetStatus(void)
{
    return (DataFlash_Status);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    DataFlash Handler
 *    @details  N/A
 */
void DataFlash__Handler(void)
{

}

#if (FILL_DATA_FEATURE == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 *   @brief    This method fills the Data Flash with specific value as per the size and returns the response of fill operation
 *   @details  This method fills the specific no of bytes of data into the Data Flash with specific value.
 *              Destination address is starting address of data flash where specific value to be filled.
 *              It returns the Data flash response for write operation
 *            Note -  1. Controller supports byte by byte and 1 long word write.
 *                    2. The software design supports for both even and odd size write.
 *
 *    @param    destination : Defines the address of the Data flash where data to be written
 *    @param    size        : Number of bytes to be written
 *    @param    value       : Defines the specific value which should be filled into Data Flash
 *    @return   enum DATAFLASH_RESPONSE_TYPE
 *      @retval DATAFLASH_RESPONSE_DONE(0)                 : If the write operation is complete.
 *      @retval DATAFLASH_RESPONSE_WRITE_ERROR(3)          : If the write operation is unsuccessful.
 *      @retval DATAFLASH_RESPONSE_ERROR_OUT_OF_RANGE(4)   : If the requested memory is out of range
 *      @retval DATAFLASH_RESPONSE_INVALID_SIZE(5)         : If the size is invalid i.e. odd size
 */
DATAFLASH_RESPONSE_TYPE DataFlash__Fill(ADDRESS_TYPE destination, SIZE_TYPE size, uint8 value)
{

}

#endif

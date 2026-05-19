/**
 *  @file
 *    @brief    Configurations for the DataFlash memory and NVData
 *
 *    @details Some important configurations for STM32G0x
 *      #define NVDATA_PAGE_FORMAT_SIZE              0x800
 *      #define NVDATA_MEMORY_ALIGMENT               8
 *
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
#ifndef DATAFLASH_PRM_H
    #define DATAFLASH_PRM_H
#include "C_Extensions.h"

//=====================================================================================================================
/**
 *    @brief  - Enables or disables the system to compile the Fill feature.
 *    @details- Fill feature fills the specific no of bytes of data into the Data Flash with specific value.
 *              Destination address is starting address of data flash where specific value to be filled.
 *
 *    @param  - ENABLED(1) - Enables fill feature
 *              DISABLED(0) – Disables fill feature
 *
 *    @note   - Following are the Examples\n
 *              "#define FILL_DATA_FEATURE   ENABLED"
 *              "#define FILL_DATA_FEATURE   DISABLED"
 */
//=====================================================================================================================
#define FILL_DATA_FEATURE       DISABLED

//=====================================================================================================================
/**
 *    @brief  - Memory is addressed as relative or absolute address mode.
 *    @details- Absolute address –
 *              means address must be between
 *                  DATAFLASH_LOW_LIMIT & DATAFLASH_HIGH_LIMIT for EEPROM
 *                  FLASH_LOW_LIMIT & FLASH_HIGH_LIMIT for FLASH memory
 *
 *             Relative address -
 *             System works with relative address meaning 0x0000 is starting address of flash.
 *             Here EEPROM and FLASH memory is treated as one segment of Memory.
 *
 *             When the relative address is enabled and Word by Word writing is enabled then-
 *             DATAFLASH_HIGH_LIMIT macro value should be
 *             DATAFLASH_HIGH_LIMIT = EEPROM address divide of 4 – 1
 *
 *             FLASH_LOW_LIMIT macro value should be
 *             DATAFLASH_HIGH_LIMIT = FLASH address divide of 4
 *
 *    @param  - ENABLED(1) - Relative address mode
 *              DISABLED(0) – Absolute address mode
 *
 *    @note   - Following are the Examples\n
 *              "#define DATAFLASH_RELATIVE_ADDRESS   ENABLED"  Relative address mode enabled
 *              "#define DATAFLASH_RELATIVE_ADDRESS   DISABLED" Absolute address mode enabled
 */
//=====================================================================================================================
#define DATAFLASH_RELATIVE_ADDRESS      ENABLED

//=====================================================================================================================
/**
 *    @brief  - Defines the lower and upper limits for flash memory.
 *    @details- The application code also sits in the main program area so before configuring the
 *              Data FLASH start address should take care of the application code size.
 *
 *              Note Refer data sheet

 *    @note   - Following are the Examples\n
 *              "#define DATAFLASH_LOW_LIMIT     0x0800F800"
 *              "#define DATAFLASH_HIGH_LIMIT    0x0800FFFF"
 */
//=====================================================================================================================
extern uint32 __DataFlash_Start;
extern uint32 __DataFlash_End;

#define DATAFLASH_LOW_LIMIT     (uint32)(&__DataFlash_Start)          // Flash lower boundary.
#define DATAFLASH_HIGH_LIMIT    (uint32)(&__DataFlash_End)          // Flash upper boundary.


//=====================================================================================================================
// Setup for NVData

//=====================================================================================================================
/**
 * @brief definition of minimum page size that can be erased in one operation. This definition refers to a physical dataflash page.
 * @details DataFlash devices allows the system to write once in each position, after that if the system wants to
 * write over a position of memory again, the page that constrains the this memory has to be erase. This parameter defines
 * how big the erasing page is.
 * @note
 * <pre>
 *          For STM32FG0x :          #define NVDATA_PAGE_FORMAT_SIZE              0x800
 * </pre>
 */
#define NVDATA_PAGE_FORMAT_SIZE                             DATAFLASH_PAGE_FORMAT_SIZE

//=====================================================================================================================
/**
 * @brief definition of the size of a logical page in the storage device (the logical page is composed of 1 or more physical pages)
 * @details When using Dataflash module over Flash, this configuration has to match the size of a data flash page or multiple of that.
  * The size of a page has to be big enough to hold at least one copy of each segment plus header of each segment
 * (6 bytes including header and CRC).\n
 * For E2prom usage this number is not really used.\n
 * @note
 * <pre>
 *      consider a STM32G0x as examples..
 *      Each physical page has 0x800 bytes
 *      The microcontroller has 384 pages of 0x800 each.
 *      We can take the first 2 physical pages to compose each logical page.
 *
 *      #define NVDATA_PAGE_FORMAT_SIZE                             0x800
 *      #define NVDATA_PAGE_SIZE                                    (NVDATA_PAGE_FORMAT_SIZE * 2)
 *
 *      In this case the system will consider a big page of the size = (NVDATA_PAGE_FORMAT_SIZE * 2) but when it needs to erase the page it will to
 *      in chunks of 0x800.
 *
 * </pre>
 * <pre>
 *          #define NVDATA_PAGE_SIZE                            (NVDATA_PAGE_FORMAT_SIZE * 3)
 * </pre>
 */
#define NVDATA_PAGE_SIZE                                    (NVDATA_PAGE_FORMAT_SIZE * 1)

#define NVDATA_STATIC_DATA_LOCATION                         (NVDATA_PAGE_SIZE * 2)

//=====================================================================================================================
/**
 * @brief Macro that defines the method used to initialize the device
 * @note
 * <pre>
 *          For STM32G0x:
 *          #define NVDATA__INITIALIZEDEVICE()                      DataFlash__Initialize()
 * </pre>
 */
#define NVDATA__INITIALIZEDEVICE()                      DataFlash__Initialize()

//=====================================================================================================================
/**
 * @brief Macro that defines the method used to write into the storage system.
 * @details Normally it's a call to NVData__Write or EepromI2c__Write
 * @note
 * <pre>
 *          For STM32G0x:
 *          #define NVDATA__WRITE(destination,source,size)              DataFlash__Write(destination,source,size)
 * </pre>
 */
#define NVDATA__WRITE(destination,source,size)              DataFlash__Write(destination,source,size)

//=====================================================================================================================
/**
 * @brief Macro that defines the method used to read from the storage system.
 * @details Normally it's a call to NVData__Read or EepromI2c__Read
 * @note
 * <pre>
 *          For STM32G0x:
 *          #define NVDATA__READ(destination,source,size)              DataFlash__Read(destination,source,size)
 * </pre>
 */
#define NVDATA__READ(destination,source,size)               DataFlash__Read(destination,source,size)

//=====================================================================================================================
/**
 * @brief Macro that defines what value is consider that a write or read operation is completed.
 * @note
 * <pre>
 *          For STM32G0x:
 *          #define NVDATA_PROCESS_OKAY                                DATAFLASH_RESPONSE_DONE
 * </pre>
 */
#define NVDATA_PROCESS_OKAY                                 DATAFLASH_RESPONSE_DONE

//=====================================================================================================================
/**
 * @brief Macro that defines the condition used to define if the storage device is free (nor performing any writing or reading).
 * @details This define should contains a boolean result, so normally it's defined as a comparison in between 2 elements.
 * @note
 * <pre>
 *          For STM32G0x:
 *          #define NVDATA__ISFREE()                                   (DataFlash__GetStatus() != DATAFLASH_STATE_BUSY)
 * </pre>
 */
#define NVDATA__ISFREE()                                    (DataFlash__GetStatus() != DATAFLASH_STATE_BUSY)

//=====================================================================================================================
/**
 * @brief Command used to erase a page of the target device.
 * @details Only required when the storage device doesn't allow the system to overwrite data before the position have been erased.
 * This macro is only used when the directive NVDATA_MEMORY_STRIPING is defined as ENABLED
 * @note
 * <pre>
 *          For STM32G0x:
 *          #define NVDATA__PAGEERASE(page_address)                     DataFlash__FormatPage(page_address)
 * </pre>
 */
#define NVDATA__PAGEERASE(page_address)                     DataFlash__FormatPage(page_address)

//=====================================================================================================================
/**
 * @brief Command used to execute the handler of the storage device.
 * @details The handler of the storage device should be called so it performs the actual reading or writing.
 * @note
 * <pre>
 *          For STM32G0x:
 *          #define NVDATA__DEVICEBLOCKINGHANDLER()                     DataFlash__Handler()
 * </pre>
 */
#define NVDATA__DEVICEBLOCKINGHANDLER()                     DataFlash__Handler()

//=====================================================================================================================
/**
 * @brief Memory alignment is related to the fact that some devices have constrains in terms of the smallest memory cell that allow writing.
 * @details For most device there's no constrains so this directive should be defined as 1. In some cases thre's a constrains which can be
 * managed in the Storage device module like the Cortex microcontroller
 * @note
 * <pre>
 *          For STM32G0x:                #define NVDATA_MEMORY_ALIGMENT                              8
 * </pre>
 */
#define NVDATA_MEMORY_ALIGMENT                              DATAFLASH_ALIGNMENT



#endif

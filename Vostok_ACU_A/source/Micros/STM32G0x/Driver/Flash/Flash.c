/**
 *  @file       
 *
 *    @brief    This module implements the standard Flash API (Application Programming Interface)
 *              for STM32F0x
 *    @details  This modules provides the interface to application in order to write or
 *              format the Flash of STM32G0x.
 *              Note -  1. Controller supports only 8 byte addressed write. If the application tries to write on odd address
 *                         gives error.
 *                      2. Controller supports only 8 byte write. Application can write 1 byte were the next adjacent byte
 *                         is written with 0xff(because the controller support word writing)
 *                      3. The software design supports for odd size write.
 *                         The next adjacent byte is written with 0xff(because the controller support word writing)
 *                      4. Before writing data on the memory the flash memory should have erase value(0xff).
 *                      5. Fast programming is performed writing 256 bytes
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
#include "Flash.h" 
#include "uc.h"
#include "Micro.h"
#include <string.h>

#include "DataFlash.h"

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define FLASH_LOW_LIMIT_VAL         (FLASH_LOW_LIMIT - 1)
#define FLASH_HIGH_LIMIT_VAL        (FLASH_HIGH_LIMIT + 1)

#define DATAFLASH_LOW_LIMIT_VAL     (DATAFLASH_LOW_LIMIT - 1)
#define DATAFLASH_HIGH_LIMIT_VAL    (DATAFLASH_HIGH_LIMIT + 1)

#define FLASH_ER_PRG_TIMEOUT        ((uint32)0x000B0000)

#define FLASH_FLAG_BSY              FLASH_SR_BSY     //! FLASH Busy flag
#define FLASH_FLAG_PGERR            FLASH_SR_PGERR   //! FLASH Programming error flag
#define FLASH_FLAG_WRPERR           FLASH_SR_WRPERR  //! FLASH Write protected error flag
#define FLASH_FLAG_EOP              FLASH_SR_EOP     //! FLASH End of Programming flag

#define FLASH_LOWBYTE_DEFAULT       ((uint16)0x00ff)
#define FLASH_HIGHBYTE_DEFAULT      ((uint16)0xff00)

#define FLASH_TIMEOUT_VALUE         1000U          /*!< FLASH Execution Timeout, 1 s at 64MHz System Clock Speed*/
#define FLASH_FAST_PGM_BYTES_SIZE   256
#define FLASH_DWORD_PGM_BYTES_SIZE  8

// This method  unlocks the  Flash memory
// Flash should be unlocked before performing the write operation
#define FLASH_UNLOCK_SEQUENCE()       {FLASH.KEYR = FLASH_FKEY1; FLASH.KEYR = FLASH_FKEY2;}

// This method  locks the  Flash memory
// should be called once the write operation is done in order to avoid unintentional write operation
#define FLASH_LOCK_SEQUENCE()         {FLASH.CR |= FLASH_CR_LOCK;}

#define BANK1_START_ADDRESS         0x08000000
#define BANK2_START_ADDRESS_256KB   0x08020000
#define BANK2_START_ADDRESS_512KB   0x08040000
#define BANK2_START_PAGE_NUMBER     256

#define FLASH_256KB_SIZE            (256*1024)

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static FLASH_RESPONSE_TYPE FlashProgramDoubleWord(uint32 Address, uint8 * source);
__ramfunc static FLASH_RESPONSE_TYPE FlashFastProgramming(FLASH_ADDRESS_TYPE start_address, FLASH_ADDRESS_TYPE data_address, uint16 num_pages);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method initializes the flash module.
 *    @details  This method initializes flash related variables.
 */
void Flash__Initialize(void)
{

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method Copies data from the RAM to the Flash as per the size and returns
 *              the response for write operation.
 *    @details  This method writes data from source (RAM) to destination ( Flash) as per the size
 *              and returns the response for the write operation.
 *              Note -  1. Controller supports only 8-byte addressed write. If the application tries to write on odd address
 *                         gives error.
 *                      2. Controller supports only 8-byte addressed write. Application can write 1 byte were the next adjacent byte
 *                         is written with 0xff(because the controller support word writing)
 *                      3. The software design supports for odd size write.
 *                         The next adjacent byte is written with 0xff(because the controller support word writing)
 *                      4. Before writing data on the memory the flash memory should have erase value(0xff).
 *
 *    @param    destination : Defines the address of the flash where data to be written
 *    @param    source      : Defines the address of RAM from where data to be written to flash
 *    @param    size        : Number of bytes to be written
 *    @return   enum FLASH_RESPONSE_TYPE
 *      @retval FLASH_RESPONSE_DONE                     : If the write operation is complete.
 *      @retval FLASH_RESPONSE_ACCEPTED                 : If the command accepted
 *      @retval FLASH_RESPONSE_ERROR_BUSY               : If the flash is busy
 *      @retval FLASH_RESPONSE_WRITE_ERROR              : If the write operation is unsuccessful.
 *      @retval FLASH_RESPONSE_ERROR_OUT_OF_RANGE       : If the requested memory is out of range
 *      @retval FLASH_RESPONSE_INVALID_SIZE             : If the size is invalid i.e. odd size
 */
FLASH_RESPONSE_TYPE Flash__Write(FLASH_ADDRESS_TYPE destination, uint8 * source, FLASH_SIZE_TYPE size)
{
    FLASH_RESPONSE_TYPE response;
    uint16 num_pages = 0;

    response = FLASH_RESPONSE_ERROR_OUT_OF_RANGE;

    if( ( (destination > FLASH_LOW_LIMIT_VAL) && ((destination + (FLASH_ADDRESS_TYPE)size) <= FLASH_HIGH_LIMIT_VAL) ) ||
        ( (destination > DATAFLASH_LOW_LIMIT_VAL) && ((destination + (FLASH_ADDRESS_TYPE)size) <= DATAFLASH_HIGH_LIMIT_VAL) )  )
    {
        //Cortex Flash memory is 4-byte aligned, it writes only on the 4 byte aligned address
        if ((destination % ((FLASH_ADDRESS_TYPE) FLASH_ALIGNMENT)) != 0)
        {
            response = FLASH_RESPONSE_INVALID_SIZE;
        }
        else
        {
            //Writing the single byte in the flash memory at the end
            response = FLASH_RESPONSE_DONE;

            //Trimming the
            uint8 end_address_alignment = size % FLASH_ALIGNMENT;

            if (end_address_alignment != 0)
            {
                uint8 filler_data[8];

                size = size - end_address_alignment;
                memset((uint8 *)filler_data, 0xFF, 8);
                memcpy((uint8 *)filler_data, (uint8 *)(source+size), end_address_alignment);


                response = FlashProgramDoubleWord((destination + size), filler_data);
            }

            //Writing the double word data in the flash memory
            while ((size != 0) &&
                   (response == FLASH_RESPONSE_DONE))
            {
                //Refresh Watchdog
                Micro__ServiceWatchdog();

                if (((destination %  FLASH_PAGE_FORMAT_SIZE) == 0) && (size >= FLASH_PAGE_FORMAT_SIZE))
                {
                    num_pages = size/FLASH_PAGE_FORMAT_SIZE;

                    response = FlashFastProgramming(destination, (FLASH_ADDRESS_TYPE)source, num_pages);

                    source += (num_pages * FLASH_PAGE_FORMAT_SIZE);
                    destination += (num_pages *FLASH_PAGE_FORMAT_SIZE);
                    size -= (num_pages * FLASH_PAGE_FORMAT_SIZE);
                }
                else
                {
                    response = FlashProgramDoubleWord(destination, source);

                    source += FLASH_DWORD_PGM_BYTES_SIZE;
                    destination += FLASH_DWORD_PGM_BYTES_SIZE;
                    size -= FLASH_DWORD_PGM_BYTES_SIZE;
                }
            }
        }
    }

    return(response);
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
FLASH_RESPONSE_TYPE Flash__Read(uint8 *destination, FLASH_ADDRESS_TYPE source, FLASH_SIZE_TYPE size)
{
    FLASH_RESPONSE_TYPE response;
    FLASH_SIZE_TYPE count;

    response = FLASH_RESPONSE_ERROR_OUT_OF_RANGE;

     if (((source >= FLASH_LOW_LIMIT_VAL) && ((source + (uint32)size - (uint32)1 )<= FLASH_HIGH_LIMIT)) ||
         ((source >= DATAFLASH_LOW_LIMIT_VAL) && ((source + (uint32)size - (uint32)1 )<= DATAFLASH_HIGH_LIMIT_VAL)) )

    {
        for (count=0; count<size;count++)
        {
            destination[count] = ((uint8 *)source)[count];
        }
        response = FLASH_RESPONSE_DONE;
    }
    return(response);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method performs erase/format operation on Flash.
     *    @param    start_address :  address from where the erase to be started
     *    @param    end_address   :  address till erase to be performed
 *    @return   enum FLASH_RESPONSE_TYPE
 *      @retval FLASH_RESPONSE_DONE                     : If the erase operation is complete.
 *      @retval FLASH_RESPONSE_ACCEPTED                 : If the command accepted
 *      @retval FLASH_RESPONSE_ERROR_BUSY               : If the flash is busy
 *      @retval FLASH_RESPONSE_WRITE_ERROR              : If the erase operation is unsuccessful.
 *      @retval FLASH_RESPONSE_ERROR_OUT_OF_RANGE       : If the requested memory is out of range
 *      @retval FLASH_RESPONSE_INVALID_SIZE             : If the size is invalid i.e. odd size
 */
FLASH_RESPONSE_TYPE Flash__Format(FLASH_ADDRESS_TYPE start_address, FLASH_ADDRESS_TYPE end_address)
{
    FLASH_RESPONSE_TYPE flash_response = FLASH_RESPONSE_ERROR_OUT_OF_RANGE;
    uint32 flash_cr = 0;
    uint32 timeout = 0;
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    if( ((start_address > FLASH_LOW_LIMIT_VAL) && (end_address < FLASH_HIGH_LIMIT_VAL)) ||
        ((start_address > DATAFLASH_LOW_LIMIT_VAL) && (end_address <= DATAFLASH_HIGH_LIMIT_VAL)))
    {
        flash_response = FLASH_RESPONSE_DONE;

        uint16 page_number = 0;

        /**
         * Flash memory organization in BANKs
         *
         * Up to 128K Flash micro have single bank
         *
         * 256KB micor have
         *      BANK 1 from 0x08000000 - 0x0801FFFF, 0 to 63 pages
         *      BANK 2 from 0x08020000 - 0x0803FFFF, 256 to 319 pages
         *
         * 512KB micor have
         *      BANK 1 from 0x08000000 - 0x0803FFFF, 0 to 127 pages
         *      BANK 2 from 0x08040000 - 0x0807FFFF, 256 to 383 pages
         *
         */
        uint32 bank2_start_address = BANK2_START_ADDRESS_512KB;

        if (Micro__GetFlashSize() <= FLASH_256KB_SIZE)
        {
            bank2_start_address = BANK2_START_ADDRESS_256KB;
        }

        while((flash_response == FLASH_RESPONSE_DONE) &&
                (start_address < end_address))
        {
            if (start_address >= bank2_start_address)
            {
                page_number = 256 + ((start_address - bank2_start_address) / FLASH_PAGE_FORMAT_SIZE);
            }
            else
            {
                page_number = (start_address - BANK1_START_ADDRESS) / FLASH_PAGE_FORMAT_SIZE;
            }

            //Refresh Watchdog
            Micro__ServiceWatchdog();

            /* Enter critical section: row programming should not be longer than 7 ms */
            MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

            FLASH_UNLOCK_SEQUENCE();

            /* Clear SR register */
            FLASH.SR = FLASH_SR_CLEAR;

            /* Clear CR Reg */
            FLASH.CR = 0;
            flash_cr = FLASH.CR;

            /* Check if page has to be erased in bank 1 or 2 */
            if (start_address >= bank2_start_address)
            {
                flash_cr |= FLASH_CR_BKER;
            }
            else
            {
                flash_cr &= ~FLASH_CR_BKER;
            }

            /* Set page number, Page Erase bit & Start bit */
            FLASH.CR = (flash_cr | (FLASH_CR_STRT | (page_number <<  FLASH_CR_PNB_POS) | FLASH_CR_PER));

            /* wait for BSY1 in order to be sure that flash operation is ended before
            allowing prefetch in flash. Timeout does not return status, as it will
            be anyway done later */
            timeout = 0;
            while ((FLASH.SR & (FLASH_SR_BSY1 | FLASH_SR_BSY2)) != 0x00U)
            {
                timeout++;
                if (timeout >= FLASH_TIMEOUT_VALUE)
                {
                    break;
                }
            }

            if (((FLASH.SR & FLASH_SR_ERRORS) != 0) && ((FLASH.SR & FLASH_SR_EOP) == 0))
            {
                flash_response = FLASH_RESPONSE_WRITE_ERROR;
                break;
            }

            FLASH_LOCK_SEQUENCE();

            /* Exit critical section: restore previous priority mask */
            MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();

            start_address += FLASH_PAGE_FORMAT_SIZE;
        }
    }

    return (flash_response);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief    This method performs erase/format operation on Flash page.
 * @param page_init -   Page address to be erased
 *    @return   enum FLASH_RESPONSE_TYPE
 *      @retval FLASH_RESPONSE_DONE                     : If the erase operation is complete.
 *      @retval FLASH_RESPONSE_ACCEPTED                 : If the command accepted
 *      @retval FLASH_RESPONSE_ERROR_BUSY               : If the flash is busy
 *      @retval FLASH_RESPONSE_WRITE_ERROR              : If the erase operation is unsuccessful.
 *      @retval FLASH_RESPONSE_ERROR_OUT_OF_RANGE       : If the requested memory is out of range
 *      @retval FLASH_RESPONSE_INVALID_SIZE             : If the size is invalid i.e. odd size
 */
FLASH_RESPONSE_TYPE Flash__FormatPage(FLASH_ADDRESS_TYPE page_init)
{
    return(Flash__Format(page_init, (page_init + FLASH_PAGE_FORMAT_SIZE)));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method performs erase operation as flash range mentioned in the configuration file
 *    @details  Erases a memory from  FLASH_LOW_LIMIT_VAL to FLASH_HIGH_LIMIT_VAL
 *
 *    @return   enum FLASH_RESPONSE_TYPE
 *      @retval FLASH_RESPONSE_DONE                     : If the erase operation is complete.
 *      @retval FLASH_RESPONSE_ACCEPTED                 : If the command accepted
 *      @retval FLASH_RESPONSE_ERROR_BUSY               : If the flash is busy
 *      @retval FLASH_RESPONSE_WRITE_ERROR              : If the erase operation is unsuccessful.
 *      @retval FLASH_RESPONSE_ERROR_OUT_OF_RANGE       : If the requested memory is out of range
 *      @retval FLASH_RESPONSE_INVALID_SIZE             : If the size is invalid i.e. odd size
 */
FLASH_RESPONSE_TYPE Flash__FormatChip(void)
{
    return(Flash__Format(FLASH_LOW_LIMIT, FLASH_HIGH_LIMIT));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *    @brief    This method provides status of  flash whether busy/free/error
 *    @return   enum FLASH_STATE_TYPE
 *      @retval FLASH_STATE_FREE    :  Flash is free for read write
 */
FLASH_STATE_TYPE Flash__GetStatus(void)
{
    return (FLASH_STATE_FREE);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief  Program double-word (64-bit) at a specified address.
  * @param  Address Specifies the address to be programmed.
  * @param  Data Specifies the data to be programmed.
 *    @return   enum FLASH_RESPONSE_TYPE
 *      @retval FLASH_RESPONSE_DONE                     : If the write operation is complete.
 *      @retval FLASH_RESPONSE_ACCEPTED                 : If the command accepted
 *      @retval FLASH_RESPONSE_ERROR_BUSY               : If the flash is busy
 *      @retval FLASH_RESPONSE_WRITE_ERROR              : If the write operation is unsuccessful.
 *      @retval FLASH_RESPONSE_ERROR_OUT_OF_RANGE       : If the requested memory is out of range
 *      @retval FLASH_RESPONSE_INVALID_SIZE             : If the size is invalid i.e. odd size
  */
static FLASH_RESPONSE_TYPE FlashProgramDoubleWord(uint32 Address, uint8 * source)
{
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    FLASH_RESPONSE_TYPE flash_response = FLASH_RESPONSE_DONE;
    uint32 timeout = 0;

    uint32 word_data_0 = (uint32)(  (source[3] << 24) |
                                    (source[2] << 16) |
                                    (source[1] << 8 ) |
                                    (source[0]      )) ;

    uint32 word_data_1 = (uint32)(  (source[7] << 24) |
                                    (source[6] << 16) |
                                    (source[5] << 8 ) |
                                    (source[4]      )) ;

    /* Enter critical section: row programming should not be longer than 7 ms */
    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

    FLASH_UNLOCK_SEQUENCE();

    /* Clear CR Reg */
    FLASH.CR = 0;

    /* Clear SR register */
    FLASH.SR = FLASH_SR_CLEAR;


    /* Set PG bit */
    SET_BIT(FLASH.CR, FLASH_CR_PG);

    /* Program first word */
    *(uint32 *)Address = word_data_0;

    /* Barrier to ensure programming is performed in 2 steps, in right order
    (independently of compiler optimization behavior) */
    __ISB();

    /* Program second word */
    *(uint32 *)(Address + 4U) = word_data_1;

    /* wait for BSY1 in order to be sure that flash operation is ended before
    allowing prefetch in flash. Timeout does not return status, as it will
    be anyway done later */
    timeout = 0;
    while ((FLASH.SR & (FLASH_SR_BSY1 | FLASH_SR_BSY2)) != 0x00U)
    {
        timeout++;
        if (timeout >= FLASH_TIMEOUT_VALUE)
        {
            break;
        }
    }

    if (((FLASH.SR & FLASH_SR_ERRORS) != 0) && ((FLASH.SR & FLASH_SR_EOP) == 0))
    {
        flash_response = FLASH_RESPONSE_WRITE_ERROR;
    }

    /* Clear PG bit */
    CLEAR_BIT(FLASH.CR, FLASH_CR_PG);

    FLASH_LOCK_SEQUENCE();

    /* Exit critical section: restore previous priority mask */
    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();

    return flash_response;
}


//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief  Fast program a 32 row double-word (64-bit) at a specified address.
  * @param  Address Specifies the address to be programmed.
  * @param  DataAddress Specifies the address where the data are stored.
 *    @return   enum FLASH_RESPONSE_TYPE
 *      @retval FLASH_RESPONSE_DONE                     : If the write operation is complete.
 *      @retval FLASH_RESPONSE_ACCEPTED                 : If the command accepted
 *      @retval FLASH_RESPONSE_ERROR_BUSY               : If the flash is busy
 *      @retval FLASH_RESPONSE_WRITE_ERROR              : If the write operation is unsuccessful.
 *      @retval FLASH_RESPONSE_ERROR_OUT_OF_RANGE       : If the requested memory is out of range
 *      @retval FLASH_RESPONSE_INVALID_SIZE             : If the size is invalid i.e. odd size
 */
__ramfunc static FLASH_RESPONSE_TYPE FlashFastProgramming(FLASH_ADDRESS_TYPE start_address, FLASH_ADDRESS_TYPE data_address, uint16 num_pages)
{
    uint8 index = 0;
    uint32 timeout = 0;
    uint16 page_number = 0;
    uint32 flash_cr = 0;
    FLASH_RESPONSE_TYPE flash_response = FLASH_RESPONSE_DONE;
    uint8 fast_programming_loops;
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    /**
     * Flash memory organization in BANKs
     *
     * Up to 128K Flash micro have single bank
     *
     * 256KB micor have
     *      BANK 1 from 0x08000000 - 0x0801FFFF, 0 to 63 pages
     *      BANK 2 from 0x08020000 - 0x0803FFFF, 256 to 319 pages
     *
     * 512KB micor have
     *      BANK 1 from 0x08000000 - 0x0803FFFF, 0 to 127 pages
     *      BANK 2 from 0x08040000 - 0x0807FFFF, 256 to 383 pages
     *
     */
    uint32 bank2_start_address = BANK2_START_ADDRESS_512KB;

    if (Micro__GetFlashSize() <= FLASH_256KB_SIZE)
    {
        bank2_start_address = BANK2_START_ADDRESS_256KB;
    }


    /* Enter critical section: row programming should not be longer than 7 ms */
    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();

    FLASH_UNLOCK_SEQUENCE();


    while((num_pages) && (flash_response == FLASH_RESPONSE_DONE))
    {
        num_pages--;

        /**
         * FAST Programming steps :
         * Erase the 2K page even though its erased previously otherwise it will set PGSERR bit
         * Then program
         */
        //Refresh Watchdog
        Micro__ServiceWatchdog();

        if (start_address >= bank2_start_address)
        {
            page_number = 256 + ((start_address - bank2_start_address) / FLASH_PAGE_FORMAT_SIZE);
        }
        else
        {
            page_number = (start_address - BANK1_START_ADDRESS) / FLASH_PAGE_FORMAT_SIZE;
        }

        /* Clear CR Reg */
        FLASH.CR = 0;
        flash_cr = FLASH.CR;

        /* Clear SR register */
        FLASH.SR = FLASH_SR_CLEAR;

        /* Check if page has to be erased in bank 1 or 2 */
        if (start_address >= bank2_start_address)
        {
            flash_cr |= FLASH_CR_BKER;
        }
        else
        {
            flash_cr &= ~FLASH_CR_BKER;
        }

        /* Set page number, Page Erase bit & Start bit */
        FLASH.CR = (flash_cr | (FLASH_CR_STRT | (page_number <<  FLASH_CR_PNB_POS) | FLASH_CR_PER));

        /* wait for BSY1 in order to be sure that flash operation is ended before
        allowing prefetch in flash. Timeout does not return status, as it will
        be anyway done later */
        timeout = 0;
        while ((FLASH.SR & (FLASH_SR_BSY1 | FLASH_SR_BSY2)) != 0x00U)
        {
            timeout++;
            if (timeout >= FLASH_TIMEOUT_VALUE)
            {
                break;
            }
        }

        if (((FLASH.SR & FLASH_SR_ERRORS) != 0) && ((FLASH.SR & FLASH_SR_EOP) == 0))
        {
            flash_response = FLASH_RESPONSE_WRITE_ERROR;
        }

        fast_programming_loops = FLASH_PAGE_FORMAT_SIZE/FLASH_FAST_PGM_BYTES_SIZE;

        while(fast_programming_loops)
        {
            fast_programming_loops--;

            /* Clear CR Reg */
            FLASH.CR = 0;

            /* Clear SR register */
            FLASH.SR = FLASH_SR_CLEAR;


            /* Set FSTPG bit */
            SET_BIT(FLASH.CR, FLASH_CR_FSTPG);

            /* Fast Program : 64 words */
            index = 0;
            while (index < 64U)
            {
                *(uint32 *)start_address = *(uint32 *)data_address;
                /* Barrier to ensure programming is performed in 2 steps, in right order
                (independently of compiler optimization behavior) */
                __ISB();

                start_address += 4U;
                data_address += 4U;
                index++;
            }

            /* wait for BSY1 in order to be sure that flash operation is ended before
            allowing prefetch in flash. Timeout does not return status, as it will
            be anyway done later */
            timeout = 0;
            while ((FLASH.SR & (FLASH_SR_BSY1 | FLASH_SR_BSY2)) != 0x00U)
            {
                timeout++;
                if (timeout >= FLASH_TIMEOUT_VALUE)
                {
                    break;
                }
            }

            if (((FLASH.SR & FLASH_SR_ERRORS) != 0) && ((FLASH.SR & FLASH_SR_EOP) == 0))
            {
                flash_response = FLASH_RESPONSE_WRITE_ERROR;
                break;
            }
        }
    }

    /* Clear CR Reg */
    FLASH.CR = 0;

    /* Clear SR register */
    FLASH.SR = FLASH_SR_CLEAR;

    FLASH_LOCK_SEQUENCE();

    /* Exit critical section: restore previous priority mask */
    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();

    return flash_response;
}

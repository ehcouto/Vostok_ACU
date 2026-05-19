/**
 *  @file
 *
 *  @brief      This file provides the API interface for the FITMgr module
 *
 *  @details    Flash Integrity Table Manager
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: FITMgr.c 1.19 2015/07/24 16:03:23EDT Dasar Hanumanth (DASARHB) Exp  $
 *
 *  @copyright  Copyright 2014-$Date: 2015/07/24 16:03:23EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "FITMgr.h"
#include "FITMgr_prv.h"
#include "Micro.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
#ifndef FITMGR_MAX_BLOCKS_IN_TABLE
    #define FITMGR_MAX_BLOCKS_IN_TABLE       10
#endif

#if (FITMGR_MAX_BLOCKS_IN_TABLE > 255)
    #error "FITMGR_MAX_BLOCKS_IN_TABLE shall be within 255"
#endif

//This includes the HEADER_BLOCK and FOOTER_BLOCK
#define FITMGR_TABLE_FRAME_SIZE        2

#include "crc16.h"
#define FITMGR_INTRIGITY_PROCESS(buffer_ptr, seed, size)        Crc16__ConstRangeProcess(buffer_ptr, seed, size);

//Initial value for the accumulator
#define FITMGR_INTIGRITY_INITIAL_SEED                           0x0000

//Number of bytes processed for CRC calculation per handler
#define FITMGR_NUM_BYTES_CHECK_PER_HANDLER_CALL                 32000

#define FITMGR_INTEGRITY_ENGINE_BITS_USED                       16
#define FITMGR_INTEGRITY_MASK                                   (0x0000FFFF)

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
typedef enum
{
    FITMGR_STATE_IDLE,
    FITMGR_STATE_START_TABLE_VALIDATION,
    FITMGR_STATE_START_BLOCK_VALIDATIOIN,
    FITMGR_STATE_ERROR
}FITMGR_STATE_TYPE;

static FITMGR_STATE_TYPE FITMgr_State;
static FITMGR_STATUS_TYPE FITMgr_Status;

static FITMGR_TABLE_TYPE FITMgr_Table;
static uint8 FITMgr_BlockIndex;
static uint8 FITMgr_TotalBlocks;

static FITMGR_BLOCK_TYPE FITMgr_Block;
static uint32 FITMgr_CalculatedIntigrity;

static FITMGR_BLOCK_TYPE *FITMgr_BlockInfoPtr;
static uint32 *FIT_TABLE[] = FITMGR_TABLE_LIST;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static uint8 GetTotalBlocks(uint32 * table_ptr);
static FITMGR_STATUS_TYPE ValidateBlock(FITMGR_BLOCK_TYPE *block);
static FITMGR_STATUS_TYPE Validate(uint32 *table_ptr, FITMGR_ADDRESS_TYPE block_address, BOOL_TYPE read_info);
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module FITMgr and its variables
 *
 */
void FITMgr__Initialize(void)
{
    FITMgr_State            =   FITMGR_STATE_IDLE;
    FITMgr_Status           =   FITMGR_STATUS_IDLE;

    FITMgr_Table            =   (FITMGR_TABLE_TYPE)0;
    FITMgr_BlockIndex       =   0;
    FITMgr_TotalBlocks      =   0;

    FITMgr_Block.START_ADDRESS     =   (FITMGR_ADDRESS_TYPE)FIT_TABLE[0];
    FITMgr_Block.END_ADDRESS       =   0;
    FITMgr_Block.INTIGRITY_ADDRESS =   0;
    FITMgr_CalculatedIntigrity     =   0;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Process the validation of table and block in non-blocking mode.
 * @details This must be called periodically
 *          The number of bytes are processed is based on the FITMGR_NUM_BYTES_CHECK_PER_HANDLER_CALL macro value
 *
 */
void FITMgr__Handler(void)
{
    switch(FITMgr_State)
    {
        case FITMGR_STATE_IDLE:
            break;

        case FITMGR_STATE_START_TABLE_VALIDATION:
            //Manage the Table contents
            if (FITMgr_TotalBlocks > 0)
            {
                FITMgr_Block.START_ADDRESS     = ((FITMGR_BLOCK_TYPE FAR *)(FIT_TABLE[FITMgr_Table]))[FITMgr_BlockIndex].START_ADDRESS;
                FITMgr_Block.END_ADDRESS       = ((FITMGR_BLOCK_TYPE FAR *)(FIT_TABLE[FITMgr_Table]))[FITMgr_BlockIndex].END_ADDRESS;
                FITMgr_Block.INTIGRITY_ADDRESS = ((FITMGR_BLOCK_TYPE FAR *)(FIT_TABLE[FITMgr_Table]))[FITMgr_BlockIndex].INTIGRITY_ADDRESS & FITMGR_INTEGRITY_MASK;

                FITMgr_CalculatedIntigrity    = FITMGR_INTIGRITY_INITIAL_SEED;
                FITMgr_BlockIndex++;
                FITMgr_TotalBlocks--;

                FITMgr_State            = FITMGR_STATE_START_BLOCK_VALIDATIOIN;
            }
            else
            {
                FITMgr_Status   = FITMGR_STATUS_VALID;
                FITMgr_State    = FITMGR_STATE_IDLE;
            }
            break;

        case FITMGR_STATE_START_BLOCK_VALIDATIOIN:
            //Performs the integrity validation for the block
            //Its processed in multiple handler calls if the Block is bigger FITMGR_NUM_BYTES_CHECK_PER_HANDLER_CALL

            //Refresh the independent watch dog
            Micro__ServiceWatchdog();

            if ((FITMgr_Block.END_ADDRESS - FITMgr_Block.START_ADDRESS) > FITMGR_NUM_BYTES_CHECK_PER_HANDLER_CALL)
            {
                //Calculates the CRC over a range of memory up to 64kb of memory
                FITMgr_CalculatedIntigrity = FITMGR_INTRIGITY_PROCESS((uint8 FAR *)FITMgr_Block.START_ADDRESS, FITMgr_CalculatedIntigrity, FITMGR_NUM_BYTES_CHECK_PER_HANDLER_CALL);
                FITMgr_Block.START_ADDRESS +=  FITMGR_NUM_BYTES_CHECK_PER_HANDLER_CALL;
            }
            else
            {
                //Calculates the CRC over a range of memory up to 64kb of memory
                FITMgr_CalculatedIntigrity = FITMGR_INTRIGITY_PROCESS((uint8 FAR*)FITMgr_Block.START_ADDRESS, FITMgr_CalculatedIntigrity, (FITMgr_Block.END_ADDRESS - FITMgr_Block.START_ADDRESS));

                FITMgr_Block.START_ADDRESS =  FITMgr_Block.END_ADDRESS;

            }

            //Refresh the independent watch dog
            Micro__ServiceWatchdog();

            //Validate
            if (FITMgr_Block.END_ADDRESS <= FITMgr_Block.START_ADDRESS)
            {
                //compare the calculated CRC against a CRC value stored in FLASH memory by the IELF tool.
                if ((*(uint32 FAR *)FITMgr_Block.INTIGRITY_ADDRESS & FITMGR_INTEGRITY_MASK) ==  FITMgr_CalculatedIntigrity)
                {
                    FITMgr_State    = FITMGR_STATE_START_TABLE_VALIDATION;
                }
                else
                {
                    FITMgr_Status   = FITMGR_STATUS_INVALID;
                    FITMgr_State    = FITMGR_STATE_IDLE;
                }
            }
            break;

        default:
            FITMgr_State    = FITMGR_STATE_IDLE;
            break;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      Starts the validation of the specified table
 *
 * @param table - table to be validated should be from the FITMGR_TABLE_TYPE list
 * @return
 *      @retval FITMGR_STATUS_IDLE          -   This is returned by FITMgr__GetStatus() if the handler is in IDLE state
 *      @retval FITMGR_STATUS_BUSY          -   If the Handler is busy in processing the previous requests,
 *      @retval FITMGR_STATUS_ACCEPTED      -   Request is accepted and started processing
 *      @retval FITMGR_STATUS_VALID         -   This is returned by FITMgr__GetStatus() if the integrity is valid for the requested table
 *      @retval FITMGR_STATUS_INVALID       -   This is returned by FITMgr__GetStatus() if the integrity is invalid for the requested table
 *      @retval FITMGR_STATUS_INVALID_TABLE -   If the requested is not registered in the FITMGR_TABLE_TYPE or the Table format is corrupted
 *      @retval FITMGR_STATUS_INVALID_BLOCK -   If the table contains 0 blocks except the Header and Footer
 */
FITMGR_STATUS_TYPE FITMgr__StartTableValidation(FITMGR_TABLE_TYPE table)
{
    FITMGR_STATUS_TYPE response;

    response = FITMGR_STATUS_BUSY;

    if (FITMgr_State == FITMGR_STATE_IDLE)
    {
        response = FITMGR_STATUS_INVALID_TABLE;

        if (table < FITMGR_NUM_FIT_TABLE)
        {
            FITMgr_Status = FITMGR_STATUS_INVALID_BLOCK;

            FITMgr_TotalBlocks  = GetTotalBlocks(FIT_TABLE[table]);

            response            = FITMGR_STATUS_ACCEPTED;

            if (FITMgr_TotalBlocks > FITMGR_TABLE_FRAME_SIZE)
            {
                //Exclude the HEADER_BLOCK and FOOTER_BLOCK
                FITMgr_TotalBlocks  = FITMgr_TotalBlocks - FITMGR_TABLE_FRAME_SIZE;
                FITMgr_BlockIndex   = 1;

                FITMgr_Table        = table;
                FITMgr_State        = FITMGR_STATE_START_TABLE_VALIDATION;
                FITMgr_Status       = FITMGR_STATUS_BUSY;
            }
        }
    }

    return(response);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      Starts the validation of the specified block from the specified table
 * @details     The block_address should be start address of the Block defined in the FIT table
 *
 * @param table - table to be validated should be from the FITMGR_TABLE_TYPE list
 * @param block_address - Start address of the block to be validated

 * @return
 *      @retval FITMGR_STATUS_IDLE          -   This is returned by FITMgr__GetStatus() if the handler is in IDLE state
 *      @retval FITMGR_STATUS_BUSY          -   If the Handler is busy in processing the previous requests,
 *      @retval FITMGR_STATUS_ACCEPTED      -   Request is accepted and started processing
 *      @retval FITMGR_STATUS_VALID         -   This is returned by FITMgr__GetStatus() if the integrity is valid for the requested table
 *      @retval FITMGR_STATUS_INVALID       -   This is returned by FITMgr__GetStatus() if the integrity is invalid for the requested table
 *      @retval FITMGR_STATUS_INVALID_TABLE -   If the requested is not registered in the FITMGR_TABLE_TYPE or the Table format is corrupted
 *      @retval FITMGR_STATUS_INVALID_BLOCK -   If the table contains 0 blocks except the Header and Footer or
 *                                              if the block_address does not match with the FIT Table address
 */
FITMGR_STATUS_TYPE FITMgr__StartBlockValidation(FITMGR_TABLE_TYPE table, FITMGR_ADDRESS_TYPE block_address)
{
    FITMGR_STATUS_TYPE response;
    uint8 index;

    response    = FITMGR_STATUS_BUSY;
    index       = 0;

    if (FITMgr_State == FITMGR_STATE_IDLE)
    {
        response = FITMGR_STATUS_INVALID_TABLE;

        if (table < FITMGR_NUM_FIT_TABLE)
        {
            response = FITMGR_STATUS_INVALID_BLOCK;

            FITMgr_TotalBlocks  = GetTotalBlocks(FIT_TABLE[table]);

            if (FITMgr_TotalBlocks > FITMGR_TABLE_FRAME_SIZE)
            {
                //Exclude the HEADER_BLOCK and FOOTER_BLOCK
                FITMgr_TotalBlocks  = FITMgr_TotalBlocks - FITMGR_TABLE_FRAME_SIZE;

                for (index = 1; index <= FITMgr_TotalBlocks; index++)
                {
                    if ((((((FITMGR_BLOCK_TYPE FAR *)(FIT_TABLE[table]))[index].START_ADDRESS) == block_address)))
                    {
                        FITMgr_TotalBlocks  = 1;
                        FITMgr_BlockIndex   = index;

                        FITMgr_Table        = table;
                        FITMgr_State        = FITMGR_STATE_START_TABLE_VALIDATION;
                        FITMgr_Status       = FITMGR_STATUS_BUSY;
                        response            = FITMGR_STATUS_ACCEPTED;

                        break;
                    }
                }
            }
        }
    }

    return(response);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      Validation of the specified table in blocking mode
 *
 * @param table - table to be validated should be from the FITMGR_TABLE_TYPE list
 * @return
 *      @retval FITMGR_STATUS_IDLE          -   This is returned by FITMgr__GetStatus() if the handler is in IDLE state
 *      @retval FITMGR_STATUS_BUSY          -   If the Handler is busy in processing the previous requests,
 *      @retval FITMGR_STATUS_ACCEPTED      -   Request is accepted and started processing
 *      @retval FITMGR_STATUS_VALID         -   This is returned by FITMgr__GetStatus() if the integrity is valid for the requested table
 *      @retval FITMGR_STATUS_INVALID       -   This is returned by FITMgr__GetStatus() if the integrity is invalid for the requested table
 *      @retval FITMGR_STATUS_INVALID_TABLE -   If the requested is not registered in the FITMGR_TABLE_TYPE or the Table format is corrupted
 *      @retval FITMGR_STATUS_INVALID_BLOCK -   If the table contains 0 blocks except the Header and Footer
 */
FITMGR_STATUS_TYPE FITMgr__StartTableValidationBlocking(FITMGR_TABLE_TYPE table)
{
    FITMGR_STATUS_TYPE response;

    response = FITMGR_STATUS_INVALID_TABLE;

    if (table < FITMGR_NUM_FIT_TABLE)
    {
        response = Validate(FIT_TABLE[table], 0xFFFF, FALSE);
    }

    return response;
}


FITMGR_STATUS_TYPE FITMgr__StartTableValidationByRef(uint32 * table_ptr)
{
    return (Validate(table_ptr, 0xFFFF, FALSE));
}
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      Starts the validation of the specified block from the specified table in blocking mode
 * @details     The block_address should be start address of the Block defined in the FIT table
 *
 * @param table - table to be validated should be from the FITMGR_TABLE_TYPE list
 * @param block_address - Start address of the block to be validated

 * @return
 *      @retval FITMGR_STATUS_IDLE          -   This is returned by FITMgr__GetStatus() if the handler is in IDLE state
 *      @retval FITMGR_STATUS_BUSY          -   If the Handler is busy in processing the previous requests,
 *      @retval FITMGR_STATUS_ACCEPTED      -   Request is accepted and started processing
 *      @retval FITMGR_STATUS_VALID         -   This is returned by FITMgr__GetStatus() if the integrity is valid for the requested table
 *      @retval FITMGR_STATUS_INVALID       -   This is returned by FITMgr__GetStatus() if the integrity is invalid for the requested table
 *      @retval FITMGR_STATUS_INVALID_TABLE -   If the requested is not registered in the FITMGR_TABLE_TYPE or the Table format is corrupted
 *      @retval FITMGR_STATUS_INVALID_BLOCK -   If the table contains 0 blocks except the Header and Footer or
 *                                              if the block_address does not match with the FIT Table address
 */
FITMGR_STATUS_TYPE FITMgr__StartBlockValidationBlocking(FITMGR_TABLE_TYPE table, FITMGR_ADDRESS_TYPE block_address)
{
    return (Validate(FIT_TABLE[table], block_address, FALSE));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      Gets the status of the handler when used non-blocking mode API's

 * @return
 *      @retval FITMGR_STATUS_IDLE          -   Ready to take the new request
 *      @retval FITMGR_STATUS_BUSY          -   If the Handler is busy in processing the previous requests,
 *      @retval FITMGR_STATUS_ACCEPTED      -
 *      @retval FITMGR_STATUS_VALID         -   The integrity of the requested table or block is valid
 *      @retval FITMGR_STATUS_INVALID       -   The integrity of the requested table or block is invalid
 *      @retval FITMGR_STATUS_INVALID_TABLE -
 *      @retval FITMGR_STATUS_INVALID_BLOCK -
 */
FITMGR_HANDLER_STATUS_TYPE FITMgr__GetHandlerStatus(void)
{
    if (FITMgr_State != FITMGR_STATE_IDLE)
    {
        return(FITMGR_HANDLER_STATUS_BUSY);
    }
    else
    {
        return(FITMGR_HANDLER_STATUS_IDLE);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      Gets the validation status for the requested table

 * @return
 *      @retval FITMGR_STATUS_IDLE          -   Ready to take the new request
 *      @retval FITMGR_STATUS_BUSY          -   If the Handler is busy in processing the previous requests,
 *      @retval FITMGR_STATUS_ACCEPTED      -
 *      @retval FITMGR_STATUS_VALID         -   The integrity of the requested table or block is valid
 *      @retval FITMGR_STATUS_INVALID       -   The integrity of the requested table or block is invalid
 *      @retval FITMGR_STATUS_INVALID_TABLE -
 *      @retval FITMGR_STATUS_INVALID_BLOCK -
 */
FITMGR_STATUS_TYPE FITMgr__GetTableStatus(FITMGR_TABLE_TYPE table)
{
    FITMGR_STATUS_TYPE status;

    status = FITMGR_STATUS_BUSY;

    if (FITMgr_State == FITMGR_STATE_IDLE)
    {
        status = FITMGR_STATUS_NOT_PROCESSED;
        if (table == FITMgr_Table)
        {
            status = FITMgr_Status;
        }

    }

    return(status);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      Gets the validation status for the requested block address

 * @return
 *      @retval FITMGR_STATUS_IDLE          -   Ready to take the new request
 *      @retval FITMGR_STATUS_BUSY          -   If the Handler is busy in processing the previous requests,
 *      @retval FITMGR_STATUS_ACCEPTED      -
 *      @retval FITMGR_STATUS_VALID         -   The integrity of the requested table or block is valid
 *      @retval FITMGR_STATUS_INVALID       -   The integrity of the requested table or block is invalid
 *      @retval FITMGR_STATUS_INVALID_TABLE -
 *      @retval FITMGR_STATUS_INVALID_BLOCK -
 */
FITMGR_STATUS_TYPE FITMgr__GetBlockStatus(FITMGR_TABLE_TYPE table, FITMGR_ADDRESS_TYPE block_address)
{
    FITMGR_STATUS_TYPE status;

    status = FITMGR_STATUS_BUSY;

    if (FITMgr_State == FITMGR_STATE_IDLE)
    {
        status = FITMGR_STATUS_NOT_PROCESSED;
        if (table == FITMgr_Table)
        {
            status = FITMgr_Status;
        }

    }

    return(status);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Gets the Block information if its registered in the FIT table
 *
 * @param table - table to be validated should be from the FITMGR_TABLE_TYPE list
 * @param block_address - Start address of the block to be validated
 * @param block_info    - Address were block information will be copied
 * @return
 *      Returns the Block information if it matches
 *      otherwise sends the NULL
 */
FITMGR_BLOCK_TYPE * FITMgr__GettBlockInfo(FITMGR_TABLE_TYPE table, FITMGR_ADDRESS_TYPE block_address)
{
    FITMgr_BlockInfoPtr  = NULL;

    Validate(FIT_TABLE[table], block_address, TRUE);

    return(FITMgr_BlockInfoPtr);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief      Starts the validation of the specified block from the specified table
 * @details    The block_address should be start address of the Block defined in the FIT table
 *
 * @param table - table to be validated should be from the FITMGR_TABLE_TYPE list
 * @param block_address - Start address of the block to be validated
 * @param read_info - TRUE - Checks the Table Integrity without the calculation of the crc
 *                  - FALSE - Checks the Table Integrity along with the calculation of the crc
 * @return
 *      @retval FITMGR_STATUS_IDLE          -   This is returned by FITMgr__GetStatus() if the handler is in IDLE state
 *      @retval FITMGR_STATUS_BUSY          -   If the Handler is busy in processing the previous requests,
 *      @retval FITMGR_STATUS_ACCEPTED      -   Request is accepted and started processing
 *      @retval FITMGR_STATUS_VALID         -   This is returned by FITMgr__GetStatus() if the integrity is valid for the requested table
 *      @retval FITMGR_STATUS_INVALID       -   This is returned by FITMgr__GetStatus() if the integrity is invalid for the requested table
 *      @retval FITMGR_STATUS_INVALID_TABLE -   If the requested is not registered in the FITMGR_TABLE_TYPE or the Table format is corrupted
 *      @retval FITMGR_STATUS_INVALID_BLOCK -   If the table contains 0 blocks except the Header and Footer or
 *                                              if the block_address does not match with the FIT Table address
 */
static FITMGR_STATUS_TYPE Validate(uint32 *table_ptr, FITMGR_ADDRESS_TYPE block_address, BOOL_TYPE read_info)
{
    FITMGR_BLOCK_TYPE *block_ptr;
    FITMGR_STATUS_TYPE response;
    uint8 index;
    uint8 num_blocks;

    response = FITMGR_STATUS_INVALID_TABLE;

    if (table_ptr != NULL)
    {
        response = FITMGR_STATUS_INVALID_BLOCK;

        num_blocks  = GetTotalBlocks(table_ptr);

        //Exclude the HEADER_BLOCK and FOOTER_BLOCK
        for (index = 1; (index + 1) < num_blocks; index++)
        {
            block_ptr = &((FITMGR_BLOCK_TYPE *)((void *) table_ptr))[index];

            //validates all the blocks from the table
            if (block_address == 0xFFFF)
            {
                response = ValidateBlock(block_ptr);

                if (response == FITMGR_STATUS_INVALID)
                {
                    break;
                }
            }
            else
            {
                //validates the specified blocks from the table
                // If CRC address is lower than Start address, it's accepted as start address of that block
                if((block_ptr->START_ADDRESS == block_address) ||
                    ((block_ptr->INTIGRITY_ADDRESS == block_address) &&
                     (block_ptr->INTIGRITY_ADDRESS < block_ptr->START_ADDRESS)))
                {
                    if (read_info == TRUE)
                    {
                        FITMgr_BlockInfoPtr = block_ptr;
                    }
                    else
                    {
                        response = ValidateBlock(block_ptr);
                    }
                    break;
                }
            }
        }
    }

    return(response);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief       Reads the total block in the FIT table
 * @param table
 * @return
 *
 */
static uint8 GetTotalBlocks(uint32 * table_ptr)
{
    uint8 num_blocks;
    FITMGR_BLOCK_TYPE * pt;

    num_blocks  =  1;

    //Validate the TABLE_HEADER
    pt = (FITMGR_BLOCK_TYPE *)(table_ptr);

    if (((pt->START_ADDRESS) == FIT_HEADER))
    {
        pt++;

        //Validate the TABLE_FOOTER
        while ((pt->START_ADDRESS != FIT_FOOTER))
        {
            pt++;
            if (num_blocks >= FITMGR_MAX_BLOCKS_IN_TABLE)
            {
                num_blocks = 0;
                break;
            }

            num_blocks++;
        }

        num_blocks++;
    }

    return (num_blocks);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Performs the validation of the block in Blocking mode
 *
 * @param block
 * @return
 */
static FITMGR_STATUS_TYPE ValidateBlock(FITMGR_BLOCK_TYPE * block)
{
    FITMGR_STATUS_TYPE status;
    uint32 calc_integrity;
    uint32 crc_size;
    FITMGR_ADDRESS_TYPE start_addr;
    BOOL_TYPE process;

    status = FITMGR_STATUS_INVALID;

    // Check if the block doesn't require CRC check
    if(block->INTIGRITY_ADDRESS == FIT_SKIP_CRC)
    {
        // Skip CRC check can be accepted only  on other blocks than the Application one;
        // Application block should be always checked, and it contains the FIT table.
        if((((FITMGR_ADDRESS_TYPE) block) < block->START_ADDRESS) ||
            (((FITMGR_ADDRESS_TYPE) block) > block->END_ADDRESS))
        {
            status    = FITMGR_STATUS_VALID;
        }
    }
    else
    {
        start_addr = block->START_ADDRESS;
        calc_integrity = FITMGR_INTIGRITY_INITIAL_SEED;
        process = TRUE;

        //Performs the integrity validation for the block
        while (process == TRUE)
        {
            //Refresh the independent watch dog
            Micro__ServiceWatchdog();

            if ((block->END_ADDRESS - start_addr) > FITMGR_NUM_BYTES_CHECK_PER_HANDLER_CALL)
            {
                //Calculates the CRC over a range of memory up to 64kb of memory
                crc_size = FITMGR_NUM_BYTES_CHECK_PER_HANDLER_CALL;
            }
            else
            {
                //Calculates the CRC inclusive of the end address
                crc_size = ((block->END_ADDRESS - start_addr) + 1);
                process    = FALSE;
            }

            calc_integrity = FITMGR_INTRIGITY_PROCESS((uint8 FAR *)start_addr, calc_integrity, crc_size);

            start_addr +=  crc_size;

        }

        //Refresh the independent watch dog
        Micro__ServiceWatchdog();

        //compare the calculated CRC against a CRC value stored in FLASH memory by the IELF tool.
        if ((*(uint32 FAR *)block->INTIGRITY_ADDRESS & FITMGR_INTEGRITY_MASK) ==  calc_integrity)
        {
            status    = FITMGR_STATUS_VALID;
        }
    }

    return(status);
}



/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: IDTable_prv.h 1.1 2015/06/11 16:54:21EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2015-$Date: 2015/06/11 16:54:21EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef IDTABLE_PRV_H_
#define IDTABLE_PRV_H_

#define IDTABLE_MCU_DEF                     MODEL_R5F52108                  //  BE 7-4:  model  3-0:    manufacturer
#define IDTABLE_BOARD_APPLICANCE            ((BOARD_MAIN<<4) | APPLIANCE_WM )  //  BE 7-4:  board  3-0:    appliance type
#define IDTABLE_INFO                        ((BLOCK_SIZE__64 << 6) | (IDTABLE_ENDIANESS << 4) | (CRC__YES << 3) |  STD_IDENT32_VER_0)           //  BE 7: unused, 6: write block size, 5: unused, 4: endianness, 3: error checking, 2-0:    version

// Fw version
#define IDTABLE_VERION_MAJOR                1           // version
#define IDTABLE_VERION_MINOR                2           // verification
#define IDTABLE_VERION_DEBUG                3           // modification

// Product configuration
#define IDTABLE_PRODUCT_CFG1                4
#define IDTABLE_PRODUCT_CFG0                5           // Product configuration LSB

// Project code
#define IDTABLE_PROJECT_CODE                6           // Project code



extern uint16 __SetingFile_Start;
extern uint8 __SetingFile_Size;
extern uint8  __RAM_Start;
 extern uint8 __RAM_Size;


#define IDTABLE_32_RAM_ADDRESS              ((uint32)&__RAM_Start)   //  RAM address
#define IDTABLE_32_RAM_SIZE                 ((uint32)&__RAM_Size)   //  Ram size
#define IDTABLE_32_LKT_ADDRESS              0                         //  Lkt address
#define IDTABLE_32_WBOX_ADDRESS             0                        //  WBox address
#define IDTABLE_32_PDM_ADDRESS              0                         //  PDM address
#define IDTABLE_32_EEPROM_ADDRESS           0                         //  Setting address
#define IDTABLE_32_BCK_ADDRESS              0                         //  Backup address
#define IDTABLE_32_STR_FILE_ADDRESS         0


#define IDTABLE_16_LKT_SIZE                 0                         //  Lkt size in bytes
#define IDTABLE_16_WBOX_SIZE                0                        //  Wbox size in bytes
#define IDTABLE_16_EEPORM_SIZE              0                        //  Eep size in bytes
#define IDTABLE_16_BCK_SIZE                 0                         //  Bck size in bytes
#define IDTABLE_16_UCV_FW                   0                           //  Unique code Visual fw
#define IDTABLE_16_STR_FILE_SIZE            0

#define IDTABLE_08_EEPROM_PAGE_NUM          0           //  num of pages for Eep
#define IDTABLE_08_STRING_FILE_NUM_PAGES    0


//=====================================================================================================================
//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//=====================================================================================================================

#endif // IDTABLE_PRV_H_

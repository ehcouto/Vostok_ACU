/**
 *  @file
 *
 *  @brief      This module acts as an interface to the Setting File data and also provides some basic information
 *              about the setting file and setting file sections. It is written to meet Class B requirements due to
 *              the need to locate Class B parameters in the setting file.
 *
 *  @details    This module has the following responsibilities:
 *              - Iterates through all the sections of the setting file and determines if the proper setting file sections
 *                are found and validates the checksum of each section.
 *              - Provides "hard coded" setting file feature for the ACU and/or HMI sections as an alternative to
 *                storing setting file data on the main ACU. This feature only applies to an expansion board
 *                that requires one setting file section from the main ACU as well as some setting file data that cannot be modified.expansion boards. The binary
 *                output for the "hard coded" setting file(s) shall be merged with the firmware during the build process.
 *              - Determines the validity of the overall setting file. The setting file is not valid if: If at least one
 *                checksum validation fails, or if the ACU or HMI section is not found for an ACU, or if the HMI section
 *                is found for an HMI.
 *              - Provides an interface to load data from the setting file based on a pointer and a displacement.
 *              - Provides an interface to basic setting file information: CRC of each setting file section, size of each
 *                setting file section, size of entire setting file, start address of each setting file section and start
 *                address of the entire setting file.
 *
 *  @defgroup   CLASS_B
 *
    @startuml{SettingFile_ClassDiagram.png}

        class "SettingFile" as core {
            +SETTINGFILE_OFFSET_TYPE
            +SETTINGFILE_CROSS_PRODUCT_DISPLACEMENT_TYPE
            +void SettingFile__Initialize(void);
            +BOOL_TYPE SettingFile__IsPlatformPointerUsed(uint8 sf_index, SETTINGFILE_OFFSET_TYPE platform_pointer);
            +BOOL_TYPE SettingFile__IsValid(void);
            +void SettingFile__CheckDataIntegrity(void);
            +uint8 SettingFile__GetNumExtraSF(void);
            +PASS_FAIL_TYPE SettingFile__BasicLoader(SETTINGFILE_OFFSET_TYPE pointer_id, uint16 displacement, SETTINGFILE_LOADER_TYPE * pointer_data_out);
            +PASS_FAIL_TYPE SettingFile__MSettingBasicLoader( uint8 sf_index,  SETTINGFILE_OFFSET_TYPE  pointer_id, uint16 displacement,  SETTINGFILE_LOADER_TYPE * data_pointer);
            +uint16 SettingFile__GetNumDisplacements(uint8 sf_index, SETTINGFILE_OFFSET_TYPE pointer_id);
            +BOOL_TYPE SettingFile__IsExtendedDisplacement(uint8 sf_index,SETTINGFILE_OFFSET_TYPE pointer_id);
            +uint16 SettingFile__GetSettingFileCRC(SF_ID_TYPE sf_id);
            +void * SettingFile__GetSettingFileStart(SF_ID_TYPE sf_id);
            +uint8 SettingFile__GetSectionIDs(SF_ID_TYPE* section_ids);
            +uint32 SettingFile__GetSettingFileSize(SF_ID_TYPE sf_id);
            +void * SettingFile__GetMemorySectionStart(void);
            +uint32 SettingFile__GetMemorySectionSize(void);
            +void * SettingFile__GetSettingFilePointerByIndex(SF_ID_TYPE sf_id,uint32 index);
            +uint16 SettingFile__CalculateCRC(SF_ID_TYPE sf_id);
            }

       class "SettingFile_defs.h" as defs  <<interface>>{
            +SETTINGFILE_CROSS_CATEGORY_SF_PTR
       }

       class "SettingFile_prm.h" as cat_dynamic  <<interface>>{
            +SETTINGFILE_APPLICATION_SPECIFIC_SF_PTR
       }

       class "SettingFile_SRCat.h" as srcat_dynamic   <<interface>>{
            +SETTINGFILE_APPLICATION_SPECIFIC_SR_SF_PTR
       }
	   
       core ..> defs
       core ..> cat_dynamic
       core ..> srcat_dynamic

    @enduml
 *  @copyright  Copyright 2015-2017 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SRSystemConfig.h"
#include "SystemConfig.h"
#include "SettingFile.h"
#include "SettingFile_prv.h"

#include "Crc16.h"
#include "Log.h"
#include "SRData.h"
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    #include "SRTaskSequence.h"
#endif
#include <string.h>

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

#ifndef SF_START_ADDRESS
    //! Linker symbol that must be defined in config.icf The value of the variable should contain the starting address of the setting file.
    extern uint16 __SetingFile_Start;

    /**
     *  @brief      Start address of the memory that contains the setting file data.
     *
     *  @details    There are 2 use cases for this macro.
     *              1. Typical usage of this macro: Get the setting file address from the linker symbol (__SetingFile_Start)
     *              defined in the config.icf file. This will require an extern statement before this macro.
     *              (Example: extern uint16 __SetingFile_Start; )
     *              2. A GUI project can utilize this macro to look up the address of the setting file data, as the address
     *              will change every time the system boots.
     */
    #define SF_START_ADDRESS   (&__SetingFile_Start)
#endif

#ifndef SF_SIZE
    //! Linker symbol that must be define in config.icf. The value of the variable should contain the maximum size of the setting file.
    extern uint32 __SetingFile_Size;

    /**
     *  @brief      Size of the memory section that contains the setting file data.
     *
     *  @details    There are 2 use cases for this macro.
     *              1. Typical usage of this macro: Get the setting file size from the linker symbol (__SetingFile_Size)
     *              defined in the config.icf file. This will require an extern statement before this macro.
     *              (Example: extern uint32 __SetingFile_Size; )
     *              2. A GUI project can utilize this macro to return the value of a constant that defines the setting file size.
     */
    #define SF_SIZE  (&__SetingFile_Size)
#endif

#ifdef ENDIANU16_STON
    // If the setting file endianess conversion macro is defined, then it must be defined for little endian data.
    CT_ASSERT(ENDIANU16_STON(0xABCD) == 0xABCD);
#endif

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//-------------------------------------- IDs FOR LOG messages----------------------------------------------------------
//! A list of log message IDs that are generated by this module.
typedef enum MODULE_SETTINGFILE_LOG_MESSAGE_ID_STRUCT
{
    UNHANDLED_SECTION_ENDIANNESS,                   // Data: Section ID detected with big endianess.
    UNHANDLED_HARDCODED_ENDIANNESS                  // Data: Section ID detected with big endianess.
} MODULE_SETTINGFILE_LOG_MESSAGE_ID_TYPE;

//! The name of this module for use by the Log module.
#define MODULE_NAME     MODULE_SETTINGFILE
//------------------------------------------

typedef enum SETTINGFILE_BSLDR_RETURN_ENUM
{
    SETTINGFILE_BSLDR_OK                    =   0x00,   //BSLDR_OK                    0x00
    SETTINGFILE_BSLDR_INVALID_LENGTH        =   0x02,   //BSLDR_WRN_LENGHT            0x02
    SETTINGFILE_BSLDR_ERR_INVALID_DISPL     =   0x82,   //BSLDR_ERR_INVALID_DISPL     0x82
} SETTINGFILE_BSLDR_RETURN_TYPE;

// Defines the address of a hard coded ACU setting file (if used); Symbol should be defined in makefile.dat
#if (USE_ACU_HARDCODED_SETTING_FILE == ENABLED)
    extern uint16 __ACU_Hardcoded_SettingFile_Start;
#endif

// Defines the address of a hard coded HMI setting file (if used); Symbol should be defined in makefile.dat
#if (USE_HMI_HARDCODED_SETTING_FILE == ENABLED)
    extern uint16 __HMI_Hardcoded_SettingFile_Start;
#endif

#define SETTINGFILE_NULL_VALUE              0x0000
#define SETTINGFILE_MASK_SECTION_ID_TYPE    0x0F
#define SETTINGFILE_INITIAL_CRC             0xFFFF      // Initial value of the CRC to use during the calculation of the CRC

//! A structure to describe the header of each setting file section.
typedef PACKED struct SF_SECTION_HEADER_STRUCT
{
    BOOL_TYPE Endianess : 1;                        //!< 0 = Big endian, 1 = Little endian.
    uint8 Unused : 7;
    SF_ID_TYPE Section_ID;
} SF_SECTION_HEADER_TYPE;

//! Validity of the setting file (FALSE if there is something wrong with the setting file, else TRUE)
static BOOL_TYPE Is_Settingfile_Valid;
//! Inverted memory for the Class B variables used by this module.
static BOOL_TYPE N_Is_Settingfile_Valid;

//! Tracks the current setting file section whose content is being verified
static uint8 Settingfile_Section_Position;
//! Inverted memory for the Class B variable
static uint8 N_Settingfile_Section_Position;

//! Tracks the calculated CRC for the current setting file section whose content is being verified
static uint16 Settingfile_Flash_Crc;

//! Points to the current setting file data used during the validation of the CRC for a setting file section
static uint32 * Settingfile_Flash_Crc_Ptr;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static uint16 CalculateSectionCRC(uint16 * section_start_address);
static uint16 * GetSettingFileStartByID(SF_ID_TYPE sf_id);
static uint16 * GetSettingFileStartByPosition(uint8 sf_position);
static uint16 * ExtractAbsPointer(uint16 * data_struct_start_ptr , SETTINGFILE_OFFSET_TYPE pointer_id);
static SETTINGFILE_BSLDR_RETURN_TYPE GetDisplacementData(uint16 * data_start_address, uint16 displacement, SETTINGFILE_LOADER_TYPE * data_out_ptr);
#if ((USE_ACU_HARDCODED_SETTING_FILE == ENABLED) || (USE_HMI_HARDCODED_SETTING_FILE == ENABLED))
    static BOOL_TYPE IsHardCodedSettingFileValid(SF_ID_TYPE sf_id);
#endif

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module SettingFile and its variables.
 *
 *  @details    - Iterates through all the sections of the setting file, where the start address is defined by
 *                the SF_START_ADDRESS macro.
 *              - For each section, assigns a position in a local buffer so it can be referenced later.
 *              - For each section, validates the checksum of each section in the setting file.
 *              - For each section, validates the endianness compared to the endianness expected by the firmware
 *              - If no setting file is found, this function will also search in alternative memory locations for a
 *                "hard coded" setting file (one for ACU and one for HMI).
 *              - Determines the validity of the setting file. The setting file is not valid if:
 *                      - If at least one checksum validation fails, OR
 *                      - If the ACU or HMI section is not found for an ACU, OR
 *                      - If the HMI section is found for an HMI OR
 *                      - The endianness of at least one of the sections does not match the endianness expected by the
 *                          firmware
 */
void SettingFile__Initialize(void)
{
    // The 'has_type' variable contains a bit for each possible section type.
    // If the bit is set, then at least one valid section of that type was found.
    // Bit 0 for ACU sections, Bit 1 for HMI sections, Bit 2 for MCU sections, etc.
    uint16 has_type = 0;

    // Start with assumption that setting file is invalid
    uint8 is_settingfile_valid = FALSE;

    // Go sequentially through all the setting file sections.
    uint16* pt16 = SF_START_ADDRESS;
    uint8 section_count = 0;
    for (uint8 sf_position = 0; sf_position < SF_MAX_SECTIONS; sf_position++)
    {
        SF_ID_TYPE sf_id = ((SF_SECTION_HEADER_TYPE*)(void*)pt16)->Section_ID;
        if (sf_id == SF_ID_END)
        {
            // Reached the end of the setting file (no more sections).
            break;                                  // Break out of the for() loop.
        }
        else if (((SF_SECTION_HEADER_TYPE*)(void*)pt16)->Endianess == 0)
        {
            // The section endianess is incorrect. Treat the whole setting file as invalid.
            is_settingfile_valid = FALSE;
            LOG_ADD_EXCEPTION(UNHANDLED_SECTION_ENDIANNESS, sf_id);
            break;                                  // Break out of the for() loop.
        }
        else
        {
            pt16++;                                 // Point to the start of the section data.
            uint16 calculated_crc = CalculateSectionCRC(pt16);
            uint16 stored_crc = pt16[pt16[SF_PTR_ACU_CRC]];
            section_count++;
            if (stored_crc == calculated_crc)       // Verify the integrity of the setting file section by comparing the CRC
            {
                uint8 sf_type = (sf_id & SETTINGFILE_MASK_SECTION_ID_TYPE);
                BIT_SET(has_type, sf_type);
                is_settingfile_valid = TRUE;        // So far, all sections are good.
            }
            else
            {
                is_settingfile_valid = FALSE;       // This section is corrupt. Treat the whole setting file as corrupt.
                break;                              // Break out of the for() loop.
            }

            // Point to the next section of the setting file.
            pt16 = &pt16[pt16[SF_PTR_ACU_END_OF_SECTION]];
        }
    }

    // Look for hard-coded setting files.
    if (is_settingfile_valid == TRUE)
    {
        #if (USE_ACU_HARDCODED_SETTING_FILE == ENABLED)
            if ((IsHardCodedSettingFileValid(SF_ID_ACU) == FALSE) ||
                (BIT_TEST(has_type, SF_ID_ACU) == TRUE))
            {
                is_settingfile_valid = FALSE;
            }
        #endif
        #if (USE_HMI_HARDCODED_SETTING_FILE == ENABLED)
            if ((IsHardCodedSettingFileValid(SF_ID_HMI) == FALSE) ||
                (BIT_TEST(has_type, SF_ID_HMI) == TRUE))
            {
                is_settingfile_valid = FALSE;
            }
        #endif
    }
    else if (section_count == 0)
    {
        #if ((USE_ACU_HARDCODED_SETTING_FILE == ENABLED) && (USE_HMI_HARDCODED_SETTING_FILE == ENABLED))
            is_settingfile_valid = (BOOL_TYPE)(IsHardCodedSettingFileValid(SF_ID_ACU) &&
                                               IsHardCodedSettingFileValid(SF_ID_HMI));
        #elif (USE_ACU_HARDCODED_SETTING_FILE == ENABLED)
            is_settingfile_valid = IsHardCodedSettingFileValid(SF_ID_ACU);
        #elif (USE_HMI_HARDCODED_SETTING_FILE == ENABLED)
            is_settingfile_valid = IsHardCodedSettingFileValid(SF_ID_HMI)
        #endif
    }

    // Set module variable for validity of the setting file
    SRData__UpdateByte((uint8 *)(void *)&Is_Settingfile_Valid, (uint8 *)(void *)&N_Is_Settingfile_Valid, is_settingfile_valid);

    // Initialize module variable for setting file position to start with the section at the first position
    SRData__UpdateByte(&Settingfile_Section_Position, &N_Settingfile_Section_Position, 0);

    // Set initial value of the CRC for the setting file section CRC calculation
    Settingfile_Flash_Crc = SETTINGFILE_INITIAL_CRC;

    // Set pointer used during CRC calculation to the first setting file section
    Settingfile_Flash_Crc_Ptr = (uint32 *)(void *)SF_START_ADDRESS;

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_INIT_PHASE, SRTASKSEQUENCE_INIT_SETTINGFILE);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Validates the integrity of the setting file data by calculating a CRC on each section of the setting file
 *          and comparing it to the CRC stored in the setting file data.
 *
 * @details Each time this function is called, a different section is checked. It starts with the first section. The next
 *          time the function is called, it will check the next section. After the last section is reached, the process
 *          repeats again starting with the first section.
 *          If there is a CRC mismatch, the function sets a variable that will allow external modules to know that the
 *          setting file is no longer valid. If a section is invalid, safe behavior shall be implemented outside
 *          of this function.
 */
void SettingFile__CheckDataIntegrity(void)
{
    // Validate protected variable
    if (SR_CHECK_BYTE(Settingfile_Section_Position) == SRDATA_OK)
    {
        uint8 current_settingfile_position = Settingfile_Section_Position;
        uint16 * section_ptr16 = GetSettingFileStartByPosition(current_settingfile_position);           // Point to the start of the section data
        uint16 * data_crc_address = ExtractAbsPointer(section_ptr16, SF_PTR_ACU_CRC);

        // Validate pointers and protected variables before continuing
        if ((section_ptr16 != NULL) &&
            (data_crc_address != NULL))
        {
            uint16 remaining_bytes = (uint8 *)(void *)data_crc_address - (uint8 *)(void *)Settingfile_Flash_Crc_Ptr;

            if (remaining_bytes <= sizeof(uint32))                                                       // If at the end of the section
            {
                // Update the CRC of the setting file section based on the remaining byte(s)
                Settingfile_Flash_Crc = Crc16__RangeProcess((uint8 *)(void *)Settingfile_Flash_Crc_Ptr, Settingfile_Flash_Crc, remaining_bytes);

                // Compare the CRC value stored in the section of the setting file to the calculated CRC
                if (section_ptr16[section_ptr16[SF_PTR_ACU_CRC]] !=  Settingfile_Flash_Crc)             // If CRCs do not match, setting file is invalid
                {
                    // Indicate that the setting file is invalid
                    SRData__UpdateByte((uint8 *)(void *)&Is_Settingfile_Valid, (uint8 *)(void *)&N_Is_Settingfile_Valid, FALSE);
                }
                else                                                                                    // If CRCs match, check another section
                {
                    uint16 * end_of_section_ptr = ExtractAbsPointer(section_ptr16, SF_PTR_ACU_END_OF_SECTION);
                    SF_ID_TYPE section_id = ((SF_SECTION_HEADER_TYPE*)(void*)end_of_section_ptr)->Section_ID;
                    if (section_id == SF_ID_END)                                                        // If last section was validated, reset position so the first section is checked on the next iteration
                    {
                        current_settingfile_position = 0;
                        Settingfile_Flash_Crc_Ptr = (uint32 *)(void *)SF_START_ADDRESS;                 // Set pointer used during CRC calculation to the first setting file section
                    }
                    else                                                                                // If not at the last section, increment position so it is checked on the next iteration
                    {
                        current_settingfile_position++;
                        Settingfile_Flash_Crc_Ptr = (uint32 *)(void *)(end_of_section_ptr);             // Adjust pointer to skip over the word that contains the endianness and section ID (not included in CRC)
                    }

                    // Set initial value of the CRC for the setting file section CRC calculation
                    Settingfile_Flash_Crc = SETTINGFILE_INITIAL_CRC;

                    // Update setting file position to check next section
                    SRData__UpdateByte(&Settingfile_Section_Position, &N_Settingfile_Section_Position, current_settingfile_position);
                }
            }
            else    // Not at the end of the section yet
            {
                // Update the CRC of the setting file section based on the next bytes
                Settingfile_Flash_Crc = Crc16__RangeProcess((uint8 *)(void *)Settingfile_Flash_Crc_Ptr, Settingfile_Flash_Crc, sizeof(uint32));
                Settingfile_Flash_Crc_Ptr++;
            }
        }   // else, data_crc_address == NULL || section_ptr16 == NULL
        else
        {
            // Indicate that the setting file is invalid
            SRData__UpdateByte((uint8 *)(void *)&Is_Settingfile_Valid, (uint8 *)(void *)&N_Is_Settingfile_Valid, FALSE);
        }
    }   // else, corrupted variable
    else
    {
        // Indicate that the setting file is invalid
        SRData__UpdateByte((uint8 *)(void *)&Is_Settingfile_Valid, (uint8 *)(void *)&N_Is_Settingfile_Valid, FALSE);
    }

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_SETTINGFILE_EXECUTED);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Test platform_pointer to check if it is used (not equal to 0xFFFF).
 *
 * @param   sf_index: Not used, kept from previous idea of mSetting Files (may be removed in future)
 * @param   pointer_id: The pointer that specifies which section of the setting file to check.
 *
 * @return  TRUE if the platform pointer is used, else FALSE
 */
BOOL_TYPE SettingFile__IsPlatformPointerUsed(uint8 sf_index, SETTINGFILE_OFFSET_TYPE pointer_id)
{
    uint16 * data_start_address;

    data_start_address = GetSettingFileStartByID((SF_ID_TYPE)(pointer_id >> 8));      // Extract upper 8 bits as the setting file id (ACU = 0, HMI = 1)
    data_start_address = ExtractAbsPointer(data_start_address, pointer_id);

    if (data_start_address != NULL)
    {
        return(TRUE);
    }
    else
    {
        return(FALSE);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Returns the validity of the setting file.
 * @return  FALSE if there is something wrong with the setting file or the module variable is corrupt, else TRUE.
 */
BOOL_TYPE SettingFile__IsValid(void)
{
    if (SRData__CheckByte((uint8 *)(void *)&Is_Settingfile_Valid, (uint8 *)(void *)&N_Is_Settingfile_Valid) == SRDATA_OK)
    {
        return (Is_Settingfile_Valid);
    }
    else
    {
        return(FALSE);      // If data integrity check fails, force return value to report that it is an invalid setting file
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Returns the number of "Extra" setting files
 * @details This feature is not currently used.
 * @return  Number of "Extra" setting files
 */
uint8 SettingFile__GetNumExtraSF(void)
{
    return (0);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Loads setting file data for a displacement within a pointer.
 * @param   pointer_id: See SETTINGFILE_OFFSET_TYPE
 * @param   displacement: The displacement (or offset) into the pointer where the data is located
 * @param   sf_data_ptr: Pointer to the setting file data (see SETTINGFILE_LOADER_TYPE)
 * @return  TRUE if valid data was found at the given pointer and displacement.
 */
PASS_FAIL_TYPE SettingFile__BasicLoader(SETTINGFILE_OFFSET_TYPE pointer_id, uint16 displacement, SETTINGFILE_LOADER_TYPE * sf_data_ptr)
{
    uint16 * data_start_address;

    data_start_address = GetSettingFileStartByID((SF_ID_TYPE)(pointer_id >> 8));         // Extract upper 8 bits as the setting file id (ACU = 0, HMI = 1)
    data_start_address = ExtractAbsPointer(data_start_address, pointer_id);

    if ((data_start_address != NULL) &&
        (GetDisplacementData(data_start_address, displacement, sf_data_ptr) == SETTINGFILE_BSLDR_OK))
    {
        return(PASS);
    }
    else
    {
        return(FAIL);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Loads setting file data from an mSetting File. NOTE: mSetting files are currently not supported.
 */
PASS_FAIL_TYPE SettingFile__MSettingBasicLoader(uint8 sf_index, SETTINGFILE_OFFSET_TYPE pointer_id, uint16 displacement, SETTINGFILE_LOADER_TYPE * sf_data_ptr)
{
    return(SettingFile__BasicLoader(pointer_id, displacement, sf_data_ptr));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Get the number of displacements referenced by a setting file index and a pointer.
 * @param   sf_index: The index that specifies which setting file to check. NOTE: mSetting Files not currently supported.
 * @param   pointer_id: The pointer that specifies which section of the setting file to check.
 * @return  The number of displacements referenced by a setting file index and a pointer.
 */
uint16 SettingFile__GetNumDisplacements(uint8 sf_index, SETTINGFILE_OFFSET_TYPE pointer_id)
{
    uint16 num_of_displacements = 0;

    uint16 *data_start_address = GetSettingFileStartByID((SF_ID_TYPE)(pointer_id >> 8));     // Extract upper 8 bits as the setting file id (ACU = 0, HMI = 1)
    data_start_address = ExtractAbsPointer(data_start_address, pointer_id);
    if (data_start_address != NULL)
    {
        // Check for displacements that use the extended format.
        uint8 *byte_ptr = (void*)data_start_address;
        if ((byte_ptr[0] & 0x80) == 0)
        {
            // Displacements use normal format (1 byte per displacement).
            data_start_address = &data_start_address[byte_ptr[0]];                  // Skip over extra-pointers field.
            byte_ptr = (void*)data_start_address;
            num_of_displacements = (2 * byte_ptr[0]) - 1;
            if (byte_ptr[num_of_displacements - 1] == byte_ptr[num_of_displacements])
            {
                num_of_displacements--;             // Don't count last displacement (zero-length).
            }
        }
        else
        {
            // Displacements use extended format (2 bytes per displacement).
            data_start_address = &data_start_address[1];     // Skip over extra-pointers field.
            num_of_displacements = (data_start_address[0]) - 1;
        }
    }

    return (num_of_displacements);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Determine whether the specified pointer uses the extended displacement format.
 *
 * @param   sf_index: The index of the setting file to check.
 * @param   pointer_id: The specific section of the setting file to check.
 *
 * @return TRUE if the specified pointer uses the extended displacement format, else FALSE.
 */
BOOL_TYPE SettingFile__IsExtendedDisplacement(uint8 sf_index, SETTINGFILE_OFFSET_TYPE pointer_id)
{
    uint16 * data_start_address;

    data_start_address = GetSettingFileStartByID((SF_ID_TYPE)(pointer_id >> 8));    // Extract upper 8 bits as the setting file id (ACU = 0, HMI = 1)
    data_start_address = ExtractAbsPointer(data_start_address, pointer_id);

    if (data_start_address != NULL)
    {
        uint8* byte_ptr = (void*)data_start_address;
        return ((BOOL_TYPE)((byte_ptr[0] & 0x80) != 0));
    }
    else
    {
        return(FALSE);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Get the CRC of a specific setting file section.
 * @param   sf_id: ID of the setting file section (See SF_ID_TYPE)
 * @return  CRC of a setting file section
 */
uint16 SettingFile__GetSettingFileCRC(SF_ID_TYPE sf_id)
{
    uint16 * data_crc_address;
    data_crc_address = ExtractAbsPointer(GetSettingFileStartByID(sf_id), SF_PTR_ACU_CRC);

    if (data_crc_address != NULL)
    {
        return (*data_crc_address);
    }
    else
    {
        return 0;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get a pointer to the start of the setting file.
 * @return A pointer to the start of the setting file.
 */
void * SettingFile__GetMemorySectionStart(void)
{
    return ((void *)SF_START_ADDRESS);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @return  Size of the memory section reserved for the entire setting file.
 */
uint32 SettingFile__GetMemorySectionSize(void)
{
    return ((uint32)SF_SIZE);       //lint !e923 cast from pointer to unsigned long [MISRA 2004 Rule 11.1],[MISRA 2004 Rule 11.3]
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get a pointer to the data in the specified section of the setting file.
 * @param sf_id = The section ID of the requested setting file section.
 * @return A pointer to the data in the specified section of the setting file.
 *         If the setting file section is not found, then return NULL.
 */
void * SettingFile__GetSettingFileStart(SF_ID_TYPE sf_id)
{
    uint16 * pt;
    // Get the address of the first pointer
    pt = GetSettingFileStartByID(sf_id);

    if (pt != NULL)
    {
        //Decrement to include the SF ID
        pt--;
    }
    return ((void *)pt);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Determine the section ID of the section at the specified position.
 * @param   section_ids: Pointer to a list of section IDs, See SF_ID_TYPE
 * @return  The section ID of the section at the specified position.
 *          If there is no section at the specified position, then return SF_ID_END.
 */
uint8 SettingFile__GetSectionIDs(SF_ID_TYPE * section_ids)
{
    memset(section_ids, SF_ID_END, SF_MAX_SECTIONS);
    uint8 position = 0;

    if (SettingFile__IsValid() == TRUE)
    {
        uint16* ptr = SF_START_ADDRESS;
        for (position = 0; position < SF_MAX_SECTIONS; position++)
        {
            SF_ID_TYPE section_id = ((SF_SECTION_HEADER_TYPE*)(void*)ptr)->Section_ID;
            if (section_id == SF_ID_END)
            {
                break;                                  // Break out of the for() loop.
            }
            section_ids[position] = section_id;
            ptr++;
            ptr = &ptr[ptr[SF_PTR_ACU_END_OF_SECTION]];
        }
    }
    return position;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the address of a specific byte in a section of the setting file.
 * @param sf_id = The section of the setting file that contains the byte.
 * @param index = The index of the byte from the beginning of the setting file section.
 * @return The address of a specific byte in a section of the setting file.
 */
void * SettingFile__GetSettingFilePointerByIndex(SF_ID_TYPE sf_id, uint32 index)
{
    uint8 * pt;
    pt = SettingFile__GetSettingFileStart(sf_id);

    //Check if the accessed memory within the setting file range
    if ((pt != NULL) &&
        ((uint16 *)(&pt[index]) < (uint16 *)((uint32)(void *)SF_START_ADDRESS + (uint32)SF_SIZE))) //lint !e927 Lint: 927 cast from pointer to pointer [MISRA 2004 Rule 11.4]
    {
        return ((void *)&pt[index]);
    }
    else
    {
    	return NULL;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * Get the number of bytes in a the specified section of the setting file.
 * @param sf_id = The section ID for the requested section of the setting file.
 * @return The number of bytes in a the specified section of the setting file.
 *         If the specified section does not exist, then return 0.
 */
uint32 SettingFile__GetSettingFileSize(SF_ID_TYPE sf_id)
{
    uint16 size = 0;    // Initialize to zero in case setting file with ID is not found

    // Get the first pointer of the specific setting file
    uint16 * data_address = GetSettingFileStartByID(sf_id);

    if (data_address != NULL)
    {
        // offset by the pointer
        // (Needed in case SF_PTR_ACU_END_OF_SECTION ever changes from 0 to something else.)
        data_address += (SF_PTR_ACU_END_OF_SECTION & 0x00FF);

        // Get the content
        size = (*data_address);

        // Convert from number of words to number of bytes.
        size <<= 1;

        // Add 2 to include the section header.
        size += 2;
    }

    return (size);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Calculates the CRC of a setting file section.
 * @param   sf_id: see SF_ID_TYPE
 * @return  CRC of a setting file section
 */
uint16 SettingFile__CalculateCRC(SF_ID_TYPE sf_id)
{
    return (CalculateSectionCRC(GetSettingFileStartByID(sf_id)));
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Calculates the CRC of a setting file section
 * @param   section_start_address: Address of the start of the setting file data (does not include first word that
 *          contains the section ID and endianness)
 * @return  CRC of the setting file section
 */
static uint16 CalculateSectionCRC(uint16 * section_start_address)
{
    uint16   crc = 0;   // Initialize value to zero in case the setting file with the requested ID is not found
    uint16 * data_crc_address;

    data_crc_address = ExtractAbsPointer(section_start_address, SF_PTR_ACU_CRC);

    if (data_crc_address != NULL)
    {
        uint8 * byte_ptr = (void*)(section_start_address - 1);
        uint8 * end_of_data_ptr = (void*)data_crc_address;

        crc = 0xFFFF;

        while (byte_ptr < end_of_data_ptr)
        {
            crc = Crc16__ByteProcess(crc,*byte_ptr);
            byte_ptr++;
        }
    }

    return (crc);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Get the offset to the start of a setting file section
 * @param   sf_id: see SF_ID_TYPE
 * @return  Number of words offset from the start of the whole setting file to the start of a setting file section
 */
static uint16 * GetSettingFileStartByID(SF_ID_TYPE sf_id)
{
    uint16 * ptr = NULL;

    if (SettingFile__IsValid() == TRUE)
    {
        BOOL_TYPE skip_search = FALSE;

        #if (USE_ACU_HARDCODED_SETTING_FILE == ENABLED)
            if (sf_id == SF_ID_ACU)
            {
                ptr = (void*)&__ACU_Hardcoded_SettingFile_Start;
                ptr++;       // Adjust pointer to skip over the word for the section ID and point to the End Of Section data
                skip_search = TRUE;
            }
        #endif
        #if (USE_HMI_HARDCODED_SETTING_FILE == ENABLED)
            if (sf_id == SF_ID_HMI)
            {
                ptr = (void*)&__HMI_Hardcoded_SettingFile_Start;
                ptr++;       // Adjust pointer to skip over the word for the section ID and point to the End Of Section data
                skip_search = TRUE;
            }
        #endif

        uint8 sf_type = (sf_id & SETTINGFILE_MASK_SECTION_ID_TYPE);
        uint16* remember_ptr = NULL;
        if (skip_search == FALSE)
        {
            ptr = SF_START_ADDRESS;
            SF_ID_TYPE section_id = ((SF_SECTION_HEADER_TYPE*)(void*)ptr)->Section_ID;
            ptr++;
            for (uint8 section_position = 0; section_position < SF_MAX_SECTIONS; section_position++)
            {
                uint8 section_type = (section_id & SETTINGFILE_MASK_SECTION_ID_TYPE);
                if (section_id == sf_id)                // Found the requested setting file section.
                {
                    break;                              // Break out of the for() loop.
                }
                else if ((remember_ptr == NULL) && (section_type == sf_type))
                {
                    // This is the main ACU or main HMI section.
                    // Save the pointer in case it is what is really requested.
                    remember_ptr = ptr;
                }
                else if (section_id == SF_ID_END)
                {
                    ptr = NULL;
                    break;                              // Break out of the for() loop.
                }
                ptr = &ptr[ptr[SF_PTR_ACU_END_OF_SECTION]];
                section_id = ((SF_SECTION_HEADER_TYPE*)(void*)ptr)->Section_ID;
                ptr++;
            }
        }
        // When an expansion board requests the main ACU or main HMI section, it will get the first ACU or HMI section.
        if ((ptr == NULL) && ((sf_id == SF_ID_ACU) || (sf_id == SF_ID_HMI)))
        {
            ptr = remember_ptr;
        }
    }
    return (ptr);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Returns the start address of the setting file data based on the position of the section in the setting file.
 * @param   sf_position: position of the section in the setting file
 */
static uint16 * GetSettingFileStartByPosition(uint8 sf_position)
{
    uint16* ptr16 = NULL;

    if (SettingFile__IsValid() == TRUE)
    {
        ptr16 = SF_START_ADDRESS;
        for (uint8 position = 0; position < SF_MAX_SECTIONS; position++)
        {
            SF_ID_TYPE section_id = ((SF_SECTION_HEADER_TYPE*)(void*)ptr16)->Section_ID;
            if (section_id == SF_ID_END)
            {
                ptr16 = NULL;                           // Invalidate the pointer if the end of the setting file is reached
                break;                                  // Break out of the for() loop.
            }
            else if (position == sf_position)
            {
                break;                                  // Reached the desired section so break out of the for() loop
            }

            ptr16++;                                    // Adjust pointer to point to the beginning of the data (excluded section ID and endianness)
            ptr16 = &ptr16[ptr16[SF_PTR_ACU_END_OF_SECTION]];   // Adjust pointer to point to the start of the next section
        }

        ptr16++;                                        // Adjust pointer to point to the beginning of the data (excluded section ID and endianness)
    }

    return(ptr16);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   This routine give absolute address in a big endian 16-bit pointers table
 * @details former LIB_SEF_GetBEWrdRelShort_Ptr
 * @param   data_struct_start_ptr
 * @param   pointer_id
 * @return
 */
static uint16 * ExtractAbsPointer(uint16 * data_struct_start_ptr, SETTINGFILE_OFFSET_TYPE pointer_id)
{
    uint16 * retval;
    uint16 pointer_offset;

    retval = NULL;

    if ((pointer_id != SF_PTR_INVALID_VALUE) &&
        (data_struct_start_ptr != NULL))
    {
        // Find the offset to apply to the start address
        pointer_offset = (data_struct_start_ptr[pointer_id & 0x00FF]);   // Extract lower 8 bits to get the pointer

        if ((SETTINGFILE_NULL_VALUE != pointer_offset) &&
            (SF_PTR_INVALID_VALUE != pointer_offset))
        {
            retval = data_struct_start_ptr + pointer_offset;
        }

        //Check if the accessed memory within the setting file range
        if (retval >= (uint16 *)((uint32)(void *)SF_START_ADDRESS + (uint32)SF_SIZE)) //lint !e923 Lint: 923 cast from pointer to unsigned long [Encompasses MISRA 2004 Rule 11.1], [MISRA 2004 Rule 11.3]
        {
            retval = NULL;
        }
    }

    return retval;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get the address and size of the data block associated with the specified displacement.
 *
 *  @details    former LIB_SEF_GetDisplData
 *
 *  @param data_start_address = Address of the start of the data in the setting file referenced by a pointer.
 *  @param displacement = The displacement used to get the data block.
 *  @param data_out_ptr = A structure to fill with the address and length of the data block.
 *
 *  @return SETTINGFILE_BSLDR_OK if the data is found and referenced successfully.
 */
static SETTINGFILE_BSLDR_RETURN_TYPE GetDisplacementData(uint16 * data_start_address, uint16 displacement, SETTINGFILE_LOADER_TYPE * data_out_ptr)
{
    SETTINGFILE_BSLDR_RETURN_TYPE retval = SETTINGFILE_BSLDR_ERR_INVALID_DISPL;

    if (SettingFile__IsValid() == TRUE)
    {
        BOOL_TYPE is_extended_displacement;
        uint16 num_page;
        uint16 num_page_end;
        uint16 num_extra_ptr_word;
        uint8 * data_ptr;

        retval = SETTINGFILE_BSLDR_OK;

        uint8* byte_ptr = (void*)data_start_address;
        is_extended_displacement = (BOOL_TYPE)((byte_ptr[0] & 0x80) != 0);
        if (is_extended_displacement)
        {
            /* Extended Displacement => 16 bit */
            //data_start_address points to the begin of the displacement array
            data_start_address++;

            // Check for a displacement that is out of range. (This includes a displacement of 0xFFFF.)
            if (displacement >= ((*data_start_address) - 1))
            {
                retval = SETTINGFILE_BSLDR_ERR_INVALID_DISPL;
            }
            else
            {
                // the calculation is valid even in a case where *( EepData + Displacement + 1 ) < *( EepData + Displacement ) which
                // can cause a page overflow
                data_out_ptr->Length = ((data_start_address[displacement + 1]) - (data_start_address[displacement])) * 2;
                data_out_ptr->Data = (void*)&data_start_address[(data_start_address[displacement])];
                if (0 == data_out_ptr->Length)
                {
                    retval = SETTINGFILE_BSLDR_INVALID_LENGTH;
                }
            }
        }
        else
        {
            byte_ptr = (void*)data_start_address;
            num_extra_ptr_word = byte_ptr[0];

            // Control the ExtraPointer forcing the safe page
            num_page = 0;
            num_page_end = 0;

            data_ptr = (void*)(data_start_address);

            for (uint8 i = 1; i < (2 * num_extra_ptr_word); i++)
            {
                /* displacement < 0xFF */
                if (displacement >= data_ptr[i])
                {
                    num_page += 0x0100U;
                }

                if ((displacement+1) >= data_ptr[i])
                {
                    num_page_end += 0x0100U;
                }
            }
            data_ptr += (2 * num_extra_ptr_word);/* data_ptr points to the begin of displacement */

            // Check for a displacement that is out of range. (This includes a displacement of 0xFF.)
            if (displacement >= ((2 * (*data_ptr)) - 1))
            {
                retval = SETTINGFILE_BSLDR_ERR_INVALID_DISPL;
            }
            else
            {
                data_out_ptr->Length = ( num_page_end + *( data_ptr + displacement + 1 ) - ( *( data_ptr + displacement ) + num_page) ) * 2;

                data_out_ptr->Data = (data_ptr + (2 * (*(data_ptr + displacement) + num_page)));

                if (0 == data_out_ptr->Length)
                {
                    retval = SETTINGFILE_BSLDR_INVALID_LENGTH;
                }
            }
        }
    }

    return (retval);
}

#if ((USE_ACU_HARDCODED_SETTING_FILE == ENABLED) || (USE_HMI_HARDCODED_SETTING_FILE == ENABLED))
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Checks if the hard coded setting file is valid, if it exists
 * @param   sf_id: see SF_ID_TYPE
 * @return  TRUE if there is a hard coded setting file for the requested setting file ID and its CRC is valid, else FALSE
 */
static BOOL_TYPE IsHardCodedSettingFileValid(SF_ID_TYPE sf_id)
{
    BOOL_TYPE crc_matches = FALSE;

    if (SettingFile__IsValid() == TRUE)
    {
        uint16 calculated_crc;
        uint16 * crc_address = NULL;
        uint16 * sf_start_address = NULL;
        uint16 stored_crc;

        #if (USE_ACU_HARDCODED_SETTING_FILE == ENABLED) // Note: Intentionally not calling GetSettingFileStartByID() here because this function is limited
            if (sf_id == SF_ID_ACU)                     //       to hard-coded setting files only
            {
                sf_start_address = (void*)&__ACU_Hardcoded_SettingFile_Start;
            }
        #endif

        #if (USE_HMI_HARDCODED_SETTING_FILE == ENABLED)
            if (sf_id == SF_ID_HMI)
            {
                sf_start_address = (void*)&__HMI_Hardcoded_SettingFile_Start;
            }
        #endif

        if (sf_start_address != NULL)
        {
            if (((SF_SECTION_HEADER_TYPE*)(void*)sf_start_address)->Endianess == 0)
            {
                // The section endianess is incorrect. Treat the whole setting file as invalid.
                LOG_ADD_EXCEPTION(UNHANDLED_HARDCODED_ENDIANNESS, sf_id);
            }
            else
            {
                sf_start_address++;                     // Adjust pointer to skip over the word for the section ID and point to the End Of Section data

                /**
                 * This routine give absolute address in a big endian 16-bit pointers table
                 */
                if (sf_start_address != NULL)
                {
                    // Find the offset to apply to the start address
                    uint16 pointer_offset = (sf_start_address[SF_PTR_ACU_CRC & 0x00FF]);   // Extract lower 8 bits to get the pointer

                    if ((SETTINGFILE_NULL_VALUE != pointer_offset) &&
                        (SF_PTR_INVALID_VALUE != pointer_offset))
                    {
                        crc_address = sf_start_address + pointer_offset;
                    }
                }

                if (crc_address != NULL)
                {
                    calculated_crc = SettingFile__CalculateCRC(sf_id);  // Calculate the CRC based on the data in the setting file memory location
                    stored_crc = (*crc_address);        // Extract the CRC stored in the setting file

                    if (stored_crc == calculated_crc)
                    {
                        crc_matches = TRUE;
                    }
                }
            }
        }
    }

    return(crc_matches);
}
#endif

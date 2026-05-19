/**
 *  @file
 *
 *  @brief      This module manages the process to download the setting file section and displacement data from the ACU
 *
 *      Handling of unaligned section data
 *      When we have more then one section of setting file and if the previous section does not end with the correct FLASH_ALIGNMENT than the previous
 *      or if FLASH_ALGINMENT size is greater than maximum data transferred through Reveal, even if there is one section only.
 *
 *      The following sequence is used
 *
@startuml{SFUpdater_Unaliged_Section_Data_Handling.png}
    title SFUpdater handling Unaligned data Sequence Diagram

    (*) --> "Set first Section of Setting file"
    --> "Get one block of data in the current Section of Setting file"
    if "Is the received block size less than FLASH_ALIGNMENT" then
        -->[true]   if "Is Data ends at unaligned address" then
                      --> [true] "Write block and clear the Temporary Buffer"
                      --> "Prepare next block to get"
                    else
                      -->[false] "Append data to Temporary buffer"
                      --> "Prepare next block to get"
                    endif
    else
          -->[false] "Write to Flash"
          --> "Prepare next block to get"
    endif
    "Prepare next block to get" --> if "Is there more data to get from current section" then
       -u> [TRUE] "Get one block of data in the current Section of Setting file"
     else
         --> if "Is there another setting file section to get" then
           -u-> [TRUE] "Set next Section of Setting file"
           -u> "Get one block of data in the current Section of Setting file"
         else
          --> [Ending process] (*)
        endif
    endif
@enduml

 *
 *  @copyright  Copyright 2015-$Date: 2015/07/01 13:51:14EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SFUpdater.h"
#include "Win.h"
#include "Reveal.h"
#include "Flash.h"
#include "API007Data.h"
#include "Crc16.h"
#include "Millis.h"
#include "string.h"

#if (SETTINGFILE_MASTER_FILE_FEATURE == ENABLED)
    #define SFUPDATER_MANAGE_SF_ID_DATA                 DISABLED
    #define SFUPDATER_MANAGE_DISPLACEMENT_DATA_ONFLASH  DISABLED
#else
    #if defined(SFUPDATER_ACU_SECTION_ID) || defined(SFUPDATER_HMI_SECTION_ID) || defined(SFUPDATER_MCU_SECTION_ID)
        #define SFUPDATER_MANAGE_SF_ID_DATA             ENABLED
    #else
        #define SFUPDATER_MANAGE_SF_ID_DATA             DISABLED
    #endif

    #ifdef SFUPDATER_DISPLACEMENT_LIST_DEF
        #define SFUPDATER_MANAGE_DISPLACEMENT_DATA_ONFLASH      ENABLED
    #else
        #define SFUPDATER_MANAGE_DISPLACEMENT_DATA_ONFLASH      DISABLED
    #endif
#endif

#if (SFUPDATER_MANAGE_SF_ID_DATA == ENABLED)
    #include "SettingFile.h"
    #include "Mode.h"
#endif

#if (SFUPDATER_MANAGE_DISPLACEMENT_DATA_ONFLASH == ENABLED)
    #include "Gpio.h"
    #include "Timer.h"
#endif

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------


//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#if (SFUPDATER_MANAGE_SF_ID_DATA == ENABLED)
    typedef enum
    {
        SFUPDATER_STATE_INITIALIZE = 0,
        SFUPDATER_STATE_ERASE_FLASH,
        SFUPDATER_STATE_WAIT_ACU_ALIVE,
        SFUPDATER_STATE_WAIT_MSG_SIZE,
        SFUPDATER_STATE_WAIT_SF_INFO,
        SFUPDATER_STATE_PROGRAMMING_DATA,
        SFUPDATER_STATE_DATA_END
    }SFUPDATER_STATE_TYPE;

    static SFUPDATER_STATE_TYPE SFUpdater_State;
    static uint8 SFUpdater_Section_Index;           //!< Index into the SFUPDATER_SECTIONS[] array.
    //! The section address allows us to use the SFUpdater_Data_Index as the offset in requests for data from the ACU.
    static FLASH_ADDRESS_TYPE SFUpdater_Section_Address;
    static uint8 SFUpdater_Reveal_Max_Size;

    static uint16 SFUpdater_Data_Size;
    static uint16 SFUpdater_Data_Index;

    static FLASH_ADDRESS_TYPE SFUpdater_Erase_Address;
    static MILLIS_TIMER_TYPE SFUpdater_Timer;

    //! A list of the setting file sections that are transferred from the ACU.
    static const SF_ID_TYPE SFUPDATER_SECTIONS[] =
    {
        #ifdef SFUPDATER_ACU_SECTION_ID
            SFUPDATER_ACU_SECTION_ID,
        #endif
        #ifdef SFUPDATER_HMI_SECTION_ID
            SFUPDATER_HMI_SECTION_ID,
        #endif
        #ifdef SFUPDATER_MCU_SECTION_ID
            SFUPDATER_MCU_SECTION_ID,
        #endif
        SF_ID_END
    };

    // Unaligned section data issue could be present if we have more then one section of data in the setting file
    #ifdef SFUPDATER_ACU_SECTION_ID
        #ifdef SFUPDATER_HMI_SECTION_ID
            #define SFUPDATER_ENABLE_UNALIGNED_MEMORY_HANDLING
        #else
            #ifdef SFUPDATER_MCU_SECTION_ID
                #define SFUPDATER_ENABLE_UNALIGNED_MEMORY_HANDLING
            #endif
        #endif
    #else
        #ifdef SFUPDATER_HMI_SECTION_ID
            #ifdef SFUPDATER_MCU_SECTION_ID
                #define SFUPDATER_ENABLE_UNALIGNED_MEMORY_HANDLING
            #endif
        #endif
    #endif

    // Unaligned section data could be present also if Flash write alignment block is greater than the maximum data block coming from Reveal
    #if(FLASH_ALIGNMENT > (REVEAL_PAYLOAD_INCOMING_SIZE - 6))
        #ifndef SFUPDATER_ENABLE_UNALIGNED_MEMORY_HANDLING
            #define SFUPDATER_ENABLE_UNALIGNED_MEMORY_HANDLING
        #endif
        #define SFUPDATER_REVEAL_DATA_ALIGNMENT          4
    #endif

    #ifdef SFUPDATER_ENABLE_UNALIGNED_MEMORY_HANDLING
        #define SFUPDATER_FLASH_DEFAULT_VALUE           0xFF

        typedef enum
        {
            SFUPDATER_UNALIGNED_DATA_NONE = 0,
            SFUPDATER_UNALIGNED_DATA_BUFFERING,
            SFUPDATER_UNALIGNED_DATA_BUFFER_WRITE,
        }SFUPDATER_UNALIGNED_DATA_STATE;


        //Handles the unaligned section data
        static uint8 SFUpdater_Flash_Buffer[FLASH_ALIGNMENT];
        static uint16 SFUpdater_Flash_Buffer_Write_Index;
        static SFUPDATER_UNALIGNED_DATA_STATE SFUpdater_Unaligned_Data_State;
    #endif
    
    #ifndef SFUPDATER_REVEAL_DATA_ALIGNMENT
        #define SFUPDATER_REVEAL_DATA_ALIGNMENT      FLASH_ALIGNMENT
    #endif
#endif      // #if (SFUPDATER_MANAGE_SF_ID_DATA == ENABLED)


//! Below macros and variables are to manage the Displacement Data over RAM
#define SFUPDATER_RAM_DATA_ALIGNMENT                        1
#define SFUPDATER_DISPLACEMENT_RESPONSE_TIMEOUT             1000
#define SFUPDATER_DISPLACEMENT_DATA_READ_TIMEOUT            200

static SFUPDATER_DISPLACEMENT_INFO_TYPE SFUpdater_Requested_Displacement_Info;
static SFUPDATER_DISPLACEMENT_INFO_TYPE SFUpdater_Received_Displacement_Info;
static SFUPDATER_REQUESTER_STATUS_TYPE SFUpdater_Requester_Status;
static uint16 SFUpdater_Displacement_Data_Index;
static FLASH_ADDRESS_TYPE SFUpdater_Displacement_Address;
static uint8 SFUpdater_Displacement_Reveal_Max_Size;
static MILLIS_TIMER_TYPE SFUpdater_Requester_Timer;
static uint16 SFUpdater_Requester_Timeout;


#if (SFUPDATER_MANAGE_DISPLACEMENT_DATA_ONFLASH == ENABLED)
    //! Below macros and variables are to manage the Displacement Data over FLASH

    #define SFUPDATER_ONE_MILLISECOND_IN_MICROSECOND_UNIT       1000U
    #define SFUPDATER_FIVE_MILLISECOND_IN_MS_UNIT               5U
    #define SFUPDATER_ONE_SECOND_IN_FIVE_MS_UNIT                200U
    #define SFUPDATER_TEN_SECOND_IN_FIVE_MS_UNIT                2000U
    #define SFUPDATER_NUM_DISPLACEMENT                          (sizeof(SFUPDATER_DISPLACEMENT_LIST) / sizeof(SFUPDATER_DISPLACEMENT_LIST_TYPE))

    typedef enum
    {
        SFUPDATER_DISPLACEMENT_STATE_INITIALIZE = 0,             //!< SFUPDATER_DISPLACEMENT_STATE_INITIALIZE
        SFUPDATER_DISPLACEMENT_STATE_WAIT_ACU_ALIVE,             //!< SFUPDATER_DISPLACEMENT_STATE_WAIT_ACU_ALIVE
        SFUPDATER_DISPLACEMENT_STATE_WAIT_MSG_SIZE,              //!< SFUPDATER_DISPLACEMENT_STATE_WAIT_MSG_SIZE
        SFUPDATER_DISPLACEMENT_STATE_REQUEST_SFDISPLACEMENT_INFO,//!< SFUPDATER_DISPLACEMENT_STATE_REQUEST_SFDISPLACEMENT_INFO
        SFUPDATER_DISPLACEMENT_STATE_WAIT_SFDISPLACEMENT_INFO,   //!< SFUPDATER_DISPLACEMENT_STATE_WAIT_SFDISPLACEMENT_INFO
        SFUPDATER_DISPLACEMENT_STATE_PROGRAMMING_DATA,           //!< SFUPDATER_DISPLACEMENT_STATE_PROGRAMMING_DATA
        SFUPDATER_DISPLACEMENT_STATE_PROGRAMMING_END             //!< SFUPDATER_DISPLACEMENT_STATE_PROGRAMMING_END
    }SFUPDATER_DISAPLACEMENT_STATE_TYPE;

    static SFUPDATER_DISAPLACEMENT_STATE_TYPE SFUpdater_Displacement_State;

    static const SFUPDATER_DISPLACEMENT_LIST_TYPE SFUPDATER_DISPLACEMENT_LIST[] = SFUPDATER_DISPLACEMENT_LIST_DEF;
    extern uint16 __SFDisplacement_Start;
    extern uint8 __SFDisplacement_Size;

#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
#if (SFUPDATER_MANAGE_SF_ID_DATA == ENABLED)
    static BOOL_TYPE RequestSFData(void);
    BOOL_TYPE ValidateSectionID(SF_ID_TYPE sf_id);
#endif

static BOOL_TYPE RequestSFDisplacementData(uint8 data_alignmrnt);
static SFUPDATER_REQUESTER_STATUS_TYPE ProcessReadRequest(SFUPDATER_DISPLACEMENT_INFO_TYPE *sf_displacement_info, uint8 request_type);


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module SFUpdater and its variables
 */
void SFUpdater__Initialize(void)
{
    #if (SFUPDATER_MANAGE_DISPLACEMENT_DATA_ONFLASH == ENABLED)
        uint8 displacement_index;
        SFUPDATER_DISPLACEMENT_INFO_TYPE *displacement_ptr;
        BOOL_TYPE crc_mismatch;
        TIMER_PARAMETER_TYPE timer_parameter;
        uint16 displacement_timeout_counter;
        uint8 communication_1ms_timer;
        uint8 communication_5ms_counter;

        crc_mismatch        = FALSE;
    #endif

    SFUpdater_Displacement_Reveal_Max_Size  = REVEAL_PAYLOAD_SIZE;
    SFUpdater_Requester_Timeout             = 0;
    SFUpdater_Displacement_Address          = 0;
    SFUpdater_Displacement_Data_Index       = 0;
    SFUpdater_Requester_Status              = SFUPDATER_REQUESTER_IDLE;

    memset(&SFUpdater_Requested_Displacement_Info, 0, sizeof(SFUpdater_Requested_Displacement_Info));

    #if (SFUPDATER_MANAGE_SF_ID_DATA == ENABLED)
        SFUpdater_State             = SFUPDATER_STATE_INITIALIZE;
        SFUpdater_Section_Index     = 0;
        SFUpdater_Section_Address   = (FLASH_ADDRESS_TYPE)SettingFile__GetMemorySectionStart(); //lint !e923 cast from pointer to unsigned long [Encompasses MISRA 2004 Rule 11.1], [MISRA 2004 Rule 11.3]
        SFUpdater_Reveal_Max_Size   = REVEAL_PAYLOAD_SIZE;
        Millis__Start(&SFUpdater_Timer);

        #ifdef SFUPDATER_ENABLE_UNALIGNED_MEMORY_HANDLING
            SFUpdater_Unaligned_Data_State            = SFUPDATER_UNALIGNED_DATA_NONE;
            SFUpdater_Flash_Buffer_Write_Index         = 0;
            memset(SFUpdater_Flash_Buffer, SFUPDATER_FLASH_DEFAULT_VALUE, sizeof(SFUpdater_Flash_Buffer));
        #endif
    #endif

    #if (SFUPDATER_MANAGE_DISPLACEMENT_DATA_ONFLASH == ENABLED)
        SFUpdater_Displacement_State = SFUPDATER_DISPLACEMENT_STATE_INITIALIZE;

        displacement_ptr               = (SFUPDATER_DISPLACEMENT_INFO_TYPE *)&__SFDisplacement_Start;
        SFUpdater_Displacement_Address  = (FLASH_ADDRESS_TYPE)&__SFDisplacement_Start;

        //check for the existing Displacement Data integrity
        for (displacement_index = 0; displacement_index < SFUPDATER_NUM_DISPLACEMENT; displacement_index++)
        {
            //validate the header
            if ((displacement_ptr->SF_Pointer_ID == SFUPDATER_DISPLACEMENT_LIST[displacement_index].SF_Pointer_ID) &&
                (displacement_ptr->SF_Displacement == SFUPDATER_DISPLACEMENT_LIST[displacement_index].SF_Displacement))
            {
                 if (displacement_ptr->Crc != Crc16__ConstRangeProcess((uint8 *)(&displacement_ptr->Data_Ptr), 0xFFFF , displacement_ptr->Size))
                 {
                     crc_mismatch = TRUE;
                     break;
                 }
            }
            else
            {
                crc_mismatch = TRUE;
                break;
            }

            //point to the next displacement header
            displacement_ptr = (SFUPDATER_DISPLACEMENT_INFO_TYPE *)((uint8 *)&(displacement_ptr->Data_Ptr) + displacement_ptr->Size);
        }

        //displacement data broken then engage in update
        if (crc_mismatch == TRUE)
        {
            //initialize the communication modules
            Gpio__Initialize();
            Timer__Initialize();        //TimerInterrupt can not be used as it triggers the application scheduler or thread
            Win__Initialize();
            Reveal__Initialize();
            Flash__Initialize();

            ENABLE_INTERRUPTS();

            //format the displacement region even though if one of the displacement is corrupted
            (void) Flash__Format((FLASH_ADDRESS_TYPE)&__SFDisplacement_Start, (FLASH_ADDRESS_TYPE)((uint32)&__SFDisplacement_Start + (uint32)&__SFDisplacement_Size - 1U));

            //All the micros at least support for TIMER1. We are using this only for the purpose of timing
            // Cannot use SystemTimer as it based on TimerInterrupt which will excite the OS/Scheduler
            timer_parameter.Timer     = TIMER1;
            timer_parameter.Channel   = TIMER_CHANNEL1;
            timer_parameter.Mode      = TIMER_MODE_INPUT_CAPTURE;
            timer_parameter.Port      = PORT_NULL;
            timer_parameter.Pin       = 0;
            timer_parameter.Pin_ReMap = 0;
            timer_parameter.Polarity  = TIMER_POLARITY_ACTIVE_HIGH;

            Timer__Config(&timer_parameter);

            communication_1ms_timer         = Timer__StartCount(&timer_parameter);
            communication_5ms_counter       = 0;
            displacement_timeout_counter    = 0;
            displacement_index              = 0;

            while (1)
            {
                if (Timer__GetCount(communication_1ms_timer) > SFUPDATER_ONE_MILLISECOND_IN_MICROSECOND_UNIT)
                {
                    Timer__ReStartCount(communication_1ms_timer);
                    Win__Handle1ms();
                    Reveal__Handler1ms();
                    communication_5ms_counter++;
                }

                if (communication_5ms_counter >= SFUPDATER_FIVE_MILLISECOND_IN_MS_UNIT)
                {
                    communication_5ms_counter = 0;
                    Reveal__Handler();
                    displacement_timeout_counter++;
                    SERVICE_WATCHDOG();
                }

                //manage updating the displacement data
                switch (SFUpdater_Displacement_State)
                {
                    case SFUPDATER_DISPLACEMENT_STATE_INITIALIZE:
                        SFUpdater_Displacement_State = SFUPDATER_DISPLACEMENT_STATE_WAIT_ACU_ALIVE;
                        break;

                    case SFUPDATER_DISPLACEMENT_STATE_WAIT_ACU_ALIVE:
                        if ((Reveal__GetNodestatus() & ((uint16) 1U << (uint8) WIN_ACU_ADDRESS)) != 0U)
                        {
                            // ACU is alive
                            SFUpdater_Displacement_State = SFUPDATER_DISPLACEMENT_STATE_WAIT_MSG_SIZE;
                            (void) Reveal__QueueMessageNoData(1,17,WIN_ACU_ADDRESS,TRUE,FALSE);
                            displacement_timeout_counter = 0;
                        }
                        break;

                    case SFUPDATER_DISPLACEMENT_STATE_WAIT_MSG_SIZE:
                        // Wait for reply from ACU with message size; See SFUpdater__RxMaxPackage()
                        if (displacement_timeout_counter >= SFUPDATER_ONE_SECOND_IN_FIVE_MS_UNIT)
                        {
                            //Restart the process if  ACU did not replied
                            SYSTEM_RESET();
                        }
                        break;

                    case SFUPDATER_DISPLACEMENT_STATE_REQUEST_SFDISPLACEMENT_INFO:
                        if (displacement_index < SFUPDATER_NUM_DISPLACEMENT)
                        {
                            (void) API007Data__RequestSettingFileDisplacementInfo(WIN_ACU_ADDRESS, SFUPDATER_DISPLACEMENT_LIST[displacement_index].SF_Pointer_ID,
                                SFUPDATER_DISPLACEMENT_LIST[displacement_index].SF_Displacement);

                            displacement_index++;
                            SFUpdater_Displacement_State = SFUPDATER_DISPLACEMENT_STATE_WAIT_SFDISPLACEMENT_INFO;
                            displacement_timeout_counter = 0;
                        }
                        else
                        {
                            //Successful completed the update of Displacement region
                            #ifdef SFUPDATER_ENABLE_UNALIGNED_MEMORY_HANDLING
                                if(SFUpdater_Flash_Buffer_Write_Index > 0)
                                {
                                    uint16 bytes_to_fill = FLASH_ALIGNMENT - SFUpdater_Flash_Buffer_Write_Index;
                                    memset(&SFUpdater_Flash_Buffer[SFUpdater_Flash_Buffer_Write_Index], SFUPDATER_FLASH_DEFAULT_VALUE, bytes_to_fill);
                                    (void) Flash__Write(SFUpdater_Displacement_Address, SFUpdater_Flash_Buffer, FLASH_ALIGNMENT);
                                }
                            #endif
                            SYSTEM_RESET();
                        }
                        break;

                    case SFUPDATER_DISPLACEMENT_STATE_WAIT_SFDISPLACEMENT_INFO:
                        // Wait for reply from ACU with message size; See SFUpdater__RxSFDisplacementInfo()
                        if (displacement_timeout_counter >= SFUPDATER_ONE_SECOND_IN_FIVE_MS_UNIT)
                        {
                            //Restart the process if  ACU did not replied
                            SYSTEM_RESET();
                        }
                        break;

                    case SFUPDATER_DISPLACEMENT_STATE_PROGRAMMING_DATA:
                        // shall not stuck in the programming
                        if (displacement_timeout_counter >= SFUPDATER_TEN_SECOND_IN_FIVE_MS_UNIT)
                        {
                            //Restart the process if  ACU did not replied
                            SYSTEM_RESET();
                        }
                        break;

                    case SFUPDATER_DISPLACEMENT_STATE_PROGRAMMING_END:
                        SFUpdater_Displacement_State = SFUPDATER_DISPLACEMENT_STATE_REQUEST_SFDISPLACEMENT_INFO;
                        break;

                    default:
                        break;
                }
            }
        }
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This is the handler called while in programming mode to manage the settingfile download process.
 * @return TRUE if a cold reset is required to continue; otherwise FALSE.
 */
BOOL_TYPE SFUpdater__Handler(void)
{
    BOOL_TYPE retval;

    retval = FALSE;

    #if (SFUPDATER_MANAGE_SF_ID_DATA == ENABLED)
        // Check if Programming Mode is engaged and SystemTimer is running; otherwise it's safer to reset and start over.
        if ((Mode__IsProgrammingEngaged() != FALSE) &&
            (Millis__ReachedTarget(&SFUpdater_Timer, 1000) == FALSE))
        {
            switch (SFUpdater_State)
            {
                case SFUPDATER_STATE_INITIALIZE:
                    SFUpdater_State = SFUPDATER_STATE_ERASE_FLASH;
                    Flash__Initialize();
                    SFUpdater_Erase_Address = (FLASH_ADDRESS_TYPE)SettingFile__GetMemorySectionStart();     //lint !e923 cast from pointer to unsigned long [Encompasses MISRA 2004 Rule 11.1], [MISRA 2004 Rule 11.3]
                    Millis__Start(&SFUpdater_Timer);
                    break;

                case SFUPDATER_STATE_ERASE_FLASH:
                    (void) Flash__Format(SFUpdater_Erase_Address, SFUpdater_Erase_Address + SettingFile__GetMemorySectionSize() - 1U);
                    SFUpdater_State = SFUPDATER_STATE_WAIT_ACU_ALIVE;
                    Millis__Start(&SFUpdater_Timer);
                    break;

                case SFUPDATER_STATE_WAIT_ACU_ALIVE:
                    if ((Reveal__GetNodestatus() & ((uint16) 1U << (uint8) WIN_ACU_ADDRESS)) != 0U)
                    {
                        // ACU is alive
                        SFUpdater_State = SFUPDATER_STATE_WAIT_MSG_SIZE;
                        (void) Reveal__QueueMessageNoData(1,17,WIN_ACU_ADDRESS,TRUE,FALSE);
                        Millis__Start(&SFUpdater_Timer);
                    }
                    break;
                case SFUPDATER_STATE_WAIT_MSG_SIZE:
                    // Wait for reply from ACU with message size; See SFUpdater__RxMaxPackage()
                    break;
                case SFUPDATER_STATE_WAIT_SF_INFO:
                    // Wait for reply from ACu with SettingFile info; See SFUpdater__RxSFInfo()
                    break;
                case SFUPDATER_STATE_PROGRAMMING_DATA:
                    break;
                case SFUPDATER_STATE_DATA_END:
                default:
                    // After SFUpdater process has been completed or SFUpdater_State doesn't contain a valid state, force reset
                    retval = TRUE;
                    break;
            }
        }
        else
        {
            retval = TRUE;
        }
    #endif

    return (retval);
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param node
 * @param max_payload
 */
void SFUpdater__RxMaxPackage(uint8 node, uint8 max_payload)
{
    #if (SFUPDATER_MANAGE_SF_ID_DATA == ENABLED)
        if ((Mode__IsProgrammingEngaged() != FALSE) &&
            (SFUpdater_State == SFUPDATER_STATE_WAIT_MSG_SIZE))
        {
            if (max_payload < SFUpdater_Reveal_Max_Size)
            {
                SFUpdater_Reveal_Max_Size = max_payload;
            }

            (void) API007Data__RequestSettingFileIDInfo(WIN_ACU_ADDRESS, SFUPDATER_SECTIONS[SFUpdater_Section_Index]);

            SFUpdater_State = SFUPDATER_STATE_WAIT_SF_INFO;
            SFUpdater_Data_Index = 0;

            Millis__Start(&SFUpdater_Timer);
        }
    #endif

    #if (SFUPDATER_MANAGE_DISPLACEMENT_DATA_ONFLASH == ENABLED)
        if (SFUpdater_Displacement_State == SFUPDATER_DISPLACEMENT_STATE_WAIT_MSG_SIZE)
        {
            if (max_payload < SFUpdater_Displacement_Reveal_Max_Size)
            {
                SFUpdater_Displacement_Reveal_Max_Size = max_payload;
            }

            SFUpdater_Displacement_State  = SFUPDATER_DISPLACEMENT_STATE_REQUEST_SFDISPLACEMENT_INFO;
        }
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @details -
 * @param node
 * @param section_crc
 * @param section_size
 */
void SFUpdater__RxSFInfo(uint8 node, SF_ID_TYPE sf_id, uint16 section_crc, uint16 section_size)
{
    #if (SFUPDATER_MANAGE_SF_ID_DATA == ENABLED)
        uint16 sf_crc;

        if (ValidateSectionID(sf_id) != FALSE)
        {
            if ((Mode__IsProgrammingEngaged() != FALSE) &&
                (SFUpdater_State == SFUPDATER_STATE_WAIT_SF_INFO))
            {
                SFUpdater_Data_Size = section_size;
                SFUpdater_Data_Index = 0;
                SFUpdater_State = SFUPDATER_STATE_PROGRAMMING_DATA;
                (void) RequestSFData();
                Millis__Start(&SFUpdater_Timer);
            }
            else if (Mode__GetMode() == MODE_NORMAL)
            {
                // Check the CRC against its one CRC
                sf_crc = SettingFile__GetSettingFileCRC(sf_id);

                if (section_crc != sf_crc)
                {
                    // CRC Mismatch - force Node in programming mode.
                    Mode__SetMode(MODE_PROGRAMMING);
                }
            }
        }
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @details -
 * @param node
 * @param sf_pointer_id
 * @param sf_displacement
 * @param sf_displacement_crc
 * @param sf_displacement_size
 */
void SFUpdater__RxSFDisplacementInfo(SETTINGFILE_OFFSET_TYPE sf_pointer_id, uint16 sf_displacement, uint16 sf_displacement_crc, uint8 sf_displacement_size)
{
    #if (SFUPDATER_MANAGE_DISPLACEMENT_DATA_ONFLASH == ENABLED)
        SFUPDATER_DISPLACEMENT_INFO_TYPE *displacement_ptr;
        uint8 displacement_index;
        uint8 index;
    #endif

    SFUpdater_Received_Displacement_Info.SF_Pointer_ID     = sf_pointer_id;
    SFUpdater_Received_Displacement_Info.SF_Displacement   = sf_displacement;
    SFUpdater_Received_Displacement_Info.Crc               = sf_displacement_crc;
    SFUpdater_Received_Displacement_Info.Size              = sf_displacement_size;
    SFUpdater_Displacement_Data_Index                      = 0;

    //check is its been requested by the application
    if ((SFUpdater_Requested_Displacement_Info.SF_Pointer_ID  == sf_pointer_id) &&
        (SFUpdater_Requested_Displacement_Info.SF_Displacement == sf_displacement) &&
        (SFUpdater_Requester_Status == SFUPDATER_REQUESTER_PROCESSING))
    {
        SFUpdater_Requested_Displacement_Info.Crc   = sf_displacement_crc;
        SFUpdater_Requested_Displacement_Info.Size  = sf_displacement_size;

        Millis__Start(&SFUpdater_Requester_Timer);
        SFUpdater_Requester_Timeout = SFUPDATER_DISPLACEMENT_DATA_READ_TIMEOUT;
        SFUpdater_Requester_Status = SFUPDATER_REQUESTER_DATA_READY;
    }

    #if (SFUPDATER_MANAGE_DISPLACEMENT_DATA_ONFLASH == ENABLED)
        if (SFUpdater_Displacement_State == SFUPDATER_DISPLACEMENT_STATE_WAIT_SFDISPLACEMENT_INFO)
        {
            //Create the displacement header and write in the flash
            if ((sf_pointer_id != SF_PTR_INVALID_VALUE) &&
                (sf_displacement != SF_DISPL_INVALID_VALUE))
            {
                SFUpdater_Displacement_State                  = SFUPDATER_DISPLACEMENT_STATE_PROGRAMMING_DATA;

                // -4 is to exclude writing on Data_ptr structure.
                SFUpdater__RxSFDisplacementData(sf_pointer_id, sf_displacement, 0, sizeof(SFUPDATER_DISPLACEMENT_INFO_TYPE) - 4U, (uint8 *)(&SFUpdater_Received_Displacement_Info));
            }
            else
            {
                //requested displacement does not present
                SYSTEM_RESET();
            }
        }
    else
    {
        //validate the received displacement info with the stored
        for (displacement_index = 0; displacement_index < SFUPDATER_NUM_DISPLACEMENT; displacement_index++)
        {
            //this needs to be compared with the SFUPDATER_DISPLACEMENT_LIST to make sure its existed in the list
            if ((sf_pointer_id == SFUPDATER_DISPLACEMENT_LIST[displacement_index].SF_Pointer_ID) &&
                (sf_displacement == SFUPDATER_DISPLACEMENT_LIST[displacement_index].SF_Displacement))
            {
                //search for the absolute address in the flash
                displacement_ptr = (void *)&__SFDisplacement_Start;

                for (index = 0; index < displacement_index; index++)
                {
                    //point to the next displacement header
                    displacement_ptr = (SFUPDATER_DISPLACEMENT_INFO_TYPE *)((uint8 *)&(displacement_ptr->Data_Ptr) + displacement_ptr->Size);
                }

                if (sf_displacement_crc != displacement_ptr->Crc)
                {
                    //format the displacement region even though if one of the displacement is updated
                    (void) Flash__Format((FLASH_ADDRESS_TYPE)&__SFDisplacement_Start, (FLASH_ADDRESS_TYPE)((uint32)&__SFDisplacement_Start + (uint32)&__SFDisplacement_Size - 1U));

                    SYSTEM_RESET();
                }
                break;
            }
        }
    }
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @details
 * @param node
 * @param sf_pointer_id
 * @param sf_displacement
 * @param index
 * @param data_ptr
 */
void SFUpdater__RxSFDisplacementData(SETTINGFILE_OFFSET_TYPE sf_pointer_id, uint16 sf_displacement, uint16 data_index, uint8 sf_displacement_size, uint8 * data_ptr)
{
    #if (SFUPDATER_MANAGE_DISPLACEMENT_DATA_ONFLASH == ENABLED)
        if (SFUpdater_Displacement_State == SFUPDATER_DISPLACEMENT_STATE_PROGRAMMING_DATA)
        {
            #ifdef SFUPDATER_ENABLE_UNALIGNED_MEMORY_HANDLING
                uint16 next_size = SFUpdater_Flash_Buffer_Write_Index + sf_displacement_size;

                if(next_size < FLASH_ALIGNMENT)
                {
                    // Append data into the Flash write buffer
                    memcpy(&SFUpdater_Flash_Buffer[SFUpdater_Flash_Buffer_Write_Index], data_ptr, sf_displacement_size);
                    SFUpdater_Flash_Buffer_Write_Index += sf_displacement_size;
                }
                else if((next_size % FLASH_ALIGNMENT) != 0)
                {
                    // Write the data block and copy the remaining frame at the begin on the Flash write buffer
                    uint16 data_to_write = FLASH_ALIGNMENT - SFUpdater_Flash_Buffer_Write_Index;
                    memcpy(&SFUpdater_Flash_Buffer[SFUpdater_Flash_Buffer_Write_Index], data_ptr, data_to_write);
                    (void) Flash__Write(SFUpdater_Displacement_Address, SFUpdater_Flash_Buffer, FLASH_ALIGNMENT);
                    SFUpdater_Displacement_Address += FLASH_ALIGNMENT;
                    next_size -= data_to_write;
                    // Check if there is still data to write
                    if(next_size > 0)
                    {
                        // If it doesn't fill the buffer, copy remaining data (FLASH_ALIGNMENT bigger than Reveal size)
                        if(next_size < FLASH_ALIGNMENT)
                        {
                            memcpy(SFUpdater_Flash_Buffer, data_ptr + data_to_write, next_size);
                            SFUpdater_Flash_Buffer_Write_Index = next_size;
                        }
                        // If bigger than FLASH_ALIGNMENT, data to write should be a multiple of it (FLASH_ALIGNMENT smaller than Reveal size)
                        else
                        {
                            (void) Flash__Write(SFUpdater_Displacement_Address, data_ptr + data_to_write, next_size);
                            SFUpdater_Displacement_Address += next_size;
                        }
                    }
                }
                else
            #endif
            {
                // Write displacement data size
                (void) Flash__Write(SFUpdater_Displacement_Address, data_ptr, (FLASH_SIZE_TYPE)sf_displacement_size);
                SFUpdater_Displacement_Address += sf_displacement_size;
            }

            if (RequestSFDisplacementData(SFUPDATER_REVEAL_DATA_ALIGNMENT) != FALSE)
            {
                SFUpdater_Displacement_State = SFUPDATER_DISPLACEMENT_STATE_PROGRAMMING_END;
            }
        }
    #endif

    if ((SFUpdater_Requested_Displacement_Info.SF_Pointer_ID  == sf_pointer_id) &&
        (SFUpdater_Requested_Displacement_Info.SF_Displacement == sf_displacement) &&
        (SFUpdater_Requester_Status == SFUPDATER_REQUESTER_PROCESSING))
    {
        memcpy((void *)SFUpdater_Displacement_Address, data_ptr, sf_displacement_size); //lint !e923 cast from unsigned long to pointer [Encompasses MISRA 2004 Rule 11.1], [MISRA 2004 Rule 11.3]

        SFUpdater_Displacement_Address += sf_displacement_size;

        if (RequestSFDisplacementData(SFUPDATER_RAM_DATA_ALIGNMENT) != FALSE)
        {
            Millis__Start(&SFUpdater_Requester_Timer);
            SFUpdater_Requester_Timeout = SFUPDATER_DISPLACEMENT_DATA_READ_TIMEOUT;
            SFUpdater_Requester_Status = SFUPDATER_REQUESTER_DATA_READY;
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @details Request setting file displacement information for each managed displacement
 */
void SFUpdater__ValidateFlashDisplacementData(void)
{
    #if (SFUPDATER_MANAGE_DISPLACEMENT_DATA_ONFLASH == ENABLED)
        uint8 displacement_index;
        for (displacement_index = 0; displacement_index < SFUPDATER_NUM_DISPLACEMENT; displacement_index++)
        {
            (void) API007Data__RequestSettingFileDisplacementInfo(WIN_ACU_ADDRESS, SFUPDATER_DISPLACEMENT_LIST[displacement_index].SF_Pointer_ID,
                SFUPDATER_DISPLACEMENT_LIST[displacement_index].SF_Displacement);
        }
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @details Read the displacement information stored in the flash
 *
 * @param sf_displacement_info
 * @return
 */
BOOL_TYPE SFUpdater__ReadSFDisplacementInfoFLASH(SFUPDATER_DISPLACEMENT_INFO_TYPE *sf_displacement_info)
{
    #if (SFUPDATER_MANAGE_DISPLACEMENT_DATA_ONFLASH == ENABLED)
        BOOL_TYPE response = FALSE;
        uint8 displacement_index;

        //validate the received displacement info with the stored
        for (displacement_index = 0; displacement_index < SFUPDATER_NUM_DISPLACEMENT; displacement_index++)
        {
            //this needs to be compared with the SFUPDATER_DISPLACEMENT_LIST to make sure its existed in the list
            if ((sf_displacement_info->SF_Pointer_ID == SFUPDATER_DISPLACEMENT_LIST[displacement_index].SF_Pointer_ID) &&
                (sf_displacement_info->SF_Displacement == SFUPDATER_DISPLACEMENT_LIST[displacement_index].SF_Displacement))
            {
                //search for the absolute address in the flash
                SFUPDATER_DISPLACEMENT_INFO_TYPE *displacement_ptr = (void *)&__SFDisplacement_Start;
                uint8 index;

                for (index = 0; index < displacement_index; index++)
                {
                    //point to the next displacement header
                    displacement_ptr = (void *) (displacement_ptr->Data_Ptr + displacement_ptr->Size);
                }

                sf_displacement_info->Size       = displacement_ptr->Size;
                sf_displacement_info->Crc        = displacement_ptr->Crc;
                sf_displacement_info->Data_Ptr   = displacement_ptr->Data_Ptr;

                response = TRUE;
                break;
            }
        }

        return(response);
    #else
        return(FALSE);
    #endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @details Read the Displacement Info which are managed in RAM
 *
 * @param sf_displacement_info
 * @return
 */
SFUPDATER_REQUESTER_STATUS_TYPE SFUpdater__ReadSFDisplacementInfoRAM(SFUPDATER_DISPLACEMENT_INFO_TYPE *sf_displacement_info)
{
    return(ProcessReadRequest(sf_displacement_info, 0));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @details Read the Displacement Data which are managed in RAM
 *
 * @param sf_displacement_info
 * @return
 */
SFUPDATER_REQUESTER_STATUS_TYPE SFUpdater__ReadSFDisplacementDataRAM(SFUPDATER_DISPLACEMENT_INFO_TYPE *sf_displacement_info)
{
    return(ProcessReadRequest(sf_displacement_info, 1));
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @details
 * @brief
 *
 * @param node
 * @param size
 * @param index
 * @param data_ptr
 */
void SFUpdater__RxSFData(uint8 node, uint8 size, uint16 data_index, uint8 * data_ptr)
{
    #if (SFUPDATER_MANAGE_SF_ID_DATA == ENABLED)
        if ((Mode__IsProgrammingEngaged() != FALSE) &&
            (node == (uint8) WIN_ACU_ADDRESS) &&                          // checks if message is received from ACU
            ((data_index + size) == SFUpdater_Data_Index) &&              // checks if next address to get is the expected one
            (SFUpdater_State == SFUPDATER_STATE_PROGRAMMING_DATA))
        {
            #ifdef SFUPDATER_ENABLE_UNALIGNED_MEMORY_HANDLING
                //Skip writing the unaligned ending data to the Flash
                //Instead of that store in a temporary buffer and ask the next segment data and concatenate the data
                switch(SFUpdater_Unaligned_Data_State)
                {
                    default:
                    case SFUPDATER_UNALIGNED_DATA_NONE:
                        (void) Flash__Write(SFUpdater_Section_Address + (FLASH_ADDRESS_TYPE)data_index, data_ptr, (FLASH_SIZE_TYPE)size);
                        break;

                    case SFUPDATER_UNALIGNED_DATA_BUFFERING:
                        if(SFUpdater_Flash_Buffer_Write_Index + size >= FLASH_ALIGNMENT)
                        {
                            uint16 data_to_write = FLASH_ALIGNMENT - SFUpdater_Flash_Buffer_Write_Index;
                            memcpy(&SFUpdater_Flash_Buffer[SFUpdater_Flash_Buffer_Write_Index], data_ptr, data_to_write);
                            (void) Flash__Write(SFUpdater_Section_Address + (FLASH_ADDRESS_TYPE)data_index - SFUpdater_Flash_Buffer_Write_Index,
                                SFUpdater_Flash_Buffer, (FLASH_SIZE_TYPE)FLASH_ALIGNMENT);
                            SFUpdater_Flash_Buffer_Write_Index = size - data_to_write;

                            // If SFUpdater_Flash_Buffer_Write_Index is higher than FLASH_ALIGNMENT, write all possible complete data blocks in Flash
                            if(SFUpdater_Flash_Buffer_Write_Index >= FLASH_ALIGNMENT)
                            {
                                uint16 flash_write_bytes = SFUpdater_Flash_Buffer_Write_Index / FLASH_ALIGNMENT;
                                flash_write_bytes *= FLASH_ALIGNMENT;
                                (void) Flash__Write(SFUpdater_Section_Address + (FLASH_ADDRESS_TYPE)data_index + data_to_write, data_ptr + data_to_write, (FLASH_SIZE_TYPE) flash_write_bytes);
                                SFUpdater_Flash_Buffer_Write_Index -= flash_write_bytes;
                                data_to_write += flash_write_bytes;
                            }

                            // SFUpdater_Flash_Buffer_Write_Index is less than FLASH_ALIGNMENT here; if not 0, data is saved to the Flash buffer for the next data request
                            if(SFUpdater_Flash_Buffer_Write_Index > 0)
                            {
                                memcpy(SFUpdater_Flash_Buffer, data_ptr + data_to_write, SFUpdater_Flash_Buffer_Write_Index);
                            }
                            else
                            {
                                SFUpdater_Unaligned_Data_State = SFUPDATER_UNALIGNED_DATA_NONE;
                            }
                        }
                        else
                        {
                            memcpy(&SFUpdater_Flash_Buffer[SFUpdater_Flash_Buffer_Write_Index], data_ptr, size);
                            SFUpdater_Flash_Buffer_Write_Index += size;
                        }
                        break;

                    case SFUPDATER_UNALIGNED_DATA_BUFFER_WRITE:
                        memcpy(&SFUpdater_Flash_Buffer[SFUpdater_Flash_Buffer_Write_Index], data_ptr, size);

                        // Adjust the Flash Write Address
                        (void) Flash__Write(SFUpdater_Section_Address + (FLASH_ADDRESS_TYPE)data_index - SFUpdater_Flash_Buffer_Write_Index,
                            SFUpdater_Flash_Buffer, (FLASH_SIZE_TYPE)FLASH_ALIGNMENT);
                        SFUpdater_Flash_Buffer_Write_Index = 0;
                        SFUpdater_Unaligned_Data_State = SFUPDATER_UNALIGNED_DATA_NONE;
                        break;
                }
		    #else
                (void) Flash__Write(SFUpdater_Section_Address + (FLASH_ADDRESS_TYPE)data_index, data_ptr, (FLASH_SIZE_TYPE)size);
            #endif


            if (RequestSFData() != FALSE)
            {
                SFUpdater_Section_Index++;
                if (SFUPDATER_SECTIONS[SFUpdater_Section_Index] != SF_ID_END)
                {
                    SFUpdater_Section_Address += SFUpdater_Data_Size;
                    (void) API007Data__RequestSettingFileIDInfo(WIN_ACU_ADDRESS, SFUPDATER_SECTIONS[SFUpdater_Section_Index]);

                    SFUpdater_State = SFUPDATER_STATE_WAIT_SF_INFO;
                    SFUpdater_Data_Index = 0;
                }
                else
                {
                    #ifdef SFUPDATER_ENABLE_UNALIGNED_MEMORY_HANDLING
                        if(SFUpdater_Flash_Buffer_Write_Index > 0U)
                        {
                            uint16 bytes_to_fill = FLASH_ALIGNMENT - SFUpdater_Flash_Buffer_Write_Index;
                            memset(&SFUpdater_Flash_Buffer[SFUpdater_Flash_Buffer_Write_Index], SFUPDATER_FLASH_DEFAULT_VALUE, bytes_to_fill);
                            (void) Flash__Write(SFUpdater_Section_Address + SFUpdater_Data_Size - SFUpdater_Flash_Buffer_Write_Index,
                                SFUpdater_Flash_Buffer, FLASH_ALIGNMENT);
                        }
                    #endif
                    SFUpdater_State = SFUPDATER_STATE_DATA_END;
                }
            }
            Millis__Start(&SFUpdater_Timer);
        }
    #endif
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
#if (SFUPDATER_MANAGE_SF_ID_DATA == ENABLED)
//---------------------------------------------------------------------------------------------------------------------
/**
 * @details
 *
 * @return
 */
static BOOL_TYPE RequestSFData(void)
{
    BOOL_TYPE completed = TRUE;

    if (SFUpdater_Data_Index < SFUpdater_Data_Size)
    {
        uint16 remaining = SFUpdater_Data_Size - SFUpdater_Data_Index;
        // -4 is because of the header of the API007_OP_REQ_PUB_SETTING_FILE_ID_DATA
        uint16 max_payload_size = (uint16) SFUpdater_Reveal_Max_Size - 4U;

        if(remaining > max_payload_size)
        {
            remaining = max_payload_size;
        }

        if (remaining > (uint8) SFUPDATER_REVEAL_DATA_ALIGNMENT)
        {
            remaining /= (uint8) SFUPDATER_REVEAL_DATA_ALIGNMENT;
            remaining *= (uint8) SFUPDATER_REVEAL_DATA_ALIGNMENT;
        }

        #ifdef SFUPDATER_ENABLE_UNALIGNED_MEMORY_HANDLING
            if(SFUpdater_Unaligned_Data_State != SFUPDATER_UNALIGNED_DATA_BUFFERING)
            {
                // Add data to the Buffer if data to get is less than FLASH_ALIGNMENT
                if(remaining < (uint16) FLASH_ALIGNMENT)
                {
                    SFUpdater_Flash_Buffer_Write_Index = 0;
                    SFUpdater_Unaligned_Data_State = SFUPDATER_UNALIGNED_DATA_BUFFERING;
                }
            }
            else
            {
                uint16 buffer_to_fill = FLASH_ALIGNMENT - SFUpdater_Flash_Buffer_Write_Index;
                // If already buffering and needs less than SFUPDATER_REVEAL_DATA_ALIGNMENT bytes, try to get them to align with Flash block
                if(buffer_to_fill < SFUPDATER_REVEAL_DATA_ALIGNMENT)
                {
                    remaining -= SFUPDATER_REVEAL_DATA_ALIGNMENT;
                    remaining += buffer_to_fill;
                }
            }

            // Clear the buffer when the last section transfer has been completed and flash buffer is not full
            if((remaining + SFUpdater_Data_Index) >= SFUpdater_Data_Size)
            {
                uint8 next_section_index    = SFUpdater_Section_Index + 1;

                if ((SFUPDATER_SECTIONS[next_section_index] == SF_ID_END) &&
                    ((SFUpdater_Flash_Buffer_Write_Index + remaining) < FLASH_ALIGNMENT))
                {
                    memset(&SFUpdater_Flash_Buffer[SFUpdater_Flash_Buffer_Write_Index + remaining], SFUPDATER_FLASH_DEFAULT_VALUE,
                        FLASH_ALIGNMENT - (SFUpdater_Flash_Buffer_Write_Index + remaining));
                    SFUpdater_Unaligned_Data_State = SFUPDATER_UNALIGNED_DATA_BUFFER_WRITE;
                }
            }
        #endif

        if (API007Data__RequestSettingFileIDData(WIN_ACU_ADDRESS, SFUPDATER_SECTIONS[SFUpdater_Section_Index], SFUpdater_Data_Index, (uint8) remaining) != FALSE)
        {
            SFUpdater_Data_Index += remaining;
        }

        completed = FALSE;
    }

    return(completed);
}
#endif

//---------------------------------------------------------------------------------------------------------------------
/**
 * @details
 *
 * @param data_alignmrnt
 * @return
 */
static BOOL_TYPE RequestSFDisplacementData(uint8 data_alignmrnt)
{
    BOOL_TYPE completed = TRUE;

    if (SFUpdater_Displacement_Data_Index < SFUpdater_Received_Displacement_Info.Size)
    {
        // -6 is because of the header of the API007Data__RequestSettingFileDisplacementData
        uint8 this_size = SFUpdater_Displacement_Reveal_Max_Size - 6U;
        uint16 remaining = SFUpdater_Received_Displacement_Info.Size - SFUpdater_Displacement_Data_Index;

        if (remaining < this_size)
        {
            this_size = (uint8) remaining;
        }

        //FLASH alignment adjust..
        if (this_size >= data_alignmrnt)
        {
            this_size /= data_alignmrnt;
            this_size *=  data_alignmrnt;
        }

        if (API007Data__RequestSettingFileDisplacementData(WIN_ACU_ADDRESS, SFUpdater_Received_Displacement_Info.SF_Pointer_ID,
            SFUpdater_Received_Displacement_Info.SF_Displacement, SFUpdater_Displacement_Data_Index, this_size) != FALSE)
        {
            SFUpdater_Displacement_Data_Index += this_size;
        }

        completed = FALSE;
    }

    return(completed);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @details
 *
 * @param sf_displacement_info
 * @param request_type
 * @return
 */
static SFUPDATER_REQUESTER_STATUS_TYPE ProcessReadRequest(SFUPDATER_DISPLACEMENT_INFO_TYPE *sf_displacement_info, uint8 request_type)
{
    SFUPDATER_REQUESTER_STATUS_TYPE response;

    switch(SFUpdater_Requester_Status)
    {
        case SFUPDATER_REQUESTER_IDLE:
            if ((sf_displacement_info->SF_Pointer_ID != SF_PTR_INVALID_VALUE) &&
                (sf_displacement_info->SF_Displacement != (uint16) SF_DISPL_INVALID_VALUE))
            {
                SFUpdater_Requester_Status  = SFUPDATER_REQUESTER_PROCESSING;
                response                    =  SFUPDATER_REQUESTER_PROCESSING;

                SFUpdater_Requested_Displacement_Info.SF_Pointer_ID   = sf_displacement_info->SF_Pointer_ID;
                SFUpdater_Requested_Displacement_Info.SF_Displacement = sf_displacement_info->SF_Displacement;

                //Set the timer
                Millis__Start(&SFUpdater_Requester_Timer);
                SFUpdater_Requester_Timeout = SFUPDATER_DISPLACEMENT_RESPONSE_TIMEOUT;

                if (request_type == 0U)
                {
                    (uint8) API007Data__RequestSettingFileDisplacementInfo(WIN_ACU_ADDRESS, SFUpdater_Requested_Displacement_Info.SF_Pointer_ID, SFUpdater_Requested_Displacement_Info.SF_Displacement);
                }
                else if (sf_displacement_info->Data_Ptr != NULL)
                {
                    SFUpdater_Received_Displacement_Info.SF_Pointer_ID    = sf_displacement_info->SF_Pointer_ID;
                    SFUpdater_Received_Displacement_Info.SF_Displacement  = sf_displacement_info->SF_Displacement;
                    SFUpdater_Received_Displacement_Info.Size             = sf_displacement_info->Size;
                    SFUpdater_Displacement_Address                        = (FLASH_ADDRESS_TYPE)sf_displacement_info->Data_Ptr; //lint !e923 cast from pointer to unsigned long [Encompasses MISRA 2004 Rule 11.1], [MISRA 2004 Rule 11.3]
                    SFUpdater_Displacement_Data_Index                     = 0;

                    (void) RequestSFDisplacementData(SFUPDATER_RAM_DATA_ALIGNMENT);
                }
                else
                {
                    SFUpdater_Requester_Status  = SFUPDATER_REQUESTER_IDLE;
                    response                    = SFUPDATER_REQUESTER_REQUEST_DISCARDED;
                }
            }
            else
            {
                response =  SFUPDATER_REQUESTER_REQUEST_DISCARDED;
            }
            break;

        case SFUPDATER_REQUESTER_PROCESSING:
            if (Millis__ReachedTarget(&SFUpdater_Requester_Timer, SFUpdater_Requester_Timeout) != FALSE)
            {
                SFUpdater_Requester_Status = SFUPDATER_REQUESTER_IDLE;
                response =  SFUPDATER_REQUESTER_REQUEST_DISCARDED;
            }
            else if ((SFUpdater_Requested_Displacement_Info.SF_Pointer_ID  == sf_displacement_info->SF_Pointer_ID) &&
                     (SFUpdater_Requested_Displacement_Info.SF_Displacement == sf_displacement_info->SF_Displacement))
            {
                response =  SFUPDATER_REQUESTER_PROCESSING;
            }
            else
            {
                response =  SFUPDATER_REQUESTER_BUSY;
            }
            break;

        case SFUPDATER_REQUESTER_DATA_READY:
            if ((SFUpdater_Requested_Displacement_Info.SF_Pointer_ID  == sf_displacement_info->SF_Pointer_ID) &&
                (SFUpdater_Requested_Displacement_Info.SF_Displacement == sf_displacement_info->SF_Displacement))
            {
                sf_displacement_info->Crc  = SFUpdater_Requested_Displacement_Info.Crc;
                sf_displacement_info->Size = SFUpdater_Requested_Displacement_Info.Size;

                response =  SFUPDATER_REQUESTER_DATA_READY;
                SFUpdater_Requester_Status = SFUPDATER_REQUESTER_IDLE;
            }
            else if (Millis__ReachedTarget(&SFUpdater_Requester_Timer, SFUpdater_Requester_Timeout) != FALSE)
            {
                if ((sf_displacement_info->SF_Pointer_ID != SF_PTR_INVALID_VALUE) &&
                    (sf_displacement_info->SF_Displacement != (uint16) SF_DISPL_INVALID_VALUE))
                {
                    // Set the timer
                    Millis__Start(&SFUpdater_Requester_Timer);
                    SFUpdater_Requester_Timeout = SFUPDATER_DISPLACEMENT_RESPONSE_TIMEOUT;

                    SFUpdater_Requester_Status  = SFUPDATER_REQUESTER_PROCESSING;
                    response                    =  SFUPDATER_REQUESTER_PROCESSING;

                    SFUpdater_Requested_Displacement_Info.SF_Pointer_ID   = sf_displacement_info->SF_Pointer_ID;
                    SFUpdater_Requested_Displacement_Info.SF_Displacement = sf_displacement_info->SF_Displacement;

                    if (request_type == 0U)
                    {
                        (void) API007Data__RequestSettingFileDisplacementInfo(WIN_ACU_ADDRESS, SFUpdater_Requested_Displacement_Info.SF_Pointer_ID, SFUpdater_Requested_Displacement_Info.SF_Displacement);
                    }
                    else if (sf_displacement_info->Data_Ptr != NULL)
                    {
                        SFUpdater_Received_Displacement_Info.SF_Pointer_ID    = sf_displacement_info->SF_Pointer_ID;;
                        SFUpdater_Received_Displacement_Info.SF_Displacement  = sf_displacement_info->SF_Displacement;;
                        SFUpdater_Received_Displacement_Info.Size             = sf_displacement_info->Size;
                        SFUpdater_Displacement_Address                        = (FLASH_ADDRESS_TYPE)sf_displacement_info->Data_Ptr; //lint !e923 cast from pointer to unsigned long [Encompasses MISRA 2004 Rule 11.1], [MISRA 2004 Rule 11.3]
                        SFUpdater_Displacement_Data_Index                     = 0;

                        (void) RequestSFDisplacementData(SFUPDATER_RAM_DATA_ALIGNMENT);
                    }
                    else
                    {
                        SFUpdater_Requester_Status  = SFUPDATER_REQUESTER_IDLE;
                        response                    = SFUPDATER_REQUESTER_REQUEST_DISCARDED;
                    }
                }
                else
                {
                    response =  SFUPDATER_REQUESTER_REQUEST_DISCARDED;
                }
            }
            else
            {
                response =  SFUPDATER_REQUESTER_REQUEST_DISCARDED;
            }
            break;

        default:
            SFUpdater_Requester_Status = SFUPDATER_REQUESTER_IDLE;
            response =  SFUPDATER_REQUESTER_REQUEST_DISCARDED;
            break;
    }

    return(response);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This function will return TRUE if the requested section ID is present in the list
 * @param sf_id
 *
 * @return BOOL_TYPE
 *  @retval TRUE(1)
 *  @retval FALSE(0)
 */
#if (SFUPDATER_MANAGE_SF_ID_DATA == ENABLED)
BOOL_TYPE ValidateSectionID(SF_ID_TYPE sf_id)
{
    BOOL_TYPE verification = FALSE;

    for (uint8 section_index = 0; SFUPDATER_SECTIONS[section_index] != SF_ID_END; section_index++)
    {
        if (sf_id == SFUPDATER_SECTIONS[section_index])
        {
            verification = TRUE;
            break;
        }
    }

    return verification;
}

#endif

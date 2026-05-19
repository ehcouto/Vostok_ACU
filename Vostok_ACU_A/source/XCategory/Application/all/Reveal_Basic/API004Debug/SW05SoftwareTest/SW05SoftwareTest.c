/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2023-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "SW05SoftwareTest.h"

#if (SW05_SOFTWARE_TESTS_FEATURE == ENABLED)
#include "SRSystemConfig.h"
#include "API004Debug.h"
#include "Utilities.h"
#include <string.h>


//--- Include other modules
#include "StackAnalysis.h"
#include "TimingAnalysis.h"
#include "SRTaskMonitor.h"
#include "Reveal.h"
#include "Micro.h"
#include "SettingFile.h"
#include "Millis.h"
#include "NVData.h"
#include "SRIsrMonitor.h"
#include "SRClockMonitor.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
#if (CLASSB_FEATURE == ENABLED)
    #if ((SYSTEM_HMI_HANDLING == ENABLED) || (SYSTEM_HMI_EXPANSION_HANDLING == ENABLED))
        #if (SR_HMI_EVENTS_FEATURE  != ENABLED)
            SW05_WARNING(12, "SR_HMI_EVENTS_FEATURE is disabled", SW05_CLASSB)
        #endif
    #endif
#endif

#define SW05_BUFFER_SIZE            REVEAL_PAYLOAD_SIZE
#define SW05_ECHO_WIN_BUFFER_SIZE   10U

#define FAILED_TO_RESET_BY_WATCHDOG 0xFAU
#define UNSUPPORTED_TEST            0xFBU
#define TOOL_COMMUNICATION_TIMEOUT  (uint32)(5*60)     //5min

// This always points to the next free location of the 'SW05_Fbk'
static uint8 SW05_Fbk_Index;
static MILLIS_TIMER_TYPE Node_Run_Timer;
static MILLIS_TIMER_TYPE Refresh_Timer;

static uint8 SW05_Fbk[SW05_BUFFER_SIZE];

#if (NVDATA_STORE_APPLIANCE_AND_STATISTICS_FEATURE == ENABLED)
    #pragma location = "_appliance_segment"
    static uint32 SW05_Appliance_Data;

    #pragma location = "_statistics_segment"
    static uint32 SW05_Statistics_Data;
#endif

#if (NVDATA_STORE_STATIC_FEATURE == ENABLED)
    #pragma location = "_static_cfg_segment"
    static uint32 SW05_Static_Data;
#endif

static uint8 SW05_Win_Echo[SW05_ECHO_WIN_BUFFER_SIZE];

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define SW05_TEST_ID                       (SW05_TEST_ID_TYPE)buffer_ptr[0]
#define SW05_TEST_ID_PARAM_SIZE            buffer_ptr[1]
#define SW05_TEST_ID_PARAM_DATA_PTR(index) (&buffer_ptr[2 + index])

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void QueueNewFeedback(SW05_TEST_ID_TYPE test_id);

static void QueueAddU8(uint8 data_u8);
static void QueueAddU16(uint16 data_u16);
static void QueueAddU32(uint32 data_u32);
static void QueueAddU8Buffer(uint8 *data_buffer_ptr, uint8 size);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module SW05SoftwareTest and its variables
 *
 */
void SW05SoftwareTest__Initialize(void)
{
    Millis__Start(&Node_Run_Timer);
    Millis__Start(&Refresh_Timer);

    memset(SW05_Fbk, 0,sizeof(SW05_Fbk));
    SW05_Fbk_Index = 0;
}



//---------------------------------------------------------------------------------------------------------------------
/**
 * @details This API handles the sw05 test requests coming from tool
 * @details - called from API004
 *
 * @param source_node_id -node to whom are accountable to answer
 * @param buffer_length - pay load length
 * @param buffer_ptr - data address
 *
 */
void SW05SoftwareTest__Execute(uint8 source_node_id, uint8 buffer_length, uint8 *buffer_ptr)
{
    // Reset the 'node run time' if the tool is disconnected for more then TOOL_COMMUNICATION_TIMEOUT time
    if (TOOL_COMMUNICATION_TIMEOUT < Millis__GetElapsedSeconds(&Refresh_Timer))
    {
        Millis__Start(&Node_Run_Timer);
    }

    Millis__Start(&Refresh_Timer);

    uint16 run_time_min = (uint16)(Millis__GetElapsedSeconds(&Node_Run_Timer)/60U);


    QueueNewFeedback(SW05_TEST_ID);

    switch(SW05_TEST_ID)
    {
        case SW05_TEST_ID_FORCE_WATCHDOG:

            /*
             * data[0]    - Test ID
             * data[1]    - FAILED_TO_RESET_BY_WATCHDOG
             *
             */

            DISABLE_INTERRUPTS();

            //This is an alternative to reset the micro if the watchdog fails to reset the micro
            //This reset is asserted after running the delay for more then 2seconds
            for(uint16 num_loops = 0; num_loops < 10000U; num_loops++)
            {
                Micro__DelayNumNops(0xFFFF);
            }

            // Failed to reset by watch dog
            ENABLE_INTERRUPTS();
            QueueAddU8(FAILED_TO_RESET_BY_WATCHDOG);
            break;

        case SW05_TEST_ID_ECHO_WIN_DATA:
            /*
             * data[0]    - Test ID
             * data[1]    - Echo_Data[0]
             * data[2]    - Echo_Data[1]
             * ...
             * data[10]   - Echo_Data[9]
             *
             */
            QueueAddU8Buffer(SW05_Win_Echo, SW05_ECHO_WIN_BUFFER_SIZE);

            for (uint8 index = 0; index < SW05_TEST_ID_PARAM_SIZE; index++)
            {
                if (index < SW05_ECHO_WIN_BUFFER_SIZE)
                {
                    SW05_Win_Echo[index] = SW05_TEST_ID_PARAM_DATA_PTR(0)[index];
                }
                else
                {
                    break;
                }
            }
            break;

        case SW05_TEST_ID_SETTINGFILE_STATS:
            {
                /*
                 * data[0]      - Test ID
                 * data[1-4]    - Allocated SF size
                 * data[5-8]    - Consumed SF size
                 */
                uint32 sf_size = 0;

                sf_size += SettingFile__GetSettingFileSize(SF_ID_ACU);
                sf_size += SettingFile__GetSettingFileSize(SF_ID_HMI);
                sf_size += SettingFile__GetSettingFileSize(SF_ID_MCU);
                sf_size += SettingFile__GetSettingFileSize(SF_ID_ACU_EXP_1);
                sf_size += SettingFile__GetSettingFileSize(SF_ID_ACU_EXP_2);
                sf_size += SettingFile__GetSettingFileSize(SF_ID_ACU_EXP_3);
                sf_size += SettingFile__GetSettingFileSize(SF_ID_ACU_EXP_4);
                sf_size += SettingFile__GetSettingFileSize(SF_ID_ACU_EXP_5);
                sf_size += SettingFile__GetSettingFileSize(SF_ID_HMI_EXP_1);
                sf_size += SettingFile__GetSettingFileSize(SF_ID_HMI_EXP_2);
                sf_size += SettingFile__GetSettingFileSize(SF_ID_HMI_EXP_3);
                sf_size += SettingFile__GetSettingFileSize(SF_ID_HMI_EXP_4);
                sf_size += SettingFile__GetSettingFileSize(SF_ID_HMI_EXP_5);

                QueueAddU32(SettingFile__GetMemorySectionSize());
                QueueAddU32(sf_size);
            }
            break;


        case SW05_TEST_ID_MICRO_CLOCK_INFO:
            /*
             * data[0]      - Test ID
             * data[1]      - MICRO_CLOCK_DEF[0]
             * data[2]      - MICRO_CLOCK_DEF[1]
             * ....
             */

            QueueAddU8Buffer((uint8 *)Micro__GetClock(), sizeof(MICRO_CLOCK_DEF));
            break;


            #if (NVDATA_STORE_APPLIANCE_AND_STATISTICS_FEATURE == ENABLED)
                case SW05_TEST_ID_GETSET_APPLIANCE_AND_STATISTIC_NVDATA:
                    {
                        /*
                         * data[0]      - Test ID
                         * data[1]      - SW05_Appliance_Data[0]
                         * data[2]      - SW05_Appliance_Data[1]
                         * data[3]      - SW05_Appliance_Data[2]
                         * data[4]      - SW05_Appliance_Data[3]
                         * data[5]      - SW05_Statistics_Data[0]
                         * data[6]      - SW05_Statistics_Data[1]
                         * data[7]      - SW05_Statistics_Data[2]
                         * data[8]      - SW05_Statistics_Data[3]
                         */

                        //Get the Data
                        QueueAddU32(SW05_Appliance_Data);
                        QueueAddU32(SW05_Statistics_Data);

                        //Then Set the Data
                        SW05_Appliance_Data = EXTRACTU32(SW05_TEST_ID_PARAM_DATA_PTR(0));
                        SW05_Statistics_Data = EXTRACTU32(SW05_TEST_ID_PARAM_DATA_PTR(4));
                    }
                    break;
            #endif

            #if (NVDATA_STORE_STATIC_FEATURE == ENABLED)
                case SW05_TEST_ID_GETSET_STATIC_NVDATA:
                    {
                        /*
                         * data[0]      - Test ID
                         * data[1]      - SW05_Static_Data[0]
                         * data[2]      - SW05_Static_Data[1]
                         * data[3]      - SW05_Static_Data[2]
                         * data[4]      - SW05_Static_Data[3]
                         */
                        //Get the Data
                        QueueAddU32(SW05_Static_Data);

                        //Then Set the Data
                        SW05_Static_Data = EXTRACTU32(SW05_TEST_ID_PARAM_DATA_PTR(0));
                        (void)NVData__StoreStaticData();
                    }
                    break;
            #endif

            case SW05_TEST_ID_INTERRUPTS_ENABLED:
                /*
                 * data[0]      - Test ID
                 * data[1]      - Number of Interrupts
                 */
                QueueAddU8(Micro__NumberOfInterruptsDefined());
                break;






            case SW05_TEST_ID_TASK_TIME_STATS:
                {
                    TIMING_ANALYSIS_TASK_DATA_TYPE *task_time_ptr = TimingAnalysis__GetTaskInfo(MAIN_TASK);
                    /*
                     * data[0]          - Test ID
                     * data[1 - 2 ]     - run_time_min
                     * data[3 - 6 ]     - cpuEntryMax
                     * data[7 - 10]     - cpuEntryMin
                     * data[11- 14]     - cpuloadMax ...
                     */

                    if (task_time_ptr != NULL)
                    {
                        QueueAddU16(run_time_min);

                        for(uint8 task_id = (uint8)MAIN_TASK; task_id < (uint8)NUM_TASKS; task_id++)
                        {
                            if(task_id > (uint8) MAIN_TASK)
                            {
                                task_time_ptr = TimingAnalysis__GetTaskInfo((TASK_ID_TYPE)task_id);
                            }
                            QueueAddU32(task_time_ptr->Cpu_Entry_Max);
                            QueueAddU32(task_time_ptr->Cpu_Entry_Min);
                            QueueAddU32(task_time_ptr->Cpu_Max_Load) ;
                        }
                    }
                }
                break;

            case SW05_TEST_ID_SRTASKMONITOR_STATS:
                {
                    /*
                     * data[0]          - Test ID
                     * data[1 - 2 ]     - run_time_min
                     *
                     * data[3 - 6 ]     - start_value[0]
                     * data[7 - 10]     - min_value[0]
                     * data[11- 14]     - max_value[0]
                     * data[15- 18]     - max_elapsed[0]
                     * ....               ....
                     * ....               ....
                     */

                    QueueAddU16(run_time_min);

                    SRTASKMONITOR_STATS_TYPE *task_time_ptr = NULL;

                    for(uint8 task_id = (uint8)MAIN_TASK; task_id < (uint8)NUM_TASKS; task_id++)
                    {
                        task_time_ptr = SRTaskMonitor__GetStatisticsData((TASK_ID_TYPE)task_id);

                        if (task_time_ptr != NULL)
                        {
                            QueueAddU16(task_time_ptr->start_value);
                            QueueAddU16(task_time_ptr->min_value);
                            QueueAddU16(task_time_ptr->max_value);
                            QueueAddU16(task_time_ptr->max_elapsed);
                        }
                        else
                        {
                            QueueAddU16(0);
                            QueueAddU16(0);
                            QueueAddU16(0);
                            QueueAddU16(0);
                        }
                    }
                }
                break;

            case SW05_TEST_ID_STACK_STATS:
                {
                    /*
                     * data[0]          - Test ID
                     * data[1 - 2 ]     - run_time_min
                     * data[1 - 2 ]     - stack_size
                     * data[3 - 4 ]     - stack_free_space
                     */

                    QueueAddU16(run_time_min);

                    STACKANALYSIS_STACK_DATA_TYPE stack_data;

                    for(uint8 stack_id = (uint8)MAIN_TASK_STACK; stack_id < (uint8)NUM_STACKS; stack_id++)
                    {
                        stack_data = StackAnalysis__GetStackInformation((STACK_ID_TYPE)stack_id);

                        QueueAddU16(stack_data.Total_Size);
                        QueueAddU16(stack_data.Free_Space);
                    }
                }
                break;

        case SW05_TEST_ID_REVEAL_STATS:
            /*
             * data[0]      - Test ID
             * data[1 - 2 ] - run_time_min
             * data[3]      - REVEAL_QUEUE_SIZE
             * data[4]      - Reveal__GetMinimumQueueSpace()
             * data[5]      - REVEAL_QUEUE_PRIORITY_SIZE
             * data[6]      - Reveal__GetMinimumPriorityQueueSpace()
             *
             */

            QueueAddU16(run_time_min);
            QueueAddU8(REVEAL_QUEUE_SIZE);
            QueueAddU8(Reveal__GetMinimumQueueSpace());
            QueueAddU8(REVEAL_QUEUE_PRIORITY_SIZE);
            QueueAddU8(Reveal__GetMinimumPriorityQueueSpace());
            break;

        case SW05_TEST_ID_SRISRMONITOR_STATS:
            if (SRIsrMonitor__GetStatisticsData() != (SRISR_STATISTICS_DATA_TYPE *)NULL)
            {
                /*
                 * data[0]         - Test ID
                 * data[1 - 2 ]    - run_time_min
                 * data[3 - 4 ]    - Isr_Min_Cntr
                 * data[5 - 6 ]    - Isr_Max_Cntr
                 * data[7     ]    - Isr_Attempt_Min_Cntr
                 * data[8     ]    - Isr_Attempt_Max_Cntr
                 * data[9 - 12]    - SRISRMONITOR_MIN
                 * data[13- 16]    - SRISRMONITOR_MAX
                 * data[17- 20]    - SRISRMONITOR_MAX_ATTEMPTS
                 *
                 */
                QueueAddU16(run_time_min);
                QueueAddU16(SRIsrMonitor__GetStatisticsData()->Isr_Min_Cntr);
                QueueAddU16(SRIsrMonitor__GetStatisticsData()->Isr_Max_Cntr);

                QueueAddU8(SRIsrMonitor__GetStatisticsData()->Isr_Attempt_Min_Cntr);
                QueueAddU8(SRIsrMonitor__GetStatisticsData()->Isr_Attempt_Max_Cntr);

                QueueAddU32((uint32)SRISRMONITOR_MIN);
                QueueAddU32((uint32)SRISRMONITOR_MAX);
                QueueAddU32((uint32)SRISRMONITOR_MAX_ATTEMPTS);
            }
        break;






        #if (SRCLOCKMONITOR_FEATURE == ENABLED)
            case SW05_TEST_ID_SRCLOCKMONITOR_STATS:
                if (SRClockMonitor__GetStatisticsData() != (SRCLOCKMONITOR_STATISTICS_DATA_TYPE *)NULL)
                {
                    /*
                     * data[0]          - Test ID
                     * data[1 - 2]      - run_time_min
                     * data[3 - 4]      - External_Ticks
                     * data[5 - 6]      - Max_Delta_Ticks
                     */
                    QueueAddU16(run_time_min);

                    QueueAddU16(SRClockMonitor__GetStatisticsData()->External_Ticks);
                    QueueAddU16(SRClockMonitor__GetStatisticsData()->Max_Delta_Ticks);
                }
                break;
        #endif


        default:
            QueueAddU8(UNSUPPORTED_TEST);
            break;
    }

    if (SW05_Fbk_Index != 0U)
    {
        API004Debug__SendFbkForFixedArgsFunctions(source_node_id, SW05_Fbk_Index, FALSE, SW05_Fbk);
        SW05_Fbk_Index = 0U;
    }
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Queue the Test Status
 * @param test_id
 * @param test_status
 */
static void QueueNewFeedback(SW05_TEST_ID_TYPE test_id)
{
    SW05_Fbk_Index = 0U;
    SW05_Fbk[SW05_Fbk_Index]      = test_id;
    SW05_Fbk_Index = 1U;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Queue the unsigned 8 bit data
 *
 * @param data_u8
 */
static void QueueAddU8(uint8 data_u8)
{
    SW05_Fbk[SW05_Fbk_Index + 0U]    = data_u8;

    SW05_Fbk_Index += 1U;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Queue the unsigned 16 bit data
 *
 * @param data_u16
 */
static void QueueAddU16(uint16 data_u16)
{
    SW05_Fbk[SW05_Fbk_Index+0U]    = HIBYTE(data_u16);
    SW05_Fbk[SW05_Fbk_Index+1U]    = LOBYTE(data_u16);

    SW05_Fbk_Index += 2U;
}



//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Queue the unsigned 32 bit data
 *
 * @param data_u32
 */
static void QueueAddU32(uint32 data_u32)
{
    SW05_Fbk[SW05_Fbk_Index+0U]    = (uint8)(data_u32 >> 24U);
    SW05_Fbk[SW05_Fbk_Index+1U]    = (uint8)(data_u32 >> 16U);
    SW05_Fbk[SW05_Fbk_Index+2U]    = (uint8)(data_u32 >> 8U);
    SW05_Fbk[SW05_Fbk_Index+3U]    = (uint8)(data_u32);

    SW05_Fbk_Index += 4U;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Queue the unsigned 8 bit array
 *
 * @param data_buffer_ptr
 * @param size
 */
static void QueueAddU8Buffer(uint8 *data_buffer_ptr, uint8 size)
{
    memcpy(&SW05_Fbk[(SW05_Fbk_Index + 0U)], data_buffer_ptr, size);
    SW05_Fbk_Index += size;
}
#endif

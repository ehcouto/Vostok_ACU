/**
 *  @file       SRMain.c
 *
 *  @brief      Nucleus embOS based intialization and tasks execution module
 *  @details
 *
 *  @defgroup   CLASS_B
 *
 *  @copyright  Copyright 2012-2021 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "RTOS.h"       // RTOS.h must be the first include statement
#include "SystemConfig.h"

// Class B Includes
#include "SRSystemConfig.h"
#include "SRMain.h"
#include "SRMain_prv.h"
#include "SRTaskMonitor.h"
#include "SRClockMonitor.h"
#include "SRTimerReference.h"
#include "SRException.h"
#include "SRData.h"
#if (SR_HMI_EVENTS_FEATURE == ENABLED)
    #include "SRHmiEvents.h"
#endif
#include "SRMicro.h"

#include "EmbOSBasic.h"
#if (BACKGROUNDTHREADS_FEATURE == ENABLED)
    #include "BackgroundThreads.h"
#endif
#include "Gpio.h"
#include "Log.h"
#include "Micro.h"
#include "Millis.h"
#include "Mode.h"
#include "Reveal.h"
#include "SettingFile.h"
#if(FAST_HANDLER_FEATURE == ENABLED)
    #include "Timer.h"
#endif
#include "TimerInterrupt.h"
#include "Uart.h"
#include "Win.h"
#include "SW05SoftwareTest.h"
#include "TimingAnalysis.h"

#ifndef SYSTEM_ACU_HANDLING
    #define SYSTEM_ACU_HANDLING  DISABLED
#endif

#ifndef SYSTEM_HMI_HANDLING
    #define SYSTEM_HMI_HANDLING  DISABLED
#endif

#ifndef SYSTEM_ACU_EXPANSION_HANDLING
    #define SYSTEM_ACU_EXPANSION_HANDLING   DISABLED
#endif

#ifndef SYSTEM_EXPANSION_HANDLING
    #define SYSTEM_EXPANSION_HANDLING       DISABLED
#endif

#ifndef SYSTEM_CONNECTIVITY_HANDLING
    #define SYSTEM_CONNECTIVITY_HANDLING    DISABLED
#endif

#ifndef SYSTEM_ENABLE_TASKSEQUENCE
    #define SYSTEM_ENABLE_TASKSEQUENCE  DISABLED
#endif

#ifndef SYSTEM_ENABLE_ISRMONITOR
    #define SYSTEM_ENABLE_ISRMONITOR    DISABLED
#endif

#if (SYSTEM_ACU_HANDLING == ENABLED) || (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED)
    #include "SRHbl.h"
#endif

#if (SR_HBLHMI_FEATURE == ENABLED)
    #include "SRHblHmi.h"
#endif

#if (SYSTEM_ENABLE_ISRMONITOR == ENABLED)
    #include "SRIsrMonitor.h"
#endif
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    #include "SRTaskSequence.h"
#endif

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//  Criterion to refresh watchdog - if unset, refresh it always
#ifndef MAIN__WATCHDOG_CHECK
#define MAIN__WATCHDOG_CHECK()      TRUE
#endif

#ifndef SRMAIN__BEFORE_1MS
#define SRMAIN__BEFORE_1MS()
#endif

#ifndef SRMAIN__AFTER_1MS
#define SRMAIN__AFTER_1MS()
#endif

// Task stack signature values
#define TASK_STACK_SIGNATURE_BEFORE_THREADS     0x4E75636CU
#define TASK_STACK_SIGNATURE_BETWEEN_THREADS_1  0x3B0A13B5U
#define TASK_STACK_SIGNATURE_BETWEEN_THREADS_2  0x6E96A347U
#define TASK_STACK_SIGNATURE_BETWEEN_THREADS_3  0x997D5920U
#define TASK_STACK_SIGNATURE_BETWEEN_THREADS_4  0xDE0BDF22U
#define TASK_STACK_SIGNATURE_BETWEEN_THREADS_5  0x592B3F10U
#define TASK_STACK_SIGNATURE_AFTER_THREADS      0x65757321U

// System stack and related signatures sections
#pragma section = "STACK_UNDERFLOW_SIGNATURE"
#pragma section = "STACK_OVERFLOW_SIGNATURE"
#pragma section = "INTERRUPT_STACK"
#pragma section = "SYSTEM_STACK"

#define STACK_TEST_START_ADDRESS                (__section_begin("STACK_OVERFLOW_SIGNATURE"))
#define STACK_TEST_END_ADDRESS                  (__section_begin("STACK_UNDERFLOW_SIGNATURE"))
#define INTERRUPT_STACK_START_ADDRESS           (__section_begin("INTERRUPT_STACK"))
#define INTERRUPT_STACK_END_ADDRESS             (__section_end("INTERRUPT_STACK"))
#define SYSTEM_STACK_START_ADDRESS              (__section_begin("SYSTEM_STACK"))
#define SYSTEM_STACK_END_ADDRESS                (__section_end("SYSTEM_STACK"))

//=================================================================================
// Main Thread
//=================================================================================
#if (MAIN_THREAD_MONITOR != ENABLED)
    SW05_WARNING(6, "MAIN_THREAD_MONITOR is disabled", SW05_CLASSB)
#endif


#define MAIN_THREAD_INTERVAL_NORMAL_POWER  10
#define MAIN_THREAD_INTERVAL_LOW_POWER     2
#ifdef SRMAIN_CUSTOMIZE_MAIN_THREAD_STACK_SIZE
#define MAIN_THREAD_STACK_SIZE      SRMAIN_CUSTOMIZE_MAIN_THREAD_STACK_SIZE
#else
#define MAIN_THREAD_STACK_SIZE      128
#endif
#define MAIN_THREAD_PRIORITY        120

#ifndef MAIN_THREAD_MONITOR
#define MAIN_THREAD_MONITOR                 DISABLED
#endif


OS_TASK MainTCB;
#pragma location="TASKS_STACK_MAIN_THREAD"
OS_STACKPTR uint32 MainTaskStartStk[MAIN_THREAD_STACK_SIZE];
static void MainThread(void);

//=================================================================================
// AppMgr Thread
//=================================================================================
#if (MAIN_APPMGR_THREAD_MONITOR != ENABLED)
    SW05_WARNING(7, "MAIN_APPMGR_THREAD_MONITOR is disabled", SW05_CLASSB)
#endif

#define MAIN_APPMGR_THREAD_INTERVAL         100
#ifdef SRMAIN_CUSTOMIZE_MAIN_APPMGR_THREAD_STACK_SIZE
#define MAIN_APPMGR_THREAD_STACK_SIZE      SRMAIN_CUSTOMIZE_MAIN_APPMGR_THREAD_STACK_SIZE
#else
#define MAIN_APPMGR_THREAD_STACK_SIZE      128
#endif
#define MAIN_APPMGR_THREAD_PRIORITY         40

#ifndef MAIN_APPMGR_THREAD_MONITOR
#define MAIN_APPMGR_THREAD_MONITOR          DISABLED
#endif

OS_TASK MainAppMgrTCB;
#pragma location="TASKS_STACK_APPMGR_THREAD"
OS_STACKPTR uint32 MainAppMgrTaskStartStk[MAIN_APPMGR_THREAD_STACK_SIZE];
static void MainAppMgrInitialize(void);
static void MainAppMgrThread(void);

//=================================================================================
// FastAppMgr Thread
//=================================================================================
#if (MAIN_FASTAPPMGR_THREAD_MONITOR != ENABLED)
    SW05_WARNING(8, "MAIN_FASTAPPMGR_THREAD_MONITOR is disabled", SW05_CLASSB)
#endif

#define MAIN_FASTAPPMGR_THREAD_INTERVAL         20
#define MAIN_FASTAPPMGR_THREAD_PRIORITY         60

#ifndef MAIN_FASTAPPMGR_THREAD_MONITOR
#define MAIN_FASTAPPMGR_THREAD_MONITOR          DISABLED
#endif

#ifdef SRMAIN_CUSTOMIZE_MAIN_FASTAPPMGR_THREAD_STACK_SIZE
#define MAIN_FASTAPPMGR_THREAD_STACK_SIZE           SRMAIN_CUSTOMIZE_MAIN_FASTAPPMGR_THREAD_STACK_SIZE
#else
#define MAIN_FASTAPPMGR_THREAD_STACK_SIZE           128
#endif

#pragma location="TASKS_STACK_FASTAPPMGR_THREAD"
OS_STACKPTR uint32 MainFastAppMgrTaskStartStk[MAIN_FASTAPPMGR_THREAD_STACK_SIZE];

OS_TASK MainFastAppMgrTCB;
// The stack for the Main FastAppMgr thread is defined in the FastAppMgr Stack section
static void MainFastAppMgrInitialize(void);
static void MainFastAppMgrThread(void);

//=================================================================================
// Communication Thread
//=================================================================================
#if (MAIN_COMMUNICATION_THREAD_MONITOR != ENABLED)
    SW05_WARNING(9, "MAIN_COMMUNICATION_THREAD_MONITOR is disabled", SW05_CLASSB)
#endif

#define MAIN_COMMUNICATION_THREAD_INTERVAL          5
#ifdef SRMAIN_CUSTOMIZE_MAIN_COMMUNICATION_THREAD_STACK_SIZE
#define MAIN_COMMUNICATION_THREAD_STACK_SIZE        SRMAIN_CUSTOMIZE_MAIN_COMMUNICATION_THREAD_STACK_SIZE
#else
#define MAIN_COMMUNICATION_THREAD_STACK_SIZE        128
#endif
#define MAIN_COMMUNICATION_THREAD_PRIORITY          80

#ifndef MAIN_COMMUNICATION_THREAD_MONITOR
#define MAIN_COMMUNICATION_THREAD_MONITOR   DISABLED
#endif

OS_TASK MainCommunicationTCB;
#pragma location="TASKS_STACK_COMMUNICATION_THREAD"
OS_STACKPTR uint32 MainCommunicationTaskStartStk[MAIN_COMMUNICATION_THREAD_STACK_SIZE];
static void MainCommunicationInitialize(void);
static void MainCommunicationThread(void);
static BOOL_TYPE Main_Communication_Enabled;

//=================================================================================
// Asynch Thread
//=================================================================================
#if (MAIN_ASYNCH_THREAD_MONITOR != ENABLED)
    SW05_WARNING(10, "MAIN_ASYNCH_THREAD_MONITOR is disabled", SW05_CLASSB)
#endif

#define MAIN_ASYNCH_THREAD_INTERVAL         5
#define MAIN_ASYNCH_THREAD_PRIORITY         100

#ifndef MAIN_ASYNCH_THREAD_MONITOR
#define MAIN_ASYNCH_THREAD_MONITOR          DISABLED
#endif

OS_TASK MainProgrammingAsynchTCB;
// The stack for the Main Asynch thread is defined in the Asynch/Programming Stack section and is shared with the Programming thread
static void MainAsynchInitialize(void);
static void MainAsynchThread(void);

//=================================================================================
// Programming Thread
//=================================================================================
#define MAIN_PROGRAMMING_THREAD_INTERVAL    10
#define MAIN_PROGRAMMING_THREAD_PRIORITY    60

#ifndef MAIN_PROGRAMMING_THREAD_MONITOR
#define MAIN_PROGRAMMING_THREAD_MONITOR     DISABLED
#endif

// The stack for the Main Programming thread is defined in the Asynch/Programming Stack section and is shared with the Asynch thread
static void MainProgrammingInitialize(void);
static void MainProgrammingThread(void);

//=================================================================================
// Asynch/Programming Stack
//=================================================================================
#ifdef SRMAIN_CUSTOMIZE_MAIN_PROGRAMMING_ASYNCH_THREAD_STACK_SIZE
#define MAIN_PROGRAMMING_ASYNCH_THREAD_STACK_SIZE           SRMAIN_CUSTOMIZE_MAIN_PROGRAMMING_ASYNCH_THREAD_STACK_SIZE
#else
#define MAIN_PROGRAMMING_ASYNCH_THREAD_STACK_SIZE           128
#endif

#pragma location="TASKS_STACK_PROGRAMMINGASYNCH_THREAD"
OS_STACKPTR uint32 MainProgrammingAsynchTaskStartStk[MAIN_PROGRAMMING_ASYNCH_THREAD_STACK_SIZE];


//---------------------------------------------------------------------------------
// Signatures for protection of the thread stacks
//---------------------------------------------------------------------------------
#pragma location="TASKS_STACK_SIG_BEFORE_THREADS"
uint32 Stack_Signature_Before_Threads;

#pragma location="TASKS_STACK_SIG_BETWEEN_THREADS_1"
uint32 Stack_Signature_Between_Threads_1;

#pragma location="TASKS_STACK_SIG_BETWEEN_THREADS_2"
uint32 Stack_Signature_Between_Threads_2;

#pragma location="TASKS_STACK_SIG_BETWEEN_THREADS_3"
uint32 Stack_Signature_Between_Threads_3;

#pragma location="TASKS_STACK_SIG_BETWEEN_THREADS_4"
uint32 Stack_Signature_Between_Threads_4;

#pragma location="TASKS_STACK_SIG_BETWEEN_THREADS_5"
uint32 Stack_Signature_Between_Threads_5;

#pragma location="TASKS_STACK_SIG_AFTER_THREADS"
uint32 Stack_Signature_After_Threads;

static BOOL_TYPE Main_Normal_Enabled;
extern OS_GLOBAL OS_Global;

#if(FAST_HANDLER_FEATURE == ENABLED)
    TIMER_PARAMETER_TYPE SRMain_FastHandler_Timer;
#endif

static uint32 * const SIGNATURE_PTR_LIST[] =
{
    &Stack_Signature_Before_Threads,
    &Stack_Signature_Between_Threads_1,
    &Stack_Signature_Between_Threads_2,
    &Stack_Signature_Between_Threads_3,
    &Stack_Signature_Between_Threads_4,
    &Stack_Signature_Between_Threads_5,
    &Stack_Signature_After_Threads
};

static uint32 const SIGNATURE_VALUES_LIST[] =
{
    TASK_STACK_SIGNATURE_BEFORE_THREADS,
    TASK_STACK_SIGNATURE_BETWEEN_THREADS_1,
    TASK_STACK_SIGNATURE_BETWEEN_THREADS_2,
    TASK_STACK_SIGNATURE_BETWEEN_THREADS_3,
    TASK_STACK_SIGNATURE_BETWEEN_THREADS_4,
    TASK_STACK_SIGNATURE_BETWEEN_THREADS_5,
    TASK_STACK_SIGNATURE_AFTER_THREADS
};

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void CheckAllocationOfAllStacks(void);
static BOOL_TYPE IsTaskStackInOneFreeSlot(uint32 *slot_bitmap_ptr,uint32 *stack_ptr, uint32 size_words);
static void ExecuteTasks(FUNCTION_TYPE * task_list);
static OS_TIME FindNextSlot(OS_TIME last_exec, OS_TIME slot_time);
#if(FAST_HANDLER_FEATURE == ENABLED)
    static void MainFastHandlerInitialize(void);
#endif
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This is the main function where all the application is launched from.
 */
void main(void)
{
    Main_Communication_Enabled = FALSE;
    Main_Normal_Enabled = FALSE;

    // Basic Micro and System Initialization
    SRMicro__Initialize();

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__Initialize();
        SRTaskSequence__StartSequence(SRTASKSEQUENCE_INIT_PHASE);
    #endif
        SRTaskMonitor__Initialize();
    #if (SYSTEM_ENABLE_ISRMONITOR == ENABLED)
        SRIsrMonitor__Initialize();
    #endif
    SRTimerReference__Initialize();
    SRClockMonitor__Initialize();

    Gpio__Initialize();
    Log__Initialize();
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_INIT_PHASE, SRTASKSEQUENCE_INIT_MICRO);
#endif
    SettingFile__Initialize();

    //This call will publish the last logged exception
    SRException__LogLastException();

    // Extra Initialization tasks if needed
    ExecuteTasks(SRMainGeneralInitialization_Tasks);

    Millis__Initialize();
    SW05SoftwareTest__Initialize();
    TimingAnalysis__Initialize();

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_INIT_PHASE, SRTASKSEQUENCE_INIT_INITIALIZATION_TASKS);
    #endif

    // Initialize Task signatures and check their correct addresses order
    //  Suppressed Note[923]: cast from pointer to unsigned long [MISRA 2012 Rule 11.6, required] to perform address check

    for(uint8 index = 0U; index < ELEMENTS_IN_ARRAY(SIGNATURE_PTR_LIST); index++)
    {
        *(SIGNATURE_PTR_LIST[index]) = SIGNATURE_VALUES_LIST[index];
        if(((index > 0U) &&
            ((uint32) SIGNATURE_PTR_LIST[index] <= (uint32) SIGNATURE_PTR_LIST[index - 1U])) ||          //lint !e923
            ((uint32) SIGNATURE_PTR_LIST[index] < (uint32) STACK_TEST_START_ADDRESS) ||                  //lint !e923
            ((uint32) SIGNATURE_PTR_LIST[index] > (uint32) STACK_TEST_END_ADDRESS))                      //lint !e923
        {
            SRException__Queue(SREXCEPTION_STACK_OVERFLOW, 0, 1);
        }
    }
    CheckAllocationOfAllStacks();

    // Flush out the data related to the tasks
    memset(&MainTCB,0,sizeof(OS_TASK));
    memset(&MainAppMgrTCB,0,sizeof(OS_TASK));
    memset(&MainCommunicationTCB,0,sizeof(OS_TASK));
    memset(&MainProgrammingAsynchTCB,0,sizeof(OS_TASK));
    memset(&MainFastAppMgrTCB,0,sizeof(OS_TASK));

    // Flush the Stacks for the tasks
    memset(MainTaskStartStk,0,sizeof(MainTaskStartStk));
    memset(MainAppMgrTaskStartStk,0,sizeof(MainAppMgrTaskStartStk));
    memset(MainCommunicationTaskStartStk,0,sizeof(MainCommunicationTaskStartStk));
    memset(MainProgrammingAsynchTaskStartStk,0,sizeof(MainProgrammingAsynchTaskStartStk));
    memset(MainFastAppMgrTaskStartStk,0,sizeof(MainFastAppMgrTaskStartStk));

    EmbOSBasics__FlushOS();                         // Flush the OS so all previously loaded tasks are reinitialized.
    OS_IncDI();                                     // Initially disable interrupts
    OS_InitKern();                                  // Initialize OS
    TimerInterrupt__Initialize();                   // Initialize the TimerInterrupt used as tick for the OS

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_INIT_PHASE, SRTASKSEQUENCE_INIT_OS_INITALIZE);
    #endif

    //ENABLE_INTERRUPTS();
    #if (MAIN_THREAD_MONITOR == ENABLED)
        SRTaskMonitor__EnableMonitor(SRTASKMONITOR_MAIN_TASK);
    #endif

    OS_CREATETASK(&MainTCB, "Main Task", MainThread,  MAIN_THREAD_PRIORITY, MainTaskStartStk);
    ENABLE_INTERRUPTS();
    OS_Start();                                     // Start multitasking
}


//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Interrupt handler for the timer interrupt.
 *
 *  @details    This routine implements an interrupt service routine with a gated slotted
 *              architecture.  Each interrupt calls the routines for the next interrupt slot.
 *              It can be configured through the parameter files.
 */
INTERRUPT_DIRECTIVE void SRMain__TimerISR( void )
{
    #if (SYSTEM_ENABLE_ISRMONITOR == ENABLED)
        SRIsrMonitor__EnterIsr();
    #endif
    TimingAnalysis__SampleInterrupt(TIMING_ANALYSIS_INTERRUPT_START);
    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__StartSequence(SRTASKSEQUENCE_TICK_HANDLER);
        SRTaskSequence__Handler();
    #endif
    SRMAIN__BEFORE_1MS();
    SRTimerReference__Handler1ms();
    SRTaskMonitor__Handler();

    SRClockMonitor__Monitor1ms();

    if (Main_Communication_Enabled == TRUE)
    {
        Win__Handle1ms();
        Reveal__Handler1ms();
    }

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_TICK_HANDLER, SRTASKSEQUENCE_TICK_COMMUNICATION);
    #endif

    if (Main_Normal_Enabled == TRUE)
    {
        ExecuteTasks(SRMainNormalMode1ms_Tasks);
    }

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_TICK_HANDLER, SRTASKSEQUENCE_TICK_TASKS);
    #endif

    Millis__Handler1ms();

    // Periodic tasks stack signatures
    for(uint8 index = 0U; index < ELEMENTS_IN_ARRAY(SIGNATURE_PTR_LIST); index++)
    {
        if(*(SIGNATURE_PTR_LIST[index]) != SIGNATURE_VALUES_LIST[index])
        {
            SRException__Queue(SREXCEPTION_STACK_OVERFLOW, 0, 0);
        }
    }

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_TICK_HANDLER, SRTASKSEQUENCE_TICK_TASK_STACK_SIGNATURE);
    #endif

    SRMAIN__AFTER_1MS();
    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__EndSequence(SRTASKSEQUENCE_TICK_HANDLER);
    #endif
    TimingAnalysis__SampleInterrupt(TIMING_ANALYSIS_INTERRUPT_END);
    #if (SYSTEM_ENABLE_ISRMONITOR == ENABLED)
        SRIsrMonitor__ExitIsr();
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Fast Interrupt handler for the timer interrupt.
 *
 *  @details    This is called outside the EmbOS thread
 *              All the tasks to be executed here shall not use the EmbOS functions
 */
void SRMain__FastTimerISR(void)
{
    #if(FAST_HANDLER_FEATURE == ENABLED)
        Timer__OutputCompareStartFromLast(&SRMain_FastHandler_Timer, TIMER_ONCOMPARE_INTERRUPT, FAST_HANDLER_TIMING_USEC);
        if (Main_Normal_Enabled == TRUE)
        {
            ExecuteTasks(SRMainNormalModeFastInterrupt_Tasks);
        }
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   DEPRECATED - Analog task has been removed.
 *          This method is called from another process to unlock the Analog tasks. For the ACU it will be handled from
 *          the 270 degrees on the mains wave.
 */
void SRMain__UnlockAnalogTask(void)
{
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////   Main Thread
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Main Task which is responsible to define the status and decide which task will be executed versus which one will not.
 */
static void MainThread(void)
{
    OS_TIME this_timer;

    OS_EnterRegion();                                       // Disable the Scheduler

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_INIT_PHASE, SRTASKSEQUENCE_INIT_START_MAIN_THREAD);
    #endif

    Mode__Initialize();                                     // Initialize the Mode module which defines the Mode the software will start.

    if ((Mode__GetMode() != MODE_PROGRAMMING) &&
        (SettingFile__IsValid() == FALSE))                  // If Setting file is not consistent, force the Not Consistent mode
    {
        Mode__SetMode(MODE_SETTINGFILE_NOT_CONSISTENT);
    }
    else if (Mode__GetMode() == MODE_SETTINGFILE_NOT_CONSISTENT) // If Setting file is consistent but the mode is Not Consistent, force the Normal mode
    {
        Mode__SetMode(MODE_NORMAL);
    }

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_INIT_PHASE, SRTASKSEQUENCE_INIT_START_MODE_THREADS);
    #endif

    switch (Mode__GetMode())
    {
        default:
            Mode__SetMode(MODE_NORMAL);
            //lint -fallthrough
        case MODE_NORMAL:
            #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
                SRTaskSequence__StartSequence(SRTASKSEQUENCE_APPL_SAFETY_INIT);
            #endif

            ExecuteTasks(SRMainNormalInitialization_Tasks);

            #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
                SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_INIT, SRTASKSEQUENCE_APPL_INIT_HBL_DONE);
                SRTaskSequence__EndSequence(SRTASKSEQUENCE_APPL_SAFETY_INIT);
            #endif

            MainAsynchInitialize();
            MainAppMgrInitialize();
            MainFastAppMgrInitialize();
            #if (BACKGROUNDTHREADS_FEATURE == ENABLED)
                BackgroundThreads__Initialize();
            #endif
            #if(FAST_HANDLER_FEATURE == ENABLED)
                MainFastHandlerInitialize();
            #endif

            break;
        case MODE_PROGRAMMING:
        case MODE_SETTINGFILE_NOT_CONSISTENT:
            #if ((SETTINGFILE_MASTER_FILE_FEATURE == ENABLED) || (SETTINGFILE_SECTION_DOWNLOAD_FEATURE == ENABLED))
                MainProgrammingInitialize();
            #endif
                break;
        case MODE_LOWPOWER:
            ExecuteTasks(SRMainLowPowerInitialization_Tasks);
            break;
    }

    if (Mode__GetMode() != MODE_LOWPOWER)
    {
        MainCommunicationInitialize();
    }

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_INIT_PHASE, SRTASKSEQUENCE_INIT_COMPLETED);
        SRTaskSequence__EndSequence(SRTASKSEQUENCE_INIT_PHASE);
    #endif

    OS_LeaveRegion();
    this_timer = OS_GetTime();
    while(1)
    {
        TimingAnalysis__SampleTask(MAIN_TASK);
        #if (MAIN_THREAD_MONITOR == ENABLED)
            SRTaskMonitor__Enter(SRTASKMONITOR_MAIN_TASK);
        #endif
        #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
            SRTaskSequence__StartSequence(SRTASKSEQUENCE_MAIN_HANDLER);
        #endif

        Mode__Handler();
        #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
            SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_MAIN_HANDLER, SRTASKSEQUENCE_MAIN_MODE_HANDLER);
        #endif

        if (Mode__GetMode() != MODE_LOWPOWER)
        {
            ExecuteTasks(SRMain_Non_Low_Power_Tasks);
        }

        #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
            SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_MAIN_HANDLER, SRTASKSEQUENCE_MAIN_TASKS);
        #endif
        ExecuteTasks(SRMainRunning_Tasks);

        if (MAIN__WATCHDOG_CHECK())
        {
           SRMicro__ServiceWatchdog();
            #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
               SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_MAIN_HANDLER, SRTASKSEQUENCE_MAIN_WATCHDOG);
            #endif
        }

        #if (SYSTEM_ENABLE_ISRMONITOR == ENABLED)
            SRIsrMonitor__Handler();
        #endif

        #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
            SRTaskSequence__EndSequence(SRTASKSEQUENCE_MAIN_HANDLER);
        #endif
        #if (MAIN_THREAD_MONITOR == ENABLED)
            SRTaskMonitor__Exit(SRTASKMONITOR_MAIN_TASK);
        #endif

        #if (SRPOWERMGR_LOWPOWER_MODE != 0)
            // Setup thread timer based on mode; clock is slowed down in low power mode so thread can be called fewer times
            if (Mode__GetMode() == MODE_LOWPOWER)
            {
                this_timer = FindNextSlot(this_timer, MAIN_THREAD_INTERVAL_LOW_POWER);
            }
            else
        #endif
        {
            this_timer = FindNextSlot(this_timer, MAIN_THREAD_INTERVAL_NORMAL_POWER);
        }

        // Make sure interrupts are enabled. There is no reason for them to be disabled at this point.
        // Interrupts must be enabled so the watchdog can be kicked properly in this thread
        // System does not rely on the RTOS to detect interrupts being disabled
        ENABLE_INTERRUPTS();

        OS_DelayUntil(this_timer);
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////   AppMgr Thread
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Method to launch the Application Manager Task.
 */
static void MainAppMgrInitialize(void)
{
    #if (MAIN_APPMGR_THREAD_MONITOR == ENABLED)
        SRTaskMonitor__EnableMonitor(SRTASKMONITOR_APPLICATION_TASK);
    #endif
    OS_CREATETASK(&MainAppMgrTCB, "AppMgr Task", MainAppMgrThread,  MAIN_APPMGR_THREAD_PRIORITY, MainAppMgrTaskStartStk);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Application Manager Task responsible for the Supervisor and Interpreter processes.
 */
static void MainAppMgrThread(void)
{
    OS_TIME this_timer;

    ExecuteTasks(SRMain_AppMgr_Initialize);
    Main_Normal_Enabled = TRUE;

    this_timer = OS_GetTime();

    while(1)
    {
        TimingAnalysis__SampleTask(APPLICATION_TASK);
        #if (MAIN_APPMGR_THREAD_MONITOR == ENABLED)
            SRTaskMonitor__Enter(SRTASKMONITOR_APPLICATION_TASK);
        #endif
        #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
            SRTaskSequence__StartSequence(SRTASKSEQUENCE_APPL_SAFETY_THREAD);
        #endif

        #if (SYSTEM_ACU_HANDLING == ENABLED) || (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED)
            SRHbl__ResetLoadPriority();
        #endif

        SettingFile__CheckDataIntegrity();
        if (SettingFile__IsValid() == FALSE)                    // If Setting file data is invalid, force the "Not Consistent" mode
        {
            Mode__SetMode(MODE_SETTINGFILE_NOT_CONSISTENT);
            Micro__ForceReset(MICRO_RESET_MODE_COLD);           // Explicitly perform a cold reset since setting mode to "MODE_SETTINGFILE_NOT_CONSISTENT" won't cause a reset
        }

        #if (SR_HBLHMI_FEATURE == ENABLED)
            SRHblHmi__CheckDataIntegrity();
            #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
                SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_HMI_CLASSB_EXECUTED);
            #endif
        #endif

        ExecuteTasks(SRMain_AppMgr_Handler);
        #if (BACKGROUNDTHREADS_FEATURE == ENABLED)
            BackgroundThreads__Handler();
        #endif

        #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
            SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_CLASSA_EXECUTED);
        #endif

        #if (SYSTEM_ACU_HANDLING == ENABLED) || (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED)
            SRHbl__Process();
        #endif

        #if (SR_HMI_EVENTS_FEATURE == ENABLED)
            SRHmiEvents__Background();
        #endif

        #if (CLASSB_FEATURE == ENABLED)

            //This method performs CPU register and Stack test periodically.
            Micro__SRRamTest();

            //This method performs non-volatile memory (FLASH) test periodically.
            Micro__SRFlashTest();
        #endif

        #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
            SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_COMPLETED);
            SRTaskSequence__EndSequence(SRTASKSEQUENCE_APPL_SAFETY_THREAD);
        #endif
        #if (MAIN_APPMGR_THREAD_MONITOR == ENABLED)
            SRTaskMonitor__Exit(SRTASKMONITOR_APPLICATION_TASK);
        #endif

        this_timer= FindNextSlot(this_timer, MAIN_APPMGR_THREAD_INTERVAL);
        OS_DelayUntil(this_timer);
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////   Fast AppMgr Thread (15ms)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Method used to launch the FastAppMgr Task.
 */
static void MainFastAppMgrInitialize(void)
{
#if (MAIN_FASTAPPMGR_THREAD_MONITOR == ENABLED)
    SRTaskMonitor__EnableMonitor(SRTASKMONITOR_FASTAPPMGR_TASK);
#endif
    OS_CREATETASK(&MainFastAppMgrTCB, "FastAppMgr Task", MainFastAppMgrThread,  MAIN_FASTAPPMGR_THREAD_PRIORITY, MainFastAppMgrTaskStartStk);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief FastAppMgr Task:  Handles processes that don't need to be in synch with ZeroCross.
 */
static void MainFastAppMgrThread(void)
{
    OS_TIME this_timer;
    ExecuteTasks(SRMain_FastAppMgr_Initialize);

    this_timer = OS_GetTime();
    while(1)
    {
        TimingAnalysis__SampleTask(FASTAPPMGR_TASK);
        #if (MAIN_FASTAPPMGR_THREAD_MONITOR == ENABLED)
            SRTaskMonitor__Enter(SRTASKMONITOR_FASTAPPMGR_TASK);
        #endif

        ExecuteTasks(SRMain_FastAppMgr_Handler);

        #if (MAIN_FASTAPPMGR_THREAD_MONITOR == ENABLED)
            SRTaskMonitor__Exit(SRTASKMONITOR_FASTAPPMGR_TASK);
        #endif

        #if (MAIN_FASTAPPMGR_THREAD_INTERVAL>0)
            this_timer= FindNextSlot(this_timer, MAIN_FASTAPPMGR_THREAD_INTERVAL);
            OS_DelayUntil(this_timer);
        #endif
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////   Asynchronous Thread (5ms)
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Method used to launch the Asynch Task.
 */
static void MainAsynchInitialize(void)
{
#if (MAIN_ASYNCH_THREAD_MONITOR == ENABLED)
    SRTaskMonitor__EnableMonitor(SRTASKMONITOR_ASYNCH_TASK);
#endif
    OS_CREATETASK(&MainProgrammingAsynchTCB, "Asynch Task", MainAsynchThread,  MAIN_ASYNCH_THREAD_PRIORITY, MainProgrammingAsynchTaskStartStk);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Asynch Task:  Handles processes that don't need to be in synch with ZeroCross.
 */
static void MainAsynchThread(void)
{
    OS_TIME this_timer;
    ExecuteTasks(SRMain_Asynch_Initialize);

    this_timer = OS_GetTime();
    while(1)
    {
        TimingAnalysis__SampleTask(ASYNCH_TASK);
        #if (MAIN_ASYNCH_THREAD_MONITOR == ENABLED)
            SRTaskMonitor__Enter(SRTASKMONITOR_ASYNCH_TASK);
        #endif

        ExecuteTasks(SRMain_Asynch_Handler);

        #if (MAIN_ASYNCH_THREAD_MONITOR == ENABLED)
            SRTaskMonitor__Exit(SRTASKMONITOR_ASYNCH_TASK);
        #endif

        #if (MAIN_ASYNCH_THREAD_INTERVAL>0)
            this_timer= FindNextSlot(this_timer, MAIN_ASYNCH_THREAD_INTERVAL);
            OS_DelayUntil(this_timer);
        #endif
    }
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////   Programming Thread
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method launches the Programming Task.
 */
static void MainProgrammingInitialize(void)
{
#if (MAIN_PROGRAMMING_THREAD_MONITOR == ENABLED)
    SRTaskMonitor__EnableMonitor(SRTASKMONITOR_PROGRAMMING_TASK);
#endif
    OS_CREATETASK(&MainProgrammingAsynchTCB, "Programming Task", MainProgrammingThread,  MAIN_PROGRAMMING_THREAD_PRIORITY, MainProgrammingAsynchTaskStartStk);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Programming Task: Used in the process to update Settingfile in the board.
 */
static void MainProgrammingThread(void)
{
    OS_TIME this_timer;

    ExecuteTasks(SRMain_Programming_Initialize);
    this_timer = OS_GetTime();
    while(1)
    {
        #if (MAIN_PROGRAMMING_THREAD_MONITOR == ENABLED)
            SRTaskMonitor__Enter(SRTASKMONITOR_PROGRAMMING_TASK);
        #endif

        ExecuteTasks(SRMain_Programming_Handler);

        #if (SETTINGFILE_SECTION_DOWNLOAD_FEATURE == ENABLED)
            if (SFUpdater__Handler() == TRUE)
            {
                SRMicro__ForceReset(MICRO_RESET_MODE_COLD);
            }
        #endif

        #if (MAIN_PROGRAMMING_THREAD_MONITOR == ENABLED)
            SRTaskMonitor__Exit(SRTASKMONITOR_PROGRAMMING_TASK);
        #endif

        #if (MAIN_PROGRAMMING_THREAD_INTERVAL > 0)
            this_timer = FindNextSlot(this_timer, MAIN_PROGRAMMING_THREAD_INTERVAL);
            OS_DelayUntil(this_timer);
        #endif
    }
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
////////   Communication Thread
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Method use to launch the communication task
 */
static void MainCommunicationInitialize(void)
{
    #if (MAIN_COMMUNICATION_THREAD_MONITOR == ENABLED)
        SRTaskMonitor__EnableMonitor(SRTASKMONITOR_COMMUNICATION_TASK);
    #endif
    OS_CREATETASK(&MainCommunicationTCB, "Communication Task", MainCommunicationThread,  MAIN_COMMUNICATION_THREAD_PRIORITY, MainCommunicationTaskStartStk);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Communication task, controls all the communication processes, including Reveal, Win and APIs
 */
static void MainCommunicationThread(void)
{
    OS_TIME this_timer;
    uint8 divider;

    divider = 0;
    Win__Initialize();
    Reveal__Initialize();

    ExecuteTasks(SRMain_Communication_Initialize);

    Main_Communication_Enabled = TRUE;

    this_timer = OS_GetTime();
    while(1)
    {
        TimingAnalysis__SampleTask(COMMUNICATION_TASK);
        #if (MAIN_COMMUNICATION_THREAD_MONITOR == ENABLED)
            SRTaskMonitor__Enter(SRTASKMONITOR_COMMUNICATION_TASK);
        #endif
        Reveal__Handler();

        ExecuteTasks(SRMain_Communication_EveryRun);

        divider++;

        switch (divider)
        {
            case 1:
                ExecuteTasks(SRMain_Communication_Run1);
                break;
            case 2:
                ExecuteTasks(SRMain_Communication_Run2);
                break;
            case 3:
                ExecuteTasks(SRMain_Communication_Run3);
                break;
            case 4:
                ExecuteTasks(SRMain_Communication_Run4);
                break;
            default:
            case 5:
                ExecuteTasks(SRMain_Communication_Run5);
                divider=0;
                break;
        }

        #if (MAIN_COMMUNICATION_THREAD_MONITOR == ENABLED)
            SRTaskMonitor__Exit(SRTASKMONITOR_COMMUNICATION_TASK);
        #endif

        #if (MAIN_COMMUNICATION_THREAD_INTERVAL > 0)
            this_timer = FindNextSlot(this_timer, MAIN_COMMUNICATION_THREAD_INTERVAL);
            OS_DelayUntil(this_timer);
        #endif
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
*  @brief      Execute the list of tasks pointed to by task_list.
*  @details    This code is approximately a 33% improvement in execution time and a 44%
*              improvement in code size over a version that uses an index into the task_list
*              array.  Former code shown here.
*              <pre>
*                  void ExecuteTasks(TASK_TYPE task_list[])
*                  {
*                      unsigned char task;
*
*                      for (task=0; task_list[task] != NULL_TASK; task++)
*                      {
*                          task_list[task]();
*                      }
*                  }
*              </pre>
*
*  @param      task_list = Null-terminated list of tasks to execute.
*                          Each task must have the prototype: void Task(void)
*/
static void ExecuteTasks(FUNCTION_TYPE * task_list)
{
   while (*task_list != NULL_TASK)
   {
       (*task_list)();
       task_list++;
   }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *
 * @param last_exec
 * @param slot_time
 * @return
 */
static OS_TIME FindNextSlot(OS_TIME last_exec, OS_TIME slot_time)
{
    OS_TIME new_timer;
    OS_TIME timestamp;

    new_timer = last_exec + slot_time;
    timestamp = OS_GetTime();

    while ((timestamp - new_timer) >= 0)
    {
        new_timer += slot_time;
    }

    return new_timer;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  CheckAllocationOfAllStacks: Stack allocation check: this function shall verify if
 *                              - All task stacks are allocated in a stack slot, surrounded by stack signatures
 *                              - System and Interrput stacks are defined in the proper stack space defined by
 *                                  STACK_TEST_START_ADDRESS and STACK_TEST_END_ADDRESS
 *                              - System and Interrupt stacks are not contiguous
 *
 * These explicit suppress messages are required since constants and pointers are used to perform correct stack placement check.
 *  Warning[506]: Constant value Boolean [MISRA 2012 Rule 2.1, required]
 *  Note[923]: cast from pointer to unsigned long [MISRA 2012 Rule 11.6, required]
 *  Note[9033]: Impermissible cast of composite expression (different essential type categories) [MISRA 2012 Rule 10.8, required]
 *
 */
static void CheckAllocationOfAllStacks(void)
{
    BOOL_TYPE is_error = FALSE;
    uint32 cumulative_task_slot_bitmap = 0U;
    sint32 delta;

    // Check if all task stacks are allocated and do not overlap each other
    is_error = (IsTaskStackInOneFreeSlot(&cumulative_task_slot_bitmap, MainTaskStartStk, MAIN_THREAD_STACK_SIZE) == FALSE) ? TRUE : is_error;
    is_error = (IsTaskStackInOneFreeSlot(&cumulative_task_slot_bitmap, MainCommunicationTaskStartStk, MAIN_COMMUNICATION_THREAD_STACK_SIZE) == FALSE) ? TRUE : is_error;
    is_error = (IsTaskStackInOneFreeSlot(&cumulative_task_slot_bitmap, MainAppMgrTaskStartStk, MAIN_APPMGR_THREAD_STACK_SIZE) == FALSE) ? TRUE : is_error;
    is_error = (IsTaskStackInOneFreeSlot(&cumulative_task_slot_bitmap, MainProgrammingAsynchTaskStartStk, MAIN_PROGRAMMING_ASYNCH_THREAD_STACK_SIZE) == FALSE) ? TRUE : is_error;
    is_error = (IsTaskStackInOneFreeSlot(&cumulative_task_slot_bitmap, MainFastAppMgrTaskStartStk, MAIN_FASTAPPMGR_THREAD_STACK_SIZE) == FALSE) ? TRUE : is_error;

    // Interrupt stack area cannot be outside the Stack Test area or inside Task Stack area
    if(((uint32) INTERRUPT_STACK_START_ADDRESS < (uint32) STACK_TEST_START_ADDRESS) ||                //lint !e923
        ((uint32) INTERRUPT_STACK_END_ADDRESS > (uint32) STACK_TEST_END_ADDRESS))                     //lint !e506, !e923
    {
        is_error = TRUE;
    }

    // System stack area cannot be outside the Stack Test area
    if(((uint32) SYSTEM_STACK_START_ADDRESS < (uint32) STACK_TEST_START_ADDRESS ) ||                  //lint !e923
        ((uint32) SYSTEM_STACK_END_ADDRESS > (uint32) STACK_TEST_END_ADDRESS))                        //lint !e506, !e923
    {
        is_error = TRUE;
    }

    // System stack and Interrupt shall not be contiguous
    // Check if absolute distance between Interrupt stack and System stack is less than 4 bytes
    delta = (sint32) ((uint32) INTERRUPT_STACK_END_ADDRESS - (uint32) SYSTEM_STACK_START_ADDRESS);  //lint !e923, !e9033
    if(delta < 0)
    {
        delta = -delta;
    }
    if(delta < 4)
    {
        is_error = TRUE;
    }

    // Check absolute distance between System stack and Interrupt stack is less than 4 bytes
    delta = (sint32) ((uint32) SYSTEM_STACK_END_ADDRESS - (uint32) INTERRUPT_STACK_START_ADDRESS);  //lint !e923, !e9033
    if(delta < 0)
    {
        delta = -delta;
    }
    if(delta < 4)
    {
        is_error = TRUE;
    }

    if(is_error)
    {
        SRException__Queue(SREXCEPTION_STACK_OVERFLOW, 0, 2);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  IsTaskStackInOneFreeSlot -  Find if a task stack is placed between two consecutive task stack signatures (stack slot) and
 *                              that stack slot has not been already used by other task stacks.
 *
 * This explicit suppress is required since pointers are required to find the stack area placement:
 * Suppress Note[946]: Relational or subtract operator applied to pointers [MISRA 2012 Rule 18.2, required], [MISRA 2012 Rule 18.3, required]
 *
 * @param slot_bitmap_ptr   Pointer to slot bitmap to check if task slot is free and set it for the next call
 * @param stack_ptr		    Stack address
 * @param size_words        Stack size in words
 * @return 				    TRUE if task is found in an unallocated slot, FALSE if not found or found in an already allocated slot
 */
static BOOL_TYPE IsTaskStackInOneFreeSlot(uint32 *slot_bitmap_ptr,uint32 *stack_ptr, uint32 size_words)
{
    BOOL_TYPE found = FALSE;

    if((stack_ptr > &Stack_Signature_Before_Threads) &&                 //lint !e946 
        ((stack_ptr + size_words - 1U) < &Stack_Signature_After_Threads))     //lint !e946
    {
        uint32 *stack_end_ptr = stack_ptr + size_words;

        for (uint8 index = 0U; index < (ELEMENTS_IN_ARRAY(SIGNATURE_PTR_LIST) - 1U); index++)
        {
            if((stack_ptr == (SIGNATURE_PTR_LIST[index] + 1U)) &&       // stack_ptr address shall be the next (uint32) to current (index) SIGNATURE_PTR_LIST item
                (stack_end_ptr == SIGNATURE_PTR_LIST[index+1U]))        // stack_end_ptr shall also match the next (index+1) SIGNATURE_PTR_LIST item
            {
                uint32 found_slot = (uint32) 1U << index;
                if((*slot_bitmap_ptr & found_slot) == 0U)
                {
                    // Update slot bitmap value with new slot found
                    *slot_bitmap_ptr |= found_slot;
                    found = TRUE;
                }
                break;
            }
        }
    }

    return(found);
}


#if(FAST_HANDLER_FEATURE == ENABLED)

void MainFastHandlerInitialize(void)
{
    SRMain_FastHandler_Timer.Timer = FAST_HANDLER_TIMER;
    SRMain_FastHandler_Timer.Channel = FAST_HANDLER_TIMER_CHANNEL;
    SRMain_FastHandler_Timer.Mode = TIMER_MODE_OUTPUT_COMPARE;
    SRMain_FastHandler_Timer.Port = PORT_NULL;
    SRMain_FastHandler_Timer.Pin = 0;
    SRMain_FastHandler_Timer.Pin_ReMap = 0;
    SRMain_FastHandler_Timer.Polarity = TIMER_POLARITY_ACTIVE_HIGH;

    if(Timer__Config(&SRMain_FastHandler_Timer) == PASS)
    {
        Timer__OutputCompareStart(&SRMain_FastHandler_Timer, TIMER_ONCOMPARE_INTERRUPT, FAST_HANDLER_TIMING_USEC);
    }
}

#endif
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Debug function used to debug interrupts that aren't handled.
 */
void Main__DoNothing1(void)
{
    SYSTEM_RESET();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Debug function used to debug interrupts that aren't handled.
 */
void Main__DoNothing2(void)
{
    SYSTEM_RESET();
}

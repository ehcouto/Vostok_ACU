/**
 *  @file       
 *
 *  @brief      Implementation of the BackgroundThreads feature
 *
 *  @details    This module creates a pool of Background threads depending by the items present in the
 *              BACKGROUNDTHREADS_STACK_LIST configuration, that contain a list of stack size for each thread.
 *              BACKGROUNDTHREADS_MAX_STACK_SIZE macro shall be defined to be at least equal to the sum of all stacks
 *              listed in BACKGROUNDTHREADS_STACK_LIST.
 *
 *              Enable this module defining BACKGROUNDTHREADS_FEATURE as ENABLED in SystemConfig_prm.h
 *
 *              ** If BACKGROUNDTHREADS_MAX_STACK_SIZE cannot fit all thread stacks defined, no Background threads
 *              ** will be created.
 *
 *              ** Stack RAM occupation will be incremented by (number of threads + 1)*4 bytes to add
 *              ** stack check keywords for each thread and at the end of the entire area.
 *
 *              -  Any module can request to run a Background function using BackgroundThreads__Run(&FunctionName, minStack)
 *              -  Any module can also request to reserve a Thread to run a specific Background function
 *                 using BackgroundThreads__ReserveThread(&FunctionName, minStack) and first free thread
 *                 with the minimum stack size higher than the minStack value.
 *                 Each function can be reserved on one thread only.
 *              -  If a module wants to free a reserved Thread it can call BackgroundThreads__ReleaseThread(&FunctionName)
 *              -  BackgroundThreads__IsRunning(&FunctionName) will return if a function is running
 *              -  BackgroundThreads__GetElapsedTime(&FunctionName) will return time since it started, 0 otherwise
 *
 *              FunctionName shall be defined as: void FunctionName(void)
 *
 *  @copyright  Copyright 2021 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"
#include "BackgroundThreads.h"
#if (BACKGROUNDTHREADS_FEATURE == ENABLED)
    #include "BackgroundThreads_prv.h"
    #include "SRException.h"
    #include "RTOS.h"
    #include <string.h>
#endif

#if (BACKGROUNDTHREADS_FEATURE == ENABLED)
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

    #ifndef BACKGROUNDTHREADS_TIME_SLICE
        #define BACKGROUNDTHREADS_TIME_SLICE           20
    #endif

    #if((BACKGROUNDTHREADS_TIME_SLICE < 1) || (BACKGROUNDTHREADS_TIME_SLICE > 255))
        #error "Wrong value for BACKGROUNDTHREADS_TIME_SLICE: it must be in 1-255 range"
    #endif

    #if(OS_TRACKNAME != 0)
        #define         BACKGROUNDTHREADS_NAME         "Background Thread",
    #else
        #define         BACKGROUNDTHREADS_NAME
    #endif

    #define     BACKGROUNDTHREADS_THREADS_PRIO         10
    #define     BACKGROUNDTHREADS_EVENT                1
    #define     BACKGROUNDTHREADS_STACK_KEYWORD        0xFACECAFE
    #define     NUM_BG_THREADS      (ELEMENTS_IN_ARRAY(BACKGROUNDTHREADS_THREADS_STACKS))
    #define     BACKGROUNDTHREADS_SIZE_ELEMENTS        \
                (((BACKGROUNDTHREADS_MAX_STACK_SIZE + sizeof(uint32) - 1)/sizeof(uint32))+ NUM_BG_THREADS + 1)

    typedef struct BACKGROUNDTHREADS_DATA_STRUCT
    {
        void *Assigned_Func;
        void *Running_Func;
        uint32 Timestamp;
        uint16 Stack_Use;
    } BACKGROUNDTHREADS_DATA_TYPE;

    static uint16 const BACKGROUNDTHREADS_THREADS_STACKS[] = BACKGROUNDTHREADS_STACK_LIST;
    // Thread control blocks - contains embOS thread information for all Background threads
    static OS_TASK BackgroundThreads_Tcb[NUM_BG_THREADS];
    static BACKGROUNDTHREADS_DATA_TYPE BackgroundThreads_Data[NUM_BG_THREADS];
    static uint16 BackgroundThreads_Stack_Allocated;
    #pragma location="TASKS_STACK_BACKGROUND_THREAD"
    OS_STACKPTR uint32 BackgroundThreads_Stack_Area[BACKGROUNDTHREADS_SIZE_ELEMENTS];

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
    static void BackgroundThreadsExec(void *data_ptr);
    static uint8 GetFreeBackgroundIndex(void *function_ptr, uint16 stack_required);
#endif

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It initializes the BackgroundThread module and its variables
 *  @details    Initialize method checks if BackgroundThreads_Stack_Area can contain all stacks defined in
 *              BACKGROUNDTHREADS_STACK_LIST macro before creating all Background threads.
 *              Checks if embOS supports Round Robin feature to perform Background threads switch;
 *              if not only one thread will be allowed to be defined.
 *
 *              If one of these checks fail no threads will be created: Run and ReserveThread methods will return FAIL.
 *
 */
void BackgroundThreads__Initialize(void)
{
    #if (BACKGROUNDTHREADS_FEATURE == ENABLED)
        BACKGROUNDTHREADS_DATA_TYPE *data_ptr;
        uint16 stack_offset;
        uint8 index;

        memset(BackgroundThreads_Data, 0, sizeof(BackgroundThreads_Data));
        memset(BackgroundThreads_Tcb, 0, sizeof(BackgroundThreads_Tcb));
        BackgroundThreads_Stack_Allocated = 1;
        // Check if reserved stack area fits the thread configuration
        for(index = 0; index < NUM_BG_THREADS; index++)
        {
            BackgroundThreads_Stack_Allocated += BACKGROUNDTHREADS_THREADS_STACKS[index]/sizeof(uint32)+1;
        }
        // Allow Background thread if stack fits the space requested and
        // OS supports Round Robin feature or there is only one thread defined
        if(((OS_RR_SUPPORTED != 0) || (NUM_BG_THREADS == 1)) &&
            (BackgroundThreads_Stack_Allocated <= ELEMENTS_IN_ARRAY(BackgroundThreads_Stack_Area)))
        {
            stack_offset = 0;
            for(index = 0; index < NUM_BG_THREADS; index++)
            {
                data_ptr = &BackgroundThreads_Data[index];
                BackgroundThreads_Stack_Area[stack_offset] = BACKGROUNDTHREADS_STACK_KEYWORD;
                stack_offset++;
                OS_CreateTaskEx(&BackgroundThreads_Tcb[index], BACKGROUNDTHREADS_NAME BACKGROUNDTHREADS_THREADS_PRIO,
                    BackgroundThreadsExec, &BackgroundThreads_Stack_Area[stack_offset],
                    BACKGROUNDTHREADS_THREADS_STACKS[index], BACKGROUNDTHREADS_TIME_SLICE, (void *) data_ptr);
                stack_offset += BACKGROUNDTHREADS_THREADS_STACKS[index]/sizeof(uint32);
            }
            BackgroundThreads_Stack_Area[ELEMENTS_IN_ARRAY(BackgroundThreads_Stack_Area)-1] = BACKGROUNDTHREADS_STACK_KEYWORD;
        }
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Periodic handler to perform thread stack check
 *
 */
void BackgroundThreads__Handler(void)
{
    #if (BACKGROUNDTHREADS_FEATURE == ENABLED)
        if(BackgroundThreads_Stack_Allocated <= ELEMENTS_IN_ARRAY(BackgroundThreads_Stack_Area))
        {
            uint16 stack_offset = 0;
            uint8 index;
            for(index = 0; index < NUM_BG_THREADS; index++)
            {
                if(BackgroundThreads_Stack_Area[stack_offset] != BACKGROUNDTHREADS_STACK_KEYWORD)
                {
                    SRException__Queue(SREXCEPTION_STACK_OVERFLOW, 1, index);
                }
                stack_offset += BACKGROUNDTHREADS_THREADS_STACKS[index]/sizeof(uint32) + 1;
                BackgroundThreads_Data[index].Stack_Use = (uint16) OS_GetStackUsed(&BackgroundThreads_Tcb[index]);
            }
            if(BackgroundThreads_Stack_Area[ELEMENTS_IN_ARRAY(BackgroundThreads_Stack_Area)-1] != BACKGROUNDTHREADS_STACK_KEYWORD)
            {
                SRException__Queue(SREXCEPTION_STACK_OVERFLOW, 1, NUM_BG_THREADS);
            }
        }
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This function will reserve a thread that will run exclusively a specific function
 *
 *  @param function_ptr    - Pointer to the function that will be reserved to a thread
 *  @param stack_required  - Minimum stack size required by the function to run
 *
 *  @return
 *  PASS    if function_ptr has been - or it's already - reserved to a thread
 *  FAIL    if no free threads with enough stack space has been found
 *
 */
PASS_FAIL_TYPE BackgroundThreads__ReserveThread(FUNCTION_TYPE function_ptr, uint16 stack_required)
{
    PASS_FAIL_TYPE result = FAIL;

    #if (BACKGROUNDTHREADS_FEATURE == ENABLED)
        if(BackgroundThreads_Stack_Allocated <= ELEMENTS_IN_ARRAY(BackgroundThreads_Stack_Area))
        {
            uint8 found_index = NUM_BG_THREADS;
            uint8 index;

            for(index = 0; index < NUM_BG_THREADS; index++)
            {
                BACKGROUNDTHREADS_DATA_TYPE *data_ptr = &BackgroundThreads_Data[index];
                if(((void *)function_ptr == data_ptr->Assigned_Func) ||
                    ((void *)function_ptr == data_ptr->Running_Func))
                {
                    // Function already reserved to - or running in - a thread
                    found_index = index;
                    break;
                }
                else if((data_ptr->Assigned_Func == NULL) &&
                    (data_ptr->Running_Func == NULL) &&
                    (stack_required <= BACKGROUNDTHREADS_THREADS_STACKS[index]))
                {
                    if(found_index < NUM_BG_THREADS)
                    {
                        if(BACKGROUNDTHREADS_THREADS_STACKS[index] < BACKGROUNDTHREADS_THREADS_STACKS[found_index])
                        {
                            found_index = index;
                        }
                    }
                    else
                    {
                        found_index = index;
                    }
                }
            }

            if(found_index < NUM_BG_THREADS)
            {
                BackgroundThreads_Data[found_index].Assigned_Func = (void *) function_ptr;
                result = PASS;
            }
        }
    #endif

    return(result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This function will release thread that has been reserved to a thread
 *
 *  @param function_ptr    - Pointer to the function to remove that will be assigned to a thread
 *
 */
void BackgroundThreads__ReleaseThread(FUNCTION_TYPE function_ptr)
{
    #if (BACKGROUNDTHREADS_FEATURE == ENABLED)
        if(BackgroundThreads_Stack_Allocated <= ELEMENTS_IN_ARRAY(BackgroundThreads_Stack_Area))
        {
            uint8 index;

            for(index = 0; index < NUM_BG_THREADS; index++)
            {
                if((void *)function_ptr == BackgroundThreads_Data[index].Assigned_Func)
                {
                    BackgroundThreads_Data[index].Assigned_Func = NULL;
                    break;
                }
            }
        }
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This function will start a function running in a thread
 *
 *  @param function_ptr    - Pointer to the function to run
 *  @param stack_required  - Minimum stack size required by the function to run
 *                            Not used if function has a reserved thread
 *
 *  @return
 *  PASS    if function is able to start its execution in a free or reserved thread
 *  FAIL    if no free threads are available to run the function
 *
 */
PASS_FAIL_TYPE BackgroundThreads__Run(FUNCTION_TYPE function_ptr, uint16 stack_required)
{
    PASS_FAIL_TYPE result = FAIL;

    #if (BACKGROUNDTHREADS_FEATURE == ENABLED)
        if(BackgroundThreads_Stack_Allocated <= ELEMENTS_IN_ARRAY(BackgroundThreads_Stack_Area))
        {
            uint8 index = GetFreeBackgroundIndex((void *) function_ptr, stack_required);
            if(index < NUM_BG_THREADS)
            {
                BackgroundThreads_Data[index].Running_Func = (void *)function_ptr;
                result = PASS;
                OS_SignalEvent(BACKGROUNDTHREADS_EVENT, &BackgroundThreads_Tcb[index]);
            }
        }
    #endif

    return(result);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This function will return if a function is running
 *
 *  @param function_ptr    - Pointer to the function to check
 *
 *  @return
 *  TRUE    if function is running
 *  FALSE   if function is not running - not requested or execution has completed
 *
 */
BOOL_TYPE BackgroundThreads__IsRunning(FUNCTION_TYPE function_ptr)
{
    BOOL_TYPE is_running = FALSE;

    #if (BACKGROUNDTHREADS_FEATURE == ENABLED)
        uint8 index;
        for(index = 0; index < NUM_BG_THREADS; index++)
        {
            if(BackgroundThreads_Data[index].Running_Func == (void *)function_ptr)
            {
                // Running function found, exit the loop
                is_running = TRUE;
                break;
            }
        }
    #endif

    return(is_running);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This function will return time (in ms) since a running function started
 *
 *  @param function_ptr    - Pointer to the function to check
 *
 *  @return
 *  time (in ms) elapsed from function started, if it's running
 *  0 if function is not running - not requested or execution has completed
 *
 *
 */
uint32 BackgroundThreads__GetElapsedTime(FUNCTION_TYPE function_ptr)
{
    uint32 elapsed = 0;

    #if (BACKGROUNDTHREADS_FEATURE == ENABLED)
        uint8 index;
        for(index = 0; index < NUM_BG_THREADS; index++)
        {
            if(BackgroundThreads_Data[index].Running_Func == (void *)function_ptr)
            {
                // Running function found, exit the loop
                elapsed = OS_GetTime32() - BackgroundThreads_Data[index].Timestamp;
                break;
            }
        }
    #endif

    return(elapsed);
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#if (BACKGROUNDTHREADS_FEATURE == ENABLED)

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Background thread function that waits and run requests
 *
 *  @param data_ptr    - Pointer to the function data to handle
 *
 */
void BackgroundThreadsExec(void *data_ptr)
{
    OS_TASK_EVENT my_event;
    while(1)
    {
        my_event = OS_WaitEvent(BACKGROUNDTHREADS_EVENT);
        if((my_event == BACKGROUNDTHREADS_EVENT) &&
            (data_ptr != NULL) &&
            (((BACKGROUNDTHREADS_DATA_TYPE *)data_ptr)->Running_Func != NULL))
        {
            ((BACKGROUNDTHREADS_DATA_TYPE *)data_ptr)->Timestamp = OS_GetTime32();
            ((FUNCTION_TYPE) ((BACKGROUNDTHREADS_DATA_TYPE *)data_ptr)->Running_Func)();
            ((BACKGROUNDTHREADS_DATA_TYPE *)data_ptr)->Running_Func = NULL;
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      This function will return a free thread to run a requested function
 *
 *  @param function_ptr    - Pointer to the function to run
 *  @param stack_required  - Minimum stack size required by the function to run
 *                            Not used if function has a reserved thread
 *
 *  @return
 *  found_index if a free or already reserved thread has been found
 *  NUM_BG_THREADS (out of index) if no free threads have been found
 *
 */
uint8 GetFreeBackgroundIndex(void *function_ptr, uint16 stack_required)
{
    uint8 found_index = NUM_BG_THREADS;
    uint8 index;

    for(index = 0; index < NUM_BG_THREADS; index++)
    {
        BACKGROUNDTHREADS_DATA_TYPE *data_ptr = &BackgroundThreads_Data[index];

        if(data_ptr->Assigned_Func == function_ptr)
        {
            // Assigned thread: exit loop; if not running, save index to return
            if(data_ptr->Running_Func == NULL)
            {
                found_index = index;
            }
            else
            {
                found_index = NUM_BG_THREADS;
            }
            break;
        }
        else if((data_ptr->Assigned_Func == NULL) &&
            (data_ptr->Running_Func == NULL) &&
            (stack_required <= BACKGROUNDTHREADS_THREADS_STACKS[index]) &&
            (found_index >= NUM_BG_THREADS))
        {
            // First free thread found: don't exit to find if there is an assigned thread
            found_index = index;
        }
    }

    return(found_index);
}

#endif

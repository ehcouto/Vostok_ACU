/**
 *  @file       SRTaskMonitor.c
 *  @defgroup   CLASS_B
 *  @brief      Periodic task execution monitor module
 *
 *  @details    This module monitor that tasks defined are executed with expected timing
 *              That expected timing is defined with a minimum-maximum range, and an startup time.
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SRSystemConfig.h"
#include "SRTaskMonitor.h"
#include "SRTaskMonitor_prv.h"
#include "SRPowerMgr.h"
#include "SRException.h"
#include "SRData.h"
#include "string.h"
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    #include "SRTaskSequence.h"
#endif

//-------------------------------------- Default defines ---------------------------------------------------------------
#ifdef SRTASKMONITOR_SKIP_RESET
    #if (SRTASKMONITOR_SKIP_RESET != DISABLED)
        SW05_WARNING(3, "SRTASKMONITOR_SKIP_RESET is Enabled", SW05_CLASSB)
    #endif
#endif

#ifndef SRTASKMONITOR_SKIP_RESET
#define SRTASKMONITOR_SKIP_RESET            DISABLED
#endif

#ifndef SRTASKMONITOR_ALLOW_NO_TASK
#define SRTASKMONITOR_ALLOW_NO_TASK         DISABLED
#endif

#ifndef SRTASKMONITOR_STATS_FEATURE
#define SRTASKMONITOR_STATS_FEATURE         DISABLED
#endif

#ifndef SRTASKMONITOR_SUSPEND_TIME
#define SRTASKMONITOR_SUSPEND_TIME          0
#else

#ifndef SRTASKMONITOR_SUSPEND_MAX_ATTEMPTS
#define SRTASKMONITOR_SUSPEND_MAX_ATTEMPTS  1
#endif
#endif

#if (SRTASKMONITOR_SUSPEND_MAX_ATTEMPTS == 0)
#error "SRTASKMONITOR_SUSPEND_MAX_ATTEMPTS cannot be set to 0"
#endif

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------
#if (SRTASKMONITOR_SUSPEND_TIME > 0)
    #if (SRTASKMONITOR_SUSPEND_TIME < 255)
        typedef uint8 SUSPEND_TIME_DATA_TYPE;

        #define SRDATA__SUSPENDCHECK(var, n_var)                SRData__CheckByte((var), (n_var))
        #define SRDATA__SUSPENDUPDATE(var, n_var, val)          SRData__UpdateByte((var), (n_var), (val))

    #elif (SRTASKMONITOR_SUSPEND_TIME < 65535)
        typedef uint16 SUSPEND_TIME_DATA_TYPE;

        #define SRDATA__SUSPENDCHECK(var, n_var)                SRData__CheckShort((var), (n_var))
        #define SRDATA__SUSPENDUPDATE(var, n_var, val)          SRData__UpdateShort((var), (n_var), (val))
    #else
        #error : "SRTASKMONITOR_SUSPEND_TIME can not be more then 0xFFFF"
    #endif
#endif

#if (SRTASKMONITOR_STATS_FEATURE == ENABLED)
    SRTASKMONITOR_STATS_TYPE SRTaskMonitor_Stats[SRTASKMONITOR_NUM_TASKS];
    #if (SRTASKMONITOR_SUSPEND_TIME > 0)
        SUSPEND_TIME_DATA_TYPE SRTaskMonitor_MinSuspendTime;
        SUSPEND_TIME_DATA_TYPE SRTaskMonitor_MaxSuspendTime;
    #endif
#endif

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------


#if (SRTASKMONITOR_SUSPEND_TIME > 0)
#define SRTASKMONITOR__HANDLER_CHECK()      ((SRTaskMonitor_Error == FALSE) && (SRTaskMonitor_Suspend_Time == 0))
#else
#define SRTASKMONITOR__HANDLER_CHECK()      (SRTaskMonitor_Error == FALSE)
#endif


typedef union
{
    uint16  rawdata;
    struct
    {
        uint16  counter     : 12;
        uint16  enter       : 1;
        uint16  suspended   : 1;
        uint16  first_run   : 1;
        uint16  enabled     : 1;
    } data;
} SRTASKMONITOR_TASK_STATUS_TYPE;

static uint8 SRTaskMonitor_Error;
static SRTASKMONITOR_TASK_STATUS_TYPE SRTaskMonitor_Status[SRTASKMONITOR_NUM_TASKS];

#if (SRTASKMONITOR_SUSPEND_TIME > 0)
    static SUSPEND_TIME_DATA_TYPE SRTaskMonitor_Suspend_Time;

    #if (SRTASKMONITOR_SUSPEND_MAX_ATTEMPTS > 0)
        static uint8 SRTaskMonitor_Suspend_Attempts;
    #endif
#endif

static uint8 N_SRTaskMonitor_Error;
static SRTASKMONITOR_TASK_STATUS_TYPE N_SRTaskMonitor_Status[SRTASKMONITOR_NUM_TASKS];

#if (SRTASKMONITOR_SUSPEND_TIME > 0)
    static SUSPEND_TIME_DATA_TYPE N_SRTaskMonitor_Suspend_Time;

    #if (SRTASKMONITOR_SUSPEND_MAX_ATTEMPTS > 0)
        static uint8 N_SRTaskMonitor_Suspend_Attempts;
    #endif
#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void SetSRTaskError(SREXCEPTION_EVENT_TYPE message, uint8 data);
static void CheckSRTaskError(void);
static SRTASKMONITOR_TASK_COUNTER_REF const *GetTaskCounterRef(uint8 task_id);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It initializes the module SRTaskMonitor and its variables
 *
 */
void SRTaskMonitor__Initialize(void)
{
    uint8 index;

    SRData__UpdateByte(&SRTaskMonitor_Error, &N_SRTaskMonitor_Error, (uint8) FALSE);

    for (index = 0; index < SRTASKMONITOR_NUM_TASKS; index++)
    {
        SRData__UpdateShort(&SRTaskMonitor_Status[index].rawdata, &N_SRTaskMonitor_Status[index].rawdata, 0);
    }

    #if (SRTASKMONITOR_SUSPEND_TIME > 0)
        SRDATA__SUSPENDUPDATE(&SRTaskMonitor_Suspend_Time, &N_SRTaskMonitor_Suspend_Time, 0);

        #if (SRTASKMONITOR_SUSPEND_MAX_ATTEMPTS > 0)
            SRData__UpdateByte(&SRTaskMonitor_Suspend_Attempts, &N_SRTaskMonitor_Suspend_Attempts, 0);
        #endif
    #endif

    #if (SRTASKMONITOR_STATS_FEATURE == ENABLED)
        memset(SRTaskMonitor_Stats, 0, sizeof(SRTaskMonitor_Stats));
        #if (SRTASKMONITOR_SUSPEND_TIME > 0)
            SRTaskMonitor_MinSuspendTime = SRTASKMONITOR_SUSPEND_TIME;
            SRTaskMonitor_MaxSuspendTime = 0;
        #endif
    #endif

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_INIT_PHASE, SRTASKSEQUENCE_INIT_SRTASKMONITOR);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Function that enables monitoring of a task
 *              This function must be executed only once after a reset, otherwise it will go in error state
 *
 *  @param      task_id - Task to enable
 *
 */
void SRTaskMonitor__EnableMonitor(SRTASKMONITOR_TASKLIST_TYPE task_id)
{
    SRTASKMONITOR_TASK_STATUS_TYPE local_status;
    SRTASKMONITOR_TASK_STATUS_TYPE *ptr_status;
    SRTASKMONITOR_TASK_STATUS_TYPE *ptr_n_status;

    memset(&local_status, 0, sizeof(local_status));

    if (task_id < SRTASKMONITOR_NUM_TASKS)
    {
        CheckSRTaskError();

        if (SRTaskMonitor_Error == FALSE)
        {
            ptr_status = &SRTaskMonitor_Status[task_id];
            ptr_n_status = &N_SRTaskMonitor_Status[task_id];

            if (SRData__CheckShort(&ptr_status->rawdata, &ptr_n_status->rawdata) == SRDATA_OK)
            {
                if (ptr_status->data.enabled == 0)
                {
                    SRTASKMONITOR_TASK_COUNTER_REF const *task_ref_ptr = GetTaskCounterRef(task_id);
                    local_status.data.enabled = 1;
                    local_status.data.first_run = 1;
                    local_status.data.counter = task_ref_ptr->counter_max_init;
                    SRData__UpdateShort(&ptr_status->rawdata, &ptr_n_status->rawdata, local_status.rawdata);

        #if (SRTASKMONITOR_STATS_FEATURE == ENABLED)
                    SRTaskMonitor_Stats[task_id].min_value = task_ref_ptr->counter_max;
        #endif
                }
                else
                {
                    SetSRTaskError(SREXCEPTION_SRTASKMON_ALREADY_ENABLED, task_id);
                }
            }
        }

    }

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      1ms Handler - this function must be executed in ISR
 *
 */
void SRTaskMonitor__Handler(void)
{
    uint8 index;
    SRTASKMONITOR_TASK_STATUS_TYPE local_status;
    SRTASKMONITOR_TASK_STATUS_TYPE *ptr_status;
    SRTASKMONITOR_TASK_STATUS_TYPE *ptr_n_status;
    #if (SRTASKMONITOR_ALLOW_NO_TASK == DISABLED)
        BOOL_TYPE are_tasks_running;

        are_tasks_running = FALSE;
    #endif

    CheckSRTaskError();

    #if (SRTASKMONITOR_SUSPEND_TIME > 0)
        SRDATA__SUSPENDCHECK(&SRTaskMonitor_Suspend_Time, &N_SRTaskMonitor_Suspend_Time);
        if (SRTaskMonitor_Suspend_Time > 0)
        {
            SRDATA__SUSPENDUPDATE(&SRTaskMonitor_Suspend_Time, &N_SRTaskMonitor_Suspend_Time, SRTaskMonitor_Suspend_Time-1);

            if (SRTaskMonitor_Suspend_Time == 0)
            {
                SetSRTaskError(SREXCEPTION_SRTASKMON_SUSPEND_TIMEOUT, 0);
            }
        }

        #if (SRTASKMONITOR_SUSPEND_MAX_ATTEMPTS > 0)
            SRData__CheckByte(&SRTaskMonitor_Suspend_Attempts, &N_SRTaskMonitor_Suspend_Attempts);
        #endif

    #endif

    if (SRTASKMONITOR__HANDLER_CHECK() == TRUE)
    {
        for (index=0; index < SRTASKMONITOR_NUM_TASKS; index++)
        {
            ptr_status = &SRTaskMonitor_Status[index];
            ptr_n_status = &N_SRTaskMonitor_Status[index];

                // Data integrity
            if (SRData__CheckShort(&ptr_status->rawdata, &ptr_n_status->rawdata) == SRDATA_OK)
            {
                // Check only enabled tasks
                if (ptr_status->data.enabled == 1)
                {
                    #if (SRTASKMONITOR_ALLOW_NO_TASK == DISABLED)
                        are_tasks_running = TRUE;
                    #endif

                    // Check if task timeout occurred
                    if (ptr_status->data.counter == 0)
                    {
                        SetSRTaskError(SREXCEPTION_SRTASKMON_TIMEOUT, index);
                    }
                    else
                    {
                        local_status.rawdata = ptr_status->rawdata;
                        local_status.data.counter--;

                        SRData__UpdateShort(&ptr_status->rawdata, &ptr_n_status->rawdata, local_status.rawdata);
                    }
                }
            }
        }

        #if (SRTASKMONITOR_ALLOW_NO_TASK == DISABLED)
            // There must be at least one task running - if not, raise an error
            if (are_tasks_running == FALSE)
            {
                SetSRTaskError(SREXCEPTION_SRTASKMON_NO_TASKS, 0);
            }
        #endif
    }
    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_TICK_HANDLER, SRTASKSEQUENCE_TICK_SRTASKMONITOR);
    #endif

}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Function that marks entering a task
 *              This function must be executed only after Enable or Exit that task, otherwise it will go in error state
 *
 *  @param      task_id - Task to mark as enter
 *
 */
void SRTaskMonitor__Enter(SRTASKMONITOR_TASKLIST_TYPE task_id)
{
    SRTASKMONITOR_TASK_STATUS_TYPE local_status;
    SRTASKMONITOR_TASK_STATUS_TYPE *ptr_status;
    SRTASKMONITOR_TASK_STATUS_TYPE *ptr_n_status;
    #if (SRTASKMONITOR_STATS_FEATURE == ENABLED)
        uint16 elapsed_counter;
    #endif

    if (task_id < SRTASKMONITOR_NUM_TASKS)
    {
        ptr_status = &SRTaskMonitor_Status[task_id];
        ptr_n_status = &N_SRTaskMonitor_Status[task_id];

        CheckSRTaskError();

        if (SRTASKMONITOR__HANDLER_CHECK() == TRUE)
        {
            // Data integrity
            if (SRData__CheckShort(&ptr_status->rawdata, &ptr_n_status->rawdata) == SRDATA_OK)
            {
                SRTASKMONITOR_TASK_COUNTER_REF const *task_ref_ptr = GetTaskCounterRef(task_id);

                // Check if enabled
                if (ptr_status->data.enabled == 1)
                {
                    // Check if not already enter
                    if (ptr_status->data.enter == 0)
                    {
                        // Minimum time check is disabled at first run and after a suspend
                        // (only if suspend time exceeds counter)
                        if ( (ptr_status->data.first_run == 0) && ((ptr_status->data.suspended == 0)) )
                        {
                            // Check minimum time allowed (max - min)
                            if (ptr_status->data.counter > (task_ref_ptr->counter_max - task_ref_ptr->counter_min) )
                            {
                                SetSRTaskError(SREXCEPTION_SRTASKMON_ENTER_OUTOFRANGE, task_id);
                            }
                        }
                    }
                    else
                    {
                        SetSRTaskError(SREXCEPTION_SRTASKMON_ALREADY_ENTER, task_id);
                    }
                }
                else
                {
                    SetSRTaskError(SREXCEPTION_SRTASKMON_ENTER_INACTIVE, task_id);
                }

                if (SRTaskMonitor_Error == FALSE)
                {
                    #if (SRTASKMONITOR_STATS_FEATURE == ENABLED)
                        if ( (ptr_status->data.first_run == 1) && (ptr_status->data.suspended == 0))
                        {
                            SRTaskMonitor_Stats[task_id].start_value = task_ref_ptr->counter_max_init - ptr_status->data.counter;
                        }
                        else
                        {
                            elapsed_counter = task_ref_ptr->counter_max - ptr_status->data.counter;

                            if (elapsed_counter < SRTaskMonitor_Stats[task_id].min_value)
                            {
                                SRTaskMonitor_Stats[task_id].min_value = elapsed_counter;
                            }

                            if (elapsed_counter > SRTaskMonitor_Stats[task_id].max_value)
                            {
                                SRTaskMonitor_Stats[task_id].max_value = elapsed_counter;
                            }
                        }

                    #endif

                    local_status.rawdata = ptr_status->rawdata;
                    local_status.data.enter = 1;

                    // If it was suspended, clear first_run flag before suspend
                    // This allows to relax check twice - when expected period time is
                    // higher than suspend time
                    if (ptr_status->data.first_run == 1)
                    {
                        local_status.data.first_run = 0;
                    }
                    else
                    {
                        local_status.data.suspended = 0;
                    }
                    local_status.data.counter = task_ref_ptr->counter_max;

                    SRData__UpdateShort(&ptr_status->rawdata, &ptr_n_status->rawdata, local_status.rawdata);
                }
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Function that marks exit a task, refreshing its timeout.
 *              This function must be executed only when task has been already in enter status, otherwise it will go in error state
 *
 *  @param      task_id - Task to leave its enter status and refresh timeout
 *
 */
void SRTaskMonitor__Exit(SRTASKMONITOR_TASKLIST_TYPE task_id)
{
    SRTASKMONITOR_TASK_STATUS_TYPE local_status;
    SRTASKMONITOR_TASK_STATUS_TYPE *ptr_status;
    SRTASKMONITOR_TASK_STATUS_TYPE *ptr_n_status;
    #if (SRTASKMONITOR_STATS_FEATURE == ENABLED)
        uint16 elapsed_counter;
    #endif

    if (task_id < SRTASKMONITOR_NUM_TASKS)
    {
        ptr_status = &SRTaskMonitor_Status[task_id];
        ptr_n_status = &N_SRTaskMonitor_Status[task_id];

        CheckSRTaskError();

        if (SRTASKMONITOR__HANDLER_CHECK() == TRUE)
        {

            // Data integrity
            if (SRData__CheckShort(&ptr_status->rawdata, &ptr_n_status->rawdata) == SRDATA_OK)
            {
                // Check if enabled
                if (ptr_status->data.enabled == 1)
                {
                    // Check if has been enter
                    if (ptr_status->data.enter == 0)
                    {
                        SetSRTaskError(SREXCEPTION_SRTASKMON_ALREADY_EXIT, task_id);
                    }

                }
                else
                {
                    SetSRTaskError(SREXCEPTION_SRTASKMON_EXIT_INACTIVE, task_id);
                }

                if (SRTaskMonitor_Error == FALSE)
                {
                    #if (SRTASKMONITOR_STATS_FEATURE == ENABLED)
                        SRTASKMONITOR_TASK_COUNTER_REF const *task_ref_ptr = GetTaskCounterRef(task_id);
                        if (ptr_status->data.first_run == 0)
                        {
                            elapsed_counter = task_ref_ptr->counter_max - ptr_status->data.counter;

                            if (elapsed_counter > SRTaskMonitor_Stats[task_id].max_elapsed)
                            {
                                SRTaskMonitor_Stats[task_id].max_elapsed = elapsed_counter;
                            }
                        }

                    #endif

                    local_status.rawdata = ptr_status->rawdata;
                    local_status.data.enter = 0;

                    SRData__UpdateShort(&ptr_status->rawdata, &ptr_n_status->rawdata, local_status.rawdata);
                }
            }
        }

    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Function that returns Error state
 *
 */
BOOL_TYPE SRTaskMonitor__IsError(void)
{
    return ((BOOL_TYPE) SRTaskMonitor_Error);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Function to request a temporary task monitor suspend.
 *              This function can be used when there are some long interruptions, like NVData writing to flash
 *              Use with care!
 *
 */
void SRTaskMonitor__Suspend(void)
{
    #if (SRTASKMONITOR_SUSPEND_TIME > 0)
        SRDATA__SUSPENDCHECK(&SRTaskMonitor_Suspend_Time, &N_SRTaskMonitor_Suspend_Time);
        #if (SRTASKMONITOR_SUSPEND_MAX_ATTEMPTS > 0)
            SRData__CheckByte(&SRTaskMonitor_Suspend_Attempts, &N_SRTaskMonitor_Suspend_Attempts);
        #endif

        if (SRTaskMonitor_Suspend_Time == 0)
        {
            // Set suspend timeout and increment number of attempts done
            SRDATA__SUSPENDUPDATE(&SRTaskMonitor_Suspend_Time, &N_SRTaskMonitor_Suspend_Time, SRTASKMONITOR_SUSPEND_TIME+1);

            #if (SRTASKMONITOR_SUSPEND_MAX_ATTEMPTS > 0)
                SRData__UpdateByte(&SRTaskMonitor_Suspend_Attempts, &N_SRTaskMonitor_Suspend_Attempts, SRTaskMonitor_Suspend_Attempts + 1);
            #endif
        }
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Function to resume a task monitor suspended.
 *              This function must be called after execution that needed to suspend has been done
 *              If not, task monitor will raise an exception for suspend timeout.
 *
 */
void SRTaskMonitor__Resume(void)
{
    #if (SRTASKMONITOR_SUSPEND_TIME > 0)
        SRTASKMONITOR_TASK_STATUS_TYPE local_status;
        SRTASKMONITOR_TASK_STATUS_TYPE *ptr_status;
        SRTASKMONITOR_TASK_STATUS_TYPE *ptr_n_status;
        uint8 index;
        SUSPEND_TIME_DATA_TYPE elapsed_time;

        #if (SRTASKMONITOR_SUSPEND_MAX_ATTEMPTS > 0)
            SRData__CheckByte(&SRTaskMonitor_Suspend_Attempts, &N_SRTaskMonitor_Suspend_Attempts);
            // When maximum number of attempts is reached, raises an exception
            if (SRTaskMonitor_Suspend_Attempts > SRTASKMONITOR_SUSPEND_MAX_ATTEMPTS)
            {
                SetSRTaskError(SREXCEPTION_SRTASKMON_SUSPEND_OVERRUN, 0);
            }
        #endif

        SRDATA__SUSPENDCHECK(&SRTaskMonitor_Suspend_Time, &N_SRTaskMonitor_Suspend_Time);
        if (SRTaskMonitor_Suspend_Time > 0)
        {
            elapsed_time = (SRTASKMONITOR_SUSPEND_TIME + 1 - SRTaskMonitor_Suspend_Time);

            for (index = 0; index < SRTASKMONITOR_NUM_TASKS; index++)
            {
                ptr_status = &SRTaskMonitor_Status[index];
                ptr_n_status = &N_SRTaskMonitor_Status[index];

                // Data integrity
                if (SRData__CheckShort(&ptr_status->rawdata, &ptr_n_status->rawdata) == SRDATA_OK)
                {
                    local_status.rawdata = ptr_status->rawdata;

                    if (ptr_status->data.enabled == 1)
                    {
                        SRTASKMONITOR_TASK_COUNTER_REF const *task_ref_ptr = GetTaskCounterRef(index);

                        // Relax next task check if suspend elapsed time is higher than minimum remaining time allowed
                        if (ptr_status->data.counter <= (elapsed_time + task_ref_ptr->counter_max - task_ref_ptr->counter_min))
                        {
                            local_status.data.first_run = 1;
                            local_status.data.suspended = 1;
                            local_status.data.counter = task_ref_ptr->counter_max;
                        }
                        else
                        {
                            // Elapsed time during suspend is substracted from task check counter
                            local_status.data.counter -= elapsed_time;
                        }

                        SRData__UpdateShort(&ptr_status->rawdata, &ptr_n_status->rawdata, local_status.rawdata);
                    }

                }
            }

            #if (SRTASKMONITOR_STATS_FEATURE == ENABLED)
                if (SRTaskMonitor_MinSuspendTime > elapsed_time)
                {
                    SRTaskMonitor_MinSuspendTime = elapsed_time;
                }

                if (SRTaskMonitor_MaxSuspendTime < elapsed_time)
                {
                    SRTaskMonitor_MaxSuspendTime = elapsed_time;
                }
            #endif

            SRDATA__SUSPENDUPDATE(&SRTaskMonitor_Suspend_Time, &N_SRTaskMonitor_Suspend_Time, 0);
        }
    #endif
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This API returns the static data usually used for debugging
 *
 * @return
 *  @retval SRTASKMONITOR_STATS_TYPE *
 */
SRTASKMONITOR_STATS_TYPE * SRTaskMonitor__GetStatisticsData(uint8 task_id)
{
    #if (SRTASKMONITOR_STATS_FEATURE == ENABLED)
        if (task_id < SRTASKMONITOR_NUM_TASKS)
        {
            return &SRTaskMonitor_Stats[task_id];
        }
        else
        {
            return NULL;
        }
    #else
        return NULL;
    #endif
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Function that raise and logs an Error state
 *
 *  @param      message - Message to log
 *  @param      data - Data to log - except for NO_TASKS_RUNNING message it's task ID that raises error
 *
 */
static void SetSRTaskError(SREXCEPTION_EVENT_TYPE message, uint8 data)
{
    #if (SRTASKMONITOR_SKIP_RESET == ENABLED)
        SRTASKMONITOR_TASK_STATUS_TYPE local_status;

        SRData__UpdateByte(&SRTaskMonitor_Error, &N_SRTaskMonitor_Error,(uint8) FALSE);

        if (message != SREXCEPTION_SRTASKMON_NO_TASKS)
        {
            local_status.rawdata = SRTaskMonitor_Status[data].rawdata;

            if (message == SREXCEPTION_SRTASKMON_TIMEOUT)
            {
                SRTASKMONITOR_TASK_COUNTER_REF const *task_ref_ptr = GetTaskCounterRef(data);
                local_status.data.counter = task_ref_ptr->counter_max;
            }

            SRData__UpdateShort(&SRTaskMonitor_Status[data].rawdata, &N_SRTaskMonitor_Status[data].rawdata, local_status.rawdata);
        }

        SRException__Queue(SREXCEPTION_PRODUCT_LEVEL_RECOVERABLE, message, data);

    #else
        SRData__UpdateByte(&SRTaskMonitor_Error, &N_SRTaskMonitor_Error,(uint8) TRUE);

        SRException__Queue(message, data, 0);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Function that marks exit a task, refeshing its timeout.
 *              This function must be executed only when task has been already in enter status, otherwise it will go in error state
 *
 *  @param      task_id - Task to leave its enter status and refresh timeout
 *
 */
static void CheckSRTaskError(void)
{
    if (SRData__CheckByte(&SRTaskMonitor_Error, &N_SRTaskMonitor_Error) != SRDATA_OK)
    {
        SRData__UpdateByte(&SRTaskMonitor_Error, &N_SRTaskMonitor_Error, (uint8) TRUE);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Function that provides Task counter references
 *
 *  @return     Pointer to counter reference - depending on SRPowerMgr mode
 *
 */
static SRTASKMONITOR_TASK_COUNTER_REF const *GetTaskCounterRef(uint8 task_id)
{
    SRTASKMONITOR_TASK_COUNTER_REF const *ptr_ref = NULL;
    if (task_id < SRTASKMONITOR_NUM_TASKS)
    {
        ptr_ref = &SRTaskMonitor_Tasks_Counter_Ref[task_id];
        #if (SRPOWERMGR_LOWPOWER_MODE == 3) // SRPOWERMGR_LOW_SPEED_MODE
            if (SRPowerMgr__GetMode() == SRPOWERMGR_LOW_SPEED_MODE)
            {
                ptr_ref = &SRTaskMonitor_Tasks_Low_Speed_Counter_Ref[task_id];
            }
        #endif
    }
    return(ptr_ref);
}

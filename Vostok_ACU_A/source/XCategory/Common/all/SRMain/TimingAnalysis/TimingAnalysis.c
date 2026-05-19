/**
 *  @file
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  @copyright  Copyright 2023. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"
#include "TimingAnalysis.h"

#if (TIMING_ANALYSIS == ENABLED)
#include "RTOS.h"
#include "Micro.h"
#include "TimerInterrupt.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------


typedef struct TIMING_ANALYSIS_PRIVATE_INTERRUPTS_STRUCT
{
    TIMING_ANALYSIS_INTERRUPT_DATA_TYPE Data;
    uint32 Working_Timer;
} TIMING_ANALYSIS_PRIVATE_INTERRUPTS_TYPE;

typedef struct TIMING_ANALYSIS_PRIVATE_TASKS_DATA_STRUCT
{
    TIMING_ANALYSIS_TASK_DATA_TYPE Data;
    uint32 Reference;
    uint32 Working_Timer;
} TIMING_ANALYSIS_PRIVATE_TASKS_DATA_TYPE;

static TIMING_ANALYSIS_PRIVATE_TASKS_DATA_TYPE Tasks_Monitor_Data[NUM_TASKS];
static TIMING_ANALYSIS_PRIVATE_INTERRUPTS_TYPE Interrupt_Monitor_Data;
static BOOL_TYPE Clear_Task_Data[NUM_TASKS];
static BOOL_TYPE Clear_Interrupt_Data;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It initializes the TimingAnalysis module and its variables
 *
 */
void TimingAnalysis__Initialize(void)
{
    Clear_Interrupt_Data = TRUE;
    memset(&Interrupt_Monitor_Data, 0x00, sizeof(Interrupt_Monitor_Data));
    memset(Tasks_Monitor_Data, 0x00, sizeof(Tasks_Monitor_Data));
    for(uint8 index=0; index<ELEMENTS_IN_ARRAY(Tasks_Monitor_Data); index++)
    {
        Clear_Task_Data[index] = TRUE;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Function handler to get and save Task timing data
 *  @param  task_index - Index of the Task under analysis
 *
 */
void TimingAnalysis__SampleTask(TASK_ID_TYPE task_index)
{
    if(task_index < NUM_TASKS)
    {
        TIMING_ANALYSIS_PRIVATE_TASKS_DATA_TYPE* task_ptr = &Tasks_Monitor_Data[task_index];
        uint32 time_exec;
        uint32 cpu_load;
        uint32 entry_time;

        OS_EnterRegion();
        OS_STAT_Sample();
        time_exec = OS_STAT_GetTaskExecTime(NULL);
        OS_LeaveRegion();

        OS_Timing_End(&task_ptr->Working_Timer);
        entry_time = TimerInterrupt__ConvertCyclesToUs(task_ptr->Working_Timer);
        OS_Timing_Start(&task_ptr->Working_Timer);

        cpu_load = TimerInterrupt__ConvertCyclesToUs(time_exec - task_ptr->Reference);

        if (task_ptr->Reference != 0U)
        {
            TIMING_ANALYSIS_TASK_DATA_TYPE *task_data_ptr = &task_ptr->Data;
            if(Clear_Task_Data[task_index])
            {
                Clear_Task_Data[task_index] = FALSE;
                task_data_ptr->Cpu_Max_Load =  cpu_load;
                task_data_ptr->Cpu_Entry_Max = entry_time;
                task_data_ptr->Cpu_Entry_Min = entry_time;
            }
            else
            {
                if(cpu_load > task_data_ptr->Cpu_Max_Load)
                {
                    task_data_ptr->Cpu_Max_Load =  cpu_load;
                }
                if(entry_time > task_data_ptr->Cpu_Entry_Max)
                {
                    task_data_ptr->Cpu_Entry_Max = entry_time;
                }
                if(entry_time < task_data_ptr->Cpu_Entry_Min)
                {
                    task_data_ptr->Cpu_Entry_Min = entry_time;
                }
            }
        }
        task_ptr->Reference = time_exec;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Function handler to get and save Task timing data
 *  @param  event - One of Start / End events to keep track for timing analysis
 *
 */
void TimingAnalysis__SampleInterrupt(TIMING_ANALYSIS_INTERRUPT_EVENTS_TYPE event)
{
	if (event == TIMING_ANALYSIS_INTERRUPT_START)
	{
		OS_Timing_Start(&Interrupt_Monitor_Data.Working_Timer);
	}
	else
	{
	    uint32 elapsed;

		OS_Timing_End(&Interrupt_Monitor_Data.Working_Timer);
		elapsed = TimerInterrupt__ConvertCyclesToUs(Interrupt_Monitor_Data.Working_Timer);
		Interrupt_Monitor_Data.Data.Elapsed_Time = elapsed;
		if(Clear_Interrupt_Data)
		{
		    Clear_Interrupt_Data = FALSE;
		    Interrupt_Monitor_Data.Data.Max_Elapsed_Time = elapsed;
		}
		else if (elapsed > Interrupt_Monitor_Data.Data.Max_Elapsed_Time)
		{
			Interrupt_Monitor_Data.Data.Max_Elapsed_Time = elapsed;
		}
	}
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Function handler to return Task timing data, if available; it set the clear data flag for the next event
 *  @param  task_index - Index of the Task under analysis
 *  @return pointer of TIMING_ANALYSIS_TASK_DATA_TYPE data
 *
 */
TIMING_ANALYSIS_TASK_DATA_TYPE* TimingAnalysis__GetTaskInfo(TASK_ID_TYPE taskIndex)
{
	TIMING_ANALYSIS_TASK_DATA_TYPE* retval = NULL;
	if ((taskIndex < NUM_TASKS) &&
	    (Clear_Task_Data[taskIndex] == FALSE))
	{
		retval = &(Tasks_Monitor_Data[taskIndex].Data);
		Clear_Task_Data[taskIndex] = TRUE;
	}
	return retval;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief  Function handler to return Interrupt timing data, if available; it set the clear data flag for the next event
 *  @return pointer of TIMING_ANALYSIS_TASK_DATA_TYPE data
 *
 */
TIMING_ANALYSIS_INTERRUPT_DATA_TYPE* TimingAnalysis__GetInterruptInfo(void)
{
    TIMING_ANALYSIS_INTERRUPT_DATA_TYPE* retval = NULL;
    if (Clear_Interrupt_Data == FALSE)
    {
        Clear_Interrupt_Data = TRUE;
        retval = &Interrupt_Monitor_Data.Data;
    }
    return retval;
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#endif


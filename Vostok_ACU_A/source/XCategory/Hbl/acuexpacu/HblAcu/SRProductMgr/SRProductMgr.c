/**
 *  @file		SRProductMgr.c
 *
 *  @brief      Class B Product Safety common module
 *
 *  @defgroup   CLASS_B
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SRSystemConfig.h"
#include "SRProductMgr.h"

#if(SRPRODUCTMGR_FEATURE == ENABLED)
    #include "SRProductMgr_prv.h"
    #include "SRException.h"
    #include "SRData.h"
    #include "HblDefs.h"
#endif
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    #include "SRTaskSequence.h"
#endif

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

#if(SRPRODUCTMGR_FEATURE == ENABLED)
    #define SRCHECK_NUM_HANDLERS		(sizeof(SRCHECK_HANDLERS) / sizeof(SRCHECK_HOOK_TYPE))

    CT_ASSERT(SRCHECK_NUM_HANDLERS < 255U);

    #if(SRPRODUCTMGR_SRACTION_FEATURE == ENABLED)
        #define SRACTION_NUM_HANDLERS		(sizeof(SRACTION_HANDLERS) / sizeof(SRACTION_HOOK_TYPE))

        CT_ASSERT(SRACTION_NUM_HANDLERS < 255U);

        typedef PACKED struct
            {
                uint8 SRCheck_Request[SRCHECK_NUM_HANDLERS+1U];
                uint8 Current;
                BOOL_TYPE Stopping;
            } SRACTION_PRIORITY_TYPE;

		static uint8 Current_SRCheck;
		static SRACTION_PRIORITY_TYPE SRAction_Priority_Requests[SRACTION_NUM_HANDLERS];
        static uint8 N_Current_SRCheck;
		static SRACTION_PRIORITY_TYPE N_SRAction_Priority_Requests[SRACTION_NUM_HANDLERS];
	#endif
#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module SRProductMgr and its variables
 *
 */
void SRProductMgr__Initialize(void)
{
#if(SRPRODUCTMGR_FEATURE == ENABLED)
    uint8 index;

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
            SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_INIT, SRTASKSEQUENCE_APPL_INIT_SRPRODUCTMGR);
    #endif

	#if(SRPRODUCTMGR_SRACTION_FEATURE == ENABLED)
        SRData__UpdateByte(&Current_SRCheck, &N_Current_SRCheck, SRCHECK_NUM_HANDLERS);
        SRData__Fill(SRAction_Priority_Requests, N_SRAction_Priority_Requests, (uint8) SRHBL_PRIORITY_MINIMUM, sizeof(SRAction_Priority_Requests));
	#endif

    // Initialize SRProductMgr Check modules
    for (index=0; index < SRCHECK_NUM_HANDLERS; index++)
    {
    	if (SRCHECK_HANDLERS[index].Initialize != NULL)
    	{
            #if(SRPRODUCTMGR_SRACTION_FEATURE == ENABLED)
                SRData__UpdateByte(&Current_SRCheck, &N_Current_SRCheck, index);
            #endif
    		SRCHECK_HANDLERS[index].Initialize();
    	}
    	else
    	{
		#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
			SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_INIT, ((uint8) SRTASKSEQUENCE_APPL_INIT_SRPRODUCTMGR + index + 1U));
		#endif
    	}
        #if(SRPRODUCTMGR_SRACTION_FEATURE == ENABLED)
            (void) SRData__CheckByte(&Current_SRCheck, &N_Current_SRCheck);
        #endif
    }
    #if(SRPRODUCTMGR_SRACTION_FEATURE == ENABLED)
        SRData__UpdateByte(&Current_SRCheck, &N_Current_SRCheck, SRCHECK_NUM_HANDLERS);
    #endif

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_INIT, SRTASKSEQUENCE_APPL_INIT_SRCHECKS_DONE);
    #endif

    #if(SRPRODUCTMGR_SRACTION_FEATURE == ENABLED)
        // Initialize SRProductMgr Action modules
        for (index=0; index < SRACTION_NUM_HANDLERS; index++)
        {
            SRData__UpdateByte((uint8 *) &SRAction_Priority_Requests[index].Stopping,(uint8 *)  &N_SRAction_Priority_Requests[index].Stopping, (uint8) FALSE);
            if (SRACTION_HANDLERS[index].Initialize != NULL)
            {
                SRACTION_HANDLERS[index].Initialize();
            }
            else
            {
            #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
                SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_INIT, (uint8) ((uint8) SRTASKSEQUENCE_APPL_INIT_SRCHECKS_DONE + index + 1U));
            #endif
            }
        }
        #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
            SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_INIT, SRTASKSEQUENCE_APPL_INIT_SRACTIONS_DONE);
        #endif
    #endif  // SRPRODUCTMGR_SRACTION_FEATURE == ENABLED
#endif  //  SRPRODUCTMGR_FEATURE == ENABLED
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      SRProductMgr handler - it executes all SRcheck and SRAction modules
 *
 */
void SRProductMgr__Handler(void)
{
#if(SRPRODUCTMGR_FEATURE == ENABLED)
    uint8 index;
	#if(SRPRODUCTMGR_SRACTION_FEATURE == ENABLED)
		SRPRODUCTMGR_ACTION_TYPE action_status;
		BOOL_TYPE is_suspended = FALSE;
	#endif

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
            SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_SRPRODUCTMGR_START);
    #endif

    #if(SRPRODUCTMGR_SRACTION_FEATURE == ENABLED)
        // Check and limit SRAction Start/Stop requests to Class A range
        (void) SRData__CheckByte(&Current_SRCheck, &N_Current_SRCheck);
        if(Current_SRCheck != SRCHECK_NUM_HANDLERS)
        {
			SRException__Queue(SREXCEPTION_SRACTION_DATA_NOT_CONSISTENT, 0xFFFF, 0xFFFF);
        }
    #endif

	// Execute all check handlers
    for (index=0; index < SRCHECK_NUM_HANDLERS; index++)
    {
    	if (SRCHECK_HANDLERS[index].Handler != NULL)
    	{
            #if(SRPRODUCTMGR_SRACTION_FEATURE == ENABLED)
                SRData__UpdateByte(&Current_SRCheck, &N_Current_SRCheck, index);
            #endif
    		SRCHECK_HANDLERS[index].Handler();
    	}
    	else
    	{
			#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
				SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, (uint8) ((uint8) SRTASKSEQUENCE_APPL_SRPRODUCTMGR_START + (2U * index) + 1U));
				SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, (uint8) ((uint8) SRTASKSEQUENCE_APPL_SRPRODUCTMGR_START + (2U * index) + 2U));
			#endif
    	}
        #if(SRPRODUCTMGR_SRACTION_FEATURE == ENABLED)
            (void) SRData__CheckByte(&Current_SRCheck, &N_Current_SRCheck);
        #endif
    }
    #if(SRPRODUCTMGR_SRACTION_FEATURE == ENABLED)
        SRData__UpdateByte(&Current_SRCheck, &N_Current_SRCheck, SRCHECK_NUM_HANDLERS);
    #endif

    #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
        SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_SRCHECKS_DONE);
    #endif

    #if(SRPRODUCTMGR_SRACTION_FEATURE == ENABLED)
        // Execute all actions handlers
        for (index=0; index < SRACTION_NUM_HANDLERS; index++)
        {
            const SRACTION_HOOK_TYPE *hook_ptr = &SRACTION_HANDLERS[index];
            SRACTION_PRIORITY_TYPE *srcheck_prio_ptr = &SRAction_Priority_Requests[index];
            SRACTION_PRIORITY_TYPE *n_srcheck_prio_ptr = &N_SRAction_Priority_Requests[index];
            uint8 new_prio = 0;
            uint8 srcheck_index;
            BOOL_TYPE do_start;
            BOOL_TYPE do_stop;

            action_status = SRPRODUCTMGR_ACTION_IDLE;

            (void) SRData__CheckArray((uint8 *) srcheck_prio_ptr,(uint8 *) n_srcheck_prio_ptr, sizeof(SRAction_Priority_Requests[0]));

            for(srcheck_index = 0; srcheck_index < (SRCHECK_NUM_HANDLERS + 1U); srcheck_index++)
            {
                uint8 current_prio = srcheck_prio_ptr->SRCheck_Request[srcheck_index];

                // Class A requests cannot be on the Class B priority range
                if((srcheck_index >= SRCHECK_NUM_HANDLERS) &&
                    (current_prio >= (uint8) SRHBL_PRIORITY_CLASSB_LO))
                {
                    current_prio = (uint8) SRHBL_PRIORITY_CLASSB_LO - 1U;
                }

                // Find the highest priority to use as current
                if(current_prio > new_prio)
                {
                    new_prio = current_prio;
                }
            }

            // Get the SRAction status
            if(hook_ptr->GetStatus != NULL)
            {
                action_status = hook_ptr->GetStatus();
            }

            // Start request will be issued if SRAction is in Idle state or waiting to stop
            do_start = (BOOL_TYPE) ((new_prio != (uint8) SRHBL_PRIORITY_MINIMUM) &&
                ((action_status == SRPRODUCTMGR_ACTION_IDLE) ||
                (srcheck_prio_ptr->Stopping != FALSE)));
            // Stop request will be issued if SRAction is not in Idle state and stop is not in progress
            do_stop = (BOOL_TYPE) ((new_prio == (uint8) SRHBL_PRIORITY_MINIMUM) &&
                (action_status != SRPRODUCTMGR_ACTION_IDLE) &&
                (srcheck_prio_ptr->Stopping == FALSE));

            // Issue Stop action if stop request issued OR suspended flag set
            if ((hook_ptr->Stop != NULL) &&
                (is_suspended || do_stop))
            {
                (void) hook_ptr->Stop();
                SRData__UpdateByte((uint8 *) &srcheck_prio_ptr->Stopping,(uint8 *) &n_srcheck_prio_ptr->Stopping, (uint8) TRUE);
            }
            // Issue Start action if start request issued
            else if ((hook_ptr->Start != NULL) &&
                (do_start))
            {
                (void) hook_ptr->Start();
                SRData__UpdateByte((uint8 *) &srcheck_prio_ptr->Stopping,(uint8 *) &n_srcheck_prio_ptr->Stopping, (uint8) FALSE);
            }

            // When stop action is in progress, keep current priority - in case of SRHBL_PRIORITY_MINIMUM, it uses SRHBL_PRIORITY_CLASSB_LO - 1 instead.
            if(srcheck_prio_ptr->Stopping)
            {
                if(srcheck_prio_ptr->Current > new_prio)
                {
                    new_prio = srcheck_prio_ptr->Current;
                }
                if(new_prio == SRHBL_PRIORITY_MINIMUM)
                {
                    new_prio = SRHBL_PRIORITY_CLASSB_LO - 1;
                }
            }

            if (hook_ptr->Handler != NULL)
            {
                action_status = hook_ptr->Handler(new_prio);

                // First action with SRPRODUCTMGR_ACTION_RUNNING_EXCL status will stop execution of next ones
                if ( (action_status == SRPRODUCTMGR_ACTION_RUNNING_EXCL) ||
                        (action_status == SRPRODUCTMGR_ACTION_STOPPING_EXCL) )
                {
                    is_suspended = TRUE;
                }
            }
            else
            {
                #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
                    SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, (uint8) ((uint8) SRTASKSEQUENCE_APPL_SRCHECKS_DONE + (2U * index) + 1U));
                    SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, (uint8) ((uint8) SRTASKSEQUENCE_APPL_SRCHECKS_DONE + (2U * index) + 2U));
                #endif
            }

            // Reset action priority if action status is idle
            if (action_status == SRPRODUCTMGR_ACTION_IDLE)
            {
                new_prio = (uint8) SRHBL_PRIORITY_MINIMUM;
                SRData__UpdateByte((uint8 *) &srcheck_prio_ptr->Stopping,(uint8 *) &n_srcheck_prio_ptr->Stopping, (uint8) FALSE);
            }

            // Update current priority value
            if(new_prio != srcheck_prio_ptr->Current)
            {
                SRData__UpdateByte((uint8 *) &srcheck_prio_ptr->Current,(uint8 *) &n_srcheck_prio_ptr->Current, new_prio);
            }
        }

        #if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
            SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_SRACTIONS_DONE);
        #endif
    #endif
#endif
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//          SRCheck related interface
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Send a command to all SRCheck modules
 *
 */
void SRProductMgr__IssueSRCheckCmd(SRPRODUCTMGR_CHECK_CMD_TYPE cmd)
{
#if(SRPRODUCTMGR_FEATURE == ENABLED)
	uint8 index;

	for (index = 0; index < SRCHECK_NUM_HANDLERS; index++)
	{
		if (SRCHECK_HANDLERS[index].IssueCmd != NULL)
		{
			SRCHECK_HANDLERS[index].IssueCmd(cmd);
		}
	}
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Function to get a specific SRCheck status + fault source
 *  @param		index
 *  @param		status
 *
 */
PASS_FAIL_TYPE SRProductMgr__GetSRCheckStatus(uint8 index, SRPRODUCTMGR_CHECK_STATUS_TYPE *status)
{
	PASS_FAIL_TYPE retval = FAIL;

    #if(SRPRODUCTMGR_FEATURE == ENABLED)
        if (index < SRCHECK_NUM_HANDLERS)
        {
            if (SRCHECK_HANDLERS[index].GetStatus != NULL)
            {
                SRCHECK_HANDLERS[index].GetStatus(status);
                retval =  PASS;
            }
        }
    #endif
	return(retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Function to get a SRCheck status for a single fault source
 *  @param		index
 *  @param		status
 *
 */
SRPRODUCTMGR_STATUS_TYPE SRProductMgr__GetSRCheckStatusByFaultID(uint8 fault_id)
{
	SRPRODUCTMGR_STATUS_TYPE fault_status = SRPRODUCTMGR_STATUS_IDLE;

    #if(SRPRODUCTMGR_FEATURE == ENABLED)
        SRPRODUCTMGR_CHECK_STATUS_TYPE status_check;
        uint8 index;

        for (index = 0; index < SRCHECK_NUM_HANDLERS; index++)
        {
            if (SRCHECK_HANDLERS[index].GetStatus != NULL)
            {
                SRCHECK_HANDLERS[index].GetStatus(&status_check);
                if (status_check.Fault_ID == fault_id)
                {
                    fault_status = status_check.Status;
                    break;
                }
            }
        }
    #endif
	return(fault_status);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Function to get SRCheck timer set - its value depends on its status
 *  @param		index
 *
 */
uint16 SRProductMgr__GetSRCheckTimer(uint8 index)
{
	uint16 retval = 0;

    #if(SRPRODUCTMGR_FEATURE == ENABLED)
        if (index < SRCHECK_NUM_HANDLERS)
        {
            if (SRCHECK_HANDLERS[index].GetTimer != NULL)
            {
                retval = SRCHECK_HANDLERS[index].GetTimer();
            }
        }
    #endif
	return(retval);
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//          SRAction related interface
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


SRPRODUCTMGR_ACTION_TYPE SRProductMgr__GetSRActionStatus(uint8 index)
{
	SRPRODUCTMGR_ACTION_TYPE retval = SRPRODUCTMGR_ACTION_INVALID;

    #if((SRPRODUCTMGR_FEATURE == ENABLED) && (SRPRODUCTMGR_SRACTION_FEATURE == ENABLED))
        if (index < SRACTION_NUM_HANDLERS)
        {
            if (SRACTION_HANDLERS[index].GetStatus != NULL)
            {
                retval = SRACTION_HANDLERS[index].GetStatus();
            }
        }
    #endif
	return(retval);
}

PASS_FAIL_TYPE SRProductMgr__StartSRAction(uint8 index, uint8 priority)
{
	PASS_FAIL_TYPE retval = FAIL;

    #if((SRPRODUCTMGR_FEATURE == ENABLED) && (SRPRODUCTMGR_SRACTION_FEATURE == ENABLED))
        (void) SRData__CheckByte(&Current_SRCheck, &N_Current_SRCheck);
        if ((index < SRACTION_NUM_HANDLERS) &&
            (Current_SRCheck <= SRCHECK_NUM_HANDLERS))
        {
            uint8 *srcheck_prio_ptr = &SRAction_Priority_Requests[index].SRCheck_Request[Current_SRCheck];
            uint8 *n_srcheck_prio_ptr = &N_SRAction_Priority_Requests[index].SRCheck_Request[Current_SRCheck];

            (void) SRData__CheckByte(srcheck_prio_ptr, n_srcheck_prio_ptr);
            if (*srcheck_prio_ptr <= priority)
            {
                SRData__UpdateByte(srcheck_prio_ptr, n_srcheck_prio_ptr, priority);
                retval = PASS;
            }
        }
    #endif
	return(retval);
}

PASS_FAIL_TYPE SRProductMgr__StopSRAction(uint8 index, uint8 priority)
{
	PASS_FAIL_TYPE retval = FAIL;

    #if((SRPRODUCTMGR_FEATURE == ENABLED) && (SRPRODUCTMGR_SRACTION_FEATURE == ENABLED))
        (void) SRData__CheckByte(&Current_SRCheck, &N_Current_SRCheck);
        if ((index < SRACTION_NUM_HANDLERS) &&
            (Current_SRCheck <= SRCHECK_NUM_HANDLERS))
        {
            uint8 *srcheck_prio_ptr = &SRAction_Priority_Requests[index].SRCheck_Request[Current_SRCheck];
            uint8 *n_srcheck_prio_ptr = &N_SRAction_Priority_Requests[index].SRCheck_Request[Current_SRCheck];

            (void) SRData__CheckByte(srcheck_prio_ptr, n_srcheck_prio_ptr);
            SRData__UpdateByte(srcheck_prio_ptr, n_srcheck_prio_ptr, SRHBL_PRIORITY_MINIMUM);
            retval = PASS;
        }
    #endif
	return(retval);
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================



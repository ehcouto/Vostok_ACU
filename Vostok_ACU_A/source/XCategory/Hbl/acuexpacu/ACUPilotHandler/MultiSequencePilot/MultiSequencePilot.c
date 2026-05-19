/**
 *  @file
 *
 *  @brief      This is Damper ON/OFF type
 *
 *
 *
 *  @details    Detailed description of the file contents
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
#include "C_Extensions.h"
#include "MultiSequencePilot.h"

#if (HBL_PILOT_NUM_MULTISEQUENCE > 0)
#include "string.h"
#include "Millis.h"
#include "SettingFile.h"

#if (HBL_PILOT_NUM_DIGITALOUTPUT > 0)
    #include "DigitalOutputPilot.h"
#endif

#if (HBL_PILOT_NUM_TRIAC > 0)
    #include "TriacPilot.h"
#endif

#if (HBL_PILOT_NUM_ZCRELAY > 0)
    #include "ZCRelayPilot.h"
#endif

#if (HBL_PILOT_NUM_STEADYPWMPROFILE > 0)
    #include "SteadyPwmProfilePilot.h"
#endif

#if (HBL_PILOT_NUM_EXPANSION > 0)
    #include "ExpansionPilot.h"
#endif

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#if (HBL_PILOT_NUM_DIGITALOUTPUT > 0)
    #define MULTISEQUENCE_REGISTERED_PILOT_DIGITAL        1
#else
    #define MULTISEQUENCE_REGISTERED_PILOT_DIGITAL        0
#endif

#if (HBL_PILOT_NUM_TRIAC > 0)
    #define MULTISEQUENCE_REGISTERED_PILOT_TRIAC          1
#else
    #define MULTISEQUENCE_REGISTERED_PILOT_TRIAC          0
#endif

#if (HBL_PILOT_NUM_ZCRELAY > 0)
    #define MULTISEQUENCE_REGISTERED_PILOT_ZCRELAY        1
#else
    #define MULTISEQUENCE_REGISTERED_PILOT_ZCRELAY        0
#endif

#if (HBL_PILOT_NUM_STEADYPWMPROFILE > 0)
    #define MULTISEQUENCE_REGISTERED_PILOT_STEADYPWMPROFILE        1
#else
    #define MULTISEQUENCE_REGISTERED_PILOT_STEADYPWMPROFILE        0
#endif

#if (HBL_PILOT_NUM_EXPANSION > 0)
    #define MULTISEQUENCE_REGISTERED_PILOT_EXPANSION    1
#else
    #define MULTISEQUENCE_REGISTERED_PILOT_EXPANSION    0
#endif


#define MULTISEQUENCE_NUM_REGISTERED_PILOT                  (MULTISEQUENCE_REGISTERED_PILOT_DIGITAL + MULTISEQUENCE_REGISTERED_PILOT_TRIAC + MULTISEQUENCE_REGISTERED_PILOT_ZCRELAY + MULTISEQUENCE_REGISTERED_PILOT_STEADYPWMPROFILE + MULTISEQUENCE_REGISTERED_PILOT_EXPANSION)

#define NO_PARAMETER_OFFSET                                 0xFF
#define MULTISEQUENCEPILOT_MAX_PILOT_HANDLING               4

#define MULTISEQUENCEPILOT_MINIMUM_SEQUENCE_STATE_BYE_SIZE  2
#define MULTISEQUENCEPILOT_SEQUENCE_STATE_HEADER_SIZE       2

PACKED typedef struct
{
    uint8 Duration;
    uint8 Pilot_State[MULTISEQUENCEPILOT_MAX_PILOT_HANDLING];
} MULTISEQUENCEPILOT_SEQUENCE_STATE_TYPE;


PACKED typedef struct
{
    uint8 Number_Sequence;
    uint8 unused;
    MULTISEQUENCEPILOT_SEQUENCE_STATE_TYPE *Sequence_State;
} MULTISEQUENCEPILOT_SEQUENCE_TYPE;

PACKED typedef struct
{
    HBL_PILOT_ENUM_TYPE Pilot_Type;
    uint8 Pilot_Type_Params_Offset;
} MULTISEQUENCEPILOT_PARAMETERS_TYPE;


typedef struct
{
    HBL_LOAD_TYPE Load[MULTISEQUENCEPILOT_MAX_PILOT_HANDLING];
    HBL_LOAD_STATUS_TYPE Status;
    uint8 Current_Sequence_State;
    uint8 Requested_State;
    uint8 Processed_State;
    uint8 Pilot_OutPut_State[MULTISEQUENCEPILOT_MAX_PILOT_HANDLING];
    uint8 Max_Pilot;
    uint8 Max_Sequence;
    uint8 *Sequence_Displacement_Ptr;
    MULTISEQUENCEPILOT_SEQUENCE_TYPE *Sequence_State_Ptr;
    MILLIS_TIMER_TYPE Millis_Timer;
    uint8 TimeOut;
} MULTISEQUENCEPILOT_DATA_TYPE;


#if (MULTISEQUENCE_NUM_REGISTERED_PILOT > 0)
static MULTISEQUENCEPILOT_DATA_TYPE MultiSequencePilot_Data[HBL_PILOT_NUM_MULTISEQUENCE];
static uint8 MultiSequencePilot_Allocation;

static const HBL_PILOT_HANDLER_HOOK_TYPE MULTIDRIVER__PILOT_HANDLERS[MULTISEQUENCE_NUM_REGISTERED_PILOT] = {
                                                                                                        #if (HBL_PILOT_NUM_DIGITALOUTPUT > 0)
                                                                                                            {DIGITALOUTPUTPILOT_REGISTER        ,HBL_PILOT_DIGITAL}
                                                                                                        #endif

                                                                                                        #if (HBL_PILOT_NUM_TRIAC > 0)
                                                                                                            ,{TRIACPILOT_REGISTER               ,HBL_PILOT_TRIAC}
                                                                                                        #endif

                                                                                                        #if (HBL_PILOT_NUM_ZCRELAY > 0)
                                                                                                            ,{ZCRELAYPILOT_REGISTER             ,HBL_PILOT_ZEROCROSSING_RELAY}
                                                                                                        #endif

                                                                                                        #if (HBL_PILOT_NUM_STEADYPWMPROFILE > 0)
                                                                                                            ,{STEADYPWMPROFILEPILOT_REGISTER    ,HBL_PILOT_STEADY_PWM_PROFILE}
                                                                                                        #endif

                                                                                                        #if (HBL_PILOT_NUM_EXPANSION > 0)
                                                                                                            ,{EXPANSION_PILOT_REGISTER          ,HBL_PILOT_EXPANSION_DIGITAL}
                                                                                                        #endif
                                                                                                     };

#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module MULTISEQUENCEPILOT and its variables
 *
 */
void MultiSequencePilot__Initialize(void)
{
    #if (MULTISEQUENCE_NUM_REGISTERED_PILOT > 0)
        MultiSequencePilot_Allocation = 0;
        memset(&MultiSequencePilot_Data,0,sizeof(MultiSequencePilot_Data));
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @return
 */
uint8 MultiSequencePilot__Allocate(void)
{
    uint8 retval;
    retval = 0xFF;

    #if (MULTISEQUENCE_NUM_REGISTERED_PILOT > 0)
        if (MultiSequencePilot_Allocation < HBL_PILOT_NUM_MULTISEQUENCE)
        {
            retval = MultiSequencePilot_Allocation;
            MultiSequencePilot_Allocation++;
        }
    #endif

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @param load
 */
PASS_FAIL_TYPE MultiSequencePilot__InitializeInstance(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval;
    uint8 index;
    uint8 handler_index;
    MULTISEQUENCEPILOT_PARAMETERS_TYPE *parameters;

    retval = FAIL;

    #if (MULTISEQUENCE_NUM_REGISTERED_PILOT > 0)
        if (load->Pilot_Data_Index < MultiSequencePilot_Allocation)
        {
            parameters = (void*)load->Parameter_Ptr.Data;

            //HBL_PILOT_MULTISEQUENCE_1
            MultiSequencePilot_Data[load->Pilot_Data_Index].Max_Pilot             = 1;

            if (load->Pilot_Type == HBL_PILOT_MULTISEQUENCE_2)
            {
                MultiSequencePilot_Data[load->Pilot_Data_Index].Max_Pilot = 2;
            }
            else if (load->Pilot_Type == HBL_PILOT_MULTISEQUENCE_3)
            {
                MultiSequencePilot_Data[load->Pilot_Data_Index].Max_Pilot = 3;
            }
            else if (load->Pilot_Type == HBL_PILOT_MULTISEQUENCE_4)
            {
                MultiSequencePilot_Data[load->Pilot_Data_Index].Max_Pilot = 4;
            }

            //Load the Pilot parameters
            for (index = 0; index < MultiSequencePilot_Data[load->Pilot_Data_Index].Max_Pilot; index++)
            {
                MultiSequencePilot_Data[load->Pilot_Data_Index].Load[index].Load_Id                  = load->Load_Id;
                MultiSequencePilot_Data[load->Pilot_Data_Index].Load[index].Pilot_Type               = parameters[index].Pilot_Type;
                MultiSequencePilot_Data[load->Pilot_Data_Index].Load[index].Pilot_Type_Params_Offset = parameters[index].Pilot_Type_Params_Offset;
                MultiSequencePilot_Data[load->Pilot_Data_Index].Load[index].Pin_Ptr                  = &(load->Pin_Ptr[index]);

                MultiSequencePilot_Data[load->Pilot_Data_Index].Load[index].Hi_Low_Activation_Bitmap =  (load->Hi_Low_Activation_Bitmap >> index) & 0x01;

                if (MultiSequencePilot_Data[load->Pilot_Data_Index].Load[index].Pilot_Type_Params_Offset != NO_PARAMETER_OFFSET)
                {
                    SettingFile__BasicLoader(SF_PTR_ACU_IO_DEVICE, MultiSequencePilot_Data[load->Pilot_Data_Index].Load[index].Pilot_Type_Params_Offset, &MultiSequencePilot_Data[load->Pilot_Data_Index].Load[index].Parameter_Ptr);
                }
                else
                {
                    MultiSequencePilot_Data[load->Pilot_Data_Index].Load[index].Parameter_Ptr.Data = NULL;
                    MultiSequencePilot_Data[load->Pilot_Data_Index].Load[index].Parameter_Ptr.Length = 0;
                }

                MultiSequencePilot_Data[load->Pilot_Data_Index].Load[index].Pilot_Data_Index = 0xFF;
                MultiSequencePilot_Data[load->Pilot_Data_Index].Load[index].Pilot_Handler_Index= HBL_INDEX_INVALID;

                for (handler_index = 0; handler_index < MULTISEQUENCE_NUM_REGISTERED_PILOT; handler_index++)
                {
                    if (MultiSequencePilot_Data[load->Pilot_Data_Index].Load[index].Pilot_Type == MULTIDRIVER__PILOT_HANDLERS[handler_index].Pilot)
                    {
                        MultiSequencePilot_Data[load->Pilot_Data_Index].Load[index].Pilot_Handler_Index  = handler_index;
                        MultiSequencePilot_Data[load->Pilot_Data_Index].Load[index].Pilot_Data_Index  = MULTIDRIVER__PILOT_HANDLERS[handler_index].Allocate();
                        retval  = MULTIDRIVER__PILOT_HANDLERS[handler_index].InitializeInstance(&MultiSequencePilot_Data[load->Pilot_Data_Index].Load[index]);
                        break;
                    }
                }
            }

            //Multi Sequence part
            MultiSequencePilot_Data[load->Pilot_Data_Index].Max_Sequence              = load->Parameter_Ptr.Data[sizeof(MULTISEQUENCEPILOT_PARAMETERS_TYPE) * MultiSequencePilot_Data[load->Pilot_Data_Index].Max_Pilot];
            MultiSequencePilot_Data[load->Pilot_Data_Index].Sequence_Displacement_Ptr = &(load->Parameter_Ptr.Data[(sizeof(MULTISEQUENCEPILOT_PARAMETERS_TYPE) * MultiSequencePilot_Data[load->Pilot_Data_Index].Max_Pilot) + 1]);

            MultiSequencePilot_Data[load->Pilot_Data_Index].TimeOut = 0;
            Millis__Reset(&MultiSequencePilot_Data[load->Pilot_Data_Index].Millis_Timer);

            MultiSequencePilot_Data[load->Pilot_Data_Index].Requested_State           = 0;
            MultiSequencePilot_Data[load->Pilot_Data_Index].Processed_State           = 0;
            MultiSequencePilot_Data[load->Pilot_Data_Index].Status.Load.Status        = HBL_LOAD_STATUS_TARGET_COMPLETED;
            MultiSequencePilot_Data[load->Pilot_Data_Index].Status.Load.Cur_Reg       = 0;

            //Turn off all the pilots
            memset(MultiSequencePilot_Data[load->Pilot_Data_Index].Pilot_OutPut_State, 0, MultiSequencePilot_Data[load->Pilot_Data_Index].Max_Pilot);
        }

    #endif

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @param load
 * @param data
 * @return
 */
PASS_FAIL_TYPE MultiSequencePilot__Request(HBL_LOAD_TYPE * load, void * data)
{
    PASS_FAIL_TYPE retval;

    retval = FAIL;

    #if (MULTISEQUENCE_NUM_REGISTERED_PILOT > 0)
        if (load->Pilot_Data_Index < MultiSequencePilot_Allocation)
        {
            if (MultiSequencePilot_Data[load->Pilot_Data_Index].Max_Sequence > *((uint8 *)data))
            {
                MultiSequencePilot_Data[load->Pilot_Data_Index].Requested_State = *((uint8 *)data);

                retval = PASS;
            }
        }

    #endif

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @param load
 * @return
 */
PASS_FAIL_TYPE MultiSequencePilot__Process(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval;
    SETTINGFILE_LOADER_TYPE sequence_state_loader;

    retval     = FAIL;

    #if (MULTISEQUENCE_NUM_REGISTERED_PILOT > 0)
        if (load->Pilot_Data_Index < MultiSequencePilot_Allocation)
        {
            //Sequence 0 is to turn off all the loads has the highest priority
            // Other sequence requested are accepted only when the previous sequence is reached to SEQUENCE_END
            // If Requested_State and Processed_State are the same, it continues on that sequence.
            if (((MultiSequencePilot_Data[load->Pilot_Data_Index].Requested_State == 0) ||
                    (MultiSequencePilot_Data[load->Pilot_Data_Index].Status.Load.Status == HBL_LOAD_STATUS_TARGET_COMPLETED)) &&
                (MultiSequencePilot_Data[load->Pilot_Data_Index].Requested_State != MultiSequencePilot_Data[load->Pilot_Data_Index].Processed_State))
            {
                MultiSequencePilot_Data[load->Pilot_Data_Index].Sequence_State_Ptr       = NULL;
                SettingFile__BasicLoader(SF_PTR_ACU_IO_DEVICE, MultiSequencePilot_Data[load->Pilot_Data_Index].Sequence_Displacement_Ptr[MultiSequencePilot_Data[load->Pilot_Data_Index].Requested_State], &sequence_state_loader);
                MultiSequencePilot_Data[load->Pilot_Data_Index].Processed_State          = MultiSequencePilot_Data[load->Pilot_Data_Index].Requested_State;
                MultiSequencePilot_Data[load->Pilot_Data_Index].Current_Sequence_State   = 0;
                MultiSequencePilot_Data[load->Pilot_Data_Index].Status.Load.Status       = HBL_LOAD_STATUS_WAITING_TARGET;
                MultiSequencePilot_Data[load->Pilot_Data_Index].Status.Load.Cur_Reg      = MultiSequencePilot_Data[load->Pilot_Data_Index].Requested_State;

                //expire the timeout
                MultiSequencePilot_Data[load->Pilot_Data_Index].TimeOut = 0;
                Millis__Reset(&MultiSequencePilot_Data[load->Pilot_Data_Index].Millis_Timer);

                MultiSequencePilot_Data[load->Pilot_Data_Index].Sequence_State_Ptr = (void*)sequence_state_loader.Data;

                retval = PASS;
            }
        }

    #endif

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @param load
 * @param data
 * @return
 */
PASS_FAIL_TYPE MultiSequencePilot__AsynchProcess(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval = FAIL;
    #if (MULTISEQUENCE_NUM_REGISTERED_PILOT > 0)
    if (load->Pilot_Data_Index < MultiSequencePilot_Allocation)
    {
        MULTISEQUENCEPILOT_DATA_TYPE * data = &MultiSequencePilot_Data[load->Pilot_Data_Index];

        // Manage the Sequence State
        if ( (Millis__ReachedTarget(&data->Millis_Timer , data->TimeOut * 5) == TRUE) &&
             (data->Sequence_State_Ptr != NULL) )
        {
            if ( data->Current_Sequence_State < data->Sequence_State_Ptr->Number_Sequence)
            {
                MULTISEQUENCEPILOT_SEQUENCE_STATE_TYPE * sequence_state_parameter = (MULTISEQUENCEPILOT_SEQUENCE_STATE_TYPE *)((uint8 *)data->Sequence_State_Ptr +
                                                                                                                               MULTISEQUENCEPILOT_SEQUENCE_STATE_HEADER_SIZE +
                                                                                                                               data->Current_Sequence_State * (data->Max_Pilot + 1));

                memcpy(data->Pilot_OutPut_State, sequence_state_parameter->Pilot_State, data->Max_Pilot);

                // Handle the Request function for pilots
                for ( uint8 index = 0; index < data->Max_Pilot; index++ )
                {
                    uint8 handler_index = data->Load[index].Pilot_Handler_Index;
                    if (( handler_index != HBL_INDEX_INVALID ) &&
                        ( MULTIDRIVER__PILOT_HANDLERS[handler_index].Request != NULL ))
                    {
                        retval = MULTIDRIVER__PILOT_HANDLERS[handler_index].Request(&(data->Load[index]), &(data->Pilot_OutPut_State[index]) );
                        if (( retval == PASS ) &&
                            ( MULTIDRIVER__PILOT_HANDLERS[handler_index].Process != NULL ))
                        {
                            retval &= MULTIDRIVER__PILOT_HANDLERS[handler_index].Process(&(data->Load[index]));
                        }
                    }
                }

                // set the timer
                data->TimeOut = sequence_state_parameter->Duration;
                Millis__Start(&data->Millis_Timer);

                data->Current_Sequence_State++;

            }
            else
            {
                data->Status.Load.Status = HBL_LOAD_STATUS_TARGET_COMPLETED;
                data->Sequence_State_Ptr = NULL;
            }
        }

        // Handle the Asynchronous function for pilots
        for ( uint8 index = 0; index < data->Max_Pilot; index++ )
        {
            uint8 handler_index = data->Load[index].Pilot_Handler_Index;
            if (( handler_index != HBL_INDEX_INVALID ) &&
                ( MULTIDRIVER__PILOT_HANDLERS[handler_index].AsynchProcess != NULL ))
            {
                retval = MULTIDRIVER__PILOT_HANDLERS[handler_index].AsynchProcess(&(data->Load[index]));
            }
        }
    }
    #endif
    return ( retval );
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @param load
 * @param data
 * @return
 */
PASS_FAIL_TYPE MultiSequencePilot__ZCProcess(HBL_LOAD_TYPE * load)
{
    PASS_FAIL_TYPE retval;
    uint8 index;
    uint8 handler_index;

    retval     = FAIL;

    #if (MULTISEQUENCE_NUM_REGISTERED_PILOT > 0)
        if (load->Pilot_Data_Index < MultiSequencePilot_Allocation)
        {
            for (index = 0; index < MultiSequencePilot_Data[load->Pilot_Data_Index].Max_Pilot; index++)
            {
                handler_index = MultiSequencePilot_Data[load->Pilot_Data_Index].Load[index].Pilot_Handler_Index;

                if (handler_index != HBL_INDEX_INVALID)
                {
                    if (MULTIDRIVER__PILOT_HANDLERS[handler_index].ZcProcess != NULL)
                    {
                        retval  = MULTIDRIVER__PILOT_HANDLERS[handler_index].ZcProcess(&MultiSequencePilot_Data[load->Pilot_Data_Index].Load[index]);
                    }
                }
            }
        }

    #endif

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @param load
 * @return
 */
PASS_FAIL_TYPE MultiSequencePilot__OneMsHandler(HBL_LOAD_TYPE * load)
{
    #if (HBL_PILOT_NUM_STEADYPWMPROFILE > 0)
        PASS_FAIL_TYPE retval;
        uint8 index;
        uint8 handler_index;

        retval     = FAIL;

        #if (MULTISEQUENCE_NUM_REGISTERED_PILOT > 0)
            if (load->Pilot_Data_Index < MultiSequencePilot_Allocation)
            {
                for (index = 0; index < MultiSequencePilot_Data[load->Pilot_Data_Index].Max_Pilot; index++)
                {
                    handler_index = MultiSequencePilot_Data[load->Pilot_Data_Index].Load[index].Pilot_Handler_Index;

                    if (handler_index != HBL_INDEX_INVALID)
                    {
                        if (MULTIDRIVER__PILOT_HANDLERS[handler_index].OneMsHandler != NULL)
                        {
                            retval  = MULTIDRIVER__PILOT_HANDLERS[handler_index].OneMsHandler(&MultiSequencePilot_Data[load->Pilot_Data_Index].Load[index]);
                        }
                    }
                }
            }

        #endif

        return (retval);
    #else
        return(FAIL);
    #endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @param load
 * @return
 */
PASS_FAIL_TYPE MultiSequencePilot__OCProcess(HBL_LOAD_TYPE * load)
{
    #if (HBL_PILOT_NUM_TRIAC > 0)
        PASS_FAIL_TYPE retval;
        uint8 index;
        uint8 handler_index;

        retval     = FAIL;

        #if (MULTISEQUENCE_NUM_REGISTERED_PILOT > 0)
            if (load->Pilot_Data_Index < MultiSequencePilot_Allocation)
            {
                for (index = 0; index < MultiSequencePilot_Data[load->Pilot_Data_Index].Max_Pilot; index++)
                {
                    handler_index = MultiSequencePilot_Data[load->Pilot_Data_Index].Load[index].Pilot_Handler_Index;

                    if (handler_index != HBL_INDEX_INVALID)
                    {
                        if (MULTIDRIVER__PILOT_HANDLERS[handler_index].OCProcess != NULL)
                        {
                            retval  = MULTIDRIVER__PILOT_HANDLERS[handler_index].OCProcess(&MultiSequencePilot_Data[load->Pilot_Data_Index].Load[index]);
                        }
                    }
                }
            }

        #endif

    return (retval);
    #else
        return(FAIL);
    #endif
}
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @param load
 * @return
 */
void * MultiSequencePilot__GetRequested(HBL_LOAD_TYPE * load)
{
    void *retval;

    retval = NULL;

#if (MULTISEQUENCE_NUM_REGISTERED_PILOT > 0)
    if (load->Pilot_Data_Index < MultiSequencePilot_Allocation)
    {
        retval = (void *)(&MultiSequencePilot_Data[load->Pilot_Data_Index].Requested_State);
    }
#endif

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief
 * @param load
 * @return
 */
void * MultiSequencePilot__GetProcessed(HBL_LOAD_TYPE * load)
{
    void *retval;

    retval = NULL;

#if (MULTISEQUENCE_NUM_REGISTERED_PILOT > 0)
    if (load->Pilot_Data_Index < MultiSequencePilot_Allocation)
    {
        retval = (void *) (&MultiSequencePilot_Data[load->Pilot_Data_Index].Processed_State);
    }
#endif

    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Get Multi sequence current status.
 *
 *  @param      HBL_LOAD_TYPE *load - Pointer to MultiSequence load instance.
 *
 *  @return     void *status - pointer to status of pilot instance. Cast it using *((HBL_LOAD_STATUS_TYPE *)ptr) to get value.
 *
 */
void * MultiSequencePilot__GetStatus(HBL_LOAD_TYPE *load)
{
    void *status;

    status = NULL;

    #if (MULTISEQUENCE_NUM_REGISTERED_PILOT > 0)
        if (load->Pilot_Data_Index < MultiSequencePilot_Allocation)
        {
            status = (void *)(&MultiSequencePilot_Data[load->Pilot_Data_Index].Status.Byte);
        }
    #endif

    return (status);
}


//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

#endif

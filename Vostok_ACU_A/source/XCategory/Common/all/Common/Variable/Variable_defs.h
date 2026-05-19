/**
 *  @file      Variable_defs.h
 *
 *  @brief      This is derived from Variable.c to make that file cleaner all definitions related to the Cross-Category
 *  			variables was placed here.
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef VARIABLE_DEFS_H_
#define VARIABLE_DEFS_H_

#include "SystemConfig.h"
#if(VISUAL_MACRO_VARIABLES_FEATURE == ENABLED)
    #include "HmiVariable.h"
#endif

//=====================================================================================================================
//-------------------------------------- Variables, Constants & Defines				    -------------------------------
//=====================================================================================================================

/**
 * Dummy variable to hide the array of Regulation. We'll hold a position on DATA_HOLDER to allow us to link with Regulation
 * and use Variable module to access data using the save interface. No need to add special flags on the Settings File to inform
 * the firmware we are dealing with this or that type.
 */
static const uint8 Regulation_Dummy = 0;

#ifndef VARIABLE_NUMBER_OF_GENERAL_PURPOSE
#define VARIABLE_NUMBER_OF_GENERAL_PURPOSE  6
#endif

#if (SYSTEM_ACU_HANDLING == ENABLED)
    #pragma location = "_appliance_segment"
    static uint16 Variable_General_Purpose[VARIABLE_NUMBER_OF_GENERAL_PURPOSE];
    // Variable to used for flags by the User on his Cycles.
    #pragma location = "_appliance_segment"
    static uint8 Variable_General_Flags;
    // A value used in JumpIf statements to always jump or never jump.
    static const BOOL_TYPE Always_True = TRUE;
    // Variable from Interpreter.
    #include "Interpreter.h"
    extern INTERPRETER_STATUS_TYPE Interpreter_Status;
    // Variable from Statistics
    #include "Statistics.h"
    extern STATISTICS_TYPE Statistics;
    extern uint32 Statistics_Seconds_Since_Reset;
    // Variables from Fault.
    #include "Fault.h"
    extern FAULT_ID_TYPE Fault_Id[];
    extern FAULT_ID_TYPE Prefault_Id[];
    // Other Variables waiting for definition or changes on the declaration.
    extern SYSTEM_STATE_TYPE System_State[];
#elif (SYSTEM_HMI_HANDLING == ENABLED)
    extern uint8 Acu_Cycle[NUM_OF_COMPARTMENTS];
    extern uint8 Acu_Phase[NUM_OF_COMPARTMENTS];
    extern uint8 Acu_Step[NUM_OF_COMPARTMENTS];
    extern uint16 Acu_Step_Time_Elapsed[NUM_OF_COMPARTMENTS];
    extern uint8 Acu_Sub_Cycle[NUM_OF_COMPARTMENTS];
    extern uint8 Acu_Sub_Phase[NUM_OF_COMPARTMENTS];
    extern uint8 Acu_Sub_Cycle_Step[NUM_OF_COMPARTMENTS];
    extern uint32 Acu_Statistics_Seconds;
    extern uint16 Acu_Statistics_Cycles_Completed;
    static const BOOL_TYPE Always_True = TRUE;
    extern uint16 Acu_Variable_General_Purpose[VARIABLE_NUMBER_OF_GENERAL_PURPOSE];
    extern uint8 Acu_Variable_General_Flags;
    extern uint8 Acu_Prefault_Id[NUM_OF_COMPARTMENTS];
    extern uint8 Acu_Fault_Id[NUM_OF_COMPARTMENTS];
    extern uint8 Acu_Supervisor_State[NUM_OF_COMPARTMENTS];
#endif


/**
 * This is the array that contains the information about the variables you're exposing.
 */
static VARIABLE_TYPE const ACU_DATA_HOLDER[] =
{
    /*  Address                                     Length                              Type                        RW  */
    { (void*)(&Regulation_Dummy)                    ,1                                  ,DATATYPE_IS_REGULATION     ,FALSE  }   // VARIABLE_REGULATION_HUB
#if (SYSTEM_ACU_HANDLING == ENABLED)
    ,{ &Interpreter_Status.Main_Cycle[0]            ,APPLIANCE_MAX_COMPARTMENTS         ,DATATYPE_UINT8             ,FALSE  }   // VARIABLE_CYCLE
    ,{ &Interpreter_Status.Main_Phase[0]            ,APPLIANCE_MAX_COMPARTMENTS         ,DATATYPE_UINT8             ,FALSE  }   // VARIABLE_PHASE
    ,{ &Interpreter_Status.Main_Step[0]             ,APPLIANCE_MAX_COMPARTMENTS         ,DATATYPE_UINT8             ,FALSE  }   // VARIABLE_STEP
    ,{ &Interpreter_Status.Step_Time_Elapsed[0]     ,APPLIANCE_MAX_COMPARTMENTS         ,DATATYPE_UINT16            ,FALSE  }   // VARIABLE_STEP_SECONDS
    ,{ &Interpreter_Status.Sub_Cycle[0]             ,APPLIANCE_MAX_COMPARTMENTS         ,DATATYPE_UINT8             ,FALSE  }   // VARIABLE_SUB_CYCLE
    ,{ &Interpreter_Status.Sub_Phase[0]             ,APPLIANCE_MAX_COMPARTMENTS         ,DATATYPE_UINT8             ,FALSE  }   // VARIABLE_SUB_PHASE
    ,{ &Interpreter_Status.Sub_Cycle_Step[0]        ,APPLIANCE_MAX_COMPARTMENTS         ,DATATYPE_UINT8             ,FALSE  }   // VARIABLE_SUB_STEP
    ,{ &Statistics_Seconds_Since_Reset              ,1                                  ,DATATYPE_UINT32            ,FALSE  }   // VARIABLE_SECONDS_SINCE_RESET
    ,{ &Statistics.Lifetime.Cycles_Completed        ,1                                  ,DATATYPE_UINT16            ,FALSE  }   // VARIABLE_CYCLES_COMPLETED
    ,{ (void*)(&Always_True)                        ,1                                  ,DATATYPE_UINT8             ,FALSE  }   // VARIABLE_ALWAYS_TRUE
    ,{ &Variable_General_Purpose[0]                 ,VARIABLE_NUMBER_OF_GENERAL_PURPOSE ,DATATYPE_UINT16            ,TRUE   }   // VARIABLE_USER_WORD
    ,{ &Variable_General_Flags                      ,1                                  ,DATATYPE_UINT8             ,TRUE   }   // VARIABLE_USER_FLAGS
    ,{ &Prefault_Id[0]                     		    ,APPLIANCE_MAX_COMPARTMENTS 	    ,DATATYPE_UINT8             ,FALSE  }   // VARIABLE_PREFAULT
    ,{ &Fault_Id[0]                     		    ,APPLIANCE_MAX_COMPARTMENTS         ,DATATYPE_UINT8             ,FALSE  }   // VARIABLE_FAULT
    ,{ &System_State[0]                             ,APPLIANCE_MAX_COMPARTMENTS		    ,DATATYPE_UINT8             ,FALSE  }   // VARIABLE_SYSTEM_STATE
#elif (SYSTEM_HMI_HANDLING == ENABLED)
    ,{ &Acu_Cycle                                   ,NUM_OF_COMPARTMENTS                ,DATATYPE_UINT8             ,FALSE  }   // VARIABLE_CYCLE
    ,{ &Acu_Phase                                   ,NUM_OF_COMPARTMENTS                ,DATATYPE_UINT8             ,FALSE  }   // VARIABLE_PHASE
    ,{ &Acu_Step                                    ,NUM_OF_COMPARTMENTS                ,DATATYPE_UINT8             ,FALSE  }   // VARIABLE_STEP
    ,{ &Acu_Step_Time_Elapsed                       ,NUM_OF_COMPARTMENTS                ,DATATYPE_UINT16            ,FALSE  }   // VARIABLE_STEP_SECONDS
    ,{ &Acu_Sub_Cycle                               ,NUM_OF_COMPARTMENTS                ,DATATYPE_UINT8             ,FALSE  }   // VARIABLE_SUB_CYCLE
    ,{ &Acu_Sub_Phase                               ,NUM_OF_COMPARTMENTS                ,DATATYPE_UINT8             ,FALSE  }   // VARIABLE_SUB_PHASE
    ,{ &Acu_Sub_Cycle_Step                          ,NUM_OF_COMPARTMENTS                ,DATATYPE_UINT8             ,FALSE  }   // VARIABLE_SUB_STEP
    ,{ &Acu_Statistics_Seconds                      ,1                                  ,DATATYPE_UINT32            ,FALSE  }   // VARIABLE_SECONDS_SINCE_RESET
    ,{ &Acu_Statistics_Cycles_Completed             ,1                                  ,DATATYPE_UINT16            ,FALSE  }   // VARIABLE_CYCLES_COMPLETED
    ,{ (void*)(&Always_True)                        ,1                                  ,DATATYPE_UINT8             ,FALSE  }   // VARIABLE_ALWAYS_TRUE
    ,{ &Acu_Variable_General_Purpose[0]             ,VARIABLE_NUMBER_OF_GENERAL_PURPOSE ,DATATYPE_UINT16            ,TRUE   }   // VARIABLE_USER_WORD
    ,{ &Acu_Variable_General_Flags                  ,1                                  ,DATATYPE_UINT8             ,TRUE   }   // VARIABLE_USER_FLAGS
    ,{ &Acu_Prefault_Id[0]                          ,NUM_OF_COMPARTMENTS                ,DATATYPE_UINT8             ,FALSE  }   // VARIABLE_PREFAULT
    ,{ &Acu_Fault_Id[0]                             ,NUM_OF_COMPARTMENTS                ,DATATYPE_UINT8             ,FALSE  }   // VARIABLE_FAULT
    ,{ &Acu_Supervisor_State[0]                     ,NUM_OF_COMPARTMENTS                ,DATATYPE_UINT8             ,FALSE  }   // VARIABLE_SYSTEM_STATE
#endif
#ifdef ACU_CATEGORY_VARIABLE_LIST
    ,ACU_CATEGORY_VARIABLE_LIST
#endif
};

static VARIABLE_TYPE const HMI_DATA_HOLDER[] =
{
    /*  Address                     Length     Type                     RW  */
#ifdef HMI_CATEGORY_VARIABLE_LIST
    HMI_CATEGORY_VARIABLE_LIST,
#else
    {NULL,                          0,         DATATYPE_INVALID,        FALSE  }
#endif
};

/**
 * This is the translation table to be used by the getters and setters to access variables defined by the KVP_ID.
 */
#if (VARIABLES_HAVE_KVP_ID == ENABLED)
	static KVP_TO_VARIABLE_TRANSLATION_TYPE const Kvp_To_Variable_Table[] = KVP_TO_VARIABLE_TRANSLATION_TABLE;
#endif

/**
 * Hmi specific Variables, they are not exchanged with ACU
 */
#if(VISUAL_MACRO_VARIABLES_FEATURE == ENABLED)
	//!Extern HMI Variables
	extern uint32 Hmi_Timer_Buffer[HMI_NUM_OF_TIMERS];
	extern sint32 Hmi_Variable_Buffer[HMI_NUM_OF_VARIABLES];
	extern uint8 Hmi_Time_Estimation_Profile[NUM_OF_COMPARTMENTS];

	//! An array that contains all HmiVariables.
	//! New variables should be added on TOP not at the bottom.
    static VARIABLE_TYPE const VISUAL_MACRO_VARIABLES_DATA_HOLDER[] =
    {
        /*  ==============================   Add new variables to the TOP ==============================================*/
        /*  Address                                 Length                              Type                        RW  */
        { (void*)(&Hmi_Time_Estimation_Profile)     ,NUM_OF_COMPARTMENTS                ,DATATYPE_UINT8             ,TRUE  }
        ,{ (void*)(&Hmi_Variable_Buffer)            ,HMI_NUM_OF_VARIABLES               ,DATATYPE_SINT32            ,TRUE  }
        ,{(void*)(&Hmi_Timer_Buffer)                ,HMI_NUM_OF_TIMERS                  ,DATATYPE_UINT32            ,TRUE  }
    };
#endif

#endif // VARIABLE_DEFS_H_

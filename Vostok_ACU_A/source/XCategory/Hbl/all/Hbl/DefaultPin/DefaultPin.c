/**
 *  @file       DefaultPin.c
 *
 *  @brief      Hbl Acu/Hmi module to refresh default configuration of unused pins from Setting file
 *
 *  @details    This module gets a list of pins and their configuration to be applied.
 *                  TO ENABLE THIS FEATURE apply these changes in SRMain_prv.h:
 *                  - Include DefaultPin.h
 *                  - Add DefaultPin__Initialize in SRMainNormalInitialization_Tasks function list, e.g. after Hbl__Initialize
 *                  - Add DefaultPin__Handler in SRMain_AppMgr_Handler function list
 *  			Compile time parameters (it can bel placed on SystemConfig_prm.h):
 *  			- DEFAULTPIN_REFRESH_COUNT: if set, pin refresh is performed after DEFAULTPIN_REFRESH_COUNT*100ms time.
 *  			    Default is 0 (100ms)
 *  			- DEFAULTPIN_ACU_EXP_SECTION: it should be defined explicitly if SYSTEM_ACU_EXPANSION_HANDLING is ENABLED and
 *  			    SFUPDATER_ACU_SECTION_ID is not defined; if SYSTEM_ACU_EXPANSION_HANDLING is DISABLED this feature is disabled.
 *                  If DEFAULTPIN_ACU_EXP_SECTION is not defined and SFUPDATER_ACU_SECTION_ID is, feature is enabled using SFUPDATER_ACU_SECTION_ID value.
 *  			    If both DEFAULTPIN_ACU_EXP_SECTION and SFUPDATER_ACU_SECTION_ID are not defined, this feature is disabled.
 *              - DEFAULTPIN_HMI_EXP_SECTION: it should be defined explicitly if SYSTEM_HMI_EXPANSION_HANDLING is ENABLED and
 *                  SFUPDATER_HMI_SECTION_ID is not defined; if SYSTEM_HMI_EXPANSION_HANDLING is DISABLED this feature is disabled.
 *                  If DEFAULTPIN_HMI_EXP_SECTION is not defined and SFUPDATER_HMI_SECTION_ID is, feature is enabled using SFUPDATER_HMISECTION_ID value.
 *                  If both DEFAULTPIN_HMI_EXP_SECTION and SFUPDATER_HMI_SECTION_ID are not defined, this feature is disabled.
 *
 *  @copyright  Copyright 2019 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "SystemConfig.h"
#include "DefaultPin.h"
#include "SettingFile.h"
#include "VirtualPin.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define SF_DEFAULT_PIN_DISPLACEMENT         7

#ifndef DEFAULTPIN_REFRESH_COUNT
    #define DEFAULTPIN_REFRESH_COUNT        0
#endif

typedef PACKED struct DEFAULT_PIN_PARAM_STRUCT
{
    uint8   Pin;
    uint8   Gpio_Type:7;
    uint8   Is_High:1;
} DEFAULT_PIN_PARAM_TYPE;

#if (SYSTEM_ACU_HANDLING == ENABLED)
    // Reference for ACU DefaultPin configuration from Setting File
    static SETTINGFILE_LOADER_TYPE DefaultPin_Acu_Ptr;
#endif
#if (SYSTEM_HMI_HANDLING == ENABLED)
    // Reference for HMI DefaultPin configuration from Setting File
    static SETTINGFILE_LOADER_TYPE DefaultPin_Hmi_Ptr;
#endif
#if (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED)
    #if (!defined(DEFAULTPIN_ACU_EXP_SECTION) && defined(SFUPDATER_ACU_SECTION_ID))
        #define DEFAULTPIN_ACU_EXP_SECTION      SFUPDATER_ACU_SECTION_ID
    #endif
    #if (defined(DEFAULTPIN_ACU_EXP_SECTION))
        // Reference for ACU Expansion DefaultPin configuration from Setting File
        static SETTINGFILE_LOADER_TYPE DefaultPin_AcuExp_Ptr;
    #endif
#endif
#if (SYSTEM_HMI_EXPANSION_HANDLING == ENABLED)
    #if (!defined(DEFAULTPIN_HMI_EXP_SECTION) && defined(SFUPDATER_HMI_SECTION_ID))
        #define DEFAULTPIN_HMI_EXP_SECTION      SFUPDATER_HMI_SECTION_ID
    #endif
    #if (defined(DEFAULTPIN_HMI_EXP_SECTION))
        // Reference for HMI Expansion DefaultPin configuration from Setting File
        static SETTINGFILE_LOADER_TYPE DefaultPin_HmiExp_Ptr;
    #endif
#endif

#if(DEFAULTPIN_REFRESH_COUNT > 1)
    // Counter to wait DEFAULTPIN_REFRESH_COUNT times before Pin config update
    static uint16 DefaultPin_Exec_Counter;
    #define DEFAULTPIN__IS_COUNTER_EXPIRED()        ( DefaultPin_Exec_Counter >= DEFAULTPIN_REFRESH_COUNT )
    #define DEFAULTPIN__INITIALIZE_COUNTER()        { DefaultPin_Exec_Counter = DEFAULTPIN_REFRESH_COUNT; }
    #define DEFAULTPIN__INCREMENT_COUNTER()         { DefaultPin_Exec_Counter++; }
    #define DEFAULTPIN__CLEAR_COUNTER()             { DefaultPin_Exec_Counter = 0; }

#else
    #define DEFAULTPIN__IS_COUNTER_EXPIRED()        (TRUE)
    #define DEFAULTPIN__INITIALIZE_COUNTER()        {}
    #define DEFAULTPIN__INCREMENT_COUNTER()         {}
    #define DEFAULTPIN__CLEAR_COUNTER()             {}
#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void DoPinUpdate(DEFAULT_PIN_PARAM_TYPE *param_ptr, uint16 n_items);
static void InitializePtr(SETTINGFILE_LOADER_TYPE *ptr, SETTINGFILE_OFFSET_TYPE sf_ptr);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      DefaultPin Initialize method; it calls DefaultPin__Handler, no other feature are required.
 *
 */
void DefaultPin__Initialize(void)
{
    #if (SYSTEM_ACU_HANDLING == ENABLED)
        InitializePtr(&DefaultPin_Acu_Ptr, SF_PTR_ACU_IO_CONFIG);
    #endif
    #if (SYSTEM_HMI_HANDLING == ENABLED)
        InitializePtr(&DefaultPin_Hmi_Ptr, SF_PTR_UI_IO_CONFIG);
    #endif
    #if ((SYSTEM_ACU_EXPANSION_HANDLING == ENABLED) && defined(DEFAULTPIN_ACU_EXP_SECTION))
        InitializePtr(&DefaultPin_AcuExp_Ptr,
        	(SETTINGFILE_OFFSET_TYPE) ((DEFAULTPIN_ACU_EXP_SECTION << 8) | (SF_PTR_ACU_IO_CONFIG & 0xFF)));
    #endif
    #if ((SYSTEM_HMI_EXPANSION_HANDLING == ENABLED) && defined(DEFAULTPIN_HMI_EXP_SECTION))
        InitializePtr(&DefaultPin_HmiExp_Ptr,
        	(SETTINGFILE_OFFSET_TYPE) ((DEFAULTPIN_HMI_EXP_SECTION << 8) | (SF_PTR_UI_IO_CONFIG & 0xFF)));
    #endif
    DEFAULTPIN__INITIALIZE_COUNTER();
    DefaultPin__Handler();
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Handler to refresh Default pin configuration
 *
 */
void DefaultPin__Handler(void)
{
    DEFAULTPIN__INCREMENT_COUNTER();
    if(DEFAULTPIN__IS_COUNTER_EXPIRED())
    {
        #if (SYSTEM_ACU_HANDLING == ENABLED)
            // Check pins on ACU I/O Configuration pointer
            if(DefaultPin_Acu_Ptr.Data != NULL)
            {
                DoPinUpdate((void *) DefaultPin_Acu_Ptr.Data, DefaultPin_Acu_Ptr.Length);
            }
        #endif
        #if (SYSTEM_HMI_HANDLING == ENABLED)
            // Check pins on HMI I/O Configuration pointer
            if(DefaultPin_Hmi_Ptr.Data != NULL)
            {
                DoPinUpdate((void *) DefaultPin_Hmi_Ptr.Data, DefaultPin_Hmi_Ptr.Length);
            }
        #endif
        #if ((SYSTEM_ACU_EXPANSION_HANDLING == ENABLED) && defined(DEFAULTPIN_ACU_EXP_SECTION))
            // Check pins on ACU Expansion I/O Configuration pointer
            if(DefaultPin_AcuExp_Ptr.Data != NULL)
            {
                DoPinUpdate((void *) DefaultPin_AcuExp_Ptr.Data, DefaultPin_AcuExp_Ptr.Length);
            }
        #endif
        #if ((SYSTEM_HMI_EXPANSION_HANDLING == ENABLED) && defined(DEFAULTPIN_HMI_EXP_SECTION))
            // Check pins on HMI Expansion I/O Configuration pointer
            if(DefaultPin_HmiExp_Ptr.Data != NULL)
            {
                DoPinUpdate((void *) DefaultPin_HmiExp_Ptr.Data, DefaultPin_HmiExp_Ptr.Length);
            }
        #endif
        DEFAULTPIN__CLEAR_COUNTER();
    }
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Function that perform Gpio pin configuration and set level for Ouptut pin according to Pin list provided.
 *  @param      param_ptr - Pointer to Setting file parameters to process
 *  @param      n_items - Number of elements of Parameters list to process
 *
 */
void DoPinUpdate(DEFAULT_PIN_PARAM_TYPE *param_ptr, uint16 n_items)
{
    uint16 index;
    for(index = 0; index < n_items; index++)
    {
        DEFAULT_PIN_PARAM_TYPE *cur_param_ptr = &param_ptr[index];
        if(cur_param_ptr->Pin < VirtualPin__GetNumOf())
        {
            VIRTUALPIN_TYPE *pin_ptr = (void *) &VIRTUALPIN_TABLE[cur_param_ptr->Pin];

            // Check an entry where Port is not set
            if(pin_ptr->port != PORT_NULL)
            {
            	// Set Pin value if an Output Type is set
                if((cur_param_ptr->Gpio_Type == OUTPUT_PUSHPULL) ||
                    (cur_param_ptr->Gpio_Type == OUTPUT_OPEN_DRAIN) ||
                    (cur_param_ptr->Gpio_Type == SPECIAL_FUNCTION_PUSH_PULL) ||
                    (cur_param_ptr->Gpio_Type == SPECIAL_FUNCTION_OPEN_DRAIN))
                {
                	Gpio__PinWrite(pin_ptr->port, pin_ptr->pin, (BOOL_TYPE) cur_param_ptr->Is_High);
                }
                Gpio__PinConfig(pin_ptr->port, pin_ptr->pin,(GPIO_PIN_MODE_TYPE) cur_param_ptr->Gpio_Type);
            }
        }
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      Function that initializes Setting file pointer
 *  @param      ptr - Pointer to Settingfile Loader structure to save parameter table or initialize it as NULL if not present
 *  @param      sf_section - Setting file section where to get Default Pin parameters table
 *
 */
void InitializePtr(SETTINGFILE_LOADER_TYPE *ptr, SETTINGFILE_OFFSET_TYPE sf_ptr)
{
    if(SettingFile__BasicLoader(sf_ptr, SF_DEFAULT_PIN_DISPLACEMENT, ptr) == PASS)
    {
        // Use Length field to keep number of elements
        ptr->Length /= sizeof(DEFAULT_PIN_PARAM_TYPE);
    }
    else
    {
        ptr->Data = NULL;
        ptr->Length = 0;
    }
}

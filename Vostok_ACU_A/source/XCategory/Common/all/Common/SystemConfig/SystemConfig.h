/**
 *  @file
 *
 *  @brief      Public declarations to configure features by project type.
 *
 *  @details    The following list describes all selectable features:
 *
 *  API005_CONTROL_CYCLES_FEATURE                   Allows API005 to control appliance cycles
 *  API005_USE_LOADS_AND_GIS_FEATURE                Allows API005 to set and get Loads and GIs through Hbl
 *  API005_USE_OBJECTS_AND_EVENTS_FEATURE           Allows API005 to set objects and get events through HblHmi
 *  API005LOWLEVEL_FEATURE                          Provides PC control of the board I/O
 *  API006HMI_FEATURE                               Allows an external tool to Simulate GI events, MEVP events and to receive information about HMI outputs like leds status
 *  API007DATA_FEATURE                              Used for data transfer -- including setting files from ACU to other nodes
 *  API010_USES_FIXED_VARIABLES_FEATURE             Indicates that API010 uses the Variables module for fixed variables.
 *  API010POLLVAR_FEATURE                           Reports platform variables and memory mapped variables over WIN.
 *  API011APPCTRL_FEATURE                           Supports Nucleus modes between boards. Supports regulations & status as needed.
 *  API013_CLIENT_FEATURE                           Controls application-layer functions on remote boards.
 *  API013_SERVER_FEATURE                           Provides application-layer functions through communications.
 *  API014EXPHMI_FEATURE                            Provides communication between Main HMI and Exp HMIs
 *  API018_CLIENT_FEATURE                           Requests fault information and can generate a fault.
 *  API018_SERVER_FEATURE                           Publishes fault information and stores it in nonvolatile memory.
 *  API019_BOARD_DATA_FEEDBACK                      Indicates that the board supports API019 board data feedback for displaying on an HMI.
 *  API201_DEVICE_SECURITY_FEATURE                  Provides ability to secure (lock/unlock) Android based HMI devices.
 *  API220MOTORCONTROL_FEATURE                      Enables API220 functionality on motor control boards with MCI.
 *  API221_CLIENT_FEATURE                           Sends motor control messages and receives motor status messages.
 *  API221_SERVER_FEATURE                           Receives motor control messages and sends motor status messages.
 *  BACKGROUNDTHREADS_FEATURE                       Enables the Background feature to perform execution in a separate thread
 *  CALIBRATION_DATA_FEATURE                        Uses Calibration Data to calibrate some GIs on FVT/EOL using API019
 *  COMMUNICATION_MONITOR_FEATURE                   Monitors the Communication faults
 *  CONNECTIVITY_FEATURE                            Enables connectivity data model, API147, and API151.
 *  CONNECTIVITY_HMI_FEATURE                        Enables API144 and API141.  Requires CONNECTIVITY_FEATURE ENABLED.
 *  FAST_HANDLER_FEATURE                            Enables the Fast Handler feature (e.g. 200us) - disabled by default
 *  FAST_HANDLER_TIMER                              Configures the TIMER peripheral to use for Fast Handler feature
 *  FAST_HANDLER_TIMER_CHANNEL                      Configures the TIMER_CHANNEL peripheral to use for Fast Handler feature
 *  FAST_HANDLER_TIMING_USEC                        Configure the Fast Handler timing rate - default 200us
 *  FAULT_MANAGEMENT_FEATURE                        Enables firmware that depends on the Fault module to track error conditions and report faults.
 *  HBLACU_ZEROCROSS_FEATURE                        Enables Zerocross handling on HblAcu (Default: Enabled with SYSTEM_ACU_HANDLING defined, Disabled for other cases)
 *  HBLHMI_LLI_INITIALIZATION_FEATURE               Enables the initialization of LLIs in the HblHmi module (Disabled for P1)
 *  HMI_ACUVARIABLE_MODULE_FEATURE                  Enables the module that declares ACU variables instances for a P3 HMI
 *  HMI_RECIPE_ENGINE_FEATURE                       Enables the HMI Recipe Engine for running cycles from the HMI
 *  HMI_VARIABLE_FEATURE                            Enable the module to get Configuration of HMI Variables using API010
 *  MEVP_HMI_FRAMEWORK_FEATURE                      Enables the usage of the MEVP Architecture for HMI projects.MEVP is for: Model,Event,View,Presenter.
 *  NVDATA_STORE_APPLIANCE_AND_STATISTICS_FEATURE   Allows management of the Appliance and Statistics segment data using the NVData module
 *  NVDATA_STORE_STATIC_FEATURE                     Allows API019 to store product, calibration, FVT, and EOL static nonvolatile data.
 *  RECEIVE_STATUS_VARIABLES_FEATURE                Allows status variables to be updated over API011
 *  SETTINGFILE_MASTER_FILE_FEATURE                 Indicates that the board contains the master setting file for the system.
 *  SETTINGFILE_SECTION_DOWNLOAD_FEATURE            Indicates that the board will download a section of the setting file from the Main ACU.
 *  SFUPDATER_ACU_SECTION_ID                        The SF_ID_TYPE for the ACU section of the setting file. For ACU Expansion boards.
 *  SFUPDATER_HMI_SECTION_ID                        The SF_ID_TYPE for the HMI section of the setting file. For HMI and HMI Expansion boards.
 *  SFUPDATER_MCU_SECTION_ID                        The SF_ID_TYPE for the MCU section of the setting file. For MUC boards.
 *  SW05_SOFTWARE_TESTS_FEATURE                     SW05 related tests can be done. Applies to all type of boards
 *  SYSTEM_EXPANSION_HANDLING                       Enables use of communication with an Expansion - it can be ENABLED if only one of Main ACU or HMI are ENABLED. Default is DISABLED.
 *  SYSTEM_TIMERS_FEATURE                           Enables the usage of the SystemTimers module. Can be disabled if only Millis module is used.
 *  TIME_ESTIMATION_FEATURE                         Enables cycle time estimation -- provides an Estimated_Remaining_Time[] variable.
 *  VARIABLES_REGULATIONS_FEATURE                   Enables the ability to read regulations through the Variable module.
 *  VISUAL_MACRO_VARIABLES_FEATURE                  Defines if Variable module handles Visual macro Variables indexes
 *  ZEROCROSS_POLLING_MODE                          Enables Polling mode for ZeroCross - it requires that FAST_HANDLER_FEATURE is ENABLED
 *
 *  Example Project Types:
 *  These are set in the SystemConfig_prm.h file, but shown here for reference.
 *
 *      ACU Main and HMI Main (P1 board) - Not controlling expansions
 *          #define SYSTEM_ACU_HANDLING             ENABLED
 *          #define SYSTEM_HMI_HANDLING             ENABLED
 *          #define SYSTEM_ACU_EXPANSION_HANDLING   DISABLED
 *          #define SYSTEM_HMI_EXPANSION_HANDLING   DISABLED
 *          #define SYSTEM_EXPANSION_HANDLING       DISABLED
 *
 *      ACU Main - Controlling an ACU expansion
 *          #define SYSTEM_ACU_HANDLING             ENABLED
 *          #define SYSTEM_HMI_HANDLING             DISABLED
 *          #define SYSTEM_ACU_EXPANSION_HANDLING   DISABLED
 *          #define SYSTEM_HMI_EXPANSION_HANDLING   DISABLED
 *          #define SYSTEM_EXPANSION_HANDLING       ENABLED
 *
 *      ACU Expansion
 *          #define SYSTEM_ACU_HANDLING             DISABLED
 *          #define SYSTEM_HMI_HANDLING             DISABLED
 *          #define SYSTEM_ACU_EXPANSION_HANDLING   ENABLED
 *          #define SYSTEM_HMI_EXPANSION_HANDLING   DISABLED
 *          #define SYSTEM_EXPANSION_HANDLING       DISABLED
 *          #define SFUPDATER_ACU_SECTION_ID        SF_ID_ACU_EXP_1
 *
 *      HMI Main - Not controlling expansions
 *          #define SYSTEM_ACU_HANDLING             DISABLED
 *          #define SYSTEM_HMI_HANDLING             ENABLED
 *          #define SYSTEM_ACU_EXPANSION_HANDLING   DISABLED
 *          #define SYSTEM_HMI_EXPANSION_HANDLING   DISABLED
 *          #define SYSTEM_EXPANSION_HANDLING       DISABLED
 *
 *      HMI Main with ACU Expansion, controlling an HMI expansion
 *          #define SYSTEM_ACU_HANDLING             DISABLED
 *          #define SYSTEM_HMI_HANDLING             ENABLED
 *          #define SYSTEM_ACU_EXPANSION_HANDLING   ENABLED
 *          #define SYSTEM_HMI_EXPANSION_HANDLING   DISABLED
 *          #define SYSTEM_EXPANSION_HANDLING       ENABLED
 *
 *      HMI Main - Controlling an HMI Expansion
 *          #define SYSTEM_ACU_HANDLING             DISABLED
 *          #define SYSTEM_HMI_HANDLING             ENABLED
 *          #define SYSTEM_ACU_EXPANSION_HANDLING   DISABLED
 *          #define SYSTEM_HMI_EXPANSION_HANDLING   DISABLED
 *          #define SYSTEM_EXPANSION_HANDLING       ENABLED
 *
 *      HMI Expansion
 *          #define SYSTEM_ACU_HANDLING             DISABLED
 *          #define SYSTEM_HMI_HANDLING             DISABLED
 *          #define SYSTEM_ACU_EXPANSION_HANDLING   DISABLED
 *          #define SYSTEM_HMI_EXPANSION_HANDLING   ENABLED
 *          #define SYSTEM_EXPANSION_HANDLING       DISABLED
 *          #define SFUPDATER_HMI_SECTION_ID        SF_ID_HMI_EXP_1
 *
 *  @copyright  Copyright 2016-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SYSTEMCONFIG_H
	#define SYSTEMCONFIG_H

#include "C_Extensions.h"
#include "SRSystemConfig.h"
#include "SystemConfig_prm.h"


#ifndef SYSTEM_ACU_HANDLING
    #error SYSTEM_ACU_HANDLING must be defined as either ENABLED or DISABLED in the SystemConfig_prm.h file.
#endif
#ifndef SYSTEM_HMI_HANDLING
    #error SYSTEM_HMI_HANDLING must be defined as either ENABLED or DISABLED in the SystemConfig_prm.h file.
#endif
#ifndef SYSTEM_ACU_EXPANSION_HANDLING
    #error SYSTEM_ACU_EXPANSION_HANDLING must be defined as either ENABLED or DISABLED in the SystemConfig_prm.h file.
#endif
#ifndef SYSTEM_HMI_EXPANSION_HANDLING
    #error SYSTEM_HMI_EXPANSION_HANDLING must be defined as either ENABLED or DISABLED in the SystemConfig_prm.h file.
#endif
#ifndef SYSTEM_MCU_HANDLING
    #error SYSTEM_MCU_HANDLING must be defined as either ENABLED or DISABLED in the SystemConfig_prm.h file.
#endif


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================


//---------------------------------------------------------------------------------------------------------------------
// Public Declarations that are not dependent on the type of project (ACU, HMI, MCU, etc.)
//---------------------------------------------------------------------------------------------------------------------


// Enable/Disable communication features
#define API003DISC_FEATURE                              ENABLED
#define API009SYSTEM_FEATURE                            ENABLED
#define API019PRODUCTINFO_FEATURE                       ENABLED
#if ((SYSTEM_ACU_HANDLING != ENABLED) && (SYSTEM_HMI_HANDLING != ENABLED))
    #define SYSTEM_EXPANSION_HANDLING                   DISABLED
#endif

// Enable/Disable other features
#ifndef CALIBRATION_DATA_FEATURE
    #define CALIBRATION_DATA_FEATURE                    DISABLED
#endif
#ifndef I2CMGR_FEATURE
    #define I2CMGR_FEATURE                              DISABLED
#endif
#ifndef BACKGROUNDTHREADS_FEATURE
    #define BACKGROUNDTHREADS_FEATURE                   DISABLED
#endif
#ifndef SYSTEM_TIMERS_FEATURE
    #define SYSTEM_TIMERS_FEATURE                       ENABLED
#endif

//---------------------------------------------------------------------------------------------------------------------
// Public Declarations that are dependent on the type of project (ACU, HMI, MCU, etc.)
//---------------------------------------------------------------------------------------------------------------------


#if (SYSTEM_ACU_HANDLING == ENABLED) && (SYSTEM_HMI_HANDLING == ENABLED)

    // The following configuration is for a board that is both the main ACU and the main HMI (P1).
    #if ((SYSTEM_ACU_EXPANSION_HANDLING == ENABLED) || (SYSTEM_HMI_EXPANSION_HANDLING == ENABLED))
        #error A P1 board cannot act as an expansion board.
    #endif
    // Check for the following declarations below, if necessary.
    //      SYSTEM_EXPANSION_HANDLING may be ENABLED or DISABLED.

    // Enable/Disable communication features
	#ifndef API004DEBUG_FEATURE
        #define API004DEBUG_FEATURE                         ENABLED
    #endif
    #define API005LOWLEVEL_FEATURE                          ENABLED
    #define API005_CONTROL_CYCLES_FEATURE                   ENABLED
    #define API005_USE_LOADS_AND_GIS_FEATURE                ENABLED
    #define API005_USE_OBJECTS_AND_EVENTS_FEATURE           ENABLED
    #ifndef API006HMI_FEATURE
    #define API006HMI_FEATURE                               DISABLED
    #endif
    #define API007DATA_FEATURE                              ENABLED
    #define API010POLLVAR_FEATURE                           ENABLED
    #define API010_USES_FIXED_VARIABLES_FEATURE             ENABLED
    #define API011APPCTRL_FEATURE                           ENABLED
    #define API012EXP_FEATURE                               SYSTEM_EXPANSION_HANDLING
    #ifndef API013_CLIENT_FEATURE
        #define API013_CLIENT_FEATURE                       DISABLED
    #endif  
    #ifndef API013_SERVER_FEATURE
        #define API013_SERVER_FEATURE                       DISABLED
    #endif
    #if (SYSTEM_EXPANSION_HANDLING == ENABLED)
        #ifndef API014EXPHMI_FEATURE
            #define API014EXPHMI_FEATURE                    ENABLED
        #endif
    #else
        #define API014EXPHMI_FEATURE                        DISABLED
    #endif

    #ifndef API017TOUCH_FEATURE
        #define API017TOUCH_FEATURE                         DISABLED
    #endif
    #define API018_CLIENT_FEATURE                           ENABLED
    #define API018_SERVER_FEATURE                           ENABLED
    #ifndef API019_BOARD_DATA_FEEDBACK
        #define API019_BOARD_DATA_FEEDBACK                  DISABLED
    #endif
    #ifndef API201_DEVICE_SECURITY_FEATURE
        #define API201_DEVICE_SECURITY_FEATURE              DISABLED
    #endif
    #ifndef API220MOTORCONTROL_FEATURE
        #define API220MOTORCONTROL_FEATURE                  DISABLED
    #endif
    #ifndef API221_CLIENT_FEATURE
        #define API221_CLIENT_FEATURE                       DISABLED
    #endif
    #ifndef API221_SERVER_FEATURE
        #define API221_SERVER_FEATURE                       DISABLED
    #endif
    #ifndef COMMUNICATION_MONITOR_FEATURE
        #define COMMUNICATION_MONITOR_FEATURE               DISABLED
    #endif
    #ifndef CONNECTIVITY_HMI_FEATURE
        #define CONNECTIVITY_HMI_FEATURE                    DISABLED
    #endif

    // Enable/Disable other features
    #define FAULT_MANAGEMENT_FEATURE                        ENABLED
    #ifndef HBLACU_ZEROCROSS_FEATURE
        #define HBLACU_ZEROCROSS_FEATURE                    ENABLED
    #endif
    #define HBLHMI_LLI_INITIALIZATION_FEATURE               DISABLED
    #define HMI_ACUVARIABLE_MODULE_FEATURE                  DISABLED
    #ifndef HMI_RECIPE_ENGINE_FEATURE
        #define HMI_RECIPE_ENGINE_FEATURE                   DISABLED
    #endif
    #ifndef HMI_VARIABLE_FEATURE
        #define HMI_VARIABLE_FEATURE                        DISABLED
    #endif
    #ifndef MEVP_HMI_FRAMEWORK_FEATURE
        #define MEVP_HMI_FRAMEWORK_FEATURE                  HMI_RECIPE_ENGINE_FEATURE
    #endif
    #define NVDATA_STORE_STATIC_FEATURE                     ENABLED
    #define NVDATA_STORE_APPLIANCE_AND_STATISTICS_FEATURE   ENABLED
    #define RECEIVE_STATUS_VARIABLES_FEATURE                DISABLED
    #define SETTINGFILE_MASTER_FILE_FEATURE                 ENABLED
    #ifndef TIME_ESTIMATION_FEATURE
        #define TIME_ESTIMATION_FEATURE                     DISABLED
    #endif
    #ifndef VARIABLES_REGULATIONS_FEATURE
        #define VARIABLES_REGULATIONS_FEATURE               ENABLED
    #endif
    #ifndef VISUAL_MACRO_VARIABLES_FEATURE
        #define VISUAL_MACRO_VARIABLES_FEATURE              ENABLED
    #endif

#elif (SYSTEM_ACU_HANDLING == ENABLED)

    // The following configuration is for a board that is the main ACU, but not the main HMI.
    #if (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED)
        #error The main ACU board cannot act as an ACU expansion board.
    #endif
    // Check for the following declarations below, if necessary.
    //      SYSTEM_EXPANSION_HANDLING may be ENABLED or DISABLED.
    //      SYSTEM_HMI_EXPANSION_HANDLING may be ENABLED or DISABLED.

    // Enable/Disable communication features
	#ifndef API004DEBUG_FEATURE
        #define API004DEBUG_FEATURE                         ENABLED
    #endif
    #define API005LOWLEVEL_FEATURE                          ENABLED
    #define API005_CONTROL_CYCLES_FEATURE                   ENABLED
    #define API005_USE_LOADS_AND_GIS_FEATURE                ENABLED
    #define API005_USE_OBJECTS_AND_EVENTS_FEATURE           DISABLED
    #define API006HMI_FEATURE                               DISABLED
    #define API007DATA_FEATURE                              ENABLED
    #define API010POLLVAR_FEATURE                           ENABLED
    #define API010_USES_FIXED_VARIABLES_FEATURE             ENABLED
    #define API011APPCTRL_FEATURE                           ENABLED
    #if (SYSTEM_EXPANSION_HANDLING == ENABLED)
        #define API012EXP_FEATURE                           ENABLED
    #elif ((SYSTEM_HMI_EXPANSION_HANDLING == ENABLED) && (API014EXPHMI_FEATURE == DISABLED))
        #define API012EXP_FEATURE                           ENABLED
    #else
        #define API012EXP_FEATURE                           DISABLED
    #endif
    #ifndef API013_CLIENT_FEATURE
        #define API013_CLIENT_FEATURE                       DISABLED
    #endif
    #ifndef API013_SERVER_FEATURE
        #define API013_SERVER_FEATURE                       DISABLED
    #endif
    #if (SYSTEM_HMI_EXPANSION_HANDLING == ENABLED)
        #ifndef API014EXPHMI_FEATURE
            #define API014EXPHMI_FEATURE                    ENABLED
        #endif
    #else
        #define API014EXPHMI_FEATURE                        DISABLED
    #endif
    #define API017TOUCH_FEATURE                             DISABLED
    #define API018_CLIENT_FEATURE                           SYSTEM_HMI_EXPANSION_HANDLING
    #define API018_SERVER_FEATURE                           ENABLED
    #ifndef API019_BOARD_DATA_FEEDBACK
        #define API019_BOARD_DATA_FEEDBACK                  DISABLED
    #endif
    #define API201_DEVICE_SECURITY_FEATURE                  DISABLED
    #ifndef API220MOTORCONTROL_FEATURE
        #define API220MOTORCONTROL_FEATURE                  DISABLED
    #endif
    #ifndef API221_CLIENT_FEATURE
        #define API221_CLIENT_FEATURE                       DISABLED
    #endif
    #ifndef API221_SERVER_FEATURE
        #define API221_SERVER_FEATURE                       DISABLED
    #endif
    #define COMMUNICATION_MONITOR_FEATURE                   ENABLED
    #define CONNECTIVITY_HMI_FEATURE                        DISABLED

    // Enable/Disable other features
    #define FAULT_MANAGEMENT_FEATURE                        ENABLED
    #ifndef HBLACU_ZEROCROSS_FEATURE
        #define HBLACU_ZEROCROSS_FEATURE                    ENABLED
    #endif
    #define HBLHMI_LLI_INITIALIZATION_FEATURE               DISABLED
    #define HMI_ACUVARIABLE_MODULE_FEATURE                  DISABLED
    #define HMI_RECIPE_ENGINE_FEATURE                       DISABLED
    #define HMI_VARIABLE_FEATURE                            DISABLED
    #define MEVP_HMI_FRAMEWORK_FEATURE                      DISABLED
    #define NVDATA_STORE_STATIC_FEATURE                     ENABLED
    #define NVDATA_STORE_APPLIANCE_AND_STATISTICS_FEATURE   ENABLED
    #define RECEIVE_STATUS_VARIABLES_FEATURE                DISABLED
    #define SETTINGFILE_MASTER_FILE_FEATURE                 ENABLED
    #ifndef TIME_ESTIMATION_FEATURE
        #define TIME_ESTIMATION_FEATURE                     DISABLED
    #endif
    #ifndef VARIABLES_REGULATIONS_FEATURE
        #define VARIABLES_REGULATIONS_FEATURE               ENABLED
    #endif
    #define VISUAL_MACRO_VARIABLES_FEATURE                  DISABLED

#elif (SYSTEM_HMI_HANDLING == ENABLED)

    // The following configuration is for a board that is the main HMI, but not the main ACU.
    #if (SYSTEM_HMI_EXPANSION_HANDLING == ENABLED)
        #error The main HMI board cannot act as an HMI expansion board.
    #endif
    #if (SYSTEM_MCU_HANDLING == ENABLED)
        #error The main HMI board cannot act as an MCU board.
    #endif
    // Check for the following declarations below, if necessary.
    //      SYSTEM_EXPANSION_HANDLING may be ENABLED or DISABLED.
    //      SYSTEM_ACU_EXPANSION_HANDLING may be ENABLED or DISABLED.


    // Enable/Disable communication features
	#ifndef API004DEBUG_FEATURE
        #define API004DEBUG_FEATURE                         ENABLED
    #endif
    #define API005LOWLEVEL_FEATURE                          ENABLED
    #define API005_CONTROL_CYCLES_FEATURE                   DISABLED
    #if (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED)
        #define API005_USE_LOADS_AND_GIS_FEATURE            ENABLED
    #else
        #ifndef API005_USE_LOADS_AND_GIS_FEATURE
            #define API005_USE_LOADS_AND_GIS_FEATURE        DISABLED
        #endif
    #endif
    #define API005_USE_OBJECTS_AND_EVENTS_FEATURE           ENABLED
    #ifndef API006HMI_FEATURE
    #define API006HMI_FEATURE                               DISABLED
    #endif
    #define API007DATA_FEATURE                              ENABLED
    #define API010POLLVAR_FEATURE                           ENABLED
    #define API010_USES_FIXED_VARIABLES_FEATURE             ENABLED
    #define API011APPCTRL_FEATURE                           ENABLED
    #if (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED)
        #define API012EXP_FEATURE                           ENABLED
    #elif ((SYSTEM_EXPANSION_HANDLING == ENABLED) && (API014EXPHMI_FEATURE == DISABLED))
        #define API012EXP_FEATURE                           ENABLED
    #else
        #define API012EXP_FEATURE                           DISABLED
    #endif
    #ifndef API013_CLIENT_FEATURE
        #define API013_CLIENT_FEATURE                       DISABLED
    #endif
    #ifndef API013_SERVER_FEATURE
        #define API013_SERVER_FEATURE                       DISABLED
    #endif
    #ifndef API014EXPHMI_FEATURE
        #if ((SYSTEM_EXPANSION_HANDLING == ENABLED)&&( API012EXP_FEATURE == DISABLED))
            #define API014EXPHMI_FEATURE                    ENABLED
        #else
            #define API014EXPHMI_FEATURE                    DISABLED
        #endif
    #endif
    #ifndef API017TOUCH_FEATURE
        #define API017TOUCH_FEATURE                         DISABLED
    #endif
    #define API018_CLIENT_FEATURE                           ENABLED
    #define API018_SERVER_FEATURE                           DISABLED
    #ifndef API019_BOARD_DATA_FEEDBACK
        #define API019_BOARD_DATA_FEEDBACK                  DISABLED
    #endif
    #ifndef API201_DEVICE_SECURITY_FEATURE
        #define API201_DEVICE_SECURITY_FEATURE              DISABLED
    #endif
    #define API220MOTORCONTROL_FEATURE                      DISABLED
    #define API221_CLIENT_FEATURE                           DISABLED
    #define API221_SERVER_FEATURE                           DISABLED
    #define COMMUNICATION_MONITOR_FEATURE                   ENABLED
    #ifndef CONNECTIVITY_HMI_FEATURE
        #define CONNECTIVITY_HMI_FEATURE                    DISABLED
    #endif

    // Enable/Disable other features
    #define FAULT_MANAGEMENT_FEATURE                        DISABLED
    #ifndef HBLACU_ZEROCROSS_FEATURE
        #define HBLACU_ZEROCROSS_FEATURE                    DISABLED
    #endif
    #define HBLHMI_LLI_INITIALIZATION_FEATURE               ENABLED
    #define HMI_ACUVARIABLE_MODULE_FEATURE                  ENABLED
    #ifndef HMI_RECIPE_ENGINE_FEATURE
        #define HMI_RECIPE_ENGINE_FEATURE                   DISABLED
    #endif
    #ifndef HMI_VARIABLE_FEATURE
        #define HMI_VARIABLE_FEATURE                        DISABLED
    #endif
    #ifndef MEVP_HMI_FRAMEWORK_FEATURE
        #define MEVP_HMI_FRAMEWORK_FEATURE                  HMI_RECIPE_ENGINE_FEATURE
    #endif
    #define NVDATA_STORE_STATIC_FEATURE                     ENABLED
    #define NVDATA_STORE_APPLIANCE_AND_STATISTICS_FEATURE   DISABLED
    #define RECEIVE_STATUS_VARIABLES_FEATURE                ENABLED
    #define SETTINGFILE_MASTER_FILE_FEATURE                 DISABLED
    #define SFUPDATER_HMI_SECTION_ID                        SF_ID_HMI
    #define TIME_ESTIMATION_FEATURE                         DISABLED
    #ifndef VARIABLES_REGULATIONS_FEATURE
        #define VARIABLES_REGULATIONS_FEATURE               ENABLED
    #endif
    #define VISUAL_MACRO_VARIABLES_FEATURE                  ENABLED


#elif (SYSTEM_MCU_HANDLING == ENABLED)

    // The following configuration is for an MCU board.

    // Enable/Disable communication features
	#ifndef API004DEBUG_FEATURE
        #define API004DEBUG_FEATURE                         DISABLED
    #endif
    #define API005LOWLEVEL_FEATURE                          DISABLED
    #define API005_CONTROL_CYCLES_FEATURE                   DISABLED
    #define API005_USE_LOADS_AND_GIS_FEATURE                DISABLED
    #define API005_USE_OBJECTS_AND_EVENTS_FEATURE           DISABLED
    #define API006HMI_FEATURE                               DISABLED
    #ifndef API007DATA_FEATURE
        #define API007DATA_FEATURE                          ENABLED
    #endif
    #ifndef API010POLLVAR_FEATURE
        #define API010POLLVAR_FEATURE                       ENABLED
    #endif
    #ifndef API010_USES_FIXED_VARIABLES_FEATURE
        #define API010_USES_FIXED_VARIABLES_FEATURE         DISABLED
    #endif
    #ifndef API011APPCTRL_FEATURE
        #define API011APPCTRL_FEATURE                       DISABLED
    #endif
    #define API012EXP_FEATURE                               DISABLED
    #ifndef API013_CLIENT_FEATURE
        #define API013_CLIENT_FEATURE                       DISABLED
    #endif
    #ifndef API013_SERVER_FEATURE
        #define API013_SERVER_FEATURE                       DISABLED
    #endif
    #define API014EXPHMI_FEATURE                            DISABLED
    #define API017TOUCH_FEATURE                             DISABLED
    #ifndef API018_CLIENT_FEATURE
        #define API018_CLIENT_FEATURE                       DISABLED
    #endif
    #define API018_SERVER_FEATURE                           DISABLED
    #define API019_BOARD_DATA_FEEDBACK                      DISABLED
    #define API201_DEVICE_SECURITY_FEATURE                  DISABLED
    #ifndef API220MOTORCONTROL_FEATURE
        #define API220MOTORCONTROL_FEATURE                  DISABLED
    #endif
    #define API221_CLIENT_FEATURE                           DISABLED
    #ifndef API221_SERVER_FEATURE
        #define API221_SERVER_FEATURE                       ENABLED
    #endif
    #define COMMUNICATION_MONITOR_FEATURE                   DISABLED
    #define CONNECTIVITY_HMI_FEATURE                        DISABLED

    // Enable/Disable other features
    #define FAULT_MANAGEMENT_FEATURE                        DISABLED
    #ifndef HBLACU_ZEROCROSS_FEATURE
        #define HBLACU_ZEROCROSS_FEATURE                    DISABLED
    #endif
    #define HMI_ACUVARIABLE_MODULE_FEATURE                  DISABLED
    #define HMI_RECIPE_ENGINE_FEATURE                       DISABLED
    #define HMI_VARIABLE_FEATURE                            DISABLED
    #define MEVP_HMI_FRAMEWORK_FEATURE                      DISABLED
    #ifndef NVDATA_STORE_STATIC_FEATURE
        #define NVDATA_STORE_STATIC_FEATURE                 DISABLED
    #endif
    #define NVDATA_STORE_APPLIANCE_AND_STATISTICS_FEATURE   DISABLED
    #define RECEIVE_STATUS_VARIABLES_FEATURE                ENABLED
    #define SETTINGFILE_MASTER_FILE_FEATURE                 DISABLED
    #define SFUPDATER_MCU_SECTION_ID                        SF_ID_MCU
    #define TIME_ESTIMATION_FEATURE                         DISABLED
    #ifndef VARIABLES_REGULATIONS_FEATURE
        #define VARIABLES_REGULATIONS_FEATURE               DISABLED
    #endif
    #define VISUAL_MACRO_VARIABLES_FEATURE                  DISABLED

#elif ((SYSTEM_ACU_EXPANSION_HANDLING == ENABLED) && (SYSTEM_HMI_EXPANSION_HANDLING == ENABLED))

    // The following configuration is for a board that expands both the main ACU and the main HMI.
    #if (SYSTEM_EXPANSION_HANDLING == ENABLED)
        #error An expansion board cannot control other expansion boards directly.
    #endif
    #if (SYSTEM_ACU_HANDLING == ENABLED)
        #error The expansion ACU board cannot act as an ACU main board.
    #endif
    #if (SYSTEM_HMI_HANDLING == ENABLED)
        #error The expansion HMI board cannot act as an HMI main board.
    #endif

    // Enable/Disable communication features
	#ifndef API004DEBUG_FEATURE
        #define API004DEBUG_FEATURE                         DISABLED
    #endif
    #define API005LOWLEVEL_FEATURE                          ENABLED
    #define API005_CONTROL_CYCLES_FEATURE                   DISABLED
    #define API005_USE_LOADS_AND_GIS_FEATURE                ENABLED
    #define API005_USE_OBJECTS_AND_EVENTS_FEATURE           ENABLED
    #define API006HMI_FEATURE                               DISABLED
    #define API007DATA_FEATURE                              ENABLED
    #define API010POLLVAR_FEATURE                           ENABLED
    #define API010_USES_FIXED_VARIABLES_FEATURE             ENABLED
    #define API011APPCTRL_FEATURE                           ENABLED
    #define API012EXP_FEATURE                               ENABLED
    #ifndef API013_CLIENT_FEATURE
        #define API013_CLIENT_FEATURE                       DISABLED
    #endif
    #ifndef API013_SERVER_FEATURE
        #define API013_SERVER_FEATURE                       DISABLED
    #endif
    #ifndef API014EXPHMI_FEATURE
        #define API014EXPHMI_FEATURE                        ENABLED
    #endif
    #ifndef API017TOUCH_FEATURE
        #define API017TOUCH_FEATURE                         DISABLED
    #endif
    #define API018_CLIENT_FEATURE                           ENABLED
    #define API018_SERVER_FEATURE                           DISABLED
    #ifndef API019_BOARD_DATA_FEEDBACK
        #define API019_BOARD_DATA_FEEDBACK                  DISABLED
    #endif
    #define API201_DEVICE_SECURITY_FEATURE                  DISABLED
    #ifndef API220MOTORCONTROL_FEATURE
        #define API220MOTORCONTROL_FEATURE                  DISABLED
    #endif
    #define API221_CLIENT_FEATURE                           DISABLED
    #ifndef API221_SERVER_FEATURE
        #define API221_SERVER_FEATURE                       DISABLED
    #endif
    #ifndef COMMUNICATION_MONITOR_FEATURE
        #define COMMUNICATION_MONITOR_FEATURE               DISABLED
    #endif
    #ifndef CONNECTIVITY_HMI_FEATURE
        #define CONNECTIVITY_HMI_FEATURE                    DISABLED
    #endif

    // Enable/Disable other features
    #define FAULT_MANAGEMENT_FEATURE                        DISABLED
    #ifndef HBLACU_ZEROCROSS_FEATURE
        #define HBLACU_ZEROCROSS_FEATURE                    DISABLED
    #endif
    #define HBLHMI_LLI_INITIALIZATION_FEATURE               DISABLED
    #define HMI_ACUVARIABLE_MODULE_FEATURE                  DISABLED
    #define HMI_RECIPE_ENGINE_FEATURE                       DISABLED
    #define HMI_VARIABLE_FEATURE                            DISABLED
    #define MEVP_HMI_FRAMEWORK_FEATURE                      DISABLED
    #ifndef NVDATA_STORE_STATIC_FEATURE
        #define NVDATA_STORE_STATIC_FEATURE                 ENABLED
    #endif
    #define NVDATA_STORE_APPLIANCE_AND_STATISTICS_FEATURE   DISABLED
    #define RECEIVE_STATUS_VARIABLES_FEATURE                DISABLED
    #define SETTINGFILE_MASTER_FILE_FEATURE                 DISABLED
    #ifndef SFUPDATER_ACU_SECTION_ID
        #define SFUPDATER_ACU_SECTION_ID                    SF_ID_ACU_EXP_1
    #endif
    #ifndef SFUPDATER_HMI_SECTION_ID
        #define SFUPDATER_HMI_SECTION_ID                    SF_ID_HMI_EXP_1
    #endif
    #define TIME_ESTIMATION_FEATURE                         DISABLED
    #ifndef VARIABLES_REGULATIONS_FEATURE
        #define VARIABLES_REGULATIONS_FEATURE               DISABLED
    #endif
    #define VISUAL_MACRO_VARIABLES_FEATURE                  DISABLED


#elif (SYSTEM_ACU_EXPANSION_HANDLING == ENABLED)

    // The following configuration is for an ACU expansion board that is not the main HMI.
    #if ((SYSTEM_HMI_HANDLING == DISABLED) && (SYSTEM_EXPANSION_HANDLING == ENABLED))
        #error An expansion board cannot control other expansion boards directly.
    #endif
    #if (SYSTEM_ACU_HANDLING == ENABLED)
        #error The expansion ACU board cannot act as an ACU main board.
    #endif

    // Enable/Disable communication features
	#ifndef API004DEBUG_FEATURE
        #define API004DEBUG_FEATURE                         DISABLED
    #endif
    #define API005LOWLEVEL_FEATURE                          ENABLED
    #define API005_CONTROL_CYCLES_FEATURE                   DISABLED
    #define API005_USE_LOADS_AND_GIS_FEATURE                ENABLED
    #define API005_USE_OBJECTS_AND_EVENTS_FEATURE           DISABLED
    #define API006HMI_FEATURE                               DISABLED
    #define API007DATA_FEATURE                              ENABLED
    #ifndef API010POLLVAR_FEATURE
        #define API010POLLVAR_FEATURE                       ENABLED
    #endif
    #ifndef API010_USES_FIXED_VARIABLES_FEATURE
        #define API010_USES_FIXED_VARIABLES_FEATURE         DISABLED
    #endif
    #define API011APPCTRL_FEATURE                           ENABLED
    #define API012EXP_FEATURE                               ENABLED
    #ifndef API013_CLIENT_FEATURE
        #define API013_CLIENT_FEATURE                       DISABLED
    #endif
    #ifndef API013_SERVER_FEATURE
        #define API013_SERVER_FEATURE                       DISABLED
    #endif
    #define API014EXPHMI_FEATURE                            DISABLED
    #define API017TOUCH_FEATURE                             DISABLED
    #ifndef API018_CLIENT_FEATURE
        #define API018_CLIENT_FEATURE                       DISABLED
    #endif
    #define API018_SERVER_FEATURE                           DISABLED
    #define API019_BOARD_DATA_FEEDBACK                      DISABLED
    #define API201_DEVICE_SECURITY_FEATURE                  DISABLED
    #ifndef API220MOTORCONTROL_FEATURE
        #define API220MOTORCONTROL_FEATURE                  DISABLED
    #endif
    #define API221_CLIENT_FEATURE                           DISABLED
    #ifndef API221_SERVER_FEATURE
        #define API221_SERVER_FEATURE                       DISABLED
    #endif
    #define COMMUNICATION_MONITOR_FEATURE                   DISABLED
    #define CONNECTIVITY_HMI_FEATURE                        DISABLED

    // Enable/Disable other features
    #define FAULT_MANAGEMENT_FEATURE                        DISABLED
    #ifndef HBLACU_ZEROCROSS_FEATURE
        #define HBLACU_ZEROCROSS_FEATURE                    DISABLED
    #endif
    #define HBLHMI_LLI_INITIALIZATION_FEATURE               DISABLED
    #define HMI_ACUVARIABLE_MODULE_FEATURE                  DISABLED
    #define HMI_RECIPE_ENGINE_FEATURE                       DISABLED
    #define HMI_VARIABLE_FEATURE                            DISABLED
    #define MEVP_HMI_FRAMEWORK_FEATURE                      DISABLED
    #ifndef NVDATA_STORE_STATIC_FEATURE
        #define NVDATA_STORE_STATIC_FEATURE                 ENABLED
    #endif
    #define NVDATA_STORE_APPLIANCE_AND_STATISTICS_FEATURE   DISABLED
    #define RECEIVE_STATUS_VARIABLES_FEATURE                DISABLED
    #define SETTINGFILE_MASTER_FILE_FEATURE                 DISABLED
    #ifndef SFUPDATER_ACU_SECTION_ID
        #define SFUPDATER_ACU_SECTION_ID                    SF_ID_ACU_EXP_1
    #endif
    #define TIME_ESTIMATION_FEATURE                         DISABLED
    #ifndef VARIABLES_REGULATIONS_FEATURE
        #define VARIABLES_REGULATIONS_FEATURE               DISABLED
    #endif
    #define VISUAL_MACRO_VARIABLES_FEATURE                  DISABLED


#elif (SYSTEM_HMI_EXPANSION_HANDLING == ENABLED)

    // The following configuration is for an HMI expansion board that is not the main ACU.
    #if ((SYSTEM_ACU_HANDLING == DISABLED) && (SYSTEM_EXPANSION_HANDLING == ENABLED))
        #error An expansion board cannot control other expansion boards directly.
    #endif
    #if (SYSTEM_HMI_HANDLING == ENABLED)
        #error The expansion HMI board cannot act as an HMI main board.
    #endif

    // Enable/Disable communication features
	#ifndef API004DEBUG_FEATURE
        #define API004DEBUG_FEATURE                         DISABLED
    #endif
    #define API005LOWLEVEL_FEATURE                          ENABLED
    #define API005_CONTROL_CYCLES_FEATURE                   DISABLED
    #define API005_USE_LOADS_AND_GIS_FEATURE                DISABLED
    #define API005_USE_OBJECTS_AND_EVENTS_FEATURE           ENABLED
    #define API006HMI_FEATURE                               DISABLED
    #define API007DATA_FEATURE                              ENABLED
    #define API010POLLVAR_FEATURE                           ENABLED
    #define API010_USES_FIXED_VARIABLES_FEATURE             ENABLED
    #define API011APPCTRL_FEATURE                           ENABLED
    #if(!defined(API014EXPHMI_FEATURE) || (API014EXPHMI_FEATURE == ENABLED))
        #define API012EXP_FEATURE                           DISABLED
    #elif (API014EXPHMI_FEATURE == DISABLED) 
        #define API012EXP_FEATURE                           ENABLED
    #else
        #define API012EXP_FEATURE                           DISABLED
    #endif
    #ifndef API013_CLIENT_FEATURE
        #define API013_CLIENT_FEATURE                       DISABLED
    #endif
    #ifndef API013_SERVER_FEATURE
        #define API013_SERVER_FEATURE                       DISABLED
    #endif
    #ifndef API014EXPHMI_FEATURE
        #define API014EXPHMI_FEATURE                        ENABLED
    #endif
    #ifndef API017TOUCH_FEATURE
        #define API017TOUCH_FEATURE                         DISABLED
    #endif
    #define API018_CLIENT_FEATURE                           ENABLED
    #define API018_SERVER_FEATURE                           DISABLED
    #ifndef API019_BOARD_DATA_FEEDBACK
        #define API019_BOARD_DATA_FEEDBACK                  DISABLED
    #endif
    #define API201_DEVICE_SECURITY_FEATURE                  DISABLED
    #define API220MOTORCONTROL_FEATURE                      DISABLED
    #define API221_CLIENT_FEATURE                           DISABLED
    #define API221_SERVER_FEATURE                           DISABLED
    #ifndef COMMUNICATION_MONITOR_FEATURE
        #define COMMUNICATION_MONITOR_FEATURE               DISABLED
    #endif
    #ifndef CONNECTIVITY_HMI_FEATURE
        #define CONNECTIVITY_HMI_FEATURE                    DISABLED
    #endif

    // Enable/Disable other features
    #define FAULT_MANAGEMENT_FEATURE                        DISABLED
    #define HBLACU_ZEROCROSS_FEATURE                        DISABLED
    #define HBLHMI_LLI_INITIALIZATION_FEATURE               ENABLED
    #define HMI_ACUVARIABLE_MODULE_FEATURE                  DISABLED
    #define HMI_RECIPE_ENGINE_FEATURE                       DISABLED
    #define HMI_VARIABLE_FEATURE                            DISABLED
    #define MEVP_HMI_FRAMEWORK_FEATURE                      DISABLED
    #ifndef NVDATA_STORE_STATIC_FEATURE
        #define NVDATA_STORE_STATIC_FEATURE                 ENABLED
    #endif
    #define NVDATA_STORE_APPLIANCE_AND_STATISTICS_FEATURE   DISABLED
    #define RECEIVE_STATUS_VARIABLES_FEATURE                DISABLED
    #define SETTINGFILE_MASTER_FILE_FEATURE                 DISABLED
    #ifndef SFUPDATER_HMI_SECTION_ID
        #define SFUPDATER_HMI_SECTION_ID                    SF_ID_HMI_EXP_1
    #endif
    #define TIME_ESTIMATION_FEATURE                         DISABLED
    #ifndef VARIABLES_REGULATIONS_FEATURE
        #define VARIABLES_REGULATIONS_FEATURE               DISABLED
    #endif
    #define VISUAL_MACRO_VARIABLES_FEATURE                  DISABLED


#else

    // The following configuration is invalid. Missing SYSTEM_X_HANDLING macro declarations in SystemConfig_prm.h.
    #error The board type cannot be determined. You must declare the SYSTEM_X_HANDLING macros in SystemConfig_prm.h.



#endif      // SYSTEM_MCU_HANDLING


#ifndef CONNECTIVITY_FEATURE
    #define CONNECTIVITY_FEATURE CONNECTIVITY_HMI_FEATURE
#endif
#if (defined(SFUPDATER_HMI_SECTION_ID) || defined(SFUPDATER_ACU_SECTION_ID) || defined(SFUPDATER_MCU_SECTION_ID))
    #define SETTINGFILE_SECTION_DOWNLOAD_FEATURE ENABLED
#else
    #define SETTINGFILE_SECTION_DOWNLOAD_FEATURE DISABLED
#endif

#if (API004DEBUG_FEATURE == ENABLED)
    #ifndef SW05_SOFTWARE_TESTS_FEATURE
        #define SW05_SOFTWARE_TESTS_FEATURE  ENABLED
    #endif
#else
    #define SW05_SOFTWARE_TESTS_FEATURE  DISABLED
#endif

// CONNECTIVITY_HMI_FEATURE cannot be enabled if CONNECTIVITY_FEATURE is DISABLED.
CT_ASSERT((CONNECTIVITY_FEATURE == ENABLED) || (CONNECTIVITY_HMI_FEATURE == DISABLED));

//--------------------------------------------- Fast Handler feature -------------------------------------------------
#ifndef FAST_HANDLER_FEATURE
    #define FAST_HANDLER_FEATURE                DISABLED
#else
    #ifndef FAST_HANDLER_TIMING_USEC
        // Default Faster Handler timing is 200us
        #define FAST_HANDLER_TIMING_USEC        200
    #endif
    #ifndef FAST_HANDLER_TIMER
        #error "FAST_HANDLER_TIMER must be defined"
    #endif
    #ifndef FAST_HANDLER_TIMER_CHANNEL
        #error "FAST_HANDLER_TIMER_CHANNEL must be defined"
    #endif
#endif

//------------------------------------ ZeroCross Polling Mode and Gpio Request number --------------------------------
#ifndef ZEROCROSS_POLLING_MODE
    #define ZEROCROSS_POLLING_MODE              FAST_HANDLER_FEATURE
#endif
#ifndef ZEROCROSS_GPIO_REQUEST_NUMBER
    #define ZEROCROSS_GPIO_REQUEST_NUMBER       0
#endif

#endif // SYSTEMCONFIG_H

/**
 *  @file
 *
 *  @brief      Public definitions for API 019 module.
 *
 *  @copyright  Copyright 2015 - 2017 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
#ifndef API019PRODUCTINFO_DEFS_H_
    #define API019PRODUCTINFO_DEFS_H_

//  --- Include Files -------------------------------------------------------------------------------------------------
#include "SystemConfig.h"

//=====================================================================================================================
//  --- Public Properties ---------------------------------------------------------------------------------------------
//=====================================================================================================================
typedef enum
{
    API019PRODUCTINFO_OP_REQ_PUB_APPLIANCE_DATA     =   1,
    API019PRODUCTINFO_OP_SET_APPLIANCE_DATA         =   2,
    API019PRODUCTINFO_OP_REQ_PUB_BOARD_DATA         =   3,
    API019PRODUCTINFO_OP_SET_BOARD_DATA             =   4,
    API019PRODUCTINFO_OP_REQ_PUB_PROJECT_DATA       =   5,
    API019PRODUCTINFO_OP_REQ_PUB_SERVICE_DATA       =   6,
    API019PRODUCTINFO_OP_SET_SERVICE_DATA           =   7,
    API019PRODUCTINFO_OP_REQ_PUB_FVT_DATA           =   8,
    API019PRODUCTINFO_OP_SET_FVT_DATA               =   9,
    API019PRODUCTINFO_OP_SET_FVT_DATA_RESULT        =   10,
    API019PRODUCTINFO_OP_REQ_PUB_EOL_DATA           =   11,
    API019PRODUCTINFO_OP_SET_EOL_DATA               =   12,
    API019PRODUCTINFO_OP_SET_EOL_DATA_RESULT        =   13,
    API019PRODUCTINFO_OP_SET_CALIB_DATA             =   14,
    API019PRODUCTINFO_OP_REQ_PUB_CALIB_DATA         =   15,
    API019PRODUCTINFO_OP_REQ_PUB_CLASS_B_SIGN       =   16,
	API019PRODUCTINFO_OP_REQ_PUB_PRODUCT_CONFIG_DATA=   17,
    // 18 to 30 reserved for future expansion
    API019PRODUCTINFO_OP_STORE_DATA                 =   31,
} API019PRODUCTINFO_OPCODE_TYPE;

// -- Public Type Declarations --
//! API019 Appliance Data Structure type definition
PACKED typedef struct API019_APPLIANCE_DATA_STRUCT
{
    char Appliance_Model_Number[20];
    char Appliance_Serial_Number[20];
    uint8 Appliance_Number_Boadrs;
} API019_APPLIANCE_DATA_TYPE;

//! API019 Board Data Structure type definition
PACKED typedef struct API019_BOARD_DATA_STRUCT
{
    char Board_Part_Number[20];
    uint8 Board_Revision[2];
    char Board_Serial_Number[20];
    uint8 Board_Replacement;
    uint8 Board_Num_Projects;
} API019_BOARD_DATA_TYPE;

//! API019 Micro Vendor type definition
typedef enum API019_MICRO_VENDOR_ENUM
{
    API019_MICRO_VENDOR_ATMEL       =   0,
    API019_MICRO_VENDOR_CYPRESS,
    API019_MICRO_VENDOR_FREESCALE,
    API019_MICRO_VENDOR_NXP,
    API019_MICRO_VENDOR_RENESAS,
    API019_MICRO_VENDOR_ST,
    API019_MICRO_VENDOR_ESPRESSIF,
    API019_MICRO_VENDOR_NOT_DEFINED = 255
} API019_MICRO_VENDOR_TYPE;

//! API019 Micro Model type definition
typedef enum API019_MICRO_MODEL_ENUM
{
    // UNKNOWN MICRO
    API019_MICRO_SPECIFIC_NOT_DEFINED   = 0xFF,

    API019_MICRO_ATMEL_MEGA             = 0,
    API019_MICRO_ATMEL_SAMD20           = 1,
    API019_MICRO_ATMEL_SAMD21           = 2,

    API019_MICRO_CYPRESS_CY8C4014       = 0,
    API019_MICRO_CYPRESS_CY8C4045       = 1,

    API019_MICRO_NXP_MKE04Z128VLD4      = 0,
    API019_MICRO_NXP_MKE02Zxxx          = 1,
    API019_MICRO_NXP_MKE14Zxxx          = 2,
    API019_MICRO_NXP_MKV31xxx           = 3,
    API019_MICRO_NXP_MKV30xxx           = 4,
    API019_MICRO_NXP_MKE14Fxxx          = 5,
    API019_MICRO_NXP_iMx8               = 6,

    API019_MICRO_RENESAS_RX210          = 0,
    API019_MICRO_RENESAS_RX130          = 1,
    API019_MICRO_RENESAS_RX24Txxx       = 2,
    API019_MICRO_RENESAS_RS5X           = 3,
    API019_MICRO_RENESAS_RX66T          = 4,

    API019_MICRO_ST_STM32F0X            = 0,
    API019_MICRO_ST_STM32F100           = 1,
    API019_MICRO_ST_STM32F101           = 2,
    API019_MICRO_ST_STM32F103           = 3,
    API019_MICRO_ST_STM32F303           = 4,
    API019_MICRO_ST_STM32F40X           = 5,
    API019_MICRO_ST_STM32F41X           = 6,
    API019_MICRO_ST_STM32F42X           = 7,
    API019_MICRO_ST_STM32F43X           = 8,
    API019_MICRO_ST_STM32G0X            = 9,

    API019_MICRO_ESPRESSIF_ESP32        = 0

} API019_MICRO_MODEL_TYPE;

//! API019 Platform Type type definition
typedef enum API019_PLATFORM_TYPE_ENUM
{
    API019_PLATFORM_HA_WASHER      = 0,
    API019_PLATFORM_VA_WASHER      = 1,
    API019_PLATFORM_REFRIGERATION  = 2,
    API019_PLATFORM_COOKING        = 3,
    API019_PLATFORM_DRYER          = 4,
    API019_PLATFORM_DISHWASHER     = 5,
    API019_PLATFORM_HMI_BRIDGE     = 6,

    API019_PLATFORM_NOT_AVAILABLE  = 255
} API019_PLATFORM_TYPE;

//! API019 Device Type type definition
typedef enum API019_DEVICE_ENUM
{
    API019_DEVICE_ACU           = 0,
    API019_DEVICE_HMI           = 1,
    API019_DEVICE_MCU           = 2,
    API019_DEVICE_EXPANSION_ACU = 3,
    API019_DEVICE_EXPANSION_HMI = 4,
    API019_DEVICE_EXPANSION_MCU = 5,
    API019_DEVICE_SETTING_FILE  = 6,
    API019_DEVICE_TOUCH  		= 7,

    API019_DEVICE_NOT_AVAILABLE = 255
} API019_DEVICE_TYPE;

//! API019 Test Data Structure type definition
PACKED typedef struct API019_TEST_DATA_STRUCT
{
    uint8 Tester_ID;
    uint8 Tester_Version[3];
    char Test_Date[8];
    uint8 Test_Result[16];
} API019_TEST_DATA_TYPE;

//! API019 Service Data Structure type definition
PACKED typedef struct API019_SERVICE_DATA_STRUCT
{
    uint8 Tester_ID;
    uint8 Tester_Version[3];
    char Test_Date[8];
    uint32 Test_Age;
    uint16 Test_Duration;
    uint8 Test_Result[12];
} API019_SERVICE_DATA_TYPE;

//! API019 Calibration Data type definition
typedef uint8 API019_CAL_DATA_TYPE;

//! Project type enumeration.
typedef enum PRODUCTINFO_PROJECT_ENUM
{
    API019_PROJECT_FIRMWARE         = 0,
    API019_PROJECT_TOUCH_1          = 1,
    API019_PROJECT_TOUCH_2          = 2,
    API019_PROJECT_TOUCH_CONFIG_1   = 3,
    API019_PROJECT_TOUCH_CONFIG_2   = 4,

    // Development project types are not to be used in production
    // Development project types can be used temporarily until one is formally defined
    API019_PROJECT_DEVELOPMENT_1    = 5,
    API019_PROJECT_DEVELOPMENT_2    = 6,
    API019_PROJECT_DEVELOPMENT_3    = 7,
    API019_PROJECT_DEVELOPMENT_4    = 8,
    API019_PROJECT_DEVELOPMENT_5    = 9,
    API019_PROJECT_DEVELOPMENT_6    = 10,
    API019_PROJECT_DEVELOPMENT_7    = 11,
    API019_PROJECT_DEVELOPMENT_8    = 12,
    API019_PROJECT_DEVELOPMENT_9    = 13,
    API019_PROJECT_DEVELOPMENT_10   = 14,
    API019_PROJECT_DEVELOPMENT_11   = 15,
    API019_PROJECT_DEVELOPMENT_12   = 16,

    API019_PROJECT_AUDIO            = 241,
    API019_PROJECT_WIFI             = 242,
    API019_PROJECT_IAP              = 243,  //!< In Application Programmer
    API019_PROJECT_CAPABILITY_FILE  = 244,
    API019_PROJECT_AOSP             = 245,  //!< Android Open Source Project

    //! ACU Boards only have the Setting file Project information built in the core module
    API019_PROJECT_SETTINGFILE      = 254,
    API019_PROJECT_NOT_AVAILABLE    = 255
} API019_PROJECT_TYPE;

//! API019 Project Data Structure type definition
PACKED typedef struct API019_PROJECT_DATA_STRUCT
{
    char Project_Name[10];
    char Project_Release_Number[10];
    char Project_Release_Date[8];
    uint8 Project_Release_Version[3];
    API019_MICRO_VENDOR_TYPE Micro_Vendor;
    API019_MICRO_MODEL_TYPE Micro_Specific;
    API019_PLATFORM_TYPE Platform_Type;
    API019_DEVICE_TYPE Device_Type;
    uint8 Product_Cfg1;
    uint8 Product_Type;
    uint32 ClassB_CRC;
    char Project_ClassB_Id[11];
} API019_PROJECT_DATA_TYPE;

#endif // API019PRODUCTINFO_DEFS_H_

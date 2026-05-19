/**
 *  @file       
 *
 *  @brief      Basic description of file contents
 *
 *  @details    Detailed description of the file contents
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>	
 *
 *  
 *
 *  @copyright  Copyright 2014-. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------


//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "SystemConfig.h"
#include "IAPEmul.h" 
#include "Gpio.h"
#include "Uart.h"
#include "Timeout.h"
#include "Micro.h"
#include "Micro.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief  - Call back handler function called at every 1ms
 * @details- The execution of the method shall be smaller. It should allow enough time for the IAPEmul execution
 *
 */
#ifndef IAPEMUL__1MS_HANDLER
    #define IAPEMUL__1MS_HANDLER()                  //App_1msHandler()
#endif

#define IAPEMUL_COM                             WIN_PORT

#ifndef IAPEMUL_BASE_SPEED
    #define IAPEMUL_BASE_SPEED                      B38400
#endif

#define MST_IAPEMUL_MODE_TIMEOUT                        ((TIMEOUT_LABEL)0)
#define IAPEMUL_MODE_TIMEOUT_IN_MS                      5000

#define IAPEMUL_WMSP_RX_DATA_TIMEOUT                    400
#define IAPEMUL_WMSP_HEADER                             0xDE
#define IAPEMUL_WMSP_CONTROL_FIELD_LENGHT               3
#define IAPEMUL_WMSP_RX_PAYLOAD_SIZE                    10
#define IAPEMUL_WMSP_NODE_ALIVE_SAP_ID_LIST             0x0E
#define IAPEMUL_WMSP_BROADCAST_ADDRESS_NODE             0x0F
#define IAPEMUL_IAP_CMD_ID_PUBLISH_SPEED_CHANGE_SIZE    3

typedef struct
{
    uint8 destination       :4;
    uint8 source            :4;
    uint8 sap               :4;
    uint8 sdu_extension     :4;
    uint8 sdu_length;
    uint8 *payload;
}IAPEMUL_WMSP_MSG_TYPE;

#define IAPEMUL_WMSP_ADDR_INDEX                         0
#define IAPEMUL_WMSP_SAP_SDUEXT_INDEX                   1
#define IAPEMUL_WMSP_SDU_INDEX                          2
#define IAPEMUL_WMSP_PAYLOAD_INDEX                      3

#define IAPEMUL_WMSP_DEST_ADDR_MASK                     0x0F
#define IAPEMUL_WMSP_SRC_ADDR_MASK                      0xF0
#define IAPEMUL_WMSP_SAP_MASK                           0x0F
#define IAPEMUL_WMSP_SDU_EXT_MASK                       0xF0

typedef enum
{
    IAPEMUL_WMSP_RX_SYNCHRONIZATION,
    IAPEMUL_WMSP_RX_DATA_RECEVIENG
}IAPEMUL_RX_HANDLER_STATE;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static IAPEMUL_RX_HANDLER_STATE IAPEmul_Wmsp_Rx_Status;
static uint8 IAPEmul_Wmsp_Rx_Buffer[IAPEMUL_WMSP_RX_PAYLOAD_SIZE + IAPEMUL_WMSP_CONTROL_FIELD_LENGHT];
static uint16 IAPEmul_Wmsp_Rx_Index;
static uint16 IAPEmul_Wmsp_Sdu;
static uint16 IAPEmul_Wmsp_Rx_Timeout;
static uint16 IAPEmul_TimeoutCount;

static void WmspInitialize(void);
static void WmspHandlerRxPackage(void);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module IAPEmul and its variables
 *              This must be called before starting the scheduler
 *
 */
void IAPEmul__Engage(void)
{
    uint16 time_cnt;

    if ((Micro__GetWarmResetAction() & MICRO_WARM_RESET_ACTION_ENTER_IAP) == MICRO_WARM_RESET_ACTION_ENTER_IAP)
    {
        //Disable the interrupts
        DISABLE_INTERRUPTS();

        Gpio__Initialize();
        Timeout__Initialize();
        WmspInitialize();
        Micro__ServiceWatchdog();
        Uart__Initialize(IAPEMUL_COM, IAPEMUL_BASE_SPEED, UART_STOP_BIT_1, UART_PARITY_NONE);

        //Refresh the IAP Engage Time
        Timeout__SetTime(MST_IAPEMUL_MODE_TIMEOUT, IAPEMUL_MODE_TIMEOUT_IN_MS);

        //Emulates the IAP Engage mode
        while (Timeout__GetStatus(MST_IAPEMUL_MODE_TIMEOUT) == FALSE)
        {
            WmspHandlerRxPackage();

            time_cnt = Timeout__GetCount();

            //1ms scheduler
            if (IAPEmul_TimeoutCount != time_cnt)
            {
                IAPEmul_TimeoutCount = time_cnt;

                Micro__ServiceWatchdog();
                IAPEMUL__1MS_HANDLER();
            }
        }

        Micro__DisableWarmResetAction(MICRO_WARM_RESET_ACTION_ENTER_IAP);
        Uart__Disable(IAPEMUL_COM, UART_OPTION_ALL);
    }
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief    This method initializes the Wmsp module.
 * @details  This method should be called after initialization of basic drivers such as Micro and Gpio and
 *           before TimerInterrupt__Initialize since Wmsp have tasks being called from Slots associated to
 *           timer interruption.<br>
 */
static void WmspInitialize(void)
{
    IAPEmul_Wmsp_Rx_Status              = IAPEMUL_WMSP_RX_SYNCHRONIZATION;
    IAPEmul_Wmsp_Rx_Index               = 0;
    IAPEmul_Wmsp_Sdu                    = 0;
    IAPEmul_Wmsp_Rx_Timeout             = 0;

    IAPEmul_Wmsp_Rx_Buffer[IAPEMUL_WMSP_ADDR_INDEX]        = 0;
    IAPEmul_Wmsp_Rx_Buffer[IAPEMUL_WMSP_SAP_SDUEXT_INDEX]  = 0;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief    This method handles the process the reception of Wmsp packages.
 * @details  This method should be called every 1ms from an Interrupt Slot. It interfaces intensively with Uart driver.
 *           The Reception process runs all the time by monitoring bytes coming from the Uart driver.
 *           Once a package is fully received it flags the Wmsp__HandlerProtocol() which will take the package and process it.<br>
 *
 *           This receives the eco of the message as well in the Half duplex mode.<br>
 *
 *           Flowchart shown below-
 * @image      html Wmsp__HandlerRxPackage.jpg
 *
 */

static void WmspHandlerRxPackage(void)
{
    uint16 size;
    uint8 data;
    UART_SPEED_DEF speed;

    size = Uart__GetStatus(IAPEMUL_COM, UART_STATUS_RX_AVAILABLE);

    while(size > 0)
    {
        size--;

        data = Uart__ReadByte(IAPEMUL_COM);

        IAPEmul_Wmsp_Rx_Timeout = IAPEMUL_WMSP_RX_DATA_TIMEOUT;

        if (IAPEmul_Wmsp_Rx_Status == IAPEMUL_WMSP_RX_SYNCHRONIZATION)
        {
            if (data == IAPEMUL_WMSP_HEADER)
            {
                WmspInitialize();
                IAPEmul_Wmsp_Rx_Timeout = IAPEMUL_WMSP_RX_DATA_TIMEOUT;
                IAPEmul_Wmsp_Rx_Status  = IAPEMUL_WMSP_RX_DATA_RECEVIENG;
            }
        }
        else // if (IAPEmul_Wmsp_Rx_Status == IAPEMUL_WMSP_RX_DATA_RECEVIENG)
        {
            if (IAPEmul_Wmsp_Rx_Index < (IAPEMUL_WMSP_RX_PAYLOAD_SIZE + IAPEMUL_WMSP_CONTROL_FIELD_LENGHT))
            {
                IAPEmul_Wmsp_Rx_Buffer[IAPEmul_Wmsp_Rx_Index]   = data;
                IAPEmul_Wmsp_Sdu = ((IAPEmul_Wmsp_Rx_Buffer[IAPEMUL_WMSP_SAP_SDUEXT_INDEX] & IAPEMUL_WMSP_SDU_EXT_MASK)  << 4 ) | IAPEmul_Wmsp_Rx_Buffer[IAPEMUL_WMSP_SDU_INDEX];
            }

            // This must be screened on every byte reception for Alive status
            if (IAPEMUL_WMSP_NODE_ALIVE_SAP_ID_LIST == (IAPEmul_Wmsp_Rx_Buffer[IAPEMUL_WMSP_SAP_SDUEXT_INDEX] & IAPEMUL_WMSP_SAP_MASK) )
            {
                //Refresh the IAP Engage Time
                Timeout__SetTime(MST_IAPEMUL_MODE_TIMEOUT, IAPEMUL_MODE_TIMEOUT_IN_MS);
            }

            //After receiving the full message
            if (IAPEmul_Wmsp_Rx_Index > (IAPEmul_Wmsp_Sdu + IAPEMUL_WMSP_CONTROL_FIELD_LENGHT))
            {
                //Handling the echo over the bus
                if ((IAPEmul_Wmsp_Rx_Buffer[IAPEMUL_WMSP_ADDR_INDEX] & IAPEMUL_WMSP_DEST_ADDR_MASK) == IAPEMUL_WMSP_BROADCAST_ADDRESS_NODE)
                {
                    if (IAPEmul_Wmsp_Sdu == IAPEMUL_IAP_CMD_ID_PUBLISH_SPEED_CHANGE_SIZE)
                    {
                        //IAP Speed Change command
                        speed = (UART_SPEED_DEF)IAPEmul_Wmsp_Rx_Buffer[IAPEMUL_WMSP_PAYLOAD_INDEX + 2];

                        if ((speed <= B115200) &&
                            (speed >= B9600))
                        {
                            Uart__Initialize(IAPEMUL_COM, speed, UART_STOP_BIT_1, UART_PARITY_NONE);
                        }
                    }
                }

                IAPEmul_Wmsp_Rx_Status = IAPEMUL_WMSP_RX_SYNCHRONIZATION;
                IAPEmul_Wmsp_Rx_Timeout = 0;
            }

            IAPEmul_Wmsp_Rx_Index++;
        }
    }

    // Generic timeout for the reception state machine
    // If the system stops in the middle of a package, after this timeout it resets the state machine.
    if (IAPEmul_Wmsp_Rx_Timeout > 0)
    {
        IAPEmul_Wmsp_Rx_Timeout--;
    }
    else
    {
        IAPEmul_Wmsp_Rx_Status = IAPEMUL_WMSP_RX_SYNCHRONIZATION;
    }
}

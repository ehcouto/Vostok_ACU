/**
 *  @file       
 *
 *  @brief      Direct memory access (DMA) is used in order to provide high-speed data transfer between
 *              peripherals and memory as well as memory to memory. Data can be quickly moved by DMA
 *              without any CPU actions. This keeps CPU resources free for other operations.
 *
 *  @details    STM32G0x supports 12 Dma channels.
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "uc.h"
#include "Micro.h"
#include "Dma.h"
//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
#define DMA1_MAX_CHANNEL    7
#define DMA2_MAX_CHANNEL    5

#define DMA_MAX_CHANNEL     (DMA1_MAX_CHANNEL + DMA2_MAX_CHANNEL)

//Dummy Dma channel is created to match the index of the Cortex-M3 hardware Dma channel with 'DMA_CHANNEL_TYPE'
#define DMA_CHANNEL_NULL    (*(DMA_CHANNEL_TYPEDEF *) NULL)

#define INVALID_DMA_INDEX   (0xFF)

//This array holds all the Dma channel. Cortex-M3 doesn't consists of Dma channel 0. In-order to match the index of hardware Dma channel with 'DMA_CHANNEL_TYPE'
// a DMA1_CHANNEL0 is added at the beginning of the array
static DMA_CHANNEL_TYPEDEF * const DMA_CHANNEL[DMA_MAX_CHANNEL +1 ] =
{
    &DMA_CHANNEL_NULL,      //DMA_CHANNEL_NULL -
    &DMA1_CHANNEL1,         //DMA_ENGINE1, DMA_CH1
    &DMA1_CHANNEL2,         //DMA_ENGINE1, DMA_CH2
    &DMA1_CHANNEL3,         //DMA_ENGINE1, DMA_CH3
    &DMA1_CHANNEL4,         //DMA_ENGINE1, DMA_CH4
    &DMA1_CHANNEL5,         //DMA_ENGINE1, DMA_CH5
    &DMA1_CHANNEL6,         //DMA_ENGINE1, DMA_CH6
    &DMA1_CHANNEL7,         //DMA_ENGINE1, DMA_CH7
    &DMA2_CHANNEL1,         //DMA_ENGINE2, DMA_CH1
    &DMA2_CHANNEL2,         //DMA_ENGINE2, DMA_CH2
    &DMA2_CHANNEL3,         //DMA_ENGINE2, DMA_CH3
    &DMA2_CHANNEL4,         //DMA_ENGINE2, DMA_CH4
    &DMA2_CHANNEL5          //DMA_ENGINE2, DMA_CH5
};

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------


//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module DMA and its variables.
 *  @details    The engine is not enabled if its previously enabled to prevent the reset of the configuration.
 *
 */
void Dma__EngineInitialize(void)
{
    //Enable the DMA1 device in the AHB bus
    if ((RCC.AHBENR & RCC_AHBENR_DMA1EN) == 0)
    {
        RCC.AHBENR |= RCC_AHBENR_DMA1EN;
    }

    //Enable the DMA2 device in the AHB bus
    if ((RCC.AHBENR & RCC_AHBENR_DMA2EN) == 0)
    {
        RCC.AHBENR |= RCC_AHBENR_DMA2EN;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief  Initializes the Dma channel according to the specified parameters
  *         in the DMA_InitStruct.
  *
  * @param  dma_engine  : Specify the Dma Engine
  * @param  channel     : Defines the specific Dma channel from the DMA_CHANNEL_TYPE
  * @param  DMA_InitStruct: pointer to a DMA_InitTypeDef structure that contains
  *         the configuration information for the specified DMA Channel.
  *
  * @retval DMA_RETURN_TYPE
  *      @retval DMA_RETURN_FAIL(0)      : Dma channel was fail to initialize
  *      @retval DMA_RETURN_SUCCESS(1)   : Dma channel was successfully initialized
  */
DMA_RETURN_TYPE Dma__Initialize(DMA_ENGINE_TYPE dma_engine, DMA_CHANNEL_TYPE channel, DMA_INIT_TYPE* DMA_InitStruct)
{
    DMA_RETURN_TYPE ret;
    uint32 tmpreg;
    uint8 dma_index;

    tmpreg = 0;
    ret = DMA_RETURN_FAIL;
    dma_index = INVALID_DMA_INDEX;
    uint8 dmamux_channel = 0;

    if ((channel) && (DMA_InitStruct->DMAMux_Request_ID <= DMAMUX_MAX_REQ))
    {
        if (dma_engine == DMA_ENGINE1)
        {
            if(channel <= DMA1_MAX_CHANNEL )
            {
                dma_index = channel;
            }
        }

        else if (dma_engine == DMA_ENGINE2)
        {
            if(channel <= DMA2_MAX_CHANNEL )
            {
                dma_index = DMA1_MAX_CHANNEL + channel;
            }
        }
    }

    if (dma_index != INVALID_DMA_INDEX)
    {
        //--------------------------- DMAy Channelx CCR Configuration ----------------
        // Configure DMA channel: data transfer, data size, priority level and mode
        // Set DIR bit according to DMA_DIR value
        if (DMA_InitStruct->DMA_DIR == DMA_DATA_DIRECTION_MEM_TO_PERIPH)
        {
            tmpreg |= DMA_CCR_DIR;

        }
        else if (DMA_InitStruct->DMA_DIR == DMA_DATA_DIRECTION_MEM_TO_MEM)
        {
            tmpreg |= DMA_CCR_MEM2MEM;
        }

        // Set CIRC bit according to DMA_Mode value
        if (DMA_InitStruct->DMA_Mode == DMA_MODE_CIRCULAR)
        {
            tmpreg |= DMA_CCR_CIRC;
        }

        // Set PINC bit according to DMA_PeripheralInc value
        // Set MINC bit according to DMA_MemoryInc value
        switch(DMA_InitStruct->DMA_AddressInc)
        {
            case DMA_ADDRESS_INCREMENT_FOR_PERIPH_ADDRESS:
                tmpreg |= DMA_CCR_PINC;
                break;

            case DMA_ADDRESS_INCREMENT_FOR_MEM_ADDRESS:
                tmpreg |= DMA_CCR_MINC;
                break;

            case DMA_ADDRESS_INCREMENT_FOR_PERIPH_MEM_ADDRESS:
                tmpreg |= (DMA_CCR_PINC | DMA_CCR_MINC);
                break;

            default:
                break;
        }

        // Set PSIZE bits according to DMA_PeripheralDataSize value
        if (DMA_InitStruct->DMA_PeriDataSize == DMA_DATA_WIDTH_HALFWORD)
        {
            tmpreg |= DMA_CCR_PSIZE_0;

        }
        else if (DMA_InitStruct->DMA_PeriDataSize == DMA_DATA_WIDTH_WORD)
        {
            tmpreg |= DMA_CCR_PSIZE_1;
        }

        // Set MSIZE bits according to DMA_MemoryDataSize value
        if (DMA_InitStruct->DMA_MemDataSize == DMA_DATA_WIDTH_HALFWORD)
        {
            tmpreg |= DMA_CCR_MSIZE_0;

        }
        else if (DMA_InitStruct->DMA_MemDataSize == DMA_DATA_WIDTH_WORD)
        {
            tmpreg |= DMA_CCR_MSIZE_1;
        }

        // Set PL bits according to DMA_Priority value
        tmpreg |= (DMA_InitStruct->DMA_Priority & 0x03) << 12;


        // Write to DMA Channel CCR
        DMA_CHANNEL[dma_index]->CCR = tmpreg;

        // Write to DMA Channel CNDTR
        DMA_CHANNEL[dma_index]->CNDTR = DMA_InitStruct->DMA_BufferSize;

        // Write to DMA Channel CPAR
        DMA_CHANNEL[dma_index]->CPAR = DMA_InitStruct->DMA_PeriBaseAddr;

        // Write to DMA Channel CMAR
        DMA_CHANNEL[dma_index]->CMAR = DMA_InitStruct->DMA_MemBaseAddr;

        //DMA MUX
        DMAMUX1_CHANNELSTATUS.CFR = 1<< (dma_index-1);

        DMAMUX1.CCR[dma_index-1] = DMA_InitStruct->DMAMux_Request_ID;


        ret = DMA_RETURN_SUCCESS;
    }

    return (ret);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief    Starts the Dma transfer.
 * @details  Before calling this Channel should be initialized by calling Dma__Initialize() API
 *
 * @param    dma_engine  : Specify the Dma Engine
 * @param    channel     : Defines the specific Dma channel from the DMA_CHANNEL_TYPE
 * @param    DataNumber  : Number of data to be transfer
 *
 * @return   BOOL_TYPE
 *      @retval TRUE : Dma data transfer started.
 *      @retval FALSE: Dma data transfer failed.
 */
BOOL_TYPE Dma__StartTransfer(DMA_ENGINE_TYPE dma_engine, DMA_CHANNEL_TYPE channel, DMA_SIZE_TYPE DataNumber)
{
    BOOL_TYPE status;
    uint8 dma_index;

    status = FALSE;

    dma_index = INVALID_DMA_INDEX;

    if (channel)
    {
        if (dma_engine == DMA_ENGINE1)
        {
            if(channel <= DMA1_MAX_CHANNEL )
            {
                dma_index = channel;
            }
        }

        else if (dma_engine == DMA_ENGINE2)
        {
            if(channel <= DMA2_MAX_CHANNEL )
            {
                dma_index = DMA1_MAX_CHANNEL + channel;
            }
        }
    }

    if (dma_index != INVALID_DMA_INDEX)
    {
        if ((DMA_CHANNEL[dma_index]->CNDTR) == 0)
        {
            //Disable the DMA channel to initiate the new transfer
            DMA_CHANNEL[dma_index]->CCR &= (uint16)(~DMA_CCR_EN);

            //Set the data size to be transfered
            DMA_CHANNEL[dma_index]->CNDTR = DataNumber;

            //DMA MUX
            DMAMUX1_CHANNELSTATUS.CFR = 1<< (dma_index-1);


            //Enable the DMA channel
            DMA_CHANNEL[dma_index]->CCR |= DMA_CCR_EN;

            status = TRUE;
        }
    }

    return (status);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief    Stops the Dma transfer.
 * @details
 *
 * @param  dma_engine : Specify the Dma Engine
 * @param  channel    : Defines the specific Dma channel from the DMA_CHANNEL_TYPE
 */
void Dma__StopTransfer(DMA_ENGINE_TYPE dma_engine, DMA_CHANNEL_TYPE channel)
{
    uint8 dma_index;

    dma_index = INVALID_DMA_INDEX;

    if (channel)
    {
        if (dma_engine == DMA_ENGINE1)
        {
            if(channel <= DMA1_MAX_CHANNEL )
            {
                dma_index = channel;
            }
        }

        else if (dma_engine == DMA_ENGINE2)
        {
            if(channel <= DMA2_MAX_CHANNEL )
            {
                dma_index = DMA1_MAX_CHANNEL + channel;
            }
        }
    }

    if (dma_index != INVALID_DMA_INDEX)
    {
            //Disable the DMA channel to initiate the new transfer
            DMA_CHANNEL[dma_index]->CCR &= (uint16)(~DMA_CCR_EN);

            //Set the data size to be transfered
            DMA_CHANNEL[dma_index]->CNDTR = 0;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief  Enables the specified DMA channel
  *
  * @param  dma_engine : Specify the Dma Engine
  * @param  channel    : Defines the specific Dma channel from the DMA_CHANNEL_TYPE
  */
void Dma__Enable(DMA_ENGINE_TYPE dma_engine, DMA_CHANNEL_TYPE channel)
{
    uint8 dma_index;

    dma_index = INVALID_DMA_INDEX;

    if (channel)
    {
        if (dma_engine == DMA_ENGINE1)
        {
            if(channel <= DMA1_MAX_CHANNEL )
            {
                dma_index = channel;
            }
        }

        else if (dma_engine == DMA_ENGINE2)
        {
            if(channel <= DMA2_MAX_CHANNEL )
            {
                dma_index = DMA1_MAX_CHANNEL + channel;
            }
        }
    }

    if (dma_index != INVALID_DMA_INDEX)
    {
        DMA_CHANNEL[dma_index]->CCR |= DMA_CCR_EN;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief  Disables the specified DMAy Channelx.
  *
  * @param  dma_engine : Specify the Dma Engine
  * @param  channel    : Defines the specific Dma channel from the DMA_CHANNEL_TYPE
  */
void Dma__Disable(DMA_ENGINE_TYPE dma_engine, DMA_CHANNEL_TYPE channel)
{
    uint8 dma_index;

    dma_index = INVALID_DMA_INDEX;

    if (channel)
    {
        if (dma_engine == DMA_ENGINE1)
        {
            if(channel <= DMA1_MAX_CHANNEL )
            {
                dma_index = channel;
            }
        }

        else if (dma_engine == DMA_ENGINE2)
        {
            if(channel <= DMA2_MAX_CHANNEL )
            {
                dma_index = DMA1_MAX_CHANNEL + channel;
            }
        }
    }

    if (dma_index != INVALID_DMA_INDEX)
    {
        DMA_CHANNEL[dma_index]->CCR &= (uint16)(~DMA_CCR_EN);
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief  Returns the number of remaining data units in the current
  *         DMAy Channelx transfer.
  *
  * @param  dma_engine : Specify the Dma Engine
  * @param  channel    : Defines the specific Dma channel from the DMA_CHANNEL_TYPE
  *
  * @retval The number of remaining data units in the current DMAy Channelx
  *         transfer.
  */
DMA_SIZE_TYPE Dma__GetCurrDataCounter(DMA_ENGINE_TYPE dma_engine, DMA_CHANNEL_TYPE channel)
{
    uint8 dma_index;

    dma_index = INVALID_DMA_INDEX;

    if (channel)
    {
        if (dma_engine == DMA_ENGINE1)
        {
            if(channel <= DMA1_MAX_CHANNEL )
            {
                dma_index = channel;
            }
        }

        else if (dma_engine == DMA_ENGINE2)
        {
            if(channel <= DMA2_MAX_CHANNEL )
            {
                dma_index = DMA1_MAX_CHANNEL + channel;
            }
        }
    }

    if (dma_index != INVALID_DMA_INDEX)
    {
      // Return the number of remaining data units for DMAy Channelx
      return ((DMA_SIZE_TYPE)(DMA_CHANNEL[dma_index]->CNDTR));
    }
    else
    {
        return(0);
    }

}

//---------------------------------------------------------------------------------------------------------------------
/**
  * @brief  Returns the status of the specified Dma channel.
  *
  * @param  dma_engine : Specify the Dma Engine
  * @param  channel    : Defines the specific Dma channel from the DMA_CHANNEL_TYPE
  * @retval DMA_TRANSFER_STATUS_TYPE-
  *      @retval DMA_TRANSFER_IDLE      : Dma channel is in idle state
  *      @retval DMA_TRANSFER_ERROR     : Dma channel encountered data transfer error
  *      @retval DMA_TRANSFER_ONGOING   : Dma channel is transferring data
  *      @retval DMA_TRANSFER_COMPLETE  : Dma channel completed data transfer
  */
DMA_TRANSFER_STATUS_TYPE Dma__GetFlagStatus(DMA_ENGINE_TYPE dma_engine, DMA_CHANNEL_TYPE channel)
{
    DMA_TRANSFER_STATUS_TYPE status;
    uint8 dma_index;


    status = DMA_TRANSFER_IDLE;

    dma_index = INVALID_DMA_INDEX;

    if (channel)
    {
        if (dma_engine == DMA_ENGINE1)
        {
            if(channel <= DMA1_MAX_CHANNEL )
            {
                dma_index = channel;
            }
        }

        else if (dma_engine == DMA_ENGINE2)
        {
            if(channel <= DMA2_MAX_CHANNEL )
            {
                dma_index = DMA1_MAX_CHANNEL + channel;
            }
        }
    }

    if (dma_index != INVALID_DMA_INDEX)
    {
        if ((DMA_CHANNEL[dma_index]->CNDTR) != 0)
        {
            status = DMA_TRANSFER_ONGOING;
        }
        else
        {
            if (dma_index <= DMA1_MAX_CHANNEL)
            {
                // Check the status of the specified DMA flag
                if ((DMA1.ISR & (DMA_ISR_TEIF1 <<dma_index*4)) != FALSE)
                {
                    status = DMA_TRANSFER_ERROR;
                }
                else if ((DMA1.ISR & (DMA_ISR_TCIF1 <<dma_index*4)) != FALSE)
                {
                    status = DMA_TRANSFER_COMPLETE;
                }

                //Clear all the flags
                DMA1.IFCR &= (uint32)(~((uint32)(1 << dma_index)));
            }
            else
            {
                // Check the status of the specified DMA flag
                if ((DMA2.ISR & (DMA_ISR_TEIF1 <<dma_index*4)) != FALSE)
                {
                    status = DMA_TRANSFER_ERROR;
                }
                else if ((DMA2.ISR & (DMA_ISR_TCIF1 <<dma_index*4)) != FALSE)
                {
                    status = DMA_TRANSFER_COMPLETE;
                }

                //Clear all the flags
                DMA2.IFCR &= (uint32)(~((uint32)(1 << dma_index)));
            }
        }
    }

    // Return the DMA_FLAG status
    return  status;
}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================



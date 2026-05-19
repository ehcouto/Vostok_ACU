/**
 *  @file       
 *
 *  @brief      Direct memory access (DMA) is used in order to provide high-speed data transfer between
 *              peripherals and memory as well as memory to memory. Data can be quickly moved by DMA
 *              without any CPU actions. This keeps CPU resources free for other operations.
 *
 *  @section    Applicable_Documents
 *                  List here all the applicable documents if needed. <tr>
 *
 *  $Header: Dma.h 1.1 2015/07/30 14:39:00EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2013-$Date: 2015/07/30 14:39:00EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef DMA_H_
#define DMA_H_

#include "C_Extensions.h"
#include "Dma_def.h"
#include "Dma_prm.h"

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================
//! DMA_ENGINE_TYPE defines the list of Dma engines supported by the controller
typedef enum
{
    DMA_ENGINE0,
    DMA_ENGINE1,
    DMA_ENGINE2,
    DMA_ENGINE3,
    DMA_ENGINE4,
    DMA_ENGINE5,
    DMA_ENGINE6,
    DMA_ENGINE7,
    DMA_ENGINE8,
    DMA_ENGINE9,
    DMA_ENGINE10,
    DMA_ENGINE11,
    DMA_ENGINE12,
    DMA_ENGINE13,
    DMA_ENGINE14,
    DMA_ENGINE15,
    DMA_ENGINE16,
    DMA_ENGINE17,
    DMA_ENGINE18,
    DMA_ENGINE19
}DMA_ENGINE_TYPE;


//! DMA_CHANNEL_TYPE defines the list of channels the API supports
//! Depending on the implementation not all channels in this list will have support
typedef enum
{
    DMA_CH0,
    DMA_CH1,
    DMA_CH2,
    DMA_CH3,
    DMA_CH4,
    DMA_CH5,
    DMA_CH6,
    DMA_CH7,
    DMA_CH8,
    DMA_CH9,
    DMA_CH10,
    DMA_CH11,
    DMA_CH12,
    DMA_CH13,
    DMA_CH14,
    DMA_CH15,
    DMA_CH16,
    DMA_CH17,
    DMA_CH18,
    DMA_CH19
}DMA_CHANNEL_TYPE;

//! Defines the return from the Dma Driver
typedef enum
{
    DMA_TRANSFER_IDLE,
    DMA_TRANSFER_ERROR,
    DMA_TRANSFER_ONGOING,
    DMA_TRANSFER_COMPLETE,
}DMA_TRANSFER_STATUS_TYPE;

//! Defines the return from the Dma driver
typedef enum
{
    DMA_RETURN_FAIL,
    DMA_RETURN_SUCCESS
}DMA_RETURN_TYPE;

//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================
void Dma__EngineInitialize(void);
DMA_RETURN_TYPE Dma__Initialize(DMA_ENGINE_TYPE dma_engine, DMA_CHANNEL_TYPE channel, DMA_INIT_TYPE* DMA_InitStruct);
BOOL_TYPE Dma__StartTransfer(DMA_ENGINE_TYPE dma_engine, DMA_CHANNEL_TYPE channel, DMA_SIZE_TYPE DataNumber);
void Dma__Enable(DMA_ENGINE_TYPE dma_engine, DMA_CHANNEL_TYPE channel);
void Dma__Disable(DMA_ENGINE_TYPE dma_engine, DMA_CHANNEL_TYPE channel);
void Dma__StopTransfer(DMA_ENGINE_TYPE dma_engine, DMA_CHANNEL_TYPE channel);
DMA_TRANSFER_STATUS_TYPE Dma__GetFlagStatus(DMA_ENGINE_TYPE dma_engine, DMA_CHANNEL_TYPE channel);
DMA_SIZE_TYPE Dma__GetCurrDataCounter(DMA_ENGINE_TYPE dma_engine, DMA_CHANNEL_TYPE channel);

#endif // DMA_H_

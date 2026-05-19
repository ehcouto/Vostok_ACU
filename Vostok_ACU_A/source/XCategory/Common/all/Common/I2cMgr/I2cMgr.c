/**
 *  @file       
 *
 *  @brief      This module acts as the manager for all the I2c channels on the micro-controller
 *
 *  @details    The module will grant or deny an i2c driver module access to a specific I2c channel on the micro-controller\n
 *  This module will ensure that only one device can operate over the I2c module at a time. The module will also\n
 *  ensure that each device will only operate over the bus for a predefined time. If the device maintains control of\n
 *  the I2C channel for too long, this module will kick the device from the bus.\n
 *
 *  $Header: I2cMgr.c 1.1 2016/08/31 13:12:31EDT Seth Herndon (HERNDS) Exp  $
 *
 *  @copyright  Copyright 2016-$Date: 2016/08/31 13:12:31EDT$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#include "SystemConfig.h"

#ifndef I2CMGR_FEATURE
    #error: I2CMGR_FEATURE must be defined in SystemConfig_prm.h as ENABLED or DISABLED
#endif

#if (I2CMGR_FEATURE == ENABLED)
//-------------------------------------- Include Files ----------------------------------------------------------------
#include "I2cMgr.h"
#include "I2cMgr_prv.h"
#include "Millis.h"
#include <string.h>

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief Define invalid i2c channel value
 * @note  This value is always 0xFF since there will never be 255 i2c channels.\n
 * #define I2CMGR_CHANNEL_INVALID       (0xFF)
 *
 */
#define I2CMGR_CHANNEL_INVALID       (0xFF)

typedef struct _I2CMGR_QUEUE_TYPE
{
    uint8 I2cMgr_TokenQueue[I2CMGR_QUEUE_SIZE];
    uint8 I2cMgr_WriteIndex;
    uint8 I2cMgr_ReadIndex;
}I2CMGR_QUEUE_TYPE;

typedef struct _I2CMGR_HANDLE_TYPE
{
    I2C_ENUM_TYPE i2c_channel;
}I2CMGR_HANDLE_TYPE;

static I2CMGR_HANDLE_TYPE I2cMgr_DeviceHandle[I2CMGR_NUM_DEVICES];
static MILLIS_TIMER_TYPE I2cMgr_TimeoutHandle[I2CMGR_NUM_CHANNELS];

#if(I2CMGR_NUM_DEVICES > 1)
static I2CMGR_QUEUE_TYPE I2cMgr_RequestQueue[I2CMGR_NUM_CHANNELS];
#endif

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
static void TimeoutAction(I2C_ENUM_TYPE i2c_channel);
#if(I2CMGR_NUM_DEVICES > 1)
static void AddItemToQueue(uint8 device_id);
static void RemoveItemFromQueue(uint8 device_id);
#endif

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

//---------------------------------------------------------------------------------------------------------------------
/**
 *  @brief      It Initializes the module I2cMgr and its variables
 *
 */
void I2cMgr__Initialize(void)
{
    uint8 index;

    memset(I2cMgr_DeviceHandle, I2CMGR_CHANNEL_INVALID, sizeof(I2cMgr_DeviceHandle));
    memset(I2cMgr_TimeoutHandle, 0, sizeof(I2cMgr_TimeoutHandle));

#if(I2CMGR_NUM_DEVICES > 1)
    memset(I2cMgr_RequestQueue, 0, sizeof(I2cMgr_RequestQueue));
#endif

    for(index= 0; index < I2CMGR_NUM_CHANNELS; index++)
    {
        Millis__Reset(&I2cMgr_TimeoutHandle[index]);

#if(I2CMGR_NUM_DEVICES > 1)
        memset(I2cMgr_RequestQueue[index].I2cMgr_TokenQueue, I2CMGR_HANDLE_INVALID, sizeof(I2cMgr_RequestQueue[index].I2cMgr_TokenQueue));
#endif
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method obtains a handle from the array for the requesting device
 * @details This method is necessary to prepare a device to be used with I2C correctly. It lets the manager know which\n
 * channel the device needs to operate on as well.
 *
 * @param i2c_channel - define the i2c channel the calling device is operating on
 * @retval uint8 device_id - define the index into the handle array for devices
 */
uint8 I2cMgr__GetDeviceHandle(I2C_ENUM_TYPE i2c_channel)
{
    uint8 device_id = I2CMGR_HANDLE_INVALID;
    uint8 index;

    if(i2c_channel < I2CMGR_NUM_CHANNELS)
    {
        for(index = 0; index < I2CMGR_NUM_DEVICES; index++)
        {
            if(I2cMgr_DeviceHandle[index].i2c_channel == (I2C_ENUM_TYPE) I2CMGR_CHANNEL_INVALID)
            {
                I2cMgr_DeviceHandle[index].i2c_channel = i2c_channel;
                device_id = index;
                break; // Break when a free handle is found
            }
        }
    }

    return device_id;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method releases a handle from the array for the requesting device
 * @details This method releases a device from the handle array and creates room for a new device.
 *
 * @param uint8 device_id - define the index into the handle array for devices
 */
void I2cMgr__ReleaseDeviceHandle(uint8 device_id)
{
    if(device_id < I2CMGR_NUM_DEVICES)
    {
        I2cMgr_DeviceHandle[device_id].i2c_channel = (I2C_ENUM_TYPE) I2CMGR_CHANNEL_INVALID;
    }
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method gives a calling device access to an I2c channel.
 * @details The method is used to grant access to a specific i2c channel's bus. The method returns true to allow\n
 * the calling device to operate over i2c. It will return false if the i2c bus is not free
 *
 * @param uint8 device_id - define the index into the handle array for devices
 *
 * @retval FALSE which means the the requested i2c write operation cannot be performed since i2c bus is busy
 * @retval TRUE which means the requested i2c write operation is completed
 */
BOOL_TYPE I2cMgr__RequestBus(uint8 device_id)
{
    BOOL_TYPE response = FALSE;
    BOOL_TYPE exists = FALSE;
    uint8 index;

    if(device_id < I2CMGR_NUM_DEVICES)
    {
        if(I2cMgr_DeviceHandle[device_id].i2c_channel != (I2C_ENUM_TYPE) I2CMGR_CHANNEL_INVALID)
        {
#if(I2CMGR_NUM_DEVICES > 1)
            // See if the device is already in the queue
            for(index = 0; index < I2CMGR_QUEUE_SIZE; index++)
            {
                if(I2cMgr_RequestQueue[I2cMgr_DeviceHandle[device_id].i2c_channel].I2cMgr_TokenQueue[index] == device_id)
                {
                    exists = TRUE;
                    break;
                }
            }

            // Add the device to the queue if it isnt already in
            if((I2cMgr_RequestQueue[I2cMgr_DeviceHandle[device_id].i2c_channel].I2cMgr_TokenQueue[I2cMgr_RequestQueue[I2cMgr_DeviceHandle[device_id].i2c_channel].I2cMgr_WriteIndex] == I2CMGR_HANDLE_INVALID) &&
               (exists == FALSE))
            {
                AddItemToQueue(device_id);
            }

            // Process the next item in the queue for request
            // The device gets access if it is the current device at the read index of the queue
            if(I2cMgr_RequestQueue[I2cMgr_DeviceHandle[device_id].i2c_channel].I2cMgr_TokenQueue[I2cMgr_RequestQueue[I2cMgr_DeviceHandle[device_id].i2c_channel].I2cMgr_ReadIndex] == device_id)
            {
                // Prepare timeout timer for countdown
                Millis__Start(&I2cMgr_TimeoutHandle[I2cMgr_DeviceHandle[device_id].i2c_channel]);
                response = TRUE;
            }
#else       // Return true is only 1 device exists since there is no possible interference
            // Prepare timeout timer for countdown
            Millis__Start(&I2cMgr_TimeoutHandle[I2cMgr_DeviceHandle[device_id].i2c_channel]);

            response = TRUE;
#endif
        }
    }

    return response;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method releases the i2c bus if it is being used by the calling device.
 * @details The method will only release the bus if the calling device currently owns the bus operation
 *
 * @param uint8 device_id - define the index into the handle array for devices
 *
 *  @retval TRUE which means the i2c bus was released successfully
 *  @retval FALSE which means the i2c bus was not released
 */
BOOL_TYPE I2cMgr__ReleaseBus(uint8 device_id)
{
    BOOL_TYPE response = FALSE;

    if(device_id < I2CMGR_NUM_DEVICES)
    {
        if(I2cMgr_DeviceHandle[device_id].i2c_channel != (I2C_ENUM_TYPE) I2CMGR_CHANNEL_INVALID)
        {
#if(I2CMGR_NUM_DEVICES > 1)
            // The device can only release the bus if it currently has control
            if(I2cMgr_RequestQueue[I2cMgr_DeviceHandle[device_id].i2c_channel].I2cMgr_TokenQueue[I2cMgr_RequestQueue[I2cMgr_DeviceHandle[device_id].i2c_channel].I2cMgr_ReadIndex] == device_id)
            {
                RemoveItemFromQueue(device_id);

                // Invalidate timeout for the channel
                Millis__Reset(&I2cMgr_TimeoutHandle[I2cMgr_DeviceHandle[device_id].i2c_channel]);

                response = TRUE;
            }
#else       // Return true is only 1 device exists since there is no possible interference
            // Invalidate timeout for the channel
            Millis__Reset(&I2cMgr_TimeoutHandle[I2cMgr_DeviceHandle[device_id].i2c_channel]);

            response = TRUE;
#endif
        }
    }

    return response;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This handler is called constantly by the OS to calculate timeouts
 * @details This method will call the timeout action when a timeout occurs. The method will also keep track of the\n
 * timeout for each i2c channel.
 */
void I2cMgr__Handler(void)
{
    uint8 index;

    // Process each timeout variable to see if time has expired for the channel
    for(index = 0; index < I2CMGR_NUM_CHANNELS; index++)
    {

#if(I2CMGR_NUM_DEVICES > 1)
        if((Millis__IsTimerPaused(&I2cMgr_TimeoutHandle[index]) != FALSE) &&
           ((I2cMgr_RequestQueue[index].I2cMgr_TokenQueue[I2cMgr_RequestQueue[index].I2cMgr_ReadIndex]) != I2CMGR_HANDLE_INVALID))
        {
             Millis__Start(&I2cMgr_TimeoutHandle[index]);
        }
#endif

        if(Millis__ReachedTarget(&I2cMgr_TimeoutHandle[index], I2CMGR_TIMEOUT_VALUE))
        {
            TimeoutAction((I2C_ENUM_TYPE) index);
        }
    }
}
//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method is used to process a timeout for an i2c channel
 * @details This method will either reset the i2c bus if it is locked or it will remove the token from the device\n
 * currently holding it.
 *
 * @param i2c_channel - define the i2c channel the calling device is operating on
 */
static void TimeoutAction(I2C_ENUM_TYPE i2c_channel)
{
    // Only reset the I2c bus if it is locked up
    if(I2c__GetStatus(i2c_channel, I2C_STATUS_ERROR) == I2C_ERROR_BUS_LOCKED)
    {
        I2c__ResetBus(i2c_channel);
    }

    // Invalidate the timeout variable
    Millis__Reset(&I2cMgr_TimeoutHandle[i2c_channel]);

#if(I2CMGR_NUM_DEVICES > 1)
    // Remove the current item at the read index for the channel that has timed out
    RemoveItemFromQueue(I2cMgr_RequestQueue[i2c_channel].I2cMgr_TokenQueue[I2cMgr_RequestQueue[i2c_channel].I2cMgr_ReadIndex]);
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method is used to add a device to the i2c channel request queue
 * @details This method will add the current device id passed to the position in the token queue pointed to by the\n
 * write index.
 *
* @param uint8 device_id - define the index into the handle array for devices
 */
#if(I2CMGR_NUM_DEVICES > 1)
static void AddItemToQueue(uint8 device_id)
{
    // Add device to the current queue
    I2cMgr_RequestQueue[I2cMgr_DeviceHandle[device_id].i2c_channel].I2cMgr_TokenQueue[I2cMgr_RequestQueue[I2cMgr_DeviceHandle[device_id].i2c_channel].I2cMgr_WriteIndex] = device_id;

    // Move the token queue pointer to the next write spot
    I2cMgr_RequestQueue[I2cMgr_DeviceHandle[device_id].i2c_channel].I2cMgr_WriteIndex++;
    I2cMgr_RequestQueue[I2cMgr_DeviceHandle[device_id].i2c_channel].I2cMgr_WriteIndex %= I2CMGR_QUEUE_SIZE;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method is used to remove a device from the i2c channel request queue
 * @details This method will remove the current device id passed from the position in the token queue pointed to by the\n
 * read index.
 *
* @param uint8 device_id - define the index into the handle array for devices
 */
static void RemoveItemFromQueue(uint8 device_id)
{
    // Remove device to the current queue
    I2cMgr_RequestQueue[I2cMgr_DeviceHandle[device_id].i2c_channel].I2cMgr_TokenQueue[I2cMgr_RequestQueue[I2cMgr_DeviceHandle[device_id].i2c_channel].I2cMgr_ReadIndex] = I2CMGR_HANDLE_INVALID;

    // Move the token queue pointer to read next device
    I2cMgr_RequestQueue[I2cMgr_DeviceHandle[device_id].i2c_channel].I2cMgr_ReadIndex++;
    I2cMgr_RequestQueue[I2cMgr_DeviceHandle[device_id].i2c_channel].I2cMgr_ReadIndex %= I2CMGR_QUEUE_SIZE;
}
#endif  //I2CMGR_NUM_DEVICES > 1
#endif  //I2CMGR_FEATURE == ENABLED

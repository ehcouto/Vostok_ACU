/**
 *  @file       
 *
 *  @brief      SR Reading and Filtering of an I2C Pressure Sensor
 *
 *  @details    Interacts with the I2C driver to request and read Pressures sensor data.
 *              This Module Runs an IIR Filter on the Incoming data. Returns a known invalid value while the filter settles.
 *              If the I2C data request is not received within a configured time a fault is set.
 *
 *  @defgroup   CLASS_B
 *
 *  @copyright  Copyright 2018. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------

#include "SRI2CPressureSensorLLI.h"

#if (HBL_LLI_NUM_SRI2C_PRESSURE_SENSOR > 0U)

#include "Crc8.h"
#include "I2c.h"
#include "Micro_Defs.h"
#include "SRData.h"
#include "SRException.h"
#include "SRI2CPresureSensorDefs.h"
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    #include "SRTaskSequence.h"
#endif
#include <string.h>
#include "VirtualPin.h"

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------

//! Hold initial value of the filter
#define PRESURE_SENSOR_INITIAL_FILTER_VALUE     0

//! Holds the LLI Output value while the filter is stabilizing;
static const sint32 STABILIZING_PRESSURE = SRI2CPRESSURESENSOR_STABILIZING;

//! Holds the LLI Output value if there is an active I2C communication failure
static const sint32 I2C_COMM_FAILURE_PRESSURE = SRI2CPRESSURESENSOR_I2C_FAILURE;

//! The maximum bit value for the Alpha used on the IIR filter.
//! A maximum bit value of 15 results in the heaviest filtering provided by an alpha of 1/32768.
#define IIRFILTER_MAX_COEFFICIENT_BIT   15

//! Rate at which the Asynchronous function is called. This is used for stabilizing time.
#define ASYNCH_CALL_RATE 5U

//! Identifies the format of the raw and filtered values in the IIR filter.
typedef sint32 FILTER_DATA_TYPE;

//! A structure that holds the variables required for the IIR filter functionality.
PACKED typedef struct IIR_FILTER_MEASURE_STRUCT
{
    //! The filtered value provided by the filter. This value is always available after initialization.
        sint32 Filtered_Value;

    //! The accumulator value contains a fixed-point version of the filtered value.
    //! This fixed-point value uses higher precision based on the alpha coefficient used.
        sint32 Accumulator;

    //! This value is initially the inverted alpha coefficient. After the Filter__Initialize()
    //! function is called, this value is converted to the number of bits to shift between the
    //! accumulator and the filtered value.
    uint16 Inverted_Alpha;
} IIR_FILTER_MEASURE_TYPE;

typedef enum
{
    I2C_PRESSURE_SENSOR_REQUEST = 0,
    I2C_PRESSURE_SENSOR_READ
}I2C_PRESSURE_SENSOR_STATE_TYPE;

//! Definition of setting file parameters for the module
PACKED typedef struct
{
    uint16 Unused_1;                        // 2 bytes reserved for all LLI modules (none of the data applies to this LLI)
    uint8  Unused_2;                        // Currently not used (previously Device Type, but this is configured by hardware in the device); kept for backwards compatibility
    uint8  Inverted_Alpha;                  // Feedback filter coefficient for IIR filter
    uint16 Initial_Time;
    uint8  Communication_Fault_Debounce;    // Amount of time for the I2C communication failure to be present before a recovery/retry
    uint8  Communication_Fault_Retries;     // Number of recovery retries to attempt before reporting a failure to the application
}I2C_PRESSURE_SENSOR_PARAM_TYPE;

//! Definition of variables used by the module
PACKED typedef struct
{
    I2C_PRESSURE_SENSOR_STATE_TYPE State;
    IIR_FILTER_MEASURE_TYPE Filter;
    uint16 Settling_Timer;
    uint16 Pressure;
    uint8 Communication_Fault_Debounce;             // Hold the debounce limit configure in setting file
    uint8 Communication_Fault_Retries;              // Hold the retires limit configure in setting file
    uint8 Communication_Fault_Debounce_Counter;     // This counter counts till Communication_Fault_Debounce value then retry to recovery from I2C communication failure
    uint8 Communication_Fault_Retry_Counter;        // This counter counts till Communication_Fault_Retries value then report I2C communication failure
    uint8 Buffer[4];
}I2C_PRESSURE_SENSOR_DATA_TYPE;


//! CRC of the data for the module to be used when checking the integrity of the data
static uint8 SRI2CPressureSensorLLI_Crc[HBL_LLI_NUM_SRI2C_PRESSURE_SENSOR];

//! Module data used to request and process I2C Pressure Sensor data
static I2C_PRESSURE_SENSOR_DATA_TYPE Sr_I2c_Pressure_Data[HBL_LLI_NUM_SRI2C_PRESSURE_SENSOR];

//! Tracks the number of SR I2C Pressure Sensors Configured in the Setting file.
static uint8 Sr_I2CPressure_Sensor_LLI_Allocation;

//! Inverted variable for Class B verification
static uint8 N_Sr_I2CPressure_Sensor_LLI_Allocation;

//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------

void InitIirFilter(void* filter_struct, FILTER_DATA_TYPE initial_value);
void UpdateIirFilter(void* filter_struct, FILTER_DATA_TYPE new_sample_value);

//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   It Initializes the module I2CPressureSensor LLI and its variables
 */
void SRI2CPressureSensorLLI__Initialize(void)
{
    memset(Sr_I2c_Pressure_Data,0,sizeof(Sr_I2c_Pressure_Data));

    for (uint8 index = 0; index < HBL_LLI_NUM_SRI2C_PRESSURE_SENSOR; index++)
    {
        SRI2CPressureSensorLLI_Crc[index] = Crc8(0xFF, (uint8 *)(void *)&Sr_I2c_Pressure_Data[index], sizeof(I2C_PRESSURE_SENSOR_DATA_TYPE));
    }

    SR_SET_BYTE(Sr_I2CPressure_Sensor_LLI_Allocation,0);
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_INIT, SRTASKSEQUENCE_APPL_INIT_SRI2C_PRESSURE_SENSOR_LLI);
#endif
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Determines the destination (index) of the data for the LLI.
 * @return  Index to a local buffer where the data will reside.
 */
uint8 SRI2CPressureSensorLLI__Allocate(void)
{
    uint8 retval;
    retval = HBL_INDEX_INVALID;
    SR_CHECK_BYTE(Sr_I2CPressure_Sensor_LLI_Allocation);
    if (Sr_I2CPressure_Sensor_LLI_Allocation < HBL_LLI_NUM_SRI2C_PRESSURE_SENSOR)
    {
        retval = Sr_I2CPressure_Sensor_LLI_Allocation;
        SR_SET_BYTE(Sr_I2CPressure_Sensor_LLI_Allocation,Sr_I2CPressure_Sensor_LLI_Allocation + 1U);
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Initialize an instance of the LLI by extracting parameter data and initializing variables.
 * @param   input: data related to the instance of the LLI
 * @param   PASS if the initialization was successful, else FAIL
 */
PASS_FAIL_TYPE SRI2CPressureSensorLLI__InitializeInstance(HBL_LLI_TYPE * input)
{
    PASS_FAIL_TYPE retval = FAIL;
    I2C_PRESSURE_SENSOR_PARAM_TYPE *param_ptr;
    uint8 i2c_address = 0;

    SR_CHECK_BYTE(Sr_I2CPressure_Sensor_LLI_Allocation);
    if (input->Data_Index < Sr_I2CPressure_Sensor_LLI_Allocation)
    {
        param_ptr = (I2C_PRESSURE_SENSOR_PARAM_TYPE *) (void *)(input->Parameter_Ptr.Data);

        const VIRTUALPIN_TYPE* pin_ptr = &(VIRTUALPIN_TABLE[input->Pin_Ptr[0]]);

        i2c_address = (((pin_ptr->param0) << 1) & 0xFFU);

        retval = (PASS_FAIL_TYPE)I2c__Initialize((I2C_ENUM_TYPE)pin_ptr->device, (I2C_SPEED_TYPE)pin_ptr->param1, I2C_ADDR_7BITS, i2c_address); // initialize I2C bus with this sensor

        Sr_I2c_Pressure_Data[input->Data_Index].Settling_Timer = param_ptr->Initial_Time/ASYNCH_CALL_RATE;

        Sr_I2c_Pressure_Data[input->Data_Index].Filter.Inverted_Alpha = param_ptr->Inverted_Alpha;

        Sr_I2c_Pressure_Data[input->Data_Index].Communication_Fault_Debounce = param_ptr->Communication_Fault_Debounce;

        Sr_I2c_Pressure_Data[input->Data_Index].Communication_Fault_Retries = param_ptr->Communication_Fault_Retries;

        //Initialize filter
        InitIirFilter((void *)&(Sr_I2c_Pressure_Data[input->Data_Index].Filter), PRESURE_SENSOR_INITIAL_FILTER_VALUE);
        SRI2CPressureSensorLLI_Crc[input->Data_Index] = Crc8(0xFF, (uint8 *)(void *)&Sr_I2c_Pressure_Data[input->Data_Index], sizeof(I2C_PRESSURE_SENSOR_DATA_TYPE));
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Retrieves the current value of the LLI
 * @param   input: data related to the instance of the LLI
 * @retval  NULL = lli instance not allocated
 *          STABILIZING_PRESSURE = Filter is settling
 *          else Current LLI Value
 */
void * SRI2CPressureSensorLLI__Get(HBL_LLI_TYPE * input)
{
    void* ptr = NULL;

    SR_CHECK_BYTE(Sr_I2CPressure_Sensor_LLI_Allocation);

    if (input->Data_Index < Sr_I2CPressure_Sensor_LLI_Allocation)
    {
        MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
        BOOL_TYPE test_crc_match = ((SRI2CPressureSensorLLI_Crc[input->Data_Index] == (Crc8(0xFF, (uint8 *)(void *)&Sr_I2c_Pressure_Data[input->Data_Index], sizeof(I2C_PRESSURE_SENSOR_DATA_TYPE)))) ? TRUE : FALSE);
        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();

        if(test_crc_match == FALSE)
        {
            SRException__Queue(SREXCEPTION_SRCHECK_DATA_NOT_CONSISTENT,input->LLI_Handler_Index, input->Data_Index);
        }

        if (Sr_I2c_Pressure_Data[input->Data_Index].Settling_Timer == 0U)
        {

            if (Sr_I2c_Pressure_Data[input->Data_Index].Communication_Fault_Retry_Counter > Sr_I2c_Pressure_Data[input->Data_Index].Communication_Fault_Retries)
            {
                // Return unique pressure value if there is an I2c communication failure
                ptr = (void *)&I2C_COMM_FAILURE_PRESSURE;
            }
            else
            {
                ptr =  ((void *)&Sr_I2c_Pressure_Data[input->Data_Index].Filter.Filtered_Value);
            }
        }
        else
        {
            ptr = (void *)&STABILIZING_PRESSURE;
        }
    }
    return ptr;
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Asynchronous processing of a the LLI
 * @param   input: data related to the instance of the LLI
 * @return  PASS if the LLI was successfully processed, else FAIL
 */
BOOL_TYPE SRI2CPressureSensorLLI__AsynchProcess(HBL_LLI_TYPE * input)
{
    BOOL_TYPE retval = FALSE;
    I2C_PRESSURE_SENSOR_DATA_TYPE *ptr;

    SR_CHECK_BYTE(Sr_I2CPressure_Sensor_LLI_Allocation);
    if (SRI2CPressureSensorLLI_Crc[input->Data_Index] == Crc8(0xFF, (uint8 *)(void *)&Sr_I2c_Pressure_Data[input->Data_Index], sizeof(I2C_PRESSURE_SENSOR_DATA_TYPE)))
    {
        if (input->Data_Index < Sr_I2CPressure_Sensor_LLI_Allocation)
        {
            ptr = &Sr_I2c_Pressure_Data[input->Data_Index];

            ptr->Settling_Timer = (ptr->Settling_Timer > 0U) ? (ptr->Settling_Timer - 1U) : 0U;

            const VIRTUALPIN_TYPE* pin_ptr = &(VIRTUALPIN_TABLE[input->Pin_Ptr[0]]);

            switch(Sr_I2c_Pressure_Data[input->Data_Index].State)
            {
                case I2C_PRESSURE_SENSOR_REQUEST:

                    retval = I2c__RequestRead((I2C_ENUM_TYPE)pin_ptr->device, 2);                           // Request the reading to the device
                    ptr->State = I2C_PRESSURE_SENSOR_READ;
                    break;

                case I2C_PRESSURE_SENSOR_READ:

                    if (I2c__GetStatus((I2C_ENUM_TYPE)pin_ptr->device,I2C_STATUS_STATE) == (uint16)I2C_STATE_IDLE)  // Check if bus is free
                    {
                        retval = I2c__Read((I2C_ENUM_TYPE)pin_ptr->device, Sr_I2c_Pressure_Data[input->Data_Index].Buffer, sizeof(Sr_I2c_Pressure_Data[input->Data_Index].Buffer)); // Copy the I2c Rx buffer to the I2c_Pressure_Buffer

                        ptr->Pressure  = (((uint16)ptr->Buffer[0]) << 8) | ptr->Buffer[1]; // Merge the pressure data bytes
                        ptr->Pressure &= (uint16)0x3FFF;    // Mask off 2 status bits

                        //Update Filter
                        UpdateIirFilter((void *)&(ptr->Filter), (FILTER_DATA_TYPE)(ptr->Pressure));

                        ptr->Communication_Fault_Debounce_Counter = 0;
                        ptr->Communication_Fault_Retry_Counter = 0;

                        ptr->State = I2C_PRESSURE_SENSOR_REQUEST;
                    }
                    else if (ptr->Communication_Fault_Retry_Counter <= ptr->Communication_Fault_Retries)    // Handle communication error
                    {
                        // Max retries not reached; keep debouncing the communication error and/or attempting to recover the I2C communication
                        if (ptr->Communication_Fault_Debounce_Counter < ptr->Communication_Fault_Debounce)
                        {
                            // No action; wait for debounce counter to expire or for communication to be restored
                            ptr->Communication_Fault_Debounce_Counter++;
                        }
                        else if (ptr->Communication_Fault_Debounce_Counter >= ptr->Communication_Fault_Debounce)
                        {
                            // Attempt recovery of I2C communication and retry data request
                            I2c__Clear((I2C_ENUM_TYPE)pin_ptr->device);
                            (void)I2c__ResetBus((I2C_ENUM_TYPE)pin_ptr->device);
                            ptr->Communication_Fault_Debounce_Counter = 0;
                            ptr->Communication_Fault_Retry_Counter++;
                            ptr->State = I2C_PRESSURE_SENSOR_REQUEST;
                        }
                    }
                    else //ptr->Communication_Fault_Retry_Counter > SRI2cPressureSensorLLI_Param[input->Data_Index]->Communication_Fault_Retries
                    {
                        //Attempt recovery of I2C communication and retry data request
                        I2c__Clear((I2C_ENUM_TYPE)pin_ptr->device);
                        (void)I2c__ResetBus((I2C_ENUM_TYPE)pin_ptr->device);
                        ptr->State = I2C_PRESSURE_SENSOR_REQUEST;
                    }

                    break;

                default:
                    // lint -fallthrough
                    break;
            }
            SRI2CPressureSensorLLI_Crc[input->Data_Index] = Crc8(0xFF, (uint8 *)(void *)&Sr_I2c_Pressure_Data[input->Data_Index], sizeof(I2C_PRESSURE_SENSOR_DATA_TYPE));
        }
    }
    else
    {
        SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, input->LLI_Handler_Index, input->Data_Index);
    }
    return (retval);
}

//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief   Handler for periodic SRI2CPressureSensorLLI Data Verification.
 */
void SRI2CPressureSensorLLI__Handler(void)
{
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    BOOL_TYPE retval;
    uint8 index;

    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
    retval = (BOOL_TYPE) (SR_CHECK_BYTE(Sr_I2CPressure_Sensor_LLI_Allocation) == SRDATA_OK);
    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();

    for (index = 0; index < HBL_LLI_NUM_SRI2C_PRESSURE_SENSOR; index++)
    {
        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();
        retval &= (BOOL_TYPE) (Crc8(0xFF, (uint8 *) &Sr_I2c_Pressure_Data[index], sizeof(I2C_PRESSURE_SENSOR_DATA_TYPE)) == SRI2CPressureSensorLLI_Crc[index]); //lint !e928 Suppress Info: cast from pointer to pointer [MISRA 2004 Rule 11.4]
        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();
        if (retval == FALSE)
        {
            SRException__Queue(SREXCEPTION_HBL_DATA_NOT_CONSISTENT, 0, index);
            break;
        }
    }
#if (SYSTEM_ENABLE_TASKSEQUENCE == ENABLED)
    SRTaskSequence__SetSequenceStep(SRTASKSEQUENCE_APPL_SAFETY_THREAD, SRTASKSEQUENCE_APPL_SRI2C_PRESSURE_SENSOR_LLI_CHECKS);
#endif

}

//=====================================================================================================================
//-------------------------------------- Private Functions ------------------------------------------------------------
//=====================================================================================================================
//---------------------------------------------------------------------------------------------------------------------
/**
 * Initialize the filter data structure.
 * @param filter_struct = The structure used by the filter.
 * @param initial_value = The initial value that the filter should be set to.
 * @param filter_type = The type of filter that is being initialized.
 */
void InitIirFilter(void* filter_struct, FILTER_DATA_TYPE initial_value)
{
    IIR_FILTER_MEASURE_TYPE* iir_filter;
    uint16 i;

    // Convert the inverted alpha to the number of bits that will be shifted.
    iir_filter = (IIR_FILTER_MEASURE_TYPE*)filter_struct;

    for (i = IIRFILTER_MAX_COEFFICIENT_BIT; i > 0U; i--)
    {
        if (BIT_TEST(iir_filter->Inverted_Alpha, i))
        {
            break;  // Exit the loop
        }
    }
    iir_filter->Inverted_Alpha = i;

    // Setup the Accumulator value.
    iir_filter->Accumulator = initial_value << iir_filter->Inverted_Alpha;

    iir_filter->Filtered_Value = initial_value;
}


//---------------------------------------------------------------------------------------------------------------------
/**
 * Update the filter variables based on a new sampled value.
 * @param filter_struct = The structure used by the filter.
 * @param new_sample_value = The new sampled value that should be considered by the filter.
 * @param filter_type = The type of filter that is being updated.
 */
void UpdateIirFilter(void* filter_struct, FILTER_DATA_TYPE new_sample_value)
{
    IIR_FILTER_MEASURE_TYPE* iir_filter;
    FILTER_DATA_TYPE value;

    iir_filter = ((IIR_FILTER_MEASURE_TYPE*)filter_struct);
    iir_filter->Accumulator += new_sample_value - iir_filter->Filtered_Value;
    value = iir_filter->Accumulator;

    if (iir_filter->Inverted_Alpha > 1U)
    {
        value >>= iir_filter->Inverted_Alpha - 1;
    }
    if (iir_filter->Inverted_Alpha > 0U)
    {
        value += 1;
        value >>= 1;
    }
    iir_filter->Filtered_Value = value;
}

#endif // (HBL_LLI_NUM_SRI2C_PRESSURE_SENSOR > 0)

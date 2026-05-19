/**
 *  @file
 *  @defgroup CLASS_B
 *  @brief      This module handle the update and check of SR (Safety Relevant) Data.
 *
 *  @details    There is a set of functions for Byte,Short, Long ,Array of Bytes and floats.
 *  <pre>
 *  Related to the usage this module it's required that the variable and its complement should have a separation in memory.
 *  so they should not be next each other.
 *  We recommend 32bits of space in between the variables
 *  When declaring variables it can be done by declaring all the variables and after that all the complements.
 *  For example:
 *
 *      uint8 A;
 *      uint16 B;
 *      uint8 C
 *      uint32 D
 *
 *      uint8 NA;
 *      uint16 NB;
 *      uint8 NC;
 *      uint32 ND;
 *
 *  The space in between a variable and its complement recommended is minimum of 32bits.
 * </pre>
 *
 *  PCLint:
 *  This module converts pointers to uint32 on calls to SRException__Queue().
 *  The converted arguments are only used for debugging purposes so they will
 *  not affect any functional behaviors. Therefore, for these cases, it is
 *  acceptable to suppress the following PCLint note.
 *  "Note [923]: cast from pointer to unsigned long [MISRA 2012 Rule 11.6, required]"
 *
 *  @copyright  Copyright 2007-2023 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------

//-------------------------------------- Include Files ----------------------------------------------------------------
#include "C_Extensions.h"
#include "SRData.h"
#include "SRException.h"
#include "Micro.h"

//-------------------------------------- PUBLIC (Variables) -----------------------------------------------------------

//-------------------------------------- PRIVATE (Variables, Constants & Defines) -------------------------------------


#define SRDATA__COMPLEMENT(data)    (~(data))
//-------------------------------------- PRIVATE (Function Prototypes) ------------------------------------------------
#if (SRDATA_FLOAT_FEATURE == ENABLED)
    typedef union
    {
        float32 vfloat;
        uint32 vlong;
    }SRDATA_CONVERTION_F_L_TYPE;
#endif
//=====================================================================================================================
//-------------------------------------- Public Functions -------------------------------------------------------------
//=====================================================================================================================

#if (SRDATA_BYTE_FEATURE == ENABLED)
#ifdef __IAR_SYSTEMS_ICC__
    #pragma optimize = none
#endif
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method updates a Safety Byte Variable and it's backup variable
 *
 * @param data pointer to the SR data
 * @param ndata pointer to the complementary SR data
 * @param value value to be updated in the SR variable
 */
void SRData__UpdateByte(uint8 * data,uint8 * ndata,uint8 value)
{
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    //------- ISR Protected Code --------------------------------------------------------------------------------------
    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();                              // Save context and disable ISRs
    *data = value;                                  // load variable
    *ndata = SRDATA__COMPLEMENT(value);             // load inverted variable
    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();                           // Restore the ISR context previously saved.
    //-----------------------------------------------------------------------------------------------------------------
}


#ifdef __IAR_SYSTEMS_ICC__
#pragma optimize = none
#endif
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method checks the integrity of a Safety Byte Variable and it's backup
 *
 * @param data pointer to the SR data
 * @param ndata pointer to the complementary SR data
 * @return Result of the verification
 */
SRDATA_CHECK_TYPE SRData__CheckByte(uint8 * data,uint8 * ndata)
{
    volatile uint8 aux;
    volatile SRDATA_CHECK_TYPE response;
    volatile uint8 mask;
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();          // Defined in SRMicro module, it declares a local variable;
    response = SRDATA_OK;
    //------- ISR Protected Code --------------------------------------------------------------------------------------
    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();                              // Save context and disable ISRs.
    aux = SRDATA__COMPLEMENT(*data);
    if (aux != (*ndata) )                           // test variable against it's complement if they don't match it performs a memory test
    {
        SRException__Queue(SREXCEPTION_SRDATA_NOISE,(uint32)data,(uint32)ndata);  //lint !e923 (see PCLint note in file comment)
        response = SRDATA_CORRUPT_NOISE;
        aux = *data;                                // backup data
        for (mask=1U;mask>0U;mask <<= 1)            // test variable
        {
            *data = mask;
            if (*data != mask)                      // test bit by bit
            {
                SRException__Queue(SREXCEPTION_SRDATA_BAD_MEMORY,(uint32)data,(uint32)ndata);  //lint !e923 (see PCLint note in file comment)
                response = SRDATA_CORRUPT_BAD_MEMORY;
            }
        }
        *data = aux;                                // restore data


        aux = *ndata;                               // backup inverted variable data
        for (mask=1U;mask>0U;mask <<= 1)            // test the inverted variable
        {
            *ndata = mask;
            if (*ndata != mask)                     // test bit by bit
            {
                SRException__Queue(SREXCEPTION_SRDATA_BAD_MEMORY,(uint32)data,(uint32)ndata);  //lint !e923 (see PCLint note in file comment)
                response = SRDATA_CORRUPT_BAD_BACKUP_MEMORY;
            }
        }
        *ndata = aux;                               // restore inverted variable data
    }
    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();                           // Restore the ISR context previously saved.
    //-----------------------------------------------------------------------------------------------------------------
    return (response);
}
#endif


#if (SRDATA_ARRAY_FEATURE == ENABLED)
#ifdef __IAR_SYSTEMS_ICC__
    #pragma optimize = none
#endif
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method updates a Safety Array of Byte Variable and it's backup
 *
 * @param data pointer to the data array
 * @param ndata pointer to the backup of the data array
 * @param value pointer to the content to be stored in the data
 * @param size defines size of the arrays
 */
void SRData__UpdateArray(void * data,void * ndata,void * value,size_t size)
{
    size_t index;
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    //------- ISR Protected Code --------------------------------------------------------------------------------------
    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();                              // Save context and disable ISRs
    for (index =0; index < size; index++)
    {
        ((uint8 *)data)[index] = ((uint8 *)value)[index];   // write variable data
        ((uint8 *)ndata)[index] = SRDATA__COMPLEMENT(((uint8 *)value)[index]); // write inverted variable data
    }
    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();                           // Restore the ISR context previously saved.
    //-----------------------------------------------------------------------------------------------------------------
}

//---------------------------------------------------------------------------------------------------------------------
#ifdef __IAR_SYSTEMS_ICC__
    #pragma optimize = none
#endif
/**
 * @brief This method checks the data array and it's backup.
 *
 * @param data pointer to the data array
 * @param ndata pointer to the backup of the data array
 * @param size defines size of the arrays
 * @return SRDATA_CHECK_TYPE
 */
SRDATA_CHECK_TYPE SRData__CheckArray(void * data, void * ndata, size_t size)
{

    volatile uint8 aux;
    volatile SRDATA_CHECK_TYPE response;
    volatile uint8 mask;
    size_t index;
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    response = SRDATA_OK;

    //------- ISR Protected Code --------------------------------------------------------------------------------------
    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();                              // Save context and disable ISRs

    for (index=0; index< size; index++)
    {
        aux = SRDATA__COMPLEMENT(((uint8 *)data)[index]);
        if (aux !=  ((uint8 *)ndata)[index]  )    // Test each position of data against its inverted position
        {

            if(response == SRDATA_OK)               // Make sure we don't overwrite a BAD memory exception
            {
                SRException__Queue(SREXCEPTION_SRDATA_NOISE,(uint32)data,(uint32)ndata);  //lint !e923 (see PCLint note in file comment)
                response = SRDATA_CORRUPT_NOISE;
            }

            aux = ((uint8 *)data)[index];         // backup data
            for (mask=1U;mask>0U;mask <<= 1)
            {
                ((uint8 *)data)[index] = mask;
                if (((uint8 *)data)[index] != mask)   // test bit by bit
                {
                    SRException__Queue(SREXCEPTION_SRDATA_BAD_MEMORY,(uint32)data,(uint32)ndata);  //lint !e923 (see PCLint note in file comment)
                    response = SRDATA_CORRUPT_BAD_MEMORY;
                }
            }
            ((uint8 *)data)[index] = aux;         // restore data

            aux = ((uint8 *)ndata)[index];        // backup inverted data
            for (mask=1U;mask>0U;mask <<= 1)
            {
                ((uint8 *)ndata)[index] = mask;
                if (((uint8 *)ndata)[index] != mask)  // test bit by bit
                {
                    SRException__Queue(SREXCEPTION_SRDATA_BAD_MEMORY,(uint32)data,(uint32)ndata);  //lint !e923 (see PCLint note in file comment)
                    response = SRDATA_CORRUPT_BAD_BACKUP_MEMORY;
                }
            }
            ((uint8 *)ndata)[index] = aux;        // restore inverted data

        }
    }


    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();                           // Restore the ISR context previously saved.
    //-----------------------------------------------------------------------------------------------------------------
    return (response);

}
#endif



#if (SRDATA_ARRAY_NON_BLOCKING_FEATURE == ENABLED)
#ifdef __IAR_SYSTEMS_ICC__
    #pragma optimize = none
#endif
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method updates a Safety Array of Byte Variable and it's backup
 * * @details This method will update byte by byte a SR array but it allows interrupts in between bytes. So if there is
 * any change of a Array be used by 2 different places where one place can interrupt the other it should be used the set of
 * functions that are blocking: SRData__UpdateArray() and SRData__CheckArray();
 * @param data pointer to the data array
 * @param ndata pointer to the backup of the data array
 * @param value pointer to the content to be stored in the data
 * @param size defines size of the arrays
 */
void SRData__UpdateArrayNonBlocking(void * data,void * ndata,void * value,size_t size)
{
    size_t index;
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    for (index =0; index < size; index++)
    {
        //------- ISR Protected Code --------------------------------------------------------------------------------------
        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();                              // Save context and disable ISRs
        ((uint8 *)data)[index] = ((uint8 *)value)[index];   // write variable data
        ((uint8 *)ndata)[index] = SRDATA__COMPLEMENT(((uint8 *)value)[index]); // write inverted variable data
        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();                           // Restore the ISR context previously saved.
        //-----------------------------------------------------------------------------------------------------------------
    }
}

//---------------------------------------------------------------------------------------------------------------------
#ifdef __IAR_SYSTEMS_ICC__
    #pragma optimize = none
#endif
/**
 * @brief This method checks the data array and it's backup.
 * @details This method will check byte by byte a SR array but it allows interrupts in between bytes. So if there is
 * any change of a Array be used by 2 different places where one place can interrupt the other it should be used the set of
 * functions that are blocking: SRData__UpdateArray() and SRData__CheckArray();
 * @param data pointer to the data array
 * @param ndata pointer to the backup of the data array
 * @param size defines size of the arrays
 * @return SRDATA_CHECK_TYPE
 */
SRDATA_CHECK_TYPE SRData__CheckArrayNonBlocking(void * data, void * ndata, size_t size)
{
    SRDATA_CHECK_TYPE response;
    volatile size_t index;
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    response = SRDATA_OK;


    for (index=0; index< size; index++)
    {
        //------- ISR Protected Code --------------------------------------------------------------------------------------
        MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();                              // Save context and disable ISRs

        uint8 aux = SRDATA__COMPLEMENT(((uint8 *)data)[index]);
        if (aux !=  ((uint8 *)ndata)[index]  )    // Test each position of data against its inverted position
        {
            if(response == SRDATA_OK)               // Make sure we don't overwrite a BAD memory exception
            {
                SRException__Queue(SREXCEPTION_SRDATA_NOISE,(uint32)data,(uint32)ndata);  //lint !e923 (see PCLint note in file comment)
                response = SRDATA_CORRUPT_NOISE;
            }
            aux = ((uint8 *)data)[index];         // backup data
            for (uint8 mask=1U;mask>0U;mask <<= 1)
            {
                ((uint8 *)data)[index] = mask;
                if (((uint8 *)data)[index] != mask)   // test bit by bit
                {
                    SRException__Queue(SREXCEPTION_SRDATA_BAD_MEMORY,(uint32)data,(uint32)ndata);  //lint !e923 (see PCLint note in file comment)
                    response = SRDATA_CORRUPT_BAD_MEMORY;
                }
            }
            ((uint8 *)data)[index] = aux;         // restore data

            aux = ((uint8 *)ndata)[index];        // backup inverted data
            for (uint8 mask=1U;mask>0U;mask <<= 1)
            {
                ((uint8 *)ndata)[index] = mask;
                if (((uint8 *)ndata)[index] != mask)  // test bit by bit
                {
                    SRException__Queue(SREXCEPTION_SRDATA_BAD_MEMORY,(uint32)data,(uint32)ndata);  //lint !e923 (see PCLint note in file comment)
                    response = SRDATA_CORRUPT_BAD_BACKUP_MEMORY;
                }
            }
            ((uint8 *)ndata)[index] = aux;        // restore inverted data

        }
        MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();                           // Restore the ISR context previously saved.
        //-----------------------------------------------------------------------------------------------------------------
    }

    return (response);
}
#endif


#if (SRDATA_SHORT_FEATURE == ENABLED)
#ifdef __IAR_SYSTEMS_ICC__
    #pragma optimize = none
#endif
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method updates a Safety Short Variable and it's backup variable
 *
 * @param data pointer to the SR data
 * @param ndata pointer to the complementary SR data
 * @param value value to be updated in the SR variable
 */
void SRData__UpdateShort(uint16 PACKED * data,uint16 PACKED * ndata,uint16 value)
{
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    //------- ISR Protected Code --------------------------------------------------------------------------------------
    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();                              // Save context and disable ISRs
    *data = value;                                  // write data
    *ndata = SRDATA__COMPLEMENT(value);             // write inverted data
    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();                           // Restore the ISR context previously saved.
    //-----------------------------------------------------------------------------------------------------------------
}

#ifdef __IAR_SYSTEMS_ICC__
    #pragma optimize = none
#endif
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method checks the integrity of a Safety Short Variable and it's backup
 *
 * @param data pointer to the SR data
 * @param ndata pointer to the complementary SR data
 * @return Result of the verification
 */
SRDATA_CHECK_TYPE SRData__CheckShort(uint16 PACKED * data, uint16 PACKED * ndata)
{
    volatile uint16 aux;
    volatile SRDATA_CHECK_TYPE response;
    volatile uint16 mask;
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    response = SRDATA_OK;

    //------- ISR Protected Code --------------------------------------------------------------------------------------
    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();                              // Save context and disable ISRs
    aux = SRDATA__COMPLEMENT(*data);
    if (aux != (*ndata) )                           // test variable against the inverted variable
    {
        SRException__Queue(SREXCEPTION_SRDATA_NOISE,(uint32)data,(uint32)ndata);  //lint !e923 (see PCLint note in file comment)
        response = SRDATA_CORRUPT_NOISE;
        aux = *data;                                // backup variable data
        for (mask=1U;mask>0U;mask <<= 1)
        {
            *data = mask;
            if (*data != mask)                      // test bit by bit
            {
                SRException__Queue(SREXCEPTION_SRDATA_BAD_MEMORY,(uint32)data,(uint32)ndata);  //lint !e923 (see PCLint note in file comment)
                response = SRDATA_CORRUPT_BAD_MEMORY;
            }
        }
        *data = aux;                                // restore variable data

        aux = *ndata;                               // backup inverted variable data
        for (mask=1U;mask>0U;mask <<= 1)
        {
            *ndata = mask;
            if (*ndata != mask)                     // test bit by bit
            {
                SRException__Queue(SREXCEPTION_SRDATA_BAD_MEMORY,(uint32)data,(uint32)ndata);  //lint !e923 (see PCLint note in file comment)
                response = SRDATA_CORRUPT_BAD_BACKUP_MEMORY;
            }
        }
        *ndata = aux;                               // restore inverted variable data

    }
    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();                           // Restore the ISR context previously saved.
    //-----------------------------------------------------------------------------------------------------------------
    return (response);
}
#endif

#if (SRDATA_LONG_FEATURE == ENABLED)
#ifdef __IAR_SYSTEMS_ICC__
    #pragma optimize = none
#endif
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method updates a Safety Long Variable and it's backup variable
 *
 * @param data pointer to the SR data
 * @param ndata pointer to the complementary SR data
 * @param value value to be updated in the SR variable
 */
void SRData__UpdateLong(uint32 PACKED * data,uint32 PACKED * ndata,uint32 value)
{
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    //------- ISR Protected Code --------------------------------------------------------------------------------------
    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();                              // Save context and disable ISRs
    *data = value;
    *ndata = SRDATA__COMPLEMENT(value);
    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();                           // Restore the ISR context previously saved.
    //-----------------------------------------------------------------------------------------------------------------
}

#ifdef __IAR_SYSTEMS_ICC__
    #pragma optimize = none
#endif
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method checks the integrity of a Safety Long Variable and it's backup
 *
 * @param data pointer to the SR data
 * @param ndata pointer to the complementary SR data
 * @return Result of the verification
 */
SRDATA_CHECK_TYPE SRData__CheckLong(uint32 PACKED * data,uint32 PACKED * ndata)
{
    uint32 aux;
    SRDATA_CHECK_TYPE response;
    uint32 mask;
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    response = SRDATA_OK;

    //------- ISR Protected Code --------------------------------------------------------------------------------------
    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();                              // Save context and disable ISRs
    aux = SRDATA__COMPLEMENT(*data);
    if (aux != (*ndata) )
    {
        SRException__Queue(SREXCEPTION_SRDATA_NOISE,(uint32)data,(uint32)ndata);  //lint !e923 (see PCLint note in file comment)
        response = SRDATA_CORRUPT_NOISE;

        aux = *data;                                // backup variable data
        for (mask=1U;mask>0U;mask <<= 1)
        {
            *data = mask;
            if (*data != mask)                      // test bit by bit
            {
                SRException__Queue(SREXCEPTION_SRDATA_BAD_MEMORY,(uint32)data,(uint32)ndata);  //lint !e923 (see PCLint note in file comment)
                response = SRDATA_CORRUPT_BAD_MEMORY;
            }
        }
        *data = aux;                                // restore variable data


        aux = *ndata;                               // backup inverted variable data
        for (mask=1U;mask>0U;mask <<= 1)
        {
            *ndata = mask;
            if (*ndata != mask)                     // test bit by bit
            {
                SRException__Queue(SREXCEPTION_SRDATA_BAD_MEMORY,(uint32)data,(uint32)ndata);  //lint !e923 (see PCLint note in file comment)
                response = SRDATA_CORRUPT_BAD_BACKUP_MEMORY;
            }
        }
        *ndata = aux;                               // restore inverted variable data
    }
    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();                           // Restore the ISR context previously saved.
    //-----------------------------------------------------------------------------------------------------------------
    return (response);
}
#endif


#if (SRDATA_FLOAT_FEATURE == ENABLED)
#ifdef __IAR_SYSTEMS_ICC__
    #pragma optimize = none
#endif
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method updates a Safety Long Variable and it's backup variable
 *
 * @param data pointer to the SR data
 * @param ndata pointer to the complementary SR data
 * @param value value to be updated in the SR variable
 */
void SRData__UpdateFloat(float32 PACKED * data,float32 PACKED * ndata,float32 value)
{
    SRDATA_CONVERTION_F_L_TYPE aux;
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    aux.vfloat = value;                             // load the float size of the union
    aux.vlong = SRDATA__COMPLEMENT(aux.vlong);      // use the data as long

    //------- ISR Protected Code --------------------------------------------------------------------------------------
    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();                              // Save context and disable ISRs
    *data = value;                                  // update variable
    *ndata = aux.vfloat;                            // take the inverted data as a float and load into the inverted variable
    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();                           // Restore the ISR context previously saved.
    //-----------------------------------------------------------------------------------------------------------------
}

#ifdef __IAR_SYSTEMS_ICC__
    #pragma optimize = none
#endif
//---------------------------------------------------------------------------------------------------------------------
/**
 * @brief This method checks the integrity of a Safety Long Variable and it's backup
 *
 * @param data pointer to the SR data
 * @param ndata pointer to the complementary SR data
 * @return Result of the verification
 */
SRDATA_CHECK_TYPE SRData__CheckFloat(float32 PACKED * data, float32 PACKED * ndata)
{
    SRDATA_CONVERTION_F_L_TYPE aux;
    SRDATA_CHECK_TYPE response;
    uint32 mask;
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();

    response = SRDATA_OK;

    //------- ISR Protected Code --------------------------------------------------------------------------------------
    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();                              // Save context and disable ISRs
    aux.vfloat = *data;                             // load the variable as float
    aux.vlong = SRDATA__COMPLEMENT(aux.vlong);      // calculates it's complement

    if (aux.vlong != (*((uint32 *)(void*)ndata)))   // test the inverted variable with the data  complement
    {
        SRException__Queue(SREXCEPTION_SRDATA_NOISE,(uint32)data,(uint32)ndata);  //lint !e923 (see PCLint note in file comment)
        response = SRDATA_CORRUPT_NOISE;

        aux.vfloat = *data;                         // backup variable data
        for (mask=1U;mask>0U;mask <<= 1)
        {
            *(uint32 *)(void*)data = mask;
            if (*(uint32 *)(void*)data != mask) // test bit by bit of the variable
            {
                SRException__Queue(SREXCEPTION_SRDATA_BAD_MEMORY,(uint32)data,(uint32)ndata);  //lint !e923 (see PCLint note in file comment)
                response = SRDATA_CORRUPT_BAD_MEMORY;
            }
        }
        *data = aux.vfloat;                         // restore inverted variable data


        aux.vfloat = *ndata;                        // save inverted variable data
        for (mask=1U;mask>0U;mask <<= 1)
        {
            *(uint32 *)(void*)ndata = mask;
            if (*(uint32 *)(void*)ndata != mask)// test bit by bit
            {
                SRException__Queue(SREXCEPTION_SRDATA_BAD_MEMORY,(uint32)data,(uint32)ndata);  //lint !e923 (see PCLint note in file comment)
                response = SRDATA_CORRUPT_BAD_BACKUP_MEMORY;
            }
        }
        *ndata = aux.vfloat;                        // restore inverted variable data
    }
    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();                           // Restore the ISR context previously saved.
    //-----------------------------------------------------------------------------------------------------------------

    return (response);
}
#endif


#ifdef __IAR_SYSTEMS_ICC__
    #pragma optimize = none
#endif
//---------------------------------------------------------------------------------------------------------------------
/**
 * Fill a memory block with a fill value.
 * @param data = Pointer to the block with non-inverted data.
 * @param ndata = Pointer to the block with inverted data.
 * @param fill_value = The non-inverted data value to fill.
 * @param size = The number of bytes to fill in the memory block.
 */
void SRData__Fill(void* data, void* ndata, uint8 fill_value, size_t size)
{
    MICRO_DECLARE_INTERRUPT_CONTEXT_LOCAL();
    uint8* ptr = data;
    uint8* nptr = ndata;
    MICRO_SAVE_INTERRUPT_CONTEXT_LOCAL();           // Save context and disable ISRs
    for (size_t i = 0; i < size; i++)
    {
        *ptr = fill_value;
        ptr++;
        *nptr = SRDATA__COMPLEMENT(fill_value);
        nptr++;
    }
    MICRO_RESTORE_INTERRUPT_CONTEXT_LOCAL();        // Restore the ISR context previously saved.
}


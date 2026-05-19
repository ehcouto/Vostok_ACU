/**
 *  @file
 *  @defgroup CLASS_B
 *  @brief      This is the public API for the SRData module which implements the Update and Check for
 *              SR (Safety Relevant) Variables
 *
 *  @copyright  Copyright 2007-2023 Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRDATA_H_
    #define SRDATA_H_

#include "C_Extensions.h"
#include "SRData_prm.h"
#include "stddef.h"


#ifndef SRDATA_BYTE_FEATURE
    #define SRDATA_BYTE_FEATURE     ENABLED
#endif

#ifndef SRDATA_SHORT_FEATURE
    #define SRDATA_SHORT_FEATURE    ENABLED
#endif

#ifndef SRDATA_LONG_FEATURE
    #define SRDATA_LONG_FEATURE     ENABLED
#endif

#ifndef SRDATA_ARRAY_FEATURE
    #define SRDATA_ARRAY_FEATURE     ENABLED
#endif

#ifndef SRDATA_FLOAT_FEATURE
    #define SRDATA_FLOAT_FEATURE     DISABLED
#endif


#ifndef SRDATA_ARRAY_NON_BLOCKING_FEATURE
    #define SRDATA_ARRAY_NON_BLOCKING_FEATURE   DISABLED
#endif


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

typedef enum
{
    SRDATA_OK,
    SRDATA_CORRUPT_NOISE,
    SRDATA_CORRUPT_BAD_MEMORY,
    SRDATA_CORRUPT_BAD_BACKUP_MEMORY
}SRDATA_CHECK_TYPE;


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#if (SRDATA_BYTE_FEATURE == ENABLED)
    void SRData__UpdateByte(uint8* data, uint8* ndata, uint8 value);
    /**
     * @brief This method copies a byte to a Safety Byte Variable and its backup.
     *
     * @param var = The Safety Byte Variable name. The complementary SR data must be named using the
     *          'N_' prefix followed by the normal SR data variable name.
     * @param dat = A byte value to copy into the Safety Byte Variable.
     */
    #define SR_SET_BYTE(var, dat)   SRData__UpdateByte(&var, &N_##var, (dat))
    SRDATA_CHECK_TYPE SRData__CheckByte(uint8* data, uint8* ndata);
    /**
     * @brief This method checks the integrity of a Safety Byte Variable and its backup.
     *
     * @param var = The Safety Byte Variable name. The complementary SR data must be named using the
     *          'N_' prefix followed by the normal SR data variable name.
     *
     * @return TRUE if corruption is detected.
     */
    #define SR_CHECK_BYTE(var)      SRData__CheckByte(&var, &N_##var)
#endif

#if (SRDATA_SHORT_FEATURE == ENABLED)
    void SRData__UpdateShort(uint16 PACKED* data, uint16 PACKED* ndata, uint16 value);
    /**
     * @brief This method copies a short to a Safety Short Variable and its backup.
     *
     * @param var = The Safety Short Variable name. The complementary SR data must be named using the
     *          'N_' prefix followed by the normal SR data variable name.
     * @param dat = A short value to copy into the Safety Short Variable.
     */
    #define SR_SET_SHORT(var, dat)  SRData__UpdateShort(&var, &N_##var, (dat))
    SRDATA_CHECK_TYPE SRData__CheckShort(uint16 PACKED* data, uint16 PACKED* ndata);
    /**
     * @brief This method checks the integrity of a Safety Short Variable and its backup.
     *
     * @param var = The Safety Short Variable name. The complementary SR data must be named using the
     *          'N_' prefix followed by the normal SR data variable name.
     *
     * @return TRUE if corruption is detected.
     */
    #define SR_CHECK_SHORT(var)     SRData__CheckShort(&var, &N_##var)
#endif

#if (SRDATA_FLOAT_FEATURE == ENABLED)
    void SRData__UpdateFloat(float32 PACKED* data, float32 PACKED* ndata,float32 value);
    /**
     * @brief This method copies a float to a Safety Float Variable and its backup.
     *
     * @param var = The Safety Float Variable name. The complementary SR data must be named using the
     *          'N_' prefix followed by the normal SR data variable name.
     * @param dat = A float value to copy into the Safety Float Variable.
     */
    #define SR_SET_FLOAT(var, dat)  SRData__UpdateFloat(&var, &N_##var, (dat))
    SRDATA_CHECK_TYPE SRData__CheckFloat(float32 PACKED* data, float32 PACKED* ndata);
    /**
     * @brief This method checks the integrity of a Safety Float Variable and its backup.
     *
     * @param var = The Safety Float Variable name. The complementary SR data must be named using the
     *          'N_' prefix followed by the normal SR data variable name.
     *
     * @return TRUE if corruption is detected.
     */
    #define SR_CHECK_FLOAT(var)     SRData__CheckFloat(&var, &N_##var)
#endif

#if (SRDATA_LONG_FEATURE == ENABLED)
    void SRData__UpdateLong(uint32 PACKED* data, uint32 PACKED* ndata,uint32 value);
    /**
     * @brief This method copies a long to a Safety Long Variable and its backup.
     *
     * @param var = The Safety Long Variable name. The complementary SR data must be named using the
     *          'N_' prefix followed by the normal SR data variable name.
     * @param dat = A long value to copy into the Safety Long Variable.
     */
    #define SR_SET_LONG(var, dat)   SRData__UpdateLong(&var, &N_##var, (dat))
    SRDATA_CHECK_TYPE SRData__CheckLong(uint32 PACKED* data, uint32 PACKED* ndata);
    /**
     * @brief This method checks the integrity of a Safety Long Variable and its backup.
     *
     * @param var = The Safety Long Variable name. The complementary SR data must be named using the
     *          'N_' prefix followed by the normal SR data variable name.
     *
     * @return TRUE if corruption is detected.
     */
    #define SR_CHECK_LONG(var)      SRData__CheckLong(&var, &N_##var)
#endif

#if (SRDATA_ARRAY_FEATURE == ENABLED)
    void SRData__UpdateArray(void* data, void* ndata, void* value, size_t size);
    /**
     * @brief This method copies a data array to a Safety Data Array Variable and its backup.
     * @details The array must be defined such that 'sizeof(var)' is the correct size of the array.
     *
     * @param var = The Safety Data Array Variable name. The complementary SR data must be named using
     *          the 'N_' prefix followed by the normal SR data variable name.
     * @param dat = An array of data to copy into the Safety Data Array Variable.
     */
    #define SR_SET_ARRAY(var, dat)  SRData__UpdateArray(&var, &N_##var, &(dat), sizeof(var))
    SRDATA_CHECK_TYPE SRData__CheckArray(void* data, void* ndata, size_t size);
    /**
     * @brief This method checks a Safety Data Array Variable and its backup.
     * @details The array must be defined such that 'sizeof(var)' is the correct size of the array.
     *
     * @param var = The Safety Data Array Variable name. The complementary SR data must be named using
     *          the 'N_' prefix followed by the normal SR data variable name.
     *
     * @return TRUE if corruption is detected.
     */
    #define SR_CHECK_ARRAY(var)     SRData__CheckArray(&var, &N_##var, sizeof(var))
#endif

#if (SRDATA_ARRAY_NON_BLOCKING_FEATURE == ENABLED)
    void SRData__UpdateArrayNonBlocking(void* data, void* ndata, void* value, size_t size);
    SRDATA_CHECK_TYPE SRData__CheckArrayNonBlocking(void* data, void* ndata, size_t size);
#endif

void SRData__Fill(void* data, void* ndata, uint8 fill_value, size_t size);
/**
 * @brief This method fills a Safety Data Block Variable and its backup with the specified fill value.
 * @param var = The Safety Data Block Variable name. The complementary SR data must be named using
 *          the 'N_' prefix followed by the normal SR data variable name.
 * @param dat = A byte value to fill into the Safety Data Block Variable.
 * @param size = The number of bytes to write to the Safety Data Block Variable and its backup.
 */
#define SR_FILL(var, dat, size)     SRData__Fill(&var, &N_##var, (dat), (size))


#endif // SRDATA_H_


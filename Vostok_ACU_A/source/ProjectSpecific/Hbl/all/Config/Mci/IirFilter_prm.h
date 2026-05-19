/**
 *  @file
 *  @brief      Public parameters for the IIR Filter module.
 *
 * $Revision: 1.2 $
 *
 * Copyright 2010-2011. Whirlpool Corporation.  All rights reserved - CONFIDENTIAL
 */
#ifndef IIRFILTER_PRM_H_
#define IIRFILTER_PRM_H_

//  --- Include Files -------------------------------------------------------------------------------------------------

// -- This Module --

// -- ANSI/ISO --

// -- Project --


//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

/**
 * @brief       Module-level compiler switch to determine whether the functions so decorated are located in NEAR or FAR
 *              memory.
 * @details     The possible values for all xxx_MM_CODEn parameters are:\n
 *              @arg @c #NEAR - uses the Vortex definition of #NEAR (in the Cosmic world, "@near").
 *              @arg @c #FAR  - uses the Vortex definition of #FAR  (in the Cosmic world, "@far").
 *              @arg <i>nothing</i> - uses the default memory model.
 *
 * In this project, we are assuming that the default memory model is to place all functions and data in #NEAR space, and
 * we will specifically override the functions we want placed in #FAR memory.
 */
#define ADDR_SIZE_IIRFILTER         /* Nothing */


//=====================================================================================================================
//-------------------------------------- PUBLIC (Function Prototypes) -------------------------------------------------
//=====================================================================================================================

#endif // IIRFILTER_PRM_H_

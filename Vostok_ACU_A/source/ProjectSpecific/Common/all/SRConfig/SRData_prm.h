/**
 *  @file		SRData_prm.h
 *
 *  @brief      Public (prm) configuration file of SRData module.
 *
 *  @defgroup	CLASS_B
 *
 *  @details    The configuration of the SRData consists enabled the features and address the exceptions that can be
 *              generated due to the mismatches in between the data and it's complement.
 *
 *  $Header: SRData_prv.h 1.4 2012/06/01 14:53:23EDT MANOEL GARBUIO DE SOUZA (SOUZAMG) Exp  $
 *
 *  @copyright  Copyright 2007-$Date: 2012/06/01 14:53:23EDT $. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRDATA_PRV_H_
	#define SRDATA_PRV_H_
	
#include "C_Extensions.h"

/**
 * @brief SRDATA_BYTE_FEATURE defines the availability of the set of functions to handle BYTEs
 * @details The acceptable values are ENABLED  or DISABLED
 */
//=====================================================================================================================
#define SRDATA_BYTE_FEATURE     ENABLED

//=====================================================================================================================
/**
 * @brief SRDATA_SHORT_FEATURE defines the availability of the set of functions to handle BYTEs
 * @details The acceptable values are ENABLED  or DISABLED
 */
//=====================================================================================================================
#define SRDATA_SHORT_FEATURE    ENABLED

//=====================================================================================================================
/**
 * @brief SRDATA_LONG_FEATURE defines the availability of the set of functions to handle BYTEs
 * @details The acceptable values are ENABLED  or DISABLED
 */
//=====================================================================================================================
#define SRDATA_LONG_FEATURE     ENABLED

//=====================================================================================================================
/**
 * @brief SRDATA_FLOAT_FEATURE defines the availability of the set of functions to handle BYTEs
 * @details The acceptable values are ENABLED  or DISABLED
 */
//=====================================================================================================================
#define SRDATA_FLOAT_FEATURE     ENABLED

//=====================================================================================================================
/**
 * @brief SRDATA_ARRAY_FEATURE defines the availability of the set of functions to handle BYTEs
 * @details The acceptable values are ENABLED  or DISABLED
 */
//=====================================================================================================================
#define SRDATA_ARRAY_FEATURE    ENABLED


#define SRDATA_ARRAY_NON_BLOCKING_FEATURE   ENABLED

#endif // SRDATA_PRV_H_



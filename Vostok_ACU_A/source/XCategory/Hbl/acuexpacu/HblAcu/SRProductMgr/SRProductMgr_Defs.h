/**
 *  @file       SRProductMgr_Defs.h
 *
 *  @brief      Common definitions and function prototypes for SRChecks and SRActions
 *
 *  @section    Applicable_Documents
 *					List here all the applicable documents if needed. <tr>
 *
 *	@defgroup	CLASS_B
 *
 *  $Header: $
 *
 *  @copyright  Copyright 2015-$Date$. Whirlpool Corporation. All rights reserved - CONFIDENTIAL
 */
//---------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#ifndef SRPRODUCTMGR_DEFS_H_
#define SRPRODUCTMGR_DEFS_H_

//=====================================================================================================================
//-------------------------------------- PUBLIC (Extern Variables, Constants & Defines) -------------------------------
//=====================================================================================================================

typedef enum _SRPRODUCTMGR_ACTION_ENUM
{
	SRPRODUCTMGR_ACTION_IDLE 				= 0,
	SRPRODUCTMGR_ACTION_INITIALIZE			= 1,
	SRPRODUCTMGR_ACTION_RUNNING 			= 2,
	SRPRODUCTMGR_ACTION_RUNNING_EXCL		= 3,
	SRPRODUCTMGR_ACTION_STOPPING 			= 4,
	SRPRODUCTMGR_ACTION_STOPPING_EXCL		= 5,
	SRPRODUCTMGR_ACTION_COMPLETED 			= 6,
	SRPRODUCTMGR_ACTION_INVALID				= 255
} SRPRODUCTMGR_ACTION_TYPE;

typedef enum _SRPRODUCTMGR_STATUS_ENUM
{
	SRPRODUCTMGR_STATUS_IDLE		= 0,
	SRPRODUCTMGR_STATUS_RESET		= 1,
	SRPRODUCTMGR_STATUS_MONITORING	= 2,
	SRPRODUCTMGR_STATUS_WARNING		= 3,
	SRPRODUCTMGR_STATUS_PROTECTION	= 4,
	SRPRODUCTMGR_STATUS_PREFAULT	= 5,
	SRPRODUCTMGR_STATUS_FAULT		= 6,
	SRPRODUCTMGR_STATUS_INVALID		= 255
} SRPRODUCTMGR_STATUS_TYPE;

typedef enum _SRPRODUCTMGR_CHECK_CMD_ENUM
{
	SRPROUCTMGR_CHECK_CMD_NO_ACTION			= 0,
	SRPROUCTMGR_CHECK_CMD_RESET_FAULT		= 1,
	SRPROUCTMGR_CHECK_CMD_TEST_TIMER		= 2,
	SRPROUCTMGR_CHECK_CMD_NORMAL_TIMER		= 3,
	SRPROUCTMGR_CHECK_CMD_END_CYCLE			= 4
} SRPRODUCTMGR_CHECK_CMD_TYPE;

typedef struct _SRPRODUCTMGR_CHECK_STATUS_STRUCT
{
	SRPRODUCTMGR_STATUS_TYPE	Status;
	uint8						Fault_ID;
} SRPRODUCTMGR_CHECK_STATUS_TYPE;

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//          SRCheck prototype templates
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#define 		DUMMY_SRCHECK_REGISTER		NULL, NULL, NULL, NULL, NULL

typedef void (* const SRCHECK__INITIALIZE)(void);
typedef void (* const SRCHECK__HANDLER)(void);
typedef void (* const SRCHECK__ISSUECMD)(SRPRODUCTMGR_CHECK_CMD_TYPE cmd);
typedef void (* const SRCHECK__GETSTATUS)(SRPRODUCTMGR_CHECK_STATUS_TYPE *status);
typedef uint16 (* const SRCHECK__GETTIMER)(void);

/**
 * SRCheck Hook Struct
 */
typedef struct
{
	SRCHECK__INITIALIZE Initialize;		//!< SRCheck Initialize method
	SRCHECK__HANDLER Handler;			//!< Handler method
	SRCHECK__ISSUECMD IssueCmd;			//!< Gets a command request to the SRCheck
	SRCHECK__GETSTATUS GetStatus;		//!< Provides current SRCheck status
	SRCHECK__GETTIMER GetTimer;			//!< Provide a timer reference related to current SRCheck status
} SRCHECK_HOOK_TYPE;


//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//          SRAction prototype templates
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//---------------------------------------------------------------------------------------------------------------------
#define 		DUMMY_SRACTION_REGISTER		NULL, NULL, NULL, NULL, NULL

typedef void (* const SRACTION__INITIALIZE)(void);
typedef SRPRODUCTMGR_ACTION_TYPE (* const SRACTION__HANDLER)(uint8 priority);
typedef SRPRODUCTMGR_ACTION_TYPE (* const SRACTION__GETSTATUS)(void);
typedef PASS_FAIL_TYPE (* const SRACTION__START)(void);
typedef PASS_FAIL_TYPE (* const SRACTION__STOP)(void);

/**
 * SRAction Hook Struct
 */
typedef struct
{
	SRACTION__INITIALIZE Initialize;	//!< SRAction Initialize method
	SRACTION__HANDLER Handler;			//!< Handler method
	SRACTION__GETSTATUS GetStatus;		//!< Provides current SRAction status
	SRACTION__START Start;				//!< Issue Start command on SRAction to run it (or keep it run)
	SRACTION__STOP Stop;				//!< Issue Stop command on SRAction
} SRACTION_HOOK_TYPE;


#endif // SRPRODUCTMGR_DEFS_H_

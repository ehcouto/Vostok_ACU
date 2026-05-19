/**
 *  @file
 *  @brief      Private parameter file for time capture driver module
 *
 *  $Revision: 1.7 $
 *
 *  Copyright 2010 Whirlpool Corporation. All rights reserved - CONFIDENTIAL.
 */

#ifndef TIME_CAPTURE_PRV_H
#define TIME_CAPTURE_PRV_H

#include "Mci.h"
#define INTERRUPT_CALLBACK_C01()                    Mci__TriacFireHandler()

#define IC00_ENABLED ENABLED                        //!< Define if channel is enabled and compile
#endif // TIME_CAPTURE_PRV_H

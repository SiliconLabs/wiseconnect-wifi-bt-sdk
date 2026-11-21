/*******************************************************************************
* @file  rsi_common_types.h
* @brief 
*******************************************************************************
* # License
* <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
*******************************************************************************
*
* The licensor of this software is Silicon Laboratories Inc. Your use of this
* software is governed by the terms of Silicon Labs Master Software License
* Agreement (MSLA) available at
* www.silabs.com/about-us/legal/master-software-license-agreement. This
* software is distributed to you in Source Code format and is governed by the
* sections of the MSLA applicable to Source Code.
*
******************************************************************************/
/**
 * @file         rsi_common_types.h
 * @version      1.0
 * @date         2015-Feb-17
 *
 *
 *
 * @brief HEADER, APP, APPLICATION Header file which contains application specific structures 
 *
 * @section Description
 * This file contains the Application related information.
 *
 *
 */

/**
 * Includes
 * */

#ifndef __RSI_COMMON_TYPES_H
#define __RSI_COMMON_TYPES_H
typedef unsigned char	          UINT08;
typedef signed char             INT08;

#ifndef UINT8
typedef unsigned char           UINT8;
#endif
#ifndef INT8
typedef signed char             INT8;
#endif
#ifndef UINT16
typedef unsigned short int      UINT16;
#endif
#ifndef INT16
typedef short                   INT16;
#endif
#ifndef UINT32
typedef unsigned int       UINT32;
#endif

#ifndef INT32
typedef int       			INT32;
#endif

typedef long                    SINT32;
typedef long                    SINT_32;
typedef unsigned long long int  UINT64;
typedef long long  int          INT64;

#ifndef WINDOWS
typedef unsigned char           BOOL;
#endif

typedef unsigned char           uint8;
typedef unsigned short          uint16;
typedef unsigned int            uint32;
typedef signed char             int8;
typedef short                   int16;
typedef long                    int32;

//ZIgb Datatypes
typedef int sint32, sint_32;
typedef unsigned long long int uint64;
typedef long long  int int64;
typedef unsigned char RSI_ZB_STATUS;
#if (defined WINDOWS || defined LINUX_PLATFORM)
typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned int uint32_t;
typedef int int32_t;
#endif
#ifdef LINUX_PLATFORM
typedef uint16_t profile_id_t;
typedef uint16_t cluster_id_t;
typedef uint16_t ProfileID;
typedef uint16_t ClusterID;
typedef uint16_t GroupID;
#endif

#endif

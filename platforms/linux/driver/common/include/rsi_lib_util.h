/*******************************************************************************
* @file  rsi_lib_util.h
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
 * @file     rsi_lib_util.h
 * @version  2.7
 * @date     2012-Sep-26
 *
 *
 *
 * @brief HEADER UTIL: Util Header file, the things that are used in the library 
 *
 * @section Description
 * This is the util.h file for the utility functions used by library.
 * Contains prototypes of utils used in rsi_lib_util.c
 *
 */


#ifndef _RSILIBUTIL_H_
#define _RSILIBUTIL_H_

#include "rsi_global.h"

void rsi_uint32_to_4bytes(uint8 *dBuf, uint32 val);
void rsi_uint16_to_2bytes(uint8 *dBuf, uint16 val);
uint16 rsi_bytes2R_to_uint16(uint8 *dBuf);
#endif

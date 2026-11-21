/*HEADER**********************************************************************
Copyright (c)
All rights reserved
This software embodies materials and concepts that are confidential to Silicon Labs
and is made available solely pursuant to the terms of a written license
agreement with Silicon Labs

Project name : WiSeConnect
Module name  : LINUX-SPI-driver
File Name    : common.h

File Description:
        Some common defines will be here.
List of functions:

Author :

Rev History:
Sl By date change details
---------------------------------------------------------
01.                   created
---------------------------------------------------------
Anji
*END*************************************************************************/

#ifndef __COMMON_H__
#define __COMMON_H__

#ifndef UINT32
#define UINT32 unsigned int
#endif
#ifndef UINT16
#define UINT16 unsigned short
#endif
#ifndef UINT8
#define UINT8 unsigned char
#endif
#ifndef PVOID
#define PVOID void*
#endif

#ifndef INT32
#define INT32 int
#endif
#ifndef INT16
#define INT16 short
#endif
#ifndef INT8
#define INT8 char
#endif
#ifndef VOID
#define VOID void
#endif

extern UINT32 rsi_zone_enabled;

#define RSI_STATIC   static
#define RSI_EXTERN   extern

#define RSI_STATUS_FAIL     -1
#define RSI_STATUS_SUCCESS   0
#define RSI_STATUS INT32

#define RSI_MAC_ADDR_LEN     6

#endif

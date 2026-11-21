/*******************************************************************************
* @file  rsi_config.h
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
 * @file           rsi_config.h
 * @version        1.1
 * @date           2019-Mar-07
 *
 *
 *
 * @brief CONFIG INIT contains the default configurations used in the api's
 *
 * @section Description
 * USER is supposed to configure the module/API's  by using the following defines 
 *
 *
 */

#ifndef _INITSTRUCT_H_
#define _INITSTRUCT_H_


#include "rsi_api.h"
/*==============================================*/
/**
 * Global Defines
 */


#define RSI_INTERFACE RSI_USB            //@ RSI_SPI or RSI_UART or RSI_USB or RSI_SDIO host interface for communication with module

#if (RSI_INTERFACE == RSI_SPI)
#ifndef RSI_SPI_INTERFACE
#define RSI_SPI_INTERFACE
#endif
#undef  RSI_UART_INTERFACE
#undef  RSI_USB_INTERFACE
#undef  RSI_SDIO_INTERFACE
#elif (RSI_INTERFACE == RSI_UART)
#ifndef RSI_UART_INTERFACE
#define RSI_UART_INTERFACE
#endif
#undef  RSI_SPI_INTERFACE
#undef  RSI_USB_INTERFACE
#undef  RSI_SDIO_INTERFACE
#elif (RSI_INTERFACE == RSI_USB)
#ifndef RSI_USB_INTERFACE
#define RSI_USB_INTERFACE
#endif
#undef  RSI_SPI_INTERFACE
#undef  RSI_UART_INTERFACE
#undef  RSI_SDIO_INTERFACE
#elif (RSI_INTERFACE == RSI_SDIO)
#ifndef RSI_SDIO_INTERFACE
#define RSI_SDIO_INTERFACE
#endif
#undef  RSI_SPI_INTERFACE
#undef  RSI_UART_INTERFACE
#undef  RSI_USB_INTERFACE
#endif

#ifdef RSI_UART_INTERFACE
#ifndef RSI_UART_DEVICE
#ifdef LINUX_PLATFORM
#define RSI_UART_DEVICE                    "/dev/ttyUSB0"
#elif WINDOWS 
#define RSI_UART_DEVICE                    "\\\\.\\COM98"
#define BYPASS_CARD_READY 							0 //@ 0 - for Card Ready Bypass and 1 - Wait for Card Ready 
#endif

#endif
#define RSI_USE_HOST_WAKEUP_AS_INTERRUPT    ENABLE
#endif
#define RSI_SECURE_BOOT            DISABLE
#define HOST_INTERACTION_MODE      ENABLE                      //@ ENABLE or DISABLE host interaction for bootloader
#define RSI_TCP_IP_BYPASS          DISABLE                     //@ ENABLE or DISABLE TCP/IP bypass mode
#if RSI_TCP_IP_BYPASS
#define RSI_TCP_IP_FEATURE_BIT_MAP  TCP_IP_FEAT_BYPASS
#else
#define RSI_TCP_IP_FEATURE_BIT_MAP (TCP_IP_FEAT_DHCPV4_CLIENT | TCP_IP_FEAT_HTTP_CLIENT)
#endif
#endif

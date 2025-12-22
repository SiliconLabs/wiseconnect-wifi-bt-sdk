/*******************************************************************************
* @file  rsi_usb_intf.h
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
 * @file     rsi_usb_intf.h
 * @version  0.1
 * @date     12 August 2025
 *
 *
 *
 * @brief USB interface HEADER: Function prototypes and defines for USB
 *
 * @section Description
 * This file contains the function definition prototypes, defines used in USB
 *
 *
 */

#ifndef RSI_USB_INTF_H
#define RSI_USB_INTF_H

#include "rsi_driver.h"

#if defined(LINUX_PLATFORM) && defined(RSI_USB_INTERFACE)
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
extern int32_t rsi_hal_platform_init(void);
extern int32_t rsi_hal_platform_deinit(void);
extern int16_t rsi_hal_frame_write(rsi_uFrameDsc *uFrameDscFrame, uint8_t *payloadparam, uint16_t size_param);
extern int16_t rsi_hal_frame_read(uint8_t *pkt_buffer);
extern int16_t rsi_hal_mem_wr(uint32_t addr, uint16_t len, uint8_t *dBuf);
extern int16_t rsi_hal_mem_rd(uint32_t addr, uint16_t len, uint8_t *dBuf);
int16_t rsi_frame_write(rsi_frame_desc_t *uFrameDscFrame, uint8_t *payloadparam, uint16_t size_param);
int16_t rsi_frame_read(uint8_t *pkt_buffer);
int16_t rsi_mem_wr(uint32_t addr, uint16_t len, uint8_t *dBuf);
int16_t rsi_mem_rd(uint32_t addr, uint16_t len, uint8_t *dBuf);
#endif

#endif //RSI_USB_INTF_H

/*******************************************************************************
* @file  rsi_usb_mem_rd_wr.c
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
 * @file       rsi_usb_mem_rd_wr.c
 * @version    0.1
 * @date       15 Aug 2015
 *
 *
 *
 * @brief USB MEMRDWR: USB Memory Read/Write functions, r/w memory on the module via the USB interface
 * Description
 * Contains the USB Memory Read/Write functions to the module
 *
 *
 */

/**
 * Includes
 */
#if (defined LINUX_PLATFORM) && (defined RSI_USB_INTERFACE)
#include "rsi_usb_intf.h"
#include "rsi_bootup_config.h"

#define RSI_USB_DEV_STATUS_ADDR RSI_HOST_INTF_STATUS_REG
#define SIZE                    1
/*
  Global Variables
 */
/** @addtogroup DRIVER4
* @{
*/
/*====================================================*/
/**
 * @fn          int16_t rsi_mem_wr(uint32_t addr, uint16_t len, uint8_t *dBuf)
 * @brief       This API writes contents of dBuf buffer to the device memory address addr for the length len
 * @param[in]   addr            -  Device memory address. 
 * @param[in]   len             -  Length of the buffer.
 * @param[in]   dBuf            -  Data present in this buffer
 * @return       0              -  Success \n
 * @return       Non-Zero value -  Failure \n
 *
 */

int16_t rsi_mem_wr(uint32_t addr, uint16_t len, uint8_t *dBuf)
{
  int16_t retval = RSI_SUCCESS;

  retval = rsi_hal_mem_wr(addr, len, dBuf);
  if (retval != RSI_SUCCESS) {
    return retval;
  }

  return retval;
}

/*====================================================*/
/**
 * @fn          int16_t rsi_mem_rd(uint32_t addr, uint16_t len, uint8_t *dBuf)
 * @brief       This API reads the device memory contents present at the address addr into the dBuf buffer for the length len
 * @param[in]   addr             -  Device memory address. 
 * @param[in]   len              -  Length of the buffer.
 * @param[in]   dBuf             -  Destination buffer.
 * @return       0               -  Success \n
 * @return       Non-Zero value  -  Failure \n
 *
 */

int16_t rsi_mem_rd(uint32_t addr, uint16_t len, uint8_t *dBuf)
{
  int16_t retval = RSI_SUCCESS;

  retval = rsi_hal_mem_rd(addr, len, dBuf);
  if (retval != RSI_SUCCESS) {
    return retval;
  }

  return retval;
}

/*==================================================*/
/**
 * @fn          int16_t rsi_device_interrupt_status(uint8_t *int_status)
 * @brief       Read the USB device status
 * @param[in]   int_status - Pointer to the buffer of data to be read, assumed
 * to be at least a byte
 * @return        0              - Success \n
 * @return        Non-Zero value - Failure \n
 */

int16_t rsi_device_interrupt_status(uint8_t *int_status)
{
  int16_t retval = RSI_SUCCESS;

  // Read the interrupt register
  retval = rsi_mem_rd(RSI_HOST_INTF_STATUS_REG, SIZE, int_status);

  return retval;
}

#endif
/** @} */

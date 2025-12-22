/*******************************************************************************
* @file  rsi_usb_frame_rd_wr.c
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
 * @file     rsi_usb_frame_rd_wr.c
 * @version  0.1
 * @date     15 Aug 2015
 *
 *
 *
 * @brief USB Control: Functions to control USB frame read/write functions
 *
 * Description
 * USB function to read/write management descriptors frames to/from the wlan module
 *
 * Improvements
 * Function header is modified for frameDscRd() function
 */

/**
 * Includes
 * */
#if (defined LINUX_PLATFORM) && (defined RSI_USB_INTERFACE)
#include "rsi_usb_intf.h"
/*
  Global Variables
 */
/** @addtogroup DRIVER4
* @{
*/

/*====================================================*/
/**
 * @fn          int16 rsi_frame_write(rsi_frame_desc_t *uFrameDscFrame, uint8_t *payloadparam, uint16_t size_param)
 * @brief       Common function for all the commands.
 * @param[in]   rsi_frame_desc_t uFrameDscFrame, frame descriptor
 * @param[in]   uint8_t *payloadparam, pointer to the command payload parameter structure
 * @param[in]   uint16_t size_param, size of the payload for the command
 * @return      Negative Value = Failure
 *              0 = SUCCESS
 * @section description
 * This is a common function used to process a command to the Wi-Fi module.
 */

int16_t rsi_frame_write(rsi_frame_desc_t *uFrameDscFrame, uint8_t *payloadparam, uint16_t size_param)
{
  int16_t status = RSI_SUCCESS;

  status = rsi_hal_frame_write(uFrameDscFrame, payloadparam, size_param);
  if (status != RSI_SUCCESS) {
    return status;
  }

  return status;
}

/*====================================================*/
/**
 * @fn          int16 rsi_frame_read(uint8 *pkt_buffer)
 * @brief This  function is used to read the response from module.
 * @param[in]   uint8 *pkt_buffer, pointer to the buffer to which packet has to read
 *              which is used to store the response from the module
 * @param[out]  none
 * @return      Negative Value = Failure
 *              0 = SUCCESS
 * @section description
 * This is a common function to read response for all the command and data from Wi-Fi module.
 */

int16_t rsi_frame_read(uint8_t *pkt_buffer)
{
  int16_t status = RSI_SUCCESS;

  status = rsi_hal_frame_read(pkt_buffer);
  if (status != RSI_SUCCESS) {
    return status;
  }

  return status;
}

#endif
/** @} */

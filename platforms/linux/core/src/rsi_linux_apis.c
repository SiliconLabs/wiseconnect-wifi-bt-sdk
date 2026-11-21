/*******************************************************************************
* @file  rsi_linux_apis.c
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
 * @file      rsi_linux_apis.c
 * @version   1.2
 * @date      2018-July-04
 *
 *
 *
 * @brief Implements common functionality for all the commands
 *
 * @section Description
 * This file contains common api needed for all the commands
 *
 *
 */

/**
 * Includes
 */
#include<stdint.h>
#include "rsi_nl_app.h"
#include "rsi_wlan_apis.h"

const uint8_t rsi_frameCmdUpdateInfo[3] = {0x00,  0x00, 0xE1};

rsi_linux_driver_cb_t rsi_linux_driver_app_cb;


/*==============================================*/
/**
 * @fn             int16 rsi_update_info(type)
 * @brief          update the info to kernel
 * @param[in]      none
 * @param[out]     none
 * @return         errCode
 *                 -2 = Failure
 *                 0  = SUCCESS
 * @section description
 * This function is used to send some info which is required
 * by the kernel netdevice driver.
 */

int16_t rsi_update_info(uint8_t type)
{
  int16_t            retval = 0;
  uint8_t            payload[10];
#ifndef RSI_UART_INTERFACE
  switch(type)
  {
    case MODULE_POWER_CYCLE:
      payload[0] = type;
      retval =rsi_execute_cmd((uint8_t *)rsi_frameCmdUpdateInfo, payload, 1 /* type */);
      break;
    case UPDATE_JOIN_DONE:
      payload[0] = type;
#ifdef RSI_WLAN_ENABLE
      rsi_wlan_get(2,rsi_linux_driver_app_cb.mac_addr,6);
      memcpy(&payload[1],(uint8_t*)rsi_linux_driver_app_cb.mac_addr, 6);
#endif
      retval =rsi_execute_cmd((uint8_t *)rsi_frameCmdUpdateInfo, payload, 7 /* 1 byte type + 6 bytes mac */);
      break;
    case PS_CONTINUE:
      payload[0] = type;
      retval =rsi_execute_cmd((uint8_t *)rsi_frameCmdUpdateInfo, payload, 1 /* type */);
      break;
    case WKP_FROM_HOST:
      payload[0] = type;
      retval =rsi_execute_cmd((uint8_t *)rsi_frameCmdUpdateInfo, payload, 1 /* type */);
      break;
    default:
      retval = -2;
      break;
  }
  if(retval < 0)
  {
    return 2;
  }
#endif
  return 0;

}


/*====================================================*/
/**
 * @fn          int16 rsi_execute_cmd(uint8 *descparam,uint8 *payloadparam,uint16 size_param)
 * @brief       Common function for all the commands.
 * @param[in]   uint8 *descparam, pointer to the frame descriptor parameter structure
 * @param[in]   uint8 *payloadparam, pointer to the command payload parameter structure
 * @param[in]   uint16 size_param, size of the payload for the command
 * @return      errCode
 *              -2 = Command issue failed
 *              0  = SUCCESS
 * @section description
 * This is a common function used to process a command to the Wi-Fi module.
 */



int16_t rsi_execute_cmd(uint8_t *descparam, uint8_t *payloadparam, uint16_t size_param)
{

  int16_t                                         retval = 0;
  rsi_uFrameDsc         uFrameDscFrame;
  uint8_t           *cmd_buff;

  //! Build 16 bytes, send/receive command descriptor frame
  rsi_buildFrameDescriptor(&uFrameDscFrame,descparam);

  cmd_buff = rsi_alloc_and_init_cmdbuff((uint8_t *)&uFrameDscFrame,payloadparam,size_param);
  retval = rsi_send_usr_cmd(cmd_buff, GET_SEND_LENGTH(cmd_buff));
  if(retval < 0)
  {
    retval = -2;
  }
  //! Free the command buffer
  rsi_free(cmd_buff);

  return retval;
}


/*==================================================*/
/**
 * @fn          void rsi_buildFrameDescriptor(rsi_uFrameDsc *uFrameDscFrame, uint8 *cmd)
 * @brief       Creates a Frame Descriptor
 * @param[in]   rsi_uFrameDsc *uFrameDscFrame,Frame Descriptor
 * @param[in]   uint8 *cmd,Indicates type of the packet(data or management)
 * @param[out]  none
 * @return      none
 */

void rsi_buildFrameDescriptor(rsi_uFrameDsc *uFrameDscFrame, uint8_t *cmd)
{
  uint8_t i;
  for (i = 0; i < RSI_FRAME_DESC_LEN; i++)
  {
    uFrameDscFrame->uFrmDscBuf[i] = 0x00;
  }       //! zero the frame descriptor buffer
  //! data or management frame type
  //! uFrameDscFrame->uFrmDscBuf[14] = cmd[2];
  //! The first two bytes have different functions for management frames and
  //! control frames, but these are set in the pre-defined
  uFrameDscFrame->uFrmDscBuf[0] = cmd[0];
  //! arrays which are the argument passed to this function, so we just set the two values
  uFrameDscFrame->uFrmDscBuf[1] = cmd[1];
  uFrameDscFrame->uFrmDscBuf[2] = cmd[2] ;
  uFrameDscFrame->uFrmDscBuf[3] = cmd[3] ; // USB 2nd byte bug-fix

  return;
}



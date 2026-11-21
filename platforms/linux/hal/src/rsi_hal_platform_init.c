/*******************************************************************************
* @file  rsi_hal_platform_init.c
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
 * @file       rsi_hal_platform_init.c
 *
 * @section License
 * T:vshis program should be used on your own responsibility.
 * Silicon Labs assumes no responsibility for any losses
 * incurred by customers or third parties arising from the use of this file.
 *
 * @brief rsi_hal_platform_init, Top level file, it all starts here
 *
 * @section Description
 * This file contains the entry point for the application. It also has the
 * initialization of parameters of the global structure and the operations to
 * control & configure the module, like netlink socket initialization, frame_read/write etc.
 */


/**
 * Includes
 */

#include "rsi_nl_app.h"
#include <stdio.h>
#include <pthread.h>

rsi_linux_driver_cb_t rsi_linux_driver_app_cb;

/*==============================================*/
/**
 * @fn          int32_t rsi_hal_platform_init(void)
 * @brief       Host interface Initialization
 * @return      0 if success and -1 if failure
 * @section description
 * This API initializes the Host interface module
 */
int16_t rsi_hal_platform_init()
{
  pthread_t     thread1;
  rsi_linux_driver_cb_t *driver_cbPtr = &rsi_linux_driver_app_cb;
  /* generic return value and counter*/
  int16_t retval = 0;
  int16_t rc1;
  int8_t *message1 = "Recv Thread";

  /* Open a socket for issueing ioctls */
  if ((driver_cbPtr->ioctl_sd = socket (AF_INET, SOCK_DGRAM, 0)) < 0)
  {
    return 2;
  }

  retval = rsi_nl_socket_init();
  if (retval == -1) {
    /* Error occurred! */
    return 2;
  }

  rsi_fill_genl_nl_hdrs_for_cmd();
  retval = rsi_update_info(MODULE_POWER_CYCLE);
  if(retval < 0)
  {
    return 2;
  }

     pthread_create( &thread1, NULL, RecvThreadBody, (void*) message1);

   rsi_register_interrupt_irq();

  return 0;

}

/*==============================================*/
/**
 * @fn          int32_t rsi_hal_platform_deinit(void)
 * @brief       Host interface De-initialization
 * @return      0 if success and -1 if failure
 * @section description
 * This API de-initializes the Host interface module
 */
int32_t rsi_hal_platform_deinit(void)
{
     rsi_linux_driver_cb_t *driver_cbPtr = &rsi_linux_driver_app_cb;

    // Close IOCTL socket
    if (driver_cbPtr->ioctl_sd >= 0) {
        close(driver_cbPtr->ioctl_sd);
        driver_cbPtr->ioctl_sd = -1;
    }
    
    // Destroy mutex
    pthread_mutex_destroy(&driver_cbPtr->mutex1);
    
    return 0;
}

/*====================================================*/
/**
 * @fn          int16 rsi_hal_frame_write(rsi_uFrameDsc *uFrameDscFrame,uint8 *payloadparam,uint16 size_param)
 * @brief       Common function for all the commands.
 * @param[in]   uFrameDsc uFrameDscFrame, frame descriptor
 * @param[in]   uint8 *payloadparam, pointer to the command payload parameter structure
 * @param[in]   uint16 size_param, size of the payload for the command
 * @return      errCode
 *              0  = SUCCESS
 * @section description
 * This is a common function used to process a command to the Wi-Fi module.
 */
#ifndef RSI_UART_INTERFACE
int16_t rsi_hal_frame_write(rsi_uFrameDsc *uFrameDscFrame,uint8_t *payloadparam,uint16_t size_param)
{
  int16_t   retval = 0;
  uint8_t *dummy;
  int i;
#ifdef RSI_ENABLE_DEBUG_PRINT
    dummy = (uint8_t *)uFrameDscFrame;
    RSI_DPRINT(RSI_PL0,"\n **TX PACKET SENT** \n");
    for (i=0; i< 16 ; i++)
    {
      RSI_DPRINT (RSI_PL0, "0x%02x ", dummy[i]);
    }
    RSI_DPRINT(RSI_PL0, "\n");

    for (i=0; i< size_param ; i++)
    {
        RSI_DPRINT (RSI_PL0, "0x%02x ", payloadparam[i]);
        if ((i+1)%16 == 0) {
            RSI_DPRINT(RSI_PL0, "\n");
        }
    }
    RSI_DPRINT(RSI_PL0, "\n");
#endif
  retval = rsi_execute_cmd((uint8_t *)uFrameDscFrame, payloadparam, size_param);

  if(retval <0)
  {
    return -1;
  }
  else
  {
    return retval;
  }
}
#endif

/*=================================================*/
/**
 *@fn           int16 rsi_register_interrupt_irq(uint8 mask)
 * @brief       Sends the register interrupt irq to the Wi-Fi module via SPI
 * @param[in]   none
 * @param[out]  none
 * @return      errCode
 *              SPI:
 *              -1 = SPI busy / Timeout
 *              -2 = SPI Failure
 *              -3 = BUFFER FULL
 *              0  = SUCCESS
 *              UART/USB/USB-CDC:
 *              -2 = Command issue failed
 *              0  = SUCCESS
 * @section description
 * This API is used to register the interrupt irq.
 */

int16_t rsi_register_interrupt_irq(void)
{
  int16_t     retval;
  /* set unblock interrupt frame */
  uint8_t      rsi_frameRegisterInterruptIrq[RSI_BYTES_3] = {0x01,  0x40, 0xEE};

#ifdef RSI_DEBUG_PRINT
  RSI_DPRINT(RSI_PL3,"\r\n\nunblocking interrupt");
#endif
  retval = rsi_execute_cmd((uint8_t *)rsi_frameRegisterInterruptIrq, NULL, 0);
  return retval;
}

/*====================================================*/
/**
 * @fn          int16 rsi_hal_frame_read(uint8 *pkt_buffer)
 * @brief This  function is used to read the response from module.
 * @param[in]   uint8 *pkt_buffer, pointer to the buffer to which packet has to read
 *              which is used to store the response from the module
 * @param[out]  none
 * @return      errCode
 *              0  = SUCCESS
 * @section description
 * This is a common function to read response for all the command and data from Wi-Fi module.
 */
#ifndef RSI_UART_INTERFACE
int16_t rsi_hal_frame_read(uint8_t *pkt_buffer)
{
  pkt_struct_t *rx_pkt = NULL;
  uint16_t length,i;
  //! dequeue the packet from application control block
  rsi_dequeue_from_rcv_q(&rx_pkt);
  if(rx_pkt)
  {

    //! calculate the length from the packet
    length = (rx_pkt->desc[0] | ((rx_pkt->desc[1] & 0xF)<<8)) + RSI_FRAME_DESC_LEN;
    //! copy the data
    memcpy(pkt_buffer,&rx_pkt->desc,length);

#ifdef RSI_ENABLE_DEBUG_PRINT
    RSI_DPRINT(RSI_PL0,"\n **RX PACKET RECEIVED** \n");

    for (i=0; i< length ; i++)
    {
      RSI_DPRINT (RSI_PL0, "0x%02x ", pkt_buffer[i]);
      if ((i+1)%16 == 0) {
        RSI_DPRINT(RSI_PL0, "\n");
      }
    }
    RSI_DPRINT(RSI_PL0, "\n");
#endif


    //! Free the packet allocated
    free(rx_pkt);
    return 0;
  }
  return -1;
}
#endif

/*====================================================*/
/**
 * @fn          int16 rsi_hal_mem_rd(uint32_t addr, uint16_t len, uint8_t *dBuf)
 * @brief       This API reads the content of the host interface memory addr to the buffer dBuf
 * @param[in]   addr   -  Host interface address.
 * @param[in]   dBuf   -  Buffer.
 * @return      0      -  Success \n
 *		Negative Value   -  Failure
 */
int16_t rsi_hal_mem_rd(uint32_t addr, uint16_t len, uint8_t *dBuf)
{
    rsi_linux_driver_cb_t *driver_cbPtr = &rsi_linux_driver_app_cb;
    int16_t retval;

    if (dBuf == NULL) {
        return -1;
    }

    if (!len || len > 4) {
        return -2;
    }

    retval = rsi_ioctl_send_req(driver_cbPtr->ioctl_sd, dBuf, len, OID_MASTER_READ);

    return retval;
}

/*====================================================*/
/**
 * @fn          int16_t rsi_hal_mem_wr(uint32_t addr, uint16_t len, uint8_t *dBuf)
 * @brief       This API writes the content provided by dBuf to the host interface memory addr
 * @param[in]   addr    -  Destination address.
 * @param[in]   dBuf    -  Buffer with the content to be written to the register.
 * @return       0      -  Success \n
 *              Negative value - Failure
 */
int16_t rsi_hal_mem_wr(uint32_t addr, uint16_t len, uint8_t *dBuf)
{
    rsi_linux_driver_cb_t *driver_cbPtr = &rsi_linux_driver_app_cb;
    int16_t retval;

    if (dBuf == NULL) {
        return -1;
    }

    if (!len || len > 4) {
        return -2;
    }

    retval = rsi_ioctl_send_req(driver_cbPtr->ioctl_sd, dBuf, len, OID_WSC_BOOT_WRITE);

    return retval;
}

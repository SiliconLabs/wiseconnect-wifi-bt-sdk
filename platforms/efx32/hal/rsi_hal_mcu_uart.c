/*******************************************************************************
* @file  rsi_hal_mcu_uart.c
* @brief
*******************************************************************************
* # License
* <b>Copyright 2021 Silicon Laboratories Inc. www.silabs.com</b>
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
 * Includes
 */
#include "rsi_driver.h"
#include "em_device.h"
#include "em_usart.h"
/**
 * Global Variables
 */

/*==================================================================*/
/**
 * @fn         int16_t rsi_uart_send(uint8_t *ptrBuf,uint16_t bufLen)
 * @param[in]  uint8 *ptrBuf, pointer to the buffer with the data to be sent/received
 * @param[in]  uint16 bufLen, number of bytes to send
 * @param[out] None
 * @return     0, 0=success
 * @section description
 * This API is used to send data to the Wi-Fi module through the UART interface.
 */
int16_t rsi_uart_send(uint8_t *ptrBuf, uint16_t bufLen)
{
uint16_t i = 0u;
 while (i < bufLen){
  USART_Tx(USART0, ptrBuf[i++]);
}
return 0;
  UNUSED_PARAMETER(ptrBuf); //This statement is added only to resolve compilation warnings, value is unchanged
  UNUSED_PARAMETER(bufLen); //This statement is added only to resolve compilation warnings, value is unchanged
  return 0;
}


/*==================================================================*/
/**
 * @fn         int16_t rsi_uart_recv(uint8_t *ptrBuf,uint16_t bufLen)
 * @param[in]  uint8_t *ptrBuf, pointer to the buffer with the data to be sent/received
 * @param[in]  uint16_t bufLen, number of bytes to send
 * @param[out] None
 * @return     0, 0=success
 * @description
 * This API is used to receive data from Wi-Fi module through the UART interface.
 */


int16_t rsi_uart_recv(uint8_t *ptrBuf, uint16_t bufLen)
{
  UNUSED_PARAMETER(ptrBuf); //This statement is added only to resolve compilation warnings, value is unchanged
  UNUSED_PARAMETER(bufLen); //This statement is added only to resolve compilation warnings, value is unchanged
  return 0;
}

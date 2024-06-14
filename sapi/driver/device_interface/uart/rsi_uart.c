/*******************************************************************************
* @file  rsi_uart.c
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
 * @file     rsi_uart.c
 * @version  0.1
 * @date     15 Aug 2015
 *
 *
 *
 *  @brief : Contains UART HAL porting functionality
 *
 * Description  Contains UART HAL porting functionality
 *
 *
 */
#include "rsi_driver.h"
#ifndef EFM32_SDIO // This file is not needed for EFM32 board. In order to avoid compilation warnings, we excluded the below code for EFM32
#include "rsi_board_configuration.h"
#endif
#ifdef RSI_UART_INTERFACE
#ifdef RSI_STM32
#include "stm32f4xx_hal.h"
#include "stm32f4xx_hal_uart.h"
extern UART_HandleTypeDef huart1;
#endif
#include <stdlib.h>
#include <stdio.h>
#include "rsi_data_types.h"
#include "rsi_error.h"
#include "rsi_utils.h"
#include "rsi_pkt_mgmt.h"
#include "rsi_os.h"
#include "rsi_queue.h"
#include "rsi_uart.h"
#include "rsi_events.h"
#include <stdbool.h>
#ifdef EFX_UART_HAL
#include "em_gpio.h"
#include "em_usart.h"
#include "em_emu.h"
#include "em_cmu.h"
#include "em_chip.h"
#include "em_device.h"
#endif
#include "rsi_common_apis.h"
#include "rsi_hal.h"
USART_TypeDef *uart = USART0;
#define BUFFER_SIZE 2048UL
uint8_t uart_rev_buf[1600];
uint8_t uart_rev_buf2[1600];
volatile uint8_t data_rx_flag = 0;
volatile uint16_t index1      = 0;
uint32_t value = 0, previous = 0, desired_len = 0, last_len = 0;
volatile uint8_t abrd_bit = false;
rsi_linux_app_cb_t rsi_linux_app_cb;
/** @addtogroup DRIVER3
* @{
*/
/*==============================================*/
/**
 * @brief       Reads response for every command and data from the module.
 * @param[in]   pkt_buffer - pointer to buffer to which packet has to read
 * @return        0                - Success \n

 */

int16_t rsi_frame_read(uint8_t *pkt_buffer)
{
  // API to read packet via UART interface
  memcpy(pkt_buffer, uart_rev_buf2 + 4, last_len);

  return 0;
}

/*==============================================*/
/**
 * @brief       Process a command to the wlan module.
  * @param[in]   uFrameDscFrame  -  Frame descriptor
  * @param[in]   payloadparam    -  Pointer to the command payload parameter structure
  * @param[in]   size_param      -  Size of the payload for the command
  * @return       0              - SUCCESS \n
  *             < 0              - FAILURE \n
  */
int16_t rsi_frame_write(rsi_frame_desc_t *uFrameDscFrame, uint8_t *payloadparam, uint16_t size_param)
{
  int16_t retval = 0;

  // API to send packet via UART interface
  retval = rsi_uart_send((uint8_t *)uFrameDscFrame, (size_param + RSI_FRAME_DESC_LEN));
  UNUSED_PARAMETER(payloadparam); //This statement is added only to resolve compilation warnings, value is unchanged
#ifdef RSI_STM32
  while (huart1.gState != HAL_UART_STATE_READY)
    ;
  while (huart1.TxXferCount != 0)
    ;
#endif
  return retval;
}

/*==============================================*/
/**
 * @brief       Initialize the UART interface of the module
 * @param[in]   void
 * @return      0              - Success \n
 *              Non-Zero Value - Failure
 */

int32_t rsi_uart_init(void)
{
  SL_PRINTF(SL_UART_INIT_ENTRY, DRIVER, LOG_INFO);
#ifdef RSI_STM32
  huart1.Instance          = USART1;
  huart1.Init.BaudRate     = 921600;
  huart1.Init.WordLength   = UART_WORDLENGTH_8B;
  huart1.Init.StopBits     = UART_STOPBITS_1;
  huart1.Init.Parity       = UART_PARITY_NONE;
  huart1.Init.Mode         = UART_MODE_TX_RX;
  huart1.Init.HwFlowCtl    = UART_HWCONTROL_NONE;
  huart1.Init.OverSampling = UART_OVERSAMPLING_16;
#endif
  USART_InitAsync_TypeDef init = USART_INITASYNC_DEFAULT;
  init.baudrate                = 115200;
  // Enable oscillator to GPIO and USART0 modules
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_USART0, true);

  // UART RX
  GPIO_PinModeSet(gpioPortA, 6, gpioModeInput, 0);

  //UART TX
  GPIO_PinModeSet(gpioPortA, 5, gpioModePushPull, 1);

  //CTS //EXP header pin 6
  GPIO_PinModeSet(gpioPortC, 0, gpioModeInput, 1);

  //RTS //EXP header pin 4
  GPIO_PinModeSet(gpioPortC, 1, gpioModePushPull, 1);

  // Initialize UART asynchronous mode and route pins
  USART_InitAsync(USART0, &init);

  GPIO->USARTROUTE[0].RXROUTE  = (gpioPortA << _GPIO_USART_RXROUTE_PORT_SHIFT) | (6 << _GPIO_USART_RXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[0].TXROUTE  = (gpioPortA << _GPIO_USART_TXROUTE_PORT_SHIFT) | (5 << _GPIO_USART_TXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[0].CTSROUTE = (gpioPortC << _GPIO_USART_CTSROUTE_PORT_SHIFT) | (0 << _GPIO_USART_CTSROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[0].RTSROUTE = (gpioPortC << _GPIO_USART_RTSROUTE_PORT_SHIFT) | (1 << _GPIO_USART_RTSROUTE_PIN_SHIFT);

  // Enable USART interface pins
  GPIO->USARTROUTE[0].ROUTEEN = GPIO_USART_ROUTEEN_RXPEN | GPIO_USART_ROUTEEN_TXPEN | GPIO_USART_ROUTEEN_RTSPEN;

  // Enable interrupt
  NVIC_ClearPendingIRQ(USART0_RX_IRQn);
  NVIC_EnableIRQ(USART0_RX_IRQn);

  // Enable receive data valid interrupt
  USART_IntEnable(USART0, USART_IEN_RXDATAV);
  SL_PRINTF(SL_UART_INIT_EXIT, DRIVER, LOG_INFO);
  return 0;
}

/*==============================================*/
/**
 * @brief       De-initialize the UART interface of the module
 * @param[in]   void
 * @return      0              - Success \n
 */

int32_t rsi_uart_deinit(void)
{
  return 0;
}

void USART0_RX_IRQHandler(void)
{

  // Get the character just received
  uart_rev_buf[index1++] = USART0->RXDATA;
  data_rx_flag           = 1;
  NVIC_DisableIRQ(USART0_RX_IRQn);
  NVIC_ClearPendingIRQ(USART0_RX_IRQn);
  NVIC_EnableIRQ(USART0_RX_IRQn);
  if (abrd_bit == 1) {
    if (index1 == 3) {
      previous    = desired_len;
      desired_len = (((*(uint16_t *)&uart_rev_buf[0])) - 4);
    } else if ((index1 > 3) && ((index1 - 4) == desired_len)) {

      last_len = desired_len;
      memcpy(uart_rev_buf2, uart_rev_buf, index1);

      index1 = 0;
      value  = 0;
#ifdef RSI_WITH_OS
      rsi_set_event_from_isr(RSI_RX_EVENT);
#else
      rsi_set_event(RSI_RX_EVENT);
#endif
    }
  }

  if (index1 >= BUFFER_SIZE) {
    index1 = 0;
  }
}
/*=============================================*/
/**
 * @brief       Initialize the UART interface of the module
 * @param[in]   void
 * @return        0              - Success \n
 * @return        Non-Zero value - Failure \n
 */
short int rsi_uart_iface_int(void)
{
  char abrd[1], abrd1[1], load[1], load_binary[1];
  int16_t retval = 0;
  abrd[0]        = 0x7C;
  abrd1[0]       = 0x55;
  load_binary[0] = 'H';
  load[0]        = '1';
  int16_t count  = 0;
  while (1) {

    rsi_uart_send((uint8_t *)abrd, 1);
    rsi_delay_ms(200);

    if (uart_rev_buf[index1 - 1] == 0x55) {
      break;
    }
    if (count == 10) {
      return RSI_ERROR_BOARD_READY_TIMEOUT;
    }
    count++;
  }
  index1 = 0;
  //NVIC_EnableIRQ(USART1_RX_IRQn);
  rsi_uart_send((uint8_t *)abrd1, 1);

  while (1) {
    rsi_delay_ms(300);
    if (data_rx_flag == 1) {
      data_rx_flag = 0;
      if (strstr((const char *)uart_rev_buf, "JTAG Selection")) {
        // break the loop when find string "JTAG Selection"
        memset(uart_rev_buf, 0, sizeof(uart_rev_buf));
        index1 = 0;
        break;
      } else {
        return RSI_ERROR_TIMEOUT;
      }
    }
  }

  index1 = 0;
  //NVIC_EnableIRQ(USART1_RX_IRQn);
  rsi_uart_send((uint8_t *)load_binary, 1);
  while (1) {
    rsi_delay_ms(200);
    if (data_rx_flag == 1) {
      data_rx_flag = 0;
      if (strstr((const char *)uart_rev_buf, "Enter Next Command")) {
        // break the loop when find string "Enter Wireless Image No(0-f)"
        memset(uart_rev_buf, 0, sizeof(uart_rev_buf));
        index1 = 0;
        break;
      } else {
        return RSI_ERROR_INVALID_OPTION;
      }
    }
  }

  index1 = 0;
  //NVIC_EnableIRQ(USART1_RX_IRQn);
  rsi_uart_send((uint8_t *)load, 1);
  while (1) {
    rsi_delay_ms(10);
    if (data_rx_flag == 1) {
      data_rx_flag = 0;
      if (strstr((const char *)uart_rev_buf, "Loading...")) {
        // break the loop when find string "Enter Wireless Image No(0-f)"
        //LOG_PRINT("UART interface Loading Done.\r\n");
        memset(uart_rev_buf, 0, sizeof(uart_rev_buf));
        index1 = 0;
        retval = RSI_SUCCESS;
        break;
      }
    }
  }

  abrd_bit = 1; // set abrd bit true
  return retval;
}
#ifdef RSI_UART_FLOW_CTRL_ENABLE
void uart_init(void)
{

  USART_InitAsync_TypeDef init = USART_INITASYNC_DEFAULT;
  init.baudrate                = 115200;
  init.hwFlowControl           = usartHwFlowControlCtsAndRts;

  // Enable oscillator to GPIO and USART1 modules
  CMU_ClockEnable(cmuClock_GPIO, true);
  CMU_ClockEnable(cmuClock_USART0, true);

  // set pin modes for UART TX and RX pins
  GPIO_PinModeSet(gpioPortA, 6, gpioModeInput, 0);
  GPIO_PinModeSet(gpioPortA, 5, gpioModePushPull, 1);
  // set pin modes for UART TX and RX pins
  //CTS
  GPIO_PinModeSet(gpioPortC, 0, gpioModeInput, 1);
  //RTS
  GPIO_PinModeSet(gpioPortC, 1, gpioModePushPull, 1);

  // Initialize UART asynchronous mode and route pins
  USART_InitAsync(USART0, &init);

  GPIO->USARTROUTE[0].RXROUTE  = (gpioPortA << _GPIO_USART_RXROUTE_PORT_SHIFT) | (6 << _GPIO_USART_RXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[0].TXROUTE  = (gpioPortA << _GPIO_USART_TXROUTE_PORT_SHIFT) | (5 << _GPIO_USART_TXROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[0].CTSROUTE = (gpioPortC << _GPIO_USART_CTSROUTE_PORT_SHIFT) | (0 << _GPIO_USART_CTSROUTE_PIN_SHIFT);
  GPIO->USARTROUTE[0].RTSROUTE = (gpioPortC << _GPIO_USART_RTSROUTE_PORT_SHIFT) | (1 << _GPIO_USART_RTSROUTE_PIN_SHIFT);

  // Enable USART interface pins
  GPIO->USARTROUTE[0].ROUTEEN = GPIO_USART_ROUTEEN_RXPEN | GPIO_USART_ROUTEEN_TXPEN | GPIO_USART_ROUTEEN_RTSPEN;
  // Enable interrupt
  NVIC_ClearPendingIRQ(USART0_RX_IRQn);
  NVIC_EnableIRQ(USART0_RX_IRQn);

  // Enable receive data valid interrupt
  USART_IntEnable(USART0, USART_IEN_RXDATAV);
}
#endif
#endif
/** @} */

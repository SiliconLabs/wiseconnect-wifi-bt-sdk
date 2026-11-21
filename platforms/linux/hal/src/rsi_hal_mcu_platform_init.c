/*******************************************************************************
* @file  rsi_hal_mcu_platform_init.c
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

#include "rsi_driver.h"

/** @addtogroup HAL
* @{
*/
/*==============================================*/
/**
 * @fn           void rsi_hal_board_init()
 * @brief        This function Initializes the platform
 * @pre          None
 * @param[in]    none 
 * @param[out]   none
 * @return       none
 */

void rsi_hal_board_init(void)
{
  // Initializes the platform
}

/*==============================================*/
/**
 * @fn           void rsi_switch_to_high_clk_freq()
 * @brief        This function intializes SPI to high clock
 * @pre          None
 * @param[in]    none 
 * @param[out]   none
 * @return       none
 */

void rsi_switch_to_high_clk_freq(void)
{
  // Initializes the high speed clock
}

#ifdef RSI_UART_INTERFACE
/*==============================================*/
/**
 * @fn          void rsi_platform_uart_post_init(void)
 * @brief       Platform-specific post-initialization for UART interface
 * @param[in]   None
 * @return      None
 * @section description
 * This function is called after rsi_device_init() completes for UART interface.
 * Leave this function empty (default) to keep state as NONE and wait for 
 * CARD_READY response in rsi_wireless_init().
 */
void rsi_platform_uart_post_init(void)
{
}
#endif
/** @} */

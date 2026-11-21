/*******************************************************************************
* @file  rsi_hal_mcu_ioports.c
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
/*===========================================================*/
/**
 * @fn            void rsi_hal_config_gpio(uint8_t gpio_number,uint8_t mode,uint8_t value)
 * @brief         Configure host GPIO pin in output mode.
 * @param[in]     gpio_number - gpio pin number to be configured
 * @param[in]     mode        - input/output mode of the gpio pin to configure \n
 *                              0 - input mode \n
 *                              1 - output mode
 * @param[in]     value       - default value to be driven if gpio is configured in output mode \n
 *                              0 - low \n
 *                              1 - high
 * @return        void 
 */

void rsi_hal_config_gpio(uint8_t gpio_number, uint8_t mode, uint8_t value)
{

  // Initialise the gpio pins in input/output mode

  // Drive a default value on gpio if gpio is configured in output mode

  return;
}

/*===========================================================*/
/**
 * @fn            void rsi_hal_set_gpio(uint8_t gpio_number)
 * @brief         Drive or make the host GPIO value high. 
 * @param[in]     uint8_t gpio_number - gpio pin number
 * @return        void 
 */

void rsi_hal_set_gpio(uint8_t gpio_number)
{

  // drives a high value on GPIO

  return;
}

/*===========================================================*/
/**
 * @fn          uint8_t rsi_hal_get_gpio(uint8_t gpio_number)
 * @brief       Get the GPIO pin value.
 * @param[in]   gpio_number - gpio pin number
 * @return      gpio pin value
 */

uint8_t rsi_hal_get_gpio(uint8_t gpio_number)
{
  uint8_t gpio_value = 0;

  // Get the gpio value

  return gpio_value;
}

/*===========================================================*/
/**
 * @fn            void rsi_hal_clear_gpio(uint8_t gpio_number)
 * @brief         Drive or make the host GPIO value low.
 * @param[in]     gpio_number - gpio pin number
 * @return        void 
 */

void rsi_hal_clear_gpio(uint8_t gpio_number)
{

  // drives a low value on GPIO

  return;
}
/** @} */
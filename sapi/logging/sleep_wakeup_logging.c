/*******************************************************************************
 * @file  sleep_wakeup_logging.c
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
#ifdef SLEEP_WAKEUP_LOGGING
/**
 * Includes
 */
#include "sleep_wakeup_logging.h"

/* GPIO Interrupt Sleep flag for logging */
uint32_t g_gpio_sleep_log;
/* GPIO Interrupt Wakeup flag for logging */
uint32_t g_gpio_wakeup_log;
/* Variables used for Sleep and Wake up aspects */
uint32_t gpio_rise_timestamp, gpio_fall_timestamp, initial_time;

/*============================*/
/**
 * @fn         sl_sleepwakeup_log_task
 * @param[in]  none.
 * @return     none.
 * @section description
 * This API is used to log sleep/wakeup statistics based on the
 * global variables g_gpio_sleep_log & g_gpio_wakeup_log which are set
 * as part of GPIO Interrupt event handler.
 */
void sl_sleepwakeup_log_task()
{
  if (g_gpio_sleep_log) {
    LOG_PRINT("S %ld\n", (gpio_rise_timestamp - gpio_fall_timestamp));
    g_gpio_sleep_log = RSI_FALSE;
  } else if (g_gpio_wakeup_log) {
    LOG_PRINT("W %ld\n", (gpio_fall_timestamp - gpio_rise_timestamp));
    g_gpio_wakeup_log = RSI_FALSE;
  } else {
    /* no logging */
    return;
  }
}
#endif
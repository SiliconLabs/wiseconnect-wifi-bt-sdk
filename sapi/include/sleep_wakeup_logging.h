#ifndef __SLEEP_WAKEUP_LOGGING_H__
#define __SLEEP_WAKEUP_LOGGING_H__
//Includes

#include "rsi_driver.h"

/**
 * @fn         sl_sleepwakeup_log_task
 * @param[in]  none.
 * @return     none.
 * @section description
 * This API is used to log sleep/wakeup statistics based on the
 * global variables g_gpio_sleep_log & g_gpio_wakeup_log which are set
 * as part of GPIO Interrupt event handler.
 */
void sl_sleepwakeup_log_task();
#endif
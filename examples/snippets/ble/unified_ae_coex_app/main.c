
/*******************************************************************************
* @file  main.c
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
 * @file    main.c
 * @version 0.1
 * @date    8 May  2019
 *
 *
 *
 *  @brief : This file contains driver, module initialization and application execution
 *
 *  @section Description  This file contains driver, module initializations
 *
 *
 */

#include <rsi_common_app.h>
#include <rsi_driver.h>
#include "rsi_wlan.h"
#include "rsi_common_utils.h"

#ifdef RSI_WITH_OS
//! OS include file to refer OS specific functionality
#include "rsi_os.h"
#ifdef FW_LOGGING_ENABLE
//! Firmware logging includes
#include "sl_fw_logging.h"
#endif
#define RSI_DRIVER_TASK_PRIORITY 4
#define RSI_UI_TASK_PRIORITY     2
#define RSI_BLE_TASK_PRIORITY    1

//! Wireless driver task stack size
#define RSI_DRIVER_TASK_STACK_SIZE 1024 * 2
//! User interface task stack size
#define RSI_UI_TASK_STACK_SIZE 512 * 2
//! BLE task stack size
#define RSI_BLE_TASK_STACK_SIZE 1000

rsi_semaphore_handle_t ui_task_sem;
rsi_semaphore_handle_t bt_a2dp_avrcp_sync_sem;
rsi_semaphore_handle_t ble_hfp_sync_sem;

rsi_task_handle_t driver_task_handle = NULL;
rsi_task_handle_t ui_task_handle     = NULL;
rsi_task_handle_t ble_task_handle    = NULL;
#ifdef FW_LOGGING_ENABLE
rsi_task_handle_t fw_log_task_handle = NULL;
#endif
#endif
#ifdef FW_LOGGING_ENABLE
//! Memory length of driver updated for firmware logging
#define GLOBAL_BUFF_LEN (15000 + (FW_LOG_QUEUE_SIZE * MAX_FW_LOG_MSG_LEN))
#else
//! Memory length for driver
#define GLOBAL_BUFF_LEN 15000 //16900

#endif

//! Flag for infinite loop
#define RSI_FOREVER 1

//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN] = { 0 };

extern void rsi_ui_app_task(void);
extern void rsi_ble_main_app_task(void);

int main(void)
{
#ifdef RSI_WITH_OS
  (void)driver_task_handle;
  (void)ble_task_handle;
#endif

  int32_t status = RSI_SUCCESS;

  //! Board Initialization
  rsi_hal_board_init();

  //! Driver initialization
  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }

#ifdef RSI_M4_INTERFACE
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Failed device init 0x%x\r\n", status);
  }
#endif

#ifdef RSI_WITH_OS
  rsi_semaphore_create(&ui_task_sem, 0);
#else
  _assert("NEED OS");
#endif

#ifdef RSI_WITH_OS
  rsi_task_create((void *)rsi_ble_main_app_task,
                  (uint8_t *)"ble_task",
                  RSI_BLE_TASK_STACK_SIZE,
                  NULL,
                  RSI_BLE_TASK_PRIORITY,
                  &ble_task_handle);
  rsi_task_create((void *)rsi_ui_app_task,
                  (uint8_t *)"ui_task",
                  RSI_UI_TASK_STACK_SIZE,
                  NULL,
                  RSI_UI_TASK_PRIORITY,
                  &ui_task_handle);

  //! Task created for Driver task
  rsi_task_create((void *)rsi_wireless_driver_task,
                  (uint8_t *)"driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);
#endif

#ifdef FW_LOGGING_ENABLE
  //! Create firmware logging semaphore
  rsi_semaphore_create(&fw_log_app_sem, 0);
  //! Create firmware logging task
  rsi_task_create((rsi_task_function_t)sl_fw_log_task,
                  (uint8_t *)"fw_log_task",
                  RSI_FW_TASK_STACK_SIZE,
                  NULL,
                  RSI_FW_TASK_PRIORITY,
                  &fw_log_task_handle);
#endif
  //! OS TAsk Start the scheduler
#ifdef RSI_WITH_OS
  rsi_start_os_scheduler();
#else
  _assert("NEED OS");
#endif
}

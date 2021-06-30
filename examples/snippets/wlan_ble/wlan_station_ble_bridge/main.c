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
/*************************************************************************
 *
 */

/*================================================================================
 * @brief : This file contains example application for WLAN Station BLE Bridge
 * @section Description :
 * The coex application demonstrates how information can be exchanged seamlessly
 * using two wireless protocols (WLAN and BLE) running in the same.
 * In this coex application, Redpine BLE device connects with remote BLE device
 * (Smart Phone) and Redpine WiFi interface connects with an Access Point in
 * station mode and do data transfer in BLE and WiFi interfaces.
 =================================================================================*/

/**
 * Include files
 **/

#include "rsi_driver.h"

#ifdef RSI_M4_INTERFACE
#include "rsi_board.h"
#endif
//! Error include files
#include "rsi_error.h"

#ifdef RSI_WITH_OS
//! OS include file to refer OS specific functionality
#include "rsi_os.h"
#endif

//!BLE Specific inclusions
#include <rsi_ble_apis.h>
#include <rsi_ble_config.h>

//! Memory length for driver
#define GLOBAL_BUFF_LEN 15000

#ifdef RSI_WITH_OS
//!Wlan BLE task priority
#define RSI_WLAN_BLE_TASK_PRIORITY 1

//! Wlan task priority
#define RSI_WLAN_TASK_PRIORITY 2

//! Ble task priority
#define RSI_BLE_TASK_PRIORITY 3

//! Wireless driver task priority
#define RSI_DRIVER_TASK_PRIORITY 4

//! Wlan BT task stack size
#define RSI_WLAN_BLE_TASK_STACK_SIZE 1000

//! Wlan task stack size
#define RSI_WLAN_TASK_STACK_SIZE 1000

//! BT task stack size
#define RSI_BLE_TASK_STACK_SIZE 1000

//! Wireless driver task stack size
#define RSI_DRIVER_TASK_STACK_SIZE 3000

#endif

//! Wlan client mode
#define RSI_WLAN_CLIENT_MODE 0

//! BTLE Coex mode
#define RSI_BTLE_MODE 13

//! Parameter to run forever loop
#define RSI_FOREVER 1

//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN];

//! Function prototypes
extern void rsi_wlan_app_task(void);
extern void rsi_ble_app_init(void);
extern void rsi_ble_app_task(void);

#ifdef RSI_WITH_OS
rsi_semaphore_handle_t commonsem, wlan_thread_sem, ble_thread_sem;

int32_t rsi_wlan_ble_app_init(void)
{
  int32_t status                     = RSI_SUCCESS;
  rsi_task_handle_t wlan_task_handle = NULL;
  rsi_task_handle_t ble_task_handle  = NULL;

#ifdef RSI_WITH_OS
  //! Redpine module intialisation
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nDevice Initialization Failed, Error Code : 0x%X\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nDevice Initialization Success\r\n");
  }
#endif

  //! WiSeConnect initialization
  status = rsi_wireless_init(RSI_WLAN_CLIENT_MODE, RSI_OPERMODE_WLAN_BLE);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWireless Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nWireless Initialization Success\r\n");
  }

  //! BLE initialization
  rsi_ble_app_init();

  //FIXME
  //rsi_wlan_app_init();

  //! Task created for WLAN task
  rsi_task_create((rsi_task_function_t)rsi_wlan_app_task,
                  (uint8_t *)"wlan_task",
                  RSI_WLAN_TASK_STACK_SIZE,
                  NULL,
                  RSI_WLAN_TASK_PRIORITY,
                  &wlan_task_handle);

  //! Task created for BLE task
  rsi_task_create((rsi_task_function_t)rsi_ble_app_task,
                  (uint8_t *)"ble_task",
                  RSI_BLE_TASK_STACK_SIZE,
                  NULL,
                  RSI_BLE_TASK_PRIORITY,
                  &ble_task_handle);

  rsi_semaphore_wait(&commonsem, 0);
  return RSI_SUCCESS;
}

#endif

int32_t rsi_wlan_ble_app(void)
{
  int32_t status = RSI_SUCCESS;

  //! WiSeConnect initialization
  status = rsi_wireless_init(RSI_WLAN_CLIENT_MODE, RSI_OPERMODE_WLAN_BLE);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWireless Initialization Failed, Error Code : 0x%X\r\n", (int)status);
    return status;
  } else {
    LOG_PRINT("\r\nWireless Initialization Success\r\n");
  }

  //! WLAN initialization
  status = rsi_wlan_app_init();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWLAN App Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nWLAN App Initialization Success\r\n");
  }

  //! BLE initialization
  rsi_ble_app_init();

  while (RSI_FOREVER) {
    //! WLAN application tasks
    rsi_wlan_app_task();

    //! BLE application tasks
    rsi_ble_app_task();

    //! wireless driver tasks
    rsi_wireless_driver_task();
  }
}

//! Forever in wireless driver task
void main_loop()
{
  while (RSI_FOREVER) {
    rsi_wireless_driver_task();
  }
}

//! main funtion definition
int main(void)
{
  int32_t status;

#ifdef RSI_WITH_OS
  rsi_task_handle_t wlan_ble_task_handle = NULL;
  rsi_task_handle_t driver_task_handle   = NULL;
#endif

  //! Driver initialization
  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }

#ifndef RSI_WITH_OS
  //! Redpine module intialization
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nDevice Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nDevice Initialization Success\r\n");
  }
#endif
  //Start BT Stack
  intialize_bt_stack(STACK_BTLE_MODE);

#ifdef RSI_WITH_OS

  rsi_semaphore_create(&wlan_thread_sem, 0);
  rsi_semaphore_create(&ble_thread_sem, 0);
  rsi_semaphore_create(&commonsem, 0);
  //! OS case
  //! Task created for COMMON task
  rsi_task_create((rsi_task_function_t)rsi_wlan_ble_app_init,
                  (uint8_t *)"common_task",
                  RSI_WLAN_BLE_TASK_STACK_SIZE,
                  NULL,
                  RSI_WLAN_BLE_TASK_PRIORITY,
                  &wlan_ble_task_handle);

  //! Task created for Driver task
  rsi_task_create((rsi_task_function_t)rsi_wireless_driver_task,
                  (uint8_t *)"driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);

  //! OS TAsk Start the scheduler
  rsi_start_os_scheduler();

#else
  //! NON - OS case
  //! Call WLAN BLE application
  status = rsi_wlan_ble_app();

  //! Application main loop
  main_loop();
#endif
  return status;
}

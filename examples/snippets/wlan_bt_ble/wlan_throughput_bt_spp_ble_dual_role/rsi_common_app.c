/*******************************************************************************
* @file  rsi_common_app.c
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
 * @file    rsi_common_app.c
 * @version 0.1
 * @date    01 Feb 2021
 *
 *
 *  @section Licenseremote_name
 *  This program should be used on your own responsibility.
 *  Silicon Labs assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief : This file contains example application for device initialization
 *
 *  @section Description  This application initiates SiLabs device and create tasks.
 *
 */

/*=======================================================================*/
//  ! INCLUDES
/*=======================================================================*/

#include <stdio.h>
#include <string.h>
#include "rsi_driver.h"
#include "rsi_common_config.h"
#include "rsi_ble_device_info.h"

#ifdef FW_LOGGING_ENABLE
//! Firmware logging includes
#include "sl_fw_logging.h"
#endif
/*=======================================================================*/
//   ! MACROS
/*=======================================================================*/
#ifdef FW_LOGGING_ENABLE
//! Memory length of driver updated for firmware logging
#define GLOBAL_BUFF_LEN (20000 + (FW_LOG_QUEUE_SIZE * MAX_FW_LOG_MSG_LEN))
#else
#define GLOBAL_BUFF_LEN 20000
#endif

/*=======================================================================*/
//   ! GLOBAL VARIABLES
/*=======================================================================*/
rsi_task_handle_t driver_task_handle = NULL;
rsi_task_handle_t common_task_handle = NULL;

//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN] = { 0 };
//! flag to check bt power save
#if RSI_ENABLE_BLE_TEST
rsi_parsed_conf_t rsi_parsed_conf = { 0 };
#endif
rsi_semaphore_handle_t ble_main_task_sem, ble_slave_conn_sem, bt_app_sem, wlan_app_sem, bt_inquiry_sem, ble_scan_sem;
#if (RSI_ENABLE_BT_TEST && WLAN_THROUGHPUT_TEST)
rsi_semaphore_handle_t bt_wlan_throughput_sync_sem;
#endif
#if (RSI_ENABLE_BLE_TEST && WLAN_THROUGHPUT_TEST)
rsi_semaphore_handle_t ble_wlan_throughput_sync_sem;
#endif

#if WLAN_SYNC_REQ
rsi_semaphore_handle_t sync_coex_ble_sem, sync_coex_bt_sem;
#endif
rsi_task_handle_t ble_main_app_task_handle, bt_app_task_handle, wlan_app_task_handle;
bool rsi_ble_running, rsi_bt_running, rsi_wlan_running, wlan_radio_initialized, powersave_cmd_given;
bool other_protocol_activity_enabled = false;
rsi_mutex_handle_t power_cmd_mutex;

/*=======================================================================*/
//   ! EXTERN VARIABLES
/*=======================================================================*/
#ifdef FW_LOGGING_ENABLE
/*=======================================================================*/
//!    Firmware logging configurations
/*=======================================================================*/
//! Firmware logging task defines
#define RSI_FW_TASK_STACK_SIZE (512 * 2)
#define RSI_FW_TASK_PRIORITY   2
//! Firmware logging variables
extern rsi_semaphore_handle_t fw_log_app_sem;
rsi_task_handle_t fw_log_task_handle = NULL;
//! Firmware logging prototypes
void sl_fw_log_callback(uint8_t *log_message, uint16_t log_message_length);
void sl_fw_log_task(void);
#endif

/*=======================================================================*/
//   ! EXTERN FUNCTIONS
/*=======================================================================*/

/*=======================================================================*/
//   ! PROCEDURES
/*=======================================================================*/
/*==============================================*/
/**
 * @fn         rsi_initiate_power_save
 * @brief      send power save command to RS9116 module
 *
 * @param[out] none
 * @return     status of commands, success-> 0, failure ->-1
 * @section description
 * This function sends command to keep module in power save
 */
int32_t rsi_initiate_power_save(void)
{
  int32_t status = RSI_SUCCESS;
  //! enable wlan radio
  if (!wlan_radio_initialized) {
    status = rsi_wlan_radio_init();
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\n radio init failed,error = %ld\n", status);
      return status;
    } else {
      wlan_radio_initialized = true;
    }
  }
  status = rsi_bt_power_save_profile(RSI_SLEEP_MODE_2, PSP_TYPE);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Failed in initiating power save\r\n");
    return status;
  }
  //! initiating power save in wlan mode
  status = rsi_wlan_power_save_profile(RSI_SLEEP_MODE_2, PSP_TYPE);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Failed in initiating power save\r\n");
    return status;
  }
  return status;
}

/*==============================================*/
/**
 * @fn         rsi_ble_initialize_conn_buffer
 * @brief      this function initializes the configurations for each connection
 * @param[out] none
 * @param[out] none
 * @return     none
 * @section description
 */
int8_t rsi_ble_initialize_conn_buffer(rsi_ble_conn_config_t *ble_conn_spec_conf)
{
  int8_t status = RSI_SUCCESS;
  if (ble_conn_spec_conf != NULL) {
    if (RSI_BLE_MAX_NBR_SLAVES > 0) {
      //! Initialize slave1 configurations
      ble_conn_spec_conf[SLAVE1].smp_enable        = SMP_ENABLE_S1;
      ble_conn_spec_conf[SLAVE1].add_to_whitelist  = ADD_TO_WHITELIST_S1;
      ble_conn_spec_conf[SLAVE1].profile_discovery = PROFILE_QUERY_S1;
      ble_conn_spec_conf[SLAVE1].data_transfer     = DATA_TRANSFER_S1;
      //ble_conn_spec_conf[SLAVE1].bidir_datatransfer = SMP_ENABLE_S1;
      ble_conn_spec_conf[SLAVE1].rx_notifications                 = RX_NOTIFICATIONS_FROM_S1;
      ble_conn_spec_conf[SLAVE1].rx_indications                   = RX_INDICATIONS_FROM_S1;
      ble_conn_spec_conf[SLAVE1].tx_notifications                 = TX_NOTIFICATIONS_TO_S1;
      ble_conn_spec_conf[SLAVE1].tx_write                         = TX_WRITES_TO_S1;
      ble_conn_spec_conf[SLAVE1].tx_write_no_response             = TX_WRITES_NO_RESP_TO_S1;
      ble_conn_spec_conf[SLAVE1].tx_indications                   = TX_INDICATIONS_TO_S1;
      ble_conn_spec_conf[SLAVE1].conn_param_update.conn_int       = CONN_INTERVAL_S1;
      ble_conn_spec_conf[SLAVE1].conn_param_update.conn_latncy    = CONN_LATENCY_S1;
      ble_conn_spec_conf[SLAVE1].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_S1;
      ble_conn_spec_conf[SLAVE1].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_S1;
      ble_conn_spec_conf[SLAVE1].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_S1;
      ble_conn_spec_conf[SLAVE1].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_S1;
    }

    if (RSI_BLE_MAX_NBR_SLAVES > 1) {
      //! Initialize slave2 configurations
      ble_conn_spec_conf[SLAVE2].smp_enable        = SMP_ENABLE_S2;
      ble_conn_spec_conf[SLAVE2].add_to_whitelist  = ADD_TO_WHITELIST_S2;
      ble_conn_spec_conf[SLAVE2].profile_discovery = PROFILE_QUERY_S2;
      ble_conn_spec_conf[SLAVE2].data_transfer     = DATA_TRANSFER_S2;
      //ble_conn_spec_conf[SLAVE2].bidir_datatransfer = SMP_ENABLE_S2;
      ble_conn_spec_conf[SLAVE2].rx_notifications                 = RX_NOTIFICATIONS_FROM_S2;
      ble_conn_spec_conf[SLAVE2].rx_indications                   = RX_INDICATIONS_FROM_S2;
      ble_conn_spec_conf[SLAVE2].tx_notifications                 = TX_NOTIFICATIONS_TO_S2;
      ble_conn_spec_conf[SLAVE2].tx_write                         = TX_WRITES_TO_S2;
      ble_conn_spec_conf[SLAVE2].tx_write_no_response             = TX_WRITES_NO_RESP_TO_S2;
      ble_conn_spec_conf[SLAVE2].tx_indications                   = TX_INDICATIONS_TO_S2;
      ble_conn_spec_conf[SLAVE2].conn_param_update.conn_int       = CONN_INTERVAL_S2;
      ble_conn_spec_conf[SLAVE2].conn_param_update.conn_latncy    = CONN_LATENCY_S2;
      ble_conn_spec_conf[SLAVE2].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_S2;
      ble_conn_spec_conf[SLAVE2].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_S2;
      ble_conn_spec_conf[SLAVE2].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_S2;
      ble_conn_spec_conf[SLAVE2].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_S2;
    }

    if (RSI_BLE_MAX_NBR_SLAVES > 2) {
      //! Initialize SLAVE3 configurations
      ble_conn_spec_conf[SLAVE3].smp_enable        = SMP_ENABLE_S3;
      ble_conn_spec_conf[SLAVE3].add_to_whitelist  = ADD_TO_WHITELIST_S3;
      ble_conn_spec_conf[SLAVE3].profile_discovery = PROFILE_QUERY_S3;
      ble_conn_spec_conf[SLAVE3].data_transfer     = DATA_TRANSFER_S3;
      //ble_conn_spec_conf[SLAVE3].bidir_datatransfer = SMP_ENABLE_S3;
      ble_conn_spec_conf[SLAVE3].rx_notifications                 = RX_NOTIFICATIONS_FROM_S3;
      ble_conn_spec_conf[SLAVE3].rx_indications                   = RX_INDICATIONS_FROM_S3;
      ble_conn_spec_conf[SLAVE3].tx_notifications                 = TX_NOTIFICATIONS_TO_S3;
      ble_conn_spec_conf[SLAVE3].tx_write                         = TX_WRITES_TO_S3;
      ble_conn_spec_conf[SLAVE3].tx_write_no_response             = TX_WRITES_NO_RESP_TO_S3;
      ble_conn_spec_conf[SLAVE3].tx_indications                   = TX_INDICATIONS_TO_S3;
      ble_conn_spec_conf[SLAVE3].conn_param_update.conn_int       = CONN_INTERVAL_S3;
      ble_conn_spec_conf[SLAVE3].conn_param_update.conn_latncy    = CONN_LATENCY_S3;
      ble_conn_spec_conf[SLAVE3].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_S3;
      ble_conn_spec_conf[SLAVE3].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_S3;
      ble_conn_spec_conf[SLAVE3].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_S3;
      ble_conn_spec_conf[SLAVE3].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_S3;
    }

    if (RSI_BLE_MAX_NBR_MASTERS > 0) {
      //! Initialize master1 configurations
      ble_conn_spec_conf[MASTER1].smp_enable        = SMP_ENABLE_M1;
      ble_conn_spec_conf[MASTER1].add_to_whitelist  = ADD_TO_WHITELIST_M1;
      ble_conn_spec_conf[MASTER1].profile_discovery = PROFILE_QUERY_M1;
      ble_conn_spec_conf[MASTER1].data_transfer     = DATA_TRANSFER_M1;
      //ble_conn_spec_conf[MASTER1].bidir_datatransfer = SMP_ENABLE_M1;
      ble_conn_spec_conf[MASTER1].rx_notifications                 = RX_NOTIFICATIONS_FROM_M1;
      ble_conn_spec_conf[MASTER1].rx_indications                   = RX_INDICATIONS_FROM_M1;
      ble_conn_spec_conf[MASTER1].tx_notifications                 = TX_NOTIFICATIONS_TO_M1;
      ble_conn_spec_conf[MASTER1].tx_write                         = TX_WRITES_TO_M1;
      ble_conn_spec_conf[MASTER1].tx_write_no_response             = TX_WRITES_NO_RESP_TO_M1;
      ble_conn_spec_conf[MASTER1].tx_indications                   = TX_INDICATIONS_TO_M1;
      ble_conn_spec_conf[MASTER1].conn_param_update.conn_int       = CONN_INTERVAL_M1;
      ble_conn_spec_conf[MASTER1].conn_param_update.conn_latncy    = CONN_LATENCY_M1;
      ble_conn_spec_conf[MASTER1].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_M1;
      ble_conn_spec_conf[MASTER1].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_M1;
      ble_conn_spec_conf[MASTER1].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_M1;
      ble_conn_spec_conf[MASTER1].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_M1;
    }

    if (RSI_BLE_MAX_NBR_MASTERS > 1) {
      //! Initialize master2 configurations
      ble_conn_spec_conf[MASTER2].smp_enable        = SMP_ENABLE_M2;
      ble_conn_spec_conf[MASTER2].add_to_whitelist  = ADD_TO_WHITELIST_M2;
      ble_conn_spec_conf[MASTER2].profile_discovery = PROFILE_QUERY_M2;
      ble_conn_spec_conf[MASTER2].data_transfer     = DATA_TRANSFER_M2;
      //ble_conn_spec_conf[MASTER2].bidir_datatransfer = SMP_ENABLE_M2;
      ble_conn_spec_conf[MASTER2].rx_notifications                 = RX_NOTIFICATIONS_FROM_M2;
      ble_conn_spec_conf[MASTER2].rx_indications                   = RX_INDICATIONS_FROM_M2;
      ble_conn_spec_conf[MASTER2].tx_notifications                 = TX_NOTIFICATIONS_TO_M2;
      ble_conn_spec_conf[MASTER2].tx_write                         = TX_WRITES_TO_M2;
      ble_conn_spec_conf[MASTER2].tx_write_no_response             = TX_WRITES_NO_RESP_TO_M2;
      ble_conn_spec_conf[MASTER2].tx_indications                   = TX_INDICATIONS_TO_M2;
      ble_conn_spec_conf[MASTER2].conn_param_update.conn_int       = CONN_INTERVAL_M2;
      ble_conn_spec_conf[MASTER2].conn_param_update.conn_latncy    = CONN_LATENCY_M2;
      ble_conn_spec_conf[MASTER2].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_M2;
      ble_conn_spec_conf[MASTER2].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_M2;
      ble_conn_spec_conf[MASTER2].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_M2;
      ble_conn_spec_conf[MASTER2].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_M2;
    }
  } else {
    LOG_PRINT("\r\n Invalid buffer passed \r\n");
    status = RSI_FAILURE;
  }
  return status;
}

#if RSI_ENABLE_BLE_TEST
/*==============================================*/
/**
 * @fn         rsi_fill_user_config
 * @brief      this function fills the compile time user inputs to local buffer
 * @param[out] none
 * @return     none.
 * @section description
 * this function fills the compile time userinputs to local buffer
 */
int8_t rsi_fill_user_config()
{
  int8_t status = RSI_SUCCESS;
  //! copy protocol selection macros
  status = rsi_ble_initialize_conn_buffer((rsi_ble_conn_config_t *)&rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config);
  return status;
}
#endif

/*==============================================*/
/**
 * @fn         rsi_common_app_task
 * @brief      This function creates the main tasks for selected protocol
 * @param[out] none
 * @return     none.
 * @section description
 * This function creates the main tasks for enabled protocols based on local buffer 'rsi_parsed_conf'
 */
void rsi_common_app_task(void)
{
  int8_t status            = RSI_SUCCESS;
  rsi_bt_running           = false;
  rsi_wlan_running         = false;
  wlan_radio_initialized   = false;
  powersave_cmd_given      = false;
  ble_main_app_task_handle = NULL;
  bt_app_task_handle       = NULL;
  wlan_app_task_handle     = NULL;
  uint8_t fmversion[20]    = { 0 };

  while (1) {
#ifdef FW_LOGGING_ENABLE
    //Fw log component level
    sl_fw_log_level_t fw_component_log_level;
#endif
    //! SiLabs module initialization
    status = rsi_device_init(LOAD_NWP_FW);
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\r\n device init failed \n");
      return;
    }
// rsi_wireless_driver_task is creating in rsi_common_app_task only for EFM platform
#ifdef EFM32GG11B820F2048GL192
    //! Task created for Driver task
    rsi_task_create((rsi_task_function_t)rsi_wireless_driver_task,
                    (uint8_t *)"driver_task",
                    RSI_DRIVER_TASK_STACK_SIZE,
                    NULL,
                    RSI_DRIVER_TASK_PRIORITY,
                    &driver_task_handle);
#endif
    //! WiSeConnect initialization
    status = rsi_wireless_init(RSI_WLAN_CLIENT_MODE, RSI_COEX_MODE);
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\r\n wireless init failed \n");
      return;
    }

    //! Firmware version Prints
    status = rsi_get_fw_version(fmversion, sizeof(fmversion));
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\r\nFirmware version Failed, Error Code : 0x%lX\r\n", status);
    } else {
      LOG_PRINT("\nfirmware_version = %s", fmversion);
    }
#ifdef FW_LOGGING_ENABLE
    //! Set log levels for firmware components
    sl_set_fw_component_log_levels(&fw_component_log_level);

    //! Configure firmware logging
    status = sl_fw_log_configure(FW_LOG_ENABLE,
                                 FW_TSF_GRANULARITY_US,
                                 &fw_component_log_level,
                                 FW_LOG_BUFFER_SIZE,
                                 sl_fw_log_callback);
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\r\n Firmware Logging Init Failed\r\n");
    }
#ifdef RSI_WITH_OS
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
#endif
    //! Send Feature frame
    status = rsi_send_feature_frame();
    if (status != RSI_SUCCESS) {
      return;
    }
#if RSI_ENABLE_BLE_TEST
    //! fill the configurations in local structure based on compilation macros
    status = rsi_fill_user_config();
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\r\n failed to fill the configurations in local buffer \r\n");
      return;
    }
#endif
    //! initialize wlan radio
    status = rsi_wlan_radio_init();
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\n WLAN radio init failed \n");
      return;
    } else {
      wlan_radio_initialized = true;
    }
    LOG_PRINT("\n Basic Init Done i.e deviceinit=>wirelessinit=>feature=>wlanradioinit \n");

#if ENABLE_POWER_SAVE
    //! create mutex
    status = rsi_mutex_create(&power_cmd_mutex);
    if (status != RSI_ERROR_NONE) {
      LOG_PRINT("failed to create mutex object, error = %d \r\n", status);
      return;
    }
#endif

#if RSI_ENABLE_WLAN_TEST
    rsi_wlan_running = true;                //! Making sure wlan got triggered.
    rsi_semaphore_create(&wlan_app_sem, 0); //! This lock will be used from one download complete notification.
#if WLAN_SYNC_REQ
    rsi_semaphore_create(&sync_coex_bt_sem, 0); //! This lock will be used from wlan task to be done.
    rsi_semaphore_create(&sync_coex_ble_sem, 0);
#endif
#ifdef RSI_WITH_OS
    status = rsi_task_create((rsi_task_function_t)(int32_t)rsi_wlan_app_task,
                             (uint8_t *)"wlan_task",
                             RSI_WLAN_APP_TASK_SIZE,
                             NULL,
                             RSI_WLAN_APP_TASK_PRIORITY,
                             &wlan_app_task_handle);
    if (status != RSI_ERROR_NONE) {
      LOG_PRINT("\r\n rsi_wlan_app_task failed to create \r\n");
      break;
    }
#endif
#endif

    //! create ble main task if ble protocol is selected
#if RSI_ENABLE_BLE_TEST
    rsi_ble_running = 1;
    status          = rsi_semaphore_create(&ble_main_task_sem, 0);
    if (status != RSI_ERROR_NONE) {
      LOG_PRINT("\r\n failed to create ble_main_task_sem \r\n");
      return;
    }
    status = rsi_semaphore_create(&ble_scan_sem, 0);
    if (status != RSI_ERROR_NONE) {
      LOG_PRINT("\r\n failed to create ble_scan_sem \r\n");
      return;
    }
    if (RSI_BLE_MAX_NBR_SLAVES > 0) {
      status = rsi_semaphore_create(&ble_slave_conn_sem, 0);
      if (status != RSI_ERROR_NONE) {
        LOG_PRINT("\r\n failed to create ble_slave_conn_sem \r\n");
        return;
      }
    }
#if WLAN_THROUGHPUT_TEST
    status = rsi_semaphore_create(&ble_wlan_throughput_sync_sem, 0);
    if (status != RSI_ERROR_NONE) {
      LOG_PRINT("\r\n failed to create bt_wlan_throughput_sync_sem \r\n");
      return;
    }
#endif
#ifdef RSI_WITH_OS
    status = rsi_task_create((rsi_task_function_t)rsi_ble_main_app_task,
                             (uint8_t *)"ble_main_task",
                             RSI_BLE_APP_MAIN_TASK_SIZE,
                             NULL,
                             RSI_BLE_MAIN_TASK_PRIORITY,
                             &ble_main_app_task_handle);
    if (status != RSI_ERROR_NONE) {
      LOG_PRINT("\r\n ble main task failed to create \r\n");
      return;
    }
#endif
#endif

    //! create bt task if bt protocol is selected
#if RSI_ENABLE_BT_TEST
    rsi_bt_running = 1;
    rsi_semaphore_create(&bt_app_sem, 0);
    rsi_semaphore_create(&bt_inquiry_sem, 0);
#ifdef RSI_WITH_OS
    status = rsi_task_create((rsi_task_function_t)(int32_t)rsi_bt_spp_task,
                             (uint8_t *)"bt_spp_task",
                             RSI_BT_APP_TASK_SIZE,
                             NULL,
                             RSI_BT_APP_TASK_PRIORITY,
                             &bt_app_task_handle);
    if (status != RSI_ERROR_NONE) {
      LOG_PRINT("\r\n rsi_bt_spp_task failed to create \r\n");
      return;
    }
#endif
#if WLAN_THROUGHPUT_TEST
    status = rsi_semaphore_create(&bt_wlan_throughput_sync_sem, 0);
    if (status != RSI_ERROR_NONE) {
      LOG_PRINT("\r\n failed to create bt_wlan_throughput_sync_sem \r\n");
      return;
    }

#endif
#endif
    //! delete the task as initialization is completed
#ifdef RSI_WITH_OS
    rsi_task_destroy(NULL);
#endif
  }
}

/*====================================================*/
/**
 * @fn         int main(void)
 * @brief      starting point of application
 * @param[in]  none
 * @return     status
 *=====================================================*/

int main(void)
{
  int32_t status = RSI_SUCCESS;

  //! Driver initialization
  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }
#ifdef RSI_WITH_OS
  //! OS case
  rsi_task_create((rsi_task_function_t)rsi_common_app_task,
                  (uint8_t *)"common_task",
                  RSI_COMMON_TASK_STACK_SIZE,
                  NULL,
                  RSI_COMMON_TASK_PRIORITY,
                  &common_task_handle);

  // rsi_wireless_driver_task is created in rsi_common_app_task for EFM platform
#ifndef EFM32GG11B820F2048GL192
  //! Task created for Driver task
  rsi_task_create((rsi_task_function_t)rsi_wireless_driver_task,
                  (uint8_t *)"driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);
#endif
  //! OS TAsk Start the scheduler
  rsi_start_os_scheduler();
#endif
  return 0;
}

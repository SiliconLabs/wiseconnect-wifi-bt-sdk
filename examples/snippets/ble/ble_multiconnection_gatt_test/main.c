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
 * @date    19 Mar 2021
 *
 *
 *
 *  @brief : This file contains driver, module initialization and application execution
 *
 *  @section Description  This file contains driver, module initializations
 *
 *
 */

/*=======================================================================*/
//  ! INCLUDES
/*=======================================================================*/

#include <stdio.h>
#include <string.h>
#include "rsi_driver.h"
#include "rsi_common_config.h"

/*=======================================================================*/
//   ! MACROS
/*=======================================================================*/
#define RSI_DRIVER_TASK_PRIORITY   4         //! Wireless driver task priority
#define RSI_DRIVER_TASK_STACK_SIZE (512 * 2) //! Wireless driver task stack size
#define RSI_COMMON_TASK_PRIORITY   0         //! application task priority
#define RSI_COMMON_TASK_STACK_SIZE (512 * 2) //! application task size

#define GLOBAL_BUFF_LEN 15000 //! Memory length of driver

/*=======================================================================*/
//! Powersave configurations
/*=======================================================================*/
#define PSP_MODE RSI_SLEEP_MODE_2
#define PSP_TYPE RSI_MAX_PSP

/*=======================================================================*/
//   ! GLOBAL VARIABLES
/*=======================================================================*/
rsi_task_handle_t common_task_handle = NULL;
rsi_task_handle_t driver_task_handle = NULL;
//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN] = { 0 };
//! flag to check bt power save
rsi_parsed_conf_t rsi_parsed_conf = { 0 };
rsi_semaphore_handle_t ble_main_task_sem, ble_slave_conn_sem, ble_scan_sem;
rsi_task_handle_t ble_main_app_task_handle;
bool powersave_cmd_given;

/*=======================================================================*/
//   ! EXTERN VARIABLES
/*=======================================================================*/

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
  status = rsi_wlan_radio_init();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\n radio init failed \n");
  }

  //! initiating power save in BLE mode
  status = rsi_bt_power_save_profile(PSP_MODE, PSP_TYPE);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Failed to initiate power save in BLE mode \r\n");
    return status;
  }

  //! initiating power save in wlan mode
  status = rsi_wlan_power_save_profile(PSP_MODE, PSP_TYPE);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Failed to initiate power save in WLAN mode \r\n");
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
  powersave_cmd_given      = false;
  ble_main_app_task_handle = NULL;

  while (1) {
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
    //! Send Feature frame
    status = rsi_send_feature_frame();
    if (status != RSI_SUCCESS) {
      return;
    }
    //! fill the configurations in local structure based on compilation macros
    status = rsi_fill_user_config();
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\r\n failed to fill the configurations in local buffer \r\n");
      return;
    }

    //! create ble main task if ble protocol is selected
    rsi_semaphore_create(&ble_main_task_sem, 0);
    rsi_semaphore_create(&ble_scan_sem, 0);
    if (RSI_BLE_MAX_NBR_SLAVES > 0) {
      rsi_semaphore_create(&ble_slave_conn_sem, 0);
    }
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

    //! delete the task as initialization is completed
    rsi_task_destroy(NULL);
  }
}

#ifdef RSI_WITH_OS
void rsi_wireless_driver_task_create()
{
  //! Task created for Driver task
  rsi_task_create((rsi_task_function_t)rsi_wireless_driver_task,
                  (uint8_t *)"driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);
}
#endif

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

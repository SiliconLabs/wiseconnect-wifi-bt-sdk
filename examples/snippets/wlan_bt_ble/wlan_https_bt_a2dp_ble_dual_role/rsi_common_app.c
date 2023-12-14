/*******************************************************************************
* @file  rsi_common_app_DEMO_57.c
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
 * @file    rsi_common_app_DEMO_57.c
 * @version 0.1
 * @date    01 Feb 2020
 *
 *
 *  @section Licenseremote_name
 *  This program should be used on your own responsibility.
 *  Silicon Labs assumes no responsibility for any losses
 *  incurred by customers or third parties arising from the use of this file.
 *
 *  @brief : This file contains example application for device initialization
 *
 *  @section Description  This application initiates Silabs  device and create tasks.
 *
 */

/*=======================================================================*/
//   ! INCLUDES
/*=======================================================================*/

#include <rsi_common_app.h>
//#if COEX_MAX_APP
#include "rsi_bt_common_apis.h"
#include <stdio.h>
#include <string.h>
#include <rsi_ble.h>
#include "rsi_driver.h"
#include "rsi_ble_apis.h"
#include "rsi_ble_config.h"
#include "rsi_common_config.h"
#ifdef FW_LOGGING_ENABLE
//! Firmware logging includes
#include "sl_fw_logging.h"
#endif

/*=======================================================================*/
//   ! MACROS
/*=======================================================================*/

/*=======================================================================*/
//   ! GLOBAL VARIABLES
/*=======================================================================*/
//! flag to check bt power save
#if !RUN_TIME_CONFIG_ENABLE
rsi_parsed_conf_t rsi_parsed_conf = { 0 };
#endif
rsi_semaphore_handle_t ble_main_task_sem, ble_peripheral_conn_sem, bt_app_sem, ant_app_sem, wlan_app_sem,
  bt_inquiry_sem, ble_scan_sem;
#if WLAN_SYNC_REQ
rsi_semaphore_handle_t sync_coex_ble_sem, sync_coex_prop_protocol_sem, sync_coex_bt_sem;
bool other_protocol_activity_enabled;
#endif
#if SOCKET_ASYNC_FEATURE
rsi_semaphore_handle_t sock_wait_sem;
#endif
#if (RSI_TCP_IP_BYPASS && SOCKET_ASYNC_FEATURE)
rsi_semaphore_handle_t lwip_sock_async_sem;
#endif
rsi_task_handle_t ble_main_app_task_handle, bt_app_task_handle, prop_protocol_app_task_handle, wlan_app_task_handle,
  wlan_task_handle;
rsi_task_handle_t window_reset_notify_task_handle;
bool rsi_ble_running, rsi_bt_running, rsi_prop_protocol_running, rsi_wlan_running, wlan_radio_initialized,
  powersave_cmd_given;
rsi_mutex_handle_t power_cmd_mutex;
rsi_mutex_handle_t window_update_mutex;
bool rsi_window_update_sem_waiting;
rsi_semaphore_handle_t window_reset_notify_sem;
#if (SSL_TX_DATA || SSL_RX_DATA || (RX_DATA && HTTPS_DOWNLOAD))
rsi_semaphore_handle_t cert_sem, conn_sem;
rsi_task_handle_t cert_bypass_task_handle[SOCKTEST_INSTANCES_MAX];
#endif
/*=======================================================================*/
//   ! EXTERN VARIABLES
/*=======================================================================*/
extern rsi_task_handle_t common_task_handle;
#if RUN_TIME_CONFIG_ENABLE
extern rsi_semaphore_handle_t common_task_sem;
extern rsi_parsed_conf_t rsi_parsed_conf;
#endif

/*========================================================================*/
//!  CALLBACK FUNCTIONS
/*=======================================================================*/

/*=======================================================================*/
//   ! EXTERN FUNCTIONS
/*=======================================================================*/

/*=======================================================================*/
//   ! PROCEDURES
/*=======================================================================*/
int32_t set_power_config(void)
{
  int32_t status = RSI_SUCCESS;

  status = rsi_bt_power_save_profile(RSI_ACTIVE, RSI_MAX_PSP);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Failed to keep in ACTIVE MODE\r\n");
    return status;
  }

  status = rsi_wlan_power_save_profile(RSI_ACTIVE, RSI_MAX_PSP);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\n Failed to keep in ACTIVE MODE\r\n");
    return status;
  }
  return status;
}

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
      LOG_PRINT("\n radio init failed,error = %d\n", status);
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

#if !RUN_TIME_CONFIG_ENABLE
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
    if (RSI_BLE_MAX_NBR_PERIPHERALS > 0) {
      //! Initialize peripheral1 configurations
      ble_conn_spec_conf[PERIPHERAL1].smp_enable        = SMP_ENABLE_P1;
      ble_conn_spec_conf[PERIPHERAL1].add_to_acceptlist = ADD_TO_ACCEPTLIST_P1;
      ble_conn_spec_conf[PERIPHERAL1].profile_discovery = PROFILE_QUERY_P1;
      ble_conn_spec_conf[PERIPHERAL1].data_transfer     = DATA_TRANSFER_P1;
      //ble_conn_spec_conf[PERIPHERAL1].bidir_datatransfer = SMP_ENABLE_P1;
      ble_conn_spec_conf[PERIPHERAL1].rx_notifications                 = RX_NOTIFICATIONS_FROM_P1;
      ble_conn_spec_conf[PERIPHERAL1].rx_indications                   = RX_INDICATIONS_FROM_P1;
      ble_conn_spec_conf[PERIPHERAL1].tx_notifications                 = TX_NOTIFICATIONS_TO_P1;
      ble_conn_spec_conf[PERIPHERAL1].tx_write                         = TX_WRITES_TO_P1;
      ble_conn_spec_conf[PERIPHERAL1].tx_write_no_response             = TX_WRITES_NO_RESP_TO_P1;
      ble_conn_spec_conf[PERIPHERAL1].tx_indications                   = TX_INDICATIONS_TO_P1;
      ble_conn_spec_conf[PERIPHERAL1].conn_param_update.conn_int       = CONN_INTERVAL_P1;
      ble_conn_spec_conf[PERIPHERAL1].conn_param_update.conn_latncy    = CONN_LATENCY_P1;
      ble_conn_spec_conf[PERIPHERAL1].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_P1;
      ble_conn_spec_conf[PERIPHERAL1].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_P1;
      ble_conn_spec_conf[PERIPHERAL1].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_P1;
      ble_conn_spec_conf[PERIPHERAL1].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_P1;
    }

    if (RSI_BLE_MAX_NBR_PERIPHERALS > 1) {
      //! Initialize peripheral2 configurations
      ble_conn_spec_conf[PERIPHERAL2].smp_enable        = SMP_ENABLE_P2;
      ble_conn_spec_conf[PERIPHERAL2].add_to_acceptlist = ADD_TO_ACCEPTLIST_P2;
      ble_conn_spec_conf[PERIPHERAL2].profile_discovery = PROFILE_QUERY_P2;
      ble_conn_spec_conf[PERIPHERAL2].data_transfer     = DATA_TRANSFER_P2;
      //ble_conn_spec_conf[PERIPHERAL2].bidir_datatransfer = SMP_ENABLE_P2;
      ble_conn_spec_conf[PERIPHERAL2].rx_notifications                 = RX_NOTIFICATIONS_FROM_P2;
      ble_conn_spec_conf[PERIPHERAL2].rx_indications                   = RX_INDICATIONS_FROM_P2;
      ble_conn_spec_conf[PERIPHERAL2].tx_notifications                 = TX_NOTIFICATIONS_TO_P2;
      ble_conn_spec_conf[PERIPHERAL2].tx_write                         = TX_WRITES_TO_P2;
      ble_conn_spec_conf[PERIPHERAL2].tx_write_no_response             = TX_WRITES_NO_RESP_TO_P2;
      ble_conn_spec_conf[PERIPHERAL2].tx_indications                   = TX_INDICATIONS_TO_P2;
      ble_conn_spec_conf[PERIPHERAL2].conn_param_update.conn_int       = CONN_INTERVAL_P2;
      ble_conn_spec_conf[PERIPHERAL2].conn_param_update.conn_latncy    = CONN_LATENCY_P2;
      ble_conn_spec_conf[PERIPHERAL2].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_P2;
      ble_conn_spec_conf[PERIPHERAL2].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_P2;
      ble_conn_spec_conf[PERIPHERAL2].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_P2;
      ble_conn_spec_conf[PERIPHERAL2].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_P2;
    }

    if (RSI_BLE_MAX_NBR_PERIPHERALS > 2) {
      //! Initialize PERIPHERAL3 configurations
      ble_conn_spec_conf[PERIPHERAL3].smp_enable        = SMP_ENABLE_P3;
      ble_conn_spec_conf[PERIPHERAL3].add_to_acceptlist = ADD_TO_ACCEPTLIST_P3;
      ble_conn_spec_conf[PERIPHERAL3].profile_discovery = PROFILE_QUERY_P3;
      ble_conn_spec_conf[PERIPHERAL3].data_transfer     = DATA_TRANSFER_P3;
      //ble_conn_spec_conf[PERIPHERAL3].bidir_datatransfer = SMP_ENABLE_P3;
      ble_conn_spec_conf[PERIPHERAL3].rx_notifications                 = RX_NOTIFICATIONS_FROM_P3;
      ble_conn_spec_conf[PERIPHERAL3].rx_indications                   = RX_INDICATIONS_FROM_P3;
      ble_conn_spec_conf[PERIPHERAL3].tx_notifications                 = TX_NOTIFICATIONS_TO_P3;
      ble_conn_spec_conf[PERIPHERAL3].tx_write                         = TX_WRITES_TO_P3;
      ble_conn_spec_conf[PERIPHERAL3].tx_write_no_response             = TX_WRITES_NO_RESP_TO_P3;
      ble_conn_spec_conf[PERIPHERAL3].tx_indications                   = TX_INDICATIONS_TO_P3;
      ble_conn_spec_conf[PERIPHERAL3].conn_param_update.conn_int       = CONN_INTERVAL_P3;
      ble_conn_spec_conf[PERIPHERAL3].conn_param_update.conn_latncy    = CONN_LATENCY_P3;
      ble_conn_spec_conf[PERIPHERAL3].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_P3;
      ble_conn_spec_conf[PERIPHERAL3].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_P3;
      ble_conn_spec_conf[PERIPHERAL3].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_P3;
      ble_conn_spec_conf[PERIPHERAL3].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_P3;
    }

    if (RSI_BLE_MAX_NBR_CENTRALS > 0) {
      //! Initialize central1 configurations
      ble_conn_spec_conf[CENTRAL1].smp_enable        = SMP_ENABLE_C1;
      ble_conn_spec_conf[CENTRAL1].add_to_acceptlist = ADD_TO_ACCEPTLIST_C1;
      ble_conn_spec_conf[CENTRAL1].profile_discovery = PROFILE_QUERY_C1;
      ble_conn_spec_conf[CENTRAL1].data_transfer     = DATA_TRANSFER_C1;
      //ble_conn_spec_conf[CENTRAL1].bidir_datatransfer = SMP_ENABLE_C1;
      ble_conn_spec_conf[CENTRAL1].rx_notifications                 = RX_NOTIFICATIONS_FROM_C1;
      ble_conn_spec_conf[CENTRAL1].rx_indications                   = RX_INDICATIONS_FROM_C1;
      ble_conn_spec_conf[CENTRAL1].tx_notifications                 = TX_NOTIFICATIONS_TO_C1;
      ble_conn_spec_conf[CENTRAL1].tx_write                         = TX_WRITES_TO_C1;
      ble_conn_spec_conf[CENTRAL1].tx_write_no_response             = TX_WRITES_NO_RESP_TO_C1;
      ble_conn_spec_conf[CENTRAL1].tx_indications                   = TX_INDICATIONS_TO_C1;
      ble_conn_spec_conf[CENTRAL1].conn_param_update.conn_int       = CONN_INTERVAL_C1;
      ble_conn_spec_conf[CENTRAL1].conn_param_update.conn_latncy    = CONN_LATENCY_C1;
      ble_conn_spec_conf[CENTRAL1].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_C1;
      ble_conn_spec_conf[CENTRAL1].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_C1;
      ble_conn_spec_conf[CENTRAL1].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_C1;
      ble_conn_spec_conf[CENTRAL1].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_C1;
    }

    if (RSI_BLE_MAX_NBR_CENTRALS > 1) {
      //! Initialize central2 configurations
      ble_conn_spec_conf[CENTRAL2].smp_enable        = SMP_ENABLE_C2;
      ble_conn_spec_conf[CENTRAL2].add_to_acceptlist = ADD_TO_ACCEPTLIST_C2;
      ble_conn_spec_conf[CENTRAL2].profile_discovery = PROFILE_QUERY_C2;
      ble_conn_spec_conf[CENTRAL2].data_transfer     = DATA_TRANSFER_C2;
      //ble_conn_spec_conf[CENTRAL2].bidir_datatransfer = SMP_ENABLE_C2;
      ble_conn_spec_conf[CENTRAL2].rx_notifications                 = RX_NOTIFICATIONS_FROM_C2;
      ble_conn_spec_conf[CENTRAL2].rx_indications                   = RX_INDICATIONS_FROM_C2;
      ble_conn_spec_conf[CENTRAL2].tx_notifications                 = TX_NOTIFICATIONS_TO_C2;
      ble_conn_spec_conf[CENTRAL2].tx_write                         = TX_WRITES_TO_C2;
      ble_conn_spec_conf[CENTRAL2].tx_write_no_response             = TX_WRITES_NO_RESP_TO_C2;
      ble_conn_spec_conf[CENTRAL2].tx_indications                   = TX_INDICATIONS_TO_C2;
      ble_conn_spec_conf[CENTRAL2].conn_param_update.conn_int       = CONN_INTERVAL_C2;
      ble_conn_spec_conf[CENTRAL2].conn_param_update.conn_latncy    = CONN_LATENCY_C2;
      ble_conn_spec_conf[CENTRAL2].conn_param_update.supervision_to = CONN_SUPERVISION_TIMEOUT_C2;
      ble_conn_spec_conf[CENTRAL2].buff_mode_sel.buffer_mode        = DLE_BUFFER_MODE_C2;
      ble_conn_spec_conf[CENTRAL2].buff_mode_sel.buffer_cnt         = DLE_BUFFER_COUNT_C2;
      ble_conn_spec_conf[CENTRAL2].buff_mode_sel.max_data_length    = RSI_BLE_MAX_DATA_LEN_C2;
    }

    /* Check the Total Number of Buffers allocated.*/
    if ((DLE_BUFFER_COUNT_P1 + DLE_BUFFER_COUNT_P2 + DLE_BUFFER_COUNT_P3 + DLE_BUFFER_COUNT_C1 + DLE_BUFFER_COUNT_C2)
        > RSI_BLE_NUM_CONN_EVENTS) {
      LOG_PRINT("\r\n Total number of per connection buffer count is more than the total number alllocated \r\n");
      status = RSI_FAILURE;
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
  rsi_parsed_conf.rsi_protocol_sel.is_ble_enabled           = RSI_ENABLE_BLE_TEST;
  rsi_parsed_conf.rsi_protocol_sel.is_bt_enabled            = RSI_ENABLE_BT_TEST;
  rsi_parsed_conf.rsi_protocol_sel.is_prop_protocol_enabled = RSI_ENABLE_PROP_PROTOCOL_TEST;
  rsi_parsed_conf.rsi_protocol_sel.is_wifi_enabled          = RSI_ENABLE_WIFI_TEST;

  //! copy ble connection specific configurations

  if ((RSI_BLE_MAX_NBR_CENTRALS > 2) || (RSI_BLE_MAX_NBR_PERIPHERALS > 3)) {
    LOG_PRINT("\r\n number of BLE CENTRALS or BLE PERIPHERALS Given wrong declaration\r\n");
    rsi_delay_ms(1000);
    return RSI_FAILURE;
  }

  if (rsi_parsed_conf.rsi_protocol_sel.is_ble_enabled) {
    status =
      rsi_ble_initialize_conn_buffer((rsi_ble_conn_config_t *)&rsi_parsed_conf.rsi_ble_config.rsi_ble_conn_config);
  }

  //! copy bt configurations
  if (rsi_parsed_conf.rsi_protocol_sel.is_bt_enabled) {
    rsi_parsed_conf.rsi_bt_config.rsi_bd_addr               = malloc(sizeof(uint8_t) * RSI_REM_DEV_ADDR_LEN);
    rsi_parsed_conf.rsi_bt_config.rsi_bd_addr               = RSI_BT_REMOTE_BD_ADDR;
    rsi_parsed_conf.rsi_bt_config.rsi_app_avdtp_role        = RSI_APP_AVDTP_ROLE;
    rsi_parsed_conf.rsi_bt_config.rsi_bt_avdtp_stats_enable = RSI_BT_AVDTP_STATS;
    rsi_parsed_conf.rsi_bt_config.rsi_ta_based_encoder      = TA_BASED_ENCODER;
    rsi_parsed_conf.rsi_bt_config.rsi_bt_inquiry_enable     = INQUIRY_ENABLE;
    rsi_parsed_conf.rsi_bt_config.rsi_inq_conn_simultaneous = INQUIRY_CONNECTION_SIMULTANEOUS;
  }
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
#if (PER_TEST_TX_ENABLE || PER_TEST_RX_ENABLE)
//! Variable used to stop receiving stats
volatile uint8_t stop_receiving_stats = 0;
//! Variable used to initialise stats received
uint32_t stats_received = 0;

//! wlan per stats structure
typedef struct rsi_per_stats_rsp_s {
  uint8_t tx_pkts[2];
  uint8_t reserved_1[2];
  uint8_t tx_retries[2];
  uint16_t crc_pass;
  uint16_t crc_fail;
  uint8_t cca_stk[2];
  uint8_t cca_not_stk[2];
  uint8_t pkt_abort[2];
  uint8_t fls_rx_start[2];
  uint8_t cca_idle[2];
  uint8_t reserved_2[26];
  uint8_t rx_retries[2];
  uint8_t reserved_3[2];
  uint16_t cal_rssi;
  uint8_t reserved_4[4];
  uint8_t xretries[2];
  uint8_t max_cons_pkts_dropped[2];
  uint8_t reserved_5[2];
  uint8_t bss_broadcast_pkts[2];
  uint8_t bss_multicast_pkts[2];
  uint8_t bss_filter_matched_multicast_pkts[2];
} rsi_per_stats_rsp_t;

//! call back to receive RX Stats data
void rsi_wlan_stats_receive_handler(uint16_t status, const uint8_t *buffer, const uint16_t length)
{
  rsi_per_stats_rsp_t *per_stats_p = (rsi_per_stats_rsp_t *)buffer;
  stats_received++;
  LOG_PRINT("\n Stats received \n");
}
#endif

void rsi_common_app_task(void)
{
  int8_t status                 = RSI_SUCCESS;
  uint8_t fmversion[20]         = { 0 };
  rsi_ble_running               = false;
  rsi_bt_running                = false;
  rsi_prop_protocol_running     = false;
  rsi_wlan_running              = false;
  wlan_radio_initialized        = false;
  powersave_cmd_given           = false;
  ble_main_app_task_handle      = NULL;
  bt_app_task_handle            = NULL;
  prop_protocol_app_task_handle = NULL;
  wlan_app_task_handle          = NULL;
  rsi_window_update_sem_waiting = false;
  uint8_t own_bd_addr[6]        = { 0x66, 0x55, 0x44, 0x33, 0x22, 0x11 };

  while (1) {
#if RUN_TIME_CONFIG_ENABLE
    //! wait untill completion of parsing input file
    rsi_semaphore_wait(&common_task_sem, 0);
#endif

    //!  Silabs module initialization
    status = rsi_device_init(LOAD_NWP_FW);
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\r\n device init failed \n");
      return;
    }
#if !RUN_TIME_CONFIG_ENABLE
    //! fill the configurations in local structure based on compilation macros
    status = rsi_fill_user_config();
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\r\n failed to fill the configurations in local buffer \r\n");
      return;
    }
#endif
    //! WiSeConnect initialization
#if (PER_TEST_TX_ENABLE || PER_TEST_RX_ENABLE)
    status = rsi_wireless_init(RSI_WLAN_TRANSMIT_TEST_MODE, RSI_WLAN_CLIENT_MODE);
#else
    status = rsi_wireless_init(RSI_WLAN_MODE, RSI_COEX_MODE);
#endif
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
      LOG_PRINT("\r\nFirmware version Failed, Error Code : 0x%lX\r\n", status);
    } else {
      LOG_PRINT("\nfirmware_version = %s", fmversion);
    }
#endif
#if !(PER_TEST_TX_ENABLE || PER_TEST_RX_ENABLE)
    status = rsi_bt_set_bd_addr(own_bd_addr);
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\nSET BD ADDR FAILED : 0x%x\n", status);
    } else {
      LOG_PRINT("\nSET BD ADDR SUCCESS : 0x%x\n", status);
    }
#endif
    //! Send Feature frame
    status = rsi_send_feature_frame();
    if (status != RSI_SUCCESS) {
      return;
    }
#if CONFIGURE_TIMEOUT
    status = rsi_config_timeout(KEEP_ALIVE_TIMEOUT, 60);
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\n Timeout Configuration Failed : 0x%x \n", status);
      return status;
    } else {
      LOG_PRINT("\nTimeout Configuration SUCCESS");
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

#if (PER_TEST_TX_ENABLE || PER_TEST_RX_ENABLE)
    //! To selct Internal antenna or uFL connector
    status = rsi_wireless_antenna(RSI_ANTENNA, RSI_ANTENNA_GAIN_2G, RSI_ANTENNA_GAIN_5G);
    if (status != RSI_SUCCESS) {
      return;
    }
#endif

#if PER_TEST_TX_ENABLE
    //! transmit test start
    status = rsi_transmit_test_start(RSI_TX_TEST_POWER,
                                     RSI_TX_TEST_RATE,
                                     RSI_TX_TEST_LENGTH,
                                     RSI_TX_TEST_MODE,
                                     RSI_TX_TEST_CHANNEL);
    if (status != RSI_SUCCESS) {
      return;
    } else {
      LOG_PRINT("\nPER TRANSMIT START SUCCESS");
    }
    rsi_delay_ms(PER_TX_TIMEOUT);
    //! Stop TX transmit
    status = rsi_transmit_test_stop();
    if (status != RSI_SUCCESS) {
      return;
    }
    while (1)
      ;
#endif

#if PER_TEST_RX_ENABLE
    //! Register Wlan receive stats call back handler
    rsi_wlan_register_callbacks(RSI_WLAN_RECEIVE_STATS_RESPONSE_CB, rsi_wlan_stats_receive_handler);
    //! Start/Receive publishing RX stats
    status = rsi_wlan_receive_stats_start(RSI_TX_TEST_CHANNEL);
    if (status != RSI_SUCCESS) {
      return;
    } else {
      LOG_PRINT("\nPER RECEIVE START SUCCESS");
    }
    rsi_delay_ms(PER_RX_TIMEOUT);
    //! Stop Receiving RX stats
    status = rsi_wlan_receive_stats_stop();
    if (status != RSI_SUCCESS) {
      return;
    }
    while (1)
      ;
#endif

#if ENABLE_POWER_SAVE
    //! create mutex
    status = rsi_mutex_create(&power_cmd_mutex);
    if (status != RSI_ERROR_NONE) {
      LOG_PRINT("failed to create mutex object, error = %d \r\n", status);
      return;
    }
#endif

    if (rsi_parsed_conf.rsi_protocol_sel.is_wifi_enabled) {
      rsi_wlan_running = true;                //! Making sure wlan got triggered.
      rsi_semaphore_create(&wlan_app_sem, 0); //! This lock will be used from one download complete notification.
#if (SSL_TX_DATA || SSL_RX_DATA || (RX_DATA && HTTPS_DOWNLOAD))
      rsi_semaphore_create(&cert_sem, 0);
      rsi_semaphore_create(&conn_sem, 0);
      rsi_semaphore_post(&conn_sem);
#endif
#if WLAN_SYNC_REQ
      rsi_semaphore_create(&sync_coex_bt_sem, 0); //! This lock will be used from wlan task to be done.
      rsi_semaphore_create(&sync_coex_prop_protocol_sem, 0);
      rsi_semaphore_create(&sync_coex_ble_sem, 0);
#endif
#if SOCKET_ASYNC_FEATURE
      rsi_semaphore_create(&sock_wait_sem, 0);
#endif
#if (RSI_TCP_IP_BYPASS && SOCKET_ASYNC_FEATURE)
      rsi_semaphore_create(&lwip_sock_async_sem, 0);
#endif
#if WLAN_STA_TX_CASE
      status = rsi_task_create((void *)rsi_app_task_wifi_tcp_tx_ps,
                               (uint8_t *)"wlan_task",
                               RSI_WLAN_TASK_STACK_SIZE,
                               NULL,
                               RSI_WLAN_APP_TASK_PRIORITY,
                               &wlan_task_handle);
      if (status != RSI_ERROR_NONE) {
        LOG_PRINT("\r\n rrsi_app_task_wifi_tcp_tx_ps failed to create \r\n");
        break;
      }
#else
      status = rsi_task_create((void *)rsi_wlan_app_task,
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
    }
    //! create ble main task if ble protocol is selected
    if (rsi_parsed_conf.rsi_protocol_sel.is_ble_enabled) {
      rsi_ble_running = 1;
      rsi_semaphore_create(&ble_main_task_sem, 0);
      rsi_semaphore_create(&ble_scan_sem, 0);
      if (RSI_BLE_MAX_NBR_PERIPHERALS > 0) {
        rsi_semaphore_create(&ble_peripheral_conn_sem, 0);
      }
      status = rsi_task_create((void *)rsi_ble_main_app_task,
                               (uint8_t *)"ble_main_task",
                               RSI_BLE_APP_MAIN_TASK_SIZE,
                               NULL,
                               RSI_BLE_MAIN_TASK_PRIORITY,
                               &ble_main_app_task_handle);
      if (status != RSI_ERROR_NONE) {
        LOG_PRINT("\r\n ble main task failed to create \r\n");
        return;
      }
    }

    //! create bt task if bt protocol is selected
    if (rsi_parsed_conf.rsi_protocol_sel.is_bt_enabled) {
      rsi_bt_running = 1;
      rsi_semaphore_create(&bt_app_sem, 0);
      rsi_semaphore_create(&bt_inquiry_sem, 0);
      status = rsi_task_create((void *)rsi_bt_app_task,
                               (uint8_t *)"bt_task",
                               RSI_BT_APP_TASK_SIZE + RSI_SBC_APP_ENCODE_SIZE,
                               NULL,
                               RSI_BT_APP_TASK_PRIORITY,
                               &bt_app_task_handle);
      if (status != RSI_ERROR_NONE) {
        LOG_PRINT("\r\n rsi_bt_app_task failed to create \r\n");
        return;
      }
    }

    //! create prop_protocol task if prop_protocol protocol is selected
    /*if (rsi_parsed_conf.rsi_protocol_sel.is_prop_protocol_enabled) {
      rsi_prop_protocol_running = 1;
      rsi_semaphore_create(&prop_protocol_app_sem, 0);
      status = rsi_task_create((void *)rsi_prop_protocol_app_task,
                               (uint8_t *)"prop_protocol_task",
                               RSI_PROP_PROTOCOL_APP_TASK_SIZE,
                               NULL,
                               RSI_PROP_PROTOCOL_APP_TASK_PRIORITY,
                               &prop_protocol_app_task_handle);
      if (status != RSI_ERROR_NONE) {
        LOG_PRINT("\r\n rsi_prop_protocol_app_task failed to create \r\n");
        return;
      }
    }*/

    //! delete the task as initialization is completed
    rsi_task_destroy(common_task_handle);
  }
}
//#endif

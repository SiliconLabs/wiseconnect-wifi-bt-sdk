/*******************************************************************************
* @file  ae_peripheral.c
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
 * @brief : This file contains example application for BLE AE Peripheral
 * @section Description :
 * This application demonstrates how to configure the device in AE peripheral
 * mode and how to get connected from the remote Central device.
 =================================================================================*/

/**
 * Include files
 * */
//! BLE include file to refer BLE APIs
#include <rsi_ble_apis.h>
#include <rsi_ble_config.h>
#include <rsi_bt_common_apis.h>
#include <rsi_bt_common.h>
#include <rsi_ble.h>
#include "rsi_driver.h"
//! Common include file
#include <rsi_common_apis.h>

#include <string.h>

#define RSI_BLE_LOCAL_NAME  "AE_PERIPHERAL"
#define BLE_AE_ADV_DATA     "AE_PERIPHERAL_DATA_1"
#define BLE_AE_ADV_DATA_LEN 0x19
#define RSI_SEL_ANTENNA     RSI_SEL_INTERNAL_ANTENNA //RSI_SEL_EXTERNAL_ANTENNA
#define RSI_SCAN_RESP_DATA  "REDPINE_AE_DEVICE"
#define ADV_SET2            1

#define REM_DEV_ADDR "00:00:00:00:00:00"

#define PERIODIC_ADV_EN 0
#define BLE_ADV_HNDL1   0x00

//! Address type of the device to connect
#define RSI_BLE_WHITELIST_DEV_ADDR1_TYPE LE_RANDOM_ADDRESS

//! Address of the device to add in white list
uint8_t ble_whitelist_addr1[6] = { 0xDA, 0x81, 0xE0, 0xB8, 0xB9, 0x42 };

//! application events list
#define RSI_APP_EVENT_CONNECTED          1
#define RSI_APP_EVENT_DISCONNECTED       2
#define RSI_APP_EVENT_EXT_ADV_TERMINATED 3
#define RSI_APP_EVENT_SCAN_REQ_RECEIVED  4

/*=======================================================================*/
//!	Powersave configurations
/*=======================================================================*/
#define ENABLE_POWER_SAVE 1 //! Set to 1 for powersave mode
#if ENABLE_POWER_SAVE
//! Power Save Profile Mode
#define PSP_MODE RSI_SLEEP_MODE_2
//! Power Save Profile type
#define PSP_TYPE RSI_MAX_PSP
#endif

//! Memory length for the driver
#define BLE_GLOBAL_BUFF_LEN 15000

//! Memory to initialize the driver
uint8_t global_buf[BLE_GLOBAL_BUFF_LEN];

#ifdef RSI_WITH_OS
//! BLE task stack size
#define RSI_BLE_TASK_STACK_SIZE 1000

//! BT task priority
#define RSI_BLE_TASK_PRIORITY 1

//! Number of packet to send or receive
#define NUMBER_OF_PACKETS 1000

//! Wireless driver task priority
#define RSI_DRIVER_TASK_PRIORITY 2

//! Wireless driver task stack size
#define RSI_DRIVER_TASK_STACK_SIZE 3000

void rsi_wireless_driver_task(void);

#endif

//! Application global parameters.
static rsi_bt_resp_get_local_name_t rsi_app_resp_get_local_name = { 0 };
static uint8_t rsi_app_resp_get_dev_addr[RSI_DEV_ADDR_LEN]      = { 0 };
static uint8_t rsi_app_async_event_map                          = 0;
static rsi_ble_event_conn_status_t rsi_app_connected_device     = { 0 };
static rsi_ble_event_disconnect_t rsi_app_disconnected_device   = { 0 };
static rsi_ble_adv_set_terminated_t adv_set_terminated          = { 0 };
static rsi_ble_scan_req_recvd_t scan_req_recvd                  = { 0 };

int8_t rsi_app_resp_tx_power                  = 0;
uint8_t rsi_app_resp_max_no_of_supp_adv_sets  = 0;
static uint16_t rsi_app_resp_max_adv_data_len = 0;

/*==============================================*/
/**
 * @fn         rsi_ble_app_init_events
 * @brief      initializes the event parameter.
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function is used during BLE initialization.
 */
static void rsi_ble_app_init_events()
{
  rsi_app_async_event_map = 0;
  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_set_event
 * @brief      sets the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
static void rsi_ble_app_set_event(uint32_t event_num)
{
  rsi_app_async_event_map |= BIT(event_num);
  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_clear_event
 * @brief      clears the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to clear the specific event.
 */
static void rsi_ble_app_clear_event(uint32_t event_num)
{
  rsi_app_async_event_map &= ~BIT(event_num);
  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_get_event
 * @brief      returns the first set event based on priority
 * @param[in]  none.
 * @return     int32_t
 *             > 0  = event number
 *             -1   = not received any event
 * @section description
 * This function returns the highest priority event among all the set events
 */
static int32_t rsi_ble_app_get_event(void)
{
  uint32_t ix;

  for (ix = 0; ix < 32; ix++) {
    if (rsi_app_async_event_map & (1 << ix)) {
      return ix;
    }
  }

  return (RSI_FAILURE);
}

/*==============================================*/
/**
 * @fn         rsi_ble_simple_peripheral_on_conn_status_event
 * @brief      invoked when connection complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
void rsi_ble_simple_peripheral_on_conn_status_event(rsi_ble_event_conn_status_t *resp_conn)
{
  memcpy(&rsi_app_connected_device, resp_conn, sizeof(rsi_ble_event_conn_status_t));
  rsi_ble_app_set_event(RSI_APP_EVENT_CONNECTED);
}

/*==============================================*/
/**
 * @fn         rsi_ble_simple_peripheral_on_disconnect_event
 * @brief      invoked when disconnection event is received
 * @param[in]  resp_disconnect, disconnected remote device information
 * @param[in]  reason, reason for disconnection.
 * @return     none.
 * @section description
 * This callback function indicates disconnected device information and status
 */
void rsi_ble_simple_peripheral_on_disconnect_event(rsi_ble_event_disconnect_t *resp_disconnect, uint16_t reason)
{
  LOG_PRINT("\n Disconnect callback received with status 0x%X \n", reason);
  memcpy(&rsi_app_disconnected_device, resp_disconnect, sizeof(rsi_ble_event_conn_status_t));
  rsi_ble_app_set_event(RSI_APP_EVENT_DISCONNECTED);
}

/*==============================================*/
/**
 * @fn         rsi_ble_simple_peripheral_on_enhance_conn_status_event
 * @brief      invoked when enhanced connection complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
void rsi_ble_simple_peripheral_on_enhance_conn_status_event(rsi_ble_event_enhance_conn_status_t *resp_enh_conn)
{
  rsi_app_connected_device.dev_addr_type = resp_enh_conn->dev_addr_type;
  memcpy(rsi_app_connected_device.dev_addr, resp_enh_conn->dev_addr, RSI_DEV_ADDR_LEN);
  rsi_app_connected_device.status = resp_enh_conn->status;
  rsi_ble_app_set_event(RSI_APP_EVENT_CONNECTED);
}

/*==============================================*/
/**
 * @fn         rsi_ble_simple_peripheral_on_enhance_conn_status_event
 * @brief      invoked when enhanced connection complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
void ble_extended_advertise_terminated_callback(uint16_t status, rsi_ble_adv_set_terminated_t *resp_terminated)
{
  LOG_PRINT("\n Terminated callback received with status 0x%X \n", status);
  adv_set_terminated.adv_handle              = resp_terminated->adv_handle;
  adv_set_terminated.status                  = resp_terminated->status;
  adv_set_terminated.num_completed_ae_events = resp_terminated->num_completed_ae_events;
  adv_set_terminated.conn_handle             = resp_terminated->conn_handle;
  rsi_ble_app_set_event(RSI_APP_EVENT_EXT_ADV_TERMINATED);
}

/*==============================================*/
/**
 * @fn         rsi_ble_simple_peripheral_on_enhance_conn_status_event
 * @brief      invoked when enhanced connection complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
void ble_extended_scan_request_received_callback(uint16_t status, rsi_ble_scan_req_recvd_t *resp_scan_received)
{
  LOG_PRINT("\n Scan request callback received with status 0x%X \n", status);
  scan_req_recvd.adv_handle        = resp_scan_received->adv_handle;
  scan_req_recvd.scanner_addr_type = resp_scan_received->scanner_addr_type;
  memcpy(scan_req_recvd.scanner_addr, resp_scan_received->scanner_addr, RSI_DEV_ADDR_LEN);
  rsi_ble_app_set_event(RSI_APP_EVENT_SCAN_REQ_RECEIVED);
}

/*==============================================*/
/**
 * @fn         rsi_ble_peripheral
 * @brief      Tests the BLE GAP peripheral role.
 * @param[in]  none
 * @return    none.
 * @section description
 * This function is used to test the BLE peripheral role and simple GAP API's.
 */
int32_t rsi_ble_peripheral(void)
{
  int32_t status                      = 0;
  int32_t temp_event_map              = 0;
  uint8_t remote_dev_addr[18]         = { 0 };
  uint8_t rand_addr[RSI_DEV_ADDR_LEN] = { 0 };
  rsi_ascii_dev_address_to_6bytes_rev((uint8_t *)rand_addr, (int8_t *)RSI_BLE_SET_RAND_ADDR);
  uint8_t data[BLE_AE_ADV_DATA_LEN] = { 0 };
  strncpy((char *)data, BLE_AE_ADV_DATA, BLE_AE_ADV_DATA_LEN);

#ifdef RSI_WITH_OS
  rsi_task_handle_t driver_task_handle = NULL;
#endif
#ifdef FW_LOGGING_ENABLE
  //Fw log component level
  sl_fw_log_level_t fw_component_log_level;
#endif
#ifndef RSI_WITH_OS
  //! Driver initialization
  status = rsi_driver_init(global_buf, BLE_GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > BLE_GLOBAL_BUFF_LEN)) {
    return status;
  }

  //! SiLabs module intialisation
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nDevice Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nDevice Initialization Success\r\n");
  }
#endif
#ifdef RSI_WITH_OS
#ifndef RSI_M4_INTERFACE
  //! SiLabs module initialization
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nDevice Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nDevice Initialization Success\r\n");
  }
#endif
  //! Task created for Driver task
  rsi_task_create((rsi_task_function_t)rsi_wireless_driver_task,
                  (uint8_t *)"driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);
#endif
  //! WC initialization
  status = rsi_wireless_init(0, RSI_OPERMODE_WLAN_BLE);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWireless Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nWireless Initialization Success\r\n");
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
  //! BLE register GAP callbacks
  rsi_ble_gap_register_callbacks(NULL,
                                 rsi_ble_simple_peripheral_on_conn_status_event,
                                 rsi_ble_simple_peripheral_on_disconnect_event,
                                 NULL,
                                 NULL,
                                 NULL,
                                 rsi_ble_simple_peripheral_on_enhance_conn_status_event,
                                 NULL,
                                 NULL,
                                 NULL);

  status = rsi_ble_adv_ext_events_register_callbacks(RSI_BLE_ON_ADV_EXT_ADVERTISE_SET_TERMINATED_EVENT,
                                                     (void *)ble_extended_advertise_terminated_callback);

  if (status != RSI_SUCCESS) {
    LOG_PRINT("\n RSI_BLE_ON_ADV_EXT_ADVERTISE_SET_TERMINATED_EVENT callback registering failed \n");
  } else {
    LOG_PRINT("\n RSI_BLE_ON_ADV_EXT_ADVERTISE_SET_TERMINATED_EVENT callback registering success \n");
  }
  status = rsi_ble_adv_ext_events_register_callbacks(RSI_BLE_ON_ADV_EXT_SCAN_REQUEST_RECEIVED_EVENT,
                                                     (void *)ble_extended_scan_request_received_callback);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\n RSI_BLE_ON_ADV_EXT_SCAN_REQUEST_RECEIVED_EVENT callback registering failed \n");
  } else {
    LOG_PRINT("\n RSI_BLE_ON_ADV_EXT_SCAN_REQUEST_RECEIVED_EVENT callback registering success \n");
  }
  //! initialize the event map
  rsi_ble_app_init_events();

  status = rsi_ble_addto_whitelist((int8_t *)ble_whitelist_addr1, RSI_BLE_WHITELIST_DEV_ADDR1_TYPE);
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! get the local device address(MAC address).
  status = rsi_bt_get_local_device_address(rsi_app_resp_get_dev_addr);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\n rsi_bt_get_local_device_address failed with 0x%lX \n", status);
  }

  //! set the local device name
  status = rsi_bt_set_local_name((uint8_t *)RSI_BLE_LOCAL_NAME);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\n rsi_bt_set_local_name failed with 0x%lX \n", status);
  }

  //! get the local device name
  status = rsi_bt_get_local_name(&rsi_app_resp_get_local_name);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\n rsi_bt_get_local_name failed with 0x%lX \n", status);
  } else {
    LOG_PRINT(" \n get_local_name done \n");
  }

  //! get the supported max adv data len
  status = rsi_ble_get_max_adv_data_len((uint8_t *)&rsi_app_resp_max_adv_data_len);

  if (status != RSI_SUCCESS) {
    LOG_PRINT("get max adv data length failed with 0x%lX \n", status);
    return status;
  } else {
    LOG_PRINT("Max supported Adv Data length is %d \n", (uint8_t)rsi_app_resp_max_adv_data_len);
  }

  //! get the Max no.of supported adv sets
  status = rsi_ble_get_max_no_of_supp_adv_sets(&rsi_app_resp_max_no_of_supp_adv_sets);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("get max supported adv sets failed with 0x%lX\n", status);
    return status;
  } else {
    LOG_PRINT("Max number of supported Adv sets are %d  \n", rsi_app_resp_max_no_of_supp_adv_sets);
  }

  //! set ae adv params
  rsi_ble_ae_adv_params_t ble_ae_params = { 0 };
  uint8_t peer_addr[RSI_DEV_ADDR_LEN]   = { 0 };

  ble_ae_params.adv_handle                = BLE_ADV_HNDL1;
  ble_ae_params.adv_event_prop            = BLE_AE_ADV_EVNT_PROP;
  ble_ae_params.primary_adv_intterval_min = RSI_BLE_ADV_INT_MIN; //140 only 3bytes are valid;
  ble_ae_params.primary_adv_intterval_max = RSI_BLE_ADV_INT_MAX; //160 only 3bytes are valid;
  ble_ae_params.primary_adv_chnl_map      = RSI_BLE_ADV_CHANNEL_MAP;
  ble_ae_params.own_addr_type             = LE_RANDOM_ADDRESS;
  ble_ae_params.peer_addr_type            = LE_RANDOM_ADDRESS;
  rsi_ascii_dev_address_to_6bytes_rev((uint8_t *)peer_addr, (int8_t *)REM_DEV_ADDR);
  memcpy(ble_ae_params.peer_dev_addr, peer_addr, RSI_DEV_ADDR_LEN);
  ble_ae_params.adv_filter_policy      = BLE_AE_ADV_FILTER_POLICY;
  ble_ae_params.adv_tx_power           = BLE_AE_ADV_TX_PWR;
  ble_ae_params.primary_adv_phy        = PRIMARY_ADV_PHY;
  ble_ae_params.sec_adv_max_skip       = BLE_AE_SEC_ADV_MAX_SKIP;
  ble_ae_params.sec_adv_phy            = SECONDARY_ADV_PHY;
  ble_ae_params.adv_sid                = BLE_AE_ADV_SID;
  ble_ae_params.scan_req_notify_enable = SCAN_REQ_NOTIF_EN;

  LOG_PRINT("\nBLE_AE_ADV_EVNT_PROP = %x \n", BLE_AE_ADV_EVNT_PROP);

  status = rsi_ble_set_ae_params(&ble_ae_params, &rsi_app_resp_tx_power);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("set ae params failed with 0x%lX \n", status);
    return status;
  } else {
    LOG_PRINT("Setting AE params successful and selected TX Power is %d dbm \n", rsi_app_resp_tx_power);
  }

#if PERIODIC_ADV_EN
  rsi_ble_ae_periodic_adv_params_t ae_periodic_param = { 0 };
  ae_periodic_param.max_interval                     = 0x190;
  ae_periodic_param.min_interval                     = 0x90;
  ae_periodic_param.properties                       = 0x40;
  ae_periodic_param.adv_handle                       = BLE_ADV_HNDL1;
  status                                             = rsi_ble_app_set_periodic_ae_params(&ae_periodic_param);

  if (status != RSI_SUCCESS) {
    LOG_PRINT("set ae Periodic adv data failed with 0x%lX\n", status);
    return status;
  } else {
    LOG_PRINT("set ae periodic adv data success \n");
  }

  status = rsi_ble_app_set_periodic_ae_enable(1, 0);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("set ae Periodic adv enable failed with 0x%lX\n", status);
    return status;
  } else {
    LOG_PRINT("set ae periodic adv enable success \n");
  }
#endif

#if ADV_SET2
  ble_ae_params.adv_handle                = 0x01;
  ble_ae_params.adv_event_prop            = 0x01; //Connectable;
  ble_ae_params.primary_adv_intterval_min = 0x30; //140 only 3bytes are valid;
  ble_ae_params.primary_adv_intterval_max = 0x30; //160 only 3bytes are valid;
  ble_ae_params.primary_adv_chnl_map      = RSI_BLE_ADV_CHANNEL_MAP;
  ble_ae_params.own_addr_type             = LE_PUBLIC_ADDRESS;
  ble_ae_params.peer_addr_type            = LE_RANDOM_ADDRESS;
  rsi_ascii_dev_address_to_6bytes_rev((uint8_t *)peer_addr, (int8_t *)REM_DEV_ADDR);
  memcpy(ble_ae_params.peer_dev_addr, peer_addr, RSI_DEV_ADDR_LEN);
  ble_ae_params.adv_filter_policy      = BLE_AE_ADV_FILTER_POLICY;
  ble_ae_params.adv_tx_power           = BLE_AE_ADV_TX_PWR;
  ble_ae_params.primary_adv_phy        = PRIMARY_ADV_PHY;
  ble_ae_params.sec_adv_max_skip       = BLE_AE_SEC_ADV_MAX_SKIP;
  ble_ae_params.sec_adv_phy            = SECONDARY_ADV_PHY;
  ble_ae_params.adv_sid                = 0x01;
  ble_ae_params.scan_req_notify_enable = SCAN_REQ_NOTIF_EN;

  LOG_PRINT("\nBLE_AE_ADV_EVNT_PROP = %x \n", BLE_AE_ADV_EVNT_PROP);

  status = rsi_ble_set_ae_params(&ble_ae_params, &rsi_app_resp_tx_power);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("set ae params failed with 0x%lX \n", status);
    return status;
  } else {
    LOG_PRINT("Setting AE params of set 2 successful and selected TX Power is %d dbm \n", rsi_app_resp_tx_power);
  }
#endif

  //! set AE set random address
  status = rsi_ble_set_ae_set_random_address(BLE_ADV_HNDL1, rand_addr);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("set ae set random address failed with 0x%lX \n", status);
    return status;
  } else {
    LOG_PRINT("set ae set random address successful \n");
  }

  //ae advertising enable for set 1
  rsi_ble_ae_adv_enable_t ble_ae_adv = { 0 };

  ble_ae_adv.enable        = RSI_BLE_START_ADV;
  ble_ae_adv.no_of_sets    = 1;
  ble_ae_adv.adv_handle    = BLE_ADV_HNDL1;
  ble_ae_adv.duration      = 0;
  ble_ae_adv.max_ae_events = BLE_MAX_AE_EVNTS;

  status = rsi_ble_start_ae_advertising(&ble_ae_adv);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("set ae adv enable failed with status 0x%lX\n", status);
    return status;
  } else {
    LOG_PRINT("set ae adv enable success \n");
  }

#if ADV_SET2
  ble_ae_adv.enable        = RSI_BLE_START_ADV;
  ble_ae_adv.no_of_sets    = 1;
  ble_ae_adv.adv_handle    = 0x01;
  ble_ae_adv.duration      = 0;
  ble_ae_adv.max_ae_events = BLE_MAX_AE_EVNTS;

  status = rsi_ble_start_ae_advertising(&ble_ae_adv);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("set ae adv enable failed with status 0x%lX\n", status);
    return status;
  } else {
    LOG_PRINT("set ae adv enable success \n");
  }
#endif

#if PERIODIC_ADV_EN
  //setting periodic advertising data
  rsi_ble_ae_data_t ble_ae_data = { 0 };
  ble_ae_data.type              = BLE_AE_PERIODIC_ADV_DATA_TYPE;
  ble_ae_data.adv_handle        = 0x00;
  ble_ae_data.operation         = 0x00;
  ble_ae_data.frag_pref         = 0x00;
  ble_ae_data.data_len          = BLE_AE_ADV_DATA_LEN;
  memcpy(ble_ae_data.data, data, ble_ae_data.data_len);

  status = rsi_ble_set_ae_data(&ble_ae_data);

  if (status != RSI_SUCCESS) {
    LOG_PRINT("set periodic adv data failed with 0x%lX\n", status);
    return status;
  } else {
    LOG_PRINT("set periodic adv data success \n");
  }
#else
  //setting extended advertising data
  rsi_ble_ae_data_t ble_ae_data = { 0 };

  ble_ae_data.type       = BLE_AE_ADV_DATA_TYPE;
  ble_ae_data.adv_handle = BLE_ADV_HNDL1;
  ble_ae_data.operation  = 0x00;
  ble_ae_data.frag_pref  = 0x00;
  ble_ae_data.data_len   = BLE_AE_ADV_DATA_LEN;
  memcpy(ble_ae_data.data, data, ble_ae_data.data_len);

  status = rsi_ble_set_ae_data(&ble_ae_data);

  if (status != RSI_SUCCESS) {
    LOG_PRINT("set ae data failed with 0x%lX\n", status);
    return status;
  } else {
    LOG_PRINT("set ae data success \n");
  }

  //setting ae scan response data
  ble_ae_data.type       = BLE_AE_SCAN_RSP_DATA_TYPE;
  ble_ae_data.adv_handle = BLE_ADV_HNDL1;
  ble_ae_data.operation  = 0x00;
  ble_ae_data.frag_pref  = 0x00;
  ble_ae_data.data_len   = BLE_AE_ADV_DATA_LEN;
  memcpy(ble_ae_data.data, data, ble_ae_data.data_len);

  status = rsi_ble_set_ae_data(&ble_ae_data);

  if (status != RSI_SUCCESS) {
    LOG_PRINT("set ae data failed with 0x%lX\n", status);
    return status;
  } else {
    LOG_PRINT("set ae data success \n");
  }
#endif
#if ADV_SET2
  //setting extended advertising data for set 2

  ble_ae_data.type       = BLE_AE_ADV_DATA_TYPE;
  ble_ae_data.adv_handle = 0x01;
  ble_ae_data.operation  = 0x03;
  ble_ae_data.frag_pref  = 0x00;
  ble_ae_data.data_len   = BLE_AE_ADV_DATA_LEN;
  memcpy(ble_ae_data.data, data, ble_ae_data.data_len);

  status = rsi_ble_set_ae_data(&ble_ae_data);

  if (status != RSI_SUCCESS) {
    LOG_PRINT("set ae data failed with 0x%lX\n", status);
    return status;
  } else {
    LOG_PRINT("set ae data success \n");
  }

  //setting ae scan response data
  ble_ae_data.type       = BLE_AE_SCAN_RSP_DATA_TYPE;
  ble_ae_data.adv_handle = 0x01;
  ble_ae_data.operation  = 0x03;
  ble_ae_data.frag_pref  = 0x00;
  ble_ae_data.data_len   = BLE_AE_ADV_DATA_LEN;
  memcpy(ble_ae_data.data, data, ble_ae_data.data_len);

  status = rsi_ble_set_ae_data(&ble_ae_data);

  if (status != RSI_SUCCESS) {
    LOG_PRINT("set ae data failed with 0x%lX\n", status);
    return status;
  } else {
    LOG_PRINT("set ae data success \n");
  }
#endif

#if ENABLE_POWER_SAVE
  LOG_PRINT("\r\n Initiate module in to power save \r\n");
  //! enable wlan radio
  status = rsi_wlan_radio_init();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\n radio init failed \n");
    return status;
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

  LOG_PRINT("\r\n Module is in power save \r\n");
#endif

  while (1) {
    //! Application main loop
#ifndef RSI_WITH_OS
    //! run the non OS scheduler
    rsi_wireless_driver_task();
#endif
    //! checking for received events
    temp_event_map = rsi_ble_app_get_event();
    if (temp_event_map == RSI_FAILURE) {
      //! if events are not received loop will be continued.
      continue;
    }

    //! if any event is received, it will be served.
    switch (temp_event_map) {
      case RSI_APP_EVENT_CONNECTED: {
        //! remote device connected event

        //! clear the connected event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_CONNECTED);

        //! converts the 6 byte MAC address to string format
        rsi_6byte_dev_address_to_ascii((uint8_t *)remote_dev_addr, rsi_app_connected_device.dev_addr);

        LOG_PRINT("\n Remote device with %s address connected \n", remote_dev_addr);

      } break;

      case RSI_APP_EVENT_DISCONNECTED: {
        //! remote device connected event

        //! clear the disconnected event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_DISCONNECTED);

        rsi_6byte_dev_address_to_ascii((uint8_t *)remote_dev_addr, rsi_app_disconnected_device.dev_addr);

        LOG_PRINT("\n Remote device with %s address disconnected \n", remote_dev_addr);

        //retriggering ae advertiser enable for set 1
        rsi_ble_ae_adv_enable_t ble_ae_adv = { 0 };

        ble_ae_adv.enable        = RSI_BLE_START_ADV;
        ble_ae_adv.no_of_sets    = 1;
        ble_ae_adv.adv_handle    = BLE_ADV_HNDL1;
        ble_ae_adv.duration      = 0;
        ble_ae_adv.max_ae_events = BLE_MAX_AE_EVNTS;

        status = rsi_ble_start_ae_advertising(&ble_ae_adv);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("set ae adv enable failed with 0x%lX \n", status);
        } else {
          LOG_PRINT("set ae adv enable success \n");
        }

      } break;
      case RSI_APP_EVENT_EXT_ADV_TERMINATED: {
        rsi_ble_app_clear_event(RSI_APP_EVENT_EXT_ADV_TERMINATED);
        LOG_PRINT("\n Extended Advertsing terminated event received for adv handle %d , conn handle %d \n",
                  adv_set_terminated.adv_handle,
                  adv_set_terminated.conn_handle);

        //retriggering ae advertiser enable for set 1
        rsi_ble_ae_adv_enable_t ble_ae_adv = { 0 };

        ble_ae_adv.enable        = RSI_BLE_START_ADV;
        ble_ae_adv.no_of_sets    = 1;
        ble_ae_adv.adv_handle    = BLE_ADV_HNDL1;
        ble_ae_adv.duration      = 0;
        ble_ae_adv.max_ae_events = BLE_MAX_AE_EVNTS;

        status = rsi_ble_start_ae_advertising(&ble_ae_adv);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("set ae adv enable failed with 0x%lX \n", status);
        } else {
          LOG_PRINT("set ae adv enable success \n");
        }
      } break;
      case RSI_APP_EVENT_SCAN_REQ_RECEIVED: {
        rsi_ble_app_clear_event(RSI_APP_EVENT_SCAN_REQ_RECEIVED);
        rsi_6byte_dev_address_to_ascii((uint8_t *)remote_dev_addr, scan_req_recvd.scanner_addr);
        LOG_PRINT("\n Scan request received for handle %d with addr %s \n", scan_req_recvd.adv_handle, remote_dev_addr);
      } break;
    }
  }

  return 0;
}

/*==============================================*/
/**
 * @fn         main_loop
 * @brief      Schedules the Driver task.
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function schedules the Driver task.
 */
void main_loop(void)
{
  while (1) {
    rsi_wireless_driver_task();
  }
}

/*==============================================*/
/**
 * @fn         main
 * @brief      main function
 * @param[in]  none.
 * @return     none.
 * @section description
 * This is the main function to call Application
 */
int main(void)
{
  int32_t status;
#ifdef RSI_WITH_OS
  rsi_task_handle_t ble_task_handle = NULL;
#endif

#ifndef RSI_WITH_OS
  //Start BT Stack
  intialize_bt_stack(STACK_BTLE_MODE);

  //! Call BLE Peripheral application
  status = rsi_ble_peripheral();

  //! Application main loop
  main_loop();

  return status;
#endif
#ifdef RSI_WITH_OS
  //! Driver initialization
  status = rsi_driver_init(global_buf, BLE_GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > BLE_GLOBAL_BUFF_LEN)) {
    return status;
  }
#ifdef RSI_M4_INTERFACE
  // Silicon labs module initialization
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nDevice Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
#endif
  //Start BT Stack
  intialize_bt_stack(STACK_BTLE_MODE);

  //! OS case
  //! Task created for BLE task
  rsi_task_create((rsi_task_function_t)(int32_t)rsi_ble_peripheral,
                  (uint8_t *)"ble_task",
                  RSI_BLE_TASK_STACK_SIZE,
                  NULL,
                  RSI_BLE_TASK_PRIORITY,
                  &ble_task_handle);

  //! OS TAsk Start the scheduler
  rsi_start_os_scheduler();

  return status;
#endif
}

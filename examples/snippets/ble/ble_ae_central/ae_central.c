/*******************************************************************************
* @file  ae_central.c
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
 * @brief : This file contains example application for BLE AE Central Application
 * @section Description :
 * This application demonstrates how to connect with remote BLE device in BLE
 * central AE mode.
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

#include <string.h>
//! Common include file
#include <rsi_common_apis.h>

//! Address type of the device to connect
#define RSI_BLE_DEV_ADDR_TYPE LE_RANDOM_ADDRESS

//! Address of the device to connect
#define RSI_BLE_DEV_ADDR "00:23:A7:80:70:B9"

//! Remote Device Name to connect
#define RSI_REMOTE_DEVICE_NAME "SILABS_DEVTEJ"

//! Remote Device ADDR to Show ADV Report
#define RSI_REMOTE_DEVICE_ADDR "88:DA:1A:9E:81:54"

#define SET2_ENABLE 1

/*=======================================================================*/
//!	Powersave configurations
/*=======================================================================*/
#define ENABLE_POWER_SAVE 0 //! Set to 1 for powersave mode
#if ENABLE_POWER_SAVE
//! Power Save Profile Mode
#define PSP_MODE RSI_SLEEP_MODE_2
//! Power Save Profile type
#define PSP_TYPE RSI_MAX_PSP
#endif

#if defined(RSI_DEBUG_PRINTS)
#define LOG_PRINT_D LOG_PRINT
#else
#define LOG_PRINT_D
#endif

//! Memory length for the driver
#define BLE_GLOBAL_BUFF_LEN 15000

//! Memory to initialize the driver
uint8_t global_buf[BLE_GLOBAL_BUFF_LEN];

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

//! Application supported events list
#define RSI_APP_EVENT_ADV_REPORT   0
#define RSI_APP_EVENT_CONNECTED    1
#define RSI_APP_EVENT_DISCONNECTED 2
#if 1 //def AE_ENABLE
#define RSI_APP_EVENT_AE_ADV_REPORT         3
#define RSI_APP_EVENT_AE_PER_SYNC_ESTBL     4
#define RSI_APP_EVENT_AE_PER_ADV_REPORT     5
#define RSI_APP_EVENT_AE_PER_ADV_SYNC_LOST  6
#define RSI_APP_EVENT_AE_SCAN_TIMEOUT       7
#define RSI_APP_EVENT_AE_ADV_SET_TERMINATED 8
#define RSI_APP_EVENT_AE_SCAN_REQ_RECVD     9
#endif

#ifdef RSI_WITH_OS
//! BLE task stack size
#define RSI_BT_TASK_STACK_SIZE 1000

//! BT task priority
#define RSI_BT_TASK_PRIORITY 1

//! Number of packet to send or receive
#define NUMBER_OF_PACKETS 1000

//! Wireless driver task priority
#define RSI_DRIVER_TASK_PRIORITY 2

//! Wireless driver task stack size
#define RSI_DRIVER_TASK_STACK_SIZE 3000

void rsi_wireless_driver_task(void);

#endif
//! Application global parameters.
static uint8_t rsi_app_async_event_map = 0;
static uint8_t remote_addr_type        = 0;
static uint8_t remote_name[31];
static uint8_t remote_dev_addr[18]   = { 0 };
static uint8_t device_found          = 0;
static uint8_t remote_dev_bd_addr[6] = { 0 };

static rsi_ble_event_enhance_conn_status_t rsi_app_enhanced_connected_device;
static rsi_ble_event_conn_status_t rsi_app_connected_device;
static rsi_ble_event_disconnect_t rsi_app_disconnected_device;
static rsi_ble_ae_adv_report_t ble_app_ae_adv_report;

rsi_semaphore_handle_t ble_peripheral_conn_sem;
rsi_semaphore_handle_t ble_main_task_sem;

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
 * @brief      set the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
static void rsi_ble_app_set_event(uint32_t event_num)
{
  rsi_app_async_event_map |= BIT(event_num);
  rsi_semaphore_post(&ble_main_task_sem);
  return;
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_clear_event
 * @brief      clear the specific event.
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

  return (-1);
}

void rsi_ble_simple_central_on_ae_adv_report_event(uint16_t status, rsi_ble_ae_adv_report_t *ae_adv_report)
{
  UNUSED_PARAMETER(status);
  memcpy(ble_app_ae_adv_report.remote_addr, ae_adv_report->remote_addr, RSI_DEV_ADDR_LEN);
  ble_app_ae_adv_report.remote_addr_type = ae_adv_report->remote_addr_type;

  if (device_found == 1) {
    return;
  }

  memset(remote_name, 0, 31);
  BT_LE_ADPacketExtract(remote_name, ae_adv_report->data, ae_adv_report->data_len);

  remote_addr_type = ae_adv_report->remote_addr_type;
  rsi_6byte_dev_address_to_ascii(remote_dev_addr, (uint8_t *)ae_adv_report->remote_addr);
  memcpy(remote_dev_bd_addr, (uint8_t *)ae_adv_report->remote_addr, 6);
  if (((device_found == 0) && ((strcmp((const char *)remote_name, RSI_REMOTE_DEVICE_NAME)) == 0))
      || ((remote_addr_type == RSI_BLE_DEV_ADDR_TYPE)
          && ((strcmp((const char *)remote_dev_addr, RSI_BLE_DEV_ADDR) == 0)))) {
    device_found = 1;
    rsi_ble_app_set_event(RSI_APP_EVENT_AE_ADV_REPORT);
  }
}

void rsi_ble_simple_central_on_ae_periodic_adv_sync_estbl_event(uint16_t status,
                                                                rsi_ble_per_adv_sync_estbl_t *ae_periodic_adv_sync)
{
  UNUSED_PARAMETER(status);
  UNUSED_PARAMETER(ae_periodic_adv_sync);
  rsi_ble_app_set_event(RSI_APP_EVENT_AE_PER_SYNC_ESTBL);
}

void rsi_ble_simple_central_on_ae_periodic_adv_report_event(uint16_t status,
                                                            rsi_ble_per_adv_report_t *ae_per_adv_report)
{
  UNUSED_PARAMETER(status);
  UNUSED_PARAMETER(ae_per_adv_report);

  rsi_ble_app_set_event(RSI_APP_EVENT_AE_PER_ADV_REPORT);
}

void rsi_ble_simple_central_on_ae_periodic_adv_sync_lost_event(uint16_t status,
                                                               rsi_ble_per_adv_sync_lost_t *ae_per_sync_lost)
{
  UNUSED_PARAMETER(status);

  LOG_PRINT("\n \n Periodic Adv Sync Lost Event : sync handle = %d \r\n", ae_per_sync_lost->sync_handle);

  rsi_ble_app_set_event(RSI_APP_EVENT_AE_PER_ADV_SYNC_LOST);
}

void rsi_ble_simple_central_on_ae_scan_timeout_event(uint16_t status, rsi_ble_scan_timeout_t *ae_scan_timeout)
{
  UNUSED_PARAMETER(status);
  UNUSED_PARAMETER(ae_scan_timeout);

  LOG_PRINT("\n \n AE Scan time-out event \r\n");

  rsi_ble_app_set_event(RSI_APP_EVENT_AE_SCAN_TIMEOUT);
}

void rsi_ble_simple_central_on_ae_adv_set_terminated_event(uint16_t status, rsi_ble_adv_set_terminated_t *ae_adv_set)
{
  UNUSED_PARAMETER(status);

  LOG_PRINT("\n \n Adv set terminated event :status = %d \r\n", ae_adv_set->status);
  LOG_PRINT_D("\n \n \t \t adv handle = %d \r\n", ae_adv_set->adv_handle);
  LOG_PRINT_D("\n \n \t \t connection handle = %d \r\n", ae_adv_set->conn_handle);
  LOG_PRINT_D("\n \n \t \t No.of Completed AE events = %d \r\n", ae_adv_set->num_completed_ae_events);

  rsi_ble_app_set_event(RSI_APP_EVENT_AE_ADV_SET_TERMINATED);
}

void rsi_ble_simple_central_on_ae_scan_req_recvd_event(uint16_t status, rsi_ble_scan_req_recvd_t *ae_scan_req)
{
  UNUSED_PARAMETER(status);

  uint8_t scanner_addr[18] = { 0 };

  LOG_PRINT("\n \n AE scan request received event :adv handle = %d \r\n", ae_scan_req->adv_handle);
  LOG_PRINT_D("\n \n \t \t scanner addr type = %d \r\n", ae_scan_req->scanner_addr_type);

  rsi_6byte_dev_address_to_ascii((uint8_t *)scanner_addr, (uint8_t *)ae_scan_req->scanner_addr);
  LOG_PRINT_D("\n \n \t \t Adv addr = %s\r\n", scanner_addr);

  rsi_ble_app_set_event(RSI_APP_EVENT_AE_SCAN_REQ_RECVD);
}

/*==============================================*/
/**
 * @fn         rsi_ble_simple_central_on_adv_report_event
 * @brief      invoked when advertise report event is received
 * @param[in]  adv_report, pointer to the received advertising report
 * @return     none.
 * @section description
 * This callback function updates the scanned remote devices list
 */
void rsi_ble_simple_central_on_adv_report_event(rsi_ble_event_adv_report_t *adv_report)
{

  if (device_found == 1) {
    return;
  }

  BT_LE_ADPacketExtract(remote_name, adv_report->adv_data, adv_report->adv_data_len);

  remote_addr_type = adv_report->dev_addr_type;
  rsi_6byte_dev_address_to_ascii(remote_dev_addr, (uint8_t *)adv_report->dev_addr);
  if (((device_found == 0) && ((strcmp((const char *)remote_name, RSI_REMOTE_DEVICE_NAME)) == 0))
      || ((remote_addr_type == RSI_BLE_DEV_ADDR_TYPE)
          && ((strcmp((const char *)remote_dev_addr, RSI_BLE_DEV_ADDR) == 0)))) {
    device_found = 1;
    rsi_ble_app_set_event(RSI_APP_EVENT_ADV_REPORT);
  }
}

/*==============================================*/
/**
 * @fn         rsi_ble_simple_central_on_enhance_conn_status_event
 * @brief      invoked when enhanced connection complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
void rsi_ble_simple_central_on_enhance_conn_status_event(rsi_ble_event_enhance_conn_status_t *resp_enh_conn)
{
  //rsi_app_connected_device.dev_addr_type = resp_enh_conn->dev_addr_type;
  //memcpy(rsi_app_connected_device.dev_addr, resp_enh_conn->dev_addr, RSI_DEV_ADDR_LEN);
  //rsi_app_connected_device.status = resp_enh_conn->status;
  memcpy(&rsi_app_enhanced_connected_device, resp_enh_conn, sizeof(rsi_ble_event_enhance_conn_status_t));
  rsi_ble_app_set_event(RSI_APP_EVENT_CONNECTED);
  rsi_semaphore_post(&ble_peripheral_conn_sem);
}

/*==============================================*/
/**
 * @fn         rsi_ble_simple_central_on_conn_status_event
 * @brief      invoked when connection complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
void rsi_ble_simple_central_on_conn_status_event(rsi_ble_event_conn_status_t *resp_conn)
{
  memcpy(&rsi_app_connected_device, resp_conn, sizeof(rsi_ble_event_conn_status_t));
  rsi_ble_app_set_event(RSI_APP_EVENT_CONNECTED);
  rsi_semaphore_post(&ble_peripheral_conn_sem);
}

/*==============================================*/
/**
 * @fn         rsi_ble_simple_central_on_disconnect_event
 * @brief      invoked when disconnection event is received
 * @param[in]  resp_disconnect, disconnected remote device information
 * @param[in]  reason, reason for disconnection.
 * @return     none.
 * @section description
 * This Callback function indicates disconnected device information and status
 */
void rsi_ble_simple_central_on_disconnect_event(rsi_ble_event_disconnect_t *resp_disconnect, uint16_t reason)
{
  memcpy(&rsi_app_disconnected_device, resp_disconnect, sizeof(rsi_ble_event_disconnect_t));
  LOG_PRINT("\nRem Device Dosconnected with status = %x\n", reason);

  rsi_ble_app_set_event(RSI_APP_EVENT_DISCONNECTED);
}

/*==============================================*/
/**
 * @fn         ble_extended_adv_callbacks_register
 * @brief      Registers the extended advertiser callbacks
 * @param[in]  none.
 * @param[out] none.
 * @return     none.
 * @section description
 * This function registers all the extended advertiser callbacks
 */
void ble_extended_adv_callbacks_register(void)
{
  uint32_t status = 0;
  status          = rsi_ble_adv_ext_events_register_callbacks(RSI_BLE_ON_ADV_EXT_ADVERTISE_REPORT_EVENT,
                                                     (void *)rsi_ble_simple_central_on_ae_adv_report_event);

  if (status != RSI_SUCCESS) {
    LOG_PRINT("\n RSI_BLE_ON_ADV_EXT_ADVERTISE_REPORT_EVENT callback registering failed \n");
  }
  status =
    rsi_ble_adv_ext_events_register_callbacks(RSI_BLE_ON_ADV_EXT_PERIODIC_ADV_SYNC_ESTBL_EVENT,
                                              (void *)rsi_ble_simple_central_on_ae_periodic_adv_sync_estbl_event);

  if (status != RSI_SUCCESS) {
    LOG_PRINT("\n RSI_BLE_ON_ADV_EXT_PERIODIC_ADV_SYNC_ESTBL_EVENT callback registering failed \n");
  }
  status = rsi_ble_adv_ext_events_register_callbacks(RSI_BLE_ON_ADV_EXT_PERIODIC_ADVERTISE_REPORT_EVENT,
                                                     (void *)rsi_ble_simple_central_on_ae_periodic_adv_report_event);

  if (status != RSI_SUCCESS) {
    LOG_PRINT("\n RSI_BLE_ON_ADV_EXT_PERIODIC_ADVERTISE_REPORT_EVENT callback registering failed \n");
  }
  status = rsi_ble_adv_ext_events_register_callbacks(RSI_BLE_ON_ADV_EXT_PERIODIC_ADV_SYNC_LOST_EVENT,
                                                     (void *)rsi_ble_simple_central_on_ae_periodic_adv_sync_lost_event);

  if (status != RSI_SUCCESS) {
    LOG_PRINT("\n RSI_BLE_ON_ADV_EXT_PERIODIC_ADV_SYNC_LOST_EVENT callback registering failed \n");
  }
  status = rsi_ble_adv_ext_events_register_callbacks(RSI_BLE_ON_ADV_EXT_SCAN_TIMEOUT_EVENT,
                                                     (void *)rsi_ble_simple_central_on_ae_scan_timeout_event);

  if (status != RSI_SUCCESS) {
    LOG_PRINT("\n RSI_BLE_ON_ADV_EXT_SCAN_TIMEOUT_EVENT callback registering failed \n");
  }
  status = rsi_ble_adv_ext_events_register_callbacks(RSI_BLE_ON_ADV_EXT_ADVERTISE_SET_TERMINATED_EVENT,
                                                     (void *)rsi_ble_simple_central_on_ae_adv_set_terminated_event);

  if (status != RSI_SUCCESS) {
    LOG_PRINT("\n RSI_BLE_ON_ADV_EXT_ADVERTISE_SET_TERMINATED_EVENT callback registering failed \n");
  }
  status = rsi_ble_adv_ext_events_register_callbacks(RSI_BLE_ON_ADV_EXT_SCAN_REQUEST_RECEIVED_EVENT,
                                                     (void *)rsi_ble_simple_central_on_ae_scan_req_recvd_event);

  if (status != RSI_SUCCESS) {
    LOG_PRINT("\n RSI_BLE_ON_ADV_EXT_SCAN_REQUEST_RECEIVED_EVENT callback registering failed \n");
  }
}

/*==============================================*/
/**
 * @fn         rsi_ble_central
 * @brief      Tests the BLE GAP central role.
 * @param[in]  none
 * @return    none.
 * @section description
 * This function is used to test the BLE central role and simple GAP API's.
 */
int32_t rsi_ble_central(void)
{
  int32_t status          = 0;
  int32_t temp_event_map  = 0;
  int32_t temp_event_map1 = 0;
  uint8_t size            = 0;
  uint8_t fmversion[20]   = { 0 };
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

  //! BLE register GAP callbacks
  rsi_ble_gap_register_callbacks(rsi_ble_simple_central_on_adv_report_event,
                                 rsi_ble_simple_central_on_conn_status_event,
                                 rsi_ble_simple_central_on_disconnect_event,
                                 NULL,
                                 NULL,
                                 NULL,
                                 rsi_ble_simple_central_on_enhance_conn_status_event,
                                 NULL,
                                 NULL,
                                 NULL);

  //Registering extended advertising event callbacks
  ble_extended_adv_callbacks_register();

  //! initialize the event map
  rsi_ble_app_init_events();
  rsi_semaphore_create(&ble_main_task_sem, 0);
  rsi_semaphore_create(&ble_peripheral_conn_sem, 0);

  // AE set scan params
  rsi_ble_ae_set_scan_params_t ae_set_scan_params          = { 0 };
  ae_set_scan_params.own_addr_type                         = LE_PUBLIC_ADDRESS;
  ae_set_scan_params.scanning_filter_policy                = RSI_BLE_SCAN_FILTER_TYPE;
  ae_set_scan_params.scanning_phys                         = (PHY_1M | PHY_LE_CODED);
  ae_set_scan_params.ScanParams[AE_SCAN_SET1].ScanType     = PRI_PHY_BLE_SCAN_TYPE;
  ae_set_scan_params.ScanParams[AE_SCAN_SET1].ScanInterval = PRI_PHY_LE_SCAN_INTERVAL;
  ae_set_scan_params.ScanParams[AE_SCAN_SET1].ScanWindow   = PRI_PHY_LE_SCAN_WINDOW;
#if SET2_ENABLE
  ae_set_scan_params.ScanParams[AE_SCAN_SET2].ScanType     = SEC_PHY_BLE_SCAN_TYPE;
  ae_set_scan_params.ScanParams[AE_SCAN_SET2].ScanInterval = SEC_PHY_LE_SCAN_INTERVAL;
  ae_set_scan_params.ScanParams[AE_SCAN_SET2].ScanWindow   = SEC_PHY_LE_SCAN_WINDOW;
#endif
  status = rsi_ble_ae_set_scan_params(&ae_set_scan_params);
  if (status != RSI_SUCCESS) {
    LOG_PRINT(" \n set ae scan params failed with status 0x%lX\n", status);
  } else {
    LOG_PRINT(" \n set ae scan params success \n");
  }
  // AE scan enable
  rsi_ble_ae_set_scan_enable_t ae_set_scan_enable = { 0 };
  ae_set_scan_enable.enable                       = RSI_BLE_START_SCAN;
  ae_set_scan_enable.filter_duplicates            = BLE_SCAN_ENABLE_FILTER_DUP;
  ae_set_scan_enable.duration                     = BLE_SCAN_DUR;
  ae_set_scan_enable.period                       = BLE_SCAN_PERIOD;
  status                                          = rsi_ble_ae_set_scan_enable(&ae_set_scan_enable);
  if (status != RSI_SUCCESS) {
    LOG_PRINT(" \n set ae scan enable failed with 0x%lX \n", status);
    return status;
  } else {
    LOG_PRINT(" \n set ae scan enable success \n");
  }

  rsi_ble_ae_dev_to_periodic_list_t ae_add_dev = { 0 };
  rsi_ascii_dev_address_to_6bytes_rev((uint8_t *)ae_add_dev.adv_addr, (int8_t *)REM_ADDR1);
  ae_add_dev.adv_addr_type = LE_RANDOM_ADDRESS;
  ae_add_dev.adv_sid       = 0x01;
  ae_add_dev.type          = BLE_AE_PER_ADD_DEV;
  // AE Add Device to Periodic Adv list
  status = rsi_ble_ae_dev_to_periodic_list(&ae_add_dev);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\n Adding device to Periodic list failed with 0x%lX", status);
  } else {
    LOG_PRINT("\n successfully added remote device to Periodic Adv List \n");
  }

  // AE Periodic create sync
  rsi_ble_ae_set_periodic_adv_create_sync_t ae_per_sync_create = { 0 };
  rsi_ascii_dev_address_to_6bytes_rev((uint8_t *)ae_per_sync_create.adv_addr, (int8_t *)REM_ADDR);
  ae_per_sync_create.adv_addr_type = LE_PUBLIC_ADDRESS;
  ae_per_sync_create.adv_sid       = BLE_AE_ADV_SID;
  ae_per_sync_create.options       = BLE_AE_OPTIONS;
  ae_per_sync_create.skip          = BLE_AE_ADV_SKIP;
  ae_per_sync_create.sync_timeout  = BLE_AE_SYNC_TIMOUT;
  LOG_PRINT("\n size of create sync structure is %d\n", sizeof(ae_per_sync_create));
  status = rsi_ble_ae_set_periodic_sync(BLE_AE_PER_SYNC_CREATE, &ae_per_sync_create);
  if (status != RSI_SUCCESS) {
    LOG_PRINT(" \n set ae periodic sync failed with 0x%lX \n", status);
  } else {
    LOG_PRINT(" \n set ae periodic sync success \n");
  }

#if BLE_AE_PERIODIC_SYNC_CANCEL
  // AE Periodic create cancel sync
  status = rsi_ble_ae_set_periodic_sync(BLE_AE_PER_SYNC_CANCEL, &ae_per_sync_create);
  if (status != RSI_SUCCESS) {
    LOG_PRINT(" \n set ae periodic sync cancel failed with 0x%lX \n ", status);
    return status;
  } else {
    LOG_PRINT(" \n set ae periodic sync cancel success \n");
  }
#endif

#if 0
  rsi_ble_ae_dev_to_periodic_list_t ae_remv_dev ={0};
  rsi_ascii_dev_address_to_6bytes_rev((uint8_t *)ae_remv_dev.adv_addr,(int8_t *)REM_ADDR1);
  ae_remv_dev.adv_addr_type = LE_RANDOM_ADDRESS;
  ae_remv_dev.adv_sid = 1;
  ae_remv_dev.type = BLE_AE_PER_REM_DEV;
  // AE Remove Device to Periodic Adv list
  status = rsi_ble_ae_dev_to_periodic_list(&ae_remv_dev);
  if(status != RSI_SUCCESS)
  {
    LOG_PRINT("\n Removing device from AE periodic list failed with 0x%lX",status);
  }
  else{
      LOG_PRINT("\n successfully removed remote device from Periodic Adv List \n");
  }
#endif

#if BLE_AE_PERIODIC_CLEAR_ADV_LIST
  rsi_ble_ae_dev_to_periodic_list_t ae_clear_dev = { 0 };
  // AE Clear Periodic Adv list
  rsi_ascii_dev_address_to_6bytes_rev((uint8_t *)REM_ADDR1, (int8_t *)ae_clear_dev.adv_addr);
  ae_clear_dev.adv_addr_type = LE_RANDOM_ADDRESS;
  ae_clear_dev.adv_sid       = 1;
  ae_clear_dev.type          = BLE_AE_CLEAR_PER_LIST;
  status                     = rsi_ble_ae_dev_to_periodic_list(&ae_clear_dev);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\n Clearing Periodic device list failed with 0x%lX", status);
  } else {
    LOG_PRINT("\n successfully cleared Periodic Adv List \n");
  }

  // AE Read Periodic Advertiser list size
  status = rsi_ble_ae_read_periodic_adv_list_size(&size);
  if (status != RSI_SUCCESS) {
    LOG_PRINT(" \n read ae periodic adv list size cmd failed with 0x%lX \n", status);
    return status;
  } else {
    LOG_PRINT(" \n read ae periodic adv list size cmd success and size is %d \n", size);
  }
#endif
  // Read Transmit Power
  rsi_ble_tx_pwr_t tx_pwr;
  status = rsi_ble_read_transmit_power(&tx_pwr);
  if (status != RSI_SUCCESS) {
    LOG_PRINT(" \n read tx_power cmd failed with 0x%lX \n", status);
    return status;
  } else {
    LOG_PRINT(" \n read tx power cmd success and  \n min_tx_pwr = %i \n max_tx_pwr = %i \n",
              tx_pwr.min_tx_pwr,
              tx_pwr.max_tx_pwr);
  }

#if 0
  // Read RF Path Compensation Read/Write
  rsi_ble_query_rf_path_comp_t *rf_path_comp;
  status = rsi_ble_read_rf_path_compensation(&rf_path_comp);
  if(status != RSI_SUCCESS)
  {
    LOG_PRINT (" \n read rf path comp cmd failed with 0x%lX \n",status);
    return status;
  }
  else
  {
    LOG_PRINT (" \n read rf path comp cmd success and  \n tx_path_value  = %i \n rx_path_value = %i \n",rf_path_comp->tx_path_value,rf_path_comp->rx_path_value);
  }


  // Write RF Path Compensation Read/Write
  status = rsi_ble_write_rf_path_compensation(10,10);
  if(status != RSI_SUCCESS)
  {
    LOG_PRINT (" \n write rf path comp cmd failed with 0x%lX \n");
    return status;
  }

#endif

  while (1) {
    //! Application main loop
#ifndef RSI_WITH_OS
    //! run the non os scheduler
    rsi_wireless_driver_task();
#endif
    //! checking for received events
    temp_event_map = rsi_ble_app_get_event();
    if (temp_event_map == RSI_FAILURE) {
      rsi_semaphore_wait(&ble_main_task_sem, 0);
      //! if events are not received, loop will be continued
      continue;
    }

    switch (temp_event_map) {
      case RSI_APP_EVENT_ADV_REPORT: {
        //! advertise report event.

        //! clear the advertise report event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_ADV_REPORT);

        status = rsi_ble_connect(remote_addr_type, (int8_t *)remote_dev_addr);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("connect status: 0x%lX\r\n", status);
        }

      } break;
      case RSI_APP_EVENT_AE_ADV_REPORT: {

        //! AE Adv Report

        //! clear the event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_AE_ADV_REPORT);

        //rsi_ble_ae_set_scan_enable_t ae_set_scan_enable = { 0 };
        ae_set_scan_enable.enable            = RSI_BLE_STOP_SCAN;
        ae_set_scan_enable.filter_duplicates = BLE_SCAN_ENABLE_FILTER_DUP;
        ae_set_scan_enable.duration          = BLE_SCAN_DUR;
        ae_set_scan_enable.period            = BLE_SCAN_PERIOD;
        status                               = rsi_ble_ae_set_scan_enable(&ae_set_scan_enable);
        if (status != RSI_SUCCESS) {
          LOG_PRINT(" \n set ae scan disable failed with 0x%lX \n", status);
        } else {
          LOG_PRINT(" \n set ae scan diasable success \n");
        }

        rsi_6byte_dev_address_to_ascii((uint8_t *)remote_dev_addr, (uint8_t *)ble_app_ae_adv_report.remote_addr);
        LOG_PRINT("\n ae report rec for %s with type %d\n", remote_dev_addr, ble_app_ae_adv_report.remote_addr_type);
#if 0
        rsi_delay_ms(1000);
        device_found = 0;
        // AE scan enable
       // rsi_ble_ae_set_scan_enable_t ae_set_scan_enable = { 0 };
        ae_set_scan_enable.enable                       = RSI_BLE_START_SCAN;
        ae_set_scan_enable.filter_duplicates            = BLE_SCAN_ENABLE_FILTER_DUP;
        ae_set_scan_enable.duration                     = BLE_SCAN_DUR;
        ae_set_scan_enable.period                       = BLE_SCAN_PERIOD;
        status                                          = rsi_ble_ae_set_scan_enable(&ae_set_scan_enable);
        if (status != RSI_SUCCESS) {
            LOG_PRINT(" \n set ae scan enable failed with 0x%lX \n", status);
        } else
          LOG_PRINT(" \n set ae scan enable success \n");
#else

        /**************************************************/
        /************ Extended Create Connect *************/
        /**************************************************/

        rsi_ble_ae_extended_create_connect_t ble_extended_create_conn = { 0 };
        uint8_t ix;

        ble_extended_create_conn.own_addr_type    = LE_PUBLIC_ADDRESS;
        ble_extended_create_conn.remote_addr_type = remote_addr_type;
        memcpy(ble_extended_create_conn.remote_addr, (int8_t *)remote_dev_bd_addr, RSI_DEV_ADDR_LEN);
        ble_extended_create_conn.initiator_filter_policy = ALLOW_SCAN_REQ_ANY_CONN_REQ_ANY;
        ble_extended_create_conn.init_phys               = INITIATING_PHYS;

        for (ix = 0; ix < 3; ix++) {
          ble_extended_create_conn.init_params[ix].ScanInterval    = PRI_PHY_LE_SCAN_INTERVAL;
          ble_extended_create_conn.init_params[ix].ScanWindow      = PRI_PHY_LE_SCAN_WINDOW;
          ble_extended_create_conn.init_params[ix].ConnIntervalMin = CONNECTION_INTERVAL_MIN;
          ble_extended_create_conn.init_params[ix].ConnIntervalMax = CONNECTION_INTERVAL_MAX;
          ble_extended_create_conn.init_params[ix].ConnLatency     = CONNECTION_LATENCY;
          ble_extended_create_conn.init_params[ix].ConnSTO         = SUPERVISION_TIMEOUT;
          ble_extended_create_conn.init_params[ix].MinCELen        = CONNECTION_EVENT_LEN_MIN;
          ble_extended_create_conn.init_params[ix].MaxCELen        = CONNECTION_EVENT_LEN_MAX;
        }

        LOG_PRINT("\r\n Initiating connect command \n");
        status = rsi_ble_extended_connect_with_params(&ble_extended_create_conn);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("connect status: 0x%lX\r\n", status);
        } else {

          rsi_semaphore_wait(&ble_peripheral_conn_sem, 10000);
          temp_event_map1 = rsi_ble_app_get_event();

          if ((temp_event_map1 == -1) || (!(temp_event_map1 & RSI_APP_EVENT_CONNECTED))) {
            LOG_PRINT("\r\n Initiating connect cancel command \n");
            status = rsi_ble_connect_cancel((int8_t *)remote_dev_bd_addr);
            if (status != RSI_SUCCESS) {
              LOG_PRINT("\r\n ble connect cancel cmd status = %lx \n", status);
            } else {
              rsi_ble_app_set_event(RSI_APP_EVENT_DISCONNECTED);
            }
          }
        }

#endif

      } break;

      case RSI_APP_EVENT_AE_PER_SYNC_ESTBL: {

        //! Ae Periodic Sync Estabilished event
#if BLE_AE_PERIODIC_SYNC_TERMIN
        // AE Terminate Periodic sync
        rsi_ble_ae_set_periodic_adv_terminate_sync_t ae_per_sync_terminate = { 0 };
        ae_per_sync_terminate.sync_handle                                  = BLE_AE_ADV_SID;
        status = rsi_ble_ae_set_periodic_sync(BLE_AE_PER_SYNC_TERMINATE, &ae_per_sync_terminate);
        if (status != RSI_SUCCESS) {
          LOG_PRINT(" \n set ae periodic sync terminate failed with 0x%lX \n");
        } else {
          LOG_PRINT(" \n set ae periodic sync terminate success \n");
        }
#endif
        //! clear the event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_AE_PER_SYNC_ESTBL);
      } break;

      case RSI_APP_EVENT_AE_PER_ADV_REPORT: {

        //! AE Periodic Adv Report

        //! clear the event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_AE_PER_ADV_REPORT);
      } break;

      case RSI_APP_EVENT_AE_PER_ADV_SYNC_LOST: {

        //! Ae Periodic Sync Lost event

        //! clear the event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_AE_PER_ADV_SYNC_LOST);
      } break;

      case RSI_APP_EVENT_AE_SCAN_TIMEOUT: {

        //! AE Scan Time-out event

        //! clear the event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_AE_SCAN_TIMEOUT);
      } break;

      case RSI_APP_EVENT_AE_ADV_SET_TERMINATED: {

        //! Ae Adv Set Terminated event

        //! clear the event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_AE_ADV_SET_TERMINATED);
      } break;

      case RSI_APP_EVENT_AE_SCAN_REQ_RECVD: {

        //! AE Scan Request Received event

        //! clear the event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_AE_SCAN_REQ_RECVD);
      } break;

      case RSI_APP_EVENT_CONNECTED: {
        //! remote device connected event

        //! clear the connected event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_CONNECTED);

        rsi_6byte_dev_address_to_ascii((uint8_t *)remote_dev_addr,
                                       (uint8_t *)rsi_app_enhanced_connected_device.dev_addr);
        LOG_PRINT("\n Remote device connected %s with type %d\n",
                  remote_dev_addr,
                  rsi_app_enhanced_connected_device.dev_addr_type);

        /**************************************************/
        /************ Extended Advertising Enable *********/
        /**************************************************/

        //device_found = 0;

        ae_set_scan_enable.enable            = RSI_BLE_START_SCAN;
        ae_set_scan_enable.filter_duplicates = BLE_SCAN_ENABLE_FILTER_DUP;
        ae_set_scan_enable.duration          = BLE_SCAN_DUR;
        ae_set_scan_enable.period            = BLE_SCAN_PERIOD;
        status                               = rsi_ble_ae_set_scan_enable(&ae_set_scan_enable);
        if (status != RSI_SUCCESS) {
          LOG_PRINT(" \n set ae scan enable failed with 0x%lX \n", status);
        } else
          LOG_PRINT(" \n set ae scan enable success \n");

      } break;

      case RSI_APP_EVENT_DISCONNECTED: {
        //! remote device disconnected event

        //! clear the disconnected event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_DISCONNECTED);
        memset(&rsi_app_enhanced_connected_device, 0, sizeof(rsi_ble_event_enhance_conn_status_t));

        /**************************************************/
        /************ Extended Advertising Enable *********/
        /**************************************************/

        device_found = 0;
        // AE scan enable
        //rsi_ble_ae_set_scan_enable_t ae_set_scan_enable = { 0 };
        ae_set_scan_enable.enable            = RSI_BLE_START_SCAN;
        ae_set_scan_enable.filter_duplicates = BLE_SCAN_ENABLE_FILTER_DUP;
        ae_set_scan_enable.duration          = BLE_SCAN_DUR;
        ae_set_scan_enable.period            = BLE_SCAN_PERIOD;
        status                               = rsi_ble_ae_set_scan_enable(&ae_set_scan_enable);
        if (status != RSI_SUCCESS) {
          LOG_PRINT(" \n set ae scan enable failed with 0x%lX \n", status);
        } else
          LOG_PRINT(" \n set ae scan enable success \n");
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
  rsi_task_handle_t bt_task_handle = NULL;
#endif

#ifndef RSI_WITH_OS
  //Start BT Stack
  intialize_bt_stack(STACK_BTLE_MODE);

  //! Call BLE Peripheral application
  status = rsi_ble_central();

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
  rsi_task_create((rsi_task_function_t)(int32_t)rsi_ble_central,
                  (uint8_t *)"ble_task",
                  RSI_BT_TASK_STACK_SIZE,
                  NULL,
                  RSI_BT_TASK_PRIORITY,
                  &bt_task_handle);

  //! OS TAsk Start the scheduler
  rsi_start_os_scheduler();

  return status;
#endif
}

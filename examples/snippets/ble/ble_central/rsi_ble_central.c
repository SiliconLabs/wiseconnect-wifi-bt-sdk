/*******************************************************************************
* @file  rsi_ble_central.c
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
 * @brief : This file contains example application for BLE Simple Central
 * @section Description :
 * This application demonstrates how to connect with remote BLE device in BLE
 * central mode.
 =================================================================================*/

/**
 * Include files
 * */

//! BLE include file to refer BLE APIs
#include <rsi_ble_apis.h>
#include <rsi_ble_config.h>
#include <rsi_bt_common_apis.h>
#include <rsi_bt_common.h>
#include "rsi_driver.h"
#include <string.h>
//! Common include file
#include <rsi_common_apis.h>

#ifdef RSI_M4_INTERFACE
#include "rsi_board.h"
#include "rsi_driver.h"
#include "rsi_wisemcu_hardware_setup.h"
#include "rsi_m4.h"
#include "rsi_ps_ram_func.h"
#include "rsi_ds_timer.h"
#endif

#ifdef FW_LOGGING_ENABLE
//! Firmware logging includes
#include "sl_fw_logging.h"
#endif

//! the below array is used to compare the data in the controller for the adv report.
uint8_t adv_payload_for_compare[31] = { 0x6E, 0xC5, 0xFD, 0x05, 0x54, 0x9E, 0x68, 0xF8, 0xD0, 0x05 };

//! Address type of the device to connect
#define RSI_BLE_DEV_ADDR_TYPE LE_PUBLIC_ADDRESS

//! Address of the device to connect
#define RSI_BLE_DEV_ADDR "00:1E:7C:25:E9:4D"

//! Remote Device Name to connect
#define RSI_REMOTE_DEVICE_NAME "SILABS_DEV"

#ifdef FW_LOGGING_ENABLE
//! Memory length of driver updated for firmware logging
#define BT_GLOBAL_BUFF_LEN (15000 + (FW_LOG_QUEUE_SIZE * MAX_FW_LOG_MSG_LEN))
#else
//! Memory length for the driver
#define BT_GLOBAL_BUFF_LEN 15000
#endif

//! Memory to initialize the driver
uint8_t global_buf[BT_GLOBAL_BUFF_LEN];

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

/*=======================================================================*/
//!    Powersave configurations
/*=======================================================================*/
#define ENABLE_POWER_SAVE 0 //! Set to 1 for powersave mode

#if ENABLE_POWER_SAVE
//! Power Save Profile Mode
#define PSP_MODE RSI_SLEEP_MODE_2
//! Power Save Profile type
#define PSP_TYPE RSI_MAX_PSP
#endif

//! Application global parameters.
// static uint8_t rsi_app_async_event_map = 0;
static uint8_t remote_addr_type = 0;
static uint8_t remote_name[31];
static uint8_t remote_dev_addr[18]   = { 0 };
static uint8_t remote_dev_bd_addr[6] = { 0 };
static uint8_t device_found          = 0;

static rsi_ble_event_conn_status_t rsi_app_connected_device;
static rsi_ble_event_disconnect_t rsi_app_disconnected_device;
rsi_semaphore_handle_t ble_peripheral_conn_sem;
uint8_t str_remote_address[18] = { '\0' };

rsi_semaphore_handle_t ble_main_task_sem;
static uint32_t ble_app_event_map;
static uint32_t ble_app_event_map1;

/*================================================================================================*/
void main_loop(void);
int32_t rsi_ble_central(void);
void rsi_ble_simple_central_on_disconnect_event(rsi_ble_event_disconnect_t *resp_disconnect, uint16_t reason);
void rsi_ble_simple_central_on_conn_status_event(rsi_ble_event_conn_status_t *resp_conn);
void rsi_ble_simple_central_on_enhance_conn_status_event(rsi_ble_event_enhance_conn_status_t *resp_enh_conn);
void rsi_ble_simple_central_on_adv_report_event(rsi_ble_event_adv_report_t *adv_report);
void rsi_ble_app_set_event(uint32_t event_num);

/**
 * @fn         rsi_ble_only_Trigger_M4_Sleep
 * @brief      Keeps the M4 In the Sleep
 * @param[in]  none
 * @return    none.
 * @section description
 * This function is used to trigger sleep in the M4 and in the case of the retention submitting the buffer valid
 * to the TA for the rx packets.
 */
#if M4_POWERSAVE_ENABLE
void rsi_ble_only_Trigger_M4_Sleep(void)
{
  /* Configure Wakeup-Source */
  RSI_PS_SetWkpSources(WIRELESS_BASED_WAKEUP);
  /* sets the priority of an Wireless wakeup interrupt. */
  NVIC_SetPriority(WIRELESS_WAKEUP_IRQHandler, WIRELESS_WAKEUP_IRQ_PRI);
  NVIC_EnableIRQ(WIRELESS_WAKEUP_IRQHandler);

#ifndef FLASH_BASED_EXECUTION_ENABLE
  /* LDOSOC Default Mode needs to be disabled */
  RSI_PS_LdoSocDefaultModeDisable();

  /* bypass_ldorf_ctrl needs to be enabled */
  RSI_PS_BypassLdoRfEnable();

  RSI_PS_FlashLdoDisable();

  /* Configure RAM Usage and Retention Size */
  RSI_WISEMCU_ConfigRamRetention(WISEMCU_48KB_RAM_IN_USE, WISEMCU_RETAIN_DEFAULT_RAM_DURING_SLEEP);

  /* Trigger M4 Sleep */
  RSI_WISEMCU_TriggerSleep(SLEEP_WITH_RETENTION,
                           DISABLE_LF_MODE,
                           0,
                           (uint32_t)RSI_PS_RestoreCpuContext,
                           0,
                           RSI_WAKEUP_WITH_RETENTION_WO_ULPSS_RAM);

#else

#ifdef COMMON_FLASH_EN
  M4SS_P2P_INTR_SET_REG &= ~BIT(3);
#endif
  /* Configure RAM Usage and Retention Size */
  RSI_WISEMCU_ConfigRamRetention(WISEMCU_192KB_RAM_IN_USE, WISEMCU_RETAIN_DEFAULT_RAM_DURING_SLEEP);

  RSI_WISEMCU_TriggerSleep(SLEEP_WITH_RETENTION,
                           DISABLE_LF_MODE,
                           WKP_RAM_USAGE_LOCATION,
                           (uint32_t)RSI_PS_RestoreCpuContext,
                           IVT_OFFSET_ADDR,
                           RSI_WAKEUP_FROM_FLASH_MODE);
#endif
#ifdef RSI_WITH_OS
  /*  Setup the systick timer */
  vPortSetupTimerInterrupt();
#endif
#ifdef DEBUG_UART
  fpuInit();
  DEBUGINIT();
#endif
}
#endif
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
  ble_app_event_map  = 0;
  ble_app_event_map1 = 0;
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
void rsi_ble_app_set_event(uint32_t event_num)
{

  if (event_num < 32) {
    ble_app_event_map |= BIT(event_num);
  } else {
    ble_app_event_map1 |= BIT((event_num - 32));
  }

  rsi_semaphore_post(&ble_main_task_sem);

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

  if (event_num < 32) {
    ble_app_event_map &= ~BIT(event_num);
  } else {
    ble_app_event_map1 &= ~BIT((event_num - 32));
  }

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

  for (ix = 0; ix < 64; ix++) {
    if (ix < 32) {
      if (ble_app_event_map & (1 << ix)) {
        return (int32_t)ix;
      }
    } else {
      if (ble_app_event_map1 & (1 << (ix - 32))) {
        return (int32_t)ix;
      }
    }
  }

  return (-1);
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

  memset(remote_name, 0, 31);
  BT_LE_ADPacketExtract(remote_name, adv_report->adv_data, adv_report->adv_data_len);

  remote_addr_type = adv_report->dev_addr_type;
  rsi_6byte_dev_address_to_ascii(remote_dev_addr, (uint8_t *)adv_report->dev_addr);
  memcpy(remote_dev_bd_addr, (uint8_t *)adv_report->dev_addr, 6);
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
  rsi_app_connected_device.dev_addr_type = resp_enh_conn->dev_addr_type;
  memcpy(rsi_app_connected_device.dev_addr, resp_enh_conn->dev_addr, RSI_DEV_ADDR_LEN);
  rsi_app_connected_device.status = resp_enh_conn->status;
  //! unblock connection semaphore
  rsi_semaphore_post(&ble_peripheral_conn_sem);
  rsi_ble_app_set_event(RSI_APP_EVENT_CONNECTED);
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
  //! unblock connection semaphore
  rsi_semaphore_post(&ble_peripheral_conn_sem);
  rsi_ble_app_set_event(RSI_APP_EVENT_CONNECTED);
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
  LOG_PRINT("\r\n Disconnected reason:0x%x \n", reason);
  rsi_ble_app_set_event(RSI_APP_EVENT_DISCONNECTED);
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
  uint8_t fmversion[20]   = { 0 };
#ifdef RSI_BLE_ENABLE_ACCEPTLIST_BASEDON_ADV_PAYLOAD
  uint8_t compare[31];
#endif
#ifdef RSI_WITH_OS
  rsi_task_handle_t driver_task_handle = NULL;
#endif
#ifdef FW_LOGGING_ENABLE
  //Fw log component level
  sl_fw_log_level_t fw_component_log_level;
#endif
#ifndef RSI_WITH_OS
  //! Driver initialization
  status = rsi_driver_init(global_buf, BT_GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > BT_GLOBAL_BUFF_LEN)) {
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
#if M4_POWERSAVE_ENABLE

  //RSI_PS_FlashLdoEnable();
  /* MCU Hardware Configuration for Low-Power Applications */
  RSI_WISEMCU_HardwareSetup();
  LOG_PRINT("\r\nRSI_WISEMCU_HardwareSetup Success\r\n");

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
  rsi_semaphore_create(&ble_main_task_sem, 0);

  //! initialize the event map
  rsi_ble_app_init_events();

#ifdef RSI_BLE_ENABLE_ACCEPTLIST_BASEDON_ADV_PAYLOAD
  memset(compare, 0, 31);
  memcpy(&compare[RSI_BLE_ADV_PAYLOAD_INDEX_FOR_COMPARE],
         adv_payload_for_compare,
         RSI_BLE_ADV_PAYLOAD_LENGTH_FROM_INDEX_TO_COMPARE);

  status = rsi_ble_accept_list_using_adv_data(RSI_BLE_ENABLE_ACCEPTLIST_BASEDON_ADV_PAYLOAD,
                                              RSI_BLE_ADV_PAYLOAD_INDEX_FOR_COMPARE,
                                              RSI_BLE_ADV_PAYLOAD_LENGTH_FROM_INDEX_TO_COMPARE,
                                              compare);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("status: 0x%x\r\n", status);
    return status;
  }
#endif
  rsi_semaphore_create(&ble_peripheral_conn_sem, 0);
  //! start scanning
  status = rsi_ble_start_scanning();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("start_scanning status: 0x%lX\r\n", status);
    return status;
  }

#if ENABLE_POWER_SAVE

  LOG_PRINT("\r\n Initiate module in to power save \r\n");
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

  LOG_PRINT("\r\n Module is in power save \r\n");
#endif

#if M4_POWERSAVE_ENABLE
  P2P_STATUS_REG &= ~M4_wakeup_TA;
  // LOG_PRINT("\n RSI_BLE_REQ_PWRMODE\n ");
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
#if M4_POWERSAVE_ENABLE
      //! if events are not received loop will be continued.
      if ((!(P2P_STATUS_REG & TA_wakeup_M4)) && (!rsi_driver_cb->scheduler_cb.event_map)) {
        P2P_STATUS_REG &= ~M4_wakeup_TA;
        rsi_ble_only_Trigger_M4_Sleep();
      }
#else
      {
        rsi_semaphore_wait(&ble_main_task_sem, 0);
      }
#endif
      continue;
    }

    switch (temp_event_map) {
      case RSI_APP_EVENT_ADV_REPORT: {
        //! advertise report event.

        //! clear the advertise report event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_ADV_REPORT);
        LOG_PRINT("\r\nIn Advertising Event\r\n");

        //! initiate stop scanning command.
        status = rsi_ble_stop_scanning();
        if (status != RSI_SUCCESS) {
          return status;
        }

        status = rsi_ble_connect(remote_addr_type, (int8_t *)remote_dev_bd_addr);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("connect status: 0x%lX\r\n", status);
        }
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
      } break;

      case RSI_APP_EVENT_CONNECTED: {
        //! remote device connected event

        //! clear the connected event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_CONNECTED);
        rsi_6byte_dev_address_to_ascii(str_remote_address, rsi_app_connected_device.dev_addr);
        LOG_PRINT("\r\n Module connected to address : %s \r\n", str_remote_address);
      } break;

      case RSI_APP_EVENT_DISCONNECTED: {
        //! remote device disconnected event

        //! clear the disconnected event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_DISCONNECTED);
        LOG_PRINT("\r\nModule got Disconnected\r\n");

#if ENABLE_POWER_SAVE

        LOG_PRINT("\r\n keep module in to active state \r\n");
        //! initiating Active mode in BT mode
        status = rsi_bt_power_save_profile(RSI_ACTIVE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n Failed to keep Module in ACTIVE mode \r\n");
          return status;
        }

        //! initiating Active mode in WLAN mode
        status = rsi_wlan_power_save_profile(RSI_ACTIVE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n Failed to keep Module in ACTIVE mode \r\n");
          return status;
        }

#endif

        device_found = 0;
        LOG_PRINT("\r\n Restarted Scanning\r\n");
        //! start scanning
        status = rsi_ble_start_scanning();
        if (status != RSI_SUCCESS) {
          LOG_PRINT("start_scanning status: 0x%lX\r\n", status);
        }
#if ENABLE_POWER_SAVE

        LOG_PRINT("\r\n keep module in to power save \r\n");
        status = rsi_bt_power_save_profile(PSP_MODE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          return status;
        }

        status = rsi_wlan_power_save_profile(PSP_MODE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n Failed to keep Module in power save \r\n");
          return status;
        }
        LOG_PRINT("\r\n Module is in power save \r\n");
#endif
      } break;
      default: {
      }
    }
  }
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

  //! Call BLE Central application
  status = rsi_ble_central();

  //! Application main loop
  main_loop();

  return status;
#endif
#ifdef RSI_WITH_OS
  //! Driver initialization
  status = rsi_driver_init(global_buf, BT_GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > BT_GLOBAL_BUFF_LEN)) {
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

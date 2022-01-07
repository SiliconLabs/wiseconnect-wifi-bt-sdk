/*******************************************************************************
 * @file  rsi_ble_power_save.c
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
 * Include files
 * */

//! BLE include file to refer BLE APIs
#include <rsi_ble_apis.h>
#include <rsi_ble_config.h>
#include <rsi_bt_common_apis.h>
#include <rsi_bt_common.h>
#include <rsi_common_apis.h>
#include <rsi_driver.h>
#include <string.h>

/*=======================================================================*/
//   ! MACROS
/*=======================================================================*/
//! Maximum number of advertise reports to hold
#define NO_OF_ADV_REPORTS 10

//! Memory length for driver
#define GLOBAL_BUFF_LEN 15000

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
//   ! GLOBAL VARIABLES
/*=======================================================================*/
//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN] = { 0 };
uint8_t device_found                = 0;
uint8_t remote_dev_addr[18]         = { 0 };
uint8_t remote_dev_bd_addr[6]       = { 0 };
static uint8_t remote_name[31];
static uint8_t remote_addr_type                                 = 0;
static rsi_bt_resp_get_local_name_t rsi_app_resp_get_local_name = { 0 };
static uint8_t rsi_app_resp_get_dev_addr[RSI_DEV_ADDR_LEN]      = { 0 };
static uint8_t rsi_app_async_event_map                          = 0;
static rsi_ble_event_conn_status_t rsi_app_connected_device     = { 0 };
static rsi_ble_event_disconnect_t rsi_app_disconnected_device   = { 0 };
uint8_t rsi_ble_states_bitmap;

rsi_semaphore_handle_t ble_slave_conn_sem;
rsi_semaphore_handle_t ble_main_task_sem;
static uint32_t ble_app_event_map;
static uint32_t ble_app_event_map1;
/*=======================================================================*/
//   ! EXTERN VARIABLES
/*=======================================================================*/

/*=======================================================================*/
//   ! EXTERN FUNCTIONS
/*=======================================================================*/

/*========================================================================*/
//!  CALLBACK FUNCTIONS
/*=======================================================================*/

/*=======================================================================*/
//   ! PROCEDURES
/*=======================================================================*/
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
        return ix;
      }
    } else {
      if (ble_app_event_map1 & (1 << (ix - 32))) {
        return ix;
      }
    }
  }

  return (-1);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_adv_report_event
 * @brief      invoked when advertise report event is received
 * @param[in]  adv_report, pointer to the received advertising report
 * @return     none.
 * @section description
 * This callback function updates the scanned remote devices list
 */
void rsi_ble_on_adv_report_event(rsi_ble_event_adv_report_t *adv_report)
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
 * @fn         rsi_ble_on_connect_event
 * @brief      invoked when connection complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
void rsi_ble_on_connect_event(rsi_ble_event_conn_status_t *resp_conn)
{
  memcpy(&rsi_app_connected_device, resp_conn, sizeof(rsi_ble_event_conn_status_t));
  rsi_ble_app_set_event(RSI_APP_EVENT_CONNECTED);
  //! unblock connection semaphore
  rsi_semaphore_post(&ble_slave_conn_sem);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_disconnect_event
 * @brief      invoked when disconnection event is received
 * @param[in]  resp_disconnect, disconnected remote device information
 * @param[in]  reason, reason for disconnection.
 * @return     none.
 * @section description
 * This callback function indicates disconnected device information and status
 */
void rsi_ble_on_disconnect_event(rsi_ble_event_disconnect_t *resp_disconnect, uint16_t reason)
{
  memcpy(&rsi_app_disconnected_device, resp_disconnect, sizeof(rsi_ble_event_disconnect_t));
  //! Comparing Remote slave bd address to check the scan bitmap
  if (!(memcmp(remote_dev_bd_addr, (uint8_t *)resp_disconnect->dev_addr, 6))) {
    CLR_BIT(rsi_ble_states_bitmap, RSI_SCAN_STATE);
  } else {
    CLR_BIT(rsi_ble_states_bitmap, RSI_ADV_STATE);
  }
  rsi_ble_app_set_event(RSI_APP_EVENT_DISCONNECTED);
}

/*==============================================*/
/**
 * @fn         rsi_ble_on_enhance_conn_status_event
 * @brief      invoked when enhanced connection complete event is received
 * @param[out] resp_conn, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
void rsi_ble_on_enhance_conn_status_event(rsi_ble_event_enhance_conn_status_t *resp_enh_conn)
{
  rsi_app_connected_device.dev_addr_type = resp_enh_conn->dev_addr_type;
  memcpy(rsi_app_connected_device.dev_addr, resp_enh_conn->dev_addr, RSI_DEV_ADDR_LEN);
  rsi_app_connected_device.status = resp_enh_conn->status;
  rsi_ble_app_set_event(RSI_APP_EVENT_CONNECTED);
  //! unblock connection semaphore
  rsi_semaphore_post(&ble_slave_conn_sem);
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_task
 * @brief      Tests the BLE GAP peripheral role.
 * @param[in]  none
 * @return    none.
 * @section description
 * This function is used to test the BLE peripheral/central role and simple GAP API's.
 */
int32_t rsi_ble_app_task(void)
{
  int32_t status          = 0;
  int32_t temp_event_map  = 0;
  int32_t temp_event_map1 = 0;

#if ((BLE_ROLE == SLAVE_MODE) || (BLE_ROLE == DUAL_MODE))
  uint8_t adv[31] = { 2, 1, 6 };
#endif
#ifdef RSI_WITH_OS
  rsi_task_handle_t driver_task_handle = NULL;
#endif
#ifndef RSI_WITH_OS
  //! Driver initialization
  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
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
  //! Silabs module initialisation
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nDevice Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nDevice Initialization Success\r\n");
  }

  //! Task created for Driver task
  rsi_task_create((rsi_task_function_t)rsi_wireless_driver_task,
                  "driver_task",
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

  //! BLE register GAP callbacks
  rsi_ble_gap_register_callbacks(rsi_ble_on_adv_report_event,
                                 rsi_ble_on_connect_event,
                                 rsi_ble_on_disconnect_event,
                                 NULL,
                                 NULL,
                                 NULL,
                                 rsi_ble_on_enhance_conn_status_event,
                                 NULL,
                                 NULL,
                                 NULL);
  //! create ble main task if ble protocol is selected
  rsi_semaphore_create(&ble_main_task_sem, 0);
  //! initialize the event map
  rsi_ble_app_init_events();

  //! get the local device address(MAC address).
  status = rsi_bt_get_local_device_address(rsi_app_resp_get_dev_addr);
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! set the local device name
  status = rsi_bt_set_local_name((uint8_t *)RSI_BLE_LOCAL_NAME);
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! get the local device name
  status = rsi_bt_get_local_name(&rsi_app_resp_get_local_name);
  if (status != RSI_SUCCESS) {
    return status;
  }

#if ((BLE_ROLE == SLAVE_MODE) || (BLE_ROLE == DUAL_MODE))
  //! prepare advertise data //local/device name
  adv[3] = strlen(RSI_BLE_LOCAL_NAME) + 1;
  adv[4] = 9;
  strcpy((char *)&adv[5], RSI_BLE_LOCAL_NAME);

  //! set advertise data
  rsi_ble_set_advertise_data(adv, strlen(RSI_BLE_LOCAL_NAME) + 5);

  rsi_semaphore_create(&ble_slave_conn_sem, 0);

  //! start the advertising
  LOG_PRINT("\n Start advertising \n");
  status = rsi_ble_start_advertising();
  if (status != RSI_SUCCESS) {
    return status;
  }
  SET_BIT(rsi_ble_states_bitmap, RSI_ADV_STATE);
#endif

#if ((BLE_ROLE == MASTER_MODE) || (BLE_ROLE == DUAL_MODE))
  //! start scanning
  LOG_PRINT("\n Start scanning \n");
  status = rsi_ble_start_scanning();
  if (status != RSI_SUCCESS) {
    return status;
  }
  SET_BIT(rsi_ble_states_bitmap, RSI_SCAN_STATE);
#endif

  LOG_PRINT("\n Initiate module in to power save \n");

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

  LOG_PRINT("\n Module is in power save \n");

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
      rsi_semaphore_wait(&ble_main_task_sem, 0);
      continue;
    }

    //! if any event is received, it will be served.
    switch (temp_event_map) {
      case RSI_APP_EVENT_ADV_REPORT: {
        //! clear the advertise report event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_ADV_REPORT);
        //! advertise report event.
        //! initiate stop scanning command.
        status = rsi_ble_stop_scanning();
        if (status != RSI_SUCCESS) {
          return status;
        }

        //! initiating the connection with remote BLE device
        status = rsi_ble_connect(remote_addr_type, (int8_t *)remote_dev_bd_addr);
        if (status != RSI_SUCCESS) {
          return status;
        }

        rsi_semaphore_wait(&ble_slave_conn_sem, 10000);
        temp_event_map1 = rsi_ble_app_get_event();

        if ((temp_event_map1 == -1) || (!(temp_event_map1 & RSI_APP_EVENT_CONNECTED))) {
          LOG_PRINT("\r\n Initiating connect cancel command \n");
          status = rsi_ble_connect_cancel((int8_t *)remote_dev_bd_addr);
          if (status != RSI_SUCCESS) {
            LOG_PRINT("\r\n ble connect cancel cmd status = %x \n", status);
          } else {
            rsi_ble_app_set_event(RSI_APP_EVENT_DISCONNECTED);
          }
        }

      } break;
      case RSI_APP_EVENT_CONNECTED: {
        //! remote device connected event
        LOG_PRINT("\n connection is success .............\n");
        //! clear the connected event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_CONNECTED);

      } break;
      case RSI_APP_EVENT_DISCONNECTED: {
        //! remote device disconnected event
        LOG_PRINT("\n In disconnect event............ \n ");
        //! clear the disconnected event.
        rsi_ble_app_clear_event(RSI_APP_EVENT_DISCONNECTED);

        LOG_PRINT("\n keep module in to active state \n");

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
        if (((BLE_ROLE == SLAVE_MODE) || (BLE_ROLE == DUAL_MODE))
            && (!(CHK_BIT(rsi_ble_states_bitmap, RSI_ADV_STATE)))) {
          //! set device in advertising mode.
          LOG_PRINT("\n Start advertising \n");
adv:
          status = rsi_ble_start_advertising();
          if (status != RSI_SUCCESS) {
            goto adv;
          }
          SET_BIT(rsi_ble_states_bitmap, RSI_ADV_STATE);
        }
        if (((BLE_ROLE == MASTER_MODE) || (BLE_ROLE == DUAL_MODE))
            && (!(CHK_BIT(rsi_ble_states_bitmap, RSI_SCAN_STATE)))) {
          device_found = 0;
          //! set device in scanning mode.
          LOG_PRINT("\n Start scanning \n");
scan:
          status = rsi_ble_start_scanning();
          if (status != RSI_SUCCESS) {
            goto scan;
          }
          SET_BIT(rsi_ble_states_bitmap, RSI_SCAN_STATE);
        }
        LOG_PRINT("\n keep module in to power save \n");
        status = rsi_bt_power_save_profile(PSP_MODE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          return status;
        }

        status = rsi_wlan_power_save_profile(PSP_MODE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n Failed to keep Module in power save \r\n");
          return status;
        }
        LOG_PRINT("\n Module is in power save \n");
      } break;
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
  rsi_task_handle_t ble_task_handle = NULL;
#endif

#ifndef RSI_WITH_OS

  //Start BT Stack
  intialize_bt_stack(STACK_BTLE_MODE);

  //! Call BLE Peripheral application
  status = rsi_ble_app_task();

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

  //Start BT Stack
  intialize_bt_stack(STACK_BTLE_MODE);

  //! OS case
  //! Task created for BLE task
  rsi_task_create((rsi_task_function_t)rsi_ble_app_task,
                  "ble_task",
                  RSI_BT_TASK_STACK_SIZE,
                  NULL,
                  RSI_BT_TASK_PRIORITY,
                  &ble_task_handle);

  //! OS TAsk Start the scheduler
  rsi_start_os_scheduler();

  return status;
#endif
}

/*******************************************************************************
* @file  rsi_bt_power_save_profile.c
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

//! BT include file to refer BT APIs
#include <rsi_bt_apis.h>
#include <rsi_bt_common_apis.h>
#include <rsi_bt_common.h>
#include <rsi_bt_config.h>
#include <rsi_bt.h>
#include <stdio.h>

//! Common include file
#include <rsi_common_apis.h>
#include "rsi_driver.h"
//! application defines

#define SPP_SLAVE  0
#define SPP_MASTER 1

#define SPP_MODE SPP_SLAVE

#if (SPP_MODE == SPP_MASTER)
#define RSI_BT_LOCAL_NAME "BT_SPP_MASTER_POWER_SAVE"
#else
#define RSI_BT_LOCAL_NAME "BT_SPP_SLAVE_POWER_SAVE"
#endif

#define PIN_CODE       "4321"
#define REMOTE_BD_ADDR "20:f4:78:55:40:5b"

//! application events list
#define RSI_APP_EVENT_CONNECTED    1
#define RSI_APP_EVENT_PINCODE_REQ  2
#define RSI_APP_EVENT_LINKKEY_SAVE 3
#define RSI_APP_EVENT_AUTH_COMPLT  4
#define RSI_APP_EVENT_DISCONNECTED 5
#define RSI_APP_EVENT_LINKKEY_REQ  6
#define RSI_APP_EVENT_SPP_CONN     7
#define RSI_APP_EVENT_SPP_DISCONN  8
#define RSI_APP_EVENT_SPP_RX       9
/** ssp related defines********/
#define RSI_APP_EVENT_PASSKEY_DISPLAY 10
#define RSI_APP_EVENT_PASSKEY_REQUEST 11
#define RSI_APP_EVENT_SSP_COMPLETE    12
#define RSI_APP_EVENT_CONFIRM_REQUEST 13
/*** SNIFF RELATED DEFINES********/
#define RSI_APP_EVENT_MODE_CHANGED    14
#define RSI_APP_EVENT_SNIFF_SUBRATING 15

#define RSI_APP_EVENT_UNBOND_STATUS 16
//! Memory length for driver
#define GLOBAL_BUFF_LEN 15000

//! Power Save Profile Mode
#define PSP_TYPE RSI_MAX_PSP

//! Sniff Parameters
#define SNIFF_MAX_INTERVAL 0xA0
#define SNIFF_MIN_INTERVAL 0XA0
#define SNIFF_ATTEMPT      0X04
#define SNIFF_TIME_OUT     0X02

#ifdef RSI_WITH_OS
//! BLE task stack size
#define RSI_BT_TASK_STACK_SIZE 3000

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
//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN] = { 0 };

//! Application global parameters.
static uint32_t rsi_app_async_event_map        = 0;
static rsi_bt_resp_get_local_name_t local_name = { 0 };
static uint8_t str_conn_bd_addr[18];
static uint8_t remote_dev_addr[RSI_DEV_ADDR_LEN] = { 0 };
static uint8_t local_dev_addr[RSI_DEV_ADDR_LEN]  = { 0 };
static uint8_t data[RSI_BT_MAX_PAYLOAD_SIZE];
static uint16_t data_len;
static uint8_t linkkey[RSI_LINKKEY_REPLY_SIZE];
uint8_t auth_resp_status = 0;
uint8_t sniff_mode;
uint8_t power_save_given = 0;

/*==============================================*/
/**
 * @fn         rsi_bt_app_init_events
 * @brief      initializes the event parameter.
 * @param[in]  none.
 * @return     none.
 * @section description
 * This function is used during BT initialization.
 */
static void rsi_bt_app_init_events()
{
  rsi_app_async_event_map = 0;
  return;
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_set_event
 * @brief      sets the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to set/raise the specific event.
 */
static void rsi_bt_app_set_event(uint32_t event_num)
{
  rsi_app_async_event_map |= BIT(event_num);
  return;
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_clear_event
 * @brief      clears the specific event.
 * @param[in]  event_num, specific event number.
 * @return     none.
 * @section description
 * This function is used to clear the specific event.
 */
static void rsi_bt_app_clear_event(uint32_t event_num)
{
  rsi_app_async_event_map &= ~BIT(event_num);
  return;
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_get_event
 * @brief      returns the first set event based on priority
 * @param[in]  none.
 * @return     int32_t
 *             > 0  = event number
 *             -1   = not received any event
 * @section description
 * This function returns the highest priority event among all the set events
 */
static int32_t rsi_bt_app_get_event(void)
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
 * @fn         rsi_bt_app_on_conn
 * @brief      invoked when connection complete event is received
 * @param[out] resp_status, connection status of the connected device.
 * @param[out] conn_event, connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the status of the connection
 */
void rsi_bt_app_on_conn(uint16_t resp_status, rsi_bt_event_bond_t *conn_event)
{
  if (resp_status == 0) {
    rsi_bt_app_set_event(RSI_APP_EVENT_CONNECTED);
  } else {
    rsi_bt_app_set_event(RSI_APP_EVENT_DISCONNECTED);
  }
  memcpy((int8_t *)remote_dev_addr, conn_event->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("on_conn: str_conn_bd_addr: %s, resp_status: 0x%x\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, conn_event->dev_addr),
            resp_status);
}
void rsi_bt_on_unbond_status(uint16_t resp_status, rsi_bt_event_unbond_t *unbond_status)
{
  rsi_bt_app_set_event(RSI_APP_EVENT_UNBOND_STATUS);
  memcpy((int8_t *)remote_dev_addr, unbond_status->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("on unbond status event: str_conn_bd_addr: %s\r\n",
            rsi_6byte_dev_address_to_ascii((uint8_t *)str_conn_bd_addr, unbond_status->dev_addr));
  LOG_PRINT("\n Unbond status reason:0x%x\n", resp_status);
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_pincode_req
 * @brief      invoked when pincode request event is received
 * @param[out] resp_status, connection status of the connected device.
 * @param[out] user_pincode_request, pairing remote device information
 * @return     none.
 * @section description
 * This callback function indicates the pincode request from remote device
 */
void rsi_bt_app_on_pincode_req(uint16_t resp_status, rsi_bt_event_user_pincode_request_t *user_pincode_request)
{
  UNUSED_PARAMETER(resp_status); //This statement is added only to resolve compilation warning, value is unchanged
  rsi_bt_app_set_event(RSI_APP_EVENT_PINCODE_REQ);
  memcpy((int8_t *)remote_dev_addr, user_pincode_request->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("on_pin_code_req: str_conn_bd_addr: %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, user_pincode_request->dev_addr));
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_on_linkkey_req
 * @brief      invoked when linkkey request event is received
 * @param[out] status, connection status of the connected device.
 * @param[out] user_linkkey_req, pairing remote device information
 * @return     none.
 * @section description
 * This callback function indicates the linkkey request from remote device
 */
void rsi_ble_app_on_linkkey_req(uint16_t status, rsi_bt_event_user_linkkey_request_t *user_linkkey_req)
{
  UNUSED_PARAMETER(status); //This statement is added only to resolve compilation warning, value is unchanged
  rsi_bt_app_set_event(RSI_APP_EVENT_LINKKEY_REQ);
  memcpy((int8_t *)remote_dev_addr, user_linkkey_req->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("linkkey_req: str_conn_bd_addr: %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, user_linkkey_req->dev_addr));
}

/*==============================================*/
/**
 * @fn         rsi_ble_app_on_linkkey_save
 * @brief      invoked when linkkey save event is received
 * @param[out] status, connection status of the connected device.
 * @param[out] user_linkkey_req, paired remote device information
 * @return     none.
 * @section description
 * This callback function indicates the linkkey save from local device
 */
void rsi_ble_app_on_linkkey_save(uint16_t status, rsi_bt_event_user_linkkey_save_t *user_linkkey_save)
{
  UNUSED_PARAMETER(status); //This statement is added only to resolve compilation warning, value is unchanged
  rsi_bt_app_set_event(RSI_APP_EVENT_LINKKEY_SAVE);
  memcpy((int8_t *)remote_dev_addr, user_linkkey_save->dev_addr, RSI_DEV_ADDR_LEN);
  memcpy(linkkey, user_linkkey_save->linkKey, RSI_LINKKEY_REPLY_SIZE);
  LOG_PRINT("linkkey_save: str_conn_bd_addr: %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, user_linkkey_save->dev_addr));
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_auth_complete
 * @brief      invoked when authentication event is received
 * @param[out] resp_status, authentication status
 * @param[out] auth_complete, paired remote device information
 * @return     none.
 * @section description
 * This callback function indicates the pairing status and remote device information
 */
void rsi_bt_app_on_auth_complete(uint16_t resp_status, rsi_bt_event_auth_complete_t *auth_complete)
{
  /* Keeping track of Auth complete response status will be used in unbond event */
  auth_resp_status = resp_status;
  if (resp_status == 0) {
    rsi_bt_app_set_event(RSI_APP_EVENT_AUTH_COMPLT);
  } else {
    memset(linkkey, 0, RSI_LINKKEY_REPLY_SIZE);
    rsi_bt_app_set_event(RSI_APP_EVENT_DISCONNECTED);
  }
  memcpy((int8_t *)remote_dev_addr, auth_complete->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("auth_complete: str_conn_bd_addr: %s, resp_status: 0x%x\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, auth_complete->dev_addr),
            resp_status);
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_disconn
 * @brief      invoked when disconnect event is received
 * @param[out] resp_status, disconnect status/error
 * @param[out] bt_disconnected, disconnected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the disconnected device information
 */
void rsi_bt_app_on_disconn(uint16_t resp_status, rsi_bt_event_disconnect_t *bt_disconnected)
{
  rsi_bt_app_set_event(RSI_APP_EVENT_DISCONNECTED);
  memcpy((int8_t *)remote_dev_addr, bt_disconnected->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("on_disconn: str_conn_bd_addr: %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, bt_disconnected->dev_addr));
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_spp_connect
 * @brief      invoked when spp profile connected event is received
 * @param[out] spp_connect, spp connected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the spp connected remote device information
 */
void rsi_bt_app_on_spp_connect(uint16_t resp_status, rsi_bt_event_spp_connect_t *spp_connect)
{
  rsi_bt_app_set_event(RSI_APP_EVENT_SPP_CONN);
  memcpy((int8_t *)remote_dev_addr, spp_connect->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("spp_conn: str_conn_bd_addr: %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, spp_connect->dev_addr));
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_spp_disconnect
 * @brief      invoked when spp profile disconnected event is received
 * @param[out] spp_disconn, spp disconnected remote device information
 * @return     none.
 * @section description
 * This callback function indicates the spp disconnected event
 */
void rsi_bt_app_on_spp_disconnect(uint16_t resp_status, rsi_bt_event_spp_disconnect_t *spp_disconn)
{
  UNUSED_PARAMETER(resp_status); //This statement is added only to resolve compilation warning, value is unchanged
  rsi_bt_app_set_event(RSI_APP_EVENT_SPP_DISCONN);
  memcpy((int8_t *)remote_dev_addr, spp_disconn->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("spp_disconn: str_conn_bd_addr: %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, spp_disconn->dev_addr));
}

/*==============================================*/
/**
 * @fn         rsi_bt_on_passkey_display
 * @brief      invoked when passkey display event is received
 * @param[out] bt_event_user_passkey_display, remote device & passkey information
 * @return     none.
 * @section description
 * This callback function indicates the passkey display event in ssp pairing process.
 */
void rsi_bt_on_passkey_display(uint16_t resp_status, rsi_bt_event_user_passkey_display_t *bt_event_user_passkey_display)
{
  UNUSED_PARAMETER(resp_status); //This statement is added only to resolve compilation warning, value is unchanged
  uint8_t ix;
  rsi_bt_app_set_event(RSI_APP_EVENT_PASSKEY_DISPLAY);
  //rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, bt_event_user_passkey_display->passkey);
  memcpy(data, bt_event_user_passkey_display->passkey, 4);
  for (ix = 0; ix < 4; ix++) {
    LOG_PRINT(" 0x%02x,", bt_event_user_passkey_display->passkey[ix]);
  }

  LOG_PRINT("\r\n");
  LOG_PRINT("passkey: %ld", *((uint32_t *)bt_event_user_passkey_display->passkey));
  LOG_PRINT("\r\n");
}

/*==============================================*/
/**
 * @fn         rsi_bt_on_passkey_request
 * @brief      invoked when passkey request event is received
 * @param[out] user_passkey_request, remote device information
 * @return     none.
 * @section description
 * This callback function indicates the passkey request event in ssp pairing process,
 * we need to send passkey request reply command
 */
void rsi_bt_on_passkey_request(uint16_t resp_status, rsi_bt_event_user_passkey_request_t *user_passkey_request)
{
  UNUSED_PARAMETER(resp_status); //This statement is added only to resolve compilation warning, value is unchanged
  rsi_bt_app_set_event(RSI_APP_EVENT_PASSKEY_REQUEST);
  memcpy((int8_t *)remote_dev_addr, user_passkey_request->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("passkey_request: str_conn_bd_addr: %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, user_passkey_request->dev_addr));
}

/*==============================================*/
/**
 * @fn         rsi_bt_on_ssp_complete
 * @brief      invoked when ssp pairing process complete event is received
 * @param[out] ssp_complete, spp pairing process status event
 * @return     none.
 * @section description
 * This callback function indicates the ssp pairing process complete,
 */

void rsi_bt_on_ssp_complete(uint16_t resp_status, rsi_bt_event_ssp_complete_t *ssp_complete)
{
  rsi_bt_app_set_event(RSI_APP_EVENT_SSP_COMPLETE);
  memcpy((int8_t *)remote_dev_addr, ssp_complete->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("ssp_complete: str_conn_bd_addr: %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, ssp_complete->dev_addr));
  LOG_PRINT("ssp_complete: resp_status: %d\r\n", resp_status);
}

/*==============================================*/
/**
 * @fn         rsi_bt_on_confirm_request
 * @brief      invoked when passkey confirmation event is received
 * @param[out] user_confirmation_request, passkey and remote device information
 * @return     none.
 * @section description
 * This callback function indicates the passkey confirmation in ssp pairing process.
 * we have respond with +ve/-ve confirmation. 
 */
void rsi_bt_on_confirm_request(uint16_t resp_status,
                               rsi_bt_event_user_confirmation_request_t *user_confirmation_request)
{
  UNUSED_PARAMETER(resp_status); //This statement is added only to resolve compilation warning, value is unchanged
  rsi_bt_app_set_event(RSI_APP_EVENT_CONFIRM_REQUEST);
  LOG_PRINT("\r\n");
  LOG_PRINT("data: %s", user_confirmation_request->confirmation_value);
}

/*==============================================*/
/**
 * @fn         rsi_bt_on_mode_change
 * @brief      invoked when mode is changed event is received
 * @param[out] mode_change, mode related information
 * @return     none.
 * @section description
 * This callback function indicates the mode change in BT device.
 */

void rsi_bt_on_mode_change(uint16_t resp_status, rsi_bt_event_mode_change_t *mode_change)
{

  UNUSED_PARAMETER(resp_status); //This statement is added only to resolve compilation warning, value is unchanged
  sniff_mode = mode_change->current_mode;
  LOG_PRINT("sniff_mode:%d", sniff_mode);
  rsi_bt_app_set_event(RSI_APP_EVENT_MODE_CHANGED);
  memcpy((int8_t *)remote_dev_addr, mode_change->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("mode_change_event: str_conn_bd_addr: %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, mode_change->dev_addr));
}

/*==============================================*/
/**
 * @fn         rsi_bt_on_sniff_subrating
 * @brief      invoked when mode is changed(sniff subrating) event is received
 * @param[out] mode_change, mode related information
 * @return     none.
 * @section description
 * This callback function indicates the mode change in BT device.
 */
void rsi_bt_on_sniff_subrating(uint16_t resp_status, rsi_bt_event_sniff_subrating_t *mode_change)
{
  UNUSED_PARAMETER(resp_status); //This statement is added only to resolve compilation warning, value is unchanged
  rsi_bt_app_set_event(RSI_APP_EVENT_SNIFF_SUBRATING);
  memcpy((int8_t *)remote_dev_addr, mode_change->dev_addr, RSI_DEV_ADDR_LEN);
  LOG_PRINT("mode_change_event: str_conn_bd_addr: %s\r\n",
            rsi_6byte_dev_address_to_ascii(str_conn_bd_addr, mode_change->dev_addr));
}

/*==============================================*/
/**
 * @fn         rsi_bt_app_on_spp_data_rx
 * @brief      invoked when spp data rx event is received
 * @param[out] spp_receive, spp data from remote device
 * @return     none.
 * @section description
 * This callback function indicates the spp data received event
 */
void rsi_bt_app_on_spp_data_rx(uint16_t resp_status, rsi_bt_event_spp_receive_t *spp_receive)
{
  UNUSED_PARAMETER(resp_status); //This statement is added only to resolve compilation warning, value is unchanged
  uint16_t ix;

  rsi_bt_app_set_event(RSI_APP_EVENT_SPP_RX);
  data_len = spp_receive->data_len;
  memset(data, 0, sizeof(data));
  spp_receive->data[spp_receive->data_len] = '\0';
  memcpy(data, spp_receive->data, spp_receive->data_len);
  LOG_PRINT("spp_rx: data_len: %d, data: ", spp_receive->data_len);
  for (ix = 0; ix < spp_receive->data_len; ix++) {
    LOG_PRINT(" 0x%02x,", spp_receive->data[ix]);
  }
  LOG_PRINT("\r\n");
  LOG_PRINT("data: %s", spp_receive->data);
}

void switch_proto_async(uint16_t mode, uint8_t *bt_disabled_status)
{
  UNUSED_PARAMETER(mode); //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(
    bt_disabled_status); //This statement is added only to resolve compilation warning, value is unchanged
  LOG_PRINT("\n IN ASYNC \n");
}

/*==============================================*/
/**
 * @fn         rsi_bt_spp_slave
 * @brief      Tests the BT Classic SPP Slave role.
 * @param[in]  none
 * @return    none.
 * @section description
 * This function is used to test the SPP Slave role.
 */
int32_t rsi_bt_spp_slave(void)
{
  int32_t status          = 0;
  int32_t temp_event_map  = 0;
  uint8_t str_bd_addr[18] = { 0 };
  uint8_t eir_data[200]   = { 2, 1, 0 };

#if (SPP_MODE == SPP_MASTER)
  int32_t tx_ix           = 0;
  uint8_t spp_tx_data_len = 0;
#endif
#ifndef RSI_WITH_OS
  //! Driver initialization
  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }

  //! Redpine module intialisation
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    return status;
  }
#endif

  //! WC initialization
  status = rsi_wireless_init(0, RSI_OPERMODE_WLAN_BT_CLASSIC);

  if (status != RSI_SUCCESS) {
    return status;
  }

  status = rsi_switch_proto(1, NULL);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\n bt enable fail \n");
    //return status;
  }

  //! BT register GAP callbacks:
  rsi_bt_gap_register_callbacks(NULL, //role_change
                                rsi_bt_app_on_conn,
                                rsi_bt_on_unbond_status, //
                                rsi_bt_app_on_disconn,
                                NULL,                      //scan_resp
                                NULL,                      //remote_name_req
                                rsi_bt_on_passkey_display, //passkey_display
                                NULL,                      //remote_name_req+cancel
                                rsi_bt_on_confirm_request, //confirm req
                                rsi_bt_app_on_pincode_req,
                                rsi_bt_on_passkey_request, //passkey request
                                NULL,                      //inquiry complete
                                rsi_bt_app_on_auth_complete,
                                rsi_ble_app_on_linkkey_req, //linkkey request
                                rsi_bt_on_ssp_complete,     //ssp coplete
                                rsi_ble_app_on_linkkey_save,
                                NULL, //get services
                                NULL,
                                rsi_bt_on_mode_change,
                                rsi_bt_on_sniff_subrating, //search service
                                NULL);

  //! initialize the event map
  rsi_bt_app_init_events();

  //! get the local device address(MAC address).
  status = rsi_bt_get_local_device_address(local_dev_addr);
  if (status != RSI_SUCCESS) {
    return status;
  }
  LOG_PRINT("\r\nlocal_bd_address: %s\r\n", rsi_6byte_dev_address_to_ascii(str_bd_addr, local_dev_addr));

  //! set the local device name
  status = rsi_bt_set_local_name((uint8_t *)RSI_BT_LOCAL_NAME);
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! get the local device name
  status = rsi_bt_get_local_name(&local_name);
  if (status != RSI_SUCCESS) {
    return status;
  }
  LOG_PRINT("\r\nlocal_name: %s\r\n", local_name.name);

  //! prepare Extended Response Data
  eir_data[3] = strlen(RSI_BT_LOCAL_NAME) + 1;
  eir_data[4] = 9;
  strcpy((char *)&eir_data[5], RSI_BT_LOCAL_NAME);
  //! set eir data
  rsi_bt_set_eir_data(eir_data, strlen(RSI_BT_LOCAL_NAME) + 5);
#if (SPP_MODE != SPP_MASTER)
  //! start the discover mode
  status = rsi_bt_start_discoverable();
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! start the connectability mode
  status = rsi_bt_set_connectable();
  if (status != RSI_SUCCESS) {
    return status;
  }
#else
  //! stop the discover mode
  status = rsi_bt_stop_discoverable();
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! stop the connectability mode
  status = rsi_bt_set_non_connectable();
  if (status != RSI_SUCCESS) {
    return status;
  }
#endif

  //! initilize the SPP profile
  status = rsi_bt_spp_init();
  if (status != RSI_SUCCESS) {
    return status;
  }

  LOG_PRINT("spp init success\n");
  //! register the SPP profile callback's
  rsi_bt_spp_register_callbacks(rsi_bt_app_on_spp_connect, rsi_bt_app_on_spp_disconnect, rsi_bt_app_on_spp_data_rx);
#if (SPP_MODE == SPP_MASTER)
  status = rsi_bt_connect(rsi_ascii_dev_address_to_6bytes_rev(remote_dev_addr, REMOTE_BD_ADDR));
  if (status != RSI_SUCCESS) {
    return status;
  }
  LOG_PRINT("bt_conn resp is 0x%x \n", status);
#endif
  while (1) {
    //! Application main loop

#ifndef RSI_WITH_OS
    //! run the non OS scheduler
    rsi_wireless_driver_task();
#endif
    //! checking for received events
    temp_event_map = rsi_bt_app_get_event();
    if (temp_event_map == RSI_FAILURE) {
      //! if events are not received loop will be continued.
      continue;
    }

    //! if any event is received, it will be served.
    switch (temp_event_map) {
      case RSI_APP_EVENT_CONNECTED: {
        //! remote device connected event

        //! clear the connected event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_CONNECTED);
      } break;
      case RSI_APP_EVENT_UNBOND_STATUS: {
        //! remote device connected event

        //! clear the connected event.
        LOG_PRINT("\n Unbond status EVENT \n");
        rsi_bt_app_clear_event(RSI_APP_EVENT_UNBOND_STATUS);
#if (SPP_MODE == SPP_MASTER)
        /* Initialize BT connect only if auth resp status is success */
        if (auth_resp_status == 0) {
          status = rsi_bt_connect(rsi_ascii_dev_address_to_6bytes_rev(remote_dev_addr, REMOTE_BD_ADDR));
          if (status != RSI_SUCCESS) {
            return status;
          }
          LOG_PRINT("bt_conn resp is 0x%x \n", status);
        } else {
          /* Clearing auth response status */
          auth_resp_status = 0;
        }
        LOG_PRINT("bt_conn resp is 0x%x \n", status);
#endif
      } break;

      case RSI_APP_EVENT_PINCODE_REQ: {
        //! pincode request event
        uint8_t *pin_code = (uint8_t *)PIN_CODE;

        //! clear the pincode request event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_PINCODE_REQ);

        //! sending the pincode requet reply
        status = rsi_bt_pincode_request_reply((uint8_t *)remote_dev_addr, pin_code, 1);
        if (status != RSI_SUCCESS) {
          return status;
        }
      } break;
      case RSI_APP_EVENT_LINKKEY_SAVE: {
        //! linkkey save event

        //! clear the likkey save event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_LINKKEY_SAVE);
      } break;
      case RSI_APP_EVENT_AUTH_COMPLT: {
        //! authentication complete event

        //! clear the authentication complete event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_AUTH_COMPLT);
#if (SPP_MODE == SPP_MASTER)
        rsi_delay_ms(500);
        status = rsi_bt_spp_connect(remote_dev_addr);
        if (status != RSI_SUCCESS) {
          //return status;
        }
#endif
      } break;
      case RSI_APP_EVENT_DISCONNECTED: {
        //! remote device connected event

        //! clear the disconnected event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_DISCONNECTED);
#if (ENABLE_POWER_SAVE)
        //! initiating Active mode in BLE mode
        status = rsi_bt_power_save_profile(RSI_ACTIVE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n Failed in keep Module in ACTIVE mode \r\n");
        }
        //! initiating Active mode in wlan mode
        status = rsi_wlan_power_save_profile(RSI_ACTIVE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\n Failed in keep Module in ACTIVE mode \r\n");
        }
        LOG_PRINT("In Active mode:\n");
        power_save_given = 0;

#endif
#if (SPP_MODE == SPP_MASTER)
        status = rsi_bt_connect(rsi_ascii_dev_address_to_6bytes_rev(remote_dev_addr, REMOTE_BD_ADDR));
        if (status != RSI_SUCCESS) {
          return status;
        }
        LOG_PRINT("bt_conn resp is 0x%x \n", status);
#endif
      } break;
      case RSI_APP_EVENT_LINKKEY_REQ: {
        //! linkkey request event

        //! clear the linkkey request event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_LINKKEY_REQ);
        if ((linkkey[0] == 0) && (linkkey[15] == 0)) {
          LOG_PRINT("linkkey request negative reply\r\n");
          //! sending the linkkey request negative reply
          rsi_bt_linkkey_request_reply((uint8_t *)remote_dev_addr, NULL, 0);
        } else {
          LOG_PRINT("linkkey request positive reply\r\n");
          //! sending the linkkey request positive reply
          rsi_bt_linkkey_request_reply((uint8_t *)remote_dev_addr, linkkey, 1);
        }
      } break;

      case RSI_APP_EVENT_SPP_CONN: {
        //! spp connected event

        //! clear the spp connected event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_SPP_CONN);

#if (SPP_MODE == SPP_MASTER)
        strcpy((char *)data, "spp_test_sample_1");
        spp_tx_data_len       = strlen((char *)data);
        data[spp_tx_data_len] = (tx_ix++) % 10;
        status                = rsi_bt_spp_transfer(remote_dev_addr, data, strlen(data));
#endif
        /* here we call the sniff_mode command*/
        status =
          rsi_bt_sniff_mode(remote_dev_addr, SNIFF_MAX_INTERVAL, SNIFF_MIN_INTERVAL, SNIFF_ATTEMPT, SNIFF_TIME_OUT);
        if (status != RSI_SUCCESS) {
          return status;
        }

      } break;

      case RSI_APP_EVENT_SPP_DISCONN: {
        //! spp disconnected event

        //! clear the spp disconnected event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_SPP_DISCONN);
      } break;

      case RSI_APP_EVENT_SPP_RX: {
        //! spp receive event

        //! clear the spp receive event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_SPP_RX);

        //! SPP data transfer (loop back)
#if (SPP_MODE != SPP_MASTER)
        rsi_bt_spp_transfer(remote_dev_addr, data, data_len);
#else
        strcpy((char *)data, "spp_test_sample_1");
        spp_tx_data_len       = strlen((char *)data);
        data[spp_tx_data_len] = (tx_ix++) % 10;
        LOG_PRINT("tx_ix: %d\r\n", tx_ix);
        status = rsi_bt_spp_transfer(remote_dev_addr, data, strlen(data));
#endif
      } break;

      case RSI_APP_EVENT_PASSKEY_DISPLAY: {
        //! clear the ssp receive event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_PASSKEY_DISPLAY);
      } break;

      case RSI_APP_EVENT_PASSKEY_REQUEST: {
        //! clear the ssp receive event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_PASSKEY_REQUEST);
      } break;

      case RSI_APP_EVENT_SSP_COMPLETE: {
        //! clear the ssp receive event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_SSP_COMPLETE);
        LOG_PRINT(" SSP conection completed\n");
      } break;

      case RSI_APP_EVENT_CONFIRM_REQUEST: {
        //! clear the ssp receive event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_CONFIRM_REQUEST);
        LOG_PRINT("Confirmation is requested\n");
        rsi_bt_accept_ssp_confirm((uint8_t *)remote_dev_addr);

      } break;

      case RSI_APP_EVENT_MODE_CHANGED: {
        //! clear the ssp receive event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_MODE_CHANGED);
        LOG_PRINT("MODE_CHANGE IS COMPLETED\n");
        //! initiating bt powersave mode
        if (sniff_mode == 2) {
#if (ENABLE_POWER_SAVE)
          if (!power_save_given) {
            //! enable wlan radio
            status = rsi_wlan_radio_init();
            if (status != RSI_SUCCESS) {
              LOG_PRINT("\n radio init failed \n");
            }
            //! initiating power save in BLE mode
            status = rsi_bt_power_save_profile(RSI_SLEEP_MODE_2, PSP_TYPE);
            if (status != RSI_SUCCESS) {
              LOG_PRINT("\r\n Failed in initiating power save \r\n");
            }
            //! initiating power save in wlan mode
            status = rsi_wlan_power_save_profile(RSI_SLEEP_MODE_2, PSP_TYPE);
            if (status != RSI_SUCCESS) {
              LOG_PRINT("\r\n Failed in initiating power save \r\n");
            }
            LOG_PRINT("In Power Save mode:\n");
            power_save_given = 1;
          }
#endif
        }
#if 0
					if(sniff_mode == 0)
					{
						/* here we call the sniff_mode command*/
						status = rsi_bt_sniff_mode(remote_dev_addr,SNIFF_MAX_INTERVAL,SNIFF_MIN_INTERVAL,SNIFF_ATTEMPT,SNIFF_TIME_OUT);  
						if(status != RSI_SUCCESS)
						{
							return status;
						}
					}
#endif
      } break;

      case RSI_APP_EVENT_SNIFF_SUBRATING: {
        //! clear the ssp receive event.
        rsi_bt_app_clear_event(RSI_APP_EVENT_SNIFF_SUBRATING);
        LOG_PRINT("SNIFF SUBRATING IS COMPLETED\n");
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
  rsi_task_handle_t bt_task_handle     = NULL;
  rsi_task_handle_t driver_task_handle = NULL;
#endif

#ifndef __linux__
  //! Board Initialization
  rsi_hal_board_init();
#endif

#ifndef RSI_WITH_OS
  //Start BT-BLE Stack
  intialize_bt_stack(STACK_BT_MODE);

  //! Call BLE Peripheral application
  status = rsi_bt_spp_slave();

  //! Application main loop
  main_loop();

  return status;
#endif

#ifdef RSI_WITH_OS
  //! Driver initialization
  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }
  //! Redpine module intialisation
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    return status;
  }

  //Start BT-BLE Stack
  intialize_bt_stack(STACK_BT_MODE);

  //! OS case
  //! Task created for BLE task
  rsi_task_create((rsi_task_function_t)rsi_bt_spp_slave,
                  (uint8_t *)"bt_task",
                  RSI_BT_TASK_STACK_SIZE,
                  NULL,
                  RSI_BT_TASK_PRIORITY,
                  &bt_task_handle);

  //! Task created for Driver task
  rsi_task_create((rsi_task_function_t)rsi_wireless_driver_task,
                  (uint8_t *)"driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);

  rsi_start_os_scheduler();
  return status;
#endif
}

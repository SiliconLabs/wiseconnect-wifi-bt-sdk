/*******************************************************************************
* @file  rsi_wlan_app.c
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

//! Driver Header file
#include "rsi_driver.h"

//! WLAN include file to refer wlan APIs
#include "rsi_wlan_apis.h"
#include "rsi_wlan_non_rom.h"

//! socket include file to refer socket APIs
#include "rsi_socket.h"
#include "rsi_utils.h"
//! include the certificate
#include "cacert.pem"

#define RSI_APP_BUF_SIZE 1600

//! SEND event number used in the application
#define RSI_SEND_EVENT BIT(0)

//! Access point SSID to connect
#define SSID "SILABS_AP"

//! Security type
#define SECURITY_TYPE 0

//! Password
#define PSK ""

//! DHCP mode 1- Enable 0- Disable
#define DHCP_MODE 1

//! If DHCP mode is disabled give IP statically
#if !(DHCP_MODE)

//! IP address of the module
//! E.g: 0x650AA8C0 == 192.168.10.101
#define DEVICE_IP 0x6500A8C0

//! IP address of Gateway
//! E.g: 0x010AA8C0 == 192.168.10.1
#define GATEWAY 0x010AA8C0

//! IP address of netmask
//! E.g: 0x00FFFFFF == 255.255.255.0
#define NETMASK 0x00FFFFFF

#endif

//! Device port number
#define DEVICE_PORT 5001

//! Server port number
#define SERVER_PORT 5001

//! Server IP address. Should be in reverse long format
//! E.g: 0x640AA8C0 == 192.168.10.100
#define SERVER_IP_ADDRESS 0x6400A8C0

//! Load certificate to device flash :
//! Certificate could be loaded once and need not be loaded for every boot up
#define LOAD_CERTIFICATE 1

//! Power Save Profile Mode
#define PSP_TYPE RSI_MAX_PSP

//! client socket id
int32_t client_socket;

//! server and client ip addresses
struct rsi_sockaddr_in server_addr, client_addr;

#if !(DHCP_MODE)

//! device static ip address
uint32_t ip_addr = DEVICE_IP;

//! Network mask
uint32_t network_mask = NETMASK;

//! Gateway
uint32_t gateway = GATEWAY;
#else
uint8_t dhcp_mode = (RSI_DHCP | RSI_DHCP_UNICAST_OFFER);

#endif

//! Enumeration for states in applcation
typedef enum rsi_wlan_app_state_e {
  RSI_WLAN_INITIAL_STATE          = 0,
  RSI_WLAN_UNCONNECTED_STATE      = 1,
  RSI_WLAN_CONNECTED_STATE        = 2,
  RSI_WLAN_IPCONFIG_DONE_STATE    = 3,
  RSI_WLAN_SOCKET_CONNECTED_STATE = 4
} rsi_wlan_app_state_t;

//! wlan application control block
typedef struct rsi_wlan_app_cb_s {
  //! wlan application state
  rsi_wlan_app_state_t state;

  //! length of buffer to copy
  uint32_t length;

  //! application buffer
  uint8_t buffer[RSI_APP_BUF_SIZE];

  //! to check application buffer availability
  uint8_t buf_in_use;

  //! application events bit map
  uint32_t event_map;

} rsi_wlan_app_cb_t;

//! application control block
rsi_wlan_app_cb_t rsi_wlan_app_cb;

//! Enumeration for commands used in application
typedef enum rsi_app_cmd_e { RSI_DATA = 0 } rsi_app_cmd_t;

//! Function prototypes
extern void rsi_wlan_app_task(void);
extern int32_t rsi_ble_app_send_to_wlan(uint8_t msg_type, uint8_t *buffer, uint32_t length);
extern void rsi_wlan_async_data_recv(uint32_t sock_no, uint8_t *buffer, uint32_t length);
extern void rsi_wlan_app_callbacks_init(void);
extern void rsi_join_fail_handler(uint16_t status, uint8_t *buffer, const uint32_t length);
extern void rsi_ip_renewal_fail_handler(uint16_t status, uint8_t *buffer, const uint32_t length);
extern void rsi_remote_socket_terminate_handler(uint16_t status, uint8_t *buffer, const uint32_t length);
extern void rsi_ip_change_notify_handler(uint16_t status, uint8_t *buffer, const uint32_t length);

extern void rsi_wlan_app_send_to_ble(uint16_t msg_type, uint8_t *data, uint16_t data_len);
void rsi_wlan_app_callbacks_init(void)
{
  //! Initialze join fail call back
  rsi_wlan_register_callbacks(RSI_JOIN_FAIL_CB, rsi_join_fail_handler);

  //! Initialze ip renewal fail call back
  rsi_wlan_register_callbacks(RSI_IP_FAIL_CB, rsi_ip_renewal_fail_handler);

  //! Initialze remote terminate call back
  rsi_wlan_register_callbacks(RSI_REMOTE_SOCKET_TERMINATE_CB, rsi_remote_socket_terminate_handler);

  //! Initialze ip change notify call back
  rsi_wlan_register_callbacks(RSI_IP_CHANGE_NOTIFY_CB, rsi_ip_change_notify_handler);
}

void rsi_wlan_app_task(void)
{
  int32_t status = RSI_SUCCESS;

  switch (rsi_wlan_app_cb.state) {
    case RSI_WLAN_INITIAL_STATE: {
      //! register call backs
      rsi_wlan_app_callbacks_init();

#if LOAD_CERTIFICATE
      //! Load certificates
      status = rsi_wlan_set_certificate(5, cacert, (sizeof(cacert) - 1));
      if (status != RSI_SUCCESS) {
        LOG_PRINT("\r\nLoad SSL CA Certificate Failed, Error Code : 0x%lX\r\n", status);
        break;
      } else
#endif
      {
        LOG_PRINT("\r\nLoad SSL CA Certificate Success\r\n");
        //! update wlan application state
        rsi_wlan_app_cb.state = RSI_WLAN_UNCONNECTED_STATE;
        break;
      }
    }
    case RSI_WLAN_UNCONNECTED_STATE: {
      //! Connect to an Access point
      status = rsi_wlan_connect((int8_t *)SSID, (rsi_security_mode_t)SECURITY_TYPE, PSK);
      if (status != RSI_SUCCESS) {
        LOG_PRINT("\r\nWLAN AP Connect Failed, Error Code : 0x%lX\r\n", status);
        break;
      } else {
        LOG_PRINT("\r\nWLAN AP Connect Success\r\n");
        //! update wlan application state
        rsi_wlan_app_cb.state = RSI_WLAN_CONNECTED_STATE;
        break;
      }
    }
    case RSI_WLAN_CONNECTED_STATE: {
      //! Configure IP
#if DHCP_MODE
      status = rsi_config_ipaddress(RSI_IP_VERSION_4, dhcp_mode, 0, 0, 0, NULL, 0, 0);
#else
      status = rsi_config_ipaddress(RSI_IP_VERSION_4,
                                    RSI_STATIC,
                                    (uint8_t *)&ip_addr,
                                    (uint8_t *)&network_mask,
                                    (uint8_t *)&gateway,
                                    NULL,
                                    0,
                                    0);
#endif
      if (status != RSI_SUCCESS) {
        LOG_PRINT("\r\nIP Config Failed, Error Code : 0x%lX\r\n", status);
        break;
      } else {
        LOG_PRINT("\r\nIP Config Success\r\n");
        //! update wlan application state
        rsi_wlan_app_cb.state = RSI_WLAN_IPCONFIG_DONE_STATE;
      }
#if (ENABLE_POWER_SAVE)

      //! initiating power save in WLAN mode
      status = rsi_wlan_power_save_profile(RSI_SLEEP_MODE_2, PSP_TYPE);
      if (status != RSI_SUCCESS) {
        break;
      }

      //! Enable Broadcast data filter
      status = rsi_wlan_filter_broadcast(5000, 1, 1);
      if (status != RSI_SUCCESS) {
        break;
      } else
#endif
        break;
    }
    case RSI_WLAN_IPCONFIG_DONE_STATE: {

      //! Create socket

      client_socket = rsi_socket_async(AF_INET, SOCK_STREAM, 1, rsi_wlan_async_data_recv);
      if (client_socket < 0) {
        status = rsi_wlan_get_status();
        LOG_PRINT("\r\nSocket Create Failed, Error Code : 0x%lX\r\n", status);
        break;
      }
      LOG_PRINT("\r\nSocket Create Success\r\n");

      //! Memset client structrue
      memset(&client_addr, 0, sizeof(client_addr));

      //! Set family type
      client_addr.sin_family = AF_INET;

      //! Set local port number
      client_addr.sin_port = htons(DEVICE_PORT);

      //! Bind socket
      status = rsi_bind(client_socket, (struct rsi_sockaddr *)&client_addr, sizeof(client_addr));
      if (status != RSI_SUCCESS) {
        status = rsi_shutdown(client_socket, 0);
        LOG_PRINT("\r\nBind Failed, Error code : 0x%lX\r\n", status);
        break;
      }
      LOG_PRINT("\r\nBind Success\r\n");
      //! Reset server structure
      memset(&server_addr, 0, sizeof(server_addr));

      //! Set server address family
      server_addr.sin_family = AF_INET;

      //! Set server port number, using htons function to use proper byte order
      server_addr.sin_port = htons(SERVER_PORT);

      //! Set IP address to localhost
      server_addr.sin_addr.s_addr = SERVER_IP_ADDRESS;

      //! Connect to server socket
      status = rsi_connect(client_socket, (struct rsi_sockaddr *)&server_addr, sizeof(server_addr));
      if (status != RSI_SUCCESS) {
        status = rsi_shutdown(client_socket, 0);
        LOG_PRINT("\r\nFailed to Connect to SSL Server, Error code : 0x%lX\r\n", status);
        break;
      } else {
        LOG_PRINT("\r\nConnect to SSL Server Success\r\n");
        //! update wlan application state
        rsi_wlan_app_cb.state = RSI_WLAN_SOCKET_CONNECTED_STATE;
        break;
      }
    }
    case RSI_WLAN_SOCKET_CONNECTED_STATE: {
      if (rsi_wlan_app_cb.event_map & RSI_SEND_EVENT) {
        //! send data on socket
        status = rsi_send(client_socket, (const int8_t *)rsi_wlan_app_cb.buffer, rsi_wlan_app_cb.length, 0);
        if (status < 0) {
          status = rsi_wlan_get_status();
          LOG_PRINT("\r\nFailed to Send data to SSL Server, Error Code : 0x%lX\r\n", status);
          break;
        } else {
          LOG_PRINT("\r\nSSL data sent \r\n");
          //! make buffer in use
          rsi_wlan_app_cb.buf_in_use = 0;

          //! clear send event after data send is successful
          rsi_wlan_app_cb.event_map &= ~(RSI_SEND_EVENT);
        }
      }
    }
    default:
      break;
  }
}

int32_t rsi_ble_app_send_to_wlan(uint8_t msg_type, uint8_t *buffer, uint32_t length)
{
  switch (msg_type) {
    case RSI_DATA: {
      //! buffer is in use or not
      if (!rsi_wlan_app_cb.buf_in_use) {
        //! if not in use

        //! copy the buffer to wlan app cb tx buffer
        memcpy(rsi_wlan_app_cb.buffer, buffer, length);

        //! hold length information
        rsi_wlan_app_cb.length = length;

        //! make buffer in use
        rsi_wlan_app_cb.buf_in_use = 1;

        //! raise event to wlan app task
        rsi_wlan_app_cb.event_map |= RSI_SEND_EVENT;

      } else
      //!if buffer is in use
      {
        return -1;
        //! return error
      }
    }
    default:
      break;
  }
  return 0;
}

void rsi_wlan_async_data_recv(uint32_t sock_no, uint8_t *buffer, const uint32_t length)
{
  UNUSED_PARAMETER(sock_no); //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(buffer);  //This statement is added only to resolve compilation warning, value is unchanged

  //! send packet to ble
  rsi_wlan_app_send_to_ble(RSI_DATA, buffer, length);
}

//! callback functions

//! rejoin failure call back handler in station mode
void rsi_join_fail_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  UNUSED_PARAMETER(status);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(buffer);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_CONST_PARAMETER(length); //This statement is added only to resolve compilation warning, value is unchanged

  //! update wlan application state
  rsi_wlan_app_cb.state = RSI_WLAN_UNCONNECTED_STATE;
}

//! ip renewal failure call back handler in station mode
void rsi_ip_renewal_fail_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  UNUSED_PARAMETER(status);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(buffer);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_CONST_PARAMETER(length); //This statement is added only to resolve compilation warning, value is unchanged

  //! update wlan appication state
  rsi_wlan_app_cb.state = RSI_WLAN_CONNECTED_STATE;
}

//! stations remote socket terminate call back handler
void rsi_remote_socket_terminate_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  UNUSED_PARAMETER(status);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(buffer);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_CONST_PARAMETER(length); //This statement is added only to resolve compilation warning, value is unchanged

  //! update wlan appication state
  rsi_wlan_app_cb.state = RSI_WLAN_IPCONFIG_DONE_STATE;
}

//! stations ip change notify call back handler in station mode
void rsi_ip_change_notify_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  UNUSED_PARAMETER(status);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(buffer);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_CONST_PARAMETER(length); //This statement is added only to resolve compilation warning, value is unchanged

  //! update wlan appication state
  rsi_wlan_app_cb.state = RSI_WLAN_IPCONFIG_DONE_STATE;
}

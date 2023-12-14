/*******************************************************************************
* @file  rsi_concurrent_mode.c
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
 * Include files
 * */

//! include file to refer data types
#include "rsi_data_types.h"

//! COMMON include file to refer wlan APIs
#include "rsi_common_apis.h"

//! WLAN include file to refer wlan APIs
#include "rsi_wlan_apis.h"
#include "rsi_wlan_non_rom.h"

//! socket include file to refer socket APIs
#include "rsi_socket.h"

#include "rsi_bootup_config.h"
//! Error include files
#include "rsi_error.h"
#include "rsi_utils.h"
//! OS include file to refer OS specific functionality
#include "rsi_os.h"

#include "rsi_driver.h"

#ifdef RSI_M4_INTERFACE
#include "rsi_board.h"
#endif

#define SSL 1

#if SSL
// Include SSL CA certificate
#include <cacert.pem>
#endif

//! STATIC_IP_FOR_AP 1- Enable 0- Disable for APUT
#define STATIC_IP_FOR_AP 1

//! Access point SSID to connect
#define SSID "SILABS_AP"

//! Security type Access Point to connect
#define STA_SECURITY_TYPE RSI_OPEN

//! Password of AP to connect
#define STA_PSK NULL

//! SSID of Access point to be created
#define AP_SSID "SILABS_AP"

//! Security type of created Access point
#define AP_SECURITY_TYPE RSI_WPA2

//! Encryption type of created Access point
#define AP_ENCRYPTION_TYPE RSI_CCMP

//! Password of created Access point
#define AP_PSK "1234567890"

//! Beacon Interval of created Access point
#define BEACON_INTERVAL 100

//! DTIM Count of created Access Point
#define DTIM_COUNT 4

//! DHCP mode 1- Enable 0- Disable for STAUT
#define DHCP_MODE 1

//! If DHCP mode is disabled given IP statically
#if !(DHCP_MODE)

// IP address of the module
#define STA_DEVICE_IP6 "2001:db8:0:1::121"
// IP address of Gateway
#define STA_GATEWAY6 "2001:db8:0:1::121"

//! IP address of the module
//! E.g: 0x650AA8C0 == 192.168.10.101
#define STA_DEVICE_IP "192.168.10.101" //0x650AA8C0
//! IP address of Gateway
//! E.g: 0x010AA8C0 == 192.168.10.1
#define STA_GATEWAY "192.168.10.1" //0x010AA8C0//! IP address of netmask
//! E.g: 0x00FFFFFF == 255.255.255.0
#define STA_NETMASK "255.255.255.0" //0x00FFFFFF

#endif

#if STATIC_IP_FOR_AP
//! IP address of the module
//! E.g: 0x650AA8C0 == 192.168.10.200
#define AP_DEVICE_IP "192.168.10.200" //0xC80AA8C0
//! IP address of Gateway
//! E.g: 0x010AA8C0 == 192.168.10.1
#define AP_GATEWAY "192.168.10.1" //0x010AA8C0
//! IP address of netmask
//! E.g: 0x00FFFFFF == 255.255.255.0
#define AP_NETMASK "255.255.255.0" //0x00FFFFFF

// IP address of the module
#define AP_DEVICE_IP6 "2001:db8:0:1::200"
// IP address of Gateway
#define AP_GATEWAY6 "2001:db8:0:1::200"
#endif

#define RSI_APP_BUF_SIZE 1600

//! Device port number
#define DEVICE_PORT_1 5001
#define DEVICE_PORT_2 5002

//! Server port number
#define SERVER_PORT 5001

//! Server IP address.
#define SERVER_IP_ADDRESS "192.168.0.199"

//! Server IP address.
#define SERVER_IP6_ADDRESS "2409:408c:2dc1:84ce:984a:75ae:99ec:73e6"

//! Number of packet to send or receive
#define NUMBER_OF_PACKETS 1000

//! Memory length for driver
#define GLOBAL_BUFF_LEN 15000

//! Wlan task priority
#define RSI_WLAN_TASK_PRIORITY 1

//! Wireless driver task priority
#define RSI_DRIVER_TASK_PRIORITY 2

//! Wlan task stack size
#define RSI_WLAN_TASK_STACK_SIZE 1000

//! Wireless driver task stack size
#define RSI_DRIVER_TASK_STACK_SIZE 500
//! vap id to create socket

#define RSI_VAP_ID    0
#define RSI_AP_VAP_ID 1

#define MAX_CNT              1000
#define SOCKET_ASYNC_FEATURE 1

volatile uint8_t data_recvd = 0;
volatile uint64_t num_bytes = 0;
uint8_t station_mac[6];

#if ENABLE_POWER_SAVE
//! Power Save Profile mode
#define PSP_MODE RSI_SLEEP_MODE_2

//! Power Save Profile type
#define PSP_TYPE RSI_MAX_PSP

int32_t rsi_wlan_power_save_profile(uint8_t psp_mode, uint8_t psp_type);
#endif

extern int rsi_inet_pton6(const char *src, const char *src_endp, unsigned char *dst, unsigned int *ptr_result);
#include <string.h>

//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN];
uint64_t ip_to_reverse_hex(char *ip);

//! Enumeration for states in applcation
typedef enum rsi_wlan_app_state_e {
  RSI_WLAN_IDEAL_STATE         = 0,
  RSI_WLAN_AP_BRINGUP_STATE    = 1,
  RSI_WLAN_DATA_TRANSFER_STATE = 2,
  RSI_WLAN_WAIT_STATE          = 3,
  RSI_WLAN_AP_DISCONNECT_STATE = 4
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

//! rejoin failure call back handler in station mode
void rsi_join_fail_handler(uint32_t status, const uint8_t *buffer, const uint32_t length)
{
  //! update wlan application state
  LOG_PRINT("\nSwitching to RSI_WLAN_AP_DISCONNECT_STATE");
  rsi_wlan_app_cb.state = RSI_WLAN_AP_DISCONNECT_STATE;
}

//! This callback function is called when Station is connected.
//! Buffer has the MAC address of the station connected
void stations_connect_notify_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  LOG_PRINT("\r\nStation Connected\r\n");
  UNUSED_PARAMETER(status);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_CONST_PARAMETER(length); //This statement is added only to resolve compilation warning, value is unchanged
  memcpy(station_mac, buffer, 6);
}

//! This callback function is called when Station is disconnected.
//! Buffer has the MAC address of the station disconnected
void stations_disconnect_notify_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  LOG_PRINT("\r\nStation Disconnected\r\n");
  UNUSED_PARAMETER(status);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(buffer);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_CONST_PARAMETER(length); //This statement is added only to resolve compilation warning, value is unchanged
}

uint32_t pkt_cnt = 0;
// Memory length for send buffer
#define SSL_BUFF_SIZE     1300
#define DEFAULT_BUFF_SIZE 1460
#define BUF_SIZE          SSL_BUFF_SIZE
#define MAX_TX_PKTS       100

// Load certificate to device flash :
// Certificate could be loaded once and need not be loaded for every boot up
#define LOAD_CERTIFICATE   1
#define RSI_SSL_BIT_ENABLE 0

// Memory to initialize driv
uint8_t global_buf[GLOBAL_BUFF_LEN];
uint8_t ip_buff[20];
uint32_t pkt_rx_cnt = 0, tick_count_s = 1;
#ifdef LINUX_PLATFORM
static uint32_t last_tx_print_time = 0;
static uint32_t last_rx_print_time;
#endif

void compute_throughput(void);
extern int32_t rsi_wlan_buffer_config(void);
extern void rsi_remote_socket_terminate_handler(uint16_t status, uint8_t *buffer, const uint32_t length);
extern uint64_t ip_to_reverse_hex(char *ip);

// Call back for Socket Async
void socket_async_recive(uint32_t sock_no, uint8_t *buffer, uint32_t length)
{
  UNUSED_PARAMETER(sock_no);
  UNUSED_PARAMETER(buffer);

  num_bytes += length;
}

void rsi_wlan_app_call_backs_init(void)
{
  //! Initialze join fail call back
  rsi_wlan_register_callbacks(RSI_JOIN_FAIL_CB, (void *)rsi_join_fail_handler);
  rsi_wlan_register_callbacks(RSI_STATIONS_CONNECT_NOTIFY_CB, stations_connect_notify_handler);
  rsi_wlan_register_callbacks(RSI_STATIONS_DISCONNECT_NOTIFY_CB, stations_disconnect_notify_handler);
}

int32_t rsi_concurrent_mode_dual_ip()
{
  int8_t vap_id    = RSI_VAP_ID;
  int8_t ap_vap_id = RSI_AP_VAP_ID;
  int32_t status   = RSI_SUCCESS;
  int32_t addr_size;
  uint8_t ssl_bit_map = RSI_SSL_BIT_ENABLE;

  int32_t server_socket, new_socket, client_socket;
  int8_t send_buf[BUF_SIZE];
  uint32_t pkt_cnt = 0, i;

#ifdef RSI_WITH_OS
  rsi_task_handle_t driver_task_handle = NULL;
#endif

#if DHCP_MODE
  uint8_t dhcp_mode = (RSI_DHCP | RSI_DHCP_UNICAST_OFFER);
#endif

#if DHCP_MODE
  uint8_t ip_buff_v4[20];
#else
  uint32_t ip_addr      = ip_to_reverse_hex(STA_DEVICE_IP);
  uint32_t network_mask = ip_to_reverse_hex(STA_NETMASK);
  uint32_t gateway      = ip_to_reverse_hex(STA_GATEWAY);
#endif

#if DHCP_MODE
  uint32_t ip_buff_v6[13];
#else
  uint32_t ip_addr6[4];
  uint32_t gateway6[4];
  uint32_t *network_mask6 = NULL;
#endif

#if STATIC_IP_FOR_AP

  uint8_t ap_ip_buff_v4[20];

  uint32_t ap_ip_addr      = ip_to_reverse_hex(AP_DEVICE_IP);
  uint32_t ap_network_mask = ip_to_reverse_hex(AP_NETMASK);
  uint32_t ap_gateway      = ip_to_reverse_hex(AP_GATEWAY);

  uint32_t ap_ip_buff_v6[13];

  uint32_t ap_ip_addr_v6[4];
  uint32_t *ap_network_mask_v6 = NULL;
  uint32_t ap_gateway_v6[4];

  unsigned char ap_link_local_addr[46] = { 0 };
  unsigned char ap_global_addr[46]     = { 0 };
  unsigned char ap_gateway6[46]        = { 0 };

#endif

  struct rsi_sockaddr_in server_addr, client_addr;

  uint32_t serv_addr_v6[4];
  unsigned char hex_addr[RSI_IPV6_ADDRESS_LENGTH] = { 0 };
  unsigned char link_local_addr[46]               = { 0 };
  unsigned char global_addr[46]                   = { 0 };
  unsigned char gateway_6[46]                     = { 0 };

  struct rsi_sockaddr_in6 server_addr6, client_addr6;

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

  //! Register callbacks for Station conencted and disconnected events
  rsi_wlan_app_call_backs_init();

#ifdef RSI_WITH_OS
  //! Task created for Driver task
  rsi_task_create((rsi_task_function_t)rsi_wireless_driver_task,
                  (uint8_t *)"driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);
#endif
  //! WC initialization
  status = rsi_wireless_init(9, 0);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWireless Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nWireless Initialization Success\r\n");
  }

  status = rsi_send_feature_frame();
  if (status != RSI_SUCCESS) {
    return status;
  }

  // Scanning for channel no of Access point so as to start Module AP in same channel
  rsi_rsp_scan_t result;
  int32_t length = sizeof(result);

  rsi_wlan_app_cb.state = RSI_WLAN_IDEAL_STATE;
  while (1) {
    switch (rsi_wlan_app_cb.state) {
      case RSI_WLAN_IDEAL_STATE:

        memset(&result, 0, length);
        status = rsi_wlan_scan((int8_t *)SSID, 0, &result, length);

        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nWLAN Scan Failed, Error Code : 0x%lX\r\n", status);
          return status;
        }
        uint8_t channel_no = result.scan_info->rf_channel;
        LOG_PRINT("\r\nScan successful channel: 0x%lX\r\n", channel_no);

        //! Connect to an Access point
        status = rsi_wlan_connect((int8_t *)SSID, STA_SECURITY_TYPE, STA_PSK);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nConnect to Access point Failed, Error Code : 0x%lX\r\n", status);
          return status;
        } else {
          LOG_PRINT("\r\nConnect to Access point Success\r\n");
        }

#if LOAD_CERTIFICATE
#if SSL
        // Load SSL CA certificate
        status = rsi_wlan_set_certificate(RSI_SSL_CA_CERTIFICATE, cacert, (sizeof(cacert) - 1));
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nLoad SSL CA Certificate Failed, Error Code : 0x%lX\r\n", status);
          return status;
        }
        LOG_PRINT("\r\nLoad SSL CA Certificate Success\r\n");
#endif
#endif

        //! Configure IPv4 for station mode

#if DHCP_MODE
        status = rsi_config_ipaddress(RSI_IP_VERSION_4, dhcp_mode, 0, 0, 0, ip_buff_v4, sizeof(ip_buff_v4), 0);
#else
        status        = rsi_config_ipaddress(RSI_IP_VERSION_4,
                                      RSI_STATIC,
                                      (uint8_t *)&ip_addr,
                                      (uint8_t *)&network_mask,
                                      (uint8_t *)&gateway,
                                      NULL,
                                      0,
                                      0);
#endif

        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nIP CONFIG FOR IPV4 STA, Failed, Error Code : 0x%lX\r\n", status);
          return status;
        }
        LOG_PRINT("\r\nIP CONFIG FOR IPV4 STA Success\r\n");
#if DHCP_MODE
        LOG_PRINT("RSI_STA IP ADDR: %d.%d.%d.%d \r\n", ip_buff_v4[6], ip_buff_v4[7], ip_buff_v4[8], ip_buff_v4[9]);
#endif

//! Configure IPv6 for station mode
#if DHCP_MODE
        status =
          rsi_config_ipaddress(RSI_IP_VERSION_6, dhcp_mode, 0, 0, 0, (uint8_t *)ip_buff_v6, sizeof(ip_buff_v6), 0);
        rsi_inet_ntop6((const unsigned char *)&ip_buff_v6[1], (char *)link_local_addr, sizeof(link_local_addr));
        rsi_inet_ntop6((const unsigned char *)&ip_buff_v6[5], (char *)global_addr, sizeof(global_addr));
        rsi_inet_ntop6((const unsigned char *)&ip_buff_v6[9], (char *)gateway_6, sizeof(gateway_6));
#else
        status        = rsi_inet_pton6(STA_DEVICE_IP6, STA_DEVICE_IP6 + strlen(STA_DEVICE_IP6), hex_addr, ip_addr6);
        status        = rsi_inet_pton6(STA_GATEWAY6, STA_GATEWAY6 + strlen(STA_GATEWAY6), hex_addr, gateway6);
        status        = rsi_config_ipaddress(RSI_IP_VERSION_6,
                                      RSI_STATIC,
                                      (uint8_t *)&ip_addr6,
                                      (uint8_t *)&network_mask6,
                                      (uint8_t *)&gateway6,
                                      NULL,
                                      0,
                                      0);
#endif

        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nIP CONFIG FOR IPV6 STA Failed, Error Code : 0x%lX\r\n", status);
          return status;
        }
        LOG_PRINT("\r\nIP CONFIG FOR IPV6 STA Success\r\n");

#if DHCP_MODE
        LOG_PRINT("linklocaladdr: %s\r\n", link_local_addr);
        LOG_PRINT("global addr:   %s\r\n", global_addr);
        LOG_PRINT("gateway addr:  %s\r\n", gateway_6);
#endif

        status =
          rsi_inet_pton6(SERVER_IP6_ADDRESS, SERVER_IP6_ADDRESS + strlen(SERVER_IP6_ADDRESS), hex_addr, serv_addr_v6);

        rsi_wlan_app_cb.state = RSI_WLAN_AP_BRINGUP_STATE;
        break;
      case RSI_WLAN_AP_BRINGUP_STATE:

#if STATIC_IP_FOR_AP

        //! Configure IPV4 for AP
        status = rsi_config_ipaddress(RSI_IP_VERSION_4,
                                      RSI_STATIC,
                                      (uint8_t *)&ap_ip_addr,
                                      (uint8_t *)&ap_network_mask,
                                      (uint8_t *)&ap_gateway,
                                      ap_ip_buff_v4,
                                      sizeof(ap_ip_buff_v4),
                                      1);

        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nIP CONFIG FOR IPV4 AP Failed, Error Code : 0x%lX\r\n", status);
          return status;
        }
        LOG_PRINT("\r\nIP CONFIG FOR IPV4 AP Success\r\n");
        LOG_PRINT("******RSI_AP IP ADDR: %d.%d.%d.%d **********\r\n",
                  ap_ip_buff_v4[6],
                  ap_ip_buff_v4[7],
                  ap_ip_buff_v4[8],
                  ap_ip_buff_v4[9]);

        //! Configure IPV6 for AP

        status = rsi_inet_pton6(AP_DEVICE_IP6, AP_DEVICE_IP6 + strlen(AP_DEVICE_IP6), hex_addr, ap_ip_addr_v6);
        status = rsi_inet_pton6(AP_GATEWAY6, AP_GATEWAY6 + strlen(AP_GATEWAY6), hex_addr, ap_gateway_v6);

        status = rsi_config_ipaddress(RSI_IP_VERSION_6,
                                      RSI_STATIC,
                                      (uint8_t *)&ap_ip_addr_v6,
                                      (uint8_t *)&ap_network_mask_v6,
                                      (uint8_t *)&ap_gateway_v6,
                                      ap_ip_buff_v6,
                                      sizeof(ap_ip_buff_v6),
                                      1);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nIP CONFIG FOR IPV6 AP Failed, Error Code : 0x%lX\r\n", status);
          return status;
        } else {
          LOG_PRINT("\r\nIP CONFIG FOR IPV6 AP Success\r\n");

          rsi_inet_ntop6((const unsigned char *)&ap_ip_buff_v6[1],
                         (char *)ap_link_local_addr,
                         sizeof(ap_link_local_addr));
          rsi_inet_ntop6((const unsigned char *)&ap_ip_buff_v6[5], (char *)ap_global_addr, sizeof(ap_global_addr));
          rsi_inet_ntop6((const unsigned char *)&ap_ip_buff_v6[9], (char *)ap_gateway6, sizeof(ap_gateway6));

          LOG_PRINT("linklocaladdr: %s\r\n", ap_link_local_addr);
          LOG_PRINT("global addr:   %s\r\n", ap_global_addr);
          LOG_PRINT("gateway addr:  %s\r\n", ap_gateway6);
        }

#endif
        //! Start Access point
        status = rsi_wlan_ap_start((int8_t *)AP_SSID,
                                   channel_no,
                                   AP_SECURITY_TYPE,
                                   AP_ENCRYPTION_TYPE,
                                   (uint8_t *)AP_PSK,
                                   BEACON_INTERVAL,
                                   DTIM_COUNT);

        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nAP Start Failed, Error Code : 0x%lX\r\n", status);
          return status;
        } else {
          LOG_PRINT("\r\nAP Start Success\r\n");
        }
#if ENABLE_POWER_SAVE
        //! Apply power save profile
        status = rsi_wlan_power_save_profile(PSP_MODE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          return status;
        }
#endif

        rsi_wlan_app_cb.state = RSI_WLAN_DATA_TRANSFER_STATE;
        break;

      case RSI_WLAN_DATA_TRANSFER_STATE:

        //! AP DATA TRANSFER
        LOG_PRINT("\r\nTCP server socket on AP VAP on IPv4. TCP_RX\r\n");

        //! Create socket
#if SOCKET_ASYNC_FEATURE
        server_socket = rsi_socket_async(AF_INET, SOCK_STREAM, 0, socket_async_recive);
#else
        server_socket = rsi_socket(AF_INET, SOCK_STREAM, 0);
#endif

        if (server_socket < 0) {
          status = rsi_wlan_get_status();
          LOG_PRINT("\r\nSocket Create Failed, Error Code : 0x%lX\r\n", status);
          return status;
        } else {
          LOG_PRINT("\r\nSocket Create Success\r\n");
        }
        status = rsi_setsockopt(client_socket, SOL_SOCKET, SO_SOCK_VAP_ID, &ap_vap_id, sizeof(ap_vap_id));
        if (status != RSI_SUCCESS) {
          return status;
        }

        //! Set server structure
        memset(&server_addr, 0, sizeof(server_addr));

        //! Set family type
        server_addr.sin_family = AF_INET;

        //! Set local port number
        server_addr.sin_port = htons(DEVICE_PORT_2);

        //! Bind socket
        status = rsi_bind(server_socket, (struct rsi_sockaddr *)&server_addr, sizeof(server_addr));
        if (status != RSI_SUCCESS) {
          status = rsi_wlan_get_status();
          rsi_shutdown(server_socket, 0);
          LOG_PRINT("\r\nBind Failed, Error code : 0x%lX\r\n", status);
          return status;
        } else {
          LOG_PRINT("\r\nBind Success\r\n");
        }

        //! Socket listen
        status = rsi_listen(server_socket, 1);
        if (status != RSI_SUCCESS) {
          status = rsi_wlan_get_status();
          rsi_shutdown(server_socket, 0);
          LOG_PRINT("\r\nListen Failed, Error code : 0x%lX\r\n", status);
          return status;
        } else {
          LOG_PRINT("\r\nListen Success\r\n");
        }

        addr_size = sizeof(server_socket);

        //! Socket accept
        new_socket = rsi_accept(server_socket, (struct rsi_sockaddr *)&client_addr, &addr_size);
        if (new_socket < 0) {
          status = rsi_wlan_get_status();
          rsi_shutdown(server_socket, 0);
          LOG_PRINT("\r\nSocket Accept Failed, Error code : 0x%lX\r\n", status);
          return status;
        } else {
          LOG_PRINT("\r\nSocket Accept Success\r\n");
        }

        //! STA DATA TRANSFER
        LOG_PRINT("\r\nSSL client socket on STA VAP on IPv6. SSL_TX\r\n");
        // Create socket on station Vap on ipv6
        client_socket = rsi_socket(AF_INET6, SOCK_STREAM, 1);
        if (client_socket < 0) {
          status = rsi_wlan_get_status();
          LOG_PRINT("\r\nSocket Create Failed, Error Code : 0x%lX\r\n", status);
          return status;
        }
        LOG_PRINT("\r\nSocket Create Success\r\n");

        status = rsi_setsockopt(client_socket, SOL_SOCKET, SO_SOCK_VAP_ID, &vap_id, sizeof(vap_id));
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nSet Socket Option Failed, Error Code : 0x%lX\r\n", status);
          return status;
        }
        LOG_PRINT("\r\nSet Socket Option Success\r\n");

        status = rsi_setsockopt(client_socket, SOL_SOCKET, SO_SSL_ENABLE, &ssl_bit_map, sizeof(ssl_bit_map));
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nSet Socket Option Failed, Error Code : 0x%lX\r\n", status);
          return status;
        }
        LOG_PRINT("\r\nSet Socket Option Success\r\n");
        // Memset client structrue
        memset(&client_addr6, 0, sizeof(client_addr6));
        // Set family type
        client_addr6.sin6_family = AF_INET6;
        // Set local port number
        client_addr6.sin6_port = htons(DEVICE_PORT_1);
        // Bind socket
        status = rsi_bind(client_socket, (struct rsi_sockaddr *)&client_addr6, sizeof(client_addr6));

        if (status != RSI_SUCCESS) {
          status = rsi_wlan_get_status();
          rsi_shutdown(client_socket, 0);
          LOG_PRINT("\r\nBind Failed, Error code : 0x%lX\r\n", status);
          return status;
        }
        LOG_PRINT("\r\nBind Success\r\n");

        // Set server structure
        memset(&server_addr6, 0, sizeof(server_addr6));

        // Set server address family
        server_addr6.sin6_family = AF_INET6;
        // Set server port number, using htons function to use proper byte order
        server_addr6.sin6_port = htons(SERVER_PORT);
        // Set IP address to localhost
        memcpy(server_addr6.sin6_addr._S6_un._S6_u32, serv_addr_v6, RSI_IPV6_ADDRESS_LENGTH);

        // Connect to server socket
        status = rsi_connect(client_socket, (struct rsi_sockaddr *)&server_addr6, sizeof(server_addr6));

        if (status != RSI_SUCCESS) {
          status = rsi_wlan_get_status();
          rsi_shutdown(client_socket, 0);
          LOG_PRINT("\r\nFailed to Connect to SSL Server, Error code : 0x%lX\r\n", status);
          return status;
        }
        LOG_PRINT("\r\nConnect to SSL Server Success\r\n");

        for (i = 0; i < BUF_SIZE; i++) {
          send_buf[i] = i;
        }

        LOG_PRINT("\r\nSSL TX Start \r\n");
        ;
        while (1) {
          // Send data on socket
          status = rsi_send(client_socket, send_buf, BUF_SIZE, 0);
          if (status < 0) {
            status = rsi_wlan_get_status();
            rsi_shutdown(client_socket, 0);
            LOG_PRINT("\r\nFailed to Send to SSL Server, Error Code : 0x%lX\r\n", status);
            break;
          } else {
            pkt_cnt++;

            if (pkt_cnt == MAX_TX_PKTS) {
              rsi_shutdown(client_socket, 0);

              LOG_PRINT("\r\nSSL TX completed \r\n");
              ;
              break;
            }
          }
        }
        while (1) {
          rsi_wireless_driver_task();
        }

        rsi_wlan_app_cb.state = RSI_WLAN_WAIT_STATE;
        break;
      case RSI_WLAN_WAIT_STATE:
        break;

      case RSI_WLAN_AP_DISCONNECT_STATE:

        //! Stop Access point
        status = rsi_wlan_ap_stop();
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nAP Stop Failed, Error Code : 0x%lX\r\n", status);
          return status;
        } else {
          rsi_shutdown(server_socket, 0);
          LOG_PRINT("\r\nAP Stop Success\r\n");
        }
        rsi_wlan_app_cb.state = RSI_WLAN_IDEAL_STATE;
        break;

      default:
        break;
    }
  }
}

void main_loop(void)
{
  while (1) {
    ////////////////////////
    //! Application code ///
    ////////////////////////

    //! event loop
    rsi_wireless_driver_task();
  }
}

int main()
{
  int32_t status = RSI_SUCCESS;

#ifdef RSI_WITH_OS

  rsi_task_handle_t wlan_task_handle = NULL;
#endif

#ifdef RSI_WITH_OS
  //! OS case
  //! Task created for WLAN task
  rsi_task_create((rsi_task_function_t)(int32_t)rsi_concurrent_mode_dual_ip,
                  (uint8_t *)"wlan_task",
                  RSI_WLAN_TASK_STACK_SIZE,
                  NULL,
                  RSI_WLAN_TASK_PRIORITY,
                  &wlan_task_handle);

  //! OS TAsk Start the scheduler
  rsi_start_os_scheduler();

#else
  //! NON - OS case
  //! Call concurrent mode  application
  status = rsi_concurrent_mode_dual_ip();

  //! Application main loop
  main_loop();
#endif
  return status;
}

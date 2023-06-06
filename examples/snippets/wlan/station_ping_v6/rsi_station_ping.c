/*******************************************************************************
* @file  rsi_station_ping.c
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
/*************************************************************************
 *
 */

/*================================================================================
 * @brief : This file contains example application for Station Ping
 * @section Description :
 * The application demonstrates how to configure SiLabs device in client mode to
 * send ping request to target IP address.
 =================================================================================*/

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

#ifdef RSI_M4_INTERFACE
#include "rsi_board.h"
#endif

//! OS include file to refer OS specific functionality
#include "rsi_os.h"
#include "rsi_utils.h"
#include "rsi_driver.h"
//! configuration Parameters

//! Access point SSID to connect
#define SSID "SILABS_AP"

//!Scan Channel number , 0 - to scan all channels
#define CHANNEL_NO 0

//! Security type
#define SECURITY_TYPE RSI_WPA2

//! Password
#define PSK "12345678"

//! To provide the cmd type for wlan set
#define CMD_TYPE RSI_JOIN_BSSID

//! To configure the AP mac
#define AP_BSSID                       \
  {                                    \
    0x64, 0x0f, 0xf5, 0x3f, 0xbb, 0xf0 \
  }

#ifndef RSI_CONFIGURE_IPV6
//! IP address of the remote device
#define REMOTE_IP "192.168.1.1"
#else
#define REMOTE_IP "2401:4290:1289:10ed::106"
#endif

//! ping size
#define PING_SIZE 100

#ifndef RSI_CONFIGURE_IPV6
#define RSI_IP_VERSION RSI_IP_VERSION_4
#else
#define RSI_IP_VERSION RSI_IP_VERSION_6
#endif

//! DHCP mode 1- Enable 0- Disable
#define DHCP_MODE 1

//! If DHCP mode is disabled given IP statically
#if !(DHCP_MODE)

#ifndef RSI_CONFIGURE_IPV6

//! IP address of the module
//! E.g: 0x650AA8C0 == 192.168.10.101
#define DEVICE_IP 0x0800A8C0

//! IP address of Gateway
//! E.g: 0x010AA8C0 == 192.168.10.1
#define GATEWAY 0x0100A8C0

//! IP address of netmask
//! E.g: 0x00FFFFFF == 255.255.255.0
#define NETMASK 0x00FFFFFF

#else
//! IPv6 address of the module
#define DEVICE_IP6 "2401:4290:1289:10ed::106"

//! IPv6 address of Gateway
#define GATEWAY6   "2401:4290:1289:10ed::106"

#endif
#endif

//! Number of packet to send
#define NUMBER_OF_PACKETS 20

//! Memory length for driver
#define GLOBAL_BUFF_LEN 15000

//! Wlan task priority
#define RSI_WLAN_TASK_PRIORITY 1

//! Wireless driver task priority
#define RSI_DRIVER_TASK_PRIORITY 2

//! Wlan task stack size
#define RSI_WLAN_TASK_STACK_SIZE 500

//! Wireless driver task stack size
#define RSI_DRIVER_TASK_STACK_SIZE 500

//! connect through PMK
//! To Enable keep 1 else 0
#define CONNECT_WITH_PMK 0

//! Type
#define PMK_TYPE 3

//!size of pmk
#define PMK_SIZE 32

//! standard defines

//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN];
volatile uint8_t ping_rsp_received;
uint64_t ip_to_reverse_hex(char *ip);
void rsi_ping_response_handler(uint16_t status, const uint8_t *buffer, const uint16_t length);
#ifndef RSI_WITH_OS
static void main_loop(void);
#endif

extern const char *rsi_inet_ntop6(const unsigned char *input, char *dst, rsi_socklen_t size);
extern int rsi_inet_pton6(const char *src, const char *src_endp, unsigned char *dst, unsigned int *ptr_result);

//! Ping Application in station mode
int32_t rsi_station_ping_app()
{
#ifndef RSI_CONFIGURE_IPV6
  uint8_t ip_buff[20];
#else
  uint32_t ip_buff[13];
  unsigned char hex_addr[RSI_IPV6_ADDRESS_LENGTH] = { 0 };
  unsigned char link_local_addr[46]               = { 0 };
  unsigned char global_addr[46]                   = { 0 };
  unsigned char gateway6[46]                      = { 0 };
#endif
  int32_t status        = RSI_SUCCESS;
  uint8_t join_bssid[6] = AP_BSSID;
  uint16_t size         = PING_SIZE;
  int32_t packet_count  = 0;
#if !(DHCP_MODE)
#ifndef RSI_CONFIGURE_IPV6
  uint32_t ip_addr      = DEVICE_IP;
  uint32_t network_mask = NETMASK;
  uint32_t gateway      = GATEWAY;
#else
  uint32_t ip_addr[4];
  uint32_t gateway[4];
  uint32_t *network_mask = NULL;
#endif
#else
  uint8_t dhcp_mode                               = (RSI_DHCP | RSI_DHCP_UNICAST_OFFER);
#endif
#ifndef RSI_CONFIGURE_IPV6
  uint32_t remote_address = ip_to_reverse_hex(REMOTE_IP);
#else
  uint32_t remote_address[4];
#endif

#if CONNECT_WITH_PMK
  uint8_t pmk[PMK_SIZE];
#endif
#ifdef RSI_WITH_OS
  rsi_task_handle_t driver_task_handle = NULL;
#endif

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
  status = rsi_wireless_init(0, 0);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWireless Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nWireless Initialization Success\r\n");
  }

  //! enable debug log prints
  status = rsi_common_debug_log(0, 15);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nEnable debug log prints Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nEnable debug log prints Success\r\n");
  }

  //! Scan for Access points
  status = rsi_wlan_scan((int8_t *)SSID, (uint8_t)CHANNEL_NO, NULL, 0);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWLAN AP Scan Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nWLAN AP Scan Success\r\n");
  }

#if CONNECT_WITH_PMK
  //! Generate pmk
  status = rsi_wlan_pmk_generate(PMK_TYPE, (int8_t *)PSK, (int8_t *)SSID, pmk, PMK_SIZE);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nGenerate pmk Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nGenerate pmk Success\r\n");
  }
#endif

  //!To set the wlan option
  status = rsi_wlan_set(CMD_TYPE, join_bssid, 6);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nSet WLAN Option Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nSet WLAN Option Success\r\n");
  }

#if CONNECT_WITH_PMK
  //! Connect to an Access point through PMK method
  if (SECURITY_TYPE == RSI_WPA2_PMK) {
    status = rsi_wlan_connect(SSID, SECURITY_TYPE, pmk);
  }
#else
  //! Connect to an Access point
  status = rsi_wlan_connect((int8_t *)SSID, SECURITY_TYPE, PSK);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWLAN AP Connect Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nWLAN AP Connect Success\r\n");
  }
#endif

  //! Configure IP
#if DHCP_MODE
  status = rsi_config_ipaddress(RSI_IP_VERSION, dhcp_mode, 0, 0, 0, (uint8_t *)ip_buff, sizeof(ip_buff), 0);
#ifdef RSI_CONFIGURE_IPV6
  rsi_inet_ntop6((const unsigned char *)&ip_buff[1], (char *)link_local_addr, sizeof(link_local_addr));
  rsi_inet_ntop6((const unsigned char *)&ip_buff[5], (char *)global_addr, sizeof(global_addr));
  rsi_inet_ntop6((const unsigned char *)&ip_buff[9], (char *)gateway6, sizeof(gateway6));
#endif
#else
#ifdef RSI_CONFIGURE_IPV6
  status = rsi_inet_pton6(DEVICE_IP6, DEVICE_IP6 + strlen(DEVICE_IP6), hex_addr, ip_addr);
  status = rsi_inet_pton6(GATEWAY6, GATEWAY6 + strlen(GATEWAY6), hex_addr, gateway);
#endif
  status = rsi_config_ipaddress(RSI_IP_VERSION,
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
    return status;
  } else {
    LOG_PRINT("\r\nIP Config Success\r\n");
  }
#ifndef RSI_CONFIGURE_IPV6
  LOG_PRINT("RSI_STA IP ADDR: %d.%d.%d.%d \r\n", ip_buff[6], ip_buff[7], ip_buff[8], ip_buff[9]);

#else
  LOG_PRINT("RSI_STA IP ADDR \r\n");
  LOG_PRINT("prefix length : %d\r\n", ip_buff[0]);
  LOG_PRINT("linklocaladdr: %s\r\n", link_local_addr);
  LOG_PRINT("global addr:   %s\r\n", global_addr);
  LOG_PRINT("gateway addr:  %s\r\n", gateway6);

  status = rsi_inet_pton6(REMOTE_IP, REMOTE_IP + strlen(REMOTE_IP), hex_addr, remote_address);
#endif
  while (packet_count < NUMBER_OF_PACKETS) {

    status = rsi_wlan_ping_async(1, (uint8_t *)&remote_address, size, rsi_ping_response_handler);
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\r\nPing Async Failed, Error Code : 0x%lX\r\n", status);
      return status;
    } else {
      LOG_PRINT("\r\nPing Async Success\r\n");
    }

    ping_rsp_received = 0;
    packet_count++;

#ifndef RSI_WITH_OS
    //! wait for Driver task to complete
    main_loop();
#else
    while (!ping_rsp_received)
      ;
#endif
  }

  return 0;
}
#ifndef RSI_WITH_OS
void main_loop(void)
{
  while (1) {
    ////////////////////////
    //! Application code ///
    ////////////////////////

    //! event loop
    rsi_wireless_driver_task();

    if (ping_rsp_received == 1) {
      ping_rsp_received = 0;
      break;
    }
  }
}
#endif
int main()
{
  int32_t status = RSI_SUCCESS;

#ifdef RSI_WITH_OS

  rsi_task_handle_t wlan_task_handle = NULL;

#endif

#ifdef RSI_WITH_OS
  //! OS case
  //! Task created for WLAN task
  rsi_task_create((rsi_task_function_t)(int32_t)rsi_station_ping_app,
                  (uint8_t *)"wlan_task",
                  RSI_WLAN_TASK_STACK_SIZE,
                  NULL,
                  RSI_WLAN_TASK_PRIORITY,
                  &wlan_task_handle);

  //! OS TAsk Start the scheduler
  rsi_start_os_scheduler();

#else
  //! NON - OS case
  //! Call Ping application
  status = rsi_station_ping_app();

  //! Application main loop
  main_loop();
#endif
  return status;
}

//! ping response notify call back handler
void rsi_ping_response_handler(uint16_t status, const uint8_t *buffer, const uint16_t length)
{
  UNUSED_PARAMETER(status);       //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_CONST_PARAMETER(buffer); //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_CONST_PARAMETER(length); //This statement is added only to resolve compilation warning, value is unchanged
  ping_rsp_received = 1;
}
/*******************************************************************************
* @file  rsi_dual_ip.c
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
 * @brief : This file contains example application for Throughput Measurement with
 * ipv4 and ipv6 simultaneously
 * @section Description :
 * This application will demonstrate the throughput measurement, measuring the
 * throughput from Client to Server and vice versa.
 =================================================================================*/

/*=======================================================================*/
//  ! INCLUDES
/*=======================================================================*/
#include "rsi_driver.h"

// include file to refer data types
#include "rsi_data_types.h"

// COMMON include file to refer wlan APIs
#include "rsi_common_apis.h"

// WLAN include file to refer wlan APIs
#include "rsi_wlan_apis.h"
#include "rsi_wlan_non_rom.h"

// WLAN include file for configuration
#include "rsi_wlan_config.h"

// socket include file to refer socket APIs
#include "rsi_socket.h"

#include "rsi_bootup_config.h"
// Error include files
#include "rsi_error.h"

// OS include file to refer OS specific functionality
#include "rsi_os.h"
#ifndef LINUX_PLATFORM
#include "rsi_hal.h"
#endif

#ifdef RSI_M4_INTERFACE
#include "rsi_board.h"
#include "rsi_chip.h"
#endif

// Access point SSID to connect
#define SSID "SILABS_AP"

//Scan Channel number , 0 - to scan all channels
#define CHANNEL_NO 0

// Security type
#define SECURITY_TYPE RSI_WPA2

// Password
#define PSK "12345678"

#define RSI_CONFIGURE_IPV6 1

#define RSI_CONFIGURE_IPV4 1

#define UDP_BUFF_SIZE_4 1470

#define TCP_BUFF_SIZE_6 1440

// DHCP mode 1- Enable 0- Disable
#define DHCP_MODE 1
//#define RSI_HIGH_PERFORMANCE_SOCKET         1
// If DHCP mode is disabled give IP statically
#if !(DHCP_MODE)
//! IP address of the module
//! E.g: 0x650AA8C0 == 192.168.10.101
#define DEVICE_IP "192.168.0.101" //0x650AA8C0

//! IP address of Gateway
//! E.g: 0x010AA8C0 == 192.168.10.1
#define GATEWAY "192.168.0.1" //0x010AA8C0

//! IP address of netmask
//! E.g: 0x00FFFFFF == 255.255.255.0
#define NETMASK "255.255.255.0" //0x00FFFFFF

//! IPv6 address of the module
#define DEVICE_IP6 "2401:4290:1289:10ed::106"
//! IPv6 address of Gateway
#define GATEWAY6 "2401:4290:1289:10ed::106"

#endif

//Module port number
#define PORT_NUM 5001

// Server port number
#define SERVER_PORT 5001

//! Server IP address.
#define SERVER_IP_ADDRESS "192.168.0.111"
//#else
#define SERVER_IP_ADDRESS_6 "2451:4590:1269:105d::104"
//#endif

// Memory length for driver
#define GLOBAL_BUFF_LEN 15000

// Wlan task priority
#define RSI_APPLICATION_TASK_PRIORITY 1

// Wireless driver task priority
#define RSI_DRIVER_TASK_PRIORITY 2

// Wlan task stack size
#define RSI_APPLICATION_TASK_STACK_SIZE (512 * 2)

// Wireless driver task stack size
#define RSI_DRIVER_TASK_STACK_SIZE (512 * 2)

#ifdef RSI_M4_INTERFACE
#define ICACHE2_ADDR_TRANSLATE_1_REG  *(volatile uint32_t *)(0x20280000 + 0x24)
#define MISC_CFG_SRAM_REDUNDANCY_CTRL *(volatile uint32_t *)(0x46008000 + 0x18)
#define MISC_CONFIG_MISC_CTRL1        *(volatile uint32_t *)(0x46008000 + 0x44)
#define MISC_QUASI_SYNC_MODE          *(volatile uint32_t *)(0x46008000 + 0x84)

#define SOC_PLL_REF_FREQUENCY 40000000  /*<! PLL input REFERENCE clock 40MHZ */
#define PS4_SOC_FREQ          119000000 /*<! PLL out clock 100MHz            */
void switch_m4_frequency(void);
#endif

// Type of throughput
#define UDP_TX_4 0
#define TCP_TX_6 1

#define SOCKET_ASYNC_FEATURE 1
// Power measurement type
#define THROUGHPUT_TYPE_4 UDP_TX_4
#define THROUGHPUT_TYPE_6 TCP_TX_6

// Memory length for send buffer
#define DEFAULT_BUFF_SIZE 1460

#if (THROUGHPUT_TYPE_4 == UDP_TX_4)
#define BUF_SIZE_4 UDP_BUFF_SIZE_4
#endif

#if (THROUGHPUT_TYPE_6 == TCP_TX_6)
#define BUF_SIZE TCP_BUFF_SIZE_6
#endif

// throughput interval configurations
#define MAX_TX_PKTS         10000 // Applies in SSL TX and UDP_RX, calculate throughput after transmitting MAX_TX_PKTS
#define THROUGHPUT_AVG_TIME 30000 // throughput average time in ms
#define CONTINUOUS_THROUGHPUT \
  0 /*! 1- measure throughput for every throughout interval configured,
     0 - measure once for throughout interval configured */

// Throughput parameters
#ifdef FLOAT_PRINT_BYPASS
uint64_t num_bits = 0;
#else
uint32_t num_bits           = 0;
#endif
uint32_t xfer_time;
uint32_t t_start = 0;
uint32_t t_end;
float throughput;

volatile uint8_t data_recvd = 0;
#ifdef FLOAT_PRINT_BYPASS
volatile uint64_t num_bytes = 0;
#else
volatile uint32_t num_bytes = 0;
#endif

// Memory to initialize driv
uint8_t global_buf[GLOBAL_BUFF_LEN];
uint32_t total_tx_bytes = 0;
uint32_t total_rx_bytes;
uint32_t secs;
uint32_t pkt_rx_cnt = 0, tick_count_s = 1;
#ifdef LINUX_PLATFORM
static uint32_t last_tx_print_time = 0;
static uint32_t last_rx_print_time;
#endif

extern int32_t rsi_wlan_buffer_config(void);
extern void rsi_remote_socket_terminate_handler(uint16_t status, uint8_t *buffer, const uint32_t length);
extern uint64_t ip_to_reverse_hex(char *ip);
extern const char *rsi_inet_ntop6(const unsigned char *input, char *dst, rsi_socklen_t size);
extern int rsi_inet_pton6(const char *src, const char *src_endp, unsigned char *dst, unsigned int *ptr_result);

/*====================================================*/
/**
 * @fn         void Throughput(void)
 * @brief      Function to calculate throughput
 * @param[in]  void
 * @return     void
 * @section description
 *====================================================*/

#ifdef LINUX_PLATFORM
void measure_throughput(uint32_t pkt_length, uint32_t tx_rx)
{
  static uint32_t current_time;
  static uint32_t last_print_time;
  uint32_t total_bytes;
  float through_put;
#ifdef LINUX_PLATFORM
  struct rsi_timeval tv1;
  gettimeofday(&tv1, NULL);
  current_time = tv1.tv_sec * 1000000 + tv1.tv_usec;
#else
  current_time = rsi_hal_gettickcount();
#endif
  if (tx_rx == 0) {
    total_tx_bytes += pkt_length;
    total_bytes     = total_tx_bytes;
    last_print_time = last_tx_print_time;
  } else {
    total_rx_bytes += pkt_length;
    total_bytes     = total_rx_bytes;
    last_print_time = last_rx_print_time;
  }
#ifdef LINUX_PLATFORM
  if ((current_time - last_print_time) >= 1000000) //for 1 sec
#else
  if ((current_time - last_print_time) >= 30000) //for 30 sec
#endif
  {
#ifdef LINUX_PLATFORM
    through_put = ((float)(total_bytes) / ((current_time - last_print_time))) * 8;
#else
    through_put = ((float)(total_bytes) / ((current_time - last_print_time))) * 8;
    through_put /= 1000;
#endif
    if (tx_rx == 0) {
#ifdef LINUX_PLATFORM
      printf("\nSecs: %d     Bytes Transmitted %d,Throughput for last %d seconds is = %3.1f Mbps\n",
             secs++,
             total_bytes,
             (current_time - last_print_time) / 1000000,
             through_put);
#endif
      last_tx_print_time = current_time;
      total_tx_bytes     = 0;
    } else {
#ifdef LINUX_PLATFORM
      printf("\nSecs: %d     Bytes Received %d,Throughput for last %d seconds is = %3.1f Mbps\n",
             secs++,
             total_bytes,
             (current_time - last_print_time) / 1000000,
             through_put);
#endif
      last_rx_print_time = current_time;
      total_rx_bytes     = 0;
    }
  }
  return;
}
#else

/*====================================================*/
/**
 * @fn         void measure_throughput(uint32_t total_bytes, uint32_t start_time, uint32_t end_time)
 * @brief      Function to calculate throughput
 * @param[in]  void
 * @return     void
 * @section description
 *====================================================*/
void measure_throughput(uint32_t total_bytes, uint32_t start_time, uint32_t end_time)
{
#ifdef FLOAT_PRINT_BYPASS
  uint32_t through_put;
  through_put = ((float)(total_bytes * 8) / ((end_time - start_time) / tick_count_s));
  through_put /= 1000;
  LOG_PRINT("\r\nThroughput in mbps is : %d\r\n", through_put);
  LOG_PRINT("Time taken in sec: %lu \r\n", (uint32_t)((end_time - start_time) / (1000 * tick_count_s)));
#else
  float through_put;
  through_put = ((float)(total_bytes * 8) / ((end_time - start_time) / tick_count_s));
  through_put /= 1000;
  LOG_PRINT("\r\nThroughput in mbps is : %3.2f\r\n", through_put);
  LOG_PRINT("Time taken in sec: %lu \r\n", (uint32_t)((end_time - start_time) / (1000 * tick_count_s)));
#endif
}
#endif

/*====================================================*/
/**
 * @fn         int32_t application(void)
 * @brief      This function explains how to scan, connect and tx/rx wlan packets from remote device
 * @param[in]  void
 * @return     int32_t
 * @section description
 *====================================================*/
int32_t application()
{
  int32_t client_socket;
  int32_t server_socket, new_socket;
  struct rsi_sockaddr_in server_addr, client_addr;
  struct rsi_sockaddr_in6 server_addr_6, client_addr_6;
  int32_t status = RSI_SUCCESS;
  uint8_t ip_buff[20];
  uint32_t ip_buff_6[13];
  unsigned char hex_addr[RSI_IPV6_ADDRESS_LENGTH] = { 0 };
  unsigned char link_local_addr[46]               = { 0 };
  unsigned char global_addr[46]                   = { 0 };
  unsigned char gateway6[46]                      = { 0 };
  uint32_t server_address_6[4];

#if !SOCKET_ASYNC_FEATURE
  uint32_t recv_size = 0;
  // buffer to receive data over TCP/UDP client socket
  int8_t recv_buffer[BUF_SIZE];
  uint32_t total_bytes_rx = 0;
#endif

#if !(DHCP_MODE)
  uint32_t ip_addr      = ip_to_reverse_hex(DEVICE_IP);
  uint32_t network_mask = ip_to_reverse_hex(NETMASK);
  uint32_t gateway      = ip_to_reverse_hex(GATEWAY);

  uint32_t ip_addr_6[4];
  uint32_t gateway_6[4];
  uint32_t *network_mask_6 = NULL;

#else
  uint8_t dhcp_mode = (RSI_DHCP | RSI_DHCP_UNICAST_OFFER);
#endif

  int8_t send_buf[BUF_SIZE];
  uint16_t i              = 0;
  uint32_t total_bytes_tx = 0, tt_start = 0, tt_end = 0, pkt_cnt = 0;

  // Driver initialization
  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }

#ifdef RSI_M4_INTERFACE
#ifdef RSI_WITH_OS
  tick_count_s = 1;
#endif
  switch_m4_frequency();
  SysTick_Config(SystemCoreClock / (1000 * tick_count_s));
#endif

#ifdef RSI_WITH_OS
  rsi_task_handle_t driver_task_handle = NULL;
  // Task created for Driver task
  rsi_task_create((rsi_task_function_t)rsi_wireless_driver_task,
                  (uint8_t *)"driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);
#endif

  // Silicon labs module intialisation
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nDevice Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  LOG_PRINT("\r\nDevice Initialization Success\r\n");

  // WC initialization
  status = rsi_wireless_init(0, 0);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWireless Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  LOG_PRINT("\r\nWireless Initialization Success\r\n");

  // Send feature frame
  status = rsi_send_feature_frame();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nSend Feature Frame Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  LOG_PRINT("\r\nSend Feature Frame Success\r\n");

  rsi_wlan_register_callbacks(RSI_REMOTE_SOCKET_TERMINATE_CB,
                              rsi_remote_socket_terminate_handler); // Initialize remote terminate call back

#if TX_RX_RATIO_ENABLE
  status = rsi_wlan_buffer_config();
  if (status != RSI_SUCCESS) {
    return status;
  }
#endif
  // Scan for Access points
  status = rsi_wlan_scan((int8_t *)SSID, (uint8_t)CHANNEL_NO, NULL, 0);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWLAN AP Scan Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  LOG_PRINT("\r\nWLAN AP Scan Success\r\n");

  // Connect to an Access point
  status = rsi_wlan_connect((int8_t *)SSID, SECURITY_TYPE, PSK);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWLAN AP Connect Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  LOG_PRINT("\r\nWLAN AP Connect Success\r\n");

  // Configure IP
#if DHCP_MODE
#if RSI_CONFIGURE_IPV4
  status = rsi_config_ipaddress(RSI_IP_VERSION_4, dhcp_mode, 0, 0, 0, (uint8_t *)ip_buff, sizeof(ip_buff), 0);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nIP Config Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nIPv4 Config Success\r\n");
  }
  LOG_PRINT("RSI_STA IP ADDR: %d.%d.%d.%d \r\n", ip_buff[6], ip_buff[7], ip_buff[8], ip_buff[9]);
#endif
#if RSI_CONFIGURE_IPV6
  status = rsi_config_ipaddress(RSI_IP_VERSION_6, dhcp_mode, 0, 0, 0, (uint8_t *)ip_buff_6, sizeof(ip_buff_6), 0);
  rsi_inet_ntop6((const unsigned char *)&ip_buff[1], (char *)link_local_addr, sizeof(link_local_addr));
  rsi_inet_ntop6((const unsigned char *)&ip_buff[5], (char *)global_addr, sizeof(global_addr));
  rsi_inet_ntop6((const unsigned char *)&ip_buff[9], (char *)gateway6, sizeof(gateway6));
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nIP Config Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nIPv6 Config Success\r\n");
  }
  LOG_PRINT("RSI_STA IP ADDR \r\n");
  LOG_PRINT("prefix length : %d\r\n", ip_buff_6[0]);
  LOG_PRINT("linklocaladdr: %s\r\n", link_local_addr);
  LOG_PRINT("global addr:   %s\r\n", global_addr);
  LOG_PRINT("gateway addr:  %s\r\n", gateway6);
#endif
#else
#if RSI_CONFIGURE_IPV4
  status            = rsi_config_ipaddress(RSI_IP_VERSION_4,
                                RSI_STATIC,
                                (uint8_t *)&ip_addr,
                                (uint8_t *)&network_mask,
                                (uint8_t *)&gateway,
                                NULL,
                                0,
                                0);

  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nIP Config Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nIPv4 Config Success\r\n");
  }
#endif
#if RSI_CONFIGURE_IPV6
  status = rsi_inet_pton6(DEVICE_IP6, DEVICE_IP6 + strlen(DEVICE_IP6), hex_addr, ip_addr_6);
  status = rsi_inet_pton6(GATEWAY6, GATEWAY6 + strlen(GATEWAY6), hex_addr, gateway_6);
  status = rsi_config_ipaddress(RSI_IP_VERSION_6,
                                RSI_STATIC,
                                (uint8_t *)&ip_addr_6,
                                (uint8_t *)&network_mask_6,
                                (uint8_t *)&gateway_6,
                                NULL,
                                0,
                                0);

  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nIP Config Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nIPv6 Config Success\r\n");
  }
#endif
#endif

  status =
    rsi_inet_pton6(SERVER_IP_ADDRESS_6, SERVER_IP_ADDRESS_6 + strlen(SERVER_IP_ADDRESS_6), hex_addr, server_address_6);

  for (i = 0; i < BUF_SIZE; i++) {
    send_buf[i] = i;
  }
  LOG_PRINT("\r\nUDP_TX_4 TEST\r\n");
  // Create socket
  client_socket = rsi_socket(AF_INET, SOCK_DGRAM, 0);
  if (client_socket < 0) {
    status = rsi_wlan_get_status();
    LOG_PRINT("\r\nSocket Create Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  LOG_PRINT("\r\nSocket Create Success\r\n");

  // Set server structure
  memset(&server_addr, 0, sizeof(server_addr));

  // Set server address family
  server_addr.sin_family = AF_INET;
  // Set server port number, using htons function to use proper byte order
  server_addr.sin_port = htons(SERVER_PORT);
  // Set IP address to localhost
  server_addr.sin_addr.s_addr = ip_to_reverse_hex((char *)SERVER_IP_ADDRESS);

  LOG_PRINT("\r\nUDP TX start\r\n");
  // take the current time
  tt_start = rsi_hal_gettickcount();
  while (1) {
    // Send data on socket
    status =
      rsi_sendto(client_socket, send_buf, BUF_SIZE_4, 0, (struct rsi_sockaddr *)&server_addr, sizeof(server_addr));

    if (status < 0) {
      status = rsi_wlan_get_status();
      rsi_shutdown(client_socket, 0);
      LOG_PRINT("\r\nFailed to send data to UDP Server, Error Code : 0x%lX\r\n", status);
      return status;
    }

#ifdef LINUX_PLATFORM
    // Measure throughput per second, 0 - Tx and 1 - Rx
    measure_throughput(BUF_SIZE, 0);
#else
    pkt_cnt++;
    // Measure throughput if avg time is reached
    if ((rsi_hal_gettickcount() - tt_start) >= THROUGHPUT_AVG_TIME) {
      // store the time after sending data
      tt_end         = rsi_hal_gettickcount();
      total_bytes_tx = pkt_cnt * BUF_SIZE_4;
      // Measure throughput for every interval of THROUGHPUT_AVG_TIME
#if CONTINUOUS_THROUGHPUT
      // Measure throughput
      measure_throughput(total_bytes_tx, tt_start, tt_end);
      // reset to initial value
      total_bytes_tx = 0;
      pkt_cnt        = 0;
      tt_start       = rsi_hal_gettickcount();
#else
      LOG_PRINT("\r\nUDP TX completed\r\n");
      // Measure throughput
      measure_throughput(total_bytes_tx, tt_start, tt_end);
      pkt_cnt = 0;
      break;
#endif
    }
#endif
  }

  LOG_PRINT("\r\nTCP_TX_6 TEST\r\n");
  // Create socket
  client_socket = rsi_socket(AF_INET6, SOCK_STREAM, 0);
  if (client_socket < 0) {
    status = rsi_wlan_get_status();
    LOG_PRINT("\r\nSocket Create Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  LOG_PRINT("\r\nSocket Create Success\r\n");

  // Memset client structrue
  memset(&client_addr_6, 0, sizeof(client_addr_6));
  // Set family type
  client_addr_6.sin6_family = AF_INET6;
  // Set local port number
  client_addr_6.sin6_port = htons(PORT_NUM);
  // Bind socket
  status = rsi_bind(client_socket, (struct rsi_sockaddr *)&client_addr_6, sizeof(client_addr_6));
  if (status != RSI_SUCCESS) {
    status = rsi_wlan_get_status();
    rsi_shutdown(client_socket, 0);
    LOG_PRINT("\r\nBind Failed, Error code : 0x%lX\r\n", status);
    return status;
  }
  LOG_PRINT("\r\nBind Success\r\n");

  // Set server structure
  memset(&server_addr_6, 0, sizeof(server_addr_6));

  // Set server address family
  server_addr_6.sin6_family = AF_INET6;
  // Set server port number, using htons function to use proper byte order
  server_addr_6.sin6_port = htons(SERVER_PORT);
  // Set IP address to localhost
  memcpy(server_addr_6.sin6_addr._S6_un._S6_u32, server_address_6, RSI_IPV6_ADDRESS_LENGTH);
  // Connect to server socket
  status = rsi_connect(client_socket, (struct rsi_sockaddr *)&server_addr_6, sizeof(server_addr_6));
  if (status != RSI_SUCCESS) {
    status = rsi_wlan_get_status();
    rsi_shutdown(client_socket, 0);
    LOG_PRINT("\r\nConnect to Server Socket Failed, Error Code : 0x%lX\r\n", status);
    return status;
  }
  LOG_PRINT("\r\nConnect to Server Socket Success\r\n");

  LOG_PRINT("\r\nTCP TX Start\r\n");

  // take the current time
  tt_start = rsi_hal_gettickcount();
  while (1) {
    // Send data on socket
    status = rsi_send(client_socket, send_buf, BUF_SIZE, 0);
    if (status < 0) {
      status = rsi_wlan_get_status();
      rsi_shutdown(client_socket, 0);
      LOG_PRINT("\r\nFailed to Send data to TCP Server, Error Code : 0x%lX\r\n", status);
      return status;
    }

#ifdef LINUX_PLATFORM
    // Measure throughput per second, 0 - Tx and 1 - Rx
    measure_throughput(BUF_SIZE, 0);
#else
    pkt_cnt++;
    // Measure throughput if avg time is reached
    if ((rsi_hal_gettickcount() - tt_start) >= THROUGHPUT_AVG_TIME) {
      // store the time after sending data
      tt_end         = rsi_hal_gettickcount();
      total_bytes_tx = pkt_cnt * BUF_SIZE;
#if CONTINUOUS_THROUGHPUT
      // Measure throughput
      measure_throughput(total_bytes_tx, tt_start, tt_end);
      // reset to initial value
      total_bytes_tx = 0;
      pkt_cnt        = 0;
      tt_start       = rsi_hal_gettickcount();
#else
      rsi_shutdown(client_socket, 0);
      LOG_PRINT("\r\nTCP TX completed \r\n");
      // Measure throughput
      measure_throughput(total_bytes_tx, tt_start, tt_end);
      break;
#endif
    }
#endif
  }

  return 0;
}

#ifdef RSI_M4_INTERFACE
void switch_m4_frequency(void)
{
  /*Switch M4 SOC clock to Reference clock*/
  /*Default keep M4 in reference clock*/
  RSI_CLK_M4SocClkConfig(M4CLK, M4_ULPREFCLK, 0);
  /*Configure the PLL frequency*/
  RSI_CLK_SetSocPllFreq(M4CLK, PS4_SOC_FREQ, SOC_PLL_REF_FREQUENCY);
  /*Switch M4 clock to PLL clock for speed operations*/
  RSI_CLK_M4SocClkConfig(M4CLK, M4_SOCPLLCLK, 0);
}
#endif

/*====================================================*/
/**
 * @fn         void rsi_remote_socket_terminate_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
 * @brief      Callback handler to terminate stations remote socket
 * @param[in]  uint16_t status, uint8_t *buffer, const uint32_t length
 * @return     void
 *=====================================================*/
void rsi_remote_socket_terminate_handler(uint16_t status, uint8_t *buffer, const uint32_t length)
{
  UNUSED_PARAMETER(status);
  UNUSED_PARAMETER(buffer);
  UNUSED_CONST_PARAMETER(length);
  data_recvd = 1; // Set data receive flag

  t_end = rsi_hal_gettickcount(); // capture time-stamp after data transfer is completed
}

// main function definition
int main(void)
{
#ifdef RSI_WITH_OS
  rsi_task_handle_t application_handle = NULL;

  // Create application task
  rsi_task_create((rsi_task_function_t)application,
                  (uint8_t *)"application_task",
                  RSI_APPLICATION_TASK_STACK_SIZE,
                  NULL,
                  RSI_APPLICATION_TASK_PRIORITY,
                  &application_handle);

  // Start the scheduler
  rsi_start_os_scheduler();
#else
  application();
#endif
}
/*******************************************************************************
* @file  rsi_wlan_tcp_server_app.c
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
 * @brief : This file contains example application for Throughput Measurement
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
#define SSID "YOUR_AP_SSID"

//Scan Channel number , 0 - to scan all channels
#define CHANNEL_NO 0

// Security type
#define SECURITY_TYPE RSI_WPA2

// Password
#define PSK "YOUR_AP_PASSWORD"

// DHCP mode 1- Enable 0- Disable
#define DHCP_MODE 1

// If DHCP mode is disabled give IP statically
#if !(DHCP_MODE)

// IP address of the module
// E.g: 0x650AA8C0 == 192.168.10.101
#define DEVICE_IP "192.168.10.101" //0x6500A8C0

// IP address of Gateway
// E.g: 0x010AA8C0 == 192.168.10.1
#define GATEWAY "192.168.10.1" //0x010AA8C0

// IP address of netmask
// E.g: 0x00FFFFFF == 255.255.255.0
#define NETMASK "255.255.255.0" //0x00FFFFFF

#endif

//WLAN TCP data transfer
#define TCP_DATA_TEST_CODE 0

//Module port number
#define PORT_NUM 5003

// Server port number
#define SERVER_PORT 5003

// Server IP address.
#define SERVER_IP_ADDRESS "192.168.26.126"

// Memory length for driver
#define GLOBAL_BUFF_LEN 16000

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

#define SOCKET_ASYNC_FEATURE 0

// Type of throughput
#define TCP_RX 1
#define TCP_TX 2

// Power measurement type
//#define THROUGHPUT_TYPE  TCP_RX
#define THROUGHPUT_TYPE TCP_TX

// Memory length for send buffer
#define TCP_BUFF_SIZE     1460
#define DEFAULT_BUFF_SIZE 1460
#define BUF_SIZE          TCP_BUFF_SIZE

#define MAX_TX_PKTS 10000 // SSL TX and RX cases

#define THROUGHPUT_AVG_TIME (30000 * tick_count_s) // throughput average time in ms
#define CONTINUOUS_THROUGHPUT \
  0 /*! 1- measure throughput for every throughout interval configured,
     0 - measure once for throughout interval configured */

#define DISCONNECTION_TIME 100
#define GPIO_PIN           10

//! SEND event number used in the application
#define RSI_SEND_EVENT BIT(0)

#define RSI_APP_BUF_SIZE 1600

//! Load certificate to device flash :
//! Certificate could be loaded once and need not be loaded for every boot up
#define LOAD_CERTIFICATE 1

extern uint8_t ble_connected;

//! Enumeration for states in application
typedef enum rsi_wlan_app_state_e {
  RSI_WLAN_INITIAL_STATE       = 0,
  RSI_WLAN_SCAN_STATE          = 1,
  RSI_WLAN_UNCONNECTED_STATE   = 2,
  RSI_WLAN_CONNECTED_STATE     = 3,
  RSI_WLAN_IPCONFIG_DONE_STATE = 4,
  RSI_WLAN_TCP_RX_STATE        = 5,
  RSI_WLAN_TCP_DATA_RX_STATE   = 6,
  RSI_WLAN_TCP_TX_STATE        = 7,
  RSI_WLAN_TCP_DATA_TX_STATE   = 8,
  RSI_WLAN_TCP_DISCONNECT      = 9,
} rsi_wlan_app_state_t;

//! wlan application control block
typedef struct rsi_wlan_app_cbs {
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
//uint8_t global_buf[GLOBAL_BUFF_LEN];
uint32_t total_tx_bytes = 0;
uint32_t total_rx_bytes;
uint32_t secs;
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

rsi_wlan_app_cb_t rsi_wlan_app_cb; //! application control block

//! Enumeration for commands used in application
typedef enum rsi_app_cmd_e { RSI_DATA = 0 } rsi_app_cmd_t;

#ifdef RSI_WITH_OS
extern rsi_semaphore_handle_t wlan_thread_sem, ble_thread_sem, wlan_tcp_server_sem;
#endif

// Call back for Socket Async
void socket_async_recive(uint32_t sock_no, uint8_t *buffer, uint32_t length)
{
  UNUSED_PARAMETER(sock_no);
  UNUSED_PARAMETER(buffer);

  num_bytes += length;
#ifdef LINUX_PLATFORM
  measure_throughput(length, 1);
#else
//! Measure throughput for every interval
#if CONTINUOUS_THROUGHPUT
  //! compute throughput for every interval of THROUGHPUT_AVG_TIME
  if ((rsi_hal_gettickcount() - t_start) >= THROUGHPUT_AVG_TIME) {
    t_end = rsi_hal_gettickcount();
    compute_throughput();
    num_bytes = 0;
    t_start   = rsi_hal_gettickcount();
  }
#endif
#endif
}

/*====================================================*/
/**
 * @fn         void Throughput(void)
 * @brief      Function to calculate throughput
 * @param[in]  void
 * @return     void
 * @section description
 *====================================================*/
void compute_throughput(void)
{
  num_bits  = num_bytes * 8;                               // number of bits
  xfer_time = ((t_end - t_start) / (1000 * tick_count_s)); // data transfer time
#ifdef FLOAT_PRINT_BYPASS
  uint32_t throughput;
  throughput = (((float)(num_bits) / xfer_time) / 1000000); //Throughput calculation
#ifdef RSI_DEBUG_PRINTS
  LOG_PRINT("throughput in mbps=%lu \r\n", (throughput));
  LOG_PRINT("Time taken in sec: %lu \r\n", xfer_time);
#endif
#else
  throughput = (((float)(num_bits) / xfer_time) / 1000000); //Throughput calculation
#ifdef RSI_DEBUG_PRINTS
  LOG_PRINT("throughput in mbps=%f \r\n", (throughput));
  LOG_PRINT("Time taken in sec: %lu \r\n", xfer_time);
#endif
#endif
}

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
  through_put = ((total_bytes * 8) / ((end_time - start_time) / tick_count_s));
  through_put /= 1000;
  LOG_PRINT("throughput in mbps=%lu \r\n", through_put);
  LOG_PRINT("Time taken in sec: %lu \r\n", (uint32_t)((end_time - start_time) / (1000 * tick_count_s)));
#else
  float through_put;
  through_put = ((float)(total_bytes * 8) / ((end_time - start_time) / tick_count_s));
  through_put /= 1000;
  LOG_PRINT("\r\nThroughput in mbps is : %3.2f\r\n", through_put);
  LOG_PRINT("Time taken in sec: %lu \r\n", (uint32_t)((end_time - start_time) / (1000 * tick_count_s)));
#endif
}

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

void rsi_wlan_app_callbacks_init(void)
{
  //! Initialze remote terminate call back
  rsi_wlan_register_callbacks(RSI_REMOTE_SOCKET_TERMINATE_CB, rsi_remote_socket_terminate_handler);
}

int32_t rsi_wlan_tcp_server_app(void)
{
  int32_t client_socket;
  int32_t server_socket, new_socket;
  struct rsi_sockaddr_in server_addr, client_addr;
  int32_t status = RSI_SUCCESS;
#if !SOCKET_ASYNC_FEATURE
  uint32_t recv_size = 0;
  // buffer to receive data over TCP/UDP client socket
  int8_t recv_buffer[BUF_SIZE];
  uint32_t total_bytes_rx = 0;
#endif
  int32_t addr_size;
  uint8_t high_performance_socket = RSI_HIGH_PERFORMANCE_SOCKET;
#if !(DHCP_MODE)
  uint32_t ip_addr      = ip_to_reverse_hex(DEVICE_IP);
  uint32_t network_mask = ip_to_reverse_hex(NETMASK);
  uint32_t gateway      = ip_to_reverse_hex(GATEWAY);
#else
  uint8_t dhcp_mode = (RSI_DHCP | RSI_DHCP_UNICAST_OFFER);
#endif
  int8_t send_buf[BUF_SIZE];
  uint16_t i              = 0;
  uint32_t total_bytes_tx = 0, tt_start = 0, tt_end = 0, pkt_cnt = 0;

  uint8_t ip_rsp[18]      = { 0 };
  uint8_t fw_version[20]  = { 0 };
  uint8_t drv_version[20] = { 0 };

#ifdef RSI_WITH_OS
  rsi_semaphore_wait(&wlan_tcp_server_sem, 0);
#endif

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

  status = rsi_wlan_get(RSI_FW_VERSION, fw_version, 10);
  if (status != RSI_SUCCESS) {
    return status;
  }

  status = rsi_driver_version(drv_version);
  printf("\r\nDriver Version: %s\r\n", drv_version);

  status = rsi_get_fw_version(fw_version, sizeof(fw_version));
  printf("\r\nFirmware Version: %s\r\n", fw_version);

  rsi_wlan_app_cb.state = RSI_WLAN_INITIAL_STATE;
  while (1) {
    switch (rsi_wlan_app_cb.state) {
      case RSI_WLAN_INITIAL_STATE: {
        rsi_wlan_app_callbacks_init();               //! register callback to initialize WLAN
        rsi_wlan_app_cb.state = RSI_WLAN_SCAN_STATE; //! update WLAN application state to unconnected state
      } break;
      case RSI_WLAN_SCAN_STATE: {
        //! Scan for Access points
        status = rsi_wlan_scan((int8_t *)SSID, 0, NULL, 0);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("Scan Failed...\n");
          rsi_wlan_app_cb.state = RSI_WLAN_SCAN_STATE;
        } else {
          rsi_wlan_app_cb.state = RSI_WLAN_UNCONNECTED_STATE; //! update WLAN application state to unconnected state
          LOG_PRINT("\r\nScan Success\r\n");
        }
      } break;
      case RSI_WLAN_UNCONNECTED_STATE: {
        //! Connect to an Access point
        LOG_PRINT("Wi-Fi Connecting...\n");
        status = rsi_wlan_connect((int8_t *)SSID, SECURITY_TYPE, PSK);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("Wi-Fi Connection failed with %lX\n", status);
          rsi_wlan_app_cb.state = RSI_WLAN_UNCONNECTED_STATE;
        } else {
          rsi_wlan_app_cb.state = RSI_WLAN_CONNECTED_STATE; //! update WLAN application state to unconnected state
        }
      } break;
      case RSI_WLAN_CONNECTED_STATE: {
        LOG_PRINT("\n WLAN connected");
        //! Configure IP
#if DHCP_MODE
        status = rsi_config_ipaddress(RSI_IP_VERSION_4, dhcp_mode, 0, 0, 0, ip_rsp, 18, 0);
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
          rsi_wlan_app_cb.state = RSI_WLAN_CONNECTED_STATE;
        } else {
          LOG_PRINT("\r\nIP Config Success\r\n");
          LOG_PRINT("RSI_STA IP ADDR: %d.%d.%d.%d \r\n", ip_rsp[6], ip_rsp[7], ip_rsp[8], ip_rsp[9]);
          //! update wlan application state
          rsi_wlan_app_cb.state = RSI_WLAN_IPCONFIG_DONE_STATE;
        }

#if ENABLE_POWER_SAVE
        LOG_PRINT("\r\nInitiate module in to power save\r\n");
        //! initiating power save in BLE mode
        status = rsi_bt_power_save_profile(PSP_MODE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nFailed to initiate power save in BLE mode\r\n");
          return status;
        }

        //! initiating power save in wlan mode
        status = rsi_wlan_power_save_profile(PSP_MODE, PSP_TYPE);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nFailed to initiate power save in WLAN mode\r\n");
          return status;
        }
        LOG_PRINT("\r\nModule is in power save\r\n");
#endif
      } break;

#if TCP_DATA_TEST_CODE
      case RSI_WLAN_IPCONFIG_DONE_STATE: {
        if (THROUGHPUT_TYPE == TCP_RX) {
          rsi_wlan_app_cb.state = RSI_WLAN_TCP_RX_STATE;
        } else if (THROUGHPUT_TYPE == TCP_TX) {
          rsi_wlan_app_cb.state = RSI_WLAN_TCP_TX_STATE;
        } else {
          rsi_wlan_app_cb.state = RSI_WLAN_CONNECTED_STATE;
        }
      } break;

      case RSI_WLAN_TCP_RX_STATE: {
        LOG_PRINT("\r\nTCP RX Test\r\n");
#if HIGH_PERFORMANCE_ENABLE
        status = rsi_socket_config();
        if (status < 0) {
          status = rsi_wlan_get_status();
          LOG_PRINT("\r\nSocket config Failed, Error Code : 0x%lX\r\n", status);
          rsi_wlan_app_cb.state = RSI_WLAN_IPCONFIG_DONE_STATE;
        } else {
          LOG_PRINT("\r\nSocket config Success\r\n");
        }
#endif
        // Create socket
#if SOCKET_ASYNC_FEATURE
        server_socket = rsi_socket_async(AF_INET, SOCK_STREAM, 0, socket_async_recive);
#else
        server_socket = rsi_socket(AF_INET, SOCK_STREAM, 0);
#endif
        if (server_socket < 0) {
          status = rsi_wlan_get_status();
          LOG_PRINT("\r\nSocket Create Failed, Error Code : 0x%lX\r\n", status);
          rsi_wlan_app_cb.state = RSI_WLAN_IPCONFIG_DONE_STATE;
        } else {
          LOG_PRINT("\r\nSocket Create Success\r\n");
        }
        status = rsi_setsockopt(server_socket,
                                SOL_SOCKET,
                                SO_HIGH_PERFORMANCE_SOCKET,
                                &high_performance_socket,
                                sizeof(high_performance_socket));
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nSet Socket Option Failed, Error Code : 0x%lX\r\n", status);
          rsi_wlan_app_cb.state = RSI_WLAN_IPCONFIG_DONE_STATE;
        } else {

          // Set server structure
          memset(&server_addr, 0, sizeof(server_addr));

          // Set family type
          server_addr.sin_family = AF_INET;

          // Set local port number
          server_addr.sin_port = htons(SERVER_PORT);
        }

        // Bind socket
        status = rsi_bind(server_socket, (struct rsi_sockaddr *)&server_addr, sizeof(server_addr));
        if (status != RSI_SUCCESS) {
          status = rsi_wlan_get_status();
          rsi_shutdown(server_socket, 0);
          LOG_PRINT("\r\nBind Failed, Error code : 0x%lX\r\n", status);
          rsi_wlan_app_cb.state = RSI_WLAN_IPCONFIG_DONE_STATE;
        } else {
          LOG_PRINT("\r\nBind Success\r\n");
        }

        // Socket listen
        status = rsi_listen(server_socket, 1);
        if (status != RSI_SUCCESS) {
          status = rsi_wlan_get_status();
          rsi_shutdown(server_socket, 0);
          LOG_PRINT("\r\nListen Failed, Error code : 0x%lX\r\n", status);
          rsi_wlan_app_cb.state = RSI_WLAN_IPCONFIG_DONE_STATE;
        } else {
          rsi_wlan_app_cb.state = RSI_WLAN_TCP_DATA_RX_STATE;
          LOG_PRINT("\r\nServer listening on port %d\r\n", SERVER_PORT);

          addr_size = sizeof(server_socket);
        }
      } break;

      case RSI_WLAN_TCP_DATA_RX_STATE: {

        // Socket accept
        new_socket = rsi_accept(server_socket, (struct rsi_sockaddr *)&client_addr, &addr_size);
        if (new_socket < 0) {
          status = rsi_wlan_get_status();
          rsi_shutdown(server_socket, 0);
          LOG_PRINT("\r\nSocket Accept Failed, Error code : 0x%lX\r\n", status);
          rsi_wlan_app_cb.state = RSI_WLAN_IPCONFIG_DONE_STATE;
        } else {
          LOG_PRINT("\r\nClient Connected\r\n");

#if SOCKET_ASYNC_FEATURE
          LOG_PRINT("\r\nTCP RX data start... \r\n");
          data_recvd = 0;
          t_start    = rsi_hal_gettickcount(); // capture time-stamp before data transfer is starts
          while (!data_recvd) {
#ifndef RSI_WITH_OS
            rsi_wireless_driver_task();
#endif
          }
          LOG_PRINT("\r\nTCP RX completed \r\n");

#if !CONTINUOUS_THROUGHPUT
          compute_throughput();
          rsi_wlan_app_cb.state = RSI_WLAN_TCP_DATA_RX_STATE;
#endif
#else
          LOG_PRINT("\r\nReceiving data from the client...\r\n");
          // take the current time
          t_start = rsi_hal_gettickcount();
          while (1) {
            recv_size = BUF_SIZE;

            do {
              // Receive data on socket
              status =
                rsi_recvfrom(new_socket, recv_buffer, recv_size, 0, (struct rsi_sockaddr *)&client_addr, &addr_size);
              if (status <= 0) {
                status = rsi_wlan_socket_get_status(new_socket);
                if (status == RSI_RX_BUFFER_CHECK) {
                  continue;
                }
                // data received completed from remote device
                if (data_recvd) {
                  rsi_shutdown(server_socket, 0);
                  rsi_shutdown(new_socket, 0);
                  LOG_PRINT("\r\nData Reception Completed\r\n");
                  break;
                } else {
                  LOG_PRINT("\r\nTCP Recv Failed, Error code : 0x%lX\r\n", status);
                  rsi_shutdown(server_socket, 0);
                  return status;
                }
              }

              // subtract received bytes
              recv_size -= status;
              total_bytes_rx += status;

#ifdef LINUX_PLATFORM
              // Measure throughput per second, 0 - Tx and 1 - Rx
              measure_throughput(status, 1);
#endif
            } while (recv_size > 0);
            if (data_recvd) {
              break;
            }
          }

          // Measure throughput
          measure_throughput(total_bytes_rx, t_start, t_end);
          data_recvd = 0;
#endif
        }
      } break;

      case RSI_WLAN_TCP_TX_STATE: {
        LOG_PRINT("\r\nTCP_TX TEST\r\n");

        // Create socket
        client_socket = rsi_socket(AF_INET, SOCK_STREAM, 0);
        if (client_socket < 0) {
          status = rsi_wlan_get_status();
          LOG_PRINT("\r\nSocket Create Failed, Error Code : 0x%lX\r\n", status);
          rsi_wlan_app_cb.state = RSI_WLAN_TCP_TX_STATE;
        } else {
          LOG_PRINT("\r\nSocket Create Success\r\n");

          // Memset client structrue
          memset(&client_addr, 0, sizeof(client_addr));

          // Set family type
          client_addr.sin_family = AF_INET;

          // Set local port number
          client_addr.sin_port = htons(PORT_NUM);
        }

        // Bind socket
        status = rsi_bind(client_socket, (struct rsi_sockaddr *)&client_addr, sizeof(client_addr));
        if (status != RSI_SUCCESS) {
          status = rsi_wlan_get_status();
          rsi_shutdown(client_socket, 0);
          LOG_PRINT("\r\nBind Failed, Error code : 0x%lX\r\n", status);
          rsi_wlan_app_cb.state = RSI_WLAN_TCP_TX_STATE;
        } else {
          LOG_PRINT("\r\nBind Success\r\n");

          // Set server structure
          memset(&server_addr, 0, sizeof(server_addr));

          // Set server address family
          server_addr.sin_family = AF_INET;

          // Set server port number, using htons function to use proper byte order
          server_addr.sin_port = htons(SERVER_PORT);

          // Set IP address to localhost
          server_addr.sin_addr.s_addr = ip_to_reverse_hex((char *)SERVER_IP_ADDRESS);
        }

        // Connect to server socket
        status = rsi_connect(client_socket, (struct rsi_sockaddr *)&server_addr, sizeof(server_addr));
        if (status != RSI_SUCCESS) {
          status = rsi_wlan_get_status();
          rsi_shutdown(client_socket, 0);
          rsi_wlan_app_cb.state = RSI_WLAN_TCP_TX_STATE;
          LOG_PRINT("\r\nConnect to Server Socket Failed, Error Code : 0x%lX\r\n", status);
        } else {
          rsi_wlan_app_cb.state = RSI_WLAN_TCP_DATA_TX_STATE;
          LOG_PRINT("\r\nConnected to Server Socket...\r\n");
        }
      } break;

      case RSI_WLAN_TCP_DATA_TX_STATE: {
        LOG_PRINT("\r\nSending data to the server...\r\n");

        uint32_t total_bytes_tx = 0, tt_start = 0, tt_end = 0, pkt_cnt = 0;

        // take the current time
        tt_start = rsi_hal_gettickcount();
        while (1) {
          // Send data on socket
          status = rsi_send(client_socket, send_buf, BUF_SIZE, 0);
          if (status < 0) {
            status = rsi_wlan_get_status();
            rsi_shutdown(client_socket, 0);
            rsi_wlan_app_cb.state = RSI_WLAN_TCP_TX_STATE;
            LOG_PRINT("\r\nFailed to Send data to TCP Server, Error Code : 0x%lX\r\n", status);
            break;
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
            LOG_PRINT("\r\nTCP TX completed \r\n");

            // Measure throughput
            measure_throughput(total_bytes_tx, tt_start, tt_end);
            rsi_wlan_app_cb.state = RSI_WLAN_TCP_DATA_TX_STATE;
            break;
#endif
          }
#endif
        }
      } break;
#endif //TCP_DATA_TEST_CODE
      default:
        break;
    }
  }
  vTaskDelete(NULL);
  return 0;
}

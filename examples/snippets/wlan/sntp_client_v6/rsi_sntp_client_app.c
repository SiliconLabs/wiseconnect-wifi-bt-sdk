/*******************************************************************************
* @file  rsi_sntp_client_app.c
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

/*================================================================================
 * @brief : This file contains example application for SNTP Client
 * @section Description :
 * This application demonstrates how SiLabs device gets info from SNTP server.
 * In this application, SiLabs device connects to Access Point in client mode
 * and connects to SNTP server. After successful connection with SNTP server,
 * application gets time and date info from SNTP server.
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
#include "rsi_sntp_client.h"

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

#include "string.h"
#include "rsi_utils.h"

#include "rsi_nwk.h"
//! configurattion Parameters

//! Access point SSID to connect
#define SSID "SILABS_AP"

//!Scan Channel number , 0 - to scan all channels
#define CHANNEL_NO 0

//! Security type
#define SECURITY_TYPE RSI_OPEN

//! Password
#define PSK NULL

#ifndef RSI_CONFIGURE_IPV6
//! server ip
#define SERVER_IP      0x6704A384
#define RSI_IP_VERSION RSI_IP_VERSION_4
//! flags
#define FLAGS 0
#else
#define SERVER_IP                                  \
  {                                                \
    0x26100020, 0x6f960096, 0x00000000, 0x00000006 \
  }
#define RSI_IP_VERSION RSI_IP_VERSION_6
//! flags
#define FLAGS          1
#endif
//!SNTP Method to USe
#define SNTP_METHOD RSI_SNTP_UNICAST_MODE

//!SNTP retry count value
#define SNTP_RETRY_COUNT 5

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
#define DEVICE_IP6                                 \
  {                                                \
    0x20010db8, 0x00000001, 0x00000000, 0x00000121 \
  }
//! IPv6 address of Gateway
#define GATEWAY6                                   \
  {                                                \
    0x20010db8, 0x00000001, 0x00000000, 0x00000120 \
  }

//! IP address of netmask
#define NETMASK "255.255.255.0" //0x00FFFFFF
#endif
#endif

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

//! SNTP Response buffer length
#define RSI_SNTP_RSP_BUFFER_LENGTH 30

//! standard defines
volatile uint8_t rsp_received;

//! SNTP server info response
rsi_sntp_server_info_rsp_t sntp_server_info_rsp;

//! SNTP server asyncronous response
rsi_sntp_server_rsp_t sntp_server_async_rsp;

//! SNTP time response
volatile uint8_t sntp_time[RSI_SNTP_RSP_BUFFER_LENGTH];

//! SNTP date time response
volatile uint8_t sntp_date_time[RSI_SNTP_RSP_BUFFER_LENGTH];

//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN];

//! Function prototypes
void main_loop(void);

//! SNTP response handler
void rsi_sntp_client_create_response_handler(uint16_t status, const uint8_t cmd_type, const uint8_t *buffer);

//! sntp client  Application
int32_t rsi_sntp_client_app()
{
#ifndef RSI_CONFIGURE_IPV6
  uint8_t ip_buff[20];
#else
  uint32_t ip_buff[13];
#endif
  int32_t status = RSI_SUCCESS;
#if !(DHCP_MODE)
#ifndef RSI_CONFIGURE_IPV6
  uint32_t ip_addr      = DEVICE_IP;
  uint32_t network_mask = NETMASK;
  uint32_t gateway      = GATEWAY;
#else
  uint32_t ip_addr[4]    = DEVICE_IP6;
  uint32_t gateway[4]    = GATEWAY6;
  uint32_t *network_mask = NULL;
#endif
#else
  uint8_t dhcp_mode     = (RSI_DHCP | RSI_DHCP_UNICAST_OFFER);
#endif
#ifndef RSI_CONFIGURE_IPV6
  uint32_t server_ip = SERVER_IP;
#else
  uint32_t server_ip[4] = SERVER_IP;
#endif

  uint16_t length = RSI_SNTP_RSP_BUFFER_LENGTH;

  //! WC initialization
  status = rsi_wireless_init(0, 0);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWireless Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nWireless Initialization Success\r\n");
  }

  //! Scan for Access points
  status = rsi_wlan_scan((int8_t *)SSID, (uint8_t)CHANNEL_NO, NULL, 0);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWLAN AP Scan Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nWLAN AP Scan Success\r\n");
  }

  //! Connect to an Access point
  status = rsi_wlan_connect((int8_t *)SSID, SECURITY_TYPE, PSK);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWLAN AP Connect Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nWLAN AP Connect Success\r\n");
  }

  //! Configure IP
#if DHCP_MODE
  status = rsi_config_ipaddress(RSI_IP_VERSION, dhcp_mode, 0, 0, 0, (uint8_t *)ip_buff, sizeof(ip_buff), 0);
#else
  status                = rsi_config_ipaddress(RSI_IP_VERSION,
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
  LOG_PRINT("linklocaladdr:%x.%x.%x.%x \r\n", ip_buff[1], ip_buff[2], ip_buff[3], ip_buff[4]);
  LOG_PRINT("global addr:  %x.%x.%x.%x \r\n", ip_buff[5], ip_buff[6], ip_buff[7], ip_buff[8]);
  LOG_PRINT("gateway addr: %x.%x.%x.%x \r\n", ip_buff[9], ip_buff[10], ip_buff[11], ip_buff[12]);

#endif

  //! Create sntp client
  status = rsi_sntp_client_create_async((uint8_t)FLAGS,
                                        (uint8_t *)&server_ip,
                                        (uint8_t)SNTP_METHOD,
                                        (uint16_t)SNTP_RETRY_COUNT,
                                        rsi_sntp_client_create_response_handler);
  //! Check for valid status
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nCreate SNTP Client Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nCreate SNTP Client Success\r\n");
  }

  //! wait for the response
  do {
    //! event loop
#ifndef RSI_WITH_OS
    rsi_wireless_driver_task();
#endif

  } while (!rsp_received);

  //! Check for error response
  if (rsp_received != 1) {
    return rsi_wlan_get_nwk_status();
  }
  //! reset rsp received
  rsp_received = 0;

  //! Get time
  status = rsi_sntp_client_gettime((uint16_t)length, (uint8_t *)sntp_time);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nGet Time Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nGet Time Success\r\n");
  }

  //! Check for Error response
  if (rsp_received == 3) {
    //! Return error '-1'
    return rsi_wlan_get_nwk_status();
  }

  //! Get time and date
  status = rsi_sntp_client_gettime_date((uint16_t)length, (uint8_t *)sntp_date_time);
  //! Check for valid status
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nGet Time and Date Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nGet Time and Date Success\r\n");
  }

  //! Check for Error response
  if (rsp_received == 3) {
    //! Return error '-1'
    return rsi_wlan_get_nwk_status();
  }

  //! Get the length of the server info response structure
  length = sizeof(rsi_sntp_server_info_rsp_t);

  //! Get sntp server information
  status = rsi_sntp_client_server_info((uint16_t)length, (uint8_t *)&sntp_server_info_rsp);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nGet SNTP Server Info Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nGet SNTP Server Info Success\r\n");
  }

  //! Check for Error response
  if (rsp_received == 3) {
    return rsi_wlan_get_nwk_status();
  }

  //! Sntp client delete
  status = rsi_sntp_client_delete_async();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nDelete SNTP Client Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nDelete SNTP Client Success\r\n");
  }

  do {
    //! event loop
#ifndef RSI_WITH_OS
    rsi_wireless_driver_task();
#endif

  } while (!rsp_received);

  if (rsp_received != 1) {
    return rsi_wlan_get_nwk_status();
  }

  //! reset rsp received
  rsp_received = 0;
  return status;
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
  int32_t status;
#ifdef RSI_WITH_OS

  rsi_task_handle_t wlan_task_handle = NULL;

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
  //! OS case
  //! Task created for WLAN task
  rsi_task_create((rsi_task_function_t)rsi_sntp_client_app,
                  (uint8_t *)"wlan_task",
                  RSI_WLAN_TASK_STACK_SIZE,
                  NULL,
                  RSI_WLAN_TASK_PRIORITY,
                  &wlan_task_handle);

  //! Task created for Driver task
  rsi_task_create((rsi_task_function_t)rsi_wireless_driver_task,
                  (uint8_t *)"driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);

  //! OS TAsk Start the scheduler
  rsi_start_os_scheduler();

#else
  //! NON - OS case
  //! Call sntp client application
  status = rsi_sntp_client_app();

  //! Application main loop
  main_loop();
#endif
  return status;
}

//! sntp client create response handler
void rsi_sntp_client_create_response_handler(uint16_t status, const uint8_t cmd_type, const uint8_t *buffer)
{

  if (status == RSI_SUCCESS) {
    //! Set SUCCESS response
    rsp_received = 1;

    switch (cmd_type) {
      case RSI_SNTP_CREATE: {
        //! SNTP Create response
      } break;
      case RSI_SNTP_DELETE: {
        //! SNTP Delete response
      } break;
      case RSI_SNTP_GETSERVER_ADDRESS:
      case RSI_SNTP_SERVER_ASYNC_RSP: {
        memcpy(&sntp_server_async_rsp, buffer, sizeof(rsi_sntp_server_rsp_t));
        //! SNTP server asynchronous response
        //! Set FAILURE ASYNCresponse
        LOG_PRINT("\r\nSNTP Failed, SNTP server asynchronous response");
        rsp_received = 3;
      } break;
      default:
        break;
    }
  } else {
    //! Set FAILURE response
    LOG_PRINT("\r\nSNTP Failed, Error Code : 0x%lX\r\n", status);
    rsp_received = 2;
  }
}

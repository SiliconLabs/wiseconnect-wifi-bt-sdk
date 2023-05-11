/*******************************************************************************
* @file  rsi_http_client_app.c
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

#ifdef RSI_M4_INTERFACE
#include "rsi_board.h"
#endif

#include "rsi_wlan_config.h"
//! include file to refer data types
#include "rsi_data_types.h"

//! COMMON include file to refer wlan APIs
#include "rsi_common_apis.h"

//! WLAN include file to refer wlan APIs
#include "rsi_wlan_apis.h"

//! HTTP include file to refer HTTP APIs
#include "rsi_http_client.h"

#include "rsi_utils.h"
//! socket include file to refer socket APIs
#include "rsi_socket.h"

#include "rsi_nwk.h"
//! Error include files
#include "rsi_error.h"

//! OS include file to refer OS specific functionality
#include "rsi_os.h"
#include "rsi_utils.h"
#include "rsi_bootup_config.h"
//! include the certificate
#include "cacert.pem"

//! include the index html page
#include "index.txt"

#include "string.h"
//! standard defines

//! Enable IPv6 set this bit in FLAGS, Default is IPv4
#define HTTPV6 BIT(0)

//! Set HTTPS_SUPPORT to use HTTPS feature
#define HTTPS_SUPPORT BIT(1)

//! Set HTTP_POST_DATA to use HTTP POST LARGE DATA feature
#define HTTP_POST_DATA BIT(5)

//! Set HTTP_V_1_1 to use HTTP version 1.1
#define HTTP_V_1_1 BIT(6)

//! Enable user defined http content type in FLAGS
#define HTTP_USER_DEFINED_CONTENT_TYPE BIT(7)

//! configurattion Parameters

//! Access point SSID to connect
#define SSID "SILABS_AP"

//!Scan Channel number , 0 - to scan all channels
#define CHANNEL_NO 0

//! Security type
#define SECURITY_TYPE RSI_OPEN

//! Password
#define PSK NULL

//!SSL TLS versions

/*
RSI_SSL_V_1_0                       BIT(2)

RSI_SSL_V_1_1                       BIT(4)

RSI_SSL_V_1_2                       BIT(3)

*/

#define SSL_TLS_VERSION RSI_SSL_V_1_1

//! Flags
//! for example select required flag bits,  Eg:(HTTPS_SUPPORT | HTTPV6 | HTTP_USER_DEFINED_CONTENT_TYPE)
#define FLAGS HTTPV6

//! Server port number
#define HTTP_PORT 80

#ifndef RSI_CONFIGURE_IPV6
//! HTTP Server IP address.
#define HTTP_SERVER_IP_ADDRESS "192.168.0.111"
//! set HTTP hostname
#define HTTP_HOSTNAME  "192.168.0.111"
#define RSI_IP_VERSION RSI_IP_VERSION_4
#else
//! HTTP Server IPv6 address.
#define HTTP_SERVER_IP_ADDRESS "2401:4900:4e40:3b72:79f5:83cf:f414:d52b"
//! set HTTP hostname
#define HTTP_HOSTNAME          "2401:4900:4e40:3b72:79f5:83cf:f414:d52b"
#define RSI_IP_VERSION         RSI_IP_VERSION_6
#endif

//! HTTP resource name
#define HTTP_URL "/index.html"

//! set HTTP extended header
//! if NULL , driver fills default extended header
#define HTTP_EXTENDED_HEADER NULL

//! set HTTP hostname
#define USERNAME "admin"

//! set HTTP hostname
#define PASSWORD "admin"

//! HTTP post data
#define HTTP_DATA "employee_name=MR.REDDY&employee_id=RSXYZ123&designation=Engineer&company=SILABS&location=Hyderabad"

//! DHCP mode 1- Enable 0- Disable
#define DHCP_MODE 1

//! If DHCP mode is disabled given IP statically
#if !(DHCP_MODE)
#ifndef RSI_CONFIGURE_IPV6

//! IP address of the module
//! E.g: 0x650AA8C0 == 192.168.10.101
#define DEVICE_IP 0x650AA8C0

//! IP address of Gateway
//! E.g: 0x010AA8C0 == 192.168.10.1
#define GATEWAY 0x010AA8C0

//! IP address of netmask
//! E.g: 0x00FFFFFF == 255.255.255.0
#define NETMASK 0x00FFFFFF

#else
//! IP address of the module
#define DEVICE_IP6 "2401:4290:1289:10ed::106"
//! IP address of Gateway
#define GATEWAY6   "2401:4290:1289:10ed::106"

#endif
#endif

//! Load certificate to device flash :
//! Certificate could be loaded once and need not be loaded for every boot up
#define LOAD_CERTIFICATE 1

//! Enable this macro if Webpage content is returned as HTTP response
#define WEBPAGE_AS_HTTP_PUT_RESPONSE 0

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

//! Application buffer length
#define APP_BUFF_LEN 2000

//! Max HTTP PUT buffer length
#define MAX_HTTP_CLIENT_PUT_BUFFER_LENGTH 900

//! HTTP response received
volatile int32_t rsp_received;

//! End of http put file/content
volatile uint8_t end_of_file;

//! http receive status
volatile int32_t http_recv_status;

#define RSI_HTTP_END_OF_DATA 1

#define RSI_HTTP_CLIENT_SUCCESS 1

//! data buffer length
uint32_t buf_len;

#if RSI_HTTP_STATUS_INDICATION_EN
uint16_t count = 1;
#endif

//!data offset
uint32_t offset;

//! Application Buffer
uint8_t app_buf[APP_BUFF_LEN];

//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN];

//! prototypes
void rsi_http_client_put_response_handler(uint16_t status,
                                          const uint8_t type,
                                          const uint8_t *buffer,
                                          uint16_t length,
                                          const uint8_t end_of_put_pkt);
#if RSI_HTTP_STATUS_INDICATION_EN
void rsi_http_response_handler(uint16_t status,
                               const uint8_t *buffer,
                               const uint16_t length,
                               const uint32_t moredata,
                               uint16_t status_code);
#else
void rsi_http_response_handler(uint16_t status, const uint8_t *buffer, const uint16_t length, const uint32_t moredata);
#endif
int32_t rsi_http_response_status(int32_t *rsp_var);
void rsi_http_reset_handler(void);
extern const char *rsi_inet_ntop6(const unsigned char *input, char *dst, rsi_socklen_t size);
extern int rsi_inet_pton6(const char *src, const char *src_endp, unsigned char *dst, unsigned int *ptr_result);

//! Http client Application
int32_t rsi_http_client_app()
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
  int32_t status = RSI_SUCCESS;
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
  uint16_t flags = FLAGS;

  uint32_t chunk_length = 0;
  uint8_t *file_content = NULL;

  //! Get the length of the index html page
  uint32_t file_size = (sizeof(rsi_index) - 1);

  //! WC initialization
  status = rsi_wireless_init(0, 0);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWireless Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nWireless Initialization Success\r\n");
  }

#if LOAD_CERTIFICATE
  if (flags & HTTPS_SUPPORT) {
    //! Load certificates
    status = rsi_wlan_set_certificate(5, cacert, (sizeof(cacert) - 1));
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\r\nLoad Certificate Failed, Error Code : 0x%lX\r\n", status);
      return status;
    } else {
      LOG_PRINT("\r\nLoad Certificate Success\r\n");
    }
  }
#endif
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
#ifdef RSI_CONFIGURE_IPV6
  rsi_inet_ntop6(&ip_buff[1], link_local_addr, sizeof(link_local_addr));
  rsi_inet_ntop6(&ip_buff[5], &global_addr, sizeof(global_addr));
  rsi_inet_ntop6(&ip_buff[9], &gateway6, sizeof(gateway6));
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
#if DHCP_MODE
  LOG_PRINT("RSI_STA IP ADDR \r\n");
  LOG_PRINT("prefix length : %d\r\n", ip_buff[0]);
  LOG_PRINT("linklocaladdr: %s\r\n", link_local_addr);
  LOG_PRINT("global addr:   %s\r\n", global_addr);
  LOG_PRINT("gateway addr:  %s\r\n", gateway6);
#endif
#endif

  //! Create HTTP client for PUT method
  status = rsi_http_client_put_create();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nCreate HTTP client Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nCreate HTTP client Success\r\n");
  }

  //! Start HTTP client PUT method for the given URL
  status = rsi_http_client_put_start((uint16_t)flags,
                                     (uint8_t *)HTTP_SERVER_IP_ADDRESS,
                                     (uint32_t)HTTP_PORT,
                                     (uint8_t *)HTTP_URL,
                                     (uint8_t *)HTTP_HOSTNAME,
                                     (uint8_t *)HTTP_EXTENDED_HEADER,
                                     (uint8_t *)USERNAME,
                                     (uint8_t *)PASSWORD,
                                     (uint32_t)(sizeof(rsi_index) - 1),
                                     rsi_http_client_put_response_handler);

  //! wait for the success response
  status = rsi_http_response_status((int32_t *)&rsp_received);
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! HTTP client PUT packet
  while (!end_of_file) {
    //! Get the current length that you want to send
    chunk_length = ((file_size - offset) > MAX_HTTP_CLIENT_PUT_BUFFER_LENGTH) ? MAX_HTTP_CLIENT_PUT_BUFFER_LENGTH
                                                                              : (file_size - offset);

    file_content = (uint8_t *)(rsi_index + offset);

    //! Send resource content to the HTTP server for the given URL and total content length
    status = rsi_http_client_put_pkt((uint8_t *)file_content, (uint16_t)chunk_length);

    //! wait for the success response
    status = rsi_http_response_status((int32_t *)&rsp_received);
    if (status != RSI_SUCCESS) {
      return status;
    }

    //! Move content offset by chunk length
    offset += chunk_length;

//! Enable this macro if Webpage content is returned as HTTP response
#if WEBPAGE_AS_HTTP_PUT_RESPONSE
    //! wait for the webpage success as response
    status = rsi_http_response_status(&http_recv_status);
    if (status != RSI_SUCCESS) {
      return status;
    }
#endif
  }
  //! Delete HTTP Client
  status = rsi_http_client_put_delete();

  //! wait for the success response
  status = rsi_http_response_status((int32_t *)&rsp_received);
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! resetting the variables
  rsi_http_reset_handler();

  //! send http get request for the given url
  status = rsi_http_client_get_async((uint16_t)flags,
                                     (uint8_t *)HTTP_SERVER_IP_ADDRESS,
                                     (uint16_t)HTTP_PORT,
                                     (uint8_t *)HTTP_URL,
                                     (uint8_t *)HTTP_HOSTNAME,
                                     (uint8_t *)HTTP_EXTENDED_HEADER,
                                     (uint8_t *)USERNAME,
                                     (uint8_t *)PASSWORD,
                                     rsi_http_response_handler);

  //! wait for the get success response
  status = rsi_http_response_status((int32_t *)&http_recv_status);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nHTTP Get request failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nHTTP Get request Success\r\n");
  }

  //! resetting the variables
  rsi_http_reset_handler();

  //! send http post request for the given url with given http data
  status = rsi_http_client_post_async((uint16_t)FLAGS,
                                      (uint8_t *)HTTP_SERVER_IP_ADDRESS,
                                      (uint16_t)HTTP_PORT,
                                      (uint8_t *)HTTP_URL,
                                      (uint8_t *)HTTP_HOSTNAME,
                                      (uint8_t *)HTTP_EXTENDED_HEADER,
                                      (uint8_t *)USERNAME,
                                      (uint8_t *)PASSWORD,
                                      (uint8_t *)HTTP_DATA,
                                      strlen(HTTP_DATA),
                                      rsi_http_response_handler);

  //! wait for the get success response
  status = rsi_http_response_status((int32_t *)&http_recv_status);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nHTTP Post request failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nHTTP Post request Success\r\n");
  }
  //! resetting the variables
  rsi_http_reset_handler();
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

  //! Silabs module intialisation
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
  rsi_task_create((rsi_task_function_t)rsi_http_client_app,
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
  //! Call HTTP client application
  status = rsi_http_client_app();

  //! Application main loop
  main_loop();
#endif
  return status;
}

//! http put response notify call back handler
void rsi_http_client_put_response_handler(uint16_t status,
                                          const uint8_t type,
                                          const uint8_t *buffer,
                                          uint16_t length,
                                          const uint8_t end_of_put_pkt)
{

  if (status == RSI_SUCCESS) {
    if (type == HTTP_CLIENT_PUT_OFFSET_PKT) {
      if (buf_len + length > APP_BUFF_LEN) {
        //! Application buffer over flowed
        return;
      }
      if (!(end_of_put_pkt & RSI_HTTP_END_OF_DATA)) {
        //! copy received data to application buffer
        memcpy(app_buf + buf_len, buffer, length);

        //! update buffer length
        buf_len += length;
      } else {
        if (length) {
          //! copy received data to application buffer
          memcpy(app_buf + buf_len, buffer, length);

          //! update buffer length
          buf_len += length;
        }

        //! set to exit from driver task
        http_recv_status = RSI_HTTP_CLIENT_SUCCESS;
      }
    } else {
      //! Set success response
      rsp_received = RSI_HTTP_CLIENT_SUCCESS;

      if (type == HTTP_CLIENT_PUT_PKT) {
        //! Get end of file/resource
        end_of_file = end_of_put_pkt;
      } else {
        //! resetting the variables
        rsi_http_reset_handler();
      }
    }
  } else {
    //! resetting the variables
    rsi_http_reset_handler();
  }
}

//! http get/post response notify call back handler
#if RSI_HTTP_STATUS_INDICATION_EN
void rsi_http_response_handler(uint16_t status,
                               const uint8_t *buffer,
                               const uint16_t length,
                               const uint32_t moredata,
                               uint16_t status_code)
#else
void rsi_http_response_handler(uint16_t status, const uint8_t *buffer, const uint16_t length, const uint32_t moredata)
#endif
{

#if RSI_HTTP_STATUS_INDICATION_EN
  if (count == 1) {
    LOG_PRINT("\r\nHTTP response Handler ,Status code :%ld\r\n", status_code);
    count++;
  }
#endif

  if (status == RSI_SUCCESS) {
    if (buf_len + length > APP_BUFF_LEN) {
      //! Application buffer over flowed
      return;
    }
    if (!moredata) {
      //! copy received data to application buffer
      memcpy(app_buf + buf_len, buffer, length);

      //! update buffer length
      buf_len += length;
    } else {
      if (length) {
        //! copy received data to application buffer
        memcpy(app_buf + buf_len, buffer, length);

        //! update buffer length
        buf_len += length;
      }
      //! set to exit from driver task
      http_recv_status = RSI_HTTP_CLIENT_SUCCESS;
    }
  }

  else {
    //!trying to connect non existing TCP server socket
    http_recv_status = status;
    LOG_PRINT("\r\n  , Error Code : 0x%lX\r\n", status);
    rsi_http_reset_handler();
  }
}

int32_t rsi_http_response_status(int32_t *rsp_var)
{
  //! wait for the success response
  do {
    //! event loop
#ifndef RSI_WITH_OS
    rsi_wireless_driver_task();
#endif

  } while (!(*rsp_var));
  *rsp_var = 0;
  if (*rsp_var != RSI_HTTP_CLIENT_SUCCESS) {
    return rsi_wlan_get_nwk_status();
  } else {
    return RSI_SUCCESS;
  }
}

void rsi_http_reset_handler(void)
{
  buf_len     = 0;
  end_of_file = 0;
  offset      = 0;
#if RSI_HTTP_STATUS_INDICATION_EN
  count = 1;
#endif
  return;
}
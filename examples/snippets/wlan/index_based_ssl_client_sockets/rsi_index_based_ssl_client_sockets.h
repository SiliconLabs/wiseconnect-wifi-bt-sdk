/*******************************************************************************
* @file  rsi_index_based_ssl_client_sockets.h
* @brief This file contains application configurations
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
#ifndef RSI_INDEX_BASED_SSL_CLIENT_SOCKETS_H
#define RSI_INDEX_BASED_SSL_CLIENT_SOCKETS_H

/**
 * Include files
 * */

//! WLAN include file to refer wlan APIs
#include "rsi_wlan_apis.h"

#include "rsi_wlan_non_rom.h"

//! MQTT wrappers to include
#include "rsi_mqtt_client.h"

#include "rsi_wlan.h"

#include "rsi_wlan_config.h"

//! COMMON include file to refer wlan APIs
#include "rsi_common_apis.h"

#include "rsi_nwk.h"

//! include file to refer data types
#include "rsi_data_types.h"

//! Driver include file to refer driver APIs
#include "rsi_driver.h"

//! socket include file to refer socket APIs
#include "rsi_socket.h"

//! Error include files
#include "rsi_error.h"

//! HTTP include file to refer HTTP APIs
#include "rsi_http_client.h"

//! OS include file to refer OS specific functionality
#include "rsi_os.h"
#include "rsi_utils.h"
#include "rsi_bootup_config.h"
#include "rsi_http_otaf_app.h"

#ifdef RSI_M4_INTERFACE
#include "rsi_board.h"
#endif

//! standard defines
#include "string.h"

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

//! Enable MQTT ssl feature in FLAGS
#define MQTT_SSL_ENABLE BIT(1)

//! Enable MQTT cert index in FLAGS
#define MQTT_CERT_INDEX_1 BIT(2)

//! configuration Parameters

#define NUMBER_OF_PACKETS 1000

//! Access point SSID to connect
#define SSID "YOUR_AP_SSID"

//!Scan Channel number , 0 - to scan all channels
#define CHANNEL_NO 0

//! Security type
#define SECURITY_TYPE RSI_WPA2

//! Password
#define PSK "YOUR_AP_PASSWORD"

//! DHCP mode 1- Enable 0- Disable
#define DHCP_MODE 1

//! If DHCP mode is disabled give IP statically
#if !(DHCP_MODE)
//! IP address of the module
//! E.g: 0x650AA8C0 == 192.168.10.101
#define DEVICE_IP "192.168.10.101" //0x650AA8C0

//! IP address of Gateway
//! E.g: 0x010AA8C0 == 192.168.10.1
#define GATEWAY "192.168.10.1" //0x010AA8C0

//! IP address of netmask
//! E.g: 0x00FFFFFF == 255.255.255.0
#define NETMASK "255.255.255.0" //0x00FFFFFF
#endif

//! Server port number
#define HTTP_SERVER_PORT 443 //HTTPS Server Port

//! HTTP Server IP address.
#define SERVER_IP_ADDRESS "192.168.10.10" //Server IP Address

//! HTTP resource name
#define HTTP_FW_UPDATE_URL "FIRMWARE_UPDATE_URL" //Index-3 used for FW update

//! HTTP resource name
#define HTTP_SERVER_URL "FIRMWARE_LISTING_URL" //Index-2 used for Listing the FW and Certificates

//! HTTP resource name
#define HTTP_SERVER_URL1 "/index.html" //Index-0 Connection for HTTPS Server

#define HTTP_CERTS_DOWNLOAD_URL "CERTIFICATE_URL" //Index-3 used for cert download

//! set HTTP hostname
#define HTTP_HOSTNAME "192.168.10.10"

//! set HTTP extended header
//! if NULL , driver fills default extended header
#define HTTP_EXTENDED_HEADER NULL

//! set HTTP hostname
#define USERNAME "admin"

//! set HTTP hostname
#define PASSWORD "admin"

//! HTTP post data
#define HTTP_DATA "HTTP_DATA_TO_BE_POSTED"

#define HTTP_INDEX0_FLAGS (HTTPS_SUPPORT)                       //Index 0 for HTTPS connection
#define MQTT_INDEX1_FLAGS (MQTT_SSL_ENABLE | MQTT_CERT_INDEX_1) //Index 1 for MQTT Connection
#define HTTP_INDEX2_FLAGS (HTTPS_SUPPORT | HTTPS_CERT_INDEX_2)  //Index 2 for HTTPS connection. FW & Certs Listing
#define HTTP_INDEX3_FLAGS (HTTPS_SUPPORT | HTTPS_CERT_INDEX_3)  //Index 3 for FW update.

//! Load certificate to device flash :
//! Certificate could be loaded once and need not be loaded for every boot up
#define LOAD_CERTIFICATE 1

//! SSL bitmap 1- Enable 0- Disable
#define RSI_SSL_BIT_ENABLE 1

//! Device port number for 1st connection
#define DEVICE_PORT1 5001

//! Device port number for 2nd connection
#define DEVICE_PORT2 5003

//! Device port number for 3rd connection
#define DEVICE_PORT3 5005

//! Device port number for 3rd index socket connection
#define DEVICE_PORT4 5007

//! Server port number for 1st connection
#define HTTP_SERVER_PORT1 5443

//! Server port number for 2nd connection
#define MQTT_SERVER_PORT 8883

//! Server port number for 3rd connection
#define HTTP_SERVER_PORT2 8443

//! Client port number
#define MQTT_CLIENT_PORT 1883

//! Memory length for driver
#define GLOBAL_BUFF_LEN 15000

#define MAX_SSL_CERTS_TO_FLASH 3

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

//! HTTP OTAF success status
#define RSI_HTTP_OTAF_UPGRADE_SUCCESS 1

//! HTTP OTAF Failed status
#define RSI_HTTP_OTAF_UPGRADE_FAILED 2

//! Firmware file name length
#define RSI_FIRMWARE_NAME_SIZE 20

//! HTTP OTAF semaphore wait timeout
#define HTTP_OTAF_SEM_WAIT_TIMEOUT 0

#define RSI_APP_BUF_SIZE 1600

#define CACERT_BUF_SIZE 4096

//! Application Buffer
extern uint8_t app_buf[APP_BUFF_LEN];

//! Memory to initialize driver
extern uint8_t global_buf[GLOBAL_BUFF_LEN];

#define RSI_HTTP_END_OF_DATA 1

#define RSI_HTTP_CLIENT_SUCCESS 1

//! data buffer length
extern uint32_t buf_len;

//! HTTP response received
extern volatile int32_t rsp_received;

//! End of http put file/content
extern volatile uint8_t end_of_file;

//!data offset
extern uint32_t offset;

typedef struct http_response_s {
  uint8_t *output_data;
  uint16_t output_len;
} http_response_t;

extern uint8_t cacert_buf[CACERT_BUF_SIZE];
extern uint32_t cacert_len;

//! http receive status
extern volatile int32_t http_recv_status;

//! Enumeration for states in application
typedef enum rsi_wlan_app_state_e {
  RSI_WLAN_INITIAL_STATE                 = 0,
  RSI_WLAN_SCAN_STATE                    = 1,
  RSI_WLAN_UNCONNECTED_STATE             = 2,
  RSI_WLAN_CONNECTED_STATE               = 3,
  RSI_WLAN_IPCONFIG_DONE_STATE           = 4,
  RSI_WLAN_HTTP_CLIENT_INDEX0_STATE      = 5,
  RSI_WLAN_MQTT_CLIENT_INDEX1_STATE      = 6,
  RSI_WLAN_HTTP_CLIENT_INDEX2_STATE      = 7,
  RSI_WLAN_HTTP_OTA_UPDATE_INDEX3_STATE  = 8,
  RSI_WLAN_HTTP_OTA_WIRELESS_DEINT_STATE = 9,
  RSI_WLAN_HTTP_OTA_UPDATE_DONE          = 10
} rsi_wlan_app_state_t;

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

#if ENABLE_POWER_SAVE
//! Power Save Profile mode
#define PSP_MODE RSI_SLEEP_MODE_2

//! Power Save Profile type
#define PSP_TYPE RSI_MAX_PSP

int32_t rsi_wlan_power_save_profile(uint8_t psp_mode, uint8_t psp_type);
#endif

//! MQTT defines
//! Keep alive period
#define RSI_KEEP_ALIVE_PERIOD 100

//!Memory to initialize MQTT client Info structure
#define MQTT_CLIENT_INIT_BUFF_LEN 3500

//! MQTT Connection ack
#define MQTT_CONNACK 0x20

//! MQTT Subscription ack
#define MQTT_SUBACK 0x90

//! QOS of the message
#define QOS 0

//! MQTT Related Macros and declarations
#define RSI_MQTT_TOPIC "MQTT_DEMO"

#ifdef RSI_M4_INTERFACE
uint32_t xtal_enable = 1;
#endif

#ifdef ASYNC_MQTT
rsi_mqtt_client_info_t *rsi_mqtt_client = NULL;
uint8_t connack, suback, data_received;
uint8_t rcvbuf[2048];
uint8_t pub = 1;
#endif //ASYNC_MQTT

//! prototypes
void http_otaf_response_handler(uint16_t status, const uint8_t *buffer);
int32_t rsi_http_response_status(int32_t *rsp_var);
int32_t rsi_http_fw_update_response_status(int32_t *rsp_var);
uint64_t ip_to_reverse_hex(char *ip);
#if RSI_HTTP_STATUS_INDICATION_EN
void rsi_http_response_handler(uint16_t status,
                               const uint8_t *buffer,
                               const uint16_t length,
                               const uint32_t moredata,
                               uint16_t status_code);
#else
void rsi_http_response_handler(uint16_t status, const uint8_t *buffer, const uint16_t length, const uint32_t moredata);
#endif
void rsi_http_reset_handler(void);
int32_t rsi_http_client_fw_listing_task();
void output_values(http_response_t response);
int32_t rsi_http_client_task();
int32_t rsi_mqtt_client_task();
void rsi_http_client_put_response_handler(uint16_t status,
                                          const uint8_t type,
                                          const uint8_t *buffer,
                                          uint16_t length,
                                          const uint8_t end_of_put_pkt);

#endif /* RSI_INDEX_BASED_SSL_CLIENT_SOCKETS_H */

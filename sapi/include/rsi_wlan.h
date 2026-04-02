/*******************************************************************************
* @file  rsi_wlan.h
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

#ifndef RSI_WLAN_H
#define RSI_WLAN_H

#include <rsi_os.h>
#include <rsi_pkt_mgmt.h>
#include <rsi_wlan_defines.h>
#include "rsi_socket.h"
#include "rsi_wlan_non_rom.h"
/******************************************************
 * *                      Macros
 * ******************************************************/
// Maximum length of PSK
#define RSI_PSK_LEN 64

// length of the PMK
#define RSI_PMK_LEN 32

// Host descriptor length
#define RSI_HOST_DESC_LENGTH 16

// Maximum payload size
#define RSI_MAX_PAYLOAD_SIZE 1500

// IP address length in bytes
#define RSI_IP_ADDRESS_LEN 4

// Maximum certificate length supported in one chunk
#define RSI_MAX_CERT_SEND_SIZE 1400

// Maximum certificate length supported in one chunk
#define RSI_MAX_CERT_RECV_SIZE 1400

// country code length in set region command
#define RSI_COUNTRY_CODE_LENGTH 3

// maximum no of channels to set rules
#define RSI_MAX_POSSIBLE_CHANNEL 24

// maximum no of channels to set rules
#define RSI_MAX_POSSIBLE_SINGLE_BAND_CHANNEL 14

#define RSI_SINGLE_NON_PREF_CHAN_PARAMS_LEN 15

// length of wps pin
#define RSI_WPS_PIN_LEN 8

// BIT to identify set mac to give before init
#define RSI_SET_MAC_BIT BIT(0)

// Websocket max url length
#define RSI_WEBS_MAX_URL_LENGTH 51

// Websocket max host length
#define RSI_WEBS_MAX_HOST_LENGTH 51

// snmp response buf max length
#define MAX_SNMP_VALUE 200

// snmp trap buffer length
#define RSI_SNMP_TRAP_BUFFER_LENGTH 1024

// Maximum chunk size of firmware upgradation
#define RSI_MAX_FWUP_CHUNK_SIZE 1024

// Maximum chunk size of OTA firmware upgradation
#define RSI_MAX_OTA_FWUP_CHUNK_SIZE 800

// RPS header size
#define RSI_RPS_HEADER_SIZE 64

#define RSI_WPS_NOT_SUPPORTED 0x86

// Auto config going on bitmap
#define RSI_AUTO_CONFIG_FAILED BIT(1)

// Auto config going on bitmap
#define RSI_AUTO_CONFIG_GOING_ON BIT(2)

// Auto config done bitmap
#define RSI_AUTO_CONFIG_DONE BIT(3)

#define RSI_USER_STORE_CFG_STATUS 0x5a5a

// frame type of raw data packets for host stack
#define DUAL_HOST_RAW_DATA_PACKET 1

// AKM suite types
#define RSN_SELECTOR(d, c, b, a) \
  ((((uint32_t)(a)) << 24) | (((uint32_t)(b)) << 16) | (((uint32_t)(c)) << 8) | (uint32_t)(d))
#define RSN_AKM_COUNT_OFFSET                   10
#define RSN_AKM_OFFSET                         12
#define RSN_SELECTOR_LEN                       4
#define WPA_DRIVER_CAPA_KEY_MGMT_WPA           0x00000001
#define WPA_DRIVER_CAPA_KEY_MGMT_WPA2          0x00000002
#define WPA_DRIVER_CAPA_KEY_MGMT_WPA_PSK       0x00000004
#define WPA_DRIVER_CAPA_KEY_MGMT_WPA2_PSK      0x00000008
#define WPA_DRIVER_CAPA_KEY_MGMT_WPA_NONE      0x00000010
#define WPA_DRIVER_CAPA_KEY_MGMT_SAE           0x00010000
#define WPA_DRIVER_CAPA_KEY_MGMT_FT_SAE        0x00100000
#define WPA_DRIVER_CAPA_KEY_MGMT_802_1X_SHA256 0x00020000
#define WPA_DRIVER_CAPA_KEY_MGMT_PSK_SHA256    0x00040000

#define RSN_AUTH_KEY_MGMT_UNSPEC_802_1X   RSN_SELECTOR(0x00, 0x0f, 0xac, 1)
#define RSN_AUTH_KEY_MGMT_PSK_OVER_802_1X RSN_SELECTOR(0x00, 0x0f, 0xac, 2)
#define RSN_AUTH_KEY_MGMT_FT_802_1X       RSN_SELECTOR(0x00, 0x0f, 0xac, 3)
#define RSN_AUTH_KEY_MGMT_FT_PSK          RSN_SELECTOR(0x00, 0x0f, 0xac, 4)
#define RSN_AUTH_KEY_MGMT_802_1X_SHA256   RSN_SELECTOR(0x00, 0x0f, 0xac, 5)
#define RSN_AUTH_KEY_MGMT_PSK_SHA256      RSN_SELECTOR(0x00, 0x0f, 0xac, 6)
#define RSN_AUTH_KEY_MGMT_TPK_HANDSHAKE   RSN_SELECTOR(0x00, 0x0f, 0xac, 7)
#define RSN_AUTH_KEY_MGMT_SAE             RSN_SELECTOR(0x00, 0x0f, 0xac, 8)
#define RSN_AUTH_KEY_MGMT_FT_SAE          RSN_SELECTOR(0x00, 0x0f, 0xac, 9)

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/

// enumeration for WLAN control block state machine
typedef enum rsi_wlan_state_e {
  RSI_WLAN_STATE_NONE = 0,
  RSI_WLAN_STATE_OPERMODE_DONE,
  RSI_WLAN_STATE_BAND_DONE,
  RSI_WLAN_STATE_INIT_DONE,
  RSI_WLAN_STATE_SCAN_DONE,
  RSI_WLAN_STATE_CONNECTED,
  RSI_WLAN_STATE_IP_CONFIG_DONE,
  RSI_WLAN_STATE_IPV6_CONFIG_DONE,
  RSI_WLAN_STATE_AUTO_CONFIG_GOING_ON,
  RSI_WLAN_STATE_AUTO_CONFIG_DONE,
  RSI_WLAN_STATE_AUTO_CONFIG_FAILED,
} rsi_wlan_state_t;

// enumeration for WLAN command response codes
typedef enum rsi_wlan_cmd_response_e {
  RSI_WLAN_RSP_CLEAR                   = 0x00,
  RSI_WLAN_RSP_BAND                    = 0x11,
  RSI_WLAN_RSP_INIT                    = 0x12,
  RSI_WLAN_RSP_SCAN                    = 0x13,
  RSI_WLAN_RSP_JOIN                    = 0x14,
  RSI_WLAN_RSP_CONFIG                  = 0XBE,
  RSI_WLAN_RSP_SET_MAC_ADDRESS         = 0x17,
  RSI_WLAN_RSP_QUERY_NETWORK_PARAMS    = 0x18,
  RSI_WLAN_RSP_DISCONNECT              = 0x19,
  RSI_WLAN_RSP_AP_STOP                 = 0xAE,
  RSI_WLAN_RSP_SET_REGION              = 0x1D,
  RSI_WLAN_RSP_CFG_SAVE                = 0x20,
  RSI_WLAN_RSP_AUTO_CONFIG_ENABLE      = 0x21,
  RSI_WLAN_RSP_GET_CFG                 = 0x22,
  RSI_WLAN_RSP_USER_STORE_CONFIG       = 0x23,
  RSI_WLAN_RSP_AP_CONFIGURATION        = 0x24,
  RSI_WLAN_RSP_SET_WEP_KEYS            = 0x25,
  RSI_WLAN_RSP_PING_PACKET             = 0x29,
  RSI_WLAN_RSP_TWT_AUTO_CONFIG         = 0x2E,
  RSI_WLAN_RSP_TWT_PARAMS              = 0x2F,
  RSI_WLAN_RSP_P2P_CONNECTION_REQUEST  = 0x30,
  RSI_WLAN_RSP_SET_PROFILE             = 0x31,
  RSI_WLAN_RSP_GET_PROFILE             = 0x32,
  RSI_WLAN_RSP_DELETE_PROFILE          = 0x33,
  RSI_WLAN_RSP_RSSI                    = 0x3A,
  RSI_WLAN_RSP_BEACON_STOP             = 0x63,
  RSI_WLAN_RSP_IPCONFV4                = 0x41,
  RSI_WLAN_RSP_SOCKET_CREATE           = 0x42,
  RSI_WLAN_RSP_SOCKET_CLOSE            = 0x43,
  RSI_WLAN_RSP_WMM_PARAMS              = 0x45,
  RSI_WLAN_RSP_EAP_CONFIG              = 0x4C,
  RSI_WLAN_RSP_FW_VERSION              = 0x49,
  RSI_WLAN_RSP_MAC_ADDRESS             = 0x4A,
  RSI_WLAN_RSP_GET_DEVICE_ID           = 0xBB,
  RSI_WLAN_RSP_QUERY_GO_PARAMS         = 0x4E,
  RSI_WLAN_RSP_CONN_ESTABLISH          = 0x61,
  RSI_WLAN_RSP_REMOTE_TERMINATE        = 0x62,
  RSI_WLAN_RSP_SOCKET_READ_DATA        = 0x6B,
  RSI_WLAN_RSP_SOCKET_ACCEPT           = 0x6C,
  RSI_WLAN_RSP_IPCONFV6                = 0xA1,
  RSI_WLAN_RSP_HOST_PSK                = 0xA5,
  RSI_WLAN_RSP_SOCKET_CONFIG           = 0xA7,
  RSI_WLAN_RSP_IPV4_CHANGE             = 0xAA,
  RSI_WLAN_RSP_TCP_ACK_INDICATION      = 0xAB,
  RSI_WLAN_RSP_UART_DATA_ACK           = 0xAC,
  RSI_WLAN_RSP_SET_MULTICAST_FILTER    = 0x40,
  RSI_WLAN_RSP_GAIN_TABLE              = 0x47,
  RSI_WLAN_RSP_SET_CERTIFICATE         = 0x4D,
  RSI_WLAN_RSP_DNS_QUERY               = 0x44,
  RSI_WLAN_RSP_DNS_UPDATE              = 0xED,
  RSI_WLAN_RSP_CONNECTION_STATUS       = 0x48,
  RSI_WLAN_RSP_CONFIGURE_P2P           = 0x4B,
  RSI_WLAN_RSP_HTTP_CLIENT_GET         = 0x51,
  RSI_WLAN_RSP_HTTP_CLIENT_POST        = 0x52,
  RSI_WLAN_RSP_HTTP_CLIENT_POST_DATA   = 0xEB,
  RSI_WLAN_RSP_HTTP_CLIENT_PUT         = 0x53,
  RSI_WLAN_RSP_WFD_DEVICE              = 0x54,
  RSI_WLAN_RSP_DNS_SERVER_ADD          = 0x55,
  RSI_WLAN_RSP_NWK_APP_PROTOCOL_CONFIG = 0x58,
  RSI_WLAN_RSP_WIRELESS_FWUP_OK        = 0x59,
  RSI_WLAN_RSP_WIRELESS_FWUP_DONE      = 0x5A,
  RSI_WLAN_RSP_BG_SCAN                 = 0x6A,
  RSI_WLAN_RSP_HT_CAPABILITIES         = 0x6D,
  RSI_WLAN_RSP_REJOIN_PARAMS           = 0x6F,
  RSI_WLAN_RSP_WPS_METHOD              = 0x72,
  RSI_WLAN_RSP_ROAM_PARAMS             = 0x7B,
  RSI_WLAN_RSP_TX_TEST_MODE            = 0x7C,
  RSI_WLAN_RSP_WMM_PS                  = 0x97,
  RSI_WLAN_RSP_FWUP                    = 0x99,
  RSI_WLAN_RSP_RX_STATS                = 0xA2,
  RSI_WLAN_RSP_MULTICAST               = 0xB1,
  RSI_WLAN_RSP_SET_SNI                 = 0x6E,
  RSI_WLAN_RSP_HTTP_ABORT              = 0xB3,
  RSI_WLAN_RSP_HTTP_CREDENTIALS        = 0xB4,
#ifdef RSI_WAC_MFI_ENABLE
  RSI_WLAN_RSP_ADD_MFI_IE = 0xB5,
#endif
#ifndef RSI_M4_INTERFACE
  RSI_WLAN_RSP_CERT_VALID = 0xBC,
#endif
  RSI_WLAN_RSP_SET_REGION_AP         = 0xBD,
  RSI_WLAN_RSP_CLIENT_CONNECTED      = 0xC2,
  RSI_WLAN_RSP_CLIENT_DISCONNECTED   = 0xC3,
  RSI_WLAN_RSP_FREQ_OFFSET           = 0xF3,
  RSI_WLAN_RSP_CALIB_WRITE           = 0xCA,
  RSI_WLAN_RSP_CALIB_READ            = 0xCF,
  RSI_WLAN_RSP_DYNAMIC_POOL          = 0xC7,
  RSI_WLAN_RSP_FILTER_BCAST_PACKETS  = 0xC9,
  RSI_WLAN_RSP_EMB_MQTT_CLIENT       = 0xCB,
  RSI_WLAN_RSP_EMB_MQTT_PUBLISH_PKT  = 0xCC,
  RSI_WLAN_RSP_MQTT_REMOTE_TERMINATE = 0xF0,
  RSI_WLAN_RSP_MDNSD                 = 0xDB,
  RSI_WLAN_RSP_FTP                   = 0xE2,
  RSI_WLAN_RSP_FTP_FILE_WRITE        = 0xE3,
  RSI_WLAN_RSP_SNTP_CLIENT           = 0xE4,
  RSI_WLAN_RSP_SNTP_SERVER           = 0xE5,
  RSI_WLAN_RSP_SMTP_CLIENT           = 0xE6,
  RSI_WLAN_RSP_OTA_FWUP              = 0xEF,
  RSI_WLAN_RSP_WEBPAGE_LOAD          = 0x50,
  RSI_WLAN_RSP_JSON_LOAD             = 0x9c,
  RSI_WLAN_RSP_WEBPAGE_ERASE         = 0x9A,
  RSI_WLAN_RSP_JSON_OBJECT_ERASE     = 0x9B,
  RSI_WLAN_RSP_WEBPAGE_CLEAR_ALL     = 0x7F,
  RSI_WLAN_RSP_HOST_WEBPAGE_SEND     = 0x56,
  RSI_WLAN_RSP_JSON_UPDATE           = 0x9D,
  RSI_WLAN_RSP_GET_RANDOM            = 0xF8,
  RSI_WLAN_RSP_ASYNCHRONOUS          = 0xFF,
  RSI_WLAN_RSP_JSON_EVENT            = 0xEE,
  RSI_WLAN_RSP_POP3_CLIENT           = 0xE7,
  RSI_WLAN_RSP_POP3_CLIENT_TERMINATE = 0xE8,
  RSI_WLAN_RSP_DHCP_USER_CLASS       = 0xEC,
  RSI_WLAN_RSP_TIMEOUT               = 0xEA,
  RSI_WLAN_RSP_URL_REQUEST           = 0x64,
  RSI_WLAN_RSP_MODULE_STATE          = 0x70,
  RSI_WLAN_RSP_TWT_ASYNC             = 0x71,
  RSI_WLAN_RSP_SELECT_REQUEST        = 0x74,
  RSI_WLAN_RSP_SCAN_RESULTS          = 0xAF,
  RSI_WLAN_RSP_RADIO                 = 0x81,
  RSI_WLAN_RSP_GET_STATS             = 0xF1,
  RSI_WLAN_RSP_HTTP_OTAF             = 0xF4,
  RSI_WLAN_RSP_UPDATE_TCP_WINDOW     = 0xF5,
  RSI_WLAN_RATE_RSP_STATS            = 0x88,
  RSI_WLAN_RSP_GET_CSI_DATA          = 0xC4,
  RSI_WLAN_RSP_GET_DPD_DATA          = 0xDC,
  RSI_WLAN_RSP_EXT_STATS             = 0x68
} rsi_wlan_cmd_response_t;

// enumeration for WLAN command request codes
typedef enum rsi_wlan_cmd_request_e {
  RSI_WLAN_REQ_BAND                    = 0x11,
  RSI_WLAN_REQ_INIT                    = 0x12,
  RSI_WLAN_REQ_SCAN                    = 0x13,
  RSI_WLAN_REQ_JOIN                    = 0x14,
  RSI_WLAN_REQ_CONFIG                  = 0xBE,
  RSI_WLAN_REQ_SET_SLEEP_TIMER         = 0x16,
  RSI_WLAN_REQ_SET_MAC_ADDRESS         = 0x17,
  RSI_WLAN_REQ_QUERY_NETWORK_PARAMS    = 0x18,
  RSI_WLAN_REQ_DISCONNECT              = 0x19,
  RSI_WLAN_REQ_AP_STOP                 = 0xAE,
  RSI_WLAN_REQ_SET_REGION              = 0x1D,
  RSI_WLAN_REQ_CFG_SAVE                = 0x20,
  RSI_WLAN_REQ_AUTO_CONFIG_ENABLE      = 0x21,
  RSI_WLAN_REQ_GET_CFG                 = 0x22,
  RSI_WLAN_REQ_USER_STORE_CONFIG       = 0x23,
  RSI_WLAN_REQ_AP_CONFIGURATION        = 0x24,
  RSI_WLAN_REQ_SET_WEP_KEYS            = 0x25,
  RSI_WLAN_REQ_PING_PACKET             = 0x29,
  RSI_WLAN_REQ_TWT_AUTO_CONFIG         = 0x2E,
  RSI_WLAN_REQ_TWT_PARAMS              = 0x2F,
  RSI_WLAN_REQ_SET_PROFILE             = 0x31,
  RSI_WLAN_REQ_GET_PROFILE             = 0x32,
  RSI_WLAN_REQ_DELETE_PROFILE          = 0x33,
  RSI_WLAN_REQ_RSSI                    = 0x3A,
  RSI_WLAN_REQ_BEACON_STOP             = 0x63,
  RSI_WLAN_REQ_IPCONFV4                = 0x41,
  RSI_WLAN_REQ_SOCKET_CREATE           = 0x42,
  RSI_WLAN_REQ_SOCKET_CLOSE            = 0x43,
  RSI_WLAN_REQ_WMM_PARAMS              = 0x45,
  RSI_WLAN_REQ_GET_DEVICE_ID           = 0xBB,
  RSI_WLAN_REQ_EAP_CONFIG              = 0x4C,
  RSI_WLAN_REQ_FW_VERSION              = 0x49,
  RSI_WLAN_REQ_MAC_ADDRESS             = 0x4A,
  RSI_WLAN_REQ_QUERY_GO_PARAMS         = 0x4E,
  RSI_WLAN_REQ_SOCKET_READ_DATA        = 0x6B,
  RSI_WLAN_REQ_SOCKET_ACCEPT           = 0x6C,
  RSI_WLAN_REQ_SET_SNI                 = 0x6E,
  RSI_WLAN_REQ_IPCONFV6                = 0x90,
  RSI_WLAN_REQ_HOST_PSK                = 0xA5,
  RSI_WLAN_REQ_SET_MULTICAST_FILTER    = 0x40,
  RSI_WLAN_REQ_GAIN_TABLE              = 0x47,
  RSI_WLAN_REQ_SET_CERTIFICATE         = 0x4D,
  RSI_WLAN_REQ_DNS_QUERY               = 0x44,
  RSI_WLAN_REQ_DNS_UPDATE              = 0xED,
  RSI_WLAN_REQ_CONNECTION_STATUS       = 0x48,
  RSI_WLAN_REQ_CONFIGURE_P2P           = 0x4B,
  RSI_WLAN_REQ_HTTP_CLIENT_GET         = 0x51,
  RSI_WLAN_REQ_HTTP_CLIENT_POST        = 0x52,
  RSI_WLAN_REQ_HTTP_CLIENT_POST_DATA   = 0xEB,
  RSI_WLAN_REQ_HTTP_CLIENT_PUT         = 0x53,
  RSI_WLAN_REQ_DNS_SERVER_ADD          = 0x55,
  RSI_WLAN_REQ_NWK_APP_PROTOCOL_CONFIG = 0x58,
  RSI_WLAN_REQ_WIRELESS_FWUP           = 0x59,
  RSI_WLAN_REQ_BG_SCAN                 = 0x6A,
  RSI_WLAN_REQ_HT_CAPABILITIES         = 0x6D,
  RSI_WLAN_REQ_REJOIN_PARAMS           = 0x6F,
  RSI_WLAN_REQ_WPS_METHOD              = 0x72,
  RSI_WLAN_REQ_ROAM_PARAMS             = 0x7B,
  RSI_WLAN_REQ_TX_TEST_MODE            = 0x7C,
  RSI_WLAN_REQ_WMM_PS                  = 0x97,
  RSI_WLAN_REQ_FWUP                    = 0x99,
  RSI_WLAN_REQ_RX_STATS                = 0xA2,
  RSI_WLAN_REQ_SOCKET_CONFIG           = 0xA7,
  RSI_WLAN_REQ_MULTICAST               = 0xB1,
  RSI_WLAN_REQ_HTTP_ABORT              = 0xB3,
  RSI_WLAN_REQ_HTTP_CREDENTIALS        = 0xB4,
#ifdef RSI_WAC_MFI_ENABLE
  RSI_WLAN_REQ_ADD_MFI_IE = 0xB5,
#endif
#ifndef RSI_M4_INTERFACE
  RSI_WLAN_REQ_CERT_VALID = 0xBC,
#endif
  RSI_WLAN_REQ_SET_REGION_AP        = 0xBD,
  RSI_WLAN_REQ_FREQ_OFFSET          = 0xF3,
  RSI_WLAN_REQ_CALIB_WRITE          = 0xCA,
  RSI_WLAN_REQ_CALIB_READ           = 0xCF,
  RSI_WLAN_REQ_DYNAMIC_POOL         = 0xC7,
  RSI_WLAN_REQ_FILTER_BCAST_PACKETS = 0xC9,
  RSI_WLAN_REQ_EMB_MQTT_CLIENT      = 0xCB,
  RSI_WLAN_REQ_MDNSD                = 0xDB,
  RSI_WLAN_REQ_FTP                  = 0xE2,
  RSI_WLAN_REQ_FTP_FILE_WRITE       = 0xE3,
  RSI_WLAN_REQ_SNTP_CLIENT          = 0xE4,
  RSI_WLAN_REQ_SMTP_CLIENT          = 0xE6,
  RSI_WLAN_REQ_OTA_FWUP             = 0xEF,
  RSI_WLAN_REQ_WEBPAGE_LOAD         = 0x50,
  RSI_WLAN_REQ_JSON_LOAD            = 0x9c,
  RSI_WLAN_REQ_WEBPAGE_ERASE        = 0x9A,
  RSI_WLAN_REQ_JSON_OBJECT_ERASE    = 0x9B,
  RSI_WLAN_REQ_WEBPAGE_CLEAR_ALL    = 0x7F,
  RSI_WLAN_REQ_HOST_WEBPAGE_SEND    = 0x56,
  RSI_WLAN_REQ_GET_RANDOM           = 0xF8,
  RSI_WLAN_REQ_POP3_CLIENT          = 0xE7,
  RSI_WLAN_REQ_DHCP_USER_CLASS      = 0xEC,
  RSI_WLAN_REQ_SELECT_REQUEST       = 0x74,
  RSI_WLAN_REQ_TIMEOUT              = 0xEA,
  RSI_WLAN_REQ_RADIO                = 0x81,
  RSI_WLAN_REQ_GET_STATS            = 0xF1,
  RSI_WLAN_REQ_HTTP_OTAF            = 0xF4,
  RSI_WLAN_REQ_UPDATE_TCP_WINDOW    = 0xF5,
  RSI_WLAN_REQ_11AX_PARAMS          = 0xFF,
  RSI_WLAN_REQ_GET_CSI_DATA         = 0xC4,
  RSI_WLAN_REQ_GET_DPD_DATA         = 0xDC,
  RSI_WLAN_REQ_EVM_OFFSET           = 0x36,
  RSI_WLAN_REQ_EVM_WRITE            = 0x37,
  RSI_WLAN_REQ_EXT_STATS            = 0x68,
  RSI_WLAN_REQ_SET_NON_PREF_CHAN    = 0x5F
} rsi_wlan_cmd_request_t;

typedef enum rsi_wlan_opermode_e {
  RSI_WLAN_CLIENT_MODE            = 0,
  RSI_WLAN_WIFI_DIRECT_MODE       = 1,
  RSI_WLAN_ENTERPRISE_CLIENT_MODE = 2,
  RSI_WLAN_ACCESS_POINT_MODE      = 6,
  RSI_WLAN_TRANSMIT_TEST_MODE     = 8,
  RSI_WLAN_CONCURRENT_MODE        = 9

} rsi_wlan_opermode_t;

/******************************************************
 * *                 Type Definitions
 * ******************************************************/

// driver WLAN control block
typedef struct rsi_wlan_cb_s {
  // driver wlan control block state
  volatile rsi_wlan_state_t state;

  // Auto config state
  volatile uint8_t auto_config_state;

  // driver wlan control block status
  volatile int32_t status;

  // driver wlan control block mutex
  rsi_mutex_handle_t wlan_mutex;

  // driver wlan control block expected command response
  rsi_wlan_cmd_response_t expected_response;

  // driver wlan control block semaphore
  rsi_semaphore_handle_t wlan_sem;

  // driver wlan control block tx packet pool
  rsi_pkt_pool_t wlan_tx_pool;

  // socket id of socket which is waiting for socket create response
  uint8_t waiting_socket_id;

  // buffer pointer given by application to driver
  uint8_t *app_buffer;

  // buffer length given by application to driver
  uint32_t app_buffer_length;

  // driver wlan control block requested query command
  uint8_t query_cmd;

  // validity
  uint16_t field_valid_bit_map;

  // mac address
  uint8_t mac_address[6];

  // opermode
  uint16_t opermode;

  // wlan station state
  volatile rsi_wlan_state_t sta_state;

  // wlan ap state
  volatile rsi_wlan_state_t ap_state;
} rsi_wlan_cb_t;

// Band command request structure
typedef struct rsi_req_band_s {
  // uint8_t, band value to set
  uint8_t band_value;
} rsi_req_band_t;

#ifdef CHIP_917
typedef struct rsi_set_non_pref_chan_info_s {
  uint8_t channel_num;

  struct {
    /*
   * mbo non-preferred channel reason codes:
   * unspecified      = 0
   * rssi             = 1
   * ext_interference = 2
   * int_interference = 3
   * */
    uint8_t reason;
    uint8_t operating_class;
    uint8_t channel;
    uint8_t preference;
  } __attribute__((__packed__)) mbo_non_pref_chan[RSI_MAX_POSSIBLE_SINGLE_BAND_CHANNEL];

} __attribute__((__packed__)) rsi_set_non_pref_chan_info_t;
#endif

// Config command request structure
typedef struct rsi_req_config_s {
  // config type
  uint16_t config_type;
  // value to set
  uint16_t value;
} rsi_req_config_t;

// Set Mac address command request structure
typedef struct rsi_req_mac_address_s {

  // byte array, mac address
  uint8_t mac_address[6];
} rsi_req_mac_address_t;

// Rejoin params command request structure
typedef struct rsi_req_rejoin_params_s {
  // maximum number of retries before indicating join failure
  uint8_t max_retry[4];

  // scan interval between each retry
  uint8_t scan_interval[4];

  // beacon miss count to start rejoin
  uint8_t beacon_missed_count[4];

  // retry enable or disable for first time joining
  uint8_t first_time_retry_enable[4];

} rsi_req_rejoin_params_t;

// Setregion command request structure
typedef struct rsi_req_set_region_s {
  // Enable or disable set region from user: 1-take from user configuration,0-Take from Beacons
  uint8_t set_region_code_from_user_cmd;

  // region code(1-US,2-EU,3-JP.4-World Domain)*/
  uint8_t region_code;

  // module type (0- Without on board antenna, 1- With on board antenna)
  uint8_t module_type[2];

} rsi_req_set_region_t;

// Set region in AP mode command request structure
typedef struct rsi_req_set_region_ap_s {
  // Enable or disable set region from user: 1-take from user configuration, 0-Take US or EU or JP
  uint8_t set_region_code_from_user_cmd;

  // region code(1-US,2-EU,3-JP)
  uint8_t country_code[RSI_COUNTRY_CODE_LENGTH];

  uint8_t no_of_rules[4];

  struct {
    uint8_t first_channel;
    uint8_t no_of_channels;
    uint8_t max_tx_power;
  } channel_info[RSI_MAX_POSSIBLE_CHANNEL];

} rsi_req_set_region_ap_t;

// Scan command request structure
typedef struct rsi_req_scan_s {
  // RF channel to scan, 0=All, 1-14 for 2.5GHz channels 1-14
  uint8_t channel[4];

  // uint8[34], ssid to scan
  uint8_t ssid[RSI_SSID_LEN];

  // uint8[6], reserved fields
  uint8_t reserved[5];

  // uint8 , scan_feature_bitmap
  uint8_t scan_feature_bitmap;

  // uint8[2], channel bit map for 2.4 Ghz
  uint8_t channel_bit_map_2_4[2];

  // uint8[4], channel bit map for 5 Ghz
  uint8_t channel_bit_map_5[4];

} rsi_req_scan_t;

// PSK command request  structure
typedef struct rsi_req_psk_s {
  // psk type , 1-psk alone, 2-pmk, 3-generate pmk from psk
  uint8_t type;

  // psk or pmk
  uint8_t psk_or_pmk[RSI_PSK_LEN];

  // access point ssid: used for generation pmk
  uint8_t ap_ssid[RSI_SSID_LEN];

} rsi_req_psk_t;

typedef struct rsi_req_wps_method_s {
  // wps method: 0 - push button, 1 - pin method
  uint8_t wps_method[2];

  // If 0 - validate given pin, 1 - generate new pin
  uint8_t generate_pin[2];

  // wps pin for validation
  uint8_t wps_pin[RSI_WPS_PIN_LEN];

} rsi_req_wps_method_t;

// Access point configuration parameters
typedef struct rsi_req_ap_config_s {

  // channel number of the access point
  uint8_t channel[2];

  // ssid of the AP to be created
  uint8_t ssid[RSI_SSID_LEN];

  // security type of the Access point
  uint8_t security_type;

  // encryption mode
  uint8_t encryption_mode;

  // password in case of security mode
  uint8_t psk[RSI_PSK_LEN];

  // beacon interval of the access point
  uint8_t beacon_interval[2];

  // DTIM period of the access point
  uint8_t dtim_period[2];

  // beacon interval of the access point
  uint8_t ap_keepalive_type;

  // This is the bitmap to enable AP keep alive functionality and to
  // select the keep alive type.
  uint8_t ap_keepalive_period;

  // Number of clients supported
  uint8_t max_sta_support[2];

} rsi_req_ap_config_t;

// High throughputs enable command
typedef struct rsi_req_ap_ht_caps_s {
  // Variable to enable/disable publish high throughputs
  // capablities in the beacon in Acess point mode
  uint8_t mode_11n_enable[2];

  // HT capabilities bitmap which is published
  // in the beacon
  uint8_t ht_caps_bitmap[2];

} rsi_req_ap_ht_caps_t;

typedef struct rsi_req_configure_p2p_s {

  // GO Intent Value 0-15 for P2p GO or client , 16 - Soft AP
  uint8_t go_intent[2];

  // name of the device
  uint8_t device_name[64];

  // In which channel we are operating after becomes Group owner
  uint8_t operating_channel[2];

  // Postfix SSID
  uint8_t ssid_post_fix[64];

  // PSK of the device
  uint8_t psk[64];

} rsi_req_configure_p2p_t;

// join command request  structure
typedef struct rsi_req_join_s {
  // reserved bytes:Can be used for security Type
  uint8_t reserved1;

  // 0- Open, 1-WPA, 2-WPA2,6-MIXED_MODE
  uint8_t security_type;

  // data rate, 0=auto, 1=1Mbps, 2=2Mbps, 3=5.5Mbps, 4=11Mbps, 12=54Mbps
  uint8_t data_rate;

  // transmit power level, 0=low (6-9dBm), 1=medium (10-14dBm, 2=high (15-17dBm)
  uint8_t power_level;

  // pre-shared key, 63-byte string , last charecter is NULL
  uint8_t psk[RSI_PSK_LEN];

  // ssid of access point to join to, 34-byte string
  uint8_t ssid[RSI_SSID_LEN];

  // feature bitmap for join
  uint8_t join_feature_bitmap;

  // reserved bytes
  uint8_t reserved2[2];

  // length of ssid given
  uint8_t ssid_len;

  // listen interval
  uint8_t listen_interval[4];

  // vap id, 0 - station mode, 1 - AP1 mode
  uint8_t vap_id;

  // join bssid for mac based join
  uint8_t join_bssid[6];

} rsi_req_join_t;

//! beacon stop command request structure
typedef struct rsi_req_beacon_stop_s {
  //! beacon stop value
  uint8_t beacon_stop;
} rsi_req_beacon_stop_t;

// structure for ping request command
typedef struct rsi_req_ping_s {
  // ip version
  uint8_t ip_version[2];

  // ping size
  uint8_t ping_size[2];

  union {
    // ipv4 address
    uint8_t ipv4_address[4];

    // ipv6 address
    uint8_t ipv6_address[16];

  } ping_address;

  uint16_t timeout;
} rsi_req_ping_t;

// Ping Response Frame
typedef struct rsi_rsp_ping_s {
  // ip version
  uint8_t ip_version[2];

  // ping size
  uint8_t ping_size[2];
  union {
    // ipv4 address
    uint8_t ipv4_addr[4];

    // ipv6 address
    uint8_t ipv6_addr[16];
  } ping_address;

} rsi_rsp_ping_t;

// bg scan command request  structure
typedef struct rsi_req_bg_scan_s {
  // enable or disable BG scan
  uint8_t bgscan_enable[2];

  // Is it instant bgscan or normal bgscan
  uint8_t enable_instant_bgscan[2];

  // bg scan threshold value
  uint8_t bgscan_threshold[2];

  // tolerance threshold
  uint8_t rssi_tolerance_threshold[2];

  // periodicity
  uint8_t bgscan_periodicity[2];

  // active scan duration
  uint8_t active_scan_duration[2];

  // passive scan duration
  uint8_t passive_scan_duration[2];

  // multi probe
  uint8_t multi_probe;

} rsi_req_bg_scan_t;

typedef struct rsi_req_roam_params_s {
  // Enable or disable roaming
  uint8_t roam_enable[4];

  // roaming threshold
  uint8_t roam_threshold[4];

  // roaming hysteresis
  uint8_t roam_hysteresis[4];

} rsi_req_roam_params_t;

// IPV4 ipconfig command request  structure
typedef struct rsi_req_ipv4_parmas_s {
  // 0=Manual, 1=Use DHCP
  uint8_t dhcp_mode;

  // IP address of this module if in manual mode
  uint8_t ipaddress[4];

  // Netmask used if in manual mode
  uint8_t netmask[4];

  // IP address of default gateway if in manual mode
  uint8_t gateway[4];

  // DHCP client host name
  uint8_t hostname[31];

  // vap id, 0 - station and 1 - AP
  uint8_t vap_id;

} rsi_req_ipv4_parmas_t;

// IPV6 ipconfig command request  structure
typedef struct rsi_req_ipv6_parmas_s {

  // 0=Manual, 1=Use DHCP
  uint8_t mode[2];

  // prefix length
  uint8_t prefixLength[2];

  // IPV6 address of the module
  uint8_t ipaddr6[16];

  // address of gateway
  uint8_t gateway6[16];

  // vap id, 0 - station and 1 - AP
  uint8_t vap_id;

} rsi_req_ipv6_parmas_t;

// disassociate command request structure
typedef struct rsi_req_disassoc_s {

  // 0- Module in Client mode, 1- AP mode
  uint8_t mode_flag[2];

  // client MAC address, Ignored/Reserved in case of client mode
  uint8_t client_mac_address[6];

} rsi_req_disassoc_t;

// Enterprise configuration command request structure
typedef struct rsi_req_eap_config_s {
  // EAP method
  uint8_t eap_method[32];

  // Inner method
  uint8_t inner_method[32];

  // Username
  uint8_t user_identity[64];

  // Password
  uint8_t password[128];

  // Opportunistic key caching enable
  int8_t okc_enable[4];

  // Private key password for encrypted private keys
  uint8_t private_key_password[82];

} rsi_req_eap_config_t;

// Set certificate information structure
typedef struct rsi_cert_info_s {
  // total length of the certificate
  uint8_t total_len[2];

  // type of certificate
  uint8_t certificate_type;

  // more chunks flag
  uint8_t more_chunks;

  //length of the current segment
  uint8_t certificate_length[2];

  // index of certificate
  uint8_t certificate_inx;

  // reserved
  uint8_t key_password[127];

} rsi_cert_info_t;
#ifndef RSI_M4_INTERFACE
// Certificate Received structure
typedef struct rsi_cert_recv_s {
  // Ip version
  uint8_t ip_version[2];

  // Socket descriptor
  uint8_t sock_desc[2];

  // local port
  uint8_t src_port[2];

  // Destination port
  uint8_t dst_port[2];

  union {
    // destination ipv4 address
    uint8_t ipv4_address[4];

    // destination ipv6 address
    uint8_t ipv6_address[16];
  } ip_address;

  // sequence number;
  uint8_t sequence_no[2];

  // total length of the certificate
  uint8_t total_len[2];

  //length of the current segment
  uint8_t current_len[2];

  // more chunks flag
  uint8_t more_chunks;

  uint8_t reserved[5];

} rsi_cert_recv_t;

typedef struct rsi_req_cert_valid_s {
  // Socket ID
  uint16_t socket_id;

  // certificate valid status
  volatile uint16_t status;

} rsi_req_cert_valid_t;
#endif
#define RSI_CERT_MAX_DATA_SIZE (RSI_MAX_CERT_SEND_SIZE - (sizeof(struct rsi_cert_info_s)))
// Set certificate command request structure
typedef struct rsi_req_set_certificate_s {
  // certificate information structure
  struct rsi_cert_info_s cert_info;

  // certificate
  uint8_t certificate[RSI_MAX_CERT_SEND_SIZE];

} rsi_req_set_certificate_t;

// tx test mode command request structure
typedef struct rsi_req_tx_test_info_s {

  // uint8_t, enable/disable tx test mode
  uint8_t enable[2];

  // uint8_t, tx test mode power
  uint8_t power[2];

  // uint8_t, tx test mode rate
  uint8_t rate[4];

  // uint8_t, tx test mode length
  uint8_t length[2];

  // uint8_t, tx test mode mode
  uint8_t mode[2];

  // uint8_t, tx test mode channel
  uint8_t channel[2];

  // uint8_t, tx test mode rate_flags
  uint8_t rate_flags[2];

  // uint8_t, tx test mode tx test_ch_bw
  uint8_t channel_bw[2];

  // uint8_t, tx test mode aggr_enable
  uint8_t aggr_enable[2];

  // uint8_t, tx test mode reserved
  uint8_t reserved[2];

  // uint8_t, tx test mode no_of_pkts
  uint8_t no_of_pkts[2];

  // uint8_t, tx test mode delay
  uint8_t delay[4];
#ifdef CHIP_917
  uint8_t enable_11ax;
  uint8_t coding_type;
  uint8_t nominal_pe;
  uint8_t UL_DL;
  uint8_t he_ppdu_type;
  uint8_t beam_change;
  uint8_t BW;
  uint8_t STBC;
  uint8_t Tx_BF;
  uint8_t GI_LTF;
  uint8_t DCM;
  uint8_t NSTS_MIDAMBLE;
  uint8_t spatial_reuse;
  uint8_t BSS_color;
  uint8_t HE_SIGA2_RESERVED[2];
  uint8_t RU_ALLOCATION;
  uint8_t N_HELTF_TOT;
  uint8_t SIGB_DCM;
  uint8_t SIGB_MCS;
  uint8_t USER_STA_ID[2];
  uint8_t USER_IDX;
  uint8_t SIGB_COMPRESSION_FIELD;
#endif
} rsi_req_tx_test_info_t;

// per stats command request structure
typedef struct rsi_req_rx_stats_s {
  // 0 - start , 1 -stop
  uint8_t start[2];

  // channel number
  uint8_t channel[2];

} rsi_req_rx_stats_t;

// SNMP command structure
typedef struct rsi_snmp_response_s {
  uint8_t type;
  uint8_t value[MAX_SNMP_VALUE];
} rsi_snmp_response_t;

// snmp object data structure format
typedef struct rsi_snmp_object_data_s {

  // Type of SNMP data contained
  uint8_t snmp_object_data_type[4];

  // Most significant 32 bits
  uint8_t snmp_object_data_msw[4];

  // Least significant 32 bits
  uint8_t snmp_object_data_lsw[4];

  // snmp ip version
  uint8_t snmp_ip_version[4];
  union {

    // ipv4 address
    uint8_t ipv4_address[4];

    // ipv6 address
    uint8_t ipv6_address[16];
  } snmp_ip_address;

} rsi_snmp_object_data_t;

// snmp trap object
typedef struct rsi_snmp_trap_object_s {
  // SNMP object string
  uint8_t snmp_object_string_ptr[40];

  // SNMP object data
  rsi_snmp_object_data_t snmp_object_data;

} rsi_snmp_trap_object_t;

// SNMP trap structure
typedef struct rsi_snmp_trap_s {
  // snmp version
  uint8_t snmp_version;

  // snmp ip version
  uint8_t ip_version[4];
  union {

    // ipv4 address
    uint8_t ipv4_address[4];

    // ipv6 address
    uint8_t ipv6_address[16];
  } destIPaddr;

  // communit : Private or public
  uint8_t community[32];

  // trap type
  uint8_t trap_type;

  // elapsed time
  uint8_t elapsed_time[4];

  // trap oid
  uint8_t trap_oid[51];

  // object list count
  uint8_t obj_list_count;

  // snmp buffer
  uint8_t snmp_buf[RSI_SNMP_TRAP_BUFFER_LENGTH];

} rsi_snmp_trap_t;

/* Structure for SNMP Enable */
typedef struct rsi_snmp_enable_s {
  // snmp enable
  uint8_t snmp_enable;

} rsi_snmp_enable_t;

// Network params command response structure
typedef struct rsi_rsp_nw_params_s {
  // uint8, 0=NOT Connected, 1=Connected
  uint8_t wlan_state;

  // channel number of connected AP
  uint8_t channel_number;

  // PSK
  uint8_t psk[64];

  // Mac address
  uint8_t mac_address[6];

  // uint8[32], SSID of connected access point
  uint8_t ssid[RSI_SSID_LEN];

  // 2 bytes, 0=AdHoc, 1=Infrastructure
  uint8_t connType[2];

  // security type
  uint8_t sec_type;

  // uint8, 0=Manual IP Configuration,1=DHCP
  uint8_t dhcpMode;

  // uint8[4], Module IP Address
  uint8_t ipv4_address[4];

  // uint8[4], Module Subnet Mask
  uint8_t subnetMask[4];

  // uint8[4], Gateway address for the Module
  uint8_t gateway[4];

  // number of sockets opened
  uint8_t num_open_socks[2];

  // prefix length for ipv6 address
  uint8_t prefix_length[2];

  // modules ipv6 address
  uint8_t ipv6_address[16];

  // router ipv6 address
  uint8_t defaultgw6[16];

  // BIT(0) =1 - ipv4, BIT(1)=2 - ipv6, BIT(0) & BIT(1)=3 - BOTH
  uint8_t tcp_stack_used;

  //sockets information array
  rsi_sock_info_query_t socket_info[RSI_MN_NUM_SOCKETS];

  //bssid of connected AP
  uint8_t bssid[6];

} rsi_rsp_nw_params_t;

// go paramas response structure
typedef struct rsi_rsp_go_params_s {
  // SSID of the P2p GO
  uint8_t ssid[RSI_SSID_LEN];

  // BSSID of the P2p GO
  uint8_t mac_address[6];

  // Operating channel of the GO
  uint8_t channel_number[2];

  // PSK of the GO
  uint8_t psk[64];

  // IPv4 Address of the GO
  uint8_t ipv4_address[4];

  // IPv6 Address of the GO
  uint8_t ipv6_address[16];

  // Number of stations Connected to GO
  uint8_t sta_count[2];

  rsi_go_sta_info_t sta_info[RSI_MAX_STATIONS];

} rsi_rsp_go_params_t;

// socket create command request structure
typedef struct rsi_req_socket_s {
  // ip version4 or 6
  uint8_t ip_version[2];

  // 0=TCP Client, 1=UDP Client, 2=TCP Server (Listening TCP)
  uint8_t socket_type[2];

  // Our local module port number
  uint8_t module_socket[2];

  // Port number of what we are connecting to
  uint8_t dest_socket[2];

  union {

    //  remote IPv4 Address
    uint8_t ipv4_address[4];

    //  remote IPv6 Address
    uint8_t ipv6_address[16];
  } __attribute__((__packed__)) dest_ip_addr;

  // maximum no of LTCP sockets on same port
  uint8_t max_count[2];

  // type of service
#ifdef CHIP_917
  uint8_t tos[2];
#else
  uint8_t tos[4];
#endif

  // ssl version select bit map
#ifdef CHIP_917
  uint32_t ssl_bitmap;
#else
  uint8_t ssl_bitmap;

  // ssl ciphers bitmap
  uint8_t ssl_ciphers;
#endif

  // web socket resource name
  uint8_t webs_resource_name[RSI_WEBS_MAX_URL_LENGTH];

  // web socket host name
  uint8_t webs_host_name[RSI_WEBS_MAX_HOST_LENGTH];

  // TCP retries
  uint8_t max_tcp_retries_count;

  // Socket bitmap
  uint8_t socket_bitmap;

  // RX window size
  uint8_t rx_window_size;

  // TCP keepalive initial timeout
  uint8_t tcp_keepalive_initial_time[2];

  // VAPID
  uint8_t vap_id;

  //socket cert inx
  uint8_t socket_cert_inx;

  //ssl ciphers bitmap
  uint32_t ssl_ciphers_bitmap;

  //ssl extended ciphers bitmap
#ifdef CHIP_917
  uint32_t ssl_ext_ciphers_bitmap;

  //! max retransmission timeout value
  uint8_t max_retransmission_timeout_value;
#endif
  // tcp retry transmission timer
  uint8_t tcp_retry_transmit_timer;

  // TCP MSS
  uint16_t tcp_mss;

  uint16_t no_of_tls_extensions;
  uint16_t total_extension_length;
  uint8_t tls_extension_data[MAX_SIZE_OF_EXTENSION_DATA];
#ifdef CHIP_917
  uint16_t recv_buff_len;
#endif

} __attribute__((__packed__)) rsi_req_socket_t;

// Socket close command request structure
typedef struct rsi_req_socket_close_s {

  // 2 bytes, socket that was closed
  uint8_t socket_id[2];

  // 4 bytes, port number
  uint8_t port_number[2];

} rsi_req_socket_close_t;

// socket close command response structure
typedef struct rsi_rsp_socket_close_s {
  // 2 bytes, socket that was closed
  uint8_t socket_id[2];

  // 4 bytes, sent bytes count
  uint8_t sent_bytes_count[4];

  // 4 bytes, port number
  uint8_t port_number[2];

} rsi_rsp_socket_close_t;

// LTCP socket establish request structure
typedef struct rsi_rsp_ltcp_est_s {
  uint8_t ip_version[2];

  // 2 bytes, socket handle
  uint8_t socket_id[2];

  // 2 bytes, remote port number
  uint8_t dest_port[2];

  union {

    //  remote IPv4 Address
    uint8_t ipv4_address[4];

    //  remote IPv6 Address
    uint8_t ipv6_address[16];
  } dest_ip_addr;

  // 2 bytes, remote peer MSS size
  uint8_t mss[2];

  // 4 bytes, remote peer Window size
  uint8_t window_size[4];

  // source port number
  uint8_t src_port_num[2];

} rsi_rsp_ltcp_est_t;

// socket accept request structure
typedef struct rsi_req_socket_accept_s {
  // Socket ID
  uint8_t socket_id;

  // Local port number
  uint8_t source_port[2];

} rsi_req_socket_accept_t;
// socket accept request structure
typedef struct rsi_req_socket_select_s {
  // Socket ID
  uint8_t num_fd;
  uint8_t select_id;

  rsi_fd_set rsi_read_fds;

  rsi_fd_set rsi_write_fds;

  struct rsi_timeval rsi_select_timeval;

  uint8_t no_timeout;
} rsi_req_socket_select_t;

// read bytes coming on socket request structure
typedef struct rsi_req_socket_read_s {
  // socket id
  uint8_t socket_id;

  // requested bytes
  uint8_t requested_bytes[4];

  // Timeout for read
  uint8_t read_timeout[2];

} rsi_req_socket_read_t;

// send data on socket request structure
typedef struct rsi_req_socket_send_s {

  // ip version 4 or 6
  uint8_t ip_version[2];

  // socket descriptor of the already opened socket connection
  uint8_t socket_id[2];

  // length of the data to be sent
  uint8_t length[4];

  // Data Offset, TCP=44, UDP=32
  uint8_t data_offset[2];

  // destination port
  uint8_t dest_port[2];

  union {
    // 4 bytes, IPv4 Address of the remote device
    uint8_t ipv4_address[RSI_IP_ADDRESS_LEN];

    // 4 bytes, IPv6 Address of the remote device
    uint8_t ipv6_address[RSI_IP_ADDRESS_LEN * 4];

  } dest_ip_addr;

  // data buffer to send
  uint8_t send_buffer[RSI_MAX_PAYLOAD_SIZE];

} rsi_req_socket_send_t;

typedef struct rsi_rsp_socket_recv_s {
  // 2 bytes, the ip version of the ip address , 4 or 6
  uint8_t ip_version[2];

  // 2 bytes, the socket number associated with this read event
  uint8_t socket_id[2];

  // 4 bytes, length of data received
  uint8_t length[4];

  // 2 bytes, offset of data from start of buffer
  uint8_t offset[2];

  // 2 bytes, port number of the device sending the data to us
  uint8_t dest_port[2];

  union {

    // 4 bytes, IPv4 Address of the device sending the data to us
    uint8_t ipv4_address[4];

    // 4 bytes, IPv6 Address of the device sending the data to us
    uint8_t ipv6_address[16];
  } dest_ip_addr;
#ifdef RSI_PROCESS_MAX_RX_DATA
  uint8_t end_of_the_data;
  uint8_t more_data_pending;
#endif
} rsi_rsp_socket_recv_t;

// socket create command response structure
typedef struct rsi_rsp_socket_create_s {
  // ip version 4 or 6
  uint8_t ip_version[2];

  // 2 bytes, type of socket created
  uint8_t socket_type[2];

  // 2 bytes socket descriptor, like a file handle, usually 0x00
  uint8_t socket_id[2];

  // 2 bytes, Port number of our local socket
  uint8_t module_port[2];
  uint8_t dst_port[2];
  union {

    // 4 bytes, Our (module) IPv4 Address
    uint8_t ipv4_addr[4];

    // 4 bytes, Our (module) IPv6 Address
    uint8_t ipv6_addr[16];

  } module_ip_addr;
  union {
    // 4 bytes, Our (module) IPv4 Address
    uint8_t ipv4_addr[4];
    // 4 bytes, Our (module) IPv6 Address
    uint8_t ipv6_addr[16];
  } dest_ip_addr;

  // 2 bytes, Remote peer MSS size
  uint8_t mss[2];

  // 4 bytes, Remote peer Window size
  uint8_t window_size[4];

} rsi_rsp_socket_create_t;

typedef struct rsi_rsp_socket_select_s {
  // select id
  uint8_t select_id;

  // readfd struct to store select info
  rsi_fd_set rsi_read_fds;

  // readfd struct to store select info
  rsi_fd_set rsi_write_fds;

  // readfd struct to store select info
  uint32_t socket_terminate_bitmap;
} rsi_rsp_socket_select_t;

//structure for power save request
typedef struct rsi_power_save_req_s {
  // power mode to set
  uint8_t power_mode;

  // set LP/ULP/ULP-without RAM retention
  uint8_t ulp_mode_enable;

  // set DTIM aligment required
  // 0 - module wakes up at beacon which is just before or equal to listen_interval
  // 1 - module wakes up at DTIM beacon which is just before or equal to listen_interval
  uint8_t dtim_aligned_type;

  //Set PSP type, 0-Max PSP, 1- FAST PSP, 2-APSD
  uint8_t psp_type;

  // Monitor interval for the FAST PSP mode
  // default is 50 ms, and this parameter is valid for FAST PSP only
  uint16_t monitor_interval;
  uint8_t num_of_dtim_skip;
  uint16_t listen_interval;

} rsi_power_save_req_t;

//structure for wmm params
typedef struct rsi_wmm_ps_parms_s {
  // wmm ps enable or disable
  uint8_t wmm_ps_enable[2];

  // wmm ps type
  uint8_t wmm_ps_type[2];

  // wmm ps wakeup interval
  uint8_t wmm_ps_wakeup_interval[4];

  // wmm ps UAPSD bitmap
  uint8_t wmm_ps_uapsd_bitmap;
} rsi_wmm_ps_parms_t;

// Structure for TCP ACK indication
typedef struct rsi_rsp_tcp_ack_s {
  // Socket ID
  uint8_t socket_id;

  // Length
  uint8_t length[2];

} rsi_rsp_tcp_ack_t;

// Structure for firmware upgradation
typedef struct rsi_req_fwup_s {
  // Type of the packet
  uint8_t type[2];

  // Length of the packet
  uint8_t length[2];

  // RPS content
  uint8_t content[RSI_MAX_FWUP_CHUNK_SIZE];

} rsi_req_fwup_t;

// Structure for OTA firmware upgradation
typedef struct rsi_req_ota_fwup_s {
  // Type of the packet
  uint8_t ip_version;

  union {
    // 4 bytes, IPv4 Address of the server
    uint8_t ipv4_address[RSI_IP_ADDRESS_LEN];

    // 4 bytes, IPv6 Address of the server
    uint8_t ipv6_address[RSI_IP_ADDRESS_LEN * 4];

  } server_ip_address;

  // server port
  uint8_t server_port[4];

  // Chunk number
  uint8_t chunk_number[2];

  // Timeout
  uint8_t timeout[2];

  // TCP retry count
  uint8_t retry_count[2];

} rsi_req_ota_fwup_t;

//Structure for Timeout
typedef struct rsi_req_timeout_s {
  // Timeout bitmap
  uint8_t timeout_bitmap[4];
  // Timeout value
  uint8_t timeout_value[2];
} rsi_req_timeout_t;

//Structure for WLAN filter broadcast packets
typedef struct rsi_req_wlan_filter_bcast_s {
  uint8_t beacon_drop_threshold[2];

  uint8_t filter_bcast_in_tim;

  uint8_t filter_bcast_tim_till_next_cmd;

} rsi_req_wlan_filter_bcast_t;

// buffer allocation command parameters
typedef struct {
  // tx ratio
  uint8_t dynamic_tx_pool;
  // rx ratio
  uint8_t dynamic_rx_pool;
  // global ratio
  uint8_t dynamic_global_pool;
} rsi_udynamic;

#ifdef RSI_WAC_MFI_ENABLE
// Add MFI IE in Beacon command request structure
typedef struct rsi_req_add_mfi_ie_s {
  // IE length
  uint8_t mfi_ie_len;

  // Actual IE
  uint8_t mfi_ie[200];

} rsi_req_add_mfi_ie_t;
#endif

/*===================================================*/
/**
 * HT CAPS Parameters set structure
 */
typedef union {
  struct {
    uint8_t mode_11n_enable[2];
    uint8_t ht_caps_bitmap[2];
  } htCapsFrameSnd;
  uint8_t uHtCapsBuf[4];
} rsi_uHtCaps;

/*===================================================*/
/**
 *Structure for rejoin_params
 */
typedef struct rsi_rejoin_params_s {
  //! Maximum number of retries before indicating join failure \n
  //! Note: A value of 1 indicates no retries, while 0 indicates infinite retries.
  uint8_t rsi_max_try[4];

  //! Scan interval in seconds between each retry
  uint8_t rsi_scan_interval[4];

  //! Number of missed beacons that will trigger rejoin
  uint8_t rsi_beacon_missed_count[4];

  //! Retry enable or disable for first time joining
  uint8_t rsi_first_time_retry_enable[4];
} rsi_rejoin_params_t;

/*===================================================*/
/**
 * Roaming Parameters set structure
 */
typedef union {
  struct {
    //! Enables or disables the roaming feature.
    //! 0 - Disable
    //! 1 - Enable
    uint8_t roam_enable[4];

    //! Specifies the RSSI threshold (in dBm) below which the module will attempt to roam to a better access point.
    uint8_t roam_threshold[4];

    //! Defines the hysteresis value to prevent frequent roaming when the RSSI fluctuates around the threshold.
    uint8_t roam_hysteresis[4];
  } roamParamsFrameSnd;
  uint8_t uRoamParamsBuf[12];
} rsi_uRoamParams;

/*==================================================*/
/*This will keep the wepkey params*/

typedef struct {
  uint8_t index[2];
  uint8_t key[4][32];
} rsi_wepkey;

/*====================================================*/
/*This will keep the AP configuration parameter*/

typedef struct {
  uint8_t channel_no[2];
  uint8_t ssid[RSI_SSID_LEN];
  uint8_t security_type;
  uint8_t encryp_mode;
  uint8_t psk[RSI_PSK_LEN];
  uint8_t beacon_interval[2];
  uint8_t dtim_period[2];
  uint8_t ap_keepalive_type;
  uint8_t ap_keepalive_period;
  uint8_t max_sta_support[2]; // it can be configured from 1 to 4
} rsi_apconfig;
// below defines and structure for CFG_GET: Getting user store configuration.
#define IP_ADDRESS_SZ            4
#define RSI_SSID_LEN             34
#define WISE_PMK_LEN             32
#define MAX_HTTP_SERVER_USERNAME 31
#define MAX_HTTP_SERVER_PASSWORD 31
#define RSI_PSK_LEN              64
// structure for cfg_get
typedef struct rsi_cfgGetFrameRcv {

  //! Configuration enable/disable for auto-join or auto-create modes \n
  //! 0x00 - Auto-join/create modes disabled (primary configuration) \n
  //! 0x01 - Auto-join/create modes enabled (primary configuration) \n
  //! 0x03 - Auto-join/create modes disabled (secondary configuration, not supported in SAPI) \n
  //! 0x04 - Auto-join/create modes enabled (secondary configuration, not supported in SAPI)
  uint8_t cfg_enable;

  //! WiFi operating mode configuration \n
  //! 0 - Client/Station mode \n
  //! 1 - Wi-Fi Direct/P2P mode (not supported) \n
  //! 2 - Enterprise Client mode with EAP \n
  //! 6 - Access Point mode \n
  //! 8 - Transmit test mode \n
  //! 9 - Concurrent mode
  uint8_t opermode[4];

  //! Bitmap of enabled WLAN features. Refer feature_bit_map
  uint8_t feature_bit_map[4];

  //! Bitmap of enabled TCP/IP related features. Refer tcp_ip_feature_bit_map
  uint8_t tcp_ip_feature_bit_map[4];

  //! Bitmap of enabled custom features. Refer custom custom_feature_bit_map
  uint8_t custom_feature_bit_map[4];

  //! Operating band
  //! 0x00 - device configured to operate in 2.4 GHz - 0x01 - device configured to operate in 5 GHz \n
  //! 0x02 - Dual band(2.4 GHz and 5GHz). Dual band is valid in station mode
  uint8_t band;

  //! Scan feature bitmap for scan behavior \n
  //! BIT[0]: Quick scan feature (1 - To enable quick scan feature, 0 - To disable quick scan feature). \n
  //! BIT[1-7]: Reserved
  uint8_t scan_feature_bitmap;

  //! SSID of the AP for auto-join mode (34-byte buffer, null-terminated string). \n
  //! If actual length < 32 bytes, 0x00 filler bytes appended to make the buffer length 34 bytes.
  uint8_t join_ssid[RSI_SSID_LEN];

  //! Data rate configured in the module. \n
  //! 0=Auto, 1-4=802.11b rates (1,2,5.5,11 Mbps), 5-12=802.11g rates (6,9,12,18,24,36,48,54 Mbps), \n
  //! 13-20=802.11n MCS rates (MCS0-MCS7). Values > 20 are rejected. \n
  //! If 802.11b disabled and rate < 5 provided, defaults to 6 Mbps.
  uint8_t uRate;

  //! Tx power to be configured in the device. \n
  //! At 2.4GHz: 0 – Low power (7 +/- 1) dBm  1 – Medium power (10 +/- 1)dBm  2 – High power (18 +/- 2)dBm \n
  //! At 5 GHz: 0 – Low power (5 +/- 1) dBm  1 – Medium power (7 +/- 1) dBm  2 – High power (12 +/- 2) dBm
  uint8_t uTxPower;

  //! Reserved 1
  uint8_t reserved_1;

  //! Reserved 2
  uint8_t reserved_2;

  //! Length of SSID in scan_ssid array (uint8_t, range 0-32). \n
  //! 0 = Broadcast scan (scan all APs), 1-32 = Directed scan (scan for specific SSID).
  uint8_t scan_ssid_len;

  //! Reserved 3
  uint8_t reserved_3;

  //! Security mode used to connect to an Access Point in auto-join mode \n
  //! 0 – Open mode \n
  //! 1 – WPA security \n
  //! 2 – WPA2 Security \n
  //! 3 – WEP security \n
  //! 4 – Enterprise WPA security  \n
  //! 5 – Enterprise WPA2 security  \n
  //! 6 – WPA/WPA2 mixed security \n
  //! 7 – WPA3 Personal Mode Security \n
  //! 8 – WPA3 Personal Transition Mode Security
  uint8_t csec_mode;

  //! Pre-shared key of the access point to which module wants to associate in auto-join \n
  //! Filler bytes of 0x00 are added to make it 64 bytes if the original PSK is less than 64 bytes
  uint8_t psk[RSI_PSK_LEN];

  //! SSID for directed scan operations (34 byte buffer). \n
  //! If the actual length is not 32 bytes, 0x00 filler bytes are appended to make the length 34 bytes. \n
  //! If scan_ssid_len = 0, broadcast scan is performed. \n
  //! If scan_ssid_len > 0, only APs matching this SSID are scanned. \n
  //! Stored as binary data (may not be null-terminated).
  uint8_t scan_ssid[RSI_SSID_LEN];

  //! Channel number to be scanned in auto join mode. Refer to the PER Mode command section for the supported channels in 2.4GHz and 5 GHz band
  uint8_t scan_cnum;

  //! Controls how the device gets its IPv4 address in auto‑join mode \n
  //! 0x00 - DHCP client is disabled in device (auto-join mode) \n
  //! 0x01 - DHCP client is enabled in device (auto-join mode)
  uint8_t dhcp_enable;

  //! Static IP configured in the device in auto-join or auto-create mode. For auto-join mode, this is valid when dhcp_enable is 0.
  uint8_t ip[IP_ADDRESS_SZ];

  //! Subnet mask,this is valid only if dhcp_enable is 0.
  uint8_t sn_mask[IP_ADDRESS_SZ];

  //! Default gateway, this is valid only if dhcp_enable is 0.
  uint8_t dgw[IP_ADDRESS_SZ];

  //! EAP method string for Enterprise security mode (32-byte null-terminated ASCII). \n
  //! Valid values: "TLS", "TTLS", "PEAP", "FAST", "LEAP".
  uint8_t eap_method[32];

  //! This parameter gives the EAP inner method configured for the device/EAP Client
  uint8_t inner_method[32];

  //! User ID in enterprise security mode.
  uint8_t user_identity[64];

  //! The password configured for the user identity on the RADIUS server for enterprise authentication. \n
  //! Filler bytes of 0x00 are used to make the length 128 bytes, of the original length is less than 128 bytes.
  uint8_t passwd[128];

  //! GO Intent Value 0-15 for P2P GO or client, 16 for Soft AP
  uint8_t go_intent[2];

  //! Name of the GO device, meant only for P2P. \n
  //! Note: P2P is not supported
  uint8_t device_name[64];

  //! The channel on which the device operates after becoming a Group Owner (GO) in P2P
  //! Note: P2P is not supported
  uint8_t operating_channel[2];

  //! Postfix data to add to the SSID in Wi-Fi Direct (P2P) mode \n
  //! This data will be added to the end of the SSID after the DIRECT-<random two octets> prefix. \n
  //! Note: P2P is not supported
  uint8_t ssid_postfix[64];

  //! Psk of the device in P2P \n
  //! Note: P2P is not supported
  uint8_t psk_key[64];

  //! PMK key
  uint8_t pmk[WISE_PMK_LEN];

  //! AP Configuration information
  rsi_apconfig apconfig;

  //! WiFi MAC address of the device (6 bytes).
  uint8_t module_mac[6];

  //! Selected antenna \n
  //! 0 -  Built‑in / inbuilt antenna selected \n
  //! 1 - UFL connector (external antenna) selected
  uint8_t antenna_select[2];

  //! Reserved 4
  uint8_t reserved_4[2];

  //! WEP key in the module to connect to an AP with WEP security. \n
  //! There are two modes in which a WEP key can be set in an Access Point- WEP (hex) mode and WEP (ASCII) mode. The module supports WEP (hex) mode only.
  rsi_wepkey wep_key;

  //! 0x00 – Indicates that the device uses static IPv6 address provided by the user \n
  //! 0x01 – Indicates that the device uses IPv6 address either derived using SLAAC method or DHCPv6 process based on the DHCPv6 server configuration.
  uint8_t dhcp6_enable[2];

  //! Prefix length of IPv6 address.
  uint8_t prefix_length[2];

  //! IPv6 address of device.
  uint8_t ip6[16];

  //! IPv6 address of default router (gateway).
  uint8_t dgw6[16];

  //! Shows which TCP stack is used, this is a bitmap.  \n
  //! BIT(0)- If set indicates IPv4 Stack is in use \n
  //! BIT(1) – If set indicates IPv6 Stack is in use
  uint8_t tcp_stack_used;

  //! This magic code is used to validate the bgscan parameter present in flash memory. It is not configurable. \n
  //! (0x4321) indicates stored bg scan parameters are valid
  uint8_t bgscan_magic_code[2];

  //! Enable/Disable bgscan \n
  //! 0 – Disable, 1 - Enable
  uint8_t bgscan_enable[2];

  //! Threshold in dBm to trigger the bgscan. After bgscan periodicity, If connected AP RSSI falls below this then bgscan will be triggered.
  uint8_t bgscan_threshold[2];

  //! This is difference of last RSSI of connected AP and current RSSI of connected AP. \n
  //! Here last RSSI is the RSSI calculated at the last beacon received and current RSSI is the RSSI calculated at current beacon received. \n
  //! If this difference is more than rssi_tolerance_threshold then bgscan will be triggered irrespective of periodicity.
  uint8_t rssi_tolerance_threshold[2];

  //! Time period in seconds to trigger bgscan if RSSI of connected AP is greater than the given bgscan_threshold. Default value is 10 second.
  uint8_t bgscan_periodicity[2];

  //! This is active scan duration in ms. The recommended value is 20ms for quicker Background scan operation and uninterrupted throughput.
  uint8_t active_scan_duration[2];

  //! This is passive scan duration in ms. During a passive scan, if any beacon is received in a channel, then the recommended value for this parameter will be 100ms
  uint8_t passive_scan_duration[2];

  //! If set to one then module will send two probe request one with specific SSID provided during join command and other with NULL ssid (to scan all the access points)
  uint8_t multi_probe;

  //! Used to validate the given scan channel bitmaps. If magic code is 0x4321, then only the scan channel bitmaps are considered as valid
  uint8_t chan_bitmap_magic_code[2];

  //! Channel bitmap for scanning in set of selective channels in 2.4 GHz band.
  uint8_t scan_chan_bitmap_stored_2_4_GHz[4];

  //! Channel bitmap for scanning in set of selective channels in 5 GHz band.
  uint8_t scan_chan_bitmap_stored_5_GHz[4];

  //! This magic code is used to validate the roaming parameters stored in the flash memory. This is not configurable, 0x4321 indicating stored roam parameters are valid
  uint8_t roam_magic_code[2];

  //! Roaming parameters stored in the module.
  rsi_uRoamParams roam_params_stored;

  //! This magic code is used to validate the rejoin parameters stored in the flash memory. If magic code is 0x4321, then rejoin params considered as valid
  uint8_t rejoin_magic_code[2];

  //! rejoin_param_stored stores user configured rejoin parameters. \n
  //! This is updated from the user input and will be configured to the TA using RSI_WLAN_RSP_USER_STORE_CFG command. \n
  //! We can configure values like  rsi_max_try – max rejoin try count rsi_scan_interval, rsi_beacon_missed_count
  rsi_rejoin_params_t rejoin_param_stored;

  //! If this variable is 1, region of the device is set either in auto join or auto create mode based on the opermode. \n
  //! 0 – Disable set region in Auto create or Auto join mode \n
  //! 1 – Enable set region in Auto create or Auto join mode
  uint8_t region_request_from_host;

  //! Enable/Disable set region code from user. \n
  //! If opermode is 0 or 2: \n
  //! 0 - Disable. Use the region information from beacon (country IE) \n
  //! 1 - Enable. Use the region information from user command \n
  //! If opermode is 6: \n
  //! 0 - Disable. Get the region information based on region code from internal memory
  uint8_t rsi_region_code_from_host;

  //! If the region code is given as 0, US domain is considered by default and device is configured according to the US domain regulations. \n
  //! 1 - US domain \n
  //!  2 - Europe domain \n
  //! 3 - Japan Domain \n
  //! 4 -Rest of world \n
  //! 5 - Korea Domain
  uint8_t region_code;

  //! reserved_5
  uint8_t reserved_5[43];

  //! A validation flag, (0x4321) indicating stored multicast parameters are valid
  uint8_t multicast_magic_code[2];

  //! There are two bytes in the command which represent 2 parts. Lower order byte represents the command type and higher order byte is the hash value (6 Bits) generated from the desired multicast mac address (48 Bits) using hash function. Refer to mcast_bitmap_frame.
  uint8_t multicast_bitmap[2];

  //! A validation flag, (0x4321) indicating stored powermode parameters are valid
  uint8_t powermode_magic_code[2];

  //! powermode variable configures the powersave mode of the device. \n
  //! Below are the configurable powersave values on 9116. \n
  //! 2 - Connected Powersave. \n
  //! 8 - Unconnected power save with RAM Retention \n
  //! 10 - Unconnected power save mode without RAM Retention
  uint8_t powermode;

  //! 1 - Low power mode. \n
  //! 2 - Ultra low power mode with RAM retention. \n
  //! 3 - Ultra low power mode without RAM retention. \n
  //! Refer section Powersave operation for detail description about powersave operation.
  uint8_t ulp_mode;

  //! This magic code is used to validate the WMM powersave parameters stored in the flash memory.(0x4321) indicating stored parameters are valid
  //! Note: WMM PS is not supported
  uint8_t wmm_ps_magic_code[2];

  //! To enable or disable WMM PS\n
  //! 0 - Disable, 1 - Enable
  //! Note: WMM PS is not supported
  uint8_t wmm_ps_enable;

  //! WMM PS type \n
  //! 0 - Tx Based, 1 - Periodic
  //! Note: WMM PS is not supported
  uint8_t wmm_ps_type;

  //! Wakeup interval in milliseconds.
  //! Note: WMM PS is not supported
  uint8_t wmm_ps_wakeup_interval[4];

  //! 0 to 15 possible values. \n
  //! wmm_ps_uapsd_bitmap[0] Access category: Voice \n
  //! wmm_ps_uapsd_bitmap[1] Access category: Video \n
  //! wmm_ps_uapsd_bitmap[2] Access category: background \n
  //! wmm_ps_uapsd_bitmap[3] Access category: Best effort U-APSD \n
  //! wmm_ps_uapsd_bitmap[4:7] All set to 0 / Don't care
  //! Note: WMM PS is not supported
  uint8_t wmm_ps_uapsd_bitmap;

  //! This is valid only if BIT(1) in join_feature_bitmap is set. This value is given in Time units (1024 microseconds). \n
  //! This parameter gives the listen interval configured in power save mode
  uint8_t listen_interval[4];

  //! This parameter is valid only if BIT(1) is set in the join_feature_bitmap and valid listen interval is given in join command. \n
  //! If this parameter is set, the device computes the desired sleep duration based on listen interval (from join command) and its wakeup align with Beacon or DTIM Beacon (based on this parameter). \n
  //! 0 - device wakes up before nearest Beacon that does not exceed the specified listen interval time. \n
  //! 1 - device wakes up before nearest DTIM Beacon that does not exceed the specified listen interval time.
  uint8_t listen_interval_dtim;

  //! Gives the features enabled in Extended custom feature bitmap.
  uint8_t ext_custom_feature_bit_map[4];

  //! Password to decrypt an encrypted private key used in EAP authentication
  uint8_t private_key_password[82];

  //! BSSID for BSSID-based join feature (6 bytes) \n
  //! This is connected AP MAC address when BSSID-based join is enabled and used during auto-config restore to automatically reconnect.
  uint8_t join_bssid[6];

  //! BIT[0] - To enable b/g only mode in station mode \n
  //! BIT[1] - To take listen interval from join command \n
  //! BIT[2] - To enable quick join feature \n
  //! BIT[3] - To enable CCXV2 feature \n
  //! BIT[4] - To connect to AP based on BSSID \n
  //! BIT[5] - Management Frame Protection Capable only(802.11W) \n
  //! BIT[6] - Management Frame Protection required(802.11W) \n
  //! BIT[7] - listen interval from power save command \n
  uint8_t join_feature_bitmap;

  //! HT (High Throughput) capabilities parameters.
  rsi_uHtCaps ht_caps;

  //! ht_caps_magic_word – Fixed to 0x4321, used to validate the saved stored config info
  uint8_t ht_caps_magic_word[2];

  //! fast_psp_enable temporarily disables power save during data transfers and re-enables it after a monitor interval of inactivity.
  uint8_t fast_psp_enable;

  //! Time in milliseconds to keep device in wakeup state for each Tx or Rx traffic sent or received respectively. Default value for this is 50 ms.
  uint8_t monitor_interval[2];

  //! Magic word of request time out. Is fixed to 0x4321 in TA side.
  uint8_t request_timeout_magic_word[2];

  //! Timeout value used with timeout_bitmap to get timeout durations
  uint8_t timeout_value[2];

  //! bitmap[0]   - timeout for association and authentication request in ms. \n
  //! bitmap[1]   - each channel active scan time in ms \n
  //! bitmap[2] 	- WLAN keep alive timeout in seconds \n
  //! bitmap[3]   - Passive scan dwell time in ms \n
  //! bitmap[31-4] - Reserved (not used)
  uint8_t timeout_bitmap[4];

  // AP IP parameters in Concurrent mode
  //! DHCPv4 mode enable or disable.
  uint8_t dhcp_ap_enable;

  //! IPv4 address of the device in AP mode.
  uint8_t ap_ip[4];

  //! Sub-net mask
  uint8_t ap_sn_mask[4];

  //! Default gateway for the AP in AP mode.
  uint8_t ap_dgw[4];

  //! DHCP mode (has to be zero) for AP in IPv6
  uint8_t dhcp6_ap_enable[2];

  //! prefix length of IPv6 address in AP mode.
  uint8_t ap_prefix_length[2];

  //! IPv6 address of device in AP mode.
  uint8_t ap_ip6[16];

  //! IPv6 address of default router in AP mode.
  uint8_t ap_dgw6[16];

  //! Exteneded TCP/IP feature bitmap.
  uint8_t ext_tcp_ip_feature_bit_map[4];

  //! HTTP server credentials availability indicator. \n
  //! This field indicates the source of HTTP server authentication credentials (username/password). \n
  //! 0 - No HTTP credentials available (default) \n
  //! 1 - HTTP credentials available in runtime (stored in TCP/IP control block) \n
  //! 2 - HTTP credentials stored in saved configuration (flash memory)
  uint8_t http_credentials_avail;

  //! HTTP server username.
  uint8_t http_username[MAX_HTTP_SERVER_USERNAME];

  //! HTTP server password.
  uint8_t http_password[MAX_HTTP_SERVER_PASSWORD];
} rsi_cfgGetFrameRcv_t, rsi_user_store_config_t;

typedef struct wmm_info_s {
  uint8_t aifsn : 4;
  uint8_t acm : 1;
  uint8_t aci : 2;
  uint8_t res : 1;
  uint8_t ECW_min : 4;
  uint8_t ECW_max : 4;
  uint16_t txop;
} wmm_info_t;

// user configurable gain table structure
typedef struct rsi_gain_table_info_s {
  // band value
  uint8_t band;
  // bandwidth value
  uint8_t bandwidth;
  // payload size
  uint16_t size;
  uint32_t reserved;
  // payload
  uint8_t gain_table[1];

} rsi_gain_table_info_t;

// ram_dump structure
typedef struct rsi_ram_dump_s {
  uint32_t addr;
  uint32_t length;
  uint32_t uartSel;
} rsi_ram_dump_t;
// switch protocol structure
typedef struct rsi_switch_proto_s {
  uint8_t mode_value;
} rsi_switch_proto_t;

typedef struct rsi_freq_offset_s {
  int32_t freq_offset_in_khz;
} rsi_freq_offset_t;

typedef struct rsi_calib_write_s {
#define BURN_INTO_EFUSE 0
#define BURN_INTO_FLASH 1
  uint8_t target;
  uint8_t reserved0[3];
#define CALIB_WRITE_RESERVED     BIT(0)
#define BURN_FREQ_OFFSET         BIT(1)
#define SW_XO_CTUNE_VALID        BIT(2)
#define BURN_XO_FAST_DISABLE     BIT(3)
#define BURN_GAIN_OFFSET_LOW     BIT(4)
#define BURN_GAIN_OFFSET_MID     BIT(5)
#define BURN_GAIN_OFFSET_HIGH    BIT(6)
#define SELECT_GAIN_OFFSETS_1P8V BIT(7)
  uint32_t flags;
  int8_t gain_offset[3];
  uint8_t xo_ctune;
} rsi_calib_write_t;

typedef struct rsi_evm_offset_s {
  int8_t evm_offset_val;
  uint8_t evm_index;
} rsi_evm_offset_t;

typedef struct rsi_evm_write_s {
  uint8_t target;
  uint8_t reserved[3];
  uint32_t flags;
  int8_t evm_offset_11B;
  int8_t evm_offset_11G_6M_24M_11N_MCS0_MCS2;
  int8_t evm_offset_11G_36M_54M_11N_MCS3_MCS7;
  int8_t evm_offset_11N_MCS0;
  int8_t evm_offset_11N_MCS7;
} rsi_evm_write_t;
// csi config structure
typedef struct rsi_csi_config_s {
  // enable/disable CSI data retrieval
  uint32_t csi_enable;
  // periodicity of CSI data retrieval
  uint32_t periodicity;
  // number of MAC addresses
  uint32_t num_of_mac_addr;
  // MAC addresses
  uint8_t mac_addresses[1][6];
} rsi_csi_config_t;

/******************************************************
 * *                    Structures
 * ******************************************************/

/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/

uint8_t *rsi_extract_filename(uint8_t *json, uint8_t *buffer);
void rsi_check_wlan_buffer_full(rsi_pkt_t *pkt);
void rsi_check_common_buffer_full(rsi_pkt_t *pkt);

void rsi_wlan_packet_transfer_done(rsi_pkt_t *pkt);
int8_t rsi_wlan_cb_init(rsi_wlan_cb_t *wlan_cb);
uint32_t rsi_check_wlan_state(void);
int32_t rsi_wlan_radio_init(void);
void rsi_wlan_set_status(int32_t status);
int32_t rsi_driver_process_wlan_recv_cmd(rsi_pkt_t *pkt);
int32_t rsi_driver_wlan_send_cmd(rsi_wlan_cmd_request_t cmd, rsi_pkt_t *pkt);
int32_t rsi_wlan_check_waiting_socket_cmd(void);
int32_t rsi_wlan_check_waiting_wlan_cmd(void);
void rsi_wlan_process_raw_data(rsi_pkt_t *pkt);
int32_t rsi_wlan_filter_broadcast(uint16_t beacon_drop_threshold,
                                  uint8_t filter_bcast_in_tim,
                                  uint8_t filter_bcast_tim_till_next_cmd);
rsi_error_t rsi_wait_on_wlan_semaphore(rsi_semaphore_handle_t *semaphore, uint32_t timeout_ms);
void rsi_update_wlan_cmd_state_to_progress_state(void);
void rsi_update_wlan_cmd_state_to_free_state(void);
rsi_error_t rsi_wait_on_nwk_semaphore(rsi_semaphore_handle_t *semaphore, uint32_t timeout_ms);
int32_t rsi_wlan_get_nwk_status(void);
void rsi_wlan_set_nwk_status(int32_t status);
int32_t rsi_post_waiting_semaphore(void);
void rsi_call_asynchronous_callback(void);
void rsi_assertion_cb(uint16_t assert_val, uint8_t *buffer, const uint32_t length);
void rsi_max_available_rx_window(uint16_t status, uint8_t *buffer, const uint32_t length);
int32_t send_timeout(uint32_t timeout_bitmap, uint16_t timeout_value);
int32_t rsi_config_timeout(uint32_t timeout_type, uint16_t timeout_value);
#ifdef RSI_ENABLE_DEMOS
void rsi_wlan_app_callbacks_init(void);
void rsi_join_fail_handler(uint16_t status, uint8_t *buffer, const uint32_t length);
void rsi_ip_renewal_fail_handler(uint16_t status, uint8_t *buffer, const uint32_t length);
void rsi_remote_socket_terminate_handler(uint16_t status, uint8_t *buffer, const uint32_t length);
void rsi_ip_change_notify_handler(uint16_t status, uint8_t *buffer, const uint32_t length);
void rsi_stations_connect_notify_handler(uint16_t status, uint8_t *buffer, const uint32_t length);
void rsi_stations_disconnect_notify_handler(uint16_t status, uint8_t *buffer, const uint32_t length);
int32_t rsi_wlan_ping_async(uint8_t flags,
                            uint8_t *ip_address,
                            uint16_t size,
                            void (*wlan_ping_response_handler)(uint16_t status,
                                                               const uint8_t *buffer,
                                                               const uint16_t length));
#endif
#endif

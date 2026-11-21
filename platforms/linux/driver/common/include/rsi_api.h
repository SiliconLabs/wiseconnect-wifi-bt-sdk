/*******************************************************************************
* @file  rsi_api.h
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
 * @file     rsi_api.h
 * @version  3.6
 * @date     2013-May-16
 *
 *
 *
 * @brief HEADER: API specific Defines
 *
 * @section Description
 * This file contains the function prototypes of the APIs defined in library, and defines used in API's.
 */


#ifndef _SPIAPI_H_
#define _SPIAPI_H_

/**
 * Include Files
 */
#include "rsi_global.h"
#include "rsi_app_util.h"
/**
 * Global Defines
 */

//! Host interfaces
#define RSI_SPI    0
#define RSI_UART   1
#define RSI_USB    2
#define RSI_SDIO   3

#ifdef RS9116
//! Upgrade Image types
#define BURN_NWP_FW                      'B'
#else
#define RSI_UPGRADE_IMAGE_I_FW           '2'
#endif
#define RSI_UPGRADE_BL                   '#'


//! Firmware Upgradation form host params
#define FW_UP_PL  0
#define FW_UP_REQ 1
#ifdef RS9116
#define RSI_RPS_HEADER 64
#else
#define RSI_RPS_HEADER 32
#endif
#define RSI_FW_UP_SUCCESS 0x0003

//!Load Image types
#ifdef RS9116
#define LOAD_NWP_FW                          'A'
#else
#define LOAD_NWP_FW                          '1'
#endif
#define LOAD_DEFAULT_NWP_FW_ACTIVE_LOW       0x71 

//!Select Default image
#define SELECT_DEFAULT_NWP_FW               '5'

// bypass bootup
#define ENABLE_GPIO_BASED_BYPASS            '7'
#define DISABLE_GPIO_BASED_BYPASS           '8'

//!load default
#define RSI_LOAD_DEFAULT                  '9'

//! Check CRC
#define CHECK_NWP_INTEGRITY                     'K'


#define HOST_INTF_REG_OUT                 0x4105003C
#define HOST_INTF_REG_IN                  0x41050034
#define BOARD_READY                       0xABCD  
#define REG_READ                          0xD1
#define REG_WRITE                         0xD2
#define PONG_WRITE                        0xD4
#define PING_WRITE                        0xD5
#define GPIO_RESET                        0xD7
#define LOAD_BOOTLOADER                   0xD8
#ifdef RS9116
#define HOST_INTERACT_REG_VALID           (0xA0 << 8)
#define HOST_INTERACT_REG_VALID_READ      (0xAB << 8)
#else
#define HOST_INTERACT_REG_VALID           (0xAB << 8)
#define HOST_INTERACT_REG_VALID_READ      (0xAB << 8)
#endif
#define PONG_AVAIL                        'O'
#define PING_AVAIL                        'I'
#define PONG_VALID                        'O'   
#define PING_VALID                        'I'   
#define LOADING_INITIATED                 '1'
#define SEND_RPS_FILE                     '2'
#define FWUP_SUCCESSFUL                   'S'
#define EOF_REACHED                       'E'
#define BOOTUP_OPTIONS_LAST_CONFIG_NOT_SAVED 0xF1
#define BOOTUP_OPTIONS_CHECKSUM_FAIL      0xF2
#define INVALID_OPTION                    0xF3
#define CHECKSUM_SUCCESS                  0xAA
#define CHECKSUM_FAILURE                  0xCC
#define CHECKSUM_INVALID_ADDRESS          0x4C

#define RSI_SUCCESS                        0
#define RSI_BUSY                           -1
#define RSI_FAIL                           -2
#define RSI_BUFFER_FULL                    -3
#define RSI_IN_SLEEP                       -4


#define RSI_RESET_LOOP_COUNTER(X)              X = 0;
#define RSI_WHILE_LOOP(X, Y)                   while((X++) < (uint32)Y)
#define RSI_LOOP_COUNT_UPGRADE_IMAGE           0xFFFF
#define RSI_LOOP_COUNT_WAKEUP_REQ              0xFFFFFFFF
#define RSI_LOOP_COUNT_WAKEUP_WAIT             0xFFFFFFFF   
#define RSI_LOOP_COUNT_UPGRADE_REQ             0xFFFF
#define RSI_LOOP_COUNT_UPGRADE_CHUNK           0xFFFF
#define RSI_LOOP_COUNT_UPGRADE_STATUS          0xFFFF
#define RSI_LOOP_COUNT_SELECT_OPTION           0xFFFF
#define RSI_CHECK_LOOP_COUNTER(X, Y)           { if(X >= Y)\
                                                  return -1;}

//!SPI Internal Register Offset
#define RSI_SPI_INT_REG_ADDR               0x00      //@ register access method
#define RSI_SPI_MODE_REG_ADDR              0x08      //@ register access method

//!Power Mode Constants
#define RSI_POWER_MODE_0                   0x0000
#define RSI_POWER_MODE_1                   0x0001
#define RSI_POWER_MODE_2                   0x0002
#define RSI_POWER_MODE_3                   0x0003
#define RSI_POWER_MODE_8                   0x0008
#define RSI_POWER_MODE_9                   0x0009
#define RSI_LP_MODE                        0x0
#define RSI_ULP_MODE_WITH_RAM_RET          0x1
#define RSI_ULP_MODE_NO_RAM_RET            0x2
#define RSI_MAX_PSP                        0x0
#define RSI_FAST_PSP                       0x1
#define RSI_UAPSD                          0x2
//!Tx Power level
#define RSI_POWER_LEVEL_LOW                0x0000
#define RSI_POWER_LEVEL_MEDIUM             0x0001
#define RSI_POWER_LEVEL_HIGH               0x0002

//!Join feature bitmap
#define RSI_JOIN_FEAT_STA_BG_ONLY_MODE_ENABLE 	BIT(0)
#define RSI_JOIN_FEAT_LISTEN_INTERVAL_VALID 	BIT(1)

//! MFP type
#define MFP_DISABLE                         0                 //@Disable MFP(802.11W)
#define MFP_CAPABLE_ONLY                    BIT(5)            //@Enable Indicating MFP Capable only(802.11W)
#define MFP_CAPABLE_REQUIRED                (BIT(5) | BIT(6)) //@Enable Indicating MFP Capable and required(802.11W)

//!Interrupt Defines
#define RSI_INT_MASK_REG_ADDR              0x41050000      //@ Interrupt mask register
#define RSI_INT_CLR_REG_ADDR               0x22000010      //@ Interrupt clear register
//!bit5=1 to clear the interrupt, must use read-modify-write to preserver the other bits


/*======================================================*/
//!Management Request Frame Codes (Type)
#define RSI_REQ_OPERMODE                   0x10
#define RSI_REQ_BAND                       0x11
#define RSI_REQ_INIT                       0x12
#define RSI_REQ_SCAN                       0x13
#define RSI_REQ_JOIN                       0x14
#define RSI_REQ_PWRMODE                    0x15
#define RSI_REQ_SLEEP_TIMER                0x16
#define RSI_REQ_SET_MAC_ADDR               0x17
#define RSI_REQ_QUERY_NETWORK_PARAMS       0x18
#define RSI_REQ_DISCONNECT                 0x19
#define RSI_REQ_AP_STOP                    0xAE
#define RSI_REQ_RSSI_QUERY                 0x3A
#define RSI_REQ_ANTENNA_SEL                0x1B
#define RSI_REQ_SET_REG_CODE               0x1D 
#define RSI_REQ_CFG_SAVE                   0x20
#define RSI_REQ_CFG_ENABLE                 0x21
#define RSI_REQ_CFG_GET                    0x22
#define RSI_REQ_USER_STORE_CFG             0x23
#define RSI_REQ_APCONFIG                   0x24
#define RSI_REQ_SET_WEP_KEY                0x25
#define RSI_REQ_DEBUG_PRINTS               0x26
#define RSI_REQ_PING_PACKET                0x29
#define RSI_REQ_MULTICAST_FILTER           0x40
#define RSI_REQ_HOST_WEBPAGE               0x64
#define RSI_REQ_BG_SCAN                    0x6a
#define RSI_REQ_REJOIN_PARAMS              0x6F

//!IPV6 IPCONFIG REQ
#define RSI_REQ_IPCONF6                    0x90

//!SNMP COMMANDS
#define RSI_REQ_SNMP_GET                   0x80
#define RSI_REQ_SNMP_GETNEXT               0x81
#define RSI_REQ_SNMP_SET                   0x82

#define RSI_REQ_SNMP_ENABLE                0x85
#define RSI_REQ_SNMP_TRAP                  0x86

#define RSI_REQ_CREATE_JSON                0x9C
#define RSI_REQ_CLEAR_FILES                0x7F
#define RSI_REQ_ERASE_FILE                 0x9A
#define RSI_REQ_ERASE_JSON                 0x9B
#define RSI_REQ_HOST_PSK                   0xA5
#define RSI_REQ_MULTICAST                  0xB1
#define RSI_REQ_SET_REG_AP                 0xBD
#define RSI_REQ_IPPARAM_CONFIG             0x41
#define RSI_REQ_SOCKET_CREATE              0x42
#define RSI_REQ_SOCKET_CLOSE               0x43
#define RSI_REQ_DNS_QRY                    0x44
#define RSI_REQ_WMM_PARAMS                 0x45
#define RSI_REQ_TCP_CONN_STATUS_QUERY      0x46
#define RSI_REQ_CONNECTION_STATUS          0x48
#define RSI_REQ_FWVERSION_QUERY            0x49
#define RSI_REQ_GET_MAC_ADDR               0x4A
#define RSI_REQ_CFG_P2P                    0x4B
#define RSI_REQ_SET_EAP_CFG                0x4C
#define RSI_REQ_SET_CERT                   0x4D
#define RSI_REQ_QUERY_GO_PARAMS            0x4E
#define RSI_REQ_GET_WEBPAGE                0x50
#define RSI_REQ_HTTP_GET                   0x51
#define RSI_REQ_HTTP_POST                  0x52
#define RSI_REQ_DNS_SERVER                 0x55
#define RSI_REQ_READ                       0x6B
#define RSI_REQ_HTTP_POST_DATA             0xEB
#define RSI_REQ_HTTP_CREDENTIALS           0xB4

//!Roaming parameter request
#define RSI_REQ_ROAM_PARAMS                0x7B

//!HT CAPS request
#define RSI_REQ_AP_HT_CAPS                 0x6D


//!WMM PS request
#define RSI_REQ_WMM_PS                     0x97

//! Request types
#define RSI_REQ_FW_UP_REQ                    0x99

//!WPS MEHOD request
#define RSI_REQ_WPS_METHOD                 0x72

//!PER MODE request
#define RSI_REQ_PER_PARAMS                 0x7c

//! PER stats request
#define RSI_REQ_PER_STATS                  0xA2

//!Wireless firmware upgradation 
#define RSI_FWUP_RSP                       0x59

//! RF Current Mode configuration
#define RSI_REQ_RF_CURRENT_CONFIG          0xAD

//! Feature frame
#define RSI_REQ_FEATURE_FRAME              0xC8

//! socket configuration
#define RSI_REQ_SOCKET_CONFIG              0xA7

//! FTP client
#define RSI_REQ_FTP                        0xE2

//! SNTP client
#define RSI_REQ_SNTP                        0xE4

//! MDNS
#define RSI_REQ_MDNS                       0xDB

//! SMTP
#define RSI_REQ_SMTP                       0xE6

//!HTTP_PUT_CLIENT 
#define RSI_REQ_HTTP_CLIENT_PUT            0x53


//! POP3
#define RSI_REQ_POP3                       0xE7

//! DNS Update
#define RSI_REQ_DNS_UPDATE                 0xED

//! DHCP USER CLASS
#define RSI_REQ_DHCP_USER_CLASS                 0xEC

//! OTAF
#define RSI_REQ_OTA_FW_UP                  0xEF

//!Timeout
#define RSI_REQ_TIMEOUT                    0xEA 

//! Beacon stop
#define RSI_REQ_BEACON_STOP                0x63

//! Management Response Frame Codes
#define RSI_RSP_DATA_RECEIVE               0x00
#define RSI_RSP_CARD_READY                 0x89
#define RSI_RSP_OPERMODE                   0x10
#define RSI_RSP_BAND                       0x11
#define RSI_RSP_INIT                       0x12
#define RSI_RSP_SCAN                       0x13
#define RSI_RSP_JOIN                       0x14
#define RSI_RSP_PWRMODE                    0x15
#define RSI_RSP_SLEEP_TIMER                0x16
#define RSI_RSP_MACADDRESS_SET             0x17
#define RSI_RSP_NETWORK_PARAMS             0x18
#define RSI_RSP_DISCONNECT                 0x19
#define RSI_RSP_AP_STOP                    0xAE
#define RSI_RSP_RSSI_QUERY                 0x3A
#define RSI_RSP_ANTENNA_SEL                0x1B
#define RSI_RSP_SOFT_RESET                 0x1C
#define RSI_RSP_SET_REG_CODE               0x1D 
#define RSI_RSP_FEATSEL                    0x1E
#define RSI_RSP_SNR                        0x1F
#define RSI_RSP_CFG_SAVE                   0x20
#define RSI_RSP_CFG_ENABLE                 0x21
#define RSI_RSP_CFG_GET                    0x22
#define RSI_RSP_USER_STORE_CFG             0x23
#define RSI_RSP_APCONFIG                   0x24
#define RSI_RSP_SET_WEP_KEY                0x25
#define RSI_RSP_DEBUG_PRINTS               0x26
#define RSI_RSP_GPIO_CONF                  0x28
#define RSI_RSP_PING_PACKET                0x29
#define RSI_RSP_P2P_CONNREQ                0x30
#define RSI_RSP_MULTICAST_FILTER           0x40
#define RSI_RSP_BG_SCAN                    0x6a
#define RSI_RSP_REJOIN_PARAMS              0x6F
#define RSI_RSP_MODULE_STATE               0x70
#define RSI_RSP_ULP_NO_RAM_RET             0xCD
#define RSI_RSP_FEATURE_FRAME              0xC8 

//!SNMP RESPONSES
#define RSI_RSP_SNMP_GET_RSP               0x83
#define RSI_RSP_SNMP_GETNEXT_RSP           0x84
#define RSI_RSP_SNMP_ENABLE                0x85
#define RSI_RSP_SNMP_TRAP                  0x86

//!IPV6 IPCONFIG RSP
#define RSI_RSP_IPCONF6                    0xa1

//! PER stats RSP
#define RSI_RSP_PER_STATS                  0xA2

//! UART FLow control
#define RSI_RSP_UART_FLOW_CONTROL          0xA4

//!IP change ASYNC MSG 
#define RSI_RSP_IPCHANGE_NOTIFY            0xaa
#define RSI_RSP_HOST_PSK                   0xA5
#define RSI_RSP_SET_REG_AP                 0xBD
#define RSI_FWUP_OK                        0x5a

//! Control Response Frame Codes
#define RSI_RSP_NULL                       0x00
#define RSI_RSP_IPPARAM_CONFIG             0x41
#define RSI_RSP_SOCKET_CREATE              0x42
#define RSI_RSP_CLOSE                      0x43
#define RSI_RSP_DNS_QRY                    0x44
#define RSI_RSP_WMM_PARAMS                 0x45
#define RSI_RSP_TCP_CONN_STATUS_QUERY      0x46
#define RSI_RSP_CONNECTION_STATUS          0x48
#define RSI_RSP_FWVERSION_QUERY            0x49
#define RSI_RSP_MAC_QUERY                  0x4A
#define RSI_RSP_CFG_P2P                    0x4B
#define RSI_RSP_SET_EAP_CFG                0x4C
#define RSI_RSP_SET_CERT                   0x4D
#define RSI_RSP_QUERY_GO_PARAMS            0x4E
#define RSI_RSP_GET_WEBFIELDS              0x4F  
#define RSI_RSP_GET_WEBPAGE                0x50
#define RSI_RSP_HTTP_GET                   0x51
#define RSI_RSP_HTTP_POST                  0x52
#define RSI_RSP_SENT_BYTES                 0xB2
#define RSI_RSP_HTTP_ABORT                 0xB3
#define RSI_RSP_HTTP_CREDENTIALS           0xB4
#define RSI_RSP_HTTP_POST_DATA             0xEB


//!Roaming parameter response
#define RSI_RSP_ROAM_PARAMS                0x7B


#define RSI_RESP_WFD_DEV                   0x54
#define RSI_RSP_CONN_ESTABLISH             0x61
#define RSI_RSP_REMOTE_TERMINATE           0x62
#define RSI_RSP_DNS_SERVER                 0x55
#define RSI_RSP_HOST_WEBPAGE               0x56
 
//! HT CAPS response
#define RSI_RSP_AP_HT_CAPS                 0x6D

//! WMM PS response 
#define RSI_RSP_WMM_PS                     0x97

//! Response types
#define RSI_RSP_FW_UP_REQ                    0x99

//!WPS MEHOD response
#define RSI_RSP_WPS_METHOD                 0x72

//! PER MODE response
#define RSI_RSP_PER_PARAMS                 0x7c
#define RSI_RSP_CREATE_JSON                0x9c
#define RSI_RSP_CLEAR_FILES                0x7f
#define RSI_RSP_ERASE_FILE                 0x9A
#define RSI_RSP_ERASE_JSON                 0x9B
#define RSI_RSP_JSON_UPDATE                0x9D
#define RSI_RSP_MULTICAST                  0xB1

#define RSI_RSP_SLP                        0xDE
#define RSI_RSP_WKP                        0xDD
/* Ack to received data */
#define RSI_RSP_DATA_PACKET_ACK            0xAC

#define RSI_RSP_CLIENT_CONNECTED           0xC2
#define RSI_RSP_CLIENT_DISCONNECTED        0xC3

//! RF Current Mode configuration
#define RSI_RSP_RF_CURRENT_CONFIG          0xAD

//! socket configuration
#define RSI_RSP_SOCKET_CONFIG              0xA7

//! FTP client
#define RSI_RSP_FTP                        0xE2

//! SNTP client
#define RSI_RSP_SNTP                        0xE4
#define RSI_RSP_SNTP_SERVER                 0xE5

//! MDNS
#define RSI_RSP_MDNS                       0xDB

//! SMTP
#define RSI_RSP_SMTP                       0xE6

//! HTTP_PUT_CLIENT 
#define RSI_RSP_HTTP_CLIENT_PUT            0x53

//! POP3
#define RSI_RSP_POP3                       0xE7
#define RSI_RSP_POP3_REMOTE_TERMINATE      0xE8

//! HTTP POST DATA
#define RSI_HTTP_END_OF_DATA               BIT(0)
#define RSI_RSP_HTTP_POST_START            BIT(1)
#define RSI_RSP_HTTP_POST_DATA_SEND        BIT(2)
#define RSI_RSP_HTTP_POST_DATA_RECV        BIT(3)


//! DNS Update
#define RSI_RSP_DNS_UPDATE                 0xED

//! DHCP USER CLASS
#define RSI_RSP_DHCP_USER_CLASS                 0xEC

//! OTAF
#define RSI_RSP_OTA_FW_UP                  0xEF

//!Timeout
#define RSI_RSP_TIMEOUT                    0xEA 

//! Beacon stop
#define RSI_RSP_BEACON_STOP                0x63

/*==============================================*/
//! Band Defines
#define RSI_BAND_2P4GHZ                    0x00  
#define RSI_BAND_5GHZ                      0x01  
#define RSI_DUAL_BAND                      0x02  

//! TCPIP Defines
#define RSI_STATIC_IP_MODE                 0x00
#define RSI_DHCP_IP_MODE                   0x01
#define RSI_FEAT_DHCP_HOST_NAME            0x02
#define RSI_FEAT_DHCP_FQDN                 0x04
#define RSI_DNS_MODE_ENABLE                0x01
#define RSI_DNS_MODE_DISABLE               0x00
#define RSI_AUTO_IP_CFG                    0x02
#define RSI_DHCP_UNICAST_OFFER_SUPPORT     0x08

//!CERTIFICATE defines
#define RSI_EAP_TLS_CERTIFICATE            0x01    //@ WLAN EAP-TLS Client Certificate/Private Key/CA Root
#define RSI_EAP_FAST_PAC_CERTIFICATE       0x02    //@ WLAN EAP-FAST PAC file
#define RSI_SSL_CLIENT_CERTIFICATE         0x03    //@ SSL client certificate
#define RSI_SSL_CLIENT_PRIVATE_KEY         0x04    //@ SSL client private key
#define RSI_SSL_CA_CERTIFICATE             0x05    //@ SSL CA certificate
#define RSI_SSL_SERVER_CERTIFICATE         0x06    //@ SSL server certificate
#define RSI_SSL_SERVER_PRIVATE_KEY         0x07    //@ SSL server private key

//!SOCKET Defines
#define RSI_SOCKET_TCP_CLIENT              0x0000 
#define RSI_SOCKET_UDP_CLIENT              0x0001 
#define RSI_SOCKET_TCP_SERVER              0x0002 
#define RSI_SOCKET_LUDP                    0x0004 

#define RSI_AP_VAP_ID                      1
#define RSI_CLIENT_VAP_ID                  2

//!SEND Defines
#define RSI_UDP_FRAME_HEADER_LEN           44
//!UDP Frame header is 42 bytes, padding bytes are extra
#define RSI_TCP_FRAME_HEADER_LEN           56                                                
//!TCP Frame header is 54 bytes, padding bytes are extra
#define RSI_UDP_V6_FRAME_HEADER_LEN        64 
//!Frame header is 42 bytes, padding bytes are extra
#define RSI_TCP_V6_FRAME_HEADER_LEN        76
//!TCP Frame header is 54 bytes, padding bytes are extra
#define RSI_UDP_SEND_OFFSET                32                                  
//!Offset of sent UDP payload data
#define RSI_TCP_SEND_OFFSET                44                                
//!Offset of sent TCP payload data
#define RSI_UDP_RECV_OFFSET                26                                
//!Offset of received UDP payload data
#define RSI_TCP_RECV_OFFSET                38                                
//!Offset of received TCP payload data

//!Security types for AP Configuration
//! SECURITY Type Defines
#define RSI_SECURITY_NONE                   0                   
//! Security type NONE and OPEN are alises for each other
#define RSI_SECURITY_OPEN                   0
#define RSI_SECURITY_WPA1                   1
#define RSI_SECURITY_WPA2                   2

//! Encryption type for AP Configuration 
#define RSI_ENCRYPTION_NONE                 0                   
#define RSI_ENCRYPTION_TKIP                 1
#define RSI_ENCRYPTION_AES                  2

//! NETWORK Type
#define RSI_IBSS_OPEN_MODE                  0
#define RSI_INFRASTRUCTURE_MODE             1
#define RSI_IBSS_SEC_MODE                   2
#define RSI_IBSS_JOINER                     0
#define RSI_IBSS_CREATOR                    1

// DATA Rates
#define RSI_DATA_RATE_AUTO                  0
#define RSI_DATA_RATE_1                     1
#define RSI_DATA_RATE_2                     2
#define RSI_DATA_RATE_5P5                   3
#define RSI_DATA_RATE_11                    4
#define RSI_DATA_RATE_6                     5
#define RSI_DATA_RATE_9                     6
#define RSI_DATA_RATE_12                    7
#define RSI_DATA_RATE_54                    12
#define RSI_DATA_RATE_20                    20
#define RSI_DATA_RATE_18                    18


#define RSI_MACADDRLEN                      6

#define RSI_MODE_8BIT                       0
#define RSI_MODE_32BIT                      1

//!PER mode data rates
#define RSI_RATE_1                         0x0
#define RSI_RATE_2                         0x2
#define RSI_RATE_5_5                       0x4
#define RSI_RATE_11                        0x6
#define RSI_RATE_6                         0x8b
#define RSI_RATE_9                         0x8f
#define RSI_RATE_12                        0x8a
#define RSI_RATE_18                        0x8e
#define RSI_RATE_24                        0x89
#define RSI_RATE_36                        0x8d
#define RSI_RATE_48                        0x88
#define RSI_RATE_54                        0x8c
#define RSI_RATE_MCS0                      0x100
#define RSI_RATE_MCS1                      0x101
#define RSI_RATE_MCS2                      0x102
#define RSI_RATE_MCS3                      0x103
#define RSI_RATE_MCS4                      0x104
#define RSI_RATE_MCS5                      0x105
#define RSI_RATE_MCS6                      0x106
#define RSI_RATE_MCS7                      0x107
#define RSI_RATE_MCS7_SG                   0x307

#define RSI_PER_BURST_MODE               0
#define RSI_PER_CONTT_MODE               1
#define RSI_PER_CONT_WAVE_MODE_DC        2

//! WPS
#define RSI_WPS_PUSH_METHOD              0
#define RSI_WPS_PIN_METHOD               1

//!Multicast
#define RSI_MULTICAST_LEAVE              0
#define RSI_MULTICAST_JOIN               1

//!Multicast filter cmds
#define RSI_MULTICAST_MAC_CLEAR_ALL      2
#define RSI_MULTICAST_MAC_SET_ALL        3


//!SNMP
#define SNMP_ANS1_INTEGER                0x2
#define SNMP_ANS1_OCTET_STRING           0x4
#define SNMP_ANS1_OBJECT_ID              0x6
#define SNMP_ANS1_IP_ADDRESS             0x40
#define SNMP_ANS1_COUNTER                0x41 
#define SNMP_ANS1_GAUGE                  0x42
#define SNMP_ANS1_TIME_TICS              0x43
#define SNMP_ANS1_IPV6_ADDRESS           0x44
#define SNMP_ANS1_COUNTER64              0x46
#define SNMP_ANS1_NO_SUCH_OBJECT         0x80
#define SNMP_ANS1_NO_SUCH_INSTANCE       0x81
#define SNMP_ANS1_END_OF_MIB_VIEW        0x82

#define SNMP_VERSION_1                   1
#define SNMP_VERSION_2                   2
#define SNMP_VERSION_3                   3

#define IP_VERSION_4                     4                  
#define IP_VERSION_6                     6

#define SNMP_TRAP_COLD_START             0
#define SNMP_TRAP_WARM_START             1
#define SNMP_TRAP_LINK_DOWN              2
#define SNMP_TRAP_LINK_UP                3
#define SNMP_TRAP_AUTH_FAILURE           4
#define SNMP_TRAP_EGP_NEIGHBOURLOSS      5 
#define SNMP_TRAP_USER_SPECIFIC          6
 
#define BIT(a) ((long int)1 << a)
//! feature bit map
#define FEAT_SECURITY_OPEN               BIT(0)
#define FEAT_SECURITY_PSK                BIT(1)
#define FEAT_AGGREGATION                 BIT(2)
#define FEAT_LP_GPIO_BASED_HANDSHAKE     BIT(3)
#define FEAT_ULP_GPIO_BASED_HANDSHAKE    BIT(4)
#define FEAT_DEV_TO_HOST_ULP_GPIO_1      BIT(5)
#define FEAT_RF_SUPPY_VOL_3_3_VOLT       BIT(6)

//! tcp/ip feature bit map
#define TCP_IP_FEAT_BYPASS               BIT(0)
#define TCP_IP_FEAT_HTTP_SERVER          BIT(1)
#define TCP_IP_FEAT_DHCPV4_CLIENT        BIT(2)
#define TCP_IP_FEAT_DHCPV6_CLIENT        BIT(3)
#define TCP_IP_FEAT_DHCPV4_SERVER        BIT(4)
#define TCP_IP_FEAT_DHCPV6_SERVER        BIT(5)
#define TCP_IP_FEAT_JSON_OBJECTS         BIT(6)
#define TCP_IP_FEAT_HTTP_CLIENT          BIT(7)
#define TCP_IP_FEAT_DNS_CLIENT           BIT(8)
#define TCP_IP_FEAT_SNMP_AGENT           BIT(9)
#define TCP_IP_FEAT_SSL                  BIT(10)
#define TCP_IP_FEAT_ICMP                 BIT(11)
#define TCP_IP_FEAT_HTTPS_SERVER         BIT(12)
#define TCP_IP_FEAT_FTP_CLIENT           BIT(15)
#define TCP_IP_FEAT_SNTP_CLIENT          BIT(16)

#define TCP_IP_FEAT_MDNSD		             BIT(19)
#define TCP_IP_FEAT_SMTP_CLIENT		       BIT(20)
#define TCP_IP_FEAT_POP3_CLIENT		       BIT(29)
#define TCP_IP_FEAT_OTAF                 BIT(30)


#define TCP_IP_FEAT_SINGLE_SSL_SOCKET		 BIT(25)
#define TCP_IP_FEAT_LOAD_PUBLIC_PRIVATE_CERTS   BIT(26)

#define TCP_IP_FEAT_EXTENSION_VALID     	     BIT(31)

#define EXT_TCP_FEAT_DHCP_OPT77      BIT(1)

#define EXT_TCP_IP_HTTP_SERVER_BYPASS      BIT(2)

//! Select No of sockets

#define TCP_IP_TOTAL_SOCKETS_1     (1<<21)
#define TCP_IP_TOTAL_SOCKETS_2     (2<<21) 
#define TCP_IP_TOTAL_SOCKETS_3     (3<<21)
#define TCP_IP_TOTAL_SOCKETS_4     (4<<21)
#define TCP_IP_TOTAL_SOCKETS_5     (5<<21)
#define TCP_IP_TOTAL_SOCKETS_6     (6<<21)
#define TCP_IP_TOTAL_SOCKETS_7     (7<<21)
#define TCP_IP_TOTAL_SOCKETS_8     (8<<21)
#define TCP_IP_TOTAL_SOCKETS_9     (9<<21)
#define TCP_IP_TOTAL_SOCKETS_10    (10<<21)

//!Custom feature select bitmap
#define FEAT_SEL_GATEWAY_SKIP                    BIT(2)	
#define FEAT_SEL_DISABLE_INTERNAL_WEB_SERVER     BIT(3)
#define FEAT_SEL_UART_HW_FLOW_CTRL               BIT(4)
#define FEAT_SEL_HIDDEN_AP                       BIT(5)
#define FEAT_SEL_INCLUDE_DNS                     BIT(6)
#define FEAT_SEL_DHCP_UNICAST                    BIT(7)
#define FEAT_SEL_DFS_CHANNEL_SUPPORT             BIT(8)
#define FEAT_SEL_LED_SUPPORT                     BIT(9)

#define FEAT_SEL_DISABLE_SUPP_REJECTLIST          BIT(12)   	
#define FEAT_SEL_ROAM_MODE_WITH_DEAUTH           BIT(13)   	
 
#define FEAT_WAIT_ON_HOST                        BIT(20)
#define FEAT_EVK_HTTP_SERVER_PKT_SIZE            BIT(21)	
#define FEAT_HTTP_SERVER_AUTHENTICATION          BIT(23)	
#define FEAT_HTTP_SERVER_CREDENTIALS             BIT(25) //! Http server credentials details in get configuration command
#define FEAT_LTCP_ZERO_LISTEN_REQUEST_QUEUE      BIT(26)
#define FEAT_CUSTOM_FEAT_EXTENTION_VALID         BIT(31)

//! Extended custom feature select bitmap
#define EXT_FEAT_RSA_KEY_WITH_4096_SUPPORT       BIT(1)
#define EXT_FEAT_TELEC_SUPPORT                   BIT(2)
#define EXT_FEAT_SSL_CERT_WITH_4096_KEY_SUPPORT  BIT(3)
#define EXT_FEAT_AP_BROADCAST_PKT_SND_B4_DTIM    BIT(4)
#define EXT_FEAT_FCC_LOW_PWR    		             BIT(5)
#define EXT_FEAT_PUF_PRIVATE_KEY                 BIT(7)
#define EXT_FEAT_SPECTRAL_MASK_NOKIA             BIT(8)
#define EXT_HTTP_SKIP_DEFAULT_LEADING_CHARACTER  BIT(9)

//! To enable 802.11R Over The Air Roaming
#define EXT_FEAT_ENABLE_11R_OTA                  BIT(11)

//! To enable 802.11J protocol
#define EXT_FEAT_IEEE_80211J                     BIT(12)  

//! To enable 802.11W protocol
#define EXT_FEAT_IEEE_80211W                     BIT(13)   

//! To enable the Multiverion TCP over SSL support
#define EXT_FEAT_SSL_VERSIONS_SUPPORT            BIT(14)   

//! To Enable 16 client support
#define EXT_FEAT_16th_STATION_IN_AP_MODE         BIT(15)

//! To enable 802.11R Over the Distribution System Roaming
#define EXT_FEAT_ENABLE_11R_ODS             BIT(16)	
//! Low power mode feature 
#define EXT_FEAT_LOW_POWER_ENABLE           BIT(19)
//! To Enable 256K memory mode for NWP so M4 will have 128K only in WiseMCU mode
#define EXT_FEAT_256K_MODE                  BIT(21)
//! To Enable 320K memory mode for NWP so M4 will have 64K only in WiseMCU mode
#define EXT_FEAT_320K_MODE                  BIT(20)
//! To Enable 328K memory mode for NWP ,This bit is not applicable  in WiseMCU mode
#define EXT_FEAT_384K_MODE                 (BIT(20) | BIT(21))

//! To select UART debug prints pin selection
//! If BIT(27) is enabled,Debug prints are supported on UART1
//! If BIT(27) is disabled,Debug prints are supported on UART2
#define EXT_FEAT_UART_SEL_FOR_DEBUG_PRINTS  BIT(27)

//! If this bit is enabled,NWP disables Debug prints support
#define EXT_FEAT_DISABLE_DEBUG_PRINTS       BIT(28)


//! socket feature
#define RSI_SSL_SUPPORT                  BIT(0)
#define RSI_WEBS_SUPPORT                 BIT(1)
#define RSI_SSL_WEBS_SUPPORT             (BIT(0) | BIT(1))
#define RSI_SSL_TLS_V_1                  BIT(2)
#define RSI_SSL_TLS_V_1_2                BIT(3)
#define RSI_SSL_TLS_V_1_1                BIT(4)
#define RSI_TCP_HIGH_PERFORMANCE         BIT(7)

//! Socket bitmap
#define RSI_SYNCHRONOUS_DATA_READ       BIT(0)
#define RSI_LTCP_ACCEPT                 BIT(1)
#define RSI_ASYNCHRONOUS_DATA_READ      BIT(3)
#define RSI_TCP_RX_WINDOW               BIT(4)

//! HTTP Feature
#define RSI_HTTP_NULL_DELIMITER          BIT(1)
#define RSI_FEAT_HTTP_POST_DATA          BIT(5)
#define RSI_FEAT_HTTP_V_1_1              BIT(6)


//!PMK
#define RSI_PSK_FROM_HOST               1
#define RSI_PMK_FROM_HOST               2
#define RSI_GENERATE_PMK                3

//! SMTP feature
#define RSI_SMTP_MAIL_PRIORITY_LOW       BIT(0)
#define RSI_SMTP_MAIL_PRIORITY_NORMAL    BIT(1)        
#define RSI_SMTP_MAIL_PRIORITY_HIGH      BIT(2)        
#define RSI_FEAT_SMTP_EXTENDED_HEADER    BIT(3)
#define RSI_SMTP_AUTH_PLAIN              1
#define RSI_SMTP_AUTH_LOGIN              3 

//! SSL
#define SSL_ALL_CIPHERS 0
#define BIT_TLS_RSA_WITH_AES_256_CBC_SHA256    (1<<0)
#define BIT_TLS_RSA_WITH_AES_128_CBC_SHA256    (1<<1)
#define BIT_TLS_RSA_WITH_AES_256_CBC_SHA       (1<<2)
#define BIT_TLS_RSA_WITH_AES_128_CBC_SHA       (1<<3)
#define BIT_TLS_RSA_WITH_AES_128_CCM_8         (1<<4)
#define BIT_TLS_RSA_WITH_AES_256_CCM_8         (1<<5)


//! FTP client commands
#define FTP_COMMAND_CREATE              1
#define FTP_COMMAND_CONNECT             2
#define FTP_COMMAND_DIRECTORY_MAKE      3
#define FTP_COMMAND_DIRECTORY_DELETE    4
#define FTP_COMMAND_DIRECTORY_CWD       5
#define FTP_COMMAND_DIRECTORY_LIST      6
#define FTP_COMMAND_FILE_READ           7
#define FTP_COMMAND_FILE_WRITE          8
#define FTP_COMMAND_FILE_WRITE_CONTENT  9
#define FTP_COMMAND_FILE_DELETE         10
#define FTP_COMMAND_FILE_RENAME         11
#define FTP_COMMAND_DISCONNECT          12
#define FTP_COMMAND_DESTROY             13
#define FTP_COMMAND_PASSIVE_MODE        14
#define FTP_COMMAND_ACTIVE_MODE         15

//! SNTP client commands
#define SNTP_COMMAND_CREATE             1
#define SNTP_GET_TIME                   2
#define SNTP_GET_TIME_DATE              3
#define SNTP_GET_SERVER_ADDRESS         4
#define SNTP_DELETE                     5
#define SNTP_GET_SERVER_INFO            6

#define SNTP_BROADCAST_MODE             1
#define SNTP_UNICAST_MODE               2

//! MDNS commands 
#define MDNS_INIT                       1
#define MDNS_REGISTER_SERVICE           3
#define MDNS_DEINT                      6

/**
 * Enumerations
 */
enum RSI_INTTYPE { 
    RSI_IRQ_NONE             = 0x00,
    RSI_IRQ_BUFFERFULL       = 0x01,
    RSI_IRQ_BUFFEREMPTY      = 0x02,
    RSI_IRQ_MGMTPACKET       = 0x04,
    RSI_IRQ_DATAPACKET       = 0x08,
    RSI_IRQ_10               = 0x10,
    RSI_IRQ_PWRMODE          = 0x20,
    RSI_IRQ_40               = 0x40,
    RSI_IRQ_80               = 0x80,
    RSI_IRQ_ANY              = 0xff
};

//! PS defines 
#define PS_CONFIRM              0x2


/*=====================================================================================*/
/**
 *         This is platform dependent operation.Needs to be implemented 
 *         specific to the platform.This timer is mentioned in the following functions
 *             Application/TCPDemo/Source/main.c
 *             WLAN/SPI/Source/spi_functs.c
 *             WLAN/SPI/Source/spi_iface_init.c
 *     
 */

extern uint32            rsi_spiTimer1;
extern uint32            rsi_spiTimer2;
extern uint32            rsi_spiTimer3;

extern volatile rsi_powerstate rsi_pwstate;
/*
 * Function Prototype Definitions
 */
int16 rsi_oper_mode(rsi_uOperMode *uOperMode);
int16 rsi_band(uint8 band);
int16 rsi_init(void);
int16 rsi_scan(rsi_uScan *uScanFrame);
int16 rsi_send_feature_frame(rsi_uFeatureFrame *ufeatureframe);
int16 rsi_join(rsi_uJoin *uJoinFrame);
int16 rsi_ip_param_set(rsi_uIpparam *uIpparamFrame);
int16 rsi_ipv6_param_set(rsi_uIPconf6  *uIpconf6Frame);
int16 rsi_query_rssi(void);
int16 rsi_query_snr(void);
int16 rsi_set_mac_addr(uint8 *macAddress);
int16 rsi_query_mac_address(void);
int16 rsi_power_mode(uint8 powermode, uint8 ulp_mode_enable, uint8 listen_interval_dtim, uint8 rsi_psp_type, uint16 monitor_interval);
int16 rsi_socket(rsi_uSocket *uSocketFrame);
int16 rsi_socket_close(uint16 socketDescriptor, uint16 port_number);
int16 rsi_query_conn_status(void);
int16 rsi_query_fw_version(void);
int16 rsi_query_net_parms(void);
int16 rsi_select_antenna(uint8 antenna_val,uint8 gain_2g,uint8 gain_5g);
int16 rsi_query_go_parms(void);
int16 rsi_p2p_config(rsi_uConfigP2p *uConfigP2p);
int16 rsi_set_eap(rsi_uSetEap *uSetEap);
int16 rsi_set_certificate_index(uint8 certificate_type, uint8 cert_inx, uint8 *buffer,uint32 certificate_length, struct SET_CHUNK_S *SetChunks);
int16 rsi_set_certificate(uint8 certificate_type,uint8 *buffer,uint32 certificate_length, struct SET_CHUNK_S *SetChunks);
int16 rsi_load_web_page(rsi_uWebServer *uWebServer, uint8* webpage_file, uint8* webpage_load_done);
int16 rsi_send_url_rsp(HostWebpageSnd_t *uUrlRsp, uint8 *webpage_morechunks, uint8 *webpage_file , uint32 file_size, uint8 first_chunk);
int16 rsi_web_fields(rsi_uWebFields *uWebFields);
int16 rsi_sleep_timer(rsi_uSleepTimer *uSleepTimer);
int16 rsi_module_soft_reset(void);
int16 rsi_set_ap_config(rsi_apconfig *apconf);
int16 rsi_set_wepkey(rsi_wepkey *wepkey);
int16 rsi_dns_server(rsi_uDnsServer *uDnsServer);
int16 rsi_http_get(rsi_uHttpReq *uHttpGetReqFrame);
int16 rsi_http_post(rsi_uHttpReq *uHttpGetPostFrame);
int16 rsi_dns_query(rsi_uDnsQry  *uDnsQryFrame);
int16 rsi_dns_update(rsi_uDnsUpdate  *uDnsUpdateFrame);
int16 rsi_ltcp_conn_status(uint16 socketDescriptor);
int16 rsi_ping_request(rsi_ping_request_t *pingReq);
int16 rsi_disconnect(rsi_disassoc_t *disassoc_Frame);
int16 rsi_sys_init(void);
uint8* rsi_fill_parameters(uint32 type, uint8 *buffer);
int16 rsi_fwup_frm_host(rsi_fw_up_t *ptr_fw_up, uint8 *rps_file,uint32 rps_offset,uint16 length,uint16 type);
/*Multicast api's*/
uint8 lmac_crc8_c(uint8 crc8_din, uint8 crc8_state, uint8 end);
uint8 multicast_mac_hash(uint8 *mac);
int16 rsi_multicast_mac_filter(uint8 cmd, uint8 MAC[6]);
int16 rsi_multicast(rsi_uMulticast *uMulticastFrame);

int16 rsi_rejoin_params(rsi_rejoin_params_t *uRejoinFrame);
int16 rsi_psk(rsi_uPsk *uPskReqFrame);
int16 rsi_bg_scan(rsi_ubgScan *ubgScanFrame);

/* For power save */
int16 rsi_pwrsave_continue(void);
int16 rsi_pwrsave_hold(void);
void config_gpio_output(uint8 value);
void config_gpio_input(void);
uint8 get_gpio_value();
uint8 get_spi_intr_gpio_value();
int16 rsi_http_abort(void);
int16 rsi_cfg_enable(uint8 cfg_enable_val);
int16 rsi_cfg_get(void);
int16 rsi_cfg_save(void);

int16 rsi_debug_prints(rsi_uDebug *uDebugFrame);

int16 rsi_module_power_cycle(void);
void  rsi_build_frame_descriptor(rsi_uFrameDsc *uFrameDscFrame, uint8 *cmd);
int16 rsi_spi_frame_dsc_wr(rsi_uFrameDsc *uFrmDscFrame);
int16 rsi_spi_frame_data_wr(uint16 bufLen, uint8 *dBuf,uint16 tbufLen,uint8 *tBuf);
int16 rsi_frame_write(rsi_uFrameDsc *uFrameDscFrame,uint8 *payloadparam,uint16 size_param);
int16 rsi_execute_cmd(uint8 *descparam,uint8 *payloadparam,uint16 size_param);
int16 rsi_send_data(uint16 socketDescriptor, uint8 *payload, uint32 payloadLen,uint8 protocol,uint32 *bytes_sent);
int32 rsi_send_raw_data(uint8 *payload, uint32 payloadLen,uint32 *bytes_sent);
int16 rsi_send_ludp_data(uint16 socketDescriptor, uint8 *payload,uint32 payloadLen,uint8 protocol, uint8 *destIp, uint16 destPort,uint32 *bytes_sent);
int16 rsi_frame_read(uint8 *pkt_buffer);
void  rsi_wakeup_from_host(void);
int32 rsi_req_wakeup(void);
int32 rsi_wait4wakeup(void);
void rsi_allow_sleep(void);
int16 rsi_wireless_fwupgrade(void);
#ifdef RS9116
int16 rsi_gpio_reset(uint8 require_board_ready);
void rsi_module_gpio_reset(void);
#endif



/* For SNMP */
int16 rsi_snmp_enable(uint8 snmpEnable);
int16 rsi_snmp_get_rsp(rsi_uSnmp *uSnmpGetRsp);
int16 rsi_snmp_get_next_rsp(rsi_uSnmp *uSnmpGetNextRsp);
int16 rsi_snmp_trap(rsi_uSnmptrap  *uSnmpTrapFrame, int snmp_len);


/* For IPV6 ipconfig*/
int16 rsi_ipconf6(rsi_uIPconf6  *uIpconf6Frame);

/* Bootup configuration APIs*/
int16 rsi_waitfor_boardready(void);
int16 rsi_upgrade_fw(uint8 image_type, uint8 *fw_image, uint32 fw_image_size);
int16 rsi_boot_insn(uint8 type, uint16* data);
int16 rsi_select_option(uint8 cmd);


/* Roaming parameter set API*/
int16 rsi_roam_params(rsi_uRoamParams *uRoamParamsFrame);

/* HT CAPS parameter wet API*/
int16 rsi_ht_caps(rsi_uHtCaps *uHtCapsFrame);

/* WMM PS parameter wet API*/
int16 rsi_wmm_ps(rsi_uWmmPs *uWmmPsFrame);

/* WPS PIN METHOD parameter API*/
int16 rsi_wps_method(rsi_uWpsMethod *uWpsMethodFrame);

/* PER MODE parameter API*/
int16 rsi_per_mode(rsi_uPerMode *uPerMode);
/* PER stats API */
int16 rsi_per_stats(rsi_uPerStats *uPerStats);

/* JSON & Webpage Related Functionality */
int16 rsi_json_create(rsi_jsonCreateObject_t* json, uint8* buffer, uint8 *json_load_done);
int16 rsi_json_erase_file(rsi_tfs_erase_file_t* file);
int16 rsi_webpage_clear_files(rsi_tfs_clear_files_t* clear);
int16 rsi_webpage_erase_file(rsi_tfs_erase_file_t* file);

/*set region */
int16 rsi_set_region(rsi_usetregion *uSetRegionReqFrame);
/*Set region AP*/
int16 rsi_set_region_ap(rsi_usetregion_ap_t *uSetRegionApReqFrame);
int16 rsi_send_websocket_data(uint16 socketDescriptor, uint8 *payload, uint32 payloadLen,uint8 protocol, uint8 opcode, uint32 *total_bytes_sent);
int16 rsi_user_store_config(rsi_user_store_config_t  *ptr_userstrcfg);
void rsi_buildHttpExtendedHeader(uint8 *http_header, uint8 mode);
int16 rsi_bytes_sent_count(uint16 socketDescriptor);
/* RF Current Configuration */
int16 rsi_rf_current_config(rsi_rf_current_config_t *rf_config);

int16 rsi_uart_flow_control(uint8 enable);
/* Socket configuration */
int16 rsi_socket_config(rsi_socket_config_t *socket_config);
/* Trigger Auto configuration */
int16 rsi_trigger_auto_config(void);
/* HTTP Server Credentials */
int16 rsi_http_credentials(rsi_uhttpCredentials  *uhttpCredentialsFrame);

//! FTP client
int16 rsi_ftp_client(uint8 type, rsi_ftp_client_t *ftp_client);
int16 rsi_ftp_file_write_content(rsi_ftp_file_write_t *ftp_file_write, uint8 *file_content, uint32 file_size, uint8 ip_version);

//! SNTP client
int16 rsi_sntp_client(uint8 type, rsi_sntp_client_t *sntp_client);

//! MDNS 
int16 rsi_mdns_req(uint8 type, rsi_mdns_t *mdns);
uint8 rsi_irq_status(void);

//! SNMP 
uint16 rsi_build_snmp_buffer(uint8 *snmp_buf, char *obj_string, uint32 data_type, uint32 data_msw, uint32 data_lsw,char *data_string);
int  rsi_snmp_trap_init(uint8 *buffer);

//! SMTP
int16 rsi_smtp_client(uint8 type, rsi_smtp_client_t *smtp_client);
void  rsi_buil_smtp_header(uint8 *smtp_header);

//! POP3 
int16 rsi_pop3_client(uint8 type, rsi_pop3_client_t *pop3_client);

//! OTAF
int16 rsi_ota_fw_up(rsi_uOtafReq  *uOtafReqFrame);

//! DNS update
int16 rsi_dns_update(rsi_uDnsUpdate  *uDnsUpdateFrame);

//! HTTP Client PUT
int16 rsi_http_client_put(uint8 type, rsi_http_client_put_req_t *http_client_put);

//! RTC time from host
int16 rsi_host_rtc_time(module_rtc_time_t *rtc_time);
int16 rsi_dhcp_client_usr_cls(rsi_dhcp_user_class_t *dhcp_usr_cls);
int16 rsi_timeout(rsi_utimeout *uTimeoutFrame);
#endif

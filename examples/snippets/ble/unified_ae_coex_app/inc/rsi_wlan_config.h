#ifndef SAPIS_EXAMPLES_RSI_DEMO_APPS_INC_RSI_WLAN_CONFIG_H_
#define SAPIS_EXAMPLES_RSI_DEMO_APPS_INC_RSI_WLAN_CONFIG_H_

#include "rsi_common_app.h"

#if BLE_PERIPHERAL
#include "rsi_wlan_config_DEMO_1.h"
#elif BLE_CENTRAL
#include <rsi_wlan_config_DEMO_2.h>
#elif BLE_DUAL_ROLE
#include <rsi_wlan_config_DEMO_3.h>
#elif BLE_POWERSAVE
#include <rsi_wlan_config_DEMO_4.h>
#elif (BT_A2DP_SOURCE)
#include <rsi_wlan_config_DEMO_5.h>
#elif BT_EIR_SNIFF_SPP_PERIPHERAL
#include <rsi_wlan_config_DEMO_7.h>
#elif BLE_ADV_BT_SPP_THROUGHPUT
#include <rsi_wlan_config_DEMO_8.h>
#elif WIFI_SSL_RX_THROUGHPUT
#include "rsi_wlan_config_DEMO_9.h"
#elif WIFI_TCP_BI_DIR
#include <rsi_wlan_config_DEMO_10.h>
#elif WIFI_SSL_TX_THROUGHPUT
#include <rsi_wlan_config_DEMO_11.h>
#elif WIFI_TCP_RX_POWERSAVE
#include "rsi_wlan_config_DEMO_12.h"
#elif WIFI_CONNECTED_STANDBY
#include "rsi_wlan_config_DEMO_13.h"
#elif WIFI_STA_BGSCAN
#include "rsi_wlan_config_DEMO_14.h"
#elif WIFI_STA_CONN_DISCONN_SSL_CONN_DISCONN
#include "rsi_wlan_config_DEMO_15.h"
#elif WIFI_TCP_TX_POWERSAVE
#include "rsi_wlan_config_DEMO_16.h"
#elif WIFI_DEEPSLEEP_STANDBY
#include <rsi_wlan_config_DEMO_17.h>
#elif BT_A2DP_SOURCE_WIFI_HTTP_S_RX
#include <rsi_wlan_config_DEMO_18.h>
#elif WIFI_HTTP_S_5MB_RX_WITH_STANDBY
#include "rsi_wlan_config_DEMO_19.h"
#elif WIFI_ANT_HTTP_S_5MB_RX_WITH_STANDBY
#include "rsi_wlan_http_s_ant_config.h"
#elif BLE_DUAL_MODE_BT_A2DP_SOURCE_WIFI_HTTP_S_RX
#include "rsi_wlan_config_DEMO_20.h"
#elif BLE_DUAL_MODE_BT_SPP_PERIPHERAL
#include "rsi_wlan_config_DEMO_21.h"
#elif FIRMWARE_UPGRADE
#include "rsi_wlan_config_DEMO_22.h"
#elif BLE_ADV
#include "rsi_wlan_config_DEMO_23.h"
#elif BLE_IDLE_POWERSAVE
#include "rsi_wlan_config_DEMO_24.h"
#elif BLE_SCAN
#include "rsi_wlan_config_DEMO_25.h"
#elif BT_SNIFF
#include "rsi_wlan_config_DEMO_26.h"
#elif BT_PAGE_INQUIRY
#include "rsi_wlan_config_DEMO_27.h"
#elif BT_PER
#include "rsi_wlan_config_DEMO_28.h"
#elif BLE_PER
#include "rsi_wlan_config_DEMO_31.h"
#elif BT_SPP_RX_TX
#include "rsi_wlan_config_DEMO_29.h"
#elif BT_BLE_IDLE_POWERSAVE
#include "rsi_wlan_config_DEMO_30.h"
#elif BT_INQUIRY_SCAN
#include "rsi_wlan_config_DEMO_32.h"
#elif BT_PAGE_SCAN
#include "rsi_wlan_config_DEMO_33.h"
#elif BT_SPP_CENTRAL_SNIFF
#include "rsi_wlan_config_DEMO_34.h"
#elif BT_SPP_PERIPHERAL_SNIFF
#include "rsi_wlan_config_DEMO_35.h"
#elif WIFI_SSL_SERVER_CERT_BYPASS
#include "rsi_wlan_config_DEMO_36.h"
#elif BLE_DATA_TRANSFER
#include "rsi_wlan_config_DEMO_37.h"
#elif BLE_PRIVACY
#include "rsi_wlan_config_DEMO_38.h"
#elif BLE_LONG_READ
#include "rsi_wlan_config_DEMO_39.h"
#elif BLE_L2CAP_CBFC
#include "rsi_wlan_config_DEMO_40.h"
#elif BLE_GATT_TEST_ASYNC
#include "rsi_wlan_config_DEMO_41.h"
#elif BLE_GATT_INDICATION_STATUS
#include "rsi_wlan_config_DEMO_55.h"
#elif (BT_A2DP_SOURCE_SBC_CODEC)
#include "rsi_wlan_config_DEMO_42.h"
#elif FIRMWARE_UPGRADE_VIA_BOOTLOADER
#include "rsi_wlan_config_DEMO_43.h"
#elif RAM_DUMP_ASSERTION_CASE
#include "rsi_wlan_config_DEMO_65.h"
#elif BLE_SIMPLE_GATT
#include "rsi_wlan_config_DEMO_44.h"
#elif BT_A2DP_SOURCE_WIFI_HTTP_S_RX_DYN_COEX
#include <rsi_wlan_config_DEMO_45.h>
#elif WIFI_SOCKET_SELECT
#include "rsi_wlan_config_DEMO_46.h"
#elif WIFI_HTTP_SOCKET_SELECT
#include "rsi_wlan_config_DEMO_58.h"
#elif BLE_MULTI_PERIPHERAL_CENTRAL
#include "rsi_wlan_config_DEMO_47.h"
#elif WIFI_TCP_DNS_APP
#include "rsi_wlan_config_DEMO_49.h"
#elif WIFI_TCP_TX_APP
#include "rsi_wlan_config_DEMO_60.h"
#elif WIFI_TCP_WLAN_GET_APP
#include "rsi_wlan_config_DEMO_59.h"
#elif BLE_INDICATION
#include "rsi_wlan_config_DEMO_50.h"
#elif BLE_MULTI_PERIPHERAL_CENTRAL_BT_A2DP
#include "rsi_wlan_config_DEMO_51.h"
#elif BLE_MULTI_PERIPHERAL_CENTRAL_BT_A2DP_WIFI_HTTP_S_RX
#include "rsi_wlan_config_DEMO_52.h"
#elif BLE_OVERLAPPING_PROCEDURES
#include "rsi_wlan_config_DEMO_53.h"
#elif WIFI_STA_SCAN_CONN_DISCONN
#include "rsi_wlan_config_DEMO_56.h"
#elif UNIFIED_PROTOCOL
#include "rsi_wlan_config_DEMO_54.h"
#elif COEX_MAX_APP
#include "rsi_wlan_config_DEMO_57.h"
#elif ANT_APP
#include "rsi_wlan_config_ANT.h"
#elif ANT_APP_POWERSAVE
#include "rsi_wlan_config_ANT_APP_PWRSAVE.h"
#elif BLE_CENTRAL_ANT_APP
#include "rsi_wlan_config_BLE_CENTRAL_ANT_APP.h"
#elif BLE_PERIPHERAL_ANT_APP
#include "rsi_wlan_config_BLE_PERIPHERAL_ANT_APP.h"
#elif ANT_BLE_PERI_BT_A2DP_SRC_APP
#include "rsi_wlan_config_ANT_BLE_PERI_A2DP_SRC.h"
#elif ANT_BLE_CENT_BT_A2DP_SRC_APP
#include "rsi_wlan_config_ANT_BLE_CENT_A2DP_SRC.h"
#elif ANT_BLE_DUAL_ROLE_BT_A2DP_SRC_APP
#include "rsi_wlan_config_ANT_BLE_DUAL_ROLE_BT_A2DP_SRC.h"
#elif BT_A2DP_SOURCE_SBC_ENC_IN_RS9116
#include "rsi_wlan_config_DEMO_48.h"
#elif BT_A2DP_SOURCE_WIFI_SELECT
#include <rsi_wlan_config_DEMO_18.h>
#elif WLAN_STANDBY_WITH_ANT
#include "rsi_wlan_ant_config.h"
#elif BT_CW_MODE
#include "rsi_wlan_config_DEMO_61.h"
#elif BLE_CW_MODE
#include "rsi_wlan_config_DEMO_62.h"
#elif ANT_CW_MODE
#include "rsi_wlan_config_DEMO_66.h"
#elif COEX_THROUGHPUT
#include "rsi_wlan_config_DEMO_67.h"
#elif BT_BLE_UPDATE_GAIN_TABLE
#include "rsi_wlan_config_DEMO_68.h"
#else
/***********************************************************************************************************************************************/
//! WLAN SAPI CONFIG DEFINES
/***********************************************************************************************************************************************/
//! Enable feature
#define RSI_ENABLE                 1

//! Disable feature
#define RSI_DISABLE                0

//! To enable concurrent mode
#define CONCURRENT_MODE            RSI_DISABLE

//! opermode command paramaters
/*=======================================================================*/
//! To set wlan feature select bit map
#define RSI_FEATURE_BIT_MAP        (FEAT_SECURITY_OPEN)

//! TCP IP BYPASS feature check
#define RSI_TCP_IP_BYPASS          RSI_DISABLE

//! TCP/IP feature select bitmap for selecting TCP/IP features
#define RSI_TCP_IP_FEATURE_BIT_MAP (TCP_IP_FEAT_DHCPV4_CLIENT)

#define RSI_EXT_TCPIP_FEATURE_BITMAP 0

#define RSI_BT_FEATURE_BITMAP      0
//! To set custom feature select bit map
#define RSI_CUSTOM_FEATURE_BIT_MAP FEAT_CUSTOM_FEAT_EXTENTION_VALID

//! To set Extended custom feature select bit map
#if ENABLE_1P8V
#ifdef RSI_M4_INTERFACE
#define RSI_EXT_CUSTOM_FEATURE_BIT_MAP (EXT_FEAT_1P8V_SUPPORT | MEMORY_CONFIG)
#else
#define RSI_EXT_CUSTOM_FEATURE_BIT_MAP (EXT_FEAT_384K_MODE | EXT_FEAT_1P8V_SUPPORT)
#endif
#else
#ifdef RSI_M4_INTERFACE
#define RSI_EXT_CUSTOM_FEATURE_BIT_MAP (MEMORY_CONFIG)
#else
#define RSI_EXT_CUSTOM_FEATURE_BIT_MAP (EXT_FEAT_384K_MODE)
#endif
#endif
/*=======================================================================*/

//! Band command paramters
/*=======================================================================*/

//! RSI_BAND_2P4GHZ(2.4GHz) or RSI_BAND_5GHZ(5GHz) or RSI_DUAL_BAND
#define RSI_BAND RSI_BAND_2P4GHZ //! RSI_BAND_5GHZ or RSI_BAND_2P4GHZ

//! Feature frame parameters
/*=======================================================================*/
#define PLL_MODE 0
#define RF_TYPE \
  1 //! 0 - External RF 1- Internal RF(use if not using external antenna) //! applicable only is feature_frame is used
#define WIRELESS_MODE                      0
#define ENABLE_PPP                         0
#define AFE_TYPE                           1
#define FEATURE_ENABLES                    0
/*=======================================================================*/

//! set region command paramters
/*=======================================================================*/

//! RSI_ENABLE or RSI_DISABLE Set region support
#define RSI_SET_REGION_SUPPORT             RSI_DISABLE //@ RSI_ENABLE or RSI_DISABLE set region

//! If 1:region configurations taken from user ;0:region configurations taken from beacon
#define RSI_SET_REGION_FROM_USER_OR_BEACON 1

//! 0-Default Region domain ,1-US, 2-EUROPE, 3-JAPAN
#define RSI_REGION_CODE                    1

//! 0- Without On Board Antenna , 1- With On Board Antenna
#define RSI_MODULE_TYPE                    1

/*=======================================================================*/

//! set region AP command paramters
/*=======================================================================*/

//! RSI_ENABLE or RSI_DISABLE Set region AP support
#define RSI_SET_REGION_AP_SUPPORT          RSI_DISABLE

//! If 1:region configurations taken from user ;0:region configurations taken from firmware
#define RSI_SET_REGION_AP_FROM_USER        RSI_DISABLE

//! "US" or "EU" or "JP" or other region codes
#define RSI_COUNTRY_CODE                   "US "

/*=======================================================================*/

//! Rejoin parameters
/*=======================================================================*/

//! RSI_ENABLE or RSI_DISABLE rejoin params
#define RSI_REJOIN_PARAMS_SUPPORT          RSI_DISABLE

//! Rejoin retry count. If 0 retries infinity times
#define RSI_REJOIN_MAX_RETRY               0

//! Periodicity of rejoin attempt
#define RSI_REJOIN_SCAN_INTERVAL           4

//! Beacon missed count
#define RSI_REJOIN_BEACON_MISSED_COUNT     40

//! RSI_ENABLE or RSI_DISABLE retry for first time join failure
#define RSI_REJOIN_FIRST_TIME_RETRY        RSI_DISABLE

/*=======================================================================*/

//!BG scan command parameters
/*=======================================================================*/

//! RSI_ENABLE or RSI_DISABLE BG Scan support
#define RSI_BG_SCAN_SUPPORT                RSI_DISABLE

//! RSI_ENABLE or RSI_DISABLE BG scan
#define RSI_BG_SCAN_ENABLE                 RSI_ENABLE

//! RSI_ENABLE or RSI_DISABLE instant BG scan
#define RSI_INSTANT_BG                     RSI_DISABLE

//! BG scan threshold value
#define RSI_BG_SCAN_THRESHOLD              10

//! RSSI tolerance Threshold
#define RSI_RSSI_TOLERANCE_THRESHOLD       4

//! BG scan periodicity
#define RSI_BG_SCAN_PERIODICITY            10

//! Active scan duration
#define RSI_ACTIVE_SCAN_DURATION           15

//! Passive scan duration
#define RSI_PASSIVE_SCAN_DURATION          20

//! Multi probe
#define RSI_MULTIPROBE                     RSI_DISABLE

/*=======================================================================*/

//!Roaming parameters
/*=======================================================================*/

//! RSI_ENABLE or RSI_DISABLE Roaming support
#define RSI_ROAMING_SUPPORT                RSI_DISABLE

//! roaming threshold value
#define RSI_ROAMING_THRESHOLD              10

//! roaming hysterisis value
#define RSI_ROAMING_HYSTERISIS             4

/*=======================================================================*/

//! High Throughput Capabilies related information
/*=======================================================================*/

//! RSI_ENABLE or RSI_DISABLE 11n mode in AP mode
#define RSI_MODE_11N_ENABLE                RSI_DISABLE

//! HT caps bit map.
#define RSI_HT_CAPS_BIT_MAP                2

/*=======================================================================*/

//! Scan command parameters
/*=======================================================================*/

//! scan channel bit map in 2.4GHz band,valid if given channel to scan is 0
#define RSI_SCAN_CHANNEL_BIT_MAP_2_4       0

//! scan channle bit map in 5GHz band ,valid if given channel to scan is 0
#define RSI_SCAN_CHANNEL_BIT_MAP_5         0

//! scan_feature_bitmap ,valid only if specific channel to scan and ssid are given
#define RSI_SCAN_FEAT_BITMAP               RSI_ENABLE_QUICK_SCAN

/*=======================================================================*/

//! Enterprise configuration command parameters
/*=======================================================================*/

//! Enterprise method ,should be one of among TLS, TTLS, FAST or PEAP
#define RSI_EAP_METHOD                     "TTLS"
//! This parameter is used to configure the module in Enterprise security mode
#define RSI_EAP_INNER_METHOD               "\"auth=MSCHAPV2\""
//! Private Key Password is required for encrypted private key, format is like "\"12345678\""
#define RSI_PRIVATE_KEY_PASSWORD           ""

/*=======================================================================*/

//! AP configuration command parameters
/*=======================================================================*/

//! This Macro is used to enable AP keep alive functionality
#define RSI_AP_KEEP_ALIVE_ENABLE           RSI_ENABLE

//! This parameter is used to configure keep alive type
#define RSI_AP_KEEP_ALIVE_TYPE             RSI_NULL_BASED_KEEP_ALIVE

//! This parameter is used to configure keep alive period
#define RSI_AP_KEEP_ALIVE_PERIOD           100

//! This parameter is used to configure maximum stations supported
#define RSI_MAX_STATIONS_SUPPORT           4
/*=======================================================================*/

//! Join command parameters
/*=======================================================================*/

//! Tx power level
#define RSI_POWER_LEVEL                    RSI_POWER_LEVEL_HIGH

//! RSI_JOIN_FEAT_STA_BG_ONLY_MODE_ENABLE or RSI_JOIN_FEAT_LISTEN_INTERVAL_VALID
#define RSI_JOIN_FEAT_BIT_MAP              0

//!
#define RSI_LISTEN_INTERVAL                0

//! Transmission data rate. Physical rate at which data has to be transmitted.
#define RSI_DATA_RATE                      RSI_DATA_RATE_AUTO

/*=======================================================================*/

//! Ipconf command parameters
/*=======================================================================*/

//! DHCP client host name
#define RSI_DHCP_HOST_NAME                 "dhcp_client"

//! Transmit test command parameters
/*=======================================================================*/
//! TX TEST rate flags
#define RSI_TX_TEST_RATE_FLAGS             0

//! TX TEST per channel bandwidth
#define RSI_TX_TEST_PER_CH_BW              0

//! TX TEST aggregation enable or disable
#define RSI_TX_TEST_AGGR_ENABLE            RSI_DISABLE

//! TX TEST delay
#define RSI_TX_TEST_DELAY                  0

/*=======================================================================*/

//! ssl parameters
/*=======================================================================*/
//! ssl version
#define RSI_SSL_VERSION                    0

//! ssl ciphers
#define RSI_SSL_CIPHERS                    SSL_ALL_CIPHERS

//! Enable TCP over SSL with TLS version depends on remote side
#define PROTOCOL_DFLT_VERSION              BIT(0)

//! Enable TCP over SSL with TLS version 1.0
#define PROTOCOL_TLS_1_0                   (BIT(0) | BIT(13))

//! Enable TCP over SSL with TLS version 1.1
#define PROTOCOL_TLS_1_1                   (BIT(0) | BIT(14))

//! Enable TCP over SSL with TLS version 1.2
#define PROTOCOL_TLS_1_2                   (BIT(0) | BIT(15))
/*=======================================================================*/

//! Power save command parameters
/*=======================================================================*/
//! set handshake type of power mode
#define RSI_HAND_SHAKE_TYPE                GPIO_BASED

//! 0 - LP, 1- ULP mode with RAM retention and 2 - ULP with Non RAM retention
#define RSI_SELECT_LP_OR_ULP_MODE          RSI_ULP_WITH_RAM_RET

//! set DTIM aligment required
//! 0 - module wakes up at beacon which is just before or equal to listen_interval
//! 1 - module wakes up at DTIM beacon which is just before or equal to listen_interval
#define RSI_DTIM_ALIGNED_TYPE              0

//! Monitor interval for the FAST PSP mode
//! default is 50 ms, and this parameter is valid for FAST PSP only
#define RSI_MONITOR_INTERVAL               50

//! Number of DTIMs to skip during powersave
#define RSI_NUM_OF_DTIM_SKIP               0

//!WMM PS parameters
#if UAPSD_POWER_SAVE
//! set wmm enable or disable
#define RSI_WMM_PS_ENABLE        RSI_ENABLE

//! set wmm enable or disable
//! 0- TX BASED 1 - PERIODIC
#define RSI_WMM_PS_TYPE          1

//! set wmm wake up interval
#define RSI_WMM_PS_WAKE_INTERVAL 30

//! set wmm UAPSD bitmap
#define RSI_WMM_PS_UAPSD_BITMAP  15
#else
//! set wmm enable or disable
#define RSI_WMM_PS_ENABLE        RSI_DISABLE

//! set wmm enable or disable
//! 0- TX BASED 1 - PERIODIC
#define RSI_WMM_PS_TYPE          0

//! set wmm wake up interval
#define RSI_WMM_PS_WAKE_INTERVAL 20

//! set wmm UAPSD bitmap
#define RSI_WMM_PS_UAPSD_BITMAP  15
#endif
/*=======================================================================*/

//! Socket configuration
/*=======================================================================*/
//! RSI_ENABLE or RSI_DISABLE High performance socket
#define HIGH_PERFORMANCE_ENABLE RSI_ENABLE //@ RSI_ENABLE or RSI_DISABLE High performance socket

#define TOTAL_SOCKETS                     1  //@ Total number of sockets. TCP TX + TCP RX + UDP TX + UDP RX
#define TOTAL_TCP_SOCKETS                 1  //@ Total TCP sockets. TCP TX + TCP RX
#define TOTAL_UDP_SOCKETS                 0  //@ Total UDP sockets. UDP TX + UDP RX
#define TCP_TX_ONLY_SOCKETS               0  //@ Total TCP TX only sockets. TCP TX
#define TCP_RX_ONLY_SOCKETS               1  //@ Total TCP RX only sockets. TCP RX
#define UDP_TX_ONLY_SOCKETS               0  //@ Total UDP TX only sockets. UDP TX
#define UDP_RX_ONLY_SOCKETS               0  //@ Total UDP RX only sockets. UDP RX
#define TCP_RX_HIGH_PERFORMANCE_SOCKETS   1  //@ Total TCP RX High Performance sockets
#define TCP_RX_WINDOW_SIZE_CAP            44 //@ TCP RX Window size - 64K (44 * 1460)
#define TCP_RX_WINDOW_DIV_FACTOR          44
/*=======================================================================*/

//! Socket Create parameters
/*=======================================================================*/

//! Initial timeout for Socket
#define RSI_SOCKET_KEEPALIVE_TIMEOUT      1200

//! VAP ID for Concurrent mode
#define RSI_VAP_ID                        0

//! Timeout for join or scan
/*=======================================================================*/

//!RSI_ENABLE or RSI_DISABLE Timeout support
#define RSI_TIMEOUT_SUPPORT               RSI_DISABLE

//! roaming threshold value
#define RSI_TIMEOUT_BIT_MAP               1

//! roaming hysterisis value
#define RSI_TIMEOUT_VALUE                 1500

//! Timeout for ping request
/*=======================================================================*/

//!Timeout for PING_REQUEST
#define RSI_PING_REQ_TIMEOUT              1

//! Store Config Profile parameters
/*=======================================================================*/

//! Client profile
#define RSI_WLAN_PROFILE_CLIENT           0
//! P2P profile
#define RSI_WLAN_PROFILE_P2P              1
//! EAP profile
#define RSI_WLAN_PROFILE_EAP              2
//! AP profile
#define RSI_WLAN_PROFILE_AP               6
//! All profiles
#define RSI_WLAN_PROFILE_ALL              0xFF

//! AP Config Profile Parameters
/*==============================================================================*/

//! Transmission data rate. Physical rate at which data has to be transmitted.
#define RSI_CONFIG_AP_DATA_RATE           RSI_DATA_RATE_AUTO
//! To set wlan feature select bit map
#define RSI_CONFIG_AP_WLAN_FEAT_BIT_MAP   (FEAT_SECURITY_PSK)
//! TCP/IP feature select bitmap for selecting TCP/IP features
#define RSI_CONFIG_AP_TCP_IP_FEAT_BIT_MAP (TCP_IP_FEAT_DHCPV4_SERVER)
//! To set custom feature select bit map
#define RSI_CONFIG_AP_CUSTOM_FEAT_BIT_MAP 0
//! RSI_BAND_2P4GHZ(2.4GHz) or RSI_BAND_5GHZ(5GHz) or RSI_DUAL_BAND
#define RSI_CONFIG_AP_BAND                RSI_BAND_2P4GHZ
//! Tx power level
#define RSI_CONFIG_AP_TX_POWER            RSI_POWER_LEVEL_HIGH
//! AP SSID
#define RSI_CONFIG_AP_SSID                "REDPINE_AP"
//! RSI_BAND_2P4GHZ(2.4GHz) or RSI_BAND_5GHZ(5GHz) or RSI_DUAL_BAND
#define RSI_CONFIG_AP_BAND                RSI_BAND_2P4GHZ
//! To configure AP channle number
#define RSI_CONFIG_AP_CHANNEL             6
//! To configure security type
#define RSI_CONFIG_AP_SECURITY_TYPE       RSI_WPA
//! To configure encryption type
#define RSI_CONFIG_AP_ENCRYPTION_TYPE     1
//! To configure PSK
#define RSI_CONFIG_AP_PSK                 "1234567890"
//! To configure beacon interval
#define RSI_CONFIG_AP_BEACON_INTERVAL     100
//! To configure DTIM period
#define RSI_CONFIG_AP_DTIM                2
//! This parameter is used to configure keep alive type
#define RSI_CONFIG_AP_KEEP_ALIVE_TYPE     0 //! RSI_NULL_BASED_KEEP_ALIVE

#define RSI_CONFIG_AP_KEEP_ALIVE_COUNTER           0 //! 100
//! This parameter is used to configure keep alive period
#define RSI_CONFIG_AP_KEEP_ALIVE_PERIOD            100
//! This parameter is used to configure maximum stations supported
#define RSI_CONFIG_AP_MAX_STATIONS_COUNT           4
//! P2P Network parameters
//! TCP_STACK_USED BIT(0) - IPv4, BIT(1) -IPv6, (BIT(0) | BIT(1)) - Both IPv4 and IPv6
#define RSI_CONFIG_AP_TCP_STACK_USED               BIT(0)
//! IP address of the module
//! E.g: 0x0A0AA8C0 == 192.168.10.10
#define RSI_CONFIG_AP_IP_ADDRESS                   0x0A0AA8C0
//! IP address of netmask
//! E.g: 0x00FFFFFF == 255.255.255.0
#define RSI_CONFIG_AP_SN_MASK_ADDRESS              0x00FFFFFF
//! IP address of Gateway
//! E.g: 0x0A0AA8C0 == 192.168.10.10
#define RSI_CONFIG_AP_GATEWAY_ADDRESS              0x0A0AA8C0

//! Client Profile Parameters
/* ===================================================================================== */

//! To configure data rate
#define RSI_CONFIG_CLIENT_DATA_RATE                RSI_DATA_RATE_AUTO
//! To configure wlan feature bitmap
#define RSI_CONFIG_CLIENT_WLAN_FEAT_BIT_MAP        0
//! To configure tcp/ip feature bitmap
#define RSI_CONFIG_CLIENT_TCP_IP_FEAT_BIT_MAP      BIT(2)
//! To configure custom feature bit map
#define RSI_CONFIG_CLIENT_CUSTOM_FEAT_BIT_MAP      0
//! To configure TX power
#define RSI_CONFIG_CLIENT_TX_POWER                 RSI_POWER_LEVEL_HIGH
//! To configure listen interval
#define RSI_CONFIG_CLIENT_LISTEN_INTERVAL          0
//! To configure SSID
#define RSI_CONFIG_CLIENT_SSID                     "REDPINE_AP"
//! RSI_BAND_2P4GHZ(2.4GHz) or RSI_BAND_5GHZ(5GHz) or RSI_DUAL_BAND
#define RSI_CONFIG_CLIENT_BAND                     RSI_BAND_2P4GHZ
//! To configure channel number
#define RSI_CONFIG_CLIENT_CHANNEL                  0
//! To configure security type
#define RSI_CONFIG_CLIENT_SECURITY_TYPE            RSI_WPA
//! To configure encryption type
#define RSI_CONFIG_CLIENT_ENCRYPTION_TYPE          0
//! To configure PSK
#define RSI_CONFIG_CLIENT_PSK                      "1234567890"
//! To configure PMK
#define RSI_CONFIG_CLIENT_PMK                      ""
//! Client Network parameters
//! TCP_STACK_USED BIT(0) - IPv4, BIT(1) -IPv6, (BIT(0) | BIT(1)) - Both IPv4 and IPv6
#define RSI_CONFIG_CLIENT_TCP_STACK_USED           BIT(0)
//! DHCP mode 1- Enable 0- Disable
//! If DHCP mode is disabled given IP statically
#define RSI_CONFIG_CLIENT_DHCP_MODE                RSI_DHCP
//! IP address of the module
//! E.g: 0x0A0AA8C0 == 192.168.10.10
#define RSI_CONFIG_CLIENT_IP_ADDRESS               0x0A0AA8C0
//! IP address of netmask
//! E.g: 0x00FFFFFF == 255.255.255.0
#define RSI_CONFIG_CLIENT_SN_MASK_ADDRESS          0x00FFFFFF
//! IP address of Gateway
//! E.g: 0x010AA8C0 == 192.168.10.1
#define RSI_CONFIG_CLIENT_GATEWAY_ADDRESS          0x010AA8C0
//! scan channel bit map in 2.4GHz band,valid if given channel to scan is 0
#define RSI_CONFIG_CLIENT_SCAN_FEAT_BITMAP         0
//! Scan channel magic code
#define RSI_CONFIG_CLIENT_MAGIC_CODE               0x4321
//! scan channel bit map in 2.4GHz band,valid if given channel to scan is 0
#define RSI_CONFIG_CLIENT_SCAN_CHAN_BITMAP_2_4_GHZ 0
//! scan channle bit map in 5GHz band ,valid if given channel to scan is 0
#define RSI_CONFIG_CLIENT_SCAN_CHAN_BITMAP_5_0_GHZ 0

//! EAP Client Profile Parameters
/* =========================================================================== */

//! To configure data rate
#define RSI_CONFIG_EAP_DATA_RATE                   RSI_DATA_RATE_AUTO
//! To configure wlan feature bitmap
#define RSI_CONFIG_EAP_WLAN_FEAT_BIT_MAP           0
//! To configure tcp/ip feature bitmap
#define RSI_CONFIG_EAP_TCP_IP_FEAT_BIT_MAP         BIT(2)
//! To configure custom feature bit map
#define RSI_CONFIG_EAP_CUSTOM_FEAT_BIT_MAP         0
//! To configure EAP TX power
#define RSI_CONFIG_EAP_TX_POWER                    RSI_POWER_LEVEL_HIGH
//! To Configure scan channel feature bitmap
#define RSI_CONFIG_EAP_SCAN_FEAT_BITMAP            0
//! scan channel bit map in 2.4GHz band,valid if given channel to scan is 0
#define RSI_CONFIG_EAP_CHAN_MAGIC_CODE             0 //0x4321
//! scan channel bit map in 2.4GHz band,valid if given channel to scan is 0
#define RSI_CONFIG_EAP_SCAN_CHAN_BITMAP_2_4_GHZ    0
//! scan channle bit map in 5GHz band ,valid if given channel to scan is 0
#define RSI_CONFIG_EAP_SCAN_CHAN_BITMAP_5_0_GHZ    0
//! To configure SSID
#define RSI_CONFIG_EAP_SSID                        "REDPINE_AP"
//! RSI_BAND_2P4GHZ(2.4GHz) or RSI_BAND_5GHZ(5GHz) or RSI_DUAL_BAND
#define RSI_CONFIG_EAP_BAND                        RSI_BAND_2P4GHZ
//! To set security type
#define RSI_CONFIG_EAP_SECURITY_TYPE               RSI_WPA2_EAP
//! To set encryption type
#define RSI_CONFIG_EAP_ENCRYPTION_TYPE             0
//! To set channel number
#define RSI_CONFIG_EAP_CHANNEL                     0
//! Enterprise method ,should be one of among TLS, TTLS, FAST or PEAP
#define RSI_CONFIG_EAP_METHOD                      "TLS"
//! This parameter is used to configure the module in Enterprise security mode
#define RSI_CONFIG_EAP_INNER_METHOD                "\"auth=MSCHAPV2\""
//! To configure EAP user identity
#define RSI_CONFIG_EAP_USER_IDENTITY               "\"user1\""
//! TO configure EAP password
#define RSI_CONFIG_EAP_PASSWORD                    "\"test123\""
//! EAP Network parameters
//! TCP_STACK_USED BIT(0) - IPv4, BIT(1) -IPv6, (BIT(0) | BIT(1)) - Both IPv4 and IPv6
#define RSI_CONFIG_EAP_TCP_STACK_USED              BIT(0)
//! DHCP mode 1- Enable 0- Disable
//! If DHCP mode is disabled given IP statically
#define RSI_CONFIG_EAP_DHCP_MODE                   RSI_DHCP
//! IP address of the module
//! E.g: 0x0A0AA8C0 == 192.168.10.10
#define RSI_CONFIG_EAP_IP_ADDRESS                  0x0A0AA8C0
//! IP address of netmask
//! E.g: 0x00FFFFFF == 255.255.255.0
#define RSI_CONFIG_EAP_SN_MASK_ADDRESS             0x00FFFFFF
//! IP address of Gateway
//! E.g: 0x010AA8C0 == 192.168.10.1
#define RSI_CONFIG_EAP_GATEWAY_ADDRESS             0x010AA8C0

//! P2P Profile parameters
/* ================================================================================= */

//! To configure data rate
#define RSI_CONFIG_P2P_DATA_RATE                   RSI_DATA_RATE_AUTO
//! To configure wlan feature bitmap
#define RSI_CONFIG_P2P_WLAN_FEAT_BIT_MAP           0
//! To configure P2P tcp/ip feature  bitmap
#define RSI_CONFIG_P2P_TCP_IP_FEAT_BIT_MAP         BIT(2)
//! To configure P2P custom feature bitmap
#define RSI_CONFIG_P2P_CUSTOM_FEAT_BIT_MAP         0
//! TO configure P2P tx power level
#define RSI_CONFIG_P2P_TX_POWER                    RSI_POWER_LEVEL_HIGH
//! Set P2P go intent
#define RSI_CONFIG_P2P_GO_INTNET                   16 //! Support only Autonomous GO mode
//! Set device name
#define RSI_CONFIG_P2P_DEVICE_NAME                 "WSC1.1"
//! Set device operating channel
#define RSI_CONFIG_P2P_OPERATING_CHANNEL           11
//! Set SSID postfix
#define RSI_CONFIG_P2P_SSID_POSTFIX                "WSC_1_0_0"
//! Set P2P join SSID
#define RSI_CONFIG_P2P_JOIN_SSID                   "REDPINE_AP"
//! Set psk key
#define RSI_CONFIG_P2P_PSK_KEY                     "12345678"
//! P2P Network parameters
//! TCP_STACK_USED BIT(0) - IPv4, BIT(1) -IPv6, (BIT(0) | BIT(1)) - Both IPv4 and IPv6
#define RSI_CONFIG_P2P_TCP_STACK_USED              BIT(0)
//! DHCP mode 1- Enable 0- Disable
//! If DHCP mode is disabled given IP statically
#define RSI_CONFIG_P2P_DHCP_MODE                   1
//! IP address of the module
//! E.g: 0x0A0AA8C0 == 192.168.10.10
#define RSI_CONFIG_P2P_IP_ADDRESS                  0x0A0AA8C0
//! IP address of netmask
//! E.g: 0x00FFFFFF == 255.255.255.0
#define RSI_CONFIG_P2P_SN_MASK_ADDRESS             0x00FFFFFF
//! IP address of Gateway
//! E.g: 0x010AA8C0 == 192.168.10.1
#define RSI_CONFIG_P2P_GATEWAY_ADDRESS             0x010AA8C0
/***********************************************************************************************************************************************/
#include "rsi_wlan_common_config.h"
#endif
#ifdef FW_LOGGING_ENABLE
/*=======================================================================*/
//! Firmware Logging Parameters
/*=======================================================================*/
//! Enable or Disable firmware logging (Enable = 1; Disable = 0)
#define FW_LOG_ENABLE 1
//! Set TSF Granularity for firmware logging in micro seconds
#define FW_TSF_GRANULARITY_US 10
//! Log level for COMMON component in firmware
#define COMMON_LOG_LEVEL FW_LOG_INFO //FW_LOG_ERROR
//! Log level for CM_PM component in firmware
#define CM_PM_LOG_LEVEL FW_LOG_INFO //FW_LOG_ERROR
//! Log level for WLAN_LMAC component in firmware
#define WLAN_LMAC_LOG_LEVEL FW_LOG_INFO //FW_LOG_ERROR
//! Log level for WLAN_UMAC component in firmware
#define WLAN_UMAC_LOG_LEVEL FW_LOG_INFO //FW_LOG_ERROR
//! Log level for WLAN NETWORK STACK component in firmware
#define WLAN_NETSTACK_LOG_LEVEL FW_LOG_ERROR
//! Log level for BT BLE CONTROL component in firmware
#define BT_BLE_CTRL_LOG_LEVEL FW_LOG_ERROR
//! Log level for BT BLE STACK component in firmware
#define BT_BLE_STACK_LOG_LEVEL FW_LOG_ERROR
//! Min Value = 2048 bytes; Max Value = 4096 bytes; Value should be in multiples of 512 bytes
#define FW_LOG_BUFFER_SIZE 2048
//! Set queue size for firmware log messages
#define FW_LOG_QUEUE_SIZE 2
#endif

#endif
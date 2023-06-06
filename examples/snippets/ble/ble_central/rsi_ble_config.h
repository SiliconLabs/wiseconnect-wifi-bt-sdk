/*******************************************************************************
* @file  rsi_ble_config.h
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

#ifndef RSI_BLE_CONFIG_H
#define RSI_BLE_CONFIG_H

#include <rsi_data_types.h>

/******************************************************
 * *                      Macros
 * ******************************************************/
//! Memory length for the driver
#define BT_GLOBAL_BUFF_LEN 15000

#define RSI_BLE_SET_RAND_ADDR "00:23:A7:12:34:56"

#define CLEAR_WHITELIST              0x00
#define ADD_DEVICE_TO_WHITELIST      0x01
#define DELETE_DEVICE_FROM_WHITELIST 0x02

#define ALL_PHYS 0x00

#define RSI_BLE_DEV_ADDR_RESOLUTION_ENABLE 0

#define RSI_OPERMODE_WLAN_BLE 13

#ifdef RSI_M4_INTERFACE
#define RSI_BLE_MAX_NBR_ATT_REC 20
#define RSI_BLE_MAX_NBR_SLAVES  1
#define RSI_BLE_NUM_CONN_EVENTS 2
#else
#define RSI_BLE_MAX_NBR_ATT_REC 80
#define RSI_BLE_MAX_NBR_SLAVES  3
#define RSI_BLE_NUM_CONN_EVENTS 20
#endif

#define RSI_BLE_MAX_NBR_ATT_SERV  10
#define RSI_BLE_MAX_NBR_MASTERS   1
#define RSI_BLE_GATT_ASYNC_ENABLE 0
#define RSI_BLE_GATT_INIT         0

/* Number of BLE GATT RECORD SIZE IN (n*16 BYTES), eg:(0x40*16)=1024 bytes */
#define RSI_BLE_NUM_REC_BYTES 0x40

#define RSI_BLE_INDICATE_CONFIRMATION_FROM_HOST 0

/*=======================================================================*/
//! Advertising command parameters
/*=======================================================================*/

#define RSI_BLE_ADV_TYPE          UNDIR_CONN
#define RSI_BLE_ADV_FILTER_TYPE   ALLOW_SCAN_REQ_ANY_CONN_REQ_ANY
#define RSI_BLE_ADV_DIR_ADDR_TYPE LE_PUBLIC_ADDRESS
#define RSI_BLE_ADV_DIR_ADDR      "00:15:83:6A:64:17"

#define RSI_BLE_ADV_INT_MIN     0x100
#define RSI_BLE_ADV_INT_MAX     0x200
#define RSI_BLE_ADV_CHANNEL_MAP 0x07

//!Advertise status
//! Start the advertising process
#define RSI_BLE_START_ADV 0x01
//! Stop the advertising process
#define RSI_BLE_STOP_ADV 0x00

//! BLE Tx Power Index On Air
#define RSI_BLE_PWR_INX 30

//! BLE Active H/w Pwr Features
#define BLE_DISABLE_DUTY_CYCLING 0
#define BLE_DUTY_CYCLING         1
#define BLR_DUTY_CYCLING         2
#define BLE_4X_PWR_SAVE_MODE     4
#define RSI_BLE_PWR_SAVE_OPTIONS BLE_DISABLE_DUTY_CYCLING

//!Advertise types

/* Advertising will be visible(discoverable) to all the devices.
 * Scanning/Connection is also accepted from all devices
 * */
#define UNDIR_CONN 0x80

/* Advertising will be visible(discoverable) to the particular device 
 * mentioned in RSI_BLE_ADV_DIR_ADDR only. 
 * Scanning and Connection will be accepted from that device only.
 * */
#define DIR_CONN 0x81

/* Advertising will be visible(discoverable) to all the devices.
 * Scanning will be accepted from all the devices.
 * Connection will be not be accepted from any device.
 * */
#define UNDIR_SCAN 0x82

/* Advertising will be visible(discoverable) to all the devices.
 * Scanning and Connection will not be accepted from any device
 * */
#define UNDIR_NON_CONN 0x83

/* Advertising will be visible(discoverable) to the particular device 
 * mentioned in RSI_BLE_ADV_DIR_ADDR only. 
 * Scanning and Connection will be accepted from that device only.
 * */
#define DIR_CONN_LOW_DUTY_CYCLE 0x84

//!Advertising flags
#define LE_LIMITED_DISCOVERABLE 0x01
#define LE_GENERAL_DISCOVERABLE 0x02
#define LE_BR_EDR_NOT_SUPPORTED 0x04

//!Advertise filters
#define ALLOW_SCAN_REQ_ANY_CONN_REQ_ANY               0x00
#define ALLOW_SCAN_REQ_WHITE_LIST_CONN_REQ_ANY        0x01
#define ALLOW_SCAN_REQ_ANY_CONN_REQ_WHITE_LIST        0x02
#define ALLOW_SCAN_REQ_WHITE_LIST_CONN_REQ_WHITE_LIST 0x03

//! Address types
#define LE_PUBLIC_ADDRESS            0x00
#define LE_RANDOM_ADDRESS            0x01
#define LE_RESOLVABLE_PUBLIC_ADDRESS 0x02
#define LE_RESOLVABLE_RANDOM_ADDRESS 0x03

/*=======================================================================*/

/*=======================================================================*/
//! Connection parameters
/*=======================================================================*/
#define LE_SCAN_INTERVAL 0x0100
#define LE_SCAN_WINDOW   0x0050

#define CONNECTION_INTERVAL_MIN 0x00A0
#define CONNECTION_INTERVAL_MAX 0x00A0

#define CONNECTION_LATENCY  0x0000
#define SUPERVISION_TIMEOUT 0x07D0 //2000

/*=======================================================================*/

/*=======================================================================*/
//! Scan command parameters
/*=======================================================================*/

#define RSI_BLE_SCAN_TYPE        SCAN_TYPE_ACTIVE
#define RSI_BLE_SCAN_FILTER_TYPE SCAN_FILTER_TYPE_ALL

//!Scan status
#define RSI_BLE_START_SCAN 0x01
#define RSI_BLE_STOP_SCAN  0x00

//!Scan types
#define SCAN_TYPE_ACTIVE  0x01
#define SCAN_TYPE_PASSIVE 0x00

//!Scan filters
#define SCAN_FILTER_TYPE_ALL             0x00
#define SCAN_FILTER_TYPE_ONLY_WHITE_LIST 0x01

#define RSI_SEL_INTERNAL_ANTENNA 0x00
#define RSI_SEL_EXTERNAL_ANTENNA 0x01

/*=======================================================================*/
//! Parameters for whitelisting of BLE advertise reports based on the ADV payload
/*=======================================================================*/
//#define RSI_BLE_ENABLE_WHITELIST_BASEDON_ADV_PAYLOAD     0x00
//#define RSI_BLE_ADV_PAYLOAD_INDEX_FOR_COMPARE            0x00
//#define RSI_BLE_ADV_PAYLOAD_LENGTH_FROM_INDEX_TO_COMPARE 0x00

/***********************************************************************************************************************************************/
//! RS9116 Firmware Configurations
/***********************************************************************************************************************************************/

/*=======================================================================*/
//! Opermode command parameters
/*=======================================================================*/

#define RSI_FEATURE_BIT_MAP \
  (FEAT_ULP_GPIO_BASED_HANDSHAKE | FEAT_DEV_TO_HOST_ULP_GPIO_1) //! To set wlan feature select bit map
#define RSI_TCP_IP_BYPASS RSI_DISABLE                           //! TCP IP BYPASS feature check
#define RSI_TCP_IP_FEATURE_BIT_MAP \
  (TCP_IP_FEAT_DHCPV4_CLIENT) //! TCP/IP feature select bitmap for selecting TCP/IP features
#define RSI_EXT_TCPIP_FEATURE_BITMAP 0

#define RSI_CUSTOM_FEATURE_BIT_MAP FEAT_CUSTOM_FEAT_EXTENTION_VALID //! To set custom feature select bit map

#ifdef CHIP_9117
#define RSI_EXT_CUSTOM_FEATURE_BIT_MAP \
  (EXT_FEAT_LOW_POWER_MODE | EXT_FEAT_XTAL_CLK_ENABLE | RAM_LEVEL_NWP_ADV_MCU_BASIC)
#else
#define RSI_EXT_CUSTOM_FEATURE_BIT_MAP (EXT_FEAT_LOW_POWER_MODE | EXT_FEAT_XTAL_CLK_ENABLE)
#endif

#define RSI_BT_FEATURE_BITMAP (BT_RF_TYPE | ENABLE_BLE_PROTOCOL)

/*=======================================================================*/
//! Power save command parameters
/*=======================================================================*/
//! set handshake type of power mode
#ifdef RSI_M4_INTERFACE
#define RSI_HAND_SHAKE_TYPE M4_BASED
#else
#define RSI_HAND_SHAKE_TYPE GPIO_BASED
#endif

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
#define COMMON_LOG_LEVEL FW_LOG_ERROR
//! Log level for CM_PM component in firmware
#define CM_PM_LOG_LEVEL FW_LOG_ERROR
//! Log level for WLAN_LMAC component in firmware
#define WLAN_LMAC_LOG_LEVEL FW_LOG_ERROR
//! Log level for WLAN_UMAC component in firmware
#define WLAN_UMAC_LOG_LEVEL FW_LOG_ERROR
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

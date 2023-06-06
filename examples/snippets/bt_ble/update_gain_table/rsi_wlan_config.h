/*******************************************************************************
* @file  rsi_wlan_config.h
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

#ifndef RSI_CONFIG_H
#define RSI_CONFIG_H

#include "rsi_wlan_defines.h"
/***********************************************************************************************************************************************/
//! WLAN SAPI CONFIG DEFINES
/***********************************************************************************************************************************************/
//! Enable feature
#define RSI_ENABLE 1
//! Disable feature
#define RSI_DISABLE 0
//! To enable concurrent mode
#define CONCURRENT_MODE RSI_DISABLE

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

#define RSI_EXT_CUSTOM_FEATURE_BIT_MAP (EXT_FEAT_LOW_POWER_MODE | EXT_FEAT_XTAL_CLK_ENABLE | RAM_LEVEL_NWP_ALL_MCU_ZERO)

#define RSI_BT_FEATURE_BITMAP (BT_RF_TYPE)
/*=======================================================================*/
//! Band command parameters
/*=======================================================================*/
//! RSI_BAND_2P4GHZ(2.4GHz) or RSI_BAND_5GHZ(5GHz) or RSI_DUAL_BAND
#define RSI_BAND RSI_BAND_2P4GHZ //! RSI_BAND_5GHZ or RSI_BAND_2P4GHZ

/*=======================================================================*/
//! Power save command parameters
/*=======================================================================*/
//! set handshake type of power mode
#define RSI_HAND_SHAKE_TYPE GPIO_BASED

//! 0 - LP, 1- ULP mode with RAM retention and 2 - ULP with Non RAM retention
#define RSI_SELECT_LP_OR_ULP_MODE RSI_ULP_WITH_RAM_RET

//! set DTIM aligment required
//! 0 - module wakes up at beacon which is just before or equal to listen_interval
//! 1 - module wakes up at DTIM beacon which is just before or equal to listen_interval
#define RSI_DTIM_ALIGNED_TYPE 0

//! Monitor interval for the FAST PSP mode
//! default is 50 ms, and this parameter is valid for FAST PSP only
#define RSI_MONITOR_INTERVAL 50
/***********************************************************************************************************************************************/

//! Number of DTIMs to skip during powersave
#define RSI_NUM_OF_DTIM_SKIP 0

#define TCP_RX_WINDOW_DIV_FACTOR 44
//! including the common defines
#include "rsi_wlan_common_config.h"

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
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
/**
 * @file    rsi_wlan_config.h
 * @version 0.1
 * @date    01 Feb 2021
 *
 *
 *
 *  @brief : This file contains user configurable details to configure the device
 *
 *  @section Description  This file contains user configurable details to configure the device
 */

#ifndef RSI_WLAN_CONFIG_H_
#define RSI_WLAN_CONFIG_H_
/*=======================================================================*/
//  ! INCLUDES
/*=======================================================================*/
#include <rsi_bt.h>
/***********************************************************************************************************************************************/
//! WLAN SAPI CONFIG DEFINES
/***********************************************************************************************************************************************/
//! WLAN Mode
#define RSI_WLAN_MODE RSI_WLAN_CLIENT_MODE
//! CO-EX Mode configuration
#define RSI_COEX_MODE 5
#define ENABLE_1P8V   1 //! Disable this when using 3.3v
//! Enable feature
#define RSI_ENABLE 1
//! Disable feature
#define RSI_DISABLE 0
//! To enable concurrent mode
#define CONCURRENT_MODE RSI_DISABLE

//! BT power control
#define BT_PWR_CTRL_DISABLE 0
#define BT_PWR_CTRL         1
#define BT_PWR_INX          10

/*=======================================================================*/
//! Opermode command parameters
/*=======================================================================*/
#define RSI_FEATURE_BIT_MAP (FEAT_SECURITY_OPEN) //! To set wlan feature select bit map

#define RSI_TCP_IP_BYPASS RSI_DISABLE //! TCP IP BYPASS feature check

#define RSI_TCP_IP_FEATURE_BIT_MAP \
  (TCP_IP_FEAT_DHCPV4_CLIENT) //! TCP/IP feature select bitmap for selecting TCP/IP features

#define RSI_EXT_TCPIP_FEATURE_BITMAP 0

#define RSI_CUSTOM_FEATURE_BIT_MAP FEAT_CUSTOM_FEAT_EXTENTION_VALID

/*=======================================================================*/
//! To set Extended custom feature select bit map
/*=======================================================================*/
#if ENABLE_1P8V
#define RSI_EXT_CUSTOM_FEATURE_BIT_MAP (RAM_LEVEL_NWP_ALL_MCU_ZERO | EXT_FEAT_1P8V_SUPPORT)
#else
#define RSI_EXT_CUSTOM_FEATURE_BIT_MAP (RAM_LEVEL_NWP_ALL_MCU_ZERO)
#endif

#define RSI_BT_FEATURE_BITMAP (A2DP_PROFILE_ENABLE | A2DP_SOURCE_ROLE_ENABLE | BT_RF_TYPE)
/*=======================================================================*/
//! Band command parameters
/*=======================================================================*/
#define RSI_BAND RSI_BAND_2P4GHZ //! RSI_BAND_5GHZ or RSI_BAND_2P4GHZ

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
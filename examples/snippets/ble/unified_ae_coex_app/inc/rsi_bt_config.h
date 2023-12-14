/*******************************************************************************
* @file  rsi_bt_config.h
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
 * @file     rsi_bt_config.h
 * @version  0.1  
 * @date     03 Sep 2015
 *
 *
 *
 *  @brief : This file contain definitions and declarations of BT APIs.
 *
 *  @section Description  This file contains definitions and declarations required to
 *  configure BT module.
 *
 *
 */

#ifndef SAPIS_EXAMPLES_RSI_DEMO_APPS_INC_RSI_BT_CONFIG_DEF_H_
#define SAPIS_EXAMPLES_RSI_DEMO_APPS_INC_RSI_BT_CONFIG_DEF_H_
#include <rsi_common_app.h>
#include <a2dp_avrcp_config.h>
#if BT_A2DP_SOURCE
#include "rsi_bt_config_DEMO_5.h"
#elif BT_HFP_HF_ROLE
#include "rsi_bt_config_DEMO_x.h"
#include "audio_task.h"
#elif BLE_ADV_BT_SPP_THROUGHPUT
#include "rsi_bt_config_DEMO_8.h"
#elif BT_EIR_SNIFF_SPP_PERIPHERAL
#include "rsi_bt_config_DEMO_7.h"
#elif BT_A2DP_SOURCE_WIFI_HTTP_S_RX
#include "rsi_bt_config_DEMO_18.h"
#elif BLE_DUAL_MODE_BT_A2DP_SOURCE_WIFI_HTTP_S_RX
#include "rsi_bt_config_DEMO_20.h"
#elif BLE_DUAL_MODE_BT_SPP_PERIPHERAL
#include "rsi_bt_config_DEMO_21.h"
#elif BLE_PER
#include "rsi_bt_config_DEMO_31.h"
#elif BT_SNIFF
#include "rsi_bt_config_DEMO_26.h"
#elif BT_PAGE_INQUIRY
#include "rsi_bt_config_DEMO_27.h"
#elif BT_PER
#include "rsi_bt_config_DEMO_28.h"
#elif BT_SPP_RX_TX
#include "rsi_bt_config_DEMO_29.h"
#elif BT_INQUIRY_SCAN
#include "rsi_bt_config_DEMO_32.h"
#elif BT_PAGE_SCAN
#include "rsi_bt_config_DEMO_33.h"
#elif BT_SPP_CENTRAL_SNIFF
#include "rsi_bt_config_DEMO_34.h"
#elif BT_SPP_PERIPHERAL_SNIFF
#include "rsi_bt_config_DEMO_35.h"
#elif (BT_A2DP_SOURCE_SBC_CODEC)
#include "rsi_bt_config_DEMO_42.h"
#elif BT_A2DP_SOURCE_WIFI_HTTP_S_RX_DYN_COEX
#include "rsi_bt_config_DEMO_45.h"
#elif ANT_APP
#include "rsi_bt_config_ANT.h"
#elif ANT_APP_POWERSAVE
#include "rsi_bt_config_ANT_APP_PWRSAVE.h"
#elif BLE_CENTRAL_ANT_APP
#include "rsi_bt_config_BLE_CENTRAL_ANT_APP.h"
#elif BLE_PERIPHERAL_ANT_APP
#include "rsi_bt_config_BLE_PERIPHERAL_ANT_APP.h"
#elif ANT_BLE_PERI_BT_A2DP_SRC_APP
#include "rsi_bt_config_ANT_BLE_PERI_A2DP_SRC.h"
#elif ANT_BLE_CENT_BT_A2DP_SRC_APP
#include "rsi_bt_config_ANT_BLE_CENT_A2DP_SRC.h"
#elif ANT_BLE_DUAL_ROLE_BT_A2DP_SRC_APP
#include "rsi_bt_config_ANT_BLE_DUAL_ROLE_BT_A2DP_SRC.h"
#elif BT_A2DP_SOURCE_SBC_ENC_IN_RS9116
#include "rsi_bt_config_DEMO_48.h"
#elif BLE_MULTI_PERIPHERAL_CENTRAL_BT_A2DP
#include "rsi_bt_config_DEMO_51.h"
#elif BLE_MULTI_PERIPHERAL_CENTRAL_BT_A2DP_WIFI_HTTP_S_RX
#include "rsi_bt_config_DEMO_52.h"
#elif UNIFIED_PROTOCOL
#include "rsi_bt_config_DEMO_54.h"
#elif COEX_MAX_APP
#include "rsi_bt_config_DEMO_57.h"
#elif BT_CW_MODE
#include "rsi_bt_config_DEMO_61.h"
#elif BLE_CW_MODE
#include "rsi_bt_config_DEMO_62.h"
#elif COEX_THROUGHPUT
#include "rsi_bt_config_DEMO_67.h"
#else

//! WLAN + BT Classic coex mode
#define RSI_OPERMODE_WLAN_BT_CLASSIC 5

/*=======================================================================*/
//! Discovery command parameters
/*=======================================================================*/

//! Discovery mode
//! Start the discoverable mode
#define START_DISCOVERY              0x01
//! Start the limited discoverable mode
#define START_LIMITED_DISCOVERY      0x02
//! Stop the discoverable mode
#define STOP_DISCOVERY               0x00

/*=======================================================================*/
//! Connectability command parameters
/*=======================================================================*/

//! Connectable mode
//! Start the connectable mode
#define CONNECTABLE                  0x01
//! Stop the connectable mode
#define NON_CONNECTABLE              0x00

/*=======================================================================*/
//!  Authentication command parameters
/*=======================================================================*/

//! Pair mode
//! Enable authentication
//#define RSI_ENABLE                                  0x01
//! Enable authentication
//#define RSI_DISABLE                                 0x00

/*=======================================================================*/
//!  SSP Confirmation command parameters
/*=======================================================================*/

//! Confimation type
//! Accept confirmation
#define ACCEPT                       0x01
//! Reject confirmation
#define REJECT                       0x00

/*=======================================================================*/
//!  BT classic profile enable BITS
/*=======================================================================*/
#define RSI_SPP_PROFILE_BIT          BIT(0)
#define RSI_A2DP_PROFILE_BIT         BIT(1)
#define RSI_AVRCP_PROFILE_BIT        BIT(2)
#define RSI_HFP_PROFILE_BIT          BIT(3)
#define RSI_PBAP_PROFILE_BIT         BIT(4)

#include "rsi_bt_common_config.h"
#endif
#endif /* SAPIS_EXAMPLES_RSI_DEMO_APPS_INC_RSI_BT_CONFIG_DEF_H_ */

/*******************************************************************************
* @file  rsi_driver.h
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

#ifndef RSI_DRIVER_H
#define RSI_DRIVER_H

#include "rsi_board_configuration.h"
#ifdef RSI_M4_INTERFACE
#include "rsi_ccp_user_config.h"
#endif
#include <rsi_data_types.h>
#include <rsi_error.h>
#include <rsi_wlan_defines.h>
#include <rsi_wlan_config.h>
#ifndef RSI_ENABLE_DEMOS
#include <rsi_wlan_common_config.h>
#endif
#include <rsi_user.h>
#include <rsi_utils.h>
#include <rsi_os.h>
#include <rsi_events.h>
#include <rsi_scheduler.h>
#include <rsi_pkt_mgmt.h>
#include <rsi_queue.h>
#include <rsi_common_apis.h>
#include <rsi_common.h>
#include <rsi_wlan_apis.h>
#include <rsi_wlan.h>
#include <rsi_socket.h>
#include <rsi_timer.h>
#ifdef RSI_SPI_INTERFACE
#include <rsi_spi_intf.h>
#include <rsi_spi_cmd.h>
#endif
#ifdef RSI_SDIO_INTERFACE
#include <rsi_sdio.h>
#endif
#ifdef RSI_UART_INTERFACE
#include <rsi_uart.h>
#endif
#include <rsi_hal.h>
#include <rsi_nwk.h>
#include <rsi_setregion_countryinfo.h>
#include <rsi_bootup_config.h>
#if (defined(RSI_BT_ENABLE) || defined(RSI_BLE_ENABLE))
#include <rsi_bt_common.h>
#include <rsi_bt_apis.h>
#include <rsi_bt_config.h>
#include <rsi_ble_config.h>
#include "rsi_bt.h"
#include "rsi_ble.h"
#include "rsi_bt_common_apis.h"
#endif
#ifdef RSI_ZB_ENABLE
#include <rsi_zb_api.h>
#endif
#include <string.h>
#ifdef __GNUC__
#include "strings.h"
#endif
#ifdef RSI_WAC_MFI_ENABLE
#include "rsi_iap.h"
#endif
#ifdef RSI_PROP_PROTOCOL_ENABLE
#include "rsi_prop_protocol.h"
#endif
#include "rsi_apis_rom.h"
#include "rsi_wlan_non_rom.h"

//#include <stdlib.h>
/******************************************************
 * *                      Macros
 * ******************************************************/

// string functions
#define rsi_strcpy(x, y)         strcpy((char *)x, (const char *)y)
#define rsi_strcat(x, y)         strcat((char *)x, (const char *)y)
#define rsi_strlen(x)            strlen((const char *)x)
#define rsi_strncasecmp(x, y, z) strncasecmp((const char *)x, (const char *)y, (size_t)z)
// Below macros were used to avoid warnings for unused variables present in the API arguments
#define USED_PARAMETER(x)   x = x /*To avoid gcc/g++ warnings*/
#define UNUSED_PARAMETER(x) x = x /*To avoid gcc/g++ warnings*/
// for unused constant variables, the below assignment is valid than above
#define USED_CONST_PARAMETER(x)   (void)x /*To avoid gcc/g++ warnings*/
#define UNUSED_CONST_PARAMETER(x) (void)x /*To avoid gcc/g++ warnings*/

//! common  type
#define RSI_COMMON_Q 0
// ZB protocol type
#define RSI_ZB_Q 1

// BT protocol type
#define RSI_BT_Q 2
#ifdef SAPIS_BT_STACK_ON_HOST
#define RSI_BT_INT_MGMT_Q 6
#define RSI_BT_HCI_Q      7
#endif
// WLAN mgmt queue type
#define RSI_WLAN_MGMT_Q 4

// WLAN data queue type
#define RSI_WLAN_DATA_Q 5

// frame descriptor length
#define RSI_FRAME_DESC_LEN 16

// status offset in frame descriptor
#define RSI_STATUS_OFFSET 12

// response type offset in frame descriptor
#define RSI_RESP_OFFSET 2

#define RSI_TRUE  1
#define RSI_FALSE 0

/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/

// driver control block structure
typedef struct rsi_driver_cb_s {
  // rx packet pool
  rsi_pkt_pool_t rx_pool;

  // driver common control block structure pointer
  rsi_common_cb_t *common_cb;

  // driver common control block TX queue
  rsi_queue_cb_t common_tx_q;

  // driver WLAN  control block structure pointer
  rsi_wlan_cb_t *wlan_cb;

  // driver WLAN control block TX queue
  rsi_queue_cb_t wlan_tx_q;

  // driver scheduler control block
  rsi_scheduler_cb_t scheduler_cb;

  // driver event list structure
#if (defined(RSI_M4_INTERFACE) && !defined(A11_ROM))
  rsi_event_cb_t event_list[RSI_MAX_NUM_EVENTS];
#else
  rsi_event_cb_t *event_list;
#endif
  rsi_queue_cb_t m4_tx_q;

  rsi_queue_cb_t m4_rx_q;

  void (*rsi_tx_done_handler)(struct rsi_pkt_s *);

#ifdef RSI_ZB_ENABLE
  // driver BT/ZB control block structure pointer
  rsi_zigb_cb_t *zigb_cb;

  // driver BT/ZB control block TX queue
  rsi_queue_cb_t zigb_tx_q;
#endif

#if (defined(RSI_BT_ENABLE) || defined(RSI_BLE_ENABLE) || defined(RSI_PROP_PROTOCOL_ENABLE))
  rsi_bt_global_cb_t *bt_global_cb;

  rsi_bt_cb_t *bt_common_cb;
  rsi_queue_cb_t bt_single_tx_q;
  rsi_queue_cb_t bt_common_tx_q;
#endif
#ifdef RSI_BT_ENABLE
  rsi_bt_cb_t *bt_classic_cb;
  rsi_queue_cb_t bt_classic_tx_q;
#endif
#ifdef RSI_BLE_ENABLE
  rsi_bt_cb_t *ble_cb;
#endif
#ifdef RSI_PROP_PROTOCOL_ENABLE
  rsi_bt_cb_t *prop_protocol_cb;
  rsi_queue_cb_t prop_protocol_tx_q;
#endif
#ifdef SAPIS_BT_STACK_ON_HOST
  rsi_bt_cb_t *bt_ble_stack_cb;
  // driver BT/BLE stack control block TX queue
  rsi_queue_cb_t bt_ble_stack_tx_q;
#endif

// tx packet state in case of UART interface
#ifdef RSI_UART_INTERFACE
  uint8_t tx_pkt_state;
#endif

  void (*unregistered_event_callback)(uint32_t event_num);

} rsi_driver_cb_t;

#if !defined(RSI_PKT_ALLOC_RESPONSE_WAIT_TIME)
#define RSI_PKT_ALLOC_RESPONSE_WAIT_TIME 600000
#endif
#if !defined(RSI_PKT_FREE_RESPONSE_WAIT_TIME)
#define RSI_PKT_FREE_RESPONSE_WAIT_TIME  600000
#endif

extern rsi_driver_cb_t *rsi_driver_cb;
/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
int32_t rsi_send_ps_mode_to_module(uint8_t selected_ps_mode, uint8_t selected_ps_type);
int32_t rsi_driver_process_bt_zb_recv_cmd(rsi_pkt_t *pkt);
void rsi_clear_sockets(int32_t sockID);
void rsi_interrupt_handler(void);
void rsi_mask_ta_interrupt(void);
void rsi_unmask_ta_interrupt(void);

int32_t rsi_driver_process_recv_data(rsi_pkt_t *pkt);
//int32_t rsi_driver_send_data(int32_t sock_no, uint8_t* buffer, uint32_t length, struct rsi_sockaddr *destAddr);
int32_t rsi_send_raw_data(uint8_t *buffer, uint32_t length);
int32_t rsi_ftp_mode_set(uint8_t mode);
#ifdef RSI_CHIP_MFG_EN
typedef void (*rsi_host_packet_recv_notify_t)(uint8_t *buf, uint32_t length);

// prototypes
int32_t rsi_host_packet_recv(uint8_t *buffer, uint32_t length);
void rsi_host_send(uint8_t *tx_buff, uint16_t transfer_length);

//rsi_common_apis.c function declarations
int32_t rsi_cmd_uart_flow_ctrl(uint8_t uartflow_en);
int32_t rsi_set_rtc_timer(module_rtc_time_t *timer);
int32_t rsi_get_ram_log(uint32_t addr, uint32_t length);
void rsi_uregister_events_callbacks(void (*callback_handler_ptr)(uint32_t event_num));
int32_t rsi_get_rtc_timer(module_rtc_time_t *response);

#endif

#endif

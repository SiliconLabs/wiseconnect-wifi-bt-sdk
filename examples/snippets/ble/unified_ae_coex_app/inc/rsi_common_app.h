/*
 * rsi_common_app.h
 *
 *  Created on: 03-Nov-2018
 *     
 */

#ifndef RSI_COMMON_APP_H_
#define RSI_COMMON_APP_H_

#include "stdint.h"
#include "stdio.h"
//#include "fsl_debug_console.h"

int32_t ble_ae_set_1_advertising_enable(void);
void sl_btc_ble_adv_extended_register_callbacks_wrapper();
int32_t rsi_ble_dual_role(void);
int32_t ble_ext_scan_enable(void);

//For RT595 boards, bydefault 1.8v is set, enable ENABLE_1P8V macro if RT595 board is using

#define ENABLE_1P8V 1 //! Disable this when using 3.3v

#define SPI_DMA //! Enable SPI with DMA

/*********************Demo Macros*******************************/

#define BT_A2DP_SOURCE           0
#define BT_HFP_HF_ROLE           0
#define BT_A2DP_SOURCE_SBC_CODEC 0
#define BT_CW_MODE               0

#define BLE_PERIPHERAL               0
#define BLE_CENTRAL                  0
#define BLE_POWERSAVE                0
#define BLE_PRIVACY                  0
#define BLE_GATT_TEST_ASYNC          0
#define BLE_GATT_INDICATION_STATUS   0
#define BLE_SIMPLE_GATT              0
#define BLE_INDICATION               0
#define BLE_MULTI_PERIPHERAL_CENTRAL 0
#define BLE_OVERLAPPING_PROCEDURES   0
#define BLE_CW_MODE                  0

#define WIFI_TCP_BI_DIR                 0
#define WIFI_SSL_TX_THROUGHPUT          0
#define WIFI_TCP_RX_POWERSAVE           0
#define WIFI_CONNECTED_STANDBY          0
#define WIFI_SOCKET_SELECT              0
#define WIFI_HTTP_SOCKET_SELECT         0
#define WIFI_STA_BGSCAN                 0
#define WIFI_TCP_TX_POWERSAVE           0
#define WIFI_TCP_DNS_APP                0
#define WIFI_TCP_TX_APP                 0
#define WIFI_TCP_WLAN_GET_APP           0
#define WIFI_SSL_SERVER_CERT_BYPASS     0
#define WIFI_HTTP_S_5MB_RX_WITH_STANDBY 0
#define FIRMWARE_UPGRADE                0
#define FIRMWARE_UPGRADE_VIA_BOOTLOADER 0
#define RAM_DUMP_ASSERTION_CASE         0

#define BT_A2DP_SOURCE_WIFI_HTTP_S_RX_DYN_COEX 0
#define BT_A2DP_SOURCE_WIFI_HTTP_S_RX          0
#define UNIFIED_PROTOCOL                       0
#define COEX_MAX_APP                           0
#define COEX_THROUGHPUT                        0

#define ANT_CW_MODE 0

#define BT_BLE_UPDATE_GAIN_TABLE 0

/***************************************************************/

/***************************************************************/
/******************** WEARABLES UNUSED DEMOS **********************/

#define BT_EIR_SNIFF_SPP_PERIPHERAL      0
#define BT_PAGE_INQUIRY                  0
#define BT_SNIFF                         0
#define BT_PER                           0
#define BT_SPP_RX_TX                     0
#define BT_INQUIRY_SCAN                  0
#define BT_PAGE_SCAN                     0
#define BT_SPP_CENTRAL_SNIFF             0
#define BT_SPP_PERIPHERAL_SNIFF          0
#define BT_A2DP_SOURCE_SBC_ENC_IN_RS9116 0

#define BLE_DUAL_ROLE         0
#define BLE_ADV               0
#define BLE_SCAN              0
#define BLE_IDLE_POWERSAVE    0
#define BLE_PER               0
#define BLE_DATA_TRANSFER     0
#define BLE_LONG_READ         0
#define BT_BLE_IDLE_POWERSAVE 0

#define BLE_DUAL_MODE_BT_A2DP_SOURCE_WIFI_HTTP_S_RX 0
#define BLE_DUAL_MODE_BT_SPP_PERIPHERAL             0
#define BLE_ADV_BT_SPP_THROUGHPUT                   0
#ifdef BLE_ADV_BT_SPP_THROUGHPUT
#define BLE_ADVERTISE_ENABLE 0 //! Enable BLE with SPP
#endif
#define BLE_MULTI_PERIPHERAL_CENTRAL_BT_A2DP_WIFI_HTTP_S_RX 0

#define WIFI_DEEPSLEEP_STANDBY                 0
#define WIFI_SSL_RX_THROUGHPUT                 0
#define WIFI_STA_CONN_DISCONN_SSL_CONN_DISCONN 0
#define WIFI_STA_SCAN_CONN_DISCONN             0
#define MBR_BURN_ERASE                         0

#define ANT_APP                      0
#define BLE_CENTRAL_ANT_APP          0
#define BLE_PERIPHERAL_ANT_APP       0
#define ANT_BLE_PERI_BT_A2DP_SRC_APP 0
#define ANT_BLE_CENT_BT_A2DP_SRC_APP 0

#define COEX_TEST_FRAMEWORK 0

/***************************************************************/
/***************************************************************/

#if ANT_BLE_DUAL_ROLE_BT_A2DP_SRC_APP

#define ENABLE_ANT 1
#define ENABLE_BT  1
#define ENABLE_BLE 1

#endif
//! WLAN Mode
#define RSI_WLAN_MODE RSI_WLAN_CLIENT_MODE

#if COEX_MAX_APP // Major
//! By default this macro is disabled
//! Enable this macro to take run time inputs
#define RUN_TIME_CONFIG_ENABLE 0
//! compile time configurations
#if !RUN_TIME_CONFIG_ENABLE
/***********************************************************************************************************************************************/
//! Macros to enable/disable individual protocol activities Minors
/***********************************************************************************************************************************************/
#define RSI_ENABLE_BLE_TEST  1
#define RSI_ENABLE_BT_TEST   1
#define RSI_ENABLE_ANT_TEST  1
#define RSI_ENABLE_WIFI_TEST 0
#endif
#endif

#if 1
#define RSI_ENABLE_BLE_TEST 0
#endif

#if UNIFIED_PROTOCOL // Major
/***********************************************************************************************************************************************/
//! Macros to enable/disable individual protocol activities Minors
/***********************************************************************************************************************************************/
#define RSI_ENABLE_BT_TEST     0
#define RSI_ENABLE_WIFI_TEST   0
#define RSI_ENABLE_BLE_TEST    1
#define RSI_ENABLE_ANT_TEST    0
#define RSI_GENERAL_POWER_TEST 0

#if RSI_ENABLE_BLE_TEST
//! By default this macro is enabled, verifies multiconnection procedures
#define RSI_BLE_MULTICONN_TEST 1
//! By default this macro is disabled, enable this macro to test BLE power save figures
#define RSI_BLE_POWER_NUM_TEST 0
//! By default this macro is disabled, enable this to test advertising interval test
#define RSI_BLE_ADVERTISING_TEST 0
//! By default this macro is disabled, enable this macro to test sanity test
#define RSI_BLE_SANITY_TEST 0

#if RSI_BLE_SANITY_TEST
//! BY default this macro is enabled, if enabled 'Notifications' procedure is executed
#define CHECK_NOTIFICATIONS 1
//! BY default this macro is disabled, if enabled 'RSI receives notifications from remote device'
#define CHECK_NOTIFICATIONS_RX 0
//! BY default this macro is disabled, if enabled 'Indication' procedure is executed
#define CHECK_INDICATIONS 0
//! BY default this macro is disabled, if enabled 'write to read only characteristic' procedure is executed under DUT as central
#define WRITE_TO_READONLY_CHAR_TEST 0
#endif

#if RSI_BLE_MULTICONN_TEST
//! BY default this macro is enabled, if enabled 'Notifications' procedure is executed in all connections
#define CHECK_NOTIFICATIONS 1
//! BY default this macro is disable, if enabled 'Write Without Response' procedure is executed in all connections
#define CHECK_WRITE_WITHOUT_RESP 0
//! BY default this macro is disabled, if enabled 'write with response' procedure is executed in all connections
#define CHECK_WRITE_PROPERTY 0
//! BY default this macro is disabled, if enabled 'Indication' procedure is executed in all connections
#define CHECK_INDICATIONS 0
#endif

#endif

#if RSI_GENERAL_POWER_TEST
#define RSI_DEEP_SLEEP_WITH_RAM_RET_AFTER_RESET      1
#define RSI_DEEP_SLEEP_WITH_N_WO_RAM_RET_AFTER_RESET 0
#define RSI_POWER_AFTER_CHIP_INIT                    0
#define RSI_STANDBY                                  0
#define RSI_DEEP_SLEEP_WITHOUT_RAM_RET               0
#endif

#if RSI_ENABLE_BT_TEST
#define CHECK_BT_POWER_STATE      0
#define BT_EIR_FRIENDLY_NAME_TEST 0
#endif

#if RSI_ENABLE_WIFI_TEST
//! enable this macro to test SSL receive max throughput
#define SSL_RX_MAX_THROUGHPUT_TEST 1
//! enable this macro to test WLAN secure and TLS SOCKET connection & disconnections
#define WIFI_STA_AND_SSL_CONN_DISCONN_TEST 0
//! enable this macro to test Powersave with Standby and deepsleep
#define WIFI_DEEPSLEEP_STANDBY_TEST 0
//! enable this macro to test continuous scan + connect + disconnect tests
#define STA_SCAN_CONN_DISCNCT_TEST 0
//! enable this macro to test Multithreaded HTTP/S download test
#define MULTITHREADED_HTTP_DOWNLOAD_TEST 0
//! enable this macro to test Multithreaded TCP TX test
#define MULTITHREADED_TCP_TX_TEST 0
//! enable this macro to verify wifi power numbers
#define CHECK_WIFI_POWER_STATE 0
#endif
#endif

//! CO-EX Mode configuration
#if (WIFI_SSL_RX_THROUGHPUT || WIFI_TCP_BI_DIR || WIFI_SSL_TX_THROUGHPUT || WIFI_TCP_RX_POWERSAVE                 \
     || WIFI_CONNECTED_STANDBY || WIFI_SOCKET_SELECT || WIFI_STA_BGSCAN || WIFI_STA_CONN_DISCONN_SSL_CONN_DISCONN \
     || WIFI_TCP_TX_POWERSAVE || WIFI_DEEPSLEEP_STANDBY || WIFI_HTTP_S_5MB_RX_WITH_STANDBY || FIRMWARE_UPGRADE    \
     || FIRMWARE_UPGRADE_VIA_BOOTLOADER || WIFI_IDLE_POWERSAVE || WIFI_SSL_SERVER_CERT_BYPASS || WIFI_TCP_DNS_APP \
     || WIFI_STA_SCAN_CONN_DISCONN || WIFI_TCP_WLAN_GET_APP || WIFI_HTTP_SOCKET_SELECT || WIFI_TCP_TX_APP         \
     || RAM_DUMP_ASSERTION_CASE)
#define RSI_COEX_MODE (0x109)
#elif (BT_A2DP_SOURCE_WIFI_HTTP_S_RX || BT_A2DP_SOURCE || BT_A2DP_SOURCE_SBC_ENC_IN_RS9116 || BT_A2DP_SOURCE_SBC_CODEC \
       || (BLE_ADV_BT_SPP_THROUGHPUT && (!BLE_ADVERTISE_ENABLE)) || BT_EIR_SNIFF_SPP_PERIPHERAL || BT_PAGE_INQUIRY     \
       || BT_SNIFF || BT_PER || BT_SPP_RX_TX || BT_INQUIRY_SCAN || BT_PAGE_SCAN || BT_SPP_CENTRAL_SNIFF                \
       || BT_SPP_PERIPHERAL_SNIFF || BT_A2DP_SOURCE_WIFI_HTTP_S_RX_DYN_COEX || BT_CW_MODE)
#define RSI_COEX_MODE 5
#elif ((BLE_ADV_BT_SPP_THROUGHPUT && BLE_ADVERTISE_ENABLE) || BLE_DUAL_MODE_BT_A2DP_SOURCE_WIFI_HTTP_S_RX  \
       || BLE_DUAL_MODE_BT_SPP_PERIPHERAL || BT_BLE_IDLE_POWERSAVE || BLE_MULTI_PERIPHERAL_CENTRAL_BT_A2DP \
       || BLE_MULTI_PERIPHERAL_CENTRAL_BT_A2DP_WIFI_HTTP_S_RX || BT_BLE_UPDATE_GAIN_TABLE)
#define RSI_COEX_MODE 9
#elif (BLE_PERIPHERAL || BLE_CENTRAL || BLE_DUAL_ROLE || BLE_POWERSAVE || BLE_ADV || BLE_SCAN || BLE_IDLE_POWERSAVE \
       || BLE_PER || BLE_PRIVACY || BLE_DATA_TRANSFER || BLE_LONG_READ || BLE_L2CAP_CBFC || BLE_GATT_TEST_ASYNC     \
       || BLE_GATT_INDICATION_STATUS || BLE_SIMPLE_GATT || BLE_MULTI_PERIPHERAL_CENTRAL || BLE_INDICATION           \
       || BLE_OVERLAPPING_PROCEDURES || BLE_CW_MODE)
#define RSI_COEX_MODE 13
#elif (ANT_APP || BLE_CENTRAL_ANT_APP || BLE_PERIPHERAL_ANT_APP)
#define RSI_COEX_MODE (0x10D)
#elif (ANT_BLE_PERI_BT_A2DP_SRC_APP || ANT_BLE_CENT_BT_A2DP_SRC_APP || ANT_BLE_DUAL_ROLE_BT_A2DP_SRC_APP   \
       || WLAN_STANDBY_WITH_ANT || WIFI_ANT_HTTP_S_5MB_RX_WITH_STANDBY || UNIFIED_PROTOCOL || COEX_MAX_APP \
       || ANT_APP_POWERSAVE || ANT_CW_MODE || COEX_THROUGHPUT || BT_HFP_HF_ROLE)
#define RSI_COEX_MODE \
  (0x109) //! ensure RSI_COEX_MODE macro value and RSI_OPERMODE_WLAN_BT_DUAL_MODE macro value in  rsi_bt_common.h should be same.
#endif

#define TICK_1_MSEC 1 //!	0 - Disable, 1 - Enable
#ifndef RSI_WITH_OS
//! systick timer configurations
#define TICK_100_USEC 0 //!	0 - Disable, 1 - Enable
#define TICK_1_MSEC   1 //!	0 - Disable, 1 - Enable
#define T_NOP         0 //! Use NOP for delay
#endif

#define RSI_DEBUG_EN 0

//#define LOG_PRINT PRINTF
#if RSI_DEBUG_EN
#define LOG_PRINT_D(...)                                          \
  {                                                               \
    rsi_mutex_lock(&rsi_driver_cb_non_rom->debug_prints_mutex);   \
    printf(__VA_ARGS__);                                          \
    rsi_mutex_unlock(&rsi_driver_cb_non_rom->debug_prints_mutex); \
  }
#else
#define LOG_PRINT_D(...) \
  {                      \
  }
#endif

//! Function declarations
void rsi_demo_app(void);
#if (BT_A2DP_SOURCE || BT_A2DP_SOURCE_SBC_ENC_IN_RS9116)
int32_t rsi_app_task_bt_a2dp_source(void);
int32_t rsi_bt_a2dp_source(void);

#elif BT_HFP_HF_ROLE
int32_t rsi_app_task_bt_hfp_hf_role(void);
int32_t rsi_task_bt_hfp_audio(void);

#elif BT_A2DP_SOURCE_SBC_CODEC
int32_t rsi_app_task_bt_a2dp_source_sbc_codec(void);
int32_t rsi_bt_a2dp_source_sbc_codec(void);

#elif BT_PER
int32_t rsi_app_task_bt_per(void);

#elif BT_SNIFF
int32_t rsi_app_task_bt_connected_sniff(void);

#elif BT_SPP_RX_TX
int32_t rsi_app_task_bt_spp_rx_tx(void);

#elif BLE_ADV_BT_SPP_THROUGHPUT
int32_t rsi_app_task_ble_adv_bt_spp_tpt(void);

#elif BT_EIR_SNIFF_SPP_PERIPHERAL
int32_t rsi_app_task_eir_sniff_spp(void);

#elif BLE_PERIPHERAL
void rsi_app_task_ble_peripheral(void);
int32_t rsi_ble_app_task(void);
int32_t rsi_ble_dual_role(void);

#elif BLE_INDICATION
void rsi_app_task_ble_peripheral(void);
int32_t rsi_ble_app_task(void);
int32_t rsi_ble_dual_role(void);

#elif BLE_SIMPLE_GATT
void rsi_app_task_ble_simple_gatt(void);
int32_t rsi_ble_app_task(void);
int32_t rsi_ble_dual_role(void);

#elif BLE_PRIVACY
void rsi_app_task_ble_privacy(void);
int32_t rsi_ble_app_task(void);
int32_t rsi_ble_dual_role(void);

#elif BLE_PER
int32_t rsi_ble_per(void);

#elif BLE_DATA_TRANSFER
void rsi_app_task_ble_data_transfer(void);
int32_t rsi_ble_app_task(void);
int32_t rsi_ble_peripheral_role(void);

#elif BLE_L2CAP_CBFC
void rsi_app_task_le_l2cap_cbfc(void);
int32_t rsi_ble_central_l2cap_cbfc(void);
int32_t rsi_ble_app_task(void);

#elif BLE_LONG_READ
void rsi_app_task_ble_long_read(void);
int32_t rsi_app_init_ble_simple_gatt_test(void);
int32_t rsi_ble_app_task(void);

#elif BLE_GATT_TEST_ASYNC
void rsi_app_task_ble_simple_gatt_test(void);
int32_t rsi_ble_simple_gatt_test_app_task(void);
int32_t rsi_ble_simple_gatt_test_app_init(void);

#elif BLE_GATT_INDICATION_STATUS
void rsi_app_task_ble_gatt_indication_status(void);
int32_t rsi_ble_gatt_indication_status_app_task(void);
int32_t rsi_ble_gatt_indication_status_app_init(void);

#elif BLE_CENTRAL
void rsi_app_task_ble_central(void);
int32_t rsi_ble_app_task(void);
int32_t rsi_ble_dual_role(void);

#elif BLE_DUAL_ROLE
void rsi_app_task_ble_dual_role(void);
void rsi_ble_app_task(void);
int32_t rsi_ble_dual_role(void);
#elif BLE_POWERSAVE
void rsi_app_task_ble_powersave(void);
int32_t rsi_ble_app_task(void);
int32_t rsi_ble_dual_role(void);

#elif BT_PAGE_INQUIRY
int32_t rsi_app_task_bt_page_inquiry(void);

#elif BT_INQUIRY_SCAN
int32_t rsi_app_task_bt_inquiry_scan(void);

#elif BT_PAGE_SCAN
int32_t rsi_app_task_bt_page_scan(void);

#elif BT_SPP_CENTRAL_SNIFF
int32_t rsi_app_task_bt_spp_central_sniff(void);

#elif BT_SPP_PERIPHERAL_SNIFF
int32_t rsi_app_task_bt_spp_peripheral_sniff(void);

#elif BLE_ADV
void rsi_app_task_ble_peripheral_adv(void);
int32_t rsi_ble_peripheral_adv(void);
int32_t rsi_ble_adv(void);
#elif BLE_SCAN
void rsi_app_task_ble_central_scan(void);
int32_t rsi_ble_central_scan(void);
int32_t rsi_ble_scan(void);

#elif BLE_IDLE_POWERSAVE
int32_t rsi_app_task_ble_ilde_powersave(void);

#elif WIFI_SSL_RX_THROUGHPUT
int32_t rsi_app_task_wifi_ssl_rx_tpt(void);

#elif MBR_BURN_ERASE
int32_t rsi_app_task_mbr_burn_erase(void);

#elif WIFI_TCP_BI_DIR
int32_t rsi_app_task_wifi_tcp_bi_dir(void);

#elif WIFI_SSL_TX_THROUGHPUT
int32_t rsi_app_task_wifi_ssl_tx_tpt(void);

#elif WIFI_TCP_RX_POWERSAVE
int32_t rsi_app_task_wifi_tcp_rx_ps(void);

#elif WIFI_CONNECTED_STANDBY
int32_t rsi_app_task_wifi_connected_standby(void);
#elif WIFI_SOCKET_SELECT
int32_t rsi_app_task_wifi_socket_select(void);
int32_t rsi_app_wlan_socket_create1(void);
int32_t rsi_app_wlan_socket_create2(void);

#elif WIFI_HTTP_SOCKET_SELECT
int32_t rsi_app_task_wifi_http_socket_select(void);
int32_t rsi_app_wlan_socket_create1(void);
#elif WIFI_STA_BGSCAN
int32_t rsi_app_task_wifi_sta_bgscan(void);

#elif WIFI_STA_CONN_DISCONN_SSL_CONN_DISCONN
int32_t rsi_app_task_wifi_sta_ssl_conn_disconn(void);

#elif WIFI_STA_SCAN_CONN_DISCONN
int32_t rsi_app_task_wifi_sta_scan_conn_disconn(void);

#elif WIFI_TCP_TX_POWERSAVE
int32_t rsi_app_task_wifi_tcp_tx_ps(void);
#elif WIFI_TCP_DNS_APP
int32_t rsi_app_task_wifi_tcp_tx(void);
int32_t rsi_app_task_dns(void);
#elif WIFI_TCP_TX_APP
int32_t rsi_app_task_wifi_tcp_tx(void);
#elif WIFI_TCP_WLAN_GET_APP
int32_t rsi_app_task_wifi_tcp_tx(void);
int32_t rsi_app_task_wlan_get(void);

#elif WIFI_DEEPSLEEP_STANDBY
int32_t rsi_app_task_wifi_deepsleep_standby(void);

#elif WIFI_SSL_SERVER_CERT_BYPASS
int32_t rsi_app_task_wifi_ssl_server_cert_bypass(void);
int32_t rsi_certificate_valid(uint16_t valid, uint16_t socket_id);
void rsi_app_task_send_certificates(void);
#elif BT_A2DP_SOURCE_WIFI_HTTP_S_RX
void rsi_app_task_bt_a2dp_wifi_http_s(void);
int32_t rsi_wlan_app_task(void);
int32_t rsi_bt_app_task(void);
int32_t rsi_bt_a2dp_source(void);

#elif BT_A2DP_SOURCE_WIFI_HTTP_S_RX_DYN_COEX
void rsi_app_task_bt_a2dp_wifi_http_s_dyn_coex(void);
void rsi_wlan_app_task(void);
int32_t rsi_bt_app_task(void);
int32_t rsi_bt_a2dp_source(void);
void rsi_ui_app_task(void);

#elif WIFI_HTTP_S_5MB_RX_WITH_STANDBY
int32_t rsi_app_task_wifi_https_rx_standby(void);

#elif WIFI_ANT_HTTP_S_5MB_RX_WITH_STANDBY
void rsi_wlan_ant_common_task();
int32_t rsi_app_task_wifi_ant_https_rx_standby(void);
void rsi_ble_ant_app_task(void);
int32_t rsi_ant_task(void);
#elif BLE_DUAL_MODE_BT_A2DP_SOURCE_WIFI_HTTP_S_RX
void rsi_app_task_bt_ble_dual_mode_wifi_https(void);
void rsi_wlan_app_task(void);
int32_t rsi_bt_app_task(void);
void rsi_ble_app_task(void);
int32_t rsi_bt_a2dp_source(void);
int32_t rsi_ble_dual_role(void);

#elif BLE_DUAL_MODE_BT_SPP_PERIPHERAL
void rsi_app_task_bt_ble_dualmode(void);
int32_t rsi_bt_app_task(void);
void rsi_ble_app_task(void);
int32_t rsi_bt_spp(void);
int32_t rsi_ble_dual_role(void);

//#elif BLE_ADV_BT_A2DP_SOURCE
//void rsi_app_task_ble_adv_bt_a2dp(void);
//int32_t rsi_bt_app_task(void);
//void rsi_ble_app_task(void);
//int32_t rsi_bt_a2dp_source(void);
//int32_t rsi_ble_adv_role(void);

#elif FIRMWARE_UPGRADE
int32_t rsi_app_task_fw_update(void);

#elif FIRMWARE_UPGRADE_VIA_BOOTLOADER
int32_t rsi_app_task_fw_update_via_bootloader(void);
#elif RAM_DUMP_ASSERTION_CASE
int32_t rsi_app_task_ram_dump_assertion_case(void);
#elif BT_BLE_IDLE_POWERSAVE
int32_t rsi_app_task_bt_ble_ilde_powersave(void);

#elif ANT_APP
void rsi_app_task_ant_app(void);
void rsi_ant(void);
int32_t rsi_ant_power_save(void);
#elif ANT_APP_POWERSAVE
void rsi_app_task_ant_app_power_save(void);
int32_t rsi_ant_power_save(void);
extern int32_t rsi_bt_power_save_profile(uint8_t psp_mode, uint8_t psp_type);
#elif BLE_CENTRAL_ANT_APP
int32_t rsi_ble_central(void);
void rsi_ant_ble_central_coex_task();
#elif BLE_PERIPHERAL_ANT_APP
int32_t rsi_ble_peripheral(void);
void rsi_ant_ble_peripheral_coex_task();
//  extern void rsi_ant_register_callbacks (rsi_ant_resp_handler_t   ant_async_resp_handler);
#elif ANT_BLE_PERI_BT_A2DP_SRC_APP
uint32_t rsi_ble_ant_app(void);
void rsi_ble_ant_app_task(void);
void rsi_app_task_ant_ble_peri_bt_a2dp_coex_task(void);
void rsi_ble_app_task(void);
int32_t rsi_ble_peripheral_role(void);
int32_t rsi_bt_a2dp_source(void);
int32_t rsi_bt_app_task();
#elif ANT_BLE_CENT_BT_A2DP_SRC_APP
uint32_t rsi_ble_ant_app(void);
void rsi_ble_ant_app_task(void);
int32_t rsi_ant_ble_cent_bt_a2dp_source(void);
void rsi_app_task_ant_ble_cent_bt_a2dp_coex_task(void);
int32_t rsi_ble_app_task();
int32_t rsi_bt_a2dp_source(void);
int32_t rsi_bt_app_task();
int32_t rsi_ble_central_role(void);
#elif ANT_BLE_DUAL_ROLE_BT_A2DP_SRC_APP
uint32_t rsi_ant_app(void);
void rsi_ant_app_task(void);
int32_t rsi_ant_ble_dual_role_bt_a2dp_source(void);
void rsi_app_task_ant_ble_dual_role_bt_a2dp_coex_task(void);
void rsi_ble_app_task(void);
int32_t rsi_ble_dual_role(void);
int32_t rsi_bt_a2dp_source(void);
int32_t rsi_bt_app_task();
int32_t rsi_ant_task(void);
int32_t rsi_ble_main_app_task(void);
void rsi_ble_ant_app_task(void);
uint32_t rsi_ble_ant_app(void);
#elif BLE_MULTI_PERIPHERAL_CENTRAL
void rsi_ble_main_app_task(void);
void rsi_ble_central1_app_task(void);
void rsi_ble_central2_app_task(void);
void rsi_ble_peripheral1_app_task(void);
void rsi_ble_peripheral2_app_task(void);
void rsi_ble_peripheral3_app_task(void);
void rsi_ui_app_task(void);
void rsi_ble_adv_scan_start(void);
void rsi_board_reset(void);
#elif BLE_OVERLAPPING_PROCEDURES
void rsi_ble_main_app_task(void);
void rsi_ble_central1_app_task(void);
void rsi_ble_central2_app_task(void);
void rsi_ble_peripheral1_app_task(void);
void rsi_ble_peripheral2_app_task(void);
void rsi_ble_peripheral3_app_task(void);
#elif BLE_MULTI_PERIPHERAL_CENTRAL_BT_A2DP
int32_t rsi_bt_app_task();
void rsi_ble_main_app_task(void);
void rsi_ble_central1_app_task(void);
void rsi_ble_central2_app_task(void);
void rsi_ble_peripheral1_app_task(void);
void rsi_ble_peripheral2_app_task(void);
void rsi_ble_peripheral3_app_task(void);
void rsi_ui_app_task(void);
#elif BLE_MULTI_PERIPHERAL_CENTRAL_BT_A2DP_WIFI_HTTP_S_RX
int32_t rsi_wlan_app_task(void);
int32_t rsi_bt_app_task();
void rsi_ble_main_app_task(void);
void rsi_ble_central1_app_task(void);
void rsi_ble_central2_app_task(void);
void rsi_ble_peripheral1_app_task(void);
void rsi_ble_peripheral2_app_task(void);
void rsi_ble_peripheral3_app_task(void);
void rsi_ui_app_task(void);
#elif COEX_MAX_APP
int32_t rsi_wlan_app_task(void);
int32_t rsi_bt_app_task();
void rsi_ble_main_app_task(void);
void rsi_ble_task_on_conn(void *parameters);
void rsi_ble_peripheral_app_task(void);
void rsi_ant_app_task(void);
void rsi_ui_app_task(void);
void rsi_common_app_task(void);
void rsi_window_reset_notify_app_task(void);
int32_t rsi_certificate_valid(uint16_t valid, uint16_t socket_id);
void rsi_app_task_send_certificates(void);
#elif UNIFIED_PROTOCOL
void rsi_common_app_task(void);
#if RSI_ENABLE_WIFI_TEST
int32_t rsi_wlan_app_task(void);
#elif RSI_ENABLE_BT_TEST
int32_t rsi_bt_app_task();
#elif RSI_ENABLE_BLE_TEST
void rsi_ble_main_app_task(void);
void rsi_ble_central1_app_task(void);
void rsi_ble_central2_app_task(void);
void rsi_ble_peripheral1_app_task(void);
void rsi_ble_peripheral2_app_task(void);
void rsi_ble_peripheral3_app_task(void);
void rsi_ui_app_task(void);
int32_t rsi_ble_sanity_app_task(void);
#elif RSI_GENERAL_POWER_TEST
int32_t rsi_power_num_app_task(void);
#elif RSI_ENABLE_ANT_TEST
int32_t rsi_ant_task(void);
#endif
#elif (WLAN_STANDBY_WITH_ANT)
void rsi_wlan_ant_common_task();
int32_t rsi_wlan_standby_with_ant_task(void);
int32_t rsi_ant_task(void);
void rsi_ble_ant_app_task(void);
#elif BT_CW_MODE
int32_t rsi_app_task_bt_cw_mode(void);
#elif BLE_CW_MODE
int32_t rsi_ble_cw_mode(void);
#elif BT_BLE_UPDATE_GAIN_TABLE
void rsi_app_task_update_gain_table(void);
#endif
#endif

extern void rsi_task_bt_gap(void);
extern int32_t rsi_app_task_bt_hfp_hf_role(void);
extern void rsi_task_bt_hfp(void);

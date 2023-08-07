/*******************************************************************************
* @file  rsi_wlan_apis.c
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

#include "rsi_driver.h"
#include "rsi_wlan_non_rom.h"
#define GAIN_TABLE_MAX_PAYLOAD_LEN 128

extern struct wpa_scan_results_arr *scan_results_array;

/** @addtogroup WLAN
* @{
*/

#ifdef PROCESS_SCAN_RESULTS_AT_HOST
/*==============================================*/
/**
 * @brief       Sort the scan results in the order of RSSI.
 * @param[in]   scan_results_array     - Contains the array of scan results that need to be sorted.
 * @return      Void
 * @note        **Precondition** - \ref rsi_wlan_scan_with_bitmap_options() API needs to be called before this API.
 */
void rsi_sort_scan_results_array_based_on_rssi(struct wpa_scan_results_arr *scan_results_array)
{
  SL_PRINTF(SL_SORT_SCAN_RESULTS_ENTRY, WLAN, LOG_INFO);
  uint16_t i = 0, j = 0;
  struct wpa_scan_res temp;
  memset(&temp, 0, sizeof(struct wpa_scan_res));
  if (scan_results_array != NULL) {
    for (i = 0; i < scan_results_array->num; i++) {
      for (j = 0; j < ((scan_results_array->num) - i - 1); j++) {
        if (scan_results_array->res[j].level < scan_results_array->res[j + 1].level) {
          memcpy(&temp, &(scan_results_array->res[j]), sizeof(struct wpa_scan_res));
          memcpy(&(scan_results_array->res[j]), &(scan_results_array->res[j + 1]), sizeof(struct wpa_scan_res));
          memcpy(&(scan_results_array->res[j + 1]), &temp, sizeof(struct wpa_scan_res));
        }
      }
    }
  }
  SL_PRINTF(SL_SORT_SCAN_RESULTS_EXIT, WLAN, LOG_INFO);
}
#endif
/** @addtogroup WLAN
* @{
*/
/*==============================================*/
/**
 * @fn         int32_t rsi_config_timeout(uint32_t timeout_bitmap,uint16_t timeout_value)
 * @brief      This API is used to set timeouts. \n This is a blocking API.
 * @pre        \ref rsi_wireless_init() API needs to be called before this API.
 * @param[in]  timeout_type        - It is used to identify which timeout to be set.\n 
 *																		 if timeout_type is
 *																		 1 - set for association and authentication timeout request in ms.
 *																		 2 - set for the each channel active scan time in ms
 *																		 3 - is set for  the WLAN keep alive time in seconds
 * @param[in]  timeout_value         - timeout value to be set.
 * @note			 Packet is sent as Wlan Keep alive before \ref rsi_config_ipaddress() \n
 *						 After \ref rsi_config_ipaddress() it is sent as gratuitous ARP.
 * @return     0              -    Success \n
 *             Non-Zero Value -    Failure
 *
 */

int32_t rsi_config_timeout(uint32_t timeout_type, uint16_t timeout_value)
{
  int32_t status = RSI_SUCCESS;

  // Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb  = rsi_driver_cb->wlan_cb;
  uint32_t timeout_bitmap = 0;

  // check whether valid timeout_type is given or not
  if (timeout_type != AUTH_ASSOC_TIMEOUT && timeout_type != CHANNEL_ACTIVE_SCAN_TIMEOUT
      && timeout_type != KEEP_ALIVE_TIMEOUT) {
    return RSI_ERROR_INVALID_PARAM;
  }

  // check whether module is in valid state range or not
  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE || wlan_cb->state >= RSI_WLAN_STATE_INIT_DONE) {
    // Command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);

  if (status == RSI_SUCCESS) {

    timeout_bitmap = BIT(timeout_type - 1);

    status = send_timeout(timeout_bitmap, timeout_value);

    //Changing the wlan cmd state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // return wlan command error
    return status;
  }

  // Return the status
  return status;
}

/*==============================================*/
/**
 * @fn         int32_t send_timeout(uint32_t timeout_bitmap,uint16_t timeout_value)
 * @brief      This API is used to set timeouts. \n This is a blocking API.
 * @pre        \ref rsi_wireless_init() API needs to be called before this API.
 * @param[in]  timeout_type        - It is used to identify which timeout to be set.\n 
 *																		 if timeout_type is
 *																		 1 - set for association and authentication timeout request in ms.
 *																		 2 - set for the each channel active scan time in ms
 *																		 3 - is set for  the WLAN keep alive time in seconds
 * @param[in]  timeout_value         - timeout value to be set.
 * @note			 Packet is sent as Wlan Keep alive before \ref rsi_config_ipaddress() \n
 *						 After \ref rsi_config_ipaddress() it is sent as gratuitous ARP.
 * @return     0              -    Success \n
 *             Non-Zero Value -    Failure
 *
 */
int32_t send_timeout(uint32_t timeout_bitmap, uint16_t timeout_value)
{
  int32_t status = RSI_SUCCESS;

  // Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  rsi_pkt_t *pkt = NULL;

  rsi_req_timeout_t *rsi_timeout = NULL;

  // allocate command buffer  from wlan pool
  pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

  // If allocation of packet fails
  if (pkt == NULL) {
    // return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }

  // Memset data
  memset(&pkt->data, 0, sizeof(rsi_req_timeout_t));

  rsi_timeout = (rsi_req_timeout_t *)pkt->data;

  // Timeout Bitmap
  rsi_uint32_to_4bytes(rsi_timeout->timeout_bitmap, timeout_bitmap);

  // Timeout value
  rsi_uint16_to_2bytes(rsi_timeout->timeout_value, timeout_value);

#ifndef RSI_WLAN_SEM_BITMAP
  rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif

  // send configure timeout command
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_TIMEOUT, pkt);
  // wait on wlan semaphore
  rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_TIMEOUT_RESPONSE_WAIT_TIME);
  // get wlan/network command response status
  status = rsi_wlan_get_status();
  // Return the status

  return status;
}
/*==============================================*/
/**
 * @brief      Scan surrounding access points and post scan response. You can call this API to get the scan results. This is a blocking API. 
 * @param[in]  ssid            - SSID of an access point to connect. \n SSID should be less than or equal to 32 bytes. \n
 *                               Note: SSID is a null terminated string.                          
 * @param[in]  chno            - Channel number of the access point.
 * @param[out]  result         - Buffer address provided by the application to fill the scan response.
 * @param[in]  length          - Length of the resulting buffer measured in bytes to hold scan results.\n
 *                               Size of structure \ref rsi_rsp_scan_t is 54 bytes.\n
 *                               Length for storing one scan result is sizeof( \ref rsi_scan_info_t), which is 44 bytes. \n
 *                               Maximum of 11 scan results will be returned by the module, \n 
 *                               in this case, length should be configured as 8 + 11*sizeof( \ref rsi_scan_info_t).
 * @param[in]  scan_bitmap     - Scan bitmap options. \n
 *                               BIT[0] (QUICK SCAN feature) - It is valid only if channel number and ssid is given. \n
 *                               BIT[1] (SCAN RESULTS TO HOST) - When it is enabled additional scan results are given to host. \n 
 *                               After getting scan results, host has to issue another scan request by disabling this bit in scan feature bitmap in same API \n
 *                               or call \ref rsi_wlan_scan API before issuing join command.                        
 * @return     0                -    Success \n
 * @return     Non-Zero Value   -    Failure
 * @note       **Precondition** - \ref rsi_wireless_init() API needs to be called before this API.
 *
 */
int32_t rsi_wlan_scan_with_bitmap_options(int8_t *ssid,
                                          uint8_t chno,
                                          rsi_rsp_scan_t *result,
                                          uint32_t length,
                                          uint32_t scan_bitmap)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_SCAN_BITMAP_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE || wlan_cb->state >= RSI_WLAN_STATE_CONNECTED) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_SCAN_BITMAP_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Attach the buffer given by user
    wlan_cb->app_buffer = (uint8_t *)result;

    // Length of the buffer provided by user
    wlan_cb->app_buffer_length = length;

    // Call async scan to perform actual scan
    status = rsi_wlan_scan_async_with_bitmap_options(ssid, chno, scan_bitmap, NULL);

    if (status != RSI_SUCCESS) {
      // Return status if error in sending command occurs
      SL_PRINTF(SL_WLAN_SCAN_BITMAP_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR);
      return status;
    }

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_SCAN_WITH_BITMAP_RESPONSE_WAIT_TIME);

    // Change WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_SCAN_BITMAP_WLAN_COMMAND_ERROR, WLAN, LOG_INFO, "status: %4x", status);

    return status;
  }
  // Return status
  SL_PRINTF(SL_WLAN_SCAN_BITMAP_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief               Scan available access points and post scan response to application.
 *                      Application must call this API to get the scan results.
 *                      This is a non-blocking API.
 * @pre                 \ref rsi_wireless_init() API needs to be called before this API.
 * @param[in]   ssid     - SSID of an access point to connect. \n
 *                        SSID should be less than or equal to 32 bytes.
 * @param[in]   chno     - Channel number of the access point.
 * @param[in]   bitmap   - Scan feature bitmap \n
 *                         BIT[0] (QUICK SCAN feature) - It is valid only if channel number and ssid is given. \n
 *                         BIT[1] (SCAN RESULTS TO HOST) - When it is enabled additional scan results
 *                         are given to host. \n     After getting scan results, host has to issue another
 *                         scan request by disabling this bit in scan feature bitmap in same api
 *                         or call \ref rsi_wlan_scan api \n     before issuing join command.
 * @param[out]  status   - Response status
 * @param[out]  buffer   - Buffer address provided by the application to fill the scan response.
 * @param[out]  length   - Length of the response buffer measured
 *                        in bytes to hold scan results. \n Size of structure 
 *                        \ref rsi_rsp_scan_t is 54 bytes.
 *                        Length for storing one scan result is
 *                        sizeof( \ref rsi_scan_info_t), which is 46 bytes. \n
 *                        Maximum of 11 scan results will be returned by the
 *                        module, \n in this case, length should be configured
 *                        as 8 + 11*sizeof( \ref rsi_scan_info_t).
 * @return      0              -   Success \n
 *              Non-Zero Value -   Failure
 *
 */

int32_t rsi_wlan_scan_async_with_bitmap_options(int8_t *ssid,
                                                uint8_t chno,
                                                uint32_t bitmap,
                                                void (*scan_response_handler)(uint16_t status,
                                                                              const uint8_t *buffer,
                                                                              const uint16_t length))

{
  rsi_pkt_t *pkt = NULL;
  rsi_req_scan_t *scan;
  int32_t status = RSI_SUCCESS;
  int32_t allow  = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_ENTRY, WLAN, LOG_INFO);

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

#ifdef PROCESS_SCAN_RESULTS_AT_HOST
  if (bitmap & RSI_SCAN_RESULTS_TO_HOST) {
    memset(scan_results_array, 0, sizeof(struct wpa_scan_results_arr));
    rsi_wlan_cb_non_rom->scan_results_to_host = 1;
  }
#endif

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE || wlan_cb->state >= RSI_WLAN_STATE_CONNECTED) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  if (scan_response_handler != NULL) {
    allow = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  }
  if (allow == RSI_SUCCESS) {
    switch (wlan_cb->state) {
      case RSI_WLAN_STATE_OPERMODE_DONE: {
        if (wlan_cb->field_valid_bit_map & RSI_SET_MAC_BIT) {
          // Allocate command buffer from WLAN pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
          // If allocation of packet fails
          if (pkt == NULL) {
            // Change WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // Return packet allocation failure error
            SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_PKT_ALLOCATION_FAILURE_1, WLAN, LOG_ERROR);
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }

          // Memset data
          memset(&pkt->data, 0, sizeof(rsi_req_mac_address_t));

          // Memcpy data
          memcpy(&pkt->data, wlan_cb->mac_address, sizeof(rsi_req_mac_address_t));

#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // Send set MAC command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_MAC_ADDRESS, pkt);

          // Wait on WLAN semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_MAC_RESPONSE_WAIT_TIME);

          // Get WLAN/network command response status
          status = rsi_wlan_get_status();

          if (status != RSI_SUCCESS) {
            // Change WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

            // Return status if error in sending command occurs
            SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_ERROR_IN_SENDING_COMMAND_1, WLAN, LOG_ERROR);
            return status;
          }
        }
#if HE_PARAMS_SUPPORT
        status = rsi_wlan_11ax_config(GUARD_INTERVAL);
        if (status != RSI_SUCCESS) {
          return status;
        }
#endif

        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_PKT_ALLOCATION_FAILURE_2, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset data
        memset(&pkt->data, 0, sizeof(rsi_req_band_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send band command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_BAND, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_BAND_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_ERROR_IN_SENDING_COMMAND_2, WLAN, LOG_ERROR);
          return status;
        }
      }
        //  No break
      // Fall through
      case RSI_WLAN_STATE_BAND_DONE: {

#if RSI_TIMEOUT_SUPPORT

        status = send_timeout(RSI_TIMEOUT_BIT_MAP, RSI_TIMEOUT_VALUE);
        if (status != RSI_SUCCESS) {
          // Change WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_ERROR_IN_SENDING_COMMAND_3, WLAN, LOG_ERROR);
          return status;
        }
#endif
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
        // If allocation of packet fails
        if (pkt == NULL) {
          // Change WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_PKT_ALLOCATION_FAILURE_4, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif

        // Send init command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_INIT, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_INIT_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_ERROR_IN_SENDING_COMMAND_4, WLAN, LOG_ERROR);
          return status;
        }
#if RSI_SET_REGION_SUPPORT
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_PKT_ALLOCATION_FAILURE_5, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset data
        memset(&pkt->data, 0, sizeof(rsi_req_set_region_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif

        // Send set region command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_REGION, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_REGION_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_ERROR_IN_SENDING_COMMAND_5, WLAN, LOG_ERROR);
          return status;
        }
#endif
#if RSI_SET_REGION_AP_SUPPORT
        if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE) {
          // allocate command buffer  from wlan pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

          // If allocation of packet fails
          if (pkt == NULL) {
            //Changing the wlan cmd state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // return packet allocation failure error
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }
          // Memset data
          memset(&pkt->data, 0, sizeof(rsi_req_set_region_ap_t));

#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // send set region AP command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_REGION_AP, pkt);

          // wait on wlan semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_REGION_AP_RESPONSE_WAIT_TIME);

          // get wlan/network command response status
          status = rsi_wlan_get_status();

          if (status != RSI_SUCCESS) {
            //Changing the wlan cmd state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

            // Return the status if error in sending command occurs
            return status;
          }
        }
#endif
      }
        // No break
      // Fall through
      case RSI_WLAN_STATE_SCAN_DONE:
      case RSI_WLAN_STATE_INIT_DONE:
      case RSI_WLAN_STATE_CONNECTED: {
#if RSI_WLAN_CONFIG_ENABLE
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_PKT_ALLOCATION_FAILURE_6, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif

        // Send Configuration request command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_CONFIG, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_WLAN_CONFIG_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_ERROR_IN_SENDING_COMMAND_6, WLAN, LOG_ERROR);
          return status;
        }
#endif
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_PKT_ALLOCATION_FAILURE_7, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Fill SCAN parameters
        scan = (rsi_req_scan_t *)pkt->data;

        // Memset the PKT
        memset(&pkt->data, 0, sizeof(rsi_req_scan_t));

        // Copy the channel number
        rsi_uint32_to_4bytes(scan->channel, chno);

        // Copy SSID given by the application
        if (ssid != NULL) {
          rsi_strcpy(scan->ssid, ssid);
        }

        scan->scan_feature_bitmap = bitmap;

        if (scan_response_handler != NULL) {
          // Register scan response handler
          rsi_wlan_cb_non_rom->callback_list.wlan_scan_response_handler = scan_response_handler;
        } else {
#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        }

        // Send scan command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SCAN, pkt);
      } break;
      case RSI_WLAN_STATE_NONE:
      case RSI_WLAN_STATE_IP_CONFIG_DONE:
      case RSI_WLAN_STATE_IPV6_CONFIG_DONE:
      case RSI_WLAN_STATE_AUTO_CONFIG_GOING_ON:
      case RSI_WLAN_STATE_AUTO_CONFIG_DONE:
      case RSI_WLAN_STATE_AUTO_CONFIG_FAILED:
        break;

      default: {
        // Return status if command given in driver is an invalid state
        status = RSI_ERROR_COMMAND_NOT_SUPPORTED;
        if (scan_response_handler == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
        }
      } break;
    }
  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR);
    return allow;
  }
  SL_PRINTF(SL_WLAN_SCAN_ASYNC_BITMAP_EXIT, WLAN, LOG_INFO, "status: %4x", status);

  return status;
}

/*==============================================*/
/**
 * @brief      Scan the surrounding access points. Invokes rsi_wlan_scan_async() API, which is an asynchronous call. This is a blocking API.
 * @pre		     \ref rsi_wireless_init() API needs to be called before this API.
 * @param[in]  ssid    - SSID size should be less than or equal to 32 bytes. Note: SSID is a null terminated string.
 * @param[in]  chno    - Channel number to perform scan. If 0, then the module will scan all the channels.
 * @param[out]  result - Scanned Wi-Fi networks information. \n
 *                       This output contains a maximum of 11 scan results \n
 *                       The structure \ref rsi_rsp_scan_t contains members scan_count, which specifies the number of scan \n
 *                       results followed by an array of structure type \ref rsi_scan_info_t, where each array element contains \n
 *                       information about each network scanned.
 * @param[in]  length  - Size that should be allocated to buffer that will store scan results. \n
 *             ## Channels supported in 2.4 GHz Band ##
 *         		 Channel numbers       	 |	chno
 * 		         :-----------------------|:--------------------------------------------------------------------------------------------------
 * 	           All Channels	       	   |       0	
 * 	         	 1			                 |       1	
 * 		         2			                 |       2	
 * 		         3	                     |       3	
 * 		         4			                 |       4	
 * 		         5			                 |       5	
 * 		         6			                 |       6	
 * 	     	     7			                 |       7	
 * 		         8			                 |       8	
 * 		         9			                 |       9	
 * 		         10                      |       10	
 * 		         11			                 |       11	
 * 		         12                      |       12	
 *		         13			                 |       13	
 * 		         14			                 |       14	
 *             #### Channels supported in 5 GHz Band ####
 * 		         Channel numbers         |	chno
 * 		         :-----------------------|:--------------------------------------------------------------------------------------------------
 * 		         All Channels		         |       0	
 * 		         36	        	           |       36
 * 		         40	        	           |       40
 * 		         44	        	           |       44
 * 		         48	        	           |       48
 *		         149	                   |       149
 * 		         153		                 |       153
 * 		         157		                 |       157
 * 		         161        		         |       161
 *		         165	         	         |       165
 *             #### DFS Channels supported in 5 GHz Band ####
 * 		         Channel numbers         |	chno
 * 		         :-----------------------|:--------------------------------------------------------------------------------------------------
 * 		         52(DFS)	        	     |       52
 * 		         56(DFS)	        	     |       56
 * 		         60(DFS)	        	     |       60
 * 		         64(DFS)	        	     |       64
 * 		         100(DFS)	               |       100
 * 		         104(DFS)	               |       104
 * 		         108(DFS)	               |       108
 * 		         112(DFS)	               |       112
 * 		         116(DFS)	               |       116
 * 		         120(DFS)	               |       120
 * 		         124(DFS)	               |       124
 * 		         128(DFS)	               |       128
 * 		         132(DFS)                |       132
 * 		         136(DFS)	          	   |       136
 * 		         140(DFS)         		   |       140
 * 		         144(DFS)         		   |       144
 *             #### Channels supported in 4.9 GHz Band ####
 * 		         Channel numbers         |	chno
 * 		         :-----------------------|:--------------------------------------------------------------------------------------------------
 * 		         All Channels		         |       0	
 * 		         184	        	         |       184
 * 		         188	        	         |       188
 * 		         192	        	         |       192
 * 		         196	        	         |       196
 * 		         8	                     |       8
 * 		         12	                     |       12
 * 		         16	                     |       16
 * @return     0              - Success
 * @return     Non-Zero Value - Failure (**Possible Error Codes** - 0xfffffffe,0xfffffffd,0xfffffffa,0x0002, 0x0003, 0x0005, 0x000A, 0x0014, 0x0015, 0x001A, 0x0021,0x0024,0x0025,0x0026,0x002C,0x003c) \n
 * 	          ##	Scan Info structure ##
 * 		        Structure Fields        |	Description	
 * 		        :-----------------------|:--------------------------------------------------------------------------------------------------
 * 	 	         rf_channel	            |      Access point channel number
 * 	 	         security_mode	        |      Security modes
 *             ^                             0 : Open
 *	           ^        	                 	 1 : WPA
 *	           ^        	                	 2 : WPA2
 *	           ^        	                	 3 : WEP
 *	           ^        	                	 4 : WPA Enterprise
 *	           ^        	                	 5 : WPA2 Enterprise
 *	           ^                             6 : WPA3
 * 	 	         rssi_val	              |       RSSI value of the Access Point
 * 	 	         network_type	          |       This is the type of the network
 * 	 	         ^            	                1 : Infrastructure mode	
 * 	 	         ssid		                |       SSID of the access point
 * 	 	         bssid		              |       MAC address of the access point
 * @note       **Precondition** - \ref rsi_wireless_init() API needs to be called before this API.   
 * @note       Refer to \ref error-codes for the description of above error codes.    
 * @note       To set various timeouts, user should change the following macros in rsi_wlan_config.h \n
 *      		   #define RSI_TIMEOUT_SUPPORT RSI_ENABLE \n
 *      		   #define RSI_TIMEOUT_BIT_MAP 2 \n
 *      		   #define RSI_TIMEOUT_VALUE 150  (This macro can be configured based on user requirement) \n
 *        		timeout_bitmap        	|       Description	
 * 		        :-----------------------|:--------------------------------------------------------------------------------------------------
 * 		        timeout_bitmap[0]     	|	      Set timeout for association and authentication request.timeout_value : timeout value in ms(default 300ms).
 * 		        timeout_bitmap[1]     	|       Sets each channel active scan time in ms (default 100ms)
 * 		        timeout_bitmap[2] 	    |       Used for WLAN keep alive timeout(default value is 30s)
 * 		        timeout_bitmap[31-3]    |       Reserved
 *
 *
 */
int32_t rsi_wlan_scan(int8_t *ssid, uint8_t chno, rsi_rsp_scan_t *result, uint32_t length)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_SCAN_ENTRY, WLAN, LOG_INFO);

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE || wlan_cb->state > RSI_WLAN_STATE_IP_CONFIG_DONE) {
    // Command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Attach the buffer given by user
    wlan_cb->app_buffer = (uint8_t *)result;

    // Length of the buffer provided by user
    wlan_cb->app_buffer_length = length;

    //Ccall async scan to perform actual scan
    status = rsi_wlan_scan_async(ssid, chno, NULL);

    if (status != RSI_SUCCESS) {

      // Return status if error in sending command occurs
      SL_PRINTF(SL_WLAN_SCAN_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
      return status;
    }

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_SCAN_RESPONSE_WAIT_TIME);
    // Get WLAN/network command response status
    status = rsi_wlan_get_status();

    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_SCAN_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_WLAN_SCAN_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}
/*==============================================*/
/**
 * @brief      Scan the surrounding access points. A scan response handler is registered with it to get the response for scan. This is a non-blocking API. 
 * @param[in]  ssid		        			 - SSID to scan. If this input parameter is present, module will scan for that particular SSID only. SSID size should be less than or equal to 32 bytes \n
 *                                     Note: SSID is a null terminated string.
 * @param[in]  chno 				         - Channel number to perform scan, if 0, then module will scan in all channels.
 * @param[in]  Scan_response_handler - Callback tht is called when the response for scan is received from the module. \n
 *				                             The parameters involved are status, buffer, & length. \n
 *@param[out]	 Status                - Response status.
 *@param[out]	 Buffer                - Response buffer \n
 *@param[out]	 Length                - Length of the response buffer measured in bytes to hold scan results. \n Size of structure \ref rsi_rsp_scan_t is 54 bytes. \n
 *					                           Length for storing one scan result is sizeof(rsi_scan_info_t) which is 44 bytes. \n
 *						                         Maximum of 11 scan results will be returned by the module, in this case, length must be configured as 8 + 11*sizeof(rsi_scan_info_t).      
 *             ## Channels supported in 2.4 GHz Band ##
 *         		 Channel numbers       	 |	chno
 * 		         :-----------------------|:--------------------------------------------------------------------------------------------------
 * 	           All Channels	       	   |       0	
 * 	         	 1			                 |       1	
 * 		         2			                 |       2	
 * 		         3	                     |       3	
 * 		         4			                 |       4	
 * 		         5			                 |       5	
 * 		         6			                 |       6	
 * 	     	     7			                 |       7	
 * 		         8			                 |       8	
 * 		         9			                 |       9	
 * 		         10                      |       10	
 * 		         11			                 |       11	
 * 		         12                      |       12	
 *		         13			                 |       13	
 * 		         14			                 |       14	
 *             #### Channels supported in 5 GHz Band ####
 * 		         Channel numbers         |	chno
 * 		         :-----------------------|:--------------------------------------------------------------------------------------------------
 * 		         All Channels		         |       0	
 * 		         36	        	           |       36
 * 		         40	        	           |       40
 * 		         44	        	           |       44
 * 		         48	        	           |       48
 * 		         100	                   |       100
 * 		         104	                   |       104
 * 		         108	                   |       108
 * 		         116	                   |       116
 * 		         132                     |       132
 * 		         136	          	       |       136
 * 		         140         		         |       140
 *		         149	                   |       149
 * 		         153		                 |       153
 * 		         157		                 |       157
 * 		         161        		         |       161
 *		         165	         	         |       165
 * @return     0              -  Success \n
 * @return     Non-Zero	Value - Failure (**Possible Error Codes** - 0xfffffffe,0xfffffffd,0xfffffffa,0x0002, 0x0003, 0x0005, 0x000A, 0x0014, 0x0015, 0x001A, 0x0021,0x0024,0x0025,0x0026,0x002C,0x003c) \n
 * 	           #### Scan Response structure format ####
 * 		         Structure Fields        |	Description	
 * 		         :-----------------------|:--------------------------------------------------------------------------------------------------
 * 	 	         scan_count	             |      Number of access points scanned.
 * 	 	         ^                              scan_count[0] Contains the scan count.
 * 	 	         ^                              scan_count[3-1] are reserved.
 * 	 	         scan_info               |      Information about scanned Access points in rsi_scan_info_t structure.
 * 	           ##	Scan Info - rsi_scan_info_t structure ##
 * 		         Structure Fields        |	Description	
 * 		         :-----------------------|:--------------------------------------------------------------------------------------------------
 * 	 	         rf_channel	             |      Access point channel number
 * 	 	         security_mode	         |      Security mode0 : Open
 *	           ^        	                  	1 . WPA
 *	           ^        	                  	2 . WPA2
 *	           ^        	                  	3 . WEP
 *	           ^        	                  	4 . WPA Enterprise
 *	           ^        	                  	5 . WPA2 Enterprise
 *	           ^        	                  	7 . WPA3
 * 	 	         rssi_val	               |      RSSI value of the Access Point
 * 	 	         network_type	           |      Type of network
 * 	 	         ^            	                1 . Infrastructure mode	
 * 	 	         ssid		                 |      SSID of the access point
 * 	 	         bssid		               |      MAC address of the access point
 * @note       **Precondition** - \ref rsi_wireless_init() API needs to be called before this API.
 * @note	     If status is Success , the argument buffer passed to scan_response_handler \n 
 * 			       holds scan results in \ref rsi_rsp_scan_t structure format.
 * @note       To set various timeouts, user should change the following macros in rsi_wlan_config.h \n
 *			       #define RSI_TIMEOUT_SUPPORT RSI_ENABLE \n
 *			       #define RSI_TIMEOUT_BIT_MAP 4 \n
 *			       #define RSI_TIMEOUT_VALUE 300
 *
 *        	   timeout_bitmap          |       Description	
 * 		         :-----------------------|:--------------------------------------------------------------------------------------------------
 * 		         timeout_bitmap[0]     	 |	     Set timeout for association and authentication request.timeout_value : timeout value in ms(default 300ms).
 * 		         timeout_bitmap[1]     	 |       Sets each channel active scan time in ms (default 100ms)
 * 		         timeout_bitmap[2] 	     |       Used for WLAN keep alive timeout(default value is 30s)
 * 		         timeout_bitmap[31-3]    |       Reserved
 *
 */

int32_t rsi_wlan_scan_async(int8_t *ssid,
                            uint8_t chno,
                            void (*scan_response_handler)(uint16_t status,
                                                          const uint8_t *buffer,
                                                          const uint16_t length))

{
  rsi_pkt_t *pkt;
  rsi_req_scan_t *scan;
  int32_t status = RSI_SUCCESS;
  int32_t allow  = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_SCAN_ASYNC_ENTRY, WLAN, LOG_INFO);

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE || wlan_cb->state > RSI_WLAN_STATE_IP_CONFIG_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_SCAN_ASYNC_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  if (scan_response_handler != NULL) {
    allow = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  }
  if (allow == RSI_SUCCESS) {

    switch (wlan_cb->state) {
      case RSI_WLAN_STATE_OPERMODE_DONE: {
        if (wlan_cb->field_valid_bit_map & RSI_SET_MAC_BIT) {
          // Allocate command buffer from WLAN pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

          // If allocation of packet fails
          if (pkt == NULL) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // Return packet allocation failure error
            SL_PRINTF(SL_WLAN_SCAN_ASYNC_PKT_ALLOCATION_FAILURE_1, WLAN, LOG_ERROR, "status: %4x", status);
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }

          // Memset data
          memset(&pkt->data, 0, sizeof(rsi_req_mac_address_t));

          // Memcpy data
          memcpy(&pkt->data, wlan_cb->mac_address, sizeof(rsi_req_mac_address_t));

#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // Send set MAC command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_MAC_ADDRESS, pkt);

          // Wait on WLAN semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_MAC_RESPONSE_WAIT_TIME);

          // Get WLAN/network command response status
          status = rsi_wlan_get_status();

          if (status != RSI_SUCCESS) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

            // Return status if error in sending command occurs
            SL_PRINTF(SL_WLAN_SCAN_ASYNC_ERROR_IN_SENDING_COMMAND_1, WLAN, LOG_ERROR, "status: %4x", status);
            return status;
          }
        }
#if HE_PARAMS_SUPPORT
        status = rsi_wlan_11ax_config(GUARD_INTERVAL);
        if (status != RSI_SUCCESS) {
          return status;
        }
#endif
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_PKT_ALLOCATION_FAILURE_2, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset data
        memset(&pkt->data, 0, sizeof(rsi_req_band_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send band command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_BAND, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_BAND_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_ERROR_IN_SENDING_COMMAND_2, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }
      }
        // No break
      // Fall through
      case RSI_WLAN_STATE_BAND_DONE: {
#if RSI_TIMEOUT_SUPPORT
        status = send_timeout(RSI_TIMEOUT_BIT_MAP, RSI_TIMEOUT_VALUE);
        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_ERROR_IN_SENDING_COMMAND_3, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }
#endif
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_PKT_ALLOCATION_FAILURE_3, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send init command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_INIT, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_INIT_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          return status;
        }
#if RSI_SET_REGION_SUPPORT
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_PKT_ALLOCATION_FAILURE_4, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset data
        memset(&pkt->data, 0, sizeof(rsi_req_set_region_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif

        // Send set region command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_REGION, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_REGION_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_ERROR_IN_SENDING_COMMAND_4, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }
#endif
#if RSI_SET_REGION_AP_SUPPORT
        if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE) {
          // allocate command buffer  from wlan pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

          // If allocation of packet fails
          if (pkt == NULL) {
            //Changing the wlan cmd state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // return packet allocation failure error
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }
          // Memset data
          memset(&pkt->data, 0, sizeof(rsi_req_set_region_ap_t));

#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // send set region AP command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_REGION_AP, pkt);

          // wait on wlan semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_REGION_AP_RESPONSE_WAIT_TIME);

          // get wlan/network command response status
          status = rsi_wlan_get_status();

          if (status != RSI_SUCCESS) {
            //Changing the wlan cmd state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

            // Return the status if error in sending command occurs
            return status;
          }
        }
#endif
      }
        // No break
      // Fall through
      case RSI_WLAN_STATE_SCAN_DONE:
      case RSI_WLAN_STATE_INIT_DONE:
      case RSI_WLAN_STATE_CONNECTED:
      case RSI_WLAN_STATE_IP_CONFIG_DONE: {
#if RSI_WLAN_CONFIG_ENABLE
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_PKT_ALLOCATION_FAILURE_5, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send Configuration request command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_CONFIG, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_WLAN_CONFIG_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_ERROR_IN_SENDING_COMMAND_5, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }
#endif
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SCAN_ASYNC_PKT_ALLOCATION_FAILURE_6, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Fill SCAN parameters
        scan = (rsi_req_scan_t *)pkt->data;

        // Memset the PKT
        memset(&pkt->data, 0, sizeof(rsi_req_scan_t));

        // Copy the channel number
        rsi_uint32_to_4bytes(scan->channel, chno);

        // Copy SSID given by the application
        if (ssid != NULL) {
          rsi_strcpy(scan->ssid, ssid);
        }

        if (scan_response_handler != NULL) {
          // Register scan response handler
          rsi_wlan_cb_non_rom->callback_list.wlan_scan_response_handler = scan_response_handler;
        } else {
#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        }
        // Send scan command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SCAN, pkt);
      } break;
      case RSI_WLAN_STATE_NONE:
      case RSI_WLAN_STATE_IPV6_CONFIG_DONE:
      case RSI_WLAN_STATE_AUTO_CONFIG_GOING_ON:
      case RSI_WLAN_STATE_AUTO_CONFIG_DONE:
      case RSI_WLAN_STATE_AUTO_CONFIG_FAILED:
        break;
      default: {
        // Return status if command given in driver is an invalid state
        status = RSI_ERROR_COMMAND_NOT_SUPPORTED;
        if (scan_response_handler == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
        }
      } break;
    }
  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_SCAN_ASYNC_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return allow;
  }
  SL_PRINTF(SL_WLAN_SCAN_ASYNC_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief       Connect to the specified Wi-Fi network. This is a blocking API.
 * @param[in]   ssid		- SSID of an access point to connect. SSID should be less than or equal to 32 bytes.
 * @param[in]   sec_type 	- Security type of the access point to connect. \n
 * 		Value	|	Security Type
 * 		:-----------------------|:--------------------------------------------------------------------------------------------------
 * 		0		|	RSI_OPEN 
 * 		1		|	RSI_WPA
 * 		2		|	RSI_WPA2
 * 		3		|	RSI_WEP
 * 		4		|	RSI_WPA_EAP
 * 		5		|	RSI_WPA2_EAP
 * 		6		|	RSI_WPA_WPA2_MIXED
 * 		7		|	RSI_WPA_PMK
 * 		8		|	RSI_WPA2_PMK
 * 		9		|	RSI_WPS_PIN
 * 		10	|	RSI_USE_GENERATED_WPSPIN
 * 		11	|	RSI_WPS_PUSH_BUTTON
 * 		12	|	RSI_WPA_WPA2_MIXED_PMK
 * 		13	|	RSI_WPA3
 * @param[in]   secret_key	- Pointer to a buffer that contains security information based on sec_type. \n
 * 		Security type(sec_type)	|	Secret key structure format (secret_key)
 * 		:-----------------------|:--------------------------------------------------------------------------------------------------
 * 		RSI_OPEN		|	No secret key in open security mode.
 * 		RSI_WPA			|	PSK string terminated with NULL. Length of PSK should be at least 8 and less than 64 bytes.
 * 		RSI_WPA2		|	PSK string terminated with NULL. Length of PSK should be at least 8 and less than 64 bytes.
 * 		RSI_WEP			|	WEP keys should be in the following format:
 * 		^				typedef struct rsi_wep_keys_s
 * 		^				{ 
 * 		^				uint8_t index[2];
 * 		^				uint8_t key[4][32];
 * 		^				} rsi_wep_keys_t; 
 * 		^				index: WEP key index to use for TX packet encryption.
 * 		^				key: 4 WEP keys, last three WEP keys are optional. If only first WEP key is valid then index should be 0.
 * 		RSI_WPA_EAP		|	Enterprise credentials should be in the following format:
 * 		^				typedef struct rsi_eap_credentials_s
 * 		^				{ 
 * 		^				uint8_t username[64];
 * 		^				uint8_t password[128];
 * 		^				} rsi_eap_credentials_t;
 *		^				username: username to be used in enterprise
 * 		^				password: password for the given username
 * 		RSI_WPA2_EAP		|	Enterprise credentials should be in the following format:
 * 		^				typedef struct rsi_eap_credentials_s
 * 		^				{ 
 * 		^				uint8_t username[64];
 * 		^				uint8_t password[128];
 * 		^				} rsi_eap_credentials_t;
 * 		^				username: username to be used in enterprise
 * 		^				password: password for a given username.
 * 		RSI_WPA_WPA2_MIXED	|	PSK string terminated with NULL. Length of PSK should be at least 8 and less than 64 bytes
 * 		RSI_WPA_PMK		|	PMK string, should be 32 bytes in length
 * 		RSI_WPA2_PMK		|	PMK string, should be 32 bytes in length
 * 		RSI_WPS_PIN		|	8-byte WPS PIN
 * 		RSI_USE_GENERATED_WPSPIN|	NULL string indicate to use PIN generated using rsi_wps_generate_pin API
 * 		RSI_WPS_PUSH_BUTTON	|	NULL string indicate to generate push button event
 * 		RSI_WPA3            |   PSK string terminated with NULL. Length of PSK should be at least 8 and less than 64 bytes.
 * @return     0              -  Success \n
 * @return     Non-Zero	Value - Failure (**Possible Error Codes** - 0xfffffffe,0xfffffffd,0xfffffffa,0x0002,0x0003,0x0005,0x0008,0x0009,0x000A,0x000E,0x0014, \n
 *                                                                  0x0015,0x0016,0x0019,0x001A,0x001E,0x0020,0x0021,0x0024,0x0025,0x0026,0x0028,0x0039,0x003C, \n
 *                                                                  0x0044,0x0045,0x0046,0x0047,0x0048,0x0049,0xFFF8) \n
 * @note       **Precondition** - \ref rsi_wireless_init() API needs to be called before this API.
 * @note       Refer to \ref error-codes for the description of above error codes.
 * @note       To set various timeouts, user should change the following macros in rsi_wlan_config.h \n
 *			       #define RSI_TIMEOUT_SUPPORT RSI_ENABLE. \n
 *			       #define RSI_TIMEOUT_BIT_MAP 4. \n
 *			       #define RSI_TIMEOUT_VALUE 300. \n
 *
 *        	   timeout_bitmap          |       Description	
 * 		         :-----------------------|:--------------------------------------------------------------------------------------------------
 * 		         timeout_bitmap[0]     	 |	     Set timeout for association and authentication request.timeout_value : timeout value in ms(default 300ms).
 * 		         timeout_bitmap[1]     	 |       Sets each channel active scan time in ms (default 100ms)
 * 		         timeout_bitmap[2] 	     |       Used for WLAN keep alive timeout(default value is 30s)
 * 		         timeout_bitmap[31-3]    |       Reserved
 * @note	      To set Rejoin parameters, user should configure the following macros in rsi_wlan_config.h \n 
 *        	      Macro                          |       Description	
 * 		            :------------------------------|:--------------------------------------------------------------------------------------------------
 *		            RSI_REJOIN_PARAMS_SUPPORT      | Enable to send rejoin parameters command during Wi-Fi client connection \n
 *		            RSI_REJOIN_MAX_RETRY           | Number of maximum rejoin retries by module\n
 *		            note                           | If Max retries is 0 , retries infinity times \n
 *		            RSI_REJOIN_SCAN_INTERVAL       | Periodicity of rejoin attempt \n
 *		            RSI_REJOIN_BEACON_MISSED_COUNT | Number of consecutive beacon misses after which modules goes to unconnected state \n
 *		            RSI_REJOIN_FIRST_TIME_RETRY    | ENABLE  - Try to rejoin in the first attempt after join failure. \n
 *		                                             DISABLE - Try to rejoin based on maximum rejoin retries configured. \n
 * @note	      When RSI_REJOIN_PARAMS_SUPPORT is enabled in the rsi_wlan_config.h, the rejoin frame will be sent to the firmware after the scan is done.
 * @note        This API internally handles following commands based on wlan_cb state and sends the next command and finally sends the join command. \n
 *              Set MAC address, Band, Timeout, Init, Set region, WMM parameters, Scan, EAP config, WMM PS parameters, WPS method, Set WEP keys, Host PSK, Rejoin params, Join \n
 * @note        For example, \n
 *              after calling rsi_wireless_init(), wlan_cb state is updated to OPERMODE_DONE state. So when we call this API, it will execute band, init, scan and join commands. \n
 * @note        For example, \n
 *              after calling rsi_wlan_disconnect() or else after rejoin failure, wlan_cb state is updated to BAND_DONE state. So when we call this API, it will execute init, scan and join commands. \n
 * @note        For example, \n
 *              after calling rsi_wlan_scan()/ rsi_wlan_scan_with_bitmap_options() API, wlan_cb state is updated to SCAN_DONE state. So when we call this API, it will execute join command directly.
 *
 */

int32_t rsi_wlan_connect(int8_t *ssid, rsi_security_mode_t sec_type, void *secret_key)
{

  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_CONNECT_ENTRY, WLAN, LOG_INFO);
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Call async connect to call actual connect
    status = rsi_wlan_connect_async(ssid, sec_type, secret_key, NULL);

    if (status != RSI_SUCCESS) {
      // Return status if error in sending command occurs
      SL_PRINTF(SL_WLAN_CONNECT_ERROR_IN_SENDING_COMMAND_1, WLAN, LOG_ERROR, "status: %4x", status);
      return status;
    }

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_CONNECT_RESPONSE_WAIT_TIME);

    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();

    if (status != RSI_SUCCESS) {
      // Return status if error in sending command occurs
      SL_PRINTF(SL_WLAN_CONNECT_ERROR_IN_SENDING_COMMAND_2, WLAN, LOG_ERROR, "status: %4x", status);
      return status;
    }

  } else {
    // Return WLAN command error
    return status;
  }
  // Execute post connect commands
  status = rsi_wlan_execute_post_connect_cmds();

  status = rsi_wlan_get_status();
  SL_PRINTF(SL_WLAN_CONNECT_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}
/*==============================================*/
/**
 * @brief       Connect to the specified Wi-Fi network. A join response handler is registered to get the response for join. This is a non-blocking API.
 * @param[in]   ssid - SSID of an access point to connect. SSID should be less than or equal to 32 bytes.
 * @param[in]   sec_type 	- Security type of the access point to connect. \n
 * 		Value	|	Security Type
 * 		:-----------------------|:--------------------------------------------------------------------------------------------------
 * 		0		|	RSI_OPEN 
 * 		1		|	RSI_WPA
 * 		2		|	RSI_WPA2
 * 		3		|	RSI_WEP
 * 		4		|	RSI_WPA_EAP
 * 		5		|	RSI_WPA2_EAP
 * 		6		|	RSI_WPA_WPA2_MIXED
 * 		7		|	RSI_WPA_PMK
 * 		8		|	RSI_WPA2_PMK
 * 		9		|	RSI_WPS_PIN
 * 		10	|	RSI_USE_GENERATED_WPSPIN
 * 		11	|	RSI_WPS_PUSH_BUTTON
 * 		12	|	RSI_WPA_WPA2_MIXED_PMK
 * 		13	|	RSI_WPA3
 * @param[in]   secret_key	- Pointer to a buffer that contains security information based on sec_type. \n
 * 		Security type(sec_type)	|	Secret key structure format (secret_key)
 * 		:-----------------------|:--------------------------------------------------------------------------------------------------
 * 		RSI_OPEN		|	No secret key in open security mode.
 * 		RSI_WPA			|	PSK string terminated with NULL. Length of PSK should be at least 8 and less than 64 bytes.
 * 		RSI_WPA2		|	PSK string terminated with NULL. Length of PSK should be at least 8 and less than 64 bytes.
 * 		RSI_WEP			|	WEP keys should be in the following format:
 * 		^				typedef struct rsi_wep_keys_s
 * 		^				{ 
 * 		^				uint8_t index[2];
 * 		^				uint8_t key[4][32];
 * 		^				} rsi_wep_keys_t; 
 * 		^				index: WEP key index to use for TX packet encryption.
 * 		^				key: 4 WEP keys, last three WEP keys are optional. If only first WEP key is valid then index should be 0.
 * 		RSI_WPA_EAP		|	Enterprise credentials should be in the following format:
 * 		^				typedef struct rsi_eap_credentials_s
 * 		^				{ 
 * 		^				uint8_t username[64];
 * 		^				uint8_t password[128];
 * 		^				} rsi_eap_credentials_t;
 *		^				username: username to be used in enterprise
 * 		^				password: password for the given username
 * 		RSI_WPA2_EAP		|	Enterprise credentials should be in the following format:
 * 		^				typedef struct rsi_eap_credentials_s
 * 		^				{ 
 * 		^				uint8_t username[64];
 * 		^				uint8_t password[128];
 * 		^				} rsi_eap_credentials_t;
 * 		^				username: username to be used in enterprise
 * 		^				password: password for a given username.
 * 		RSI_WPA_WPA2_MIXED	|	PSK string terminated with NULL. Length of PSK should be at least 8 and less than 64 bytes
 * 		RSI_WPA_PMK		|	PMK string, should be 32 bytes in length
 * 		RSI_WPA2_PMK		|	PMK string, should be 32 bytes in length
 * 		RSI_WPS_PIN		|	8-byte WPS PIN
 * 		RSI_USE_GENERATED_WPSPIN|	NULL string indicate to use PIN generated using rsi_wps_generate_pin API
 * 		RSI_WPS_PUSH_BUTTON	|	NULL string indicate to generate push button event
 * 		RSI_WPA3            |   PSK string terminated with NULL. Length of PSK should be at least 8 and less than 64 bytes.
 * @param[in]   join_response_handler - Called when the response for join has been received from the module. \n
 *                                      Parameters involved are status, buffer, & length \n
 * @param[out]  Status                - Response status. 
 * @param[out]  Buffer                - Response buffer. On successful execution of the command.  \n GO_Status (1 byte, hex): 0x47 (ASCII "G") - If the module becomes a Group Owner (GO) after the GO
 *                                      negotiation stage or becomes an Access Point. \n 0x43 (ASCII "C") - If the module does not become a GO after the GO negotiation stage or becomes a client (or station).
 * @param[out]  length 		            -	Length of the response buffer.
 * @note       **Precondition** - \ref rsi_wireless_init() API needs to be called before this API.
 * @note       Refer to \ref error-codes for the description of above error codes.
 * @note       The module gets a default IP of 192.168.100.76, if it becomes a Group Owner or Access Point in case of IPv4 and gets a default IP of 2001:db8:0:1:0:0:0:120 in case of IPv6.
 * @note       To set various timeouts, user should change the following macros in rsi_wlan_config.h \n
 *			       #define RSI_TIMEOUT_SUPPORT RSI_ENABLE. \n
 *			       #define RSI_TIMEOUT_BIT_MAP 4. \n
 *			       #define RSI_TIMEOUT_VALUE 300. \n
 *
 *        	   timeout_bitmap          |       Description	
 * 		         :-----------------------|:--------------------------------------------------------------------------------------------------
 * 		         timeout_bitmap[0]     	 |	     Set timeout for association and authentication request.timeout_value : timeout value in ms(default 300ms).
 * 		         timeout_bitmap[1]     	 |       Sets each channel active scan time in ms (default 100ms)
 * 		         timeout_bitmap[2] 	     |       Used for WLAN keep alive timeout(default value is 30s)
 * 		         timeout_bitmap[31-3]    |       Reserved
 * @note	      To set Rejoin parameters, user should configure the following macros in rsi_wlan_config.h \n 
 *        	      Macro                          |       Description	
 * 		            :------------------------------|:--------------------------------------------------------------------------------------------------
 *		            RSI_REJOIN_PARAMS_SUPPORT      | Enable to send rejoin parameters command during Wi-Fi client connection \n
 *		            RSI_REJOIN_MAX_RETRY           | Number of maximum rejoin retries by module\n
 *		            note                           | If Max retries is 0 , retries infinity times \n
 *		            RSI_REJOIN_SCAN_INTERVAL       | Periodicity of rejoin attempt \n
 *		            RSI_REJOIN_BEACON_MISSED_COUNT | Number of consecutive beacon misses after which modules goes to unconnected state \n
 *		            RSI_REJOIN_FIRST_TIME_RETRY    | ENABLE  - Try to rejoin in the first attempt after join failure. \n
 *		                                             DISABLE - Try to rejoin based on maximum rejoin retries configured. \n
 * @note	      When RSI_REJOIN_PARAMS_SUPPORT is enabled in the rsi_wlan_config.h, the rejoin frame will be sent to the firmware after the scan is done.
 * @note        This API internally handles following commands based on wlan_cb state and sends the next command and finally sends the join command. \n
 *              Set MAC address, Band, Timeout, Init, Set region, WMM parameters, Scan, EAP config, WMM PS parameters, WPS method, Set WEP keys, Host PSK, Rejoin params, Join \n
 * @note        For example, \n
 *              after calling rsi_wireless_init(), wlan_cb state is updated to OPERMODE_DONE state. So when we call this API, it will execute band, init, scan and join commands. \n
 * @note        For example, \n
 *              after calling rsi_wlan_disconnect() or else after rejoin failure, wlan_cb state is updated to BAND_DONE state. So when we call this API, it will execute init, scan and join commands. \n
 * @note        For example, \n
 *              after calling rsi_wlan_scan()/ rsi_wlan_scan_with_bitmap_options() API, wlan_cb state is updated to SCAN_DONE state. So when we call this API, it will execute join command directly.
 *
 */

int32_t rsi_wlan_connect_async(int8_t *ssid,
                               rsi_security_mode_t sec_type,
                               void *secret_key,
                               void (*join_response_handler)(uint16_t status,
                                                             const uint8_t *buffer,
                                                             const uint16_t length))
{
  rsi_eap_credentials_t *credentials;
  rsi_req_wps_method_t *wps_method;
  rsi_req_psk_t *psk;
  rsi_wep_keys_t *wep_keys;
  rsi_pkt_t *pkt;
  rsi_req_scan_t *scan;
  rsi_req_join_t *join;
  rsi_req_eap_config_t *eap_config;
  int32_t status = RSI_SUCCESS;
  int32_t allow  = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_CONNECT_ASYNC_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;
  if (join_response_handler != NULL) {
    allow = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  }

  if (allow == RSI_SUCCESS) {
    // Check whether PSK is provided or not for security modes
    if ((sec_type != 0) && (secret_key == NULL) && (sec_type != RSI_WPS_PUSH_BUTTON)
        && (sec_type != RSI_USE_GENERATED_WPSPIN)) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Throw error in case of invalid parameters
      SL_PRINTF(SL_WLAN_CONNECT_ASYNC_INVALID_PARAM_1, WLAN, LOG_ERROR);
      return RSI_ERROR_INVALID_PARAM;
    }

    // Check whether module is in valid state range or not
    if ((wlan_cb->opermode != RSI_WLAN_CONCURRENT_MODE && wlan_cb->state >= RSI_WLAN_STATE_CONNECTED)
        || (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE && wlan_cb->state >= RSI_WLAN_STATE_CONNECTED
            && wlan_cb->sta_state >= RSI_WLAN_STATE_CONNECTED)) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Command given in wrong state
      SL_PRINTF(SL_WLAN_CONNECT_ASYNC_COMMAND_GIVEN_IN_WRONG_STATE_1, WLAN, LOG_ERROR);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }

    if ((wlan_cb->opermode == RSI_WLAN_ENTERPRISE_CLIENT_MODE)
        && !((sec_type == RSI_WPA_EAP) || (sec_type == RSI_WPA2_EAP))) {
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Throw error in case of invalid parameters
      SL_PRINTF(SL_WLAN_CONNECT_ASYNC_INVALID_PARAM_2, WLAN, LOG_ERROR);
      return RSI_ERROR_INVALID_PARAM;
    }

    switch (wlan_cb->state) {
      case RSI_WLAN_STATE_OPERMODE_DONE: {
        if (wlan_cb->field_valid_bit_map & RSI_SET_MAC_BIT) {
          // Allocate command buffer from WLAN pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
          // If allocation of packet fails
          if (pkt == NULL) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // Return packet allocation failure error
            SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_1, WLAN, LOG_ERROR);
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }
          // Memset data
          memset(&pkt->data, 0, sizeof(rsi_req_mac_address_t));
          // Memcpy data
          memcpy(&pkt->data, wlan_cb->mac_address, sizeof(rsi_req_mac_address_t));
#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // Send set MAC command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_MAC_ADDRESS, pkt);
          // Wait on WLAN semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_MAC_RESPONSE_WAIT_TIME);
          // Get WLAN/network command response status
          status = rsi_wlan_get_status();
          if (status != RSI_SUCCESS) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // Return status if error in sending command occurs
            SL_PRINTF(SL_WLAN_CONNECT_ASYNC_ERROR_IN_SENDING_COMMAND_1, WLAN, LOG_ERROR);
            return status;
          }
        }
#if HE_PARAMS_SUPPORT
        status = rsi_wlan_11ax_config(GUARD_INTERVAL);
        if (status != RSI_SUCCESS) {
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_HE_PARAMS_SUPPORT, WLAN, LOG_INFO);
          return status;
        }
#endif
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_2, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send band command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_BAND, pkt);
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_BAND_RESPONSE_WAIT_TIME);
        // Get WLAN/network command response status
        status = rsi_wlan_get_status();
        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_ERROR_IN_SENDING_COMMAND_2, WLAN, LOG_ERROR);
          return status;
        }
      }
        //  No break
      // Fall through
      case RSI_WLAN_STATE_BAND_DONE: {
#if RSI_TIMEOUT_SUPPORT
        status = send_timeout(RSI_TIMEOUT_BIT_MAP, RSI_TIMEOUT_VALUE);
        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_ERROR_IN_SENDING_COMMAND_3, WLAN, LOG_ERROR);
          return status;
        }
#endif
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_4, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send init command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_INIT, pkt);
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_INIT_RESPONSE_WAIT_TIME);
        // Get WLAN/network command response status
        status = rsi_wlan_get_status();
        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_WLAN_CMD_STATE_CHANGED_TO_ALLOW, WLAN, LOG_INFO);
          return status;
        }
#if RSI_SET_REGION_SUPPORT
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_5, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
        // Memset data
        memset(&pkt->data, 0, sizeof(rsi_req_set_region_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send set region command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_REGION, pkt);
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_REGION_RESPONSE_WAIT_TIME);
        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_ERROR_IN_SENDING_COMMAND_4, WLAN, LOG_ERROR);
          return status;
        }
#endif
#if RSI_SET_REGION_AP_SUPPORT
        if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE) {
          // allocate command buffer  from wlan pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

          // If allocation of packet fails
          if (pkt == NULL) {
            //Changing the wlan cmd state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // return packet allocation failure error
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }
          // Memset data
          memset(&pkt->data, 0, sizeof(rsi_req_set_region_ap_t));

#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // send set region AP command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_REGION_AP, pkt);

          // wait on wlan semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_REGION_AP_RESPONSE_WAIT_TIME);

          // get wlan/network command response status
          status = rsi_wlan_get_status();

          if (status != RSI_SUCCESS) {
            //Changing the wlan cmd state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

            // Return the status if error in sending command occurs
            return status;
          }
        }
#endif
      }
        //  No break
      // Fall through
      case RSI_WLAN_STATE_INIT_DONE: {
#if RSI_WMM_PS_ENABLE
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_6, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset the packet data
        memset(&pkt->data, 0, sizeof(rsi_wmm_ps_parms_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send WMM parameters
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_WMM_PS, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_WMM_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_ERROR_IN_SENDING_COMMAND_5, WLAN, LOG_ERROR);
          return status;
        }

#endif
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_7, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Give buffer as NULL, no need to give scan results to host
        wlan_cb->app_buffer = NULL;

        // Length of the buffer is zero
        wlan_cb->app_buffer_length = 0;

        // Send scan command
        scan = (rsi_req_scan_t *)pkt->data;

        // Memset the packet data
        memset(&pkt->data, 0, sizeof(rsi_req_scan_t));

        // Copy SSID given by the application
        if (ssid != NULL) {
          rsi_strcpy(scan->ssid, ssid);
        }

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send scan command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SCAN, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_SCAN_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_ERROR_IN_SENDING_COMMAND_6, WLAN, LOG_ERROR);
          return status;
        }
      }
        // No break
      // Fall through
      case RSI_WLAN_STATE_SCAN_DONE: {

        // Send EAP command for EAP security
        if ((wlan_cb->opermode == RSI_WLAN_ENTERPRISE_CLIENT_MODE)) {
          // Allocate command buffer from WLAN pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

          // If allocation of packet fails
          if (pkt == NULL) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // Return packet allocation failure error
            SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_8, WLAN, LOG_ERROR);
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }

          credentials = (rsi_eap_credentials_t *)secret_key;

          // Memset the packet data
          memset(&pkt->data, 0, sizeof(rsi_req_eap_config_t));

          eap_config = (rsi_req_eap_config_t *)pkt->data;

          // Copy username
          memcpy(eap_config->user_identity, credentials->username, RSI_EAP_USER_NAME_LENGTH);

          // Copy password
          memcpy(eap_config->password, credentials->password, RSI_EAP_PASSWORD_LENGTH);

#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // Send EAP config command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_EAP_CONFIG, pkt);

          // Wait on WLAN semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_EAP_RESPONSE_WAIT_TIME);

          // Get WLAN/network command response status
          status = rsi_wlan_get_status();

          if (status != RSI_SUCCESS) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

            // Return status if error in sending command occurs
            SL_PRINTF(SL_WLAN_CONNECT_ASYNC_ERROR_IN_SENDING_COMMAND_7, WLAN, LOG_ERROR);
            return status;
          }
        }

#if RSI_WMM_PS_ENABLE
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_9, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset the packet data
        memset(&pkt->data, 0, sizeof(rsi_wmm_ps_parms_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send WMM parameters
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_WMM_PS, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_WMM_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_ERROR_IN_SENDING_COMMAND_8, WLAN, LOG_ERROR);
          return status;
        }

#endif
        if ((sec_type == RSI_WPS_PIN) && (wlan_cb->opermode != RSI_WLAN_ENTERPRISE_CLIENT_MODE)) {
          // Allocate command buffer from WLAN pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

          // If allocation of packet fails
          if (pkt == NULL) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // Return packet allocation failure error
            SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_10, WLAN, LOG_ERROR);
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }

          wps_method = (rsi_req_wps_method_t *)pkt->data;

          // Memset buffer
          memset(&pkt->data, 0, sizeof(rsi_req_wps_method_t));

          // Set configuration to validate WPS pin
          rsi_uint16_to_2bytes(wps_method->wps_method, 1);

          // Set configuration to validate WPS pin
          rsi_uint16_to_2bytes(wps_method->generate_pin, 0);

          // Copy WPS pin
          memcpy(wps_method->wps_pin, secret_key, RSI_WPS_PIN_LEN);

#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // Send WPS method command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_WPS_METHOD, pkt);

          // Wait on WLAN semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_WPS_RESPONSE_WAIT_TIME);

          // Get WLAN/network command response status
          status = rsi_wlan_get_status();

          if (status != RSI_SUCCESS) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // Return status if error in sending command occurs
            SL_PRINTF(SL_WLAN_CONNECT_ASYNC_ERROR_IN_SENDING_COMMAND_9, WLAN, LOG_ERROR);
            return status;
          }
        } else if (sec_type == RSI_WEP) {
          // Allocate command buffer from WLAN pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

          // If allocation of packet fails
          if (pkt == NULL) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // Return packet allocation failure error
            SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_11, WLAN, LOG_ERROR);
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }

          // Memset buffer
          memset(&pkt->data, 0, sizeof(rsi_wep_keys_t));

          wep_keys = (rsi_wep_keys_t *)pkt->data;

          // Copy WEP keys
          memcpy(wep_keys, secret_key, sizeof(rsi_wep_keys_t));

#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // Send PSK command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_WEP_KEYS, pkt);

          // Wait on WLAN semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_WEP_RESPONSE_WAIT_TIME);

          // Get WLAN/network command response status
          status = rsi_wlan_get_status();

          if (status != RSI_SUCCESS) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

            // Return status if error in sending command occurs
            SL_PRINTF(SL_WLAN_CONNECT_ASYNC_ERROR_IN_SENDING_COMMAND_10, WLAN, LOG_ERROR);
            return status;
          }

        }
        // Send PSK command for PSK security
        else if (((sec_type == RSI_WPA) || (sec_type == RSI_WPA2) || (sec_type == RSI_WPA3)
                  || (sec_type == RSI_WPA3_TRANSITION) || (sec_type == RSI_WPA_WPA2_MIXED)
                  || (sec_type == RSI_WPA_WPA2_MIXED_PMK) || (sec_type == RSI_WPA_PMK) || (sec_type == RSI_WPA2_PMK))
                 && (wlan_cb->opermode != RSI_WLAN_ENTERPRISE_CLIENT_MODE)) {
          // Allocate command buffer from WLAN pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

          // If allocation of packet fails
          if (pkt == NULL) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // Return packet allocation failure error
            SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_12, WLAN, LOG_ERROR);
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }
          psk = (rsi_req_psk_t *)pkt->data;

          // Memset buffer
          memset(&pkt->data, 0, sizeof(rsi_req_psk_t));

          if ((sec_type == RSI_WPA2_PMK) || (sec_type == RSI_WPA_PMK) || (sec_type == RSI_WPA_WPA2_MIXED_PMK)) {
            if (sec_type == RSI_WPA_WPA2_MIXED_PMK) {
              sec_type = RSI_WPA_WPA2_MIXED;
            }
            // Set type as PMK
            psk->type = 2;

            // Copy PMK
            memcpy(psk->psk_or_pmk, secret_key, RSI_PMK_LEN);

          } else {
            // Set type as PSK
            psk->type = 1;

            // Copy PSK
            memcpy(psk->psk_or_pmk, secret_key, RSI_PSK_LEN);
          }

#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // Send PSK command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_HOST_PSK, pkt);
          // Wait on WLAN semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_WPS_RESPONSE_WAIT_TIME);

          // Get WLAN/network command response status
          status = rsi_wlan_get_status();

          if (status != RSI_SUCCESS) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

            // Return status if error in sending command occurs
            SL_PRINTF(SL_WLAN_CONNECT_ASYNC_ERROR_IN_SENDING_COMMAND_11, WLAN, LOG_ERROR);
            return status;
          }
        }

#if RSI_REJOIN_PARAMS_SUPPORT
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_13, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset data
        memset(&pkt->data, 0, sizeof(rsi_req_rejoin_params_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send rejoin params command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_REJOIN_PARAMS, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_REJOIN_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_ERROR_IN_SENDING_COMMAND_12, WLAN, LOG_ERROR);
          return status;
        }
#endif
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_CONNECT_ASYNC_PKT_ALLOCATION_FAILURE_14, WLAN, LOG_ERROR);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset the PKT
        memset(&pkt->data, 0, sizeof(rsi_req_join_t));

        join = (rsi_req_join_t *)pkt->data;

        // Fill security type
        if (sec_type == RSI_WPA2_PMK) {
          join->security_type = RSI_WPA2;
        } else if (sec_type == RSI_WPA_PMK) {
          join->security_type = RSI_WPA;
        } else if ((sec_type == RSI_WPS_PIN) || (sec_type == RSI_USE_GENERATED_WPSPIN)
                   || (sec_type == RSI_WPS_PUSH_BUTTON)) {
          join->security_type = RSI_OPEN;
        } else if (sec_type == RSI_WPA3) {
          join->security_type = SME_WPA3;
        } else if (sec_type == RSI_WPA3_TRANSITION) {
          join->security_type = SME_WPA3_TRANSITION;
        } else {
          join->security_type = sec_type;
        }

        if (ssid != NULL) {
          // Copy SSID and SSID len
          join->ssid_len = rsi_strlen(ssid);

          // Copy Join SSID
          rsi_strcpy(join->ssid, ssid);
        }

        if (join_response_handler != NULL) {
          // Register scan response handler
          rsi_wlan_cb_non_rom->callback_list.wlan_join_response_handler = join_response_handler;
        } else {
#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        }
        // Send join command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_JOIN, pkt);
      } break;
      case RSI_WLAN_STATE_NONE:
      case RSI_WLAN_STATE_CONNECTED:
      case RSI_WLAN_STATE_IP_CONFIG_DONE:
      case RSI_WLAN_STATE_IPV6_CONFIG_DONE:
      case RSI_WLAN_STATE_AUTO_CONFIG_GOING_ON:
      case RSI_WLAN_STATE_AUTO_CONFIG_DONE:
      case RSI_WLAN_STATE_AUTO_CONFIG_FAILED:
        break;
      default: {
        // Return status if command given in driver is in an invalid state
        status = RSI_ERROR_COMMAND_NOT_SUPPORTED;
        if (join_response_handler == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
        }
      } break;
    }
  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_CONNECT_ASYNC_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR);
    return allow;
  }
  // Return status
  SL_PRINTF(SL_WLAN_CONNECT_ASYNC_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief        Enable background scan and get the results or disable background scan. This is a blocking API.
 * @param[in]	   cmd - Command given by user, to enable or disable bgscan. \n
 *                     0 - disable bgscan, \n
 *                     1 - enable bgscan
 * @param[out]   result - buffer to store bgscan results
 * @param[in]    length - Length of bgscan result buffer
 * @return       0  	  -  Success \n
 * @return       Non-Zero Value - Failure (**Possible Error Codes** - 0xfffffffd, 0xfffffffc) \n
 * @note         **Precondition** - \ref rsi_wlan_connect() API needs to be called before this API.
 * @note	       Refer to \ref error-codes for the description of above error codes.
 *
 */

int32_t rsi_wlan_bgscan_profile(uint8_t cmd, rsi_rsp_scan_t *result, uint32_t length)
{
  rsi_pkt_t *pkt;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_BGSCAN_PROFILE_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Check whether module is in valid state range or not
  if (wlan_cb->state < RSI_WLAN_STATE_CONNECTED) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_BGSCAN_PROFILE_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Attach the buffer given by user
    wlan_cb->app_buffer = (uint8_t *)result;

    // Length of the buffer provided by user
    wlan_cb->app_buffer_length = length;

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_BGSCAN_PROFILE_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Memset data
    memset(&pkt->data, 0, sizeof(rsi_req_bg_scan_t));

    // Magic word
    pkt->data[0] = 0xAB;

    // cmd
    pkt->data[1] = cmd;
#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif

    // Send BG scan command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_BG_SCAN, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_BGSCAN_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_BGSCAN_PROFILE_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_BGSCAN_PROFILE_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}
/*==============================================*/
/**
 * @brief      Enable scan and roaming after connecting the module to the access point. This is a blocking API.
 * @param[in]  Void
 * @return     0  	  -  Success \n
 * @return     Non-Zero Value - Failure (**Possible Error Codes** - 0xfffffffd,0xfffffffc,0x0006,0x0021,0x002C,0x004A,0x0025,0x0026) \n
 * @note       **Precondition** - \ref rsi_wlan_connect() API needs to be called before this API.
 * @note	     Refer to \ref error-codes for the description of above error codes.
 *
 */

// Execute post connect commands
int32_t rsi_wlan_execute_post_connect_cmds(void)
{
#if (RSI_BG_SCAN_SUPPORT || RSI_ROAMING_SUPPORT)
  rsi_pkt_t *pkt;
#endif
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_EXECUTE_POST_CONNECT_CMDS_ENRTY, WLAN, LOG_INFO, "status: %4x", status);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Check whether module is in valid state range or not
  if (wlan_cb->state < RSI_WLAN_STATE_CONNECTED) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_EXECUTE_POST_CONNECT_CMDS_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
#if RSI_BG_SCAN_SUPPORT
    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_EXECUTE_POST_CONNECT_CMDS_PKT_ALLOCATION_FAILURE_1, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Memset data
    memset(&pkt->data, 0, sizeof(rsi_req_bg_scan_t));

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send BG scan command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_BG_SCAN, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_BGSCAN_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();

    if (status != RSI_SUCCESS) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

      // Return status if error in sending command occurs
      SL_PRINTF(SL_WLAN_EXECUTE_POST_CONNECT_CMDS_ERROR_IN_SENDING_COMMAND_1, WLAN, LOG_ERROR, "status: %4x", status);
      return status;
    }

#endif
#if RSI_ROAMING_SUPPORT
    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_EXECUTE_POST_CONNECT_CMDS_PKT_ALLOCATION_FAILURE_2, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }
    // Memset data
    memset(&pkt->data, 0, sizeof(rsi_req_roam_params_t));

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send roaming parameters command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_ROAM_PARAMS, pkt);

    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_ROAMING_RESPONSE_WAIT_TIME);
    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
#endif
    // Wait on WLAN semaphore

    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_WLAN_EXECUTE_POST_CONNECT_CMDS_NWK_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_EXECUTE_POST_CONNECT_CMDS_ERROR_IN_SENDING_COMMAND_2, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief       Start the WPS Push button in AP mode. This is a blocking API.
 * @param[in]   ssid - SSID of the Access Point. SSID should be same as that of given in AP start API. Length of the SSID should be less than or equal to 32 bytes.
 * @return      0  	  -  Success \n
 * @return      Non-Zero Value - Failure (**Possible Error Codes** - 0xfffffffc,0x0021) \n
 * @note        **Precondition** - \ref rsi_wlan_ap_start() API needs to be called before this API.
 * @note	      Refer to \ref error-codes for the description of above error codes.
 *
 */

int32_t rsi_wlan_wps_push_button_event(int8_t *ssid)
{
  rsi_pkt_t *pkt;
  rsi_req_join_t *join;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_WPS_PUSH_BUTTON_EVENT_ENRTY, WLAN, LOG_INFO, "status: %4x", status);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_WPS_PUSH_BUTTON_EVENT_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_WPS_PUSH_BUTTON_EVENT_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_req_join_t));

    join = (rsi_req_join_t *)pkt->data;

    if (ssid != NULL) {
      // Copy SSID and SSID len
      join->ssid_len = rsi_strlen(ssid);

      // Copy Join SSID
      rsi_strcpy(join->ssid, ssid);
    }

    // Deregister join response handler
    rsi_wlan_cb_non_rom->callback_list.wlan_join_response_handler = NULL;

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send join command to start wps
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_JOIN, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_JOIN_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_WPS_PUSH_BUTTON_EVENT_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_WPS_PUSH_BUTTON_EVENT_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief        Application to provide feedback of frequency error in KHz. This is a blocking API.
 * @param[in]    freq_offset_in_khz - Frequency deviation observed in KHz
 * @return       0          - Success \n   
 * @note         Refer to \ref error-codes for the description of above error codes.
 */
int32_t rsi_send_freq_offset(int32_t freq_offset_in_khz)
{
  rsi_pkt_t *pkt;
  rsi_freq_offset_t *freq_offset = NULL;
  int32_t status                 = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_SEND_FREQ_OFFSET_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_SEND_FREQ_OFFSET_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_SEND_FREQ_OFFSET_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }
    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_freq_offset_t));

    freq_offset = (rsi_freq_offset_t *)pkt->data;

    freq_offset->freq_offset_in_khz = freq_offset_in_khz;

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send WPS request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_FREQ_OFFSET, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_FREQ_OFFSET_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_SEND_FREQ_OFFSET_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_SEND_FREQ_OFFSET_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief             RF calibration process. This API will command the firmware to update the existing Flash/EFuse calibration data. This is a blocking API.
 * @param[in]         Target - 0 - BURN_INTO_EFUSE (Burns calibration data to EFuse) \n
 * 			                       1 - BURN_INTO_FLASH (Burns calibration data to Flash) \n
 * @param[in]         Flags  - Validate information \n
 *                     Bit |	MACRO 		           |	Description
 *                     :---|:---------------------:|:---------------------------------------------------
 *                     0   |	RESERVED_0           |  Reserved
 *                     1   |	BURN_FREQ_OFFSET     |	1 - Update XO Ctune to calibration data \n	0 - Skip XO Ctune update
 *                     2   |	SW_XO_CTUNE_VALID    |	1 - Use XO Ctune provided as argument to update calibration data. \n	0 - Use XO Ctune value as read from hardware register
 *                     3   |	BURN_XO_FAST_DISABLE |  Used to apply patch for cold temperature issue (host interface detection) observed on CC0/CC1 modules. \ref appendix
 *                     4   |  BURN_GAIN_OFFSET_LOW |  1 - Update gain offset for low sub-band (2 GHz) \n	0 - Skip low sub-band gain-offset update
 *                     5   |  BURN_GAIN_OFFSET_MID |  1 - Update gain offset for mid sub-band (2 GHz) \n	0 - Skip mid sub-band gain-offset update
 *                     6   |  BURN_GAIN_OFFSET_HIGH|  1 - Update gain offset for high sub-band (2 GHz) \n	0 - Skip high sub-band gain-offset update
 *                     7   |  SELECT_GAIN_OFFSETS_1P8V | 1 - Update gain offsets for 1.8 V \n    0 - Update gain offsets for 3.3 V
 *                     31-4 |                      |	Reserved
 * @param[in]         gain_offset_low - gain_offset as observed in dBm in channel-1
 * @param[in]         gain_offset_mid - gain_offset as observed in dBm in channel-6
 * @param[in]         gain_offset_high - gain_offset as observed in dBm in channel-11
 * @param[in]         xo_ctune - Allow user to directly update xo_ctune value to calibration data bypassing the freq offset loop,
 * 			              valid only when BURN_FREQ_OFFSET & SW_XO_CTUNE_VALID of flags is set. The range of xo_ctune is [0, 255], and the typical value is 80.            
 * @return            0			- Success \n
 * @return            Non-Zero Value	- Failure
 * @note              **Precondition** - \ref rsi_transmit_test_start(), \ref rsi_send_freq_offset() API needs to be called before this API. This API is relevant in PER mode only. 
 * @note              For gain-offset calibration in 2.4 GHz, the user needs to calibrate gain-offset for low sub-band (channel-1), mid sub-band (channel-6), and high sub-band (channel-11) and input the three gain-offsets to this API and set the corresponding flags to validate it. \n
 * @note              Recalibration is not possible if EFuse is being used instead of flash as calibration data storage
 */

int32_t rsi_calib_write(uint8_t target,
                        uint32_t flags,
                        int8_t gain_offset_low,
                        int8_t gain_offset_mid,
                        int8_t gain_offset_high,
                        int8_t xo_ctune)
{
  rsi_pkt_t *pkt;
  rsi_calib_write_t *calib_wr = NULL;
  int32_t status              = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_CALIB_WRITE_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_CALIB_WRITE_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_CALIB_WRITE_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    calib_wr = (rsi_calib_write_t *)pkt->data;

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_calib_write_t));

    // Target flash or efuse
    calib_wr->target = target;

    // Flags
    calib_wr->flags = flags;

    // Gain offset
    calib_wr->gain_offset[0] = gain_offset_low;
    calib_wr->gain_offset[1] = gain_offset_mid;
    calib_wr->gain_offset[2] = gain_offset_high;

    // Freq offset
    calib_wr->xo_ctune = xo_ctune;

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send WPS request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_CALIB_WRITE, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_CALIB_WRITE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_CALIB_WRITE_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_CALIB_WRITE_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}
/*==============================================*/
/**
 * @brief             RF calibration process. This API reads the calibration data from the Flash/EFuse storage. This is a blocking API.
 * @pre    	\ref rsi_wireless_init() needs to be called prior this API. 
 * @param[in]         Target \n
 * 			0 - READ_FROM_EFUSE (read calibration data from the EFuse) \n
 * 			1 - READ_FROM_FLASH (read calibration data from the Flash) \n
 * @param[out]         gain_offset_low - gain_offset in dBm that will be applied for transmissions in channel-1.
 * @param[out]         gain_offset_mid - gain_offset in dBm that will be applied for transmissions in channel-6.
 * @param[out]         gain_offset_high -gain_offset in dBm that will be applied for transmissions in channel-11.
 * @param[out]         xo_ctune - xo_ctune value as read from the target memory. 
 * @return            0			- Success \n
 *                    Non-Zero Value	- Failure
 */

int32_t rsi_calib_read(uint8_t target, rsi_calib_read_t *calib_data)
{
  rsi_pkt_t *pkt;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_CALIB_READ_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_CALIB_READ_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_CALIB_READ_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Memset buffer
    memset(&pkt->data, 0, 4);

    // Target flash or efuse
    pkt->data[0] = target;

    // Attach the buffer given by user
    wlan_cb->app_buffer = (uint8_t *)calib_data;

    // Length of the buffer provided by user
    wlan_cb->app_buffer_length = sizeof(rsi_calib_read_t);

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send WPS request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_CALIB_READ, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_CALIB_READ_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_CALIB_READ_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_CALIB_READ_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}
/*==============================================*/
/**
 * @brief         Parse and convert the given value in ASCII to the datatype of a given length.. This is a non-blocking API.
 * @param[in]     address         - Address
 * @param[in]     length          - Length
 * @param[in]     value           - Value
 * @return        0               - Success \n
 * @return        Non-Zero Value  - Failure
 */
int16_t rsi_parse(void *address, uint16_t length, uint8_t *value)
{
  uint8_t temp_buff[15];
  uint16_t temp_offset = 0;
  uint16_t offset;

  for (offset = 0; offset < length; offset++) {
    uint16_t index = 0;
    /* Copy the bytes until '.' ',' '\0'  is reached */
    while ((value[temp_offset] != '.') && (value[temp_offset] != ',') && (value[temp_offset] != '\0')) {
      if (index >= (sizeof(temp_buff) - 1))
        break;
      temp_buff[index++] = value[temp_offset++];
    }
    temp_buff[index] = '\0';
    temp_offset++;
    if (length == RSI_PARSE_2_BYTES) {
      *((uint16_t *)address) = rsi_atoi((int8_t *)temp_buff);
      break;
    } else if (length == RSI_PARSE_4_BYTES) {
      *((uint32_t *)address) = rsi_atoi((int8_t *)temp_buff);
      break;
    } else
      *((uint8_t *)address + offset) = rsi_atoi((int8_t *)temp_buff);
  }

  return temp_offset;
}

/*==============================================*/
/**
 * @brief      Generate WPS pin. This is a blocking API.
 * @param[out] wps_pin - 8-byte WPS pin generated by the device.
 * @param[in]  length  - This is the length of the resulted buffer measured in bytes to hold WPS pin.
 * @return     0               - Success \n
 * @return     Non-Zero  Value - Failure (**Possible Error Codes** - 0xfffffffc,0x0021,0x002C,0x0025,0x0037,0x0038) \n
 * @note       **Precondition** - \ref rsi_wireless_init() API needs to be called before this API.
 * @note       Refer to Error Codes section for the description of the above error codes  \ref error-codes.
 *
 */

int32_t rsi_wlan_wps_generate_pin(uint8_t *wps_pin, uint16_t length)
{
  rsi_pkt_t *pkt;
  rsi_req_wps_method_t *wps_method;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_WPS_GENERATE_PIN_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_WPS_GENERATE_PIN_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_WPS_GENERATE_PIN_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    wps_method = (rsi_req_wps_method_t *)pkt->data;

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_req_wps_method_t));

    // Set configuration to generate WPS pin
    rsi_uint16_to_2bytes(wps_method->wps_method, 1);

    // Set configuration to generate WPS pin
    rsi_uint16_to_2bytes(wps_method->generate_pin, 1);

    // Attach the buffer given by user
    wlan_cb->app_buffer = (uint8_t *)wps_pin;

    // Length of the buffer provided by user
    wlan_cb->app_buffer_length = length;

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send WPS request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_WPS_METHOD, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_WPS_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_WPS_GENERATE_PIN_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_WPS_GENERATE_PIN_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief	    	Validate WPS pin entered. This is a blocking API.
 * @param[in]   wps_pin - 8-byte valid wps pin
 * @param[out]  Same WPS pin if command is successful
 * @return		  0 	          	- Success \n
 * @return		  Non-Zero Value  - Failure
 * @note        **Precondition** - \ref rsi_wireless_init() API needs to be called before this API.
 */

int32_t rsi_wlan_wps_enter_pin(int8_t *wps_pin)
{
  rsi_pkt_t *pkt;
  rsi_req_wps_method_t *wps_method;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_WPS_ENTER_PIN_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_WPS_ENTER_PIN_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_WPS_ENTER_PIN_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    wps_method = (rsi_req_wps_method_t *)pkt->data;

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_req_wps_method_t));

    // Set configuration to validate WPS pin
    rsi_uint16_to_2bytes(wps_method->wps_method, 1);

    // Set configuration to validate WPS pin
    rsi_uint16_to_2bytes(wps_method->generate_pin, 0);

    // Copy WPS pin
    memcpy(wps_method->wps_pin, wps_pin, RSI_WPS_PIN_LEN);

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send WPS request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_WPS_METHOD, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_WPS_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_WPS_ENTER_PIN_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_WPS_ENTER_PIN_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief       Get random bytes from the device. This is a blocking API.
 * @param[in]   result - Pointer to the buffer in which random data are copied from.
 * @param[in]   length - Length of the random data required
 * @return      0               - Success \n
 * @return      Non-Zero Value  - Failure
 */

int32_t rsi_get_random_bytes(uint8_t *result, uint32_t length)
{
  rsi_pkt_t *pkt;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_GET_RANDOM_BYTES_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;
  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_GET_RANDOM_BYTES_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Attach the buffer given by user
    wlan_cb->app_buffer = (uint8_t *)result;

    // Length of the buffer provided by user
    wlan_cb->app_buffer_length = length;

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_GET_RANDOM_BYTES_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_req_disassoc_t));

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send get random number command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_GET_RANDOM, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_WLAN_GET_RANDOM_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_GET_RANDOM_BYTES_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_GET_RANDOM_BYTES_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief       Disconnect the module from the connected access point. This is a blocking API.
 * @return      0               - Success \n
 *              Non-Zero Value  - Failure (**Possible Error Codes** - 0xfffffffd,0xfffffffc,0x0006,0x0021,0x002C,0x004A,0x0025,0x0026)\n
 * @note        **Precondition** - \ref rsi_wlan_connect() API needs to be called before this API.
 * @note        Refer to \ref error-codes for the description of above error codes.
 *
 */

int32_t rsi_wlan_disconnect(void)
{
  rsi_pkt_t *pkt;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_DISCONNECT_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_CONNECTED) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_DISCONNECT_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_DISCONNECT_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_req_disassoc_t));

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send disconnect command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_DISCONNECT, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_DISCONNECT_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_DISCONNECT_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_DISCONNECT_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief      Disconnect a connected station from the module in AP mode. This is a blocking API.
 * @param[in]  mac_address - Mac address (6 bytes) of the station to be disconnected.
 * @return     0               - Success \n
 *             Non-Zero Value  - Failure (**Possible Error Codes** - 0xfffffffc,0x0013,0x0021,0x002C,0x0015)\n
 * @note       **Precondition** - \ref rsi_wlan_ap_start() API needs to be called before this API.
 * @note       Refer to \ref error-codes for the description of above error codes.
 *
 */

int32_t rsi_wlan_disconnect_stations(uint8_t *mac_address)
{
  rsi_pkt_t *pkt;
  rsi_req_disassoc_t *disassoc_info;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_DISCONNECT_STATIONS_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_DISCONNECT_STATIONS_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_DISCONNECT_STATIONS_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    disassoc_info = (rsi_req_disassoc_t *)pkt->data;

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_req_disassoc_t));

    // Set mode flag to represent AP mode
    rsi_uint16_to_2bytes(disassoc_info->mode_flag, 1);

    // Copy MAC address of station to be disconnected
    memcpy(disassoc_info->client_mac_address, mac_address, 6);

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send disconnect command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_DISCONNECT, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_DISCONNECT_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_DISCONNECT_STATIONS_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_DISCONNECT_STATIONS_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}
/** @} */

/** @addtogroup WLAN
* @{
*/

/*==============================================*/
/**
 * @fn             int32_t rsi_config_ipaddress(rsi_ip_version_t version, uint8_t mode, uint8_t *ip_addr,
 *                                           uint8_t *mask, uint8_t *gw, uint8_t *ipconfig_rsp,
 *                                            uint16_t length, uint8_t vap_id);
 * @brief        Configure the IP address to the module. This is a blocking API.
 * @param[in]    version           - IP version to be used. RSI_IP_VERSION_4 (4) – to select IPv4, RSI_IP_VERSION_6 (6) – to select IPv6
 * @param[in]    mode              - 1 - DHCP mode ; 0 - static mode
 * @param[in]    ip_addr           - Pointer to IP address
 *               IPv6 Address Description                             |   Indices in the ip_addr
 *               :----------------------------------------------------|:-----------------------------------------------------------
 *               Prefix Length                                        |   ip_addr[0]
 *               Link Local Address (128 bits)                        |   ip_addr[1] to ip_addr[4]
 *               Global Address (128 bits)                            |   ip_addr[5] to ip_addr[8]
 *               Gateway Address (128 bits)                           |   ip_addr[9] to ip_addr[12]
 * @param[in]    mask              - Pointer to network mask
 * @param[in]    gw                - Pointer to gateway address
 * @param[in]    ipconfig_rsp      - Hold the IP configuration received using DHCP.
 * @param[in]    length            - Length of ipconfig_rsp buffer
 * @param[in]    vap_id            - VAP id to differentiate between AP and station in concurrent mode. 0 – for station, 1 – for Access point
 * @return       0                - Success \n
 *               Non-Zero Value   - Failure (**Possible Error Codes** - 0xfffffffe,0xfffffffd,0xfffffffa) \n
 * @note         **Precondition** - \ref rsi_wlan_connect() API needs to be called before this API.
 * @note         Refer to \ref error-codes for the description of above error codes.
 *
 */
int32_t rsi_config_ipaddress(rsi_ip_version_t version,
                             uint8_t mode,
                             uint8_t *ip_addr,
                             uint8_t *mask,
                             uint8_t *gw,
                             uint8_t *ipconfig_rsp,
                             uint16_t length,
                             uint8_t vap_id)
{
  rsi_pkt_t *pkt;
  rsi_req_ipv4_parmas_t *ip_params;
  rsi_req_ipv6_parmas_t *ipv6_params;
  int32_t status                 = RSI_SUCCESS;
  int32_t rsi_response_wait_time = 0;
  // Get WLAN CB struct pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // WLAN radio init
  status = rsi_wlan_radio_init();
  SL_PRINTF(SL_WLAN_CONFIG_IPADDRESS_ENTRY, WLAN, LOG_INFO);
  if (status != RSI_SUCCESS) {
    // Return status if error in sending command occurs
    SL_PRINTF(SL_WLAN_CONFIG_IPADDRESS_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_CONFIG_IPADDRESS_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Attach the buffer given by user
    wlan_cb->app_buffer = (uint8_t *)ipconfig_rsp;

    // Length of the buffer provided by user
    wlan_cb->app_buffer_length = length;

    // If ipversion is IPv4
    if (version == RSI_IP_VERSION_4) {
      // Fill all the ipv4 parameters
      ip_params = (rsi_req_ipv4_parmas_t *)pkt->data;

      // Set DHCP mode
      ip_params->dhcp_mode = mode;

      // Fill IP address only if static mode is selected
      if (mode == RSI_STATIC) {
        // Fill IP address
        memcpy(ip_params->ipaddress, ip_addr, RSI_IPV4_ADDRESS_LENGTH);

        // Fill network mask
        memcpy(ip_params->netmask, mask, RSI_IPV4_ADDRESS_LENGTH);

        // Fill gateway
        memcpy(ip_params->gateway, gw, RSI_IPV4_ADDRESS_LENGTH);
      }

      // Check for DHCP HOSTNAME feature
      if ((mode & RSI_DHCP_HOSTNAME) == RSI_DHCP_HOSTNAME) {
        // Copy DHCP client hostname
        rsi_strcpy(ip_params->hostname, RSI_DHCP_HOST_NAME);
      }

      // Set vap_id
      ip_params->vap_id = vap_id;
#ifndef RSI_WLAN_SEM_BITMAP
      rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif

      // Send ipconfig command
      status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_IPCONFV4, pkt);
      rsi_response_wait_time = RSI_IPCONFV4_RESPONSE_WAIT_TIME;

    } else if (version == RSI_IP_VERSION_6) {
      // Fill all the IPv6 params
      ipv6_params = (rsi_req_ipv6_parmas_t *)pkt->data;

      // Fill prefix length
      rsi_uint16_to_2bytes(ipv6_params->prefixLength, 64);

      // Fill DHCP mode
      rsi_uint16_to_2bytes(ipv6_params->mode, mode);

      // Fill IP address only if static mode is selected
      if (mode == RSI_STATIC) {
        // Fill IPv6 address
        memcpy(ipv6_params->ipaddr6, ip_addr, RSI_IPV6_ADDRESS_LENGTH);

        // Fill gateway
        memcpy(ipv6_params->gateway6, gw, RSI_IPV6_ADDRESS_LENGTH);
      }

#ifndef RSI_WLAN_SEM_BITMAP
      rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
      // Send ipconfig command
      status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_IPCONFV6, pkt);
      rsi_response_wait_time = RSI_IPCONFV6_RESPONSE_WAIT_TIME;
    } else {

      // Detach buffer
      wlan_cb->app_buffer = NULL;

      // Make length of the buffer to 0
      wlan_cb->app_buffer_length = 0;

      // Update Throw error in case of invalid parameters
      status = RSI_ERROR_INVALID_PARAM;
      // Free the packet
      rsi_pkt_free(&wlan_cb->wlan_tx_pool, pkt);
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      SL_PRINTF(SL_WLAN_CONFIG_IPADDRESS_CHANGE_WLAN_CMD_STATE_TO_ALLOW, WLAN, LOG_ERROR, "status: %4x", status);
      return status;
    }

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_CONFIG_IPADDRESS_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_WLAN_CONFIG_IPADDRESS_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief      Write or erase certificate into module. This is a blocking API.
 * @param[in]  certificate_type   - Type of certificate
 * @param[in]  cert_inx 		      - Index of certificate
 * @param[in]  buffer 			      - Certificate content
 * @param[in]  certificate_length - Certificate length
 * @return     0                -  Success \n
 * @return     Non-Zero  Value  - Failure (**Possible Error Codes** - 0xfffffffe,0xfffffffd,0xfffffffa,0x0015,0x0021,0x0025,0x0026,0x002C) \n
 * @note       **Precondition** - \ref rsi_wireless_init() API must be called before this API.
 * @note 		   Index-based certificate loading is valid only for storing certificates on to RAM or flash but not both at the same time. \n
 * @note 		   Enable bit[27] in tcp_ip_feature_bit_map to load SSl certificate into RAM. \n
 * @note 		   Enable bit[31] in tcp_ip_feature_bit_map and bit[29] in ext_tcp_ip_feature_bit_map to open 3 SSL client sockets. \n
 * 				     Three SSL client sockets feature is supported only in WLAN mode.
 *
 *
 */
int32_t rsi_wlan_set_certificate_index(uint8_t certificate_type,
                                       uint8_t cert_inx,
                                       uint8_t *buffer,
                                       uint32_t certificate_length)
{
  static uint32_t rem_len;
  uint16_t chunk_size = 0;
  static uint32_t offset;
  rsi_pkt_t *pkt;
  int32_t status = RSI_SUCCESS;
  rsi_req_set_certificate_t *rsi_chunk_ptr;
  int32_t rsi_response_wait_time = 0;
  SL_PRINTF(SL_WLAN_SET_CERTIFICATE_INDEX_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Check if command is in correct state or not
  if ((wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE)) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_SET_CERTIFICATE_INDEX_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  if ((buffer == NULL) && (certificate_length)) {
    // Throw error in case of invalid parameters
    SL_PRINTF(SL_WLAN_SET_CERTIFICATE_INDEX_INVALID_PARAM, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_INVALID_PARAM;
  }

  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Get the certificate chunk size
    chunk_size = (RSI_MAX_CERT_SEND_SIZE - sizeof(struct rsi_cert_info_s));

    // Get certificate length
    rem_len = certificate_length;

    // If certificate length is 0, erase certificate
    if (rem_len == 0) {
      // Allocate command buffer from WLAN pool
      pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

      // If allocation of packet fails
      if (pkt == NULL) {
        // Change the WLAN CMD state to allow
        rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
        // Return packet allocation failure error
        SL_PRINTF(SL_WLAN_SET_CERTIFICATE_INDEX_PKT_ALLOCATION_FAILURE_1, WLAN, LOG_ERROR, "status: %4x", status);
        return RSI_ERROR_PKT_ALLOCATION_FAILURE;
      }

      rsi_chunk_ptr = (rsi_req_set_certificate_t *)pkt->data;

      // Memset the PKT
      memset(&pkt->data, 0, RSI_MAX_CERT_SEND_SIZE);

      //Set the total length of certificate
      rsi_uint16_to_2bytes(rsi_chunk_ptr->cert_info.total_len, certificate_length);

      // Set the certificate type
      rsi_chunk_ptr->cert_info.certificate_type = certificate_type;

      // Set the certificate index
      rsi_chunk_ptr->cert_info.certificate_inx = cert_inx;

      // More chunks to send
      rsi_chunk_ptr->cert_info.more_chunks = 0;

      // Set the length of the certificate chunk
      rsi_uint16_to_2bytes(rsi_chunk_ptr->cert_info.certificate_length, 0);

      rsi_response_wait_time = RSI_CERTIFICATE_RESPONSE_WAIT_TIME;
#ifndef RSI_WLAN_SEM_BITMAP
      rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
      // Send the driver command
      status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_CERTIFICATE, pkt);
      // Wait on WLAN semaphore
      rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);
      // Get WLAN/network command response status
      status = rsi_wlan_get_status();
      if (status != RSI_SUCCESS) {
        // Change the WLAN CMD state to allow
        rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

        // Return status if error in sending command occurs
        SL_PRINTF(SL_WLAN_SET_CERTIFICATE_INDEX_ERROR_IN_SENDING_COMMAND_1, WLAN, LOG_ERROR, "status: %4x", status);
        return status;
      }
    }
    while (rem_len) {
      // Allocate command buffer from WLAN pool
      pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

      // If allocation of packet fails
      if (pkt == NULL) {
        // Change the WLAN CMD state to allow
        rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
        // Return packet allocation failure error
        SL_PRINTF(SL_WLAN_SET_CERTIFICATE_INDEX_PKT_ALLOCATION_FAILURE_2, WLAN, LOG_ERROR, "status: %4x", status);
        return RSI_ERROR_PKT_ALLOCATION_FAILURE;
      }

      rsi_chunk_ptr = (rsi_req_set_certificate_t *)pkt->data;
      // Memset the PKT
      memset(&pkt->data, 0, RSI_MAX_CERT_SEND_SIZE);

      if (rem_len >= chunk_size) {
        // Copy the certificate chunk
        memcpy(rsi_chunk_ptr->certificate, buffer + offset, chunk_size);

        // Move the offset by chunk size
        offset += chunk_size;

        // Subtract the rem_len by the chunk size
        rem_len -= chunk_size;

        //Set the total length of certificate
        rsi_uint16_to_2bytes(rsi_chunk_ptr->cert_info.total_len, certificate_length);

        // Set the certificate type
        rsi_chunk_ptr->cert_info.certificate_type = certificate_type;

        // Set the certificate index
        rsi_chunk_ptr->cert_info.certificate_inx = cert_inx;

        // More chunks to send
        rsi_chunk_ptr->cert_info.more_chunks = 1;

        // Set the length of the certificate chunk
        rsi_uint16_to_2bytes(rsi_chunk_ptr->cert_info.certificate_length, chunk_size);

        rsi_response_wait_time = RSI_CERTIFICATE_RESPONSE_WAIT_TIME;
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send the driver command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_CERTIFICATE, pkt);
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);
        // Get WLAN/network command response status
        status = rsi_wlan_get_status();
        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_SET_CERTIFICATE_INDEX_ERROR_IN_SENDING_COMMAND_2, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }

      } else {

        // Copy the certificate chunk
        memcpy(rsi_chunk_ptr->certificate, buffer + offset, rem_len);

        // Move the offset by chunk size
        offset += rem_len;

        //Set the total length of certificate
        rsi_uint16_to_2bytes(rsi_chunk_ptr->cert_info.total_len, certificate_length);

        // Set the certificate type
        rsi_chunk_ptr->cert_info.certificate_type = certificate_type;

        // Set the certificate index
        rsi_chunk_ptr->cert_info.certificate_inx = cert_inx;

        // Last chunks to send
        rsi_chunk_ptr->cert_info.more_chunks = 0;

        // Set the length of the certificate chunk
        rsi_uint16_to_2bytes(rsi_chunk_ptr->cert_info.certificate_length, rem_len);

        rsi_response_wait_time = RSI_CERTIFICATE_RESPONSE_WAIT_TIME;
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send the driver command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_CERTIFICATE, pkt);
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);
        // Get WLAN/network command response status
        status = rsi_wlan_get_status();
        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_SET_CERTIFICATE_INDEX_ERROR_IN_SENDING_COMMAND_3, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }

        // Reset rem_len and offset
        rem_len = 0;
        offset  = 0;
      }
    }

    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_SET_CERTIFICATE_INDEX_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Get WLAN/network command response status
  status = rsi_wlan_get_status();

  // Return status
  SL_PRINTF(SL_WLAN_SET_CERTIFICATE_INDEX_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}
/** @} */

/** @addtogroup NETWORK16
* @{
*/
/*==============================================*/
/**
 * @brief      Load SSL/EAP certificate in the module. This is a blocking API.
 * @param[in]  certificate_type   - Type of certificate \n
 *                                  These are the certificate types: \n
 *                                  Value      |   Certificate type
 *                                  |:---------|:----------------------
 *                                  1          | TLS client certificate \n
 *                                  2          | FAST PAC file          \n
 *                                  3          | SSL Client Certificate \n
 *                                  4          | SSL Client Private Key \n
 *                                  5          | SSL CA Certificate     \n
 *                                  6          | SSL Server Certificate \n
 *                                  7          | SSL Server Private Key \n
 * @param[in]  buffer 			      - Pointer to a buffer which contains the certificate
 * @param[in]  certificate_length - Certificate length
 * @return     0               - Success \n
 * @return     Non-Zero  Value - Failure 	(**Possible Error Codes** - 0xfffffffe,0xfffffffd,0xfffffffa,0x0015,0x0021,0x0025,0x0026,0x002C) \n 
 * @note       **Precondition** - \ref rsi_wireless_init() API must be called before this API.
 * @note       Refer to \ref error-codes for the description of above error codes.
 *
 */
int32_t rsi_wlan_set_certificate(uint8_t certificate_type, uint8_t *buffer, uint32_t certificate_length)
{
  SL_PRINTF(SL_WLAN_SET_CERTIFICATE_EXIT, NETWORK, LOG_INFO);
  return rsi_wlan_set_certificate_index(certificate_type, 0, buffer, certificate_length);
}
/** @} */

/** @addtogroup WLAN
* @{
*/
/*==============================================*/
/**
 * @brief       Check the error code encountered during a call to a WLAN API or BSD sockets functions. This is a non-blocking API.
 * @param[in]   Void
 * @return      Returns the error code that previously occurred. If no error occurred, returns 0.
 * @note        Refer to \ref error-codes for the description of above error codes.
 */
int32_t rsi_wlan_get_status(void)
{
  SL_PRINTF(SL_WLAN_GET_STATUS_EXIT, WLAN, LOG_INFO);
  return rsi_driver_cb->wlan_cb->status;
}

/*==============================================*/
/**
 * @brief     Get the required information based on the type of command. This is a blocking API.
 * @param[in] cmd_type   -  Query command type: \n
 *                         Value |	Command Type
 *                         :---|:---------------------
 *                      	 1 | RSI_FW_VERSION \n
 *              		       2 | RSI_MAC_ADDRESS \n
 *              		       3 | RSI_RSSI \n
 *              		       4 | RSI_WLAN_INFO \n
 *              		       5 | RSI_CONNECTION_STATUS \n
 *              		       6 | RSI_STATIONS_INFO \n
 *              		       7 | RSI_SOCKETS_INFO \n
 *              		       8 | RSI_CFG_GET \n
 *                         9 | RSI_GET_WLAN_STAT
 * @param[out] response  - Response of the requested command.
 * @param[in]  length     - Length of the response buffer in bytes to hold result.
 * @note	     RSI_WLAN_INFO is relevant in both station and AP mode. \n
 * @note	     RSI_SOCKETS_INFO is relevant in both station mode and AP mode. \n
 * @note	     RSI_STATIONS_INFO is relevant in AP mode \n
 * @note	     RSI_GET_WLAN_STATS is relevant in AP and Station mode
 * @return   	 0 	             - Success 
 * @return   	 Non-Zero Value  - Failure (**Possible Error Codes** - 0xfffffffd,0xfffffffc,0xfffffffa) \n           
 * @note       **Precondition** - \ref rsi_wireless_init () API needs to be called before this API.
 * @note       Refer to \ref error-codes for the description of above error codes.
 *
 *
 */
int32_t rsi_wlan_get(rsi_wlan_query_cmd_t cmd_type, uint8_t *response, uint16_t length)
{
  int32_t status = RSI_SUCCESS;
  rsi_pkt_t *pkt;
  int32_t rsi_response_wait_time = 0;
  rsi_timer_instance_t timer_instance;

  SL_PRINTF(SL_WLAN_GET_ENTRY, WLAN, LOG_INFO);

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Get common control block structure pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  rsi_init_timer(&timer_instance, RSI_CARD_READY_WAIT_TIME);
  // If state is not in card ready received state
  if (common_cb->state == RSI_COMMON_STATE_NONE) {
    if (cmd_type == RSI_FW_VERSION) {
      while (common_cb->state != RSI_COMMON_CARDREADY) {
#ifndef RSI_WITH_OS
        rsi_scheduler(&rsi_driver_cb->scheduler_cb);
#endif
        if (rsi_timer_expired(&timer_instance)) {
          SL_PRINTF(SL_WIRELESS_INIT_CARD_READY_TIMEOUT, COMMON, LOG_ERROR);
          return RSI_ERROR_CARD_READY_TIMEOUT;
        }
      }
    } else {
      // Command given in wrong state
      SL_PRINTF(SL_WLAN_GET_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
    }
  }

  if (cmd_type == RSI_FW_VERSION) {
    // Send firmware version query request
    status = rsi_get_fw_version(response, length);
    SL_PRINTF(SL_WLAN_GET_FW_VERSION_QUERY_REQ, WLAN, LOG_INFO, "status: %4x", status);
    return status;
  }

  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Copy query CMD type in driver control block
    wlan_cb->query_cmd = cmd_type;

    if (response != NULL) {
      // Attach the buffer given by user
      wlan_cb->app_buffer = response;

      // Length of the buffer provided by user
      wlan_cb->app_buffer_length = length;
    } else {
      // Assign NULL to the app_buffer to avoid junk
      wlan_cb->app_buffer = NULL;

      // Length of the buffer to 0
      wlan_cb->app_buffer_length = 0;
    }
    switch (cmd_type) {
      case RSI_RSSI: {
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_GET_PKT_ALLOCATION_FAILURE_1, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
        rsi_response_wait_time = RSI_WLAN_RSSI_RESPONSE_WAIT_TIME;
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send RSSI query request
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_RSSI, pkt);
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);

      } break;
      case RSI_MAC_ADDRESS: {
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_GET_PKT_ALLOCATION_FAILURE_2, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send MAC address query request
        status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_MAC_ADDRESS, pkt);
        rsi_response_wait_time = RSI_MAC_RESPONSE_WAIT_TIME;
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);

      } break;
      case RSI_CONNECTION_STATUS: {
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_GET_PKT_ALLOCATION_FAILURE_3, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send MAC address query request
        status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_CONNECTION_STATUS, pkt);
        rsi_response_wait_time = RSI_CONNECT_RESPONSE_WAIT_TIME;
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);

      } break;
      case RSI_WLAN_INFO: {
        if (length < sizeof(rsi_rsp_wireless_info_t)) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          SL_PRINTF(SL_WLAN_GET_INSUFFICIENT_BUFFER_1, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_INSUFFICIENT_BUFFER;
        }
        if (wlan_cb->opermode == RSI_WLAN_ACCESS_POINT_MODE) {
          // Allocate command buffer from WLAN pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

          // If allocation of packet fails
          if (pkt == NULL) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // Return packet allocation failure error
            SL_PRINTF(SL_WLAN_GET_PKT_ALLOCATION_FAILURE_4, WLAN, LOG_ERROR, "status: %4x", status);
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }
#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // Send CMD for WLAN info in AP mode
          status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_QUERY_GO_PARAMS, pkt);
          rsi_response_wait_time = RSI_WLAN_QUERY_NETWORK_PARAMS_WAIT_TIME;
          // Wait on WLAN semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);
        } else if ((wlan_cb->opermode == RSI_WLAN_CLIENT_MODE) || (wlan_cb->opermode == RSI_WLAN_ENTERPRISE_CLIENT_MODE)
                   || (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE)) {
          // Allocate command buffer from WLAN pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

          // If allocation of packet fails
          if (pkt == NULL) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // Return packet allocation failure error
            SL_PRINTF(SL_WLAN_GET_PKT_ALLOCATION_FAILURE_5, WLAN, LOG_ERROR, "status: %4x", status);
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }
#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // Send CMD for WLAN info in client mode
          status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_QUERY_NETWORK_PARAMS, pkt);
          rsi_response_wait_time = RSI_WLAN_QUERY_NETWORK_PARAMS_WAIT_TIME;
          // Wait on WLAN semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);
        } else if (wlan_cb->opermode == RSI_WLAN_WIFI_DIRECT_MODE) {
          // No code here
        }
      } break;
      case RSI_STATIONS_INFO: {
        if (length < sizeof(rsi_rsp_stations_info_t)) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          SL_PRINTF(SL_WLAN_GET_INSUFFICIENT_BUFFER_2, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_INSUFFICIENT_BUFFER;
        }
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_GET_PKT_ALLOCATION_FAILURE_6, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send MAC address query request
        status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_QUERY_GO_PARAMS, pkt);
        rsi_response_wait_time = RSI_WLAN_REQ_QUERY_GO_PARAMS;
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);

      } break;
      case RSI_SOCKETS_INFO: {
        if (length < sizeof(rsi_rsp_sockets_info_t)) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          SL_PRINTF(SL_WLAN_GET_INSUFFICIENT_BUFFER_3, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_INSUFFICIENT_BUFFER;
        }

        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_GET_PKT_ALLOCATION_FAILURE_7, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send MAC address query request
        status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_QUERY_NETWORK_PARAMS, pkt);
        rsi_response_wait_time = RSI_WLAN_QUERY_NETWORK_PARAMS_WAIT_TIME;
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);

      } break;
      case RSI_CFG_GET: {
        if (length < sizeof(rsi_cfgGetFrameRcv_t)) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          SL_PRINTF(SL_WLAN_GET_INSUFFICIENT_BUFFER_4, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_INSUFFICIENT_BUFFER;
        }
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_GET_PKT_ALLOCATION_FAILURE_8, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send CFG_GET command for getting user store configuration
        status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_GET_CFG, pkt);
        rsi_response_wait_time = RSI_WLAN_REQ_GET_CFG_WAIT_TIME;
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);

      } break;
      case RSI_GET_WLAN_STATS: {

        if (length < sizeof(rsi_rsp_wlan_stats_t)) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          SL_PRINTF(SL_WLAN_GET_INSUFFICIENT_BUFFER_5, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_INSUFFICIENT_BUFFER;
        }
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_GET_PKT_ALLOCATION_FAILURE_9, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send Wi-fi STATS query request
        status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_GET_STATS, pkt);
        rsi_response_wait_time = RSI_WLAN_REQ_GET_STATS_WAIT_TIME;
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);

      } break;
      case RSI_WLAN_EXT_STATS: {

        if (length < sizeof(rsi_wlan_ext_stats_t)) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          //   SL_PRINTF(SL_WLAN_GET_INSUFFICIENT_BUFFER_5, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_INSUFFICIENT_BUFFER;
        }
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          //  SL_PRINTF(SL_WLAN_GET_PKT_ALLOCATION_FAILURE_9, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send Wi-fi STATS query request
        status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_EXT_STATS, pkt);
        rsi_response_wait_time = RSI_WLAN_REQ_EXT_STATS_WAIT_TIME;
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);
      } break;
      case RSI_FW_VERSION:
        break;
      case RSI_WMM_PARAMS: {
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_GET_PKT_ALLOCATION_FAILURE_1, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
        rsi_response_wait_time = RSI_WMM_PARAMS_RESPONSE_WAIT_TIME;
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send WMM PARAMS query request
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_WMM_PARAMS, pkt);
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);

      } break;
      default:
        // Return status if command given in driver is in an invalid state
        status = RSI_ERROR_COMMAND_NOT_SUPPORTED;
        break;
    }
    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_GET_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_WLAN_GET_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief       Request configuration based on the command type. This is a blocking API.
 * @param[in]   cmd_type  - 	Set command type:  : \n
 *                            Value |	Command Type
 *                            :---|:---------------------:
 *                       	    1 | RSI_SET_MAC_ADDRESS \n
 *                       	    2 | RSI_MULTICAST_FILTER \n
 *                      	    3 | RSI_JOIN_BSSID \n
 *                      	    4 | RSI_CFG_SAVE  \n
 *                      	    5 | RSI_CFG_STORE  \n
 * @param[in]  request    -   Request buffer
 * @param[in]  length     -   Length of the request buffer in bytes
 * 		         cmd type                |	Request structure	
 * 		         :-----------------------|:--------------------------------------------------------------------------------------------------:
 * 	 	         RSI_SET_MAC_ADDRESS     |      uint8_t mac_address[6]
 * 	 	         RSI_MULTICAST_FILTER    |      typedef struct rsi_req_multicast_filter_info_s
 *	           ^        	                 	  { uint8_t cmd_type;
 *	           ^        	                 	    uint8_t mac_address[6];
 *	           ^                                }rsi_req_multicast_filter_info_t;
 *	           ^                              cmd_type are : \n
 *             ^                              1 . RSI_ MULTICAST_MAC_ADD_BIT (To set particular bit in multicast bitmap)
 *	           ^        	                 	  2 . RSI_MULTICAST_MAC_CLEAR_BIT (To reset particular bit in multicast bitmap
 *	           ^        	                 	  3 . RSI_MULTICAST_MAC_CLEAR_ALL (To clear all the bits in multicast bitmap) \n
 *	           ^        	                 	  4 . RSI_MULTICAST_MAC_SET_ALL (To set all the bits in multicast bitmap) \n
 * 	 	         RSI_JOIN_BSSID          |      uint8_t join_bssid[6]
 * 	 	         RSI_CFG_SAVE            |      This cmd_type is used to save the parameters into non-volatile memory which are used either to join to an Access point (auto-join mode) or to create an Access point(auto-create mode).
 * 	 	         RSI_CFG_STORE           |      This cmd_type is used to give the configuration values which are supposed to be stored in the module's non-volatile memory and that are used in auto-join or auto-create modes. 
 *	           ^        	                  	User configurations are stored in rsi_user_store_config_t structure. \n
 * @return     0              -  Success \n
 *             Non-Zero	Value - Failure (**Possible Error Codes** - 0xfffffffe,0xfffffffd,0xfffffffc,0x0002, 0x0003, 0x0005, 0x000A, 0x0014, 0x0015, 0x001A, 0x0021,0x0024,0x0025,0x0026,0x002C,0x003c) \n
 * @note       **Precondition** - \ref rsi_wireless_init() API needs to be called before this API. \n
 *        		  For setting MAC address, call this API immediately after \n
 *		  		    \ref rsi_wireless_init() and before calling any other API. \n
 *              \ref rsi_config_ipaddress() needs to be call for RSI_CFG_SAVE and RSI_CFG_STORE.
 * @note       Refer to \ref error-codes for the description of above error codes.
 *
 *
 */
int32_t rsi_wlan_set(rsi_wlan_set_cmd_t cmd_type, uint8_t *request, uint16_t length)
{
  // Statement added to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(length);
  int32_t status = RSI_SUCCESS;

  SL_PRINTF(SL_WLAN_SET_ENTRY, WLAN, LOG_INFO);
  rsi_pkt_t *pkt;
  rsi_user_store_config_t *user_store_ptr = NULL;
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb         = rsi_driver_cb->wlan_cb;
  int32_t rsi_response_wait_time = 0;
  // Get common control block structure pointer
  rsi_common_cb_t *common_cb = rsi_driver_cb->common_cb;

  // If state is not in card ready received state
  if (common_cb->state == RSI_COMMON_STATE_NONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_SET_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  if (!(cmd_type == 4)) {
    if (request == NULL) {
      // Command given in wrong state
      SL_PRINTF(SL_WLAN_SET_NVALID_PARAM, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_INVALID_PARAM;
    }
  }

  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    switch (cmd_type) {
      case RSI_SET_MAC_ADDRESS: {
        // Enable valid bit to ensure setting MAC id before every initilization
        wlan_cb->field_valid_bit_map |= RSI_SET_MAC_BIT;

        // Copy MAC address in to WLAN control block
        memcpy(wlan_cb->mac_address, request, 6);

      } break;

      case RSI_MULTICAST_FILTER: {
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SET_PKT_ALLOCATION_FAILURE_1, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset data
        memset(&pkt->data, 0, sizeof(rsi_req_multicast_filter_info_t));

        // Copy MAC address
        memcpy((uint8_t *)&pkt->data, request, sizeof(rsi_req_multicast_filter_info_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Set multicast filter configuration
        status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_MULTICAST_FILTER, pkt);
        rsi_response_wait_time = RSI_MULTICAST_FIL_RESPONSE_WAIT_TIME;
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);
      } break;

      case RSI_JOIN_BSSID: {

        // Copy MAC address in to WLAN control block
        memcpy(rsi_wlan_cb_non_rom->join_bssid_non_rom, request, 6);

      } break;

      case RSI_CFG_SAVE: {
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SET_PKT_ALLOCATION_FAILURE_2, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send CFG_SAVE command for saving user store configuration.
        status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_CFG_SAVE, pkt);
        rsi_response_wait_time = RSI_WLAN_CFG_SAVE_WAIT_TIME;
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);

      } break;
      case RSI_CFG_STORE: {
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_SET_PKT_ALLOCATION_FAILURE_3, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
        user_store_ptr = (rsi_user_store_config_t *)pkt->data;
        // Memset buffer
        memset(&pkt->data, 0, sizeof(rsi_user_store_config_t));
        // Copy user store configuration parameter to the packet
        memcpy(user_store_ptr, (rsi_user_store_config_t *)request, sizeof(rsi_user_store_config_t));
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send PSK command
        status                 = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_USER_STORE_CONFIG, pkt);
        rsi_response_wait_time = RSI_USER_SC_RESPONSE_WAIT_TIME;
        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, rsi_response_wait_time);

      } break;

      default:
        // Return status if command given in driver is in an invalid state
        status = RSI_ERROR_COMMAND_NOT_SUPPORTED;
        break;
    }
    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_SET_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_WLAN_SET_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @fn       int32_t rsi_wlan_buffer_config(void)
 * @brief    Configure the TX and RX global buffers ratio. This is a blocking API.
 * @param[in]	dynamic_tx_pool	- Configure the dynamic tx ratio
 * @param[in]	dynamic_rx_pool	- Configure the dynamic rx ratio
 * @param[in]	dynamic_global_pool - Configure the dynamic global ratio
 * @return    0 		- Success \n
 * @return    Non-Zero Value  - Failure (**Possible Error Codes** - 0xfffffffe,0xfffffffd,0xfffffffa,0x0021) \n
 * @note      **Precondition** - \ref rsi_wireless_init() API needs to be called before this API. 
 * @note      Refer to \ref error-codes for the description of above error codes.
 *
 */
int32_t rsi_wlan_buffer_config(void)
{
  rsi_pkt_t *pkt;
  rsi_udynamic *dyn_buf;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_BUFFER_CONFIG_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_BUFFER_CONFIG_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_BUFFER_CONFIG_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Take the user provided data and fill it in antenna select structure
    dyn_buf = (rsi_udynamic *)pkt->data;

    // PLL mode value
    dyn_buf->dynamic_tx_pool = TX_POOL_RATIO;

    // RF type
    dyn_buf->dynamic_rx_pool = RX_POOL_RATIO;

    // Wireless mode
    dyn_buf->dynamic_global_pool = GLOBAL_POOL_RATIO;

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send antenna select command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_DYNAMIC_POOL, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_DYNAMIC_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_BUFFER_CONFIG_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status
  SL_PRINTF(SL_WLAN_BUFFER_CONFIG_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief      Start the module in access point mode with the given configuration. This is a blocking API.
 * @param[in]  ssid    - SSID of the access point. Length of the SSID should be less than or equal to 32 bytes.
 * @param[in]  channel - Channel number. Refer the following channels for the valid channel numbers supported: \n
 *                       2.4GHz Band Channel Mapping, 5GHz Band Channel Mapping \n
 *                      channel number 0 is to enable ACS feature
 *			###The following table maps the channel number to the actual radio frequency in the 2.4 GHz spectrum###
 *			Channel numbers (2.4GHz)|	Center frequencies for 20MHz channel width 
 *			:----------------------:|:-----------------------------------------------:
 *			1			|	2412 
 *			2			|	2417  
 *			3			|	2422 
 *			4			|	2427
 *			5			|	2432 
 *			6			|	2437 
 *			7			|	2442 
 *			8			|	2447 
 *			9			|	2452
 *			10		|	2457  
 *			11		|	2462 
 *			12		|	2467 
 *			13		|	2472 
 *			14		|	2484 
 * 	  ###The following table maps the channel number to the actual radio frequency in the 5 GHz spectrum###
 * 		Channel Numbers(5GHz) |	Center frequencies for 20MHz channel width 
 * 		:--------------------:|:------------------------------------------:
 *		36		      |5180 
 *		40		      |5200 
 *		44		      |5220 
 *		48		      |5240 
 *		144		      |5700 
 *		149		      |5745 
 *		153		      |5765 
 *		157		      |5785 
 *		161		      |5805 
 *		165		      |5825 
 * @param[in]  security_type - Type of the security modes on which an access point needs to be operated: \n
 * 		                     Value |	Security type 
 * 		                     :----:|:------------------------------------------:
 *                         1     | RSI_OPEN \n
 *                         2     | RSI_WPA \n
 *                         3     | RSI_WPA2 \n
 *                         4     | RSI_WPA_WPA2_MIXED \n
 *                         5     | RSI_WPS_PUSH_BUTTON
 * @param[in]  encryption_mode - Type of the encryption mode: \n
 * 		                     Value |	Encryption mode 
 * 		                     :----:|:------------------------------------------:
 *                         0     | RSI_NONE \n
 *                         1     | RSI_TKIP \n
 *                         2     | RSI_CCMP
 * @param[in]  password - PSK to be used in security mode. \n
 *                        Minimum and maximum length of PSK is 8 bytes and 63 bytes respectively
 * @param[in]  beacon_interval - Beacon interval in ms. Allowed values are integers from 100 to 1000 which are multiples of 100.
 * @param[in]  dtim_period     - DTIM period. Allowed values are integers between 1 and 255.
 * @return     0               - Success \n
 * @return     Non-Zero Value  - Failure (**Possible Error Codes** - 0xfffffffe,0xfffffffd,0xfffffffa) \n      
 * @note       **Precondition** - \ref rsi_wireless_init() API needs to be called before this API.
 * @note	     DFS channels are not supported in AP mode.
 * @note       Refer to \ref error-codes for the description of above error codes.
 *
 *
 */
int32_t rsi_wlan_ap_start(int8_t *ssid,
                          uint8_t channel,
                          rsi_security_mode_t security_type,
                          rsi_encryption_mode_t encryption_mode,
                          uint8_t *password,
                          uint16_t beacon_interval,
                          uint8_t dtim_period)
{

  rsi_pkt_t *pkt;
  rsi_req_join_t *join;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_AP_START_ENTRY, WLAN, LOG_INFO);
  rsi_req_ap_config_t *ap_config;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_AP_START_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    switch (wlan_cb->state) {
      case RSI_WLAN_STATE_OPERMODE_DONE: {
        if (wlan_cb->field_valid_bit_map & RSI_SET_MAC_BIT) {
          // Allocate command buffer from WLAN pool
          pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

          // If allocation of packet fails
          if (pkt == NULL) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
            // Return packet allocation failure error
            SL_PRINTF(SL_WLAN_AP_START_PKT_ALLOCATION_FAILURE_1, WLAN, LOG_ERROR, "status: %4x", status);
            return RSI_ERROR_PKT_ALLOCATION_FAILURE;
          }
          // Memset data
          memset(&pkt->data, 0, sizeof(rsi_req_mac_address_t));

          // Memcpy data
          memcpy(&pkt->data, wlan_cb->mac_address, sizeof(rsi_req_mac_address_t));

#ifndef RSI_WLAN_SEM_BITMAP
          rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
          // Send set MAC command
          status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_MAC_ADDRESS, pkt);

          // Wait on WLAN semaphore
          rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_MAC_RESPONSE_WAIT_TIME);

          // Get WLAN/network command response status
          status = rsi_wlan_get_status();

          if (status != RSI_SUCCESS) {
            // Change the WLAN CMD state to allow
            rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

            // Return status if error in sending command occurs
            SL_PRINTF(SL_WLAN_AP_START_ERROR_IN_SENDING_COMMAND_1, WLAN, LOG_ERROR, "status: %4x", status);
            return status;
          }
        }
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_AP_START_PKT_ALLOCATION_FAILURE_2, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset data
        memset(&pkt->data, 0, sizeof(rsi_req_band_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send band command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_BAND, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_BAND_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // If BAND command fails
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_AP_START_ERROR_IN_SENDING_COMMAND_2, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }
      }
        // No break
      // Fall through
      case RSI_WLAN_STATE_BAND_DONE: {
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_AP_START_PKT_ALLOCATION_FAILURE_3, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send init command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_INIT, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_INIT_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_AP_START_ERROR_IN_SENDING_COMMAND_3, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }
#if RSI_SET_REGION_AP_SUPPORT
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_AP_START_PKT_ALLOCATION_FAILURE_4, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset data
        memset(&pkt->data, 0, sizeof(rsi_req_set_region_ap_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send set region AP command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_REGION_AP, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_REGION_AP_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_AP_START_ERROR_IN_SENDING_COMMAND_4, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }
#endif
      }
        // No break
      // Fall through
      case RSI_WLAN_STATE_CONNECTED: {
        // Reject if AP is already up, allow if connected state is due to STA
        if (wlan_cb->opermode == RSI_WLAN_CONCURRENT_MODE && wlan_cb->ap_state >= RSI_WLAN_STATE_CONNECTED) {
          status = RSI_ERROR_COMMAND_NOT_SUPPORTED;
          break;
        }
      }
        // No break
      // Fall through
      case RSI_WLAN_STATE_IP_CONFIG_DONE:
      case RSI_WLAN_STATE_IPV6_CONFIG_DONE:
      case RSI_WLAN_STATE_INIT_DONE: {
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_AP_START_PKT_ALLOCATION_FAILURE_4, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
        // Memset packet
        memset(&pkt->data, 0, sizeof(rsi_req_ap_config_t));

        ap_config = (rsi_req_ap_config_t *)pkt->data;

        // Copy channel by uint16 to 2 bytes conversion
        rsi_uint16_to_2bytes(ap_config->channel, channel);

        if (ssid != NULL) {
          // Copy Join SSID
          rsi_strcpy(ap_config->ssid, ssid);
        }

        // Security type
        ap_config->security_type = security_type;
        if (ap_config->security_type == RSI_WPA3) {
          ap_config->security_type = SME_WPA3;
        }
        if (ap_config->security_type == RSI_WPA3_TRANSITION) {
          ap_config->security_type = SME_WPA3_TRANSITION;
        }

        // Encryption mode
        ap_config->encryption_mode = encryption_mode;

        // If security is enabled
        if (security_type) {
          // Copy PSK
          rsi_strcpy(ap_config->psk, password);
        }

        // Copy beacon interval by uint16 to 2 bytes conversion
        rsi_uint16_to_2bytes(ap_config->beacon_interval, beacon_interval);

        // Copy DTIM period by uint16 to 2 bytes conversion
        rsi_uint16_to_2bytes(ap_config->dtim_period, dtim_period);

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send AP config command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_AP_CONFIGURATION, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_AP_CONFIG_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        // Check status for AP config command
        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_AP_START_ERROR_IN_SENDING_COMMAND_5, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }

#if RSI_MODE_11N_ENABLE
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_AP_START_PKT_ALLOCATION_FAILURE_5, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }

        // Memset packet
        memset(&pkt->data, 0, sizeof(rsi_req_ap_ht_caps_t));

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send HT capabilities command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_HT_CAPABILITIES, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_HT_CAPS_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_AP_START_ERROR_IN_SENDING_COMMAND_6, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }
#endif
        // Allocate command buffer from WLAN pool
        pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

        // If allocation of packet fails
        if (pkt == NULL) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
          // Return packet allocation failure error
          SL_PRINTF(SL_WLAN_AP_START_PKT_ALLOCATION_FAILURE_6, WLAN, LOG_ERROR, "status: %4x", status);
          return RSI_ERROR_PKT_ALLOCATION_FAILURE;
        }
        // Memset the PKT
        memset(&pkt->data, 0, sizeof(rsi_req_join_t));

        join = (rsi_req_join_t *)pkt->data;

        if (ssid != NULL) {
          // Copy SSID and SSID len
          join->ssid_len = rsi_strlen(ssid);

          // Copy Join SSID
          rsi_strcpy(join->ssid, ssid);
        }

        // vap id of the current mode 0 - station mode, 1 - AP1 mode
        //  Applicable in cocurrent mode only
#if CONCURRENT_MODE
        join->vap_id = 1;
#else
        join->vap_id = 0;
#endif

        // Deregister join response handler
        rsi_wlan_cb_non_rom->callback_list.wlan_join_response_handler = NULL;

#ifndef RSI_WLAN_SEM_BITMAP
        rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
        // Send join command
        status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_JOIN, pkt);

        // Wait on WLAN semaphore
        rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_JOIN_RESPONSE_WAIT_TIME);

        // Get WLAN/network command response status
        status = rsi_wlan_get_status();

        if (status != RSI_SUCCESS) {
          // Change the WLAN CMD state to allow
          rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

          // Return status if error in sending command occurs
          SL_PRINTF(SL_WLAN_AP_START_ERROR_IN_SENDING_COMMAND_7, WLAN, LOG_ERROR, "status: %4x", status);
          return status;
        }

      } break;

      case RSI_WLAN_STATE_NONE:
      case RSI_WLAN_STATE_SCAN_DONE:
      case RSI_WLAN_STATE_AUTO_CONFIG_GOING_ON:
      case RSI_WLAN_STATE_AUTO_CONFIG_DONE:
      case RSI_WLAN_STATE_AUTO_CONFIG_FAILED:

        break;
      default: {
        // Return status if command given in driver is in an invalid state
        status = RSI_ERROR_COMMAND_NOT_SUPPORTED;
      } break;
    }
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return NWK command error
    SL_PRINTF(SL_WLAN_AP_START_NWK_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_AP_START_ERROR_IN_SENDING_COMMAND_8, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @fn          int32_t rsi_wlan_power_save_with_listen_interval(uint8_t psp_mode, uint8_t psp_type,uint16_t listen_interval)
 * @brief       Set the power save profile in WLAN mode with listen interval-based wakeup. This is a blocking API.
 * @param[in]   psp_mode      - PSP modes available are mentioned below
 *              Parameter              |  Description	
 * 		          :----------------------|:--------------------------------------------------------------------------------------------------
 *              Active(0)              |   In this mode, module is active and power save is disabled. \n
 *              RSI_SLEEP_MODE_1 (1)   |   Connected sleep mode. \n
 *              ^                          In this sleep mode, SoC will never turn off, therefore no handshake is required before sending data to the module. \n
 *              RSI_SLEEP_MODE_2 (2)   |   In this sleep mode, SoC will go to LP/ULP (with/without RAM RETENTION) sleep based on the selected value set for RSI_SELECT_LP_OR_ULP_MODE in rsi_wlan_config.h. \n
 *              ^                          Therefore handshake is required before sending data to the module. \n
 * 		          RSI_SLEEP_MODE_8 (8)   |   Deep sleep mode with ULP RAM RETENTION. In deep sleep mode, module will turn off the SoC and a GPIO or Message based handshake is required before sending commands to the module. \n
 *              RSI_SLEEP_MODE_10 (10) |   Deep sleep mode without ULP RAM RETENTION. \n                  
 * @param[in]   psp_type      - PSP types available are mentioned below
 *              Parameter              |  Description	
 * 		          :----------------------|:-------------------------------------------------------------------------------------------------- 	          	
 * 		          RSI_MAX_PSP (0)        |   This psp_type will be used for max power saving. \n
 * 		          RSI_FAST_PSP (1)       |   This psp_type allows module to disable power save for any Tx / Rx packet for monitor interval of time \n
 * 		          ^                          (monitor interval can be set by RSI_MONITOR_INTERVAL in rsi_wlan_config.h file, default value is 50 ms). \n
 * 		          ^                          If there is no data for monitor interval of time then module will again enable power save. \n
 * 		          RSI_UAPSD (2)          |   This psp_type is used to enable WMM power save.
 * @param[in]   listen_interval        -   Used to configure sleep duration in power save and should be less than the listen interval configured by RSI_LISTEN_INTERVAL Macro in join command parameters in rsi_wlan_config.h file. \n 
 *                                         Valid only if BIT (7) in join_feature_bit_map is set. This value is given in time units (1024 microsecond). \n
 * @note        **Precondition** - \ref rsi_wireless_init() API needs to be called before this API. 
 * @note    		psp_type is only valid in psp_mode 1 and 2. \n
 * @note			  psp_type: RSI_UAPSD is applicable only, if WMM_PS is enabled in rsi_wlan_config.h file. \n
 * @note			  In RSI_MAX_PSP mode, Few access points will not aggregate the packets, when power save is enabled from STA. This may cause the drop in throughputs.
 * @note        For the power save mode 3, select RSI_SLEEP_MODE_2 in psp_mode and RSI_HAND_SHAKE_TYPE as MSG_BASED in rsi_wlan_config.h file. \n
 * @note        For the power save mode 9, select RSI_SLEEP_MODE_8 in psp_mode and RSI_HAND_SHAKE_TYPE as MSG_BASED in rsi_wlan_config.h file. \n
 * @note        For the deep sleep without ram retention case, select RSI_SLEEP_MODE_10 in psp_mode and RSI_HAND_SHAKE_TYPE as MSG_BASED for msg_based or GPIO_BASED for gpio_based. \n
 * @note        For LP sleep, select RSI_SLEEP_MODE_2 in psp_mode, select RSI_SELECT_LP_OR_ULP_MODE as RSI_LP_MODE and RSI_HAND_SHAKE_TYPE as MSG_BASED/GPIO_BASED in rsi_wlan_config.h file. \n 
 * @return      0               - Success \n
 *              Non-Zero	Value - Failure (**Possible Error Codes** - 0xfffffffe,0xfffffffd,0xfffffffa) \n
 * @note        Refer to \ref error-codes for the description of above error codes.
 */

int32_t rsi_wlan_power_save_with_listen_interval(uint8_t psp_mode, uint8_t psp_type, uint16_t listen_interval)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_POWER_SAVE_LISTEN_INTERVAL_ENTRY, WLAN, LOG_INFO);
  // Get commmon CB pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  // Keep backup of power save profile mode and type
  rsi_common_cb->power_save.wlan_psp_type = psp_type;

  // Keep backup of power save profile mode and type
  rsi_common_cb->power_save.wlan_psp_mode = psp_mode;

  rsi_wlan_cb_non_rom->ps_listen_interval = listen_interval;

  status = rsi_sleep_mode_decision(rsi_common_cb);
  SL_PRINTF(SL_WLAN_POWER_SAVE_LISTEN_INTERVAL_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief       Set the power save profile in WLAN mode. This is a blocking API. 
 * @param[in]   psp_mode      - PSP modes available are mentioned below
 *              Parameter              |  Description	
 * 		          :----------------------|:--------------------------------------------------------------------------------------------------
 *              Active(0)              |   In this mode, module is active and power save is disabled. \n
 *              RSI_SLEEP_MODE_1 (1)   |   Connected sleep mode. \n
 *              ^                          In this sleep mode, SoC will never turn off, therefore no handshake is required before sending data to the module. \n
 *              RSI_SLEEP_MODE_2 (2)   |   In this sleep mode, SoC will go to LP/ULP (with/without RAM RETENTION) sleep based on the selected value set for RSI_SELECT_LP_OR_ULP_MODE in rsi_wlan_config.h. \n
 *              ^                          Therefore handshake is required before sending data to the module. \n
 * 		          RSI_SLEEP_MODE_8 (8)   |   Deep sleep mode with ULP RAM RETENTION. In deep sleep mode, module will turn off the SoC and a GPIO or Message based handshake is required before sending commands to the module. \n
 *              RSI_SLEEP_MODE_10 (10) |   Deep sleep mode without ULP RAM RETENTION. \n                  
 * @param[in]   psp_type      - PSP types available are mentioned below
 *              Parameter              |  Description	
 * 		          :----------------------|:-------------------------------------------------------------------------------------------------- 	          	
 * 		          RSI_MAX_PSP (0)        |   This psp_type will be used for max power saving. \n
 * 		          RSI_FAST_PSP (1)       |   This psp_type allows module to disable power save for any Tx / Rx packet for monitor interval of time \n
 * 		          ^                          (monitor interval can be set by RSI_MONITOR_INTERVAL in rsi_wlan_config.h file, default value is 50 ms). \n
 * 		          ^                          If there is no data for monitor interval of time then module will again enable power save. \n
 * 		          RSI_UAPSD (2)          |   This psp_type is used to enable WMM power save.
 *             ### Enhanced max psp ###
 *		         Enhanced max PSP is recommended. This is essentially a RSI_MAX_PSP but switches to RSI_FAST_PSP, if AP does not deliver data within 20ms for PS-Poll. \n
 *		         To enable this mode, follow this procedure: \n
 *             Add ENABLE_ENHANCED_MAX_PSP (BIT[26]) in RSI_CONFIG_FEATURE_BITMAP, \n
 *             Set psp_type to RSI_FAST_PSP (1) \n
 * @note	     **Precondition** - \ref rsi_wireless_init() API needs to be called before this API. 
 * @note	     Configure the monitor interval using the RSI_MONITOR_INTERVAL macro in rsi_wlan_config.h file. (default value is 50 ms) \n
 * @note	     psp_type is only valid in psp_mode 1 and 2. \n
 * @note	     psp_type - UAPSD is applicable only if WMM_PS is enabled in rsi_wlan_config.h file. \n
 * @note       In RSI_MAX_PSP mode, Few Access points won't aggregate the packets, when power save is enabled from STA. This may cause the drop in throughputs.\n
 * @note       For the power save mode 3, select RSI_SLEEP_MODE_2 in psp_mode and RSI_HAND_SHAKE_TYPE as MSG_BASED in rsi_wlan_config.h file.\n
 * @note       For the power save mode 9, select RSI_SLEEP_MODE_8 in psp_mode and RSI_HAND_SHAKE_TYPE as MSG_BASED in rsi_wlan_config.h file.\n
 * @note       For the deep sleep without ram retention case, select RSI_SLEEP_MODE_10 in psp_mode and RSI_HAND_SHAKE_TYPE as MSG_BASED for msg_based or GPIO_BASED for gpio_based.\n
 * @note       For LP sleep, select RSI_SLEEP_MODE_2 in psp_mode, select RSI_SELECT_LP_OR_ULP_MODE as RSI_LP_MODE and RSI_HAND_SHAKE_TYPE as MSG_BASED/GPIO_BASED in rsi_wlan_config.h file.\n 
 * @note	     Powersave handshake option: \n
 *					   - When sleep clock source is configured to '32KHz bypass clock on UULP_VBAT_GPIO_3', \n
 *						 use UULP_VBAT_GPIO_0 for SLEEP_IND_FROM_DEV \n
 *						 set RS9116_SILICON_CHIP_VER in 'RS9116.NB0.WC.GENR.OSI.X.X.X\host\sapis\include\rsi_user.h' to 'CHIP_VER_1P4_AND_ABOVE' \n
 *					   - If not using external clock on UULP_VBAT_GPIO_3' as sleep clock source, \n
 *						 use UULP_VBAT_GPIO_3 for SLEEP_IND_FROM_DEV \n
 *						 set RS9116_SILICON_CHIP_VER in 'RS9116.NB0.WC.GENR.OSI.X.X.X\host\sapis\include\rsi_user.h' to 'CHIP_VER_1P3'. \n
 *             EXT_FEAT_LOW_POWER_MODE is not supported for 1.3 version chipset(CHIP_VER_1P3).
 *            ## Power save modes description ##
 *	        + **Power Save Mode 0** \n
 *            - In this mode, module is active and power save is disabled. It can be configured any time, while the module is configured in Power Save mode 2 or 8.\n
 *          + **Power save Mode 1** \n
 *            - Once the module is configured to power save mode 1, it wakes up periodically based upon the DTIM interval configured in connected AP. \n
 *            In power mode 1, only the RF of the module is in power save while SOC continues to work normally. \n
 *	       	  This command has to be given only when module is in connected state (with the AP). After having configured the module to power save mode, the Host can issue subsequent commands. \n
 *	       	  In power save mode 1 the module can receive data from host at any point of time but it can send/receive the data to/from remote terminal only when it is awake at DTIM intervals. \n
 *          + **Power Save Mode 2** \n
 *            - Once the module is configured to power save mode 2, it can be woken up either by the Host or periodically during its sleep-wakeup cycle. \n
 *            Power Save mode 2 is GPIO based. \n
 *            In case of GPIO based mode, whenever host wants to send data to module, it gives wakeup request by asserting UULP GPIO #2. \n
 *		        After wakeup, if the module is ready for data transfer, it sends wakeup indication to host by asserting UULP GPIO #3 or UULP GPIO #0. \n
 *		        Host is required to wait until module gives wakeup indication before sending any data to the module. \n
 *            After the completion of data transfer, host can give sleep permission to module by de-asserting UULP GPIO #2. After recognizing sleep permission from host, \n
 *	       	  module gives confirmation to host by de-asserting UULP GPIO #3 or UULP GPIO #0 and again goes back to its sleep-wakeup cycle. \n
 *            Module can send received packets or responses to host at any instant of time. No handshake is required on Rx path. \n
 *          + **Power Save mode 3**  \n
 *            - Power Mode 3 is message based power save. In Power Mode 3, both radio and SOC of RS9116-WiSeConnect are in power save mode. \n
 *		        This mode is significant when module is in associated state with AP. Module wakes up periodically upon every DTIM and gives wakeup message ("WKP") to host. \n
 *		        Module can not be woken up asynchronously. Every time module intends to go to sleep it sends a sleep request message ("SLP") to the host and expects host to send the acknowledgement message ("ACK"). \n
 *		        Host either send acknowledgement ("ACK") or any other pending message. But once ACK is sent, Host should not send any other message unless next wakeup message from module is received. \n
 *            Module shall not go into complete power-save state, if ACK is not received from host for given sleep message. Module can send received packets or responses to host at any instant of time. \n
 *	       	  No handshake is required on Rx path. \n
 *          + **Power Save mode 8**  \n
 *		        - In Power save mode 8, both RF and SOC of the module are in complete power save mode. This mode is significant only when module is in un-connected state. \n
 *		        Power Save mode 8 can be GPIO based. \n
 *            In case of GPIO based, host can wakeup the module from power save by asserting UULP GPIO #2. \n
 *		        After wakeup, if the module is ready for data transfer, it sends wakeup indication to host by asserting UULP GPIO #3 or UULP GPIO #0. \n
 *            Host is required to wait until module gives wakeup indication before sending any data to the module. \n
 *            After the completion of data transfer, host can give sleep permission to module by de-asserting UULP GPIO #2. After recognizing sleep permission from host,
 *	       	  module gives confirmation to host by de-asserting UULP GPIO #3 or UULP GPIO #0 and again goes back to its sleep-wakeup cycle. \n
 *            Module can send received packets or responses to host at any instant of time. No handshake is required on Rx path. \n
 *          + **Power save mode 9**  \n
 *		        - In Power Mode 9 both Radio and SOC of RS9116-WiSeConnect are in complete power save mode. This mode is significant when module is not connected with any AP. \n
 *		        Once power mode 9 command is given, the module sends ("SLP") request to host and wait for the ("ACK") from host and goes to sleep when ACK is given by host. Timer starts when power save command is issued and it can be configured by host using \ref rsi_wlan_set_sleep_timer API. \n
 *	          If host does not set any sleep time, then the timer is configured for 3sec by default. Upon wakeup module sends a wakeup message to the host and expects host to give ACK before \n
 *		        it goes into next sleep cycle. Host either send ACK or any other messages but once ACK is sent no other packet should be sent before receiving next wakeup message. \n
 *            When ulp_mode_enable is set to '2', after waking up from sleep, the module sends WKP FRM SLEEP message to host when RAM retention is not enabled. \n
 *		        After receiving WKP FRM SLEEP message, host needs to start giving commands from beginning as module's state is not retained. \n
 * @return    0               -  Success \n
 *            Non-Zero	Value - Failure (**Possible Error Codes** - 0xfffffffe,0xfffffffd,0xfffffffa,0x0021,0x0025,0x002C,0xFFF8,0x0015,0x0026,0x0052) \n
 * @note      If the user wants to enable power save in CoEx mode (WLAN + BT LE) mode - It is mandatory to enable WLAN power save along with BT power save. \n
 * @note      In CoEx mode, the device will enter into power save only if both protocol (WLAN, BLE) power save modes are enabled. \n
 * @note      Refer to \ref error-codes for the description of above error codes.
 *
 */

int32_t rsi_wlan_power_save_profile(uint8_t psp_mode, uint8_t psp_type)
{
  int32_t status = RSI_SUCCESS;
  status         = rsi_wlan_power_save_with_listen_interval(psp_mode, psp_type, 0);
  SL_PRINTF(SL_WLAN_POWER_SAVE_PROFILE_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}
/*==============================================*/
/**
 * @brief 	    Disable or enable the power save feature. This is a blocking API.
 * @param[in]   psp_mode        -  Power save mode
 * @param[in]   psp_tye         -  Power save type
 * @return      0 		          - Success \n
 * @return      Non-Zero Value  - Failure \n 
 * @note        **Precondition** - \ref rsi_wireless_init() API needs to be called before this API.
 * @note        If RSI_SLEEP_MODE_10 is chosen as the sleep mode, commands should be recalled from driver_init() as this is without RAM retention.            
 *
 *
 */
int32_t rsi_wlan_power_save_disable_and_enable(uint8_t psp_mode, uint8_t psp_type)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_POWER_SAVE_DISABLE_ENABLE_ENTRY, WLAN, LOG_INFO);
  status = rsi_wlan_power_save_profile(RSI_ACTIVE, psp_type);
  if (status != RSI_SUCCESS) {
    // Return status if error in sending command occurs
    SL_PRINTF(SL_WLAN_POWER_SAVE_DISABLE_ENABLE_ERROR_IN_SENDING_COMMAND_1, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }
  status = rsi_wlan_power_save_profile(psp_mode, psp_type);
  if (status != RSI_SUCCESS) {
    // Return status if error in sending command occurs
    SL_PRINTF(SL_WLAN_POWER_SAVE_DISABLE_ENABLE_ERROR_IN_SENDING_COMMAND_2, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }
  SL_PRINTF(SL_WLAN_POWER_SAVE_DISABLE_ENABLE_EXIT, WLAN, LOG_INFO, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief      Start the transmit test. This is a blocking API. \n 
 *             This API is relevant in PER mode.
 * @param[in]  power  - Set TX power in dbm. The valid values are from 2dBm to 18dBm. \n
 * @note                User can configure the maximum power level allowed for the given frequncey in the configured region by providing 127 as power level \n
 * @note                User should configure a minimum delay (approx. 10 milliseconds) before and after \ref rsi_transmit_test_start API to observe a stable output at requested dBm level. \n
 * @param[in]  rate   - Set transmit data rate
 * @param[in]  length - Configure length of the TX packet. \n
 *                      The valid values are in the range of 24 to 1500 bytes in the burst mode and range of 24 to 260 bytes in the continuous mode. 
 * @param[in]  mode   - Below mentioned are the available modes
 *            Value     |       Parameter                                   |  Description	
 * 		        :---------|:--------------------------------------------------|:-------------------------------------------------
 *            0         |  Burst Mode                                       | DUT transmits a burst of packets with the given power, rate, length in the channel configured. 
 *            ^                                                               The burst size will be determined by the <number of packets> and if its zero, then DUT keeps transmitting till a rsi_transmit_test_stop API is called.
 *            1         |  Continuous Mode                                  | The DUT transmits a unmodulated waveform continuously
 *            2         |  Continuous wave Mode (non modulation) in DC mode | The DUT transmits a spectrum only at the center frequency of the channel.
 *            ^                                                               A basic signal with no modulation is that of a sine wave and is usually referred to as a continuous wave (CW) signal.
 *            ^                                                               A basic signal source produces sine waves. Ideally, the sine wave is perfect. In the frequency domain, it is viewed as a single line at some specified frequency.
 *            3         | Continuous wave Mode (non modulation) in single tone mode (center frequency -2.5MHz) | The DUT transmits a spectrum that is generated at -2.5MHz from the center frequency of the channel selected.
 *            ^                                                                                                  Some amount of carrier leakage will be seen at Center Frequency. Eg: for 2412MHz, the output will be seen at 2409.5MHz
 *            4         | Continuous wave Mode (non modulation) in single tone mode (center frequency +5MHz)   | The DUT transmits a spectrum that is generated at 5MHz from the center frequency of the channel selected.
 *                                                                                                               Some amount of carrier leakage will be seen at Center Frequency. Eg: for 2412MHz, the output will be seen at 2417MHz.                                                                                     
 * @param[in]  channel - Set the channel number in 2.4 GHz / 5GHz.
 *      ### Data Rates ###       
 *			Data rate(Mbps)	|	Value of rate 
 *			:--------------:|:-------------------:
 *			1		            |	0 
 *			2		            |	2 \n
 *			5.5		          |	4 \n
 *			11		          |	6 \n
 *			6		            |	139 \n
 *			9		            |	143 \n
 *			12		          |	138 \n
 *			18		          |	142 \n
 *			24		          |	137 \n
 *			36		          |	141 \n
 *			48		          |	136 \n
 *			54		          |	140 \n
 *			MCS0		        |	256 \n
 *			MCS1		        |	257 \n
 *			MCS2		        |	258 \n
 *			MCS3		        |	259 \n
 *			MCS4		        |	260 \n
 *			MCS5		        |	261 \n
 *			MCS6		        |	262 \n
 *			MCS7		        |	263 \n			
 *			###The following table maps the channel number to the actual radio frequency in the 2.4 GHz spectrum. ### 
 *			Channel numbers (2.4GHz)|	Center frequencies for 20MHz channel width 
 *			:----------------------:|:-----------------------------------------------:
 *			1			|	2412 
 *			2			|	2417  
 *			3			|	2422 
 *			4			|	2427
 *			5			|	2432 
 *			6			|	2437 
 *			7			|	2442 
 *			8			|	2447 
 *			9			|	2452
 *			10			|	2457  
 *			11			|	2462 
 *			12			|	2467 
 *			13			|	2472 
 *			14			|	2484 
 *    ###	The following table maps the channel number to the actual radio frequency in the 5 GHz spectrum for 20MHz channel bandwidth. The channel numbers in 5 GHz range is from 36 to 165. ###
 * 		Channel Numbers(5GHz) |	Center frequencies for 20MHz channel width 
 * 		:--------------------:|:------------------------------------------:
 *		36		      |5180 
 *		40		      |5200 
 *		44		      |5220 
 *		48		      |5240 
 *		52		      |5260 
 *		56	        |5280 
 *		60		      |5300 
 *		64		      |5320 
 *		149		      |5745 
 *		153		      |5765 
 *		157		      |5785 
 *		161		      |5805 
 *		165		      |5825 
 * @note       **Precondition** - \ref rsi_wlan_radio_init() API needs to be called before this API.
 * @note       Rate flags can be added in rsi_wlan_common_config.h file. \n
 *             In rate flags, BIT(6) - Immediate Transfer, set this bit to transfer packets immediately ignoring energy/traffic in channel \n
 * @note       Before starting Continuous Wave mode, user must start Continuous mode with power and channel values that are intended to be used in Continuous Wave mode i.e. \n
 *             - Start Continuous mode with intended power value and channel values - Pass any valid values for rate and length. \n
 *             - Stop Continuous mode \n 
 *             - Start Continuous Wave mode \n
 * @note       If user wants to switch continuous wave mode, first need to stop the transmit test and again need to give continous wave mode which user wants to switch. 
 * @note	     In 2.4GHz, to start transmit test in 12,13,14 channels, configure set region parameters in rsi_wlan_config.h \n
 * @return     0 		- Success \n
 *             Non-Zero Value  - Failure (**Possible Error Codes** - 0xfffffffa,0x000A, 0x0021, 0x0025, 0x002C) \n
 * @note       Refer to \ref error-codes for the description of above error codes.
 *
 */

int32_t rsi_transmit_test_start(uint16_t power, uint32_t rate, uint16_t length, uint16_t mode, uint16_t channel)
{
  rsi_pkt_t *pkt;
  rsi_req_tx_test_info_t *tx_test_info;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_TX_TEXT_START_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Check whether transmit test mode is selected
  if (wlan_cb->opermode != RSI_WLAN_TRANSMIT_TEST_MODE) {
    SL_PRINTF(SL_WLAN_TX_TEXT_START_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  // WLAN radio init
  status = rsi_wlan_radio_init();

  if (status != RSI_SUCCESS) {
    // Return status if error in sending command occurs
    SL_PRINTF(SL_WLAN_TX_TEXT_START_ERROR_IN_SENDING_COMMAND_1, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_TX_TEXT_START_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Fill TX test info parameters
    tx_test_info = (rsi_req_tx_test_info_t *)pkt->data;

    // Memset the PKT
    memset(&pkt->data, 0, sizeof(rsi_req_tx_test_info_t));

    // enable transmit test
    rsi_uint16_to_2bytes(tx_test_info->enable, 1);

    // Configure transmit power of TX test
    rsi_uint16_to_2bytes(tx_test_info->power, power);

    // Configure transmit rate of TX test
    rsi_uint32_to_4bytes(tx_test_info->rate, rate);

    // Configure packet length of TX test
    rsi_uint16_to_2bytes(tx_test_info->length, length);

    // Configure transmit mode of TX test
    rsi_uint16_to_2bytes(tx_test_info->mode, mode);

    // Configure channel of TX test
    rsi_uint16_to_2bytes(tx_test_info->channel, channel);
    rsi_uint16_to_2bytes(tx_test_info->aggr_enable, RSI_TX_TEST_AGGR_ENABLE);
#ifdef CHIP_9117
    tx_test_info->enable_11ax = RSI_11AX_ENABLE;
    if (tx_test_info->enable_11ax) {
      tx_test_info->coding_type   = RSI_CODING_TYPE;
      tx_test_info->nominal_pe    = RSI_NOMINAL_PE;
      tx_test_info->he_ppdu_type  = RSI_HE_PPDU_TYPE;
      tx_test_info->beam_change   = RSI_BEAM_CHANGE;
      tx_test_info->BW            = RSI_BW;
      tx_test_info->UL_DL         = RSI_UL_DL;
      tx_test_info->STBC          = RSI_STBC;
      tx_test_info->Tx_BF         = RSI_TX_BF;
      tx_test_info->GI_LTF        = RSI_GI_LTF;
      tx_test_info->DCM           = RSI_DCM;
      tx_test_info->NSTS_MIDAMBLE = RSI_NSTS_MIDAMBLE;
      tx_test_info->spatial_reuse = RSI_SPATIAL_REUSE;
      tx_test_info->BSS_color     = RSI_BSS_COLOR;
      //tx_test_info->he_conf_params.HE_SIGA2_RESERVED = RSI_HE_SIGA2_RESERVED;
      rsi_uint16_to_2bytes(tx_test_info->HE_SIGA2_RESERVED, RSI_HE_SIGA2_RESERVED);
      tx_test_info->RU_ALLOCATION = RSI_RU_ALLOCATION;
      tx_test_info->N_HELTF_TOT   = RSI_N_HELTF_TOT;
      tx_test_info->SIGB_DCM      = RSI_SIGB_DCM;
      tx_test_info->SIGB_MCS      = RSI_SIGB_MCS;
      //tx_test_info->he_conf_params.USER_STA_ID = RSI_USER_STA_ID;
      rsi_uint16_to_2bytes(tx_test_info->USER_STA_ID, RSI_USER_STA_ID);
      tx_test_info->USER_IDX               = RSI_USER_IDX;
      tx_test_info->SIGB_COMPRESSION_FIELD = RSI_SIGB_COMPRESSION_FIELD;
    }
#endif
#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_TX_TEST_MODE, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_TRANSMIT_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_TX_TEXT_START_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_TX_TEXT_START_ERROR_IN_SENDING_COMMAND_2, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief      Stops the transmit test. This is a blocking API. \n
 *             This API is relevant in PER mode. 
 * @param[in]  void    
 * @return	   0		           - Success
 * @return	   Non Zero Value - Failure (**Possible Error Codes** - 0xfffffffa,0x0021, 0x0025, 0x002C) \n    
 * @note       **Precondition** - \ref rsi_wlan_radio_init() API needs to be called before this API.
 * @note       User should configure a minimum delay (approx. 10 milliseconds) before and after \ref rsi_transmit_test_start API to observe a stable output at requested dBm level. \n
 * @note       Refer to Error Codes section for the description of the above error codes  \ref error-codes.
 *
 */

int32_t rsi_transmit_test_stop(void)
{
  rsi_pkt_t *pkt;
  rsi_req_tx_test_info_t *tx_test_info;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_TX_TEXT_STOP_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_TX_TEXT_STOP_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_TX_TEXT_STOP_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Fill TX test info parameters
    tx_test_info = (rsi_req_tx_test_info_t *)pkt->data;

    // Memset the PKT
    memset(&pkt->data, 0, sizeof(rsi_req_tx_test_info_t));

    // Configure to stop TX test
    rsi_uint16_to_2bytes(tx_test_info->enable, 0);

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_TX_TEST_MODE, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_TRANSMIT_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_TX_TEXT_STOP_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_TX_TEXT_STOP_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief     Get the Transmit (TX) and Receive (RX) packets statistics. When this API is called by the host with valid channel number,
 *            the module gives the statistics to the host for every 1 second asynchronously. If wlan_receive_stats_response_handler()
 *            is registered through rsi_wlan_register_callbacks(), it's a non blocking API, otherwise, a blocking API.
 * @param[in] channel         - Valid channel number: 2.4GHz or 5GHz          
 * @return    0               - Success
 * @return    Non-Zero Value  - Failure (**Possible Error Codes** - 0xfffffffa,0x0021, 0x0025, 0x002c, 0x000A) \n
 * @note      **Precondition** - \ref rsi_wlan_radio_init() API needs to be called before this API.
 * @note      Refer to \ref error-codes for the description of above error codes.
 * 
 */

int32_t rsi_wlan_receive_stats_start(uint16_t channel)
{
  rsi_pkt_t *pkt;
  rsi_req_rx_stats_t *rx_stats;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_RX_STATS_START_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_RX_STATS_START_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_RX_STATS_START_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Fill RX stats  parameters
    rx_stats = (rsi_req_rx_stats_t *)pkt->data;

    // Memset the PKT
    memset(&pkt->data, 0, sizeof(rsi_req_rx_stats_t));

    // Configure to start RX stats
    rsi_uint16_to_2bytes(rx_stats->start, 0);

    // Copy the channel number
    rsi_uint16_to_2bytes(rx_stats->channel, channel);

    if (rsi_wlan_cb_non_rom->callback_list.wlan_receive_stats_response_handler == NULL) {
#ifndef RSI_WLAN_SEM_BITMAP
      rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    }
    // Send command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_RX_STATS, pkt);

    if (rsi_wlan_cb_non_rom->callback_list.wlan_receive_stats_response_handler == NULL) {
      // Wait on WLAN semaphore
      rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_RX_STATS_RESPONSE_WAIT_TIME);
      // Get WLAN/network command response status
      status = rsi_wlan_get_status();
    }
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_RX_STATS_START_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_RX_STATS_START_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief     Stop the Transmit (TX) & Receive(RX) packets statistics. \n
 *            If wlan_receive_stats_response_handler() is registered through rsi_wlan_register_callbacks(), it's non blocking API, otherwise, a blocking API.
 * @param[in] void
 * @return    0               - Success
 * @return    Non-Zero Value  - Failure (**Possible Error Codes** - 0xfffffffa,0x0021, 0x0025, 0x002c) \n
 * @note      **Precondition** - \ref rsi_wlan_radio_init() API needs to be called before this API.
 * @note      Refer to \ref error-codes for the description of above error codes.
 */

int32_t rsi_wlan_receive_stats_stop(void)
{
  rsi_pkt_t *pkt;
  rsi_req_rx_stats_t *rx_stats;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_RX_STATS_STOP_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_RX_STATS_STOP_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_RX_STATS_STOP_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Fill RX stats  parameters
    rx_stats = (rsi_req_rx_stats_t *)pkt->data;

    // Memset the PKT
    memset(&pkt->data, 0, sizeof(rsi_req_rx_stats_t));

    // Configure to stop RX stats
    rsi_uint16_to_2bytes(rx_stats->start, 1);

    if (rsi_wlan_cb_non_rom->callback_list.wlan_receive_stats_response_handler == NULL) {
#ifndef RSI_WLAN_SEM_BITMAP
      rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    }
    // Send command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_RX_STATS, pkt);

    if (rsi_wlan_cb_non_rom->callback_list.wlan_receive_stats_response_handler == NULL) {
      // Wait on WLAN semaphore
      rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_RX_STATS_RESPONSE_WAIT_TIME);

      // Get WLAN/network command response status
      status = rsi_wlan_get_status();
    }

    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_RX_STATS_STOP_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_RX_STATS_STOP_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}
/*! \cond RS9116 */
/*==============================================*/
/**
 * @brief       Start discovery in Wi-Fi direct mode. \n
 *              If wlan_wfd_discovery_notify_handler() is registered through rsi_wlan_register_callbacks(), its non blocking API, otherwise blocking API.						
 * @param[in]	  go_intent                                  - Determine whether the device is intended to form a GO (group owner) or work as a Wi-Fi Direct Peer node. \n
 *                                                           Value used in the GO negotiation process, when the module negotiates with another Wi-Fi Direct Node on who would become the Group Owner. \n
 *                                                           Valid range of values for this parameter is: 0 to 16. Higher the number, higher is the willingness of the module to become a GO. \n
 *                                                           After the module becomes a GO in Wi-Fi Direct mode, it appears as an Access Point to the client devices. \n
 *                                                           If the number is between 0 and 15, a GO negotiation takes place. If the value is 16, the module forms an Autonomous GO without negotiating with any other device.
 * @param[in]	  device_name                                - Device name for the module. The maximum length of this field is 32 characters and the remaining bytes are filled with 0x00. \n
 *                                                           Another Wi-Fi Direct device would see this name when it scans for Wi-Fi Direct nodes.  \n 
 * @param[in]	  channel                                    - Another Wi-Fi Direct device would see this name when it scans for Wi-Fi Direct nodes.  \n
 * @param[in]	  ssid_post_fix                              - Used to add a postfix to the SSID in Wi-Fi Direct GO mode and Autonomous GO mode. \n
 * @param[in]	  psk                                        - Passphrase of a maximum length of 63 characters (a null character should be supplied to make it 64 bytes in the structure). \n
 *                                                           PSK used if the module becomes a GO owner. \n
 * @param[in]	  wlan_wfd_discovery_notify_handler          - Asynchronous message sent from module to the host when module finds any Wi-Fi Direct node. \n
 *                                                           Parameters involved are status, buffer & length.
 *                                                           Parameter             |	Description 
 * 		                                                       :--------------------:|:------------------------------------------:
 *		                                                       status                |  Response status
 *		                                                       buffer       		     |  Response buffer 
 *		                                                       length       		     |  Response buffer length
 * @param[in]	  wlan_wfd_connection_request_notify_handler - Asynchronous message sent from module to the host when module receives a connection request from any remote Wi-Fi Direct node. \n  
 *                                                           Parameters involved are status, buffer & length.
 *                                                           Parameter             |	Description 
 * 		                                                       :--------------------:|:------------------------------------------:
 *		                                                       status                |  Response status
 *		                                                       buffer       		     |  Response buffer 
 *		                                                       length       		     |  Response buffer length
 * @return      0  -  Success \n
 * @return      Non-Zero	Value - Failure (**Possible Error Codes** - 0xfffffffd,0xfffffffa,0x001D, 0x0021, 0x002C, 0x0015) \n
 * @pre         **Precondition** - \ref rsi_wireless_init() API needs to be called before this API.
 * @note        Refer to \ref error-codes for the description of above error codes.
 *
 */

int32_t rsi_wlan_wfd_start_discovery(
  uint16_t go_intent,
  int8_t *device_name,
  uint16_t channel,
  int8_t *ssid_post_fix,
  uint8_t *psk,
  void (*wlan_wfd_discovery_notify_handler)(uint16_t status, uint8_t *buffer, const uint32_t length),
  void (*wlan_wfd_connection_request_notify_handler)(uint16_t status, uint8_t *buffer, const uint32_t length))
{
  rsi_pkt_t *pkt;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_WFD_START_DISCOVERY_ENTRY, WLAN, LOG_INFO);

  rsi_req_configure_p2p_t *config_p2p;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Check whether Wi-Fi-Direct mode is selected
  if (wlan_cb->opermode != RSI_WLAN_WIFI_DIRECT_MODE) {
    SL_PRINTF(SL_WLAN_WFD_START_DISCOVERY_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  // WLAN radio init
  status = rsi_wlan_radio_init();

  if (status != RSI_SUCCESS) {
    // Return status if error in sending command occurs
    SL_PRINTF(SL_WLAN_WFD_START_DISCOVERY_ERROR_IN_SENDING_COMMAND_1, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_WFD_START_DISCOVERY_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    config_p2p = (rsi_req_configure_p2p_t *)pkt->data;

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_req_configure_p2p_t));

    // Configure go intent
    rsi_uint16_to_2bytes(config_p2p->go_intent, go_intent);

    if (device_name != NULL) {
      // Copy device name
      rsi_strcpy(config_p2p->device_name, device_name);
    }

    // Configure operating channel
    rsi_uint16_to_2bytes(config_p2p->operating_channel, channel);

    if (ssid_post_fix != NULL) {
      // Copy SSID post fix
      rsi_strcpy(config_p2p->ssid_post_fix, ssid_post_fix);
    }

    if (psk != NULL) {
      // Copy PSK
      rsi_strcpy(config_p2p->psk, psk);
    }

    if (wlan_wfd_discovery_notify_handler != NULL) {
      rsi_wlan_cb_non_rom->callback_list.wlan_wfd_discovery_notify_handler = wlan_wfd_discovery_notify_handler;
    }

    if (wlan_wfd_connection_request_notify_handler != NULL) {
      rsi_wlan_cb_non_rom->callback_list.wlan_wfd_connection_request_notify_handler =
        wlan_wfd_connection_request_notify_handler;
    }
    if ((wlan_wfd_discovery_notify_handler == NULL) && (wlan_wfd_connection_request_notify_handler == NULL)) {
#ifndef RSI_WLAN_SEM_BITMAP
      rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    }
    // Send wi-fi direct discovery command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_CONFIGURE_P2P, pkt);

    if ((wlan_wfd_discovery_notify_handler == NULL) && (wlan_wfd_connection_request_notify_handler == NULL)) {
      // Wait on WLAN semaphore
      rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_P2P_RESPONSE_WAIT_TIME);
      // Get WLAN/network command response status
      status = rsi_wlan_get_status();
      //Changing the wlan cmd state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
    }

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_WFD_START_DISCOVERY_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_WFD_START_DISCOVERY_ERROR_IN_SENDING_COMMAND_2, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}
/*==============================================*/
/**
 * @brief       A join_response_handler() API that connects to the specified Wi-Fi-Direct device.This is a non-blocking API.
 *              if join_response_handler() is registered through rsi_wlan_register_callbacks(), it's non blocking, otherwise, a blocking call.
 * @pre         \ref rsi_wlan_wfd_start_discovery() API needs to be called before this API.
 * @param[in]   device_name           -  Device name of the Wi-Fi Direct node to connect. \n
 * @param[in]   join_response_handler -  Called when the response for join has come from the module \n
 * @param[in]   device_name           - Device name of the Wi-Fi Direct node to connect. \n
 * @param[in]   join_response_handler - Called when the response for join has come from the module \n
 *                                       Parameters involved are status, buffer, & length.
 * @param[out]  status		      -	 Response status \n
 *                                        If status is zero, join response is stated as success 
 * @param[out]	buffer		      -	 Response buffer. On successful execution of the command. \n
 *       			      	                   GO_Status (1 byte, hex): 0x47 (ASCII "G") � If the module becomes a Group Owner (GO) after the GO negotiation stage, \n
 *       			       	                   or becomes an Access Point. 0x43 (ASCII "C") � If the module does not become a GO after the GO negotiation stage, or becomes a client (or station). \n
 * @param[out] length		      -  Response buffer length
 * @param[out]  <nodetype> \n
 *              'G' - GO \n
 *              'C' - Client
 * @note        The module gets a default IP of 192.168.100.76 if it becomes a Group Owner or Access Point in case of IPv4. and gets a default IP of 2001:db8:0:1:0:0:0:120 in case of IPv6.
 * @return      0               -  Success \n
 *              Non-Zero	Value - Failure \n
 *	            If return value is less than 0 \n
 *	            -4              - Buffer not available to serve the command \n
 *              If return value is greater than 0 \n
 * 	            0x0014, 0x0009, 0x0003, 0x0021, 0x0012c 0x0015
 *
 */

int32_t rsi_wlan_wfd_connect(int8_t *device_name,
                             void (*join_response_handler)(uint16_t status,
                                                           const uint8_t *buffer,
                                                           const uint16_t length))
{
  rsi_pkt_t *pkt;
  rsi_req_join_t *join;
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_WFD_CONNECT_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_WFD_CONNECT_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_WFD_CONNECT_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_req_join_t));

    join = (rsi_req_join_t *)pkt->data;

    if (device_name != NULL) {
      // Copy SSID and SSID len
      join->ssid_len = rsi_strlen(device_name);

      // Copy Join SSID
      rsi_strcpy(join->ssid, device_name);
    }

    if (join_response_handler != NULL) {
      // Register scan response handler
      rsi_wlan_cb_non_rom->callback_list.wlan_join_response_handler = join_response_handler;
    }

    if (join_response_handler == NULL) {
#ifndef RSI_WLAN_SEM_BITMAP
      rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    }
    // Send join command to start wps
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_JOIN, pkt);

    if (join_response_handler == NULL) {
      // Wait on WLAN semaphore
      rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_JOIN_RESPONSE_WAIT_TIME);
      // Get WLAN/network command response status
      status = rsi_wlan_get_status();
      //Changing the wlan cmd state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
    }

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_WFD_CONNECT_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_WFD_CONNECT_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}
/*! \endcond */
/*==============================================*/
/**
 * @brief      Send the raw data in TCP/IP bypass mode. This is a blocking API.
 * @param[in]  buffer - Pointer to the buffer to send
 * @param[in]  length - Length of the buffer to send
 * @return     0               -  Success \n
 *             Non-Zero  Value - Failure \n
 *             If return value is less than 0 \n
 *             -2              - Invalid Parameters \n
 *		         -4              - Buffer not available to serve the command \n
 *             If return value is greater than 0 \n
 *             0x0021,0x002C,0x0025
 *
 *
 */

int32_t rsi_wlan_send_data(uint8_t *buffer, uint32_t length)
{
  int32_t status = RSI_SUCCESS;
  uint8_t *host_desc;
  rsi_pkt_t *pkt;
  SL_PRINTF(SL_WLAN_SEND_DATA_ENTRY, WLAN, LOG_INFO);
  // Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_CONNECTED) {
    // Command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  // If buffer is not valid
  if ((buffer == NULL) || (length == 0)) {
    // Return packet allocation failure error
    SL_PRINTF(SL_WLAN_SEND_DATA_INVALID_PARAM, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_INVALID_PARAM;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Allocate packet to send data
    pkt = rsi_pkt_alloc(&rsi_driver_cb->wlan_cb->wlan_tx_pool);

    if (pkt == NULL) {
      SL_PRINTF(SL_WLAN_SEND_DATA_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      //Changing the wlan cmd state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Get host descriptor pointer
    host_desc = pkt->desc;

    // Memset host descriptor
    memset(host_desc, 0, RSI_HOST_DESC_LENGTH);

    // Fill host descriptor
    rsi_uint16_to_2bytes(host_desc, (length & 0xFFF));

    // Fill packet type
    host_desc[1] |= (RSI_WLAN_DATA_Q << 4);
    host_desc[2] |= 0x01;

    // Copy data to be sent
    memcpy(pkt->data, buffer, length);

    // Enqueue packet to WLAN TX queue
    rsi_enqueue_pkt(&rsi_driver_cb->wlan_tx_q, pkt);

#ifndef RSI_SEND_SEM_BITMAP
    rsi_driver_cb_non_rom->send_wait_bitmap |= BIT(0);
#endif
    // Set TX packet pending event
    rsi_set_event(RSI_TX_EVENT);

    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->send_data_sem, RSI_SEND_DATA_RESPONSE_WAIT_TIME);

    status = rsi_wlan_get_status();
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
  }
  // Return status
  SL_PRINTF(SL_WLAN_SEND_DATA_EXIT, WLAN, LOG_INFO, "status: %4x", status);

  return status;
}
/// @cond DOCS_9117
/**
 * @fn          int32_t rsi_wlan_twt_config(uint8_t twt_enable, uint8_t twt_flow_id, twt_user_params_t *twt_req_params)
 * @brief       Configures TWT parameters. Enables or disables a TWT session. This is blocking API.
 * @pre         needs to be called after @ref rsi_wireless_init() OPERMODE command.
 * @param[in]   twt_enable - TWT session setup or teardown \n 1 - To setup TWT session with given parameters \n
 *              0 - To teardown TWT session with given flow ID.
 * @param[in]   twt_flow_id - TWT session flow ID. \n 0 - 7 are valid flow IDs. \n 0xFF value is used to teardown all active sessions. 
 * @param[in]   twt_req_params Configurable TWT parameters. Refer \ref twt_user_params_s.
 * @return      0      - Success \n
 *              Non-Zero Value - Failure \n
 * @note       Refer Error Codes section for above \ref error-codes .
 */
int32_t rsi_wlan_twt_config(uint8_t twt_enable, uint8_t twt_flow_id, twt_user_params_t *twt_req_params)
{
  int32_t status         = RSI_SUCCESS;
  rsi_pkt_t *pkt         = NULL;
  rsi_twt_req_t *twt_req = NULL;
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;
  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  if (twt_enable == 0) {
    if ((twt_flow_id != 0xFF) && (twt_flow_id > 7)) {
      return RSI_ERROR_INVALID_PARAM;
    }
  } else if (twt_enable == 1) {
    if (twt_flow_id > 7) {
      return RSI_ERROR_INVALID_PARAM;
    }
    if (twt_req_params->wake_int_exp > 31) {
      return RSI_ERROR_INVALID_PARAM;
    }
    if (twt_req_params->wake_int_exp_tol > 31) {
      return RSI_ERROR_INVALID_PARAM;
    }
    if (twt_req_params->wake_duration_unit > 1) {
      return RSI_ERROR_INVALID_PARAM;
    }
    if ((twt_req_params->un_announced_twt > 1) || (twt_req_params->triggered_twt > 1)
        || (twt_req_params->restrict_tx_outside_tsp > 1)) {
      return RSI_ERROR_INVALID_PARAM;
    }
    if (twt_req_params->twt_retry_limit > 15) {
      return RSI_ERROR_INVALID_PARAM;
    }
    if (twt_req_params->twt_retry_interval < 5) {
      return RSI_ERROR_INVALID_PARAM;
    }
    if (twt_req_params->req_type > 2) {
      return RSI_ERROR_INVALID_PARAM;
    }
    if ((twt_req_params->req_type != 0)
        && ((twt_req_params->wake_duration == 0) || (twt_req_params->wake_int_mantissa == 0)
            || (((uint64_t)twt_req_params->wake_duration * (uint64_t)(twt_req_params->wake_duration_unit ? 1024 : 256))
                > ((uint64_t)twt_req_params->wake_int_mantissa * ((uint64_t)1 << twt_req_params->wake_int_exp))))) {
      return RSI_ERROR_INVALID_PARAM;
    }
    if ((twt_req_params->twt_channel != 0) || (twt_req_params->twt_protection != 0)
        || (twt_req_params->implicit_twt != 1)) {
      return RSI_ERROR_INVALID_PARAM;
    }
  } else {
    return RSI_ERROR_INVALID_PARAM;
  }

  rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

      // Return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_twt_req_t));

    twt_req = (rsi_twt_req_t *)pkt->data;

    /* copying input params into structure*/
    twt_req->twt_enable  = twt_enable;
    twt_req->twt_flow_id = twt_flow_id;
    if (twt_enable) {
      twt_req->wake_duration      = twt_req_params->wake_duration;
      twt_req->wake_duration_tol  = twt_req_params->wake_duration_tol;
      twt_req->wake_int_exp       = twt_req_params->wake_int_exp;
      twt_req->wake_int_exp_tol   = twt_req_params->wake_int_exp_tol;
      twt_req->wake_duration_unit = twt_req_params->wake_duration_unit;
      rsi_uint16_to_2bytes(twt_req->wake_int_mantissa, twt_req_params->wake_int_mantissa);
      rsi_uint16_to_2bytes(twt_req->wake_int_mantissa_tol, twt_req_params->wake_int_mantissa_tol);
      twt_req->implicit_twt            = twt_req_params->implicit_twt;
      twt_req->un_announced_twt        = twt_req_params->un_announced_twt;
      twt_req->triggered_twt           = twt_req_params->triggered_twt;
      twt_req->negotiation_type        = 0; //Only Individual TWT is supported for now. Not taking input from user.
      twt_req->twt_channel             = twt_req_params->twt_channel;
      twt_req->twt_protection          = twt_req_params->twt_protection;
      twt_req->restrict_tx_outside_tsp = twt_req_params->restrict_tx_outside_tsp;
      twt_req->twt_retry_limit         = twt_req_params->twt_retry_limit;
      twt_req->twt_retry_interval      = twt_req_params->twt_retry_interval;
      twt_req->req_type                = twt_req_params->req_type;
    }
#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send twt command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_TWT_PARAMS, pkt);
    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_WLAN_TWT_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();

    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
  }
  // Return status if error in sending command occurs
  return status;
}
/// @endcond
/*==============================================*/
/**
 * @brief       Send a ping request to the target IP address. \n
 *              If wlan_ping_response_handler() is registered through rsi_wlan_register_callbacks(), it's non blocking API, otherwise, a blocking API.
 * @param[in]   flags                      - BIT(0): RSI_IPV6 Set this bit to enable IPv6; by default, it is configured to IPv4
 * @param[in]   ip_address                 - Target IP address. \n
 *						                               IPv4 address: 4 Bytes hexa-decimal, \n
 *   					                               IPv6 address: 16 Bytes hexa-decimal
 * @param[in]   size                       - Ping data size to send. Maximum supported is 300 bytes.
 * @param[in]   wlan_ping_response_handler - Called when ping response has been received from the module. \n
 *                                           Parameters involved are status, buffer, & length.
 * @param[out]	status			               - Response status
 * @param[out]  buffer                     - Response buffer
 * @param[out]  length                     - Length of the response buffer
 * @return      0                          -  Success \n
 * @return      Non-Zero	Value            - Failure (**Possible Error Codes** - 0xfffffffe,0xfffffffa,0x0015,0xBB21,0xBB4B,0xBB55) \n
 * @note        **Precondition** - \ref rsi_config_ipaddress() API needs to be called before this API.
 * @note        The module can't receive ping packets whose length is more than 308 bytes.
 * @note        Refer to \ref error-codes for the description of above error codes.
 *
 *
 */

int32_t rsi_wlan_ping_async(uint8_t flags,
                            uint8_t *ip_address,
                            uint16_t size,
                            void (*wlan_ping_response_handler)(uint16_t status,
                                                               const uint8_t *buffer,
                                                               const uint16_t length))
{
  int32_t status = RSI_SUCCESS;
  rsi_pkt_t *pkt;
  rsi_req_ping_t *ping;
  SL_PRINTF(SL_WLAN_PING_ASYNC_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;
  if (wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_PING_ASYNC_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  // If IP address is not valid
  if ((ip_address == NULL) || (size == 0)) {
    // Return packet allocation failure error
    SL_PRINTF(SL_WLAN_PING_ASYNC_INVALID_PARAM, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_INVALID_PARAM;
  }

  status = rsi_check_and_update_cmd_state(NWK_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);

      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_PING_ASYNC_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_req_ping_t));

    ping = (rsi_req_ping_t *)pkt->data;

    if (flags & RSI_IPV6) {
      // Copy IP version
      rsi_uint16_to_2bytes(ping->ip_version, (uint16_t)RSI_IP_VERSION_6);

      // Copy IPv6 address
      memcpy(ping->ping_address.ipv6_address, ip_address, RSI_IPV6_ADDRESS_LENGTH);
    } else {
      // Copy IP version
      rsi_uint16_to_2bytes(ping->ip_version, (uint16_t)RSI_IP_VERSION_4);

      // Copy IPv4 address
      memcpy(ping->ping_address.ipv4_address, ip_address, RSI_IPV4_ADDRESS_LENGTH);
    }

    // Copy ping size
    rsi_uint16_to_2bytes(ping->ping_size, size);

    ping->timeout = RSI_PING_REQ_TIMEOUT;

    // Register ping response handler
    if (wlan_ping_response_handler != NULL) {
      rsi_wlan_cb_non_rom->callback_list.wlan_ping_response_handler = wlan_ping_response_handler;
      rsi_wlan_cb_non_rom->nwk_cmd_rsp_pending |= PING_RESPONSE_PENDING;
    }

    if (wlan_ping_response_handler == NULL) {
#ifndef RSI_WLAN_SEM_BITMAP
      rsi_driver_cb_non_rom->nwk_wait_bitmap |= BIT(0);
#endif
    }
    // Send ping command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_PING_PACKET, pkt);

    if (wlan_ping_response_handler == NULL) {
      // Wait on WLAN semaphore
      rsi_wait_on_nwk_semaphore(&rsi_driver_cb_non_rom->nwk_sem, RSI_PING_RESPONSE_WAIT_TIME);
      // Get WLAN/network command response status
      status = rsi_wlan_get_nwk_status();
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(NWK_CMD, ALLOW);
    }
  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_PING_ASYNC_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_PING_ASYNC_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief      Register auto-configuration response handler.. This is a non-blocking API.
 * @param[in]  rsi_auto_config_rsp_handler - Pointer to rsi_auto_config_rsp_handler
 * @param[out] status			                 - Response status, 0 if success else failure.
 * @param[out] state			                 - BIT(1) RSI_AUTO_CONFIG_FAILED. \n 
 *                                           BIT(2) RSI_AUTO_CONFIG_GOING_ON. \n
 *                                           BIT(3) RSI_AUTO_CONFIG_DONE 
 * @return     Void
 */

void rsi_register_auto_config_rsp_handler(void (*rsi_auto_config_rsp_handler)(uint16_t status, uint8_t state))
{

  // Register callback handler
  SL_PRINTF(SL_WLAN_REGISTER_AUTO_CONFIG_RSP_HANDLER_EXIT, WLAN, LOG_INFO);
  rsi_wlan_cb_non_rom->callback_list.auto_config_rsp_handler = rsi_auto_config_rsp_handler;
}

/*==============================================*/
/**
 * @brief      Add profile for auto configuration. This is a blocking API.
 * @param[in]  type - profile type. Supported profile types are: \n
 *  							    1.RSI_WLAN_PROFILE_AP, \n
 *							      2.RSI_WLAN_PROFILE_CLIENT, \n
 *							      3.RSI_WLAN_PROFILE_EAP, \n
 *							      4.RSI_WLAN_PROFILE_P2P, \n
 *							      5.RSI_WLAN_PROFILE_ALL
 * @param[in]  profile - Pointer to config profile and profile structure \n
 * 			                 ap_profile, \n
 * 		                 	 eap_client_profile_t, \n
 * 			                 client_profile_t, \n
 * 			                 p2p_profile_t, \n
 * 			                 rsi_config_profile_t
 * @return      0  -  Success \n
 * @return      Non-Zero Value - Failure (**Possible Error Codes** - 0xfffffffa) \n
 * @note       **This API is not supported in current release.**
 * @note       **Precondition** - \ref rsi_wireless_init() API needs to be called before this API.
 * @note       Refer to \ref error-codes for the description of above error codes.
 *
 */

int32_t rsi_wlan_add_profile(uint32_t type, uint8_t *profile)
{
  int32_t status = RSI_SUCCESS;
  rsi_pkt_t *pkt;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  rsi_config_profile_t *config_profile;
  SL_PRINTF(SL_WLAN_ADD_PROFILE_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_ADD_PROFILE_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_ADD_PROFILE_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_config_profile_t));

    config_profile = (rsi_config_profile_t *)pkt->data;

    // Fill profile type
    rsi_uint32_to_4bytes(config_profile->profile_type, type);

    // Fill WLAN profile
    if (type == RSI_WLAN_PROFILE_AP) {
      memcpy(&config_profile->wlan_profile_struct.ap_profile, (uint8_t *)profile, sizeof(ap_profile_t));
    } else if (type == RSI_WLAN_PROFILE_CLIENT) {
      memcpy(&config_profile->wlan_profile_struct.client_profile, (uint8_t *)profile, sizeof(client_profile_t));
    } else if (type == RSI_WLAN_PROFILE_EAP) {
      memcpy(&config_profile->wlan_profile_struct.eap_client_profile, (uint8_t *)profile, sizeof(eap_client_profile_t));
    } else if (type == RSI_WLAN_PROFILE_P2P) {
      memcpy(&config_profile->wlan_profile_struct.p2p_profile, (uint8_t *)profile, sizeof(p2p_profile_t));
    } else {
      // No code here
    }

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_config_profile_t);

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send set profile command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_PROFILE, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_SET_PROFILE_RESPONSE_WAIT_TIME);
    // Get WLAN/network command response status
    status = rsi_wlan_get_status();

    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_ADD_PROFILE_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_ADD_PROFILE_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief      Get the current WLAN state.. This is a non-blocking API.
 * @param[in]  Void
 * @return     Current WLAN state. \n
               WLAN states are as follows: \n
 * @return     RSI_WLAN_STATE_NONE = 0, \n
 * @return     RSI_WLAN_STATE_OPERMODE_DONE, \n
 * @return     RSI_WLAN_STATE_BAND_DONE, \n
 * @return     RSI_WLAN_STATE_INIT_DONE, \n
 * @return     RSI_WLAN_STATE_SCAN_DONE, \n
 * @return     RSI_WLAN_STATE_CONNECTED, \n
 * @return     RSI_WLAN_STATE_IP_CONFIG_DONE, \n
 * @return     RSI_WLAN_STATE_IPV6_CONFIG_DONE, \n
 * @return     RSI_WLAN_STATE_AUTO_CONFIG_GOING_ON, \n
 * @return     RSI_WLAN_STATE_AUTO_CONFIG_DONE, \n
 * @return     RSI_WLAN_STATE_AUTO_CONFIG_FAILED
 *	       
 */

uint8_t rsi_wlan_get_state(void)
{
  // Return WLAN state
  SL_PRINTF(SL_WLAN_GET_STATE_EXIT, WLAN, LOG_INFO);
  return rsi_driver_cb->wlan_cb->state;
}

/*==============================================*/
/**
 * @brief      Get the stored config profile. This is a blocking API.
 * @param[in]  type           - Config profile type. \n
 *                              Supported profile types are as follows: \n
 *  	    	                    RSI_WLAN_PROFILE_AP, \n
 *		                          RSI_WLAN_PROFILE_CLIENT, \n
 *		                          RSI_WLAN_PROFILE_EAP, \n 
 *		                          RSI_WLAN_PROFILE_P2P, \n 
 *		                          RSI_WLAN_PROFILE_ALL 
 * @param[in]  profile_rsp    - Config profile response in the form of below structure: \n
 *                              ap_profile, \n
 *                              eap_client_profile_t, \n
 *                              client_profile_t, \n  
 *                              p2p_profile_t, \n
 *                              rsi_config_profile_t
 * @param[in]  length         - Length of the config profile response
 * @return     0              -  Success \n
 * @return     Non-Zero Value - Failure (**Possible Error Codes** - 0xfffffffa) \n
 * @note       **Precondition** - \ref rsi_wireless_init() API needs to be called before this API.
 * @note       **This API is not supported in current release.**
 * @note       Refer to \ref error-codes for the description of above error codes.
 */

int32_t rsi_wlan_get_profile(uint32_t type, rsi_config_profile_t *profile_rsp, uint16_t length)
{
  int32_t status                 = RSI_SUCCESS;
  rsi_pkt_t *pkt                 = NULL;
  rsi_profile_req_t *profile_req = NULL;
  SL_PRINTF(SL_WLAN_GET_PROFILE_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_GET_PROFILE_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_GET_PROFILE_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_profile_req_t));

    profile_req = (rsi_profile_req_t *)pkt->data;

    // Fill profile type
    rsi_uint32_to_4bytes(profile_req->profile_type, type);

    // Attach the buffer given by user
    wlan_cb->app_buffer = (uint8_t *)profile_rsp;

    // Length of the buffer provided by user
    wlan_cb->app_buffer_length = length;

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send get profile command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_GET_PROFILE, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_GET_PROFILE_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_GET_PROFILE_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_GET_PROFILE_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief      Fill the config profile based on the profile type.. This is a non-blocking API.
 * @param[in]  Type           - Profile type
 * @param[in]  Profile_buffer - Pointer to profile buffer
 * @return     Profile buffer
 * @note       **Precondition** - \ref rsi_wireless_init() API needs to be called before this API.
 */
/// @private

uint8_t *rsi_fill_config_profile(uint32_t type, uint8_t *profile_buffer)
{
  SL_PRINTF(SL_WLAN_FILL_CONFIG_PROFILE_ENTRY, WLAN, LOG_INFO);
  int i, j = 0;
  uint32_t ip_addr                             = 0;
  uint32_t mask                                = 0;
  uint32_t gw                                  = 0;
  eap_client_profile_t *eap_client_profile_ptr = NULL;
  network_profile_t *nwk_profile_ptr           = NULL;
  feature_frame_params_t *feature_frame_ptr    = NULL;
  client_profile_t *client_profile_ptr         = NULL;
  ap_profile_t *ap_profile_ptr                 = NULL;
  p2p_profile_t *p2p_profile_ptr               = NULL;

  if (type == RSI_WLAN_PROFILE_AP) {
    ip_addr = RSI_CONFIG_AP_IP_ADDRESS;
    mask    = RSI_CONFIG_AP_SN_MASK_ADDRESS;
    gw      = RSI_CONFIG_AP_GATEWAY_ADDRESS;

    ap_profile_ptr = (ap_profile_t *)profile_buffer;

    nwk_profile_ptr = &ap_profile_ptr->network_profile;

    feature_frame_ptr = &ap_profile_ptr->feature_frame_params_s;

    memset(ap_profile_ptr, 0, sizeof(ap_profile_t));

    // Fill AP WLAN feature bitmap
    rsi_uint32_to_4bytes(ap_profile_ptr->wlan_feature_bit_map, RSI_CONFIG_AP_WLAN_FEAT_BIT_MAP);

    // Fill AP TCP_IP feature bitmap
    rsi_uint32_to_4bytes(ap_profile_ptr->tcp_ip_feature_bit_map, RSI_CONFIG_AP_TCP_IP_FEAT_BIT_MAP);

    // Fill AP custom feature bitmap
    rsi_uint32_to_4bytes(ap_profile_ptr->custom_feature_bit_map, RSI_CONFIG_AP_CUSTOM_FEAT_BIT_MAP);

    // Fill data rate
    ap_profile_ptr->data_rate = RSI_CONFIG_AP_DATA_RATE;

    // Fill TX power
    ap_profile_ptr->tx_power = RSI_CONFIG_AP_TX_POWER;

    // Fill AP band
    ap_profile_ptr->band = RSI_CONFIG_AP_BAND;

    // Fill AP channel number
    rsi_uint16_to_2bytes(ap_profile_ptr->channel, RSI_CONFIG_AP_CHANNEL);

    // Fill AP SSID
    rsi_strcpy(ap_profile_ptr->ssid, RSI_CONFIG_AP_SSID);

    // Fill AP security type
    ap_profile_ptr->security_type = RSI_CONFIG_AP_SECURITY_TYPE;

    // Fill AP encryption type
    ap_profile_ptr->encryption_type = RSI_CONFIG_AP_ENCRYPTION_TYPE;

    // Fill AP PSK
    if (RSI_CONFIG_AP_PSK)
      rsi_strcpy(ap_profile_ptr->psk, RSI_CONFIG_AP_PSK);

    // Fill beacon interval
    rsi_uint16_to_2bytes(ap_profile_ptr->beacon_interval, RSI_CONFIG_AP_BEACON_INTERVAL);

    // Fill DTIM period
    rsi_uint16_to_2bytes(ap_profile_ptr->dtim_period, RSI_CONFIG_AP_DTIM);

    // Fill AP keep alive time
    ap_profile_ptr->keep_alive_type = RSI_CONFIG_AP_KEEP_ALIVE_TYPE;

    // Fill AP keep alive counter
    ap_profile_ptr->keep_alive_counter = RSI_CONFIG_AP_KEEP_ALIVE_COUNTER;

    // Fill AP max number of stations
    rsi_uint16_to_2bytes(ap_profile_ptr->max_no_sta, RSI_CONFIG_AP_MAX_STATIONS_COUNT);

    nwk_profile_ptr->tcp_stack_used = RSI_CONFIG_AP_TCP_STACK_USED;

    if (nwk_profile_ptr->tcp_stack_used == 1) {
      nwk_profile_ptr->dhcp_enable = 0;

      // Fill IP address
      memcpy(nwk_profile_ptr->ip_address, (uint8_t *)&ip_addr, RSI_IPV4_ADDRESS_LENGTH);

      // Fill network mask
      memcpy(nwk_profile_ptr->sn_mask, (uint8_t *)&mask, RSI_IPV4_ADDRESS_LENGTH);

      // Fill gateway
      memcpy(nwk_profile_ptr->default_gw, (uint8_t *)&gw, RSI_IPV4_ADDRESS_LENGTH);
    } else {
      // Not supported
    }
    // Fill feature frame params
    feature_frame_ptr->pll_mode      = PLL_MODE;
    feature_frame_ptr->rf_type       = RF_TYPE;
    feature_frame_ptr->wireless_mode = WIRELESS_MODE;
    feature_frame_ptr->enable_ppp    = ENABLE_PPP;
    feature_frame_ptr->afe_type      = AFE_TYPE;
    rsi_uint32_to_4bytes(feature_frame_ptr->feature_enables, FEATURE_ENABLES);

  } else if (type == RSI_WLAN_PROFILE_CLIENT) {
    ip_addr = RSI_CONFIG_CLIENT_IP_ADDRESS;
    mask    = RSI_CONFIG_CLIENT_SN_MASK_ADDRESS;
    gw      = RSI_CONFIG_CLIENT_GATEWAY_ADDRESS;

    client_profile_ptr = (client_profile_t *)profile_buffer;

    nwk_profile_ptr = &client_profile_ptr->network_profile;

    feature_frame_ptr = &client_profile_ptr->feature_frame_params_s;

    memset(client_profile_ptr, 0, sizeof(client_profile_t));

    // Fill WLAN feature bitmap
    rsi_uint32_to_4bytes(client_profile_ptr->wlan_feature_bit_map, RSI_CONFIG_CLIENT_WLAN_FEAT_BIT_MAP);

    // Fill TCP_IP feature bitmap
    rsi_uint32_to_4bytes(client_profile_ptr->tcp_ip_feature_bit_map, RSI_CONFIG_CLIENT_TCP_IP_FEAT_BIT_MAP);

    // Fill custom feature bitmap
    rsi_uint32_to_4bytes(client_profile_ptr->custom_feature_bit_map, RSI_CONFIG_CLIENT_CUSTOM_FEAT_BIT_MAP);

    // Fill listen interval
    rsi_uint32_to_4bytes(client_profile_ptr->listen_interval, RSI_CONFIG_CLIENT_LISTEN_INTERVAL);

    // Fill data rate
    client_profile_ptr->data_rate = RSI_CONFIG_CLIENT_DATA_RATE;

    // Fill TX power
    client_profile_ptr->tx_power = RSI_CONFIG_CLIENT_TX_POWER;

    // Fill AP band
    client_profile_ptr->band = RSI_CONFIG_CLIENT_BAND;

    // Fill Client SSID
    rsi_strcpy(client_profile_ptr->ssid, RSI_CONFIG_CLIENT_SSID);

    // Fill AP SSID length
    client_profile_ptr->ssid_len = strlen(RSI_CONFIG_CLIENT_SSID);

    // Fill scan feature bitmap
    client_profile_ptr->scan_feature_bitmap = RSI_CONFIG_CLIENT_SCAN_FEAT_BITMAP;

    // Fill client channel number
    rsi_uint16_to_2bytes(client_profile_ptr->channel, RSI_CONFIG_CLIENT_CHANNEL);

    // Scan channel bitmap magic code
    rsi_uint16_to_2bytes(client_profile_ptr->scan_chan_bitmap_magic_code, RSI_CONFIG_CLIENT_MAGIC_CODE);

    // Scan channel bitmap for 2_4 Ghz
    rsi_uint32_to_4bytes(client_profile_ptr->scan_chan_bitmap_2_4_ghz, RSI_CONFIG_CLIENT_SCAN_CHAN_BITMAP_2_4_GHZ);

    // Scan channel bitmap for 5_0 Ghz
    rsi_uint32_to_4bytes(client_profile_ptr->scan_chan_bitmap_5_0_ghz, RSI_CONFIG_CLIENT_SCAN_CHAN_BITMAP_5_0_GHZ);

    // Fill client security type
    client_profile_ptr->security_type = RSI_CONFIG_CLIENT_SECURITY_TYPE;

    // Fill AP encryption type
    client_profile_ptr->encryption_type = RSI_CONFIG_CLIENT_ENCRYPTION_TYPE;

    // Fill client PSK
    if (RSI_CONFIG_CLIENT_PSK)
      rsi_strcpy(client_profile_ptr->psk, RSI_CONFIG_CLIENT_PSK);

    // Fill client PMK
    if (strlen(RSI_CONFIG_CLIENT_PMK)) {
      for (i = 0, j = 0; i < 64 && j < 32; i += 2, j++) {
        if (RSI_CONFIG_CLIENT_PMK[i] && RSI_CONFIG_CLIENT_PMK[i + 1]) {
          client_profile_ptr->pmk[j] = ((uint16_t)convert_lower_case_to_upper_case(RSI_CONFIG_CLIENT_PMK[i])) * 16;
          client_profile_ptr->pmk[j] += convert_lower_case_to_upper_case(RSI_CONFIG_CLIENT_PMK[i + 1]);
        } else {
          client_profile_ptr->pmk[j] = 0;
        }
      }
    }

    nwk_profile_ptr->tcp_stack_used = RSI_CONFIG_CLIENT_TCP_STACK_USED;

    if (nwk_profile_ptr->tcp_stack_used == 1) {
      nwk_profile_ptr->dhcp_enable = RSI_CONFIG_CLIENT_DHCP_MODE;

      // Fill IP address
      memcpy(nwk_profile_ptr->ip_address, (uint8_t *)&ip_addr, RSI_IPV4_ADDRESS_LENGTH);

      // Fill network mask
      memcpy(nwk_profile_ptr->sn_mask, (uint8_t *)&mask, RSI_IPV4_ADDRESS_LENGTH);

      // Fill gateway
      memcpy(nwk_profile_ptr->default_gw, (uint8_t *)&gw, RSI_IPV4_ADDRESS_LENGTH);
    } else {
      // Not supported
    }

    // Fill feature frame params
    feature_frame_ptr->pll_mode      = PLL_MODE;
    feature_frame_ptr->rf_type       = RF_TYPE;
    feature_frame_ptr->wireless_mode = WIRELESS_MODE;
    feature_frame_ptr->enable_ppp    = ENABLE_PPP;
    feature_frame_ptr->afe_type      = AFE_TYPE;
    rsi_uint32_to_4bytes(feature_frame_ptr->feature_enables, FEATURE_ENABLES);

  } else if (type == RSI_WLAN_PROFILE_EAP) {
    ip_addr = RSI_CONFIG_EAP_IP_ADDRESS;
    mask    = RSI_CONFIG_EAP_SN_MASK_ADDRESS;
    gw      = RSI_CONFIG_EAP_GATEWAY_ADDRESS;

    eap_client_profile_ptr = (eap_client_profile_t *)profile_buffer;

    nwk_profile_ptr = &eap_client_profile_ptr->network_profile;

    feature_frame_ptr = &eap_client_profile_ptr->feature_frame_params_s;

    memset(eap_client_profile_ptr, 0, sizeof(eap_client_profile_t));

    // Fill WLAN feature bitmap
    rsi_uint32_to_4bytes(eap_client_profile_ptr->wlan_feature_bit_map, RSI_CONFIG_EAP_WLAN_FEAT_BIT_MAP);

    // Fill TCP_IP feature bitmap
    rsi_uint32_to_4bytes(eap_client_profile_ptr->tcp_ip_feature_bit_map, RSI_CONFIG_EAP_TCP_IP_FEAT_BIT_MAP);

    // Fill custom feature bitmap
    rsi_uint32_to_4bytes(eap_client_profile_ptr->custom_feature_bit_map, RSI_CONFIG_EAP_CUSTOM_FEAT_BIT_MAP);

    // Fill data rate
    eap_client_profile_ptr->data_rate = RSI_CONFIG_EAP_DATA_RATE;

    // Fill TX power
    eap_client_profile_ptr->tx_power = RSI_CONFIG_EAP_TX_POWER;

    // Fill band
    eap_client_profile_ptr->band = RSI_CONFIG_EAP_BAND;

    // Fill Client SSID
    rsi_strcpy(eap_client_profile_ptr->ssid, RSI_CONFIG_EAP_SSID);

    // Fill AP SSID length
    eap_client_profile_ptr->ssid_len = strlen(RSI_CONFIG_EAP_SSID);

    // Fill scan feature bitmap
    eap_client_profile_ptr->scan_feature_bitmap = RSI_CONFIG_EAP_SCAN_FEAT_BITMAP;

    // Fill security type
    eap_client_profile_ptr->security_type = RSI_CONFIG_EAP_SECURITY_TYPE;

    // Fill client channel number
    rsi_uint16_to_2bytes(eap_client_profile_ptr->channel, RSI_CONFIG_EAP_CHANNEL);

    // Scan channel bitmap magic code
    rsi_uint16_to_2bytes(eap_client_profile_ptr->scan_chan_bitmap_magic_code, RSI_CONFIG_EAP_CHAN_MAGIC_CODE);

    // Scan channel bitmap for 2_4 Ghz
    rsi_uint32_to_4bytes(eap_client_profile_ptr->scan_chan_bitmap_2_4_ghz, RSI_CONFIG_EAP_SCAN_CHAN_BITMAP_2_4_GHZ);

    // Scan channel bitmap for 5_0 Ghz
    rsi_uint32_to_4bytes(eap_client_profile_ptr->scan_chan_bitmap_5_0_ghz, RSI_CONFIG_EAP_SCAN_CHAN_BITMAP_5_0_GHZ);

    // Fill EAP method
    rsi_strcpy(eap_client_profile_ptr->eap_method, RSI_CONFIG_EAP_METHOD);

    // Fill inner method
    rsi_strcpy(eap_client_profile_ptr->inner_method, RSI_CONFIG_EAP_INNER_METHOD);

    // Fill user identity
    rsi_strcpy(eap_client_profile_ptr->user_identity, RSI_CONFIG_EAP_USER_IDENTITY);

    // Fill password
    rsi_strcpy(eap_client_profile_ptr->passwd, RSI_CONFIG_EAP_PASSWORD);

    nwk_profile_ptr->tcp_stack_used = RSI_CONFIG_EAP_TCP_STACK_USED;

    if (nwk_profile_ptr->tcp_stack_used == 1) {
      nwk_profile_ptr->dhcp_enable = RSI_CONFIG_EAP_DHCP_MODE;

      // Fill IP address
      memcpy(nwk_profile_ptr->ip_address, (uint8_t *)&ip_addr, RSI_IPV4_ADDRESS_LENGTH);

      // Fill network mask
      memcpy(nwk_profile_ptr->sn_mask, (uint8_t *)&mask, RSI_IPV4_ADDRESS_LENGTH);

      // Fill gateway
      memcpy(nwk_profile_ptr->default_gw, (uint8_t *)&gw, RSI_IPV4_ADDRESS_LENGTH);
    } else {
      // Not supported
    }

    // Fill feature frame params
    feature_frame_ptr->pll_mode      = PLL_MODE;
    feature_frame_ptr->rf_type       = RF_TYPE;
    feature_frame_ptr->wireless_mode = WIRELESS_MODE;
    feature_frame_ptr->enable_ppp    = ENABLE_PPP;
    feature_frame_ptr->afe_type      = AFE_TYPE;
    rsi_uint32_to_4bytes(feature_frame_ptr->feature_enables, FEATURE_ENABLES);

  } else if (type == RSI_WLAN_PROFILE_P2P) {
    ip_addr = RSI_CONFIG_P2P_IP_ADDRESS;
    mask    = RSI_CONFIG_P2P_SN_MASK_ADDRESS;
    gw      = RSI_CONFIG_P2P_GATEWAY_ADDRESS;

    p2p_profile_ptr = (p2p_profile_t *)profile_buffer;

    nwk_profile_ptr = &p2p_profile_ptr->network_profile;

    // feature_frame_params_t *feature_frame_ptr = NULL;

    feature_frame_ptr = &p2p_profile_ptr->feature_frame_params_s;

    memset(p2p_profile_ptr, 0, sizeof(p2p_profile_t));

    // Fill WLAN feature bitmap
    rsi_uint32_to_4bytes(p2p_profile_ptr->wlan_feature_bit_map, RSI_CONFIG_P2P_WLAN_FEAT_BIT_MAP);

    // Fill TCP_IP feature bitmap
    rsi_uint32_to_4bytes(p2p_profile_ptr->tcp_ip_feature_bit_map, RSI_CONFIG_P2P_TCP_IP_FEAT_BIT_MAP);

    // Fill custom feature bitmap
    rsi_uint32_to_4bytes(p2p_profile_ptr->custom_feature_bit_map, RSI_CONFIG_P2P_CUSTOM_FEAT_BIT_MAP);

    // Fill Join SSID
    rsi_strcpy(p2p_profile_ptr->join_ssid, RSI_CONFIG_P2P_JOIN_SSID);

    // Fill data rate
    p2p_profile_ptr->data_rate = RSI_CONFIG_CLIENT_DATA_RATE;

    // Fill TX power
    p2p_profile_ptr->tx_power = RSI_CONFIG_CLIENT_TX_POWER;

    // Fill AP band
    p2p_profile_ptr->band = RSI_CONFIG_CLIENT_BAND;

    // Fill GO intent
    rsi_uint16_to_2bytes(p2p_profile_ptr->go_intent, RSI_CONFIG_P2P_GO_INTNET);

    // Fill device name
    rsi_strcpy(p2p_profile_ptr->device_name, RSI_CONFIG_P2P_DEVICE_NAME);

    // Fill operating channel
    rsi_uint16_to_2bytes(p2p_profile_ptr->operating_channel, RSI_CONFIG_P2P_OPERATING_CHANNEL);

    // Fill SSID postfix
    rsi_strcpy(p2p_profile_ptr->ssid_postfix, RSI_CONFIG_P2P_SSID_POSTFIX);

    // Fill PSK key
    rsi_strcpy(p2p_profile_ptr->psk_key, RSI_CONFIG_P2P_PSK_KEY);

    nwk_profile_ptr->tcp_stack_used = RSI_CONFIG_P2P_TCP_STACK_USED;

    if (nwk_profile_ptr->tcp_stack_used == 1) {
      nwk_profile_ptr->dhcp_enable = RSI_CONFIG_P2P_DHCP_MODE;

      // Fill IP address
      memcpy(nwk_profile_ptr->ip_address, (uint8_t *)&ip_addr, RSI_IPV4_ADDRESS_LENGTH);

      // Fill network mask
      memcpy(nwk_profile_ptr->sn_mask, (uint8_t *)&mask, RSI_IPV4_ADDRESS_LENGTH);

      // Fill gateway
      memcpy(nwk_profile_ptr->default_gw, (uint8_t *)&gw, RSI_IPV4_ADDRESS_LENGTH);
    } else {
      // Not supported
    }

    // Fill feature frame params
    feature_frame_ptr->pll_mode      = PLL_MODE;
    feature_frame_ptr->rf_type       = RF_TYPE;
    feature_frame_ptr->wireless_mode = WIRELESS_MODE;
    feature_frame_ptr->enable_ppp    = ENABLE_PPP;
    feature_frame_ptr->afe_type      = AFE_TYPE;
    rsi_uint32_to_4bytes(feature_frame_ptr->feature_enables, FEATURE_ENABLES);

  } else {
    profile_buffer = NULL;
  }
  SL_PRINTF(SL_WLAN_FILL_CONFIG_PROFILE_EXIT, WLAN, LOG_INFO);
  return profile_buffer;
}

/*==============================================*/
/**
 * @brief       Delete stored configuration based on profile type. This is a blocking API.
 * @param[in]   type           - Profile type
 * @return    	0              - Success \n
 * @return    	Non-Zero Value - Failure (**Possible Error Codes** - 0xfffffffa) \n
 * @note        **This API is not supported in current release.**
 * @note        **Precondition** - \ref rsi_wireless_init() API needs to be called before this API.
 */

int32_t rsi_wlan_delete_profile(uint32_t type)
{
  int32_t status                 = RSI_SUCCESS;
  rsi_pkt_t *pkt                 = NULL;
  rsi_profile_req_t *profile_req = NULL;
  SL_PRINTF(SL_WLAN_DELETE_PROFILE_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;
  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_DELETE_PROFILE_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR, "status: %4x", status);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_DELETE_PROFILE_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_profile_req_t));

    profile_req = (rsi_profile_req_t *)pkt->data;

    // Fill profile type
    rsi_uint32_to_4bytes(profile_req->profile_type, type);

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send delete profile command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_DELETE_PROFILE, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_DELETE_PROFILE_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_DELETE_PROFILE_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_DELETE_PROFILE_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief     Enable or disable auto-config with respect to profile. This is a blocking API.
 * @param[in] enable   - 0 - Disable the auto configuration feature. \n
 *                       1 - Enable the auto configuration feature on power up. \n
 *                       2 - Enable the auto configuration for profile based.
 * @param[in] type   - Profile type \n
 *			               Value    |	Parameter          | Macro 
 *			               :-------:|:------------------:|:------------------------------------------------------
 *                     0        | Client profile     | RSI_WLAN_PROFILE_CLIENT 
 *                     1        | P2P profile        | RSI_WLAN_PROFILE_P2P 
 *                     2        | EAP profile        | RSI_WLAN_PROFILE_EAP 
 *                     6        | AP profile         | RSI_WLAN_PROFILE_AP 
 *                     0xF      | All profiles       | RSI_WLAN_PROFILE_ALL 
 * @return   	0              -  Success \n
 * @return   	Non-Zero Value - Failure (**Possible Error Codes** - 0xfffffffa) \n
 * @note      **Precondition** - \ref rsi_wlan_set() API needs to be called before this API.
 * @note	    Currently Profile based feature is not supported.
 * @note      If user tries to give any other command during autojoin, then user gets error 0x002C. \n
 *            To avoid this, user have to disable auto_join feature and give other commands.
 * @note      The parameters of the following APIs are saved when rsi_wlan_enable_auto_config() is called: \n
 *            rsi_wireless_init(), rsi_wlan_scan(), rsi_wlan_scan_with_bitmap_options(), rsi_wlan_connect(), \n
 *            rsi_config_ipaddress(), rsi_wireless_antenna(), rsi_wlan_bgscan_profile(), rsi_radio_caps(), rsi_wlan_ap_start
 * @note      Refer to \ref error-codes for the description of above error codes.
 *
 *
 */

int32_t rsi_wlan_enable_auto_config(uint8_t enable, uint32_t type)
{
  int32_t status                              = RSI_SUCCESS;
  rsi_pkt_t *pkt                              = NULL;
  rsi_auto_config_enable_t *config_enable_req = NULL;
  SL_PRINTF(SL_WLAN_ENABLE_AUTO_CONFIG_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_ENABLE_AUTO_CONFIG_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_ENABLE_AUTO_CONFIG_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }
    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_auto_config_enable_t));

    config_enable_req = (rsi_auto_config_enable_t *)pkt->data;

    // Fill config profile enable/disable
    config_enable_req->config_enable = enable;

    // Fill profile type
    rsi_uint32_to_4bytes(config_enable_req->profile_type, type);

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send delete profile command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_AUTO_CONFIG_ENABLE, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_AUTO_CONFIG_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_ENABLE_AUTO_CONFIG_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_ENABLE_AUTO_CONFIG_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}
/*==============================================*/
/**
 * @brief   	  Generate PMK if PSK and SSID are provided. This is a blocking API.
 * @param[in]   type   - Possible values of this field are 1, 2, and 3, but we only pass 3 for generation of PMK.
 * @param[in]   psk    - Expected parameters are pre-shared key(PSK) of the access point
 * @param[in]   ssid   - Contain the SSID of the access point, this field will be valid only if TYPE value is 3.
 * @param[in]   pmk    - PMK array
 * @param[in]   length - Length of PMK array
 * @param[out]  32-byte PMK
 * @return	    0              - Success (If type value is 3) \n
 * @return	    Non-Zero Value - Failure (**Possible Error Codes** - 0x0021,0x0025,0x0026,0x0028,0x002C,0x0039,0x003a,0x003b) \n
 * @note 		    **Precondition** - \ref rsi_wlan_connect() API needs to be called before this API.
 * @note        Refer to \ref error-codes for the description of above error codes.
 *
 */

int32_t rsi_wlan_pmk_generate(int8_t type, int8_t *psk, int8_t *ssid, uint8_t *pmk, uint16_t length)
{
  int32_t status         = RSI_SUCCESS;
  rsi_pkt_t *pkt         = NULL;
  rsi_req_psk_t *psk_ptr = NULL;
  SL_PRINTF(SL_WLAN_PMK_GENERATE_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_PMK_GENERATE_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_PMK_GENERATE_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    psk_ptr = (rsi_req_psk_t *)pkt->data;

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_req_psk_t));
    //
    // Attach the buffer given by user
    wlan_cb->app_buffer = (uint8_t *)pmk;

    // Length of the buffer provided by user
    wlan_cb->app_buffer_length = length;

    // Assign type into structure member
    psk_ptr->type = type;

    // Copy PSK given by user into structure member
    rsi_strcpy(psk_ptr->psk_or_pmk, psk);

    // Copy SSID given by user into structure member
    rsi_strcpy(psk_ptr->ap_ssid, ssid);

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send PSK command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_HOST_PSK, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_PSK_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_PMK_GENERATE_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_PMK_GENERATE_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}
/*==============================================*/
/**
 * @fn       int32_t rsi_wlan_11ax_config(void)
 * @brief    Configure the 11ax params. This is a blocking API.
 * @pre    	\ref rsi_wlan_11ax_config() API needs to be called after opermode command only
 * @return 
 * 		Non-Zero Value  - Failure \n
 *              0 		- Success \n
 *              -2 - Invalid parameters \n
 *		-3 - Command given in wrong state \n
 *		-4 - Buffer not availableto serve the command \n
 *		If return value is greater than zero : 0x0021
 * @note        Refer to Error Codes section for the description of the above error codes  \ref SP16
 * 		Parameters given here are used internally by the API
 *
 *
 */
int32_t rsi_wlan_11ax_config(uint8_t gi_ltf)
{
  rsi_pkt_t *pkt;
  int32_t status = RSI_SUCCESS;
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;
#if !(HE_PARAMS_SUPPORT)
  // Change WLAN CMD state to allow
  rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
  // Return command not supported error
  return RSI_ERROR_COMMAND_NOT_SUPPORTED;
#endif
  if (wlan_cb->state >= RSI_WLAN_STATE_INIT_DONE && wlan_cb->state < RSI_WLAN_STATE_CONNECTED) {
    // Change WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
    // Return command given in wrong state error
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  // Allocate command buffer from WLAN pool
  pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);
  // If allocation of packet fails
  if (pkt == NULL) {
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
    // Return packet allocation failure error
    return RSI_ERROR_PKT_ALLOCATION_FAILURE;
  }
#ifndef RSI_WLAN_SEM_BITMAP
  rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
  wlan_11ax_config_params_t *config = (wlan_11ax_config_params_t *)pkt->data;
  config->guard_interval            = gi_ltf;
  config->nominal_pe                = NOMINAL_PE;
  config->dcm_enable                = DCM_ENABLE;
  config->ldpc_enable               = LDPC_ENABLE;
  config->ng_cb_enable              = NG_CB_ENABLE;
  config->ng_cb_values              = NG_CB_VALUES;
  config->uora_enable               = UORA_ENABLE;
  config->trigger_rsp_ind           = TRIGGER_RESP_IND;
  config->ipps_valid_value          = IPPS_VALID_VALUE;
  config->tx_only_on_ap_trig        = TX_ONLY_ON_AP_TRIG;
  config->twt_support               = TWT_SUPPORT;
  config->config_er_su              = CONFIG_ER_SU;

  // Send antenna select command
  status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_11AX_PARAMS, pkt);

  // Wait on WLAN semaphore
  rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_WLAN_11AX_WAIT_TIME);

  // Get WLAN/network command response status
  status = rsi_wlan_get_status();
  // Return status
  return status;
}
/*==============================================*/
/**
 * @brief     Configure the sleep timer mode of the module to go into sleep during power save operation. This is a blocking API.
 * @param[in] sleep_time     - Sleep Time value in seconds. Minimum value is 1, and maximum value is 2100
 * @return    0              - Success \n
 * @return    Non-Zero Value - Failure
 * @note   	  Can be issued any time in case of power save mode 8 with handshake type as MSG_BASED.
 *
 */
int16_t rsi_wlan_set_sleep_timer(uint16_t sleep_time)
{
  int16_t status               = RSI_SUCCESS;
  rsi_pkt_t *pkt               = NULL;
  rsi_set_sleep_timer_t *timer = NULL;
  SL_PRINTF(SL_WLAN_SET_SLEEP_TIMER_ENTRY, WLAN, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_SET_SLEEP_TIMER_COMMAND_GIVEN_IN_WRONG_STATE, WLAN, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_SET_SLEEP_TIMER_PKT_ALLOCATION_FAILURE, WLAN, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    timer = (rsi_set_sleep_timer_t *)pkt->data;
    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_set_sleep_timer_t));

    // Copy sleep_timer given by user into structure member
    rsi_uint16_to_2bytes(timer->timeval, sleep_time);

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send set sleep timer command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SET_SLEEP_TIMER, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_SET_SLEEP_TIMER_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_SET_SLEEP_TIMER_WLAN_COMMAND_ERROR, WLAN, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_SET_SLEEP_TIMER_ERROR_IN_SENDING_COMMAND, WLAN, LOG_ERROR, "status: %4x", status);
  return status;
}

/*====================================================*/
/**
 * @brief      Register the WLAN callback functions. This is a non-blocking API.
 * @param[in]  callback_id                       - ID of the callback function. Table below displays supported IDs
 * 	           Parameter callback ID                     |   Value	
 *             :-----------------------------------------|:-----------------------------------------------------------
 * 	           RSI_JOIN_FAIL_CB	                         |   0
 * 		         RSI_IP_FAIL_CB                            |   1
 * 		         RSI_REMOTE_SOCKET_TERMINATE_CB            |   2
 * 		         RSI_IP_CHANGE_NOTIFY_CB                   |   3
 * 		         RSI_STATIONS_DISCONNECT_NOTIFY_CB         |   4
 * 		         RSI_STATIONS_CONNECT_NOTIFY_CB            |   5
 * 		         RSI_WLAN_DATA_RECEIVE_NOTIFY_CB           |   6
 * 		         RSI_WLAN_WFD_DISCOVERY_NOTIFY_CB          |   7
 * 		         RSI_WLAN_RECEIVE_STATS_RESPONSE_CB        |   8
 * 		         RSI_WLAN_WFD_CONNECTION_REQUEST_NOTIFY_CB |   9
 * 		         RSI_WLAN_SCAN_RESPONSE_HANDLER            |   10
 * 		         RSI_WLAN_JOIN_RESPONSE_HANDLER            |   11
 * 		         RSI_WLAN_RAW_DATA_RECEIVE_HANDLER         |   12
 * 		         RSI_WLAN_SOCKET_CONNECT_NOTIFY_CB         |   13
 * 		         RSI_WLAN_SERVER_CERT_RECEIVE_NOTIFY_CB    |   14
 * 		         RSI_WLAN_ASYNC_STATS                      |   15
 * 		         RSI_FLASH_WRITE_RESPONSE                  |   16
 * 		         RSI_WLAN_ASSERT_NOTIFY_CB                 |   17
 * 		         RSI_WLAN_MAX_TCP_WINDOW_NOTIFY_CB         |   18
 * @param[in]  callback_handler_ptr              - Callback handler
 * @param[out] status                            - Status of the asynchronous response
 * @param[out] buffer                            - Payload of the asynchronous response
 * @param[out] length                            - Length of the payload
 *             ### Prototypes of the callback functions with given callback id ###
 * 	           Callback id                               |   Function Description
 * 	           :-----------------------------------------|:-----------------------------------------------------------
 * 		         RSI_JOIN_FAIL_CB                          |   Called when asynchronous rejoin failure is received from the firmware. Application should try to re-join to the AP. This is valid in both AP and STA mode This callback is triggered when module fails to connect to AP in STA mode or when AP creation fails. \n
 *             ^                                             @note  Need to call rsi_scan() API in STA mode, rsi_wlan_ap_start() API in AP mode. 
 *             ^                                             @param buffer   NULL \n 
 *             ^                                             @param status  Possible Error response codes - 0x0019,0x0045,0x0046,0x0047,0x0048,0x0049
 * 		         RSI_IP_FAIL_CB                            |   Called when asynchronous DHCP renewal failure is received from the firmware. Application should retry IP configuration. This is valid in both AP and STA mode. This callback is triggered when module fails to renew the DHCP. 
 *             ^                                             @note Need to call rsi_wlan_connect() API. 
 *             ^                                             @param buffer  NULL \n 
 *             ^                                             @param status  Possible Error response codes - 0xFF9C, 0xFF9D
 * 		         RSI_REMOTE_SOCKET_TERMINATE_CB            |   Called when asynchronous remote TCP socket closed is received from the firmware. It is an indication given to application that the socket is terminated from remote. This is valid in both STA and AP mode. This callback is triggered when remote socket is terminated or closed 
 *             ^                                             @note  Need to connect to socket. 
 *             ^                                             @param buffer rsi_rsp_socket_close_t ( \ref rsi_rsp_socket_close_s) response structure is provided in callback   \n 
 *             ^                                             @param status  NA
 *             RSI_IP_CHANGE_NOTIFY_CB                   |   Called when asynchronous IP change notification is received from the firmware. It is an indication given to application that the IP has been modified. This is valid only in STA mode. This callback is triggered when AP changes the IP address. 
 *             ^                                             @note Need to call rsi_wlan_connect(),rsi_config_ipaddress() API. 
 *             ^                                             @param buffer  NULL \n 
 *             ^                                             @param status  RSI_SUCCESS
 *             RSI_STATIONS_DISCONNECT_NOTIFY_CB         |   Called when asynchronous station disconnect notification is received from the firmware in AP mode. It is an indication that the AP is disconnect. Application should retry to connect to the AP. This is valid when module acts as AP. This callback is triggered when stations are disconnected. 
 *             ^                                             @note STA need to connect to the Accesspoint 9116. 
 *             ^                                             @param buffer mac address is provided in response structure   \n 
 *             ^                                             @param status Response status @param length 6
 *             RSI_STATIONS_CONNECT_NOTIFY_CB            |   Called when asynchronous station connect notification is received from the firmware in AP mode. It is an indication that the application is connected to the AP. This is valid when 9116 module acts as AP. This callback is triggered when stations are connected. 
 *             ^                                             @pre  STA need to connect to the accesspoint 9116.  
 *             ^                                             @param buffer mac address is provided in response structure \n 
 *             ^                                             @param status Response status 
 *             ^                                             @param length 6
 *             RSI_WLAN_DATA_RECEIVE_NOTIFY_CB           |   Called when asynchronous data is received from the firmware in TCP/IP bypass mode. This is valid in both AP and STA mode. This callback is triggered when data is received in TCP/IP bypass mode. 
 *             ^                                             @pre  Need to connect to socket. 
 *             ^                                             @param buffer raw data received \n 
 *             ^                                             @param status  Response status
 * 		         RSI_WLAN_WFD_DISCOVERY_NOTIFY_CB          |   Called when Wi-Fi direct device discovery notification received from the firmware. This is valid in Wi-Fi Direct Mode only. This callback is triggered when a peer is discovered by the device. 
 *             ^                                             @pre  Need to call  rsi_wireless_init() ,rsi_wlan_wfd_start_discovery API.  
 *             ^                                             @param buffer  NULL \n 
 *             ^                                             @param status  Response status
 *             RSI_WLAN_RECEIVE_STATS_RESPONSE_CB        |   Called when asynchronous receive statistics from the firmware in PER mode. This is valid in PER Mode only. This callback is triggered when module wants to receive statistics. 
 *             ^                                             @pre  Need to call rsi_wlan_radio_init() API. 
 *             ^                                             @param buffer rsi_per_stats_rsp_t (\ref rsi_per_stats_rsp_s) response structure is provided in callback.  \n 
 *             ^                                             @param status  Response status
 * 		         RSI_WLAN_WFD_CONNECTION_REQUEST_NOTIFY_CB |   Called when Wi-Fi direct connection request from the firmware. This is valid in Wi-Fi Direct Mode only. This call back is triggered when there is a connection request from other peer device. 
 *             ^                                             @pre Need to rsi_wireless_init() ,rsi_wlan_wfd_start_discovery() API. 
 *             ^                                             @param buffer  device name \n 
 *             ^                                             @param status  Response status 
 *             ^                                             @param length 32
 *             RSI_WLAN_SCAN_RESPONSE_HANDLER            |   Called when a response for scan request is received from the firmware. It is an indication to host that the scan is success or failed. This is valid only STA mode. This callback is triggered when module try to scan and receive response of all the available AP's. 
 *             ^                                             @pre Need to call rsi_scan() API.  
 *             ^                                             @param buffer NULL  \n 
 *             ^                                             @param status Response status. Possible error codes - 0x0002, 0x0003, 0x0005, 0x000A, 0x0014, 0x0015, 0x001A, 0x0021,0x0024,0x0025,0x0026,0x002C,0x003c
 * 		         RSI_WLAN_JOIN_RESPONSE_HANDLER		         |   Called when a response for join request is received from the firmware. It is an indication to application that the join to AP is success or failed. This is valid in STA mode. This callback is triggered when station is successfully connected.  
 *             ^                                             @pre  Need to call rsi_scan API.  
 *             ^                                             @param buffer  NULL \n 
 *             ^                                             @param status  Response status. Possible Error response codes -0x0019, 0x0048,0x0045,0x0008
 *             RSI_WLAN_RAW_DATA_RECEIVE_HANDLER         |   Called when raw data packets are received from the firmware. This is valid in both AP and STA mode. This callback is triggered when raw data is received in TCP/IP bypass mode. 
 *             ^                                             @pre Need to connect to socket.  
 *             ^                                             @param buffer raw data  \n 
 *             ^                                             @param status Response status.
 *             RSI_WLAN_SOCKET_CONNECT_NOTIFY_CB         |   Called when a socket connection response comes to the host. This is valid in both STA and AP mode. This callback is registered and triggered when socket connects. 
 *             ^                                             @pre  Need to create socket. 
 *             ^                                             @param buffer rsi_rsp_socket_create_t ( \ref rsi_rsp_socket_create_s)   \n 
 *             ^                                             @param status Response status.
 *             RSI_WLAN_SERVER_CERT_RECEIVE_NOTIFY_CB    |   Reserved 
 *             RSI_WLAN_ASYNC_STATS                      |   Called when asynchronous response comes from the firmware to the host. Host can register this callback to get all the information regarding AP connectivity.
 *             ^                                             @param buffer rsi_state_notification_t ( \ref rsi_state_notification_s) response structure is provided in callback   \n 
 *             ^                                             @param status Response status
 * 		         RSI_WLAN_ASSERT_NOTIFY_CB                 |   Called when WLAN assertion is triggered from firmware. It returns the assert value to the application. 
 *             ^                                             @param buffer   NULL \n 
 *             ^                                             @param status   Assert Value
 *             RSI_WLAN_MAX_TCP_WINDOW_NOTIFY_CB         |   Reserved
 * 
 * ### RSI_WLAN_ASYNC_STATS ###
 * • Asychronous messages are used to indicate module state to host. Asynchronous message are enabled by setting bit 10 of the custom feature bitmap in opermode. \n
 * • In async messages time_stamp, state_code, reason_code, rsi_channel, rsi_rssi and rsi_bssid are logged. \n
 * - time_stamp (4 bytes)
 *    - This is the value of the time stamp counter at the time of message; timestamps increment at intervals of 100ms. \n
 *  
 * - state_code (1 byte)
 *    - This field indicates the state of the module. `state_code` contains two parts, the upper nibble and lower nibble. \n
 *    - The upper nibble indicates the state of the rejoin process. The following table documents the possible values of the upper nibble of state_code. \n
 * State                           | Upper Nibble | Indication                                                      
 *|:---------------------------------|:--------------|:-----------------------------------------------------------------
 * Scan Trigger (State I)          | `0x00`       | Startup. Initial Roam                                           
 * ^                                | `0x10`       | Beacon Loss. Failover Roam                                      
 * ^                               | `0x20`       | De-authentication. AP induced roam / Disconnect from supplicant 
 * Scan Result/Decision (State II) | `0x50`       | Current AP is best                                              
 * ^                                | `0x60`       | Better AP found                                                 
 * ^                               | `0x70`       | No AP found                                                     
 * Final Connection (State III)    | `0x80`       | Associated                                                      
 *  ^                               | `0x90`       | Unassociated                                                    
 *    - The lower nibble of state_code indicates the reason for a state change. The following table documents the possible values of the lower nibble of state_code.
 * Lower Nibble | Reason for State Change                                                                     
 *|:--------------|:-----------------------------------------------------------------------------------------
 * `0x00`       | No reason specified                                                                         
 * `0x01`       | No response from AP for authentication request(Authentication denial)                       
 * `0x02`       | Association denial                                                                          
 * `0x03`       | User configured AP is not present                                                           
 * `0x05`       | EAPOL TX failure                                                                            
 * `0x06`       | Deauthentication from user                                                                  
 * `0x07`       | PSK not configured                                                                          
 * `0x08`       | key-handshake failure during rejoin/roaming/after connection(Disconnection from supplicant) 
 * `0x09`       | Roaming not enabled                                                                         
 * 
 * - reason_code (1 byte)
 *      - Indicates the reason for a failure. \n
 *      - These failures also include EAP connection errors for certificate parsing. \n
 *      - All supported reason codes are listed below 
 * reason_code |   Reason codes 
 * |:-------|:----------------------
 * 0x00 | No reason specified                                                                         
 * 0x01 | No response from AP for authentication request(Authentication denial)                       
 * 0x02 | Association denial                                                                          
 * 0x03 | User configured AP is not present                                                           
 * 0x05 | EAPOL TX failure                                                                            
 * 0x06 | Deauthentication from user                                                                  
 * 0x07 | PSK not configured                                                                          
 * 0x08 | key-handshake failure during rejoin/roaming/after connection(Disconnection from supplicant) 
 * 0x09 | Roaming not enabled                                                                         
 * 0x10 | Beacon Loss (failover Roam)                                                                 
 * 0x20 | De-authentication (AP induced Roam/Deauth from supplicant)                                  
 * 0x28 |TLS CA Cert not present                      
 * 0x29 |TLS PRIVATE key not present                
 * 0x2A |TLS Client Cert not present                
 * 0x2B |TLS no Cert present                        
 * 0x2C |PEAP CA Cert not present                   
 * 0x2D |Server Cert Invalid Key Type               
 * 0x2E |Server Intermediate CA Invalid Key Type    
 * 0x2F |Server Root CA Invalid Key Type            
 * 0x30 |Client Cert Invalid Key Type               
 * 0x31 |Client Root CA Invalid Key Type            
 * 0x32 |FIPS Server Cert Invalid Length            
 * 0x33 |FIPS Server Intermediate CA Invalid Length 
 * 0x34 |FIPS Server Root CA Invalid Length         
 * 0x35 |FIPS Client Cert Invlaid Length            
 * 0x36 |FIPS Client Root CA Invalid Length         
 * 0x37 |Server Cert 4096-bit length support is not enabled                               
|* 0x38 |Server Intermediate CA 4096-bit length support is not enabled  
|* 0x39 |Server Root CA 4096-bit length support is not enabled      
|* 0x3A |Client Cert 4096-bit length support is not enabled
|* 0x3B |Client Root CA 4096-bit length support is not enabled                 
 * 0x3C |Server Cert Invalid Sign Alg               
 * 0x3D |Server Intermediate CA Invalid Sign Alg    
 * 0x3E |Server Root CA Invalid Sign Length         
 * 0x3F |Client Cert Invalid Sign Alg               
 * 0x40 |Client Root CA Invalid Sign Length         
 * 0x41 |Server Intermediate CA not Present         
 * 0x42 |Server Root CA Parse Error                 
 * 0x43 |Server Intermediate Root CA Parse Error    
 * 0x44 |Sever Cert Parse Error                     
 * 0x45 |Client Cert Parse Error                    
 * 0x46 |Incorrect Private Key Password             
 * 0x47 |EAP Failure Received                       
 * 0x48 |Client Cert Bad Date Error                 
 * 0x49 |Server Cert Bad Date Error                 
 * 0x4A |Server Root CA Bad Date Error              
 * 0x4B |Client Root CA Bad Date Error              
 * 0x4C |Server Intermediate Root CA Bad Date Error   
 * 0x4D |Pem Header Error                           
 * 0x4E |Pem Footer Error                           
 * 0x4F |Client Intermediate CA Invalid Sign Length 
 * 0x50 |Client Intermediate CA Invalid Length      
 * 0x51 |FIPS Client Intermediate CA Invalid Length 
 * 0x52 |Client Intermediate CA invalid Key Type    
 * 0x53 |Pem Error                                  
 *      - In addition to the above, reason code received in Deauthentication/Disassociation frame from AP  is added. This will set the MSB bit of reason_code. \n
 *      - If MSB bit is set in reason code, then mask it with 0x7f to get the acutal reason code received in Deauthentication/Disassociation frame. \n
 *      - In RS9116 Rev 1.4, above reason codes will come only in TCP/IP bypass mode. \n
 *      - Pem Header Error(0x4D) or Pem Footer Error(0x4E) are only applicable if certificates are loaded individually. In case if certificates are loaded combinedly in a single file, only Pem Error(0x53) will be triggered for Header or Footer errors. \n
 */
uint16_t rsi_wlan_register_callbacks(uint32_t callback_id,
                                     void (*callback_handler_ptr)(uint16_t status,
                                                                  uint8_t *buffer,
                                                                  const uint32_t length))
{
  SL_PRINTF(SL_WLAN_REGISTER_CALLBACKS_ENTRY, WLAN, LOG_INFO);
  if (callback_id > RSI_MAX_NUM_CALLBACKS) {
    /*
     *Return , if the callback number exceeds the RSI_MAX_NUM_CALLBACKS ,or
     * the callback is already registered
     */
    SL_PRINTF(SL_WLAN_REGISTER_CALLBACKS_NUMBER_EXCEEDS_MAX_NUM_CALLBACKS_OR_CALLBACK_ALREADY_REG, WLAN, LOG_INFO);
    return 1;
  }
  if (callback_id == RSI_JOIN_FAIL_CB) // check for NULL or not
  {
    // Register join fail callback handler
    rsi_wlan_cb_non_rom->callback_list.join_fail_handler = callback_handler_ptr;
  } else if (callback_id == RSI_IP_FAIL_CB) {
    // Register IP renewal fail callback handler
    rsi_wlan_cb_non_rom->callback_list.ip_renewal_fail_handler = callback_handler_ptr;
  } else if (callback_id == RSI_REMOTE_SOCKET_TERMINATE_CB) {
    // Register remote socket terminate callback handler
    rsi_wlan_cb_non_rom->callback_list.remote_socket_terminate_handler = callback_handler_ptr;
  } else if (callback_id == RSI_IP_CHANGE_NOTIFY_CB) {
    // Register IP change notify callback handler
    rsi_wlan_cb_non_rom->callback_list.ip_change_notify_handler = callback_handler_ptr;
  } else if (callback_id == RSI_STATIONS_CONNECT_NOTIFY_CB) {
    // Register station connect notify callback handler
    rsi_wlan_cb_non_rom->callback_list.stations_connect_notify_handler = callback_handler_ptr;
  } else if (callback_id == RSI_STATIONS_DISCONNECT_NOTIFY_CB) {
    // Register station disconnect notify callback handler
    rsi_wlan_cb_non_rom->callback_list.stations_disconnect_notify_handler = callback_handler_ptr;
  } else if (callback_id == RSI_WLAN_DATA_RECEIVE_NOTIFY_CB) {
    // Register WLAN data packet receive notify callback handler
    rsi_wlan_cb_non_rom->callback_list.wlan_data_receive_handler = callback_handler_ptr;
  } else if (callback_id == RSI_WLAN_RECEIVE_STATS_RESPONSE_CB) {
    // Register WLAN receive stats notify callback handler
    rsi_wlan_cb_non_rom->callback_list.wlan_receive_stats_response_handler = callback_handler_ptr;
  } else if (callback_id == RSI_WLAN_WFD_DISCOVERY_NOTIFY_CB) {
    // Register Wi-Fi direct device discovery notify callback handler
    rsi_wlan_cb_non_rom->callback_list.wlan_wfd_discovery_notify_handler = callback_handler_ptr;
  } else if (callback_id == RSI_WLAN_WFD_CONNECTION_REQUEST_NOTIFY_CB) {
    // Register Wi-Fi direct connection request notify callback handler
    rsi_wlan_cb_non_rom->callback_list.wlan_wfd_connection_request_notify_handler = callback_handler_ptr;
  } else if (callback_id == RSI_WLAN_RAW_DATA_RECEIVE_HANDLER) {
    // Register raw data receive notify callback handler
    rsi_wlan_cb_non_rom->callback_list.raw_data_receive_handler = callback_handler_ptr;
  } else if (callback_id == RSI_WLAN_RECEIVE_CSI_DATA) {
    // Register CSI data receive notify callback handler
    rsi_wlan_cb_non_rom->callback_list.wlan_receive_csi_data_response_handler = callback_handler_ptr;
  }
#ifndef RSI_M4_INTERFACE
  else if (callback_id == RSI_WLAN_SOCKET_CONNECT_NOTIFY_CB) {
    // Register socket connection notify callback handler
    rsi_wlan_cb_non_rom->callback_list.socket_connect_response_handler = callback_handler_ptr;
  } else if (callback_id == RSI_WLAN_SERVER_CERT_RECEIVE_NOTIFY_CB) {
    // Register certificate response callback handler
    rsi_wlan_cb_non_rom->callback_list.certificate_response_handler = callback_handler_ptr;
  } else if (callback_id == RSI_WLAN_ASYNC_STATS) {
    rsi_wlan_cb_non_rom->callback_list.wlan_async_module_state = callback_handler_ptr;
  }
#endif
  else if (callback_id == RSI_WLAN_ASSERT_NOTIFY_CB) {
    // Register assert notify callback handler
    rsi_wlan_cb_non_rom->callback_list.rsi_assertion_cb = callback_handler_ptr;
  } else if (callback_id == RSI_WLAN_MAX_TCP_WINDOW_NOTIFY_CB) {
    if ((RSI_TCP_IP_FEATURE_BIT_MAP & TCP_IP_FEAT_EXTENSION_VALID)
        && (RSI_EXT_TCPIP_FEATURE_BITMAP & EXT_TCP_DYNAMIC_WINDOW_UPDATE_FROM_HOST)) {
      // Register max available RX window callback handler
      rsi_wlan_cb_non_rom->callback_list.rsi_max_available_rx_window = callback_handler_ptr;
    }
  } else if (callback_id == RSI_WLAN_TWT_RESPONSE_CB) {
    rsi_wlan_cb_non_rom->callback_list.twt_response_handler = callback_handler_ptr;
  }
  SL_PRINTF(SL_WLAN_REGISTER_CALLBACKS_EXIT, WLAN, LOG_INFO);
  return 0;
}
/** @} */

/** @addtogroup NETWORK5
* @{
*/
/*==============================================*/
/**
 * @brief	    Set the socket configuration parameters. The configurations can be done using the Socket configuration in the rsi_wlan_config.h file. \n
 *            Using this API is highly recommended. Based on the socket configuration, module will use available buffers effectively. \n
 *            This is a blocking API.
 * @param     void       
 * @return    0              -  Success \n
 * @return    Non-Zero Value - Failure 	(**Possible Error Codes** - 0x0021, 0x0025, 0x002C, 0xFF6D) \n
 * @note      **Precondition** - \ref rsi_config_ipaddress() API and socket creation should be done before this API is called.
 * @note      The following parameters are provided by user,
 *  Parameters                      |    Description
 *  :-------------------------------|:--------------------------------------------------------------------------------------------------
 *  total socket                    |    Desired total number of sockets to open
 *  total_tcp_sockets               |    Desired total number of TCP sockets to open
 *  total_udp_sockets               |    Desired total number of UDP sockets to open
 *  tcp_tx_only_sockets             |    Desired total number of TCP sockets to open which are used only for data transmission
 *  tcp_rx_only_sockets             |    Desired total number of TCP sockets to open which are used only for data reception
 *  udp_tx_only_sockets             |    Desired total number of UDP sockets to open which are used only for data transmission
 *  udp_rx_only_sockets             |    Desired total number of UDP sockets to open which are used only for data reception
 *  tcp_rx_high_performance_sockets |    Desired total number of high performance TCP sockets to open. High performance sockets can be \n 
 *  ^                                    allocated with more buffers based on the buffers availability. This option is valid only for TCP \n 
 *  ^                                    data receive sockets. Socket can be opened as high performance by setting high performance bit in \n
 *  ^                                    socket create command.
 *  tcp_rx_window_size_cap          |    Desired total to increase the TCP RX window size
 *  tcp_ack_window_div_factor       |    In case of high latency networks to configure the TCP ACK division factor with respective to the \n 
 *  ^                                    window size; Increases the ACK frequency for asynchronous sockets \n
 *  ^                                    Note: \n
 *  ^                                    Default value is tcp_rx_window_size_cap.
 * @note       The following conditions must be met: \n
 * @note       total_sockets <= Maximum allowed sockets(10) \n
 * @note       (total_tcp_sockets + total_udp_sockets) <= total_sockets \n
 * @note       (total_tcp_tx_only_sockets + total_tcp_rx_only_sockets) <= total_tcp_sockets \n
 * @note       (total_udp_tx_only_sockets + total_udp_rx_only_sockets) <= total_udp_sockets \n
 * @note       total_tcp_rx_high_performance_sockets <= total_tcp_rx_only_sockets \n
 * @note       Refer to \ref error-codes for the description of above error codes.
 */
int32_t rsi_socket_config(void)
{

  rsi_pkt_t *pkt;
  rsi_socket_config_t *rsi_socket_config;
  int32_t status = RSI_SUCCESS;

  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_IP_CONFIG_DONE) {
    // Command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    // Take the user provided data and fill it in antenna select structure
    rsi_socket_config = (rsi_socket_config_t *)pkt->data;

    // Total sockets
    rsi_socket_config->total_sockets = TOTAL_SOCKETS;

    // Total TCP sockets
    rsi_socket_config->total_tcp_sockets = TOTAL_TCP_SOCKETS;

    // Total UDP sockets
    rsi_socket_config->total_udp_sockets = TOTAL_UDP_SOCKETS;

    // Total TCP TX sockets
    rsi_socket_config->tcp_tx_only_sockets = TCP_TX_ONLY_SOCKETS;

    // Total TCP RX sockets
    rsi_socket_config->tcp_rx_only_sockets = TCP_RX_ONLY_SOCKETS;

    // Total UDP TX sockets
    rsi_socket_config->udp_tx_only_sockets = UDP_TX_ONLY_SOCKETS;

    // Total UDP RX sockets
    rsi_socket_config->udp_rx_only_sockets = UDP_RX_ONLY_SOCKETS;

    // TCP RX high performance sockets
    rsi_socket_config->tcp_rx_high_performance_sockets = TCP_RX_HIGH_PERFORMANCE_SOCKETS;

    // TCP RX windows size
    rsi_socket_config->tcp_rx_window_size_cap = TCP_RX_WINDOW_SIZE_CAP;

    // TCP RX windows division factor
    rsi_socket_config->tcp_rx_window_div_factor = TCP_RX_WINDOW_DIV_FACTOR;

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send socket configuration command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_SOCKET_CONFIG, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_SOCK_CONFIG_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    return status;
  }

  // Return status
  return status;
}
/** @} */
/** @addtogroup NETWORK17
* @{
*/

/*==============================================*/
/**
 * @brief        Register and Deregister WLAN radio.
 * @param[in]    enable - To register and deregister WLAN radio
 * @param[out]   RSI_SUCCESS
 * @return		   0  -  Success \n
 * @return		   Non-Zero Value - Failure
 *
 */
/// @private
int32_t rsi_wlan_req_radio(uint8_t enable)
{
  rsi_pkt_t *pkt                   = NULL;
  int32_t status                   = RSI_SUCCESS;
  rsi_wlan_req_radio_t *wlan_radio = NULL;
  SL_PRINTF(SL_WLAN_REQ_RADIO_ENTRY, NETWORK, LOG_INFO);
  // Get WLAN CB structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    SL_PRINTF(SL_WLAN_REQ_RADIO_COMMAND_GIVEN_IN_WRONG_STATE, NETWORK, LOG_ERROR);
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }
  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {
    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_REQ_RADIO_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }
    wlan_radio = (rsi_wlan_req_radio_t *)pkt->data;

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_wlan_req_radio_t));

    wlan_radio->radio_req = enable;
    if (wlan_radio->radio_req) {
      rsi_wlan_cb_non_rom->wlan_radio = RSI_WLAN_RADIO_REGISTER;
    } else {
      rsi_wlan_cb_non_rom->wlan_radio = RSI_WLAN_RADIO_DEREGISTER;
    }

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_RADIO, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_WLAN_RADIO_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();

    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_REQ_RADIO_WLAN_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_REQ_RADIO_ERROR_IN_SENDING_COMMAND, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief       Deregister WLAN radio. This is a blocking API.
 * @param[in]   void      
 * @return    	0              -  Success \n
 * @return    	Non-Zero Value - Failure (**Possible Error Codes** - 0x0021,0x0067) \n
 * @note        If rsi_wlan_radio_deinit() API is called after rsi_wlan_scan(). Need to  call rsi_wlan_scan() API again after the rsi_wlan_radio_init(). \n
 * @note        If rsi_wlan_radio_deinit() API is allowed before WLAN connected state only.\n
 *              ### Below APIs are allowed after rsi_wlan_radio_deinit() API ###            
 *              1. \ref  rsi_wlan_radio_init()
 *              2. \ref rsi_wlan_power_save_profile() 
 *              3. \ref rsi_wlan_get() In this API only RSI_MAC_ADDRESS and RSI_CONNECTION_STATUS cmd_type are allowed.
 *              4. \ref  rsi_wireless_deinit()
 *              5. \ref  rsi_switch_proto()
 *              6. \ref  rsi_get_ram_log()
 *              7. \ref  rsi_get_fw_version()
 *              8. \ref  rsi_common_debug_log()
 *              9. \ref  rsi_bt_power_save_profile()
 * @note        **Precondition** -  The application should be in radio init and unconnected state before calling this function. \n
 *              \ref rsi_wlan_radio_deinit() API is called after \ref rsi_wlan_scan()
 *
 */

int32_t rsi_wlan_radio_deinit(void)
{
  int32_t status = RSI_SUCCESS;
  // Req for WLAN radio
  status = rsi_wlan_req_radio(RSI_DISABLE);

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_RADIO_DEINIT_ERROR_IN_SENDING_COMMAND, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}
/** @} */
//#ifdef RSI_WAC_MFI_ENABLE

#ifdef RSI_WAC_MFI_ENABLE
/** @addtogroup NETWORK18
* @{
*/

/*==============================================*/
/**
 * @brief      Add the Apple defined IE elements to the beacon command request structure.
 * @param[in]  mfi_ie - Pointer to the IE element
 * @param[in]  ie_len - Length of the IE element
 * @return     0              -  Success \n
 * @return     Non-Zero Value - Failure (**Possible Error Codes** - 0xfffffffa)\n
 */
/// @private
int32_t rsi_wlan_add_mfi_ie(int8_t *mfi_ie, uint32_t ie_len)
{
  int32_t status = RSI_SUCCESS;
  SL_PRINTF(SL_WLAN_ADD_MFI_IE_ENTRY, NETWORK, LOG_INFO);
  rsi_pkt_t *pkt;
  rsi_req_add_mfi_ie_t *mfi_ie_req = NULL;

  // Get WLAN CB struct pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_ADD_MFI_IE_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    mfi_ie_req = (rsi_req_add_mfi_ie_t *)pkt->data;

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_req_add_mfi_ie_t));

    // Fill IE length
    mfi_ie_req->mfi_ie_len = ie_len;

    // Copy IE
    memcpy(mfi_ie_req->mfi_ie, mfi_ie, ie_len);

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send WPS request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_ADD_MFI_IE, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_ADD_MFI_IE_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();
    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_ADD_MFI_IE_WLAN_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_ADD_MFI_IE_ERROR_IN_SENDING_COMMAND, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}

#endif
/** @} */

/** @addtogroup WLAN
* @{
*/
/*==============================================*/
/**
 * @brief      Assign the user-configurable channel gain values in different regions to the module from the user. This API is used for overwriting default gain tables that are present in the firmware. \n
 *             Users can load 2 gain tables (i.e. 2.4GHz-20Mhz, 5GHz-20Mhz) one after the other by changing band and bandwidth values. This is a blocking API.
 * @param[in]  band        -  0 : 2.4GHz \n
 *					                  1 : 5GHz \n
 *					                  2 : Dual band (2.4 Ghz and 5 Ghz)
 * @param[in]  bandwidth   -  0 : 20 MHz \n
 *                            1 : 40 MHz
 * @param[in]  payload     - Pass channel gain values for different regions in an given array format.
 * @param[in]  payload_len - Max payload length (table size) in 2.4GHz is 128 bytes. \n
 *                           Max payload length (table size) in 5GHz is 64 bytes
 *             ### Gain Table Payload Format ###
 *
 *                            1. Gain table Format for 2.4GHz Band: (Each entry of the table is 1 byte)
 *                               In 2.4GHz, Max Gain/Power obtained from certification should be doubled and loaded.
 *                            <TABLE NAME[]>= {
 *                            <NO.of Regions>,
 *                            <REGION NAME 1>, <CHANNEL_CODE_2G>,
 *                            <CHANNEL NUMBER 1>, <2 * MAX POWER FOR b RATE>, <2 * MAX POWER FOR g RATE>, <2 * MAX POWER FOR n RATE>, 
 *                            <CHANNEL NUMBER 2>, <2 * MAX POWER FOR b RATE>, <2 * MAX POWER FOR g RATE>, <2 * MAX POWER FOR n RATE>, 
 *                            . 
 *                            . 
 *                            .
 *                            .
 *                            .       
 *                            <CHANNEL NUMBER m-1>, <2 * MAX POWER FOR b RATE>, <2 * MAX POWER FOR g RATE>, <2 * MAX POWER FOR n RATE>,
 *                            <CHANNEL NUMBER m>, <2 * MAX POWER FOR b RATE>, <2 * MAX POWER FOR g RATE>, <2 * MAX POWER FOR n RATE>, 
 *                            <REGION NAME 2>, <CHANNEL_CODE_2G>,
 *                            <CHANNEL NUMBER 1>, <2 * MAX POWER FOR b RATE>, <2 * MAX POWER FOR g RATE>, <2 * MAX POWER FOR n RATE>,
 *                            <CHANNEL NUMBER 2>, <2 * MAX POWER FOR b RATE>, <2 * MAX POWER FOR g RATE>, <2 * MAX POWER FOR n RATE>,
 *                            . 
 *                            . 
 *                            . 
 *                            . 
 *                            <CHANNEL NUMBER m-1>, <2 * MAX POWER FOR b RATE>, <2 * MAX POWER FOR g RATE>, <2 * MAX POWER FOR n RATE>,
 *                            <CHANNEL NUMBER m>, <2 * MAX POWER FOR b RATE>, <2 * MAX POWER FOR g RATE>, <2 * MAX POWER FOR n RATE>, 
 *                            }; 
 *    
 *                            Gain table Format for 5GHz Band (Each entry of the table is 1 byte):
 *                              In 5GHz, Max Gain/Power obtained from certification should be loaded.
 *                             <TABLE NAME[]>= { 
 *                                               <NO.of Regions>, 
 *                                               <REGION NAME 1>, <CHANNEL_CODE_5G>, 
 *                                               <CHANNEL NUMBER IN BAND 1 IF ANY>, <MAX POWER FOR 11a RATE>, <MAX POWER FOR n RATE>, 
 *                                                <BAND_NUMBER 1>, <MAX POWER FOR 11a RATE>, <MAX POWER FOR n RATE>, 
 *                                               <CHANNEL NUMBER IN BAND 2 IF ANY>, <MAX POWER FOR 11a RATE>, <MAX POWER FOR n RATE>, 
 *                                               <BAND_NUMBER 2>, <MAX POWER FOR 11a RATE>, <MAX POWER FOR n RATE>, 
 *                                               <CHANNEL NUMBER IN BAND 3 IF ANY>, <MAX POWER FOR 11a RATE>, <MAX POWER FOR n RATE>, 
 *                                               <BAND_NUMBER 3>, <MAX POWER FOR 11a RATE>, <MAX POWER FOR n RATE>, 
 *                                               <CHANNEL NUMBER IN BAND 4 IF ANY>, <MAX POWER FOR 11a RATE>, <MAX POWER FOR n RATE>, 
 *                                               <BAND_NUMBER 4>, <MAX POWER FOR 11a RATE>, <MAX POWER FOR n RATE>, 
 *                                               . 
 *                                               . 
 *                                               . 
 *                                               . 
 *                                               . 
 *                                               <REGION NAME y>, <CHANNEL_CODE_5G>, 
 *                                               }; 
 *                            2. Supported Region names:
 *                                                   FCC, RED,TELEC  
 *                                                   The following are the regions and the values to be passed instead of macros in the example.
 *                                                   Region      |          Macro Value
 *                                                   ------------|--------------------
 *                                                   FCC         |            0
 *                                                   RED         |            1
 *                                                   TELEC       |            2
 *                            3. <CHANNEL_CODE_2G> is a 8 bit value which is encoded as:
 *                               If TX powers of all the channels are same, then use CHANNEL_CODE_2G as 17. In this case, mention channel number as 255.
 *                               Tf TX power is not same for all channels, then indicate CHANNEL_CODE_2G as no-of channels. And specify tx power values for all the channels indicated.
 *                            4. <CHANNEL_CODE_5G> is a 8 bit value encoded as number of rows in a region for 5G band.
 *                                a. 5G is divided into 4 sub bands: 
 *                                      band 1: channel number <= 48 
 *                                      band 2: channel number > 48 and channel number <= 64 
 *                                      band 3: channel number > 64 and channel number <= 144 
 *                                      band 4: channel number > 144 
 *                                b. If any channel in a band has different set of power values, specify the channel number followed by power values. 
 *                                c. If all the channels in a band 1 has same power values, specify the band number as 1 followed by power value. 
 *                                d. If all the channels in a band 2 has same power values, specify the band number as 2 followed by power value. 
 *                                e. If all the channels in a band 3 has same power values, specify the band number as 3 followed by power value. 
 *                                f. If all the channels in a band 4 has same power values, specify the band number as 4 followed by power value.
 *
 *
 * ### Example payload formats ###
 *
 *                          Examples: 
 *                          For 2.4GHz Band in 20MHz bandwidth
 *                            {3, //NUM_OF_REGIONS 
 *                                FCC, 13, //NUM_OF_CHANNELS 
 *                            //   rate,  11b, 11g, 11n   
 *                                    1,  34,  20,  20,  
 *                                    2,  34,  28,  28,  
 *                                    3,  34,  32,  32,  
 *                                    4,  34,  36,  36,  
 *                                    5,  34,  38,  38,  
 *                                    6,  34,  40,  40,  
 *                                    7,  34,  38,  38,  
 *                                    8,  34,  36,  36,  
 *                                    9,  34,  32,  32,  
 *                                    10, 34,  32,  32,  
 *                                    11, 34,  24,  24,  
 *                                    12, 34,  16,  24,  
 *                                    13, 34,  12,  12,  
 *                                TELEC, 17, 
 *                                     255, 20,  16, 16, 
 *                            }; //}}} 
 *
 *                          For 5GHz band in 20MHz bandwidth
 *                             {2, 
 *                             FCC, 6, 
 *                                 1,  9, 10, //band 1 
 *                                 2,  8,  9, //band 2 
 *                               100,  4,  4, //band 3 
 *                                 3,  6,  8, //band 3 
 *                               149,  3,  3, //band 4 
 *                             TELEC, 4, 
 *                               1, 9, 10, //band 1 
 *                               2, 8, 10, //band 2   
 *                               3, 6,  8, //band 3   
 *                               4, 6,  7, //band 4 
 *                             };
 *                              
 *                                
 * ###Customers using Certified MARS antenna should use the gain table structures below:###
 *
 *                          For 2.4GHz Band in 20MHz bandwidth
 *                           {3,//NUM_OF_REGIONS
 *                               FCC, 0xD,//NUM_OF_CHANNELS
 *                           //   rate,  11b, 11g, 11n
 *                                   1,  28,  32,  30,
 *                                   2,  28,  32,  30,
 *                                   3,  28,  32,  30,
 *                                   4,  30,  28,  34,
 *                                   5,  30,  28,  34,
 *                                   6,  30,  28,  34,
 *                                   7,  30,  28,  34,
 *                                   8,  30,  28,  34,
 *                                   9,  28,  30,  30,
 *                                   10, 28,  30,  30,
 *                                   11, 28,  30,  30,
 *                                   12, 28,  30,  30,
 *                                   13, 28,  30,  30,
 *                               TELEC,0x11, //NA
 *                                    255, 20,  16, 16,
 *                           };
 *                            
 *                          For 5GHz band in 20MHz bandwidth
 *                           {2,
 *                           FCC, 0x6,
 *                               1, 12, 12, //band 1
 *                               2, 11, 11, //band 2   
 *                             100, 10, 12, //band 3   
 *                               3, 13, 13, //band 3   
 *                             140, 10, 11, //band 4   
 *                               4, 13, 13, //band 4   
 *                           TELEC, 0x4, //NA
 *                             1, 9, 10, //band 1
 *                             2, 8, 10, //band 2   
 *                             3, 6,  8, //band 3   
 *                             4, 6,  7, //band 4
 *                           };
 *                            
 *                            
 * @return     0              -  Success \n
 * @return     Non-Zero	Value - Failure (**Possible Error Codes** - 0xfffffffe,0xfffffffd,0x0021,0x003E) \n
 * @note   	   **Precondition** - \ref rsi_radio_init() API needs to be called before this API
 * @note		   Length of the payload should match with payload_len parameter value.
 * @note		   40MHz is not supported in both 2.4GHz and 5GHz
 * @note       This API must be used by customers who has done FCC/RED/TELEC certification with their own antenna. Inappropriate use of this API may result in violation of FCC/RED/TELEC, or any certifications and Silicon labs is not liable for that.
 * @note       Internally firmware maintains two gain tables: Worldwide table & Region based table. Worldwide table is populated by firmware with maximum power values that chip can transmit that meets target specifications like EVM. Region based table has default gain value set.
 * @note       When certifying with your own antenna, region must be set to Worldwide and sweep the power from 0 to 21dBm. Arrive at maximum power level that is passing certification especially band-edge.
 * @note       These FCC/RED/TELEC maximum power level should be loaded in end-to-end mode via WLAN User Gain table.
 * @note       This API must be called after every boot-up since this information is not saved inside flash.
 * @note       Region based user gain table sent by application is copied onto Region based table. SoC uses this table in FCC/RED/TELEC to limit power and not to violate allowed limits. For Worldwide region firmware uses Worldwide table for transmit. \n
 *             For other regions (FCC/RED/TELEC), firmware uses minimum value of Worldwide and Region based table for transmit.
 * @note       There will be part to part variation across chips and offsets are estimated during manufacturing flow which will be applied as correction factor during normal mode of operation.
 * @note       Refer to \ref error-codes for the description of above error codes.
 */
int32_t rsi_wlan_update_gain_table(uint8_t band, uint8_t bandwidth, uint8_t *payload, uint16_t payload_len)
{
  int32_t status = 0;
  SL_PRINTF(SL_WLAN_UPDATE_GAIN_TABLE_ENTRY, NETWORK, LOG_INFO);
  rsi_pkt_t *pkt = NULL;

  rsi_gain_table_info_t *gain_table = NULL;

  // Get WLAN CB struct pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Check for gain table max supported payload length
  if (payload_len > GAIN_TABLE_MAX_PAYLOAD_LEN) {
    SL_PRINTF(SL_WLAN_UPDATE_GAIN_TABLE_INVALID_PARAM, NETWORK, LOG_ERROR);
    return RSI_ERROR_INVALID_PARAM;
  }

  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_UPDATE_GAIN_TABLE_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    gain_table = (rsi_gain_table_info_t *)pkt->data;

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_gain_table_info_t));

    // Fill band
    gain_table->band = band;

    // Fill bandwidth
    gain_table->bandwidth = bandwidth;

    // Fill payload length
    gain_table->size = payload_len;

    // Fill payload content
    memcpy(gain_table->gain_table, payload, payload_len);

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_GAIN_TABLE, pkt);

    // Wait on WLAN semaphore
    rsi_wait_on_wlan_semaphore(&rsi_driver_cb_non_rom->wlan_cmd_sem, RSI_GAIN_TABLE_RESPONSE_WAIT_TIME);

    // Get WLAN/network command response status
    status = rsi_wlan_get_status();

    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_UPDATE_GAIN_TABLE_WLAN_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_UPDATE_GAIN_TABLE_ERROR_IN_SENDING_COMMAND, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}

/*==============================================*/
/**
 * @brief     Enable or disable CSI data retrieval with configured periodicity.
 * @pre   	  \ref rsi_wlan_scan() API needs to be called before this API.
 * @param[in] enable                                - Enable or disable CSI data retrieval. 
 * @param[in] periodicity                           - Periodicity of CSI data retrieval in microseconds.
 * @param[in] num_of_mac_addr                       - Number of MAC addresses.
 * @param[in] mac_addr                              - Pointer to a 2-D matrix containing the list of MAC addresses.
 * @param[in] rsi_wlan_csi_data_response_handler    - Called when CSI data has been received from the module.
 * @note      Callback implementation example - \n
 *            void rsi_wlan_csi_data_response_handler(uint16_t status, rsi_rsp_csi_data_t *payload, const uint32_t payload_length) \n
 * @param[out]	status               - Response status: 0 - Success, 0x21 - rsi_wlan_csi_config_async() called in wrong state 
 * @param[out]  payload              - Response buffer
 * @param[out]  payload_length       - Length of the response buffer. Payload length will be sizeof(rsi_rsp_csi_data_t) bytes.
 * @note      \ref rsi_rsp_csi_data_s : Structure type supposed to hold incoming CSI data
 * @return    0              - Success \n
 *            Non-Zero Value - Failure \n
 *            -1  - Callback not registered \n
 *            -4  - Buffer unavailable to serve the command
 * @note      Refer to Error Codes section for the description of the above error codes  \ref error-codes
 * 
 */
int32_t rsi_wlan_csi_config_async(uint8_t enable,
                                  uint32_t periodicity,
                                  uint8_t num_of_mac_addr,
                                  uint8_t (*mac_addr)[6],
                                  void (*wlan_csi_data_response_handler)(uint16_t status,
                                                                         uint8_t *buffer,
                                                                         const uint32_t length))
{
  int32_t status                   = 0;
  rsi_pkt_t *pkt                   = NULL;
  rsi_csi_config_t *rsi_csi_config = NULL;

  SL_PRINTF(SL_WLAN_CSI_CONFIG_ENTRY, NETWORK, LOG_INFO);

  if (rsi_wlan_cb_non_rom->callback_list.wlan_receive_csi_data_response_handler == NULL) {
    if (wlan_csi_data_response_handler != NULL) {
      rsi_wlan_cb_non_rom->callback_list.wlan_receive_csi_data_response_handler = wlan_csi_data_response_handler;
    } else {
      return RSI_FAILURE;
    }
  }

  // Get WLAN CB struct pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  status = rsi_check_and_update_cmd_state(WLAN_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer from WLAN pool
    pkt = rsi_pkt_alloc(&wlan_cb->wlan_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change the WLAN CMD state to allow
      rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);
      // Return packet allocation failure error
      SL_PRINTF(SL_WLAN_CSI_CONFIG_PKT_ALLOCATION_FAILURE, NETWORK, LOG_ERROR, "status: %4x", status);
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    rsi_csi_config = (rsi_csi_config_t *)pkt->data;

    // Memset buffer
    memset(&pkt->data, 0, sizeof(rsi_csi_config_t));

    // Fill enable/disable status
    rsi_csi_config->csi_enable = enable;

    // Fill periodicity of receiving CSI data from module
    rsi_csi_config->periodicity = periodicity;

    // Fill the number of mac addresses
    rsi_csi_config->num_of_mac_addr = num_of_mac_addr;

    // Copy the MAC addresses
    memcpy(rsi_csi_config->mac_addresses, mac_addr, num_of_mac_addr * RSI_MAC_ADDR_LEN);

#ifndef RSI_WLAN_SEM_BITMAP
    rsi_driver_cb_non_rom->wlan_wait_bitmap |= BIT(0);
#endif
    // Send CSI data retrieval request command
    status = rsi_driver_wlan_send_cmd(RSI_WLAN_REQ_GET_CSI_DATA, pkt);

    // Change the WLAN CMD state to allow
    rsi_check_and_update_cmd_state(WLAN_CMD, ALLOW);

  } else {
    // Return WLAN command error
    SL_PRINTF(SL_WLAN_CSI_CONFIG_WLAN_COMMAND_ERROR, NETWORK, LOG_ERROR, "status: %4x", status);
    return status;
  }

  // Return status if error in sending command occurs
  SL_PRINTF(SL_WLAN_CSI_CONFIG_ERROR_IN_SENDING_COMMAND, NETWORK, LOG_ERROR, "status: %4x", status);
  return status;
}
/** @} */

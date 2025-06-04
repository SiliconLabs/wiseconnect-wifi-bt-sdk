/*******************************************************************************
* @file  rsi_index_based_ssl_client_sockets.c
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
 * @file    rsi_index_based_ssl_client_sockets.c
 * @version 1.0
 * @date    16 Apr 2025
 *
 *
 *
 *  @brief : This file contains example application for three TCP client socket connections over SSL with the certificates loaded in to FLASH
 *
 *  @section Description  This file contains example application for three TCP client socket connections over SSL with the SSL certificates loaded in to FLASH
 *
 *
 */

/**
 * Include files
 * */
#include <stdlib.h>

#include "rsi_index_based_ssl_client_sockets.h"

//! Certificates to be loaded
#include "cacert.pem"

//! Global Variables
rsi_wlan_app_cb_t rsi_wlan_app_cb; //! application control block

//! Join_fail indication
uint8_t rejoin_fail = 0;

//! http receive status
int32_t http_ota_recv_status;

volatile int32_t rsp_received = 0;

#ifdef RSI_WITH_OS
rsi_task_handle_t wlan_task_handle   = NULL;
rsi_task_handle_t driver_task_handle = NULL;
rsi_semaphore_handle_t rsi_http_otaf_sem;
rsi_semaphore_handle_t rsi_http_response_sem;
#endif

void join_fail_notify_handler(uint16_t sock_no, uint8_t *buffer, uint32_t length)
{
  UNUSED_PARAMETER(sock_no); //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(buffer);  //This statement is added only to resolve compilation warning, value is unchanged
  UNUSED_PARAMETER(length);  //This statement is added only to resolve compilation warning, value is unchanged
  LOG_PRINT("\n join fail notify received \n");
#ifdef RSI_WITH_OS
  if (rsi_wlan_app_cb.state == RSI_WLAN_HTTP_OTA_UPDATE_INDEX3_STATE) {
    rsi_semaphore_post(&rsi_http_response_sem);
  }
#endif
  rejoin_fail = 1;
  /* No explict response will be received from FW for OTA upgarde status, so change the http_ota_recv_status to FAILED here */
  http_ota_recv_status = RSI_HTTP_OTAF_UPGRADE_FAILED;
}

void rsi_wlan_app_callbacks_init(void)
{
  rsi_wlan_register_callbacks(RSI_JOIN_FAIL_CB, join_fail_notify_handler);
}

void output_values(http_response_t response)
{
  if (strcmp((const char *)response.output_data, "-----BEGIN CERTIFICATE-----")) {
    size_t copy_len = response.output_len < CACERT_BUF_SIZE - 1 ? response.output_len : CACERT_BUF_SIZE - 1;
    memcpy(cacert_buf, response.output_data, copy_len);
    cacert_buf[copy_len] = '\0';
  }
}

int32_t rsi_index_based_ssl_client_socket_app()
{
  uint8_t ip_buff[20];
  uint8_t resp_buf[RSI_FIRMWARE_NAME_SIZE];
  int32_t status      = RSI_SUCCESS;
  uint8_t i           = 0;
  uint16_t flags      = HTTP_INDEX2_FLAGS;
  uint16_t flags_otaf = HTTP_INDEX3_FLAGS;
#if !(DHCP_MODE)
  uint32_t ip_addr      = ip_to_reverse_hex(DEVICE_IP);
  uint32_t network_mask = ip_to_reverse_hex(NETMASK);
  uint32_t gateway      = ip_to_reverse_hex(GATEWAY);
#else
  uint8_t dhcp_mode = (RSI_DHCP | RSI_DHCP_UNICAST_OFFER);
#endif

  http_response_t response = { .output_data = NULL, .output_len = 0 };

  memset(resp_buf, 0, sizeof(resp_buf));

  //! Driver initialization
  status = rsi_driver_init(global_buf, GLOBAL_BUFF_LEN);
  if ((status < 0) || (status > GLOBAL_BUFF_LEN)) {
    return status;
  }

  //! SiLabs module intialisation
  status = rsi_device_init(LOAD_NWP_FW);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nDevice Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nDevice Initialization Success\r\n");
  }

#ifdef RSI_WITH_OS
  //! OS case
  //! Task created for Driver task
  rsi_task_create((rsi_task_function_t)rsi_wireless_driver_task,
                  (uint8_t *)"driver_task",
                  RSI_DRIVER_TASK_STACK_SIZE,
                  NULL,
                  RSI_DRIVER_TASK_PRIORITY,
                  &driver_task_handle);
#endif
  //! WC initialization
  status = rsi_wireless_init(0, 0);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nWireless Initialization Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nWireless Initialization Success\r\n");
  }

#ifdef RSI_WITH_OS
  rsi_semaphore_create(&rsi_http_response_sem, 0);
#endif

  uint8_t arr[10] = { 0 };
  status          = rsi_driver_version(arr);
  LOG_PRINT("\r\nDriver Version: %s\r\n", arr);

  uint8_t fmversion[20] = { 0 };
  status                = rsi_get_fw_version(fmversion, sizeof(fmversion));
  LOG_PRINT("\r\nFirmware Version: %s\r\n", fmversion);

  //! Send feature frame
  status = rsi_send_feature_frame();
  if (status != RSI_SUCCESS) {
    return status;
  }

  for (i = 0; i < MAX_SSL_CERTS_TO_FLASH; i++) {
    //! Clearing SSL CA certificate loaded in to FLASH if any with index i
    status = rsi_wlan_set_certificate_index(RSI_SSL_CA_CERTIFICATE, i, NULL, 0);
    if (status != RSI_SUCCESS) {
      return status;
    }

    //! Clearing SSL Client certificate loaded in to FLASH if any with index i
    status = rsi_wlan_set_certificate_index(RSI_SSL_CLIENT, i, NULL, 0);
    if (status != RSI_SUCCESS) {
      return status;
    }

    //! Clearing SSL private key loaded in to FLASH if any with index i
    status = rsi_wlan_set_certificate_index(RSI_SSL_CLIENT_PRIVATE_KEY, i, NULL, 0);
    if (status != RSI_SUCCESS) {
      return status;
    }
  }

  LOG_PRINT("\r\nClearing SSL CA certificate loaded in to FLASH if any with index 3\r\n");

  //! Clearing SSL CA certificate loaded in to FLASH if any with index 3
  status = rsi_wlan_set_certificate_index(RSI_SSL_CA_CERTIFICATE, RSI_CERT_INDEX_3, NULL, 0);
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nClearing SSL CA certificate in to FLASH with index 3 Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nClearing SSL CA certificate loaded in to FLASH if any with index 3 Success\r\n");
  }

  //! Loading SSL CA certificate in to FLASH with index 0
  status = rsi_wlan_set_certificate_index(RSI_SSL_CA_CERTIFICATE, RSI_CERT_INDEX_0, cacert, (sizeof(cacert) - 1));
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nLoading SSL CA certificate in to FLASH with index 0 Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nLoading SSL CA certificate in to FLASH with index 0 Success\r\n");
  }

  //! Loading SSL CA certificate in to FLASH with index 1
  status = rsi_wlan_set_certificate_index(RSI_SSL_CA_CERTIFICATE, RSI_CERT_INDEX_1, cacert, (sizeof(cacert) - 1));
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nLoading SSL CA certificate in to FLASH with index 1 Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nLoading SSL CA certificate in to FLASH with index 1 Success\r\n");
  }

  //! Loading SSL CA certificate in to FLASH with index 2
  status = rsi_wlan_set_certificate_index(RSI_SSL_CA_CERTIFICATE, RSI_CERT_INDEX_2, cacert, (sizeof(cacert) - 1));
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nLoading SSL CA certificate in to FLASH with index 2 Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nLoading SSL CA certificate in to FLASH with index 2 Success\r\n");
  }
  rsi_wlan_app_cb.state = RSI_WLAN_INITIAL_STATE;
#ifdef RSI_WITH_OS
  rsi_semaphore_post(&rsi_http_otaf_sem);
#endif
  while (1) {
#ifdef RSI_WITH_OS
    rsi_semaphore_wait(&rsi_http_otaf_sem, HTTP_OTAF_SEM_WAIT_TIMEOUT);
#endif
    switch (rsi_wlan_app_cb.state) {
      case RSI_WLAN_INITIAL_STATE: {
        rsi_wlan_app_callbacks_init();
        rsi_wlan_app_cb.state = RSI_WLAN_SCAN_STATE; //! update WLAN application state to scan state
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_http_otaf_sem);
#endif
      } break;
      case RSI_WLAN_SCAN_STATE: {
        //! Scan for Access points
        status = rsi_wlan_scan((int8_t *)SSID, 0, NULL, 0);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nWLAN AP Scan Failed\r\n");
          rsi_wlan_app_cb.state = RSI_WLAN_SCAN_STATE;
        } else {
          LOG_PRINT("\r\nWLAN AP Scan Success\r\n");
          rsi_wlan_app_cb.state = RSI_WLAN_UNCONNECTED_STATE; //! update WLAN application state to unconnected state
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_http_otaf_sem);
#endif
      } break;
      case RSI_WLAN_UNCONNECTED_STATE: {
        //! Connect to an Access point
        LOG_PRINT("Wifi Connecting...\n");
        status = rsi_wlan_connect((int8_t *)SSID, SECURITY_TYPE, PSK);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nWLAN AP Connect Failed\r\n");
          rsi_wlan_app_cb.state = RSI_WLAN_UNCONNECTED_STATE;
        } else {
          LOG_PRINT("\r\nWLAN AP Connect Success\r\n");
          rsi_wlan_app_cb.state = RSI_WLAN_CONNECTED_STATE; //! update WLAN application state to connected state
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_http_otaf_sem);
#endif
      } break;
      case RSI_WLAN_CONNECTED_STATE: {
        //! Configure IP
#if DHCP_MODE
        status = rsi_config_ipaddress(RSI_IP_VERSION_4, dhcp_mode, 0, 0, 0, ip_buff, sizeof(ip_buff), 0);
#else
        status = rsi_config_ipaddress(RSI_IP_VERSION_4,
                                      RSI_STATIC,
                                      (uint8_t *)&ip_addr,
                                      (uint8_t *)&network_mask,
                                      (uint8_t *)&gateway,
                                      NULL,
                                      0,
                                      0);
#endif
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nIP Config Failed, Error Code : 0x%lX\r\n", status);
          rsi_wlan_app_cb.state = RSI_WLAN_CONNECTED_STATE;
        } else {
          LOG_PRINT("\r\nIP Config Success\r\n");
          LOG_PRINT("RSI_STA IP ADDR: %d.%d.%d.%d \r\n", ip_buff[6], ip_buff[7], ip_buff[8], ip_buff[9]);
          rsi_wlan_app_cb.state = RSI_WLAN_IPCONFIG_DONE_STATE;
        } //! update WLAN application state to ipconfig done state

#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_http_otaf_sem);
#endif
      } break;
      case RSI_WLAN_IPCONFIG_DONE_STATE: {
        rsi_wlan_app_cb.state = RSI_WLAN_HTTP_CLIENT_INDEX0_STATE;
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_http_otaf_sem);
#endif
      } break;
      case RSI_WLAN_HTTP_CLIENT_INDEX0_STATE: {
        LOG_PRINT("\r\nStart HTTPS Client task for Index_0\r\n");
        rsi_http_client_task();
        LOG_PRINT("\r\nHTTP Client task for Index_0 is completed\r\n");
        rsi_wlan_app_cb.state = RSI_WLAN_MQTT_CLIENT_INDEX1_STATE;
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_http_otaf_sem);
#endif
      } break;

      case RSI_WLAN_MQTT_CLIENT_INDEX1_STATE: {
        LOG_PRINT("\r\nStart MQTT Client task for Index_1\r\n");
        rsi_mqtt_client_task();
        LOG_PRINT("\r\nMQTT Client task for Index_1 is completed\r\n");
        rsi_wlan_app_cb.state = RSI_WLAN_HTTP_CLIENT_INDEX2_STATE;
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_http_otaf_sem);
#endif
      } break;

      case RSI_WLAN_HTTP_CLIENT_INDEX2_STATE: {
        LOG_PRINT("\r\nStart HTTP Client task for Index_2\r\n");
        rsi_http_client_fw_listing_task();
        LOG_PRINT("\r\nHTTP Client task is for Index_2 completed\r\n");
        rsi_wlan_app_cb.state = RSI_WLAN_HTTP_OTA_UPDATE_INDEX3_STATE;
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_http_otaf_sem);
#endif
      } break;

      case RSI_WLAN_HTTP_OTA_UPDATE_INDEX3_STATE: {
        LOG_PRINT("\r\nStart HTTP Client task for Index_3\r\n");
        LOG_PRINT("\r\nLocal Apache Server IP Address: %s ......\r\n", HTTP_HOSTNAME);

        //! resetting the variables
        rsi_http_reset_handler();

        //! send http get request to download the certificate from server-2
        status = rsi_http_client_get_async((uint16_t)flags,
                                           (uint8_t *)SERVER_IP_ADDRESS,
                                           (uint16_t)HTTP_SERVER_PORT,
                                           (uint8_t *)HTTP_CERTS_DOWNLOAD_URL,
                                           (uint8_t *)HTTP_HOSTNAME,
                                           (uint8_t *)HTTP_EXTENDED_HEADER,
                                           (uint8_t *)USERNAME,
                                           (uint8_t *)PASSWORD,
                                           rsi_http_response_handler);
        //! wait for the get success response
        status = rsi_http_response_status((int32_t *)&http_recv_status);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nHTTP Get request failed, Error Code : 0x%lX\r\n", status);
          return status;
        } else {
          LOG_PRINT("\r\nHTTP Get request Success\r\n");
        }
        //! Loading SSL CA certificate in to FLASH with index 3
        status = rsi_wlan_set_certificate_index(RSI_SSL_CA_CERTIFICATE,
                                                RSI_CERT_INDEX_3,
                                                cacert_buf,
                                                (sizeof(cacert_buf) - 1));
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nLoading SSL CA certificate in to FLASH with index 3 Failed, Error Code : 0x%lX\r\n", status);
          return status;
        } else {
          LOG_PRINT("\r\nLoading SSL CA certificate in to FLASH with index 3 Success\r\n");
        }

        //! resetting the variables
        rsi_http_reset_handler();

        LOG_PRINT("\r\nFirmware download in progress from Local Apache Server......\r\n");

        //! send http otaf request for the given url
        status = rsi_http_fw_update((uint16_t)flags_otaf,
                                    (uint8_t *)SERVER_IP_ADDRESS,
                                    (uint16_t)HTTP_SERVER_PORT,
                                    (uint8_t *)HTTP_FW_UPDATE_URL,
                                    (uint8_t *)HTTP_HOSTNAME,
                                    (uint8_t *)HTTP_EXTENDED_HEADER,
                                    (uint8_t *)USERNAME,
                                    (uint8_t *)PASSWORD,
                                    http_otaf_response_handler);

        status = rsi_http_fw_update_response_status(&http_ota_recv_status);
        if (status != RSI_SUCCESS) {
          http_ota_recv_status = 0;
          if (rejoin_fail) /* If Re-join fail is received, then retry FW upgrade */
          {
            rejoin_fail = 0;
            status      = rsi_wlan_get_status();
            LOG_PRINT("\r\n Reset the state to RSI_WLAN_INITIAL_STATE\r\n");
            rsi_wlan_app_cb.state = RSI_WLAN_INITIAL_STATE;
          } else {
            LOG_PRINT("\r\nFirmware Download Restart ....\r\n");
            status                = RSI_SUCCESS;
            rsi_wlan_app_cb.state = RSI_WLAN_HTTP_OTA_UPDATE_INDEX3_STATE;
          }
        } else {
          rsi_wlan_app_cb.state = RSI_WLAN_HTTP_OTA_WIRELESS_DEINT_STATE;
        }
#ifdef RSI_WITH_OS
        rsi_semaphore_post(&rsi_http_otaf_sem);
#endif
      } break;
      case RSI_WLAN_HTTP_OTA_WIRELESS_DEINT_STATE: {
#ifndef RSI_M4_INTERFACE
#ifdef RSI_WITH_OS
        status = rsi_destroy_driver_task_and_driver_deinit(driver_task_handle);
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nDriver deinit failed, Error Code : 0x%lX\r\n", status);
        } else {
          LOG_PRINT("\r\nTask destroy and driver deinit success\r\n");
        }
#endif
#endif
        status = rsi_wireless_deinit();
        if (status != RSI_SUCCESS) {
          LOG_PRINT("\r\nWireless deinit failed, Error Code : 0x%1X\r\n", (unsigned int)status);
        }
#ifndef RSI_M4_INTERFACE
#ifdef RSI_WITH_OS
        // Task created for Driver task
        rsi_task_create((rsi_task_function_t)rsi_wireless_driver_task,
                        (uint8_t *)"driver_task",
                        RSI_DRIVER_TASK_STACK_SIZE,
                        NULL,
                        RSI_DRIVER_TASK_PRIORITY,
                        &driver_task_handle);
#endif
#endif

#ifdef RSI_M4_INTERFACE
        RSI_CLK_M4ssRefClkConfig(M4CLK, ULP_32MHZ_RC_CLK);
#endif
        status = rsi_wireless_init(0, 0);

        if (status == RSI_SUCCESS) {
          status = rsi_wlan_get(RSI_FW_VERSION, resp_buf, RSI_FIRMWARE_NAME_SIZE);
          LOG_PRINT("\r\nFirmware version after update: %s\r\n", resp_buf);
          LOG_PRINT("\r\nFW update task for Index_3 success\r\n");
        } else {
          http_ota_recv_status = status;
          LOG_PRINT("\r\n,Error Code : 0x%X\r\n", (unsigned int)status);
          LOG_PRINT("\r\nFirmware Update Failed\r\n");
        }
        rsi_wlan_app_cb.state = RSI_WLAN_HTTP_OTA_UPDATE_DONE;
        free(response.output_data);
      } break;

      case RSI_WLAN_HTTP_OTA_UPDATE_DONE: {
      } break;
      default:
        break;
    }
  }
}

void main_loop(void)
{
  while (1) {
    ////////////////////////
    //! Application code ///
    ////////////////////////

    //! event loop
    rsi_wireless_driver_task();
  }
}

int main()
{
  int32_t status = RSI_SUCCESS;

#ifdef RSI_WITH_OS
  //! OS case
  //! Task created for WLAN task
  rsi_task_create((rsi_task_function_t)(int32_t)rsi_index_based_ssl_client_socket_app,
                  (uint8_t *)"wlan_task",
                  RSI_WLAN_TASK_STACK_SIZE,
                  NULL,
                  RSI_WLAN_TASK_PRIORITY,
                  &wlan_task_handle);

  //! OS TAsk Start the scheduler
  rsi_start_os_scheduler();

#else
  //! NON - OS case
  //! Call SSL client with certificates loaded in to RAM application
  status = rsi_index_based_ssl_client_socket_app();

  //! Application main loop
  main_loop();
#endif
  return status;
}

//! http otaf response notify call back handler
void http_otaf_response_handler(uint16_t status, const uint8_t *buffer)
{
  UNUSED_CONST_PARAMETER(buffer); //This statement is added only to resolve compilation warning, value is unchanged
  uint8_t resp_buf[RSI_FIRMWARE_NAME_SIZE];

  //! memset the buffer
  memset(resp_buf, 0, sizeof(resp_buf));

  if (status == RSI_SUCCESS) {
    //LOG_PRINT("\r\n[http_otaf_response_handler] [http_ota_recv_status = RSI_HTTP_OTAF_UPGRADE_SUCCESS]\r\n");
    http_ota_recv_status = RSI_HTTP_OTAF_UPGRADE_SUCCESS;
#ifdef AWS_ENABLE
    LOG_PRINT("\r\nFirmware download complete using AWS S3 Bucket\r\n");
#elif AZURE_ENABLE
    LOG_PRINT("\r\nFirmware download complete using AZURE Blob Storage\r\n");
#else
    LOG_PRINT("\r\nFirmware download complete using Local Apache Server\r\n");
#endif
  } else {
    LOG_PRINT("\r\nFirmware update FAILED , error code : %X\r\n", status);
    http_ota_recv_status = RSI_HTTP_OTAF_UPGRADE_FAILED;
  }
#ifdef RSI_WITH_OS
  rsi_semaphore_post(&rsi_http_response_sem);
#endif
}

int32_t rsi_http_fw_update_response_status(int32_t *rsp_var)
{
  //! wait for the success response
#ifndef RSI_WITH_OS
  do {
    //! event loop
    rsi_wireless_driver_task();
  } while (!(*rsp_var));
#else
  rsi_semaphore_wait(&rsi_http_response_sem, 0);
#endif
  *rsp_var = 0;
  if (*rsp_var != RSI_HTTP_OTAF_UPGRADE_SUCCESS) {
    return *rsp_var;
  } else {
    return RSI_SUCCESS;
  }
}

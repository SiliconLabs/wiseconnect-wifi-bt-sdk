/*******************************************************************************
* @file  rsi_http_client.c
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
#include "rsi_index_based_ssl_client_sockets.h"
#include <stdlib.h>

//! include the index html page
#include "index.txt"

//! Global Variables
//! data buffer length
uint32_t buf_len;

//! End of http put file/content
volatile uint8_t end_of_file;

//!data offset
uint32_t offset;

//! http receive status
volatile int32_t http_recv_status;

//! Application Buffer
uint8_t app_buf[APP_BUFF_LEN];

//! Memory to initialize driver
uint8_t global_buf[GLOBAL_BUFF_LEN];

uint8_t cacert_buf[CACERT_BUF_SIZE];
uint32_t cacert_len = 0;

//! Http client task
int32_t rsi_http_client_task()
{
  int32_t status = RSI_SUCCESS;
  uint16_t flags = HTTP_INDEX0_FLAGS;

  //! resetting the variables
  rsi_http_reset_handler();

  //! send http get request for the given url
  status = rsi_http_client_get_async((uint16_t)flags,
                                     (uint8_t *)SERVER_IP_ADDRESS,
                                     (uint16_t)HTTP_SERVER_PORT1,
                                     (uint8_t *)HTTP_SERVER_URL1,
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
  rsi_http_client_abort();

  //! resetting the variables
  rsi_http_reset_handler();

  return status;
}

//! Http client firmware listing task
int32_t rsi_http_client_fw_listing_task()
{
  int32_t status = RSI_SUCCESS;
  uint16_t flags = HTTP_INDEX2_FLAGS;

  uint32_t chunk_length = 0;
  uint8_t *file_content = NULL;

  //! Get the length of the index html page
  uint32_t file_size = (sizeof(rsi_index) - 1);

  //! resetting the variables
  rsi_http_reset_handler();

  //! Create HTTP client for PUT method
  status = rsi_http_client_put_create();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nCreate HTTP client Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nCreate HTTP client Success\r\n");
  }

#if HTTPS_CIPHERS_BITMAP_SUPPORT
  if ((flags & HTTPS_SUPPORT) && (flags & RSI_HTTPS_CIPHERS_BITMAP)) {
    cipher_value = CIPHER_VALUE;

    status = rsi_network_app_protocol_config(RSI_HTTP, RSI_CIPHER_SELECTION, &cipher_value, sizeof(cipher_value));
    if (status != RSI_SUCCESS) {
      LOG_PRINT("\r\nNetwork app protocol config Failed, Error Code : 0x%lX\r\n", status);
      return status;
    } else {
      LOG_PRINT("\r\nNetwork app protocol config Success\r\n");
    }
  }
#endif

  //! Start HTTP client PUT method for the given URL
  status = rsi_http_client_put_start((uint16_t)flags,
                                     (uint8_t *)SERVER_IP_ADDRESS,
                                     (uint32_t)HTTP_SERVER_PORT,
                                     (uint8_t *)HTTP_SERVER_URL,
                                     (uint8_t *)HTTP_HOSTNAME,
                                     (uint8_t *)HTTP_EXTENDED_HEADER,
                                     (uint8_t *)USERNAME,
                                     (uint8_t *)PASSWORD,
                                     (uint32_t)(sizeof(rsi_index) - 1),
                                     rsi_http_client_put_response_handler);

  //! wait for the success response
  status = rsi_http_response_status((int32_t *)&rsp_received);
  if (status != RSI_SUCCESS) {
    return status;
  }

  //! HTTP client PUT packet
  while (!end_of_file) {
    //! Get the current length that you want to send
    chunk_length = ((file_size - offset) > MAX_HTTP_CLIENT_PUT_BUFFER_LENGTH) ? MAX_HTTP_CLIENT_PUT_BUFFER_LENGTH
                                                                              : (file_size - offset);

    file_content = (uint8_t *)(rsi_index + offset);

    //! Send resource content to the HTTP server for the given URL and total content length
    status = rsi_http_client_put_pkt((uint8_t *)file_content, (uint16_t)chunk_length);

    //! wait for the success response
    status = rsi_http_response_status((int32_t *)&rsp_received);
    if (status != RSI_SUCCESS) {
      return status;
    }

    //! Move content offset by chunk length
    offset += chunk_length;

//! Enable this macro if Webpage content is returned as HTTP response
#if WEBPAGE_AS_HTTP_PUT_RESPONSE
    //! wait for the webpage success as response
    status = rsi_http_response_status(&http_recv_status);
    if (status != RSI_SUCCESS) {
      return status;
    }
#endif
  }

  //! Delete HTTP Client
  status = rsi_http_client_put_delete();
  if (status != RSI_SUCCESS) {
    LOG_PRINT("\r\nDelete HTTP PUT Client Failed, Error Code : 0x%lX\r\n", status);
    return status;
  } else {
    LOG_PRINT("\r\nDelete HTTP PUT Client Success\r\n");
  }

  //! wait for the success response
  status = rsi_http_response_status((int32_t *)&rsp_received);
  if (status != RSI_SUCCESS) {
    return status;
  }
  rsi_http_client_abort();

  //! resetting the variables
  rsi_http_reset_handler();

  return status;
}

//! http put response notify call back handler
void rsi_http_client_put_response_handler(uint16_t status,
                                          const uint8_t type,
                                          const uint8_t *buffer,
                                          uint16_t length,
                                          const uint8_t end_of_put_pkt)
{
  if (status == RSI_SUCCESS) {
    if (type == HTTP_CLIENT_PUT_OFFSET_PKT) {
      if (buf_len + length > APP_BUFF_LEN) {
        //! Application buffer over flowed
        return;
      }
      if (!(end_of_put_pkt & RSI_HTTP_END_OF_DATA)) {
        //! copy received data to application buffer
        memcpy(app_buf + buf_len, buffer, length);

        //! update buffer length
        buf_len += length;
      } else {
        if (length) {
          //! copy received data to application buffer
          memcpy(app_buf + buf_len, buffer, length);

          //! update buffer length
          buf_len += length;
        }

        //! set to exit from driver task
        http_recv_status = RSI_HTTP_CLIENT_SUCCESS;
      }
    } else {
      //! Set success response
      rsp_received = RSI_HTTP_CLIENT_SUCCESS;

      if (type == HTTP_CLIENT_PUT_PKT) {
        //! Get end of file/resource
        end_of_file = end_of_put_pkt;
      } else {
        //! resetting the variables
        rsi_http_reset_handler();
      }
    }
  } else {
    //! resetting the variables
    rsi_http_reset_handler();
  }
}

//! http get/post response notify call back handler
#if RSI_HTTP_STATUS_INDICATION_EN
void rsi_http_response_handler(uint16_t status,
                               const uint8_t *buffer,
                               const uint16_t length,
                               const uint32_t moredata,
                               uint16_t status_code)
#else
void rsi_http_response_handler(uint16_t status, const uint8_t *buffer, const uint16_t length, const uint32_t moredata)
#endif
{
  http_response_t response;

#if RSI_HTTP_STATUS_INDICATION_EN
  if (count == 1) {
    LOG_PRINT("\r\nHTTP response Handler ,Status code :%ld\r\n", status_code);
    count++;
  }
#endif

  response.output_data = (uint8_t *)malloc(sizeof(length) + 1);
  if (response.output_data == NULL) {
    LOG_PRINT("\r\nMemory_allocation failed\r\n");
    return;
  }
  memcpy(response.output_data, buffer, length);
  response.output_len = length;
  output_values(response);

  if (status == RSI_SUCCESS) {
    if (buf_len + length > APP_BUFF_LEN) {
      //! Application buffer over flowed
      return;
    }
    if (!moredata) {
      //! copy received data to application buffer
      memcpy(app_buf + buf_len, buffer, length);

      //! update buffer length
      buf_len += length;
    } else {
      if (length) {
        //! copy received data to application buffer
        memcpy(app_buf + buf_len, buffer, length);

        //! update buffer length
        buf_len += length;
      }
      //! set to exit from driver task
      http_recv_status = RSI_HTTP_CLIENT_SUCCESS;
    }
  } else {
    //!trying to connect non existing TCP server socket
    http_recv_status = RSI_HTTP_CLIENT_SUCCESS;
    LOG_PRINT("\r\n  , Error Code : 0x%lX\r\n", (long unsigned int)status);
    rsi_http_reset_handler();
  }
}

int32_t rsi_http_response_status(int32_t *rsp_var)
{
  //! wait for the success response
#ifndef RSI_WITH_OS
  do {
    //! event loop
    rsi_wireless_driver_task();
  } while (!(*rsp_var));
#endif
  *rsp_var = 0;
  if (*rsp_var != RSI_HTTP_CLIENT_SUCCESS) {
    return rsi_wlan_get_nwk_status();
  } else {
    return RSI_SUCCESS;
  }
}

void rsi_http_reset_handler(void)
{
  buf_len     = 0;
  end_of_file = 0;
  offset      = 0;
#if RSI_HTTP_STATUS_INDICATION_EN
  count = 1;
#endif
  return;
}

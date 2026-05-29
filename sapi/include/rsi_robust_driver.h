/*******************************************************************************
 * @file  rsi_robust_driver.h
 * @brief CRC-32 Integrity (Driver-R) layer for RS9116 SAPI - UART
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 ******************************************************************************/

#ifndef RSI_ROBUST_DRIVER_H
#define RSI_ROBUST_DRIVER_H

#include <stdint.h>
#include "rsi_timer.h"

#ifdef RSI_ROBUST_MODE_ENABLE
#ifdef RSI_UART_INTERFACE

#define RSI_ROBUST_MAX_RETRIES      5
#define RSI_ROBUST_ACK_TIMEOUT_MS   10000
#define RSI_ROBUST_OVERHEAD_BYTES   8
#define RSI_ROBUST_CTRL_FRAME_LEN   24
#define RSI_ROBUST_MAX_TX_FRAME_LEN 1600

#define RSI_CRC32_PRESET        0xFFFFFFFFUL
#define RSI_CRC32_DMA_THRESHOLD 100
#define RSI_CRC32_LDMA_CHANNEL  0

#define RSI_ROBUST_STATE_DISABLED 0x00
#define RSI_ROBUST_STATE_RST_SENT 0x01
#define RSI_ROBUST_STATE_ACTIVE   0x02
#define RSI_ROBUST_STATE_WAIT_ACK 0x03
#define RSI_ROBUST_STATE_ERROR    0x04

#define RSI_ROBUST_FRAME_ACK  BIT(2)
#define RSI_ROBUST_FRAME_NACK BIT(3)

typedef struct rsi_robust_drv_cb_s {
  uint8_t enabled;
  uint8_t state;
  uint8_t ack_status;
  uint16_t tx_sn;
  uint16_t rx_sn;
  uint8_t retry_count;
  uint8_t max_retries;
  uint32_t ack_timeout_ms;
  uint8_t ctrl_frame_buf[RSI_ROBUST_CTRL_FRAME_LEN];
  uint8_t tx_frame_buf[RSI_ROBUST_MAX_TX_FRAME_LEN];
  uint16_t tx_frame_len;
  rsi_timer_instance_t ack_timer;
} rsi_robust_drv_cb_t;

#define RSI_ERROR_ROBUST_HANDSHAKE_FAILED ((int16_t)0x0081)
#define RSI_ERROR_ROBUST_MAX_RETRIES      ((int16_t)0x0082)
#define RSI_ERROR_ROBUST_CRC_FAILED       ((int16_t)0x0083)
#define RSI_ERROR_ROBUST_SN_MISMATCH      ((int16_t)0x0084)

void rsi_robust_gpcrc_ldma_init(void);
void rsi_robust_drv_init(rsi_robust_drv_cb_t *cb);
int16_t rsi_robust_drv_recv_frame(uint8_t *frame, uint16_t length);
void rsi_robust_drv_process_ctrl_frame(uint8_t *buf);
int16_t rsi_robust_drv_send(uint8_t *buf_ptr, uint16_t payload_length);
int16_t rsi_robust_drv_check_ack(void);
uint32_t rsi_robust_compute_crc32(uint8_t *data, uint32_t length);
uint8_t rsi_robust_compute_crc8(const uint8_t *data, uint16_t length);

#endif /* RSI_UART_INTERFACE */
#endif /* RSI_ROBUST_MODE_ENABLE */

#endif /* RSI_ROBUST_DRIVER_H */

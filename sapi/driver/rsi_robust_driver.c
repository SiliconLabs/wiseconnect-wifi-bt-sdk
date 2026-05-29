/*******************************************************************************
 * @file  rsi_robust_driver.c
 * @brief CRC-32 Integrity (Driver-R) implementation for RS9116 SAPI UART
 *******************************************************************************
 * # License
 * <b>Copyright 2020 Silicon Laboratories Inc. www.silabs.com</b>
 ******************************************************************************/

#include "rsi_driver.h"

#ifdef RSI_ROBUST_MODE_ENABLE
#include "rsi_robust_driver.h"
#include "em_device.h"
#include "em_gpcrc.h"
#include "em_cmu.h"
#include <string.h>

#define RSI_ROBUST_CTRL_SN_OFF     19
#define RSI_ROBUST_CTRL_BITMAP_OFF 21
#define RSI_ROBUST_CTRL_CRC8_OFF   23

void rsi_robust_gpcrc_ldma_init(void)
{
  CMU_ClockEnable(cmuClock_GPCRC, true);
  GPCRC_Init_TypeDef gpcrc_init = GPCRC_INIT_DEFAULT;
  gpcrc_init.initValue          = 0;
  gpcrc_init.autoInit           = true;
  gpcrc_init.reverseByteOrder   = false;
  GPCRC_Init(GPCRC, &gpcrc_init);
}

static uint32_t rsi_compute_crc32_byte(uint8_t *data, uint32_t length)
{
  uint32_t i;

  if (length == 0) {
    return 0;
  }

  GPCRC_Start(GPCRC);
  for (i = 0; i < length; i++) {
    GPCRC_InputU8(GPCRC, data[i]);
  }
  return GPCRC_DataRead(GPCRC);
}

uint32_t rsi_robust_compute_crc32(uint8_t *data, uint32_t length)
{
  /* Byte path matches RS9116 firmware CRC over the same UART octet stream. DMA+reverseByteOrder does not. */
  return rsi_compute_crc32_byte(data, length);
}

static uint16_t rsi_robust_drv_append_sn_crc(uint8_t *buf_ptr, uint16_t payload_length)
{
  rsi_robust_drv_cb_t *cb = rsi_driver_cb_non_rom->robust_drv_cb;

  if (cb == NULL || !cb->enabled || cb->state != RSI_ROBUST_STATE_ACTIVE) {
    return payload_length;
  }

  uint16_t frame_len = (uint16_t)(payload_length + RSI_HOST_DESC_LENGTH);
  uint16_t new_payload;
  uint16_t sn = cb->tx_sn;
  uint32_t crc;

  uint8_t padding_needed_len = (uint8_t)((4U - (payload_length % 4U)) % 4U);
  new_payload                = (uint16_t)(payload_length + padding_needed_len + RSI_ROBUST_OVERHEAD_BYTES);
  *(uint16_t *)buf_ptr       = (*(uint16_t *)buf_ptr & 0xF000) | ((new_payload - padding_needed_len) & 0x0FFF);

  while (padding_needed_len != 0U) {
    buf_ptr[frame_len++] = 0x00;
    padding_needed_len--;
  }
  buf_ptr[frame_len]     = 0x00;
  buf_ptr[frame_len + 1] = 0x00;

  buf_ptr[frame_len + 2] = (uint8_t)(sn & 0xFF);
  buf_ptr[frame_len + 3] = (uint8_t)(sn >> 8);

  crc                    = rsi_robust_compute_crc32(buf_ptr, (uint32_t)frame_len + 4U);
  buf_ptr[frame_len + 4] = (uint8_t)(crc & 0xFF);
  buf_ptr[frame_len + 5] = (uint8_t)((crc >> 8) & 0xFF);
  buf_ptr[frame_len + 6] = (uint8_t)((crc >> 16) & 0xFF);
  buf_ptr[frame_len + 7] = (uint8_t)((crc >> 24) & 0xFF);

  return new_payload;
}

int16_t rsi_robust_drv_send(uint8_t *buf_ptr, uint16_t payload_length)
{
  rsi_robust_drv_cb_t *cb = rsi_driver_cb_non_rom->robust_drv_cb;
  int16_t status;

  if (cb == NULL || !cb->enabled || cb->state != RSI_ROBUST_STATE_ACTIVE) {
    return rsi_frame_write((rsi_frame_desc_t *)buf_ptr, &buf_ptr[RSI_HOST_DESC_LENGTH], payload_length);
  }

  uint16_t new_length = rsi_robust_drv_append_sn_crc(buf_ptr, payload_length);
  uint16_t total_len  = new_length + RSI_HOST_DESC_LENGTH;

  memcpy(cb->tx_frame_buf, buf_ptr, total_len);
  cb->tx_frame_len = new_length;
  cb->retry_count  = 0;
  cb->ack_status   = 0;

  status = rsi_frame_write((rsi_frame_desc_t *)buf_ptr, &buf_ptr[RSI_HOST_DESC_LENGTH], new_length);
  if (status < 0) {
    cb->state = RSI_ROBUST_STATE_ERROR;
    return status;
  }

  cb->state = RSI_ROBUST_STATE_WAIT_ACK;
  rsi_init_timer(&cb->ack_timer, cb->ack_timeout_ms);
  return RSI_SUCCESS;
}

int16_t rsi_robust_drv_check_ack(void)
{
  rsi_robust_drv_cb_t *cb = rsi_driver_cb_non_rom->robust_drv_cb;
  int16_t status;

  if (cb == NULL || cb->state != RSI_ROBUST_STATE_WAIT_ACK) {
    return RSI_SUCCESS;
  }

  if (cb->ack_status == RSI_ROBUST_FRAME_ACK) {
    cb->tx_sn++;
    cb->state = RSI_ROBUST_STATE_ACTIVE;
    return RSI_SUCCESS;
  }

  if (cb->ack_status == RSI_ROBUST_FRAME_NACK || rsi_timer_expired(&cb->ack_timer)) {
    cb->retry_count++;
    if (cb->retry_count > cb->max_retries) {
      cb->state = RSI_ROBUST_STATE_ERROR;
      rsi_error_timeout_and_clear_events(RSI_ERROR_ROBUST_MAX_RETRIES, TX_EVENT_CMD);
      return RSI_ERROR_ROBUST_MAX_RETRIES;
    }

    cb->ack_status = 0;

    status =
      rsi_frame_write((rsi_frame_desc_t *)cb->tx_frame_buf, &cb->tx_frame_buf[RSI_HOST_DESC_LENGTH], cb->tx_frame_len);
    if (status < 0) {
      cb->state = RSI_ROBUST_STATE_ERROR;
      return status;
    }
    rsi_init_timer(&cb->ack_timer, cb->ack_timeout_ms);
  }
  if (rsi_driver_cb->scheduler_cb.event_map & ~(BIT(RSI_TX_EVENT))) {
    // masking the tx event
    rsi_mask_event(RSI_TX_EVENT);
    // lock the mutex
    rsi_mutex_lock(&rsi_driver_cb_non_rom->tx_mutex);
    rsi_driver_cb_non_rom->tx_mask_event = 1;
    // unlock mutex
    rsi_mutex_unlock(&rsi_driver_cb_non_rom->tx_mutex);
  }

  return RSI_ROBUST_STATE_WAIT_ACK;
}

uint8_t rsi_robust_compute_crc8(const uint8_t *data, uint16_t length)
{
  uint8_t crc = 0x00;
  uint16_t i;
  uint8_t j;
  for (i = 0; i < length; i++) {
    crc ^= data[i];
    for (j = 0; j < 8; j++) {
      if (crc & 0x80) {
        crc = (crc << 1) ^ 0x07;
      } else {
        crc = crc << 1;
      }
    }
  }
  return crc;
}

void rsi_robust_drv_init(rsi_robust_drv_cb_t *cb)
{
  if (cb == NULL)
    return;
  memset(cb, 0, sizeof(rsi_robust_drv_cb_t));
  cb->max_retries    = RSI_ROBUST_MAX_RETRIES;
  cb->ack_timeout_ms = RSI_ROBUST_ACK_TIMEOUT_MS;
  cb->state          = RSI_ROBUST_STATE_DISABLED;
}

static int16_t send_ctrl_frame(uint16_t sn, uint16_t bitmap)
{
  rsi_robust_drv_cb_t *cb = rsi_driver_cb_non_rom->robust_drv_cb;
  uint16_t payload_len    = RSI_ROBUST_CTRL_FRAME_LEN - RSI_HOST_DESC_LENGTH;

  memset(cb->ctrl_frame_buf, 0, RSI_ROBUST_CTRL_FRAME_LEN);

  cb->ctrl_frame_buf[0] = (uint8_t)(payload_len & 0xFF);
  cb->ctrl_frame_buf[1] = (uint8_t)(((payload_len >> 8) & 0x0F) | (RSI_WLAN_MGMT_Q << 4));
  cb->ctrl_frame_buf[2] = RSI_COMMON_REQ_ROBUST_HANDSHAKE;

  cb->ctrl_frame_buf[RSI_ROBUST_CTRL_SN_OFF]         = (uint8_t)(sn & 0xFF);
  cb->ctrl_frame_buf[RSI_ROBUST_CTRL_SN_OFF + 1]     = (uint8_t)(sn >> 8);
  cb->ctrl_frame_buf[RSI_ROBUST_CTRL_BITMAP_OFF]     = (uint8_t)(bitmap & 0xFF);
  cb->ctrl_frame_buf[RSI_ROBUST_CTRL_BITMAP_OFF + 1] = (uint8_t)(bitmap >> 8);
  cb->ctrl_frame_buf[RSI_ROBUST_CTRL_CRC8_OFF] = rsi_robust_compute_crc8(cb->ctrl_frame_buf, RSI_ROBUST_CTRL_CRC8_OFF);

  return rsi_frame_write((rsi_frame_desc_t *)cb->ctrl_frame_buf,
                         &cb->ctrl_frame_buf[RSI_HOST_DESC_LENGTH],
                         payload_len);
}

void rsi_robust_drv_process_ctrl_frame(uint8_t *buf)
{
  rsi_robust_drv_cb_t *cb = rsi_driver_cb_non_rom->robust_drv_cb;
  if (!cb->enabled)
    return;

  uint16_t bitmap = (uint16_t)buf[RSI_ROBUST_CTRL_BITMAP_OFF] | ((uint16_t)buf[RSI_ROBUST_CTRL_BITMAP_OFF + 1] << 8);
  if (bitmap != RSI_ROBUST_FRAME_ACK && bitmap != RSI_ROBUST_FRAME_NACK) {
    return;
  }

  uint8_t crc_recv = buf[RSI_ROBUST_CTRL_CRC8_OFF];
  uint8_t crc_calc = rsi_robust_compute_crc8(buf, RSI_ROBUST_CTRL_CRC8_OFF);
  if (crc_recv != crc_calc) {
    if (cb->state == RSI_ROBUST_STATE_WAIT_ACK) {
      cb->ack_status = RSI_ROBUST_FRAME_NACK;
      if (rsi_driver_cb_non_rom->tx_mask_event == 1) {
        rsi_unmask_event(RSI_TX_EVENT);
        // lock the mutex
        rsi_mutex_lock(&rsi_driver_cb_non_rom->tx_mutex);
        rsi_driver_cb_non_rom->tx_mask_event = 0;
        // unlock mutex
        rsi_mutex_unlock(&rsi_driver_cb_non_rom->tx_mutex);
      }
    }
    return;
  }

  uint16_t sn = (uint16_t)buf[RSI_ROBUST_CTRL_SN_OFF] | ((uint16_t)buf[RSI_ROBUST_CTRL_SN_OFF + 1] << 8);
  if (cb->state == RSI_ROBUST_STATE_WAIT_ACK) {
    if (bitmap == RSI_ROBUST_FRAME_ACK && sn == cb->tx_sn) {
      cb->ack_status = RSI_ROBUST_FRAME_ACK;
    } else {
      cb->ack_status = RSI_ROBUST_FRAME_NACK;
    }
    if (rsi_driver_cb_non_rom->tx_mask_event == 1) {
      rsi_unmask_event(RSI_TX_EVENT);
      // lock the mutex
      rsi_mutex_lock(&rsi_driver_cb_non_rom->tx_mutex);
      rsi_driver_cb_non_rom->tx_mask_event = 0;
      // unlock mutex
      rsi_mutex_unlock(&rsi_driver_cb_non_rom->tx_mutex);
    }
  }
}

int16_t rsi_robust_drv_recv_frame(uint8_t *frame, uint16_t length)
{
  rsi_robust_drv_cb_t *cb = rsi_driver_cb_non_rom->robust_drv_cb;
  length                  = ((length + 3) & ~3);
  if (length < RSI_HOST_DESC_LENGTH + RSI_ROBUST_OVERHEAD_BYTES)
    return RSI_ERROR_ROBUST_CRC_FAILED;

  uint16_t sn       = (uint16_t)frame[length - 6] | ((uint16_t)frame[length - 5] << 8);
  uint32_t crc_recv = (uint32_t)frame[length - 4] | ((uint32_t)frame[length - 3] << 8)
                      | ((uint32_t)frame[length - 2] << 16) | ((uint32_t)frame[length - 1] << 24);

  {
    uint32_t crc_calc = rsi_robust_compute_crc32(frame, length - 4U);
    if (crc_calc != crc_recv) {
      send_ctrl_frame(sn, RSI_ROBUST_FRAME_NACK);
      return RSI_ERROR_ROBUST_CRC_FAILED;
    }
  }
  if (sn != cb->rx_sn) {
    if (sn < cb->rx_sn) {
      send_ctrl_frame(sn, RSI_ROBUST_FRAME_ACK);
    } else {
      send_ctrl_frame(sn, RSI_ROBUST_FRAME_NACK);
    }
    return RSI_ERROR_ROBUST_SN_MISMATCH;
  }
  cb->rx_sn++;
  send_ctrl_frame(sn, RSI_ROBUST_FRAME_ACK);

  uint16_t orig_len  = (*(uint16_t *)frame & 0x0FFF) - RSI_ROBUST_OVERHEAD_BYTES;
  *(uint16_t *)frame = (*(uint16_t *)frame & 0xF000) | (orig_len & 0x0FFF);

  return 0;
}

#endif /* RSI_ROBUST_MODE_ENABLE */

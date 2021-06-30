/*******************************************************************************
* @file  rsi_crypto.c
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
#include "rsi_crypto.h"

#ifdef RSI_CRYPTO_ENABLE

/** @addtogroup CRYPTO
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported in current release.
 * @brief      Compute the SHA digest. This is a blocking API.
 * @param[in]  sha_mode     1 – For SHA1 \n 2 – For SHA256 \n 3 – For SHA384 \n 4 – For SHA512 \n
 * @param[in]  msg          - Pointer to message
 * @param[in]  msg_length   - Total message length
 * @param[in]  chunk_length - Current chunk length
 * @param[in]  pending_flag - BIT(0) - 1st chunk \n BIT(1) - Middle chunk \n BIT(2) - Last chunk \n
 * @param[out]  digest      - Output parameter to hold computed digest from SHA
 * @return	   0              - Success \n
 *             Non-Zero Value - Failure
 * @note Refer Error Codes section for above error codes \ref error-codes. 
 *
 */

int32_t rsi_sha_pen(uint8_t sha_mode,
                    uint8_t *msg,
                    uint16_t msg_length,
                    uint16_t chunk_len,
                    uint8_t pending_flag,
                    uint8_t *digest)
{

  int32_t status     = RSI_SUCCESS;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  uint8_t digest_len = 0;
  rsi_pkt_t *pkt;
  rsi_sha_req_t *sha;

  // Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  // Input pointer check
  if (msg == NULL) {
    return RSI_ERROR_INVALID_PARAM;
  }
  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Fill digest length based on sha mode
    if (sha_mode == SHA_1) {
      digest_len = SHA_1_DIGEST_LEN;
    } else if (sha_mode == SHA_256) {
      digest_len = SHA_256_DIGEST_LEN;
    } else if (sha_mode == SHA_384) {
      digest_len = SHA_384_DIGEST_LEN;
    } else if (sha_mode == SHA_512) {
      digest_len = SHA_512_DIGEST_LEN;
    } else {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      return RSI_ERROR_INVALID_PARAM;
    }

    // Allocate command buffer from wlan pool
    pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      //Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    if (digest != NULL) {
      // Attach the buffer given by user
      rsi_common_cb->app_buffer = digest;

      // Length of the buffer provided by user
      rsi_common_cb->app_buffer_length = digest_len;
    }

    // Get Data Pointer
    sha = (rsi_sha_req_t *)pkt->data;

    // Memset before filling
    memset(sha, 0, sizeof(rsi_sha_req_t));

    // Fill Algorithm type SHA - 4
    sha->algorithm_type = SHA;

    sha->algorithm_sub_type = sha_mode;

    // Fill sha_flags BIT(0) - 1st chunk BIT(2) - Last chunk
    sha->sha_flags = pending_flag;

    // Fill total msg length
    sha->total_msg_length = msg_length;

    // Fill current chunk length
    sha->current_chunk_length = chunk_len;

    // Data
    // Memset before filling
    memset(&sha->msg[0], 0, MAX_DATA_SIZE_BYTES);

    // Copy Data
    memcpy(&sha->msg[0], msg, chunk_len);

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_sha_req_t) - MAX_DATA_SIZE_BYTES + chunk_len;

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send sha digest computation request to module
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_ENCRYPT_CRYPTO, pkt);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_CRYPTO_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    return status;
  }

  // Get common command response stattus
  status = rsi_common_get_status();

  // Return the status
  return status;
}

/*==============================================*/
/**
 * @note       This API is not supported in current release.
 * @brief      Decide whether the SHA message can be sent once or requires multiple calls to send. This is a blocking API.
 * @param[in]  sha_mode   1 – For SHA1 \n 2 – For SHA256 \n 3 – For SHA384 \n 4 – For SHA512 \n
 * @param[in]  msg        - Pointer to message
 * @param[in]  msg_length - Total message length
 * @param[out]  digest    - Output parameter to hold computed digest from SHA
 * @return	   0              - Success \n
 *             Non-Zero Value - Failure
 *
 * @note Refer Error Codes section for above error codes \ref error-codes. 
 *
 */

int32_t rsi_sha(uint8_t sha_mode, uint8_t *msg, uint16_t msg_length, uint8_t *digest)
{
  int32_t status     = RSI_SUCCESS;
  uint16_t total_len = 0;
  uint16_t chunk_len = 0;
  uint16_t offset    = 0;
  uint8_t sha_flags  = 0;

  total_len = msg_length;

  while (total_len) {
    // Check total length
    if (total_len > MAX_DATA_SIZE_BYTES) {
      chunk_len = MAX_DATA_SIZE_BYTES;
      if (offset == 0) {
        // Make sha_flag as first chunk
        sha_flags |= FIRST_CHUNK;
      } else {
        // Make sha_flag as Middle chunk
        sha_flags = MIDDLE_CHUNK;
      }
    } else {
      chunk_len = total_len;

      // Make sha_flag as Last chunk
      sha_flags = LAST_CHUNK;
      if (offset == 0) {
        /* If the total length is less than 1400 and offset is zero,
					 make sha_flag as both first chunk as well as last chunk*/
        sha_flags |= FIRST_CHUNK;
      }
    }

    // Send the current chunk length message
    status = rsi_sha_pen(sha_mode, msg, msg_length, chunk_len, sha_flags, digest);

    if (status != RSI_SUCCESS) {
      return status;
    }

    // Increment the offset value
    offset += chunk_len;
    msg += chunk_len;

    // Decrement the total message lenth
    total_len -= chunk_len;
  }

  return status;
}

/*==============================================*/
/**
 * @note       This API is not supported in current release.
 * @brief      Hold computed digest from HMAC-SHA. This is a blocking API.
 * @param[in]  hmac_sha_mode - 1 – For HMAC-SHA1 \n 2 – For HMAC-SHA256 \n 3 – For HMAC-SHA384 \n 4 – For HMAC-SHA512 \n
 * @param[in]  data          - Pointer to key along with message
 * @param[in]  total_length  - Total message length
 * @param[in]  chunk_length  - Current chunk length
 * @param[in]  pending_flag  - BIT(0) - 1st chunk \n BIT(1) - Middle chunk \n BIT(2) - Last chunk \n
 * @param[out] digest        - Output parameter to hold computed digest from HMAC-SHA
 * @return	   0              - Success \n
 *             Non-Zero Value - Failure
 *
 * @note Refer Error Codes section for above error codes \ref error-codes. 
 */

int32_t rsi_hmac_sha_pen(uint8_t hmac_sha_mode,
                         uint8_t *data,
                         uint16_t total_length,
                         uint16_t chunk_length,
                         uint32_t key_length,
                         uint8_t pending_flag,
                         uint8_t *digest)
{

  int32_t status     = RSI_SUCCESS;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  uint8_t digest_len = 0;
  rsi_pkt_t *pkt;
  rsi_hmac_sha_req_t *hmac_sha;

  // Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  // Input pointer check
  if (data == NULL) {
    return RSI_ERROR_INVALID_PARAM;
  }

  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Fill digest length based on sha mode
    if (hmac_sha_mode == SHA_1) {
      digest_len = SHA_1_DIGEST_LEN;
    } else if (hmac_sha_mode == SHA_256) {
      digest_len = SHA_256_DIGEST_LEN;
    } else if (hmac_sha_mode == SHA_384) {
      digest_len = SHA_384_DIGEST_LEN;
    } else if (hmac_sha_mode == SHA_512) {
      digest_len = SHA_512_DIGEST_LEN;
    } else {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      return RSI_ERROR_INVALID_PARAM;
    }
    // Allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    if (digest != NULL) {
      // Attach the buffer given by user
      rsi_common_cb->app_buffer = digest;

      // Length of the buffer provided by user
      rsi_common_cb->app_buffer_length = digest_len;
    }

    // Get Data Pointer
    hmac_sha = (rsi_hmac_sha_req_t *)pkt->data;

    // Memset before filling
    memset(hmac_sha, 0, sizeof(rsi_hmac_sha_req_t));

    // Fill Algorithm type HMAC_SHA - 5
    hmac_sha->algorithm_type = HMAC_SHA;

    // Fill Algorithm sub type
    hmac_sha->algorithm_sub_type = hmac_sha_mode;

    // Fill sha_flags BIT(0) - 1st chunk BIT(2) - Last chunk
    hmac_sha->hmac_sha_flags = pending_flag;

    // Fill total msg length
    hmac_sha->total_msg_length = total_length;

    // Fill current chunk length
    hmac_sha->current_chunk_length = chunk_length;

    // Fill key length
    hmac_sha->key_length = key_length;

    // Data
    // Memset before filling
    memset(&hmac_sha->hmac_data[0], 0, MAX_DATA_SIZE_BYTES);

    // Copy Data
    memcpy(&hmac_sha->hmac_data[0], data, chunk_length);

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_hmac_sha_req_t) - MAX_DATA_SIZE_BYTES + chunk_length;

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send hmac sha digest computation request to module
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_ENCRYPT_CRYPTO, pkt);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_CRYPTO_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    return status;
  }

  // Get common command response stattus
  status = rsi_common_get_status();

  // Return the status
  return status;
}

/** @} */
/** @addtogroup CRYPTO
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported in current release.
 * @brief      Decide whether the HMAC-SHA message can be sent once or requires multiple calls to send the message. This is a blocking API.
 * @param[in]  hmac_sha_mode - 1 – For HMAC-SHA1 \n 2 – For HMAC-SHA256 \n 3 – For HMAC-SHA384 \n 4 – For HMAC-SHA512 \n
 * @param[in]  msg           - Pointer to message
 * @param[in]  msg_length    - Total message length
 * @param[in]  key           - Pointer to HMAC key
 * @param[in]  key_length    - HMAC key length in bytes
 * @param[out]  digest       - Output parameter to hold computed digest from HMAC-SHA
 * @return	   0          - Success   \n 
 *             Non-Zero Value - If return value is lesser than 0 \n
 *                              -2: Invalid parameters \n
 *                              -3: Command given in wrong state \n
 *                              -4: Buffer not available to serve the command \n
 *                              If return value is greater than 0 \n
 *                              0xFF15, 0xCC9C, 0xCC9B
 *
 * @note Refer Error Codes section for above error codes \ref error-codes. 
 *
 *
 */

int32_t rsi_hmac_sha(uint8_t hmac_sha_mode,
                     uint8_t *msg,
                     uint32_t msg_length,
                     uint8_t *key,
                     uint32_t key_length,
                     uint8_t *digest,
                     uint8_t *hmac_buffer)
{
  int32_t status         = RSI_SUCCESS;
  uint32_t total_len     = 0;
  uint16_t chunk_len     = 0;
  uint16_t offset        = 0;
  uint8_t hmac_sha_flags = 0;
  uint8_t *data          = NULL;
  data                   = hmac_buffer;
  total_len              = msg_length + key_length;

  // Copy key in to data
  memcpy(data, key, key_length);

  // Copy message in to data
  memcpy((data + key_length), msg, msg_length);

  while (total_len) {

    // Check total length
    if (total_len > MAX_DATA_SIZE_BYTES) {
      chunk_len = MAX_DATA_SIZE_BYTES;
      if (offset == 0) {
        // Make hmac_sha_flag as first chunk
        hmac_sha_flags |= FIRST_CHUNK;
      } else {
        // Make hmac_sha_flag as Middle chunk
        hmac_sha_flags = MIDDLE_CHUNK;
      }
    } else {
      chunk_len = total_len;

      // Make hmac_sha_flag as Last chunk
      hmac_sha_flags = LAST_CHUNK;
      if (offset == 0) {
        /* If the total length is less than 1400 and offset is zero,
					 make hmac_sha_flag as both first chunk as well as last chunk*/
        hmac_sha_flags |= FIRST_CHUNK;
      }
    }

    // Send the current chunk length message
    status =
      rsi_hmac_sha_pen(hmac_sha_mode, data, (msg_length + key_length), chunk_len, key_length, hmac_sha_flags, digest);

    if (status != RSI_SUCCESS) {
      return status;
    }

    // Increment the offset value
    offset += chunk_len;
    data += chunk_len;

    // Decrement the total message lenth
    total_len -= chunk_len;
  }

  return status;
}

/** @} */

/** @addtogroup CRYPTO
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported in current release.
 * @brief      Decide whether the AES message can be sent once or requires multiple calls to send the message. This is a blocking API.
 * @param[in]  aes_mode   - 1 – For AES CBC mode \n 2 – For AES ECB mode \n 3 – For AES CTR mode \n
 * @param[in]  enc_dec    - 1 – For AES Encryption \n 2 – For AES Decryption \n
 * @param[in]  msg        - Pointer to message
 * @param[in]  msg_length - Total message length
 * @param[in]  key        - Pointer to AES key
 * @param[in]  key_length - AES key length in bytes
 * @param[in]  iv         - Pointer to AES iv
 * @param[out]  output    - Output parameter to hold encrypted/decrypted from AES
 * @return		0              - Success   \n
 *              Non-Zero Value - If return value is lesser than 0 \n
 *                              -2: Invalid parameters \n
 *                              -3: Command given in wrong state \n
 *                              -4: Buffer not available to serve the command \n
 *                              If return value is greater than 0 \n
 *                              0xFF15, 0xCC9C, 0xCC9B
 *
 * @note Refer Error Codes section for above error codes \ref error-codes. 
 *
 */

int32_t rsi_aes(uint16_t aes_mode,
                uint16_t enc_dec,
                uint8_t *msg,
                uint16_t msg_length,
                uint8_t *key,
                uint16_t key_length,
                uint8_t *iv,
                uint8_t *output)
{

  uint16_t total_len = 0;
  uint16_t chunk_len = 0;
  uint16_t offset    = 0;
  uint8_t aes_flags  = 0;
  int32_t status     = RSI_SUCCESS;

  total_len = msg_length;

  while (total_len) {
    // Check total length
    if (total_len > MAX_DATA_SIZE_BYTES) {
      chunk_len = MAX_DATA_SIZE_BYTES;
      if (offset == 0) {
        // Make aes_flags as first chunk
        aes_flags |= FIRST_CHUNK;
      } else {
        // Make aes_flags as Last chunk
        aes_flags = MIDDLE_CHUNK;
      }
    } else {
      chunk_len = total_len;
      aes_flags = LAST_CHUNK;
      if (offset == 0) {
        /* If the total length is less than 1400 and offset is zero,
					 make aes_flags as both first chunk as well as last chunk*/
        aes_flags |= FIRST_CHUNK;
      }
    }

    // Send the current chunk length message
    status = rsi_aes_pen(aes_mode, enc_dec, msg, msg_length, chunk_len, key, key_length, iv, aes_flags, output);

    if (status != RSI_SUCCESS) {
      return status;
    }

    // Increment the offset value
    offset += chunk_len;
    msg += chunk_len;

    // Decrement the total message lenth
    total_len -= chunk_len;
  }

  return status;
}
/** @} */

/*==============================================*/
/**
 * @note       This API is not supported in current release.
 * @brief      Encrypt/decrypt the data using AES
 * @param[in]  aes_mode     - 1 – For AES CBC mode \n 2 – For AES ECB mode \n 3 – For AES CTR mode \n
 * @param[in]  enc_dec      - 1 – For AES Encryption \n 2 – For AES Decryption \n
 * @param[in]  msg          - Pointer to message
 * @param[in]  msg_lengthm  - Total message length
 * @param[in]  chunk_length - Current chunk length
 * @param[in]  key          - Pointer to AES key
 * @param[in]  key_length   - AES key length in bytes
 * @param[in]  iv           - Pointer to AES iv
 * @param[in]  aes_flags    - BIT(0) - 1st chunk \n BIT(1) - Middle chunk \n BIT(2) - Last chunk\n 
 * @param[out]  output      - Output parameter to hold encrypted/decrypted from AES
 * @return	   0              - Success  \n
 *             Non-Zero Value - Failure
 *
 * @note Refer Error Codes section for above error codes \ref error-codes. 
 *
 */

int32_t rsi_aes_pen(uint16_t aes_mode,
                    uint16_t enc_dec,
                    uint8_t *msg,
                    uint16_t msg_length,
                    uint16_t chunk_length,
                    uint8_t *key,
                    uint16_t key_length,
                    uint8_t *iv,
                    uint8_t aes_flags,
                    uint8_t *output)
{

  int32_t status     = RSI_SUCCESS;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  rsi_pkt_t *pkt;
  rsi_aes_req_t *aes;

  // Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  // Input pointer check
  if (msg == NULL) {
    return RSI_ERROR_INVALID_PARAM;
  }

  if (((aes_mode == CBC_MODE) || (aes_mode == CTR_MODE)) && (iv == NULL)) {
    return RSI_ERROR_INVALID_PARAM;
  }

  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      //Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    if (output != NULL) {
      // Attach the buffer given by user
      rsi_common_cb->app_buffer = output;

      // Length of the buffer provided by user
      rsi_common_cb->app_buffer_length = msg_length;
    }

    // Get Data Pointer
    aes = (rsi_aes_req_t *)pkt->data;

    // Memset before filling
    memset(aes, 0, sizeof(rsi_aes_req_t));

    // Fill Algorithm type AES - 2
    aes->algorithm_type = AES;

    // Fill Algorithm subtype
    aes->algorithm_sub_type = aes_mode;

    // Fill enc_dec: 1 – For AES Encryption 2 – For AES Decryption
    aes->encrypt_decryption = enc_dec;

    // Fill aes_flags BIT(0) - 1st chunk BIT(2) - Last chunk
    aes->aes_flags = aes_flags;

    // Fill msg length
    aes->total_msg_length = msg_length;

    // Fill chunk length
    aes->current_chunk_length = chunk_length;

    // Fill key length
    aes->key_length = key_length;

    //KEY
    // Memset before filling
    memset(&aes->key[0], 0, key_length);

    // Copy KEY
    memcpy(&aes->key[0], key, key_length);

    if ((aes_mode == CBC_MODE) || (aes_mode == CTR_MODE)) {
      // IV
      // Memset before filling
      memset(&aes->IV[0], 0, IV_SIZE);

      // Copy IV
      memcpy(&aes->IV[0], iv, IV_SIZE);
    }

    // Data
    // Memset before filling
    memset(&aes->msg[0], 0, MAX_DATA_SIZE_BYTES);

    // Copy Data
    memcpy(&aes->msg[0], msg, chunk_length);

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_aes_req_t) - MAX_DATA_SIZE_BYTES + chunk_length;

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send aes encrypt/decrypt request to module
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_ENCRYPT_CRYPTO, pkt);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_CRYPTO_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    return status;
  }

  // Get common command response stattus
  status = rsi_common_get_status();

  // Return the status
  return status;
}

/** @} */

/** @addtogroup CRYPTO
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported in current release.
 * @brief      Calculate the DH key. This is a blocking API.
 * @param[in]  prime           - Pointer to the prime
 * @param[in]  prime_length    - Length of the prime
 * @param[in]  base            - Pointer to base
 * @param[in]  base_length     - Length of the base
 * @param[in]  exponent        - Pointer to exponent
 * @param[in]  exponent_length - Length of the exponent
 * @param[out] exp_result      - Output exponentiation result
 * @return		0              - Success   \n
 *              Non-Zero Value - If return value is lesser than 0 \n
 *                              -2: Invalid parameters \n
 *                              -3: Command given in wrong state \n
 *                              -4: Buffer not available to serve the command \n
 *                              If return value is greater than 0 \n
 *                              0xFF15, 0xCC9C, 0xCC9B
 *
 * @note Refer Error Codes section for above error codes \ref error-codes. 
 *
 */
int32_t rsi_exponentiation(uint8_t *prime,
                           uint32_t prime_length,
                           uint8_t *base,
                           uint32_t base_length,
                           uint8_t *exponent,
                           uint32_t exponent_length,
                           uint8_t *exp_result)
{
  int32_t status     = RSI_SUCCESS;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  rsi_pkt_t *pkt;
  rsi_exp_req_t *exp;

  // Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  if (wlan_cb->state < RSI_WLAN_STATE_OPERMODE_DONE) {
    // Command given in wrong state
    return RSI_ERROR_COMMAND_GIVEN_IN_WRONG_STATE;
  }

  // Input pointer check
  if ((base == NULL) || (prime == NULL) || (exponent == NULL)) {
    return RSI_ERROR_INVALID_PARAM;
  }

  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    if (exp_result != NULL) {
      // Attach the buffer given by user
      rsi_common_cb->app_buffer = exp_result;

      // Length of the buffer provided by user
      rsi_common_cb->app_buffer_length = DH_MAX_OUTPUT_SIZE;
    }

    // Get Data Pointer
    exp = (rsi_exp_req_t *)pkt->data;

    // Memset before filling
    memset(exp, 0, sizeof(rsi_exp_req_t));

    // Fill Algorithm type Exponentiation - 14
    exp->algorithm_type = DH;

    // Fill prime length
    exp->prime_length = prime_length;

    // Fill base length
    exp->base_length = base_length;

    // Fill exponent length
    exp->exponent_length = exponent_length;

    //PRIME
    // Memset before filling
    memset(&exp->prime[0], 0, prime_length);

    // Copy Prime
    memcpy(&exp->prime[0], prime, prime_length);

    //BASE
    // Memset before filling
    memset(&exp->base[0], 0, base_length);

    // Copy Base
    memcpy(&exp->base[0], base, base_length);

    // EXPONENT
    // Memset before filling
    memset(&exp->exponent[0], 0, exponent_length);

    // Copy Exponent
    memcpy(&exp->exponent[0], exponent, exponent_length);

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_exp_req_t);

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send DH key calculation request to module
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_ENCRYPT_CRYPTO, pkt);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_CRYPTO_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    return status;
  }

  // Get common command response stattus
  status = rsi_common_get_status();

  // Return the status
  return status;
}

/** @} */

/** @addtogroup CRYPTO
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported in current release.
 * @brief      Compute the ECDH point multiplication vector. This is a blocking API.
 * @param[in]  ecdh_mode  - 1 – For ECDH 192 \n 2 – For ECDH 224 \n 3 – For ECDH 256 \n
 * @param[in]  d          - Pointer to scalar value that needs to be multiplied
 * @param[in]  sx, sy, sz - Pointers to x, y, z coordinates of the point to be multiplied with scalar ‘d’
 * @param[out] rx, ry, rz - Pointers to x, y, z coordinates of the result point
 * @return		0              - Success   \n
 *              Non-Zero Value - If return value is lesser than 0 \n
 *                              -2: Invalid parameters \n
 *                              -3: Command given in wrong state \n
 *                              -4: Buffer not available to serve the command \n
 *                              If return value is greater than 0 \n
 *                              0xFF15, 0xCC9C, 0xCC9B
 *
 * @note Refer Error Codes section for above error codes \ref error-codes. 
 *
 */

int32_t rsi_ecdh_point_multiplication(uint8_t ecdh_mode,
                                      uint8_t *d,
                                      uint8_t *sx,
                                      uint8_t *sy,
                                      uint8_t *sz,
                                      uint8_t *rx,
                                      uint8_t *ry,
                                      uint8_t *rz)
{
  int32_t status     = RSI_SUCCESS;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  uint8_t result[96] = { 0 };
  uint8_t size       = 0;
  uint8_t offset     = 0;
  rsi_pkt_t *pkt;
  rsi_ecdh_pm_req_t *ecdh;

  // Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    if (ecdh_mode == ECDH_192) {
      size = ECDH_VECTOR_192_SIZE;
    } else if (ecdh_mode == ECDH_224) {
      size = ECDH_VECTOR_224_SIZE;
    } else if (ecdh_mode == ECDH_256) {
      size = ECDH_VECTOR_256_SIZE;
    } else {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      return RSI_ERROR_INVALID_PARAM;
    }
    // Allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    if (result != NULL) {
      // Attach the buffer given by user
      rsi_common_cb->app_buffer = result;

      // Length of the buffer provided by user
      rsi_common_cb->app_buffer_length = ECDH_MAX_OUTPUT_SIZE;
    }

    // Get Data Pointer
    ecdh = (rsi_ecdh_pm_req_t *)pkt->data;

    // Memset before filling
    memset(ecdh, 0, sizeof(rsi_ecdh_pa_ps_req_t));

    // Fill Algorithm type ECDH - 15
    ecdh->algorithm_type = ECDH;

    // Fill ecdh mode
    ecdh->ecdh_mode = ecdh_mode;

    // Fill ecdh sub mode 1 - point Multiplication
    ecdh->ecdh_sub_mode = ECDH_PM;

    // Memset before filling
    memset(&ecdh->sx[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sx[0], sx, size);

    // Memset before filling
    memset(&ecdh->sy[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sy[0], sy, size);

    // Memset before filling
    memset(&ecdh->sz[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sz[0], sz, size);

    // Memset before filling
    memset(&ecdh->d[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->d[0], d, size);

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_ecdh_pm_req_t);

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send ecdh point multiplication request to module
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_ENCRYPT_CRYPTO, pkt);

    // Copy Data
    memcpy(rx, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);
    offset += ECDH_MAX_VECTOR_SIZE;

    // Copy Data
    memcpy(ry, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);
    offset += ECDH_MAX_VECTOR_SIZE;

    // Copy Data
    memcpy(rz, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_CRYPTO_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    return status;
  }

  // Get common command response stattus
  status = rsi_common_get_status();

  // Return the status
  return status;
}

/** @} */

/** @addtogroup CRYPTO
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported in current release.
 * @brief      Compute the ECDH point addition vector. This is a blocking API.
 * @param[in]  ecdh_mode  - 1 – For ECDH 192 \n 2 – For ECDH 224 \n 3 – For ECDH 256 \n
 * @param[in]  sx, sy, sz - Pointers to x, y, z coordinates of the point1 that needs to be added
 * @param[in]  tx, ty, tz - Pointers to x, y, z coordinates of the point2 that needs to be added
 * @param[out] rx, ry, rz - Pointers to x, y, z coordinates of the result point
 * @return	    0              - Success   \n
 *              Non-Zero Value - If return value is lesser than 0 \n
 *                              -2: Invalid parameters \n
 *                              -3: Command given in wrong state \n
 *                              -4: Buffer not available to serve the command \n
 *                              If return value is greater than 0 \n
 *                              0xFF15, 0xCC9C, 0xCC9B
 *
 * @note Refer Error Codes section for above error codes \ref error-codes. 
 *
 */

int32_t rsi_ecdh_point_addition(uint8_t ecdh_mode,
                                uint8_t *sx,
                                uint8_t *sy,
                                uint8_t *sz,
                                uint8_t *tx,
                                uint8_t *ty,
                                uint8_t *tz,
                                uint8_t *rx,
                                uint8_t *ry,
                                uint8_t *rz)
{

  int32_t status     = RSI_SUCCESS;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  uint8_t result[96] = { 0 };
  uint8_t size       = 0;
  uint8_t offset     = 0;
  rsi_pkt_t *pkt;
  rsi_ecdh_pa_ps_req_t *ecdh;

  // Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  if (rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE) == RSI_SUCCESS) {
    // Fill vector size based on ecdh mode
    if (ecdh_mode == ECDH_192) {
      size = ECDH_VECTOR_192_SIZE;
    } else if (ecdh_mode == ECDH_224) {
      size = ECDH_VECTOR_224_SIZE;
    } else if (ecdh_mode == ECDH_256) {
      size = ECDH_VECTOR_256_SIZE;
    } else {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      return RSI_ERROR_INVALID_PARAM;
    }

    // Allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    if (result != NULL) {
      // Attach the buffer given by user
      rsi_common_cb->app_buffer = result;

      // Length of the buffer provided by user
      rsi_common_cb->app_buffer_length = ECDH_MAX_OUTPUT_SIZE;
    }

    // Get Data Pointer
    ecdh = (rsi_ecdh_pa_ps_req_t *)pkt->data;

    // Memset before filling
    memset(ecdh, 0, sizeof(rsi_ecdh_pa_ps_req_t));

    // Fill Algorithm type ECDH - 15
    ecdh->algorithm_type = ECDH;

    // Fill ecdh mode
    ecdh->ecdh_mode = ecdh_mode;

    // Fill ecdh sub mode 2 - point Addition
    ecdh->ecdh_sub_mode = ECDH_PA;

    // Memset before filling
    memset(&ecdh->sx[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sx[0], sx, size);

    // Memset before filling
    memset(&ecdh->sy[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sy[0], sy, size);

    // Memset before filling
    memset(&ecdh->sz[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sz[0], sz, size);

    // Memset before filling
    memset(&ecdh->tx[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->tx[0], tx, size);

    // Memset before filling
    memset(&ecdh->ty[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->ty[0], ty, size);

    // Memset before filling
    memset(&ecdh->tz[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->tz[0], tz, size);

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_ecdh_pa_ps_req_t);

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send point addition calculation request to module
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_ENCRYPT_CRYPTO, pkt);

    // Copy Data
    memcpy(rx, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);
    offset += ECDH_MAX_VECTOR_SIZE;

    // Copy Data
    memcpy(ry, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);
    offset += ECDH_MAX_VECTOR_SIZE;

    // Copy Data
    memcpy(rz, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_CRYPTO_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    return RSI_ERROR_COMMON_CMD_IN_PROGRESS;
  }

  // Get common command response stattus
  status = rsi_common_get_status();

  // Return the status
  return status;
}

/** @} */

/** @addtogroup CRYPTO
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported in current release.
 * @brief      Compute the ECDH point subtraction vector. This is a blocking API.
 * @param[in]  ecdh_mode  - 1 – For ECDH 192 \n 2 – For ECDH 224 \n 3 – For ECDH 256 \n    
 * @param[in]  sx, sy, sz - Pointers to x, y, z coordinates of the point1 that needs to be subtracted
 * @param[in]  tx, ty, tz - Pointers to x, y, z coordinates of the point2 that needs to be subtracted
 * @param[out] rx, ry, rz - Pointers to x, y, z coordinates of the result point
 * @return		0              - Success   \n
 *              Non-Zero Value - If return value is lesser than 0 \n
 *                              -2: Invalid parameters \n
 *                              -3: Command given in wrong state \n
 *                              -4: Buffer not available to serve the command \n
 *                              If return value is greater than 0 \n
 *                              0xFF15, 0xCC9C, 0xCC9B
 *
 *
 */

int32_t rsi_ecdh_point_subtraction(uint8_t ecdh_mode,
                                   uint8_t *sx,
                                   uint8_t *sy,
                                   uint8_t *sz,
                                   uint8_t *tx,
                                   uint8_t *ty,
                                   uint8_t *tz,
                                   uint8_t *rx,
                                   uint8_t *ry,
                                   uint8_t *rz)
{
  int32_t status     = RSI_SUCCESS;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  uint8_t result[96] = { 0 };
  uint8_t size       = 0;
  uint8_t offset     = 0;
  rsi_pkt_t *pkt;
  rsi_ecdh_pa_ps_req_t *ecdh;

  // Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Fill vector size based on ecdh mode
    if (ecdh_mode == ECDH_192) {
      size = ECDH_VECTOR_192_SIZE;
    } else if (ecdh_mode == ECDH_224) {
      size = ECDH_VECTOR_224_SIZE;
    } else if (ecdh_mode == ECDH_256) {
      size = ECDH_VECTOR_256_SIZE;
    } else {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      return RSI_ERROR_INVALID_PARAM;
    }

    // Allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    if (result != NULL) {
      // Attach the buffer given by user
      rsi_common_cb->app_buffer = result;

      // Length of the buffer provided by user
      rsi_common_cb->app_buffer_length = ECDH_MAX_OUTPUT_SIZE;
    }

    // Get Data Pointer
    ecdh = (rsi_ecdh_pa_ps_req_t *)pkt->data;

    // Memset before filling
    memset(ecdh, 0, sizeof(rsi_ecdh_pa_ps_req_t));

    // Fill Algorithm type ECDH - 15
    ecdh->algorithm_type = ECDH;

    // Fill ecdh mode
    ecdh->ecdh_mode = ecdh_mode;

    // Fill ecdh sub mode 3 - point subtraction
    ecdh->ecdh_sub_mode = ECDH_PS;

    // Memset before filling
    memset(&ecdh->sx[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sx[0], sx, size);

    // Memset before filling
    memset(&ecdh->sy[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sy[0], sy, size);

    // Memset before filling
    memset(&ecdh->sz[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sz[0], sz, size);

    // Memset before filling
    memset(&ecdh->tx[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->tx[0], tx, size);

    // Memset before filling
    memset(&ecdh->ty[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->ty[0], ty, size);

    // Memset before filling
    memset(&ecdh->tz[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->tz[0], tz, size);

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_ecdh_pa_ps_req_t);

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send point subtraction calculation request to module
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_ENCRYPT_CRYPTO, pkt);

    // Copy Data
    memcpy(rx, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);
    offset += ECDH_MAX_VECTOR_SIZE;

    // Copy Data
    memcpy(ry, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);
    offset += ECDH_MAX_VECTOR_SIZE;

    // Copy Data
    memcpy(rz, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_CRYPTO_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    return status;
  }

  // Get common command response stattus
  status = rsi_common_get_status();

  // Return the status
  return status;
}

/** @} */

/** @addtogroup CRYPTO
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported in current release.
 * @brief      Compute the ECDH point double vector. This is a blocking API.
 * @param[in]  ecdh_mode  - 1 – For ECDH 192 \n 2 – For ECDH 224 \n 3 – For ECDH 256 \n
 * @param[in]  sx, sy, sz - Pointers to x, y, z coordinates of the point1 that needs to be doubled
 * @param[out] rx, ry, rz - Pointers to x, y, z coordinates of the result point
 * @return		0              - Success   \n
 *              Non-Zero Value - If return value is lesser than 0 \n
 *                              -2: Invalid parameters \n
 *                              -3: Command given in wrong state \n
 *                              -4: Buffer not available to serve the command \n
 *                              If return value is greater than 0 \n
 *                              0xFF15, 0xCC9C, 0xCC9B
 *
 * @note Refer Error Codes section for above error codes \ref error-codes. 
 *
 *
 */

int32_t rsi_ecdh_point_double(uint8_t ecdh_mode,
                              uint8_t *sx,
                              uint8_t *sy,
                              uint8_t *sz,
                              uint8_t *rx,
                              uint8_t *ry,
                              uint8_t *rz)
{
  int32_t status     = RSI_SUCCESS;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  uint8_t result[96] = { 0 };
  uint8_t size       = 0;
  uint8_t offset     = 0;
  rsi_pkt_t *pkt;
  rsi_ecdh_pd_req_t *ecdh;

  // Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Fill vector size based on ecdh mode
    if (ecdh_mode == ECDH_192) {
      size = ECDH_VECTOR_192_SIZE;
    } else if (ecdh_mode == ECDH_224) {
      size = ECDH_VECTOR_224_SIZE;
    } else if (ecdh_mode == ECDH_256) {
      size = ECDH_VECTOR_256_SIZE;
    } else {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      return RSI_ERROR_INVALID_PARAM;
    }
    // Allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    if (result != NULL) {
      // Attach the buffer given by user
      rsi_common_cb->app_buffer = result;

      // Length of the buffer provided by user
      rsi_common_cb->app_buffer_length = ECDH_MAX_OUTPUT_SIZE;
    }

    // Get Data Pointer
    ecdh = (rsi_ecdh_pd_req_t *)pkt->data;

    // Memset before filling
    memset(ecdh, 0, sizeof(rsi_ecdh_pa_ps_req_t));

    // Fill Algorithm type ECDH - 15
    ecdh->algorithm_type = ECDH;

    // Fill ecdh mode
    ecdh->ecdh_mode = ecdh_mode;

    // Fill ecdh sub mode 4 - point double
    ecdh->ecdh_sub_mode = ECDH_PD;

    // Memset before filling
    memset(&ecdh->sx[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sx[0], sx, size);

    // Memset before filling
    memset(&ecdh->sy[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sy[0], sy, size);

    // Memset before filling
    memset(&ecdh->sz[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sz[0], sz, size);

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_ecdh_pd_req_t);

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send point double calculation request to module
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_ENCRYPT_CRYPTO, pkt);

    // Copy Data
    memcpy(rx, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);
    offset += ECDH_MAX_VECTOR_SIZE;

    // Copy Data
    memcpy(ry, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);
    offset += ECDH_MAX_VECTOR_SIZE;

    // Copy Data
    memcpy(rz, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_CRYPTO_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    return status;
  }

  // Get common command response stattus
  status = rsi_common_get_status();

  // Return the status
  return status;
}

/** @} */

/** @addtogroup CRYPTO
* @{
*/
/*==============================================*/
/**
 * @note       This API is not supported in current release.
 * @brief      Compute the ECDH point affinity vector. This is a blocking API.
 * @param[in]  ecdh_mode  - 1 – For ECDH 192 \n 2 – For ECDH 224 \n 3 – For ECDH 256 \n
 * @param[in]  sx, sy, sz - Pointers to x, y, z coordinates of the point1 that needs to be affinified
 * @param[out] rx, ry, rz - Pointers to x, y, z coordinates of the result point
 * @return		0              - Success   \n
 *              Non-Zero Value - If return value is lesser than 0 \n
 *                              -2: Invalid parameters \n
 *                              -3: Command given in wrong state \n
 *                              -4: Buffer not available to serve the command \n
 *                              If return value is greater than 0 \n
 *                              0xFF15, 0xCC9C, 0xCC9B
 *
 * @note Refer Error Codes section for above error codes \ref error-codes. 
 *
 */

int32_t rsi_ecdh_point_affine(uint8_t ecdh_mode,
                              uint8_t *sx,
                              uint8_t *sy,
                              uint8_t *sz,
                              uint8_t *rx,
                              uint8_t *ry,
                              uint8_t *rz)
{
  int32_t status     = RSI_SUCCESS;
  uint16_t send_size = 0;
  uint8_t *host_desc = NULL;
  uint8_t result[96] = { 0 };
  uint8_t size       = 0;
  uint8_t offset     = 0;
  rsi_pkt_t *pkt;
  rsi_ecdh_affine_req_t *ecdh;

  // Get wlan cb structure pointer
  rsi_wlan_cb_t *wlan_cb = rsi_driver_cb->wlan_cb;

  // Get commmon cb pointer
  rsi_common_cb_t *rsi_common_cb = rsi_driver_cb->common_cb;

  status = rsi_check_and_update_cmd_state(COMMON_CMD, IN_USE);
  if (status == RSI_SUCCESS) {

    // Fill vector size based on ecdh mode
    if (ecdh_mode == ECDH_192) {
      size = ECDH_VECTOR_192_SIZE;
    } else if (ecdh_mode == ECDH_224) {
      size = ECDH_VECTOR_224_SIZE;
    } else if (ecdh_mode == ECDH_256) {
      size = ECDH_VECTOR_256_SIZE;
    } else {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      return RSI_ERROR_INVALID_PARAM;
    }

    // Allocate command buffer  from wlan pool
    pkt = rsi_pkt_alloc(&rsi_common_cb->common_tx_pool);

    // If allocation of packet fails
    if (pkt == NULL) {
      // Change common state to allow state
      rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);
      // Return packet allocation failure error
      return RSI_ERROR_PKT_ALLOCATION_FAILURE;
    }

    if (result != NULL) {
      // Attach the buffer given by user
      rsi_common_cb->app_buffer = result;

      // Length of the buffer provided by user
      rsi_common_cb->app_buffer_length = ECDH_MAX_OUTPUT_SIZE;
    }

    // Get Data Pointer
    ecdh = (rsi_ecdh_affine_req_t *)pkt->data;

    // Memset before filling
    memset(ecdh, 0, sizeof(rsi_ecdh_affine_req_t));

    // Fill Algorithm type ECDH - 15
    ecdh->algorithm_type = ECDH;

    // Fill ecdh mode
    ecdh->ecdh_mode = ecdh_mode;

    // Fill ecdh sub mode 5 - point affine
    ecdh->ecdh_sub_mode = ECDH_PAF;

    // Memset before filling
    memset(&ecdh->sx[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sx[0], sx, size);

    // Memset before filling
    memset(&ecdh->sy[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sy[0], sy, size);

    // Memset before filling
    memset(&ecdh->sz[0], 0, ECDH_MAX_VECTOR_SIZE);

    // Copy Data
    memcpy(&ecdh->sz[0], sz, size);

    // Using host descriptor to set payload length
    send_size = sizeof(rsi_ecdh_affine_req_t);

    // Get the host descriptor
    host_desc = (pkt->desc);

    // Fill data length in the packet host descriptor
    rsi_uint16_to_2bytes(host_desc, (send_size & 0xFFF));

#ifndef RSI_COMMON_SEM_BITMAP
    rsi_driver_cb_non_rom->common_wait_bitmap |= BIT(0);
#endif
    // Send point affine calculation request to module
    status = rsi_driver_common_send_cmd(RSI_COMMON_REQ_ENCRYPT_CRYPTO, pkt);

    // Copy Data
    memcpy(rx, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);
    offset += ECDH_MAX_VECTOR_SIZE;

    // Copy Data
    memcpy(ry, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);
    offset += ECDH_MAX_VECTOR_SIZE;

    // Copy Data
    memcpy(rz, (rsi_common_cb->app_buffer) + offset, ECDH_MAX_VECTOR_SIZE);

    // Wait on common semaphore
    rsi_wait_on_common_semaphore(&rsi_driver_cb_non_rom->common_cmd_sem, RSI_CRYPTO_RESPONSE_WAIT_TIME);

    // Change common state to allow state
    rsi_check_and_update_cmd_state(COMMON_CMD, ALLOW);

  }

  else {
    // Return common command error
    return status;
  }

  // Get common command response stattus
  status = rsi_common_get_status();

  // Return the status
  return status;
}
/** @} */
void reverse_8(unsigned char *xx, int no_digits)
{
  int count;
  uint8_t temp;
  for (count = 0; count < (no_digits / 2); count++) {
    temp                      = xx[count];
    xx[count]                 = xx[no_digits - count - 1];
    xx[no_digits - count - 1] = temp;
  }
}

#endif

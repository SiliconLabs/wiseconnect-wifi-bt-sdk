/*******************************************************************************
* @file  rsi_crypto.h
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

#ifndef RSI_SHA_H
#define RSI_SHA_H
/******************************************************
 * *                      Macros
 * ******************************************************/

#define FIRST_CHUNK  BIT(0)
#define MIDDLE_CHUNK BIT(1)
#define LAST_CHUNK   BIT(2)

#define SHA_1   1
#define SHA_256 2
#define SHA_384 3
#define SHA_512 4

#define AES      2
#define SHA      4
#define HMAC_SHA 5
#define DH       14
#define ECDH     15

#define ECDH_PM  1
#define ECDH_PA  2
#define ECDH_PS  3
#define ECDH_PD  4
#define ECDH_PAF 5

#define SHA_1_DIGEST_LEN   20
#define SHA_256_DIGEST_LEN 32
#define SHA_384_DIGEST_LEN 48
#define SHA_512_DIGEST_LEN 64

#define HMAC_SHA_1    1
#define HMAC_SHA_256  2
#define HMAC_SHAA_384 3
#define HMAC_SHA_512  4
#define IV_SIZE       16

#define ECDH_VECTOR_192_SIZE 24
#define ECDH_VECTOR_224_SIZE 28
#define ECDH_VECTOR_256_SIZE 32

#define ECDH_MAX_OUTPUT_SIZE 96
#define ECDH_MAX_VECTOR_SIZE 32
#define DH_MAX_OUTPUT_SIZE   512

#define HMAC_SHA_1_DIGEST_LEN   20
#define HMAC_SHA_256_DIGEST_LEN 32
#define HMAC_SHA_384_DIGEST_LEN 48
#define HMAC_SHA_512_DIGEST_LEN 64

#define CBC_MODE 1
#define ECB_MODE 2
#define CTR_MODE 3

#define AES_ENCRYPTION 1
#define AES_DECRYPTION 2

#define AES_KEY_SIZE_128 16
#define AES_KEY_SIZE_192 24
#define AES_KEY_SIZE_256 32

#define ECDH_192 1
#define ECDH_224 2
#define ECDH_256 4

#define MAX_DATA_SIZE_BYTES 1400 /*Data size*/
/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/

// SHA Request Frames Structures

typedef struct rsi_sha_req_s {
  uint16_t algorithm_type;
  uint8_t algorithm_sub_type;
  uint8_t sha_flags;
  uint16_t total_msg_length;
  uint16_t current_chunk_length;
  uint8_t msg[1400];
} rsi_sha_req_t;

typedef struct rsi_hmac_sha_req_s {
  uint16_t algorithm_type;
  uint8_t algorithm_sub_type;
  uint8_t hmac_sha_flags;
  uint16_t total_msg_length;
  uint16_t current_chunk_length;
  uint32_t key_length;
  uint8_t hmac_data[1400];
} rsi_hmac_sha_req_t;

typedef struct rsi_aes_req_s {
  uint16_t algorithm_type;
  uint8_t algorithm_sub_type;
  uint8_t aes_flags;
  uint16_t total_msg_length;
  uint16_t current_chunk_length;
  uint16_t encrypt_decryption;
  uint16_t key_length;
  uint8_t key[32];
  uint8_t IV[16];
  uint8_t msg[1400];
} rsi_aes_req_t;

typedef struct rsi_exp_req_s {
  uint32_t algorithm_type;
  uint32_t prime_length;
  uint32_t base_length;
  uint32_t exponent_length;
  uint8_t prime[512];
  uint8_t base[512];
  uint8_t exponent[512];
} rsi_exp_req_t;

typedef struct rsi_ecdh_pm_req_s {
  uint8_t algorithm_type;
  uint8_t ecdh_mode;
  uint8_t ecdh_sub_mode;
  uint8_t d[32];
  uint8_t sx[32];
  uint8_t sy[32];
  uint8_t sz[32];
} rsi_ecdh_pm_req_t;

typedef struct rsi_ecdh_pa_ps_req_s {
  uint8_t algorithm_type;
  uint8_t ecdh_mode;
  uint8_t ecdh_sub_mode;
  uint8_t sx[32];
  uint8_t sy[32];
  uint8_t sz[32];
  uint8_t tx[32];
  uint8_t ty[32];
  uint8_t tz[32];
} rsi_ecdh_pa_ps_req_t;

typedef struct rsi_ecdh_pd_req_s {
  uint8_t algorithm_type;
  uint8_t ecdh_mode;
  uint8_t ecdh_sub_mode;
  uint8_t sx[32];
  uint8_t sy[32];
  uint8_t sz[32];
} rsi_ecdh_pd_req_t;

typedef struct rsi_ecdh_affine_req_s {
  uint8_t algorithm_type;
  uint8_t ecdh_mode;
  uint8_t ecdh_sub_mode;
  uint8_t sx[32];
  uint8_t sy[32];
  uint8_t sz[32];
} rsi_ecdh_affine_req_t;
/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/
int32_t rsi_sha(uint8_t sha_mode, uint8_t *msg, uint16_t msg_length, uint8_t *digest);
int32_t rsi_sha_pen(uint8_t sha_mode,
                    uint8_t *msg,
                    uint16_t msg_length,
                    uint16_t chunk_len,
                    uint8_t pending_flag,
                    uint8_t *digest);
int32_t rsi_hmac_sha(uint8_t hmac_sha_mode,
                     uint8_t *msg,
                     uint32_t msg_length,
                     uint8_t *key,
                     uint32_t key_length,
                     uint8_t *digest,
                     uint8_t *hmac_buffer);
int32_t rsi_hmac_sha_pen(uint8_t hmac_sha_mode,
                         uint8_t *data,
                         uint16_t total_length,
                         uint16_t chunk_length,
                         uint32_t key_length,
                         uint8_t pending_flag,
                         uint8_t *digest);
int32_t rsi_aes_pen(uint16_t aes_mode,
                    uint16_t enc_dec,
                    uint8_t *msg,
                    uint16_t msg_length,
                    uint16_t chunk_length,
                    uint8_t *key,
                    uint16_t key_length,
                    uint8_t *iv,
                    uint8_t aes_flags,
                    uint8_t *output);
int32_t rsi_aes(uint16_t aes_mode,
                uint16_t enc_dec,
                uint8_t *msg,
                uint16_t msg_length,
                uint8_t *key,
                uint16_t key_length,
                uint8_t *iv,
                uint8_t *output);
int32_t rsi_exponentiation(uint8_t *prime,
                           uint32_t prime_length,
                           uint8_t *base,
                           uint32_t base_length,
                           uint8_t *exponent,
                           uint32_t exponent_length,
                           uint8_t *exp_result);
#endif

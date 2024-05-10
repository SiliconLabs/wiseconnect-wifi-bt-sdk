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
#ifdef SIDE_BAND_CRYPTO
#ifndef RSI_CRYPTO_H
#define RSI_CRYPTO_H
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
#define SHA_224 5

#define SHAKE_128 21
#define SHAKE_256 17
#define SHA3_224  18
#define SHA3_256  17
#define SHA3_384  13
#define SHA3_512  9

#define AES               2
#define SHA               4
#define HMAC_SHA          5
#define DH                14
#define ECDH              15
#define SHA3              16
#define CHACHAPOLY        17
#define GCM               18
#define SHAKE             19
#define WRAP              20
#define TRNG              21
#define RSI_BUILT_IN_KEYS 22
#define ATTESTATION       30
#define CCM               31

#define ECDH_PM  1
#define ECDH_PA  2
#define ECDH_PS  3
#define ECDH_PD  4
#define ECDH_PAF 5

#define SHA_1_DIGEST_LEN     20
#define SHA_256_DIGEST_LEN   32
#define SHA_384_DIGEST_LEN   48
#define SHA_512_DIGEST_LEN   64
#define SHA_224_DIGEST_LEN   28
#define SHAKE_128_DIGEST_LEN 16

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

#define GCM_ENCRYPTION 0
#define GCM_DECRYPTION 1

#define CCM_ENCRYPTION              0
#define CCM_DECRYPTION              1
#define CCM_IV_MIN_SIZE_BYTES       7
#define CCM_IV_MAX_SIZE_BYTES       13
#define CCM_MAX_HEADER_LENGTH       32
#define CCM_KEY_SIZE_128            16
#define CCM_KEY_SIZE_192            24
#define CCM_KEY_SIZE_256            32
#define MAX_DATA_SIZE_BYTES_FOR_CCM 1200

#define GCM_MODE  0
#define CMAC_MODE 1

//! AES Block size
#define AES_BLOCK_SIZE 16

#define AES_KEY_SIZE_128 16
#define AES_KEY_SIZE_192 24
#define AES_KEY_SIZE_256 32

#define GCM_KEY_SIZE_128 16
#define GCM_KEY_SIZE_192 24
#define GCM_KEY_SIZE_256 32

#define GCM_IV_SIZE_IN_BYTES 12
#define ECDH_192             1
#define ECDH_224             2
#define ECDH_256             4

//! ECDH curve types
#define ECDH_CURVE_P 0
#define ECDH_CURVE_K 1
#define ECDH_CURVE_B 2

//!DMA Support
#define DMA_ENABLE  1
#define DMA_DISABLE 0

//! PAD Char
#define PAD_CHAR_SHA3                      0x06
#define PAD_CHAR_SHAKE                     0x1F
#define CHACHA20POLY1305                   0
#define CHACHA20                           1
#define CHACHAPOLY_MODE_POLY1305_KEYR_KEYS 2
#define POLY1305_MODE                      3

#define TAG_SIZE 16

#define CHACHAPOLY_ENCRYPTION 0
#define CHACHAPOLY_DECRYPTION 1

#define KEYR_KEYS_LENGTH                   8
#define KEY_CHACHA_SIZE                    32
#define KEYR_SIZE                          16
#define KEYS_SIZE                          16
#define NONCE_SIZE                         16
#define MAX_DATA_SIZE_BYTES                1400 /*Data size*/
#define MAX_DATA_SIZE_BYTES_FOR_CHACHAPOLY 1200 /*Data size for chachapoly*/

#define TRNG_INIT       1
#define TRNG_ENTROPY    2
#define TRNG_KEY        3
#define TRNG_GENERATION 4

#define TRNG_INIT_MSG_LENGTH 16
#define TRNG_KEY_SIZE        4
#define TRNG_TEST_DATA_SIZE  64

//! Types of keys
#define RSI_TRANSPARENT_KEY 0
#define RSI_WRAPPED_KEY     1
#define RSI_BUILT_IN_KEY    2

//! Wrap IV mode
#define WRAP_IV_ECB_MODE 0
#define WRAP_IV_CBC_MODE 1

#define NONCE_DATA_SIZE 32

//! Key slot for Built in keys
#define GET_M4_OTA_KEY    1
#define GET_M4_PUBLIC_KEY 2

//! Key size for Built in keys
#define M4_OTA_KEY_SIZE    32
#define M4_PUBLIC_KEY_SIZE 96
/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/

#ifdef CHIP_917B0
typedef struct crypto_key_s {
  //! For built-in key
  uint32_t key_slot;
  //!IV mode 0-> ECB; 1-> CBC
  uint32_t wrap_iv_mode;
  //! IV for CBC mode
  uint8_t wrap_iv[16];
  //! Key data wrapped/ Plain text
  uint8_t key_buffer[32];
} crypto_key_t;

typedef struct sapi_key_info_s {
  uint32_t key_size;
  crypto_key_t key_spec;
} sapi_key_info_t;

typedef struct key_descriptor_s {
  uint32_t key_type;
  uint32_t reserved;
  sapi_key_info_t key_detail;
} key_descriptor_t;

typedef struct key_descriptor_wrap_s {
  uint32_t key_type;
  uint32_t reserved;
  uint32_t key_size;
  //!IV mode 0-> ECB; 1-> CBC
  uint32_t wrap_iv_mode;
  //! IV for CBC mode
  uint8_t wrap_iv[16];
  //! Key data wrapped/ Plain text
  uint8_t key_buffer[1400];
} key_descriptor_wrap_t;

typedef struct wrap_req_s {
  uint8_t algorithm_type;
  uint8_t wrap_flags;
  uint16_t current_chunk_length;
  key_descriptor_wrap_t key_info;
} wrap_req_t;

//! Built-in keys strcuture
typedef struct built_in_key_req_s {
  uint16_t algorithm_type;
  uint16_t key_size;
  uint32_t key_slot;
} built_in_key_req_t;
#endif

// TRNG Request Frames Structures
typedef struct rsi_trng_req_s {
  uint8_t algorithm_type;
  uint8_t algorithm_sub_type;
  uint16_t total_msg_length;
  uint32_t trng_key[TRNG_KEY_SIZE];
  uint32_t msg[TRNG_TEST_DATA_SIZE];
} rsi_trng_req_t;

// Attestation token Request Frames Structures
typedef struct rsi_token_req_s {
  uint8_t algorithm_type;
  uint16_t total_msg_length;
  uint32_t msg[NONCE_DATA_SIZE];
} rsi_token_req_t;

// SHA Request Frames Structures
typedef struct rsi_sha_req_s {
  uint16_t algorithm_type;
  uint8_t algorithm_sub_type;
  uint8_t sha_flags;
  uint16_t total_msg_length;
  uint16_t current_chunk_length;
  uint8_t msg[1400];
} rsi_sha_req_t;

typedef struct rsi_chachapoly_req_s {
  uint16_t algorithm_type;
  uint8_t algorithm_sub_type;
  uint8_t chachapoly_flags;
  uint16_t total_msg_length;
  uint16_t header_length;
  uint16_t current_chunk_length;
  uint16_t encrypt_decryption;
  uint32_t dma_use;
  uint8_t nonce[16];
#ifdef CHIP_917B0
  key_descriptor_t key_info;
#else
  uint8_t key_chacha[32];
  uint8_t keyr_in[16];
  uint8_t keys_in[16];
#endif
  uint8_t header_input[128];
  uint8_t msg[1200];
} rsi_chachapoly_req_t;

typedef struct rsi_hmac_sha_req_s {
  uint16_t algorithm_type;
  uint8_t algorithm_sub_type;
  uint8_t hmac_sha_flags;
  uint16_t total_msg_length;
  uint16_t current_chunk_length;
#ifdef CHIP_917B0
  key_descriptor_t key_info;
#else
  uint32_t key_length;
#endif
  uint8_t hmac_data[1400];
} rsi_hmac_sha_req_t;

typedef struct rsi_aes_req_s {
  uint16_t algorithm_type;
  uint8_t algorithm_sub_type;
  uint8_t aes_flags;
  uint16_t total_msg_length;
  uint16_t current_chunk_length;
  uint32_t encrypt_decryption;
  uint16_t output_length;
#ifdef CHIP_917B0
  key_descriptor_t key_info;
#else
  uint32_t key_length;
  uint8_t key[32];
#endif
  uint8_t *IV;
  uint8_t *msg;
  uint8_t *output;
} rsi_aes_req_t;

typedef struct rsi_gcm_req_s {
  uint16_t algorithm_type;
  uint8_t gcm_flags;
  uint8_t encrypt_decryption;
  uint16_t total_msg_length;
  uint16_t current_chunk_length;
  uint16_t header_length;
  uint16_t dma_use;
  uint16_t output_length;
#ifdef CHIP_917B0
  uint32_t gcm_mode;
  key_descriptor_t key_info;
#else
  uint32_t key_length;
  uint8_t key[32];
#endif
  uint8_t *IV;
  uint8_t *header;
  uint8_t *msg;
  uint8_t *output;
} rsi_gcm_req_t;

typedef struct rsi_ccm_req_s {
  uint16_t algorithm_type;
  uint8_t ccm_flags;
  uint8_t nonce_len;
  uint16_t encrypt_decryption;
  uint16_t total_msg_length;
  uint16_t current_chunk_length;
  uint16_t ad_len;
  uint32_t tag_len;
#ifdef CHIP_917B0
  key_descriptor_t key_info;
#else
  uint32_t key_length;
  uint8_t key[AES_KEY_SIZE_256];
#endif
  uint8_t nonce[CCM_IV_MAX_SIZE_BYTES]; // max iv length = 13 bytes
  uint8_t ad[CCM_MAX_HEADER_LENGTH];    //max ad length = 2^32
  uint8_t tag[TAG_SIZE];
  uint8_t msg[MAX_DATA_SIZE_BYTES_FOR_CCM];
} rsi_ccm_req_t;

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
  uint8_t ecdh_curve_type;
  uint32_t affinity;
  uint8_t *d;
  uint8_t *sx;
  uint8_t *sy;
  uint8_t *sz;
  uint8_t *rx;
  uint8_t *ry;
  uint8_t *rz;
} rsi_ecdh_pm_req_t;

typedef struct rsi_ecdh_pa_ps_req_s {
  uint8_t algorithm_type;
  uint8_t ecdh_mode;
  uint8_t ecdh_sub_mode;
  uint8_t *sx;
  uint8_t *sy;
  uint8_t *sz;
  uint8_t *tx;
  uint8_t *ty;
  uint8_t *tz;
  uint8_t *rx;
  uint8_t *ry;
  uint8_t *rz;
} rsi_ecdh_pa_ps_req_t;

typedef struct rsi_ecdh_pd_req_s {
  uint8_t algorithm_type;
  uint8_t ecdh_mode;
  uint8_t ecdh_sub_mode;
  uint8_t *sx;
  uint8_t *sy;
  uint8_t *sz;
  uint8_t *rx;
  uint8_t *ry;
  uint8_t *rz;
} rsi_ecdh_pd_req_t;

typedef struct rsi_ecdh_affine_req_s {
  uint8_t algorithm_type;
  uint8_t ecdh_mode;
  uint8_t ecdh_sub_mode;
  uint8_t ecdh_curve_type;
  uint8_t *sx;
  uint8_t *sy;
  uint8_t *sz;
  uint8_t *rx;
  uint8_t *ry;
  uint8_t *rz;
} rsi_ecdh_affine_req_t;

#ifdef CHIP_917B0
typedef struct key_var_initialize_s {
  uint32_t key_type;
  uint32_t key_size;
  uint32_t key_slot;
  uint32_t wrap_iv_mode;
  uint32_t reserved;
} key_var_initialize_t;
#endif
/******************************************************
 * *                    Structures
 * ******************************************************/
/******************************************************
 * *                 Global Variables
 * ******************************************************/
/******************************************************
 * *               Function Declarations
 * ******************************************************/

int32_t rsi_crypto_get_status(void);
void rsi_crypto_set_status(int32_t status);
rsi_error_t rsi_wait_on_crypto_semaphore(rsi_semaphore_handle_t *semaphore, uint32_t timeout_ms);
int32_t rsi_driver_crypto_send_cmd(rsi_common_cmd_request_t cmd, rsi_pkt_t *pkt);

int32_t rsi_sha(uint8_t sha_mode, uint8_t *msg, uint16_t msg_length, uint8_t *digest);
int32_t rsi_sha_pen(uint8_t sha_mode,
                    uint8_t *msg,
                    uint16_t msg_length,
                    uint16_t chunk_len,
                    uint8_t pending_flag,
                    uint8_t *digest);
#ifdef CHIP_917B0
int32_t rsi_hmac_sha(uint8_t hmac_sha_mode,
                     uint8_t *msg,
                     uint32_t msg_length,
                     uint8_t *key_var,
                     key_descriptor_t *key_info,
                     uint8_t *digest,
                     uint8_t *hmac_buffer);
#else
int32_t rsi_hmac_sha(uint8_t hmac_sha_mode,
                     uint8_t *msg,
                     uint32_t msg_length,
                     uint8_t *key,
                     uint32_t key_length,
                     uint8_t *digest,
                     uint8_t *hmac_buffer);
#endif
#ifdef CHIP_917B0
int32_t rsi_hmac_sha_pen(uint8_t hmac_sha_mode,
                         uint8_t *data,
                         uint16_t total_length,
                         uint16_t chunk_length,
                         key_descriptor_t *key_info,
                         uint8_t pending_flag,
                         uint8_t *digest);
#else
int32_t rsi_hmac_sha_pen(uint8_t hmac_sha_mode,
                         uint8_t *data,
                         uint16_t total_length,
                         uint16_t chunk_length,
                         uint32_t key_length,
                         uint8_t pending_flag,
                         uint8_t *digest);
#endif
#ifdef CHIP_917B0
int32_t rsi_aes_pen(uint16_t aes_mode,
                    uint16_t enc_dec,
                    uint8_t *msg,
                    uint16_t msg_length,
                    uint16_t chunk_length,
                    key_descriptor_t *key_info,
                    uint8_t *iv,
                    uint8_t aes_flags,
                    uint8_t *output);

int32_t rsi_aes(uint16_t aes_mode,
                uint16_t enc_dec,
                uint8_t *msg,
                uint16_t msg_length,
                key_descriptor_t *key_info,
                uint8_t *iv,
                uint8_t *output);
#else
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
#endif
int32_t rsi_exponentiation(uint8_t *prime,
                           uint32_t prime_length,
                           uint8_t *base,
                           uint32_t base_length,
                           uint8_t *exponent,
                           uint32_t exponent_length,
                           uint8_t *exp_result);
int32_t rsi_sha3(uint8_t mode, uint8_t *msg, uint16_t msg_length, uint8_t *digest);
int32_t rsi_shake(uint8_t mode, uint8_t *msg, uint16_t msg_length, uint8_t *digest);
int32_t rsi_sha3_shake_pen(uint8_t pad_char,
                           uint8_t mode,
                           uint8_t *msg,
                           uint16_t msg_length,
                           uint16_t chunk_len,
                           uint8_t pending_flag,
                           uint8_t *digest);
#ifdef CHIP_917B0
int32_t rsi_chachapoly(uint16_t chachapoly_mode,
                       uint16_t enc_dec,
                       uint16_t dma_use,
                       uint8_t *msg,
                       uint16_t msg_length,
                       key_descriptor_t *key_info,
                       uint8_t *nonce,
                       uint8_t *header_input,
                       uint16_t header_length,
                       uint8_t *output);
int32_t rsi_chachapoly_pen(uint16_t chachapoly_mode,
                           uint16_t enc_dec,
                           uint16_t dma_use,
                           uint8_t *msg,
                           uint16_t msg_length,
                           uint16_t chunk_len,
                           key_descriptor_t *key_info,
                           uint8_t *nonce,
                           uint8_t *header_input,
                           uint16_t header_length,
                           uint8_t chachapoly_flags,
                           uint8_t *output);
#else
int32_t rsi_chachapoly(uint16_t chachapoly_mode,
                       uint16_t enc_dec,
                       uint16_t dma_use,
                       uint8_t *msg,
                       uint16_t msg_length,
                       uint8_t *key_chacha,
                       uint8_t *keyr_in,
                       uint8_t *keys_in,
                       uint8_t *nonce,
                       uint8_t *header_input,
                       uint16_t header_length,
                       uint8_t *output);
int32_t rsi_chachapoly_pen(uint16_t chachapoly_mode,
                           uint16_t enc_dec,
                           uint16_t dma_use,
                           uint8_t *msg,
                           uint16_t msg_length,
                           uint16_t chunk_len,
                           uint8_t *key_chacha,
                           uint8_t *keyr_in,
                           uint8_t *keys_in,
                           uint8_t *nonce,
                           uint8_t *header_input,
                           uint16_t header_length,
                           uint8_t chachapoly_flags,
                           uint8_t *output);
#endif
#ifdef CHIP_917B0
int32_t rsi_gcm_pen(uint32_t gcm_mode,
                    uint8_t enc_dec,
                    uint16_t dma_use,
                    uint8_t *msg,
                    uint16_t msg_length,
                    uint16_t chunk_length,
                    key_descriptor_t *key_info,
                    uint8_t *iv,
                    uint16_t iv_sz,
                    uint8_t *header,
                    uint16_t header_length,
                    uint8_t gcm_flags,
                    uint8_t *output);
int32_t rsi_gcm(uint32_t gcm_mode,
                uint8_t enc_dec,
                uint16_t dma_use,
                uint8_t *msg,
                uint16_t msg_length,
                key_descriptor_t *key_info,
                uint8_t *iv,
                uint16_t iv_sz,
                uint8_t *header,
                uint16_t header_length,
                uint8_t *output);
#else
int32_t rsi_gcm_pen(uint8_t enc_dec,
                    uint16_t dma_use,
                    uint8_t *msg,
                    uint16_t msg_length,
                    uint16_t chunk_length,
                    uint8_t *key,
                    uint16_t key_length,
                    uint8_t *iv,
                    uint16_t iv_sz,
                    uint8_t *header,
                    uint16_t header_length,
                    uint8_t gcm_flags,
                    uint8_t *output);
int32_t rsi_gcm(uint8_t enc_dec,
                uint16_t dma_use,
                uint8_t *msg,
                uint16_t msg_length,
                uint8_t *key,
                uint16_t key_length,
                uint8_t *iv,
                uint16_t iv_sz,
                uint8_t *header,
                uint16_t header_length,
                uint8_t *output);
#endif
#ifdef CHIP_917B0
int32_t rsi_ccm_pen(uint8_t enc_dec,
                    uint8_t *msg,
                    uint16_t msg_length,
                    uint16_t chunk_length,
                    key_descriptor_t *key_info,
                    uint8_t *iv,
                    uint16_t iv_sz,
                    uint8_t *tag,
                    uint16_t tag_len,
                    uint8_t *ad,
                    uint16_t ad_len,
                    uint8_t ccm_flags,
                    uint8_t *output);
int32_t rsi_ccm(uint8_t enc_dec,
                uint8_t *msg,
                uint16_t msg_length,
                key_descriptor_t *key_info,
                uint8_t *iv,
                uint16_t iv_sz,
                uint8_t *tag,
                uint16_t tag_len,
                uint8_t *ad,
                uint16_t ad_len,
                uint8_t *output);
#else
int32_t rsi_ccm_pen(uint8_t enc_dec,
                    uint8_t *msg,
                    uint16_t msg_length,
                    uint16_t chunk_length,
                    uint8_t *key,
                    uint16_t key_length,
                    uint8_t *iv,
                    uint16_t iv_sz,
                    uint8_t *tag,
                    uint16_t tag_len,
                    uint8_t *ad,
                    uint16_t ad_len,
                    uint8_t ccm_flags,
                    uint8_t *output);
int32_t rsi_ccm(uint8_t enc_dec,
                uint8_t *msg,
                uint16_t msg_length,
                uint8_t *key,
                uint16_t key_length,
                uint8_t *iv,
                uint16_t iv_sz,
                uint8_t *tag,
                uint16_t tag_len,
                uint8_t *ad,
                uint16_t ad_len,
                uint8_t *output);
#endif
#ifdef CHIP_917B0
int32_t rsi_wrap(key_descriptor_wrap_t *key_info, uint8_t *output);
void initialize_key_variable(uint32_t key_type_var,
                             uint32_t key_size_var,
                             uint32_t key_slot_var,
                             uint32_t wrap_iv_mode_var,
                             uint32_t reserved_var);
void wrap_key_info_initialization(key_descriptor_wrap_t *key_info, uint8_t *key_var, uint8_t *wrap_iv_var);
void crypto_key_info_initialization(key_descriptor_t *key_info, uint8_t *key_var, uint8_t *wrap_iv_var);
void crypto_key_info_hmac_initialization(key_descriptor_t *key_info, uint8_t *key_var, uint8_t *wrap_iv_var);
#endif

int32_t trng_init(uint32_t *trng_key, uint32_t *trng_test_data, uint16_t input_length, uint32_t *output);
int32_t trng_get_random_num(uint32_t *random_number, uint16_t length);
int32_t trng_program_key(uint32_t *trng_key, uint16_t key_length);
int32_t trng_entropy(void);
int32_t sl_attestation_get_token(uint8_t *token, uint16_t length, uint32_t *nonce);
#endif
#else
//====================================================== NORMAL CRYPTO ==================================================================================== //
#ifndef RSI_CRYPTO_H
#define RSI_CRYPTO_H
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
#define SHA_224 5

#define SHAKE_128 21
#define SHAKE_256 17
#define SHA3_224  18
#define SHA3_256  17
#define SHA3_384  13
#define SHA3_512  9

#define AES               2
#define SHA               4
#define HMAC_SHA          5
#define DH                14
#define ECDH              15
#define SHA3              16
#define CHACHAPOLY        17
#define GCM               18
#define SHAKE             19
#define WRAP              20
#define TRNG              21
#define RSI_BUILT_IN_KEYS 22
#define ATTESTATION       30
#define CCM               31

#define ECDH_PM  1
#define ECDH_PA  2
#define ECDH_PS  3
#define ECDH_PD  4
#define ECDH_PAF 5

#define SHA_1_DIGEST_LEN     20
#define SHA_256_DIGEST_LEN   32
#define SHA_384_DIGEST_LEN   48
#define SHA_512_DIGEST_LEN   64
#define SHA_224_DIGEST_LEN   28
#define SHAKE_128_DIGEST_LEN 16

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

#define GCM_ENCRYPTION 0
#define GCM_DECRYPTION 1

#define CCM_ENCRYPTION              0
#define CCM_DECRYPTION              1
#define CCM_IV_MIN_SIZE_BYTES       7
#define CCM_IV_MAX_SIZE_BYTES       13
#define CCM_IV_BUFF_LEN_BYTES       16  // CCM_IV_MAX_SIZE_BYTES + 3 Bytes for padding.
#define CCM_MAX_HEADER_LENGTH       128 // Limiting header length to 128.
#define CCM_KEY_SIZE_128            16
#define CCM_KEY_SIZE_192            24
#define CCM_KEY_SIZE_256            32
#define MAX_DATA_SIZE_BYTES_FOR_CCM 1200

#define GCM_MODE       0
#define CMAC_MODE      1

//! AES Block size
#define AES_BLOCK_SIZE 16

#define AES_KEY_SIZE_128 16
#define AES_KEY_SIZE_192 24
#define AES_KEY_SIZE_256 32

#define GCM_KEY_SIZE_128 16
#define GCM_KEY_SIZE_192 24
#define GCM_KEY_SIZE_256 32

#define GCM_IV_SIZE_IN_BYTES               12
#define ECDH_192                           1
#define ECDH_224                           2
#define ECDH_256                           4

//! ECDH curve types
#define ECDH_CURVE_P                       0
#define ECDH_CURVE_K                       1
#define ECDH_CURVE_B                       2

//!DMA Support
#define DMA_ENABLE                         1
#define DMA_DISABLE                        0

//! PAD Char
#define PAD_CHAR_SHA3                      0x06
#define PAD_CHAR_SHAKE                     0x1F
#define CHACHA20POLY1305                   0
#define CHACHA20                           1
#define CHACHAPOLY_MODE_POLY1305_KEYR_KEYS 2
#define POLY1305_MODE                      3

#define TAG_SIZE 16

#define CHACHAPOLY_ENCRYPTION 0
#define CHACHAPOLY_DECRYPTION 1

#define KEYR_KEYS_LENGTH                   8
#define KEY_CHACHA_SIZE                    32
#define KEYR_SIZE                          16
#define KEYS_SIZE                          16
#define NONCE_SIZE                         16
#define MAX_DATA_SIZE_BYTES                1400 /*Data size*/
#define MAX_DATA_SIZE_BYTES_FOR_CHACHAPOLY 1200 /*Data size for chachapoly*/

#define TRNG_INIT       1
#define TRNG_ENTROPY    2
#define TRNG_KEY        3
#define TRNG_GENERATION 4

#define TRNG_INIT_MSG_LENGTH 16
#define TRNG_KEY_SIZE        4
#define TRNG_TEST_DATA_SIZE  64

//! Types of keys
#define RSI_TRANSPARENT_KEY  0
#define RSI_WRAPPED_KEY      1
#define RSI_BUILT_IN_KEY     2

//! Wrap IV mode
#define PLAIN_TEXT_MODE      0
#define WRAP_IV_ECB_MODE     0
#define WRAP_IV_CBC_MODE     1
#define WRAP_IV_SIZE         16

#define NONCE_DATA_SIZE                    32

//! Key slot for Built in keys
#define GET_M4_OTA_KEY                     1
#define GET_M4_PUBLIC_KEY                  2

//! Key size for Built in keys
#define M4_OTA_KEY_SIZE                    32
#define M4_PUBLIC_KEY_SIZE                 96

//! Error Codes
#define RSI_ERROR_INVALID_SIGNATURE        0xCC9A // here 0xCC-Crypto Code, 0x9A= -102.
#define RSI_ERROR_CRYPTO_INVALID_PARAMETER 0xCCFE // here 0xCC-Crypto Code, 0xFE= -2.
/******************************************************
 * *                    Constants
 * ******************************************************/
/******************************************************
 * *                   Enumerations
 * ******************************************************/
/******************************************************
 * *                 Type Definitions
 * ******************************************************/

#ifdef CHIP_917B0
typedef struct crypto_key_s {
  //! For built-in key
  uint32_t key_slot;
  //!IV mode 0-> ECB; 1-> CBC
  uint32_t wrap_iv_mode;
  //! IV for CBC mode
  uint8_t wrap_iv[WRAP_IV_SIZE];
  //! Key data wrapped/ Plain text
  uint8_t key_buffer[32];
} crypto_key_t;

typedef struct sapi_key_info_s {
  uint32_t key_size;
  crypto_key_t key_spec;
} sapi_key_info_t;

typedef struct key_descriptor_s {
  uint32_t key_type;
  uint32_t reserved;
  sapi_key_info_t key_detail;
} key_descriptor_t;

typedef struct key_descriptor_wrap_s {
  uint32_t key_type;
  uint32_t reserved;
  uint32_t key_size;
  //!IV mode 0-> ECB; 1-> CBC
  uint32_t wrap_iv_mode;
  //! IV for CBC mode
  uint8_t wrap_iv[WRAP_IV_SIZE];
  //! Key data wrapped/ Plain text
  uint8_t key_buffer[1400];
} key_descriptor_wrap_t;

typedef struct wrap_req_s {
  uint8_t algorithm_type;
  uint8_t wrap_flags;
  uint16_t current_chunk_length;
  key_descriptor_wrap_t key_info;
} wrap_req_t;

//! Built-in keys strcuture
typedef struct built_in_key_req_s {
  uint16_t algorithm_type;
  uint16_t key_size;
  uint32_t key_slot;
} built_in_key_req_t;
#endif

// TRNG Request Frames Structures
typedef struct rsi_trng_req_s {
  uint8_t algorithm_type;
  uint8_t algorithm_sub_type;
  uint16_t total_msg_length;
  uint32_t trng_key[TRNG_KEY_SIZE];
  uint32_t msg[TRNG_TEST_DATA_SIZE];
} rsi_trng_req_t;

// Attestation token Request Frames Structures
typedef struct rsi_token_req_s {
  uint8_t algorithm_type;
  uint16_t total_msg_length;
  uint32_t msg[NONCE_DATA_SIZE];
} rsi_token_req_t;

// SHA Request Frames Structures
typedef struct rsi_sha_req_s {
  uint16_t algorithm_type;
  uint8_t algorithm_sub_type;
  uint8_t sha_flags;
  uint16_t total_msg_length;
  uint16_t current_chunk_length;
  uint8_t msg[1400];
} rsi_sha_req_t;

typedef struct rsi_chachapoly_req_s {
  uint16_t algorithm_type;
  uint8_t algorithm_sub_type;
  uint8_t chachapoly_flags;
  uint16_t total_msg_length;
  uint16_t header_length;
  uint16_t current_chunk_length;
  uint16_t encrypt_decryption;
  uint32_t dma_use;
  uint8_t nonce[16];
#ifdef CHIP_917B0
  key_descriptor_t key_info;
#else
  uint8_t key_chacha[32];
  uint8_t keyr_in[16];
  uint8_t keys_in[16];
#endif
  uint8_t header_input[128];
  uint8_t msg[1200];
} rsi_chachapoly_req_t;

typedef struct rsi_hmac_sha_req_s {
  uint16_t algorithm_type;
  uint8_t algorithm_sub_type;
  uint8_t hmac_sha_flags;
  uint16_t total_msg_length;
  uint16_t current_chunk_length;
#ifdef CHIP_917B0
  key_descriptor_t key_info;
#else
  uint32_t key_length;
#endif
  uint8_t hmac_data[1400];
} rsi_hmac_sha_req_t;

typedef struct rsi_aes_req_s {
  uint16_t algorithm_type;
  uint8_t algorithm_sub_type;
  uint8_t aes_flags;
  uint16_t total_msg_length;
  uint16_t current_chunk_length;
  uint32_t encrypt_decryption;
#ifdef CHIP_917B0
  key_descriptor_t key_info;
#else
  uint32_t key_length;
  uint8_t key[32];
#endif
  uint8_t IV[16];
  uint8_t msg[1400];
} rsi_aes_req_t;

typedef struct rsi_gcm_req_s {
  uint16_t algorithm_type;
  uint8_t gcm_flags;
  uint8_t encrypt_decryption;
  uint16_t total_msg_length;
  uint16_t current_chunk_length;
  uint16_t header_length;
  uint16_t dma_use;
#ifdef CHIP_917B0
  uint32_t gcm_mode;
  key_descriptor_t key_info;
#else
  uint32_t key_length;
  uint8_t key[32];
#endif
  uint8_t IV[12];
  uint8_t header[128];
  uint8_t msg[1400];
} rsi_gcm_req_t;

typedef struct rsi_ccm_req_s {
  uint16_t algorithm_type;
  uint8_t ccm_flags;
  uint8_t nonce_len;
  uint16_t encrypt_decryption;
  uint16_t total_msg_length;
  uint16_t current_chunk_length;
  uint16_t ad_len;
  uint32_t tag_len;
#ifdef CHIP_917B0
  key_descriptor_t key_info;
#else
  uint32_t key_length;
  uint8_t key[AES_KEY_SIZE_256];
#endif
  uint8_t nonce[CCM_IV_BUFF_LEN_BYTES];
  uint8_t ad[CCM_MAX_HEADER_LENGTH];
  uint8_t tag[TAG_SIZE];
  uint8_t msg[MAX_DATA_SIZE_BYTES_FOR_CCM];
} rsi_ccm_req_t;

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
  uint8_t ecdh_curve_type;
  uint32_t affinity;
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
  uint8_t ecdh_curve_type;
  uint8_t sx[32];
  uint8_t sy[32];
  uint8_t sz[32];
} rsi_ecdh_affine_req_t;

#ifdef CHIP_917B0
typedef struct key_var_initialize_s {
  uint32_t key_type;
  uint32_t key_size;
  uint32_t key_slot;
  uint32_t wrap_iv_mode;
  uint32_t reserved;
} key_var_initialize_t;
#endif
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
#ifdef CHIP_917B0
int32_t rsi_hmac_sha(uint8_t hmac_sha_mode,
                     uint8_t *msg,
                     uint32_t msg_length,
                     uint8_t *key_var,
                     key_descriptor_t *key_info,
                     uint8_t *digest,
                     uint8_t *hmac_buffer);
#else
int32_t rsi_hmac_sha(uint8_t hmac_sha_mode,
                     uint8_t *msg,
                     uint32_t msg_length,
                     uint8_t *key,
                     uint32_t key_length,
                     uint8_t *digest,
                     uint8_t *hmac_buffer);
#endif
#ifdef CHIP_917B0
int32_t rsi_hmac_sha_pen(uint8_t hmac_sha_mode,
                         uint8_t *data,
                         uint16_t total_length,
                         uint16_t chunk_length,
                         key_descriptor_t *key_info,
                         uint8_t pending_flag,
                         uint8_t *digest);
#else
int32_t rsi_hmac_sha_pen(uint8_t hmac_sha_mode,
                         uint8_t *data,
                         uint16_t total_length,
                         uint16_t chunk_length,
                         uint32_t key_length,
                         uint8_t pending_flag,
                         uint8_t *digest);
#endif
#ifdef CHIP_917B0
int32_t rsi_aes_pen(uint16_t aes_mode,
                    uint16_t enc_dec,
                    uint8_t *msg,
                    uint16_t msg_length,
                    uint16_t chunk_length,
                    key_descriptor_t *key_info,
                    uint8_t *iv,
                    uint8_t aes_flags,
                    uint8_t *output);

int32_t rsi_aes(uint16_t aes_mode,
                uint16_t enc_dec,
                uint8_t *msg,
                uint16_t msg_length,
                key_descriptor_t *key_info,
                uint8_t *iv,
                uint8_t *output);
#else
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
#endif
int32_t rsi_exponentiation(uint8_t *prime,
                           uint32_t prime_length,
                           uint8_t *base,
                           uint32_t base_length,
                           uint8_t *exponent,
                           uint32_t exponent_length,
                           uint8_t *exp_result);
int32_t rsi_sha3(uint8_t mode, uint8_t *msg, uint16_t msg_length, uint8_t *digest);
int32_t rsi_shake(uint8_t mode, uint8_t *msg, uint16_t msg_length, uint8_t *digest);
int32_t rsi_sha3_shake_pen(uint8_t pad_char,
                           uint8_t mode,
                           uint8_t *msg,
                           uint16_t msg_length,
                           uint16_t chunk_len,
                           uint8_t pending_flag,
                           uint8_t *digest);
#ifdef CHIP_917B0
int32_t rsi_chachapoly(uint16_t chachapoly_mode,
                       uint16_t enc_dec,
                       uint16_t dma_use,
                       uint8_t *msg,
                       uint16_t msg_length,
                       key_descriptor_t *key_info,
                       uint8_t *nonce,
                       uint8_t *header_input,
                       uint16_t header_length,
                       uint8_t *output);
int32_t rsi_chachapoly_pen(uint16_t chachapoly_mode,
                           uint16_t enc_dec,
                           uint16_t dma_use,
                           uint8_t *msg,
                           uint16_t msg_length,
                           uint16_t chunk_len,
                           key_descriptor_t *key_info,
                           uint8_t *nonce,
                           uint8_t *header_input,
                           uint16_t header_length,
                           uint8_t chachapoly_flags,
                           uint8_t *output);
#else
int32_t rsi_chachapoly(uint16_t chachapoly_mode,
                       uint16_t enc_dec,
                       uint16_t dma_use,
                       uint8_t *msg,
                       uint16_t msg_length,
                       uint8_t *key_chacha,
                       uint8_t *keyr_in,
                       uint8_t *keys_in,
                       uint8_t *nonce,
                       uint8_t *header_input,
                       uint16_t header_length,
                       uint8_t *output);
int32_t rsi_chachapoly_pen(uint16_t chachapoly_mode,
                           uint16_t enc_dec,
                           uint16_t dma_use,
                           uint8_t *msg,
                           uint16_t msg_length,
                           uint16_t chunk_len,
                           uint8_t *key_chacha,
                           uint8_t *keyr_in,
                           uint8_t *keys_in,
                           uint8_t *nonce,
                           uint8_t *header_input,
                           uint16_t header_length,
                           uint8_t chachapoly_flags,
                           uint8_t *output);
#endif
#ifdef CHIP_917B0
int32_t rsi_gcm_pen(uint32_t gcm_mode,
                    uint8_t enc_dec,
                    uint16_t dma_use,
                    uint8_t *msg,
                    uint16_t msg_length,
                    uint16_t chunk_length,
                    key_descriptor_t *key_info,
                    uint8_t *iv,
                    uint16_t iv_sz,
                    uint8_t *header,
                    uint16_t header_length,
                    uint8_t gcm_flags,
                    uint8_t *output);
int32_t rsi_gcm(uint32_t gcm_mode,
                uint8_t enc_dec,
                uint16_t dma_use,
                uint8_t *msg,
                uint16_t msg_length,
                key_descriptor_t *key_info,
                uint8_t *iv,
                uint16_t iv_sz,
                uint8_t *header,
                uint16_t header_length,
                uint8_t *output);
#else
int32_t rsi_gcm_pen(uint8_t enc_dec,
                    uint16_t dma_use,
                    uint8_t *msg,
                    uint16_t msg_length,
                    uint16_t chunk_length,
                    uint8_t *key,
                    uint16_t key_length,
                    uint8_t *iv,
                    uint16_t iv_sz,
                    uint8_t *header,
                    uint16_t header_length,
                    uint8_t gcm_flags,
                    uint8_t *output);
int32_t rsi_gcm(uint8_t enc_dec,
                uint16_t dma_use,
                uint8_t *msg,
                uint16_t msg_length,
                uint8_t *key,
                uint16_t key_length,
                uint8_t *iv,
                uint16_t iv_sz,
                uint8_t *header,
                uint16_t header_length,
                uint8_t *output);
#endif
#ifdef CHIP_917B0
int32_t rsi_ccm_pen(uint8_t enc_dec,
                    uint8_t *msg,
                    uint16_t msg_length,
                    uint16_t chunk_length,
                    key_descriptor_t *key_info,
                    uint8_t *iv,
                    uint16_t iv_sz,
                    uint8_t *tag,
                    uint16_t tag_len,
                    uint8_t *ad,
                    uint16_t ad_len,
                    uint8_t ccm_flags,
                    uint8_t *output);
int32_t rsi_ccm(uint8_t enc_dec,
                uint8_t *msg,
                uint16_t msg_length,
                key_descriptor_t *key_info,
                uint8_t *iv,
                uint16_t iv_sz,
                uint8_t *tag,
                uint16_t tag_len,
                uint8_t *ad,
                uint16_t ad_len,
                uint8_t *output);
#else
int32_t rsi_ccm_pen(uint8_t enc_dec,
                    uint8_t *msg,
                    uint16_t msg_length,
                    uint16_t chunk_length,
                    uint8_t *key,
                    uint16_t key_length,
                    uint8_t *iv,
                    uint16_t iv_sz,
                    uint8_t *tag,
                    uint16_t tag_len,
                    uint8_t *ad,
                    uint16_t ad_len,
                    uint8_t ccm_flags,
                    uint8_t *output);
int32_t rsi_ccm(uint8_t enc_dec,
                uint8_t *msg,
                uint16_t msg_length,
                uint8_t *key,
                uint16_t key_length,
                uint8_t *iv,
                uint16_t iv_sz,
                uint8_t *tag,
                uint16_t tag_len,
                uint8_t *ad,
                uint16_t ad_len,
                uint8_t *output);
#endif
#ifdef CHIP_917B0
int32_t rsi_wrap(key_descriptor_wrap_t *key_info, uint8_t *output);
void initialize_key_variable(uint32_t key_type_var,
                             uint32_t key_size_var,
                             uint32_t key_slot_var,
                             uint32_t wrap_iv_mode_var,
                             uint32_t reserved_var);
void wrap_key_info_initialization(key_descriptor_wrap_t *key_info, uint8_t *key_var, uint8_t *wrap_iv_var);
void crypto_key_info_initialization(key_descriptor_t *key_info, uint8_t *key_var, uint8_t *wrap_iv_var);
void crypto_key_info_hmac_initialization(key_descriptor_t *key_info, uint8_t *key_var, uint8_t *wrap_iv_var);
#endif

int32_t trng_init(uint32_t *trng_key, uint32_t *trng_test_data, uint16_t input_length, uint32_t *output);
int32_t trng_get_random_num(uint32_t *random_number, uint16_t length);
int32_t trng_program_key(uint32_t *trng_key, uint16_t key_length);
int32_t trng_entropy(void);
int32_t sl_attestation_get_token(uint8_t *token, uint16_t length, uint32_t *nonce);
void reverse_8(unsigned char *xx, int no_digits);
#endif
#endif
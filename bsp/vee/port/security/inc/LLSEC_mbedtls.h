/*
 * C
 *
 * Copyright 2021-2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief Security natives mbedtls structs.
 * @author MicroEJ Developer Team
 * @version 2.0.1
 */

#ifndef LLSEC_MBEDTLS_H
#define LLSEC_MBEDTLS_H

#include <stdint.h>
#include <microej_async_worker.h>
#include "LLSEC_configuration.h"
// This module is compatible with different versions of mbedtls. Include version.h to check compatibility
#include "mbedtls/version.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef MBEDTLS_VERSION_MAJOR
#error MBEDTLS_VERSION_MAJOR must be defined.
#endif

#ifndef MBEDTLS_VERSION_MINOR
#error MBEDTLS_VERSION_MINOR must be defined.
#endif

#ifndef MBEDTLS_VERSION_NUMBER
#error MBEDTLS_VERSION_NUMBER must be defined.
#endif

#if (MBEDTLS_VERSION_MAJOR < 2) || (MBEDTLS_VERSION_MAJOR > 3)
#error LLSEC_MBEDTLS is compatible with MbedTLS 2 and 3
#endif

#define XSTR(x) #x
#define STR(x)  XSTR(x)

/**
 * MbedTLS does not define a success constant. Define one here to avoir magic
 * numbers */
#define LLSEC_MBEDTLS_SUCCESS                     (0)

/**
 * Internal use constant to signal invalid arguments were passed. This is used
 * to build meaningful exceptions */
#define LLSEC_INVALID_ARGUMENT                    (-2)

/**
 * Internal use constant to signal an error with mbedtls. The mbedtls error is
 * retrieved by another mean and used to build meaningful exception */
#define LLSEC_MBEDTLS_ERR                         (-3)

/**
 * Parse a MbedTLS error and return only the 'high-level' part.
 * cf. <mbedtls/error_common.h>
 */
#define MBEDTLS_MODULE_ERROR(err)                 (-((-(err)) & 0xFF80))

typedef enum {
	TYPE_RSA   = 6,   //EVP_PKEY_RSA,
	TYPE_ECDSA = 408, //EVP_PKEY_EC,
} LLSEC_pub_key_type;

/*key must be mbedtls_rsa_context or mbedtls_ecdsa_context TYPE*/
typedef struct {
	LLSEC_pub_key_type type;
	void *key; /*mbedtls_rsa_context or mbedtls_ecdsa_context*/
} LLSEC_priv_key;

typedef struct {
	LLSEC_pub_key_type type;
	void *key; /*mbedtls_rsa_context or mbedtls_ecdsa_context*/
} LLSEC_pub_key;

typedef struct {
	unsigned char *key;
	int32_t key_length;
} LLSEC_secret_key;

/**
 * Generate a `random` string for personalization token of ctr_drbg
 *
 * @note To be removed when entropy pool has been properly set up.
 * Personalization doesn't need to be random. This add a dependency to stdlib's
 * `random()`
 *
 * @param[out] str   Buffer to be filled with random NULL terminated string
 * @param[in] buflen Length of the buffer
 */
void llsec_mbedtls_gen_random_str(char *str, size_t buflen);

/**
 * Wraps llsec_mbedtls_gen_random_str with malloc
 *
 * @param[in] length Length of the resulting buffer (strlen() + 1)
 * @return Allocated random buffer, NULL on failure
 *
 * @note to be removed when all calls ported to llsec_mbedtls_gen_random_str().
 * This adds an unwanted failure case, buffer are short enough that they can
 * live on the stack.
 */
char *llsec_gen_random_str_internal(int length);

extern MICROEJ_ASYNC_WORKER_handle_t llsec_worker;

/* Forward declaration of types refered only by pointer in the async worker structures
 * This is prefereable as the only requirement in this header is to calculate the size of
 * the union.
 */
typedef struct LLSEC_KEY_PAIR_GENERATOR_algorithm LLSEC_KEY_PAIR_GENERATOR_algorithm;
typedef struct LLSEC_SECRET_KEY_FACTORY_IMPL_algorithm LLSEC_SECRET_KEY_FACTORY_IMPL_algorithm;
typedef struct LLSEC_SIG_algorithm LLSEC_SIG_algorithm;

struct LLSEC_KEY_PAIR_GENERATOR_generateKeyPair_params {
	LLSEC_priv_key *key;
	const LLSEC_KEY_PAIR_GENERATOR_algorithm *algorithm;
	int32_t rsa_key_size;
	int32_t rsa_public_exponent;
	char *ec_curve_stdname;
	int rc;
	int mbedtls_rc;
	const char *reason;
	char ec_curve_stdname_buffer[LLSEC_ECP_CURVE_NAME_BUFFER_LEN];
};

struct LLSEC_SECRET_KEY_FACTORY_get_key_data_params {
	const LLSEC_SECRET_KEY_FACTORY_IMPL_algorithm *algorithm;
	uint8_t *password;
	int32_t password_length;
	uint8_t *salt;
	int32_t salt_length;
	int32_t iterations;
	int32_t key_length;
	LLSEC_secret_key *key;
	int32_t rc;
	const char *reason;
};

struct LLSEC_SIG_sign_params {
	LLSEC_SIG_algorithm *algorithm;
	uint8_t *signature;
	int32_t signature_length;
	LLSEC_priv_key *private_key;
	uint8_t *digest;
	int32_t digest_length;
	int rc;
	int mbedtls_rc;
};

struct LLSEC_SIG_verify_params {
	LLSEC_SIG_algorithm *algorithm;
	uint8_t *signature;
	int32_t signature_length;
	LLSEC_pub_key *public_key;
	uint8_t *digest;
	int32_t digest_length;
	int rc;
	int mbedtls_rc;
};

struct LLSEC_RSA_CIPHER_IMPL_decrypt_params {
	int32_t transformation_id;
	int32_t native_id;
	uint8_t *buffer;
	int32_t buffer_length;
	uint8_t *output;
	int32_t rc;
	int mbedtls_rc;
};

struct LLSEC_RSA_CIPHER_IMPL_encrypt_params {
	int32_t transformation_id;
	int32_t native_id;
	uint8_t *buffer;
	int32_t buffer_length;
	uint8_t *output;
	int32_t rc;
	int mbedtls_rc;
};

union LLSEC_worker_params {
	struct LLSEC_KEY_PAIR_GENERATOR_generateKeyPair_params KEY_PAIR_GENERATOR_generateKeyPair;
	struct LLSEC_SECRET_KEY_FACTORY_get_key_data_params SECRET_KEY_FACTORY_get_key_data;
	struct LLSEC_SIG_sign_params SIG_sign;
	struct LLSEC_SIG_verify_params SIG_verify;
	struct LLSEC_RSA_CIPHER_IMPL_decrypt_params RSA_CIPHER_decrypt;
	struct LLSEC_RSA_CIPHER_IMPL_encrypt_params RSA_CIPHER_encrypt;
};

#ifdef __cplusplus
}
#endif

#endif /* LLSEC_MBEDTLS_H */

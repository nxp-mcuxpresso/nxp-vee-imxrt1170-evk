/*
 * C
 *
 * Copyright 2021-2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief MicroEJ Security low level API implementation for MbedTLS Library.
 * @author MicroEJ Developer Team
 * @version 2.0.1
 */

// set to 1 to enable profiling
#define LLSEC_PROFILE   0

#include <LLSEC_KEY_FACTORY_impl.h>
#include <LLSEC_CONSTANTS.h>
#include <LLSEC_mbedtls.h>
#include <sni.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "mbedtls/platform.h"
#include "mbedtls/pk.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"

// cppcheck-suppress misra-c2012-8.9 // Define here for code readability even if it called once in this file.
static const char *pkcs8_format = "PKCS#8";

// cppcheck-suppress misra-c2012-8.9 // Define here for code readability even if it called once in this file.
static const char *x509_format = "X.509";

typedef int32_t (*LLSEC_KEY_FACTORY_get_private_key_data)(LLSEC_priv_key *priv_key, uint8_t *encoded_key,
                                                          int32_t encoded_key_length);
typedef int32_t (*LLSEC_KEY_FACTORY_get_public_key_data)(LLSEC_pub_key *pub_key, uint8_t *encoded_key,
                                                         int32_t encoded_key_length);
typedef void (*LLSEC_KEY_FACTORY_key_close)(void *native_id);

typedef struct {
	char *name;
	LLSEC_KEY_FACTORY_get_private_key_data get_private_key_data;
	LLSEC_KEY_FACTORY_get_public_key_data get_public_key_data;
	LLSEC_KEY_FACTORY_key_close private_key_close;
	LLSEC_KEY_FACTORY_key_close public_key_close;
} LLSEC_KEY_FACTORY_algorithm;

static int32_t LLSEC_KEY_FACTORY_RSA_mbedtls_get_private_key_data(LLSEC_priv_key *priv_key, uint8_t *encoded_key,
                                                                  int32_t encoded_key_length);
static int32_t LLSEC_KEY_FACTORY_RSA_mbedtls_get_public_key_data(LLSEC_pub_key *pub_key, uint8_t *encoded_key,
                                                                 int32_t encoded_key_length);
static int32_t LLSEC_KEY_FACTORY_EC_mbedtls_get_private_key_data(LLSEC_priv_key *priv_key, uint8_t *encoded_key,
                                                                 int32_t encoded_key_length);
static int32_t LLSEC_KEY_FACTORY_EC_mbedtls_get_public_key_data(LLSEC_pub_key *pub_key, uint8_t *encoded_key,
                                                                int32_t encoded_key_length);
static void LLSEC_KEY_FACTORY_mbedtls_private_key_close(void *native_id);
static void LLSEC_KEY_FACTORY_mbedtls_public_key_close(void *native_id);

// cppcheck-suppress misra-c2012-8.9 // Define here for code readability even if it called once in this file.
static const LLSEC_KEY_FACTORY_algorithm available_key_algorithms[2] = {
	{
		.name = "RSA",
		.get_private_key_data = LLSEC_KEY_FACTORY_RSA_mbedtls_get_private_key_data,
		.get_public_key_data = LLSEC_KEY_FACTORY_RSA_mbedtls_get_public_key_data,
		.private_key_close = LLSEC_KEY_FACTORY_mbedtls_private_key_close,
		.public_key_close = LLSEC_KEY_FACTORY_mbedtls_public_key_close
	},
	{
		.name = "EC",
		.get_private_key_data = LLSEC_KEY_FACTORY_EC_mbedtls_get_private_key_data,
		.get_public_key_data = LLSEC_KEY_FACTORY_EC_mbedtls_get_public_key_data,
		.private_key_close = LLSEC_KEY_FACTORY_mbedtls_private_key_close,
		.public_key_close = LLSEC_KEY_FACTORY_mbedtls_public_key_close
	}
};

static int32_t LLSEC_KEY_FACTORY_RSA_mbedtls_get_private_key_data(LLSEC_priv_key *priv_key, uint8_t *encoded_key,
                                                                  int32_t encoded_key_length) {
	LLSEC_KEY_FACTORY_DEBUG_TRACE("%s (key = %p) \n", __func__, priv_key);
	LLSEC_KEY_FACTORY_DEBUG_TRACE("encLen %d, strlen: %zu, enc:%s  \n", (int)encoded_key_length,
	                              strlen((char *)encoded_key), encoded_key);

	int return_code = LLSEC_SUCCESS;
	int mbedtls_rc = LLSEC_MBEDTLS_SUCCESS;

	priv_key->type = TYPE_RSA;

	mbedtls_pk_context pk;
	mbedtls_pk_init(&pk);

#if (MBEDTLS_VERSION_MAJOR == 3)
	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
	const char *pers = llsec_gen_random_str_internal(8);

	mbedtls_entropy_init(&entropy);
	mbedtls_ctr_drbg_init(&ctr_drbg);

	mbedtls_rc = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *)pers,
	                                   strlen(pers));
	if (LLSEC_MBEDTLS_SUCCESS != mbedtls_rc) {
		int sni_rc = SNI_throwNativeException(mbedtls_rc, "mbedtls_ctr_drbg_seed failed");
		LLSEC_ASSERT(sni_rc == SNI_OK);
		return_code = LLSEC_ERROR;
	}
#endif

	if (LLSEC_SUCCESS == return_code) {
#if (MBEDTLS_VERSION_MAJOR == 2)
		mbedtls_rc = mbedtls_pk_parse_key(&pk, encoded_key, encoded_key_length, NULL, 0);
#elif (MBEDTLS_VERSION_MAJOR == 3)
		mbedtls_rc = mbedtls_pk_parse_key(&pk, encoded_key, encoded_key_length, NULL, 0, mbedtls_ctr_drbg_random,
		                                  &ctr_drbg);
#else
		#error "Unsupported mbedTLS major version"
#endif
		if (LLSEC_MBEDTLS_SUCCESS != mbedtls_rc) {
			LLSEC_KEY_FACTORY_DEBUG_TRACE("mbedtls_pk_parse_key failed (rc = %d)", mbedtls_rc);
			return_code = LLSEC_ERROR;
		}
	}

	if (LLSEC_SUCCESS == return_code) {
		priv_key->key = (char *)mbedtls_pk_rsa(pk);
		if (NULL == priv_key->key) {
			int sni_rc = SNI_throwNativeException(LLSEC_ERROR, "RSA context extraction failed");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			return_code = LLSEC_ERROR;
		}
	}

	if (LLSEC_SUCCESS == return_code) {
		void *native_id = (void *)priv_key;
		if (SNI_OK != SNI_registerResource(native_id, (SNI_closeFunction)LLSEC_KEY_FACTORY_mbedtls_private_key_close,
		                                   NULL)) {
			int sni_rc = SNI_throwNativeException(LLSEC_ERROR, "Can't register SNI native resource");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			mbedtls_rsa_free((mbedtls_rsa_context *)priv_key->key);
			return_code = LLSEC_ERROR;
		} else {
			// cppcheck-suppress misra-c2012-11.6 // Abstract data type for SNI usage
			return_code = (int32_t)native_id;
		}
	}

#if (MBEDTLS_VERSION_MAJOR == 3)
	mbedtls_ctr_drbg_free(&ctr_drbg);
	mbedtls_entropy_free(&entropy);
	// cppcheck-suppress misra-c2012-11.8 // Cast for matching free function signature
	mbedtls_free((void *)pers);
#endif

	LLSEC_KEY_FACTORY_DEBUG_TRACE("%s (rc = %d)\n", __func__, return_code);
	return return_code;
}

static int32_t LLSEC_KEY_FACTORY_RSA_mbedtls_get_public_key_data(LLSEC_pub_key *pub_key, uint8_t *encoded_key,
                                                                 int32_t encoded_key_length) {
	LLSEC_KEY_FACTORY_DEBUG_TRACE("%s (key = %p) \n", __func__, pub_key);

	int return_code = LLSEC_SUCCESS;
	int mbedtls_rc = LLSEC_MBEDTLS_SUCCESS;

	pub_key->type = TYPE_RSA;

	mbedtls_pk_context pk;
	mbedtls_pk_init(&pk);
	mbedtls_rc = mbedtls_pk_parse_public_key(&pk, encoded_key, encoded_key_length);
	if (LLSEC_MBEDTLS_SUCCESS != mbedtls_rc) {
		LLSEC_KEY_FACTORY_DEBUG_TRACE("mbedtls_pk_parse_public_key failed (rc = %d)", mbedtls_rc);
		return_code = LLSEC_ERROR;
	}

	if (LLSEC_SUCCESS == return_code) {
		pub_key->key = (char *)mbedtls_pk_rsa(pk);
		if (NULL == pub_key->key) {
			int sni_rc = SNI_throwNativeException(LLSEC_ERROR, "RSA public key extraction failed");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			return_code = LLSEC_ERROR;
		}
	}

	if (LLSEC_SUCCESS == return_code) {
		void *native_id = (void *)pub_key;
		if (SNI_OK != SNI_registerResource(native_id, (SNI_closeFunction)LLSEC_KEY_FACTORY_mbedtls_public_key_close,
		                                   NULL)) {
			int32_t sni_rc = SNI_throwNativeException(LLSEC_ERROR, "Can't register SNI native resource");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			mbedtls_rsa_free((mbedtls_rsa_context *)pub_key->key);
			return_code = LLSEC_ERROR;
		} else {
			// cppcheck-suppress misra-c2012-11.6 // Abstract data type for SNI usage
			return_code = (int32_t)native_id;
		}
	}

	LLSEC_KEY_FACTORY_DEBUG_TRACE("%s (rc = %d)\n", __func__, return_code);
	return return_code;
}

static int32_t LLSEC_KEY_FACTORY_EC_mbedtls_get_private_key_data(LLSEC_priv_key *priv_key, uint8_t *encoded_key,
                                                                 int32_t encoded_key_length) {
	LLSEC_KEY_FACTORY_DEBUG_TRACE("%s (key = %p) \n", __func__, priv_key);
	LLSEC_KEY_FACTORY_DEBUG_TRACE("encLen %d, strlen: %zu, enc:%s  \n", (int)encoded_key_length,
	                              strlen((char *)encoded_key), encoded_key);

	int return_code = LLSEC_SUCCESS;
	int mbedtls_rc = LLSEC_MBEDTLS_SUCCESS;

	priv_key->type = TYPE_ECDSA;

	mbedtls_pk_context pk;
	mbedtls_pk_init(&pk);

#if (MBEDTLS_VERSION_MAJOR == 3)
	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
	const char *pers = llsec_gen_random_str_internal(8);

	mbedtls_entropy_init(&entropy);
	mbedtls_ctr_drbg_init(&ctr_drbg);

	mbedtls_rc = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *)pers,
	                                   strlen(pers));
	if (LLSEC_MBEDTLS_SUCCESS != mbedtls_rc) {
		int32_t sni_rc = SNI_throwNativeException(mbedtls_rc, "mbedtls_ctr_drbg_seed failed");
		LLSEC_ASSERT(sni_rc == SNI_OK);
		return_code = LLSEC_ERROR;
	}
#endif

	if (LLSEC_SUCCESS == return_code) {
#if (MBEDTLS_VERSION_MAJOR == 2)
		mbedtls_rc = mbedtls_pk_parse_key(&pk, encoded_key, encoded_key_length, NULL, 0);
#elif (MBEDTLS_VERSION_MAJOR == 3)
		mbedtls_rc = mbedtls_pk_parse_key(&pk, encoded_key, encoded_key_length, NULL, 0, mbedtls_ctr_drbg_random,
		                                  &ctr_drbg);
#else
		#error "Unsupported mbedTLS major version"
#endif
		if (LLSEC_MBEDTLS_SUCCESS != mbedtls_rc) {
			LLSEC_KEY_FACTORY_DEBUG_TRACE("mbedtls_pk_parse_key failed (rc = %d)\n", mbedtls_rc);
			return_code = LLSEC_ERROR;
		}
	}

	if (LLSEC_SUCCESS == return_code) {
		priv_key->key = (char *)mbedtls_pk_ec(pk);
		if (NULL == priv_key->key) {
			int32_t sni_rc = SNI_throwNativeException(LLSEC_ERROR, "EC context extraction failed");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			return_code = LLSEC_ERROR;
		}
	}

	if (LLSEC_SUCCESS == return_code) {
		void *native_id = (void *)priv_key;
		if (SNI_OK != SNI_registerResource(native_id, (SNI_closeFunction)LLSEC_KEY_FACTORY_mbedtls_private_key_close,
		                                   NULL)) {
			int32_t sni_rc = SNI_throwNativeException(LLSEC_ERROR, "Can't register SNI native resource");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			mbedtls_ecdsa_free((mbedtls_ecdsa_context *)priv_key->key);
			return_code = LLSEC_ERROR;
		} else {
			// cppcheck-suppress misra-c2012-11.6 // Abstract data type for SNI usage
			return_code = (int32_t)native_id;
		}
	}

#if (MBEDTLS_VERSION_MAJOR == 3)
	mbedtls_ctr_drbg_free(&ctr_drbg);
	mbedtls_entropy_free(&entropy);
	// cppcheck-suppress misra-c2012-11.8 // Cast for matching free function signature
	mbedtls_free((void *)pers);
#endif

	LLSEC_KEY_FACTORY_DEBUG_TRACE("%s (rc = %d)\n", __func__, return_code);
	return return_code;
}

static int32_t LLSEC_KEY_FACTORY_EC_mbedtls_get_public_key_data(LLSEC_pub_key *pub_key, uint8_t *encoded_key,
                                                                int32_t encoded_key_length) {
	LLSEC_KEY_FACTORY_DEBUG_TRACE("%s (key = %p) \n", __func__, pub_key);

	int return_code = LLSEC_SUCCESS;
	int mbedtls_rc = LLSEC_MBEDTLS_SUCCESS;

	pub_key->type = TYPE_ECDSA;

	mbedtls_pk_context pk;
	mbedtls_pk_init(&pk);
	mbedtls_rc = mbedtls_pk_parse_public_key(&pk, encoded_key, encoded_key_length);
	if (LLSEC_MBEDTLS_SUCCESS != mbedtls_rc) {
		LLSEC_KEY_FACTORY_DEBUG_TRACE("mbedtls_pk_parse_public_key failed (rc = %d)\n", mbedtls_rc);
		return_code = LLSEC_ERROR;
	}

	if (LLSEC_SUCCESS == return_code) {
		pub_key->key = (char *)mbedtls_pk_ec(pk);
		if (NULL == pub_key->key) {
			int32_t sni_rc = SNI_throwNativeException(LLSEC_ERROR, "EC public key extraction failed");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			return_code = LLSEC_ERROR;
		}
	}

	if (LLSEC_SUCCESS == return_code) {
		void *native_id = (void *)pub_key;
		if (SNI_registerResource(native_id, (SNI_closeFunction)LLSEC_KEY_FACTORY_mbedtls_public_key_close,
		                         NULL) != SNI_OK) {
			int32_t sni_rc = SNI_throwNativeException(LLSEC_ERROR, "Can't register SNI native resource");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			mbedtls_ecdsa_free((mbedtls_ecdsa_context *)pub_key->key);
			return_code = LLSEC_ERROR;
		} else {
			// cppcheck-suppress misra-c2012-11.6 // Abstract data type for SNI usage
			return_code = (int32_t)native_id;
		}
	}

	LLSEC_KEY_FACTORY_DEBUG_TRACE("%s (rc = %d)\n", __func__, return_code);
	return return_code;
}

static void LLSEC_KEY_FACTORY_mbedtls_private_key_close(void *native_id) {
	LLSEC_priv_key *key = (LLSEC_priv_key *)native_id;

	if (TYPE_RSA == key->type) {
		mbedtls_rsa_free((mbedtls_rsa_context *)key->key);
	} else {
		mbedtls_ecdsa_free((mbedtls_ecdsa_context *)key->key);
	}
	if (NULL != key) {
		mbedtls_free(key);
	}
}

static void LLSEC_KEY_FACTORY_mbedtls_public_key_close(void *native_id) {
	LLSEC_pub_key *key = (LLSEC_pub_key *)native_id;

	if (TYPE_RSA == key->type) {
		mbedtls_rsa_free((mbedtls_rsa_context *)key->key);
	} else {
		mbedtls_ecdsa_free((mbedtls_ecdsa_context *)key->key);
	}
	if (NULL != key) {
		mbedtls_free(key);
	}
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_KEY_FACTORY_IMPL_get_algorithm_description(uint8_t *algorithm_name) {
	int32_t return_code = LLSEC_ERROR;
	LLSEC_KEY_FACTORY_DEBUG_TRACE("%s \n", __func__);
	LLSEC_PROFILE_START();
	int32_t nb_algorithms = sizeof(available_key_algorithms) / sizeof(LLSEC_KEY_FACTORY_algorithm);
	const LLSEC_KEY_FACTORY_algorithm *algorithm = &available_key_algorithms[0];

	while (--nb_algorithms >= 0) {
		if (strcmp((char *)algorithm_name, algorithm->name) == 0) {
			break;
		}
		algorithm++;
	}

	if (0 <= nb_algorithms) {
		return_code = (int32_t)algorithm;
	}
	LLSEC_PROFILE_END();
	return return_code;
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_KEY_FACTORY_IMPL_get_public_key_data(int32_t algorithm_id, uint8_t *format_name, uint8_t *encoded_key,
                                                   int32_t encoded_key_length) {
	int32_t return_code = LLSEC_SUCCESS;
	LLSEC_KEY_FACTORY_DEBUG_TRACE("%s \n", __func__);
	LLSEC_PROFILE_START();
	LLSEC_pub_key *public_key = NULL;

	if (0 != strcmp((char *)format_name, x509_format)) {
		int32_t sni_rc = SNI_throwNativeException(LLSEC_ERROR, "Invalid format name");
		LLSEC_ASSERT(sni_rc == SNI_OK);
		return_code = LLSEC_ERROR;
	}

	if (LLSEC_SUCCESS == return_code) {
		public_key = (LLSEC_pub_key *)mbedtls_calloc(1, sizeof(LLSEC_pub_key));
		if (public_key == NULL) {
			int32_t sni_rc = SNI_throwNativeException(LLSEC_ERROR, "Can't allocate LLSEC_pub_key structure");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			return_code = LLSEC_ERROR;
		}
	}

	if (LLSEC_SUCCESS == return_code) {
		const LLSEC_KEY_FACTORY_algorithm *algorithm = (const LLSEC_KEY_FACTORY_algorithm *)algorithm_id;
		return_code = algorithm->get_public_key_data(public_key, encoded_key, encoded_key_length);
		if ((LLSEC_ERROR == return_code) && (NULL != public_key)) {
			mbedtls_free(public_key);
		}
	}

	LLSEC_KEY_FACTORY_DEBUG_TRACE("%s (rc = %d)\n", __func__, (int)return_code);
	LLSEC_PROFILE_END();
	return return_code;
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_KEY_FACTORY_IMPL_get_private_key_data(int32_t algorithm_id, uint8_t *format_name, uint8_t *encoded_key,
                                                    int32_t encoded_key_length) {
	int32_t return_code = LLSEC_SUCCESS;
	LLSEC_KEY_FACTORY_DEBUG_TRACE("%s \n", __func__);
	LLSEC_PROFILE_START();
	LLSEC_priv_key *private_key = NULL;

	if (0 != strcmp((char *)format_name, pkcs8_format)) {
		int32_t sni_rc = SNI_throwNativeException(LLSEC_ERROR, "Invalid format name");
		LLSEC_ASSERT(sni_rc == SNI_OK);
		return_code = LLSEC_ERROR;
	}

	if (LLSEC_SUCCESS == return_code) {
		private_key = (LLSEC_priv_key *)mbedtls_calloc(1, sizeof(LLSEC_priv_key));
		if (NULL == private_key) {
			int32_t sni_rc = SNI_throwNativeException(LLSEC_ERROR, "Can't allocate LLSEC_priv_key structure");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			return_code = LLSEC_ERROR;
		}
	}

	if (LLSEC_SUCCESS == return_code) {
		const LLSEC_KEY_FACTORY_algorithm *algorithm = (const LLSEC_KEY_FACTORY_algorithm *)algorithm_id;
		return_code = algorithm->get_private_key_data(private_key, encoded_key, encoded_key_length);
		if ((LLSEC_ERROR == return_code) && (NULL != private_key)) {
			mbedtls_free(private_key);
		}
	}

	LLSEC_KEY_FACTORY_DEBUG_TRACE("%s (rc = %d)\n", __func__, (int)return_code);
	LLSEC_PROFILE_END();
	return return_code;
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_KEY_FACTORY_IMPL_get_private_key_close_id(int32_t algorithm_id) {
	LLSEC_KEY_FACTORY_DEBUG_TRACE("%s \n", __func__);

	const LLSEC_KEY_FACTORY_algorithm *algorithm = (const LLSEC_KEY_FACTORY_algorithm *)algorithm_id;
	// cppcheck-suppress [misra-c2012-11.1, misra-c2012-11.6] // Abstract data type for SNI usage
	return (int32_t)algorithm->private_key_close;
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_KEY_FACTORY_IMPL_get_public_key_close_id(int32_t algorithm_id) {
	LLSEC_KEY_FACTORY_DEBUG_TRACE("%s \n", __func__);

	const LLSEC_KEY_FACTORY_algorithm *algorithm = (const LLSEC_KEY_FACTORY_algorithm *)algorithm_id;
	// cppcheck-suppress [misra-c2012-11.1, misra-c2012-11.6] // Abstract data type for SNI usage
	return (int32_t)algorithm->public_key_close;
}

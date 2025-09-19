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

#include <LLSEC_PUBLIC_KEY_impl.h>
#include <LLSEC_CONSTANTS.h>
#include <LLSEC_mbedtls.h>

#include <sni.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#if (MBEDTLS_VERSION_MAJOR == 2)
#include "mbedtls/pk_internal.h"
#elif (MBEDTLS_VERSION_MAJOR == 3)
#include "pk_wrap.h"
#else
#error "Unsupported mbedTLS major version"
#endif
#include "mbedtls/platform.h"
#include "mbedtls/pk.h"
#include "mbedtls/error.h"

/**
 * @brief Public pk context.
 * Received as input by the LLSEC_PUBLIC native functions, contains an initialized public key context
 * that will be used by the public pk context.
 */
static void *pub_pk_ctx;

static void *pub_ctx_alloc_func(void) {
	return pub_pk_ctx;
}

static void pub_ctx_free_func(void *ctx) {
	// nothing to do, context is received as input to the native function, so it must not be freed here
	LLSEC_UNUSED_PARAM(ctx);
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_PUBLIC_KEY_IMPL_get_encoded_max_size(int32_t native_id) {
	LLSEC_UNUSED_PARAM(native_id);
	LLSEC_PUBLIC_KEY_DEBUG_TRACE("%s \n", __func__);
	return LLSEC_PUBLIC_KEY_MAX_DER_LEN;
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_PUBLIC_KEY_IMPL_get_encode(int32_t native_id, uint8_t *output, int32_t outputLength) {
	LLSEC_PUBLIC_KEY_DEBUG_TRACE("%s \n", __func__);
	LLSEC_PROFILE_START();
	int return_code = LLSEC_ERROR;
	int mbedtls_rc = LLSEC_MBEDTLS_SUCCESS;

	LLSEC_pub_key *key = (LLSEC_pub_key *)native_id;
	mbedtls_pk_context pk;
	mbedtls_pk_type_t pk_type;

	if (TYPE_RSA == key->type) {
		pk_type = MBEDTLS_PK_RSA;
	} else {
		pk_type = MBEDTLS_PK_ECKEY;
	}

	mbedtls_pk_init(&pk);

	mbedtls_pk_info_t info;
	(void)memcpy(&info, mbedtls_pk_info_from_type(pk_type), sizeof(mbedtls_pk_info_t));
	info.ctx_alloc_func = pub_ctx_alloc_func;
	info.ctx_free_func = pub_ctx_free_func;

	pub_pk_ctx = (void *)key->key;

	mbedtls_rc = mbedtls_pk_setup(&pk, &info);
	if (LLSEC_MBEDTLS_SUCCESS != mbedtls_rc) {
		int32_t sni_rc = SNI_throwNativeException(mbedtls_rc, "Public key context setup failed");
		LLSEC_ASSERT(sni_rc == SNI_OK);
	} else {
		/* Write a public key to a SubjectPublicKeyInfo DER structure */
		int length = mbedtls_pk_write_pubkey_der(&pk, output, outputLength);
		if (0 > length) {
			int32_t sni_rc = SNI_throwNativeException(-length, "Public key encoding failed");
			LLSEC_ASSERT(sni_rc == SNI_OK);
		} else {
			/* data was written at the end of the buffer but the llapi expects it at the begining,
			 * use memmove() to move data in place */
			(void)memmove(output, output + outputLength - length, length);
			return_code = length;
		}
	}

	LLSEC_PROFILE_END();
	return return_code;
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_PUBLIC_KEY_IMPL_get_output_size(int32_t native_id) {
	LLSEC_PUBLIC_KEY_DEBUG_TRACE("%s \n", __func__);
	LLSEC_PROFILE_START();
	int return_code = LLSEC_ERROR;
	int mbedtls_rc = LLSEC_MBEDTLS_SUCCESS;

	LLSEC_pub_key *key = (LLSEC_pub_key *)native_id;
	mbedtls_pk_context pk;
	mbedtls_pk_type_t pk_type;

	if (TYPE_RSA == key->type) {
		pk_type = MBEDTLS_PK_RSA;
	} else {
		pk_type = MBEDTLS_PK_ECKEY;
	}

	mbedtls_pk_init(&pk);

	mbedtls_pk_info_t info;
	(void)memcpy(&info, mbedtls_pk_info_from_type(pk_type), sizeof(mbedtls_pk_info_t));
	info.ctx_alloc_func = pub_ctx_alloc_func;
	info.ctx_free_func = pub_ctx_free_func;

	pub_pk_ctx = (void *)key->key;

	mbedtls_rc = mbedtls_pk_setup(&pk, &info);
	if (LLSEC_MBEDTLS_SUCCESS != mbedtls_rc) {
		int32_t sni_rc = SNI_throwNativeException(mbedtls_rc, "Public key context setup failed");
		LLSEC_ASSERT(sni_rc == SNI_OK);
	} else {
		return_code = mbedtls_pk_get_bitlen(&pk) / 8UL;
	}

	LLSEC_PROFILE_END();
	return return_code;
}

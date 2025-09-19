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

#include <LLSEC_PRIVATE_KEY_impl.h>
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
#include "mbedtls/asn1write.h"
#include "mbedtls/oid.h"
#include "mbedtls/error.h"

/* Run predicate if ret is greater than or equal to zero.
 * Store the returned value from the predicate in ret.
 * If ret is greater than or equal to zero, add ret it to total. */
#define CHK_ADD_IF_GE0(total, ret, predicate) \
		do {                                  \
			if ((ret) >= 0) {                 \
				(ret) = (predicate);          \
				if ((ret) >= 0) {             \
					(total) += (ret);         \
				}                             \
			}                                 \
		} while (0)

/**
 * @brief Private pk context.
 * Received as input by the LLSEC_PRIVATE native functions, contains an initialized private key context
 * that will be used by the private pk context.
 */
static void *priv_pk_ctx;

static void *priv_ctx_alloc_func(void) {
	return priv_pk_ctx;
}

static void priv_ctx_free_func(void *ctx) {
	// nothing to do, context is received as input to the native function, so it must not be freed here
	LLSEC_UNUSED_PARAM(ctx);
}

static int pk_write_ec_key_asn1(mbedtls_pk_context *key, unsigned char *buf, unsigned char **p) {
	mbedtls_ecp_keypair *ecp_keypair = mbedtls_pk_ec(*key);
	mbedtls_ecp_group_id group_id;
	const char *local_oid;
	size_t local_oid_len;
	int ret;

#if (MBEDTLS_VERSION_NUMBER < 0x03000000)
	group_id = ecp_keypair->grp.id;
#elif (MBEDTLS_VERSION_NUMBER < 0x03060000)
	// This is unfortunate, but the API to access grp.id was introduced in v3.6
	group_id = ecp_keypair->MBEDTLS_PRIVATE(grp).id;
#else
	group_id = mbedtls_ecp_keypair_get_group_id(ecp_keypair);
#endif

	ret = mbedtls_oid_get_oid_by_ec_grp(group_id, &local_oid, &local_oid_len);

	if (ret >= 0) {
		ret = mbedtls_asn1_write_oid(p, buf, local_oid, local_oid_len);
	}

	return ret;
}

static int pk_write_key_pkcs8_der(mbedtls_pk_context *key, unsigned char *buf, int32_t size) {
	mbedtls_pk_type_t pk_type;
	size_t par_len = 0;
	unsigned char *p;
	const char *oid;
	size_t oid_len;
	int32_t len = 0;
	int ret;

	/* Write the private key to a PKCS#1 or SEC1 DER structure */
	ret = mbedtls_pk_write_key_der(key, buf, size);

	if (ret >= 0) {
		len = ret;
		p = buf + size - len;
	}

	CHK_ADD_IF_GE0(len, ret, mbedtls_asn1_write_len(&p, buf, len));
	CHK_ADD_IF_GE0(len, ret, mbedtls_asn1_write_tag(&p, buf, MBEDTLS_ASN1_OCTET_STRING));

	if (ret >= 0) {
		pk_type = mbedtls_pk_get_type(key);
		ret = mbedtls_oid_get_oid_by_pk_alg(pk_type, &oid, &oid_len);
	}

	if ((ret >= 0) && (pk_type == MBEDTLS_PK_ECKEY)) {
		ret = pk_write_ec_key_asn1(key, buf, &p);
		if (ret >= 0) {
			par_len = ret;
		}
	}

	/* privateKeyAlgorithm */
	CHK_ADD_IF_GE0(len, ret, mbedtls_asn1_write_algorithm_identifier(&p, buf, oid, oid_len, par_len));
	/* version */
	CHK_ADD_IF_GE0(len, ret, mbedtls_asn1_write_int(&p, buf, 0));
	/* sequence and length */
	CHK_ADD_IF_GE0(len, ret, mbedtls_asn1_write_len(&p, buf, len));
	CHK_ADD_IF_GE0(len, ret, mbedtls_asn1_write_tag(&p, buf, MBEDTLS_ASN1_CONSTRUCTED | MBEDTLS_ASN1_SEQUENCE));

	return (ret < 0) ? ret : len;
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_PRIVATE_KEY_IMPL_get_encoded_max_size(int32_t native_id) {
	LLSEC_UNUSED_PARAM(native_id);
	LLSEC_PRIVATE_KEY_DEBUG_TRACE("%s \n", __func__);
	return LLSEC_PRIVATE_KEY_MAX_PKCS8_LEN;
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_PRIVATE_KEY_IMPL_get_encode(int32_t native_id, uint8_t *output, int32_t outputLength) {
	LLSEC_PRIVATE_KEY_DEBUG_TRACE("%s \n", __func__);
	LLSEC_PROFILE_START();
	int return_code = LLSEC_ERROR;
	int mbedtls_rc = LLSEC_MBEDTLS_SUCCESS;

	LLSEC_priv_key *key = (LLSEC_priv_key *)native_id;
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
	info.ctx_alloc_func = priv_ctx_alloc_func;
	info.ctx_free_func = priv_ctx_free_func;

	priv_pk_ctx = (void *)key->key;

	mbedtls_rc = mbedtls_pk_setup(&pk, &info);
	if (LLSEC_MBEDTLS_SUCCESS != mbedtls_rc) {
		int32_t sni_rc = SNI_throwNativeException(mbedtls_rc, "Private key context setup failed");
		LLSEC_ASSERT(sni_rc == SNI_OK);
	} else {
		/* Write a private key to a PKCS#8 DER structure */
		int length = pk_write_key_pkcs8_der(&pk, output, outputLength);
		if (0 > length) {
			int32_t sni_rc = SNI_throwNativeException(-length, "Private key encoding failed");
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
int32_t LLSEC_PRIVATE_KEY_IMPL_get_output_size(int32_t native_id) {
	LLSEC_PRIVATE_KEY_DEBUG_TRACE("%s \n", __func__);
	LLSEC_PROFILE_START();
	int return_code = LLSEC_ERROR;
	int mbedtls_rc = LLSEC_MBEDTLS_SUCCESS;

	LLSEC_priv_key *key = (LLSEC_priv_key *)native_id;
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
	info.ctx_alloc_func = priv_ctx_alloc_func;
	info.ctx_free_func = priv_ctx_free_func;

	priv_pk_ctx = (void *)key->key;

	mbedtls_rc = mbedtls_pk_setup(&pk, &info);
	if (LLSEC_MBEDTLS_SUCCESS != mbedtls_rc) {
		int32_t sni_rc = SNI_throwNativeException(mbedtls_rc, "Private key context setup failed");
		LLSEC_ASSERT(sni_rc == SNI_OK);
	} else {
		return_code = mbedtls_pk_get_bitlen(&pk) / 8UL;
	}

	LLSEC_PROFILE_END();
	return return_code;
}

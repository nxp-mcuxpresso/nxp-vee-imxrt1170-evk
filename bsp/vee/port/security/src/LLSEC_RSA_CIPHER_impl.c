/*
 * C
 *
 * Copyright 2023-2025 MicroEJ Corp. All rights reserved.
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

#include <LLSEC_RSA_CIPHER_impl.h>
#include <LLSEC_CONSTANTS.h>
#include <LLSEC_mbedtls.h>

#include <sni.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/rsa.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"

// Forward declaration
typedef struct LLSEC_RSA_CIPHER_ctx LLSEC_RSA_CIPHER_ctx;

/**
 * RSA Cipher init function type
 */
typedef int32_t (*LLSEC_RSA_CIPHER_init)(int32_t transformation_id, LLSEC_RSA_CIPHER_ctx *cipher_ctx,
                                         uint8_t is_decrypting, int32_t key_id, int32_t padding, int32_t hash);
typedef int32_t (*LLSEC_RSA_CIPHER_decrypt)(LLSEC_RSA_CIPHER_ctx *cipher_ctx, const uint8_t *buffer,
                                            int32_t buffer_length, uint8_t *output, int *mbedtls_rc);
typedef int32_t (*LLSEC_RSA_CIPHER_encrypt)(LLSEC_RSA_CIPHER_ctx *cipher_ctx, const uint8_t *buffer,
                                            int32_t buffer_length, uint8_t *output, int *mbedtls_rc);
typedef void (*LLSEC_RSA_CIPHER_close)(void *native_id);

typedef struct {
	char *name; // the name of the transformation
	LLSEC_RSA_CIPHER_init init;
	LLSEC_RSA_CIPHER_decrypt decrypt;
	LLSEC_RSA_CIPHER_encrypt encrypt;
	LLSEC_RSA_CIPHER_close close;
	LLSEC_RSA_CIPHER_transformation_desc description;
} LLSEC_RSA_CIPHER_transformation;

struct LLSEC_RSA_CIPHER_ctx {
	const LLSEC_RSA_CIPHER_transformation *transformation;
	mbedtls_rsa_context mbedtls_ctx;
	mbedtls_ctr_drbg_context ctr_drbg;
};

static int32_t llsec_rsa_cipher_init(int32_t transformation_id, LLSEC_RSA_CIPHER_ctx *cipher_ctx, uint8_t is_decrypting,
                                     int32_t key_id,
                                     int32_t padding_type, int32_t oaep_hash_algorithm);
static int32_t llsec_rsa_cipher_decrypt(LLSEC_RSA_CIPHER_ctx *cipher_ctx, const uint8_t *buffer, int32_t buffer_length,
                                        uint8_t *output,
                                        int *mbedtls_rc);
static int32_t llsec_rsa_cipher_encrypt(LLSEC_RSA_CIPHER_ctx *cipher_ctx, const uint8_t *buffer, int32_t buffer_length,
                                        uint8_t *output,
                                        int *mbedtls_rc);
static void llsec_rsa_cipher_close(void *native_id);

// cppcheck-suppress misra-c2012-8.9 // Define here for code readability even if it called once in this file.
static const LLSEC_RSA_CIPHER_transformation available_transformations[3] = {
	{
		.name = "RSA/ECB/PKCS1Padding",
		.init = llsec_rsa_cipher_init,
		.decrypt = llsec_rsa_cipher_decrypt,
		.encrypt = llsec_rsa_cipher_encrypt,
		.close = llsec_rsa_cipher_close,
		.description = {
			.padding_type = PAD_PKCS1_TYPE,
			.oaep_hash_algorithm = 0,
		},
	},
	{
		.name = "RSA/ECB/OAEPWithSHA-1AndMGF1Padding",
		.init = llsec_rsa_cipher_init,
		.decrypt = llsec_rsa_cipher_decrypt,
		.encrypt = llsec_rsa_cipher_encrypt,
		.close = llsec_rsa_cipher_close,
		.description = {
			.padding_type = PAD_OAEP_MGF1_TYPE,
			.oaep_hash_algorithm = OAEP_HASH_SHA_1_ALGORITHM,
		},
	},
	{
		.name = "RSA/ECB/OAEPWithSHA-256AndMGF1Padding",
		.init = llsec_rsa_cipher_init,
		.decrypt = llsec_rsa_cipher_decrypt,
		.encrypt = llsec_rsa_cipher_encrypt,
		.close = llsec_rsa_cipher_close,
		.description = {
			.padding_type = PAD_OAEP_MGF1_TYPE,
			.oaep_hash_algorithm = OAEP_HASH_SHA_256_ALGORITHM,
		},
	}
};

static int32_t llsec_rsa_cipher_init(int32_t transformation_id, LLSEC_RSA_CIPHER_ctx *cipher_ctx, uint8_t is_decrypting,
                                     int32_t key_id, int32_t padding_type, int32_t oaep_hash_algorithm) {
	int return_code = LLSEC_SUCCESS;
	int mbedtls_rc = LLSEC_MBEDTLS_SUCCESS;
	mbedtls_rsa_context *key_ctx = NULL;
	LLSEC_RSA_CIPHER_DEBUG_TRACE("%s\n", __func__);

	cipher_ctx->transformation = (LLSEC_RSA_CIPHER_transformation *)transformation_id;

	if ((uint8_t)0 != is_decrypting) {
		LLSEC_priv_key *key = (LLSEC_priv_key *)key_id;
		key_ctx = (mbedtls_rsa_context *)key->key;
	} else {
		LLSEC_pub_key *key = (LLSEC_pub_key *)key_id;
		key_ctx = (mbedtls_rsa_context *)key->key;
	}

	mbedtls_rc = mbedtls_rsa_copy(&cipher_ctx->mbedtls_ctx, key_ctx);
	if (LLSEC_MBEDTLS_SUCCESS == mbedtls_rc) {
		int32_t padding = ((uint32_t)padding_type == PAD_PKCS1_TYPE) ? MBEDTLS_RSA_PKCS_V15 : MBEDTLS_RSA_PKCS_V21;
		int32_t hash_id = ((uint32_t)oaep_hash_algorithm ==
		                   OAEP_HASH_SHA_1_ALGORITHM) ? MBEDTLS_MD_SHA1 : MBEDTLS_MD_SHA256;
#if (MBEDTLS_VERSION_MAJOR == 3)
		// mbedtls_rsa_set_padding only returns an error code starting with version 3
		mbedtls_rc =
#endif
		mbedtls_rsa_set_padding(&cipher_ctx->mbedtls_ctx, padding, hash_id);
#if (MBEDTLS_VERSION_MAJOR == 3)
		LLSEC_ASSERT(mbedtls_rc == LLSEC_MBEDTLS_SUCCESS);
#endif
	} else {
		mbedtls_rsa_free(&cipher_ctx->mbedtls_ctx);
		int32_t sni_rc = SNI_throwNativeException(mbedtls_rc, "mbedtls_rsa_copy failed");
		LLSEC_ASSERT(sni_rc == SNI_OK);
		return_code = LLSEC_ERROR;
	}

	if (LLSEC_SUCCESS == return_code) {
		mbedtls_entropy_context entropy;
		const char *pers = llsec_gen_random_str_internal(8);

		mbedtls_entropy_init(&entropy);
		mbedtls_ctr_drbg_init(&cipher_ctx->ctr_drbg);

		mbedtls_rc = mbedtls_ctr_drbg_seed(&cipher_ctx->ctr_drbg, mbedtls_entropy_func, &entropy, (const uint8_t *)pers,
		                                   strlen(pers));
		if (LLSEC_MBEDTLS_SUCCESS != mbedtls_rc) {
			mbedtls_ctr_drbg_free(&cipher_ctx->ctr_drbg);
			mbedtls_entropy_free(&entropy);
			// cppcheck-suppress misra-c2012-11.8 // Cast for matching free function signature
			mbedtls_free((void *)pers);
			int32_t sni_rc = SNI_throwNativeException(mbedtls_rc, "mbedtls_ctr_drbg_seed failed");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			return_code = LLSEC_ERROR;
		}
	}

	return return_code;
}

static int32_t llsec_rsa_cipher_decrypt(LLSEC_RSA_CIPHER_ctx *cipher_ctx, const uint8_t *buffer, int32_t buffer_length,
                                        uint8_t *output, int *mbedtls_rc) {
	LLSEC_RSA_CIPHER_DEBUG_TRACE("%s\n", __func__);

	(void)buffer_length;

	size_t out_len = 0;
	int32_t return_code = LLSEC_SUCCESS;
#if (MBEDTLS_VERSION_MAJOR == 2)
	*mbedtls_rc = mbedtls_rsa_pkcs1_decrypt(&cipher_ctx->mbedtls_ctx, mbedtls_ctr_drbg_random, &cipher_ctx->ctr_drbg,
	                                        MBEDTLS_RSA_PRIVATE, &out_len, buffer, output,
	                                        mbedtls_rsa_get_len(&cipher_ctx->mbedtls_ctx));
#elif (MBEDTLS_VERSION_MAJOR == 3)
	*mbedtls_rc = mbedtls_rsa_pkcs1_decrypt(&cipher_ctx->mbedtls_ctx, mbedtls_ctr_drbg_random, &cipher_ctx->ctr_drbg,
	                                        &out_len, buffer, output, mbedtls_rsa_get_len(&cipher_ctx->mbedtls_ctx));
#else
	#error "Unsupported mbedTLS major version"
#endif
	if (LLSEC_MBEDTLS_SUCCESS == *mbedtls_rc) {
		LLSEC_ASSERT(out_len <= (size_t)INT32_MAX);
		/* it can be assumed that the decrypted pkcs1 won't be bigger than 2GiB. */
		return_code = (int32_t)out_len;
	} else {
		return_code = LLSEC_MBEDTLS_ERR;
	}

	return return_code;
}

static int32_t llsec_rsa_cipher_encrypt(LLSEC_RSA_CIPHER_ctx *cipher_ctx, const uint8_t *buffer, int32_t buffer_length,
                                        uint8_t *output, int *mbedtls_rc) {
	LLSEC_RSA_CIPHER_DEBUG_TRACE("%s\n", __func__);

	int32_t return_code = LLSEC_SUCCESS;
#if (MBEDTLS_VERSION_MAJOR == 2)
	*mbedtls_rc = mbedtls_rsa_pkcs1_encrypt(&cipher_ctx->mbedtls_ctx, mbedtls_ctr_drbg_random, &cipher_ctx->ctr_drbg,
	                                        MBEDTLS_RSA_PUBLIC, buffer_length, buffer, output);
#elif (MBEDTLS_VERSION_MAJOR == 3)
	*mbedtls_rc = mbedtls_rsa_pkcs1_encrypt(&cipher_ctx->mbedtls_ctx, mbedtls_ctr_drbg_random, &cipher_ctx->ctr_drbg,
	                                        buffer_length, buffer, output);
#else
	#error "Unsupported mbedTLS major version"
#endif
	if (LLSEC_MBEDTLS_SUCCESS == *mbedtls_rc) {
		return_code = mbedtls_rsa_get_len(&cipher_ctx->mbedtls_ctx);
	} else {
		return_code = LLSEC_MBEDTLS_ERR;
	}

	return return_code;
}

static void llsec_rsa_cipher_close(void *native_id) {
	LLSEC_RSA_CIPHER_DEBUG_TRACE("%s native_id:%p\n", __func__, native_id);
	LLSEC_RSA_CIPHER_ctx *cipher_ctx = (LLSEC_RSA_CIPHER_ctx *)native_id;
	mbedtls_rsa_free(&cipher_ctx->mbedtls_ctx);
	mbedtls_free(native_id);
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_RSA_CIPHER_IMPL_get_transformation_description(uint8_t *transformation_name,
                                                             LLSEC_RSA_CIPHER_transformation_desc *transformation_desc)
{
	int32_t return_code = LLSEC_ERROR;
	LLSEC_RSA_CIPHER_DEBUG_TRACE("%s transformation_name %s\n", __func__, transformation_name);
	LLSEC_PROFILE_START();
	int32_t nb_transformations = sizeof(available_transformations) / sizeof(LLSEC_RSA_CIPHER_transformation);
	const LLSEC_RSA_CIPHER_transformation *transformation = &available_transformations[0];

	while (--nb_transformations >= 0) {
		if (strcmp((const char *)transformation_name, transformation->name) == 0) {
			(void)memcpy(transformation_desc, &(transformation->description),
			             sizeof(LLSEC_RSA_CIPHER_transformation_desc));
			break;
		}
		transformation++;
	}

	if (0 <= nb_transformations) {
		return_code = (int32_t)transformation;
	}
	LLSEC_PROFILE_END();
	return return_code;
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_RSA_CIPHER_IMPL_init(int32_t transformation_id, uint8_t is_decrypting, int32_t key_id,
                                   int32_t padding_type, int32_t oaep_hash_algorithm) {
	int32_t return_code = LLSEC_SUCCESS;
	LLSEC_RSA_CIPHER_DEBUG_TRACE("%s\n", __func__);
	LLSEC_PROFILE_START();
	LLSEC_RSA_CIPHER_ctx *cipher_ctx = NULL;
	const LLSEC_RSA_CIPHER_transformation *transformation = (LLSEC_RSA_CIPHER_transformation *)transformation_id;

	if (0 == key_id) {
		int32_t sni_rc = SNI_throwNativeException(key_id, "LLSEC_RSA_CIPHER_IMPL_init invalid key_id");
		LLSEC_ASSERT(sni_rc == SNI_OK);
		return_code = LLSEC_ERROR;
	}

	if (((uint32_t)padding_type != PAD_PKCS1_TYPE) && ((uint32_t)padding_type != PAD_OAEP_MGF1_TYPE)) {
		int32_t sni_rc = SNI_throwNativeException(padding_type, "LLSEC_RSA_CIPHER_IMPL_init invalid padding_type");
		LLSEC_ASSERT(sni_rc == SNI_OK);
		return_code = LLSEC_ERROR;
	}

	if (((uint32_t)padding_type == PAD_OAEP_MGF1_TYPE) &&
	    (((uint32_t)oaep_hash_algorithm != OAEP_HASH_SHA_1_ALGORITHM) &&
	     ((uint32_t)oaep_hash_algorithm != OAEP_HASH_SHA_256_ALGORITHM))) {
		int32_t sni_rc = SNI_throwNativeException(oaep_hash_algorithm,
		                                          "LLSEC_RSA_CIPHER_IMPL_init invalid oaep_hash_algorithm");
		LLSEC_ASSERT(sni_rc == SNI_OK);
		return_code = LLSEC_ERROR;
	}

	if (LLSEC_SUCCESS == return_code) {
		cipher_ctx = mbedtls_calloc(1, sizeof(LLSEC_RSA_CIPHER_ctx));
		if (cipher_ctx == NULL) {
			int32_t sni_rc = SNI_throwNativeException(SNI_ERROR, "failed to allocate LLSEC_RSA_CIPHER_ctx");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			return_code = LLSEC_ERROR;
		}
	}

	if (LLSEC_SUCCESS == return_code) {
		return_code = transformation->init(transformation_id, cipher_ctx, is_decrypting, key_id, padding_type,
		                                   oaep_hash_algorithm);

		if (LLSEC_SUCCESS != return_code) {
			int32_t sni_rc = SNI_throwNativeException(return_code, "LLSEC_RSA_CIPHER_IMPL_init failed");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			return_code = LLSEC_ERROR;
		}
	}

	if (LLSEC_SUCCESS == return_code) {
		/* cppcheck-suppress misra-c2012-11.8 ; we can safely remove const qualifier from a function pointer.
		 * Transformation is const so close is technically a const pointer to a function. If for any reason
		 * SNI_unregisterResource decided to change the pointer value, it wouldn't have any side effect. */
		if (SNI_OK != SNI_registerResource(cipher_ctx, transformation->close, NULL)) {
			int32_t sni_rc = SNI_throwNativeException(LLSEC_ERROR, "Can't register SNI native resource");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			transformation->close(cipher_ctx);
			return_code = LLSEC_ERROR;
		}
	}

	if (LLSEC_SUCCESS == return_code) {
		return_code = (int32_t)(cipher_ctx);
	} else {
		mbedtls_free(cipher_ctx);
	}

	LLSEC_PROFILE_END();
	return return_code;
}

static void LLSEC_RSA_CIPHER_IMPL_decrypt_job(MICROEJ_ASYNC_WORKER_job_t *job) {
	LLSEC_RSA_CIPHER_DEBUG_TRACE("%s\n", __func__);
	LLSEC_PROFILE_START();

	struct LLSEC_RSA_CIPHER_IMPL_decrypt_params *params = job->params;

	const LLSEC_RSA_CIPHER_transformation *transformation =
		(LLSEC_RSA_CIPHER_transformation *)params->transformation_id;

	params->rc = transformation->decrypt((LLSEC_RSA_CIPHER_ctx *)params->native_id, params->buffer,
	                                     params->buffer_length,
	                                     params->output, &params->mbedtls_rc);

	LLSEC_PROFILE_END();
}

static int32_t LLSEC_RSA_CIPHER_IMPL_decrypt_job_done(int32_t transformation_id, int32_t native_id, uint8_t *buffer,
                                                      int32_t buffer_offset, int32_t buffer_length, uint8_t *output,
                                                      int32_t output_offset) {
	LLSEC_UNUSED_PARAM(transformation_id);
	LLSEC_UNUSED_PARAM(native_id);
	LLSEC_UNUSED_PARAM(buffer);
	LLSEC_UNUSED_PARAM(buffer_offset);
	LLSEC_UNUSED_PARAM(buffer_length);

	int32_t return_code;
	int result = LLSEC_SUCCESS;
	const char *reason = NULL;
	int32_t exception_code = 0;
	struct LLSEC_RSA_CIPHER_IMPL_decrypt_params *params = NULL;
	MICROEJ_ASYNC_WORKER_job_t *job = MICROEJ_ASYNC_WORKER_get_job_done();

	if (job == NULL) {
		reason = "can't retrieve job result";
		exception_code = SNI_ERROR;
		result = LLSEC_ERROR;
	}

	if (result == LLSEC_SUCCESS) {
		params = (struct LLSEC_RSA_CIPHER_IMPL_decrypt_params *)job->params;

		if (params->rc < LLSEC_SUCCESS) {
			/* error case */
			switch (params->rc) {
			case LLSEC_MBEDTLS_ERR:
				reason = "mbedtls error";
				exception_code = params->mbedtls_rc;
				break;
			default:
				reason = "LLSEC_RSA_CIPHER_IMPL_decrypt failed";
				exception_code = params->rc;
				break;
			}

			result = LLSEC_ERROR;
		}
	}

	if (result == LLSEC_SUCCESS) {
		(void)memcpy(&output[output_offset], params->output, params->rc);
	}

	if (params != NULL) {
		mbedtls_free(params->output);
		mbedtls_free(params->buffer);
	}

	MICROEJ_ASYNC_WORKER_status_t async_rc = MICROEJ_ASYNC_WORKER_free_job(&llsec_worker, job);
	LLSEC_ASSERT(async_rc);

	if (result < LLSEC_SUCCESS) {
		int32_t sni_rc = SNI_throwNativeException(exception_code, reason);
		LLSEC_ASSERT(sni_rc == SNI_OK);
		return_code = SNI_IGNORED_RETURNED_VALUE;
	} else {
		return_code = params->rc;
	}

	return return_code;
}

int32_t LLSEC_RSA_CIPHER_IMPL_decrypt(int32_t transformation_id, int32_t native_id, uint8_t *buffer,
                                      int32_t buffer_offset, int32_t buffer_length, uint8_t *output,
                                      int32_t output_offset) {
	int result = LLSEC_SUCCESS;
	struct LLSEC_RSA_CIPHER_IMPL_decrypt_params *params = NULL;
	MICROEJ_ASYNC_WORKER_job_t *job = MICROEJ_ASYNC_WORKER_allocate_job(&llsec_worker,
	                                                                    (SNI_callback)LLSEC_RSA_CIPHER_IMPL_decrypt);

	if (job == NULL) {
		// MICROEJ_ASYNC_WORKER_allocate_job() has thrown a NativeException
		result = LLSEC_ERROR;
	}

	if (result == LLSEC_SUCCESS) {
		params = job->params;
		params->transformation_id = transformation_id;
		params->native_id = native_id;
		params->buffer = NULL;
		params->buffer_length = buffer_length - buffer_offset;
		params->output = NULL;

		params->buffer = mbedtls_calloc(params->buffer_length, 1);
		if (params->buffer == NULL) {
			int32_t sni_rc = SNI_throwNativeException(LLSEC_ERROR, "failed to alloc encrypted buffer");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			result = LLSEC_ERROR;
		}
	}

	if (result == LLSEC_SUCCESS) {
		(void)memcpy(params->buffer, &buffer[buffer_offset], params->buffer_length);

		params->output = mbedtls_calloc(SNI_getArrayLength(output) - output_offset, 1);
		if (params->output == NULL) {
			int32_t sni_rc = SNI_throwNativeException(LLSEC_ERROR, "failed to alloc decrypt buffer");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			result = LLSEC_ERROR;
		}
	}

	if (result == LLSEC_SUCCESS) {
		MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&llsec_worker, job,
		                                                                       LLSEC_RSA_CIPHER_IMPL_decrypt_job,
		                                                                       (SNI_callback)
		                                                                       LLSEC_RSA_CIPHER_IMPL_decrypt_job_done);

		if (status != MICROEJ_ASYNC_WORKER_OK) {
			// MICROEJ_ASYNC_WORKER_async_exec() has thrown a NativeException
			result = LLSEC_ERROR;
		}
	}

	if (result != LLSEC_SUCCESS) {
		if (params != NULL) {
			if (params->output != NULL) {
				mbedtls_free(params->output);
			}
			if (params->buffer != NULL) {
				mbedtls_free(params->buffer);
			}
		}

		if (job != NULL) {
			MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_free_job(&llsec_worker, job);
			LLSEC_ASSERT(status == MICROEJ_ASYNC_WORKER_OK);
		}
	}

	// Java Thread successfully suspended with callback
	return SNI_IGNORED_RETURNED_VALUE;
}

static void LLSEC_RSA_CIPHER_IMPL_encrypt_job(MICROEJ_ASYNC_WORKER_job_t *job) {
	LLSEC_RSA_CIPHER_DEBUG_TRACE("%s\n", __func__);
	LLSEC_PROFILE_START();
	struct LLSEC_RSA_CIPHER_IMPL_encrypt_params *params = job->params;

	const LLSEC_RSA_CIPHER_transformation *transformation =
		(LLSEC_RSA_CIPHER_transformation *)params->transformation_id;
	// cppcheck-suppress misra-c2012-11.6 // Abstract data type for SNI usage
	params->rc = transformation->encrypt((void *)params->native_id, params->buffer, params->buffer_length,
	                                     params->output, &params->mbedtls_rc);

	LLSEC_PROFILE_END();
}

static int32_t LLSEC_RSA_CIPHER_IMPL_encrypt_job_done(int32_t transformation_id, int32_t native_id, uint8_t *buffer,
                                                      int32_t buffer_offset, int32_t buffer_length, uint8_t *output,
                                                      int32_t output_offset) {
	LLSEC_UNUSED_PARAM(transformation_id);
	LLSEC_UNUSED_PARAM(native_id);
	LLSEC_UNUSED_PARAM(buffer);
	LLSEC_UNUSED_PARAM(buffer_offset);
	LLSEC_UNUSED_PARAM(buffer_length);

	int32_t return_code;
	int result = LLSEC_SUCCESS;
	const char *reason = NULL;
	int32_t exception_code = 0;

	struct LLSEC_RSA_CIPHER_IMPL_encrypt_params *params = NULL;
	MICROEJ_ASYNC_WORKER_job_t *job = MICROEJ_ASYNC_WORKER_get_job_done();
	if (job == NULL) {
		exception_code = SNI_ERROR;
		reason = "can't retrieve job result";
		result = LLSEC_ERROR;
	}

	if (result == LLSEC_SUCCESS) {
		params = job->params;

		if (params->rc < LLSEC_SUCCESS) {
			switch (params->rc) {
			case LLSEC_MBEDTLS_ERR:
				exception_code = params->mbedtls_rc;
				reason = "mbedtls error";
				break;
			default:
				exception_code = params->rc;
				reason = "LLSEC_RSA_CIPHER_IMPL_encrypt() failed";
				break;
			}

			result = LLSEC_ERROR;
		}
	}

	if (result == LLSEC_SUCCESS) {
		(void)memcpy(&output[output_offset], params->output, params->rc);
	}

	if (params != NULL) {
		mbedtls_free(params->output);
		mbedtls_free(params->buffer);
	}

	MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_free_job(&llsec_worker, job);
	LLSEC_ASSERT(status == MICROEJ_ASYNC_WORKER_OK);

	if (result < LLSEC_SUCCESS) {
		int32_t sni_rc = SNI_throwNativeException(exception_code, reason);
		LLSEC_ASSERT(sni_rc == SNI_OK);
		return_code = SNI_IGNORED_RETURNED_VALUE;
	} else {
		return_code = params->rc;
	}

	return return_code;
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_RSA_CIPHER_IMPL_encrypt(int32_t transformation_id, int32_t native_id, uint8_t *buffer,
                                      int32_t buffer_offset, int32_t buffer_length, uint8_t *output,
                                      int32_t output_offset) {
	int result = LLSEC_SUCCESS;
	struct LLSEC_RSA_CIPHER_IMPL_encrypt_params *params = NULL;
	MICROEJ_ASYNC_WORKER_job_t *job = MICROEJ_ASYNC_WORKER_allocate_job(&llsec_worker,
	                                                                    (SNI_callback)LLSEC_RSA_CIPHER_IMPL_encrypt);

	if (job == NULL) {
		// MICROEJ_ASYNC_WORKER_allocate_job() has thrown a NativeException
		result = LLSEC_ERROR;
	}

	if (result == LLSEC_SUCCESS) {
		params = job->params;
		params->transformation_id = transformation_id;
		params->native_id = native_id;
		params->buffer = NULL;
		params->buffer_length = buffer_length - buffer_offset;
		params->output = NULL;

		params->buffer = mbedtls_calloc(params->buffer_length, 1);
		if (params->buffer == NULL) {
			int32_t sni_rc = SNI_throwNativeException(LLSEC_ERROR, "failed to alloc plain buffer");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			result = LLSEC_ERROR;
		}
	}

	if (result == LLSEC_SUCCESS) {
		(void)memcpy(params->buffer, &buffer[buffer_offset], params->buffer_length);

		params->output = mbedtls_calloc(SNI_getArrayLength(output) - output_offset, 1);
		if (params->output == NULL) {
			int32_t sni_rc = SNI_throwNativeException(LLSEC_ERROR, "failed to alloc encrypted output");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			result = LLSEC_ERROR;
		}
	}

	if (result == LLSEC_SUCCESS) {
		MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&llsec_worker, job,
		                                                                       LLSEC_RSA_CIPHER_IMPL_encrypt_job,
		                                                                       (SNI_callback)
		                                                                       LLSEC_RSA_CIPHER_IMPL_encrypt_job_done);

		if (status != MICROEJ_ASYNC_WORKER_OK) {
			// MICROEJ_ASYNC_WORKER_async_exec() has thrown a NativeException
			result = LLSEC_ERROR;
		}
	}

	if (result != LLSEC_SUCCESS) {
		if (params != NULL) {
			if (params->output != NULL) {
				mbedtls_free(params->output);
			}

			if (params->buffer != NULL) {
				mbedtls_free(params->buffer);
			}
		}

		if (job != NULL) {
			MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_free_job(&llsec_worker, job);
			LLSEC_ASSERT(status == MICROEJ_ASYNC_WORKER_OK);
		}
	}

	// Java Thread successfully suspended with callback
	return SNI_IGNORED_RETURNED_VALUE;
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
void LLSEC_RSA_CIPHER_IMPL_close(int32_t transformation_id, int32_t native_id) {
	LLSEC_RSA_CIPHER_DEBUG_TRACE("%s\n", __func__);
	LLSEC_PROFILE_START();
	const LLSEC_RSA_CIPHER_transformation *transformation = (LLSEC_RSA_CIPHER_transformation *)transformation_id;

	// cppcheck-suppress misra-c2012-11.6 // Abstract data type for SNI usage
	transformation->close((void *)native_id);
	// cppcheck-suppress misra-c2012-11.6 // Abstract data type for SNI usage
	/* cppcheck-suppress misra-c2012-11.8 ; we can safely remove const qualifier from a function pointer.
	 * Transformation is const so close is technically a const pointer to a function. If for any reason
	 * SNI_unregisterResource decided to change the pointer value, it wouldn't have any side effect. */
	if (SNI_OK != SNI_unregisterResource((void *)native_id, transformation->close)) {
		int32_t sni_rc = SNI_throwNativeException(LLSEC_ERROR, "Can't unregister SNI native resource");
		LLSEC_ASSERT(sni_rc == SNI_OK);
	}
	LLSEC_PROFILE_END();
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_RSA_CIPHER_IMPL_get_close_id(int32_t transformation_id) {
	LLSEC_RSA_CIPHER_DEBUG_TRACE("%s\n", __func__);
	const LLSEC_RSA_CIPHER_transformation *transformation = (LLSEC_RSA_CIPHER_transformation *)transformation_id;
	// cppcheck-suppress [misra-c2012-11.1, misra-c2012-11.6] // Abstract data type for SNI usage
	return (int32_t)transformation->close;
}

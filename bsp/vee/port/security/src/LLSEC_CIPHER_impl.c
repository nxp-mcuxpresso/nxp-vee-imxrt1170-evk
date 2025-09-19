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

#include <LLSEC_CIPHER_impl.h>
#include <LLSEC_CONSTANTS.h>
#include <LLSEC_mbedtls.h>
#include <sni.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "mbedtls/aes.h"
#include "mbedtls/des.h"
#include "mbedtls/platform.h"
#include "mbedtls/platform_util.h"

#define AES_CBC_BLOCK_BITS    (128u)
#define AES_CBC_BLOCK_BYTES   (AES_CBC_BLOCK_BITS / 8u)

#define DES_CBC_BLOCK_BITS    (64u)
#define DES_CBC_BLOCK_BYTES   (DES_CBC_BLOCK_BITS / 8u)

typedef union {
	mbedtls_aes_context aes_ctx;
	mbedtls_des3_context des3_ctx;
} cipher_ctx;

typedef struct {
	cipher_ctx mbedtls_ctx;
	int32_t iv_length;
	uint8_t *iv;
} LLSEC_CIPHER_ctx;

/**
 * Cipher init function type
 */
typedef int (*LLSEC_CIPHER_init)(LLSEC_CIPHER_ctx *cipher_ctx, uint8_t is_decrypting,
                                 const uint8_t *key, int32_t key_length);
typedef int (*LLSEC_CIPHER_decrypt)(LLSEC_CIPHER_ctx *cipher_ctx, const uint8_t *buffer, int32_t buffer_length,
                                    uint8_t *output);
typedef int (*LLSEC_CIPHER_encrypt)(LLSEC_CIPHER_ctx *cipher_ctx, const uint8_t *buffer, int32_t buffer_length,
                                    uint8_t *output);
// must use void* for cipher_ctx to avoid casting function pointer type
typedef void (*LLSEC_CIPHER_close)(void *cipher_ctx);

typedef struct {
	char *name; // the name of the transformation
	LLSEC_CIPHER_init init;
	LLSEC_CIPHER_decrypt decrypt;
	LLSEC_CIPHER_encrypt encrypt;
	LLSEC_CIPHER_close close;
	LLSEC_CIPHER_transformation_desc description;
} LLSEC_CIPHER_transformation;

static int LLSEC_CIPHER_aescbc_init(LLSEC_CIPHER_ctx *cipher_ctx, uint8_t is_decrypting,
                                    const uint8_t *key, int32_t key_length);
static int LLSEC_CIPHER_des3cbc_init(LLSEC_CIPHER_ctx *cipher_ctx, uint8_t is_decrypting,
                                     const uint8_t *key, int32_t key_length);
static int mbedtls_aes_cipher_decrypt(LLSEC_CIPHER_ctx *cipher_ctx, const uint8_t *buffer, int32_t buffer_length,
                                      uint8_t *output);
static int mbedtls_des3_cipher_decrypt(LLSEC_CIPHER_ctx *cipher_ctx, const uint8_t *buffer, int32_t buffer_length,
                                       uint8_t *output);
static int mbedtls_aes_cipher_encrypt(LLSEC_CIPHER_ctx *cipher_ctx, const uint8_t *buffer, int32_t buffer_length,
                                      uint8_t *output);
static int mbedtls_des3_cipher_encrypt(LLSEC_CIPHER_ctx *cipher_ctx, const uint8_t *buffer, int32_t buffer_length,
                                       uint8_t *output);
static void mbedtls_aes_cipher_close(void *cipher_ctx);
static void mbedtls_des3_cipher_close(void *cipher_ctx);

static int LLSEC_CIPHER_aescbc_init(LLSEC_CIPHER_ctx *cipher_ctx, uint8_t is_decrypting,
                                    const uint8_t *key, int32_t key_length) {
	LLSEC_CIPHER_DEBUG_TRACE("%s %d\n", __func__, is_decrypting);

	int return_code = LLSEC_SUCCESS;
	int mbedtls_rc = LLSEC_MBEDTLS_SUCCESS;

	mbedtls_aes_init(&cipher_ctx->mbedtls_ctx.aes_ctx);

	if ((uint8_t)0 != is_decrypting) {
		mbedtls_rc = mbedtls_aes_setkey_dec(&cipher_ctx->mbedtls_ctx.aes_ctx, key, key_length * 8);
		LLSEC_CIPHER_DEBUG_TRACE("%s mbedtls_aes_setkey_dec (rc = %d)\n", __func__, mbedtls_rc);
	} else {
		mbedtls_rc = mbedtls_aes_setkey_enc(&cipher_ctx->mbedtls_ctx.aes_ctx, key, key_length * 8);
		LLSEC_CIPHER_DEBUG_TRACE("%s mbedtls_aes_setkey_enc (rc = %d)\n", __func__, mbedtls_rc);
	}

	if (LLSEC_MBEDTLS_SUCCESS != mbedtls_rc) {
		return_code = LLSEC_ERROR;
		mbedtls_aes_free(&cipher_ctx->mbedtls_ctx.aes_ctx);
	}

	return return_code;
}

static int LLSEC_CIPHER_des3cbc_init(LLSEC_CIPHER_ctx *cipher_ctx, uint8_t is_decrypting,
                                     const uint8_t *key, int32_t key_length) {
	LLSEC_CIPHER_DEBUG_TRACE("%s %d\n", __func__, is_decrypting);
	LLSEC_UNUSED_PARAM(key_length);

	int return_code = LLSEC_SUCCESS;
	int mbedtls_rc = LLSEC_MBEDTLS_SUCCESS;

	mbedtls_des3_init(&cipher_ctx->mbedtls_ctx.des3_ctx);

	if ((uint8_t)0 != is_decrypting) {
		mbedtls_rc = mbedtls_des3_set3key_dec(&cipher_ctx->mbedtls_ctx.des3_ctx, key);
		LLSEC_CIPHER_DEBUG_TRACE("%s mbedtls_des3_set3key_dec (rc = %d)\n", __func__, mbedtls_rc);
	} else {
		mbedtls_rc = mbedtls_des3_set3key_enc(&cipher_ctx->mbedtls_ctx.des3_ctx, key);
		LLSEC_CIPHER_DEBUG_TRACE("%s mbedtls_des3_set3key_enc (rc = %d)\n", __func__, mbedtls_rc);
	}

	if (LLSEC_MBEDTLS_SUCCESS != mbedtls_rc) {
		return_code = LLSEC_ERROR;
		mbedtls_des3_free(&cipher_ctx->mbedtls_ctx.des3_ctx);
	}

	return return_code;
}

static int mbedtls_aes_cipher_decrypt(LLSEC_CIPHER_ctx *cipher_ctx, const uint8_t *buffer, int32_t buffer_length,
                                      uint8_t *output) {
	LLSEC_CIPHER_DEBUG_TRACE("%s \n", __func__);
	return mbedtls_aes_crypt_cbc(&cipher_ctx->mbedtls_ctx.aes_ctx, MBEDTLS_AES_DECRYPT, buffer_length, cipher_ctx->iv,
	                             buffer, output);
}

static int mbedtls_des3_cipher_decrypt(LLSEC_CIPHER_ctx *cipher_ctx, const uint8_t *buffer, int32_t buffer_length,
                                       uint8_t *output) {
	LLSEC_CIPHER_DEBUG_TRACE("%s \n", __func__);
	return mbedtls_des3_crypt_cbc(&cipher_ctx->mbedtls_ctx.des3_ctx, MBEDTLS_DES_DECRYPT, buffer_length, cipher_ctx->iv,
	                              buffer, output);
}

static int mbedtls_aes_cipher_encrypt(LLSEC_CIPHER_ctx *cipher_ctx, const uint8_t *buffer, int32_t buffer_length,
                                      uint8_t *output) {
	LLSEC_CIPHER_DEBUG_TRACE("%s \n", __func__);
	return mbedtls_aes_crypt_cbc(&cipher_ctx->mbedtls_ctx.aes_ctx, MBEDTLS_AES_ENCRYPT, buffer_length, cipher_ctx->iv,
	                             buffer, output);
}

static int mbedtls_des3_cipher_encrypt(LLSEC_CIPHER_ctx *cipher_ctx, const uint8_t *buffer, int32_t buffer_length,
                                       uint8_t *output) {
	LLSEC_CIPHER_DEBUG_TRACE("%s \n", __func__);
	return mbedtls_des3_crypt_cbc(&cipher_ctx->mbedtls_ctx.des3_ctx, MBEDTLS_DES_ENCRYPT, buffer_length, cipher_ctx->iv,
	                              buffer, output);
}

static void mbedtls_aes_cipher_close(void *ctx) {
	LLSEC_CIPHER_DEBUG_TRACE("%s native_id %p\n", __func__, ctx);
	LLSEC_CIPHER_ctx *cipher_ctx = ctx;
	mbedtls_aes_free(&cipher_ctx->mbedtls_ctx.aes_ctx);
	mbedtls_free(cipher_ctx);
}

static void mbedtls_des3_cipher_close(void *ctx) {
	LLSEC_CIPHER_DEBUG_TRACE("%s native_id %p\n", __func__, ctx);
	LLSEC_CIPHER_ctx *cipher_ctx = ctx;
	mbedtls_des3_free(&cipher_ctx->mbedtls_ctx.des3_ctx);
	mbedtls_free(cipher_ctx);
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
// cppcheck-suppress constParameterPointer; SNI limitation, all java arrays are mutable
int32_t LLSEC_CIPHER_IMPL_get_transformation_description(uint8_t *transformation_name,
                                                         LLSEC_CIPHER_transformation_desc *transformation_desc) {
	static const LLSEC_CIPHER_transformation available_transformations[2] = {
		{
			.name = "AES/CBC/NoPadding",
			.init = LLSEC_CIPHER_aescbc_init,
			.decrypt = mbedtls_aes_cipher_decrypt,
			.encrypt = mbedtls_aes_cipher_encrypt,
			.close = mbedtls_aes_cipher_close,
			.description = {
				.block_size = AES_CBC_BLOCK_BYTES,
				.unit_bytes = AES_CBC_BLOCK_BYTES,
				.cipher_mode = CBC_MODE,
			},
		},
		{
			.name = "DESede/CBC/NoPadding",
			.init = LLSEC_CIPHER_des3cbc_init,
			.decrypt = mbedtls_des3_cipher_decrypt,
			.encrypt = mbedtls_des3_cipher_encrypt,
			.close = mbedtls_des3_cipher_close,
			.description = {
				.block_size = DES_CBC_BLOCK_BYTES,
				.unit_bytes = DES_CBC_BLOCK_BYTES,
				.cipher_mode = CBC_MODE,
			},
		}
	};
	LLSEC_CIPHER_DEBUG_TRACE("%s transformation_name %s\n", __func__, transformation_name);
	LLSEC_PROFILE_START();
	int32_t return_code = LLSEC_SUCCESS;
	int32_t nb_transformations = sizeof(available_transformations) / sizeof(LLSEC_CIPHER_transformation);
	const LLSEC_CIPHER_transformation *transformation = &available_transformations[0];

	while (--nb_transformations >= 0) {
		if (strcmp((const char *)transformation_name, transformation->name) == 0) {
			(void)memcpy(transformation_desc, &(transformation->description), sizeof(LLSEC_CIPHER_transformation_desc));
			break;
		}
		transformation++;
	}

	if (nb_transformations >= 0) {
		return_code = (int32_t)transformation;
	} else {
		return_code = LLSEC_ERROR;
	}
	LLSEC_PROFILE_END();
	return return_code;
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_CIPHER_IMPL_get_buffered_length(int32_t nativeTransformationId, int32_t nativeId) {
	LLSEC_UNUSED_PARAM(nativeTransformationId);
	LLSEC_UNUSED_PARAM(nativeId);
	return LLSEC_SUCCESS;
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
void LLSEC_CIPHER_IMPL_get_IV(int32_t transformation_id, int32_t native_id, uint8_t *iv, int32_t iv_length) {
	LLSEC_CIPHER_DEBUG_TRACE("%s \n", __func__);
	LLSEC_UNUSED_PARAM(transformation_id);
	LLSEC_PROFILE_START();
	const LLSEC_CIPHER_ctx *cipher_ctx = (const LLSEC_CIPHER_ctx *)native_id;
	(void)memcpy(iv, cipher_ctx->iv, iv_length);
	LLSEC_PROFILE_END();
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
// cppcheck-suppress constParameterPointer; SNI limitation, all java arrays are mutable
void LLSEC_CIPHER_IMPL_set_IV(int32_t transformation_id, int32_t native_id, uint8_t *iv, int32_t iv_length) {
	LLSEC_CIPHER_DEBUG_TRACE("%s \n", __func__);
	LLSEC_UNUSED_PARAM(transformation_id);
	LLSEC_PROFILE_START();
	LLSEC_CIPHER_ctx *cipher_ctx = (LLSEC_CIPHER_ctx *)native_id;
	(void)memcpy(cipher_ctx->iv, iv, iv_length);
	LLSEC_PROFILE_END();
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_CIPHER_IMPL_get_IV_length(int32_t transformation_id, int32_t native_id) {
	LLSEC_CIPHER_DEBUG_TRACE("%s \n", __func__);
	LLSEC_UNUSED_PARAM(transformation_id);
	const LLSEC_CIPHER_ctx *cipher_ctx = (const LLSEC_CIPHER_ctx *)native_id;
	return cipher_ctx->iv_length;
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_CIPHER_IMPL_init(int32_t transformation_id, uint8_t is_decrypting, uint8_t *key, int32_t key_length,
// cppcheck-suppress constParameterPointer; SNI does not support immutable array
                               uint8_t *iv, int32_t iv_length) {
	LLSEC_CIPHER_DEBUG_TRACE("%s \n", __func__);
	LLSEC_PROFILE_START();
	int32_t return_code = LLSEC_SUCCESS;
	const LLSEC_CIPHER_transformation *transformation = (const LLSEC_CIPHER_transformation *)transformation_id;
	LLSEC_CIPHER_ctx *cipher_ctx = NULL;

	if (iv_length <= 0) {
		int32_t sni_rc = SNI_throwNativeException(iv_length, "LLSEC_CIPHER_IMPL_init invalid iv length");
		LLSEC_ASSERT(sni_rc == SNI_OK);
		return_code = LLSEC_ERROR;
	}

	if (LLSEC_SUCCESS == return_code) {
		cipher_ctx = mbedtls_calloc(1, sizeof(LLSEC_CIPHER_ctx));
		if (cipher_ctx == NULL) {
			int32_t sni_rc = SNI_throwNativeException(iv_length, "failed to allocate cipher_ctx\n");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			return_code = LLSEC_ERROR;
		}
	}

	if (LLSEC_SUCCESS == return_code) {
		cipher_ctx->iv = mbedtls_calloc(1, sizeof(LLSEC_CIPHER_ctx));
		if (cipher_ctx->iv == NULL) {
			int32_t sni_rc = SNI_throwNativeException(iv_length, "failed to allocate iv\n");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			return_code = LLSEC_ERROR;
		}
	}

	if (LLSEC_SUCCESS == return_code) {
		cipher_ctx->iv_length = iv_length;
		(void)memcpy(cipher_ctx->iv, iv, iv_length);

		return_code = transformation->init(cipher_ctx, is_decrypting, key, key_length);
		if (LLSEC_SUCCESS != return_code) {
			int32_t sni_rc = SNI_throwNativeException(return_code, "LLSEC_CIPHER_IMPL_init failed");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			return_code = LLSEC_ERROR;
		}
	}

	if (LLSEC_SUCCESS == return_code) {
		/* cppcheck-suppress misra-c2012-11.8 ; we can safely remove const qualifier from a function pointer.
		 * Transformation is const so close is technically a const pointer to a function. If for any reason
		 * SNI_unregisterResource decided to change the pointer value, it wouldn't have any side effect. */
		if (SNI_registerResource(cipher_ctx, transformation->close, NULL) != SNI_OK) {
			int32_t sni_rc = SNI_throwNativeException(LLSEC_ERROR, "Can't register SNI native resource");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			transformation->close(cipher_ctx);
			return_code = LLSEC_ERROR;
		}
	}

	if (LLSEC_SUCCESS == return_code) {
		return_code = (int32_t)(cipher_ctx);
	} else {
		// fail case, give back resources
		if (cipher_ctx != NULL) {
			if (cipher_ctx->iv != NULL) {
				mbedtls_free(cipher_ctx);
			}
			mbedtls_free(cipher_ctx);
		}
	}

	LLSEC_PROFILE_END();
	return return_code;
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_CIPHER_IMPL_decrypt(int32_t transformation_id, int32_t native_id, uint8_t *buffer, int32_t buffer_offset,
                                  int32_t buffer_length, uint8_t *output, int32_t output_offset) {
	LLSEC_CIPHER_DEBUG_TRACE("%s \n", __func__);
	LLSEC_PROFILE_START();
	int32_t return_code = LLSEC_SUCCESS;
	const LLSEC_CIPHER_transformation *transformation = (const LLSEC_CIPHER_transformation *)transformation_id;
	LLSEC_CIPHER_ctx *cipher_ctx = (LLSEC_CIPHER_ctx *)native_id;

	int rc = transformation->decrypt(cipher_ctx, &buffer[buffer_offset], buffer_length, &output[output_offset]);
	if (LLSEC_SUCCESS != rc) {
		int32_t sni_rc = SNI_throwNativeException(rc, "LLSEC_CIPHER_IMPL_decrypt failed");
		LLSEC_ASSERT(sni_rc == SNI_OK);
		return_code = LLSEC_ERROR;
	} else {
		return_code = buffer_length;
	}
	LLSEC_PROFILE_END();
	return return_code;
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_CIPHER_IMPL_encrypt(int32_t transformation_id, int32_t native_id, uint8_t *buffer, int32_t buffer_offset,
                                  int32_t buffer_length, uint8_t *output, int32_t output_offset) {
	LLSEC_CIPHER_DEBUG_TRACE("%s \n", __func__);
	LLSEC_PROFILE_START();
	int32_t return_code = LLSEC_SUCCESS;
	const LLSEC_CIPHER_transformation *transformation = (const LLSEC_CIPHER_transformation *)transformation_id;
	LLSEC_CIPHER_ctx *cipher_ctx = (LLSEC_CIPHER_ctx *)native_id;

	int rc = transformation->encrypt(cipher_ctx, &buffer[buffer_offset], buffer_length, &output[output_offset]);
	if (LLSEC_SUCCESS != rc) {
		int32_t sni_rc = SNI_throwNativeException(rc, "LLSEC_CIPHER_IMPL_encrypt failed");
		LLSEC_ASSERT(sni_rc == SNI_OK);
		return_code = LLSEC_ERROR;
	} else {
		return_code = buffer_length;
	}
	LLSEC_PROFILE_END();
	return return_code;
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
void LLSEC_CIPHER_IMPL_close(int32_t transformation_id, int32_t native_id) {
	LLSEC_CIPHER_DEBUG_TRACE("%s \n", __func__);
	LLSEC_PROFILE_START();
	const LLSEC_CIPHER_transformation *transformation = (const LLSEC_CIPHER_transformation *)transformation_id;
	LLSEC_CIPHER_ctx *cipher_ctx = (LLSEC_CIPHER_ctx *)native_id;

	transformation->close(cipher_ctx);
	/* cppcheck-suppress misra-c2012-11.8 ; we can safely remove const qualifier from a function pointer.
	 * Transformation is const so close is technically a const pointer to a function. If for any reason
	 * SNI_unregisterResource decided to change the pointer value, it wouldn't have any side effect. */
	if (SNI_OK != SNI_unregisterResource(cipher_ctx, transformation->close)) {
		int32_t sni_rc = SNI_throwNativeException(LLSEC_ERROR, "Can't unregister SNI native resource");
		LLSEC_ASSERT(sni_rc == SNI_OK);
	}
	LLSEC_PROFILE_END();
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_CIPHER_IMPL_get_close_id(int32_t transformation_id) {
	LLSEC_CIPHER_DEBUG_TRACE("%s \n", __func__);
	const LLSEC_CIPHER_transformation *transformation = (const LLSEC_CIPHER_transformation *)transformation_id;
	// cppcheck-suppress [misra-c2012-11.1, misra-c2012-11.6] // Abstract data type for SNI usage
	return (int32_t)transformation->close;
}

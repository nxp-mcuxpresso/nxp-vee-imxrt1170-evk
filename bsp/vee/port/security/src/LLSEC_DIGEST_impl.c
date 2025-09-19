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

#include <LLSEC_DIGEST_impl.h>
#include <LLSEC_CONSTANTS.h>
#include <LLSEC_mbedtls.h>
#include <sni.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "mbedtls/platform.h"
#include "mbedtls/md.h"

#define MD5_DIGEST_LENGTH     (16)
#define SHA1_DIGEST_LENGTH    (20)
#define SHA256_DIGEST_LENGTH  (32)
#define SHA512_DIGEST_LENGTH  (64)

typedef int (*LLSEC_DIGEST_init)(void **native_id);
typedef int (*LLSEC_DIGEST_update)(void *native_id, const uint8_t *buffer, int32_t buffer_length);
typedef int (*LLSEC_DIGEST_digest)(void *native_id, uint8_t *out, int32_t *out_length);
typedef void (*LLSEC_DIGEST_close)(void *native_id);

/*
 * LL-API related functions & struct
 */
typedef struct {
	char *name;
	LLSEC_DIGEST_init init;
	LLSEC_DIGEST_update update;
	LLSEC_DIGEST_digest digest;
	LLSEC_DIGEST_close close;
	LLSEC_DIGEST_algorithm_desc description;
} LLSEC_DIGEST_algorithm;

static int mbedtls_digest_update(void *native_id, const uint8_t *buffer, int32_t buffer_length);
static int mbedtls_digest_digest(void *native_id, uint8_t *out, int32_t *out_length);
static int LLSEC_DIGEST_MD5_init(void **native_id);
static int LLSEC_DIGEST_SHA1_init(void **native_id);
static int LLSEC_DIGEST_SHA256_init(void **native_id);
static int LLSEC_DIGEST_SHA512_init(void **native_id);
static void mbedtls_digest_close(void *native_id);

// cppcheck-suppress misra-c2012-8.9 // Define here for code readability even if it called once in this file.
static const LLSEC_DIGEST_algorithm available_digest_algorithms[4] = {
	{
		.name = "MD5",
		.init = LLSEC_DIGEST_MD5_init,
		.update = mbedtls_digest_update,
		.digest = mbedtls_digest_digest,
		.close = mbedtls_digest_close,
		.description = {
			.digest_length = MD5_DIGEST_LENGTH
		}
	},
	{
		.name = "SHA-1",
		.init = LLSEC_DIGEST_SHA1_init,
		.update = mbedtls_digest_update,
		.digest = mbedtls_digest_digest,
		.close = mbedtls_digest_close,
		.description = {
			.digest_length = SHA1_DIGEST_LENGTH
		}
	},
	{
		.name = "SHA-256",
		.init = LLSEC_DIGEST_SHA256_init,
		.update = mbedtls_digest_update,
		.digest = mbedtls_digest_digest,
		.close = mbedtls_digest_close,
		.description = {
			.digest_length = SHA256_DIGEST_LENGTH
		}
	},
	{
		.name = "SHA-512",
		.init = LLSEC_DIGEST_SHA512_init,
		.update = mbedtls_digest_update,
		.digest = mbedtls_digest_digest,
		.close = mbedtls_digest_close,
		.description = {
			.digest_length = SHA512_DIGEST_LENGTH
		}
	}
};

/*
 * Generic mbedtls function
 */
static int mbedtls_digest_update(void *native_id, const uint8_t *buffer, int32_t buffer_length) {
	LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);

	mbedtls_md_context_t *md_ctx = (mbedtls_md_context_t *)native_id;
	int mbedtls_rc = mbedtls_md_update(md_ctx, buffer, buffer_length);
	return mbedtls_rc;
}

static int mbedtls_digest_digest(void *native_id, uint8_t *out, int32_t *out_length) {
	LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);

	mbedtls_md_context_t *md_ctx = (mbedtls_md_context_t *)native_id;
	int mbedtls_rc = mbedtls_md_finish(md_ctx, out);

	if (LLSEC_MBEDTLS_SUCCESS == mbedtls_rc) {
		*out_length = strlen((char *)out);
	}

	return mbedtls_rc;
}

static void mbedtls_digest_close(void *native_id) {
	LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);

	mbedtls_md_context_t *md_ctx = (mbedtls_md_context_t *)native_id;

	/* Memory deallocation */
	mbedtls_md_free(md_ctx);
	mbedtls_free(md_ctx);
}

/*
 * Specific md5 function
 */
static int LLSEC_DIGEST_MD5_init(void **native_id) {
	int return_code = LLSEC_SUCCESS;
	LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);

	mbedtls_md_context_t *md_ctx = mbedtls_calloc(1, sizeof(mbedtls_md_context_t));
	if (NULL == md_ctx) {
		return_code = LLSEC_ERROR;
	}

	if (LLSEC_SUCCESS == return_code) {
		mbedtls_md_init(md_ctx);
		int mbedtls_rc = mbedtls_md_setup(md_ctx, mbedtls_md_info_from_type(MBEDTLS_MD_MD5), 0);
		if (LLSEC_MBEDTLS_SUCCESS != mbedtls_rc) {
			return_code = LLSEC_ERROR;
		}
	}

	if (LLSEC_SUCCESS == return_code) {
		int mbedtls_rc = mbedtls_md_starts(md_ctx);
		if (LLSEC_MBEDTLS_SUCCESS != mbedtls_rc) {
			return_code = LLSEC_ERROR;
		}
	}

	if (LLSEC_SUCCESS != return_code) {
		mbedtls_md_free(md_ctx);
		mbedtls_free(md_ctx);
	} else {
		*native_id = md_ctx;
	}

	return return_code;
}

/*
 * Specific sha-1 function
 */
static int LLSEC_DIGEST_SHA1_init(void **native_id) {
	int return_code = LLSEC_SUCCESS;
	LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);

	mbedtls_md_context_t *md_ctx = mbedtls_calloc(1, sizeof(mbedtls_md_context_t));
	if (NULL == md_ctx) {
		return_code = LLSEC_ERROR;
	}

	if (LLSEC_SUCCESS == return_code) {
		mbedtls_md_init(md_ctx);
		int mbedtls_rc = mbedtls_md_setup(md_ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA1), 0);
		if (LLSEC_MBEDTLS_SUCCESS != mbedtls_rc) {
			return_code = LLSEC_ERROR;
		}
	}

	if (LLSEC_SUCCESS == return_code) {
		int mbedtls_rc = mbedtls_md_starts(md_ctx);
		if (LLSEC_MBEDTLS_SUCCESS != mbedtls_rc) {
			return_code = LLSEC_ERROR;
		}
	}

	if (LLSEC_SUCCESS != return_code) {
		mbedtls_md_free(md_ctx);
		mbedtls_free(md_ctx);
	} else {
		*native_id = md_ctx;
	}

	return return_code;
}

/*
 * Specific sha-256 function
 */
static int LLSEC_DIGEST_SHA256_init(void **native_id) {
	int return_code = LLSEC_SUCCESS;
	LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);

	mbedtls_md_context_t *md_ctx = mbedtls_calloc(1, sizeof(mbedtls_md_context_t));
	if (NULL == md_ctx) {
		return_code = LLSEC_ERROR;
	}

	if (LLSEC_SUCCESS == return_code) {
		mbedtls_md_init(md_ctx);
		int mbedtls_rc = mbedtls_md_setup(md_ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA256), 0);
		if (LLSEC_MBEDTLS_SUCCESS != mbedtls_rc) {
			return_code = LLSEC_ERROR;
		}
	}

	if (LLSEC_SUCCESS == return_code) {
		int mbedtls_rc = mbedtls_md_starts(md_ctx);
		if (LLSEC_MBEDTLS_SUCCESS != mbedtls_rc) {
			return_code = LLSEC_ERROR;
		}
	}

	if (LLSEC_SUCCESS != return_code) {
		mbedtls_md_free(md_ctx);
		mbedtls_free(md_ctx);
	} else {
		*native_id = md_ctx;
	}

	return return_code;
}

/*
 * Specific sha-512 function
 */
static int LLSEC_DIGEST_SHA512_init(void **native_id) {
	int return_code = LLSEC_SUCCESS;
	LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);

	mbedtls_md_context_t *md_ctx = mbedtls_calloc(1, sizeof(mbedtls_md_context_t));
	if (NULL == md_ctx) {
		return_code = LLSEC_ERROR;
	}

	if (LLSEC_SUCCESS == return_code) {
		mbedtls_md_init(md_ctx);
		int mbedtls_rc = mbedtls_md_setup(md_ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA512), 0);
		if (LLSEC_MBEDTLS_SUCCESS != mbedtls_rc) {
			return_code = LLSEC_ERROR;
		}
	}

	if (LLSEC_SUCCESS == return_code) {
		int mbedtls_rc = mbedtls_md_starts(md_ctx);
		if (LLSEC_MBEDTLS_SUCCESS != mbedtls_rc) {
			return_code = LLSEC_ERROR;
		}
	}

	if (LLSEC_SUCCESS != return_code) {
		mbedtls_md_free(md_ctx);
		mbedtls_free(md_ctx);
	} else {
		*native_id = md_ctx;
	}

	return return_code;
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_DIGEST_IMPL_get_algorithm_description(uint8_t *algorithm_name,
                                                    LLSEC_DIGEST_algorithm_desc *algorithm_desc) {
	int32_t return_code = LLSEC_ERROR;
	LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);
	LLSEC_PROFILE_START();
	int32_t nb_algorithms = sizeof(available_digest_algorithms) / sizeof(LLSEC_DIGEST_algorithm);
	const LLSEC_DIGEST_algorithm *algorithm = &available_digest_algorithms[0];

	while (--nb_algorithms >= 0) {
		if (strcmp((char *)algorithm_name, (algorithm->name)) == 0) {
			(void)memcpy(algorithm_desc, &(algorithm->description), sizeof(LLSEC_DIGEST_algorithm_desc));
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
int32_t LLSEC_DIGEST_IMPL_init(int32_t algorithm_id) {
	int32_t return_code = LLSEC_SUCCESS;
	LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);
	LLSEC_PROFILE_START();
	void *native_id = NULL;
	const LLSEC_DIGEST_algorithm *algorithm = (const LLSEC_DIGEST_algorithm *)algorithm_id;

	return_code = algorithm->init((void **)&native_id);

	if (LLSEC_SUCCESS != return_code) {
		int32_t sni_rc = SNI_throwNativeException(return_code, "LLSEC_DIGEST_IMPL_init failed");
		LLSEC_ASSERT(sni_rc == SNI_OK);
	} else {
		/* register SNI native resource */
		// cppcheck-suppress misra-c2012-11.8 ; no risk in removing const qualifier from a pointer to a function
		if (SNI_registerResource(native_id, algorithm->close, NULL) != SNI_OK) {
			int32_t sni_rc = SNI_throwNativeException(LLSEC_ERROR, "Can't register SNI native resource");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			algorithm->close((void *)native_id);
			return_code = LLSEC_ERROR;
		} else {
			// cppcheck-suppress misra-c2012-11.6 // Abstract data type for SNI usage
			return_code = (int32_t)native_id;
		}
	}
	LLSEC_PROFILE_END();
	return return_code;
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
void LLSEC_DIGEST_IMPL_close(int32_t algorithm_id, int32_t native_id) {
	LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);
	LLSEC_PROFILE_START();
	const LLSEC_DIGEST_algorithm *algorithm = (const LLSEC_DIGEST_algorithm *)algorithm_id;

	// cppcheck-suppress misra-c2012-11.6 // Abstract data type for SNI usage
	algorithm->close((void *)native_id);

	// cppcheck-suppress misra-c2012-11.6 // Abstract data type for SNI usage
	// cppcheck-suppress misra-c2012-11.1 // Abstract data type for SNI usage
	// cppcheck-suppress misra-c2012-11.8 // no risk in removing const qualifier from a pointer to a function
	if (SNI_OK != SNI_unregisterResource((void *)native_id, (SNI_closeFunction)algorithm->close)) {
		int32_t sni_rc = SNI_throwNativeException(LLSEC_ERROR, "Can't unregister SNI native resource");
		LLSEC_ASSERT(sni_rc == SNI_OK);
	}
	LLSEC_PROFILE_END();
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
void LLSEC_DIGEST_IMPL_update(int32_t algorithm_id, int32_t native_id, uint8_t *buffer, int32_t buffer_offset,
                              int32_t buffer_length) {
	LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);
	LLSEC_PROFILE_START();
	const LLSEC_DIGEST_algorithm *algorithm = (const LLSEC_DIGEST_algorithm *)algorithm_id;
	// cppcheck-suppress misra-c2012-11.6 // Abstract data type for SNI usage
	int return_code = algorithm->update((void *)native_id, &buffer[buffer_offset], buffer_length);

	if (LLSEC_SUCCESS != return_code) {
		int32_t sni_rc = SNI_throwNativeException(return_code, "LLSEC_DIGEST_IMPL_update failed");
		LLSEC_ASSERT(sni_rc == SNI_OK);
	}
	LLSEC_PROFILE_END();
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
void LLSEC_DIGEST_IMPL_digest(int32_t algorithm_id, int32_t native_id, uint8_t *out, int32_t out_offset,
                              int32_t out_length) {
	LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);
	LLSEC_PROFILE_START();
	const LLSEC_DIGEST_algorithm *algorithm = (const LLSEC_DIGEST_algorithm *)algorithm_id;
	// cppcheck-suppress misra-c2012-11.6 // Abstract data type for SNI usage
	int return_code = algorithm->digest((void *)native_id, &out[out_offset], &out_length);

	if (LLSEC_SUCCESS != return_code) {
		int32_t sni_rc = SNI_throwNativeException(return_code, "LLSEC_DIGEST_IMPL_digest failed");
		LLSEC_ASSERT(sni_rc == SNI_OK);
	}
	LLSEC_PROFILE_END();
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_DIGEST_IMPL_get_close_id(int32_t algorithm_id) {
	LLSEC_DIGEST_DEBUG_TRACE("%s \n", __func__);
	const LLSEC_DIGEST_algorithm *algorithm = (const LLSEC_DIGEST_algorithm *)algorithm_id;
	// cppcheck-suppress [misra-c2012-11.1, misra-c2012-11.6] // Abstract data type for SNI usage
	return (int32_t)algorithm->close;
}

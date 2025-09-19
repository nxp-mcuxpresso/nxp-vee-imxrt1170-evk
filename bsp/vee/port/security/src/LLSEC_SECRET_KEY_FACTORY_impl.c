/*
 * C
 *
 * Copyright 2023-2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief MicroEJ Security low level API
 * @author MicroEJ Developer Team
 * @version 2.0.1
 */

// set to 1 to enable profiling
#define LLSEC_PROFILE   0

#include <LLSEC_SECRET_KEY_FACTORY_impl.h>
#include <LLSEC_CONSTANTS.h>
#include <LLSEC_mbedtls.h>
#include <sni.h>
#include <string.h>
#include "mbedtls/platform.h"
#include "mbedtls/pkcs5.h"

// Test version superior or equal to 3.5
#define MBEDTLS_VERSION_GE_3_5            (MBEDTLS_VERSION_NUMBER >= 0x03050000)

typedef int32_t (*LLSEC_SECRET_KEY_FACTORY_get_key_data)(LLSEC_secret_key *secret_key, mbedtls_md_type_t md_type,
                                                         uint8_t *password, int32_t password_length, uint8_t *salt,
                                                         int32_t salt_length, int32_t iterations, int32_t key_length,
                                                         const char **reason);
typedef void (*LLSEC_SECRET_KEY_FACTORY_key_close)(void *native_id);

static int32_t LLSEC_SECRET_KEY_FACTORY_PBKDF2_mbedtls_get_key_data(LLSEC_secret_key *secret_key,
                                                                    mbedtls_md_type_t md_type, uint8_t *password,
                                                                    int32_t password_length, uint8_t *salt,
                                                                    int32_t salt_length, int32_t iterations,
                                                                    int32_t key_length, const char **reason);
static void LLSEC_SECRET_KEY_FACTORY_PBKDF2_mbedtls_key_close(void *native_id);
static void LLSEC_SECRET_KEY_FACTORY_mbedtls_free_secret_key(LLSEC_secret_key *secret_key);

struct LLSEC_SECRET_KEY_FACTORY_IMPL_algorithm {
	const char *name;
	mbedtls_md_type_t md_type;
	LLSEC_SECRET_KEY_FACTORY_get_key_data get_key_data;
	LLSEC_SECRET_KEY_FACTORY_key_close key_close;
};

// cppcheck-suppress misra-c2012-8.9 // Define here for code readability even if it called once in this file.
static const LLSEC_SECRET_KEY_FACTORY_IMPL_algorithm available_algorithms[5] = {
	{
		.name = "PBKDF2WithHmacSHA1",
		.md_type = MBEDTLS_MD_SHA1,
		.get_key_data = LLSEC_SECRET_KEY_FACTORY_PBKDF2_mbedtls_get_key_data,
		.key_close = LLSEC_SECRET_KEY_FACTORY_PBKDF2_mbedtls_key_close
	},
	{
		.name = "PBKDF2WithHmacSHA224",
		.md_type = MBEDTLS_MD_SHA224,
		.get_key_data = LLSEC_SECRET_KEY_FACTORY_PBKDF2_mbedtls_get_key_data,
		.key_close = LLSEC_SECRET_KEY_FACTORY_PBKDF2_mbedtls_key_close
	},
	{
		.name = "PBKDF2WithHmacSHA256",
		.md_type = MBEDTLS_MD_SHA256,
		.get_key_data = LLSEC_SECRET_KEY_FACTORY_PBKDF2_mbedtls_get_key_data,
		.key_close = LLSEC_SECRET_KEY_FACTORY_PBKDF2_mbedtls_key_close
	},
	{
		.name = "PBKDF2WithHmacSHA384",
		.md_type = MBEDTLS_MD_SHA384,
		.get_key_data = LLSEC_SECRET_KEY_FACTORY_PBKDF2_mbedtls_get_key_data,
		.key_close = LLSEC_SECRET_KEY_FACTORY_PBKDF2_mbedtls_key_close
	},
	{
		.name = "PBKDF2WithHmacSHA512",
		.md_type = MBEDTLS_MD_SHA512,
		.get_key_data = LLSEC_SECRET_KEY_FACTORY_PBKDF2_mbedtls_get_key_data,
		.key_close = LLSEC_SECRET_KEY_FACTORY_PBKDF2_mbedtls_key_close
	}
};

static void LLSEC_SECRET_KEY_FACTORY_mbedtls_free_secret_key(LLSEC_secret_key *secret_key) {
	if (NULL != secret_key->key) {
		mbedtls_free(secret_key->key);
	}
	if (NULL != secret_key) {
		mbedtls_free(secret_key);
	}
}

static int32_t LLSEC_SECRET_KEY_FACTORY_PBKDF2_mbedtls_get_key_data(LLSEC_secret_key *secret_key,
                                                                    mbedtls_md_type_t md_type,
                                                                    uint8_t *password, int32_t password_length,
                                                                    uint8_t *salt,
                                                                    int32_t salt_length, int32_t iterations,
                                                                    int32_t key_length, const char **reason) {
	LLSEC_SECRET_KEY_FACTORY_DEBUG_TRACE("%s \n", __func__);

	int32_t return_code = LLSEC_SUCCESS;
#if !(MBEDTLS_VERSION_GE_3_5)
	mbedtls_md_context_t md_ctx;
	const mbedtls_md_info_t *md_info = NULL;
#endif

	/* Allocate resources */
	secret_key->key = mbedtls_calloc(1, key_length);
	if (NULL == secret_key->key) {
		*reason = "mbedtls_calloc() failed";
		return_code = LLSEC_ERROR;
	}

#if !(MBEDTLS_VERSION_GE_3_5)
	/* Initialize HMAC context - not needed anymore for mbedTLS >= 3.5.0 */
	if (LLSEC_SUCCESS == return_code) {
		mbedtls_md_init(&md_ctx);
		md_info = mbedtls_md_info_from_type(md_type);
		if (NULL == md_info) {
			*reason = "mbedtls_md_info_from_type() failed";
			return_code = LLSEC_ERROR;
		}
	}

	/* Setup HMAC context - not needed anymore for mbedTLS >= 3.5.0 */
	if (LLSEC_SUCCESS == return_code) {
		int mbedtls_rc = mbedtls_md_setup(&md_ctx, md_info, 1);
		if (LLSEC_MBEDTLS_SUCCESS != mbedtls_rc) {
			LLSEC_SECRET_KEY_FACTORY_DEBUG_TRACE("%s mbedtls_md_setup() failed (rc = %d)\n", __func__, mbedtls_rc);
			*reason = "mbedtls_md_setup() failed";
			return_code = LLSEC_ERROR;
		}
	}
#endif

	/* PKCS#5 PBKDF2 using HMAC */
	if (LLSEC_SUCCESS == return_code) {
		secret_key->key_length = key_length;
#if (MBEDTLS_VERSION_GE_3_5)
		int mbedtls_rc = mbedtls_pkcs5_pbkdf2_hmac_ext(md_type,
#else
		int mbedtls_rc = mbedtls_pkcs5_pbkdf2_hmac((mbedtls_md_context_t *)&md_ctx,
#endif
		                                               (const unsigned char *)password,
		                                               (size_t)password_length,
		                                               (const unsigned char *)salt,
		                                               (size_t)salt_length,
		                                               (unsigned int)iterations,
		                                               (uint32_t)secret_key->key_length,
		                                               (unsigned char *)secret_key->key);
		if (LLSEC_MBEDTLS_SUCCESS != mbedtls_rc) {
			LLSEC_SECRET_KEY_FACTORY_DEBUG_TRACE("%s mbedtls_pkcs5_pbkdf2_hmac() failed (rc = %d)\n", __func__,
			                                     mbedtls_rc);
			*reason = "mbedtls_pkcs5_pbkdf2_hmac() failed";
			return_code = LLSEC_ERROR;
		}
	}

#if !(MBEDTLS_VERSION_GE_3_5)
	/* Release HMAC context */
	mbedtls_md_free(&md_ctx);
#endif

	return return_code;
}

static void LLSEC_SECRET_KEY_FACTORY_PBKDF2_mbedtls_key_close(void *native_id) {
	LLSEC_SECRET_KEY_FACTORY_DEBUG_TRACE("%s (native_id = %p)\n", __func__, native_id);
	LLSEC_secret_key *secret_key = (LLSEC_secret_key *)native_id;

	/* Release resources */
	LLSEC_SECRET_KEY_FACTORY_mbedtls_free_secret_key(secret_key);

	/* Unregister SNI close callback */
	if (SNI_OK != SNI_unregisterResource((void *)native_id,
	                                     (SNI_closeFunction)LLSEC_SECRET_KEY_FACTORY_PBKDF2_mbedtls_key_close)) {
		(void) SNI_throwNativeException(LLSEC_ERROR, "Can't unregister SNI native resource");
	}
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_SECRET_KEY_FACTORY_IMPL_get_algorithm(uint8_t *algorithm_name) {
	LLSEC_SECRET_KEY_FACTORY_DEBUG_TRACE("%s \n", __func__);
	LLSEC_PROFILE_START();
	int32_t return_code = LLSEC_ERROR;
	int32_t nb_algorithms = sizeof(available_algorithms) / sizeof(LLSEC_SECRET_KEY_FACTORY_IMPL_algorithm);
	const LLSEC_SECRET_KEY_FACTORY_IMPL_algorithm *algorithm = &available_algorithms[0];

	/* Check corresponding algorithm */
	while (--nb_algorithms >= 0) {
		if (strcmp((char *)algorithm_name, algorithm->name) == 0) {
			LLSEC_SECRET_KEY_FACTORY_DEBUG_TRACE("%s Algorithm %s found\n", __func__, algorithm->name);
			break;
		}
		algorithm++;
	}

	if (0 <= nb_algorithms) {
		return_code = (int32_t)algorithm;
	}
	LLSEC_SECRET_KEY_FACTORY_DEBUG_TRACE("%s Return handler = %d\n", __func__, (int)return_code);

	LLSEC_PROFILE_END();
	return return_code;
}

static void LLSEC_SECRET_KEY_FACTORY_IMPL_get_key_data_job(MICROEJ_ASYNC_WORKER_job_t *job) {
	int32_t return_code = LLSEC_SUCCESS;
	struct LLSEC_SECRET_KEY_FACTORY_get_key_data_params *params =
		(struct LLSEC_SECRET_KEY_FACTORY_get_key_data_params *)job->params;

	/* Allocate secret key structure */
	params->key = (LLSEC_secret_key *)mbedtls_calloc(1, sizeof(LLSEC_secret_key));
	if (NULL == params->key) {
		params->reason = "Can't allocate LLSEC_secret_key structure";
		return_code = LLSEC_ERROR;
	}

	if (LLSEC_SUCCESS == return_code) {
		const LLSEC_SECRET_KEY_FACTORY_IMPL_algorithm *algorithm = params->algorithm;
		return_code = algorithm->get_key_data(params->key, algorithm->md_type, params->password,
		                                      params->password_length,
		                                      params->salt, params->salt_length, params->iterations,
		                                      params->key_length / 8, &params->reason);
	}

	if (LLSEC_SUCCESS != return_code) {
		mbedtls_free(params->key);
	}

	params->rc = return_code;
}

static int32_t LLSEC_SECRET_KEY_FACTORY_IMPL_get_key_data_job_done(int32_t algorithm_id, uint8_t *password,
                                                                   int32_t password_length,
                                                                   uint8_t *salt, int32_t salt_length,
                                                                   int32_t iterations, int32_t key_length) {
	LLSEC_UNUSED_PARAM(algorithm_id);
	LLSEC_UNUSED_PARAM(password);
	LLSEC_UNUSED_PARAM(password_length);
	LLSEC_UNUSED_PARAM(salt);
	LLSEC_UNUSED_PARAM(salt_length);
	LLSEC_UNUSED_PARAM(iterations);
	LLSEC_UNUSED_PARAM(key_length);

	int32_t result = LLSEC_SUCCESS;
	const char *reason = NULL;
	struct LLSEC_SECRET_KEY_FACTORY_get_key_data_params *params = NULL;
	MICROEJ_ASYNC_WORKER_job_t *job = MICROEJ_ASYNC_WORKER_get_job_done();
	if (job == NULL) {
		result = LLSEC_ERROR;
		reason = "can't retrieve job";
	}

	if (result == LLSEC_SUCCESS) {
		params = (struct LLSEC_SECRET_KEY_FACTORY_get_key_data_params *)job->params;
		// retrieve returned code from job
		result = params->rc;
		reason = params->reason;
	}

	// register closure if the key was allocated
	if (result == LLSEC_SUCCESS) {
		const LLSEC_SECRET_KEY_FACTORY_IMPL_algorithm *algorithm = params->algorithm;
		// cppcheck-suppress misra-c2012-11.8 // no risk in removing const qualifier from a pointer to a function
		if (SNI_OK != SNI_registerResource(params->key, algorithm->key_close, NULL)) {
			algorithm->key_close(params->key);
			result = LLSEC_ERROR;
			reason = "Can't register closure";
		}
	}

	if (result == LLSEC_SUCCESS) {
		// return a pointer to the generated key
		result = (int32_t)params->key;
	} else {
		// something went wrong, throw an exception
		int32_t sni_rc = SNI_throwNativeException(result, reason);
		LLSEC_ASSERT(sni_rc == SNI_OK);
	}

	if (params != NULL) {
		if (params->password != NULL) {
			mbedtls_platform_zeroize(params->password, params->password_length);
			mbedtls_free(params->password);
		}
		if (params->salt != NULL) {
			mbedtls_free(params->salt);
		}
	}

	MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_free_job(&llsec_worker, job);
	LLSEC_ASSERT(status == MICROEJ_ASYNC_WORKER_OK);
	return result;
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_SECRET_KEY_FACTORY_IMPL_get_key_data(int32_t algorithm_id, uint8_t *password, int32_t password_length,
                                                   uint8_t *salt, int32_t salt_length, int32_t iterations,
                                                   int32_t key_length) {
	LLSEC_SECRET_KEY_FACTORY_DEBUG_TRACE("%s password length = %d, salt length = %d, key length = %d (handler = %d)\n",
	                                     __func__, (int)password_length, (int)salt_length, (int)key_length,
	                                     (int)algorithm_id);
	// control flow
	int result = LLSEC_SUCCESS;
	MICROEJ_ASYNC_WORKER_job_t *job = NULL;
	struct LLSEC_SECRET_KEY_FACTORY_get_key_data_params *params = NULL;

	if ((password_length < 0) || (password_length > LLSEC_SECRET_KEY_MAX_PWD_LEN)) {
		int32_t sni_rc = SNI_throwNativeException(SNI_ILLEGAL_ARGUMENT,
		                                          "password length must be " STR(
													  LLSEC_SECRET_KEY_MAX_PWD_LEN) " chars at most");
		LLSEC_ASSERT(sni_rc == SNI_OK);
		result = LLSEC_INVALID_ARGUMENT;
	}

	if ((result == LLSEC_SUCCESS)
	    && ((salt_length < 0) || (salt_length > LLSEC_SECRET_KEY_MAX_SALT_LEN))) {
		int32_t sni_rc = SNI_throwNativeException(SNI_ILLEGAL_ARGUMENT,
		                                          "salt length must be " STR(
													  LLSEC_SECRET_KEY_MAX_SALT_LEN) " chars at most");
		LLSEC_ASSERT(sni_rc == SNI_OK);
		result = LLSEC_INVALID_ARGUMENT;
	}

	if (result == LLSEC_SUCCESS) {
		job = MICROEJ_ASYNC_WORKER_allocate_job(&llsec_worker,
		                                        (SNI_callback)
		                                        LLSEC_SECRET_KEY_FACTORY_IMPL_get_key_data);
		if (job == NULL) {
			// MICROEJ_ASYNC_WORKER_allocate_job() has thrown a NativeException
			result = LLSEC_ERROR;
		}
	}

	if (result == LLSEC_SUCCESS) {
		params = job->params;
		params->algorithm = (LLSEC_SECRET_KEY_FACTORY_IMPL_algorithm *)algorithm_id;
		params->iterations = iterations;
		params->key_length = key_length;
		params->password = NULL;
		params->password_length = password_length;
		params->salt = NULL;
		params->salt_length = salt_length;

		if (password_length > 0) {
			params->password = mbedtls_calloc(password_length, 1);
			if (params->password == NULL) {
				int32_t sni_rc = SNI_throwNativeException(LLSEC_ERROR, "failed to allocate password buffer");
				LLSEC_ASSERT(sni_rc == SNI_OK);
				result = LLSEC_ERROR;
			}
		}
	}

	if (result == LLSEC_SUCCESS) {
		if (password_length > 0) {
			(void)memcpy(params->password, password, password_length);
		}

		if (salt_length > 0) {
			params->salt = mbedtls_calloc(salt_length, 1);
			if (params->salt == NULL) {
				int32_t sni_rc = SNI_throwNativeException(LLSEC_ERROR, "failed to allocate salt buffer");
				LLSEC_ASSERT(sni_rc == SNI_OK);
				result = LLSEC_ERROR;
			}
		}
	}

	if (result == LLSEC_SUCCESS) {
		if (salt_length > 0) {
			(void)memcpy(params->salt, salt, salt_length);
		}

		MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&llsec_worker, job,
		                                                                       LLSEC_SECRET_KEY_FACTORY_IMPL_get_key_data_job,
		                                                                       (SNI_callback)
		                                                                       LLSEC_SECRET_KEY_FACTORY_IMPL_get_key_data_job_done);
		if (status != MICROEJ_ASYNC_WORKER_OK) {
			// MICROEJ_ASYNC_WORKER_async_exec() has thrown a NativeException
			result = LLSEC_ERROR;
		}
	}

	if (result != LLSEC_SUCCESS) {
		if (params != NULL) {
			if (params->password != NULL) {
				mbedtls_platform_zeroize(params->password, params->password_length);
				mbedtls_free(params->password);
			}

			if (params->salt != NULL) {
				mbedtls_free(params->salt);
			}
		}

		if (job != NULL) {
			/* if something went wrong, free the allocated job */
			MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_free_job(&llsec_worker, job);
			LLSEC_ASSERT(status == MICROEJ_ASYNC_WORKER_OK);
		}
	}

	/* The return value is either ignored because the async worker has been scheduled,
	 * or an exception has been thrown */
	return SNI_IGNORED_RETURNED_VALUE;
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_SECRET_KEY_FACTORY_IMPL_get_close_id(int32_t algorithm_id) {
	LLSEC_SECRET_KEY_FACTORY_DEBUG_TRACE("%s (handler = %d)\n", __func__, (int)algorithm_id);

	const LLSEC_SECRET_KEY_FACTORY_IMPL_algorithm *algorithm = (LLSEC_SECRET_KEY_FACTORY_IMPL_algorithm *)algorithm_id;
	// cppcheck-suppress [misra-c2012-11.1, misra-c2012-11.6] // Abstract data type for SNI usage
	return (int32_t)algorithm->key_close;
}

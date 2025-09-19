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

#include <LLSEC_KEY_PAIR_GENERATOR_impl.h>
#include <LLSEC_CONSTANTS.h>
#include <LLSEC_mbedtls.h>
#include <sni.h>
#include <string.h>

#include "mbedtls/platform.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/dhm.h"
#include "mbedtls/ecdh.h"
#include "mbedtls/ecdsa.h"
#include "mbedtls/entropy.h"
#include "mbedtls/rsa.h"
#include "mbedtls/error.h"

typedef void (*LLSEC_KEY_PAIR_GENERATOR_close)(void *native_id);

//RSA
static int LLSEC_KEY_PAIR_GENERATOR_RSA_mbedtls_generateKeyPair(LLSEC_priv_key *key,
                                                                int32_t rsa_Key_size, int32_t rsa_public_exponent,
                                                                int *mbedtls_rc, const char **reason);

//EC
static int LLSEC_KEY_PAIR_GENERATOR_EC_mbedtls_generateKeyPair(LLSEC_priv_key *key,
                                                               char *ec_curve_stdname, int *mbedtls_rc,
                                                               const char **reason);

//common
static void LLSEC_KEY_PAIR_GENERATOR_mbedtls_close(void *native_id);

struct LLSEC_KEY_PAIR_GENERATOR_algorithm {
	char *name;
	LLSEC_KEY_PAIR_GENERATOR_close close;
};

// cppcheck-suppress misra-c2012-8.9 // Define here for code readability even if it called once in this file.
static const LLSEC_KEY_PAIR_GENERATOR_algorithm supportedAlgorithms[2] = {
	{
		.name = "RSA",
		.close = LLSEC_KEY_PAIR_GENERATOR_mbedtls_close
	},
	{
		.name = "EC",
		.close = LLSEC_KEY_PAIR_GENERATOR_mbedtls_close
	}
};

static int LLSEC_KEY_PAIR_GENERATOR_RSA_mbedtls_generateKeyPair(LLSEC_priv_key *key,
                                                                int32_t rsa_Key_size, int32_t rsa_public_exponent,
                                                                int *mbedtls_rc, const char **reason) {
	LLSEC_KEY_PAIR_GENERATOR_DEBUG_TRACE("%s\n", __func__);
	LLSEC_PROFILE_START();
	int return_code = LLSEC_SUCCESS;
	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_rsa_context *ctx = NULL;
	char pers[LLSEC_PERSONALIZATION_LEN];
	*mbedtls_rc = LLSEC_MBEDTLS_SUCCESS;

	ctx = mbedtls_calloc(1, sizeof(mbedtls_rsa_context)); //RSA key structure
	if (ctx == NULL) {
		LLSEC_KEY_PAIR_GENERATOR_DEBUG_TRACE("%s failed to alloc RSA context", __func__);
		return_code = LLSEC_ERROR;
	}

	if (LLSEC_SUCCESS == return_code) {
		/* init rsa structure: padding PKCS#1 v1.5 */
#if (MBEDTLS_VERSION_MAJOR == 2)
		mbedtls_rsa_init(ctx, MBEDTLS_RSA_PKCS_V15, MBEDTLS_MD_NONE);
#elif (MBEDTLS_VERSION_MAJOR == 3)
		mbedtls_rsa_init(ctx);
#else
		#error "Unsupported mbedTLS major version"
#endif

		mbedtls_entropy_init(&entropy);   //init entropy structure
		mbedtls_ctr_drbg_init(&ctr_drbg); //Initial random structure

		llsec_mbedtls_gen_random_str(pers, LLSEC_PERSONALIZATION_LEN);

		/* update seed according personal string */
		*mbedtls_rc = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const uint8_t *)pers,
		                                    strlen(pers));
		if (LLSEC_MBEDTLS_SUCCESS != *mbedtls_rc) {
			LLSEC_KEY_PAIR_GENERATOR_DEBUG_TRACE("%s mbedtls_ctr_drbg_seed (rc = %d)\n", __func__, *mbedtls_rc);
			*reason = mbedtls_high_level_strerr(*mbedtls_rc);
			return_code = LLSEC_MBEDTLS_ERR;
		}
	}

	if (LLSEC_SUCCESS == return_code) {
		/*Generate ras key pair*/
		*mbedtls_rc = mbedtls_rsa_gen_key(ctx, mbedtls_ctr_drbg_random, //API of generating random
		                                  &ctr_drbg,                    //random structure
		                                  rsa_Key_size,                 //the size of public key
		                                  rsa_public_exponent);         //publick key exponent 0x01001

		if (LLSEC_MBEDTLS_SUCCESS != *mbedtls_rc) {
			LLSEC_KEY_PAIR_GENERATOR_DEBUG_TRACE("%s mbedtls_ctr_drbg_random (rc = %d)", __func__, *mbedtls_rc);
			*reason = mbedtls_high_level_strerr(*mbedtls_rc);
			return_code = LLSEC_MBEDTLS_ERR;
		}
	}

	if (LLSEC_SUCCESS == return_code) {
		key->key = (char *)ctx;
		key->type = TYPE_RSA;
	} else {
		// Clean up allocated memory on error
		if (NULL != ctx) {
			// only free rsa memory if it was allocated successfully
			// free the rsa context if it was initialized
			mbedtls_rsa_free(ctx);
			mbedtls_free(ctx);
		}
	}

	// clean up allocated memory that wont be needed in any case
	mbedtls_ctr_drbg_free(&ctr_drbg);
	mbedtls_entropy_free(&entropy);

	LLSEC_PROFILE_END();
	return return_code;
}

static int LLSEC_KEY_PAIR_GENERATOR_EC_mbedtls_generateKeyPair(LLSEC_priv_key *key,
                                                               char *ec_curve_stdname, int *mbedtls_rc,
                                                               const char **reason) {
	LLSEC_KEY_PAIR_GENERATOR_DEBUG_TRACE("%s (key = %p)\n", __func__, key);
	LLSEC_PROFILE_START();

	int return_code = LLSEC_SUCCESS;
	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
	mbedtls_ecdsa_context *ctx = NULL;
	*mbedtls_rc = LLSEC_MBEDTLS_SUCCESS;

	const mbedtls_ecp_curve_info *curve_info = mbedtls_ecp_curve_info_from_name(ec_curve_stdname);
	if (curve_info == NULL) {
		LLSEC_KEY_PAIR_GENERATOR_DEBUG_TRACE("%s can't find ecp curve '%s'", __func__, ec_curve_stdname);
		*reason = "can't find ecp curve";
		return_code = LLSEC_INVALID_ARGUMENT;
	}

	if (return_code == LLSEC_SUCCESS) {
		ctx = mbedtls_calloc(1, sizeof(mbedtls_ecdsa_context));
		if (ctx == NULL) {
			LLSEC_KEY_PAIR_GENERATOR_DEBUG_TRACE("%s failed to alloc ECDSA context", __func__);
			*reason = "failed to alloc ECDSA context";
			return_code = LLSEC_ERROR;
		}
	}

	if (LLSEC_SUCCESS == return_code) {
		char pers[LLSEC_PERSONALIZATION_LEN];
		mbedtls_ecdsa_init(ctx);

		mbedtls_entropy_init(&entropy);
		mbedtls_ctr_drbg_init(&ctr_drbg);

		llsec_mbedtls_gen_random_str(pers, LLSEC_PERSONALIZATION_LEN);

		/* update seed according personal string */
		*mbedtls_rc = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const uint8_t *)pers,
		                                    strlen(pers));
		if (LLSEC_MBEDTLS_SUCCESS != *mbedtls_rc) {
			*reason = mbedtls_high_level_strerr(*mbedtls_rc);
			return_code = LLSEC_MBEDTLS_ERR;
		}
	}

	if (LLSEC_SUCCESS == return_code) {
		/* Generate ecdsa Key pair */
		*mbedtls_rc = mbedtls_ecdsa_genkey(ctx, curve_info->grp_id, mbedtls_ctr_drbg_random, &ctr_drbg);
		if (LLSEC_MBEDTLS_SUCCESS != *mbedtls_rc) {
			*reason = mbedtls_high_level_strerr(*mbedtls_rc);
			return_code = LLSEC_MBEDTLS_ERR;
		}
	}

	if (LLSEC_SUCCESS == return_code) {
		key->key = (char *)ctx;
		key->type = TYPE_ECDSA;
	} else {
		// Clean up allocated memory on error
		if (NULL != ctx) {
			// only free rsa memory if it was allocated successfully
			// free the ecdsa context if it was initialized
			mbedtls_ecdsa_free(ctx);
			mbedtls_free(ctx);
		}
	}

	// clean up allocated memory that wont be needed in any case
	mbedtls_ctr_drbg_free(&ctr_drbg);
	mbedtls_entropy_free(&entropy);

	LLSEC_PROFILE_END();
	return return_code;
}

static void LLSEC_KEY_PAIR_GENERATOR_mbedtls_close(void *native_id) {
	LLSEC_KEY_PAIR_GENERATOR_DEBUG_TRACE("%s \n", __func__);

	LLSEC_priv_key *key = (LLSEC_priv_key *)native_id;

	if (TYPE_RSA == key->type) {
		mbedtls_rsa_free((mbedtls_rsa_context *)key->key);
	} else {
		mbedtls_ecdsa_free((mbedtls_ecdsa_context *)key->key);
	}
	mbedtls_free(key);
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_KEY_PAIR_GENERATOR_IMPL_get_algorithm(uint8_t *algorithm_name) {
	LLSEC_PROFILE_START();
	int32_t return_code = LLSEC_ERROR;
	LLSEC_KEY_PAIR_GENERATOR_DEBUG_TRACE("%s \n", __func__);

	int32_t nb_algorithms = sizeof(supportedAlgorithms) / sizeof(LLSEC_KEY_PAIR_GENERATOR_algorithm);
	const LLSEC_KEY_PAIR_GENERATOR_algorithm *algorithm = &supportedAlgorithms[0];

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

static void LLSEC_KEY_PAIR_GENERATOR_generateKeyPair_job(MICROEJ_ASYNC_WORKER_job_t *job) {
	struct LLSEC_KEY_PAIR_GENERATOR_generateKeyPair_params *params =
		(struct LLSEC_KEY_PAIR_GENERATOR_generateKeyPair_params *)job->params;
	const LLSEC_KEY_PAIR_GENERATOR_algorithm *algorithm = params->algorithm;

	// assume the key generation will succeed to handle fail case in a single place
	params->rc = LLSEC_SUCCESS;
	params->reason = NULL;

	if (0 == strcmp(algorithm->name, "RSA")) {
		params->rc = LLSEC_KEY_PAIR_GENERATOR_RSA_mbedtls_generateKeyPair(params->key,
		                                                                  params->rsa_key_size,
		                                                                  params->rsa_public_exponent,
		                                                                  &params->mbedtls_rc, &params->reason);
	} else if (0 == strcmp(algorithm->name, "EC")) {
		params->rc = LLSEC_KEY_PAIR_GENERATOR_EC_mbedtls_generateKeyPair(params->key,
		                                                                 params->ec_curve_stdname, &params->mbedtls_rc,
		                                                                 &params->reason);
	} else {
		// Algorithm not found error.
		// This should never happen because the algorithm_id is a valid algorithm at this level.
		params->rc = LLSEC_ERROR;
		params->reason = "unsupported algorithm";
	}
}

static int32_t LLSEC_KEY_PAIR_GENERATOR_generateKeyPair_job_done(int32_t algorithm_id, int32_t rsa_key_size,
                                                                 int32_t rsa_public_exponent,
                                                                 uint8_t *ec_curve_stdname) {
	LLSEC_UNUSED_PARAM(algorithm_id);
	LLSEC_UNUSED_PARAM(rsa_key_size);
	LLSEC_UNUSED_PARAM(rsa_public_exponent);
	LLSEC_UNUSED_PARAM(ec_curve_stdname);

	int32_t result = LLSEC_SUCCESS;
	const char *reason = NULL;
	struct LLSEC_KEY_PAIR_GENERATOR_generateKeyPair_params *params = NULL;
	MICROEJ_ASYNC_WORKER_job_t *job = MICROEJ_ASYNC_WORKER_get_job_done();
	if (job == NULL) {
		result = LLSEC_ERROR;
		reason = "can't retrieve job";
	}

	if (result == LLSEC_SUCCESS) {
		params = (struct LLSEC_KEY_PAIR_GENERATOR_generateKeyPair_params *)job->params;
		// retrieve returned code from job
		result = params->rc;
		reason = params->reason;
	}

	// register close if the key was allocated
	if (result == LLSEC_SUCCESS) {
		int32_t rc = SNI_registerResource(params->key, LLSEC_KEY_PAIR_GENERATOR_mbedtls_close, NULL);

		if (rc != SNI_OK) {
			// free allocated memory if the resource cannot be registered
			LLSEC_KEY_PAIR_GENERATOR_mbedtls_close(params->key);
			result = LLSEC_ERROR;
			reason = "can't register close";
		}
	}

	switch (result) {
	case LLSEC_SUCCESS:
	{
		result = (int32_t)params->key;
		break;
	}
	case LLSEC_MBEDTLS_ERR:
	{
		int32_t sni_rc = SNI_throwNativeException(params->mbedtls_rc, reason);
		LLSEC_ASSERT(sni_rc == SNI_OK);
		result = SNI_IGNORED_RETURNED_VALUE;
		break;
	}
	default:
	{
		int32_t sni_rc = SNI_throwNativeException(result, reason);
		LLSEC_ASSERT(sni_rc == SNI_OK);
		result = SNI_IGNORED_RETURNED_VALUE;
	}
	}

	MICROEJ_ASYNC_WORKER_status_t async_rc = MICROEJ_ASYNC_WORKER_free_job(&llsec_worker, job);
	LLSEC_ASSERT(async_rc == MICROEJ_ASYNC_WORKER_OK);
	return result;
}

int32_t LLSEC_KEY_PAIR_GENERATOR_IMPL_generateKeyPair(int32_t algorithm_id, int32_t rsa_key_size,
                                                      int32_t rsa_public_exponent, uint8_t *ec_curve_stdname) {
	int result = LLSEC_SUCCESS;
	LLSEC_KEY_PAIR_GENERATOR_DEBUG_TRACE("%s \n", __func__);

	MICROEJ_ASYNC_WORKER_job_t *job = MICROEJ_ASYNC_WORKER_allocate_job(&llsec_worker,
	                                                                    (SNI_callback)
	                                                                    LLSEC_KEY_PAIR_GENERATOR_IMPL_generateKeyPair);
	struct LLSEC_KEY_PAIR_GENERATOR_generateKeyPair_params *params = NULL;

	if (job == NULL) {
		// MICROEJ_ASYNC_WORKER_allocate_job() has thrown a NativeException
		result = LLSEC_ERROR;
	}

	if (result == LLSEC_SUCCESS) {
		params = (struct LLSEC_KEY_PAIR_GENERATOR_generateKeyPair_params *)job->params;
		params->key = mbedtls_calloc(1, sizeof(LLSEC_priv_key));
		if (params->key == NULL) {
			int32_t sni_rc = SNI_throwNativeException(SNI_ERROR, "failed to allocate keypair context");
			LLSEC_ASSERT(sni_rc == SNI_OK);

			result = LLSEC_ERROR;
		}
	}

	if (result == LLSEC_SUCCESS) {
		int rc;
		uint32_t discarded;
		params->algorithm = (LLSEC_KEY_PAIR_GENERATOR_algorithm *)algorithm_id;
		params->rsa_key_size = rsa_key_size;
		params->rsa_public_exponent = rsa_public_exponent;

		rc = SNI_retrieveArrayElements((jbyte *)ec_curve_stdname, 0, SNI_getArrayLength(ec_curve_stdname),
		                               (int8_t *)params->ec_curve_stdname_buffer,
		                               sizeof(params->ec_curve_stdname_buffer),
		                               (int8_t **)&params->ec_curve_stdname, &discarded, true);
		if (rc != SNI_OK) {
			int32_t sni_rc = SNI_throwNativeException(rc, "SNI_retrieveArrayElements() failed");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			result = LLSEC_ERROR;
		}
	}

	if (result == LLSEC_SUCCESS) {
		MICROEJ_ASYNC_WORKER_status_t rc = MICROEJ_ASYNC_WORKER_async_exec(
			&llsec_worker, job,
			LLSEC_KEY_PAIR_GENERATOR_generateKeyPair_job,
			(SNI_callback)
			LLSEC_KEY_PAIR_GENERATOR_generateKeyPair_job_done);

		if (rc != MICROEJ_ASYNC_WORKER_OK) {
			// MICROEJ_ASYNC_WORKER_async_exec() has thrown a NativeException
			result = LLSEC_ERROR;
		}
	}

	if ((result != LLSEC_SUCCESS)
	    && (job != NULL)) {
		if (params->key != NULL) {
			mbedtls_free(params->key);
		}
		// Something went wrong while a job was allocated, free it
		MICROEJ_ASYNC_WORKER_status_t async_rc = MICROEJ_ASYNC_WORKER_free_job(&llsec_worker, job);
		LLSEC_ASSERT(async_rc);
	}

	/* Either the thread was suspended, or an error happened and an exception
	 * will be thrown */
	return SNI_IGNORED_RETURNED_VALUE;
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_KEY_PAIR_GENERATOR_IMPL_get_close_id(int32_t algorithm_id) {
	LLSEC_PROFILE_START();
	const LLSEC_KEY_PAIR_GENERATOR_algorithm *algorithm = (LLSEC_KEY_PAIR_GENERATOR_algorithm *)algorithm_id;
	LLSEC_PROFILE_END();
	// cppcheck-suppress [misra-c2012-11.1, misra-c2012-11.6] // Abstract data type for SNI usage
	return (int32_t)algorithm->close;
}

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

#include <LLSEC_SIG_impl.h>
#include <LLSEC_CONSTANTS.h>
#include <LLSEC_mbedtls.h>
#include <sni.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
// cppcheck-suppress misra-c2012-21.10 // Type defined in library used in mbedtls
#include <time.h>
#include <inttypes.h>
#include <stdbool.h>

#include "mbedtls/platform.h"
#include "mbedtls/ctr_drbg.h"
#include "mbedtls/entropy.h"
#include "mbedtls/error.h"
#include "mbedtls/md.h"
#include "mbedtls/pk.h"

/**
 * @brief A function pointer to verify a signature
 *
 * @param[in] algorithm A structure describing the algorithm used to verify the signature
 * @param[in] signature The signature to be verified
 * @param[in] signature_length The number of bytes in signature
 * @param[in] pub_key The public key used to verify the signature
 * @param[in] digest The digest of the message whose signature is to be verified
 * @param[in] digest_len The number of bytes in digest
 * @param[out] mbedtls_rc The return code from mbedtls in case the function returned {@link LLSEC_MBEDTLS_ERR}
 *
 * @return The status of the operation, <LLSEC_SUCCESS if it failed
 * @retval JTRUE The signature was successfully verified
 * @retval JFALSE The signature is invalid
 * @retval LLSEC_MBEDTLS_ERR mbedtls encountered a fault condition. The error code is available in *mbedtls_rc
 */
typedef int (*LLSEC_SIG_verify)(LLSEC_SIG_algorithm *algorithm, const uint8_t *signature, int32_t signature_length,
                                LLSEC_pub_key *pub_key, const uint8_t *digest, int32_t digest_length, int *mbedtls_rc);

/**
 * @brief A function pointer to sign a digest
 *
 * @param[in] algorithm A structure describing the algorithm used to sign the digest
 * @param[out] signature Buffer where the signature will be written
 * @param[inout] signature_length Contains the length of the buffer on entry, the generated signature length
 * if the function returned LLSEC_SUCCESS.
 * @param[in] priv_key Private key used to sign
 * @param[in] digest Digest to be signed
 * @param[in] digest_length Length of digest
 * @param[out] mbedtls_rc The return code from mbedtls in cas the function return {@link LLSEC_MBEDTLS_ERR}
 *
 * @return The status of the operation, <LLSEC_SUCCESS if it failed
 * @retval LLSEC_SUCCESS The signature was successfully generated
 * @retval LLSEC_MBEDTLS_ERR mbedtls encountered a fault condition. The error code is available in *mbedtls_rc
 */
typedef int (*LLSEC_SIG_sign)(LLSEC_SIG_algorithm *algorithm, uint8_t *signature, int32_t *signature_length,
                              LLSEC_priv_key *priv_key, const uint8_t *digest, int32_t digest_length, int *mbedtls_rc);

struct LLSEC_SIG_algorithm {
	char *name;
	char *digest_name;
	char *digest_native_name;
	char *oid;
	LLSEC_SIG_verify verify;
	LLSEC_SIG_sign sign;
};

static int LLSEC_SIG_mbedtls_verify(LLSEC_SIG_algorithm *algorithm, const uint8_t *signature, int32_t signature_length,
                                    LLSEC_pub_key *pub_key, const uint8_t *digest, int32_t digest_length,
                                    int *mbedtls_rc);
static int LLSEC_SIG_mbedtls_sign(LLSEC_SIG_algorithm *algorithm, uint8_t *signature, int32_t *signature_length,
                                  LLSEC_priv_key *priv_key, const uint8_t *digest, int32_t digest_length,
                                  int *mbedtls_rc);

static int LLSEC_SIG_mbedtls_ec_verify(LLSEC_SIG_algorithm *algorithm, const uint8_t *signature,
                                       int32_t signature_length, LLSEC_pub_key *pub_key, const uint8_t *digest,
                                       int32_t digest_length, int *mbedtls_rc);
static int LLSEC_SIG_mbedtls_ec_sign(LLSEC_SIG_algorithm *algorithm, uint8_t *signature, int32_t *signature_length,
                                     LLSEC_priv_key *priv_key, const uint8_t *digest, int32_t digest_length,
                                     int *mbedtls_rc);

static LLSEC_SIG_algorithm available_sig_algorithms[2] = {
	{
		.name = "SHA256withRSA",
		.digest_name = "SHA-256",
		.digest_native_name = "SHA256",
		.oid = "1.2.840.113549.1.1.11",
		.verify = LLSEC_SIG_mbedtls_verify,
		.sign = LLSEC_SIG_mbedtls_sign
	},
	{
		.name = "SHA256withECDSA",
		.digest_name = "SHA-256",
		.digest_native_name = "SHA256",
		.oid = "1.2.840.10045.4.3.2",
		.verify = LLSEC_SIG_mbedtls_ec_verify,
		.sign = LLSEC_SIG_mbedtls_ec_sign
	}
};

static int LLSEC_SIG_mbedtls_ec_verify(LLSEC_SIG_algorithm *algorithm, const uint8_t *signature,
                                       int32_t signature_length, LLSEC_pub_key *pub_key, const uint8_t *digest,
                                       int32_t digest_length, int *mbedtls_rc) {
	LLSEC_UNUSED_PARAM(algorithm);

	LLSEC_SIG_DEBUG_TRACE("%s \n", __func__);

	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;

	int return_code = LLSEC_SUCCESS;
	*mbedtls_rc = LLSEC_MBEDTLS_SUCCESS;

	mbedtls_entropy_init(&entropy);
	mbedtls_ctr_drbg_init(&ctr_drbg);

	const char *pers = llsec_gen_random_str_internal(8);
	if (NULL == pers) {
		LLSEC_SIG_DEBUG_TRACE("%s llsec_gen_random_str_internal: allocation error\n", __func__);
		return_code = LLSEC_ERROR;
	}
	if (LLSEC_SUCCESS == return_code) {
		*mbedtls_rc = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *)pers,
		                                    strlen(pers));
		LLSEC_SIG_DEBUG_TRACE("%s mbedtls_ctr_drbg_seed: %d\n", __func__, *mbedtls_rc);
		if (LLSEC_MBEDTLS_SUCCESS != *mbedtls_rc) {
			return_code = LLSEC_MBEDTLS_ERR;
		}
	}
	if (LLSEC_SUCCESS == return_code) {
		mbedtls_ecdsa_context *ctx = (mbedtls_ecdsa_context *)pub_key->key;
		*mbedtls_rc = mbedtls_ecdsa_read_signature(ctx, digest, (size_t)digest_length, signature, signature_length);
		LLSEC_SIG_DEBUG_TRACE("%s mbedtls_ecdsa_read_signature: %d\n", __func__, *mbedtls_rc);
		switch (MBEDTLS_MODULE_ERROR(*mbedtls_rc)) {
		case 0:
			// this is a valid signature
			return_code = JTRUE;
			break;
		case MBEDTLS_ERR_ECP_VERIFY_FAILED:
			// this is a invalid signature
			return_code = JFALSE;
			break;
		case MBEDTLS_ERR_ECP_BAD_INPUT_DATA:
		case MBEDTLS_ERR_ECP_INVALID_KEY:
		case MBEDTLS_ERR_ECP_SIG_LEN_MISMATCH:
			// everything that could come from poor user inputs is treated as unverified signature
			return_code = JFALSE;
			break;
		default:
			// any other error should raise an exception
			return_code = LLSEC_MBEDTLS_ERR;
			break;
		}
	}

	mbedtls_ctr_drbg_free(&ctr_drbg);
	mbedtls_entropy_free(&entropy);
	// cppcheck-suppress misra-c2012-11.8 // Cast for matching free function signature
	mbedtls_free((void *)pers);

	LLSEC_SIG_DEBUG_TRACE("%s: return_code = %d\n", __func__, return_code);
	return return_code;
}

static int LLSEC_SIG_mbedtls_ec_sign(LLSEC_SIG_algorithm *algorithm, uint8_t *signature, int32_t *signature_length,
                                     LLSEC_priv_key *priv_key, const uint8_t *digest, int32_t digest_length,
                                     int *mbedtls_rc) {
	LLSEC_UNUSED_PARAM(algorithm);

	LLSEC_SIG_DEBUG_TRACE("%s \n", __func__);

	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;

	int return_code = LLSEC_SUCCESS;
	*mbedtls_rc = LLSEC_MBEDTLS_SUCCESS;

	mbedtls_entropy_init(&entropy);
	mbedtls_ctr_drbg_init(&ctr_drbg);

	const char *pers = llsec_gen_random_str_internal(8);
	if (NULL == pers) {
		LLSEC_SIG_DEBUG_TRACE("%s llsec_gen_random_str_internal: allocation error\n", __func__);
		return_code = LLSEC_ERROR;
	}
	if (LLSEC_SUCCESS == return_code) {
		*mbedtls_rc = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *)pers,
		                                    strlen(pers));
		LLSEC_SIG_DEBUG_TRACE("%s mbedtls_ctr_drbg_seed: %d\n", __func__, *mbedtls_rc);
		if (LLSEC_MBEDTLS_SUCCESS != *mbedtls_rc) {
			return_code = LLSEC_MBEDTLS_ERR;
		}
	}
	if (LLSEC_SUCCESS == return_code) {
		mbedtls_ecdsa_context *ctx = (mbedtls_ecdsa_context *)priv_key->key;
#if (MBEDTLS_VERSION_MAJOR == 2)
		*mbedtls_rc = mbedtls_ecdsa_write_signature(ctx, MBEDTLS_MD_SHA256, digest, (size_t)digest_length, signature,
		                                            (size_t *)signature_length, mbedtls_ctr_drbg_random, &ctr_drbg);
#elif (MBEDTLS_VERSION_MAJOR == 3)
		*mbedtls_rc = mbedtls_ecdsa_write_signature(ctx, MBEDTLS_MD_SHA256, digest, (size_t)digest_length, signature,
		                                            (size_t)*signature_length, (size_t *)signature_length,
		                                            mbedtls_ctr_drbg_random, &ctr_drbg);
#else
		#error "Unsupported mbedTLS major version"
#endif
		LLSEC_SIG_DEBUG_TRACE("%s mbedtls_ecdsa_write_signature: %d\n", __func__, *mbedtls_rc);
		if (LLSEC_MBEDTLS_SUCCESS != *mbedtls_rc) {
			return_code = LLSEC_MBEDTLS_ERR;
		}
	}

	mbedtls_ctr_drbg_free(&ctr_drbg);
	mbedtls_entropy_free(&entropy);
	// cppcheck-suppress misra-c2012-11.8 // Cast for matching free function signature
	mbedtls_free((void *)pers);

	LLSEC_SIG_DEBUG_TRACE("%s: return_code = %d\n", __func__, return_code);
	return return_code;
}

static int LLSEC_SIG_mbedtls_verify(LLSEC_SIG_algorithm *algorithm, const uint8_t *signature, int32_t signature_length,
                                    LLSEC_pub_key *pub_key, const uint8_t *digest, int32_t digest_length,
                                    int *mbedtls_rc) {
	LLSEC_UNUSED_PARAM(algorithm);

	LLSEC_SIG_DEBUG_TRACE("%s \n", __func__);

	int return_code = LLSEC_ERROR;
	bool ok_so_far = true;
	*mbedtls_rc = LLSEC_MBEDTLS_SUCCESS;

	if ((size_t)signature_length != mbedtls_rsa_get_len((mbedtls_rsa_context *)pub_key->key)) {
		/* mbedtls assumes the provided signature buffer is the required size. We must verify the size
		 * to prevent mbedtls from illegal accesses. Return an invalid signature */
		return_code = JFALSE;
		ok_so_far = false;
	}

#if (MBEDTLS_VERSION_MAJOR == 2)
	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;
	char pers[LLSEC_PERSONALIZATION_LEN];

	mbedtls_entropy_init(&entropy);
	mbedtls_ctr_drbg_init(&ctr_drbg);
	llsec_mbedtls_gen_random_str(pers, sizeof(pers));

	if (ok_so_far) {
		*mbedtls_rc = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *)pers,
		                                    strlen(pers));
		LLSEC_SIG_DEBUG_TRACE("%s mbedtls_ctr_drbg_seed: %d\n", __func__, *mbedtls_rc);
		if (LLSEC_MBEDTLS_SUCCESS != *mbedtls_rc) {
			return_code = LLSEC_MBEDTLS_ERR;
			ok_so_far = false;
		}
	}
#endif

	if (ok_so_far) {
#if (MBEDTLS_VERSION_MAJOR == 2)
		*mbedtls_rc = mbedtls_rsa_pkcs1_verify((mbedtls_rsa_context *)pub_key->key, mbedtls_ctr_drbg_random, &ctr_drbg,
		                                       MBEDTLS_RSA_PUBLIC, MBEDTLS_MD_SHA256, digest_length, digest, signature);
#elif (MBEDTLS_VERSION_MAJOR == 3)
		*mbedtls_rc = mbedtls_rsa_pkcs1_verify((mbedtls_rsa_context *)pub_key->key, MBEDTLS_MD_SHA256, digest_length,
		                                       digest, signature);
#else
		#error "Unsupported mbedTLS major version"
#endif
		LLSEC_SIG_DEBUG_TRACE("%s mbedtls_rsa_pkcs1_verify: %d\n", __func__, *mbedtls_rc);
		switch (MBEDTLS_MODULE_ERROR(*mbedtls_rc)) {
		case 0:
			// this is a valid signature
			return_code = JTRUE;
			break;
		case MBEDTLS_ERR_RSA_VERIFY_FAILED:
			// this is a invalid signature
			return_code = JFALSE;
			break;
		case MBEDTLS_ERR_RSA_BAD_INPUT_DATA:
		case MBEDTLS_ERR_RSA_INVALID_PADDING:
			// everything that could come from poor user inputs is treated as unverified signature
			return_code = JFALSE;
			break;
		default:
			// any other error should raise an exception
			return_code = LLSEC_MBEDTLS_ERR;
			break;
		}
	}

#if (MBEDTLS_VERSION_MAJOR == 2)
	mbedtls_ctr_drbg_free(&ctr_drbg);
	mbedtls_entropy_free(&entropy);
#endif

	LLSEC_SIG_DEBUG_TRACE("%s: return_code = %d\n", __func__, return_code);
	return return_code;
}

static int LLSEC_SIG_mbedtls_sign(LLSEC_SIG_algorithm *algorithm, uint8_t *signature, int32_t *signature_length,
                                  LLSEC_priv_key *priv_key, const uint8_t *digest, int32_t digest_length,
                                  int *mbedtls_rc) {
	LLSEC_UNUSED_PARAM(algorithm);

	LLSEC_SIG_DEBUG_TRACE("%s \n", __func__);

	mbedtls_entropy_context entropy;
	mbedtls_ctr_drbg_context ctr_drbg;

	int return_code = LLSEC_SUCCESS;
	*mbedtls_rc = LLSEC_MBEDTLS_SUCCESS;

	mbedtls_entropy_init(&entropy);
	mbedtls_ctr_drbg_init(&ctr_drbg);

	const char *pers = llsec_gen_random_str_internal(8);
	if (NULL == pers) {
		LLSEC_SIG_DEBUG_TRACE("%s llsec_gen_random_str_internal: allocation error\n", __func__);
		return_code = LLSEC_ERROR;
	}
	if (LLSEC_SUCCESS == return_code) {
		*mbedtls_rc = mbedtls_ctr_drbg_seed(&ctr_drbg, mbedtls_entropy_func, &entropy, (const unsigned char *)pers,
		                                    strlen(pers));
		LLSEC_SIG_DEBUG_TRACE("%s mbedtls_ctr_drbg_seed: %d\n", __func__, *mbedtls_rc);
		if (LLSEC_MBEDTLS_SUCCESS != *mbedtls_rc) {
			return_code = LLSEC_MBEDTLS_ERR;
		}
	}
	if (LLSEC_SUCCESS == return_code) {
		size_t rsa_len = mbedtls_rsa_get_len((mbedtls_rsa_context *)priv_key->key);
		// verify that provided buffer for signature is large enough
		if (*signature_length < rsa_len) {
			LLSEC_SIG_DEBUG_TRACE("%s signature_len too short: %" PRId32 "(buffer) < %lu(rsa_key)\n",
			                      __func__, *signature_length, (unsigned long)rsa_len);
			return_code = LLSEC_ERROR;
		}
	}
	if (LLSEC_SUCCESS == return_code) {
#if (MBEDTLS_VERSION_MAJOR == 2)
		*mbedtls_rc = mbedtls_rsa_pkcs1_sign((mbedtls_rsa_context *)priv_key->key, mbedtls_ctr_drbg_random, &ctr_drbg,
		                                     MBEDTLS_RSA_PRIVATE, MBEDTLS_MD_SHA256, digest_length, digest, signature);
#elif (MBEDTLS_VERSION_MAJOR == 3)
		*mbedtls_rc = mbedtls_rsa_pkcs1_sign((mbedtls_rsa_context *)priv_key->key, mbedtls_ctr_drbg_random, &ctr_drbg,
		                                     MBEDTLS_MD_SHA256, digest_length, digest, signature);
#else
		#error "Unsupported mbedTLS major version"
#endif
		LLSEC_SIG_DEBUG_TRACE("%s mbedtls_rsa_pkcs1_sign: %d\n", __func__, *mbedtls_rc);
		if (LLSEC_MBEDTLS_SUCCESS != *mbedtls_rc) {
			return_code = LLSEC_MBEDTLS_ERR;
		}
	}
	if (LLSEC_SUCCESS == return_code) {
		*signature_length = mbedtls_rsa_get_len((mbedtls_rsa_context *)priv_key->key);
	}

	mbedtls_ctr_drbg_free(&ctr_drbg);
	mbedtls_entropy_free(&entropy);
	// cppcheck-suppress misra-c2012-11.8 // Cast for matching free function signature
	mbedtls_free((void *)pers);

	LLSEC_SIG_DEBUG_TRACE("%s: return_code = %d\n", __func__, return_code);
	return return_code;
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_SIG_IMPL_get_algorithm_description(uint8_t *algorithm_name, uint8_t *digest_algorithm_name,
                                                 int32_t digest_algorithm_name_length) {
	int32_t return_code = LLSEC_ERROR;
	LLSEC_SIG_DEBUG_TRACE("%s \n", __func__);
	LLSEC_PROFILE_START();

	int32_t nb_algorithms = sizeof(available_sig_algorithms) / sizeof(LLSEC_SIG_algorithm);
	LLSEC_SIG_algorithm *algorithm = &available_sig_algorithms[0];

	while (--nb_algorithms >= 0) {
		if (strcmp((char *)algorithm_name, algorithm->name) == 0) {
			(void)strncpy((char *)digest_algorithm_name, algorithm->digest_name, digest_algorithm_name_length);
			/* strncpy result may not be null-terminated. */
			digest_algorithm_name[digest_algorithm_name_length - 1] = '\0';
			break;
		}
		algorithm++;
	}

	if (nb_algorithms >= 0) {
		return_code = (int32_t)algorithm;
	}
	LLSEC_PROFILE_END();
	return return_code;
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
void LLSEC_SIG_IMPL_get_algorithm_oid(uint8_t *algorithm_name, uint8_t *oid, int32_t oid_length) {
	LLSEC_SIG_DEBUG_TRACE("%s \n", __func__);
	LLSEC_PROFILE_START();

	int32_t nb_algorithms = sizeof(available_sig_algorithms) / sizeof(LLSEC_SIG_algorithm);
	LLSEC_SIG_algorithm *algorithm = &available_sig_algorithms[0];

	while (--nb_algorithms >= 0) {
		if (strcmp((char *)algorithm_name, algorithm->name) == 0) {
			int32_t length = strlen(algorithm->oid);
			if ((length + 1) > oid_length) {
				int32_t sni_rc = SNI_throwNativeException(-1, "Native oid length is bigger that the output byte array");
				LLSEC_ASSERT(sni_rc == SNI_OK);
			}
			(void)strncpy((char *)oid, algorithm->oid, length);
			/* strncpy result may not be null-terminated. */
			oid[length + 1] = '\0';
			break;
		}
		algorithm++;
	}
	if (0 > nb_algorithms) {
		/* Algorithm not found. */
		int32_t sni_rc = SNI_throwNativeException(LLSEC_ERROR, "Algorithm not found");
		LLSEC_ASSERT(sni_rc == SNI_OK);
	}
	LLSEC_PROFILE_END();
}

static void LLSEC_SIG_IMPL_verify_job(MICROEJ_ASYNC_WORKER_job_t *job) {
	struct LLSEC_SIG_verify_params *params = job->params;
	LLSEC_SIG_algorithm *algorithm = params->algorithm;
	params->rc = algorithm->verify(algorithm, params->signature, params->signature_length,
	                               params->public_key, params->digest, params->digest_length,
	                               &params->mbedtls_rc);
}

static uint8_t LLSEC_SIG_IMPL_verify_job_done(int32_t algorithm_id, uint8_t *signature, int32_t signature_length,
                                              int32_t public_key_id, uint8_t *digest, int32_t digest_length) {
	LLSEC_UNUSED_PARAM(algorithm_id);
	LLSEC_UNUSED_PARAM(signature);
	LLSEC_UNUSED_PARAM(signature_length);
	LLSEC_UNUSED_PARAM(public_key_id);
	LLSEC_UNUSED_PARAM(digest);
	LLSEC_UNUSED_PARAM(digest_length);
	// returned value
	uint8_t return_code;
	// thrown by SNI
	int exception_code = SNI_ERROR;
	const char *reason = NULL;
	// controls the execution flow
	int result = LLSEC_SUCCESS;
	struct LLSEC_SIG_verify_params *params = NULL;
	MICROEJ_ASYNC_WORKER_job_t *job = MICROEJ_ASYNC_WORKER_get_job_done();
	if (job == NULL) {
		result = LLSEC_ERROR;
		reason = "can't retrieve job";
	}

	if (result == LLSEC_SUCCESS) {
		params = (struct LLSEC_SIG_verify_params *)job->params;
		// retrieve returned code from job
		result = params->rc;

		// prepare exceptions
		switch (result) {
		case JTRUE:
		case JFALSE:
			// normal execution path, no exception will be thrown
			break;
		case LLSEC_MBEDTLS_ERR:
			reason = "mbedtls err";
			exception_code = params->mbedtls_rc;
			break;
		default:
			reason = "LLSEC_SIG_IMPL_verify failed";
			exception_code = result;
			break;
		}
	}

	if (result < LLSEC_SUCCESS) {
		// some fault happened, throw an exception
		int32_t sni_rc = SNI_throwNativeException(exception_code, reason);
		LLSEC_ASSERT(sni_rc == SNI_OK);
		return_code = SNI_IGNORED_RETURNED_VALUE;
	} else {
		/* execution went smoothly, the result from the job can be returned. At this point,
		 * result should contain JFALSE(0) or JTRUE(1). */
		LLSEC_ASSERT((result == JFALSE) || (result == JTRUE));
		return_code = (uint8_t)result;
	}

	if (params != NULL) {
		mbedtls_free(params->digest);
		mbedtls_free(params->signature);
	}

	MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_free_job(&llsec_worker, job);
	LLSEC_ASSERT(status == MICROEJ_ASYNC_WORKER_OK);

	return return_code;
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
uint8_t LLSEC_SIG_IMPL_verify(int32_t algorithm_id, uint8_t *signature, int32_t signature_length, int32_t public_key_id,
                              uint8_t *digest, int32_t digest_length) {
	int result = LLSEC_SUCCESS;
	struct LLSEC_SIG_verify_params *params = NULL;
	LLSEC_SIG_DEBUG_TRACE("%s \n", __func__);

	MICROEJ_ASYNC_WORKER_job_t *job = MICROEJ_ASYNC_WORKER_allocate_job(&llsec_worker,
	                                                                    (SNI_callback)LLSEC_SIG_IMPL_sign);
	if (job == NULL) {
		// MICROEJ_ASYNC_WORKER_allocate_job() has thrown a NativeException
		result = LLSEC_ERROR;
	}

	if (result == LLSEC_SUCCESS) {
		params = job->params;
		params->algorithm = (LLSEC_SIG_algorithm *)algorithm_id;
		params->signature = NULL;
		params->signature_length = signature_length;
		params->public_key = (LLSEC_pub_key *)public_key_id;
		params->digest = NULL;
		params->digest_length = digest_length;

		params->signature = mbedtls_calloc(signature_length, 1);
		if (params->signature == NULL) {
			int32_t sni_rc = SNI_throwNativeException(LLSEC_ERROR, "failed to alloc signature buffer");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			result = LLSEC_ERROR;
		}
	}

	if (result == LLSEC_SUCCESS) {
		(void)memcpy(params->signature, signature, signature_length);
		params->digest = mbedtls_calloc(digest_length, 1);
		if (params->digest == NULL) {
			int32_t sni_rc = SNI_throwNativeException(LLSEC_ERROR, "failed to alloc digest buffer");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			result = LLSEC_ERROR;
		}
	}

	if (result == LLSEC_SUCCESS) {
		(void)memcpy(params->digest, digest, digest_length);
		MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&llsec_worker, job,
		                                                                       LLSEC_SIG_IMPL_verify_job,
		                                                                       (SNI_callback)
		                                                                       LLSEC_SIG_IMPL_verify_job_done);

		if (status != MICROEJ_ASYNC_WORKER_OK) {
			// MICROEJ_ASYNC_WORKER_async_exec() has thrown a NativeException
			result = LLSEC_ERROR;
		}
	}

	if (result != LLSEC_SUCCESS) {
		// on error, free the allocated resources in reverse order
		if (params != NULL) {
			if (params->digest != NULL) {
				mbedtls_free(params->digest);
			}
			if (params->signature != NULL) {
				mbedtls_free(params->signature);
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

static void LLSEC_SIG_IMPL_sign_job(MICROEJ_ASYNC_WORKER_job_t *job) {
	struct LLSEC_SIG_sign_params *params = job->params;
	struct LLSEC_SIG_algorithm *algorithm = params->algorithm;

	params->rc = algorithm->sign(algorithm, params->signature, &params->signature_length, params->private_key,
	                             params->digest, params->digest_length, &params->mbedtls_rc);
}

static int32_t LLSEC_SIG_IMPL_sign_job_done(int32_t algorithm_id, uint8_t *signature, int32_t signature_length,
                                            int32_t private_key_id, uint8_t *digest, int32_t digest_length) {
	LLSEC_UNUSED_PARAM(algorithm_id);
	LLSEC_UNUSED_PARAM(signature_length);
	LLSEC_UNUSED_PARAM(private_key_id);
	LLSEC_UNUSED_PARAM(digest);
	LLSEC_UNUSED_PARAM(digest_length);
	// returned value
	int32_t return_code;
	// thrown by SNI
	int exception_code = SNI_ERROR;
	const char *reason = NULL;
	// controls the execution flow
	int result = LLSEC_SUCCESS;
	struct LLSEC_SIG_sign_params *params = NULL;
	MICROEJ_ASYNC_WORKER_job_t *job = MICROEJ_ASYNC_WORKER_get_job_done();
	if (job == NULL) {
		result = LLSEC_ERROR;
		reason = "can't retrieve job";
	}

	if (result == LLSEC_SUCCESS) {
		params = (struct LLSEC_SIG_sign_params *)job->params;
		// retrieve returned code from job
		result = params->rc;

		switch (result) {
		case LLSEC_SUCCESS:
			// normal execution path, no exception will be thrown
			(void)memcpy(signature, params->signature, params->signature_length);
			break;
		case LLSEC_MBEDTLS_ERR:
			reason = "mbedtls err";
			exception_code = params->mbedtls_rc;
			break;
		default:
			reason = "LLSEC_SIG_IMPL_sign() failed";
			exception_code = result;
			break;
		}
	}

	if (result < LLSEC_SUCCESS) {
		// some fault happened, throw an exception
		int32_t sni_rc = SNI_throwNativeException(exception_code, reason);
		LLSEC_ASSERT(sni_rc == SNI_OK);
		return_code = SNI_IGNORED_RETURNED_VALUE;
	} else {
		return_code = params->signature_length;
	}

	if (params != NULL) {
		mbedtls_free(params->digest);
		mbedtls_free(params->signature);
	}

	MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_free_job(&llsec_worker, job);
	LLSEC_ASSERT(status == MICROEJ_ASYNC_WORKER_OK);

	return return_code;
}

// cppcheck-suppress misra-c2012-8.7; external linkage is required as this function is part of the API
int32_t LLSEC_SIG_IMPL_sign(int32_t algorithm_id, uint8_t *signature, int32_t signature_length, int32_t private_key_id,
                            uint8_t *digest, int32_t digest_length) {
	// signature is unused as the execution is delegated to the async worker
	LLSEC_UNUSED_PARAM(signature);
	int result = LLSEC_SUCCESS;
	struct LLSEC_SIG_sign_params *params = NULL;
	LLSEC_SIG_DEBUG_TRACE("%s \n", __func__);

	MICROEJ_ASYNC_WORKER_job_t *job = MICROEJ_ASYNC_WORKER_allocate_job(&llsec_worker,
	                                                                    (SNI_callback)LLSEC_SIG_IMPL_sign);
	if (job == NULL) {
		// MICROEJ_ASYNC_WORKER_allocate_job() has thrown a NativeException
		result = LLSEC_ERROR;
	}

	if (result == LLSEC_SUCCESS) {
		params = job->params;

		params->algorithm = (LLSEC_SIG_algorithm *)algorithm_id;
		params->signature = NULL;
		params->signature_length = signature_length;
		params->private_key = (LLSEC_priv_key *)private_key_id;
		params->digest = NULL;
		params->digest_length = digest_length;

		params->signature = mbedtls_calloc(signature_length, 1);
		if (params->signature == NULL) {
			int32_t sni_rc = SNI_throwNativeException(LLSEC_ERROR, "failed to alloc signature buffer");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			result = LLSEC_ERROR;
		}
	}

	if (result == LLSEC_SUCCESS) {
		params->digest = mbedtls_calloc(digest_length, 1);
		if (params->digest == NULL) {
			int32_t sni_rc = SNI_throwNativeException(LLSEC_ERROR, "failed to alloc digest buffer");
			LLSEC_ASSERT(sni_rc == SNI_OK);
			result = LLSEC_ERROR;
		}
	}

	if (result == LLSEC_SUCCESS) {
		(void)memcpy(params->digest, digest, digest_length);

		MICROEJ_ASYNC_WORKER_status_t status = MICROEJ_ASYNC_WORKER_async_exec(&llsec_worker, job,
		                                                                       LLSEC_SIG_IMPL_sign_job,
		                                                                       (SNI_callback)
		                                                                       LLSEC_SIG_IMPL_sign_job_done);

		if (status != MICROEJ_ASYNC_WORKER_OK) {
			// MICROEJ_ASYNC_WORKER_async_exec() has thrown a NativeException
			result = LLSEC_ERROR;
		}
	}

	if (result == LLSEC_ERROR) {
		// on error, free the allocated resources in reverse order
		if (params != NULL) {
			if (params->digest != NULL) {
				mbedtls_free(params->digest);
			}

			if (params->signature != NULL) {
				mbedtls_free(params->signature);
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

/*
 * C
 *
 * Copyright 2021-2025 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

/**
 * @file
 * @brief Security natives configuration.
 * @author MicroEJ Developer Team
 * @version 2.0.1
 */

#ifndef LLSEC_CONFIGURATION_H
#define LLSEC_CONFIGURATION_H

#ifdef __cplusplus
extern "C" {
#endif

#define LLSEC_UNUSED_PARAM(x)                     ((void)(x))

/* Enable local asserts by redirecting to the BSP's assert macro */
#define LLSEC_ASSERT(x)                           ((void)(x))

/*
 * Used for private and public key generation
 */
#define LLSEC_PRIVATE_KEY_MAX_PKCS8_LEN           (3072)
#define LLSEC_PUBLIC_KEY_MAX_DER_LEN              (3072)

/* The longest ECP curve name defined in mbedtls is 15 char long and the buffer
 * must be able to store the null terminating char */
#define LLSEC_ECP_CURVE_NAME_BUFFER_LEN           (15 + 1)

#define LLSEC_SECRET_KEY_MAX_PWD_LEN              256
#define LLSEC_SECRET_KEY_MAX_SALT_LEN             64

#define LLSEC_PERSONALIZATION_LEN                 (8)

/*
 * Debug traces activation
 */
#define LLSEC_DEBUG_TRACE_ENABLE                  (1)
#define LLSEC_DEBUG_TRACE_DISABLE                 (0)

#ifndef LLSEC_DEBUG_TRACE
// cppcheck-suppress misra-c2012-21.6 // Include only in debug
#include <stdio.h>
#include "fsl_debug_console.h"
#define LLSEC_DEBUG_TRACE(...)                    (void)PRINTF(__VA_ARGS__)
#endif
#ifndef LLSEC_ALL_DEBUG
#define LLSEC_ALL_DEBUG                           LLSEC_DEBUG_TRACE_DISABLE
#endif
#ifndef LLSEC_CIPHER_DEBUG
#define LLSEC_CIPHER_DEBUG                        LLSEC_DEBUG_TRACE_DISABLE
#endif
#ifndef LLSEC_DIGEST_DEBUG
#define LLSEC_DIGEST_DEBUG                        LLSEC_DEBUG_TRACE_DISABLE
#endif
#ifndef LLSEC_KEY_FACTORY_DEBUG
#define LLSEC_KEY_FACTORY_DEBUG                   LLSEC_DEBUG_TRACE_DISABLE
#endif
#ifndef LLSEC_KEY_PAIR_GENERATOR_DEBUG
#define LLSEC_KEY_PAIR_GENERATOR_DEBUG            LLSEC_DEBUG_TRACE_DISABLE
#endif
#ifndef LLSEC_MAC_DEBUG
#define LLSEC_MAC_DEBUG                           LLSEC_DEBUG_TRACE_DISABLE
#endif
#ifndef LLSEC_PRIVATE_KEY_DEBUG
#define LLSEC_PRIVATE_KEY_DEBUG                   LLSEC_DEBUG_TRACE_DISABLE
#endif
#ifndef LLSEC_PUBLIC_KEY_DEBUG
#define LLSEC_PUBLIC_KEY_DEBUG                    LLSEC_DEBUG_TRACE_DISABLE
#endif
#ifndef LLSEC_RANDOM_DEBUG
#define LLSEC_RANDOM_DEBUG                        LLSEC_DEBUG_TRACE_DISABLE
#endif
#ifndef LLSEC_RSA_CIPHER_DEBUG
#define LLSEC_RSA_CIPHER_DEBUG                    LLSEC_DEBUG_TRACE_DISABLE
#endif
#ifndef LLSEC_SECRET_KEY_FACTORY_DEBUG
#define LLSEC_SECRET_KEY_FACTORY_DEBUG            LLSEC_DEBUG_TRACE_DISABLE
#endif
#ifndef LLSEC_SECRET_KEY_DEBUG
#define LLSEC_SECRET_KEY_DEBUG                    LLSEC_DEBUG_TRACE_DISABLE
#endif
#ifndef LLSEC_SIG_DEBUG
#define LLSEC_SIG_DEBUG                           LLSEC_DEBUG_TRACE_DISABLE
#endif
#ifndef LLSEC_X509_DEBUG
#define LLSEC_X509_DEBUG                          LLSEC_DEBUG_TRACE_DISABLE
#endif
#ifndef LLSEC_PROFILE
#define LLSEC_PROFILE                             0
#endif

#if (LLSEC_CIPHER_DEBUG == LLSEC_DEBUG_TRACE_ENABLE || LLSEC_ALL_DEBUG == LLSEC_DEBUG_TRACE_ENABLE)
#define LLSEC_CIPHER_DEBUG_TRACE(...)             LLSEC_DEBUG_TRACE(__VA_ARGS__)
#else
#define LLSEC_CIPHER_DEBUG_TRACE(...)             ((void)(0))
#endif

#if (LLSEC_DIGEST_DEBUG == LLSEC_DEBUG_TRACE_ENABLE || LLSEC_ALL_DEBUG == LLSEC_DEBUG_TRACE_ENABLE)
#define LLSEC_DIGEST_DEBUG_TRACE(...)             LLSEC_DEBUG_TRACE(__VA_ARGS__)
#else
#define LLSEC_DIGEST_DEBUG_TRACE(...)             ((void)(0))
#endif

#if (LLSEC_KEY_FACTORY_DEBUG == LLSEC_DEBUG_TRACE_ENABLE || LLSEC_ALL_DEBUG == LLSEC_DEBUG_TRACE_ENABLE)
#define LLSEC_KEY_FACTORY_DEBUG_TRACE(...)        LLSEC_DEBUG_TRACE(__VA_ARGS__)
#else
#define LLSEC_KEY_FACTORY_DEBUG_TRACE(...)        ((void)(0))
#endif

#if (LLSEC_KEY_PAIR_GENERATOR_DEBUG == LLSEC_DEBUG_TRACE_ENABLE || LLSEC_ALL_DEBUG == LLSEC_DEBUG_TRACE_ENABLE)
#define LLSEC_KEY_PAIR_GENERATOR_DEBUG_TRACE(...) LLSEC_DEBUG_TRACE(__VA_ARGS__)
#else
#define LLSEC_KEY_PAIR_GENERATOR_DEBUG_TRACE(...) ((void)(0))
#endif

#if (LLSEC_MAC_DEBUG == LLSEC_DEBUG_TRACE_ENABLE || LLSEC_ALL_DEBUG == LLSEC_DEBUG_TRACE_ENABLE)
#define LLSEC_MAC_DEBUG_TRACE(...)                LLSEC_DEBUG_TRACE(__VA_ARGS__)
#else
#define LLSEC_MAC_DEBUG_TRACE(...)                ((void)(0))
#endif

#if (LLSEC_PRIVATE_KEY_DEBUG == LLSEC_DEBUG_TRACE_ENABLE || LLSEC_ALL_DEBUG == LLSEC_DEBUG_TRACE_ENABLE)
#define LLSEC_PRIVATE_KEY_DEBUG_TRACE(...)        LLSEC_DEBUG_TRACE(__VA_ARGS__)
#else
#define LLSEC_PRIVATE_KEY_DEBUG_TRACE(...)        ((void)(0))
#endif

#if (LLSEC_PUBLIC_KEY_DEBUG == LLSEC_DEBUG_TRACE_ENABLE || LLSEC_ALL_DEBUG == LLSEC_DEBUG_TRACE_ENABLE)
#define LLSEC_PUBLIC_KEY_DEBUG_TRACE(...)         LLSEC_DEBUG_TRACE(__VA_ARGS__)
#else
#define LLSEC_PUBLIC_KEY_DEBUG_TRACE(...)         ((void)(0))
#endif

#if (LLSEC_RANDOM_DEBUG == LLSEC_DEBUG_TRACE_ENABLE || LLSEC_ALL_DEBUG == LLSEC_DEBUG_TRACE_ENABLE)
#define LLSEC_RANDOM_DEBUG_TRACE(...)             LLSEC_DEBUG_TRACE(__VA_ARGS__)
#else
#define LLSEC_RANDOM_DEBUG_TRACE(...)             ((void)(0))
#endif

#if (LLSEC_RSA_CIPHER_DEBUG == LLSEC_DEBUG_TRACE_ENABLE || LLSEC_ALL_DEBUG == LLSEC_DEBUG_TRACE_ENABLE)
#define LLSEC_RSA_CIPHER_DEBUG_TRACE(...)         LLSEC_DEBUG_TRACE(__VA_ARGS__)
#else
#define LLSEC_RSA_CIPHER_DEBUG_TRACE(...)         ((void)(0))
#endif

#if (LLSEC_SECRET_KEY_FACTORY_DEBUG == LLSEC_DEBUG_TRACE_ENABLE || LLSEC_ALL_DEBUG == LLSEC_DEBUG_TRACE_ENABLE)
#define LLSEC_SECRET_KEY_FACTORY_DEBUG_TRACE(...) LLSEC_DEBUG_TRACE(__VA_ARGS__)
#else
#define LLSEC_SECRET_KEY_FACTORY_DEBUG_TRACE(...) ((void)(0))
#endif

#if (LLSEC_SECRET_KEY_DEBUG == LLSEC_DEBUG_TRACE_ENABLE || LLSEC_ALL_DEBUG == LLSEC_DEBUG_TRACE_ENABLE)
#define LLSEC_SECRET_KEY_DEBUG_TRACE(...)         LLSEC_DEBUG_TRACE(__VA_ARGS__)
#else
#define LLSEC_SECRET_KEY_DEBUG_TRACE(...)         ((void)(0))
#endif

#if (LLSEC_SIG_DEBUG == LLSEC_DEBUG_TRACE_ENABLE || LLSEC_ALL_DEBUG == LLSEC_DEBUG_TRACE_ENABLE)
#define LLSEC_SIG_DEBUG_TRACE(...)                LLSEC_DEBUG_TRACE(__VA_ARGS__)
#else
#define LLSEC_SIG_DEBUG_TRACE(...)                ((void)(0))
#endif

#if (LLSEC_X509_DEBUG == LLSEC_DEBUG_TRACE_ENABLE || LLSEC_ALL_DEBUG == LLSEC_DEBUG_TRACE_ENABLE)
#define LLSEC_X509_DEBUG_TRACE(...)               LLSEC_DEBUG_TRACE(__VA_ARGS__)
#else
#define LLSEC_X509_DEBUG_TRACE(...)               ((void)(0))
#endif

#if LLSEC_PROFILE

#include <inttypes.h>
#include <microej_time.h>

#define LLSEC_PROFILE_START()       int64_t _profile_start_time = microej_time_get_current_time(1)
#define LLSEC_PROFILE_END()                                                                     \
		do {                                                                                    \
			int64_t _profile_duration = microej_time_get_current_time(1) - _profile_start_time; \
			LLSEC_DEBUG_TRACE("profile %s: %" PRId64 "ms\n", __func__, _profile_duration);      \
		} while (0)

#else

#define LLSEC_PROFILE_START()       ((void)0)
#define LLSEC_PROFILE_END()         ((void)0)

#endif /*LLSEC_PROFILE*/

/**
 * Stack size in bytes allocated to the worker thread
 *
 * @note Stack analysis running the security test suite on ESP32-S3 FreeRTOS
 * shows a lowest remaining size of 672 bytes (16% headroom). May need to be
 * adjusted on a per-platform basis.
 *
 * @warning Some port of MbedTLS may be configured to run on a hardware
 * accelerator and may require more stack than the default software
 * implementation.
 */
#define LLSEC_WORKER_STACK_SIZE                     5*1024
/**
 * Priority of the thread running the worker.
 *
 * The worker thread should be set to run at a priority lower than the Java
 * thread, so as to free execution time for the foreground application. Adjust
 * accordingly.
 *
 * @note This value is RTOS dependent.
 */
#define LLSEC_WORKER_PRIORITY                       2
/**
 * Number of jobs that can be queued in the worker thread.
 */
#define LLSEC_WORKER_JOB_COUNT                      2
/**
 * Number of Java thread that can wait for a free job.
 */
#define LLSEC_WAITING_LIST_SIZE                     4

#ifdef __cplusplus
}
#endif

#endif /* LLSEC_CONFIGURATION_H */

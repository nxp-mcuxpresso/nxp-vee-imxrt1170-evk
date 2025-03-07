/*
 * C
 *
 * Copyright 2024 MicroEJ Corp. All rights reserved.
 * Use of this source code is governed by a BSD-style license that can be found with this software.
 */

#include "fsl_silicon_id.h"
#include "fsl_silicon_id_soc.h"

#include "mbedtls/platform.h"
#include "mbedtls/md.h"

#define SHA1_DIGEST_LENGTH   20

static int siliconid_digest_sha1(uint8_t* silicon_id, uint32_t id_len, uint8_t* hash)
{
    int return_code = 0;

    mbedtls_md_context_t* md_ctx = mbedtls_calloc(1, sizeof(mbedtls_md_context_t));
    if (md_ctx == NULL) {
        return_code = -1;
    }

    if (return_code == 0){
        mbedtls_md_init(md_ctx);
        return_code = mbedtls_md_setup(md_ctx, mbedtls_md_info_from_type(MBEDTLS_MD_SHA1), 0);
    }

    if (return_code == 0){
        return_code = mbedtls_md_starts(md_ctx);
    }

    if (return_code == 0){
        return_code = mbedtls_md_update(md_ctx, silicon_id, id_len);
    }
    
    if (return_code == 0){
        return_code = mbedtls_md_finish(md_ctx, hash);
    }

    mbedtls_md_free(md_ctx);

    return return_code;
}

status_t workaround_SILICONID_ConvertToMacAddr(uint8_t (*macAddr)[6])
{
    status_t result;

    uint8_t siliconId[SILICONID_MAX_LENGTH];
    uint32_t idLen = sizeof(siliconId);

    uint8_t sha1_hash[SHA1_DIGEST_LENGTH];

    result = SILICONID_GetID(&siliconId[0], &idLen);
    if (result != kStatus_Success)
    {
        return result;
    }

    /* Valid ID should > 3. */
    assert(idLen >= 3U);

    /* Set NXP OUI. */
    macAddr[0][0] = 0x54;
    macAddr[0][1] = 0x27;
    macAddr[0][2] = 0x8d;

    if (siliconid_digest_sha1(&siliconId[0], idLen, &sha1_hash[0]) == 0) {
        macAddr[0][3] = sha1_hash[0];
        macAddr[0][4] = sha1_hash[1];
        macAddr[0][5] = sha1_hash[2];
    } else {
        macAddr[0][3] = siliconId[idLen - 3];
        macAddr[0][4] = siliconId[idLen - 2];
        macAddr[0][5] = siliconId[idLen - 1];
    }

    return result;
}
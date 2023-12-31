#ifndef HEADER_AISINO_RAND_MBEDTLS_RAND_H
#define HEADER_AISINO_RAND_MBEDTLS_RAND_H

#if !defined(TARGET_PLATFORM_ANDROID)
// Config file is generated by cmake
#include "config.h"
#endif

#if defined(AISINO_RAND_VER_MBEDTLS)

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

#include <AisinoSSL/mbedtls/hmac_drbg.h>
#include <AisinoSSL/mbedtls/md.h>

#ifdef __cplusplus
extern "C" {
#endif

// ------------------------------
// Cmake config -> Local config
//

// ------------------
// 1. Hash Algorithm
//

// SM3 as Hash Algorithm
#ifdef AISINO_RAND_HASH_ALG_SM3
#define RANDOM_HASH_ALGORITHM MD_SM3
#endif

// Control HMAC-DRBG Hash Algorithm
// Attention! Use #define to set it
#ifndef RANDOM_HASH_ALGORITHM
#error "Must set Aisino Rand Hash Algorithm!"
#endif

// ------------------
// 2. iOS Sensors
//

// #define AISINO_RAND_ENABLE_SEED_IOS_SENSOR

// ------------ END ------------
//

static unsigned int aisinoSSLRandomContextCount = 0;

// HMAC-DRBG Random Class
struct aisino_rand_context {
    short isInitial;
    short isSeeded;

    unsigned char hashLen;
    unsigned char *hash;

    const mbedtls_md_info_t *md_info;     /*!<  Hash Type Info    */
    mbedtls_md_context_t md_ctx;         /*!<  MD Context        */
    mbedtls_hmac_drbg_context drbg_ctx;  /*!<  HMAC DRBG Context */
};

#ifdef __cplusplus
}
#endif

#endif /* END AISINO_RAND_VER_MBEDTLS */

#endif /* END HEADER_AISINO_RAND_MBEDTLS_RAND_H */

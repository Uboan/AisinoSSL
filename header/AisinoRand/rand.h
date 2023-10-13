#ifndef HEADER_aisino_rand_H
#define HEADER_aisino_rand_H

/*
Random Module

Sample:
void *ctx;
int arr[30];
memset(arr, 0, sizeof(arr));
aisino_rand_init(&ctx);
aisino_rand_seed(ctx, NULL, 0);
aisino_rand_rand_int_array(ctx, arr, 30);
aisino_rand_free(ctx);

*/

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef _WIN32
#define EXPORT __declspec(dllexport)
#else
#define EXPORT __attribute ((visibility("default")))
#endif

typedef struct aisino_rand_context aisino_rand_context;

// Init Random Context
EXPORT
int aisino_rand_init(aisino_rand_context **ctx);

// Set or reset a seed
EXPORT
int aisino_rand_seed(aisino_rand_context* ctx, unsigned char *seed_buf, size_t buf_size);

EXPORT
int aisino_rand_seed_with_option(aisino_rand_context *ctx, unsigned char *seed_buf, size_t buf_size, int options);

// Rand a list of Number
EXPORT
int aisino_rand_rand(aisino_rand_context *ctx, void *output, size_t size);

EXPORT
int aisino_rand_rand_with_add(aisino_rand_context *ctx, void *output, size_t size,
                              const unsigned char *additional,
                              size_t add_len);

// Release random context
EXPORT
void aisino_rand_free(aisino_rand_context *ctx);

// Shuffle unsigned char array
EXPORT
int aisino_rand_shuffle_u8(unsigned char *list, int len);

// Rand a list of int32 (if ctx==NULL, then init a global ctx)
EXPORT
int aisino_rand_list(aisino_rand_context* ctx, int *list, int len);

// ERROR define
#define AISINO_RAND_ERROR_HASH_ALGO_NOT_FOUND -0xF101
#define AISINO_RAND_ERROR_NOT_INITIAL -0xF102
#define AISINO_RAND_ERROR_NOT_SEEDED -0xF103
#define AISINO_RAND_ERROR_OUT_SIZE_TO_LARGE -0xF104
#define AISINO_RAND_ERROR_INVLIAD_SIZE -0xF105

// OPTION define
#define AISINO_RAND_DISABLE_TIME 0x1
#define AISINO_RAND_DISABLE_URANDOM 0x2

#ifdef TARGET_PLATFORM_ANDROID
#define AISINO_RAND_DISABLE_ANDROID_INFO 0x10
#endif

#ifdef AISINO_RAND_ENABLE_SEED_IOS_SENSOR
#define AISINO_RAND_DISABLE_IOS_SENSOR 0x40
#endif

// OTHER define
#define AISINO_RAND_MAX_BYTES_COUNT 1024
#define AISINO_RAND_MAX_INT_COUNT (1024/4)

#ifdef __cplusplus
}
#endif

#endif

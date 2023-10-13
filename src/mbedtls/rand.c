/*
 * @Author: Weijie Li
 * @Date: 2017-11-02 10:32:40
 * @Last Modified by: Peiran Luo
 * @Last Modified time: 2018-11-08 15:00:00
 */

#include "rand.h"
#include "entropy.h"
#include <AisinoRand/rand.h>

#if defined(AISINO_RAND_VER_MBEDTLS)

//
// Plaform Definations:
// TARGET_PLATFORM_ANDROID / TARGET_PLATFORM_IOS
//

#if defined(TARGET_PLATFORM_ANDROID) && defined(ENABLE_DEBUG)
#include <android/log.h>
#define LOG(...) __android_log_print(ANDROID_LOG_DEBUG, "AISINO_RAND", __VA_ARGS__)
#else
#define LOG(...) 
#endif

#if defined(TARGET_PLATFORM_ANDROID)
#include <sys/system_properties.h>
#endif

// -- SUPPORT IOS START --
#if defined(__APPLE__) && defined(TARGET_PLATFORM_IOS) && defined(AISINO_RAND_ENABLE_SEED_IOS_SENSOR)

#include <TargetConditionals.h>
#include <objc/runtime.h>
#include <objc/objc.h>
#include <objc/message.h>
#include <unistd.h>

typedef struct {
    double x;
    double y;
    double z;
} PP;

BOOL (*msgSend_bool)(id, SEL) = (BOOL (*)(id, SEL)) objc_msgSend;

#if defined (__arm64__)
PP (*msgSend_pp)(id, SEL) = (PP (*)(id, SEL)) objc_msgSend;
#else
PP (*msgSend_pp)(id, SEL) = (PP (*)(id, SEL)) objc_msgSend_stret;
#endif

#endif // END AISINO_RAND_ENABLE_SEED_IOS_SENSOR

int read_file(const char *filename, unsigned char *buf, int size) {
	FILE *fd;
	int ret = 0;
	fd = fopen(filename, "rb");
	if (fd >= 0) {
		ret = fread(buf, size, 1, fd);
		fclose(fd);
	}
	return ret;
}

int aisino_rand_init(aisino_rand_context **_ctx) {
    aisino_rand_context *ctx = (aisino_rand_context *)malloc(sizeof(aisino_rand_context));

	// 0. Set MD Info
	ctx->md_info = mbedtls_md_info_from_type(RANDOM_HASH_ALGORITHM);
	if (ctx->md_info == NULL) {
		return AISINO_RAND_ERROR_HASH_ALGO_NOT_FOUND;
	}

	// 1. Init and Setup MD
	mbedtls_md_init(&ctx->md_ctx);

	int ret = mbedtls_md_setup(&ctx->md_ctx, ctx->md_info, 0);
  	if (ret != 0) return ret;

	// 2. Init HMAC-DRBG
	mbedtls_hmac_drbg_init(&ctx->drbg_ctx);

	// 3. Init Hash Buffer
	ctx->hashLen = mbedtls_md_get_size(ctx->md_info);
	ctx->hash = (unsigned char *)malloc(ctx->hashLen);
	memset(ctx->hash, 0, ctx->hashLen);

	// 4. Set isInitial
	ctx->isInitial = 1;
	aisinoSSLRandomContextCount++;

	*_ctx = ctx;

	return 0;
}

int aisino_rand_seed(aisino_rand_context *ctx, unsigned char *seed_buf, size_t buf_size) {
	return aisino_rand_seed_with_option(ctx, seed_buf, buf_size, 0);
}

int aisino_rand_seed_with_option(aisino_rand_context* ctx, unsigned char *seed_buf, size_t buf_size, int options) {
	if (ctx->isInitial != 1) return AISINO_RAND_ERROR_NOT_INITIAL;

	int i, ret;
	unsigned int tmp;

	// 0. Init MD
	mbedtls_md_init(&ctx->md_ctx);

	ret = mbedtls_md_setup(&ctx->md_ctx, ctx->md_info, 0);
	if (ret) return ret;



	// 1. Set time() as seed
	// General
	if ((options & AISINO_RAND_DISABLE_TIME) == 0) {
		time_t t = time(NULL);
		mbedtls_md_update(&ctx->md_ctx, (const unsigned char *)&t, sizeof(t));
	}

	//1.5 Set local entropy as seed





	

	// 2. Set /dev/urandom as seed
	// FOR Linux/Unix
	#if defined(unix) || defined(__unix__) || defined(__unix) || defined(__APPLE__) || defined(TARGET_PLATFORM_ANDROID)
	if ((options & AISINO_RAND_DISABLE_URANDOM) == 0) {
		unsigned char urandomBuf[1024];
		
		ret = read_file("/dev/urandom", urandomBuf, sizeof(urandomBuf));
		if (ret) {
			mbedtls_md_update(&ctx->md_ctx, urandomBuf, sizeof(urandomBuf));
		}
		LOG("URANDOM: %d ", ret);
	}
	#endif

	// 3. Set Android Hardware Info
	#ifdef TARGET_PLATFORM_ANDROID
	if ((options & AISINO_RAND_DISABLE_ANDROID_INFO) == 0) {
		char buf[PROP_VALUE_MAX];
		size_t _len;

    __system_property_get("ro.build.version.release", buf);
		_len = (strlen(buf) / 4 + 1) * 4;
		mbedtls_md_update(&ctx->md_ctx, (const unsigned char *)buf, _len);

    __system_property_get("ro.hardware", buf);
		_len = (strlen(buf) / 4 + 1) * 4;
		mbedtls_md_update(&ctx->md_ctx, (const unsigned char *)buf, _len);

		__system_property_get("ro.serialno", buf);
		_len = (strlen(buf) / 4 + 1) * 4;
		mbedtls_md_update(&ctx->md_ctx, (const unsigned char *)buf, _len);
	}
	#endif // END TARGET_PLATFORM_ANDROID

	#if defined(__APPLE__) && defined(TARGET_PLATFORM_IOS) && defined(AISINO_RAND_ENABLE_SEED_IOS_SENSOR)

	Class CMMotionManager = objc_getClass("CMMotionManager");
    if (CMMotionManager) {
		
    	SEL sel = sel_registerName("init");
    	id motionManager = class_createInstance(CMMotionManager, 0);
    	motionManager = objc_msgSend(motionManager, sel);

		//acceleration
		BOOL isAccelerometerAvailable = msgSend_bool(motionManager, sel_registerName("isAccelerometerAvailable"));
		if(isAccelerometerAvailable){
			objc_msgSend(motionManager, sel_registerName("startAccelerometerUpdates"));

			BOOL isAccelerometerActive = NO;
			while(!isAccelerometerActive){
				isAccelerometerActive = msgSend_bool(motionManager, sel_registerName("isAccelerometerActive"));
				sleep(1);
			}
			id accelerometerData = NULL;
			while(!accelerometerData)
				accelerometerData = objc_msgSend(motionManager, sel_registerName("accelerometerData"));
			PP acceleration;
            acceleration = msgSend_pp(accelerometerData, sel_registerName("acceleration"));
			mbedtls_md_update(&ctx->md_ctx, (const unsigned char *)&acceleration, sizeof(acceleration));
			#if defined(ENABLE_DEBUG)
			printf("acceleration x:%lf y:%lf z:%lf\n", acceleration.x, acceleration.y, acceleration.z);
			#endif
			objc_msgSend(motionManager, sel_registerName("stopAccelerometerUpdates"));
		}

		//rotation
		BOOL isGyroAvailable = msgSend_bool(motionManager, sel_registerName("isGyroAvailable"));
		if(isGyroAvailable){
			objc_msgSend(motionManager, sel_registerName("startGyroUpdates"));
			BOOL isGyroActive = NO;
			while(!isGyroActive){
				isGyroActive = msgSend_bool(motionManager, sel_registerName("isGyroActive"));
				sleep(1);
			}
			id gyroData = NULL;
			while(!gyroData)
				gyroData = objc_msgSend(motionManager, sel_registerName("gyroData"));
			PP rotationRate;
            rotationRate = msgSend_pp(gyroData, sel_registerName("rotationRate"));
			mbedtls_md_update(&ctx->md_ctx, (const unsigned char *)&rotationRate, sizeof(rotationRate));
			#if defined(ENABLE_DEBUG)
			printf("rotation x:%lf y:%lf z:%lf\n", rotationRate.x, rotationRate.y, rotationRate.z);
			#endif
			objc_msgSend(motionManager, sel_registerName("stopGyroUpdates"));
		}

		//magnetic
		BOOL isMagnetometerAvailable = msgSend_bool(motionManager, sel_registerName("isMagnetometerAvailable"));
		if(isMagnetometerAvailable){
			objc_msgSend(motionManager, sel_registerName("startMagnetometerUpdates"));
			bool isMagnetometerActive = NO;
			while(!isMagnetometerActive){
				isMagnetometerActive = msgSend_bool(motionManager, sel_registerName("isMagnetometerActive"));
				sleep(1);
			}
			id magnetometerData = NULL;
			while(!magnetometerData)
				magnetometerData = objc_msgSend(motionManager, sel_registerName("magnetometerData"));
			PP magneticField;
            magneticField = msgSend_pp(magnetometerData, sel_registerName("magneticField"));
			mbedtls_md_update(&ctx->md_ctx, (const unsigned char *)&magneticField, sizeof(magneticField));
			#if defined(ENABLE_DEBUG)
			printf("magnetic x:%lf y:%lf z:%lf\n", magneticField.x, magneticField.y, magneticField.z);
			#endif
			objc_msgSend(motionManager, sel_registerName("stopMagnetometerUpdates"));
		}
	
	}
	#endif

	// Set Additional message
	if (buf_size > 0 && seed_buf != NULL) {
		mbedtls_md_update(&ctx->md_ctx, seed_buf, buf_size);
	}

	// Last, Finish hash
	ret = mbedtls_md_finish(&ctx->md_ctx, ctx->hash);
	if (ret) return ret;

	// Set seed
	mbedtls_hmac_drbg_seed_buf(&ctx->drbg_ctx, ctx->md_info, ctx->hash, ctx->hashLen);

	// Clear Seed Message
	memset(ctx->hash, 0, ctx->hashLen);

	ctx->isSeeded = 1;

	return 0;
}

int aisino_rand_rand(aisino_rand_context *ctx, void *output, size_t size) {
	return aisino_rand_rand_with_add(ctx, output, size, NULL, 0);
}

int aisino_rand_rand_with_add(aisino_rand_context *ctx, void *output, size_t size, const unsigned char *additional, size_t add_len) {
	if (ctx->isSeeded != 1) return AISINO_RAND_ERROR_NOT_SEEDED;

	int ret = mbedtls_hmac_drbg_random_with_add(&ctx->drbg_ctx, output, size, additional, add_len);
	switch(ret) {
		case MBEDTLS_ERR_HMAC_DRBG_REQUEST_TOO_BIG:
		return AISINO_RAND_ERROR_OUT_SIZE_TO_LARGE;
		default:
		return ret;
	}
}

void aisino_rand_free(aisino_rand_context* ctx) {
	if (ctx->isInitial != 1) return;

	mbedtls_md_free(&ctx->md_ctx);
	mbedtls_hmac_drbg_free(&ctx->drbg_ctx);

	free(ctx->hash);

	ctx->isInitial = 0;
	ctx->isSeeded = 0;
	aisinoSSLRandomContextCount--;

	free(ctx);
}


int aisino_rand_shuffle_u8(unsigned char *list, int len) {
	int t, roundCnt, ret;
    unsigned int *randNumbers;

    if (len <= 0) return AISINO_RAND_ERROR_INVLIAD_SIZE;
    randNumbers = (unsigned int *)malloc((len + 10) * sizeof(unsigned int));

	aisino_rand_list(NULL, (int*) randNumbers, len);

    while (len > 0) {
        int r = randNumbers[len] % len;
        len--;
        unsigned char tmp = *(list + len);
        *(list + len) = *(list + r);
        *(list + r) = tmp;
    }

    free(randNumbers);
    return 0;
}

aisino_rand_context *__ctx = NULL;

int aisino_rand_list(aisino_rand_context* ctx, int *list, int len) {
	int t, roundCnt, ret;
	
	if (len <= 0) return AISINO_RAND_ERROR_INVLIAD_SIZE;

	// Context do not exist
	if (ctx == NULL) {
		// Global context do not exist, create one!
		if (__ctx == NULL) {
			aisino_rand_init(&__ctx);
			ret = aisino_rand_seed(__ctx, NULL, 0);
			if (ret) return ret;
		} 
		ctx = (aisino_rand_context *)__ctx;
	}
    
	t = 0;
    while(t < len) {
        roundCnt = (len - t <= AISINO_RAND_MAX_INT_COUNT) ? (len - t) : AISINO_RAND_MAX_INT_COUNT;
        ret = aisino_rand_rand(ctx, (unsigned int *)list + t, roundCnt * sizeof(int));
        if (ret) return ret;
        t += roundCnt;
    }

	return ret;
}

#endif

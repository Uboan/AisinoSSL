#
# build.mk
# For Android Aisino Random Library
#
# RUN: ndk-build NDK_PROJECT_PATH=./ NDK_APPLICATION_MK=./build.mk
#

LOCAL_PATH := $(call my-dir)

include $(CLEAR_VARS)

APP_BUILD_SCRIPT := build.mk
NDK_APP_DST_DIR := ./android-lib/$(TARGET_ARCH_ABI)

APP_ABI := armeabi-v7a arm64-v8a x86 x86_64
APP_PLATFORM := android-16 # >= 4.1
APP_STL := c++_static

APP_CFLAGS += -frtti -fexceptions
APP_CFLAGS += -ffunction-sections -fdata-sections
APP_CFLAGS += -fvisibility=hidden

APP_LDFLAGS += -Wl,--gc-sections
APP_LDFLAGS += -Wl,--exclude-libs,ALL

LOCAL_C_INCLUDES += $(LOCAL_PATH)/include/
LOCAL_C_INCLUDES += $(LOCAL_PATH)/header/

LOCAL_SRC_FILES += $(wildcard $(LOCAL_PATH)/src/mbedtls/*.c)

#
# Compile Static Library
#

LOCAL_MODULE := aisinorand-lib

# Local config
LOCAL_CFLAGS += -DTARGET_PLATFORM_ANDROID
LOCAL_CFLAGS += -DAISINO_RAND_HASH_ALG_SM3
LOCAL_CFLAGS += -DAISINO_RAND_VER_MBEDTLS

# LOCAL_LDLIBS += -landroid
# LOCAL_LDLIBS += -llog # When Debug

include $(BUILD_STATIC_LIBRARY)

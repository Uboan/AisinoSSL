# 0. Options for Compiler
option(ENABLE_DEBUG "debug" OFF)
option(ENABLE_SEED_IOS_SENSOR "Enable iOS sensor as part of seed" ON)

# 1. Set cmake version
cmake_minimum_required(VERSION 3.2)

# 2. Set project name
PROJECT(aisino_rand)

# 3. set environment variable

# set name for build
SET(BUILD_NAME ${PROJECT_NAME})

# set debug
IF(CMAKE_BUILD_TYPE STREQUAL Debug)
    set(ENABLE_DEBUG ON)
ENDIF()

SET(BUILD_SHARED_NAME ${PROJECT_NAME}_SHARED)
SET(BUILD_STATIC_NAME ${PROJECT_NAME}_STATIC)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)

IF(BUILD_TARGET)
  message("Build target platform: ${BUILD_TARGET}")
ENDIF()

# Random library version
IF(LIB_VERSION STREQUAL AisinoSSL)
    message("TARGET LIBRARY: AisinoSSL Random module")
    set(AISINO_RAND_VER_MBEDTLS ON)
    set(AISINO_RAND_SRC_VER "mbedtls")
# ELSEIF(LIB_VERSION STREQUAL OpenSSL)
#    message("TARGET LIBRARY: OpenSSL Random module")
#    set(AISINO_RAND_VER_OPENSSL ON)
ELSE()
    message("TARGET LIBRARY: default (AisinoSSL Random module)")
    set(AISINO_RAND_VER_MBEDTLS ON)
    set(AISINO_RAND_SRC_VER "mbedtls")
ENDIF()

# Hash Algorithm
IF(HASH_ALG STREQUAL SM3)
    message("Hash algorithm: SM3")
    set(AISINO_RAND_HASH_ALG_SM3 ON)
ELSE()
    message("Hash algorithm: default (SM3)")
    set(AISINO_RAND_HASH_ALG_SM3 ON)
ENDIF()

# iOS Setting
IF(IOS_PLATFORM STREQUAL iOS)
    SET(TARGET_PLATFORM_IOS ON)

    # turn of Seed: iOS sensor 
    IF(ENABLE_SEED_IOS_SENSOR)
        SET(AISINO_RAND_ENABLE_SEED_IOS_SENSOR ON)
    ENDIF()

ELSEIF(BUILD_TARGET STREQUAL Android)
    IF(NOT DEFINED CMAKE_TOOLCHAIN_FILE)
        message(FATAL_ERROR "If you want to compile Android version, please add the definition CMAKE_TOOLCHAIN_FILE. For more details, please read the README.md")
    ENDIF()
ENDIF()

# ------------------
# Compiler Environment Configure
#
message("CMake Complier IS:")

IF(ENABLE_DEBUG)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Werror -O0 -g3")
ENDIF(ENABLE_DEBUG)

if(CMAKE_COMPILER_IS_GNUCC)
    # some warnings we want are not available with old GCC versions
    # note: starting with CMake 2.8 we could use CMAKE_C_COMPILER_VERSION
    message("GNUCC\n")
    execute_process(COMMAND ${CMAKE_C_COMPILER} -dumpversion
                    OUTPUT_VARIABLE GCC_VERSION)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -lm  -fPIC -D_POSIX_SOURCE -std=c99 -Wall -Wextra -W -Wdeclaration-after-statement -Wwrite-strings")
    if (GCC_VERSION VERSION_GREATER 4.5 OR GCC_VERSION VERSION_EQUAL 4.5)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wlogical-op")
    endif()
    if (GCC_VERSION VERSION_GREATER 4.8 OR GCC_VERSION VERSION_EQUAL 4.8)
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wshadow")
    endif()
    set(CMAKE_C_FLAGS_RELEASE     "-O2")
    set(CMAKE_C_FLAGS_DEBUG       "-O0 -g3")
    set(CMAKE_C_FLAGS_COVERAGE    "-O0 -g3 --coverage")
    set(CMAKE_C_FLAGS_ASAN        "-Werror -fsanitize=address -fno-common -O3")
    set(CMAKE_C_FLAGS_ASANDBG     "-Werror -fsanitize=address -fno-common -O1 -g3 -fno-omit-frame-pointer -fno-optimize-sibling-calls ")
    set(CMAKE_C_FLAGS_CHECK       "-Werror -Os")
    set(CMAKE_C_FLAGS_CHECKFULL   "${CMAKE_C_FLAGS_CHECK} -Wcast-qual")
endif(CMAKE_COMPILER_IS_GNUCC)

if(CMAKE_COMPILER_IS_CLANG)
    message("CLang\n")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -W -Wdeclaration-after-statement -Wwrite-strings -Wpointer-arith -Wimplicit-fallthrough -Wshadow")
    set(CMAKE_C_FLAGS_RELEASE     "-O2")
    set(CMAKE_C_FLAGS_DEBUG       "-O0 -g3")
    set(CMAKE_C_FLAGS_COVERAGE    "-O0 -g3 --coverage")
    set(CMAKE_C_FLAGS_ASAN        "-Werror -fsanitize=address -fno-common -fsanitize=undefined -fno-sanitize-recover -O3")
    set(CMAKE_C_FLAGS_ASANDBG     "-Werror -fsanitize=address -fno-common -fsanitize=undefined -fno-sanitize-recover -O1 -g3 -fno-omit-frame-pointer -fno-optimize-sibling-calls ")
    set(CMAKE_C_FLAGS_MEMSAN      "-Werror -fsanitize=memory -O3")
    set(CMAKE_C_FLAGS_MEMSANDBG   "-Werror -fsanitize=memory -O1 -g3 -fno-omit-frame-pointer -fno-optimize-sibling-calls -fsanitize-memory-track-origins=2")
    set(CMAKE_C_FLAGS_CHECK       "-Werror -Os")
endif(CMAKE_COMPILER_IS_CLANG)

if(MSVC)
    # Strictest warnings, and treat as errors
    message("MicroSoft Visual C\n")
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} /W3")
else(MSVC)
    set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -fvisibility=hidden")
endif(MSVC)

# ------------------
# Compiler Module Configure
#
message("Compiler Module Configure:")


# -- TODO --

# 5. Generate config file
configure_file(config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/src/${AISINO_RAND_SRC_VER}/config.h @ONLY)

# 6. head file path
INCLUDE_DIRECTORIES(header)
INCLUDE_DIRECTORIES(include)
INCLUDE_DIRECTORIES(include/AisinoSSL/mbedtls)
INCLUDE_DIRECTORIES(src/mbedtls)
# 7. set header install
add_subdirectory(header)

# 8. set source files
add_subdirectory(src) 

   
#  test DRNG 

AUX_SOURCE_DIRECTORY(src/mbedtls DIR_SRCS)
AUX_SOURCE_DIRECTORY(src DIR_SRCS)

ADD_LIBRARY(LIB ${DIR_SRCS})
add_executable(TEST_drng test/testdrng.c)
TARGET_LINK_LIBRARIES(TEST_drng LIB)

#  test Aisino

add_executable(TEST_aisino test/testAisino.c)
TARGET_LINK_LIBRARIES(TEST_aisino LIB)
# Aisino 随机数库 - AisinoRand

## 概要

Aisino随机数库为提供独立化的随机数获取以及数组shuffle等随机数相关的功能函数。

当前版本的随机数库基于AisinoSSL中的随机数模块。同时也提供一定的扩展性，未来可扩展OpenSSL的兼容。

## 编译

### Windows

必备软件：`cmake`、`Visual Studio` 或 `MinGW`

```powershell
mkdir build
cd build
cmake ..
```

打开 `build/AisinoRand.sln`，并使用 `MinSizeRel` 或 `Release` 配置编译，编译出的库在 `build/lib` 目录。

### Unix / inux

必备软件：`cmake`、`gcc`

```bash
mkdir build
cd build
cmake ..
make -j8
```

编译出的库在 `build/lib` 目录。

### macOS

必备软件：`cmake`、`gcc(clang)`

```bash
mkdir build
cd build
cmake ..
make -j8
```

编译出的库在 `build/lib` 目录。

### iOS

必备软件：`cmake`、`gcc(clang)`

```bash
mkdir build
cd build
cmake -DCMAKE_TOOLCHAIN_FILE=ios.toolchain.cmake -DIOS_PLATFORM=OS -DIOS_ARCH='armv7;arm64' ..
make -j8
```

编译出的库在 `build/lib` 目录。

#### 编译参数

`-DENABLE_SEED_IOS_SENSOR=OFF` 关闭iOS传感器数据作为种子数据源【默认iOS传感器打开】

### Android

必备软件：`ndk`

编译出的库在 `build/lib` 目录。

【推荐】使用mk脚本编译静态库：

mk脚本编译前，请写出 `config.h` 到 `/src/mbedtls/` 目录，内容如下：

```c
#ifndef CMAKE_RAND_CONFIG
#define CMAKE_RAND_CONFIG

// Lib version
#define AISINO_RAND_VER_MBEDTLS

// Hash Algorithm
#define AISINO_RAND_HASH_ALG_SM3

#endif
```

运行下面的指令开始编译：

```bash
ndk-build NDK_PROJECT_PATH=./ NDK_APPLICATION_MK=./build.mk
```

编译完成后库位于 `/obj/local` 文件夹。

【不推荐】Linux或macOS下，可以使用下面的命令编译：

```bash
mkdir build
cd build
cmake -DBUILD_TARGET=Android -DCMAKE_TOOLCHAIN_FILE=${NDK_ROOT}/build/cmake/android.toolchain.cmake -DANDROID_ABI='armeabi-v7a' ..
make
strip -s lib/libaisino_rand.so
```

> strip 用于去除SO库的特征，增大破解难度。

【不推荐】Windows下，请先下载 [Ninja](https://ninja-build.org/) 并设置 Ninja目录 到系统变量PATH中，再使用下面的命令编译：

```bash
mkdir build
cd build
cmake -DBUILD_TARGET=Android -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=%NDK_ROOT%/build/cmake/android.toolchain.cmake -DANDROID_ABI="armeabi-v7a" -DCMAKE_GENERATOR=Ninja ..
ninja
strip -s lib/libaisino_rand.so
```

备用编译方案：

```bash
ndk-build NDK_PROJECT_PATH=./ NDK_APPLICATION_MK=./build.mk
```

## 引用

若需要使用随机数库，您可以选择复制该目录或者软链接该目录到您的项目上，再到代码中include。

所有用于第三方调用随机数库的头文件存放于 `header` 目录（并非是 `include` 目录）。

## 接口

随机数库的接口位于 `AisinoRand/rand.h`

可使用 `#include <AisinoRand/rand.h>` 引用

### aisino_rand_init

初始化随机数模块上下文（Context）

接口：`int aisino_rand_init(void **ctx);`

### aisino_rand_seed

设置或者重设随机数模块的随机数种子。

接口：`int aisino_rand_seed(void* ctx, unsigned char *seed_buf, size_t buf_size);`

### aisino_rand_seed_with_option

携带配置的情况下，设置或者重设随机数模块的随机数种子。

接口：`int aisino_rand_seed_with_option(void *ctx, unsigned char *seed_buf, size_t buf_size, int options);`

Options 配置:

- `AISINO_RAND_DISABLE_TIME` 不使用当前时间戳作为种子
- `AISINO_RAND_DISABLE_URANDOM` 不使用 urandom 作为种子
- `AISINO_RAND_DISABLE_ANDROID_INFO` 不使用安卓设备信息作为种子
- `AISINO_RAND_DISABLE_IOS_SENSOR` 不使用iOS传感器作为种子

Options使用方法案例：

```c
AISINO_RAND_DISABLE_TIME | AISINO_RAND_DISABLE_URANDOM
```

### aisino_rand_rand

生成 `size` 个成员的 `unsigned char` 数组的随机数

接口：`int aisino_rand_rand(void *ctx, unsigned char *output, size_t size);`

### aisino_rand_rand_int_array

生成 `count` 个成员的 `int` 数组的随机数

接口：`int aisino_rand_rand_int_array(void *ctx, int *output, int count);`

### aisino_rand_rand_uint_array

生成 `count` 个成员的 `unsigned int` 数组的随机数

接口：`int aisino_rand_rand_uint_array(void *ctx, unsigned int *output, int count);`

### aisino_rand_free

释放随机数模块上下文

接口：`void aisino_rand_free(void *ctx);`

### aisino_rand_shuffle_u8

混淆 `len` 个成员的 `unsigned char` 数组

接口：`int aisino_rand_shuffle_u8(unsigned char *list, int len);`

### aisino_rand_list

混淆 `len` 个成员的 `int` 数组

接口：`int aisino_rand_list(void* ctx, int *list, int len);`

> 当 `ctx == NULL` 时，将会在内部初始化一个全局的上下文。

## 范例代码

### 随机生成5个int数

```c
void *ctx;
int arr[5];
memset(arr, 0, sizeof(arr));
aisino_rand_init(&ctx);
aisino_rand_seed(ctx, NULL, 0);
aisino_rand_rand_int_array(ctx, arr, 5);
aisino_rand_free(ctx);
```

### 随机混淆5个int数

```c
unsigned char numbers[5] = { 1, 2, 3, 4, 5 };
aisino_rand_shuffle_u8(numbers, 5);
```

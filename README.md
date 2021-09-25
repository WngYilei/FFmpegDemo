# Linux 交叉编译FFmpeg库

最近打算学习一下NDK方面的知识，想通过文章来记录一下学习笔记

我们都知道java是跨平台的，一套代码，多个平台都可以正常使用，而C，C++ 却不是，可能在windos上正常编译使用，但是在linux不能使用，为了让C，C++的代码能够在Android系统上使用，就需要用NDK 去编译，然后将编译后的库 移植到Android 上使用，这就是交叉编译。接下来我们就编译一下FFmpeg库。

首先说明一下我的环境，编译的系统用的是Centos7 ，FFmpeg版本选择的是4.0.2

分为一下几步来进行讲解：

1.配置环境

2.编写脚本

3.交叉编译

4.移植使用

首先我们需要在Linux 上配置 NDK 环境和 FFmpeg

Linux：

通过

```shell
wget https://dl.google.com/android/repository/android-ndk-r17c-linux-x86_64.zip? hl=zh_cn  
```

下载NDK，然后解压，再在/etc/profile中配置环境变量

```shell
#NDK
export NDK=/root/NDK/android-ndk-r17c //这里修改为你自己的NDK路径就好了
export NDK_GCC_x86="$NDK/toolchains/x86-4.9/prebuilt/linux-x86_64/bin/i686- linux-android-gcc"
export NDK_GCC_x64="$NDK/toolchains/x86_64-4.9/prebuilt/linux-x86_64/bin/x86_64- linux-android-gcc"
export NDK_GCC_arm="$NDK/toolchains/arm-linux-androideabi-4.9/prebuilt/linux- x86_64/bin/arm-linux-androideabi-gcc"
export NDK_GCC_arm_64="$NDK/toolchains/aarch64-linux-android-4.9/prebuilt/linux- x86_64/bin/aarch64-linux-android-gcc"
export NDK_CFIG_x86="--sysroot=$NDK/platforms/android-21/arch-x86 -isystem $NDK/sysroot/usr/include -isystem $NDK/sysroot/usr/include/i686-linux-android"
export NDK_CFIG_x64="--sysroot=$NDK/platforms/android-21/arch-x86_64 -isystem $NDK/sysroot/usr/include -isystem $NDK/sysroot/usr/include/x86_64-linux-android"
export NDK_CFIG_arm="--sysroot=$NDK/platforms/android-21/arch-arm -isystem $NDK/sysroot/usr/include -isystem $NDK/sysroot/usr/include/arm-linux- androideabi"
export NDK_CFIG_arm_64="--isysroot=$NDK/platforms/android-21/arch-arm64 -isystem $NDK/sysroot/usr/include -isystem -isystem $NDK/sysroot/usr/include/aarch64- linux-android"
```



然后是下载FFmpeg

```shell
wget http://www.ffmpeg.org/releases/ffmpeg-4.0.2.tar.bz2 
```

解压之后，通过./configure --help  可以查看 ffmpeg 的帮助文档



ffmpeg 大致分为一下几个模块:

libavformat:用于各种音视频封装格式的生成和解析，包括获取解码所需信息以生成解码上下文结构和 读取音视频帧等功能;

libavcodec:用于各种类型声音/图像编解码;

libavutil:包含一些公共的工具函 数;

libswscale:用于视频场景比例缩放、色彩映射转换;

libpostproc:用于后期效果处理;

ffmpeg:该项目提供的一个工具，可用于格式转换、解码或电视卡即时编码等;

ffsever:一个 HTTP 多媒体即时广播串流服务器;

ffplay:是一个简单的播放器，使用ffmpeg 库解析和解码，通过SDL显 示;



接下来就是编写 编译脚本,这里就需要一些shell 脚本的知识了

编译过程中需要用到NDK，所有需要先定义NDK 的路径

```shell
NDK_ROOT=/xxx/android-ndk-r17c
```

指定执行NDK中交叉编译的GCC的路径

```shell
TOOLCHAIN=$NDK_ROOT/toolchains/arm-linux-androideabi-4.9/prebuilt/linux-x86_64
```

然后是定义FLAGS,这个是给GCC的传参，这个是参考 android 项目中的externalNativeBuild/xxx/build.ninja 的传参内容结合Linux环境进行修改的

```shell
FLAGS="-isystem $NDK_ROOT/sysroot/usr/include/arm-linux-androideabi -D__ANDROID_API__=21 -g -DANDROID -ffunction-sections -funwind-tables -fstack-protector-strong -no-canonical-prefixes -march=armv7-a -mfloat-abi=softfp -mfpu=vfpv3-d16 -mthumb -Wa,--noexecstack -Wformat -Werror=format-security  -O0 -fPIC"
INCLUDES=" -isystem $NDK_ROOT/sources/android/support/include"
```

最后再定义一下 我们库的输出路径

```shell
PREFIX=./android/arm
```

在然后就是一些 ffmpeg的参数配置了

```shell
./configure \
--prefix=$PREFIX \
--enable-small \
--disable-programs \
--disable-avdevice \
--disable-encoders \
--disable-muxers \
--disable-filters \
--enable-cross-compile \
--cross-prefix=$TOOLCHAIN/bin/arm-linux-androideabi- \
--disable-shared \
--enable-static \
--sysroot=$NDK_ROOT/platforms/android-21/arch-arm \
--extra-cflags="$FLAGS $INCLUDES" \
--extra-cflags="-isysroot $NDK_ROOT/sysroot/" \
--arch=arm \
--target-os=android
```

--enable-small 优化大小 非常重要，必须优化才行的哦

--disable-programs 不编译ffmpeg程序（命令行工具），我们是需要获取静态、动态库

--disable-avdevice 关闭avdevice模块，此模块在android中无用

--disable-encoders 关闭所有编码器（播放不需要编码）

--disable-muxers 关闭所有复用器（封装器），不需要生成mp4这样的文件，所有关闭

.--disable-filters 关闭所有滤镜

--enable-cross-compile 开启交叉编译（ffmpeg是跨平台的，注意：并不是所有库都有这么happy的选项）

--cross-prefix 看右边的值就知道是干嘛的，gcc的前缀..

.disable-shared / enable-static 这个不写也可以，默认就是这样的，（代表关闭动态库，开启静态库）

--sysroot

--extra-cflags 会传给gcc的参数

--arch  --target-os

这是一些 配置的解释，更多的配置可以通过./configure --help 去查看文档

最后 在通过make 来完成编译输出

```shell
make clean
make install
```

在编写脚本的过程中 会有一些注意事项:

1.在编译前要 执行 ./configure --disable-x86asm ，来关闭 asm，不使用汇编优化，不然编译器报错

2.注意权限，一定要是root权限

3.最好手动创建输出的文件夹

4.\ 后面不能有空格，不能有注释

最后 使用sh 命令 执行 脚本文件就好了

执行后，会报出一个错误

![image-20210925182506155](/Users/wyl/Library/Application Support/typora-user-images/image-20210925182506155.png)

关于这个错误 我们可以不用关系，然后等待编译就好了

等到控制台完成编译，我们进入输出目录看一下

如果有内容，就证明编译成功了，如果没内容那就是编译失败了

![image-20210925183732079](/Users/wyl/Library/Application Support/typora-user-images/image-20210925183732079.png)

编译成功的情况下有这三个文件夹。



至此，ffmpeg库我们就已经编译好了，接下来就是移植到我们Android 的项目中使用了。

在这块，我是创建了一个新的NDK 的项目，然后将编译好的include 文件夹复制到 /src/main/cpp 目录下，

再在cpp 目录下创建 armeabi-v7a，将ffmpeg lib 目录下的文件 复制到armeabi-v7a目录下，pkgconfig目录下的不用复制，这是Android暂时用不到。

然后需要在Cmake文件中引入库

```cmake
#引入头文件 FFmpeg
include_directories(${CMAKE_SOURCE_DIR}/include)
#引入库文件FFmpeg
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/${CMAKE_ANDROID_ARCH_ABI}")

target_link_libraries(
        #注意顺序 avcodec 是依赖于 avformat 的，所以avformat 要在前面，否则会导致编译失败
        avformat avcodec avfilter avutil swresample swscale
        )
```

然后我们就可以愉快的使用ffmpeg 库了

因为FFmpeg是纯C 的，所以我们需要这样引用

```cpp
extern "C"{
#include <libavutil/avutil.h>
}
```

最后我们显示一下 ffmpeg 的版本号

```cpp
Java_com_xl_ffmpeg_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "当前FFmpeg的版本是";
    hello.append(av_version_info());
    return env->NewStringUTF(hello.c_str());
}
```

![device](/Users/wyl/Desktop/device.png)

至此，关于ffmpeg 从交叉编译到移植Android 使用流程都跑通了。

有相关问题可以留言交流。

Demo：https://gitee.com/yileiwang/ffmpeg.git


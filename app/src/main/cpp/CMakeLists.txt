# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html

# Sets the minimum version of CMake required to build the native library.

cmake_minimum_required(VERSION 3.10.2)

project("ffmpeg")

add_library(
        ffmpeg
        SHARED
        native-lib.cpp)
#因为头文件 FFmpeg
include_directories(${CMAKE_SOURCE_DIR}/include)
#引入库文件FFmpeg
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -L${CMAKE_SOURCE_DIR}/${CMAKE_ANDROID_ARCH_ABI}")

find_library(
        log-lib
        log)

target_link_libraries(
        ffmpeg
        #注意顺序 avcodec 是依赖于 avformat 的，所以avformat 要在前面，否则会导致编译失败
        avformat avcodec avfilter avutil swresample swscale
        ${log-lib})
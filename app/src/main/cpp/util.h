#ifndef NE_PLAYER_MACRO_H
#define NE_PLAYER_MACRO_H

#define THREAD_MAIN 1 // 主线程
#define THREAD_CHILD 2 // 子线程

#endif //NE_PLAYER_MACRO_H

#include <string>

#include <android/log.h>

#define  LOG_TAG    "NativeLog->>>>>>>>>>>>>>"
#define  LOGI(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGD(...)  __android_log_print(ANDROID_LOG_INFO, LOG_TAG, __VA_ARGS__)
#define LOGE(...)  __android_log_print(ANDROID_LOG_ERROR, LOG_TAG, __VA_ARGS__)

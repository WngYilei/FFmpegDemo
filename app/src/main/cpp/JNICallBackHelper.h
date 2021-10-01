//
// Created by 王一蕾 on 2021/10/1.
//

#ifndef FFMPEGDEMO_JNICALLBACKHELPER_H
#define FFMPEGDEMO_JNICALLBACKHELPER_H

#include "JNICallBackHelper.h"
#include "jni.h"

class JNICallBackHelper {

private:
    JavaVM *vm = 0;
    JNIEnv *env = 0;
    jobject job = 0;
    jmethodID methodPrepare = 0;

    jmethodID methodError = 0;

public:
    JNICallBackHelper();

    JNICallBackHelper(JavaVM *vm, JNIEnv *env, jobject job);

    ~JNICallBackHelper();

    void onPrepare(int i);

    void onError(int i, const char string[1]);
};


#endif //FFMPEGDEMO_JNICALLBACKHELPER_H

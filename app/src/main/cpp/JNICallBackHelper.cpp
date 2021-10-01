//
// Created by 王一蕾 on 2021/10/1.
//

#include "JNICallBackHelper.h"
#include "util.h"

JNICallBackHelper::JNICallBackHelper() {

}

JNICallBackHelper::JNICallBackHelper(JavaVM *pVm, JNIEnv *env, jobject job) {
    this->env = env;
    this->vm = pVm;
//    this->pJobject = job;  jobject 不能跨越函数，必须全局引用

    this->job = env->NewGlobalRef(job);

    jclass clazz = env->GetObjectClass(job);

    methodPrepare = env->GetMethodID(clazz, "onPrepare", "()V");
    methodError = env->GetMethodID(clazz, "onError", "(Ljava/lang/String;)V");

}

JNICallBackHelper::~JNICallBackHelper() {
    vm = 0;
    env->DeleteGlobalRef(job);
    job = 0;
    env = 0;

}

void JNICallBackHelper::onPrepare(int thread_model) {
    LOGE("JNICallBackHelper===onPrepare");
    if (thread_model == THREAD_MAIN) {
        env->CallVoidMethod(job, methodPrepare);
    } else if (thread_model == THREAD_CHILD) {
//   子线程，env 不能 跨线程 ，需要全新的env
        JNIEnv *envChild;
        vm->AttachCurrentThread(&envChild, 0);
        envChild->CallVoidMethod(job, methodPrepare);
        vm->DetachCurrentThread();
    }
}

void JNICallBackHelper::onError(int thread_model, const char *string) {
    jstring msg = env->NewStringUTF(string);

    if (thread_model == THREAD_MAIN) {
        env->CallVoidMethod(job, methodPrepare, msg);
    } else if (thread_model == THREAD_CHILD) {
//   子线程，env 不能 跨线程 ，需要全新的env
        JNIEnv *envChild;
        vm->AttachCurrentThread(&envChild, 0);
        envChild->CallVoidMethod(job, methodError, msg);
        vm->DetachCurrentThread();
    }
}

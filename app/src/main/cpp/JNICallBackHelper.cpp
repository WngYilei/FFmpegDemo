#include "JNICallBackHelper.h"
#include "util.h"

JNICallBackHelper::JNICallBackHelper() = default;

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
    vm = nullptr;
    env->DeleteGlobalRef(job);
    job = nullptr;
    env = nullptr;

}

void JNICallBackHelper::onPrepare(int thread_model) {
    if (thread_model == THREAD_MAIN) {
        env->CallVoidMethod(job, methodPrepare);
    } else if (thread_model == THREAD_CHILD) {
//   子线程，env 不能 跨线程 ，需要全新的env
        JNIEnv *envChild;
        vm->AttachCurrentThread(&envChild, nullptr);
        envChild->CallVoidMethod(job, methodPrepare);
        vm->DetachCurrentThread();
    }
}

void JNICallBackHelper::onError(int thread_model, const char *string) {

    if (thread_model == THREAD_MAIN) {
        jstring msg = env->NewStringUTF(string);
        env->CallVoidMethod(job, methodPrepare, msg);
    } else if (thread_model == THREAD_CHILD) {
//      子线程，env 不能 跨线程 ，需要全新的env
        JNIEnv *envChild;
        vm->AttachCurrentThread(&envChild, nullptr);
        jstring msg = envChild->NewStringUTF(string);
        envChild->CallVoidMethod(job, methodError, msg);
        vm->DetachCurrentThread();
    }
}

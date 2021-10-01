#include <jni.h>
#include <string>
#include "FFmpegPlay.h"
#include "JNICallBackHelper.h"
//FFmpeg 是纯C的
extern "C" {
#include <libavutil/avutil.h>
}


extern "C" JNIEXPORT jstring JNICALL
Java_com_xl_ffmpeg_play_FFmpegPlay_stringFromJNI(JNIEnv *env, jobject thiz) {
    std::string hello = "当前FFmpeg的版本是";
    hello.append(av_version_info());
    return env->NewStringUTF(hello.c_str());
}
JavaVM *vm = 0;

jint JNI_OnLoad(JavaVM *vm, void *arfs) {
    ::vm = vm;
    return JNI_VERSION_1_6;
}

extern "C" JNIEXPORT void JNICALL
Java_com_xl_ffmpeg_play_FFmpegPlay_nativePrepare(JNIEnv *env, jobject thiz, jstring data_source) {
    const char *date_source_ = env->GetStringUTFChars(data_source, 0);
    auto *helper = new JNICallBackHelper(vm, env,thiz);
    auto *play = new FFmpegPlay(date_source_, helper);
    play->prepare();
    env->ReleaseStringUTFChars(data_source, date_source_);
}


extern "C" JNIEXPORT void JNICALL
Java_com_xl_ffmpeg_play_FFmpegPlay_nativeStartPlay(JNIEnv *env, jobject thiz) {

}


extern "C" JNIEXPORT void JNICALL
Java_com_xl_ffmpeg_play_FFmpegPlay_nativeStop(JNIEnv *env, jobject thiz) {

}


extern "C" JNIEXPORT void JNICALL
Java_com_xl_ffmpeg_play_FFmpegPlay_naticeRelease(JNIEnv *env, jobject thiz) {

}
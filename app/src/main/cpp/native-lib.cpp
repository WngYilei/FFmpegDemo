#include <jni.h>
#include <string>
//FFmpeg 是纯C的
extern "C"{
#include <libavutil/avutil.h>
}
extern "C" JNIEXPORT jstring JNICALL
Java_com_xl_ffmpeg_MainActivity_stringFromJNI(
        JNIEnv* env,
        jobject /* this */) {
    std::string hello = "当前FFmpeg的版本是";
  hello.append(av_version_info());
    return env->NewStringUTF(hello.c_str());
}
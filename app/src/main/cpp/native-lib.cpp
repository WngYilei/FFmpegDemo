#include <jni.h>
#include <string>
#include "FFmpegPlay.h"
#include "JNICallBackHelper.h"
//FFmpeg 是纯C的
extern "C" {
#include <libavutil/avutil.h>
}

#include <android/native_window_jni.h> // ANativeWindow 用来渲染画面的 == Surface对象


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

FFmpegPlay *play;
ANativeWindow *window = 0;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER; // 静态初始化 锁



// 函数指针 实现  渲染工作
void renderFrame(uint8_t * src_data, int width, int height, int src_lineSize) {
    pthread_mutex_lock(&mutex);
    if (!window) {
        pthread_mutex_unlock(&mutex); // 出现了问题后，必须考虑到，释放锁，怕出现死锁问题
    }

    // 设置窗口的大小，各个属性
    ANativeWindow_setBuffersGeometry(window, width, height, WINDOW_FORMAT_RGBA_8888);

    // 他自己有个缓冲区 buffer
    ANativeWindow_Buffer window_buffer; // 目前他是指针吗？

    // 如果我在渲染的时候，是被锁住的，那我就无法渲染，我需要释放 ，防止出现死锁
    if (ANativeWindow_lock(window, &window_buffer, 0)) {
        ANativeWindow_release(window);
        window = 0;

        pthread_mutex_unlock(&mutex); // 解锁，怕出现死锁
        return;
    }

    // TODO 开始真正渲染，因为window没有被锁住了，就可以把 rgba数据 ---> 字节对齐 渲染
    // 填充window_buffer  画面就出来了  === [目标]
    uint8_t *dst_data = static_cast<uint8_t *>(window_buffer.bits);
    int dst_linesize = window_buffer.stride * 4;
    for (int i = 0; i < window_buffer.height; ++i) { // 图：一行一行显示 [高度不用管，用循环了，遍历高度]
        // 视频分辨率：426 * 240
        // 视频分辨率：宽 426
        // 426 * 4(rgba8888) = 1704
        // memcpy(dst_data + i * 1704, src_data + i * 1704, 1704); // 花屏
        // 花屏原因：1704 无法 64字节对齐，所以花屏

        // ANativeWindow_Buffer 64字节对齐的算法，  1704无法以64位字节对齐
        // memcpy(dst_data + i * 1792, src_data + i * 1704, 1792); // OK的
//         memcpy(dst_data + i * 1793, src_data + i * 1704, 1793); // 部分花屏，无法64字节对齐
        // memcpy(dst_data + i * 1728, src_data + i * 1704, 1728); // 花屏

        // ANativeWindow_Buffer 64字节对齐的算法  1728
        // 占位 占位 占位 占位 占位 占位 占位 占位
        // 数据 数据 数据 数据 数据 数据 数据 空值

        // ANativeWindow_Buffer 64字节对齐的算法  1792  空间换时间
        // 占位 占位 占位 占位 占位 占位 占位 占位 占位
        // 数据 数据 数据 数据 数据 数据 数据 空值 空值

        // FFmpeg为什么认为  1704 没有问题 ？
        // FFmpeg是默认采用8字节对齐的，他就认为没有问题， 但是ANativeWindow_Buffer他是64字节对齐的，就有问题

        // 通用的
        memcpy(dst_data + i * dst_linesize, src_data + i * src_lineSize, dst_linesize); // OK的
    }

    // 数据刷新
    ANativeWindow_unlockAndPost(window); // 解锁后 并且刷新 window_buffer的数据显示画面

    pthread_mutex_unlock(&mutex);
}




extern "C" JNIEXPORT void JNICALL
Java_com_xl_ffmpeg_play_FFmpegPlay_nativePrepare(JNIEnv *env, jobject thiz, jstring data_source) {
    const char *date_source_ = env->GetStringUTFChars(data_source, 0);
    auto *helper = new JNICallBackHelper(vm, env, thiz);
    play = new FFmpegPlay(date_source_, helper);
    play->setRenderCallback(renderFrame);
    play->prepare();
    env->ReleaseStringUTFChars(data_source, date_source_);
}


extern "C" JNIEXPORT void JNICALL
Java_com_xl_ffmpeg_play_FFmpegPlay_nativeStartPlay(JNIEnv *env, jobject thiz) {
    play->start();

}


extern "C" JNIEXPORT void JNICALL
Java_com_xl_ffmpeg_play_FFmpegPlay_nativeStop(JNIEnv *env, jobject thiz) {

}


extern "C" JNIEXPORT void JNICALL
Java_com_xl_ffmpeg_play_FFmpegPlay_naticeRelease(JNIEnv *env, jobject thiz) {

}

extern "C"
JNIEXPORT void JNICALL
Java_com_xl_ffmpeg_play_FFmpegPlay_setSurfaceNative(JNIEnv *env, jobject thiz, jobject surface) {

    pthread_mutex_lock(&mutex);

    if (window) {
        ANativeWindow_release(window);
        window = 0;
    }
    window = ANativeWindow_fromSurface(env, surface);
    pthread_mutex_unlock(&mutex);
}


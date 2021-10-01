#ifndef FFMPEGDEMO_FFMPEGPLAY_H
#define FFMPEGDEMO_FFMPEGPLAY_H

#include <cstring>
#include "FFmpegPlay.h"
#include <pthread.h>
#include <pthread.h>
#include "AudioChannel.h"
#include "VideoChannel.h"
#include "JNICallBackHelper.h"
#include "AudioChannel.h"
#include "VideoChannel.h"
#include "JNICallBackHelper.h"
#include "util.h"
extern "C" {
#include "libavformat/avformat.h"
};

class FFmpegPlay {

private:
//  指针请赋初始值，防止地址乱指
    char *date_source = 0;
    pthread_t pid_prepare;
    AVFormatContext *formatContext = 0;
    AudioChannel *audio_channel = 0;
    VideoChannel *video_channel = 0;
    JNICallBackHelper *helper = 0;
public:
    FFmpegPlay(const char *date_source, JNICallBackHelper *pHelper);

    ~FFmpegPlay();

    void prepare();

    void prepare_();
};


#endif //FFMPEGDEMO_FFMPEGPLAY_H

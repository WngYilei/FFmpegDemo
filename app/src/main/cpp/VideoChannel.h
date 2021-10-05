#ifndef FFMPEGDEMO_VIDEOCHANNEL_H
#define FFMPEGDEMO_VIDEOCHANNEL_H


#include "BaseChannel.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}


typedef void(*RenderCallback)(uint8_t *, int, int, int); // 函数指针声明定义

class VideoChannel : public BaseChannel {

private:
    pthread_t pid_video_decode;
    pthread_t pid_video_play;
    RenderCallback renderCallback;

public:
    VideoChannel(int streamIndex, AVCodecContext *avCodecContext);

    void stop();

    ~VideoChannel();

    void start();


    void video_decode();

    void setRenderCallback(RenderCallback renderCallback);

    void video_play();
};


#endif //FFMPEGDEMO_VIDEOCHANNEL_H

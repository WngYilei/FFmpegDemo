//
// Created by 王一蕾 on 2021/10/3.
//

#ifndef FFMPEGDEMO_BASECHANNEL_H
#define FFMPEGDEMO_BASECHANNEL_H

#include "safe_queue.h"

extern "C" {
#include <libavcodec/avcodec.h>
#include <libswscale/swscale.h>
#include <libavutil/imgutils.h>
}


class BaseChannel {

public:
    int stream_index;

    SafeQueue<AVPacket *> packes;
    SafeQueue<AVFrame *> frames;
    bool isPlay;
    AVCodecContext *avCodecContext = 0;

    BaseChannel(int streamIndex, AVCodecContext *avCodecContext) :
            stream_index(streamIndex),
            avCodecContext(avCodecContext) {
        packes.setReleaseCallback(releaseAVPacket);
        frames.setReleaseCallback(releaseAVFrame);
    }

    ~BaseChannel() {
        packes.clear();
        frames.clear();
    }

    static void releaseAVPacket(AVPacket **p) {
        if (p) {
            av_packet_free(p);
            *p = 0;
        }
    }

    static void releaseAVFrame(AVFrame **f) {
        if (f) {
            av_frame_free(f);
            *f = 0;
        }
    }

};

#endif //FFMPEGDEMO_BASECHANNEL_H

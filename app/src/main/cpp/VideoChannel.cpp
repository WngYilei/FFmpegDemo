
#include "VideoChannel.h"
#include "util.h"

VideoChannel::VideoChannel(int streamIndex, AVCodecContext *avCodecContext)
        : BaseChannel(streamIndex, avCodecContext) {


};

void VideoChannel::stop() {


}

void *task_video_decode(void *args) {
    auto *video_channel = static_cast<VideoChannel *>(args);
    video_channel->video_decode();
    return nullptr;//必须返回
}

void *task_video_play(void *args) {
    auto *video_channel = static_cast<VideoChannel *>(args);
    video_channel->video_play();
    return nullptr;//必须返回
}

void VideoChannel::start() {
    isPlay = 1;
    packes.setWork(1);
    frames.setWork(1);
//第一个线程，取出队列的压缩包进行编码，再将编码后的数据 push 到队列中
    pthread_create(&pid_video_decode, 0, task_video_decode, this);
//第二个线程 从队列中取出原始包，进行播放
    pthread_create(&pid_video_play, 0, task_video_play, this);
}

void VideoChannel::video_decode() {

    AVPacket *pck = 0;
    while (isPlay) {
        int ret = packes.getQueueAndDel(pck);
        if (!isPlay) break;
        if (!ret) {
            continue;   //没有成功  也要继续
        }
        ret = avcodec_send_packet(avCodecContext, pck);
        releaseAVPacket(&pck);
        if (ret) {
            return;
        }

        AVFrame *frame = av_frame_alloc();
        ret = avcodec_receive_frame(avCodecContext, frame);
        if (ret == AVERROR(EAGAIN)) {
//          B帧参考前面成功，参考后面失败，可能P帧没出来，再拿一次就好了
            continue;
        } else if (ret != 0) {
            break;  //发生错误  直接停止
        }
        frames.insertToQueue(frame);
    }
    releaseAVPacket(&pck);


}

void VideoChannel::video_play() {
    AVFrame *avFrame = 0;
    uint8_t *dst_data[4];
    int dstlinesize[4];
    av_image_alloc(dst_data,
                   dstlinesize,
                   avCodecContext->width,
                   avCodecContext->height,
                   AV_PIX_FMT_RGBA,
                   1);
    SwsContext *swsContext = sws_getContext(
//          输入参数
            avCodecContext->width,
            avCodecContext->height,
            avCodecContext->pix_fmt,  //自动获取MP4的像素格式

//          输出结果
            avCodecContext->width,
            avCodecContext->height,
            AV_PIX_FMT_RGBA,
            SWS_BILINEAR,
            NULL, NULL, NULL);
    while (isPlay) {
        int ret = frames.getQueueAndDel(avFrame);
        if (!isPlay) {
            break;
        }


        if (!ret) {
            continue;
        }
//      格式转换  YUV 转 RGBA
        sws_scale(
//              输入参数
                swsContext,
                avFrame->data,
                avFrame->linesize,
                0,
                avCodecContext->height,

//              输出结果
                dst_data, dstlinesize
        );
        renderCallback(dst_data[0], avCodecContext->width, avCodecContext->height, dstlinesize[0]);
        releaseAVFrame(&avFrame); // 释放原始包，因为已经被渲染完了，没用了
    }

    releaseAVFrame(&avFrame);
    isPlay = 0;
    av_free(&dst_data[0]);
    sws_freeContext(swsContext);
}

void VideoChannel::setRenderCallback(RenderCallback renderCallback) {
    this->renderCallback = renderCallback;
}

VideoChannel::~VideoChannel() = default;

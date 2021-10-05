//所有引用包放头文件，开发规范

#include "FFmpegPlay.h"

FFmpegPlay::FFmpegPlay(const char *date_source, JNICallBackHelper *pHelper) {
//  有释放风险，如果被释放会造成悬空指针
//  this->date_source  =date_source;

//  深拷贝
//    this->date_source = new char[strlen(date_source)];
//  java demo.mp4
//  C层  demo.mp4\0   c会自动+ \0 strlen 不会计算\0的长度，需要手动+0

    this->date_source = new char[strlen(date_source) + 1];
    this->helper = pHelper;
    strcpy(this->date_source, date_source);   //copy 给成员变量

}


FFmpegPlay::~FFmpegPlay() {
    delete date_source;
    delete helper;
}


// void* (*__start_routine)(void*)  函数指针
//此函数和 play 对象没有关系 ，没办法获取 play对象的私有
void *task_prepeare(void *args) {

//    avformat_open_input(0,this-)
    auto *player = static_cast<FFmpegPlay *>(args);
    player->prepare_();
    return nullptr;//必须返回
}

//此函数是子线程
// prepare 传对象给 子线程
void FFmpegPlay::prepare_() {
    int r;
    //  ffmpeg 大量使用上下文 ，因为ffmpeg是纯C的,使用上下文,是为了贯穿环境,就相当于java 的this
    formatContext = avformat_alloc_context();//存在内存泄露问题，以后专门处理
    AVDictionary *avDictionary = nullptr;
    av_dict_set(&avDictionary, "timeout", "500000", 0);

/**
 * 第一步 打开媒体
 * 1.formatContext上下文
 * 2.播放地址
 * 3.AVInputFormat  打开 mac win 摄像头 麦克风
 * 4.各种设置，例如连接超时，打开rtmp超时时间
 * */
    r = avformat_open_input(&formatContext, date_source, nullptr, &avDictionary);
//  释放字典
    av_dict_free(&avDictionary);

    if (r) {
//      失败的情况，把错误信息回调给java
        if (helper) {
            char *contant = av_err2str(r);
            helper->onError(THREAD_CHILD, contant);
        }
        return;
    }

/**
 * 第二步 查找媒体流 的音视频信息
 * 1上下文
 * */
    r = avformat_find_stream_info(formatContext, nullptr);
    if (r < 0) {
//      检查失败
        if (helper) {
            char *contant = av_err2str(r);
            helper->onError(THREAD_CHILD, contant);
        }
        return;
    }

/**
 * 第三步 根据流信息 流的个数 ，用循环找
 * */

    for (int i = 0; i < formatContext->nb_streams; ++i) {

//     四 获取媒体流（视频流，音频流）
        AVStream *avStream = formatContext->streams[i];
//     五 从流中获取解码的参数，后面的解码器  编码器 都需要编码的参数
        AVCodecParameters *parameters = avStream->codecpar;
//     六 获取解码器 ,根据上面的参数
        AVCodec *avCodec = avcodec_find_decoder(parameters->codec_id);
//     七 获取编解码器 上下文，真正干活的东西
//     目前是一张白纸
        AVCodecContext *avCodecContexta = avcodec_alloc_context3(avCodec);
        if (!avCodecContexta) {
//          获取失败
            if (helper) {
                char *contant = av_err2str(r);
                helper->onError(THREAD_CHILD, contant);
            }
            return;
        }

//      八 将 parameters  copy 给 avcodeContext
        r = avcodec_parameters_to_context(avCodecContexta, parameters);
        if (r < 0) {
//          失败
            if (helper) {
                char *contant = av_err2str(r);
                helper->onError(THREAD_CHILD, contant);
            }
            return;
        }

//      九 打开解码器
        r = avcodec_open2(avCodecContexta, avCodec, nullptr);
        if (r) {
            if (helper) {
                char *contant = av_err2str(r);
                helper->onError(THREAD_CHILD, contant);
            }
            return;
        }
//      十 从编解码器中 获取流的类型 ，codec_type  是音频 还是视频
        if (parameters->codec_type == AVMediaType::AVMEDIA_TYPE_AUDIO) {
//            audio_channel = new AudioChannel();
        } else if (parameters->codec_type == AVMEDIA_TYPE_VIDEO) {
            video_channel = new VideoChannel(0, avCodecContexta);
            video_channel->setRenderCallback(renderCallback);
        }
    }

//   十一,如果流中没有音频和视频，健壮性校验，
    if (!video_channel && !video_channel) {
//      两个都没初始化
        if (helper) {
            helper->onError(THREAD_CHILD, "获取媒体流类型错误");
        }
        return;
    }

//   十二 准备成功，通知上层
    if (helper) {
        helper->onPrepare(THREAD_CHILD);
    }
}


void FFmpegPlay::prepare() {
//  此函数是在主线程调用
// 解封装 FFmpeg来解析，data_source == 文件IO流，网络rtmp，会耗时，所以必须用子线程
    pthread_create(&pid_prepare, nullptr, task_prepeare, this);
}

// void* (*__start_routine)(void*)  函数指针
//此函数和 play 对象没有关系 ，没办法获取 play对象的私有
void *task_start(void *args) {

//    avformat_open_input(0,this-)
    auto *player = static_cast<FFmpegPlay *>(args);
    player->start_();
    return nullptr;//必须返回
}

void FFmpegPlay::start() {
    isPlay = 1;
    if (video_channel) {
        video_channel->start();
    }
    pthread_create(&pid_start, nullptr, task_start, this);
}

void FFmpegPlay::start_() {
    while (isPlay) {
        AVPacket *avPacket = av_packet_alloc();
        int ret = av_read_frame(formatContext, avPacket);
        if (!ret) {
            if (video_channel && video_channel->stream_index == avPacket->stream_index) {
//            代表是视频
                video_channel->packes.insertToQueue(avPacket);
            } else if (audio_channel && audio_channel->stream_index == avPacket->stream_index) {
//             代表是音频
//                audio_channel->packes.insertToQueue(avPacket);
            }
        } else if (ret == AVERROR_EOF) {
//          代表读完了

        } else {
//         报错了
            break;
        }
    }
    isPlay = 0;
    video_channel->stop();
    audio_channel->stop();

}

void FFmpegPlay::setRenderCallback(RenderCallback renderCallback) {
    this->renderCallback = renderCallback;
}

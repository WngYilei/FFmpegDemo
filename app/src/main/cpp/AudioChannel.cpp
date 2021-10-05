#include "AudioChannel.h"
AudioChannel::AudioChannel(int streamIndex, AVCodecContext *avCodecContext)
        : BaseChannel(streamIndex, avCodecContext) {

}

void AudioChannel::stop() {

}

AudioChannel::~AudioChannel() = default;

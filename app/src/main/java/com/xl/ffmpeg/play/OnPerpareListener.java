package com.xl.ffmpeg.play;

public interface OnPerpareListener {
    void onPrepare();

    void start();

    void onError(String msg);

    void stop();

    void release();
}

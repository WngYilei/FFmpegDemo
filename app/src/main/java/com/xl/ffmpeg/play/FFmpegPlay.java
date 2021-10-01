package com.xl.ffmpeg.play;

public class FFmpegPlay {

    static {
        System.loadLibrary("ffmpeg");
    }

    private OnPerpareListener listener;

    public FFmpegPlay(OnPerpareListener listener) {
        this.listener = listener;
    }

    private String dataSource;

    public void setDataSource(String dataSource) {
        this.dataSource = dataSource;
    }


    public void prepare() {
        nativePrepare(dataSource);
    }


    public void startPlay() {
        nativeStartPlay();
    }


    public void stop() {
        nativeStop();
    }


    public void release() {
        naticeRelease();
    }


// TODO  >>>>>>>>>>native 放射调用函数 >>>>>>>>>>>>>>

    public void onPrepare() {
        if (listener != null) {
            listener.onPrepare();
        }
    }

    public void onStart() {
        if (listener != null) {
            listener.start();
        }
    }

    public void onStop() {
        if (listener != null) {
            listener.stop();
        }
    }

    public void onRelease() {
        if (listener != null) {
            listener.release();
        }
    }

    public void onError(String msg) {
        if (listener != null) {
            listener.onError(msg);
        }
    }

//  TODO>>>>>>>>>>>>>>>>>>>>>>>以下是native 函数区域>>>>>>>>>>>>>>>>>>>>>>>


    public native String stringFromJNI();

    public native void nativePrepare(String dataSource);

    public native void nativeStartPlay();

    public native void nativeStop();

    public native void naticeRelease();
}

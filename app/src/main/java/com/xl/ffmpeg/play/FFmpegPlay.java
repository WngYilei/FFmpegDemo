package com.xl.ffmpeg.play;

import android.view.Surface;
import android.view.SurfaceHolder;
import android.view.SurfaceView;

import androidx.annotation.NonNull;

public class FFmpegPlay implements SurfaceHolder.Callback {

    static {
        System.loadLibrary("ffmpeg");
    }

    private final OnPerpareListener listener;
    private SurfaceHolder surfaceHolder; // TODO 第三节课新增

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


    public void setSurfaceHolder(SurfaceView surfaceView){
        if (this.surfaceHolder!=null){
            surfaceHolder.removeCallback(this);
        }
        surfaceHolder = surfaceView.getHolder();
        surfaceHolder.addCallback(this); // 监听
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


    public native void setSurfaceNative(Surface surface) ;


    @Override
    public void surfaceCreated(@NonNull SurfaceHolder surfaceHolder) {

    }

    @Override
    public void surfaceChanged(@NonNull SurfaceHolder surfaceHolder, int i, int i1, int i2) {
        setSurfaceNative(surfaceHolder.getSurface());
    }

    @Override
    public void surfaceDestroyed(@NonNull SurfaceHolder surfaceHolder) {

    }
}

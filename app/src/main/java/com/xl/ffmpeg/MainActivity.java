package com.xl.ffmpeg;

import androidx.appcompat.app.AppCompatActivity;
import android.os.Bundle;
import android.widget.Toast;
import com.xl.ffmpeg.databinding.ActivityMainBinding;
import com.xl.ffmpeg.play.FFmpegPlay;
import com.xl.ffmpeg.play.OnPerpareListener;

public class MainActivity extends AppCompatActivity {

    @Override
    protected void onCreate(Bundle savedInstanceState) {
        super.onCreate(savedInstanceState);

        ActivityMainBinding binding = ActivityMainBinding.inflate(getLayoutInflater());
        setContentView(binding.getRoot());
        FFmpegPlay play = new FFmpegPlay(new OnPerpareListener() {
            @Override
            public void onPrepare() {
                showMsg("准备成功");

            }

            @Override
            public void start() {
                showMsg("开始播放");
            }

            @Override
            public void onError(String msg) {
                showMsg(msg);
            }

            @Override
            public void stop() {
                showMsg("播放停止");

            }

            @Override
            public void release() {
                showMsg("释放资源");
            }
        });

        String path = "data/data/com.xl.ffmpeg/demo.mp";
        play.setDataSource(path);
        binding.btnStartPlay.setOnClickListener(view -> play.prepare());
    }

    public void showMsg(String msg) {
        runOnUiThread(() -> Toast.makeText(MainActivity.this, msg, Toast.LENGTH_SHORT).show());
    }

}
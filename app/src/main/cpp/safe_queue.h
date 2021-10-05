#ifndef FFMPEGDEMO_SAFE_QUEUE_H
#define FFMPEGDEMO_SAFE_QUEUE_H

#include <queue>
#include <pthread.h>
#include "util.h"

using namespace std;

template<typename T>  //泛型，存放任意对象
class SafeQueue {
    typedef void (*ReleaseCallBack)(T *); // 函数指针回调，回调给外界 做释放

private:
    queue<T> queue;  // 队列
    pthread_mutex_t mutex;  //互斥锁 安全
    pthread_cond_t condX; // 等待和唤醒
    int work; //队列是否工作
    ReleaseCallBack releaseCallBack;
public:
    SafeQueue() {
        pthread_mutex_init(&mutex, 0); // 初始化互斥锁
        pthread_cond_init(&condX, 0);//初始化环境变量
    }

    ~SafeQueue() {
        pthread_mutex_destroy(&mutex);
        pthread_cond_destroy(&condX);
    }

    void insertToQueue(T value) {
        pthread_mutex_lock(&mutex); //锁住
        if (work) {
            queue.push(value);  //数据塞入队里
            pthread_cond_signal(&condX); // 当插入数据包进入队列后，要通知唤醒
        } else {
//         非工作状态，释放value
            if (releaseCallBack) {
                releaseCallBack(&value); //让外界释放
            }

        }
        pthread_mutex_unlock(&mutex); //锁住
    }

    int getQueueAndDel(T &value) {
        int ret = 0;
        pthread_mutex_lock(&mutex); //锁住
        while (work && queue.empty()) {
            pthread_cond_wait(&condX, &mutex);
        }

        if (!queue.empty()) {
            value = queue.front();
            queue.pop();
            ret = 1;
        }
        pthread_mutex_unlock(&mutex); //锁住
        return ret;
    }

//   设置工作状态
    void setWork(int work) {
        pthread_mutex_lock(&mutex); //锁住
        this->work = work;
        pthread_cond_signal(&condX);
        pthread_mutex_unlock(&mutex); //锁住
    }

    int empty() {
        return queue.empty();
    }

    int size() {
        return queue.size();
    }


    void clear() {
        pthread_mutex_lock(&mutex); //锁住

        unsigned int size = queue.size();
        for (int i = 0; i < size; ++i) {
            T value = queue.front();
            if (releaseCallBack) {
                releaseCallBack(&value);
            }
            queue.pop();

        }


        pthread_mutex_unlock(&mutex); //锁住
    }

    void setReleaseCallback(ReleaseCallBack releaseCallback) {
        this->releaseCallBack = releaseCallback;
    }
};

#endif //FFMPEGDEMO_SAFE_QUEUE_H